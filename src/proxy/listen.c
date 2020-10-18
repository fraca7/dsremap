
#include <glib-unix.h>

#include "l2cap_con.h"
#include "listen.h"
#include "optparse.h"
#include "channel.h"

static gboolean listening_socket_callback(gint fd, GIOCondition cond, struct listening_socket_data_t* data)
{
  bdaddr_t bdaddr;
  unsigned short psm, cid;

  int afd = l2cap_accept(fd, &bdaddr, &psm, &cid);
  if (afd < 0) {
    g_warning("accept() error (PSM 0x%04x)", data->psm);
    return TRUE;
  }

  if (!bacmp(&data->context->host_bdaddr, &bdaddr)) {
    g_info("Connection from host (PSM 0x%04x)", data->psm);
    if (!channel_setup(data, cid, afd, data->context->host_addr, data->context->device_addr)) {
      g_warning("Could not set up channel to device");
      close(afd);
    }
  } else if (!bacmp(&data->context->device_bdaddr, &bdaddr)) {
    g_info("Connection from device (PSM 0x%04x)", data->psm);
    if (!channel_setup(data, cid, afd, data->context->device_addr, data->context->host_addr)) {
      g_warning("Could not set up channel to host");
      close(afd);
    }
  } else {
    gchar addr[18];
    ba2str(&bdaddr, addr);

    g_warning("Connection from unknown device %s (PSM 0x%04x); closing", addr, data->psm);
    close(afd);
  }

  return TRUE;
}

gboolean listening_socket_add(struct global_context_t* ctx, unsigned short psm)
{
  int fd = l2cap_listen(psm);
  if (fd < 0)
    return FALSE;

  struct listening_socket_data_t* data = (struct listening_socket_data_t*)g_malloc(sizeof(struct listening_socket_data_t));
  data->context = ctx;
  data->fd = fd;
  data->psm = psm;
  data->watch_id = g_unix_fd_add(fd, G_IO_IN, (GUnixFDSourceFunc)listening_socket_callback, (gpointer)data);

  ctx->listening = g_list_prepend(ctx->listening, data);

  return TRUE;
}

void listening_socket_close(struct listening_socket_data_t* data)
{
  g_source_remove(data->watch_id);
  close(data->fd);
  g_free(data);
}
