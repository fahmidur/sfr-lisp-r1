var wasm_bytes = null;
var wasm_memory = null;
var wasm_instance = null;
var term_inited = false;
var wasm_started = false;
var stdin = null;

// var stringio_state_ptr = null;

var logprefix = 'worker1.';


// function fetch_wasm_bytes() {
//   var respPromise = fetch("./build/sfr-lisp-wasm.wasm");
//   respPromise.then(function(response) {
//     console.log('response = ', response);
//     response.arrayBuffer().then(function(bytes) {
//       console.log('got bytes = ', bytes);
//       wasm_bytes = bytes;
//       make_wasm_instance();
//     });
//   });
// }

function wasm_memory_buffer() {
  return wasm_instance.exports.memory.buffer;
  // if(wasm_memory == null) {
  //   throw 'wasm_memory is null';
  // }
  // if(!(wasm_memory.buffer instanceof SharedArrayBuffer)) {
  //   throw 'wasm_memory.buffer is NOT instance of SharedArrayBuffer';
  // }
  // return wasm_memory.buffer;
}


function make_wasm_instance() {
  console.log(logprefix, 'make_wasm_instance. wasm_memory.buffer=', wasm_memory.buffer);
  if(!(wasm_memory.buffer instanceof SharedArrayBuffer)) {
    throw 'Expecting wasm_memory.buffer instanceof SharedArrayBuffer';
  }
  WebAssembly.instantiate(wasm_bytes, {
    env: {
      memory: wasm_memory
    },
    wasi_snapshot_preview1: {
      fd_close: function(fd) {
        console.log('fd_close. fd=', fd);
      },
      args_get: function(argv_ptr, argv_buf_ptr, ret_ptr) {
        var ret_arr = new Uint32Array(wasm_memory_buffer(), ret_ptr, 2);
        console.log(logprefix, 'args_get. argv_ptr=', argv_ptr, 'argv_buf_ptr = ', argv_buf_ptr);
        var argv_total_size = wasm_args.reduce((s,e) => (s+e.length+1), 0);
        var argv_arr = new Uint32Array(wasm_memory_buffer(), argv_ptr, wasm_args.length);
        var argv_buf_arr = new Uint8Array(wasm_memory_buffer(), argv_buf_ptr, argv_total_size);
        var pos = 0;
        for(i = 0; i < wasm_args.length; i++) {
          let arg = wasm_args[i] + "\0";
          let arg_encoded = (new TextEncoder()).encode(arg);
          console.log(logprefix, 'i = ', i, 'arg = ', arg);
          console.log(logprefix, 'i = ', i, 'arg_encoded = ', arg_encoded);
          for(j = 0; j < arg_encoded.length; j++) {
            argv_buf_arr[pos+j] = arg_encoded[j];
          }
          argv_arr[i] = argv_buf_ptr+pos;
          pos += arg_encoded.length;
        }
        console.log(logprefix, 'argv_buf_arr = ', argv_buf_arr);
        console.log(logprefix, 'argv_arr = ', argv_arr);
        ret_arr[0] = 1;
        ret_arr[1] = 0; // errno=0
      },
      args_sizes_get: function(ret_ptr) {
        var ret_arr = new Uint32Array(wasm_memory_buffer(), ret_ptr, 3);
        // the number of args
        ret_arr[0] = wasm_args.length;
        // the total size of the args in bytes
        ret_arr[1] = wasm_args.reduce((s, e) => (s+e.length+1), 0);
        ret_arr[2] = 0 // errno=0
        console.log(logprefix, 'arg_sizes_get. ret_arr=', ret_arr);
      },
      environ_get: function() {
        console.log(logprefix, environ_get);
      },
      environ_sizes_get: function() {
        console.log(logprefix, 'environ_sizes_get');
        return 0;
      },
      fd_fdstat_get: function(fd) {
        console.log(logprefix, 'fd_fdstat_get. fd=', fd);
      },
      fd_fdstat_set_flags: function() {
        console.log(logprefix, 'fd_fdstat_set_flags');
      },
      fd_prestat_get: function() {
        console.log(logprefix, 'fd_prestat_get');
      },
      fd_prestat_dir_name: function() {
        console.log(logprefix, 'fd_prestat_dir_name');
      },
      fd_seek: function() {
        console.log(logprefix, 'fd_seek');
      },
      fd_read: function(fd, iovs_ptr, iovs_len, ret_ptr) {
        console.log(logprefix, 'fd_read. fd=', fd, 'iovs_ptr=', iovs_ptr, 'iovs_len=', iovs_len, 'ret_ptr=', ret_ptr);
        if(fd !== 0) {
          throw 'only reading on STDIN fd=0 is currently supported';
        }
        var stdin_i32 = new Int32Array(stdin);
        console.log(logprefix, 'fd_read. waiting...');
        Atomics.wait(stdin_i32, 0, 0);
        console.log(logprefix, 'fd_read. --- wait complete ---');
        Atomics.store(stdin_i32, 0, 0);
        // // output size = 8 octets (64 bits), 2 (32bit, 4byte) uint values
        // var ret_view = new Uint32Array(wasm_memory.buffer, ret_ptr, 2);
        // the number of byets read
        var bytes_read = stdin_i32[1];
        console.log(logprefix, 'fd_read. bytes_read=', bytes_read);
        var stdin_view = new Uint8Array(stdin, 8, bytes_read);
        // for(i = 0; i < bytes_read; i++) {
        //   let ch = stdin_view[i];
        //   console.log(logprefix, 'ch=', String.fromCharCode(ch));
        // }
        var memory = new Uint32Array(wasm_memory_buffer());
        for(let i = 0; i < iovs_len; i++) {
          let offset = i * 8;
          let iov = new Uint32Array(memory.buffer, iovs_ptr + offset, 2);
          console.log('iov[', i, ']=', iov);
          let buf_ptr = iov[0];
          let buf_size = iov[1];
          console.log('buf_ptr=', buf_ptr);
          console.log('wasm_instance=', wasm_instance);
          let buf_view = new Uint8Array(memory.buffer, buf_ptr, buf_size);
          buf_view.set(stdin_view.subarray(0, stdin_view.byteLength));
          console.log('buf_view=', buf_view);
        }
        console.log('bytes_read=', bytes_read);
        // var ret_view = new Int32Array(wasm_memory_buffer(), ret_ptr, 2);
        // ret_view[0] = bytes_read;
        var ret_view = new DataView(wasm_memory_buffer());
        ret_view.setUint32(ret_ptr, bytes_read, false);
        return 0;
      },
      path_open: function() {
      },
      fd_write: function(fd, iovs, iovs_len, ret_ptr) {
        console.log(logprefix, 'fd_write. fd=', fd, 'iovs=', iovs, 'ret_ptr =', ret_ptr);
        const memory = new Uint32Array(wasm_instance.exports.memory.buffer);
        console.log(logprefix, '*ret_ptr = ', memory[ret_ptr]);
        let nwritten = 0;
        for (let i = 0; i < iovs_len; i++) {
          const offset = i * 8; // = jump over 2 i32 values per iteration
          const iov = new Uint32Array(memory.buffer, iovs + offset, 2);
          // use the iovs to read the data from the memory
          const bytes = new Uint8Array(memory.buffer, iov[0], iov[1]);
          const data = new TextDecoder("utf8").decode(bytes);
          console.log(logprefix, 'fd=', fd, '||', data);
          var term_data = data.replaceAll("\n", "\r\n");
          // term.write(term_data);
          postMessage({
            type: 'term_stdout',
            data: term_data
          });
          nwritten += iov[1];
        }
        console.log(logprefix, 'nwritten =', nwritten);
        // Set the nwritten in ret_ptr
        // const bytes_written = new Uint32Array(memory.buffer, ret_ptr, 1);
        // bytes_written[0] = nwritten;
        memory[ret_ptr] = nwritten;
        console.log(logprefix, 'fd=', fd, '| bytes written =', nwritten);
        return nwritten;
      },
      poll_oneoff: function() {
      },
      proc_exit: function(exitcode){
        console.log(logprefix, 'proc_exit. exitcode=', exitcode);
        return exitcode;
      }
    }
  }).then(function(out) {
    console.log('out = ', out);
    wasm_instance = out.instance;
    postMessage({
      type: 'inited'
    });
  });
}

function maybe_wasm_start() {
  // if(stringio_state_ptr == null) {
  //   throw 'Expecting non-null stringio_state_ptr';
  // }
  if(!term_inited) {
    return;
  }
  if(wasm_started) {
    return;
  }
  wasm_started = true;
  console.log(logprefix, '=============================== wasm_start()');
  wasm_instance.exports._start();
  console.log(logprefix, '=============================== wasm_start() ... DONE');
}

onmessage = function(ev) {
  var msg = ev.data;
  console.log(logprefix, 'ev = ', ev, 'msg=', msg);
  var data = msg.data;
  switch(msg.type) {
    case 'init':
      if(!data.wasm_memory) {
        console.error(logprefix, 'Expecting wasm_memory in msg.data');
        return;
      }
      if(!data.wasm_bytes) {
        console.error(logprefix, 'Expecting wasm_bytes in msg.data');
        return;
      }

      console.log(logprefix, 'got wasm_memory=', data.wasm_memory, 'buffer=', data.wasm_memory.buffer);
      console.log(logprefix, 'got stdin=', data.stdin);
      stdin = data.stdin;
      wasm_args = data.wasm_args;
      wasm_memory = data.wasm_memory;
      wasm_bytes = data.wasm_bytes;
      make_wasm_instance();
      break;
    case 'term_inited':
      // console.log(logprefix, 'got stringio_state_ptr=', data.stringio_state_ptr)
      // stringio_state_ptr = data.stringio_state_ptr;
      // console.log(logprefix, 'got stringio_buf_ptr=', data.stringio_buf_ptr);
      // wasm_instance.exports.stringio_set(data.stringio_buf_ptr, 4);
      // var my_stringio_buf_ptr = wasm_instance.exports.stringio_get_buf();
      // console.log(logprefix, 'my_stringio_buf_ptr=', my_stringio_buf_ptr);
      term_inited = true;
      maybe_wasm_start();
    default:
  }
};

