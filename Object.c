#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Symbol.h"
#include "String.h"
#include "Number.h"
#include "List.h"
#include "Error.h"
#include "Object.h"

ObjectSystem* object_system;

Symbol* SYMBOL_NEW;
Symbol* SYMBOL_DEL;

Symbol* SYMBOL_SYMBOL;
Symbol* SYMBOL_STRING;
Symbol* SYMBOL_NUMBER;
Symbol* SYMBOL_ERROR;
Symbol* SYMBOL_LIST;

char Object_oti_set(Symbol* type, ObjectTypeInfo otiarg) {
  printf("Object_oti_set("); Symbol_print(type); printf(")\n");
  size_t hash = type->hash;
  size_t key = hash % OBJECT_TYPES_BUCKETS_SIZE;
  ObjectTypeInfo* oti = calloc(1, sizeof(ObjectTypeInfo));
  oti->hash = hash;
  oti->key = key;
  oti->next = NULL;
  oti->prev = NULL;
  oti->type = type;
  oti->fn_del   = otiarg.fn_del;
  oti->fn_print = otiarg.fn_print;
  oti->fn_clone = otiarg.fn_clone;
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
  SYMBOL_ERROR  = Symbol_new("Error");
  // Composit Types
  SYMBOL_LIST = Symbol_new("List");

  printf("\n\n");

  ObjectTypeInfo otiarg_symbol = { 
    .fn_del   = Symbol_noop, 
    .fn_print = (void  (*)(void*))Symbol_print,
    .fn_clone = (void* (*)(void*))Symbol_clone
  };
  Object_oti_set(SYMBOL_SYMBOL, otiarg_symbol);

  ObjectTypeInfo otiarg_string = {
    .fn_del   = (void  (*)(void*))String_del,
    .fn_print = (void  (*)(void*))String_print,
    .fn_clone = (void* (*)(void*))String_clone
  };
  Object_oti_set(SYMBOL_STRING, otiarg_string);

  ObjectTypeInfo otiarg_number = {
    .fn_del   = (void  (*)(void*))Number_del,
    .fn_print = (void  (*)(void*))Number_print,
    .fn_clone = (void* (*)(void*))Number_clone
  };
  Object_oti_set(SYMBOL_NUMBER, otiarg_number);

  ObjectTypeInfo otiarg_list = {
    .fn_del =   (void   (*)(void*))List_del,
    .fn_print = (void   (*)(void*))List_print,
    .fn_clone = (void*  (*)(void*))List_clone
  };
  Object_oti_set(SYMBOL_LIST, otiarg_list);

  ObjectTypeInfo otiarg_error = {
    .fn_del   = (void  (*)(void*))Error_del,
    .fn_print = (void  (*)(void*))Error_print,
    .fn_clone = (void* (*)(void*))Error_clone
  };
  Object_oti_set(SYMBOL_ERROR, otiarg_error);

  object_system->init_called = 1;
  object_system->done_called = 0;

  printf("--- END. Object_system_init() ---\n");
}

Object* Object_new(Symbol* type, int rc, void* impl) {
  Object* self = calloc(1, sizeof(Object));

  self->type = type;
  self->impl = impl;
  self->rc = rc;
  self->gc_skipped = 0;

  Object_add_to_system(self);
  
  return self;
}

size_t Object_system_size() {
  if(object_system == NULL) {
    return 0;
  }
  return object_system->size;
}

void Object_system_gc() {
  Object* iter = object_system->head;
  Object* next = NULL;
  while(iter != NULL) {
    next = iter->next;
    Object_gc(iter);
    iter = next;
  }
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
  printf("Object_del(%p). type = ", self); Symbol_print(Object_type(self)); printf("\n");
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
    printf("FATAL: unknown ObjectTypeInfo oti for type ");
    Symbol_print(self->type);
    exit(1);
  }
  if(oti->fn_del == NULL) {
    printf("FATAL: ObjectTypeInfo oti for type ");
    Symbol_print(self->type);
    printf(" is missing fn_del\n");
    exit(1);
  }
  oti->fn_del(self->impl);
  /*printf("Object_del(%p). calling free on self.\n", self);*/
  free(self);
}

void Object_system_done() {
  if(object_system->done_called) {
    return;
  }
  printf("--- { Object_system_done() { ---\n");
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
  printf("--- } Object_system_done() } ---\n");
}

Symbol* Object_type(Object* self) {
  return self->type;
}

void Object_rc_incr(Object* self) {
  self->rc++;
  self->gc_skipped = 0;
}

void Object_gc(Object* self) {
  if(self->rc <= 0) {
    /*if(self->rc == 0 && self->gc_skipped <= 3) {*/
      /*// we really do not know if the object*/
      /*// is assigned, so do not delete it until*/
      /*// the next gc cycle. */
      /*self->gc_skipped++;*/
      /*printf("--- gc_skipped=%d\n", self->gc_skipped);*/
      /*return;*/
    /*}*/
    Object_del(self);
  }
}

void Object_rc_decr(Object* self) {
  if(self->rc >= 1) {
    self->rc--;
  }
  Object_gc(self);
}

void Object_print(Object* self) {
  ObjectTypeInfo* oti = Object_oti_get(self->type);
  if(oti == NULL) {
    printf("FATAL: unknown ObjectTypeInfo oti for type ");
    Symbol_print(self->type);
    exit(1);
  }
  // Or you might want to have a default print.
  if(oti->fn_print == NULL) {
    printf("FATAL: ObjectTypeInfo oti for type ");
    Symbol_print(self->type);
    printf(" is missing fn_del\n");
    exit(1);
  }
  oti->fn_print(self->impl);
}

Object* Object_clone(Object* self) {
  ObjectTypeInfo* oti = Object_oti_get(self->type);
  if(oti == NULL) {
    return Object_new(SYMBOL_ERROR, 0, Error_new("Missing oti for Object Type"));
  }
  if(oti->fn_clone == NULL) {
    return Object_new(SYMBOL_ERROR, 0, Error_new("Missing clone for Object Type"));
  }
  void* rawobj = oti->fn_clone(self->impl);
  return Object_new(self->type, 0, rawobj);
}

int Object_cmp(Object* a, Object* b) {
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    return Number_cmp(a->impl, b->impl);
  }
  else
  if(Object_type(a) == SYMBOL_STRING && Object_type(b) == SYMBOL_STRING) {
    return String_cmp(a->impl, b->impl);
  }
  else
  if(Object_type(a) == SYMBOL_SYMBOL && Object_type(b) == SYMBOL_SYMBOL) {
    return Symbol_cmp(a->impl, b->impl);
  }
  return -9;
}

Object* Object_bop_add(Object* a, Object* b) {
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    return Object_new(SYMBOL_NUMBER, 0, Number_add(a->impl, b->impl));
  }
  if(Object_type(a) == SYMBOL_STRING && Object_type(b) == SYMBOL_STRING) {
    return Object_new(SYMBOL_STRING, 0, String_add(a->impl, b->impl));
  }
  return Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types bop_add"));
}

Object* Object_bop_sub(Object* a, Object* b) {
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    return Object_new(SYMBOL_NUMBER, 0, Number_sub(a->impl, b->impl));
  }
  return Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types for bop_sub"));
}

Object* Object_bop_mul(Object* a, Object* b) {
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    return Object_new(SYMBOL_NUMBER, 0, Number_mul(a->impl, b->impl));
  }
  return Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types for bop_mul"));
}

Object* Object_bop_div(Object* a, Object* b) {
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    return Object_new(SYMBOL_NUMBER, 0, Number_div(a->impl, b->impl));
  }
  return Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types for bop_div"));
}



