// - Should use the new object system.
// - We no longer care about partial tokenization,
//   just give me an Object of type List.

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "object.h"

// Tokenize a String object into flat List of objects.
Object* Lisp_tokenize(Object* string);

// Parse a flat List of objects (tokens) into a 
// nested List representing Lisp code.
Object* Lisp_parse_tokens(Object* tokenlist);

// Parse a String representing Lisp code directly to 
// nested List representing Lisp code.
// Mainly a helper function
// Lisp_parse_string(x) = Lisp_parse_tokens(Lisp_tokenize(x))
Object* Lisp_parse_string(Object* string);

// Eval the code given. 
// Code is represented as an S-Expression.
Object* Lisp_eval_code(Object* s_exp);

// Eval the given string 
// Lisp_eval_string(x) = Lisp_eval_code(Lisp_parse_string(x))
Object* Lisp_eval_string(Object* string);

