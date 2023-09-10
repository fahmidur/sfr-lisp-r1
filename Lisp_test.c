#include <stdio.h>
#include "nassert.h"
#include "Lisp.h"

int main(int argc, char** argv) {

  Runtime_init();

  // TODO add tests here
  Object* tc1_string = QSTRING_NEW1("(+ 2 3)");
  Object* tc1_tokens = Lisp_tokenize(tc1_string);

  Runtime_done();

  //----
  nassert_report();
  return nassert_exit_code();
}
