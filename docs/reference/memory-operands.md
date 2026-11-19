# Memory operands and struct results in the machine layer

This page covers two parts of `compiler/nat_ir.fd`, the machine layer behind
`fid emit` for the Linux targets and `--nat` Windows: memory operands
(competitive-roadmap OP18) and struct results returned in registers
(OP12, llvm-roadmap ME27 / LF24). All of these passes run on the MIR
before register allocation. Each one has a switch, so a build can be
compared with and without it.

## Memory operands (OP18)

| Fold | Targets | MIR | Switch |
|------|---------|-----|--------|
| A load read once by an ALU operation (`add r, [m]`) | x86-64 | `MO_ALUM` (`ng_fuse_ldop`) | `FID_NAT_OFF=ldop` |
| A load read once by a compare, a branch on it or a SETCC (`cmp r, [m]`; `cmp size [m], imm`) | x86-64 | `MO_BRCCM`, `MO_SETCCM` (`ng_fuse_memcmp`) | `FID_NAT_OFF=memcmp` |
| `p.f = p.f op x` with op add, sub, and, or, xor (`op [m], r`; `op qword [m], imm`) | x86-64 | `MO_ALUMST` (`ng_fuse_rmw`) | `FID_NAT_OFF=rmw` |
| An extension of a load read once, done by a narrower load (`movzx`/`movsx`, `ldrb`/`ldrsh`, `lbu`/`lh`) | all | `MO_LOAD` / `MO_LDX` of 1, 2 or 4 bytes (`ng_fuse_ldext`) | `FID_NAT_OFF=ldext` |
| A multiply whose product an add or sub reads (`madd` / `msub`) | AArch64 | applied when the code is emitted (`ng_a64_mac`) | `FID_NAT_OFF=madd` |

Every fold follows the same rules (the `ng_fb_*` helpers). Between the
load and the operation that takes it there may only be vreg moves,
constants and line rows. So no store or call comes between them, and the
memory is read where the load used to be, with the same contents. The base
register must not be written in between. No other instruction may read
the loaded value: `ng_fb_deletable`, or `ng_fb_deletable_def` for the
two-address forms (`MOV d, t; EXT d, d`) whose result register also
carries the load. A checked add reads its operands a second time for the
overflow test, so it keeps its load. Only wrapping arithmetic and the bit
operations become read-modify-writes.

A compare against a constant works on the field's own bytes, and
`ng_memcmp_cc` adjusts the compare kind to the field. A sign-extended
field takes equality or a signed kind, with a constant inside the field's
signed range. A zero-extended field takes equality or an unsigned kind:
a signed kind becomes the unsigned one, and the constant must lie in the
field's unsigned range. An 8-byte field takes any kind, with a constant
of 32 signed bits.

Narrow loads rely on the byte order: all three targets are little-endian,
so the low bytes of a loaded value are the first ones in memory.

`FID_NAT_MUTATE=memcmp|rmw|ldext` makes each fold wrong on purpose, which
shows that a test really takes the fold.

Checks:

- `tests/nattools/memop_unit.fd`: the folds on small MIR sequences.
  `FID_VERIFY=1` builds run the same checks, from `nat_sel_check`.
- `tests/bootstrap/test_machine_memop.fd` and
  `tests/nattools/machine_memop_check.sh`.
- `tests/nattools/enc-oracle-op18.py`: the encoders compared with clang.
- For `MO_ALUM`: `tests/bootstrap/test_machine_ldop.fd` and
  `tests/nattools/machine_batch9_check.sh`.

## Struct results in two registers (OP12)

Some functions return a struct of one or two words, such as a pair, an
`Option` or a `Result` of a word. When every caller of such a function
calls it directly, the function returns the struct in argument registers
0 and 1 (`ng_rr_plan`, `ng_rr_analyze`, `ng_rr_callee_rewrite`;
`FID_NAT_OFF=retregs`). On the caller's side, `ng_rr_caller_rewrite`
stores the two registers into the result slot, and `ng_rf_promote` then
keeps that slot in vregs (`FID_NAT_OFF=retfwd`), together with the
locals the slot is copied to:

- the stores after the call become moves from the two registers;
- a field load becomes a move, or a shift and an extension for a narrow
  field;
- a field store becomes a masked insert;
- a copy between two such slots becomes moves.

A slot qualifies only when the pass can see everywhere its address goes.
Every vreg that holds an address into it may only be moved, offset by a
constant, loaded through, stored through, copied, or passed as the
hidden argument of such a call. The callee no longer reads that argument.

Once a slot is kept in vregs, its address is not taken at all (LF24): the
slot's `slotaddr` becomes a constant 0, which the callee gets as its
unread hidden argument, and the moves of it are removed as dead. A `match`
on a returned `Option<i64>` or `Result<i64, i64>` then reads the tag and
the payload straight from the two result registers, with no load, store or
slot address left. A `Result` whose payload owns memory (a `str` error) is
dropped at the end of its statement through a call that takes the slot's
address, so that slot stays in memory. A struct of more than two words
keeps the hidden result pointer.

Checks:

- `tests/nattools/retfwd_unit.fd`. `FID_VERIFY=1` builds run the same
  checks.
- `tests/bootstrap/test_natabi_retfwd.fd`, `test_natabi_retregs.fd` and
  `tests/nattools/machine_retfwd_check.sh`.
- `tests/bootstrap/test_natabi_optmatch.fd` and
  `tests/nattools/machine_optmatch_check.sh` (LF24): `index_or`, `div_or`
  and `count_found` have no memory operation or `slotaddr` in their final
  MIR; with `FID_NAT_OFF=retfwd` they do.
