#ifndef _SFR_RUNTIME_H
#define _SFR_RUNTIME_H

#include <stdlib.h>

enum RoType {
  RotString,
  RotNumber,
  RotList,
};

typedef struct RuntimeObject RuntimeObject;
struct RuntimeObject {
  unsigned int id;
  enum RoType type;
};


// A very simple runtime which contains
// objects. 
// Each object has:
// - ID
// - Type
// - Reference-Count
// - Pointer-to-Impl
typedef struct Runtime Runtime;
struct Runtime {
  size_t objects_len;
  void** objects; // dynamically sized array of pointers
};

Runtime* Runtime_new(void);
// pre-allocate some heap space
// pre-allocate stack
// 
// (defvar x 123) -> (Expr)
//                     |
//               (Atom 'defvar)
//               /            \
//              /              \
//             /                \
//         (Atom 'x)          (Number 123)

// methods
bool Runtime_object_add(Runtime* self, void* obj);
bool Runtime_gc(Runtime* self);

#endif
