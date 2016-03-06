#include "Print.h"
#include <inttypes.h>  // PRIx8
#include <arpa/inet.h> // ntohs

void Print_fprintEtherHeader(FILE *fp,
                             const struct ether_header *const header) {
  fprintf(fp, "Ether Header\n");

  fprintf(fp, "src: "
              "%02" PRIx8 ":%02" PRIx8 ":%02" PRIx8 ":%02" PRIx8 ":%02" PRIx8
              ":%02" PRIx8 "\n",
          header->ether_dhost[0], header->ether_dhost[1],
          header->ether_dhost[2], header->ether_dhost[3],
          header->ether_dhost[4], header->ether_dhost[5]);

  fprintf(fp, "dst: "
              "%02" PRIx8 ":%02" PRIx8 ":%02" PRIx8 ":%02" PRIx8 ":%02" PRIx8
              ":%02" PRIx8 "\n",
          header->ether_shost[0], header->ether_shost[1],
          header->ether_shost[2], header->ether_shost[3],
          header->ether_shost[4], header->ether_shost[5]);

  fprintf(fp, "type: %02x ", ntohs(header->ether_type));
  switch (ntohs(header->ether_type)) {
  case ETH_P_IP:
    fprintf(fp, "IP\n");
    break;
  case ETH_P_IPV6:
    fprintf(fp, "IPv6\n");
    break;
  case ETH_P_ARP:
    fprintf(fp, "ARP\n");
    break;
  // case ETH_P_REVARP: fprintf(fp, "RARP\n");    break;
  default:
    fprintf(fp, "UNKNOWN\n");
    break;
  }
}
