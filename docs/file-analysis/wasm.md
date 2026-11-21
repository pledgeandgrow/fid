# wasm.fd — WebAssembly backend

> **compiler/wasm.fd was rewritten on Nov 15 (release F9):** it now lowers like nat_ir, uses i32 addresses, emits only what `_start` reaches, runs the Linux runtime over WASI (stdlib/linux_runtime/wasi.fd) and validates; scripts/wasm-test.sh `ALL=1` matches `fid run` on 316 of 344 tests. The findings below describe the module before the rewrite.

> **Re-audited 2026-11-09.** Since 10-23 the only change is `str_equal(a,b)`
> → `a == b` (commit 34259be). Probes with the latest self-hosted binary
> (s18) and Node's `WebAssembly.Module`.

- **Path:** `compiler/wasm.fd`
- **LOC:** 4,247 (2026-11-21; 1,656 on Nov 9; Rust `src/wasm.rs`: 1,277). Nov 21: imported by
  compiler.fd and main.fd (`fid wasm`), and imports `checks`; `fid wasm` runs no IR optimizer.
  New open (known-bugs 2g.108): a struct returned through a function value comes back wrong.
- **Role:** `IrModule` → WASM binary (`generate_wasm` :1580), or a section
  listing (`dump_wat` :1586). `WasmBuf` byte emitter; host imports from
  `env`; control flow as a `cur_block` local + `br $dispatch` loop.

## Where it's called

| Caller | Line | What |
|---|---|---|
| `main.fd` | 32, 179 | `wasm` → `cmd_wasm` :1584 |
| `compiler.fd` | 59, 1858 | `compile_to_wasm`: front end → `lower` → `generate_wasm` / `dump_wat` |

The working WebAssembly route is `ccompile --target wasm32-wasi` (C via
zig cc), not this module.

## Findings — verified live

### HIGH — every module fails validation (NEW, inherited)

Probe: hello world (`fn add` + `println`) → 129,632-byte module (all of
core is compiled in). `WebAssembly.Module` rejects it: "type error in
constant expression (expected i32, got i64)" — the data segment offset is
`i64.const` (:1536) for a 32-bit memory (limits flag 0x00, :1439).
Patching that byte in a scratch copy gives the next error: "i64.load
expected type i32, found local.get of type i64" — addresses are i64 for a
32-bit memory. Rust's wasm.rs has the same `i64.const` (src/wasm.rs:1182).
So no program compiled by `fid wasm` loads in any engine. Fix: wrap
addresses to i32 (`i32.wrap_i64`) before memory ops and use `i32.const`
offsets, or declare memory64 (limits flag 0x04) everywhere.

## Findings — code inspection

### LOW — `--wat` is a section listing, not WAT (STILL TRUE)

`dump_wat` :1586 prints sizes and function names; `cmd_wasm` says "Emitted
WAT text".

### LOW — stubs and silent zeros (STILL TRUE)

- `MakeDyn`/`DynCall`/`CallPtr` → `i32.const 0` / typed zero (:1162-1183).
- Unknown callees → typed zero; the header comment (:20-21) says
  `p_vec_*`/`p_map_*`/`p_string_*` emit `unreachable`; they do not.
- `StructCopy` dropped (:1282 TODO); `Txn*`, `Unsafe*`, `Grad*`,
  `Backward`, `Region*` no-ops; `Asm` dropped (:1292).

### Unchanged, faithful to Rust

Section order, host import table (16 names), call lowering with i32↔i64
extension, block dispatch, `Panic` → `p_panic` + `unreachable`.

## Fix order

1. Make the module valid (address width) — nothing else matters before.
2. `unreachable` for unsupported builtins (or fix the comment).
3. Real WAT text if wanted; `memory.copy` for StructCopy.

## Verification state

Probe: `fid wasm` → module → `WebAssembly.validate` false (two errors
found, above). Arms re-read at the cited lines. Runtime behaviour not
tested (module does not load).
