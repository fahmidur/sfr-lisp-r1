function Editor(el) {
  var self = this;
  self.els = {};
  self.cm = null;
  self.el = self.elmake(el);
  console.log('Editor. self=', self);
}

Editor.prototype.elmake = function(el) {
  var self = this;
  if(!el) {
    el = $('<div></div>');
  }
  el.addClass('Editor');
  var cmholder = $('<div></div>').addClass('cmholder');
  el.append(cmholder);
  self.els.cmholder = cmholder;
  self.cm = new CodeMirror(cmholder.get(0), {
    mode: 'scheme',
    lineNumbers: true,
    viewportMargin: +Infinity,
    theme: 'nord',
    value: `(begin
  (define make_account (lambda (balance) (lambda (amt) (begin (set! balance (+ balance amt)) balance ))))
  (define acc (make_account 100))
  (displayln (acc -20))
  (displayln (acc -10))
  (displayln (acc -5))
  (displayln (acc 40))
)`
  });
  return el;
};

Editor.prototype.getValue = function() {
  var self = this;
  return self.cm.getValue();
};
