# Tail calls in the machine layer

How `return f(..)` becomes a jump in the native backends of
`compiler/nat_ir.fd` (Linux x86-64, AArch64 and RISC-V; `--nat` Windows),
and when it stays a call. Roadmap: `docs/roadmap/llvm-roadmap.md` OP13 and
MI28.

## Two mechanisms

1. **The IR's loop (OP13).** `compiler/iropt.fd` `iro_tailcalls` turns a
   self-recursive call in tail position (`return f(..)` in `f`, the call's
   only use the return) into a jump back to the function's start, so it is
   a loop on every backend. Switch: `FID_IROPT_OFF=tailcall`.
2. **The machine layer's jump (OP13, MI28).** After allocation, a call in
   tail position (`ng_tail_at`: from the call through moves, constants,
   labels and jumps only to an epilogue, its result still in the result
   register) to a compiled Fides function is emitted as the epilogue and a
   jump (`ng_epilogue_tail`). This covers calls to other functions (mutual
   recursion, sibling calls) and self-recursion whether or not the IR
   optimizer made it a loop. Switch: `FID_NAT_OFF=tailcall` (never).

## When the machine layer jumps

A call at MIR position k becomes a jump when all of these hold:

- the function is not `@constant_time` (`ng_tail_fn_ok`);
- the call is in tail position and its callee is a compiled Fides
  function that returns in one register (`ng_tail_at`);
- **stack arguments fit** (`ng_tail_stack_ok`): each of the call's
  `MO_STOUT`s has an index below the number of stack parameters this
  function itself was passed (one more than its highest `MO_LDIN`), and no
  `MO_LDIN` follows the first of them. The stores then go to this
  function's own incoming argument area (`ng_st_incoming`), where the
  callee, entered with the same stack pointer, reads them;
- **no argument carries this frame's address** (`ng_tail_taint`, MI28,
  below).

### The limit: more stack arguments than the caller received

A callee that needs more stack arguments than the calling function was
given stays a call. Its arguments past the incoming area would have to go
below the caller's return address, into the caller's caller's frame, which
the jump cannot own. On x86-64 Linux a function has five register
arguments (with the hidden environment), so `fn few(n, a)` calling
`many(n, a, 1, 2, .., 10)` in tail position is a call;
`tests/bootstrap/test_machine_tailcall_stack.fd` (`few` / `many`) keeps
such a pair. Calls between functions with the same number of parameters,
or to ones with fewer, are jumps.

## Functions that take their frame's address (MI28)

A struct local handed to a callee by reference, a struct result slot, the
argument block of a spawn: each is an `MO_SLOTADDR`, the address of a slot
of this function's frame. After a jump the frame belongs to the callee, so
such an address must not reach it. Before MI28 any `MO_SLOTADDR` kept the
whole function from jumping; now the decision is per call:

- **The analysis** (`ng_ft_run`, mode 0) follows frame addresses from
  `MO_SLOTADDR` (and any read of the stack or frame register) through every
  operation that computes a register from registers. Virtual registers are
  tracked for the whole function; physical registers forward over the
  machine blocks (a call clobbers the caller-saved, argument and result
  registers). A load gives a clean value. A call's arguments are the
  registers set in its window (`ng_load_args` sets each one there) and its
  `MO_STOUT`s. A call that receives a frame address is marked: it stays a
  call.
- **Escapes.** If a frame address is stored to memory (`MO_STORE`,
  `MO_STX`, `MO_STSLOT`, `MO_ALUMST`, a vector splat), or handed to a
  callee that may keep it or give it back, every call of the function is
  marked: the address could reach any callee from there.
- **Callee summaries** (`ng_ft_settle`, `ng_ft_run` mode 1). A callee is
  clean when nothing derived from its arguments is stored, returned (in a
  result register at an epilogue, written there by the callee) or handed to
  a callee that is not clean. Reading through an argument is allowed: that
  is what `fn len(t: &S) -> i64` does. The summaries are the least fixed
  point over the callees reached this way (recursive helpers included),
  computed once per build and kept by label; a group of more than 64
  functions or 32 rounds is counted as escaping. Indirect calls, imports,
  runtime stubs and primitives are not clean.
- **Why loads are clean.** A frame address is in memory only if it was
  stored there: by this function (an escape, above) or by a callee (then
  that callee is not clean, also an escape). So while a function has no
  escape, nothing it loads can be its own frame's address. Fid's rules
  keep references second-class (`docs/reference/language-guide.md` §4.6):
  a reference may be returned only when it comes from a reference
  parameter, which a summary sees as a returned argument.
- **The function cache (SP20).** With `FID_FNCACHE` the summaries are the
  same as without it, so the image is too. Each cache entry keeps the
  function's own summary (`ft_self`, settled when it is captured:
  `nc_note_ft`) and the summary its analyses read of each function it
  refers to (`dep_ft`). A function compiled again reads a replayed
  callee's summary from the callee's entry (`ng_ft_esc_map` seeds the
  summaries with them: a replayed function's MIR is only a skeleton). A
  replayed function is kept only while the summaries it read still hold:
  `nc_deps_ok` checks the replayed callees, `nc_ft_hold` (at its replay)
  the ones compiled again; one that does not hold makes the image again
  without it and its callers (`nc_give_up_callers`). One exception: a
  summary group past 64 functions or 32 rounds counts as escaping, and
  which functions share a group depends on the order summaries are asked
  for, which the cache changes (it settles each captured function's
  summary at once); a program with such a group may get other jumps with
  the cache than without it (correct either way).
  Check: `tests/nattools/sp20_fncache_tail_check.sh`.
- Functions of more than 40000 MIR operations are not analysed (every
  call marked). The analysis runs once per function, at the first call
  that could be a jump.

Switch: `FID_NAT_OFF=tailframe` restores the old rule (no jumps at all in
a function with an `MO_SLOTADDR`).

### Examples

```fid
struct Acc { a: i64, b: i64, c: i64 }

fn acc_sum(t: &Acc, d: i64) -> i64 {          // clean: only reads t
    if d <= 0 { return t.a + t.b * 2 + t.c * 3; }
    return acc_sum(t, d - 1) + 1;
}

fn walk(n: i64, total: i64) -> i64 {
    if n == 0 { return total; }
    var t: Acc = Acc { a: n % 7, b: n % 5, c: 1 };
    return walk(n - 1, total + acc_sum(&t, 1));  // a jump: passes numbers
}

fn peek(n: i64) -> i64 {
    var t: Acc = Acc { a: n, b: n + 1, c: n + 2 };
    return acc_sum(&t, 2);                       // a call: passes &t
}
```

`walk` runs three million levels deep in one frame even with
`FID_IROPT_OFF=tailcall`; so does mutual recursion between two such
functions, which the IR's loop never covered
(`tests/nattools/tailcall_frame_deep.fd`).

## Tests

- `tests/bootstrap/test_machine_tailcall_stack.fd`,
  `tests/nattools/tailcall_deep.fd` (stack arguments; checked by
  `tests/nattools/machine_batch4_check.sh`)
- `tests/bootstrap/test_machine_tailcall_frame.fd`,
  `tests/nattools/tailcall_frame_deep.fd` (frame addresses; checked by
  `tests/nattools/machine_tailcall_frame_check.sh`, which also builds with
  `FID_NAT_OFF=tailframe` and `tailcall` and expects the deep test to
  overflow without the jumps)
