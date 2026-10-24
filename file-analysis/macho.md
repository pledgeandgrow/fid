# macho.fd — Mach-O Object Writer (x86-64)

**483 LOC** vs `src/macho.rs` 374 LOC (~129% — growth is entry structs + local str helpers).
**Was the eleventh dead module — WIRED since 2026-10-23.**

## Where it's called

- `main.fd` :40 `use compiler::macho`, :1734 `cmd_codegen` →
  `fidesc codegen --macho <file> -o out.o` → `emit_macho_file` →
  `emit_object_file` — mirrors Rust's `fidesc codegen --macho`
  (src/main.rs:2075/2132-2139). Reachable, though the encoder below is still
  a 7-mnemonic toy — the wired path produces nop-heavy objects just like
  coff's.

## What it is

A **toy** emitter — in both implementations:

- `encode_instruction` handles exactly **7 mnemonics** (`ret`/`nop`/`push`/`pop`/`call`/
  `jmp`/`int3|hlt`) as raw x86-64 bytes; everything else → `0x90` NOP (:201-203 — same
  nop-fallback family as elf64, but harmless here since nothing feeds it real asm).
- `push reg` decodes only `rbp`/`rsp`/other→`0x50` — `push rax` and `push rbx` produce the
  same byte (0x50 is `push rax`; rbx needs 0x53) — **inherited** (:163, src :126).
- `pop` similar — `pop rbp`→0x5D else→0x58.
- Emits **x86 opcodes inside a Mach-O container** — nominally valid for Intel macOS,
  useless on Apple Silicon (arm64 Mach-O needs `CPU_TYPE_ARM64`+arm64 enc).

## Layout — verified structurally correct (1:1 with Rust)

- MH_MAGIC_64 / CPU_TYPE_X86_64 / MH_OBJECT constants match.
- Header (32B) + LC_SEGMENT_64 (72B) + 80B `__text` section + LC_SYMTAB (24B);
  offsets/sizes precomputed identically.
- `maxprot`/`initprot`/section `flags` all 0 — **inherited** (Rust :266-269 same zeros);
  tolerable for `MH_OBJECT` (the linker remaps) but non-canonical.
- `call`/`jmp` emit E8/E9 + 4-byte placeholder + `MachOReloc` (pcrel, len2, extern,
  rtype 0 = `X86_64_RELOC_BRANCH` is *not* a real Mach-O rtype — real ones are
  `X86_64_RELOC_BRANCH=2`? actually `X86_64_RELOC_SIGNED=1`/`BRANCH=2` — rtype 0 is
  `X86_64_RELOC_UNSIGNED` which is **wrong for a pcrel branch** — **inherited**).
- `.` directives and `//`/`#` comments skipped; labels recorded as `N_SECT|N_EXT` symbols.

## Port adaptation notes

- `emit_object_file(module: i64, ...)` — `L3Module` can't be a param type, so the port
  uses a dummy `i64` (Rust: `_module: &L3Module`, equally unused).
- 5 local str helpers (`str_lines`/`str_trim`/`str_trim_end`/`str_starts_with`/
  `str_ends_with`) re-implemented despite `use string` at :16 — same shadow-artifact
  family as embedded.fd.

## Assessment

A faithful port of a deliberately-minimal Rust scaffolding file — dead on both sides
(`--macho` is Rust-only) and worth ~nothing until a real ARM64 or full x86 encoder backs
it. If kept, fix the `push`/`pop` register encoding and the branch rtype; more likely it
should be deleted with its siblings (coff/cparser/debugger/embedded/gpu/hotreload).
