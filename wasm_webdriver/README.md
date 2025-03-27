# README

## Running the WASM/WASI Build in the Browser

This directory provides some HTML, JavaScript, and CSS for running the WASM/WASI build of the toy Lisp interpreter in a browser. It defines a `wasi_snapshot_preview1` interface to interact with the WASM bytecode, which is compiled from [`../sfr-lisp-wasm.c`](../sfr-lisp-wasm.c).

The main goal here is to learn to interface C code compiled to WASM with JavaScript running in the browser via WASI. The key breakthrough in getting these calls working was reading through the wasi-libc source code, which provided detailed insights into the struct layouts and function implementations required for WASM-WASI interoperability.

## Reference Links

- [WASI Preview1 Documentation](https://github.com/WebAssembly/WASI/blob/main/legacy/preview1/docs.md)
- [wasi-libc GitHub Repository](https://github.com/WebAssembly/wasi-libc.git)
