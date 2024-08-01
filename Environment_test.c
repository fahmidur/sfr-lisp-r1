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

  Environment* env1 = Environment_new(NULL);
  Environment_set(env1, QSYMBOL("apple"), QSTRING("red"));
  Environment_set(env1, QSYMBOL("grape"), QSTRING("purple"));

  nassert(
    Object_cmp(
      Environment_get(env1, QSYMBOL("apple")),
      QSTRING("red")
    ) == 0
  );

  nassert(Environment_len(env1) == 2);

  Environment* env2 = Environment_new(env1);

  Environment_set(env2, QSYMBOL("banana"), QSTRING("yellow"));
  nassert(
    Object_cmp(
      Environment_get(env2, QSYMBOL("apple")),
      QSTRING("red")
    ) == 0
  )

  // test shadowing
  Environment_set(env2, QSYMBOL("apple"), QSTRING("green"));
  nassert(
    Object_cmp(
      Environment_get(env2, QSYMBOL("apple")),
      QSTRING("green")
    ) == 0
  )
  nassert(
    Object_cmp(
      Environment_get(env1, QSYMBOL("apple")),
      QSTRING("red")
    ) == 0
  )

  Object_system_done();
  Symbol_system_done();
  ErrorSystem_done();

  nassert_report();
  return nassert_exit_code();
}
