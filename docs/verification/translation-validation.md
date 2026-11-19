# Translation validation (VF6): scope and first steps

Status (Nov 16 2026): **a first validator is written** (`compiler/tv.fd`,
section "What is built" at the end), not yet run on the suite. Before it
(Nov 7) nothing proved that an optimized function means the same as the
original. This page records what is checked besides, the design chosen
for the validator, and what is left.

## What is checked today (testing, not validation)

- **VF4** (`scripts/verify/variants.py toggle`): every pass of
  `compiler/iropt.fd` switched off at run time (`FID_IROPT_OFF`), and every
  `PEIR_*` constant of the native backend switched off in a rebuilt
  compiler; the output of each test must not change.
- **VF5** (`tests/bootstrap/test_opt_*.fd`): one program per pass whose
  output depends on the pass being right, checked against the interpreter,
  which runs no optimizer.
- **VF7** (`scripts/verify/variants.py mutate`): a compiler with a
  deliberate bug in a pass must fail one of those tests.

These find a wrong pass only on the inputs the tests give it.

## The design for a first validator

Pass by pass, function by function, on the IR of `compiler/ir.fd`:

1. **Scope:** the passes of `iropt_native` (inline, fold, dce, gvn,
   tailcall, licm), which rewrite one function's IR into IR. Register
   allocation and encoding (`pe_ir*.fd`) are out of scope: their output is
   machine code, a different language.
2. **Witness:** each pass already knows why its rewrite is right (a value
   number, a hoisted instruction and the loop it left, a folded constant).
   It records that as a witness: a map from each new value to the old value
   it replaces, plus the blocks it moved.
3. **Check:** a separate checker, much smaller than the passes, walks the
   original and the optimized function together and checks each witness
   locally:
   - fold: the folded constant equals the operation on the constants, at the
     operation's type (signed or unsigned; the VF5 test `test_opt_fold.fd`
     shows why the type matters), and the operation cannot panic;
   - gvn: the reused value is computed by the same pure instruction on the
     same (renamed) operands, and its block dominates the use;
   - licm: the hoisted instruction is pure, cannot panic or is guarded, and
     its operands are not written in the loop;
   - dce: every removed instruction is pure;
   - tailcall: the new parameter values are computed before any is
     overwritten;
   - inline: the copied block equals the callee's with parameters renamed.
   A check that fails rejects the optimized function and keeps the original
   (and reports it under `--validate`).
4. **Equivalence where a witness is not enough:** for straight-line integer
   code, compare the two functions symbolically (both are expressions over
   the parameters; normalise and compare, or ask an SMT solver with
   bit-vector theory). Loops are left to the witnesses.

## What it needs

- A `--validate` mode in `iropt_native` that keeps the pre-pass function and
  calls the checker (the native-speed session owns `compiler/iropt.fd`).
- The witnesses, one per pass.
- The checker itself, in a module of its own (`compiler/tv.fd`), with
  `tests/verify/tv/` holding a pair (original, optimized) per rule it must
  accept and per mistake it must reject — the VF7 mutants are the first
  mistakes to reject.

## What is built (Nov 16)

`fid emit <file.fd> --validate` (or `FID_VALIDATE=1`, which is also in the
build-cache key, `compiler/inc.fd`) turns it on:

- **The hook.** `iro_time` in `compiler/iropt.fd`, which runs each
  per-function pass of `iropt_native` (fold, simplify, branchfold,
  branchfacts, sccp, promote, dse, dce, tailcall, gvn, iv), keeps the
  function before the pass (`tv_before`: a copy of every Vec a pass edits
  in place) and asks `tv_after` about the result; iropt_me does the same
  for the SSA round trip (`ssa`, `compiler/ssa.fd`). A rejected result is
  reported (`tv: <function>: pass <name> rejected: <why>`) and undone: the
  function gets back its IR from before the pass. `tv_report` prints the
  totals at the end of the mid-end. The module passes (inline, ipcp, spec,
  hofspec, the iropt_me passes other than `ssa`) and licm/devirt, which
  `iropt_native` calls directly, are not validated yet.
- **Witnesses (VF6)** for the passes that keep value ids — fold, dce, gvn
  and ssa: the witness is the id correspondence between the two functions,
  and the checker (`tv_structural`) checks each rewrite it implies:
  instructions with an effect stay, in their order, with operands of equal
  value; an instruction that may panic stays (a fold must give the value
  the operation has at its type, checked arithmetic included) or an equal
  one that runs first stands for it (gvn); values are equal when both are
  the same constant, or the same pure operation on equal operands, or the
  same load with nothing between that writes what it reads. For `ssa` the
  values of the promoted locals are left to the random runs.
- **Random inputs (RL5)** for every pass but tailcall (`tv_random`): when
  both functions use only integers, plain locals, calls and prints, a small
  evaluator runs both on the same 12 inputs per parameter set (0, 1, -1,
  small, larger, full-width values). Calls are uninterpreted: the result
  is a hash of the callee, the arguments and, for a call with effects, its
  place among those calls, which go into a trace with the prints. The
  outcome (returned value, panic, Unreachable) and the trace must agree.
- **Trusted:** effect annotations on call sites (a pure site is a function
  of its arguments) and dominance (the IR verifier, `FID_VERIFY=1`).

Tests: `tests/bootstrap/test_ssa_validate.fd` (the suite's programs must
print the same with validation on). Left: run the suite with
`FID_VALIDATE=1` and look at every rejection (each is a miscompile or a
false alarm of the checker); `tests/verify/tv/` pairs (original,
optimized) per rule to accept and per mistake to reject, the VF7 mutants
first; witnesses for licm, inline and the module passes; SMT for
straight-line code (step 4 above) is not done.

## What is built (Nov 17)

Written Nov 17 (VF6), checked with a compiler of the day before only
(see "How it was checked"):

- **LICM** (`tv_licm`): the ids are kept and instructions only move,
  into the block that enters their loop, under the rules of
  `iropt.fd iro_licm`: an instruction that may panic runs first each time
  the loop is entered, a load reads nothing the loop stores, a call moved
  is one the effect masks say is pure (`iro_tv_licm_calls` hands them to
  `tv_trust_call`). Random runs as well.
- **Devirtualization** (`tv_devirt`, `tv_after_devirt`): a trait call
  becomes a direct call of the method of the type the value was made from.
- **The module passes** (`tv_mod_before`/`tv_mod_after` around inline,
  inline2, hofspec, ipcp, spec, gconst and icf in `iropt_native`): each
  function the pass changed is checked against its copy from before the
  pass, by the pass's witness and by runs on random inputs in which calls
  into the module are evaluated in the module before or after the pass
  (`tv_wrandom`). Witnesses: inline and inline2 (the new instructions are a
  copy of the callee's one block with its parameters read as the call's
  arguments, the call a move of its result), spec (a call sent to a clone
  of its callee that is the callee with one parameter replaced by the
  constant the call passes), gconst (the folded loads read a global
  nothing writes), icf (a call sent to a function with the same body),
  ipcp (an operand changed from a parameter to a new constant every call
  passes for it). hofspec has no witness of its own: the runs decide
  (they evaluate `CallPtr` through function values). A rejected module
  pass is undone as a whole.
- **SMT for straight-line code** (design step 4): with
  `FID_VALIDATE_SMT=<dir>` every pair (before, after a pass) of which both
  are one block of integer code without calls is written as
  `<dir>/tv<N>.smt2` (QF_BV: both functions over the same parameters, and
  the assertion that they differ in whether they panic or in what they
  return; checked arithmetic is computed on 128 bits, signed division on
  64 with its one overflow). `unsat` proves the pass right on that
  function; the first line says what the validator decided.
  `tv_smt_examples` writes nine pairs whose answer is known (x * 2 as
  x + x; x + y - y as x, wrong under checked arithmetic; a fold right and
  wrong; an unused division removed; (x ^ y) ^ y as x; x / 4 as x >> 2,
  wrong for negative x; x * 8 as x << 3, wrong under checked arithmetic;
  x - x as 0).
- **Negative tests.** `FID_TV_INJECT=<passes>` (`all` for every one) puts
  a fault into what those passes produce (an addition made a subtraction,
  a product, xor or or made an addition, a constant's low bit flipped:
  first in the value the function returns), only into functions whose
  name starts with `FID_TV_INJECT_IN` when that is set; the compiler
  reports each as caught or NOT caught. `FID_TV_SELFTEST=1` (with
  validation on) runs `tv_selftest`: 24 pairs built by hand, one per rule
  a check must accept and one per mistake it must reject (fold, dce, gvn,
  licm, inline, gconst, icf, ipcp, spec, devirt, an injected fault, the
  SMT writer).

Tests:

- `tests/bootstrap/test_ssa_validate.fd`: validation on, the expected
  output, nothing rejected.
- `tests/bootstrap/test_tv_inject.fd`: faults put into what fold,
  simplify, branchfold, branchfacts, sccp, promote, dse, dce, gvn, iv,
  licm, the SSA round trip, inline, inline2, ipcp, spec, gconst and icf
  make of its `main_*` functions; the program must still print what it
  prints with no fault, so every fault was caught and undone.
- `tests/tooling/tv_selftest_main.fd`: the self-test and the SMT examples
  as a program of their own (C backend), without a build of the compiler.
- `tests/tooling/tv-check.sh`: the self-test (as a program and inside the
  compiler), z3 on the SMT examples (each answer as its first line says)
  and on the obligations the compiler writes for the tests (each pair the
  validator accepted must be `unsat`), the two tests above, and with
  `SUITE=1` the whole suite with validation on (lines SAME / REJECT /
  DIFF / BUILD, and the totals checked, rejected and not checkable).

How it was checked (Nov 17): `tv_selftest_main.fd`, built through C by
the compiler of Nov 16 against the tree of Nov 17, prints "24 cases, 24 as
expected"; z3 (the z3-solver wheel) answers every one of the nine examples
as expected; `test_tv_inject.fd` prints its expected output under `run`.
The compiler with these changes was not built that day, so the suite run
with `FID_VALIDATE=1` (and with it every rejection it finds) is left to
`tests/tooling/tv-check.sh SUITE=1` with the next build.

Left: tailcall's witness (a self tail call made into a loop; tailcall is
the one pass neither check applies to); a witness of its own for hofspec;
witnesses for the per-function passes of `iropt_me.fd` (sra, loops,
unroll, loadhoist, forward, matchfacts, ...: `ime_tv_run` checks them by
the random runs only); the module passes of `iropt.fd` not listed above
(dynspec, fielddevirt, closuredevirt, envtrim); a solver run inside the
compiler (the obligations are written for z3 to run after the build).

## Before/after pairs and triage (RL5, Nov 17)

RL5 asked for pairs the checker must accept or reject, kept as files, and
for the suite's rejections to be triaged.

- `tests/verify/tv/*.tvp`: 56 pairs, each a function (or a module's
  functions) as it was before a pass and as the pass left it, with the
  verdict the checks must give (`expect accept`, `reject` or `n/a`, and
  optionally `why <text>` that a rejection's reason must contain). The
  format, a small text IR with explicit value ids, is described above
  `tv_pairs_text` in `compiler/tv.fd`. Files: `fold.tvp` (folds right and
  wrong, checked overflow at i64 and i8, @wrapping, division by zero),
  `dce_gvn.tvp` (what may panic or has an effect stays; loads across
  stores; calls with effects are not merged), `licm_ssa.tvp` (LICM's three
  rules; promoted locals, also out of SSA through a slot), `random.tvp`
  (passes no witness covers: strength reduction traps such as `x * 8` vs
  `x << 3` and `x / 4` vs `x >> 2`, branch folding, unrolling, the order
  of calls with effects, narrowing), `module.tvp` (inline, inline2,
  inlineclo, ipcp, spec, gconst, icf, devirt, each right and wrong) and
  `limits.tvp` (results no check applies to, which must stay `n/a` until
  one is written: tailcall, memory through a pointer, an SSA result whose
  promoted values cannot be run).
- `tv_pair_check` (pub, `compiler/tv.fd`) gives the verdict the mid-end
  gives: `tv_check` for function passes, devirt's witness plus random
  runs, `tv_mod_check` for the module passes.
- `tests/tooling/tv_pairs_main.fd` runs the pairs without a build of the
  compiler; the compiler runs them with `FID_TV_PAIRS=<file or dir>` (at
  `tv_report`). `tests/tooling/tv_pairs_wrong.tvp` holds three pairs
  whose expectations are wrong on purpose (and one malformed line): the
  runner must report all three. `tests/tooling/tv-check.sh` step 1b runs
  all of this.

Found by the pairs and fixed in `compiler/tv.fd`:

- `ssa` (the SSA round trip) replacing a promoted load by a parameter was
  rejected ("an operand ... changed its value"): the rule for parameters
  came before the rule that leaves promoted values to the random runs. A
  false alarm of the checker, for code lowering makes all the time (a
  parameter stored to its slot and read back). The order is swapped.
- When the witness had left promoted values to runs that could not be
  made (a load through a pointer), the result was accepted unchecked; it
  is now counted as not checkable (`g_tv_deferred`).
- `inlineclo` (iropt_me's once-called rule for closures, a module pass
  since Nov 17) had no witness; it now uses inline's.

Triage of the suite (Nov 17, with the compiler of Nov 16, whose `tv.fd`
lacks the Nov 17 changes): every ninth program of `tests/bootstrap` that
is not expected to fail, built with `FID_VALIDATE=1`: 73 built, 2,332
pass results checked, 0 rejected, 3,862 not checkable; 27 did not build
with that compiler at all (type errors against the newer tree, the same
without validation). So no pass result was rejected; what is open is the
share no check applies to (62%). `tv_report` now prints which passes
those results come from ("tv: not checkable, by pass: ..."), so the run
with the next build (`SUITE=1 tests/tooling/tv-check.sh`) shows where a
witness is worth writing next.
