#include <stdio.h>
#include <stdlib.h>
#include "Util.h"

size_t Util_cstr_hash_djb2(char* str) {
  size_t hash = 5381;
  char c;
  while((c = *str++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

size_t Util_sizet_max(size_t a, size_t b) {
  if(a > b) {
    return a;
  }
  return b;
}
