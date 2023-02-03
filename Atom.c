#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Atom.h"

static AtomVector* av_buckets[ATOM_BUCKET_SIZE];

void Atom_system_init() {
  printf("--- Atom_system_init() ---\n");
  int i = 0;
  for(i = 0; i < ATOM_BUCKET_SIZE; i++) {
    av_buckets[i] = NULL;
  }
}

void Atom_system_done() {
  printf("--- Atom_system_done() ---\n");
  int i;
  AtomVector* iter;
  for(i = 0; i < ATOM_BUCKET_SIZE; i++) {
    iter = av_buckets[i];
    if(iter == NULL) {
      continue;
    }
    AtomVector_del(iter);
  }
}

AtomVector* AtomVector_new() {
  AtomVector* self = (AtomVector*) malloc(sizeof(AtomVector));
  self->size = ATOMVECTOR_ISIZE;
  self->lidx = 0;
  self->list = (Atom**) malloc(self->size * sizeof(Atom));
  return self;
}

void AtomVector_push(AtomVector* self, Atom* atom) {
  self->list[self->lidx++] = atom;
  if(self->lidx == self->size) {
    self->list = (Atom**) realloc(self->list, 2 * self->lidx);
    self->lidx = 2 * self->lidx;
  }
}

void AtomVector_del(AtomVector* self) {
  int i;
  size_t lsize = self->size;
  for(i = 0; i < lsize; i++) {
    Atom_del(self->list[i]);
  }
  free(self->list);
  free(self);
}

size_t Atom_key(char* str) {
  printf("Atom_key. str=%s\n", str);
  int i = 0;
  size_t out = 0;
  size_t len = strlen(str);
  for(i = 0; i < len; i++) {
    printf("i=%u\n", i);
    out = (out + (str[i] * (i+1))) % ATOM_BUCKET_SIZE;
  }
  printf("Atom_key. out=%u\n", (unsigned int)out);
  return out;
}

Atom* Atom_find(char* str) {
  printf("Atom_find(%s).\n", str);
  size_t key = Atom_key(str);
  printf("Atom_find(%s). key=%u\n", str, (unsigned int)key);
  AtomVector* av = av_buckets[key];
  if(av == NULL) {
    return NULL;
  }
  printf("Atom_find(%s). av=%p\n", str, av);
  size_t lidx = av->lidx;
  printf("Atom_find(%s). lidx=%u\n", str, (unsigned int)lidx);
  if(av == NULL || lidx == 0) {
    printf("Atom_find(%s). return null 1\n", str);
    return NULL;
  }
  int i;
  Atom* iatom = NULL;
  for(i = 0; i < lidx; i++) {
    iatom = av->list[i];
    if(strcmp(iatom->str, str) == 0) {
      return iatom;
    }
  }
  printf("Atom_find. return null 2\n");
  return NULL;
}

Atom* Atom_new(char* str) {
  printf("Atom_new. str=%s\n", str);
  Atom* self = Atom_find(str);
  if(self != NULL) {
    return self;
  }
  self = malloc(sizeof(Atom));
  size_t str_len = strlen(str);
  self->str_len = str_len;
  size_t buflen = str_len+1;
  self->str = malloc(buflen);
  strcpy(self->str, str);
  return self;
}

void Atom_del(Atom* self) {
  free(self->str);
  free(self);
}

