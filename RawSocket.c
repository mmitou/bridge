#include <sys/socket.h>      // AF_*, socket
#include <arpa/inet.h>       // htons
#include <stdio.h>           // perror, fprintf, stderr
#include <linux/if_ether.h>  // ETH_P_ALL
#include <unistd.h>          // close
#include <sys/ioctl.h>       // ioctl, SIOCGIFINDEX
#include <net/if.h>          // sturct ifreq, IFNAMSIZ, if_nametoindex
#include <string.h>          // memset, strlen
#include <linux/if_packet.h> // struct sockaddr_ll
#include <stdbool.h>         // bool, true, false
#include "RawSocket.h"

typedef RawSocket_Result result;

result newRawSocket(int *fd) {
  *fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (*fd < 0) {
    return RawSocket_fail_socket;
  }
  return RawSocket_success;
}

result bindInterface(const int fd, const char *const device_name) {
  const unsigned int ifindex = if_nametoindex(device_name);
  if (ifindex < 1) {
    return RawSocket_fail_if_nametoindex;
  }

  // ソケットをインタフェースにバインドする。
  struct sockaddr_ll sa = {.sll_family = AF_PACKET,
                           .sll_protocol = htons(ETH_P_ALL),
                           .sll_ifindex = ifindex};
  if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
    return RawSocket_fail_bind;
  }

  return RawSocket_success;
}

result setPromiscousMode(const int fd, const char *const ifname) {
  // ネットワークデバイスに入ってきたパケットを
  // 無差別(promiscuous)モードで全て見えるようにする。

  struct ifreq ifreq = {.ifr_name = {0}};
  strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);
  if (ioctl(fd, SIOCGIFFLAGS, &ifreq) < 0) {
    return RawSocket_fail_ioctl_SIOCGIFFLAGS;
  }
  ifreq.ifr_flags = ifreq.ifr_flags | IFF_PROMISC;
  if (ioctl(fd, SIOCSIFFLAGS, &ifreq) < 0) {
    return RawSocket_fail_ioctl_SIOCSIFFLAGS;
  }
  return RawSocket_success;
}

struct RawSocket_OptionFileDescriptor
RawSocket_initRawSocket(const char *const ifname) {
  int fd = 0;
  RawSocket_Result result = RawSocket_fail_precondition;
  if (strlen(ifname) > IFNAMSIZ) {
    goto ERROR;
  }

  if ((result = newRawSocket(&fd)) != RawSocket_success) {
    goto ERROR;
  }

  if ((result = bindInterface(fd, ifname)) != RawSocket_success) {
    goto ERROR;
  }

  if ((result = setPromiscousMode(fd, ifname)) != RawSocket_success) {
    goto ERROR;
  }

  return (struct RawSocket_OptionFileDescriptor){RawSocket_success, fd};

ERROR:
  close(fd);
  return (struct RawSocket_OptionFileDescriptor){result, 0};
}
