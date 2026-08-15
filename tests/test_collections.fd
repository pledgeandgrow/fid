// Test String, Vec, and HashMap methods
pf main() -> i64 {
    // ─── String methods ───
    pv s = String::from("Hello World");
    println(s.len());           // 11
    println(s.contains("World")); // true
    println(s.contains("xyz"));   // false
    println(s.starts_with("Hello")); // true
    println(s.ends_with("World"));   // true
    println(s.to_uppercase());   // HELLO WORLD
    println(s.to_lowercase());   // hello world

    pv t = String::from("  trim me  ");
    println(t.trim());          // trim me

    pv r = String::from("a,b,c");
    pv parts = r.split(",");
    println(parts.len());       // 3
    println(parts.get(0));      // a
    println(parts.get(1));      // b
    println(parts.get(2));      // c

    pv replaced = String::from("hello world").replace("world", "Pledge");
    println(replaced);          // hello Pledge

    println(String::from("42").parse_int()); // 42

    pv pos = String::from("hello world").find("world");
    println(pos);               // 6

    pv neg = String::from("hello").find("xyz");
    println(neg);               // -1

    println(String::from("ab").repeat(3)); // ababab

    // ─── Vec methods ───
    pv v = Vec::new();
    v.push(10);
    v.push(20);
    v.push(30);
    println(v.len());           // 3
    println(v.first());         // 10
    println(v.last());          // 30
    println(v.contains(20));    // true
    println(v.contains(99));    // false

    v.insert(1, 15);
    println(v.get(1));          // 15
    println(v.len());           // 4

    pv removed = v.remove(0);
    println(removed);           // 10
    println(v.get(0));          // 15

    v.reverse();
    println(v.get(0));          // 30
    println(v.get(1));          // 20
    println(v.get(2));          // 15

    v.sort();
    println(v.get(0));          // 15
    println(v.get(1));          // 20
    println(v.get(2));          // 30

    v.truncate(2);
    println(v.len());           // 2

    pv v2 = Vec::new();
    v2.push(100);
    v2.push(200);
    v.extend(v2);
    println(v.len());           // 4
    println(v.get(2));          // 100
    println(v.get(3));          // 200

    pv v3 = v.clone();
    println(v3.len());          // 4

    v.clear();
    println(v.len());           // 0
    println(v.is_empty());      // true

    // ─── HashMap methods ───
    pv m = HashMap::new();
    m.insert("a", 1);
    m.insert("b", 2);
    m.insert("c", 3);
    println(m.len());           // 3
    println(m.contains("a"));   // true
    println(m.contains("z"));   // false
    println(m.get("a"));        // 1
    println(m.get_or("z", 99)); // 99

    m.remove("b");
    println(m.len());           // 2
    println(m.contains("b"));   // false

    pv m2 = m.clone();
    println(m2.len());          // 2

    m.clear();
    println(m.len());           // 0
    println(m.is_empty());      // true

    ret 0;
}
