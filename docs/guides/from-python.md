# Coming from Python

Fid will feel strict at first. Every variable has one type, decided when
the program is compiled; there is no `None` that can turn up anywhere; a
list holds one kind of thing; and a program is compiled before it runs.
What you get for it is a single native executable that starts instantly,
runs at C speed, has no interpreter or packages to install next to it,
and fails at compile time for most of the mistakes a Python program finds
in production.

Naming: **Fid** is the language, **Fides** the compiler, `fid` the
command.

## Syntax map

| Python | Fid |
|---|---|
| `x = 1` | `var x = 1;` (can change) or `val x = 1;` (can't) |
| `x: int = 1` | `var x: i64 = 1;` |
| `def f(a, b=10):` | `fn f(a: i64, b: i64 = 10) -> i64 { … }` |
| `f(b=2, a=1)` | `f(b: 2, a: 1)` |
| `return a, b` | `return (a, b);` with `-> (i64, i64)`; `val (q, r) = f();` |
| `lambda x: x * x` | `fn(x: i64) => x * x` |
| indentation | `{ }` braces; `;` optional |
| `if … elif … else` | `if c { } else if d { } else { }` |
| `x if c else y` | `if c { x } else { y }` |
| `and`, `or`, `not` | `&&`, `\|\|`, `!` |
| `True`, `False`, `None` | `true`, `false`, `Option::None` |
| `for i in range(n)` | `for i in 0..n { }` |
| `for i in range(1, n + 1)` | `for i in 1..=n { }` |
| `for x in items` | `for x in items { }` |
| `for i, x in enumerate(items)` | `for i in 0..items.len() { val x = items.get(i); }` |
| `while c:` | `while c { }`; `loop { }` for `while True` |
| `match x: case 1:` | `match x { 1 => { } _ => { } }` |
| `[]`, `l.append(x)`, `len(l)` | `Vec::new()`, `v.push(x)`, `v.len()` |
| `x in l` | `v.contains(x)` |
| `l[i]` | `v.get(i)` or `v[i]` (bounds-checked: a panic, not an exception) |
| `[x * x for x in l]` | `v.map(fn(x: i64) => x * x)` |
| `[x for x in l if x > 2]` | `v.filter(fn(x: i64) => x > 2)` |
| `{}`, `d[k] = v` | `HashMap::new()`, `m.insert(k, v)` |
| `d.get(k, 0)` | `m.get_or(k, 0)` |
| `k in d` | `m.contains(k)` |
| `len(s)` | `str_len(s)` (bytes, not characters) |
| `s.upper()`, `s.split(",")`, `s[1:4]` | `str_to_upper(s)`, `str_split(s, ",")`, `str_slice(s, 1, 4)` |
| `", ".join(parts)` | `str_join(parts, ", ")` (`use string;`) |
| `str(n)`, `int(s)` | `int_to_str(n)`, `str_parse_int(s)` (0 if `s` is not a number) |
| `f"{name} is {age}"` | `f"{name} is {age}"` — same, but no `:4` or `:.2f` specs |
| `print(a, b)` | `println(a + " " + b)`, or `println(f"{a} {b}")` |
| `class P:` + `def m(self)` | `struct P { … }` + `impl P { pub fn m(self: P) … }` |
| `raise ValueError(…)` | `return Result::Err(…)`, or `panic(…)` for a bug |
| `try: … except:` | `match r { Result::Ok(v) => { } Result::Err(e) => { } }` |
| `with open(p) as f:` | `read_file(p)` returns the whole text; `defer { }` for cleanup |
| `import json` | `use json;` |
| `if __name__ == "__main__":` | `fn main() -> i64 { … return 0; }` — the result is the exit status |
| `sys.argv[1]` | `arg_at(1)` (an `Option<str>`) |
| `os.environ["HOME"]` | `env_var("HOME")` (an `Option<str>`) |
| `python app.py` | `fid run app.fd` (interpreter) or `fid emit app.fd` (native executable) |

```fides
// tour.fd
use string;

struct Point { x: i64, y: i64 }

impl Point {
    pub fn norm1(self: Point) -> i64 { return abs(self.x) + abs(self.y); }
}

fn div_mod(a: i64, b: i64) -> (i64, i64) {
    return (a / b, a % b);
}

fn greet(name: str, greeting: str = "hello") -> str {
    return greeting + ", " + name;
}

fn main() -> i64 {
    var nums: Vec<i64> = Vec::new();
    for i in 1..=6 { nums.push(i); }
    val squares = nums.map(fn(x: i64) => x * x);
    val evens = nums.filter(fn(x: i64) => x % 2 == 0);
    println(f"squares {squares}");
    println(f"evens {evens}");
    println(f"has 4? {nums.contains(4)} len {nums.len()}");

    var ages: HashMap<str, i64> = HashMap::new();
    ages.insert("ada", 36);
    ages.insert("alan", 41);
    println(f"ada {ages.get("ada")} bob {ages.get_or("bob", -1)} has bob? {ages.contains("bob")}");

    val (q, r) = div_mod(-7, 2);
    println(f"-7 / 2 = {q}, -7 % 2 = {r}");
    println(greet("ada"));
    println(greet("alan", greeting: "hi"));
    val p = Point { x: 3, y: -4 };
    println(f"norm1 {p.norm1()}");
    println(float_to_str(10.0 / 4.0));
    val words: Vec<str> = str_split("a,b,c", ",");
    println(str_join(words, " | "));
    println(str_to_upper("shout") + " " + str_slice("python", 1, 4));
    return 0;
}
```

```
$ fid run tour.fd
squares [1, 4, 9, 16, 25, 36]
evens [2, 4, 6]
has 4? true len 6
ada 36 bob -1 has bob? false
-7 / 2 = -3, -7 % 2 = -1
hello, ada
hi, alan
norm1 7
2.5
a | b | c
SHOUT yth
```

Look at the `-7 / 2` line: Python says `-4` and `1`. Integer division in
Fid truncates toward zero, like C, and `%` takes the sign of the left
operand.

## Types, for someone used to Python

**Every value has one type, known before the program runs.** You write
types on function parameters and results; inside a function the compiler
infers most of them (`var i = 0;` is an `i64`). A wrong type is a compile
error, not a `TypeError` an hour into a run.

**Integers have a size.** `i64` is the default (also `i8` … `i128`,
`u8` … `u128`). There is no big integer: going past `i64`'s range
*panics* (`Panic: Integer overflow on addition: …`), it doesn't silently
grow or wrap. Functions meant to wrap — hashes, checksums — are marked
`@wrapping`. Floats are `f64` (or `f32`); an integer goes into a float
freely, a float into an integer only with `as`: `x as i64`.

**There is no `None` that can be anything.** A function that might have
no answer says so in its type, `Option<str>`, and the caller has to deal
with both cases before it can use the value:

```fides
fn find_user(id: i64) -> Option<str> {
    if id == 1 { return Option::Some("ada"); }
    return Option::None;
}

fn main() -> i64 {
    val name: str = find_user(7) ?? "nobody";       // `or` with a default
    println(name);
    match find_user(1) {
        Option::Some(n) => { println("found " + n); }
        Option::None => { println("no such user"); }
    }
    return 0;
}
```

(Give the result of `??` a type, `val name: str = …`; the book's chapter
on errors explains why.)

**Collections hold one type.** `Vec<str>` is a list of strings,
`HashMap<str, i64>` a dict from strings to integers. A list that mixes
kinds in Python becomes an `enum` in Fid, one variant per kind:

```fides
enum Shape {
    Circle(r: i64),
    Rect(w: i64, h: i64),
}

fn area(s: Shape) -> i64 {
    match s {
        Shape::Circle(r) => { return 3 * r * r; }
        Shape::Rect(w, h) => { return w * h; }
    }
}
```

and a `match` that forgets a variant doesn't compile.

**Strings are not objects.** `str` has no methods; the string functions
are free functions (`str_len`, `str_find`, `str_split`, `str_trim`,
`str_replace`, `str_to_lower`, …, in core and `use string;`). Strings are
immutable, as in Python, and indexed by **byte**: `str_byte_at(s, i)` is
a number, and `'a'` is the number 97.

**Classes are structs plus functions.** `struct` holds the data, `impl`
holds the methods, and `self` is written with its type. There is no
inheritance and no `__init__`: a constructor is an ordinary function
returning the struct. Duck typing becomes an explicit interface, a
`cap`:

```fides
cap Speaker { fn speak(self) -> str; }

struct Dog { name: str }

impl Speaker for Dog {
    pub fn speak(self: Dog) -> str { return self.name + " says woof"; }
}

fn main() -> i64 {
    val d = Dog { name: "rex" } as dyn Speaker;
    println(d.speak());
    return 0;
}
```

## Ownership, for someone used to Python

There is no garbage collector. Each value has one owner — the variable
that holds it — and is freed when that variable goes out of scope. You
never call `free`, and you rarely think about it. What you *do* notice
is the other side of the same rule:

**Assignment copies.** In Python `b = a` gives a second name for the same
list. In Fid it gives a second list:

```fides
// copy.fd
fn add_item(v: &mut Vec<i64>) -> void {
    v.push(99);
}

fn main() -> i64 {
    var a: Vec<i64> = Vec::new();
    a.push(1);
    var b = a;          // a copy, not a second name for the same list
    b.push(2);
    println(f"a {a}  b {b}");
    add_item(&mut a);
    println(f"a {a}");
    return 0;
}
```

```
$ fid run copy.fd
a [1]  b [1, 2]
a [1, 99]
```

(The copy is cheap: the data is shared until one side writes.)

**A function changes your list only if you let it.** Python functions can
mutate any list they are given. A Fid function that takes `v: Vec<i64>`
gets a value it may not change at all —

```
error: Type error (line 2): 'v' is a by-value parameter of 'add_item' and cannot be changed: the caller would not see the change.
    Fix: take `v: &mut Vec<i64>` and pass `&mut ...`, or change a copy: `var v = v;`.
```

— so a function that is meant to change it says `&mut Vec<i64>`, and the
caller writes `&mut a`. Read-only access is `&Vec<i64>` and `&a`.

**References can't be kept.** A `&` or `&mut` lives for one call or one
local variable. You can't store one in a struct, put one in a list or
return one to a caller that outlives it. Where Python code keeps a
reference to an object inside another object, Fid code keeps the object
itself, or an index or key to look it up. Chapter 4 of the book
(`docs/book/04-ownership.md`) is the full story.

## A real port

A Python script that prints the three most frequent words on standard
input:

```python
# topwords.py
import re
import sys

def words(line):
    return re.findall(r"[a-z0-9']+", line.lower())

counts = {}
for line in sys.stdin:
    for w in words(line):
        counts[w] = counts.get(w, 0) + 1

ranked = sorted(counts.items(), key=lambda kv: (-kv[1], kv[0]))
for word, count in ranked[:3]:
    print(f"{count:4} {word}")
```

And in Fid:

```fides
// topwords.fd: the most frequent words on standard input
use string;

struct WordCount {
    word: str,
    count: i64,
}

fn is_word_byte(b: i64) -> bool {
    return (b >= 'a' && b <= 'z') || (b >= '0' && b <= '9') || b == '\'';
}

// The lowercased words of a line, punctuation dropped.
fn words(line: str) -> Vec<str> {
    val lower: str = str_to_lower(line);
    var out: Vec<str> = Vec::new();
    var start: i64 = -1;
    for i in 0..str_len(lower) + 1 {
        var inside: bool = false;
        if i < str_len(lower) {
            inside = is_word_byte(str_byte_at(lower, i));
        }
        if inside && start < 0 {
            start = i;
        } else if !inside && start >= 0 {
            out.push(str_slice(lower, start, i));
            start = -1;
        }
    }
    return out;
}

// True when a ranks above b: more uses first, then alphabetical.
fn ranks_above(a: WordCount, b: WordCount) -> bool {
    if a.count != b.count { return a.count > b.count; }
    return str_compare(a.word, b.word) < 0;
}

fn ranked(counts: &HashMap<str, i64>) -> Vec<WordCount> {
    var out: Vec<WordCount> = Vec::new();
    for w in counts.keys() {
        val item = WordCount { word: w, count: counts.get(w) };
        out.push(item);
        // insertion sort: slide the new item left past everything it outranks
        var j: i64 = out.len() - 1;
        while j > 0 && ranks_above(item, out.get(j - 1)) {
            out.set(j, out.get(j - 1));
            j = j - 1;
        }
        out.set(j, item);
    }
    return out;
}

fn main() -> i64 {
    var counts: HashMap<str, i64> = HashMap::new();
    loop {
        match read_line() {
            Option::Some(line) => {
                for w in words(line) {
                    counts.insert(w, counts.get_or(w, 0) + 1);
                }
            }
            Option::None => { break; }
        }
    }
    var top: Vec<WordCount> = ranked(&counts);
    top.truncate(3);
    for wc in top {
        println(str_pad_left(int_to_str(wc.count), 4, ' ') + " " + wc.word);
    }
    return 0;
}
```

```
$ printf 'The cat sat.\nThe dog sat, and the cat ran!\n' > story.txt
$ CC="zig cc" fid ccompile topwords.fd -o topwords.exe
$ ./topwords.exe < story.txt
   3 the
   2 cat
   2 sat
```

What changed, and why:

- **`re.findall` became a loop.** There is a `regex` module in the
  standard library, but a byte loop is short and has no surprises. Note
  `'a'` is a number: character literals are integers.
- **`sorted(…, key=…)` became an insertion sort.** `v.sort()` sorts
  numbers; it does not sort strings or structs, and there is no `key=`.
  The comparison is a plain function, `ranks_above`, which is also where
  the tie-break lives.
- **The `(word, count)` tuples became a struct.** Tuples exist
  (`(a, b)`, `val (a, b) = t`), but a named struct is clearer once a value
  travels through several functions.
- **`ranked[:3]` became `truncate(3)`.** There are no slices.
- **`ranked` borrows the map** (`&HashMap<str, i64>`) rather than copying
  it; `main` still owns it.
- **`f"{count:4}"` became `str_pad_left`.** f-strings have no format
  specs.
- **Nothing is freed by hand**, and `fid run topwords.fd --leaks` (which
  builds through C) reports `every block was freed`.

`fid run` reads standard input too (since Nov 19: compiler/interp.fd
`interp_read_input`). One thing the port may run into: a Windows build
from `fid emit` never sees the end of
standard input (`read_line` keeps returning lines), so it loops; build
through `ccompile` on Windows, or read the text from a file with
`read_file(path)` instead.

## What doesn't carry over

- **No REPL workflow.** You edit a file and run `fid run file.fd`. Under
  `fid run` the interpreter can't call C functions (a few libc calls
  aside), and it is much slower than a built program; for anything
  timed or interactive, `fid emit` or `fid ccompile` and run the
  executable. It also has a few bugs builds don't: today `for s in shapes`
  over a `Vec` of enum values gives wrong payloads under `fid run`
  (`shapes.get(i)` is right). When `fid run` and a build disagree, that is
  a compiler bug worth reporting.
- **No exceptions.** Failures you expect are values (`Option`, `Result`)
  that the caller handles; bugs are panics, which end the program and
  cannot be caught. There is no `try`/`except` around a panic.
- **No big integers, and different division.** Overflow panics;
  `-7 / 2` is `-3`.
- **No `sorted(key=…)` for strings or records.** `v.sort()` is correct
  for numbers only; sort anything else with your own comparison (as in
  the port). Don't rely on the order of `m.keys()`.
- **`map` keeps the element type.** `v.map(f)` works when `f` returns
  the same type it takes; `names.map(fn(s: str) => str_len(s))` compiles
  but fails at run time. Turning a `Vec<str>` into a `Vec<i64>` is a
  `for` loop with `push`.
- **Missing dict keys are not errors.** `m.get(k)` on a key that isn't
  there gives a zero value — `0` for numbers, an empty string for `str`
  values in a build — not a `KeyError`. Use `m.contains(k)` or
  `m.get_or(k, default)` when it matters.
- **No `enumerate`, `zip`, slices, comprehensions, generators or
  `yield`.** Index loops and `map`/`filter` cover most of it; `yield`
  parses and is not implemented.
- **No `*args`/`**kwargs`, no decorators, no metaclasses, no
  monkey-patching.** Default and named arguments do exist. The `@` things
  you will see (`@crashfree`, `@wrapping`) are compiler-checked
  annotations, not decorators.
- **`print` takes one value.** `println(x)` prints one string; build it
  with `+` or an f-string.
- **`str_parse_int` is not `int()`.** It returns 0 for text that isn't a
  number instead of raising. Validate first if 0 is a valid input.
- **Command-line arguments under `fid run`.** The interpreter's own
  arguments come first: in `fid run app.fd a`, `a` is `arg_at(3)`. In a
  built program it is `arg_at(1)`, as `sys.argv[1]` would be.
- **No pip.** `fid ark` manages dependencies (`ark.toml`), but it is
  young and has few published pacts (the index is github.com/pledgeandgrow/pacts); most of what you'd install is in `stdlib/`
  (JSON, TOML, CSV, HTTP, regex, dates, SQLite through C).

## What you get that Python doesn't

- **One file to ship.** `fid emit app.fd --target x86_64-linux -o app`
  writes a static Linux executable from any host — no Python, virtualenv
  or system libraries needed on the server (book chapter 10).
- **Errors before running.** Types, missing `match` cases, `None`
  handling, mutation of something you didn't mean to change, and data
  races on globals are compile errors.
- **Speed.** Built programs run as native code.
- **Real threads.** `spawn f(x)` and `parallel { | a = f() | b = g() }`
  run on several cores at once — no GIL — with the compiler checking what
  may cross between threads (book chapter 8).
- **Effects you can declare.** `fn score(x: i64) -> i64 !{}` promises no
  I/O, and the compiler holds the body (and everything it calls) to it
  (book chapter 5).

Next: the book's chapters 2–4 (`docs/book/`) for the language itself,
then chapter 6 for the error model, which is the biggest change in habit.
