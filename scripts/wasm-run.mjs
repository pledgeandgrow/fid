// wasm-run.mjs — run a Fides program built for WebAssembly (release F9):
//   fid ccompile prog.fd --target wasm32-wasi -o prog.wasm   (through C)
//   fid wasm prog.fd -o prog.wasm                            (native backend)
//   node scripts/wasm-run.mjs prog.wasm [args...]
// The program runs under Node's WASI (preview 1) with the current folder
// as its file system root ("." and "/"), the host's temporary directory
// as /tmp, its arguments and environment. Exit code: the
// program's. Any WASI host works as well (wasmtime, wasmer).
import { readFile } from 'node:fs/promises';
import { readdirSync } from 'node:fs';
import { WASI } from 'node:wasi';
import process from 'node:process';
import { tmpdir } from 'node:os';
import path from 'node:path';
import { Worker, isMainThread } from 'node:worker_threads';

// XT28: the program runs on a worker thread with a 64 MB stack. WASM's
// call stack is the host's, and Node's main thread has about 1 MB: 513
// nested JSON arrays (test_std_json_depth) overflowed it, where a native
// build has 8 MB. The program's own output goes straight to fds 1 and 2
// (WASI), the exit code comes back from the worker.
// FID_WASM_MAIN_THREAD=1 runs it on the main thread, as before.
if (isMainThread && !process.env.FID_WASM_MAIN_THREAD) {
  // (the worker's "WASI is experimental" warning is printed here)
  process.removeAllListeners('warning');
  const worker = new Worker(new URL(import.meta.url), {
    argv: process.argv.slice(2),
    resourceLimits: { stackSizeMb: 64 },
  });
  const code = await new Promise((resolve) => {
    worker.on('error', (e) => {
      process.stderr.write('Panic: WebAssembly host: ' + String(e && e.message || e) + String.fromCharCode(10));
      resolve(1);
    });
    worker.on('exit', (c) => resolve(c));
  });
  process.exit(code);
}

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
const preopens = { '.': '.', '/': '.', '/tmp': tmpdir() };
const wasi = new WASI({
  version: 'preview1',
  args: [file, ...args],
  env,
  preopens,
  returnOnExit: true,
});
const imports = wasi.getImportObject();
let memory = null;
// Node's WASI (uvwasi) has no fd_readdir on Windows (ENOSYS): list a
// directory here, from the host path of each directory the program opened
// (path_open is followed for that; the preopens are fds 3, 4, ... in order)
if (process.platform === 'win32') {
  const w = imports.wasi_snapshot_preview1;
  const hostPath = new Map();
  Object.values(preopens).forEach((p, i) => hostPath.set(3 + i, path.resolve(p)));
  const npre = hostPath.size;
  const open = w.path_open;
  w.path_open = (dirfd, dirflags, ptr, len, oflags, base, inh, fdflags, out) => {
    const r = open(dirfd, dirflags, ptr, len, oflags, base, inh, fdflags, out);
    if (r === 0 && memory && hostPath.has(dirfd)) {
      const rel = Buffer.from(memory.buffer, ptr, len).toString();
      const fd = new DataView(memory.buffer).getUint32(out, true);
      hostPath.set(fd, path.resolve(hostPath.get(dirfd), rel));
    }
    return r;
  };
  const close = w.fd_close;
  w.fd_close = (fd) => {
    if (fd >= 3 + npre) hostPath.delete(fd);
    return close(fd);
  };
  w.fd_readdir = (fd, buf, bufLen, cookie, usedAt) => {
    const dir = hostPath.get(fd);
    if (dir === undefined || !memory) return 8;              // EBADF
    let ents;
    try {
      ents = readdirSync(dir, { withFileTypes: true });
    } catch (e) {
      return e && e.code === 'ENOTDIR' ? 54 : 44;           // ENOTDIR, ENOENT
    }
    const view = new DataView(memory.buffer);
    const bytes = new Uint8Array(memory.buffer);
    let used = 0;
    for (let i = Number(cookie); i < ents.length; i++) {
      // a dirent: d_next (u64), d_ino (u64), d_namlen (u32), d_type (u8)
      if (used + 24 > bufLen) { used = bufLen; break; }
      const name = Buffer.from(ents[i].name);
      const e = ents[i];
      const type = e.isDirectory() ? 3 : e.isFile() ? 4 : e.isSymbolicLink() ? 7 : 0;
      view.setBigUint64(buf + used, BigInt(i + 1), true);
      view.setBigUint64(buf + used + 8, 0n, true);
      view.setUint32(buf + used + 16, name.length, true);
      view.setUint32(buf + used + 20, type, true);
      used += 24;
      const n = Math.min(name.length, bufLen - used);
      bytes.set(name.subarray(0, n), buf + used);
      used += n;
      if (used >= bufLen) break;
    }
    view.setUint32(usedAt, used, true);
    return 0;
  };
}
const module = await WebAssembly.compile(await readFile(file));
const instance = await WebAssembly.instantiate(module, imports);
memory = instance.exports.memory;
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
// XT31: FID_WASM_MEMSTAT=1 prints the linear memory's size at exit (it
// never shrinks: the program's peak) for tests/wasm/memory-check.sh
if (process.env.FID_WASM_MEMSTAT && memory) {
  const pages = memory.buffer.byteLength / 65536;
  process.stderr.write('[wasm-run] memory: ' + pages + ' pages (' + Math.round(pages / 16) + ' MB)' + String.fromCharCode(10));
}
