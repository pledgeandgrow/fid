# 3. A language tour

The whole language in one chapter — enough to read real code.
`docs/reference/reference.md` has the complete grammar.

## Functions & control flow

```fides
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
```

`if`/`match` can be values:

```fides
val sign = if n < 0 { -1 } else { 1 };
```

Loops: `while cond { }`, `for x in coll { }` where supported, and the
counted loop `for i in 0..n { }`. `break`/`continue` work as usual.

## Structs & impl

```fides
struct Point { x: i64, y: i64 }

impl Point {
    pub fn new(x: i64, y: i64) -> Point {
        return Point { x: x, y: y };
    }
    pub fn dist2(self: Point) -> i64 {
        return self.x * self.x + self.y * self.y;
    }
}

var p: Point = Point { x: 3, y: 4 };
println(int_to_str(p.dist2()));     // 25
```

Layout control: `@repr(C)` for C ABI, `@packed` for no padding.

## Enums, Option, Result

```fides
enum Shape {
    Circle(r: i64),
    Rect(w: i64, h: i64),
    Point,
}

match s {
    Shape::Circle(r: i64) => { /* r bound */ }
    Shape::Rect(w: i64, h: i64) => { }
    Shape::Point => { }
}
```

`Option`/`Result` are ordinary enums with syntax support; `x ?? default`
unwraps or substitutes.

## Capabilities (`cap`) & trait objects

Interfaces are `cap`s, implemented explicitly:

```fides
cap Animal {
    fn sound(self) -> i64;
    fn legs(self) -> i64;
}

impl Animal for Dog {
    pub fn sound(self: Dog) -> i64 { return 1; }
    pub fn legs(self: Dog) -> i64 { return 4; }
}

fn describe(a: dyn Animal) -> i64 {
    return a.sound() + a.legs();     // dynamic dispatch
}

var da = Dog { age: 3 } as dyn Animal;
```

## Generics

```fides
fn id<T>(x: T) -> T { return x; }
```

Monomorphized per type — no boxing, no runtime cost.

## Closures & function values

```fides
fn make_adder(n: i64) -> fn(i64) -> i64 {
    return fn(x: i64) -> i64 { return x + n; };
}
var add5 = make_adder(5);
println(int_to_str(add5(10)));      // 15
```

Closures capture their environment; escaping captures get the borrow
checker's escape rules (chapter 4).

## Strings & collections

`str` is immutable + length-tagged; `StringBuilder` builds incrementally
(`append_str`/`append_int`/`to_str`). `Vec<T>` and `HashMap<K,V>` are
the workhorses — see `docs/cookbook.md` for recipes.

## Annotations (preview)

```fides
@crashfree fn in_bounds(v: Vec<i64>, i: i64) -> bool { ... }
@no-alloc fn isr() -> void { ... }
@send fn job(...) -> ... { ... }
```

Chapter 5 covers what the compiler checks vs trusts.
