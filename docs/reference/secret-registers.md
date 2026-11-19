# Secrets in registers on the machine layer (LF20)

Roadmap: docs/roadmap/llvm-roadmap.md, LF20, "`@secret` values never
spilled to memory unencrypted where a register will do, and their spill
slots cleared". This page covers the machine layer (compiler/nat_ir.fd:
the Linux targets, macOS and `--nat` Windows). The pe_ir side is
`PeirGen_wipe_frame` in compiler/pe_ir.fd.

## Which functions

A function holds a secret (`NatFnRec.secret`) when the IR says so
(`IrFunction.has_secret`: a `@secret` / `@sensitive` function, parameter or
local, or a `@zeroize` local), when it has `@zeroize` locals, or when it is
`@constant_time`.

## What such a function does at every return

1. **Frame**: `ng_wipe_frame` zeroes `[sp, sp + save_off)`. That is every
   value slot, spill slot, local and outgoing argument. Not the register
   save area, which holds the caller's values. Switch: `FID_NAT_OFF=wipe`.
2. **Registers**: `ng_wipe_regs` zeroes each register the internal calling
   convention lets any call clobber, as pe_ir clears rcx, rdx, r8-r11 and
   xmm0-xmm5. Switch: `FID_NAT_OFF=wiperegs`.

   | target  | integer                          | float / vector                       |
   |---------|----------------------------------|--------------------------------------|
   | x86-64  | rax rcx rdx rsi rdi r8-r11       | xmm0-xmm15 (`pxor`)                  |
   | AArch64 | x0-x17 (never x18)               | v0-v7, v16-v31 (`fmov d, xzr`)       |
   | RISC-V  | t0-t6, a0-a7                     | ft0-ft7, fa0-fa7, ft8-ft11 (`fmv.d.x`) |

   These registers are never zeroed:
   - argument register 0, which holds the result;
   - argument register 1 when the function returns a two-word struct in
     registers (ME27, `g.rr_size` 16);
   - a caller-saved *pool* register (AArch64 x8-x11, RISC-V t4) that the
     function's clobber mask leaves out. IPRA callers keep their own values
     there across the call (`ng_call_mask`). The function and its callees
     never wrote that register, so it holds none of the function's secrets.

   The callee-saved float registers (AArch64 d8-d15, RISC-V fs0-fs11,
   RA18) come back from the save area. On `--nat` Windows with float
   registers off (`FID_NAT_OFF=winfregs`) only xmm0-xmm5 are zeroed. The
   vector registers of RISC-V's V extension are not zeroed.

   A leaf function (WP8, no frame) zeroes its registers too.

   Such a function makes no sibling-call jumps (`ng_tail_fn_ok`). A jump
   would leave the callee's arguments in their registers.

## Secret values spill last

In a secret function, the values that carry a secret get `1 << 40` added
to their spill weight (RA5). That puts them above every other value, so
these allocators give them a register first and spill them only when
nothing else is left to spill:
- the linear scan;
- the greedy allocator (RA9);
- the second chance in holes (RA2);
- the free argument registers (RA3);
- the float pass (MI25).

The secret values (`ng_secret_ids`) start from the marks the IR carries for
each value. Today those are the slots of `@zeroize` locals
(`IrFunction.zeroize_locals`). From these marks the set grows to a fixed
point over the function:
- a value loaded from a secret place is secret;
- a value stored into a secret place is secret;
- a place a secret is stored into is secret;
- a value computed from a secret operand is secret.

A piece of a split value (RA4) is as secret as the value itself. A
rematerialized value is left alone, because it is recomputed and never
stored. Switch: `FID_NAT_OFF=secretregs`. The weights are only used when
spill weights are on (`weights`); the fast allocator (RA14) does not use
them.

Not yet marked: `@secret` / `@sensitive` parameters and locals that are not
`@zeroize`. The IR records that the function holds them
(`has_secret`), but not which values they are. Marking them needs a value
list from lowering: `IrFunction.secret_values`, filled by compiler/lower.fd
for those parameters' values and those locals' slots. `ng_secret_ids` then
takes those values as seeds too.

## Checking it

`fid emit <file> --target <t> --stats` prints, for each secret function:

    [lf20] <fn>: S secret values, R in registers; each return zeroes W registers

`tests/nattools/machine_lf20_check.sh [fid]` runs
`tests/bootstrap/test_machine_secret_regs.fd` on x86_64, AArch64, RISC-V
and `--nat` Windows. It checks:
- under register pressure all secret values keep a register (R == S);
- with `FID_NAT_OFF=secretregs`, fewer of them do;
- a leaf zeroes its registers;
- `FID_NAT_OFF=wiperegs` zeroes none;
- the outputs are equal in all builds;
- the disassembly shows the zeroing.
