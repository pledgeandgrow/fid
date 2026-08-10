// Test: Database driver framework — Phase 5D.4
// Tests the MemoryDB stub implementation: insert, query, query_one.

use db;
use json;

pf main() {
    // ─── Create database and insert rows ─────────────────────────────
    pv database = MemoryDB::new();

    pv n1 = database.exec("INSERT INTO users (key, value) VALUES ('alice', 'admin')");
    print(n1);
    print("\n");

    pv n2 = database.exec("INSERT INTO users (key, value) VALUES ('bob', 'user')");
    print(n2);
    print("\n");

    pv n3 = database.exec("INSERT INTO users (key, value) VALUES ('carol', 'user')");
    print(n3);
    print("\n");

    // ─── Query all rows ──────────────────────────────────────────────
    pv all = database.query("SELECT * FROM users");
    print(all);
    print("\n");

    // ─── Query with WHERE clause ─────────────────────────────────────
    pv filtered = database.query("SELECT * FROM users WHERE key='bob'");
    print(filtered);
    print("\n");

    // ─── Query one helper ────────────────────────────────────────────
    pv one = query_one(database, "SELECT * FROM users WHERE key='alice'");
    print(one);
    print("\n");

    // ─── Extract field from result ───────────────────────────────────
    pv value = json_get(one, "value");
    print(value);
    print("\n");

    // ─── Count rows ──────────────────────────────────────────────────
    pv count = json_len(all);
    print(count);
    print("\n");

    // ─── Delete rows ─────────────────────────────────────────────────
    pv deleted = database.exec("DELETE FROM users WHERE key='bob'");
    print(deleted);
    print("\n");

    pv remaining = database.query("SELECT * FROM users");
    print(remaining);
    print("\n");

    pv remaining_count = json_len(remaining);
    print(remaining_count);
    print("\n");

    // ─── Close ───────────────────────────────────────────────────────
    database.close();

    ret 0;
}
