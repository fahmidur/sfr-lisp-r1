#include "Error.h"
#include "Function.h"
#include "Object.h"

void FunctionSystem_init() {
}

void FunctionSystem_done() {
}

Function* Function_new(
  Object* env, 
  Object* (*impl)(Function* fn, Object* env, Object* args), 
  int arity, 
  Object* params,
  Object* body
) {
  Function* self = calloc(1, sizeof(Function));
  if(self == NULL) {
    ErrorSystem_set(1, "Function_new. calloc failed");
    return NULL;
  }
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

  if(env == NULL) {
    // it is not possible for a function to exist without an environment
    self->env = Object_new(SYMBOL_ENVIRONMENT, 1, Environment_new());
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

  Object* tmpEnv = Object_new(SYMBOL_ENVIRONMENT, 1, Environment_new());
  Environment_child_attach(self->env, tmpEnv);
  //TODO: replace above with Object-level API
  Object* ret = (self->impl)(self, tmpEnv, argv);
  if(ret == NULL) {
    ret = Object_new_null();
  }
  Object_rc_decr(argv);
  Object_assign(&tmpEnv, NULL);
  return Object_return(ret);
}

// Set key/val in the Functions environment
Object* Function_env_set(Function* self, Object* key, Object* val) {
  assert(self != NULL);
  assert(self->env != NULL);
  return Object_top_hset(self->env, key, val);
}

Object* Function_env_get(Function* self, Object* key) {
  assert(self != NULL);
  assert(self->env != NULL);
  return Object_bop_hget(self->env, key);
}

void Function_del(Function* self) {
  Object_assign(&(self->env), NULL);
  Object_assign(&(self->params), NULL);
  Object_assign(&(self->body), NULL);
  free(self);
}

