#ifndef RAWSOCKET_H
#define RAWSOCKET_H

#include <stdbool.h>

typedef enum RawSocket_Result {
  RawSocket_success,
  RawSocket_fail_precondition,
  RawSocket_fail_socket,
  RawSocket_fail_if_nametoindex,
  RawSocket_fail_bind,
  RawSocket_fail_ioctl_SIOCGIFFLAGS,
  RawSocket_fail_ioctl_SIOCSIFFLAGS
} RawSocket_Result;

struct RawSocket_OptionFileDescriptor {
  RawSocket_Result result;
  int fd;
};

struct RawSocket_OptionFileDescriptor
RawSocket_initRawSocket(const char *const ifname);

#endif // RAWSOCKET_H
