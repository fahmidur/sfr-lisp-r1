#include "Error.h"
#include "Function.h"
#include "Object.h"

Object* SYMBOL_FNENV_PARENT;

void FunctionSystem_init() {
  SYMBOL_FNENV_PARENT = QSYMBOL("_FNENV_PARENT");
  // a special key used to refer to the parent Object<Hash>
  // representing the parent environment/scope.
}

void FunctionSystem_done() {
}

/* FunctionEnv* FunctionEnv_new(FunctionEnv* parent) { */
/*   FunctionEnv* self = calloc(1, sizeof(FunctionEnv)); */

/*   if(self == NULL) { */
/*     // ERROR: unable to reserve memory */
/*     ErrorSystem_set(1, "FunctionEnv_new. calloc failed"); */
/*     return self; */
/*   } */

/*   self->children_head = NULL; */
/*   self->children_tail = NULL; */
/*   self->sibling_next = NULL; */
/*   self->sibling_prev = NULL; */
/*   self->objects_map = QHASH_NEW1(); */

/*   if(parent != NULL) { */
/*     FunctionEnv_child_add(parent, self); */
/*   } */
/*   return self; */
/* } */

/* void FunctionEnv_del(FunctionEnv* self) { */
/*   assert(self != NULL); */
/*   if(self->children_head != NULL) { */
/*   } */
/* } */

/* void FunctionEnv_child_add(FunctionEnv* self, FunctionEnv* child) { */
/*   assert(self != NULL); */
/*   assert(child != NULL); */
/*   assert(child->sibling_next == NULL); */
/*   assert(child->sibling_prev == NULL); */
/*   child->parent = self; */
/*   if(self->children_head == NULL && self->children_tail == NULL) { */
/*     // first child being added */
/*     self->children_head = self->children_tail = child; */
/*     child->sibling_prev = NULL; */
/*     child->sibling_next = NULL; */
/*   } */
/*   else { */
/*     FunctionEnv* last_child = self->children_tail; */
/*     last_child->sibling_next = child; */
/*     child->sibling_prev = last_child; */
/*     self->children_tail = child; */
/*   } */
/* } */

Function* Function_new(
  Object* (*impl)(Function* fn, Object* args), 
  Object* env,
  int arity,
  Object* params
) {
  char idbuf[128];
  Function* self = calloc(1, sizeof(Function));
  if(self == NULL) {
    ErrorSystem_set(1, "Function_new. calloc failed");
    return NULL;
  }
  sprintf(idbuf, "%p", self);
  self->id = QSTRING_NEW1(idbuf);
  self->impl = impl;
  self->arity = arity;
  self->params = params;
  if(self->arity >= 0 && Object_len(self->params) != self->arity) {
    ErrorSystem_set(1, "Function_new. Invalid arity for given params");
    return NULL;
  }
  assert(env != NULL);
  self->env = env;
  // The use of the body is entirely dependent on the impl.
  // we do not do anything special to the body.
  self->body = NULL;
  return self;
}

void Function_print(Function* self) {
  ObjectUtil_eprintf("Function(%p ID=%v)", self, self->id);
}

Function* Function_clone(Function* self) {
  char idbuf[128];
  Function* clone = calloc(1, sizeof(Function));
  if(clone == NULL) {
    ErrorSystem_set(1, "Function_clone. calloc failed");
    return NULL;
  }
  sprintf(idbuf, "%p", clone);
  clone->id = QSTRING_NEW1(idbuf);
  clone->impl = self->impl;
  clone->arity = self->arity;
  clone->env = Object_accept(Object_clone(self->env));
  clone->params = Object_rc_incr(self->params);
  return clone;
}

Function* Function_get_parent(Function* self) {
  Object* parent_obj = Object_accept(Object_bop_hget(self->env, SYMBOL_FNENV_PARENT));
  if(Object_is_error(parent_obj) || Object_is_null(parent_obj)) {
    Object_assign(&parent_obj, NULL);
    return NULL;
  }
  return parent_obj->impl;
}
