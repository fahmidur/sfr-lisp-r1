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

var worker1 = new Worker('./worker1.js');

worker1.onmessage = function(ev) {
  var msg = ev.data;
  console.log('worker1. ev=', ev, ' msg=', msg);
  switch(msg.type) {
    case 'term_stdout': 
      term.write(msg.data);
      break;
    default:
  }
};

worker1.postMessage({
  type: 'init',
  data: {
    wasm_memory: wasm_memory
  }
});
