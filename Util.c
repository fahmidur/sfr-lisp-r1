#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Util.h"

#ifdef DEBUG
void dbg_printf(char* fmt, ...) {
  va_list argv;
  va_start(argv, fmt);
  vprintf(fmt, argv);
}
#else
void dbg_printf(char* fmt, ...) {
  // do nothing
}
#endif

void Util_indent(int size) {
  int i = 0;
  for(i = 0; i < size; i++) {
    printf("  ");
  }
}

size_t Util_cstr_hash_djb2(char* str) {
  size_t hash = 5381;
  char c;
  while((c = *str++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

void Util_vt_set(int code) {
  if(getenv("NO_COLOR") != NULL) {
    return;
  }
  printf("\033[%dm", code);
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
