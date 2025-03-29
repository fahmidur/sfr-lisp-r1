import express from 'express';
import redis from 'redis';

const app = express();
const port = 3000;

var redis_client = redis.createClient({});
await redis_client.connect();

app.get('/.well-known/acme-challenge/:token', async function(req, res) {
  var log_prefix = 'acme.';
  var rkey = 'ACME:'+req.params.token;
  console.log(log_prefix, 'redis.get('+rkey+') = ... ');
  let token = await redis_client.get(rkey);
  console.log(log_prefix, 'redis.get('+rkey+') =', token);
  res.end(token||'NULL'); 
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
