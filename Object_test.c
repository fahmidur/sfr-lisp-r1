#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Object.h"

int main(int argc, char** argv) {
  Symbol_system_init();
  Object_system_init();

  printf("=========================\n");

  Object* str1 = Object_new(Symbol_new("String"), String_new("Hello there 001"));
  Object* str2 = Object_new(Symbol_new("String"), String_new("Hello there 002"));
  Object* str3 = Object_new(Symbol_new("String"), String_new("Hello there 003"));
  
  printf("=========================\n");

  Object_system_done();
  Symbol_system_done();

  nassert_report();
  return nassert_exit_code();
}
