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

/* typedef struct FunctionEnv FunctionEnv; */
/* struct FunctionEnv { */
/*   FunctionEnv* parent; */
/*   FunctionEnv* children_head; */
/*   FunctionEnv* children_tail; */
/*   FunctionEnv* sibling_prev; */
/*   FunctionEnv* sibling_next; */
/*   Object*      objects_map;    // Object<Hash> */
/* }; */

/* FunctionEnv* FunctionEnv_new(FunctionEnv* parent); */
/* void FunctionEnv_child_add(FunctionEnv* self, FunctionEnv* child); */
/* void FunctionEnv_del(FunctionEnv* self); */

typedef struct Function Function;
struct Function {
  Object* id;
  Object* env;
  Object* params;
  Object* (*impl)(Function* fn, Object* argv);
};

// this is what a function call looks like
// (foo a b c)
// we don't want to create a new list for the args,
// so we send the whole Object<List> to the function implementation.
// just ignore argv[0].

Function* Function_new(
  Object* (*impl)(Function* fn, Object* args), 
  Object* env, 
  Object* params // List of Symbols
);

Function* Function_get_parent(Function* self);

void Function_print(Function* self);

void FunctionSystem_init();
void FunctionSystem_done();

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

