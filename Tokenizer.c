#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Tokenizer.h"
#include "debug.h"

#define BUF_ISIZE 4

Tokenizer* Tokenizer_new() {
  Tokenizer* self = malloc(sizeof(Tokenizer));
  self->buf_curp = 0;
  self->buf_endp = 0;
  self->buf = malloc(BUF_ISIZE * sizeof(char));
  self->buf_size = BUF_ISIZE;
  self->state = ts_Init;
  return self;
}

void Tokenizer_ptoken(Tokenizer* self, Token tok) {
  char hasbody = 0;
  switch(tok.type) {
    case tt_ParenBeg:
      printf("<ParenBeg");
      hasbody = 1;
      break;
    case tt_ParenEnd:
      printf("<ParenEnd");
      hasbody = 1;
      break;
    case tt_Symbol:
      printf("<Symbol");
      break;
    case tt_Operator:
      printf("<Operator");
      hasbody = 1;
      break;
    case tt_BareWord:
      printf("<BareWord");
      hasbody = 1;
      break;
    case tt_String:
      printf("<String");
      hasbody = 1;
      break;
    case tt_Number:
      printf("<Number");
      if(tok.info) {
        if(tok.info & TOKEN_IS_NUMBER_FLOAT) {
          printf(" FLOAT");
        }
        if(tok.info & TOKEN_IS_NUMBER_NEGATIVE) {
          printf(" NEG");
        }
      }
      hasbody = 1;
      break;
    case tt_Unknown:
      printf("<Unknown");
      break;
    case tt_Eos:
      printf("<Eos");
      break;
    default:
      printf("<ERROR");
  }
  if(hasbody) {
    unsigned int i = tok.seg_begp;
    printf(" : '");
    while(i < tok.seg_endp) {
      printf("%c", self->buf[i++]);
    }
    printf("'");
  }
  printf(">");
}

void Tokenizer_dprint(Tokenizer* self) {
  printf(
    "Tokenizer(%p){buf_curp=%d, buf_endp=%d, buf_size=%d}\n", 
    self,
    self->buf_curp,
    self->buf_endp,
    self->buf_size
  );
}

void Tokenizer_del(Tokenizer** self_ptr) {
  Tokenizer* self = *self_ptr;
  if(self == NULL) {
    return;
  }
  if(self->buf != NULL) {
    free(self->buf);
    self->buf = NULL;
  }
  free(self);
  *self_ptr = NULL;
}

char Tokenizer_buf_push(Tokenizer* self, char* str, unsigned int str_size) {
  printf("Tokenizer_buf_push. buf=|%s| str=%s\n", self->buf, str);
  unsigned int i;
  unsigned int buf_size_new = self->buf_size + str_size;
  while(buf_size_new >= self->buf_size) {
    // double the size of buf
    self->buf = realloc(self->buf, self->buf_size*2);
    self->buf_size = self->buf_size*2;
  }
  for(i = 0; i < str_size; i++) {
    self->buf[self->buf_endp++] = str[i];
  }
  printf("Tokenizer_buf_push. buf=|%s| buf_endp=%d buf_size=%d\n", self->buf, self->buf_endp, self->buf_size);
}

char Tokenizer_nq_cstr1(Tokenizer* self, char* str) {
  printf("Tokenizer_nq_cstr1. str=|%s|\n", str);
  return Tokenizer_buf_push(self, str, strlen(str));
  return 1;
}

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

void Tokenizer_dq_back1(Tokenizer* self) {
  if(self->buf_curp > 0) {
    self->buf_curp--;
  }
}

Token Tokenizer_dq(Tokenizer* self) {
  /*printf("Tokenizer_dq. beg. %3d/%3d buf=|%s|\n", self->buf_curp, self->buf_endp, self->buf);*/

  Token ret = {
    .type = tt_Unknown,
    .info = 0
  };

  char* buf = self->buf;
  char ch;

  unsigned int idx;
  char is_buf_present = 0;

  char quote_hitted = 0;

  while(self->buf_curp < self->buf_endp) {
    is_buf_present = 1;
    idx = self->buf_curp++;
    ch = buf[idx];
    printf("%3d/%3d || ch=%c\n", idx, self->buf_endp, ch);
    //---
    if(self->state == ts_Init) {
      if(ch == '-') {
        self->state = ts_InNumberNegMaybe;
        ret.seg_begp = idx; ret.seg_endp = idx+1;
      }
      else
      if(TokenizerUtil_isdigit(ch)) {
        self->state = ts_InNumber;
        ret.type = tt_Number;
        ret.seg_begp = idx; ret.seg_endp = idx+1;
      }
      else
      if(TokenizerUtil_wordlike(ch)) {
        self->state = ts_InBareWord;
        ret.type = tt_BareWord;
        ret.seg_begp = idx; ret.seg_endp = idx+1;
      }
      else
      if(ch == '(') {
        ret.type = tt_ParenBeg;
        ret.seg_begp = idx; ret.seg_endp = idx+1;
        break;
      }
      else
      if(ch == ')') {
        ret.type = tt_ParenEnd;
        ret.seg_begp = idx; ret.seg_endp = idx+1;
        break;
      }
      else
      if(ch == '"') {
        self->state = ts_InString;
        ret.type = tt_String;
        ret.seg_begp = idx+1; ret.seg_endp = idx+1;
      }
      else
      if(ch == ' ') {
        // eat
      }
      else {
        ret.type = tt_Error;
        break;
      }
    }
    else
    if(self->state == ts_InNumber) {
      if(TokenizerUtil_isdigit(ch)) {
        ret.seg_endp++;
      }
      else
      if(ch == '.') {
        self->state = ts_InNumberFloat;
        ret.info |= TOKEN_IS_NUMBER_FLOAT;
      }
      else {
        self->state = ts_Init;
        ret.seg_endp = idx;
        Tokenizer_dq_back1(self);
        break;
      }
    }
    else
    if(self->state == ts_InNumberNegMaybe) {
      if(TokenizerUtil_isdigit(ch)) {
        self->state = ts_InNumber;
        ret.type = tt_Number;
        ret.info |= TOKEN_IS_NUMBER_NEGATIVE;
        ret.seg_endp++;
      }
      else {
        ret.type = tt_BareWord;
        ret.seg_endp = idx;
        self->state = ts_Init;
        Tokenizer_dq_back1(self);
        break;
      }
    }
    else
    if(self->state == ts_InNumberFloat) {
      if(TokenizerUtil_isdigit(ch)) {
        ret.seg_endp++;
      }
      else {
        self->state = ts_Init;
        ret.seg_endp = idx;
        Tokenizer_dq_back1(self);
        break;
      }
    }
    else
    if(self->state == ts_InBareWord) {
      if(TokenizerUtil_wordlike(ch)) {
        ret.seg_endp++;
      }
      else {
        self->state = ts_Init;
        Tokenizer_dq_back1(self);
        break;
      }
    }
    else
    if(self->state == ts_InString) {
      if(ch == '"') {
        self->state = ts_Init;
        ret.seg_endp = idx;
        break;
      }
      else
      if(ch == 92) { // backslash character
        self->state = ts_InStringEscaped;
      }
      else {
        ret.seg_endp++;
      }
    }
    else
    if(self->state == ts_InStringEscaped) {
      ret.seg_endp++;
      self->state = ts_InString;
    }
  }

  if(!is_buf_present) {
    ret.type = tt_Eos;
  }

  /*printf("Tokenizer_dq. end. %3d/%3d\n", self->buf_curp, self->buf_endp);*/
  return ret;
}


