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

Function* Function_new(Object* (impl*)(Object* args)) {
  Function* self = calloc(1, sizeof(Function));
  return self;
}

Object* Function_call(Function* self, Object* args) {
  if(Object_type(args) != SYMBOL_LIST) {
    return Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Function expecting args of type SYMBOL_LIST")))
  }
  if(self->arity != -1 && self->arity != Object_len(args)) {
    return Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Function args length arity mismatch")));
  }
  // TODO
  return NULL;
}

#endif
