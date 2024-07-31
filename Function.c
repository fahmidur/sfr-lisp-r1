#include "Error.h"
#include "Function.h"
#include "Object.h"

Object* SYMBOL_FNENV_PARENT;

void FunctionSystem_init() {
  /* SYMBOL_FNENV_PARENT = QSYMBOL("_FNENV_PARENT"); */
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

  // this is where we keep a Symbol -> Object map
  self->objects = QHASH_NEW1();

  if(parent != NULL) {
    FunctionEnv_child_add(parent, self);
  }
  return self;
}

void FunctionEnv_del(FunctionEnv* self) {
  assert(self != NULL);
  // assert that the child is detached from siblings
  assert(self->sibling_prev == NULL);
  assert(self->sibling_next == NULL);
  FunctionEnv* iter = NULL;
  FunctionEnv* next = NULL;

  // delete all children
  if(self->children_head != NULL && self->children_tail != NULL) {
    iter = self->children_head;
    while(iter != NULL) {
      next = iter->sibling_next;
      // detach this child from its siblings
      iter->sibling_next = NULL;
      iter->sibling_prev = NULL;
      // now delete the child recursively
      FunctionEnv_del(iter);
      // move the head over to next child
      self->children_head = next;
      iter = next;
    }
    self->children_tail = self->children_head = NULL;
  }
  if(self->objects) {
    // release our reference to the objects Hash
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

Function* Function_new(
  Function* parent, 
  Object* (*impl)(Function* fn, Object* args), 
  int arity, 
  Object* params,
  Object* body
) {
  Function* self = calloc(1, sizeof(Function));
  if(self == NULL) {
    ErrorSystem_set(1, "Function_new. calloc failed");
    return NULL;
  }
  self->parent = parent;
  self->impl = impl;
  self->arity = arity;
  if(params == NULL) {
    self->params = Object_new_null();
  } else {
    self->params = Object_accept(params);
  }
  if(body == NULL) {
    self->body = Object_new_null();
  } else {
    self->body = Object_accept(body);
  }

  // validate the params to match the given arity
  if(self->arity >= 0 && Object_len(self->params) != self->arity) {
    ErrorSystem_set(1, "Function_new. Invalid arity for given params");
    return NULL;
  }
  if(parent == NULL) {
    self->env = FunctionEnv_new(NULL);
  } else {
    self->env = FunctionEnv_new(parent->env);
  }
  // The use of the body is entirely dependent on the impl.
  // we do not do anything special to the body.
  self->body = NULL;
  return self;
}

void Function_print(Function* self) {
  printf("Function(");
  Util_vt_set(VT_COLOR_BRIGHT_YELLOW_FG);
  printf("addr=%p arity=%d", self, self->arity);
  Util_vt_set(VT_RESET);
  printf(")");
  if(self->parent != NULL) {
    printf(" < ");
    Function_print(self->parent);
  }
}

Object* Function_call(Function* self, Object* argv) {
  if(argv == NULL) {
    argv = Object_new_null();
  }
  Object_rc_incr(argv);
  /* if(self->arity >= 0) { */
  /*   if(Object_len(argv) == (self->arity+1)) { */
  /*     ErrorSystem_set(1, "Function_call. argv arity mismatch"); */
  /*     return NULL; */
  /*   } */
  /* } */
  Object* ret = (self->impl)(self, argv);
  if(ret == NULL) {
    ret = Object_new_null();
  }
  Object_rc_decr(argv);
  return Object_return(ret);
}

// Set key/val in the Functions environment
Object* Function_env_set(Function* self, Object* key, Object* val) {
  assert(self != NULL);
  assert(self->env != NULL);
  assert(self->env->objects != NULL);
  return Object_top_hset(self->env->objects, key, val);
}

Object* Function_env_get(Function* self, Object* key) {
  assert(self != NULL);
  assert(self->env != NULL);
  assert(self->env->objects != NULL);
  return Object_bop_hget(self->env->objects, key);
}

void Function_del(Function* self) {
  if(self->env != NULL) {
    FunctionEnv_del(self->env);
  }
  self->env = NULL;
  self->parent = NULL;
  free(self);
}

/* Function* Function_clone(Function* self) { */
/*   /1* char idbuf[128]; *1/ */
/*   Function* clone = calloc(1, sizeof(Function)); */
/*   if(clone == NULL) { */
/*     ErrorSystem_set(1, "Function_clone. calloc failed"); */
/*     return NULL; */
/*   } */
/*   /1* sprintf(idbuf, "%p", clone); *1/ */
/*   /1* clone->id = QSTRING_NEW1(idbuf); *1/ */
/*   clone->impl = self->impl; */
/*   clone->arity = self->arity; */
/*   clone->env = self->env; // TODO: implement FunctionEnv_clone */
/*   clone->params = Object_rc_incr(self->params); */
/*   return clone; */
/* } */

/* Function* Function_get_parent(Function* self) { */
/*   Object* parent_obj = Object_accept(Object_bop_hget(self->env, SYMBOL_FNENV_PARENT)); */
/*   if(Object_is_error(parent_obj) || Object_is_null(parent_obj)) { */
/*     Object_assign(&parent_obj, NULL); */
/*     return NULL; */
/*   } */
/*   return parent_obj->impl; */
/* } */
