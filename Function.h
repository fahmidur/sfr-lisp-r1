#ifndef _SFR_FUNCTION
#define _SFR_FUNCTION

#include <stdio.h>
#include <stdlib.h>
#include "Symbol.h"
#include "Object.h"

typedef struct Function Function;
struct Function {
  int arity;
  Symbol* retwrap_type;
  Object* (impl*)(List* args);
};

Function* Function_new(int arity, Symbol* retwrap_type) {
}

#endif
