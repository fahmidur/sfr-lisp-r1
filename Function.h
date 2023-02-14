#ifndef _SFR_FUNCTION
#define _SFR_FUNCTION

typedef struct Function Function;
struct Function {
  int arity;
  FunctionRet (impl*)(FunctionArgList*);
};

#endif
