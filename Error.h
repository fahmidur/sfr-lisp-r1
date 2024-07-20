#ifndef _SFR_ERROR_H
#define _SFR_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Error Error;
struct Error {
  int code;
  char* msg;
};

Error* Error_new(char* str);
void   Error_del(Error* self);
void   Error_print(Error* self);
Error* Error_clone(Error* self);

void ErrorSystem_init();
void ErrorSystem_done();
void ErrorSystem_reset();
void ErrorSystem_set(int code, char* msg);
void ErrorSystem_set_obj(int code, void* ptr);

int   ErrorSystem_get_code();
char* ErrorSystem_get_msg();
void* ErrorSystem_get_ptr();

#endif
