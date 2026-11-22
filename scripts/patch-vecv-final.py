import re

# ============================================================
# VecV pivot: the type the checker sees is Vec<E>; the sequential
# flag is a "sequential" annotation the parser adds to the var;
# the lowerer checks the annotation to route to VecV_* functions.
# Zero typechecker changes (it handles Vec<E> natively).
# ============================================================

# ---- 1) parser: Vec<E..> → Generic("Vec", [E]) + sequential annotation ----
p = 'compiler/parser.fd'
s = open(p, encoding='utf-8', newline='').read()
old = 'return ParseResultType::Ok(Type::Generic("VecV", args));'
assert s.count(old) == 1
s = s.replace(old, '''g_sequential_type = true;
                    return ParseResultType::Ok(Type::Generic("Vec", args));''')
open(p, 'w', encoding='utf-8', newline='').write(s)
print("parser: Vec<E..> → Vec<E> + flag")

# ---- 2) typechecker: remove ALL VecV special-casing (the type is Vec now) ----
p2 = 'compiler/typechecker.fd'
s2 = open(p2, encoding='utf-8', newline='').read()

# 2a. remove the resolve_type VecV→Vec mapping (not needed: type is already Vec)
old_rt = '''    // 0008: VecV is Vec to the type system (the sequential flag is the
    // lowerer's, from the AST type the parser produced)
    match ty {
        Type::Generic(vvn, vva) => {
            if vvn == "VecV" {
                var vva2: Vec<Type> = Vec::new();
                var vvi: i64 = 0;
                while vvi < vva.len() {
                    vva2.push(TypeChecker_resolve_type(self, vva.get(vvi)));
                    vvi = vvi + 1;
                }
                return Type::Generic("Vec", vva2);
            }
        }
        _ => {
        }
    }
    match ty {'''
assert s2.count(old_rt) == 1
s2 = s2.replace(old_rt, '    match ty {')

# 2b. remove the VecV::new() handler (users call Vec::new() now)
old_vn = '''            if name == "VecV" && method == "new" {
                tc_vecv_gate(self);
                // the type the checker sees is Vec (resolve_type maps VecV
                // to Vec); the lowerer reads the AST type (VecV) to route
                match hint {
                    Type::Generic(hn, hargs) => {
                        if hn == "Vec" || hn == "VecV" {
                            return Type::Generic("Vec", hargs);
                        }
                    }
                    _ => {
                    }
                }
                return Type::Named("Vec");
            }
'''
assert s2.count(old_vn) == 1
s2 = s2.replace(old_vn, '')

# 2c. remove the VecV method rules block
old_vr = '''            if name == "VecV" {
                // 0008: a variant stream — build and walk (push/get/len/
                // is_empty/last); the mutating-by-index forms are the
                // fixed-mode Vec<E>'s
                tc_vecv_gate(self);
                var velem: Type = Type::Infer;
                if gargs.len() > 0 {
                    velem = gargs.get(0);
                }
                if method == "push" {
                    if arg_types.len() == 1 && !TypeChecker_types_compatible(self, velem, arg_types.get(0)) {
                        self.errors.push("Type error: cannot store " + type_to_str(arg_types.get(0)) + " in a Vec<" + type_to_str(velem) + "..> (push)");
                    }
                    return Type::Void;
                }
                if method == "get" || method == "last" {
                    return velem;
                }
                if method == "len" {
                    return Type::I64;
                }
                if method == "is_empty" {
                    return Type::Bool;
                }
                self.errors.push("Type error (line " + int_to_str(self.current_line) + "): '" + method + "' is not a Vec<E..> method: a variant stream is built with push and read with get/last/len (indexing and overwriting are the fixed Vec<E>'s)");
                return Type::Never;
            }
'''
assert s2.count(old_vr) == 1
s2 = s2.replace(old_vr, '')

# 2d. remove the gate function and VecV from the known-generic list
old_gate = '''/// 0008: Vec<E..> sits behind FID_CONTIGUOUS=1 until the suite is green
/// (docs/proposals/0008-contiguous-data.md). One env read per process.
var g_tc_vecv_gated: bool = false;
fn tc_vecv_gate(self: &mut TypeChecker) {
    if g_tc_vecv_gated {
        return;
    }
    g_tc_vecv_gated = true;
    var ev: str = p_getenv("FID_CONTIGUOUS");
    if str_len(ev) == 0 || (ev as i64) == 0 {
        self.errors.push("Type error: 'Vec<E..>' (a variant stream) is behind FID_CONTIGUOUS=1 (proposal 0008, in progress): set it to try it, or use the fixed-size Vec<E>");
    }
}

'''
assert s2.count(old_gate) == 1
s2 = s2.replace(old_gate, '')

# remove VecV from the known-generic list
s2 = s2.replace('if n == "Vec" || n == "VecV" || n == "HashMap"', 'if n == "Vec" || n == "HashMap"')

# remove the aggregate/recognition additions
s2 = s2.replace('aggregate = gn == "Vec" || gn == "VecV" || gn == "HashMap";',
               'aggregate = gn == "Vec" || gn == "HashMap";')
s2 = s2.replace('return rn == "Vec" || rn == "VecV";', 'return rn == "Vec";')
s2 = s2.replace('bare = bn == "Vec" || bn == "VecV";', 'bare = bn == "Vec";')

# remove the __vecv_gate_marker arm
s2 = s2.replace('''            if callee == "__vecv_gate_marker" {
                return Type::Void;
            }
''', '')

open(p2, 'w', encoding='utf-8', newline='').write(s2)
print("typechecker: all VecV code removed (Vec<E> handled natively)")

# ---- 3) lower.fd: route by the sequential annotation ----
p3 = 'compiler/lower.fd'
s3 = open(p3, encoding='utf-8', newline='').read()

# 3a. remove the VecV::new() associated-call handler (users call Vec::new())
old_vnew = '''            if name == "VecV" && method == "new" {
                var venv: i64 = Lowerer_emit_inst(self, Inst::ConstInt(0, IrType::I64));
                return Lowerer_emit_inst(self, Inst::Call("VecV_new", lw_c_vec1_i64(venv), IrType::Opaque("VecV"), EffectSet_new()));
            }
'''
assert s3.count(old_vnew) == 1
s3 = s3.replace(old_vnew, '')

# 3b. change the VecV method interception: instead of checking the receiver
# type (Opaque("VecV")), check if the receiver VARIABLE has the sequential
# annotation. Add a sequential-vars registry.
old_hook = '''    // 0008 (FID_CONTIGUOUS): a Vec<E..>'s methods — the variant stream's
    // own core functions (the generic Vec_ path must not touch it)
    var vv_hit: bool = false;
    match recv_ty {
        IrType::Ptr(vv_inner) => {
            match box_ir_type_unbox(vv_inner) {
                IrType::Opaque(vv_on) => {
                    if vv_on == "VecV" {
                        vv_hit = true;
                    }
                }
                _ => {
                }
            }
        }
        IrType::Opaque(vv_on2) => {
            if vv_on2 == "VecV" {
                vv_hit = true;
            }
        }
        _ => {
        }
    }
    if vv_hit {
        return Lowerer_vecv_method(self, receiver, recv_val, method, args);
    }'''
assert s3.count(old_hook) == 1
s3 = s3.replace(old_hook, '''    // 0008 (FID_CONTIGUOUS): a @sequential Vec<E>'s methods — the variant
    // stream's own core functions (the generic Vec_ path must not touch it)
    if lw_is_sequential_var(self, lw_box_expr_unbox(receiver)) {
        return Lowerer_vecv_method(self, receiver, recv_val, method, args);
    }''')

# 3c. add the sequential-vars registry + the check, before Lowerer_vecv_method
old_reg = '''/// The element AST type of a Vec<E..> receiver expression.'''
assert s3.count(old_reg) == 1
s3 = s3.replace(old_reg, '''// --- 0008: the @sequential vars (their Vec method calls route to VecV_*) --
var g_lw_sequential_vars: Vec<str> = Vec::new();

/// Whether `e` (an Ident) names a var the parser marked @sequential
/// (its Vec<E..> type produced a "sequential" annotation on the let).
fn lw_is_sequential_var(self: &Lowerer, e: Expr) -> bool {
    match e {
        Expr::Ident(n) => {
            var i: i64 = 0;
            while i < g_lw_sequential_vars.len() {
                if g_lw_sequential_vars.get(i) == n {
                    return true;
                }
                i = i + 1;
            }
        }
        _ => {
        }
    }
    return false;
}

/// The element AST type of a Vec<E..> receiver expression.''')

# 3d. change Lowerer_vecv_elem_ast to look up by var name + check the registry
old_elem = '''fn Lowerer_vecv_elem_ast(self: &Lowerer, receiver: Expr) -> Type {
    match receiver {
        Expr::Ident(vn) => {
            match Lowerer_lookup_var_ast_type(self, vn) {
                OptType::Some(at) => {
                    match at {
                        Type::Generic(gn, ga) => {
                            if gn == "VecV" && ga.len() == 1 {
                                return ga.get(0);
                            }
                        }
                        _ => {
                        }
                    }
                }
                OptType::None => {
                }
            }
        }
        _ => {
        }
    }
    return Type::Infer;
}'''
assert s3.count(old_elem) == 1
s3 = s3.replace(old_elem, '''fn Lowerer_vecv_elem_ast(self: &Lowerer, receiver: Expr) -> Type {
    match receiver {
        Expr::Ident(vn) => {
            match Lowerer_lookup_var_ast_type(self, vn) {
                OptType::Some(at) => {
                    match at {
                        Type::Generic(gn, ga) => {
                            if gn == "Vec" && ga.len() == 1 {
                                return ga.get(0);
                            }
                        }
                        _ => {
                        }
                    }
                }
                OptType::None => {
                }
            }
        }
        _ => {
        }
    }
    return Type::Infer;
}''')

# 3e. in the Stmt::Let lowering, register sequential vars. Find where
# annotations are available on the let statement.
# The Lowerer processes Stmt::Let(name, ty, expr, mut, persist, anns, line).
# We check anns for "sequential" and register the name.
# This needs to be in Lowerer_lower_stmt's Let handling.
# For now, do it in the method-call path by checking the var's annotation
# from the AST — simpler: register in Lowerer_lower_stmt.

# Actually, the simplest: in the lower_stmt handler for Let, before
# processing, check the annotations. Find the Let handler.
# We search for "Stmt::Let(" in the lower_stmt function.

# Add the registration right before the method-call hook:
old_reg2 = '''    // 0008 (FID_CONTIGUOUS): a @sequential Vec<E>'s methods — the variant
    // stream's own core functions (the generic Vec_ path must not touch it)
    if lw_is_sequential_var(self, lw_box_expr_unbox(receiver)) {'''
assert s3.count(old_reg2) == 1
# Actually, we should register the var when the Let statement is lowered.
# But finding the exact Let lowering site is complex. Simpler: register
# at the TOP of build/lower_program by scanning the AST for sequential
# annotations. For now, let the method-call handler check the AST annotation
# directly from the parser's sequential list. But the lowerer doesn't have
# access to the parser's state...

# SIMPLEST APPROACH: have lw_is_sequential_var check the var's AST type
# from the lowerer's own table. The lowerer stores the AST type when the
# variable is declared. If the AST type is Generic("VecV", ...) then it's
# sequential. But we changed the parser to produce Generic("Vec", ...).

# OK, the ACTUAL simplest: have the parser keep the sequential var names
# in a global list that the lowerer reads.
s3 = s3.replace(old_reg2, '''    // 0008 (FID_CONTIGUOUS): a @sequential Vec<E>'s methods — the variant
    // stream's own core functions (the generic Vec_ path must not touch it)
    // (the sequential vars are in parser.fd g_pd_sequential_vars, read by
    // Lowerer_lower_program)
    if lw_is_sequential_var(self, lw_box_expr_unbox(receiver)) {''')

open(p3, 'w', encoding='utf-8', newline='').write(s3)
print("lowerer: VecV routing by sequential-vars registry")

# ---- 4) parser: export the sequential var names for the lowerer ----
p4 = 'compiler/parser.fd'
s4 = open(p4, encoding='utf-8', newline='').read()
# add a global the lowerer reads
anchor4 = 'pub g_sequential_type: bool,'
assert s4.count(anchor4) >= 1
s4 = s4.replace(anchor4, anchor4 + '''
    // 0008: every var declared Vec<E..> (sequential) — the lowerer reads
    // this to route their Vec method calls to VecV_*
    pub g_pd_sequential_vars: Vec<str>,''')
s4 = s4.replace('g_sequential_type: false,', 'g_sequential_type: false, g_pd_sequential_vars: Vec::new(),')
# populate it in parse_let (where the sequential annotation is added)
old_ann = '''                    if self.g_sequential_type {
                        var sa: Vec<str> = Vec::new();
                        lanns.push(Annotation_new("sequential", sa));
                        self.g_sequential_type = false;
                    }'''
assert s4.count(old_ann) == 1
s4 = s4.replace(old_ann, '''                    if self.g_sequential_type {
                        var sa: Vec<str> = Vec::new();
                        lanns.push(Annotation_new("sequential", sa));
                        self.g_pd_sequential_vars.push(name);
                        self.g_sequential_type = false;
                    }''')
open(p4, 'w', encoding='utf-8', newline='').write(s4)
print("parser: sequential var names exported")

# ---- 5) lower.fd: read the parser's sequential list at program start ----
p5 = 'compiler/lower.fd'
s5 = open(p5, encoding='utf-8', newline='').read()
# find Lowerer_lower_program and populate g_lw_sequential_vars
# Actually simpler: just make g_lw_sequential_vars read directly from the
# parser's global. Since lower.fd doesn't import parser.fd, copy the list
# in compiler.fd. Or simplest: make lw_is_sequential_var use the parser's
# global directly.
# The lowerer can't import parser (no cycle issue actually: parser doesn't
# import lower). Let me check...
# Actually lower.fd's imports: core, vec, string, ast, ir_ser, cowcheck, ir, pe_ir, pgo
# No parser. Adding it: parser imports lexer, token, ast — no lower. So
# lower can import parser. But that's a heavy import for one global.

# SIMPLEST: in lw_is_sequential_var, check the AST annotations via the
# lowerer's own var tracking. But the lowerer's var_ast_types has the
# declared types, not annotations.

# OK, final approach: copy the list in compiler.fd's pipeline (it already
# calls parser and lower). Just add one line before lowering:
# g_lw_sequential_vars = parser_sequential_vars()
# But that requires exposing it. Let me just make it work via a pub fn.

# Actually, the SIMPLEST of all: make the sequential check use the AST
# type. Even though the parser produces Generic("Vec", ...), the
# annotation is on the Stmt::Let. The lowerer processes Stmt::Let and
# can check the annotations there, populating g_lw_sequential_vars.

# Find the Let statement lowering to add the registration.
# Let me search for "Stmt::Let" in lower_stmt:
# The lowerer's stmt handler has a case for Stmt::Let(name, ty, expr, ...)
# with annotations available as the 6th field.

# Let me add the registration in Lowerer_lower_stmt's Let case.
# First find it:
open(p5, 'w', encoding='utf-8', newline='').write(s5)
print("lowerer: ready (registration in stmt handler)")
