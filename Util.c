#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void Util_cstr_print(char* cstr) {
  char bchar = 92; // The backslash character
  char wchar = '"';
  printf("%c", wchar);
  size_t str_len = strlen(cstr);
  size_t i;
  char ch;
  for(i = 0; i < str_len; i++) {
    ch = cstr[i];
    if(ch == wchar) {
      putc(bchar, stdout);
    } 
    if(ch == bchar) {
      putc(bchar, stdout);
    }
    putc(ch, stdout);
  }
  printf("%c", wchar);
}

void Util_heading1(char isbeg, char* str) {
  if(isbeg) {
    printf("{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{\n");
  } else {
    printf("}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}\n");
  }
  printf("=== %s. %s \n", (isbeg ? "BEG" : "END"), str);
  if(isbeg) {
    printf("{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{\n");
  } else {
    printf("}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}\n");
    printf("\n\n");
  }
}
