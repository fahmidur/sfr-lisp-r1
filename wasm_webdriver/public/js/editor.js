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
    theme: 'nord'
  });
  return el;
};
