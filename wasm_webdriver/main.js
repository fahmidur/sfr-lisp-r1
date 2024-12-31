console.log('--- main.js ---');

var term_kbmode = false;
var term = new Terminal();
term.open(document.getElementById('terminal'));
// term.write('Hello from \x1B[1;3;31mxterm.js\x1B[0m $ ')

term.onKey(function(e) {
  // term.write(e.key);
});


var wasm_memory = new WebAssembly.Memory({
  initial: 2,
  maximum: 1000,
  shared: true,
});

var workers = {};
function worker_add(name, worker) {
  workers[name] = {
    state: {
      inited: false,
    },
    worker: worker,
  }
}

var term_encoder = new TextEncoder();
function maybe_init_term() {
  for(var k in workers) {
    if(workers[k].state.inited !== true) {
      return;
    }
  }
  term.onKey(function(ev) {
    let key_code = ev.key.charCodeAt(0);
    console.log('term. ev=', ev, 'key=', ev.key, 'key_code=', key_code);
    let io_worker = workers['worker2'].worker;
    io_worker.postMessage({
      type: 'stdin',
      data: {
        key_code: key_code
      }
    });
  });
}

var worker1 = new Worker('./worker1.js');
worker1.onmessage = function(ev) {
  var msg = ev.data;
  console.log('worker1. ev=', ev, ' msg=', msg);
  switch(msg.type) {
    case 'term_stdout': 
      term.write(msg.data);
      break;
    case 'inited':
      console.log('heard worker1 inited');
      workers['worker1'].state.inited = true;
      maybe_init_term();
    default:
  }
};
worker_add('worker1', worker1);

var worker2 = new Worker('./worker2.js');
worker2.onmessage = function(ev) {
  var msg = ev.data;
  console.log('worker2. ev=', ev, ' msg=', msg);
  switch(msg.type) {
    case 'term_stdout': 
      term.write(msg.data);
      break;
    case 'inited':
      console.log('heard worker2 inited');
      workers['worker2'].state.inited = true;
      maybe_init_term();
      break;
    default:
  }
};
worker_add('worker2', worker2);

function workers_broadcast(msg) {
  for(var k in workers) {
    let worker = workers[k].worker;
    worker.postMessage(msg);
  }
}

var respPromise = fetch("./build/sfr-lisp-wasm.wasm");
respPromise.then(function(response) {
  console.log('response = ', response);
  response.arrayBuffer().then(function(bytes) {
    console.log('got bytes = ', bytes);
    var wasm_bytes = bytes;
    workers_broadcast({
      type: 'init',
      data: {
        wasm_memory: wasm_memory,
        wasm_bytes: wasm_bytes,
      }
    });
  });
});

