#ifndef _SFR_NUMBER_H
#define _SFR_NUMBER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Util.h"

typedef struct Number Number;
struct Number {
  double rep;
};

Number* Number_new(double x);
Number* Number_new_from_int(int x);
Number* Number_new_from_float(float x);
Number* Number_new_from_double(double x);
Number* Number_new_from_cstr(char* cstr);

char Number_zero(Number* self);

Number* Number_clone(Number* self);
Number* Number_add (Number* self, Number* other);  // returns clone
Number* Number_addx(Number* self, Number* other);  // returns self
Number* Number_sub(Number* self, Number* other);
Number* Number_mul(Number* self, Number* other);
Number* Number_div(Number* self, Number* other);

int  Number_cmp(Number* self, Number* other);
void Number_del(Number* self);
void Number_print(Number* self);

#endif
