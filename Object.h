#ifndef _SFR_OBJECT
#define _SFR_OBJECT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Symbol.h"
#include "String.h"
#include "Number.h"

#define OBJECT_TYPES_BUCKETS_SIZE 128

typedef struct Object Object;
struct Object {
  Symbol* type;
  Object* next;
  Object* prev;
  void*   impl;
};

typedef struct ObjectTypeInfo ObjectTypeInfo;
struct ObjectTypeInfo {
  size_t hash;
  size_t key;
  Symbol* type;
  ObjectTypeInfo* prev;
  ObjectTypeInfo* next;
  void (*del)(void* s);
};

typedef struct ObjectSystem ObjectSystem;
struct ObjectSystem {
  size_t         init_called;
  size_t         done_called;
  Object*        head;
  Object*        tail;
  size_t         size;
  ObjectTypeInfo* types[OBJECT_TYPES_BUCKETS_SIZE];
};
//   [head]                        [tail]
//   |                             |
//   |                             |
//   +                             +
//   [Object] +---+ [Object] +---+ [Object] ---+ [NULL]

char Object_type_set(Symbol* type, void (*del)(void* s));
ObjectTypeInfo* Object_type_get(Symbol* type);
Object* Object_new(Symbol* type, void* impl);
void Object_add(Object* self);
void Object_del(Object* self);

void Object_system_init();
void Object_system_done();

#endif
