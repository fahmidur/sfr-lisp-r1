#include "Function.h"
#include "Error.h"
#include <stdio.h>
#include <stdlib.h>
#include "Object.h"
#include "nassert.h"

Object* fn_println(Function* fn, Object* env, Object* argv) {
  assert(fn != NULL);
  assert(argv != NULL);
  if(Object_is_null(argv)) {
    printf("\n");
    return NULL;
  }
  assert(Object_type(argv) == SYMBOL_LIST);
  /* printf("fn_println. Called\n"); */
  /* ObjectUtil_eprintf("fn_println. argv=%v\n", argv); */
  /* ObjectUtil_eprintf("fn_println. len(argv)=%d\n", Object_len(argv)); */
  int argv_len = Object_len(argv);
  Object* tmp;
  Symbol* tmp_type;
  int i = 0;
  ListIter* argv_iter = ListIter_new(argv->impl);
  ListIter_next(argv_iter);
  ListIter_next(argv_iter);
  while(!ListIter_at_end(argv_iter)) {
    tmp = ListIter_get_val(argv_iter);
    /* ObjectUtil_eprintf("debug. tmp=%v\n", tmp); */
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
  printf("\n");
  return NULL;
}

Object* fn_add(Function* fn, Object* env, Object* argv) {
  // This Function was defined with named-params, and so we should be able to
  // 'a' and 'b' from the environment.
  Object* a = Object_bop_hget(env, QSYMBOL("a"));
  Object* b = Object_bop_hget(env, QSYMBOL("b"));
  ObjectUtil_eprintf("fn_add. got a = %v\n", a);
  ObjectUtil_eprintf("fn_add. got b = %v\n", b);
  return Object_bop_add(a, b);
}

Function* fn_make_adder(Function* fn, Object* argv) {
  return NULL;
}

int main(int argc, char** argv) {
  ErrorSystem_init();
  Symbol_system_init();
  Object_system_init();
  FunctionSystem_init();

  printf("Constructing fn1 ...\n");
  Function* fn1 = Function_new(QSYMBOL("println"), NULL, fn_println, -1, NULL, NULL);
  printf("fn1 constructed\n");
  nassert(fn1 != NULL);
  // a function 'has' an Environment env
  nassert(fn1->env != NULL); 

  printf("function fn1 = ");
  Function_print(fn1);
  printf("\n");

  printf("calling fn1...\n");
  Function_call(fn1, NULL);

  Function_call(fn1, Object_new_list(1, 2, QSYMBOL("println"), QSTRING("Hello there")));
  Function_call(fn1, Object_new_list(1, 3, QSYMBOL("println"), QNUMBER(3.14), QSTRING("is my favorite number")));
  Function_call(fn1, Object_new_list(1, 3, QSYMBOL("println"), QSTRING("My favorite number is"), QNUMBER(3.14)));

  printf("Constructing fn2 ...\n");
  Function* fn2 = Function_new(QSYMBOL("add"), NULL, fn_add, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL);
  printf("fn2 constructed\n");

  Object* res1 = Object_accept(Function_call(fn2, Object_new_list(1, 3, QSYMBOL("add"), QNUMBER(2.1), QNUMBER(3.1))));
  ObjectUtil_eprintf("res1 = %v\n", res1);

  /* printf("function fn2 = "); */
  /* Function_print(fn2); */
  /* printf("\n"); */

  /* Object* num1 = QNUMBER(2); */
  /* Object* num2 = QNUMBER(3); */
  /* Object* res1 = Object_accept(Function_call(fn2, Object_new_list(1, 3, QSYMBOL("+"), num1, num2))); */
  /* ObjectUtil_eprintf("%v + %v = %v\n", num1, num2, res1); */

  // now let's try closures

  // manually delete each function
  Function_del(fn1);
  Function_del(fn2);

  FunctionSystem_done();
  Object_system_done();
  Symbol_system_done();
  ErrorSystem_done();

  nassert_report();
  return nassert_exit_code();
}

