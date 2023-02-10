#ifndef _SFR_OBJECT
#define _SFR_OBJECT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Object Object;
struct Object {
  Symbol* klass;
  Object* next;
  Object* prev;
  void* impl;
};

typedef struct ObjectSystem ObjectSystem;
struct ObjectSystem {
  Object* head;
  Object* tail;
};

void Object_system_init();
void Object_system_done();

#endif
