# autograd.fd — file analysis

- **Path:** `compiler/autograd.fd` — module `compiler::autograd`
- **LOC:** 41 (2026-11-09, unchanged). Rust `src/autograd.rs`: 32.
- **Role:** a no-op shim: `process_module(&mut IrModule)` does nothing.
  Gradients are computed at run time, not by an IR pass.
- **Status:** DEAD. Nothing imports it.
- **Re-audited:** 2026-11-09.

## Where it is called

**Nowhere.** No `use compiler::autograd` in `compiler/`, `stdlib/`,
`tests/` or `scripts/` (grep 2026-11-09), and no call of `process_module`
from this module. Not reachable from `main.fd`; no CLI command reaches it.
Rust calls its shim at `src/compiler.rs:1678,1689`; the port dropped the
call.

Imports: `core`, `compiler::ir`. Type-checks clean (`s18.exe check
compiler/autograd.fd`: no errors).

Where autograd actually lives now:
- `lower.fd:6304-6316` emits `GradBegin` / `GradEnd` / `Backward`.
- `interp.fd` (`fid run`): a real tape (`ag_tape`), `Interpreter_run_backward`
  (:6916-7017), `p_grad_get` / `p_grad_zero` (:7162-7177).
  `tests/bootstrap/test_autodiff_grad.fd` runs under `fid run`.
- `codegen_c.fd`: `p_autograd_begin/end/backward` and the C tape runtime
  (:1633-1635, :3904-3907).

## Findings

### From code inspection (`fid lint` not re-run)

1. **STILL TRUE, LOW — unused import.** `compiler::ir` is only used for the
   parameter type of an empty function; `core` is unused.

2. **CHANGED, MEDIUM — the header comment is half right now.** Lines 4-8
   and 31-37 say the interpreter builds a tape and computes gradients at
   `Backward`: that is now true (it was false on 2026-10-22). But the table
   at :9-18 says `mul` gives `grad_a = grad_out * b` and `matmul` gives
   `grad_out * b^T`; the interpreter passes `grad_out` unchanged to both
   inputs of `mul` and all-ones tensors to both inputs of `matmul`
   (interp.fd:6956-6958, :6965-6967), as `src/interp.rs` does. The comment
   describes math that no backend in this file's reach implements.
3. **FIXED (was MEDIUM) — "under `fid run` grad blocks compute nothing".**
   The interpreter now differentiates add, sub, scale, relu, sigmoid, sum,
   mean correctly; mul and matmul as in finding 2.
4. **STILL TRUE, LOW — whole-file dead code.** Either delete it or call
   `process_module` from `compiler.fd` after lowering; as an empty pass it
   changes nothing either way.

## Fix order

1. Fix the gradient table in the comment, or fix `mul`/`matmul` in
   interp.fd's `run_backward` (needs forward values on the tape).
2. Delete the module (preferred), or wire it as an extension point.

## Verification state

- 2026-11-09: file read; importers grepped; `s18.exe check` clean;
  interp.fd's tape code read; `test_autodiff_grad.fd` run under `fid run`.
