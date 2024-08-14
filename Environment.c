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

  Object_bop_push(self->children, child_obj);
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

  //TODO: finish this

_return:
  Object_rc_decr(self_obj);
  Object_rc_decr(child_obj);

}

/* void Environment_set_parent(Environment* self, Object* parent_obj) { */
/*   assert(self != NULL); */
/*   assert(parent_obj != NULL); */
/*   assert(Object_type(parent_obj) == SYMBOL_ENVIRONMENT); */

/*   Object* old_parent = self->parent; */
/*   if(old_parent != NULL) { */
/*     //TODO: remove the old parent object */
/*   } */
/*   self->parent = Object_accept(parent); */
/*   Environment* parent_env = parent->impl; */
/*   /1* Object_bop_push(parent_env->children, self); *1/ */
/* } */

void Environment_del(Environment* self) {
  assert(self != NULL);
  printf("  Environment_del. Deleting self_\t\tself=%p\n", self);

  Environment* iter = NULL;
  Environment* next = NULL;

  if(self->parent != NULL) {
    printf("  Environment_del. detaching parent...\n");
    // detach parent-child relationship to this Environment
  }


  // release our reference to the objects Hash
  if(self->objects != NULL) {
    printf("  Environment_del. Releasing self->objects Hash\n");
    Object_assign(&(self->objects), NULL);
  }
  printf("  Environment_del. free(self)\n");
  free(self);
}

/**
 * @private
 * Setup the parent-child relationship between a parent environment 'self'
 * and the child environment 'child'.
 **/
/* void Environment_child_attach(Environment* self, Environment* child) { */
/*   assert(self != NULL); */
/*   assert(child != NULL); */
/*   assert(child->sibling_next == NULL); */
/*   assert(child->sibling_prev == NULL); */
/*   child->parent = self; */
/*   if(self->children_head == NULL && self->children_tail == NULL) { */
/*     // first child being added */
/*     self->children_head = self->children_tail = child; */
/*   } */
/*   else { */
/*     Environment* last_child = self->children_tail; */
/*     last_child->sibling_next = child; */
/*     child->sibling_prev = last_child; */
/*     self->children_tail = child; */
/*   } */
/* } */

/* void Environment_child_detach(Environment* self, Environment* child) { */
/*   assert(self != NULL); */
/*   assert(child != NULL); */
/*   child->parent = NULL; */
/*   Environment* iter = self->children_head; */
/*   Environment* sibling_prev = NULL; */
/*   Environment* sibling_next = NULL; */
/*   while(iter != NULL) { */
/*     if(iter == child) { */
/*       printf("  found matching child self=%p iter=%p child=%p\n", self, iter, child); */
/*       if(iter == self->children_head && iter == self->children_tail) { */
/*         printf("  child is the only child\n"); */
/*         // iter is child which is the only child */
/*         self->children_head = NULL; */
/*         self->children_tail = NULL; */
/*         iter->sibling_next = NULL; */
/*         iter->sibling_prev = NULL; */
/*       } */
/*       else */
/*       if(iter == self->children_head) { */
/*         printf("  child is at head\n"); */
/*         // iter is child which is at head position */
/*         self->children_head = iter->sibling_next; */
/*         self->children_head->sibling_prev = NULL; */
/*         iter->sibling_next = NULL; */
/*         iter->sibling_prev = NULL; */
/*       } */
/*       else */
/*       if(iter == self->children_tail) { */
/*         printf("  child is at tail\n"); */
/*         // iter is child which is at tail position */
/*         self->children_tail = iter->sibling_prev; */
/*         self->children_tail->sibling_next = NULL; */
/*         iter->sibling_next = NULL; */
/*         iter->sibling_prev = NULL; */
/*       } */
/*       else { */
/*         printf("  child is somewhere in between head and tail\n"); */
/*         // iter is child somewhere in between */
/*         sibling_prev = iter->sibling_prev; */
/*         sibling_next = iter->sibling_next; */
/*         sibling_prev->sibling_next = sibling_next; */
/*         sibling_next->sibling_prev = sibling_prev; */
/*         iter->sibling_prev = NULL; */
/*         iter->sibling_prev = NULL; */
/*       } */
/*       break; */
/*     } */
/*     iter = iter->sibling_next; */
/*   } */
/* } */

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

