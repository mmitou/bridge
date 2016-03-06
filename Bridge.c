#include "Bridge.h"
#include "RawSocket.h"
#include "Print.h"
#include <unistd.h> // close, write, read
#include <stdio.h>
#include <sys/epoll.h> // epoll_create, struct epoll_event

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

bool sendPacket(const int srcfd, const int *const fds,
                 const char *const ifnames[], const int length) {
  char buf[BUFSIZ];
  const ssize_t packet_size = read(srcfd, buf, sizeof(buf));
  if (packet_size < 0) {
    perror("read");
    return false;
  }

  for (int i = 0; i < length; ++i) {
    if (srcfd != fds[i]) {
      printf("from %d, write %d(%s)\n", srcfd, fds[i], ifnames[i]);
      Print_fprintEtherHeader(stdout, (const struct ether_header* const)buf);
      // write(fds[i], buf, packet_size);
      // perror(ifnames[i]);
    }
  }

  return true;
}

enum BridgeResult {
  BridgeResult_success,
  BridgeResult_fail_precondition,
  BridgeResult_fail_epol_create,
  BridgeResult_fail_epol_ctl,
  BridgeResult_fail_epoll_wait
};

enum BridgeResult bridge(const int *const fds, const char *const ifnames[],
                         const int length) {
  if (fds == NULL || length < 1) {
    return BridgeResult_fail_precondition;
  }

  const int epollfd = epoll_create(length);

  if (epollfd < 0) {
    perror("epoll_create");
    return BridgeResult_fail_epol_create;
  }

  for (int i = 0; i < length; ++i) {
    struct epoll_event ev = {.events = EPOLLIN, .data.fd = fds[i]};
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fds[i], &ev) < 0) {
      perror("epoll_ctl");
      close(epollfd);
      return BridgeResult_fail_epol_ctl;
    }
  }

  struct epoll_event events[length];
  while (true) {
    int nfds = epoll_wait(epollfd, events, length, -1);
    if (nfds < 0) {
      perror("epoll_wait");
      return BridgeResult_fail_epoll_wait;
    }

    for (int i = 0; i < nfds; ++i) {
      sendPacket(events[i].data.fd, fds, ifnames, length);
    }
  }

  close(epollfd);

  return BridgeResult_success;
}

bool Bridge_bridge(const char *const ifnames[], const int length) {
  if (length < 1) {
    return false;
  }

  int fds[length];
  bool result = false;

  const int len_of_fds = initRawSockets(ifnames, length, fds);
  if (len_of_fds != length) {
    result = false;
    goto ERROR;
  }

  bridge(fds, ifnames, len_of_fds);

  result = true;
  goto FINALLY;

ERROR:

FINALLY:
  for (int i = 0; i < len_of_fds; ++i) {
    close(fds[i]);
  }

  return result;
}
