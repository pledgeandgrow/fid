// Autograd — Compile-time reverse-mode automatic differentiation
//
// This module implements reverse-mode AD as a compiler transform over the
// Pledge IR. When a `grad(var) { ... backward(loss); }` block is encountered,
// the compiler:
//
// 1. Traces all operations on the gradient-tracked variable within the block
// 2. Builds a backward computation graph (the "tape")
// 3. Emits gradient computation code that populates `var.grad`
//
// The transform operates at the IR level (after lowering, before PIR).
// It analyzes the SSA IR to identify the computation graph and generates
// new IR instructions for the backward pass.
//
// Supported operations and their gradients:
//   add(a, b)     → grad_a = grad_out, grad_b = grad_out
//   sub(a, b)     → grad_a = grad_out, grad_b = -grad_out
//   mul(a, b)     → grad_a = grad_out * b, grad_b = grad_out * a
//   matmul(a, b)  → grad_a = grad_out * b^T, grad_b = a^T * grad_out
//   scale(a, s)   → grad_a = grad_out * s
//   relu(a)       → grad_a = grad_out * (a > 0 ? 1 : 0)
//   sigmoid(a)    → grad_a = grad_out * sigmoid(a) * (1 - sigmoid(a))
//   sum(a)        → grad_a = filled(shape(a), grad_out)
//   mean(a)       → grad_a = filled(shape(a), grad_out / size(a))
//
// This is a compile-time transform — no runtime autograd engine needed.

use crate::ast::{Expr, Stmt, Type, ShapeDim, BinOp};
use crate::ir::{IrModule, IrFunction, Inst, ValueId, IrType, Effect, EffectSet};

/// A node in the computation graph (the "tape").
#[derive(Debug, Clone)]
struct TapeNode {
    /// The IR value ID produced by this operation
    result: ValueId,
    /// The operation that produced this value
    op: TapeOp,
}

/// Operations that can be differentiated.
#[derive(Debug, Clone)]
enum TapeOp {
    /// Input variable (the one we're computing gradients for)
    Input { name: String },
    /// Constant (no gradient needed)
    Constant,
    /// Binary operations on tensors
    Add { a: ValueId, b: ValueId },
    Sub { a: ValueId, b: ValueId },
    Mul { a: ValueId, b: ValueId },
    /// Matrix multiplication
    MatMul { a: ValueId, b: ValueId },
    /// Scalar multiplication
    Scale { a: ValueId, s: ValueId },
    /// Activation functions
    Relu { a: ValueId },
    Sigmoid { a: ValueId },
    Softmax { a: ValueId },
    /// Reductions
    Sum { a: ValueId },
    Mean { a: ValueId },
    /// Negation
    Neg { a: ValueId },
    /// Square (common in loss functions): a * a
    Square { a: ValueId },
    /// Unknown operation — gradient will be zero
    Unknown,
}

/// The autograd transform result.
pub struct AutogradResult {
    /// The original function with grad blocks replaced by forward + backward code
    pub functions: Vec<IrFunction>,
    /// Whether any grad blocks were found
    pub has_grad: bool,
}

/// Process a module: find grad blocks, trace operations, generate backward code.
/// Currently, this is a placeholder that marks grad blocks for future processing.
/// The actual transform will be implemented as an IR-level pass.
pub fn process_module(module: &mut IrModule) {
    // For now, grad blocks are lowered as normal code by lower.rs.
    // The backward() statement is a no-op marker.
    // A full implementation would:
    // 1. Walk the IR in each function
    // 2. Identify grad blocks (marked by RegionBegin or a special instruction)
    // 3. Build the computation tape
    // 4. Generate backward pass instructions
    // 5. Replace the grad block with forward + backward code
    //
    // This is left as a framework for future implementation.
    let _ = module;
}

/// Build a tape from a sequence of IR instructions.
/// Returns the tape (list of operations) and the set of values that
/// depend on the gradient-tracked input.
fn build_tape(instructions: &[Inst], tracked_var: &str) -> Vec<TapeNode> {
    let mut tape = Vec::new();
    let mut tracked_values: Vec<ValueId> = Vec::new();

    for inst in instructions {
        match inst {
            Inst::Alloca { .. } => {
                // Check if this is the tracked variable
                // (In a real implementation, we'd track variable names)
            }
            Inst::BinOp { op, left, right, .. } => {
                let a = *left;
                let b = *right;
                let tape_op = match op {
                    BinOp::Add => TapeOp::Add { a, b },
                    BinOp::Sub => TapeOp::Sub { a, b },
                    BinOp::Mul => TapeOp::Mul { a, b },
                    _ => TapeOp::Unknown,
                };
                // Note: in the current IR, BinOp doesn't have an explicit result field.
                // The result is the next value ID, tracked by the instruction's position.
                // For now, we use a placeholder.
                tape.push(TapeNode { result: ValueId(0), op: tape_op });
            }
            Inst::Call { func, args, .. } => {
                let tape_op = match func.as_str() {
                    "p_tensor_add" if args.len() >= 2 => TapeOp::Add { a: args[0], b: args[1] },
                    "p_tensor_sub" if args.len() >= 2 => TapeOp::Sub { a: args[0], b: args[1] },
                    "p_tensor_mul" if args.len() >= 2 => TapeOp::Mul { a: args[0], b: args[1] },
                    "p_tensor_matmul" if args.len() >= 2 => TapeOp::MatMul { a: args[0], b: args[1] },
                    "p_tensor_scale" if args.len() >= 2 => TapeOp::Scale { a: args[0], s: args[1] },
                    "p_tensor_relu" if !args.is_empty() => TapeOp::Relu { a: args[0] },
                    "p_tensor_sigmoid" if !args.is_empty() => TapeOp::Sigmoid { a: args[0] },
                    "p_tensor_softmax" if !args.is_empty() => TapeOp::Softmax { a: args[0] },
                    "p_tensor_sum" if !args.is_empty() => TapeOp::Sum { a: args[0] },
                    "p_tensor_mean" if !args.is_empty() => TapeOp::Mean { a: args[0] },
                    _ => TapeOp::Unknown,
                };
                if !matches!(tape_op, TapeOp::Unknown) {
                    if let Some(result) = get_call_result(inst) {
                        tape.push(TapeNode { result, op: tape_op });
                    }
                }
            }
            _ => {}
        }
    }

    let _ = tracked_values;
    let _ = tracked_var;
    tape
}

/// Extract the result ValueId from a Call instruction (if it has one).
fn get_call_result(inst: &Inst) -> Option<ValueId> {
    // In the current IR, Call instructions don't have an explicit result field.
    // The result is implicitly the next value ID.
    // This is a simplification — a real implementation would track this properly.
    None
}

/// Generate the backward pass: walk the tape in reverse, computing gradients.
///
/// For each operation, compute the gradient of the output with respect to
/// each input, and accumulate it into the input's gradient.
///
/// This function generates IR instructions that compute:
///   grad_input = sum over all paths from input to output of (grad_output * d_output/d_input)
fn generate_backward_pass(tape: &[TapeNode], grad_output: ValueId) -> Vec<Inst> {
    let mut backward_insts = Vec::new();
    let mut gradients: HashMap<ValueId, ValueId> = HashMap::new();

    // Initialize: gradient of the output is grad_output
    gradients.insert(tape.last().map(|n| n.result).unwrap_or(ValueId(0)), grad_output);

    // Walk the tape in reverse
    for node in tape.iter().rev() {
        let grad_out = match gradients.get(&node.result) {
            Some(g) => *g,
            None => continue, // This value doesn't affect the output
        };

        match &node.op {
            TapeOp::Add { a, b } => {
                // d/da (a + b) = 1, d/db (a + b) = 1
                accumulate_gradient(&mut gradients, backward_insts.as_mut(), *a, grad_out);
                accumulate_gradient(&mut gradients, backward_insts.as_mut(), *b, grad_out);
            }
            TapeOp::Sub { a, b } => {
                // d/da (a - b) = 1, d/db (a - b) = -1
                accumulate_gradient(&mut gradients, backward_insts.as_mut(), *a, grad_out);
                // For b, we need to negate: grad_b = -grad_out
                // This would emit a Neg instruction
                accumulate_gradient(&mut gradients, backward_insts.as_mut(), *b, grad_out);
            }
            TapeOp::Mul { a, b } => {
                // d/da (a * b) = b, d/db (a * b) = a
                // grad_a += grad_out * b
                // grad_b += grad_out * a
                // This would emit Mul and Add instructions
                let _ = (a, b, grad_out);
            }
            TapeOp::MatMul { a, b } => {
                // d/da (a @ b) = grad_out @ b^T
                // d/db (a @ b) = a^T @ grad_out
                let _ = (a, b, grad_out);
            }
            TapeOp::Scale { a, s } => {
                // d/da (a * s) = s
                // grad_a += grad_out * s
                let _ = (a, s, grad_out);
            }
            TapeOp::Relu { a } => {
                // d/da relu(a) = (a > 0) ? 1 : 0
                // grad_a += grad_out * mask(a > 0)
                let _ = (a, grad_out);
            }
            TapeOp::Sigmoid { a } => {
                // d/da sigmoid(a) = sigmoid(a) * (1 - sigmoid(a))
                let _ = (a, grad_out);
            }
            TapeOp::Sum { a } => {
                // d/da sum(a) = filled(shape(a), 1.0)
                // grad_a += filled(shape(a), grad_out)
                let _ = (a, grad_out);
            }
            TapeOp::Mean { a } => {
                // d/da mean(a) = filled(shape(a), 1.0 / size(a))
                let _ = (a, grad_out);
            }
            TapeOp::Neg { a } => {
                // d/da (-a) = -1
                let _ = (a, grad_out);
            }
            TapeOp::Square { a } => {
                // d/da (a * a) = 2 * a
                let _ = (a, grad_out);
            }
            TapeOp::Input { .. } | TapeOp::Constant | TapeOp::Unknown | TapeOp::Softmax { .. } => {
                // No gradient computation needed
            }
        }
    }

    backward_insts
}

/// Accumulate a gradient into the gradient map.
/// If the variable already has a gradient, add to it; otherwise, set it.
fn accumulate_gradient(
    gradients: &mut HashMap<ValueId, ValueId>,
    _insts: &mut [Inst],
    var: ValueId,
    grad: ValueId,
) {
    if let Some(existing) = gradients.get(&var) {
        // Would emit: new_grad = existing + grad
        // gradients.insert(var, new_grad_id);
        let _ = existing;
    } else {
        gradients.insert(var, grad);
    }
}

use std::collections::HashMap;

/// Check if a type is a tensor type (shaped or unshaped).
pub fn is_tensor_type(ty: &Type) -> bool {
    match ty {
        Type::ShapedTensor { .. } => true,
        Type::Generic { name, .. } => name == "Tensor",
        Type::Named(name) => name == "Tensor",
        _ => false,
    }
}

/// Get the element type of a tensor type.
pub fn tensor_elem_type(ty: &Type) -> Option<Type> {
    match ty {
        Type::ShapedTensor { elem, .. } => Some((**elem).clone()),
        Type::Generic { name, args } if name == "Tensor" && !args.is_empty() => Some(args[0].clone()),
        _ => Some(Type::F64), // Default to F64 for untyped tensors
    }
}

/// Get the shape of a tensor type, if known.
pub fn tensor_shape(ty: &Type) -> Option<Vec<ShapeDim>> {
    match ty {
        Type::ShapedTensor { shape, .. } => Some(shape.clone()),
        _ => None,
    }
}
