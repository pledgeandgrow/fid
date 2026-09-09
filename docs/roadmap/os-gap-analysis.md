# Fides OS Gap Analysis — What's Missing to Build an OS

> Generated Sep 8, 2026. Updated Sep 9, 2026 (Session 21 — volatile runtime
> support added, test bugs fixed, C codegen memory corruption fixed). Based on
> full codebase analysis (compiler, stdlib, tests, docs, roadmap).

---

## Executive Summary

Fides **aspires** to be a memory-safe OS/systems language and has made real
progress: bare-metal mode, x86-64 inline asm, UEFI bindings, GDT/IDT/paging
helpers, `@repr(C)`, `@cpu_local`, `--no-std`, flat/boot binary generation,
custom ELF linker sections, and kernel primitives (LAPIC, timer, kalloc,
TSS, CPUID, MSR, syscall, context-switch) all exist. The core safety
annotations (`@no-alloc`, `@no-panic`, `@crashfree`, `@medical`,
`@safety-critical`, `@autosar`) are now enforced by the self-hosted
typechecker. But the OS story is **not yet production-ready**. There are
critical blockers and ~30 specific gaps blocking real OS development.
No real kernel has been written or booted.

**Bottom line:** You can write toy kernels that compile, print to serial,
and use basic OS primitives. You cannot yet write a production OS with
proper interrupt handling, memory management, multi-core boot, or device
drivers.

---

## Part 1: Critical Blockers (Language-Level)

These affect ALL use cases, not just OS development. They are documented in
`docs/roadmap/roadmap-issues.md` → "Critical Blockers" section.

### CB1: Self-hosted compiler partially enforces safety annotations

**Impact on OS dev:** Fatal. The language's main selling point for OS work
is safety annotations (`@constant_time`, `@medical`, `@crashfree`, etc.).
The self-hosted compiler (`compiler/typechecker.fd`) now enforces the
**core safety primitives** (Session 19): `@no-alloc`, `@no-panic`,
`@no-recursion`, `@no-unbounded`, `@no-indirect`, `@requires-audit`, and
domain presets `@crashfree`, `@medical`, `@safety-critical`, `@autosar`.

**Session 20 update:** All 13 remaining annotation stubs have been replaced
with effect-based enforcement: `@conserve`, `@gas`, `@constant_time`,
`@realtime`, `@tainted`, `@differentiable`, `@quantized`, `@checkpoint`,
`@dp`, `@shard`, `@adapter`, `given`/`ensure` contracts (recognized),
`@allocator`. See `docs/roadmap/roadmap-issues.md` → CB1 for details.

**Remaining:** `@deterministic` has partial enforcement. `given`/`ensure`
contract evaluation pending comptime support for full pre/postcondition
evaluation.

### CB2: Result<T,E> and Option<T> hardcoded to i64

**Status:** ✅ Resolved (Session 19) — Generic type parameters are now
properly substituted during lowering. See `docs/roadmap/roadmap-issues.md`
→ CB2 for details.

### CB3: Backend test parity not 100%

**Current numbers** (updated Sep 9, 2026):

| Backend | Pass rate |
|---------|----------|
| Interpreter | ~235/243 (96.7%) |
| C codegen | ~218/243 (89.7%) |
| PE emitter | ~238/243 (98.0%) |

**Recently fixed (Session 21):**
- `test_os_features` — fixed `val` keyword conflict, updated bitfield API
- `test_tier1_kernel` — added `p_volatile_load`/`p_volatile_store` to interpreter
- `test_regression_collections` — fixed C codegen `free()` on arena strings
- `test_sandbox_alloc_fail` — moved to expected-fail (negative test)
- `test_token_full` — excluded (library module, no `main`)

**Impact on OS dev:** Medium. OS tests (`test_tier1_kernel`,
`test_os_features`) now pass on interpreter. Remaining failures are
mostly C codegen issues and tests requiring real OS resources.

### CB4: Parser error recovery needs hardening

**Impact on OS dev:** Low for OS specifically, but debug prints
(`print("DBG: parse loop done\n")`) in `compiler/parser.fd` lines 451-456
must be removed before any production use.

---

## Part 2: What Already Works for OS Development

### Language features that exist and work

| Feature | Status | Location |
|---------|--------|----------|
| Raw pointers (`*mut T`, `*const T`) | ✅ Works | `stdlib/core.fd`, used throughout |
| Pointer arithmetic | ✅ Works | `ptr + 8`, `*ptr` in `unsafe` blocks |
| `unsafe { }` blocks | ✅ Works | `compiler/parser.fd:3288`, `compiler/typechecker.fd:691` |
| Inline assembly (`asm!`) | ✅ Works | `compiler/parser.fd:4372`, 50+ x86 instructions in `compiler/pe.fd` |
| `@repr(C)` struct layout | ✅ Works | `compiler/lower.fd:1301-1392` |
| `@cpu_local` (GS-relative) | ✅ Works | `compiler/pe.fd:1425`, GS segment encoding |
| `--no-std` / `--bare` mode | ✅ Works | `compiler/compiler.fd:584-651` |
| `interrupt fn` syntax | ✅ Parsed | `compiler/parser.fd:710`, `compiler/pe.fd:1428` |
| `Volatile<T>` wrapper | ✅ Works | `stdlib/volatile.fd` |
| Bit manipulation operators | ✅ Works | `& \| ^ << >>` in `compiler/ast.fd` |
| FFI (`extern "C"`) | ✅ Works | `compiler/parser.fd:748` |
| Capabilities (traits) | ✅ Works | `cap Iterator`, `cap Error` in `core.fd` |
| Pattern matching | ✅ Works | `match` with enum variants, guards, ranges |
| Borrow checker | ✅ Works | `compiler/borrowck.fd`, NLL, field-wise, two-phase |
| Comptime evaluation | ✅ Works | `comptime`, `comptime fn`, `comptime gen` |
| Generic functions | ✅ Works | Monomorphized in `compiler/mono.fd` |
| Generic structs | ✅ Works | Parsed + lowered |
| Integer types i8-i128, u8-u128 | ✅ Works | `compiler/ast.fd:103-113` |
| f32/f64 | ✅ Works | `compiler/ast.fd:114-115` |
| Custom allocators | ✅ Works | `stdlib/core.fd:237-260` |
| Regions (memory regions) | ✅ Works | `region { }` blocks, region budgets |

### OS primitives that exist in stdlib

| Module | Lines | What's provided |
|--------|-------|----------------|
| `stdlib/kernel.fd` | 273 | GDT init, IDT handlers, page mapping, COM1 serial, `cli`/`sti`/`hlt`, port I/O (`inb`/`outb`), CR0/CR3/CR4 read/write, `invlpg`, physical memory read/write |
| `stdlib/uefi.fd` | 287 | UEFI boot services: `AllocatePool`, `FreePool`, `AllocatePages`, `GetMemoryMap`, `ExitBootServices`, `Stall`, console output, volatile pointers |
| `stdlib/volatile.fd` | ~140 | `Volatile<T>`, volatile load/store at 8/16/32/64-bit widths |
| `stdlib/bitfield.fd` | ~200 | Bit manipulation, CR0/CR4/EFER/PTE/GDT/IDT constants |
| `stdlib/sync.fd` | 255 | Atomics, mutex, RwLock, TLS, thread spawn |
| `stdlib/channel.fd` | 67 | Bounded MPSC channels |
| `stdlib/alloc.fd` | 32 | `p_malloc`/`p_free` wrappers |

### x86-64 instructions supported in inline asm

**Privileged/system instructions that work:**
`hlt`, `cli`, `sti`, `cld`, `lgdt`, `lidt`, `invlpg`, `in`, `out`, `int`,
`iret`, `iretq`, `rdmsr`, `wrmsr`, `swapgs`, `wbinvd`, `cpuid`, `rdtsc`,
`syscall`, `sysret`, `sysexit`, `pause`, `mfence`, `sfence`, `lfence`

**Standard user-space instructions:**
Full ALU (`add`/`sub`/`mul`/`div`/`and`/`or`/`xor`/`shl`/`shr`/`sar`),
`mov`, `cmp`, `test`, `push`/`pop`, `call`/`ret`/`jmp`/`jcc`, `setcc`,
`lea`, SSE2 float ops, `movsx`/`movzx`, `nop`, `ud2`

---

## Part 3: What's Missing to Build an OS

### Tier 1: Showstoppers (can't boot without these)

#### 1. No flat/binary output in self-hosted compiler

**Status:** ✅ Resolved — Session 19

`generate_flat` and `generate_boot` have been ported to `compiler/pe.fd`.
`--bare` and `--boot` CLI flags added to `compiler/main.fd`. Multiboot2
header generation is included in boot mode.

**Remaining:** UEFI PE32+ executable emission not yet ported.

#### 2. GDT/IDT loading is broken

**Status:** ✅ Resolved — Session 19

`stdlib/kernel.fd` now builds proper 10-byte GDTR/IDTR descriptors
(`limit:u16 + base:u64`) before `lgdt`/`lidt`.

#### 3. No TSS support

**Status:** ✅ Resolved — Session 19

`Tss` struct, `tss_init()`, `ltr` instruction encoding, and `p_ltr`
builtin added to `stdlib/kernel.fd`.

#### 4. No interrupt handler epilogue

**Status:** ✅ Resolved — Session 19

`compiler/pe.fd` now emits `iretq` instead of `ret` for `interrupt fn`.

#### 5. No 16-bit or 32-bit code generation

All x86-64 encoders assume 64-bit long mode. You can't write:
- BIOS real-mode boot code (16-bit)
- Protected-mode setup code (32-bit)
- Long-mode transition code (32-bit → 64-bit)

**Fix:** Either add 16/32-bit encoding support, or require UEFI boot
(which starts in 64-bit long mode) and document that BIOS boot is not
supported.

#### 6. Bare-metal runtime stubs missing from self-hosted PE

The self-hosted PE emitter (`compiler/pe.fd`) doesn't emit bare-metal
runtime stubs (`p_hlt`, `p_cli`, `p_sti`, `p_inb`, `p_outb`, `p_serial`,
`p_cr0_read`, etc.). These exist in the Rust `src/pe.rs` but were not
ported. On Linux runtime, they're no-ops (`0xc3` = just `ret`).

**Fix:** Port bare-metal runtime stubs from `src/pe.rs` to `compiler/pe.fd`.

### Tier 2: Core OS Features (can't build a real kernel without these)

#### 7. No APIC/LAPIC support

**Status:** ✅ Resolved — Session 19

`stdlib/apic.fd` added with LAPIC init, EOI, vector mapping, and
IOAPIC redirection entries.

#### 8. No timers

**Status:** ✅ Resolved — Session 19

`stdlib/timer.fd` added with PIT and HPET initialization.

#### 9. No CPUID support

**Status:** ✅ Resolved — Session 19

`cpuid(eax, ecx)` wrapper added to `stdlib/kernel.fd` that returns
eax/ebx/ecx/edx results.

#### 10. No MSR wrappers

**Status:** ✅ Resolved — Session 19

`msr_read(addr)` / `msr_write(addr, val)` and constants for common
MSRs (EFER, PAT, STAR, LSTAR, SFMASK, KERNEL_GS_BASE) added to
`stdlib/kernel.fd`.

#### 11. No syscall/sysenter setup

**Status:** ✅ Resolved — Session 19

`syscall_init(entry_point)` added to `stdlib/kernel.fd` with MSR_LSTAR,
MSR_STAR, and MSR_SFMASK setup.
- User/kernel stack switching

**Fix:** Add `syscall_init(entry_point)` in `stdlib/kernel.fd`.

#### 12. No context switching

**Status:** ✅ Resolved — Session 19

`ThreadContext` struct and `switch_context(old, new)` added to
`stdlib/kernel.fd` using inline asm to save/restore callee-saved
registers.

#### 13. No proper memory allocator for kernel

**Status:** ✅ Resolved — Session 19

`stdlib/kalloc.fd` added with physical frame allocator, page frame
management, and kernel heap.

#### 14. No disk/filesystem drivers

No ATA/IDE, no AHCI, no NVMe, no disk I/O. `stdlib/fs.fd` uses libc
`fopen`/`fread` which don't exist in a kernel.

**Fix:** Add disk driver framework. At minimum, ATA/IDE PIO mode for
basic disk access.

#### 15. No DMA support

No DMA abstraction, no DMA controller setup, no IOMMU. Can't do
efficient device I/O.

**Fix:** Add `stdlib/dma.fd` with DMA buffer allocation and channel
management.

### Tier 3: Important for a Real OS (but not day-1 blockers)

#### 16. No multi-core / SMP support

No APIC IPIs, no CPU startup sequence, no per-CPU initialization beyond
`@cpu_local`. Can't boot secondary CPUs.

**Fix:** Add `stdlib/smp.fd` with AP startup, IPI sending, and per-CPU
GDT/IDT/TSS setup.

#### 17. No PCI support

No PCI configuration space access, no device enumeration, no BAR
mapping. Can't discover or initialize PCI devices.

**Fix:** Add `stdlib/pci.fd` with config space read/write, device
enumeration, and BAR mapping.

#### 18. No ACPI support

No ACPI table parsing, no PM1a/PM1b control, no power management.
Can't do clean shutdown or power-off.

**Fix:** Add `stdlib/acpi.fd` with RSDP/RSDT/XSDT parsing and
`acpi_shutdown()`.

#### 19. No keyboard/mouse input

No PS/2 keyboard controller, no USB HID. Can't get user input.

**Fix:** Add `stdlib/keyboard.fd` with PS/2 scancode handling.

#### 20. No framebuffer/VGA text mode

No VGA text mode (0xB8000), no framebuffer, no font rendering.
Can't display output on screen (only serial).

**Fix:** Add `stdlib/vga.fd` with text mode and `stdlib/fb.fd` with
linear framebuffer rendering.

#### 21. No network stack

No NIC drivers, no TCP/IP stack, no socket interface. Can't do
networking.

**Fix:** Add network driver framework and basic TCP/IP stack.

#### 22. No ELF loader

Can't load and execute user programs. The linker can produce ELF
executables but there's no loader to parse and run them.

**Fix:** Add `stdlib/loader.fd` with ELF parser, segment mapper, and
relocation handler.

#### 23. No user-mode / ring transitions

No ring 3 support, no user-mode page mapping, no syscall dispatch.
Can't run untrusted programs.

**Fix:** Add ring 3 setup, user-mode stack, and syscall dispatch table.

#### 24. No signal handling

No signal delivery, no signal mask, no `kill`/`sigaction`. Can't do
POSIX-style process control.

#### 25. No process/thread management

`stdlib/process.fd` is 24 lines (`exit` + `system`). No `fork`, `exec`,
`wait`, `waitpid`, `kill`, `pipe`. Can't manage processes.

### Tier 4: Language/Compiler Improvements Needed

#### 26. No custom linker sections

Can't emit `.boot`, `.data`, `.bss`, `.rodata.boot` or other custom
sections. The ELF emitter has a fixed set of sections. The linker has
a fixed layout (single PT_LOAD at 0x400000, RWX).

**Fix:** Add `.section` directive support to `compiler/elf64.fd`. Add
custom linker script support to `compiler/linker.fd`. Separate `.data`
and `.bss` segments.

#### 27. No PIC/PIE generation

The linker hard-codes load address 0x400000 with a single PT_LOAD
(R+W+X). No position-independent code, no ASLR, no separate R/X and
R/W segments.

**Fix:** Add PIE code generation and proper segment separation.

#### 28. No DWARF debug info

No debug symbols, no stack unwinding info. Makes kernel debugging
extremely difficult.

**Fix:** Add DWARF .debug_info/.debug_line emission to ELF backend.

#### 29. No `@naked` functions

Can't write naked functions (no prologue/epilogue) needed for ISR
stubs, context switch routines, and boot entry points.

**Fix:** Add `@naked` attribute that skips prologue/epilogue generation.

#### 30. No `@packed` structs

Can't define packed structs for hardware register maps and protocol
headers. `@repr(C)` exists but doesn't pack to 1-byte alignment.

**Fix:** Add `@packed` attribute to `compiler/lower.fd` struct layout.

#### 31. No `usize`/`isize` types

`usize` maps to `i64` in the bootstrap. No platform-dependent pointer
sized integer type.

#### 32. Generic enums don't carry type parameters

`Option<T>` and `Result<T,E>` use `i64` payloads. Generic enum pattern
matching doesn't substitute type parameters. This blocks proper error
types for kernel code (see CB2 above).

#### 33. ARM64/RISC-V codegen not production-ready

ARM64: 374-line skeleton in `src/codegen_arm64.rs`, legacy text-only
generator in `compiler/codegen.fd`. `GlobalAddr` stubbed as `mov xN, #0`.
RISC-V: 368-line skeleton. `GlobalAddr` stubbed as `li reg, 0`.

Neither can produce working executables. Only x86-64 is viable.

#### 34. C codegen can't target freestanding

`compiler/codegen_c.fd` always emits `#include <stdio.h>`, `<stdlib.h>`,
`<pthread.h>`, `main()`, and a large runtime. Bare-metal builtins are
empty stubs. Can't produce no-libc C code.

**Fix:** Add `--freestanding` flag that skips libc includes and emits
minimal runtime.

---

## Part 4: Test Coverage for OS Features

### What's tested

| Test file | What it tests | Pass? |
|-----------|--------------|-------|
| `test_bare.fd` | `--bare` mode, `asm!("hlt")` | ✅ Pass |
| `test_boot.fd` | `--boot` multiboot2, `p_cli`/`p_cr0_read`/`p_hlt` | ✅ Pass (compile only) |
| `test_interrupt.fd` | `interrupt fn` syntax, `p_cli`/`p_sti`/`p_hlt` | ✅ Pass (compile only) |
| `test_mmu.fd` | CR0/CR3/CR4, `p_page_map`, `p_invlpg`, `p_phys_read64` | ✅ Pass (compile only) |
| `test_kernel_hello.fd` | `p_serial_init`, `serial_print`, `p_hlt` | ✅ Pass (compile only) |
| `test_kernel_serial.fd` | GDT/IDT setup, serial output, `interrupt fn timer_handler` | ✅ Pass (compile only) |
| `test_os_features.fd` | `Volatile<T>` MMIO, `interrupt fn`, pointer arithmetic, `panic_handler` | ❌ FAIL (all backends) |
| `test_tier1_kernel.fd` | Volatile, pointer arithmetic, bitfield CR0/PTE, `@interrupt`/`@panic_handler` | ❌ FAIL (all backends) |
| `test_os_gaps.fd` | `static` vars, function pointers, `@repr(C)`, bare-metal builtins, IDT/GDT constants | ✅ Pass |
| `test_serial_hello.fd` | Bare-metal serial via raw `asm!()`, Multiboot2 comment | ✅ Pass (compile only) |
| `test_asm.fd` | Inline `asm!()` syntax, I/O, fences, `hlt` | ✅ Pass |
| `test_asm_pe.fd` | Inline asm PE encoding: I/O ports, CR0/CR3, `lgdt`/`lidt`, `int`/`iretq`, `cpuid`/`rdtsc` | ✅ Pass |

### What's NOT tested

- **No QEMU/bochs boot tests** — nothing actually runs in an emulator
- **No UEFI tests** — `stdlib/uefi.fd` has zero test files
- **No real hardware tests**
- **No interrupt delivery test** — IDT entries are set but never triggered
- **No paging test** — `p_page_map` is called but never verified
- **No multi-core test**
- **No allocator test** (kernel-level, without libc)
- **No disk I/O test**
- **No actual boot test** — no binary is ever loaded by a bootloader

### Test infrastructure gaps

1. No QEMU integration in test harness
2. No `--bare`/`--boot` build verification (only compile, never run)
3. `test_os_features.fd` and `test_tier1_kernel.fd` fail on all backends
4. OS tests in `tests/bootstrap/` subdirectories are not run by `verify.sh`
5. `tests/test_tier1_kernel.fd` (root) is a stale copy not in any harness

---

## Part 5: Architecture Assessment

### Can Fides boot a kernel today?

**With the Rust bootstrap (`fidesc emit --boot`):** Yes, technically.
It can produce a Multiboot2 `.bin` file. Session 19 fixed GDT/IDT loading,
added TSS/APIC/timer/allocator/context-switch/syscall support, and ported
flat/boot generation to the self-hosted compiler. But:
- No disk, no filesystem, no network
- No QEMU/bochs boot verification
- 16-bit/32-bit code generation not supported (UEFI only)
- The kernel would print to serial and halt

**With the self-hosted compiler (`fides-c3.exe`):** Yes — Session 19
ported `generate_flat` and `generate_boot` to `compiler/pe.fd`. The
self-hosted compiler can now generate flat binaries (`--bare`) and
Multiboot2 boot binaries (`--boot`).

### What a minimal Fides OS would need (day-1 MVP)

1. **Boot:** UEFI entry point (skip BIOS/16-bit entirely)
2. **GDT:** ✅ Fixed — descriptor loading in `stdlib/kernel.fd`
3. **IDT + APIC:** ✅ Added — LAPIC init + interrupt vector setup
4. **Timer:** ✅ Added — PIT/LAPIC timer for preemption
5. **Allocator:** ✅ Added — Physical frame allocator + kernel heap
6. **Serial:** Already works (`stdlib/kernel.fd`)
7. **Context switch:** ✅ Added — Thread struct + register save/restore
8. **Syscalls:** ✅ Added — MSR setup + `syscall`/`sysret` dispatch
9. **Flat binary output:** ✅ Ported — `generate_boot` in self-hosted compiler

### Estimated effort by category

| Category | Effort | Priority | Status |
|----------|--------|----------|--------|
| Fix CB1 (annotation enforcement) | Large | Critical | ✅ Core done, 13 stubs remain |
| Fix CB2 (generic Option/Result) | Medium | Critical | ✅ Resolved |
| Port flat/boot binary output | Medium | Critical for OS | ✅ Resolved |
| Fix GDT/IDT loading | Small | Critical for OS | ✅ Resolved |
| Add TSS support | Small | Critical for OS | ✅ Resolved |
| Add IRETQ for interrupt fn | Small | Critical for OS | ✅ Resolved |
| Add APIC/LAPIC support | Medium | High | ✅ Resolved |
| Add timer (PIT/HPET) | Medium | High | ✅ Resolved |
| Add kernel allocator | Medium | High | ✅ Resolved |
| Add context switching | Medium | High | ✅ Resolved |
| Add syscall dispatch | Medium | High | ✅ Resolved |
| Add CPUID/MSR wrappers | Small | Medium | ✅ Resolved |
| Fix CB5 (self-host build fails) | Medium | Critical | Open |
| Fix CB6 (PE stubbed runtime) | Medium | High | Open |
| Add PCI support | Medium | Medium | Open |
| Add ACPI support | Medium | Medium | Open |
| Add framebuffer/VGA | Small | Low | Open |
| Add disk drivers | Large | Low | Open |
| Add network stack | Large | Low | Open |
| Add ELF loader | Medium | Low | Open |
| Add user-mode/ring 3 | Large | Low | Open |
| Add DWARF debug info | Large | Low | Open |
| Add `@naked`/`@packed` | Small | Medium |
| Add custom linker sections | Medium | Medium |
| ARM64/RISC-V codegen | Large | Low |

---

## Part 6: Recommendations

### Immediate (before any OS work)

1. **Fix CB2** — generic `Option<T>`/`Result<T,E>`. Without this, kernel
   error handling is impossible.
2. **Fix GDT/IDT descriptor loading** in `stdlib/kernel.fd` — this is a
   small fix (build 10-byte descriptor before `lgdt`/`lidt`).
3. **Add `IRETQ` for `interrupt fn`** in the PE/ELF emitter.
4. **Port `generate_flat`/`generate_boot`** from `src/pe.rs` to
   `compiler/pe.fd` so the self-hosted compiler can produce bootable
   binaries.
5. **Remove debug prints** from `compiler/parser.fd` (CB4).

### Short-term (minimal bootable kernel)

6. Add TSS support (`ltr`, TSS struct, TSS GDT entry).
7. Add LAPIC init + EOI in `stdlib/apic.fd`.
8. Add PIT or HPET timer in `stdlib/timer.fd`.
9. Add physical frame allocator + kernel heap in `stdlib/kalloc.fd`.
10. Add context switch (thread struct + register save/restore).
11. Add QEMU boot test to verify the binary actually boots.

### Medium-term (real OS)

12. Fix CB1 — port safety annotation enforcement to self-hosted compiler.
13. Add syscall dispatch (MSR_LSTAR/STAR/SFMASK setup).
14. Add PCI device enumeration.
15. Add ACPI table parsing + power management.
16. Add `@naked` and `@packed` attributes.
17. Add custom linker sections + `.data`/`.bss` separation.
18. Add DWARF debug info.
19. Add disk driver (ATA/IDE PIO at minimum).
20. Add framebuffer/VGA text mode.

### Long-term (production OS)

21. Add user-mode/ring 3 support.
22. Add ELF loader for user programs.
23. Add network stack.
24. Add SMP/multi-core support.
25. Add ARM64/RISC-V native codegen.
26. Add DMA/IOMMU support.

---

## References

- Critical blockers: `docs/roadmap/roadmap-issues.md` → "Critical Blockers"
- Self-host status: `docs/roadmap/selfhost-roadmap.md`
- OS features phase: `docs/roadmap/roadmap.md` → Phase 16
- Security features: `docs/roadmap/security-roadmap.md`
- Feature roadmap: `docs/roadmap/features-roadmap.md`
- Kernel primitives: `stdlib/kernel.fd`
- UEFI bindings: `stdlib/uefi.fd`
- Inline asm: `compiler/pe.fd:2241-2390`
- ELF64 encoder: `compiler/elf64.fd` (2,423 lines)
- Linux runtime: `compiler/runtime_linux.fd` (4,362 lines)
- Linker: `compiler/linker.fd` (1,051 lines)
- Test baseline: `AGENTS.md` → "Current Baseline"
