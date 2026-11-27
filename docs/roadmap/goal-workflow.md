# Goal workflow — the Nov 27 night run

> Created Nov 27 at about 02:10, at the user's request: "handle goals one by
> one till each is finished validated and mark done" for the open goals of
> `bugs-roadmap.md`, `competitive-roadmap.md` and `llvm-roadmap.md`. A
> scheduled run works the queue below every 30 minutes and follows the
> procedure here. Delete this file once the queue is closed.

## Hard rules

1. **One folder.** Work only inside this repository. Never a worktree, an
   export or a copy of the tree anywhere else.
2. **Another session edits this tree at the same time.** Before you touch a
   file, run `git status --porcelain -- <file>`. If it is modified or staged
   and you did not edit it yourself in this run, do not edit it and do not
   commit it: mark the goal `waiting` in the queue and take the next one.
   The reverse happens too: that session commits by sweeping the tree, so
   files of yours that are not yet committed can land inside its commit
   (be363179 took the half-checked BG223 test). Keep the edits of a goal
   uncommitted for as short as possible and commit them the moment their
   validation passes.
3. **Commit only your own files, by path:**
   `git add -- <files>` then
   `git -c user.name="Mehdi Berel" -c user.email="pledgeandgrow@gmail.com" commit -m "<one short line>" -- <files>`.
   Never `git commit -a`, never `git add .`, never push, never `git stash`,
   never `git reset`, never `git checkout` a file, never `git clean`, never
   delete a file you did not create.
4. **Done means proved.** Change a goal's mark to `[x]` only when the entry's
   own definition of done holds on this machine, and write what ran into the
   entry: a dated note with the numbers and the binary. Otherwise leave the
   mark and add a dated note with the precise gap.
5. **One heavy job at a time.** A compiler build and a whole-suite run never
   overlap each other or another session's compile. Before a build, at least
   8 GB of memory must be free:
   `powershell -NoProfile -Command "[math]::Round((Get-CimInstance Win32_OperatingSystem).FreePhysicalMemory/1024)"`.
6. **A result under pressure is not a result.** "out of memory" in a compiler
   log, a build that dies with no error text, a shell `fork` failure: wait for
   memory and run it once more before recording anything.
7. **Time.** A run works for at most 80 minutes. It finishes the goal in its
   hands, writes the queue, and stops.

## Which compiler validates what

- **Standard library, `tools/`, tests:** read at run time, so
  `release/fid-v27.exe` (a copy of the fid-dev built Nov 25) validates them
  with no build.
- **A change in `compiler/*.fd` needs a build**, to a side name, never over
  `release/fid.exe` or `release/fid-dev.exe`:

  ```
  OUT=build/selfhost-night STAGE0=release/stage0/fid.exe SPLIT=16 JOBS=2 CC="zig cc" bash scripts/build-selfhost.sh
  ```

  15 to 30 minutes with the machine shared. Its last line must be
  `OK: build/selfhost-night/fid-dev.exe` and the one before it must say
  `(16 units at -O2)`: anything else means the split was not taken. Then
  `cp build/selfhost-night/fid-dev.exe release/fid-night.exe` and validate
  with `release/fid-night.exe`. One build per batch of fixes, at most two per
  run.
- **Three backends for every new or changed test:**

  ```
  CC="zig cc" python scripts/verify/expect.py --fid release/fid-v27.exe --backends run,ccompile,emit <test.fd>
  ```

  Run a new test through the interpreter once before writing its expected
  output into it.

## Procedure for each goal

1. Read the entry in its roadmap.
2. **Reproduce.** Find or write the test (for a bug, `tests/bootstrap/test_bg<N>_<name>.fd`
   with the entry's trigger and an `// Expected output:` block). If it already
   behaves, the fix is in the tree: go to step 4.
3. **Fix**, the smallest change that makes the entry's rule true.
4. **Validate:** the test on the three backends, and the gate named in the
   queue.
5. **Record:** the mark and a dated note in the entry (`Nov 27 (night): ...`),
   the status in the queue, then commit by path.

## First: record the background runs

**Nov 27 afternoon and evening (the interactive session): all its jobs are
done and recorded.** The whole of tests/bootstrap (2,056 programs) ran with
`release/fid-night.exe` (the -O2 build of 16:19) four times: `on` (emit and
`--nat`, the default: WP14 and LF11), `off` (emit, FID_IROPT_OFF=spec,
version), `sole` (emit, FID_IROPT_ON=soleown: LF16) and `vec` (emit,
FID_IROPT_ON=vecsroa: LF14); `python build/llvm19/suite-diff.py on off sole
vec` compares them (logs build/llvm19/suite-<name>.log and .raw). 2,011 of
2,056 pass on emit and `--nat` together; no verdict changes with any pass
on or off. The gates RA1, LF11 and SP9 ran after them (`FID=<fid> bash
build/llvm19/gates.sh ra1 lf11 sp9`, logs build/llvm19/<gate>.log).

**Nov 27, 23:55: nothing of this workflow runs.** The chain of the 19:10
run is done (CHAIN_DONE at 21:05) and its legs are in MI14. Its
BACKEND=nat fixpoint had not started (the script passed
`emit --nat compiler/main.fd`, fixed in f7a61a6e); the 23:10 run ran it
by hand in four stages (`build/llvm19/fix-nat2`, stage2.log, stage3.log,
stage3b.log, stage4.log) and the result is in RL14: it needs 8 GB of
commit free, and died once under 5 GB. While a whole-suite run or a build
of this workflow runs, start no other.**

| Marker | Result | Roadmap entries |
|---|---|---|
| `MI14_LEGS_DONE`, then `CHAIN_DONE`, at the end of `build/llvm19/chain-1910.log` | the whole of tests/bootstrap on `--nat` Windows with `release/fid-night.exe` (the -O2 build of 16:19), three times: `nat` (the default), `natwin` (FID_NAT_OFF=winfregs), `natsplit` (FID_NAT_OFF=splitfloat); `python build/llvm19/suite-diff.py nat natwin natsplit` gives, per leg, the files it fails that the default `--nat` run does not. Then scripts/native-fixpoint.sh with BACKEND=nat (`build/llvm19/fix-nat.log`, `NAT_FIXPOINT_DONE` at its end) | `MI14` (no file fails with a float switch off that the default `--nat` run passes), `RL14` (the `--nat` host) |

Started at 01:26 with `release/fid-v27.exe`, output in `build/verify-nov27/`.
When a marker appears and the entry has no note of it, write the note, then
set the queue row to `done`.

| Marker | Result | Roadmap entry |
|---|---|---|
| `MI5_DONE` in `mi5.log` (done 04:07; re-runs in `mi5-rerun`, `mi5-rerun2`) | 1,287 same of 1,331 after re-running the rest on their own | `MI5` and `MI17`: written Nov 27 |
| `WASM_DONE` in `wasm-all.log` (done 06:17) | `fid wasm: 1116 passed, 35 failed, 25 did not build, 139 skipped (of 1315)`, beside a build | `PL4` in `competitive-roadmap.md`: written Nov 27 07:30; the re-run of the 154 listed programs on a quiet machine is left |
| `BENCH_DONE` in `mi14_bench.log` | the `pe_ir ... nat ... nat/pe_ir` lines | `MI14` in `llvm-roadmap.md`: written Nov 27 (ee0b0830) |

Before counting a `DIFF`, `NATBUILD`, `PEBUILD` or `NOBUILD` line of the
comparison, build that program again on its own
(`FID=release/fid-v27.exe FILES="tests/bootstrap/<name>.fd" bash tests/nattools/mi5_nat_peir_check.sh`).

Already known from the evening and still to be written into the entries:

- **MI14:** `machine_mi14_check.sh` 25 ok, 10 fail (4: the script takes
  `wsl.exe` on PATH for a working WSL; 6: it expects the splitter to keep
  `around` on all four targets, outputs right). `machine_fregs_check.sh` 21
  ok, 4 fail (3 WSL; 1: its disassembly pattern counts the scratch registers
  of a runtime `round` routine). `mi7_call_parity.sh` 13 same, 0 differ (one
  re-run after an out-of-memory under load). `--nat` still opt-in
  (`compiler/main.fd`: `nat_win` only with `--nat`).
- **PL4 so far:** default WebAssembly set 13 of 13, `task-switch-check.sh`
  3 of 3, `stackify-check.sh` 12 of 12.

## Queue

Status is one of `todo`, `running`, `done`, `waiting` (another session has
the file), `deferred` (cannot be done on this machine), with the reason.

**Nov 27, 11:57: the user stopped the bugs roadmap.** Sections B to G are
the record of the night and are not worked on any more (rows 26 to 41 name
another session's files). The queue is the LLVM roadmap since 12:00, at
the user's word: help that session finish, verify and mark done the goals
whose code lives in the files it edits. Rule 4 (a goal whose file another
session edits is skipped) does not hold for them: edit outside its hunks
and commit only your own (a hunk of a shared file goes in alone, with `git
apply --cached`). A mark becomes `[x]` only when the entry's own definition
holds, with its gate green on a built compiler.

### A. The LLVM roadmap (docs/roadmap/llvm-roadmap.md)

| # | Goal | Files | Status |
|---|------|-------|--------|
| L1 | SP6 | `compiler/nat_ir.fd` (the function cache) | done (Nov 27 day: `machine_fncache_check.sh` all passed, `sp20_fncache_tail_check.sh` 21 ok) |
| L2 | SP7 | `compiler/nat_ir.fd` (`nl_loop_end`) | done (Nov 27 day, ad4dfa83: a kept slot for a function a build inlines away; `relink_check.sh` 21 ok, 0 fail on the -O2 build of 15:52) |
| L3 | LF20 | `compiler/nat_ir.fd` (`ng_secret_mir`) | done (Nov 27 day, 5c5d0644: `machine_lf20_check.sh` 44 ok, `machine_lf20_param_check.sh` 22 ok; the 38 constant-time tests on run, ccompile, emit and `--nat`, the last under FID_VERIFY=1) |
| L4 | LF14 | `compiler/iropt.fd` (`iro_vec_sroa_one`) | done (Nov 27 evening, 748df914: the whole of tests/bootstrap on emit with FID_IROPT_ON=vecsroa is the same as with it off; 5 of the test's 5 Vecs) |
| L5 | MI14 | `compiler/nat_ir.fd` (another session's file) | gate green (33 ok; fregs 25 ok); the two legs are in the entry (the 23:10 run: with each float switch off, no file fails that the default `--nat` run passes, once built on its own); 7 `--nat`-only failures of the whole suite are in the entry; left: the default switch (the loop 2.34x, the sieve 3.56x slower on `--nat`); waiting: the file is another session's |
| L6 | SP16 | `compiler/pe_ir*.fd` | done (the 19:10 run: `fid emit` builds the compiler in 5 minutes and it is the same compiler; scripts/native-fixpoint.sh on pe_ir: stage 2 == stage 3; known-bugs 2g.114 keeps the six unresolved labels) |
| L7 | TG11 | `compiler/nat_ir.fd` (the vectorizer; another session's file) | waiting (the file is modified by another session): verified (the 19:10 run): vectorize_spill_check.sh 32 ok; its 6 failures are the reductions red6 and redk; left: those, then MIR vector operations |
| L8 | WP6 | `compiler/nat_ir.fd` (AArch64's pool) | waiting (the file is modified by another session): verified: wp6_args_check.sh 23 ok and the native suite ran; left: x0-x7 in AArch64's pool (a change of the machine layer's design) |
| L9 | SP9 | `compiler/compiler.fd`, `lexer.fd`, `parser.fd`, `borrowck.fd`, `escape.fd`, `secrecy.fd` (another session modifies all six) | waiting (the files are modified by another session): verified Nov 27 evening: the four checks pass and the bench is in the entry; left: on by default, step 3 (type check in parallel), parallel code generation |
| L10 | RA1 | `tests/nattools/ra1_pe_ra_off_check.sh`; `compiler/pe_ir*.fd` (another session's) | waiting (pe_ir*.fd is modified by another session): verified Nov 27 evening: 132 same, 0 differ after the gate's second chance, switch takes effect; left: deleting pe_ir's allocator (waits for MI17 and the Windows default) |
| L11 | LF11 | `tests/tooling/version-default-check.sh` | done (Nov 27 evening, be77cdf3: the gate 0 failed; the whole suite the same with versioning off) |
| L12 | WP14, LF16 | whole-suite legs | done (Nov 27 evening, 748df914: the whole of tests/bootstrap on emit and `--nat`, 2,011 of 2,056; no verdict changes with any pass on or off; LF14 the same) |
| L13 | MI5, MI17 | `compiler/pe_ir*.fd`, `nat_ir.fd` | waiting: BG248, BG249, BG251 (the other session's), then `tests/nattools/mi5_nat_peir_check.sh` again; the default switch is MI14's |
| L14 | MI9, TG16, LF13, LF21 | MI9: SRA of local structs (`nat_ir.fd`); TG16: the port onto MIR (the other session builds tg16-fid-v5); LF13: the other producers, `Vec::new()`+push buffers (`lower.fd`); LF21: the region arena in the Linux runtime | deferred: multi-week design work in files another session edits (each entry says what is left); not a goal of an 80-minute run |
| L15 | RL14 (not among the 19; its run is this run's) | `scripts/native-fixpoint.sh` | pe_ir: done (the 19:10 run: stage 2 == stage 3 byte for byte). `--nat` (the 23:10 run): stage 3 == stage 4 byte for byte, stage 2 != stage 3 (two compilers: stage 1 is the C build of 16:19), in the entry; it needs 8 GB of commit free (the cache writer's 1 GiB vector). Left: a stage 1 of the same source (a C build of the current tree, when its files are free) and the three Linux hosts (not on this machine) |

Gate scripts, once fixed, are in tests/nattools (a working WSL only: `wsl.exe -e sh -c 'echo wsl-ok'`; a path given to `fid serve` on stdin is a Windows path, `cygpath -m`). The driver of one run: `FID=<fid> bash build/llvm19/gates.sh [name ...]` (tg11 wp6 sp16 mi14 fregs lf20 sp6 sp20 sp7 ct ra1 lf11 sp9), its log `build/llvm19/progress.log`; the whole-suite legs: `bash build/llvm19/suite.sh <name> <list-or-dash> [VAR=value]` (tests/bootstrap on emit and `--nat`).

**The machine's commit limit is the build wall, not free RAM (Nov 27 afternoon):** 44 GB of the 48 GB limit were taken with 8 GB of RAM free, and `zig cc` died at once on the giant units. Check `CommitLimit - CommittedBytes` before a build. `SPLIT_OPT=-O1 JOBS=1` builds every unit when at least 4 GB of commit are free; the six giant units (1, 3, 4, 6, 7, 9) fit at -O0 only. A mixed -O1/-O0 fid validates behaviour, not timings.

### B. Bugs in the standard library — no build needed

| # | Goal | What is wrong | Files | Gate | Status |
|---|---|---|---|---|---|
| 1 | BG215 | UTF-8 decoders accept overlong, surrogate and out-of-range sequences | `stdlib/encoding.fd` (`utf8_decode`), `stdlib/unicode.fd` (`uni_to_codepoints`) | the entry's trigger bytes give U+FFFD; `uni_lower`/`uni_nfc` of `.. C0 AF x` keep the bytes | done (Nov 27 02:15) |
| 2 | BG217 | decimal: a negative scale prints the wrong value | `stdlib/decimal.fd` (`dec_round`, `dec_div`, `dec_to_str`) | `dec_round(dec_parse("1234"), -2, DEC_HALF_UP)` is 1200; `dec_div(1000, 3, -1, DEC_HALF_UP)` is 330 | done (Nov 27 02:20) |
| 3 | BG238 | `Money_print` writes negative amounts wrong | `stdlib/money.fd` | `Money_new(-5)` prints `-$0.05`, `-105` prints `-$1.05` | done (Nov 27 02:21) |
| 4 | BG239 | `hkdf_expand` does not reject a length over 255 * 32 | `stdlib/tls_crypto.fd` | a length of 8161 is an error; RFC 5869 vectors still pass | done (Nov 27 02:33) |
| 5 | BG223 | `url_resolve` builds broken or lossy URLs; path decoded too early | `stdlib/url.fd` | the entry's triggers (IPv6 host, userinfo, `%2F` in the path) | done (Nov 27 02:43; `Url.path` is now the path as written) |
| 6 | BG216 | `tls_crypto::x25519` gets RFC 7748's second vector wrong | `stdlib/tls_crypto.fd` (`x25519`: mask bit 255 of `u`; a constant-time swap) | RFC 7748 section 5.2 vectors 1 and 2; slow in the interpreter, so through `ccompile` | done (Nov 27 02:57; it now calls x25519.fd's ladder) |
| 7 | BG224 | `xml` and `toml` have no nesting limit | `stdlib/xml.fd`, `stdlib/toml.fd` | deep input gives an error, not a stack overflow | done (Nov 27 03:55; `XML_MAX_DEPTH`, `TOML_MAX_DEPTH`, both 128) |
| 8 | BG180 | f-string float precision wrong for large values; fill ignored | `stdlib/core.fd` (`fstr_fmt_f64`, `__fstr_pad`, `__fstr_width0`) | the entry's triggers; check first whether it still reproduces | done (Nov 27 04:05) |
| 9 | BG247 | the default Windows backend's `float_to_str` loses the low digits of a large value (found by BG180) | `stdlib/pe_runtime/base.fd` (`pert_float_to_str`; the machine layer's, in `stdlib/linux_runtime/base.fd`, is right) | `f"{1e25}"` prints `10000000000000000905969664` through `emit` | done (Nov 27 04:25) |
| 9 | BG206 | Linux runtime `pert_str_to_float` is not correctly rounded | `stdlib/linux_runtime/base.fd` | 2000 printed doubles read back; runs here through `emit --nat` | done (Nov 27 10:05, a88c72b3: tests/nattools/str_to_float_exact_check.sh, 2,000 round trips and 1,107 texts, all as Python's float()) |
| 42 | BG254 | `use transform3d;` is a parse error (transform3d, lidar, pointcloud) | `stdlib/transform3d.fd` 146 | the three modules build on their own (scripts/check-stdlib.sh) | done (Nov 27 08:05, 1ae02075; the parser's part is BG257, row 29) |
| 43 | BG255 | `use quic;` is a `@secret` error in the library (quic, quic_conn, quic_frame, quic_tls, quic_udp, http3, http3_udp) | `stdlib/quic.fd` 359 | the seven modules build on their own; the QUIC and HTTP/3 tests unchanged | done (Nov 27 08:11, dbb3347c and aea3663b: the 7 modules; the 5 tests pass on ccompile and emit with both compilers) |
| 44 | BG256 | `use parallel;` is a thread-safety error on its own | `stdlib/parallel.fd` 138 | the module builds on its own; tests/par/par_check.sh still passes | waiting on a design decision: a function value cannot reach a spawned task as an argument, as a used variable, or through a cast (the entry's note of Nov 27) |
| 45 | BG253 | tests/selfhost/test_mono.fd no longer compiled (the maps became structs, SP10) | `tests/selfhost/test_mono.fd` | the test passes on run, ccompile and emit | done (Nov 27 10:12: "All tests passed!" on the three) |
| 46 | BG249 | each task's stack is committed in full on Windows (32.8 GB for 100,000 sleepers) | `stdlib/linux_runtime/win.fd`, and a hook in `conc.fd` (`pe_task_stack_new`) | bench_sleepers with its 100,000 tasks runs through `emit --nat`; the task tests (test_task_*.fd, tests/par) unchanged on the native backend | todo (the cause is located; the fix was written and tried Nov 27 10:55 to 11:12 and not validated: it works in one build and fails in the next; the entry has what was seen; needs a debugger) |
| 47 | BG207 | a lone CR on a Windows console reads as the end of the input | `stdlib/linux_runtime/win.fd` (`pe_win_read`) | needs a real console | deferred: no console on this machine |

### C. Bugs in compiler files no other session edits — one build per batch

| # | Goal | What is wrong | Files | Gate | Status |
|---|---|---|---|---|---|
| 10 | BG233 | disasm: one-byte forms ignore the `0x66` prefix and REX.B | `compiler/disasm.fd` | `66 B8 34 12` is `mov ax, 0x1234`, `66 99` `cwd`, `49 90` `xchg r8, rax`, `F3 90` `pause`, each with the right length| done (Nov 27 05:47, 4445b05d: 7 of 7 in tests/tooling/disasm-forms-check.sh with the compiler built from 7b06ce65) |
| 11 | BG208 | disasm: `vzeroupper` swallows the next instruction | `compiler/disasm.fd` | `c5 f8 77 4d 39` is `vzeroupper` then `cmp r12, r14`| done (Nov 27 05:47, 4445b05d) |
| 12 | BG232 | disasm has no row for instructions the encoders emit | `compiler/disasm.fd` | the entry's list (x86 `blsmsk`/`blsi`, AArch64 pre/post-index and FP loads, RISC-V vector) decodes with the right length | done (Nov 27 09:21, 48a42687: disasm-forms-check 30 of 30, enc_table_unit ok, disasm-check 18 ok with 0 undecoded words on AArch64) |
| 13 | BG212 | comptime does not evaluate float/int casts | `compiler/comptime_eval.fd` (`Expr::Cast`) | `comptime avg(3, 4)` and `comptime (-(7.9 as i64))` evaluate| done (Nov 27 05:47, 4445b05d: its two tests and 321 of 324 related programs; the three fail with the compiler of Nov 25 too) |
| 14 | BG213 | C header parser evaluates unsigned macro arithmetic as signed | `compiler/cparser.fd` (`CpEval`/`CpExpr`) | `fid parse-header` on a header with `UDIV`, `UNEG`, `MASK`, `COND`, `CMP`, `0b1010`| deferred to a later batch: needs a typed macro evaluator (unsigned 32-bit arithmetic), about fifteen functions |
| 15 | LG3 | reads are not checked against a live `&mut` | `compiler/borrowck.fd` | `val v = m` while `r = id_mut(&mut m)` is live and used after is a borrow error; `test_frontend_lifetime*.fd` and `tests/conformance/s8_*` still pass| the rule of 7b06ce65 passed its tests but rejected the compiler's own tree (three calls of the form `f(&mut g.e, read(g))`); refined in 21e16421; the build of 06:35 still rejected one place (nat_ir.fd ng_mir_finish: a released borrow was counted as a named one), fixed in 4d7dcd55 with test_lg3_read_after_released_mut; validated with the build of 07:41 (three rejections, the accepted tests, `check compiler/main.fd` clean, 154 of 155 of the lifetime and §8.4 files); the entry stays `[~]` for its other gaps (lifetime parameters by name, generic structs by lifetime, the Linux targets) |

### D. Verify only — the gate runs on this machine

Read the entry first; its last line says what is left. Run the gate when the
machine is free of the two heavy runs; record the numbers.

| # | Goal | Gate | Status |
|---|---|---|---|
| 16 | VF10 | the conformance suite on every backend: `CC="zig cc" python scripts/verify/expect.py --fid release/fid-v27.exe --backends run,ccompile,emit tests/conformance`, then `python scripts/verify/conf-known.py --log <that output>` | run once (Nov 27 10:48: with release/fid-v27.exe 1,607 of 1,617 pass on run, ccompile, emit; the 15 listed gaps pass and are off the list; CF32 to CF37 filed; four tests of the night's fixes fail with that compiler only and pass with the build of 09:17) |
| 48 | CF32 to CF37, found by the conformance run of Nov 27 | `@public` taken for secret by the compiled backends' constant-time check; `main` without `return` on emit; two closure captures (by reference; a str printed as an address); `comptime if` in a generic on emit; a null dereference that does not trap on ccompile | `compiler/checks.fd` and `compiler/nat_ir.fd` (CF32), `compiler/pe_ir*.fd`, `compiler/codegen_c.fd`, lowering (CF33 to CF37) | their six conformance tests pass on run, ccompile and emit | todo (docs/verification/vf10-conformance-2026-11-17.md). CF32: the native backend is fixed in c437509b (build of 11:30: s11_annotation_on_param passes on run and emit; 38 constant-time tests pass on run, ccompile and emit); the C generator has its own copy of the check, codegen_c.fd `cg_ct_check`, which still marks every parameter: the line checks.fd `peir_ct_check` got, one build; the accept test (a function, a method, a generic function) waits in build/verify-nov27/cf32/ until ccompile passes (with the patch of the change); then the front-end verdicts of build 6 (release/fid-night-prev.exe) against the new build. CF33 to CF37 not started. Another session is in compiler/typechecker.fd (4 hunks of its own), lower.fd, lexer.fd, interp.fd, escape.fd and nat_ir.fd, with tests for BG203, BG225 and BG252 in the tree: row 48's other files, and rows 28 and 34, wait for it | Nov 27 (day): CF32 and CF37 done (cc293424): the type checker's marks are read by their index in the source on every backend (the -O2 build of 16:19: s11_annotation_on_param and s8_2_null_deref_panics pass on run, ccompile and emit), taken out of known-gaps.txt. CF33 to CF36 stay with the stopped roadmap. |
| 17 | VF15 | `scripts/fuzz/fuzz_parsers.py` (read its header) | the tls target is run (Nov 27 09:38: 6,600 inputs over two seeds, 0 findings; the other eight parsers 125 each, 0); the entry stays `[~]` for long runs |
| 18 | AD4 | `tools/doctest.fd` against `scripts/doctest.py` (read the entry) | todo |
| 19 | TL10 | `tree-sitter generate` for `editors/tree-sitter-fides`, Zed (read the entry) | todo |
| 20 | SP13 | the debug mode's timing (read the entry) | todo |
| 21 | SL17 | files, paths and processes on `run`, `ccompile`, `emit`, `--nat` (read the entry) | todo |
| 22 | RA9 | the greedy allocator on the suite (read the entry) | todo |
| 23 | WP7 | `SUITE=1 bash tests/tooling/wp-defaults-check.sh` (heavy) | todo |
| 24 | RL9 | `FID=<bin> bash scripts/image-check.sh`: 211 pass, 15 fail on the debug-info defect below until it is fixed | done as far as this machine goes (Nov 27 08:26: 226 of 226 with release/fid-night.exe; RL9 stays `[~]` for loading on the targets and otool) |
| 25 | TL6, RL25, RL1, RL3, VF2, VF1, VF18, VF6 | a recorded run each; read the entry, run what fits the budget | todo |

### E. In flight in another session — do not start

BG159, BG165, BG166, BG182, BG183, BG185, BG204, BG227, CS6, SP6, SP7, SP8,
CB3: their tests are in the working tree. Take them only when their row in
the roadmap is `[x]` and the test is committed; then run the gate and record.

### F. Waiting: the files are staged by another session

Take in this order as soon as `git status --porcelain -- <file>` is empty.

| # | Goal | Files |
|---|---|---|
| 26 | DWARF: `DW_AT_decl_file` is 0 for generated functions (the 15 `image-check.sh` failures) | `compiler/nat_ir.fd` (`ng_gen_function`: `g_nat_line_file_ix = 0` for a function without a source file), `compiler/nat_enc.fd` | Status: done (the fix is another session's, e52b278d; confirmed Nov 27 08:26 with release/fid-night.exe: scripts/image-check.sh 226 of 226) |
| 27 | BG201, BG202, BG240, BG242, BG235 | `compiler/typechecker.fd` | Status: BG201, BG202, BG240 and BG242 done (Nov 27 night; BG201 with the build of 07:41; marked in bugs-roadmap.md). BG235 not started |
| 28 | BG203, BG228, BG230, BG241 | `compiler/lower.fd` |
| 29 | BG209, BG210, BG211, BG257 | `compiler/lexer.fd`, `compiler/parser.fd` |
|    | (BG257: a literal of a lowercase-named struct of another module; parser.fd `pd_field_init_follows`, compiler.fd:3606) | | Status: BG257 done (Nov 27 09:21, e1c82060: the capital is no longer asked for; 3,724 verdicts unchanged); BG209 to BG211 not started |
| 30 | BG205, BG231, BG244 | `compiler/codegen_c.fd` | Status: BG231 done (Nov 27 night, the build of 07:41, on ccompile); BG205 and BG244 not started |
| 31 | BG236 | `compiler/mono.fd` | done (Nov 27 night: tests/nattools/mangle_func_unit.fd passes on run, ccompile and emit with the build of 07:41; test_mono.fd no longer compiles: BG253) |
| 32 | BG234, BG245 | `compiler/nat_enc.fd` |
| 33 | BG218, BG219 | `compiler/compiler.fd` | done (Nov 27 night, the build of 06:35: test_bg218_duplicate_impl_fail and test_bg219_generic_fn_value_fail rejected, test_bg219_generic_fn_call passes on run, ccompile and emit) |
| 34 | BG225, BG229 | `compiler/interp.fd` |
| 35 | BG237 | `compiler/main.fd` or `compiler/fmt.fd` | done (Nov 27 night, the build of 06:35: tests/tooling/fmt-flag-order-check.sh 6 of 6) |
| 36 | BG246 | `compiler/pe_ir*.fd` (check whether it is clean) |
| 37 | MI9, MI5, MI14, MI17, RA1, TG5, TG11, TG16, WP6, WP14, LF11, LF13, LF14, LF16, LF20, SP16, OP15, CS3, VF3, VF5, TL5 | `nat_ir.fd`, `iropt.fd`, `wasm.fd` and the like; each large, take only after B to D are closed |
| 38 | BG248 (found by the comparison) | `compiler/nat_ir.fd` | todo: `--nat` Windows: a call through a function value made from a C function's address returns 0 (test_fix_fnval_c_ptr prints 7, 12, 5 through `emit`) |
| 39 | BG249 | `compiler/nat_ir.fd`, `stdlib/linux_runtime/win.fd` | todo: `--nat` Windows runs out of memory on bench_sleepers |
| 40 | BG250 | `compiler/pe_ir*.fd`, `stdlib/pe_runtime/` | todo: pe_ir hangs on test_read_line_long (a child with redirected stdin) |
| 41 | BG251 | `compiler/nat_ir.fd`, `stdlib/linux_runtime/win.fd` | todo: the machine layer on Windows lacks `p_str_join_n`, `p_inb`/`p_outb`, two C imports and the bare-metal builtins |

### G. Not on this machine — mark deferred with the reason

- **Needs macOS, QEMU, Linux or green CI:** MI16, RL11, RL12, RL14, RL15,
  RL22, PL2, BM5, BM6, BM8, CB4, CB5, CI4, SL4, SL18, AD3, VF4, VF7, VF8,
  VF14, VF17, VF12 (its two-machine half).
- **Multi-week:** CS1, CS4, CS5, SP3, SP9, SP12, LG4, LG16, SL2, SL6, VF9,
  VF11, RA1, TG5, MI9.
- **A design decision first:** LF21.

## Run log

One line per goal: the time of its commit, what was done. (The first
version of this log carried times I estimated, ahead of the clock; these
are the commit times.)

- Nov 27 02:15, BG215 (`06c69853`): `stdlib/encoding.fd`,
  `stdlib/unicode.fd`; `test_bg215_utf8_strict` passes on run, ccompile
  and emit.
- 02:20, BG217 (`7efca793`): `stdlib/decimal.fd` (`dec_to_str`,
  `dec_from_units`); `test_bg217_decimal_negative_scale` passes.
- 02:21, BG238 (`b3a6fd43`): `stdlib/money.fd` (`Money_print`);
  `test_bg238_money_negative` passes.
- 02:33, BG239 (`3e07c20f`): `stdlib/tls_crypto.fd` (`hkdf_expand`);
  `test_bg239_hkdf_expand_limit_fail` passes (the 8160-byte limit itself
  checked once through ccompile).
- 02:43, BG223 (`cc9b7d95`): `stdlib/url.fd`, `stdlib/http_client.fd`
  (`Url.path` is the path as written; `url_path_escape`, `url_authority`,
  `url_ref_path`); `test_bg223_url_path_and_authority` passes. Another
  session's commit be363179 ("0.8.5 unstable") took the source and the
  test from the tree before the test's last expected line was corrected;
  cc9b7d95 corrects it and records the goal.
- 02:57, BG216 (`7463e2f3`): `stdlib/tls_crypto.fd` (`x25519` calls
  x25519.fd's `x25519_scalarmult`); `test_bg216_tls_crypto_x25519` passes
  (about 100 s on run).
- 03:47, BG224 (`42a7d968`): `stdlib/xml.fd`, `stdlib/toml.fd` (`XML_MAX_DEPTH`,
  `TOML_MAX_DEPTH`, both 128); `test_bg224_xml_toml_depth_limit` passes
  (59 s on run); the entry's large inputs checked once through ccompile
  and emit.
- 04:05, BG180: `stdlib/core.fd` (`fstr_fmt_f64`, `fstr_fmt_i64`, `__fstr_pad`,
  `__fstr_body`); `test_bg180_fstring_precision_fill` passes, and the 59
  other f-string programs with it. Found on the way and filed: BG247.
  Next: BG247, then queue C.
- 04:25, BG247 (found by BG180): `stdlib/pe_runtime/base.fd`
  (`pe_big_integer_text`); `test_bg247_float_to_str_large` passes on run,
  ccompile and emit. Queue B is closed: next the whole-suite
  comparison's result (queue A), then queue C.
- 05:10: the whole-suite comparison is recorded in MI5 and MI17 (queue
  A): 1,331 programs, 1,287 the same on both backends after every other
  one was rebuilt on its own; pe_ir wrong on 6, `--nat` wrong on 6, only
  pe_ir builds 5, only `--nat` builds 4, neither 21, 2 unfinished tests
  of another session. Filed: BG248, BG249, BG250, BG251. The debug-info
  file index (queue F, first row) was fixed by another session in
  e52b278d: to confirm with the next compiler, not to change. BG213
  needs a typed macro evaluator (unsigned 32-bit arithmetic) and is
  moved to a later batch. Next: compiler batch 1 (BG233, BG208, BG212,
  LG3), one build.
- 04:54: the first compiler batch is in the tree (7b06ce65: BG233, BG208,
  BG212, LG3, with tests/tooling/disasm-forms-check.sh and a rule 4 in
  reference 8.4); the stage-0 compiler checks the whole tree (18 s). The
  build to release/fid-night.exe started at 04:56. Validate with it: the
  four goals' tests on run, ccompile and emit, the f-string and lifetime
  programs, `check compiler/main.fd`, then scripts/image-check.sh (row 26).
- 05:20 (the 05:00 automation run, in the session that wrote the workflow;
  it took over its own lock of 04:53): the second compiler batch is in the
  tree and the stage-0 compiler checks it after every change: b03479b8
  (BG201, BG202, BG240, BG237), 0fcd0261 (BG236, a regression test for
  BG242, which no longer reproduces), 9ec4af52 (BG218), f0973224 (BG219).
  Build 1 is finishing (two units retried one at a time); build 2 waits for
  the WebAssembly suite to end. Also in the tree: cb8f8b69 (BG231, the C
  runtime). The lock stays with this session while build 1 is in flight;
  it validates batch 1 when the build lands, then builds batch 2.
  Started and left for a later batch (not small): BG213 (a typed macro
  evaluator), BG235, BG246, BG205, BG244.
- 06:14, the end of this run (74 minutes; three over the 80 at most if the
  next lines are read first). Done: build 1 (release/fid-night.exe, from
  7b06ce65: one of its sixteen units at -O0, the rest at -O2) and its
  validation (build/verify-nov27/validate-batch1.sh): BG233, BG208 and
  BG212 are marked done (4445b05d); the LG3 rule rejected the compiler's
  own tree at three calls of the form `f(&mut g.e, read(g))` and is
  refined in 21e16421, to validate with the next build. The second batch
  is in the tree and type-checks after every change (the stage-0 compiler):
  b03479b8, 0fcd0261, 9ec4af52, f0973224, cb8f8b69, 21e16421, 22851471.
  NEXT, IN THIS ORDER: (1) until build/verify-nov27/mi14_bench.log holds
  BENCH_DONE, start nothing heavy: the WebAssembly suite (ALL=1, 1,315
  programs, in build/wasm-native) and then the speed comparison run in the
  session that wrote this, and the comparison needs a quiet machine; when
  they end, record them in PL4 and MI14 (the table under "First: record
  the background runs"); (2) build the tree to release/fid-night.exe (the
  build command above; a second build is allowed in that run); (3)
  `NIGHT=release/fid-night.exe bash build/verify-nov27/validate-batch2.sh`
  and the four LG3 tests, `check compiler/main.fd`,
  `bash scripts/check-stdlib.sh release/fid-night.exe` (the read rule
  against every module); (4) scripts/image-check.sh (row 26) and
  scripts/disasm-check.sh with the new compiler; (5) mark what passed.
  Build 1's check of the 324 related programs: 321 pass; three fail with
  the compiler of Nov 25 as well (test_frontend_comptime_if_assign,
  s4_11_comptime_if_generic, s8_2_null_deref_panics): not this batch's.
- 07:30, the end of the 06:13 run (77 minutes), two builds. Build 2
  (06:19-06:35, tree 2e3a15e9, 16 units at -O2) validated the formatter
  (BG237, 6 of 6) and the rejections of BG201's expression form, BG202,
  BG240, BG218, BG219 and BG242; `check compiler/main.fd` still failed at
  nat_ir.fd ng_mir_finish (LG3). The cause, found by reducing the site: a
  released borrow stays in the checker's table without a borrower, and the
  helper of 21e16421 counted it as a named one (4d7dcd55, with
  test_lg3_read_after_released_mut). BG201's compound form was written
  (f148a589) and was WRONG: build 3 (06:55-07:10, 16 units at -O2)
  rejects every plain `x = ...` on a float (stdlib/core.fd:8662 and 30
  places in the compiler's own source), because `assign_op_binop` gives
  BitAnd for `=` as for `&=`. Corrected in 647b4368 (the five compound
  operators only); stage-0 check clean; no build was left in the run.
  Also 5c86e3dd (two test fixes) and ac4ae268 (the mangle check as a unit
  test). Marked done on build 2's evidence: BG202, BG218, BG219, BG236,
  BG237, BG240, BG242 (bugs-roadmap.md; BG252 and BG253 filed there).
  release/fid-night.exe was build 3, which rejects float assignments: it
  is renamed release/fid-night-0710-bad.exe (to delete with the user's OK),
  and there is no release/fid-night.exe until the next build.
  NEXT, IN THIS ORDER: (1) build again (the command above) and copy the
  result to release/fid-night.exe; (2) `STEPS="0 2 6" bash
  build/verify-nov27/validate-batch3.sh`: 11 rejections (among them
  test_bg201_float_shift_assign_fail) and `check compiler/main.fd` clean,
  which is LG3's last open point; (3) side by side, `STEPS="1 3 4"`,
  `STEPS=5`, `STEPS=7`, `STEPS=8` (step 8 is LG3's gate); (4) mark the LG3
  read rule, BG201 and BG231 from those logs; (5) the WebAssembly list, 154
  programs in build/verify-nov27/wasm-rerun-files.txt (`FID=release/fid-v27.exe
  JOBS=2 OUT=build/wasm-rerun bash scripts/wasm-test.sh $(cat
  build/verify-nov27/wasm-rerun-files.txt)`), classify, then PL4; (6)
  scripts/image-check.sh (row 26) and scripts/disasm-check.sh.
  Lesson of the run: the stage-0 check proves that the tree type-checks,
  not that new code is right; a compiler change committed before its build
  must say so in its commit, and the next build comes first.
- 08:30, the end of the 07:25 run (65 minutes), one build (07:26 to 07:41,
  16 units at -O2, release/fid-night.exe of 16,785,408 bytes). Validated
  with it and marked (45537169): BG201 (the compound form is rejected and
  the plain float assignments are accepted again: `check compiler/main.fd`
  is clean, 26 s), BG231 (ccompile), BG236 (its unit test on run,
  ccompile and emit), and LG3's read rule (11 rejections; its gate 154 of
  155; the entry stays `[~]` for its other gaps). PL4 has its note of the
  whole WebAssembly run (14f37bfd, 7d381a77): 1,116 of 1,315 pass, beside
  a build; the 154 to run again are listed. The standard-library check
  (scripts/check-stdlib.sh) then named three defects, all older than
  tonight's compiler changes: BG254 (transform3d, lidar, pointcloud: fixed
  in the library, 1ae02075; the parser's part is BG257), BG255 (the QUIC
  key update: fixed, dbb3347c and aea3663b; the 5 QUIC and HTTP/3 tests
  pass on ccompile and emit, and none compiled before), BG256 (parallel:
  NOT fixed, a function value cannot reach a spawned task; it needs a
  decision about the language). scripts/check-stdlib.sh with
  release/fid-v27.exe: 400 of 401 modules build on their own (390 before);
  parallel is the one left.
  NEXT, IN THIS ORDER: (1) BG232 (row 12: the probe through ccompile) and
  BG257 (row 29: parser.fd `pd_field_init_follows`, or the struct names of
  a module's imports in compiler.fd): one build for both; (2) row 26
  (scripts/image-check.sh with release/fid-night.exe), then rows 16 to 25
  of section D; (3) the WebAssembly list of PL4 when the machine is quiet
  (154 programs; an hour at least); (4) BG256 only once function values
  across tasks are decided.
  Traps of this run: `( ... ) &` after `cd .. && VAR=.. &&` runs only its
  first group with them (three steps wrote to /run-N.log and were run
  again); scripts/check-stdlib.sh says its count on its last line (a
  summary made of the last matching lines showed 2 of 11 failures);
  `x as fn(..)` is the C function pointer of `fid ccompile`, not a Fides
  function value; a unit test that imports compiler::disasm and
  compiler::nat_enc takes 537 s through `fid run` and 38 s through
  ccompile.
- 08:27, rows 24 and 26: `FID=release/fid-night.exe bash
  scripts/image-check.sh` on every format: 226 checks passed, 0 failed
  (build/verify-nov27/image-check-night.log); otool is the one reader
  not installed. RL9 keeps `[~]` for loading on the targets.
- 08:29: `FID=release/fid-night.exe bash scripts/disasm-check.sh` (x86-64,
  AArch64 and RISC-V 64 against llvm-objdump, with the round trip): 18 ok,
  0 failed in 33 s (build/verify-nov27/disasm-check-night.log); AArch64
  leaves 8 words undecoded in test_machine_fast where LLVM decodes 2,957:
  the rows of BG232.
- 09:45, the end of the 08:30 run (75 minutes), two builds. Build 5
  (08:33 to 08:47, 16 units at -O2) carried BG257: the parser opens a
  struct literal after a name of any case (e1c82060); validated with
  test_bg257 on run, ccompile and emit, a clean `check compiler/main.fd`
  and the front end's verdict on all 3,724 files of tests/bootstrap and
  tests/conformance, the build of 07:41's file for file (2,724 accepted,
  1,000 rejected, 0 differ: build/verify-nov27/verdicts.log; the script
  is build/verify-nov27/frontend-verdicts.sh and -diff.sh: reuse it for
  every parser or checker change). Build 6 (09:03 to 09:17) carried
  BG232 (48a42687): rows for the encoders that had none on the three
  architectures, their bytes llvm-mc's; tests/tooling/disasm-forms-
  check.sh 30 of 30 (the build of 08:47 fails 21), tests/nattools/
  enc_table_unit.fd ok on ccompile and emit, scripts/disasm-check.sh
  18 ok with 0 words undecoded on AArch64 in test_machine_fast (8
  before). scripts/check-stdlib.sh with the build of 09:17: 400 compiled, 1 failed.
  PL4 has the evening's three counts. release/fid-night.exe is build 6;
  release/fid-night-prev.exe is build 5.
  The fast loop that worked for the disassembler: compile the unit test
  through `fid ccompile` with the previous build (40 s); it runs the
  tree's source, so rows are tried without a build. enc_table_unit's
  own table check caught three slips of mine in the rows (example words
  with operand bits as row values, the funct3 bits one digit off, a
  lane bit fixed).
  NEXT, IN THIS ORDER: (1) section D, rows 16 to 25 (verify only: each
  entry's last "Left" line, run what fits one run); (2) the WebAssembly
  list of PL4 when the machine is quiet (154 programs, an hour at
  least); (3) BG256 only on a decision about function values across
  tasks; (4) rows 27 to 36, one build each: BG235, BG203, BG228, BG230,
  BG241 (lower.fd), BG209 to BG211 (lexer.fd, parser.fd), BG205, BG244
  (codegen_c.fd), BG234, BG245 (nat_enc.fd).
- 09:42, row 17 (VF15): `python scripts/fuzz/fuzz_parsers.py --fid
  release/fid-v27.exe --parsers tls -n 6000 --seed 2811`: 0 distinct
  findings in 7 minutes (600 with seed 27 before: 0); the other eight
  parsers with `-n 1000` (125 each): 0. About 850 inputs a minute
  through C. The entry stays `[~]` for its long runs.
- 10:55, the end of the 09:39 run (76 minutes), no build. BG206 (a88c72b3,
  marked 19362638): the Linux runtime's pert_str_to_float is exact: a
  big-integer numerator and denominator and a division bit by bit;
  tests/nattools/str_to_float_exact_check.sh: 2,000 doubles printed and
  read back, 0 wrong (1,989 before), 1,107 texts as Python's float()
  (751 differed before); the 15 float, f-string, decimal and JSON tests
  pass on the native backend. BG253 (4067c378): tests/selfhost/
  test_mono.fd passes on run, ccompile and emit. BG249: the cause is
  located, the fix designed (row 46). BG207 needs a console (row 47,
  deferred). VF10 (row 16): 1,607 of 1,617; CF32 to CF37 filed (row 48).
  Trap of the run: a bignum remainder one limb past the denominator's
  length, compared and subtracted over the denominator's limbs only,
  made the division run for good; and I typed the repository's path
  wrong five times in `cd` lines: an absolute path that fails leaves
  the shell where it was, and the rest of the line runs there.
  NEXT, IN THIS ORDER: (1) row 48 with row 27 to 36 (compiler bugs, one
  build each; CF32 first: the IR checks keep no @public);
  (2) row 46 (BG249, the runtime only); (3) section D rows 18 to 25;
  (4) the WebAssembly list of PL4 when the machine is quiet.
- 11:40, the end of the 10:51 run (49 minutes: stopped to answer the user),
  one build (11:16 to 11:30, 16 units at -O2, release/fid-night.exe; build
  6 is release/fid-night-prev.exe). BG249 (row 46): the fix was written and
  tried for 25 minutes, not validated, nothing kept: what the system does
  with a stack committed as used is in the entry, the experiment in
  build/tmp/bg249/. CF32 (row 48): the type checker puts the parameters
  @public marks down for the check on the IR (c437509b): the native
  backend accepts them now, the interpreter always did, and 38
  constant-time tests pass on run, ccompile and emit; the C generator's
  own copy of the check still marks every parameter. Another session is
  in compiler/typechecker.fd, lower.fd, lexer.fd, interp.fd, escape.fd
  and nat_ir.fd, with tests for BG203, BG225 and BG252 uncommitted in the
  tree: rows 28 and 34 and the rest of 48 wait for it. My one hunk in
  typechecker.fd was committed alone (git apply --cached of that hunk).
  NEXT, IN THIS ORDER: (1) CF32's C half: codegen_c.fd `cg_ct_check`
  seeds its parameters as checks.fd `peir_ct_check` did before c437509b:
  the same one line, a build, then build/verify-nov27/cf32/
  test_cf32_public_param.fd on run, ccompile and emit and the front-end
  verdicts of build 6 against the new build; (2) rows 27, 29 to 31 and
  32 as the other session leaves them; (3) section D rows 18 to 25;
  (4) the WebAssembly list of PL4 when the machine is quiet.
- 12:00 to 16:45, the interactive session on the 19 LLVM goals the user
  named (help finish, verify, mark). Done and committed, each with its
  gate green on a built compiler: SP6 (1a6f802f), LF20 (5c5d0644: the
  3-of-8 was the statistic, ng_secret_mir), SP7 (ad4dfa83: a kept slot
  for a function a build inlines away). Verified, their gates green, left
  partly for what the entry names: MI14 (33 ok), SP16 (9 ok), TG11, WP6
  (23 ok: AArch64's pool has none of x0-x7). Gate defects fixed
  (f10eb6a1): wsl.exe without a distribution, MI14's `around`, fregs'
  stubs, the relink path, SP16's probe and its checker. CF32 and CF37
  closed (cc293424). Two builds failed on the machine's commit limit
  (44 of 48 GB taken) and one bug of mine in the build script's success
  path (56492abb); the -O2 builds of 15:52 and 16:19 are installed as
  release/fid-night.exe (fid-night-prev.exe is the one of 15:52). Still running when this was written (17:15): two suite jobs, the default
  leg and the chain off, sole, vec; the gates RA1, LF11, SP9 wait for
  them (above). Left for another session or another day: MI5,
  MI17 (BG248, BG249, BG251), MI9, TG11's reductions, TG16, WP6's
  AArch64 pool, LF13, LF21, SP16's 2g.114, MI14's default switch.
- ~18:02, the doc-accuracy session recorded the four suite legs of 16:38
  as finished, with `on` 604 ok / 10 fail, `off` 1122 ok / 18 fail,
  `sole` 816 ok / 137 fail, `vec` 815 ok / 133 fail. Corrected at 19:05
  by the session that ran them: those four had been stopped at 17:00
  (five jobs side by side made the machine crawl) and their logs were
  partial, `sole` and `vec` leftovers of the stop. The legs restarted at
  17:04 ended between 17:43 and 18:12 with the numbers in the next
  entry; RA1's gate, still running then, is in that entry too.
- 18:00 to 18:55, the end of the afternoon's session. The four suite
  legs: 2,011 of 2,056 on emit and `--nat`; the same 38 fail in every
  leg (the suite's own on these backends), 7 more on `--nat` only
  (MI14's entry); no verdict changes with specialization and versioning
  off, with soleown on, or with vecsroa on. Marked done: WP14, LF16,
  LF14 (748df914), LF11 (be77cdf3: version-default-check 0 failed once
  it stops comparing the code addresses a panic trace prints). SP9's
  four checks pass and the bench is in its entry (FID_PAR stays
  opt-in: on by default is read in six modules, three of them another
  session's today). RA1's gate: 132 same, 0 differ (one task program
  raced under load; the gate gives such a one a second run on its
  own). 7 of the 19 named goals are done (SP6, LF20, SP7, WP14, LF16,
  LF14, LF11); 167 of 196 in the roadmap. An AppData check the user
  asked for: the compiler's per-build caches in %TEMP% (1.2 GB), the
  runner's expect* folders (3.8 GB), another agent's scratchpad (8.9
  GB); no crash dumps; nothing deleted.
- 19:10 to 20:25, a scheduled run on the LLVM queue (the lock was
  free). Under hard rule 2 every goal whose file another session
  modifies was left alone (nat_ir.fd, iropt.fd, lower.fd, pe_ir.fd,
  lexer.fd, parser.fd, escape.fd, wasm.fd, win.fd are modified by
  it; BG248, BG249 and BG251 are still partly). Done with gates only:
  SP16 — `fid emit` builds the compiler (5 minutes) and it is the
  same compiler: scripts/native-fixpoint.sh on pe_ir reaches its
  fixpoint, stage 2 == stage 3 byte for byte, the first run of it to
  finish (its last check compared the interpreter's stderr: fixed).
  TG11: vectorize_spill_check.sh follows the loops the inliner takes
  into their caller: 32 ok, and its 6 failures are the two
  reductions. WP6: the native suite ran. known-bugs 2g.114: the crash
  is gone, six unresolved labels stay. RL14 (new row L15): the pe_ir
  host. 8 of the 19 named goals are done. Left running: the chain
  (MI14's two float-switch suites on `--nat`, then the `--nat`
  fixpoint).
- 20:25 to 22:45, on the user's request (not a goal run): why every
  GitHub Actions workflow is red. The build step killed the 16 GB
  Linux and macOS runners; fixed in scripts/build-units.sh (one heavy
  unit at a time, a limit per compile on Linux) and in the job
  timeouts, to be seen by the first push. After the build, CI's
  selfhost job has three red steps left: fmt (9 to 17 files that
  another session modifies are still unformatted: five stdlib and
  four to twelve compiler files, measured on their committed copies;
  the formatter's `as *mut` defect, the one thing wrong in 15 other
  files, is fixed, with tests/fmt/cast), the
  lint gate
  (scripts/lint-check.baseline is from Nov 19 and does not know 62
  modules with warnings; vec, math, crypto_bind_unix and parallel do
  not pass `fid check` as files) and the doc examples of hash,
  parallel and uuid (fixed in that session's working tree). BG490 is
  new. No goal of section A was touched.
- 23:09 to 23:58, a scheduled run (the lock was free). MI14: the chain's
  two legs are in the entry; each of their 60 files was built again on
  its own, and none fails with a float switch off that the default
  `--nat` run passes (the 12 that fail are another session's uncommitted
  expected-fail tests). RL14 on `--nat`: stage 2 is the first compiler
  built by `emit --nat` (236 s); stage 3 died once for lack of commit
  (the function cache writer's 1 GiB vector, under 5 GB free) and built
  at the second run (549 s); stage 3 == stage 4 byte for byte, stage 2
  != stage 3 (two compilers: stage 1 is the C build of 16:19); in the
  entry. No other goal could be touched: at 23:20 every file behind
  L7 to L10 and L13 (nat_ir.fd, iropt*.fd, pe_ir*.fd, compiler.fd,
  lexer.fd, parser.fd, borrowck.fd, escape.fd, secrecy.fd, win.fd) was
  modified by another session, and MI5 waits on BG248, BG249, BG251.
  No compiler build: the three stages were the run's heavy job. Not
  filed (the bugs roadmap is that session's file just now): a `--nat`
  self-compile needs 8 GB of commit free for the cache writer
  (compiler/ast_ser.fd AwBuf keeps a byte in a word; nat_ir.fd
  nc_finish).
