#include <stdio.h>
#include "nassert.h"
#include "Tokenizer.h"

void assert_token_match(Tokenizer* tokenizer, TokenType* tarr, int tarr_len) {
  int i;
  Token token;
  int limit = 2*tarr_len;
  for(i = 0; i < limit && token.type != tt_Eos; i++) {
    token = Tokenizer_dq(tokenizer);
    nassert_eq(token.type, tarr[i]);
  }
  nassert(i == tarr_len);
}

int main(int argc, char** argv) {
  Tokenizer* tokenizer = NULL;
  Token token;
  int i = 0;

  printf("=========== { CASE 001 {\n");
  tokenizer = Tokenizer_new();
  Tokenizer_dprint(tokenizer);
  Tokenizer_nq_cstr1(tokenizer, "(+ 2.5 (- 3 1))");
  TokenType texpected[] = {
    tt_ParenBeg,
    tt_BareWord,
    tt_Number,
    tt_ParenBeg,
    tt_BareWord,
    tt_Number,
    tt_Number,
    tt_ParenEnd,
    tt_ParenEnd,
    tt_Eos
  };
  assert_token_match(tokenizer, texpected, 10);
  printf("=========== } CASE 001 }\n");
  return 0;

  printf("=========== { CASE 002 {\n");
  tokenizer = Tokenizer_new();
  Tokenizer_dprint(tokenizer);
  Tokenizer_nq_cstr1(tokenizer, "(println \"hello world\")");
  while(1) {
    token = Tokenizer_dq(tokenizer);
    Tokenizer_ptoken(tokenizer, token); printf("\n");
    if(token.type == tt_Eos) {
      break;
    }
  }
  Tokenizer_del(&tokenizer);
  printf("=========== } CASE 002 }\n");

  printf("=========== { CASE 002b {\n");
  tokenizer = Tokenizer_new();
  Tokenizer_dprint(tokenizer);
  Tokenizer_nq_cstr1(tokenizer, "(println \"hello \\\" world\")");
  while(1) {
    token = Tokenizer_dq(tokenizer);
    Tokenizer_ptoken(tokenizer, token); printf("\n");
    if(token.type == tt_Eos) {
      break;
    }
  }
  Tokenizer_del(&tokenizer);
  printf("=========== } CASE 002b }\n");


  printf("=========== { CASE 003 {\n");
  tokenizer = Tokenizer_new();
  Tokenizer_dprint(tokenizer);
  Tokenizer_nq_cstr1(tokenizer, "(- 25 (+ -4 1))");
  i = 0;
  while(1) {
    token = Tokenizer_dq(tokenizer);
    Tokenizer_ptoken(tokenizer, token); printf("\n");
    switch(i) {
      case 0:
        nassert(token.type == tt_ParenBeg);
        break;
      case 1:
        nassert(token.type == tt_BareWord);
        break;
      case 2:
        nassert(token.type == tt_Number);
        break;
      case 3:
        nassert(token.type == tt_ParenBeg);
        break;
      case 4:
        nassert(token.type == tt_BareWord);
        break;
      case 5:
        nassert(token.type == tt_Number);
        break;
      case 6:
        nassert(token.type == tt_Number);
        break;
      case 7:
        nassert(token.type == tt_ParenEnd);
        break;
      case 8:
        nassert(token.type == tt_ParenEnd);
        break;
    }
    i++;
    if(token.type == tt_Eos) {
      break;
    }
  }
  Tokenizer_del(&tokenizer);
  nassert(i == 10);
  printf("=========== } CASE 003 }\n");

  printf("=========== { CASE Float-1 {\n");
  tokenizer = Tokenizer_new();
  Tokenizer_dprint(tokenizer);
  Tokenizer_nq_cstr1(tokenizer, "(print 3.14)");
  i = 0;
  while(1) {
    token = Tokenizer_dq(tokenizer);
    Tokenizer_ptoken(tokenizer, token); printf("\n");
    switch(i) {
      case 0:
        nassert(token.type == tt_ParenBeg);
        break;
      case 1:
        nassert(token.type == tt_BareWord);
        break;
      case 2:
        nassert(token.type == tt_Number);
        nassert(token.info & TOKEN_IS_NUMBER_FLOAT);
        nassert(!(token.info & TOKEN_IS_NUMBER_NEGATIVE));
        break;
      case 3:
        nassert(token.type == tt_ParenEnd);
        break;
    }
    i++;
    if(token.type == tt_Eos) {
      break;
    }
  }
  Tokenizer_del(&tokenizer);
  printf("=========== } CASE Float-2 }\n");

  printf("=========== { CASE Float-Negative-1 {\n");
  tokenizer = Tokenizer_new();
  Tokenizer_dprint(tokenizer);
  Tokenizer_nq_cstr1(tokenizer, "(print -3.14)");
  i = 0;
  while(1) {
    token = Tokenizer_dq(tokenizer);
    Tokenizer_ptoken(tokenizer, token); printf("\n");
    switch(i) {
      case 0:
        nassert(token.type == tt_ParenBeg);
        break;
      case 1:
        nassert(token.type == tt_BareWord);
        break;
      case 2:
        nassert(token.type == tt_Number);
        nassert(token.info & TOKEN_IS_NUMBER_FLOAT);
        nassert(token.info & TOKEN_IS_NUMBER_NEGATIVE);
        break;
      case 3:
        nassert(token.type == tt_ParenEnd);
        break;
    }
    i++;
    if(token.type == tt_Eos) {
      break;
    }
  }
  Tokenizer_del(&tokenizer);
  printf("=========== } CASE Float-Negative-1 }\n");

  //----
  nassert_report();
  return nassert_exit_code();
}
