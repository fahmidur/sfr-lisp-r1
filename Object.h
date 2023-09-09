#ifndef _SFR_OBJECT_H
#define _SFR_OBJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "Symbol.h"
#include "String.h"
#include "Number.h"

#define OBJECT_TYPES_BUCKETS_SIZE 128

extern Symbol* SYMBOL_SYMBOL;
extern Symbol* SYMBOL_STRING;
extern Symbol* SYMBOL_NUMBER;
extern Symbol* SYMBOL_ERROR;
extern Symbol* SYMBOL_LIST;
extern Symbol* SYMBOL_HASH;
extern Symbol* SYMBOL_NULL;

typedef struct Object Object;
struct Object {
  Symbol* type;
  Object* next;
  Object* prev;
  void*   impl;
  int     rc;
  char    returning;
};

// unused for now
//typedef struct ObjectPtrNode ObjectPtrNode;
//struct ObjectPtrNode {
  //ObjectPtrNode* prev;
  //ObjectPtrNode* next;
  //void* object;
//};

typedef struct ObjectTypeInfo ObjectTypeInfo;
struct ObjectTypeInfo {
  Symbol* type;
  size_t hash;
  size_t key;
  ObjectTypeInfo* prev;
  ObjectTypeInfo* next;
  // vtable
  void   (*fn_del)(void* s); 
  void   (*fn_print)(void* s);
  void*  (*fn_clone)(void* s);
};

typedef struct ObjectSystem ObjectSystem;
struct ObjectSystem {
  char             init_called;
  char             done_called;
  Object*          head;
  Object*          tail;
  size_t           size;
  ObjectTypeInfo*  types[OBJECT_TYPES_BUCKETS_SIZE];
  Object*          null_object;
};
//   [head]                        [tail]
//   |                             |
//   |                             |
//   +                             +
//   [Object] +---+ [Object] +---+ [Object] ---+ [NULL]

char Object_oti_set(
  Symbol* type, 
  ObjectTypeInfo otiarg
);
ObjectTypeInfo* Object_oti_get(Symbol* type);

Object* Object_new(Symbol* type, int rc, void* impl);
Object* Object_new_null();
void Object_add_to_system(Object* self);
void Object_del(Object* self);
Object* Object_clone(Object* self);
Symbol* Object_type(Object* self);
int Object_cmp(Object* a, Object* b);
void Object_print(Object* self);
Object* Object_rc_incr(Object* self);
Object* Object_rc_decr(Object* self);
Object* Object_gc(Object* self);
Object* Object_return(Object* self);
Object* Object_accept(Object* self);
Object* Object_reject(Object* self);
char Object_is_null(Object* self);
char Object_is_error(Object* self);


// Helper to make assignments with rc easier.
void Object_assign(Object** targetptr, Object* source);

// Return the hash of the object for use in Hash tables.
size_t Object_hash(Object* self);

// Mainly Number Ops
Object* Object_bop_add(Object* a, Object* b);
Object* Object_bop_sub(Object* a, Object* b);
Object* Object_bop_mul(Object* a, Object* b);
Object* Object_bop_div(Object* a, Object* b);

// Mainly List Ops
Object* Object_bop_push(Object* a, Object* b);
size_t Object_len(Object* self);
Object* Object_uop_pop(Object* a);
Object* Object_bop_unshift(Object* a, Object* b);
Object* Object_uop_shift(Object* self);
Object* Object_bop_at(Object* self, size_t idx);

void ObjectUtil_eprintf(char* fmt, ...);

void    Object_system_init();
size_t  Object_system_size();
void    Object_system_done();
void    Object_system_print();
void    Object_system_gc();

// Composits
#include "List.h"

//#define OBJ_RETURN(expr) do { \
  //return Object_return(expr); \
//} while(0);
//#define OBJ_ACCEPT(x) Object_accept(x)
//#define OBJ_REJECT(x) Object_reject(x)

#define QSYMBOL_NEW1(x) Object_new(SYMBOL_SYMBOL, 1, Symbol_new(x))
#define QSYMBOL_NEW0(x) Object_new(SYMBOL_SYMBOL, 0, Symbol_new(x))
#define QSTRING_NEW1(x) Object_new(SYMBOL_STRING, 1, String_new(x))
#define QSTRING_NEW0(x) Object_new(SYMBOL_STRING, 0, String_new(x))
#define QNUMBER_NEW1(x) Object_new(SYMBOL_NUMBER, 1, Number_new_from_double(x))
#define QNUMBER_NEW0(x) Object_new(SYMBOL_NUMBER, 0, Number_new_from_double(x))

#define QLIST_NEW1() Object_new(SYMBOL_LIST, 1, List_new())

#endif
