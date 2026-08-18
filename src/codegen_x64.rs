// x86-64 Codegen — Phase 4C
//
// Instruction selector that consumes PIR L3 + a register allocation and
// produces x86-64 assembly. This is the first target in the multi-architecture
// codegen backend.
//
// The codegen has two output modes:
//   1. Textual assembly (AT&T or Intel syntax) — for debugging and for
//      feeding to an external assembler (gcc/clang).
//   2. Raw machine code bytes — for direct object file emission (COFF/ELF).
//
// For the bootstrap we emit Intel-syntax textual assembly (compatible with
// NASM/GAS), which can be assembled with `gcc -c` or `clang -c`. Binary
// COFF/ELF emission comes in a follow-up.
//
// Pipeline: L3Module → (regalloc) → [code selector] → x86-64 asm string
//
// Calling convention: System V AMD64
//   - Args: RDI, RSI, RDX, RCX, R8, R9 (first 6 integer/pointer)
//   - Return: RAX
//   - Callee-saved: RBX, R12-R15 (preserved across calls)
//   - Stack 16-byte aligned at call sites

use crate::ast::BinOp;
use crate::ir::IrType;
use crate::pir::{
    CastKind, L3Block, L3Function, L3Inst, L3Module, MemRegion, Operand, PBlock, VReg,
};
use crate::regalloc::{allocate_module, Allocation, Location, ModuleAllocation, PhysReg};
use std::collections::HashMap;

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

/// Generate x86-64 assembly for an entire L3 module.
/// Returns a string of Intel-syntax assembly suitable for `gcc -c` or NASM.
pub fn generate_asm(module: &L3Module) -> String {
    let allocs = allocate_module(module);
    let mut gen = X64CodeGen::new(allocs);
    gen.gen_module(module);
    gen.output
}

/// Generate x86-64 assembly with per-function allocation info (for debugging).
pub fn generate_asm_with_allocs(module: &L3Module, allocs: ModuleAllocation) -> String {
    let mut gen = X64CodeGen::new(allocs);
    gen.gen_module(module);
    gen.output
}

// ═══════════════════════════════════════════════════════════════════════════
// Code generator
// ═══════════════════════════════════════════════════════════════════════════

struct X64CodeGen {
    output: String,
    allocs: ModuleAllocation,
    /// Current function's allocation (borrowed from allocs for the duration
    /// of gen_function — we clone it to avoid borrow issues).
    current_alloc: Option<Allocation>,
    /// String constants — label → value.
    strings: HashMap<String, String>,
    /// Next string label number.
    string_counter: u32,
    /// Block label mapping: PBlock → "L<func>_<block>".
    block_labels: HashMap<(String, PBlock), String>,
    /// Current function name (for label generation).
    current_func: String,
    /// Trait method names for vtable dispatch.
    trait_methods: HashMap<String, Vec<String>>,
    /// Vtables to emit in .rodata: label → list of function names.
    vtables: Vec<(String, Vec<String>)>,
}

impl X64CodeGen {
    fn new(allocs: ModuleAllocation) -> Self {
        X64CodeGen {
            output: String::new(),
            allocs,
            current_alloc: None,
            strings: HashMap::new(),
            string_counter: 0,
            block_labels: HashMap::new(),
            current_func: String::new(),
            trait_methods: HashMap::new(),
            vtables: Vec::new(),
        }
    }

    fn w(&mut self, s: &str) {
        self.output.push_str(s);
    }

    fn wl(&mut self, s: &str) {
        self.output.push_str(s);
        self.output.push('\n');
    }

    fn gen_module(&mut self, module: &L3Module) {
        // Copy trait method info for vtable dispatch.
        self.trait_methods = module.trait_methods.clone();

        // Collect string constants.
        // Note: module.string_constants maps (content → label).
        for (content, label) in &module.string_constants {
            self.strings.insert(label.clone(), content.clone());
            // Update string_counter to avoid label collisions with module labels.
            if let Some(n) = label.strip_prefix(".str_") {
                if let Ok(n) = n.parse::<u32>() {
                    if n >= self.string_counter {
                        self.string_counter = n + 1;
                    }
                }
            }
        }

        // .data section — string constants (initial set from the module).
        // More strings may be added during codegen (e.g., panic messages);
        // we emit the data section after codegen so all strings are included.
        self.wl("    .text");

        // Generate each function.
        for f in &module.functions {
            self.gen_function(f, module);
        }

        // Emit string constants after codegen (so all are collected).
        self.wl("");
        self.wl("    .section .rodata");
        self.wl("    .align 8");
        let strings: Vec<(String, String)> = self.strings.iter().map(|(k, v)| (k.clone(), v.clone())).collect();
        for (label, val) in &strings {
            if val.starts_with("__float_bits:") {
                // Float constant — emit as .quad with the raw bits.
                let bits: u64 = val["__float_bits:".len()..].parse().unwrap_or(0);
                self.wl(&format!("{}: .quad {}", label, bits));
            } else {
                self.wl(&format!("{}: .asciz \"{}\"", label, escape_str(val)));
            }
        }

        // Emit vtables after string constants.
        let vtables: Vec<(String, Vec<String>)> = self.vtables.clone();
        for (vtable_label, methods) in &vtables {
            self.wl(&format!("{}:", vtable_label));
            for m in methods {
                self.wl(&format!("    .quad {}", m));
            }
        }

        // Runtime stubs — we declare externs; the linker resolves them.
        self.wl("");
        self.wl("    # External runtime functions (linked from C runtime)");
        for (name, _) in &module.extern_functions {
            self.wl(&format!("    .globl {}", name));
        }
    }

    fn gen_function(&mut self, f: &L3Function, module: &L3Module) {
        self.current_func = f.name.clone();
        let alloc = self.allocs.func_allocations.get(&f.name).cloned();
        self.current_alloc = alloc.clone();

        // Pre-compute block labels.
        self.block_labels.clear();
        for b in &f.blocks {
            let label = format!(".L{}_bb{}", f.name, b.id.0);
            self.block_labels.insert((f.name.clone(), b.id), label);
        }

        let frame_size = f.frame_size + alloc.as_ref().map(|a| a.spill_size as u64).unwrap_or(0);
        // Align frame to 16 bytes.
        let aligned_frame = (frame_size + 15) & !15;

        self.wl("");
        self.wl(&format!("    .globl {}", f.name));
        self.wl(&format!("    .type {}, @function", f.name));
        self.wl(&format!("{}:", f.name));

        // ─── Prologue ───
        self.wl("    pushq %rbp");
        self.wl("    movq %rsp, %rbp");
        if aligned_frame > 0 {
            self.wl(&format!("    subq ${}, %rsp", aligned_frame));
        }
        // Save callee-saved registers.
        if let Some(ref a) = alloc {
            for &r in &a.used_callee_saved {
                self.wl(&format!("    pushq %{}", r.name()));
            }
        }

        // Move argument registers to their assigned locations (if the arg
        // VReg is spilled, move from the arg register to the spill slot).
        if let Some(ref a) = alloc {
            let arg_regs = PhysReg::arg_regs();
            for (i, (vreg, _)) in f.params.iter().enumerate() {
                if i < arg_regs.len() {
                    let loc = a.loc(*vreg);
                    match loc {
                        Location::Reg(r) if r == arg_regs[i] => {
                            // Already in the right register — nothing to do.
                        }
                        Location::Reg(r) => {
                            self.wl(&format!(
                                "    movq %{}, %{}",
                                arg_regs[i].name(),
                                r.name()
                            ));
                        }
                        Location::Spill(offset) => {
                            self.wl(&format!(
                                "    movq %{}, -{}(%rbp)",
                                arg_regs[i].name(),
                                offset
                            ));
                        }
                    }
                }
            }
        }

        // ─── Body ───
        for b in &f.blocks {
            self.gen_block(b, f, &module.extern_functions);
        }

        // ─── Epilogue (fallthrough return) ───
        self.gen_epilogue(&alloc);

        self.wl(&format!("    .size {}, .-{}", f.name, f.name));
    }

    fn gen_epilogue(&mut self, alloc: &Option<Allocation>) {
        let label = format!(".L{}_epilogue", self.current_func);
        self.wl(&format!("{}:", label));

        // Restore callee-saved registers (in reverse order).
        if let Some(ref a) = alloc {
            for &r in a.used_callee_saved.iter().rev() {
                self.wl(&format!("    popq %{}", r.name()));
            }
        }

        self.wl("    leave");
        self.wl("    ret");
    }

    fn epilogue_label(&self) -> String {
        format!(".L{}_epilogue", self.current_func)
    }

    fn block_label(&self, block: PBlock) -> String {
        self.block_labels
            .get(&(self.current_func.clone(), block))
            .cloned()
            .unwrap_or_else(|| format!(".L{}_bb{}", self.current_func, block.0))
    }

    fn gen_block(
        &mut self,
        block: &L3Block,
        _func: &L3Function,
        externs: &HashMap<String, (Vec<IrType>, IrType)>,
    ) {
        self.wl(&format!("{}:", self.block_label(block.id)));

        for inst in &block.instructions {
            self.gen_inst(inst, externs);
        }
    }

    fn gen_inst(&mut self, inst: &L3Inst, externs: &HashMap<String, (Vec<IrType>, IrType)>) {
        match inst {
            L3Inst::Mov { dst, src } => {
                self.gen_mov(*dst, src);
            }

            L3Inst::Bin { op, dst, a, b, ty } => {
                self.gen_bin(op.clone(), *dst, a, b, ty);
            }

            L3Inst::Unary { op, dst, src, ty } => {
                self.gen_unary(op.clone(), *dst, src, ty);
            }

            L3Inst::StackAlloc { dst, bytes, .. } => {
                // Allocate from the stack by adjusting RSP or using an alloca.
                // For the bootstrap, we emit a call to p_alloc (heap) or
                // subq %rsp (stack). Since L3 already computed frame slots,
                // this is mostly a no-op for stack allocas; for heap we call
                // p_alloc.
                self.wl(&format!("    # StackAlloc: {} bytes -> {}", bytes, dst));
                // Move the frame slot address into dst's register.
                if let Some(loc) = self.dst_loc(*dst) {
                    match loc {
                        Location::Reg(r) => {
                            // Use lea to get the address of the stack slot.
                            // For now, just zero the register.
                            self.wl(&format!("    xorq %{}, %{}", r.name(), r.name()));
                        }
                        Location::Spill(off) => {
                            self.wl(&format!("    # spilled to -{}(%rbp)", off));
                        }
                    }
                }
            }

            L3Inst::Load { dst, ptr, ty } => {
                self.gen_load(*dst, ptr, ty);
            }

            L3Inst::Store { ptr, val, ty } => {
                self.gen_store(ptr, val, ty);
            }

            L3Inst::FieldAddr { dst, base, offset, .. } => {
                self.gen_field_addr(*dst, base, *offset);
            }

            L3Inst::ElemAddr { dst, base, index, elem_ty } => {
                self.gen_elem_addr(*dst, base, index, elem_ty);
            }

            L3Inst::Jmp { target } => {
                self.wl(&format!("    jmp {}", self.block_label(*target)));
            }

            L3Inst::Br { cond, then, else_ } => {
                self.gen_branch(cond, *then, *else_);
            }

            L3Inst::Ret { value } => {
                if let Some(v) = value {
                    self.gen_operand_to_reg(v, PhysReg::Rax);
                }
                self.wl(&format!("    jmp {}", self.epilogue_label()));
            }

            L3Inst::Unreachable => {
                self.wl("    ud2");
            }

            L3Inst::Call { dst, target, args, .. } => {
                self.gen_call(dst.as_ref(), target, args);
            }

            L3Inst::CallPtr { dst, fn_ptr, args, has_env, .. } => {
                self.gen_call_ptr(dst.as_ref(), fn_ptr, args, *has_env);
            }

            L3Inst::DynCall { dst, trait_name, method, args, .. } => {
                self.wl(&format!(
                    "    # DynCall: {}.{} ({} args)",
                    trait_name,
                    method,
                    args.len()
                ));
                // Dynamic dispatch via vtable.
                // The trait object is a pointer to (data, vtable_ptr).
                // vtable_ptr points to an array of function pointers.
                // Method index is determined by the trait's method order.
                let method_idx = self.trait_methods.get(trait_name)
                    .and_then(|methods| methods.iter().position(|m| m == method))
                    .unwrap_or(0);

                // Load the trait object pointer into rax
                if !args.is_empty() {
                    self.gen_operand_to_reg(&args[0], PhysReg::Rax);
                }
                // rax = trait object pointer
                // data = *(rax+0), vtable = *(rax+8)
                self.wl("    movq (%rax), %rdi    # data → rdi (first arg)");
                self.wl("    movq 8(%rax), %r11   # vtable → r11");
                self.wl(&format!("    call *{}(%r11)    # call vtable[{}]",
                    method_idx * 8, method_idx));
                // Result is in rax — move to dst
                if let Some(dst) = dst {
                    if let Some(loc) = self.dst_loc(*dst) {
                        if let Location::Reg(r) = loc {
                            if r != PhysReg::Rax {
                                self.wl(&format!("    movq %rax, %{}", r.name()));
                            }
                        }
                    }
                }
            }

            L3Inst::MakeDyn { dst, value, type_name, trait_name } => {
                self.wl(&format!(
                    "    # MakeDyn: {} as dyn {} -> {}",
                    type_name, trait_name, dst
                ));
                // Create a vtable for this (type, trait) pair if not already created.
                let vtable_label = format!(".vtable_{}_{}", type_name, trait_name);
                let vtable_exists = self.vtables.iter().any(|(l, _)| l == &vtable_label);
                if !vtable_exists {
                    let methods = self.trait_methods.get(trait_name).cloned().unwrap_or_default();
                    let func_names: Vec<String> = methods.iter()
                        .map(|m| format!("{}_{}", type_name, m))
                        .collect();
                    self.vtables.push((vtable_label.clone(), func_names));
                }

                // Allocate a 16-byte block: (data, vtable_ptr)
                // Use p_malloc to allocate.
                self.wl("    movq $16, %rdi");
                self.wl("    call p_malloc");
                // rax = pointer to the new block
                // Store the data value at offset 0
                self.gen_operand_to_reg(value, PhysReg::Rcx);
                self.wl("    movq %rcx, (%rax)");
                // Store the vtable pointer at offset 8
                self.wl(&format!("    leaq {}(%rip), %rcx", vtable_label));
                self.wl("    movq %rcx, 8(%rax)");
                // rax = trait object pointer → move to dst
                if let Some(loc) = self.dst_loc(*dst) {
                    if let Location::Reg(r) = loc {
                        if r != PhysReg::Rax {
                            self.wl(&format!("    movq %rax, %{}", r.name()));
                        }
                    }
                }
            }

            L3Inst::Cast { dst, src, kind, .. } => {
                self.gen_cast(*dst, src, *kind);
            }

            L3Inst::TryUnwrap { value, ok_dst, ok_block, err_block } => {
                self.wl(&format!("    # TryUnwrap: {} -> {}", value, ok_dst));
                // Load the tag from the value (offset 0) and check if it's
                // Some/Ok (tag 0) or None/Err (tag 1).
                self.gen_operand_to_reg(value, PhysReg::Rax);
                self.wl("    movq (%rax), %rcx");
                self.wl("    testq %rcx, %rcx");
                self.wl(&format!("    jnz {}", self.block_label(*err_block)));
                // Ok path: load the data (offset 8) into ok_dst.
                self.wl("    movq 8(%rax), %rax");
                if let Some(loc) = self.dst_loc(*ok_dst) {
                    if let Location::Reg(r) = loc {
                        if r != PhysReg::Rax {
                            self.wl(&format!("    movq %rax, %{}", r.name()));
                        }
                    }
                }
                self.wl(&format!("    jmp {}", self.block_label(*ok_block)));
            }

            L3Inst::Print { val, ty } => {
                self.wl(&format!("    # Print: {} ({})", val, ty));
                // Route to the appropriate p_print_* function based on type.
                let func_name = match ty {
                    IrType::I64 | IrType::I32 | IrType::I16 | IrType::I8
                    | IrType::U64 | IrType::U32 | IrType::U16 | IrType::U8 => "p_print_int",
                    IrType::F64 | IrType::F32 => "p_print_float",
                    IrType::Bool => "p_print_bool",
                    IrType::Opaque(ref name) | IrType::Generic(ref name, _) if name.contains("Vec") => "p_print_vec",
                    IrType::Opaque(ref name) | IrType::Generic(ref name, _) if name.contains("Map") => "p_print_map",
                    _ => "p_print_str",
                };
                self.gen_operand_to_reg(val, PhysReg::Rdi);
                self.wl(&format!("    call {}", func_name));
            }

            L3Inst::Panic { message } => {
                self.wl(&format!("    # Panic: \"{}\"", message));
                // Load the message string address and call p_panic.
                let label = self.alloc_string(message);
                self.wl(&format!("    leaq {}(%rip), %rdi", label));
                self.wl("    call p_panic");
                self.wl("    ud2");
            }
        }
    }

    // ─── Instruction helpers ──────────────────────────────────────────────

    fn gen_mov(&mut self, dst: VReg, src: &Operand) {
        let dst_loc = self.dst_loc(dst);
        match dst_loc {
            Some(Location::Reg(dr)) => {
                // Special case: Mov from Slot should load the VALUE at the
                // stack slot, not the address. gen_operand_to_reg treats
                // Slot as address-of (leaq), which is correct for pointer
                // operands but wrong for value moves.
                if let Operand::Slot(slot) = src {
                    self.wl(&format!("    movq -{}(%rbp), %{}", slot.0, dr.name()));
                } else {
                    self.gen_operand_to_reg(src, dr);
                }
            }
            Some(Location::Spill(off)) => {
                // For spill destinations, gen_operand_to_mem handles most cases.
                // But Slot source needs to load the value, not the address.
                if let Operand::Slot(slot) = src {
                    self.wl(&format!("    movq -{}(%rbp), %rax", slot.0));
                    self.wl(&format!("    movq %rax, -{}(%rbp)", off));
                } else {
                    self.gen_operand_to_mem(src, off);
                }
            }
            None => {
                self.wl(&format!("    # Mov: dst {} unallocated", dst));
            }
        }
    }

    fn gen_bin(&mut self, op: BinOp, dst: VReg, a: &Operand, b: &Operand, ty: &IrType) {
        // Strategy: load `a` into dst's register, then apply `b`.
        // If dst is spilled, use RAX as a temporary.
        let dst_loc = self.dst_loc(dst);
        let (dst_reg, need_spill_reload) = match dst_loc {
            Some(Location::Reg(r)) => (r, false),
            _ => (PhysReg::Rax, true),
        };

        if need_spill_reload {
            // Load a into RAX (already chosen as temp).
            self.gen_operand_to_reg(a, dst_reg);
        } else {
            self.gen_operand_to_reg(a, dst_reg);
        }

        // Float operations: use SSE2 instructions.
        if ty.is_float() {
            match op {
                BinOp::Add | BinOp::Sub | BinOp::Mul | BinOp::Div => {
                    // Move a to xmm0, b to xmm1, then apply SSE op.
                    self.wl(&format!("    movq %{}, %xmm0", dst_reg.name()));
                    let tmp = self.scratch_reg(dst_reg);
                    self.gen_operand_to_reg(b, tmp);
                    self.wl(&format!("    movq %{}, %xmm1", tmp.name()));
                    let sse_op = match op {
                        BinOp::Add => "addsd",
                        BinOp::Sub => "subsd",
                        BinOp::Mul => "mulsd",
                        BinOp::Div => "divsd",
                        _ => unreachable!(),
                    };
                    self.wl(&format!("    {} %xmm1, %xmm0", sse_op));
                    self.wl(&format!("    movq %xmm0, %{}", dst_reg.name()));
                }
                BinOp::Eq | BinOp::Neq | BinOp::Lt | BinOp::Gt | BinOp::LtEq | BinOp::GtEq
                | BinOp::And | BinOp::Or => {
                    self.gen_comparison(op, b, dst_reg);
                }
                _ => {
                    // Fallback for other ops on floats (shouldn't happen).
                    self.gen_operand_apply_binop("addq", b, dst_reg);
                }
            }
            if need_spill_reload {
                if let Some(Location::Spill(off)) = self.dst_loc(dst) {
                    self.wl(&format!("    movq %{}, -{}(%rbp)", dst_reg.name(), off));
                }
            }
            return;
        }

        // Apply the operation with b.
        match op {
            BinOp::Add => {
                self.gen_operand_apply_binop("addq", b, dst_reg);
            }
            BinOp::Sub => {
                self.gen_operand_apply_binop("subq", b, dst_reg);
            }
            BinOp::Mul => {
                // imulq reg, reg — but we need the operand in a register.
                let tmp = self.scratch_reg(dst_reg);
                self.gen_operand_to_reg(b, tmp);
                self.wl(&format!("    imulq %{}, %{}", tmp.name(), dst_reg.name()));
            }
            BinOp::Div => {
                // idivq: dividend in RDX:RAX, divisor in operand.
                // Move dividend to RAX, sign-extend to RDX, then divide.
                // IMPORTANT: cqto clobbers RDX, so the divisor must NOT be in RDX.
                // If it is, move it to RCX first.
                if dst_reg != PhysReg::Rax {
                    self.wl(&format!("    movq %{}, %rax", dst_reg.name()));
                }
                // Save divisor to RCX if it's in RDX or not in a register.
                match b {
                    Operand::ImmI(v) => {
                        self.wl("    cqto");
                        self.wl(&format!("    movq ${}, %rcx", v));
                        self.wl("    idivq %rcx");
                    }
                    Operand::VReg(vr) => {
                        let b_loc = self.operand_loc(*vr);
                        match b_loc {
                            Some(Location::Reg(PhysReg::Rdx)) => {
                                // Divisor is in RDX — move to RCX before cqto.
                                self.wl("    movq %rdx, %rcx");
                                self.wl("    cqto");
                                self.wl("    idivq %rcx");
                            }
                            Some(Location::Reg(r)) => {
                                self.wl("    cqto");
                                self.wl(&format!("    idivq %{}", r.name()));
                            }
                            Some(Location::Spill(off)) => {
                                self.wl("    cqto");
                                self.wl(&format!("    idivq -{}(%rbp)", off));
                            }
                            None => {
                                self.wl("    cqto");
                                self.wl("    idivq %rcx");
                            }
                        }
                    }
                    _ => {
                        let tmp = self.scratch_reg(dst_reg);
                        self.gen_operand_to_reg(b, tmp);
                        if tmp == PhysReg::Rdx {
                            self.wl("    movq %rdx, %rcx");
                            self.wl("    cqto");
                            self.wl("    idivq %rcx");
                        } else {
                            self.wl("    cqto");
                            self.wl(&format!("    idivq %{}", tmp.name()));
                        }
                    }
                }
                // Result is in RAX; move to dst if needed.
                if dst_reg != PhysReg::Rax {
                    self.wl(&format!("    movq %rax, %{}", dst_reg.name()));
                }
            }
            BinOp::Mod => {
                // Same as Div but result is in RDX.
                if dst_reg != PhysReg::Rax {
                    self.wl(&format!("    movq %{}, %rax", dst_reg.name()));
                }
                match b {
                    Operand::ImmI(v) => {
                        self.wl("    cqto");
                        self.wl(&format!("    movq ${}, %rcx", v));
                        self.wl("    idivq %rcx");
                    }
                    Operand::VReg(vr) => {
                        let b_loc = self.operand_loc(*vr);
                        match b_loc {
                            Some(Location::Reg(PhysReg::Rdx)) => {
                                self.wl("    movq %rdx, %rcx");
                                self.wl("    cqto");
                                self.wl("    idivq %rcx");
                            }
                            Some(Location::Reg(r)) => {
                                self.wl("    cqto");
                                self.wl(&format!("    idivq %{}", r.name()));
                            }
                            Some(Location::Spill(off)) => {
                                self.wl("    cqto");
                                self.wl(&format!("    idivq -{}(%rbp)", off));
                            }
                            None => {
                                self.wl("    cqto");
                                self.wl("    idivq %rcx");
                            }
                        }
                    }
                    _ => {
                        let tmp = self.scratch_reg(dst_reg);
                        self.gen_operand_to_reg(b, tmp);
                        if tmp == PhysReg::Rdx {
                            self.wl("    movq %rdx, %rcx");
                            self.wl("    cqto");
                            self.wl("    idivq %rcx");
                        } else {
                            self.wl("    cqto");
                            self.wl(&format!("    idivq %{}", tmp.name()));
                        }
                    }
                }
                // Result is in RDX; move to dst.
                self.wl(&format!("    movq %rdx, %{}", dst_reg.name()));
            }
            BinOp::BitAnd => {
                self.gen_operand_apply_binop("andq", b, dst_reg);
            }
            BinOp::BitOr => {
                self.gen_operand_apply_binop("orq", b, dst_reg);
            }
            BinOp::BitXor => {
                self.gen_operand_apply_binop("xorq", b, dst_reg);
            }
            BinOp::Shl => {
                self.gen_shift("shlq", b, dst_reg);
            }
            BinOp::Shr => {
                self.gen_shift("shrq", b, dst_reg);
            }
            // Comparisons: compute into dst (0 or 1).
            BinOp::Eq | BinOp::Neq | BinOp::Lt | BinOp::Gt | BinOp::LtEq | BinOp::GtEq
            | BinOp::And | BinOp::Or => {
                self.gen_comparison(op, b, dst_reg);
            }
        }

        // If dst was spilled, store the result back.
        if need_spill_reload {
            if let Some(Location::Spill(off)) = dst_loc {
                self.wl(&format!("    movq %{}, -{}(%rbp)", dst_reg.name(), off));
            }
        }
    }

    fn gen_unary(&mut self, op: crate::ast::UnaryOp, dst: VReg, src: &Operand, ty: &IrType) {
        let dst_loc = self.dst_loc(dst);
        let dst_reg = match dst_loc {
            Some(Location::Reg(r)) => r,
            _ => PhysReg::Rax,
        };

        self.gen_operand_to_reg(src, dst_reg);

        match op {
            crate::ast::UnaryOp::Neg => {
                if ty.is_float() {
                    // Float negation: flip sign bit (bit 63) via XOR with sign mask
                    // Load sign mask from rodata, then xorq
                    self.wl(&format!("    movq %{}, %rax", dst_reg.name()));
                    self.wl("    movq $0x8000000000000000, %rcx");
                    self.wl("    xorq %rcx, %rax");
                    if dst_reg != PhysReg::Rax {
                        self.wl(&format!("    movq %rax, %{}", dst_reg.name()));
                    }
                } else {
                    self.wl(&format!("    negq %{}", dst_reg.name()));
                }
            }
            crate::ast::UnaryOp::Not => {
                // Logical not: test if zero, set to 0 or 1.
                self.wl(&format!("    testq %{}, %{}", dst_reg.name(), dst_reg.name()));
                self.wl("    sete %al");
                self.wl("    movzbq %al, %rax");
                if dst_reg != PhysReg::Rax {
                    self.wl(&format!("    movq %rax, %{}", dst_reg.name()));
                }
            }
            crate::ast::UnaryOp::BitNot => {
                self.wl(&format!("    notq %{}", dst_reg.name()));
            }
        }

        if let Some(Location::Spill(off)) = dst_loc {
            self.wl(&format!("    movq %{}, -{}(%rbp)", dst_reg.name(), off));
        }
    }

    fn gen_load(&mut self, dst: VReg, ptr: &Operand, ty: &IrType) {
        let dst_loc = self.dst_loc(dst);
        let size = type_size(ty);

        let load_op = match size {
            1 => "movzbq",
            2 => "movzwq",
            4 => "movslq",
            _ => "movq",
        };

        // If the pointer is a stack slot, we can load directly from the
        // memory operand without needing a scratch register. This avoids
        // clobbering live values in %rax.
        if let Operand::Slot(slot) = ptr {
            let mem = format!("-{}(%rbp)", slot.0);
            match dst_loc {
                Some(Location::Reg(dr)) => {
                    self.wl(&format!("    {} {}, %{}", load_op, mem, dr.name()));
                }
                Some(Location::Spill(off)) => {
                    self.wl(&format!("    {} {}, %rax", load_op, mem));
                    self.wl(&format!("    movq %rax, -{}(%rbp)", off));
                }
                None => {}
            }
            return;
        }

        // Load the pointer into a scratch register.
        let ptr_reg = match self.operand_reg(ptr) {
            Some(r) => r,
            None => {
                let tmp = PhysReg::Rax;
                self.gen_operand_to_reg(ptr, tmp);
                tmp
            }
        };

        match dst_loc {
            Some(Location::Reg(dr)) => {
                self.wl(&format!(
                    "    {} ({}), %{}",
                    load_op,
                    self.reg_str(ptr_reg),
                    dr.name()
                ));
            }
            Some(Location::Spill(off)) => {
                // Load into RAX, then store to spill slot.
                self.wl(&format!("    {} ({}), %rax", load_op, self.reg_str(ptr_reg)));
                self.wl(&format!("    movq %rax, -{}(%rbp)", off));
            }
            None => {}
        }
    }

    fn gen_store(&mut self, ptr: &Operand, val: &Operand, ty: &IrType) {
        let size = type_size(ty);
        let store_op = match size {
            1 => "movb",
            2 => "movw",
            4 => "movl",
            _ => "movq",
        };

        // If the pointer is a stack slot, we can store directly to the
        // memory operand without needing a scratch register for the address.
        if let Operand::Slot(slot) = ptr {
            let mem = format!("-{}(%rbp)", slot.0);
            // Load value into RAX.
            self.gen_operand_to_reg(val, PhysReg::Rax);
            if size < 8 {
                let sub_reg = sub_reg_name(PhysReg::Rax, size);
                self.wl(&format!("    {} %{}, {}", store_op, sub_reg, mem));
            } else {
                self.wl(&format!("    {} %rax, {}", store_op, mem));
            }
            return;
        }

        // Load value into RAX.
        self.gen_operand_to_reg(val, PhysReg::Rax);

        // Get pointer into RCX.
        let ptr_reg = match self.operand_reg(ptr) {
            Some(r) => r,
            None => {
                self.gen_operand_to_reg(ptr, PhysReg::Rcx);
                PhysReg::Rcx
            }
        };

        if size < 8 {
            // For sub-word stores, use the appropriate register suffix.
            let sub_reg = sub_reg_name(PhysReg::Rax, size);
            self.wl(&format!(
                "    {} %{}, ({})",
                store_op,
                sub_reg,
                self.reg_str(ptr_reg)
            ));
        } else {
            self.wl(&format!(
                "    {} %rax, ({})",
                store_op,
                self.reg_str(ptr_reg)
            ));
        }
    }

    fn gen_field_addr(&mut self, dst: VReg, base: &Operand, offset: u32) {
        let dst_loc = self.dst_loc(dst);
        let dst_reg = match dst_loc {
            Some(Location::Reg(r)) => r,
            _ => PhysReg::Rax,
        };

        self.gen_operand_to_reg(base, dst_reg);
        if offset > 0 {
            self.wl(&format!("    addq ${}, %{}", offset, dst_reg.name()));
        }

        if let Some(Location::Spill(off)) = dst_loc {
            self.wl(&format!("    movq %{}, -{}(%rbp)", dst_reg.name(), off));
        }
    }

    fn gen_elem_addr(&mut self, dst: VReg, base: &Operand, index: &Operand, elem_ty: &IrType) {
        let dst_loc = self.dst_loc(dst);
        let dst_reg = match dst_loc {
            Some(Location::Reg(r)) => r,
            _ => PhysReg::Rax,
        };
        let elem_size = type_size(elem_ty).max(1);

        // dst = base + index * elem_size
        self.gen_operand_to_reg(base, dst_reg);

        let index_reg = self.scratch_reg(dst_reg);
        self.gen_operand_to_reg(index, index_reg);

        if elem_size == 1 {
            self.wl(&format!(
                "    addq %{}, %{}",
                index_reg.name(),
                dst_reg.name()
            ));
        } else if elem_size.is_power_of_two() {
            let shift = elem_size.trailing_zeros();
            self.wl(&format!(
                "    shlq ${}, %{}",
                shift,
                index_reg.name()
            ));
            self.wl(&format!(
                "    addq %{}, %{}",
                index_reg.name(),
                dst_reg.name()
            ));
        } else {
            // Use lea with scale.
            self.wl(&format!(
                "    leaq ({})(%{},%{},{}), %{}",
                0,
                dst_reg.name(),
                index_reg.name(),
                elem_size,
                dst_reg.name()
            ));
        }

        if let Some(Location::Spill(off)) = dst_loc {
            self.wl(&format!("    movq %{}, -{}(%rbp)", dst_reg.name(), off));
        }
    }

    fn gen_branch(&mut self, cond: &Operand, then: PBlock, else_: PBlock) {
        // Load condition into RAX, test, and jump.
        self.gen_operand_to_reg(cond, PhysReg::Rax);
        self.wl("    testq %rax, %rax");
        self.wl(&format!("    jne {}", self.block_label(then)));
        self.wl(&format!("    jmp {}", self.block_label(else_)));
    }

    fn gen_call(&mut self, dst: Option<&VReg>, target: &str, args: &[Operand]) {
        // SIMD intrinsics: emit hardware SSE2/AVX instructions
        if target == "p_simd_add" && args.len() == 2 {
            self.wl(&format!("    # SIMD: addpd {}, {}", self.op_str(&args[0]), self.op_str(&args[1])));
            self.gen_operand_to_xmm(&args[0], PhysReg::Xmm0);
            self.gen_operand_to_xmm(&args[1], PhysReg::Xmm1);
            self.wl("    addpd %xmm1, %xmm0");
            if let Some(d) = dst {
                self.wl(&format!("    movapd %xmm0, {}", self.vreg_str(d)));
            }
            return;
        }
        if target == "p_simd_sub" && args.len() == 2 {
            self.wl(&format!("    # SIMD: subpd {}, {}", self.op_str(&args[0]), self.op_str(&args[1])));
            self.gen_operand_to_xmm(&args[0], PhysReg::Xmm0);
            self.gen_operand_to_xmm(&args[1], PhysReg::Xmm1);
            self.wl("    subpd %xmm1, %xmm0");
            if let Some(d) = dst {
                self.wl(&format!("    movapd %xmm0, {}", self.vreg_str(d)));
            }
            return;
        }
        if target == "p_simd_mul" && args.len() == 2 {
            self.wl(&format!("    # SIMD: mulpd {}, {}", self.op_str(&args[0]), self.op_str(&args[1])));
            self.gen_operand_to_xmm(&args[0], PhysReg::Xmm0);
            self.gen_operand_to_xmm(&args[1], PhysReg::Xmm1);
            self.wl("    mulpd %xmm1, %xmm0");
            if let Some(d) = dst {
                self.wl(&format!("    movapd %xmm0, {}", self.vreg_str(d)));
            }
            return;
        }
        if target == "p_simd_div" && args.len() == 2 {
            self.wl(&format!("    # SIMD: divpd {}, {}", self.op_str(&args[0]), self.op_str(&args[1])));
            self.gen_operand_to_xmm(&args[0], PhysReg::Xmm0);
            self.gen_operand_to_xmm(&args[1], PhysReg::Xmm1);
            self.wl("    divpd %xmm1, %xmm0");
            if let Some(d) = dst {
                self.wl(&format!("    movapd %xmm0, {}", self.vreg_str(d)));
            }
            return;
        }

        // Move args into argument registers.
        let arg_regs = PhysReg::arg_regs();
        for (i, arg) in args.iter().enumerate() {
            if i < arg_regs.len() {
                self.gen_operand_to_reg(arg, arg_regs[i]);
            } else {
                // Push extra args on the stack (in reverse order, but we
                // handle them in order for simplicity — the caller handles
                // alignment).
                self.wl(&format!("    # arg {} on stack", i));
                self.gen_operand_to_reg(arg, PhysReg::Rax);
                self.wl("    pushq %rax");
            }
        }

        // Align stack to 16 bytes before call.
        let extra_pushed = if args.len() > arg_regs.len() {
            args.len() - arg_regs.len()
        } else {
            0
        };
        let needs_align = extra_pushed % 2 != 0;
        if needs_align {
            self.wl("    subq $8, %rsp");
        }

        self.wl(&format!("    call {}", target));

        if needs_align {
            self.wl("    addq $8, %rsp");
        }
        if extra_pushed > 0 {
            self.wl(&format!("    addq ${}, %rsp", extra_pushed * 8));
        }

        // Move result from RAX to dst.
        if let Some(dst) = dst {
            if let Some(loc) = self.dst_loc(*dst) {
                match loc {
                    Location::Reg(r) if r != PhysReg::Rax => {
                        self.wl(&format!("    movq %rax, %{}", r.name()));
                    }
                    Location::Spill(off) => {
                        self.wl(&format!("    movq %rax, -{}(%rbp)", off));
                    }
                    _ => {}
                }
            }
        }
    }

    fn gen_call_ptr(&mut self, dst: Option<&VReg>, fn_ptr: &Operand, args: &[Operand], has_env: bool) {
        // Load function pointer into R11 (a caller-saved scratch reg).
        // If has_env is true, fn_ptr is actually an env pointer and the
        // real function pointer is stored at env[0].
        self.gen_operand_to_reg(fn_ptr, PhysReg::R11);
        if has_env {
            // Load the real function pointer from env[0].
            self.wl("    movq (%r11), %r11");
        }

        // Move args into argument registers.
        // When has_env is true, the env pointer is passed as the first
        // argument (in %rdi), and the actual args start at index 1 (%rsi).
        let arg_regs = PhysReg::arg_regs();
        let arg_start = if has_env { 1 } else { 0 };
        if has_env {
            // Pass env pointer in %rdi.
            self.gen_operand_to_reg(fn_ptr, PhysReg::Rdi);
        }
        for (i, arg) in args.iter().enumerate() {
            let reg_idx = i + arg_start;
            if reg_idx < arg_regs.len() {
                self.gen_operand_to_reg(arg, arg_regs[reg_idx]);
            }
        }

        self.wl("    call *%r11");

        if let Some(dst) = dst {
            if let Some(loc) = self.dst_loc(*dst) {
                match loc {
                    Location::Reg(r) if r != PhysReg::Rax => {
                        self.wl(&format!("    movq %rax, %{}", r.name()));
                    }
                    Location::Spill(off) => {
                        self.wl(&format!("    movq %rax, -{}(%rbp)", off));
                    }
                    _ => {}
                }
            }
        }
    }

    fn gen_cast(&mut self, dst: VReg, src: &Operand, kind: CastKind) {
        let dst_loc = self.dst_loc(dst);
        let dst_reg = match dst_loc {
            Some(Location::Reg(r)) => r,
            _ => PhysReg::Rax,
        };

        self.gen_operand_to_reg(src, dst_reg);

        match kind {
            CastKind::BitCast => {
                // No-op for same-size types.
            }
            CastKind::IntExtend => {
                // Sign-extend from 32-bit to 64-bit.
                self.wl(&format!("    movslq %e{}, %{}", dst_reg.name(), dst_reg.name()));
            }
            CastKind::IntTrunc => {
                // Truncate to 32-bit (zero-extend).
                self.wl(&format!("    movl %e{}, %e{}", dst_reg.name(), dst_reg.name()));
            }
            CastKind::IntToFloat => {
                // cvtsi2sdq
                self.wl(&format!("    cvtsi2sdq %{}, %xmm0", dst_reg.name()));
                self.wl("    movsd %xmm0, -8(%rbp)");
                self.wl("    movq -8(%rbp), %rax");
                if dst_reg != PhysReg::Rax {
                    self.wl(&format!("    movq %rax, %{}", dst_reg.name()));
                }
            }
            CastKind::FloatToInt => {
                // cvttsd2siq
                self.wl("    movq %rax, -8(%rbp)");
                self.wl("    movsd -8(%rbp), %xmm0");
                self.wl(&format!("    cvttsd2siq %xmm0, %{}", dst_reg.name()));
            }
        }

        if let Some(Location::Spill(off)) = dst_loc {
            self.wl(&format!("    movq %{}, -{}(%rbp)", dst_reg.name(), off));
        }
    }

    // ─── Operand helpers ──────────────────────────────────────────────────

    /// Get the location of a VReg from the current function's allocation.
    fn dst_loc(&self, vreg: VReg) -> Option<Location> {
        self.current_alloc.as_ref().map(|a| a.loc(vreg))
    }

    /// String representation of a VReg's current location
    fn vreg_str(&self, vreg: &VReg) -> String {
        match self.dst_loc(*vreg) {
            Some(Location::Reg(r)) => format!("%{}", r.name()),
            Some(Location::Spill(off)) => format!("-{}(%rbp)", off),
            None => format!("%v{}", vreg.0),
        }
    }

    /// Get the location of an operand (if it's a VReg).
    fn operand_loc(&self, vreg: VReg) -> Option<Location> {
        self.dst_loc(vreg)
    }

    /// Get the physical register holding an operand (if it's a VReg in a reg).
    fn operand_reg(&self, op: &Operand) -> Option<PhysReg> {
        match op {
            Operand::VReg(vr) => self.dst_loc(*vr).and_then(|l| match l {
                Location::Reg(r) => Some(r),
                _ => None,
            }),
            _ => None,
        }
    }

    /// Load an operand into a physical register.
    fn gen_operand_to_reg(&mut self, op: &Operand, reg: PhysReg) {
        match op {
            Operand::ImmI(v) => {
                self.wl(&format!("    movq ${}, %{}", v, reg.name()));
            }
            Operand::ImmF(bits) => {
                // Load float immediate from a constant pool entry.
                let label = self.alloc_float_const(*bits);
                self.wl(&format!("    movsd {}(%rip), %xmm0", label));
                self.wl("    movq %xmm0, %rax");
                if reg != PhysReg::Rax {
                    self.wl(&format!("    movq %rax, %{}", reg.name()));
                }
            }
            Operand::ImmStr(s) => {
                let label = self.alloc_string(s);
                self.wl(&format!("    leaq {}(%rip), %{}", label, reg.name()));
            }
            Operand::ImmFunc(name) => {
                // Load function address via RIP-relative LEA.
                self.wl(&format!("    leaq {}(%rip), %{}", name, reg.name()));
            }
            Operand::ImmNull => {
                self.wl(&format!("    xorq %{}, %{}", reg.name(), reg.name()));
            }
            Operand::VReg(vr) => {
                let loc = self.dst_loc(*vr);
                match loc {
                    Some(Location::Reg(r)) if r != reg => {
                        self.wl(&format!("    movq %{}, %{}", r.name(), reg.name()));
                    }
                    Some(Location::Reg(_)) => {} // already in the right register
                    Some(Location::Spill(off)) => {
                        self.wl(&format!("    movq -{}(%rbp), %{}", off, reg.name()));
                    }
                    None => {
                        self.wl(&format!("    # operand {} unallocated", vr));
                    }
                }
            }
            Operand::Slot(slot) => {
                self.wl(&format!(
                    "    leaq -{}(%rbp), %{}",
                    slot.0,
                    reg.name()
                ));
            }
        }
    }

    /// Load an operand into an XMM register (for SIMD operations).
    /// Uses movapd for VReg/stack operands, movsd for scalar floats.
    fn gen_operand_to_xmm(&mut self, op: &Operand, reg: PhysReg) {
        let xmm_name = reg.name();
        match op {
            Operand::ImmF(bits) => {
                let label = self.alloc_float_const(*bits);
                self.wl(&format!("    movsd {}(%rip), %{}", label, xmm_name));
            }
            Operand::VReg(vr) => {
                let loc = self.dst_loc(*vr);
                match loc {
                    Some(Location::Reg(r)) => {
                        // GP reg → XMM via movq
                        self.wl(&format!("    movq %{}, %{}", r.name(), xmm_name));
                    }
                    Some(Location::Spill(off)) => {
                        self.wl(&format!("    movapd -{}(%rbp), %{}", off, xmm_name));
                    }
                    None => {
                        self.wl(&format!("    # operand {} unallocated for XMM", vr));
                    }
                }
            }
            Operand::Slot(slot) => {
                self.wl(&format!("    movapd -{}(%rbp), %{}", slot.0, xmm_name));
            }
            _ => {
                // Fallback: load via GP reg then move to XMM
                self.gen_operand_to_reg(op, PhysReg::Rax);
                self.wl(&format!("    movq %rax, %{}", xmm_name));
            }
        }
    }

    /// String representation of an operand for inline comments
    fn op_str(&self, op: &Operand) -> String {
        match op {
            Operand::ImmI(v) => format!("${}", v),
            Operand::ImmF(bits) => format!("f64:0x{:016x}", bits),
            Operand::ImmStr(s) => format!("\"{}\"", s),
            Operand::ImmFunc(s) => format!("&{}", s),
            Operand::ImmNull => "null".to_string(),
            Operand::VReg(vr) => format!("%v{}", vr.0),
            Operand::Slot(slot) => format!("slot({})", slot.0),
        }
    }

    /// Store an operand to a memory location (stack slot at offset from RBP).
    fn gen_operand_to_mem(&mut self, op: &Operand, offset: u32) {
        match op {
            Operand::ImmI(v) => {
                self.wl(&format!("    movq ${}, -{}(%rbp)", v, offset));
            }
            Operand::ImmNull => {
                self.wl(&format!("    movq $0, -{}(%rbp)", offset));
            }
            Operand::VReg(vr) => {
                let loc = self.dst_loc(*vr);
                match loc {
                    Some(Location::Reg(r)) => {
                        self.wl(&format!("    movq %{}, -{}(%rbp)", r.name(), offset));
                    }
                    Some(Location::Spill(src_off)) => {
                        // Spill-to-spill: load into RAX, then store.
                        self.wl(&format!("    movq -{}(%rbp), %rax", src_off));
                        self.wl(&format!("    movq %rax, -{}(%rbp)", offset));
                    }
                    None => {}
                }
            }
            Operand::ImmStr(s) => {
                let label = self.alloc_string(s);
                self.wl(&format!("    leaq {}(%rip), %rax", label));
                self.wl(&format!("    movq %rax, -{}(%rbp)", offset));
            }
            Operand::ImmFunc(name) => {
                self.wl(&format!("    leaq {}(%rip), %rax", name));
                self.wl(&format!("    movq %rax, -{}(%rbp)", offset));
            }
            Operand::ImmF(_) => {
                self.wl(&format!("    movq $0, -{}(%rbp)", offset));
            }
            Operand::Slot(slot) => {
                self.wl(&format!("    leaq -{}(%rbp), %rax", slot.0));
                self.wl(&format!("    movq %rax, -{}(%rbp)", offset));
            }
        }
    }

    /// Apply a binary op instruction (addq, subq, andq, orq, xorq) with
    /// the given operand as the source.
    fn gen_operand_apply_binop(&mut self, op: &str, b: &Operand, dst_reg: PhysReg) {
        match b {
            Operand::ImmI(v) => {
                self.wl(&format!("    {} ${}, %{}", op, v, dst_reg.name()));
            }
            Operand::VReg(vr) => {
                let loc = self.dst_loc(*vr);
                match loc {
                    Some(Location::Reg(r)) => {
                        self.wl(&format!("    {} %{}, %{}", op, r.name(), dst_reg.name()));
                    }
                    Some(Location::Spill(off)) => {
                        self.wl(&format!("    {} -{}(%rbp), %{}", op, off, dst_reg.name()));
                    }
                    None => {}
                }
            }
            _ => {
                // Load into scratch and apply.
                let tmp = self.scratch_reg(dst_reg);
                self.gen_operand_to_reg(b, tmp);
                self.wl(&format!("    {} %{}, %{}", op, tmp.name(), dst_reg.name()));
            }
        }
    }

    /// Generate a shift instruction (shlq, shrq) with the shift count from `b`.
    fn gen_shift(&mut self, op: &str, b: &Operand, dst_reg: PhysReg) {
        match b {
            Operand::ImmI(v) => {
                self.wl(&format!("    {} ${}, %{}", op, v, dst_reg.name()));
            }
            _ => {
                // x86 shifts require the count in CL.
                self.gen_operand_to_reg(b, PhysReg::Rcx);
                self.wl(&format!("    {} %cl, %{}", op, dst_reg.name()));
            }
        }
    }

    /// Generate a comparison: set dst to 0 or 1 based on the comparison.
    fn gen_comparison(&mut self, op: BinOp, b: &Operand, dst_reg: PhysReg) {
        // Handle logical And/Or separately — they don't use cmpq.
        match op {
            BinOp::And => {
                // Logical and: dst = (dst != 0) && (b != 0)
                // Load b into scratch FIRST (before setne clobbers %al which may alias b's register).
                let tmp = self.scratch_reg(dst_reg);
                self.gen_operand_to_reg(b, tmp);
                // Now test dst_reg.
                self.wl(&format!("    testq %{}, %{}", dst_reg.name(), dst_reg.name()));
                self.wl("    setne %al");
                // Test b (in scratch).
                self.wl(&format!("    testq %{}, %{}", tmp.name(), tmp.name()));
                self.wl("    setne %cl");
                // AND the two results.
                self.wl("    andb %cl, %al");
                self.wl(&format!("    movzbq %al, %{}", dst_reg.name()));
                return;
            }
            BinOp::Or => {
                // Logical or: dst = (dst != 0) || (b != 0)
                let tmp = self.scratch_reg(dst_reg);
                self.gen_operand_to_reg(b, tmp);
                self.wl(&format!("    testq %{}, %{}", dst_reg.name(), dst_reg.name()));
                self.wl("    setne %al");
                self.wl(&format!("    testq %{}, %{}", tmp.name(), tmp.name()));
                self.wl("    setne %cl");
                self.wl("    orb %cl, %al");
                self.wl(&format!("    movzbq %al, %{}", dst_reg.name()));
                return;
            }
            _ => {}
        }

        // Compare dst_reg with b.
        match b {
            Operand::ImmI(v) => {
                self.wl(&format!("    cmpq ${}, %{}", v, dst_reg.name()));
            }
            Operand::VReg(vr) => {
                let loc = self.dst_loc(*vr);
                match loc {
                    Some(Location::Reg(r)) => {
                        self.wl(&format!("    cmpq %{}, %{}", r.name(), dst_reg.name()));
                    }
                    Some(Location::Spill(off)) => {
                        self.wl(&format!("    cmpq -{}(%rbp), %{}", off, dst_reg.name()));
                    }
                    None => {}
                }
            }
            _ => {
                let tmp = self.scratch_reg(dst_reg);
                self.gen_operand_to_reg(b, tmp);
                self.wl(&format!("    cmpq %{}, %{}", tmp.name(), dst_reg.name()));
            }
        }

        let setcc = match op {
            BinOp::Eq => "sete",
            BinOp::Neq => "setne",
            BinOp::Lt => "setl",
            BinOp::Gt => "setg",
            BinOp::LtEq => "setle",
            BinOp::GtEq => "setge",
            _ => "sete",
        };

        self.wl(&format!("    {} %al", setcc));
        self.wl(&format!("    movzbq %al, %{}", dst_reg.name()));
    }

    /// Pick a scratch register that doesn't conflict with `dst`.
    fn scratch_reg(&self, dst: PhysReg) -> PhysReg {
        // Prefer RCX (often free), then RDX, then R10.
        if dst != PhysReg::Rcx {
            PhysReg::Rcx
        } else if dst != PhysReg::Rdx {
            PhysReg::Rdx
        } else {
            PhysReg::R10
        }
    }

    /// Format a register for memory operand syntax: "%reg" → "%reg".
    fn reg_str(&self, r: PhysReg) -> String {
        format!("%{}", r.name())
    }

    /// Allocate a string constant and return its label.
    fn alloc_string(&mut self, s: &str) -> String {
        // Check if we already have this exact string.
        for (label, val) in &self.strings {
            if val == s {
                return label.clone();
            }
        }
        let label = format!(".str_{}", self.string_counter);
        self.string_counter += 1;
        self.strings.insert(label.clone(), s.to_string());
        label
    }

    /// Allocate a float constant and return its label.
    fn alloc_float_const(&mut self, bits: u64) -> String {
        let label = format!(".flt_{}", self.string_counter);
        self.string_counter += 1;
        // Store the bits as a .quad in the data section.
        // We'll emit it at the end; for now, record it.
        self.strings.insert(label.clone(), format!("__float_bits:{}", bits));
        label
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Helpers
// ═══════════════════════════════════════════════════════════════════════════

fn type_size(ty: &IrType) -> u64 {
    match ty {
        IrType::I8 | IrType::U8 | IrType::Bool => 1,
        IrType::I16 | IrType::U16 => 2,
        IrType::I32 | IrType::U32 | IrType::F32 => 4,
        IrType::I64 | IrType::U64 | IrType::F64 | IrType::Ptr(_) => 8,
        IrType::Void => 0,
        IrType::Never => 0,
        IrType::Array(elem, n) => type_size(elem) * (*n as u64),
        IrType::Struct(name) => {
            let _ = name;
            8
        }
        IrType::Opaque(_) => 8,
        _ => 8,
    }
}

fn sub_reg_name(reg: PhysReg, size: u64) -> &'static str {
    // x86-64 sub-register names for 1/2/4-byte accesses.
    if size == 1 {
        match reg {
            PhysReg::Rax => "%al",
            PhysReg::Rcx => "%cl",
            PhysReg::Rdx => "%dl",
            PhysReg::Rbx => "%bl",
            PhysReg::Rsi => "%sil",
            PhysReg::Rdi => "%dil",
            PhysReg::R8 => "%r8b",
            PhysReg::R9 => "%r9b",
            PhysReg::R10 => "%r10b",
            PhysReg::R11 => "%r11b",
            PhysReg::R12 => "%r12b",
            PhysReg::R13 => "%r13b",
            PhysReg::R14 => "%r14b",
            PhysReg::R15 => "%r15b",
            _ => "%al",
        }
    } else if size == 2 {
        match reg {
            PhysReg::Rax => "%ax",
            PhysReg::Rcx => "%cx",
            PhysReg::Rdx => "%dx",
            PhysReg::Rbx => "%bx",
            PhysReg::Rsi => "%si",
            PhysReg::Rdi => "%di",
            PhysReg::R8 => "%r8w",
            PhysReg::R9 => "%r9w",
            _ => "%ax",
        }
    } else {
        // 4-byte
        match reg {
            PhysReg::Rax => "%eax",
            PhysReg::Rcx => "%ecx",
            PhysReg::Rdx => "%edx",
            PhysReg::Rbx => "%ebx",
            PhysReg::Rsi => "%esi",
            PhysReg::Rdi => "%edi",
            PhysReg::R8 => "%r8d",
            PhysReg::R9 => "%r9d",
            _ => "%eax",
        }
    }
}

fn escape_str(s: &str) -> String {
    s.replace('\\', "\\\\")
        .replace('"', "\\\"")
        .replace('\n', "\\n")
        .replace('\t', "\\t")
        .replace('\r', "\\r")
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;
    use crate::compiler;
    use crate::pir::{lower_ir_to_l1, lower_l1_to_l2, lower_l2_to_l3};

    fn compile_to_x64(src: &str) -> String {
        let module = compiler::lower_to_ir_module(src).expect("compile failed");
        let l1 = lower_ir_to_l1(&module);
        let l2 = lower_l1_to_l2(&l1);
        let l3 = lower_l2_to_l3(&l2, &module);
        generate_asm(&l3)
    }

    #[test]
    fn generates_function_label() {
        let asm = compile_to_x64("pf main() -> i64 { ret 42; }\n");
        assert!(
            asm.contains("main:"),
            "expected function label 'main:' in output:\n{}",
            asm
        );
    }

    #[test]
    fn generates_prologue_epilogue() {
        let asm = compile_to_x64("pf main() -> i64 { ret 0; }\n");
        assert!(asm.contains("pushq %rbp"), "expected pushq %rbp in prologue");
        assert!(asm.contains("movq %rsp, %rbp"), "expected movq %rsp, %rbp");
        assert!(asm.contains("leave"), "expected leave in epilogue");
        assert!(asm.contains("ret"), "expected ret in epilogue");
    }

    #[test]
    fn generates_return_value() {
        let asm = compile_to_x64("pf main() -> i64 { ret 42; }\n");
        assert!(
            asm.contains("movq $42, %rax") || asm.contains("$42"),
            "expected immediate 42 in output:\n{}",
            asm
        );
    }

    #[test]
    fn generates_add_instruction() {
        let asm = compile_to_x64("pf main() -> i64 { ret 40 + 2; }\n");
        assert!(
            asm.contains("addq") || asm.contains("imulq"),
            "expected addq or arithmetic in output:\n{}",
            asm
        );
    }

    #[test]
    fn generates_call_instruction() {
        let asm = compile_to_x64("pf main() -> i64 { print(\"hi\"); ret 0; }\n");
        assert!(
            asm.contains("call") || asm.contains("Call"),
            "expected call instruction in output:\n{}",
            asm
        );
    }

    #[test]
    fn generates_string_constant() {
        let asm = compile_to_x64("pf main() -> i64 { print(\"hello\"); ret 0; }\n");
        assert!(
            asm.contains("hello") || asm.contains(".str"),
            "expected string constant in output:\n{}",
            asm
        );
    }

    #[test]
    fn generates_branch_instruction() {
        let asm = compile_to_x64("pf main() -> i64 { if true { ret 1; } else { ret 0; } ret 9; }\n");
        assert!(
            asm.contains("jne") || asm.contains("je") || asm.contains("jmp"),
            "expected branch/jump instruction in output:\n{}",
            asm
        );
    }

    #[test]
    fn generates_multiple_functions() {
        let asm = compile_to_x64(
            "pf helper(x: i64) -> i64 { ret x + 1; }\npf main() -> i64 { ret helper(5); }\n",
        );
        assert!(asm.contains("helper:"), "expected helper function label");
        assert!(asm.contains("main:"), "expected main function label");
    }

    #[test]
    fn type_size_correct() {
        assert_eq!(type_size(&IrType::I8), 1);
        assert_eq!(type_size(&IrType::I16), 2);
        assert_eq!(type_size(&IrType::I32), 4);
        assert_eq!(type_size(&IrType::I64), 8);
        assert_eq!(type_size(&IrType::F64), 8);
        assert_eq!(type_size(&IrType::Bool), 1);
        assert_eq!(type_size(&IrType::Void), 0);
        assert_eq!(type_size(&IrType::Ptr(Box::new(IrType::I64))), 8);
    }

    #[test]
    fn escape_str_works() {
        assert_eq!(escape_str("hello"), "hello");
        assert_eq!(escape_str("a\nb"), "a\\nb");
        assert_eq!(escape_str("a\"b"), "a\\\"b");
    }
}
