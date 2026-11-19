# Native suite on Linux and macOS, Nov 17 (complaint-roadmap CI4)

CI4 asks for Linux and macOS to be Tier 1 the way Windows is: native
executables with no C compiler or linker, the suite run through them and
compared with `fid run`, and CI that runs it. This page records what was run
on Nov 17, what it found, and what is wired into CI.

## What CI runs

| Where | Targets | What |
|-------|---------|------|
| ci.yml `natgate` | x86_64-linux (native), aarch64-linux and riscv64-linux (qemu-user) | `ALL=1 REQUIRE_Z3=1 REQUIRE_LOADER=1 scripts/nat-gate.sh <target>`: the whole suite through `fid emit --target`, each test against `fid run` (output, exit code, panic message), with the IR verifier and the allocation checker on |
| tier1.yml | linux-x64, linux-arm64 (ARM64 runner, native), macos-arm64 | `scripts/tier1.sh`: the C lane and the native lane, the lanes agreeing, expected-fail tests refused, every binary checked |
| tier2.yml, suite-all.yml | riscv64-linux, x86_64-macos | the native suite (`NATIVE=1 cross-test.sh`) |
| macos-native.yml | aarch64-macos (macos-15), x86_64-macos (macos-15-intel) | hello and six programs against `fid run`, macho-check.py, the runtime services, then the whole suite through the machine layer |

New on Nov 17: a native Linux program that calls C functions is a dynamic
executable (CI2), and qemu-user loads it only with the target's C library.
Without one, cross-test.sh lists such a test as `SKIP (dynamic: needs
...)`, and the gate passed without running it. The natgate job now
installs `libc6-arm64-cross libc6-riscv64-cross`, and nat-gate.sh's
`REQUIRE_LOADER=1` turns any such SKIP into a gate failure. suite-all.yml's
and tier2.yml's riscv64 jobs install `libc6-riscv64-cross`.
`scripts/ci-wiring-check.sh` checks all of this (lines tagged CI4).

## Sample run on this machine

There was no compiler with today's changes, and the tree may not be built
mid-run. The compiler was the session's stage0 (built Nov 16), with
`FID_NAT_OFF=ldyn`, because stage0 binds the runtime's own primitives
dynamically (see "Open" below). Each lane is `NATIVE=1 JOBS=1
scripts/cross-test.sh <target> <tests>`. The Linux binaries ran in WSL,
under qemu-user for AArch64 and RISC-V.

stage0's interpreter cannot run today's `stdlib/core.fd`: it stops with
"Unknown function 'p_region_word'" (the region work of another session).
Where that broke the reference, the reference was made again with stage0
over a snapshot of HEAD's `stdlib/`, or taken from the expected output
written in the test.

**x86_64-linux**: 39 tests (the nat-gate subset plus every 45th test of the
suite).
- 35 give the interpreter's output and exit code. 22 passed directly
  (test_int_semantics on a second run: the first build ran out of memory on
  a loaded machine). 13 pass against the second reference or the written
  output: test_hashmap, test_tasks, test_fs, test_vec_builtins,
  test_recursive_box, test_bigint, test_args, test_iter, test_for_enum_vec,
  test_plot2, test_runtime_vblk, test_stdlib_jsondoc_loaders,
  test_variadic_args.
- 1 real difference: **test_enum_struct_field2** prints 40, where `fid run`
  and C print 82. See below.
- test_generic_struct_return also fails through C with stage0. The fix the
  test describes is newer than stage0, so this is not a native difference.
- 2 did not build: stage0's checker rejects test_crypto_tls_vectors
  (@secret) and test_lowering_async_cap_default (Future), which were
  written for today's checker.

**aarch64-linux, riscv64-linux**: the same 12 tests on each (int semantics,
checked arithmetic, closures, floats, threads, strings, traits, an overflow
panic, test_enum_struct_field2, test_machine_remat, test_tco, test_time).
11 of 12 pass on each target. The one failure is test_enum_struct_field2.

**aarch64-macos**: built only (no Mac here): hello, test_threads, test_fs,
test_closure, test_panic_overflow_fail, test_tasks. All six built.
`scripts/macho-check.py` reports each one as a well-formed, signed Mach-O,
and `scripts/bin-target-check.py aarch64-macos` reports 6 files, 0 bad.

## The difference found: ME26 drops a struct copied into an enum payload

Here is a struct parameter wrapped in a variant:

```
pub fn wrap(p: P) -> OptP { return OptP::Some(p); }
```

The lowering copies `p` into the enum's inline payload with one typed store:
`Store(fieldptr(enum, 1), p, Some(struct.P))`, a copy of the whole struct
through the address of payload word 1. The mid-end's scalar replacement
(compiler/iropt_me.fd `ime_sra`, ME26, on by default since Nov 16) treats
that field address as a one-word slot, because its check accepts every
Store. It then splits the enum local into one local per word. The struct
copy writes 16 bytes or more into the 8-byte local of word 1, and the
locals of the other payload words are never written. So every native
backend (pe_ir, `emit --nat` and the Linux and macOS targets) keeps only
the payload's first word. The interpreter and C (which do not run the
mid-end) are right.

`FID_IROPT_OFF=sra` gives the right output. The fix (in needsOther, for the
mid-end's owner) makes `ime_sra` reject a field address that is used by a
Load or Store of a non-scalar type. The new test is
`tests/bootstrap/test_native_enum_payload_param.fd`. With stage0 and pe_ir
it gave wrong numbers on 5 of its 7 lines, and with `FID_IROPT_OFF=sra` it
gave all 7 right. test_enum_struct_field2 and the new test are now in
nat-gate.sh's default subset.

## Open

- **nat_ir binds the runtime's primitives dynamically** on Linux targets
  (`nat_tls_set`, `nat_frame_fp`, ...: base.fd declares them in an
  `extern "C"` block, and the CI2 path makes every such name a libc
  import). Every native Linux binary then dies at load with "symbol
  lookup error". Until compiler/nat_ir.fd guards the CI2 branch, every
  native Linux lane fails. This run worked around it with
  `FID_NAT_OFF=ldyn`.
- The ME26 fix above.
- The whole suite with this run's compiler on all three Linux targets
  (the validation phase), and a recorded green CI run.
- macOS: nothing has run on a Mac yet. macos-native.yml and tier1.yml's
  macos-15 job run it after a push.
