# Sanitizers over the whole suite, Nov 17 2026 (VF14)

The first whole-suite run of the C path under AddressSanitizer +
UndefinedBehaviorSanitizer, and ThreadSanitizer over every program that
uses threads, tasks, channels or atomics. The sample run of Nov 16 (45
programs, findings S1-S4) is in
[infra-runs-2026-11-16-b7.md](infra-runs-2026-11-16-b7.md).

## How it was run

On a busy shared Windows 11 machine, gcc 15.2 in WSL2 (Kali, 16 cores).

- **Compiler**: the newest built `fid` on the machine, a one-stage build of
  the Nov 16 tree (`stage0`). None of the day's changes to
  `compiler/*.fd` were built (the whole tree is built once at the end of
  the run), so the C of each test had two sed scripts applied:
  `scripts/verify/sanitize-patches.sed` (S1-S3, as in the Nov 16 run) and
  one with the C runtime changes of this run (the ones listed under
  "Fixed" below). A finding that the patched C still showed was looked at
  in the source of the tree, not of stage0.
- **Reference output**: stage0's `fid run` with HEAD's `stdlib/core.fd`
  (stage0's interpreter lacks `p_region_word`, which today's core calls).
  Tests added since the last commit had no reference run: they count only
  sanitizer reports, and their output was compared with the
  `// Expected output:` block written in the test where there is one.
- **Steps** are those of `scripts/sanitize-wsl.sh` (Windows writes the C
  and the reference, WSL builds and runs), run 6 programs at a time with the
  binaries in WSL's `/tmp`; the script itself runs one at a time, which for
  ~1,100 programs of 1-2 MB of C each is most of a day.
- Tests: every `tests/bootstrap/test_*.fd` but `*_fail`, `test_sqlite`,
  `test_gpu*`, `test_sdl*` (the list of `sanitize-wsl.sh`), taken at 15:00
  and again at 19:15 for the tests added in between (other sessions were
  adding the `test_std_*` modules during the run).

## ASan + UBSan: 1,118 tests

| Result | Programs |
|---|---|
| clean, same output as `fid run` | 341 |
| clean, no reference (new test, or stage0's interpreter cannot run it) | 615 |
| ... of those, output equal to the test's "Expected output" block | 535 (37 differ, each a case below; 43 have no block) |
| sanitizer report | 17 (all explained below; none left in the tree's runtime) |
| output differs from `fid run` also without sanitizers | 24 (all explained below) |
| not built in WSL (C libraries: SDL, GL, sqlite, OpenSSL, Objective-C, Android; inline C of another target) | 46 |
| `dump-c` failed (stage0's front end predates the test's feature) | 75 |

Over all programs with an expected-output block: 568 equal, 58 different.
Most differences are tests added today with a front-end or lowering fix
stage0 does not have: function values (test_fix_fnval_*, test_await_spawn_collections,
test_frontend_fn_value_args, test_lowering_async_generic_*), `f64` in Vec and
HashMap (test_fix_map_f64_get, test_fix_vec_filled_f64,
test_vec_infer_float_var), test_closure_capture_owner, test_coalesce_str,
test_global_u64, test_variadic_pack, and ten of the new `test_std_*`
modules (apic, app_lifecycle, grpc, hpet, middleware, netcode, smp, smtp,
wav, xhci), which stop with SIGABRT in stage0's dispatch of a function value
read from a struct field (`val f = b.wr; f(off, v)`, stdlib-agent-bugs 30).
Of the rest, the tests added since the last commit (test_regex_flags,
test_stack_box, test_midend_inline2_vecfast, test_frontend_gen_eq,
test_call_lowering_parity, test_cparser_include, test_fix_float_inf_nan,
test_verify_bm_*) check other changes stage0 predates, and these are
environment ones
(`test_cparser_dll_exports` reads msvcrt, `test_world_sockets` binds a port
twice, which Linux allows with `SO_REUSEADDR`, `test_site_render` and
`test_qualified_names` read files relative to the working directory,
`test_io_stdin` reads a stdin the run closes, the blocks of `test_bare_fault`
and `test_bare_panic_handler` describe a bare-metal run).

### Sanitizer reports

**Fixed in compiler/codegen_c.fd this run** (each rechecked: the same C with
the change patched in is clean under ASan+UBSan and prints the expected
output):

- S5. **Text built with the C library given to `p_str_own_c`.** The runtime
  functions that build their result with `malloc`/`strdup`/`realloc`
  (`p_db_get`, `p_db_mem_query`, `p_json_parse`, `p_json_pretty`,
  `p_json_string`, `p_json_get`, `p_simd_to_str`, `p_tensor_to_json`,
  `p_find_quant_params`, `p_http_build_response`, `p_http_parse_request`,
  `p_socket_recv_str`, `p_read_stdin`) went through `p_str_own_c`, whose
  header probe reads 16 bytes in front of the block (heap-buffer-overflow:
  test_db, test_derive_serialize, test_serde, test_simd, test_machine_simd,
  test_tensor, test_quantization, test_ml_edge_cases), and the C library's
  buffer was never freed. Now `p_str_own_libc_c` copies it into a counted
  string and frees it; `p_getenv`, `p_argv` and `p_read_line`, whose text
  the runtime does not own, go through `p_str_copy_c` (copy, nothing read
  in front, nothing freed).
- S6. **`p_json_get_int`/`_float`/`_bool` freed a block-allocator string
  with `free()`** (C heap corruption once `p_json_get`'s result became a
  counted string): `p_str_free_c`. Likewise the canonical text of a nested
  object or array inside `p_json_get` (copied with `strdup` for the
  caller, then freed with `p_str_free_c`).
- S7. **`p_process_stdout`/`p_process_stderr` of a bad handle returned the C
  literal `""`** to `p_str_own_c` (global-buffer-overflow:
  test_runtime_process_capture): a counted empty string.
- S8. **`p_file_read` of a directory**: glibc opens it and `ftell` gives
  `LONG_MAX`, so `sz + 1` was negative and `p_malloc` aborted
  (test_files_spec printed nothing after line 55): a size no file has reads
  as a missing file, as on Windows.
- `p_read_stdin` at end of input leaked its buffer.

**Not in the tree's C** (stage0's C only):

- test_runtime_vblk, test_runtime_memchr: misaligned 8-byte stores in core's
  `p_mem_copy` (UBSan) when source and target are not word-aligned. The
  tree's C generator replaces that function's body with `memmove` (OP23,
  `fast_core_vec_body`); with it, both are clean. The interpreter keeps the
  word loop (its memory has no alignment), pe_ir and nat_ir have their own
  `pert_mem_copy`.
- test_fix_deref_assign_struct (heap-use-after-free in `p_str_concat`),
  test_fix_fnval_c_ptr (misaligned load: a C address called as a closure),
  test_fix_fnval_vec_ret (overflow in `strlen`: a `Vec<str>` from a function
  value read as numbers), test_map_struct_keys (a struct key compared with
  `p_str_equal`, reading in front of a stack copy),
  test_trait_generic_method_generic_impl (a generic method's `str` read from
  the stack): each test was added with the lowering, type-checker or mono
  fix it checks (stdlib-agent-bugs 10, 18, 34, 45, 49, 50; RT5; LG4), none
  of which stage0 has. To recheck with the final compiler.
- test_deref_minimal: the program stores through a null pointer on purpose
  (`var p: *mut i64 = 0; *p = 42;`); UBSan reports exactly that.

### C-path output differences (not sanitizer findings)

The five named in the roadmap:

- **test_c_callback**: the interpreter cannot call C (`qsort` here) and
  stops with a message saying so; the C build prints `432103`, which is
  right (3 0 2 4 1 sorted descending, then 3 found at index 3 of the
  ascending order).
- **test_metrics_serve**: the C build is right (`requests_total 5` is in the
  served text). The interpreter was wrong: stage0 gives a struct global one
  8-byte slot, so `http.fd`'s next global (`http_tls_recv`) overwrote the
  second field of `metrics_srv` (stdlib-agent-bugs 25; the help text printed
  as `hc_no_tls_recv`). Fixed in the tree (`Interpreter_init_globals`
  sizes a global by its type; test_fix_global_struct); stage0 still prints
  7 for that bug's 5-line repro.
- **test_references**: no difference. The test prints a string literal that
  spans two source lines, and the file has CRLF line endings, so the
  literal holds `\r\n`; the comparison had removed `\r` from the reference
  only. test_http, test_lang_features, test_lib_medical, test_return_order,
  test_secret and test_vec_f64 are the same case.
- **test_midend_egraph_unary**: clean, and its output equals its expected
  block (the Nov 16 difference does not reproduce).
- **test_async_suspend**: the C build prints `1 2 100 12 11 6 hi! 54321`,
  what the test's comment describes (an `async fn` runs until it first
  waits, then its caller goes on). The interpreter runs each task until it
  returns (`1 11 2 12 100 6`): compiler/interp.fd does not interleave tasks
  (AGENTS.md, interp.rs row), a known limit, not a C-path bug.

The others: the interpreter (stage0, HEAD's core) failed where the C build
printed the expected output: test_decimal, test_pe_defer, test_vec_builtins
(out of arena memory), test_defer, test_deref_assign2, test_lib_stack,
test_refs_mut_methods, test_regression_collections (exit 1, no message),
test_ws_local (an arithmetic panic), test_http_server (no `p_co_accept` in
the interpreter); test_zeroize_vec prints `interp: skipped` in the
interpreter by design; test_files_spec is S8. One is a C-path question:
**test_comptime_reflection2** indexes `comptime function_capabilities(show)`,
which comes back empty; the interpreter stops with "index 0 but array has 0
elements", the C build prints 0 (no bounds check on that array). Not
followed up here (front end and lowering).

## ThreadSanitizer: 90 programs

The 22 of `sanitize.sh`'s TSan set and every other test whose source uses
`spawn`, tasks, channels, atomics or threads.

| Result | Programs |
|---|---|
| clean, same output as `fid run` | 33 |
| clean, no reference | 50 |
| output differs from `fid run` also without TSan | 2 (test_async_suspend, test_http_server: as above) |
| TSan report | 3 |
| not built | 2 (test_std_sched, test_stdlib_postgres_tls) |

**No data race was reported.** The three reports (test_std_hpet,
test_std_netcode, test_std_wav) are one runtime finding:

- S9. **The crash handler allocated inside the signal handler.** A program
  that faults (here: stage0's lowering of a function value read from a
  struct field, which the tree fixes) enters `__p_sig_handler`, which names
  the frames with `__p_fn_name`; the first call sorts the function table
  with `malloc` and `qsort` ("signal-unsafe call inside of a signal"; a
  deadlock when the fault was inside `malloc`). Fixed in
  compiler/codegen_c.fd `emit_runtime_impl`: the POSIX handler and the
  Windows crash handler set `__p_fn_linear` first, and `__p_fn_name` then
  searches the unsorted table without allocating. Rechecked by patching
  the three programs' C: no TSan report, the traces still name each frame.

## Validation with the final compiler

```bash
# the tests of each finding (ASan+UBSan): every one clean but test_deref_minimal
FID=$FID OUT=/c/tmp/vf14a bash scripts/sanitize-wsl.sh test_db test_derive_serialize \
  test_serde test_simd test_machine_simd test_tensor test_quantization \
  test_ml_edge_cases test_runtime_process_capture test_files_spec test_runtime_vblk \
  test_runtime_memchr test_fix_deref_assign_struct test_fix_fnval_c_ptr \
  test_fix_fnval_vec_ret test_map_struct_keys test_trait_generic_method_generic_impl \
  test_free_escape test_c_callback test_metrics_serve test_references \
  test_midend_egraph_unary test_async_suspend
# the crash handler (S9): no "WARNING: ThreadSanitizer" in $OUT/*.san
FID=$FID OUT=/c/tmp/vf14t SAN=thread bash scripts/sanitize-wsl.sh test_deref_minimal test_std_hpet
# the whole suite (long: run it overnight, or in parts with SAMPLE=)
FID=$FID OUT=/c/tmp/vf14all bash scripts/sanitize-wsl.sh
FID=$FID OUT=/c/tmp/vf14tall SAN=thread bash scripts/sanitize-wsl.sh
```
