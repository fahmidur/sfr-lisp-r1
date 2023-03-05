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
  Object* hello = Object_new(SYMBOL_STRING, 1, String_new("Hello"));

  List_push(list1, hello);
  nassert(List_size(list1) == 1);

  Object* hello_popped = List_pop(list1);
  nassert(hello_popped == hello);
  nassert(List_size(list1) == 0);

  Object* listobj = Object_new(Symbol_new("List"), 1, List_new());
  List_push(listobj->impl, hello);
  printf("--- listobj = "); Object_print(listobj); printf("\n");

  Object* str1 = Object_new(SYMBOL_STRING, 1, String_new("str1"));
  Object* str2 = Object_new(SYMBOL_STRING, 1, String_new("str2"));
  Object* str3 = Object_new(SYMBOL_STRING, 1, String_new("str3"));

  List_push(listobj->impl, str1);
  List_push(listobj->impl, str2);
  List_push(listobj->impl, str3);

  nassert(List_size(listobj->impl) == 4);

  printf("--- listobj = "); Object_print(listobj); printf("\n");

  //--- cleanup
  Object_system_done();
  Symbol_system_done();
  List_del(list1);

  nassert_report();
  return nassert_exit_code();
}
