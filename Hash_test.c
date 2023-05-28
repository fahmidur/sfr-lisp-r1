#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Symbol.h"
#include "Object.h"
#include "Hash.h"

int main(int argc, char** argv) {
  printf("--- Hash test ---\n");
  Symbol_system_init();
  Object_system_init();


  Object* apple = QSYMBOL_NEW1("apple");
  Object* red = QSTRING_NEW1("red");
  Hash* h1 = Hash_new();
  Hash_kv_set(h1, apple, red);

  Symbol_system_done();
  Object_system_done();

  //manual cleanup
  Hash_del(h1);

  nassert_report();
  return nassert_exit_code();
}
