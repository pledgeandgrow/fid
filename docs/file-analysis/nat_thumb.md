# nat_thumb.fd — Cortex-M (ARMv7-M, Thumb-2) code generator

> **New doc 2026-11-21.** Read from the source; the status is the Nov 21
> check of competitive-roadmap PL5.

- **Path:** `compiler/nat_thumb.fd`
- **LOC:** 3,207 (2026-11-21). In git since Nov 19 (49fb2b9).
- **Role:** competitive-roadmap PL5. IR → Thumb-2 machine code for a
  32-bit target over the `t2_*`/`t_*` encoders of nat_enc.fd, as one ELF.
  `nat_thumb_generate(module, bare)` is the entry point. Two targets:
  `--target thumbv7m-none` (no OS: vector table, a reset stub that copies
  `.data` to RAM, UART output, exit through semihosting; boards
  `mps2-an385`, the default, and `lm3s6965evb`, chosen by
  `FID_THUMB_BOARD`) and `--target thumbv7m-linux` (the same code over
  ARM EABI Linux system calls, a static ELF for qemu-arm: the test bed).
  A value is 64 bits held in a register pair; a pointer is its 32-bit
  address in the low word, so memory layouts are the 64-bit ones. Each
  value has an 8-byte frame slot. The runtime is stdlib/linux_runtime,
  merged in as for the other machine targets; this module writes its two
  primitives `nat_syscall` and `nat_start_sp`.
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `ir`, `nat_enc`.

## Callers

| File | What |
|------|------|
| `nat_ir.fd` | for a `thumbv7m-*` target: `nat_thumb_generate(module, g_nat_thumb_bare)` (bare unless the target names linux); `--cpu cortex-m3\|cortex-m4\|cortex-m7\|armv7-m\|baseline` |

Only nat_ir.fd imports it. Command: `fid emit|build <file.fd> --target
thumbv7m-none|thumbv7m-linux`.

## Not supported

Floating-point arithmetic and conversions (an f64 is moved and printed as
bits), threads and tasks, signals, C functions. An operation that needs
them panics with a message when it is reached, and the build notes where
they are. 64-bit division, the checked multiplications and variable shifts
are helper routines (`__th_*`).

## Status (Nov 21 check, PL5: partly)

tests/nattools/enc-oracle-pl5.py (226 forms) and enc-oracle-op21.py (662 forms)
found no mismatch; `test_thumb_core` 23/23; tests/nattools/thumb_check.sh:
10 passed (thumbv7m-linux under qemu-arm: core, overflow panic, exit
status; the thumbv7m-none qemu-user board; the image shape for both
boards). The no-OS images (vector table, UART, semihosting exit) were not
run: no qemu-system-arm on the machine of the check.

## Findings

- **INFO:** no floats on this target (above).
- No other bugs found from reading.
