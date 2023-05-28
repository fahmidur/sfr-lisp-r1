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

// Can only handle delete from the 
// front. 
void HashNode_del(HashNode* self) {
  if(self->next) {
    HashNode_del(self->next);
  }
  self->next = NULL;
  // TODO: what if key == val
  if(self->key != NULL) {
    self->key = Object_rc_decr(self->key);
  }
  if(self->val != NULL) {
    self->key = Object_rc_decr(self->val);
  }
  self->key = NULL;
  self->val = NULL;
  free(self);
}

void HashNode_set_key(HashNode* self, Object* key) {
  Object_rc_incr(key);
  if(self->key != NULL) {
    Object_rc_decr(key);
    self->key = NULL;
  }
  self->key = key;
}

void HashNode_set_val(HashNode* self, Object* val) {
  Object_rc_incr(val);
  if(self->val != NULL) {
    Object_rc_decr(self->val);
    self->val = NULL;
  }
  self->val = val;
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

void Hash_del(Hash* self) {
  size_t i;
  HashNode* node;
  for(i = 0; i < HASH_BUCKET_SIZE; i++) {
    node = self->buckets[i];
    if(node != NULL) {
      HashNode_del(node);
    }
  }
  free(self->buckets);
  free(self);
}

size_t Hash_size(Hash* self) {
  return self->size;
}

size_t Hash_len(Hash* self) {
  return self->size;
  /* size_t i = 0; */
  /* size_t len = 0; */
  /* size_t bi; */
  /* HashNode* iter = NULL; */
  /* for(i = 0; i < HASH_BUCKET_SIZE; i++) { */
  /*   iter = self->buckets[i]; */
  /*   bi = 0; */
  /*   while(iter != NULL) { */
  /*     bi++; */
  /*     iter = iter->next; */
  /*   } */
  /*   len += bi; */
  /* } */
  /* return len; */
}

size_t Hash_set(Hash* self, Object* key, Object* val) {
  Object_rc_incr(key);
  Object_rc_incr(val);

  size_t index = Object_hash(key) % HASH_BUCKET_SIZE;
  HashNode* node = self->buckets[index];
  HashNode* iter = NULL;
  HashNode* iter_prev = NULL;
  char clash = 0;
  if(node == NULL) {
    node = HashNode_new(key, val);
    self->buckets[index] = node;
    self->size++;
    printf("** donuts ** size=%zu\n", self->size);
  }
  else {
    iter = node;
    while(iter != NULL) {
      if(Object_cmp(iter->key, key) == 0) {
        clash = 1;
        node = iter;
        break;
      }
      iter_prev = iter;
      iter = iter->next;
    }
    if(clash) {
      HashNode_set_val(node, val);
    } 
    else {
      node = HashNode_new(key, val);
      iter_prev->next = node;
      node->prev = iter_prev;
      self->size++;
    }
  }

  Object_rc_decr(key);
  Object_rc_decr(val);

  return self->size;
}

Object* Hash_get(Hash* self, Object* key) {
  Object_rc_incr(key);
  size_t index = Object_hash(key) % HASH_BUCKET_SIZE;
  Object* ret = NULL;
  HashNode* iter = self->buckets[index];
  while(iter != NULL) {
    if(Object_cmp(iter->key, key) == 0) {
      ret = iter->val;
    }
    iter = iter->next;
  }
  Object_rc_decr(key);
  return ret;
}

