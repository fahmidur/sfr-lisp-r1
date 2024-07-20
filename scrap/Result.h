/**
 * Should be free to use with or without the Object-System.
 * It should be object-system agnostic.
 **/

#ifndef _SFR_RESULT_H
#define _SFR_RESULT_H

#include <stdio.h>
#include <stddef.h>
#include "Error.h"

typedef struct Result Result;

/* enum RESULT_TYPE { */
/*   RESULT_VAL, */
/*   RESULT_ERR_STATIC, */
/*   RESULT_ERR_DYNAMIC, */
/* }; */

/* typedef union ResultValue ResultValue; */
/* union ResultValue { */
/*   Error   v_error; */
/*   size_t  v_size_t; */
/*   ssize_t v_ssize_t; */
/*   int     v_int; */
/*   float   v_float; */
/*   double  v_double; */
/*   char    v_char; */
/*   void*   v_void_ptr; */
/* }; */

/* struct Result { */
/*   char type; */
/*   ResultValue value; */
/* }; */
/* struct ResultSizeT { */
/*   char err_code; */
/*   size_t data; */
/* }; */

struct Result {
  char   err;
  char*  msg;
  void*  ptr;
};


#endif
