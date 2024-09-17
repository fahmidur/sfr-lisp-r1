#include <stdio.h>
#include <stdlib.h>
#include "Util.h"
#include "Object.h"
#include "Lisp.h"

Object* LISP_PAREN_BEG;
Object* LISP_PAREN_END;

// The root Environment for the Lisp interpreter.
// It is this Environment that contains the core functions
// that user-defined Lisp functions will use.
Object* LispEnv_root;

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

Object* fn_add(Function* fn, Object* env, Object* argv) {
  Object* a = Object_accept(Object_bop_hget(env, QSYMBOL("a")));
  Object* b = Object_accept(Object_bop_hget(env, QSYMBOL("b")));
  /* ObjectUtil_eprintf("fn_add. got a = %v\n", a); */
  /* ObjectUtil_eprintf("fn_add. got b = %v\n", b); */
  Object* ret = Object_return(Object_accept(Object_bop_add(a, b)));
  Object_rc_decr(a);
  Object_rc_decr(b);
  Object_rc_decr(ret);
  return ret;
}

Object* fn_sub(Function* fn, Object* env, Object* argv) {
  Object* a = Object_accept(Object_bop_hget(env, QSYMBOL("a")));
  Object* b = Object_accept(Object_bop_hget(env, QSYMBOL("b")));
  /* ObjectUtil_eprintf("fn_sub. got a = %v\n", a); */
  /* ObjectUtil_eprintf("fn_sub. got b = %v\n", b); */
  Object* ret = Object_return(Object_accept(Object_bop_sub(a, b)));
  Object_rc_decr(a);
  Object_rc_decr(b);
  Object_rc_decr(ret);
  return ret;
}

Object* fn_mul(Function* fn, Object* env, Object* argv) {
  Object* a = Object_accept(Object_bop_hget(env, QSYMBOL("a")));
  Object* b = Object_accept(Object_bop_hget(env, QSYMBOL("b")));
  /* ObjectUtil_eprintf("fn_mul. got a = %v\n", a); */
  /* ObjectUtil_eprintf("fn_mul. got b = %v\n", b); */
  Object* ret = Object_return(Object_accept(Object_bop_mul(a, b)));
  Object_rc_decr(a);
  Object_rc_decr(b);
  Object_rc_decr(ret);
  return ret;
}

Object* fn_div(Function* fn, Object* env, Object* argv) {
  Object* a = Object_accept(Object_bop_hget(env, QSYMBOL("a")));
  Object* b = Object_accept(Object_bop_hget(env, QSYMBOL("b")));
  /* ObjectUtil_eprintf("fn_div. got a = %v\n", a); */
  /* ObjectUtil_eprintf("fn_div. got b = %v\n", b); */
  Object* ret = Object_return(Object_accept(Object_bop_div(a, b)));
  Object_rc_decr(a);
  Object_rc_decr(b);
  Object_rc_decr(ret);
  return ret;
}

Object* fn_display(Function* fn, Object* env, Object* argv) {
  assert(fn != NULL);
  assert(argv != NULL);
  if(Object_is_null(argv)) {
    printf("\n");
    return Object_new_null();
  }
  assert(Object_type(argv) == SYMBOL_LIST);
  printf("fn_display. Called\n");
  ObjectUtil_eprintf("fn_display. argv=%v\n", argv);
  ObjectUtil_eprintf("fn_display. len(argv)=%d\n", Object_len(argv));
  int argv_len = Object_len(argv);
  Object* tmp;
  Symbol* tmp_type;
  int i = 0;
  ListIter* argv_iter = ListIter_new(argv->impl);
  ListIter_head(argv_iter);
  while(!ListIter_at_end(argv_iter)) {
    tmp = ListIter_get_val(argv_iter);
    /* ObjectUtil_eprintf("display. tmp=%v\n", tmp); */
    tmp_type = Object_type(tmp);
    if(i > 0) {
      printf(" ");
    }
    if(tmp_type == SYMBOL_STRING) {
      printf("%s", (char*)(((String*)tmp->impl)->buf));
    } 
    else
    if(tmp_type == SYMBOL_NUMBER) {
      printf("%f", (((Number*)tmp->impl)->rep));
    }
    else {
      Object_print(tmp);
    }
    ListIter_next(argv_iter);
    i++;
  }
  ListIter_del(argv_iter);
  return Object_new_null();
}

Object* fn_displayln(Function* fn, Object* env, Object* argv) {
  fn_display(fn, env, argv);
  printf("\n");
  return Object_new_null();
}

Object* fn_begin(Function* fn, Object* env, Object* argv) {
  Object* ret = NULL;
  if(Object_len(argv) > 0) {
    ret = Object_uop_tail(argv);
  }
  else {
    ret = Object_new_null();
  }
  return ret;
}

void Lisp_init() {
  LISP_PAREN_BEG = QSYMBOL_NEW1("(");
  LISP_PAREN_END = QSYMBOL_NEW1(")");
  LispEnv_root = Object_new(SYMBOL_ENVIRONMENT, 1, Environment_new());

  Object* fnobj_display = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("display"), NULL, fn_display, -1, NULL, NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("display"), fnobj_display);
  
  Object* fnobj_displayln = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("displayln"), NULL, fn_displayln, -1, NULL, NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("displayln"), fnobj_displayln);


  Object* fnobj_add = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("+"), NULL, fn_add, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("+"), fnobj_add);

  Object* fnobj_sub = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("-"), NULL, fn_sub, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("-"), fnobj_sub);

  Object* fnobj_mul = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("*"), NULL, fn_mul, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("*"), fnobj_mul);
  
  Object* fnobj_div = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("/"), NULL, fn_div, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("/"), fnobj_div);

  Object* fnobj_begin = Object_new(SYMBOL_FUNCTION, 1,
    Function_new(QSYMBOL("begin"), NULL, fn_begin, -1, NULL, NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("begin"), fnobj_begin);
}

void Lisp_done() {
  Object_assign(&LISP_PAREN_BEG, NULL);
  Object_assign(&LISP_PAREN_END, NULL);
  Object_assign(&LispEnv_root, NULL);
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

/* Object* Lisp_parse(Object* tokens) { */
/*   assert(tokens != NULL); */
/*   Object_rc_incr(tokens); */
/*   // TODO */
/*   Object_rc_decr(tokens); */
/*   return NULL; */
/* } */

Object* Lisp_tokenize(Object* string) {
  assert(string != NULL);
  Object_rc_incr(string);
  assert(Object_type(string) == SYMBOL_STRING);

  size_t string_len = Object_len(string);
  Object* ret = QLIST_NEW1();
  /* printf("string_len = %ld\n", string_len); */

  size_t i = 0;
  TokenizerState state = ts_Init;

  char ch;
  Object* paren_beg = QSYMBOL_NEW1("(");
  Object* paren_end = QSYMBOL_NEW1(")");

  Object* tmp_str = QSTRING_NEW1("");
  /* ObjectUtil_eprintf("tmp_str=%v\n", tmp_str); */

  /* printf("--- beg-of-forloop ---\n"); */ 
  for(i = 0; i < string_len; i++) {
    ch = Object_bop_charat(string, i);
    /* printf("---\n"); */
    /* printf("state="); print_TokenizerState(state); printf("\n"); */
    /* ObjectUtil_eprintf("Lisp_tokenizer. ch=|%c| tmp_str=%v\n", ch, tmp_str); */
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
  Object* ret = Object_new_null();
  Object* sublist = Object_new_null();
  int idx = 0;
  char softbreak = 0;
  /* if(Object_len(tokenlist) == 1 && Object_type(Object_uop_head(tokenlist)) != SYMBOL_LIST) { */
  /* } */
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
      if(Object_is_null(ret)) {
        ret = QLIST_NEW1();
      }
      Object_bop_push(ret, sublist);
      Object_assign(&sublist, NULL);
    }
    else
    if(Object_cmp(tmp, LISP_PAREN_END) == 0) {
      softbreak = 1;
    }
    else {
      // some sort of atom
      if(Object_is_null(ret) && Object_len(tokenlist) == 0) {
        ret = Object_accept(tmp);
        softbreak = 1;
      }
      else {
        if(Object_is_null(ret)) {
          ret = QLIST_NEW1();
        }
        Object_bop_push(ret, tmp);
      }
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

  // We are returning a constructed object, that must
  // be accepted or reject with rc=0;
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

Object* Lisp_eval_sexp2(Object* sexp, Object* env) {
  Object* ret = Object_new_null();
  Object* tmp = NULL;
  Object* tmp2 = NULL;
  Object* op = NULL;
  Object* opval = NULL;
  Object* opargs1 = NULL;
  Object* opargs2 = NULL;
  Symbol* sexp_type = Object_type(sexp);
  if(
      sexp_type == SYMBOL_NUMBER ||
      sexp_type == SYMBOL_STRING ||
      sexp_type == SYMBOL_FUNCTION
  ) {
    ret = Object_accept(sexp);
  }
  else
  if(sexp_type == SYMBOL_SYMBOL) {
    ret = Object_accept(Object_bop_hget(env, sexp));
  }
  else
  if(sexp_type == SYMBOL_LIST) {
    // it is in this case that we can have various lisp forms
    op = Object_accept(Object_uop_head(sexp));
    opval = Lisp_eval_sexp2(op, env);
    if(Object_type(op) == SYMBOL_SYMBOL) {
      if(op == QSYMBOL("quote")) {
        ret = Object_accept(Object_uop_rest(sexp));
      }
      else
      if(op == QSYMBOL("define")) {
        opargs1 = Object_accept(Object_uop_rest(sexp));
        Object_top_hset(env, op, Lisp_eval_sexp2(opargs1, env));
      }
      else
      if(op == QSYMBOL("set!")) {
        // set the value of a variable that has been already defined
        if(!Object_is_null(opval)) {
          opargs1 = Object_accept(Object_uop_rest(sexp));
          Object_reject(Object_top_hset(env, op, Lisp_eval_sexp2(opargs1, env)));
        }
      }
      else
      if(Object_type(opval) == SYMBOL_FUNCTION) {
        // map each arg to values
        opargs1 = Object_accept(Object_uop_rest(sexp));
        opargs2 = Object_new_list(1, 0);
        ListIter* iter = ListIter_new(opargs1->impl);
        ListIter_head(iter);
        while(!ListIter_at_end(iter)) {
          tmp = Object_accept(ListIter_get_val(iter));
          tmp2 = Object_accept(Lisp_eval_sexp2(tmp, env));
          Object_bop_push(opargs2, tmp2);
          Object_assign(&tmp, NULL);
          Object_assign(&tmp2, NULL);
          ListIter_next(iter);
        }
        ListIter_del(iter);
        ret = Object_bop_call(opval, opargs2);
      }
    }
  }
_return:
  if(ret == NULL) {
    ret = Object_new_null();
  }
  if(op != NULL) {
    Object_assign(&op,  NULL);
  }
  if(opval != NULL) {
    Object_assign(&opval, NULL);
  }
  if(Object_is_null(ret)) {
  }
  else {
    Object_return(ret);
    Object_rc_decr(ret);
  }
  return ret;
}

Object* Lisp_eval_sexp(Object* sexp) {
  return Lisp_eval_sexp2(sexp, LispEnv_root);
}

/* LispEnv* LispEnv_new(LispEnv* parent) { */
/*   LispEnv* self = calloc(1, sizeof(LispEnv)); */
/*   if(self == NULL) { */
/*     // ERROR: unable to reserve memory */
/*     return self; */
/*   } */
/*   self->children_head = NULL; */
/*   self->children_tail = NULL; */
/*   self->sibling_next = NULL; */
/*   self->sibling_prev = NULL; */
/*   self->objects = QHASH_NEW1(); */

/*   if(parent != NULL) { */
/*     LispEnv_child_add(parent, self); */
/*   } */
/*   return self; */
/* } */

/* void LispEnv_del(LispEnv* self) { */
/*   assert(self != NULL); */
/*   LispEnv* curr_child = self->children_head; */
/*   LispEnv* next_child = NULL; */
/*   while(curr_child != NULL) { */
/*     // save the next child */
/*     next_child = curr_child->sibling_next; */
/*     LispEnv_del(curr_child); */
/*     curr_child = next_child; */
/*   } */
/*   // unlink everything */
/*   self->parent = NULL; */
/*   self->children_head = NULL; */
/*   self->children_tail = NULL; */
/*   self->sibling_next = NULL; */
/*   self->sibling_prev = NULL; */
/*   Object_assign(&(self->objects), NULL); */
/*   free(self); */
/* } */

/* void LispEnv_child_add(LispEnv* self, LispEnv* child) { */
/*   assert(self != NULL); */
/*   assert(child != NULL); */
/*   assert(child->sibling_next == NULL); */
/*   assert(child->sibling_prev == NULL); */
/*   child->parent = self; */
/*   if(self->children_head == NULL && self->children_tail == NULL) { */
/*     // first child being added */
/*     self->children_head = self->children_tail = child; */
/*     child->sibling_prev = NULL; */
/*     child->sibling_next = NULL; */
/*   } */
/*   else { */
/*     LispEnv* iter = self->children_head; */
/*     while(iter != NULL) { */
/*       if(iter == child) { */
/*         // the child is already in this list */
/*         return; */
/*       } */
/*       iter = iter->sibling_next; */
/*     } */
/*     LispEnv* last_child = self->children_tail; */
/*     last_child->sibling_next = child; */
/*     child->sibling_prev = last_child; */
/*     self->children_tail = child; */
/*   } */
/* } */

/* void LispEnv_child_rem(LispEnv* self, LispEnv* child) { */
/*   assert(self != NULL); */
/*   assert(child != NULL); */
/*   // TODO */
/* } */

