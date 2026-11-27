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
| `MI5_DONE` in `mi5.log` | last line of `mi5.log`; anything not SAME in `mi5/results.txt` | `MI5` and `MI17` in `llvm-roadmap.md` |
| `WASM_DONE` in `wasm-all.log` | the `fid wasm:` summary line | `PL4` in `competitive-roadmap.md` |
| `BENCH_DONE` in `mi14_bench.log` | the `pe_ir ... nat ... nat/pe_ir` lines | `MI14` in `llvm-roadmap.md` |

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
| 8 | BG180 | f-string float precision wrong for large values; fill ignored | `stdlib/core.fd` (`fstr_fmt_f64`, `__fstr_pad`, `__fstr_width0`) | the entry's triggers; check first whether it still reproduces | todo |
| 9 | BG206 | Linux runtime `pert_str_to_float` is not correctly rounded | `stdlib/linux_runtime/base.fd` | 2000 printed doubles read back; runs here through `emit --nat` | todo (a correct-rounding conversion: the last of this group) |

### C. Bugs in compiler files no other session edits — one build per batch

| # | Goal | What is wrong | Files | Gate | Status |
|---|---|---|---|---|---|
| 10 | BG233 | disasm: one-byte forms ignore the `0x66` prefix and REX.B | `compiler/disasm.fd` | `66 B8 34 12` is `mov ax, 0x1234`, `66 99` `cwd`, `49 90` `xchg r8, rax`, `F3 90` `pause`, each with the right length | todo |
| 11 | BG208 | disasm: `vzeroupper` swallows the next instruction | `compiler/disasm.fd` | `c5 f8 77 4d 39` is `vzeroupper` then `cmp r12, r14` | todo |
| 12 | BG232 | disasm has no row for instructions the encoders emit | `compiler/disasm.fd` | the entry's list (x86 `blsmsk`/`blsi`, AArch64 pre/post-index and FP loads, RISC-V vector) decodes with the right length | todo |
| 13 | BG212 | comptime does not evaluate float/int casts | `compiler/comptime_eval.fd` (`Expr::Cast`) | `comptime avg(3, 4)` and `comptime (-(7.9 as i64))` evaluate | todo |
| 14 | BG213 | C header parser evaluates unsigned macro arithmetic as signed | `compiler/cparser.fd` (`CpEval`/`CpExpr`) | `fid parse-header` on a header with `UDIV`, `UNEG`, `MASK`, `COND`, `CMP`, `0b1010` | todo |
| 15 | LG3 | reads are not checked against a live `&mut` | `compiler/borrowck.fd` | `val v = m` while `r = id_mut(&mut m)` is live and used after is a borrow error; `test_frontend_lifetime*.fd` and `tests/conformance/s8_*` still pass | todo |

### D. Verify only — the gate runs on this machine

Read the entry first; its last line says what is left. Run the gate when the
machine is free of the two heavy runs; record the numbers.

| # | Goal | Gate | Status |
|---|---|---|---|
| 16 | VF10 | the conformance suite on every backend: `CC="zig cc" python scripts/verify/expect.py --fid release/fid-v27.exe --backends run,ccompile,emit tests/conformance`, then `python scripts/verify/conf-known.py --log <that output>` | todo |
| 17 | VF15 | `scripts/fuzz/fuzz_parsers.py` (read its header) | todo |
| 18 | AD4 | `tools/doctest.fd` against `scripts/doctest.py` (read the entry) | todo |
| 19 | TL10 | `tree-sitter generate` for `editors/tree-sitter-fides`, Zed (read the entry) | todo |
| 20 | SP13 | the debug mode's timing (read the entry) | todo |
| 21 | SL17 | files, paths and processes on `run`, `ccompile`, `emit`, `--nat` (read the entry) | todo |
| 22 | RA9 | the greedy allocator on the suite (read the entry) | todo |
| 23 | WP7 | `SUITE=1 bash tests/tooling/wp-defaults-check.sh` (heavy) | todo |
| 24 | RL9 | `FID=<bin> bash scripts/image-check.sh`: 211 pass, 15 fail on the debug-info defect below until it is fixed | waiting (row 30) |
| 25 | TL6, RL25, RL1, RL3, VF2, VF1, VF18, VF6 | a recorded run each; read the entry, run what fits the budget | todo |

### E. In flight in another session — do not start

BG159, BG165, BG166, BG182, BG183, BG185, BG204, BG227, CS6, SP6, SP7, SP8,
CB3: their tests are in the working tree. Take them only when their row in
the roadmap is `[x]` and the test is committed; then run the gate and record.

### F. Waiting: the files are staged by another session

Take in this order as soon as `git status --porcelain -- <file>` is empty.

| # | Goal | Files |
|---|---|---|
| 26 | DWARF: `DW_AT_decl_file` is 0 for generated functions (the 15 `image-check.sh` failures) | `compiler/nat_ir.fd` (`ng_gen_function`: `g_nat_line_file_ix = 0` for a function without a source file), `compiler/nat_enc.fd` |
| 27 | BG201, BG202, BG240, BG242, BG235 | `compiler/typechecker.fd` |
| 28 | BG203, BG228, BG230, BG241 | `compiler/lower.fd` |
| 29 | BG209, BG210, BG211 | `compiler/lexer.fd`, `compiler/parser.fd` |
| 30 | BG205, BG231, BG244 | `compiler/codegen_c.fd` |
| 31 | BG236 | `compiler/mono.fd` |
| 32 | BG234, BG245 | `compiler/nat_enc.fd` |
| 33 | BG218, BG219 | `compiler/compiler.fd` |
| 34 | BG225, BG229 | `compiler/interp.fd` |
| 35 | BG237 | `compiler/main.fd` or `compiler/fmt.fd` |
| 36 | BG246 | `compiler/pe_ir*.fd` (check whether it is clean) |
| 37 | MI9, MI5, MI14, MI17, RA1, TG5, TG11, TG16, WP6, WP14, LF11, LF13, LF14, LF16, LF20, SP16, OP15, CS3, VF3, VF5, TL5 | `nat_ir.fd`, `iropt.fd`, `wasm.fd` and the like; each large, take only after B to D are closed |

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
- 03:55, BG224: `stdlib/xml.fd`, `stdlib/toml.fd` (`XML_MAX_DEPTH`,
  `TOML_MAX_DEPTH`, both 128); `test_bg224_xml_toml_depth_limit` passes
  (59 s on run); the entry's large inputs checked once through ccompile
  and emit. Next: BG180, then queue C.
