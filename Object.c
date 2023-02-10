#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Symbol.h"
#include "String.h"
#include "Number.h"
#include "Object.h"

static ObjectSystem object_system;

static Symbol* SYMBOL_SYMBOL;
static Symbol* SYMBOL_STRING;
static Symbol* SYMBOL_NUMBER;

void Object_system_init() {
  printf("--- Object_system_init() ---\n");
  object_system.head = NULL;
  object_system.tail = NULL;
  object_system.size = 0;

  // TODO register the base types

  SYMBOL_SYMBOL = Symbol_new("Symbol");
  SYMBOL_STRING = Symbol_new("String");
  SYMBOL_NUMBER = Symbol_new("Number");

}

Object* Object_new(Symbol* type, void* impl) {
  Object* self = malloc(sizeof(Object));
  // TODO guard against unknown types
  self->type = type;
  self->impl;
  return self;
}

void Object_system_done() {
  printf("--- Object_system_done() ---\n");
}
