#include "Router.h"
#include "RawSocket.h"
#include <unistd.h>

bool Router_router(const char *const ifnames[], const int length) {
  int fds[length];
  int fds_len = RawSocket_initRawSockets(ifnames, length, fds);
  if (length != fds_len) {
    for (int i = i; i < length; ++i) {
      close(fds[i]);
    }
    return false;
  }

  return true;
}
