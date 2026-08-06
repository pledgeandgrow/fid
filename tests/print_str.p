// Test: just print a string using inline assembly stubs
// This tests if the IAT calls work at all from main's context
pf main() {
    print("Hello!");
    ret 0;
}
