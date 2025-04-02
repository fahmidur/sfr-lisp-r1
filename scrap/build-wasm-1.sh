#!/bin/bash

##
# Only here as a reference/notes. 
# Use `rake build` -- 
# it will try to build wasm target if possible.
## 

/usr/lib/llvm-18/bin/clang \
  -O3 \
  -target wasm32-wasi \
  --sysroot=/home/syed/xrepos/wasi-libc/sysroot \
  -nodefaultlibs \
  -lc \
  -o ./build/sfr-lisp.wasm \
  sfr-lisp.c \
  sub/linenoise/linenoise.c \
  build/Util.o \
  build/Object.o \
  build/Symbol.o \
  build/String.o \
  build/Number.o \
  build/List.o \
  build/Hash.o \
  build/Environment.o \
  build/Function.o \
  build/Runtime.o \
  build/Lisp.o \
  build/Error.o

