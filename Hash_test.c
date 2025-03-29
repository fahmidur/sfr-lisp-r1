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
  Hash_set(h1, apple, red);
  nassert(Hash_len(h1) == 1);

  Object* apple_color = NULL;
  apple_color = Hash_get(h1, apple);
  nassert(apple_color == red);
  nassert(Object_cmp(apple_color, red) == 0);

  //manual cleanup
  Hash_del(h1);

  Object_system_print();
  Object_system_done();
  Symbol_system_done();

  nassert_report();
  return nassert_exit_code();
}
