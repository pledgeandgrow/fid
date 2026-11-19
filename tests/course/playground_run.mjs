#!/usr/bin/env node
// tests/course/playground_run.mjs -- run Fides files the way the browser
// playground runs them, for tests/course/check_labs.py --playground.
//
//   node tests/course/playground_run.mjs [--assets <dir>] [--shim <index.js>] file.fd...
//
// Each file is compiled and run by the playground's own compiler (the WASI
// build of fidesc, `fidesc.wasm`, with `stdlib.json`) through the worker of
// docs/playground/embed.js, driven as the page drives it (`run` mode, empty
// standard input). <dir> defaults to the website checkout beside this
// repository (../fides-lang/public/playground) or $FIDES_PLAYGROUND_ASSETS;
// the shim to that checkout's node_modules/@bjorn3/browser_wasi_shim or
// $FIDES_WASI_SHIM (the same defaults as scripts/playground-embed-check.mjs).
//
// Prints one JSON object: { "<file>": { "stdout", "stderr", "exitCode" } or
// { "error" } }. Exit 0 when every file was run (whatever it printed), 1 on
// a failure of the harness, 2 when the assets are missing.
import { existsSync, readFileSync } from 'node:fs';
import { createRequire } from 'node:module';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const root = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');
const args = process.argv.slice(2);
const files = [];
const flags = {};
for (let i = 0; i < args.length; i++) {
  if (args[i] === '--assets' || args[i] === '--shim') { flags[args[i]] = args[i + 1]; i++; }
  else files.push(args[i]);
}
const site = resolve(root, '..', 'fides-lang');
const assets = flags['--assets'] || process.env.FIDES_PLAYGROUND_ASSETS || join(site, 'public', 'playground');
const shimPath = flags['--shim'] || process.env.FIDES_WASI_SHIM ||
  join(site, 'node_modules', '@bjorn3', 'browser_wasi_shim', 'dist', 'index.js');
for (const [what, p] of [['fidesc.wasm', join(assets, 'fidesc.wasm')], ['stdlib.json', join(assets, 'stdlib.json')], ['the WASI shim', shimPath]]) {
  if (!existsSync(p)) {
    process.stderr.write(`playground_run: ${what} not found at ${p} (--assets / --shim)\n`);
    process.exit(2);
  }
}

const embed = createRequire(import.meta.url)(join(root, 'docs', 'playground', 'embed.js'));
const shim = await import(pathToFileURL(shimPath).href);
// the shim logs each WASI path it opens with console.log
console.log = () => {};

const fake = { onmessage: null, inbox: [], waiters: [] };
fake.postMessage = (m) => {
  const w = fake.waiters.shift();
  if (w) w(m); else fake.inbox.push(m);
};
const next = () => new Promise((res, rej) => {
  const t = setTimeout(() => rej(new Error('no reply in 120 s')), 120000);
  const done = (m) => { clearTimeout(t); res(m); };
  if (fake.inbox.length) done(fake.inbox.shift()); else fake.waiters.push(done);
});
embed.workerMain(fake, shim, embed.MODE_ARGS);
const send = (m) => { fake.onmessage({ data: m }); return next(); };

const out = {};
try {
  const wasm = readFileSync(join(assets, 'fidesc.wasm'));
  const stdlib = JSON.parse(readFileSync(join(assets, 'stdlib.json'), 'utf-8'));
  const r = await send({ type: 'init', wasm: wasm.buffer.slice(wasm.byteOffset, wasm.byteOffset + wasm.byteLength), stdlib });
  if (r.type !== 'ready') throw new Error('init: ' + JSON.stringify(r));
  for (const f of files) {
    // the playground's share links carry LF line endings
    const source = readFileSync(f, 'utf-8').replace(/\r\n?/g, '\n');
    try {
      const res = await send({ mode: 'run', source, stdin: '' });
      out[f] = res.type === 'result'
        ? { stdout: res.stdout, stderr: res.stderr, exitCode: res.exitCode }
        : { error: res.message || 'no result' };
    } catch (e) {
      out[f] = { error: e.message };
    }
  }
} catch (e) {
  process.stderr.write(`playground_run: ${e.message}\n`);
  process.stdout.write(JSON.stringify(out) + '\n');
  process.exit(1);
}
process.stdout.write(JSON.stringify(out) + '\n');
process.exit(0);
