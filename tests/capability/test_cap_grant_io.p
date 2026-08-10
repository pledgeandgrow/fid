// Capability test: IO capability granted, print works (Phase 2C)
// The `with { IO }` clause grants IO capability to this module.
use core with { IO }

pf main() -> i64 {
    print("hello");
    ret 0;
}
