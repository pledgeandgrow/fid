# Path to Self-Reproduction — Quick Reference

> Companion to `docs/roadmap/selfhost-roadmap.md`.
> The full plan with debugging workflows is there. This is the short version.
>
> Last updated: Sep 3, 2026.

---

## Where We Are

```
✅ Lexer — works (stdlib/core.fd tokenized)
✅ Parser — works (stdlib + user code parsed)
❌ TypeChecker — crashes in map_str_str_get (Vec-of-struct truncation)
⬜ Lowerer — not reached yet
⬜ CCodeGen — not reached yet
```

## The One Root Cause

Almost every crash is the same bug: **the C backend treats structs as
pointers (`struct Foo_s*`), so struct-by-value operations only copy 8 bytes
instead of the full struct.**

This affects:
- **Vec::get(i)** on a Vec of structs → only 8 bytes copied → garbage fields
- **Function returns** → needs SRET (hidden out-param) → missing if not pre-registered
- **Enum variant extraction** → `Inst::Load` with `Struct` type reads 8 bytes
- **Enum literal storage** → struct fields must be stored inline (memcpy), not as pointers

## The 7 Steps to Fixpoint

### Step 1: Fix Vec-of-struct truncation [CURRENT]

Fix `map_str_str_get` crash. The Vec runtime (`p_vec_push`/`p_vec_get`) stores
single 8-byte pointers. For struct elements >8 bytes, only the first 8 bytes
are stored. Fix: heap-allocate structs before pushing. Both lowerers need this.

### Step 2: Port Fides lowerer gaps 1-4

The Rust lowerer has 4 fixes the Fides lowerer is missing. These don't block
`fides-c.exe` (built by Rust), but they block `stage1.exe` (built by `fides-c.exe`).

- Gap 1: Stmt::Let alloca type — use `Struct(name)` not `Ptr(Struct)`
- Gap 2: Stmt::Let store_ty — use `Some(Struct(_))` for struct alloca
- Gap 3: Expr::Ident struct alloca — return alloca pointer directly
- Gap 4: StructLit field store — use `emit_struct_copy` for struct fields

### Step 3: Fix remaining crashes (iterative)

Run → crash → read stack → fix → rebuild → repeat.
Expected: 4-10 more crashes, all the same struct-by-value root cause.

### Step 4: hello.fd compiles without crash

```bash
.\release\fides-c.exe ccompile tests\hello.fd -o release\hello_test
```

### Step 5: hello.fd output is correct

```bash
C:\tcc\tcc.exe -O2 -w -o release\hello_test.exe release\hello_test.c
.\release\hello_test.exe
# Expected: Hello, Fides!50y is big
```

### Step 6: Self-compilation (stage1)

```bash
.\release\fides-c.exe ccompile compiler\main.fd -o release\stage1
C:\tcc\tcc.exe -O2 -w -o release\stage1.exe release\stage1.c C:\tcc\lib\ws2_32.def
```

### Step 7: Fixpoint

```bash
.\release\stage1.exe ccompile compiler\main.fd -o release\stage2
C:\tcc\tcc.exe -O2 -w -o release\stage2.exe release\stage2.c C:\tcc\lib\ws2_32.def
.\release\stage2.exe ccompile compiler\main.fd -o release\stage3
C:\tcc\tcc.exe -O2 -w -o release\stage3.exe release\stage3.c C:\tcc\lib\ws2_32.def
diff release\stage2.c release\stage3.c
# Must be identical
```

## Rebuild Cycle

Every fix requires a full rebuild:

```bash
cargo build --release
.\target\release\fidesc.exe ccompile compiler\main.fd -o release\fides-c
C:\tcc\tcc.exe -O2 -w -o release\fides-c.exe release\fides-c.c C:\tcc\lib\ws2_32.def
.\release\fides-c.exe ccompile tests\hello.fd -o release\hello_test
```

## What Success Looks Like

No crash ≠ working. The three failure modes after crashes stop:

1. **Wrong output** — program runs but prints wrong thing → compare C output against Rust compiler
2. **Fixpoint divergence** — stage2.c ≠ stage3.c → non-determinism (HashMap order, uninit memory)
3. **Correct + stable** — stage2.c == stage3.c → **fixpoint achieved**
