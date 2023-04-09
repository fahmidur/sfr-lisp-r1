// - Should use the new object system.
// - We no longer care about partial tokenization,
//   just give me an Object of type List.

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "object.h"

Object* Lisp_tokenize(Object* string);
Object* Lisp_parse(Object* tokenlist);
Object* Lisp_parsestr(Object* string);

