#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "nassert.h"

int main(int argc, char** argv) {
  String* str0 = String_new("");
  nassert(strcmp("", str0->buf) == 0);
  nassert(str0->len == 0);
  nassert(String_len(str0) == 0);

  String* str1 = String_new("Hello");
  nassert(strcmp("Hello", str1->buf) == 0);
  nassert(str1->len == 5);
  nassert(String_len(str1) == 5);

  String_del(str0);
  String_del(str1);

  nassert_report();
  return nassert_exit_code();
}

