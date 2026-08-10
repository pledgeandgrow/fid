// Phase 3B.2 — Associated types.
// A trait with an associated type, like Rust's Iterator.

cap Container {
    ty Item;
    pf first(self) -> Item;
    pf count(self) -> i64;
}

struct IntBox {
    val: i64,
    count: i64,
}

impl Container for IntBox {
    ty Item = i64;
    pub pf first(self: IntBox) -> i64 {
        ret self.val;
    }
    pub pf count(self: IntBox) -> i64 {
        ret self.count;
    }
}

// A generic function that uses the associated type.
// T::Item is resolved to the concrete type during monomorphization.
pf get_first<T: Container>(c: T) -> T::Item {
    ret c.first();
}

pf main() -> i64 {
    pv box = IntBox { val: 42, count: 3 };
    pv v = get_first(box);
    print(v);

    pv c = box.count();
    print(c);

    ret 0;
}
