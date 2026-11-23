# Week 13 — Concurrency: the four themes together, and the project

**Reading:** book chapter 8 (`docs/book/08-concurrency.md`); reference §10;
proposal 0003 (`docs/proposals/0003-memory-model-and-cancellation.md`);
book chapter 9 (`docs/book/09-service.md`) for the project.

**After this week you can:** run work on threads and tasks; explain how
ownership, second-class references, effects and the panic rule together
rule out data races on the values a program shares; say where that
argument stops; scope and start the final project.

## 1. Why this comes last

A data race is two threads accessing the same memory, at least one
writing, without synchronisation. Every theme of the course takes part
in keeping them out of Fid programs:

| Theme | Its part |
|---|---|
| Ownership (week 5) | a value sent to another thread is *copied* (proposal 0004, rule 8): the thread gets its own value, and the counts are never shared |
| Second-class references (week 6) | a reference cannot be sent: nothing on another thread can point into your stack |
| Effects and checks (weeks 8–9) | a function that runs on another thread may not write a global without calling a lock |
| Capabilities (week 10) | `Spawn` is a capability: a sandbox cannot start threads at all |
| The panic rule (week 3) | a panic on any thread ends the whole program: no half-updated state is observed afterwards |

## 2. Threads and copies

`spawn f(args)` runs the call on a new OS thread and gives a `Future`;
`await` waits for its result. Every argument must be `@send`: numbers,
`str`, enums, structs of such values, and `Vec`/`HashMap`/`Option` of
them. References and raw pointers are not:

```fides
// halves.fd
fn count_long(words: Vec<str>, min: i64) -> i64 {
    var n: i64 = 0;
    for w in words {
        if str_len(w) >= min { n = n + 1; }
    }
    return n;
}

fn split_half(words: &Vec<str>, first: bool) -> Vec<str> {
    var out: Vec<str> = Vec::new();
    val mid: i64 = words.len() / 2;
    for i in 0..words.len() {
        if (i < mid) == first { out.push(words.get(i)); }
    }
    return out;
}

fn main() -> i64 {
    val words: Vec<str> = str_split("ownership borrows regions effects capabilities sandbox layouts ffi", " ");
    val left: Vec<str> = split_half(&words, true);
    var right: Vec<str> = split_half(&words, false);
    val a = spawn count_long(left, 7);
    val b = spawn count_long(right, 7);
    right.push("concurrency");
    println("long words: " + int_to_str((await a) + (await b)) + " of " + int_to_str(words.len()));
    println("right now holds " + int_to_str(right.len()) + "; the thread saw its own copy");
    return 0;
}
```

```
$ fid run halves.fd
long words: 7 of 8
right now holds 5; the thread saw its own copy
```

`main` pushed to `right` while a thread was reading it, and nothing went
wrong: the thread had its own `Vec`. Passing a reference instead is
rejected, which is week 6's escape rule seen from the other side:

```fides
// send_fail.fd
fn fill(v: &mut Vec<i64>) -> i64 {
    v.push(1);
    return v.len();
}

fn main() -> i64 {
    var v: Vec<i64> = Vec::new();
    val f = spawn fill(&mut v);
    v.push(2);
    println(int_to_str(await f));
    return 0;
}
```

```
$ fid check send_fail.fd
error[E078]: Thread safety error (line 9): argument 1 of the spawned call has type &mut Vec<i64>, which is not @send.
    Fix: pass a value (numbers, strings, structs of such values) or mark the struct @send.
...
```

Globals are the remaining shared state, and writing one from a thread
requires a lock (`stdlib/sync.fd`):

```fides
// race_fail.fd
var served: i64 = 0;

fn handle(id: i64) -> i64 {
    served = served + 1;
    return id;
}

fn main() -> i64 {
    val a = spawn handle(1);
    val b = spawn handle(2);
    println(int_to_str((await a) + (await b)));
    return 0;
}
```

```
$ fid check race_fail.fd
error[E082]: Race error (line 5): 'handle' runs on another thread and writes the global 'served' with no lock.
    Fix: hold a mutex around the write, or use p_shared_alloc and the p_atomic_ functions.
...
```

## 3. Where the argument stops

Read the language guide's "What Safe Code Promises" again, now as a
reviewer. The race check accepts a function that calls *any* function
named like a lock, without checking that the lock is held around the
write; memory shared through `p_shared_alloc`, raw pointers or C is not
checked; `@sync` is recorded and not enforced. Tasks (`task_go`, `async`)
are scheduled M:N in compiled programs and switch only where they wait
under `fid run` (docs/reference/interp-tasks.md), so their interleaving, and any output that depends on it, can
differ between the two (reference §10.7 says what is ordered). These are
the open edges of the design, and good material for a project.

## 4. The final project

Teams of two or three, five weeks (proposal in week 9, demo in the exam
period). The deliverable is a Fid program or a change to the compiler,
with tests that run under `fid run` and at least one native back end,
and a report of 6–10 pages. Suggested topics, one per theme:

- **Memory safety.** Implement a bounded LRU cache whose eviction is
  `@no-alloc` after warm-up, and prove with `--leaks` and
  `--checked-alloc` that it neither leaks nor touches freed memory.
  Or: make `drop` methods run at the end of the enclosing block (week 5)
  and measure what changes in the test suite.
- **Effect systems.** Close the `@deterministic` float gap of week 9 in
  `compiler/typechecker.fd`, with negative tests; or make a `do` nested
  in an `if` capture its continuation (week 8).
- **Capability security.** A plugin host: plugins are Fid modules run in
  `sandbox`, given input and returning output as values; a threat-model
  document and an attack suite of rejected plugins.
- **Systems programming.** A binary format (PNG chunks, ELF headers, a
  network protocol) parsed with `@packed` layouts and checked against a
  C implementation through FFI; or a new peephole rule in the machine
  back end, checked with `FID_VERIFY=1` and `scripts/pass-matrix.sh`.

Every project report answers the course's recurring question for its own
code: *what does the contract say, where is it enforced, and where does
it stop?*

## Exercises

The lab for this week is [Lab 13 — Threads that share nothing, and one counter that is shared](labs/lab13-threads/README.md).

1. Make `race_fail.fd` correct with `sync_mutex_lock`/`unlock`. Then
   write a version the checker accepts that is still racy, and explain
   why the checker cannot see it.
2. Rewrite `halves.fd` with `parallel { | … | … }` (reference §10.3) and
   with `use channel;` (send each count over a channel). Which version
   copies the most data?
3. Explain, using only rules from this course, why two threads can never
   free the same `str`.
4. A task panics while holding a mutex. What does the panic rule say
   happens to the other tasks, and why does that make "poisoned locks"
   (Rust's `PoisonError`) unnecessary in Fid?

## Discussion

Rust prevents data races with `Send`/`Sync` and the borrow checker; Go
leaves them to a run-time race detector; Erlang shares nothing. Where is
Fid on that line, and what does its position cost the programmer who
needs a shared, mutable cache?
