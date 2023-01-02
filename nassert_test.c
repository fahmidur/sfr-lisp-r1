#include <stdlib.h>
#include <stdio.h>

#include "nassert.h"

int main(int argc, char** argv) {
  nassert(2+2 == 4);
  /*nassert(2+2 == 5);*/
  nassert(2+3 == 5);

  nassert_report();
  return nassert_exit_code();
}
