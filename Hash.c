#include <stdio.h>
#include <stdlib.h>
#include "Object.h"
#include "Hash.h"
#include "Error.h"
#include "Util.h"

HashNode* HashNode_new(Object* key, Object* val) {
  HashNode* self = NULL;
  Object* tmp = NULL;
  Object_rc_incr(key);
  Object_rc_incr(val);
  self = calloc(1, sizeof(HashNode));
  self->prev = NULL;
  self->next = NULL;
  self->key = Object_rc_incr(key);
  self->val = Object_rc_incr(val);
_return:
  Object_rc_decr(key);
  Object_rc_decr(val);
  return self;
}

void HashNode_print(HashNode* self) {
#ifdef DEBUG
  ObjectUtil_eprintf("HashNode(%v => %v)", self->key, self->val);
#else 
  ObjectUtil_eprintf("  %v => %v", self->key, self->val);
#endif
}

void HashNode_del(HashNode* self) {
  if(self->prev != NULL && self->next != NULL) {
    HashNode* prev = self->prev;
    HashNode* next = self->next;
    prev->next = next;
    next->prev = prev;
    self->prev = NULL;
    self->next = NULL;
  }
  assert(self->prev == NULL && self->next == NULL);
  if(self->key != NULL && Object_system_delete_recurse()) {
#ifdef DEBUG
    printf("Releasing HashNode key(%p)\n", self->key);
#endif
    self->key = Object_rc_decr(self->key);
  }
  if(self->val != NULL && Object_system_delete_recurse()) {
#ifdef DEBUG
    printf("Releasing HashNode val(%p, rc=%d)\n", self->val, self->val->rc);
    if(self->val != NULL && !Object_is_container(self->val)) {
      ObjectUtil_eprintf("  HashNode val(%p, rc=%d) = %v\n", self->val, self->val->rc, self->val);
    }
#endif
    self->val = Object_rc_decr(self->val);
  }
  self->key = NULL;
  self->val = NULL;
  free(self);
}

void HashNode_del_fwd(HashNode* self) {
  if(self->next) {
    HashNode_del_fwd(self->next);
  }
  self->next = NULL;
  self->prev = NULL;
  HashNode_del(self);
}

void HashNode_set_key(HashNode* self, Object* key) {
  Object_rc_incr(key);
  if(self->key != NULL) {
    Object_assign(&(self->key), NULL);
  }
  self->key = key;
}

void HashNode_set_val(HashNode* self, Object* val) {
  Object_rc_incr(val);
  if(self->val != NULL) {
    Object_assign(&(self->val), NULL);
  }
  self->val = val;
}

Hash* Hash_new() {
  Hash* self = calloc(1, sizeof(Hash));
  self->size = 0;
  size_t buckets_size = HASH_BUCKETS_ISIZE;
  self->buckets_size = buckets_size;
  self->buckets = calloc(buckets_size, sizeof(HashNode*));
  int i = 0;
  for(i = 0; i < buckets_size; i++) {
    self->buckets[i] = NULL;
  }
  self->bitset = calloc(MAX(1, buckets_size/8), sizeof(char));
  self->grow_count = 0;
  return self;
}

void Hash_del(Hash* self) {
  size_t i;
  HashNode* node;
  size_t buckets_size = self->buckets_size;
  for(i = 0; i < buckets_size; i++) {
    node = self->buckets[i];
    if(node != NULL) {
      HashNode_del_fwd(node);
    }
  }
  free(self->bitset);
  free(self->buckets);
  free(self);
}

float Hash_load(Hash* self) {
  return (float)self->size / (float)self->buckets_size;
}

void Hash_grow(Hash* self) {
  size_t old_buckets_size = self->buckets_size;
  size_t new_buckets_size = 2*old_buckets_size;
  HashNode** old_buckets = self->buckets;
  HashNode** new_buckets = calloc(new_buckets_size, sizeof(HashNode*));
  if(new_buckets == NULL) {
    printf("ERROR: Hash_grow failed to allocate memory\n");
    exit(1);
  }

  size_t i;
  size_t new_index = 0;
  HashNode* iter = NULL;
  HashNode* iter_next = NULL;

  for(i = 0; i < old_buckets_size; i++) {
    iter = old_buckets[i];
    while(iter != NULL) {
      // save the iter->next for this iter loop
      iter_next = iter->next;

      // detach the iter from old_buckets
      iter->next = NULL;
      iter->prev = NULL;
      if(iter_next != NULL) {
        iter_next->prev = NULL;
      }
      old_buckets[i] = iter_next;
      
      // calculate new index
      new_index = Object_hash(iter->key) % new_buckets_size;

      // insert into new_buckets
      iter->next = new_buckets[new_index];
      new_buckets[new_index] = iter;

      //---
      iter = iter_next;
    }
  }

  free(self->buckets); self->buckets = NULL;
  self->buckets = new_buckets;
  self->buckets_size = new_buckets_size;
  self->grow_count++;
}

void Hash_resize(Hash *self) {
  float load = Hash_load(self); 
  if(load < 0.75) {
    // no need to resize
    return;
  }
  // printf("donuts. Hash(%p) growing. old buckets_size=%lu\n", self, self->buckets_size);
  Hash_grow(self);
}

ssize_t Hash_size(Hash* self) {
  return self->size;
}

ssize_t Hash_len(Hash* self) {
  return self->size;
}

void Hash_rem(Hash* self, Object* key) {
  Object_rc_incr(key);
  size_t index = Object_hash(key) % self->buckets_size;
  HashNode* iter = self->buckets[index];
  size_t iter_idx = 0;
  HashNode* found = NULL;
  size_t found_idx = 0;
  while(iter != NULL) {
    if(Object_cmp(iter->key, key) == 0) {
      found = iter;
      found_idx = iter_idx;
      break;
    }
    iter = iter->next;
    iter_idx++;
  }
  if(found != NULL) {
    HashNode_del(found);
    self->size--;
    if(found_idx == 0) {
      self->buckets[index] = NULL;
    }
  }
  Object_rc_decr(key);
}

Object* Hash_set(Hash* self, Object* key, Object* val) {
  Hash_resize(self);
  if(key == NULL) {
    return QERROR_NEW0("key cannot be NULL");
  }
  if(val == NULL) {
    return QERROR_NEW0("val cannot be NULL");
  }
  if(key == val) {
    return QERROR_NEW0("key cannot equal val");
  }

  Object_rc_incr(key);
  Object_rc_incr(val);

  Object* ret = Object_new_null();
  Object* key_clone = Object_accept(Object_clone(key));
  if(Object_is_error(key_clone)) {
    Object_move(&ret, &key_clone);
    goto _return;
  }

  size_t index = Object_hash(key_clone) % self->buckets_size;

  HashNode* node = self->buckets[index];
  HashNode* iter = NULL;
  HashNode* iter_prev = NULL;
  char clash = 0;

  if(node == NULL) {
    node = HashNode_new(key_clone, val);
    if(node == NULL) {
      ret = Object_return(QERROR_NEW0("Failed to create HashNode"));
      goto _return;
    }
    self->buckets[index] = node;
    self->size++;
  }
  else {
    iter = node;
    while(iter != NULL) {
      if(Object_cmp(iter->key, key_clone) == 0) {
        clash = 1;
        node = iter;
        break;
      }
      iter_prev = iter;
      iter = iter->next;
    }
    if(clash) {
      /* ObjectUtil_eprintf("Hash_set(%p). key(%v) already exists. new val=%v\n", self, key_clone, val); */
      HashNode_set_val(node, val);
    } 
    else {
      node = HashNode_new(key_clone, val);
      if(node == NULL) {
        ret = Object_return(QERROR_NEW0("Failed to create HashNode"));
        goto _return;
      }
      iter_prev->next = node;
      node->prev = iter_prev;
      self->size++;
    }
  }
_return:
  Object_rc_decr(key);
  Object_rc_decr(val);
  assert(ret != NULL);
  return ret;
}

Object* Hash_get(Hash* self, Object* key) {
  Object_rc_incr(key);
  size_t index = Object_hash(key) % self->buckets_size;
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

char Hash_has(Hash* self, Object* key) {
  Object_rc_incr(key);
  size_t index = Object_hash(key) % self->buckets_size;
  char ret = 0;
  HashNode* iter = self->buckets[index];
  while(iter != NULL) {
    if(Object_cmp(iter->key, key) == 0) {
      ret = 1;
    }
    iter = iter->next;
  }
  Object_rc_decr(key);
  return ret;
}

void Hash_print(Hash* self) {
  HashNode* iter = NULL;
  size_t i = 0;
  printf("Hash(");
  if(self->size > 0) {
    printf("\n");
  }
  size_t printed = 0;
  size_t buckets_size = self->buckets_size;
  for(i = 0; i < buckets_size; i++) {
    iter = self->buckets[i];
    while(iter != NULL) {
      if(printed > 0) {
        printf(",\n");
      }
      printf("  "); HashNode_print(iter);
      printed++;
      iter = iter->next;
    }
  }
  if(self->size > 0) {
    printf("\n");
  }
  printf(")\n");
}

/**
 * Reset this hash to the empty hash
 * deleting everything
 */
char Hash_zero(Hash* self) {
  /* assert(self != NULL); */
  if(self == NULL) {
    ErrorSystem_set(1, "Hash_zero. self is NULL");
    return 0;
  }
  HashNode* iter = NULL;
  size_t i = 0;
  size_t buckets_size = self->buckets_size;
  for(i = 0; i < buckets_size; i++) {
    iter = self->buckets[i];
    if(iter != NULL) {
      HashNode_del_fwd(iter);
    }
    self->buckets[i] = NULL;
  }
  self->size = 0;
  return 1;
}

HashIter* HashIter_new(Hash* hash) {
  HashIter* self = calloc(1, sizeof(HashIter));
  self->hash = hash;
  self->at_pos = HASH_ITER_POS_NIL;
  self->cbucket = 0;
  self->cnode = NULL;
  return self;
}

void HashIter_del(HashIter* self) {
  if(self == NULL) {
    return;
  }
  free(self);
}

// Go to the first element.
// Define 'head': The first element of Iterable
HashIter* HashIter_head(HashIter* self) {
  Hash* hash = self->hash;
  self->at_pos = HASH_ITER_POS_BEG;
  self->cbucket = 0;
  self->cnode = NULL;

  // Look for the first non-empty bucket
  size_t buckets_size = hash->buckets_size;
  while(self->cbucket < buckets_size && hash->buckets[self->cbucket] == NULL) {
    self->cbucket++;
  }
  self->at_pos = HASH_ITER_POS_INN;
  // cbucket is the first non-empty bucket index.
  // OR 
  // you have reached the end.
  if(self->cbucket == buckets_size) {
    self->at_pos = HASH_ITER_POS_END;
    return self;
  }
  self->cnode = hash->buckets[self->cbucket];
  return self;
}

HashIter* HashIter_next(HashIter* self) {
  Hash* hash = self->hash;

  if(self->at_pos == HASH_ITER_POS_NIL) { 
    // You are in limbo, neither the beginning or the end.
    // Treat this position as the 'beg'.
    // The user most likely forgot to call: HashIter_head(iter)
    self->at_pos = HASH_ITER_POS_BEG;
  }

  // Special position 'end' denotes a place after the last element.
  if(self->at_pos == HASH_ITER_POS_END) {
    // You have reached the end. 
    // The only way out of this position is with head and tail methods.
    return self;
  }

  // Special position 'beg' denotes a place before even the first element.
  // Automatically go to the head element and stop.
  if(self->at_pos == HASH_ITER_POS_BEG) {
    return HashIter_head(self);
  }

  // Go to next node if there is one.
  if(self->cnode != NULL && self->cnode->next != NULL) {
    self->cnode = self->cnode->next;
    return self;
  } 

  // Look for the next non-empty bucket
  self->cnode = NULL;
  self->cbucket++;
  size_t buckets_size = hash->buckets_size;
  while(self->cbucket < buckets_size && hash->buckets[self->cbucket] == NULL) {
    self->cbucket++;
  }
  if(self->cbucket == buckets_size) {
    self->at_pos = HASH_ITER_POS_END;
    return self;
  }
  self->cnode = hash->buckets[self->cbucket];
  return self;
}

Object* HashIter_get_key(HashIter* self) {
  if(self->cnode == NULL) {
    return NULL;
  }
  return self->cnode->key;
}

Object* HashIter_get_val(HashIter* self) {
  if(self->cnode == NULL) {
    return NULL;
  }
  return self->cnode->val;
}

char HashIter_at_beg(HashIter* self) {
  return self->at_pos == HASH_ITER_POS_BEG;
}

char HashIter_at_end(HashIter* self) {
  return self->at_pos == HASH_ITER_POS_END;
}

Hash* Hash_clone(Hash* self) {
  Hash* clone = Hash_new();
  Object* key = NULL;
  Object* val = NULL;
  HashIter* iter = HashIter_new(self);
  HashIter_head(iter);
  while(!HashIter_at_end(iter)) {
    key = HashIter_get_key(iter);
    val = HashIter_get_val(iter);
    /* ObjectUtil_eprintf("donuts. iter. key=%v val=%v\n", key, val); */
    Hash_set(clone, key, val);
    //---
    HashIter_next(iter);
  }
  HashIter_del(iter);
  return clone;
}

int Hash_cmp(Hash* self, Hash* other) {
  if(self->size != other->size) {
    return 1;
  }
  Object* key = NULL;
  Object* val1 = NULL;
  Object* val2 = NULL;
  HashIter* iter = HashIter_new(self);
  HashIter_head(iter);
  while(!HashIter_at_end(iter)) {
    key = HashIter_get_key(iter);
    val1 = HashIter_get_val(iter);
    val2 = Hash_get(other, key);
    /* ObjectUtil_eprintf("donuts. iter. key=%v val1=%v val2=%v\n", key, val1, val2); */
    if(Object_cmp(val1, val2) != 0) {
      return 1;
    }
  }
  return 0;
}

