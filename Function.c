#include "Function.h"

Function* Function_new(Object* (impl*)(Object* args), int arity) {
  Function* self = calloc(1, sizeof(Function));
  self->arity = arity;
  return self;
}

Object* Function_apply(Function* self, Object* args) {
  if(Object_type(args) != SYMBOL_LIST) {
    return Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Function expecting args of type SYMBOL_LIST")))
  }
  if(self->arity < 0 && self->arity != Object_len(args)) {
    return Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Function args length arity mismatch")));
  }
  // TODO
  return NULL;
}

Object* Function_call(Function* self, ...) {
  int argc = 0;
  va_list argv;
  if(self->arity >= 0) {
    argc = self->arity;
  }
  va_start(argv, self);
  switch(argc) {
    case 1:
      self->impl(
        va_arg(argv, Object*)
      );
      break;
    case 2:
      self->impl(
        va_arg(argv, Object*),
        va_arg(argv, Object*),
      );
      break;
    default:
      printf("ERROR: invalid function arity")
  }
  // TODO: auto-generate above switch block
  // for n=100 or some maximum number of args.
  va_end(argv);
}
