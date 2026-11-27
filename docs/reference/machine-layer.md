# Machine layer notes

Consolidated Nov 22: instruction selection, memory operands, register allocation, post-allocation rewrites and validation in `compiler/nat_ir.fd`/`nat_enc.fd` (TG1/TG2, RA4, RA14, RA19, TG17, RL6, LF20).

## Instruction selection as rules (machine layer)

llvm-roadmap TG1 and TG2. The machine layer (`compiler/nat_ir.fd`, the
Linux AArch64 / RISC-V 64 / x86-64 targets and `--nat` Windows) selects
target instructions for its machine IR (MIR) operations from a table of
rules written as Fides data, not from hand-written branches per target.

### The table

`ng_isel_rows` and `ng_isel_rows_tg1` build the rows; `ng_isel_build`
groups them by `(arch, op)` and sorts each group by cost (stable). A row is

    [arch, op, form, cost, pre, arg, parg]

- `form` names the target instruction or short sequence (`NG_IF_*`), and
  `arg` is the form's own operand (an ALU kind, a test-against-zero flag,
  the swap / unsigned / invert bits of a RISC-V compare).
- `pre` is the precondition (`NG_IP_*`) and `parg` its operand: an
  immediate class (`IMM_U32`, `IMM_S32`, `IMM_S12`, `IMM_MOVZ`,
  `IMM_MOVN`), a compare kind (`CC`), the second operand being the zero
  register (`B_ZERO`, `ZERO_CC`), a branch in short reach (`SHORT`), a
  displacement the target's load / store immediate holds (`A64_UIMM`,
  `DISP_S12`), an indexed form the target has (`INDEXED`), float
  registers (`FREG_MEM`, `FREG_OP`), an element scale (`SH_IDX`,
  `SH_ZERO`, `SH_ZBA`).
- `cost` orders the rules of an operation: instructions emitted, cheapest
  first. The first rule whose precondition holds and whose form does not
  decline is taken (`ng_isel_mi`).

`ng_mi_dispatch` tries the table first for every operation
`ng_isel_covers` names; the hand-written selection (`ng_li`, `ng_load`,
`ng_brcc`, `ng_fbin`, ...) runs when no rule applies and with
`FID_NAT_OFF=isel`.

### What the table covers

| Family | MIR | Rules |
|---|---|---|
| ALU | ADD, SUB, MUL, AND, OR, XOR, SHL, SAR, SHR, ADDI | x86-64 two-operand forms, `add r, imm` / LEA; AArch64 MADD / MSUB folding, three-register forms, ADD / SUB #imm; RISC-V R-type, ADDI |
| Constants | LI | x86-64 `xor r32` / `mov r32, imm32` / `mov r/m64, simm32` / `movabs`; AArch64 MOVZ / MOVN (one 16-bit piece) / MOVZ+MOVK sequence; RISC-V ADDI / LUI+ADDIW / longer chain |
| Shifts by a constant | SHI (new, code 56) | x86-64 `shl/sar/shr r, imm8`; AArch64 LSL / ASR / LSR #imm (UBFM / SBFM); RISC-V SLLI / SRAI / SRLI |
| Element addresses | ADDSHL | x86-64 LEA with a scaled index (or through r11 past scale 8); AArch64 ADD lsl; RISC-V ADD, Zba shNadd, SLLI+ADD |
| Compares | SETCC, BRCC | x86-64 TEST / CMP + SETcc / Jcc; AArch64 CMP + CSET, CBZ / CBNZ, CMP + B.cond; RISC-V SLT(U) with swap and XORI per kind, SUB + SLTIU / SLTU, B<cc> short or inverted over JAL |
| Loads, stores | LOAD, STORE, LDX, STX | float register forms; x86-64 `[base + disp32]`, SIB; AArch64 scaled unsigned offset or the displacement through a scratch register, register offset; RISC-V 12-bit offset or through a scratch register; ADDSHL then the access where no indexed form applies |
| Negation, extensions | NEG, NOT, EXT | x86-64 NEG / NOT (after a move), MOVSX / MOVZX / MOVSXD / `mov r32` by kind; AArch64 NEG, MVN, SXTB..UXTW; RISC-V SUB from zero, XORI -1, Zbb / Zba one-instruction extensions or ADDIW / ANDI, else SLLI + SRAI / SRLI |
| Floats | FBIN, FCMP | float registers (MI25), else the bits through the scratch pair: SSE2, AArch64 FP, RISC-V D |
| Vectors | VBIN | SSE2 / AVX2 / NEON / RVV (TG11, `docs/reference/vector-mir.md`) |
| Division, high product | DIV, REM, MULH (i0 != 0: unsigned) | x86-64 IDIV / DIV / IMUL / MUL over rax:rdx (CQO or rdx = 0 first, the other operand in r11); AArch64 SDIV / UDIV, SDIV + MSUB for the remainder, SMULH / UMULH; RISC-V DIV(U) / REM(U) / MULH(U) |
| Sign mask | ASR63 | x86-64 SAR 63 (after a move); AArch64 ASR #63; RISC-V SRAI 63 |
| Conversions | I2F, F2I (i0 != 0: f32) | float registers (MI25) first; x86-64 CVTSI2SD / CVTTSD2SI (xmm0 reused when it holds the source); AArch64 SCVTF / FCVTZS; RISC-V FCVT.D.L / FCVT.L.D |
| Fused bit operations | ANDN, BLSR (kinds BLSR / BLSMSK / BLSI) | x86-64 BMI1 under `--cpu x86-64-v3` / `v4`, else through r11; AArch64 BIC, NEG [+ MVN] then AND / EOR; RISC-V Zbb ANDN under `--cpu`, else XORI + AND; ADDI / SUB then AND / XOR |
| Select | CSEL | x86-64 TEST + CMOVcc; AArch64 CMP + CSEL; RISC-V Zicond (`--cpu rva23u64`) CZERO + OR, else a branch over a move; nothing when the destination is the value |
| Test bit and branch | TBNZ | x86-64 TEST imm / BT + Jcc (rel8 when short); AArch64 TBZ / TBNZ when short, else the inverse over B; RISC-V the bit to the sign then BLT / BGE, over JAL when long |
| 32-bit operations | OPW (RISC-V only) | ADDW / SUBW / MULW |
| Memory operand | ALUM (x86-64 only) | `op r, [base + disp]` after a move; through r11 when the destination is the base |

MO_SHI is formed by `ng_fuse_addi` from a SHL / SAR / SHR whose count is a
known constant (taken mod 64, as the register forms do). `FID_NAT_OFF=shi`
keeps the register forms.

### Checks (TG2)

Under `FID_VERIFY=1`, once per build (`nat_sel_check`, RL6):

- every case of `nat_enc.fd nat_sel_cases` (LI, the ALU, ADDI, SHI,
  ADDSHL, SETCC, NEG, NOT, EXT, ...) is selected and run by the selection evaluator
  on edge and random values against the operation's meaning, and its
  bytes must equal the hand-written selection's;
- `nat_isel_mem_check`: loads and stores (every size and signedness,
  displacements inside and outside each immediate, the stack pointer as a
  base, an index no indexed form takes), compare-and-branch (every kind,
  against a register and zero, short and long) and float operations (f64,
  f32, float registers under `FID_NAT_ON=fregs`) give the same bytes from
  the table as from the hand-written selection;
- `nat_isel_rule_check`: every rule of an operation the evaluator runs is
  tested on its own: random registers and constants are drawn until the
  rule is the one the table would select (its precondition holds and no
  cheaper rule's does), then that rule's form alone and the generic
  lowering (the hand-written selection) are both run on values against
  the operation's meaning. A rule no draw reaches is an error. `--stats`
  prints `[isel] TG2: N rules of the table checked alone on random
  operands`.

Mutations (`FID_NAT_MUTATE`, never in a real build) show the checks see
the table: `isel` (SUB's operands swapped), `iselimm` (the constant of
LI / ADDI / SHI rules off by one), `iselcc` (SETCC / BRCC rules on the
inverse condition), and `shi` (the fused count off by one).

Program-level tests: `tests/bootstrap/test_machine_isel.fd`,
`tests/bootstrap/test_machine_isel_tg1.fd`, the `shi`, `li`, `cmp` and
`mem` lines of `tests/bootstrap/test_machine_rules_random.fd`. Scripts:
`tests/nattools/isel_table_check.sh`, `tests/nattools/isel_rules_check.sh`,
`tests/nattools/rule_mutation_check.sh`.

The rows of the last families (`ng_isel_rows_tg1b`, forms in
`ng_isel_emit_tg1b`, Nov 17) are checked by `nat_isel_tg1b_check` under
`FID_VERIFY=1`: each against the hand-written selection, byte for byte,
for every register shape that changes the code (the destination equal to
an operand, x86-64's rax / rdx as operands, the base as the destination),
signed and unsigned, f64 and f32 in general and float registers, two F2I
of one source in a row (the xmm0 cache), each BLSR kind, both select
senses, bits 0, 7, 31, 32 and 63 short and long, and again under each
`--cpu` that changes a rule (x86-64-v3, rva23u64). The value evaluator
does not run these operations, so `nat_isel_rule_check` leaves them to
this comparison. Mutation `iseldiv` (the division rules take the other
result register or signedness) shows the comparison sees the rows.
Program test: `tests/bootstrap/test_machine_isel_tg1b.fd`; script:
`tests/nattools/isel_tg1b_check.sh` (byte identity of the images with
`FID_NAT_OFF=isel` on the machine tests, each target and `--cpu`, float
registers off and on).

### Not covered by rules yet

Moves, calls, jumps and jump tables, BNEZ / BEQZ, PANICIF / TRAP, the
prologue and epilogue, slot and incoming / outgoing argument accesses
(SLOTADDR, LDSLOT, STSLOT, LDIN, STOUT), COPY, F32BITS, OP18's BRCCM /
SETCCM / ALUMST and the vector operations other than VBIN: these keep
their hand-written selection. AArch64's MADD / MSUB rules are checked by
byte comparison only (they fold the multiply emitted before them, which a
rule run alone does not have).

## Memory operands and struct results in the machine layer

This page covers two parts of `compiler/nat_ir.fd`, the machine layer behind
`fid emit` for the Linux targets and `--nat` Windows: memory operands
(competitive-roadmap OP18) and struct results returned in registers
(OP12, llvm-roadmap ME27 / LF24). All of these passes run on the MIR
before register allocation. Each one has a switch, so a build can be
compared with and without it.

### Memory operands (OP18)

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

### Struct results in two registers (OP12)

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

## Live-range splitting in the machine layer (RA4)

The machine layer (`compiler/nat_ir.fd`: Linux x86-64, AArch64, RISC-V,
macOS, `--nat` Windows) gives each value one register or one frame slot for
its whole live range (`ng_mir_alloc`: linear scan over hulls, coalescing,
spill weights, rematerialization, second chance over holes, argument
register hints). A value that loses gets a slot everywhere: every read is a
load, every write a store, also inside a hot loop that has registers to
spare. Live-range splitting lets such a value have a register where it is
used most and the slot elsewhere.

### What is split

`ng_mir_alloc_split` allocates the function once. The values that spilled
(integer, f64 or f32 class, not rematerialized, not the hidden result
pointer) are the candidates of `ng_split_plan`:

- **Around loops.** For each candidate, the largest natural loops (one per
  head that some later block jumps back to: the blocks that reach such a
  jump without passing the head, so every way in from outside enters at
  the head; the entry block may not be in it) with no call, prologue or
  epilogue that read or write it, at most 128 loops a function. Inside such a loop the value is a new vreg, a
  piece. The piece is loaded on each edge that enters the head from
  outside (the preheader) when the value is live there, and, when the loop
  writes the value, stored on each edge that leaves the loop to a block
  where the value is live (the exits).
- **At calls.** Elsewhere (outside the value's own loop pieces), block by
  block, each run of two or more
  instructions that read or write the value with no call, prologue or
  epilogue between them is a piece: loaded before the first instruction
  when it reads the value, stored after the last write when the value is
  live after the run.

A piece is joined to its value by ordinary moves (`MO_MOV`). The second
allocation runs on the split function with these moves kept out of the
coalescer (`ng_split_pair`), so each piece takes a register or slot of its
own: a piece in a call-free loop can take a caller-saved register (AArch64
x8-x11, RISC-V t4), and its spill weight is its own loop's.

#### Floats (MI14)

No float register is callee-saved on any target (the float pool, MI25:
x86-64 xmm2..xmm11, `--nat` Windows included since MI14; AArch64
v16..v27; RISC-V ft2..ft11), so a float value that a call falls inside
spills for its whole range (`ng_float_assign`). Since MI14 such values
are candidates too: the pieces between two calls and in a call-free loop
take float registers, and the value itself keeps the slot across the
calls. Each piece's class comes from its own operations
(`ng_mir_classes_c` counts the float operands of a copy class, moves not
counted), so a piece of a float value is a float, and the value, left
with only the moves, may become an integer; the moves between them cross
classes where they must (`ng_fmov_any`: `movq`/`movd`, `fmov`, `fmv`; an
f32 as its 32 bits zero-extended). `FID_NAT_OFF=splitfloat` keeps the
candidates to integers.

### Where the moves go

The moves of one place form one parallel move (`nat_enc.fd`
`nat_par_moves`), in four phases: a piece's store after its last write,
stores at loop exits, loads at loop entries, a piece's load before its
first read. For an edge from block P to block S:

- P ends in an unconditional jump to S: before the jump;
- P falls through into S: after P's last instruction (only that way runs
  it, since S's label comes after);
- P ends in a conditional branch to S: an edge block. The branch goes to a
  new label; the block (label, moves, jump to S) is put where nothing falls
  into it: right before S when the instruction before S never goes on (the
  block then falls into S, no jump), else after the nearest earlier such
  instruction, else after a later one (the function's end at the latest).

### Kept only when cheaper

`ng_split_cost` estimates both allocations: each slot access of a spilled
value counts 2, each move between two registers and each jump 1, each
weighted 8^(loop depth) as the spill weights are, plus 2 per callee-saved
register saved. The split is kept when it costs less; otherwise the first
allocation stays and nothing changes.

A value none of whose pieces took a register in the second allocation
gained only moves (the pieces of one loop crowd each other out of a small
caller-saved pool). The function is then split once more without such
values and allocated a third time; the cheaper of the two splits is the
one compared with the first allocation (`FID_NAT_OFF=splitprune`: the
first split only; never with `splitall`). `fid emit --stats` prints
`[split] <fn>: N pieces (L around loops), E edge blocks, [V values left
whole, ]cost A -> B, kept` (or `not kept`). The allocations that may be thrown away print nothing; the
one kept is run again when `--stats` or `FID_NAT_DUMP` asked for output.

### Checking

With `FID_VERIFY=1` the split function goes through the MIR verifier
(`ng_mir_verify`), the allocation checker (`ng_mir_check`, which sees the
pieces as ordinary vregs and their moves as ordinary moves) and the split
checker `ng_split_check`, run from `ng_mir_check` while a split function is
allocated. The split checker proves that the split function is the original
one but for renamed operands (each a piece of the operand's own value),
conditional branches sent to edge blocks that go on to the old target, and
inserted moves between a value and its pieces; and, by a forward
must-analysis over the split function's blocks, that every read names a
vreg holding the value's current contents on every way there (a write
makes its vreg the only holder, a move copies its source's state). On
entry to the function only the values themselves hold their contents (a
parameter, or nothing yet); a piece holds nothing until a move or a write
gives it the value, so a load missing before a loop or a run is caught as
well as a store missing at an exit.

### Switches

- `FID_NAT_OFF=split`: no splitting (the allocation of before).
- `FID_NAT_OFF=splitprune`: no second split without the values whose
  pieces all spilled.
- `FID_NAT_OFF=splitfloat`: integers only (MI14: by default float values
  that spilled are split too).
- `FID_NAT_ON=splitall`: every value is a candidate and the split is
  always kept: a stress test of the planner and the checker.
- `FID_NAT_OFF=splitfloat`: integer candidates only (MI14).
- `FID_NAT_MUTATE=split`: the stores at loop exits are left out; a
  `FID_VERIFY=1` build must then fail with the split checker's message.

### Tests

- `tests/nattools/split_unit.fd` (`fid ccompile`, then run the program:
  under `fid run` the interpreter runs all of `compiler::nat_ir` and the
  random programs take hours): the planner and the checker
  on hand-made machine IR, each function run before and after the split by
  a small interpreter; then 40 random programs (calls, nested loops, loops
  left from the middle, if and if/else) with every vreg a candidate: each
  split must pass the checker and compute the same, and each mutated split
  (no stores at loop exits) that computes something else must be rejected.
  `noload` / `nostore` leave the load before a loop or the store at its
  exit out of a split (`nat_split_unit_drop`): both must be rejected; then
  twelve more random programs, each split with one inserted move left out
  at a time: no such split that computes something else may be accepted.
  Before the compiler was rebuilt (Nov 17) this file was built with the
  previous compiler (`ccompile`, the tree's compiler/*.fd against the
  stdlib of the last commit) and printed its expected lines: 40 random
  programs split into 1252 pieces with 20 edge blocks, 23 mutants
  rejected, 72 of 72 dropped moves rejected. The planner, the checker and
  nat_par_moves have not yet run inside a native build.
- `tests/nattools/split_alloc_unit.fd` (`fid ccompile`, then run): the
  whole allocation (`nat_split_alloc_unit`: `ng_mir_alloc_split` with the
  MIR verifier, the allocation checker and the split checker on) of
  hand-made functions on AArch64, RISC-V and x86-64, three ways (no split,
  the default, `splitall`); each allocated function is run by a small
  interpreter over the registers and slots the allocation gave (a call
  overwrites every register that is not callee-saved) and must compute
  what the function computes over its vregs. Cases: `wide` (twenty values
  across two calls, changed in a call-free loop), `calls`, `midexit`, and
  24 random programs per target. Under `FID_NAT_MUTATE=split` some
  random allocations must be rejected and none may compute something else
  unnoticed.
- `tests/bootstrap/test_machine_split.fd`: sixteen values live across
  calls, a call-free loop over them, runs between calls, a loop left from
  its middle.
- `tests/nattools/machine_split_check.sh [fid]`: that program on every
  target with the switch on, off, `splitall` and the mutation; `FILES=...`
  for the whole suite under `splitall`.
- `tests/bootstrap/test_machine_split_float.fd` (MI14): floats live across
  calls, with runs of float operations between the calls, a call-free loop
  over values live across calls (`around`, which has no integer spill, so
  only the float split touches it) and more floats across a call than the
  pool holds. Every product is by a power of two, so a fused multiply-add
  gives the same bits. `tests/nattools/machine_mi14_check.sh [fid]`: that
  program on every target, `--nat` Windows included, by default, with
  `splitfloat` off, `split` off and `splitall`, all under `FID_VERIFY=1`;
  `around` must be split (and kept) by default and not with `splitfloat`
  off; it also checks MI14's Windows half (float registers by default,
  vectors beside them, the exception handler keeping xmm6..xmm15).

### Numbers

Measured Nov 17 with `tests/nattools/split_alloc_unit.fd` (the tree's
`compiler/nat_ir.fd`, built through C by the previous compiler; the cost
is `ng_split_cost`'s estimate, not a timing):

| target | `wide` (spilled values) | `wide` cost, split off -> on | 24 random programs: split kept | random cost, off -> on |
|---|---|---|---|---|
| AArch64 | 11 | 592 -> 466 (4 pieces kept, 7 values left whole) | 22 | 6371 -> 4934 (-23%) |
| RISC-V 64 | 10 | 542 (not kept: t4, its only caller-saved register, holds the loop's counter) | 22 | 6426 -> 5721 (-11%) |
| x86-64 | 17 | 892 (not kept: no caller-saved register in the pool) | 20 | 14225 -> 12937 (-9%) |

Without the second split (`FID_NAT_OFF=splitprune`) the random programs
cost 4956, 5775 and 13196, with 21, 20 and 15 splits kept, and AArch64's
`wide` 522. Every allocation, with the split off, on and under
`splitall`, passed the checkers and computed the same result.

The numbers of real programs (`[split]` and `[stats]` lines of
`fid emit --stats`, the suite's output with and without
`FID_NAT_OFF=split`) come from the validation of the built compiler:
`tests/nattools/machine_split_check.sh` and `scripts/cross-test.sh`.

#### A coalescing bug found on the way

The first allocations of some random programs failed the allocation
checker on RISC-V and x86-64 even with the split off ("writes register N
while another live value holds it"), and one x86-64 split computed a
different result. The cause was in `ng_mir_alloc`'s copy coalescing, not
in the split: the first round joined `d <- s` when `s` has one definition,
`d` is not written inside `s`'s range and `d` is not read between `s`'s
definition and the copy in layout order. A branch between them can skip
the copy and reach a read of `d`'s older value, which `s`'s definition
then overwrites in the shared register. The rule now also requires `s`'s
definition and the copy to be in one block (every way on from the
definition reaches the copy); RA6's second round, over exact liveness,
still joins the copies that are safe across blocks.

### Limits

- Pieces are made only for values the first allocation spilled; a value in
  a callee-saved register across a loop that does not use it is not split
  to give the loop that register.
- A loop's pieces take caller-saved registers; where the pool has few or
  none (RISC-V t4, x86-64) they rarely get one. Putting the argument
  registers into the pool (RA7) is what would give them more.
- Loops are found from jumps back in the layout; a loop whose head is laid
  out after its latch, or a rotated loop entered in its middle, gets runs
  only.
- Debug information (DB4) keeps describing a split variable by its
  original vreg: inside a piece's range the location shows the slot, which
  may be stale.

## The fast register allocator (RA14)

The machine layer (`compiler/nat_ir.fd`) has two register allocators in
`ng_mir_alloc`:

- the default one: block liveness to a fixed point (from use lists, RA2),
  copy coalescing in two rounds (RA6), spill weights by loop depth (RA5),
  the segment scan with holes (RA2), argument-register hints and fills
  (RA7, RA3), a second chance for spilled ranges, the float class
  (MI25), and then live-range splitting (RA4, `ng_mir_alloc_split`), which
  runs the allocator two or three times for a function that spills;
- the fast one, for debug builds: one pass over the instructions for the
  positions, no liveness fixed point, no coalescing, no spill weights, no
  splitting. A value that lives in more than one block stays in its frame
  slot (each use loads it, each definition stores it); the values local to
  a block share the registers by one linear scan.

The fast one runs with `fid emit --fast-alloc` (or `--fast`: mid-end on),
with `fid emit --debug` (mid-end off, SP13), with `FID_NAT_FAST=1`, or when
a driver sets `g_nat_fast`. The function cache's key holds the mode, so a
cached function of one allocator is never replayed by the other.
`tests/bootstrap/test_machine_fast.fd` and
`tests/bootstrap/test_driver_fast_alloc.fd` check that programs print the
same either way.

### Measuring it

Every `fid emit` prints, after its `[prof] native:` line,

    [prof] native allocation: <ms>ms, fast allocator | default allocator

the time spent in `ng_mir_alloc_split` over every function that was not
replayed from the function cache (`g_nat_alloc_ms`, splitting included).
With `--time`, `[time] total: <ms> ms` is the whole build.

    bash tests/nattools/ra14_fast_alloc_time.sh $FID [--self] [--runs N]

builds several large tests (and, with `--self`, `compiler/main.fd`) for
x86_64-linux with the default allocator, `--fast-alloc` and `--debug
--no-cache` (a cold debug build, not one replayed from the debug build's
function cache), keeps the fastest of N runs of each, and prints the
allocation time, the total, the share and the fast allocator's speed-up.
It fails when the fast modes' allocation is 10% of the build or more.
Run it on a quiet machine: several fid processes at once make a build 10
to 200 times slower, unevenly.

### Numbers

Nov 17, with the compiler of Nov 16 (`stage0`, which has the fast
allocator but not the allocation-only row, so these are allocation and
emission together: an upper bound), on a machine running other builds at
the same time, `test_compiler_sized.fd` for x86_64-linux:

| mode | allocate and emit | total | share |
|------|------------------:|------:|------:|
| default allocator, mid-end on | 2523 ms | 25.1 s | 10.1% |
| `--fast-alloc`, mid-end on | 1832 ms | 24.9 s | 7.4% |
| `--debug` (mid-end off) | 2862 ms | 16.6 s | 17.3% |
| `--debug` with `FID_NAT_FAST=1` | 2470 ms | 15.1 s | 16.3% |

Nov 21, with the allocation-only row (fid built from the self-hosted stage
0 at 86cc919, the roadmap's RA14 check): in debug mode the allocation is
11–13% of the build (test_compiler_sized 11.2%, test_machine_probe 12.4%,
compiler/main.fd 13.1%), so the 10% bound fails there; `--fast-alloc` with
the mid-end is 6.1–9.5%. The main.fd rows with the mid-end on panic in
iro_sccp (known-bugs 2g.89).

With the mid-end on, the fast allocator's allocation and emission together
are under 10% of the build, so its allocation is too. A debug build has no
mid-end, so the same work is a larger share, and the Nov 21 allocation
row shows allocation alone is over 10% there. Since the fast modes fail
the 10% bound in debug builds, the next places to look in the fast path of
`ng_mir_alloc` are the call clobber masks (`ng_call_mask_i`, once per
call), the register classes (`ng_mir_classes_c`) and the per-vreg tables,
which are allocated for every vreg of the function even when most of them
go to frame slots.

## Post-allocation rules (TG17)

The machine layer (`compiler/nat_ir.fd`) runs a few rewrites after register
allocation, while it emits the allocated MIR (`ng_mir_emit_pass`). Which
rules a target runs is data: `ng_post_rules` returns each target's row of
rule names, and `ng_post_rule(g, name)` tests the row and the rule's switch.

| Target | Rules |
|--------|-------|
| x86-64 (Linux, `--nat` Windows) | ldfwd, ldfwdmov, memop, cmpslot, rmwslot |
| AArch64 | ldfwd, ldfwdmov, wbidx |
| RISC-V 64 | ldfwd, ldfwdmov |

- **ldfwd**: load after store. Suppose an instruction stores a spilled vreg
  to its slot, and the next emitting instruction reads that vreg. The read
  then becomes a register move from the register that was stored, or
  nothing when that is the register it wanted (`ng_ld_fwd`).
  - What is remembered is dropped at a label, a branch, a call, or a
    deferred save or early restore of RA12. It is also dropped when a
    scratch load in the same instruction writes the stored register.
  - `MO_LINE` keeps it.
- **ldfwdmov** (Nov 17): the same forward survives a register-to-register
  `MO_MOV` that does not write the stored register. A move whose registers
  came out equal emits nothing and keeps it too. Such moves are the ones
  that set up a call's arguments, or the copies at a block's end. The move
  must:
  - read no spilled vreg;
  - write no slot;
  - not cross register classes.

  Every target emits such a move as one instruction with no temporaries,
  which is why this is safe.
- **memop**, **cmpslot**, **rmwslot** (x86-64, TG7): a spilled operand is
  used in place from its slot: an ALU operand, a compare, or a
  read-modify-write.
- **wbidx** (AArch64, TG8): a load or store and the `add` of its base become
  one access with writeback.

Two more rewrites hold on every target and are not in the table:

- After allocation, the emitter drops a move whose registers came out
  equal.
- Before allocation, the shared MIR peephole (`ng_peep`, OP19) removes
  branches to the next block, threads jumps and drops redundant moves.

### Switches

`FID_NAT_OFF=<rule>` turns one rule off. `ng_post_rule_switch` spells each
rule's `ng_on("...")` out, so that `scripts/switch-check.sh --list nat`, and
therefore `scripts/pass-matrix.sh`, see every rule.

`FID_NAT_MUTATE=ldfwdmov` keeps the forward even when the move writes the
stored register. It is a wrong rule on purpose, for TG2's mutation checks,
and is never set in a real build.

### Checks

`--stats` prints `[ldfwd] N slot loads after a store became register moves`.

`bash tests/nattools/machine_tg17_check.sh $FID [--matrix]` builds
`tests/bootstrap/test_machine_ldfwd.fd` and `test_machine_ldfwd2.fd` with
`FID_VERIFY=1` on x86_64-linux, aarch64-linux, riscv64-linux and `--nat`,
in three modes: all rules on, `ldfwd` off, and `ldfwdmov` off. It checks:

- the forward counts;
- the outputs;
- that the image changes with `ldfwd` off.

With `--matrix`, it also runs `scripts/pass-matrix.sh` on each target over
the post-allocation switches.

## Branch conditions in the flags: `!` and EXT (RA19, Nov 17)

This page adds to `docs/reference/shrink-wrapping-and-branch-flags.md`.
RA19 keeps a value used only as a branch condition out of a register: the
compare is made at the branch, as one `BRCC` (`compiler/nat_ir.fd`).

- `ng_fuse_branches` handles a compare and its branch in one straight
  stretch, with up to 32 other instructions between (brgap) and `!` in the
  chain (brnot).
- `ng_fuse_andor` handles `&&` and `||` after select formation (brandor).
- `ng_fuse_xblock` handles a compare in a block that dominates the branch
  (brxblock).

### Added

- **EXT in the chain** (`FID_NAT_OFF=brext`). An extension of the compare's
  0 or 1 gives the same 0 or 1, whatever its width and sign. So an
  `EXT d, s` or an in-place `EXT v, v` is followed like a copy:
  - in `ng_fuse_branches`' walk back from the branch;
  - in `ng_fuse_xblock`.

  The lowering's normalization of a `bool` it keeps in a variable is such
  an in-place EXT.
- **`!` across blocks** (`ng_fuse_xblock`, under `brnot`). The chain from
  the branch back to the compare may now hold `!`, an XOR with an LI 1, in
  place or not. It may sit in the branch's own stretch, as in `if !c`, or
  in the compare's block A, as in `val c: bool = !(a < b)`.
  - Each `!` flips the branch's condition.
  - Its LI goes too when nothing else reads it.
  - `FID_NAT_MUTATE=brnot` flips it back (TG2).
- **In-place definitions.** An in-place EXT or `!` gives its vreg a second
  definition. `ng_fuse_xblock` follows it to the value's other definition
  (`prevdef`), which must sit earlier in the same block. Both definitions
  go with the chain, and the rule that every vreg of the chain is written
  only by the chain still holds.

### Still open

- `while a && b` loop heads that select formation (MI24) does not turn into
  a select.
- `&&` / `||` whose second operand contains a call: no select is formed,
  and the join keeps a bool. This needs jump threading of the bool's
  phi.
- RISC-V without Zicond has no select, so `&&` keeps the SETCC of its first
  compare there.

### Checks

`bash tests/nattools/machine_ra19_more_check.sh $FID` covers
`tests/bootstrap/test_machine_brflags2.fd`:

- fewer SETCC after branch fusion in `xb_not`, `xb_nval` and `xb_nn` on
  every target;
- the outputs, with the rules on and off;
- that the mutation is caught;
- that `test_machine_brflags.fd` still passes.

It also prints the spill effect of `ng_fuse_xblock`, whose longer ranges
of the compared values may spill. It sums `[stats] spilled / reloads`
over the brflags tests and `bench/fides/{fib,loop,sieve}.fd`, with and
without `FID_NAT_OFF=brxblock`.

Nov 21 (roadmap RA19 check, fid built from a self-hosted stage 0): the two
test programs print the right output, and with `FID_IROPT_OFF=inline,inline2`
`not_if`, `and_if`, `or_if` and the `xb_n*` cases lose their SETCCs. In a
default build both check scripts fail, because the functions are inlined;
`ext_if`, `xb_ext` and `xb` keep their SETCCs, and the x86-64 / `--nat`
`FID_VERIFY=1` builds of brflags2 hit the argument-register check
(known-bugs 2g.88).

## Secrets in registers on the machine layer (LF20)

Roadmap: docs/roadmap/llvm-roadmap.md, LF20, "`@secret` values never
spilled to memory unencrypted where a register will do, and their spill
slots cleared". This page covers the machine layer (compiler/nat_ir.fd:
the Linux targets, macOS and `--nat` Windows). The pe_ir side is
`PeirGen_wipe_frame` in compiler/pe_ir.fd.

### Which functions

A function holds a secret (`NatFnRec.secret`) when the IR says so
(`IrFunction.has_secret`: a `@secret` / `@sensitive` function, parameter or
local, or a `@zeroize` local), when it has `@zeroize` locals, or when it is
`@constant_time`.

### What such a function does at every return

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

### Secret values spill last

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

### Checking it

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

## Translation validation of instruction selection (RL6)

The machine layer (`compiler/nat_ir.fd`) selects machine instructions for
each MIR operation. Under `FID_VERIFY=1` every machine-layer build first
runs `nat_sel_check(arch)`: each operation is selected alone, with
physical registers, through `ng_mi_dispatch` (the path the replay after
register allocation takes), and the bytes are run by a small evaluator of
the target's instructions (`compiler/nat_enc.fd`, section "RL6") on edge
and pseudo-random values. The result must be what the MIR operation
means. A mismatch, or an instruction the evaluator does not know, stops
the build with a message naming the operation, its registers and the
inputs.

### What is checked

| Part | Function | Operations | Meaning checked |
|------|----------|------------|-----------------|
| integer | `nat_sel_eval_case` | LI, MOV, ADD..SHR, NEG, NOT, DIV, REM, MULH, SETCC, EXT, ASR63, ADDSHL, ADDI, SHI | the destination's value; inputs unchanged |
| floats | `nat_sel_eval_float` | MO_FBIN (+ - * /), MO_FCMP (EQ..GE), MO_I2F, MO_F2I; f64 and f32 | IEEE result (any NaN for a NaN), compares false on NaN but `!=`; F2I only where defined (no NaN, inside the i64 range) |
| memory | `nat_sel_eval_mem` | MO_LOAD / MO_STORE / MO_LDX / MO_STX, sizes 1, 2, 4, 8, signed and unsigned | a load reads the bytes at base + (index << shift) + disp, extended, and writes nothing; a store writes exactly its bytes there |
| branches | `nat_sel_eval_branch` | MO_BRCC (every kind, against a register, the zero register, itself), MO_BNEZ, MO_BEQZ, MO_TBNZ (bits 0, 7, 31, 32, 63, set and clear) | taken exactly when the condition holds; short and long forms |
| calls | `nat_sel_eval_call` | argument setup (MO_LI / MO_MOV into the argument registers, MO_STOUT for stack arguments) then MO_CALL (short and long) or MO_CALLR | at the call, each argument is in its register or its outgoing slot, the target is the callee's, the stack pointer unchanged |

The cases come from `nat_sel_cases` (integer) and `nat_sel_check_more`
(`nat_sel_float_check`, `nat_sel_mem_eval_check`, `nat_sel_branch_check`,
`nat_sel_call_check`). Float cases put operands in general registers (an
f32 as its 32-bit pattern, zero-extended) and, with float registers on
(the default; `FID_NAT_OFF=fregs` turns them off), in float registers.
Memory cases use displacements inside and outside each target's
immediate, a register and the stack pointer as the base, float registers
as the value, every index scale and an index register no indexed form
takes. Call cases cover the platform convention and the internal one
(WP6), with 0, 1, all register and two stack arguments.

### The evaluator

The state is 64 registers (general 0..31, float `32 + f`, the low 64 bits
of xmm / d / f registers: x86-64 keeps the rest of a register an f32
operation writes, AArch64 clears it, RISC-V NaN-boxes an f32), the
flags, and a byte memory (`NselM`): a byte nothing wrote holds a pattern
of its address, or a fixed byte (0x80, 0x7F, 0xFF) so sign extension is
tested both ways; every write is kept, in order. A run starts at byte 0
and stops at a position (the code's end, or the fall-through of a branch,
whose target is placed after a gap), or at a call, whose position,
target and registers are kept (the callee is not run).

Covered instructions, besides the integer ones: x86-64 `jmp`/`jcc`
(rel8, rel32), `call` rel32 and through a register, `test` with an
immediate, `bt`, `mov`/`movzx`/`movsx`/`movsxd` loads and stores (ModRM,
SIB, displacements), SSE `movq`/`movd`, `movsd`/`movss`, `movaps`,
`addsd`..`divsd` and the single forms, `ucomisd`/`ucomiss` (with the
parity flag, `setp`/`setnp`), `cvtsi2sd`, `cvttsd2si`/`cvttss2si`,
`cvtss2sd`/`cvtsd2ss`, `pxor`; AArch64 `b`, `bl`, `b.cond`, `cbz`/`cbnz`,
`tbz`/`tbnz`, `blr`, `br`, `ldr`/`str` of every size and sign (unsigned
offset, register offset, float registers), `add` (extended register),
`fmov`, `fadd`..`fdiv`, `fcmp`, `fcvt`, `scvtf`, `fcvtzs`; RISC-V
`beq`..`bgeu`, `jal`, `jalr`, `auipc`, `lb`..`ld`, `lbu`..`lwu`,
`sb`..`sd`, `flw`/`fld`/`fsw`/`fsd`, `fadd`..`fdiv` (.d, .s), `feq`/`flt`/
`fle`, `fmv` between the register files, `fcvt` (f32/f64, from and to an
integer), `fsgnj`/`fsgnjx`. Float arithmetic is the host's f64 (an f32
operation is the f64 one on the widened operands rounded once to single,
the single-precision result for + - * /); compares and the f32 widening
work on the bits.

### Found by it

The first run found that an AArch64 `MO_LOAD` / `MO_STORE` with the
stack pointer as its base and a displacement outside the immediate
computed its address from `xzr` (ADD, shifted register, reads register 31
as zero): `nat_enc.fd a64_add_base` now uses ADD (extended register) for
a base of sp, in both the selection table (`ng_isel_emit_mem`) and the
hand-written selection (`ng_load`, `ng_store`, `ng_fmem`). The MIR the
lowering makes reaches the frame through `MO_LDSLOT` / `MO_STSLOT`
(correct already), so no program was miscompiled; the bytes for every
other base are unchanged.

### Running it

- Every `FID_VERIFY=1` build on the machine layer (`fid emit --target
  x86_64-linux|aarch64-linux|riscv64-linux`, `--nat`), e.g.
  `scripts/nat-tools-check.sh` and `FID_VERIFY=1 NATIVE=1
  scripts/cross-test.sh <target>`; `tests/bootstrap/
  test_nattools_selcheck_more.fd` uses the operations.
- Directly: `fid ccompile tests/nattools/selcheck_more_unit.fd -o
  selcheck_more && ./selcheck_more` prints each part per target, the
  whole check with `--cpu` x86-64-v3 and rva23u64, and that 12 mutated
  selections (a wrong operation, size, condition, sign, a missing
  argument, a wrong call target) are rejected and a correct one accepted.
  Run it also with `FID_NAT_OFF=fregs` and `FID_NAT_OFF=isel`.

Not covered: vector operations (TG11 checks their rows byte for byte,
`nat_vsel_check`), the memory-operand ALU (`MO_ALUM`, `MO_BRCCM`,
`MO_SETCCM`, `MO_ALUMST`; `nat_memop_unit_check` checks their formation),
jump tables, the prologue and epilogue, calls with float arguments.

---

## SP8: a one-line edit rebuilt by a warm `fid serve`

Goal (docs/roadmap/llvm-roadmap.md, Track SP): a one-line edit rebuilds in
under 100 ms, with the stdlib, the lowered functions and the machine code
kept in a warm `fid serve`, and only the changed module or function checked
and lowered again. On Nov 13, hello took about 0.5 s with every cache warm
(CS6).

### What a build request of `fid serve` reuses

A build request is `emit <file> [flags]`, `build <file> [flags]` or
`ccompile <file> [flags]` (compiler/main.fd `serve_build`). Between
requests the server keeps:

| What | Where | Since |
|------|-------|-------|
| core and the runtime, parsed and merged | compiler.fd `astcache_load`, `astcache_memo_merged` | SP4, SP18 |
| core's checked bodies are skipped | inc.fd `inc_core_checked` (D9) | D9 |
| imported modules' parses | compiler.fd `modcache_get` | CB2 |
| **the program's checked bodies** | compiler.fd `cb2_request_build` (this goal) | SP8 |
| machine code of each unchanged function | nat_ir.fd `g_nat_fncache_mem` | SP7, SP18 |

The new part: a build request runs CB2's incremental body check
(docs/reference/incremental.md). The server compares the program's modules with
the last build whose checks passed. It checks again only the function
bodies that an edit since then can reach: the edited items, and items that
mention a name they define, followed to a fixpoint. Unlike a check request,
the build goes on past the borrow check: comptime substitution,
monomorphization, lowering and code generation.

Skipping a body's checks does not change what gets lowered. The checkers
read the Program and do not change it. Core's bodies are skipped the same
way on every build (D9).

A check request and a build request differ in their CB2 settings (the
runtime merged, the @cfg target, `--debug`; compiler.fd `cb2_config`). So
the first build after a check, or after a change of target, checks every
body.

Each build request writes one line on the server's stderr:

    fid serve: emit prog.fd: 84 ms, 2 of 5 bodies checked

This is the time from the request to the written output, followed by CB2's
note. Over the socket (`fid serve --port`) the request's note also carries
the time and CB2's note. Set `FID_CB2=off` to check every body on every
request.

Lints are not run in an incremental build request (as in a check request).
Warnings that come from the checks themselves, such as a call to a
`@deprecated` function, appear only when the body holding the call is
checked again.

### Measuring

    FID=release/fid.exe bash tests/tooling/sp8-edit-rebuild-check.sh
    T=nat ...            # Windows x64 through the machine layer (--nat)
    MODE=debug ...       # only the debug mode (no mid-end, fast allocator)
    STRICT_TIME=1 ...    # a rebuild of 100 ms or more fails the script

The script edits one line of one function, then one line of another, and
rebuilds after each edit through one server, in release mode and in debug
mode. It checks that:

- each rebuilt image is byte-identical to the one-shot `fid emit` of the
  edited file;
- CB2 checked fewer bodies than the program has;
- a type error is reported, and the image is right again once the error is
  taken out.

It prints each request's time and whether the 100 ms target was met.

Known bug (Nov 21, 2026, known-bugs 2g.82): the machine layer's function
cache does not keep images byte-identical. A cache-hit image differs from a
cache-off one, and `fid serve` rebuilds differ from a one-shot emit, so the
first check above fails. Every image still runs the same.

### What is left

- **Lowering.** Every build lowers every reachable function again. The
  lowered-IR cache (ir_ser.fd `irc_*`, FID_IRCACHE) is file-based, and its
  records make files far too large for an edit loop. The in-memory mode it
  needs (records kept between `lower()` calls of one process, the salt
  compared in memory) is a change in compiler/ir_ser.fd. The server would
  then turn it on, as it does `g_nat_fncache_mem`.
- **The mid-end.** iropt, the rules and the e-graph run on the whole module
  on every build. In release mode this is most of a warm rebuild. A cache
  per function has to respect iropt's whole-module passes (inlining,
  identical-code folding, facts about globals), so it belongs in iropt.fd.
  In debug mode (`--debug`) there is no mid-end, so the edit loop's
  target is measured there.
- **Relinking (SP7, done Nov 27).** In a long-lived process that asks for
  it (`fid serve --relink`, FID_NAT_ON=relink) the next build of a program
  keeps the last one's layout: an unchanged function is replayed at its
  offset, a changed one goes back into its slot when it fits, a function a
  build inlined away keeps its slot for the next one, and only the fixups
  are resolved again (docs/reference/incremental.md;
  tests/nattools/relink_check.sh, 21 ok on the -O2 build of Nov 27, 16:19).
  A one-shot `emit` still lays the image out afresh: that is the image SP8
  wants.

---

## pe_ir: the per-function cache and `--cpu`

The default Windows backend (`compiler/pe_ir.fd`) has two switches added
on Nov 16–17, 2026.

### The per-function cache (inc-roadmap B5)

`fid emit` (and `fid build`) for Windows saves each function's machine
code before its labels and `.rdata` references are resolved. The
references are stored by name: a function, an arithmetic panic stub, a
string's text, a global, an import slot, or the allocator's state. A
function's entry is keyed by its IR (`iw_irfunc`) and by a salt. The salt
covers the compiler's own hash, the struct layouts, globals, externs,
every labelled name, the C overrides, the named and cold functions,
`FID_PE_OFF`, `FID_PE_CT_OFF`, `--cpu` and `--checked-alloc`.

On the next build, a function whose key matches has its bytes copied to
its new place, and its references are registered again. The image is
laid out and linked from scratch on every build, so it comes out
byte-identical to a build without the cache.

These are never cached: `@interrupt` and `@constant_time` functions,
`asm!`, dyn values, functions that call something nobody defines, and
debug builds (`-g`, `FID_PE_DEBUG`).

| Setting | Effect |
|---|---|
| (nothing) | on: the file `<output>.fides-fncache.pe` sits beside the output, like the build stamp `<output>.fides-stamp` |
| `FID_FNCACHE=<path>` | the file `<path>.pe` |
| `FID_FNCACHE=0` (or `off`, `no`, `none`, `false`), `--no-fncache` | off |
| `FIDES_NO_CACHE=1`, `--no-cache` | off, unless `FID_FNCACHE` names a path |
| `FID_PE_FNCACHE_STATS=1` | prints `[pe] fncache: N hits, M misses` |

Code: `pe_fc_base_path`, `pe_fc_output`, `PeirGen_fc_start`,
`PeirGen_fc_capture`, `PeirGen_fc_replay`, `PeirGen_fc_load`,
`PeirGen_fc_finish` and `PeirGen_gen_cached`. The driver records the
output with `pe_set_output_path`. Without that call, the `-o` of an
`emit`/`build` command line is used.

Checks:
- `tests/nattools/pe_fncache_check.sh`: for every suite program it builds
  without the cache, cold, warm and with the default cache, and every
  image must be the same; it also checks an edit pair and the off
  switches.
- `scripts/pe-win-check.sh` (B5).

### `--cpu` (competitive OP20)

`fid emit --cpu <name>` takes the names that `nat_cpu_check` accepts for
x86-64. An unknown name is an error. With `x86-64-v3` or `x86-64-v4`
(`peir_bmi`), the backend uses these instructions:

- `SHLX`/`SARX`/`SHRX` for a 64-bit shift by a variable count
  (`PeirGen_shift_bmi`). The operands come straight from their registers
  and `CL` is not needed.
- `BLSR` for a 64-bit `x & (x - 1)` (`peir_blsr_src`, `PeirGen_gen_blsr`).
  The `x - 1` is still generated, along with its overflow check.

Without `--cpu`, and with `baseline` or `native`, the image is the same
as before. The encoders `PeirEnc_shiftx`, `PeirEnc_blsr` and
`PeirEnc_andn` (`compiler/pe_ir_enc.fd`) use the same encodings as the
machine layer's `x64_shiftx`, `x64_blsr` and `x64_andn`.

ANDN is not selected on pe_ir yet. pe_ir frees a value's register after
its last use, so at the And, the operand of `!b` may already be gone. The
fusion needs the Not to be skipped and `b` kept live.

Check: `tests/nattools/pe_cpu_check.sh` runs
`tests/bootstrap/test_pe_cpu_bits.fd`.

---

## One interface for the runtime's machine-code primitives (MI20)

Every native backend defines the same set of `nat_*` primitives, the
machine-code pieces a runtime cannot write in Fid: system calls and
threads, the thread word, signals, atomics, the task context switch,
calls by address, traces (frame records and the function tables), the
start block, and a few target facts. The list is
`prim_interface_names()` in `compiler/pe_ir_stubs.fd`.

| Group | Names | Contract |
|---|---|---|
| system | `nat_syscall(n, a..f)` | a Linux system call by generic number; `-ENOSYS` on pe_ir |
| threads | `nat_clone_thread(entry, arg, stack_top, ctid, tls)` | starts `entry(arg)` on a new thread whose thread word is `tls`; `*ctid` becomes 0 when it ends; the thread id, or `-EAGAIN` |
| | `nat_tls_get()`, `nat_tls_set(v)` | the thread word |
| signals | `nat_sig_handler()`, `nat_sig_restorer()` | the handler's address (Windows: the vectored exception handler) and the restorer's (0 on Windows) |
| atomics | `nat_atomic_load/store/add/cas` | 8-byte, sequentially consistent; `add` returns the old value, `cas` 1 or 0 |
| tasks | `nat_ctx_swap(save, sp)`, `nat_ctx_tramp()` | switch contexts; the trampoline a new context starts in (its saved layout belongs to the runtime of the target) |
| calls | `nat_call_fn(addr, nargs, argv)` | `addr(env 0, argv[0..8])` |
| traces | `nat_fn_table()` | `[name, address]` per function named by a string, then `[0, 0]` |
| | `nat_fn_names()` | `[address, name]` per function in address order, then `[end of code, 0]` |
| | `nat_frame_fp()`, `nat_frame_ret(fp)`, `nat_frame_next(fp)` | the caller's frame record, its return address, the next record |
| start | `nat_start_sp()` | `[argc][argv...][0][envp...][0]` |
| target | `nat_arch()`, `nat_os()`, `nat_fma(a, b, c)` | 1 AArch64, 2 RISC-V, 3 x86-64; 0 Linux, 1 Windows, 2 macOS; `a * b + c` rounded once on f64 bits |

### Who defines them

- The machine layer (`compiler/nat_ir.fd`): Linux x86-64, AArch64,
  RISC-V, `emit --nat` Windows and macOS. `ng_prim_check` fails a build
  under `FID_VERIFY=1` when one is missing.
- pe_ir, the default Windows `fid emit` (`compiler/pe_ir_stubs.fd`,
  `PeirGen_gen_nat_prims`; the tables in `compiler/pe_ir.fd`,
  `PeirGen_gen_nat_tables`). Arguments come in the C convention (rcx,
  rdx, r8, r9, then the stack). The atomics, `nat_call_fn`,
  `nat_ctx_swap` and `nat_ctx_tramp` share the bytes of their `pe_*`
  twins. A primitive is made only when a reached `extern "C"`
  declaration names it, so images that use none are unchanged except for
  a 2-byte trap per unused name. Under `FID_VERIFY=1` all of them are
  made and `PeirGen_nat_check` fails the build ("the runtime primitives
  of this target lack ...") if one is missing. The entry point allocates
  the TLS index and builds the start block from the command line. The
  tables hold absolute addresses, because the image loads at its preferred
  base and has no relocations.

### Checking

`bash tests/nattools/mi20_prim_check.sh <fid>` builds
`tests/nattools/mi20_prims.fd`, which calls every group of primitives by
these names, with pe_ir and `--nat`, both with and without `FID_VERIFY=1`.
With `LINUX=1` it also builds for the Linux targets. Each build must print
the same lines.

### Still to do

`stdlib/pe_runtime` still calls the `pe_*` names (`pe_atomic_*`,
`pe_call_fn`, `pe_ctx_swap`, `pe_ctx_tramp_addr`, `pe_fn_lookup`,
`pe_thread_entry_addr`). The next steps are to move it to the `nat_*`
names, and then to drop the `pe_*` twins from `pe_primitive_names` and
`pe_stub_deps`.
