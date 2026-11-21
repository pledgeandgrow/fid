# Jump tables in the machine layer (MI27)

A `match` on an integer or an enum tag reaches the IR as a chain of
compares; `IrFunction_form_switches` (compiler/ir.fd, ME28) turns a chain of
4 or more keys on one value into `Inst::Switch(value, keys, targets,
default)`. The machine layer (compiler/nat_ir.fd) lowers a Switch in one of
two ways:

- **a jump table** when `ng_jt_span` says so: the machine IR is being
  recorded (not the direct path, `FID_NAT_OFF=mir`), `FID_NAT_OFF=jtab` is
  not set, there are at least 4 keys, they fill at least 40% of their span
  (max - min + 1), the span is at most 1024, and the switch is not one the
  bit tests already do in one AND per block (a span of at most 64 values
  going to at most 3 blocks);
- otherwise `ir_switch_plan`'s tree: equality tests, bit tests and a
  balanced tree of signed compares.

## Machine IR

`ng_gen_jtab` records

```
sub   ix, x, lo                (only when lo != 0; wrapping)
jtab  ix, i0 = default label, i1 = n      MO_JTAB
jte   i0 = label of index 0                MO_JTE
...
jte   i0 = label of index n - 1
jmp   default                              (ends the chain; often dropped)
```

An index with no key goes to the default; of equal keys the first wins, as
in the tree. Both `jtab` and `jte` are conditional branches to their `i0`
for every pass (`mi_is_cond_br`): every target is a successor in
`mi_blocks`, so liveness and the allocator see each one, and every label
they name is referenced for the passes that count references and for the
verifier. Each entry is a one-instruction block falling into the next: a
fall-through that never runs (the dispatch jumps through the table, whose
entries are data), which only makes liveness a little larger.

Nothing may come between a dispatch and its entries. The passes that put
code on a fall-through or after a block's last instruction, or move
blocks, leave a function with a table alone (`mi_has_jtab`): live-range
splitting (RA4), shrink-wrapping (RA12), block parameters (MI1), the cold
layout (WP17/OP15) and select formation (MI24). The MIR verifier
(`mir_verify`, `FID_VERIFY=1`) checks that a `jtab` of `n` entries is
followed by exactly `n` `jte`s and that no `jte` stands alone.

## Code

`ng_jtab_emit` uses only the address scratch registers (ad, ad2); the
table follows the indirect jump. Each entry is 4 bytes: its target's
offset from the entry itself (`NATFIX_SELF32`, compiler/nat_enc.fd). That
is position-independent, so a PIE, a PE (`--nat` Windows), a Mach-O image
and an object file need no relocation for it.

| target   | sequence |
|----------|----------|
| x86-64   | `mov r11, n-1; cmp ix, r11; ja dflt; lea r10, [rip+T]; lea r10, [r10+ix*4]; movsxd r11, [r10]; add r10, r11; jmp r10; T:` |
| AArch64  | `mov x17, n-1; cmp ix, x17; b.hi dflt; adrp/add x16, T; add x16, x16, ix, lsl #2; ldrsw x17, [x16]; add x16, x16, x17; br x16; T:` |
| RISC-V   | `li t5, n-1; bgeu t5, ix, +8; jal dflt; auipc/addi t0, T; slli t5, ix, 2; add t0, t0, t5; lw t5, 0(t0); add t0, t0, t5; jr t0; T:` |

The bounds check is unsigned after the subtraction, so every value outside
`[lo, lo + n)` (the extreme i64 values included) goes to the default. The
dispatch and its entries keep one form (branch relaxation leaves them
alone).

## Switches

A `match` on a narrow integer (`u8`, `u16`, `u32`, `i8`, `i16`, `i32`)
forms a Switch like one on an `i64` (ME28, `ir_sw_word` in
`compiler/ir.fd`). Every backend holds a narrow value in its canonical
form (zero-extended when unsigned, sign-extended when signed) and compares
the whole word, so the table, the tree and the compare chain all give the
same result. A key outside the type's range never matches.

- `FID_NAT_OFF=jtab`: no jump tables (the tree).
- `FID_SWITCH=off`: no Switch at all (the compare chain).
- `FID_NAT_DUMP=<function>` (or `*`): the MIR shows `jtab` / `jte`.

## Tests

- `tests/bootstrap/test_machine_jtab.fd`: tables from 0, from a positive
  base with holes, from a negative base, out-of-range and extreme values,
  enum tags, values live across the table, a table in a loop, 64 entries;
  a sparse switch and a bit-test switch for comparison.
- `tests/nattools/machine_jtab_check.sh`: every Linux target and `--nat`
  Windows, default / `FID_NAT_OFF=jtab` / `FID_SWITCH=off` and a PIE, with
  `FID_VERIFY=1`; which functions hold a table; the indirect jump in the
  x86-64 binary. Nov 21 (roadmap MI27 check): it fails 5 checks because
  `digit_name` and `live_across` are now inlined into `main`, and with
  inlining off `bits3` gets a table the check says it must not; the tables
  themselves work (test_machine_jtab is right on every target and PIE).
- `tests/bootstrap/test_irswitch_narrow.fd` and
  `tests/nattools/machine_switch_narrow_check.sh` (ME28): switches on
  `u8`, `u16`, `i16`, `u32` and `i32` values, on the C path (a C `switch`
  in each function) and on every machine target (tables in the dense
  ones), with `FID_SWITCH=off` to compare.
