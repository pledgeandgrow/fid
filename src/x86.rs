// x86_64 backend — stack machine code generation from Pledge IR
//
// Strategy: each SSA value lives on the stack. We use RAX as the accumulator.
// This is simple, correct, and a starting point for register allocation later.
//
// Windows x64 calling convention:
// - Integer args: RCX, RDX, R8, R9 (then stack)
// - Return: RAX
// - Shadow space: 32 bytes on stack for callee
// - Stack must be 16-byte aligned before CALL

use crate::ir::*;
use crate::ast::{BinOp, UnaryOp};
use std::collections::HashMap;
use std::fmt::Write;

/// Macro for formatted writeln to self.out
macro_rules! wln {
    ($s:expr, $fmt:expr) => {{
        $s.out.push_str($fmt);
        $s.out.push('\n');
    }};
    ($s:expr, $fmt:expr, $($arg:expr),*) => {{
        $s.out.push_str(&format!($fmt, $($arg),*));
        $s.out.push('\n');
    }};
}

/// Offset on the stack for each SSA value
struct StackFrame {
    /// Map from ValueId to stack offset (from RBP)
    offsets: HashMap<u32, i64>,
    /// Current stack size
    size: i64,
    /// Pre-allocated offsets for Alloca data slots, keyed by alloca ValueId
    alloca_slots: HashMap<u32, i64>,
}

impl StackFrame {
    fn new() -> Self {
        StackFrame {
            offsets: HashMap::new(),
            size: 0,
            alloca_slots: HashMap::new(),
        }
    }

    /// Allocate space for a value and return its offset
    fn alloc(&mut self, id: ValueId, size: u64) -> i64 {
        // Align to size (min 8 for simplicity on 64-bit)
        let align = if size <= 1 { 1 } else if size <= 2 { 2 } else if size <= 4 { 4 } else { 8 };
        self.size = (self.size + align as i64 - 1) & !(align as i64 - 1);
        self.size += size as i64;
        let offset = -self.size;
        self.offsets.insert(id.0, offset);
        offset
    }

    /// Allocate raw space (not tied to a ValueId) and return its offset
    fn alloc_raw(&mut self, size: i64) -> i64 {
        let align = if size <= 1 { 1 } else if size <= 2 { 2 } else if size <= 4 { 4 } else { 8 };
        self.size = (self.size + align - 1) & !(align - 1);
        self.size += size;
        -self.size
    }

    /// Get the offset for a value
    fn offset(&self, id: ValueId) -> i64 {
        *self.offsets.get(&id.0).unwrap_or(&0)
    }
}

/// The x86_64 code generator
pub struct X86CodeGen {
    /// Output assembly text
    out: String,
    /// String constants for .data section
    data: String,
    /// String label counter
    string_counter: u32,
    /// Map string content to label
    string_labels: HashMap<String, String>,
}

impl X86CodeGen {
    pub fn new() -> Self {
        X86CodeGen {
            out: String::new(),
            data: String::new(),
            string_counter: 0,
            string_labels: HashMap::new(),
        }
    }

    /// Generate assembly for an entire IR module
    pub fn generate(&mut self, module: &IrModule) -> String {
        // .data section — string constants
        self.emit_data_section(module);

        // .code section — code
        wln!(self, ".code");
        wln!(self, "EXTERN GetStdHandle:PROC");
        wln!(self, "EXTERN WriteFile:PROC");
        wln!(self, "EXTERN ExitProcess:PROC");
        wln!(self, "EXTERN GetProcessHeap:PROC");
        wln!(self, "EXTERN HeapAlloc:PROC");
        wln!(self, "EXTERN HeapFree:PROC");
        wln!(self, "");
        wln!(self, "PUBLIC main");
        wln!(self, "PUBLIC _main");
        wln!(self, "");

        // Generate each function
        for (name, func) in &module.functions {
            self.gen_function(name, func, module);
        }

        // Generate runtime stubs (direct Windows API calls)
        self.gen_runtime_stubs();

        wln!(self, "END");

        // Combine data + text
        format!("{}\n{}", self.data, self.out)
    }

    /// Emit the .data section with string constants (MASM syntax)
    fn emit_data_section(&mut self, module: &IrModule) {
        self.data.clear();
        self.data.push_str(".data\n");

        // Collect all string constants from all functions
        for func in module.functions.values() {
            for (vid, inst) in &func.instructions {
                if let Inst::ConstStr { value } = inst {
                    let label = self.intern_string(value);
                    let _ = vid;
                    let escaped = escape_string(value);
                    let _ = writeln!(self.data, "{} BYTE \"{}\", 0", label, escaped);
                }
            }
        }

        // Boolean string constants
        let _ = writeln!(self.data, "str_true BYTE \"true\", 0");
        let _ = writeln!(self.data, "str_false BYTE \"false\", 0");

        // String concatenation buffer (static, 4096 bytes)
        let _ = writeln!(self.data, "str_concat_buf BYTE 4096 DUP(?)");
        let _ = writeln!(self.data, "str_concat_tmp BYTE 4096 DUP(?)");

        self.data.push_str("\n");
    }

    /// Intern a string and return its label
    fn intern_string(&mut self, s: &str) -> String {
        if let Some(label) = self.string_labels.get(s) {
            return label.clone();
        }
        let label = format!("str_{}", self.string_counter);
        self.string_counter += 1;
        self.string_labels.insert(s.to_string(), label.clone());
        label
    }

    /// Generate code for a single function
    fn gen_function(&mut self, name: &str, func: &IrFunction, module: &IrModule) {
        let mangled = mangle_name(name);
        // Function prologue
        wln!(self, "");
        wln!(self, "; ─── Function: {} ───", name);
        wln!(self, "{}:", mangled);
        wln!(self, "    push rbp");
        wln!(self, "    mov rbp, rsp");

        // Allocate stack frame — estimate size from all values
        let mut frame = StackFrame::new();
        for (vid, meta) in &func.values {
            let size = module.type_size(&meta.ty).max(8);
            frame.alloc(*vid, size);
        }

        // Pre-allocate extra space for Alloca instructions' data
        // Each Alloca needs a separate data slot beyond its pointer slot
        for (vid, inst) in &func.instructions {
            if let Inst::Alloca { ty, .. } = inst {
                let data_size = module.type_size(ty).max(8) as i64;
                let offset = frame.alloc_raw(data_size);
                frame.alloca_slots.insert(vid.0, offset);
            }
        }

        // Align stack to 16 bytes and allocate
        // Add 32 bytes for Windows x64 shadow space used during calls,
        // so sub rsp,32 before calls doesn't overwrite alloca data at frame bottom
        let stack_size = ((frame.size + 32 + 15) & !15) as i64;
        // Reserve at least 64 bytes (32 shadow + 32 minimum frame)
        let stack_size = stack_size.max(64);
        wln!(self, "    sub rsp, {}", stack_size);

        // Store parameters from registers to stack
        // Windows x64: RCX, RDX, R8, R9 for first 4 integer args
        // 5th+ params are on the stack at [rbp+48], [rbp+56], etc.
        let arg_regs = ["rcx", "rdx", "r8", "r9"];
        for (i, (vid, _ty)) in func.params.iter().enumerate() {
            if i < 4 {
                let offset = frame.offset(*vid);
                wln!(self, "    mov [rbp{}], {}", fmt_offset(offset), arg_regs[i]);
            } else {
                // 5th param at [rbp+48], 6th at [rbp+56], etc.
                let stack_offset = 48 + ((i - 4) as i64) * 8;
                let offset = frame.offset(*vid);
                wln!(self, "    mov rax, [rbp{}]", fmt_offset(stack_offset));
                wln!(self, "    mov [rbp{}], rax", fmt_offset(offset));
            }
        }

        // Generate code for each basic block
        for block in &func.blocks {
            wln!(self, "");
            wln!(self, "{}_bb_{}:", mangled, block.id.0);

            for &inst_id in &block.instructions {
                let inst = func.get_inst(inst_id);
                if let Some(inst) = inst {
                    self.gen_inst(inst_id, inst, &mut frame, func, module, &mangled);
                }
            }
        }

        // Function epilogue (in case we fall through)
        wln!(self, "    mov rsp, rbp");
        wln!(self, "    pop rbp");
        wln!(self, "    ret");
    }

    /// Generate code for a single instruction
    fn gen_inst(
        &mut self,
        id: ValueId,
        inst: &Inst,
        frame: &mut StackFrame,
        func: &IrFunction,
        module: &IrModule,
        func_name: &str,
    ) {
        let dst = frame.offset(id);

        match inst {
            // ─── Constants ───
            Inst::ConstInt { value, ty } => {
                let size = module.type_size(ty).max(8);
                let _ = size;
                wln!(self, "    mov rax, {}", value);
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }

            Inst::ConstBool { value } => {
                let val = if *value { 1 } else { 0 };
                wln!(self, "    mov rax, {}", val);
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }

            Inst::ConstStr { value } => {
                // Check if this is a function name — emit function address
                if module.functions.contains_key(value) || module.extern_functions.contains_key(value) {
                    wln!(self, "    lea rax, {}", value);
                    wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
                } else {
                    let label = self.string_labels.get(value)
                        .cloned()
                        .unwrap_or_else(|| format!("str_unknown"));
                    wln!(self, "    lea rax, {}", label);
                    wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
                }
            }

            Inst::ConstFloat { bits, ty } => {
                if *ty == IrType::F32 {
                    let val = *bits as u32;
                    wln!(self, "    mov dword ptr [rbp{}], {}", fmt_offset(dst), fmt_hex(val));
                } else {
                    let lo = *bits as u32;
                    let hi = (*bits >> 32) as u32;
                    wln!(self, "    mov dword ptr [rbp{}], {}", fmt_offset(dst), fmt_hex(lo));
                    wln!(self, "    mov dword ptr [rbp{}], {}", fmt_offset(dst + 4), fmt_hex(hi));
                }
            }

            Inst::ConstNull { .. } => {
                wln!(self, "    xor rax, rax");
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }

            // ─── Arithmetic ───
            Inst::BinOp { op, left, right, ty } => {
                let loff = frame.offset(*left);
                let roff = frame.offset(*right);

                if ty.is_float() {
                    self.gen_float_binop(op, loff, roff, dst, ty);
                } else {
                    self.gen_int_binop(op, loff, roff, dst, ty);
                }
            }

            Inst::UnaryOp { op, operand, ty } => {
                let off = frame.offset(*operand);

                match op {
                    UnaryOp::Neg => {
                        if ty.is_float() {
                            // Float negation: XOR sign bit
                            wln!(self, "    movq xmm0, qword ptr [rbp{}]", fmt_offset(off));
                            wln!(self, "    mov rax, 8000000000000000h");
                            wln!(self, "    mov [rsp], rax");
                            wln!(self, "    xorpd xmm0, xmmword ptr [rsp]");
                            if *ty == IrType::F32 {
                                wln!(self, "    movss [rbp{}], xmm0", fmt_offset(dst));
                            } else {
                                wln!(self, "    movq qword ptr [rbp{}], xmm0", fmt_offset(dst));
                            }
                        } else {
                            wln!(self, "    mov rax, [rbp{}]", fmt_offset(off));
                            wln!(self, "    neg rax");
                            wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
                        }
                    }
                    UnaryOp::Not => {
                        wln!(self, "    mov rax, [rbp{}]", fmt_offset(off));
                        wln!(self, "    xor rax, 1");
                        wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
                    }
                    UnaryOp::BitNot => {
                        wln!(self, "    mov rax, [rbp{}]", fmt_offset(off));
                        wln!(self, "    not rax");
                        wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
                    }
                }
            }

            // ─── Memory ───
            Inst::Alloca { .. } => {
                // Use pre-allocated data slot for this alloca
                let data_offset = *frame.alloca_slots.get(&id.0).unwrap_or(&0);
                wln!(self, "    lea rax, [rbp{}]", fmt_offset(data_offset));
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }

            Inst::Load { ptr, ty } => {
                let poff = frame.offset(*ptr);
                let size = module.type_size(ty).max(1);
                wln!(self, "    mov rax, [rbp{}]", fmt_offset(poff));
                match size {
                    1 => wln!(self, "    movsx rax, byte ptr [rax]"),
                    2 => wln!(self, "    movsx rax, word ptr [rax]"),
                    4 => wln!(self, "    movsxd rax, dword ptr [rax]"),
                    _ => wln!(self, "    mov rax, [rax]"),
                }
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }

            Inst::Store { ptr, value } => {
                let poff = frame.offset(*ptr);
                let voff = frame.offset(*value);
                wln!(self, "    mov rax, [rbp{}]", fmt_offset(poff));
                wln!(self, "    mov rcx, [rbp{}]", fmt_offset(voff));
                // Use correct store size based on value type
                let val_ty = func.values.get(value).map(|m| &m.ty);
                let store_size = val_ty.map(|t| module.type_size(t)).unwrap_or(8);
                match store_size {
                    1 => wln!(self, "    mov byte ptr [rax], cl"),
                    2 => wln!(self, "    mov word ptr [rax], cx"),
                    4 => wln!(self, "    mov dword ptr [rax], ecx"),
                    _ => wln!(self, "    mov [rax], rcx"),
                }
            }

            Inst::GetFieldPtr { struct_ptr, field_index, field_ty: _, .. } => {
                let poff = frame.offset(*struct_ptr);
                wln!(self, "    mov rax, [rbp{}]", fmt_offset(poff));
                // Look up field offset from struct layout
                // The struct type is in the struct_ptr's value metadata
                let offset = if let Some(ptr_meta) = func.values.get(struct_ptr) {
                    let struct_name = match &ptr_meta.ty {
                        IrType::Ptr(inner) => match inner.as_ref() {
                            IrType::Struct(name) => Some(name.as_str()),
                            _ => None,
                        },
                        IrType::Struct(name) => Some(name.as_str()),
                        _ => None,
                    };
                    if let Some(name) = struct_name {
                        if let Some(layout) = module.get_struct(name) {
                            if let Some((_, _, off)) = layout.fields.get(*field_index as usize) {
                                *off as i64
                            } else {
                                *field_index as i64 * 8
                            }
                        } else {
                            *field_index as i64 * 8
                        }
                    } else {
                        *field_index as i64 * 8
                    }
                } else {
                    *field_index as i64 * 8
                };
                if offset > 0 {
                    wln!(self, "    add rax, {}", offset);
                }
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }

            Inst::GetElemPtr { array_ptr, index, elem_ty } => {
                let poff = frame.offset(*array_ptr);
                let ioff = frame.offset(*index);
                let elem_size = module.type_size(elem_ty).max(8);
                wln!(self, "    mov rax, [rbp{}]", fmt_offset(poff));
                wln!(self, "    mov rcx, [rbp{}]", fmt_offset(ioff));
                wln!(self, "    imul rcx, {}", elem_size);
                wln!(self, "    add rax, rcx");
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }

            // ─── Control Flow ───
            Inst::Jump { target } => {
                wln!(self, "    jmp {}_bb_{}", func_name, target.0);
            }

            Inst::Branch { cond, then, else_ } => {
                let coff = frame.offset(*cond);
                wln!(self, "    mov rax, [rbp{}]", fmt_offset(coff));
                wln!(self, "    test rax, rax");
                wln!(self, "    jnz {}_bb_{}", func_name, then.0);
                wln!(self, "    jmp {}_bb_{}", func_name, else_.0);
            }

            Inst::Return { value } => {
                if let Some(v) = value {
                    let voff = frame.offset(*v);
                    wln!(self, "    mov rax, [rbp{}]", fmt_offset(voff));
                } else {
                    wln!(self, "    xor rax, rax");
                }
                wln!(self, "    mov rsp, rbp");
                wln!(self, "    pop rbp");
                wln!(self, "    ret");
            }

            Inst::Unreachable => {
                wln!(self, "    ud2  ; unreachable");
            }

            // ─── Function Calls ───
            Inst::Call { func: call_name, args, .. } => {
                // Special-case: route "print" to the right runtime stub
                let target_func = if call_name == "print" {
                    // Check arg type to pick the right print function
                    if let Some(arg) = args.first() {
                        let arg_ty = func.values.get(arg).map(|m| &m.ty);
                        match arg_ty {
                            Some(IrType::Ptr(_)) => "p_print_str",
                            Some(t) if t.is_float() => "p_print_float",
                            Some(IrType::Bool) => "p_print_bool",
                            _ => "p_print_int",
                        }
                    } else {
                        "p_print_int"
                    }
                } else {
                    // Mangle user function names to avoid x86 mnemonic conflicts
                    // (e.g., "add" is an x86 instruction)
                    Box::leak(mangle_name(call_name).into_boxed_str())
                };

                // Windows x64: RCX, RDX, R8, R9 for first 4 args
                // 5th+ args go on stack at [rsp+32], [rsp+40], etc. (above shadow space)
                let arg_regs = ["rcx", "rdx", "r8", "r9"];
                let stack_args = if args.len() > 4 { args.len() - 4 } else { 0 };
                // Reserve shadow space (32) + space for stack args, aligned to 16
                let call_space = ((32 + stack_args * 8 + 15) & !15) as i64;
                for (i, arg) in args.iter().enumerate() {
                    let aoff = frame.offset(*arg);
                    if i < 4 {
                        wln!(self, "    mov {}, [rbp{}]", arg_regs[i], fmt_offset(aoff));
                    }
                }
                wln!(self, "    sub rsp, {}  ; shadow space + stack args", call_space);
                // Place stack args after shadow space
                for (i, arg) in args.iter().enumerate().skip(4) {
                    let aoff = frame.offset(*arg);
                    let stack_pos = 32 + ((i - 4) as i64) * 8;
                    wln!(self, "    mov rax, [rbp{}]", fmt_offset(aoff));
                    wln!(self, "    mov [rsp+{}], rax", stack_pos);
                }
                wln!(self, "    call {}", target_func);
                wln!(self, "    add rsp, {}  ; cleanup", call_space);
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }

            Inst::CallPtr { fn_ptr, args, .. } => {
                let fpoff = frame.offset(*fn_ptr);

                // All function pointers are env structs: field 0 = fn addr, fields 1..N = captures
                // Load function address from field 0
                wln!(self, "    mov rax, [rbp{}]", fmt_offset(fpoff));
                wln!(self, "    mov rax, [rax]  ; load fn addr from env struct field 0");

                // Pass env_ptr as first arg (rcx), shift other args
                let total_args = args.len() + 1; // +1 for env_ptr
                let arg_regs = ["rcx", "rdx", "r8", "r9"];
                let stack_args = if total_args > 4 { total_args - 4 } else { 0 };
                let call_space = ((32 + stack_args * 8 + 15) & !15) as i64;

                wln!(self, "    sub rsp, {}  ; shadow space + stack args", call_space);

                // First arg (rcx) = env_ptr
                wln!(self, "    mov rcx, [rbp{}]", fmt_offset(fpoff));

                // Remaining args shifted by 1
                for (i, arg) in args.iter().enumerate() {
                    let aoff = frame.offset(*arg);
                    let arg_idx = i + 1; // shifted by 1 for env_ptr
                    if arg_idx < 4 {
                        wln!(self, "    mov {}, [rbp{}]", arg_regs[arg_idx], fmt_offset(aoff));
                    } else {
                        let stack_pos = 32 + ((arg_idx - 4) as i64) * 8;
                        wln!(self, "    mov r10, [rbp{}]", fmt_offset(aoff));
                        wln!(self, "    mov [rsp+{}], r10", stack_pos);
                    }
                }

                // Save fn addr, call indirectly
                wln!(self, "    mov r10, rax  ; save fn addr");
                wln!(self, "    call r10");
                wln!(self, "    add rsp, {}  ; cleanup", call_space);
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }

            // ─── Conversions ───
            Inst::IntExtend { value, from, to } => {
                let voff = frame.offset(*value);
                wln!(self, "    mov rax, [rbp{}]", fmt_offset(voff));
                // Sign-extend based on source type
                match from {
                    IrType::I8 => wln!(self, "    movsx rax, al"),
                    IrType::I16 => wln!(self, "    movsx rax, ax"),
                    IrType::I32 => wln!(self, "    movsxd rax, eax"),
                    _ => {}
                }
                let _ = to;
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }

            Inst::IntTrunc { value, from, to } => {
                let voff = frame.offset(*value);
                wln!(self, "    mov rax, [rbp{}]", fmt_offset(voff));
                // Truncate by masking
                match to {
                    IrType::I8 | IrType::U8 => wln!(self, "    movzx eax, al"),
                    IrType::I16 | IrType::U16 => wln!(self, "    movzx eax, ax"),
                    IrType::I32 | IrType::U32 => wln!(self, "    mov eax, eax"),
                    _ => {}
                }
                let _ = from;
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }

            Inst::IntToFloat { value, from, to } => {
                let voff = frame.offset(*value);
                wln!(self, "    mov rax, [rbp{}]", fmt_offset(voff));
                match from {
                    IrType::I64 | IrType::U64 => wln!(self, "    cvtsi2sd xmm0, rax"),
                    _ => wln!(self, "    cvtsi2sd xmm0, eax"),
                }
                if *to == IrType::F32 {
                    wln!(self, "    movss [rbp{}], xmm0", fmt_offset(dst));
                } else {
                    wln!(self, "    movq qword ptr [rbp{}], xmm0", fmt_offset(dst));
                }
            }

            Inst::FloatToInt { value, from, to } => {
                let voff = frame.offset(*value);
                if *from == IrType::F32 {
                    wln!(self, "    movss xmm0, [rbp{}]", fmt_offset(voff));
                } else {
                    wln!(self, "    movq xmm0, qword ptr [rbp{}]", fmt_offset(voff));
                }
                wln!(self, "    cvttsd2si rax, xmm0");
                let _ = to;
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }

            Inst::MakeDyn { .. } => {
                wln!(self, "    xor rax, rax  ; MakeDyn not yet supported");
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }
            Inst::DynCall { .. } => {
                wln!(self, "    xor rax, rax  ; DynCall not yet supported");
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }
            Inst::BitCast { value, .. } => {
                let voff = frame.offset(*value);
                wln!(self, "    mov rax, [rbp{}]", fmt_offset(voff));
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }

            // ─── Pledge-Specific ───
            Inst::Print { value, ty } => {
                let voff = frame.offset(*value);
                let _ = ty;
                // Call our print stub
                wln!(self, "    mov rcx, [rbp{}]", fmt_offset(voff));
                wln!(self, "    sub rsp, 32");
                wln!(self, "    call p_print_int");
                wln!(self, "    add rsp, 32");
            }

            Inst::PersistLoad { default, .. } => {
                // Just copy default value to destination
                let doff = frame.offset(*default);
                wln!(self, "    mov rax, [rbp{}]", fmt_offset(doff));
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }
            Inst::PersistSave { .. } => {
                // No-op
            }
            Inst::Panic { message } => {
                let label = self.intern_string(message);
                // Emit the string in data section if not already there
                if !self.data.contains(&format!("{} BYTE", label)) {
                    let _ = writeln!(self.data, "{} BYTE \"{}\", 0", label, escape_string(message));
                }
                wln!(self, "    lea rcx, {}", label);
                wln!(self, "    sub rsp, 32");
                wln!(self, "    call p_panic");
                wln!(self, "    add rsp, 32");
            }

            Inst::TryUnwrap { .. } => {
                // Simplified — not fully implemented in backend yet
                wln!(self, "    ; TODO: TryUnwrap");
            }
            Inst::Asm { .. } => {
                wln!(self, "    ; asm not yet supported");
                wln!(self, "    xor rax, rax");
                wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
            }
            Inst::RegionBegin { .. } => {
                wln!(self, "    ; region begin (no-op — stack allocation)");
                wln!(self, "    xor rax, rax");
            }
            Inst::RegionEnd { .. } => {
                wln!(self, "    ; region end (no-op)");
            }
        }
    }

    /// Generate integer binary operation
    fn gen_int_binop(&mut self, op: &BinOp, loff: i64, roff: i64, dst: i64, _ty: &IrType) {
        // Load left into RAX, right into RCX
        wln!(self, "    mov rax, [rbp{}]", fmt_offset(loff));
        wln!(self, "    mov rcx, [rbp{}]", fmt_offset(roff));

        match op {
            BinOp::Add => {
                wln!(self, "    add rax, rcx");
            }
            BinOp::Sub => {
                wln!(self, "    sub rax, rcx");
            }
            BinOp::Mul => {
                wln!(self, "    imul rax, rcx");
            }
            BinOp::Div => {
                // Sign-extend RAX into RDX:RAX, then idiv
                wln!(self, "    cqo");
                wln!(self, "    idiv rcx");
            }
            BinOp::Mod => {
                wln!(self, "    cqo");
                wln!(self, "    idiv rcx");
                wln!(self, "    mov rax, rdx"); // remainder
            }
            BinOp::BitAnd => {
                wln!(self, "    and rax, rcx");
            }
            BinOp::BitOr => {
                wln!(self, "    or rax, rcx");
            }
            BinOp::BitXor => {
                wln!(self, "    xor rax, rcx");
            }
            BinOp::Shl => {
                // Shift count must be in CL
                wln!(self, "    shl rax, cl");
            }
            BinOp::Shr => {
                wln!(self, "    sar rax, cl");
            }
            BinOp::Eq => {
                wln!(self, "    cmp rax, rcx");
                wln!(self, "    sete al");
                wln!(self, "    movzx eax, al");
            }
            BinOp::Neq => {
                wln!(self, "    cmp rax, rcx");
                wln!(self, "    setne al");
                wln!(self, "    movzx eax, al");
            }
            BinOp::Lt => {
                wln!(self, "    cmp rax, rcx");
                wln!(self, "    setl al");
                wln!(self, "    movzx eax, al");
            }
            BinOp::Gt => {
                wln!(self, "    cmp rax, rcx");
                wln!(self, "    setg al");
                wln!(self, "    movzx eax, al");
            }
            BinOp::LtEq => {
                wln!(self, "    cmp rax, rcx");
                wln!(self, "    setle al");
                wln!(self, "    movzx eax, al");
            }
            BinOp::GtEq => {
                wln!(self, "    cmp rax, rcx");
                wln!(self, "    setge al");
                wln!(self, "    movzx eax, al");
            }
            BinOp::And => {
                wln!(self, "    test rax, rax");
                wln!(self, "    setne al");
                wln!(self, "    test rcx, rcx");
                wln!(self, "    setne cl");
                wln!(self, "    and al, cl");
                wln!(self, "    movzx eax, al");
            }
            BinOp::Or => {
                wln!(self, "    or rax, rcx");
                wln!(self, "    setne al");
                wln!(self, "    movzx eax, al");
            }
        }

        wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
    }

    /// Generate float binary operation
    fn gen_float_binop(&mut self, op: &BinOp, loff: i64, roff: i64, dst: i64, ty: &IrType) {
        if *ty == IrType::F32 {
            wln!(self, "    movss xmm0, [rbp{}]", fmt_offset(loff));
            wln!(self, "    movss xmm1, [rbp{}]", fmt_offset(roff));
            match op {
                BinOp::Add => wln!(self, "    addss xmm0, xmm1"),
                BinOp::Sub => wln!(self, "    subss xmm0, xmm1"),
                BinOp::Mul => wln!(self, "    mulss xmm0, xmm1"),
                BinOp::Div => wln!(self, "    divss xmm0, xmm1"),
                BinOp::Eq => {
                    wln!(self, "    ucomiss xmm0, xmm1");
                    wln!(self, "    sete al");
                    wln!(self, "    movzx eax, al");
                }
                BinOp::Neq => {
                    wln!(self, "    ucomiss xmm0, xmm1");
                    wln!(self, "    setne al");
                    wln!(self, "    movzx eax, al");
                }
                BinOp::Lt => {
                    wln!(self, "    ucomiss xmm0, xmm1");
                    wln!(self, "    setb al");
                    wln!(self, "    movzx eax, al");
                }
                BinOp::Gt => {
                    wln!(self, "    ucomiss xmm1, xmm0");
                    wln!(self, "    setb al");
                    wln!(self, "    movzx eax, al");
                }
                BinOp::LtEq => {
                    wln!(self, "    ucomiss xmm0, xmm1");
                    wln!(self, "    setbe al");
                    wln!(self, "    movzx eax, al");
                }
                BinOp::GtEq => {
                    wln!(self, "    ucomiss xmm1, xmm0");
                    wln!(self, "    setbe al");
                    wln!(self, "    movzx eax, al");
                }
                _ => {}
            }
            // For arithmetic ops, store xmm0; for comparisons, store rax
            match op {
                BinOp::Add | BinOp::Sub | BinOp::Mul | BinOp::Div => {
                    wln!(self, "    movss [rbp{}], xmm0", fmt_offset(dst));
                }
                _ => {
                    wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
                }
            }
        } else {
            // F64
            wln!(self, "    movq xmm0, qword ptr [rbp{}]", fmt_offset(loff));
            wln!(self, "    movq xmm1, qword ptr [rbp{}]", fmt_offset(roff));
            match op {
                BinOp::Add => wln!(self, "    addsd xmm0, xmm1"),
                BinOp::Sub => wln!(self, "    subsd xmm0, xmm1"),
                BinOp::Mul => wln!(self, "    mulsd xmm0, xmm1"),
                BinOp::Div => wln!(self, "    divsd xmm0, xmm1"),
                BinOp::Eq => {
                    wln!(self, "    ucomisd xmm0, xmm1");
                    wln!(self, "    sete al");
                    wln!(self, "    movzx eax, al");
                }
                BinOp::Neq => {
                    wln!(self, "    ucomisd xmm0, xmm1");
                    wln!(self, "    setne al");
                    wln!(self, "    movzx eax, al");
                }
                BinOp::Lt => {
                    wln!(self, "    ucomisd xmm0, xmm1");
                    wln!(self, "    setb al");
                    wln!(self, "    movzx eax, al");
                }
                BinOp::Gt => {
                    wln!(self, "    ucomisd xmm1, xmm0");
                    wln!(self, "    setb al");
                    wln!(self, "    movzx eax, al");
                }
                BinOp::LtEq => {
                    wln!(self, "    ucomisd xmm0, xmm1");
                    wln!(self, "    setbe al");
                    wln!(self, "    movzx eax, al");
                }
                BinOp::GtEq => {
                    wln!(self, "    ucomisd xmm1, xmm0");
                    wln!(self, "    setbe al");
                    wln!(self, "    movzx eax, al");
                }
                _ => {}
            }
            match op {
                BinOp::Add | BinOp::Sub | BinOp::Mul | BinOp::Div => {
                    wln!(self, "    movq qword ptr [rbp{}], xmm0", fmt_offset(dst));
                }
                _ => {
                    wln!(self, "    mov [rbp{}], rax", fmt_offset(dst));
                }
            }
        }
    }

    /// Generate runtime stubs — direct Windows API calls (no C runtime)
    fn gen_runtime_stubs(&mut self) {
        wln!(self, "");
        wln!(self, "; ─── Runtime stubs (direct Windows API) ───");

        // p_print_int(rcx = i64 value)
        // Converts integer to string and writes to stdout + newline
        wln!(self, "p_print_int:");
        wln!(self, "    push rbp");
        wln!(self, "    mov rbp, rsp");
        wln!(self, "    push r12");
        wln!(self, "    push r13");
        wln!(self, "    sub rsp, 80  ; buffer + shadow space + alignment");
        wln!(self, "");
        wln!(self, "    mov rax, rcx  ; value");
        wln!(self, "    lea r8, [rbp-1]  ; buffer end pointer");
        wln!(self, "    mov byte ptr [r8], 10  ; newline at end");
        wln!(self, "    mov r9, 10  ; divisor");
        wln!(self, "");
        wln!(self, "    test rax, rax");
        wln!(self, "    jns pi_pos");
        wln!(self, "    neg rax");
        wln!(self, "pi_pos:");
        wln!(self, "pi_loop:");
        wln!(self, "    dec r8");
        wln!(self, "    xor rdx, rdx");
        wln!(self, "    div r9");
        wln!(self, "    add dl, '0'");
        wln!(self, "    mov [r8], dl");
        wln!(self, "    test rax, rax");
        wln!(self, "    jnz pi_loop");
        wln!(self, "");
        wln!(self, "    test rcx, rcx");
        wln!(self, "    jns pi_no_neg");
        wln!(self, "    dec r8");
        wln!(self, "    mov byte ptr [r8], '-'");
        wln!(self, "pi_no_neg:");
        wln!(self, "");
        wln!(self, "    ; WriteFile(stdout, r8, len, &written, NULL)");
        wln!(self, "    mov ecx, -11");
        wln!(self, "    sub rsp, 32");
        wln!(self, "    call GetStdHandle");
        wln!(self, "    add rsp, 32");
        wln!(self, "    mov r12, rax");
        wln!(self, "    lea r9, [rbp]  ; end (after newline)");
        wln!(self, "    sub r9, r8  ; length");
        wln!(self, "    mov rcx, r12");
        wln!(self, "    mov rdx, r8");
        wln!(self, "    mov r8, r9");
        wln!(self, "    lea r9, [rbp-32]");
        wln!(self, "    mov dword ptr [r9], 0");
        wln!(self, "    sub rsp, 48  ; shadow space + 5th arg + alignment");
        wln!(self, "    mov qword ptr [rsp+32], 0  ; lpOverlapped = NULL");
        wln!(self, "    call WriteFile");
        wln!(self, "    add rsp, 48");
        wln!(self, "    xor rax, rax");
        wln!(self, "    add rsp, 80");
        wln!(self, "    pop r13");
        wln!(self, "    pop r12");
        wln!(self, "    mov rsp, rbp");
        wln!(self, "    pop rbp");
        wln!(self, "    ret");

        // p_print_str(rcx = *u8 pointer to null-terminated string)
        wln!(self, "");
        wln!(self, "p_print_str:");
        wln!(self, "    push rbp");
        wln!(self, "    mov rbp, rsp");
        wln!(self, "    push r12");
        wln!(self, "    push r13");
        wln!(self, "    sub rsp, 64");
        wln!(self, "    mov r12, rcx  ; save string pointer");
        wln!(self, "");
        wln!(self, "    ; Calculate string length");
        wln!(self, "    xor r8, r8");
        wln!(self, "ps_len:");
        wln!(self, "    cmp byte ptr [r12 + r8], 0");
        wln!(self, "    je ps_done");
        wln!(self, "    inc r8");
        wln!(self, "    jmp ps_len");
        wln!(self, "ps_done:");
        wln!(self, "    mov r13, r8  ; save length");
        wln!(self, "");
        wln!(self, "    ; WriteFile string");
        wln!(self, "    mov ecx, -11");
        wln!(self, "    sub rsp, 32");
        wln!(self, "    call GetStdHandle");
        wln!(self, "    add rsp, 32");
        wln!(self, "    mov rcx, rax  ; handle");
        wln!(self, "    mov rdx, r12  ; buffer");
        wln!(self, "    mov r8, r13  ; length");
        wln!(self, "    lea r9, [rbp-32]");
        wln!(self, "    mov dword ptr [r9], 0");
        wln!(self, "    sub rsp, 48  ; shadow space + 5th arg + alignment");
        wln!(self, "    mov qword ptr [rsp+32], 0  ; lpOverlapped = NULL");
        wln!(self, "    call WriteFile");
        wln!(self, "    add rsp, 48");
        wln!(self, "");
        wln!(self, "    ; Print newline");
        wln!(self, "    mov byte ptr [rbp-33], 10");
        wln!(self, "    mov ecx, -11");
        wln!(self, "    sub rsp, 32");
        wln!(self, "    call GetStdHandle");
        wln!(self, "    add rsp, 32");
        wln!(self, "    mov rcx, rax");
        wln!(self, "    lea rdx, [rbp-33]");
        wln!(self, "    mov r8, 1");
        wln!(self, "    lea r9, [rbp-40]");
        wln!(self, "    mov dword ptr [r9], 0");
        wln!(self, "    sub rsp, 48  ; shadow space + 5th arg + alignment");
        wln!(self, "    mov qword ptr [rsp+32], 0  ; lpOverlapped = NULL");
        wln!(self, "    call WriteFile");
        wln!(self, "    add rsp, 48");
        wln!(self, "    xor rax, rax");
        wln!(self, "    add rsp, 64");
        wln!(self, "    pop r13");
        wln!(self, "    pop r12");
        wln!(self, "    mov rsp, rbp");
        wln!(self, "    pop rbp");
        wln!(self, "    ret");

        // p_print_bool(rcx = 0 or 1)
        wln!(self, "");
        wln!(self, "p_print_bool:");
        wln!(self, "    push rbp");
        wln!(self, "    mov rbp, rsp");
        wln!(self, "    push r12");
        wln!(self, "    push r13");
        wln!(self, "    sub rsp, 64");
        wln!(self, "    test rcx, rcx");
        wln!(self, "    jnz pb_true");
        wln!(self, "    lea r12, str_false");
        wln!(self, "    jmp pb_print");
        wln!(self, "pb_true:");
        wln!(self, "    lea r12, str_true");
        wln!(self, "pb_print:");
        wln!(self, "    xor r8, r8");
        wln!(self, "pb_len:");
        wln!(self, "    cmp byte ptr [r12 + r8], 0");
        wln!(self, "    je pb_done");
        wln!(self, "    inc r8");
        wln!(self, "    jmp pb_len");
        wln!(self, "pb_done:");
        wln!(self, "    mov r13, r8");
        wln!(self, "    mov ecx, -11");
        wln!(self, "    sub rsp, 32");
        wln!(self, "    call GetStdHandle");
        wln!(self, "    add rsp, 32");
        wln!(self, "    mov rcx, rax");
        wln!(self, "    mov rdx, r12");
        wln!(self, "    mov r8, r13");
        wln!(self, "    lea r9, [rbp-32]");
        wln!(self, "    mov dword ptr [r9], 0");
        wln!(self, "    sub rsp, 48  ; shadow space + 5th arg + alignment");
        wln!(self, "    mov qword ptr [rsp+32], 0  ; lpOverlapped = NULL");
        wln!(self, "    call WriteFile");
        wln!(self, "    add rsp, 48");
        wln!(self, "    ; newline");
        wln!(self, "    mov byte ptr [rbp-33], 10");
        wln!(self, "    mov ecx, -11");
        wln!(self, "    sub rsp, 32");
        wln!(self, "    call GetStdHandle");
        wln!(self, "    add rsp, 32");
        wln!(self, "    mov rcx, rax");
        wln!(self, "    lea rdx, [rbp-33]");
        wln!(self, "    mov r8, 1");
        wln!(self, "    lea r9, [rbp-40]");
        wln!(self, "    mov dword ptr [r9], 0");
        wln!(self, "    sub rsp, 48  ; shadow space + 5th arg + alignment");
        wln!(self, "    mov qword ptr [rsp+32], 0  ; lpOverlapped = NULL");
        wln!(self, "    call WriteFile");
        wln!(self, "    add rsp, 48");
        wln!(self, "    xor rax, rax");
        wln!(self, "    add rsp, 64");
        wln!(self, "    pop r13");
        wln!(self, "    pop r12");
        wln!(self, "    mov rsp, rbp");
        wln!(self, "    pop rbp");
        wln!(self, "    ret");

        // p_print_float(rcx = f64 bits stored as i64)
        // Formats as integer.fractional with 6 decimal places
        wln!(self, "");
        wln!(self, "p_print_float:");
        wln!(self, "    push rbp");
        wln!(self, "    mov rbp, rsp");
        wln!(self, "    push r12");
        wln!(self, "    push r13");
        wln!(self, "    push r14");
        wln!(self, "    sub rsp, 152  ; buffer(64) + temp(32) + locals(56), 16-byte aligned");
        wln!(self, "");
        wln!(self, "    ; Save float bits and load as xmm0");
        wln!(self, "    mov [rbp-72], rcx");
        wln!(self, "    movq xmm0, qword ptr [rbp-72]");
        wln!(self, "");
        wln!(self, "    ; r12 = buffer pointer, r13 = length");
        wln!(self, "    lea r12, [rbp-64]");
        wln!(self, "    xor r13, r13");
        wln!(self, "");
        wln!(self, "    ; Check negative");
        wln!(self, "    xorpd xmm1, xmm1");
        wln!(self, "    ucomisd xmm0, xmm1");
        wln!(self, "    jae pf_pos");
        wln!(self, "    mov byte ptr [r12], '-'");
        wln!(self, "    inc r12");
        wln!(self, "    inc r13");
        wln!(self, "    mov rax, 8000000000000000h");
        wln!(self, "    mov [rbp-80], rax");
        wln!(self, "    xorpd xmm0, xmmword ptr [rbp-80]");
        wln!(self, "");
        wln!(self, "pf_pos:");
        wln!(self, "    ; Save xmm0 (possibly negated)");
        wln!(self, "    movq qword ptr [rbp-72], xmm0");
        wln!(self, "");
        wln!(self, "    ; Get integer part");
        wln!(self, "    cvttsd2si rax, xmm0");
        wln!(self, "    mov r14, rax  ; save integer part");
        wln!(self, "");
        wln!(self, "    ; Handle integer part == 0");
        wln!(self, "    test rax, rax");
        wln!(self, "    jnz pf_int_start");
        wln!(self, "    mov byte ptr [r12], '0'");
        wln!(self, "    inc r12");
        wln!(self, "    inc r13");
        wln!(self, "    jmp pf_dot");
        wln!(self, "");
        wln!(self, "pf_int_start:");
        wln!(self, "    ; Convert integer to reversed string at [rbp-128]");
        wln!(self, "    lea r8, [rbp-128]");
        wln!(self, "    xor r9, r9");
        wln!(self, "pf_int_loop:");
        wln!(self, "    xor rdx, rdx");
        wln!(self, "    mov rcx, 10");
        wln!(self, "    div rcx");
        wln!(self, "    add dl, '0'");
        wln!(self, "    mov [r8], dl");
        wln!(self, "    inc r8");
        wln!(self, "    inc r9");
        wln!(self, "    test rax, rax");
        wln!(self, "    jnz pf_int_loop");
        wln!(self, "");
        wln!(self, "    ; Copy reversed digits to buffer in correct order");
        wln!(self, "    dec r8");
        wln!(self, "pf_int_copy:");
        wln!(self, "    mov al, [r8]");
        wln!(self, "    mov [r12], al");
        wln!(self, "    inc r12");
        wln!(self, "    inc r13");
        wln!(self, "    dec r8");
        wln!(self, "    dec r9");
        wln!(self, "    jnz pf_int_copy");
        wln!(self, "");
        wln!(self, "pf_dot:");
        wln!(self, "    mov byte ptr [r12], '.'");
        wln!(self, "    inc r12");
        wln!(self, "    inc r13");
        wln!(self, "");
        wln!(self, "    ; Get fractional part: xmm0 = original - integer_part");
        wln!(self, "    movq xmm0, qword ptr [rbp-72]");
        wln!(self, "    cvtsi2sd xmm1, r14");
        wln!(self, "    subsd xmm0, xmm1");
        wln!(self, "    ; Clamp negative to 0 (FP precision)");
        wln!(self, "    xorpd xmm1, xmm1");
        wln!(self, "    ucomisd xmm0, xmm1");
        wln!(self, "    jae pf_frac_ok");
        wln!(self, "    movq xmm0, xmm1");
        wln!(self, "pf_frac_ok:");
        wln!(self, "");
        wln!(self, "    ; Load 1000000.0 constant");
        wln!(self, "    mov rax, 412E848000000000h");
        wln!(self, "    mov [rbp-80], rax");
        wln!(self, "    movq xmm2, qword ptr [rbp-80]  ; xmm2 = 1000000.0");
        wln!(self, "");
        wln!(self, "    ; Multiply fractional part by 1000000");
        wln!(self, "    mulsd xmm0, xmm2");
        wln!(self, "");
        wln!(self, "    ; Add 0.5 for rounding");
        wln!(self, "    mov rax, 3FE0000000000000h");
        wln!(self, "    mov [rbp-80], rax");
        wln!(self, "    movq xmm1, qword ptr [rbp-80]  ; xmm1 = 0.5");
        wln!(self, "    addsd xmm0, xmm1");
        wln!(self, "");
        wln!(self, "    ; Truncate to integer");
        wln!(self, "    cvttsd2si rax, xmm0  ; rax = rounded fractional * 10^6");
        wln!(self, "");
        wln!(self, "    ; Extract 6 digits from rax (reversed into temp buffer)");
        wln!(self, "    lea r8, [rbp-128]");
        wln!(self, "    mov rcx, 6");
        wln!(self, "    mov r9, 10");
        wln!(self, "pf_frac_loop:");
        wln!(self, "    xor rdx, rdx");
        wln!(self, "    div r9");
        wln!(self, "    add dl, '0'");
        wln!(self, "    mov [r8], dl");
        wln!(self, "    inc r8");
        wln!(self, "    dec rcx");
        wln!(self, "    jnz pf_frac_loop");
        wln!(self, "");
        wln!(self, "    ; Copy digits in reverse order to output buffer");
        wln!(self, "    dec r8");
        wln!(self, "    mov rcx, 6");
        wln!(self, "pf_frac_copy:");
        wln!(self, "    mov al, [r8]");
        wln!(self, "    mov [r12], al");
        wln!(self, "    inc r12");
        wln!(self, "    inc r13");
        wln!(self, "    dec r8");
        wln!(self, "    dec rcx");
        wln!(self, "    jnz pf_frac_copy");
        wln!(self, "");
        wln!(self, "    ; Add newline");
        wln!(self, "    mov byte ptr [r12], 10");
        wln!(self, "    inc r13");
        wln!(self, "");
        wln!(self, "    ; WriteFile(stdout, buffer, length, &written, NULL)");
        wln!(self, "    mov ecx, -11");
        wln!(self, "    sub rsp, 32");
        wln!(self, "    call GetStdHandle");
        wln!(self, "    add rsp, 32");
        wln!(self, "    mov rcx, rax  ; handle");
        wln!(self, "    lea rdx, [rbp-64]  ; buffer");
        wln!(self, "    mov r8, r13  ; length");
        wln!(self, "    lea r9, [rbp-32]");
        wln!(self, "    mov dword ptr [r9], 0");
        wln!(self, "    sub rsp, 48  ; shadow space + 5th arg + alignment");
        wln!(self, "    mov qword ptr [rsp+32], 0  ; lpOverlapped = NULL");
        wln!(self, "    call WriteFile");
        wln!(self, "    add rsp, 48");
        wln!(self, "");
        wln!(self, "    xor rax, rax");
        wln!(self, "    add rsp, 152");
        wln!(self, "    pop r14");
        wln!(self, "    pop r13");
        wln!(self, "    pop r12");
        wln!(self, "    mov rsp, rbp");
        wln!(self, "    pop rbp");
        wln!(self, "    ret");

        // p_to_str(rcx = i64 value) -> rax = pointer to null-terminated string
        // Converts an integer to a string in a static buffer
        wln!(self, "");
        wln!(self, "p_to_str:");
        wln!(self, "    push rbp");
        wln!(self, "    mov rbp, rsp");
        wln!(self, "    sub rsp, 64  ; buffer + shadow space");
        wln!(self, "    lea r8, [rbp-1]  ; buffer end pointer");
        wln!(self, "    mov byte ptr [r8], 0  ; null terminator");
        wln!(self, "    mov rax, rcx  ; value");
        wln!(self, "    mov r9, 10  ; divisor");
        wln!(self, "    test rax, rax");
        wln!(self, "    jns ts_pos");
        wln!(self, "    neg rax");
        wln!(self, "ts_pos:");
        wln!(self, "ts_loop:");
        wln!(self, "    dec r8");
        wln!(self, "    xor rdx, rdx");
        wln!(self, "    div r9");
        wln!(self, "    add dl, '0'");
        wln!(self, "    mov [r8], dl");
        wln!(self, "    test rax, rax");
        wln!(self, "    jnz ts_loop");
        wln!(self, "    test rcx, rcx");
        wln!(self, "    jns ts_no_neg");
        wln!(self, "    dec r8");
        wln!(self, "    mov byte ptr [r8], '-'");
        wln!(self, "ts_no_neg:");
        wln!(self, "    ; Handle zero case");
        wln!(self, "    cmp byte ptr [r8], 0");
        wln!(self, "    jne ts_done");
        wln!(self, "    dec r8");
        wln!(self, "    mov byte ptr [r8], '0'");
        wln!(self, "ts_done:");
        wln!(self, "    lea rax, [r8]  ; return pointer to start of string");
        wln!(self, "    mov rsp, rbp");
        wln!(self, "    pop rbp");
        wln!(self, "    ret");

        // p_str_concat(rcx = left ptr, rdx = right ptr) -> rax = new string ptr
        // Concatenates two null-terminated strings into a static buffer
        // Uses a large static buffer (simplified — not thread-safe, not heap-allocated)
        wln!(self, "");
        wln!(self, "p_str_concat:");
        wln!(self, "    push rbp");
        wln!(self, "    mov rbp, rsp");
        wln!(self, "    sub rsp, 32  ; shadow space");
        wln!(self, "    ; Copy left string to temp buffer first (avoid aliasing)");
        wln!(self, "    mov r8, OFFSET str_concat_tmp  ; temp buffer");
        wln!(self, "    mov r9, 0  ; index");
        wln!(self, "sc_copy:");
        wln!(self, "    mov al, byte ptr [rcx + r9]");
        wln!(self, "    mov byte ptr [r8 + r9], al");
        wln!(self, "    test al, al");
        wln!(self, "    jz sc_copy_done");
        wln!(self, "    inc r9");
        wln!(self, "    jmp sc_copy");
        wln!(self, "sc_copy_done:");
        wln!(self, "    ; Now copy temp (left) to output buffer");
        wln!(self, "    mov r8, OFFSET str_concat_buf  ; destination");
        wln!(self, "    mov r11, OFFSET str_concat_tmp  ; source (left)");
        wln!(self, "    mov rcx, 0  ; index for left");
        wln!(self, "sc_left:");
        wln!(self, "    mov al, byte ptr [r11 + rcx]");
        wln!(self, "    test al, al");
        wln!(self, "    jz sc_right");
        wln!(self, "    mov byte ptr [r8 + rcx], al");
        wln!(self, "    inc rcx");
        wln!(self, "    jmp sc_left");
        wln!(self, "sc_right:");
        wln!(self, "    ; r9 = length of left (rcx), copy right string");
        wln!(self, "    mov r9, rcx  ; left length");
        wln!(self, "    mov r10, 0  ; index for right");
        wln!(self, "sc_right_loop:");
        wln!(self, "    mov al, byte ptr [rdx + r10]");
        wln!(self, "    test al, al");
        wln!(self, "    jz sc_end");
        wln!(self, "    mov byte ptr [r8 + r9], al");
        wln!(self, "    inc r9");
        wln!(self, "    inc r10");
        wln!(self, "    jmp sc_right_loop");
        wln!(self, "sc_end:");
        wln!(self, "    mov byte ptr [r8 + r9], 0  ; null terminate");
        wln!(self, "    mov rax, OFFSET str_concat_buf  ; return pointer");
        wln!(self, "    mov rsp, rbp");
        wln!(self, "    pop rbp");
        wln!(self, "    ret");

        // p_panic(rcx = message pointer)
        wln!(self, "");
        wln!(self, "p_panic:");
        wln!(self, "    push rbp");
        wln!(self, "    mov rbp, rsp");
        wln!(self, "    sub rsp, 64");
        wln!(self, "");
        wln!(self, "    ; Get stdout handle");
        wln!(self, "    mov ecx, -11  ; STD_OUTPUT_HANDLE");
        wln!(self, "    sub rsp, 32");
        wln!(self, "    call GetStdHandle");
        wln!(self, "    add rsp, 32");
        wln!(self, "    mov r12, rax");
        wln!(self, "");
        wln!(self, "    ; Calculate string length");
        wln!(self, "    mov rdx, rcx  ; message");
        wln!(self, "    xor r8, r8  ; length counter");
        wln!(self, "panic_len:");
        wln!(self, "    cmp byte ptr [rdx + r8], 0");
        wln!(self, "    je panic_done");
        wln!(self, "    inc r8");
        wln!(self, "    jmp panic_len");
        wln!(self, "panic_done:");
        wln!(self, "");
        wln!(self, "    ; WriteFile");
        wln!(self, "    mov rcx, r12  ; handle");
        wln!(self, "    ; rdx already = buffer");
        wln!(self, "    ; r8 already = length");
        wln!(self, "    lea r9, [rbp-32]");
        wln!(self, "    mov dword ptr [r9], 0");
        wln!(self, "    sub rsp, 48  ; shadow space + 5th arg + alignment");
        wln!(self, "    mov qword ptr [rsp+32], 0  ; lpOverlapped = NULL");
        wln!(self, "    call WriteFile");
        wln!(self, "    add rsp, 48");
        wln!(self, "");
        wln!(self, "    ; ExitProcess(1)");
        wln!(self, "    mov ecx, 1");
        wln!(self, "    sub rsp, 32");
        wln!(self, "    call ExitProcess");
        wln!(self, "    add rsp, 32");
        wln!(self, "    ud2  ; should not reach here");

        // _main — Windows entry point that calls our main
        wln!(self, "");
        wln!(self, "; ─── Heap allocation stubs ───");

        // p_malloc(rcx = size) -> rax = ptr
        wln!(self, "p_malloc:");
        wln!(self, "    push rbp");
        wln!(self, "    mov rbp, rsp");
        wln!(self, "    sub rsp, 48  ; shadow space + alignment");
        wln!(self, "");
        wln!(self, "    ; Save size in r12");
        wln!(self, "    mov r12, rcx");
        wln!(self, "");
        wln!(self, "    ; GetProcessHeap()");
        wln!(self, "    sub rsp, 32");
        wln!(self, "    call GetProcessHeap");
        wln!(self, "    add rsp, 32");
        wln!(self, "    mov r13, rax  ; heap handle");
        wln!(self, "");
        wln!(self, "    ; HeapAlloc(heap, 0, size)");
        wln!(self, "    mov rcx, r13  ; heap");
        wln!(self, "    xor rdx, rdx  ; flags = 0");
        wln!(self, "    mov r8, r12   ; size");
        wln!(self, "    sub rsp, 32");
        wln!(self, "    call HeapAlloc");
        wln!(self, "    add rsp, 32");
        wln!(self, "    ; rax = pointer");
        wln!(self, "    mov rsp, rbp");
        wln!(self, "    pop rbp");
        wln!(self, "    ret");

        // p_free(rcx = ptr) — free heap allocation
        wln!(self, "");
        wln!(self, "p_free:");
        wln!(self, "    push rbp");
        wln!(self, "    mov rbp, rsp");
        wln!(self, "    sub rsp, 48  ; shadow space + alignment");
        wln!(self, "");
        wln!(self, "    mov r12, rcx  ; save ptr");
        wln!(self, "");
        wln!(self, "    ; GetProcessHeap()");
        wln!(self, "    sub rsp, 32");
        wln!(self, "    call GetProcessHeap");
        wln!(self, "    add rsp, 32");
        wln!(self, "    mov r13, rax  ; heap handle");
        wln!(self, "");
        wln!(self, "    ; HeapFree(heap, 0, ptr)");
        wln!(self, "    mov rcx, r13  ; heap");
        wln!(self, "    xor rdx, rdx  ; flags = 0");
        wln!(self, "    mov r8, r12   ; ptr");
        wln!(self, "    sub rsp, 32");
        wln!(self, "    call HeapFree");
        wln!(self, "    add rsp, 32");
        wln!(self, "    mov rsp, rbp");
        wln!(self, "    pop rbp");
        wln!(self, "    ret");

        // _main — Windows entry point that calls our main
        wln!(self, "");
        wln!(self, "; ─── Windows entry point ───");
        wln!(self, "_main:");
        wln!(self, "    sub rsp, 40  ; 32 shadow + 8 align (OS entry: rsp is 8mod16)");
        wln!(self, "    call main");
        wln!(self, "    mov ecx, eax  ; exit code from main");
        wln!(self, "    call ExitProcess");
        wln!(self, "    ud2  ; should not reach here");
    }

}

/// Format a stack offset for display (e.g., -8 -> "-8", 0 -> "")
fn fmt_offset(offset: i64) -> String {
    if offset == 0 {
        String::new()
    } else if offset > 0 {
        format!("+{}", offset)
    } else {
        format!("-{}", -offset)
    }
}

/// Format a hex value for MASM (uses h suffix, leading 0 if starts with a-f)
fn fmt_hex(val: u32) -> String {
    let s = format!("{:x}", val);
    if s.starts_with(|c: char| c.is_ascii_digit()) {
        format!("{}h", s)
    } else {
        format!("0{}h", s)
    }
}

/// Escape a string for assembly output
fn escape_string(s: &str) -> String {
    let mut result = String::new();
    for c in s.chars() {
        match c {
            '"' => result.push_str("\\\""),
            '\\' => result.push_str("\\\\"),
            '\n' => result.push_str("\\n"),
            '\r' => result.push_str("\\r"),
            '\t' => result.push_str("\\t"),
            '\0' => result.push_str("\\0"),
            c if c.is_ascii_graphic() || c == ' ' => result.push(c),
            c => {
                let _ = write!(result, "\\x{:02x}", c as u32);
            }
        }
    }
    result
}

/// Mangle function names to avoid conflicts with x86 mnemonisms and MASM reserved words.
/// "main" and names starting with "__pledge" are kept as-is.
fn mangle_name(name: &str) -> String {
    // Don't mangle main or runtime stubs
    if name == "main" || name.starts_with("__pledge") {
        return name.to_string();
    }
    // x86 instruction mnemonics that could conflict
    const RESERVED: &[&str] = &[
        "add", "sub", "mul", "div", "mod", "and", "or", "xor", "not",
        "mov", "cmp", "test", "push", "pop", "call", "ret", "jmp",
        "je", "jne", "jz", "jnz", "jg", "jl", "jge", "jle", "ja", "jb",
        "inc", "dec", "neg", "shl", "shr", "lea", "nop", "int",
    ];
    if RESERVED.contains(&name) {
        format!("pledge_{}", name)
    } else {
        name.to_string()
    }
}

/// Convenience function to generate assembly from an IR module
pub fn generate_asm(module: &IrModule) -> String {
    let mut gen = X86CodeGen::new();
    gen.generate(module)
}
