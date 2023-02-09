#include <stdio.h>
#include <stdlib.h>
#include "Number.h"

Number* Number_new_from_int(int x) {
  Number* self = (Number*) calloc(1, sizeof(Number));
  self->rep = x;
  return self;
}

Number* Number_new_from_float(float x) {
  Number* self = (Number*) calloc(1, sizeof(Number));
  self->rep = x;
  return self;
}

Number* Number_new_from_double(double x) {
  Number* self = (Number*) calloc(1, sizeof(Number));
  self->rep = x;
  return self;
}

Number* Number_new_from_cstr(char* cstr) {
  Number* self = (Number*) calloc(1, sizeof(Number));
  self->rep = strtol(cstr, cstr+(strlen(cstr)), 10);
  return self;
}

Number* Number_clone(Number* self) {
  Number* clone = (Number*) calloc(1, sizeof(Number));
  clone->rep = self->rep;
  return clone;
}

void Number_del(Number* self) {
  if(self == NULL) {
    return;
  }
  free(self);
}
