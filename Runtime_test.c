#include <stdio.h>
#include <stdlib.h>
#include "Runtime.h"
#include "nassert.h"

int main(int argc, char** argv) {
  Runtime_init();

  Runtime_done();

  nassert_report();
  return nassert_exit_code();
}
