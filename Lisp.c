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

void print_TokenizerState(TokenizerState x) {
  switch(x) {
    case ts_Init:
      printf("TokenizerState(ts_Init)");
      break;
    case ts_InString:
      printf("TokenizerState(ts_InString)");
      break;
    case ts_InStringEscaped:
      printf("TokenizerState(ts_InStringEscaped)");
      break;
    case ts_InNumber:
      printf("TokenizerState(ts_InNumber)");
      break;
    case ts_InNumberFloat:
      printf("TokenizerState(ts_InNumberFloat)");
      break;
    case ts_InNumberNegMaybe:
      printf("TokenizerState(ts_InNumberNegMaybe)");
      break;
    case ts_InBareWord:
      printf("TokenizerState(ts_InBareWord)");
      break;
    default:
      printf("ERROR");
  }
}

Object* Lisp_tokenize(Object* string) {
  size_t string_len = Object_len(string);
  Object* ret = QLIST_NEW1();

  size_t i = 0;
  TokenizerState state = ts_Init;

  char ch;
  Object* paren_beg = QSYMBOL_NEW1("(");
  Object* paren_end = QSYMBOL_NEW1(")");

  Object* tmp_str = QSTRING_NEW1("");

  for(i = 0; i < string_len; i++) {
    ch = Object_bop_charat(string, i);
    /*printf("Lisp_tokenize. ch=|%c|\n", ch);*/
    printf("\n---\n");
    print_TokenizerState(state); printf("\n");
    ObjectUtil_eprintf("Lisp_tokenizer. ch=|%c| tmp_str=%v\n", ch, tmp_str);
    if(state == ts_Init) {
      /*printf("debug. state=ts_Init\n");*/
      if(ch == '-') {
        state = ts_InNumberNegMaybe;
        /*printf("debug. state=ts_InNumberNegMaybe\n");*/
        /*Object_bop_addx_char(tmp_str, ch);*/
      }
      if(ch == ' ') {
        // eat it
      }
      else
      if(ch == '(') {
        Object_bop_push(ret, paren_beg);
      }
      else
      if(ch == ')') {
        Object_bop_push(ret, paren_end);
      }
      else
      if(TokenizerUtil_isdigit(ch)) {
        state = ts_InNumber;
        /*printf("debug. state=ts_InNumber\n");*/
        Object_bop_addx_char(tmp_str, ch);
      }
      else
      if(TokenizerUtil_wordlike(ch)) {
        state = ts_InBareWord;
        /*printf("debug. state=ts_InBareWord\n");*/
        Object_bop_addx_char(tmp_str, ch);
      }
    }
    else
    if(state == ts_InNumber) {
      if(TokenizerUtil_isdigit(ch)) {
        Object_bop_addx_char(tmp_str, ch);
      }
      else
      if(ch == '.') {
        state = ts_InNumberFloat;
        /*printf("debug. state=ts_InNumberFloat\n");*/
        Object_bop_addx_char(tmp_str, ch);
      }
      else {
        Object_bop_push(ret, Object_to_number(tmp_str));
        Object_zero(tmp_str);
        i--;
        state = ts_Init;
        /*printf("debug. state=ts_Init\n");*/
      }
    }
    else
    if(state == ts_InNumberFloat) {
      if(TokenizerUtil_isdigit(ch)) {
        Object_bop_addx_char(tmp_str, ch);
      }
      else {
        Object_bop_push(ret, Object_to_number(tmp_str));
        Object_zero(tmp_str);
        i--;
        state = ts_Init;
        /*printf("debug. state=ts_Init\n");*/
      }
    }
    else
    if(state == ts_InBareWord) {
      if(TokenizerUtil_wordlike(ch)) {
        Object_bop_addx_char(tmp_str, ch);
      }
      else {
        Object_bop_push(ret, Object_to_symbol(tmp_str));
        Object_zero(tmp_str);
        i--;
        state = ts_Init;
        /*printf("debug. state=ts_Init\n");*/
      }
    }
    else
    if(state == ts_InNumberNegMaybe) {
      if(TokenizerUtil_isdigit(ch)) {
        state = ts_InNumber;
        Object_bop_addx_char(tmp_str, '-');
        Object_bop_addx_char(tmp_str, ch);
      }
      else {
        // the previous '-' will be treated as BareWord
        Object_bop_addx_char(tmp_str, '-');
        i--;
        state = ts_InBareWord;
        /*printf("debug. state=ts_InBareWord\n");*/
      }
    }

    //---
    ObjectUtil_eprintf("Lisp_tokenizer. ch=|%c| tmp_str=%v\n", ch, tmp_str);
  }
  printf("\n---\n"); print_TokenizerState(state); printf("\n---\n");

  if(state == ts_InNumber || state == ts_InNumberFloat) {
    Object_bop_push(ret, Object_to_number(tmp_str));
    Object_zero(tmp_str);
  }

  Object_rc_decr(tmp_str);
  Object_return(ret);
  Object_rc_decr(ret);

  return ret;
}

Object* Lisp_parse_tokens(Object* tokenlist) {
  return NULL;
}

Object* Lisp_parse_string(Object* string) {
  return NULL;
}
