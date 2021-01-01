
#include <glib.h>
#include <glib-unix.h>

#include "optparse.h"
#include "listen.h"

gboolean on_signal(GMainLoop* loop)
{
  g_main_loop_quit(loop);

  return FALSE;
}

static unsigned short psms[] = {
  0x0001, // SDP
  0x0011, // HID Control
  0x0013, // HID Interrupt
};

int main(int argc, char *argv[])
{
  int ret = 1;
  struct global_context_t context;
  GMainLoop* loop;

  context.listening = NULL;
  if (!parse_args(&context, argc, argv))
    goto err_0;

  loop = g_main_loop_new(NULL, FALSE);
  g_unix_signal_add(SIGINT, (GSourceFunc)on_signal, loop);

  for (int i = 0; i < sizeof(psms) / sizeof(*psms); ++i) {
    if (!listening_socket_add(&context, psms[i]))
      goto err_1;
  }

  g_main_loop_run(loop);

  ret = 0;

 err_1:
  g_list_free_full(context.listening, (GDestroyNotify)listening_socket_close);

 err_0:

  return ret;
}
