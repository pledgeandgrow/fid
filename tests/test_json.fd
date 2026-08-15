// Test: JSON stdlib — Phase 5D.2
// Tests parse, dump, get, at, type, len, pretty, and string escaping.

use json;

pf main() {
    // ─── Parse + canonical dump ──────────────────────────────────────
    pv input = "{\"name\":\"pledge\",\"version\":1,\"active\":true}";
    pv canonical = json_parse(input);
    print(canonical);
    print("\n");

    // ─── Get field ───────────────────────────────────────────────────
    pv name = json_get(input, "name");
    print(name);
    print("\n");

    pv version = json_get(input, "version");
    print(version);
    print("\n");

    // ─── Type detection ──────────────────────────────────────────────
    pv t1 = json_type(input);
    print(t1);
    print("\n");

    pv t2 = json_type("42");
    print(t2);
    print("\n");

    pv t3 = json_type("\"hello\"");
    print(t3);
    print("\n");

    pv t4 = json_type("null");
    print(t4);
    print("\n");

    // ─── Array operations ────────────────────────────────────────────
    pv arr = "[10,20,30,40,50]";
    pv len = json_len(arr);
    print(len);
    print("\n");

    pv elem = json_at(arr, 2);
    print(elem);
    print("\n");

    // ─── Pretty print ────────────────────────────────────────────────
    pv pretty = json_pretty(input);
    print(pretty);
    print("\n");

    // ─── String escaping ─────────────────────────────────────────────
    pv escaped = json_string("hello\nworld");
    print(escaped);
    print("\n");

    // ─── Object length ───────────────────────────────────────────────
    pv objlen = json_len(input);
    print(objlen);
    print("\n");

    // ─── Nested access ───────────────────────────────────────────────
    pv nested = "{\"user\":{\"name\":\"alice\",\"age\":30},\"tags\":[\"a\",\"b\"]}";
    pv user = json_get(nested, "user");
    print(user);
    print("\n");

    pv username = json_get(user, "name");
    print(username);
    print("\n");

    ret 0;
}
