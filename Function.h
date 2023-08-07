#ifndef _SFR_FUNCTION
#define _SFR_FUNCTION

/**
 * A wrapper around C Functions. 
 * (Object.List) --> (FUNC) --> (Object)
 **/

#include <stdio.h>
#include <stdlib.h>
#include "Symbol.h"
#include "Object.h"

// TODO: work on this.

typedef struct Function Function;
struct Function {
  int arity;       // optional.
  Object* (impl*)(Object* args);
};

Function* Function_new(Object* (impl*)(Object* args), int arity);

Object* Function_call_n(Function* self, Object* args);
Object* Function_call_1(Function* self, Object* arg1);
Object* Function_call_2(Function* self, Object* arg1, Object* arg2);

#endif

