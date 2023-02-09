# SFR-Lisp

A very simple lisp-1. This is not designed to be
used for anything important, it is merely a self-learning exercise.

> What I cannot create in C, I do not understand.

## The Plan

- Make C at least as usable as Ruby
  - Symbol -- Just like Ruby Symbols
    - del
    - clone
  - String -- Mutable Strings
    - del
    - clone
  - Number -- Generic Numbers
    - del
    - clone
  - Hash   -- Just like Ruby Hash
  - Object
  - List   -- List of objects.
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
  Symbol* type;
  void*   impl;
};
TYPE_STRING = Symbol_new("String");
Object_reg_class(TYPE_STRING, String_del); // String_del is the 0-arity destructor
Object* str1 = Object_new(Symbol_new("String"), (void*) String_new("Hello"));

void fancyprint(Object* str) {
  printf("=== BEG. fancyprint ==="\n");
  String_inspect((String*) str->impl);
  printf("=== end. fancyprint ==="\n");
}

- Where does this function live?
- How do you call this function?

---

global = Scope_new(NULL, Symbol_new("global"));
Object_reg_class(global, TYPE_STRING, String_del);

Object* str1 = Object_new(global, TYPE_STRING, String_new("Hello"));
Object_var_assign_named(global, Symbol_new("str1"), str1);

Object* str2 = Object_new(global, TYPE_STRING, String_new("Hello2"));
str1 = str2;

str1 is now garbage

Scope_del(global);
Object_system_done();

---

Object* one = Object_new(...);
Object* two = Object_new(...);

two = one; 
```
