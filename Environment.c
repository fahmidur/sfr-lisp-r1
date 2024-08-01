#include "Environment.h"

Environment* Environment_new(Environment* parent) {
  Environment* self = calloc(1, sizeof(Environment));

  if(self == NULL) {
    // ERROR: unable to reserve memory
    ErrorSystem_set(1, "Environment_new. calloc failed");
    return self;
  }

  self->children_head = NULL;
  self->children_tail = NULL;
  self->sibling_next = NULL;
  self->sibling_prev = NULL;

  // this is where we keep a Symbol -> Object map
  self->objects = QHASH_NEW1();

  if(parent != NULL) {
    // Establish the parent-child relationship
    Environment_child_add(parent, self);
  }
  return self;
}

void Environment_del(Environment* self) {
  assert(self != NULL);
  // assert that the child is detached from siblings
  assert(self->sibling_prev == NULL);
  assert(self->sibling_next == NULL);
  Environment* iter = NULL;
  Environment* next = NULL;

  // delete all children
  if(self->children_head != NULL && self->children_tail != NULL) {
    iter = self->children_head;
    while(iter != NULL) {
      next = iter->sibling_next;
      // detach this child from its siblings
      iter->sibling_next = NULL;
      iter->sibling_prev = NULL;
      // now delete the child recursively
      Environment_del(iter);
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

/**
 * @private
 * Setup the parent-child relationship between a parent environment 'self'
 * and the child environment 'child'.
 **/
void Environment_child_add(Environment* self, Environment* child) {
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
    Environment* last_child = self->children_tail;
    last_child->sibling_next = child;
    child->sibling_prev = last_child;
    self->children_tail = child;
  }
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
    ret = Environment_get(self->parent, key);
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
  if(self->parent) {
    ret += Environment_len(self->parent);
  }
  ret += Object_len(self->objects);
  return ret;
}
