// A simple hash map class

#ifndef _SFR_HASH_H
#define _SFR_HASH_H

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

enum HashIterPos {
  HASH_ITER_POS_NIL, // In limbo.
  HASH_ITER_POS_BEG, // Before the 'head' element.
  HASH_ITER_POS_INN, // Within collection of elements.
  HASH_ITER_POS_END, // After the 'tail' element.
};

typedef struct HashIter HashIter;
struct HashIter {
  Hash* hash;
  enum HashIterPos at_pos;
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
void    Hash_zero(Hash* self);

HashIter* HashIter_new(Hash* self);
HashIter* HashIter_head(HashIter* self);
HashIter* HashIter_next(HashIter* self);
Object*   HashIter_get_key(HashIter* self);
Object*   HashIter_get_val(HashIter* self);
char      HashIter_at_beg(HashIter* self);
char      HashIter_at_end(HashIter* self);

#endif
