// A simple hash map class


#ifndef _SFR_HASH
#define _SFR_HASH

#include <stdio.h>
#include <stdlib.h>
#include "Object.h"

typedef struct Hash Hash;

struct Hash {
  size_t size;
  void** buckets;
};

Hash*   Hash_new();
size_t  Hash_set(Object* key, Object* val);
Object* Hash_get(Object* key);
void    Hash_del(Hash* self);

#endif
