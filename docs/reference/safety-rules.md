# Safety rules and their evidence

Each rule `fid` enforces, the requirement it implements, and the tests that
show it holds (release L1–L4). `fid qualify <file.fd>` prints the same table
as JSON, says whether each test file is present, and lists which functions
of the program carry which rules.

This is evidence for a reviewer, not a certificate. No certification body
has assessed the toolchain, and Fides is not described as suitable for
medical, automotive or avionics use (release roadmap, Track L).

| Id | Rule | Requirement | Rejected by (negative tests) | Accepted by |
|---|---|---|---|---|
| SR-1 | `@no-alloc` | No dynamic allocation: no allocating call, builtin method (`push`, `insert`, `clone`, `split`…) or string `+` | `test_safety_method_alloc_fail`, `test_medical_fail` | `test_medical`, `test_safety_annotations` |
| SR-2 | `@no-recursion` | No direct or indirect recursion | `test_safety_recursion_fail` | `test_safety_annotations` |
| SR-3 | `@no-unbounded` | No `while` or `loop` without a bound | `test_safety_annotations_fail` | `test_safety_annotations` |
| SR-4 | `@no-panic` (`@crashfree`) | No `unwrap`, unchecked division, indexing or overflowing arithmetic | `test_crashfree_fail`, `test_crashfree_index_fail`, `test_crashfree_overflow_fail`, `test_crashfree_method_fail` | `test_safety_annotations` |
| SR-5 | `@no-indirect` | No call through a function value, closure or `dyn` | `test_safety_indirect_fail` | `test_safety_annotations` |
| SR-6 | `@requires-audit` | A `@medical` function carries `@audit` | `test_safety_no_audit_fail` | `test_medical` |
| SR-7 | transitive | An annotated function calls only functions with the same guarantees | `test_medical_fail`, `test_crashfree_method_fail` | `test_safety_annotations` |
| SR-8 | `@firmware` | No OS-dependent call (files, sockets, processes, environment) | `test_firmware_os_fail` | `test_os_features` |
| SR-9 | `@gas(N)` | Static cost within N; a loop with literal bounds counts per trip | `test_gas_fail`, `test_gas_loop_fail` | `test_gas` |
| SR-10 | `@bounded-stack` | Nothing reachable recurses or calls through a function value | `test_bounded_stack_fail` | `test_bounded_stack` |
| SR-11 | `@deterministic` | No IO, time or randomness effect | `test_deterministic_fail` | `test_deterministic` |
| SR-12 | `@tainted` | A tainted function or parameter reaches no IO or Net effect | `test_tainted_param_fail` | none yet |
| SR-13 | `@secret` | A secret reaches no output, global, branch or unmarked return | `test_secret_output_fail`, `test_secret_global_fail`, `test_secret_branch_fail`, `test_secret_helper_fail`, `test_secret_return_fail` | `test_secret` |
| SR-14 | `@constant_time` | No secret-dependent branch, early return or index | `test_constant_time_fail`, `test_constant_time_branch_fail`, `test_constant_time_index_fail` | `test_constant_time` |
| SR-15 | `sandbox` | Code in a sandbox uses only granted capabilities | `test_sandbox_fail`, `test_sandbox_alloc_fail` | `test_sandbox` |
| SR-16 | `@send` | A spawned call's arguments and captures are `@send` | `test_spawn_send_fail`, `test_send_fail` | `test_async` |
| SR-17 | borrows | No change to a `Vec` a `for` loop walks; no field assignment while borrowed | `test_iter_invalidation_fail`, `test_field_borrow_fail` | `test_borrowck` |

The tests are in `tests/bootstrap/`; each negative test names the error it
expects on its first line (`// expect-error:`), and the harness
(`fidesc test --compiler fid`) fails if the program is accepted.

## Known limits

- A method call is matched to a user method by name (`.m` → the first
  `Type_m`); the receiver's type is not used. A builtin method with the same
  name as a user method is checked as the user method.
- `@gas` counts one trip for a loop whose bound is not a literal; the runtime
  gas meter bounds those.
- The checks run in `fid`; `fidesc` (frozen) checks less: see
  [language-guide.md § 10](language-guide.md).
- `qualify` reports evidence only. The steps a certification needs (tool
  qualification plan, independent review, a qualified test suite) are in
  [safety-critical.md](../roadmap/safety-critical.md).
