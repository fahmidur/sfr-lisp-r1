#include <stdio.h>
#include "Object.h"
#include "nassert.h"
#include "Lisp.h"

int main(int argc, char** argv) {

  Runtime_init();
  Lisp_init();

  printf("\n=== === tc1 === ===\n");
  Object* tc1_string = QSTRING_NEW1("(+ 2.718 3.141)");
  Object* tc1_tokens_got = Object_accept(Lisp_tokenize(tc1_string));
  Object* tc1_tokens_exp = Object_new_list(1, 5, QSYMBOL("("), QSYMBOL("+"), QNUMBER(2.718), QNUMBER(3.141), QSYMBOL(")"));
  nassert_obj_eq(tc1_tokens_got, tc1_tokens_exp);
  Object* tc1_parsed = Object_accept(Lisp_parse_tokens(tc1_tokens_got));
  ObjectUtil_eprintf("tc1_parsed = %v\n", tc1_parsed);
  /* Object* tc1_value = Lisp_eval_sexp(tc1_parsed); */

  printf("\n=== === tc1_2 === ===\n");
  Object* tc1_2_string = QSTRING_NEW1("(+ 2.718 (* 2 3.141))");
  Object* tc1_2_tokens_got = Object_accept(Lisp_tokenize(tc1_2_string));
  Object* tc1_2_tokens_exp = Object_new_list(1, 9, QSYMBOL("("), QSYMBOL("+"), QNUMBER(2.718), QSYMBOL("("), QSYMBOL("*"), QNUMBER(2.0), QNUMBER(3.141), QSYMBOL(")"), QSYMBOL(")"));
  nassert_obj_eq(tc1_2_tokens_got, tc1_2_tokens_exp);
  Object* tc1_2_parsed_got = Object_accept(Lisp_parse_tokens(tc1_2_tokens_got));
  ObjectUtil_eprintf("tc1_2_parsed_got = %v\n", tc1_2_parsed_got);
  Object* tc1_2_parsed_exp = Object_new_list(1, 3, QSYMBOL("+"), QNUMBER(2.718), Object_new_list(1, 3, QSYMBOL("*"), QNUMBER(2.0), QNUMBER(3.141)));
  ObjectUtil_eprintf("tc1_2_parsed_exp = %v\n", tc1_2_parsed_exp);

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

  printf("\n=== === tc6 === ===\n");
  Object* tc6_string = QSTRING_NEW1(") invalid1 ()");
  ObjectUtil_eprintf("tc6_string = %v\n", tc6_string);
  Object* tc6_tokens = Object_accept(Lisp_tokenize(tc6_string));
  ObjectUtil_eprintf("tc6_tokens = %v\n", tc6_tokens);
  Object* tc6_parsed = Object_accept(Lisp_parse_string(tc6_string));
  ObjectUtil_eprintf("tc6_string  = %v\n", tc6_string);
  ObjectUtil_eprintf("tc6_parsed = %v\n", tc6_parsed);
  nassert(Object_is_error(tc6_parsed));

  Lisp_done();
  Runtime_done();

  //----
  nassert_report();
  return nassert_exit_code();
}
