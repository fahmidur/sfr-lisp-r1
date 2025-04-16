console.log('--- main.js ---');

var logprefix = 'main___.';

var btn_term = $('#btn_term');
var btn_code = $('#btn_code');
var btn_vsplit = $('#btn_vsplit');
var btn_hsplit = $('#btn_hsplit');
var btn_run = $('#btn_run');
var sel_samples = $('#sel_samples');
var default_content = `(begin
  (displayln "--- Welcome! ---")
  (define make_account
    (lambda (balance)
      (lambda (amt)
        (begin
          (set! balance (+ balance amt))
          balance
        ))))
  (define account1 (make_account 100))
  (displayln (account1 -20))
  (displayln (account1 -10))
  (displayln (account1 -5))
  (displayln (account1 40))
)`;

$.ajax({
  url: '/samples.json',
  method: 'GET',
}).then(function(res) {
  console.log('samples. res=', res);
  let files = res.files;
  if(!files) {
    console.error('samples. expecting res.files');
    return;
  }
  files.forEach(function(f) {
    sel_samples.append($('<option>').text(f).attr('value', f));
  });
});

var tilewm = new TileWM($('#tilewm-root'));

var term_kbmode = false;

var term_el = null;
var term = new Terminal({
  cursorBlink: true,
});
var fitAddon = new FitAddon.FitAddon();
term.loadAddon(fitAddon);
try {
  var webglAddon = new WebglAddon.WebglAddon();
  term.loadAddon(webglAddon);
} catch(err) {
  console.warn("Term. WebGLAddon failed. err=", err);
}
// var canvasAddon = new CanvasAddon.CanvasAddon();
// term.loadAddon(canvasAddon);

var term_holder = null;
var term_inited = false;

var editor = null;
var editor_el = null;

var term_adjust_timeout = null;

var tile_term = new Tile($('#tile-term'), {
  onupdate: function(tile, el) {
    console.log('tile_term. onupdate. el=', el);
    if(term_el == null) {
      term_el = $('<div></div>').addClass('term_el');
      term_el.appendTo(el);
      term.open(term_el.get(0));
    }
    term_el.outerWidth(el.outerWidth());
    term_el.outerHeight(el.outerHeight());
    fitAddon.fit();
    clearTimeout(term_adjust_timeout);
    term_adjust_timeout = setTimeout(function() {
      term_el.outerWidth(el.outerWidth());
      term_el.outerHeight(el.outerHeight());
      fitAddon.fit();
    }, 500);
  }
});

var tile_code = new Tile($('#tile-code'), {
  onupdate: function(tile, el) {
    console.log('tile_code. onupdate');
    if(editor == null) {
      editor_el = $("<div></div>");
      editor_el.appendTo(el);
      console.log('new Editor(). editor_el=', editor_el);
      editor = new Editor(editor_el);
      editor.setValue(default_content);
    }
    if(tilewm.split == 'vsplit') {
      editor_el.outerHeight(term_el.outerHeight());
    } else {
      editor_el.css('height', '');
    }
  }
});

function filter_pragmas(content) {
  return content.split("\n").
    filter((x) => (!(x.match(/^\s*#/)))).
    join("\n");
}

sel_samples.on('change', function() {
  let val = this.value;
  if(val == 'Default') {
    editor.setValue(default_content);
  }
  $.ajax({
    url: '/sample/'+val
  }).then(function(resp) {
    console.log('got-sample. resp=', resp);
    let content = filter_pragmas(resp);
    console.log('content=', content);
    if(editor) {
      editor.setValue(content);
    }
  });
});

tilewm.addTile('term', tile_term);
tilewm.addTile('code', tile_code);

tilewm.update();
$(window).on('resize', function() {
  tilewm.update();
});

function update_btns() {
  btn_vsplit.hide();
  btn_hsplit.hide();
  if(tile_code.visible && tile_term.visible) {
    if(tilewm.split == 'vsplit') {
      btn_hsplit.show();
    }
    else
    if(tilewm.split == 'hsplit') {
      btn_vsplit.show();
    }
  }
}
update_btns();
btn_vsplit.on('click', function() {
  console.log('vsplit click');
  tilewm.setSplit('vsplit');
  update_btns();
});
btn_hsplit.on('click', function() {
  console.log('hsplit click');
  tilewm.setSplit('hsplit');
  update_btns();
});
btn_run.on('click', function() {
  console.log('run click');
  var val = editor.getValue();
  console.log('val=', val);
  lisp_load_content(val);
});

function update_checkbox_icon(parent_el, checked) {
  let icon_el = parent_el.find('i');
  icon_el.removeClass('fa-check-square fa-square');
  if(checked) {
    icon_el.addClass('fa-check-square');
  } else {
    icon_el.addClass('fa-square');
  }
}

btn_term.on('click', function() {
  tile_term.toggleVisible();
  tilewm.update();
  update_checkbox_icon($(this), tile_term.visible);
  update_btns();
});

btn_code.on('click', function() {
  tile_code.toggleVisible();
  tilewm.update();
  update_checkbox_icon($(this), tile_code.visible);
  if(tile_code.visible) {
    btn_run.show();
  } else {
    btn_run.hide();
  }
  update_btns();
});

var term_encoder = new TextEncoder();

var stdin = new SharedArrayBuffer(1024);
var fsbuf = new SharedArrayBuffer(1024);

var wasm_memory = new WebAssembly.Memory({
  initial: 2,
  maximum: 9000,
  shared: true,
});

var wasm_args = ["sfr-lisp-wasm"];

var workers = {};
function worker_add(name, worker) {
  workers[name] = {
    state: {
      inited: false,
    },
    worker: worker,
  }
}

function workers_all_inited() {
  for(var k in workers) {
    if(workers[k].state.inited !== true) {
      return false;
    }
  }
  return true;
}

function maybe_init_term() {
  if(!workers_all_inited()) {
    return;
  }
  if(term_inited) {
    return;
  }
  term_inited = true;
  term.onData(function(str) {
    console.log(logprefix, 'term. data. str=', str);
    str.split('').forEach(function(key) {
      let key_code = key.charCodeAt(0);
      console.log(logprefix, 'term. key=', key, 'key_code=', key_code);
      let io_worker = workers['worker2'].worker;
      io_worker.postMessage({
        type: 'stdin',
        data: {
          key: key,
          key_code: key_code
        }
      });
    });

  });

  workers['worker1'].worker.postMessage({
    type: 'term_inited',
    data: {},
  });
}

var worker1 = new Worker('./js/worker1.js');
worker1.onmessage = function(ev) {
  var msg = ev.data;
  console.log(logprefix, 'heard worker1. ev=', ev, ' msg=', msg);
  switch(msg.type) {
    case 'term_stdout': 
      term.write(msg.data);
      break;
    case 'inited':
      console.log(logprefix, 'heard worker1 inited');
      workers['worker1'].state.inited = true;
      maybe_init_term();
      break;
    case 'term_echo':
      term.write(msg.data.key);
      break;
    default:
  }
};
worker_add('worker1', worker1);
function repl_prompt() {
  console.log(logprefix, 'sending repl_prompt to worker1');
  worker1.postMessage({
    type: 'repl_prompt',
    data: {},
  });
}

var worker2 = new Worker('./js/worker2.js');
worker2.onmessage = function(ev) {
  var msg = ev.data;
  console.log(logprefix, 'heard worker2. ev=', ev, ' msg=', msg);
  switch(msg.type) {
    case 'term_stdout': 
      term.write(msg.data);
      break;
    case 'inited':
      console.log(logprefix, 'heard worker2 inited');
      workers['worker2'].state.inited = true;
      // var stringio_state_ptr = msg.data.stringio_state_ptr;
      // console.log(logprefix, 'from worker2 stringio_state_ptr=', stringio_state_ptr);
      // var stringio_buf_ptr = msg.data.stringio_buf_ptr;
      // console.log(logprefix, 'stringio_buf_ptr=', stringio_buf_ptr);
      maybe_init_term();
      break;
    case 'term_echo':
      term.write(msg.data.key);
      break;
    case 'repl_prompt':
      console.log(logprefix, 'heard worker2 repl_prompt msg=', msg);
      repl_prompt();
      break;
    case 'form_feed':
      console.log(logprefix, 'heard form feed');
      let cursorY = term.buffer.active.cursorY;
      let cursorX = term.buffer.active.cursorX;
      let linecount = Math.max(0, cursorY);
      console.log(logprefix, 'linecount=', linecount);
      for(let i = 0; i < linecount; i++) {
        term.write('\x1b[H\x1b[M');
        for(let j = 0; j < linecount; j++) {
          term.write('\x1b[B');
        }
      }
      for(let i = 0; i < linecount; i++) {
        term.write('\x1b[A');
      }
      term.write('\x1b[H');
      for(let i = 0; i < cursorX; i++) {
        term.write('\x1b[C');
      }
    default:
  }
};
worker_add('worker2', worker2);

function stdin_set(val) {
  worker2.postMessage({
    type: 'stdin_set',
    data: {
      val: val
    }
  });
}

function stdin_return() {
  worker2.postMessage({
    type: 'stdin',
    data: {
      key_code: 13,
    }
  });
}

function stdin_setret(val) {
  stdin_set(val);
  stdin_return();
}

function lisp_load_content(content) {
  console.log(logprefix, "sending content=", content);
  content_arr = (new TextEncoder()).encode(content);
  // console.log(logprefix, 'content_arr=', content_arr);
  let fsbuf2 = new Uint8Array(fsbuf, 0);
  for(let i = 0; i < fsbuf2.length; i++) {
    if(i >= 0 && i < content_arr.length) {
      fsbuf2[i] = content_arr[i];
    } else {
      fsbuf2[i] = 0;
    }
  }
  // console.log(logprefix, 'fsbuf2=', fsbuf2);
  // console.log(logprefix, 'fsbuf_=', fsbuf);
  var path = "/sandbox/main.lsp";
  stdin_setret(`(load "${path}")`);
}

function workers_broadcast(msg) {
  for(var k in workers) {
    let worker = workers[k].worker;
    worker.postMessage(msg);
  }
}


var respPromise = fetch("./build/sfr-lisp-wasm.wasm");
respPromise.then(function(response) {
  console.log(logprefix, 'response = ', response);
  response.arrayBuffer().then(function(bytes) {
    console.log(logprefix, 'got bytes = ', bytes);
    var wasm_bytes = bytes;
    workers_broadcast({
      type: 'init',
      data: {
        stdin: stdin,
        fsbuf: fsbuf,
        wasm_args: wasm_args,
        wasm_memory: wasm_memory,
        wasm_bytes: wasm_bytes,
      }
    });
  });
});

