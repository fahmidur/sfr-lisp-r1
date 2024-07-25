#include "Function.h"
#include "Error.h"
#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"

Object* fn_dprint(Function* fn, Object* args) {
  printf("fn_dprintf. called");
  return NULL;
}

int main(int argc, char** argv) {
  ErrorSystem_init();
  Symbol_system_init();
  Object_system_init();

  FunctionEnv* env = FunctionEnv_new(NULL);
  /* Function* obj = Function_new(fn_dprint, -1, env); */

  Function_env_del(env);

  Object_system_done();
  Symbol_system_done();
  ErrorSystem_done();
  return nassert_exit_code();
}

