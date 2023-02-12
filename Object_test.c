#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Object.h"

int main(int argc, char** argv) {
  Object_system_init();
  
  Object_system_done();

  nassert_report();
  return nassert_exit_code();
}
