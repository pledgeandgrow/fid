# File analysis — progress tracker

One doc per `compiler/*.fd` module: `file-analysis/<name>.md` gives the
path, size and role, where the module is called from and which commands
reach it, and its findings with their status (FIXED / STILL TRUE / CHANGED /
NEW) and line numbers. **All 68 modules were re-audited on 2026-11-09**
against the code of that day (several files were being edited by other
sessions; their docs say so). The open bugs that audit found are collected
in `docs/reference/known-bugs.md` §2g. This table replaces the 2026-10-22/23
one; that pass's record follows the template below.

> **Nov 21 update:** `compiler/` has 80 modules (361,858 lines, `wc -l
> compiler/*.fd`; 360,657 at HEAD, 9 files hold +1,201 uncommitted lines:
> compiler, comptime_eval, iropt, iropt_me, lower, nat_enc, nat_ir, pe_ir,
> pe_ir_stubs); 56 are reached from main.fd through `use`, 24 are not.
> The 11 modules added since the audit have a page now (rows 69-79). Every
> page's line count is of Nov 21, and each page has a dated note on what
> changed since Nov 9: fixes (known-bugs 2g.1-2g.78), the new open bugs of
> the Nov 21 check (2g.79-2g.113) and the retirement of the PIR path
> (MI18, Nov 17; docs/pir-retired.md), which made pir, opt, regalloc,
> peephole, codegen_x64/arm64/riscv, coff, macho, pe, the PIR runtimes and
> assemblers unreachable. The findings below each note were not all
> re-checked; their line numbers are of the day the page was written. The
> State column is of Nov 21; the headlines keep the Nov 9 finding where it
> still holds.
>
> **Nov 21, uncommitted:** the 9 working-tree diffs fix known-bugs 2g.80,
> 2g.81, 2g.88, 2g.89, 2g.94-2g.96, 2g.98, 2g.99, 2g.106, 2g.109 and 2g.110,
> harden comptime evaluation (fatal errors, fuel, traces, checked
> arithmetic, lowerer-computed layouts) and cache the FID_NAT_* switches
> once per image (RA14). The affected pages carry a "Nov 21, uncommitted"
> note; whether the suite agrees is not re-checked here.

| # | File | State (Nov 21) | Headline |
|---|------|----------------|----------|
| 1 | [ark.fd](ark.md) | live | `fid ark`, now 7,169 lines with a registry (the pacts index). Directory pacts, file pacts and the empty lock no longer reproduce (2g.63); `update <name>`, git pacts and the lock contents not re-checked |
| 2 | [arm64_enc.fd](arm64_enc.md) | dead | No command reaches it. Earlier fp/mov sp/LSL/rodata bugs fixed; new: 64-bit `asr #imm` encoded as LSR |
| 3 | [ast.fd](ast.md) | live | 44 importers (34 reached). Unused `use string`, duplicate helpers, silent fallback values |
| 4 | [autograd.fd](autograd.md) | dead | No importer; no-op shim. Header gradient table wrong for mul/matmul |
| 5 | [borrowck.fd](borrowck.md) | live | `--borrowck=warn`, casts and nested field writes fixed Nov 15 (2g.9); `swap(&mut v[0], &mut v[1])` rejected (2g.73) |
| 6 | [checks.fd](checks.md) | live | Counted loops, ranges, `Vec::filled` bounds. The asm clobber crash is fixed (2g.53); ownership calls defeat the loop proof |
| 7 | [codegen.fd](codegen.md) | dead | Nothing imports it since MI18; no longer compiled into `fid` |
| 8 | [codegen_arm64.fd](codegen_arm64.md) | dead | Unreached since MI18 (Nov 17): `codegen --target arm64` is the machine layer |
| 9 | [codegen_c.fd](codegen_c.md) | live | `f32 %`, float bit casts and libc-name clashes fixed (2g.13); open: `byte` vs rpcndr.h (2g.69), extern names equal to core's (2g.107), unbounded thread tables |
| 10 | [codegen_riscv.fd](codegen_riscv.md) | dead | Unreached since MI18: `codegen --target riscv` is the machine layer |
| 11 | [codegen_x64.fd](codegen_x64.md) | dead | Unreached since MI18 (only pe.fd imports it) |
| 12 | [coff.fd](coff.md) | dead | Unreached since MI18; `codegen --coff` goes through nat_enc and fails on every program (2g.110) |
| 13 | [compiler.fd](compiler.md) | live | Pipeline, 13,468 lines. warn mode and `@cfg(debug)` fixed (2g.9, 2g.14); unconditional `[build]`/`[prof]` stderr; `ccompile --fuzz-cov` double free (2g.83) |
| 14 | [comptime_eval.fd](comptime_eval.md) | live | `if` assignments in a `comptime fn` kept and errors returned (2g.19); impl-method substitution not re-checked |
| 15 | [cowcheck.fd](cowcheck.md) | partly live | Report opt-in (`FIDES_COW_CHECK`), premise stale; walkers live in lowering |
| 16 | [cparser.fd](cparser.md) | live | `parse-header`. `#if`/`#ifdef` evaluated where known (LG15); parameters keep their C type (2g.62); `long` always i32 |
| 17 | [csplit.fd](csplit.md) | live | `split-c`, used by the release build. `static` stripped from the function head only (2g.1 fixed) |
| 18 | [debugger.fd](debugger.md) | dead | No `debug` command (`fid replay` exists, CT7, without this module); `str_find_char` gets a string, commands never split |
| 19 | [diag.fd](diag.md) | dead | Not wired; passes `fid check` again (2g.22); fid prints no error codes |
| 20 | [egraph.fd](egraph.md) | live | Machine-layer path and `emit-ir --mid-end`. Wrapping reassociation in checked code; quadratic `!!b` scan (not re-checked) |
| 21 | [elf64.fd](elf64.md) | partly live | `build-x86` only. 64-byte section headers through image_writer (2g.16 fixed, TG13) |
| 22 | [embedded.fd](embedded.md) | dead | No importer (dead in Rust too); unchecked functions reported Safe(0) |
| 23 | [escape.fd](escape.md) | live | Solid on probes; a ref passed to a `p_*` builtin is unchecked |
| 24 | [flat_ast.fd](flat_ast.md) | dead | Not imported; `dump-ast-flat` prints the summary; passes `fid check` again (2g.22) |
| 25 | [fmt.fd](fmt.md) | live | `a<b`, `>>` and prefix-operator corruption fixed (2g.6, 2g.62); refuses to write when tokens would change |
| 26 | [gpu.fd](gpu.md) | partly live | Rewritten from the IR (2g.21); SPIR-V has no structured control flow; calls, memory, floats not translated |
| 27 | [green_thread.fd](green_thread.md) | partly live | Interpreter only. A thread that yields twice blocks the next (2g.20) |
| 28 | [hotreload.fd](hotreload.md) | dead | `fid watch` runs bench; missing builtins |
| 29 | [inc.fd](inc.md) | live | Stamps, `.fidcache`, core stamp. Key still omits FID_VERIFY/FID_RULES_ON/FID_NAT_ON/FID_LIBFACTS* (2g.15); function cache not byte-identical (2g.82); seeded store, IR units (2g.91, 2g.92) |
| 30 | [interp.fd](interp.md) | live | `fid run`. Tasks interleave (LG11); printf unformatted (2g.20); `str` params (2g.45), OOM (2g.49), u64 printed signed (2g.52) |
| 31 | [ir.fd](ir.md) | live | Indexes, reachability, the pe_ir name helpers. `IrModule_get_function` does not check the name |
| 32 | [iropt.fd](iropt.md) | live | Mid-end, 11,143 lines. SCCP and NaN branchfacts fixed (2g.11, 2g.12); SCCP overflow on the compiler (2g.89); memo mismatches (2g.93) |
| 33 | [iropt_me.fd](iropt_me.md) | live | 15 passes on by default (`IME_DEFAULT_ON`, ssa among them). Unroll with a dead store on pe_ir (2g.80), sra (2g.94) |
| 34 | [irverify.fd](irverify.md) | live, opt-in | `FID_VERIFY=1`; skipped on a stamp hit; rejects two suite programs after iropt (2g.109) |
| 35 | [json.fd](json.md) | live | Malformed numbers accepted; surrogate pairs not combined |
| 36 | [lexer.fd](lexer.md) | live | `5e3` fixed; an unknown character is a `BadChar` error token (2g.18) |
| 37 | [linker.fd](linker.md) | partly live | `build-x86` only; header cause fixed (TG13), exe not run since; missing entry not an error |
| 38 | [lower.fd](lower.md) | live | Ownership 0004, 27,878 lines. 2g.2, 2g.3 and the clobber crash fixed; function-pointer struct results (2g.47), global u64 (2g.72) |
| 39 | [lsp.fd](lsp.md) | live | Double free and line-0 diagnostics fixed (2g.5); references, signatureHelp, inlayHint, codeAction, formatting added |
| 40 | [macho.fd](macho.md) | dead | Unreached since MI18; `codegen --macho` and macOS executables come from nat_enc |
| 41 | [main.fd](main.md) | live | 55 command names. `build --native` is the machine layer (MI18); POSIX `build` too (CB4); `run --timeout` not enforced |
| 42 | [mono.fd](mono.md) | live | Nested generic calls renamed (2g.8); no turbofish on the no-std path; `Vec.map` type change (2g.66), packs (2g.84), trait defaults (2g.97) |
| 43 | [nat_enc.fd](nat_enc.md) | live | Encoders and writers (through image_writer). Linux code R+X, data RW (TG14); DWARF/CFI (2g.102, 2g.103), COFF objects (2g.110) |
| 44 | [nat_ir.fd](nat_ir.md) | live | Machine layer, 40,799 lines: Linux, macOS, `--nat`, Cortex-M (nat_thumb). Page out of date; 2g.82, 2g.88, 2g.98, 2g.99 |
| 45 | [native_debugger.fd](native_debugger.md) | dead | No ptrace builtins |
| 46 | [opt.fd](opt.md) | dead | Unreached since MI18; `fid opt` prints the IR after iropt. Jump-threading fix (2g.17) never run |
| 47 | [parser.fd](parser.md) | live | Imported struct literals fixed (2g.38). Dead `parse_expr_iterative`; module globals never reset |
| 48 | [pe.fd](pe.md) | dead | Unreached; `emit --boot` goes through pe_ir (`PEIR_BARE_BOOT`) |
| 49 | [pe_ir.fd](pe_ir.md) | live | Default Windows backend. Concurrency refusal not re-checked; `--leaks` exes crash (2g.81), f32 cast (2g.95), tail calls (2g.98), `-g` locals (2g.104) |
| 50 | [pe_ir_enc.fd](pe_ir_enc.md) | live | Import directory size fixed at 20; no ASLR; writable `.rdata` (still true) |
| 51 | [pe_ir_stubs.fd](pe_ir_stubs.md) | live | Old machine `p_vec_*` stubs still in the stub list; name helpers moved to ir.fd |
| 52 | [peephole.fd](peephole.md) | dead | Unreached since MI18 (only pe.fd imports it) |
| 53 | [pir.fd](pir.md) | dead | Unreached since MI18; `fid pir`/`dump-pir` print where the views went |
| 53b | [reach.fd](reach.md) | live (emit only) | Which functions `emit` lowers; `drop` methods are roots (2g.23); `--lib --export` callees not lowered (2g.85) |
| 54 | [regalloc.fd](regalloc.md) | dead | Unreached since MI18 (only codegen_x64 imports it) |
| 55 | [riscv_enc.fd](riscv_enc.md) | dead | No command reaches it; no rodata in its result |
| 56 | [rules.fd](rules.md) | live | Machine-layer path, `test-rules`. Read-only call merging on by default (Nov 17) after the loop-write fix; facts format mismatch |
| 57 | [runtime.fd](runtime.md) | dead | Type error fixed in the source (2g.22); replaced by stdlib/linux_runtime |
| 58 | [runtime_arm64.fd](runtime_arm64.md) | dead | Assembler errors return empty parts |
| 59 | [runtime_linux.fd](runtime_linux.md) | dead | `build_executable` gone (MI18); layouts predate 0004 |
| 60 | [runtime_riscv.fd](runtime_riscv.md) | dead | As runtime_arm64 |
| 61 | [scribe.fd](scribe.md) | live | `fid doc`. CRLF names, `::` file names, missing folders fixed in the source (AD1, TL11) |
| 62 | [secrecy.fd](secrecy.md) | live | Guards and file-write sinks fixed (2g.10); `use crypto` fails @secret on native emit (2g.71) |
| 63 | [tensor.fd](tensor.md) | live | Interpreter only. 12-term exp; no per-dimension bounds |
| 64 | [token.fd](token.md) | live | 686 lines, 140 variants (`BadChar` new); keyword range check depends on declaration order |
| 65 | [typechecker.fd](typechecker.md) | live | Guards, generic enums, generic structs fixed (2g.7, 2g.28); unknown bound traits accepted; 2g.54-56, 2g.74-76, 2g.112 |
| 66 | [veil.fd](veil.md) | live | `fid veil` double free fixed (2g.2); the interpreter records coverage now; `test --coverage` fix of Nov 18 (2g.30) |
| 67 | [wasm.fd](wasm.md) | live | Rewritten Nov 15: validates, 316 of 344 suite tests match `fid run`; struct through a function value (2g.108) |
| 68 | [x86.fd](x86.md) | partly live | `build-x86`; the elf64 header cause is fixed, the exe not run since |
| 69 | [ast_ser.fd](ast_ser.md) | live | New. AST codec: checked-library artifact (`FID_ASTCACHE`) and import cache (`FID_IMPCACHE`); unused entry points, stale header |
| 70 | [build_info.fd](build_info.md) | live | New. `fid version` (1.0.0+dev), release stamp written by scripts/build-info.sh |
| 71 | [disasm.fd](disasm.md) | live | New. `fid disasm`, table-driven; x86-64 round trip fails on `shl r, 1`; nat_enc does not use the table's words |
| 72 | [image_writer.fd](image_writer.md) | live | New. One writer for ELF, PE/COFF, Mach-O (TG13); stale header names `build_executable` |
| 73 | [ir_ser.fd](ir_ser.md) | live | New. IR codec: lowered-IR cache, mid-end and iropt memos, `.fir` files, `ir-fuzz`; memo mismatches (2g.93) |
| 74 | [mir.fd](mir.md) | live | New. The machine IR (MI1): `MInst`, liveness, verifier, block parameters (opt-in) |
| 75 | [nat_thumb.fd](nat_thumb.md) | live | New. Cortex-M Thumb-2 (PL5), thumbv7m-none/linux; no floats; no-OS images not run |
| 76 | [pdb.fd](pdb.md) | live | New. PDB for `emit -g` (pe_ir, `--nat`), read by dbghelp; no variables |
| 77 | [pgo.fd](pgo.md) | live | New. `run --profile` / `--profile-use` (OP30), `--trace` list |
| 78 | [ssa.fd](ssa.md) | live | New. SSA round trip in the mid-end (ME3), on by default; 2% more instructions than promote on one test |
| 79 | [tv.fd](tv.md) | live, opt-in | New. Translation validation (`--validate`); rejects inline2 on `Str_drop` (2g.90) |

## Per-file report template

- Path / LOC / role
- Where it is called (callers table: file, line, mechanism) + imports
- Findings: severity-tagged (HIGH/MEDIUM/LOW/INFO), split into
  **verified live** vs **code inspection**
- Suggested fix order
- Verification state (what was actually run)

## History: re-verification pass — 2026-10-23

All 52 modules re-audited against current `compiler/*.fd` source. ~55 distinct
claims traced to line-level evidence. Result: **analysis ~85% accurate** —
every CRITICAL/HIGH claim re-checked held up; ~8 findings were fixed between
the analysis and this pass; ~5 remain plausible-but-unverified statically.

### Fixed since analysis (rows updated above)

| File | Was | Now |
|---|---|---|
| token.fd | Spawn/Yield tag collision (both 25) | Yield = 136 — resolved |
| tensor.fd | dead; interp stubs Int(0)/"[]" | p_tensor_*/p_quantize_*/p_kv_* wired to Tensor_*/KvCache_* |
| x86.fd | dead (14th) | wired: `build-x86` + `build_x86_linux` (compiler.fd:1912) |
| cparser.fd | dead | wired: `parse-header` → cmd_parse_header |
| gpu.fd | dead | wired: `gpu` cmd → generate_spirv/ptx (stubs now live) |
| coff.fd | dead | wired: `codegen --emit-coff` (nop-encoder now reachable) |
| macho.fd | dead | wired: `codegen --macho` |
| veil.fd | run_with_coverage uncalled | now called (:347); hook still no-op → still 0% |
| interp.fd | tensor/kv stubs | wired; chan/grad/etc still stubs |
| main.fd | ~12 dropped commands | ~8 dropped (codegen/gpu/verify/sourcemap/parse-header/notebook/build-x86 restored; debug/replay/native-debug/link/diff-test/sbom/qualify/spdx still gone) |

### Confirmed still TRUE — highest blast radius

- **linker.fd:998-1034** — 9× `write_u64(buf, buf.len(), …)` → OOB `set`;
  ELF header fields (e_entry, phdrs) never written → malformed ELF end-to-end.
- **elf64.fd:624 + ~40 decode sites** — `offset*256+base` pack decoded with
  trunc division → every `-N(%rbp)` off by +1; `jo`/`movapd`/`*pd` unhandled →
  NOP while codegen_x64 *emits* `movapd` for XMM spills (:1896,1910);
  `.rela.rodata` collected but never emitted → vtables = 0.
- **runtime_linux.fd** — the 1,450-line `runtime_asm` block is built then
  dropped (`encode_asm_block` exists at elf64.fd:2477 but is never called from
  runtime_linux) → all `p_string_*`/`p_map_*` builtins missing from ELF symtab.
- **pir.fd:1125** — StructCopy → `ConstInt(0)`; **ir.fd:1163** — `inst_effects`
  catch-all `_ => {}` returns *empty* EffectSet → struct-copy fns look pure →
  DCE can legally strip them. **interp.fd:1637** — StructCopy aliases, not copies.
- **interp.fd** — Store always 8 bytes (:1181); silent `Int(0)` fallthrough
  (:3768-3775); channels still fabricated (:2674-2691); checked arithmetic
  dropped; `Interpreter_set_debug_hook` flag-only (:4212).
- **lexer.fd:353-364** — `5e3` still broken (exponent inside `.` branch).
- **json.fd:348-351** — malformed numbers → 0.0.
- **parser.fd:5172-5175** — f-string sub-parse still commented out.
- **typechecker.fd:1332** — verify layer still "stubs — permissive" +
  cap_verified whitelist.
- **opt.fd** — Licm/EscapeAnalysis still absent; borrow_non_aliasing and
  linear_ptrs still have zero consumers.
- **compiler.fd:2602-2607** — default trait methods still dropped;
  `--borrowck=warn` still reads `bc.errors`; `apply_cfg_filter` still no-op.
- **arm64_enc.fd:1091-1164** — `enc_lsl`/`enc_asr` SBFM fields swapped
  (imms<<16 / immr<<6 — imms overlaps rn). Verified against ARM spec.
- **codegen_arm64.fd:509-514 / codegen_riscv.fd:508** — immediates →
  scratch-reg catch-all; `return` (not `ret`) still emitted.
- **pe.fd:1391/1397** — stack/heap fields same pattern; FloatLit → 8 zero
  bytes (:2078-2081); `pe_debug.txt` instrumentation still present.
- **green_thread.fd** — `p_gt_*` → `gt_global_*` but those still return
  constants; `codegen_c` still has zero `p_gt_*` → C-path link failure.
- **lower.fd** — `await` → `p_future_poll` unimplemented → Int(0).
- **wasm.fd:1163/1169** — DynCall/CallPtr → silent `i32.const 0`.
- **runtime_arm64.fd:139-141** — assembler Err → silent empty RuntimeParts.
- Still dead: codegen.fd, autograd.fd, diag.fd, debugger.fd (`debug` still not
  dispatched — comment header only), native_debugger.fd, embedded.fd,
  flat_ast.fd (`dump-ast-flat`→cmd_ast decoy), hotreload.fd (`watch`→cmd_bench).

### Plausible but not fully re-verified

- codegen_c GetElemPtr struct-array claim (needs functional test)
- fmt exact corruption bytes (mechanism confirmed)
- borrowck Rule-3 field/deref skip details (catch-alls confirmed)
- codegen_x64 float-arg-side ABI (return side confirmed)
- arm64_enc remaining claims (fp alias, reg-offset, .rodata)
- peephole `notq` flag bug (mechanism consistent with source)

### Still-open priorities by blast radius

1. `linker.fd` ELF header writes — every emitted ELF is malformed.
2. `elf64.fd` operand decode + `movapd`/`jo` gaps — native x86-64 path.
3. `runtime_linux.fd` dropped asm block — all string/map builtins absent.
4. `interp.fd` Store-8B + Int(0) fallthrough — corrupts the oracle for all backends.
5. `pir`/`ir` StructCopy evaporation — silences real optimization bug.
6. `typechecker` verify layer — the marketed safety checks are permissive stubs.

## B-batch fixes — 2026-10-23 (post re-verification)

Working through the "still-open priorities by blast radius" list:

| Priority | Item | Status |
|---|---|---|
| 1 | `linker.fd` ELF header writes — `write_u64(buf, buf.len())` OOB | **FIXED** — write_u32/write_u64 now append when `off >= len` (linker.fd:198-219). e_entry/phdrs now written. |
| 2a | `elf64.fd` `offset*256+base` trunc-decode (off-by-one on all `-N(%rbp)`) | **FIXED** — `floor_div`/`floor_mod` helpers; all 68 decode sites converted; wrong `off+=256` pseudo-fix removed. |
| 2b | `movapd`/`jo` unhandled → NOP (deletes XMM spills + overflow traps) | **FIXED** — `encode_mov_xmm` parameterized (movapd/movupd/movaps/movups); `jo`(0x80)/`jno`(0x81); `addpd/subpd/mulpd/divpd/xorpd` via `_66`. |
| 2c | `.rela.rodata` collected never emitted → vtables=0 | **FIXED** — section now emitted + header added (sec 7); also fixed swapped `sh_link`/`sh_info` on `.rela.text` (was 1/3, now 3/1). |
| 3 | `runtime_linux.fd` dropped asm block | Already fixed (A1) — encode_asm_block called at :2858; re-verify note was stale. |
| 4 | `interp.fd` Store-8B + Int(0) fallthrough | **FIXED** — Store now resolves explicit `OptIrType`→val-meta→I64 and writes 1/2/4/8B (+`Memory_store_i16`); unknown runtime funcs now `Panic("Unknown runtime function: X")` matching Rust :6165 (was silent Int(0)). |
| 5 | `pir`/`ir` StructCopy evaporation | **FIXED** — pir.fd:1125 now lowers to `L1Inst::Store(dst,src,Some(ty),Write)` (matches src/pir.rs:801); ir.fd `inst_effects` adds Write arm; `inst_value_refs` pushes dst+src; interp does real `type_size` byte memcpy (was value-slot aliasing). `codegen_x64::gen_store` already handled struct-typed stores — L3 path was ready. |
| 6 | `typechecker` verify layer | Not attempted — large rewrite, separate work item. |

## C-batch fixes — 2026-10-23

| Item | Fix |
|---|---|
| C1 `elf64.fd` hex immediates | `encode_alu`/`encode_cmp`/mov-imm→mem parsed `$imm` decimal-only → `xorq $0x5DEADBEEFCAFEBABE` stack canary became `xor $0` (never detects smashing). Added `parse_dollar_imm` helper (0x-hex fallback, mirrors `encode_mov`) at all three sites. |
| C2 `linker.fd` unknown-symbol skip | Both reloc passes collected undefined syms then silently skipped → `e8 00 00 00 00` call fallthrough. Now `LinkerResult::Err("Undefined symbols: a b c")` — inherited bug (src/linker.rs:606 identical) but too dangerous to keep after the dropped-asm-block incident. |
| C3 `interp.fd` checked arithmetic | `Add/Sub/Mul` now pre-check overflow (CERT-style division bounds — safe under checked AND wrapping hosts) → `Value::Void` → caller's "Integer overflow" panic; `@wrapping` fns still wrap. `is_wrapping_func` no longer dead. `i64::MIN/-1` div/mod edge handled (was SIGFPE on compiled backends). |
| C4 `interp.fd` txn rollback | `TxnBegin` pushed `self.memory` by value → shared Vec backing → snapshot already post-txn. Now `Memory_clone` (deep `data.clone()`) + real `opaque_values` restore via new `OpaqueSnapshot` struct (field was dead). |
| C5 `ir.fd` verify + DCE | `IrModule_verify` `Branch(then_b,else_b,_)` destructured positionally → validated cond-as-block; fixed field order. `dead_function_elimination` BFS now keeps all `ConstFnRef`-address-taken functions when any `CallPtr`/`DynCall` is reachable (was: stripped every closure/dispatch callee). |
| C6 `ir.fd` O(N²) hot path | `get_inst`/`get_meta` O(1) index-by-id fast path (ids are dense counters; verified `get(id).id==id` before accepting, scan fallback). Was ~O(insts) per executed instruction — the dominant `fidesc run` cost. |

## D-batch — consolidation refactor — 2026-10-23

Structural dedup work (the "three x86-64 codegens / three runtimes / helper-soup"
findings, executed):

| Item | Change |
|---|---|
| D1 `pe.fd` → format writer | Embedded second x86-64 codegen **deleted** (~1,000 LOC: gen_all_functions/gen_function/all gen_inst_*/gen_*_binop/gen_call*/PeStackFrame/collect_strings/pe_debug.txt). `generate_pe(module, l3)` now consumes codegen_x64 asm via `ElfEmitter` — **Windows builds now pass through L1/L2/L3 + optimizer** (was bypassed). Kept: X86Encoder for stubs, ~90 Win64 stub bodies, IAT/import machinery (now dynamic — externs collected from assembler output), PE layout, bare-metal + multiboot. New glue: SysV→Win64 arg-shuffle wrappers, import trampolines, reloc/symbol resolution (pe_build/PeLinked/PendingUserRef). 7,860→6,852 LOC. Typechecks; native e2e pending D5. |
| D2 `coff.fd`/`macho.fd` → format-only | Internal toy assemblers deleted (nop-sled, wrong push/pop, 7-mnemonic limits die by deletion). Both now `ElfEmitter` + record translation: coff 497→424 LOC (+`.rdata` section, storage class from ELF binding — F4 fixed); macho 483→445 (+`__const` section, real BRANCH/SIGNED/UNSIGNED rtypes). Typecheck clean. |
| D3 `green_thread.fd` → one real layer | `gt_global_*` stubs + `green_thread_c_runtime` blob deleted (386→207 LOC). `GreenThread.func_id`→`func_name` (matches C backend's name lookup). interp.fd owns `Interpreter.gt_scheduler` — spawn queues READY, run_all resolves+executes, is_done/get_result real. `p_thread_spawn`/`p_future_poll`/`p_executor_*` also real. **Live-verified**: spawn/await→41; multi-spawn 9/16/25→50. Open: codegen_c still lacks p_gt_* (C-path link failure). |
| D4 Shared str helpers → core.fd | `str_lines` + `str_trim_end_char` added to stdlib/core.fd. Local copies deleted: a64_str_lines, rv_str_lines, cp_str_lines, cp_str_trim_end_char, sc_str_trim_end_char, lower_str_starts_with, tc_str_starts_with (→ existing core str_starts_with). −7 duplicate helpers. |
| D5 `opt.fd` rpo_dfs → iterative | Was recursive over CFG; self-hosted `--native` overflowed at depth ~2047 even on empty programs (whole-stdlib optimization). Now explicit-stack iterative + per-block succ cache. Also fixes the PE/native verification blocker. |
| D6 `runtime.fd` scaffold (NEW, 531 LOC) | Fides-authored runtime — p_malloc/p_free (free-list over rt_sys_mmap), memset/memcpy, strlen/concat/equal/compare/itoa, print/panic/exit, file ops, argc/argv/getenv/clock over `rt_sys_*` per-arch ABI contract. Replaces the 3×-duplicated builtin logic; per-arch keeps ~150 LOC stubs (_start, syscall wrappers, ctx switch, atomics). Not yet wired. |
| D7 `core.fd` latent bug fix | `StringBuilder::append_char`/`append_int` did `str + i64` (p_char_to_str/p_to_str return i64) — type error only firing on monomorphization. → `byte_to_str`/`int_to_str` wrappers. Surfaced by D4/D6 testing. |
