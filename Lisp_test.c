#include <stdio.h>
#include "nassert.h"
#include "Lisp.h"

int main(int argc, char** argv) {

  Runtime_init();

  // TODO add tests here
  Object* tc1_string = QSTRING_NEW1("(+ 2 3)");
  Object* tc1_tokens = Object_accept(Lisp_tokenize(tc1_string));

  ObjectUtil_eprintf("tc1_tokens = %v\n", tc1_tokens);

  Object* tc2_string = QSTRING_NEW1("92.3");
  Object* tc2_tokens = Object_accept(Lisp_tokenize(tc2_string));
  ObjectUtil_eprintf("tc2_tokens = %v\n", tc2_tokens);

  Runtime_done();

  //----
  nassert_report();
  return nassert_exit_code();
}
