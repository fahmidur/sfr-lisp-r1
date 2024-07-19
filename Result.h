/**
 * Should be free to use with or without the Object-System.
 * It should be object-system agnostic.
 **/

#ifndef _SFR_RESULT_H
#define _SFR_RESULT_H
typedef struct Result Result;

struct Result {
  char   err;
  char*  msg;
  void*  ptr;
};

#endif
