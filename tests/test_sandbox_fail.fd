// Test: sandbox should reject IO calls inside the block
// This test should produce a compile error.

pf main() {
    sandbox {
        print("this should fail");
    }
    ret 0;
}
