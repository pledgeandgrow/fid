// bench/service/node/server.mjs — the Node.js (node:http) server of the
// service benchmarks (BM5); the same routes and World table as
// bench/service/fides/server.fd.
//
//   node server.mjs [port]
//   node server.mjs --selftest
import http from 'node:http';

const WORLD_ROWS = 10000;
const world = new Array(WORLD_ROWS + 1);
for (let i = 0; i <= WORLD_ROWS; i++) world[i] = (i * 7919 + 13) % WORLD_ROWS + 1;

// the 31-bit LCG of every server; BigInt keeps the product exact
let rng = 42n;
function nextId() {
  rng = (rng * 1103515245n + 12345n) % 2147483648n;
  return Number((rng >> 8n) % BigInt(WORLD_ROWS)) + 1;
}

const row = (id) => `{"id":${id},"randomNumber":${world[id]}}`;

function queryCount(q) {
  let n = /^-?\d+/.test(q ?? '') ? parseInt(q, 10) : 1;
  if (n < 1) n = 1;
  if (n > 500) n = 500;
  return n;
}

function reply(path, queries) {
  switch (path) {
    case '/plaintext': return [200, 'text/plain', 'Hello, World!'];
    case '/json': return [200, 'application/json', '{"message":"Hello, World!"}'];
    case '/db': return [200, 'application/json', row(nextId())];
    case '/queries': {
      const out = [];
      for (let i = 0, n = queryCount(queries); i < n; i++) out.push(row(nextId()));
      return [200, 'application/json', `[${out.join(',')}]`];
    }
  }
  return [404, 'text/plain', 'not found\n'];
}

const arg = process.argv[2];
if (arg === '--selftest') {
  for (const [p, q] of [['/plaintext', ''], ['/json', ''], ['/db', ''], ['/queries', '3']]) {
    const [code, , body] = reply(p, q);
    console.log(code, body);
  }
} else {
  const port = Number(arg ?? 8080);
  const server = http.createServer((req, res) => {
    const url = new URL(req.url, 'http://x');
    const [code, type, body] = reply(url.pathname, url.searchParams.get('queries'));
    res.writeHead(code, { 'Content-Type': type, 'Content-Length': Buffer.byteLength(body), Server: 'node' });
    res.end(body);
  });
  server.on('error', () => {
    console.log(`cannot listen on ${port}`);
    process.exit(1);
  });
  server.listen(port, () => console.log(`listening on ${port}`));
}
