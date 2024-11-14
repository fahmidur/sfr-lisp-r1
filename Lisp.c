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
Object* LispSymbol_quote;
Object* LispSymbol_define;
Object* LispSymbol_setbang;
Object* LispSymbol_lambda;

typedef enum TokenizerState TokenizerState;
enum TokenizerState {
  ts_Init,
  ts_InString,
  ts_InStringEscaped,
  ts_InNumber,
  ts_InNumberFloat,
  ts_InNumberNegMaybe,
  ts_InNumberPosMaybe,
  ts_InBareWord,
};

Object* fn_gc_run(Function* fn, Object* env, Object* argv) {
  printf("Calling Object_system_gc() ...\n");
  Object_system_gc();
  return NULL;
}

Object* fn_gc_info(Function* fn, Object* env, Object* argv) {
  Object_system_print();
  return NULL;
}

Object* fn_add(Function* fn, Object* env, Object* argv) {
  Object_reject(Object_bop_hget(env, QSYMBOL("a")));
  Object_reject(Object_bop_hget(env, QSYMBOL("b")));
  return Object_new_null();
  /* return Object_return( */
  /*     Object_bop_add( */
  /*       Object_bop_hget(env, QSYMBOL("a")), */
  /*       Object_bop_hget(env, QSYMBOL("b")) */
  /*     ) */
  /* ); */
  /* Object* a = Object_accept(Object_bop_hget(env, QSYMBOL("a"))); */
  /* Object* b = Object_accept(Object_bop_hget(env, QSYMBOL("b"))); */
  /* /1* ObjectUtil_eprintf("fn_add. got a = %v\n", a); *1/ */
  /* /1* ObjectUtil_eprintf("fn_add. got b = %v\n", b); *1/ */
  /* Object* ret = Object_return(Object_accept(Object_bop_add(a, b))); */
  /* Object_assign(&a, NULL); */
  /* Object_assign(&b, NULL); */
  /* Object_rc_decr(ret); */
  /* return ret; */
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

Object* fn_print(Function* fn, Object* env, Object* argv) {
  assert(fn != NULL);
  assert(argv != NULL);
  if(Object_is_null(argv)) {
    dbg_printf("\n");
    return Object_new_null();
  }
  assert(Object_type(argv) == SYMBOL_LIST);
  int argv_len = Object_len(argv);
  Object* tmp;
  Symbol* tmp_type;
  int i = 0;
  ListIter* argv_iter = ListIter_new(argv->impl);
  ListIter_head(argv_iter);
  while(!ListIter_at_end(argv_iter)) {
    tmp = ListIter_get_val(argv_iter);
    tmp_type = Object_type(tmp);
    if(i > 0) {
      printf(" ");
    }
    if(tmp_type == SYMBOL_STRING) {
      String_print_quoted(tmp->impl);
    } 
    else
    if(tmp_type == SYMBOL_NUMBER) {
      Number_print_bare(tmp->impl);
    }
    else
    if(tmp_type == SYMBOL_FUNCTION) {
      printf("#<procedure:%p>", tmp);
    }
    else {
      Object_print(tmp);
    }
    ListIter_next(argv_iter);
    i++;
  }
  ListIter_del(argv_iter);
  fflush(stdout);
  return Object_new_null();
}

Object* fn_display(Function* fn, Object* env, Object* argv) {
  assert(fn != NULL);
  assert(argv != NULL);
  if(Object_is_null(argv)) {
    dbg_printf("\n");
    return Object_new_null();
  }
  assert(Object_type(argv) == SYMBOL_LIST);
#ifdef DEBUG
  dbg_printf("fn_display. Called\n");
  ObjectUtil_eprintf("fn_display. argv=%v\n", argv);
  ObjectUtil_eprintf("fn_display. len(argv)=%d\n", Object_len(argv));
#endif
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
      Number_print_bare(tmp->impl);
    }
    /* else */
    /* if(tmp_type == SYMBOL_FUNCTION) { */
    /*   printf("#<procedure:%p>", tmp); */
    /* } */
    else {
      Object_print(tmp);
    }
    ListIter_next(argv_iter);
    i++;
  }
  ListIter_del(argv_iter);
  fflush(stdout);
  return Object_new_null();
}

Object* fn_displayln(Function* fn, Object* env, Object* argv) {
  fn_display(fn, env, argv);
  printf("\n");
  return Object_new_null();
}

Object* fn_println(Function* fn, Object* env, Object* argv) {
  fn_print(fn, env, argv);
  printf("\n");
  return Object_new_null();
}

Object* fn_newline(Function* fn, Object* env, Object* argv) {
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

Object* fn_lambda(Function* fn, Object* env, Object* argv) {
  Object* env2 = Object_new(SYMBOL_ENVIRONMENT, 1, Environment_new());
  Object_bop_child_attach(fn->env, env2);

  // zip all the args into a new environment.
  // @params.zip(argv).to_h
  if(fn->arity > 0) {
    Object* params = Object_accept(fn->params);
    ListIter* params_iter = ListIter_new(params->impl);
    ListIter_head(params_iter);
    ListIter* argv_iter = ListIter_new(argv->impl);
    ListIter_head(argv_iter);
    while( ! (ListIter_at_end(params_iter) || ListIter_at_end(argv_iter)) ) {
      Object_top_hset(env2, ListIter_get_val(params_iter), ListIter_get_val(argv_iter));
      ListIter_next(params_iter);
      ListIter_next(argv_iter);
    }
    ListIter_del(argv_iter);
    argv_iter = NULL;
    ListIter_del(params_iter);
    params_iter = NULL;
    Object_assign(&params, NULL);
  }

  Object* ret = Object_new_null();

  /* ObjectUtil_eprintf("donuts. fn_lambda. body=%v\n", fn->body); */

  // now eval the lambda body
  Object* body = Object_accept(fn->body);
  if(body != NULL && !Object_is_null(body)) {
    ListIter* body_iter = ListIter_new(body->impl);
    ListIter_head(body_iter);
    while(!ListIter_at_end(body_iter)) {
      /* ObjectUtil_eprintf("donuts. fn_lambda. body el=%v\n", ListIter_get_val(body_iter)); */
      if(!Object_is_null(ret)) {
        Object_assign(&ret, NULL);
      }
      ret = Object_accept(Lisp_eval_sexp2(ListIter_get_val(body_iter), env2));
      ListIter_next(body_iter); 
    }
    ListIter_del(body_iter);
  }
  Object_assign(&body, NULL);
  Object_assign(&env2, NULL);
  if(ret != NULL && !Object_is_null(ret)) {
    Object_return(ret);
    Object_rc_decr(ret);
  }
  return ret;
}

Object* fn_cmp_lt(Function* fn, Object* env, Object* argv) {
  Object* a = Object_accept(Object_bop_hget(env, QSYMBOL("a")));
  Object* b = Object_accept(Object_bop_hget(env, QSYMBOL("b")));
  if(Object_cmp(a, b) < 0) {
    return QSYMBOL("true");
  } else {
    return Object_new_null();
  }
}

Object* fn_cmp_lte(Function* fn, Object* env, Object* argv) {
  Object* a = Object_accept(Object_bop_hget(env, QSYMBOL("a")));
  Object* b = Object_accept(Object_bop_hget(env, QSYMBOL("b")));
  if(Object_cmp(a, b) <= 0) {
    return QSYMBOL("true");
  } else {
    return Object_new_null();
  }
}

Object* fn_cmp_gt(Function* fn, Object* env, Object* argv) {
  Object* a = Object_accept(Object_bop_hget(env, QSYMBOL("a")));
  Object* b = Object_accept(Object_bop_hget(env, QSYMBOL("b")));
  if(Object_cmp(a, b) > 0) {
    return QSYMBOL("true");
  } else {
    return Object_new_null();
  }
}

Object* fn_cmp_gte(Function* fn, Object* env, Object* argv) {
  Object* a = Object_accept(Object_bop_hget(env, QSYMBOL("a")));
  Object* b = Object_accept(Object_bop_hget(env, QSYMBOL("b")));
  if(Object_cmp(a, b) >= 0) {
    return QSYMBOL("true");
  } else {
    return Object_new_null();
  }
}

Object* fn_cmp_equal(Function* fn, Object* env, Object* argv) {
  Object* a = Object_accept(Object_bop_hget(env, QSYMBOL("a")));
  Object* b = Object_accept(Object_bop_hget(env, QSYMBOL("b")));
  if(Object_cmp(a, b) == 0) {
    return QSYMBOL("true");
  } else {
    return Object_new_null();
  }
}

void Lisp_init() {
  LISP_PAREN_BEG = QSYMBOL_NEW1("(");
  LISP_PAREN_END = QSYMBOL_NEW1(")");
  LispEnv_root = Object_new(SYMBOL_ENVIRONMENT, 1, Environment_new());

  LispSymbol_quote = QSYMBOL_NEW1("quote");
  LispSymbol_define = QSYMBOL_NEW1("define");
  LispSymbol_setbang = QSYMBOL_NEW1("set!");
  LispSymbol_lambda = QSYMBOL_NEW1("lambda");

  Object* fnobj_display = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("display"), LispEnv_root, fn_display, -1, NULL, NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("display"), fnobj_display);
  
  Object* fnobj_displayln = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("displayln"), LispEnv_root, fn_displayln, -1, NULL, NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("displayln"), fnobj_displayln);

  Object* fnobj_print = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("print"), LispEnv_root, fn_print, -1, NULL, NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("print"), fnobj_print);
  
  Object* fnobj_println = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("println"), LispEnv_root, fn_println, -1, NULL, NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("println"), fnobj_println);

  Object* fnobj_newline = Object_new(SYMBOL_FUNCTION, 1,
      Function_new(QSYMBOL("newline"), LispEnv_root, fn_newline, 0, NULL, NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("newline"), fnobj_newline);


  Object* fnobj_add = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("+"), LispEnv_root, fn_add, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("+"), fnobj_add);
  Object_assign(&fnobj_add, NULL);

  Object* fnobj_sub = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("-"), LispEnv_root, fn_sub, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("-"), fnobj_sub);
  Object_assign(&fnobj_sub, NULL);

  Object* fnobj_mul = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("*"), LispEnv_root, fn_mul, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("*"), fnobj_mul);
  Object_assign(&fnobj_mul, NULL);
  
  Object* fnobj_div = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("/"), LispEnv_root, fn_div, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("/"), fnobj_div);
  Object_assign(&fnobj_div, NULL);

  Object* fnobj_cmp_gt = Object_new(SYMBOL_FUNCTION, 1,
    Function_new(QSYMBOL(">"), LispEnv_root, fn_cmp_gt, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL(">"), fnobj_cmp_gt);
  Object_assign(&fnobj_cmp_gt, NULL);

  Object* fnobj_cmp_gte = Object_new(SYMBOL_FUNCTION, 1,
    Function_new(QSYMBOL(">="), LispEnv_root, fn_cmp_gte, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL(">="), fnobj_cmp_gte);
  Object_assign(&fnobj_cmp_gte, NULL);

  Object* fnobj_cmp_lt = Object_new(SYMBOL_FUNCTION, 1,
    Function_new(QSYMBOL("<"), LispEnv_root, fn_cmp_lt, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("<"), fnobj_cmp_lt);
  Object_assign(&fnobj_cmp_lt, NULL);

  Object* fnobj_cmp_lte = Object_new(SYMBOL_FUNCTION, 1,
    Function_new(QSYMBOL("<="), LispEnv_root, fn_cmp_lte, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("<="), fnobj_cmp_lte);
  Object_assign(&fnobj_cmp_lte, NULL);

  Object* fnobj_cmp_equal = Object_new(SYMBOL_FUNCTION, 1,
    Function_new(QSYMBOL("equal?"), LispEnv_root, fn_cmp_equal, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("equal?"), fnobj_cmp_equal);
  Object_assign(&fnobj_cmp_equal, NULL);

  Object* fnobj_begin = Object_new(SYMBOL_FUNCTION, 1,
    Function_new(QSYMBOL("begin"), LispEnv_root, fn_begin, -1, NULL, NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("begin"), fnobj_begin);
  Object_assign(&fnobj_begin, NULL);

  Object* fnobj_gc_info = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("gc_info"), LispEnv_root, fn_gc_info, 0, NULL, NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("gc_info"), fnobj_gc_info);
  Object_assign(&fnobj_gc_info, NULL);

  Object* fnobj_gc_run = Object_new(SYMBOL_FUNCTION, 1, 
    Function_new(QSYMBOL("gc_run"), LispEnv_root, fn_gc_run, 0, NULL, NULL)
  );
  Object_top_hset(LispEnv_root, QSYMBOL("gc_run"), fnobj_gc_run);
  Object_assign(&fnobj_gc_run, NULL);

}

void Lisp_done() {
  dbg_printf("--- { Lisp_done() { ---\n");
  Object_assign(&LISP_PAREN_BEG, NULL);
  Object_assign(&LISP_PAREN_END, NULL);
  Object_assign(&LispEnv_root, NULL);
  dbg_printf("--- } Lisp_done() } ---\n");
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
    (ch == '/') ||
    (ch == '!') ||
    (ch == '>') ||
    (ch == '<') ||
    (ch == '=') ||
    (ch == '?')
  );
}

char TokenizerUtil_min_uint(unsigned int a, unsigned int b) {
  return (a < b) ? a : b;
}

void print_TokenizerState(TokenizerState x) {
  switch(x) {
    case ts_Init:
      dbg_printf("TokenizerState(ts_Init)");
      break;
    case ts_InString:
      dbg_printf("TokenizerState(ts_InString)");
      break;
    case ts_InStringEscaped:
      dbg_printf("TokenizerState(ts_InStringEscaped)");
      break;
    case ts_InNumber:
      dbg_printf("TokenizerState(ts_InNumber)");
      break;
    case ts_InNumberFloat:
      dbg_printf("TokenizerState(ts_InNumberFloat)");
      break;
    case ts_InNumberNegMaybe:
      dbg_printf("TokenizerState(ts_InNumberNegMaybe)");
      break;
    case ts_InBareWord:
      dbg_printf("TokenizerState(ts_InBareWord)");
      break;
    default:
      dbg_printf("ERROR");
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
  /* dbg_printf("string_len = %ld\n", string_len); */

  size_t i = 0;
  TokenizerState state = ts_Init;

  char ch;
  Object* paren_beg = QSYMBOL_NEW1("(");
  Object* paren_end = QSYMBOL_NEW1(")");

  Object* tmp_str = QSTRING_NEW1("");
  /* ObjectUtil_eprintf("tmp_str=%v\n", tmp_str); */

  /* dbg_printf("--- beg-of-forloop ---\n"); */ 
  for(i = 0; i < string_len; i++) {
    ch = Object_bop_charat(string, i);
    /* dbg_printf("---\n"); */
    /* dbg_printf("state="); print_TokenizerState(state); dbg_printf("\n"); */
    /* ObjectUtil_eprintf("Lisp_tokenizer. ch=|%c| tmp_str=%v\n", ch, tmp_str); */
    if(state == ts_Init) {
      if(ch == '-') {
        state = ts_InNumberNegMaybe;
      }
      else
      if(ch == '+') {
        state = ts_InNumberPosMaybe;
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
      if(TokenizerUtil_wordlike(ch) || TokenizerUtil_numlike(ch)) {
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
        // the previous '-' will be treated as part of BareWord
        Object_bop_addx_char(tmp_str, '-');
        i--;
        state = ts_InBareWord;
      }
    }
    else
    if(state == ts_InNumberPosMaybe) {
      if(TokenizerUtil_isdigit(ch)) {
        i--;
        state = ts_InNumber;
      }
      else {
        // the previous '+' will be treated as part of a BareWord
        Object_bop_addx_char(tmp_str, '+');
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
    /*dbg_printf("state="); print_TokenizerState(state); dbg_printf("\n");*/
  }
  /*dbg_printf("--- end-of-forloop ---\n"); */

  /*dbg_printf("state="); print_TokenizerState(state); dbg_printf("\n");*/
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
  /*dbg_printf("---\n");*/

  Object_rc_decr(string);
  Object_rc_decr(tmp_str);
  Object_return(ret);
  Object_rc_decr(ret);

  /* ObjectUtil_eprintf("donuts. got tokens = %v\n", ret); */
  return ret;
}

Object* Lisp_parse_tokens2(Object* tokenlist, int depth) {
  assert(tokenlist != NULL);
  tokenlist = Object_accept(tokenlist);
  Object* tmp = Object_new_null();
  Object* ret = Object_new_null();
  Object* sublist = Object_new_null();
  int idx = 0;
  char softbreak = 0;
  /* ObjectUtil_eprintf("donuts. lpt2. ret = %v. tokenlist=%v\n", ret, tokenlist); */
  /* ObjectUtil_eprintf("donuts. lpt2. tokenlist=%v len=%zu\n", tokenlist, Object_len(tokenlist)); */
  /* if(Object_len(tokenlist) == 1 && Object_type(Object_uop_head(tokenlist)) != SYMBOL_LIST) { */
  /* } */
  while(Object_len(tokenlist) > 0) {
    tmp = Object_accept(Object_uop_shift(tokenlist));
    /* ObjectUtil_eprintf("donuts. lpt2. d=%d | tmp = %v\n", depth, tmp); */
    if(depth == 0 && idx == 0 && Object_cmp(tmp, LISP_PAREN_BEG) == 0) {
      // special case of first '(' in the tokenlist
    }
    else
    if(Object_cmp(tmp, LISP_PAREN_BEG) == 0) {
      // create a sublist to append to ret
      sublist = Object_accept(Lisp_parse_tokens2(tokenlist, depth+1));
      assert(sublist->rc == 1);
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
    Object_assign(&tmp, NULL);
    if(softbreak) {
      // cleanup
      Object_assign(&sublist, NULL);
      break;
    }
  }
  if(depth == 0 && Object_len(tokenlist) != 0) {
    // at depth 0 we have reached the end with left over tokens 
    // in the tokenlist. 
    ObjectUtil_eprintf("ERROR: ParseError, invalid tokenlist=%v\n", tokenlist);
    Object_assign(&ret, NULL);
    ret = QERROR_NEW1("invalid input tokenlist");
  }
  if(!Object_is_null(ret)) {
    /* ObjectUtil_eprintf("donuts. lpt2. 002. ret = %v\n", ret); */
    Object_return(ret);   // mark object for returning
    Object_rc_decr(ret);  // release the RC in this proc
    /* ObjectUtil_eprintf("donuts. lpt2. 003. ret = %v\n", ret); */
    // We are returning a constructed object, that must
    // be accepted or reject with rc=0;
    assert(ret->rc == 0);
  }
  Object_assign(&tokenlist, NULL);
#ifdef DEBUG
  if(depth == 0) {
    printf("--- { donutshop --- {\n");
    Object_system_print();
    printf("--- } donutshop --- }\n");
  }
#endif
  return ret;
}

Object* Lisp_parse_tokens(Object* tokenlist) {
  return Lisp_parse_tokens2(tokenlist, 0);
}

Object* Lisp_parse_string(Object* str) {
  Object* tokens = Object_accept(Lisp_tokenize(str));
  /* ObjectUtil_eprintf("donuts. tokens = %v\n", tokens); */
  if(Object_is_error(tokens)) {
    return tokens;
  }
  /* ObjectUtil_eprintf("donuts. calling Lisp_parse_tokens(tokens) ...\n"); */
  Object* parsed = Object_return(Lisp_parse_tokens(tokens));
  Object_rc_decr(tokens); tokens = NULL;
  /* ObjectUtil_eprintf("donuts. parsed=%v\n", parsed); */
  return parsed;
}

Object* Lisp_eval_sexp2(Object* sexp, Object* env) {
  sexp = Object_accept(sexp);
  env = Object_accept(env);
  ObjectUtil_eprintf("donuts. Lisp_eval_sexp2. sexp.rc = %d sexp = %v\n", sexp->rc, sexp);
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
    /* ObjectUtil_eprintf("donuts. here. 01. sexp.rc = %d sexp = %v\n", sexp->rc, sexp); */
    ret = Object_accept(sexp);
    /* ObjectUtil_eprintf("donuts. here. 02. sexp.rc = %d sexp = %v\n", sexp->rc, sexp); */
  }
  else
  if(sexp_type == SYMBOL_SYMBOL) {
    ret = Object_accept(Object_bop_hget(env, sexp));
  }
  else
  if(sexp_type == SYMBOL_LIST) {
    // it is in this case that we can have various lisp forms
    op = Object_accept(Object_uop_head(sexp));
    opval = Object_accept(Lisp_eval_sexp2(op, env));
    if(Object_type(op) == SYMBOL_SYMBOL) {
      if(op == LispSymbol_quote) {
        if(Object_len(sexp) != 2) {
          ret = QERROR("invalid use of 'quote'");
        }
        else {
          ret = Object_accept(Object_uop_head(Object_uop_rest(sexp)));
        }
      }
      else
      if(op == LispSymbol_define) {
        opargs1 = Object_accept(Object_uop_rest(sexp));
        if(Object_len(opargs1) != 2) {
          // return an error
          ret = QERROR("invalid use of 'define'");
        }
        else {
          opargs2 = Object_accept(Object_bop_at(opargs1, 1));
          Object_reject(Object_top_hset(env, Object_uop_head(opargs1), Lisp_eval_sexp2(opargs2, env)));
        }
      }
      else
      if(op == LispSymbol_setbang) {
        opargs1 = Object_accept(Object_uop_rest(sexp));
        // set the value of a variable which has been already defined
        if(Object_len(opargs1) != 2) {
          // return an error
          ret = QERROR("invalid use of 'set!'");
        }
        else {
          tmp = Object_accept(Object_uop_head(opargs1));
          Object* tenv = Object_accept(Object_bop_rfind(env, tmp));
          opargs2 = Object_accept(Object_bop_at(opargs1, 1)); // 2nd arg to 'set!'
          if(Object_is_null(tenv)) {
            /* Object_reject(Object_top_hset(env, tmp, Lisp_eval_sexp2(opargs2, env))); */
            ret = QERROR("called 'set!' on non-existent var");
          }
          else {
            Object_reject(Object_top_hset(tenv, tmp, Lisp_eval_sexp2(opargs2, env)));
          }
          Object_assign(&tenv, NULL);
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
          /* ObjectUtil_eprintf("donuts. tmp_.rc = %d tmp_ = %v\n", tmp->rc, tmp); */
          tmp2 = Object_accept(Lisp_eval_sexp2(tmp, env));
          /* ObjectUtil_eprintf("donuts. tmp2.rc = %d tmp2 = %v\n", tmp2->rc, tmp2); */
          /* assert(tmp2->rc == 1); */
          Object_bop_push(opargs2, tmp2);
          Object_assign(&tmp, NULL);
          Object_assign(&tmp2, NULL);
          ListIter_next(iter);
        }
        ListIter_del(iter);
        dbg_printf("--- { Object_bop_call { ---\n");
        ret = Object_accept(Object_bop_call(opval, opargs2));
        dbg_printf("--- } Object_bop_call } ---\n");
        dbg_printf("{ delete opargs1 | len=%d {\n", Object_len(opargs1));
        Object_assign(&opargs1, NULL);
        dbg_printf("} delete opargs1 }\n");
        dbg_printf("{ delete opargs2 | len=%d {\n", Object_len(opargs2));
        Object_assign(&opargs2, NULL);
        dbg_printf("} delete opargs2 }\n");
      }
      else
      if(op == LispSymbol_lambda) {
        // lambda (a b c) (+ a b c)
        // lambda <params> <body>
        if(Object_len(sexp) < 3) {
          ret = QERROR("invalid use of 'lambda'");
        }
        else {
          ret = Object_new_null();
          Object* lambda_env = Object_rc_incr(env);
          Object* lambda_params = Object_accept(Object_uop_head(Object_uop_rest(sexp)));
          ssize_t lambda_plen = Object_len(lambda_params);
          Object* lambda_body = Object_accept(Object_uop_rest(Object_uop_rest(sexp)));
          ret = Object_new(
            SYMBOL_FUNCTION, 
            1, 
            Function_new(
              Object_new_null(),
              lambda_env, 
              fn_lambda,
              (lambda_plen > 0 ? lambda_plen : -1),
              lambda_params,
              lambda_body
            )
          );
          Object_assign(&lambda_body, NULL);
          Object_assign(&lambda_params, NULL);
          Object_assign(&lambda_env, NULL);
        }
      }
      else
      if(op == QSYMBOL("if")) {
        // (if (test-expr) (then-expr) (else-expr))
        if(Object_len(sexp) != 4) {
          ret = QERROR("invalid use of 'if'");
        }
        else {
          Object* test_expr = Object_bop_at(sexp, 1);
          Object* then_expr = Object_bop_at(sexp, 2);
          Object* else_expr = Object_bop_at(sexp, 3);

          Object* test_result = Lisp_eval_sexp2(test_expr, env);
          if(!Object_is_null(test_result)) {
            // all non-null objects are treated as truthy
            ret = Lisp_eval_sexp2(then_expr, env);
          } else {
            ret = Lisp_eval_sexp2(else_expr, env);
          }
        }
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
    // don't do anything to the null object
  }
  else {
    Object_return(ret);
    Object_rc_decr(ret);
  }
  /* Object_rc_decr(sexp); */
  printf("{ delete sexp {\n");
  Object_assign(&sexp, NULL);
  printf("} delete sexp }\n");
  /* Object_rc_decr(env); */
  Object_assign(&env, NULL);
  /* ObjectUtil_eprintf("donuts. eval. ret=%v\n", ret); */
  /* assert(ret->rc == 0); */
  /* if(ret->rc != 0) { */
  /*   ObjectUtil_eprintf("donuts. strange. rc = %d for ret = %v\n", ret->rc, ret); */
  /* } */
  return ret;
}

Object* Lisp_eval_sexp(Object* sexp) {
  return Lisp_eval_sexp2(sexp, LispEnv_root);
}

Object* Lisp_eval_string2(Object* str, Object* env) {
  return Lisp_eval_sexp2(Lisp_parse_string(str), env);
}

Object* Lisp_eval_string(Object* str) {
  return Lisp_eval_sexp(Lisp_parse_string(str));
}

void Lisp_printenv2(Object* env_obj) {
  printf("--- { Object<Environment>(%p) Environment(%p) { ---\n", env_obj, env_obj->impl);
  Environment* env = env_obj->impl;
  Object_print(env->objects);
  ListIter* iter = ListIter_new(env->children->impl);
  ListIter_head(iter);
  int i = 0;
  Object* child_env_obj = NULL;
  while(!ListIter_at_end(iter)) {
    printf("--- { Object<Environment>(%p) child %d { ---\n", env_obj, i);
    child_env_obj = Object_accept(ListIter_get_val(iter));
    /* printf("%p : [%d] obj= %p obj->impl=%p\n", env_obj, i, child_env_obj, child_env_obj->impl); */
    Lisp_printenv2(child_env_obj);
    printf("--- } Object<Environment>(%p) child %d } ---\n", env_obj, i);
    ListIter_next(iter);
    i++;
  }
  ListIter_del(iter);
  printf("--- } Object<Environment>(%p) Environment(%p) } ---\n", env_obj, env_obj->impl);
}

void Lisp_printenv() {
  Lisp_printenv2(LispEnv_root);
}
