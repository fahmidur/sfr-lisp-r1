#include <stdio.h>
#include <stdlib.h>
#include "Object.h"
#include "Hash.h"

HashNode* HashNode_new(Object* key, Object* val) {
  Object_rc_incr(key);
  Object_rc_incr(val);
  HashNode* self = calloc(1, sizeof(HashNode));
  self->prev = NULL;
  self->next = NULL;
  self->key = key;
  self->val = val;
  // we simply do not rc_decr val because this
  // HashNode will continue to refer to it.
  return self;
}

Hash* Hash_new() {
  Hash* self = malloc(sizeof(Hash));
  self->size = 0;
  self->buckets = calloc(HASH_BUCKET_SIZE, sizeof(HashNode*));
  int i = 0;
  for(i = 0; i < HASH_BUCKET_SIZE; i++) {
    self->buckets[i] = NULL;
  }
  return self;
}

size_t Hash_kv_set(Hash* self, Object* key, Object* val) {
  Object_rc_incr(key);
  Object_rc_incr(val);

  size_t index = Object_hash(key) % HASH_BUCKET_SIZE;
  HashNode* node = self->buckets[index];
  HashNode* iter = NULL;
  if(node == NULL) {
    node = HashNode_new(key, val);
    self->buckets[index] = node;
  } 
  else {
    iter = node;
    while(iter->next != NULL) {
      iter = iter->next;
    }
    node = HashNode_new(key, val);
    iter->next = node;
  }

  Object_rc_decr(key);
  Object_rc_decr(val);

  self->size++;
  return self->size;
}

Object* Hash_kv_get(Hash* self, Object* key) {
  Object_rc_incr(key);
  size_t index = Object_hash(key) % HASH_BUCKET_SIZE;
  Object* ret = NULL;
  HashNode* iter = self->buckets[index];
  while(iter != NULL) {
    iter = iter->next;
    if(iter->key == key) {
      ret = iter->val;
    }
  }
  Object_rc_decr(key);
  return ret;
}

