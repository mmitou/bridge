#include "bridge.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  if(argc < 2) {
    fprintf(stderr, "arg error\n");
    return 1;
  }

  if(!bridge((const char **const)(&argv[1]), argc -1)) {
    return 1;
  }

  return 0;
}
