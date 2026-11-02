// RISC-V (RV64) Codegen — Phase 4C
//
// Instruction selector that consumes PIR L3 + a register allocation and
// produces RISC-V RV64 assembly. This is the third target in the
// multi-architecture codegen backend.
//
// Pipeline: L3Module → (regalloc) → [code selector] → RISC-V asm string
//
// Calling convention: RISC-V calling convention
//   - Args: a0-a7 (x10-x17, first 8 integer/pointer)
//   - Return: a0 (x10)
//   - Callee-saved: s0-s11 (x8, x9, x18-x27)
//   - Stack 16-byte aligned at call sites

use crate::ast::BinOp;
use crate::pir::{L3Block, L3Function, L3Inst, L3Module, Operand, VReg};
use std::collections::HashMap;

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

/// Generate RISC-V assembly for an entire L3 module.
pub fn generate_asm(module: &L3Module) -> String {
    let mut gen = RiscVCodeGen::new();
    gen.gen_module(module);
    gen.output
}

// ═══════════════════════════════════════════════════════════════════════════
// RISC-V Register definitions
// ═══════════════════════════════════════════════════════════════════════════


fn reg_name(idx: usize) -> &'static str {
    // RISC-V register names (ABI names)
    const NAMES: [&str; 32] = [
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
    ];
    // Map VReg to a usable register (skip x0-x4 which are special)
    let mapped = 5 + (idx % 23);
    NAMES.get(mapped).copied().unwrap_or("t0")
}

fn vreg_to_reg(vreg: &VReg) -> &'static str {
    reg_name(vreg.0 as usize)
}

// ═══════════════════════════════════════════════════════════════════════════
// Code Generator
// ═══════════════════════════════════════════════════════════════════════════

struct RiscVCodeGen {
    output: String,
    label_counter: usize,
    string_consts: HashMap<String, String>,
    float_consts: HashMap<u64, String>,
}

impl RiscVCodeGen {
    fn new() -> Self {
        RiscVCodeGen {
            output: String::new(),
            label_counter: 0,
            string_consts: HashMap::new(),
            float_consts: HashMap::new(),
        }
    }

    fn gen_module(&mut self, module: &L3Module) {
        // Text section
        self.emit("    .section .text");
        self.emit("    .globl main");

        for func in &module.functions {
            self.gen_function(func);
        }

        // Data section for strings and floats
        if !self.string_consts.is_empty() || !self.float_consts.is_empty() {
            self.emit("    .section .rodata");
            let entries: Vec<(String, String)> = self.string_consts.iter()
                .map(|(s, l)| (l.clone(), s.clone())).collect();
            for (label, s) in entries {
                self.emit(&format!("{}:", label));
                self.emit(&format!("    .asciz \"{}\"", s.replace('\\', "\\\\").replace('"', "\\\"")));
            }
            let floats: Vec<(String, u64)> = self.float_consts.iter()
                .map(|(b, l)| (l.clone(), *b)).collect();
            for (label, bits) in floats {
                self.emit(&format!("{}:", label));
                self.emit(&format!("    .quad {}", bits));
            }
        }
    }

    fn gen_function(&mut self, func: &L3Function) {
        self.emit(&format!(".globl {}", func.name));
        self.emit(&format!("{}:", func.name));

        // Prologue: save ra and s0, allocate stack
        self.emit("    addi sp, sp, -16");
        self.emit("    sd ra, 8(sp)");
        self.emit("    sd s0, 0(sp)");
        self.emit("    mv s0, sp");

        // Stack frame
        let frame_size = 256;
        if frame_size > 0 {
            self.emit(&format!("    addi sp, sp, -{}", frame_size));
        }

        for block in &func.blocks {
            self.emit(&format!(".L{}{}_b{}:", func.name, self.label_counter, block.id.0));
            self.gen_block(func, block);
        }

        self.label_counter += 1;
    }

    fn gen_block(&mut self, func: &L3Function, block: &L3Block) {
        for inst in &block.instructions {
            self.gen_inst(func, inst);
        }
    }

    fn gen_inst(&mut self, func: &L3Function, inst: &L3Inst) {
        match inst {
            L3Inst::AddrOfSlot { dst, slot } => {
                let d = vreg_to_reg(dst);
                self.emit(&format!("    addi {}, fp, -{}", d, slot.0));
            }
            L3Inst::GlobalAddr { dst, name, .. } => {
                let d = vreg_to_reg(dst);
                self.emit(&format!("    # global addr {}", name));
                self.emit(&format!("    li {}, 0", d));
            }
            L3Inst::Mov { dst, src } => {
                let d = vreg_to_reg(dst);
                match src {
                    Operand::ImmI(i) => {
                        if *i >= -2048 && *i <= 2047 {
                            self.emit(&format!("    li {}, {}", d, i));
                        } else {
                            let v = *i as u64;
                            let hi = (v >> 12) as i32;
                            let lo = (v & 0xfff) as i32;
                            self.emit(&format!("    lui {}, {}", d, hi));
                            if lo != 0 { self.emit(&format!("    addi {}, {}, {}", d, d, lo)); }
                        }
                    }
                    Operand::VReg(v) => {
                        let s = vreg_to_reg(v);
                        if d != s { self.emit(&format!("    mv {}, {}", d, s)); }
                    }
                    _ => {}
                }
            }

            L3Inst::Bin { op, dst, a, b, ty } => {
                let d = vreg_to_reg(dst);
                let l = operand_to_reg(a);
                let r = operand_to_reg(b);
                let _ = ty;
                match op {
                    BinOp::Add => self.emit(&format!("    add {}, {}, {}", d, l, r)),
                    BinOp::Sub => self.emit(&format!("    sub {}, {}, {}", d, l, r)),
                    BinOp::Mul => self.emit(&format!("    mul {}, {}, {}", d, l, r)),
                    BinOp::Div => self.emit(&format!("    div {}, {}, {}", d, l, r)),
                    BinOp::Mod => self.emit(&format!("    rem {}, {}, {}", d, l, r)),
                    BinOp::BitAnd => self.emit(&format!("    and {}, {}, {}", d, l, r)),
                    BinOp::BitOr => self.emit(&format!("    or {}, {}, {}", d, l, r)),
                    BinOp::BitXor => self.emit(&format!("    xor {}, {}, {}", d, l, r)),
                    BinOp::Shl => self.emit(&format!("    sll {}, {}, {}", d, l, r)),
                    BinOp::Shr => self.emit(&format!("    sra {}, {}, {}", d, l, r)),
                    BinOp::Eq => { self.emit(&format!("    sub {}, {}, {}", d, l, r)); self.emit(&format!("    seqz {}, {}", d, d)); }
                    BinOp::Neq => { self.emit(&format!("    sub {}, {}, {}", d, l, r)); self.emit(&format!("    snez {}, {}", d, d)); }
                    BinOp::Lt => self.emit(&format!("    slt {}, {}, {}", d, l, r)),
                    BinOp::Gt => self.emit(&format!("    slt {}, {}, {}", d, r, l)),
                    BinOp::LtEq => { self.emit(&format!("    slt {}, {}, {}", d, r, l)); self.emit(&format!("    xori {}, {}, 1", d, d)); }
                    BinOp::GtEq => { self.emit(&format!("    slt {}, {}, {}", d, l, r)); self.emit(&format!("    xori {}, {}, 1", d, d)); }
                    BinOp::And => { self.emit(&format!("    and {}, {}, {}", d, l, r)); self.emit(&format!("    snez {}, {}", d, d)); }
                    BinOp::Or => { self.emit(&format!("    or {}, {}, {}", d, l, r)); self.emit(&format!("    snez {}, {}", d, d)); }
                }
            }

            L3Inst::Unary { op, dst, src, .. } => {
                let d = vreg_to_reg(dst);
                let s = operand_to_reg(src);
                match op {
                    crate::ast::UnaryOp::Neg => self.emit(&format!("    neg {}, {}", d, s)),
                    crate::ast::UnaryOp::Not => self.emit(&format!("    seqz {}, {}", d, s)),
                    crate::ast::UnaryOp::BitNot => self.emit(&format!("    not {}, {}", d, s)),
                }
            }

            L3Inst::StackAlloc { dst, bytes, .. } => {
                let reg = vreg_to_reg(dst);
                self.emit(&format!("    sub {}, sp, {}", reg, bytes));
            }

            L3Inst::Load { dst, ptr, .. } => {
                let d = vreg_to_reg(dst);
                match ptr {
                    Operand::Slot(s) => {
                        self.emit(&format!("    ld {}, -{}(fp)", d, s.0));
                    }
                    Operand::VReg(v) => {
                        let p = vreg_to_reg(v);
                        self.emit(&format!("    ld {}, 0({})", d, p));
                    }
                    _ => {
                        self.emit(&format!("    ld {}, 0(t0)", d));
                    }
                }
            }

            L3Inst::Store { ptr, val, .. } => {
                let v = operand_to_reg(val);
                match ptr {
                    Operand::Slot(s) => {
                        self.emit(&format!("    sd {}, -{}(fp)", v, s.0));
                    }
                    Operand::VReg(pv) => {
                        let p = vreg_to_reg(pv);
                        self.emit(&format!("    sd {}, 0({})", v, p));
                    }
                    _ => {
                        self.emit(&format!("    sd {}, 0(t0)", v));
                    }
                }
            }

            L3Inst::FieldAddr { dst, base, offset, .. } => {
                let d = vreg_to_reg(dst);
                let b = operand_to_reg(base);
                self.emit(&format!("    addi {}, {}, {}", d, b, offset));
            }

            L3Inst::ElemAddr { dst, base, index, .. } => {
                let d = vreg_to_reg(dst);
                let b = operand_to_reg(base);
                let i = operand_to_reg(index);
                self.emit(&format!("    slli t0, {}, 3", i));
                self.emit(&format!("    add {}, {}, t0", d, b));
            }

            L3Inst::Jmp { target } => {
                self.emit(&format!("    j .L{}{}_b{}", func.name, self.label_counter, target.0));
            }

            L3Inst::Br { cond, then, else_ } => {
                let c = operand_to_reg(cond);
                self.emit(&format!("    bnez {}, .L{}{}_b{}", c, func.name, self.label_counter, then.0));
                self.emit(&format!("    j .L{}{}_b{}", func.name, self.label_counter, else_.0));
            }

            L3Inst::Ret { value } => {
                if let Some(val) = value {
                    let v = operand_to_reg(val);
                    self.emit(&format!("    mv a0, {}", v));
                }
                self.emit("    mv sp, s0");
                self.emit("    ld ra, 8(sp)");
                self.emit("    ld s0, 0(sp)");
                self.emit("    addi sp, sp, 16");
                self.emit("    ret");
            }

            L3Inst::Unreachable => self.emit("    ebreak"),

            L3Inst::Call { dst, target, args, .. } => {
                let arg_regs = ["a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"];
                for (i, arg) in args.iter().enumerate().take(8) {
                    let a = operand_to_reg(arg);
                    if a != arg_regs[i] { self.emit(&format!("    mv {}, {}", arg_regs[i], a)); }
                }
                self.emit(&format!("    call {}", target));
                if let Some(d) = dst {
                    let dreg = vreg_to_reg(d);
                    if dreg != "a0" { self.emit(&format!("    mv {}, a0", dreg)); }
                }
            }

            L3Inst::CallPtr { dst, fn_ptr, args, .. } => {
                let ptr = operand_to_reg(fn_ptr);
                let arg_regs = ["a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"];
                for (i, arg) in args.iter().enumerate().take(7) {
                    let a = operand_to_reg(arg);
                    if a != arg_regs[i] { self.emit(&format!("    mv {}, {}", arg_regs[i], a)); }
                }
                self.emit(&format!("    jalr {}", ptr));
                if let Some(d) = dst {
                    let dreg = vreg_to_reg(d);
                    if dreg != "a0" { self.emit(&format!("    mv {}, a0", dreg)); }
                }
            }

            L3Inst::DynCall { dst, trait_name, method, args, .. } => {
                let callee = format!("{}_{}", trait_name, method);
                let arg_regs = ["a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"];
                for (i, arg) in args.iter().enumerate().take(8) {
                    let a = operand_to_reg(arg);
                    if a != arg_regs[i] { self.emit(&format!("    mv {}, {}", arg_regs[i], a)); }
                }
                self.emit(&format!("    call {}", callee));
                if let Some(d) = dst {
                    let dreg = vreg_to_reg(d);
                    if dreg != "a0" { self.emit(&format!("    mv {}, a0", dreg)); }
                }
            }

            L3Inst::MakeDyn { dst, value, .. } => {
                let d = vreg_to_reg(dst);
                let v = operand_to_reg(value);
                if d != v { self.emit(&format!("    mv {}, {}", d, v)); }
            }

            L3Inst::Cast { dst, src, kind, .. } => {
                let d = vreg_to_reg(dst);
                let s = operand_to_reg(src);
                match kind {
                    crate::pir::CastKind::BitCast => { if d != s { self.emit(&format!("    mv {}, {}", d, s)); } }
                    crate::pir::CastKind::IntExtend => self.emit(&format!("    sext.w {}, {}", d, s)),
                    crate::pir::CastKind::IntTrunc => self.emit(&format!("    mv {}, {}", d, s)),
                    crate::pir::CastKind::IntToFloat => self.emit(&format!("    fcvt.s.l {}, {}", d, s)),
                    crate::pir::CastKind::FloatToInt => self.emit(&format!("    fcvt.l.s {}, {}", d, s)),
                }
            }

            L3Inst::TryUnwrap { value, ok_dst, err_block, ok_block } => {
                let v = operand_to_reg(value);
                let d = vreg_to_reg(ok_dst);
                self.emit(&format!("    beqz {}, .L{}{}_b{}", v, func.name, self.label_counter, err_block.0));
                self.emit(&format!("    mv {}, {}", d, v));
                self.emit(&format!("    j .L{}{}_b{}", func.name, self.label_counter, ok_block.0));
            }

            L3Inst::Print { val, .. } => {
                let v = operand_to_reg(val);
                self.emit(&format!("    mv a0, {}", v));
                self.emit("    call p_print_int");
            }

            L3Inst::Panic { message } => {
                self.emit(&format!("    # panic: {}", message));
                self.emit("    ebreak");
            }
        }
    }

    fn emit(&mut self, s: &str) {
        self.output.push_str(s);
        self.output.push('\n');
    }
}

fn operand_to_reg(op: &Operand) -> &'static str {
    match op {
        Operand::VReg(v) => vreg_to_reg(v),
        _ => "t0", // scratch register for immediates/slots
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_riscv_empty_module() {
        let module = L3Module::default();
        let asm = generate_asm(&module);
        assert!(asm.contains(".section .text"));
        assert!(asm.contains(".globl main"));
    }

    #[test]
    fn test_riscv_reg_names() {
        assert_eq!(reg_name(0), "t0");
    }
}
