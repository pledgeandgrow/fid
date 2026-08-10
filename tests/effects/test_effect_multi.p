// Effect system test: multiple effects in annotation.
// Declares !{IO, Net} and performs IO — should compile (over-declaring is OK).
pf greet() !{IO, Net} {
    print("hello");
}

pf main() -> i64 !IO {
    greet();
    ret 0;
}
