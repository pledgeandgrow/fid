// Test struct methods in PE emitter
struct Point {
    x: i64,
    y: i64,
}

impl Point {
    pf new(x: i64, y: i64) -> Point {
        ret Point { x: x, y: y };
    }

    pf distance(self: Point) -> i64 {
        ret self.x + self.y;
    }
}

pf main() -> i64 {
    pv p = Point::new(3, 4);
    print(p.distance());
    print(p.x);
    print(p.y);
    ret 0;
}
