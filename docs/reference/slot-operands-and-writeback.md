# Slot operands, BMI1 masks and writeback addressing (TG7, TG8)

Three selections of the machine layer (`compiler/nat_ir.fd`), from the
llvm-roadmap goals TG7 (x86-64) and TG8 (AArch64). None changes a result;
each has a switch to turn it off and a mutant that breaks it on purpose so
the tests can be seen to catch a wrong rule.

## x86-64: spilled operands read and written in their slot (TG7)

After allocation, while the MIR is emitted (`ng_mir_emit_pass`), a value
the allocator put in a stack slot used to be loaded into a scratch
register before each use and stored back after each definition. Three
post-allocation rules (`ng_post_rules`) now use the slot as the operand:

| rule | MIR | code | switch |
|------|-----|------|--------|
| `memop` (earlier) | `ADD/SUB/MUL/AND/OR/XOR d, a, s`, s spilled | `op d, [rsp + off]` | `FID_NAT_OFF=memop` |
| `cmpslot` | `BRCC a, b` or `SETCC d, a, b`, one side spilled | `cmp r, [rsp + off]` + jcc / setcc | `FID_NAT_OFF=cmpslot` |
| `rmwslot` | `ADD/SUB/AND/OR/XOR/ADDI d, a, b` with d and a in one slot | `op [rsp + off], r` / `op qword [rsp + off], imm` | `FID_NAT_OFF=rmwslot` |

`cmpslot` (`ng_cmp_slot_field`, `ng_x64_cmp_slot`): when the slot is the
right side the compare is as written; when it is the left side the compare
kind is swapped (`ng_cc_swap`), since `cmp r, [m]` puts the register on the
left; against the zero register it is `cmp qword [rsp + off], 0`, the slot
on the left. Not for a rematerialized value (a constant or an address,
which has no slot).

`rmwslot` (`ng_rmw_slot`, `ng_x64_rmw_slot`): the destination and the left
operand must be spilled to the same slot (one vreg, or two coalesced vregs
that share it, MI8), neither rematerialized; MUL has no memory destination;
`x op x` keeps the load (the slot would be read after being written); an
ADDI's constant must fit 32 bits signed. The right operand is a register,
or a vreg in another slot loaded into a scratch register first. Where it
took a load, the operation and a store, it is one instruction.

## x86-64: BMI1 BLSMSK and BLSI (TG7)

`ng_fuse_bits` selected `a & (a - 1)` as BLSR under `--cpu x86-64-v3|v4`.
MO_BLSR now has a kind in i0:

| i0 | source pattern | instruction | switch |
|----|----------------|-------------|--------|
| 0 | `a & (a - 1)` (also `a + -1`) | BLSR | `FID_NAT_OFF=blsr` |
| 1 | `a ^ (a - 1)` | BLSMSK | `FID_NAT_OFF=blsmsk` |
| 2 | `a & -a`, `a & (0 - a)` | BLSI | `FID_NAT_OFF=blsi` |

Without BMI1 (never selected there, but kept correct) `ng_blsr` computes
`a - 1` or `-a` in r11 / ad2 and ANDs or XORs. The extension-state pass
(`ng_es_def`) keeps a zero-extension bound for BLSR and BLSI (the result
is at most the operand) but not for BLSMSK (0 gives all ones).
`FID_NAT_MUTATE=blsx` selects the wrong kind. Encoders: `x64_blsmsk`
(VEX.LZ.0F38.W1 F3 /2), `x64_blsi` (F3 /3) in `compiler/nat_enc.fd`.

## AArch64: loads and stores with writeback (TG8)

A pointer stepped by a constant in a loop

```
while a < end {
    var w: *mut i64 = a;
    unsafe { x = *w; }
    s = s + x;
    a = a + 8;
}
```

becomes `ldr x, [a], #8` (post-indexed) instead of `ldr x, [a]` and
`add a, a, #8`. Two parts:

1. **MIR, before allocation** (`ng_wb_pair`, after the add-immediate
   selection): the step `ADDI p2, p, c` (c in -256..255, not 0) moves up to
   right after the load or store through p (or a copy of p: `ng_fb_same`) at
   offset 0 or c, over the operations between when none of them names p or
   p2 and they are one straight stretch (no label, branch or call; line rows
   are passed). The lowering's two-address step `MOV p2, q; ADDI p2, p2, c`
   moves as the one operation `ADDI p2, q, c`. Only p2's definition comes
   earlier; nothing between read or wrote it.
2. **At emission, after allocation** (`ng_a64_wb_fuse`, called after each
   LOAD, STORE and ADDI): the last two instructions, decoded from the code,
   are one access with writeback when they are straight code (no label
   placed at or after the second, no line row from the second on, no
   fixup), the access is a general-register `ldr/ldrs/str` with an unsigned
   offset, t is not n, n is not sp and the step fits 9 bits:

   | before | after |
   |--------|-------|
   | `ldr/str t, [n]; add n, n, #c` | `ldr/str t, [n], #c` |
   | `ldr/str t, [n, #c]; add n, n, #c` | `ldr/str t, [n, #c]!` |
   | `add n, n, #c; ldr/str t, [n]` | `ldr/str t, [n, #c]!` |

   (`sub n, n, #c` is a step of -c.) Both forms leave every register and
   memory as the two instructions did, so the rewrite holds whatever MIR
   produced them; the allocator decides whether it applies by giving p and
   p2 one register (it does when nothing reads p after the step).

The writeback form is chosen at emission rather than as a MIR operation of
its own because a load with writeback defines two registers (the value and
the base) and a MIR operation defines one (`mi_defines`); the madd/msub
fusion (`ng_a64_mac`) is chosen the same way.

Switch: `FID_NAT_OFF=wbidx` (both parts). `FID_NAT_MUTATE=wbidx` swaps
post- and pre-indexing. Encoders: `a64_ldr_wb`, `a64_str_wb`
(`a64_wb_word` over `a64_ldst_opc`) in `compiler/nat_enc.fd`.

## Tests

- `tests/nattools/tg78_unit.fd` (`fid run`): `nat_tg78_unit_check` in
  nat_ir.fd, the decisions and bytes of every rule above, with refusals.
- `tests/bootstrap/test_machine_tg7.fd`, `test_machine_tg8.fd`: programs
  whose output must not change with the rules on or off, on every target.
- `tests/nattools/machine_tg78_check.sh`: builds both with `FID_VERIFY=1`,
  counts the new instructions in the images (on, off, by switch), runs the
  images (WSL / qemu-user / `--nat`) and the mutants.
- `tests/nattools/enc-oracle-tg78.py`: the new encoders against clang.
