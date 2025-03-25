function Editor(el) {
  var self = this;
  self.els = {};
  self.el = self.elmake(el);
}

Editor.prototype.elmake = function(el) {
  var self = this;
  if(!el) {
    el = $('<div></div>');
  }
  var cmholder = $('<div></div>').addClass('cmholder');
  el.append(cmholder);
  return el;
}
