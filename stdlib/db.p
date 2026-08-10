// Pledge database driver framework — Phase 5D.4
//
// Database driver framework for the "Web & Cloud" platform milestone.
//
// Design: A `Database` capability (trait) defines the query/exec interface.
// Drivers implement this trait for specific databases. A stub `MemoryDB`
// implementation is provided for testing. Real drivers (SQLite, Postgres)
// will be implemented as native builtins in future phases.
//
// Results are represented as JSON strings (rows as JSON arrays, result
// sets as JSON arrays of objects) to work within Pledge's current type
// system. When comptime reflection lands (Phase 3A), this will be
// replaced with typed row mapping.

module db;

use json;

/// Database driver capability — any database connection implements this.
/// Methods:
///   query(sql: str) -> str       — execute SELECT, return JSON array of rows
///   exec(sql: str) -> i64        — execute INSERT/UPDATE/DELETE, return rows affected
///   close() -> void              — close the connection
cap Database {
    pf query(self, sql: str) -> str;
    pf exec(self, sql: str) -> i64;
    pf close(self);
}

/// In-memory database stub for testing.
/// Stores rows in a simple format. Supports basic SELECT/INSERT.
struct MemoryDB {
    handle: i64,  // opaque handle to native storage
}

impl MemoryDB {
    pub pf new() -> MemoryDB {
        // Create a native in-memory store
        pv handle = p_db_mem_open();
        ret MemoryDB { handle: handle };
    }
}

impl Database for MemoryDB {
    pub pf query(self: MemoryDB, sql: str) -> str {
        ret p_db_mem_query(self.handle, sql);
    }

    pub pf exec(self: MemoryDB, sql: str) -> i64 {
        ret p_db_mem_exec(self.handle, sql);
    }

    pub pf close(self: MemoryDB) {
        p_db_mem_close(self.handle);
    }
}

/// Helper: query a single row as a JSON object. Returns "" if no rows.
pub pf query_one<T: Database>(db: T, sql: str) -> str {
    pv rows = db.query(sql);
    pv count = json_len(rows);
    if count == 0 {
        ret "";
    }
    ret json_at(rows, 0);
}

/// Helper: query a scalar value (first column of first row).
pub pf query_scalar<T: Database>(db: T, sql: str) -> str {
    pv row = query_one(db, sql);
    if row == "" {
        ret "";
    }
    // Get the first key's value from the row object
    // Since we can't iterate object keys in the current type system,
    // we return the whole row and let the caller extract what they need.
    ret row;
}
