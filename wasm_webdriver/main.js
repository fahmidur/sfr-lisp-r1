console.log('--- main.js ---');


var term = new Terminal();
term.open(document.getElementById('terminal'));
// term.write('Hello from \x1B[1;3;31mxterm.js\x1B[0m $ ')

var wasm_memory = new WebAssembly.Memory({
  initial: 2
});

var wasm_bytes = null;
var wasm_instance = null;

function fetch_wasm_bytes() {
  var respPromise = fetch("./build/sfr-lisp-wasm.wasm");
  respPromise.then(function(response) {
    console.log('response = ', response);
    response.arrayBuffer().then(function(bytes) {
      console.log('got bytes = ', bytes);
      wasm_bytes = bytes;
      make_wasm_instance();
    });
  });
}

function make_wasm_instance() {
  WebAssembly.instantiate(wasm_bytes, {
    env: {
      memory: wasm_memory
    },
    wasi_snapshot_preview1: {
      fd_close: function(fd) {
        console.log('fd_close. fd=', fd);
      },
      args_get: function() {
      },
      args_sizes_get: function() {
      },
      environ_get: function() {
      },
      environ_sizes_get: function() {
      },
      fd_fdstat_get: function(fd) {
        console.log('fd_fdstat_get. fd=', fd);
      },
      fd_seek: function() {
        console.log('fd_seek');
      },
      //fd_write: function() {
      //  console.log('fd_write');
      //},
      fd_write: function(fd, iovs, iovs_len, ret_ptr) {
        console.log('ret_ptr =', ret_ptr);
        const memory = new Uint32Array(wb.instance.exports.memory.buffer);
        console.log('*ret_ptr = ', memory[ret_ptr]);
        
        let nwritten = 0;
        for (let i = 0; i < iovs_len; i++) {
            const offset = i * 8; // = jump over 2 i32 values per iteration
            const iov = new Uint32Array(memory.buffer, iovs + offset, 2);
            // use the iovs to read the data from the memory
            const bytes = new Uint8Array(memory.buffer, iov[0], iov[1]);
            const data = new TextDecoder("utf8").decode(bytes);
            console.log('fd=', fd, '||', data);
            nwritten += iov[1];
        }

        console.log('nwritten =', nwritten);
        // Set the nwritten in ret_ptr
        // const bytes_written = new Uint32Array(memory.buffer, ret_ptr, 1);
        // bytes_written[0] = nwritten;
        memory[ret_ptr] = nwritten;
        console.log('fd=', fd, `| bytes written =`, nwritten);
        return nwritten;
      },
      proc_exit: function(){
        console.log('proc_exit');
      }
    }
  }).then(function(out) {
    console.log('out = ', out);
  });
}

fetch_wasm_bytes();

