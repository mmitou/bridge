#include "bridge.h"
#include "raw_socket.h"
#include <unistd.h> // close
#include <stdio.h>

bool bridge(const char * const ifnames[], const int length) {
  int fds[length];
  bool result = false;

  for(int i = 0; i < length; ++i) {
    if(!init_raw_socket(ifnames[i], &fds[i])) {
      result = false;
      goto FINALLY;
    }
    printf("%d\n", fds[i]);
  }

  result = true;

 FINALLY:
  for(int i = 0; i < length; ++i) {
    close(fds[i]);
  }

  return result;
}
