// A simple hash map class


#ifndef _SFR_HASH
#define _SFR_HASH

#define HASH_BUCKET_SIZE 2048

#include <stdio.h>
#include <stdlib.h>
#include "Object.h"

typedef struct HashNode HashNode;
struct HashNode {
  HashNode* prev;
  HashNode* next;
  Object* key;
  Object* val;
};

typedef struct Hash Hash;
struct Hash {
  size_t size;
  HashNode** buckets;
};

Hash*   Hash_new();
size_t  Hash_set(Hash* self, Object* key, Object* val);
Object* Hash_get(Hash* self, Object* key);
void    Hash_del(Hash* self);
size_t  Hash_len(Hash* self);
size_t  Hash_size(Hash* self);

#endif
