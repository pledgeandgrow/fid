// Test defer in PE emitter
pf main() -> i64 {
    defer { print("cleanup B"); }
    defer { print("cleanup A"); }
    print("main body");
    ret 0;
}
