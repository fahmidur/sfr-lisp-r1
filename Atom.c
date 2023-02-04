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
    if(iter != NULL) {
      AtomVector_del(iter);
    }
  }
}

void Atom_system_print() {
  printf("--- BEG. Atom_system_print() ---\n");
  printf("NULL=%p\n", NULL);
  int i, j;
  AtomVector* av;
  Atom* atom;
  for(i = 0; i < ATOM_BUCKET_SIZE; i++) {
    av = av_buckets[i];
    printf("i[%02u] = %p\n", i, av);
    if(av != NULL) {
      printf("  lidx=%zu size=%zu\n", av->lidx, av->size);
      for(j = 0; j < av->size; j++) {
        atom = av->list[j];
        printf("  j[%02u] = %p || ", j, atom);
        if(atom != NULL) {
          Atom_print(atom);
        }
        printf("\n");
      }
    }
  }
  printf("--- END. Atom_system_print() ---\n");
}

AtomVector* AtomVector_new() {
  AtomVector* self = (AtomVector*) malloc(sizeof(AtomVector));
  self->size = ATOMVECTOR_ISIZE;
  self->lidx = 0;
  self->list = (Atom**) malloc(self->size * sizeof(Atom));
  return self;
}

void AtomVector_push(AtomVector* self, Atom* atom) {
  printf("AtomVector_push. self=%p , atom=", self); Atom_print(atom); printf("\n");
  self->list[self->lidx++] = atom;
  if(self->lidx == self->size) {
    size_t size_old = self->size;
    size_t size_new = 2*size_old;
    printf("AtomVector_push. grow. growing... size_old=%zu size_new=%zu\n", size_old, size_new);
    self->list = (Atom**) realloc(self->list, size_new*sizeof(Atom*));
    if(self->list == NULL) {
      printf("ERROR: AtomVector_push. grow. Failed to realloc AtomVector\n");
      exit(1);
    }
    self->size = size_new;
    int i;
    for(i = self->lidx; i < size_new; i++) {
      self->list[i] = NULL;
    }
    printf("AtomVector_push. grow. done. lidx=%zu size=%zu\n", self->lidx, self->size);
  }
}

void AtomVector_del(AtomVector* self) {
  printf("AtomVector_del(%p)\n", self);
  int i;
  size_t lidx = self->lidx;
  Atom* atom;
  for(i = 0; i < lidx; i++) {
    atom = self->list[i];
    if(atom != NULL) {
      Atom_del(atom);
    }
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
  printf("Atom_find(%s). \n", str);
  size_t key = Atom_key(str);
  AtomVector* av = av_buckets[key];
  if(av == NULL) {
    return NULL;
  }
  size_t lidx = av->lidx;
  if(lidx == 0) {
    return NULL;
  }
  int i;
  Atom* atom = NULL;
  for(i = 0; i < lidx; i++) {
    atom = av->list[i];
    if(strcmp(atom->str, str) == 0) {
      return atom;
    }
  }
  return NULL;
}

void Atom_print(Atom* self) {
  printf("Atom(%s)", self->str);
}

Atom* Atom_new(char* str) {
  Atom* self = Atom_find(str);
  if(self != NULL) {
    return self;
  }
  int i;
  self = malloc(sizeof(Atom));
  size_t str_len = strlen(str);
  self->str_len = str_len;

  self->str = calloc(str_len+1, sizeof(char));
  strcpy(self->str, str);

  size_t key = Atom_key(str);
  AtomVector* av = av_buckets[key];
  if(av == NULL) {
    av = AtomVector_new();
    av_buckets[key] = av;
  }
  AtomVector_push(av, self);
  return self;
}

void Atom_del(Atom* self) {
  free(self->str);
  free(self);
}

