
#include <stdio.h>
#include <string.h>

#include "config.h"

static const uint8_t report_02[] = {
  0x02, 0xf7, 0xff, 0x0a, 0x00, 0xef, 0xff, 0xb9, 0x22, 0x33, 0xdd, 0xd2, 0x22, 0x43, 0xdd, 0xb4,
  0x22, 0x27, 0xdd, 0x1c, 0x02, 0x1c, 0x02, 0xa9, 0x1f, 0xc4, 0xe0, 0x9d, 0x20, 0x70, 0xe0, 0xa7,
  0x20, 0x7b, 0xdf, 0x04, 0x00
};

static const uint8_t report_a3[] = {
  0xa3, 0x4a, 0x75, 0x6e, 0x20, 0x20, 0x39, 0x20, 0x32, 0x30, 0x31, 0x37, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x31, 0x32, 0x3a, 0x33, 0x36, 0x3a, 0x34, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x01, 0x08, 0xb4, 0x01, 0x00, 0x00, 0x00, 0x07, 0xa0, 0x10, 0x20, 0x00, 0xa0, 0x02,
  0x00
};

void usage(const char* name)
{
  fprintf(stderr, "Usage: %s [options]\n", name);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -d <device btaddr> BT address of the device\n");
  fprintf(stderr, "  -3 <data>          0xa3 report data\n");
  fprintf(stderr, "  -2 <data>          0x02 report data\n");
}

int parse_btaddr(const char* addr, uint8_t* bf)
{
  unsigned a, b, c, d, e, f;
  if (sscanf(addr, "%02x:%02x:%02x:%02x:%02x:%02x", &a, &b, &c, &d, &e, &f) != 6) {
    fprintf(stderr, "Invalid btaddr \"%s\"; must be xx:xx:xx:xx:xx:xx\n", addr);
    return -1;
  }

  bf[0] = f;
  bf[1] = e;
  bf[2] = d;
  bf[3] = c;
  bf[4] = b;
  bf[5] = a;

  return 0;
}

int parse_data(const char* data, uint8_t* bf, int len)
{
  if (strlen(data) != len * 2) {
    fprintf(stderr, "Invalid data (wrong length) \"%s\"\n", data);
    return -1;
  }

  for (int i = 0; i < len; ++i) {
    unsigned v;
    if (sscanf(data + 2 * i, "%02X", &v) != 1) {
      fprintf(stderr, "Invalid byte \"%s\"\n", data + 2 * i);
      return -1;
    }
    bf[i] = v;
  }

  return 0;
}

#define MASK_DEVICEADDR 1U

int parse_options(int argc, char* argv[], options_t* opt)
{
  memset(opt, 0, sizeof(*opt));
  memcpy(opt->report02, report_02, sizeof(report_02));
  memcpy(opt->reporta3, report_a3, sizeof(report_a3));

  int state = 0;
  unsigned mask = 0U;
  for (int i = 1; i < argc; ++i) {
    switch (state) {
      case 0:
        if ((argv[i][0] != '-') || (strlen(argv[i]) != 2)) {
          fprintf(stderr, "Invalid argument \"%s\"\n", argv[i]);
          usage(argv[0]);
          return -1;
        }

        switch (argv[i][1]) {
          case 'd':
            state = 1;
            break;
          case '3':
            state = 2;
            break;
          case '2':
            state = 3;
            break;
        }

        break;
      case 1:
        if (parse_btaddr(argv[i], opt->btaddr_device) < 0)
          return -1;
        mask |= MASK_DEVICEADDR;
        state = 0;
        break;
      case 2:
        if (parse_data(argv[i], opt->reporta3, sizeof(opt->reporta3)) < 0)
          return -1;
        state = 0;
        break;
      case 3:
        if (parse_data(argv[i], opt->report02, sizeof(opt->report02)) < 0)
          return -1;
        state = 0;
        break;
    }
  }

  switch (state) {
    case 1:
      fprintf(stderr, "Missing btaddr after -d\n");
      break;
    case 2:
      fprintf(stderr, "Missing data after -3\n");
      break;
    case 3:
      fprintf(stderr, "Missing data after -2\n");
      break;
    default:
      if ((mask & MASK_DEVICEADDR) == 0) {
        fprintf(stderr, "-d is mandatory\n");
        usage(argv[0]);
        return -1;
      }

      opt->report12[0] = 0x12;
      memcpy(&opt->report12[1], opt->btaddr_device, sizeof(opt->btaddr_device));
      opt->report12[7] = 0x08;
      opt->report12[8] = 0x25;
      opt->report12[9] = 0x00;

      return 0;
  }

  usage(argv[0]);
  return -1;
}
