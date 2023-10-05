#include <stdio.h>
#include "nassert.h"
#include "Lisp.h"

int main(int argc, char** argv) {

  Runtime_init();

  // TODO: add tests here

  printf("\n=== === tc1 === ===\n");
  Object* tc1_string = QSTRING_NEW1("(+ 2.718 3.141)");
  Object* tc1_tokens_got = Object_accept(Lisp_tokenize(tc1_string));
  Object* tc1_tokens_exp = Object_new_list(1, 5, QSYMBOL("("), QSYMBOL("+"), QNUMBER(2.718), QNUMBER(3.141), QSYMBOL(")"));
  nassert_obj_eq(tc1_tokens_got, tc1_tokens_exp);

  printf("\n=== === tc2 === ===\n");
  Object* tc2_string = QSTRING_NEW1("92.3");
  Object* tc2_tokens_got = Object_accept(Lisp_tokenize(tc2_string));
  Object* tc2_tokens_exp = Object_new_list(1, 1, QNUMBER(92.3));
  nassert_obj_eq(tc2_tokens_got, tc2_tokens_exp);

  printf("\n=== === tc3 === ===\n");
  Object* tc3_string = QSTRING_NEW1("(- 123 23)");
  Object* tc3_tokens_got = Object_accept(Lisp_tokenize(tc3_string));
  Object* tc3_tokens_exp = Object_new_list(1, 5, QSYMBOL("("), QSYMBOL("-"), QNUMBER(123), QNUMBER(23), QSYMBOL(")"));
  nassert_obj_eq(tc3_tokens_got, tc3_tokens_exp);

  printf("\n=== === tc4 ===  ===\n");
  Object* tc4_string = QSTRING_NEW1("(+ 5 -9)");
  Object* tc4_tokens_got = Object_accept(Lisp_tokenize(tc4_string));
  Object* tc4_tokens_exp = Object_new_list(1, 5, QSYMBOL("("), QSYMBOL("+"), QNUMBER(5), QNUMBER(-9), QSYMBOL(")"));
  nassert_obj_eq(tc4_tokens_got, tc4_tokens_exp);

  printf("\n=== === tc5 === ===\n");
  Object* tc5_string = QSTRING_NEW1("(print \"hello world\")");
  Object* tc5_tokens_got = Object_accept(Lisp_tokenize(tc5_string));
  Object* tc5_tokens_exp = Object_new_list(1, 4, QSYMBOL("("), QSYMBOL("print"), QSTRING("hello world"), QSYMBOL(")"));
  nassert_obj_eq(tc5_tokens_got, tc5_tokens_exp);

  Runtime_done();

  //----
  nassert_report();
  return nassert_exit_code();
}
