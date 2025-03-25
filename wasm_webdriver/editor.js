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
  el.addClass('Editor');
  var cmholder = $('<div></div>').addClass('cmholder');
  el.append(cmholder);
  self.els.cmholder = cmholder;
  return el;
}
