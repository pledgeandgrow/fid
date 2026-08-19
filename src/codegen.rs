use crate::ast::*;
use std::collections::HashMap;
use std::fmt::Write;

pub struct CodeGen {
    output: String,
    label_counter: usize,
    string_consts: HashMap<String, String>,
    float_consts: HashMap<u64, String>,
    locals: Vec<HashMap<String, u32>>,
    var_types: Vec<HashMap<String, String>>,
    next_offset: u32,
    frame_size: u32,
    struct_layouts: HashMap<String, Vec<(String, u32)>>,
    impl_methods: HashMap<String, Vec<String>>,
}

impl CodeGen {
    pub fn new() -> Self {
        CodeGen {
            output: String::new(),
            label_counter: 0,
            string_consts: HashMap::new(),
            float_consts: HashMap::new(),
            locals: vec![],
            var_types: vec![],
            next_offset: 0,
            frame_size: 0,
            struct_layouts: HashMap::new(),
            impl_methods: HashMap::new(),
        }
    }

    fn push_scope(&mut self) {
        self.locals.push(HashMap::new());
        self.var_types.push(HashMap::new());
    }

    fn pop_scope(&mut self) {
        self.locals.pop();
        self.var_types.pop();
    }

    fn declare_var(&mut self, name: &str) -> u32 {
        let offset = self.next_offset;
        self.next_offset += 8;
        self.locals.last_mut().unwrap().insert(name.to_string(), offset);
        offset
    }

    fn declare_var_typed(&mut self, name: &str, struct_name: &str) -> u32 {
        let offset = self.declare_var(name);
        self.var_types.last_mut().unwrap().insert(name.to_string(), struct_name.to_string());
        offset
    }

    fn lookup_var_type(&self, name: &str) -> Option<String> {
        self.var_types.iter().rev().find_map(|s| s.get(name).cloned())
    }

    fn lookup_var(&self, name: &str) -> Option<u32> {
        self.locals.iter().rev().find_map(|s| s.get(name).copied())
    }

    fn get_field_offset(&self, field: &str) -> u32 {
        // Search all struct layouts for this field name
        // Returns the first match (simplified — real impl tracks struct type)
        for layout in self.struct_layouts.values() {
            for (name, offset) in layout {
                if name == field {
                    return *offset;
                }
            }
        }
        0
    }

    pub fn generate(&mut self, program: &Program) -> String {
        // Register struct layouts for field access codegen
        for s in &program.structs {
            let layout: Vec<(String, u32)> = s
                .fields
                .iter()
                .enumerate()
                .map(|(i, f)| (f.name.clone(), (i as u32) * 8))
                .collect();
            self.struct_layouts.insert(s.name.clone(), layout);
        }

        // Register impl method names per struct
        for imp in &program.impls {
            let methods: Vec<String> = imp.methods.iter().map(|m| m.name.clone()).collect();
            self.impl_methods.insert(imp.struct_name.clone(), methods);
        }

        // Header
        writeln!(self.output, ".section .text").unwrap();
        writeln!(self.output, ".globl _start\n").unwrap();

        // _start calls main then exits
        writeln!(self.output, "_start:").unwrap();
        writeln!(self.output, "    bl p_main").unwrap();
        writeln!(self.output, "    mov x8, #93").unwrap(); // exit syscall
        writeln!(self.output, "    mov x0, #0").unwrap();
        writeln!(self.output, "    svc #0\n").unwrap();

        // Generate each function
        for func in &program.funcs {
            self.gen_func(func);
        }

        // Generate impl methods
        for imp in &program.impls {
            for method in &imp.methods {
                self.gen_method(&imp.struct_name, method);
            }
        }

        // String constants
        if !self.string_consts.is_empty() || !self.float_consts.is_empty() {
            writeln!(self.output, "\n.section .rodata").unwrap();
            for (label, s) in &self.string_consts {
                writeln!(self.output, "{}:", label).unwrap();
                writeln!(self.output, "    .asciz \"{}\"", self.escape_string(s)).unwrap();
            }
            // Float constants
            for (bits, label) in &self.float_consts {
                let f = f64::from_bits(*bits);
                writeln!(self.output, "{}:", label).unwrap();
                writeln!(self.output, "    .double {}", f).unwrap();
            }
        }

        self.output.clone()
    }

    fn escape_string(&self, s: &str) -> String {
        s.replace('\\', "\\\\")
            .replace('"', "\\\"")
            .replace('\n', "\\n")
            .replace('\t', "\\t")
            .replace('\r', "\\r")
    }

    fn fresh_label(&mut self, prefix: &str) -> String {
        let label = format!(".{}_{}", prefix, self.label_counter);
        self.label_counter += 1;
        label
    }

    fn intern_string(&mut self, s: &str) -> String {
        if let Some(label) = self.string_consts.get(s) {
            return label.clone();
        }
        let label = format!(".str_{}", self.string_consts.len());
        self.string_consts.insert(s.to_string(), label.clone());
        label
    }

    fn intern_float(&mut self, f: f64) -> String {
        let bits = f.to_bits();
        if let Some(label) = self.float_consts.get(&bits) {
            return label.clone();
        }
        let label = format!(".float_{}", self.float_consts.len());
        self.float_consts.insert(bits, label.clone());
        label
    }

    fn gen_func(&mut self, func: &Func) {
        let mangled = format!("p_{}", func.name);
        writeln!(self.output, "{}:", mangled).unwrap();

        self.next_offset = 0;
        let mut var_count = func.params.len() + count_lets(&func.body);
        var_count += 8; // temporaries
        self.frame_size = ((var_count as u32) * 8 + 15) & !15;
        self.locals = vec![HashMap::new()];
        self.var_types = vec![HashMap::new()];

        // Prologue
        writeln!(self.output, "    stp x29, x30, [sp, #-16]!").unwrap();
        writeln!(self.output, "    mov x29, sp").unwrap();
        writeln!(self.output, "    sub sp, sp, #{}", self.frame_size).unwrap();

        // Store params in stack slots
        for (i, param) in func.params.iter().enumerate() {
            let offset = self.declare_var(&param.name);
            if let Type::Named(sn) = &param.ty {
                self.var_types.last_mut().unwrap().insert(param.name.clone(), sn.clone());
            }
            if i < 8 {
                writeln!(self.output, "    str x{}, [sp, #{}]", i, offset).unwrap();
            }
        }

        // Generate body
        for stmt in &func.body {
            self.gen_stmt(stmt);
        }

        // Epilogue (default return)
        self.gen_epilogue();
        writeln!(self.output).unwrap();
    }

    fn gen_method(&mut self, struct_name: &str, func: &Func) {
        let mangled = format!("p_{}_{}", struct_name, func.name);
        writeln!(self.output, "{}:", mangled).unwrap();

        self.next_offset = 0;
        let mut var_count = func.params.len() + count_lets(&func.body);
        var_count += 8; // temporaries
        self.frame_size = ((var_count as u32) * 8 + 15) & !15;
        self.locals = vec![HashMap::new()];
        self.var_types = vec![HashMap::new()];

        // Prologue
        writeln!(self.output, "    stp x29, x30, [sp, #-16]!").unwrap();
        writeln!(self.output, "    mov x29, sp").unwrap();
        writeln!(self.output, "    sub sp, sp, #{}", self.frame_size).unwrap();

        // Store params in stack slots
        for (i, param) in func.params.iter().enumerate() {
            let offset = self.declare_var(&param.name);
            // Track struct-typed params for method calls
            if let Type::Named(sn) = &param.ty {
                self.var_types.last_mut().unwrap().insert(param.name.clone(), sn.clone());
            }
            if i < 8 {
                writeln!(self.output, "    str x{}, [sp, #{}]", i, offset).unwrap();
            }
        }

        // Generate body
        for stmt in &func.body {
            self.gen_stmt(stmt);
        }

        // Epilogue (default return)
        self.gen_epilogue();
        writeln!(self.output).unwrap();
    }

    fn gen_epilogue(&mut self) {
        writeln!(self.output, "    add sp, sp, #{}", self.frame_size).unwrap();
        writeln!(self.output, "    ldp x29, x30, [sp], #16").unwrap();
        writeln!(self.output, "    ret").unwrap();
    }

    fn gen_stmt(&mut self, stmt: &Stmt) {
        match stmt {
            Stmt::Let { name, expr, .. } => {
                // Track struct type for method call resolution
                let struct_name = match expr {
                    Expr::StructLit { name: sn, .. } => Some(sn.clone()),
                    _ => None,
                };
                self.gen_expr(expr);
                let offset = if let Some(sn) = &struct_name {
                    self.declare_var_typed(name, sn)
                } else {
                    self.declare_var(name)
                };
                writeln!(self.output, "    str x0, [sp, #{}]", offset).unwrap();
            }
            Stmt::LetUninit { name, .. } => {
                // Just declare the variable — no initialization
                self.declare_var(name);
            }
            Stmt::LetTuple { names, expr, .. } => {
                // Generate the tuple expression, then extract each field
                self.gen_expr(expr);
                // x0 holds the tuple pointer; extract each field
                for (i, name) in names.iter().enumerate() {
                    let offset = self.declare_var(name);
                    if i > 0 {
                        // Reload tuple pointer (may have been clobbered)
                        // For now, just store the same value — the interpreter
                        // handles tuple extraction correctly
                    }
                    writeln!(self.output, "    str x0, [sp, #{}]", offset).unwrap();
                }
            }
            Stmt::Assign { target, op, expr, .. } => {
                if let Some(offset) = self.lookup_var(target) {
                    match op {
                        AssignOp::Assign => {
                            self.gen_expr(expr);
                            writeln!(self.output, "    str x0, [sp, #{}]", offset).unwrap();
                        }
                        AssignOp::AddAssign => {
                            writeln!(self.output, "    ldr x0, [sp, #{}]", offset).unwrap();
                            let temp = self.next_offset;
                            self.next_offset += 8;
                            writeln!(self.output, "    str x0, [sp, #{}]", temp).unwrap();
                            self.gen_expr(expr);
                            writeln!(self.output, "    mov x1, x0").unwrap();
                            writeln!(self.output, "    ldr x0, [sp, #{}]", temp).unwrap();
                            self.next_offset -= 8;
                            writeln!(self.output, "    add x0, x0, x1").unwrap();
                            writeln!(self.output, "    str x0, [sp, #{}]", offset).unwrap();
                        }
                        AssignOp::SubAssign => {
                            writeln!(self.output, "    ldr x0, [sp, #{}]", offset).unwrap();
                            let temp = self.next_offset;
                            self.next_offset += 8;
                            writeln!(self.output, "    str x0, [sp, #{}]", temp).unwrap();
                            self.gen_expr(expr);
                            writeln!(self.output, "    mov x1, x0").unwrap();
                            writeln!(self.output, "    ldr x0, [sp, #{}]", temp).unwrap();
                            self.next_offset -= 8;
                            writeln!(self.output, "    sub x0, x0, x1").unwrap();
                            writeln!(self.output, "    str x0, [sp, #{}]", offset).unwrap();
                        }
                        AssignOp::MulAssign => {
                            writeln!(self.output, "    ldr x0, [sp, #{}]", offset).unwrap();
                            let temp = self.next_offset;
                            self.next_offset += 8;
                            writeln!(self.output, "    str x0, [sp, #{}]", temp).unwrap();
                            self.gen_expr(expr);
                            writeln!(self.output, "    mov x1, x0").unwrap();
                            writeln!(self.output, "    ldr x0, [sp, #{}]", temp).unwrap();
                            self.next_offset -= 8;
                            writeln!(self.output, "    mul x0, x0, x1").unwrap();
                            writeln!(self.output, "    str x0, [sp, #{}]", offset).unwrap();
                        }
                        _ => {
                            // Other compound ops: fall back to simple assign
                            self.gen_expr(expr);
                            writeln!(self.output, "    str x0, [sp, #{}]", offset).unwrap();
                        }
                    }
                }
            }
            Stmt::IndexAssign { array, index, op, expr, .. } => {
                // Evaluate index
                self.gen_expr(index);
                let idx_temp = self.next_offset;
                self.next_offset += 8;
                writeln!(self.output, "    str x0, [sp, #{}]", idx_temp).unwrap();

                // Evaluate RHS
                self.gen_expr(expr);
                let val_temp = self.next_offset;
                self.next_offset += 8;
                writeln!(self.output, "    str x0, [sp, #{}]", val_temp).unwrap();

                if let Some(offset) = self.lookup_var(array) {
                    // base = sp + offset
                    // addr = base + index * 8
                    writeln!(self.output, "    ldr x0, [sp, #{}]", idx_temp).unwrap();
                    writeln!(self.output, "    lsl x0, x0, #3").unwrap();
                    writeln!(self.output, "    add x3, sp, #{}", offset).unwrap();
                    writeln!(self.output, "    add x3, x3, x0").unwrap();

                    match op {
                        AssignOp::Assign => {
                            writeln!(self.output, "    ldr x0, [sp, #{}]", val_temp).unwrap();
                            writeln!(self.output, "    str x0, [x3]").unwrap();
                        }
                        _ => {
                            // Compound: load current, apply op, store
                            writeln!(self.output, "    ldr x1, [x3]").unwrap();
                            writeln!(self.output, "    ldr x0, [sp, #{}]", val_temp).unwrap();
                            match op {
                                AssignOp::AddAssign => writeln!(self.output, "    add x0, x1, x0").unwrap(),
                                AssignOp::SubAssign => writeln!(self.output, "    sub x0, x1, x0").unwrap(),
                                AssignOp::MulAssign => writeln!(self.output, "    mul x0, x1, x0").unwrap(),
                                AssignOp::DivAssign => writeln!(self.output, "    sdiv x0, x1, x0").unwrap(),
                                AssignOp::ModAssign => {
                                    writeln!(self.output, "    sdiv x2, x1, x0").unwrap();
                                    writeln!(self.output, "    msub x0, x2, x0, x1").unwrap();
                                }
                                AssignOp::BitAndAssign => writeln!(self.output, "    and x0, x1, x0").unwrap(),
                                AssignOp::BitOrAssign => writeln!(self.output, "    orr x0, x1, x0").unwrap(),
                                AssignOp::BitXorAssign => writeln!(self.output, "    eor x0, x1, x0").unwrap(),
                                AssignOp::ShlAssign => writeln!(self.output, "    lsl x0, x1, x0").unwrap(),
                                AssignOp::ShrAssign => writeln!(self.output, "    asr x0, x1, x0").unwrap(),
                                AssignOp::Assign => unreachable!(),
                            }
                            writeln!(self.output, "    str x0, [x3]").unwrap();
                        }
                    }
                }
                self.next_offset -= 16;
            }
            Stmt::Expr(expr, _) => {
                self.gen_expr(expr);
            }
            Stmt::DerefAssign { .. } => {
                // Handled by new backend, not this legacy ARM64 codegen
            }
            Stmt::Return(expr, _) => {
                if let Some(e) = expr {
                    self.gen_expr(e);
                }
                self.gen_epilogue();
            }
            Stmt::If {
                cond,
                then_block,
                else_block,
                ..
            } => {
                let else_label = self.fresh_label("else");
                let end_label = self.fresh_label("endif");

                self.gen_expr(cond);
                writeln!(self.output, "    cmp x0, #0").unwrap();
                writeln!(self.output, "    beq {}", else_label).unwrap();

                self.push_scope();
                for s in then_block {
                    self.gen_stmt(s);
                }
                self.pop_scope();

                writeln!(self.output, "    b {}", end_label).unwrap();
                writeln!(self.output, "{}:", else_label).unwrap();

                if let Some(else_b) = else_block {
                    self.push_scope();
                    for s in else_b {
                        self.gen_stmt(s);
                    }
                    self.pop_scope();
                }

                writeln!(self.output, "{}:", end_label).unwrap();
            }
            Stmt::While { cond, body, .. } => {
                let start_label = self.fresh_label("while_start");
                let end_label = self.fresh_label("while_end");

                writeln!(self.output, "{}:", start_label).unwrap();
                self.gen_expr(cond);
                writeln!(self.output, "    cmp x0, #0").unwrap();
                writeln!(self.output, "    beq {}", end_label).unwrap();

                self.push_scope();
                for s in body {
                    self.gen_stmt(s);
                }
                self.pop_scope();

                writeln!(self.output, "    b {}", start_label).unwrap();
                writeln!(self.output, "{}:", end_label).unwrap();
            }
            Stmt::For { var, iterator, body, .. } => {
                let start_label = self.fresh_label("for_start");
                let end_label = self.fresh_label("for_end");

                match iterator {
                    Expr::Range { start, end, inclusive } => {
                        // Evaluate start and end
                        self.gen_expr(start);
                        let start_offset = self.declare_var(var);
                        writeln!(self.output, "    str x0, [sp, #{}]", start_offset).unwrap();

                        self.gen_expr(end);
                        let end_offset = self.next_offset;
                        self.next_offset += 8;
                        writeln!(self.output, "    str x0, [sp, #{}]", end_offset).unwrap();

                        writeln!(self.output, "{}:", start_label).unwrap();
                        // Check if counter < end (or <= for inclusive)
                        writeln!(self.output, "    ldr x0, [sp, #{}]", start_offset).unwrap();
                        writeln!(self.output, "    ldr x1, [sp, #{}]", end_offset).unwrap();
                        writeln!(self.output, "    cmp x0, x1").unwrap();
                        if *inclusive {
                            writeln!(self.output, "    b.gt {}", end_label).unwrap();
                        } else {
                            writeln!(self.output, "    b.ge {}", end_label).unwrap();
                        }

                        self.push_scope();
                        for s in body {
                            self.gen_stmt(s);
                        }
                        self.pop_scope();

                        // Increment counter
                        writeln!(self.output, "    ldr x0, [sp, #{}]", start_offset).unwrap();
                        writeln!(self.output, "    add x0, x0, #1").unwrap();
                        writeln!(self.output, "    str x0, [sp, #{}]", start_offset).unwrap();

                        writeln!(self.output, "    b {}", start_label).unwrap();
                        writeln!(self.output, "{}:", end_label).unwrap();
                        self.next_offset -= 8;
                    }
                    _ => {
                        // Non-range iterator: evaluate once, iterate once (simplified)
                        self.gen_expr(iterator);
                        let iter_offset = self.declare_var(var);
                        writeln!(self.output, "    str x0, [sp, #{}]", iter_offset).unwrap();

                        writeln!(self.output, "{}:", start_label).unwrap();
                        self.push_scope();
                        for s in body {
                            self.gen_stmt(s);
                        }
                        self.pop_scope();
                        writeln!(self.output, "    b {}", end_label).unwrap();
                        writeln!(self.output, "{}:", end_label).unwrap();
                    }
                }
            }
            Stmt::Loop { body, .. } => {
                let start_label = self.fresh_label("loop_start");
                let end_label = self.fresh_label("loop_end");

                writeln!(self.output, "{}:", start_label).unwrap();
                self.push_scope();
                for s in body {
                    self.gen_stmt(s);
                }
                self.pop_scope();
                writeln!(self.output, "    b {}", start_label).unwrap();
                writeln!(self.output, "{}:", end_label).unwrap();
            }
            Stmt::Break(_, _) => {
                // Simplified: jump to end of current block
                // Full impl: track break/continue labels
                writeln!(self.output, "    b .loop_end_{}", self.label_counter).unwrap();
            }
            Stmt::Continue(_) => {
                writeln!(self.output, "    b .loop_start_{}", self.label_counter).unwrap();
            }
            Stmt::Defer(body, _) => {
                // Simplified: execute defer body immediately (proper impl: queue for scope exit)
                self.push_scope();
                for s in body {
                    self.gen_stmt(s);
                }
                self.pop_scope();
            }
            Stmt::Match { expr, arms, .. } => {
                // Evaluate match expression
                self.gen_expr(expr);
                let match_temp = self.next_offset;
                self.next_offset += 8;
                writeln!(self.output, "    str x0, [sp, #{}]", match_temp).unwrap();

                let end_label = self.fresh_label("match_end");
                let mut arm_labels = Vec::new();

                for (i, arm) in arms.iter().enumerate() {
                    let arm_label = self.fresh_label(&format!("match_arm_{}", i));
                    arm_labels.push(arm_label.clone());

                    match &arm.pattern {
                        Pattern::IntLit(n) => {
                            writeln!(self.output, "    ldr x0, [sp, #{}]", match_temp).unwrap();
                            writeln!(self.output, "    mov x1, #{}", n).unwrap();
                            writeln!(self.output, "    cmp x0, x1").unwrap();
                            writeln!(self.output, "    b.eq {}", arm_label).unwrap();
                        }
                        Pattern::BoolLit(b) => {
                            writeln!(self.output, "    ldr x0, [sp, #{}]", match_temp).unwrap();
                            writeln!(self.output, "    mov x1, #{}", if *b { 1 } else { 0 }).unwrap();
                            writeln!(self.output, "    cmp x0, x1").unwrap();
                            writeln!(self.output, "    b.eq {}", arm_label).unwrap();
                        }
                        Pattern::Wildcard => {
                            // Always matches
                            writeln!(self.output, "    b {}", arm_label).unwrap();
                        }
                        Pattern::Ident(name) => {
                            // Bind variable and jump to arm
                            let offset = self.declare_var(name);
                            writeln!(self.output, "    ldr x0, [sp, #{}]", match_temp).unwrap();
                            writeln!(self.output, "    str x0, [sp, #{}]", offset).unwrap();
                            writeln!(self.output, "    b {}", arm_label).unwrap();
                        }
                        Pattern::EnumVariant { .. } => {
                            // Simplified: just jump to arm
                            writeln!(self.output, "    b {}", arm_label).unwrap();
                        }
                        Pattern::StrLit(_) | Pattern::Range { .. } => {
                            // Simplified: just jump to arm
                            writeln!(self.output, "    b {}", arm_label).unwrap();
                        }
                    }
                }

                // No arm matched: jump to end
                writeln!(self.output, "    b {}", end_label).unwrap();

                for (i, arm) in arms.iter().enumerate() {
                    writeln!(self.output, "{}:", arm_labels[i]).unwrap();
                    self.push_scope();
                    for s in &arm.body {
                        self.gen_stmt(s);
                    }
                    self.pop_scope();
                    writeln!(self.output, "    b {}", end_label).unwrap();
                }

                writeln!(self.output, "{}:", end_label).unwrap();
                self.next_offset -= 8;
            }
            Stmt::Use { .. } => {
                // No-op: imports resolved at compile time
            }
            Stmt::Handle { .. } => {
                // Not yet supported in legacy ARM64 codegen
            }
            Stmt::Do { .. } => {
                // Not yet supported in legacy ARM64 codegen
            }
            Stmt::RegionBlock { body, .. } => {
                // Region creation/freeing not yet implemented in codegen
                for s in body {
                    self.gen_stmt(s);
                }
            }
            Stmt::ComptimeGen { .. } => {
                // ComptimeGen is processed during comptime substitution; no-op here
            }
            Stmt::Sandbox { body, .. } => {
                // Sandbox is transparent at codegen — safety is compile-time only
                for s in body {
                    self.gen_stmt(s);
                }
            }
            Stmt::Unsafe { body, .. } | Stmt::Txn { body, .. } => {
                // Unsafe/Txn are transparent at codegen — safety/rollback is compile-time only
                for s in body {
                    self.gen_stmt(s);
                }
            }
            Stmt::GradBlock { body, .. } => {
                for s in body {
                    self.gen_stmt(s);
                }
            }
            Stmt::Backward { .. } => {
                // Marker for autograd pass — no-op in codegen
            }
        }
    }

    fn gen_expr(&mut self, expr: &Expr) {
        match expr {
            Expr::IntLit(n) => {
                writeln!(self.output, "    mov x0, #{}", n).unwrap();
            }
            Expr::FloatLit(n) => {
                // Store float literal in rodata and load it
                let label = self.intern_float(*n);
                writeln!(self.output, "    adr x0, {}", label).unwrap();
                writeln!(self.output, "    ldr d0, [x0]").unwrap();
                // Also store as int in x0 for compatibility
                writeln!(self.output, "    mov x0, #{}", *n as i64).unwrap();
            }
            Expr::StringLit(s) => {
                let label = self.intern_string(s);
                writeln!(self.output, "    adr x0, {}", label).unwrap();
            }
            Expr::BoolLit(b) => {
                writeln!(self.output, "    mov x0, #{}", if *b { 1 } else { 0 }).unwrap();
            }
            Expr::Ident(name) => {
                if let Some(offset) = self.lookup_var(name) {
                    writeln!(self.output, "    ldr x0, [sp, #{}]", offset).unwrap();
                } else {
                    writeln!(self.output, "    mov x0, #0").unwrap();
                }
            }
            Expr::Binary { op, left, right } => {
                self.gen_expr(left);
                let temp = self.next_offset;
                self.next_offset += 8;
                writeln!(self.output, "    str x0, [sp, #{}]", temp).unwrap();
                self.gen_expr(right);
                writeln!(self.output, "    mov x1, x0").unwrap();
                writeln!(self.output, "    ldr x0, [sp, #{}]", temp).unwrap();
                self.next_offset -= 8;

                match op {
                    BinOp::Add => writeln!(self.output, "    add x0, x0, x1").unwrap(),
                    BinOp::Sub => writeln!(self.output, "    sub x0, x0, x1").unwrap(),
                    BinOp::Mul => writeln!(self.output, "    mul x0, x0, x1").unwrap(),
                    BinOp::Div => {
                        writeln!(self.output, "    sdiv x0, x0, x1").unwrap()
                    }
                    BinOp::Mod => {
                        writeln!(self.output, "    sdiv x2, x0, x1").unwrap();
                        writeln!(self.output, "    msub x0, x2, x1, x0").unwrap();
                    }
                    BinOp::Eq => {
                        writeln!(self.output, "    cmp x0, x1").unwrap();
                        writeln!(self.output, "    cset x0, eq").unwrap();
                    }
                    BinOp::Neq => {
                        writeln!(self.output, "    cmp x0, x1").unwrap();
                        writeln!(self.output, "    cset x0, ne").unwrap();
                    }
                    BinOp::Lt => {
                        writeln!(self.output, "    cmp x0, x1").unwrap();
                        writeln!(self.output, "    cset x0, lt").unwrap();
                    }
                    BinOp::Gt => {
                        writeln!(self.output, "    cmp x0, x1").unwrap();
                        writeln!(self.output, "    cset x0, gt").unwrap();
                    }
                    BinOp::LtEq => {
                        writeln!(self.output, "    cmp x0, x1").unwrap();
                        writeln!(self.output, "    cset x0, le").unwrap();
                    }
                    BinOp::GtEq => {
                        writeln!(self.output, "    cmp x0, x1").unwrap();
                        writeln!(self.output, "    cset x0, ge").unwrap();
                    }
                    BinOp::And => {
                        writeln!(self.output, "    and x0, x0, x1").unwrap()
                    }
                    BinOp::Or => {
                        writeln!(self.output, "    orr x0, x0, x1").unwrap()
                    }
                    BinOp::BitAnd => {
                        writeln!(self.output, "    and x0, x0, x1").unwrap()
                    }
                    BinOp::BitOr => {
                        writeln!(self.output, "    orr x0, x0, x1").unwrap()
                    }
                    BinOp::BitXor => {
                        writeln!(self.output, "    eor x0, x0, x1").unwrap()
                    }
                    BinOp::Shl => {
                        writeln!(self.output, "    lsl x0, x0, x1").unwrap()
                    }
                    BinOp::Shr => {
                        writeln!(self.output, "    asr x0, x0, x1").unwrap()
                    }
                }
            }
            Expr::Unary { op, expr } => {
                self.gen_expr(expr);
                match op {
                    UnaryOp::Neg => {
                        writeln!(self.output, "    neg x0, x0").unwrap()
                    }
                    UnaryOp::Not => {
                        writeln!(self.output, "    cmp x0, #0").unwrap();
                        writeln!(self.output, "    cset x0, eq").unwrap();
                    }
                    UnaryOp::BitNot => {
                        writeln!(self.output, "    mvn x0, x0").unwrap()
                    }
                }
            }
            Expr::Call { callee, args } => {
                if callee == "print" {
                    if !args.is_empty() {
                        self.gen_expr(&args[0]);
                        writeln!(self.output, "    mov x1, x0").unwrap();
                        writeln!(self.output, "    mov x2, #64").unwrap();
                        writeln!(self.output, "    mov x0, #1").unwrap();
                        writeln!(self.output, "    mov x8, #64").unwrap();
                        writeln!(self.output, "    svc #0").unwrap();
                    }
                    writeln!(self.output, "    mov x0, #0").unwrap();
                    return;
                }

                // Eval args, store on stack
                for arg in args.iter() {
                    self.gen_expr(arg);
                    let temp = self.next_offset;
                    self.next_offset += 8;
                    writeln!(self.output, "    str x0, [sp, #{}]", temp).unwrap();
                }
                // Load args into x0-x7 (reverse)
                for (i, _) in args.iter().enumerate().rev() {
                    self.next_offset -= 8;
                    if i < 8 {
                        writeln!(self.output, "    ldr x{}, [sp, #{}]", i, self.next_offset).unwrap();
                    }
                }

                let mangled = format!("p_{}", callee);
                writeln!(self.output, "    bl {}", mangled).unwrap();
            }
            Expr::FieldAccess { expr, field } => {
                // Evaluate the struct expression (pointer in x0)
                self.gen_expr(expr);
                // Look up field offset — for Phase 1, all fields are 8 bytes
                // Real impl: track struct layouts
                // For now: search field index * 8
                // We need struct layout info — store it during generate()
                let offset = self.get_field_offset(field);
                if offset > 0 {
                    writeln!(self.output, "    ldr x0, [x0, #{}]", offset).unwrap();
                } else {
                    writeln!(self.output, "    ldr x0, [x0, #0]").unwrap();
                }
            }
            Expr::OptionalFieldAccess { expr, .. } => {
                // Simplified: just evaluate expr (lowerer handles the logic)
                self.gen_expr(expr);
            }
            Expr::StructLit { name, fields, .. } => {
                // Allocate space for struct on stack
                let struct_size = fields.len() * 8;
                let struct_offset = self.next_offset;
                self.next_offset += struct_size as u32;

                // Evaluate each field and store at offset
                for (i, (_, fexpr)) in fields.iter().enumerate() {
                    self.gen_expr(fexpr);
                    let field_offset = struct_offset + (i as u32) * 8;
                    writeln!(self.output, "    str x0, [sp, #{}]", field_offset).unwrap();
                }

                // Return pointer to struct (stack address)
                writeln!(self.output, "    add x0, sp, #{}", struct_offset).unwrap();

                let _ = name;
            }
            Expr::If {
                cond,
                then_block,
                else_block,
            } => {
                let else_label = self.fresh_label("else");
                let end_label = self.fresh_label("endif");

                self.gen_expr(cond);
                writeln!(self.output, "    cmp x0, #0").unwrap();
                writeln!(self.output, "    beq {}", else_label).unwrap();

                self.push_scope();
                for s in then_block {
                    self.gen_stmt(s);
                }
                self.pop_scope();

                writeln!(self.output, "    b {}", end_label).unwrap();
                writeln!(self.output, "{}:", else_label).unwrap();
                if let Some(else_b) = else_block {
                    self.push_scope();
                    for s in else_b {
                        self.gen_stmt(s);
                    }
                    self.pop_scope();
                }
                writeln!(self.output, "{}:", end_label).unwrap();
            }
            Expr::Range { start, end, .. } => {
                // Range evaluates to start value (for standalone use)
                // For loops handle ranges specially in gen_stmt
                self.gen_expr(start);
            }
            Expr::Block(stmts) => {
                self.push_scope();
                for s in stmts {
                    self.gen_stmt(s);
                }
                self.pop_scope();
            }
            Expr::MethodCall { receiver, method, args } => {
                // Determine struct name from receiver
                let struct_name = match receiver.as_ref() {
                    Expr::Ident(name) => self.lookup_var_type(name),
                    _ => None,
                };

                // Evaluate receiver (struct pointer in x0)
                self.gen_expr(receiver);
                // Store receiver as first arg
                let temp = self.next_offset;
                self.next_offset += 8;
                writeln!(self.output, "    str x0, [sp, #{}]", temp).unwrap();

                // Evaluate other args
                for arg in args {
                    self.gen_expr(arg);
                    let arg_temp = self.next_offset;
                    self.next_offset += 8;
                    writeln!(self.output, "    str x0, [sp, #{}]", arg_temp).unwrap();
                }

                // Load args: x0 = receiver, x1.. = other args
                writeln!(self.output, "    ldr x0, [sp, #{}]", temp).unwrap();
                for (i, _) in args.iter().enumerate() {
                    let arg_offset = temp + ((i + 1) as u32) * 8;
                    if i + 1 < 8 {
                        writeln!(self.output, "    ldr x{}, [sp, #{}]", i + 1, arg_offset).unwrap();
                    }
                }
                self.next_offset -= 8 + (args.len() as u32) * 8;

                // Call mangled method name
                if let Some(sn) = &struct_name {
                    writeln!(self.output, "    bl p_{}_{}", sn, method).unwrap();
                } else {
                    writeln!(self.output, "    bl p_{}_{}", "?", method).unwrap();
                }
            }
            Expr::ArrayLit { elem_type: _, elements } => {
                // Allocate array on stack: each element is 8 bytes
                let arr_offset = self.next_offset;
                let arr_size = (elements.len() as u32) * 8;
                self.next_offset += arr_size;

                for (i, e) in elements.iter().enumerate() {
                    self.gen_expr(e);
                    let elem_offset = arr_offset + (i as u32) * 8;
                    writeln!(self.output, "    str x0, [sp, #{}]", elem_offset).unwrap();
                }

                // Return base address of array
                writeln!(self.output, "    mov x0, sp").unwrap();
                writeln!(self.output, "    add x0, x0, #{}", arr_offset).unwrap();
            }
            Expr::Index { array, index } => {
                // Evaluate array base address
                self.gen_expr(array);
                let base_temp = self.next_offset;
                self.next_offset += 8;
                writeln!(self.output, "    str x0, [sp, #{}]", base_temp).unwrap();

                // Evaluate index
                self.gen_expr(index);

                // Compute address: base + index * 8
                writeln!(self.output, "    ldr x1, [sp, #{}]", base_temp).unwrap();
                writeln!(self.output, "    lsl x0, x0, #3").unwrap(); // index * 8
                writeln!(self.output, "    add x0, x1, x0").unwrap();
                writeln!(self.output, "    ldr x0, [x0]").unwrap();
                self.next_offset -= 8;
            }
            Expr::FString { parts } => {
                // For f-strings, we concatenate all parts into a single string
                // Simplified: just evaluate expression parts and ignore literal parts
                // Full impl would use a print/format function
                for part in parts {
                    if let FStringPart::Expr(e) = part {
                        self.gen_expr(e);
                    }
                }
            }
            Expr::Match { expr, arms } => {
                // Evaluate match expression
                self.gen_expr(expr);
                let match_temp = self.next_offset;
                self.next_offset += 8;
                writeln!(self.output, "    str x0, [sp, #{}]", match_temp).unwrap();

                let end_label = self.fresh_label("match_expr_end");
                let mut arm_labels = Vec::new();

                for (i, arm) in arms.iter().enumerate() {
                    let arm_label = self.fresh_label(&format!("match_expr_arm_{}", i));
                    arm_labels.push(arm_label.clone());

                    match &arm.pattern {
                        Pattern::IntLit(n) => {
                            writeln!(self.output, "    ldr x0, [sp, #{}]", match_temp).unwrap();
                            writeln!(self.output, "    mov x1, #{}", n).unwrap();
                            writeln!(self.output, "    cmp x0, x1").unwrap();
                            writeln!(self.output, "    b.eq {}", arm_label).unwrap();
                        }
                        Pattern::BoolLit(b) => {
                            writeln!(self.output, "    ldr x0, [sp, #{}]", match_temp).unwrap();
                            writeln!(self.output, "    mov x1, #{}", if *b { 1 } else { 0 }).unwrap();
                            writeln!(self.output, "    cmp x0, x1").unwrap();
                            writeln!(self.output, "    b.eq {}", arm_label).unwrap();
                        }
                        Pattern::Wildcard => {
                            writeln!(self.output, "    b {}", arm_label).unwrap();
                        }
                        Pattern::Ident(name) => {
                            let offset = self.declare_var(name);
                            writeln!(self.output, "    ldr x0, [sp, #{}]", match_temp).unwrap();
                            writeln!(self.output, "    str x0, [sp, #{}]", offset).unwrap();
                            writeln!(self.output, "    b {}", arm_label).unwrap();
                        }
                        Pattern::EnumVariant { .. } => {
                            writeln!(self.output, "    b {}", arm_label).unwrap();
                        }
                        Pattern::StrLit(_) | Pattern::Range { .. } => {
                            writeln!(self.output, "    b {}", arm_label).unwrap();
                        }
                    }
                }

                writeln!(self.output, "    b {}", end_label).unwrap();

                for (i, arm) in arms.iter().enumerate() {
                    writeln!(self.output, "{}:", arm_labels[i]).unwrap();
                    self.push_scope();
                    for s in &arm.body {
                        self.gen_stmt(s);
                    }
                    self.pop_scope();
                    writeln!(self.output, "    b {}", end_label).unwrap();
                }

                writeln!(self.output, "{}:", end_label).unwrap();
                self.next_offset -= 8;
            }
            Expr::EnumLit { enum_name: _, variant, args } => {
                // Simplified: store variant index and args on stack
                // For unit variants: just store the variant index in x0
                let mut arg_offsets = Vec::new();
                for arg in args {
                    self.gen_expr(arg);
                    let temp = self.next_offset;
                    self.next_offset += 8;
                    writeln!(self.output, "    str x0, [sp, #{}]", temp).unwrap();
                    arg_offsets.push(temp);
                }
                // Store variant name as string label (simplified: use index 0)
                writeln!(self.output, "    mov x0, #0").unwrap();
            }
            Expr::Ref { expr, is_mut: _ } => {
                // Take address of variable
                if let Expr::Ident(name) = expr.as_ref() {
                    if let Some(offset) = self.lookup_var(name) {
                        writeln!(self.output, "    add x0, sp, #{}", offset).unwrap();
                    }
                } else {
                    // For non-ident expressions, allocate temp and take address
                    self.gen_expr(expr);
                    let temp = self.next_offset;
                    self.next_offset += 8;
                    writeln!(self.output, "    str x0, [sp, #{}]", temp).unwrap();
                    writeln!(self.output, "    add x0, sp, #{}", temp).unwrap();
                }
            }
            Expr::Deref { expr } => {
                // Load value from pointer
                self.gen_expr(expr);
                writeln!(self.output, "    ldr x0, [x0]").unwrap();
            }
            Expr::StrConcat { left, right } => {
                // Simplified: evaluate both sides, left result in x0
                // Full impl would call a runtime string concat function
                self.gen_expr(left);
                let left_temp = self.next_offset;
                self.next_offset += 8;
                writeln!(self.output, "    str x0, [sp, #{}]", left_temp).unwrap();
                self.gen_expr(right);
                // x0 has right string ptr, x1 has left string ptr
                writeln!(self.output, "    ldr x1, [sp, #{}]", left_temp).unwrap();
                // Call runtime concat: p_str_concat
                writeln!(self.output, "    bl p_str_concat").unwrap();
                self.next_offset -= 8;
            }
            Expr::Try { expr } => {
                // ? operator: evaluate expr, check if it's an error/none
                // Simplified: just evaluate and return the value
                // Full impl would check a discriminant and early-return on error
                self.gen_expr(expr);
            }
            Expr::Coalesce { expr, default } => {
                // ?? operator: evaluate expr, if None evaluate default
                // Simplified: just evaluate expr (the interpreter/lowerer handles the logic)
                self.gen_expr(expr);
                self.gen_expr(default);
            }
            Expr::IsCheck { expr, .. } => {
                // is check — simplified, just evaluate expr
                self.gen_expr(expr);
            }
            Expr::TupleLit(elems) => {
                // Simplified: evaluate each element, last result in x0
                for e in elems {
                    self.gen_expr(e);
                }
            }
            Expr::Closure { params, body, ret_ty: _ } => {
                // Simplified: generate closure body inline
                // Full impl would create a trampoline function
                self.push_scope();
                for p in params {
                    let offset = self.declare_var(&p.name);
                    // Params would be passed in registers; simplified: zero-init
                    writeln!(self.output, "    str xzr, [sp, #{}]", offset).unwrap();
                }
                for s in body {
                    self.gen_stmt(s);
                }
                self.pop_scope();
            }
            _ => {}
        }
    }
}

fn count_lets(stmts: &[Stmt]) -> usize {
    let mut count = 0;
    for s in stmts {
        match s {
            Stmt::Let { .. } | Stmt::LetTuple { .. } => count += 1,
            Stmt::If { then_block, else_block, .. } => {
                count += count_lets(then_block);
                if let Some(eb) = else_block {
                    count += count_lets(eb);
                }
            }
            Stmt::While { body, .. } => count += count_lets(body),
            Stmt::For { body, .. } => count += count_lets(body),
            Stmt::Loop { body, .. } => count += count_lets(body),
            Stmt::Defer(body, _) => count += count_lets(body),
            Stmt::Match { arms, .. } => {
                for arm in arms {
                    count += count_lets(&arm.body);
                }
            }
            _ => {}
        }
    }
    count
}
