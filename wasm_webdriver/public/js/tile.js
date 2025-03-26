function TileWM(el) {
  var self = this;
  self.el = $(el);
  self.el.addClass('TileWM');
  self.split = 'vsplit';
  self.tiles = {};
  console.log('TileWM. self=', self);
}

TileWM.prototype.setSplit = function(sname) {
  var self = this;
  self.el.removeClass('vsplit hsplit');
  self.split = sname;
  self.el.addClass(self.split);
  self.update();
};

TileWM.prototype.addTile = function(name, tile) {
  var self = this;
  self.tiles[name] = tile;
};

TileWM.prototype.init = function() {
  var self = this;
};

TileWM.prototype.setVisible = function(name, x) {
  var self = this;
  let tile = self.tiles[name];
  if(!tile) {
    console.error('setVisible. no such tile name=', name);
    return;
  }
  tile.setVisible(x);
  self.update();
};

TileWM.prototype.toggleVisible = function(name) {
  var self = this;
  let tile = self.tiles[name];
  if(!tile) {
    console.error('setVisible. no such tile name=', name);
    return;
  }
  tile.toggleVisible();
  self.update();
};

TileWM.prototype.update = function() {
  var self = this;
  console.log('TileWM. --- update ---');
  var vcount = 0;
  let maxsize = 100;
  for(var k in self.tiles) {
    let tile = self.tiles[k];
    if(tile.visible) {
      vcount++;
    }
  }
  maxsize = 100 / vcount;
  for(var k in self.tiles) {
    let tile = self.tiles[k];
    if(self.split == 'vsplit') {
      tile.el.css('max-height', '');
      tile.el.css('max-width', ''+maxsize+'%');
    } 
    else 
    if(self.split == 'hsplit') {
      tile.el.css('max-width', '');
      tile.el.css('max-height', ''+maxsize+'%');
    }
  }

  //--- now update each tile
  for(var k in self.tiles) {
    let tile = self.tiles[k];
    tile.update();
  }
};


/**
 * Represents a Tiling Window
 **/
function Tile(el, opts) {
  var self = this;
  self.el = $(el);
  self.visible = true;
  self._onupdate = null;
  if(opts.onupdate) {
    self._onupdate = opts.onupdate;
  }
  self.el.addClass('Tile');
}

Tile.prototype.update = function() {
  var self = this;
  if(!self.visible) {
    return;
  }
  if(typeof self._onupdate === 'function') {
    self._onupdate(self, self.el);
  }
}

Tile.prototype.setVisible = function(x) {
  var self = this;
  self.visible = !!x;
  if(self.visible) {
    self.el.show();
  } else {
    self.el.hide();
  }
};

Tile.prototype.toggleVisible = function() {
  var self = this;
  self.setVisible(!self.visible);
};
