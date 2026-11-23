# Week 4 — Collections, strings, closures and modules

**Reading:** book chapter 3, sections "Closures" to "Modules"
(`docs/book/03-tour.md`); reference §2.9 (modules), §4.10 (closures),
§13.3–13.4 (Vec, HashMap, strings); `docs/guides/cookbook.md`.

**After this week you can:** build programs out of modules; use `Vec`,
`HashMap` and `str` with their cost in mind; pass behaviour as a function
value or closure; explain why changing a collection while walking it is a
compile-time error.

## 1. The three workhorses

| Type | What it is | Cost model |
|---|---|---|
| `Vec<T>` | a growable array: a 40-byte header `{data, len, cap, code, tid}` and a buffer | `get`/`set` O(1) with a bounds check; `push` amortised O(1) |
| `HashMap<K, V>` | open-addressed hash table | `insert`/`get`/`contains` O(1) expected; `keys()` sorted, O(n log n) |
| `str` | immutable bytes with a length word in front | `str_len` O(1); `+` copies both sides; `StringBuilder` for loops |

All three behave as values: `var b = a;` gives `b` its own collection.
They are shared until one side writes, so the copy costs nothing until it
is needed (week 5 explains how, and why that is safe).

## 2. Modules

A file that starts with `module name;` is a module; `pub` marks what other
files may use. `use name;` looks for `name.fd` next to the program, then in
the standard library (`stdlib/`). This week's example is two files:

```fides
// textstats.fd
module textstats;

pub fn words(text: str) -> Vec<str> {
    var out: Vec<str> = Vec::new();
    for w in str_split(str_to_lower(text), " ") {
        if str_len(w) > 0 { out.push(w); }
    }
    return out;
}

pub fn frequencies(ws: Vec<str>) -> HashMap<str, i64> {
    var counts: HashMap<str, i64> = HashMap::new();
    for w in ws {
        counts.insert(w, counts.get_or(w, 0) + 1);
    }
    return counts;
}

pub fn count_if(ws: Vec<str>, keep: fn(str) -> bool) -> i64 {
    var n: i64 = 0;
    for w in ws {
        if keep(w) { n = n + 1; }
    }
    return n;
}
```

```fides
// freq.fd
use textstats;

fn main() -> i64 {
    val text: str = "the quick brown fox jumps over the lazy dog the end";
    val ws: Vec<str> = words(text);
    val counts: HashMap<str, i64> = frequencies(ws);
    println(int_to_str(ws.len()) + " words, " + int_to_str(counts.len()) + " distinct");
    for k in counts.keys() {
        val n: i64 = counts.get(k);
        if n > 1 { println(k + " x" + int_to_str(n)); }
    }
    val min_len: i64 = 4;
    val long_words: i64 = count_if(ws, fn(w: str) -> bool { return str_len(w) >= min_len; });
    println(int_to_str(long_words) + " words of at least " + int_to_str(min_len) + " letters");
    var lens: Vec<i64> = Vec::new();
    for w in ws { lens.push(str_len(w)); }
    lens.sort();
    println("shortest " + int_to_str(lens.get(0)) + ", longest " + int_to_str(lens.get(lens.len() - 1)));
    return 0;
}
```

```
$ fid run freq.fd
11 words, 9 distinct
the x3
5 words of at least 4 letters
shortest 3, longest 5
```

## 3. Functions as values

`count_if` takes a `fn(str) -> bool`. The argument in `freq.fd` is a
closure: an anonymous function that captures `min_len` from its
surroundings. Closures capture **by value**: the closure holds its own
copy of `min_len`. A closure may capture a reference only under the
rules of week 6 (it then may not outlive what it points to).

Function values matter for the rest of the course in two ways:

- **effects** (week 8): what does `count_if` do? It depends on `keep`.
  The compiler gives a higher-order function the effects of the function
  passed at each call;
- **safety** (week 9): a call through a function value is an *indirect*
  call, which `@no-indirect` and `@bounded-stack` forbid, because no
  static analysis can bound what it reaches in general.

## 4. Changing a collection while walking it

In C++ or Java, adding to a vector during iteration over it is a classic
bug (a dangling iterator or a `ConcurrentModificationException`). Fid
rejects it before the program runs:

```fides
// grow_fail.fd
fn main() -> i64 {
    var v: Vec<i64> = Vec::new();
    v.push(1);
    v.push(2);
    for x in v {
        if x == 1 { v.push(3); }
    }
    println(int_to_str(v.len()));
    return 0;
}
```

```
$ fid check grow_fail.fd
error[E080]: borrow error in main (line 7): cannot change v while a `for` loop iterates over it: the loop would read past a resize or skip elements.
    Fix: collect the changes in another Vec and apply them after the loop.
...
```

The error code `E080` is the borrow checker's: the loop holds a borrow of
`v` for its whole body. Week 6 is about those rules in general.

## 5. Cost, again

A program that looks up a name in a `Vec<str>` inside a loop over another
`Vec<str>` is quadratic; a `HashMap` makes it linear. String `+` in a loop
copies everything built so far each time, also quadratic; `StringBuilder`
is linear. `fid lint x.fd --perf` reports these patterns.

## Exercises

The lab for this week is [Lab 4 — Word counts, the top n, and a closure](labs/lab04-words/README.md).

1. Extend `textstats` with `top(counts: HashMap<str, i64>, n: i64) -> Vec<str>`
   giving the `n` most frequent words, ties broken alphabetically. What is
   its complexity?
2. Write `map_words(ws: Vec<str>, f: fn(str) -> str) -> Vec<str>` and use
   it with a closure that capitalises words longer than a captured limit.
3. Fix `grow_fail.fd` as the error message suggests. Then write the same
   loop by index (`while i < v.len()`). The compiler accepts that version:
   does it terminate? Why does the checker treat `for` and `while`
   differently?
4. Time (`fid bench x.fd`, or `CC="zig cc" fid ccompile` and the shell's
   `time`) building a 100,000-word string with `+` and with
   `StringBuilder`. Plot both against input size.

## Discussion

`counts.keys()` returns the keys sorted, not in insertion or hash order.
That costs a sort. What does it buy? (Hint: reference §10.7 and the
`tests/conformance/s13_3_*` tests; think of two runs of the same program
printing the same output.)
