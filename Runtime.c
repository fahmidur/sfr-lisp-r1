#include <stdio.h>
#include <stdlib.h>
#include "Runtime.h"

void Runtime_init() {
  dbg_printf("--- { Runtime_init() { ---\n");
  ErrorSystem_init();
  Symbol_system_init();
  Object_system_init();
  StringIO_init();
  dbg_printf("--- } Runtime_init() } ---\n");
  dbg_printf("\n\n");
}

void Runtime_done() {
  dbg_printf("\n\n");
  dbg_printf("--- { Runtime_done() { ---\n");
  StringIO_done();
  Object_system_done();
  Symbol_system_done();
  ErrorSystem_done();
  dbg_printf("--- } Runtime_done() } ---\n");
}

