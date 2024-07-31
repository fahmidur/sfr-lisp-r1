#include "Function.h"
#include "Error.h"
#include <stdio.h>
#include <stdlib.h>
#include "Object.h"
#include "nassert.h"

Object* fn_println(Function* fn, Object* argv) {
  printf("fn_println. Called\n");
  ObjectUtil_eprintf("fn_println. argv=%v\n", argv);
  ObjectUtil_eprintf("fn_println. len(argv)=%d\n", Object_len(argv));
  int i = 0;
  int argv_len = Object_len(argv);
  Object* iter;
  Symbol* iter_type;
  for(i = 1; i < argv_len; i++) {
    iter = Object_bop_at(argv, i);
    iter_type = Object_type(iter);
    Object_print(Object_bop_at(argv, i));
  }
  printf("\n");
  return NULL;
}

Object* fn_add(Function* fn, Object* argv) {
  return Object_bop_add(Object_bop_at(argv, 1), Object_bop_at(argv, 2));
}

int main(int argc, char** argv) {
  ErrorSystem_init();
  Symbol_system_init();
  Object_system_init();
  FunctionSystem_init();

  printf("Constructing fn1 ...\n");
  Function* fn1 = Function_new(NULL, fn_println, -1, NULL, NULL);
  printf("fn1 constructed\n");

  printf("function fn1 = ");
  Function_print(fn1);
  printf("\n");

  printf("calling fn1...\n");
  Function_call(fn1, NULL);

  Function_call(fn1, Object_new_list(1, 2, QSYMBOL("print"), QSTRING("Hello there")));

  printf("Constructing fn2 ...\n");
  Function* fn2 = Function_new(NULL, fn_add, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL);
  printf("fn2 constructed\n");

  printf("function fn2 = ");
  Function_print(fn2);
  printf("\n");

  Object* num1 = QNUMBER(2);
  Object* num2 = QNUMBER(3);
  Object* res1 = Object_accept(Function_call(fn2, Object_new_list(1, 3, QSYMBOL("+"), num1, num2)));
  ObjectUtil_eprintf("%v + %v = %v\n", num1, num2, res1);

  // manually delete each function
  Function_del(fn1);
  Function_del(fn2);

  FunctionSystem_done();
  Object_system_done();
  Symbol_system_done();
  ErrorSystem_done();
  return nassert_exit_code();
}

