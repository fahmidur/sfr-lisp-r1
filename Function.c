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

FunctionEnv* FunctionEnv_new(FunctionEnv* parent) {
  FunctionEnv* self = calloc(1, sizeof(FunctionEnv));

  if(self == NULL) {
    // ERROR: unable to reserve memory
    ErrorSystem_set(1, "FunctionEnv_new. calloc failed");
    return self;
  }

  self->children_head = NULL;
  self->children_tail = NULL;
  self->sibling_next = NULL;
  self->sibling_prev = NULL;
  self->objects = QHASH_NEW1();

  if(parent != NULL) {
    FunctionEnv_child_add(parent, self);
  }
  return self;
}

void FunctionEnv_del(FunctionEnv* self) {
  assert(self != NULL);
  assert(self->sibling_prev == NULL);
  assert(self->sibling_next == NULL);
  FunctionEnv* iter = NULL;
  FunctionEnv* next = NULL;
  if(self->children_head != NULL && self->children_tail != NULL) {
    iter = self->children_head;
    while(iter != NULL) {
      next = iter->sibling_next;
      iter->sibling_next = NULL;
      iter->sibling_prev = NULL;
      FunctionEnv_del(iter);
      self->children_head = iter;
      iter = next;
    }
    self->children_tail = self->children_head = NULL;
  }
  if(self->objects) {
    Object_assign(&(self->objects), NULL);
  }
  free(self);
}

void FunctionEnv_child_add(FunctionEnv* self, FunctionEnv* child) {
  assert(self != NULL);
  assert(child != NULL);
  assert(child->sibling_next == NULL);
  assert(child->sibling_prev == NULL);
  child->parent = self;
  if(self->children_head == NULL && self->children_tail == NULL) {
    // first child being added
    self->children_head = self->children_tail = child;
  }
  else {
    FunctionEnv* last_child = self->children_tail;
    last_child->sibling_next = child;
    child->sibling_prev = last_child;
    self->children_tail = child;
  }
}

Function* Function_new(Object* (*impl)(Function* fn, Object* args), int arity, Object* params) {
  /* char idbuf[128]; */
  Function* self = calloc(1, sizeof(Function));
  if(self == NULL) {
    ErrorSystem_set(1, "Function_new. calloc failed");
    return NULL;
  }
  /* sprintf(idbuf, "%p", self); */
  /* self->id = QSTRING_NEW1(idbuf); */
  self->impl = impl;
  self->arity = arity;
  self->params = params;
  if(self->arity >= 0 && Object_len(self->params) != self->arity) {
    ErrorSystem_set(1, "Function_new. Invalid arity for given params");
    return NULL;
  }
  self->env = FunctionEnv_new(NULL);
  // The use of the body is entirely dependent on the impl.
  // we do not do anything special to the body.
  self->body = NULL;
  return self;
}

void Function_print(Function* self) {
  printf("Function(");
  Util_vt_set(VT_COLOR_BRIGHT_YELLOW_FG);
  printf("%p", self);
  Util_vt_set(VT_RESET);
  printf(")");
}

Function* Function_clone(Function* self) {
  /* char idbuf[128]; */
  Function* clone = calloc(1, sizeof(Function));
  if(clone == NULL) {
    ErrorSystem_set(1, "Function_clone. calloc failed");
    return NULL;
  }
  /* sprintf(idbuf, "%p", clone); */
  /* clone->id = QSTRING_NEW1(idbuf); */
  clone->impl = self->impl;
  clone->arity = self->arity;
  clone->env = self->env; // TODO: implement FunctionEnv_clone
  clone->params = Object_rc_incr(self->params);
  return clone;
}

/* Function* Function_get_parent(Function* self) { */
/*   Object* parent_obj = Object_accept(Object_bop_hget(self->env, SYMBOL_FNENV_PARENT)); */
/*   if(Object_is_error(parent_obj) || Object_is_null(parent_obj)) { */
/*     Object_assign(&parent_obj, NULL); */
/*     return NULL; */
/*   } */
/*   return parent_obj->impl; */
/* } */
