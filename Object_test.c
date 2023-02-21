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

  nassert(Symbol_new("Symbol") == SYMBOL_SYMBOL);
  nassert(Symbol_new("Number") == SYMBOL_NUMBER);
  nassert(Symbol_new("String") == SYMBOL_STRING);
  nassert(Symbol_new("Error") == SYMBOL_ERROR);
  nassert(Symbol_new("List") == SYMBOL_LIST);

  Object* str1 = Object_new(Symbol_new("String"), String_new("Hello there 001"));
  Object* str2 = Object_new(Symbol_new("String"), String_new("Hello there 002"));
  Object* str3 = Object_new(Symbol_new("String"), String_new("Hello there 003"));

  nassert(Object_system_size() == 3);

  printf("======================================\n");
  printf("=== === BEG. NUMBER OPERATIONS === ===\n");
  printf("======================================\n");
  Object* num1 = Object_new(SYMBOL_NUMBER, Number_new(3));
  Object* num2 = Object_new(SYMBOL_NUMBER, Number_new(4));

  printf("Test Number + Number ...\n");
  Object* res1 = Object_bop_add(num1, num2);
  Object* res1_expected = Object_new(SYMBOL_NUMBER, Number_new(7));
  nassert(Object_cmp(res1, res1_expected) == 0);
  printf("res1 = "); Object_print(res1); printf("\n");

  printf("Test Number - Number ...\n");
  Object* res2 = Object_bop_sub(num1, num2);
  Object* res2_expected = Object_new(SYMBOL_NUMBER, Number_new(-1));
  nassert(Object_cmp(res2, res2_expected) == 0);

  printf("Test Number * Number ...\n");
  Object* res3 = Object_bop_mul(num1, num2);
  Object* res3_expected = Object_new(SYMBOL_NUMBER, Number_new(12));
  nassert(Object_cmp(res3, res3_expected) == 0);

  printf("Test Number / Number ...\n");
  Object* res4 = Object_bop_div(num1, num2);
  Object* res4_expected = Object_new(SYMBOL_NUMBER, Number_new(0.75));
  nassert(Object_cmp(res4, res4_expected) == 0);

  printf("Test String + Number ...\n");
  Object* sum_invalid = Object_bop_add(str1, num2);
  Object_print(sum_invalid); printf("\n");
  nassert(Object_type(sum_invalid) == Symbol_new("Error"));
  
  printf("======================================\n");
  printf("=== === END. NUMBER OPERATIONS === ===\n");
  printf("======================================\n");
  printf("\n\n");

  Object_system_done();
  Symbol_system_done();

  nassert_report();
  return nassert_exit_code();
}
