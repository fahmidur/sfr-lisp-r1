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
  } else {
    self->env = Object_accept(env);
  }

  if(!Object_is_null(name)) {
    self->name = Object_accept(name);
    // a function's name if present must live in the environment for ease
    // of recursion.
    /* Object_top_hset(self->env, name, self); */
  } else {
    self->name = Object_new_null();
  }

  return self;
}

void Function_print(Function* self) {
  printf("Function(");
  Util_vt_set(VT_COLOR_BRIGHT_YELLOW_FG);
  printf("addr=%p impl=%p arity=%d", self, self->impl, self->arity);
  Util_vt_set(VT_RESET);
  if(Object_system_delete_recurse()) {
    // we cannot print these objects which many have been deleted
    // during system shutdown.
    if(self->name) {
      ObjectUtil_eprintf(" name=%v", self->name);
    }
    ObjectUtil_eprintf(" env=%v", self->env);
    if(self->body != NULL && !Object_is_null(self->body)) {
      ObjectUtil_eprintf(" body=%v", self->body);
    }
  }
  Util_vt_set(VT_COLOR_BRIGHT_YELLOW_FG);
  Util_vt_set(VT_RESET);
  printf(")");
}

Object* Function_call(Function* self, Object* argv) {
  assert(self != NULL);
  if(argv == NULL) {
    argv = Object_new_null();
  } else {
    argv = Object_accept(argv);
  }

  /* if(self->arity >= 0) { */
  /*   if(Object_len(argv) == (self->arity+1)) { */
  /*     ErrorSystem_set(1, "Function_call. argv arity mismatch"); */
  /*     return NULL; */
  /*   } */
  /* } */

  Object* tmpEnv = Object_new(SYMBOL_ENVIRONMENT, 1, Environment_new());
  assert(tmpEnv->rc == 1);
  /* if(Object_cmp(self->name, QSTRING("lambda001")) == 0) { */
  /*   printf("donuts. Function_call over here"); */
  /* } */
  assert(self->env != NULL);
  Environment_child_attach(self->env, tmpEnv);

  if(
      !Object_is_null(argv) && Object_type(argv) == SYMBOL_LIST &&
      !Object_is_null(self->params) && Object_type(self->params) == SYMBOL_LIST &&
      Object_len(argv) > 0 && Object_len(self->params) > 0
    ) {
    // construct temporary environment with keys params x argv
    ListIter* argv_iter = ListIter_new(argv->impl);
    ListIter_head(argv_iter); // go to head
    ListIter* params_iter = ListIter_new(self->params->impl);
    ListIter_head(params_iter);
    while(!(ListIter_at_end(argv_iter) || ListIter_at_end(params_iter))) {
      Object_reject(Object_top_hset(tmpEnv, ListIter_get_val(params_iter), ListIter_get_val(argv_iter)));
      //---
      ListIter_next(argv_iter);
      ListIter_next(params_iter);
    }
    ListIter_del(argv_iter);
    ListIter_del(params_iter);
  }

  /* dbg_printf("donuts. F=%s L=%d. rtcount = %zu\n", __FILE__, __LINE__, Object_system_rtcount()); */
  // NOTE: there is some problem before and after this line causing rc of the argv elements
  // to be 1 higher than expected.
  Object* ret = (self->impl)(self, tmpEnv, argv);
  if(ret == NULL) {
    ret = Object_new_null();
  } else {
    ret = Object_accept(ret);
  }
  /* dbg_printf("donuts. F=%s L=%d. rtcount = %zu\n", __FILE__, __LINE__, Object_system_rtcount()); */
  // release the argv object, we do not need it anymore.
  /* Object_rc_decr(argv); */
  Object_assign(&argv, NULL);
  assert(Object_is_null(argv));
  Environment_child_detach(self->env, tmpEnv);
  Object_assign(&tmpEnv, NULL);
  // at this point we expect the tmpEnv to be destroyed.
  assert(Object_is_null(tmpEnv));
  Object_return(ret);
  Object_rc_decr(ret);
  return ret;
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
  if(Object_system_delete_recurse()) {
    Object_assign(&(self->env), NULL);
    Object_assign(&(self->params), NULL);
    Object_assign(&(self->body), NULL);
  }
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

ssize_t Function_len(Function* self) {
  return 1;
}
