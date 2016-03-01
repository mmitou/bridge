#include "bridge.h"
#include "raw_socket.h"
#include <unistd.h> // close
#include <stdio.h>

bool bridge(const char * const ifnames[], const int length) {
  int fds[length];

  for(int i = 0; i < length; ++i) {
    init_raw_socket(ifnames[i], &fds[i]);
    printf("%d\n", fds[i]);
  }

  for(int i = 0; i < length; ++i) {
    close(fds[i]);
  }

  return true;
}
