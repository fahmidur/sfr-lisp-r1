function Tile(el, opts) {
  var self = this;
  self.el = el;
  self._onresize = null;
  if(opts.onresize) {
    self._onresize = opts.onresize;
  }
}

Tile.prototype.resize = function() {
  var self = this;
  self._onresize();
}
