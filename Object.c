#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Symbol.h"
#include "String.h"
#include "Number.h"
#include "Object.h"

static ObjectSystem object_system;

static Symbol* SYMBOL_NEW;
static Symbol* SYMBOL_DEL;

static Symbol* SYMBOL_SYMBOL;
static Symbol* SYMBOL_STRING;
static Symbol* SYMBOL_NUMBER;

char Object_type_set(Symbol* type, void (*del)(void* s)) {
  size_t hash = type->hash;
  size_t key = hash % OBJECT_TYPES_BUCKETS_SIZE;
  ObjectTypeInfo* oti = malloc(sizeof(ObjectTypeInfo));
  oti->next = NULL;
  oti->prev = NULL;
  oti->type = type;
  oti->del = del;
  oti->key = key;
  ObjectTypeInfo* oti_old = object_system.types[oti->key];
  oti->next = oti_old;
  oti_old->prev = oti;
  object_system.types[key] = oti;
  return 0;
}

void Object_system_init() {
  if(object_system.init_called) {
    return;
  }
  printf("--- Object_system_init() ---\n");
  size_t i = 0;
  object_system.head = NULL;
  object_system.tail = NULL;
  object_system.size = 0;
  for(i = 0; i < OBJECT_TYPES_BUCKETS_SIZE; i++) {
    object_system.types[i] = NULL;
  }

  SYMBOL_NEW = Symbol_new("new");
  SYMBOL_DEL = Symbol_new("del");

  SYMBOL_SYMBOL = Symbol_new("Symbol");
  SYMBOL_STRING = Symbol_new("String");
  SYMBOL_NUMBER = Symbol_new("Number");

  Object_type_set(SYMBOL_SYMBOL, Symbol_noop);
  Object_type_set(SYMBOL_STRING, (void (*)(void*))String_del);

  object_system.init_called = 1;
  object_system.done_called = 0;
}

Object* Object_new(Symbol* type, void* impl) {
  Object* self = malloc(sizeof(Object));
  self->type = type;
  self->impl = impl;

  Object_add(self);
  
  return self;
}

void Object_add(Object* self) {
  if(object_system.size == 0) {
    object_system.head = self;
    object_system.tail = self;
    self->prev = NULL;
    self->next = NULL;
  } 
  else {
    Object* old_head = object_system.head;
    object_system.head = self;
    self->next = old_head;
    old_head->prev = self;
  }
  object_system.size++;
}

void Object_del(Object* self) {
  if(object_system.size == 0) {
    return;
  }
  Object* new_head;
  Object* new_tail;
  if(self == object_system.head && self == object_system.tail) {
    object_system.head = NULL;
    object_system.tail = NULL;
  }
  else
  if(self == object_system.head) {
    new_head = self->next;
    object_system.head = new_head;
    new_head->prev = NULL;
  }
  else
  if(self == object_system.tail) {
    new_tail = self->prev;
    object_system.tail = new_tail;
    new_tail->next = NULL;
  }
  object_system.size--;
  self->next = NULL;
  self->prev = NULL;

  ObjectTypeInfo* oti = Object_type_get(self->type);
  if(!oti) {
    printf("FATAL: unknown ObjectTypeInfo for type: ");
    Symbol_print(self->type);
    exit(1);
  }
  oti->del(self);
}

void Object_system_done() {
  if(object_system.done_called) {
    return;
  }
  printf("--- Object_system_done() ---\n");
  object_system.done_called = 1;
}
