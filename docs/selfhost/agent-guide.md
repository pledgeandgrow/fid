# Fides Self-Hosted Compiler — Agent Best Practices

> Rules and workflows for AI agents (and humans) working on the self-hosted
> compiler. Read this before touching any file in `compiler/`.

---

## 1. Golden Rule: Verify Before and After

```bash
# ALWAYS run this before starting work
./verify.sh

# AND this if you are touching the self-hosted compiler
./verify-selfhost.sh

# Run this after your changes
./verify.sh
./verify-selfhost.sh
```

If a test fails, understand why before fixing. Do not guess. Use the tools:

```bash
./scripts/bisect.sh <test_name>
.\verify-stage-diff.ps1 -File <test_name>
.\verify-stdout-diff.ps1 -File <test_name> -Verbose
```

---

## 2. Read These Files First

Every session, in this order:

1. `CURRENT.md` — what is the current blocker and where we stopped.
2. `AGENTS.md` — build/test commands and high-risk files.
3. `docs/selfhost/README.md` — this docs index.
4. The relevant section of `docs/selfhost/architecture.md` or `language-constraints.md` or `calling-conventions.md`.
5. `docs/roadmap/selfhost-roadmap.md` — the phase you are in.

---

## 3. High-Risk Files — Do Not Touch Blindly

These files can break the entire compiler or silently corrupt output:

| File | Why it is dangerous |
|------|---------------------|
| `compiler/lower.fd` | SRET, enum layout, match, try, defer, effects, calling conventions |
| `compiler/ir.fd` | Core IR types; changes ripple to all backends |
| `compiler/codegen_c.fd` | ABI, struct copy, SRET, C type mapping |
| `compiler/pe.fd` | PE emitter — x86-64 instruction encoding, ModRM/SIB bytes, REX prefixes, PE file format. Off-by-one = corrupt .exe. Must mirror `src/pe.rs`. |
| `compiler/peephole.fd` | 9 peephole passes — a wrong transform silently corrupts all output. Must mirror `src/peephole.rs`. (session 8, not yet integrated) |
| `compiler/regalloc.fd` | Linear scan register allocator — any change affects EVERY function. Must mirror `src/regalloc.rs`. (session 8, not yet integrated) |
| `compiler/codegen_x64.fd` | PIR L3 → x86-64 instruction encoding, ModRM/SIB bytes, REX prefixes, ABI rules. Must mirror `src/codegen_x64.rs`. (session 8, not yet integrated) |
| `compiler/parser.fd` | Recursive descent; subtle parse changes |
| `compiler/ast.fd` | Canonical AST; if you add/remove variants, update `lower.fd` `box_*_unbox` and `codegen_c.fd` |
| `compiler/compiler.fd` | Pipeline driver; import resolution is fragile |

For the Rust reference compiler, also treat `src/lower.rs`, `src/codegen_c.rs`, `src/pir.rs`, `src/regalloc.rs`, `src/peephole.rs`, `src/codegen_x64.rs`, and `src/pe.rs` as high-risk.

---

## 4. How to Make a Safe Change

### Step 1: Reproduce the problem

If you are fixing a bug, write a failing test or run the existing one:

```bash
./target/release/fidesc run tests/<name>.fd
./target/release/fidesc run tests/<name>.fd --backend c
```

### Step 2: Make the smallest possible change

- Edit in place.
- Do not refactor surrounding code.
- Do not rename things unless absolutely necessary.
- Do not add abstractions.
- Do not add tests/docs unless asked.

### Step 3: Verify the fix

```bash
# For the Rust compiler
./target/release/fidesc run tests/<name>.fd
./target/release/fidesc run tests/<name>.fd --backend c
./target/release/fidesc run tests/<name>.fd --backend pe

# For the self-hosted compiler, rebuild and run
.\build-selfhost.ps1
.\release\fides-c.exe run tests/<name>.fd

# For the PE emitter (native codegen, no tcc)
.\release\fides-c.exe build tests/bootstrap/hello_pe.fd -o release/hello_pe.exe
.\release\hello_pe.exe
```

### Step 4: Run the full suites

```bash
./verify.sh
./verify-selfhost.sh
```

### Step 5: Commit

See `AGENTS.md` for the commit format. Use the `git commit` helper pattern:

```powershell
git commit -m "$(cat <<'EOF'
Fix X by doing Y.

Generated with [Devin](https://devin.ai)

Co-Authored-By: Devin <...>
EOF
)"
```

Do not commit if no changes exist. Do not push unless asked.

---

## 5. What to Do When the Self-Hosted Compiler Crashes

### If the crash is `STATUS_ACCESS_VIOLATION` / segfault

1. Check if the PE stack was patched to 256 MB.
2. Check if the crash handler printed a call stack.
3. Look at the last progressive stage message to see which stage crashed.
4. Add targeted `p_eprint_str` prints in `compiler.fd` or the relevant module.
5. Rebuild and rerun.
6. Check for:
   - Null pointer from `p_file_read` / `p_malloc`
   - Unbounded recursion
   - SRET mismatch (first arg not a buffer)
   - Undersized enum/struct layout
   - `Inst::Load` on a struct-typed field

### If the crash is in `Lexer_tokenize`

Check:
- `src_len` is cached and used everywhere (not `str_len(self.source)` in hot loops).
- No O(n²) string concatenation in the lexer.
- String literals are not overflowing buffers.

### If the crash is in `Parser_...`

Check:
- `box_expr_unbox` matches the canonical `ast.fd` `Expr` enum.
- SRET is used for `ParseResult*` returns.
- Enum variant extraction is not truncating payloads.

### If the crash is in the PE emitter (`pe.fd`)

Check:
- **ModRM byte encoding** — `mod=3` for register-register ops, `mod=0` for
  memory. A wrong `mod` field generates a memory store instead of a register
  load (or vice versa). Compare with `src/pe.rs`.
- **Call targets** — verify rip-relative call offsets resolve to the correct
  function labels. Use PowerShell to read the PE bytes and decode `E8` (CALL)
  instructions.
- **Missing statements** — the Fides compiler silently drops statements in
  long functions. If a function call is missing from the generated C, split
  the source function into smaller helpers.
- **Print type dispatch** — `Inst::Print` must dispatch by IR type (Ptr→str,
  F64→float, Bool→bool, default→int). Calling `p_print_int` with a string
  pointer crashes.
- **Binary output** — `p_file_write` truncates at null bytes. PE files must
  be hex-encoded and decoded via PowerShell.

---

## 6. How to Debug the SRET / Enum Machinery

### Check the generated C

```bash
# Compile a small test to C
.\target\release\fidesc.exe ccompile test.fd -o test_out

# Or the self-hosted compiler
.\release\fides-c.exe ccompile test.fd -o test_out

# Read test_out.c
```

#### What enum variant extraction looks like in C (session 14)

For `ParseResultExpr::Ok(e) => { ... }`, under the **pointer-based enum
payload approach** (COORDINATION.md [D1]), the generated C looks like:

```c
v12 = alloca(16);                  // ParseResultExpr buffer (tag + 1 pointer)
memset(v12, 0, 16);
Parser_parse_unary(v12, ...);      // SRET call (correct)
v15 = (char*)v12 + 0;
v16 = *(int64_t*)v15;              // tag
v18 = v16 == 0;                    // Ok variant?
if (v18) goto bb2;
else goto bb3;
bb2:
v20 = (char*)v12 + 8;              // pointer to Expr payload slot
v21 = *(void**)v20;                // reads the 8-byte pointer — CORRECT
```

With the pointer-based approach, `Load` reads the 8-byte pointer to the
payload data. No `load_enum_field()` or `memcpy` is needed. This is the
correct behavior under D1.

#### What to look for in generated C

- `alloca(N)` sizes match the struct/enum layouts (pointer-based: `(1 + max_arity) * 8`).
- Calls to struct-returning functions have the SRET pointer first: `func(&buf, ...)`.
- `memcpy` is used for `Inst::Store` when the store type is `IrType::Struct(_)`.
- `Load` on `IrType::Struct(_)` reads 8 bytes (the pointer) — this is correct
  for both regular struct values and enum payload extraction under D1.
- No `*(int64_t*)` stores for `IrType::Struct(_)`.
- Float constants use union compound literals: `((union { uint64_t u; double f; }){ .u = ... }).f`

### Compare to Rust output

```bash
# Rust compiler
.\target\release\fidesc.exe dump-ir test.fd
.\target\release\fidesc.exe dump-c test.fd

# Self-hosted (when it works)
.\release\fides-c.exe dump-ir test.fd
.\release\fides-c.exe dump-c test.fd
```

### Use stage differential

```bash
.\verify-stage-diff.ps1 -File <test_name> -Stage ir
.\verify-stage-diff.ps1 -File <test_name> -Stage c
```

Tells you whether the divergence is in lexer, parser, IR, or C codegen.

---

## 7. Do Not Do These Things

- **Do not `git stash`, `git checkout` over user files, `git reset --hard`, `git clean`, force-push, or delete branches.** These are destructive.
- **Do not run `rm -rf`.**
- **Do not add dependencies published less than 7 days ago.**
- **Do not commit secrets.**
- **Do not modify `minimumReleaseAge` or other security controls to work around CI.**
- **Do not use `-i` (interactive) git flags.**
- **Do not push unless explicitly asked.**
- **Do not write new files unless explicitly required** (docs are an exception when updating docs).

---

## 8. Documentation and Roadmap Updates

If you discover new information about the self-hosted compiler, update these
files:

- `CURRENT.md` — current focus and blocker
- `docs/roadmap/selfhost-roadmap.md` — phase status and known tasks (SH-13 for PE emitter)
- `docs/selfhost/known-bugs.md` — new bugs or gotchas
- `docs/selfhost/calling-conventions.md` — new low-level conventions
- `docs/selfhost/architecture.md` — new modules or pipeline changes

Do **not** create new files for every small note. Prefer appending to the
existing files in `docs/selfhost/`.

---

## 9. When to Ask for Help

Ask the user when:
- You hit an authentication/permission issue.
- A project configuration change is needed.
- You need to run a destructive operation.
- The fix requires changing many files and you are unsure of the approach.
- The Rust compiler and Fides compiler disagree and you cannot determine which is right.

Otherwise, keep trying reasonable approaches, run the tools, and verify.

---

## 10. Quick Checklist for Every Session

- [ ] Read `CURRENT.md`.
- [ ] Read the relevant `docs/selfhost/*.md` files.
- [ ] Run `./verify.sh` (or at least check the current state).
- [ ] Make the smallest change that fixes the issue.
- [ ] Rebuild the self-hosted compiler if `compiler/*.fd` changed.
- [ ] Run the relevant tests.
- [ ] Run `./verify.sh` and `./verify-selfhost.sh` before declaring done.
- [ ] Update `CURRENT.md` and `docs/selfhost/` if you learned something new.
