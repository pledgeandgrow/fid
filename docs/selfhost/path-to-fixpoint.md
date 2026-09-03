# Path to Self-Reproduction — Step by Step

> The exact sequence of steps to get from the current state to a
> self-reproducing Fides compiler (bootstrap fixpoint).
>
> Last updated: Sep 3, 2026.

---

## Where We Are Now

```
✅ Enum variant extraction bug — FIXED (Sep 3, 2026)
✅ Self-hosted compiler parses stdlib/core.fd successfully
❌ Crashes in apply_cfg_filter / check_cfg (0xC0000005 at address 0x11)
```

The crash stack:
```
[0] main
[1] cmd_ccompile
[2] ccompile_source_with_path
[3] build_program_with_path
[4] apply_cfg_filter
[5] check_cfg
```

---

## The 7 Steps to Self-Reproduction

### Step 1: Fix the `apply_cfg_filter` crash  ← CURRENT

**Problem:** `apply_cfg_filter` pushes `Func` and `StructDef` structs into new Vecs. These are large structs (Func has 16 fields including `Vec<Param>`, `Vec<Stmt>`, `Vec<Expr>`, `Vec<Annotation>`). When the C backend stores a struct into a Vec, it may only copy 8 bytes (the pointer), leaving dangling data when the source stack frame is reused.

**The crash at address 0x11** suggests a null pointer + 17-byte offset — likely `annotations.get(i).name` where `annotations` is a corrupted Vec (null data pointer + offset to the `.name` field).

**Fix options:**

**Option A (simplest):** Make `apply_cfg_filter` a no-op. Since the self-hosted compiler always runs on the same platform, and `@cfg` annotations are rare in the compiler source, we can skip the filter entirely:
```fides
pub fn apply_cfg_filter(program: &mut Program) {
    // No-op: the self-hosted compiler includes all items.
    // @cfg filtering is only needed for cross-compilation, which
    // the self-hosted compiler doesn't do yet.
}
```

**Option B (correct):** Fix the Vec-of-struct push problem. The root cause is that `p_vec_push` stores 8 bytes, but `Func` is much larger. The fix is to heap-allocate the struct before pushing (like the Vec push boxing in `lower.fd` lines 2996-3011). But this requires the runtime `p_vec_push` to work correctly with struct pointers, which it already does for the lowerer's own Vec pushes.

**Recommendation:** Start with Option A to unblock, then fix properly later.

### Step 2: Fix the next crash (whatever it is)

After `apply_cfg_filter`, the pipeline continues with:
- `process_derive` — generates methods from `@derive` annotations
- `desugar_layers_models` — desugars ML layer declarations
- `process_gen` — processes `comptime gen { }` blocks
- `fill_default_trait_methods` — fills in trait default methods
- `TypeChecker_register_program` — registers types
- `TypeChecker_check_bodies` — type-checks function bodies
- `BorrowChecker` — borrow checking (currently Off mode)
- `Lowerer_lower_program` — lowers AST to IR
- `CCodeGen_generate` — generates C code

Each of these may have similar Vec-of-struct issues or other crashes. The approach is:
1. Run the self-hosted compiler
2. Read the crash stack
3. Fix the specific function that crashed
4. Repeat

**Key insight:** Most crashes will be the same pattern — large structs being pushed into Vecs or passed by value, where the C backend only copies 8 bytes. The fix is always the same: heap-allocate before pushing, or pass by pointer.

### Step 3: Self-hosted compiler produces C output

Once all crashes in the pipeline are fixed, the self-hosted compiler should produce `release/fides-c.c` (or equivalent) from `compiler/main.fd`. This is the **stage1** output.

**Verification:**
```bash
.\release\fides-c.exe ccompile compiler\main.fd -o release\stage1
# This should produce release/stage1.c without crashing
```

### Step 4: Link stage1 C to an executable

```bash
tcc -O2 -w -o release\stage1.exe release\stage1.c C:\tcc\lib\ws2_32.def
# Patch PE stack to 256MB
```

**Verification:**
```bash
.\release\stage1.exe ccompile tests\hello.fd -o release\hello_test
# stage1.exe should produce correct C output for a simple test
```

### Step 5: stage1 compiles the compiler again (stage2)

```bash
.\release\stage1.exe ccompile compiler\main.fd -o release\stage2
tcc -O2 -w -o release\stage2.exe release\stage2.c C:\tcc\lib\ws2_32.def
# Patch PE stack
```

**Verification:**
```bash
.\release\stage2.exe ccompile tests\hello.fd -o release\hello_test2
# stage2.exe should produce the same output as stage1.exe
```

### Step 6: stage2 compiles the compiler again (stage3)

```bash
.\release\stage2.exe ccompile compiler\main.fd -o release\stage3
tcc -O2 -w -o release\stage3.exe release\stage3.c C:\tcc\lib\ws2_32.def
```

### Step 7: Verify the fixpoint

```bash
# Compare stage2.c and stage3.c — they must be identical
diff release\stage2.c release\stage3.c
```

**If identical:** Fides is self-reproducing. The compiler can build itself, and the output is stable across generations. This is the bootstrap fixpoint.

**If not identical:** Debug which codegen path diverges. Common causes:
- Non-deterministic ordering (HashMap iteration order)
- Uninitialized memory (different values on each run)
- Pointer values being printed (different addresses each run)
- Floating-point formatting differences

---

## The Vec-of-Struct Problem (Root Cause of Most Crashes)

This is the pattern that will cause most crashes between here and the fixpoint:

```fides
var new_funcs: Vec<Func> = Vec::new();
new_funcs.push(program.funcs.get(i));  // ← BUG: only copies 8 bytes
```

`Func` is a large struct (16 fields, many of which are Vecs). The C backend's `p_vec_push` stores 8 bytes (an `i64` slot). So only the first 8 bytes of `Func` are copied — the rest is lost.

**The fix pattern (already used in `lower.fd` for Vec push):**
```fides
// Heap-allocate the struct, then push the pointer
var size: i64 = sizeof_Func;  // need to know the size
var heap_ptr: i64 = p_malloc(size);
memcpy(heap_ptr, &program.funcs.get(i), size);
new_funcs.push(heap_ptr);  // push the pointer, not the struct
```

But this requires:
1. Knowing the size of `Func` at runtime (the C backend doesn't expose this)
2. Changing `Vec<Func>` to `Vec<*Func>` (pointer-based Vec)

**Alternative fix (simpler):** Avoid copying structs entirely. Instead of building a new Vec, modify the existing Vec in-place:
```fides
pub fn apply_cfg_filter(program: &mut Program) {
    // Instead of building new Vecs, mark items as filtered
    // and skip them during iteration.
    // Or: just don't filter (Option A above).
}
```

**The real fix:** The Fides language needs to handle struct copies correctly in the C backend. This is the same root cause as the enum variant extraction bug — `c_type(Struct(_))` returns a pointer type, so copies only copy 8 bytes. The long-term fix is one of:
1. Make `p_vec_push` use `memcpy` when the element type is a struct (requires type info at runtime)
2. Make all large structs heap-allocated by default (the "pointer-based" approach from `COORDINATION.md` [D1])
3. Add a `memcpy` intrinsic that the lowerer emits for struct copies

---

## Estimated Crash Count

Based on the pipeline stages and the Vec-of-struct pattern, here's my estimate of what's left:

| Stage | Likely crashes | Fix pattern |
|-------|---------------|-------------|
| `apply_cfg_filter` | 1 | Skip or fix Vec-of-struct push |
| `process_derive` | 1-2 | Same pattern |
| `desugar_layers_models` | 0-1 | Same pattern |
| `process_gen` | 0 | No-op (pass-through) |
| `fill_default_trait_methods` | 0-1 | Same pattern |
| `TypeChecker_register_program` | 1-3 | Vec-of-struct, HashMap operations |
| `TypeChecker_check_bodies` | 2-5 | The biggest stage — many struct operations |
| `BorrowChecker` | 0 | Off mode — no-op |
| `Lowerer_lower_program` | 1-3 | Already mostly fixed (SRET, enum layout) |
| `CCodeGen_generate` | 1-2 | String building, struct layout references |
| **Total** | **7-18** | |

Each crash takes one fix-and-rebuild cycle. The pattern is always the same: find the function that crashed, fix the struct copy issue, rebuild, run again.

---

## What to Do Right Now

1. **Fix `apply_cfg_filter`** — make it a no-op (Option A) to skip past this crash
2. **Rebuild and run** — see what crashes next
3. **Fix that crash** — same pattern
4. **Repeat** until the self-hosted compiler produces C output
5. **Link and test** — verify the C output is correct
6. **Run the fixpoint** — stage2.c must equal stage3.c

The key insight: **every crash is the same root cause** — the C backend treats structs as pointers, so struct copies only copy 8 bytes. Once you internalize this pattern, each fix is mechanical.

---

## After the Fixpoint

Once Fides is self-reproducing:

1. **Delete the Rust compiler** (optional — keep it as a reference)
2. **The self-hosted compiler is the canonical compiler**
3. **All future development happens in Fides**
4. **The Rust compiler becomes the test oracle** (run both, compare output)
5. **Start working on the PE emitter rewrite** (Phase 10B)
6. **Start building the ecosystem** (packages, docs, playground)
