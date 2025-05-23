# SFR-Lisp

A simple toy lisp-1 interpreter written in C.

This is not designed to be used for anything important, it is simply a learning
exercise. It is really a test bed to play with ideas around the design of C
object systems, memory management, macros, and etc. Writing a Lisp interpreter
is one of those programming exercises that touches many things, which makes it
a great exercise. I chose C because it is about as low-level as one can get
before assembler, and the most popular implementations of other interpreted
languages (Perl, Python, Ruby, Raku) are all written in C. 

To misquote one of my favorite physicists:

> What I cannot create [in C], I do not understand.
> -- Richard P. Feynman

There are definitely better ways to do it than what you see here, efficiency is
not the goal for this implementation. This implementation tries to be as
self-contained a possible. There are no parser generators, memory-management
libraries, or complex build systems. Building is done by Ruby via a simple
readable Rakefile. There is a prototype implementation written in Ruby that I
wrote first. There is a test suite, which tests the correctness of the output
against [Racket](https://racket-lang.org/). There are memory leak tests using Valgrind, called by the test
suite.

The only third-party library used here is Linenoise, a tiny alternative to
Readline written by Antirez. The reason for this is because keyboard handling,
terminal specific escape codes for REPLs, and etc is somewhat outside of the
scope of what I intend on building/learning here. The functionality that
Linenoise provides is notoriously hard to get right across different systems.
But one day, I may switch this out as well.

## Building

### Requirements:

* GCC or Clang
* Valgrind >= v3.20
* [Racket](https://racket-lang.org/) -- This implementation is tested against
  Racket to confirm it is roughly working the same way. Racket is a far more
  mature lisp, and this is a tiny learning project.
* Ruby and Rake -- Used for building and running tests.
* [Linenoise](https://github.com/antirez/linenoise) -- A small self-contained alternative to Readline by Antirez. 
  Vendored as a Git submodule in this repo under ./sub/linenoise). Used mainly
  for the REPL.

### Build Instructions

First clone this repository with the `--recursive` flag.
This will ensure that the submodule for `./sub/linenoise` is present.

With all of the above requirements met and available
run the following command

```
$ rake
```

This will populate the build directory `./build`.
You will a number of built executables, most of which
are test programs.

The executable for the interpreter will be located at
`./build/sfr-lisp`.

### Testing

To run all tests, simply run:

```
$ rake test
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

Symbols are treated differently from normal Objects in that they are
are not garbage collected until the very end. Like an older version of Ruby,
Symbols only ever grow in quantitity. It's not great. But the idea here 
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

The Error class helps manage errors that occur throughout the 
lifetime of the C Runtime.
The class itself is a mainly String-like Object containing an Error message.
Object of type Error are given special handling in methods in the Object class.

Sometimes Errors can happen in functions that cannot return an Error object or cannot 
return an error signal in the return type.
For this purpose there is ErrorSystem defined alongside the Error class. 
Inspired in part by libc's `errno.h`, it uses a global Error instance which can be set by any other class.

At the moment the global Error state is not thread safe, and threads are not yet a consideration in this toy
implementation.

#### Object Class

The Object class is a wrapper around all of the other classes, with a `Symbol*`
type, and a `void* impl` pointer which points to the underlying implementation
class.
The Object class is provides reference-counted object collection by holding a reference count 
defined in the field `rc`.
All objects have a reference count, they are initialized either with reference count 1 or 0.
An object with reference count 1 is one that is immediately assigned to a `Object*` variable.
An object with reference count 0 only makes sense if the object is being returned and it 
is not known whether it has a referencing variable or not.

The reference count field is meant to be incremeneted or decremented by a set of methods 
like `Object_rc_incr`, `Object_rc_decr`, and `Object_assign`.

When the reference count of a non-returning object reaches zero, the object is destroyed.
I will describe what that means below.

In some cases, such as when we pop an object from a List, or when a factory function creates a new 
object, we do not want the object to be garbage collected immediately.
For these cases the, the Object class maintains a `returning` field, and this field is set to `1`.
This prevents the Garbage collector from destroying the Object immediately.

But wouldn't this result in an accumilation of non-destructable garbage objects?
For this reason, the following scheme has been constructed for passing Objects between
function-call barriers. 
Inspired in part by object-ownership principles, every function call returning
an object must be accepted or reject by using the functions `Object_accept(Object*)` or 
`Object_reject(Object*)` (respectively).
When an Object is accepted, its reference count is incremented and its returning state is reset to zero.
When an Object is rejected, if it has a zero reference count, it is destroyed.

#### List Class

The List implementation class is a simple doubly linked list of ListNodes which
hold a reference to an Object.
The List class can and does in many cases hold references to other lists.

It is possible to Lists with circular references back to itself or to another container
class which points back to the original list.

Standard reference counting cannot destroy circular lists. 
But such circular lists can be destroyed by the GC scan of all objects.
Here the mechanism, of internal reference elimination, is fundamentally the same
as that of the the CPython garbage collector.

#### Hash Class

The Hash class implements a Hashmap, where keys are stored in a buckets based on 
the Hash value of the key. 
At the moment, keys must be `Object<String>` or `Object<Symbol>`.
To ensure consistency of the Hash, all keys are duplicated when added to the Hash,
values are not. Value references can therefore be mutated, but key values cannot.

As a container class, hashes can also have circular references.
Circular hashes can only be destroyed by the garbage collector during a call
to `Object_system_gc()`.

### The C Garbage Collector

Outside of the usual reference counting garbage collection that occurs
during the lifetime of the interpreter. 
There is also a function `Object_system_gc()` which scans all objects to determine
which of these objects are no longer reachable.

To determine reachability, all Object types must be able to enumerate all objects that
they reference.
First this gc methood, copies the current reference count to temporary field called
`rc_gc`, which represents the reference count for this GC cycle.
It then enumerates all objects to subtract references to the referred object. 
Any object which now has a `rc_gc` value of 0, is marked as tentatively unreachable,
because the totality of their reference count is equal to known objects.
This indicates that there are no stack variables which refers to these objects.
However, for each of these tentaively unreachabel objects, there may be a reachable object 
still capable of reaching this one.
The GC now scans all objects again to ummark those objects that can be reached by a reachable
object.
Now all remaining objects marked as unreachable, are truly unreachable, and those objects are 
destroyed.

### The Lisp Implementation

The Lisp implementation is mainly defined in `lisp.c` with the driver
program `sfr-lisp.c`.
These files are consumers of the C Object System described above.
These two files are essentially 1-1 in functionality to the prototype implementation
`sfr-lisp.rb`.

It should be possible in the future to implement some other scripting language
using the same Object System and Runtime.

#### The Tokenizer

The tokenizer should return a flat `Object<List>` of Objects where each Object is either:
- `Object<Symbol>` -- Parenthesis and BareWords
- `Object<String>` -- Basic String type Object
- `Object<Number>` -- Basic Number type Object

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

## WASM/WASI Build Configuration

The WASM-WASI build is slightly more complicated than the regular build for your host architecture.
To get a better understanding of what/how WASM builds work, I am intentionally using the minimal set of 
tools to get a functioning wasm file to use with my web-driver.

Requirements:
- LLVM Clang 18
- [wasi-libc](https://github.com/WebAssembly/wasi-libc)
- [libclang\_rt.builtins-wasm32-wasi] from (https://github.com/WebAssembly/wasi-sdk/releases/tag/wasi-sdk-25)

First, we need to install LLVM Clang, because that is what we will use to cross-compile to the web-assembly target.
If you are on Debian/Ubuntu, you can install this with `apt install clang`

Next, we will need a linker that can link for the wasm target, that is provided by a the LLVM lld project, 
which includes a tool called [wasm-lld](https://lld.llvm.org/WebAssembly.html)
If you are on Debian/Ubuntu, you can install this with `apt install lld`.

Next, we need to download and build wasi-libc, this provides a libc and related libraries for WASI. 
Pull this from Github to some location, and build it. 
The result will be a sysroot folder for the wasm32-wasi target. 
This sysroot must be used as the sysroot parameter in ./RakefileConfig.yaml.

At this point, the WASM/WASI build will still fail.

Next, you need the following library file from the [wasi-sdk project](https://github.com/WebAssembly/wasi-sdk) -- `libclang_rt.builtins-wasm32.a`

Go to the Releases page for the latest Release for
[wasi-sdk](https://github.com/WebAssembly/wasi-sdk/releases/tag/wasi-sdk-25)
and download the zipped file [here](https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-25/libclang_rt.builtins-wasm32-wasi-25.0.tar.gz).
Unzip this file and place it into your sysroot/lib/wasm32-wasi directory.

Now, at last the wasm build should succeed.


