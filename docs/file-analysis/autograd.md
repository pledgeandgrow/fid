# autograd.fd — file analysis

> **Re-verified 2026-10-23:** still a 41-LOC no-op shim, zero importers, interp Grad*/Backward still no-ops. Accurate.

- **Path:** `compiler/autograd.fd` — 41 LOC
- **Role:** Documented no-op shim for reverse-mode AD; `process_module()` is
  intentionally empty (gradients are a runtime/tape feature, not an IR pass).
  Ported from `src/autograd.rs` (32 LOC, equally a no-op).
- **Analyzed:** 2026-10-22 — run 4 of 52 (alphabetical order)

## Where it is called

**Nowhere.** No `use compiler::autograd` exists in the tree — the module is
unreachable from `main.fd`'s import graph and `process_module` has zero Fides
callers. In Rust it's invoked from `src/compiler.rs:1678,1689` — the call
site was dropped in the port (harmless today since both are no-ops, but the
extension point is gone: filling this in later would silently do nothing).

Imports: `core`, `compiler::ir`. Leaf module.

## Findings

### Verified live

1. **LOW — unused import.** `fidesc lint` →
   `warning: unused import: 'compiler::ir'` — the `&mut IrModule` parameter
   doesn't count as a use for lint (or the type resolves without it).
   `use core` is equally unused.

### From code inspection

2. **MEDIUM — the header comment describes the wrong implementation.**
   Lines 4-8 + 31-37 claim gradients happen "at runtime in the interpreter…
   the interpreter builds a tape" and ops are "implemented in
   `compiler/interp.fd`". In fact `compiler/interp.fd:1616-1618` stubs
   `GradBegin`/`GradEnd`/`Backward` to `ExecOutcome::Continue` — **no tape,
   no `run_backward` in the self-hosted interpreter** (line 400 even says
   "autograd … stubbed"). The real Fides implementation is in the C backend:
   `codegen_c.fd` emits `p_autograd_begin/end/backward` + the `ag_tape`
   runtime (ML3, ML11 checkpoint mode). The comment was copied from the Rust
   design where `src/interp.rs:369-2202` *does* hold `autograd_tape` and
   `run_backward` — it's now misleading documentation pointing at a stub.

3. **MEDIUM — interp/backend divergence it hides:** under `fidesc run`,
   `grad { … backward(loss) }` silently computes nothing — `Backward` is a
   no-op, so a program can produce zero grads with no error. Only the C
   native backend actually differentiates. (Details belong to interp.fd's
   run; flagged here because this file's doc is what misdescribes it.)

4. **LOW — whole-file dead code.** Either delete the file, or re-add
   `use compiler::autograd` + `process_module(&mut module)` in `compiler.fd`
   at the Rust call sites to keep parity as an extension point. Keeping a
   doc-shim with no caller is fine too — but then its comment should at
   least be true.

## Suggested fix order

1. Rewrite the header comment to describe reality: tape engine lives in the
   C-backend runtime (`p_autograd_*`), `interp.fd` stubs the instructions,
   native codegen is the only backend that differentiates today.
2. Either delete the module or wire `process_module` into `compiler.fd`
   (post-lower, pre-opt — the Rust call sites are in that slot).
3. Drop unused `use compiler::ir` / `use core`.

## Verification state

- `fidesc lint` run (unused-import warning).
- `compiler/interp.fd:1616-1618` read: all three autograd insts → `Continue`.
- `src/compiler.rs:1678,1689` + `src/interp.rs:369-2202` checked: Rust calls
  the shim and implements the tape in its interpreter — the comment is true
  for Rust, false for Fides.
- `codegen_c.fd` grep: `p_autograd_begin/end/backward` emitted + `ag_tape`
  runtime present — the actual Fides implementation site.
