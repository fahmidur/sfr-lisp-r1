#ifndef _SFR_FUNCTION_H
#define _SFR_FUNCTION_H

/**
 * A wrapper around C Functions. 
 * (Object<List>* args) --> (Function) --> (Object* ret)
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "Symbol.h"
#include "Environment.h"
#include "Object.h"

/* typedef struct FunctionEnv FunctionEnv; */
/* struct FunctionEnv { */
/*   FunctionEnv* parent; */
/*   FunctionEnv* children_head; */
/*   FunctionEnv* children_tail; */
/*   FunctionEnv* sibling_prev; */
/*   FunctionEnv* sibling_next; */
/*   Object*      objects;    // Object<Hash> */
/* }; */

/* FunctionEnv* FunctionEnv_new(FunctionEnv* parent); */
/* void         FunctionEnv_child_add(FunctionEnv* self, FunctionEnv* child); */
/* void         FunctionEnv_del(FunctionEnv* self); */

// let this constant denote that a function that takes variable args.
#define FUNCTION_ARITY_ANY -1

typedef struct Function Function;
struct Function {
  Object* name;
  Object* env;
  int     arity; // must match length of params if >= 0
  Object* params;
  Object* body;
  Object* (*impl)(Function* fn, Object* env, Object* argv);
};

// this is what a function call looks like
// (foo a b c)
// we don't want to create a new list for the args,
// so we send the whole Object<List> to the function implementation.
// just ignore argv[0].

Function* Function_new(
  Object* name,
  Object* env,
  Object* (*impl)(Function* fn, Object* env, Object* args), 
  int     arity,  // Must match length of params if >= 0
  Object* params, // Object<List> of Symbols
  Object* body    // Object<List> of whatever
);

/* Function* Function_get_parent(Function* self); */

void      Function_print(Function* self);
Object*   Function_call(Function* self, Object* argv);
void      Function_del(Function* self);
// initialize the function in the context of the Object System
void      Function_obj_init(Object* self);

// I don't think this will be needed.
/* Function* Function_clone(Function* self); */

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

