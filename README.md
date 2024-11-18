# SFR-Lisp

A simple toy lisp-1 interpreter written in C.

This is not designed to be used for anything important, it is simply a learning
exercise. It is really a test bed to play with ideas around the design of C
object systems, memory management, macros, and etc. Writing a Lisp interpreter
is one of those programming exercises that touches many things, which makes it
a great exercise. I chose C because it is about as low-level as one can get
before assembler, and the most popular implementations of other interpreted
languages (Perl, Python, Ruby, Raku) are all written in C. 

To paraphrase one of my favorite physicists:

> What I cannot create [in C], I do not understand.
> -- Richard P. Feynman

There are definitely better ways to do it than what you see here, efficiency is
not the goal for this implementation. This implementation is meant to be as
self-contained a possible. There are no third-party parsers, memory-management
libraries, or complex build systems. The building is done by Ruby via a simple
readable Rakefile. There is a prototype implementation written in Ruby that I
wrote first. There is a test suite, which tests the correctness of the output
against Racket. There are memory leak tests using Valgrind, called by the test
suite.

The only third-party library used here is Linenoise, a tiny alternative to
Readline written by Antirez. The reason for this is because keyboard handling,
terminal specific escape codes for REPLs, and etc is somewhat outside of the
scope of what I intend on learning here. The functionality that Linenoise
provides is notoriously hard to get right across different systems. But one
day, I might switch this out as well.

## Building

### Requirements:

* GCC or Clang
* Valgrind >= v3.20
* [Racket](https://racket-lang.org/) -- This implementation is tested against
  Racket to confirm it is roughly working the same way. Racket is a far more
  mature lisp, and this is a tiny learning project.
* Ruby and Rake -- Used for building and running tests.
* Linenoise -- A small self-contained alternative to Readline by Antirez. 
  Vendored as a Git submodule in this repo under ./sub/linenoise). Used mainly
  for the REPL. [link](https://github.com/antirez/linenoise).

### Build Instructions

First clone this repository with the `--recursive` flag.
This will ensure that the submodule for `./sub/linenoise` is present.

With all of the above requirements met and available
run the following command

```
rake
```

This will populate the build directory `./build`.
You will a number of built executables, most of which
are test programs.

The executable for the interpreter will be located at
`./build/sfr-lisp`.

### Testing

To run all tests, simply run:

bash
```
rake test
```

## Usage

The interpreter without any arguments will start 
a REPL.
With a file path as the first argument, it will evaluate
the file as Lisp source and exit.

## The Plan

- Write a prototype Lisp interpreter in Ruby (see ./sfr-lisp.rb).
  This will help us understand the basic workings of Lisp without any of the
  memory management complications of C.
- Test this prototype Lisp interpreter against Racket.
- Now that we know how to do it in Ruby ...
- Make C at least as usable as Ruby
  - Primitives:
    - Single:
      - Symbol -- Like Ruby Symbols.
      - String -- Mutable Strings.
      - Number -- Generic Numbers.
      - Error  -- Generic Errors.
    - Containers:
      - List   -- List of objects.
      - Hash   -- A Hashmap of objects. (Similar to Ruby Hash).
      - Environment -- A tree-like object used to store Function scope.
    - Callable:
      - Function -- A wrapper object for function pointers.
  - Object -- Generic wrapper type which points to one of the above things.
  - Reference counted garbage collection for all objects.
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
which will be later created during parsing regardless. 

Unlike the current broken Tokenizer, we will return a complete List, there
is no reason to have 'streams' which implies there is some value in partial
tokenization. At the moment, we cannot see any value in doing it this way. 

### The Parser

The parser will convert the flat list of objects into a nested list of objects
representing the Lisp parse-tree. 

## Links and References

https://docs.racket-lang.org/guide/scheme-forms.html

