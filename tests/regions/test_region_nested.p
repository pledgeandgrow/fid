// Region inference test: nested regions.
pf main() -> i64 !IO {
    region outer {
        pv a = 1;
        region inner {
            pv b = 2;
            print(a + b);
        }
        print(a);
    }
    ret 0;
}
