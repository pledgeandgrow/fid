// Test: Database driver framework — Phase 5D.4
// Tests the MemoryDB stub implementation: insert, query, query_one.

use db;
use json;

pf main() {
    // ─── Create database and insert rows ─────────────────────────────
    pv database = MemoryDB::new();

    // DB stub uses simple "SET key value" or "INSERT key value" format
    pv n1 = database.exec("INSERT alice admin");
    print(n1);
    print("\n");

    pv n2 = database.exec("INSERT bob user");
    print(n2);
    print("\n");

    pv n3 = database.exec("INSERT carol user");
    print(n3);
    print("\n");

    // ─── Query all rows ──────────────────────────────────────────────
    pv all = database.query("SELECT * FROM users");
    print(all);
    print("\n");

    // ─── Query one helper ────────────────────────────────────────────
    pv one = query_one(database, "SELECT * FROM users");
    print(one);
    print("\n");

    // ─── Count rows ──────────────────────────────────────────────────
    pv count = json_len(all);
    print(count);
    print("\n");

    // ─── Close ───────────────────────────────────────────────────────
    database.close();

    ret 0;
}
