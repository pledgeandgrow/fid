# Fides Compiler Internals

> A map of `compiler/*.fd` for contributors. The self-hosted compiler is the
> product: `release/fid.exe` is built from these files by
> `RELEASE=1 ./scripts/bootstrap.sh` (3 stages, fixpoint-checked).
> The Rust code in `src/` is a frozen bootstrap reference, not the source of
> truth. Read `AGENTS.md` first.

## The pipeline

```
source .fd
   |  lexer.fd / token.fd            tokens
   |  parser.fd / ast.fd             AST
   |  (import resolution in compiler.fd: `use x;` -> stdlib/, `use compiler::x;` -> compiler/)
   |  typechecker.fd                 types, generics pre-check
   |  borrowck.fd                    borrow rules (K4/K5); --borrowck=on|warn|off (default on)
   |  escape.fd / secrecy.fd         reference escape check, @secret info flow
   |  mono.fd                        monomorphization
   |  lower.fd                       IrModule (IrFunction + Inst); annotations -> IrFunction flags
   |  iropt.fd / iropt_me.fd         mid-end passes (module + per-function)
   |  rules.fd                       effects-based rewrites, pure/read CSE, lib-facts consumer
   v
   backend (one of):
     codegen_c.fd                   C text -> zig cc / cl / tcc      (`fid ccompile`)
     pe_ir.fd / pe_ir_stubs.fd      direct PE .exe, Windows          (`fid emit`, default)
     nat_ir.fd / nat_enc.fd         direct ELF, x86_64/aarch64/riscv64 Linux (`emit --target *-linux`)
     codegen_x64/arm64/riscv.fd     asm text paths (`fid codegen --target`)
     wasm.fd                        WASM/WAT (`fid wasm`)
   runtime: runtime.fd (shared), runtime_linux.fd (nat syscalls), pe_runtime/
   linker: linker.fd, elf64.fd, coff.fd, macho.fd
```

Entry point: `compiler/main.fd` — command dispatch (`run`, `emit`, `build`,
`ccompile`, `codegen`, `wasm`, `ir`, `pir`, `dump-*`, `ark`, `scribe`,
`veil`, `audit`, `qualify`, `parse-header`, ...). `compiler/compiler.fd`
holds `build_program*` — lex/parse/check/mono driver.

## The IR (`compiler/ir.fd`)

- `IrModule`: `functions` (`IrFunctionEntry{name, func}`), `extern_functions`,
  `globals`, `c_callbacks`. Helpers: `IrModule_dead_function_elimination`,
  `pe_ir_reachable_functions`, `pe_runtime_alias_target`.
- `IrFunction`: `params: Vec<ValueIdType>`, `insts: Vec<ValueIdInst>`,
  `blocks`, `effects: EffectSet`, and flags — `is_crashfree`, `is_no_alloc`,
  `internal_conv` (WP6: may use the extended arg-register convention).
- `Inst` variants include `Call(name, args, ret, effects)`, `CallPtr`,
  `DynCall(trait, method, receiver, args, ret)`, `MakeDyn`, `ConstFnRef`,
  `ConstInt/ConstStr/ConstBool`, `Load/Store`, `Alloca`, `GlobalAddr`,
  `Jump/Branch` (no edge args — ME3 phis are blocked on this), `Return`.

Every value is an instruction id; defs are single-assignment, so "the def of
v" is `IrFunction_get_inst(f, v)`.

## The mid-end (`compiler/iropt.fd`, `iropt_me.fd`, `rules.fd`)

`iropt_native(module)` orchestrates. Order (see the pass list in the file):

1. `iro_conv_mark` — mark `internal_conv` eligibility (direct-call-only fns).
2. opt-in module passes via `FID_IROPT_ON`: `envtrim` (WP7), `ipcp` (WP13),
   `spec` (WP14 cloning), `deadf` (post-opt DFE). `gconst` folds never-written
   globals. `devirt` rewrites `DynCall` when the receiver's `MakeDyn` is local.
3. per-function passes: `fold`, `simplify`, `branchfold`, `branchfacts`,
   `sccp`, `promote`, `dse`, `dce`, `tailcall`, `gvn`, `licm` (+ `iropt_me`'s
   restructuring passes).
4. `iro_libfacts` — write `FID_LIBFACTS` artifact.

Kill switches: `FID_IROPT_OFF=<csv>` per pass, `FID_IROPT_ON=<csv>` for the
opt-in ones, `FID_NAT_OFF=<csv>` for backend passes, `FID_NAT_ON=crashfree`
for panic-check elision. Diagnostics: `FID_IROPT_STATS`, `FID_RULE_STATS`,
`FID_DEBUG_CSE`, `FID_VERIFY=1` (IR verifier + RA checker).

**Why things are opt-in:** see `docs/verification/miscompile-log.md` —
M9 (IPCP `&mut Vec`), M10 (DFE + runtime aliases), M11 (env-trim +
`pe_runtime_alias` stand-ins). A pass that changed semantics in the field
stays behind a flag until the full suite says otherwise.

`rules.fd`: `EffectSet` ({Alloc, Diverge, Read, Write, IO}) machinery.
`ru_ext_effects` computes externally-visible effects — local `Alloca`
traffic and stores through unescaped allocas are invisible, `Diverge` is
stripped for `@crashfree`, `Alloc` for `@no-alloc`. `ru_pure_cse` merges
pure/read-only calls under dominance + write barriers (typed Vec
bookkeeping — an earlier string-keyed version could crash).

## The native backend (`compiler/nat_ir.fd`, `nat_enc.fd`)

MIR recording (`MInst`/`MBlocks`, `MO_*` opcodes), one register allocator,
call graph + post-order emission, cold-block-last ordering within a
function and cold-function-last ordering across the image (WP17).
`ng_call_mask`/`ng_call_mask_i` = IPRA clobber sets (incl. indirect calls
via `MO_ADDR` trace-back); `ng_libfacts_load` merges `ext` records from
`FID_LIBFACTS_IN`.

Calling conventions: platform ABI always; `internal_conv` functions may use
the extended arg set (x64: +r9 => 5, AArch64: +x13-x15 => 11, RISC-V: +t6
=> 9) on both sides of a direct call. `Return` is single-value — no
multi-register results yet.

## Library-facts artifact (WP20)

`FID_LIBFACTS=<path>` writes:
```
# fid-facts 1
fn <name> params=<N> effects=<csv> [conv=internal]
ext <name> mask=<clobber-bitset> size=<bytes>
```
`FID_LIBFACTS_IN=<path>` consumes it: `fn` effects feed `ru_pure_call` /
`ru_read_call` for callees absent from the module; `ext` masks feed IPRA
for calls to functions outside the module. Readers require the version
header — anything else is rejected whole-file.

## Conventions that will bite you

- **ASCII only in `compiler/*.fd`.** Non-ASCII bytes have caused fixpoint
  divergence before (known-bugs §1.36); there's a good-first-issue to add a
  CI check.
- `HashMap` is `i64->i64`. Don't serialize strings/values through it (the
  `ru_pure_cse` crash came from that). Use typed `Vec`s.
- Runtime alias stand-ins (`vec_raw_*`, `Vec_*` via `pe_runtime_alias_target`)
  get an implicit null `__env` arg inserted by backend builtin routing —
  never trim their signatures or mark them internal-conv.
- Runtime functions reached by name (`p_*`, `pe_*`, `nat_*`, `pert_*`,
  callbacks, `Type_method` dyn targets) must be seeded in reachability/
  exclusions. Every pass that removes or re-signatures functions needs the
  same exclusion list.
- `@crashfree`/`@no-alloc` are **trusted** annotations — the emit path does
  not prove them. Strip-facts/merge uses are fine; check elision is opt-in.

## Building and verifying

```bash
./scripts/build-selfhost.sh            # -> release/fid-dev.exe (single stage, dev)
RELEASE=1 ./scripts/bootstrap.sh       # -> release/fid.exe  (3 stages, fixpoint)
.\release\fid-dev.exe run tests/bootstrap/x.fd           # interpreter
.\release\fid-dev.exe emit x.fd --target x86_64-linux    # nat ELF
.\release\fid-dev.exe emit x.fd                          # PE exe
FID_VERIFY=1 ... emit                  # IR verifier + RA checker
```

Tools live in `tools/`: `tools/ark.fd`, `tools/scribe.fd`, `tools/veil.fd`
are thin `fn main` wrappers over `compiler/ark|scribe|veil.fd` — build them
with `fid emit tools/<name>.fd -o release/<name>.exe`.

## Where to start reading

1. `compiler/main.fd` top-level dispatch, then `compiler/compiler.fd`
   `build_program_with_path`.
2. `compiler/ir.fd` — `IrModule`, `Inst`, `EffectSet`.
3. `compiler/iropt.fd` `iropt_native` — the pass list is the best overview
   of what the mid-end does.
4. `compiler/nat_ir.fd` `nat_generate_image` — MIR -> RA -> emit.
