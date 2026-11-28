// tests/wasm/size.mjs <file.wasm> [n]: a WASM module's size, its sections
// and its n largest function bodies (by the "name" section's names), for
// tests/wasm/size-check.sh (XT39). No dependencies: Node only.
import { readFileSync } from 'node:fs';

const [file, nArg] = process.argv.slice(2);
if (!file) {
  console.error('usage: node tests/wasm/size.mjs <file.wasm> [n]');
  process.exit(2);
}
const top = Number(nArg || 10);
const b = readFileSync(file);
const secNames = ['custom', 'type', 'import', 'function', 'table', 'memory', 'global', 'export', 'start', 'element', 'code', 'data', 'datacount'];
let i = 8;
function uleb() {
  let r = 0, s = 0;
  for (;;) {
    const x = b[i++];
    r += (x & 0x7f) * 2 ** s;
    s += 7;
    if (!(x & 0x80)) return r;
  }
}
const names = {};
let nimports = 0;
const bodies = [];
const secs = [];
while (i < b.length) {
  const id = b[i++];
  const size = uleb();
  const end = i + size;
  let label = secNames[id] || String(id);
  if (id === 0) {
    const nl = uleb();
    const nm = b.slice(i, i + nl).toString();
    i += nl;
    label = 'custom "' + nm + '"';
    if (nm === 'name') {
      while (i < end) {
        const sub = b[i++];
        const ss = uleb();
        const se = i + ss;
        if (sub === 1) {
          const n = uleb();
          for (let k = 0; k < n; k++) {
            const idx = uleb();
            const l = uleb();
            names[idx] = b.slice(i, i + l).toString();
            i += l;
          }
        }
        i = se;
      }
    }
  } else if (id === 2) {
    nimports = uleb();
  } else if (id === 10) {
    const n = uleb();
    for (let k = 0; k < n; k++) {
      const s = uleb();
      bodies.push([k, s]);
      i += s;
    }
  }
  secs.push([label, size]);
  i = end;
}
console.log('size ' + b.length + ' bytes, ' + bodies.length + ' functions');
for (const [l, s] of secs) console.log('  section ' + l.padEnd(16) + String(s).padStart(8));
bodies.sort((x, y) => y[1] - x[1]);
if (top > 0) console.log('  largest functions:');
for (const [k, s] of bodies.slice(0, top)) {
  console.log('    ' + String(s).padStart(7) + '  ' + (names[k + nimports] || '#' + (k + nimports)));
}
