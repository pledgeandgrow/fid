// Test basic trait (cap) system
cap Printable {
    pf format(self) -> i64;
}

cap Addable {
    pf add(self, other: i64) -> i64;
}

struct Point {
    x: i64,
    y: i64,
}

impl Point {
    pub pf new(x: i64, y: i64) -> Point {
        ret Point { x: x, y: y };
    }
}

impl Printable for Point {
    pub pf format(self: Point) -> i64 {
        ret self.x + self.y;
    }
}

impl Addable for Point {
    pub pf add(self: Point, other: i64) -> i64 {
        ret self.x + self.y + other;
    }
}

// Generic function with trait bound
pf print_value<T: Printable>(item: T) -> i64 {
    ret item.format();
}

pf add_value<T: Addable>(item: T, n: i64) -> i64 {
    ret item.add(n);
}

pf main() -> i64 {
    pv p = Point::new(10, 20);
    print(p.format());
    print(p.add(5));

    pv r = print_value(p);
    print(r);

    pv s = add_value(p, 100);
    print(s);

    ret 0;
}
