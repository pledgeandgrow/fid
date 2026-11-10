# linker.fd — ELF64 static linker of the PIR-era backends

Re-audited 2026-11-09 (previous audit 2026-10-22/23).

- **Path:** `compiler/linker.fd`
- **LOC:** 1,077
- **Role:** reads ELF64 relocatable objects from `elf64.fd` (or raw
  arm64/riscv assembler output), adds a runtime blob (text, symbols,
  rodata, relocations), patches x86-64 / AArch64 / RISC-V relocations,
  writes a static ELF executable (text at 0x400078). Port of
  `src/linker.rs`. Not used by the new native backend (`nat_enc.fd` writes
  its own images).

## Where it is called

| Caller | What |
|--------|------|
| `compiler.fd` `build_executable` | `link_with_runtime` (x86-64), `link_raw_with_runtime_for_target` (arm64, riscv) |
| `compiler.fd` `build_x86_linux` | `link_with_runtime` with an empty runtime |

CLI: `fid build-x86`, and `fid build --native` with the default target
`x86-64`. The arm64/riscv branches of `build_executable` are unreachable
from the CLI: `--native --target arm64|riscv*` goes to `nat_ir.fd`.

## Findings

### Verified live (2026-11-09, `s18.exe`)

1. **HIGH — links nothing from elf64's objects.** `Linker_add_object`
   (:540-562) reads 64-byte section headers; elf64 writes 56-byte ones
   (elf64.md finding 1). Offsets are garbage and the last header is
   skipped (`offset + 64 > len`), so no `.text` and no symbols come in.
   `fid build-x86` → a 120-byte executable that segfaults;
   `fid build --native --no-opt` → "Undefined symbols: main". The fix
   belongs in elf64.fd.
2. **MEDIUM — a missing entry symbol is not an error.** `entry_offset`
   stays 0 when the entry is not found (:922-925); with no text at all the
   linker still writes an executable (finding 1 shows it). Make it an `Err`.

### Code inspection (old findings re-checked)

3. **CRITICAL write_u64 at `buf.len()` — FIXED.** `write_u32`/`write_u64`
   append when `off >= len` (:207-227); the executable header now has
   `e_entry` 0x400078 and a full program header (seen in the probe file).
4. **arm64 B/BL/B.cond delta off by 4 — FIXED.** `delta = sym_offset -
   patch_offset` (:847, :854).
5. **Unknown symbol → rel32 0 — FIXED.** Collected and returned as
   "Undefined symbols: ..." (:969-976).
6. **LOW — RELA addends parsed and dropped — STILL TRUE** (:685, :720).
   Works only because elf64 always writes -4 for PC32.
7. **LOW — `dead_code_elimination` removes no bytes — not re-checked.**
8. **LOW (new) — `sections.get(si).name = get_string(...)`** (:571)
   assigns into the result of `get`; whether that writes back depends on
   value semantics of `Vec.get` on structs. Not re-checked; if it does not
   write back, no section is ever found by name even with correct headers.

## Fix order

1. elf64 section headers (elf64.md 1), then re-test finding 8.
2. Error on missing entry (2).
3. Carry addends (6).

## Verification state

- Probes: build-x86 output (header bytes, size, run in WSL); `build
  --native --no-opt` error.
- Read: object parsing, relocation patching, executable writer.
