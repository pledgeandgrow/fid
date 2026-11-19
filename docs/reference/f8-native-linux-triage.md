# F8: the native Linux suite under QEMU, Nov 17 triage

Release roadmap F8 asks for the suite on AArch64, RISC-V 64 and x86-64
Linux through the machine layer (`fid emit --target <arch>-linux`,
compiler/nat_ir.fd) to give the interpreter's output. This page records
the Nov 17 rerun, what each failure was, and what was fixed.

## How it was run

No compiler could be built during this run, so the runs used the
compiler of Nov 16 (`stage0`) on a snapshot of HEAD's `stdlib/`,
`scripts/` and `tests/bootstrap/` (the working tree's core.fd calls
`p_region_word`, which that compiler's interpreter does not know: every
reference that pushes to a Vec panicked). AArch64 ran the whole suite
(390 tests) under qemu-aarch64 in WSL. RISC-V 64 and x86-64 ran 152 of
those tests: every AArch64 failure and every third test, against the same
references.

```
NATIVE=1 JOBS=1 FID=<fid> FID_NAT_OFF=ldyn bash scripts/cross-test.sh aarch64-linux
```

`FID_NAT_OFF=ldyn` was needed with that compiler (finding 1).

| target | built | passed | failed | did not build | no reference |
|---|---|---|---|---|---|
| aarch64-linux (all 390) | 371 | 339 | 19 | 13 | 19 |
| riscv64-linux (152, SRA off) | 136 | 121 | 11 | 16 | 4 |
| x86_64-linux (152, SRA off) | 137 | 121 | 12 | 15 | 4 |

Every failure falls into one of the groups below. None is specific to
one architecture: the RISC-V and x86-64 failures are the AArch64 ones.

## Machine-layer bugs fixed (compiler/nat_ir.fd)

1. **Every Linux executable imported the runtime's own primitives.**
   stdlib/linux_runtime declares `nat_tls_set`, `nat_arch`,
   `nat_atomic_*`, `nat_clone_thread`, ... in an `extern "C"` block, and
   ng_gen_primitives defines them in the image. CI2's dynamic imports
   (ng_gen_call_std: an `extern "C"` name with no `pert_c_` or `nat_c_`
   version gets a stub bound by ld.so) took them for C functions. So
   every executable, even hello.fd, named `/lib/ld-linux-aarch64.so.1`
   and `libc.so.6` and imported 17 `nat_*` names. qemu-user stopped with
   "Could not open /lib/ld-linux-aarch64.so.1", and with a loader present
   the result was "symbol lookup error: undefined symbol: nat_tls_set".
   This is the PL2 lane failure. Fix: `ng_is_own_prim` (a `nat_*` name
   that ng_prim has placed) is never taken for an import. Check:
   tests/nattools/f8_static_prims_check.sh.
2. **A struct result in two registers lost its second word through a
   tail call.** ng_tail_at (OP13) tracked only the first result
   register. Take `StringBuilder::new()`, which returns
   `{ handle: p_sb_new(), len: 0 }`. Scalar replacement (iropt_me
   ime_sra) turns it into `call p_sb_new; mov x0, <handle>; mov x1, 0;
   return`. The call became a jump, so `len` was whatever p_sb_new left
   in x1. `sb.len()` printed an address in bench_stringbuilder,
   test_lang_features and test_derive_serialize, on all three targets.
   Fix: `ng_tail_own_rr2` makes no call a jump in a function that
   returns more than 8 bytes in registers (g.rr_size of its own label).
   Test: tests/bootstrap/test_machine_tail_rr2.fd. With the Nov 16
   compiler it prints `12 22` instead of `5 0` on every machine-layer
   target, and the right output with `FID_NAT_OFF=tailcall`.

## Fixed in the tree since HEAD (no change needed)

- test_binary_io: `p_file_write` wrote past the first NUL. Fixed by
  linux_runtime base.fd `pe_text_len`.
- test_files_spec: reading a directory as a file asked for a huge
  string. Fixed by VF14 in `pert_file_read`.
- test_constant_time, test_ct_asm, test_ed25519, test_x25519: the
  compiler crashed in ng_gen_function, which hashed a label as a string.
  Fixed by MI17 (`HashMap_insert_i(g.ct_labels, ...)`).

## Causes in other modules (left to their owners)

- **SRA of an enum's inline payload** (compiler/iropt_me.fd ime_sra).
  test_enum_struct_field, test_enum_struct_field2 and
  test_uninit_match_payload print an address or lose a field. With
  `FID_IROPT_OFF=sra` all three are right. This is the same cause, and
  the same hunk, as SP16's (docs/reference/sp16-native-selfhost.md).
- **Pure-call merging across a write to thread-local storage**
  (compiler/rules.fd purecse, with the effects from compiler/lower.fd
  `runtime_effects`). `p_tls_get` and `p_tls_set` get no effects there,
  and an empty effect set counts as read-only (iropt.fd
  iro_absent_call_mask). So `sync_tls_get(k)` after `sync_tls_set(k, 42)`
  was merged with the earlier get and returned 0. This breaks
  test_new_features. It passes with `FID_NAT_OFF=purecse`.
- **No bounds check on fixed arrays.** lower.fd emits a bare GetElemPtr
  for `a[i]` on `[T; N]`, so compiled code reads out of bounds where
  `fid run` panics. test_comptime_reflection2 prints 0 for `caps[0]` on
  an empty array instead of panicking.
- **Interpreter references that are wrong:**
  - test_async_suspend: the interpreter does not interleave tasks; the
    native order is what the test's comment describes.
  - test_metrics_serve: the C path agrees with native.
  - test_cfg: fid run does not find the platform function.
  - test_c_callback, test_http_server, test_lib_sys, test_ws_local: FFI
    or builtins the interpreter lacks.
  - test_zeroize_vec: the test prints "interp: skipped" under `fid run`,
    by design.
- **test_deref_minimal**: both builds panic. scripts/cross-test.sh
  `panic_key` strips " at line N" but not the native " at 0x0".
- **Did not build because of the front end, not the machine layer**,
  with the Nov 16 compiler on HEAD's stdlib:
  - `@secret` errors in crypto_hmac_sha256: real_crypto,
    test_crypto_random, test_crypto_tls_vectors, test_crypto_vectors,
    test_lib_ota.
  - `@crashfree` calling `StringBuilder_len`: test_crashfree_bounds,
    test_lib_scada.
  - Modules the snapshot does not hold: test_crypto_bind, test_use2,
    test_site_render.
  - Builtins native Linux does not have: test_kernel_hello (bare-metal
    serial), test_lib_asset (asset FFI), and on AArch64/RISC-V test_cfg
    (x86-64-only functions called from main).

  Rerun these with the final compiler: most are version skew between the
  compiler and the stdlib.

## Validation with the final compiler

```
bash tests/nattools/f8_static_prims_check.sh $FID
NATIVE=1 FID=$FID JOBS=4 bash scripts/cross-test.sh aarch64-linux
NATIVE=1 FID=$FID JOBS=4 bash scripts/cross-test.sh riscv64-linux
NATIVE=1 FID=$FID JOBS=4 bash scripts/cross-test.sh x86_64-linux
```

The first must end in `f8_static_prims_check: ok`. In each suite,
hello.fd, bench_stringbuilder, test_lang_features,
test_derive_serialize and test_machine_tail_rr2 must PASS without any
FID_NAT_OFF switch. Expect FAIL only for the tests in "Causes in other
modules" whose fixes have not landed.
