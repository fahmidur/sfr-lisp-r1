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

extern Object* LISP_PAREN_BEG;
extern Object* LISP_PAREN_END;

void Lisp_init();
void Lisp_done();


int Lisp_runfile(char* path);
void Lisp_print_repl_help(void);

// Tokenize a String object into flat List of objects
// where each Object is a Symbol, String, or Number
Object* Lisp_tokenize(Object* string);

// Parse a flat List of objects (tokens) into a 
// nested List representing Lisp code.
// @return S-Expression
Object* Lisp_parse_tokens(Object* tokenlist);

// Parse a String representing Lisp code directly to 
// nested List representing Lisp code.
// Mainly a helper function
// Lisp_parse_string(x) = Lisp_parse_tokens(Lisp_tokenize(x))
// @return S-Expression
Object* Lisp_parse_string(Object* string);

void Lisp_printenv();

/* typedef struct LispEnv LispEnv; */
/* struct LispEnv { */
/*   LispEnv* parent; */
/*   LispEnv* children_head; */
/*   LispEnv* children_tail; */
/*   LispEnv* sibling_prev; */
/*   LispEnv* sibling_next; */
/*   Object*  objects; */
/* }; */

/* extern Object* LispEnv_root; */

/* LispEnv* LispEnv_new(LispEnv* parent); */
/* void     LispEnv_child_add(LispEnv* self, LispEnv* child); */
/* void     LispEnv_child_rem(LispEnv* self, LispEnv* child); */
/* void     LispEnv_del(LispEnv* self); */

// Eval the code in the S-Expression
// Code is represented as an S-Expression.
// @return Object
Object* Lisp_eval_sexp2(Object* sexp, Object* env, int depth);
Object* Lisp_eval_sexp(Object* sexp);

// Eval the given string 
// Lisp_eval_string(x) = Lisp_eval_sexp(Lisp_parse_string(x))
Object* Lisp_eval_string2(Object* string, Object* env);
Object* Lisp_eval_string(Object* string);

#endif
