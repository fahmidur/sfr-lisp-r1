#include <stdio.h>
#include <stdlib.h>
#include "Symbol.h"
#include "String.h"
#include "Number.h"
#include "List.h"
#include "nassert.h"
#include "Object.h"

int main(int argc, char** argv) {
  Symbol_system_init();
  Object_system_init();

  List* list1 = List_new();
  nassert(List_size(list1) == 0);

  Symbol* SYMBOL_STRING = Symbol_new("String");
  Object* hstr1 = Object_new(SYMBOL_STRING, 1, String_new("Hello"));

  List_push(list1, hstr1);
  nassert(List_size(list1) == 1);
  printf("--- list1 = "); List_print(list1); printf("\n");

  printf("Calling list1.pop() ...\n");
  Object* hstr1_popped = Object_accept(List_pop(list1));
  nassert(hstr1_popped == hstr1);
  nassert(hstr1_popped->returning == 0);
  nassert(List_size(list1) == 0);
  printf("--- list1 = "); List_print(list1); printf("\n");

  Object* listobj = Object_new(Symbol_new("List"), 1, List_new());
  List_push(listobj->impl, hstr1);
  printf("--- listobj = "); Object_print(listobj); printf("\n");
  nassert(hstr1->rc == 3);

  Object* str1 = Object_new(SYMBOL_STRING, 1, String_new("str1"));
  Object* str2 = Object_new(SYMBOL_STRING, 1, String_new("str2"));
  Object* str3 = Object_new(SYMBOL_STRING, 1, String_new("str3"));

  List_push(listobj->impl, str1);
  List_push(listobj->impl, str2);
  List_push(listobj->impl, str3);

  nassert(List_size(listobj->impl) == 4);
  printf("--- listobj = "); Object_print(listobj); printf("\n");

  Object* str4 = Object_new(SYMBOL_STRING, 1, String_new("str4"));
  Object* str5 = Object_new(SYMBOL_STRING, 1, String_new("str5"));
  Object* str6 = Object_new(SYMBOL_STRING, 1, String_new("str6"));

  printf("calling listobj.unshift(str4) ...\n");
  List_unshift(listobj->impl, str4);
  
  nassert(List_size(listobj->impl) == 5);
  printf("--- listobj = "); Object_print(listobj); printf("\n");

  printf("calling listobj.shift() ...\n");
  Object* str4_shifted = Object_accept(List_shift(listobj->impl));
  printf("--- listobj = "); Object_print(listobj); printf("\n");
  nassert(str4_shifted == str4);
  nassert(str4_shifted->returning == 0);

  printf("calling listobj.at(1) ...\n");
  Object* atx = Object_accept(List_at(listobj->impl, 1));
  printf("atx = "); Object_print(atx); printf("\n");
  nassert(atx == str1);

  List* list2 = List_new();
  List_push(list2, str1);
  List_push(list2, str2);
  List_push(list2, str3);
  printf("list2 ="); List_print(list2); printf("\n");
  nassert(List_len(list2) == 3);
  printf("List_zero(list2)\n");
  List_zero(list2);
  printf("list2 ="); List_print(list2); printf("\n");
  nassert(List_len(list2) == 0);

  //--- cleanup
  Object_system_print();
  Object_system_done();
  Symbol_system_done();
  List_del(list1);

  nassert_report();
  return nassert_exit_code();
}
