#ifndef _SFR_ERROR
#define _SFR_ERROR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Error Error;
struct Error {
  char* msg;
};

Error* Error_new(char* str);
void   Error_del(Error* self);

void Error_print(Error* self);

#endif
