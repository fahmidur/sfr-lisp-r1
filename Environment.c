#include "Environment.h"
#include "Object.h"

Environment* Environment_new() {
  Environment* self = calloc(1, sizeof(Environment));

  if(self == NULL) {
    // ERROR: unable to reserve memory
    ErrorSystem_set(1, "Environment_new. calloc failed");
    return self;
  }

  // this is where we keep a Symbol -> Object map
  self->objects = QHASH_NEW1();
  self->children = QLIST_NEW1();

  return self;
}

void Environment_child_attach(Object* self_obj, Object* child_obj) {
  assert(self_obj != NULL);
  assert(child_obj != NULL);
  assert(Object_type(self_obj) == SYMBOL_ENVIRONMENT);
  assert(Object_type(child_obj) == SYMBOL_ENVIRONMENT);

  Object_rc_incr(self_obj);
  Object_rc_incr(child_obj);

  Environment* self = self_obj->impl;
  Environment* child = child_obj->impl;

  if(child->parent != NULL) {
    if(child->parent == self_obj) {
      // nothing to do
      goto _return;
    }
    else {
      // we must unset the old parent because an Environment
      // can only have one parent.
      Environment_child_detach(child->parent, child_obj);
    }
  }

  Object_reject(Object_bop_push(self->children, child_obj));
  child->parent = Object_accept(self_obj);

_return:
  Object_rc_decr(self_obj);
  Object_rc_decr(child_obj);
}

void Environment_child_detach(Object* self_obj, Object* child_obj) {
  assert(self_obj != NULL);
  assert(child_obj != NULL);
  assert(Object_type(self_obj) == SYMBOL_ENVIRONMENT);
  assert(Object_type(child_obj) == SYMBOL_ENVIRONMENT);

  Object_rc_incr(self_obj);
  Object_rc_incr(child_obj);

  Environment* self = self_obj->impl;
  Environment* child = child_obj->impl;

  if(child->parent != NULL) {
    // release pointer to child->parent
    Object_rc_decr(child->parent);
  }
  child->parent = NULL;

  Object* self_children = Object_rc_incr(self->children);

  Object_bop_rem(self_children, child_obj);

_return:
  Object_rc_decr(self_children);
  Object_rc_decr(self_obj);
  Object_rc_decr(child_obj);
}

void Environment_del(Environment* self) {
  assert(self != NULL);
  dbg_printf("  Environment_del. Deleting self_\t\tself=%p\n", self);

  Environment* iter = NULL;
  Environment* next = NULL;

  if(self->parent != NULL && Object_system_delete_recurse()) {
    dbg_printf("  Environment_del. detaching parent...\n");
    // TODO: detach parent-child relationship to this Environment
  }

  // release our reference to the objects Hash
  if(self->objects != NULL && Object_system_delete_recurse()) {
    dbg_printf("  Environment_del. Releasing self->objects Object<Hash>\n");
    Object_assign(&(self->objects), NULL);
  }

  if(self->children != NULL && Object_system_delete_recurse()) {
    dbg_printf("  Environment_del. Releasing self->children Object<List>\n");
    Object_assign(&(self->children), NULL);
  }
  dbg_printf("  Environment_del. free(self)\n");
  free(self);
}

/**
 * Same interface as Hash_set.
 * Sets the key in the current Environment Hash.
 * It mostly wraps Hash_set for the objects Hash.
 **/
Object* Environment_set(Environment* self, Object* key, Object* val) {
  assert(self != NULL);
  assert(self->objects != NULL);
  return Object_top_hset(self->objects, key, val);
}

/**
 * Same interface as Hash_get.
 * Get a value given key, if it does not exist
 * go up to the parent Environment
 **/
Object* Environment_get(Environment* self, Object* key) {
  assert(self != NULL);
  assert(self->objects != NULL);
  Object* ret = Object_bop_hget(self->objects, key);
  if(ret == NULL) {
    ret = Object_new_null();
  }
  ret = Object_accept(ret);
  if(Object_is_null(ret) && self->parent != NULL) {
    ret = Environment_get(self->parent->impl, key);
    if(ret == NULL) {
      ret = Object_new_null();
    }
    ret = Object_accept(ret);
  }
  return Object_return(ret);
}

Object* Environment_rem(Environment* self, Object* key) {
  assert(self != NULL);
  assert(self->objects != NULL);
  // notice that removing an item is not recursive, it 
  // does not go up to remove items in the parent environment.
  return Object_bop_hrem(self->objects, key);
}

ssize_t Environment_len(Environment* self) {
  assert(self != NULL);
  ssize_t ret = 0;
  if(self->parent != NULL) {
    ret += Environment_len(self->parent->impl);
  }
  ret += Object_len(self->objects);
  return ret;
}

void Environment_print(Environment* self) {
  assert(self != NULL);
  printf("Environment(");
  Util_vt_set(VT_COLOR_BRIGHT_YELLOW_FG);
  printf("%p", self);
  Util_vt_set(VT_RESET);
  printf(")");
  if(self->parent != NULL) {
    printf(" < ");
    Environment_print(self->parent->impl);
  }
}

char Environment_zero(Environment* self) {
  assert(self != NULL);
  assert(self->objects != NULL);
  Object* ret = Object_zero(self->objects);
  if(Object_is_error(ret)) {
    ErrorSystem_set(1, "Environment_set. Failed to zero self->objects Hash");
    return 0;
  }
  return 1;
}

