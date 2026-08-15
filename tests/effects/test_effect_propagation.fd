// Effect system test: effect propagation through call graph.
// helper() has IO effect (calls print).
// caller() calls helper() — should infer IO effect.
// main() calls caller() — should also infer IO effect.
pf helper() !IO {
    print("helper");
}

pf caller() !IO {
    helper();
}

pf main() -> i64 !IO {
    caller();
    ret 0;
}
