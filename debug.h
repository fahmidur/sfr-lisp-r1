#ifndef _SFR_DEBUG_H
#define _SFR_DEBUG_H

#include <stdio.h>

void debug_print_uint_binary(unsigned int x) {
  while(x > 0) {
    if(x%2 == 1) {
      printf("1");
    } else {
      printf("0");
    }
    x /= 2;
  }
}

#endif

