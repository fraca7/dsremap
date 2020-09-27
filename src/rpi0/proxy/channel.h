
#ifndef _CHANNEL_H
#define _CHANNEL_H

#include <glib.h>
#include <gio/gio.h>
#include <bluetooth/bluetooth.h>

struct channel_t {
  struct listening_socket_data_t* parent;

  unsigned short cid;

  GIOChannel* source;
  const gchar* source_addr;
  GIOChannel* target;
  const gchar* target_addr;

  guint wids[2];
};

gboolean channel_setup(struct listening_socket_data_t*, unsigned short cid, int fd, const gchar* addr_src, const gchar* addr_dst);

#endif
