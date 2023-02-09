#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Util.h"

int main(int argc, char** argv) {
  nassert(Util_cstr_hash_djb2("") == 5381);
  nassert(Util_sizet_max(2, 3) == 3);
  nassert(Util_sizet_max(9, 2) == 9);
  nassert(Util_sizet_max(1, 1) == 1);
  //----
  nassert_report();
  return nassert_exit_code();
}
