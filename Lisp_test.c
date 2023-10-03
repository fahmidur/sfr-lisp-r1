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
  ObjectUtil_eprintf("tc1_tokens_got = %v\n", tc1_tokens_got);
  ObjectUtil_eprintf("tc1_tokens_exp = %v\n", tc1_tokens_exp);
  nassert(Object_cmp(tc1_tokens_got, tc1_tokens_exp) == 0);

  /*printf("\n=== === tc2 === ===\n");*/
  /*Object* tc2_string = QSTRING_NEW1("92.3");*/
  /*Object* tc2_tokens_got = Object_accept(Lisp_tokenize(tc2_string));*/
  /*Object* tc2_tokens_exp = Object_new_list(1, 1, QNUMBER(92.3));*/
  /*ObjectUtil_eprintf("tc2_tokens_got = %v\n", tc2_tokens_got);*/
  /*ObjectUtil_eprintf("tc2_tokens_exp = %v\n", tc2_tokens_exp);*/
  /*nassert(Object_cmp(tc2_tokens_got, tc2_tokens_exp) == 0);*/

  /*printf("\n=== === tc3 === ===\n");*/
  /*Object* tc3_string = QSTRING_NEW1("(- 123 23)");*/
  /*Object* tc3_tokens = Object_accept(Lisp_tokenize(tc3_string));*/
  /*ObjectUtil_eprintf("tc3_tokens = %v\n", tc3_tokens);*/

  /*printf("\n=== === tc4 ===  ===\n");*/
  /*Object* tc4_string = QSTRING_NEW1("(+ 5 -9)");*/
  /*Object* tc4_tokens = Object_accept(Lisp_tokenize(tc4_string));*/
  /*ObjectUtil_eprintf("tc4_tokens = %v\n", tc4_tokens);*/

  /*printf("\n=== === tc5 === ===\n");*/
  /*Object* tc5_string = QSTRING_NEW1("(print \"hello world\")");*/
  /*ObjectUtil_eprintf("tc5_string = %v\n", tc5_string);*/
  /*Object* tc5_tokens = Object_accept(Lisp_tokenize(tc5_string));*/
  /*ObjectUtil_eprintf("tc5_tokens = %v\n", tc5_tokens);*/

  Runtime_done();

  //----
  nassert_report();
  return nassert_exit_code();
}
