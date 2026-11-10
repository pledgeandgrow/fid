# runtime_linux.fd — hand-encoded x86-64 Linux runtime of the PIR path

Re-audited 2026-11-09 (previous audit 2026-10-22/23).

- **Path:** `compiler/runtime_linux.fd`
- **LOC:** 4,399
- **Role:** the runtime blob linked into `build --native` x86-64 ELF
  output: raw machine-code bytes for `_start`, syscalls, mmap allocator,
  print, vec basics, stubs; plus an asm text block (strings, maps, more vec
  ops) encoded with `elf64::encode_asm_block`. Returns `RuntimeParts` and
  relocations. Port of `src/runtime_linux.rs`. Not the runtime of the new
  native backend: that is `stdlib/linux_runtime/*.fd` (Fides source,
  merged into the program for `fid emit --target *-linux` and `--nat`).

## Where it is called

| Caller | What |
|--------|------|
| `compiler.fd` `build_executable` (x86-64) | `runtime_parts()` / `runtime_parts_with_caps(bits)`, `runtime_relocations()` |
| `runtime_arm64.fd`, `runtime_riscv.fd` | `use` for the shared `RuntimeParts` / `RuntimeSymbol` / `RuntimeReloc` types |

CLI: `fid build --native` with the default target `x86-64` only.

## Findings

### Verified live

1. **HIGH (not this file) — never reached in a working build.** `build
   --native` panics in `opt.fd pass_jump_thread`; with `--no-opt` the link
   fails "Undefined symbols: main" because the user object is unreadable
   (elf64.md 1). The runtime itself was linked (its `_start` asked for
   `main`).

### Code inspection (old findings re-checked)

2. **HIGH asm block dropped — FIXED.** `encode_asm_block(runtime_asm)` is
   called (:2858); bytes appended at `asm_base`, defined text symbols and
   relocations merged.
3. **Capabilities — FIXED.** `build_executable` calls
   `runtime_parts_with_caps(caps_to_bits(...))`; `cap_granted` checks bits
   per capability (:90); `strip_ungranted_caps` is called.
4. **LOW — asm errors are silent.** If `encode_asm_block` fails it returns
   empty bytes (elf64.md 9), and the runtime silently loses 39 functions.
5. **INFO — stubs**: `p_getenv` → 0, `p_argc` → 1, `p_argv` → 0,
   `p_system` → -1, `p_regex_match` → false (:4251-4285); `p_vec_new` a
   fixed 4 KB buffer; json/tensor/socket/http stubs.
6. **INFO — layouts predate proposal 0004** (counted strings, 40-byte Vec
   header, boxes). Not re-checked against today's lowering.

## Fix order

Retire with the PIR path (roadmap MI17). If kept: elf64 headers first,
then compare output with `fid run`, then 4-6.

## Verification state

- Probe: `build --native` (with and without `--no-opt`) on a 4-line
  program.
- Read: caps helpers, the asm-block encoding site, the stub section.
