# SFR-Lisp

A very simple lisp-1. This is not designed to be
used for anything important, it is merely a self-learning exercise.

> What I cannot create in C, I do not understand.

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
