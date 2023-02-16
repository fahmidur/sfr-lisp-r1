#include <stdio.h>
#include <stdlib.h>
#include "List.h"
#include "nassert.h"

int main(int argc, char** argv) {
  List* list1 = List_new();

  nassert_report();
  return nassert_exit_code();
}
