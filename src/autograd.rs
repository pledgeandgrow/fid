// Autograd — Reverse-mode automatic differentiation
//
// Autodiff is implemented as a **runtime** engine in the interpreter, not as
// an IR-level transform. The lowerer emits `GradBegin`/`GradEnd`/`Backward`
// instructions, and the interpreter builds a tape at runtime and computes
// gradients when `Backward` is executed.
//
// Supported operations and their gradients (implemented in src/interp.rs):
//   add(a, b)     → grad_a = grad_out, grad_b = grad_out
//   sub(a, b)     → grad_a = grad_out, grad_b = -grad_out
//   mul(a, b)     → grad_a = grad_out * b, grad_b = grad_out * a
//   matmul(a, b)  → grad_a = grad_out * b^T, grad_b = a^T * grad_out
//   scale(a, s)   → grad_a = grad_out * s
//   relu(a)       → grad_a = grad_out * (a > 0 ? 1 : 0)
//   sigmoid(a)    → grad_a = grad_out * sigmoid(a) * (1 - sigmoid(a))
//   sum(a)        → grad_a = filled(shape(a), grad_out)
//   mean(a)       → grad_a = filled(shape(a), grad_out / size(a))

use crate::ir::IrModule;

/// Process a module: find grad blocks, trace operations, generate backward code.
///
/// Autodiff is implemented as a **runtime** engine in the interpreter, not as
/// an IR-level transform. The lowerer emits `GradBegin`/`GradEnd`/`Backward`
/// instructions, and the interpreter builds a tape at runtime and computes
/// gradients when `Backward` is executed.
///
/// This function is kept for API compatibility but is a no-op. The actual
/// gradient computation happens in `Interpreter::run_backward()`.
pub fn process_module(module: &mut IrModule) {
    let _ = module;
}
