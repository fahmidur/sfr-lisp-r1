#ifndef _SFR_FUNCTION
#define _SFR_FUNCTION

typedef struct Function Function;
struct Function {
  int arity;
  List* retlist (impl*)(List* arglist);
};

#endif
