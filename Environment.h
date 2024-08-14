#ifndef _SFR_ENVIRONMENT_H
#define _SFR_ENVIRONMENT_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "Object.h"
#include "Symbol.h"

// An Environment is mostly a Tree of Hashes.
typedef struct Environment Environment;
struct Environment {
  Object*      parent;
  Object*      children;
  Object*      objects;    // Object<Hash>
};

Environment* Environment_new();
void Environment_del(Environment* self);
/* void Environment_set_parent(Environment* self, Object* parent_obj); */
void Environment_child_attach(Object* self_obj, Object* child_obj);
void Environment_child_detach(Object* self_obj, Object* child_obj);

Object* Environment_get(Environment* self, Object* key);
Object* Environment_set(Environment* self, Object* key, Object* val);
Object* Environment_rem(Environment* self, Object* key);

ssize_t Environment_len(Environment* self);

void Environment_print(Environment* self);
char Environment_zero(Environment* self);

#endif
