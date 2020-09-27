
#include <glib-unix.h>

#include "l2cap_con.h"
#include "optparse.h"
#include "listen.h"
#include "channel.h"

static void channel_remove(struct channel_t* channel)
{
  for (int i = 0; i < sizeof(channel->wids) / sizeof(*channel->wids); ++i) {
    if (channel->wids[i])
      g_source_remove(channel->wids[i]);
  }

  close(channel->source_fd);
  close(channel->target_fd);
  channel->parent->channels = g_list_remove(channel->parent->channels, channel);
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

    if (len < 0) {
      g_warning("Read error from %s (PSM 0x%04x)", src_addr, channel->parent->psm);
      channel_remove(channel);
      return FALSE;
    } else if (len > 0) {
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

  channel->cid = cid;
  channel->wids[0] = 0;
  channel->wids[1] = 0;

  channel->source_fd = fd;
  channel->source_addr = addr_src;

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
