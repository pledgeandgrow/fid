# Fides Self-Hosting Roadmap — 10 Phases

> **Goal:** Rewrite the Fides compiler (85K LOC Rust) in Fides, one file at a time,
> with testing at every step and zero "big bang" risk.
>
> **Strategy:** Bootstrap via C codegen (stable, 153/153 tests). Fix everything
> that could break the process BEFORE starting. Then rewrite leaf modules first,
> climbing the dependency graph. Each rewritten file gets its own test suite.
>
> **Canonical source for blocker status:** `roadmap.md` Phase 10.

---

## The 10 Phases at a Glance

| Phase | Name | Goal | Risk Level |
|-------|------|------|------------|
| SH-0 | Pre-Flight Audit | Identify every gap, bug, and risk | LOW |
| SH-1 | Debugging Infrastructure | Top-notch error messages + tracing | LOW |
| SH-2 | Test Harness per Module | Golden files for every compiler stage | LOW |
| SH-3 | Fides Language Gaps | Fix features needed for compiler code | MEDIUM |
| SH-4 | Fides Stdlib Gaps | Fill stdlib holes for compiler code | MEDIUM |
| SH-5 | C Codegen Hardening | Survive compiler-sized programs | MEDIUM |
| SH-6 | Rewrite Leaf Modules | token, diag, json, fmt (no deps) | MEDIUM |
| SH-7 | Rewrite Frontend | lexer, ast, parser (recursive descent) | HIGH |
| SH-8 | Rewrite Middle-End | typechecker, lower, ir, mono, borrowck | HIGH |
| SH-9 | Rewrite Back-End + Bootstrap | codegen_c, pir, opt, compiler, bootstrap | HIGH |

---

## Phase SH-0 — Pre-Flight Audit

> **Before touching anything, know exactly what could go wrong.**

### Goal

Catalog every Rust feature, stdlib function, and compiler behavior that the
self-hosting rewrite will depend on. This is a checklist, not code changes.

### Audit Checklist

#### 0.1 Rust → Fides Feature Mapping

For each Rust feature used in the compiler, verify Fides has an equivalent:

| Rust Feature | Used In | Fides Equivalent | Status |
|-------------|---------|-----------------|--------|
| `enum` with data | ast.rs, ir.rs, token.rs, parser.rs | `enum` with payloads | ✅ Works |
| `Box<T>` (recursive types) | ast.rs, ir.rs, parser.rs, lower.rs | `Box<T>` | ✅ Works |
| `HashMap<K,V>` | 33 files | `HashMap<K,V>` via stdlib | ✅ Works |
| `Vec<T>` | all files | `Vec<T>` | ✅ Works |
| `Result<T,E>` | parser, typechecker, lower, compiler | `Result<T,E>` | ✅ Works (generic) |
| `Option<T>` | all files | `Option<T>` | ✅ Works (generic) |
| Closures with capture | 20+ files | `fn(x) { ... }` with capture | ✅ Works |
| `match` with patterns | all files | `match` with patterns | ✅ Works |
| `&str` / string literals | all files | `str` (i64 pointer) | ✅ Works |
| `&[T]` slices | parser, interp | `*T` + length param | ⚠️ Manual |
| `derive(Debug, Clone, PartialEq)` | ast.rs, token.rs, ir.rs | `@derive(Debug, Clone)` | ⚠️ No PartialEq derive |
| Lifetimes (`'a`) | borrowck.rs, codegen_arm64.rs | Not needed (GC via regions) | ✅ N/A |
| `Rc<RefCell<T>>` | interp.rs, debugger.rs | Raw pointers + manual refcount | ⚠️ Different model |
| `impl Trait` return | diag.rs | Return concrete type | ⚠️ Rewrite needed |
| `where` clauses | minimal use | Inline constraints | ✅ N/A |
| `thread::spawn` | hotreload.rs, interp.rs | `spawn` (OS threads) | ✅ Works |
| `std::fmt::Display` | ast.rs, diag.rs | `to_string()` method | ✅ Works |
| `std::fmt::Debug` | all `#[derive(Debug)]` | `@derive(Debug)` → `debug_str()` | ✅ Works |
| `Iterator` trait | minimal use | Vec-based iteration | ⚠️ No lazy iterators |
| `Cell<T>` / `RefCell<T>` | interp.rs, debugger.rs | `*mut T` + manual mutation | ⚠️ Different model |
| Associated types | ast.rs (GATs) | GATs | ✅ Works |
| Const generics | ast.rs (arrays) | Const generics | ✅ Works |
| Trait objects (`dyn`) | ast.rs | `dyn Trait` | ✅ Works |

#### 0.2 Panic/Unwrap Audit

The Rust compiler code has **285 `panic!`/`unwrap()`/`expect()` calls**. Each
one is a potential crash in the Fides rewrite. Catalog them:

| File | Count | Action |
|------|-------|--------|
| parser.rs | 225 | Replace with `Result` returns (most are `peek().unwrap()` — safe if parser logic is correct, but should be `peek_or_err()`) |
| interp.rs | 47 | Replace with proper error propagation |
| typechecker.rs | 9 | Already mostly `Result`-based |
| lower.rs | 3 | Already mostly `Result`-based |
| ir.rs | 1 | Trivial |

**Key insight:** The parser's 225 unwraps are mostly `self.peek().unwrap()` on
the token stream. In Fides, these become `peek()` returning `Option<Token>` with
explicit `?` propagation. This is actually *better* — no silent panics.

#### 0.3 Recursion Depth Audit

The parser is recursive descent with 66 `parse_*` functions. Deep nesting in
Fides source code could cause stack overflow.

| File | Recursive Functions | Max Depth (est.) | Risk |
|------|---------------------|-------------------|------|
| parser.rs | 66 (parse_expr → parse_term → parse_factor → ...) | ~50 for typical code, ~200 for pathological | MEDIUM |
| typechecker.rs | 8 (check_expr → infer_type → unify) | ~30 | LOW |
| lower.rs | 19 (lower_expr → lower_call → lower_expr) | ~40 | LOW |
| opt.rs | recursive inline passes | ~20 | LOW |

**Mitigation:** Fides interpreter has no recursion depth limit yet. Add one
(SH-1.4). C codegen uses the C stack (8MB default on Linux) — sufficient.

#### 0.4 Performance Audit

The compiler is 85K LOC. Can Fides compile a program that large?

| Concern | Rust Performance | Fides Performance | Risk |
|---------|-----------------|-------------------|------|
| Lexer speed | ~1M tokens/sec | Unknown — interpreter is slow | HIGH |
| Parser speed | ~500K tokens/sec | Unknown | HIGH |
| Typechecker speed | ~100K AST nodes/sec | Unknown | HIGH |
| C codegen output size | N/A | ~500K LOC C output (est.) | MEDIUM |
| gcc compile time | N/A | ~30-60s for 500K LOC C | LOW |

**Mitigation:** Use C codegen (not interpreter) for self-hosting. The Fides
compiler generates C, gcc compiles it. Fides compiler speed only matters for
generating the C — not for running the result.

### Exit Criteria

- [ ] Feature mapping table complete (every Rust feature has a Fides plan)
- [ ] All 285 panic/unwrap calls cataloged with replacement strategy
- [ ] Recursion depth risks identified with mitigation plan
- [ ] Performance risks identified with benchmark plan

---

## Phase SH-1 — Debugging Infrastructure

> **Before rewriting anything, make sure you can debug the rewrite.**

### Goal

When the Fides-written parser produces wrong tokens, or the Fides-written
typechecker rejects valid code, you need world-class error messages to find
the bug. This phase adds the debugging tools that make the rewrite tractable.

### 1.1 — Source-Location Error Messages

Every error message in the Fides compiler must include:
- File path
- Line number
- Column number
- The source line with a caret pointing at the error
- A helpful message

```
Error: expected ';' but got '}'
  --> test.fd:42:15
   |
42 |     val x = foo()
   |               ^ expected ';'
```

**Current state:** Fides has `Diag` with file/line/col. The format is basic.
**Action:** Improve `Diag` to include the source line + caret display (like
Rust's error format). This helps both Rust-Fides and Fides-Fides debugging.

### 1.2 — Compiler Stage Tracing

Add `--trace` flag that logs each compiler stage with timing:

```
[trace] Lexer: 1024 tokens in 2ms
[trace] Parser: AST with 512 nodes in 5ms
[trace] Typechecker: 0 errors, 0 warnings in 8ms
[trace] Lowerer: 1024 IR instructions in 3ms
[trace] Optimizer: 23 passes, 128 instructions removed in 4ms
[trace] C Codegen: 4096 lines of C in 6ms
[trace] gcc: compiled in 280ms
[trace] Total: 308ms
```

**Why:** When the Fides-written compiler is 10x slower than Rust, you need to
know WHICH stage is slow. Without per-stage timing, you're guessing.

### 1.3 — IR/AST Dump Commands

Add commands to dump intermediate representations at each stage:

```bash
fidesc dump-ast file.fd      # Print AST as S-expressions
fidesc dump-ir file.fd       # Print IR as text
fidesc dump-pir file.fd 1    # Print PIR L1
fidesc dump-pir file.fd 2    # Print PIR L2
fidesc dump-pir file.fd 3    # Print PIR L3
fidesc dump-c file.fd        # Print generated C (before gcc)
```

**Why:** When the Fides-written lowerer produces wrong IR, you need to compare
it against the Rust-written lowerer's IR. Side-by-side IR diff is the fastest
way to find the bug.

### 1.4 — Recursion Depth Limit

Add a configurable recursion depth limit to the interpreter:

```
--max-recursion 1000   # Default: 1000
```

When exceeded, panic with a clear message:
```
Panic: Recursion depth limit (1000) exceeded in function 'parse_expr'
  This may indicate infinite recursion or very deep nesting.
  Increase with --max-recursion or refactor to iterative.
```

**Why:** The parser is recursive descent. If the Fides-written parser has a
bug causing infinite recursion, this catches it immediately instead of
segfaulting.

### 1.5 — Stack Trace on Panic

Ensure `p_call_stack` (already implemented in C codegen) works correctly for
compiler-sized programs. Verify:
- Stack trace shows function names (not just addresses)
- Stack trace shows source line numbers
- Stack trace is captured on panic, not just on explicit call

### 1.6 — Assertion Infrastructure

Add `assert(condition, message)` builtin that panics with location info:

```fides
assert(idx < len, "index out of bounds: {} >= {}", idx, len);
```

**Why:** The Rust compiler uses `assert!` extensively. The Fides rewrite needs
the same safety net. Every `unwrap()` in the Rust code becomes an `assert` or
a `Result` propagation in Fides.

### Exit Criteria

- [ ] Error messages include source line + caret
- [ ] `--trace` flag shows per-stage timing
- [ ] `dump-ast`, `dump-ir`, `dump-pir` commands work
- [ ] Recursion depth limit configurable, clear panic message
- [ ] Stack traces work on panic (C codegen)
- [ ] `assert()` builtin available

---

## Phase SH-2 — Test Harness per Module

> **Each rewritten file needs its own test suite. Build it before rewriting.**

### Goal

For every Rust source file that will be rewritten, create a golden-file test
that captures its exact output. When the Fides version produces different
output, the test fails immediately.

### 2.1 — Golden Files for Each Stage

For a set of representative test programs, capture:

| Stage | Golden File Content | Command |
|-------|---------------------|---------|
| Lexer | Token stream (type + text + position) | `fidesc dump-tokens file.fd` |
| AST | S-expression tree | `fidesc dump-ast file.fd` |
| IR | IR instruction list | `fidesc dump-ir file.fd` |
| PIR L1 | PIR L1 instructions | `fidesc dump-pir file.fd 1` |
| PIR L2 | PIR L2 instructions | `fidesc dump-pir file.fd 2` |
| PIR L3 | PIR L3 instructions | `fidesc dump-pir file.fd 3` |
| C output | Generated C source | `fidesc dump-c file.fd` |
| Optimized C | C after all passes | `fidesc dump-c file.fd --opt` |

**Test programs:** Use the existing 218 `.fd` test files as the golden corpus.
Also add 5 "compiler-sized" programs (large, complex, exercising edge cases).

### 2.2 — Differential Testing

Add `fidesc diff-test <file.fd> --stage <stage>` that:
1. Runs the Rust compiler to produce golden output
2. Runs the Fides-written compiler to produce test output
3. Diffs them and reports differences

```bash
fidesc diff-test tests/test_parser.fd --stage ast
# Output: AST matches ✅

fidesc diff-test tests/test_complex.fd --stage ir
# Output: IR DIFFERS at instruction 42:
#   Expected: %43 = Add %41, %42
#   Got:      %43 = Mul %41, %42
```

### 2.3 — Per-Module Test Files

For each Rust file that will be rewritten, create a Fides test file that
exercises its functionality:

| Rust File | Fides Test File | What It Tests |
|-----------|----------------|---------------|
| token.rs | `tests/selfhost/test_token.fd` | Token construction, Display |
| lexer.rs | `tests/selfhost/test_lexer.fd` | Tokenization of all token types |
| ast.rs | `tests/selfhost/test_ast.fd` | AST construction, Display |
| parser.rs | `tests/selfhost/test_parser.fd` | Parsing all syntax forms |
| ir.rs | `tests/selfhost/test_ir.fd` | IR construction, printing |
| typechecker.rs | `tests/selfhost/test_typechecker.fd` | Type inference, error detection |
| lower.rs | `tests/selfhost/test_lower.fd` | AST → IR lowering |
| mono.rs | `tests/selfhost/test_mono.fd` | Monomorphization |
| borrowck.rs | `tests/selfhost/test_borrowck.fd` | Borrow checking |
| codegen_c.rs | `tests/selfhost/test_codegen_c.fd` | IR → C generation |

### Exit Criteria

- [ ] `dump-tokens`, `dump-ast`, `dump-ir`, `dump-pir`, `dump-c` commands work
- [ ] Golden files generated for 10 representative programs
- [ ] `diff-test` command works for each stage
- [ ] Per-module test files created for all 10 rewrite targets

---

## Phase SH-3 — Fides Language Gaps

> **Fix language features that the compiler code needs but Fides doesn't have.**

### Goal

Based on the SH-0 audit, implement any missing language features in the Rust
compiler BEFORE starting the rewrite. This way, the Fides language is ready
when we start writing Fides code.

### 3.1 — `@derive(PartialEq)`

The Rust AST uses `#[derive(PartialEq)]` on 8+ types. Fides has `@derive(Debug, Clone)`
but not `PartialEq`. Add it:

```fides
@derive(PartialEq)
enum Token {
    Ident(str),
    Int(i64),
    ...
}
```

Generates `equals(self, other) -> bool` method that compares all fields.

**Why:** The compiler compares AST nodes, tokens, and types for equality
extensively. Without `PartialEq`, every comparison is a manual `equals()` call.

### 3.2 — String Builder Performance

The lexer builds strings character-by-character. Current `StringBuilder` works
but may be slow for large files. Verify performance:

- Benchmark: tokenize a 10K-line file
- If slower than 1s in interpreter, optimize StringBuilder
- C codegen should be fast enough (direct buffer writes)

### 3.3 — Byte/Char Operations

Verify these stdlib functions work correctly and efficiently:
- `str_bytes(s) -> Vec<u8>` — get bytes of a string
- `str_from_bytes(bytes: Vec<u8>) -> str` — create string from bytes
- `str_byte_at(s, i) -> u8` — get byte at index
- `char_to_str(c: i64) -> str` — convert char to string
- `str_len(s) -> i64` — string length in bytes

**Why:** The lexer iterates over input bytes. The parser builds strings from
tokens. These must be rock-solid.

### 3.4 — HashMap Iteration Order

Rust's `HashMap` has non-deterministic iteration order (randomized). Fides's
`HashMap` should have **deterministic** iteration order (insertion order or
sorted keys) so that:
- Golden files are reproducible
- C codegen output is deterministic
- Diff testing works

**Action:** If Fides HashMap iteration is non-deterministic, make it
deterministic (insertion-ordered or sorted by key).

### 3.5 — Integer Formatting

Verify `int_to_str(i64)` and `float_to_str(f64)` produce the same output as
Rust's `i64::to_string()` and `f64::to_string()`. The compiler formats numbers
in error messages, IR dumps, and C codegen.

### 3.6 — Nested Data Structures

Verify deeply nested data structures work:
- `Vec<Vec<Token>>` (lexer token stream per file)
- `HashMap<String, Vec<AST>>` (symbol table)
- `Vec<Result<Token, Error>>` (parser results)
- `Box<Expr>` inside `Vec<Expr>` (AST children)

### Exit Criteria

- [ ] `@derive(PartialEq)` works (if needed)
- [ ] StringBuilder benchmarked on 10K-line file
- [ ] All byte/char stdlib functions verified
- [ ] HashMap iteration is deterministic
- [ ] Integer/float formatting matches Rust
- [ ] Nested data structures work in all backends

---

## Phase SH-4 — Fides Stdlib Gaps

> **Fill stdlib holes that the compiler code will need.**

### Goal

The compiler code uses Rust's stdlib extensively. Map each Rust stdlib function
to a Fides stdlib function. If Fides doesn't have it, add it.

### 4.1 — String Operations

| Rust | Fides | Status |
|------|-------|--------|
| `s.chars()` | `str_bytes(s)` + iterate | ✅ |
| `s.lines()` | `str_split(s, "\n")` | ✅ |
| `s.trim()` | `str_trim(s)` | ✅ |
| `s.starts_with(p)` | `str_starts_with(s, p)` | ✅ |
| `s.ends_with(p)` | `str_ends_with(s, p)` | ✅ |
| `s.contains(p)` | `str_contains(s, p)` | ✅ |
| `s.replace(a, b)` | `str_replace(s, a, b)` | ✅ |
| `s.split(d)` | `str_split(s, d)` | ✅ |
| `s.parse::<i64>()` | `str_parse_int(s)` | ✅ |
| `s.parse::<f64>()` | `str_parse_float(s)` | ⚠️ Add if missing |
| `format!("{}", x)` | `fstring` interpolation | ✅ |
| `s.to_uppercase()` | `str_to_upper(s)` | ⚠️ Add if missing |
| `s.to_lowercase()` | `str_to_lower(s)` | ⚠️ Add if missing |
| `char::is_digit()` | `char_is_digit(c)` | ⚠️ Add if missing |
| `char::is_alpha()` | `char_is_alpha(c)` | ⚠️ Add if missing |
| `char::is_whitespace()` | `char_is_whitespace(c)` | ⚠️ Add if missing |

### 4.2 — Collection Operations

| Rust | Fides | Status |
|------|-------|--------|
| `vec.push(x)` | `Vec_push(v, x)` | ✅ |
| `vec.pop()` | `Vec_pop(v)` | ✅ |
| `vec.get(i)` | `Vec_get(v, i)` | ✅ |
| `vec.set(i, x)` | `Vec_set(v, i, x)` | ✅ |
| `vec.len()` | `Vec_len(v)` | ✅ |
| `vec.is_empty()` | `Vec_len(v) == 0` | ✅ |
| `vec.clear()` | `Vec_clear(v)` | ⚠️ Add if missing |
| `vec.extend(other)` | loop + push | ✅ (manual) |
| `vec.sort()` | `sort(v)` | ✅ |
| `map.insert(k, v)` | `HashMap_insert(m, k, v)` | ✅ |
| `map.get(k)` | `HashMap_get(m, k)` | ✅ |
| `map.contains(k)` | `HashMap_contains(m, k)` | ✅ |
| `map.keys()` | `HashMap_keys(m)` | ✅ |
| `map.values()` | `HashMap_values(m)` | ⚠️ Add if missing |
| `map.remove(k)` | `HashMap_remove(m, k)` | ⚠️ Add if missing |

### 4.3 — File I/O

| Rust | Fides | Status |
|------|-------|--------|
| `fs::read_to_string(path)` | `file_read(path)` | ✅ |
| `fs::write(path, content)` | `file_write(path, content)` | ✅ |
| `fs::read(path)` | `file_read_bytes(path)` | ✅ |
| `fs::write(path, bytes)` | `file_write_bytes(path, bytes)` | ✅ |
| `Path::exists()` | `file_exists(path)` | ✅ |
| `fs::remove_file()` | `file_delete(path)` | ✅ |
| `fs::metadata().len()` | `file_size(path)` | ✅ |
| `std::env::args()` | `env_args()` | ✅ |
| `std::env::var()` | `env_get(name)` | ✅ |

### 4.4 — Math/Bit Operations

| Rust | Fides | Status |
|------|-------|--------|
| `i64::MAX` | `9223372036854775807` | ✅ (literal) |
| `i64::MIN` | `-9223372036854775808` | ✅ (literal) |
| `u64` arithmetic | `i64` with wrapping | ⚠️ Careful |
| `x as u8` | `bit_cast` / `as` | ✅ |
| `x.count_ones()` | `bits_popcount(x)` | ✅ |
| `x.leading_zeros()` | `bits_clz(x)` | ✅ |
| `x.trailing_zeros()` | `bits_ctz(x)` | ✅ |

### Exit Criteria

- [ ] All ⚠️ items in 4.1-4.4 implemented and tested
- [ ] Every Rust stdlib function used in compiler code has a Fides equivalent
- [ ] String operations benchmarked on 10K-line file

---

## Phase SH-5 — C Codegen Hardening

> **The C codegen must survive compiler-sized programs without crashing.**

### Goal

The self-hosted compiler will generate ~500K LOC of C. The C codegen must
handle this without:
- Stack overflow (deep IR → deep C function calls)
- Memory exhaustion (large Vec/HashMap allocations)
- Name collisions (thousands of generated function names)
- gcc compile failure (C99 compliance)

### 5.1 — Large Program Test

Write a "compiler-sized" Fides program (~5000 LOC) that exercises:
- Deep nesting (100+ levels of match/if/for)
- Large Vec operations (10K+ elements)
- Large HashMap operations (1K+ entries)
- Many functions (500+)
- Many structs/enums (100+)
- String operations on large strings (10K+ chars)

Compile with C codegen and verify:
- [ ] gcc compiles it without error
- [ ] It runs without crashing
- [ ] Output matches interpreter

### 5.2 — Name Mangling Stress Test

The C codegen renames Fides functions to avoid C identifier conflicts. With
500+ functions, verify:
- No name collisions
- No names exceeding C99 identifier length limits (31 chars for external)
- Generated names are deterministic

### 5.3 — Memory Leak Check

Run the large program under valgrind:
```bash
valgrind --leak-check=full ./compiler-sized.exe
```

Verify no memory errors. The C codegen uses `malloc`/`free` — leaks are
acceptable for a compiler (process exits), but use-after-free and buffer
overflows are not.

### 5.4 — gcc Warning Audit

Compile the large program with `-Wall -Wextra`:
```bash
fidesc ccompile compiler-sized.fd
gcc -Wall -Wextra compiler-sized.c -o compiler-sized.exe
```

Fix any warnings — they may indicate real bugs (unused variables, type
mismatches, missing return statements).

### Exit Criteria

- [ ] 5000-LOC Fides program compiles and runs via C codegen
- [ ] No name collisions in generated C
- [ ] No valgrind errors (use-after-free, buffer overflow)
- [ ] No gcc warnings on generated C

---

## Phase SH-6 — Rewrite Leaf Modules

> **Start with modules that have no dependencies on other compiler modules.**

### Goal

Rewrite the simplest, dependency-free modules in Fides. These are small,
self-contained, and easy to test. Success here builds confidence.

### 6.1 — Rewrite `token.rs` (261 LOC)

**Dependencies:** none
**Fides file:** `stdlib/compiler/token.fd`

The token module defines:
- `enum Token { Ident(str), Int(i64), Float(f64), ... }` (~30 variants)
- `enum TokenKind` (keyword categories)
- `fn token_to_str(t: Token) -> str`
- `fn token_kind(t: Token) -> TokenKind`

**Test:** `tests/selfhost/test_token.fd` — construct every token variant,
verify `to_string()` output matches Rust's `format!("{:?}", token)`.

**Verification:**
```bash
fidesc diff-test tests/selfhost/test_token.fd --stage tokens
# Must match Rust token output exactly
```

### 6.2 — Rewrite `diag.rs` (463 LOC)

**Dependencies:** none
**Fides file:** `stdlib/compiler/diag.fd`

The diag module defines:
- `struct Diag { severity, message, file, line, col }`
- `enum Severity { Error, Warning, Info }`
- `fn diag_to_str(d: Diag) -> str`
- `fn format_error(file, line, col, msg) -> str`

**Test:** `tests/selfhost/test_diag.fd` — construct diagnostics, verify
formatting matches Rust output.

### 6.3 — Rewrite `json.rs` (289 LOC)

**Dependencies:** none
**Fides file:** `stdlib/compiler/json.fd`

The JSON module is self-contained:
- `enum Json { Null, Bool(bool), Num(f64), Str(str), Arr(Vec), Obj(HashMap) }`
- `fn json_parse(s: str) -> Result<Json, str>`
- `fn json_to_str(j: Json) -> str`

**Test:** `tests/selfhost/test_json.fd` — parse and serialize JSON, verify
output matches Rust's `json.rs`.

### 6.4 — Rewrite `fmt.rs` (1165 LOC)

**Dependencies:** none (uses only stdlib)
**Fides file:** `stdlib/compiler/fmt.fd`

The formatter is self-contained:
- Tokenizes Fides source
- Reformats with consistent indentation
- No dependency on parser/AST

**Test:** `tests/selfhost/test_fmt.fd` — format a set of Fides files, verify
output matches `fidesc fmt` output.

### Exit Criteria

- [ ] `token.fd` passes differential test against `token.rs`
- [ ] `diag.fd` passes differential test against `diag.rs`
- [ ] `json.fd` passes differential test against `json.rs`
- [ ] `fmt.fd` passes differential test against `fmt.rs`
- [ ] All 4 modules work in interpreter + C codegen

---

## Phase SH-7 — Rewrite Frontend

> **The lexer, AST, and parser are the foundation. Get them right.**

### Goal

Rewrite the lexer, AST types, and parser in Fides. These are the most
recursive and complex modules. Test exhaustively with golden files.

### 7.1 — Rewrite `lexer.rs` (538 LOC)

**Dependencies:** `token.fd` (from SH-6.1)
**Fides file:** `stdlib/compiler/lexer.fd`

The lexer:
- Takes a `str` (source code)
- Produces `Vec<Token>` (token stream)
- Handles: identifiers, keywords, numbers, strings, operators, comments
- Tracks line/column for error messages

**Risk:** String iteration in Fides uses `str_bytes()` — verify this works
correctly for all edge cases (UTF-8, escape sequences, multi-line strings).

**Test strategy:**
1. Run Rust lexer on all 218 test files → golden token streams
2. Run Fides lexer on all 218 test files → compare
3. Any mismatch = bug in Fides lexer

**Verification:**
```bash
for f in tests/*.fd; do
  fidesc diff-test "$f" --stage tokens || echo "MISMATCH: $f"
done
```

### 7.2 — Rewrite `ast.rs` (936 LOC)

**Dependencies:** none (just type definitions)
**Fides file:** `stdlib/compiler/ast.fd`

The AST module defines:
- `enum Type { I64, F64, Bool, Str, Struct(...), Enum(...), ... }` (~20 variants)
- `enum Expr { Lit(...), Var(...), BinOp(...), Call(...), Match(...), ... }` (~30 variants)
- `enum Stmt { Let(...), Assign(...), If(...), While(...), For(...), ... }` (~15 variants)
- `struct Function`, `struct Struct`, `struct Enum`, `struct Module`

**Risk:** AST types use `Box<T>` for recursive types. Verify `Box<T>` works
in Fides for all recursive patterns.

**Test:** `tests/selfhost/test_ast.fd` — construct every AST variant, verify
`to_string()` / `debug_str()` output.

### 7.3 — Rewrite `parser.rs` (3226 LOC — LARGEST MODULE)

**Dependencies:** `token.fd`, `ast.fd`, `lexer.fd`
**Fides file:** `stdlib/compiler/parser.fd`

The parser is the biggest challenge:
- 66 recursive descent functions
- 225 `unwrap()` calls (→ `?` propagation in Fides)
- Handles: all expressions, statements, types, declarations, attributes
- Returns `Result<AST, Error>`

**Risk mitigation:**
1. Rewrite in stages — start with expressions, then statements, then declarations
2. Test each stage with golden AST files
3. The 225 unwraps become `peek()?` / `expect()?` — cleaner than Rust

**Test strategy:**
1. Parse all 218 test files with Rust parser → golden AST
2. Parse all 218 test files with Fides parser → compare AST
3. Also test error cases — parser should reject invalid code with same errors

**Verification:**
```bash
for f in tests/*.fd; do
  fidesc diff-test "$f" --stage ast || echo "AST MISMATCH: $f"
done
```

### Exit Criteria

- [ ] `lexer.fd` produces identical token stream to `lexer.rs` on all 218 tests
- [ ] `ast.fd` constructs all AST types correctly
- [ ] `parser.fd` produces identical AST to `parser.rs` on all 218 tests
- [ ] Parser error messages match (or improve) Rust parser errors
- [ ] All 3 modules work in interpreter + C codegen

---

## Phase SH-8 — Rewrite Middle-End

> **Typechecker, lowerer, IR, monomorphization, borrow checker.**

### Goal

Rewrite the semantic analysis and IR generation in Fides. These modules
transform the AST into IR, checking types and borrows along the way.

### 8.1 — Rewrite `ir.rs` (1754 LOC)

**Dependencies:** `ast.fd`
**Fides file:** `stdlib/compiler/ir.fd`

The IR module defines:
- `enum IrType { I64, F64, Bool, Str, Struct(...), ... }`
- `enum IrInst { Alloca, Load, Store, BinOp, Call, Branch, ... }` (~40 variants)
- `struct IrFunction`, `struct IrBlock`, `struct IrModule`
- `fn ir_to_str(...) -> str` (for debugging)

**Test:** `tests/selfhost/test_ir.fd` — construct IR, verify printing.

### 8.2 — Rewrite `typechecker.rs` (7424 LOC — LARGEST MODULE)

**Dependencies:** `ast.fd`, `ir.fd`
**Fides file:** `stdlib/compiler/typechecker.fd`

The typechecker is the second-largest module:
- Type inference and checking
- Region checking
- Effect checking
- Capability checking
- Safety annotation verification (@crashfree, @deterministic, etc.)
- Trait resolution
- Generic instantiation

**Risk:** 7424 LOC is huge. Consider splitting into multiple Fides files:
- `typechecker_types.fd` — type inference
- `typechecker_regions.fd` — region checking
- `typechecker_effects.fd` — effect checking
- `typechecker_safety.fd` — safety annotations
- `typechecker_traits.fd` — trait resolution

**Test strategy:**
1. Typecheck all 218 test files with Rust typechecker → golden IR types
2. Typecheck all 218 test files with Fides typechecker → compare
3. Test error cases — typechecker should reject invalid code with same errors

### 8.3 — Rewrite `lower.rs` (6143 LOC)

**Dependencies:** `ast.fd`, `ir.fd`, `typechecker.fd`
**Fides file:** `stdlib/compiler/lower.fd`

The lowerer transforms AST → IR:
- 19 `lower_*` functions
- Handles: expressions, statements, functions, closures, match, loops
- Inserts: drop calls, region begin/end, effect tracking

**Test strategy:**
1. Lower all 218 test files with Rust lowerer → golden IR
2. Lower all 218 test files with Fides lowerer → compare IR
3. `fidesc diff-test tests/*.fd --stage ir`

### 8.4 — Rewrite `mono.rs` (1475 LOC)

**Dependencies:** `ast.fd`
**Fides file:** `stdlib/compiler/mono.fd`

Monomorphization:
- Clones generic functions for each concrete type
- Resolves const generics
- Handles trait dispatch

**Test:** Verify monomorphized output matches Rust `mono.rs`.

### 8.5 — Rewrite `borrowck.rs` (1221 LOC)

**Dependencies:** `ast.fd`, `typechecker.fd`
**Fides file:** `stdlib/compiler/borrowck.fd`

Borrow checker:
- Tracks mutable/immutable borrows
- Field-wise analysis
- Two-phase borrows
- NLL (non-lexical lifetimes)

**Test:** Verify borrow check results match Rust `borrowck.rs`.

### Exit Criteria

- [ ] `ir.fd` produces identical IR types to `ir.rs`
- [ ] `typechecker.fd` produces identical types + errors to `typechecker.rs`
- [ ] `lower.fd` produces identical IR to `lower.rs` on all 218 tests
- [ ] `mono.fd` produces identical monomorphized output to `mono.rs`
- [ ] `borrowck.fd` produces identical borrow check results to `borrowck.rs`
- [ ] All modules work in interpreter + C codegen

---

## Phase SH-9 — Rewrite Back-End + Bootstrap

> **Code generation, optimization, and the final bootstrap.**

### Goal

Rewrite the code generators, optimizer, and compiler driver in Fides. Then
bootstrap: the Fides-written compiler compiles itself.

### 9.1 — Rewrite `pir.rs` (2119 LOC)

**Dependencies:** `ast.fd`, `ir.fd`, `compiler.fd`
**Fides file:** `stdlib/compiler/pir.fd`

PIR (Fides's intermediate representation):
- L1 → L2 → L3 transforms
- SSA form, block structure

### 9.2 — Rewrite `opt.rs` (8115 LOC — LARGEST MODULE)

**Dependencies:** `ast.fd`, `ir.fd`, `pir.fd`, `compiler.fd`
**Fides file:** `stdlib/compiler/opt.fd` (or split into multiple files)

The optimizer has 23 passes. Consider splitting:
- `opt_const.fd` — const-fold, CSE, GVN
- `opt_dce.fd` — DCE, DSE, SROA
- `opt_inline.fd` — inlining, unrolling, TCO
- `opt_loop.fd` — loop-rotate, loop-vec, vectorize
- `opt_other.fd` — devirt, jump-thread, reassociate, memoize

**Test:** `fidesc diff-test tests/*.fd --stage pir` for each optimization level.

### 9.3 — Rewrite `codegen_c.rs` (4241 LOC)

**Dependencies:** `ast.fd`, `ir.fd`
**Fides file:** `stdlib/compiler/codegen_c.fd`

C codegen transforms IR → C source code:
- Function emission
- Type declarations
- Runtime builtins (p_vec_*, p_map_*, etc.)
- Binary hardening (canaries, PIE, RELRO)

**Test:** `fidesc diff-test tests/*.fd --stage c` — generated C must match.

### 9.4 — Rewrite `compiler.rs` (2105 LOC)

**Dependencies:** all previous modules
**Fides file:** `stdlib/compiler/compiler.fd`

The compiler driver:
- Orchestrates: lexer → parser → typechecker → lower → mono → opt → codegen
- Handles: comptime evaluation, derive macros, incremental compilation
- Manages: module loading, import resolution

### 9.5 — Bootstrap

The moment of truth:

```bash
# Step 1: Rust compiler compiles Fides compiler
fidesc ccompile stdlib/compiler/compiler.fd -o fides-c

# Step 2: Fides compiler (compiled by Rust) compiles itself
./fides-c stdlib/compiler/compiler.fd -o fides-self

# Step 3: Fides compiler (compiled by Fides) compiles itself again
./fides-self stdlib/compiler/compiler.fd -o fides-self-2

# Step 4: Verify the two self-compiled binaries are identical
diff fides-self fides-self-2  # Should be identical (deterministic build)

# Step 5: Run the full test suite with the self-hosted compiler
./fides-self test
# Must pass: 153/153 interpreter, 153/153 C codegen, 153/153 PE
```

### 9.6 — Regression Testing

After bootstrap:
- [ ] Self-hosted compiler passes all 218 test files
- [ ] Self-hosted compiler produces identical C output to Rust compiler
- [ ] Self-hosted compiler can compile itself (round-trip stable)
- [ ] No performance regression > 3x (acceptable for first bootstrap)

### Exit Criteria

- [ ] `pir.fd` produces identical PIR to `pir.rs`
- [ ] `opt.fd` produces identical optimized IR to `opt.rs`
- [ ] `codegen_c.fd` produces identical C to `codegen_c.rs`
- [ ] `compiler.fd` orchestrates all stages correctly
- [ ] **Bootstrap succeeds:** Fides compiles itself
- [ ] **Round-trip stable:** Self-compiled compiler compiles itself identically
- [ ] **Full test suite passes:** 153/153 on all backends

---

## Risk Mitigation Summary

### What Could Go Wrong and How We Handle It

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Fides language missing a feature | MEDIUM | HIGH | SH-0 audit + SH-3 fixes before rewrite |
| Fides stdlib missing a function | MEDIUM | MEDIUM | SH-0 audit + SH-4 fixes before rewrite |
| C codegen crashes on large programs | LOW | HIGH | SH-5 hardening before rewrite |
| Parser recursion overflow | LOW | HIGH | SH-1.4 depth limit + iterative fallback |
| Performance 10x+ slower | HIGH | MEDIUM | Use C codegen (not interpreter); optimize later |
| Generated C doesn't compile with gcc | LOW | HIGH | SH-5.4 gcc warning audit |
| Name collisions in generated C | LOW | MEDIUM | SH-5.2 name mangling stress test |
| HashMap non-determinism | MEDIUM | LOW | SH-3.4 deterministic iteration |
| Float formatting mismatch | MEDIUM | LOW | SH-3.5 formatting verification |
| Memory leak in long-running compile | LOW | LOW | Acceptable for compiler (process exits) |
| Bootstrap produces different binary | LOW | MEDIUM | SH-9.5 step 4 — verify deterministic build |
| Self-hosted compiler has new bugs | HIGH | HIGH | SH-2 differential testing at every stage |
| Rewrite takes too long | MEDIUM | LOW | One file at a time — can pause anytime |

### The "Escape Hatch"

If any phase reveals an unsolvable problem (e.g., Fides can't express something
the compiler needs), the rewrite can pause. The Rust compiler continues to work.
The Fides-written modules that ARE done can be used as an alternative frontend
or for testing. There is no "all or nothing" risk.

### Order of Operations (Dependency-Safe)

```
SH-0 (audit) → SH-1 (debugging) → SH-2 (testing) → SH-3 (lang gaps) → SH-4 (stdlib gaps)
     ↓
SH-5 (C codegen hardening)
     ↓
SH-6 (leaf modules: token, diag, json, fmt)
     ↓
SH-7 (frontend: lexer → ast → parser)
     ↓
SH-8 (middle-end: ir → typechecker → lower → mono → borrowck)
     ↓
SH-9 (back-end: pir → opt → codegen_c → compiler → BOOTSTRAP)
```

Each phase depends on the previous one. No phase can be skipped. But any phase
can be paused — the Rust compiler remains functional throughout.

---

## Estimated Effort

| Phase | Description | Effort | Risk |
|-------|-------------|--------|------|
| SH-0 | Pre-flight audit | 1 session | LOW |
| SH-1 | Debugging infrastructure | 2-3 sessions | LOW |
| SH-2 | Test harness per module | 2-3 sessions | LOW |
| SH-3 | Fides language gaps | 2-4 sessions | MEDIUM |
| SH-4 | Fides stdlib gaps | 2-3 sessions | MEDIUM |
| SH-5 | C codegen hardening | 1-2 sessions | MEDIUM |
| SH-6 | Rewrite leaf modules (4 files, ~2K LOC) | 3-5 sessions | MEDIUM |
| SH-7 | Rewrite frontend (3 files, ~5K LOC) | 5-8 sessions | HIGH |
| SH-8 | Rewrite middle-end (5 files, ~18K LOC) | 8-12 sessions | HIGH |
| SH-9 | Rewrite back-end + bootstrap (5 files, ~22K LOC) | 8-12 sessions | HIGH |
| **Total** | | **~35-50 sessions** | |

**Note:** "Session" = ~4 hours of focused work. The total is ~140-200 hours.
This is a rough estimate — actual time depends on how many bugs are found
in the Fides language and C codegen during the rewrite.
