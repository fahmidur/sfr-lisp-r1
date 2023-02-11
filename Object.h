#ifndef _SFR_OBJECT
#define _SFR_OBJECT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Symbol.h"
#include "String.h"
#include "Number.h"

#define OBJECT_TYPES_BUCKETS_SIZE 2048

typedef struct Object Object;
struct Object {
  Symbol* type;
  Object* next;
  Object* prev;
  void*   impl;
};

typedef struct ObjectTypeInfo ObjectTypeInfo;
struct ObjectTypeInfo {
  Symbol* type;
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

char Object_reg_type(Symbol* type, void (*del)(void* s));
Object* Object_new(Symbol* type, void* impl);

void Object_system_init();
void Object_system_done();

#endif
