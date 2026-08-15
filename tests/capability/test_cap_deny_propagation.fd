// Capability test: effects propagate, denial catches transitive calls (Phase 2C)
// helper() calls print(), so helper() has IO effect.
// main() calls helper(), so main() requires IO capability.
// With only FS granted (not IO), this should fail.
use core with { FS }

pf helper() -> i64 {
    print("from helper");
    ret 0;
}

pf main() -> i64 {
    pv r = helper();  // should fail — helper requires IO, not granted
    ret r;
}
