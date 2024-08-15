#include <stdio.h>
#include <stdlib.h>
#include "Environment.h"
#include "Error.h"
#include "Object.h"
#include "nassert.h"


int main(int argc, char** argv) {
  ErrorSystem_init();
  Symbol_system_init();
  Object_system_init();

  Object* apple = QSYMBOL_NEW1("apple");
  Object* red = QSTRING_NEW1("red");

  Object* grape = QSYMBOL_NEW1("grape");
  Object* purple = QSTRING_NEW1("purple");

  Object* banana = QSYMBOL_NEW1("banana");
  Object* yellow = QSTRING_NEW1("yellow");

  Object* green = QSTRING_NEW1("green");

  Environment* env1 = Environment_new();

  printf("env1 = ");
  Environment_print(env1);
  printf("\n");

  Environment_set(env1, apple, red);
  Environment_set(env1, grape, purple);

  nassert(
    Object_cmp(
      Environment_get(env1, apple),
      red
    ) == 0
  );

  nassert(Environment_len(env1) == 2);

  Environment* env2 = Environment_new();
  printf("env2 = ");
  Environment_print(env2);
  printf("\n");

  Object* env1_obj = Object_new(SYMBOL_ENVIRONMENT, 1, env1);
  Object* env2_obj = Object_new(SYMBOL_ENVIRONMENT, 1, env2);
  nassert(env1_obj->rc == 1);
  nassert(env2_obj->rc == 1);

  // env2 is now a child of env1
  Environment_child_attach(env1_obj, env2_obj);
  nassert(env1_obj->rc == 2);
  nassert(env2_obj->rc == 2);
  // refcount of each env increases by 1 because they point at each other.

  Environment_set(env2, banana, yellow);
  nassert(
    Object_cmp(
      Environment_get(env2, apple),
      red
    ) == 0
  );

  // test shadowing
  Environment_set(env2, apple, green);
  nassert(green->rc == 2);
  nassert(
    Object_cmp(
      Environment_get(env2, apple),
      green
    ) == 0
  );
  nassert(
    Object_cmp(
      Environment_get(env1, apple),
      red
    ) == 0
  );

  // now we remove the shadowing variable
  Environment_rem(env2, apple);
  nassert(
    Object_cmp(
      Environment_get(env2, apple),
      red
    ) == 0
  );

  /* printf("deleting environments ... \n"); */
  /* printf("deleting env2 ... \n"); */
  /* Environment_del(env2); */

  /* printf("deleting env1 ... \n"); */
  /* Environment_del(env1); */
  /* printf("deleting environments ... DONE\n"); */

  Object_system_done();
  Symbol_system_done();
  ErrorSystem_done();

  nassert_report();
  return nassert_exit_code();
}
