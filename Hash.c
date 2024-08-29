#include <stdio.h>
#include <stdlib.h>
#include "Object.h"
#include "Hash.h"
#include "Error.h"

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
  ObjectUtil_eprintf("HashNode(%v => %v)", self->key, self->val);
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
    self->key = Object_rc_decr(self->key);
  }
  if(self->val != NULL && Object_system_delete_recurse()) {
    self->key = Object_rc_decr(self->val);
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
  Hash* self = calloc(1, sizeof(Hash));
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
      HashNode_del_fwd(node);
    }
  }
  free(self->buckets);
  free(self);
}

ssize_t Hash_size(Hash* self) {
  return self->size;
}

ssize_t Hash_len(Hash* self) {
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

void Hash_rem(Hash* self, Object* key) {
  Object_rc_incr(key);
  size_t index = Object_hash(key) % HASH_BUCKET_SIZE;
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

  size_t index = Object_hash(key_clone) % HASH_BUCKET_SIZE;

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

void Hash_print(Hash* self) {
  HashNode* iter = NULL;
  size_t i = 0;
  printf("Hash(");
  if(self->size > 0) {
    printf("\n");
  }
  size_t printed = 0;
  for(i = 0; i < HASH_BUCKET_SIZE; i++) {
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
  for(i = 0; i < HASH_BUCKET_SIZE; i++) {
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
  while(self->cbucket < HASH_BUCKET_SIZE && hash->buckets[self->cbucket] == NULL) {
    self->cbucket++;
  }
  self->at_pos = HASH_ITER_POS_INN;
  // cbucket is the first non-empty bucket index.
  // OR 
  // you have reached the end.
  if(self->cbucket == HASH_BUCKET_SIZE) {
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
  while(self->cbucket < HASH_BUCKET_SIZE && hash->buckets[self->cbucket] == NULL) {
    self->cbucket++;
  }
  if(self->cbucket == HASH_BUCKET_SIZE) {
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
    ObjectUtil_eprintf("iter. key=%v val=%v\n", key, val);
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
    ObjectUtil_eprintf("iter. key=%v val1=%v val2=%v\n", key, val1, val2);
    if(Object_cmp(val1, val2) != 0) {
      return 1;
    }
  }
  return 0;
}

