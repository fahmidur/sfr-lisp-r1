#ifndef _SFR_TOKENIZER_H
#define _SFR_TOKENIZER_H

typedef enum TokenType TokenType;
enum TokenType {
  tt_ParenBeg,
  tt_ParenEnd,
  tt_BareWord,
  tt_Operator, // deprecated
  tt_Symbol,
  tt_String,
  tt_Number,
  tt_Unknown,
  tt_Eos,
  tt_Error,
};

#define TOKEN_IS_NUMBER_FLOAT      0b00000001
#define TOKEN_IS_NUMBER_NEGATIVE   0b00000010

typedef struct Token Token;
struct Token {
  TokenType       type;
  unsigned int    seg_begp; // inclusive
  unsigned int    seg_endp; // exclusive
  unsigned int    info; 
};

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

typedef struct Tokenizer Tokenizer;
struct Tokenizer {
  TokenizerState state;
  unsigned int buf_curp;
  unsigned int buf_endp;
  unsigned int buf_size;
  char* buf;
};

Tokenizer* Tokenizer_new(void);
void Tokenizer_del(Tokenizer** self_ptr);

void Tokenizer_ptoken(Tokenizer* self, Token token);
void Tokenizer_dprint(Tokenizer* self);

char Tokenizer_nq_cstr1(Tokenizer* self, char* str);
Token Tokenizer_dq(Tokenizer* self);

char Token_cmp_cstr(Token tok, char* str);

#endif
