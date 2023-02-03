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
  for(i = 0; i < ATOM_BUCKET_SIZE; i++) {
    AtomVector_del(av_buckets[i]);
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
  int i = 0;
  size_t out = 0;
  size_t len = strlen(str);
  for(i = 0; i < len; i++) {
    out = (out + (str[i] * (i+1))) % ATOM_BUCKET_SIZE;
  }
  return out;
}

Atom* Atom_find(char* str) {
  size_t key = Atom_key(str);
  return av_buckets[key];
}

Atom* Atom_new(char* str) {
  Atom* self = Atom_find(str);
  if(self != NULL) {
    return self;
  }
  Atom* self = malloc(sizeof(Atom));
  size_t str_len = strlen(str);
  self->str_len = str_len;
  size_t buflen = str_len+1;
  self->str = malloc(buflen);
  strcopy(self->str, str);
  return self;
}

void Atom_del(Atom* self) {
  free(self->str);
  free(self);
}

