#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Number.h"

int main(int argc, char** argv) {
  Number* n1 = Number_new_from_int(1);
  nassert(n1->rep == 1);

  Number* n2 = Number_new_from_float(2.5);
  nassert(n2->rep == 2.5);

  Number* n3 = Number_new_from_double(3.5);
  nassert(n3->rep == 3.5);

  Number* n4 = Number_new_from_cstr("4.2");
  nassert(n4->rep == 4.2);

  Number_del(n1);
  Number_del(n2);
  Number_del(n3);

  nassert_report();
  return nassert_exit_code();
}
