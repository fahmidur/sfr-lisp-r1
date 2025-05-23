#include <stdio.h>
#include <stdlib.h>
#include "Error.h"
#include "String.h"
#include "nassert.h"

int main(int argc, char** argv) {
  ErrorSystem_init();

  String* str0 = String_new("");
  nassert(strcmp("", str0->buf) == 0);
  nassert(str0->len == 0);
  nassert(String_len(str0) == 0);

  String* str1 = String_new("Hello");
  nassert(strcmp("Hello", str1->buf) == 0);
  nassert(str1->len == 5);
  nassert(String_len(str1) == 5);
  String_zero(str1);
  nassert(str1->len == 0);
  nassert(String_len(str1) == 0);

  String* str2 = String_new("Hello\n\n");
  String_chomp(str2);
  nassert(strcmp("Hello\n", str2->buf) == 0);

  String* dolly1 = String_new("dolly");
  String* dolly2 = String_clone(dolly1);
  nassert(strcmp(dolly1->buf, dolly2->buf) == 0);

  String* hello = String_new("Hello");
  String* there = String_new("There");
  String* conc1 = String_add(hello, there);
  printf("conc1->buf = %s\n", conc1->buf);
  nassert(strcmp(conc1->buf, "HelloThere") == 0);

  String* hello2 = String_new("General");
  String* there2 = String_new("Kenobi");
  String_addx(hello2, there2);
  printf("hello2->buf = %s\n", hello2->buf);
  nassert(strcmp(hello2->buf, "GeneralKenobi") == 0);

  String* target1 = String_new("abc");
  String_addx_char(target1, 'd');
  printf("target1->buf = %s\n", target1->buf);
  nassert(String_len(target1) == 4);
  nassert(strcmp(target1->buf, "abcd") == 0);
  int i;
  char buf[] = "efghi";
  for(i = 0; i < 5; i++) {
    String_addx_char(target1, buf[i]);
  }
  printf("target1->buf = %s\n", target1->buf);
  nassert(String_len(target1) == 9);
  nassert(strcmp(target1->buf, "abcdefghi") == 0);

  String* numeric1 = String_new("1337.5");
  nassert(String_to_double(numeric1) == 1337.5);
  nassert(String_to_float(numeric1) == 1337.5f);
  nassert(String_to_long(numeric1) == 1337L);
  nassert(String_to_int(numeric1) == 1337);

  String* numeric2 = String_new("-217.15");
  nassert(String_to_double(numeric2) == -217.15);
  nassert(String_to_float(numeric2) == -217.15f);
  nassert(String_to_long(numeric2) == -217L);
  nassert(String_to_int(numeric2) == -217);

  String* single_char = String_new("x");
  nassert(String_to_char(single_char) == 'x');

  char success = String_zero(NULL);
  nassert(success == 0);
  nassert(ErrorSystem_get_code() != 0);

  String_del(str0);
  String_del(str1);
  String_del(str2);
  String_del(dolly1);
  String_del(dolly2);
  String_del(hello);
  String_del(there);
  String_del(conc1);
  String_del(hello2);
  String_del(there2);
  String_del(target1);
  String_del(numeric1);
  String_del(numeric2);
  String_del(single_char);

  ErrorSystem_done();

  nassert_report();
  return nassert_exit_code();
}

