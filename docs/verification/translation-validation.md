# Translation validation (VF6): scope and first steps

Status (Nov 7 2026): **open.** Nothing proves that an optimized function
means the same as the original. This page records what is checked instead,
the design chosen for the first validator, and what it needs.

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
