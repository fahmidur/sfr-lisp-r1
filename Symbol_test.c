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

  Symbol* orange = Symbol_new("orange");
  nassert(apple1 != orange);

  Symbol* dolly1 = Symbol_new("dolly");
  Symbol* dolly2 = Symbol_clone(dolly1);
  nassert(dolly1 == dolly2);

  Symbol_system_print();
  Symbol_system_done();

  nassert_report();
  return nassert_exit_code();
}
