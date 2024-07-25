#include "Error.h"
#include "Function.h"
#include "Object.h"

Object* SYMBOL_FNENV_PARENT;

void FunctionSystem_init() {
  SYMBOL_FNENV_PARENT = QSYMBOL("_FNENV_PARENT");
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

/*   /1* if(parent != NULL) { *1/ */
/*   /1*   FunctionEnv_child_add(parent, self); *1/ */
/*   /1* } *1/ */
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
/*     FunctionEnv* iter = self->children_head; */
/*     FunctionEnv* last_child = self->children_tail; */
/*     last_child->sibling_next = child; */
/*     child->sibling_prev = last_child; */
/*     self->children_tail = child; */
/*   } */
/* } */

Function* Function_new(
  Object* (*impl)(Function* fn, Object* args), 
  Object* env,
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
  self->params = params;
  assert(env != NULL);
  self->env = env;
  return self;
}

void Function_print(Function* self) {
  printf("Function(%p)", self);
}

Function* Function_get_parent(Function* self) {
  Object* parent_obj = Object_accept(Object_bop_hget(self->env, SYMBOL_FNENV_PARENT));
  if(Object_is_error(parent_obj) || Object_is_null(parent_obj)) {
    Object_assign(&parent_obj, NULL);
    return NULL;
  }
  return parent_obj->impl;
};
