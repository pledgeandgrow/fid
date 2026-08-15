// Test: persist variables (F2)
// In interpreter mode, persist variables use the default value
// (no hot reload state). In watch mode, they survive reloads.

pf main() {
    persist pv counter: i64 = 0;
    counter += 1;
    print(f"counter={counter}");
    print("\n");

    persist pv name = "test";
    print(f"name={name}");
    print("\n");

    ret 0;
}
