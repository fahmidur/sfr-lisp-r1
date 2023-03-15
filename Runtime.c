#include <stdio.h>
#include <stdlib.h>
#include "Runtime.h"

void Runtime_init() {
  printf("--- { Runtime_init() { ---\n");
  Symbol_system_init();
  Object_system_init();
  printf("--- } Runtime_init() } ---\n");
}

void Runtime_done() {
  printf("--- { Runtime_done() { ---\n");
  Object_system_done();
  Symbol_system_done();
  printf("--- } Runtime_done() } ---\n");
}

