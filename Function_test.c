#include "Error.h"
#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"

int main(int argc, char** argv) {
  ErrorSystem_init();

  /* Function* obj = Function_new( */

  ErrorSystem_done();

  nassert_report();
  return nassert_exit_code();
}

