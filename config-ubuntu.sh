#!/bin/bash

# We need LLVM clang to build wasm file.
sudo apt install --yes clang

# This provides the LLVM lld toolset which includes
# `wasm-ld` which is needed by clang for wasm output.
sudo apt install --yes lld

# We need Valgrind for rake test memory tests
sudo apt install --yes valgrind

# We need Racket to test against
sudo apt install --yes racket

