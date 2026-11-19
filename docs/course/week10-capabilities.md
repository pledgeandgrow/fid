# Week 10 — Capability security: what a program may do at all

**Reading:** book chapter 5, "Capabilities" and "sandbox"
(`docs/book/05-effects.md`); reference §9.2, §9.3 and the `@secret` rows of
§11; `docs/reference/language-guide.md` §5.2 and §5.4;
`docs/pact-capabilities.md`.

**After this week you can:** state a threat model for running code you
did not write; grant a module only the capabilities it needs; run
untrusted code in a `sandbox`; list the unsafe code a program depends on
with `fid audit`; keep a secret from reaching an output.

## 1. Threat model

The question changes this week. Weeks 8 and 9 asked what *your* function
does. Now the code is someone else's: a library from a package registry,
a plugin, a scoring rule sent by a customer. You want to call it and be
sure it cannot read your files, open a socket or print your data. In the
**capability** model, authority to do something is a token that must be
handed over explicitly; code that was not given the token cannot act,
however it is written. Fid has three mechanisms, from coarse to strict:

| Mechanism | Scope | Follows calls? | Reference |
|---|---|---|---|
| `use m with { IO, FS }` | a whole program, per module | no: each call by name | §9.2 |
| `sandbox { … }` | a block | yes: inferred effects, transitive | §9.3 |
| `@secret` / `@declassify` | values | yes: information flow | §11 |

## 2. Granting capabilities

A `with { … }` clause on an import grants capabilities, and its presence
puts the whole program in **strict mode**: from then on, a call that needs
a capability nobody granted is an error, in the program *and* in the
modules it imports. The checked capabilities are `IO` (printing), `FS`
(files), `Net` (sockets, HTTP), `Spawn` (threads, processes) and `Alloc`
(raw allocation).

Here is a module that formats grades and can save a report:

```fides
// grades.fd
module grades;
use file;

pub fn letter(score: i64) -> str {
    if score >= 90 { return "A"; }
    if score >= 80 { return "B"; }
    if score >= 70 { return "C"; }
    return "F";
}

pub fn save_report(path: str, text: str) -> i64 {
    return file_write(path, text);
}
```

Granted what it needs, it works:

```fides
// grades_main.fd
use grades with { IO, FS };

fn main() -> i64 {
    var report: str = "";
    for s in [72, 95, 64] {
        println(int_to_str(s) + " is a " + letter(s));
        report = report + int_to_str(s) + " " + letter(s) + "\n";
    }
    save_report("grades.txt", report);
    return 0;
}
```

```
$ fid run grades_main.fd
72 is a C
95 is a A
64 is a F
$ cat grades.txt
72 C
95 A
64 F
```

Granted only `IO`, the module's file access is rejected, before the
program runs, though the call that needs `FS` is inside `grades`:

```fides
// grades_io_fail.fd
use grades with { IO };

fn main() -> i64 {
    println("72 is a " + letter(72));
    save_report("grades.txt", "72 C\n");
    return 0;
}
```

```
$ fid check grades_io_fail.fd
error: Capability error (module 'grades'): 'file_write()' requires 'FS' capability (not granted to module 'grades')
...
```

The limit to remember: the check is **per call, by name**. It does not
follow the call graph, so it knows that `file_write` needs `FS`, but a
function of yours that wraps `println` is not itself flagged as `IO`.
Treat capabilities as a fence around modules, not a proof.

## 3. sandbox

`sandbox { … }` revokes every capability inside the block, whatever was
granted, and it **does** follow calls: anything whose inferred effects
(week 8) include `IO`, `Net` or `Spawn` is refused, and so is anything
whose effects cannot be known (C functions, run-time loaded code, inline
assembly, calls through function values). Pure computation, including
allocation, is allowed:

```fides
// plugin.fd
// Untrusted scoring rules, as a plugin author might send them.
fn vowel_score(w: str) -> i64 {
    var n: i64 = 0;
    for i in 0..str_len(w) {
        val c: i64 = str_byte_at(w, i);
        if c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' { n = n + 1; }
    }
    return n * 10 + str_len(w);
}

fn ranked(words: Vec<str>) -> Vec<i64> {
    var out: Vec<i64> = Vec::new();
    for w in words { out.push(vowel_score(w)); }
    out.sort();
    return out;
}

fn main() -> i64 {
    var words: Vec<str> = Vec::new();
    words.push("audio");
    words.push("rhythm");
    words.push("queue");
    var scores: Vec<i64> = Vec::new();
    sandbox {
        scores = ranked(words);
    }
    for s in scores { println(int_to_str(s)); }
    return 0;
}
```

```
$ fid run plugin.fd
6
45
45
```

A plugin that tries to write a file or print, at any depth, is rejected:

```fides
// exfil_fail.fd
use file;

fn vowel_score(w: str) -> i64 {
    file_write("stolen.txt", w);
    return str_len(w);
}

fn phone_home(w: str) -> i64 {
    println("sending " + w);
    return 0;
}

fn main() -> i64 {
    var total: i64 = 0;
    sandbox {
        total = vowel_score("secret");
        total = total + phone_home("secret");
    }
    println(int_to_str(total));
    return 0;
}
```

```
$ fid check exfil_fail.fd
error[E091]: Sandbox error: 'vowel_score()' has effect '!IO' -- all capabilities are revoked inside sandbox, including in the functions it calls
error[E091]: Sandbox error: 'phone_home()' has effect '!IO' -- all capabilities are revoked inside sandbox, including in the functions it calls
```

A sandbox is a compile-time guarantee about *source code*. It does not
limit CPU time or memory (a plugin can still loop forever, which
`@crashfree` would exclude), and it trusts the compiler and the standard
library. Operating-system sandboxes (seccomp, AppContainer, WASI) are the
complementary run-time layer.

## 4. Auditing unsafe code

Everything above assumes the code obeys the language. `unsafe` code
(week 7) does not have to, so a security review starts by finding it.
`fid audit` lists every `unsafe` block a program reaches and whether it
has a `// SAFETY:` comment; `--strict` makes a missing comment fail the
command, for CI:

```fides
// slots.fd
struct Slots { base: i64, count: i64 }

fn slots_new() -> Slots {
    var base: i64 = 0;
    // SAFETY: freed exactly once, by slots_free
    unsafe { base = p_malloc(64); }
    return Slots { base: base, count: 0 };
}

fn slots_set(r: &mut Slots, slot: i64, x: i64) {
    if slot < 0 || slot >= 8 { return; }
    unsafe {
        val p: *mut i64 = (r.base + slot * 8) as *mut i64;
        *p = x;
    }
}

fn slots_free(r: Slots) {
    // SAFETY: base came from p_malloc in slots_new
    unsafe { p_free(r.base); }
}

fn main() -> i64 {
    var r: Slots = slots_new();
    slots_set(&mut r, 3, 42);
    slots_free(r);
    println("ok");
    return 0;
}
```

```
$ fid audit slots.fd --strict
slots.fd:7: unsafe block in slots_new - SAFETY: freed exactly once, by slots_free
slots.fd:13: unsafe block in slots_set - no SAFETY: comment
slots.fd:21: unsafe block in slots_free - SAFETY: base came from p_malloc in slots_new
stdlib (--stdlib lists them): ...
3 unsafe site(s) outside the stdlib, 1 without a SAFETY: comment
$ echo $?
1
```

The `stdlib` line counts the standard library's own unsafe sites
(`--stdlib` lists them): the size of the trusted base you depend on.

## 5. Information flow: secrets

Capabilities control *actions*; information-flow control controls
*data*. A `@secret` value may not reach an output, a log, a file, the
network, a global or another task, directly or through a branch that
depends on it (an implicit flow). A `@declassify` function is the audited
place where a value derived from a secret is released:

```fides
// fingerprint.fd
@wrapping
fn mix(h: i64, c: i64) -> i64 { return h * 31 + c; }

@declassify
fn fingerprint(@secret key: str) -> i64 {
    var h: i64 = 7;
    for i in 0..str_len(key) { h = mix(h, str_byte_at(key, i)); }
    return h % 100000;
}

fn main() -> i64 {
    @secret val key: str = "hunter2";
    println("key fingerprint " + int_to_str(fingerprint(key)));
    return 0;
}
```

```
$ fid run fingerprint.fd
key fingerprint 35979
```

```fides
// leak_fail.fd
fn log(s: str) { println("log: " + s); }

fn main() -> i64 {
    @secret val key: str = "hunter2";
    log(key);
    if str_len(key) > 6 { println("long key"); }
    return 0;
}
```

```
$ fid check leak_fail.fd
error: Security error (@secret) (line 6): in 'main', a secret value is passed to 'log', which outputs its parameter 's'
...
error: Security error (@secret) (line 7): in 'main', output 'println' runs inside a branch or loop that depends on a secret
...
```

The second error is the implicit flow: printing "long key" reveals one
bit of the key. (A five-digit fingerprint of a short key can be brute
forced, so whether `fingerprint` should be allowed to declassify is a
review question, not a compiler one.)

## Exercises

The lab for this week is [Lab 10 — Running untrusted code in a sandbox](labs/lab10-sandbox/README.md).

1. Write a module `stats` with pure functions, one that prints and one
   that reads a file, and a program that grants it only `{ IO }`. Which
   `stats` functions still compile? Remove the `with` clause: what does
   that tell you about the default?
2. Make `exfil_fail.fd` exfiltrate through a channel the sandbox does not
   see. Try: a global variable, a function value, an `extern "C"`
   function, `p_system`. Which are caught, and by which rule of
   reference §9.3?
3. Run `fid audit` on a program that uses `stdlib/http.fd`, with and
   without `--stdlib`. How many unsafe sites does a "hello world" web
   server trust?
4. `@secret` forbids a branch on a secret that prints. Does it also catch
   a loop whose *running time* depends on the secret? Look up
   `@constant_time` in reference §11 and say which attack it addresses
   and what it checks.
5. Write the threat model for a plugin system built on `sandbox`: what is
   trusted, what is not, and which attacks remain (denial of service,
   compiler bugs, side channels).

## Discussion

The object-capability tradition (E, Joe-E, WASI, Fuchsia) holds that
authority should be passed as values, not granted by name. Fid's `with {
… }` grants by name per module; `sandbox` removes everything for a
block. What would a value-passing design look like in Fid, given that
references cannot be stored (week 6)?
