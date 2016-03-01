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

bool new_raw_socket(int *fd, char **error_message) {
  if ((*fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
    *error_message = "socket";
    *fd = 0;
    return false;
  }
  return true;
}

bool bind_interface(const int fd, const char *const device_name,
                    char **error_message) {
  // インタフェース名からインタフェースインデックスを取得する。
  const unsigned int ifindex = if_nametoindex(device_name);
  if (ifindex < 1) {
    *error_message = "if_nametoindex";
    return false;
  }

  // ソケットをインタフェースにバインドする。
  struct sockaddr_ll sa = {.sll_family = AF_PACKET,
                           .sll_protocol = htons(ETH_P_ALL),
                           .sll_ifindex = ifindex};
  if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
    *error_message = "bind";
    return false;
  }

  return true;
}

bool set_promiscous_mode(const int fd, const char *const ifname, char **error_message) {
  // ネットワークデバイスに入ってきたパケットを
  // 無差別(promiscuous)モードで全て見えるようにする。

  struct ifreq ifreq = {.ifr_name = {0}};
  strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);
  if (ioctl(fd, SIOCGIFFLAGS, &ifreq) < 0) {
    *error_message = "ioctl SIOCGIFFLAGS";
    return false;
  }
  ifreq.ifr_flags = ifreq.ifr_flags | IFF_PROMISC;
  if (ioctl(fd, SIOCSIFFLAGS, &ifreq) < 0) {
    *error_message = "ioctl SIOCSIFFLAGS";
    return false;
  }
  return true;
}

bool init_raw_socket(const char *const ifname, int *fd) {
  if (strlen(ifname) > IFNAMSIZ) {
    goto PRECONDITION_ERROR;
  }

  char *error_message = NULL;

  if (!new_raw_socket(fd, &error_message)) {
    goto ERROR;
  }

  if (!bind_interface(*fd, ifname, &error_message)) {
    goto ERROR;
  }

  if (!set_promiscous_mode(*fd, ifname, &error_message)) {
    goto ERROR;
  }

  return true;

 ERROR:
  perror(error_message);
  close(*fd);
  return false;

 PRECONDITION_ERROR:
  fprintf(stderr, "device_name length is over IFNAMSIZ\n");
  return false;
}
