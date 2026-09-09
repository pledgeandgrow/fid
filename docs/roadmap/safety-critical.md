# Safety-Critical Roadmap

> Four gaps that must be closed before Fides can be used in safety-critical
> domains (medical, automotive, avionics, financial, cryptographic). Each item
> has a stub in the compiler and a full implementation plan below.

## Status Summary

| # | Item | Status | Stub Location |
|---|------|--------|---------------|
| 17 | Certified toolchain | Stub (`qualify` command) | `src/main.rs` `generate_qualification_manifest` |
| 18 | Information-flow control | **DONE** — full data-flow taint tracking | `src/typechecker.rs` |
| 19 | SBOM export | Stub (`sbom` command) | `src/main.rs` `generate_sbom` |
| 20 | Post-codegen constant-time verification | Stub (`verify_constant_time_asm`) | `src/codegen_x64.rs` |

---

## 17. Certified Toolchain

### Problem

`@medical` (IEC 62304 / DAL-A) and `@safety-critical` annotations are only
meaningful if the compiler itself is qualified. Right now Fides is a research
compiler with no certification. A buggy compiler can silently miscompile
safety-critical code, defeating the purpose of the annotations.

### Current State

- `fidesc qualify <file.fd>` emits a JSON manifest with:
  - Compiler version and implementation (Rust bootstrap vs self-hosted)
  - Optimizer pass list (22 passes from `opt::Pass::all()`)
  - Annotations enforced in the Rust bootstrap vs stubbed in self-hosted
  - Capabilities and annotations used in the input program
  - `qualificationStatus: "NOT_QUALIFIED"`

### Full Implementation Plan

**Phase 1: Qualification Manifest (DONE — stub)**
- Emit manifest with compiler info, pass list, annotation status
- Track which annotations are enforced vs stubbed in each compiler
- Include test results from `verify.sh` in the manifest

**Phase 2: Toolchain Self-Verification**
- Implement `fidesc qualify --self-test` that runs:
  - All bootstrap tests (`verify.sh`)
  - All self-host tests (`verify-selfhost.sh`)
  - Bootstrap fixpoint check (fides-c2.c == fides-c3.c)
  - Annotation enforcement tests (positive + negative)
- Record pass/fail counts in the manifest
- Hash the compiler binary and include it in the manifest

**Phase 3: Annotation Enforcement Audit**
- For each annotation (`@medical`, `@safety-critical`, `@constant_time`, etc.):
  - Document the exact rules enforced
  - List the test cases that verify enforcement
  - List known gaps (e.g., post-codegen CT verification — see §20)
- Generate a compliance matrix: annotation → rules → tests → status

**Phase 4: Certification Partnership**
- Partner with a certification body for the target domain:
  - Medical: IEC 62304 (Class B/C) — TÜV, BSI
  - Avionics: DO-178C (Level A/B) — FAA, EASA
  - Automotive: ISO 26262 (ASIL B/C/D) — TÜV, SGS
- This is a multi-year, multi-million-dollar effort
- Requires: tool qualification plan, configuration management, traceability
  matrix, independent verification, documentation

**Phase 5: Locked-Down Build**
- Reproducible builds from source-identical inputs
- Signed compiler binaries with reproducible signatures
- Pinned dependencies (no floating versions)
- SBOM for the compiler itself (recursive — see §19)

### Files to Modify

| File | Changes |
|------|---------|
| `src/main.rs` | Expand `generate_qualification_manifest` with self-test, binary hash |
| `src/compiler.rs` | Add `qualify_self_test()` that runs test suites |
| `src/opt.rs` | Expose pass list and pass-relevance logic for the manifest |
| `src/typechecker.rs` | Expose annotation enforcement status per annotation |
| `docs/qualification/` | New directory: qualification plan, compliance matrix |

### Test Strategy

- `tests/qualification/test_manifest.fd` — verify manifest is valid JSON
- `tests/qualification/test_self_test.fd` — verify self-test reports correct counts
- `tests/qualification/test_annotation_audit.fd` — verify all annotations are audited

---

## 18. Information-Flow Control (Full Taint Tracking)

### Problem

`@tainted` marks untrusted input parameters, but the current `verify_tainted`
in `src/typechecker.rs` is a coarse approximation: it flags ALL sink calls
from ANY function that has a `@tainted` parameter, without tracking which
specific values are tainted or recognizing sanitizer functions.

This means:
- `fn process(@tainted input: str, clean: str) { sql_query(clean); }` — false
  positive (clean is not tainted, but the function has a tainted param)
- `fn process(@tainted input: str) { let x = input + "_safe"; sql_query(x); }`
  — false negative (x is derived from tainted input but the coarse check
  only looks at direct sink calls, not propagation through assignments)

### Current State

- `verify_taint_propagation` in `src/typechecker.rs` — **IMPLEMENTED** with full
  data-flow tracking:
  - `TaintState` enum: `Untainted`, `Tainted`, `Sanitized`
  - `@tainted` params initialized as `Tainted` (parser updated to parse annotations)
  - Propagation through assignments, binary ops, string concat, f-strings, struct literals
  - Sanitizer calls (`sql_escape`, `shell_escape`, `html_escape`, `path_normalize`)
    produce `Sanitized` state
  - Sink calls (SQL, shell, HTML, filesystem) reject `Tainted` (not `Sanitized`) args
  - Return propagation: returning tainted from non-`@tainted` functions is rejected
  - Call propagation: taint flows through function call arguments
- `verify_tainted` (old coarse check) is now a no-op to avoid duplicate errors
  and false positives on sanitized code

### Full Implementation Plan

**Phase 1: Taint Lattice**
- Define `TaintState` enum: `Untainted`, `Tainted`, `Sanitized`
- Initialize: `@tainted` params → `Tainted`, all others → `Untainted`
- Sanitizer functions (annotated `@sanitizer`) produce `Sanitized` results

**Phase 2: Data-Flow Analysis**
- Build a per-function taint map: `HashMap<String, TaintState>`
- Walk statements in order:
  - `let x = expr;` → x's taint = join of all variables in expr
  - `x = expr;` → update x's taint
  - `call(arg1, arg2, ...)` → propagate taint to callee params
  - `return expr` → taint the return value
- Handle control flow (if/while/match): join taint from all branches

**Phase 3: Sink Checking**
- For each call to a sink function (SQL, shell, HTML, file path):
  - Check each argument's taint state
  - If `Tainted` (not `Sanitized`) → error with taint propagation path
- Sink categories:
  - SQL: `sql_query`, `sql_literal`, `db_query`, `db_exec`
  - Shell: `process_system`, `shell_cmd`, `shell_escape`
  - HTML: `html_literal`, `html_escape`
  - Path: `file_read`, `file_write`, `file_append`

**Phase 4: Sanitizer Recognition**
- Register sanitizer functions via `@sanitizer` annotation
- `@sanitizer fn sql_escape(s: str) -> str` — input is cleansed
- Calling a sanitizer on a `Tainted` value produces a `Sanitized` value
- Sanitized values can be passed to sinks without error

**Phase 5: Transitive Taint**
- If a function is called with a `Tainted` argument:
  - Mark that parameter as `Tainted` within the callee
  - Propagate through the callee's body
  - If the callee returns a tainted value, taint the call result
- This requires interprocedural analysis or conservative approximation

**Phase 6: Typed String Integration**
- Integrate with F67 typed strings (`SQLString`, `ShellString`, `HTMLString`,
  `PathString`)
- `@tainted str` → can only be converted to a typed string via a sanitizer
- Typed strings are inherently safe at sinks

### Files to Modify

| File | Changes |
|------|---------|
| `src/typechecker.rs` | Implement `verify_taint_propagation` with full data-flow |
| `src/ast.rs` | Add `@sanitizer` annotation parsing |
| `src/parser.rs` | Parse `@sanitizer` on functions |
| `stdlib/core.fd` | Mark `sql_escape`, `shell_escape`, `html_escape`, `path_normalize` as `@sanitizer` |

### Test Strategy

- `tests/bootstrap/test_taint_basic.fd` — direct taint to sink (should fail)
- `tests/bootstrap/test_taint_sanitized.fd` — taint through sanitizer (should pass)
- `tests/bootstrap/test_taint_propagation.fd` — taint through assignment (should fail)
- `tests/bootstrap/test_taint_transitive.fd` — taint through function call (should fail)
- `tests/bootstrap/test_taint_clean.fd` — non-tainted param to sink (should pass)
- `tests/bootstrap/test_taint_typed_string.fd` — typed string at sink (should pass)

---

## 19. SBOM Export

### Problem

Despite having rich capability and effect data, Fides cannot export a Software
Bill of Materials (SBOM). SBOMs are required for compliance in many domains
(US Executive Order 14028, EU Cyber Resilience Act, FDA medical device
guidance).

### Current State

- `fidesc sbom <file.fd> [--format cyclonedx|spdx] [-o output]` — STUB
- `generate_sbom` in `src/main.rs` — emits minimal CycloneDX 1.5 or SPDX 2.3 JSON
- Includes: imported modules, their capabilities, annotations used
- Missing: dependency versions, transitive dependencies, file hashes,
  vulnerability info, license info

### Full Implementation Plan

**Phase 1: Basic SBOM (DONE — stub)**
- Parse input file, extract imports and annotations
- Emit minimal CycloneDX 1.5 or SPDX 2.3 JSON
- Include component name, version, capabilities

**Phase 2: Dependency Graph**
- Resolve transitive imports (module A imports B, B imports C)
- Include full dependency tree in SBOM
- Detect circular dependencies
- For each component:
  - Name (module path)
  - Version (from `use module --version "1.0.0"`)
  - Capabilities granted (`IO`, `FS`, `Net`, `Spawn`, `Alloc`)
  - Annotations used (`@medical`, `@constant_time`, etc.)
  - File hash (SHA-256 of the `.fd` source)

**Phase 3: Capability Analysis**
- For each function in the program:
  - Effects declared (`!IO`, `!{IO,Net}`)
  - Capabilities required
  - Annotations attached
- Include in SBOM as component properties
- Flag functions that require capabilities not granted by any import

**Phase 4: Vulnerability Cross-Reference**
- Match component names + versions against vulnerability databases
- (Requires external tooling — NVD, OSV, GitHub Advisory)
- Include `vulnerabilities` array in CycloneDX output
- Flag known-vulnerable dependencies

**Phase 5: License Information**
- Parse license comments from `.fd` files (`// License: MIT`)
- Include license info per component
- Flag license conflicts (e.g., GPL in a proprietary project)

**Phase 6: Compiler Self-SBOM**
- `fidesc sbom --self` — generate SBOM for the compiler itself
- Include: Rust bootstrap dependencies (Cargo.lock), self-hosted modules
  (compiler/*.fd), stdlib modules (stdlib/*.fd)
- This is recursive: the compiler's SBOM includes itself

### Files to Modify

| File | Changes |
|------|---------|
| `src/main.rs` | Expand `generate_sbom` with transitive deps, hashes |
| `src/compiler.rs` | Add `resolve_imports()` for transitive dependency resolution |
| `src/ast.rs` | Add `license` field to `ImportDef` (from `// License:` comments) |
| `stdlib/core.fd` | Add license comments to stdlib modules |

### Output Format

CycloneDX 1.5:
```json
{
  "bomFormat": "CycloneDX",
  "specVersion": "1.5",
  "metadata": {
    "tools": [{"name": "fidesc", "version": "0.1.0"}],
    "component": {"name": "myapp.fd", "type": "application"}
  },
  "components": [
    {
      "name": "core",
      "version": "1.0.0",
      "type": "library",
      "hashes": [{"alg": "SHA-256", "content": "..."}],
      "properties": [
        {"name": "capabilities", "value": "IO,FS"},
        {"name": "annotations", "value": "@medical,@constant_time"}
      ]
    }
  ]
}
```

### Test Strategy

- `tests/sbom/test_cyclonedx.fd` — verify valid CycloneDX JSON output
- `tests/sbom/test_spdx.fd` — verify valid SPDX JSON output
- `tests/sbom/test_transitive.fd` — verify transitive deps are included
- `tests/sbom/test_hashes.fd` — verify file hashes are correct
- Validate output with `cyclonedx` CLI tool or JSON schema validation

---

## 20. Post-Codegen Constant-Time Verification

### Problem

`@constant_time` is verified at the source level in `verify_constant_time`
(`src/typechecker.rs:2257`), which checks:
- No IO/Net/Spawn effects
- No early returns
- No calls to non-`@constant_time` functions
- No calls to timing-sensitive runtime functions

However, the optimizer and code generator can introduce branches that the
source-level check cannot catch:
- **Strength reduction**: multiply → shift+add with a branch on sign
- **Register allocator**: spilling a secret to stack creates cache-timing
  patterns
- **Peephole optimizer**: CMOV → Jcc (branch) conversion
- **Bounds-check elimination**: may introduce data-dependent branches
- **Loop unrolling**: may introduce branch conditions on loop variables

### Current State

- `verify_constant_time_asm` in `src/codegen_x64.rs` — STUB (returns empty)
- `CtViolation` struct defined with function, line, instruction, reason
- No integration into the build pipeline yet

### Full Implementation Plan

**Phase 1: Assembly Parser**
- Parse the generated x86-64 assembly text (Intel syntax)
- Track current function (by label: `funcname:`)
- Track current instruction offset (line number)
- Identify instruction type: branch, CMOV, memory access, DIV, etc.

**Phase 2: Secret Value Tracking**
- Mark `@secret` parameters and variables
- Track which registers hold secret values at each instruction
- This requires data-flow analysis from PIR L3 → register assignment
- Conservative: if a register might hold a secret, treat it as secret

**Phase 3: Branch Detection**
- Flag conditional jumps (`jne`, `je`, `jg`, `jl`, `ja`, `jb`, `jge`, `jle`,
  `jz`, `jnz`, `js`, `jns`, etc.) whose condition depends on a secret register
- Flag indirect jumps (`jmp *reg`) with secret-derived targets
- Flag `CMOVcc` with a secret condition register
- Report: function, line, instruction, which register is secret

**Phase 4: Memory Access Detection**
- Flag memory accesses with secret-derived indices:
  - `mov rax, [rbx + rcx*8]` where `rcx` is secret → cache timing leak
  - `mov [rbx + rcx*8], rax` where `rcx` is secret → same
- This is the most common side-channel in real code (cache-based)

**Phase 5: Variable-Latency Instruction Detection**
- Flag `DIV` and `IDIV` with secret operands (variable latency on x86)
- Flag `MUL`/`IMUL` with secret operands (some CPUs have variable latency)
- Flag floating-point operations (variable latency, not relevant for
  `@constant_time` since floats are already banned by `@deterministic`)

**Phase 6: Integration into Build Pipeline**
- After `codegen_x64::generate_asm()`, call `verify_constant_time_asm()`
- If violations found:
  - `--strict-ct` → fail compilation
  - default → emit warnings
- Include violations in the qualification manifest (§17)

**Phase 7: Peephole-Aware Verification**
- The peephole optimizer (`src/peephole.rs`) can convert CMOV to Jcc
- Verify AFTER peephole optimization, not before
- Track which peephole passes are safe for `@constant_time` functions
- Disable unsafe peephole passes in `@constant_time` functions

### Files to Modify

| File | Changes |
|------|---------|
| `src/codegen_x64.rs` | Implement `verify_constant_time_asm` with full analysis |
| `src/peephole.rs` | Add `@constant_time`-aware pass filtering |
| `src/compiler.rs` | Call CT verification after codegen in `build_executable` |
| `src/main.rs` | Add `--strict-ct` flag to `build` and `codegen` commands |
| `src/ast.rs` | Add `@secret` annotation parsing (marks secret parameters) |

### Instruction Classification

| Instruction | CT-Safe? | Reason |
|-------------|----------|--------|
| `mov`, `add`, `sub`, `and`, `or`, `xor` | Yes | Constant latency |
| `shl`, `shr`, `sar` | Yes | Constant latency |
| `jcc` (conditional jump) | **Check** | If condition depends on secret → leak |
| `jmp` (unconditional) | Yes | No data dependency |
| `cmovcc` | **Check** | If condition depends on secret → leak |
| `mov [mem]` | **Check** | If address depends on secret → cache leak |
| `div`, `idiv` | **No** | Variable latency on x86 |
| `call` | Yes | If callee is `@constant_time` (checked at source) |
| `ret` | Yes | No data dependency |
| `nop`, `hlt` | Yes | No data dependency |

### Test Strategy

- `tests/bootstrap/test_ct_basic.fd` — `@constant_time` function with no branches (should pass)
- `tests/bootstrap/test_ct_branch.fd` — `@constant_time` function with `if secret` (should fail at codegen)
- `tests/bootstrap/test_ct_array.fd` — `@constant_time` function with `arr[secret]` (should fail)
- `tests/bootstrap/test_ct_div.fd` — `@constant_time` function with `secret / x` (should fail)
- `tests/bootstrap/test_ct_safe.fd` — `@constant_time` function with only arithmetic (should pass)
- `tests/bootstrap/test_ct_peephole.fd` — verify peephole doesn't break CT

---

## Implementation Priority

| Priority | Item | Effort | Blocks |
|----------|------|--------|--------|
| P1 | §18 Taint tracking | Medium | Security use cases (web, DB) |
| P1 | §19 SBOM export | Medium | Compliance (EO 14028, CRA) |
| P2 | §20 Post-codegen CT | Medium | Cryptographic use cases |
| P3 | §17 Certified toolchain | Very large | Safety-critical deployment |

§17 (certification) is last because it depends on the other three being
implemented first — you can't qualify a toolchain that has known gaps in
taint tracking, SBOM export, or constant-time verification.
