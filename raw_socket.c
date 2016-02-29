#include <sys/socket.h>      // AF_*, socket
#include <arpa/inet.h>       // htons
#include <stdio.h>           // perror, fprintf, stderr
#include <linux/if_ether.h>  // ETH_P_ALL
#include <unistd.h>          // close
#include <sys/ioctl.h>       // ioctl, SIOCGIFINDEX
#include <net/if.h>          // sturct ifreq, IFNAMSIZ
#include <string.h>          // memset, strlen
#include <linux/if_packet.h> // struct sockaddr_ll
#include "raw_socket.h"

int init_raw_socket(const char *const device_name) {
  if (strlen(device_name) > IFNAMSIZ) {
    fprintf(stderr, "device_name length is over IFNAMSIZ\n");
    return -1;
  }

  char *error_message = NULL;
  int fd = -1;

  if ((fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
    error_message = "socket";
    goto ERROR;
  }

  // インタフェース名からインタフェースインデックスを取得する。
  struct ifreq ifreq = {.ifr_name = {0}};
  strncpy(ifreq.ifr_name, device_name, strlen(device_name));
  if (ioctl(fd, SIOCGIFINDEX, &ifreq) < 0) {
    error_message = "ioctl SIOCGIFINDEX:";
    goto ERROR;
  }

  // ソケットをインタフェースにバインドする。
  struct sockaddr_ll sa = {.sll_family = AF_PACKET,
                           .sll_protocol = htons(ETH_P_ALL),
                           .sll_ifindex = ifreq.ifr_ifindex};
  if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
    error_message = "bind";
    goto ERROR;
  }

  // ネットワークデバイスに入ってきたパケットを
  // 無差別(promiscuous)モードで全て見えるようにする。
  if (ioctl(fd, SIOCGIFFLAGS, &ifreq) < 0) {
    error_message = "ioctl SIOCGIFFLAGS:";
    goto ERROR;
  }
  ifreq.ifr_flags = ifreq.ifr_flags | IFF_PROMISC;
  if (ioctl(fd, SIOCSIFFLAGS, &ifreq) < 0) {
    error_message = "ioctl SIOCSIFFLAGS:";
    goto ERROR;
  }

  return fd;

ERROR:
  perror(error_message);
  close(fd);

  return -1;
}
