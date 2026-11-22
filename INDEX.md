# Fid Project Index

> Navigation hub for Fid and its compiler, Fides. Every file in the project, grouped by
> directory, with a one-line description. Use this to find anything fast.
>
> **Fides is self-hosted and released (v1.0.0).** The primary compiler is in `compiler/*.fd`
> (80 modules, ~361K lines). The Rust source in `src/*.rs` (57 files, ~113K lines) is the
> bootstrap reference implementation, frozen: it builds stage 1 and runs the test harness.
> The self-hosted compiler is the release compiler (`release/fid.exe`,
> shipped as `fid.exe`) and defines the language:
> [docs/reference/reference.md](docs/reference/reference.md).
>
> **Stats (Nov 21; exact figures in [selfhost.md → Numbers](docs/reference/selfhost.md#numbers)):** 80 self-hosted `.fd` compiler modules (~361K lines) · 57 `.rs` bootstrap files (~114K lines) ·
> 403 stdlib modules (~259K lines) · 1,753 `.fd` tests in `tests/bootstrap/` (1,682 top level, 456 expected-fail) + 57 in `tests/selfhost/` + 1,596 in `tests/conformance/` + hundreds more in `tests/{projects,verify,nattools,tooling,...}` · 12 examples · 12 tools in `tools/`.
> Fixpoint: VERIFIED byte-for-byte (stage 2 == stage 3, 67 MB of C, Nov 8; checked by `RELEASE=1 scripts/bootstrap.sh`; not yet re-verified on the Nov 19 tree). Nov 21: the current tree built in one step from a self-hosted stage 0 (the Nov 18 fid); the result passes run, emit, ccompile and the leak gate. A 3-stage bootstrap that starts cold from `fidesc` stops at stage 2 (out of memory, known-bugs 2g.79).
> v1.0.0 released Sep 9, 2026 with Windows installer (Fides-Setup-1.0.0.exe, 2.5 MB). v0.8.0-unstable prerelease Nov 10, 2026 (Fides-Setup-0.8.0.exe, 6.6 MB). `fid version` base is now `1.0.0` (`1.0.0+dev` on dev builds).

---

## Table of Contents

- [Documentation (docs/)](#documentation-docs)
- [Root Files](#root-files)
- [Self-Hosted Compiler (compiler/) — PRIMARY](#self-hosted-compiler-compiler--primary)
- [Source — Compiler Core (src/) — BOOTSTRAP REFERENCE](#source--compiler-core-src--bootstrap-reference)
- [Source — Backends & Codegen (src/)](#source--backends--codegen-src)
- [Source — Object Formats & Linking (src/)](#source--object-formats--linking-src)
- [Source — Advanced / Experimental (src/)](#source--advanced--experimental-src)
- [Standard Library (stdlib/)](#standard-library-stdlib)
- [Tests (tests/)](#tests-tests)
- [Scripts (scripts/)](#scripts-scripts)
- [Examples (examples/)](#examples-examples)
- [Tools (tools/)](#tools-tools)
- [Benchmarks (bench/)](#benchmarks-bench)
- [Build Artifacts & Generated Files](#build-artifacts--generated-files)

---

## Documentation (docs/)

Design, architecture, roadmap, and analysis documents. Read `architecture.md`
and `AGENTS.md` first when working on the compiler.

### Core Reference
| File | Description |
|------|-------------|
| [reference/architecture.md](docs/reference/architecture.md) | Custom-toolchain rationale (§0) + compiler pipeline, data flow, source file map. **Start here.** |
| [reference/syntax.md](docs/reference/syntax.md) | Full language syntax specification (current `fn`/`val`/`var`/`const`/`return`/`type` syntax — migration from old `pf`/`pv`/`pc`/`ret`/`ty` is complete). |
| [reference/features.md](docs/reference/features.md) | Feature status — verified against actual test runs. |
| [reference/tool.md](docs/reference/tool.md) | Development tools (fidesc, ark, scribe, veil). |
| [reference/testing.md](docs/reference/testing.md) | Current test results across all backends + test batch specifications. |
| [reference/changelog.md](docs/reference/changelog.md) | Per-phase change history (newest first). |
| [reference/language-guide.md](docs/reference/language-guide.md) | Comprehensive language guide — type system, ownership, effects; includes references §4.6 and @secret §5.4. |
| [reference/selfhost.md](docs/reference/selfhost.md) | Self-hosted compiler — coordination, architecture, build pipeline, porting guide (consolidated). |
| [reference/quickstart.md](docs/reference/quickstart.md) | Install + first program. |
| [reference/status.md](docs/reference/status.md) | Detailed status, test results and limits (moved from the README). |
| [reference/releasing.md](docs/reference/releasing.md) | How to cut a release: checks, installer signing, checksums, publishing. |
| [reference/reference.md](docs/reference/reference.md) | **The language reference**: what `fid` accepts, rule by rule, each with the compiler function that enforces it. |
| [reference/safety-rules.md](docs/reference/safety-rules.md) | The 21 safety rules (SR-1–SR-21), each traced to its negative test. |
| [reference/memory-model.md](docs/reference/memory-model.md) | What one thread may see of another's writes (C++11 model, proposal 0003). |
| [reference/optimizer-facts.md](docs/reference/optimizer-facts.md) | Language facts the optimizer uses, with the rule and test behind each (LF25). |
| [reference/platforms.md](docs/reference/platforms.md) | Platforms by tier: what runs where and how it is tested. |
| [reference/stability.md](docs/reference/stability.md) | Stability promise, releases, language-change rules. |
| [reference/diagnostics.md](docs/reference/diagnostics.md) | The Exxx error codes; `fid` prints them (`error[E044]: ...`) for the errors it can classify. |

### Guides (how things work)
| File | Description |
|------|-------------|
| [reference/best-practices.md](docs/reference/best-practices.md) | Hard-won insights from building a custom toolchain. |
| [reference/calling-conventions.md](docs/reference/calling-conventions.md) | SRET, enum layout, struct copy, Box semantics — low-level codegen machinery. |
| [reference/known-bugs.md](docs/reference/known-bugs.md) | Self-host known bugs & history (+ port-gap-analysis appendix). |
| [reference/borrowchecker.md](docs/reference/borrowchecker.md) | Borrow checker design document (Phases 1-6). |

### Meta (big-picture thinking)
| File | Description |
|------|-------------|
| [reference/vision.md](docs/reference/vision.md) | Vision, design principles, decisions, future direction (consolidated). |
| [reference/positioning.md](docs/reference/positioning.md) | Competitive analysis (vs Rust/Zig/Go/Swift) + adoption strategy + industry conversion guide. |
| [reference/lessons.md](docs/reference/lessons.md) | PL history lessons + IBM AIX/z/OS lessons learned. |


### Roadmaps
| File | Description |
|------|-------------|
| [roadmap/roadmap.md](docs/roadmap/roadmap.md) | Master roadmap — milestones, features, release/self-host, limitations, issues, opportunities (consolidated 2026-10-24). |
| [roadmap/release-roadmap.md](docs/roadmap/release-roadmap.md) | Road to public beta and stable 1.0 — every known gap, by track, with acceptance criteria. |
| [roadmap/competitive-roadmap.md](docs/roadmap/competitive-roadmap.md) | Maturity of Rust/Go/Zig/C++: the benchmark scoreboard, optimizer, runtime, compile speed, verification, language and stdlib tracks. |
| [roadmap/pe-roadmap.md](docs/roadmap/pe-roadmap.md) | Native Windows backend (`fid emit`, pe_ir). |
| [roadmap/inc-roadmap.md](docs/roadmap/inc-roadmap.md) | Incremental compilation. |
| [roadmap/complaint-roadmap.md](docs/roadmap/complaint-roadmap.md) | Developer complaints about other languages and Fid's answers. |
| [roadmap/llvm-roadmap.md](docs/roadmap/llvm-roadmap.md) | Own-backend roadmap: one machine layer, SSA and a rules mid-end, whole-program and language-fact optimizations, compile speed, reliability (200 goals). |
| [roadmap/external-roadmap.md](docs/roadmap/external-roadmap.md) | Goals no code can finish: security audits and a bug bounty, certification, production use, community, teaching, a foundation. |
| [roadmap/safety-critical.md](docs/roadmap/safety-critical.md) | Safety-critical roadmap (IEC 62304 / DO-178C class work). |
| [roadmap/security-roadmap.md](docs/roadmap/security-roadmap.md) | Security hardening guide and exploit-class coverage. |
| [roadmap/bug-audit-2026-10-22.md](docs/roadmap/bug-audit-2026-10-22.md) | Dated bug-audit snapshot. |

### Analysis, proposals, verification
| File | Description |
|------|-------------|
| [file-analysis/](docs/file-analysis/PROGRESS.md) | One doc per `compiler/*.fd` module (69 of 80 — the 11 modules added since Nov 9 lack a page), rewritten Nov 9, 2026: role, callers, commands that reach it, open problems. |
| [proposals/](docs/proposals/) | Language proposals: 0001 value semantics, 0002 generics, 0003 memory model and cancellation, 0004 ownership and layout, 0005 region arenas, 0006 the M5 decision (ownership, not copy-on-write). |
| [verification/README.md](docs/verification/README.md) | Verification tools (fuzzers, soak, chaos, interleavings), formal semantics, memory-safety core, translation validation, the miscompile log. |
| [stdlib/README.md](docs/stdlib/README.md) | Standard library module docs. |

### Single-topic notes (docs/*.md)
| File | Description |
|------|-------------|
| [benchmarks.md](docs/benchmarks.md) | What is measured, how to reproduce it, every result, losses included (BM10, AD13). |
| [c2fd.md](docs/c2fd.md) | `tools/c2fd.fd`: best-effort C99 to Fid translation. |
| [compiler-crash-reports.md](docs/compiler-crash-reports.md) | `--crash-report`, `fid reduce` and `fid explain --stages` (RL20, DB7). |
| [compiler-identity.md](docs/compiler-identity.md) | What names a build of `fid`: version, build hash, cache key. |
| [compiler-internals.md](docs/compiler-internals.md) | A map of `compiler/*.fd` for contributors. |
| [cookbook.md](docs/cookbook.md) | Short recipes: files, JSON, HTTP, tasks. |
| [cpp-interop.md](docs/cpp-interop.md) | Calling C++ from Fid (`@import("lib.hpp")`, LG16). |
| [disasm-encoding-table.md](docs/disasm-encoding-table.md) | One table of encoding forms for `fid disasm` and the encoders (TG5). |
| [fuzzing.md](docs/fuzzing.md) | Fuzz targets (`fn fuzz_*`) and `fid fuzz` / `fid test --fuzz`. |
| [host-build.md](docs/host-build.md) | `fid build` with no `--target` on Linux and macOS (CB4). |
| [image-writer.md](docs/image-writer.md) | One writer of ELF, PE/COFF and Mach-O files (TG13). |
| [import-cache.md](docs/import-cache.md) | The import cache of the front end (CB1). |
| [inc-instances.md](docs/inc-instances.md) | Where `ccompile --inc` puts generic instances and per-type helpers. |
| [inc-ir-units.md](docs/inc-ir-units.md) | C per IR unit in `ccompile --inc` (inc-roadmap B1, B2). |
| [inc-seed-store.md](docs/inc-seed-store.md) | The seeded store and globals in per-module builds. |
| [incremental-check.md](docs/incremental-check.md) | Incremental checks in `fid serve` (CB2). |
| [internal-tools.md](docs/internal-tools.md) | The project's own tools and what each is written in (AD4). |
| [interp-tasks.md](docs/interp-tasks.md) | Tasks in `fid run`: interleaving and cancellation (LG11). |
| [mi20-prim-interface.md](docs/mi20-prim-interface.md) | The `nat_*` machine-code primitives every native backend defines (MI20). |
| [pact-capabilities.md](docs/pact-capabilities.md) | Capabilities `ark.toml` grants each pact, checked in every build (CD1). |
| [pe-ir-cache-and-cpu.md](docs/pe-ir-cache-and-cpu.md) | pe_ir's per-function cache and `--cpu`. |
| [pir-retired.md](docs/pir-retired.md) | Why and how the PIR path was retired (MI18). |
| [sp8-edit-rebuild.md](docs/sp8-edit-rebuild.md) | A one-line edit rebuilt by a warm `fid serve` (SP8). |
| [stdlib-agent-bugs.md](docs/stdlib-agent-bugs.md) | Compiler bugs found while writing the new stdlib modules (Nov 17). |
| [stdlib-backend-matrix.md](docs/stdlib-backend-matrix.md) | The standard library on every backend (`tests/stdlib_backends.sh`, CD2). |
| [stdlib-depth-audit.md](docs/stdlib-depth-audit.md) | Stdlib depth audit of all 403 modules (Nov 18, read-only snapshot). |
| [vec.md](docs/vec.md) | An old `scribe` page of `stdlib/vec.fd`, out of date; the current one is `docs/stdlib/vec.md`. |
| [website.md](docs/website.md) | The project website, `tools/site.fd` (AD2). |

### Other
| File | Description |
|------|-------------|
| [fid-logo.svg](docs/fid-logo.svg) | Project logo (SVG). |
| [book/](docs/book/01-intro.md) | The Fid Book: first program to a network service. |
| [guides/](docs/guides/) | Coming from Rust/Go/Zig/C/Python; debugging, profiling and crash reports. |
| [course/](docs/course/README.md) | University course: 13 weeks of notes and a lab per week. |
| [playground/](docs/playground/README.md) | The browser playground and its embed script. |
| [perf/](docs/perf/) | Performance investigations, one per goal. |
| [security/](docs/security/review-index.md) | Security review index. |

---

## Root Files

| File | Description |
|------|-------------|
| [README.md](README.md) | Project overview, status, feature matrix, quick start. |
| [CONTRIBUTING.md](CONTRIBUTING.md) | How to build, test and send a change. |
| [SECURITY.md](SECURITY.md) | Reporting vulnerabilities privately; what counts as a security bug. |
| [SUPPORT.md](SUPPORT.md) | Where questions, bugs and contributions go. |
| [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) | Code of conduct (Contributor Covenant 2.1, shortened). |
| [LICENSE](LICENSE) | MIT licence. |
| [INDEX.md](INDEX.md) | This index. |
| [AGENTS.md](AGENTS.md) | **Working guide for AI assistants & humans** — build commands, high-risk files, known bugs, conventions. Read before editing. |
| [Cargo.toml](Cargo.toml) | Rust package manifest (`fidesc` v1.0.0). |
| [Cargo.lock](Cargo.lock) | Pinned dependency versions. |
| [verify.sh](verify.sh) | Full verification: build + test + compare to baseline. `--save` updates baseline, `--quick` is build-only. |
| [.gitignore](.gitignore) | Git ignore rules. |

---

## Self-Hosted Compiler (compiler/) — PRIMARY

> **This is the primary compiler.** 80 Fid modules (~361K lines) that compile
> themselves to byte-identical C across generations (bootstrap fixpoint
> verified, stage 2 == stage 3 — last verified Nov 8, before the ~2.3x
> Nov 10–19 growth). The release build
> (`RELEASE=1 scripts/bootstrap.sh` → `release/fid.exe`) is what the
> installer ships as `fid.exe`.
> 69 of the 80 modules have a doc in `docs/file-analysis/<module>.md` (role,
> callers, the commands that reach it, open problems), all written Nov 9,
> 2026; the 11 modules added since (marked **new** below) have no page yet.
> The tracker is `docs/file-analysis/PROGRESS.md`. Open bugs found by that
> audit: `docs/reference/known-bugs.md` §2g.

| File | LOC | Description |
|------|----:|-------------|
| **Driver** | | |
| [main.fd](compiler/main.fd) | 9,570 | `fid` entry point: global flags, command dispatch (`run`, `ccompile`, `emit`/`build`, `check`, `fix`, `fmt`, `lint`, `test`, `fuzz`, `bench`, `lsp`, `serve`, `wasm`, `gpu`, `disasm`, `reduce`, `ir-fuzz`, `split-c`, `test-rules`, …), C compiler invocation, build cache, `--target` handling |
| [compiler.fd](compiler/compiler.fd) | 13,502 | Pipeline: core and runtime merge, imports, AST passes, checks, lowering; backend entry points |
| [inc.fd](compiler/inc.fd) | 1,751 | Build stamps, `.fidcache` artifact store, checked-core stamp, in-memory IR cache |
| [csplit.fd](compiler/csplit.fd) | 590 | `fid split-c`: the compiler's C as a header plus n units (the -O2 release build) |
| [build_info.fd](compiler/build_info.fd) | 60 | **new.** Version stamp: `fid_version_base` "1.0.0", `fid version` prints `1.0.0+dev` on dev builds, release version written by `scripts/build-info.sh` |
| **Front end** | | |
| [token.fd](compiler/token.fd) | 686 | Token kinds, raw-discriminant tags |
| [lexer.fd](compiler/lexer.fd) | 1,805 | Source → tokens (`5e3` still lexes as `5`, `e3`) |
| [parser.fd](compiler/parser.fd) | 10,682 | Tokens → AST: f-strings, turbofish, spawn/cancel scopes, `@secret` flag |
| [ast.fd](compiler/ast.fd) | 2,640 | AST types and shared helpers |
| [ast_ser.fd](compiler/ast_ser.fd) | 2,706 | **new.** Binary AST codec ("FIDA" + hash): the checked stdlib/runtime artifact in `.fidcache` so later compiles skip lexing/parsing (CS2) |
| [comptime_eval.fd](compiler/comptime_eval.fd) | 4,989 | `comptime gen` and compile-time substitution |
| [mono.fd](compiler/mono.fd) | 5,473 | Turbofish resolution and monomorphization (renaming misses calls inside closures, f-strings, `\|>`: 2g.8) |
| **Checks** | | |
| [typechecker.fd](compiler/typechecker.fd) | 24,758 | Types, effects, capabilities, safety annotations, generics (bounds, inference), `&str`/`@scoped`, `@linear`, `@send`, sandbox |
| [escape.fd](compiler/escape.fd) | 2,570 | Second-class references: none stored, returned from a local or sent to a task |
| [secrecy.fd](compiler/secrecy.fd) | 1,349 | `@secret`/`@sensitive` information flow (runs only when a program uses them) |
| [borrowck.fd](compiler/borrowck.fd) | 3,486 | Borrow checker: NLL, field borrows, `for`-loop invalidation (`--borrowck=warn` prints nothing) |
| [cowcheck.fd](compiler/cowcheck.fd) | 1,313 | `FIDES_COW_CHECK=1` report; use/change walkers the ownership lowering uses |
| **IR and lowering** | | |
| [lower.fd](compiler/lower.fd) | 27,881 | AST → SSA IR; inserts every ownership dup/drop/move (proposal 0004, `LOWER_*` switches) |
| [reach.fd](compiler/reach.fd) | 515 | Functions `emit` lowers: what main reaches, core and runtime, names in strings (`FID_LOWER_ALL=1` lowers everything) |
| [ir.fd](compiler/ir.fd) | 5,991 | IR types and instructions, `fn_index`/`str_index`/`inst_at`, verify, reachability, dead-function removal |
| [checks.fd](compiler/checks.fd) | 6,705 | Drops proven checks: counted loops, integer ranges, `Vec::filled` bounds (C and native) |
| [irverify.fd](compiler/irverify.fd) | 166 | `FID_VERIFY=1`: operands defined, defined before use, dominance, jump targets |
| [ir_ser.fd](compiler/ir_ser.fd) | 4,535 | **new.** Binary codec for `IrFunction`: the per-function lowered-IR cache (CS4); names re-intern against the live module tables |
| **Mid-end** | | |
| [iropt.fd](compiler/iropt.fd) | 11,145 | IR optimizer and pass manager: inline, gconst, fold, branchfacts, sccp, promote, devirt, dce, tailcall, gvn, licm (`FID_IROPT_OFF`/`ON`) |
| [iropt_me.fd](compiler/iropt_me.fd) | 6,608 | Restructuring passes and the alias query; on by default since Nov 16–17 (`IME_DEFAULT_ON`: loops, sra, loadhoist, forward, matchfacts, inline2, unroll, version, ssa, ...), switch, ivsr, rotate and ipcp opt-in (`FID_IROPT_ON`) |
| [ssa.fd](compiler/ssa.fd) | 1,829 | **new.** SSA form inside the mid-end: mem2reg with merges, dead phis/stores, exit values of counted loops (ME3/ME15/ME23) |
| [rules.fd](compiler/rules.fd) | 1,079 | Rewrite rules as data (z3-proved), pure-call merging, `fid test-rules [--smt]` |
| [egraph.fd](compiler/egraph.fd) | 4,079 | E-graph rewriting and store-to-load forwarding of locals (nat_ir path) |
| [tv.fd](compiler/tv.fd) | 6,087 | **new.** Translation validation of the mid-end (RL5/VF6): with `--validate`/`FID_VALIDATE=1` the pass manager checks each transform preserves semantics; `.tvp` test pairs in tests/verify/tv/ |
| **Back ends in use** | | |
| [interp.fd](compiler/interp.fd) | 14,130 | IR interpreter (`fid run`) over a simulated byte memory, ~270 builtins |
| [codegen_c.fd](compiler/codegen_c.fd) | 16,565 | IR → C99 plus the embedded C runtime (`ccompile`, `dump-c`, `run --leaks`/`--checked-alloc`/`--mem`) |
| [pe_ir.fd](compiler/pe_ir.fd) | 9,010 | Default Windows backend: IR → x86-64 → PE32+ (`emit`/`build`); also `--bare`, `--boot`, `--uefi` and `--elf` |
| [pe_ir_enc.fd](compiler/pe_ir_enc.fd) | 3,293 | x86-64 encoder, imports, `.rdata`, PE writer for pe_ir |
| [pe_ir_stubs.fd](compiler/pe_ir_stubs.fd) | 10,196 | pe_ir's runtime hookup: stub generators, `pe_runtime_alias`, allocator, bare runtime, `asm!` encoder |
| [nat_ir.fd](compiler/nat_ir.fd) | 41,010 | Machine layer: MIR selection, linear-scan + greedy allocators, IPRA, live-range splitting, checker; AArch64/RISC-V/x86-64 Linux ELF, macOS Mach-O and `--nat` PE |
| [nat_enc.fd](compiler/nat_enc.fd) | 12,948 | Machine-layer encoders (A64, RV64, x64, Thumb-2), ELF with `.debug_line`, PE32+, Mach-O (`nat_macho`) |
| [nat_thumb.fd](compiler/nat_thumb.fd) | 3,207 | **new.** Cortex-M (ARMv7-M, Thumb-2) code generator over nat_enc's `t2_*`/`t_*` encoders (PL5) |
| [mir.fd](compiler/mir.fd) | 1,579 | **new.** The machine IR (MIR) nat_ir selects into (Cranelift VCode shape; MI1) |
| [image_writer.fd](compiler/image_writer.fd) | 968 | **new.** The one writer of executable/object files (TG13): sections, relocations, symbol tables for ELF, PE/COFF and Mach-O |
| [disasm.fd](compiler/disasm.fd) | 2,563 | **new.** `fid disasm`: disassembler for the machine layer's output on x86-64, AArch64, RV64; reads .fd through emit and exe files directly (TG5) |
| [pdb.fd](compiler/pdb.fd) | 1,438 | **new.** PDB (MSF 7.00) program databases for Windows images (DB3/N9): `fid emit -g` writes `<out>.pdb` on pe_ir and `--nat` |
| [pgo.fd](compiler/pgo.fd) | 444 | **new.** Profile-guided optimization (OP30): `fid run --profile out.prof` counts calls; `emit --profile-use` consumes it |
| [green_thread.fd](compiler/green_thread.fd) | 193 | Green-thread bookkeeping for the interpreter's `p_gt_*` |
| [tensor.fd](compiler/tensor.fd) | 832 | Tensor / KV-cache library used by the interpreter |
| [wasm.fd](compiler/wasm.fd) | 4,247 | IR → WebAssembly (`fid wasm`, `--wat`): valid WASI preview-1 modules, no C compiler; tasks switch stacks; 316/344 suite tests match (Nov 15–18) |
| [gpu.fd](compiler/gpu.fd) | 1,385 | SPIR-V / PTX output (`fid gpu --target spirv\|ptx`) |
| [elf64.fd](compiler/elf64.fd) | 3,554 | AT&T assembler + ELF `.o` writer (`--elf` path and object output) |
| [x86.fd](compiler/x86.fd) | 1,888 | IR → stack-machine x86-64 (`build-x86`; output segfaults — legacy path) |
| **Tools** | | |
| [fmt.fd](compiler/fmt.fd) | 2,196 | `fid fmt` |
| [lsp.fd](compiler/lsp.fd) | 4,576 | `fid lsp`: JSON-RPC over stdio, diagnostics, completion, hover |
| [json.fd](compiler/json.fd) | 729 | JSON parse/dump (lsp, interpreter, `qualify`) |
| [cparser.fd](compiler/cparser.fd) | 4,265 | `fid parse-header`: C header → FFI bindings (`--link <lib>`) |
| [scribe.fd](compiler/scribe.fd) | 1,910 | `fid doc`/`fid scribe`: documentation generator |
| [veil.fd](compiler/veil.fd) | 742 | `fid veil`: coverage |
| [ark.fd](compiler/ark.fd) | 7,169 | `fid ark`: package manager (init, add, install, lock, workspaces) |
| **Retired PIR path (imported by nothing main.fd reaches)** | | |
| [pir.fd](compiler/pir.fd) | 3,364 | IR → PIR L1/L2/L3; retired (MI18): `pir`/`dump-pir` say where their views went, `codegen` is the machine layer |
| [opt.fd](compiler/opt.fd) | 9,497 | PIR L2 optimizer, 19 passes; panics on hello world (2g.17) |
| [regalloc.fd](compiler/regalloc.fd) | 1,505 | Linear scan for PIR L3 |
| [peephole.fd](compiler/peephole.fd) | 1,331 | 9-pass x86-64 asm peephole |
| [codegen_x64.fd](compiler/codegen_x64.fd) | 2,286 | PIR L3 → AT&T x86-64 |
| [codegen_arm64.fd](compiler/codegen_arm64.fd) | 658 | PIR L3 → A64 assembly (wrong: `return` for `ret`) |
| [codegen_riscv.fd](compiler/codegen_riscv.fd) | 679 | PIR L3 → RV64 assembly (same problems) |
| [linker.fd](compiler/linker.fd) | 1,067 | Static ELF linker of the PIR path |
| [coff.fd](compiler/coff.fd) | 300 | COFF `.obj` writer |
| [macho.fd](compiler/macho.fd) | 363 | Mach-O `.o` writer, x86-64 |
| [pe.fd](compiler/pe.fd) | 7,878 | Old PIR → PE path |
| [arm64_enc.fd](compiler/arm64_enc.fd) | 2,433 | A64 text assembler |
| [riscv_enc.fd](compiler/riscv_enc.fd) | 1,577 | RV64 text assembler |
| [runtime_arm64.fd](compiler/runtime_arm64.fd) | 805 | A64 runtime of the PIR path |
| [runtime_riscv.fd](compiler/runtime_riscv.fd) | 832 | RV64 runtime of the PIR path |
| [runtime_linux.fd](compiler/runtime_linux.fd) | 4,685 | Hand-encoded x86-64 Linux runtime of the PIR path |
| [codegen.fd](compiler/codegen.fd) | 1,557 | Legacy AST → ARM64 |
| **Not reached from main.fd** | | |
| [autograd.fd](compiler/autograd.fd) | 35 | No-op shim |
| [debugger.fd](compiler/debugger.fd) | 978 | Breakpoints and printers; `fid` has no `debug` command |
| [diag.fd](compiler/diag.fd) | 779 | Diagnostics with error codes; not wired, fails `fid check` |
| [embedded.fd](compiler/embedded.fd) | 504 | FixedBufferAllocator, HAL generator |
| [flat_ast.fd](compiler/flat_ast.fd) | 1,476 | Flat AST dump; `dump-ast-flat` prints the summary instead; fails `fid check` |
| [hotreload.fd](compiler/hotreload.fd) | 626 | Change snapshots and watcher |
| [native_debugger.fd](compiler/native_debugger.fd) | 521 | ptrace skeleton |
| [runtime.fd](compiler/runtime.fd) | 619 | Old plan for `p_*` in Fid; replaced by stdlib/linux_runtime; fails `fid check` |
| **Total** | **361,942** | **80 modules** (`wc -l`, Nov 21; 56 reached from `main.fd`). All 80 modules: `docs/file-analysis/<name>.md` |

---

## Source — Compiler Core (src/) — BOOTSTRAP REFERENCE

> **Note:** The Rust source is the bootstrap reference implementation. It is used
> to generate the first-generation C (`fides-c.c`) from the self-hosted compiler
> source (`compiler/*.fd`), and it holds the test harness (`fidesc test`).
> It is needed when `compiler/*.fd` changes and the bootstrap chain must be
> regenerated; the installer also ships it as `fidesc.exe` (its `emit`
> makes native Windows executables without a C compiler).

The front-end and middle-end pipeline: source → tokens → AST → types → IR.

| File | LOC | Description |
|------|----:|-------------|
| [main.rs](src/main.rs) | 4239 | CLI entry point, command dispatch, external tool invocation (cl.exe/zig link). |
| [token.rs](src/token.rs) | 263 | `TokenKind` enum — all Fid keywords and operators. |
| [lexer.rs](src/lexer.rs) | 793 | Tokenizer: source text → `Vec<Token>`. |
| [ast.rs](src/ast.rs) | 1009 | AST node definitions: `Program`, `Func`, `Stmt`, `Expr`, `StructDef`, `EnumDef`, etc. |
| [parser.rs](src/parser.rs) | 4029 | Recursive descent parser: `Vec<Token>` → `Program`. Error recovery + multiple error collection. |
| [typechecker.rs](src/typechecker.rs) | 10847 | Type validation, regions, effects, capabilities, traits, lints, @constant_time, Rc/Arc. ⚠️ MEDIUM risk. |
| [borrowck.rs](src/borrowck.rs) | 1457 | Borrow checker (Phases 1-6): `--borrowck=off\|warn\|on`, field-wise borrows, NLL. |
| [escape.rs](src/escape.rs) | 972 | Reference escape analysis — second-class references (session 29). |
| [secrecy.rs](src/secrecy.rs) | 774 | `@secret` information-flow check: a secret never reaches an output. |
| [ir.rs](src/ir.rs) | 1833 | SSA IR definitions: `IrModule`, `IrFunction`, `BasicBlock`, `Inst`, `IrType`, `BorrowKind`. |
| [lower.rs](src/lower.rs) | 9272 | AST → SSA IR lowering. ⚠️ MEDIUM risk — affects all backends. |
| [mono.rs](src/mono.rs) | 1704 | Generic monomorphization — specializes generic functions per call site. |
| [comptime.rs](src/comptime.rs) | 1492 | Compile-time eval: constant folding, reflection, f-string interpolation. |
| [compiler.rs](src/compiler.rs) | 3484 | Pipeline orchestration, module resolution, stdlib auto-include, CT verify wiring. |
| [lsp.rs](src/lsp.rs) | 1240 | LSP server: JSON-RPC 2.0 over stdio, diagnostics, no external deps. |
| [json.rs](src/json.rs) | 289 | Built-in JSON parser/serializer (used by LSP, no external deps). |
| [cparser.rs](src/cparser.rs) | 315 | C header parser → generates Fid FFI bindings (`parse-header` command). |
| [diag.rs](src/diag.rs) | 481 | Unified diagnostic system: error codes, source context, colored output, Levenshtein suggestions. |
| [fmt.rs](src/fmt.rs) | 1236 | Source code formatter (`fmt` command). |
| [seqdiag.rs](src/seqdiag.rs) | 236 | `fidesc diagram <file.fd>`: a Mermaid sequence diagram of the calls, with each callee's inferred effects. |

---

## Source — Backends & Codegen (src/)

IR → target output. Multiple backends: interpreter, native ELF64, PE, C, WASM, GPU, multi-arch asm.

| File | LOC | Description |
|------|----:|-------------|
| [interp.rs](src/interp.rs) | 6952 | IR interpreter — the reference oracle. ⚠️ MEDIUM risk. Uses `vec_side_table` for p_vec_* ops. Call stack tracking for panic traces. Rc/Arc runtime via p_rc_incr/p_rc_decr. |
| [pir.rs](src/pir.rs) | 2263 | PIR multi-level IR (L1/L2/L3) transforms. ⚠️ HIGH risk. |
| [opt.rs](src/opt.rs) | 8584 | PIR optimizer — 23 passes (const-fold, CSE, GVN, SROA, DSE, DCE, inline, unroll, TCO, etc.). Function-level parallelism. ⚠️ MEDIUM risk. |
| [peephole.rs](src/peephole.rs) | 994 | 9-pass peephole optimizer, always on in `build`. ⚠️ CRITICAL. |
| [regalloc.rs](src/regalloc.rs) | 1512 | Linear scan register allocator with constant rematerialization. ⚠️ CRITICAL. |
| [codegen_x64.rs](src/codegen_x64.rs) | 2633 | PIR L3 → x86-64 assembly (used by `build` and `codegen`) + post-codegen `verify_constant_time_asm`. LEA instruction selection, System V ABI. ⚠️ HIGH risk. |
| [codegen_c.rs](src/codegen_c.rs) | 7166 | C codegen backend: IR → C99 source (`ccompile` command). Binary hardening + stack traces + Rc/Arc runtime. `cast_arg` Ptr→Generic deref (session 30). ⚠️ HIGH risk. |
| [codegen_arm64.rs](src/codegen_arm64.rs) | 405 | PIR L3 → ARM64 assembly (`codegen --target arm64`). |
| [codegen_riscv.rs](src/codegen_riscv.rs) | 388 | PIR L3 → RISC-V assembly (`codegen --target riscv`). |
| [codegen.rs](src/codegen.rs) | 1150 | Legacy codegen dispatch (unused). |
| [x86.rs](src/x86.rs) | 1420 | x86-64 MASM codegen: IR → assembly string (legacy `build` path). |
| [wasm.rs](src/wasm.rs) | 1281 | WASM backend: PIR L3 → WebAssembly binary (`wasm` command). |
| [gpu.rs](src/gpu.rs) | 590 | SPIR-V GPU backend: PIR L3 → SPIR-V binary (`gpu` command). |
| [runtime_linux.rs](src/runtime_linux.rs) | 4383 | Hand-encoded x86-64 Linux runtime (raw syscall bytes, no libc). ⚠️ HIGH risk. |
| [arm64_enc.rs](src/arm64_enc.rs) | 1344 | ARM64 (AArch64) instruction encoder: assembly text → 4-byte machine code. 17 tests. |
| [runtime_arm64.rs](src/runtime_arm64.rs) | 961 | ARM64 Linux runtime using raw SVC syscalls (write, exit, mmap). 9 tests. |
| [riscv_enc.rs](src/riscv_enc.rs) | 933 | RISC-V (RV64) instruction encoder: assembly text → 4-byte machine code. 14 tests. |
| [runtime_riscv.rs](src/runtime_riscv.rs) | 868 | RISC-V Linux runtime using raw ECALL syscalls (write, exit, mmap). 7 tests. |

---

## Source — Object Formats & Linking (src/)

Binary emitters and the integrated linker. Byte-level correctness is critical.

| File | LOC | Description |
|------|----:|-------------|
| [elf64.rs](src/elf64.rs) | 2852 | ELF64 emitter: PIR L3 → executable + relocatable objects (Linux). ⚠️ HIGH risk. |
| [pe.rs](src/pe.rs) | 11607 | Direct PE emitter: IR → x86_64 machine code → PE `.exe` (no external tools). ⚠️ HIGH risk. |
| [macho.rs](src/macho.rs) | 374 | Mach-O emitter: object files for macOS. ⚠️ HIGH risk. |
| [coff.rs](src/coff.rs) | 385 | COFF emitter: object files for Windows. ⚠️ HIGH risk. |
| [linker.rs](src/linker.rs) | 968 | Integrated linker: resolves relocations across multiple object files. Target-aware (ARM64, RISC-V, x86-64). ⚠️ MEDIUM risk. |
| [dwarf.rs](src/dwarf.rs) | 518 | DWARF debug info emission (started — see roadmap Phase 5). |

---

## Source — Advanced / Experimental (src/)

Specialized subsystems: embedded, hot reload, autograd, tensor, debugger, tools.

| File | LOC | Description |
|------|----:|-------------|
| [embedded.rs](src/embedded.rs) | 622 | Embedded systems: `FixedBufferAllocator`, `RealTime` effect, HAL abstractions. |
| [hotreload.rs](src/hotreload.rs) | 563 | Hot reload: native JIT (ELF64) + function-level change detection (`watch` command). |
| [autograd.rs](src/autograd.rs) | 32 | Tape-based reverse-mode autograd for `grad` blocks (mostly stubs — real impl in codegen_c). |
| [tensor.rs](src/tensor.rs) | 609 | Tensor type and operations support. |
| [inc.rs](src/inc.rs) | 1001 | Incremental compilation cache (in-memory, hash-keyed by path+hash+flags). |
| [debugger.rs](src/debugger.rs) | 965 | Interactive IR-level debugger (`debug` command): breakpoints, step, locals. |
| [native_debugger.rs](src/native_debugger.rs) | 313 | Native ptrace-based debugger (`autopsy` command, Linux only). |
| [ark.rs](src/ark.rs) | 881 | Package manager library: init, add, install, remove, list, lock, search, publish. |
| [scribe.rs](src/scribe.rs) | 882 | Documentation generator library: HTML/Markdown from .fd source. |
| [veil.rs](src/veil.rs) | 367 | Code coverage tool library: function/line/instruction coverage. |
| [flat_ast.rs](src/flat_ast.rs) | 784 | Flat AST serialization (`dump-ast-flat` / `diff-test`). |
| [green_thread.rs](src/green_thread.rs) | 600 | Green-thread scheduler (interp-backed). |
| [lib.rs](src/lib.rs) | 69 | Library crate root — shared by all 4 binaries. |
| [bin/ark.rs](src/bin/ark.rs) | — | `ark` binary entry point. |
| [bin/scribe.rs](src/bin/scribe.rs) | — | `scribe` binary entry point. |
| [bin/veil.rs](src/bin/veil.rs) | — | `veil` binary entry point. |

---

## Standard Library (stdlib/)

403 Fid source modules (~259K lines). `core.fd` is auto-included by the
compiler; others are imported via `use`. The table below lists the
foundational modules only — the full set spans many more domains
(networking incl. HTTP/3+QUIC and TLS 1.3, databases, post-quantum crypto,
industrial/energy protocols, automotive/robotics, kernel drivers and
embedded peripherals, media, ML, mobile/desktop interop; grouped list in
`AGENTS.md` → Current Baseline → STDLIB). Per-module docs live in
`docs/stdlib/` (generated by `scribe`: 123 module pages for the 403 modules
on Nov 21; `scripts/stdlib-docs.sh` regenerates them, `--check` compares). Native runtimes: `stdlib/pe_runtime/` (4 files, native
Windows) and `stdlib/linux_runtime/` (7 files: Linux targets, `--nat`
Windows, `darwin.fd`, `wasi.fd`).
`kernel.fd`/`uefi.fd` moved to the `fides-os` project.
See `docs/reference/features.md` for which are feature-complete.

| File | Description |
|------|-------------|
| [core.fd](stdlib/core.fd) | Core stdlib (auto-included): `Option<T>`, `Result<T,E>`, `println`, `assert`, `assert_eq`. Option/Result combinators: map, and_then, or_else, unwrap_or_else, expect. |
| [vec.fd](stdlib/vec.fd) | `Vec<T>` container, higher-order methods (map, filter, fold), iterator adapters (take, skip, zip, enumerate, flat_map, chunk, window, dedup, sort_by, contains_val). |
| [channel.fd](stdlib/channel.fd) | Bounded MPSC channels: `channel_create`, `channel_send`, `channel_recv`, `channel_try_recv`, `channel_close`, `channel_is_closed`. |
| [math.fd](stdlib/math.fd) | Math functions: sqrt, sin, cos, floor, ceil, pow, fabs, constants. Vec2/3/4, mat4, quat types with operator overloading + swizzle. |
| [ecs.fd](stdlib/ecs.fd) | Entity-Component-System runtime (Phase 8B: F18). |
| [events.fd](stdlib/events.fd) | Event system / handlers (Phase 8B: F20). |
| [parallel.fd](stdlib/parallel.fd) | Auto-parallelization framework (Phase 5C.4). |
| [sync.fd](stdlib/sync.fd) | Concurrency primitives. |
| [db.fd](stdlib/db.fd) | Database driver framework (Phase 5D.4). |
| [http.fd](stdlib/http.fd) | HTTP server (Phase 5D.3). |
| [json.fd](stdlib/json.fd) | JSON parsing/serialization (Phase 5D.2). |
| [shape.fd](stdlib/shape.fd) | Comptime tensor shape verification (Phase 5C.3). |
| [simd.fd](stdlib/simd.fd) | SIMD primitives (Phase 5C.2). |
| [tensor.fd](stdlib/tensor.fd) | Tensor type and ops (Phase 5C.1). |
| [quant.fd](stdlib/quant.fd) | Quantization (Phase 5E.1). |
| [money.fd](stdlib/money.fd) | Fixed-point money type, arithmetic, formatting (Phase 7.5B). |
| [banking.fd](stdlib/banking.fd) | Accounts, transfers, double-credit/lost-money detection (Phase 7.5B). |
| [alloc.fd](stdlib/alloc.fd) | Allocator abstractions (Phase 10A). |
| [file.fd](stdlib/file.fd) | File I/O via file descriptors. |
| [time.fd](stdlib/time.fd) | LogicalTime — deterministic, clock-free date math (Phase 7.5A, F43). |
| [typed_strings.fd](stdlib/typed_strings.fd) | SQLString/ShellString/HTMLString/JSONString/PathString — injection prevention (F67). |
| [memo.fd](stdlib/memo.fd) | Memoization helper: `Memo::new()`, `get_or_compute()`. |
| [privacy.fd](stdlib/privacy.fd) | Private field access test module. |
| [env.fd](stdlib/env.fd) | Environment variable access: `env_get`, `env_exists`. |
| [args.fd](stdlib/args.fd) | Command-line arguments: `args_len`, `args_get`, `args_program_name`. |
| [process.fd](stdlib/process.fd) | Process control: `process_exit`, `process_system`. |
| [regex.fd](stdlib/regex.fd) | Basic wildcard pattern matching. |
| [tilemap.fd](stdlib/tilemap.fd) | Tilemap game stdlib (Phase 8D S1). |
| [sprite.fd](stdlib/sprite.fd) | Sprite game stdlib (Phase 8D S2). |
| [flowfield.fd](stdlib/flowfield.fd) | Flowfield game stdlib (Phase 8D S3). |
| [streaming.fd](stdlib/streaming.fd) | Streaming game stdlib (Phase 8D S4). |
| [scene.fd](stdlib/scene.fd) | Scene graph game stdlib (Phase 8D S5). |
| [physics.fd](stdlib/physics.fd) | Physics game stdlib (Phase 8D S6). |
| [audio.fd](stdlib/audio.fd) | Audio game stdlib (Phase 8D S7). |
| [input.fd](stdlib/input.fd) | Input handling game stdlib (Phase 8D S8). |
| [rc.fd](stdlib/rc.fd) | Reference counting: `rc_new`, `rc_clone`, `rc_drop`, `rc_get`, `rc_count` (and Arc equivalents). |
| `kernel.fd` | *(moved)* — OS kernel primitives live in the `fides-os` project. |
| `uefi.fd` | *(moved)* — UEFI boot services bindings live in the `fides-os` project. |

---

## Tests (tests/)

3,672 `.fd` test files and 252 shell check scripts (Nov 21). Run the
bootstrap suite with `./target/release/fidesc test` (or `fid test`);
verify with `./verify.sh` (bootstrap), `./verify-selfhost.sh` (selfhost),
`scripts/tier1.sh` / `tier2.sh` (CI lanes).

| Directory | Contents |
|-----------|----------|
| [bootstrap/](tests/bootstrap/) | The main suite: 1,682 `.fd` at top level (456 `*_fail` expected-fail tests that must be rejected) + 71 in subdirs: `at_import`, `baseline`, `capability`, `diag_dup`, `diagnostics`, `effects`, `golden`, `nattools`, `race_lib`, `regions`, `safety`, `spec_lib`, `trait_conflict`. |
| [conformance/](tests/conformance/) | 1,596 `.fd` — one test per rule of `docs/reference/reference.md` §1–§13; `//! error:` means the compiler must reject it. `_modules/` helpers + `known-gaps.txt`. |
| [selfhost/](tests/selfhost/) | 57 `.fd` exercising self-hosted components (`verify-selfhost.sh`). |
| [nattools/](tests/nattools/) | 68 `.fd` + machine-layer check scripts (RA splitting, unwinding, encoders). |
| [projects/](tests/projects/) | 52 `.fd` whole-program fixtures: cli, service, game, embed, library, workspace, dep graphs. |
| [verify/](tests/verify/) | 50 `.fd` — bug reproductions (`bugs/`), soak/interleave programs, `.tvp` translation-validation pairs. |
| [tooling/](tests/tooling/) | 14 `.fd` + 118 `*-check.sh` (126 `.sh` in all) — per-feature tool checks (crash reports, LSP probes, caches, repl, docs, `serve`). |
| [leaks/](tests/leaks/) | 10 programs + `.expected` (`scripts/leak-check.sh`: output + "every block was freed"). |
| [harness.rs](tests/harness.rs) | Rust integration test harness — runs `.fd` tests across backends, holds the skip list (`is_skip_test()`). |
| other dirs | `ark`, `book`, `c2fd`, `cancel`, `cascade`, `checked_alloc`, `course`, `ct`, `ctx_switch`, `db`, `determinism`, `fmt`, `fuzz`, `limits`, `memprof`, `mono_share`, `net`, `par`, `perf`, `prof`, `rc_cancel`, `sched`, `tls`, `tools`, `tp`, `uefi`, `wasm` — domain-specific checks with their own `check.sh`/`run.sh`. |

### Current Test Results

**See `AGENTS.md` → "Current Baseline"** for authoritative test counts.
Baseline file: `tests/bootstrap/baseline/test_results.txt`.

> Full test inventory: see `tests/` directory listing, or run
> `./target/release/fidesc test` to see all test names and results.

---

## Scripts (scripts/)

~100 scripts; the ones you reach for first:

| File | Description |
|------|-------------|
| [verify.sh](verify.sh) | *(root)* Full verification: build + test + compare to baseline. `--save` updates baseline, `--quick` is build-only. `--golden` is currently broken (the helper script it calls is missing). |
| [verify-selfhost.sh](verify-selfhost.sh) | *(root)* tests/selfhost/ on 3 backends (interp, in-process C, in-process PE). |
| [bootstrap.sh](scripts/bootstrap.sh) | Multi-stage bootstrap: builds c1…cN and checks the C fixpoint. `RELEASE=1` builds 3 stages, installs `release/fid.exe` and smoke-tests it from an installed layout. |
| [build-selfhost.sh](scripts/build-selfhost.sh) | Build the self-hosted compiler into `release/` — produces `release/fid-dev.c` + `release/fid-dev(.exe)`. |
| [tier1.sh](scripts/tier1.sh) / [tier2.sh](scripts/tier2.sh) | The CI lanes run by `.github/workflows/tier1.yml`/`tier2.yml` (what each lane checks: `docs/reference/tier-checks.md`). |
| [cross-test.sh](scripts/cross-test.sh) | Build + run the suite for a cross target under qemu/Node; `NATIVE=1` uses `fid emit` instead of C. |
| [leak-check.sh](scripts/leak-check.sh) | tests/leaks: output + "every block was freed" gate. |
| [wasm-test.sh](scripts/wasm-test.sh) | The suite through `fid wasm` under Node (`ALL=1` for everything). |
| [macho-check.py](scripts/macho-check.py) | Structural check of Mach-O executables (native macOS path). |
| [enc-oracle.py](scripts/enc-oracle.py) | Compares nat_enc's encoders against clang's assembler. |
| [pass-matrix.sh](scripts/pass-matrix.sh) | Every native pass off in turn: results must be identical. |
| [crater.sh](scripts/crater.sh) | Bulk-compile a corpus (crater-style regression hunting). |
| `bisect.sh`, `gen_golden.sh`, `test_build.sh` | **Do not exist** — older docs still reference them; bisect manually via `dump-*`/`codegen --peephole` and regenerate golden files by hand. `fid reduce` shrinks a failing program for you. |

---

## Examples (examples/)

12 complete programs (`examples/README.md` describes each):

| File | Description |
|------|-------------|
| [http_server.fd](examples/http_server.fd) | HTTP/1.1 server. |
| [http2_server.fd](examples/http2_server.fd) | HTTP/2 server. |
| [json_parser.fd](examples/json_parser.fd) | JSON parser. |
| [lexer.fd](examples/lexer.fd) | A lexer. |
| [parser.fd](examples/parser.fd) | A parser. |
| [mini_compiler.fd](examples/mini_compiler.fd) | A small compiler front end. |
| [codegen.fd](examples/codegen.fd) | Code generation demo. |
| [neural_network.fd](examples/neural_network.fd) | Neural network forward pass in pure Fid. |
| [minimal_game.fd](examples/minimal_game.fd) | Game loop (SDL2). |
| [sdl_window.fd](examples/sdl_window.fd) | SDL window + rendering. |
| [riscv_blink.fd](examples/riscv_blink.fd) | Bare-metal LED blink for SiFive HiFive1 RISC-V. |
| [rpi_led_blink.fd](examples/rpi_led_blink.fd) | Bare-metal LED blinker for Raspberry Pi 3 ARM64. |

---

## Tools (tools/)

Standalone tools written in Fid (each builds to its own exe; the
self-hosted ones ship beside `fid`):

| File | Description |
|------|-------------|
| [ark.fd](tools/ark.fd) | `ark` package manager — self-hosted build (`ark-fd.exe`). |
| [c2fd.fd](tools/c2fd.fd) | C → Fid source translator (best effort). |
| [cas.fd](tools/cas.fd) | Remote cache server for `.fidcache` (inc-roadmap A11). |
| [check_stdlib.fd](tools/check_stdlib.fd) | Imports every stdlib module on its own — the stdlib smoke check. |
| [doctest.fd](tools/doctest.fd) | Extracts `/// Example:` blocks from `.fd` sources and runs them. |
| [fuzz_triage.fd](tools/fuzz_triage.fd) | Summarises the failures a fuzz run kept. |
| [macho_check.fd](tools/macho_check.fd) | Structural check of a 64-bit Mach-O executable. |
| [release_checksums.fd](tools/release_checksums.fd) | SHA-256 checksums of release files. |
| [release_notes.fd](tools/release_notes.fd) | Release notes generated from `docs/reference/changelog.md`. |
| [scribe.fd](tools/scribe.fd) | `scribe` documentation generator — self-hosted build (`scribe-fd.exe`). |
| [site.fd](tools/site.fd) | The project website, served by a Fid program (AD2). |
| [veil.fd](tools/veil.fd) | `veil` coverage tool (AD1, release N8). |

---

## Benchmarks (bench/)

Benchmark programs in Fid, Rust, Zig, C++ and Go, organised by domain
(`alloc`, `coldstart`, `game`, `idle`, `json`, `memops`, `mixed`,
`service`, `size`, `work`; the fib/loop/sieve/hashmap programs in `fides/`,
`rust/`, `zig/`, `cpp/`, `go/`). `bench/run.sh` runs the comparison, `bench/check.sh` gates it,
`bench/compile-speed.sh`/`compile-check.sh` cover compile time.
Results: `docs/benchmarks.md` (fib through C ~0.81 s vs Rust 0.53 s, Nov 2).

---

## Build Artifacts & Generated Files

These are outputs, not source. Safe to delete; regenerated by builds/tests.
All are gitignored (see `.gitignore`).

| File | Description |
|------|-------------|
| `target/` | Cargo build output (debug + release binaries). |
| `release/fid(.exe)` | The shipped self-hosted compiler (stage-3 fixpoint build; `RELEASE=1 scripts/bootstrap.sh`). ⚠ The binaries on disk may predate the tree — if `fid` fails with "stdlib/core.fd not found or parse error", rebuild via `scripts/build-selfhost.sh`. |
| `release/fidesc(.exe)`, `release/ark(.exe)`, `release/scribe(.exe)`, `release/veil(.exe)` | Rust-built bootstrap compiler and tools; `ark-fd`/`scribe-fd` are the self-hosted builds of the same tools (sources in `tools/`). |
| `release/fid-dev.c`, `release/fid-dev(.exe)`, `release/fid-dev.log`, `release/fid-dev.cc.log`, `release/fid-dev2*` | Dev self-compile artifacts + build logs (`./scripts/build-selfhost.sh`). `release/archive/` keeps older generations for diffing. |
| `tests/*.c` | Generated C output from test runs (auto-generated from `.fd` files). |
| `tests/bootstrap/test_async`, `tests/bootstrap/test_float`, `tests/bootstrap/test_generics` | Compiled native test binaries. |
| `tests/selfhost/*` (extensionless) | Compiled self-host probe/harness binaries (built from `.c` sources). |
| `tests/bootstrap/golden/*/` | Per-test golden output subdirectories (regenerate manually — `gen_golden.sh` does not exist). |
| `output.s` | Generated assembly output. |
| `_ast.txt`, `_ir.txt`, `_ir2.txt` | Dumped AST/IR snapshots (debugging artifacts). |
| `out.txt` | Empty output capture file. |

---

## How to Use This Index

- **New to the codebase?** Read [README.md](README.md) → [reference/architecture.md](docs/reference/architecture.md) → [AGENTS.md](AGENTS.md).
- **Looking for a specific feature?** Check [reference/features.md](docs/reference/features.md) and [reference/syntax.md](docs/reference/syntax.md).
- **About to edit the compiler?** Read [AGENTS.md](AGENTS.md) — especially the high-risk file table. Run `./verify.sh` before and after.
- **A test broke?** Bisect manually with `dump-ir` / `opt` / `dump-mir` / `disasm` (no bisect script exists).
- **Adding a new file?** Add a row to the relevant table above so this index stays current.
