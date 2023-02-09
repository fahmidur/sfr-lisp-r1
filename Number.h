#ifndef _SFR_NUMBER_H
#define _SFR_STRING_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Number Number;
struct Number {
  double rep;
};

Number* Number_new_from_int(int x);
Number* Number_new_from_float(float x);
Number* Number_new_from_double(double x);
Number* Number_new_from_cstr(char* cstr);
Number* Number_clone(Number* self);

void Number_del(Number* self);

#endif
