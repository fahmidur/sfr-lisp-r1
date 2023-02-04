#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Symbol.h"

static SymbolVector* av_buckets[SYMBOL_BUCKET_SIZE];

void Symbol_system_init() {
  printf("--- Symbol_system_init() ---\n");
  int i = 0;
  for(i = 0; i < SYMBOL_BUCKET_SIZE; i++) {
    av_buckets[i] = NULL;
  }
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
  printf("--- BEG. Symbol_system_print() ---\n");
  printf("NULL=%p\n", NULL);
  int i, j;
  SymbolVector* av;
  Symbol* atom;
  for(i = 0; i < SYMBOL_BUCKET_SIZE; i++) {
    av = av_buckets[i];
    printf("i[%02u] = %p\n", i, av);
    if(av != NULL) {
      printf("  lidx=%zu size=%zu\n", av->lidx, av->size);
      for(j = 0; j < av->size; j++) {
        atom = av->list[j];
        printf("  j[%02u] = %p || ", j, atom);
        if(atom != NULL) {
          Symbol_print(atom);
        }
        printf("\n");
      }
    }
  }
  printf("--- END. Symbol_system_print() ---\n");
}

SymbolVector* SymbolVector_new() {
  SymbolVector* self = (SymbolVector*) malloc(sizeof(SymbolVector));
  self->size = SYMBOLVECTOR_ISIZE;
  self->lidx = 0;
  self->list = (Symbol**) malloc(self->size * sizeof(Symbol));
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

size_t Symbol_key(char* str) {
  int i = 0;
  size_t out = 0;
  size_t len = strlen(str);
  for(i = 0; i < len; i++) {
    out = (out + (str[i] * (i+1))) % SYMBOL_BUCKET_SIZE;
  }
  return out;
}

Symbol* Symbol_find(char* str) {
  printf("Symbol_find(%s). \n", str);
  size_t key = Symbol_key(str);
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
  printf("Symbol(%s)", self->str);
}

Symbol* Symbol_new(char* str) {
  Symbol* self = Symbol_find(str);
  if(self != NULL) {
    return self;
  }
  int i;
  self = malloc(sizeof(Symbol));
  size_t str_len = strlen(str);
  self->str_len = str_len;

  self->str = calloc(str_len+1, sizeof(char));
  strcpy(self->str, str);

  size_t key = Symbol_key(str);
  SymbolVector* av = av_buckets[key];
  if(av == NULL) {
    av = SymbolVector_new();
    av_buckets[key] = av;
  }
  SymbolVector_push(av, self);
  return self;
}

void Symbol_del(Symbol* self) {
  free(self->str);
  free(self);
}

