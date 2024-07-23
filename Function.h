#ifndef _SFR_FUNCTION
#define _SFR_FUNCTION

/**
 * A wrapper around C Functions. 
 * (Object<List>* args) --> (Function) --> (Object* ret)
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "Symbol.h"
#include "Object.h"

// TODO: work on this.

typedef struct Function Function;
struct Function {
  int arity; 
  Object* (*impl)(Object* argv);
};

// this is what a function call looks like
// (foo a b c)
// we don't want to create a new list for the args,
// so we send the whole Object<List> to the function implementation.
// just ignore argv[0].

//Function* Function_new(Object* (impl*)(Object* args), int arity);

// Uses varargs from stdarg. 
// Every argument is expected to be Object*
// The argument length is taken from self->arity
//Object* Function_call(Function* self, ...);

// Common for unary operator functions.
//Object* Function_call_1(Function* self, Object* arg1);

// Common for binary operator functions.
//Object* Function_call_2(Function* self, Object* arg1, Object* arg2);

// Call function for an Object.List args
// Downside: Requires allocation of an Object.List for the args
//Object* Function_apply(Function* self, Object* args);

#endif

