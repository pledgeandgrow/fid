// Phase 3B.3 — Trait objects (dynamic dispatch).
// A dyn Trait value can hold any type that implements the trait,
// and method calls are dispatched at runtime.

cap Animal {
    pf sound(self) -> i64;
    pf legs(self) -> i64;
}

struct Dog {
    age: i64,
}

impl Animal for Dog {
    pub pf sound(self: Dog) -> i64 {
        ret 1;
    }
    pub pf legs(self: Dog) -> i64 {
        ret 4;
    }
}

struct Spider {
    venom: i64,
}

impl Animal for Spider {
    pub pf sound(self: Spider) -> i64 {
        ret 2;
    }
    pub pf legs(self: Spider) -> i64 {
        ret 8;
    }
}

// A function that takes a trait object and calls methods on it.
pf describe(a: dyn Animal) -> i64 {
    pv s = a.sound();
    pv l = a.legs();
    print(s);
    print(l);
    ret s + l;
}

pf main() -> i64 {
    pv d = Dog { age: 3 };
    pv s = Spider { venom: 10 };

    // Cast concrete types to trait objects
    pv da = d as dyn Animal;
    pv sa = s as dyn Animal;

    // Call the same function with different types
    pv r1 = describe(da);
    pv r2 = describe(sa);

    print(r1);
    print(r2);

    ret 0;
}
