# 8. Concurrency

Fid has two kinds of concurrent work, and it helps to keep them apart:

- **Tasks** are green threads: many of them share a few OS threads, each
  with its own small stack. They are cheap, and they are what you use to
  wait on many things at once — sockets, timers, slow calls. `use task;`
  and `async fn` give you tasks.
- **Threads** are OS threads. `spawn f(x)` and `parallel { … }` start
  them. They are what you use to keep several cores busy.

Both are checked the same way: what crosses to another thread must be
`@send`, references never cross, and a thread may not write a global
without a lock. And one rule covers both: **a panic anywhere ends the
whole program.**

## Tasks

`task_go(name, arg)` starts the function called `name` — a top-level
function taking one `i64` and returning an `i64` — as a task, and gives
back a handle. `task_run()` waits for every task to finish;
`task_result(t)` reads what one returned:

```fides
// tasks.fd
use task;

fn worker(n: i64) -> i64 {
    var total: i64 = 0;
    for i in 1..=n {
        total = total + i;
        task_yield();
    }
    return total;
}

fn main() -> i64 {
    val a = task_go("worker", 10);
    val b = task_go("worker", 100);
    task_run();
    println("sum 1..10  = " + int_to_str(task_result(a)));
    println("sum 1..100 = " + int_to_str(task_result(b)));
    return 0;
}
```

```
$ fid run tasks.fd
sum 1..10  = 55
sum 1..100 = 5050
```

The functions of `stdlib/task.fd`:

| Function | Does |
|---|---|
| `task_go(name, arg) -> i64` | start `name(arg)` as a task |
| `task_run() -> i64` | wait for all tasks (and tasks they start) |
| `task_result(t)`, `task_done(t)` | the result; whether it has finished |
| `task_yield()` | let the others run |
| `task_sleep(ms)` | wait `ms` milliseconds while others run |
| `task_wait_read(fd, ms)`, `task_wait_write(fd, ms)` | wait for a socket, with a timeout (`-1`: none) |
| `task_detach(t)` | nobody will read the result: free it when done |
| `task_join_timeout(t, ms)`, `task_timeout(name, arg, ms)` | wait at most `ms`; `-1` on timeout (the task keeps running) |

In compiled programs (C and native) tasks are scheduled M:N over a
worker per CPU, and they also switch at blocking calls the runtime can
poll and at loop safepoints the compiler inserts, so one busy loop does
not starve the rest. **Under `fid run` a task runs to its end as soon as
it starts** — the interpreter has a single call stack — so `task_yield`
does nothing there. Output order that depends on interleaving can differ
between `fid run` and a build.

### Structured groups: TaskScope

A `TaskScope` starts tasks together and joins them together; results
come back in the order the tasks were started, whatever order they
finish in:

```fides
// scope.fd
use task;

fn fetch(id: i64) -> i64 {
    task_sleep(10 * id);
    return id * 100;
}

fn main() -> i64 {
    var s: TaskScope = task_scope_new();
    task_scope_go(&mut s, "fetch", 3);
    task_scope_go(&mut s, "fetch", 1);
    task_scope_go(&mut s, "fetch", 2);
    val results: Vec<i64> = task_scope_join(&mut s);
    for r in results { println(int_to_str(r)); }
    return 0;
}
```

```
$ fid run scope.fd
300
100
200
```

`task_scope_cancel(&mut s)` sets a flag that the children poll with
`task_scope_cancelled`; cancellation is cooperative, a running task is
never killed.

### async and await

An `async fn` is a task with typed arguments and a typed result. Declare
its result as `Future<T>` and return a plain `T` from the body; `await`
gives the `T`. The call starts the task, which runs until it first waits;
then the caller continues:

```fides
// async.fd
use task;

async fn fetch(id: i64) -> Future<i64> {
    task_sleep(20);
    return id * 10;
}

fn main() -> i64 {
    val a = fetch(1);
    val b = fetch(2);
    val c = fetch(3);
    println(int_to_str((await a) + (await b) + (await c)));
    return 0;
}
```

```
$ fid run async.fd
60
```

The three sleeps overlap: built through C with 1000 ms sleeps, the
program finishes in about 1.4 s, not 3. `f64` and struct arguments and results are carried through an
adapter. An `async fn` with a reference parameter cannot become a task
(a reference cannot reach another task), so it runs to its end at the
call, on the caller's stack. `async` methods work in `impl` blocks and
through `dyn`; async methods of generic impls do not.

## Threads

### spawn

`spawn f(args)` runs the call on a new OS thread and gives a `Future` of
its result; `await` joins it:

```fides
// spawn.fd
fn count_primes(limit: i64) -> i64 {
    var n: i64 = 0;
    for x in 2..limit {
        var prime: bool = true;
        var d: i64 = 2;
        while d * d <= x {
            if x % d == 0 { prime = false; break; }
            d = d + 1;
        }
        if prime { n = n + 1; }
    }
    return n;
}

fn main() -> i64 {
    val f1 = spawn count_primes(1000);
    val f2 = spawn count_primes(2000);
    val a = await f1;
    val b = await f2;
    println("primes below 1000: " + int_to_str(a));
    println("primes below 2000: " + int_to_str(b));
    return 0;
}
```

```
$ fid run spawn.fd
primes below 1000: 168
primes below 2000: 303
```

Raise the limits to see the two threads use two cores — in a C or native
build. `fid run` is an interpreter, far slower than compiled code; it is
the place to check results, not speed.

### parallel

`parallel { … }` is fork-join in one statement. Branches start with `|`;
a branch `x = f(…)` runs on its own thread and `x` gets the result when
all branches have joined at the closing brace:

```fides
// parallel.fd
fn fib(n: i64) -> i64 {
    if n < 2 { return n; }
    return fib(n - 1) + fib(n - 2);
}

fn main() -> i64 {
    var a: i64 = 0;
    var b: i64 = 0;
    parallel {
        | a = fib(20);
        | b = fib(21);
    }
    println(int_to_str(a) + " " + int_to_str(b) + " " + int_to_str(a + b));
    return 0;
}
```

```
$ fid run parallel.fd
6765 10946 17711
```

A branch that is one call is spawned; a branch that reads only
word-sized locals and assigns none runs as a function of its own on its
own thread; any other branch runs in order on the current thread.

`spawn scope { … }` joins every thread spawned inside it at its end, and
`cancel scope;` cancels the scope's pending tasks.

### What may cross: @send

Every argument of a spawned call, and every variable a spawned expression
names, must be `@send`. Numbers, `bool`, `str`, enums, structs whose
fields are all `@send` (or that are marked `@send`), and `Vec`, `HashMap`,
`Option`, `Result`, `Box`, `Future`, `Mutex`, `Channel` of such types
are. **References and raw pointers are not:**

```fides
fn sum(v: &Vec<i64>) -> i64 { … }

fn main() -> i64 {
    var v: Vec<i64> = Vec::new();
    v.push(1);
    val f = spawn sum(&v);       // rejected
    println(int_to_str(await f));
    return 0;
}
```

```
error: Thread safety error (line 10): argument 1 of the spawned call has type &Vec<i64>, which is not @send.
    Fix: pass a value (numbers, strings, structs of such values) or mark the struct @send.
```

Pass the `Vec` itself. Assigning or passing a `Vec` gives the callee its
own value (shared until one side writes, chapter 4), so the caller can
keep using its copy:

```fides
fn total(v: Vec<i64>) -> i64 {
    var s: i64 = 0;
    for x in v { s = s + x; }
    return s;
}

fn main() -> i64 {
    var v: Vec<i64> = Vec::new();
    for i in 1..=100 { v.push(i); }
    val f = spawn total(v);
    v.push(1000);
    println(int_to_str(await f) + " " + int_to_str(v.len()));   // 5050 101
    return 0;
}
```

### Globals need a lock

A function that runs on another thread — or anything it calls — may not
assign a global of your program unless it calls a lock:

```fides
var hits: i64 = 0;

fn bump(n: i64) -> i64 {
    hits = hits + n;         // rejected when bump is spawned
    return n;
}
```

```
error: Race error (line 4): 'bump' runs on another thread and writes the global 'hits' with no lock.
    Fix: hold a mutex around the write, or use p_shared_alloc and the p_atomic_ functions.
```

With a mutex from `stdlib/sync.fd`:

```fides
// mutex.fd
use sync;

var hits: i64 = 0;
var lock: i64 = 0;

fn bump(n: i64) -> i64 {
    for i in 0..n {
        sync_mutex_lock(lock);
        hits = hits + 1;
        sync_mutex_unlock(lock);
    }
    return n;
}

fn main() -> i64 {
    lock = sync_mutex_create();
    val a = spawn bump(1000);
    val b = spawn bump(2000);
    val total = (await a) + (await b);
    println(int_to_str(total) + " " + int_to_str(hits));
    return 0;
}
```

```
$ fid run mutex.fd
3000 3000
```

Know what the check is: it accepts a function that calls anything whose
name ends in `mutex_lock` or `write_lock`. It does not check that the
lock is held *around* the write, and data shared through pointers is not
checked at all. `sync.fd` also has read/write locks
(`sync_rwlock_*`), atomics (`sync_atomic_alloc`, `_load`, `_store`,
`_add`, `_cas`) and `with_lock(m, f)`.

## Channels

`use channel;` gives bounded queues between threads. A channel carries
`i64` values only, and is itself an `i64` handle:

```fides
// pipeline.fd
use channel;

fn producer(ch: i64, n: i64) -> i64 {
    for i in 1..=n {
        channel_send(ch, i * i);
    }
    channel_close(ch);
    return n;
}

fn main() -> i64 {
    val ch = channel_create(4);
    val done = spawn producer(ch, 5);
    var total: i64 = 0;
    loop {
        val v = channel_recv(ch);
        if v == CHAN_CLOSED() { break; }
        println("got " + int_to_str(v));
        total = total + v;
    }
    val sent = await done;
    println(int_to_str(sent) + " values, total " + int_to_str(total));
    return 0;
}
```

```
$ fid run pipeline.fd
got 1
got 4
got 9
got 16
got 25
5 values, total 55
```

`channel_send` blocks while the channel is full and returns `false` once
it is closed; `channel_recv` blocks while it is empty and returns
`CHAN_CLOSED()` when it is closed and drained; `channel_try_recv` never
blocks. To send something bigger than a word, keep the data in a `Vec`
and send indexes, or send a handle your own code knows how to look up.

`select` takes the first channel that has a value:

```fides
use channel;

fn main() -> i64 {
    val fast = channel_create(1);
    val slow = channel_create(1);
    channel_send(slow, 2);
    select {
        case fast => v => { println("fast " + int_to_str(v)); }
        case slow => v => { println("slow " + int_to_str(v)); }
        default => { println("nothing ready"); }
    }
    return 0;
}
```

This prints `slow 2` from a C build. Under `fid run` it currently crashes
(`Invalid memory access`), so try `select` through `fid ccompile`.

## Panics in concurrent code

A panic is never confined to the task or thread that raised it:

```fides
use task;

fn job(n: i64) -> i64 {
    if n == 2 { panic("job 2 failed"); }
    return n;
}

fn main() -> i64 {
    val a = task_go("job", 1);
    val b = task_go("job", 2);
    task_run();
    println("results: " + int_to_str(task_result(a) + task_result(b)));
    return 0;
}
```

```
Panic: job 2 failed at line 4
```

and the program exits with status 1 before `results` prints. A worker
whose failure the program should survive returns a status the caller
checks — a negative number, an index into a table of messages — and the
caller decides.

## Common mistakes

- **A typo in a `task_go` name.** The name is a string, resolved at run
  time: `task_go("wrker", 21)` compiles. A C build panics with
  `task: no such function`; `fid run` prints
  `Executor: unknown function 'wrker'` and carries on with a result of 0.
  Keep task entry points few and named plainly.
- **A file called `channel.fd` or `task.fd` in your folder.** `use
  channel;` imports it instead of the standard module, and the
  `channel_*` functions come out undefined. Pick names the standard
  library doesn't use.
- **Passing more than one value to a task.** `task_go` passes one `i64`.
  Use `async fn` for real parameters, or put the inputs in a global table
  (under a lock) and pass an index.
- **Spawning with a reference.** `spawn f(&x)` is rejected. Pass the value.
- **Writing a global from a thread.** Take a mutex, use the atomics, or
  return the value through `await`.
- **Timing or ordering results under `fid run`.** The interpreter runs
  each task to completion when it starts. Measure and test interleaving
  in a build.
- **Iterating a borrowed Vec with `for x in v`.** Inside
  `fn f(v: &Vec<i64>)`, `for x in v` gives `x` the type `&Vec<i64>`
  today, and `s + x` fails with "arithmetic requires numeric types".
  Write `for x in *v`, or loop over `0..v.len()` with `v.get(i)`.
