
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

int test_file(const char* path)
{
  struct stat statbuf;
  if ((stat(path, &statbuf)) < 0)
    return errno;
  return 0;
}

int launch_command(const char* cmd)
{
  switch (system(cmd)) {
    case 0:
      break;
    case -1:
      perror("Cannot create child process");
      return -1;
    default:
      perror("Child process failed");
      return -1;
  }

  return 0;
}

int echo_to(const char* path, const char* str)
{
  int fd = open(path, O_WRONLY);
  if (fd < 0) {
    perror("Cannot open file");
    return -1;
  }

  size_t len = strlen(str);
  if (write(fd, str, len) < 0) {
    perror("Cannot write to file");
    close(fd);
    return -1;
  }

  close(fd);
  return 0;
}
