#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include "Symbol.h"
#include "String.h"
#include "Number.h"
#include "List.h"
#include "Hash.h"
#include "Environment.h"
#include "Error.h"
#include "Object.h"

ObjectSystem* object_system;

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
  printf("Object_oti_set("); Symbol_print(type); printf(")\n");
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
  /*printf("oti_old=%p\n", oti_old);*/
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
  printf("--- BEG. Object_system_init() ---\n");
  size_t i;
  object_system = calloc(1, sizeof(ObjectSystem));
  object_system->init_called = 0;
  object_system->done_called = 0;
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
  // Composit Types
  SYMBOL_LIST = Symbol_new("List");
  SYMBOL_HASH = Symbol_new("Hash");
  SYMBOL_ENVIRONMENT = Symbol_new("Environment");
  // Callable Object
  SYMBOL_FUNCTION = Symbol_new("Function");

  printf("\n\n");

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
    .fn_len   = (ssize_t (*)(void*))Environment_len,
  };
  Object_oti_set(SYMBOL_ENVIRONMENT, otiarg_environment);

  object_system->null_object = Object_new(SYMBOL_SYMBOL, 1, Symbol_new("NULL"));

  object_system->init_called = 1;
  object_system->done_called = 0;

  printf("--- END. Object_system_init() ---\n");
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

  Object* self = calloc(1, sizeof(Object));

  self->type = type;
  self->impl = impl;
  self->rc = rc;
  /*self->gc_skipped = 0;*/
  self->returning = 0;
  self->cloneable = 1;
  self->visited = 0;

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
    fprintf(stderr, "ERROR: Object_new_iter. Received non-iterable object\n");
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
    /*printf("debug. --- i=%d ---\n", i);*/
    tmp = va_arg(argv, void*);
    if(tmp == NULL) {
      tmp = Object_new_null();
    }
    assert(tmp != NULL);
    Object_accept(tmp);
    Object_rc_incr(tmp); // our reference to it in the args to this function
    /*ObjectUtil_eprintf("debug. pushing into list. bef. tmp= %v | .rc=%d | .rt=%d\n", tmp, tmp->rc, tmp->returning);*/
    Object_reject(Object_bop_push(list, tmp)); // push into this object, ignore error
    Object_rc_decr(tmp); // we are done with tmp
    /*ObjectUtil_eprintf("debug. pushing into list. aft. tmp= %v | .rc=%d | .rt=%d\n", tmp, tmp->rc, tmp->returning);*/
    /*ObjectUtil_eprintf("debug. list. rt=%d\n", list->returning);*/
    /*printf("debug. ---- --- \n");*/
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

void Object_system_gc() {
  Object* iter = object_system->head;
  Object* next = NULL;
  while(iter != NULL) {
    next = iter->next;
    iter = Object_gc(iter);
    iter = next;
  }
}

/**
 * TODO: make this idempotent.
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

char Object_is_composite(Object* self) {
  Symbol* self_type = Object_type(self);
  return (
    self_type == SYMBOL_LIST || 
    self_type == SYMBOL_HASH ||
    self_type == SYMBOL_ENVIRONMENT
  );
}

void Object_del(Object* self) {
  assert(self != NULL);
  printf("Object_del(%p). rc=%d. rt=%d. type=", self, self->rc, self->returning); 
    Symbol_print(Object_type(self)); 
    if(!Object_is_composite(self)) {
      printf(" || ");
      Object_print(self);
    }
  printf("\n");
  if(object_system->size == 0) {
    return;
  }
  Object* new_head;
  Object* new_tail;
  Object* prev;
  Object* next;
  if(self == object_system->head && self == object_system->tail) {
    object_system->head = NULL;
    object_system->tail = NULL;
  }
  else
  if(self == object_system->head) {
    new_head = self->next;
    assert(new_head != NULL);
    new_head->prev = NULL;
    self->next = NULL;
    object_system->head = new_head;
  }
  else
  if(self == object_system->tail) {
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
    printf("FATAL: unknown ObjectTypeInfo oti for type ");
    Symbol_print(self->type);
    exit(1);
  }
  if(oti->fn_del == NULL) {
    printf("FATAL: ObjectTypeInfo oti for type ");
    Symbol_print(self->type);
    printf(" is missing fn_del\n");
    exit(1);
  }
  oti->fn_del(self->impl);
  /*printf("Object_del(%p). calling free on self.\n", self);*/
  free(self);
}

/**
 * Recursively the refcount of this object to be 
 * greater than its parent object.
 * This ensures that all scalar objects have a higher
 * refcount than their composit parent containers objects.
 * Which then, ensures that parent container objects are 
 * GC'd before the objects they contain.
 **/
void Object_rc_done(Object* self, int parent_rc, int level, ObjectVisitRecord* ovr_tail) {
  assert(self != NULL);
  if(ovr_tail != NULL) {
    ObjectVisitRecord* iter = ovr_tail;
    int ovr_count = 0;
    while(iter != NULL) {
      Util_indent(level); 
      printf("donuts. [%2d] ovr iter=%p | ", ovr_count, iter);
      ObjectUtil_eprintf("obj=%v\n", iter->obj);
      if(iter->obj == self) {
        printf("Object_rc_done. Loop detected\n");
        return;
      }
      iter = iter->prev;
      ovr_count++;
    }
  }

  ObjectVisitRecord* new_ovr_tail;
  new_ovr_tail = (ObjectVisitRecord*) malloc(sizeof(ObjectVisitRecord));
  new_ovr_tail->obj = self;
  new_ovr_tail->prev = ovr_tail;
  if(ovr_tail != NULL) {
    ovr_tail->next = new_ovr_tail;
  }
  new_ovr_tail->next = NULL;

  Symbol* self_type = Object_type(self);
  Util_indent(level); ObjectUtil_eprintf("A[%02d] Object_rc_done. prc=%d rc=%d self=%v\n", level, parent_rc, self->rc, self);
  int i = 0;
  Object* tmp;
  if(self_type == SYMBOL_LIST) {
    //WARNING: this can infinite-loop if there are circular references.
    self->rc += parent_rc;
    ListIter* list_iter = ListIter_new(self->impl);
    ListIter_next(list_iter);
    while(!ListIter_at_end(list_iter)) {
      tmp = ListIter_get_val(list_iter);
      Object_rc_done(tmp, self->rc, level+1, new_ovr_tail);
      ListIter_next(list_iter);
    }
    ListIter_del(list_iter);
    list_iter = NULL;
  }
  else
  if(self_type == SYMBOL_HASH) {
    //WARNING: this can infinite-loop if there are circular references.
    self->rc += parent_rc;
    HashIter* hash_iter = HashIter_new(self->impl);
    HashIter_next(hash_iter);
    while(!HashIter_at_end(hash_iter)) {
      Object_rc_done(HashIter_get_key(hash_iter), self->rc, level+1, new_ovr_tail);
      Object_rc_done(HashIter_get_val(hash_iter), self->rc, level+1, new_ovr_tail);
      HashIter_next(hash_iter);
    }
    HashIter_del(hash_iter);
    hash_iter = NULL;
  }
  else
  if(self_type == SYMBOL_ENVIRONMENT) {
    //WARNING: this can infinite-loop if there are circular references.
    self->rc += parent_rc;
    Environment* env = (Environment*)(self->impl);
    if(env->objects != NULL) {
      Object_rc_done(env->objects, self->rc, level+1, new_ovr_tail);
    }
    if(env->children != NULL) {
      Object_rc_done(env->children, self->rc, level+1, new_ovr_tail);
    }
  }
  else {
    // non-composite Objects like String, Number, Symbol
    self->rc += parent_rc;
  }
  free(new_ovr_tail);
  new_ovr_tail = NULL;
  if(ovr_tail != NULL) {
    ovr_tail->next = NULL;
  }
  /* Util_indent(level); ObjectUtil_eprintf("B[%02d] Object_rc_done. prc=%d rc=%d self=%v\n", level, parent_rc, self->rc, self); */
}

void Object_system_done() {
  if(object_system->done_called) {
    return;
  }
  printf("--- { Object_system_done() { ---\n");
  object_system->done_called = 1;
  Object_system_print();

  Object* obj_curr = NULL;
  Object* obj_next = NULL;
  int i;

  printf("--- { Object_system_done(). Object_rc_done() { ---\n");
  // Ensure that the rc of
  // simple nested objects is higher than
  // that of complex container objects.
  obj_curr = object_system->head;
  obj_next = NULL;
  while(obj_curr != NULL) {
    obj_next = obj_curr->next;
    Object_rc_done(obj_curr, 0, 0, NULL);
    obj_curr = obj_next;
  }
  printf("--- } Object_system_done(). Object_rc_done() } ---\n");

  printf("--- { Object_system_done(). AFT Object_rc_done() { ---\n");
  Object_system_print();
  printf("--- } Object_system_done(). AFT Object_rc_done() } ---\n");

  // Above should guarantee that all container objects
  // get deleted before the things they contain. 

  printf("--- { OSDK { ---\n");
  // delete all objects
  while(object_system->size > 0) {
    /*Object_del(object_system->head);*/
    obj_curr = object_system->head;
    obj_next = NULL;
    while(obj_curr != NULL) {
      obj_next = obj_curr->next; 
      if(obj_curr->rc <= 1) {
        if(Object_is_composite(obj_curr)) {
          ObjectUtil_eprintf("[OSDK] || Object(p=%p) || type=%s || COMPOSITE", obj_curr, Object_type(obj_curr)->str);
        } else {
          ObjectUtil_eprintf("[OSDK] || Object(p=%p) || %v", obj_curr, obj_curr);
        }
        if(obj_curr->returning) {
          printf(" || RT_WARNING");
          obj_curr->returning = 0;
        }
        printf("\n");
      }
      obj_curr = Object_rc_decr(obj_curr);
      obj_curr = obj_next;
    }
  }
  printf("--- } OSDK } ---\n");

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
  printf("--- } Object_system_done() } ---\n");
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
  self->rc++;
  return self;
}

Object* Object_gc(Object* self) {
  assert(self != NULL);
  if(self->rc <= 0) {
    if(self->returning) {
      /*printf("Object_gc(%p). Object is returning. -SKIPPED-\n", self);*/
      /*ObjectUtil_eprintf("Object_gc(%p). SKIPPED returning object: %v\n", self, self);*/
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
  if((self->visited & OBJECT_PRINT_VFLAG) != 0) {
    // this object has already been visited;
    printf("CYCLE(%p)", self);
    goto _return;
  }
  self->visited |= OBJECT_PRINT_VFLAG;
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
    /* self->visited = (self->visited ^ OBJECT_PRINT_VFLAG); */
    self->visited = 0;
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
  Object_accept(a); Object_accept(b);
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
Object* Object_uop_first(Object* self) {
  // TODO: finish this
  return NULL;
}

/**
 * Return the last element of the list 
 * if it exists or object_null.
 */
Object* Object_uop_last(Object* self) {
  // TODO: finish this
  return NULL;
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
    printf("ERROR: Object_hash called on unsupported type");
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
  while(iter != NULL) {
    printf("[i=%03d] || Object(p=%p, rc=%03d, rt=%03d) || ", i, iter, iter->rc, iter->returning);
    Object_print(iter);
    printf("\n");
    iter = iter->next;
    i++;
  }
  printf("--------------------------------------\n");
  printf("SIZE: %zu\n", object_system->size);
  printf("--- } Object_system_print() } ---\n");
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
      printf("FATAL: unknown ObjectTypeInfo oti for type ");
      Symbol_print(self->type);
      exit(1);
    }
    // Or you might want to have a default print.
    if(oti->fn_len == NULL) {
      printf("FATAL: ObjectTypeInfo oti for type ");
      Symbol_print(self->type);
      printf(" is missing fn_len\n");
      exit(1);
    }
    ret = oti->fn_len(self->impl);
  }

  Object_rc_decr(self);
  return ret;
}

/* Object* Object_from_result(Result res) { */
/*   /1* printf("Object_from_result\n"); *1/ */
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

void ObjectUtil_eprintf_sig(int SIGSIZE, char** sigptr, int* sigposptr, va_list argv) {
  /*printf("--- ObjectUtil_eprintf_sig ---\n");*/
  char* sig = *sigptr;
  /*printf("[sig=|%s|]", sig);*/
  if(strcmp(sig, "%v") == 0) {
    Object_print(va_arg(argv, void*));
  }
  else {
    vprintf(*sigptr, argv);
  }
  memset(*sigptr, 0, SIGSIZE);
  *sigposptr = 0;
}

void ObjectUtil_eprintf_buf(int BUFSIZE, char** bufptr, int* bufposptr) {
  /*printf("--- ObjectUtil_eprintf_buf ---\n");*/
  if(*bufposptr > 0) {
    fputs(*bufptr, stdout);
  }
  memset(*bufptr, 0, BUFSIZE);
  *bufposptr = 0;
}

void ObjectUtil_eprintf(char* fmt, ...) {
  /*printf("ObjectUtil_eprintf. fmt=%s\n", fmt);*/
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
        ObjectUtil_eprintf_sig(SIGSIZE, &sig, &sigpos, argv);
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
      /*printf("bufpos=%d / BUFSIZE=%d || buf=%s\n", bufpos, BUFSIZE, buf);*/
    }
  }
  if(insig && sigpos > 0) {
    ObjectUtil_eprintf_sig(SIGSIZE, &sig, &sigpos, argv);
  }
  else
  if(bufpos > 0) {
    ObjectUtil_eprintf_buf(BUFSIZE, &buf, &bufpos);
  }
  free(buf);
  free(sig);
  va_end(argv);
}

