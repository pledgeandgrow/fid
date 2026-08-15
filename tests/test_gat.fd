// Test GATs (Generic Associated Types) — Phase 3C
// Tests basic associated types, bare self, and GAT type declarations.

cap Container {
    ty Item;
    pf first(self) -> Item;
    pf count(self) -> i64;
}

// GAT: an associated type with a type parameter
cap Factory {
    ty Product<T>;
    pf make_int(self) -> Product<i64>;
    pf make_str(self) -> Product<str>;
}

struct IntBox {
    val: i64,
}

impl Container for IntBox {
    ty Item = i64;
    pf first(self) -> i64 {
        ret self.val;
    }
    pf count(self) -> i64 {
        ret 1;
    }
}

struct StringBox {
    val: str,
}

impl Container for StringBox {
    ty Item = str;
    pf first(self) -> str {
        ret self.val;
    }
    pf count(self) -> i64 {
        ret 1;
    }
}

// Factory impl: Product<T> = T (identity)
struct IntFactory {
    _marker: i64,
}

impl Factory for IntFactory {
    ty Product<T> = T;
    pf make_int(self) -> i64 {
        ret 42;
    }
    pf make_str(self) -> str {
        ret "made";
    }
}

// Generic function using associated type
pf get_first<T: Container>(c: T) -> T::Item {
    ret c.first();
}

pf main() -> i64 {
    // Test 1: basic associated types with bare self
    pv ib = IntBox { val: 42 };
    pv r1 = get_first(ib);
    print(r1);  // 42

    pv sb = StringBox { val: "hello" };
    pv r2 = get_first(sb);
    print(r2);  // hello

    // Test 2: container count
    pv c = IntBox { val: 99 };
    print(c.count());  // 1

    // Test 3: GAT — Factory trait with Product<T> associated type
    pv f = IntFactory { _marker: 0 };
    pv i = f.make_int();
    print(i);  // 42

    pv s = f.make_str();
    print(s);  // made

    ret 0;
}
