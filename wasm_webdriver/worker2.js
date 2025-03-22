/**
 * The IO Worker.
 * This worker mainly handles IO -- dealing with STDIN/STDOUT, and so on.
 **/

var wasm_bytes = null;
var wasm_memory = null;
var wasm_instance = null;

var stdin = null;
var stdin_pos = 0;
var stdin_arr = [];
var stdin_hist = [[]];
var stdin_hist_idx = 0;

var stringio_state_ptr = null;
var stringio_buf_ptr = null;

var logprefix = 'worker2.';

var wasm_args = ["sfr-lisp-wasm"];

function gmod(n, m) {
  return ((n%m)+m)%m;
}

function arr_clone(arr) {
  return arr.map((x) => x);
}

function arr_equal(a, b) {
  if(a.length != b.length)  {
    return false;
  }
  for(let i = 0; i < a.length; i++) {
    if(a[i] != b[i]) {
      return false;
    }
  }
  return true;
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
      args_get: function(argv_ptr, argv_buf_ptr, ret) {
        var ret_arr = new Uint32Array(wasm_memory_buffer(), ret, 2);
        console.log('args_get. argv_ptr=', argv_ptr, 'argv_buf_ptr = ', argv_buf_ptr);
        // var arg1 = "lisp\0";
        // var arg1_encoded = (new TextEncoder()).encode(arg1);
        // console.log('arg1_encoded = ', arg1_encoded);
        var argv_total_size = wasm_args.reduce((s,e) => (s+e.length+1), 0);
        var argv_arr = new Uint32Array(wasm_memory_buffer(), argv_ptr, wasm_args.length);
        var argv_buf_arr = new Uint8Array(wasm_memory_buffer(), argv_buf_ptr, argv_total_size);
        var pos = 0;
        for(i = 0; i < wasm_args.length; i++) {
          let arg = wasm_args[i] + "\0";
          let arg_encoded = (new TextEncoder()).encode(arg);
          console.log('i = ', i, 'arg = ', arg);
          console.log('i = ', i, 'arg_encoded = ', arg_encoded);
          for(j = 0; j < arg_encoded.length; j++) {
            argv_buf_arr[pos+j] = arg_encoded[j];
          }
          argv_arr[i] = argv_buf_ptr+pos;
          pos += arg_encoded.length;
        }
        console.log('argv_buf_arr = ', argv_buf_arr);
        console.log('argv_arr = ', argv_arr);
        ret_arr[0] = 0; // 
        ret_arr[1] = 0; // errno=0
      },
      args_sizes_get: function(ret_ptr) {
        var ret_arr = new Uint32Array(wasm_memory_buffer(), ret_ptr, 3);
        // the number of args
        ret_arr[0] = wasm_args.length;
        // the total size of the args in bytes
        ret_arr[1] = wasm_args.reduce((s, e) => (s+e.length+1), 0);
        ret_arr[2] = 0 // errno=0
      },
      environ_get: function() {
      },
      environ_sizes_get: function() {
        return 0;
      },
      fd_fdstat_get: function(fd) {
        console.log('fd_fdstat_get. fd=', fd);
      },
      fd_fdstat_set_flags: function() {
      },
      fd_prestat_get: function() {
      },
      fd_prestat_dir_name: function() {
      },
      fd_seek: function() {
        console.log('fd_seek');
      },
      fd_read: function(fd, iovs, iovs_len, ret_ptr) {
        console.log('fd_read. fd=', fd, 'iovs=', iovs);
        const memory = new Uint32Array(wasm_instance.exports.memory.buffer);
      },
      path_open: function() {
      },
      fd_write: function(fd, iovs, iovs_len, ret_ptr) {
        console.log('ret_ptr =', ret_ptr);
        const memory = new Uint32Array(wasm_instance.exports.memory.buffer);
        console.log('*ret_ptr = ', memory[ret_ptr]);
        let nwritten = 0;
        for (let i = 0; i < iovs_len; i++) {
          const offset = i * 8; // = jump over 2 i32 values per iteration
          const iov = new Uint32Array(memory.buffer, iovs + offset, 2);
          // use the iovs to read the data from the memory
          const bytes = new Uint8Array(memory.buffer, iov[0], iov[1]);
          const data = new TextDecoder("utf8").decode(bytes);
          console.log('fd=', fd, '||', data);
          var term_data = data.replaceAll("\n", "\r\n");
          // term.write(term_data);
          postMessage({
            type: 'term_stdout',
            data: term_data
          });
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
      poll_oneoff: function() {
      },
      proc_exit: function(exitcode){
        console.log('proc_exit. exitcode=', exitcode);
        return exitcode;
      }
    }
  }).then(function(out) {
    console.log('out = ', out);
    wasm_instance = out.instance;
    stringio_state_ptr = wasm_instance.exports.stringio_init();
    console.log(logprefix, 'stringio_state_ptr=', stringio_state_ptr);
    stringio_buf_ptr = wasm_instance.exports.stringio_get_buf();
    console.log(logprefix, 'stringio_buf_ptr=', stringio_buf_ptr);
    postMessage({
      type: 'inited',
      data: {
        stringio_state_ptr: stringio_state_ptr,
        stringio_buf_ptr: stringio_buf_ptr,
      }
    });
  });
}

// function isAlphaNumeric(keyCode) {
//   // Check for letters (A-Z)
//   if (keyCode >= 65 && keyCode <= 90) {
//     return true;
//   }

//   // Check for numbers (0-9)
//   if (keyCode >= 48 && keyCode <= 57) {
//     return true;
//   }

//   // Otherwise, not alphanumeric
//   return false;
// }

function term_echo(key) {
  postMessage({
    type: 'term_echo',
    data: {
      key: key
    }
  });
}

function term_backspace() {
  stdin_arr.pop();
  term_echo('\x1b[D\x1b[K');
}

function term_clear() {
  term_echo('\x1b[2J');
}

function term_gohome() {
  term_echo('\x1b[H');
}

function do_form_feed() {
  postMessage({
    type: 'form_feed',
  });
}

function stdin_clear() {
  console.log('stdin_clear. length=', stdin_arr.length);
  while(stdin_arr.length > 0) {
    term_backspace();
  }
}

function stdin_set(new_val) {
  console.log('stdin_set. new_val=', new_val);
  stdin_clear();
  var tmp = null;
  if(new_val instanceof Array) {
    tmp = new_val;
  }
  else {
    tmp = new_val.split('').map((x) => x.charCodeAt(0));
  }
  // now tmp is an array of character codes
  stdin_arr = tmp;
  var keys = tmp.map((x) => String.fromCharCode(x)).join('');
  console.log('stdin_set. keys=', keys);
  term_echo(keys);
}

/**
 * Go up or down in REPL history.
 * @param {Number} delta - A number, usually +1 or -1 indicating the history offset.
 **/
function history_go(delta) {
  if(stdin_hist.length == 0) {
    // there is no history
    console.log(logprefix, 'history_go. ABORT. There is no stdin_hist.');
    return;
  }
  stdin_hist_idx = gmod(stdin_hist_idx+delta, stdin_hist.length);
  console.log(logprefix, 'history_go. stdin_hist=', stdin_hist, 'stdin_hist_idx=', stdin_hist_idx);
  var hist_val = arr_clone(stdin_hist[stdin_hist_idx]);
  console.log(logprefix, 'history_go. hist_val =', hist_val);
  stdin_set(hist_val);
}

function history_push(val) {
  if(stdin_hist.length > 0) {
    let last_item = stdin_hist[stdin_hist.length-1];
    if(arr_equal(last_item, val)) {
      return;
    }
  }
  stdin_hist.push(arr_clone(val));
  console.log(logprefix, 'stdin_hist=', stdin_hist);
}

onmessage = function(ev) {
  var msg = ev.data;
  console.log(logprefix, 'ev = ', ev, 'msg=', msg);
  let data = msg.data;
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
      stdin = data.stdin;
      wasm_memory = data.wasm_memory;
      wasm_bytes = data.wasm_bytes;
      // make_wasm_instance();
      postMessage({
        type: 'inited',
        data: {
        }
      });
      break;
    case 'stdin':
      var stdin_size = new Uint32Array(stdin, 4, 1);
      var stdin_view = new Uint8Array(stdin, 8);
      console.log('data key=', data.key, 'key_code=', data.key_code);
      if(data.key == '\x1B[A') { // arrow up
        console.log(logprefix, '-- arrow up --');
        history_go(-1);
      }
      if(data.key == '\x1B[B') { // arrow up
        console.log(logprefix, '-- arrow down --');
        history_go(+1);
      }
      else
      if(data.key == '\x7f') { //backspace key
        console.log(logprefix, 'stdin. -- backspace --');
        if(stdin_arr.length == 0) {
          return;
        }
        term_backspace();
      }
      else
      if(data.key_code == 12) { // form-feed character
        console.log('--- form-feed ---');
        do_form_feed();
      }
      else
      if(data.key_code === 13) {
        console.log('--- return ---');
        postMessage({
          type: 'term_echo',
          data: {
            key: "\n\r",
          }
        });
        history_push(stdin_arr);
        // make sure we push null character into the array
        stdin_arr.push(10); // LF character
        console.log('stdin_arr=', stdin_arr);
        for(i = 0; i < stdin_arr.length; i++) {
          stdin_view[i] = stdin_arr[i];
        }
        stdin_size[0] = stdin_arr.length;
        stdin_arr = [];
        console.log(logprefix, 'stdin=', stdin);
        var stdin_i32 = new Int32Array(stdin);
        Atomics.store(stdin_i32, 0, 1);
        Atomics.notify(stdin_i32, 0);
      }
      else 
      if(data.key_code >= 32 && data.key_code <= 126) {
        stdin_arr.push(data.key_code);
        console.log('stdin_arr=', stdin_arr);
        postMessage({
          type: 'term_echo',
          data: {
            key: data.key
          }
        });
      }
      break;
    case 'stdin_set':
      console.log(logprefix, 'onmessage. stdin_set. data=', data);
      stdin_set(data.val);
      break;
    default:
  }
};
