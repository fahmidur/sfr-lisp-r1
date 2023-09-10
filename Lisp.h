/**
 * The idea here is that Lisp.h will
 * define everything needed to go from our
 * crude reference counted runtime to Lisp.
 * We will define a simple tokenizer, parser,
 * and evaluater based on LispEnv.
 * @author Syed Reza
 */
#ifndef _SFR_LISP_H
#define _SFR_LISP_H

// - Should use the new object system.
// - We no longer care about partial tokenization.

#include <stdio.h>
#include <stdlib.h>
#include "Util.h"
#include "Symbol.h"
#include "Object.h"
#include "Runtime.h"

// Tokenize a String object into flat List of objects
// where each Object is a Symbol, String, or Number
Object* Lisp_tokenize(Object* string);

// Parse a flat List of objects (tokens) into a 
// nested List representing Lisp code.
Object* Lisp_parse_tokens(Object* tokenlist);

// Parse a String representing Lisp code directly to 
// nested List representing Lisp code.
// Mainly a helper function
// Lisp_parse_string(x) = Lisp_parse_tokens(Lisp_tokenize(x))
Object* Lisp_parse_string(Object* string);

// TODO: define LispEnv
//typedef struct LispEnv LispEnv;
//struct LispEnv {
  //LispEnv* parent;
  //Hash* map;
//};

//LispEnv* LispEnv_new(LispEnv* parent);
//void LispEnv_del(LispEnv* self);

// Eval the code in the sexp.
// Code is represented as an S-Expression.
//Object* Lisp_eval_sexp(Object* sexp, LispEnv* env);

// Eval the given string 
// Lisp_eval_string(x) = Lisp_eval_code(Lisp_parse_string(x))
//Object* Lisp_eval_string(Object* string, LispEnv* env);

#endif
