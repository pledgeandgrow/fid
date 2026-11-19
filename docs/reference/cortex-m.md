# Cortex-M (ARMv7-M) target

competitive-roadmap PL5. `compiler/nat_thumb.fd` turns the IR into Thumb-2
machine code for ARMv7-M (Cortex-M3; M4 and M7 run the same code), over the
encoders of `compiler/nat_enc.fd` (`t2_*`, `t_*`). `nat_ir.fd` sends
`--target thumbv7m-*` there (`nat_target_arch` gives `NAT_THUMB`,
`nat_generate_elf` calls `nat_thumb_generate`).

| Target | Image | Runs on |
|---|---|---|
| `thumbv7m-none` | ELF32 for a board: vector table at 0, reset handler, `.data` in RAM with its image in flash | `qemu-system-arm -M mps2-an385` (default board) or `-M lm3s6965evb`, a real board through its flasher |
| `thumbv7m-linux` | static ELF32 for ARM Linux (EABI system calls) | `qemu-arm`, a 32-bit ARM Linux |

```bash
fid emit prog.fd --target thumbv7m-linux -o prog && qemu-arm ./prog
fid emit prog.fd --target thumbv7m-none -o prog.elf
qemu-system-arm -M mps2-an385 -cpu cortex-m3 -nographic -monitor none \
    -semihosting-config enable=on,target=native -kernel prog.elf
FID_THUMB_BOARD=lm3s6965evb fid emit prog.fd --target thumbv7m-none -o prog.elf
FID_THUMB_FLAT=1 fid emit prog.fd --target thumbv7m-none -o prog.bin   # raw flash contents
```

## How values are held

The IR is the 64-bit IR of every backend. A value is a pair of 32-bit
registers (low word, high word); a pointer is its address in the low word
and 0 in the high one, so memory layouts (8-byte words, struct offsets,
`Vec` and string headers) are the same as on the 64-bit targets. Each value
has an 8-byte frame slot; an operation loads its operands into r0:r1 and
r2:r3 and stores r0:r1. Every argument is an 8-byte slot of the caller's
outgoing area (`[sp + 8k]`), the result comes back in r0:r1, and every
function saves r4-r8 and lr.

64-bit add/sub use the carry (`adds`/`adc`); multiply is `umull` + two
`mla`; division and remainder call `__th_udivmod` (one `udiv` when both
operands fit in 32 bits, else 64 shift-subtract steps) and `__th_sdivmod`;
the overflow-checked multiplications `__th_umul_ovf` / `__th_smul_ovf`;
variable shifts `__th_lsl` / `__th_lsr` / `__th_asr`. Arithmetic panics as on
the other native targets (overflow, division by zero, `i64::MIN / -1`).

## The runtime

`stdlib/linux_runtime` is merged in as for the other machine targets; its
machine primitives are written in `nat_thumb.fd`:

- `nat_syscall`: on Linux the generic system call numbers are mapped to the
  ARM EABI ones (`mmap` -> `mmap2`, `clock_gettime` -> `clock_gettime64`,
  `futex` -> `futex_time64`, ...; unmapped calls return -ENOSYS). With no
  OS it implements what the runtime needs: `write` to the UART, `exit` /
  `exit_group` through semihosting `SYS_EXIT_EXTENDED` (the exit status
  reaches QEMU), `mmap` as a bump allocator of zeroed memory, the clock as
  0, the rest -ENOSYS.
- `nat_start_sp`: on Linux the kernel's argc/argv/envp copied as 8-byte
  words; with no OS an empty block.
- atomics are plain loads and stores (one core), the thread word is a
  global, there are no frame records (a panic prints no trace), no signals,
  no tasks or threads.

## Boards (no OS)

| Board | Code | Stack | `.data` | Heap | UART |
|---|---|---|---|---|---|
| `mps2-an385` (default) | 0x00000000 | 0x20000000-0x20100000 (sp starts at the top) | 0x20100000 | PSRAM 0x21000000-0x22000000 | CMSDK UART0 0x40004000 |
| `lm3s6965evb` | 0x00000000 (256 KB) | 0x20000000-0x20004000 | 0x20004000 | after `.data` to 0x20010000 | PL011 UART0 0x4000C000 |
| `qemu-user` (test) | 0x00100100 | as mps2-an385 | as mps2-an385 | as mps2-an385 | `write(2)`; exit is `exit_group` |

A stack overflow leaves the RAM downwards and a heap overrun leaves it
upwards: both fault, and the fault handler prints `Panic: hard fault ...`
on the UART and exits with 1. The runtime asks for its heap in 4 MB
chunks: on lm3s6965evb the chunk is larger than the RAM, so a program
works as long as what it really allocates fits, then faults.

`qemu-user` is the no-OS image (reset handler, `.data` copy, the system
calls it implements) laid out so that `qemu-arm` runs it: it checks that
code without a system emulator; the vector table, UART and semihosting are
checked by `qemu-system-arm` only.

## Not supported

Floating-point arithmetic and int/float conversions (an f64 is still moved
and its bits printed; an operation that needs the FPU panics with "... is
not supported on thumbv7m (in <function>)" when it runs, and the build
lists where), threads and tasks, signals, inline assembly, C functions.

## Checks

- `tests/bootstrap/test_thumb_core.fd`: 64-bit arithmetic across the word
  boundary, division, shifts, compares, narrow types, strings, `Vec`,
  `HashMap`, structs, enums, closures, trait objects, globals.
- `tests/nattools/thumb_check.sh`: that test under `fid run`, under
  `qemu-arm` (thumbv7m-linux and the qemu-user board), a panic, an exit
  status, the shape of the board images, and the board runs when
  `qemu-system-arm` is installed.
- `tests/nattools/enc-oracle-pl5.py`: the 226 encodings the generator uses
  beyond enc-oracle-op21.py's, byte for byte against clang.
