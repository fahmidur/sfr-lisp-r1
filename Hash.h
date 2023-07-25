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

typedef struct HashIter HashIter;
struct HashIter {
  Hash* hash;
  char at_beg;
  char at_end;
  size_t cbucket;
  HashNode* cnode;
};
// head = first element
// tail = last element
// beg  = special position before head
// end  = special position before tail

Hash*   Hash_new();
Object* Hash_get(Hash* self, Object* key);
size_t  Hash_set(Hash* self, Object* key, Object* val);
size_t  Hash_rem(Hash* self, Object* key);
void    Hash_del(Hash* self);
size_t  Hash_len(Hash* self);
size_t  Hash_size(Hash* self);
void    Hash_print(Hash* self);
Hash*   Hash_clone(Hash* self);

HashIter* HashIter_new(Hash* self);

#endif
