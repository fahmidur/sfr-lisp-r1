var wasm_bytes = null;
var wasm_memory = null;
var wasm_instance = null;
var term_inited = false;
var wasm_started = false;
var stdin = null;
var fsbuf = null;

const UINT64_MAX = 2n ** 64n - 1n;

// var stringio_state_ptr = null;

var logprefix = 'worker1.';

// A virtual file system of preopened.
var files = {
  '/sandbox': {
    path: '/sandbox',
    type: 'directory',
    fd: 3,
    opened_at: (new Date())
  },
  '/sandbox/main.lsp': {
    path: '/sandbox/main.lsp',
    type: 'file',
    content: '(displayln "hello from main")',
    opened_at: null,
    closed_at: null,
    pos: 0,
  },
};

function vfs_iowrite(path, content) {
  let entry = files[path];
  if(entry) {
    entry.content = String(content);
  }
}

function files_getentry_by_fd(fd) {
  let entry = null;
  for(var path in files) {
    let tmp = files[path];
    if(tmp.fd == fd) {
      entry = tmp;
    }
  }
  return entry;
}

function files_getentry_by_path(path) {
  return files[path] || null;
}

function files_newfd() {
  var maxfd = 3;
  for(var path in files) {
    let entry = files[path];
    if(entry.fd && entry.fd > maxfd) {
      maxfd = entry.fd;
    }
  }
  return maxfd+1;
}

// function VirtFs() {
//   var self = this;
//   self.files = {
//   };
// }

// VirtFs.prototype.file_add = function(path, fd, content) {
//   var self = this;
// };

function ensureEnding(content, ending) {
  if(!content.endsWith(ending)) {
    content += ending;
  }
  return content;
}

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
        let lp2 = logprefix+' fd_close. fd='+fd + '.';
        console.log(lp2, '--- close ---');
        let entry = files_getentry_by_fd(fd);
        if(!fd) {
          console.error(lp2, 'could not find file from fd=', fd);
          return;
        }
        entry.opened_at = null;
        entry.closed_at = (new Date());
        entry.pos = 0;
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
      fd_fdstat_get: function(fd, fdstat_ptr) {
        let lp2 = logprefix+' fd_fdstat_get. fd='+fd+'.';
        console.log(lp2, 'fdstat_ptr=', fdstat_ptr);
        if(fd == 0 || fd == 1) {
          // stdin, stdout
          return;
        }
        var entry = files_getentry_by_fd(fd);
        if(!entry) {
          console.error(lp2, 'could not find entry from fd');
          return;
        }
        var mem = new DataView(wasm_memory_buffer());
        // __wasi_filetype_t fs_filetype;
        // typedef uint8_t __wasi_filetype_t;
        // values = {DT_DIR=4, DT_REG=8}
        if(entry.type == 'directory') {
          mem.setUint8(fdstat_ptr, 4);
        }
        else
        if(entry.type == 'file') {
          mem.setUint8(fdstat_ptr, 8);
        }

        // __wasi_fdflags_t fs_flags;
        // typedef uint16_t __wasi_fdflags_t;
        // #define O_RDONLY (0x04000000)
        // we offset by 8 because alignment is 8 bytes
        mem.setUint16(fdstat_ptr+8, 0x04000000);

        // __wasi_rights_t fs_rights_base;
        // typedef uint64_t __wasi_rights_t;
        // mem.setBigUint64(fdstat_ptr+16, UINT64_MAX);

        //__wasi_rights_t fs_rights_inheriting;
        // typedef uint64_t __wasi_rights_t;
        // mem.setBigUint64(fdstat_ptr+24, UINT64_MAX);
      },
      fd_fdstat_set_flags: function() {
        console.log(logprefix, 'fd_fdstat_set_flags');
      },
      fd_prestat_get: function(fd, prestat_ptr) {
        /**
         * Write to wasm memory a description of 
         * virtual fd that is 'preopened'.
         * 4 bytes -- the type of the file 0=directory
         **/
        let lp2 = logprefix+' fd_prestat_get. fd='+fd+'.';
        console.log(lp2, 'prestat_ptr=', prestat_ptr);
        var entry = files_getentry_by_fd(fd);
        if(!entry) {
          console.log(lp2, 'could not find entry from fd=', fd);
          // this ends the loop in __wasilibc_populate_preopens(void)
          return 8; // WASI_EBADF
        }
        console.log(lp2, 'entry=', entry);
        if(!(entry.opened_at)) {
          console.log(lp2, 'expecting entry to be preopened with opened_at');
          return 8; // WASI_EBADF
        }
        if(entry.type !== 'directory') {
          console.log(lp2, 'expecting entry type to be directory');
          return 8; // WASI_EBADF
        }
        var ret_view = new DataView(wasm_memory_buffer());

        // See definition of '__wasi_prestat_t' in wasi_libc
        // prestat_t is a struct
        // first 1 byte is a uint8 called 'tag'
        // 0 = __WASI_PREOPENTYPE_DIR -- the only option
        ret_view.setUint8(prestat_ptr, 0); 
        // prestat_t is 4 byte aligned.
        // the next field is 
        // union __wasi_prestat_u_t{
        //   struct __wasi_prestat_dir_t{
        //     __wasi_size_t pr_name_len;
        //   }
        // }
        // and __wasi_size_t pr_name is a uint32 of size 4 bytes
        ret_view.setUint32(prestat_ptr+4, entry.path.length, true);
        return 0; // success
      },
      fd_prestat_dir_name: function(fd, path_ptr, path_len) {
        /**
         * Write to path_ptr the encoded bytes for the path 
         * of the virtual directory.
         **/
        let lp2 = logprefix+' fd_prestat_dir_name. fd='+fd+'.';
        console.log(lp2, 'fd=', fd, 'path_ptr=', path_ptr, 'path_len=', path_len);
        var entry = files_getentry_by_fd(fd);
        if(!entry) {
          console.error(lp2, 'could not find find entry from fd=', fd);
          return 8; // WASI_EBADF
        }
        console.log(lp2, 'entry=', entry);
        var path_bytes = (new TextEncoder()).encode(entry.path);
        console.log(lp2, 'path_bytes=', path_bytes);
        if(path_bytes.length > path_len) {
          console.error(lp2, 'path_bytes exceeds allocated path_len');
          return 8; // WASI_EBADF
        }
        var memory_view = new Uint8Array(wasm_memory_buffer(), path_ptr, path_len);
        memory_view.set(path_bytes);

        return 0; // success
      },
      fd_seek: function(fd, offset, whence, ret_ptr) {
        /**
         * __wasi_errno_t __wasi_fd_seek(
         *   __wasi_fd_t fd,
         *   __wasi_filedelta_t offset,
         *   __wasi_whence_t whence,
         *   __wasi_filesize_t *retptr0
         * );
         * typedef uint64_t __wasi_filesize_t;
         **/
        let lp2 = logprefix+' fd_seek.';
        console.log(lp2, 'fd=', fd, 'offset=', offset, 'whence=', whence);
      },
      fd_tell: function(fd, ret_ptr) {
        // __wasi_errno_t __wasi_fd_tell(
        //   __wasi_fd_t fd,
        //   __wasi_filesize_t *retptr0
        // );
        console.log(logprefix, 'fd_tell. fd=', fd);
      },
      // fd_open: function(fd) {
      //   console.log(logprefix, 'fd_open. fd=', fd);
      // },
      fd_read: function(fd, iovs_ptr, iovs_len, ret_ptr) {
        let lp2 = logprefix + ' fd_read. fd='+fd+'.';
        console.log(lp2, 'iovs_ptr=', iovs_ptr, 'iovs_len=', iovs_len, 'ret_ptr=', ret_ptr);
        var stdin_view = null;
        var bytes_read = 0;
        var min_iov_buf_size = 2**32;
        for(let i = 0; i < iovs_len; i++) {
          let offset = i * 8;
          let iov = new Uint32Array(wasm_memory_buffer(), iovs_ptr + offset, 2);
          let iov_ptr = iov[0];
          let iov_size = iov[1];
          if(iov_size < min_iov_buf_size) {
            min_iov_buf_size = iov_size;
          }
        }
        console.log(lp2, 'min_iov_buf_size=', min_iov_buf_size);
        if(fd == 0) {
          var stdin_i32 = new Int32Array(stdin);
          console.log(lp2, 'waiting...');
          Atomics.wait(stdin_i32, 0, 0);
          console.log(lp2, '--- wait complete ---');
          Atomics.store(stdin_i32, 0, 0);
          bytes_read = stdin_i32[1];
          console.log(lp2, 'bytes_read=', bytes_read);
          stdin_view = new Uint8Array(stdin, 8, bytes_read);
          console.log(lp2, 'stdin_view=', stdin_view);
        } else {
          let entry = files_getentry_by_fd(fd);
          if(fsbuf) {
            console.log(lp2, 'donuts. fsbuf=', fsbuf);
            let fsbuf_view = new Uint8Array(fsbuf, 0);
            let fsbuf_ns = new Uint8Array(fsbuf.byteLength);
            for(let i = 0; i < fsbuf_view.byteLength; i++) {
              fsbuf_ns[i] = fsbuf_view[i];
            }
            console.log(lp2, 'donuts. fsbuf_ns=', fsbuf_ns);
            entry.content = (new TextDecoder()).decode(fsbuf_ns);
          }
          console.log(lp2, 'entry.content=', entry.content);
          let remaining = entry.content.length - entry.pos;
          let toread = Math.min(remaining, min_iov_buf_size);
          console.log(lp2, 'toread=', toread);
          if(toread > 0) {
            stdin_view = (new TextEncoder()).encode(entry.content.slice(entry.pos, entry.pos+toread));
            bytes_read = stdin_view.byteLength;
            entry.pos += toread;
            console.log(lp2, 'entry | entry.pos=', entry.pos);
          }
          else {
            stdin_view = new Uint8Array();
            bytes_read = 0;
          }
          console.log(lp2, 'entry | stdin_view=', stdin_view);
          console.log(lp2, 'entry | bytes_read=', bytes_read);
        }
        for(let i = 0; i < iovs_len; i++) {
          let offset = i * 8;
          // iov is 2 32-bit/4byte pointers
          let iov = new Uint32Array(wasm_memory_buffer(), iovs_ptr + offset, 2);
          console.log(lp2, 'iov[', i, ']=', iov);
          let buf_ptr = iov[0];
          let buf_size = iov[1];
          console.log(lp2, 'buf_ptr=', buf_ptr, 'buf_size=', buf_size);
          // console.log(lp2, 'wasm_instance=', wasm_instance);
          let buf_view = new Uint8Array(wasm_memory_buffer(), buf_ptr, buf_size);
          console.log(lp2, 'iov i=', i, ' buf_view=', buf_view, 'buf_size=', buf_size);
          buf_view.set(stdin_view.subarray(0, stdin_view.byteLength));
          console.log(lp2, 'buf_view=', buf_view, 'buf_size=', buf_size);
        }
        console.log(lp2, 'bytes_read=', bytes_read);
        var ret_view = new Uint32Array(wasm_memory_buffer(), ret_ptr, 2);
        ret_view[0] = bytes_read;
        ret_view[1] = 0;
        console.log(lp2, 'ret_view=', ret_view);
        wasm_instance.exports.stdout_flush();
        // if(fd == 0) {
        //   debugger;
        // } else {
        //   debugger;
        // }
        return 0; // 0 = success
      },
      path_open: function(fd, dirflags, path_ptr, path_len, oflags, fs_rights_base, fs_rights_inheriting, fdflags, newfd_ptr) {
        /**
         * Open a virtual file at particular directory.
         * defined in __wasilibc_real.c
         * __imported_wasi_snapshot_preview1_path_open(
         *   (int32_t) fd, 
         *   dirflags, (int32_t) path, (int32_t) path_len, oflags, fs_rights_base, fs_rights_inheriting, 
         *   fdflags, (int32_t) retptr0
         * )
         * Variable names:
         * - 'fd' should really be called 'dirfd' because it refers to a directory file descriptor.
         * - 'path_ptr' and 'path_len' should really be called 'name_ptr' and 'name_len'.
         */
        let lp2 = logprefix+' path_open. fd='+fd+'.';
        console.log(lp2, 'dirflags=', dirflags, 'path_ptr=', path_ptr, 'path_len=', path_len, 'oflags=', oflags, 'fd_rights_base=', fs_rights_base, 'fs_rights_inheriting=', fs_rights_inheriting, 'fdflags=', fdflags, 'newfd_ptr=', newfd_ptr);
        // console.log(lp2, 'newfd_ptr=', newfd_ptr);
        var entry = files_getentry_by_fd(fd);
        if(!entry) {
          console.error(lp2, 'unable to find vfs entry from fd=', fd);
          return 1; // failure
        }
        var nf_name = (new TextDecoder()).decode(new Uint8Array(wasm_memory_buffer(), path_ptr, path_len));
        var nf_path = entry.path + '/' + nf_name;
        console.log(lp2, 'nf_name=', nf_name, 'nf_path=', nf_path);
        var nf_entry = files_getentry_by_path(nf_path);
        if(!nf_entry) {
          console.error(lp2, 'unable to find vfs entry from nf_path=', nf_path);
          return 1;
        }
        nf_entry.fd = files_newfd();
        nf_entry.opened_at = (new Date());
        console.log(lp2, 'nf_entry=', nf_entry);
        var mem = new DataView(wasm_memory_buffer());
        mem.setInt32(newfd_ptr, nf_entry.fd, true);
        return 0; // success
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
      fsbuf = data.fsbuf;
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
      break;
    case 'repl_prompt':
      console.log(logprefix, 'heard repl_prompt');
      wasm_instance.exports.repl_prompt();
      break;
    case 'vfs_iowrite':
      console.log(logprefix, 'heard vfs_iowrite');
      vfs_iowrite(data.path, data.content);
      break;
    default:
  }
};

