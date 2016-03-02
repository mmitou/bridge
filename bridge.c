#include "bridge.h"
#include "raw_socket.h"
#include <unistd.h> // close
#include <stdio.h>

int new_fds(const char * const ifnames[], const int length, int *fds) {
  int i = 0;

  for(; i < length; ++i) {
    struct raw_socket_option opt = init_raw_socket(ifnames[i]);
    if(opt.result == raw_socket_success) {
      fds[i] = opt.fd;
    }
    else {
      break;
    }
  }

  return i;
}

bool bridge(const char * const ifnames[], const int length) {
  int fds[length];
  bool result = false;

  const int len_of_fds = new_fds(ifnames, length, fds);
  if(len_of_fds != length) {
    result = false;
    goto ERROR;
  }

  printf("%d %d\n", len_of_fds, fds[0]);

  result = true;
  goto FINALLY;

 ERROR:

 FINALLY:
  for(int i = 0; i < len_of_fds; ++i) {
    close(fds[i]);
  }

  return result;
}
