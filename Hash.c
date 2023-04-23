#include <stdio.h>
#include <stdlib.h>
#include "Hash.h"

Hash* Hash_new() {
  Hash* self = malloc(sizeof Hash);
  self->size = 0;
  self->buckets = calloc(HASH_BUCKET_SIZE, sizeof(HashNode));
  // TODO: finish this. 
  return self;
}

size_t Hash_set(Object* key, Object* val) {
}


