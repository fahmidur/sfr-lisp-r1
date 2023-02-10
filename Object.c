#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Object.h"

static ObjectSystem object_system;

void Object_system_init() {
  printf("--- Object_system_init() ---\n");
  object_system.head = NULL;
  object_system.tail = NULL;
}

void Object_system_done() {
  printf("--- Object_system_done() ---\n");
}
