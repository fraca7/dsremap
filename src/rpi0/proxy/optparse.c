
#include "optparse.h"

#define ARGMASK_HOST 1U
#define ARGMASK_DEV  2U
#define ARGMASK_SELF 4U

gboolean parse_args(struct global_context_t* ctx, int argc, char *argv[])
{
  unsigned int mask = 0U;
  unsigned int state = 0;

  for (unsigned int i = 1; i < argc; ++i) {
    switch (state) {
      case 0:
        if (!strcmp(argv[i], "-h"))
          state = 1;
        else if (!strcmp(argv[i], "-d"))
          state = 2;
        else if (!strcmp(argv[i], "-s"))
          state = 3;
        else {
          g_warning("Unknown option %s", argv[i]);
          return FALSE;
        }
        break;
      case 1:
        if (str2ba(argv[i], &ctx->host_bdaddr) < 0) {
          g_warning("Invalid bdaddr \"%s\"", argv[i]);
          return FALSE;
        }
        mask |= ARGMASK_HOST;
        state = 0;
        break;
      case 2:
        if (str2ba(argv[i], &ctx->device_bdaddr) < 0) {
          g_warning("Invalid bdaddr \"%s\"", argv[i]);
          return FALSE;
        }
        mask |= ARGMASK_DEV;
        state = 0;
        break;
      case 3:
        if (str2ba(argv[i], &ctx->self_bdaddr) < 0) {
          g_warning("Invalid bdaddr \"%s\"", argv[i]);
          return FALSE;
        }
        mask |= ARGMASK_SELF;
        state = 0;
        break;
    }
  }

  switch (state) {
    case 1:
      g_warning("Missing address after -h");
      return FALSE;
    case 2:
      g_warning("Missing address after -d");
      return FALSE;
    case 3:
      g_warning("Missing address after -s");
      return FALSE;
    default:
      break;
  }

  if ((mask & ARGMASK_HOST) == 0) {
    g_warning("Missing host bdaddr");
    return FALSE;
  }

  if ((mask & ARGMASK_DEV) == 0) {
    g_warning("Missing device bdaddr");
    return FALSE;
  }

  if ((mask & ARGMASK_SELF) == 0) {
    g_warning("Missing dongle bdaddr");
    return FALSE;
  }

  ba2str(&ctx->device_bdaddr, ctx->device_addr);
  ba2str(&ctx->host_bdaddr, ctx->host_addr);
  ba2str(&ctx->self_bdaddr, ctx->self_addr);

  return TRUE;
}
