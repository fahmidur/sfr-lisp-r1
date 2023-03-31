#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Symbol.h"
#include "String.h"
#include "Number.h"
#include "Object.h"

void heading(char isbeg, char* str) {
  if(isbeg) {
    printf("{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{\n");
  } else {
    printf("}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}\n");
  }
  printf("=== %s. %s \n", (isbeg ? "BEG" : "END"), str);
  if(isbeg) {
    printf("{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{\n");
  } else {
    printf("}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}\n");
    printf("\n\n");
  }
}

int main(int argc, char** argv) {

  heading(1, "RUNTIME INIT");
  Symbol_system_init();
  Object_system_init();
  heading(0, "RUNTIME INIT");

  //===========================================================================

  nassert(Symbol_new("Symbol") == SYMBOL_SYMBOL);
  nassert(Symbol_new("Number") == SYMBOL_NUMBER);
  nassert(Symbol_new("String") == SYMBOL_STRING);
  nassert(Symbol_new("Error") == SYMBOL_ERROR);
  nassert(Symbol_new("List") == SYMBOL_LIST);

  //===========================================================================
  heading(1, "SYMBOL OPERATIONS");

  Object* sym1 = QSYMBOL_NEW1("hammer_and_sickle");
  nassert(Object_type(sym1) == SYMBOL_SYMBOL);

  heading(0, "SYMBOL OPERATIONS");

  //===========================================================================
  heading(1, "STRING OPERATIONS");

  int objsize = Object_system_size();
  Object* str1 = Object_new(SYMBOL_STRING, 1, String_new("Hello there 001"));
  Object* str2 = Object_new(SYMBOL_STRING, 1, String_new("Hello there 002"));
  Object* str3 = Object_new(SYMBOL_STRING, 1, String_new("Hello there 003"));
  nassert(objsize - Object_system_size() >= 3); // gained at least 3 objects
  Object_system_gc();
  nassert(objsize - Object_system_size() >= 3);
  Object* tmp1 = Object_new(SYMBOL_STRING, 0, String_new("This too will pass."));
  tmp1->returning = 0; // manually mark it as no longer returning
  nassert(tmp1->rc == 0);
  nassert(objsize - Object_system_size() >= 4);
  objsize = Object_system_size();
  Object_system_gc();
  nassert(objsize - Object_system_size() == 1);

  Object* tmp2 = Object_new(SYMBOL_STRING, 1, String_new("So long and thanks for all the fish."));
  /*printf("BEF. tmp2 = "); Object_print(tmp2); printf("\n");*/
  ObjectUtil_eprintf("BEF. tmp2 = %v\n", tmp2);
  tmp2 = Object_rc_decr(tmp2);
  /*printf("AFT. tmp2 = "); Object_print(tmp2); printf("\n");*/
  ObjectUtil_eprintf("AFT. tmp2 = %v\n", tmp2);

  Object* stralike1 = Object_new(SYMBOL_STRING, 1, String_new("John Doe"));
  Object* stralike2 = Object_new(SYMBOL_STRING, 1, String_new("John Doe"));
  Object* stralike3 = Object_new(SYMBOL_STRING, 1, String_new("John Doo"));

  nassert(Object_cmp(str1, str2) != 0);
  nassert(Object_cmp(stralike1, stralike2) == 0);
  nassert(Object_cmp(stralike2, stralike3) != 0);

  Object* str_hello = Object_new(SYMBOL_STRING, 1, String_new("Hello"));
  Object* str_there = Object_new(SYMBOL_STRING, 1, String_new("There"));
  Object* str_conc1 = Object_accept(Object_bop_add(str_hello, str_there));
  Object* str_conc1_exp = Object_new(SYMBOL_STRING, 1, String_new("HelloThere"));
  nassert(Object_cmp(str_conc1, str_conc1_exp) == 0);

  Object* qstr1 = QSTRING_NEW1("the quick brown fox");
  nassert(Object_type(qstr1) == SYMBOL_STRING);
  /*Object_print(qstr1); printf("\n");*/
  ObjectUtil_eprintf("qstr1 = %v\n", qstr1);

  heading(0, "STRING OPERATIONS");

  //===========================================================================

  heading(1, "NUMBER OPERATIONS");

  Object* num1 = Object_new(SYMBOL_NUMBER, 1, Number_new(3));
  Object* num2 = Object_new(SYMBOL_NUMBER, 1, Number_new(4));

  printf("Test Number + Number ...\n");
  Object* res1 = Object_accept(Object_bop_add(num1, num2));
  ObjectUtil_eprintf("%v + %v = %v\n", num1, num2, res1);
  Object* res1_expected = Object_new(SYMBOL_NUMBER, 1, Number_new(7));
  nassert(Object_cmp(res1, res1_expected) == 0);
  /*printf("res1 = "); Object_print(res1); printf("\n");*/
  ObjectUtil_eprintf("res1 = %v\n", res1);

  printf("Test Number - Number ...\n");
  Object* res2 = Object_accept(Object_bop_sub(num1, num2));
  ObjectUtil_eprintf("%v - %v = %v\n", num1, num2, res2);
  Object* res2_expected = Object_new(SYMBOL_NUMBER, 1, Number_new(-1));
  nassert(Object_cmp(res2, res2_expected) == 0);

  printf("Test Number * Number ...\n");
  Object* res3 = Object_accept(Object_bop_mul(num1, num2));
  ObjectUtil_eprintf("%v * %v = %v\n", num1, num2, res3);
  Object* res3_expected = Object_new(SYMBOL_NUMBER, 1, Number_new(12));
  nassert(Object_cmp(res3, res3_expected) == 0);

  printf("Test Number / Number ...\n");
  Object* res4 = Object_accept(Object_bop_div(num1, num2));
  ObjectUtil_eprintf("%v / %v = %v\n", num1, num2, res4);
  Object* res4_expected = Object_new(SYMBOL_NUMBER, 1, Number_new(0.75));
  nassert(Object_cmp(res4, res4_expected) == 0);

  printf("Test String + Number ...\n");
  Object* sum_invalid = Object_accept(Object_bop_add(str1, num2));
  ObjectUtil_eprintf("%v + %v = %v\n", str1, num2, sum_invalid);
  Object_print(sum_invalid); printf("\n");
  nassert(Object_type(sum_invalid) == Symbol_new("Error"));

  Object* qnum1 = QNUMBER_NEW1(3.14);
  nassert(Object_type(qnum1) == SYMBOL_NUMBER);
  
  heading(0, "NUMBER OPERATIONS");

  //===========================================================================
  
  heading(1, "LIST OPERATIONS");

  // List.push
  Object* list1 = Object_new(SYMBOL_LIST, 1, List_new());
  Object* member1 = Object_new(SYMBOL_NUMBER, 1, Number_new_from_double(5.0));
  Object* member2 = Object_new(SYMBOL_NUMBER, 1, Number_new_from_double(6.0));
  Object_reject(Object_bop_push(list1, member1));
  Object_reject(Object_bop_push(list1, member2));
  printf("list1 = "); Object_print(list1); printf("\n");
  nassert(Object_len(list1) == 2);

  Object* list2 = Object_new(SYMBOL_LIST, 1, List_new());
  Object_reject(Object_bop_push(list2, member1));
  Object_reject(Object_bop_push(list2, member2));
  printf("list2 = "); Object_print(list2); printf("\n");
  printf("--- again with eprintf ---\n");
  ObjectUtil_eprintf("list2 = %v\n", list2);
  ObjectUtil_eprintf("list2, which equals %v, is great!\n", list2);
  ObjectUtil_eprintf("list2, which equals %v, contains %d elements.\n", list2, Object_len(list2));

  printf("popping from list2 into popres1\n");
  Object* popres1 = Object_accept(Object_uop_pop(list2));
  nassert(Object_is_error(popres1) == 0);
  nassert(Object_is_null(popres1) == 0);
  ObjectUtil_eprintf("list2 = %v AND popres1=%v\n", list2, popres1);
  nassert(Object_len(list2) == 1);
  nassert(Object_cmp(popres1, member2) == 0);
  nassert(Object_cmp(popres1, member1) != 0);

  heading(0, "LIST OPERATIONS");

  //===========================================================================

  heading(1, "RUNTIME DONE");
  Object_system_print();
  Object_system_done();
  Symbol_system_done();
  heading(0, "RUNTIME DONE");

  nassert_report();
  return nassert_exit_code();
}
