// A simple hash map class


#ifndef _SFR_HASH
#define _SFR_HASH

#define HASH_BUCKET_SIZE 2048

#include <stdio.h>
#include <stdlib.h>
#include "Object.h"

typedef struct HashNode HashNode;
struct HashNode {
  Object* data;
  HashNode* next;
};

typedef struct Hash Hash;
struct Hash {
  size_t size;
  HashNode* buckets;
};

Hash*   Hash_new();
size_t  Hash_set(Object* key, Object* val);
Object* Hash_get(Object* key);
void    Hash_del(Hash* self);

#endif
