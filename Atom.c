#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Atom.h"

static Atom* buckets[ATOM_BUCKET_SIZE];

// Find an Atom with String
Atom* Atom_find(char* str) {
  size_t Atom_bucket(str);
}

// Find or Create an Atom with String
Atom* Atom_new(char* str) {
  Atom* self = malloc(Atom);
  return self;
}


