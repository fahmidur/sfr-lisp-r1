#include "Function.h"
#include "Error.h"
#include <stdio.h>
#include <stdlib.h>
#include "Object.h"
#include "nassert.h"

Object* fn_dprint(Function* fn, Object* argv) {
  printf("fn_dprintf. Called\n");
  ObjectUtil_eprintf("fn_dprintf. argv=%v\n", argv);
  ObjectUtil_eprintf("fn_dprintf. len(argv)=%d\n", Object_len(argv));
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
  Function* fn1 = Function_new(NULL, fn_dprint, -1, NULL, NULL);
  printf("fn1 constructed\n");

  printf("function fn1 = ");
  Function_print(fn1);
  printf("\n");

  printf("calling fn1...\n");
  Function_call(fn1, NULL);

  printf("Constructing fn2 ...\n");
  Function* fn2 = Function_new(NULL, fn_add, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL);
  printf("fn2 constructed\n");

  printf("function fn2 = ");
  Function_print(fn2);
  printf("\n");

  Object* num1 = QNUMBER(2);
  Object* num2 = QNUMBER(3);
  Object* res1 = Function_call(fn2, Object_new_list(1, 3, QSYMBOL("+"), num1, num2));
  ObjectUtil_eprintf("%v + %v = %v\n", num1, num2, res1);

  FunctionSystem_done();
  Object_system_done();
  Symbol_system_done();
  ErrorSystem_done();
  return nassert_exit_code();
}

