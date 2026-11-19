# Week 2 — Types: structs, enums, generics and caps

**Reading:** book chapter 3 (`docs/book/03-tour.md`); reference §2.2–2.5,
§6 (patterns) and §7 (generics).

**After this week you can:** model data with structs and enums; write a
`match` the compiler accepts as complete; choose between static dispatch
(`T: Cap`) and dynamic dispatch (`dyn Cap`) and explain what each costs.

## 1. Products and sums

A `struct` is a product: it holds every field. An `enum` is a sum: it
holds exactly one of its variants, each with its own payload. Together
they describe most data a program handles, and the compiler can check
that code handles every case, because the set of variants is closed.

Recursive data needs an indirection: an `Expr` cannot contain an `Expr`
directly (its size would be infinite), but it can contain a `Box<Expr>`,
a pointer to one on the heap. `*b` reads the value a box holds.

```fides
// expr.fd
enum Expr {
    Num(i64),
    Add(Box<Expr>, Box<Expr>),
    Mul(Box<Expr>, Box<Expr>),
    Neg(Box<Expr>),
}

fn eval(e: Expr) -> i64 {
    match e {
        Expr::Num(n) => { return n; }
        Expr::Add(a, b) => { return eval(*a) + eval(*b); }
        Expr::Mul(a, b) => { return eval(*a) * eval(*b); }
        Expr::Neg(a) => { return 0 - eval(*a); }
    }
}

fn show(e: Expr) -> str {
    match e {
        Expr::Num(n) => { return int_to_str(n); }
        Expr::Add(a, b) => { return "(" + show(*a) + " + " + show(*b) + ")"; }
        Expr::Mul(a, b) => { return show(*a) + " * " + show(*b); }
        Expr::Neg(a) => { return "-" + show(*a); }
    }
}

fn num(n: i64) -> Box<Expr> { return Box::new(Expr::Num(n)); }

fn main() -> i64 {
    val e: Expr = Expr::Mul(Box::new(Expr::Add(num(2), num(3))), Box::new(Expr::Neg(num(4))));
    println(show(e) + " = " + int_to_str(eval(e)));
    return 0;
}
```

```
$ fid run expr.fd
(2 + 3) * -4 = -20
```

`eval` and `show` are *structural recursion*: one case per variant, each
calling itself on the parts. Adding a variant to `Expr` makes both
functions incomplete, and the compiler says so.

## 2. Exhaustiveness

A `match` on an enum must name every variant or end with `_`. The check
is in the type checker, so a forgotten case never reaches run time:

```fides
// light_fail.fd
enum Light { Red, Amber, Green }

fn next(l: Light) -> Light {
    match l {
        Light::Red => { return Light::Green; }
        Light::Green => { return Light::Amber; }
    }
}

fn main() -> i64 {
    val l: Light = next(Light::Red);
    return 0;
}
```

```
$ fid check light_fail.fd
error[E062]: Type error: non-exhaustive match at line 5...
```

A `_` arm silences the check, and with it the warning you would get when
a variant is added later. Prefer naming the variants.

`Option<T>` and `Result<T, E>` are ordinary enums of the prelude
(reference §13.2): a missing value or a failure is a variant the caller
must match, not a null pointer or an exception. Week 3 builds on this.

## 3. Interfaces: caps

A `cap` lists methods; `impl Cap for Type` provides them. A method with a
body in the cap is a default. A function can take a cap in two ways:

- `fn bigger<T: Shape>(a: T, b: T) -> T` is **static dispatch**. The
  function is compiled once per type it is used with (monomorphization,
  `compiler/mono.fd`); calls are direct and can be inlined; every value
  passed must have the same type `T`.
- `dyn Shape` is **dynamic dispatch**. The value carries a table of its
  methods, the call goes through it, and different types can sit in one
  `Vec<dyn Shape>`.

```fides
// shapes.fd
cap Shape {
    fn area(self) -> i64;
    fn name(self) -> str;
    fn describe(self) -> str {
        return self.name() + " of area " + int_to_str(self.area());
    }
}

struct Square { side: i64 }
struct Rect { w: i64, h: i64 }

impl Shape for Square {
    pub fn area(self: Square) -> i64 { return self.side * self.side; }
    pub fn name(self: Square) -> str { return "square"; }
}

impl Shape for Rect {
    pub fn area(self: Rect) -> i64 { return self.w * self.h; }
    pub fn name(self: Rect) -> str { return "rect"; }
}

fn bigger<T: Shape>(a: T, b: T) -> T {
    if a.area() >= b.area() { return a; }
    return b;
}

fn total(shapes: Vec<dyn Shape>) -> i64 {
    var t: i64 = 0;
    for s in shapes { t = t + s.area(); }
    return t;
}

struct Stack<T> { items: Vec<T> }

fn stack_new<T>() -> Stack<T> { return Stack { items: Vec::new() }; }

fn main() -> i64 {
    val s: Square = bigger(Square { side: 3 }, Square { side: 2 });
    println(s.describe());

    var all: Vec<dyn Shape> = Vec::new();
    all.push(Square { side: 2 } as dyn Shape);
    all.push(Rect { w: 2, h: 5 } as dyn Shape);
    println("total " + int_to_str(total(all)));
    for x in all { println(x.describe()); }

    var st: Stack<str> = stack_new::<str>();
    st.items.push("a");
    st.items.push("b");
    println(int_to_str(st.items.len()) + " on the stack");
    return 0;
}
```

```
$ fid run shapes.fd
square of area 9
total 14
square of area 4
rect of area 10
2 on the stack
```

`stack_new::<str>()` names the type argument (a turbofish): nothing in
the call's arguments says what `T` is. Generic code that the compiler
cannot instantiate is an error, never a run-time surprise (reference §7).

## 4. Where the checks live

| Property | Checked by | Reference |
|---|---|---|
| Every variant handled | type checker | §6 |
| A generic call has one type for each parameter, and the bounds hold | type checker, then `mono.fd` | §7 |
| A `dyn` value's type implements the cap | type checker, at `as dyn` | §2.5 |
| A struct literal names every field | type checker | §2.2 |

## Exercises

The lab for this week is [Lab 2 — An evaluator that cannot panic](labs/lab02-expr/README.md).

1. Add `Div(Box<Expr>, Box<Expr>)` to `expr.fd`. What does the compiler
   report before you extend `eval` and `show`? Make `eval` return
   `Result<i64, str>` so that a division by zero is an `Err`, not a panic.
2. Write `simplify(e: Expr) -> Expr` that folds `x * 1`, `x + 0` and
   `-(-x)`. Argue that it terminates.
3. Give `Shape` a method `perimeter`. Measure (`fid explain`, or by
   reading the C from `fid ccompile`) how `bigger::<Square>` and
   `total` call it. Which one can be inlined?
4. Write `largest<T: Shape>(v: Vec<T>) -> Option<T>`. Why must it return
   an `Option`?

## Discussion

Closed sums make exhaustiveness checkable; open interfaces (`dyn`) make
extension possible without editing old code. This is the *expression
problem*. Which side does a compiler's own AST want, and why does
`compiler/ast.fd` use enums?
