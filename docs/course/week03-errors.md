# Week 3 — Failure: panics and errors as values

**Reading:** book chapter 6 (`docs/book/06-errors.md`); reference §4.5
(`?`, `??`), §4.9 (`try`), §5.5 (`defer`) and §8.2 (the panic rule).

**After this week you can:** decide, for each way a function can fail,
whether it is a bug (panic) or an outcome (a `Result` the caller
handles); propagate errors with `?`; design an error enum; explain why a
panic is never caught and what that buys.

## 1. Two kinds of failure

A function is **total** when it returns a value for every input, and
**partial** when some inputs have no answer. There are two honest ways to
deal with a partial function:

1. Make the failure part of the type: return `Option<T>` (no answer) or
   `Result<T, E>` (no answer, and here is why). The caller cannot reach
   the value without saying what happens otherwise.
2. Declare the input a bug: the function panics, and the program stops.

Fid has no exceptions. A failure is either a value the caller matches or
the end of the program. The **panic rule** (reference §8.2) is the same on
every back end:

- a panic prints `Panic: <message>` and a stack trace on stderr, flushes
  what the program already printed, and exits with status 1;
- it is never caught, not even by another task or thread;
- nothing unwinds: no `defer`, no `drop` runs after it. The operating
  system takes back memory, files and sockets.

That last point is a deliberate trade. Without unwinding there is no
half-run cleanup code to reason about and no exception-safety discipline;
in exchange, cleanup that must happen even on a bug (a lock file, a
database transaction) cannot rely on the process. Week 9 shows how to
prove that a function cannot panic at all.

```fides
// lookup.fd
fn find(v: Vec<str>, name: str) -> Option<i64> {
    var i: i64 = 0;
    while i < v.len() {
        if v.get(i) == name { return Option::Some(i); }
        i = i + 1;
    }
    return Option::None;
}

fn lookup(v: Vec<str>, name: str) -> i64 {
    println("looking up " + name);
    defer { println("done with " + name); }
    return find(v, name).unwrap();
}

fn main() -> i64 {
    var users: Vec<str> = Vec::new();
    users.push("ada");
    users.push("grace");
    println(int_to_str(lookup(users, "grace")));
    println(int_to_str(find(users, "linus") ?? -1));
    println(int_to_str(lookup(users, "linus")));
    println("never printed");
    return 0;
}
```

```
$ fid run lookup.fd
looking up grace
done with grace
1
-1
looking up linus
Panic: called unwrap on None...
...
$ echo $?
1
```

`defer` ran when `lookup("grace")` returned, and did not run when
`lookup("linus")` panicked. `o ?? d` is the safe form: the value, or a
default.

## 2. Errors as values

When the caller can do something about a failure, return it. An error
**enum** lets the caller tell the cases apart, and the exhaustiveness
check of week 2 makes sure every case has a message. `e?` returns the
`Err` (or `None`) from the enclosing function at once and is the value
otherwise:

```fides
// rpn.fd
enum CalcError {
    Empty,
    Underflow(op: str),
    DivZero,
    BadToken(tok: str),
    Leftover(count: i64),
}

fn message(e: CalcError) -> str {
    match e {
        CalcError::Empty => { return "empty input"; }
        CalcError::Underflow(op) => { return "'" + op + "' needs two operands"; }
        CalcError::DivZero => { return "division by zero"; }
        CalcError::BadToken(t) => { return "not a number or operator: '" + t + "'"; }
        CalcError::Leftover(n) => { return int_to_str(n) + " values left on the stack"; }
    }
}

fn pop(stack: &mut Vec<i64>, op: str) -> Result<i64, CalcError> {
    if stack.len() == 0 { return Result::Err(CalcError::Underflow(op)); }
    return Result::Ok(stack.pop());
}

fn is_number(t: str) -> bool {
    if str_len(t) == 0 { return false; }
    var i: i64 = 0;
    while i < str_len(t) {
        val b: i64 = str_byte_at(t, i);
        if b < '0' || b > '9' { return false; }
        i = i + 1;
    }
    return true;
}

fn apply(stack: &mut Vec<i64>, op: str) -> Result<i64, CalcError> {
    val b: i64 = pop(stack, op)?;
    val a: i64 = pop(stack, op)?;
    if op == "+" { return Result::Ok(a + b); }
    if op == "-" { return Result::Ok(a - b); }
    if op == "*" { return Result::Ok(a * b); }
    if b == 0 { return Result::Err(CalcError::DivZero); }
    return Result::Ok(a / b);
}

fn eval(text: str) -> Result<i64, CalcError> {
    var stack: Vec<i64> = Vec::new();
    for t in str_split(text, " ") {
        if t == "+" || t == "-" || t == "*" || t == "/" {
            val r: i64 = apply(&mut stack, t)?;
            stack.push(r);
        } else if is_number(t) {
            stack.push(str_parse_int(t));
        } else {
            return Result::Err(CalcError::BadToken(t));
        }
    }
    if stack.len() == 0 { return Result::Err(CalcError::Empty); }
    if stack.len() > 1 { return Result::Err(CalcError::Leftover(stack.len())); }
    return Result::Ok(stack.get(0));
}

fn run(text: str) {
    match eval(text) {
        Result::Ok(v) => { println(text + "  =>  " + int_to_str(v)); }
        Result::Err(e) => { println(text + "  =>  error: " + message(e)); }
    }
}

fn main() -> i64 {
    run("3 4 + 2 *");
    run("1 2 3");
    run("5 0 /");
    run("2 +");
    run("7 x *");
    return 0;
}
```

```
$ fid run rpn.fd
3 4 + 2 *  =>  14
1 2 3  =>  error: 3 values left on the stack
5 0 /  =>  error: division by zero
2 +  =>  error: '+' needs two operands
7 x *  =>  error: not a number or operator: 'x'
```

Read `apply` closely: the two `?` make an underflow leave `apply`, then
`eval`, with no code written for it. `&mut Vec<i64>` lets `pop` change the
caller's stack (week 6 explains the rules that make that safe). The
remaining panics are overflow of `a * b` and so on; exercise 2 removes
them.

## 3. Choosing

| Situation | Use |
|---|---|
| Input from a user, a file, the network | `Result` with an error enum |
| "Not found" is a normal answer | `Option` |
| A caller broke a documented precondition | panic (`assert`, `unwrap`, `panic`) |
| Resource cleanup on normal and error paths | `defer` |
| Code that must keep running whatever the input | `Result` everywhere, then `@crashfree` (week 9) |

## Exercises

The lab for this week is [Lab 3 — A calculator that reports overflow](labs/lab03-rpn/README.md).

1. Make `rpn.fd` total: replace each arithmetic operator by a function
   that returns `Err(CalcError::Overflow)` instead of panicking (compare
   with `i64::MAX` before multiplying, or compute in `i128`). Check with
   inputs near `9223372036854775807`.
2. Give `eval` a second error type for the tokenizer and convert between
   the two at the boundary. Why does Fid not convert error types for you?
3. Write a function that opens a resource, uses `?` three times and must
   release the resource on each path. Write it with `defer`, then without.
   Count the release calls in each version.
4. A web server handles one request per task. Under the panic rule, what
   happens to the other requests when one handler indexes out of bounds?
   Propose two designs that keep the server up, and say what each
   requires of the handler code.

## Discussion

Exceptions, error codes, `Result` types and "let it crash" (Erlang) are
four answers to the same question. Fid chose values plus an uncatchable
panic. Where does it sit relative to Rust (catchable unwinding panics)
and Go (multiple return values plus `recover`)?
