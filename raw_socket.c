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
#include "raw_socket.h"

int new_raw_socket(int *fd) {
  *fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if(*fd < 0) {
    return raw_socket_fail_socket;
  }
  return raw_socket_success;
}

enum raw_socket_result bind_interface(const int fd, const char *const device_name) {
  // インタフェース名からインタフェースインデックスを取得する。
  const unsigned int ifindex = if_nametoindex(device_name);
  if (ifindex < 1) {
    return raw_socket_fail_if_nametoindex;
  }

  // ソケットをインタフェースにバインドする。
  struct sockaddr_ll sa = {.sll_family = AF_PACKET,
                           .sll_protocol = htons(ETH_P_ALL),
                           .sll_ifindex = ifindex};
  if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
    return raw_socket_fail_bind;
  }

  return raw_socket_success;
}

bool set_promiscous_mode(const int fd, const char *const ifname) {
  // ネットワークデバイスに入ってきたパケットを
  // 無差別(promiscuous)モードで全て見えるようにする。

  struct ifreq ifreq = {.ifr_name = {0}};
  strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);
  if (ioctl(fd, SIOCGIFFLAGS, &ifreq) < 0) {
    return raw_socket_fail_ioctl_SIOCGIFFLAGS;
  }
  ifreq.ifr_flags = ifreq.ifr_flags | IFF_PROMISC;
  if (ioctl(fd, SIOCSIFFLAGS, &ifreq) < 0) {
    return raw_socket_fail_ioctl_SIOCSIFFLAGS;
  }
  return raw_socket_success;
}

struct raw_socket_option init_raw_socket(const char *const ifname) {
  int fd = 0;
  RawSocketResult result = raw_socket_fail_precondition;
  if (strlen(ifname) > IFNAMSIZ) {
    goto ERROR;
  }

  if ((result = new_raw_socket(&fd)) != raw_socket_success) {
    goto ERROR;
  }

  if ((result = bind_interface(fd, ifname)) != raw_socket_success) {
    goto ERROR;
  }

  if ((result = set_promiscous_mode(fd, ifname)) != raw_socket_success) {
    goto ERROR;
  }

  return (struct raw_socket_option) {raw_socket_success, fd};

ERROR:
  close(fd);
  return (struct raw_socket_option) {result, 0};
}
