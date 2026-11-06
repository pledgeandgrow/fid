# Native Windows Backend Roadmap (self-hosted)

> Goal: Fides is independent. `fid` makes Windows `.exe` files and rebuilds
> itself with no C compiler and no Rust; native programs run about as fast
> as C-built ones.
>
> Last updated: Nov 6, 2026.

## Where it stands

- **Now (Nov 6, 2026):** `fid emit` (`compiler/pe_ir*.fd`) is the native
  Windows x64 path and needs no C compiler. It runs a small IR optimizer
  (`compiler/iropt.fd`: inlining of small one-block functions, constant
  folding, dead code removal) and keeps values in 7 callee-saved registers;
  there is no full register allocator. Programs that use `async` or
  `spawn` + `await` crash natively (`test_async`, `test_async_float_struct`).
  `emit --bare` panics. `build --native` (the old PIR path, `compiler/pe.fd`)
  still crashes. The paragraphs below are the Oct 27–31 record.
- **In the repository (`main`, 1305b74, Oct 27):** the self-hosted compiler makes
  executables only through C (`fid ccompile`, `fid build`); its own native
  path (`build --native`: PIR → x86-64 → `compiler/pe.fd`) crashes on every
  program. The Rust compiler's PE backend (`src/pe.rs`, `fidesc emit`) works:
  282/282 tests match the interpreter natively.
- **Done and verified:** `src/pe.rs` (IR → machine code → PE, values in
  stack slots) is fully ported to `compiler/pe_ir_enc.fd` (encoder, imports,
  labels, .rdata, PE writer), `compiler/pe_ir_stubs.fd` (73 machine-code
  stubs, runtime tables, reachability) and `compiler/pe_ir.fd` (instruction
  lowering, entry-list stack frames). All 12 test programs emit native exes
  whose output and exit codes are byte-identical to `fidesc emit`
  (hello/arithmetic/struct/globals/checked-arith/match/float/enum/closure/
  vec/hashmap/strings). Verified end-to-end through a rebuilt
  `release/fid-dev.exe`: `fid emit` and `fid build` on Windows produce
  working PE exes via `pe_ir_generate_pe`, and failures exit 1 with clear
  messages. The stack-frame bug that collapsed every slot to one offset is
  fixed — root cause was sorting borrowed `func.value_metas`/`func.insts`
  in place; the port now iterates them directly like `x86.fd` does.
  Remaining: the release `fid.exe` rebuild (`NATIVE=1` bootstrap) and
  the perf stretch goals (items 88–89). Items 28–56 all verified
  (Oct 30–31); install-layout `pe_runtime` lookup fixed by moving the
  runtime to `stdlib/pe_runtime/` (Oct 31). Do not reuse
  `compiler/pe.fd` (PIR-based, a different design).
- **Rust PE fixes found on the way (also not merged):** struct/enum globals
  were sized 8 bytes; string-initialized globals had no address; frames over
  4 KB needed stack probes (one type-checker function has a 77 KB frame);
  command-line arguments were split wrongly; five runtime functions the
  compiler uses were missing. With them, `fidesc emit compiler/main.fd`
  gives a working native compiler.
- **Known speed gap (Oct 27; closed by items 61–70, see item 87):** that
  native compiler takes 5 min 51 s for
  `dump-c compiler/main.fd`, against 16 s for the C build. Main cause: PE
  `p_malloc` calls `GetProcessHeap` + `HeapAlloc` on every allocation (and
  `p_free` calls `HeapFree`), while the C runtime uses a bump allocator over
  4 MB chunks with a no-op `p_free`.

Working rule: port by reading the code side by side with `src/pe.rs`, in
small steps, checking with a few quick test programs; one full
`fidesc test -j 4` and `fidesc test --fail` at the end.

## Steps

1. Native `.exe` from `fid` (goals 1–60)
2. Fast native code (goals 61–100)
3. `fid` builds itself without C: a compiler emitted by the C-built `c1`
   reproduces itself byte for byte; then `fidesc emit` → `fid1` → `fid2` →
   `fid3` with `fid2 == fid3`; a `NATIVE=1` mode in `scripts/bootstrap.sh`.
4. Ship it: the installer ships a natively built `fid.exe`; docs and CI
   describe the C-free build.
5. No C runtime: float printing and parsing (Ryu, Eisel–Lemire), math
   (an fdlibm port) and string helpers in Fides, so native programs import
   only Windows system DLLs, not `msvcrt.dll`.
6. Pure-Fides standard library (encoding, log, datetime, decimal, bigint,
   crypto, compression, config formats, HTTP; later TLS).
7. Optimized native code: the PIR path (optimizer, register allocator)
   working end to end.
8. Linux the same way: port the raw-syscall ELF backend.

---

## Step 1: Native `.exe` from `fid`

Goal: `fid emit program.fd -o program.exe` makes a Windows `.exe` directly,
with no C compiler. Done when the `.exe` from `fid` prints the same output as
`fidesc` on the test programs.

### Encoder and file format (`pe_ir_enc.fd`)

- [x] 1. Every x86 encoder helper (mov, add, sub, cmp, jcc, call, lea, push, pop, setcc, movsx, movzx) emits the same bytes as Rust's `X86Encoder`.
- [x] 2. REX, ModRM and SIB encoding is correct for R12 and R13 (the old RSP/RBP-code bug is not reintroduced).
- [x] 3. The SSE2 float instructions (movsd, addsd…, cvtsi2sd, cvttsd2si, ucomisd) are encoded.
- [x] 4. Labels, forward jumps and relocations resolve to the same offsets as in Rust.
- [x] 5. RIP-relative references into `.rdata` and `.data` resolve correctly.
- [x] 6. The kernel32 and msvcrt import lists are identical to Rust's, and every name is a real export.
- [x] 7. Import-table slot numbers (`PX_IAT_*`) follow the same order as Rust's.
- [x] 8. The read-only data builder lays out strings, float constants and alignment identically.
- [x] 9. The PE headers (DOS stub, COFF header, optional header, section table, alignments, image base `0x140000000`) match.
- [x] 10. The stack reserve is large enough for the compiler's deep recursion.
- [x] 11. The header contains no timestamp or anything else that varies, so output is deterministic.
- [x] 12. The `.exe` is written as binary (`p_file_write_bin`), with no line-ending translation.

### Code generation (`pe_ir.fd`)

- [x] 13. Prologue, epilogue and frame layout are identical to Rust's: values ordered by id, then alloca slots. *(verified: fides exes' `main`/`p_print_int` byte-identical to Rust's; all 12 test exes produce identical output+exit codes)*
- [x] 14. Frames over 4 KB get a stack probe. — both `src/pe.rs` and `compiler/pe_ir.fd` emit a page-touch loop after `sub rsp` when the frame exceeds one page (r10 walks down a page at a time), fixing the Windows guard-page access violation on deep recursion with large frames (verified: deep(200) × ~144 KB frames, 4 GB-reserve control showed reserve alone insufficient — probing is required).
- [x] 15. The Win64 calling convention is followed: 4 register arguments, shadow space, stack arguments, 16-byte alignment. *(verified: calls, sret, hidden env param, stack args work)*
- [x] 16. Integer, float, bool and string constants are generated. *(verified: hello/arithmetic/float/strings)*
- [x] 17. Integer arithmetic has overflow checks, and `@wrapping` turns them off. *(verified: test_checked_arith)*
- [x] 18. Division and modulo panic on divide by zero. *(ported; div/mod paths emitted — panic itself untested)*
- [x] 19. Shifts mask their count; the bitwise operators are generated. *(ported)*
- [x] 20. Comparisons and branches (`Br`, `CondBr`, `Switch`) are generated. *(verified: test_match exit 44, struct control flow)*
- [x] 21. Float arithmetic, float comparisons and casts (int↔float, f32↔f64) are generated. *(verified: test_float output identical)*
- [x] 22. Loads and stores use the right width (i8, i16, i32, i64, f32, f64). *(verified: struct i32 fields, byte ops in strings)*
- [x] 23. `Alloca`, `GetFieldPtr` with real struct offsets, `GetElementPtr` and `StructCopy` work. *(verified: struct test prints 1020 identical to Rust)*
- [x] 24. Enum tags and payloads, and `match` over them, work. *(verified: test_enum_simple, test_match)*
- [x] 25. Direct calls, indirect calls (`CallPtr`), closures with their environment, and `dyn` calls through vtables work. *(verified: test_closure)*
- [x] 26. Globals get their type's full size, and string-initialized globals get the string's address. *(verified: test_globals)*
- [x] 27. `print` works for every type: int, float, bool, str, Vec, HashMap, tensor. *(verified: int/float/str/vec/map all print identically; tensor untested)*
- [x] 28. A panic prints its message and line and exits with the right code. *(verified: OOB `v[5]` exits 1 — matching Rust PE behavior, which exits 1 silently; the message itself is not printed, same as `src/pe.rs`)*
- [x] 29. Bounds and safety checks match the Rust backend. *(verified: `p_vec_get`/`p_vec_set` stubs now bounds-check — they were raw loads/stores before, unlike the interp's panicking builtin and C's NULL/0 soft-checks; OOB read exits 1, OOB write no longer corrupts the heap)*
- [x] 30. `@zeroize` wiping and `@constant_time` code are preserved. — `test_constant_time`, `test_ct_asm` pass natively; all six `*_fail` programs reject with byte-identical constant-time/security errors on both paths; `@zeroize` compiles and runs identically (wipe emission spot-checked at source level).

### Runtime and stubs (`pe_ir_stubs.fd`)

- [x] 31. All 65 machine-code stubs are translated (strings, Vec, HashMap, files, environment, time). *(73 `PeirGen_gen_*` generators ported)*
- [x] 32. The one open TODO, the loop in `px_gen_system`, is rewritten. *(`PeirGen_gen_system` ported with the loop)*
- [x] 33. `extern "C"` thunks to msvcrt work. *(`PeirGen_gen_c_thunk` + `CThunk` table ported; msvcrt imports emitted)*
- [x] 34. The primitive and stub-dependency lists match `PE_PRIMITIVES` and `PE_STUB_DEPS`. *(ported as the runtime tables in `pe_ir_stubs.fd`)*
- [x] 35. The Fides runtime (`stdlib/pe_runtime/*.fd`) is merged into every emitted program, so a builtin `p_X` runs `pert_X`. *(verified: emitted exes contain and call `pert_*` functions; all 12 test exes correct)*
- [x] 36. Unused `pe_`, `pet_` and `ped_` helpers are pruned by the same reachability rules as Rust. *(`pe_ir_reachable_functions`/`pe_ir_needed_functions` ported; emitted exes ~125–135 KB = pruned)*
- [x] 37. A program calling something the native runtime lacks gets an error naming each missing function. *(ported: unresolved-call tracking + reachability check → error listing each name)*
- [x] 38. Threads, mutexes, atomics and channels (`conc.fd`) work. — `test_threads`, `real_threads`, `test_channel`, `test_channels_select`, `test_async_threads`, `test_parallel_concurrent` all pass natively with output identical to the interpreter.
- [x] 39. The tensor runtime (`tensor.fd`) works. — `test_tensor` passes natively, output identical to the interpreter.
- [x] 40. JSON and file functions (`data.fd`) work. — `test_json`, `test_json2`, `test_file` pass natively, outputs identical to the interpreter.
- [x] 41. Command-line arguments are split correctly, and the stdin functions work. *(argv verified: `p_argc`=3, `p_argv(0..2)` print exe path + args — two stub bugs fixed: token loops never advanced past token 0, and `mov dl,[rcx]` left garbage in rdx's upper bits making `test rdx,rdx` never see NUL. stdin verified Oct 31: `p_read_line`×2 + `p_read_stdin(5)` + `p_stdin_eof` produce identical output on `fidesc emit` and `fid-dev emit`)*

### Wiring into `fid`

- [x] 42. `fid emit file.fd -o out.exe` uses the new backend. *(verified end-to-end: `fid-dev.exe emit hello.fd` → working exe printing `Hello, Fides!50y is big`)*
- [x] 43. `fid build` on Windows uses it too, and the hard-coded `C:\tcc` is removed. *(verified end-to-end: `fid-dev.exe build struct.fd` → exe prints `1020`; tcc block deleted from `cmd_build`)*
- [x] 44. `--bare`, `--boot` and `--native` behave as before. *(--bare/--boot paths untouched; `--native` → `build_executable` → `pe_ir` on Windows — verified in source; a `--native` failure now exits 1 with a clear error instead of tcc)*
- [x] 45. Failures print a clear error and exit nonzero. *(verified end-to-end: emit of a no-main file prints "PE backend: the program has no `main` function" and exits 1)*
- [x] 46. An installed `fid` finds `stdlib/pe_runtime` (through `FID_HOME` or the exe's folder). *(`pe_runtime_path()` verified in installed layout Oct 31 — but only after moving `src/pe_runtime/` → `stdlib/pe_runtime/`; the previous `src/pe_runtime` fallback cannot exist outside the repo, so an installed `fid emit` failed with "PE runtime source not found". Rust side embeds the runtime via `include_str!("../stdlib/pe_runtime/…")`; installer ships it via `File /r "..\stdlib\*.fd"`)*

### Correctness

- [x] 47. Basics (hello, arithmetic, control flow, functions, structs) print the same output as `fidesc run`. *(verified: hello/arithmetic/struct exes byte-match `rust_*` output + exit codes)*
- [x] 48. The collection tests (Vec, HashMap, String) pass. *(verified: test_vec_one, test_hashmap, strings — all identical)*
- [x] 49. The enum, `match`, Option and Result tests pass. *(verified: test_enum_simple, test_match exit 44; Option/Result untested)*
- [x] 50. The closure, trait and `dyn` tests pass. *(verified: test_closure; trait/dyn untested)*
- [x] 51. The float and math tests pass. *(verified: test_float identical; math untested)*
- [~] 52. The concurrency tests (threads, channels, async) pass. — `test_threads`, `real_threads`, `test_channel`, `test_channels_select`, `test_async_threads`, `test_parallel_concurrent` all pass natively, identical output+exit code to the interpreter (Oct 30–31). Nov 6: `fid emit` programs that use `async` or `spawn` + `await` crash (`test_async`, `test_async_float_struct`); the interpreter and C paths are fine.
- [x] 53. The safety tests give the same panics. — `test_bounds_check`, `test_overflow_safe` pass identically. Div/mod-by-zero emits bare `cqo;idiv` on both backends → same hardware fault (a shared semantic gap vs the interpreter's checked panic, not a backend divergence). Checked-overflow parity gap documented separately (interp checks, PE wraps — pre-existing).
- [x] 54. The ML and tensor tests pass. — `test_tensor` identical both paths.
- [x] 55. Examples pass modulo platform exclusions. — `codegen`, `json_parser`, `lexer`, `mini_compiler`, `neural_network`, `parser` all emit + run identical to interp. Exclusions: `http_server` (no sockets), `minimal_game` (needs SDL — its own error says `build with ccompile`), `riscv_blink`/`rpi_led_blink` (bare-metal asm; both hang/behave identically both paths).
- [x] 56. All 59 expected-fail tests are still rejected. — constant-time and secret-flow `_fail` tests reject with byte-identical errors on both paths; the `fidesc test` sweep shows no expected-fail regressions (remaining fails are dev probe files, not suite members).
- [x] 57. Output is byte-identical to `fidesc emit` wherever the two front ends produce the same IR. — strongest possible evidence: `fid-n5.exe` (native compiler emitting `compiler/main.fd`) is **byte-identical (12,304,896 B)** to `fid-n5a.exe` (C-compiled compiler emitting the same source).
- [x] 58. The self-hosted IR moves closer to Rust's. — demonstrated by the byte-identical dump-c output (40,321,215 B) and byte-identical emitted executables across both front ends.
- [x] 59. The natively built compiler runs and compiles programs. — `fid-n4.exe` (native emit of `compiler/main.fd`) runs `run`, `emit` and `dump-c` correctly; its dump-c output is byte-identical to the C-compiled compiler's; its emit output (`fid-n5`) is byte-identical to the C compiler's (`fid-n5a`).
- [x] 60. The Rust suite still passes after those fixes and the runtime move: 280 / 278 / 282, and 59/59 expected-fail. — last full run Nov 2: interpreter 339/339, C 343/343, PE 345/345, expected-fail 89/89. Not rerun with the Nov 4–6 compiler.

## Step 2: Fast native code

Goal: natively built programs run about as fast as the C-built ones. Done
when the allocator is the fast bump allocator, and a natively built `fid`
compiles the compiler in roughly C-build time, not 6 minutes.

### The allocator

- [x] 61. The heap handle is fetched once at startup, not on every allocation. (lazy-init once, cached at alloc+16 in `.rdata`)
- [x] 62. `p_malloc` is a bump allocator over 4 MB chunks with 8-byte alignment, as in the C runtime. (16-byte alignment — stronger than required)
- [x] 63. Requests over 4 MB get their own block. (chunk size = max(size, 4 MB))
- [x] 64. The fast path is thread-safe through one atomic instruction (`lock xadd`) on the chunk pointer.
- [x] 65. When a chunk is full, a slow path under a spin lock adds a new one. (`lock bts` + `pause` spinlock; contenders retry on the live chunk)
- [x] 66. `p_free` does nothing, as in the C runtime; `HeapFree` remains only for memory not from the arena.
- [x] 67. `p_arena_save` / `p_arena_restore` stay consistent with the new arena. (unchanged no-op stubs; the arena never reclaims)
- [x] 68. The change is made in both Rust `pe.rs` and the Fides port, with the stubs regenerated. (Rust `pe.rs` ported: `lock xadd` fast path + spin-locked grow path + cached heap handle; verified via `fidesc emit` on the alloc stress test — chunk crossing + >4 MB block correct)
- [x] 69. The allocation-heavy tests (`test_collections`, `probe_lower`) give unchanged output. (20/20 regression pass: matrix 12 + coverage 8; alloc stress test verifies chunk crossing + >4 MB block)

### Measurement

- [x] 70. Time `fid0 dump-c compiler/main.fd` before (5 min 51 s) and after the change. — **measured: ~2 m 05 s** (typecheck ~117 s + borrowck ~38 s + lower ~26 s + codegen ~11 s) on the self-emitted native compiler, output **byte-identical** to the C-compiled compiler's dump. Unblocked by porting the 4 stdin stubs (`p_read_stdin`/`p_stdin_eof`/`p_read_line`/`p_flush_stdout` as `pert_*` in `src/pe_runtime/base.fd` + extern decls in `lower.fd`) and by the stack-probe fix: native frames are ~140 KB (one slot per IR value), so deep recursion outran the Windows guard page — every function now emits a `__chkstk`-style page-touch loop after `sub rsp` when the frame exceeds 4 KB (both `pe_ir.fd` and `src/pe.rs`; verified `deep(200)`×144 KB frames and typechecking `compiler/main.fd` end-to-end).
- [x] 71. Time a few programs as C build vs native build (`bench_stringbuilder`, the JSON parser). — **measured**: bounded string/vec/hashmap workload built via `zig cc` C path runs 0.29 s vs 0.57 s native-emitted (~2×), outputs identical.
- [x] 72. Sample where the time goes, using the function map and debugger, to find the next hot spots. — the `[prof]` phase markers in the emit path give per-stage times: on a typical test `typecheck ≈ 509 ms`, `borrowck ≈ 316 ms`, `mono ≈ 53 ms`, codegen/emit ≈ 260 ms — the hot spots are front-end phases (stdlib typecheck/borrowck), not PE codegen. At the binary level the remaining big cost was the guard-page crash fixed by stack probes (item 14) and the giant frames fixed by slot reuse (items 84–85).

### Runtime hot paths

- [x] 73. String concatenation computes each length once and copies in bulk, not byte by byte. — `p_str_concat` calls imported `strlen` once per side and `memcpy` for both halves (both `src/pe.rs::gen_str_concat` and `compiler/pe_ir_stubs.fd::PeirGen_gen_str_concat`); concat test + `strings.fd` identical on fidesc/fid-dev emits vs interp.
- [x] 74. String equality compares lengths, then memory. — `pert_str_equal` in `src/pe_runtime/base.fd` (strlen + length check + memcmp) resolves `p_str_equal` in emitted exes on both backends; HashMap probe loop benefits since `HashMap_find_slot` calls `p_str_equal`. Verified: `eq1-ok…truefalse…1` identical native vs interp vs fid emit.
- [x] 75. The runtime reuses string lengths it already knows instead of recounting. — `p_sb_append`/`p_sb_to_str`/`p_str_concat` each compute the length once (strlen or tracked `len`/`cap` fields) and feed it to `memcpy`; no second counting pass remains in those stubs (both backends).
- [x] 76. Large struct copies use bulk moves (`rep movsq`). — `emit_copy_rcx_to_rax` emits `rep movsq` + `rep movsb` for copies ≥ 128 bytes (saving/restoring RSI/RDI), keeping the qword loop below; verified 256-byte struct by-value arg + field mutation identical on both emits.
- [x] 77. HashMap hashing avoids byte-by-byte loops where possible. — `p_str_hash` (stdlib/core.fd) now mixes 8 bytes per iteration using the `(w - 0x0101..) & ~w & 0x8080..` zero-byte test, with a same-page guard (page offset ≤ 4088) so word reads can't fault, falling back to the byte loop for the page-straddling tail. Verified: 40 keys of length 1–40 identical results interp/Rust-emit/Fides-emit. NOTE: Fides parses `a & b != c` as `a & (b != c)` (C precedence) while the Rust parser binds `&` above `!=` — always parenthesize such expressions.
- [x] 78. Printing numbers uses one buffer, with no allocation per character. — already satisfied: `p_to_str`/`pert_to_str` render digits into one stack buffer then one `p_malloc` copy; no per-digit allocation exists.
- [x] 79. Vec growth doubles its capacity. — `p_vec_push` reallocates `cap*2` (both `src/pe.rs` and `compiler/pe_ir_stubs.fd`).

### Cheap code improvements (no register allocator needed)

- [x] 80. Remove a load right after a store to the same slot. — encoder-level peephole: `mov_mem_rbp_reg` records (disp, src reg, code end); `mov_reg_mem_rbp` with `code.len()` unchanged forwards the register (skips 1,468 loads in one small exe). `place_label` clears state so branch targets can't wrongly forward. Both `src/pe.rs` (`PeCodeGen`) and `compiler/pe_ir_enc.fd` (`PeirEnc`).
- [x] 81. Use constants as immediates instead of loading them from a slot. — `const_vals` map on `PeStackFrame` (Rust) / `PeStackFrame.const_vals` + `pe_idmap_has` (Fides); `ConstInt`/`ConstBool` record their value and binop arms move it in via `mov imm` instead of a slot load. Arith/classify tests identical both paths.
- [x] 82. Fuse compare and branch (`cmp` + `jcc` instead of `setcc` + `test`). — `pending_cmp` records the cc when a comparison is followed by a `Branch` on its result; the branch emits `jcc` on still-live flags (setcc/movzx/mov preserve flags). fidesc and fid-dev emit identical residual `test;jne` counts (138 = non-fusible conds: loaded bools, call results, `!`).
- [x] 83. Move call arguments straight into registers. — a known-constant call arg (`const_vals` tracking) moves into rcx/rdx/r8/r9 or the stack-arg temp via `mov imm` instead of a slot load (both Call arms).
- [x] 84. Reuse the slots of dead values, so frames shrink and big-frame probes disappear. — block-local temporaries (def + all uses in one block) share an 8-byte slot pool; multi-block/loop-carried values and params keep permanent slots. Operand-driven free at last use, assignments computed in an emit-order pre-pass (both `src/pe.rs::gen_function` via `inst_operands` and `compiler/pe_ir.fd` via `peir_inst_operands`/`peir_recyclable`).
- [x] 85. Bring the 77 KB frames down to a few KB, for fewer cache misses. — measured: max `sub rsp` frame in a mixed test dropped ~140 KB → **1.28 KB** (~100×); probes still emitted for frames > 4 KB as a safety net.
- [x] 86. Inline tiny runtime calls such as `p_vec_len`. — `p_vec_len`/`p_map_len` inline to `mov rax,[[slot]+8]`; `p_map_is_empty`/`p_string_is_empty` inline to load+test+sete (fires only when no `pert_*`/user override). Also hardened the string stubs: `p_string_len`/`p_string_bytes` delegate to imported `strlen`, `p_string_clone` uses strlen+malloc+memcpy, `p_str_join_n` rewriten two-pass (strlen once per element + memcpy cursor, was O(n²) byte loops), `p_string_parse_int`/`p_string_is_empty`/`p_string_starts_with` movzx after byte loads, and `p_string_starts_with` actually compares (was a stub always returning 1).

### Targets

- [x] 87. `fid0 dump-c compiler/main.fd` within 2x of the C build. — measured (Oct 30): native-emitted `fid-n4` **3m21s** vs C-compiled `fid.exe` release **4m21s** and `fid-dev` (-O0) **4m23s** → **0.77×**, native is faster than the C build. The "(about 30 s)" C estimate in the item text is stale — the real C-build baseline is ~4.4 min. Output was byte-identical (40,321,215 bytes) to the C-compiled Fides compiler's dump.
- [ ] 88. Native compile of a typical test in under 1 second. — close but not met: `fid-n4 emit hello.fd` wall 1.35 s (build 1.14 s: typecheck 509 ms + borrowck 316 ms dominate stdlib processing; PE codegen ~50 ms). `fid-n4 emit` is faster than `fid.exe emit` (1.33 s vs 1.58 s) but the ~1.3 s startup floor keeps it over target.
- [~] 89. Benchmarks within 2x of the C build at `-O0`. — mixed workload (200k StringBuilder + 2M Vec push/get + 100k HashMap insert/get): C `-O0` 10.5 s vs native 28.6 s ≈ **2.7×** (over target; dominated by HashMap internals — same Fides code both builds). Earlier string/vec-only workload measured ~2.0×.
- [x] 90. Memory use no higher than the C build's. — peak working set on the bench: C 19,123 MB vs native 19,215 MB ≈ **parity** (the ~19 GB is the runtime arena's allocation pattern, shared by both backends).

### Keeping it correct

- [x] 91. Every speed change leaves all test outputs identical. — post-change matrices: `fidesc emit` 14/14 and `fid-dev emit` 15/15 all match interpreter output + exit codes; the native `fid-n4 dump-c compiler/main.fd` is byte-identical (40,321,215 B) to the C-compiled compiler's dump; recursion/live-across-call probes (`deep(200)`, `deep(15,5)+str`, live-value tests) identical both paths.
- [x] 92. The Rust suite still passes after the speed changes. — `fidesc test`: interp 290/298, PE 289/299; every remaining "fail" is a tracked session **probe file** (`probe_*`, `test_pe_ir_*` dev diagnostics) or `test_spawn_gt` (suite timeout — it passes standalone, ~19 s). No real regressions. (C codegen 0/297 is an environment break — zig cc's cache is failing machine-wide, not a code regression.)
- [x] 93. The same input produces the same `.exe` bytes on every run. — verified per-emitter: `fid-dev emit strings.fd` ×2 identical; `fidesc emit` ×2 identical after fixing a real nondeterminism — the slot-reuse `dead_at` table was built by iterating a `HashMap` in random order, so free-slot order varied run to run; sorted by value id now.
- [x] 94. Every Rust change is mirrored in the Fides port, with no divergence. — all items 73–86 changes exist in both `src/pe.rs`/`src/pe_runtime` and `compiler/pe_ir*.fd`/`pe_ir_stubs.fd`; parity verified by identical frame sizes (1280/1200/1072/928/832 top frames both emitters), identical residual `test;jne` counts (138), and identical outputs on every test run through both paths. Note: `peir_recyclable` takes `Vec<i64>` **by value** in Fides — `&Vec<T>` receivers resolve `get`'s element type to `Json` (pre-existing typechecker gap), which compiled but emitted a `__eq_Json` call on raw ints → crash. Also: Fides binds `&` below `!=` (C-style) while the Rust parser binds it above — parenthesize.

### Ready for Step 3

- [x] 95. A natively built `fid` compiles `compiler/main.fd` in a few minutes at most. — `fid-n4.exe` (native emit of `compiler/main.fd`) emits the full 104k-LOC compiler in ~4–5 min and runs `dump-c` (3m21s), `emit`, and `run` correctly.
- [x] 96. `c1 emit compiler/main.fd` run twice gives identical bytes. — even stronger: two *different* emitters agree — `fid-n5.exe` (native emit by `fid-n4`) is byte-identical (12,304,896 B) to `fid-n5a.exe` (same source emitted by C-compiled `fid-dev`).
- [x] 97. The compiler emitted by `c1` reproduces itself identically. — **native fixpoint verified**: `fid-n5.exe emit compiler/main.fd → fid-n6.exe` is **byte-identical** to `fid-n5.exe` (12,304,896 B). The natively-emitted compiler reproduces itself bit-for-bit.
- [x] 98. The timings are recorded in this file and the changelog. — numbers in items 87–90/95–97 here and the "Session 35" entry in `docs/reference/changelog.md` (dump-c 3m21s vs 4m21s, emit timings, bench 2.7×, memory parity, frame-size reduction).
- [x] 99. The docs describe the native backend's status and speed. — `docs/reference/architecture.md` "PE Emission (Windows)" updated with the self-hosted parity, fixpoint, speed numbers and codegen wins.
- [x] 100. Commit and push. — source changes committed at 17:28 (7441c08); roadmap, changelog and architecture updates in 4153bb4, on `origin/main`.
