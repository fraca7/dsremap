
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

  g_io_channel_unref(channel->source);
  g_io_channel_unref(channel->target);
  channel->parent->channels = g_list_remove(channel->parent->channels, channel);
  g_free(channel);
}

static gboolean channel_in_available(GIOChannel* src, GIOChannel* dst, const gchar* dst_addr, GIOCondition cond, struct channel_t* channel)
{
  if (cond & (G_IO_ERR | G_IO_HUP)) {
    g_warning("Connection closed");
    channel_remove(channel);
    return FALSE;
  }

  if (cond & G_IO_IN) {
    guchar buf[4096];
    size_t len = read(g_io_channel_unix_get_fd(src), buf, sizeof(buf));

    if (len < 0) {
      g_warning("Read error (dst %s)", dst_addr);
      channel_remove(channel);
      return FALSE;
    } else if (len > 0) {
      if (l2cap_send(dst_addr, channel->cid, g_io_channel_unix_get_fd(dst), buf, len) < 0) {
        g_warning("Write error (dst %s)", dst_addr);
        channel_remove(channel);
        return FALSE;
      }
    }
  }

  return TRUE;
}

static gboolean channel_source_in_callback(GIOChannel* sock, GIOCondition cond, struct channel_t* channel)
{
  return channel_in_available(channel->source, channel->target, channel->target_addr, cond, channel);
}

static gboolean channel_target_in_callback(GIOChannel* sock, GIOCondition cond, struct channel_t* channel)
{
  return channel_in_available(channel->target, channel->source, channel->source_addr, cond, channel);
}

static gboolean channel_connection_callback(GIOChannel* sock, GIOCondition cond, struct channel_t* channel)
{
  if (cond & (G_IO_ERR | G_IO_HUP)) {
    g_warning("Connection error");
    channel_remove(channel);
    channel_remove(channel);
  } else if (cond & G_IO_OUT) {
    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(g_io_channel_unix_get_fd(sock), SOL_SOCKET, SO_ERROR, (char*)&error, &len) == 0) {
      if (error) {
        g_warning("Cannot connect: %d", error);
        channel_remove(channel);
        channel_remove(channel);
      } else {
        g_info("Connected");

        channel->wids[0] = g_io_add_watch(channel->source, G_IO_IN, (GIOFunc)channel_source_in_callback, channel);
        channel->wids[1] = g_io_add_watch(channel->target, G_IO_IN, (GIOFunc)channel_target_in_callback, channel);
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
  int c_fd;

  channel->cid = cid;
  channel->wids[0] = 0;
  channel->wids[1] = 0;

  channel->source = g_io_channel_unix_new(fd);
  if (!channel->source)
    goto err_0;
  g_io_channel_set_close_on_unref(channel->source, TRUE);
  g_io_channel_set_encoding(channel->source, NULL, NULL);
  channel->source_addr = addr_src;

  c_fd = l2cap_connect(data->context->self_addr, addr_dst, data->psm);
  if (c_fd < 0) {
    g_warning("Cannot connect");
    goto err_1;
  }

  channel->target = g_io_channel_unix_new(c_fd);
  if (!channel->target)
    goto err_2;
  g_io_channel_set_close_on_unref(channel->target, TRUE);
  g_io_channel_set_encoding(channel->target, NULL, NULL);
  channel->target_addr = addr_dst;

  g_io_add_watch(channel->target, G_IO_OUT, (GIOFunc)channel_connection_callback, channel);

  channel->parent = data;
  data->channels = g_list_prepend(data->channels, channel);

  return TRUE;

 err_2:
  close(c_fd);

 err_1:
  g_io_channel_unref(channel->source);

 err_0:
  g_free(channel);

  return FALSE;
}
