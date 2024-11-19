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
      - Symbol -- Generic Symbols, long-lived. (Similar to old Ruby symbols).
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
  - `new`      -- Constructor.
  - `del`      -- Destructor.
  - `clone`    -- Clone this object.
  - `cmp`      -- Compare with another object, returns -1, 0, 1.
  - `print`    -- Print this object.

## Design and Major Components

### The C Object System

The Object System designed here is primarily meant to make C 
as usable as any other Garbage Collected language.
Objects are a thin pointer-to-implementation wrapper around
more primitive types like Symbol, String, Number, List, and so on.

#### Implementation Classes

The following are notes on the implementation classes that are
wrapped by the Object class. 

It is the role of the implementation class to handle the memory of its members.
While it is the responsibility of the Object class to manage to memory of the
implementation class instance.

#### Symbol Class

The first primitive type created was the **Symbol**.
In some other implementations these are called Atoms, but here
I've named it Symbol after Ruby Symbols.

Symbols are treated differently from normal Objects in in that they are
are not garbage collected until the very end. Like an older version of Ruby,
Symbols only ever grow in quantitity. It's not great. But the idea, here 
is that Symbols should not be dynamically created by a program from user input,
and relatively few symbols should exist.

#### Number Class

The Number class is a simple wrapper around double. 
For the sake of simplicity, all numbers are doubles.
In the future, we may want to use a separate implementation for Integers,
BigIntegers, BigFloats, and so on.
But for now a Number as a double is a good start.

#### String Class

The String class is a simple wrapper around a `char* str`, which is a dynamically
allocated buffer. The buffer is resized as necessary by the string class.
This class handles things like concatenation, comparison and so on.
For a full list see the header file `./String.h`

#### Error Class

#### List Class

#### Hash Class

#### Object Class

### The C Garbage Collector

TODO: Add notes about the garbage collector


### The Lisp Implementation

The Lisp implementation is mainly defined in `lisp.c` with the driver
program `sfr-lisp.c`.
These files are consumers of the C Object System described above.
These two files are essentially 1-1 in functionality to the prototype implementation
`sfr-lisp.rb`.

It should be possible in the future to implement some other scripting language
using the same Object System and Runtime.

#### The Tokenizer

The tokenizer should return a flat Object<List> of Objects where each Object is either:
- `Object<String>` -- Basic String type Object
- `Object<Number>` -- Basic Number type Object
- `Object<Symbol>` -- Parenthesis and BareWords

The current Tokenizer, written a long time ago, returns a stream-like thing,
that spits out a series of tokens each time it is called. This design has some
issues and is being scrapped. These tokens are of type Token, an object that
refers to slices of the stream. This has many drawbacks and issues.

Instead, the new and current tokenizer returns a complete List of Objects.

There is no reason to have 'streams' which implies there is some value in
partial tokenization. There is no reason to have a temporary Token type object
which points to objects which will be later created during parsing anyway.

#### The Parser

The parser will convert the flat list of objects into a nested list of objects
representing the Lisp parse-tree. 

## Links and References

- [Racket Scheme Forms](https://docs.racket-lang.org/guide/scheme-forms.html)
- [Python Garbage Collector Design](https://github.com/python/cpython/blob/main/InternalDocs/garbage_collector.md)

## TODO

There's a lot that is missing from this implementation, such as but not limited
to Tail-Call-Optimization (TCO).

See [Todo-List](./TODO.txt) for an updated running list.

