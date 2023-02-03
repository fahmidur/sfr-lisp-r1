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
}

Atom* Atom_new(char* str) {
  Atom* self = malloc(sizeof(Atom));
  return self;
}

