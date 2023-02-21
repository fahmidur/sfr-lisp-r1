#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Symbol.h"
#include "String.h"
#include "Number.h"
#include "List.h"
#include "Object.h"

static ObjectSystem* object_system;

static Symbol* SYMBOL_NEW;
static Symbol* SYMBOL_DEL;

static Symbol* SYMBOL_SYMBOL;
static Symbol* SYMBOL_STRING;
static Symbol* SYMBOL_NUMBER;
static Symbol* SYMBOL_LIST;

char Object_oti_set(
  Symbol* type, 
  void (*del)(void* s),
  void (*print)(void* s)
  ) {
  printf("Object_oti_set("); Symbol_print(type); printf(", del=%p)\n", del);
  size_t hash = type->hash;
  size_t key = hash % OBJECT_TYPES_BUCKETS_SIZE;
  ObjectTypeInfo* oti = calloc(1, sizeof(ObjectTypeInfo));
  /*printf("oti=%p\n", oti);*/
  oti->next = NULL;
  oti->prev = NULL;
  oti->type = type;
  /*printf("oti setting oti->del\n");*/
  oti->del = del;
  oti->print = print;
  /*printf("oti->del = %p\n", oti->del);*/
  oti->hash = hash;
  oti->key = key;
  ObjectTypeInfo* oti_old = object_system->types[oti->key];
  /*printf("oti_old=%p\n", oti_old);*/
  oti->next = oti_old;
  if(oti_old != NULL) {
    oti_old->prev = oti;
  }
  object_system->types[key] = oti;
  return 0;
}

ObjectTypeInfo* Object_oti_get(Symbol* type) {
  size_t hash = type->hash;
  size_t key = hash % OBJECT_TYPES_BUCKETS_SIZE;
  ObjectTypeInfo* oti = object_system->types[key];
  if(oti == NULL) {
    return NULL;
  }
  while(oti->type != type) {
    oti = oti->next;
  }
  return oti;
}

void Object_system_init() {
  printf("--- BEG. Object_system_init() ---\n");
  size_t i;
  object_system = calloc(1, sizeof(ObjectSystem));
  object_system->init_called = 0;
  object_system->done_called = 0;
  object_system->head = NULL;
  object_system->tail = NULL;
  object_system->size = 0;
  for(i = 0; i < OBJECT_TYPES_BUCKETS_SIZE; i++) {
    object_system->types[i] = NULL;
  }

  SYMBOL_NEW = Symbol_new("new");
  SYMBOL_DEL = Symbol_new("del");

  // Atomic Types
  SYMBOL_SYMBOL = Symbol_new("Symbol");
  SYMBOL_STRING = Symbol_new("String");
  SYMBOL_NUMBER = Symbol_new("Number");
  // Composit Types
  SYMBOL_LIST = Symbol_new("List");

  printf("\n\n");

  Object_oti_set(
    SYMBOL_SYMBOL, 
    Symbol_noop, 
    (void (*)(void*))Symbol_print
  );
  Object_oti_set(
    SYMBOL_STRING, 
    (void (*)(void*))String_del,
    (void (*)(void*))String_print
  );
  Object_oti_set(
    SYMBOL_NUMBER, 
    (void (*)(void*))Number_del,
    (void (*)(void*))Number_print
  );
  Object_oti_set(
    SYMBOL_LIST, 
    (void (*)(void*))List_del,
    (void (*)(void*))List_print
  );

  object_system->init_called = 1;
  object_system->done_called = 0;

  printf("--- END. Object_system_init() ---\n");
}

Object* Object_new(Symbol* type, void* impl) {
  Object* self = calloc(1, sizeof(Object));

  self->type = type;
  self->impl = impl;
  self->refc = 0;

  Object_add_to_system(self);
  
  return self;
}

size_t Object_system_size() {
  if(object_system == NULL) {
    return 0;
  }
  return object_system->size;
}

// TODO: make this idempotent.
// May need to change object storage
// from List to Hash
void Object_add_to_system(Object* self) {
  if(object_system->size == 0) {
    object_system->head = self;
    object_system->tail = self;
    self->prev = NULL;
    self->next = NULL;
  } 
  else {
    Object* old_head = object_system->head;
    object_system->head = self;
    self->next = old_head;
    old_head->prev = self;
  }
  object_system->size++;
}

void Object_del(Object* self) {
  printf("Object_del(%p) \n", self);
  if(object_system->size == 0) {
    return;
  }
  Object* new_head;
  Object* new_tail;
  if(self == object_system->head && self == object_system->tail) {
    object_system->head = NULL;
    object_system->tail = NULL;
  }
  else
  if(self == object_system->head) {
    new_head = self->next;
    object_system->head = new_head;
    new_head->prev = NULL;
  }
  else
  if(self == object_system->tail) {
    new_tail = self->prev;
    object_system->tail = new_tail;
    new_tail->next = NULL;
  }
  object_system->size--;
  self->next = NULL;
  self->prev = NULL;

  ObjectTypeInfo* oti = Object_oti_get(self->type);
  if(!oti) {
    printf("FATAL: unknown ObjectTypeInfo for type: ");
    Symbol_print(self->type);
    exit(1);
  }
  oti->del(self->impl);
  printf("Object_del(%p). calling free on self.\n", self);
  free(self);
}

void Object_system_done() {
  if(object_system->done_called) {
    return;
  }
  printf("--- Object_system_done() ---\n");
  int i;
  object_system->done_called = 1;
  // delete all objects
  while(object_system->size > 0) {
    Object_del(object_system->head);
  }
  // delete type information
  ObjectTypeInfo* iter;
  ObjectTypeInfo* iter_next;
  for(i = 0; i < OBJECT_TYPES_BUCKETS_SIZE; i++) {
    iter = object_system->types[i];
    while(iter != NULL) {
      iter_next = iter->next;
      if(iter_next != NULL) {
        iter_next->prev = NULL;
      }
      iter->next = NULL;
      //---
      free(iter);
      //---
      object_system->types[i] = iter_next;
      iter = iter_next;
    }
  }
  free(object_system);
}

Symbol* Object_type(Object* self) {
  return self->type;
}

void Object_print(Object* self) {
  ObjectTypeInfo* oti = Object_oti_get(self->type);
  if(oti == NULL) {
    return;
  }
  oti->print(self);
}

// Binary Operations
// (+ 1 2)
// (+ "hello" "there")
Object* Object_bop_add(Object* a, Object* b) {
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    Number* ret = Number_add(a->impl, b->impl);
    return Object_new(SYMBOL_NUMBER, ret);
  }
  else {
    //TODO return Error object
    return NULL;
  }
}

