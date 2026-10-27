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
- **Started, not merged:** a port of `src/pe.rs` (IR → machine code → PE,
  values in stack slots) to new modules `compiler/pe_ir_enc.fd` (encoder,
  imports, labels), `compiler/pe_ir_stubs.fd` (65 machine-code stubs,
  translated by script), `compiler/pe_ir.fd` (instruction lowering, PE
  writer), wired to `fid emit`/`fid build`. `hello.fd` emitted by it printed
  the same output as `fidesc`. That tree lived in a temporary scratch folder;
  if it is gone, restart from `src/pe.rs`. Do not reuse `compiler/pe.fd`
  (PIR-based, a different design).
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

- [ ] 1. Every x86 encoder helper (mov, add, sub, cmp, jcc, call, lea, push, pop, setcc, movsx, movzx) emits the same bytes as Rust's `X86Encoder`.
- [ ] 2. REX, ModRM and SIB encoding is correct for R12 and R13 (the old RSP/RBP-code bug is not reintroduced).
- [ ] 3. The SSE2 float instructions (movsd, addsd…, cvtsi2sd, cvttsd2si, ucomisd) are encoded.
- [ ] 4. Labels, forward jumps and relocations resolve to the same offsets as in Rust.
- [ ] 5. RIP-relative references into `.rdata` and `.data` resolve correctly.
- [ ] 6. The kernel32 and msvcrt import lists are identical to Rust's, and every name is a real export.
- [ ] 7. Import-table slot numbers (`PX_IAT_*`) follow the same order as Rust's.
- [ ] 8. The read-only data builder lays out strings, float constants and alignment identically.
- [ ] 9. The PE headers (DOS stub, COFF header, optional header, section table, alignments, image base `0x140000000`) match.
- [ ] 10. The stack reserve is large enough for the compiler's deep recursion.
- [ ] 11. The header contains no timestamp or anything else that varies, so output is deterministic.
- [ ] 12. The `.exe` is written as binary (`p_file_write_bin`), with no line-ending translation.

### Code generation (`pe_ir.fd`)

- [ ] 13. Prologue, epilogue and frame layout are identical to Rust's: values ordered by id, then alloca slots.
- [ ] 14. Frames over 4 KB get a stack probe.
- [ ] 15. The Win64 calling convention is followed: 4 register arguments, shadow space, stack arguments, 16-byte alignment.
- [ ] 16. Integer, float, bool and string constants are generated.
- [ ] 17. Integer arithmetic has overflow checks, and `@wrapping` turns them off.
- [ ] 18. Division and modulo panic on divide by zero.
- [ ] 19. Shifts mask their count; the bitwise operators are generated.
- [ ] 20. Comparisons and branches (`Br`, `CondBr`, `Switch`) are generated.
- [ ] 21. Float arithmetic, float comparisons and casts (int↔float, f32↔f64) are generated.
- [ ] 22. Loads and stores use the right width (i8, i16, i32, i64, f32, f64).
- [ ] 23. `Alloca`, `GetFieldPtr` with real struct offsets, `GetElementPtr` and `StructCopy` work.
- [ ] 24. Enum tags and payloads, and `match` over them, work.
- [ ] 25. Direct calls, indirect calls (`CallPtr`), closures with their environment, and `dyn` calls through vtables work.
- [ ] 26. Globals get their type's full size, and string-initialized globals get the string's address.
- [ ] 27. `print` works for every type: int, float, bool, str, Vec, HashMap, tensor.
- [ ] 28. A panic prints its message and line and exits with the right code.
- [ ] 29. Bounds and safety checks match the Rust backend.
- [ ] 30. `@zeroize` wiping and `@constant_time` code are preserved.

### Runtime and stubs (`pe_ir_stubs.fd`)

- [ ] 31. All 65 machine-code stubs are translated (strings, Vec, HashMap, files, environment, time).
- [ ] 32. The one open TODO, the loop in `px_gen_system`, is rewritten.
- [ ] 33. `extern "C"` thunks to msvcrt work.
- [ ] 34. The primitive and stub-dependency lists match `PE_PRIMITIVES` and `PE_STUB_DEPS`.
- [ ] 35. The Fides runtime (`stdlib/pe_runtime/*.fd`) is merged into every emitted program, so a builtin `p_X` runs `pert_X`.
- [ ] 36. Unused `pe_`, `pet_` and `ped_` helpers are pruned by the same reachability rules as Rust.
- [ ] 37. A program calling something the native runtime lacks gets an error naming each missing function.
- [ ] 38. Threads, mutexes, atomics and channels (`conc.fd`) work.
- [ ] 39. The tensor runtime (`tensor.fd`) works.
- [ ] 40. JSON and file functions (`data.fd`) work.
- [ ] 41. Command-line arguments are split correctly, and the stdin functions work.

### Wiring into `fid`

- [ ] 42. `fid emit file.fd -o out.exe` uses the new backend.
- [ ] 43. `fid build` on Windows uses it too, and the hard-coded `C:\tcc` is removed.
- [ ] 44. `--bare`, `--boot` and `--native` behave as before.
- [ ] 45. Failures print a clear error and exit nonzero.
- [ ] 46. An installed `fid` finds `stdlib/pe_runtime` (through `FID_HOME` or the exe's folder).

### Correctness

- [ ] 47. Basics (hello, arithmetic, control flow, functions, structs) print the same output as `fidesc run`.
- [ ] 48. The collection tests (Vec, HashMap, String) pass.
- [ ] 49. The enum, `match`, Option and Result tests pass.
- [ ] 50. The closure, trait and `dyn` tests pass.
- [ ] 51. The float and math tests pass.
- [ ] 52. The concurrency tests (threads, channels, async) pass.
- [ ] 53. The safety tests (overflow, out of bounds, divide by zero, unwrapping None) give the same panics.
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

- [ ] 61. The heap handle is fetched once at startup, not on every allocation.
- [ ] 62. `p_malloc` is a bump allocator over 4 MB chunks with 8-byte alignment, as in the C runtime.
- [ ] 63. Requests over 4 MB get their own block.
- [ ] 64. The fast path is thread-safe through one atomic instruction (`lock xadd`) on the chunk pointer.
- [ ] 65. When a chunk is full, a slow path under a spin lock adds a new one.
- [ ] 66. `p_free` does nothing, as in the C runtime; `HeapFree` remains only for memory not from the arena.
- [ ] 67. `p_arena_save` / `p_arena_restore` stay consistent with the new arena.
- [ ] 68. The change is made in both Rust `pe.rs` and the Fides port, with the stubs regenerated.
- [ ] 69. The allocation-heavy tests (`test_collections`, `probe_lower`) give unchanged output.

### Measurement

- [ ] 70. Time `fid0 dump-c compiler/main.fd` before (5 min 51 s) and after the change.
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
