#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Number.h"

Number* Number_new_from_int(int x) {
  Number* self = calloc(1, sizeof(Number));
  self->rep = x;
  return self;
}

Number* Number_new_from_float(float x) {
  Number* self = calloc(1, sizeof(Number));
  self->rep = x;
  return self;
}

Number* Number_new_from_double(double x) {
  Number* self = calloc(1, sizeof(Number));
  self->rep = x;
  return self;
}

Number* Number_new_from_cstr(char* cstr) {
  Number* self = calloc(1, sizeof(Number));
  self->rep = atof(cstr);
  return self;
}

Number* Number_clone(Number* self) {
  Number* clone = calloc(1, sizeof(Number));
  clone->rep = self->rep;
  return clone;
}

void Number_del(Number* self) {
  if(self == NULL) {
    return;
  }
  free(self);
}

int Number_cmp(Number* self, Number* other) {
  if(self->rep < other->rep) {
    return -1;
  }
  else
  if(self->rep > other->rep) {
    return 1;
  }
  return 0;
}
