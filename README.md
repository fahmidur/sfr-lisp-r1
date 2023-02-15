# SFR-Lisp

A very simple lisp-1. This is not designed to be
used for anything important, it is merely a self-learning exercise.

> What I cannot create in C, I do not understand.

## The Plan

- Make C at least as usable as Ruby
  - Atomics
    - Symbol -- Just like Ruby Symbols
      - del
      - clone
    - String -- Mutable Strings
      - del
      - clone
    - Number -- Generic Numbers
      - del
      - clone
  - Compound
    - List   -- List of things.
    - Array
    - Hash   -- Similar to Ruby hash.
  - Object
  - Shared Ptr Garbage Collection

- Common object methods:
  - `new`      -- constructor
  - `del`      -- destructor
  - `clone`    -- cloner
  - `cmp`      -- compare, returns -1, 0, 1
  - `to_s`     -- To string.
