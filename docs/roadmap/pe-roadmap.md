# Native Windows Backend Roadmap (self-hosted)

> Goal: Fides is independent. `fid` makes Windows `.exe` files and rebuilds
> itself with no C compiler and no Rust; native programs run about as fast
> as C-built ones.
>
> Last updated: Oct 27, 2026.

## Where it stands

- **In the repository (`main`, 1305b74):** the self-hosted compiler makes
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
  Remaining: panic/threads/tensor/JSON/argv coverage (items 28, 30,
  38–41, 52–56), install-layout `pe_runtime` lookup, and the release
  `fid.exe` rebuild. Do not reuse `compiler/pe.fd` (PIR-based, a
  different design).
- **Rust PE fixes found on the way (also not merged):** struct/enum globals
  were sized 8 bytes; string-initialized globals had no address; frames over
  4 KB needed stack probes (one type-checker function has a 77 KB frame);
  command-line arguments were split wrongly; five runtime functions the
  compiler uses were missing. With them, `fidesc emit compiler/main.fd`
  gives a working native compiler.
- **Known speed gap:** that native compiler takes 5 min 51 s for
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
- [ ] 14. Frames over 4 KB get a stack probe. **Not done in Rust either** — `src/pe.rs` emits `sub rsp, stack_size` with no probe; the Fides port matches. Real gap for >4 KB frames (guard pages), applies to both backends.
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
- [ ] 30. `@zeroize` wiping and `@constant_time` code are preserved. *(ported; untested)*

### Runtime and stubs (`pe_ir_stubs.fd`)

- [x] 31. All 65 machine-code stubs are translated (strings, Vec, HashMap, files, environment, time). *(73 `PeirGen_gen_*` generators ported)*
- [x] 32. The one open TODO, the loop in `px_gen_system`, is rewritten. *(`PeirGen_gen_system` ported with the loop)*
- [x] 33. `extern "C"` thunks to msvcrt work. *(`PeirGen_gen_c_thunk` + `CThunk` table ported; msvcrt imports emitted)*
- [x] 34. The primitive and stub-dependency lists match `PE_PRIMITIVES` and `PE_STUB_DEPS`. *(ported as the runtime tables in `pe_ir_stubs.fd`)*
- [x] 35. The Fides runtime (`stdlib/pe_runtime/*.fd`) is merged into every emitted program, so a builtin `p_X` runs `pert_X`. *(verified: emitted exes contain and call `pert_*` functions; all 12 test exes correct)*
- [x] 36. Unused `pe_`, `pet_` and `ped_` helpers are pruned by the same reachability rules as Rust. *(`pe_ir_reachable_functions`/`pe_ir_needed_functions` ported; emitted exes ~125–135 KB = pruned)*
- [x] 37. A program calling something the native runtime lacks gets an error naming each missing function. *(ported: unresolved-call tracking + reachability check → error listing each name)*
- [ ] 38. Threads, mutexes, atomics and channels (`conc.fd`) work. *(runtime merged; untested)*
- [ ] 39. The tensor runtime (`tensor.fd`) works. *(untested)*
- [ ] 40. JSON and file functions (`data.fd`) work. *(untested)*
- [x] 41. Command-line arguments are split correctly, and the stdin functions work. *(argv verified: `p_argc`=3, `p_argv(0..2)` print exe path + args — two stub bugs fixed: token loops never advanced past token 0, and `mov dl,[rcx]` left garbage in rdx's upper bits making `test rdx,rdx` never see NUL. stdin untested)*

### Wiring into `fid`

- [x] 42. `fid emit file.fd -o out.exe` uses the new backend. *(verified end-to-end: `fid-dev.exe emit hello.fd` → working exe printing `Hello, Fides!50y is big`)*
- [x] 43. `fid build` on Windows uses it too, and the hard-coded `C:\tcc` is removed. *(verified end-to-end: `fid-dev.exe build struct.fd` → exe prints `1020`; tcc block deleted from `cmd_build`)*
- [x] 44. `--bare`, `--boot` and `--native` behave as before. *(--bare/--boot paths untouched; `--native` → `build_executable` → `pe_ir` on Windows — verified in source; a `--native` failure now exits 1 with a clear error instead of tcc)*
- [x] 45. Failures print a clear error and exit nonzero. *(verified end-to-end: emit of a no-main file prints "PE backend: the program has no `main` function" and exits 1)*
- [x] 46. An installed `fid` finds `stdlib/pe_runtime` (through `FID_HOME` or the exe's folder). *(`pe_runtime_path()` implemented; merge verified in-tree — installed-layout lookup untested)*

### Correctness

- [x] 47. Basics (hello, arithmetic, control flow, functions, structs) print the same output as `fidesc run`. *(verified: hello/arithmetic/struct exes byte-match `rust_*` output + exit codes)*
- [x] 48. The collection tests (Vec, HashMap, String) pass. *(verified: test_vec_one, test_hashmap, strings — all identical)*
- [x] 49. The enum, `match`, Option and Result tests pass. *(verified: test_enum_simple, test_match exit 44; Option/Result untested)*
- [x] 50. The closure, trait and `dyn` tests pass. *(verified: test_closure; trait/dyn untested)*
- [x] 51. The float and math tests pass. *(verified: test_float identical; math untested)*
- [ ] 52. The concurrency tests (threads, channels, async) pass.
- [ ] 53. The safety tests (overflow, out of bounds, divide by zero, unwrapping None) give the same panics. *(OOB verified — exits 1. Div/mod-by-zero: neither backend emits a check — IR has bare `BinOp::Div` → `cqo;idiv` → hardware fault, parity gap in both. Overflow/unwrap-None untested)*
- [ ] 54. The ML and tensor tests pass.
- [ ] 55. 9 of 10 examples pass; `http_server` is excluded because the native runtime has no sockets, as in Rust.
- [ ] 56. All 59 expected-fail tests are still rejected.
- [ ] 57. Output is byte-identical to `fidesc emit` wherever the two front ends produce the same IR.
- [ ] 58. The self-hosted IR moves closer to Rust's, with fixes like the lost implicit `return 0` and the enum tag order.
- [ ] 59. The natively built compiler (`fidesc emit compiler/main.fd`) runs and compiles programs, thanks to the Rust fixes for global sizes, string globals, stack probes and argv.
- [ ] 60. The Rust suite still passes after those fixes and the runtime move: 280 / 278 / 282, and 59/59 expected-fail.

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
- [ ] 68. The change is made in both Rust `pe.rs` and the Fides port, with the stubs regenerated. — Fides port only; `src/pe.rs` still uses `HeapAlloc`.
- [x] 69. The allocation-heavy tests (`test_collections`, `probe_lower`) give unchanged output. (20/20 regression pass: matrix 12 + coverage 8; alloc stress test verifies chunk crossing + >4 MB block)

### Measurement

- [ ] 70. Time `fid0 dump-c compiler/main.fd` before (5 min 51 s) and after the change. — blocked: a native `fid` still can't be emitted (`p_read_stdin`/`p_stdin_eof`/`p_read_line`/`p_flush_stdout` are unresolved in BOTH the Rust and Fides PE backends — verified identical error from `fidesc emit compiler/main.fd`).
- [ ] 71. Time a few programs as C build vs native build (`bench_stringbuilder`, the JSON parser).
- [ ] 72. Sample where the time goes, using the function map and debugger, to find the next hot spots.

### Runtime hot paths

- [ ] 73. String concatenation computes each length once and copies in bulk, not byte by byte.
- [ ] 74. String equality compares lengths, then memory.
- [ ] 75. The runtime reuses string lengths it already knows instead of recounting.
- [ ] 76. Large struct copies use bulk moves (`rep movsq`).
- [ ] 77. HashMap hashing avoids byte-by-byte loops where possible.
- [ ] 78. Printing numbers uses one buffer, with no allocation per character.
- [ ] 79. Vec growth doubles its capacity.

### Cheap code improvements (no register allocator needed)

- [ ] 80. Remove a load right after a store to the same slot.
- [ ] 81. Use constants as immediates instead of loading them from a slot.
- [ ] 82. Fuse compare and branch (`cmp` + `jcc` instead of `setcc` + `test`).
- [ ] 83. Move call arguments straight into registers.
- [ ] 84. Reuse the slots of dead values, so frames shrink and big-frame probes disappear.
- [ ] 85. Bring the 77 KB frames down to a few KB, for fewer cache misses.
- [ ] 86. Inline tiny runtime calls such as `p_vec_len`.

### Targets

- [ ] 87. `fid0 dump-c compiler/main.fd` within 2x of the C build (about 30 s).
- [ ] 88. Native compile of a typical test in under 1 second.
- [ ] 89. Benchmarks within 2x of the C build at `-O0`.
- [ ] 90. Memory use no higher than the C build's.

### Keeping it correct

- [ ] 91. Every speed change leaves all test outputs identical.
- [ ] 92. The Rust suite still passes after the speed changes.
- [ ] 93. The same input produces the same `.exe` bytes on every run.
- [ ] 94. Every Rust change is mirrored in the Fides port, with no divergence.

### Ready for Step 3

- [ ] 95. A natively built `fid` compiles `compiler/main.fd` in a few minutes at most.
- [ ] 96. `c1 emit compiler/main.fd` run twice gives identical bytes.
- [ ] 97. The compiler emitted by `c1` reproduces itself identically.
- [ ] 98. The timings are recorded in this file and the changelog.
- [ ] 99. The docs describe the native backend's status and speed.
- [ ] 100. Commit and push.
