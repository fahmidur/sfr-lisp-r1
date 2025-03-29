const express = require('express');
const redis = require('redis');
const app = express();
const port = 3000;

var redis_client = redis.createClient({
  legacyMode: true,
});

app.get('/.well-known/acme-challenge/:token', function(req, res) {
  var log_prefix = 'acme.';
  var rkey = 'ACME:'+req.params.token;
  console.log(log_prefix, 'redis.get('+rkey+') = ... ');
  redis_client.get(rkey, function(err, token) {
    if(err) {
      res.end('ERROR');
      return;
    }
    console.log(log_prefix, 'redis.get('+rkey+') =', token);
    res.end(token||'NULL'); 
  });
});

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
