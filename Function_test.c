#include "Function.h"
#include "Error.h"
#include <stdio.h>
#include <stdlib.h>
#include "Object.h"
#include "nassert.h"

Object* fn_dprint(Function* fn, Object* argv) {
  printf("fn_dprintf. called");
  return NULL;
}

int main(int argc, char** argv) {
  ErrorSystem_init();
  Symbol_system_init();
  Object_system_init();
  FunctionSystem_init();

  Function* fn1 = Function_new(fn_dprint, -1, NULL);

  printf("function fn1 = ");
  Function_print(fn1);
  printf("\n");

  FunctionSystem_done();
  Object_system_done();
  Symbol_system_done();
  ErrorSystem_done();
  return nassert_exit_code();
}

