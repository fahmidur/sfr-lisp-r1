#ifndef _SFR_OBJECT_H
#define _SFR_OBJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "Symbol.h"
#include "String.h"
#include "Number.h"
#include "Error.h"

#define OBJECT_TYPES_BUCKETS_SIZE 128
#define OBJECT_PRINT_VFLAG   0b00001
#define OBJECT_DEL_VFLAG     0b00010
#define OBJECT_RC_DONE_VFLAG 0b00100

extern Symbol* SYMBOL_SYMBOL;
extern Symbol* SYMBOL_STRING;
extern Symbol* SYMBOL_NUMBER;
extern Symbol* SYMBOL_ERROR;
extern Symbol* SYMBOL_LIST;
extern Symbol* SYMBOL_LIST_ITER;
extern Symbol* SYMBOL_HASH;
extern Symbol* SYMBOL_HASH_ITER;
extern Symbol* SYMBOL_NULL;
extern Symbol* SYMBOL_ENVIRONMENT;
extern Symbol* SYMBOL_FUNCTION;


extern char ObjectSystem_debug_eprintf;
extern char ObjectSystem_debug_001;
extern char ObjectSystem_debug_002;
extern char ObjectSystem_debug_003;
extern char ObjectSystem_debug_004;

typedef struct Object Object;
struct Object {
  Symbol* type;
  Object* next;
  Object* prev;
  void*   impl;
  int     rc;
  int     rc_gc;
  char    returning;
  char    cloneable;
  int     visited;
  char    unreachable;
};


typedef struct ObjectVisitRecord ObjectVisitRecord;
struct ObjectVisitRecord {
  Object* obj;
  ObjectVisitRecord* next;
  ObjectVisitRecord* prev;
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
  void    (*fn_del)(void* s); 
  void    (*fn_print)(void* s);
  void*   (*fn_clone)(void* s);
  char    (*fn_zero)(void* s);
  ssize_t (*fn_len)(void* s);
};

typedef struct ObjectSystem ObjectSystem;
struct ObjectSystem {
  char             init_called;
  char             done_called;
  char             delete_soft;
  char             delete_recurse;
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
Object* Object_new_list(int rc, size_t len, ...);
Object* Object_new_null();
Object* Object_new_iter(Object* iterable, int rc);
void Object_add_to_system(Object* self);
void Object_del(Object* self);
Object* Object_clone(Object* self);
Symbol* Object_type(Object* self);
int Object_cmp(Object* a, Object* b);
void Object_print(Object* self);
Object* Object_rc_incr(Object* self);
Object* Object_rc_decr(Object* self);
void    Object_rc_done(Object* self, int parent_rc, int level);
Object* Object_gc(Object* self);
Object* Object_return(Object* self);
Object* Object_accept(Object* self);
Object* Object_reject(Object* self);
char Object_is_known(Object* self);
char Object_is_null(Object* self);
char Object_is_falsey(Object* self);
char Object_is_truthy(Object* self);
char Object_is_error(Object* self);
char Object_is_returning(Object* self);
char Object_is_container(Object* self);

// Helper to make assignments with rc easier.
void Object_assign(Object** targetptr, Object* source);
void Object_move(Object** targetptr, Object** sourceptr);

// Return the hash of the object for use in Hash tables.
size_t Object_hash(Object* self);

// General ops
Object* Object_zero(Object* self);
Object* Object_to_number(Object* self);
Object* Object_to_symbol(Object* self);
Object* Object_to_string(Object* self);

// Mainly Number Ops
ssize_t Object_len(Object* self);
Object* Object_bop_add(Object* a, Object* b);
Object* Object_bop_sub(Object* a, Object* b);
Object* Object_bop_mul(Object* a, Object* b);
Object* Object_bop_div(Object* a, Object* b);

Object* Object_bop_addx(Object* a, Object* b);
Object* Object_bop_addx_char(Object* a, char ch);

// Mainly List Ops
Object* Object_uop_head(Object* self);
Object* Object_uop_tail(Object* self);
Object* Object_uop_rest(Object* self);
Object* Object_bop_push(Object* a, Object* b);
Object* Object_uop_pop(Object* a);
Object* Object_bop_unshift(Object* a, Object* b);
Object* Object_uop_shift(Object* self);
Object* Object_bop_at(Object* self, size_t idx);
Object* Object_bop_rem(Object* self, Object* val);

// Mainly Hash/Environment Ops
Object* Object_top_hset(Object* self, Object* key, Object* val);
Object* Object_bop_hget(Object* self, Object *key);
Object* Object_bop_hrem(Object* self, Object* key);
char    Object_bop_hhas(Object* self, Object* key);

// Mainly Environment Ops or anything Tree-Like (future)
Object* Object_bop_child_attach(Object* self, Object* child);
Object* Object_bop_child_detach(Object* self, Object* child);
Object* Object_bop_rfind(Object* self, Object* key);

// Mainly String ops
char    Object_bop_charat(Object* self, size_t idx);

// Mainly function ops
Object* Object_bop_call(Object* self, Object* argv);

void ObjectUtil_eprintf(char* fmt, ...);

void    Object_system_init();
size_t  Object_system_size();
size_t  Object_system_rtcount();
void    Object_system_done();
void    Object_system_print();
void    Object_system_gc();
char    Object_system_delete_recurse();
char    Object_system_delete_soft();


int ObjectSystem_count_matching_number(double x);

// Containers
#include "List.h"
#include "Hash.h"
#include "Environment.h"
#include "Function.h"

//#define OBJ_RETURN(expr) do { \
  //return Object_return(expr); \
//} while(0);
//#define OBJ_ACCEPT(x) Object_accept(x)
//#define OBJ_REJECT(x) Object_reject(x)

#define QSYMBOL_NEW1(x) Object_new(SYMBOL_SYMBOL, 1, Symbol_new(x))
#define QSYMBOL_NEW0(x) Object_new(SYMBOL_SYMBOL, 0, Symbol_new(x))
#define QSYMBOL(x) Object_new(SYMBOL_SYMBOL, 0, Symbol_new(x))

#define QSTRING_NEW1(x) Object_new(SYMBOL_STRING, 1, String_new(x))
#define QSTRING_NEW0(x) Object_new(SYMBOL_STRING, 0, String_new(x))
#define QSTRING(x) Object_new(SYMBOL_STRING, 0, String_new(x))

#define QNUMBER_NEW1(x) Object_new(SYMBOL_NUMBER, 1, Number_new_from_double(x))
#define QNUMBER_NEW0(x) Object_new(SYMBOL_NUMBER, 0, Number_new_from_double(x))
#define QNUMBER(x) Object_new(SYMBOL_NUMBER, 0, Number_new_from_double(x))

#define QERROR_NEW0(x) Object_new(SYMBOL_ERROR, 0, Error_new(x))
#define QERROR_NEW1(x) Object_new(SYMBOL_ERROR, 1, Error_new(x))
#define QERROR(x) Object_new(SYMBOL_ERROR, 0, Error_new(x))

#define QLIST_NEW0() Object_new(SYMBOL_LIST, 0, List_new())
#define QLIST_NEW1() Object_new(SYMBOL_LIST, 1, List_new())

#define QHASH_NEW0() Object_new(SYMBOL_HASH, 0, Hash_new())
#define QHASH_NEW1() Object_new(SYMBOL_HASH, 1, Hash_new())

#endif
