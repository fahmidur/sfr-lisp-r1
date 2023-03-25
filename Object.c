#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
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
  assert(type != NULL);
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
  assert(type != NULL);
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

/**
 * Create a new Object. 
 * Everything in this Runtime is an Object. 
 * TODO: ensure that there are no other objects with
 * the same impl pointer. Currently, it assumes
 * all void* impl pointers are unique. 
 */
Object* Object_new(Symbol* type, int rc, void* impl) {
  assert(type != NULL);
  assert(rc == 0 || rc == 1);
  assert(impl != NULL);

  Object* self = calloc(1, sizeof(Object));

  self->type = type;
  self->impl = impl;
  self->rc = rc;
  /*self->gc_skipped = 0;*/
  self->returning = 0;

  Object_add_to_system(self);
  
  if(rc == 0) {
    return Object_return(self);
  } else {
    return self;
  }
}

Object* Object_return(Object* self) {
  assert(self != NULL);
  self->returning = 1;
  return self;
}

Object* Object_accept(Object* self) {
  assert(self != NULL);
  self->returning = 0;
  Object_rc_incr(self);
  return self;
}

Object* Object_reject(Object* self) {
  assert(self != NULL);
  self->returning = 0;
  Object_gc(self);
  return NULL;
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
    iter = Object_gc(iter);
    iter = next;
  }
}

/**
 * TODO: make this idempotent.
 * May need to change object storage from List to Hash
 */
void Object_add_to_system(Object* self) {
  assert(self != NULL);
  if(object_system->size == 0) {
    object_system->head = self;
    object_system->tail = self;
    self->prev = NULL;
    self->next = NULL;
  } 
  else {
    Object* old_tail = object_system->tail;
    old_tail->next = self; self->prev = old_tail;
    object_system->tail = self;
  }
  object_system->size++;
}

void Object_del(Object* self) {
  assert(self != NULL);
  printf("Object_del(%p). type = ", self); 
    Symbol_print(Object_type(self)); 
    /*printf(" || ");*/
    /*Object_print(self);*/
  printf("\n");
  if(object_system->size == 0) {
    return;
  }
  Object* new_head;
  Object* new_tail;
  Object* prev;
  Object* next;
  if(self == object_system->head && self == object_system->tail) {
    object_system->head = NULL;
    object_system->tail = NULL;
  }
  else
  if(self == object_system->head) {
    new_head = self->next;
    assert(new_head != NULL);
    new_head->prev = NULL;
    self->next = NULL;
    object_system->head = new_head;
  }
  else
  if(self == object_system->tail) {
    new_tail = self->prev;
    assert(new_tail != NULL);
    new_tail->next = NULL;
    self->prev = NULL;
    object_system->tail = new_tail;
  }
  else {
    prev = self->prev;
    next = self->next;
    assert(prev != NULL);
    assert(next != NULL);
    self->next = NULL;
    self->prev = NULL;
    prev->next = next; next->prev = prev;
  }
  object_system->size--;

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
    /*Object_del(object_system->head);*/
    Object* iter = object_system->head;
    Object* next = NULL;
    while(iter != NULL) {
      next = iter->next; 
      if(iter->returning) {
        printf("WARNING: Object_system_done. Object at %p 'returning'\n", iter);
        iter->returning = 0;
      }
      iter = Object_rc_decr(iter);
      iter = next;
    }
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
  assert(self != NULL);
  return self->type;
}

Object* Object_rc_incr(Object* self) {
  assert(self != NULL);
  self->rc++;
  return self;
}

Object* Object_gc(Object* self) {
  assert(self != NULL);
  if(self->rc <= 0) {
    if(self->returning) {
      printf("Object_gc(%p). Object is returning. -SKIPPED-\n", self);
      return self;
    }
    Object_del(self);
    return NULL;
  }
  return self;
}

Object* Object_rc_decr(Object* self) {
  assert(self != NULL);
  if(self->rc >= 1) {
    self->rc--;
  }
  return Object_gc(self);
}

void Object_print(Object* self) {
  if(self == NULL) {
    printf("(NULL)");
    return;
  }
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
  assert(self != NULL);
  Object_rc_incr(self);

  ObjectTypeInfo* oti = Object_oti_get(self->type);

  Object* ret = NULL;

  if(oti == NULL) {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Missing oti for Object Type")));
    goto _return;
  }
  if(oti->fn_clone == NULL) {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Missing clone for Object Type")));
    goto _return;
  }

  void* rawobj = oti->fn_clone(self->impl);
  ret = Object_return(Object_new(self->type, 0, rawobj));

_return:
  assert(ret != NULL);
  Object_rc_decr(self);
  return ret;
}

int Object_cmp(Object* a, Object* b) {
  assert(a != NULL); assert(b != NULL);
  Object_rc_incr(a); Object_rc_incr(b);
  int ret = -9;
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    ret = Number_cmp(a->impl, b->impl);
  }
  else
  if(Object_type(a) == SYMBOL_STRING && Object_type(b) == SYMBOL_STRING) {
    ret = String_cmp(a->impl, b->impl);
  }
  else
  if(Object_type(a) == SYMBOL_SYMBOL && Object_type(b) == SYMBOL_SYMBOL) {
    ret = Symbol_cmp(a->impl, b->impl);
  }
  else
  if(Object_type(a) == SYMBOL_LIST && Object_type(b) == SYMBOL_LIST) {
    ret = List_cmp(a->impl, b->impl);
  }
  Object_rc_decr(a); Object_rc_decr(b);
  return ret;
}

Object* Object_bop_add(Object* a, Object* b) {
  assert(a != NULL); assert(b != NULL);
  Object_rc_incr(a); Object_rc_incr(b);
  Object* ret = NULL;
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    ret = Object_return(Object_new(SYMBOL_NUMBER, 0, Number_add(a->impl, b->impl)));
  }
  else
  if(Object_type(a) == SYMBOL_STRING && Object_type(b) == SYMBOL_STRING) {
    ret = Object_return(Object_new(SYMBOL_STRING, 0, String_add(a->impl, b->impl)));
  } 
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types bop_add")));
  }
  Object_rc_decr(a); Object_rc_decr(b);
  assert(ret != NULL);
  return ret;
}

Object* Object_bop_sub(Object* a, Object* b) {
  assert(a != NULL); assert(b != NULL);
  Object_rc_incr(a); Object_rc_incr(b);
  Object* ret = NULL; 
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    ret = Object_return(Object_new(SYMBOL_NUMBER, 0, Number_sub(a->impl, b->impl)));
  } 
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types for bop_sub")));
  }
  Object_rc_decr(a); Object_rc_decr(b);
  assert(ret != NULL);
  return ret;
}

Object* Object_bop_mul(Object* a, Object* b) {
  assert(a != NULL); assert(b != NULL);
  Object_rc_incr(a); Object_rc_incr(b);
  Object* ret = NULL;
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    ret = Object_return(Object_new(SYMBOL_NUMBER, 0, Number_mul(a->impl, b->impl)));
  } 
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types for bop_mul")));
  }
  Object_rc_decr(a); Object_rc_decr(b);
  assert(ret != NULL);
  return ret;
}

Object* Object_bop_div(Object* a, Object* b) {
  assert(a != NULL); assert(b != NULL);
  Object_rc_incr(a); Object_rc_incr(b);
  Object* ret = NULL;
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    ret = Object_return(Object_new(SYMBOL_NUMBER, 0, Number_div(a->impl, b->impl)));
  }
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types for bop_div")));
  }
  Object_rc_decr(a); Object_rc_decr(b);
  assert(ret != NULL);
  return ret;
}

Object* Object_bop_push(Object* a, Object* b) {
  assert(a != NULL); assert(b != NULL);
  Object_rc_incr(a); Object_rc_incr(b);
  Object* ret = NULL;
  if(Object_type(a) == SYMBOL_LIST) {
    List_push(a->impl, b);
    ret = Object_return(a);
  } 
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types for bop_push")));
  }
  Object_rc_decr(a); Object_rc_decr(b);
  assert(ret != NULL);
  return ret;
}

/**
 * Print all the objects in the object_system.
 * Mainly for debugging. 
 */
void Object_system_print() {
  printf("--- { Object_system_print(). BEG { ---\n");
  Object* iter = object_system->head;
  int i = 0;
  while(iter != NULL) {
    printf("[i=%03d] || Object(p=%p, rc=%03d, rt=%03d) || ", i, iter, iter->rc, iter->returning);
    Object_print(iter);
    printf("\n");
    iter = iter->next;
    i++;
  }
  printf("--------------------------------------\n");
  printf("SIZE: %zu\n", object_system->size);
  printf("--- } Object_system_print(). END } ---\n");
}

void ObjectUtil_eprintf_sig(int SIGSIZE, char** sigptr, int* sigposptr, va_list argv) {
  /*printf("--- ObjectUtil_eprintf_sig ---\n");*/
  char* sig = *sigptr;
  /*printf("[sig=|%s|]", sig);*/
  if(strcmp(sig, "%v") == 0) {
    Object_print(va_arg(argv, void*));
  }
  else {
    vprintf(*sigptr, argv);
  }
  memset(*sigptr, 0, SIGSIZE);
  *sigposptr = 0;
}

void ObjectUtil_eprintf_buf(int BUFSIZE, char** bufptr, int* bufposptr) {
  /*printf("--- ObjectUtil_eprintf_buf ---\n");*/
  if(*bufposptr > 0) {
    fputs(*bufptr, stdout);
  }
  memset(*bufptr, 0, BUFSIZE);
  *bufposptr = 0;
}

void ObjectUtil_eprintf(char* fmt, ...) {
  /*printf("ObjectUtil_eprintf. fmt=%s\n", fmt);*/
  int i, j;
  int argc = 0;
  va_list argv;
  size_t fmt_len = strlen(fmt);
  char ch;
  for(i = 0; i < fmt_len; i++) {
    ch = fmt[i];
    if(ch == '%') {
      argc++;
    }
  }
  va_start(argv, fmt);
  int   BUFSIZE = fmt_len+1;
  char* buf = calloc(1, fmt_len+1);
  int   bufpos = 0;
  int   SIGSIZE = 20; 
  char* sig = calloc(1, SIGSIZE);
  int   sigpos = 0;
  char  insig = 0;
  int   argpos = 0;
  for(i = 0; i < fmt_len; i++) {
    ch = fmt[i];
    if(insig) {
      if((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')) {
        sig[sigpos++] = ch;
      }
      else {
        ObjectUtil_eprintf_sig(SIGSIZE, &sig, &sigpos, argv);
        insig = 0;
        if(i > 0) { i--; }
      }
    }
    else
    if(ch == '%') {
      insig = 1;
      sig[0] = '%';
      sigpos = 1;
      ObjectUtil_eprintf_buf(BUFSIZE, &buf, &bufpos);
    }
    else {
      buf[bufpos++] = ch;
      /*printf("bufpos=%d / BUFSIZE=%d || buf=%s\n", bufpos, BUFSIZE, buf);*/
    }
  }
  if(insig && sigpos > 0) {
    ObjectUtil_eprintf_sig(SIGSIZE, &sig, &sigpos, argv);
  }
  else
  if(bufpos > 0) {
    ObjectUtil_eprintf_buf(BUFSIZE, &buf, &bufpos);
  }
  free(buf);
  free(sig);
  va_end(argv);
}

size_t Object_len(Object* self) {
  assert(self != NULL);
  /*Object_rc_incr(self);*/
  size_t ret = 0;
  if(Object_type(self) == SYMBOL_LIST) {
    ret = List_len(self->impl);
  }
  else
  if(Object_type(self) == SYMBOL_STRING) {
    ret = String_len(self->impl);
  }
  /*Object_rc_decr(self);*/
  return ret;
}
