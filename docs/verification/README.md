# Verification tools (roadmap track VF)

What each tool checks, how to run it, and its last recorded run. Bugs found
are in [miscompile-log.md](miscompile-log.md), each with a repro in
`tests/verify/bugs/`. The roadmap lines are in
[competitive-roadmap.md § Track VF](../roadmap/competitive-roadmap.md#track-vf--verification-in-place-of-years).

All tools take `--fid <compiler>` (default `release/fid.exe`) and use
`CC="zig cc"` for the C path. Python tools need Python 3.10+; `soak.py` and
`interleave.py` need `psutil`. On Windows the Linux and GCC parts run in WSL.

| VF | Tool | Run |
|----|------|-----|
| VF1 | `scripts/fuzz/gen_programs.py`: seeded random programs on `run`, `ccompile`, `emit` and the checked allocator, which must agree; `--rich` adds a struct, an enum with payloads, strings and f64 arithmetic (a seed gives the same program as before without it). `scripts/fuzz/nightly.sh` runs both kinds and, on Linux, a sample under ASan/UBSan; `--batch`, `--retries`, and `triage.py`'s machine-side groups keep a long run on a busy machine honest. Runs of record: [fuzz-runs.md](fuzz-runs.md) | `N=300 FID=release/fid scripts/fuzz/nightly.sh` |
| VF2 | `scripts/fuzz/gen_vs_c.py`: random programs, each with an equivalent C program built by Clang (`zig cc` -O0/-O2) and GCC (WSL on Windows, the host's elsewhere, -O0/-O2); all C builds must agree, then `run`, `ccompile`, `emit` must match them. `--avoid` steps around known bugs; `scripts/fuzz/triage.py` groups failures; `scripts/fuzz/reduce.py` shrinks one; `--ext2` adds fixed-precision floats, more of `str` and a `_` arm, `--jobs N` checks N seeds at once, `--log FILE` appends a line per seed and a summary; `scripts/fuzz/vf2_long_run.sh` runs 1000+ programs per backend ([vf2-v1-long-run-2026-11-17.md](vf2-v1-long-run-2026-11-17.md)) | `python scripts/fuzz/gen_vs_c.py -n 100 --seed 1` |
| VF3 | `scripts/fuzz/fuzz_frontend.py`: mutants of the suite (token edits, splices, cuts, deep nesting, odd bytes) through `fid check`; a crash, internal panic or hang is a finding; with `--cov-fid` (a compiler built with counters) it keeps inputs that reach new IR blocks; `--mode emit [--target x86_64-linux\|nat]` gives them to the whole native pipeline (the IR optimizer's and the machine layer's passes); `--keep <dir>` carries the grown corpus and the blocks reached across batches. The long coverage-guided run (instrumented compiler, check + emit --nat + emit --target x86_64-linux, in batches) is `scripts/verify/vf3-cov-run.sh`; runs: [vf3-vf4-2026-11-17.md](vf3-vf4-2026-11-17.md) | `python scripts/fuzz/fuzz_frontend.py --fid release/fid.exe -n 2000`; `FID=release/fid.exe bash scripts/verify/vf3-cov-run.sh` |
| VF4 | `scripts/verify/variants.py toggle`: each optimizer pass off in turn (iropt passes at run time through `FID_IROPT_OFF`, `PEIR_*` constants in rebuilt compilers); every test's output must not change. Whole suite: `scripts/verify/vf4-run.sh` (the five `PEIR_*` variants built in 8 units at -O2, then every `FID_IROPT_OFF` switch, pruned on `FID_IROPT_STATS`, resumable); runs: [vf3-vf4-2026-11-17.md](vf3-vf4-2026-11-17.md) | `python scripts/verify/variants.py toggle --tree . --sample 60 --gen 20`; `FID=release/fid.exe J=8 bash scripts/verify/vf4-run.sh` |
| VF5 | `tests/bootstrap/test_opt_*.fd`: one test per pass (fold, inline, dce, gvn, tailcall, licm, ranges, register allocation), checked against the interpreter (no optimizer); `tests/bootstrap/test_verify_peir_{coalesce,volatile,xmm,rotate}.fd`: one per `PEIR_*` switch of pe_ir (`PEIR_GLOBAL_RA` is `test_opt_regalloc.fd`, `PEIR_RANGES` `test_opt_ranges*.fd`) | `python scripts/verify/expect.py tests/bootstrap/test_opt_*.fd` |
| VF6 | `compiler/tv.fd` (Nov 16): `--validate` or `FID_VALIDATE=1` checks each iropt pass that changes a function, and the SSA round trip, and undoes a rejected result; design and state: [translation-validation.md](translation-validation.md). Nov 21 check: `tests/tooling/tv-check.sh` ends 3 failed (6 of 12 injected faults not caught) | `FID=release/fid.exe bash tests/tooling/tv-check.sh` |
| VF7 | `scripts/verify/variants.py mutate`: compilers with one deliberate bug in a pass; some test must notice. Exit status 1 when a mutant survives, 2 when one does not build | `python scripts/verify/variants.py mutate --tree . --tests 'tests/bootstrap/test_opt_*.fd'` |
| VF8 | `scripts/verify/cov_instrument.py` (counters at every function, IR block and source line of the compiler's C; the lines come from `FID_C_LINE_MARKS=1` marks or `-g`'s `#line`) + `scripts/verify/coverage.py` (runs the suite, maps blocks and lines back to `compiler/*.fd`; `--goal`, `--goal-lines`, `--record`). The whole run (build the counter compiler, the suite in every mode, the 90% goal, the low modules) is `scripts/verify/vf8-run.sh`; check: `tests/tooling/vf8-coverage-check.sh`; runs: [vf8-coverage.md](vf8-coverage.md) | `FID=release/fid.exe bash scripts/verify/vf8-run.sh` |
| VF9 | [semantics.md](semantics.md): big-step semantics of the integer core; [semantics-2.md](semantics-2.md): floats, `f32`, strings, enums, generics, closures, references, effects, memory, `i128`, arrays and tuples, traits and `dyn`, `defer`, regions, `@linear`, handlers, channels and `select`, threads and green tasks, one printed line per rule in `tests/bootstrap/test_verify_semantics_ext.fd`; not mechanised | `python scripts/verify/expect.py tests/bootstrap/test_verify_semantics_ext.fd` |
| VF10 | `tests/conformance/`: 1,617 tests, at least one per sentence of the reference that states a rule, every section §1–§13 (file name = section; modules the tests `use` in `_modules/`, skipped as programs), run by `scripts/verify/expect.py`; `scripts/verify/conformance_coverage.py` counts tests per section and fails on a section without one. Run of record and findings (CF1–CF31): [vf10-conformance-2026-11-17.md](vf10-conformance-2026-11-17.md) | `python scripts/verify/expect.py tests/conformance`; `python scripts/verify/conformance_coverage.py --min 1000` |
| VF11 | [memory-safety-core.md](memory-safety-core.md): calculus, invariant, proof sketch; [memory-safety-model.md](memory-safety-model.md): `scripts/verify/own_model.py`, the bounded model (copy-on-write, boxes in boxes, `&mut` calls, structs, enums, closures, `Rc`, two references), and `scripts/verify/own_diff.py`, its programs run through `fid` (`run`, `--leaks`, `--checked-alloc`) against the model | `python scripts/verify/own_model.py --size 4 --self-test`; `python scripts/verify/own_diff.py --fid release/fid.exe --size 4 --per 60 --rejected 10`; `bash tests/tooling/vf11-own-model-check.sh` |
| VF12 | `scripts/verify/repro.sh`: each program built twice in different directories, temp folders and times; `dump-c`, `emit`, `ccompile`, the Linux images and `emit --nat` compared byte for byte (`COMPILER=1` also builds fid itself twice: `dump-c` and `ccompile` of compiler/main.fd); runs: [vf12-vf13-2026-11-18.md](vf12-vf13-2026-11-18.md) | `FID=release/fid.exe scripts/verify/repro.sh` |
| VF13 | `scripts/verify/clean-bootstrap.sh`: `git archive HEAD` (or `--worktree`) into a new directory, Rust bootstrap from cargo, `scripts/bootstrap.sh` 3 stages, fixpoint; checks first that every imported module is in the tree, keeps `<out>/record.txt` | `scripts/verify/clean-bootstrap.sh` |
| VF14 | `scripts/sanitize.sh`: the suite through C under ASan + UBSan (output must match `fid run`); `SAN=thread` runs the programs that use threads, tasks and channels under ThreadSanitizer; from Windows, `scripts/sanitize-wsl.sh` (gcc in WSL). Whole-suite run of Nov 17: [sanitizers-2026-11-17.md](sanitizers-2026-11-17.md) | `CC=clang scripts/sanitize.sh`, `SAN=thread CC=clang scripts/sanitize.sh` |
| VF15 | `scripts/fuzz/fuzz_parsers.py` + `tests/fuzz/parsers.fd`: json, regex, http, csv, toml, xml, url, uuid, tls (the TLS 1.3 record layer, binary seeds) on mutated input; any panic, crash or hang; the run of record (`scripts/fuzz/vf15_long_run.sh`): [vf15-parsers-2026-11-18.md](vf15-parsers-2026-11-18.md) | `python scripts/fuzz/fuzz_parsers.py -n 5000`; `FID=release/fid.exe bash scripts/fuzz/vf15_long_run.sh` |
| VF16 | `scripts/verify/soak.py` + `tests/verify/soak_service.fd`: a request handler run for `--duration` s; memory slope over the second half must be flat | `python scripts/verify/soak.py --duration 86400` |
| VF17 | `scripts/verify/chaos.py` + `tests/verify/chaos.fd`: dropped/reset/half/silent servers, a reader that goes away, missing files and arguments, stack exhaustion; with `--linux` full disk (`ulimit -f`), out of memory (`ulimit -v`), clock jumps (`--clock`: `scripts/verify/clockjump.c` by LD_PRELOAD, or libfaketime) and network partitions (`--partition`: two namespaces, veth, nftables; `scripts/verify/chaos_linux.py`); open findings print OPEN; runs: [vf17-chaos-2026-11-18.md](vf17-chaos-2026-11-18.md) | `python scripts/verify/chaos.py --linux` (`--strict`: open findings fail too) |
| VF18 | `scripts/verify/interleave.py` + `tests/verify/interleave.fd`: atomics, a mutex, a channel pipeline, parallel branches, green tasks, async fns, `select`; thousands of runs on random CPU sets with and without load must print the program's expected block; `--sched-random` adds the runtimes' seeded scheduler (`FIDES_SCHED_RANDOM`), `--seeds` replays, `--record` logs; runs: [vf18-interleave-2026-11-18.md](vf18-interleave-2026-11-18.md) | `python scripts/verify/interleave.py -n 3000 --shards 4`; `bash tests/tooling/vf18-interleave-check.sh` |
| VF19 | [miscompile-log.md](miscompile-log.md) | — |

## Reliability of the own backend ([llvm-roadmap.md § Track RL](../roadmap/llvm-roadmap.md))

| RL | Tool | Run |
|----|------|-----|
| RL1 | `scripts/fuzz/gen_vs_c.py --backends all`: the VF2 programs on every backend this host runs (`run`, `ccompile`, `ccompile-O2`, `emit`, `nat`, `emit:<linux target>`, `ccompile:<linux target>` under qemu or WSL) against the C oracle; runs: seeds 1-10 in [infra-runs-2026-11-16.md](infra-runs-2026-11-16.md#infra-runs-nov-16-batch-8-tl10-pl7-ad3-rl1-rl2) (M12, M13), seeds 11-60 in [rl1-gen-vs-c-2026-11-18.md](rl1-gen-vs-c-2026-11-18.md) (M7, V1c) | `python scripts/fuzz/gen_vs_c.py -n 100 --backends all` |
| RL2, RL22 | `scripts/suite-all.sh`: the suite on every lane (C path and our backend, every target the host can run); `AGREE=1` fails when lanes disagree on a test; each run ends with `$OUT/report.md` (`scripts/verify/suite-all-report.py`: each failure's cause, tests grouped by the lanes they fail on, Linux binaries importing runtime names); [rl2-rl3-2026-11-18.md](rl2-rl3-2026-11-18.md). RL22's release run: `scripts/verify/rl22-release-run.sh` (AGREE=1 suite-all, then `scripts/verify/rl22-paths.py`: each C-path lane against its target's own-backend lanes, disagreements fixed or recorded in `scripts/verify/rl22-recorded.txt`); [rl15-rl22-2026-11-18.md](rl15-rl22-2026-11-18.md) | `AGREE=1 bash scripts/suite-all.sh`; `FID=$FID bash scripts/verify/rl22-release-run.sh`; `bash tests/tooling/rl2-suite-report-check.sh`; `bash tests/tooling/rl22-paths-check.sh` |
| RL3 | `scripts/pass-matrix.sh <target>`: every switch of [kill-switches.md](kill-switches.md) off in turn, the opt-in mid-end on and each of its passes off, each e-graph family (`FID_EGRAPH_OFF`) and each rule of rules.fd (`FID_RULES_OFF`) off; results compared test by test; `RESUME=1` continues an interrupted matrix; every native target and one report: `scripts/verify/rl3-matrix-all.sh` (`scripts/verify/pass-matrix-report.py`); [rl2-rl3-2026-11-18.md](rl2-rl3-2026-11-18.md) | `NATIVE=1 bash scripts/pass-matrix.sh x86_64-linux`; `bash tests/tooling/rl3-rules-off-check.sh` |
| RL4 | `scripts/miscompile-gate.sh`: every fixed entry of the log names a test file; `--run` it passes, `--before <old fid>` it fails without the fix (CI: job `gates`) | `bash scripts/miscompile-gate.sh` |
| RL9 | `scripts/image-check.sh`: every image our writers make (ELF, PE, Mach-O) through readelf, llvm-readobj, llvm-objdump, llvm-dwarfdump, dumpbin, objdump, `macho-check.py` | `bash scripts/image-check.sh` |
| RL11 | `scripts/verify/variants.py mutate`: also backend mutants (a rule, two x86-64 encodings, the allocator's clobber sets) on `nat` | `python scripts/verify/variants.py mutate --only rules-sub-zero-left` |
| RL12 | `scripts/verify/coverage.py --modes ...,emit:aarch64-linux,nat --gen 50 --goal 90` | see its header |
| RL13 | `scripts/verify/repro.sh`: also the machine layer's images; `MANIFEST=` and `--compare` across hosts; `HOSTSIM=1` (another stdlib path, user, zone, locale) and `PAR=4` (worker threads) on one host; `scripts/verify/repro-hosts.sh`: Windows against Linux under WSL; differences located by `scripts/verify/bin-diff-locate.py`; [rl13-rl14-2026-11-18.md](rl13-rl14-2026-11-18.md) | `MANIFEST=build/m.txt bash scripts/verify/repro.sh`; `bash scripts/verify/repro-hosts.sh`; `bash tests/tooling/rl13-repro-check.sh` |
| RL14 | `scripts/native-fixpoint.sh`: the compiler built by `fid emit`, stage 2 == stage 3; a drift located into `drift.txt`, `STAGE4=1` tells a settling drift from nondeterminism; [rl13-rl14-2026-11-18.md](rl13-rl14-2026-11-18.md) | `STAGE4=1 bash scripts/native-fixpoint.sh`; `bash scripts/native-fixpoint-check.sh` |
| RL15 | `COMPILER=1 scripts/sanitize.sh`: the compiler itself built with ASan/UBSan checks, lowers and runs the tests; from Windows, `scripts/verify/sanitize-compiler-wsl.sh` does it in WSL (fid's C built plain and sanitized there, each step compared with the plain build); [rl15-rl22-2026-11-18.md](rl15-rl22-2026-11-18.md) | `COMPILER=1 CC=clang bash scripts/sanitize.sh`; `FID=$FID SELF=1 bash scripts/verify/sanitize-compiler-wsl.sh`; `bash tests/tooling/rl15-sanitize-compiler-check.sh` |
| RL19 | [kill-switches.md](kill-switches.md) + `scripts/switch-check.sh` (CI: job `gates`) | `bash scripts/switch-check.sh` |
| RL21 | `scripts/fuzz/reduce.py`: `--bad/--good` (disagreement), `--fails <backend>` (same failure), `--cmd` (any command) | see its header |
| A11, M7, L7E, pe 89 | `scripts/verify/selfhost-survey.sh` (tests/selfhost per backend, times, first errors); `SUITE=1 tests/checked_alloc/run.sh` (the suite with `--checked-alloc` against the plain build, FAULT = a real memory bug); `tests/tooling/lint-unused-check.sh` + `scripts/verify/lint-scan-predict.py` (the lint scan in Python, `--gate`); `bench/mixed.sh` (the mixed workload, native against C -O0); [a11-m7-lint-mixed-2026-11-18.md](a11-m7-lint-mixed-2026-11-18.md) | `FID=$FID bash scripts/verify/selfhost-survey.sh`; `FID=$FID SUITE=1 bash tests/checked_alloc/run.sh`; `FID=$FID bash tests/tooling/lint-unused-check.sh`; `FID=$FID bash bench/mixed.sh` |

`scripts/verify/build-variant.sh <tree> <out.exe> [FILE:FROM=>TO ...]` builds
a compiler from a copy of a tree with edits; VF4, VF7, VF8 use it.
`scripts/verify/expect.py` checks `//>` (an output line), `//! panic`,
`//! error: words`, `//! exit: N` and `//@ backends:` in a test file.

## Schedules (Nov 15)

| Workflow | When | What |
|---|---|---|
| `.github/workflows/verify-nightly.yml` | every night, and by hand | VF1 (`nightly.sh`, plain and `--rich`), VF2 (Clang and GCC against `run`, `ccompile`, `ccompile-O2`, `emit:x86_64-linux`), VF3 (`check` and `emit`), VF15; VF14 ASan + UBSan over the suite and ThreadSanitizer over the threaded programs. The seed is the date; findings are the run's artifact |
| `.github/workflows/verify-weekly.yml` | Saturdays, and by hand | VF13 clean-tree bootstrap with the fixpoint; VF7 mutants; VF4 every run-time switch and the ranges off; VF8 coverage in the run's summary; VF12 reproducibility (and a manifest to compare with a Windows one); VF18 3000 interleavings; BM3/BM4/BM6/BM7/BM8 benchmark tables on Linux |

Neither had run when they were written (Nov 15): the first runs are
their first record.

## What a user meets (release-roadmap N, A10; competitive-roadmap ST6, ST10)

| Script | Checks | Runs in |
|---|---|---|
| `scripts/ark-check.sh` | N1: `ark init`, `add --path`, `install`, `list`, then `fid run` and `fid ccompile` of an app using the pact, on a fresh directory and HOME | ci.yml (Linux, Windows), macos.yml |
| `scripts/guide-check.py` (via `quickstart-check.sh`) | N2: docs/reference/quickstart.md followed step by step, its "Output:" blocks compared | ci.yml (Linux, Windows) |
| `scripts/fmt-check.sh`, `scripts/fmt-comments.py` | N4: `fmt --check` on the tree; on formatted copies of tests: stable, comments kept, same `fid run` output | ci.yml |
| `scripts/lint-check.sh` | N5: `fid lint` on every stdlib module, against `scripts/lint-check.baseline` when there is one | ci.yml |
| `tests/tooling/lint-style-check.sh` | L7E style lints: the naming and import-order warnings of tests/bootstrap/test_style_lint.fd and tests/tooling/lint_style/edge.fd, the Python predictor agreeing with fid, the five examples clean ([l7e-style-lints-2026-11-18.md](l7e-style-lints-2026-11-18.md)) | by hand |
| `scripts/projects-check.sh` | N11: the five projects of tests/projects/ built, run, lint-clean | ci.yml (Linux, Windows) |
| `scripts/install-layout-check.sh` | A10, N12: `fid version`, `run`, `ccompile`, `emit` from an installed layout, started from PATH | ci.yml; release.yml on the installed and unpacked builds |
| `scripts/release-fresh.sh` | A10: release/fid is the stamped release build of this source | release.yml (bootstrap.sh writes the stamp) |
| `scripts/crater.sh`, `scripts/crater/projects.txt`, `scripts/crater/baseline.txt` | ST6: every known project builds as it did with the last release (baseline: release/fid.exe 0.8.0 with its stdlib, [st6-crater-baseline-2026-11-18.md](st6-crater-baseline-2026-11-18.md)) | release.yml |
| `scripts/flake-repeat.sh` | A12: a test run many times beside busy processes, times against the harness limit | by hand |

`.github/workflows/release.yml` (tags, or by hand) builds with the 3-stage
bootstrap on Windows and Linux, installs, updates and uninstalls on the
clean runner, runs crater, and attaches `SHA256SUMS` and a provenance
attestation per file to a draft release (ST10). None of these had run when
they were written (Nov 15).

## Recorded runs (Nov 7 2026, Windows x64, parent's Nov 7 20:50 `fid`)

See the VF lines of the roadmap for the numbers; each line there names the
run it reports.

---

## `fid lint` on the standard library (release-roadmap N5)

`scripts/lint-check.sh` lints every `stdlib/*.fd` module and compares the
result with `scripts/lint-check.baseline` (one `module|state|count` line per
module: `clean`, `warnings` with their count, or `unchecked` when the module
does not pass `fid check`). CI runs it (`.github/workflows/ci.yml`, "lint on
every stdlib module"). `fid lint` resolves `use` imports
(`compiler/compiler.fd` `lint_with_imports`), so a module that needs another
module is linted like any other; `tests/tooling/lint-imports-check.sh` checks
that.

### State on Nov 17

Linted with the Nov 16 compiler (the stage-0 of the day's build), after the
fixes below, on the 163 modules outside the network/crypto set and the 200
modules another session was adding that day: 128 clean, 34 with warnings,
1 unchecked. What is left is not wrong in the modules:

| Kind | Modules | Why it stays |
|------|---------|--------------|
| `unused import` on a module that is used | 32 warnings (string, math, money, calendar, arena, encoding, postgres, datetime, unicode_data, db, ...) | The lint counts an import as used only when a called function starts with `<module>_`. `str_join` (string), `pledge_sqrt`/`vec3` (math), `Money_new`, `cal_*`, `frame_*`, `pg_*` do not. Needs the checker to ask which module registered the called name (`TypeChecker_get_item_module`). `use db` is used for its `cap Database` alone: traits are not registered with a module. |
| `unused function` on a function used as a value or in an initializer | shape `vec_lit` (const initializers), trace `trace_state_new` (a global's initializer), xml `x_no_resolver` (`resolver: x_no_resolver`) | The lint collects calls in function bodies only. |
| Naming | math `vec2 vec3 vec4 mat4 quat` (structs), sdl2_h's C constants (`SDLK_a`, `SDL_ScaleModeNearest`, ...), shape's `shape_2x3` constants | Public names used across the stdlib and its tests (`vec3` by most game modules); sdl2_h mirrors the C header. |
| unchecked | scada | `fid check` rejects `modbus_crc16`: the safety-annotation check resolves `data.len()` on a `Vec<i64>` to `StringBuilder_len` (core's StringBuilder now has a `len` method). A checker bug, not the module's. |

### What was fixed

- Imports nothing used: `use string` in 42 modules, `use math` in plot and
  units, `use tensor` in kv_cache, dp, shard and quant, `use encoding` in
  unicode; a repeated `use` line in audio_mixer.
- Import order (core, vec, string, then by name) in 15 modules.
- Unused locals (bigint, camera, clinical, collections, fs, math slerp, skel,
  tz) and dead private helpers (json `j_lit`/`j_scan_*` wrappers, regex's
  token accessors, log `log_safe_byte`, semver `sv_full`, virtio `vu64`);
  acpi `sdt_sig_str` is public now.
- Parameters a stub or a fixed answer does not read are named `_x` (atomic's
  `Order`, arena, async, ecs, events, game, world's refusing handlers, ...).
- Unused parameters that were bugs:
  - scene: `scene_update_entity`, `scene_set_velocity`, `scene_damage`,
    `scene_set_tag` and `scene_despawn` changed a copy of the entity; they
    store it back now (`tests/bootstrap/test_scene_entity_updates.fd`).
  - sprite: `sprite_controller_update` dropped the advanced animation and
    `sprite_controller_play` did not start the track; streaming:
    `streaming_set_chunk_data` wrote nothing
    (`tests/bootstrap/test_sprite_stream_updates.fd`).
  - gl: `gl_compile_shader_src` passed null to glShaderSource and to
    glGetShaderiv/glGetProgramiv (no source compiled; the status read
    crashed or was always 0); it passes the source pointer array and reads
    the status from a block now.
- terrain's `hL`/`hR`/`hD`/`hU` are `h_l`/`h_r`/`h_d`/`h_u`.

`scripts/lint-check.baseline` records these 163 modules, core and the 37
network/crypto modules (201 lines). The 200 modules the other session was
still writing are not in it: the gate requires a module missing from the
baseline to be clean, so record them (`--save`) once that work is done.
On Nov 21 the baseline still has those 201 lines; `stdlib/` has 403
modules.

### Running it

```bash
VERBOSE=1 bash scripts/lint-check.sh release/fid.exe   # every warning
bash scripts/lint-check.sh --save release/fid.exe      # record the state
```

---

## SL5: every address of a name, on every back end (Nov 17 2026)

Competitive roadmap SL5 asks that a connect by name try every address the
name has, IPv6 ones included, and that this be run against a real resolver.

### What changed

`--nat` Windows (the machine layer's PE, `stdlib/linux_runtime/win.fd`)
asked `gethostbyname`, which gives IPv4 addresses only, and connected to the
first one. Now:

- `win.fd` `pe_win_resolve_all` (pseudo system call 1003) asks
  `getaddrinfo` for any family and TCP, as pe_runtime's
  `pert_net_connect_sync` does, and returns up to 16 records
  `[family 2|10][IPv6 scope id][address]` in the order Windows ranks them
  (RFC 6724), then frees the list with `freeaddrinfo`.
- `sys.fd` `pert_net_connect` on Windows calls `pe_win_connect_all`, which
  tries each record until one connects (`pe_connect_sa`, task-aware), with
  the scope id kept for link-local IPv6. `pe_resolve6` on Windows uses the
  same resolver (`pe_win_resolve6`) instead of giving up.
- The two imports are `PE_W_GETADDRINFO` (75) and `PE_W_FREEADDRINFO` (76):
  `compiler/nat_ir.fd` `ng_win_imports` must list
  `ws2_32.dll|getaddrinfo` and `ws2_32.dll|freeaddrinfo` at those indexes
  (after `ntdll.dll|NtSetTimerResolution`). Without them a `--nat` program
  that connects by name reads the TLS index as an import and stops with
  "Invalid memory access ... at 0x2" (seen with the Nov 16 compiler).

### Runs (Nov 17, Nov 16 compiler `stage0`, today's runtime files)

| Back end | Program | Result |
|---|---|---|
| pe_ir (default `fid emit`, Windows) | `tests/net/real_connect.fd` | connected, HTTP status line, `.invalid` gives -1 |
| pe_ir | `tests/bootstrap/test_runtime_connect_retry.fd` | all five lines as expected (`::1: true`) |
| native Linux x86-64 (`emit --target x86_64-linux`, WSL, resolv.conf nameserver 10.255.255.254) | `tests/net/real_connect.fd` | connected, HTTP status line, `.invalid` gives -1 |
| native Linux x86-64 (WSL) | `test_runtime_connect_retry.fd` | all five lines as expected |
| `--nat` Windows | `test_runtime_connect_retry.fd` | `listening: true`, then the crash above: the Nov 16 compiler has no getaddrinfo import. To be rerun with the final compiler. |

The Nov 16 compiler's native Linux builds needed `FID_NAT_OFF=ldyn`: with
load-time binding on, it imported the runtime primitive `nat_tls_set` from
the C library ("undefined symbol: nat_tls_set"), a code generator matter
outside the runtime.

### How to check it again

```bash
FID_HOME=$PWD $FID emit tests/bootstrap/test_runtime_connect_retry.fd --nat -o r.exe && ./r.exe
FID_HOME=$PWD $FID emit tests/net/real_connect.fd --nat -o n.exe && ./n.exe
FID_HOME=$PWD $FID emit tests/net/real_connect.fd -o p.exe && ./p.exe
FID_HOME=$PWD $FID emit tests/net/real_connect.fd --target x86_64-linux -o n_lx && wsl ./n_lx
```

Each prints the "Expected output" lines of its file.
