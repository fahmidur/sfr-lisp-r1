#ifndef _SFR_ERROR_H
#define _SFR_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Error Error;
struct Error {
  char* msg;
};

Error* Error_new(char* str);
void   Error_del(Error* self);
void   Error_print(Error* self);
Error* Error_clone(Error* self);

#endif
