#include <stdio.h>
#include <stdlib.h>
#include "Util.h"
#include "Object.h"
#include "Lisp.h"

Object* Lisp_tokenize(Object* string) {
  size_t string_len = Object_len(string);
  Object* ret = QLIST_NEW1();
  size_t i = 0;
  for(i = 0; i < string_len; i++) {
  }
  return ret;
}

Object* Lisp_parse_tokens(Object* tokenlist) {
  return NULL;
}

Object* Lisp_parse_string(Object* string) {
  return NULL;
}
