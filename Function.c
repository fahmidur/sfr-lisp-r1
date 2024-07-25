#include "Error.h"
#include "Function.h"

FunctionEnv* FunctionEnv_new(FunctionEnv* parent) {
  FunctionEnv* self = calloc(1, sizeof(FunctionEnv));

  if(self == NULL) {
    // ERROR: unable to reserve memory
    ErrorSystem_set(1, "FunctionEnv_new. calloc failed");
    return self;
  }

  self->children_head = NULL;
  self->children_tail = NULL;
  self->sibling_next = NULL;
  self->sibling_prev = NULL;
  self->objects_map = QHASH_NEW1();

  /* if(parent != NULL) { */
  /*   FunctionEnv_child_add(parent, self); */
  /* } */
  return self;
}

void FunctionEnv_del(FunctionEnv* self) {
  assert(self != NULL);
  if(self->children_head != NULL) {
  }
}

void FunctionEnv_child_add(FunctionEnv* self, FunctionEnv* child) {
  assert(self != NULL);
  assert(child != NULL);
  assert(child->sibling_next == NULL);
  assert(child->sibling_prev == NULL);
  child->parent = self;
  if(self->children_head == NULL && self->children_tail == NULL) {
    // first child being added
    self->children_head = self->children_tail = child;
    child->sibling_prev = NULL;
    child->sibling_next = NULL;
  }
  else {
    FunctionEnv* iter = self->children_head;
    while(iter != NULL) {
      if(iter == child) {
        // the child is already in this list
        return;
      }
      iter = iter->sibling_next;
    }
    FunctionEnv* last_child = self->children_tail;
    last_child->sibling_next = child;
    child->sibling_prev = last_child;
    self->children_tail = child;
  }
}

Function* Function_new(
  Object* (*impl)(Function* fn, Object* args), 
  int arity, 
  FunctionEnv* env
) {
  Function* self = calloc(1, sizeof(Function));
  self->arity = arity;
  self->impl = impl;
  assert(env != NULL);
  self->env = env;
  return self;
}

/*Object* Function_apply(Function* self, Object* args) {*/
  /*if(Object_type(args) != SYMBOL_LIST) {*/
    /*return Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Function expecting args of type SYMBOL_LIST")))*/
  /*}*/
  /*if(self->arity < 0 && self->arity != Object_len(args)) {*/
    /*return Object_return(Object_new(SYMBOL_ERROR, 0, Error_new("Function args length arity mismatch")));*/
  /*}*/
  /*// TODO: finish this*/
  /*return NULL;*/
/*}*/

/*Object* Function_call(Function* self, ...) {*/
  /*int argc = 0;*/
  /*va_list argv;*/
  /*if(self->arity >= 0) {*/
    /*argc = self->arity;*/
  /*}*/
  /*va_start(argv, self);*/
  /*switch(argc) {*/
    /*case 1:*/
      /*self->impl(*/
        /*va_arg(argv, Object*)*/
      /*);*/
      /*break;*/
    /*case 2:*/
      /*self->impl(*/
        /*va_arg(argv, Object*),*/
        /*va_arg(argv, Object*),*/
      /*);*/
      /*break;*/
    /*default:*/
      /*printf("ERROR: invalid function arity")*/
  /*}*/
  /*// TODO: auto-generate above switch block*/
  /*// for n=100 or some maximum number of args.*/
  /*va_end(argv);*/
/*}*/
