#ifndef RAWSOCKET_H
#define RAWSOCKET_H

#include <stdbool.h>

int RawSocket_initRawSockets(const char *const ifnames[], const int length, int *fds);


#endif // RAWSOCKET_H
