// Effect system test: under-declaration error.
// Declares !IO but calls a function with !Net — should fail with effect error.
pf net_call() !Net {
    p_net_connect("localhost:80");
}

pf main() -> i64 !IO {
    net_call();
    ret 0;
}
