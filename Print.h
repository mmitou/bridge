#ifndef PRINT_H
#define PRINT_H

#include <stdio.h>
#include <net/ethernet.h> // struct ether_header

void Print_fprintEtherHeader(FILE *fp,
                             const struct ether_header *const ether_header);

#endif
