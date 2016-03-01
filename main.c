#include <stdio.h>
#include <unistd.h> // close
#include "raw_socket.h"

int main(int argc, char **argv) {
  (void)argc;
  int fd;
  init_raw_socket(argv[1], &fd);
  printf("%d\n", fd);
  close(fd);

  return 0;
}
