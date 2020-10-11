
#include <glib-unix.h>
#include <zlib.h>

#include "l2cap_con.h"
#include "optparse.h"
#include "listen.h"
#include "channel.h"
#include "imu.h"
#include "vm.h"

static void channel_remove(struct channel_t* channel)
{
  for (int i = 0; i < sizeof(channel->wids) / sizeof(*channel->wids); ++i) {
    if (channel->wids[i])
      g_source_remove(channel->wids[i]);
  }

  close(channel->source_fd);
  close(channel->target_fd);
  channel->parent->channels = g_list_remove(channel->parent->channels, channel);

  if (channel->vm)
    vm_free(channel->vm);
  if (channel->imu)
    imu_free(channel->imu);

  g_free(channel);
}

static gboolean channel_in_available(gint src_fd, gint dst_fd, const gchar* src_addr, const gchar* dst_addr, GIOCondition cond, struct channel_t* channel)
{
  if (cond & G_IO_HUP) {
    g_info("Peer %s has closed connection (PSM 0x%04x)", src_addr, channel->parent->psm);
    channel_remove(channel);
    return FALSE;
  }

  if (cond & G_IO_ERR) {
    g_warning("Poll error from %s (PSM 0x%04x)", src_addr, channel->parent->psm);
    channel_remove(channel);
    return FALSE;
  }

  if (cond & G_IO_IN) {
    guchar buf[4096];
    size_t len = read(src_fd, buf, sizeof(buf));

    if (channel->parent->psm == 0x0013) {
    }

    if (len < 0) {
      g_warning("Read error from %s (PSM 0x%04x)", src_addr, channel->parent->psm);
      channel_remove(channel);
      return FALSE;
    } else if (len > 0) {
      switch (channel->parent->psm) {
        case 0x0011:
          if ((buf[0] == 0xa3) && (buf[1] == 0x02) && (len == 38)) {
            // Feature report 0x02 (IMU calibration data). Find the interrupt channel
            gboolean found = FALSE;
            for (GList* listener_item = g_list_first(channel->parent->context->listening); (listener_item != NULL) && !found; listener_item = g_list_next(listener_item)) {
              struct listening_socket_data_t* listener = (struct listening_socket_data_t*)listener_item->data;
              if (listener->psm == 0x0013) {
                for (GList* channel_item = g_list_first(listener->channels); (channel_item != NULL) && !found; channel_item = g_list_next(channel_item)) {
                  struct channel_t* other = (struct channel_t*)channel_item->data;
                  if (!strcmp(channel->source_addr, other->source_addr) && !strcmp(channel->target_addr, other->target_addr)) {
                    found = TRUE;
                    if (other->vm) {
                      g_info("Setting IMU calibration data");
                      other->imu = imu_init();
                      imu_set_calibration_data(other->imu, (CalibrationData_t*)(buf + 2));
                    }
                  }
                }
              }
            }
          }
          break;
        case 0x0013:
          if ((buf[0] == 0xa1) && (buf[1] == 0x11) && channel->vm && channel->imu && (len == 79)) {
            BTReport11_t* report = (BTReport11_t*)(buf + 1);

            // Always update IMU even for skipped reports
            imu_update(channel->imu, report);

            // The DualShock sends so much reports the BT device does not follow. Skip some.
            static int counter = 0;
            if ((++counter % 2) == 0)
              return TRUE;

            vm_run(channel->vm, report, channel->imu);

            uint32_t crc = crc32(0x00000000, buf, 75);
            *((uint32_t*)(buf + 75)) = crc;
          }
          break;
      }

      if (l2cap_send(dst_addr, channel->cid, dst_fd, buf, len) < 0) {
        g_warning("Write error to %s (PSM 0x%04x)", dst_addr, channel->parent->psm);
        channel_remove(channel);
        return FALSE;
      }
    }
  }

  return TRUE;
}

static gboolean channel_source_in_callback(gint fd, GIOCondition cond, struct channel_t* channel)
{
  return channel_in_available(channel->source_fd, channel->target_fd, channel->source_addr, channel->target_addr, cond, channel);
}

static gboolean channel_target_in_callback(gint fd, GIOCondition cond, struct channel_t* channel)
{
  return channel_in_available(channel->target_fd, channel->source_fd, channel->target_addr, channel->source_addr, cond, channel);
}

static gboolean channel_connection_callback(gint fd, GIOCondition cond, struct channel_t* channel)
{
  if (cond & (G_IO_ERR | G_IO_HUP)) {
    g_warning("Connection error (PSM 0x%04x)", channel->parent->psm);
    channel_remove(channel);
  } else if (cond & G_IO_OUT) {
    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (char*)&error, &len) == 0) {
      if (error) {
        g_warning("Cannot connect (PSM 0x%04x): %d", channel->parent->psm, error);
        channel_remove(channel);
      } else {
        g_info("Connected (PSM 0x%04x)", channel->parent->psm);

        channel->wids[0] = g_unix_fd_add(channel->source_fd, G_IO_IN, (GUnixFDSourceFunc)channel_source_in_callback, channel);
        channel->wids[1] = g_unix_fd_add(channel->target_fd, G_IO_IN, (GUnixFDSourceFunc)channel_target_in_callback, channel);
      }
    } else {
      g_warning("Cannot get socket status");
    }
  }

  return FALSE;
}

gboolean channel_setup(struct listening_socket_data_t* data, unsigned short cid, int fd, const gchar* addr_src, const gchar* addr_dst)
{
  struct channel_t* channel = (struct channel_t*)g_malloc(sizeof(struct channel_t));
  gint c_fd;

  channel->imu = NULL;
  channel->vm = NULL;

  channel->cid = cid;
  channel->wids[0] = 0;
  channel->wids[1] = 0;

  channel->source_fd = fd;
  channel->source_addr = addr_src;

  if ((data->psm == 0x0013) && data->context->bytecode_file)
    channel->vm = vm_init_from_file(data->context->bytecode_file);

  c_fd = l2cap_connect(data->context->self_addr, addr_dst, data->psm);
  if (c_fd < 0) {
    g_warning("Cannot connect (PSM 0x%04x)", data->psm);
    goto err_0;
  }

  channel->target_fd = c_fd;
  channel->target_addr = addr_dst;

  g_unix_fd_add(c_fd, G_IO_OUT, (GUnixFDSourceFunc)channel_connection_callback, channel);

  channel->parent = data;
  data->channels = g_list_prepend(data->channels, channel);

  return TRUE;

 err_0:
  close(fd);
  g_free(channel);

  return FALSE;
}
