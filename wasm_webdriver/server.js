const express = require('express');
const app = express();
const port = 3000;

app.use(function(req, res, next) {
  res.set('Cross-Origin-Opener-Policy', 'same-origin');
  res.set('Cross-Origin-Resource-Policy', 'same-origin');
  res.set('Cross-Origin-Embedder-Policy', 'require-corp');
  next();
});

app.use(express.static('./public'));

app.listen(port, function() {
  console.log("Server running at\nhttp://localhost:"+port);
});
