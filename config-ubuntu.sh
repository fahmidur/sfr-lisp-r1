#!/bin/bash

# We need LLVM clang to build wasm file.
sudo apt install --yes clang

# This provides the LLVM lld toolset which includes
# `wasm-ld` which is needed by clang for wasm output.
sudo apt install --yes lld


