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
| 16 | VF10 | the conformance suite on every backend: `CC="zig cc" python scripts/verify/expect.py --fid release/fid-v27.exe --backends run,ccompile,emit tests/conformance`, then `python scripts/verify/conf-known.py --log <that output>` | todo |
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
