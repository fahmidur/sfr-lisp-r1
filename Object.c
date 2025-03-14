#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include "Util.h"
#include "Symbol.h"
#include "String.h"
#include "Number.h"
#include "List.h"
#include "Hash.h"
#include "Environment.h"
#include "Function.h"
#include "Error.h"
#include "Object.h"

ObjectSystem* object_system;

char ObjectSystem_debug_eprintf;
char ObjectSystem_debug_001;
char ObjectSystem_debug_002;
char ObjectSystem_debug_003;
char ObjectSystem_debug_004;
FILE *osg_ofile;

Symbol* SYMBOL_NEW;
Symbol* SYMBOL_DEL;

Symbol* SYMBOL_SYMBOL;
Symbol* SYMBOL_STRING;
Symbol* SYMBOL_NUMBER;
Symbol* SYMBOL_ERROR;
Symbol* SYMBOL_LIST;
Symbol* SYMBOL_LIST_ITER;
Symbol* SYMBOL_HASH;
Symbol* SYMBOL_HASH_ITER;
Symbol* SYMBOL_NULL;
Symbol* SYMBOL_ENVIRONMENT;
Symbol* SYMBOL_FUNCTION;



char Object_oti_set(Symbol* type, ObjectTypeInfo otiarg) {
  assert(type != NULL);
  dbg_printf("Object_oti_set("); 
#ifdef DEBUG
    Symbol_print(type); 
#endif
  dbg_printf(")\n");
  size_t hash = type->hash;
  size_t key = hash % OBJECT_TYPES_BUCKETS_SIZE;
  ObjectTypeInfo* oti = calloc(1, sizeof(ObjectTypeInfo));
  oti->hash = hash;
  oti->key = key;
  oti->next = NULL;
  oti->prev = NULL;
  oti->type = type;
  oti->fn_del   = otiarg.fn_del;
  oti->fn_print = otiarg.fn_print;
  oti->fn_clone = otiarg.fn_clone;
  oti->fn_zero  = otiarg.fn_zero;
  oti->fn_len   = otiarg.fn_len;
  ObjectTypeInfo* oti_old = object_system->types[oti->key];
  /*dbg_printf("oti_old=%p\n", oti_old);*/
  oti->next = oti_old;
  if(oti_old != NULL) {
    oti_old->prev = oti;
  }
  object_system->types[key] = oti;
  return 0;
}

ObjectTypeInfo* Object_oti_get(Symbol* type) {
  assert(type != NULL);
  size_t hash = type->hash;
  size_t key = hash % OBJECT_TYPES_BUCKETS_SIZE;
  ObjectTypeInfo* oti = object_system->types[key];
  if(oti == NULL) {
    return NULL;
  }
  while(oti->type != type) {
    oti = oti->next;
  }
  return oti;
}

void Object_system_init() {
  dbg_printf("--- BEG. Object_system_init() ---\n");
  ObjectSystem_debug_eprintf = 0;
  ObjectSystem_debug_001 = 0;
  ObjectSystem_debug_002 = 0;
  ObjectSystem_debug_003 = 0;
  ObjectSystem_debug_004 = 0;
  size_t i;
  object_system = calloc(1, sizeof(ObjectSystem));
  object_system->init_called = 0;
  object_system->done_called = 0;
  object_system->delete_soft = 0;
  object_system->delete_recurse = 1;
  object_system->head = NULL;
  object_system->tail = NULL;
  object_system->size = 0;
  for(i = 0; i < OBJECT_TYPES_BUCKETS_SIZE; i++) {
    object_system->types[i] = NULL;
  }

  SYMBOL_NEW = Symbol_new("new");
  SYMBOL_DEL = Symbol_new("del");

  // The NULL Type
  SYMBOL_NULL = Symbol_new("NULL");
  // Atomic Types
  SYMBOL_SYMBOL = Symbol_new("Symbol");
  SYMBOL_STRING = Symbol_new("String");
  SYMBOL_NUMBER = Symbol_new("Number");
  SYMBOL_ERROR  = Symbol_new("Error");
  // Container Types
  SYMBOL_LIST = Symbol_new("List");
  SYMBOL_HASH = Symbol_new("Hash");
  SYMBOL_ENVIRONMENT = Symbol_new("Environment");
  // Container + Callable Object
  SYMBOL_FUNCTION = Symbol_new("Function");

  dbg_printf("\n\n");

  ObjectTypeInfo otiarg_symbol = { 
    .fn_del   = Symbol_noop, 
    .fn_print = (void     (*)(void*))Symbol_print,
    .fn_clone = (void*    (*)(void*))Symbol_clone,
    .fn_zero  = (char     (*)(void*))Symbol_zero,
    .fn_len   = (ssize_t  (*)(void*))Symbol_len,
  };
  Object_oti_set(SYMBOL_SYMBOL, otiarg_symbol);

  ObjectTypeInfo otiarg_string = {
    .fn_del   = (void    (*)(void*))String_del,
    .fn_print = (void    (*)(void*))String_print,
    .fn_clone = (void*   (*)(void*))String_clone,
    .fn_zero  = (char    (*)(void*))String_zero,
    .fn_len   = (ssize_t (*)(void*))String_len,
  };
  Object_oti_set(SYMBOL_STRING, otiarg_string);

  ObjectTypeInfo otiarg_number = {
    .fn_del   = (void    (*)(void*))Number_del,
    .fn_print = (void    (*)(void*))Number_print,
    .fn_clone = (void*   (*)(void*))Number_clone,
    .fn_zero  = (char    (*)(void*))Number_zero,
    .fn_len   = (ssize_t (*)(void*))Number_len,
  };
  Object_oti_set(SYMBOL_NUMBER, otiarg_number);

  ObjectTypeInfo otiarg_list = {
    .fn_del   = (void    (*)(void*))List_del,
    .fn_print = (void    (*)(void*))List_print,
    .fn_clone = (void*   (*)(void*))List_clone,
    .fn_zero  = (char    (*)(void*))List_zero,
    .fn_len   = (ssize_t (*)(void*))List_len,
  };
  Object_oti_set(SYMBOL_LIST, otiarg_list);

  // TODO:
  // - complete ListIter such that
  //   core methods are implemented.
  // - finish and test oti registration.
  ObjectTypeInfo otiarg_list_iter = {
  };

   ObjectTypeInfo otiarg_hash = {
    .fn_del   = (void    (*)(void*))Hash_del,
    .fn_print = (void    (*)(void*))Hash_print,
    .fn_clone = (void*   (*)(void*))Hash_clone,
    .fn_zero  = (char    (*)(void*))Hash_zero,
    .fn_len   = (ssize_t (*)(void*))Hash_len,
   };
   Object_oti_set(SYMBOL_HASH, otiarg_hash);

  ObjectTypeInfo otiarg_error = {
    .fn_del   = (void    (*)(void*))Error_del,
    .fn_print = (void    (*)(void*))Error_print,
    .fn_clone = (void*   (*)(void*))Error_clone,
    .fn_len   = (ssize_t (*)(void*))Error_len,
  };
  Object_oti_set(SYMBOL_ERROR, otiarg_error);

  ObjectTypeInfo otiarg_environment = {
    .fn_del   = (void (*)(void*))Environment_del,
    .fn_print = (void (*)(void*))Environment_print,
    .fn_clone = NULL,
    .fn_zero  = NULL,
    .fn_len   = (ssize_t (*)(void*))Environment_len,
  };
  Object_oti_set(SYMBOL_ENVIRONMENT, otiarg_environment);

  ObjectTypeInfo otiarg_function = {
    .fn_del = (void (*)(void*))Function_del,
    .fn_print = (void (*)(void*))Function_print,
    .fn_clone = NULL,
    .fn_zero  = NULL,
    .fn_len   = (ssize_t (*)(void*))Function_len,
  };
  Object_oti_set(SYMBOL_FUNCTION, otiarg_function);

  object_system->null_object = Object_new(SYMBOL_SYMBOL, 1, Symbol_new("NULL"));

  object_system->init_called = 1;
  object_system->done_called = 0;

  dbg_printf("--- END. Object_system_init() ---\n");
}

char Object_system_delete_recurse() {
  return object_system->delete_recurse;
}

char Object_system_delete_soft() {
  return object_system->delete_soft;
}

/**
 * Create a new Object. 
 * Everything in this Runtime is an Object. 
 * TODO: ensure that there are no other objects with
 * the same impl pointer. Currently, it assumes
 * all void* impl pointers are unique. 
 */
Object* Object_new(Symbol* type, int rc, void* impl) {
  assert(type != NULL);
  assert(rc == 0 || rc == 1);
  assert(impl != NULL);

  if(type == SYMBOL_SYMBOL) {
    // Symbol objects are immutable and so we can use the same Object 
    // for the same Symbol impl.
    Object* iter = object_system->head;
    while(iter != NULL) {
      if(Object_type(iter) == SYMBOL_SYMBOL && iter->impl == impl) {
        return iter;
      }
      iter = iter->next;
    }
    Object_assign(&iter, NULL);
  }

  Object* self = calloc(1, sizeof(Object));

  self->type = type;
  self->impl = impl;
  self->rc = rc;
  self->rc_gc = self->rc;
  /*self->gc_skipped = 0;*/
  self->returning = 0;
  self->cloneable = 1;
  self->visited = 0;
  self->unreachable = 0;

  Object_add_to_system(self);
  
  if(rc == 0) {
    return Object_return(self);
  } else {
    return self;
  }
}

/**
 * Return a new iterator Object based on the Iterable
 * object received: (Object<List> or Object<Hash>)
 */
Object* Object_new_iter(Object* iterable, int rc) {
  assert(iterable != NULL);
  assert(rc == 0 || rc == 1);

  Object* self = calloc(1, sizeof(Object));
  if(Object_type(iterable) == SYMBOL_LIST) {
    self->type = SYMBOL_LIST_ITER;
    self->impl = ListIter_new(iterable->impl);
    Object_rc_incr(iterable);
  }
  else
  if(Object_type(iterable) == SYMBOL_HASH) {
    self->type = SYMBOL_HASH_ITER;;
    self->impl = HashIter_new(iterable->impl);
    Object_rc_incr(iterable);
  }
  else {
    dbg_printf("ERROR: Object_new_iter. Received non-iterable object\n");
    return NULL;
  }

  if(rc == 0) {
    return Object_return(self);
  } else {
    return self;
  }
}

Object* Object_new_list(int rc, size_t len, ...) {
  assert(rc >= 0);
  assert(len >= 0);
  va_list argv;
  va_start(argv, len);
  Object* list = Object_new(SYMBOL_LIST, 1, List_new());
  /*ObjectUtil_eprintf("debug. list. rt=%d\n", list->returning);*/
  int i;
  Object* tmp;
  for(i = 0; i < len; i++) {
    /*dbg_printf("debug. --- i=%d ---\n", i);*/
    tmp = va_arg(argv, Object*);
    if(tmp == NULL) {
      tmp = Object_new_null();
    }
    assert(tmp != NULL);
    Object_accept(tmp);
    assert(!Object_is_returning(tmp));
    /* Object_rc_incr(tmp); // our reference to it in the args to this function */
    /*ObjectUtil_eprintf("debug. pushing into list. bef. tmp= %v | .rc=%d | .rt=%d\n", tmp, tmp->rc, tmp->returning);*/
    Object_reject(Object_bop_push(list, tmp)); // push into this object, ignore error
    assert(!Object_is_returning(tmp));
    Object_rc_decr(tmp); // we are done with tmp
    /*ObjectUtil_eprintf("debug. pushing into list. aft. tmp= %v | .rc=%d | .rt=%d\n", tmp, tmp->rc, tmp->returning);*/
    /*ObjectUtil_eprintf("debug. list. rt=%d\n", list->returning);*/
    /*dbg_printf("debug. ---- --- \n");*/
  }
  va_end(argv);
  list->rc = rc;
  if(rc == 0) {
    Object_return(list);
  }
  /*ObjectUtil_eprintf("debug. returning list %x = %v | rc=%d | rt=%d\n", list, list, list->rc, list->returning);*/
  return list;
}

/**
 * Return the special null_object which is an object
 * of type Symbol = NULL
 */
Object* Object_new_null() {
  return object_system->null_object;
}

/**
 * A helper function to make rc_decr and assign easier.
 */
void Object_assign(Object** targetptr, Object* source) {
  assert(targetptr != NULL);
  if(*targetptr != NULL) {
    if(Object_is_null(*targetptr)) {
      // do nothing
    }
    else {
      Object_rc_decr(*targetptr);
    }
  }
  if(source == NULL) {
    // helps ensure that the object system uses null_object
    // instead of raw NULL.
    *targetptr = Object_new_null();
  }
  else {
    *targetptr = Object_accept(source);
  }
}

void Object_move(Object** targetptr, Object** sourceptr) {
  Object_assign(targetptr, *sourceptr);
  Object_assign(sourceptr, NULL);
}

Object* Object_to_number(Object* self) {
  assert(self != NULL);
  Object_rc_incr(self);
  Object* ret = NULL;
  if(Object_type(self) == SYMBOL_STRING) {
    // String to Number
    ret = Object_return(Object_new(SYMBOL_NUMBER, 0, Number_new(String_to_double(self->impl))));
  }
  else
  if(Object_type(self) == SYMBOL_NUMBER) {
    // Number to Number
    ret = Object_return(Object_clone(self));
  }
  //TODO: what about Symbol to Number?
  Object_rc_decr(self);
  return ret;
}

Object* Object_to_string(Object* self) {
  assert(self != NULL);
  Object_rc_incr(self);
  Object* ret = NULL;
  if(Object_type(self) == SYMBOL_SYMBOL) {
    // Symbol to String
    ret = Object_return(
      Object_new(SYMBOL_STRING, 0, String_new(Symbol_cstr(self->impl)))
    );
  }
  else
  if(Object_type(self) == SYMBOL_STRING) {
    // String to String
    ret = Object_clone(self);
  }
  // TODO: what about Number to String?
  Object_rc_decr(self);
  return ret;
}

Object* Object_to_symbol(Object* self) {
  assert(self != NULL);
  Object_rc_incr(self);

  Object* ret = NULL;
  if(Object_type(self) == SYMBOL_STRING) {
    ret = Object_return(
      Object_new(SYMBOL_SYMBOL, 0, Symbol_new(String_cstr(self->impl)))
    );
  }
  else
  if(Object_type(self) == SYMBOL_SYMBOL) {
    ret = self;
  }
  Object_rc_decr(self);
  return ret;
}

/**
 * Helper function to check if the object is 
 * the system null_object.
 * Raw NULL pointer should be avoided.
 * This allows us to keep accept/reject/rc semantics.
 */
char Object_is_null(Object* x) {
  return (
    x == object_system->null_object ||
    (
     Object_type(x) == SYMBOL_SYMBOL &&
     x->impl == SYMBOL_NULL
    )
  );
}

char Object_is_truthy(Object* x) {
  return !Object_is_null(x);
}

/**
 * Helper function to check if Object is an
 * Error type object.
 */
char Object_is_error(Object* x) {
  assert(x != NULL);
  return (
    Object_type(x) == SYMBOL_ERROR
  );
}

Object* Object_return(Object* self) {
  assert(self != NULL);
  if(Object_is_null(self)) {
    // do nothing to the null object
    return self;
  }
  self->returning = 1;
  return self;
}

char Object_is_returning(Object* self) {
  assert(self != NULL);
  return self->returning;
}

char Object_is_known(Object* self) {
  if(self == NULL) {
    return 0;
  }
  Object* iter = object_system->head;
  while(iter != NULL) {
    if(self == iter) {
      return 1;
    }
    iter = iter->next;
  }
  return 0;
}

Object* Object_accept(Object* self) {
  assert(self != NULL);
  if(Object_is_null(self)) {
    return self;
  }
  self->returning = 0;
  Object_rc_incr(self);
  return self;
}

Object* Object_reject(Object* self) {
  assert(self != NULL);
  /* if(!Object_is_known(self)) { */
  /*   dbg_printf("ERROR. FATAL. self=%p is not known\n", self); */
  /*   exit(1); */
  /*   /1* return NULL; *1/ */
  /* } */
  self->returning = 0;
  if(Object_is_null(self)) {
    // do nothing to the null object
    return NULL;
  }
  Object_gc(self);
  return NULL;
}

size_t Object_system_size() {
  if(object_system == NULL) {
    return 0;
  }
  return object_system->size;
}

size_t Object_system_rtcount() {
  size_t count = 0;
  Object* iter = object_system->head;
  while(iter != NULL) {
    if(iter->returning) {
#ifdef DEBUG
      ObjectUtil_eprintf("os_rtcount. nonzero obj(%p). %v\n", iter, iter);
#endif
      count++;
    }
    iter = iter->next;
  }
  return count;
}

void Object_action_rc_gc_decr(Object* referer, Object* referend) {
  /* if(Object_type(referend) == SYMBOL_SYMBOL) { */
  /*   printf("donuts. critical error!\n"); */
  /*   exit(1); */
  /* } */
  if(Object_type(referend) == SYMBOL_NUMBER && ((Number*)referend->impl)->rep == 3.456) {
    printf("donuts. referer=%p ---> referend=%p\n", referer, referend);
    printf("donuts. break here\n");
    printf("donuts. referer type="); Symbol_print(referer->type); printf("\n");
  }
  referend->rc_gc--;
}

void Object_action_set_unreachable(Object* referer, Object* referend) {
  if(referend->rc_gc <= 0) {
    referend->unreachable = 1;
  }
}

void Object_action_unset_unreachable(Object* referer, Object* referend) {
  if(referer->unreachable == 0 && referend->unreachable == 1) {
    referend->unreachable = 0;
  }
}

/* void Object_system_iterate(void fn(Object* tmp)) { */
/*   Object* iter; */
/*   iter = object_system->head; */
/*   while(iter != NULL) { */
/*     fn(iter); */
/*     iter = iter->next; */
/*   } */
/* } */

/**
 * Intentionally non-recursive.
 **/
void Object_system_walkrefs(void fn(Object* referer, Object* referend)) {
  Object* obj_curr;
  Object* obj_temp;

  Symbol* obj_curr_type;
  obj_curr = object_system->head;
  while(obj_curr != NULL) {
    obj_curr_type = Object_type(obj_curr);
    if(obj_curr_type == SYMBOL_LIST) {
      ListIter* list_iter = ListIter_new(obj_curr->impl);
      ListIter_next(list_iter);
      while(!ListIter_at_end(list_iter)) {
        obj_temp = ListIter_get_val(list_iter);
        //obj_temp_1 is a referend, that we now decrease 1 refcount from
        fn(obj_curr, obj_temp);
        ListIter_next(list_iter);
      }
      ListIter_del(list_iter);
      list_iter = NULL;
    }
    else
    if(obj_curr_type == SYMBOL_HASH) {
      HashIter* hash_iter = HashIter_new(obj_curr->impl);
      HashIter_next(hash_iter);
      while(!HashIter_at_end(hash_iter)) {
        obj_temp = HashIter_get_key(hash_iter);
        fn(obj_curr, obj_temp);
        obj_temp = HashIter_get_val(hash_iter);
        fn(obj_curr, obj_temp);
        HashIter_next(hash_iter);
      }
      HashIter_del(hash_iter);
      hash_iter = NULL;
    }
    else
    if(obj_curr_type == SYMBOL_ENVIRONMENT) {
      Environment* env = (Environment*)(obj_curr->impl);
      if(env->objects != NULL) {
        obj_temp = env->objects; // Object<Hash>
        fn(obj_curr, obj_temp);
      }
      if(env->children != NULL) {
        obj_temp = env->objects; // Object<List>
        fn(obj_curr, obj_temp);
      }
    }
    obj_curr = obj_curr->next;
  }
}

void Object_system_gc() {
  Object* obj_curr;
  Object* obj_temp_1;
  Object* obj_temp_2;

  Object* iter = object_system->head;
  Object* next = NULL;

  // copy the rc into rc_gc;
  iter = object_system->head;
  while(iter != NULL) {
    iter->rc_gc = iter->rc;
    iter->unreachable = 0;
    iter = iter->next;
  }

  // decrease the rc_gc of referand by 1 for every referring object.
  // not every object will be a referend, in that they are not referenced
  // by another container object, this is expected.
  Object_system_walkrefs(&Object_action_rc_gc_decr);

  // mark any object with rc_gc == 0 as tentatively unreachable
  iter = object_system->head;
  while(iter != NULL) {
    if(iter->rc_gc <= 0) {
      iter->unreachable = 1;
    }
    iter = iter->next;
  }

  int i = 0;
  for(i = 0; i < object_system->size; i++) {
    //unset any objects that can be reached from a reachable
    Object_system_walkrefs(&Object_action_unset_unreachable);
  }

  //now anything that remains marked unreachable is truly unreachable
  //we must now safely delete these unreachable objects

  object_system->delete_recurse = 0;
  iter = object_system->head;
  size_t iter_count = 0;
  size_t os_size = object_system->size;
  while(iter != NULL && iter_count < os_size) {
    next = iter->next;
    if(iter->unreachable) {
      /* ObjectUtil_eprintf("unreachable obj(p=%p rc=%d/%d rt=%d t=%p)", iter, iter->rc, iter->rc_gc, iter->returning, iter->type); */
      /* printf(" | t="); */
      /* Symbol_print(Object_type(iter)); */
      /* printf("\n"); */
      iter->rc = 0;
      iter = Object_gc(iter);
    }
    iter = next;
    iter_count++;
  }
  object_system->delete_recurse = 1;
}

/**
 * TODO: make this idempotent, such that adding the same Object* 
 * n times only creates 1 reference in the global object system list.
 * May need to change object storage from List to Hash
 */
void Object_add_to_system(Object* self) {
  assert(self != NULL);
  if(object_system->size == 0) {
    object_system->head = self;
    object_system->tail = self;
    self->prev = NULL;
    self->next = NULL;
  } 
  else {
    Object* old_tail = object_system->tail;
    old_tail->next = self; self->prev = old_tail;
    object_system->tail = self;
  }
  object_system->size++;
}

char Object_is_container(Object* self) {
  Symbol* self_type = Object_type(self);
  return (
    self_type == SYMBOL_LIST || 
    self_type == SYMBOL_HASH ||
    self_type == SYMBOL_ENVIRONMENT
  );
}

void Object_del(Object* self) {
  assert(self != NULL);
  if(self->visited & OBJECT_DEL_VFLAG) {
    goto _return;
  }
  self->visited = self->visited | OBJECT_DEL_VFLAG;
  dbg_printf("{ Object_del(%p). rc=%d. rt=%d. type=", self, self->rc, self->returning); 
#ifdef DEBUG
  Symbol_print(Object_type(self)); 
#endif
  if(!Object_is_container(self)) {
    dbg_printf(" || ");
#ifdef DEBUG
    Object_print(self);
#endif
  }
  dbg_printf(" { \n");
  if(object_system->size == 0) {
    goto _return;
  }
  Object* new_head;
  Object* new_tail;
  Object* prev;
  Object* next;
  if(self == object_system->head && self == object_system->tail) {
    // the object is both head and tail, it is the only object
    object_system->head = NULL;
    object_system->tail = NULL;
  }
  else
  if(self == object_system->head) {
    // the object lives at head
    new_head = self->next;
    assert(new_head != NULL);
    new_head->prev = NULL;
    self->next = NULL;
    object_system->head = new_head;
  }
  else
  if(self == object_system->tail) {
    // the object lives at tail
    new_tail = self->prev;
    assert(new_tail != NULL);
    new_tail->next = NULL;
    self->prev = NULL;
    object_system->tail = new_tail;
  }
  else {
    prev = self->prev;
    next = self->next;
    assert(prev != NULL);
    assert(next != NULL);
    self->next = NULL;
    self->prev = NULL;
    prev->next = next; next->prev = prev;
  }
  object_system->size--;

  ObjectTypeInfo* oti = Object_oti_get(self->type);
  if(!oti) {
    dbg_printf("FATAL: unknown ObjectTypeInfo oti for type ");
    Symbol_print(self->type);
    exit(1);
  }
  if(oti->fn_del == NULL) {
    dbg_printf("FATAL: ObjectTypeInfo oti for type ");
    Symbol_print(self->type);
    dbg_printf(" is missing fn_del\n");
    exit(1);
  }

  oti->fn_del(self->impl);
  /*dbg_printf("Object_del(%p). calling free on self.\n", self);*/
  free(self);

_return:
  dbg_printf("} Object_del(%p) }\n", self);
  // unsetting of the self->visited flag is not needed because
  // the object is deleted.
  return;
}

/**
 * Recursively set the refcount of this object to be 
 * greater than its parent object.
 * This ensures that all scalar objects have a higher
 * refcount than their composit parent containers objects.
 * Which then, ensures that parent container objects are 
 * GC'd before the objects they contain.
 **/
void Object_rc_done(Object* self, int parent_rc, int level) {
  assert(self != NULL);
  
  if((self->visited & OBJECT_RC_DONE_VFLAG) != 0) {
    Util_indent(level); ObjectUtil_eprintf("X[%02d] Object_rc_done. prc=%d rc=%d self=%v\n", level, parent_rc, self->rc, self);
    Util_indent(level); ObjectUtil_eprintf("Loop detected\n");
    return;
  }

  self->visited = self->visited | OBJECT_RC_DONE_VFLAG;

  Symbol* self_type = Object_type(self);
  Util_indent(level); ObjectUtil_eprintf("A[%02d] Object_rc_done. prc=%d rc=%d self=%v\n", level, parent_rc, self->rc, self);
  int i = 0;
  Object* tmp;
  if(self_type == SYMBOL_LIST) {
    self->rc += parent_rc;
    ListIter* list_iter = ListIter_new(self->impl);
    ListIter_next(list_iter);
    while(!ListIter_at_end(list_iter)) {
      tmp = ListIter_get_val(list_iter);
      Object_rc_done(tmp, self->rc, level+1);
      ListIter_next(list_iter);
    }
    ListIter_del(list_iter);
    list_iter = NULL;
  }
  else
  if(self_type == SYMBOL_HASH) {
    self->rc += parent_rc;
    HashIter* hash_iter = HashIter_new(self->impl);
    HashIter_next(hash_iter);
    while(!HashIter_at_end(hash_iter)) {
      Object_rc_done(HashIter_get_key(hash_iter), self->rc, level+1);
      Object_rc_done(HashIter_get_val(hash_iter), self->rc, level+1);
      HashIter_next(hash_iter);
    }
    HashIter_del(hash_iter);
    hash_iter = NULL;
  }
  else
  if(self_type == SYMBOL_ENVIRONMENT) {
    self->rc += parent_rc;
    Environment* env = (Environment*)(self->impl);
    if(env->objects != NULL) {
      Object_rc_done(env->objects, self->rc, level+1);
    }
    if(env->children != NULL) {
      Object_rc_done(env->children, self->rc, level+1);
    }
  }
  else {
    // non-composite Objects like String, Number, Symbol
    self->rc += parent_rc;
  }
  self->visited = self->visited & ~OBJECT_RC_DONE_VFLAG;
  /* Util_indent(level); ObjectUtil_eprintf("B[%02d] Object_rc_done. prc=%d rc=%d self=%v\n", level, parent_rc, self->rc, self); */
}

void Object_system_done() {
  if(object_system->done_called) {
    return;
  }
  dbg_printf("--- { Object_system_done() { ---\n");
  object_system->done_called = 1;
#ifdef DEBUG
  Object_system_print();
#endif

  Object* obj_curr = NULL;
  Object* obj_next = NULL;
  Object* obj_temp_1 = NULL;
  Object* obj_temp_2 = NULL;
  int i;

  // copy the rc into the rc_gc as our working scrap
  //obj_curr = object_system->head;
  //obj_next = NULL;
  //while(obj_curr != NULL) {
  //  obj_curr->rc_gc = obj_curr->rc;
  //  obj_curr = obj_curr->next;
  //}

  //dbg_printf("--- { Object_system_done(). Object_rc_done() { ---\n");
  //// Ensure that the rc of
  //// simple nested objects is higher than
  //// that of complex container objects.
  //obj_curr = object_system->head;
  //obj_next = NULL;
  //while(obj_curr != NULL) {
  //  obj_next = obj_curr->next;
  //  Object_rc_done(obj_curr, 0, 0);
  //  obj_curr = obj_next;
  //}
  //dbg_printf("--- } Object_system_done(). Object_rc_done() } ---\n");

  //dbg_printf("--- { Object_system_done(). 002 { ---\n");
  //Object_system_print();
  //dbg_printf("--- } Object_system_done(). 002 } ---\n");

  //Symbol* obj_curr_type;
  //obj_curr = object_system->head;
  //while(obj_curr != NULL) {
  //  obj_curr_type = Object_type(obj_curr);
  //  if(obj_curr_type == SYMBOL_LIST) {
  //    ListIter* list_iter = ListIter_new(obj_curr->impl);
  //    ListIter_next(list_iter);
  //    while(!ListIter_at_end(list_iter)) {
  //      obj_temp_1 = ListIter_get_val(list_iter);
  //      ListIter_next(list_iter);
  //    }
  //    ListIter_del(list_iter);
  //    list_iter = NULL;
  //  }
  //  else
  //  if(obj_curr_type == SYMBOL_HASH) {
  //    HashIter* hash_iter = HashIter_new(obj_curr->impl);
  //    HashIter_next(hash_iter);
  //    while(!HashIter_at_end(hash_iter)) {
  //      obj_temp_1 = HashIter_get_key(hash_iter);
  //      obj_temp_2 = HashIter_get_val(hash_iter);
  //      HashIter_next(hash_iter);
  //    }
  //    HashIter_del(hash_iter);
  //    hash_iter = NULL;
  //  }
  //  else
  //  if(obj_curr_type == SYMBOL_ENVIRONMENT) {
  //    Environment* env = (Environment*)(obj_curr->impl);
  //    if(env->objects != NULL) {
  //      obj_temp_1 = env->objects; // Object<Hash>
  //    }
  //    if(env->children != NULL) {
  //      obj_temp_1 = env->objects; // Object<List>
  //    }
  //  }
  //  else {
  //    // non-composite Objects like String, Number, Symbol
  //  }
  //  obj_curr = obj_curr->next;
  //}

  // Above should guarantee that all container objects
  // have an RC lower than their contained objects, thus
  // get deleted before the things they contain. 

  dbg_printf("--- { OSDK { ---\n");
  // delete all objects
  object_system->delete_recurse = 0;
  while(object_system->size > 0) {
    obj_curr = object_system->head;
    obj_next = NULL;
    while(obj_curr != NULL) {
      obj_next = obj_curr->next; 
      if(obj_curr->rc <= 1) {
        if(Object_is_container(obj_curr)) {
#ifdef DEBUG
          ObjectUtil_eprintf("[OSDK] || Object(%p) || type=%s || CONTAINER", obj_curr, Object_type(obj_curr)->str);
#endif
        } else {
#ifdef DEBUG
          ObjectUtil_eprintf("[OSDK] || Object(%p) || %v", obj_curr, obj_curr);
#endif
        }
        if(obj_curr->returning) {
#ifdef DEBUG
          dbg_printf(" || RT_WARNING");
#endif
          obj_curr->returning = 0;
        }
        dbg_printf("\n");
      }
      obj_curr = Object_rc_decr(obj_curr);
      obj_curr = obj_next;
    }
  }
  object_system->delete_recurse = 1;
  dbg_printf("--- } OSDK } ---\n");

  // delete type information
  ObjectTypeInfo* oti_curr;
  ObjectTypeInfo* oti_next;
  for(i = 0; i < OBJECT_TYPES_BUCKETS_SIZE; i++) {
    oti_curr = object_system->types[i];
    while(oti_curr != NULL) {
      oti_next = oti_curr->next;
      if(oti_next != NULL) {
        oti_next->prev = NULL;
      }
      oti_curr->next = NULL;
      //---
      free(oti_curr);
      //---
      object_system->types[i] = oti_next;
      oti_curr = oti_next;
    }
  }
  free(object_system);
  dbg_printf("--- } Object_system_done() } ---\n");
}

Symbol* Object_type(Object* self) {
  assert(self != NULL);
  return self->type;
}

Object* Object_rc_incr(Object* self) {
  /* assert(self != NULL); */
  if(self == NULL) {
    return self;
  }
  if(Object_is_null(self)) {
    // do nothing
    return self;
  }
  self->rc++;
  return self;
}

Object* Object_gc(Object* self) {
  assert(self != NULL);
  if(self->rc <= 0) {
    if(self->returning) {
      /*dbg_printf("Object_gc(%p). Object is returning. -SKIPPED-\n", self);*/
      /* ObjectUtil_eprintf("Object_gc(%p). SKIPPED returning object: %v\n", self, self); */
      return self;
    }
    Object_del(self);
    return NULL;
  }
  return self;
}

Object* Object_rc_decr(Object* self) {
  /* assert(self != NULL); */
  if(self == NULL) {
    return self;
  }
  if(Object_is_null(self) && object_system->done_called == 0) {
    // the object_system is still running.
    // do nothing.
    return self;
  }
  /* printf("donuts. Object_rc_decr. self p=%p t=%p | ", self, self->type); */
  /* Object_print(self); */
  /* printf("\n"); */
  /* ObjectUtil_eprintf("donuts. Object_rc_decr. self = %v p = %p\n", self, self); */
  if(self->rc >= 1) {
    self->rc--;
  }
  return Object_gc(self);
}

void Object_print(Object* self) {
  if(self == NULL) {
    printf("(NULL)");
    goto _return;
  }
  assert(self->type != NULL);
  if((self->visited & OBJECT_PRINT_VFLAG) != 0) {
    // this object has already been visited;
    printf("CYCLE(%p)", self);
    goto _return;
  }
  self->visited = self->visited | OBJECT_PRINT_VFLAG;
  ObjectTypeInfo* oti = Object_oti_get(self->type);
  if(oti == NULL) {
    printf("FATAL: unknown ObjectTypeInfo oti for type ");
    Symbol_print(self->type);
    exit(1);
  }
  // Or you might want to have a default print.
  if(oti->fn_print == NULL) {
    printf("FATAL: ObjectTypeInfo oti for type ");
    Symbol_print(self->type);
    printf(" is missing fn_print\n");
    exit(1);
  }
  oti->fn_print(self->impl);
_return:
  if(self != NULL) {
    self->visited = self->visited & ~OBJECT_PRINT_VFLAG;
  }
  return;
}

Object* Object_clone(Object* self) {
  assert(self != NULL);
  Object_rc_incr(self);

  ObjectTypeInfo* oti = Object_oti_get(self->type);

  Object* ret = NULL;

  if(oti == NULL) {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Missing oti for Object Type")));
    goto _return;
  }
  if(oti->fn_clone == NULL) {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Missing clone for Object Type")));
    goto _return;
  }

  if(!self->cloneable) {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Object is not cloneable")));
    goto _return;
  }

  void* rawobj = oti->fn_clone(self->impl);
  ret = Object_return(Object_new(self->type, 0, rawobj));

_return:
  assert(ret != NULL);
  Object_rc_decr(self);
  return ret;
}

/**
 * Reset this object to the zero object.
 */
Object* Object_zero(Object* self) {
  assert(self != NULL);
  Object_rc_incr(self);

  char success = 0;
  char* error_msg;

  Object* ret = NULL;
  Symbol* self_type = Object_type(self);
  ObjectTypeInfo* oti = Object_oti_get(self_type);
  
  if(oti != NULL && oti->fn_zero != NULL) {
    success = oti->fn_zero(self->impl);
    if(success) {
      ret = self;
    } else {
      if(ErrorSystem_get_code() != 0) {
        error_msg = ErrorSystem_get_msg();
        ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new(error_msg)));
        ErrorSystem_reset();
      } 
      else {
        ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Object_zero. fn_zero. strange error")));
      }
    }
  } else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Missing zero for Object Type")));
  }
  Object_rc_decr(self);
  return self;
}

int Object_cmp(Object* a, Object* b) {
  assert(a != NULL); assert(b != NULL);
  /* Object_rc_incr(a); Object_rc_incr(b); */
  Object_accept(a); 
  Object_accept(b);
  assert(a->returning == 0); 
  assert(b->returning == 0);
  int ret = -9;
  if(a == b) {
    // two objects that point to the same memory address
    // are the same object.
    ret = 0;
  }
  else
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    ret = Number_cmp(a->impl, b->impl);
  }
  else
  if(Object_type(a) == SYMBOL_STRING && Object_type(b) == SYMBOL_STRING) {
    ret = String_cmp(a->impl, b->impl);
  }
  else
  if(Object_type(a) == SYMBOL_SYMBOL && Object_type(b) == SYMBOL_SYMBOL) {
    ret = Symbol_cmp(a->impl, b->impl);
  }
  else
  if(Object_type(a) == SYMBOL_LIST && Object_type(b) == SYMBOL_LIST) {
    ret = List_cmp(a->impl, b->impl);
  }
  else
  if(Object_type(a) == SYMBOL_HASH && Object_type(b) == SYMBOL_HASH) {
    ret = Hash_cmp(a->impl, b->impl);
  }
  else {
    if(ObjectSystem_debug_001) {
      printf("donuts. here. debug_001\n");
      ObjectSystem_debug_001 = 0;
    }
  }
  Object_rc_decr(a); Object_rc_decr(b);
  return ret;
}

/**
 * Add objects a and b, modifying a.
 */
Object* Object_bop_addx(Object* a, Object* b) {
  assert(a != NULL); assert(b != NULL);
  Object_rc_incr(a); Object_rc_incr(b);
  Object* ret = NULL;
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    Number_addx(a->impl, b->impl);
    ret = a;
  }
  else
  if(Object_type(a) == SYMBOL_STRING && Object_type(b) == SYMBOL_STRING) {
    String_addx(a->impl, b->impl);
    ret = a;
  }
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types bop_addx")));
  }
  Object_rc_decr(a); Object_rc_decr(b);
  return ret;
}

/**
 * Add char CH into Object<String> A, modifying A. 
 **/
Object* Object_bop_addx_char(Object* a, char ch) {
  assert(a != NULL); assert(ch != '\0');
  Object_rc_incr(a);
  Object* ret = NULL;
  if(Object_type(a) == SYMBOL_STRING) {
    String_addx_char(a->impl, ch);
    ret = a;
  }
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types bop_addx")));
  }
  Object_rc_decr(a);
  return ret;
}

Object* Object_top_hset(Object* self, Object* key, Object* val) {
  assert(self != NULL);
  Object_accept(self);
  Object_accept(key);
  Object_accept(val);

  Object* ret = NULL;

  if(Object_type(self) == SYMBOL_HASH) {
    ret = Hash_set(self->impl, key, val);
  }
  else
  if(Object_type(self) == SYMBOL_ENVIRONMENT) {
    ret = Environment_set(self->impl, key, val);
  }
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Method missing")));
  }

  if(ret == NULL) {
    ret = Object_new_null();
  }
  
  Object_assign(&key, NULL);
  Object_assign(&val, NULL);
  Object_assign(&self, NULL);
  return ret;
}

char Object_bop_hhas(Object* self, Object* key) {
  assert(self != NULL);
  char ret = 0;

  if(Object_type(self) == SYMBOL_HASH) {
    ret = Hash_has(self->impl, key);
  }
  else
  if(Object_type(self) == SYMBOL_ENVIRONMENT) {
    ret = Environment_has(self->impl, key);
  }
  return ret;
}

Object* Object_bop_hget(Object* self, Object* key) {
  assert(self != NULL);
  Object* ret = NULL;

  if(Object_type(self) == SYMBOL_HASH) {
    ret = Hash_get(self->impl, key);
  }
  else
  if(Object_type(self) == SYMBOL_ENVIRONMENT) {
    ret = Environment_get(self->impl, key);
  }
  else {
    return Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Method missing")));
  }
  if(ret == NULL) {
    ret = Object_new_null();
  }
  Object_return(ret);
  return ret;
}

Object* Object_bop_hrem(Object* self, Object* key) {
  assert(self != NULL);
  Object* ret = NULL;
  if(Object_type(self) == SYMBOL_HASH) {
    Hash_rem(self->impl, key);
  }
  else
  if(Object_type(self) == SYMBOL_ENVIRONMENT) {
    Environment_rem(self->impl, key);
  }
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Method missing")));
  }
  if(ret == NULL) {
    ret = Object_new_null();
  }
  return ret;
}

/**
 * @param self An Object* of type Function
 * @param argv An Object* of type List
 **/
Object* Object_bop_call(Object* self, Object* argv) {
  assert(self != NULL);
  assert(argv != NULL);

  if(Object_type(self) != SYMBOL_FUNCTION) {
    return QERROR_NEW0("Expecting self to be of type Object<Function>");
  }

  if(Object_type(argv) != SYMBOL_LIST) {
    return QERROR_NEW0("Expecting argv to be of type Object<List>");
  }

  Function* fn = self->impl;
  return Function_call(fn, argv);
}

/**
 * Remove an item from an Object<List>.
 **/
Object* Object_bop_rem(Object* self, Object* item) {
  assert(self != NULL);
  if(Object_type(self) == SYMBOL_LIST) {
    List_rem(self->impl, item);
    return Object_new_null();
  }
  else {
    return QERROR("invalid type for object_bop_rem");
  }
}

Object* Object_bop_add(Object* a, Object* b) {
  assert(a != NULL); assert(b != NULL);
  Object_rc_incr(a); Object_rc_incr(b);
  Object* ret = NULL;
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    ret = Object_return(Object_new(SYMBOL_NUMBER, 0, Number_add(a->impl, b->impl)));
  }
  else
  if(Object_type(a) == SYMBOL_STRING && Object_type(b) == SYMBOL_STRING) {
    ret = Object_return(Object_new(SYMBOL_STRING, 0, String_add(a->impl, b->impl)));
  } 
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types bop_add")));
  }
  Object_rc_decr(a); Object_rc_decr(b);
  assert(ret != NULL);
  return ret;
}

Object* Object_bop_sub(Object* a, Object* b) {
  assert(a != NULL); assert(b != NULL);
  Object_rc_incr(a); Object_rc_incr(b);
  Object* ret = NULL; 
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    ret = Object_return(Object_new(SYMBOL_NUMBER, 0, Number_sub(a->impl, b->impl)));
  } 
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types for bop_sub")));
  }
  Object_rc_decr(a); Object_rc_decr(b);
  assert(ret != NULL);
  return ret;
}

Object* Object_bop_mul(Object* a, Object* b) {
  assert(a != NULL); assert(b != NULL);
  Object_rc_incr(a); Object_rc_incr(b);
  Object* ret = NULL;
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    ret = Object_return(Object_new(SYMBOL_NUMBER, 0, Number_mul(a->impl, b->impl)));
  } 
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types for bop_mul")));
  }
  Object_rc_decr(a); Object_rc_decr(b);
  assert(ret != NULL);
  return ret;
}

Object* Object_bop_div(Object* a, Object* b) {
  assert(a != NULL); assert(b != NULL);
  Object_rc_incr(a); Object_rc_incr(b);
  Object* ret = NULL;
  if(Object_type(a) == SYMBOL_NUMBER && Object_type(b) == SYMBOL_NUMBER) {
    ret = Object_return(Object_new(SYMBOL_NUMBER, 0, Number_div(a->impl, b->impl)));
  }
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types for bop_div")));
  }
  Object_rc_decr(a); Object_rc_decr(b);
  assert(ret != NULL);
  return ret;
}

char Object_bop_charat(Object* self, size_t idx) {
  assert(self != NULL);
  char ret = '\0';
  Object_rc_incr(self);
  if(Object_type(self) == SYMBOL_STRING) {
    ret = String_charat(self->impl, idx);
  }
 Object_rc_decr(self);
 return ret;
}

Object* Object_bop_at(Object* self, size_t idx) {
  assert(self != NULL);
  Object* ret = NULL;
  Object_rc_incr(self);
  if(Object_type(self) == SYMBOL_LIST) {
    ret = List_at(self->impl, idx);
  } 
  else {
    ret = Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types for bop_at"));
  }
  Object_rc_decr(self);
  assert(ret != NULL);
  return Object_return(ret);
}

/**
 * Return the first element of the list
 * if it exists or object_null.
 */
Object* Object_uop_head(Object* self) {
  Object_accept(self);
  Object* ret = Object_new_null();
  if(Object_type(self) == SYMBOL_LIST) {
    ret = Object_accept(List_head(self->impl));
  }
  Object_assign(&self, NULL);
  Object_return(ret);
  Object_rc_decr(ret);
  return ret;
}

/**
 * Return the last element of the list 
 * if it exists or object_null.
 */
Object* Object_uop_tail(Object* self) {
  if(Object_type(self) == SYMBOL_LIST) {
    return List_tail(self->impl);
  }
  return Object_new_null();
}

Object* Object_uop_rest(Object* self) {
  assert(self != NULL);
  Object_accept(self);
  Object* ret = Object_new_null();
  if(Object_type(self) == SYMBOL_LIST) {
    // clone the list
    Object* clone = Object_accept(Object_clone(self));
    // delete head item of clone list.
    Object_reject(Object_uop_shift(clone));
    ret = clone;
  }
  Object_assign(&self, NULL);
  if(!Object_is_null(ret)) {
    Object_return(ret); // mark for returning
    Object_rc_decr(ret);
  }
  return ret;
}

Object* Object_bop_push(Object* a, Object* b) {
  assert(a != NULL); assert(b != NULL);
  Object_rc_incr(a); Object_rc_incr(b);
  Object* ret = NULL;
  if(Object_type(a) == SYMBOL_LIST) {
    List_push(a->impl, b);
    ret = Object_return(a);
    // above is useful for chaining

    // we don't need to return a pointer to 'a' , the caller
    // already has a pointer to a
    /* ret = Object_new_null(); */
  }
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types for bop_push")));
  }
  Object_rc_decr(a); Object_rc_decr(b);
  assert(ret != NULL);
  return ret;
}

Object* Object_uop_pop(Object* a) {
  assert(a != NULL);
  Object_rc_incr(a);
  Object* ret = NULL;
  if(Object_type(a) == SYMBOL_LIST) {
    ret = List_pop(a->impl);
    if(ret == NULL) {
      // translate the raw NULL to Object-System's null_object.
      ret = Object_new_null();
    }
    ret = Object_return(ret);
  }
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Invalid type for uop_pop")));
  }
  Object_rc_decr(a);
  assert(ret != NULL);
  return ret;
}

Object* Object_bop_unshift(Object* a, Object* b) {
  assert(a != NULL); assert(b != NULL);
  Object_rc_incr(a); Object_rc_incr(b);
  Object* ret = NULL;
  if(Object_type(a) == SYMBOL_LIST) {
    List_unshift(a->impl, b);
    ret = Object_return(a);
  }
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types for bop_unshift")));
  }
  Object_rc_decr(a); Object_rc_decr(b);
  assert(ret != NULL);
  return ret;
}

Object* Object_uop_shift(Object* a) {
  assert(a != NULL);
  Object_rc_incr(a);
  Object* ret = NULL;
  if(Object_type(a) == SYMBOL_LIST) {
    ret = List_shift(a->impl);
    if(ret == NULL) {
      // translate raw NULL to null_object.
      ret = Object_new_null();
    }
    ret = Object_return(ret);
  }
  else {
    ret = Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Invalid types for bop_shift")));
  }
  Object_rc_decr(a);
  assert(ret != NULL);
  return ret;
}

Object* Object_bop_child_attach(Object* self, Object* child) {
  assert(self != NULL);
  assert(child != NULL);
  Object* ret = Object_new_null();
  if(Object_type(self) == SYMBOL_ENVIRONMENT && Object_type(child) == SYMBOL_ENVIRONMENT) {
    Environment_child_attach(self, child);
  } 
  else {
    ret = QERROR("invalid types for bop_child_attach");
  }
  return ret;
}

Object* Object_bop_child_detach(Object* self, Object* child) {
  assert(self != NULL);
  assert(child != NULL);
  Object_accept(self);
  Object_accept(child);
  Object* ret = Object_new_null();
  if(Object_type(self) == SYMBOL_ENVIRONMENT && Object_type(child) == SYMBOL_ENVIRONMENT) {
    printf("donuts. detaching child ...\n");
    Environment_child_detach(self, child);
  }
  else {
    ret = QERROR("invalid types for bop_child_detach");
  }
  Object_assign(&self, NULL);
  Object_assign(&child, NULL);
  return ret;
}

Object* Object_bop_rfind(Object* self, Object* key) {
  assert(self != NULL);
  assert(key != NULL);
  assert(Object_type(self) == SYMBOL_ENVIRONMENT);

  Environment* env = (Environment*) self->impl;

  Object* ret = Object_new_null();
  if(Object_bop_hhas(self, key)) {
    ret = Object_return(self);
  }
  else
  if(env->parent != NULL) {
    ret = Object_bop_rfind(env->parent, key);
  }
  
  return ret;
}

/**
 * Get the hash of the object for use in Hash tables
 * or anywhere else that a hash might be needed.
 * Should this be renamed Object_uop_hash ?
 */
size_t Object_hash(Object* self) {
  assert(self != NULL);
  Object_rc_incr(self); 
  size_t ret = 0;
  if(Object_type(self) == SYMBOL_SYMBOL) {
    ret = Symbol_hash(self->impl);
  }
  else
  if(Object_type(self) == SYMBOL_STRING) {
    ret = String_hash(self->impl);
  }
  else {
    dbg_printf("ERROR: Object_hash called on unsupported type");
  }
  Object_rc_decr(self);
  return ret;
}

/**
 * Print all the objects in the object_system.
 * Mainly for debugging. 
 */
void Object_system_print() {
  printf("--- { Object_system_print() { ---\n");
  Object* iter = object_system->head;
  int i = 0;
  int env_count = 0;
  int ret_count = 0;
  while(iter != NULL) {
    printf("[i=%03d] || Object(%p, rc=%03d/%03d, rt=%03d, u=%d, t=%p) || ", i, iter, iter->rc, iter->rc_gc, iter->returning, iter->unreachable, iter->type);
    fflush(stdout);
    Object_print(iter);
    printf("\n");
    if(Object_type(iter) == SYMBOL_ENVIRONMENT) {
      env_count++;
    }
    if(iter->returning) {
      ret_count++;
    }
    iter = iter->next;
    i++;
  }
  printf("--------------------------------------\n");
  printf("SIZE: %zu\n", object_system->size);
  printf("ENV_COUNT: %d\n", env_count);
  printf("RET_COUNT: %d\n", ret_count);
  printf("--- } Object_system_print() } ---\n");
}

void _osg_print(Object* object) {
  if(object->visited & OBJECT_OSG_VFLAG) {
    return;
  }
  object->visited = object->visited | OBJECT_OSG_VFLAG;
  fprintf(osg_ofile, "  obj_%p [label=\"%s(%p)", object, object->type->str, object);
  if(object->type == SYMBOL_SYMBOL) {
    fprintf(osg_ofile, "\\n%s", ((Symbol*)(object->impl))->str);
  }
  else
  if(object->type == SYMBOL_NUMBER) {
    fprintf(osg_ofile, "\\n%f", ((Number*)(object->impl))->rep);
  }
  else 
  if(object->type == SYMBOL_NUMBER) {
    fprintf(osg_ofile, "\\n%s", ((String*)(object->impl))->buf);
  }
  fprintf(osg_ofile, "\"];\n");
}
void _os_graph_walker(Object* referer, Object* referend) {
  _osg_print(referer);
  _osg_print(referend);
  fprintf(osg_ofile, "  obj_%p -> obj_%p;\n", referer, referend);
}
void Object_system_graph() {
  osg_ofile = fopen("tmp/osg.dot", "w");
  fprintf(osg_ofile, "digraph G {\n");
  fprintf(osg_ofile, "  node[shape=\"box\"]\n");
  Object_system_walkrefs(_os_graph_walker);
  fprintf(osg_ofile, "}\n");
  fclose(osg_ofile);
}

/**
 * Return the length of Object. 
 */
ssize_t Object_len(Object* self) {
  assert(self != NULL);
  Object_rc_incr(self);
  ssize_t ret = 0;

  /* if(Object_type(self) == SYMBOL_LIST) { */
  /*   ret = List_len(self->impl); */
  /* } */
  /* else */
  /* if(Object_type(self) == SYMBOL_STRING) { */
  /*   ret = String_len(self->impl); */
  /* } */
  /* else */
  /* if(Object_type(self) == SYMBOL_HASH) { */
  /*   ret = Hash_len(self->impl); */
  /* } */

  if(Object_is_null(self)) {
    ret = 0;
  }
  else {
    ObjectTypeInfo* oti = Object_oti_get(self->type);
    if(oti == NULL) {
      dbg_printf("FATAL: unknown ObjectTypeInfo oti for type ");
      Symbol_print(self->type);
      exit(1);
    }
    // Or you might want to have a default print.
    if(oti->fn_len == NULL) {
      dbg_printf("FATAL: ObjectTypeInfo oti for type ");
      Symbol_print(self->type);
      dbg_printf(" is missing fn_len\n");
      exit(1);
    }
    ret = oti->fn_len(self->impl);
  }

  Object_rc_decr(self);
  return ret;
}

/* Object* Object_from_result(Result res) { */
/*   /1* dbg_printf("Object_from_result\n"); *1/ */
/*   if(res.err == 0) { */
/*     // success -- err == 0 */
/*     if(res.ptr != NULL) { */
/*       // a success is always treated as an Object */
/*       return Object_return((Object*) res.ptr); */
/*     } */
/*     else { */
/*       return Object_new_null(); */
/*     } */
/*   } */
/*   else { */
/*     // failure -- err != 0 */
/*     if(res.ptr != NULL) { */
/*       // an error possibly bubbled up from the object system */
/*       return Object_return((Object*) res.ptr); */
/*     } */
/*     else */
/*     if(res.msg != NULL) { */
/*       // a raw cstr error message was set somewhere */
/*       return Object_return(QERROR_NEW0(res.msg)); */
/*     } */
/*     else { */
/*       // only an error code was supplied without a message */
/*       return Object_return(QERROR_NEW0("generic error")); */
/*     } */
/*   } */
/* } */

void ObjectUtil_eprintf_sig(int SIGSIZE, char** sigptr, int* sigposptr, va_list* argv) {
  /*dbg_printf("--- ObjectUtil_eprintf_sig ---\n");*/
  char* sig = *sigptr;
  /*dbg_printf("[sig=|%s|]", sig);*/
  if(strcmp(sig, "%v") == 0) {
    Object_print(va_arg(*argv, void*));
  }
  else {
    vprintf(*sigptr, *argv);
  }
  memset(*sigptr, 0, SIGSIZE);
  *sigposptr = 0;
}

void ObjectUtil_eprintf_buf(int BUFSIZE, char** bufptr, int* bufposptr) {
  /*dbg_printf("--- ObjectUtil_eprintf_buf ---\n");*/
  if(*bufposptr > 0) {
    fputs(*bufptr, stdout);
  }
  memset(*bufptr, 0, BUFSIZE);
  *bufposptr = 0;
}

void ObjectUtil_eprintf(char* fmt, ...) {
  /*dbg_printf("ObjectUtil_eprintf. fmt=%s\n", fmt);*/
  int i, j;
  int argc = 0;
  va_list argv;
  size_t fmt_len = strlen(fmt);
  char ch;
  for(i = 0; i < fmt_len; i++) {
    ch = fmt[i];
    if(ch == '%') {
      argc++;
    }
  }
  va_start(argv, fmt);
  int   BUFSIZE = fmt_len+1;
  char* buf = calloc(1, BUFSIZE);
  int   bufpos = 0;
  int   SIGSIZE = 20; 
  char* sig = calloc(1, SIGSIZE);
  int   sigpos = 0;
  char  insig = 0;
  int   argpos = 0;
  for(i = 0; i < fmt_len; i++) {
    ch = fmt[i];
    if(insig) {
      if((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')) {
        sig[sigpos++] = ch;
      }
      else {
        ObjectUtil_eprintf_sig(SIGSIZE, &sig, &sigpos, &argv);
        insig = 0;
        if(i > 0) { i--; }
      }
    }
    else
    if(ch == '%') {
      insig = 1;
      sig[0] = '%';
      sigpos = 1;
      ObjectUtil_eprintf_buf(BUFSIZE, &buf, &bufpos);
    }
    else {
      buf[bufpos++] = ch;
      /*dbg_printf("bufpos=%d / BUFSIZE=%d || buf=%s\n", bufpos, BUFSIZE, buf);*/
    }
  }
  if(insig && sigpos > 0) {
    ObjectUtil_eprintf_sig(SIGSIZE, &sig, &sigpos, &argv);
  }
  else
  if(bufpos > 0) {
    ObjectUtil_eprintf_buf(BUFSIZE, &buf, &bufpos);
  }
  free(buf);
  free(sig);
  va_end(argv);
}

int ObjectSystem_count_matching_number(double x) {
  int count = 0;
  Object* iter = object_system->head;
  Number* tmp;
  while(iter != NULL) {
    if(Object_type(iter) == SYMBOL_NUMBER) {
      tmp = iter->impl;
      if(tmp->rep == x) {
        count++;
      }
    }
    iter = iter->next;
  }
  return count;
}

int ObjectSystem_count_type(Symbol* type) {
  int count = 0;
  Object* iter = object_system->head;
  while(iter != NULL) {
    if(Object_type(iter) == type) {
      count++;
    }
    iter = iter->next;
  }
  return count;
}

