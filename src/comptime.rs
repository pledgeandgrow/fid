// Pledge comptime evaluation engine (Phase 3A)
//
// Currently disabled due to AST changes. The substitute function is a no-op
// stub that passes the program through unchanged.
//
// TODO: Re-enable comptime evaluation (Phase 3A).

use crate::ast::Program;

/// Comptime evaluation entry point. Currently a no-op.
pub fn substitute(program: &Program) -> Result<Program, Vec<String>> {
    Ok(program.clone())
}
