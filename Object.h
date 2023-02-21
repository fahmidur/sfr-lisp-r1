#ifndef _SFR_OBJECT
#define _SFR_OBJECT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Symbol.h"
#include "String.h"
#include "Number.h"

#define OBJECT_TYPES_BUCKETS_SIZE 128

extern Symbol* SYMBOL_SYMBOL;
extern Symbol* SYMBOL_STRING;
extern Symbol* SYMBOL_NUMBER;
extern Symbol* SYMBOL_ERROR;
extern Symbol* SYMBOL_LIST;

typedef struct Object Object;
struct Object {
  Symbol* type;
  Object* next;
  Object* prev;
  void*   impl;
  int     refc;
};

typedef struct ObjectTypeInfo ObjectTypeInfo;
struct ObjectTypeInfo {
  Symbol* type;
  size_t hash;
  size_t key;
  ObjectTypeInfo* prev;
  ObjectTypeInfo* next;
  // vtable
  void (*del)(void* s); 
  void (*print)(void* s);
};

typedef struct ObjectSystem ObjectSystem;
struct ObjectSystem {
  size_t           init_called;
  size_t           done_called;
  Object*          head;
  Object*          tail;
  size_t           size;
  ObjectTypeInfo*  types[OBJECT_TYPES_BUCKETS_SIZE];
};
//   [head]                        [tail]
//   |                             |
//   |                             |
//   +                             +
//   [Object] +---+ [Object] +---+ [Object] ---+ [NULL]

char Object_oti_set(
  Symbol* type, 
  void (*)(void* s),
  void (*)(void* s)
);
ObjectTypeInfo* Object_oti_get(Symbol* type);

Object* Object_new(Symbol* type, void* impl);
void Object_add_to_system(Object* self);
void Object_del(Object* self);
Symbol* Object_type(Object* self);
int Object_cmp(Object* a, Object* b);
void Object_print(Object* self);
Object* Object_bop_add(Object* a, Object* b);
Object* Object_bop_sub(Object* a, Object* b);
Object* Object_bop_div(Object* a, Object* b);

void    Object_system_init();
size_t  Object_system_size();
void    Object_system_done();

// Composits
#include "List.h"

#endif
