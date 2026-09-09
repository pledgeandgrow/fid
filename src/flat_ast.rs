// flat_ast.rs — Flat AST serialization for self-host parser differential testing.
// Produces a simple line-based format that's easy to implement in Fides.
//
// Format:
//   - 2 spaces per depth level
//   - Node type first, then fields (strings double-quoted, numbers bare, bools as true/false)
//   - Children on subsequent lines, indented
//   - Vecs: each element on its own line
//   - Option: "None" or the value inline/children

use crate::ast::*;

fn esc(s: &str) -> String {
    let mut out = String::new();
    for c in s.chars() {
        match c {
            '\n' => out.push_str("\\n"),
            '\r' => out.push_str("\\r"),
            '\t' => out.push_str("\\t"),
            '\\' => out.push_str("\\\\"),
            '"' => out.push_str("\\\""),
            '\0' => out.push_str("\\0"),
            c if c.is_ascii_graphic() || c == ' ' => out.push(c),
            c => out.push_str(&format!("\\u{{{:x}}}", c as u32)),
        }
    }
    out
}

fn indent(depth: usize) -> String { "  ".repeat(depth) }
fn bool_str(b: bool) -> &'static str { if b { "true" } else { "false" } }

pub fn type_flat(t: &Type, depth: usize) -> String {
    let ind = indent(depth);
    match t {
        Type::I8 => format!("{}I8", ind),
        Type::I16 => format!("{}I16", ind),
        Type::I32 => format!("{}I32", ind),
        Type::I64 => format!("{}I64", ind),
        Type::I128 => format!("{}I128", ind),
        Type::U8 => format!("{}U8", ind),
        Type::U16 => format!("{}U16", ind),
        Type::U32 => format!("{}U32", ind),
        Type::U64 => format!("{}U64", ind),
        Type::U128 => format!("{}U128", ind),
        Type::F32 => format!("{}F32", ind),
        Type::F64 => format!("{}F64", ind),
        Type::Bool => format!("{}Bool", ind),
        Type::Str => format!("{}Str", ind),
        Type::Void => format!("{}Void", ind),
        Type::Never => format!("{}Never", ind),
        Type::Infer => format!("{}Infer", ind),
        Type::Named(n) => format!("{}Named \"{}\"", ind, esc(n)),
        Type::Array { elem, size } => {
            let mut s = format!("{}Array {}\n", ind, size);
            s.push_str(&type_flat(elem, depth + 1));
            s
        }
        Type::ArrayConst { elem, size_param } => {
            let mut s = format!("{}ArrayConst \"{}\"\n", ind, esc(size_param));
            s.push_str(&type_flat(elem, depth + 1));
            s
        }
        Type::Ref { inner, is_mut } => {
            let mut s = format!("{}Ref {}\n", ind, bool_str(*is_mut));
            s.push_str(&type_flat(inner, depth + 1));
            s
        }
        Type::RawPtr { inner, is_mut } => {
            let mut s = format!("{}RawPtr {}\n", ind, bool_str(*is_mut));
            s.push_str(&type_flat(inner, depth + 1));
            s
        }
        Type::Generic { name, args } => {
            let mut s = format!("{}Generic \"{}\"\n", ind, esc(name));
            for a in args { s.push_str(&type_flat(a, depth + 1)); s.push('\n'); }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Type::Tuple(types) => {
            let mut s = format!("{}Tuple\n", ind);
            for t in types { s.push_str(&type_flat(t, depth + 1)); s.push('\n'); }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Type::Func { params, ret } => {
            let mut s = format!("{}Func\n", ind);
            for p in params { s.push_str(&type_flat(p, depth + 1)); s.push('\n'); }
            s.push_str(&type_flat(ret, depth + 1));
            s
        }
        Type::Assoc(trait_name, assoc_name, type_name, args) => {
            let mut s = format!("{}Assoc \"{}\" \"{}\" \"{}\"\n", ind, esc(trait_name), esc(assoc_name), esc(type_name));
            for a in args { s.push_str(&type_flat(a, depth + 1)); s.push('\n'); }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Type::Dyn(name) => format!("{}Dyn \"{}\"", ind, esc(name)),
        Type::ShapedTensor { elem, shape } => {
            let mut s = format!("{}ShapedTensor\n", ind);
            s.push_str(&type_flat(elem, depth + 1));
            s.push('\n');
            for d in shape {
                match d {
                    ShapeDim::Known(n) => s.push_str(&format!("{}Known {}\n", indent(depth + 1), n)),
                    ShapeDim::Symbolic(n) => s.push_str(&format!("{}Symbolic \"{}\"\n", indent(depth + 1), esc(n))),
                }
            }
            if s.ends_with('\n') { s.pop(); }
            s
        }
    }
}

pub fn binop_flat(op: &BinOp) -> &'static str {
    match op {
        BinOp::Add => "Add", BinOp::Sub => "Sub", BinOp::Mul => "Mul",
        BinOp::Div => "Div", BinOp::Mod => "Mod", BinOp::Eq => "Eq",
        BinOp::Neq => "Neq", BinOp::Lt => "Lt", BinOp::Gt => "Gt",
        BinOp::LtEq => "LtEq", BinOp::GtEq => "GtEq", BinOp::And => "And",
        BinOp::Or => "Or", BinOp::BitAnd => "BitAnd", BinOp::BitOr => "BitOr",
        BinOp::BitXor => "BitXor", BinOp::Shl => "Shl", BinOp::Shr => "Shr",
    }
}

pub fn unaryop_flat(op: &UnaryOp) -> &'static str {
    match op { UnaryOp::Neg => "Neg", UnaryOp::Not => "Not", UnaryOp::BitNot => "BitNot" }
}

pub fn assignop_flat(op: &AssignOp) -> &'static str {
    match op {
        AssignOp::Assign => "Assign", AssignOp::AddAssign => "AddAssign",
        AssignOp::SubAssign => "SubAssign", AssignOp::MulAssign => "MulAssign",
        AssignOp::DivAssign => "DivAssign", AssignOp::ModAssign => "ModAssign",
        AssignOp::BitAndAssign => "BitAndAssign", AssignOp::BitOrAssign => "BitOrAssign",
        AssignOp::BitXorAssign => "BitXorAssign", AssignOp::ShlAssign => "ShlAssign",
        AssignOp::ShrAssign => "ShrAssign",
    }
}

pub fn pattern_flat(p: &Pattern, depth: usize) -> String {
    let ind = indent(depth);
    match p {
        Pattern::Wildcard => format!("{}Wildcard", ind),
        Pattern::IntLit(n) => format!("{}IntLit {}", ind, n),
        Pattern::BoolLit(b) => format!("{}BoolLit {}", ind, bool_str(*b)),
        Pattern::Ident(s) => format!("{}Ident \"{}\"", ind, esc(s)),
        Pattern::StrLit(s) => format!("{}StrLit \"{}\"", ind, esc(s)),
        Pattern::EnumVariant { enum_name, variant, bindings } => {
            let mut s = format!("{}EnumVariant \"{}\" \"{}\"\n", ind, esc(enum_name), esc(variant));
            for (name, ty) in bindings {
                s.push_str(&format!("{}Binding \"{}\" \"{}\"\n", indent(depth + 1), esc(name), esc(ty)));
            }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Pattern::Range { start, end } => {
            let mut s = format!("{}Range\n", ind);
            s.push_str(&pattern_flat(start, depth + 1));
            s.push('\n');
            s.push_str(&pattern_flat(end, depth + 1));
            s
        }
    }
}

pub fn expr_flat(e: &Expr, depth: usize) -> String {
    let ind = indent(depth);
    match e {
        Expr::IntLit(n) => format!("{}IntLit {}", ind, n),
        Expr::FloatLit(f) => format!("{}FloatLit {}", ind, f),
        Expr::StringLit(s) => format!("{}StringLit \"{}\"", ind, esc(s)),
        Expr::BoolLit(b) => format!("{}BoolLit {}", ind, bool_str(*b)),
        Expr::Ident(s) => format!("{}Ident \"{}\"", ind, esc(s)),
        Expr::Binary { op, left, right } => {
            let mut s = format!("{}Binary {}\n", ind, binop_flat(op));
            s.push_str(&expr_flat(left, depth + 1)); s.push('\n');
            s.push_str(&expr_flat(right, depth + 1));
            s
        }
        Expr::Unary { op, expr } => {
            let mut s = format!("{}Unary {}\n", ind, unaryop_flat(op));
            s.push_str(&expr_flat(expr, depth + 1));
            s
        }
        Expr::Call { callee, args } => {
            let mut s = format!("{}Call \"{}\"\n", ind, esc(callee));
            for a in args { s.push_str(&expr_flat(a, depth + 1)); s.push('\n'); }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Expr::FieldAccess { expr, field } => {
            let mut s = format!("{}FieldAccess \"{}\"\n", ind, esc(field));
            s.push_str(&expr_flat(expr, depth + 1));
            s
        }
        Expr::OptionalFieldAccess { expr, field } => {
            let mut s = format!("{}OptionalFieldAccess \"{}\"\n", ind, esc(field));
            s.push_str(&expr_flat(expr, depth + 1));
            s
        }
        Expr::StructLit { name, fields, spread } => {
            let mut s = format!("{}StructLit \"{}\"\n", ind, esc(name));
            for (fname, fval) in fields {
                s.push_str(&format!("{}FieldInit \"{}\"\n", indent(depth + 1), esc(fname)));
                s.push_str(&expr_flat(fval, depth + 2)); s.push('\n');
            }
            if let Some(e) = spread {
                s.push_str(&format!("{}Spread\n", indent(depth + 1)));
                s.push_str(&expr_flat(e, depth + 2)); s.push('\n');
            }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Expr::If { cond, then_block, else_block } => {
            let mut s = format!("{}If\n", ind);
            s.push_str(&expr_flat(cond, depth + 1)); s.push('\n');
            s.push_str(&format!("{}Then\n", indent(depth + 1)));
            for st in then_block { s.push_str(&stmt_flat(st, depth + 2)); s.push('\n'); }
            match else_block {
                Some(stmts) => {
                    s.push_str(&format!("{}Else\n", indent(depth + 1)));
                    for st in stmts { s.push_str(&stmt_flat(st, depth + 2)); s.push('\n'); }
                }
                None => { s.push_str(&format!("{}ElseNone\n", indent(depth + 1))); }
            }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Expr::Range { start, end, inclusive } => {
            let mut s = format!("{}Range {}\n", ind, bool_str(*inclusive));
            s.push_str(&expr_flat(start, depth + 1)); s.push('\n');
            s.push_str(&expr_flat(end, depth + 1));
            s
        }
        Expr::Block(stmts) => {
            let mut s = format!("{}Block\n", ind);
            for st in stmts { s.push_str(&stmt_flat(st, depth + 1)); s.push('\n'); }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Expr::MethodCall { receiver, method, args } => {
            let mut s = format!("{}MethodCall \"{}\"\n", ind, esc(method));
            s.push_str(&expr_flat(receiver, depth + 1)); s.push('\n');
            for a in args { s.push_str(&expr_flat(a, depth + 1)); s.push('\n'); }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Expr::ArrayLit { elem_type, elements } => {
            let mut s = format!("{}ArrayLit\n", ind);
            s.push_str(&type_flat(elem_type, depth + 1)); s.push('\n');
            for e in elements { s.push_str(&expr_flat(e, depth + 1)); s.push('\n'); }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Expr::Index { array, index } => {
            let mut s = format!("{}Index\n", ind);
            s.push_str(&expr_flat(array, depth + 1)); s.push('\n');
            s.push_str(&expr_flat(index, depth + 1));
            s
        }
        Expr::FString { parts } => {
            let mut s = format!("{}FString\n", ind);
            for p in parts {
                match p {
                    FStringPart::Literal(lit) => {
                        s.push_str(&format!("{}Literal \"{}\"\n", indent(depth + 1), esc(lit)));
                    }
                    FStringPart::Expr(e) => {
                        s.push_str(&format!("{}ExprPart\n", indent(depth + 1)));
                        s.push_str(&expr_flat(e, depth + 2)); s.push('\n');
                    }
                }
            }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Expr::Match { expr, arms } => {
            let mut s = format!("{}Match\n", ind);
            s.push_str(&expr_flat(expr, depth + 1)); s.push('\n');
            for arm in arms {
                s.push_str(&format!("{}MatchArm\n", indent(depth + 1)));
                s.push_str(&pattern_flat(&arm.pattern, depth + 2)); s.push('\n');
                match &arm.guard {
                    Some(g) => {
                        s.push_str(&format!("{}Guard\n", indent(depth + 2)));
                        s.push_str(&expr_flat(g, depth + 3)); s.push('\n');
                    }
                    None => { s.push_str(&format!("{}GuardNone\n", indent(depth + 2))); }
                }
                for st in &arm.body { s.push_str(&stmt_flat(st, depth + 2)); s.push('\n'); }
            }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Expr::EnumLit { enum_name, variant, args } => {
            let mut s = format!("{}EnumLit \"{}\" \"{}\"\n", ind, esc(enum_name), esc(variant));
            for a in args { s.push_str(&expr_flat(a, depth + 1)); s.push('\n'); }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Expr::Ref { expr, is_mut } => {
            let mut s = format!("{}Ref {}\n", ind, bool_str(*is_mut));
            s.push_str(&expr_flat(expr, depth + 1));
            s
        }
        Expr::Deref { expr } => {
            let mut s = format!("{}Deref\n", ind);
            s.push_str(&expr_flat(expr, depth + 1));
            s
        }
        Expr::StrConcat { left, right } => {
            let mut s = format!("{}StrConcat\n", ind);
            s.push_str(&expr_flat(left, depth + 1)); s.push('\n');
            s.push_str(&expr_flat(right, depth + 1));
            s
        }
        Expr::Try { expr } => {
            let mut s = format!("{}Try\n", ind);
            s.push_str(&expr_flat(expr, depth + 1));
            s
        }
        Expr::Coalesce { expr, default } => {
            let mut s = format!("{}Coalesce\n", ind);
            s.push_str(&expr_flat(expr, depth + 1)); s.push('\n');
            s.push_str(&expr_flat(default, depth + 1));
            s
        }
        Expr::IsCheck { expr, type_name } => {
            let mut s = format!("{}IsCheck \"{}\"\n", ind, esc(type_name));
            s.push_str(&expr_flat(expr, depth + 1));
            s
        }
        Expr::TryBlock { body } => {
            let mut s = format!("{}TryBlock\n", ind);
            for st in body { s.push_str(&stmt_flat(st, depth + 1)); s.push('\n'); }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Expr::TupleLit(exprs) => {
            let mut s = format!("{}TupleLit\n", ind);
            for e in exprs { s.push_str(&expr_flat(e, depth + 1)); s.push('\n'); }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Expr::Closure { params, body, ret_ty } => {
            let mut s = format!("{}Closure\n", ind);
            for p in params { s.push_str(&param_flat(p, depth + 1)); s.push('\n'); }
            match ret_ty {
                Some(t) => {
                    s.push_str(&format!("{}RetTy\n", indent(depth + 1)));
                    s.push_str(&type_flat(t, depth + 2)); s.push('\n');
                }
                None => { s.push_str(&format!("{}RetTyNone\n", indent(depth + 1))); }
            }
            for st in body { s.push_str(&stmt_flat(st, depth + 1)); s.push('\n'); }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Expr::Cast { expr, target_ty } => {
            let mut s = format!("{}Cast\n", ind);
            s.push_str(&expr_flat(expr, depth + 1)); s.push('\n');
            s.push_str(&type_flat(target_ty, depth + 1));
            s
        }
        Expr::Asm { template, operands } => {
            let mut s = format!("{}Asm \"{}\"\n", ind, esc(template));
            for op in operands {
                let dir = match op.direction { AsmDir::In => "In", AsmDir::Out => "Out", AsmDir::Clobber => "Clobber" };
                s.push_str(&format!("{}AsmOperand {} \"{}\"\n", indent(depth + 1), dir, esc(&op.reg)));
                s.push_str(&expr_flat(&op.expr, depth + 2)); s.push('\n');
            }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Expr::Comptime(inner) => {
            let mut s = format!("{}Comptime\n", ind);
            s.push_str(&expr_flat(inner, depth + 1));
            s
        }
        Expr::ComptimeBlock(stmts) => {
            let mut s = format!("{}ComptimeBlock\n", ind);
            for st in stmts { s.push_str(&stmt_flat(st, depth + 1)); s.push('\n'); }
            if s.ends_with('\n') { s.pop(); }
            s
        }
        Expr::ComptimeReflect { op, ty } => {
            let op_str = match op {
                ReflectOp::TypeName => "TypeName", ReflectOp::IsStruct => "IsStruct",
                ReflectOp::IsEnum => "IsEnum", ReflectOp::FieldCount => "FieldCount",
                ReflectOp::FieldNames => "FieldNames", ReflectOp::FieldTypes => "FieldTypes",
                ReflectOp::EnumVariants => "EnumVariants",
            };
            let mut s = format!("{}ComptimeReflect {}\n", ind, op_str);
            s.push_str(&type_flat(ty, depth + 1));
            s
        }
        Expr::PipeForward { left, right } => {
            let mut s = format!("{}PipeForward\n", ind);
            s.push_str(&expr_flat(left, depth + 1)); s.push('\n');
            s.push_str(&expr_flat(right, depth + 1));
            s
        }
        Expr::QuantCast { expr, target_ty } => {
            let mut s = format!("{}QuantCast\n", ind);
            s.push_str(&expr_flat(expr, depth + 1)); s.push('\n');
            s.push_str(&type_flat(target_ty, depth + 1));
            s
        }
        Expr::Await { expr } => {
            let mut s = format!("{}Await\n", ind);
            s.push_str(&expr_flat(expr, depth + 1));
            s
        }
        Expr::Spawn { expr } => {
            let mut s = format!("{}Spawn\n", ind);
            s.push_str(&expr_flat(expr, depth + 1));
            s
        }
    }
}

pub fn stmt_flat(s: &Stmt, depth: usize) -> String {
    let ind = indent(depth);
    match s {
        Stmt::Let { name, ty, expr, is_mut, is_persist, annotations: _, line } => {
            let mut out = format!("{}Let \"{}\" {} {} {}\n", ind, esc(name), bool_str(*is_mut), bool_str(*is_persist), line);
            match ty {
                Some(t) => out.push_str(&type_flat(t, depth + 1)),
                None => out.push_str(&format!("{}None", indent(depth + 1))),
            }
            out.push('\n');
            out.push_str(&expr_flat(expr, depth + 1));
            out
        }
        Stmt::LetTuple { names, expr, line } => {
            let mut out = format!("{}LetTuple {}\n", ind, line);
            for n in names { out.push_str(&format!("{}\"{}\"\n", indent(depth + 1), esc(n))); }
            out.push_str(&expr_flat(expr, depth + 1));
            out
        }
        Stmt::LetUninit { name, ty, annotations: _, line } => {
            let mut out = format!("{}LetUninit \"{}\" {}\n", ind, esc(name), line);
            out.push_str(&type_flat(ty, depth + 1));
            out
        }
        Stmt::Assign { target, op, expr, line } => {
            let mut out = format!("{}Assign \"{}\" {} {}\n", ind, esc(target), assignop_flat(op), line);
            out.push_str(&expr_flat(expr, depth + 1));
            out
        }
        Stmt::IndexAssign { array, index, op, expr, line } => {
            let mut out = format!("{}IndexAssign \"{}\" {} {}\n", ind, esc(array), assignop_flat(op), line);
            out.push_str(&expr_flat(index, depth + 1)); out.push('\n');
            out.push_str(&expr_flat(expr, depth + 1));
            out
        }
        Stmt::DerefAssign { ptr_expr, op, expr, line } => {
            let mut out = format!("{}DerefAssign {} {}\n", ind, assignop_flat(op), line);
            out.push_str(&expr_flat(ptr_expr, depth + 1)); out.push('\n');
            out.push_str(&expr_flat(expr, depth + 1));
            out
        }
        Stmt::FieldAssign { object, field, op, expr, line } => {
            let mut out = format!("{}FieldAssign \"{}\" {} {}\n", ind, esc(field), assignop_flat(op), line);
            out.push_str(&expr_flat(object, depth + 1)); out.push('\n');
            out.push_str(&expr_flat(expr, depth + 1));
            out
        }
        Stmt::Expr { 0: expr, 1: line } => {
            let mut out = format!("{}Expr {}\n", ind, line);
            out.push_str(&expr_flat(expr, depth + 1));
            out
        }
        Stmt::Return(expr, line) => {
            let mut out = format!("{}Return {}\n", ind, line);
            match expr {
                Some(e) => out.push_str(&expr_flat(e, depth + 1)),
                None => out.push_str(&format!("{}None", indent(depth + 1))),
            }
            out
        }
        Stmt::If { cond, then_block, else_block, line } => {
            let mut out = format!("{}If {}\n", ind, line);
            out.push_str(&expr_flat(cond, depth + 1)); out.push('\n');
            out.push_str(&format!("{}Then\n", indent(depth + 1)));
            for st in then_block { out.push_str(&stmt_flat(st, depth + 2)); out.push('\n'); }
            match else_block {
                Some(stmts) => {
                    out.push_str(&format!("{}Else\n", indent(depth + 1)));
                    for st in stmts { out.push_str(&stmt_flat(st, depth + 2)); out.push('\n'); }
                }
                None => { out.push_str(&format!("{}ElseNone\n", indent(depth + 1))); }
            }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::While { cond, body, line } => {
            let mut out = format!("{}While {}\n", ind, line);
            out.push_str(&expr_flat(cond, depth + 1)); out.push('\n');
            for st in body { out.push_str(&stmt_flat(st, depth + 1)); out.push('\n'); }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::For { var, iterator, body, line } => {
            let mut out = format!("{}For \"{}\" {}\n", ind, esc(var), line);
            out.push_str(&expr_flat(iterator, depth + 1)); out.push('\n');
            for st in body { out.push_str(&stmt_flat(st, depth + 1)); out.push('\n'); }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::Loop { body, line } => {
            let mut out = format!("{}Loop {}\n", ind, line);
            for st in body { out.push_str(&stmt_flat(st, depth + 1)); out.push('\n'); }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::Break(expr, line) => {
            let mut out = format!("{}Break {}\n", ind, line);
            match expr {
                Some(e) => out.push_str(&expr_flat(e, depth + 1)),
                None => out.push_str(&format!("{}None", indent(depth + 1))),
            }
            out
        }
        Stmt::Continue(line) => format!("{}Continue {}", ind, line),
        Stmt::Defer(body, line) => {
            let mut out = format!("{}Defer {}\n", ind, line);
            for st in body { out.push_str(&stmt_flat(st, depth + 1)); out.push('\n'); }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::Match { expr, arms, line } => {
            let mut out = format!("{}Match {}\n", ind, line);
            out.push_str(&expr_flat(expr, depth + 1)); out.push('\n');
            for arm in arms {
                out.push_str(&format!("{}MatchArm\n", indent(depth + 1)));
                out.push_str(&pattern_flat(&arm.pattern, depth + 2)); out.push('\n');
                match &arm.guard {
                    Some(g) => {
                        out.push_str(&format!("{}Guard\n", indent(depth + 2)));
                        out.push_str(&expr_flat(g, depth + 3)); out.push('\n');
                    }
                    None => { out.push_str(&format!("{}GuardNone\n", indent(depth + 2))); }
                }
                for st in &arm.body { out.push_str(&stmt_flat(st, depth + 2)); out.push('\n'); }
            }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::Use { path, items, line } => {
            let mut out = format!("{}Use \"{}\" {}\n", ind, esc(path), line);
            for item in items { out.push_str(&format!("{}\"{}\"\n", indent(depth + 1), esc(item))); }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::Handle { body, handlers, line } => {
            let mut out = format!("{}Handle {}\n", ind, line);
            for st in body { out.push_str(&stmt_flat(st, depth + 1)); out.push('\n'); }
            for h in handlers {
                out.push_str(&format!("{}EffectHandler \"{}\" \"{}\"\n", indent(depth + 1), esc(&h.effect), esc(&h.op)));
                for p in &h.params { out.push_str(&format!("{}\"{}\"\n", indent(depth + 2), esc(p))); }
                for st in &h.body { out.push_str(&stmt_flat(st, depth + 2)); out.push('\n'); }
            }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::Do { effect, op, args, line } => {
            let mut out = format!("{}Do \"{}\" \"{}\" {}\n", ind, esc(effect), esc(op), line);
            for a in args { out.push_str(&expr_flat(a, depth + 1)); out.push('\n'); }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::RegionBlock { name, budget, is_gpu, is_kv_cache, window_size, body, line } => {
            let mut out = format!("{}RegionBlock \"{}\" {} {} {} {} {}\n", ind, esc(name),
                match budget { Some(n) => n.to_string(), None => "None".to_string() },
                bool_str(*is_gpu), bool_str(*is_kv_cache),
                match window_size { Some(n) => n.to_string(), None => "None".to_string() },
                line);
            for st in body { out.push_str(&stmt_flat(st, depth + 1)); out.push('\n'); }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::Sandbox { body, line } => {
            let mut out = format!("{}Sandbox {}\n", ind, line);
            for st in body { out.push_str(&stmt_flat(st, depth + 1)); out.push('\n'); }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::Unsafe { body, line } => {
            let mut out = format!("{}Unsafe {}\n", ind, line);
            for st in body { out.push_str(&stmt_flat(st, depth + 1)); out.push('\n'); }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::Txn { body, line } => {
            let mut out = format!("{}Txn {}\n", ind, line);
            for st in body { out.push_str(&stmt_flat(st, depth + 1)); out.push('\n'); }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::ComptimeGen { block, line } => {
            let mut out = format!("{}ComptimeGen {}\n", ind, line);
            for st in block { out.push_str(&stmt_flat(st, depth + 1)); out.push('\n'); }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::GradBlock { var, body, line } => {
            let mut out = format!("{}GradBlock \"{}\" {}\n", ind, esc(var), line);
            for st in body { out.push_str(&stmt_flat(st, depth + 1)); out.push('\n'); }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::Backward { loss, line } => {
            let mut out = format!("{}Backward {}\n", ind, line);
            out.push_str(&expr_flat(loss, depth + 1));
            out
        }
        Stmt::Query { components, body, line } => {
            let mut out = format!("{}Query {}\n", ind, line);
            for c in components { out.push_str(&format!("{}\"{}\"\n", indent(depth + 1), esc(c))); }
            for st in body { out.push_str(&stmt_flat(st, depth + 1)); out.push('\n'); }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::Select { cases, default, line } => {
            let mut out = format!("{}Select {}\n", ind, line);
            for c in cases {
                out.push_str(&format!("{}SelectCase \"{}\"\n", indent(depth + 1), esc(&c.var_name)));
                out.push_str(&expr_flat(&c.channel, depth + 2)); out.push('\n');
                for st in &c.body { out.push_str(&stmt_flat(st, depth + 2)); out.push('\n'); }
            }
            match default {
                Some(stmts) => {
                    out.push_str(&format!("{}Default\n", indent(depth + 1)));
                    for st in stmts { out.push_str(&stmt_flat(st, depth + 2)); out.push('\n'); }
                }
                None => { out.push_str(&format!("{}DefaultNone\n", indent(depth + 1))); }
            }
            if out.ends_with('\n') { out.pop(); }
            out
        }
        Stmt::Parallel { branches, line } => {
            let mut out = format!("{}Parallel {}\n", ind, line);
            for (i, branch) in branches.iter().enumerate() {
                out.push_str(&format!("{}Branch {}\n", indent(depth + 1), i));
                for st in branch { out.push_str(&stmt_flat(st, depth + 2)); out.push('\n'); }
            }
            if out.ends_with('\n') { out.pop(); }
            out
        }
    }
}

pub fn param_flat(p: &Param, depth: usize) -> String {
    let ind = indent(depth);
    let mut s = format!("{}Param \"{}\" {} {}\n", ind, esc(&p.name), bool_str(p.is_pub), p.annotations.len());
    s.push_str(&type_flat(&p.ty, depth + 1));
    s
}

pub fn annotation_flat(a: &Annotation, depth: usize) -> String {
    let ind = indent(depth);
    let mut s = format!("{}Annotation \"{}\"\n", ind, esc(&a.name));
    for arg in &a.args { s.push_str(&format!("{}\"{}\"\n", indent(depth + 1), esc(arg))); }
    if s.ends_with('\n') { s.pop(); }
    s
}

pub fn func_flat(f: &Func, depth: usize) -> String {
    let ind = indent(depth);
    let mut s = format!("{}Func \"{}\" {} {} {} {} {}\n", ind, esc(&f.name),
        bool_str(f.is_pub), bool_str(f.is_comptime), bool_str(f.is_async),
        bool_str(f.is_interrupt), bool_str(f.has_effect_annotation));
    s.push_str(&format!("{}TypeParams {}\n", indent(depth + 1), f.type_params.len()));
    for tp in &f.type_params { s.push_str(&format!("{}\"{}\"\n", indent(depth + 2), esc(tp))); }
    for p in &f.params { s.push_str(&param_flat(p, depth + 1)); s.push('\n'); }
    s.push_str(&type_flat(&f.ret_ty, depth + 1)); s.push('\n');
    for st in &f.body { s.push_str(&stmt_flat(st, depth + 1)); s.push('\n'); }
    if s.ends_with('\n') { s.pop(); }
    s
}

pub fn program_flat(p: &Program) -> String {
    let mut s = String::new();
    s.push_str("Program\n");
    match &p.module {
        Some(m) => s.push_str(&format!("  Module \"{}\"\n", esc(m))),
        None => s.push_str("  ModuleNone\n"),
    }
    for f in &p.funcs { s.push_str(&func_flat(f, 1)); s.push('\n'); }
    for st in &p.structs {
        s.push_str(&format!("  Struct \"{}\" {} {}\n", esc(&st.name), bool_str(st.is_pub), st.fields.len()));
        for f in &st.fields { s.push_str(&param_flat(f, 2)); s.push('\n'); }
        if s.ends_with('\n') { s.pop(); }
        s.push('\n');
    }
    for e in &p.enums {
        s.push_str(&format!("  Enum \"{}\" {} {}\n", esc(&e.name), bool_str(e.is_pub), e.variants.len()));
        for v in &e.variants {
            match &v.data {
                Some(params) => {
                    s.push_str(&format!("    Variant \"{}\" {}\n", esc(&v.name), params.len()));
                    for p in params { s.push_str(&param_flat(p, 3)); s.push('\n'); }
                    if s.ends_with('\n') { s.pop(); }
                }
                None => { s.push_str(&format!("    Variant \"{}\"\n", esc(&v.name))); }
            }
        }
        s.push('\n');
    }
    for c in &p.consts {
        s.push_str(&format!("  Const \"{}\" {}\n", esc(&c.name), bool_str(c.is_pub)));
        s.push_str(&type_flat(&c.ty, 2)); s.push('\n');
        s.push_str(&expr_flat(&c.expr, 2)); s.push('\n');
    }
    for ta in &p.type_aliases {
        s.push_str(&format!("  TypeAlias \"{}\" {}\n", esc(&ta.name), bool_str(ta.is_pub)));
        s.push_str(&type_flat(&ta.alias_for, 2)); s.push('\n');
    }
    for im in &p.impls {
        s.push_str(&format!("  Impl \"{}\" {}\n", esc(&im.struct_name), im.methods.len()));
        for m in &im.methods { s.push_str(&func_flat(m, 2)); s.push('\n'); }
        if s.ends_with('\n') { s.pop(); }
        s.push('\n');
    }
    for im in &p.imports {
        s.push_str(&format!("  Import \"{}\" {} {} {} {} {}\n", esc(&im.path),
            bool_str(im.is_pub), bool_str(im.is_glob), im.items.len(), im.capabilities.len(),
            match &im.version { Some(_) => "true", None => "false" }));
        for item in &im.items { s.push_str(&format!("    \"{}\"\n", esc(item))); }
    }
    for a in &p.attributes {
        s.push_str(&format!("  Attribute \"{}\"\n", esc(&a.name)));
        for arg in &a.args { s.push_str(&format!("    \"{}\"\n", esc(arg))); }
    }
    for t in &p.traits {
        s.push_str(&format!("  Trait \"{}\" {}\n", esc(&t.name), bool_str(t.is_pub)));
        for m in &t.methods {
            s.push_str(&format!("    TraitMethod \"{}\" {} {}\n", esc(&m.name), bool_str(m.has_self), bool_str(m.is_pub)));
            for p in &m.params { s.push_str(&param_flat(p, 3)); s.push('\n'); }
            s.push_str(&type_flat(&m.ret_ty, 3)); s.push('\n');
        }
    }
    for ti in &p.trait_impls {
        s.push_str(&format!("  TraitImpl \"{}\" \"{}\" {}\n", esc(&ti.trait_name), esc(&ti.type_name), ti.methods.len()));
        for m in &ti.methods { s.push_str(&func_flat(m, 2)); s.push('\n'); }
        if s.ends_with('\n') { s.pop(); }
        s.push('\n');
    }
    for ed in &p.extern_decls {
        s.push_str(&format!("  Extern \"{}\" \"{}\" \"{}\"\n", esc(&ed.abi), esc(&ed.c_name), esc(&ed.pledge_name)));
        for p in &ed.params { s.push_str(&param_flat(p, 2)); s.push('\n'); }
        s.push_str(&type_flat(&ed.ret_ty, 2)); s.push('\n');
    }
    s.push_str(&format!("  Layers {}\n", p.layers.len()));
    s.push_str(&format!("  Models {}\n", p.models.len()));
    s.push_str(&format!("  Components {}\n", p.components.len()));
    s.push_str(&format!("  Systems {}\n", p.systems.len()));
    s.push_str(&format!("  StateMachines {}\n", p.state_machines.len()));
    s.push_str(&format!("  EventHandlers {}\n", p.event_handlers.len()));
    if s.ends_with('\n') { s.pop(); }
    s
}
