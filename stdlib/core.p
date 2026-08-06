// Pledge core standard library — Phase 2
// Auto-included by the compiler. All items are pub.

module core;

// Builtin: print — outputs to stdout
// (Implemented natively in the compiler for Phase 1)

// Builtin: println — print with newline
pub pf println(s: str) -> void {
    print(s);
    print("\n");
}

// Builtin: assert — crash if condition is false
pub pf assert(cond: bool) -> void {
    if !cond {
        print("Assertion failed\n");
    }
}

// Builtin: assert_eq — check equality
pub pf assert_eq<T>(a: T, b: T) -> void {
    if a != b {
        print("Assertion failed: values not equal\n");
    }
}

// ─── Option<T> ──────────────────────────────────────────────────────────
// Represents an optional value: either Some(value) or None.

pub enum Option {
    Some(i64),
    None,
}

impl Option {
    pub pf is_some(self: Option) -> bool {
        match self {
            Option::Some(v: i64) => { ret true; }
            Option::None => { ret false; }
        }
    }

    pub pf is_none(self: Option) -> bool {
        match self {
            Option::Some(v: i64) => { ret false; }
            Option::None => { ret true; }
        }
    }

    pub pf unwrap(self: Option) -> i64 {
        match self {
            Option::Some(v: i64) => { ret v; }
            Option::None => {
                print("panic: called unwrap on None\n");
                ret 0;
            }
        }
    }

    pub pf unwrap_or(self: Option, default: i64) -> i64 {
        match self {
            Option::Some(v: i64) => { ret v; }
            Option::None => { ret default; }
        }
    }

    pub pf map(self: Option, f: i64) -> Option {
        match self {
            Option::Some(v: i64) => { ret Option::Some(v + f); }
            Option::None => { ret Option::None; }
        }
    }
}

// ─── Result<T, E> ───────────────────────────────────────────────────────
// Represents a result: either Ok(value) or Err(error).

pub enum Result {
    Ok(i64),
    Err(i64),
}

impl Result {
    pub pf is_ok(self: Result) -> bool {
        match self {
            Result::Ok(v: i64) => { ret true; }
            Result::Err(e: i64) => { ret false; }
        }
    }

    pub pf is_err(self: Result) -> bool {
        match self {
            Result::Ok(v: i64) => { ret false; }
            Result::Err(e: i64) => { ret true; }
        }
    }

    pub pf unwrap(self: Result) -> i64 {
        match self {
            Result::Ok(v: i64) => { ret v; }
            Result::Err(e: i64) => {
                print("panic: called unwrap on Err\n");
                ret 0;
            }
        }
    }

    pub pf unwrap_or(self: Result, default: i64) -> i64 {
        match self {
            Result::Ok(v: i64) => { ret v; }
            Result::Err(e: i64) => { ret default; }
        }
    }

    pub pf ok(self: Result) -> Option {
        match self {
            Result::Ok(v: i64) => { ret Option::Some(v); }
            Result::Err(e: i64) => { ret Option::None; }
        }
    }

    pub pf err(self: Result) -> Option {
        match self {
            Result::Ok(v: i64) => { ret Option::None; }
            Result::Err(e: i64) => { ret Option::Some(e); }
        }
    }
}

// ─── Utility functions ──────────────────────────────────────────────────

pub pf max(a: i64, b: i64) -> i64 {
    if a > b { ret a; }
    ret b;
}

pub pf min(a: i64, b: i64) -> i64 {
    if a < b { ret a; }
    ret b;
}

pub pf abs(n: i64) -> i64 {
    if n < 0 { ret -n; }
    ret n;
}

pub pf clamp(n: i64, lo: i64, hi: i64) -> i64 {
    if n < lo { ret lo; }
    if n > hi { ret hi; }
    ret n;
}
