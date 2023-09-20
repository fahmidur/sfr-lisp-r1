#include <stdio.h>
#include "nassert.h"
#include "Lisp.h"

int main(int argc, char** argv) {

  Runtime_init();

  // TODO: add tests here

  /*Object* tc1_string = QSTRING_NEW1("(+ 2.718 3.141)");*/
  /*Object* tc1_tokens = Object_accept(Lisp_tokenize(tc1_string));*/
  /*ObjectUtil_eprintf("tc1_tokens = %v\n", tc1_tokens);*/

  /*Object* tc2_string = QSTRING_NEW1("92.3");*/
  /*Object* tc2_tokens = Object_accept(Lisp_tokenize(tc2_string));*/
  /*ObjectUtil_eprintf("tc2_tokens = %v\n", tc2_tokens);*/

  printf("\n=== === tc3 === ===\n");
  Object* tc3_string = QSTRING_NEW1("(- 123 23)");
  Object* tc3_tokens = Object_accept(Lisp_tokenize(tc3_string));
  ObjectUtil_eprintf("tc3_tokens = %v\n", tc3_tokens);

  printf("\n=== === tc4 ===  ===\n");
  Object* tc4_string = QSTRING_NEW1("(+ 5 -9)");
  Object* tc4_tokens = Object_accept(Lisp_tokenize(tc4_string));
  ObjectUtil_eprintf("tc4_tokens = %v\n", tc4_tokens);

  Runtime_done();

  //----
  nassert_report();
  return nassert_exit_code();
}
