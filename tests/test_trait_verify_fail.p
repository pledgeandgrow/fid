// Phase 3B.4 — Trait impl verification.
// This test verifies that the typechecker catches missing methods
// and missing associated types in trait impls.
//
// This test should FAIL to compile with a type error.

cap Shape {
    ty Area;
    pf area(self) -> Area;
    pf perimeter(self) -> i64;
}

struct Circle {
    radius: i64,
}

// Missing `perimeter` method and `Area` associated type — should error.
impl Shape for Circle {
    pub pf area(self: Circle) -> i64 {
        ret self.radius * 2;
    }
}

pf main() -> i64 {
    ret 0;
}
