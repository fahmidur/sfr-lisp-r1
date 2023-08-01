#ifndef _SFR_FUNCTION
#define _SFR_FUNCTION

#include <stdio.h>
#include <stdlib.h>
#include "Symbol.h"
#include "Object.h"

// TODO: work on this.

typedef struct Function Function;
struct Function {
  int arity;
  Symbol* ret_type;
  // args must be of type List or Hash
  Object* (impl*)(Object* args);
};

Function* Function_new(int arity, Symbol* ret_type) {
}

#endif
