
#include "l2cap_con.h"
#include "listen.h"
#include "optparse.h"
#include "channel.h"

static gboolean listening_socket_callback(GIOChannel* sock, GIOCondition cond, struct listening_socket_data_t* data)
{
  bdaddr_t bdaddr;
  unsigned short psm, cid;

  g_info("New connection on PSM 0x%04x", data->psm);

  int fd = l2cap_accept(g_io_channel_unix_get_fd(sock), &bdaddr, &psm, &cid);
  if (fd < 0) {
    g_warning("accept() error");
    return TRUE;
  }

  if (!bacmp(&data->context->host_bdaddr, &bdaddr)) {
    g_info("Connection from host");
    if (!channel_setup(data, cid, fd, data->context->host_addr, data->context->device_addr)) {
      g_warning("Could not set up channel to device");
      close(fd);
    }
  } else if (!bacmp(&data->context->device_bdaddr, &bdaddr)) {
    g_info("Connection from device");
    if (!channel_setup(data, cid, fd, data->context->device_addr, data->context->host_addr)) {
      g_warning("Could not set up channel to host");
      close(fd);
    }
  } else {
    gchar addr[18];
    ba2str(&bdaddr, addr);

    g_warning("Connection from unknown device %s; closing", addr);
    close(fd);
  }

  return TRUE;
}

gboolean listening_socket_add(struct global_context_t* ctx, unsigned short psm)
{
  int fd = l2cap_listen(psm);
  if (fd < 0)
    return FALSE;

  GIOChannel* sock = g_io_channel_unix_new(fd);
  if (!sock) {
    close(fd);
    return FALSE;
  }

  g_io_channel_set_close_on_unref(sock, TRUE);
  g_io_channel_set_encoding(sock, NULL, NULL);

  struct listening_socket_data_t* data = (struct listening_socket_data_t*)g_malloc(sizeof(struct listening_socket_data_t));
  data->context = ctx;
  data->sock = sock;
  data->psm = psm;
  data->watch_id = g_io_add_watch(sock, G_IO_IN, (GIOFunc)listening_socket_callback, (gpointer)data);

  ctx->listening = g_list_prepend(ctx->listening, data);

  return TRUE;
}

void listening_socket_close(struct listening_socket_data_t* data)
{
  g_io_channel_unref(data->sock);
  g_free(data);
}
