# Path to Self-Reproduction — Quick Reference

> Companion to `docs/roadmap/selfhost-roadmap.md`.
> The full plan with debugging workflows is there. This is the short version.
>
> Last updated: Sep 3, 2026 (session 2).

---

## Where We Are

```
✅ Lexer — works (stdlib/core.fd tokenized)
✅ Parser — works (stdlib + user code parsed)
✅ TypeChecker — works (hello.fd type checks ok)
✅ Lowerer — works (hello.fd lowered to IR)
✅ CCodeGen — works (hello.fd → C code → tcc → correct output)
⬜ Self-compilation — NOT TESTED (fides-c.exe ccompile compiler/main.fd)
⬜ Fixpoint — NOT TESTED
```

**MILESTONE:** `tests/hello.fd` compiles end-to-end and produces correct output:
```
Hello, Fides!50y is big
```

## The One Root Cause (now fixed for hello.fd)

Almost every crash was the same bug: **the C backend treats structs as
pointers (`struct Foo_s*`), so struct-by-value operations only copy 8 bytes
instead of the full struct.**

This affected:
- **Vec::get(i)** on a Vec of structs → only 8 bytes copied → garbage fields
- **Function returns** → needs SRET (hidden out-param) → missing if not pre-registered
- **Enum variant extraction** → `Inst::Load` with `Struct` type reads 8 bytes
- **Enum literal storage** → struct fields must be stored inline (memcpy), not as pointers

All of these are now fixed in both lowerers for the paths exercised by
`tests/hello.fd`. The Fides lowerer gaps (1-5) may still cause crashes during
self-compilation.

**Session 2 additions (Sep 3):**
- **Enum SRET** — `lower_type_sret()` in Rust lowerer converts enum `Generic` → `Struct` at function sig boundaries, fixing dangling alloca for `Result`/`Option` returns. Not yet ported to Fides lowerer (Gap 5).
- **Windows binary stdout** — C codegen sets `_O_BINARY` mode on stdout to prevent `\r\n` doubling.
- **Test fixes** — test_cfg (platform-aware), test_deref_assign (added main), test_error_chain (SRET fix), test_http (binary mode). Test results: 203/203 interp, 192/203 C codegen.

## The 7 Steps to Fixpoint

### Step 1: Fix Vec-of-struct truncation ✅ DONE

Fixed `map_str_str_get` crash. All Vec paths now handle struct elements:
- `vec.set/push/insert` method — boxes struct before `Vec_set`
- `vec.get/pop/first/last` method — returns `word_repr(elem_ty)`
- `vec[i] = struct` indexing — boxes struct before `Vec_set`
- `vec[i]` indexing — returns `word_repr(elem_ty)`
- `vec[i] += x` compound — returns `word_repr(elem_ty)`
- `for x in vec` — returns `word_repr(elem_ty)` + uses `elem_ty` for alloca

### Step 2: Port Fides lowerer gaps 1-5 [TODO — before self-compilation]

The Rust lowerer (in `src/lower.rs`) has 5 fixes the Fides lowerer (in
`compiler/lower.fd`) is missing. These don't block `fides-c.exe` (built by
the bootstrap compiler), but they block `stage1.exe` (built by `fides-c.exe`).

- Gap 1: Stmt::Let alloca type — use `Struct(name)` not `Ptr(Struct)`
- Gap 2: Stmt::Let store_ty — use `Some(Struct(_))` for struct alloca
- Gap 3: Expr::Ident struct alloca — return alloca pointer directly
- Gap 4: StructLit field store — use `emit_struct_copy` for struct fields
- Gap 5: SRET for enum returns — `lower_type_sret()` converts enum Generic → Struct (NEW)

### Step 3: hello.fd compiles without crash ✅ DONE

```bash
.\release\fides-c.exe ccompile tests\hello.fd -o release\hello_test
# ✅ Produces release/hello_test.c (534KB)
```

### Step 4: hello.fd output is correct ✅ DONE

```bash
C:\tcc\tcc.exe -O2 -w -o release\hello_test.exe release\hello_test.c C:\tcc\lib\ws2_32.def
.\release\hello_test.exe
# ✅ Output: Hello, Fides!50y is big
```

### Step 5: Self-compilation (stage1) [NEXT]

```bash
.\release\fides-c.exe ccompile compiler\main.fd -o release\stage1
C:\tcc\tcc.exe -O2 -w -o release\stage1.exe release\stage1.c C:\tcc\lib\ws2_32.def
```

This is the hardest step. The compiler source is 39.5K LOC across 18 modules.
May crash due to Fides lowerer gaps 1-4 or other struct-by-value issues not
exercised by `tests/hello.fd`.

### Step 6: stage1 produces correct output

```bash
.\release\stage1.exe ccompile tests\hello.fd -o release\hello_s1
C:\tcc\tcc.exe -O2 -w -o release\hello_s1.exe release\hello_s1.c C:\tcc\lib\ws2_32.def
.\release\hello_s1.exe
# Expected: Hello, Fides!50y is big
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

1. **Wrong output** — program runs but prints wrong thing → compare C output against bootstrap compiler
2. **Fixpoint divergence** — stage2.c ≠ stage3.c → non-determinism (HashMap order, uninit memory)
3. **Correct + stable** — stage2.c == stage3.c → **fixpoint achieved**
