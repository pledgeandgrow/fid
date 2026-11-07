// wasm-run.mjs — run a Fides program built for WebAssembly (release F9):
//   fid ccompile prog.fd --target wasm32-wasi -o prog.wasm
//   node scripts/wasm-run.mjs prog.wasm [args...]
// The program runs under Node's WASI (preview 1) with the current folder
// as its file system root, its arguments and environment. Exit code: the
// program's. Any WASI host works as well (wasmtime, wasmer).
import { readFile } from 'node:fs/promises';
import { WASI } from 'node:wasi';
import process from 'node:process';

const [file, ...args] = process.argv.slice(2);
if (!file) {
  console.error('usage: node scripts/wasm-run.mjs <prog.wasm> [args...]');
  process.exit(2);
}
// Node prints an "experimental" warning for WASI on every run
process.removeAllListeners('warning');
const wasi = new WASI({
  version: 'preview1',
  args: [file, ...args],
  env: process.env,
  preopens: { '.': '.', '/': '.' },
  returnOnExit: true,
});
const module = await WebAssembly.compile(await readFile(file));
const instance = await WebAssembly.instantiate(module, wasi.getImportObject());
process.exitCode = wasi.start(instance);
