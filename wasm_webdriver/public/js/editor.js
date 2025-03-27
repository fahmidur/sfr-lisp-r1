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
)`
  });
  return el;
};

Editor.prototype.getValue = function() {
  var self = this;
  return self.cm.getValue();
};
