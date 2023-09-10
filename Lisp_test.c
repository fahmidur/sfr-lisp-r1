#include <stdio.h>
#include "nassert.h"
#include "Lisp.h"

int main(int argc, char** argv) {

  // TODO add tests here

  //----
  nassert_report();
  return nassert_exit_code();
}
