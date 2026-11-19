#!/usr/bin/env node
// scripts/playground-embed-check.mjs — the embeddable playground
// (docs/playground/embed.js, roadmap L10D) compiles and runs preset code.
//
//   node scripts/playground-embed-check.mjs [--assets <dir>] [--shim <index.js>]
//
// <dir> holds the playground's fidesc.wasm and stdlib.json (default: the
// website checkout beside this repository, ../fides-lang/public/playground,
// or $FIDES_PLAYGROUND_ASSETS); --shim is @bjorn3/browser_wasi_shim's
// dist/index.js (default: that checkout's node_modules, or $FIDES_WASI_SHIM).
//
// Checks, with embed.js's own worker function (the one the page puts in a
// Blob worker) driven the way the page drives it:
//   1. the helpers: share links round-trip and match the full playground's
//      form (btoa(encodeURIComponent(code))); dedent of indented preset code;
//      the mode list of data-modes;
//   2. init, then the presets of docs/playground/example.html: each <pre
//      class="fides-playground"> is run in its first mode; hello prints its
//      line, the loop prints 5050, the type error fails with a diagnostic;
//   3. every inspection mode (tokens, ast, ir, effects) answers;
//   4. embed.html and example.html load embed.js and use its class names.
// Exit 0 when all hold, 1 on a failure, 2 when the assets are missing.
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { createRequire } from 'node:module';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const root = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const args = process.argv.slice(2);
const opt = (name, env, dflt) => {
  const i = args.indexOf(name);
  return i >= 0 ? args[i + 1] : process.env[env] || dflt;
};
const site = resolve(root, '..', 'fides-lang');
const assets = opt('--assets', 'FIDES_PLAYGROUND_ASSETS', join(site, 'public', 'playground'));
const shimPath = opt('--shim', 'FIDES_WASI_SHIM', join(site, 'node_modules', '@bjorn3', 'browser_wasi_shim', 'dist', 'index.js'));
for (const [what, p] of [['fidesc.wasm', join(assets, 'fidesc.wasm')], ['stdlib.json', join(assets, 'stdlib.json')], ['the WASI shim', shimPath]]) {
  if (!existsSync(p)) {
    console.log(`playground-embed-check: ${what} not found at ${p} (--assets / --shim)`);
    process.exit(2);
  }
}

const embed = createRequire(import.meta.url)(join(root, 'docs', 'playground', 'embed.js'));
const shim = await import(pathToFileURL(shimPath).href);
// the shim logs each WASI path it opens with console.log ("wasi: main.fd")
const say = console.log.bind(console);
console.log = () => {};
let failures = 0;
const check = async (label, fn) => {
  try {
    await fn();
    say(`ok    ${label}`);
  } catch (e) {
    failures++;
    say(`FAIL  ${label}: ${e.message}`);
  }
};

// 1. helpers
await check('share links round-trip in the playground form', () => {
  const code = 'fn main() {\n    println("héllo + 100% #1");\n}\n';
  const h = embed.encodeShare(code);
  assert.equal(h, Buffer.from(encodeURIComponent(code), 'latin1').toString('base64'));
  assert.equal(embed.decodeShare('#' + h), code);
  assert.equal(embed.decodeShare('#not base64!'), null);
});
await check('dedent removes shared indentation and blank edges', () => {
  assert.equal(embed.dedent('\n    fn main() {\n        x();\n    }\n  \n'), 'fn main() {\n    x();\n}\n');
  assert.equal(embed.dedent('\r\n\ta\r\n\t\tb\r\n'), 'a\n\tb\n');
  assert.equal(embed.dedent(''), '');
});
await check('data-modes keeps known modes in order', () => {
  assert.deepEqual(embed.parseModes('ir, run,bogus'), ['ir', 'run']);
  assert.deepEqual(embed.parseModes(''), embed.MODES);
  assert.deepEqual(embed.parseModes('bogus'), embed.MODES);
});

// 2. the worker, as the page drives it
const fake = { onmessage: null, inbox: [], waiters: [] };
fake.postMessage = (m) => {
  const w = fake.waiters.shift();
  if (w) w(m); else fake.inbox.push(m);
};
const next = () => new Promise((res, rej) => {
  const t = setTimeout(() => rej(new Error('no reply in 60 s')), 60000);
  const done = (m) => { clearTimeout(t); res(m); };
  if (fake.inbox.length) done(fake.inbox.shift()); else fake.waiters.push(done);
});
embed.workerMain(fake, shim, embed.MODE_ARGS);
const send = (m) => { fake.onmessage({ data: m }); return next(); };

const wasm = readFileSync(join(assets, 'fidesc.wasm'));
const stdlib = JSON.parse(readFileSync(join(assets, 'stdlib.json'), 'utf-8'));
let ready = false;
await check('init compiles the compiler once', async () => {
  const before = await send({ mode: 'run', source: 'fn main() {}' });
  assert.equal(before.type, 'error', 'work before init must be refused');
  const r = await send({ type: 'init', wasm: wasm.buffer.slice(wasm.byteOffset, wasm.byteOffset + wasm.byteLength), stdlib });
  assert.equal(r.type, 'ready', JSON.stringify(r));
  ready = true;
});

const page = readFileSync(join(root, 'docs', 'playground', 'example.html'), 'utf-8');
const presets = [...page.matchAll(/<pre class="fides-playground"([^>]*)>([\s\S]*?)<\/pre>/g)].map((m) => {
  const attrs = m[1];
  const modes = embed.parseModes((attrs.match(/data-modes="([^"]*)"/) || [])[1]);
  let mode = (attrs.match(/data-mode="([^"]*)"/) || [])[1];
  if (!modes.includes(mode)) mode = modes[0];
  const text = m[2].replace(/&lt;/g, '<').replace(/&gt;/g, '>').replace(/&amp;/g, '&');
  return { mode, source: embed.dedent(text) };
});
const want = [
  { exit: 0, out: /^Hello from an embedded playground\n$/ },
  { exit: 0, out: /^5050\n$/ },
  { exit: 'nonzero', err: /error/i },
];
await check(`example.html has ${want.length} presets`, () => assert.equal(presets.length, want.length));
if (ready) {
  for (const [i, p] of presets.entries()) {
    const w = want[i];
    if (!w) break;
    await check(`preset ${i + 1} (${p.mode}) ${w.exit === 0 ? 'runs' : 'is rejected'}`, async () => {
      const r = await send({ mode: p.mode, source: p.source, stdin: '' });
      assert.equal(r.type, 'result', r.message);
      if (w.exit === 0) assert.equal(r.exitCode, 0, r.stderr);
      else assert.notEqual(r.exitCode, 0);
      if (w.out) assert.match(r.stdout.replace(/\r/g, ''), w.out);
      if (w.err) assert.match(r.stderr + r.stdout, w.err);
    });
  }
  // 3. inspection modes
  for (const [mode, needle] of [['tokens', 'Fn'], ['ast', 'Program'], ['ir', 'fn main'], ['effects', 'fn main']]) {
    await check(`mode ${mode}`, async () => {
      const r = await send({ mode, source: 'fn main() { println("test"); }' });
      assert.equal(r.type, 'result', r.message);
      assert.equal(r.exitCode, 0, r.stderr);
      assert.ok(r.stdout.includes(needle), `no ${needle} in the output`);
    });
  }
  await check('an unknown mode is an error, not a crash', async () => {
    const r = await send({ mode: 'bogus', source: '' });
    assert.equal(r.type, 'error');
  });
}

// 4. the pages
await check('embed.html and example.html load embed.js', () => {
  const frame = readFileSync(join(root, 'docs', 'playground', 'embed.html'), 'utf-8');
  for (const [name, html] of [['embed.html', frame], ['example.html', page]]) {
    assert.match(html, /<script src="embed\.js" data-assets="\.\/"><\/script>/, name);
  }
  assert.match(frame, /fides-playground/);
  assert.match(frame, /decodeURIComponent\(atob\(location\.hash\.slice\(1\)\)\)/);
  assert.match(page, /<iframe[^>]+src="embed\.html\?/);
});

say(failures ? `playground-embed-check: ${failures} FAILED` : 'playground-embed-check: the embed runs its presets');
process.exit(failures ? 1 : 0);
