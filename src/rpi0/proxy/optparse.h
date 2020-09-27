
#ifndef _OPTPARSE_H
#define _OPTPARSE_H

#include <glib.h>
#include <bluetooth/bluetooth.h>

struct global_context_t {
  bdaddr_t self_bdaddr;
  gchar self_addr[18];

  bdaddr_t host_bdaddr;
  gchar host_addr[18];

  bdaddr_t device_bdaddr;
  char device_addr[18];

  GList* listening;
};

gboolean parse_args(struct global_context_t* ctx, int argc, char *argv[]);

#endif
