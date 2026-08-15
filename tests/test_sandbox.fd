// Test: sandbox { } — capability revocation blocks (F1)
// Pure code inside a sandbox should compile fine.
// IO/FS/Net calls inside sandbox are compile errors.

pf helper(x: i64) -> i64 !{} {
    ret x * 2;
}

pf main() {
    pv result = helper(21);
    print(result);
    print("\n");

    sandbox {
        // Pure function calls are fine inside sandbox
        pv inner = helper(5);
        // Can't print inside sandbox (IO revoked)
        // But we can compute and store
    }

    print("ok");
    print("\n");

    ret 0;
}
