#include "Error.h"
#include "Function.h"
#include "Object.h"

void FunctionSystem_init() {
}

void FunctionSystem_done() {
}

Function* Function_new(
  Object* name,
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

  if(!Object_is_null(name)) {
    self->name = Object_accept(name);
    // a function's name if present must live in the environment for ease
    // of recursion.
    /* Object_top_hset(self->env, name, self); */
  } else {
    self->name = Object_new_null();
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
  assert(self != NULL);
  if(argv == NULL) {
    argv = Object_new_null();
  }
  /* Object_rc_incr(argv); */
  Object_accept(argv);
  /* if(self->arity >= 0) { */
  /*   if(Object_len(argv) == (self->arity+1)) { */
  /*     ErrorSystem_set(1, "Function_call. argv arity mismatch"); */
  /*     return NULL; */
  /*   } */
  /* } */

  Object* tmpEnv = Object_new(SYMBOL_ENVIRONMENT, 1, Environment_new());
  Environment_child_attach(self->env, tmpEnv);

  if(
      !Object_is_null(argv) && Object_type(argv) == SYMBOL_LIST &&
      !Object_is_null(self->params) && Object_type(self->params) == SYMBOL_LIST &&
      Object_len(argv) > 1 && Object_len(self->params) > 0
    ) {
    ListIter* argv_iter = ListIter_new(argv->impl);
    ListIter_head(argv_iter); // go to head
    ListIter_next(argv_iter); // go one more past head because we ignore the first arg
    ListIter* params_iter = ListIter_new(self->params->impl);
    ListIter_head(params_iter);
    while(!(ListIter_at_end(argv_iter) || ListIter_at_end(params_iter))) {
      Object_top_hset(tmpEnv, ListIter_get_val(params_iter), ListIter_get_val(argv_iter));
      //---
      ListIter_next(argv_iter);
      ListIter_next(params_iter);
    }
    ListIter_del(argv_iter);
    ListIter_del(params_iter);
  }

  Object* ret = (self->impl)(self, tmpEnv, argv);
  if(ret == NULL) {
    ret = Object_new_null();
  }
  // release the argv object, we do not need it anymore.
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

Object* Function_obj_init(Object* self) {
  assert(self != NULL);
  if(Object_type(self) != SYMBOL_FUNCTION) {
    return QERROR("Invalid type for Function_obj_init");
  }
  Function* fn = (Function*) self->impl;
  if(!Object_is_null(fn->name)) {
    Object_top_hset(fn->env, fn->name, self);
  }
  return NULL;
}
