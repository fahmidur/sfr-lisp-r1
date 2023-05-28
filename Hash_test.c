#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Symbol.h"
#include "Object.h"

int main(int argc, char** argv) {
  printf("--- Hash test ---\n");
  Symbol_system_init();
  Object_system_init();


  Symbol_system_done();
  Object_system_done();

  nassert_report();
  return nassert_exit_code();
}
