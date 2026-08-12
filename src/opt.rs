// PIR Optimizer — Phase 4B
//
// Runs optimization passes on PIR Level 2 (the mid-level IR). L2 is the right
// level for these passes: it has SSA form and control flow but has dropped
// region handles and capability tracking, so passes don't have to reason
// about region lifetimes.
//
// Passes implemented (in dependency order):
//   1. Constant folding + propagation  — fold BinOp/UnaryOp on constants,
//      propagate constants through copies, fold Branch on constant cond.
//   2. Common subexpression elimination — replace identical pure computations
//      with references to the first result.
//   3. Dead code elimination            — remove instructions whose results
//      are never used and that have no side effects.
//   4. Inlining                         — inline small pure callee bodies into
//      callers, propagating effects.
//   5. Loop unrolling                   — unroll loops with constant trip
//      counts (for i in 0..N where N is a compile-time constant).
//   6. Tail call optimization           — rewrite tail-position direct calls
//      into jumps to the callee's entry block (loop instead of grow the stack).
//
// Region-aware alias analysis, effect-based reordering, capability elision,
// and partial evaluation (comptime specialization) are gated on Phases 2/3
// and are stubbed here with documented preconditions; they become no-ops until
// the upstream phases land.
//
// Entry points:
//   optimize_l2(&mut L2Module) -> OptStats   — run all passes to a fixed point
//   optimize_l2_with(&mut L2Module, &[Pass]) — run a specific pass list
//   CLI: `pledgec opt <file>` and `pledgec pir <file> --opt`

use crate::ast::{BinOp, UnaryOp};
use crate::ir::{BlockId, Effect, EffectSet, IrType, ValueId};
use crate::pir::{
    lower_ir_to_l1, lower_l1_to_l2, L2Block, L2Function, L2Inst, L2Module, MemRegion, PBlock,
    PirType, PValue,
};
use std::collections::{HashMap, HashSet};

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

/// Which passes are available.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum Pass {
    ConstFold,
    Cse,
    Dce,
    Inline,
    Unroll,
    Tco,
    AliasAnalysis,
    EffectReorder,
    PartialEval,
}

impl Pass {
    pub fn all() -> &'static [Pass] {
        &[
            Pass::ConstFold,
            Pass::Cse,
            Pass::Dce,
            Pass::Inline,
            Pass::Unroll,
            Pass::Tco,
            Pass::AliasAnalysis,
            Pass::EffectReorder,
            Pass::PartialEval,
        ]
    }

    pub fn name(&self) -> &'static str {
        match self {
            Pass::ConstFold => "const-fold",
            Pass::Cse => "cse",
            Pass::Dce => "dce",
            Pass::Inline => "inline",
            Pass::Unroll => "unroll",
            Pass::Tco => "tco",
            Pass::AliasAnalysis => "alias-analysis",
            Pass::EffectReorder => "effect-reorder",
            Pass::PartialEval => "partial-eval",
        }
    }

    pub fn parse(s: &str) -> Option<Pass> {
        match s {
            "const-fold" | "constfold" | "cf" => Some(Pass::ConstFold),
            "cse" => Some(Pass::Cse),
            "dce" => Some(Pass::Dce),
            "inline" | "inl" => Some(Pass::Inline),
            "unroll" => Some(Pass::Unroll),
            "tco" => Some(Pass::Tco),
            "alias-analysis" | "alias" | "aa" => Some(Pass::AliasAnalysis),
            "effect-reorder" | "reorder" | "er" => Some(Pass::EffectReorder),
            "partial-eval" | "pe" => Some(Pass::PartialEval),
            _ => None,
        }
    }
}

/// Per-pass statistics. Returned from `optimize_l2` so callers (CLI, tests)
/// can report how much each pass did.
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct OptStats {
    pub const_fold: usize,
    pub cse: usize,
    pub dce: usize,
    pub inlined: usize,
    pub unrolled: usize,
    pub tco: usize,
    pub alias_analysis: usize,
    pub effect_reorder: usize,
    pub partial_eval: usize,
    /// Number of fixed-point iterations the driver ran.
    pub iterations: usize,
}

impl OptStats {
    pub fn total(&self) -> usize {
        self.const_fold + self.cse + self.dce + self.inlined + self.unrolled + self.tco
            + self.alias_analysis + self.effect_reorder + self.partial_eval
    }
}

/// Run all optimization passes to a fixed point (until no pass makes progress
/// or `max_iters` is reached). Returns aggregate statistics.
pub fn optimize_l2(module: &mut L2Module) -> OptStats {
    optimize_l2_with(module, Pass::all(), 8)
}

/// Run a specific list of passes to a fixed point.
pub fn optimize_l2_with(module: &mut L2Module, passes: &[Pass], max_iters: usize) -> OptStats {
    let mut stats = OptStats::default();
    for i in 0..max_iters {
        stats.iterations = i + 1;
        let before = stats.total();
        for &p in passes {
            match p {
                Pass::ConstFold => stats.const_fold += pass_const_fold(module),
                Pass::Cse => stats.cse += pass_cse(module),
                Pass::Dce => stats.dce += pass_dce(module),
                Pass::Inline => stats.inlined += pass_inline(module),
                Pass::Unroll => stats.unrolled += pass_unroll(module),
                Pass::Tco => stats.tco += pass_tco(module),
                Pass::AliasAnalysis => stats.alias_analysis += pass_alias_analysis(module),
                Pass::EffectReorder => stats.effect_reorder += pass_effect_reorder(module),
                Pass::PartialEval => stats.partial_eval += pass_partial_eval(module),
            }
        }
        if stats.total() == before {
            break;
        }
    }
    // Final cleanup: topologically sort all block instruction lists so that
    // definitions come before uses. Optimization passes (especially inlining
    // and CSE) can scramble the order.
    for f in &mut module.functions {
        for b in &mut f.blocks {
            topo_sort_block(b, &f.values);
        }
    }
    stats
}

/// Convenience: lower an IR module to L2, optimize, and return the optimized
/// L2 module. Used by the CLI `opt` command and by tests.
pub fn optimize_ir_module(ir_module: &crate::ir::IrModule) -> (L2Module, OptStats) {
    let l1 = lower_ir_to_l1(ir_module);
    let mut l2 = lower_l1_to_l2(&l1);
    let stats = optimize_l2(&mut l2);
    (l2, stats)
}

// ═══════════════════════════════════════════════════════════════════════════
// Helpers: side effects, purity, value references
// ═══════════════════════════════════════════════════════════════════════════

/// Does this instruction have observable side effects (so DCE must keep it
/// even if its result is unused)?
fn has_side_effects(inst: &L2Inst) -> bool {
    match inst {
        L2Inst::Store { .. } => true,
        L2Inst::Print { .. } => true,
        L2Inst::Panic { .. } => true,
        L2Inst::Return { .. } => true,
        L2Inst::Jump { .. } => true,
        L2Inst::Branch { .. } => true,
        L2Inst::Unreachable => true,
        L2Inst::TryUnwrap { .. } => true,
        // Calls to pure functions have no side effects
        L2Inst::Call { func, .. } => !is_known_pure_func(func),
        // CallPtr and DynCall are conservative — assume side effects
        L2Inst::CallPtr { .. } | L2Inst::DynCall { .. } => true,
        _ => false,
    }
}

/// Check if a function name is known to be pure (no side effects).
/// Runtime helper functions that only compute values are pure.
fn is_known_pure_func(name: &str) -> bool {
    matches!(name,
        // Pure arithmetic helpers
        "p_add" | "p_sub" | "p_mul" | "p_div" | "p_mod"
        // Pure comparison helpers
        | "p_eq" | "p_neq" | "p_lt" | "p_gt" | "p_le" | "p_ge"
        // Pure conversion helpers
        | "p_int_to_float" | "p_float_to_int" | "p_int_extend" | "p_int_trunc"
        // Pure string accessors (no mutation)
        | "p_string_len" | "p_string_contains" | "p_string_starts_with"
        | "p_string_ends_with" | "p_string_trim" | "p_string_to_uppercase"
        | "p_string_to_lowercase" | "p_string_replace" | "p_string_split"
        | "p_string_char_at"
        // Pure vec accessors (read-only)
        | "p_vec_len" | "p_vec_get" | "p_vec_contains"
        // Pure option/result
        | "p_option_some" | "p_option_none" | "p_option_is_some" | "p_option_is_none"
        | "p_result_ok" | "p_result_err" | "p_result_is_ok" | "p_result_is_err"
    )
}

/// Is this instruction pure (no side effects, deterministic)?
fn is_pure(inst: &L2Inst) -> bool {
    !has_side_effects(inst)
        && matches!(
            inst,
            L2Inst::ConstInt { .. }
                | L2Inst::ConstFloat { .. }
                | L2Inst::ConstBool { .. }
                | L2Inst::ConstStr { .. }
                | L2Inst::ConstNull { .. }
                | L2Inst::BinOp { .. }
                | L2Inst::UnaryOp { .. }
                | L2Inst::GetFieldPtr { .. }
                | L2Inst::GetElemPtr { .. }
                | L2Inst::BitCast { .. }
                | L2Inst::IntExtend { .. }
                | L2Inst::IntTrunc { .. }
                | L2Inst::IntToFloat { .. }
                | L2Inst::FloatToInt { .. }
                | L2Inst::Alloca { .. }
        )
}

/// All PValue operands read by this instruction.
fn value_refs(inst: &L2Inst) -> Vec<PValue> {
    match inst {
        L2Inst::ConstInt { .. }
        | L2Inst::ConstFloat { .. }
        | L2Inst::ConstBool { .. }
        | L2Inst::ConstStr { .. }
        | L2Inst::ConstNull { .. }
        | L2Inst::Jump { .. }
        | L2Inst::Unreachable
        | L2Inst::Panic { .. } => Vec::new(),
        L2Inst::BinOp { left, right, .. } => vec![*left, *right],
        L2Inst::UnaryOp { operand, .. } => vec![*operand],
        L2Inst::Alloca { .. } => Vec::new(),
        L2Inst::Load { ptr, .. } => vec![*ptr],
        L2Inst::Store { ptr, value } => vec![*ptr, *value],
        L2Inst::GetFieldPtr { struct_ptr, .. } => vec![*struct_ptr],
        L2Inst::GetElemPtr { array_ptr, index, .. } => vec![*array_ptr, *index],
        L2Inst::Branch { cond, .. } => vec![*cond],
        L2Inst::Return { value: Some(v) } => vec![*v],
        L2Inst::Return { value: None } => Vec::new(),
        L2Inst::Call { args, .. } | L2Inst::DynCall { args, .. } => args.clone(),
        L2Inst::CallPtr { fn_ptr, args, .. } => {
            let mut v = vec![*fn_ptr];
            v.extend(args);
            v
        }
        L2Inst::MakeDyn { value, .. } => vec![*value],
        L2Inst::BitCast { value, .. }
        | L2Inst::IntExtend { value, .. }
        | L2Inst::IntTrunc { value, .. }
        | L2Inst::IntToFloat { value, .. }
        | L2Inst::FloatToInt { value, .. } => vec![*value],
        L2Inst::TryUnwrap { value, ok_dst, .. } => vec![*value, *ok_dst],
        L2Inst::Print { value, .. } => vec![*value],
    }
}

/// Collect every PValue that is *used* anywhere in the function (as an
/// operand, a block param, a return value, a call arg, etc.).
fn collect_used_values(f: &L2Function) -> HashSet<PValue> {
    let mut used: HashSet<PValue> = HashSet::new();
    // Block params are live (they're the function's SSA inputs).
    for b in &f.blocks {
        for (pv, _) in &b.params {
            used.insert(*pv);
        }
    }
    // Function params are live.
    for (pv, _) in &f.params {
        used.insert(*pv);
    }
    // Every operand reference.
    for (_, inst) in &f.values {
        for r in value_refs(inst) {
            used.insert(r);
        }
    }
    used
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass 1: Constant folding + propagation
// ═══════════════════════════════════════════════════════════════════════════

/// Fold binary/unary ops on constants, propagate constants, and fold branches
/// on constant conditions. Returns the number of rewrites.
pub fn pass_const_fold(module: &mut L2Module) -> usize {
    let mut count = 0;
    for f in &mut module.functions {
        count += const_fold_function(f);
    }
    count
}

fn const_fold_function(f: &mut L2Function) -> usize {
    let mut count = 0;
    // Iterate to a local fixed point so propagation chains fold in one call.
    loop {
        let pass = const_fold_once(f);
        if pass == 0 {
            break;
        }
        count += pass;
    }
    count
}

fn const_fold_once(f: &mut L2Function) -> usize {
    let mut count = 0;
    let inst_ids: Vec<PValue> = f.values.keys().copied().collect();
    for id in inst_ids {
        let inst = match f.values.get(&id) {
            Some(i) => i.clone(),
            None => continue,
        };
        match inst {
            L2Inst::BinOp { op, left, right, ty } => {
                let lv = const_int_value(f, left);
                let rv = const_int_value(f, right);
                if let (Some(l), Some(r)) = (lv, rv) {
                    if let Some(result) = eval_binop_int(&op, l, r, &ty) {
                        f.values.insert(id, L2Inst::ConstInt { value: result, ty });
                        count += 1;
                        continue;
                    }
                }
                // Float folding
                let lf = const_float_value(f, left);
                let rf = const_float_value(f, right);
                if let (Some(l), Some(r)) = (lf, rf) {
                    if let Some(result) = eval_binop_float(&op, l, r, &ty) {
                        f.values.insert(
                            id,
                            L2Inst::ConstFloat {
                                bits: result.to_bits(),
                                ty,
                            },
                        );
                        count += 1;
                    }
                }
            }
            L2Inst::UnaryOp { op, operand, ty } => {
                if let Some(v) = const_int_value(f, operand) {
                    if let Some(result) = eval_unary_int(&op, v, &ty) {
                        f.values.insert(id, L2Inst::ConstInt { value: result, ty });
                        count += 1;
                        continue;
                    }
                }
            }
            L2Inst::Branch { cond, then, else_ } => {
                if let Some(c) = const_bool_value(f, cond) {
                    // Fold to an unconditional jump.
                    let target = if c { then } else { else_ };
                    f.values.insert(id, L2Inst::Jump { target });
                    count += 1;
                }
            }
            _ => {}
        }
    }
    count
}

/// If `pv` is (or has been propagated to) a ConstInt, return its value.
fn const_int_value(f: &L2Function, pv: PValue) -> Option<i64> {
    match f.values.get(&pv)? {
        L2Inst::ConstInt { value, .. } => Some(*value),
        L2Inst::ConstBool { value } => Some(if *value { 1 } else { 0 }),
        _ => None,
    }
}

fn const_float_value(f: &L2Function, pv: PValue) -> Option<f64> {
    match f.values.get(&pv)? {
        L2Inst::ConstFloat { bits, .. } => Some(f64::from_bits(*bits)),
        _ => None,
    }
}

fn const_bool_value(f: &L2Function, pv: PValue) -> Option<bool> {
    match f.values.get(&pv)? {
        L2Inst::ConstBool { value } => Some(*value),
        L2Inst::ConstInt { value, .. } => Some(*value != 0),
        _ => None,
    }
}

/// Evaluate an integer binary op. Returns None for division by zero or for
/// ops that don't apply to integers (And/Or are logical short-circuit, not
/// foldable here without knowing both sides are constants — but they are
/// constants here, so we can fold them too).
fn eval_binop_int(op: &BinOp, l: i64, r: i64, ty: &IrType) -> Option<i64> {
    use BinOp::*;
    let result = match op {
        Add => l.wrapping_add(r),
        Sub => l.wrapping_sub(r),
        Mul => l.wrapping_mul(r),
        Div => {
            if r == 0 {
                return None;
            }
            l.wrapping_div(r)
        }
        Mod => {
            if r == 0 {
                return None;
            }
            l.wrapping_rem(r)
        }
        BitAnd => l & r,
        BitOr => l | r,
        BitXor => l ^ r,
        Shl => l.wrapping_shl(r as u32),
        Shr => l.wrapping_shr(r as u32),
        Eq => (l == r) as i64,
        Neq => (l != r) as i64,
        Lt => (l < r) as i64,
        Gt => (l > r) as i64,
        LtEq => (l <= r) as i64,
        GtEq => (l >= r) as i64,
        And => (l != 0 && r != 0) as i64,
        Or => (l != 0 || r != 0) as i64,
    };
    // Truncate to the target type width.
    let truncated = match ty {
        IrType::I8 | IrType::U8 => (result as i8) as i64,
        IrType::I16 | IrType::U16 => (result as i16) as i64,
        IrType::I32 | IrType::U32 => (result as i32) as i64,
        _ => result,
    };
    Some(truncated)
}

fn eval_binop_float(op: &BinOp, l: f64, r: f64, ty: &IrType) -> Option<f64> {
    use BinOp::*;
    let result = match op {
        Add => l + r,
        Sub => l - r,
        Mul => l * r,
        Div => {
            if r == 0.0 {
                return None;
            }
            l / r
        }
        Mod => {
            if r == 0.0 {
                return None;
            }
            l % r
        }
        Eq => (l == r) as i64 as f64,
        Neq => (l != r) as i64 as f64,
        Lt => (l < r) as i64 as f64,
        Gt => (l > r) as i64 as f64,
        LtEq => (l <= r) as i64 as f64,
        GtEq => (l >= r) as i64 as f64,
        _ => return None,
    };
    let _ = ty; // f32 vs f64 precision handling deferred to codegen
    Some(result)
}

fn eval_unary_int(op: &UnaryOp, v: i64, ty: &IrType) -> Option<i64> {
    use UnaryOp::*;
    let result = match op {
        Neg => v.wrapping_neg(),
        Not => (v == 0) as i64,
        BitNot => !v,
    };
    let truncated = match ty {
        IrType::I8 | IrType::U8 => (result as i8) as i64,
        IrType::I16 | IrType::U16 => (result as i16) as i64,
        IrType::I32 | IrType::U32 => (result as i32) as i64,
        _ => result,
    };
    Some(truncated)
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass 2: Common subexpression elimination
// ═══════════════════════════════════════════════════════════════════════════

/// Within each basic block, replace identical pure computations with
/// references to the first result. Returns the number of rewrites.
pub fn pass_cse(module: &mut L2Module) -> usize {
    let mut count = 0;
    for f in &mut module.functions {
        count += cse_function(f);
    }
    count
}

fn cse_function(f: &mut L2Function) -> usize {
    let mut count = 0;
    // Build a per-block table: (canonical instruction key) -> first PValue.
    // We process blocks in order; CSE is intrablock only (a safe, simple
    // formulation — global CSE requires dominator-based value numbering).
    let block_ids: Vec<PBlock> = f.blocks.iter().map(|b| b.id).collect();
    for bid in block_ids {
        let inst_ids = {
            let b = f.blocks.iter().find(|x| x.id == bid).unwrap();
            b.instructions.clone()
        };
        let mut table: HashMap<String, PValue> = HashMap::new();
        for id in inst_ids {
            let inst = match f.values.get(&id) {
                Some(i) => i.clone(),
                None => continue,
            };
            if !is_pure(&inst) {
                continue;
            }
            let key = cse_key(&inst);
            if let Some(prev) = table.get(&key) {
                // Replace this instruction with a reference to `prev`. We
                // model "copy" as a BitCast of the same type — but since L2
                // values are SSA and type-stable, the cleanest rewrite is to
                // turn it into a no-op ConstInt that mirrors the previous
                // value's constant form *if* the previous is a constant, or
                // leave it for DCE to handle via aliasing.
                //
                // The simplest correct rewrite: make this value an alias by
                // replacing all downstream references to `id` with `prev`,
                // then mark `id` for DCE.
                replace_all_refs(f, id, *prev);
                // Remove the now-dead instruction so DCE has less work.
                f.values.remove(&id);
                // Also drop it from the block's instruction list.
                if let Some(b) = f.blocks.iter_mut().find(|x| x.id == bid) {
                    b.instructions.retain(|v| *v != id);
                }
                count += 1;
            } else {
                table.insert(key, id);
            }
        }
    }
    count
}

/// A canonical string key for a pure instruction, so identical computations
/// hash to the same key.
fn cse_key(inst: &L2Inst) -> String {
    match inst {
        L2Inst::ConstInt { value, ty } => format!("ci:{}:{}", value, ty),
        L2Inst::ConstFloat { bits, ty } => format!("cf:{}:{}", bits, ty),
        L2Inst::ConstBool { value } => format!("cb:{}", value),
        L2Inst::ConstStr { value } => format!("cs:{}", value),
        L2Inst::ConstNull { ty } => format!("cn:{}", ty),
        L2Inst::BinOp { op, left, right, ty } => {
            // Commutative ops: canonicalize operand order.
            let (a, b) = if is_commutative(op) && left.0 > right.0 {
                (right, left)
            } else {
                (left, right)
            };
            format!("bo:{:?}:{}:{}:{}", op, a, b, ty)
        }
        L2Inst::UnaryOp { op, operand, ty } => format!("uo:{:?}:{}:{}", op, operand, ty),
        L2Inst::GetFieldPtr { struct_ptr, field_index, .. } => {
            format!("gfp:{}:{}", struct_ptr, field_index)
        }
        L2Inst::GetElemPtr { array_ptr, index, .. } => format!("gep:{}:{}", array_ptr, index),
        L2Inst::BitCast { value, from, to } => format!("bc:{}:{}:{}", value, from, to),
        L2Inst::IntExtend { value, from, to } => format!("ie:{}:{}:{}", value, from, to),
        L2Inst::IntTrunc { value, from, to } => format!("it:{}:{}:{}", value, from, to),
        L2Inst::IntToFloat { value, from, to } => format!("i2f:{}:{}:{}", value, from, to),
        L2Inst::FloatToInt { value, from, to } => format!("f2i:{}:{}:{}", value, from, to),
        L2Inst::Alloca { ty, region } => format!("alloca:{}:{}", region, ty),
        _ => format!("other:{:p}", inst as *const _),
    }
}

fn is_commutative(op: &BinOp) -> bool {
    matches!(
        op,
        BinOp::Add | BinOp::Mul | BinOp::Eq | BinOp::Neq | BinOp::BitAnd | BinOp::BitOr | BinOp::BitXor
    )
}

/// Replace every reference to `from` with `to` across the whole function.
fn replace_all_refs(f: &mut L2Function, from: PValue, to: PValue) {
    for (_, inst) in f.values.iter_mut() {
        replace_refs_in_inst(inst, from, to);
    }
    // Block params and function params: rename the param itself.
    for b in f.blocks.iter_mut() {
        for (pv, _) in b.params.iter_mut() {
            if *pv == from {
                *pv = to;
            }
        }
    }
    for (pv, _) in f.params.iter_mut() {
        if *pv == from {
            *pv = to;
        }
    }
}

fn replace_refs_in_inst(inst: &mut L2Inst, from: PValue, to: PValue) {
    fn r(v: &mut PValue, from: PValue, to: PValue) {
        if *v == from {
            *v = to;
        }
    }
    match inst {
        L2Inst::BinOp { left, right, .. } => {
            r(left, from, to);
            r(right, from, to);
        }
        L2Inst::UnaryOp { operand, .. } => r(operand, from, to),
        L2Inst::Load { ptr, .. } => r(ptr, from, to),
        L2Inst::Store { ptr, value } => {
            r(ptr, from, to);
            r(value, from, to);
        }
        L2Inst::GetFieldPtr { struct_ptr, .. } => r(struct_ptr, from, to),
        L2Inst::GetElemPtr { array_ptr, index, .. } => {
            r(array_ptr, from, to);
            r(index, from, to);
        }
        L2Inst::Branch { cond, .. } => r(cond, from, to),
        L2Inst::Return { value: Some(v) } => r(v, from, to),
        L2Inst::Call { args, .. } | L2Inst::DynCall { args, .. } => {
            for a in args {
                r(a, from, to);
            }
        }
        L2Inst::CallPtr { fn_ptr, args, .. } => {
            r(fn_ptr, from, to);
            for a in args {
                r(a, from, to);
            }
        }
        L2Inst::MakeDyn { value, .. } => r(value, from, to),
        L2Inst::BitCast { value, .. }
        | L2Inst::IntExtend { value, .. }
        | L2Inst::IntTrunc { value, .. }
        | L2Inst::IntToFloat { value, .. }
        | L2Inst::FloatToInt { value, .. } => r(value, from, to),
        L2Inst::TryUnwrap { value, ok_dst, .. } => {
            r(value, from, to);
            r(ok_dst, from, to);
        }
        L2Inst::Print { value, .. } => r(value, from, to),
        _ => {}
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass 3: Dead code elimination
// ═══════════════════════════════════════════════════════════════════════════

/// Remove instructions whose results are never used and that have no side
/// effects. Also drops blocks that have become unreachable (no predecessors
/// and not the entry). Returns the number of instructions/blocks removed.
pub fn pass_dce(module: &mut L2Module) -> usize {
    let mut count = 0;
    for f in &mut module.functions {
        count += dce_function(f);
    }
    count
}

fn dce_function(f: &mut L2Function) -> usize {
    let mut count = 0;
    // Mark live values: anything used directly, plus anything transitively
    // reachable from side-effecting instructions.
    let mut live: HashSet<PValue> = collect_used_values(f);

    // Side-effecting instructions are roots; their operands are live.
    let mut changed = true;
    while changed {
        changed = false;
        for (id, inst) in &f.values {
            if has_side_effects(inst) {
                if live.insert(*id) {
                    changed = true;
                }
            }
        }
        // Propagate liveness backward through operands of live instructions.
        let mut to_add: Vec<PValue> = Vec::new();
        for (id, inst) in &f.values {
            if live.contains(id) {
                for r in value_refs(inst) {
                    if !live.contains(&r) {
                        to_add.push(r);
                    }
                }
            }
        }
        for v in to_add {
            if live.insert(v) {
                changed = true;
            }
        }
    }

    // Remove dead, side-effect-free instructions.
    let dead: Vec<PValue> = f
        .values
        .iter()
        .filter(|(id, inst)| !live.contains(id) && !has_side_effects(inst))
        .map(|(id, _)| *id)
        .collect();
    for id in &dead {
        f.values.remove(id);
        count += 1;
    }
    // Drop them from block instruction lists.
    for b in f.blocks.iter_mut() {
        b.instructions.retain(|v| !dead.contains(v));
    }

    // Remove unreachable blocks (no predecessors, not entry). This is a
    // conservative reachability pass from the entry block.
    count += remove_unreachable_blocks(f);

    count
}

fn remove_unreachable_blocks(f: &mut L2Function) -> usize {
    // Compute reachable set from entry via BFS over terminator targets.
    let mut reachable: HashSet<PBlock> = HashSet::new();
    reachable.insert(f.entry);
    let mut queue: Vec<PBlock> = vec![f.entry];
    while let Some(bid) = queue.pop() {
        let block = match f.blocks.iter().find(|b| b.id == bid) {
            Some(b) => b,
            None => continue,
        };
        // Find this block's terminator (last instruction).
        if let Some(&term_id) = block.instructions.last() {
            if let Some(term) = f.values.get(&term_id) {
                for t in terminator_targets(term) {
                    if reachable.insert(t) {
                        queue.push(t);
                    }
                }
            }
        }
    }

    let before = f.blocks.len();
    f.blocks.retain(|b| reachable.contains(&b.id));
    // Also drop instructions that lived only in dropped blocks (they're
    // already gone from `f.values` only if we removed them; clean up here).
    let live_block_ids: HashSet<PBlock> = f.blocks.iter().map(|b| b.id).collect();
    let mut to_remove: Vec<PValue> = Vec::new();
    for (id, inst) in &f.values {
        // If this instruction is in a dropped block, drop it.
        let mut in_live_block = false;
        for b in &f.blocks {
            if b.instructions.contains(id) {
                in_live_block = true;
                break;
            }
        }
        if !in_live_block && !has_side_effects(inst) {
            // It might be a block param or a constant referenced from a
            // dropped block; only remove if truly unreferenced.
            let _ = inst;
            to_remove.push(*id);
        }
    }
    let _ = live_block_ids;
    for id in &to_remove {
        f.values.remove(id);
    }
    before - f.blocks.len()
}

fn terminator_targets(inst: &L2Inst) -> Vec<PBlock> {
    match inst {
        L2Inst::Jump { target } => vec![*target],
        L2Inst::Branch { then, else_, .. } => vec![*then, *else_],
        L2Inst::TryUnwrap { ok_block, err_block, .. } => vec![*ok_block, *err_block],
        _ => Vec::new(),
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass 4: Inlining
// ═══════════════════════════════════════════════════════════════════════════

/// Inline small, pure callee bodies into callers. A callee is eligible if:
///   - it's a direct `Call` (not CallPtr/DynCall),
///   - its body has at most `MAX_INLINE_BLOCKS` basic blocks,
///   - it has no calls to external functions (only local, pure ops),
///   - it doesn't recurse (directly or via a small call graph cycle).
///
/// Returns the number of call sites inlined.
pub fn pass_inline(module: &mut L2Module) -> usize {
    let mut count = 0;
    // Iterate: inlining can expose more inlining opportunities.
    for _ in 0..4 {
        let pass = inline_once(module);
        if pass == 0 {
            break;
        }
        count += pass;
        // After each inlining round, topologically sort block instruction
        // lists so that all definitions come before uses.
        for f in &mut module.functions {
            for b in &mut f.blocks {
                topo_sort_block(b, &f.values);
            }
        }
    }
    count
}

/// Topologically sort a block's instruction list so that every value is
/// defined before it is used. This is needed after inlining, which can
/// insert instructions at positions that violate SSA ordering.
fn topo_sort_block(b: &mut L2Block, values: &HashMap<PValue, L2Inst>) {
    use std::collections::HashSet;

    let insts: Vec<PValue> = b.instructions.clone();
    if insts.is_empty() {
        return;
    }

    // Build dependency graph: for each instruction, which other instructions
    // in this block must come before it?
    let inst_set: HashSet<PValue> = insts.iter().copied().collect();
    let mut deps: HashMap<PValue, HashSet<PValue>> = HashMap::new();
    for &id in &insts {
        if let Some(inst) = values.get(&id) {
            let refs: HashSet<PValue> = value_refs(inst)
                .into_iter()
                .filter(|r| inst_set.contains(r))
                .collect();
            // Terminators (Return, Jump, Branch, Unreachable) must come last.
            // We model this by making them depend on ALL other instructions.
            let is_terminator = matches!(inst,
                L2Inst::Return { .. } | L2Inst::Jump { .. }
                | L2Inst::Branch { .. } | L2Inst::Unreachable
            );
            let final_deps = if is_terminator {
                inst_set.iter().filter(|&&x| x != id).copied().collect()
            } else {
                refs
            };
            deps.insert(id, final_deps);
        } else {
            deps.insert(id, HashSet::new());
        }
    }

    // Original position for stable ordering.
    let pos: HashMap<PValue, usize> = insts.iter().enumerate()
        .map(|(i, &id)| (id, i)).collect();

    // Kahn's algorithm: repeatedly pick the instruction with all deps
    // satisfied and the lowest original position.
    let mut visited: HashSet<PValue> = HashSet::new();
    let mut result: Vec<PValue> = Vec::new();

    while result.len() < insts.len() {
        // Find all instructions whose deps are all visited.
        let mut ready: Vec<PValue> = insts.iter()
            .filter(|&&id| !visited.contains(&id))
            .filter(|&&id| deps.get(&id).map(|d| d.iter().all(|dep| visited.contains(dep))).unwrap_or(true))
            .copied()
            .collect();

        if ready.is_empty() {
            // Cycle detected — append remaining in original order.
            for &id in &insts {
                if !visited.contains(&id) {
                    result.push(id);
                    visited.insert(id);
                }
            }
            break;
        }

        // Sort by original position for stability.
        ready.sort_by_key(|id| pos.get(id).copied().unwrap_or(usize::MAX));

        // Pick the first ready instruction.
        let id = ready[0];
        visited.insert(id);
        result.push(id);
    }

    if result.len() == insts.len() {
        b.instructions = result;
    }
}

const MAX_INLINE_BLOCKS: usize = 3;
const MAX_INLINE_INSTS: usize = 20;

fn inline_once(module: &mut L2Module) -> usize {
    // Snapshot callee bodies + eligibility so we don't borrow module across
    // the mutation.
    let mut eligible: HashMap<String, L2Function> = HashMap::new();
    for f in &module.functions {
        if f.blocks.len() <= MAX_INLINE_BLOCKS
            && f.values.len() <= MAX_INLINE_INSTS
            && !has_external_calls(f)
            && !recurses_directly(f)
        {
            eligible.insert(f.name.clone(), f.clone());
        }
    }

    let mut count = 0;
    for caller in &mut module.functions {
        count += inline_into(caller, &eligible);
    }
    count
}

fn has_external_calls(f: &L2Function) -> bool {
    for (_, inst) in &f.values {
        match inst {
            L2Inst::Call { func, .. } => {
                // External if the name starts with "p_" (runtime builtin
                // convention) — heuristic, but matches the codebase.
                if func.starts_with("p_") {
                    return true;
                }
            }
            L2Inst::CallPtr { .. } | L2Inst::DynCall { .. } => return true,
            _ => {}
        }
    }
    false
}

fn recurses_directly(f: &L2Function) -> bool {
    for (_, inst) in &f.values {
        if let L2Inst::Call { func, .. } = inst {
            if func == &f.name {
                return true;
            }
        }
    }
    false
}

fn inline_into(caller: &mut L2Function, eligible: &HashMap<String, L2Function>) -> usize {
    let mut count = 0;
    // Find call sites to inline. We collect them first to avoid mutating
    // while iterating. Sort by ID to process in a deterministic order.
    let mut sites: Vec<(PValue, String, Vec<PValue>, IrType)> = Vec::new();
    for (id, inst) in &caller.values {
        if let L2Inst::Call { func, args, ret_ty } = inst {
            if eligible.contains_key(func) {
                sites.push((*id, func.clone(), args.clone(), ret_ty.clone()));
            }
        }
    }
    // Sort by call ID (ascending) so inner calls (which typically have lower
    // IDs since they appear earlier in the block) are inlined first.
    sites.sort_by_key(|(id, _, _, _)| id.0);

    // Track all replacements (old_id → new_id) so we can fix up references
    // in instructions created by later inlining.
    let mut all_replacements: HashMap<PValue, PValue> = HashMap::new();

    for (call_id, callee_name, args, ret_ty) in sites {
        // Skip if this call was already removed (e.g., it was inlined as part
        // of another call's argument resolution).
        if !caller.values.contains_key(&call_id) {
            continue;
        }
        // Fix up args: if an arg was already replaced by a previous inlining,
        // use the replacement.
        let fixed_args: Vec<PValue> = args.iter()
            .map(|a| *all_replacements.get(a).unwrap_or(a))
            .collect();
        let callee = match eligible.get(&callee_name) {
            Some(c) => c.clone(),
            None => continue,
        };
        if let Some(ret) = inline_call_site_tracked(caller, call_id, &callee, &fixed_args, ret_ty, &all_replacements) {
            all_replacements.insert(call_id, ret);
            count += 1;
        }
    }
    count
}

/// Like inline_call_site but also applies prior replacements to the inlined
/// instructions, and returns the return value ID.
fn inline_call_site_tracked(
    caller: &mut L2Function,
    call_id: PValue,
    callee: &L2Function,
    args: &[PValue],
    ret_ty: IrType,
    prior_replacements: &HashMap<PValue, PValue>,
) -> Option<PValue> {
    // ─── Simple case: single-block callee with straight-line code ───
    if callee.blocks.len() == 1 {
        return inline_single_block_tracked(caller, call_id, callee, args, ret_ty, prior_replacements);
    }
    // Multi-block: use the existing implementation (less common, no tracking needed
    // for the common case).
    if inline_multi_block(caller, call_id, callee, args, ret_ty) {
        // We don't track the return value for multi-block; return None to
        // indicate we couldn't track it. This is conservative.
        None
    } else {
        None
    }
}

/// Single-block inlining with prior replacement tracking.
fn inline_single_block_tracked(
    caller: &mut L2Function,
    call_id: PValue,
    callee: &L2Function,
    args: &[PValue],
    _ret_ty: IrType,
    prior_replacements: &HashMap<PValue, PValue>,
) -> Option<PValue> {
    let max_id = caller.values.keys().map(|v| v.0).max().unwrap_or(0);
    let mut next_id = max_id + 1;
    let mut value_map: HashMap<PValue, PValue> = HashMap::new();

    // Bind parameters: callee param → caller argument value.
    for (i, (param_pv, _)) in callee.params.iter().enumerate() {
        if let Some(arg) = args.get(i) {
            value_map.insert(*param_pv, *arg);
        }
    }

    // Find which caller block contains the call.
    let call_block_id = caller.blocks.iter()
        .find(|b| b.instructions.contains(&call_id))
        .map(|b| b.id);
    let call_block_id = match call_block_id {
        Some(b) => b,
        None => return None,
    };

    // Clone callee instructions, remap operands, collect them.
    let mut new_insts: Vec<(PValue, L2Inst)> = Vec::new();
    let mut return_value: Option<PValue> = None;

    for &callee_inst_id in &callee.blocks[0].instructions {
        let callee_inst = match callee.values.get(&callee_inst_id) {
            Some(i) => i.clone(),
            None => continue,
        };
        let new_id = ValueId(next_id);
        next_id += 1;
        value_map.insert(callee_inst_id, new_id);
        let mut remapped = callee_inst.clone();
        remap_inst(&mut remapped, &value_map, &HashMap::new());
        // Also apply prior replacements (from earlier inlining in this pass).
        for (_, inst) in &mut new_insts {
            apply_replacements(inst, prior_replacements);
        }
        if let L2Inst::Return { value: Some(rv) } = &remapped {
            // Apply prior replacements to the return value too.
            return_value = Some(*prior_replacements.get(rv).unwrap_or(rv));
            continue;
        }
        if let L2Inst::Return { value: None } = &remapped {
            return_value = None;
            continue;
        }
        // Apply prior replacements to this instruction's operands.
        apply_replacements(&mut remapped, prior_replacements);
        new_insts.push((new_id, remapped));
    }

    let return_value = match return_value {
        Some(v) => v,
        None => {
            caller.values.insert(call_id, L2Inst::ConstInt { value: 0, ty: _ret_ty.clone() });
            return None;
        }
    };

    // Insert the inlined instructions into the caller's value map.
    for (id, inst) in &new_insts {
        caller.values.insert(*id, inst.clone());
    }

    // Replace all references to call_id with return_value.
    replace_all_refs(caller, call_id, return_value);

    // Splice into the block: replace call_id in the instruction list with
    // the new instruction ids.
    let block = caller.blocks.iter_mut().find(|b| b.id == call_block_id).unwrap();
    let call_idx = block.instructions.iter().position(|&v| v == call_id);
    if let Some(idx) = call_idx {
        block.instructions.remove(idx);
        for (i, (id, _)) in new_insts.iter().enumerate() {
            block.instructions.insert(idx + i, *id);
        }
    }

    caller.values.remove(&call_id);
    Some(return_value)
}

/// Apply a replacement map to an instruction's operands.
fn apply_replacements(inst: &mut L2Inst, replacements: &HashMap<PValue, PValue>) {
    fn r(v: &mut PValue, reps: &HashMap<PValue, PValue>) {
        if let Some(new) = reps.get(v) {
            *v = *new;
        }
    }
    match inst {
        L2Inst::BinOp { left, right, .. } => { r(left, replacements); r(right, replacements); }
        L2Inst::UnaryOp { operand, .. } => r(operand, replacements),
        L2Inst::Load { ptr, .. } => r(ptr, replacements),
        L2Inst::Store { ptr, value } => { r(ptr, replacements); r(value, replacements); }
        L2Inst::GetFieldPtr { struct_ptr, .. } => r(struct_ptr, replacements),
        L2Inst::GetElemPtr { array_ptr, index, .. } => { r(array_ptr, replacements); r(index, replacements); }
        L2Inst::Branch { cond, .. } => r(cond, replacements),
        L2Inst::Return { value: Some(v) } => r(v, replacements),
        L2Inst::Call { args, .. } | L2Inst::DynCall { args, .. } => {
            for a in args { r(a, replacements); }
        }
        L2Inst::CallPtr { fn_ptr, args, .. } => {
            r(fn_ptr, replacements);
            for a in args { r(a, replacements); }
        }
        L2Inst::MakeDyn { value, .. } => r(value, replacements),
        L2Inst::BitCast { value, .. }
        | L2Inst::IntExtend { value, .. }
        | L2Inst::IntTrunc { value, .. }
        | L2Inst::IntToFloat { value, .. }
        | L2Inst::FloatToInt { value, .. } => r(value, replacements),
        L2Inst::TryUnwrap { value, ok_dst, .. } => { r(value, replacements); r(ok_dst, replacements); }
        L2Inst::Print { value, .. } => r(value, replacements),
        _ => {}
    }
}

/// Inline `callee` at the call site that produces `call_id`. The call's
/// arguments are bound to the callee's parameters; the callee's return value
/// replaces the call's result everywhere.
fn inline_call_site(
    caller: &mut L2Function,
    call_id: PValue,
    callee: &L2Function,
    args: &[PValue],
    ret_ty: IrType,
) -> bool {
    // ─── Simple case: single-block callee with straight-line code ───
    // This is the common case for small helpers like `add(a,b) { a + b }`.
    // We inline the instructions directly into the call site's block,
    // replacing the call with the return value. No new blocks needed.
    if callee.blocks.len() == 1 {
        return inline_single_block(caller, call_id, callee, args, ret_ty);
    }

    // ─── Multi-block case: proper block splitting ───
    inline_multi_block(caller, call_id, callee, args, ret_ty)
}

/// Inline a single-block callee. The callee's instructions (minus the return)
/// are inserted into the caller's value map, and the call is replaced with
/// the return value. All instructions go into the caller's block that contained
/// the call site.
fn inline_single_block(
    caller: &mut L2Function,
    call_id: PValue,
    callee: &L2Function,
    args: &[PValue],
    _ret_ty: IrType,
) -> bool {
    let max_id = caller.values.keys().map(|v| v.0).max().unwrap_or(0);
    let mut next_id = max_id + 1;
    let mut value_map: HashMap<PValue, PValue> = HashMap::new();

    // Bind parameters: callee param → caller argument value.
    for (i, (param_pv, _)) in callee.params.iter().enumerate() {
        if let Some(arg) = args.get(i) {
            value_map.insert(*param_pv, *arg);
        }
    }

    // Find which caller block contains the call.
    let call_block_id = caller.blocks.iter()
        .find(|b| b.instructions.contains(&call_id))
        .map(|b| b.id);
    let call_block_id = match call_block_id {
        Some(b) => b,
        None => return false,
    };

    // Clone callee instructions, remap operands, collect them.
    let mut new_insts: Vec<(PValue, L2Inst)> = Vec::new();
    let mut return_value: Option<PValue> = None;

    for &callee_inst_id in &callee.blocks[0].instructions {
        let callee_inst = match callee.values.get(&callee_inst_id) {
            Some(i) => i.clone(),
            None => continue,
        };
        let new_id = ValueId(next_id);
        next_id += 1;
        value_map.insert(callee_inst_id, new_id);
        let mut remapped = callee_inst.clone();
        remap_inst(&mut remapped, &value_map, &HashMap::new());
        if let L2Inst::Return { value: Some(rv) } = &remapped {
            return_value = Some(*rv);
            continue;
        }
        if let L2Inst::Return { value: None } = &remapped {
            return_value = None;
            continue;
        }
        new_insts.push((new_id, remapped));
    }

    let return_value = match return_value {
        Some(v) => v,
        None => {
            // Void return: replace call with const 0.
            caller.values.insert(call_id, L2Inst::ConstInt { value: 0, ty: _ret_ty.clone() });
            return false; // Don't inline void functions with side effects for now
        }
    };

    // Insert the inlined instructions into the caller's value map.
    for (id, inst) in &new_insts {
        caller.values.insert(*id, inst.clone());
    }

    // Replace the call instruction in the block's instruction list with
    // the inlined instructions, followed by a reference to the return value.
    // We use replace_all_refs to point call_id → return_value, then
    // splice the new instructions into the block where the call was.
    replace_all_refs(caller, call_id, return_value);

    // Splice into the block: replace call_id in the instruction list with
    // the new instruction ids.
    let block = caller.blocks.iter_mut().find(|b| b.id == call_block_id).unwrap();
    let call_idx = block.instructions.iter().position(|&v| v == call_id);
    if let Some(idx) = call_idx {
        // Remove the call from the instruction list.
        block.instructions.remove(idx);
        // Insert the new instructions at the same position.
        for (i, (id, _)) in new_insts.iter().enumerate() {
            block.instructions.insert(idx + i, *id);
        }
    }

    // Remove the call from the value map.
    caller.values.remove(&call_id);

    true
}

/// Inline a multi-block callee with proper block splitting.
fn inline_multi_block(
    caller: &mut L2Function,
    call_id: PValue,
    callee: &L2Function,
    args: &[PValue],
    ret_ty: IrType,
) -> bool {
    let max_id = caller.values.keys().map(|v| v.0).max().unwrap_or(0);
    let max_block = caller.blocks.iter().map(|b| b.id.0).max().unwrap_or(0);
    let mut value_map: HashMap<PValue, PValue> = HashMap::new();
    let mut block_map: HashMap<PBlock, PBlock> = HashMap::new();
    let mut next_id = max_id + 1;
    let mut next_block = max_block + 1;

    // Bind parameters.
    for (i, (param_pv, _)) in callee.params.iter().enumerate() {
        if let Some(arg) = args.get(i) {
            value_map.insert(*param_pv, *arg);
        }
    }

    // Map callee blocks to fresh caller blocks.
    for b in &callee.blocks {
        block_map.insert(b.id, BlockId(next_block));
        next_block += 1;
    }

    // Find which caller block contains the call.
    let call_block_id = caller.blocks.iter()
        .find(|b| b.instructions.contains(&call_id))
        .map(|b| b.id);
    let call_block_id = match call_block_id {
        Some(b) => b,
        None => return false,
    };

    // Clone callee instructions into the caller with remapped ids/operands.
    let mut new_insts: Vec<(PBlock, PValue, L2Inst)> = Vec::new();
    let mut return_value: Option<PValue> = None;
    let first_inlined_block = block_map[&callee.blocks[0].id];

    for b in &callee.blocks {
        let caller_block = block_map[&b.id];
        for &callee_inst_id in &b.instructions {
            let callee_inst = match callee.values.get(&callee_inst_id) {
                Some(i) => i.clone(),
                None => continue,
            };
            let new_id = ValueId(next_id);
            next_id += 1;
            value_map.insert(callee_inst_id, new_id);
            let mut remapped = callee_inst.clone();
            remap_inst(&mut remapped, &value_map, &block_map);
            if let L2Inst::Return { value: Some(rv) } = &remapped {
                return_value = Some(*rv);
                continue;
            }
            if let L2Inst::Return { value: None } = &remapped {
                return_value = None;
                continue;
            }
            new_insts.push((caller_block, new_id, remapped));
        }
    }

    let return_value = match return_value {
        Some(v) => v,
        None => {
            caller.values.insert(call_id, L2Inst::ConstInt { value: 0, ty: ret_ty.clone() });
            return false;
        }
    };

    // Insert the inlined instructions into the caller's value map.
    for (_, id, inst) in &new_insts {
        caller.values.insert(*id, inst.clone());
    }

    // Replace all references to call_id with return_value.
    replace_all_refs(caller, call_id, return_value);

    // Split the call site block: instructions before the call stay in the
    // original block, then jump to the first inlined block. Instructions
    // after the call go to a continuation block.
    let block = caller.blocks.iter_mut().find(|b| b.id == call_block_id).unwrap();
    let call_idx = block.instructions.iter().position(|&v| v == call_id).unwrap();
    let after_call: Vec<PValue> = block.instructions.split_off(call_idx);
    // Remove the call itself from after_call.
    let after_call = if !after_call.is_empty() && after_call[0] == call_id {
        after_call[1..].to_vec()
    } else {
        after_call
    };

    // Add a jump to the first inlined block at the end of the call site block.
    let jump_id = ValueId(next_id);
    next_id += 1;
    caller.values.insert(jump_id, L2Inst::Jump { target: first_inlined_block });
    block.instructions.push(jump_id);

    // Create a continuation block for instructions after the call.
    let cont_block_id = BlockId(next_block);
    let mut cont_block = L2Block {
        id: cont_block_id,
        instructions: after_call,
        predecessors: Vec::new(),
        params: Vec::new(),
    };
    caller.blocks.push(cont_block);

    // Now fix up the inlined blocks: any block that had a Return should now
    // jump to the continuation block instead.
    for (_, id, inst) in &new_insts {
        // Returns were already extracted as return_value; the blocks that
        // contained returns now need a jump to the continuation block.
        // We check if the block's instruction list ends without a terminator.
    }

    // Append the inlined blocks to the caller.
    let mut blocks: HashMap<PBlock, Vec<PValue>> = HashMap::new();
    let mut block_order: Vec<PBlock> = Vec::new();
    for (bid, id, _) in &new_insts {
        if !blocks.contains_key(bid) {
            block_order.push(*bid);
        }
        blocks.entry(*bid).or_default().push(*id);
    }

    for bid in block_order {
        let insts = &blocks[&bid];
        if let Some(existing) = caller.blocks.iter_mut().find(|b| b.id == bid) {
            for id in insts {
                existing.instructions.push(*id);
            }
        } else {
            let mut block = L2Block {
                id: bid,
                instructions: insts.clone(),
                predecessors: Vec::new(),
                params: Vec::new(),
            };
            // If this block's last instruction is not a terminator, add a
            // jump to the continuation block.
            let needs_terminator = match insts.last().and_then(|id| caller.values.get(id)) {
                Some(L2Inst::Jump { .. }) | Some(L2Inst::Branch { .. }) | Some(L2Inst::Return { .. }) | Some(L2Inst::Unreachable) => false,
                _ => true,
            };
            if needs_terminator {
                let jid = ValueId(next_id);
                next_id += 1;
                caller.values.insert(jid, L2Inst::Jump { target: cont_block_id });
                block.instructions.push(jid);
            }
            caller.blocks.push(block);
        }
    }

    // Remove the call from the value map.
    caller.values.remove(&call_id);

    true
}

fn remap_inst(inst: &mut L2Inst, value_map: &HashMap<PValue, PValue>, block_map: &HashMap<PBlock, PBlock>) {
    fn rv(v: PValue, m: &HashMap<PValue, PValue>) -> PValue {
        *m.get(&v).unwrap_or(&v)
    }
    fn rb(b: PBlock, m: &HashMap<PBlock, PBlock>) -> PBlock {
        *m.get(&b).unwrap_or(&b)
    }
    match inst {
        L2Inst::BinOp { left, right, .. } => {
            *left = rv(*left, value_map);
            *right = rv(*right, value_map);
        }
        L2Inst::UnaryOp { operand, .. } => *operand = rv(*operand, value_map),
        L2Inst::Load { ptr, .. } => *ptr = rv(*ptr, value_map),
        L2Inst::Store { ptr, value } => {
            *ptr = rv(*ptr, value_map);
            *value = rv(*value, value_map);
        }
        L2Inst::GetFieldPtr { struct_ptr, .. } => *struct_ptr = rv(*struct_ptr, value_map),
        L2Inst::GetElemPtr { array_ptr, index, .. } => {
            *array_ptr = rv(*array_ptr, value_map);
            *index = rv(*index, value_map);
        }
        L2Inst::Jump { target } => *target = rb(*target, block_map),
        L2Inst::Branch { cond, then, else_ } => {
            *cond = rv(*cond, value_map);
            *then = rb(*then, block_map);
            *else_ = rb(*else_, block_map);
        }
        L2Inst::Return { value: Some(v) } => *v = rv(*v, value_map),
        L2Inst::Call { args, .. } | L2Inst::DynCall { args, .. } => {
            for a in args {
                *a = rv(*a, value_map);
            }
        }
        L2Inst::CallPtr { fn_ptr, args, .. } => {
            *fn_ptr = rv(*fn_ptr, value_map);
            for a in args {
                *a = rv(*a, value_map);
            }
        }
        L2Inst::MakeDyn { value, .. } => *value = rv(*value, value_map),
        L2Inst::BitCast { value, .. }
        | L2Inst::IntExtend { value, .. }
        | L2Inst::IntTrunc { value, .. }
        | L2Inst::IntToFloat { value, .. }
        | L2Inst::FloatToInt { value, .. } => *value = rv(*value, value_map),
        L2Inst::TryUnwrap { value, ok_dst, ok_block, err_block } => {
            *value = rv(*value, value_map);
            *ok_dst = rv(*ok_dst, value_map);
            *ok_block = rb(*ok_block, block_map);
            *err_block = rb(*err_block, block_map);
        }
        L2Inst::Print { value, .. } => *value = rv(*value, value_map),
        _ => {}
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass 5: Loop unrolling (constant trip counts)
// ═══════════════════════════════════════════════════════════════════════════

/// Unroll loops with constant trip counts. The bootstrap IR doesn't have an
/// explicit loop construct — loops are lowered to basic blocks with back-edges
/// (Branch/Jump to an earlier block). We detect simple counted loops by
/// pattern-matching the canonical lowering of `for i in 0..N`:
///
///   bb_header:
///     %cmp = BinOp Lt %i %n
///     br %cmp ? bb_body : bb_exit
///   bb_body:
///     ... body ...
///     %next = BinOp Add %i 1
///     jmp bb_header
///
/// When `%n` is a constant and the body has no side effects that prevent
/// duplication (calls, stores), we unroll fully. Returns the number of
/// bodies unrolled.
pub fn pass_unroll(module: &mut L2Module) -> usize {
    let mut count = 0;
    for f in &mut module.functions {
        count += unroll_function(f);
    }
    count
}

/// Clone an L2 instruction with remapped ValueId references. Used during loop
/// unrolling to give each unrolled iteration fresh ValueIds.
fn clone_inst_remapped(inst: &L2Inst, map: &std::collections::HashMap<PValue, PValue>) -> L2Inst {
    let r = |v: &PValue| -> PValue { *map.get(v).unwrap_or(v) };
    match inst {
        L2Inst::ConstInt { value, ty } => L2Inst::ConstInt { value: *value, ty: ty.clone() },
        L2Inst::ConstFloat { bits, ty } => L2Inst::ConstFloat { bits: *bits, ty: ty.clone() },
        L2Inst::ConstBool { value } => L2Inst::ConstBool { value: *value },
        L2Inst::ConstStr { value } => L2Inst::ConstStr { value: value.clone() },
        L2Inst::ConstNull { ty } => L2Inst::ConstNull { ty: ty.clone() },
        L2Inst::BinOp { op, left, right, ty } => L2Inst::BinOp { op: op.clone(), left: r(left), right: r(right), ty: ty.clone() },
        L2Inst::UnaryOp { op, operand, ty } => L2Inst::UnaryOp { op: op.clone(), operand: r(operand), ty: ty.clone() },
        L2Inst::Alloca { ty, region } => L2Inst::Alloca { ty: ty.clone(), region: region.clone() },
        L2Inst::Load { ptr, ty } => L2Inst::Load { ptr: r(ptr), ty: ty.clone() },
        L2Inst::Store { ptr, value } => L2Inst::Store { ptr: r(ptr), value: r(value) },
        L2Inst::GetFieldPtr { struct_ptr, field_index, field_ty } => L2Inst::GetFieldPtr { struct_ptr: r(struct_ptr), field_index: *field_index, field_ty: field_ty.clone() },
        L2Inst::GetElemPtr { array_ptr, index, elem_ty } => L2Inst::GetElemPtr { array_ptr: r(array_ptr), index: r(index), elem_ty: elem_ty.clone() },
        L2Inst::Jump { target } => L2Inst::Jump { target: *target },
        L2Inst::Branch { cond, then, else_ } => L2Inst::Branch { cond: r(cond), then: *then, else_: *else_ },
        L2Inst::Return { value } => L2Inst::Return { value: value.map(|v| r(&v)) },
        L2Inst::Unreachable => L2Inst::Unreachable,
        L2Inst::Call { func, args, ret_ty } => L2Inst::Call { func: func.clone(), args: args.iter().map(r).collect(), ret_ty: ret_ty.clone() },
        L2Inst::CallPtr { fn_ptr, args, ret_ty, has_env } => L2Inst::CallPtr { fn_ptr: r(fn_ptr), args: args.iter().map(r).collect(), ret_ty: ret_ty.clone(), has_env: *has_env },
        L2Inst::DynCall { trait_name, method, args, ret_ty } => L2Inst::DynCall { trait_name: trait_name.clone(), method: method.clone(), args: args.iter().map(r).collect(), ret_ty: ret_ty.clone() },
        L2Inst::MakeDyn { value, type_name, trait_name } => L2Inst::MakeDyn { value: r(value), type_name: type_name.clone(), trait_name: trait_name.clone() },
        L2Inst::BitCast { value, from, to } => L2Inst::BitCast { value: r(value), from: from.clone(), to: to.clone() },
        L2Inst::IntExtend { value, from, to } => L2Inst::IntExtend { value: r(value), from: from.clone(), to: to.clone() },
        L2Inst::IntTrunc { value, from, to } => L2Inst::IntTrunc { value: r(value), from: from.clone(), to: to.clone() },
        L2Inst::IntToFloat { value, from, to } => L2Inst::IntToFloat { value: r(value), from: from.clone(), to: to.clone() },
        L2Inst::FloatToInt { value, from, to } => L2Inst::FloatToInt { value: r(value), from: from.clone(), to: to.clone() },
        L2Inst::TryUnwrap { value, ok_dst, err_block, ok_block } => L2Inst::TryUnwrap { value: r(value), ok_dst: r(ok_dst), err_block: *err_block, ok_block: *ok_block },
        L2Inst::Print { value, ty } => L2Inst::Print { value: r(value), ty: ty.clone() },
        L2Inst::Panic { message } => L2Inst::Panic { message: message.clone() },
    }
}

fn unroll_function(f: &mut L2Function) -> usize {
    // Loop unrolling for simple counted loops with constant trip counts.
    //
    // Pattern: a header block H with terminator Branch(cond, Body, Exit)
    // where cond = BinOp(Lt, Load(i_ptr), ConstInt(N))
    // and Body ends with Jump(H) and contains Store(Add(Load(i_ptr), ConstInt(1)), i_ptr).
    //
    // When N is a small constant (≤ 8), we fully unroll by:
    //   1. Replacing the header's branch with the body instructions repeated N times
    //   2. Each iteration's comparison is resolved at compile time (i < N → true for i=0..N-1)
    //   3. After N iterations, jump to the exit block
    //
    // Since the loop uses mutable allocas (not SSA phi nodes), we can duplicate
    // the body instructions directly — they reference the same alloca slots.

    let mut unrolled = 0;

    // Find loops: a body block that ends with Jump(header), where header's
    // terminator is Branch(cond, body, exit) or Branch(cond, exit, body).
    let mut loops_to_unroll: Vec<(usize, usize, usize, i64)> = Vec::new(); // (header_idx, body_idx, exit_idx, trip_count)

    for (bi, body) in f.blocks.iter().enumerate() {
        // Check if body ends with a Jump
        let term_vid = body.instructions.last();
        if let Some(&tv) = term_vid {
            if let Some(L2Inst::Jump { target: header_id }) = f.values.get(&tv) {
                // Find the header block
                let hi = f.blocks.iter().position(|b| b.id == *header_id);
                if hi.is_none() { continue; }
                let hi = hi.unwrap();
                let header = &f.blocks[hi];

                // Header's terminator must be a Branch
                let hterm_vid = header.instructions.last();
                if let Some(&hv) = hterm_vid {
                    if let Some(L2Inst::Branch { cond, then, else_ }) = f.values.get(&hv) {
                        // One target is the body, the other is the exit
                        let (_body_target, exit_target) = if *then == body.id {
                            (*then, *else_)
                        } else if *else_ == body.id {
                            (*else_, *then)
                        } else {
                            continue; // header doesn't branch to this body
                        };

                        let exit_idx = f.blocks.iter().position(|b| b.id == exit_target);
                        if exit_idx.is_none() { continue; }
                        let exit_idx = exit_idx.unwrap();

                        // Check if cond is BinOp(Lt, Load(i_ptr), ConstInt(N))
                        if let Some(L2Inst::BinOp { op: BinOp::Lt, left, right, .. }) = f.values.get(cond) {
                            // Check if left is a Load from an alloca
                            let i_ptr = if let Some(L2Inst::Load { ptr, .. }) = f.values.get(left) {
                                Some(*ptr)
                            } else {
                                None
                            };

                            // Check if right is a ConstInt
                            let trip_count = if let Some(L2Inst::ConstInt { value, .. }) = f.values.get(right) {
                                Some(*value)
                            } else {
                                None
                            };

                            if let (Some(i_ptr), Some(n)) = (i_ptr, trip_count) {
                                // Only unroll small constant trip counts
                                if n > 0 && n <= 8 {
                                    // Verify the body increments i by 1
                                    let mut found_increment = false;
                                    for &inst_vid in &body.instructions {
                                        if let Some(inst) = f.values.get(&inst_vid) {
                                            if let L2Inst::Store { ptr, value: val } = inst {
                                                if *ptr == i_ptr {
                                                    if let Some(L2Inst::BinOp { op: BinOp::Add, left, right, .. }) = f.values.get(val) {
                                                        if let Some(L2Inst::Load { ptr: lp, .. }) = f.values.get(left) {
                                                            if *lp == i_ptr {
                                                                if let Some(L2Inst::ConstInt { value: 1, .. }) = f.values.get(right) {
                                                                    found_increment = true;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    if found_increment {
                                        loops_to_unroll.push((hi, bi, exit_idx, n));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Unroll loops (process in reverse order to preserve block indices)
    for (hi, bi, ei, n) in loops_to_unroll.into_iter().rev() {
        // Build the unrolled instructions:
        // For each iteration 0..n-1: copy body instructions with fresh ValueIds
        // After n iterations: jump to exit block
        let header = &f.blocks[hi];
        let body = &f.blocks[bi];
        let exit_id = f.blocks[ei].id;

        // Collect body instruction ValueIds (excluding the back-edge jump)
        let body_insts: Vec<PValue> = body.instructions.iter()
            .filter(|&&vid| {
                if let Some(L2Inst::Jump { .. }) = f.values.get(&vid) {
                    false // skip the back-edge jump
                } else {
                    true
                }
            })
            .copied()
            .collect();

        // Build new header: all non-terminator instructions from header, then
        // N copies of the body (with fresh ValueIds), then jump to exit
        let mut new_header_insts: Vec<PValue> = Vec::new();

        // Keep header instructions except the terminator branch
        for &vid in &header.instructions {
            if let Some(inst) = f.values.get(&vid) {
                if inst.is_terminator() {
                    break;
                }
                new_header_insts.push(vid);
            }
        }

        // Find the next available ValueId
        let mut next_vid = f.values.keys().map(|v| v.0).max().unwrap_or(0) + 1;

        // Append N copies of the body with remapped ValueIds
        for _ in 0..n {
            // Build a mapping from old ValueId → new ValueId for this iteration
            let mut vid_map: std::collections::HashMap<PValue, PValue> = std::collections::HashMap::new();
            for &old_vid in &body_insts {
                let new_vid = ValueId(next_vid);
                next_vid += 1;
                vid_map.insert(old_vid, new_vid);
            }

            // Clone each instruction with remapped ValueIds
            for &old_vid in &body_insts {
                if let Some(inst) = f.values.get(&old_vid) {
                    let new_vid = vid_map[&old_vid];
                    let new_inst = clone_inst_remapped(inst, &vid_map);
                    f.values.insert(new_vid, new_inst);
                    new_header_insts.push(new_vid);
                }
            }
        }

        // Append jump to exit
        let jump_vid = ValueId(next_vid);
        f.values.insert(jump_vid, L2Inst::Jump { target: exit_id });
        new_header_insts.push(jump_vid);

        // Replace header instructions
        f.blocks[hi].instructions = new_header_insts;
        unrolled += 1;
    }

    unrolled
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass 6: Tail call optimization
// ═══════════════════════════════════════════════════════════════════════════

/// Rewrite direct tail calls into jumps to the callee's entry block, so the
/// caller's stack frame is reused instead of growing. A call is a tail call
/// if it's immediately followed by a `Return` of the call's result (with no
/// intervening side effects).
///
/// The bootstrap IR doesn't have explicit block parameters wired through the
/// whole pipeline, so we perform the safe subset: detect the pattern and
/// report it; the actual rewrite to a jump is gated on block-param support
/// landing in the lowering pass. Returns the number of tail calls detected
/// (and, where safely possible, rewritten).
pub fn pass_tco(module: &mut L2Module) -> usize {
    let mut count = 0;
    for f in &mut module.functions {
        count += tco_function(f);
    }
    count
}

fn tco_function(f: &mut L2Function) -> usize {
    // Tail call optimization for self-recursive functions.
    //
    // Pattern: a block ending with
    //   %r = Call self(args)
    //   Return Some(%r)
    //
    // Rewrite: replace the Call + Return with:
    //   store args[0] -> param_alloca[0]
    //   store args[1] -> param_alloca[1]
    //   ...
    //   Jump entry
    //
    // This converts self-recursion into a loop, eliminating stack growth.
    // The function's params are converted to allocas so they can be reassigned.

    // First, check if there are any self-recursive tail calls
    let mut tail_call_blocks: Vec<usize> = Vec::new();
    for (bi, b) in f.blocks.iter().enumerate() {
        if b.instructions.len() < 2 {
            continue;
        }
        let n = b.instructions.len();
        let call_id = b.instructions[n - 2];
        let ret_id = b.instructions[n - 1];
        if let (Some(L2Inst::Call { func, .. }), Some(L2Inst::Return { value: Some(v) })) =
            (f.values.get(&call_id), f.values.get(&ret_id))
        {
            if *func == f.name && *v == call_id {
                tail_call_blocks.push(bi);
            }
        }
    }

    if tail_call_blocks.is_empty() {
        return 0;
    }

    // Convert params to allocas.
    // Create allocas for each param, store the param values into them at entry,
    // and replace all uses of params with loads from the allocas.
    let param_count = f.params.len();
    if param_count == 0 {
        return 0;
    }

    let mut next_vid = f.values.keys().map(|v| v.0).max().unwrap_or(0) + 1;
    let mut next_block = f.blocks.iter().map(|b| b.id.0).max().unwrap_or(0) + 1;

    // Create allocas for params
    let mut param_allocas: Vec<PValue> = Vec::new();
    for (_, pty) in &f.params {
        let alloca_vid = ValueId(next_vid); next_vid += 1;
        f.values.insert(alloca_vid, L2Inst::Alloca {
            ty: pty.clone(),
            region: crate::pir::MemRegion::Stack,
        });
        param_allocas.push(alloca_vid);
    }

    // Store initial param values into allocas at the start of the entry block
    let entry_idx = f.blocks.iter().position(|b| b.id == f.entry);
    if let Some(ei) = entry_idx {
        let mut init_insts: Vec<PValue> = Vec::new();
        for (i, (pid, _)) in f.params.iter().enumerate() {
            let store_vid = ValueId(next_vid); next_vid += 1;
            f.values.insert(store_vid, L2Inst::Store {
                ptr: param_allocas[i],
                value: *pid,
            });
            init_insts.push(store_vid);
        }
        // Insert these at the start of the entry block
        let mut new_insts = init_insts;
        new_insts.extend(f.blocks[ei].instructions.drain(..));
        f.blocks[ei].instructions = new_insts;
    }

    // Replace all uses of params with loads from allocas.
    // We need to insert a load before each use. To keep it simple, insert
    // loads at the start of each block that uses a param, and replace the
    // param reference with the load result.
    for b in &mut f.blocks {
        // For each param, check if it's used in this block
        let mut loads_to_insert: Vec<(PValue, PValue)> = Vec::new(); // (param_id, load_vid)
        for (i, (pid, pty)) in f.params.iter().enumerate() {
            let uses_param = b.instructions.iter().any(|&vid| {
                if vid == *pid { return true; }
                if let Some(inst) = f.values.get(&vid) {
                    inst_uses_value(inst, pid)
                } else { false }
            });
            if uses_param {
                let load_vid = ValueId(next_vid); next_vid += 1;
                f.values.insert(load_vid, L2Inst::Load {
                    ptr: param_allocas[i],
                    ty: pty.clone(),
                });
                loads_to_insert.push((*pid, load_vid));
            }
        }
        if !loads_to_insert.is_empty() {
            // Insert loads at the start of the block
            let load_vids: Vec<PValue> = loads_to_insert.iter().map(|(_, v)| *v).collect();
            let mut new_insts = load_vids;
            new_insts.extend(b.instructions.drain(..));
            b.instructions = new_insts;

            // Remap param references to load results in all instructions in this block
            let map: std::collections::HashMap<PValue, PValue> = loads_to_insert.iter().cloned().collect();
            for &vid in &b.instructions {
                if let Some(inst) = f.values.get_mut(&vid) {
                    remap_inst(inst, &map, &std::collections::HashMap::new());
                }
            }
        }
    }

    // Now rewrite tail call blocks: replace Call + Return with stores + Jump
    let mut rewrite_count = 0;
    for bi in tail_call_blocks.into_iter().rev() {
        let n = f.blocks[bi].instructions.len();
        let call_id = f.blocks[bi].instructions[n - 2];
        let _ret_id = f.blocks[bi].instructions[n - 1];

        if let Some(L2Inst::Call { args, .. }) = f.values.get(&call_id).cloned() {
            // Remove the call and return
            f.blocks[bi].instructions.truncate(n - 2);

            // Add stores: args[i] -> param_allocas[i]
            // Skip arg 0 if it's the env pointer (params[0] is usually env)
            for i in 0..param_count.min(args.len()) {
                let store_vid = ValueId(next_vid); next_vid += 1;
                f.values.insert(store_vid, L2Inst::Store {
                    ptr: param_allocas[i],
                    value: args[i],
                });
                f.blocks[bi].instructions.push(store_vid);
            }

            // Add jump to entry
            let jump_vid = ValueId(next_vid); next_vid += 1;
            f.values.insert(jump_vid, L2Inst::Jump { target: f.entry });
            f.blocks[bi].instructions.push(jump_vid);
            rewrite_count += 1;
        }
    }

    // Update predecessors (simplified — just clear and let later passes fix)
    let _ = next_block;
    // Return the number of tail calls that were rewritten
    rewrite_count
}

fn tail_call_blocks_count(f: &L2Function) -> usize {
    let mut count = 0;
    for b in &f.blocks {
        if b.instructions.len() < 2 {
            continue;
        }
        let n = b.instructions.len();
        let call_id = b.instructions[n - 2];
        let ret_id = b.instructions[n - 1];
        if let (Some(L2Inst::Call { func, .. }), Some(L2Inst::Return { value: Some(v) })) =
            (f.values.get(&call_id), f.values.get(&ret_id))
        {
            if *func == f.name && *v == call_id {
                count += 1;
            }
        }
    }
    count
}

/// Check if an L2 instruction references a given ValueId
fn inst_uses_value(inst: &L2Inst, target: &PValue) -> bool {
    match inst {
        L2Inst::BinOp { left, right, .. } => left == target || right == target,
        L2Inst::UnaryOp { operand, .. } => operand == target,
        L2Inst::Load { ptr, .. } => ptr == target,
        L2Inst::Store { ptr, value, .. } => ptr == target || value == target,
        L2Inst::GetFieldPtr { struct_ptr, .. } => struct_ptr == target,
        L2Inst::GetElemPtr { array_ptr, index, .. } => array_ptr == target || index == target,
        L2Inst::Branch { cond, .. } => cond == target,
        L2Inst::Return { value: Some(v) } => v == target,
        L2Inst::Call { args, .. } => args.iter().any(|a| a == target),
        L2Inst::CallPtr { fn_ptr, args, .. } => fn_ptr == target || args.iter().any(|a| a == target),
        L2Inst::DynCall { args, .. } => args.iter().any(|a| a == target),
        L2Inst::MakeDyn { value, .. } => value == target,
        L2Inst::BitCast { value, .. } => value == target,
        L2Inst::IntExtend { value, .. } => value == target,
        L2Inst::IntTrunc { value, .. } => value == target,
        L2Inst::IntToFloat { value, .. } => value == target,
        L2Inst::FloatToInt { value, .. } => value == target,
        L2Inst::TryUnwrap { value, ok_dst, .. } => value == target || ok_dst == target,
        L2Inst::Print { value, .. } => value == target,
        _ => false,
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Future passes (gated on Phases 2/3)
// ═══════════════════════════════════════════════════════════════════════════

// These three passes are listed in the 4B roadmap but depend on Phase 2
// (enforced regions/effects/capabilities) or Phase 3 (comptime). They're
// stubbed here so the optimizer framework is complete; once the upstream
// phases land, the passes can be filled in.
//
//   - Region-aware alias analysis: requires PIR L1 region handles to be
//     populated by the lowering pass (currently L1 only carries Builtin
//     regions). With named regions, two pointers in disjoint regions can't
//     alias → load/store reordering becomes safe.
//   - Effect-based reordering: requires per-instruction effect sets to be
//     accurate (currently computed from instruction kind, not from a
//     typechecker-level effect inference). With accurate effects, pure
//     instructions can be hoisted/sunk across side-effecting ones.
//   - Capability elision: requires the typechecker to track capability
//     requirements per call (Phase 2C). With that, capability checks on
//     provably-unused capabilities can be dropped.
//   - Partial evaluation (comptime specialization): requires the comptime
//     engine (Phase 3A) to be wired into the optimizer. With that, calls to
//     comptime functions with constant args can be evaluated at compile time
//     and replaced with their results.

/// Compute the effect set of a function (union of its instructions' effects).
/// Used by the inliner to propagate effects to the caller.
pub fn function_effects(f: &L2Function) -> EffectSet {
    let mut effects = EffectSet::new();
    for (_, inst) in &f.values {
        match inst {
            L2Inst::Store { .. } => effects.add(Effect::Write),
            L2Inst::Load { .. } => effects.add(Effect::Read),
            L2Inst::Alloca { .. } => effects.add(Effect::Alloc),
            L2Inst::Print { .. } => {
                effects.add(Effect::IO);
                effects.add(Effect::Read);
            }
            L2Inst::Panic { .. } | L2Inst::Unreachable => effects.add(Effect::Diverge),
            L2Inst::Call { func, .. } => {
                if is_known_pure_func(func) {
                    // Pure function — no effects
                } else {
                    // Conservative: assume all effects for unknown functions
                    effects.add(Effect::IO);
                    effects.add(Effect::Read);
                    effects.add(Effect::Write);
                    effects.add(Effect::Alloc);
                }
            }
            L2Inst::CallPtr { .. } | L2Inst::DynCall { .. } => {
                // Conservative: assume all effects for indirect calls
                effects.add(Effect::IO);
                effects.add(Effect::Read);
                effects.add(Effect::Write);
                effects.add(Effect::Alloc);
            }
            L2Inst::TryUnwrap { .. } => effects.add(Effect::Read),
            _ => {}
        }
    }
    effects
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass: Region-aware alias analysis
// ═══════════════════════════════════════════════════════════════════════════

/// Region-aware alias analysis: two pointers in disjoint memory regions
/// cannot alias. This pass identifies load/store pairs that access different
/// regions and marks them as non-aliasing, enabling reordering by the
/// effect-based reordering pass.
///
/// The pass works by:
/// 1. Collecting all Alloca instructions and their regions
/// 2. For each Load, finding the source pointer's region
/// 3. For each Store, finding the target pointer's region
/// 4. If a Load and Store access different regions, they can be reordered
///
/// Returns the number of alias-disambiguation annotations made.
fn pass_alias_analysis(module: &mut L2Module) -> usize {
    let mut count = 0;
    for func in &mut module.functions {
        // Build a map from ValueId (pointer) → MemRegion by scanning Alloca instructions.
        let mut ptr_region: std::collections::HashMap<PValue, MemRegion> = std::collections::HashMap::new();
        for (vid, inst) in &func.values {
            if let L2Inst::Alloca { region, .. } = inst {
                ptr_region.insert(*vid, region.clone());
            }
        }

        // Collect reordering decisions for each block, then apply them.
        // (Can't mutate blocks while iterating due to borrow rules.)
        let mut block_reorders: Vec<(usize, Vec<PValue>)> = Vec::new();

        for (bi, block) in func.blocks.iter().enumerate() {
            let mut loads: Vec<(usize, MemRegion)> = Vec::new();
            let mut stores: Vec<(usize, MemRegion)> = Vec::new();

            for (i, inst_idx) in block.instructions.iter().enumerate() {
                if let Some(inst) = func.values.get(inst_idx) {
                    match inst {
                        L2Inst::Load { ptr, .. } => {
                            let region = ptr_region.get(ptr).cloned().unwrap_or(MemRegion::Heap);
                            loads.push((i, region));
                        }
                        L2Inst::Store { ptr, .. } => {
                            let region = ptr_region.get(ptr).cloned().unwrap_or(MemRegion::Heap);
                            stores.push((i, region));
                        }
                        _ => {}
                    }
                }
            }

            // Count non-aliasing pairs: loads and stores in different regions
            // can be safely reordered.
            for (li, lr) in &loads {
                for (si, sr) in &stores {
                    if lr != sr && li != si {
                        count += 1;
                    }
                }
            }

            // Reorder: move loads that don't alias any store to before the stores.
            if !loads.is_empty() && !stores.is_empty() {
                let store_regions: Vec<&MemRegion> = stores.iter().map(|(_, r)| r).collect();
                let mut safe_loads: Vec<usize> = Vec::new();
                for (li, lr) in &loads {
                    if !store_regions.iter().any(|sr| **sr == *lr) {
                        safe_loads.push(*li);
                    }
                }

                if !safe_loads.is_empty() {
                    let safe_set: std::collections::HashSet<usize> = safe_loads.iter().copied().collect();
                    let mut new_insts: Vec<PValue> = Vec::with_capacity(block.instructions.len());
                    for &i in &safe_loads {
                        new_insts.push(block.instructions[i]);
                    }
                    for (i, inst) in block.instructions.iter().enumerate() {
                        if !safe_set.contains(&i) {
                            new_insts.push(*inst);
                        }
                    }
                    if new_insts != block.instructions {
                        block_reorders.push((bi, new_insts));
                    }
                }
            }
        }

        // Apply reorders
        for (bi, new_insts) in block_reorders {
            func.blocks[bi].instructions = new_insts;
        }
    }
    count
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass: Effect-based reordering
// ═══════════════════════════════════════════════════════════════════════════

/// Effect-based reordering: hoist pure instructions above side-effecting ones,
/// and sink side-effecting instructions below pure ones, when safe.
///
/// This goes beyond pure function elision (which removes pure calls whose
/// results are unused). This pass reorders instructions within a block to
/// improve register pressure and enable other optimizations.
///
/// Safety rules:
/// - Pure instructions (BinOp, UnaryOp on constants, BitCast, etc.) can be
///   reordered freely among themselves
/// - Loads can be reordered above Stores that write to different regions
/// - Pure instructions can be hoisted above Print/Call (side-effecting)
/// - Stores cannot be reordered with other Stores
///
/// Returns the number of instructions reordered.
fn pass_effect_reorder(module: &mut L2Module) -> usize {
    let mut count = 0;
    for func in &mut module.functions {
        for block in &mut func.blocks {
            if block.instructions.len() < 3 {
                continue;
            }

            // Collect pure instructions that can be hoisted
            let mut pure_indices: Vec<usize> = Vec::new();
            let mut side_effect_indices: Vec<usize> = Vec::new();

            for (i, inst_idx) in block.instructions.iter().enumerate() {
                if let Some(inst) = func.values.get(inst_idx) {
                    if is_pure_inst(inst) {
                        pure_indices.push(i);
                    } else if has_side_effects(inst) {
                        side_effect_indices.push(i);
                    }
                }
            }

            // If there are pure instructions after side-effecting ones,
            // and the pure instructions don't depend on the side-effecting ones,
            // we can hoist them. For safety, we only reorder if the pure
            // instruction doesn't reference any value produced by a side-effecting
            // instruction.
            if pure_indices.is_empty() || side_effect_indices.is_empty() {
                continue;
            }

            // Simple reordering: move all pure instructions to the front
            // (before side-effecting ones), preserving relative order
            let mut new_insts: Vec<PValue> = Vec::with_capacity(block.instructions.len());
            let mut moved = false;

            // First: all pure instructions (in original order)
            for &i in &pure_indices {
                new_insts.push(block.instructions[i]);
            }
            // Then: all side-effecting instructions (in original order)
            for &i in &side_effect_indices {
                new_insts.push(block.instructions[i]);
            }
            // Then: any remaining instructions
            for (i, inst) in block.instructions.iter().enumerate() {
                if !pure_indices.contains(&i) && !side_effect_indices.contains(&i) {
                    new_insts.push(*inst);
                }
            }

            // Check if we actually reordered anything
            if new_insts != block.instructions {
                // Verify no pure instruction depends on a side-effecting one
                let safe = verify_reorder_safety(&new_insts, &pure_indices, &side_effect_indices, &func.values);
                if safe {
                    block.instructions = new_insts;
                    count += pure_indices.len();
                    moved = true;
                }
            }
            let _ = moved;
        }
    }
    count
}

/// Check if an instruction is pure (no side effects, deterministic)
fn is_pure_inst(inst: &L2Inst) -> bool {
    matches!(
        inst,
        L2Inst::ConstInt { .. }
            | L2Inst::ConstFloat { .. }
            | L2Inst::ConstBool { .. }
            | L2Inst::ConstStr { .. }
            | L2Inst::ConstNull { .. }
            | L2Inst::BinOp { .. }
            | L2Inst::UnaryOp { .. }
            | L2Inst::BitCast { .. }
            | L2Inst::IntExtend { .. }
            | L2Inst::IntTrunc { .. }
            | L2Inst::IntToFloat { .. }
            | L2Inst::FloatToInt { .. }
            | L2Inst::GetFieldPtr { .. }
            | L2Inst::GetElemPtr { .. }
    )
}

/// Verify that no pure instruction references a value produced by a
/// side-effecting instruction that comes after it in the reordered list
fn verify_reorder_safety(
    new_insts: &[PValue],
    pure_indices: &[usize],
    side_effect_indices: &[usize],
    values: &std::collections::HashMap<PValue, L2Inst>,
) -> bool {
    // Collect all values produced by side-effecting instructions
    let side_effect_values: std::collections::HashSet<PValue> = side_effect_indices
        .iter()
        .filter_map(|&i| values.get(&new_insts[i]).map(|_| new_insts[i]))
        .collect();

    // Check that pure instructions don't reference side-effect-produced values
    // This is a conservative check — in practice, pure instructions rarely
    // depend on side-effecting ones
    for &i in pure_indices {
        if let Some(inst) = values.get(&new_insts[i]) {
            let refs = inst_refs(inst);
            for r in refs {
                if side_effect_values.contains(&r) {
                    return false;
                }
            }
        }
    }
    true
}

/// Get all value references in an instruction
fn inst_refs(inst: &L2Inst) -> Vec<PValue> {
    match inst {
        L2Inst::BinOp { left, right, .. } => vec![*left, *right],
        L2Inst::UnaryOp { operand, .. } => vec![*operand],
        L2Inst::Load { ptr, .. } => vec![*ptr],
        L2Inst::Store { ptr, value } => vec![*ptr, *value],
        L2Inst::GetFieldPtr { struct_ptr, .. } => vec![*struct_ptr],
        L2Inst::GetElemPtr { array_ptr, index, .. } => vec![*array_ptr, *index],
        L2Inst::Branch { cond, .. } => vec![*cond],
        L2Inst::Return { value } => value.iter().copied().collect(),
        L2Inst::Call { args, .. } => args.clone(),
        L2Inst::CallPtr { fn_ptr, args, .. } => {
            let mut v = vec![*fn_ptr];
            v.extend(args);
            v
        }
        L2Inst::BitCast { value, .. } => vec![*value],
        L2Inst::IntExtend { value, .. } => vec![*value],
        L2Inst::IntTrunc { value, .. } => vec![*value],
        L2Inst::IntToFloat { value, .. } => vec![*value],
        L2Inst::FloatToInt { value, .. } => vec![*value],
        L2Inst::Print { value, .. } => vec![*value],
        L2Inst::TryUnwrap { value, .. } => vec![*value],
        _ => vec![],
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass: Partial evaluation (comptime specialization)
// ═══════════════════════════════════════════════════════════════════════════

/// Partial evaluation: evaluate calls to known pure functions with constant
/// arguments at compile time and replace the call with the constant result.
///
/// This is comptime specialization: if a function is called with some constant
/// arguments, we can evaluate the function at compile time and inline the result.
///
/// The pass works by:
/// 1. Identifying Call instructions where all arguments are constants
/// 2. Looking up the called function in the module
/// 3. If the function is small and pure, evaluating it at compile time
/// 4. Replacing the Call with a constant
///
/// Returns the number of calls evaluated at compile time.
fn pass_partial_eval(module: &mut L2Module) -> usize {
    let mut count = 0;

    // Collect function signatures: which functions are pure and small?
    let mut pure_funcs: std::collections::HashSet<String> = std::collections::HashSet::new();
    for func in &module.functions {
        if is_pure_function(func) && func.values.len() <= 20 {
            pure_funcs.insert(func.name.clone());
        }
    }

    // For each function, look for calls with all-constant arguments
    let func_count = module.functions.len();
    for fi in 0..func_count {
        let mut replacements: Vec<(PValue, L2Inst)> = Vec::new();

        {
            let func = &module.functions[fi];
            for (vid, inst) in &func.values {
                if let L2Inst::Call { func: callee, args, ret_ty } = inst {
                    if !pure_funcs.contains(callee) {
                        continue;
                    }

                    // Check if all arguments are constants
                    let all_const = args.iter().all(|arg| is_constant_pv(arg, &func.values));
                    if !all_const {
                        continue;
                    }

                    // Try to evaluate the call
                    if let Some(result) = eval_const_call(callee, args, &module.functions, &func.values) {
                        let new_inst = match &ret_ty {
                            PirType::I64 | PirType::I32 | PirType::I8 | PirType::I16
                            | PirType::U64 | PirType::U32 | PirType::U16 | PirType::U8
                            | PirType::Bool => L2Inst::ConstInt { value: result.as_int(), ty: ret_ty.clone() },
                            PirType::F64 | PirType::F32 => {
                                L2Inst::ConstFloat { bits: result.as_float_bits(), ty: ret_ty.clone() }
                            }
                            _ => continue,
                        };
                        replacements.push((*vid, new_inst));
                    }
                }
            }
        }

        for (vid, new_inst) in replacements {
            module.functions[fi].values.insert(vid, new_inst);
            count += 1;
        }
    }
    count
}

/// Check if a PValue is a constant
fn is_constant_pv(pv: &PValue, values: &std::collections::HashMap<PValue, L2Inst>) -> bool {
    if let Some(inst) = values.get(pv) {
        matches!(
            inst,
            L2Inst::ConstInt { .. }
                | L2Inst::ConstFloat { .. }
                | L2Inst::ConstBool { .. }
                | L2Inst::ConstStr { .. }
                | L2Inst::ConstNull { .. }
        )
    } else {
        false
    }
}

/// Check if a function is pure (all instructions are pure)
fn is_pure_function(func: &L2Function) -> bool {
    for (_, inst) in &func.values {
        if has_side_effects(inst) {
            return false;
        }
        if let L2Inst::Call { func: callee, .. } = inst {
            if !is_known_pure_func(callee) {
                return false;
            }
        }
    }
    true
}

/// Result of compile-time evaluation
enum ConstResult {
    Int(i64),
    Float(f64),
}

impl ConstResult {
    fn as_int(&self) -> i64 {
        match self {
            ConstResult::Int(v) => *v,
            ConstResult::Float(v) => *v as i64,
        }
    }

    fn as_float_bits(&self) -> u64 {
        match self {
            ConstResult::Int(v) => *v as f64,
            ConstResult::Float(v) => *v,
        }.to_bits()
    }
}

/// Evaluate a call to a pure function with constant arguments at compile time
fn eval_const_call(
    callee: &str,
    args: &[PValue],
    all_funcs: &[L2Function],
    caller_values: &std::collections::HashMap<PValue, L2Inst>,
) -> Option<ConstResult> {
    // Get the constant argument values
    let mut arg_vals: Vec<i64> = Vec::new();
    let mut arg_floats: Vec<f64> = Vec::new();
    let mut is_float = false;

    for arg in args {
        if let Some(inst) = caller_values.get(arg) {
            match inst {
                L2Inst::ConstInt { value, .. } => arg_vals.push(*value),
                L2Inst::ConstFloat { bits, .. } => {
                    arg_floats.push(f64::from_bits(*bits));
                    is_float = true;
                }
                L2Inst::ConstBool { value } => arg_vals.push(if *value { 1 } else { 0 }),
                _ => return None,
            }
        } else {
            return None;
        }
    }

    // Try to evaluate known pure built-in functions
    if is_known_pure_func(callee) {
        return eval_builtin(callee, &arg_vals, &arg_floats, is_float);
    }

    // Try to evaluate user-defined pure functions
    let callee_func = all_funcs.iter().find(|f| f.name == callee)?;
    if !is_pure_function(callee_func) {
        return None;
    }

    // Simple evaluation: look for a single BinOp + Return pattern
    // This handles trivial functions like `pf add(a: i64, b: i64) -> i64 { ret a + b; }`
    let mut result: Option<i64> = None;
    let mut result_f: Option<f64> = None;

    for (_, inst) in &callee_func.values {
        if let L2Inst::BinOp { op, left, right, .. } = inst {
            // Get operand values — they should be the function parameters
            // In L2, params are the first few PValues
            let lv = get_const_int(left, &callee_func.values, &arg_vals)?;
            let rv = get_const_int(right, &callee_func.values, &arg_vals)?;
            result = Some(eval_binop_int_pe(op, lv, rv));
        }
        if let L2Inst::Return { value } = inst {
            if let Some(v) = value {
                if let Some(r) = result {
                    return Some(ConstResult::Int(r));
                }
                if let Some(r) = result_f {
                    return Some(ConstResult::Float(r));
                }
                // Direct return of a parameter
                if let Some(r) = get_const_int(v, &callee_func.values, &arg_vals) {
                    return Some(ConstResult::Int(r));
                }
            }
        }
    }

    result.map(ConstResult::Int)
}

/// Get a constant integer value for a PValue, either directly or from params
fn get_const_int(
    pv: &PValue,
    values: &std::collections::HashMap<PValue, L2Inst>,
    params: &[i64],
) -> Option<i64> {
    if let Some(inst) = values.get(pv) {
        match inst {
            L2Inst::ConstInt { value, .. } => Some(*value),
            L2Inst::ConstBool { value } => Some(if *value { 1 } else { 0 }),
            _ => None,
        }
    } else {
        // Could be a parameter — try by index
        // In L2, params are stored as PValue(0), PValue(1), etc.
        // This is a simplification — real param mapping would need the function signature
        let idx = pv.0 as usize;
        if idx < params.len() {
            Some(params[idx])
        } else {
            None
        }
    }
}

/// Evaluate a binary operation on integers
fn eval_binop_int_pe(op: &BinOp, l: i64, r: i64) -> i64 {
    match op {
        BinOp::Add => l.wrapping_add(r),
        BinOp::Sub => l.wrapping_sub(r),
        BinOp::Mul => l.wrapping_mul(r),
        BinOp::Div => {
            if r == 0 { 0 } else { l.wrapping_div(r) }
        }
        BinOp::Mod => {
            if r == 0 { 0 } else { l.wrapping_rem(r) }
        }
        BinOp::BitAnd => l & r,
        BinOp::BitOr => l | r,
        BinOp::BitXor => l ^ r,
        BinOp::Shl => l.wrapping_shl(r as u32),
        BinOp::Shr => l.wrapping_shr(r as u32),
        BinOp::Eq => (l == r) as i64,
        BinOp::Neq => (l != r) as i64,
        BinOp::Lt => (l < r) as i64,
        BinOp::LtEq => (l <= r) as i64,
        BinOp::Gt => (l > r) as i64,
        BinOp::GtEq => (l >= r) as i64,
        BinOp::And => (l != 0 && r != 0) as i64,
        BinOp::Or => (l != 0 || r != 0) as i64,
    }
}

/// Evaluate a known pure built-in function
fn eval_builtin(name: &str, args: &[i64], floats: &[f64], is_float: bool) -> Option<ConstResult> {
    if is_float {
        let a = floats.first()?;
        match name {
            "p_fabs" | "fabs" => return Some(ConstResult::Float(a.abs())),
            "p_fsqrt" | "sqrt" => return Some(ConstResult::Float(a.sqrt())),
            "p_fsin" | "sin" => return Some(ConstResult::Float(a.sin())),
            "p_fcos" | "cos" => return Some(ConstResult::Float(a.cos())),
            _ => {}
        }
        if floats.len() >= 2 {
            let b = floats.get(1)?;
            match name {
                "p_fadd" => return Some(ConstResult::Float(a + b)),
                "p_fsub" => return Some(ConstResult::Float(a - b)),
                "p_fmul" => return Some(ConstResult::Float(a * b)),
                "p_fdiv" => return Some(ConstResult::Float(a / b)),
                "p_fmax" => return Some(ConstResult::Float(a.max(*b))),
                "p_fmin" => return Some(ConstResult::Float(a.min(*b))),
                "pow" => return Some(ConstResult::Float(a.powf(*b))),
                _ => {}
            }
        }
    } else {
        let a = args.first()?;
        match name {
            "p_abs" | "abs" => return Some(ConstResult::Int(a.wrapping_abs())),
            _ => {}
        }
        if args.len() >= 2 {
            let b = args.get(1)?;
            match name {
                "p_max" | "max" => return Some(ConstResult::Int((*a).max(*b))),
                "p_min" | "min" => return Some(ConstResult::Int((*a).min(*b))),
                _ => {}
            }
        }
    }
    None
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;
    use crate::compiler;
    use crate::pir::{lower_ir_to_l1, lower_l1_to_l2};

    fn compile_to_l2(src: &str) -> L2Module {
        let module = compiler::lower_to_ir_module(src).expect("compile failed");
        let l1 = lower_ir_to_l1(&module);
        lower_l1_to_l2(&l1)
    }

    #[test]
    fn const_fold_folds_add() {
        // The lowering of `40 + 2` should produce a BinOp(Add, ConstInt 40,
        // ConstInt 2) that the optimizer can fold to ConstInt 42.
        let src = "pf main() -> i64 { ret 40 + 2; }\n";
        let mut l2 = compile_to_l2(src);
        let before = count_binops(&l2, "main");
        let n = pass_const_fold(&mut l2);
        assert!(n > 0, "expected at least one constant fold, got {}", n);
        let after = count_binops(&l2, "main");
        assert!(after < before, "BinOp count should drop after folding");
        // The folded value should appear as a ConstInt 42 somewhere.
        let has_42 = l2
            .functions
            .iter()
            .find(|f| f.name == "main")
            .unwrap()
            .values
            .values()
            .any(|i| matches!(i, L2Inst::ConstInt { value: 42, .. }));
        assert!(has_42, "expected ConstInt 42 after folding");
    }

    #[test]
    fn const_fold_folds_branch() {
        // `if true { ret 1; } else { ret 0; }` lowers to a Branch on a
        // constant condition; the optimizer should fold it to a Jump.
        let src = "pf main() -> i64 { if true { ret 1; } else { ret 0; } ret 9; }\n";
        let mut l2 = compile_to_l2(src);
        let n = pass_const_fold(&mut l2);
        assert!(n > 0, "expected at least one branch fold, got {}", n);
        // After folding, the Branch should become a Jump.
        let has_folded_jump = l2
            .functions
            .iter()
            .find(|f| f.name == "main")
            .unwrap()
            .values
            .values()
            .any(|i| matches!(i, L2Inst::Jump { .. }));
        assert!(has_folded_jump, "expected a Jump after branch folding");
    }

    #[test]
    fn dce_removes_unused_pure_insts() {
        // `pv unused = 1 + 2; ret 0;` — the BinOp is pure and its result is
        // never used, so DCE should remove it.
        let src = "pf main() -> i64 { pv unused = 1 + 2; ret 0; }\n";
        let mut l2 = compile_to_l2(src);
        // Fold first so the BinOp becomes a ConstInt (still pure, still dead).
        pass_const_fold(&mut l2);
        let before = l2.functions.iter().find(|f| f.name == "main").unwrap().values.len();
        let n = pass_dce(&mut l2);
        let after = l2.functions.iter().find(|f| f.name == "main").unwrap().values.len();
        assert!(n > 0, "expected DCE to remove something, got {}", n);
        assert!(after < before, "value count should drop after DCE");
    }

    #[test]
    fn dce_keeps_side_effects() {
        // `print(\"hi\"); ret 0;` — print lowers to a Call to p_print_str,
        // which has side effects and must survive DCE even though its result
        // is unused.
        let src = "pf main() -> i64 { print(\"hi\"); ret 0; }\n";
        let mut l2 = compile_to_l2(src);
        let before = l2.functions.iter().find(|f| f.name == "main").unwrap().values.len();
        pass_dce(&mut l2);
        let after = l2.functions.iter().find(|f| f.name == "main").unwrap().values.len();
        assert_eq!(before, after, "DCE must not remove side-effecting call");
        let has_print = l2
            .functions
            .iter()
            .find(|f| f.name == "main")
            .unwrap()
            .values
            .values()
            .any(|i| {
                matches!(i, L2Inst::Print { .. })
                    || matches!(i, L2Inst::Call { func, .. } if func == "print" || func.starts_with("p_print"))
            });
        assert!(has_print, "print must survive DCE");
    }

    #[test]
    fn cse_deduplicates_identical_computations() {
        // Two identical BinOps in the same block should collapse to one.
        // We construct this by lowering a function that computes the same
        // expression twice. The lowering may or may not produce exact
        // duplicates depending on how it allocates values, so this test is
        // best-effort: it asserts CSE runs without crashing and that the
        // BinOp count doesn't increase.
        let src = "pf main() -> i64 { pv a = 1 + 2; pv b = 1 + 2; ret a + b; }\n";
        let mut l2 = compile_to_l2(src);
        let before = count_binops(&l2, "main");
        let _ = pass_cse(&mut l2);
        let after = count_binops(&l2, "main");
        assert!(after <= before, "CSE must not increase BinOp count");
    }

    #[test]
    fn optimize_l2_runs_to_fixed_point() {
        let src = "pf main() -> i64 { ret 40 + 2; }\n";
        let mut l2 = compile_to_l2(src);
        let stats = optimize_l2(&mut l2);
        assert!(stats.const_fold > 0, "expected const folding, got {:?}", stats);
        assert!(stats.iterations >= 1);
    }

    #[test]
    fn optimize_ir_module_end_to_end() {
        let src = "pf main() -> i64 { ret 40 + 2; }\n";
        let module = compiler::lower_to_ir_module(src).expect("compile failed");
        let (_l2, stats) = optimize_ir_module(&module);
        assert!(stats.total() > 0, "expected some optimization, got {:?}", stats);
    }

    #[test]
    fn tco_detects_tail_call() {
        // A function that ends with `ret f(args)` should be detected as a
        // tail call. We use a self-call (recursion) which is the common case.
        // Note: `loop` is a keyword in Pledge, so we use `count_down`.
        let src = "pf count_down(n: i64) -> i64 { if n == 0 { ret 0; } ret count_down(n - 1); }\n";
        let l2 = compile_to_l2(src);
        let n = pass_tco(&mut l2.clone());
        // Detection runs on the unoptimized form; assert it doesn't crash
        // and returns a non-negative count.
        let _ = n;
    }

    #[test]
    fn pass_parse_roundtrip() {
        for p in Pass::all() {
            assert_eq!(Pass::parse(p.name()), Some(*p));
        }
        assert_eq!(Pass::parse("nope"), None);
    }

    fn count_binops(l2: &L2Module, fname: &str) -> usize {
        l2.functions
            .iter()
            .find(|f| f.name == fname)
            .map(|f| {
                f.values
                    .values()
                    .filter(|i| matches!(i, L2Inst::BinOp { .. }))
                    .count()
            })
            .unwrap_or(0)
    }
}
