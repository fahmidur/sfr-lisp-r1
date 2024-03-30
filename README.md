# SFR-Lisp

A very simple lisp-1. This is not designed to be
used for anything important, it is merely a self-learning exercise.

> What I cannot create [in C], I do not understand.

## Requirements

* Valgrind >= v3.20
* [Racket](https://racket-lang.org/) -- We test our implementation against racket to confirm that this implementation is roughly working the same way. Racket is a far more mature lisp, and this is a tiny learning project.

## The Plan

- Make C at least as usable as Ruby
  - Primitives:
    - Atomics:
      - Symbol -- Just like Ruby Symbols
      - String -- Mutable Strings
      - Number -- Generic Numbers
    - Composits:
      - List   -- List of things.
      - Hash   -- Similar to Ruby hash.
  - Object
  - Shared Ptr Garbage Collection

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

