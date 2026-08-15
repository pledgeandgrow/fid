// Pledge auto-parallelization framework — Phase 5C.4
//
// Uses the effect system to identify pure (side-effect-free) loops that
// can be safely parallelized. The analysis is conservative: only loops
// with no IO, no mutation of shared state, and no panics are candidates.
//
// This module provides:
//   1. A `parallel_for` construct that marks a loop as parallelizable
//   2. A `is_parallelizable` analysis function (comptime in future)
//   3. A `par_map` helper for embarrassingly parallel map operations
//
// In the current interpreter, these run sequentially. When the effect
// system is fully integrated (Phase 2B), the compiler will automatically
// parallelize loops identified as pure.

module parallel;

/// Parallel map: apply a function to each element of a Vec.
/// In the current implementation, this runs sequentially.
/// The compiler identifies this as parallelizable because:
///   - Each iteration is independent (no data dependencies)
///   - The function has no IO effects (verified by effect system)
///   - No shared mutable state
pub pf par_map(n: i64) -> Vec<i64> {
    pv result = Vec::new();
    pv i = 0;
    while i < n {
        // Each iteration is independent — no shared state
        result.push(i * i);
        i = i + 1;
    }
    ret result;
}

/// Parallel sum: compute the sum of a Vec in parallel chunks.
/// This is a reduction — the compiler can split the Vec into chunks,
/// sum each chunk in parallel, then combine the partial sums.
pub pf par_sum(data: Vec<i64>) -> i64 {
    pv total = 0;
    pv i = 0;
    pv n = data.len();
    while i < n {
        total = total + data.get(i);
        i = i + 1;
    }
    ret total;
}

/// Parallel dot product: compute the dot product of two Vecs.
/// This is a reduction that can be parallelized.
pub pf par_dot(a: Vec<i64>, b: Vec<i64>) -> i64 {
    pv total = 0;
    pv i = 0;
    pv n = a.len();
    while i < n {
        total = total + a.get(i) * b.get(i);
        i = i + 1;
    }
    ret total;
}

/// Parallel fill: fill a Vec with a constant value.
/// Each element is independent — embarrassingly parallel.
pub pf par_fill(n: i64, val: i64) -> Vec<i64> {
    pv result = Vec::new();
    pv i = 0;
    while i < n {
        result.push(val);
        i = i + 1;
    }
    ret result;
}

/// Parallel range: generate a sequence [start, start+1, ..., end-1].
/// Each element is independent.
pub pf par_range(start: i64, end: i64) -> Vec<i64> {
    pv result = Vec::new();
    pv i = start;
    while i < end {
        result.push(i);
        i = i + 1;
    }
    ret result;
}

/// Check if a loop is parallelizable by analyzing its effects.
/// Returns true if the loop body has no IO, no panic, and no shared
/// mutable state. In the current implementation, this is a stub that
/// returns true for simple loops. When the effect system is fully
/// integrated, this will be a comptime analysis.
pub pf is_parallelizable(effect_mask: i64) -> bool {
    // Effect bits:
    //   0 = IO
    //   1 = State (mutation)
    //   2 = Panic
    //   3 = RealTime
    // A loop is parallelizable if it has no IO, no State, no Panic.
    pv has_io = (effect_mask & 1) != 0;
    pv has_state = (effect_mask & 2) != 0;
    pv has_panic = (effect_mask & 4) != 0;
    ret !has_io && !has_state && !has_panic;
}

/// Mark a loop as parallel for the compiler.
/// This is a hint — the compiler will verify safety before parallelizing.
/// In the current implementation, it's a no-op.
pub pf parallel_hint() -> void {
    // No-op in interpreter — compiler directive for future parallelization
}
