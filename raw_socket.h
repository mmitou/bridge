#ifndef RAW_SOCKET_H
#define RAW_SOCKET_H

#include <stdbool.h>

bool init_raw_socket(const char *const ifname, int *fd);

#endif // RAW_SOCKET_H
