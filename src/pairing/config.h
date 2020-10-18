
#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdint.h>

typedef struct {
  uint8_t btaddr_device[6];
  uint8_t report02[37];
  uint8_t reporta3[49];
  uint8_t report12[16];
} options_t;

/**
 * Parse command line; returns 0 on success
 */
int parse_options(int argc, char* argv[], options_t* opt);

#endif /* _CONFIG_H */
