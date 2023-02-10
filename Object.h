#ifndef _SFR_OBJECT
#define _SFR_OBJECT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Symbol.h"
#include "String.h"
#include "Number.h"

typedef struct Object Object;
struct Object {
  Symbol* type;
  Object* next;
  Object* prev;
  void*   impl;
};

typedef struct ObjectSystem ObjectSystem;
struct ObjectSystem {
  Object* head;
  Object* tail;
  size_t  size;
};

Object* Object_new(Symbol* type, void* impl);

void Object_system_init();
void Object_system_done();

#endif
