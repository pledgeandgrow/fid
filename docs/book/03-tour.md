# 3. A language tour

The whole language in one chapter: enough to read real code. Every
program here is complete; run it with `fid run`.
`docs/reference/reference.md` has the complete grammar and rules.

## Functions and control flow

```fides
// control.fd
fn abs(x: i64) -> i64 {
    if x < 0 { return 0 - x; }
    return x;
}

fn classify(n: i64) -> str {
    match n {
        0 => { return "zero"; }
        _ => {
            if n > 0 { return "pos"; }
            return "neg";
        }
    }
}

fn sign(n: i64) -> i64 {
    val s: i64 = if n < 0 { -1 } else { 1 };
    return s;
}

fn main() -> i64 {
    println(int_to_str(abs(-7)) + " " + classify(0) + " " + classify(-3));
    println(int_to_str(sign(-9)) + " " + int_to_str(sign(4)));

    var sum: i64 = 0;
    for i in 0..5 { sum = sum + i; }          // 0..5 is 0, 1, 2, 3, 4
    println("0..5 sums to " + int_to_str(sum));

    var v: Vec<i64> = Vec::new();
    v.push(10);
    v.push(20);
    v.push(30);
    for x in v {
        if x == 20 { continue; }
        println("item " + int_to_str(x));
    }

    var n: i64 = 0;
    while true {
        n = n + 1;
        if n == 3 { break; }
    }
    println("stopped at " + int_to_str(n));

    val size: str = match v.len() {
        0 => "empty",
        1 => "one",
        _ => "many",
    };
    println(size);
    return 0;
}
```

```
$ fid run control.fd
7 zero neg
-1 1
0..5 sums to 10
item 10
item 30
stopped at 3
many
```

- Every parameter and result has a type; a function with no result
  leaves out `-> T`.
- `if` and `match` are values when you use them as one (`val s = if ...`,
  `val size = match ...`). A `match` used as a statement has block arms
  `=> { ... }`; one used as a value may have expression arms.
- `for i in 0..n` counts from 0 to n - 1 (`0..=n` includes `n`);
  `for x in v` walks a `Vec`. `while cond { }`, `loop { }`, `break` and
  `continue` are as in C.

## Structs and impl

```fides
// point.fd
struct Point { x: i64, y: i64 }

impl Point {
    pub fn new(x: i64, y: i64) -> Point {
        return Point { x: x, y: y };
    }
    pub fn dist2(self: Point) -> i64 {
        return self.x * self.x + self.y * self.y;
    }
    pub fn moved(self: Point, dx: i64) -> Point {
        return Point { x: self.x + dx, y: self.y };
    }
}

fn main() -> i64 {
    val p: Point = Point::new(3, 4);
    println(int_to_str(p.dist2()));
    var q: Point = p;          // a struct is copied
    q.x = 10;
    println(int_to_str(p.x) + " " + int_to_str(q.x));
    val r: Point = p.moved(2);
    println(f"({r.x}, {r.y})");
    return 0;
}
```

```
$ fid run point.fd
25
3 10
(5, 4)
```

Methods live in `impl` blocks and name their receiver: `self: Point`, or
`self: &Point` / `self: &mut Point` to borrow it (chapter 4). A method
without `self` is called on the type: `Point::new(3, 4)`. Fields and
methods without `pub` are private to their module. `@repr(C)` gives a
struct the C layout and `@packed` removes padding (chapter 5).

## Enums, Option and Result

```fides
// shapes.fd
enum Shape {
    Circle(i64),
    Rect(i64, i64),
    Dot,
}

fn area(s: Shape) -> i64 {
    match s {
        Shape::Circle(r) => { return 3 * r * r; }
        Shape::Rect(w, h) => { return w * h; }
        Shape::Dot => { return 0; }
    }
}

fn half(n: i64) -> Option<i64> {
    if n % 2 == 0 { return Option::Some(n / 2); }
    return Option::None;
}

fn parse_port(s: str) -> Result<i64, str> {
    val n: i64 = str_parse_int(s);
    if n <= 0 || n > 65535 { return Result::Err("bad port: " + s); }
    return Result::Ok(n);
}

fn main() -> i64 {
    var shapes: Vec<Shape> = Vec::new();
    shapes.push(Shape::Circle(2));
    shapes.push(Shape::Rect(3, 4));
    shapes.push(Shape::Dot);
    var total: i64 = 0;
    var i: i64 = 0;
    while i < shapes.len() {
        total = total + area(shapes.get(i));
        i = i + 1;
    }
    println("total area " + int_to_str(total));

    val h: i64 = half(10) ?? -1;
    val g: i64 = half(7) ?? -1;
    println(int_to_str(h) + " " + int_to_str(g));

    match parse_port("99999") {
        Result::Ok(p) => { println("port " + int_to_str(p)); }
        Result::Err(e) => { println(e); }
    }
    return 0;
}
```

```
$ fid run shapes.fd
total area 24
5 -1
bad port: 99999
```

A `match` on an enum must cover every variant (or end with `_`). `Option`
and `Result` are ordinary generic enums: `Option::Some(x)` /
`Option::None`, `Result::Ok(x)` / `Result::Err(e)`. `o ?? d` is the value
inside an `Option`, or `d`. Chapter 6 is about using them well (`?`,
`context`, error enums).

## Caps and trait objects

An interface is a `cap`, implemented explicitly with `impl Cap for Type`:

```fides
// animals.fd
cap Animal {
    fn name(self) -> str;
    fn legs(self) -> i64;
}

struct Dog { age: i64 }
struct Bird { wingspan: i64 }

impl Animal for Dog {
    pub fn name(self: Dog) -> str { return "dog"; }
    pub fn legs(self: Dog) -> i64 { return 4; }
}

impl Animal for Bird {
    pub fn name(self: Bird) -> str { return "bird"; }
    pub fn legs(self: Bird) -> i64 { return 2; }
}

fn describe(a: dyn Animal) -> str {
    return a.name() + " with " + int_to_str(a.legs()) + " legs";
}

fn legs_of<T: Animal>(a: T) -> i64 {
    return a.legs();
}

fn main() -> i64 {
    val d = Dog { age: 3 } as dyn Animal;
    val b = Bird { wingspan: 20 } as dyn Animal;
    println(describe(d));
    println(describe(b));
    println(int_to_str(legs_of(Dog { age: 1 })));
    return 0;
}
```

```
$ fid run animals.fd
dog with 4 legs
bird with 2 legs
4
```

`dyn Animal` is chosen at run time (`x as dyn Animal` makes one);
`T: Animal` is decided at compile time, one copy of `legs_of` per type.
A cap may give methods a default body and require other caps
(`cap Pet: Animal { ... }`).

## Generics

```fides
// generic.fd
fn id<T>(x: T) -> T { return x; }

struct Pair<T> { a: T, b: T }

fn swap<T>(p: Pair<T>) -> Pair<T> {
    return Pair { a: p.b, b: p.a };
}

fn largest(v: Vec<i64>) -> i64 {
    var best: i64 = v.get(0);
    for x in v { if x > best { best = x; } }
    return best;
}

fn main() -> i64 {
    println(int_to_str(id(42)));
    println(id("text"));
    val p: Pair<i64> = swap(Pair { a: 1, b: 2 });
    println(int_to_str(p.a) + " " + int_to_str(p.b));
    var v: Vec<i64> = Vec::new();
    v.push(4);
    v.push(9);
    v.push(2);
    println(int_to_str(largest(v)));
    return 0;
}
```

```
$ fid run generic.fd
42
text
2 1
9
```

Generic functions and structs are compiled once per type they are used
with: no boxing, no run-time cost. Type arguments are inferred; write
them when nothing gives them (`zero::<i64>()`). There are no `where`
clauses.

## Closures and function values

```fides
// closures.fd
fn make_adder(n: i64) -> fn(i64) -> i64 {
    return fn(x: i64) -> i64 { return x + n; };
}

fn apply_twice(f: fn(i64) -> i64, x: i64) -> i64 {
    return f(f(x));
}

fn double(x: i64) -> i64 { return x * 2; }

fn main() -> i64 {
    val add5 = make_adder(5);
    println(int_to_str(add5(10)));
    println(int_to_str(apply_twice(add5, 0)));
    println(int_to_str(apply_twice(double, 3)));
    return 0;
}
```

```
$ fid run closures.fd
15
10
12
```

A closure captures the variables it uses by value; a named function can be
passed wherever a `fn(...)` type is expected. A closure that holds a
reference follows the reference rules of chapter 4.

## Strings and collections

```fides
// words.fd
fn main() -> i64 {
    val text: str = "the cat saw the dog and the bird";
    val words: Vec<str> = str_split(text, " ");
    var counts: HashMap<str, i64> = HashMap::new();
    for w in words {
        val seen: i64 = counts.get_or(w, 0);
        counts.insert(w, seen + 1);
    }
    println(int_to_str(words.len()) + " words, " + int_to_str(counts.len()) + " different");
    val the: i64 = counts.get("the");
    println(f"'the' appears {the} times");

    var sb: StringBuilder = StringBuilder::new();
    sb.append_str("lengths:");
    for w in words {
        sb.append_str(" ");
        sb.append_int(str_len(w));
    }
    println(sb.to_str());
    println(str_to_upper("done"));
    return 0;
}
```

```
$ fid run words.fd
8 words, 6 different
'the' appears 3 times
lengths: 3 3 3 3 3 3 3 4
DONE
```

`str` is immutable and knows its length; `+` makes a new one, and
`StringBuilder` builds one piece by piece without copying each time.
`Vec<T>` and `HashMap<K, V>` are the workhorses: `push`, `get`, `set`,
`len`, `pop`, `sort`, `contains` on a `Vec`; `insert`, `get`, `get_or`,
`contains`, `remove`, `keys` on a map (`m.keys()` gives the keys sorted,
not in insertion order).
`docs/guides/cookbook.md` has recipes; reference.md §13 lists everything.

## Modules

A file can be a module other files `use`:

```fides
// geometry.fd
module geometry;

pub fn square(x: i64) -> i64 { return x * x; }

fn helper() -> i64 { return 1; }    // not pub: only geometry can call it
```

```fides
// usegeo.fd
use geometry;

fn main() -> i64 {
    println(int_to_str(square(12)));
    println(int_to_str(geometry::square(3)));
    return 0;
}
```

```
$ fid run usegeo.fd
144
9
```

`use geometry;` looks for `geometry.fd` next to the file, then in the
standard library. `geometry::square` names the module when two modules
define the same function.

## Annotations (preview)

```fides
@crashfree fn clamp(x: i64, lo: i64, hi: i64) -> i64 { ... }   // proved not to panic or loop
@no-alloc fn on_tick(n: i64) -> i64 { ... }                   // proved not to allocate
@wrapping fn hash(x: u64) -> u64 { ... }                      // + - * wrap instead of panicking
@send struct Job { id: i64, name: str }                       // may cross to another thread
```

Chapter 5 says which annotations the compiler proves on every build and
which it only records; chapter 8 covers `@send`.

## Common mistakes

- **A bare `None` in a pattern.** `match o { None => ... }` binds a new
  variable named `None` that matches everything. Write `Option::None`
  (and `Shape::Dot`, not `Dot`).
- **`for s in v` over a `Vec` of enums under `fid run`.** Today the
  interpreter gives each element a zero payload there (`shapes.fd` would
  print `total area 0`); C and native builds are right. Walk such a `Vec`
  by index with `v.get(i)`, as `shapes.fd` does, until it is fixed.
- **`m.get(k)` for a key that may be missing.** On a `HashMap<str, i64>`
  it gives 0, not an error and not an `Option`. Use `m.get_or(k, d)`, or
  test `m.contains(k)` first.
- **`str_parse_int` on text that is not all digits.** It reads the
  leading digits and stops: `str_parse_int("80x")` is 80 and
  `str_parse_int("abc")` is 0. Check the text first when it comes from a
  user.
- **`&self` in a method.** It does not parse; name the type:
  `self: &Point`.
- **`for (i, x) in v`.** There is no tuple pattern in `for`; count with
  `for i in 0..v.len()` and read `v.get(i)`.
