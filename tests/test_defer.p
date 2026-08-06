// Test defer blocks
pf cleanup_a() {
    print("cleanup A");
}

pf cleanup_b() {
    print("cleanup B");
}

pf main() -> i32 {
    defer {
        cleanup_a();
    }
    defer {
        cleanup_b();
    }
    print("main body");
    ret 0;
}
