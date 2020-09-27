
#ifndef _CHANNEL_H
#define _CHANNEL_H

#include <glib.h>
#include <bluetooth/bluetooth.h>

struct channel_t {
  struct listening_socket_data_t* parent;

  unsigned short cid;

  gint source_fd;
  const gchar* source_addr;
  gint target_fd;
  const gchar* target_addr;

  guint wids[2];
};

gboolean channel_setup(struct listening_socket_data_t*, unsigned short cid, int fd, const gchar* addr_src, const gchar* addr_dst);

#endif
