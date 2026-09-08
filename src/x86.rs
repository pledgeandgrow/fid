// x86_64 backend — stack machine code generation from Fides IR (Linux port)
//
// Strategy: each SSA value lives on the stack. We use RAX as the accumulator.
// This is simple, correct, and a starting point for register allocation later.
//
// Linux System V AMD64 calling convention:
// - Integer args: RDI, RSI, RDX, RCX, R8, R9 (then stack)
// - Return: RAX
// - No shadow space (unlike Windows x64)
// - Stack must be 16-byte aligned before CALL
// - Output is AT&T syntax GAS assembly, consumed by elf64.rs assembler

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
        // .rodata section — string constants
        self.emit_data_section(module);

        // .text section — code
        wln!(self, ".text");
        wln!(self, "");

        // Generate each function
        for (name, func) in &module.functions {
            self.gen_function(name, func, module);
        }

        // Generate runtime stubs (Linux syscalls)
        self.gen_runtime_stubs();

        // Combine rodata + text
        format!("{}\n{}", self.data, self.out)
    }

    /// Emit the .rodata section with string constants (GAS AT&T syntax)
    fn emit_data_section(&mut self, module: &IrModule) {
        self.data.clear();
        self.data.push_str(".section .rodata\n");

        // Collect all string constants from all functions
        for func in module.functions.values() {
            for (vid, inst) in &func.instructions {
                if let Inst::ConstStr { value } = inst {
                    let label = self.intern_string(value);
                    let _ = vid;
                    let escaped = escape_string(value);
                    let _ = writeln!(self.data, "{}: .asciz \"{}\"", label, escaped);
                }
            }
        }

        // Boolean string constants
        let _ = writeln!(self.data, "str_true: .asciz \"true\"");
        let _ = writeln!(self.data, "str_false: .asciz \"false\"");

        // String concatenation buffers (static, in .bss)
        self.data.push_str("\n.section .bss\n");
        let _ = writeln!(self.data, "str_concat_buf: .skip 4096");
        let _ = writeln!(self.data, "str_concat_tmp: .skip 4096");

        self.data.push_str("\n.text\n");
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
        wln!(self, "# ─── Function: {} ───", name);
        wln!(self, "    .globl {}", mangled);
        wln!(self, "    .type {}, @function", mangled);
        wln!(self, "{}:", mangled);
        wln!(self, "    pushq %rbp");
        wln!(self, "    movq %rsp, %rbp");

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

        // Align stack to 16 bytes.
        // System V AMD64: no shadow space needed (unlike Windows x64).
        // After pushq %rbp, RSP is 16n+8. After subq $N, RSP must be 16-aligned.
        // So N must be 16n+8 to keep alignment. We round up to (size+8+15)&~15.
        let stack_size = ((frame.size + 8 + 15) & !15) as i64;
        // Reserve at least 16 bytes (minimum frame)
        let stack_size = stack_size.max(16);
        wln!(self, "    subq ${}, %rsp", stack_size);

        // Store parameters from registers to stack
        // System V AMD64: RDI, RSI, RDX, RCX, R8, R9 for first 6 integer args
        // 7th+ params are on the stack at 16(%rbp), 24(%rbp), etc.
        let arg_regs = ["%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"];
        for (i, (vid, _ty)) in func.params.iter().enumerate() {
            if i < 6 {
                let offset = frame.offset(*vid);
                wln!(self, "    movq {}, {}(%rbp)", arg_regs[i], fmt_offset(offset));
            } else {
                // 7th param at 16(%rbp), 8th at 24(%rbp), etc.
                let stack_offset = 16 + ((i - 6) as i64) * 8;
                let offset = frame.offset(*vid);
                wln!(self, "    movq {}(%rbp), %rax", stack_offset);
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(offset));
            }
        }

        // Generate code for each basic block
        for block in &func.blocks {
            wln!(self, "");
            wln!(self, ".L{}_bb_{}:", mangled, block.id.0);

            for &inst_id in &block.instructions {
                let inst = func.get_inst(inst_id);
                if let Some(inst) = inst {
                    self.gen_inst(inst_id, inst, &mut frame, func, module, &mangled);
                }
            }
        }

        // Function epilogue (in case we fall through)
        wln!(self, "    movq %rbp, %rsp");
        wln!(self, "    popq %rbp");
        wln!(self, "    ret");
        wln!(self, "    .size {}, .-{}", mangled, mangled);
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
                wln!(self, "    movq ${}, %rax", value);
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }

            Inst::ConstBool { value } => {
                let val = if *value { 1 } else { 0 };
                wln!(self, "    movq ${}, %rax", val);
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }

            Inst::ConstStr { value } => {
                // Check if this is a function name — emit function address
                if module.functions.contains_key(value) || module.extern_functions.contains_key(value) {
                    wln!(self, "    leaq {}(%rip), %rax", value);
                    wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
                } else {
                    let label = self.string_labels.get(value)
                        .cloned()
                        .unwrap_or_else(|| format!("str_unknown"));
                    wln!(self, "    leaq {}(%rip), %rax", label);
                    wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
                }
            }

            Inst::ConstFnRef { name } => {
                // Function reference — emit function address
                wln!(self, "    leaq {}(%rip), %rax", name);
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }

            Inst::ConstFloat { bits, ty } => {
                if *ty == IrType::F32 {
                    let val = *bits as u32;
                    wln!(self, "    movl ${}, {}(%rbp)", val as i32, fmt_offset(dst));
                } else {
                    let lo = *bits as u32;
                    let hi = (*bits >> 32) as u32;
                    wln!(self, "    movl ${}, {}(%rbp)", lo as i32, fmt_offset(dst));
                    wln!(self, "    movl ${}, {}(%rbp)", hi as i32, fmt_offset(dst + 4));
                }
            }

            Inst::ConstNull { .. } => {
                wln!(self, "    xorq %rax, %rax");
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }

            Inst::GlobalAddr { name, .. } => {
                wln!(self, "    leaq _g_{}(%rip), %rax", name);
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
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
                            // Float negation: XOR sign bit via integer XOR
                            if *ty == IrType::F32 {
                                wln!(self, "    movl {}(%rbp), %eax", fmt_offset(off));
                                wln!(self, "    movl $0x80000000, %ecx");
                                wln!(self, "    xorl %ecx, %eax");
                                wln!(self, "    movl %eax, {}(%rbp)", fmt_offset(dst));
                            } else {
                                wln!(self, "    movq {}(%rbp), %rax", fmt_offset(off));
                                wln!(self, "    movq $0x8000000000000000, %rcx");
                                wln!(self, "    xorq %rcx, %rax");
                                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
                            }
                        } else {
                            wln!(self, "    movq {}(%rbp), %rax", fmt_offset(off));
                            wln!(self, "    negq %rax");
                            wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
                        }
                    }
                    UnaryOp::Not => {
                        wln!(self, "    movq {}(%rbp), %rax", fmt_offset(off));
                        wln!(self, "    xorq $1, %rax");
                        wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
                    }
                    UnaryOp::BitNot => {
                        wln!(self, "    movq {}(%rbp), %rax", fmt_offset(off));
                        wln!(self, "    notq %rax");
                        wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
                    }
                }
            }

            // ─── Memory ───
            Inst::Alloca { .. } => {
                // Use pre-allocated data slot for this alloca
                let data_offset = *frame.alloca_slots.get(&id.0).unwrap_or(&0);
                wln!(self, "    leaq {}(%rbp), %rax", fmt_offset(data_offset));
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }

            Inst::Load { ptr, ty } => {
                let poff = frame.offset(*ptr);
                let size = module.type_size(ty).max(1);
                wln!(self, "    movq {}(%rbp), %rax", fmt_offset(poff));
                match size {
                    1 => wln!(self, "    movsbq (%rax), %rax"),
                    2 => wln!(self, "    movswq (%rax), %rax"),
                    4 => wln!(self, "    movslq (%rax), %rax"),
                    _ => wln!(self, "    movq (%rax), %rax"),
                }
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }

            Inst::Store { ptr, value, .. } => {
                let poff = frame.offset(*ptr);
                let voff = frame.offset(*value);
                wln!(self, "    movq {}(%rbp), %rax", fmt_offset(poff));
                wln!(self, "    movq {}(%rbp), %rcx", fmt_offset(voff));
                // Use correct store size based on value type
                let val_ty = func.values.get(value).map(|m| &m.ty);
                let store_size = val_ty.map(|t| module.type_size(t)).unwrap_or(8);
                match store_size {
                    1 => wln!(self, "    movb %cl, (%rax)"),
                    2 => wln!(self, "    movw %cx, (%rax)"),
                    4 => wln!(self, "    movl %ecx, (%rax)"),
                    _ => wln!(self, "    movq %rcx, (%rax)"),
                }
            }

            Inst::GetFieldPtr { struct_ptr, field_index, field_ty: _, .. } => {
                let poff = frame.offset(*struct_ptr);
                wln!(self, "    movq {}(%rbp), %rax", fmt_offset(poff));
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
                    wln!(self, "    addq ${}, %rax", offset);
                }
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }

            Inst::GetElemPtr { array_ptr, index, elem_ty } => {
                let poff = frame.offset(*array_ptr);
                let ioff = frame.offset(*index);
                let elem_size = module.type_size(elem_ty).max(8);
                wln!(self, "    movq {}(%rbp), %rax", fmt_offset(poff));
                wln!(self, "    movq {}(%rbp), %rcx", fmt_offset(ioff));
                wln!(self, "    imulq ${}, %rcx", elem_size);
                wln!(self, "    addq %rcx, %rax");
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }

            // ─── Control Flow ───
            Inst::Jump { target } => {
                wln!(self, "    jmp .L{}_bb_{}", func_name, target.0);
            }

            Inst::Branch { cond, then, else_ } => {
                let coff = frame.offset(*cond);
                wln!(self, "    movq {}(%rbp), %rax", fmt_offset(coff));
                wln!(self, "    testq %rax, %rax");
                wln!(self, "    jnz .L{}_bb_{}", func_name, then.0);
                wln!(self, "    jmp .L{}_bb_{}", func_name, else_.0);
            }

            Inst::Return { value } => {
                if let Some(v) = value {
                    let voff = frame.offset(*v);
                    wln!(self, "    movq {}(%rbp), %rax", fmt_offset(voff));
                } else {
                    wln!(self, "    xorq %rax, %rax");
                }
                wln!(self, "    movq %rbp, %rsp");
                wln!(self, "    popq %rbp");
                wln!(self, "    ret");
            }

            Inst::Unreachable => {
                wln!(self, "    ud2  # unreachable");
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

                // System V AMD64: RDI, RSI, RDX, RCX, R8, R9 for first 6 args
                // 7th+ args go on stack at 0(%rsp), 8(%rsp), etc.
                // No shadow space needed.
                let arg_regs = ["%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"];
                let stack_args = if args.len() > 6 { args.len() - 6 } else { 0 };
                // Reserve space for stack args, aligned to 16
                let call_space = ((stack_args * 8 + 15) & !15) as i64;
                // Load register args first (before adjusting RSP)
                for (i, arg) in args.iter().enumerate() {
                    let aoff = frame.offset(*arg);
                    if i < 6 {
                        wln!(self, "    movq {}(%rbp), {}", fmt_offset(aoff), arg_regs[i]);
                    }
                }
                if call_space > 0 {
                    wln!(self, "    subq ${}, %rsp  # stack args", call_space);
                }
                // Place stack args (7th+ args)
                for (i, arg) in args.iter().enumerate().skip(6) {
                    let aoff = frame.offset(*arg);
                    let stack_pos = ((i - 6) as i64) * 8;
                    wln!(self, "    movq {}(%rbp), %rax", fmt_offset(aoff));
                    wln!(self, "    movq %rax, {}(%rsp)", stack_pos);
                }
                // Ensure 16-byte alignment before call
                // RSP is 16n after prologue. If call_space is not 8mod16, we need padding.
                if call_space % 16 != 0 {
                    wln!(self, "    subq $8, %rsp  # alignment");
                }
                wln!(self, "    call {}", target_func);
                if call_space % 16 != 0 {
                    wln!(self, "    addq $8, %rsp  # alignment cleanup");
                }
                if call_space > 0 {
                    wln!(self, "    addq ${}, %rsp  # cleanup", call_space);
                }
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }

            Inst::CallPtr { fn_ptr, args, .. } => {
                let fpoff = frame.offset(*fn_ptr);

                // All function pointers are env structs: field 0 = fn addr, fields 1..N = captures
                // Load function address from field 0
                wln!(self, "    movq {}(%rbp), %rax", fmt_offset(fpoff));
                wln!(self, "    movq (%rax), %rax  # load fn addr from env struct field 0");

                // Pass env_ptr as first arg (rdi), shift other args
                let total_args = args.len() + 1; // +1 for env_ptr
                let arg_regs = ["%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"];
                let stack_args = if total_args > 6 { total_args - 6 } else { 0 };
                let call_space = ((stack_args * 8 + 15) & !15) as i64;

                if call_space > 0 {
                    wln!(self, "    subq ${}, %rsp  # stack args", call_space);
                }

                // First arg (rdi) = env_ptr
                wln!(self, "    movq {}(%rbp), %rdi", fmt_offset(fpoff));

                // Remaining args shifted by 1
                for (i, arg) in args.iter().enumerate() {
                    let aoff = frame.offset(*arg);
                    let arg_idx = i + 1; // shifted by 1 for env_ptr
                    if arg_idx < 6 {
                        wln!(self, "    movq {}(%rbp), {}", fmt_offset(aoff), arg_regs[arg_idx]);
                    } else {
                        let stack_pos = ((arg_idx - 6) as i64) * 8;
                        wln!(self, "    movq {}(%rbp), %r10", fmt_offset(aoff));
                        wln!(self, "    movq %r10, {}(%rsp)", stack_pos);
                    }
                }

                // Ensure 16-byte alignment before call
                if call_space % 16 != 0 {
                    wln!(self, "    subq $8, %rsp  # alignment");
                }
                // Save fn addr, call indirectly
                wln!(self, "    movq %rax, %r10  # save fn addr");
                wln!(self, "    call *%r10");
                if call_space % 16 != 0 {
                    wln!(self, "    addq $8, %rsp  # alignment cleanup");
                }
                if call_space > 0 {
                    wln!(self, "    addq ${}, %rsp  # cleanup", call_space);
                }
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }

            // ─── Conversions ───
            Inst::IntExtend { value, from, to } => {
                let voff = frame.offset(*value);
                wln!(self, "    movq {}(%rbp), %rax", fmt_offset(voff));
                // Sign-extend based on source type
                match from {
                    IrType::I8 => wln!(self, "    movsbq %al, %rax"),
                    IrType::I16 => wln!(self, "    movswq %ax, %rax"),
                    IrType::I32 => wln!(self, "    movslq %eax, %rax"),
                    _ => {}
                }
                let _ = to;
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }

            Inst::IntTrunc { value, from, to } => {
                let voff = frame.offset(*value);
                wln!(self, "    movq {}(%rbp), %rax", fmt_offset(voff));
                // Truncate by masking
                match to {
                    IrType::I8 | IrType::U8 => wln!(self, "    movzbq %al, %rax"),
                    IrType::I16 | IrType::U16 => wln!(self, "    movzwq %ax, %rax"),
                    IrType::I32 | IrType::U32 => wln!(self, "    movl %eax, %eax"),
                    _ => {}
                }
                let _ = from;
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }

            Inst::IntToFloat { value, from, to } => {
                let voff = frame.offset(*value);
                wln!(self, "    movq {}(%rbp), %rax", fmt_offset(voff));
                match from {
                    IrType::I64 | IrType::U64 => wln!(self, "    cvtsi2sdq %rax, %xmm0"),
                    _ => wln!(self, "    cvtsi2sd %eax, %xmm0"),
                }
                if *to == IrType::F32 {
                    wln!(self, "    movss %xmm0, {}(%rbp)", fmt_offset(dst));
                } else {
                    wln!(self, "    movsd %xmm0, {}(%rbp)", fmt_offset(dst));
                }
            }

            Inst::FloatToInt { value, from, to } => {
                let voff = frame.offset(*value);
                if *from == IrType::F32 {
                    wln!(self, "    movss {}(%rbp), %xmm0", fmt_offset(voff));
                } else {
                    wln!(self, "    movsd {}(%rbp), %xmm0", fmt_offset(voff));
                }
                wln!(self, "    cvttsd2siq %xmm0, %rax");
                let _ = to;
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }

            Inst::MakeDyn { .. } => {
                wln!(self, "    xorq %rax, %rax  # MakeDyn not yet supported");
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }
            Inst::DynCall { .. } => {
                wln!(self, "    xorq %rax, %rax  # DynCall not yet supported");
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }
            Inst::BitCast { value, .. } => {
                let voff = frame.offset(*value);
                wln!(self, "    movq {}(%rbp), %rax", fmt_offset(voff));
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }

            // ─── Fides-Specific ───
            Inst::Print { value, ty } => {
                let voff = frame.offset(*value);
                let _ = ty;
                // Call our print stub (System V: arg in %rdi)
                wln!(self, "    movq {}(%rbp), %rdi", fmt_offset(voff));
                wln!(self, "    call p_print_int");
            }

            Inst::PersistLoad { default, .. } => {
                // Just copy default value to destination
                let doff = frame.offset(*default);
                wln!(self, "    movq {}(%rbp), %rax", fmt_offset(doff));
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }
            Inst::PersistSave { .. } => {
                // No-op
            }
            Inst::TxnBegin | Inst::TxnEnd | Inst::UnsafeBegin | Inst::UnsafeEnd | Inst::GradBegin { .. } | Inst::GradEnd | Inst::Backward { .. } => {
                // No-op (rollback/unsafe not supported)
            }
            Inst::Panic { message } => {
                let label = self.intern_string(message);
                // Emit the string in rodata section if not already there
                if !self.data.contains(&format!("{}: .asciz", label)) {
                    let _ = writeln!(self.data, "{}: .asciz \"{}\"", label, escape_string(message));
                }
                wln!(self, "    leaq {}(%rip), %rdi", label);
                wln!(self, "    call p_panic");
            }

            Inst::TryUnwrap { .. } => {
                // Simplified — not fully implemented in backend yet
                wln!(self, "    # TODO: TryUnwrap");
            }
            Inst::Asm { .. } => {
                wln!(self, "    # asm not yet supported");
                wln!(self, "    xorq %rax, %rax");
                wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
            }
            Inst::RegionBegin { .. } => {
                wln!(self, "    # region begin (no-op — stack allocation)");
                wln!(self, "    xorq %rax, %rax");
            }
            Inst::RegionEnd { .. } => {
                wln!(self, "    # region end (no-op)");
            }
            Inst::GradBegin { .. } | Inst::GradEnd | Inst::Backward { .. } => {
                wln!(self, "    # autodiff marker (no-op)");
            }
        }
    }

    /// Generate integer binary operation
    fn gen_int_binop(&mut self, op: &BinOp, loff: i64, roff: i64, dst: i64, _ty: &IrType) {
        // Load left into RAX, right into RCX
        wln!(self, "    movq {}(%rbp), %rax", fmt_offset(loff));
        wln!(self, "    movq {}(%rbp), %rcx", fmt_offset(roff));

        match op {
            BinOp::Add => {
                wln!(self, "    addq %rcx, %rax");
            }
            BinOp::Sub => {
                wln!(self, "    subq %rcx, %rax");
            }
            BinOp::Mul => {
                wln!(self, "    imulq %rcx, %rax");
            }
            BinOp::Div => {
                // Sign-extend RAX into RDX:RAX, then idiv
                wln!(self, "    cqto");
                wln!(self, "    idivq %rcx");
            }
            BinOp::Mod => {
                wln!(self, "    cqto");
                wln!(self, "    idivq %rcx");
                wln!(self, "    movq %rdx, %rax"); // remainder
            }
            BinOp::BitAnd => {
                wln!(self, "    andq %rcx, %rax");
            }
            BinOp::BitOr => {
                wln!(self, "    orq %rcx, %rax");
            }
            BinOp::BitXor => {
                wln!(self, "    xorq %rcx, %rax");
            }
            BinOp::Shl => {
                // Shift count must be in CL
                wln!(self, "    shlq %cl, %rax");
            }
            BinOp::Shr => {
                wln!(self, "    sarq %cl, %rax");
            }
            BinOp::Eq => {
                wln!(self, "    cmpq %rcx, %rax");
                wln!(self, "    sete %al");
                wln!(self, "    movzbq %al, %rax");
            }
            BinOp::Neq => {
                wln!(self, "    cmpq %rcx, %rax");
                wln!(self, "    setne %al");
                wln!(self, "    movzbq %al, %rax");
            }
            BinOp::Lt => {
                wln!(self, "    cmpq %rcx, %rax");
                wln!(self, "    setl %al");
                wln!(self, "    movzbq %al, %rax");
            }
            BinOp::Gt => {
                wln!(self, "    cmpq %rcx, %rax");
                wln!(self, "    setg %al");
                wln!(self, "    movzbq %al, %rax");
            }
            BinOp::LtEq => {
                wln!(self, "    cmpq %rcx, %rax");
                wln!(self, "    setle %al");
                wln!(self, "    movzbq %al, %rax");
            }
            BinOp::GtEq => {
                wln!(self, "    cmpq %rcx, %rax");
                wln!(self, "    setge %al");
                wln!(self, "    movzbq %al, %rax");
            }
            BinOp::And => {
                wln!(self, "    testq %rax, %rax");
                wln!(self, "    setne %al");
                wln!(self, "    testq %rcx, %rcx");
                wln!(self, "    setne %cl");
                wln!(self, "    andb %cl, %al");
                wln!(self, "    movzbq %al, %rax");
            }
            BinOp::Or => {
                wln!(self, "    orq %rcx, %rax");
                wln!(self, "    setne %al");
                wln!(self, "    movzbq %al, %rax");
            }
        }

        wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
    }

    /// Generate float binary operation
    fn gen_float_binop(&mut self, op: &BinOp, loff: i64, roff: i64, dst: i64, ty: &IrType) {
        if *ty == IrType::F32 {
            wln!(self, "    movss {}(%rbp), %xmm0", fmt_offset(loff));
            wln!(self, "    movss {}(%rbp), %xmm1", fmt_offset(roff));
            match op {
                BinOp::Add => wln!(self, "    addss %xmm1, %xmm0"),
                BinOp::Sub => wln!(self, "    subss %xmm1, %xmm0"),
                BinOp::Mul => wln!(self, "    mulss %xmm1, %xmm0"),
                BinOp::Div => wln!(self, "    divss %xmm1, %xmm0"),
                BinOp::Eq => {
                    wln!(self, "    ucomiss %xmm1, %xmm0");
                    wln!(self, "    sete %al");
                    wln!(self, "    movzbq %al, %rax");
                }
                BinOp::Neq => {
                    wln!(self, "    ucomiss %xmm1, %xmm0");
                    wln!(self, "    setne %al");
                    wln!(self, "    movzbq %al, %rax");
                }
                BinOp::Lt => {
                    wln!(self, "    ucomiss %xmm1, %xmm0");
                    wln!(self, "    setb %al");
                    wln!(self, "    movzbq %al, %rax");
                }
                BinOp::Gt => {
                    wln!(self, "    ucomiss %xmm0, %xmm1");
                    wln!(self, "    setb %al");
                    wln!(self, "    movzbq %al, %rax");
                }
                BinOp::LtEq => {
                    wln!(self, "    ucomiss %xmm1, %xmm0");
                    wln!(self, "    setbe %al");
                    wln!(self, "    movzbq %al, %rax");
                }
                BinOp::GtEq => {
                    wln!(self, "    ucomiss %xmm0, %xmm1");
                    wln!(self, "    setbe %al");
                    wln!(self, "    movzbq %al, %rax");
                }
                _ => {}
            }
            // For arithmetic ops, store xmm0; for comparisons, store rax
            match op {
                BinOp::Add | BinOp::Sub | BinOp::Mul | BinOp::Div => {
                    wln!(self, "    movss %xmm0, {}(%rbp)", fmt_offset(dst));
                }
                _ => {
                    wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
                }
            }
        } else {
            // F64
            wln!(self, "    movsd {}(%rbp), %xmm0", fmt_offset(loff));
            wln!(self, "    movsd {}(%rbp), %xmm1", fmt_offset(roff));
            match op {
                BinOp::Add => wln!(self, "    addsd %xmm1, %xmm0"),
                BinOp::Sub => wln!(self, "    subsd %xmm1, %xmm0"),
                BinOp::Mul => wln!(self, "    mulsd %xmm1, %xmm0"),
                BinOp::Div => wln!(self, "    divsd %xmm1, %xmm0"),
                BinOp::Eq => {
                    wln!(self, "    ucomisd %xmm1, %xmm0");
                    wln!(self, "    sete %al");
                    wln!(self, "    movzbq %al, %rax");
                }
                BinOp::Neq => {
                    wln!(self, "    ucomisd %xmm1, %xmm0");
                    wln!(self, "    setne %al");
                    wln!(self, "    movzbq %al, %rax");
                }
                BinOp::Lt => {
                    wln!(self, "    ucomisd %xmm1, %xmm0");
                    wln!(self, "    setb %al");
                    wln!(self, "    movzbq %al, %rax");
                }
                BinOp::Gt => {
                    wln!(self, "    ucomisd %xmm0, %xmm1");
                    wln!(self, "    setb %al");
                    wln!(self, "    movzbq %al, %rax");
                }
                BinOp::LtEq => {
                    wln!(self, "    ucomisd %xmm1, %xmm0");
                    wln!(self, "    setbe %al");
                    wln!(self, "    movzbq %al, %rax");
                }
                BinOp::GtEq => {
                    wln!(self, "    ucomisd %xmm0, %xmm1");
                    wln!(self, "    setbe %al");
                    wln!(self, "    movzbq %al, %rax");
                }
                _ => {}
            }
            match op {
                BinOp::Add | BinOp::Sub | BinOp::Mul | BinOp::Div => {
                    wln!(self, "    movsd %xmm0, {}(%rbp)", fmt_offset(dst));
                }
                _ => {
                    wln!(self, "    movq %rax, {}(%rbp)", fmt_offset(dst));
                }
            }
        }
    }

    /// Generate runtime stubs — Linux syscalls (no C runtime)
    /// System V AMD64 calling convention: args in RDI, RSI, RDX, R10, R8, R9
    /// Linux syscall numbers: write=1, exit=60, mmap=9, munmap=11
    fn gen_runtime_stubs(&mut self) {
        wln!(self, "");
        wln!(self, "# ─── Runtime stubs (Linux syscalls) ───");

        // p_print_int(rdi = i64 value)
        // Converts integer to string and writes to stdout + newline
        wln!(self, "p_print_int:");
        wln!(self, "    pushq %rbp");
        wln!(self, "    movq %rsp, %rbp");
        wln!(self, "    pushq %r12");
        wln!(self, "    pushq %r13");
        wln!(self, "    subq $80, %rsp  # buffer + alignment");
        wln!(self, "");
        wln!(self, "    movq %rdi, %rax  # value");
        wln!(self, "    leaq -1(%rbp), %r8  # buffer end pointer");
        wln!(self, "    movb $10, (%r8)  # newline at end");
        wln!(self, "    movq $10, %r9  # divisor");
        wln!(self, "");
        wln!(self, "    testq %rax, %rax");
        wln!(self, "    jns pi_pos");
        wln!(self, "    negq %rax");
        wln!(self, "pi_pos:");
        wln!(self, "pi_loop:");
        wln!(self, "    decq %r8");
        wln!(self, "    xorq %rdx, %rdx");
        wln!(self, "    divq %r9");
        wln!(self, "    addb $48, %dl  # '0'");
        wln!(self, "    movb %dl, (%r8)");
        wln!(self, "    testq %rax, %rax");
        wln!(self, "    jnz pi_loop");
        wln!(self, "");
        wln!(self, "    testq %rdi, %rdi");
        wln!(self, "    jns pi_no_neg");
        wln!(self, "    decq %r8");
        wln!(self, "    movb $45, (%r8)  # '-'");
        wln!(self, "pi_no_neg:");
        wln!(self, "");
        wln!(self, "    # write(1, r8, len)");
        wln!(self, "    leaq (%rbp), %r9  # end (after newline)");
        wln!(self, "    subq %r8, %r9  # length");
        wln!(self, "    movq $1, %rax  # syscall write");
        wln!(self, "    movq $1, %rdi  # stdout");
        wln!(self, "    movq %r8, %rsi  # buffer");
        wln!(self, "    movq %r9, %rdx  # length");
        wln!(self, "    syscall");
        wln!(self, "    xorq %rax, %rax");
        wln!(self, "    addq $80, %rsp");
        wln!(self, "    popq %r13");
        wln!(self, "    popq %r12");
        wln!(self, "    movq %rbp, %rsp");
        wln!(self, "    popq %rbp");
        wln!(self, "    ret");

        // p_print_str(rdi = *u8 pointer to null-terminated string)
        wln!(self, "");
        wln!(self, "p_print_str:");
        wln!(self, "    pushq %rbp");
        wln!(self, "    movq %rsp, %rbp");
        wln!(self, "    pushq %r12");
        wln!(self, "    pushq %r13");
        wln!(self, "    subq $32, %rsp");
        wln!(self, "    movq %rdi, %r12  # save string pointer");
        wln!(self, "");
        wln!(self, "    # Calculate string length");
        wln!(self, "    xorq %r8, %r8");
        wln!(self, "ps_len:");
        wln!(self, "    cmpb $0, (%r12,%r8)");
        wln!(self, "    je ps_done");
        wln!(self, "    incq %r8");
        wln!(self, "    jmp ps_len");
        wln!(self, "ps_done:");
        wln!(self, "    movq %r8, %r13  # save length");
        wln!(self, "");
        wln!(self, "    # write(1, r12, r13)");
        wln!(self, "    movq $1, %rax  # syscall write");
        wln!(self, "    movq $1, %rdi  # stdout");
        wln!(self, "    movq %r12, %rsi  # buffer");
        wln!(self, "    movq %r13, %rdx  # length");
        wln!(self, "    syscall");
        wln!(self, "");
        wln!(self, "    # Print newline");
        wln!(self, "    movb $10, -1(%rbp)");
        wln!(self, "    movq $1, %rax  # syscall write");
        wln!(self, "    movq $1, %rdi  # stdout");
        wln!(self, "    leaq -1(%rbp), %rsi  # buffer");
        wln!(self, "    movq $1, %rdx  # length");
        wln!(self, "    syscall");
        wln!(self, "    xorq %rax, %rax");
        wln!(self, "    addq $32, %rsp");
        wln!(self, "    popq %r13");
        wln!(self, "    popq %r12");
        wln!(self, "    movq %rbp, %rsp");
        wln!(self, "    popq %rbp");
        wln!(self, "    ret");

        // p_print_bool(rdi = 0 or 1)
        wln!(self, "");
        wln!(self, "p_print_bool:");
        wln!(self, "    pushq %rbp");
        wln!(self, "    movq %rsp, %rbp");
        wln!(self, "    pushq %r12");
        wln!(self, "    pushq %r13");
        wln!(self, "    subq $32, %rsp");
        wln!(self, "    testq %rdi, %rdi");
        wln!(self, "    jnz pb_true");
        wln!(self, "    leaq str_false(%rip), %r12");
        wln!(self, "    jmp pb_print");
        wln!(self, "pb_true:");
        wln!(self, "    leaq str_true(%rip), %r12");
        wln!(self, "pb_print:");
        wln!(self, "    xorq %r8, %r8");
        wln!(self, "pb_len:");
        wln!(self, "    cmpb $0, (%r12,%r8)");
        wln!(self, "    je pb_done");
        wln!(self, "    incq %r8");
        wln!(self, "    jmp pb_len");
        wln!(self, "pb_done:");
        wln!(self, "    movq %r8, %r13");
        wln!(self, "    movq $1, %rax  # syscall write");
        wln!(self, "    movq $1, %rdi  # stdout");
        wln!(self, "    movq %r12, %rsi");
        wln!(self, "    movq %r13, %rdx");
        wln!(self, "    syscall");
        wln!(self, "    # newline");
        wln!(self, "    movb $10, -1(%rbp)");
        wln!(self, "    movq $1, %rax");
        wln!(self, "    movq $1, %rdi");
        wln!(self, "    leaq -1(%rbp), %rsi");
        wln!(self, "    movq $1, %rdx");
        wln!(self, "    syscall");
        wln!(self, "    xorq %rax, %rax");
        wln!(self, "    addq $32, %rsp");
        wln!(self, "    popq %r13");
        wln!(self, "    popq %r12");
        wln!(self, "    movq %rbp, %rsp");
        wln!(self, "    popq %rbp");
        wln!(self, "    ret");

        // p_print_float(rdi = f64 bits stored as i64)
        // Formats as integer.fractional with 6 decimal places
        wln!(self, "");
        wln!(self, "p_print_float:");
        wln!(self, "    pushq %rbp");
        wln!(self, "    movq %rsp, %rbp");
        wln!(self, "    pushq %r12");
        wln!(self, "    pushq %r13");
        wln!(self, "    pushq %r14");
        wln!(self, "    subq $152, %rsp  # buffer(64) + temp(32) + locals(56)");
        wln!(self, "");
        wln!(self, "    # Save float bits and load as xmm0");
        wln!(self, "    movq %rdi, -72(%rbp)");
        wln!(self, "    movsd -72(%rbp), %xmm0");
        wln!(self, "");
        wln!(self, "    # r12 = buffer pointer, r13 = length");
        wln!(self, "    leaq -64(%rbp), %r12");
        wln!(self, "    xorq %r13, %r13");
        wln!(self, "");
        wln!(self, "    # Check negative");
        wln!(self, "    xorpd %xmm1, %xmm1");
        wln!(self, "    ucomisd %xmm1, %xmm0");
        wln!(self, "    jae pf_pos");
        wln!(self, "    movb $45, (%r12)  # '-'");
        wln!(self, "    incq %r12");
        wln!(self, "    incq %r13");
        wln!(self, "    # Flip sign bit via integer XOR");
        wln!(self, "    movq $0x8000000000000000, %rax");
        wln!(self, "    movq %rax, -80(%rbp)");
        wln!(self, "    movsd -80(%rbp), %xmm2");
        wln!(self, "    xorpd %xmm2, %xmm0");
        wln!(self, "");
        wln!(self, "pf_pos:");
        wln!(self, "    # Save xmm0 (possibly negated)");
        wln!(self, "    movsd %xmm0, -72(%rbp)");
        wln!(self, "");
        wln!(self, "    # Get integer part");
        wln!(self, "    cvttsd2siq %xmm0, %rax");
        wln!(self, "    movq %rax, %r14  # save integer part");
        wln!(self, "");
        wln!(self, "    # Handle integer part == 0");
        wln!(self, "    testq %rax, %rax");
        wln!(self, "    jnz pf_int_start");
        wln!(self, "    movb $48, (%r12)  # '0'");
        wln!(self, "    incq %r12");
        wln!(self, "    incq %r13");
        wln!(self, "    jmp pf_dot");
        wln!(self, "");
        wln!(self, "pf_int_start:");
        wln!(self, "    # Convert integer to reversed string at -128(%rbp)");
        wln!(self, "    leaq -128(%rbp), %r8");
        wln!(self, "    xorq %r9, %r9");
        wln!(self, "pf_int_loop:");
        wln!(self, "    xorq %rdx, %rdx");
        wln!(self, "    movq $10, %rcx");
        wln!(self, "    divq %rcx");
        wln!(self, "    addb $48, %dl  # '0'");
        wln!(self, "    movb %dl, (%r8)");
        wln!(self, "    incq %r8");
        wln!(self, "    incq %r9");
        wln!(self, "    testq %rax, %rax");
        wln!(self, "    jnz pf_int_loop");
        wln!(self, "");
        wln!(self, "    # Copy reversed digits to buffer in correct order");
        wln!(self, "    decq %r8");
        wln!(self, "pf_int_copy:");
        wln!(self, "    movb (%r8), %al");
        wln!(self, "    movb %al, (%r12)");
        wln!(self, "    incq %r12");
        wln!(self, "    incq %r13");
        wln!(self, "    decq %r8");
        wln!(self, "    decq %r9");
        wln!(self, "    jnz pf_int_copy");
        wln!(self, "");
        wln!(self, "pf_dot:");
        wln!(self, "    movb $46, (%r12)  # '.'");
        wln!(self, "    incq %r12");
        wln!(self, "    incq %r13");
        wln!(self, "");
        wln!(self, "    # Get fractional part: xmm0 = original - integer_part");
        wln!(self, "    movsd -72(%rbp), %xmm0");
        wln!(self, "    cvtsi2sdq %r14, %xmm1");
        wln!(self, "    subsd %xmm1, %xmm0");
        wln!(self, "    # Clamp negative to 0 (FP precision)");
        wln!(self, "    xorpd %xmm1, %xmm1");
        wln!(self, "    ucomisd %xmm1, %xmm0");
        wln!(self, "    jae pf_frac_ok");
        wln!(self, "    movsd %xmm1, %xmm0");
        wln!(self, "pf_frac_ok:");
        wln!(self, "");
        wln!(self, "    # Load 1000000.0 constant");
        wln!(self, "    movq $0x412E848000000000, %rax");
        wln!(self, "    movq %rax, -80(%rbp)");
        wln!(self, "    movsd -80(%rbp), %xmm2  # xmm2 = 1000000.0");
        wln!(self, "");
        wln!(self, "    # Multiply fractional part by 1000000");
        wln!(self, "    mulsd %xmm2, %xmm0");
        wln!(self, "");
        wln!(self, "    # Add 0.5 for rounding");
        wln!(self, "    movq $0x3FE0000000000000, %rax");
        wln!(self, "    movq %rax, -80(%rbp)");
        wln!(self, "    movsd -80(%rbp), %xmm1  # xmm1 = 0.5");
        wln!(self, "    addsd %xmm1, %xmm0");
        wln!(self, "");
        wln!(self, "    # Truncate to integer");
        wln!(self, "    cvttsd2siq %xmm0, %rax  # rax = rounded fractional * 10^6");
        wln!(self, "");
        wln!(self, "    # Extract 6 digits from rax (reversed into temp buffer)");
        wln!(self, "    leaq -128(%rbp), %r8");
        wln!(self, "    movq $6, %rcx");
        wln!(self, "    movq $10, %r9");
        wln!(self, "pf_frac_loop:");
        wln!(self, "    xorq %rdx, %rdx");
        wln!(self, "    divq %r9");
        wln!(self, "    addb $48, %dl  # '0'");
        wln!(self, "    movb %dl, (%r8)");
        wln!(self, "    incq %r8");
        wln!(self, "    decq %rcx");
        wln!(self, "    jnz pf_frac_loop");
        wln!(self, "");
        wln!(self, "    # Copy digits in reverse order to output buffer");
        wln!(self, "    decq %r8");
        wln!(self, "    movq $6, %rcx");
        wln!(self, "pf_frac_copy:");
        wln!(self, "    movb (%r8), %al");
        wln!(self, "    movb %al, (%r12)");
        wln!(self, "    incq %r12");
        wln!(self, "    incq %r13");
        wln!(self, "    decq %r8");
        wln!(self, "    decq %rcx");
        wln!(self, "    jnz pf_frac_copy");
        wln!(self, "");
        wln!(self, "    # Add newline");
        wln!(self, "    movb $10, (%r12)");
        wln!(self, "    incq %r13");
        wln!(self, "");
        wln!(self, "    # write(1, buffer, length)");
        wln!(self, "    movq $1, %rax  # syscall write");
        wln!(self, "    movq $1, %rdi  # stdout");
        wln!(self, "    leaq -64(%rbp), %rsi  # buffer");
        wln!(self, "    movq %r13, %rdx  # length");
        wln!(self, "    syscall");
        wln!(self, "");
        wln!(self, "    xorq %rax, %rax");
        wln!(self, "    addq $152, %rsp");
        wln!(self, "    popq %r14");
        wln!(self, "    popq %r13");
        wln!(self, "    popq %r12");
        wln!(self, "    movq %rbp, %rsp");
        wln!(self, "    popq %rbp");
        wln!(self, "    ret");

        // p_to_str(rdi = i64 value) -> rax = pointer to null-terminated string
        // Converts an integer to a string in a stack buffer
        wln!(self, "");
        wln!(self, "p_to_str:");
        wln!(self, "    pushq %rbp");
        wln!(self, "    movq %rsp, %rbp");
        wln!(self, "    subq $64, %rsp  # buffer");
        wln!(self, "    leaq -1(%rbp), %r8  # buffer end pointer");
        wln!(self, "    movb $0, (%r8)  # null terminator");
        wln!(self, "    movq %rdi, %rax  # value");
        wln!(self, "    movq $10, %r9  # divisor");
        wln!(self, "    testq %rax, %rax");
        wln!(self, "    jns ts_pos");
        wln!(self, "    negq %rax");
        wln!(self, "ts_pos:");
        wln!(self, "ts_loop:");
        wln!(self, "    decq %r8");
        wln!(self, "    xorq %rdx, %rdx");
        wln!(self, "    divq %r9");
        wln!(self, "    addb $48, %dl  # '0'");
        wln!(self, "    movb %dl, (%r8)");
        wln!(self, "    testq %rax, %rax");
        wln!(self, "    jnz ts_loop");
        wln!(self, "    testq %rdi, %rdi");
        wln!(self, "    jns ts_no_neg");
        wln!(self, "    decq %r8");
        wln!(self, "    movb $45, (%r8)  # '-'");
        wln!(self, "ts_no_neg:");
        wln!(self, "    # Handle zero case");
        wln!(self, "    cmpb $0, (%r8)");
        wln!(self, "    jne ts_done");
        wln!(self, "    decq %r8");
        wln!(self, "    movb $48, (%r8)  # '0'");
        wln!(self, "ts_done:");
        wln!(self, "    leaq (%r8), %rax  # return pointer to start of string");
        wln!(self, "    movq %rbp, %rsp");
        wln!(self, "    popq %rbp");
        wln!(self, "    ret");

        // p_str_concat(rdi = left ptr, rsi = right ptr) -> rax = new string ptr
        // Concatenates two null-terminated strings into a static buffer
        wln!(self, "");
        wln!(self, "p_str_concat:");
        wln!(self, "    pushq %rbp");
        wln!(self, "    movq %rsp, %rbp");
        wln!(self, "    # Save left/right pointers");
        wln!(self, "    movq %rdi, %rcx  # left");
        wln!(self, "    movq %rsi, %rdx  # right");
        wln!(self, "    # Copy left string to temp buffer first (avoid aliasing)");
        wln!(self, "    leaq str_concat_tmp(%rip), %r8  # temp buffer");
        wln!(self, "    xorq %r9, %r9  # index");
        wln!(self, "sc_copy:");
        wln!(self, "    movb (%rcx,%r9), %al");
        wln!(self, "    movb %al, (%r8,%r9)");
        wln!(self, "    testb %al, %al");
        wln!(self, "    jz sc_copy_done");
        wln!(self, "    incq %r9");
        wln!(self, "    jmp sc_copy");
        wln!(self, "sc_copy_done:");
        wln!(self, "    # Now copy temp (left) to output buffer");
        wln!(self, "    leaq str_concat_buf(%rip), %r8  # destination");
        wln!(self, "    leaq str_concat_tmp(%rip), %r11  # source (left)");
        wln!(self, "    xorq %rcx, %rcx  # index for left");
        wln!(self, "sc_left:");
        wln!(self, "    movb (%r11,%rcx), %al");
        wln!(self, "    testb %al, %al");
        wln!(self, "    jz sc_right");
        wln!(self, "    movb %al, (%r8,%rcx)");
        wln!(self, "    incq %rcx");
        wln!(self, "    jmp sc_left");
        wln!(self, "sc_right:");
        wln!(self, "    # r9 = length of left (rcx), copy right string");
        wln!(self, "    movq %rcx, %r9  # left length");
        wln!(self, "    xorq %r10, %r10  # index for right");
        wln!(self, "sc_right_loop:");
        wln!(self, "    movb (%rdx,%r10), %al");
        wln!(self, "    testb %al, %al");
        wln!(self, "    jz sc_end");
        wln!(self, "    movb %al, (%r8,%r9)");
        wln!(self, "    incq %r9");
        wln!(self, "    incq %r10");
        wln!(self, "    jmp sc_right_loop");
        wln!(self, "sc_end:");
        wln!(self, "    movb $0, (%r8,%r9)  # null terminate");
        wln!(self, "    leaq str_concat_buf(%rip), %rax  # return pointer");
        wln!(self, "    movq %rbp, %rsp");
        wln!(self, "    popq %rbp");
        wln!(self, "    ret");

        // p_panic(rdi = message pointer)
        wln!(self, "");
        wln!(self, "p_panic:");
        wln!(self, "    pushq %rbp");
        wln!(self, "    movq %rsp, %rbp");
        wln!(self, "    # Save message pointer");
        wln!(self, "    movq %rdi, %r12  # message");
        wln!(self, "");
        wln!(self, "    # Calculate string length");
        wln!(self, "    xorq %r8, %r8  # length counter");
        wln!(self, "panic_len:");
        wln!(self, "    cmpb $0, (%r12,%r8)");
        wln!(self, "    je panic_done");
        wln!(self, "    incq %r8");
        wln!(self, "    jmp panic_len");
        wln!(self, "panic_done:");
        wln!(self, "");
        wln!(self, "    # write(2, message, length)  — stderr");
        wln!(self, "    movq $1, %rax  # syscall write");
        wln!(self, "    movq $2, %rdi  # stderr");
        wln!(self, "    movq %r12, %rsi  # buffer");
        wln!(self, "    movq %r8, %rdx  # length");
        wln!(self, "    syscall");
        wln!(self, "");
        wln!(self, "    # exit(1)");
        wln!(self, "    movq $60, %rax  # syscall exit");
        wln!(self, "    movq $1, %rdi  # exit code");
        wln!(self, "    syscall");
        wln!(self, "    ud2  # should not reach here");

        // p_malloc(rdi = size) -> rax = ptr
        // Uses mmap to allocate memory
        wln!(self, "");
        wln!(self, "p_malloc:");
        wln!(self, "    pushq %rbp");
        wln!(self, "    movq %rsp, %rbp");
        wln!(self, "    # mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)");
        wln!(self, "    movq %rdi, %rsi  # size");
        wln!(self, "    movq $9, %rax  # syscall mmap");
        wln!(self, "    xorq %rdi, %rdi  # addr = NULL");
        wln!(self, "    movq $3, %rdx  # prot = PROT_READ|PROT_WRITE");
        wln!(self, "    movq $0x22, %r10  # flags = MAP_PRIVATE|MAP_ANONYMOUS");
        wln!(self, "    movq $-1, %r8  # fd = -1");
        wln!(self, "    xorq %r9, %r9  # offset = 0");
        wln!(self, "    syscall");
        wln!(self, "    movq %rbp, %rsp");
        wln!(self, "    popq %rbp");
        wln!(self, "    ret");

        // p_free(rdi = ptr) — free mmap allocation
        // Simplified: no-op (OS reclaims on exit). For proper free, need munmap.
        wln!(self, "");
        wln!(self, "p_free:");
        wln!(self, "    pushq %rbp");
        wln!(self, "    movq %rsp, %rbp");
        wln!(self, "    # munmap(ptr, 0) — simplified, OS reclaims on exit");
        wln!(self, "    # In production, we'd track allocation sizes");
        wln!(self, "    movq %rbp, %rsp");
        wln!(self, "    popq %rbp");
        wln!(self, "    ret");

        // _start — Linux ELF entry point
        // At entry, RSP points to argc. Stack is 16-byte aligned.
        // We call main() then exit with its return value.
        wln!(self, "");
        wln!(self, "    .globl _start");
        wln!(self, "    .type _start, @function");
        wln!(self, "_start:");
        wln!(self, "    # Align stack to 16 bytes (RSP is already aligned at entry)");
        wln!(self, "    # Call main()");
        wln!(self, "    call main");
        wln!(self, "    # exit(rax) — main's return value");
        wln!(self, "    movq %rax, %rdi  # exit code from main");
        wln!(self, "    movq $60, %rax  # syscall exit");
        wln!(self, "    syscall");
        wln!(self, "    ud2  # should not reach here");
        wln!(self, "    .size _start, .-_start");
    }

}

/// Format a stack offset for AT&T memory operands (e.g., -8 -> "-8", 0 -> "", 48 -> "48")
fn fmt_offset(offset: i64) -> String {
    if offset == 0 {
        String::new()
    } else {
        format!("{}", offset)
    }
}

/// Format a hex value (unused on Linux but kept for compatibility)
fn fmt_hex(val: u32) -> String {
    format!("0x{:x}", val)
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

/// Mangle function names to avoid conflicts with x86 mnemonics and GAS reserved words.
/// "main", "_start", and names starting with "__pledge" or "p_" are kept as-is.
fn mangle_name(name: &str) -> String {
    // Don't mangle main, _start, or runtime stubs
    if name == "main" || name == "_start" || name.starts_with("__pledge") || name.starts_with("p_") {
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
