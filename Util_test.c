#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Util.h"

int main(int argc, char** argv) {
  nassert(Util_cstr_hash_djb2("") == 5381);
  //----
  nassert_report();
  return nassert_exit_code();
}
