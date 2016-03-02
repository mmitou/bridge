#include "Bridge.h"
#include "RawSocket.h"
#include <unistd.h> // close
#include <stdio.h>

int initRawSockets(const char *const ifnames[], const int length, int *fds) {
  int i = 0;

  for (; i < length; ++i) {
    struct RawSocket_OptionFileDescriptor opt =
        RawSocket_initRawSocket(ifnames[i]);
    if (opt.result == RawSocket_success) {
      fds[i] = opt.fd;
    } else {
      break;
    }
  }

  return i;
}

bool Bridge_bridge(const char *const ifnames[], const int length) {
  int fds[length];
  bool result = false;

  const int len_of_fds = initRawSockets(ifnames, length, fds);
  if (len_of_fds != length) {
    result = false;
    goto ERROR;
  }

  printf("%d %d\n", len_of_fds, fds[0]);

  result = true;
  goto FINALLY;

ERROR:

FINALLY:
  for (int i = 0; i < len_of_fds; ++i) {
    close(fds[i]);
  }

  return result;
}
