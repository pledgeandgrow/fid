// wasm-run.mjs — run a Fides program built for WebAssembly (release F9):
//   fid ccompile prog.fd --target wasm32-wasi -o prog.wasm   (through C)
//   fid wasm prog.fd -o prog.wasm                            (native backend)
//   node scripts/wasm-run.mjs prog.wasm [args...]
// The program runs under Node's WASI (preview 1) with the current folder
// as its file system root ("." and "/"), the host's temporary directory
// as /tmp, its arguments and environment. Exit code: the
// program's. Any WASI host works as well (wasmtime, wasmer).
import { readFile } from 'node:fs/promises';
import { WASI } from 'node:wasi';
import process from 'node:process';
import { tmpdir } from 'node:os';

const [file, ...args] = process.argv.slice(2);
if (!file) {
  console.error('usage: node scripts/wasm-run.mjs <prog.wasm> [args...]');
  process.exit(2);
}
// Node prints an "experimental" warning for WASI on every run
process.removeAllListeners('warning');
// The host's temporary directory is the guest's /tmp, which TEMP, TMP and
// TMPDIR name (a host path such as C:/Users/.../Temp means nothing in WASI)
const env = { ...process.env, TEMP: '/tmp', TMP: '/tmp', TMPDIR: '/tmp' };
const wasi = new WASI({
  version: 'preview1',
  args: [file, ...args],
  env,
  preopens: { '.': '.', '/': '.', '/tmp': tmpdir() },
  returnOnExit: true,
});
const module = await WebAssembly.compile(await readFile(file));
const instance = await WebAssembly.instantiate(module, wasi.getImportObject());
try {
  process.exitCode = wasi.start(instance);
} catch (e) {
  // A trap ends the program as a panic does on the other targets (exit
  // status 1, "Panic: ..." on stderr): the host's stack overflowing, a
  // memory access outside the linear memory, `unreachable`
  let msg = String(e && e.message || e);
  if (e instanceof RangeError && /call stack/i.test(msg)) msg = 'Stack overflow';
  else if (/out of bounds/i.test(msg)) msg = 'Invalid memory access (null or dangling pointer)';
  else msg = 'WebAssembly trap: ' + msg;
  process.stderr.write('Panic: ' + msg + String.fromCharCode(10));
  process.exitCode = 1;
}
