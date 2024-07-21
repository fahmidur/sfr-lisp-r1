#include <stdio.h>
#include <stdlib.h>
#include "Runtime.h"

void Runtime_init() {
  printf("--- { Runtime_init() { ---\n");
  ErrorSystem_init();
  Symbol_system_init();
  Object_system_init();
  printf("--- } Runtime_init() } ---\n");
  printf("\n\n");
}

void Runtime_done() {
  printf("\n\n");
  printf("--- { Runtime_done() { ---\n");
  Object_system_done();
  Symbol_system_done();
  ErrorSystem_done();
  printf("--- } Runtime_done() } ---\n");
}

