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
  Environment* parent;
  Environment* children_head;
  Environment* children_tail;
  Environment* sibling_prev;
  Environment* sibling_next;
  Object*      objects;    // Object<Hash>
};


Environment* Environment_new(Environment* parent);
void Environment_del(Environment* self);
void Environment_child_attach(Environment* self, Environment* child);
void Environment_child_detach(Environment* self, Environment* child);

Object* Environment_get(Environment* self, Object* key);
Object* Environment_set(Environment* self, Object* key, Object* val);
Object* Environment_rem(Environment* self, Object* key);

ssize_t Environment_len(Environment* self);

void Environment_print(Environment* self);
char Environment_zero(Environment* self);

#endif
