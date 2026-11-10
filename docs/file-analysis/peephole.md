# peephole.fd — asm-level peephole optimizer

> **Re-audited 2026-11-09.** Unchanged since Oct 23 except one line (Nov 1). It is no longer
> "always on in `build`": default `build`/`emit` go through pe_ir (Windows), C (posix) or nat_ir,
> none of which emits text asm.

- **Path:** `compiler/peephole.fd` — **1,151 LOC** (Rust `src/peephole.rs` = 994)
- **Role:** 9 string-level passes over x86-64 AT&T asm from `codegen_x64.fd`, 5-iteration
  fixpoint (`peephole_optimize` :46).

## Where it is called

| Caller | Site | Command |
|---|---|---|
| `compiler.fd` `build_executable` | :2159 | `build --native` with the default x86-64 target (crashes earlier in opt, see opt.md) |
| `main.fd` `cmd_codegen` | :2230 | `fid codegen --peephole` |
| `pe.fd` `generate_boot_binary` | :2137 | `fid emit --boot` (crashes later in pe.fd) |
| `pe.fd` `generate_pe`, `generate_flat_binary` | :2123, :2130 | no callers (dead) |

`peephole_optimize_pass` (:72) still has no callers. Importers: compiler, main, pe.

## Findings

### Code inspection

#### MEDIUM (inherited) — `notq`/`notl` treated as flag-setting — STILL TRUE

`is_flag_setting` (:674) lists `notq` (:681) and `notl` (:688); x86 NOT sets no flags.
`pass_redundant_test` (:599) then drops a `testq %r, %r` right after `notq %r`. `~x` emits
`notq` (codegen_x64.fd:1222). Reachability through typed Fides code (a branch needs a bool, `~`
gives an integer) not re-checked.

#### MEDIUM — new: `imulq`, `cmpq`, `testq` also misclassified

The same table lists `imulq`/`imull` (:689-690: IMUL leaves ZF/SF undefined), and `cmpq`/`cmpl`/
`testq`/`testl` (:691-694), whose last operand is not a destination: `cmpq %rcx, %rax;
testq %rax, %rax` would lose the test and branch on `rax - rcx`. Codegen normally puts `setcc`
between a compare and a test, so not seen in output; latent. Keep only instructions whose flags
reflect their destination (add/sub/and/or/xor/neg).

#### LOW — redundant `(%r…)` checks in `pass_dead_store` — STILL TRUE

:545-558.

## Fix order

1. Remove not/imul/cmp/test from `is_flag_setting`.
2. (Optional) drop the redundant `(%r…)` checks.

## Verification state

- Live: `s18.exe codegen hello.fd --peephole` runs (34,558 lines); no `notq` in a probe's output.
- Flag semantics per the Intel SDM (NOT: none; IMUL: SF/ZF undefined).
