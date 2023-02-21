#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Symbol.h"
#include "Util.h"

static SymbolVector* av_buckets[SYMBOL_BUCKET_SIZE];
static char Symbol_system_inited = 0;

void Symbol_system_init() {
  printf("--- Symbol_system_init() ---\n");
  if(Symbol_system_inited) {
    return;
  }
  int i = 0;
  for(i = 0; i < SYMBOL_BUCKET_SIZE; i++) {
    av_buckets[i] = NULL;
  }
  Symbol_system_inited = 1;
}

void Symbol_system_done() {
  printf("--- Symbol_system_done() ---\n");
  int i;
  SymbolVector* iter;
  for(i = 0; i < SYMBOL_BUCKET_SIZE; i++) {
    iter = av_buckets[i];
    if(iter != NULL) {
      SymbolVector_del(iter);
    }
  }
}

void Symbol_system_print() {
  printf("--- { Symbol_system_print(). BEG { ---\n");
  int i, j;
  SymbolVector* av;
  Symbol* atom;
  unsigned int nulls_count = 0;
  unsigned int nulls_max = 1;
  char intrim = 0;
  size_t ilast = SYMBOL_BUCKET_SIZE-1;
  for(i = 0; i < SYMBOL_BUCKET_SIZE; i++) {
    av = av_buckets[i];
    if(av == NULL) {
      nulls_count++;
      if(nulls_count > nulls_max) {
        if(intrim) {
          if(i == ilast) {
            printf("%05u] \t= (nil) count=%d\n", i, nulls_count);
            intrim = 0;
          }
        } 
        else {
          printf("i[%05u..", i);
          intrim = 1;
        }
      }
      else {
        printf("i[%05u] \t\t= %p\n", i, av);
      }
    }
    else {
      if(intrim) {
        printf("%05u] \t= (nil) count=%d\n", i, nulls_count);
        intrim = 0;
      }
      printf("i[%05u] \t\t= %p\n", i, av);
      nulls_count = 0;
      printf("  lidx=%zu size=%zu\n", av->lidx, av->size);
      for(j = 0; j < av->size; j++) {
        atom = av->list[j];
        printf("  i[%05u] j[%03u] \t= %p", i, j, atom);
        if(atom == NULL) {
          printf("\n");
          break;
        }
        printf(" || ");
        Symbol_print(atom);
        printf("\n");
      }
    }
  }
  printf("--- } Symbol_system_print(). END } ---\n");
}

SymbolVector* SymbolVector_new() {
  SymbolVector* self = (SymbolVector*) calloc(1, sizeof(SymbolVector));
  self->size = SYMBOLVECTOR_ISIZE;
  self->lidx = 0;
  self->list = (Symbol**) calloc(self->size, sizeof(Symbol*));
  return self;
}

void SymbolVector_push(SymbolVector* self, Symbol* atom) {
  printf("SymbolVector_push. self=%p , atom=", self); Symbol_print(atom); printf("\n");
  self->list[self->lidx++] = atom;
  if(self->lidx == self->size) {
    size_t size_old = self->size;
    size_t size_new = 2*size_old;
    printf("SymbolVector_push. grow. growing... size_old=%zu size_new=%zu\n", size_old, size_new);
    self->list = (Symbol**) realloc(self->list, size_new*sizeof(Symbol*));
    if(self->list == NULL) {
      printf("ERROR: SymbolVector_push. grow. Failed to realloc SymbolVector\n");
      exit(1);
    }
    self->size = size_new;
    int i;
    for(i = self->lidx; i < size_new; i++) {
      self->list[i] = NULL;
    }
    printf("SymbolVector_push. grow. done. lidx=%zu size=%zu\n", self->lidx, self->size);
  }
}

void SymbolVector_del(SymbolVector* self) {
  printf("SymbolVector_del(%p)\n", self);
  int i;
  size_t lidx = self->lidx;
  Symbol* atom;
  for(i = 0; i < lidx; i++) {
    atom = self->list[i];
    if(atom != NULL) {
      Symbol_del(atom);
    }
  }
  free(self->list);
  free(self);
}

size_t Symbol_calc_hash(char* str) {
  return Util_cstr_hash_djb2(str); 
}

size_t Symbol_calc_key_from_hash(size_t hash) {
  return hash % SYMBOL_BUCKET_SIZE;
}

size_t Symbol_calc_key(char* str) {
  return Symbol_calc_key_from_hash(Symbol_calc_hash(str));
}

Symbol* Symbol_find(char* str) {
  /*printf("Symbol_find(%s). \n", str);*/
  size_t key = Symbol_calc_key(str);
  SymbolVector* av = av_buckets[key];
  if(av == NULL) {
    return NULL;
  }
  size_t lidx = av->lidx;
  if(lidx == 0) {
    return NULL;
  }
  int i;
  Symbol* atom = NULL;
  for(i = 0; i < lidx; i++) {
    atom = av->list[i];
    if(strcmp(atom->str, str) == 0) {
      return atom;
    }
  }
  return NULL;
}

void Symbol_print(Symbol* self) {
  printf(":%s", self->str);
}

Symbol* Symbol_new(char* str) {
  Symbol* self = Symbol_find(str);
  if(self != NULL) {
    return self;
  }
  int i;
  self = calloc(1, sizeof(Symbol));
  size_t str_len = strlen(str);
  self->str_len = str_len;

  self->str = calloc(str_len+1, sizeof(char));
  strcpy(self->str, str);

  size_t hash = Symbol_calc_hash(str);
  size_t key = Symbol_calc_key_from_hash(hash);
  self->hash = hash;
  self->key = key;
  SymbolVector* av = av_buckets[key];
  if(av == NULL) {
    av = SymbolVector_new();
    av_buckets[key] = av;
  }
  SymbolVector_push(av, self);
  return self;
}

// Symbols have a trivial clone.
Symbol* Symbol_clone(Symbol* self) {
  return self;
}

void Symbol_del(Symbol* self) {
  free(self->str);
  free(self);
}

void Symbol_noop(void* self) {
  return;
}

int Symbol_cmp(Symbol* self, Symbol* other) {
  if(self == other) {
    return 0;
  }
  return strcmp(self->str, other->str);
}
