// ARM64 (AArch64) Codegen — Phase 4C
//
// Instruction selector that consumes PIR L3 + a register allocation and
// produces ARM64 assembly. This is the second target in the multi-architecture
// codegen backend.
//
// Pipeline: L3Module → (regalloc) → [code selector] → ARM64 asm string
//
// Calling convention: AAPCS64
//   - Args: X0-X7 (first 8 integer/pointer)
//   - Return: X0
//   - Callee-saved: X19-X28, SP
//   - Stack 16-byte aligned at call sites

use crate::ast::BinOp;
use crate::pir::{L3Block, L3Function, L3Inst, L3Module, MemRegion, Operand, PBlock, VReg};
use std::collections::HashMap;

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

/// Generate ARM64 assembly for an entire L3 module.
pub fn generate_asm(module: &L3Module) -> String {
    let mut gen = Arm64CodeGen::new();
    gen.gen_module(module);
    gen.output
}

// ═══════════════════════════════════════════════════════════════════════════
// ARM64 Register definitions
// ═══════════════════════════════════════════════════════════════════════════

const NUM_REGS: usize = 29; // X0-X28 (X29=FP, X30=LR, X31=SP)

fn reg_name(idx: usize) -> &'static str {
    const NAMES: [&str; 31] = [
        "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9",
        "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x18", "x19",
        "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "x29", "x30",
    ];
    NAMES.get(idx).copied().unwrap_or("x0")
}

fn vreg_to_reg(vreg: &VReg) -> &'static str {
    reg_name(vreg.0 as usize % NUM_REGS)
}

// ═══════════════════════════════════════════════════════════════════════════
// Code Generator
// ═══════════════════════════════════════════════════════════════════════════

struct Arm64CodeGen {
    output: String,
    label_counter: usize,
    string_consts: HashMap<String, String>,
    float_consts: HashMap<u64, String>,
}

impl Arm64CodeGen {
    fn new() -> Self {
        Arm64CodeGen {
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

        // Data section for strings and floats (emitted after code so they're populated)
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

        // Prologue: save LR and FP, allocate stack
        self.emit("    stp x29, x30, [sp, #-16]!");
        self.emit("    mov x29, sp");

        // Calculate stack frame size (simplified: 256 bytes for locals)
        let frame_size = 256;
        if frame_size > 0 {
            self.emit(&format!("    sub sp, sp, #{}", frame_size));
        }

        // Generate blocks
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
            L3Inst::Mov { dst, src } => {
                let d = vreg_to_reg(dst);
                match src {
                    Operand::ImmI(i) => {
                        if *i >= -2048 && *i <= 2047 {
                            self.emit(&format!("    mov {}, #{}", d, i));
                        } else {
                            let v = *i as u64;
                            self.emit(&format!("    movz {}, #{}", d, v & 0xffff));
                            if v > 0xffff {
                                self.emit(&format!("    movk {}, #{}, lsl #16", d, (v >> 16) & 0xffff));
                            }
                            if v > 0xffffffff {
                                self.emit(&format!("    movk {}, #{}, lsl #32", d, (v >> 32) & 0xffff));
                            }
                        }
                    }
                    Operand::VReg(v) => {
                        let s = vreg_to_reg(v);
                        if d != s {
                            self.emit(&format!("    mov {}, {}", d, s));
                        }
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
                    BinOp::Div => self.emit(&format!("    sdiv {}, {}, {}", d, l, r)),
                    BinOp::Mod => {
                        self.emit(&format!("    sdiv x8, {}, {}", l, r));
                        self.emit(&format!("    msub {}, {}, x8, {}", d, r, l));
                    }
                    BinOp::BitAnd => self.emit(&format!("    and {}, {}, {}", d, l, r)),
                    BinOp::BitOr => self.emit(&format!("    orr {}, {}, {}", d, l, r)),
                    BinOp::BitXor => self.emit(&format!("    eor {}, {}, {}", d, l, r)),
                    BinOp::Shl => self.emit(&format!("    lsl {}, {}, {}", d, l, r)),
                    BinOp::Shr => self.emit(&format!("    asr {}, {}, {}", d, l, r)),
                    BinOp::Eq => { self.emit(&format!("    cmp {}, {}", l, r)); self.emit(&format!("    cset {}, eq", d)); }
                    BinOp::Neq => { self.emit(&format!("    cmp {}, {}", l, r)); self.emit(&format!("    cset {}, ne", d)); }
                    BinOp::Lt => { self.emit(&format!("    cmp {}, {}", l, r)); self.emit(&format!("    cset {}, lt", d)); }
                    BinOp::Gt => { self.emit(&format!("    cmp {}, {}", l, r)); self.emit(&format!("    cset {}, gt", d)); }
                    BinOp::LtEq => { self.emit(&format!("    cmp {}, {}", l, r)); self.emit(&format!("    cset {}, le", d)); }
                    BinOp::GtEq => { self.emit(&format!("    cmp {}, {}", l, r)); self.emit(&format!("    cset {}, ge", d)); }
                    BinOp::And => self.emit(&format!("    and {}, {}, {}", d, l, r)),
                    BinOp::Or => self.emit(&format!("    orr {}, {}, {}", d, l, r)),
                }
            }

            L3Inst::Unary { op, dst, src, .. } => {
                let d = vreg_to_reg(dst);
                let s = operand_to_reg(src);
                match op {
                    crate::ast::UnaryOp::Neg => self.emit(&format!("    neg {}, {}", d, s)),
                    crate::ast::UnaryOp::Not => { self.emit(&format!("    cmp {}, #0", s)); self.emit(&format!("    cset {}, eq", d)); }
                    crate::ast::UnaryOp::BitNot => self.emit(&format!("    mvn {}, {}", d, s)),
                }
            }

            L3Inst::StackAlloc { dst, bytes, .. } => {
                let reg = vreg_to_reg(dst);
                self.emit(&format!("    sub {}, sp, #{}", reg, bytes));
            }

            L3Inst::Load { dst, ptr, .. } => {
                let d = vreg_to_reg(dst);
                let p = operand_to_reg(ptr);
                self.emit(&format!("    ldr {}, [{}]", d, p));
            }

            L3Inst::Store { ptr, val, .. } => {
                let p = operand_to_reg(ptr);
                let v = operand_to_reg(val);
                self.emit(&format!("    str {}, [{}]", v, p));
            }

            L3Inst::FieldAddr { dst, base, offset, .. } => {
                let d = vreg_to_reg(dst);
                let b = operand_to_reg(base);
                self.emit(&format!("    add {}, {}, #{}", d, b, offset));
            }

            L3Inst::ElemAddr { dst, base, index, .. } => {
                let d = vreg_to_reg(dst);
                let b = operand_to_reg(base);
                let i = operand_to_reg(index);
                self.emit(&format!("    lsl x8, {}, #3", i));
                self.emit(&format!("    add {}, {}, x8", d, b));
            }

            L3Inst::Jmp { target } => {
                self.emit(&format!("    b .L{}{}_b{}", func.name, self.label_counter, target.0));
            }

            L3Inst::Br { cond, then, else_ } => {
                let c = operand_to_reg(cond);
                self.emit(&format!("    cmp {}, #0", c));
                self.emit(&format!("    b.ne .L{}{}_b{}", func.name, self.label_counter, then.0));
                self.emit(&format!("    b .L{}{}_b{}", func.name, self.label_counter, else_.0));
            }

            L3Inst::Ret { value } => {
                if let Some(val) = value {
                    let v = operand_to_reg(val);
                    self.emit(&format!("    mov x0, {}", v));
                }
                self.emit("    mov sp, x29");
                self.emit("    ldp x29, x30, [sp], #16");
                self.emit("    ret");
            }

            L3Inst::Unreachable => self.emit("    brk #0"),

            L3Inst::Call { dst, target, args, .. } => {
                for (i, arg) in args.iter().enumerate().take(8) {
                    let a = operand_to_reg(arg);
                    let arg_reg = reg_name(i);
                    if a != arg_reg { self.emit(&format!("    mov {}, {}", arg_reg, a)); }
                }
                self.emit(&format!("    bl {}", target));
                if let Some(d) = dst {
                    let dreg = vreg_to_reg(d);
                    if dreg != "x0" { self.emit(&format!("    mov {}, x0", dreg)); }
                }
            }

            L3Inst::CallPtr { dst, fn_ptr, args, .. } => {
                let ptr = operand_to_reg(fn_ptr);
                for (i, arg) in args.iter().enumerate().take(7) {
                    let a = operand_to_reg(arg);
                    let arg_reg = reg_name(i);
                    if a != arg_reg { self.emit(&format!("    mov {}, {}", arg_reg, a)); }
                }
                self.emit(&format!("    blr {}", ptr));
                if let Some(d) = dst {
                    let dreg = vreg_to_reg(d);
                    if dreg != "x0" { self.emit(&format!("    mov {}, x0", dreg)); }
                }
            }

            L3Inst::DynCall { dst, trait_name, method, args, .. } => {
                let callee = format!("{}_{}", trait_name, method);
                for (i, arg) in args.iter().enumerate().take(8) {
                    let a = operand_to_reg(arg);
                    let arg_reg = reg_name(i);
                    if a != arg_reg { self.emit(&format!("    mov {}, {}", arg_reg, a)); }
                }
                self.emit(&format!("    bl {}", callee));
                if let Some(d) = dst {
                    let dreg = vreg_to_reg(d);
                    if dreg != "x0" { self.emit(&format!("    mov {}, x0", dreg)); }
                }
            }

            L3Inst::MakeDyn { dst, value, .. } => {
                let d = vreg_to_reg(dst);
                let v = operand_to_reg(value);
                if d != v { self.emit(&format!("    mov {}, {}", d, v)); }
            }

            L3Inst::Cast { dst, src, kind, .. } => {
                let d = vreg_to_reg(dst);
                let s = operand_to_reg(src);
                match kind {
                    crate::pir::CastKind::BitCast => { if d != s { self.emit(&format!("    mov {}, {}", d, s)); } }
                    crate::pir::CastKind::IntExtend => self.emit(&format!("    sxtw {}, {}", d, s)),
                    crate::pir::CastKind::IntTrunc => self.emit(&format!("    mov w{}, w{}", d.strip_prefix('x').unwrap_or("0"), s.strip_prefix('x').unwrap_or("0"))),
                    crate::pir::CastKind::IntToFloat => self.emit(&format!("    scvtf d{}, {}", d.strip_prefix('x').unwrap_or("0"), s)),
                    crate::pir::CastKind::FloatToInt => self.emit(&format!("    fcvtzs {}, d{}", d, s.strip_prefix('x').unwrap_or("0"))),
                }
            }

            L3Inst::TryUnwrap { value, ok_dst, err_block, ok_block } => {
                let v = operand_to_reg(value);
                let d = vreg_to_reg(ok_dst);
                self.emit(&format!("    cmp {}, #0", v));
                self.emit(&format!("    b.eq .L{}{}_b{}", func.name, self.label_counter, err_block.0));
                self.emit(&format!("    mov {}, {}", d, v));
                self.emit(&format!("    b .L{}{}_b{}", func.name, self.label_counter, ok_block.0));
            }

            L3Inst::Print { val, .. } => {
                let v = operand_to_reg(val);
                self.emit(&format!("    mov x0, {}", v));
                self.emit("    bl p_print_int");
            }

            L3Inst::Panic { message } => {
                self.emit(&format!("    // panic: {}", message));
                self.emit("    brk #1");
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
        _ => "x9", // scratch register for immediates/slots
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_arm64_empty_module() {
        let module = L3Module::default();
        let asm = generate_asm(&module);
        assert!(asm.contains(".section .text"));
        assert!(asm.contains(".globl main"));
    }

    #[test]
    fn test_arm64_reg_names() {
        assert_eq!(reg_name(0), "x0");
        assert_eq!(reg_name(28), "x28");
    }
}
