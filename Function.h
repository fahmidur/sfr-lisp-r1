#ifndef _SFR_FUNCTION
#define _SFR_FUNCTION

/**
 * A wrapper around C Functions. 
 * (Object.List) --> (FUNC) --> (Object)
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "Symbol.h"
#include "Object.h"

// TODO: work on this.

typedef struct Function Function;
struct Function {
  int arity;       // optional.
  Object* (impl*)(Object* args);
};

Function* Function_new(Object* (impl*)(Object* args), int arity);

// Uses varargs from stdarg. 
// Every argument is expected to be Object*
// The argument length is taken from self->arity
Object* Function_call_n(Function* self, ...);

// Common for unary operator functions.
Object* Function_call_1(Function* self, Object* arg1);

// Common for binary operator functions.
Object* Function_call_2(Function* self, Object* arg1, Object* arg2);

// Call function for an Object.List args
// Downside: Requires allocation of an Object.List for the args
Object* Function_apply_n(Function* self, Object* args);

#endif

