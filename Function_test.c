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

  FunctionSystem_done();
  Object_system_done();
  Symbol_system_done();
  ErrorSystem_done();
  return nassert_exit_code();
}

