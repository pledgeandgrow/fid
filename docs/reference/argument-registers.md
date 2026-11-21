# Argument registers for values (RA7 and RA3)

The machine layer (`compiler/nat_ir.fd`: Linux x86-64, AArch64, RISC-V,
macOS, `--nat` Windows) allocates values to the pools of the target
description (`nat_target_desc`): caller-saved registers that calls lose
and callee-saved registers that calls keep. On x86-64 the caller-saved
pool is empty: rax, rcx, rdx, rsi, rdi and r8..r11 are argument, result
and scratch registers, so every value either takes one of rbx and
r12..r15 (saved in the prologue) or a frame slot.

The argument registers are free most of the time. A function names them
only to pass arguments and results, and a few operations use them without
naming them. `ng_ra7_info` models both per operation:

| Operation | Argument registers it uses without naming them |
|-----------|-----------------------------------------------|
| call (`MO_CALL`, `MO_CALLR`) | all: it reads the ones written since the last call and writes all of them |
| prologue, epilogue, vector operations | all |
| x86-64 division, remainder, high multiply, long copy | rdx |
| x86-64 shift by a register | rcx |

An epilogue reads the result words: argument registers 0 and 1 (on x86-64
rdi and rsi; the target's `result_reg`, rax, is a scratch register there).

A range [lo, hi] may hold argument register R when no operation in
[lo, hi] uses R without naming it (so no call falls inside), none in
[lo, hi) writes R by name or leaves R's own content (an argument or a
result) live after it, and no other range given R shares a position with
it (`ng_arg_fit`, `ng_gr_clash`).

## RA7: hints

`ng_arg_hints`, after the scan and RA2's allocation in holes: a range a
move ties to an argument register (a parameter copied out of it, a call
operand copied into it, a call's result) takes that register when it fits,
so the move emits nothing. `FID_NAT_OFF=arghint` turns it (and RA3) off.

## RA3: any range no call crosses

`ng_arg_free`, after RA7, gives the candidates to ranges without a hint:

- **Fill.** Each integer range still spilled, the most costly first (spill
  weights), takes the first argument register it fits. A range crossing a
  call never fits; it keeps its slot, or RA4 splits it around the call
  (`ng_mir_alloc_split`).
- **Trade.** Each callee-saved register whose ranges all fit argument
  registers gives them up (a range tries the register of a copy partner
  first), unless a move between two of them would stop being a move to
  itself. The register is then unused: its save and restore go away, and a
  leaf may lose its frame (`ng_leaf_ok`). `ng_mir_alloc` then runs RA2's
  allocation in holes again, so a spilled range that crosses a call may
  take the freed register.

The allocation checker (`FID_VERIFY=1`, `ng_mir_check_args`) judges RA3's
choices with the same model against exact value liveness.

Switches: `FID_NAT_OFF=argfree` (no RA3), `FID_NAT_OFF=argtrade` (fill
only), `FID_NAT_MUTATE=argfree` (the implicit uses ignored; the checker must
reject the build). `--stats` prints, per function,
`[argfree] f: N spilled ranges in argument registers, M ranges moved off K
callee-saved registers, H spilled ranges then given one`.

## WP6: arguments in the registers the caller has them in

WP6 asked for internal functions to take their arguments where the caller
already has them. The machine layer does it from the other side: a value a
move ties to an argument register is placed in that register when nothing
needs the register in between (RA7), and any other range no call crosses
may take a free one (RA3). A parameter passed on in the same position
after code that needs no argument register then stays in it: the
parameter copy at entry and the operand copy at the call are moves to
themselves and emit nothing. Values that live across a call cannot stay
in argument registers (every call loses them), so a per-function
convention chosen from the callers' registers would only help values
the hints cannot place anyway, and is not done.
WP6's internal convention (`internal_conv`, `ng_argreg_i`,
`ng_nregargs_i`) adds argument registers for functions called only
directly (r9 on x86-64, x13..x15 on AArch64, t6 on RISC-V); those extra
registers are not hint candidates (`ng_arg_cands` takes the platform's
`nregargs` only), so the hints never hold a value in them.
`tests/bootstrap/test_machine_wp6_args.fd` and
`tests/nattools/wp6_args_check.sh` check it: pass-through functions count
fewer moves with the defaults than with `FID_NAT_OFF=arghint,argfree`, and
a five-argument pass-through uses the extended register.

## Windows: code that Windows calls

On `--nat` Windows the machine layer uses its own convention (arguments
in rdi, rsi, rdx, rcx; callee-saved rbx, rbp, r12..r15), while Windows'
x64 convention makes rdi, rsi and xmm6..xmm15 callee-saved too. Calls
from the layer into Windows go through stubs (`nat_win_call`,
`nat_pe_win64_stub`) that Windows' own callee-saves protect: nothing
there changes with RA7 or RA3. The other direction is the risk: a value
the layer keeps in rdi or rsi (RA7, RA3; already before them, any argument
the layer passes) reaches Windows if Windows calls the layer's code and
then carries on. Audit (Nov 17) of every place Windows calls this code
(`compiler/nat_ir.fd` `ng_gen_primitives_win`):

| Entry | Called by | Returns to Windows? | Registers kept |
|---|---|---|---|
| the entry point | the loader | no: ExitProcess | none needed |
| the thread start (`nat_win_tramp`, `pe_win_thread_main`) | CreateThread | no: ExitThread | none needed |
| the vectored exception handler (`nat_sig_handler`, `pe_win_on_exception`) | ntdll's exception dispatcher | yes (0: go on searching, -1: continue) | pushes rbp, rdi, rsi and stores xmm6..xmm15 (MI14) around the call; the Fides function keeps rbx and r12..r15 itself |

So the hints are safe on `--nat` Windows. There is no other callback:
the layer does not hand Fides functions to C or Windows as callbacks
(`m.c_callbacks` is only hashed for the function cache). A new entry that
Windows calls and that returns must save rdi, rsi and xmm6..xmm15 the way
the exception handler does. `tests/nattools/win_callback_regs_check.sh`
finds the handler's bytes in a `--nat` image (with the hints on and with
`FID_NAT_OFF=arghint,argfree`) and fails if those saves are gone.

## Tests

- `tests/nattools/argfree_unit.fd` (`fid run`): fill, trade, a trade kept
  back by a copy, the first result word held in rdi up to the epilogue,
  and the mutation rejected, on hand-made machine IR.
- `tests/bootstrap/test_machine_argfree.fd`: eight values live through a
  loop with division and shifts, a leaf, values between calls, a two-word
  result.
- `tests/nattools/machine_ra3_check.sh [fid]`: the unit test, then the
  program on every target with `FID_VERIFY=1`, with and without
  `FID_NAT_OFF=argfree` (output, `[argfree]` lines, reloads), and the
  mutation.

Nov 21 (roadmap RA3/RA7 check, fid built from a self-hosted stage 0): on
AArch64 and RISC-V this passes. On x86-64 (Linux and `--nat` Windows) the
`FID_VERIFY=1` allocation checker rejects assignments made with the hints
and fills (test_machine_argfree, test_machine_simd and others), gone with
`FID_NAT_OFF=arghint` or `argfree`; the programs print the right output
without `FID_VERIFY`, so either the assignment or the checker is wrong
(known-bugs 2g.88).
