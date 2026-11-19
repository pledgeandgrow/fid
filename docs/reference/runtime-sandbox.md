# Run-time sandbox

`sandbox { body }` takes every capability away from the code in `body`.
Two checks enforce it.

1. **When the program is compiled** (all backends). The type checker
   (`compiler/typechecker.fd`, `sandbox_depth`) rejects any call in the
   body that needs a capability: IO, files, network, processes,
   environment, C functions, loaded libraries, assembly. It also rejects a
   call through a function value it cannot follow (rule SR-15 in
   [safety-rules.md](safety-rules.md)).
2. **While the program runs** (`fid run`). Lowering
   (`compiler/lower.fd`, `Lowerer_lower_sandbox`) puts `p_sandbox_enter()`
   before the body and `p_sandbox_exit()` on every way out of it: the
   normal end, `return`, `?`, and `break`/`continue` of a loop outside the
   block. Nested blocks nest. Between the two calls the interpreter
   (`compiler/interp.fd`, `Interpreter_cap_builtin`) revokes every
   capability. A builtin that needs a revoked capability does not run, and
   the program stops with:

   ```
   Sandbox error: 'p_file_exists' needs capability 'FS', which was revoked at run time
   ```

   This stops what the type checker cannot see, for example a function
   value held in a struct and called by a helper
   (`tests/tooling/sandbox_block_probe.fd`).

A program may not call `p_sandbox_enter` or `p_sandbox_exit` itself.
Otherwise a function reached from inside a sandbox could lift the sandbox
it runs in (`test_sandbox_exit_direct_fail`). Two builtins stay callable:

- `p_cap_revoke(cap)` gives up `"IO"`, `"FS"`, `"Net"`, `"Spawn"`,
  `"Env"`, `"FFI"` or `"all"` for the rest of the run. It returns 0, or -1
  for an unknown name.
- `p_cap_revoked(cap)` returns 1 when `cap` is revoked, else 0.

## Compiled programs

The run-time calls go to core's `p_sandbox_enter` and `p_sandbox_exit`,
which do nothing. In compiled programs the compile-time check is the one
that holds. Lowering emits the calls only when core defines both
functions; without them a sandbox block lowers as it did before.

Not done:

- The C runtime has a per-thread capability mask (`__p_caps`, CI6) that
  could enforce the files, network, processes and environment part in
  compiled programs too.
- `p_cap_revoke` and `p_cap_revoked` exist only in the interpreter, so a
  program that calls them directly runs only under `fid run`.

## Tests

- `tests/bootstrap/test_runtime_sandbox_block.fd`: every way out of a
  block lifts the revocation. The output is the same on every backend.
- `tests/tooling/runtime-sandbox-block-check.sh`: the probe above stops at
  run time, the block test passes under `run` (and C when `CC` is set),
  and core defines `p_sandbox_enter`/`p_sandbox_exit`.
- `tests/tooling/runtime-sandbox-check.sh` with
  `tests/bootstrap/test_runtime_sandbox_fail.fd`: `p_cap_revoke` and a
  sandbox block under `run`.
- `tests/bootstrap/test_sandbox_exit_direct_fail.fd`: a direct call is
  rejected.
