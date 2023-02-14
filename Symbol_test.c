#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Symbol.h"

int main(int argc, char** argv) {
  printf("--- Symbol test ---\n");
  
  Symbol_system_init();
  Symbol_system_print();

  Symbol* apple1 = Symbol_new("apple");
  Symbol* apple2 = Symbol_new("apple");
  nassert(apple1 == apple2);
  nassert(Symbol_cmp(apple1, apple2) == 0);

  Symbol* orange = Symbol_new("orange");
  nassert(apple1 != orange);
  nassert(apple2 != orange);
  nassert(Symbol_cmp(apple1, orange) != 0);
  nassert(Symbol_cmp(apple2, orange) != 0);

  Symbol* dolly1 = Symbol_new("dolly");
  Symbol* dolly2 = Symbol_clone(dolly1);
  nassert(dolly1 == dolly2);
  nassert(Symbol_cmp(dolly1, dolly2) == 0);

  Symbol_system_print();
  Symbol_system_done();

  nassert_report();
  return nassert_exit_code();
}
