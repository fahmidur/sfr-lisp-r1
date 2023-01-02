#ifndef __SFR_INTERPRETER_H
#define __SFR_INTERPRETER_H

typedef struct Interpreter Interpreter;
struct Interpreter {
  Runtime* runtime;
};

Interpreter* Interpreter_new0(void);
Interpreter* Interpreter_new1(Runtime* runtime);

#endif

