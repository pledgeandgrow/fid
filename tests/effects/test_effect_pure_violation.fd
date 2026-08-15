// Effect system test: pure function violation.
// Declares !{} (pure) but calls print — should fail with effect error.
pf bad() !{} {
    print("not pure!");
}

pf main() -> i64 !IO {
    bad();
    ret 0;
}
