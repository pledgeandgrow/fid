// Phase 3A — comptime serialization (success metric).
//
// A serialization schema is built at compile time using comptime type
// reflection — no macros, no derive. The resulting string is substituted into
// the program as a constant, so reflection runs zero times at runtime.

struct Point {
    x: i64,
    y: i64,
    label: str,
}

struct Rect {
    w: i64,
    h: i64,
}

// Build a schema string for a struct type at compile time.
// Uses comptime reflection to enumerate field names and types.
comptime pf schema_for_point() -> str {
    pv name = comptime type_name(Point);
    pv fields = comptime field_names(Point);
    pv types = comptime field_types(Point);
    pv count = comptime field_count(Point);

    // Build schema string using f-string interpolation.
    pv out = "(";

    for i in 0..count {
        pv fname = fields[i];
        pv ftype = types[i];
        out = f"{out}{fname} - {ftype}";
        if i + 1 < count {
            out = f"{out}, ";
        }
    }

    out = f"{out})";
    ret out;
}

pf main() -> i64 {
    // The schema string is computed entirely at compile time and substituted
    // as a string literal — no reflection runs at runtime.
    pv s = comptime schema_for_point();
    print(s);
    print("\n");

    // Demonstrate that the same reflection works for any struct.
    pv rname = comptime type_name(Rect);
    pv rcount = comptime field_count(Rect);
    print(rname);
    print(" has ");
    print(rcount);
    print(" fields\n");

    ret 0;
}
