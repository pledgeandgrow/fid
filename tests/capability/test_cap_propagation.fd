// Capability test: effects propagate through call graph (Phase 2C)
// helper() calls print(), so helper() has IO effect.
// main() calls helper(), so main() requires IO capability.
// With IO granted, this should work.
use core with { IO }

pf helper() -> i64 {
    print("from helper");
    ret 0;
}

pf main() -> i64 {
    pv r = helper();
    ret r;
}
