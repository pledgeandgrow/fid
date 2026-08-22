# Fides Compiler — Best Practices & Lessons Learned

> Hard-won insights from building a custom toolchain. Read this before making
> changes. Every rule here exists because something broke.

*Last updated: Aug 2026*

---

## 1. The Golden Rule: Verify Before and After

**Never make a change without knowing the baseline.**

```bash
# Before touching anything
./verify.sh

# Make your change

# After
./verify.sh
```

If you skip the "before" step, you can't tell if a failure is your fault or
pre-existing. If you skip the "after" step, you ship regressions.

**Lesson learned:** We spent 2 hours debugging a "small change" to the register
allocator that broke 30+ tests. The change was correct, but we had no baseline
to compare against, so we couldn't distinguish real regressions from tests that
were passing for the wrong reasons.

---

## 2. Small Changes, Big Blast Radius

### The Risk Hierarchy

| File | Blast Radius | Why |
|------|-------------|-----|
| `regalloc.rs` | **EVERY function** | Changes how all VRegs get registers/spills |
| `peephole.rs` | **EVERY instruction** | Always on in `build`; wrong transform = silent corruption |
| `pir.rs` (L2→L3) | **ALL backends** | Bad lowering propagates to x86-64, ARM64, RISC-V |
| `elf64.rs` / `pe.rs` | **ALL binaries** | Byte-level format; off-by-one = corrupt output |
| `codegen_x64.rs` | **ALL x86-64 output** | Instruction encoding; wrong ModRM = wrong code |
| `lower.rs` | **ALL backends** | AST→IR; wrong lowering = wrong everything |
| `typechecker.rs` | **ALL programs** | Wrong rejection = broken code; wrong acceptance = silent miscompilation |
| `runtime_linux.rs` | **ALL Linux binaries** | Hand-encoded machine code; wrong byte = crash |

### The Rule

**The more code your change affects, the smaller the change should be.**

- Changing `regalloc.rs`? One-line fix, then `./verify.sh`.
- Changing `lexer.rs`? You can make a bigger change — errors are loud.

### Anti-Pattern: The "While I'm Here" Fix

```rust
// BAD: fixing regalloc AND refactoring codegen in the same change
// If tests break, you don't know which change caused it

// GOOD: fix regalloc, verify, commit. Then refactor codegen, verify, commit.
```

---

## 3. The Register Allocator Is The Hardest Part

### Known Bug

The linear scan register allocator (`regalloc.rs`) has a pre-existing bug:
**constants not loaded into registers before use can be clobbered by
intervening instructions.** This is exposed by the L2 optimizer's instruction
reordering.

### What This Means

- The PIR optimizer (`opt.rs`) is **disabled by default** because of this bug
- `build` (without `--opt`) works because peephole doesn't reorder
- `build --opt` can produce wrong output on complex programs

### Rules For Editing regalloc.rs

1. **Always run `./verify.sh` after** — not just `cargo build`
2. **Test with and without `--opt`** — the bug only manifests with reordering
3. **Gate risky changes behind a flag** during development:
   ```rust
   const NEW_SPILL_LOGIC: bool = true;
   if NEW_SPILL_LOGIC { /* new code */ } else { /* old code */ }
   ```
4. **The spill offset arithmetic is fragile** — `spill_offset` must be 8-byte
   aligned, and `spill_size` in the returned `Allocation` must match what the
   codegen prologue expects. Off-by-8 here = stack corruption.

### Lesson Learned: The +8 Bug

We had `spill_offset` starting at `func.frame_size`, but the codegen prologue
was using `-(frame_size + 8)` for the first spill slot. This mismatch caused
stack corruption. The fix was `spill_offset = func.frame_size + 8`, but it
took hours to find because the symptom was "random tests fail" not "stack
alignment error."

**Rule:** When changing spill offsets, check BOTH the allocator AND the codegen
prologue. They must agree on the stack layout.

---

## 4. Peephole Optimizer: Correctness Over Cleverness

### The Trap

Peephole passes are tempting to "improve" because they're simple pattern
matches. But a wrong transform is **silent** — the binary is valid, it just
does the wrong thing.

### Known Issues (Now Fixed)

1. **LEA coalescing across calls**: `pass_lea_coalesce` didn't clear tracking
   state across `call` instructions. A call clobbers caller-saved registers,
   invalidating the LEA→register forwarding. **Fix:** clear `last_lea` on any
   `call`/`callq`.

2. **Store-to-load forwarding**: matched register-to-register moves as
   store-load pairs, incorrectly eliminating stores that were needed.

3. **Dead store elimination**: didn't check if the next instruction reads from
   the destination as a memory address (e.g., `movq %rax, -8(%rbp)` followed
   by `movq -8(%rbp), %rax` — the store IS needed if the load uses it as an
   address). **Fix:** check if the next instruction's source contains the
   destination register or uses it as a memory operand.

### Rules For Editing peephole.rs

1. **Every pass must handle `call`/`callq`** — calls clobber caller-saved regs
2. **Every pass must handle memory operands** — `(%reg)` reads are different
   from `%reg` reads
3. **Test with `./verify.sh`** — peephole is always on in `build`, so any bug
   affects the primary compilation path
4. **When in doubt, be conservative** — skipping a valid optimization is safe;
   applying an invalid optimization is catastrophic

---

## 5. PIR L2→L3: The Field Offset Trap

### The Bug

`pir.rs` L2→L3 lowering of `GetFieldPtr` uses `offset: field_index * 8`,
assuming all struct fields are 8 bytes. This works for the current type system
(where everything is pointer-sized) but will break when real field layouts are
implemented.

### The Proper Fix

Use `StructLayout` (like `codegen_c.rs` does) to compute real byte offsets:
```rust
// Instead of:
offset: *field_index * 8,
// Should be:
offset: struct_layouts.get(struct_name)
    .map(|l| l.field_offset(*field_index))
    .unwrap_or(*field_index * 8),  // fallback
```

### Rule

**Never hardcode size assumptions.** The IR type system has `IrType::size()`
and `StructLayout` for a reason. Use them. If `size()` returns 0 (placeholder),
fix `size()`, don't work around it with magic numbers.

---

## 6. Debug Prints: Use Them, Then Remove Them

### The Pattern

During debugging, add targeted `eprintln!` statements:

```rust
// In compiler.rs
eprintln!("DBG L3 apply: frame_size={}", f.frame_size);
for inst in &b.instructions {
    eprintln!("  {:?}", inst);
}

// In elf64.rs
if src == "%edx" && dst == "%edx" {
    eprintln!("DBG ALU: bytes={:02x?}", &self.text[text_len_before..]);
}
```

### The Rule

**Debug prints are temporary. Remove them before committing.**

They pollute stderr, confuse the test harness (which checks stderr for
"Panic:" and "error:"), and make the output noisy for users.

### The Exception

If a debug print is genuinely useful for ongoing development, gate it behind
an environment variable:

```rust
if std::env::var("P_DEBUG").is_ok() {
    eprintln!("DBG L3: ...");
}
```

---

## 7. Non-Deterministic Output

### The Problem

The compiler has non-deterministic output due to `HashMap`/`HashSet` iteration
order. Same source → different binary on each compile. This affects:

- String constant numbering (`.str_0`, `.str_1`, ...)
- Struct definition order in IR output
- Effect list order in function signatures (`!write,alloc,read` vs `!read,write,alloc`)
- Stack slot offsets in assembly

### Why It Matters

- **Reproducible builds are impossible** — a key feature for a systems language
- **Golden file comparison is noisy** — the `bisect.sh` script normalizes this,
  but it's a band-aid
- **Caching is broken** — if output changes, you can't cache compilation

### The Fix (When You Have Time)

Replace `HashMap` with `IndexMap` (preserves insertion order) or sort keys
before output. Replace `HashSet` with `BTreeSet` (sorted). This is a 30-60
minute fix across `lower.rs`, `codegen_x64.rs`, `typechecker.rs`, `ir.rs`.

### Rule

**Don't add new `HashMap`-based collections that affect output order.** If you
need a map for output, use `BTreeMap` or `IndexMap`.

---

## 8. Testing Discipline

### The Test Pyramid

```
                    ┌─────────────┐
                    │  End-to-end │  fidesc test (149 .fd files)
                    │   tests     │  ← tells you SOMETHING broke
                    └──────┬──────┘
                  ┌────────┴────────┐
                  │  Golden files   │  bisect.sh (IR/PIR/asm per test)
                  │  (intermediate) │  ← tells you WHICH LAYER broke
                  └────────┬────────┘
                ┌──────────┴──────────┐
                │   Unit tests        │  cargo test (133 tests)
                │   (per-module)      │  ← tells you WHICH FUNCTION broke
                └─────────────────────┘
```

### The Workflow

1. **Write a failing test first** — if you're fixing a bug, write a `.fd` test
   that exposes it before fixing the code
2. **Fix the bug** — the test should now pass
3. **Run `./verify.sh`** — make sure nothing else broke
4. **If something broke, bisect** — `./scripts/bisect.sh <test_name>`
5. **Update golden files** — `./scripts/gen_golden.sh` if codegen changed

### What Tests Don't Catch

- **Silent miscompilation** — output is "correct" but for the wrong reason
- **Non-deterministic bugs** — fail on one run, pass on the next
- **ABI violations** — wrong calling convention, but simple tests don't expose it
- **Edge cases in instruction encoding** — only triggered by specific register combinations

### Rule

**The test suite tells you what works, not what's correct.** A passing test
means the output matched — it doesn't mean the code path is sound. Always
review the actual assembly/IR when changing codegen.

---

## 9. ABI Awareness

### Two ABIs In Play

| Platform | ABI | Args | Return | Callee-saved | Caller-saved |
|----------|-----|------|--------|-------------|-------------|
| Linux | System V AMD64 | RDI, RSI, RDX, RCX, R8, R9 | RAX | RBX, RBP, R12-R15 | RAX, RCX, RDX, RSI, RDI, R8-R11 |
| Windows | Win64 | RCX, RDX, R8, R9 + shadow space | RAX | RBX, RBP, RDI, RSI, R12-R15 | RAX, R10, R11, RCX, RDX, R8, R9 |

### Common Mistakes

1. **Forgetting shadow space on Windows** — Win64 requires 32 bytes of shadow
   space above the return address, even if you don't use it
2. **Not saving caller-saved regs across calls** — if you put a value in RAX
   and then call a function, RAX is clobbered
3. **Stack alignment** — must be 16-byte aligned before `call` on both ABIs
4. **Using R10/R11 on Windows** — they're caller-saved on both ABIs, but
   Windows doesn't guarantee their values across calls

### Rule

**When touching codegen or regalloc, always check: "Does this preserve the
ABI?"** The test suite won't catch ABI violations on simple programs because
the clobbered registers aren't in use.

---

## 10. The Interpreter Is The Oracle — Until It Isn't

### The Good

The interpreter (`interp.rs`) is the reference implementation. All backends
are validated against it. This is a great architecture — you always know what
"correct" output is.

### The Bad

If the interpreter has a bug, all backends "match" but produce wrong results.
The oracle can be wrong.

### Known Interpreter Issues

- Prints Rust debug format for Vec/Map/Ptr (causes `golden_files_match` harness
  test to fail)
- Doesn't implement all runtime stubs that native backends need

### Rule

**When the interpreter and a backend disagree, don't assume the backend is
wrong.** Check the interpreter first. If the interpreter is wrong, fixing it
will change the "correct" output for all backends.

---

## 11. Commit Hygiene

### What To Commit

- Source code changes
- Test files (new `.fd` tests)
- Updated baseline (`tests/baseline/test_results.txt`)
- Updated golden files (`tests/golden/`)

### What NOT To Commit

- Debug `eprintln!` statements
- Temporary test scripts in `/tmp/`
- Generated executables (`.exe`, `.o`, `.wasm`)
- The `target/` directory (already in `.gitignore`)

### Commit Message Format

```
fix(regalloc): spill pre-assigned arg registers when clobbered by calls

When a function parameter is in a caller-saved register and a call
clobbers it during the parameter's live range, spill the parameter
to the stack instead of keeping it in the register.

Also update arg_locations so the codegen prologue saves the arg
register to the spill slot.
```

Format: `type(scope): description` where type is one of:
- `fix` — bug fix
- `feat` — new feature
- `refactor` — code restructuring
- `docs` — documentation
- `test` — test additions/changes
- `opt` — optimization

---

## 12. When You're Stuck

### Debugging Hierarchy

1. **Run `./verify.sh`** — see the exact failure pattern
2. **Run `./scripts/bisect.sh <test_name>`** — find which layer broke
3. **Add targeted `eprintln!`** — dump the IR/PIR/asm at the failing point
4. **Compare to interpreter output** — is the interpreter also wrong?
5. **Read the assembly** — `fidesc codegen <file> --target x86-64` and read it
6. **Minimize the test case** — reduce the failing `.fd` file to the smallest
   reproducer
7. **Check the ABI** — is a register being clobbered? Stack misaligned?

### Minimizing Test Cases

```fides
// If this 100-line file fails, reduce to:
pf main() {
    pv x = 42;
    pv y = add(x, 1);
    print(y);
}

pf add(a: i64, b: i64) -> i64 {
    a + b
}
```

The smaller the reproducer, the faster you find the bug.

### When To Ask For Help

- You've spent >30 minutes on a regalloc issue — these are notoriously hard
- The bug only manifests with `--opt` — the optimizer exposes latent issues
- The assembly looks correct but the output is wrong — likely an ABI issue
- The same code produces different output on different runs — non-determinism

---

## 13. File Editing Rules

### Before Editing Any High-Risk File

1. Read `AGENTS.md` — check the risk level and known bugs
2. Run `./verify.sh` — confirm baseline
3. Understand the existing code — read the function you're changing AND its callers
4. Check for debug prints — remove old ones before adding new ones

### After Editing

1. Run `./verify.sh` — check for regressions
2. If regressions: `./scripts/bisect.sh <failing_test>`
3. If no regressions: `./verify.sh --save` to update baseline
4. If you changed codegen: `./scripts/gen_golden.sh` to update golden files

### The "Three Files" Rule

**Never edit more than 3 files in one change.** If you need to touch 4+ files,
break it into multiple changes. The blast radius of a 4-file change is too
large to debug when something breaks.

---

## 14. Known Fragile Areas (Technical Debt)

These are areas where the code works but is fragile. Changes here require
extra care.

### 14.1 Stack Frame Layout

The stack frame layout is split across multiple files:
- `regalloc.rs` decides spill offsets
- `codegen_x64.rs` generates the prologue/epilogue
- `runtime_linux.rs` has hardcoded offsets for its own frame

**If you change one, check the others.** The `+8` bug (section 3) was caused
by `regalloc.rs` and `codegen_x64.rs` disagreeing on the starting offset.

### 14.2 Closure Environment Passing

Closures with captured environment use a `has_env` flag in `CallPtr`. The env
pointer is passed as the first argument (RDI on Linux, RCX on Windows), and
the real function pointer is loaded from `env[0]`.

**If you change `CallPtr` in codegen, check:**
- `pir.rs` — where `has_env` is set
- `codegen_x64.rs` — where `gen_call_ptr` handles the env pointer
- `interp.rs` — where closures are interpreted

### 14.3 String Constant Interning

String constants are interned in `lower.rs` using a `HashMap`, which means
the numbering (`.str_0`, `.str_1`, ...) is non-deterministic. All backends
refer to strings by these numbers.

**If you change string interning, all backends are affected.**

### 14.4 Effect Inference

Effect inference in `typechecker.rs` uses fixpoint iteration over the call
graph. The order of effects in the output (`!IO,Net` vs `!Net,IO`) is
non-deterministic (HashSet).

**If you change effect inference, check:**
- The output format in `ir.rs` (effects are printed in function signatures)
- The `verify_declared_effects` function — it compares sets, not ordered lists

### 14.5 ELF64 Relocation Types

`elf64.rs` handles three relocation types: `R_X86_64_PC32`, `R_X86_64_PLT32`,
`R_X86_64_64`. The `PLT32` type is treated the same as `PC32` (relative call)
in most cases, but this is a simplification.

**If you add new relocation types, test with `fidesc build` and
`fidesc codegen --elf`.**

---

## 15. The "Works On My Machine" Trap

### The Problem

The test suite runs on Linux. The PE emitter produces Windows binaries that
can't be executed on Linux (without Wine). So PE tests only check that
emission succeeds, not that the binary runs correctly.

### What This Means

- PE emitter bugs that produce invalid-but-emittable binaries are not caught
- The PE emitter could silently produce broken `.exe` files and we wouldn't know
- The ELF64 backend is the only one with true end-to-end testing on Linux

### Rule

**When editing `pe.rs`, be extra careful.** You can't rely on the test suite
to catch runtime errors. Read the PE spec, check your byte offsets, and
compare with the ELF64 emitter's approach (they share similar structure).

---

## Quick Reference Card

```
BEFORE editing:     ./verify.sh
AFTER editing:      ./verify.sh
If regressions:     ./scripts/bisect.sh <test_name>
If no regressions:  ./verify.sh --save
If codegen changed: ./scripts/gen_golden.sh

HIGH-RISK FILES (always verify):
  regalloc.rs, peephole.rs, elf64.rs, pe.rs, codegen_x64.rs,
  pir.rs, runtime_linux.rs, lower.rs, typechecker.rs

MAX 3 files per change.
MAX 1 bug fix per change.
Remove debug prints before committing.
```
