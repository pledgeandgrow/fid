use crate::ast::*;
use std::collections::{HashMap, HashSet};

/// A substitution mapping type parameters to concrete types
type Subst = HashMap<String, Type>;

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
        Type::Named(s) => s.clone(),
        Type::Generic { name, args } => {
            let args_str: Vec<String> = args.iter().map(mangle_type).collect();
            format!("{}__{}", name, args_str.join("_"))
        }
        Type::Ref { inner, .. } => format!("ref_{}", mangle_type(inner)),
        Type::Array { elem, .. } => format!("arr_{}", mangle_type(elem)),
        Type::Tuple(types) => {
            let parts: Vec<String> = types.iter().map(mangle_type).collect();
            format!("tup_{}", parts.join("_"))
        }
        Type::Func { params, ret } => {
            let p: Vec<String> = params.iter().map(mangle_type).collect();
            format!("fn_{}_{}", p.join("_"), mangle_type(ret))
        }
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
        Type::Ref { inner, is_mut } => Type::Ref {
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
        Expr::Ident(s) => Expr::Ident(s.clone()),
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
        Expr::StructLit { name, fields } => Expr::StructLit {
            name: name.clone(),
            fields: fields.iter().map(|(n, e)| {
                (n.clone(), subst_expr(e, subst, renames, method_renames))
            }).collect(),
        },
        Expr::Match { expr, arms } => Expr::Match {
            expr: Box::new(subst_expr(expr, subst, renames, method_renames)),
            arms: arms.iter().map(|a| MatchArm {
                pattern: a.pattern.clone(),
                body: a.body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            }).collect(),
        },
        Expr::Try { expr } => Expr::Try {
            expr: Box::new(subst_expr(expr, subst, renames, method_renames)),
        },
        Expr::TryBlock { body } => Expr::TryBlock {
            body: body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
        },
        Expr::Closure { params, body, ret_ty } => Expr::Closure {
            params: params.iter().map(|p| Param {
                name: p.name.clone(),
                ty: subst_type(&p.ty, subst),
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
    }
}

/// Apply substitution to a statement
fn subst_stmt(stmt: &Stmt, subst: &Subst, renames: &HashMap<String, String>, method_renames: &HashMap<String, String>) -> Stmt {
    match stmt {
        Stmt::Let { name, ty, is_mut, expr, line } => Stmt::Let {
            name: name.clone(),
            ty: ty.as_ref().map(|t| subst_type(t, subst)),
            is_mut: *is_mut,
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
                body: a.body.iter().map(|s| subst_stmt(s, subst, renames, method_renames)).collect(),
            }).collect(),
            line: *line,
        },
        Stmt::Use { path, items, line } => Stmt::Use {
            path: path.clone(),
            items: items.clone(),
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
    };

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
        type_aliases: program.type_aliases.clone(),
        impls: new_impls,
        imports: program.imports.clone(),
        attributes: program.attributes.clone(),
        traits: program.traits.clone(),
        trait_impls: new_trait_impls,
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

    // Rewrite the function body with renames
    let new_body: Vec<Stmt> = func.body.iter()
        .map(|s| subst_stmt(s, &HashMap::new(), &local_renames, &method_renames))
        .collect();

    Func {
        name: func.name.clone(),
        type_params: Vec::new(),
        type_param_bounds: Vec::new(),
        params: func.params.clone(),
        ret_ty: func.ret_ty.clone(),
        body: new_body,
        is_pub: func.is_pub,
    }
}

/// Instantiate a generic function with a concrete substitution.
/// Recursively processes the instantiated body to handle nested generic calls.
fn instantiate(gen_func: &Func, subst: &Subst, mangled_name: &str, ctx: &mut MonoCtx) -> Func {
    // Apply substitution to params
    let new_params: Vec<Param> = gen_func.params.iter().map(|p| Param {
        name: p.name.clone(),
        ty: subst_type(&p.ty, subst),
    }).collect();

    let new_ret_ty = subst_type(&gen_func.ret_ty, subst);

    // First, apply the type substitution to the body
    let subst_body: Vec<Stmt> = gen_func.body.iter()
        .map(|s| subst_stmt(s, subst, &HashMap::new(), &HashMap::new()))
        .collect();

    // Create a temporary concrete function with the substituted body
    let temp_func = Func {
        name: mangled_name.to_string(),
        type_params: Vec::new(),
        type_param_bounds: Vec::new(),
        params: new_params,
        ret_ty: new_ret_ty.clone(),
        body: subst_body,
        is_pub: gen_func.is_pub,
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
        Stmt::Assign { expr, .. } => collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None),
        Stmt::IndexAssign { index, expr, .. } => {
            collect_generic_calls_expr_with_hint(index, generics, generic_methods, calls, method_calls, current_func, None);
            collect_generic_calls_expr_with_hint(expr, generics, generic_methods, calls, method_calls, current_func, None);
        }
        Stmt::DerefAssign { ptr_expr, expr, .. } => {
            collect_generic_calls_expr_with_hint(ptr_expr, generics, generic_methods, calls, method_calls, current_func, None);
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
    }
}

/// Walk expressions to find calls to generic functions
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
        Expr::StructLit { fields, .. } => {
            for (_, e) in fields {
                collect_generic_calls_expr_with_hint(e, generics, generic_methods, calls, method_calls, current_func, None);
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
