#include "Router.h"
#include "RawSocket.h"
#include <unistd.h>    // close
#include <sys/epoll.h> // epoll_create, struct epoll_event
#include <stdio.h>    // perror

void closeAll(int fds[], int length) {
  for(int i = 0; i < length; ++i) {
    close(fds[i]);
  }
}

bool Router_router(const char *const ifnames[], const int length) {
  int fds[length];
  int fds_len = 0;
  int epollfd = 0;
  struct epoll_event events[length];

  fds_len = RawSocket_initRawSockets(ifnames, length, fds);
  if (length != fds_len) {
    goto ERROR;
  }

  epollfd = epoll_create(length);
  if(epollfd < 0) {
    perror("epoll_create");
    goto ERROR;
  }

  for (int i = 0; i < length; ++i) {
    struct epoll_event ev = {.events = EPOLLIN, .data.fd = fds[i]};
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fds[i], &ev) < 0) {
      perror("epoll_ctl");
      goto ERROR;
    }
  }

  while (true) {
    int nfds = epoll_wait(epollfd, events, length, -1);
    if (nfds < 0) {
      perror("epoll_wait");
      goto ERROR;
    }

    for (int i = 0; i < nfds; ++i) {
      //(events[i].data.fd, fds, ifnames, length);
    }
  }

  close(epollfd);
  closeAll(fds, length);

ERROR:
  close(epollfd);
  closeAll(fds, length);

  return false;
}
