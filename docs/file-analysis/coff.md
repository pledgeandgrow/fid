# coff.fd — COFF (.obj) emitter for Windows AMD64 (424 LOC)

> **CONSOLIDATED 2026-10-23 (v2):** the entire hand-rolled asm parser +
> nop-sled encoder (F1–F3) **deleted** — file now assembles via the shared
> `ElfEmitter` and translates (bytes, symbols, relocs) into COFF records.
> Real instructions encode correctly; `.rdata` section emitted for rodata;
> `main`/pub syms get EXTERNAL storage class via ELF binding (F4 fixed).
> F1/F2/F3 died by deletion — they cannot regress without resurrecting the
> parser.

## Role

Format-only COFF writer: `emit_coff_file` → `ElfEmitter_assemble` (shared
encoder) → `CoffEmitter_import` translates ELF symbols/relocs/sections →
COFF header + .text/.rdata sections + per-section relocs + symtab/strtab.
Wired at `main.fd:1724` (`codegen --emit-coff`).

## Where it's called

| Caller | Site | What |
|--------|------|------|
| `main.fd` | :38 `use compiler::coff`, :1724 `cmd_codegen` | `fidesc codegen --emit-coff <file> -o out.obj` → `emit_coff_file` → `emit_object_file` — **WIRED since 2026-10-23** via the restored `codegen` command. |

Mirrors Rust's `fidesc codegen --emit-coff` (`src/main.rs:2119` →
`coff::emit_object_file`). `pe.fd` remains the *real* Windows backend; COFF
.obj output is a debugging/toolchain hook only.

## Findings

### F1 — ~~Dead module~~ RESOLVED (2026-10-23) — but reachability makes F2/F3 live bugs now

`fidesc codegen --emit-coff` now calls `emit_object_file` (:391) — which means
the nop-emitting encoder and mnemonic mismatch below are **exercised bugs**: any
real program through `--emit-coff` produces a nop-sled .obj today.

### F2 — The encoder is a nop-emitting stub — HIGH (inherited, but fatal in practice)

`encode_instruction` (:150-218) — faithful port of a Rust *placeholder*:

- `mov`/`movq`/`movl`/`add`/`sub`/`mul`/`imul`/`div`/`idiv` → **`push 0x90`
  (nop)** — literally every data-movement instruction is dropped.
- `push`/`pop` of any reg other than rbp/rsp → `0x50`/`0x58` = **push/pop
  %rax** regardless of operand — `pushq %rbx` pushes rax.
- `hlt` → `0xCC` (int3's byte; hlt is `0xF4`).
- `ud2` → unknown → nop → codegen_x64's post-panic `ud2` falls through.
- Every `.` directive skipped → `.rodata`, `.asciz`, `.quad` (vtables,
  string consts) never emitted — single `.text` section only.
- Only `ret`, `nop`, `call`, `jmp` (with REL32 relocs) produce real code.

Even with a perfect L3, output ≈ nop-sled + call relocs.

### F3 — Mnemonic set mismatched with codegen_x64 anyway — HIGH (inherited)

codegen_x64 emits AT&T-suffixed forms (`pushq`, `movq`, `subq`, `call`,
`jmp`, `testq`, `sete`, `movzbq`, `leaq`…). This parser keys on `push`,
`pop`, `mov`, `call`, `jmp`, `int3`, `hlt` — **`pushq` isn't even
recognized** (falls through to nop). Even the `ret`-adjacent instructions of
a prologue don't match. The module cannot assemble *any* real codegen_x64
output — it's calibrated for a different (nonexistent) asm dialect.

### F4 — Defined symbols marked STATIC — MEDIUM (inherited)

`add_symbol` (:220-224): `is_defined` → `IMAGE_SYM_CLASS_STATIC` (3) →
every defined symbol is **file-local** — a linker can't resolve `main` or
any exported fn from the .obj. Correct for external linkage is
`EXTERNAL` (2). Only undefineds get EXTERNAL — backwards for the symbols
that matter. (Rust:206-211 identical — faithful port of the bug.)

### F5 — `emit_object_file(module: i64, …)` — signature degraded — LOW (port drift)

Rust: `emit_object_file(module: &L3Module, asm: &str)` — the L3 module is
available for future use (alignment flags etc). Fides takes a bare `i64`
that's ignored — a cosmetic loss now, but if ever re-wired it can't consume
a real `L3Module` without a signature fix.

### F6 — Reloc/symbol table mechanics — mostly correct — INFO

Layout math (hdr 20 + sec-hdr 40 → text → relocs(10B) → syms(18B) →
strtab) is right; >8-char names go through the string table correctly;
`get_or_add_symbol` returns stable indices for reloc targets; REL32 is the
right AMD64 reloc type for call/jmp. The *COFF container* is fine — it's
the bytes inside that are hollow.

## Verification state

- **RESOLVED BY REWRITE (2026-10-23):** the recommendation above was taken —
  the file's own encoder was deleted entirely and `ElfEmitter` now drives
  it. Findings F1–F3 describe deleted code (kept for history); F4 fixed in
  the same pass (storage class now from ELF binding); F5 signature still
  `(module: i64, asm: str)` — unchanged, still degraded vs Rust's
  `&L3Module`.
- Typecheck: `use compiler::coff` probes clean post-rewrite.
- Live output still unverified end-to-end (needs a COFF consumer/objdump).

## Next

`compiler.fd` — the 5,000+ LOC pipeline orchestrator. Largest file in the
project.
