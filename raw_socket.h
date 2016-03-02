#ifndef RAW_SOCKET_H
#define RAW_SOCKET_H

#include <stdbool.h>

typedef enum raw_socket_result {
  raw_socket_success,
  raw_socket_fail_precondition,
  raw_socket_fail_socket,
  raw_socket_fail_if_nametoindex,
  raw_socket_fail_bind,
  raw_socket_fail_ioctl_SIOCGIFFLAGS,
  raw_socket_fail_ioctl_SIOCSIFFLAGS
} RawSocketResult;

struct raw_socket_option {
  RawSocketResult result;
  int fd;
};

struct raw_socket_option init_raw_socket(const char *const ifname);

#endif // RAW_SOCKET_H
