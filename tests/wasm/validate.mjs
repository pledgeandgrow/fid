// tests/wasm/validate.mjs — validate WebAssembly modules with Node's engine
// (V8 checks every function body: types, the block / loop / if nesting,
// branch depths). Prints `valid <file>` or `INVALID <file>: <reason>` per
// module; exits 1 when one is invalid.
//
//   node tests/wasm/validate.mjs a.wasm [b.wasm ...]
import { readFileSync } from 'node:fs';

let bad = 0;
for (const f of process.argv.slice(2)) {
  let bytes;
  try {
    bytes = readFileSync(f);
  } catch (e) {
    console.log(`INVALID ${f}: cannot read (${e.message})`);
    bad++;
    continue;
  }
  if (WebAssembly.validate(bytes)) {
    console.log(`valid ${f}`);
    continue;
  }
  let why = 'invalid';
  try {
    await WebAssembly.compile(bytes);
  } catch (e) {
    why = e.message;
  }
  console.log(`INVALID ${f}: ${why}`);
  bad++;
}
process.exit(bad ? 1 : 0);
