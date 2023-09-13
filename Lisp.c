#include <stdio.h>
#include <stdlib.h>
#include "Util.h"
#include "Object.h"
#include "Lisp.h"

typedef enum TokenizerState TokenizerState;
enum TokenizerState {
  ts_Init,
  ts_InString,
  ts_InStringEscaped,
  ts_InNumber,
  ts_InNumberFloat,
  ts_InNumberNegMaybe,
  ts_InBareWord,
};

char TokenizerUtil_isdigit(char ch) {
  return (ch >= '0' && ch <= '9');
}

char TokenizerUtil_numlike(char ch) {
 return ch == '.' || TokenizerUtil_isdigit(ch);
}

char TokenizerUtil_wordlike(char ch) {
  return (ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122) || (ch == '_') || (ch == '-') || (ch == '+');
}

char TokenizerUtil_min_uint(unsigned int a, unsigned int b) {
  return (a < b) ? a : b;
}

Object* Lisp_tokenize(Object* string) {
  size_t string_len = Object_len(string);
  Object* ret = QLIST_NEW1();

  size_t i = 0;
  TokenizerState state = ts_Init;

  char ch;
  Object* paren_beg = QSYMBOL_NEW1("(");
  Object* paren_end = QSYMBOL_NEW1(")");

  Object* tmp = NULL;

  for(i = 0; i < string_len; i++) {
    ch = Object_bop_charat(string, i);
    printf("Lisp_tokenize. ch=%c\n", ch);
    if(state == ts_Init) {
      if(ch == '(') {
        Object_bop_push(ret, paren_beg);
      }
      else
      if(ch == '(') {
        Object_bop_push(ret, paren_end);
      }
      else
      if(TokenizerUtil_isdigit(ch)) {
        state = ts_InNumber;
        // TODO
      }
    }
  }

  return ret;
}

Object* Lisp_parse_tokens(Object* tokenlist) {
  return NULL;
}

Object* Lisp_parse_string(Object* string) {
  return NULL;
}
