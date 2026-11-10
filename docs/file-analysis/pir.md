# pir.fd — PIR L1/L2/L3 transforms

> **Re-audited 2026-11-09.** StructCopy fixed (Oct 23/24). PIR is no longer on any default build
> path; it backs `pir`, `opt`, `codegen`, `gpu`, `emit --boot` and `build --native` (default
> target only).

- **Path:** `compiler/pir.fd` — **2,704 LOC** (Rust `src/pir.rs` = 2,262)
- **Role:** `lower_ir_to_l1` (:964), `lower_l1_to_l2` (:1284), `lower_l2_to_l3` (:1504), plus
  `format_l1/l2/l3` printers.

## Where it is called

| Caller | Site | Command |
|---|---|---|
| `main.fd` `cmd_pir` | :557-583 | `fid pir` / `dump-pir` (L1, L2, L3, all) |
| `main.fd` `cmd_opt` | :620 | `fid opt` |
| `main.fd` `cmd_codegen` | :2225 | `fid codegen` (x86-64, arm64, riscv asm; ELF/COFF/Mach-O objects) |
| `main.fd` `cmd_gpu` | :2326 | `fid gpu` (SPIR-V / PTX from L3) |
| `main.fd` `cmd_build --boot` | :2887 | multiboot2 kernel via `pe.fd generate_boot_binary` |
| `compiler.fd` `build_executable` | :2143-2152 | `build --native` (default target x86-64, or `windows`) |

Importers: codegen_arm64, codegen_riscv, codegen_x64, compiler, elf64, gpu, main, opt, pe,
regalloc. Default `build`/`emit` (pe_ir on Windows, C on posix) and the nat_ir targets do not
use it.

## Findings

### Verified live

- `s18.exe pir hello.fd 3` and `codegen hello.fd --peephole` work (L3 printed, 34,558 asm lines).
- `emit --boot` on a 3-line program panics in `pe.fd` (`PeCodeGen_iat_off` ←
  `PeCodeGen_gen_print_int`), after PIR lowering: not this file.

### Code inspection

#### StructCopy dropped at L1 — FIXED

pir.fd:1125-1129 lowers `StructCopy(dst, src, ty)` to `L1Inst::Store(dst, src, Some(ty), {write})`.
ir.fd now has effects (:1163) and refs (:1256) arms for it.

#### `is_pure` inherits the effects hole — FIXED (upstream)

`is_pure: EffectSet_is_pure(&f.effects)` (:1461) is unchanged, but `inst_effects` now gives
StructCopy a Write effect (ir.fd:1163), so the propagation no longer marks a copy-only function pure.

#### LOW (inherited) — `ConstStr` equal to a function name becomes `ImmFunc` — STILL TRUE

:1859-1862. A string literal that spells a function name becomes that function's address.

#### INFO (inherited) — block params carried but never bound — STILL TRUE

`params: b.params` (:1205, :1449); Jump/Branch carry no arguments at any level.

#### INFO — metadata

`borrow_kinds` collected from `ValueMeta` (:1181-1188). The lowering now sets them on parameters
(lower.fd:4129); opt.fd still does not read them (opt.md).

## Fix order

1. None needed for correctness here; whether to keep PIR at all is the question (no default path
   uses it).

## Verification state

- Re-checked by grep and reading the cited lines; StructCopy fix confirmed in the source; live
  `pir`/`codegen` runs with s18.
