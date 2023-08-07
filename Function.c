#include "Function.h"

Function* Function_new(Object* (impl*)(Object* args), int arity) {
  Function* self = calloc(1, sizeof(Function));
  self->arity = arity;
  return self;
}

Object* Function_call_n(Function* self, Object* args) {
  if(Object_type(args) != SYMBOL_LIST) {
    return Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Function expecting args of type SYMBOL_LIST")))
  }
  if(self->arity < 0 && self->arity != Object_len(args)) {
    return Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Function args length arity mismatch")));
  }
  // TODO
  return NULL;
}
