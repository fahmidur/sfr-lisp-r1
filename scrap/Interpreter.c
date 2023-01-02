#include "Interpreter.h"

Interpreter* Interpreter_new0() {
  Interpreter* self = malloc(sizeof(Interpreter));
  return self;
}

void Interpreter_del(Interpreter** self_ptr) {
  Interpreter* self = *self_ptr;
  if(self->runtime != NULL) {

  }
}
