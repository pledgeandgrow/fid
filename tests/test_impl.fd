// Test impl methods on structs

struct Point {
    x: i32,
    y: i32,
}

impl Point {
    pf new(x: i32, y: i32) -> Point {
        Point { x: x, y: y }
    }

    pf distance_from_origin(self: Point) -> i32 {
        // Simplified: just return x + y (not real distance)
        ret self.x + self.y;
    }

    pf translate(self: Point, dx: i32, dy: i32) -> Point {
        ret Point { x: self.x + dx, y: self.y + dy };
    }
}

pf main() -> i32 {
    pv p = Point::new(3, 4);
    pv d = p.distance_from_origin();
    print(d);

    pv p2 = p.translate(10, 20);
    print(p2.x);
    print(p2.y);

    ret 0;
}
