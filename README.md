# SFR-Lisp


A very simple lisp-1. This is not designed to be
used for anything, it is merely a self-learning exercise.

## The Plan

- Make C at least as usable as C++
  - Atom
  - Number
  - String
  - Table
  - Error
  - Object
  - Shared Ptr Garbage Collection


What is the purpose of Atom?

```
Object *obj = eval("(add 2 3)");

Atom* atom = Object_type(obj);
if(Atom_cstr_equal(atom, "Number")) {
  // do stuff
}

if(Object_is_type(obj, "Number")) {
  // do stuff
}

// this is beautiful
Atom* number = Atom_new("Number")
if(Object_type(obj) == number) {
}

Atom* number1 = Atom_new("Number");
Atom* number2 = Atom_new("Number");
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
