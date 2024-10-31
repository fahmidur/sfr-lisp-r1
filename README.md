# SFR-Lisp

A simple toy lisp-1. 
This is not designed to be
used for anything important, it is merely a learning exercise.

> What I cannot create [in C], I do not understand.
> -- Richard P. Feynman

## Build Requirements

* Valgrind >= v3.20
* [Racket](https://racket-lang.org/) -- We test our implementation against racket to confirm that this implementation is roughly working the same way. Racket is a far more mature lisp, and this is a tiny learning project.
* Ruby and Rake
* GCC or Clang

## The Plan

- Write a lisp interpreter in Ruby (see ./sfr-lisp.rb).
- Make C at least as usable as Ruby
  - Primitives:
    - Atomics:
      - Symbol -- Like Ruby Symbols.
      - String -- Mutable Strings.
      - Number -- Generic Numbers.
    - Containers:
      - List   -- List of objects.
      - Hash   -- A Hashmap of objects. (Similar to Ruby Hash).
      - Environment -- A tree-like object used to store Function scope.
    - Function -- A wrapper object for function pointers.
  - Object -- Generic wrapper type which points to one of the above primitives.
  - Reference counted garbage collection for all objects
- Write a Lisp interpreter using the object-system created in C.

- Common object methods:
  - `new`      -- constructor
  - `del`      -- destructor
  - `clone`    -- cloner
  - `cmp`      -- compare, returns -1, 0, 1
  - `print`    -- print yourself

## Major Components

### The Tokenizer

The tokenizer should return a flat List of objects where each object is either:
- String
- Number
- Symbol -- Parenthesis and BareWords

There is no reason to have a temporary Token type object which points to objects
that will be later created during parsing regardless. 

Unlike the current broken Tokenizer, we will return a complete List, there
is no reason to have 'streams' which implies there is some value in partial
tokenization. At the moment, we cannot see any value in doing it this way. 

### The Parser

The parser will convert the flat list of objects into a nested list of objects
representing the parser tree. 

## Links and References

https://docs.racket-lang.org/guide/scheme-forms.html

