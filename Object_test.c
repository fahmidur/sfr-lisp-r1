#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Symbol.h"
#include "String.h"
#include "Number.h"
#include "Object.h"

int main(int argc, char** argv) {
  Symbol_system_init();
  Object_system_init();

  printf("=========================\n");

  Object* str1 = Object_new(Symbol_new("String"), String_new("Hello there 001"));
  Object* str2 = Object_new(Symbol_new("String"), String_new("Hello there 002"));
  Object* str3 = Object_new(Symbol_new("String"), String_new("Hello there 003"));

  nassert(Object_system_size() == 3);

  Object* num1 = Object_new(Symbol_new("Number"), Number_new(5));
  Object* num2 = Object_new(Symbol_new("Number"), Number_new(7));

  Object* sum1 = Object_bop_add(num1, num2);

  // Basic normalized interface to objects
  /*Object* add = Object_new(Symbol_new("Function"), Function_new(2, 0, &Number_add));*/
  /*Object_call(add, num1, num2);*/
  
  printf("=========================\n");

  Object_system_done();
  Symbol_system_done();

  nassert_report();
  return nassert_exit_code();
}
