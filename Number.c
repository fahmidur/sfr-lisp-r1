#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Number.h"
#include "Util.h"

Number* Number_new_from_int(int x) {
  Number* self = calloc(1, sizeof(Number));
  if(self == NULL) {
    ErrorSystem_set(1, "Number_new_from_int. calloc failed");
    return NULL;
  }
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
  char* endptr;
  self->rep = strtod(cstr, &endptr);
  if(*endptr != '\0') {
    return NULL;
  }
  return self;
}

Number* Number_new(double x) {
  return Number_new_from_double(x);
}

ssize_t Number_len(Number* self) {
  return 1;
}

char Number_zero(Number* self) {
  self->rep = 0;
  return 1;
}

Number* Number_clone(Number* self) {
  Number* clone = calloc(1, sizeof(Number));
  clone->rep = self->rep;
  return clone;
}

Number* Number_add(Number* self, Number* other) {
  Number* clone = Number_clone(self);
  clone->rep = clone->rep + other->rep;
  return clone;
}

Number* Number_addx(Number* self, Number* other) {
  self->rep = self->rep + other->rep;
  return self;
}

Number* Number_sub(Number* self, Number* other) {
  Number* clone = Number_clone(self);
  clone->rep = clone->rep - other->rep;
  return clone;
}

Number* Number_subx(Number* self, Number* other) {
  self->rep = self->rep - other->rep;
  return self;
}

Number* Number_mul(Number* self, Number* other) {
  Number* clone = Number_clone(self);
  clone->rep = clone->rep * other->rep;
  return clone;
}

Number* Number_mulx(Number* self, Number* other) {
  self->rep = self->rep * other->rep;
  return self;
}

Number* Number_div(Number* self, Number* other) {
  Number* clone = Number_clone(self);
  clone->rep = clone->rep / other->rep;
  return clone;
}

Number* Number_divx(Number* self, Number* other) {
  self->rep = self->rep / other->rep;
  return self;
}

void Number_del(Number* self) {
  dbg_printf("Number_del(%p). rep=%f\n", self, self->rep);
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

void Number_print_bare(Number* self) {
  Util_vt_set(VT_COLOR_YELLOW_FG);
  if( self->rep - (long long)self->rep == 0.0) {
    printf("%lld", (long long)self->rep);
  } else {
    printf("%g", self->rep);
  }
  Util_vt_set(VT_RESET);
}

void Number_print(Number* self) {
  /*printf("Number(%f)", self->rep);*/
  printf("Number(");
  Util_vt_set(VT_COLOR_YELLOW_FG);
  printf("%g", self->rep);
  Util_vt_set(VT_RESET);
  printf(")");
}
