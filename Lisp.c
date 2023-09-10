#include <stdio.h>
#include <stdlib.h>
#include "Util.h"
#include "Object.h"
#include "Lisp.h"

Object* Lisp_tokenize(Object* string) {
  size_t string_len = Object_len(string);
  Object* ret = QLIST_NEW1();
  size_t i = 0;
  char ch;
  for(i = 0; i < string_len; i++) {
    ch = Object_bop_charat(string, i);
    printf("Lisp_tokenize. ch=%c\n", ch);
  }
  return ret;
}

Object* Lisp_parse_tokens(Object* tokenlist) {
  return NULL;
}

Object* Lisp_parse_string(Object* string) {
  return NULL;
}
