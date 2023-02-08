# SFR-Lisp

A very simple lisp-1. This is not designed to be
used for anything important, it is merely a self-learning exercise.

> What I cannot create in C, I do not understand.

## The Plan

- Make C at least as usable as Ruby
  - Symbol -- Just like Ruby Symbols
  - String -- Mutable Strings
  - Number -- Generic Numbers
  - Hash   -- Just like Ruby Hash
  - Error
  - Object
  - Shared Ptr Garbage Collection


What is the purpose of Symbol?

```
Object *obj = eval("(add 2 3)");

Symbol* atom = Object_type(obj);
if(Symbol_cstr_equal(atom, "Number")) {
  // do stuff
}

if(Object_is_type(obj, "Number")) {
  // do stuff
}

// this is beautiful
Symbol* number = Symbol_new("Number")
if(Object_type(obj) == number) {
}

Symbol* number1 = Symbol_new("Number");
Symbol* number2 = Symbol_new("Number");
assert(number1 == number2);
```

## Scrap

```
Tokenizer *tokenizer = Tokenizer_new();
tokenizer.enqueue_cstr("(print 'hello world')");
Token tok = Tokenizer_dequeue(tokenizer);
```

```
Lisp_system_init();
LispList* list = Lisp.parse("(print 'hello world'))
list[0] == Symbol_new("print")
list[1] == String_new("Hello World") 
Lisp_system_done();
```

```
Object_system_init();

typedef struct Object Object;
struct Object {
  size_t  id;
  Symbol* type;
  void*   impl;
};
TYPE_STRING = Symbol_new("String");
Object_reg_class(TYPE_STRING, String_del); // String_del is the 0-arity destructor
Object* str1 = Object_new(Symbol_new("String"), (void*) String_new("Hello"));

void FancyPrint(Object* str) {
  printf("=== FancyPrint ==="\n");
  String_inspect((String*) str->impl);
}

- Where does FancyPrint live?
- How do you call FancyPrint?

global = Scope_new(NULL, Symbol_new("global"));
Object_reg_class(global, TYPE_STRING, String_del);

Object* str1 = Object_new(global, TYPE_STRING, String_new("Hello"));
Object_assign(global, Symbol_new("str1"), str1);

Scope_del(global);
Object_system_done();
```
