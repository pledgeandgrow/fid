# 0006: Release M5 decided: ownership, not copy-on-write sharing

- **Status:** accepted (a decision record, no new language change).
- **Decision:** recorded on 2026-11-17 (release M4, M5; complaint CM3).
  Proposal 0004 (one ownership rule) supersedes proposal 0001's
  copy-on-write lowering. `var b = a` gives `b` a value of its own for
  `Vec`, `str`, `HashMap` and structs, by the ownership rule, and the
  lowering of 0001 (`LOWER_COW` in `compiler/lower.fd`) is removed.
  The parts of 0001 that stand: values are values (a copy never changes
  the original), `==` on a struct compares its fields, and a write to a
  buffer that two owners share copies it first (core's `vec_own`, rule 7
  of 0004).

## Problem

Release M4 asked what `var b = a` means for `Vec`, `str` and `HashMap`;
M4 answered "a value" (proposal 0001), and M5 asked for that to be
enforced. Two ways were written:

1. **Copy-on-write shares** (0001, `LOWER_COW`): the lowering inserted a
   counted share (`Vec_share`, `Vec_share_deep`, `HashMap_share`) at each
   copy it recognised: a `let` or assignment from a place, a stored struct,
   a returned value, a by-value parameter the body changes. Shares were
   left out for copies it proved were never written and for last uses.
   On Nov 6 the suite gave the same output with it on and off and the
   compiler reached its fixpoint with it on, at 4.7 GB peak (1.8 GB off).
   But who frees a share was a separate question, answered by two other
   mechanisms (manual frees and the auto-drop passes), and they disagreed:
   shares held by a struct were never dropped, call arguments and enum
   payloads were never shared, and turning `LOWER_COW` on with ownership
   frees values twice.
2. **Ownership** (0004, `LOWER_RC`, on since Nov 7): a variable owns its
   value; a place that keeps a value (a `let`, an assignment, a field, an
   element, a return, a payload) takes an owner of its own
   (`Lowerer_rc_own_value`: a temporary is handed over, anything else is
   duplicated, which for a `Vec` or `HashMap` is O(1) and copies the buffer
   at the first write); temporaries are dropped at the end of their
   statement; parameters are borrowed.

## Change

Release M5 is met by ownership. The copy-on-write lowering is retired:

- `compiler/lower.fd`: the `LOWER_COW` constant and every branch it
  guarded are removed (the share in `Lowerer_emit_inst` for a struct stored
  whole, the shares of `Lowerer_lower_value` and `Lowerer_emit_vec_share`,
  the share of a changed by-value Vec parameter in the function prologue,
  the drop of shared-into parameters in the auto-drop pass, and the
  "read-only copy" exemptions that only existed to skip a share).
  `Lowerer_emit_vec_share` and `Lowerer_lower_value` stay as the identity
  and a plain lowering for their callers; `LOWER_ARG_SHARE` stays off (a
  by-value parameter is the caller's, never handed over).
- The runtime keeps its copy-on-write: `Vec_share`, `HashMap_share` and
  `vec_own` are what `dup` of a `Vec` or `HashMap` and the first write
  after it use (0004 rule 7).

## What it breaks

Nothing: `LOWER_COW` was off in every build since Nov 7, so the removed
code never ran in a compiler built from this tree.

## Alternatives

- Keep both, behind the flag: rejected, since the two cannot be on together
  (double frees) and a dead path in the lowering keeps being read and edited
  as if it ran.
- Moves (use after move rejected) instead of copies: rejected in M4; a copy
  of a collection is O(1) here and the first write pays for it.

## Implementation

- The check: `tests/conformance/s5_3_copy_then_change.fd` copies a `Vec`,
  a `str`, a `HashMap`, a struct with `str` and `Vec` fields, a `Vec` of
  `Vec`s, a `Vec` of structs and a `Vec` field, changes each copy, and
  prints both sides; `python scripts/verify/expect.py --fid <fid>
  tests/conformance/s5_3_copy_then_change.fd` runs it on run, ccompile and
  emit. `tests/bootstrap/test_spawn_mutate_both.fd` ("copy" lines) checks
  the same with a task.
- Leaks: `scripts/leak-check.sh` stays the gate that the owners these
  copies take are dropped.
