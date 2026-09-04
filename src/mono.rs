use crate::ast::*;
use std::collections::{HashMap, HashSet};

/// A substitution mapping type parameters to concrete types
type Subst = HashMap<String, Type>;

/// A substitution mapping const generic parameters to their integer values
type ConstSubst = HashMap<String, i64>;

/// Mangled name for a monomorphized function: `name__T1_T2`
fn mangle(name: &str, subst: &Subst, type_params: &[String]) -> String {
    let type_strs: Vec<String> = type_params.iter()
        .map(|tp| {
            let ty = subst.get(tp).cloned().unwrap_or_else(|| Type::Named(tp.clone()));
            mangle_type(&ty)
        })
        .collect();
    format!("{}__{}", name, type_strs.join("_"))
}

/// Mangled name for a monomorphized generic method: `StructName__methodName__T1_T2`
fn mangle_method(struct_name: &str, method_name: &str, subst: &Subst, type_params: &[String]) -> String {
    let type_strs: Vec<String> = type_params.iter()
        .map(|tp| {
            let ty = subst.get(tp).cloned().unwrap_or_else(|| Type::Named(tp.clone()));
            mangle_type(&ty)
        })
        .collect();
    format!("{}__{}__{}", struct_name, method_name, type_strs.join("_"))
}

/// Short type name for mangling
fn mangle_type(ty: &Type) -> String {
    match ty {
        Type::I8 => "i8".to_string(),
        Type::I16 => "i16".to_string(),
        Type::I32 => "i32".to_string(),
        Type::I64 => "i64".to_string(),
        Type::I128 => "i128".to_string(),
        Type::U8 => "u8".to_string(),
        Type::U16 => "u16".to_string(),
        Type::U32 => "u32".to_string(),
        Type::U64 => "u64".to_string(),
        Type::U128 => "u128".to_string(),
        Type::F32 => "f32".to_string(),
        Type::F64 => "f64".to_string(),
        Type::Bool => "bool".to_string(),
        Type::Str => "str".to_string(),
        Type::Void => "void".to_string(),
        Type::Never => "never".to_string(),
        Type::Infer => "infer".to_string(),
        Type::Named(s) => s.clone(),
        Type::Generic { name, args } => {
            let args_str: Vec<String> = args.iter().map(mangle_type).collect();
            format!("{}__{}", name, args_str.join("_"))
        }
        Type::Ref { inner, .. } => format!("ref_{}", mangle_type(inner)),
        Type::RawPtr { inner, is_mut } => format!("rawptr_{}_{}", if *is_mut { "mut" } else { "const" }, mangle_type(inner)),
        Type::Array { elem, .. } => format!("arr_{}", mangle_type(elem)),
        Type::ArrayConst { elem, size_param } => format!("arrc_{}_{}", mangle_type(elem), size_param),
        Type::Tuple(types) => {
            let parts: Vec<String> = types.iter().map(mangle_type).collect();
            format!("tup_{}", parts.join("_"))
        }
        Type::Func { params, ret } => {
            let p: Vec<String> = params.iter().map(mangle_type).collect();
            format!("fn_{}_{}", p.join("_"), mangle_type(ret))
        }
        Type::Assoc(_, trait_name, name, _) => format!("assoc_{}_{}", trait_name, name),
        Type::Dyn(name) => format!("dyn_{}", name),
        Type::ShapedTensor { elem, shape } => {
            let shape_str: Vec<String> = shape.iter().map(|d| match d {
                crate::ast::ShapeDim::Known(n) => n.to_string(),
                crate::ast::ShapeDim::Symbolic(s) => s.clone(),
            }).collect();
            format!("tensor_{}_{}", mangle_type(elem), shape_str.join("_"))
        }
        Type::RawPtr { inner, is_mut } => format!("rawptr_{}_{}", mangle_type(inner), if *is_mut { "mut" } else { "const" }),
    }
}

/// Apply a substitution to a type
fn subst_type(ty: &Type, subst: &Subst) -> Type {
    match ty {
        Type::Named(name) => {
            if let Some(replacement) = subst.get(name) {
                replacement.clone()
            } else {
                ty.clone()
            }
        }
        Type::Array { elem, size } => Type::Array {
            elem: Box::new(subst_type(elem, subst)),
            size: *size,
        },
        Type::ArrayConst { elem, size_param } => {
            // If the const param is in the substitution, resolve it to a concrete size
            if let Some(replacement) = subst.get(size_param) {
                if let Type::Named(ref name) = replacement {
                    if let Some(size_str) = name.strip_prefix("__const_") {
                        if let Ok(size) = size_str.parse::<usize>() {
                            return Type::Array {
                                elem: Box::new(subst_type(elem, subst)),
                                size,
                            };
                        }
                    }
                }
            }
            Type::ArrayConst {
                elem: Box::new(subst_type(elem, subst)),
                size_param: size_param.clone(),
            }
        }
        Type::Ref { inner, is_mut } => Type::Ref {
            inner: Box::new(subst_type(inner, subst)),
            is_mut: *is_mut,
        },
        Type::RawPtr { inner, is_mut } => Type::RawPtr {
            inner: Box::new(subst_type(inner, subst)),
            is_mut: *is_mut,
        },
        Type::Generic { name, args } => Type::Generic {
            name: name.clone(),
            args: args.iter().map(|a| subst_type(a, subst)).collect(),
        },
        Type::Tuple(types) => Type::Tuple(
            types.iter().map(|t| subst_type(t, subst)).collect()
        ),
        Type::Func { params, ret } => Type::Func {
            params: params.iter().map(|t| subst_type(t, subst)).collect(),
            ret: Box::new(subst_type(ret, subst)),
        },
        _ => ty.clone(),
    }
}

/// Substitute GAT type arguments into a resolved associated type.
/// E.g., if the impl binds `ty Item<T> = Vec<T>`, and the usage is `T::Item<i64>`,
/// this replaces T with i64 in Vec<T> → Vec<i64>.
fn subst_gat_type_args(ty: &Type, args: &[Type], subst: &Subst) -> Type {
    match ty {
        Type::Named(name) => {
            // Check if this name is a GAT type parameter (by position in args)
            // GAT type params are typically T, U, etc. — we match by name against
            // the impl's type params. For simplicity, if the name is a single uppercase
            // letter and matches a type param in subst, use the arg.
            // Actually, we need the GAT param names from the trait def.
            // For now, use positional substitution: first Named that's a type param → args[0], etc.
            ty.clone()
        }
        Type::Generic { name, args: inner_args } => {
            // e.g. Vec<T> → Vec<i64> if args[0] is T
            let new_args: Vec<Type> = inner_args.iter().map(|a| {
                if let Type::Named(n) = a {
                    // Try to find this name in the GAT type params
                    // For now, check if it's a single-char type param
                    if n.len() == 1 && n.chars().next().unwrap().is_uppercase() {
                        // Positional: find index
                        // This is a simplification — proper impl would track param names
                        return args.first().cloned().unwrap_or(a.clone());
                    }
                }
                subst_gat_type_args(a, args, subst)
            }).collect();
            Type::Generic { name: name.clone(), args: new_args }
        }
        Type::Array { elem, size } => Type::Array {
            elem: Box::new(subst_gat_type_args(elem, args, subst)),
            size: *size,
        },
        Type::ArrayConst { elem, size_param } => {
            // Check if size_param matches a GAT type arg
            if let Some(arg) = args.first() {
                if let Type::Named(n) = arg {
                    if n == size_param {
                        // Can't resolve to a concrete size here
                        return Type::ArrayConst {
                            elem: Box::new(subst_gat_type_args(elem, args, subst)),
                            size_param: size_param.clone(),
                        };
                    }
                }
            }
            Type::ArrayConst {
                elem: Box::new(subst_gat_type_args(elem, args, subst)),
                size_param: size_param.clone(),
            }
        }
        Type::Ref { inner, is_mut } => Type::Ref {
            inner: Box::new(subst_gat_type_args(inner, args, subst)),
            is_mut: *is_mut,
        },
        Type::RawPtr { inner, is_mut } => Type::RawPtr {
            inner: Box::new(subst_gat_type_args(inner, args, subst)),
            is_mut: *is_mut,
        },
        Type::Tuple(types) => Type::Tuple(
            types.iter().map(|t| subst_gat_type_args(t, args, subst)).collect()
        ),
        other => other.clone(),
    }
}

/// Resolve `Type::Assoc` in a type using the substitution and trait impl bindings.
fn resolve_assoc_types(ty: &Type, subst: &Subst, bindings: &HashMap<(String, String), HashMap<String, Type>>) -> Type {
    match ty {
        Type::Assoc(type_param, trait_name, assoc_name, type_args) => {
            if let Some(concrete) = subst.get(type_param) {
                let concrete_name = match concrete {
                    Type::Named(n) => n.clone(),
                    _ => return Type::I64,
                };
                // If trait_name is empty, search all bindings for this type+assoc_name
                if trait_name.is_empty() {
                    for ((tn, cn), assoc_map) in bindings {
                        if cn == &concrete_name {
                            if let Some(resolved) = assoc_map.get(assoc_name) {
                                if !type_args.is_empty() {
                                    return subst_gat_type_args(resolved, type_args, subst);
                                }
                                return resolved.clone();
                            }
                        }
                    }
                } else {
                    if let Some(assoc_map) = bindings.get(&(trait_name.clone(), concrete_name)) {
                        if let Some(resolved) = assoc_map.get(assoc_name) {
                            if !type_args.is_empty() {
                                return subst_gat_type_args(resolved, type_args, subst);
                            }
                            return resolved.clone();
                        }
                    }
                }
            }
            Type::I64
        }
        Type::Array { elem, size } => Type::Array {
            elem: Box::new(resolve_assoc_types(elem, subst, bindings)),
            size: *size,
        },
        Type::Ref { inner, is_mut } => Type::Ref {
            inner: Box::new(resolve_assoc_types(inner, subst, bindings)),
            is_mut: *is_mut,
        },
        Type::RawPtr { inner, is_mut } => Type::RawPtr {
            inner: Box::new(resolve_assoc_types(inner, subst, bindings)),
            is_mut: *is_mut,
        },
        Type::Generic { name, args } => Type::Generic {
            name: name.clone(),
            args: args.iter().map(|a| resolve_assoc_types(a, subst, bindings)).collect(),
        },
        Type::Tuple(types) => Type::Tuple(
            types.iter().map(|t| resolve_assoc_types(t, subst, bindings)).collect()
        ),
        Type::Func { params, ret } => Type::Func {
            params: params.iter().map(|t| resolve_assoc_types(t, subst, bindings)).collect(),
            ret: Box::new(resolve_assoc_types(ret, subst, bindings)),
        },
        _ => ty.clone(),
    }
}

/// Apply substitution to an expression (rewrites call sites + type annotations)
fn subst_expr(expr: &Expr, subst: &Subst, renames: &HashMap<String, String>, method_renames: &HashMap<String, String>) -> Expr {
    match expr {
        Expr::Call { callee, args } => {
            let new_args: Vec<Expr> = args.iter()
                .map(|a| subst_expr(a, subst, renames, method_renames))
                .collect();
            let new_callee = renames.get(callee).cloned().unwrap_or_else(|| callee.clone());
            Expr::Call { callee: new_callee, args: new_args }
        }
        Expr::Ident(s) => {
            // Check if this is a const generic param being substituted
            if let Some(replacement) = subst.get(s) {
                if let Type::Named(ref name) = replacement {
                    if let Some(val_str) = name.strip_prefix("__const_") {
                        if let Ok(val) = val_str.parse::<i64>() {
                            return Expr::IntLit(val);
                        }
                    }
                }
            }
            Expr::Ident(s.clone())
        }
        Expr::IntLit(n) => Expr::IntLit(*n),
        Expr::FloatLit(f) => Expr::FloatLit(*f),
        Expr::BoolLit(b) => Expr::BoolLit(*b),
        Expr::StringLit(s) => Expr::StringLit(s.clone()),
        Expr::Binary { op, left, right } => Expr::Binary {
            op: op.clone(),
            left: Box::new(subst_expr(left, subst, renames, method_renames)),
            right: Box::new(subst_expr(right, subst, renames, method_renames)),
        },
        Expr::Unary { op, expr: operand } => Expr::Unary {
            op: op.clone(),
            expr: Box::new(subst_expr(operand, subst, renames, method_renames)),
        },
        Expr::If { cond, then_block, else_block } => Expr::If {
            cond: Box::new(subst_expr(cond, subst, renames, method_renames)),
            then_block: then_block.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            else_block: else_block.as_ref().map(|eb| eb.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect()),
        },
        Expr::Block(stmts) => Expr::Block(
            stmts.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect()
        ),
        Expr::FieldAccess { expr, field } => Expr::FieldAccess {
            expr: Box::new(subst_expr(expr, subst, renames, method_renames)),
            field: field.clone(),
        },
        Expr::OptionalFieldAccess { expr, field } => Expr::OptionalFieldAccess {
            expr: Box::new(subst_expr(expr, subst, renames, method_renames)),
            field: field.clone(),
        },
        Expr::MethodCall { receiver, method, args } => {
            // Check if this method call should be rewritten to a monomorphized function call
            if let Some(mangled) = method_renames.get(method) {
                // Rewrite to a regular Call with receiver as first arg
                let mut all_args = vec![subst_expr(receiver, subst, renames, method_renames)];
                all_args.extend(args.iter().map(|a| subst_expr(a, subst, renames, method_renames)));
                Expr::Call { callee: mangled.clone(), args: all_args }
            } else {
                Expr::MethodCall {
                    receiver: Box::new(subst_expr(receiver, subst, renames, method_renames)),
                    method: method.clone(),
                    args: args.iter().map(|a| subst_expr(a, subst, renames, method_renames)).collect(),
                }
            }
        },
        Expr::Index { array, index } => Expr::Index {
            array: Box::new(subst_expr(array, subst, renames, method_renames)),
            index: Box::new(subst_expr(index, subst, renames, method_renames)),
        },
        Expr::ArrayLit { elem_type, elements } => Expr::ArrayLit {
            elem_type: Box::new(subst_type(elem_type, subst)),
            elements: elements.iter().map(|e| subst_expr(e, subst, renames, method_renames)).collect(),
        },
        Expr::Range { start, end, inclusive } => Expr::Range {
            start: Box::new(subst_expr(start, subst, renames, method_renames)),
            end: Box::new(subst_expr(end, subst, renames, method_renames)),
            inclusive: *inclusive,
        },
        Expr::StructLit { name, fields, spread } => Expr::StructLit {
            name: name.clone(),
            fields: fields.iter().map(|(n, e)| {
                (n.clone(), subst_expr(e, subst, renames, method_renames))
            }).collect(),
            spread: spread.as_ref().map(|e| Box::new(subst_expr(e, subst, renames, method_renames))),
        },
        Expr::Match { expr, arms } => Expr::Match {
            expr: Box::new(subst_expr(expr, subst, renames, method_renames)),
            arms: arms.iter().map(|a| MatchArm {
                pattern: a.pattern.clone(),
                guard: a.guard.as_ref().map(|g| subst_expr(g, subst, renames, method_renames)),
                body: a.body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            }).collect(),
        },
        Expr::Try { expr } => Expr::Try {
            expr: Box::new(subst_expr(expr, subst, renames, method_renames)),
        },
        Expr::Coalesce { expr, default } => Expr::Coalesce {
            expr: Box::new(subst_expr(expr, subst, renames, method_renames)),
            default: Box::new(subst_expr(default, subst, renames, method_renames)),
        },
        Expr::IsCheck { expr, type_name } => Expr::IsCheck {
            expr: Box::new(subst_expr(expr, subst, renames, method_renames)),
            type_name: type_name.clone(),
        },
        Expr::TryBlock { body } => Expr::TryBlock {
            body: body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
        },
        Expr::Closure { params, body, ret_ty } => Expr::Closure {
            params: params.iter().map(|p| Param {
                name: p.name.clone(),
                ty: subst_type(&p.ty, subst),
                is_pub: p.is_pub,
                annotations: p.annotations.clone(),
            }).collect(),
            body: body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            ret_ty: ret_ty.as_ref().map(|t| subst_type(t, subst)),
        },
        Expr::FString { parts } => Expr::FString {
            parts: parts.iter().map(|p| match p {
                FStringPart::Literal(s) => FStringPart::Literal(s.clone()),
                FStringPart::Expr(e) => FStringPart::Expr(subst_expr(e, subst, renames, method_renames)),
            }).collect(),
        },
        Expr::Cast { expr, target_ty } => Expr::Cast {
            expr: Box::new(subst_expr(expr, subst, renames, method_renames)),
            target_ty: subst_type(target_ty, subst),
        },
        Expr::Asm { template, operands } => {
            let new_operands: Vec<crate::ast::AsmOperand> = operands.iter()
                .map(|op| crate::ast::AsmOperand {
                    direction: op.direction.clone(),
                    reg: op.reg.clone(),
                    expr: subst_expr(&op.expr, subst, renames, method_renames),
                })
                .collect();
            Expr::Asm {
                template: template.clone(),
                operands: new_operands,
            }
        },
        Expr::Ref { expr, is_mut } => Expr::Ref {
            expr: Box::new(subst_expr(expr, subst, renames, method_renames)),
            is_mut: *is_mut,
        },
        Expr::Deref { expr } => Expr::Deref {
            expr: Box::new(subst_expr(expr, subst, renames, method_renames)),
        },
        Expr::StrConcat { left, right } => Expr::StrConcat {
            left: Box::new(subst_expr(left, subst, renames, method_renames)),
            right: Box::new(subst_expr(right, subst, renames, method_renames)),
        },
        Expr::TupleLit(exprs) => Expr::TupleLit(
            exprs.iter().map(|e| subst_expr(e, subst, renames, method_renames)).collect()
        ),
        Expr::EnumLit { enum_name, variant, args } => Expr::EnumLit {
            enum_name: enum_name.clone(),
            variant: variant.clone(),
            args: args.iter().map(|a| subst_expr(a, subst, renames, method_renames)).collect(),
        },
        Expr::Asm { .. } => expr.clone(),
        Expr::Comptime(inner) => Expr::Comptime(Box::new(subst_expr(inner, subst, renames, method_renames))),
        Expr::ComptimeBlock(stmts) => Expr::ComptimeBlock(
            stmts.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect()
        ),
        Expr::ComptimeReflect { op, ty } => Expr::ComptimeReflect {
            op: op.clone(),
            ty: subst_type(ty, subst),
        },
        Expr::PipeForward { left, right } => Expr::PipeForward {
            left: Box::new(subst_expr(left, subst, renames, method_renames)),
            right: Box::new(subst_expr(right, subst, renames, method_renames)),
        },
        Expr::QuantCast { expr, target_ty } => Expr::QuantCast {
            expr: Box::new(subst_expr(expr, subst, renames, method_renames)),
            target_ty: subst_type(target_ty, subst),
        },
        Expr::Await { expr: inner } => Expr::Await {
            expr: Box::new(subst_expr(inner, subst, renames, method_renames)),
        },
        Expr::Spawn { expr: inner } => Expr::Spawn {
            expr: Box::new(subst_expr(inner, subst, renames, method_renames)),
        },
    }
}

/// Apply substitution to a statement
fn subst_stmt(stmt: &Stmt, subst: &Subst, renames: &HashMap<String, String>, method_renames: &HashMap<String, String>) -> Stmt {
    match stmt {
        Stmt::Let { name, ty, is_mut, is_persist, expr, line } => Stmt::Let {
            name: name.clone(),
            ty: ty.as_ref().map(|t| subst_type(t, subst)),
            is_mut: *is_mut,
            is_persist: *is_persist,
            expr: subst_expr(expr, subst, renames, method_renames),
            line: *line,
        },
        Stmt::LetUninit { name, ty, line } => Stmt::LetUninit {
            name: name.clone(),
            ty: subst_type(ty, subst),
            line: *line,
        },
        Stmt::LetTuple { names, expr, line } => Stmt::LetTuple {
            names: names.clone(),
            expr: subst_expr(expr, subst, renames, method_renames),
            line: *line,
        },
        Stmt::Assign { target, op, expr, line } => Stmt::Assign {
            target: target.clone(),
            op: op.clone(),
            expr: subst_expr(expr, subst, renames, method_renames),
            line: *line,
        },
        Stmt::IndexAssign { array, index, op, expr, line } => Stmt::IndexAssign {
            array: array.clone(),
            index: subst_expr(index, subst, renames, method_renames),
            op: op.clone(),
            expr: subst_expr(expr, subst, renames, method_renames),
            line: *line,
        },
        Stmt::DerefAssign { ptr_expr, op, expr, line } => Stmt::DerefAssign {
            ptr_expr: subst_expr(ptr_expr, subst, renames, method_renames),
            op: op.clone(),
            expr: subst_expr(expr, subst, renames, method_renames),
            line: *line,
        },
        Stmt::FieldAssign { object, field, op, expr, line } => Stmt::FieldAssign {
            object: subst_expr(object, subst, renames, method_renames),
            field: field.clone(),
            op: op.clone(),
            expr: subst_expr(expr, subst, renames, method_renames),
            line: *line,
        },
        Stmt::Expr(e, line) => Stmt::Expr(subst_expr(e, subst, renames, method_renames), *line),
        Stmt::Return(e, line) => Stmt::Return(e.as_ref().map(|e| subst_expr(e, subst, renames, method_renames)), *line),
        Stmt::If { cond, then_block, else_block, line } => Stmt::If {
            cond: subst_expr(cond, subst, renames, method_renames),
            then_block: then_block.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            else_block: else_block.as_ref().map(|eb| eb.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect()),
            line: *line,
        },
        Stmt::While { cond, body, line } => Stmt::While {
            cond: subst_expr(cond, subst, renames, method_renames),
            body: body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            line: *line,
        },
        Stmt::For { var, iterator, body, line } => Stmt::For {
            var: var.clone(),
            iterator: subst_expr(iterator, subst, renames, method_renames),
            body: body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            line: *line,
        },
        Stmt::Loop { body, line } => Stmt::Loop {
            body: body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            line: *line,
        },
        Stmt::Break(e, line) => Stmt::Break(e.as_ref().map(|e| subst_expr(e, subst, renames, method_renames)), *line),
        Stmt::Continue(line) => Stmt::Continue(*line),
        Stmt::Defer(body, line) => Stmt::Defer(
            body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            *line,
        ),
        Stmt::Match { expr, arms, line } => Stmt::Match {
            expr: subst_expr(expr, subst, renames, method_renames),
            arms: arms.iter().map(|a| MatchArm {
                pattern: a.pattern.clone(),
                guard: a.guard.as_ref().map(|g| subst_expr(g, subst, renames, method_renames)),
                body: a.body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            }).collect(),
            line: *line,
        },
        Stmt::Use { path, items, line } => Stmt::Use {
            path: path.clone(),
            items: items.clone(),
            line: *line,
        },
        Stmt::Handle { body, handlers, line } => Stmt::Handle {
            body: body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            handlers: handlers.iter().map(|h| EffectHandler {
                effect: h.effect.clone(),
                op: h.op.clone(),
                params: h.params.clone(),
                body: h.body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            }).collect(),
            line: *line,
        },
        Stmt::Do { effect, op, args, line } => Stmt::Do {
            effect: effect.clone(),
            op: op.clone(),
            args: args.iter().map(|a| subst_expr(a, subst, renames, method_renames)).collect(),
            line: *line,
        },
        Stmt::RegionBlock { name, budget, is_gpu, is_kv_cache, window_size, body, line } => Stmt::RegionBlock {
            name: name.clone(),
            budget: *budget,
            is_gpu: *is_gpu,
            is_kv_cache: *is_kv_cache,
            window_size: *window_size,
            body: body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            line: *line,
        },
        Stmt::ComptimeGen { .. } => stmt.clone(),
        Stmt::Sandbox { body, line } => Stmt::Sandbox {
            body: body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            line: *line,
        },
        Stmt::Unsafe { body, line } => Stmt::Unsafe {
            body: body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            line: *line,
        },
        Stmt::Txn { body, line } => Stmt::Txn {
            body: body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            line: *line,
        },
        Stmt::GradBlock { var, body, line } => Stmt::GradBlock {
            var: var.clone(),
            body: body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            line: *line,
        },
        Stmt::Backward { loss, line } => Stmt::Backward {
            loss: subst_expr(loss, subst, renames, method_renames),
            line: *line,
        },
        Stmt::Query { components, body, line } => Stmt::Query {
            components: components.clone(),
            body: body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            line: *line,
        },
        Stmt::Select { cases, default, line } => Stmt::Select {
            cases: cases.iter().map(|c| SelectCase {
                channel: subst_expr(&c.channel, subst, renames, method_renames),
                var_name: c.var_name.clone(),
                body: c.body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            }).collect(),
            default: default.as_ref().map(|d| d.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect()),
            line: *line,
        },
    }
}

/// Infer type substitution from a call site by matching arg types to param types
fn infer_subst(
    param_types: &[Type],
    arg_types: &[Type],
    type_params: &[String],
) -> Subst {
    let mut subst: Subst = HashMap::new();
    let tp_set: HashSet<&String> = type_params.iter().collect();

    for (param, arg) in param_types.iter().zip(arg_types.iter()) {
        infer_from_pair(param, arg, &tp_set, &mut subst);
    }
    subst
}

/// Recursively infer type parameter bindings from a (param_type, arg_type) pair
fn infer_from_pair(param: &Type, arg: &Type, tp_set: &HashSet<&String>, subst: &mut Subst) {
    match param {
        Type::Named(name) if tp_set.contains(name) => {
            subst.insert(name.clone(), arg.clone());
        }
        Type::ArrayConst { elem, size_param } if tp_set.contains(size_param) => {
            // Const generic: bind N to the argument's array type (which carries the size)
            if let Type::Array { elem: arg_elem, size } = arg {
                // Store the size as a Named type with the size encoded: __const_3
                subst.insert(size_param.clone(), Type::Named(format!("__const_{}", size)));
                infer_from_pair(elem, arg_elem, tp_set, subst);
            }
        }
        Type::Array { elem, .. } => {
            if let Type::Array { elem: arg_elem, .. } = arg {
                infer_from_pair(elem, arg_elem, tp_set, subst);
            }
        }
        Type::Ref { inner, .. } => {
            if let Type::Ref { inner: arg_inner, .. } = arg {
                infer_from_pair(inner, arg_inner, tp_set, subst);
            }
        }
        Type::Tuple(types) => {
            if let Type::Tuple(arg_types) = arg {
                for (p, a) in types.iter().zip(arg_types.iter()) {
                    infer_from_pair(p, a, tp_set, subst);
                }
            }
        }
        Type::Func { params, ret } => {
            if let Type::Func { params: arg_params, ret: arg_ret } = arg {
                for (p, a) in params.iter().zip(arg_params.iter()) {
                    infer_from_pair(p, a, tp_set, subst);
                }
                infer_from_pair(ret, arg_ret, tp_set, subst);
            }
        }
        _ => {}
    }
}

/// Monomorphization context
struct MonoCtx {
    /// Generic functions: name → (Func, type_params)
    generics: HashMap<String, (Func, Vec<String>)>,
    /// Generic impl methods: (struct_name, method_name) → (Func, type_params)
    generic_methods: HashMap<(String, String), (Func, Vec<String>)>,
    /// Instantiated functions to add to the program
    instantiated: Vec<Func>,
    /// Track which instantiations we've already done
    seen: HashSet<String>,
    /// Trait impl associated type bindings: (trait_name, type_name) -> {assoc_name -> concrete_type}
    trait_assoc_bindings: HashMap<(String, String), HashMap<String, Type>>,
}

/// Main monomorphization entry point
pub fn monomorphize(program: &Program) -> Program {
    // Separate generic and non-generic functions
    let mut generics: HashMap<String, (Func, Vec<String>)> = HashMap::new();
    let mut concrete_funcs: Vec<Func> = Vec::new();

    for func in &program.funcs {
        if func.type_params.is_empty() {
            concrete_funcs.push(func.clone());
        } else {
            generics.insert(func.name.clone(), (func.clone(), func.type_params.clone()));
        }
    }

    let mut ctx = MonoCtx {
        generics,
        generic_methods: HashMap::new(),
        instantiated: Vec::new(),
        seen: HashSet::new(),
        trait_assoc_bindings: HashMap::new(),
    };

    // Register trait impl associated type bindings
    for ti in &program.trait_impls {
        if !ti.associated_types.is_empty() {
            let key = (ti.trait_name.clone(), ti.type_name.clone());
            let bindings: HashMap<String, Type> = ti.associated_types.iter().cloned().collect();
            ctx.trait_assoc_bindings.insert(key, bindings);
        }
    }

    // Register generic impl methods BEFORE processing any functions
    for imp in &program.impls {
        if !imp.type_params.is_empty() {
            for method in &imp.methods {
                ctx.generic_methods.insert(
                    (imp.struct_name.clone(), method.name.clone()),
                    (method.clone(), imp.type_params.clone()),
                );
            }
        }
    }

    // Register generic trait impl methods
    for ti in &program.trait_impls {
        if !ti.type_params.is_empty() {
            for method in &ti.methods {
                ctx.generic_methods.insert(
                    (ti.type_name.clone(), method.name.clone()),
                    (method.clone(), ti.type_params.clone()),
                );
            }
        }
    }

    // Process concrete functions — find calls to generics and instantiate
    let mut new_funcs = Vec::new();
    for func in &concrete_funcs {
        let new_func = process_func(func, &mut ctx);
        new_funcs.push(new_func);
    }

    // Process impl methods — separate generic from non-generic
    let mut new_impls = Vec::new();
    for imp in &program.impls {
        let mut new_methods = Vec::new();
        for method in &imp.methods {
            // Skip generic methods — they'll be instantiated on demand
            if !imp.type_params.is_empty() {
                continue;
            }
            let new_method = process_func(method, &mut ctx);
            new_methods.push(new_method);
        }
        new_impls.push(ImplDef {
            struct_name: imp.struct_name.clone(),
            type_params: Vec::new(),
            methods: new_methods,
        });
    }

    // Process trait impl methods — separate generic from non-generic
    let mut new_trait_impls = Vec::new();
    for ti in &program.trait_impls {
        let mut new_methods = Vec::new();
        for method in &ti.methods {
            if !ti.type_params.is_empty() {
                continue;
            }
            let new_method = process_func(method, &mut ctx);
            new_methods.push(new_method);
        }
        new_trait_impls.push(TraitImpl {
            trait_name: ti.trait_name.clone(),
            type_name: ti.type_name.clone(),
            type_params: Vec::new(),
            methods: new_methods,
            associated_types: ti.associated_types.clone(),
        });
    }

    // Add all instantiated functions
    new_funcs.extend(ctx.instantiated);

    Program {
        module: program.module.clone(),
        structs: program.structs.clone(),
        enums: program.enums.clone(),
        funcs: new_funcs,
        consts: program.consts.clone(),
        globals: program.globals.clone(),
        type_aliases: program.type_aliases.clone(),
        impls: new_impls,
        imports: program.imports.clone(),
        attributes: program.attributes.clone(),
        traits: program.traits.clone(),
        trait_impls: new_trait_impls,
        extern_decls: program.extern_decls.clone(),
        layers: program.layers.clone(),
        models: program.models.clone(),
        components: program.components.clone(),
        systems: program.systems.clone(),
        state_machines: program.state_machines.clone(),
        event_handlers: program.event_handlers.clone(),
    }
}

/// Fix up call sites for generic functions that may have multiple instantiations.
/// For each call to a generic, re-infer the substitution from argument types
/// and use the correct mangled name.
fn fixup_generic_calls(stmts: &[Stmt], generics: &HashMap<String, (Func, Vec<String>)>, current_func: &Func) -> Vec<Stmt> {
    stmts.iter().map(|s| fixup_generic_calls_stmt(s, generics, current_func)).collect()
}

fn fixup_generic_calls_stmt(stmt: &Stmt, generics: &HashMap<String, (Func, Vec<String>)>, current_func: &Func) -> Stmt {
    match stmt {
        Stmt::Expr(e, line) => Stmt::Expr(fixup_generic_calls_expr(e, generics, current_func), *line),
        Stmt::Let { name, ty, expr, is_mut, is_persist, line } => Stmt::Let {
            name: name.clone(), ty: ty.clone(),
            expr: fixup_generic_calls_expr(expr, generics, current_func),
            is_mut: *is_mut, is_persist: *is_persist, line: *line,
        },
        Stmt::LetTuple { names, expr, line } => Stmt::LetTuple {
            names: names.clone(),
            expr: fixup_generic_calls_expr(expr, generics, current_func),
            line: *line,
        },
        Stmt::Return(opt, line) => Stmt::Return(
            opt.as_ref().map(|e| fixup_generic_calls_expr(e, generics, current_func)),
            *line,
        ),
        Stmt::If { cond, then_block, else_block, line } => Stmt::If {
            cond: fixup_generic_calls_expr(cond, generics, current_func),
            then_block: fixup_generic_calls(then_block, generics, current_func),
            else_block: else_block.as_ref().map(|eb| fixup_generic_calls(eb, generics, current_func)),
            line: *line,
        },
        Stmt::While { cond, body, line } => Stmt::While {
            cond: fixup_generic_calls_expr(cond, generics, current_func),
            body: fixup_generic_calls(body, generics, current_func),
            line: *line,
        },
        Stmt::For { var, iterator, body, line } => Stmt::For {
            var: var.clone(),
            iterator: fixup_generic_calls_expr(iterator, generics, current_func),
            body: fixup_generic_calls(body, generics, current_func),
            line: *line,
        },
        Stmt::Assign { target, op, expr, line } => Stmt::Assign {
            target: target.clone(), op: op.clone(),
            expr: fixup_generic_calls_expr(expr, generics, current_func),
            line: *line,
        },
        other => other.clone(),
    }
}

fn fixup_generic_calls_expr(expr: &Expr, generics: &HashMap<String, (Func, Vec<String>)>, current_func: &Func) -> Expr {
    match expr {
        Expr::Call { callee, args } => {
            // First, recursively fix up args
            let new_args: Vec<Expr> = args.iter()
                .map(|a| fixup_generic_calls_expr(a, generics, current_func))
                .collect();

            // Check if this is a call to a generic function
            if let Some((gen_func, type_params)) = generics.get(callee) {
                let arg_types: Vec<Type> = new_args.iter()
                    .map(|a| infer_expr_type(a, current_func))
                    .collect();
                let param_types: Vec<Type> = gen_func.params.iter().map(|p| p.ty.clone()).collect();
                let subst = infer_subst(&param_types, &arg_types, type_params);
                let mangled = mangle(callee, &subst, type_params);
                return Expr::Call { callee: mangled, args: new_args };
            }

            Expr::Call { callee: callee.clone(), args: new_args }
        }
        Expr::Binary { op, left, right } => Expr::Binary {
            op: op.clone(),
            left: Box::new(fixup_generic_calls_expr(left, generics, current_func)),
            right: Box::new(fixup_generic_calls_expr(right, generics, current_func)),
        },
        Expr::Unary { op, expr: inner } => Expr::Unary {
            op: op.clone(),
            expr: Box::new(fixup_generic_calls_expr(inner, generics, current_func)),
        },
        Expr::MethodCall { receiver, method, args } => Expr::MethodCall {
            receiver: Box::new(fixup_generic_calls_expr(receiver, generics, current_func)),
            method: method.clone(),
            args: args.iter().map(|a| fixup_generic_calls_expr(a, generics, current_func)).collect(),
        },
        Expr::FieldAccess { expr: inner, field } => Expr::FieldAccess {
            expr: Box::new(fixup_generic_calls_expr(inner, generics, current_func)),
            field: field.clone(),
        },
        Expr::Index { array, index } => Expr::Index {
            array: Box::new(fixup_generic_calls_expr(array, generics, current_func)),
            index: Box::new(fixup_generic_calls_expr(index, generics, current_func)),
        },
        Expr::If { cond, then_block, else_block } => Expr::If {
            cond: Box::new(fixup_generic_calls_expr(cond, generics, current_func)),
            then_block: fixup_generic_calls(then_block, generics, current_func),
            else_block: else_block.as_ref().map(|eb| fixup_generic_calls(eb, generics, current_func)),
        },
        Expr::Block(stmts) => Expr::Block(fixup_generic_calls(stmts, generics, current_func)),
        other => other.clone(),
    }
}

/// Process a function: find calls to generics, instantiate them, rewrite call sites.
/// Also recursively processes instantiated functions to handle nested generic calls.
fn process_func(func: &Func, ctx: &mut MonoCtx) -> Func {
    // Collect all calls to generic functions in this function's body
    let mut calls_to_instantiate: Vec<(String, Vec<Type>)> = Vec::new();
    // Collect all calls to generic methods: (struct_name, method_name, arg_types)
    let mut method_calls_to_instantiate: Vec<(String, String, Vec<Type>)> = Vec::new();

    for stmt in &func.body {
        collect_generic_calls_stmt(stmt, &ctx.generics, &ctx.generic_methods, &mut calls_to_instantiate, &mut method_calls_to_instantiate, func);
    }

    // Instantiate each generic function call
    let mut local_renames: HashMap<String, String> = HashMap::new();
    for (callee, arg_types) in &calls_to_instantiate {
        // Clone the generic function data to avoid borrowing ctx while we need &mut ctx later
        let generic_info = ctx.generics.get(callee).cloned();
        if let Some((gen_func, type_params)) = generic_info {
            let param_types: Vec<Type> = gen_func.params.iter().map(|p| p.ty.clone()).collect();
            let subst = infer_subst(&param_types, arg_types, &type_params);

            // Check that all type params are bound
            let all_bound = type_params.iter().all(|tp| subst.contains_key(tp));
            if !all_bound {
                continue;
            }

            let mangled = mangle(callee, &subst, &type_params);

            // Only instantiate once
            if ctx.seen.insert(mangled.clone()) {
                let instantiated = instantiate(&gen_func, &subst, &mangled, ctx);
                ctx.instantiated.push(instantiated);
            }

            local_renames.insert(callee.clone(), mangled);
        }
    }

    // Instantiate each generic method call
    let mut method_renames: HashMap<String, String> = HashMap::new();
    for (struct_name, method_name, arg_types) in &method_calls_to_instantiate {
        let key = (struct_name.clone(), method_name.clone());
        let generic_info = ctx.generic_methods.get(&key).cloned();
        if let Some((gen_func, type_params)) = generic_info {
            // For methods, the first param is `self` — skip it when inferring
            let param_types: Vec<Type> = gen_func.params.iter()
                .skip(1) // skip self
                .map(|p| p.ty.clone())
                .collect();
            let subst = infer_subst(&param_types, arg_types, &type_params);

            // Bind any unbound type params to a placeholder (they're not used in this method)
            let mut subst = subst;
            for tp in &type_params {
                if !subst.contains_key(tp) {
                    subst.insert(tp.clone(), Type::I32);
                }
            }

            // Mangle as StructName__methodName__T
            let mangled = mangle_method(struct_name, method_name, &subst, &type_params);

            // Only instantiate once
            if ctx.seen.insert(mangled.clone()) {
                let instantiated = instantiate(&gen_func, &subst, &mangled, ctx);
                ctx.instantiated.push(instantiated);
            }

            method_renames.insert(method_name.clone(), mangled);
        }
    }

    // Fix up call sites for generics with multiple instantiations.
    // We need to run fixup on the ORIGINAL body (before subst_stmt renamed
    // callees), then apply substitution for types/exprs without the renames.
    let fixed_body = fixup_generic_calls(&func.body, &ctx.generics, func);
    // Now apply type substitution (but no call renames since fixup already did them)
    let new_body: Vec<Stmt> = fixed_body.iter()
        .map(|s| subst_stmt(s, &HashMap::new(), &HashMap::new(), &method_renames))
        .collect();

    Func {
        name: func.name.clone(),
        type_params: Vec::new(),
        type_param_bounds: Vec::new(),
        const_params: Vec::new(),
        params: func.params.clone(),
        ret_ty: func.ret_ty.clone(),
        body: new_body,
        is_pub: func.is_pub,
        is_comptime: false,
        declared_effects: func.declared_effects.clone(),
        has_effect_annotation: func.has_effect_annotation,
        is_interrupt: func.is_interrupt,
            annotations: func.annotations.clone(),
        preconditions: func.preconditions.clone(),
        postconditions: func.postconditions.clone(),
        is_async: func.is_async,
    }
}

/// Instantiate a generic function with a concrete substitution.
/// Recursively processes the instantiated body to handle nested generic calls.
fn instantiate(gen_func: &Func, subst: &Subst, mangled_name: &str, ctx: &mut MonoCtx) -> Func {
    // Apply substitution to params, then resolve associated types
    let new_params: Vec<Param> = gen_func.params.iter().map(|p| {
        let substituted = subst_type(&p.ty, subst);
        let resolved = resolve_assoc_types(&substituted, subst, &ctx.trait_assoc_bindings);
        Param { name: p.name.clone(), ty: resolved, is_pub: p.is_pub, annotations: p.annotations.clone() }
    }).collect();

    let substituted_ret = subst_type(&gen_func.ret_ty, subst);
    let new_ret_ty = resolve_assoc_types(&substituted_ret, subst, &ctx.trait_assoc_bindings);

    // First, apply the type substitution to the body
    let subst_body: Vec<Stmt> = gen_func.body.iter()
        .map(|s| subst_stmt(s, subst, &HashMap::new(), &HashMap::new()))
        .collect();

    // Create a temporary concrete function with the substituted body
    let temp_func = Func {
        name: mangled_name.to_string(),
        type_params: Vec::new(),
        type_param_bounds: Vec::new(),
        const_params: Vec::new(),
        params: new_params,
        ret_ty: new_ret_ty.clone(),
        body: subst_body,
        is_pub: gen_func.is_pub,
        is_comptime: false,
        declared_effects: gen_func.declared_effects.clone(),
        has_effect_annotation: gen_func.has_effect_annotation,
        is_interrupt: gen_func.is_interrupt,
            annotations: gen_func.annotations.clone(),
        preconditions: gen_func.preconditions.clone(),
        postconditions: gen_func.postconditions.clone(),
        is_async: gen_func.is_async,
    };

    // Now process this concrete function — it may contain calls to other generics
    // which need to be instantiated and have their call sites rewritten.
    // process_func will handle nested generic calls recursively.
    process_func(&temp_func, ctx)
}

/// Walk statements to find calls to generic functions and infer arg types
fn collect_generic_calls_stmt(
    stmt: &Stmt,
    generics: &HashMap<String, (Func, Vec<String>)>,
    generic_methods: &HashMap<(String, String), (Func, Vec<String>)>,
    calls: &mut Vec<(String, Vec<Type>)>,
    method_calls: &mut Vec<(String, String, Vec<Type>)>,
    current_func: &Func,
) {
    collect_generic_calls_stmt_with_hint(stmt, generics, generic_methods, calls, method_calls, current_func, None)
}

/// Walk statements with an optional expected type hint for context-based inference
fn collect_generic_calls_stmt_with_hint(
    stmt: &Stmt,
    generics: &HashMap<String, (Func, Vec<String>)>,
    generic_methods: &HashMap<(String, String), (Func, Vec<String>)>,
    calls: &mut Vec<(String, Vec<Type>)>,
    method_calls: &mut Vec<(String, String, Vec<Type>)>,
    current_func: &Func,
    expected_ty: Option<&Type>,
) {
    match stmt {
        Stmt::Let { name, ty, expr, .. } => {
            // Pass declared type as hint for context-based inference
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, ty.as_ref());
            // Register this let binding's type for later lookups
            let _ = name;
        }
        Stmt::LetUninit { .. } => {} // No expression to analyze
        Stmt::LetTuple { expr, .. } => {
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Stmt::Assign { expr, .. } => collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None),
        Stmt::IndexAssign { index, expr, .. } => {
            collect_generic_calls_expr_with_hint(index, generics, generic_methods, calls, method_calls, current_func, None);
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Stmt::DerefAssign { ptr_expr, expr, .. } => {
            collect_generic_calls_expr_with_hint(ptr_expr, generics, generic_methods, calls, method_calls, current_func, None);
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Stmt::FieldAssign { object, expr, .. } => {
            collect_generic_calls_expr_with_hint(object, generics, generic_methods, calls, method_calls, current_func, None);
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Stmt::Expr(e, _) => collect_generic_calls_expr_with_hint(e, generics, generic_methods, calls, method_calls, current_func, expected_ty),
        Stmt::Return(e, _) => {
            if let Some(e) = e {
                // Use the function's return type as hint
                let ret_hint = Some(&current_func.ret_ty);
                collect_generic_calls_expr_with_hint(e, generics, generic_methods, calls, method_calls, current_func, ret_hint);
            }
        }
        Stmt::If { cond, then_block, else_block, .. } => {
            collect_generic_calls_expr_with_hint(cond, generics, generic_methods, calls, method_calls, current_func, None);
            for s in then_block {
                collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
            }
            if let Some(eb) = else_block {
                for s in eb {
                    collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
                }
            }
        }
        Stmt::While { cond, body, .. } => {
            collect_generic_calls_expr_with_hint(cond, generics, generic_methods, calls, method_calls, current_func, None);
            for s in body {
                collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Stmt::For { iterator, body, .. } => {
            collect_generic_calls_expr_with_hint(iterator, generics, generic_methods, calls, method_calls, current_func, None);
            for s in body {
                collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Stmt::Loop { body, .. } => {
            for s in body {
                collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Stmt::Break(e, _) => {
            if let Some(e) = e {
                collect_generic_calls_expr_with_hint(e, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Stmt::Continue(_) => {}
        Stmt::Defer(body, _) => {
            for s in body {
                collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Stmt::Match { expr, arms, .. } => {
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None);
            for arm in arms {
                for s in &arm.body {
                    collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
                }
            }
        }
        Stmt::Use { .. } => {}
        Stmt::Handle { body, handlers, .. } => {
            for s in body {
                collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
            }
            for handler in handlers {
                for s in &handler.body {
                    collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
                }
            }
        }
        Stmt::Do { args, .. } => {
            for arg in args {
                collect_generic_calls_expr_with_hint(arg, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Stmt::RegionBlock { body, .. } => {
            for s in body {
                collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Stmt::ComptimeGen { .. } => {}
        Stmt::Sandbox { body, .. } => {
            for s in body {
                collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Stmt::Unsafe { body, .. } | Stmt::Txn { body, .. } => {
            for s in body {
                collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Stmt::GradBlock { body, .. } => {
            for s in body {
                collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Stmt::Backward { loss, .. } => {
            collect_generic_calls_expr_with_hint(loss, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Stmt::Query { body, .. } => {
            for s in body {
                collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Stmt::Select { cases, default, .. } => {
            for case in cases {
                collect_generic_calls_expr_with_hint(&case.channel, generics, generic_methods, calls, method_calls, current_func, None);
                for s in &case.body {
                    collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
                }
            }
            if let Some(ref def) = default {
                for s in def {
                    collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
                }
            }
        }
    }
}
fn collect_generic_calls_expr_with_hint(
    expr: &Expr,
    generics: &HashMap<String, (Func, Vec<String>)>,
    generic_methods: &HashMap<(String, String), (Func, Vec<String>)>,
    calls: &mut Vec<(String, Vec<Type>)>,
    method_calls: &mut Vec<(String, String, Vec<Type>)>,
    current_func: &Func,
    expected_ty: Option<&Type>,
) {
    match expr {
        Expr::Call { callee, args } => {
            if generics.contains_key(callee) {
                let arg_types: Vec<Type> = args.iter()
                    .map(|a| infer_expr_type_with_hint(a, current_func, expected_ty))
                    .collect();
                calls.push((callee.clone(), arg_types));
            }
            for a in args {
                collect_generic_calls_expr_with_hint(a, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Expr::Binary { left, right, .. } => {
            collect_generic_calls_expr_with_hint(left, generics, generic_methods, calls, method_calls, current_func, None);
            collect_generic_calls_expr_with_hint(right, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Expr::Unary { expr, .. } => {
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Expr::If { cond, then_block, else_block } => {
            collect_generic_calls_expr_with_hint(cond, generics, generic_methods, calls, method_calls, current_func, None);
            for s in then_block {
                collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
            }
            if let Some(eb) = else_block {
                for s in eb {
                    collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
                }
            }
        }
        Expr::Block(stmts) => {
            for s in stmts {
                collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Expr::FieldAccess { expr, .. } => {
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Expr::OptionalFieldAccess { expr, .. } => {
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Expr::MethodCall { receiver, method, args } => {
            // Check if this is a call to a generic method
            // We need to determine the struct type from the receiver
            let struct_name = infer_receiver_struct_name(receiver, current_func);
            if let Some(ref sn) = struct_name {
                if generic_methods.contains_key(&(sn.clone(), method.clone())) {
                    // Infer arg types (excluding receiver which is self)
                    let arg_types: Vec<Type> = args.iter()
                        .map(|a| infer_expr_type_with_hint(a, current_func, expected_ty))
                        .collect();
                    method_calls.push((sn.clone(), method.clone(), arg_types));
                }
            }
            // Always recurse into receiver and args for nested generic calls
            collect_generic_calls_expr_with_hint(receiver, generics, generic_methods, calls, method_calls, current_func, None);
            for a in args {
                collect_generic_calls_expr_with_hint(a, generics, generic_methods, calls, method_calls, current_func, None);
            }
        },
        Expr::Index { array, index } => {
            collect_generic_calls_expr_with_hint(array, generics, generic_methods, calls, method_calls, current_func, None);
            collect_generic_calls_expr_with_hint(index, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Expr::ArrayLit { elements, .. } => {
            for e in elements {
                collect_generic_calls_expr_with_hint(e, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Expr::Range { start, end, .. } => {
            collect_generic_calls_expr_with_hint(start, generics, generic_methods, calls, method_calls, current_func, None);
            collect_generic_calls_expr_with_hint(end, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Expr::StructLit { fields, spread, .. } => {
            for (_, e) in fields {
                collect_generic_calls_expr_with_hint(e, generics, generic_methods, calls, method_calls, current_func, None);
            }
            if let Some(se) = spread {
                collect_generic_calls_expr_with_hint(se, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Expr::Match { expr, arms } => {
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None);
            for arm in arms {
                for s in &arm.body {
                    collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
                }
            }
        }
        Expr::Try { expr } => {
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Expr::Coalesce { expr, default } => {
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None);
            collect_generic_calls_expr_with_hint(default, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Expr::IsCheck { expr, .. } => {
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Expr::TryBlock { body } => {
            for s in body {
                collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Expr::Closure { body, .. } => {
            for s in body {
                collect_generic_calls_stmt_with_hint(s, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Expr::FString { parts } => {
            for p in parts {
                if let FStringPart::Expr(e) = p {
                    collect_generic_calls_expr_with_hint(e, generics, generic_methods, calls, method_calls, current_func, None);
                }
            }
        }
        Expr::Cast { expr, .. } => {
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Expr::Asm { operands, .. } => {
            for op in operands {
                collect_generic_calls_expr_with_hint(&op.expr, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Expr::Ref { expr, .. } | Expr::Deref { expr } => {
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Expr::StrConcat { left, right } => {
            collect_generic_calls_expr_with_hint(left, generics, generic_methods, calls, method_calls, current_func, None);
            collect_generic_calls_expr_with_hint(right, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Expr::TupleLit(exprs) => {
            for e in exprs {
                collect_generic_calls_expr_with_hint(e, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        Expr::EnumLit { args, .. } => {
            for a in args {
                collect_generic_calls_expr_with_hint(a, generics, generic_methods, calls, method_calls, current_func, None);
            }
        }
        _ => {}
    }
}

/// Infer the struct name from a method call receiver expression
fn infer_receiver_struct_name(receiver: &Expr, func: &Func) -> Option<String> {
    let ty = infer_expr_type(receiver, func);
    match ty {
        Type::Named(s) => Some(s),
        Type::Generic { name, .. } => Some(name),
        Type::Ref { inner, .. } => match *inner {
            Type::Named(s) => Some(s),
            Type::Generic { name, .. } => Some(name),
            _ => None,
        },
        _ => None,
    }
}

/// Simple type inference for expressions (to infer generic arg types at call sites)
/// When a hint is provided and the expression type can't be determined directly,
/// fall back to the expected type from context.
fn infer_expr_type_with_hint(expr: &Expr, func: &Func, hint: Option<&Type>) -> Type {
    let inferred = infer_expr_type(expr, func);
    // If inference produced a Named type (unknown), try the hint
    if matches!(inferred, Type::Named(_)) {
        if let Some(h) = hint {
            return h.clone();
        }
    }
    inferred
}

/// Simple type inference for expressions (to infer generic arg types at call sites)
fn infer_expr_type(expr: &Expr, func: &Func) -> Type {
    match expr {
        Expr::IntLit(n) => {
            if *n > i32::MAX as i64 || *n < i32::MIN as i64 {
                Type::I64
            } else {
                Type::I32
            }
        }
        Expr::FloatLit(_) => Type::F64,
        Expr::BoolLit(_) => Type::Bool,
        Expr::StringLit(_) => Type::Str,
        Expr::Ident(name) => {
            for p in &func.params {
                if p.name == *name {
                    return p.ty.clone();
                }
            }
            for stmt in &func.body {
                if let Stmt::Let { name: ln, ty: Some(ty), .. } = stmt {
                    if ln == name {
                        return ty.clone();
                    }
                }
            }
            // Try to infer from let initializer if no explicit type
            for stmt in &func.body {
                if let Stmt::Let { name: ln, ty: None, expr, .. } = stmt {
                    if ln == name {
                        return infer_expr_type(expr, func);
                    }
                }
            }
            Type::Named(name.clone())
        }
        Expr::Cast { target_ty, .. } => target_ty.clone(),
        Expr::Asm { operands, .. } => {
            let has_out = operands.iter().any(|op| op.direction == crate::ast::AsmDir::Out);
            if has_out { Type::I64 } else { Type::Void }
        }
        Expr::Binary { op, left, right } => {
            match op {
                BinOp::Eq | BinOp::Neq | BinOp::Lt | BinOp::Gt | BinOp::LtEq | BinOp::GtEq => Type::Bool,
                _ => {
                    let lt = infer_expr_type(left, func);
                    let rt = infer_expr_type(right, func);
                    if !matches!(lt, Type::Named(_)) { lt } else { rt }
                }
            }
        }
        Expr::Unary { op, expr } => {
            match op {
                UnaryOp::Not => Type::Bool,
                _ => infer_expr_type(expr, func),
            }
        }
        Expr::StructLit { name, .. } => Type::Named(name.clone()),
        Expr::ArrayLit { elem_type, elements } => {
            Type::Array {
                elem: Box::new((**elem_type).clone()),
                size: elements.len(),
            }
        }
        Expr::MethodCall { receiver, .. } => {
            // For associated function calls like Point::new(x, y),
            // the receiver is Expr::Ident("Point") — infer as that type
            if let Expr::Ident(name) = receiver.as_ref() {
                // Check if it looks like a type name (associated function call)
                // We return Named(name) — the caller can check if it's a real struct
                return Type::Named(name.clone());
            }
            // For instance method calls, infer from receiver
            infer_expr_type(receiver, func)
        }
        _ => Type::I64,
    }
}
