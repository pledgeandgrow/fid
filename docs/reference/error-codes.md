# Error Codes

Every error `fid` prints starts with its code: `error[E044]: ...`. This page
lists each code `fid` gives, what it means, a program that gets it and the
change that fixes it. `fid explain-error E044` (or `fid explain E044`)
prints a code's entry here.

The code comes from the message's text: compiler/compiler.fd
`classify_error` gives the class and `errcode_number` its number, and
`enrich_errors` puts it in front of the message, with the place of the error
(`--> file:line:col`), the source line and a caret under what the message is
about. Every message also carries a `Fix:` (or `Hint:`) line saying what to
write instead; scripts/diag-coverage.py checks that each message has one, a
code and a test.

The short table of all codes, `fidesc`'s included, is
[diagnostics.md](diagnostics.md). Each entry below names a test that gets
the error (tests/bootstrap/, `// expect-error:`); scripts/diag-tests-check.py
runs them.

Codes in the table that `fid` never gives: E021–E023 (fid's parser reports
those as E020), E049, E051–E057 and E081 (the classes are E050, E058, E079
and E080 in `fid`), E094–E096 (E097, E078) — they are `fidesc`'s.

## E000 — Uncategorized

An error whose text no rule of `classify_error` knows prints as plain
`error: ...`. Today no message of the checkers is uncategorized
(diag-coverage.py: "with a code: 100%"); a new one shows up as E000 until a
rule is added for its class.

Fix: read the message and its `Fix:` line; when it is a new class, add a
rule in `classify_error` (and its mirror in scripts/diag-coverage.py).

## E001 — Unterminated string literal

`Error:L:C: unterminated string`: a `"` is not closed on its line.

```fides
fn main() -> i64 {
    print("hello);
    return 0;
}
```

Fix: close the string, `print("hello");`.
Test: test_diag_lex_unterminated_string_fail.fd

## E002 — Unterminated f-string

`Error:L:C: unterminated f-string`: an `f"..."` is not closed.

```fides
fn main() -> i64 {
    val n: i64 = 3;
    print(f"n is {n});
    return 0;
}
```

Fix: `print(f"n is {n}");`.
Test: test_diag_lex_fstring_unterminated_fail.fd

## E003 — Unknown escape sequence

`Error:L:C: unknown escape sequence \q` (also a bad `\x` or `\u{...}`).

```fides
fn main() -> i64 {
    print("a\qb");
    return 0;
}
```

Fix: use one of `\n`, `\t`, `\r`, `\0`, `\\`, `\"`, `\'`, `\xNN`, `\u{...}`,
or write the backslash twice: `"a\\qb"`.
Test: test_diag_lex_unknown_escape_fail.fd

## E004 — Unexpected character

`Error:L:C: empty char literal`, `unexpected character`: a byte that starts
no token.

```fides
fn main() -> i64 {
    val c: i64 = '';
    return 0;
}
```

Fix: put one character in the literal (`'a'`), or remove the stray byte.
Test: test_diag_lex_empty_char_fail.fd

## E020 — Parse error

`Parse error at L:C: ...`: the tokens do not form the construct the parser
expects (a missing `)`, `}` or `;`, a keyword where a name goes, a `use`
with a version pin, text after the expression of an f-string).

```fides
use mathlib = "1.0";
fn main() -> i64 {
    return 0;
}
```

Fix: what the message's `Fix:` line says; here remove the pin, `use mathlib;`.
Tests: test_diag_parse_use_version_fail.fd, test_diag_tl2_fstr_trailing_fail.fd

## E040 — Undefined variable

`Type error (line N): undefined variable 'x'`.

```fides
fn main() -> i64 {
    var counter: i64 = 1;
    print(countr);
    return 0;
}
```

Fix: declare it (`val x = ...;`) before the line, or fix the spelling: the
message says `did you mean 'counter'?` when a name of the program is close
(`fid fix` applies it).
Test: test_tooling_did_you_mean_fail.fd

## E041 — Undefined function

`Type error (line N): undefined function 'f'`.

```fides
fn main() -> i64 {
    val d = undefined_fn(3);
    return 0;
}
```

Fix: define it (`fn undefined_fn(n: i64) -> i64 { ... }`), import the module
that has it (`use m;`), or fix the spelling (a close name is suggested, from
the program and from core).
Test: test_frontend_cascade_call_fail.fd

## E042 — Undefined struct

`Type error (line N): unknown struct 'Q'`: a struct literal of a type that
is not defined.

```fides
struct P { x: i64 }
fn main() -> i64 {
    val q = Q { x: 1 };
    return 0;
}
```

Fix: define it (`struct Q { x: i64 }`), import the module that defines it,
or fix the name (`P { x: 1 }`).

## E043 — Undefined field

`Type error (line N): struct 'S' has no field 'f'`.

```fides
fn set_count<T>(v: &mut T, x: i64) {
    v.count = x;
}
```

Fix: use a field the struct declares, or add it (`count: i64`); for a type
parameter, take the struct type itself (`v: &mut Counter`).
Test: test_error_generic_field_fail.fd

## E044 — Type mismatch

`Type error (line N): cannot assign str to variable 'x' of type i64.`, with
`Expected:` and `Found:` lines.

```fides
fn main() -> i64 {
    var x: i64 = "hello";
    return 0;
}
```

Fix: give a value of the declared type (`var x: i64 = 5;`), convert it
(`x as i64` for a number, `parse_int(s)` for a str), or declare the type the
value has (`var x: str = "hello";`).
Test: test_error_type_mismatch_fail.fd

## E045 — Condition is not a bool

`Type error (line N): if condition must be bool, got str` (also `while`).

```fides
fn main() -> i64 {
    var s: str = "yes";
    if s {
        println("x");
    }
    return 0;
}
```

Fix: compare to get a bool: `if s != "" { ... }`, `if n != 0 { ... }`.
Test: test_diag_tl2_if_cond_fail.fd

## E046 — Wrong number of arguments

`Type error (line N): function 'add' expects 2-2 args, got 1`.

```fides
fn add(a: i64, b: i64) -> i64 {
    return a + b;
}
fn main() -> i64 {
    println(add(1));
    return 0;
}
```

Fix: pass what the signature takes, `add(1, 2)`, or give the parameter a
default (`b: i64 = 0`).
Test: test_error_arity_fail.fd

## E047 — Missing return

`Type error (line N): 'f' is declared to return i64 but can reach the end of
its body without returning`.

```fides
fn f(x: i64) -> i64 {
    if x > 0 {
        return 1;
    }
}
```

Fix: return on every path: add `return 0;` at the end of the body (or an
`else` that returns).

## E048 — Use before initialization

`Init error: variable 'x' is used before being assigned a value.`

```fides
fn main() -> i64 {
    var x: i64;
    print(x);
    return 0;
}
```

Fix: give it a value first, `var x: i64 = 0;`, or assign it on every path
before the read.
Test: test_init_guarantee_fail.fd

## E050 — Linear value misused

`Linear type error (line N): ...`: a `@linear` value must be consumed exactly
once: not dropped, not used twice, not consumed in a loop or captured by a
closure.

```fides
@linear
struct Tok {
    v: i64,
}
fn mk() -> Tok {
    return Tok { v: 1 };
}
fn main() -> i64 {
    mk();
    return 0;
}
```

Fix: keep and consume it, `val t = mk(); use_tok(t);` (a function that takes
the value), or return it.
Test: test_bg186_linear_discard_fail.fd

## E058 — Reference escape

`Reference error (line N): in 'f', ...`: a reference would outlive what it
points to: kept in a global, a field, an element, a variant or a by-value
parameter, returned from a local, or used after its block ends
(compiler/escape.fd; references are second-class, reference.md section 8).

```fides
enum Slot {
    Full(i64),
    Empty,
}
fn mk(x: &i64) -> Slot {
    return Slot::Full(x);
}
```

Fix: keep a copy of the value (`Slot::Full(*x)`), or, for a struct that must
hold the reference, give it a lifetime parameter (`struct S<'a> { r: &'a T }`).
Tests: test_diag_tl2_esc_variant_fail.fd, test_diag_tl2_esc_global_fail.fd,
test_escape_return_fail.fd

## E059 — Secret or sensitive data reaches output

`Security error (@secret) (line N): in 'f', ...` (or `(@sensitive)`): a value
marked `@secret`/`@sensitive`, or computed from one, reaches output, a
return, a reference parameter, or decides whether output happens
(compiler/secrecy.fd).

```fides
fn f(@sensitive v: i64) {
    println(v);
}
```

Fix: output a value computed without it; release it through privacy's
`release(consent, v)` (a consent check); or mark the function `@declassify`
(an audited release) or `@sensitive`/`@secret` (its callers keep the label).
Tests: test_sensitive_fail.fd, test_diag_tl2_sec_return_sensitive_fail.fd

## E060 — Duplicate definition

`Duplicate definition: function 'h' is defined twice in module 'main'`, and
`Ambiguous name: ...` for a name two imported modules define.

```fides
fn h() -> i64 {
    return 1;
}
fn h() -> i64 {
    return 2;
}
```

Fix: remove one definition or rename it; for two modules, call it
qualified (`m::h()`) or rename one module's function.
Tests: test_duplicate_fn_fail.fd, test_diag_dup_core_type_fail.fd

## E061 — Taint violation

`Taint error: function 'f' is @tainted: its parameter 's' reaches the sink
'println' ... without passing through a sanitizer.`

```fides
fn log_input(@tainted s: str) !IO {
    println(s);
}
```

Fix: pass the value through a sanitizer before the sink (`sanitize(s)`,
`html_escape(s)`, `sql_escape(s)`, `shell_escape(s)`, ...), or do not hand
it to the sink.
Test: test_tainted_param_fail.fd

## E062 — Non-exhaustive match

`Type error: non-exhaustive match at line N in function 'main' — missing
variant(s): E::B, E::C`.

```fides
enum E { A, B, C }
fn main() -> i64 {
    val e = E::B;
    match e {
        E::A => { print(1); }
    }
    return 0;
}
```

Fix: add an arm per missing variant (`E::B => { ... }`), or a catch-all
`_ => { ... }`. An arm with a guard (`if ...`) covers nothing on its own.
Test: test_match_nonexhaustive_fail.fd

## E078 — Error of another check

The other checks' classes: `Event error`, `Redundancy error`, `Contract
error`, `Thread safety error`, `Partition error`, `Transaction error`,
`Serverless error`, `Differential privacy error`, `Comptime error`, `Shard
error`, `Quantization error`, `Linear type error` (when no linear rule
matched first), `Checkpoint error`, `Budget error`, `Visibility error`,
`Privacy error`, `Unknown annotation`, `Misplaced annotation`,
`Differentiability error`, `DP error`, `Adapter error`, `Hermetic error`,
`Allocator error`, `KV cache region error`, `Injection error`, and inline
assembly naming no 64-bit register (`asm! in f: ...`).

```fides
fn main() -> i64 {
    abort;
    return 0;
}
```

Fix: the message's `Fix:` line; here put the `abort;` inside a `txn { ... }`
block.
Test: test_txn_abort_outside_fail.fd (and the other `*_fail.fd` tests of
each class)

## E079 — Other type error

Any other `Type error`: a call or a method that does not exist on the type,
a generic parameter that cannot be inferred, an operator on the wrong types,
a struct literal that leaves out a field, units of different dimensions, a
trait impl whose method does not have the trait's signature, ...

```fides
fn zero<T>() -> T { return 0; }
fn main() -> i64 {
    println(zero());
    return 0;
}
```

Fix: the message's `Fix:` line; here give the type, `zero::<i64>()`.
Tests: test_generic_infer_fail.fd, test_diag_tl2_trait_return_line_fail.fd,
test_diag_tl2_bool_order_fail.fd

## E080 — Borrow conflict

`borrow error in f (line N): ...` and `Borrow error (line N): ...`: a change
to a value while a borrow of it is live, a second `&mut`, a change to a Vec
a `for` loop iterates over, a `@scoped` value kept past its scope. The
message names the borrow that holds it (`Held by: 'r' (&v, line 5)`).

```fides
@scoped
struct Msg {
    text: str,
}
fn main() {
    val m: Msg = Msg { text: "a" };
    var all: Vec<Msg> = Vec::new();
    all.push(m);
}
```

Fix: use the other borrow for the last time before the line, or take it in
an inner block `{ ... }` that ends first; collect changes and apply them
after the loop; for a `@scoped` value, copy what you need out of it
(`str_clone(m.text)`).
Tests: test_scoped_store_fail.fd, test_fix_borrow_line2_fail.fd

## E082 — Data race

`Race error (line N): 'f' runs on another thread and writes the global 'g'
with no lock.`

Fix: hold a mutex around the write (`p_mutex_lock(m); ...;
p_mutex_unlock(m);`), or use `p_shared_alloc` and the `p_atomic_` functions,
or keep the global on one thread.
Test: test_race_import_fail.fd

## E083 — Locks taken in inconsistent order

`Deadlock error: locks are taken in an inconsistent order: 'a' then 'b' in
..., 'b' then 'a' in ...`.

```fides
fn lock_both(x: i64, y: i64) -> i64 {
    p_mutex_lock(x);
    p_mutex_lock(y);
    p_mutex_unlock(y);
    p_mutex_unlock(x);
    return 1;
}
fn main() {
    val a: i64 = p_mutex_init();
    val b: i64 = p_mutex_init();
    var n: i64 = lock_both(a, b);
    n = n + lock_both(b, a);
}
```

Fix: take the locks in one order everywhere (`lock_both(a, b)` both times).
Test: test_lock_order_param_fail.fd

## E084 — Task never awaited, joined or detached

`Concurrency error (line N): the task started here in 'main' is never
awaited, joined or detached`.

```fides
fn work(n: i64) -> i64 {
    return n * 2;
}
fn main() {
    spawn work(1);
}
```

Fix: keep the handle and await it (`val t = spawn work(1); await t;`), start
it in `spawn scope { ... }`, or write `spawn detached work(1)`
(docs/reference/structured-concurrency.md).
Test: test_spawn_unjoined_fail.fd

## E090 — Capability denied

`Capability error: 'f' requires 'IO' capability (not granted to module 'm')`,
and calls the sandbox makes itself (`p_sandbox_exit()`).

```fides
fn main() -> i64 {
    sandbox {
        p_sandbox_exit();
    }
    return 0;
}
```

Fix: grant the capability where the module is imported (`use m with { IO };`
or ark.toml `[capabilities]`); write `sandbox { ... }` and let the compiler
enter and leave it.
Test: test_diag_tl2_sandbox_exit_fail.fd

## E091 — Sandbox violation

`Sandbox error: 'p_file_read()' has effect '!IO' -- all capabilities are
revoked inside sandbox`.

```fides
fn main() {
    sandbox {
        val s = p_file_read("C:/Windows/win.ini");
    }
}
```

Fix: make the call before or after the `sandbox { }` block and pass its
result in.
Test: test_sandbox_file_read_fail.fd

## E092 — Conservation violation

`Conservation error: ...`: a `@conserve` function creates or destroys money:
it changes a Money's amount directly, uses floating-point, has an effect,
calls a mint function or a function that is not `@conserve`.

```fides
use money;
@conserve
fn double_it(m: &mut Money) {
    m.cents = m.cents * 2;
}
```

Fix: move money between Money values (`Money_take(&mut m, cents)`), read it
with `Money_cents(&m)`, or remove `@conserve`.
Test: test_money_field_write_fail.fd

## E093 — Gas budget exceeded

`Gas error: function 'f' has @gas(50) but total gas cost is 408`.

```fides
@gas(50)
fn sum_to_100() -> i64 {
    var total: i64 = 0;
    for i in 0..100 {
        total = total + i;
    }
    return total;
}
```

Fix: raise the bound to the cost the message gives (`@gas(408)`), or do
less work in the function and its callees.
Test: test_gas_loop_fail.fd

## E097 — Safety annotation violated

`Safety error ...` and `Determinism error ...`: raw memory outside
`unsafe { }`, a `@crashfree`/`@bounded-stack`/`@deterministic` function that
breaks its promise (docs/reference/safety-rules.md).

```fides
fn main() -> i64 {
    var p: i64 = p_malloc(16);
    return 0;
}
```

Fix: wrap raw memory in `unsafe { ... }` (or use a Vec/Box); for an
annotation, remove what breaks it or the annotation.
Test: test_malloc_safe_fail.fd

## E098 — Constant-time violation

`Security error (constant_time) (line N): function 'f' ...` and
`Constant-time error`: a `@constant_time` function branches, returns early,
indexes or divides on secret data, has an effect, or calls a function that is
not `@constant_time`.

```fides
@constant_time
fn ct(a: i64, k: i64) -> i64 {
    var x: i64 = a;
    x /= k;
    return x;
}
```

Fix: select with arithmetic on a 0/1 mask instead of `if`, return once at
the end, index only with public values, and avoid `/` and `%` on secrets.
Test: test_bg150_ct_div_assign_fail.fd

## E099 — Real-time violation

`Realtime error`/`Real-time error`: a `@realtime` function has a blocking
effect, a `while` loop or recursion.

```fides
@realtime
fn tick(x: i64) -> i64 {
    println("tick");
    return x + 1;
}
```

Fix: do the blocking work outside the real-time function, loop a bounded
number of times (`for i in 0..N`), or remove `@realtime`.
Test: test_diag_realtime_io_fail.fd

## E110 — Effect not declared

`Effect error: function 'greet' has inferred effect '!Alloc' but only
declared effects not including it`.

```fides
fn greet(name: str) -> str !{} {
    return "hello, " + name;
}
```

Fix: declare the effect, `-> str !{Alloc}`, or remove what causes it.
Test: test_effect_pure_alloc_str_fail.fd

## E120 — No main function

`Panic: No main function found` from `fid run` on a file with no `main`
(`fid check` accepts a library file).

Fix: add `fn main() -> i64 { ...; return 0; }`, or run a file that has one.

## E999 — Internal compiler error

A check of the compiler's own output failed: the IR verifier (`function 'f':
block N does not end with a terminator`, ...) or the C split (`function 'f'
is defined twice (units a and b)`). The program is not at fault.

Fix: report it with the program; `fid reduce <file.fd>` shrinks it to a
small reproduction.
