
#ifndef _LISTEN_H
#define _LISTEN_H

#include <glib.h>
#include <gio/gio.h>

struct global_context_t;

struct listening_socket_data_t {
  struct global_context_t* context;

  GIOChannel* sock;
  unsigned short psm;
  guint watch_id;

  GList* channels;
};

gboolean listening_socket_add(struct global_context_t* ctx, unsigned short psm);
void     listening_socket_close(struct listening_socket_data_t* data);

#endif
