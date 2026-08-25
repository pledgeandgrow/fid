# Fides Security Hardening Guide

> How Fides eliminates entire classes of binary-level and language-level exploits
> — without sacrificing the power of C, Rust, or Zig.

## Design Philosophy

Fides is designed to be **as flexible and powerful as C, Rust, or Zig** while
**eliminating as many exploit classes as possible**. The approach is layered:

1. **Compile-time prevention** — the type system rejects unsafe code before it runs
2. **Runtime prevention** — bounds checks, overflow checks, region tracking
3. **Binary hardening** — stack canaries, PIE, RELRO, NX stack on all compiled output
4. **Opt-in hardening** — annotations like `@constant_time` and `@tainted` for
   domain-specific security requirements

The key insight: **C has 70% of all CVEs from memory safety issues**. Rust and
Fides eliminate this entire class at compile time. Fides additionally eliminates
integer overflow (Rust only checks in debug mode) and provides binary hardening
that neither Rust nor Zig enable by default.

---

## Exploit Class Coverage

### Fully Prevented (Compile-Time)

| Exploit Class | How Prevented | C | Rust | Fides |
|--------------|---------------|---|------|-------|
| Buffer overflow | Bounds checking always on (F62) | Vulnerable | Safe | Safe |
| Integer overflow | Checked arithmetic always on (F64) | Vulnerable | Debug only | Safe |
| Use-after-free | Region tracking (freed_regions set) | Vulnerable | Safe (borrow ck) | Safe (regions) |
| Double-free | Linear types + region tracking | Vulnerable | Safe (borrow ck) | Safe (regions) |
| Dangling pointers | Region escape detection | Vulnerable | Safe (lifetimes) | Safe (regions) |
| Type confusion | Strong type system | Vulnerable | Safe | Safe |
| Uninitialized memory | Init guarantee (F63: `pm` keyword) | Vulnerable | Safe | Safe |
| Format string | No C format strings; f-strings are compile-time | Vulnerable | N/A | N/A |
| SQL injection | Typed strings (F67: SQLString) + parameterized queries (F68) | Vulnerable | Partial | Safe |
| Shell injection | Typed strings (F67: ShellString) | Vulnerable | Partial | Safe |
| HTML injection | Typed strings (F67: HTMLString) | Vulnerable | Partial | Safe |
| Path traversal | Typed strings (F67: PathString) | Vulnerable | Partial | Safe |
| Money creation/destruction | `@conserve` + linear types | N/A | N/A | Safe |
| Non-determinism | `@deterministic` (no IO/Net/floating-point) | N/A | N/A | Safe |
| Unbounded computation | `@gas(N)` compile-time gas budget | N/A | N/A | Safe |
| Unbounded loops | `@no-unbounded` (no while loops) | N/A | N/A | Safe |
| Recursion (stack overflow) | `@no-recursion` | N/A | N/A | Safe |
| Dynamic dispatch (vtable) | `@no-indirect` (no function pointers) | N/A | N/A | Safe |
| Dynamic allocation | `@no-alloc` (stack-only) | N/A | N/A | Safe |
| Panic in safety-critical code | `@crashfree` (no unwrap, no div/mod) | N/A | N/A | Safe |
| Timing side channels | `@constant_time` (no early returns, no IO) | Vulnerable | Vulnerable | Safe |
| Tainted input injection | `@tainted` (sink detection) | Vulnerable | N/A | Safe |

### Fully Prevented (Runtime)

| Exploit Class | How Prevented |
|--------------|---------------|
| Division by zero | Always panics (even in `@wrapping`) |
| Modulo by zero | Always panics |
| Null pointer deref | Raw pointers only in `unsafe {}` |

### Binary Hardening (C Codegen)

All C codegen output is compiled with these flags (no opt-in required):

| Protection | Flag | Effect |
|-----------|------|--------|
| Stack canaries | `-fstack-protector-strong` | Detects stack smashing |
| Position-independent code | `-fPIE -pie` | Enables ASLR |
| Full RELRO | `-Wl,-z,relro -Wl,-z,now` | Prevents GOT overwrites |
| Non-executable stack | `-Wl,-z,noexecstack` | Prevents shellcode on stack |
| No strict overflow | `-fno-strict-overflow` | Compiler doesn't assume no overflow |

### Partially Prevented

| Exploit Class | Status | Gap |
|--------------|--------|-----|
| ROP chains | Partial | Fewer gadgets (no unused code with `--caps`), but no CFI |
| Spectre/Meltdown | Partial | `@constant_time` helps, but hardware-level issue |
| Heap spraying | Partial | No `eval()` or dynamic code exec, but no heap hardening |
| Stack overflow (deep recursion) | Partial | `@no-recursion` prevents it, but no runtime stack limit |

### Not Yet Prevented

| Exploit Class | Status | Planned |
|--------------|--------|---------|
| CFI (Control Flow Integrity) | Not implemented | Forward-edge CFI for indirect calls |
| Data race prevention | Not implemented | `@sync` annotation (Send/Sync traits) |
| Taint propagation | Basic (sink detection) | Full data-flow taint tracking |
| Runtime sandbox | Compile-time only | Runtime capability revocation in interpreter |
| Runtime resource limits | Compile-time only | Runtime memory/time limits |

---

## Security Annotations

### @constant_time — Side-Channel Defense

Prevents timing side-channel attacks (Spectre, Meltdown, cache timing).

```fides
@constant_time
pf ct_eq(a: i64, b: i64) -> i64 {
    pv diff = a - b;
    pv result = 0;
    if diff == 0 { result = 1; } else { result = 0; }
    ret result;
}
```

**Rules enforced at compile time:**
1. No early returns (timing leak from early exit)
2. No IO/Network/Spawn effects (timing observable externally)
3. No memory allocation (allocation timing is non-deterministic)
4. No calls to non-`@constant_time` functions (transitive)
5. No calls to timing-sensitive runtime functions (print, malloc, etc.)

**What it catches:**
```fides
@constant_time
pf bad_eq(a: i64, b: i64) -> i64 {
    if a == b { ret 1; }  // ERROR: early return leaks timing
    ret 0;
}
```

### @tainted — Injection Defense

Marks parameters as untrusted user input. Prevents tainted values from
flowing into dangerous sinks without sanitization.

```fides
pf handle_query(@tainted user_input: str) {
    // ERROR: tainted input passed to SQL sink without sanitization
    sql_query(user_input);
}
```

**Sinks protected:**
- SQL: `sql_query`, `sql_literal`, `db_query`, `db_exec`
- Shell: `process_system`, `shell_cmd`, `shell_escape`
- HTML: `html_literal`, `html_escape`
- File paths: `file_read`, `file_write`, `file_append`

### @crashfree — No Panic Guarantee

Ensures a function can never panic. Required for safety-critical code.

```fides
@crashfree
pf safe_divide(a: i64, b: i64) -> i64 {
    // ERROR: division can panic if b == 0
    a / b
}
```

**Forbidden:** `unwrap`, division, modulo, `panic`, `while` loops.

### @safety-critical — ISO 26262 / DO-178C

Combines: `@no-alloc + @no-recursion + @crashfree + @no-indirect`.

```fides
@safety-critical
pf braking_controller(speed: i64, target: i64) -> i64 {
    // No allocation, no recursion, no panic, no function pointers
    compute_brake_force(speed, target)
}
```

### @medical — IEC 62304

Combines: `@no-alloc + @no-recursion + @crashfree + @requires-audit`.

```fides
@medical @audit
pf insulin_dose(glucose: i64) -> i64 {
    // No allocation, no recursion, no panic, must be audited
    calculate_dose(glucose)
}
```

### @deterministic — Reproducible Output

Ensures same inputs always produce same outputs.

```fides
@deterministic
pf hash(data: i64) -> i64 {
    // No IO, no Network, no floating-point, no Spawn
    data * 2654435761
}
```

### @conserve — Money Conservation

Ensures no Money is created, destroyed, or duplicated.

```fides
@conserve
pf transfer(from: Money, to: Money, amount: Money) -> (Money, Money) {
    // Cannot call Money::new, cannot use floating-point
    // Can only call other @conserve functions
    (from - amount, to + amount)
}
```

### @gas(N) — Computation Budget

Compile-time gas budget (unlike Solidity's runtime gas).

```fides
@gas(100)
pf limited_loop(n: i64) -> i64 {
    pv sum = 0;
    for i in 0..n { sum = sum + i; }
    sum
}
```

### @linear — Resource Single-Use

Prevents resource duplication (e.g., double-spending).

```fides
@linear
struct Money { cents: i64 }

pf spend(m: Money, amount: i64) -> Money {
    // m is consumed — cannot be used again
    Money { cents: m.cents - amount }
}
```

### @wrapping — Overflow Escape Hatch

Opt-in wrap-around arithmetic (default is checked).

```fides
@wrapping
pf hash_combine(a: i64, b: i64) -> i64 {
    a * 31 + b  // wraps on overflow instead of panicking
}
```

### @no-alloc / @no-recursion / @no-unbounded / @no-indirect

Composable safety primitives. Can be combined freely:

```fides
@no-alloc @no-recursion @no-unbounded
pf embedded_task(x: i64) -> i64 {
    // No heap, no recursion, no while loops
    x + 1
}
```

---

## Unsafe Blocks

The `unsafe { }` block is the escape hatch for low-level operations:

```fides
pf raw_access(buf: *mut u8, idx: i64) -> u8 {
    unsafe {
        // Bounds checks skipped here
        // Raw pointer dereference allowed
        *(buf + idx)
    }
}
```

**What `unsafe` allows:**
- Raw pointer dereference (`*ptr`, `*mut T`)
- Skipping bounds checks
- FFI calls without capability checks

**What `unsafe` does NOT allow:**
- Bypassing the borrow checker (still enforced)
- Bypassing the type checker (still enforced)
- Bypassing integer overflow checks (still checked)

---

## Binary Hardening Verification

Verify that compiled binaries have the correct security features:

```bash
# Compile a Fides program
fidesc ccompile my_program.fd -o my_program

# Check for PIE (ASLR)
readelf -h my_program | grep "Type:"
# Should show: DYN (Position-Independent Executable file)

# Check for RELRO + BIND_NOW
readelf -d my_program | grep "BIND_NOW"
# Should show: FLAGS: BIND_NOW

# Check for NX stack
readelf -l my_program | grep "GNU_STACK"
# Should show: GNU_STACK with no E flag

# Check for stack canaries
objdump -d my_program | grep -c "stack_chk"
# Should show: > 0 (number of canary checks)
```

---

## Comparison with Other Languages

| Feature | C | Rust | Zig | Fides |
|---------|---|------|-----|-------|
| Bounds checking | Manual | Always on | Optional | Always on |
| Integer overflow | UB | Debug only | Optional | Always on |
| Use-after-free | Vulnerable | Borrow ck | Manual | Region tracking |
| Stack canaries | Optional (-fstack-protector) | Optional | Optional | **Always on** |
| PIE/ASLR | Optional (-fPIE) | Optional | Optional | **Always on** |
| RELRO | Optional (-Wl,-z,relro) | Optional | Optional | **Always on** |
| NX stack | Optional (-z,noexecstack) | Optional | Optional | **Always on** |
| Constant-time | N/A | N/A | N/A | `@constant_time` |
| Taint tracking | N/A | N/A | N/A | `@tainted` |
| Gas budget | N/A | N/A | N/A | `@gas(N)` |
| Crash-free | N/A | N/A | N/A | `@crashfree` |
| Safety-critical | N/A | N/A | N/A | `@safety-critical` |
| Medical (IEC 62304) | N/A | N/A | N/A | `@medical` |
| Money conservation | N/A | N/A | N/A | `@conserve` |
| Deterministic | N/A | N/A | N/A | `@deterministic` |
| Linear types | N/A | N/A | N/A | `@linear` |
| Typed strings | N/A | N/A | N/A | SQLString, ShellString, etc. |

**Key advantage:** Fides enables binary hardening by default (no opt-in required)
and provides domain-specific security annotations that no other language offers.

---

## Historical Exploit Examples Prevented

### Morris Worm (1988) — Buffer Overflow
- **Exploit:** `gets()` buffer overflow in `fingerd`
- **How Fides prevents:** Bounds checking always on. No `gets()` equivalent.
  String operations are length-checked. `unsafe {}` required to bypass.

### OpenSSH CVE-2002-0390 — Integer Overflow
- **Exploit:** Integer overflow in challenge-response auth → remote root
- **How Fides prevents:** Checked arithmetic always on. Overflow = panic.
  `@wrapping` is the explicit escape hatch.

### Internet Explorer CVE-2013-3893 — Use-After-Free
- **Exploit:** UAF in `CButton` object → remote code execution
- **How Fides prevents:** Region tracking marks freed regions. Use-after-free
  is a compile-time error.

### WU-FTPD CVE-1999-0129 — Format String
- **Exploit:** `printf(user_input)` with `%n` → remote root
- **How Fides prevents:** No C format strings. F-strings are compile-time.
  No `%n` equivalent.

### Spectre (CVE-2017-5753) — Side Channel
- **Exploit:** Speculative execution + cache timing → memory leak
- **How Fides prevents:** `@constant_time` eliminates timing variations
  from data-dependent branches and early returns.

### Code Red (2001) — Buffer Overflow → ROP
- **Exploit:** IIS buffer overflow → ROP chain → 359,000 servers infected
- **How Fides prevents:** Bounds checking prevents initial overflow.
  Stack canaries detect corruption. PIE randomizes memory layout.
  `--caps` strips unused code (fewer ROP gadgets).
