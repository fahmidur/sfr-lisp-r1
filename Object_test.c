#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Object.h"

int main(int argc, char** argv) {
  Symbol_system_init();
  Object_system_init();

  Object* str1 = Object_new(Symbol_new("String"), String_new("Hello there"));
  
  Object_system_done();
  Symbol_system_done();

  nassert_report();
  return nassert_exit_code();
}
