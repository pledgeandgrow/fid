// Phase 3B.1 — Default trait methods.
// A trait method with a default body is inherited by impls that don't override it.

cap Greeter {
    pf greet(self) -> i64 {
        ret 42;
    }

    pf name(self) -> i64;
}

struct Foo {
    val: i64,
}

// Impl provides `name` but NOT `greet` — `greet` uses the default body.
impl Greeter for Foo {
    pub pf name(self: Foo) -> i64 {
        ret self.val;
    }
}

struct Bar {
    x: i64,
}

// Impl provides both — `greet` is overridden.
impl Greeter for Bar {
    pub pf greet(self: Bar) -> i64 {
        ret self.x + 100;
    }

    pub pf name(self: Bar) -> i64 {
        ret self.x * 2;
    }
}

pf main() -> i64 {
    pv f = Foo { val: 10 };
    // greet uses the default (returns 42)
    print(f.greet());
    // name is provided by the impl
    print(f.name());

    pv b = Bar { x: 5 };
    // greet is overridden (returns 105)
    print(b.greet());
    // name is provided by the impl
    print(b.name());

    ret 0;
}
