#include <stdio.h>
#include <stdlib.h>
#include "Util.h"
#include "Object.h"
#include "Lisp.h"

Object* LISP_PAREN_BEG;
Object* LISP_PAREN_END;

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

void Lisp_init() {
  LISP_PAREN_BEG = QSYMBOL_NEW1("(");
  LISP_PAREN_END = QSYMBOL_NEW1(")");
}

void Lisp_done() {
  Object_assign(&LISP_PAREN_BEG, NULL);
  Object_assign(&LISP_PAREN_END, NULL);
}

char TokenizerUtil_isdigit(char ch) {
  return (ch >= '0' && ch <= '9');
}

char TokenizerUtil_numlike(char ch) {
 return ch == '.' || TokenizerUtil_isdigit(ch);
}

char TokenizerUtil_wordlike(char ch) {
  return (
    (ch >= 65 && ch <= 90) || 
    (ch >= 97 && ch <= 122) || 
    (ch == '_') || 
    (ch == '-') || 
    (ch == '+') || 
    (ch == '*') ||
    (ch == '/')
  );
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

Object* Lisp_parse(Object* tokens) {
  assert(tokens != NULL);
  Object_rc_incr(tokens);
  // TODO
  Object_rc_decr(tokens);
  return NULL;
}

Object* Lisp_tokenize(Object* string) {
  assert(string != NULL);
  Object_rc_incr(string);
  assert(Object_type(string) == SYMBOL_STRING);

  size_t string_len = Object_len(string);
  Object* ret = QLIST_NEW1();
  printf("string_len = %ld\n", string_len);

  size_t i = 0;
  TokenizerState state = ts_Init;

  char ch;
  Object* paren_beg = QSYMBOL_NEW1("(");
  Object* paren_end = QSYMBOL_NEW1(")");

  Object* tmp_str = QSTRING_NEW1("");
  ObjectUtil_eprintf("tmp_str=%v\n", tmp_str);

  printf("--- beg-of-forloop ---\n"); 
  for(i = 0; i < string_len; i++) {
    ch = Object_bop_charat(string, i);
    printf("---\n");
    printf("state="); print_TokenizerState(state); printf("\n");
    ObjectUtil_eprintf("Lisp_tokenizer. ch=|%c| tmp_str=%v\n", ch, tmp_str);
    if(state == ts_Init) {
      if(ch == '-') {
        state = ts_InNumberNegMaybe;
      }
      else
      if(ch == ' ') {
        // eat it
      }
      else
      if(ch == '"') {
        state = ts_InString;
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
        Object_bop_addx_char(tmp_str, ch);
      }
      else
      if(TokenizerUtil_wordlike(ch)) {
        state = ts_InBareWord;
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
        Object_bop_addx_char(tmp_str, ch);
      }
      else {
        Object_bop_push(ret, Object_to_number(tmp_str));
        Object_zero(tmp_str);
        i--;
        state = ts_Init;
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
      }
    }
    else
    if(state == ts_InNumberNegMaybe) {
      if(TokenizerUtil_isdigit(ch)) {
        Object_bop_addx_char(tmp_str, '-');
        i--;
        state = ts_InNumber;
        // now we handle the rest as we would any other number.
      }
      else {
        // the previous '-' will be treated as BareWord
        Object_bop_addx_char(tmp_str, '-');
        i--;
        state = ts_InBareWord;
      }
    }
    else
    if(state == ts_InString) {
      if(ch == '\\') {
        state = ts_InStringEscaped;
      }
      else
      if(ch == '"') {
        Object_bop_push(ret, Object_clone(tmp_str));
        Object_zero(tmp_str);
        state = ts_Init;
      }
      else {
        Object_bop_addx_char(tmp_str, ch);
      }
    }
    else
    if(state == ts_InStringEscaped) {
      Object_bop_addx_char(tmp_str, ch);
      state = ts_InString;
    }
    //--- end of forloop-body
    /*ObjectUtil_eprintf("Lisp_tokenizer. ch=|%c| tmp_str=%v\n", ch, tmp_str);*/
    /*printf("state="); print_TokenizerState(state); printf("\n");*/
  }
  /*printf("--- end-of-forloop ---\n"); */

  /*printf("state="); print_TokenizerState(state); printf("\n");*/
  /*ObjectUtil_eprintf("tmp_str=%v\n", tmp_str);*/
  if(state == ts_InNumber) {
    Object_bop_push(ret, Object_to_number(tmp_str));
    Object_zero(tmp_str);
  }
  else
  if(state == ts_InNumberFloat) {
    Object_bop_push(ret, Object_to_number(tmp_str));
    Object_zero(tmp_str);
  }
  /*ObjectUtil_eprintf("tmp_str=%v\n", tmp_str);*/
  /*printf("---\n");*/

  Object_rc_decr(string);
  Object_rc_decr(tmp_str);
  Object_return(ret);
  Object_rc_decr(ret);

  return ret;
}

Object* Lisp_parse_tokens2(Object* tokenlist, int depth) {
  Object* tmp = Object_new_null();
  Object* ret = QLIST_NEW1();
  Object* sublist = Object_new_null();
  int idx = 0;
  char softbreak = 0;
  while(Object_len(tokenlist) > 0) {
    tmp = Object_accept(Object_uop_shift(tokenlist));
    ObjectUtil_eprintf("[%d] Lisp_parse_tokens2. tmp = %v\n", depth, tmp);
    if(depth == 0 && idx == 0 && Object_cmp(tmp, LISP_PAREN_BEG) == 0) {
      // special case of first '(' in the tokenlist
    }
    else
    if(Object_cmp(tmp, LISP_PAREN_BEG) == 0) {
      // create a sublist to append to ret
      sublist = Object_accept(Lisp_parse_tokens2(tokenlist, depth+1));
      Object_bop_push(ret, sublist);
      Object_assign(&sublist, NULL);
    }
    else
    if(Object_cmp(tmp, LISP_PAREN_END) == 0) {
      softbreak = 1;
    }
    else {
      // some sort of atom
      Object_bop_push(ret, tmp);
    }
    idx++;
    if(softbreak) {
      // cleanup
      Object_assign(&tmp, NULL);
      break;
    }
  }
  //cleanup
  Object_assign(&sublist, NULL);
  if(depth == 0 && Object_len(tokenlist) != 0) {
    // at depth 0 we have reached the end with left over tokens 
    // in the tokenlist. 
    ObjectUtil_eprintf("ERROR: ParseError, invalid tokenlist=%v\n", tokenlist);
    Object_assign(&ret, NULL);
    ret = QERROR_NEW1("invalid input tokenlist");
  }
  Object_return(ret);   // mark object for returning
  Object_rc_decr(ret);  // release the RC in this proc
  assert(ret->rc == 0);
  return ret;
}

Object* Lisp_parse_tokens(Object* tokenlist) {
  return Lisp_parse_tokens2(tokenlist, 0);
}

Object* Lisp_parse_string(Object* str) {
  Object* tokens = Object_accept(Lisp_tokenize(str));
  if(Object_is_error(tokens)) {
    return tokens;
  }
  Object* parsed = Object_return(Lisp_parse_tokens(tokens));
  Object_rc_decr(tokens); tokens = NULL;
  return parsed;
}
