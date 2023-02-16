#include <stdio.h>
#include <stdlib.h>
#include "Symbol.h"
#include "String.h"
#include "Number.h"
#include "List.h"
#include "nassert.h"

int main(int argc, char** argv) {
  List* list1 = List_new();
  nassert(List_size(list1) == 0);

  String* hello = String_new("Hello");
  nassert(strcmp(hello->buf, "Hello") == 0);

  List_push(list1, hello);
  nassert(List_size(list1) == 1);

  String* hello_popped = List_pop(list1);
  nassert(hello_popped == hello);
  nassert(List_size(list1) == 0);

  //--- cleanup
  String_del(hello);
  List_del(list1);

  nassert_report();
  return nassert_exit_code();
}
