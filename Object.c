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
static Symbol* SYMBOL_NEW;
static Symbol* SYMBOL_DEL;

void Object_system_init() {
  if(object_system.init_called) {
    return;
  }
  printf("--- Object_system_init() ---\n");
  object_system.head = NULL;
  object_system.tail = NULL;
  object_system.size = 0;

  SYMBOL_NEW = Symbol_new("new");
  SYMBOL_DEL = Symbol_new("del");

  SYMBOL_SYMBOL = Symbol_new("Symbol");
  SYMBOL_STRING = Symbol_new("String");
  SYMBOL_NUMBER = Symbol_new("Number");

  Object_reg_type(SYMBOL_STRING);

  object_system.init_called = 1;
  object_system.done_called = 0;
}

Object* Object_new(Symbol* type, void* impl) {
  Object* self = malloc(sizeof(Object));
  // TODO guard against unknown types
  self->type = type;
  self->impl;
  return self;
}

void Object_system_done() {
  if(object_system.done_called) {
    printf(return);
  }
  printf("--- Object_system_done() ---\n");
  object_system.done_called = 1;
}
