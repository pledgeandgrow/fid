// RISC-V (RV64) Instruction Encoder — Cross-Compilation Phase
//
// Assembles RISC-V assembly text into raw machine code bytes.
// RISC-V uses variable-length instructions (4 bytes for base ISA).
//
// Supports the instruction subset produced by codegen_riscv.rs:
//   - Arithmetic: add, sub, mul, div, rem, and, or, xor, sll, sra, slt
//   - Immediate: addi, andi, ori, xori, slli, srai, slti
//   - Pseudo: li, mv, neg, not, seqz, snez, j, call, ret
//   - Load/Store: ld, sd, lw, sw
//   - Branch: beqz, bnez, j, jalr
//   - Compare: slt, slti
//   - Extend: sext.w
//   - Float: fcvt.s.l, fcvt.l.s
//   - System: ebreak, ecall
//
// Calling convention: RISC-V LP64
//   - Args: a0-a7 (x10-x17), Return: a0 (x10)
//   - Callee-saved: s0-s11 (x8-x19), ra (x1), sp (x2)

use std::collections::HashMap;

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

/// Assemble RISC-V assembly text into machine code.
/// Returns (text_bytes, symbols, relocations).
/// reloc_type: 20 = JAL, 21 = branch, 22 = AUIPC+ADDI pair
pub fn assemble(asm: &str) -> Result<(Vec<u8>, Vec<(String, u64)>, Vec<(u64, String, u8)>), String> {
    let mut enc = RiscvAssembler::new();
    enc.assemble(asm)?;
    Ok((enc.text, enc.symbols, enc.relocations))
}

// ═══════════════════════════════════════════════════════════════════════════
// RISC-V Register encoding
// ═══════════════════════════════════════════════════════════════════════════

fn parse_reg(s: &str) -> Result<u8, String> {
    let s = s.trim();
    // ABI names
    match s {
        "zero" => Ok(0), "ra" => Ok(1), "sp" => Ok(2), "gp" => Ok(3),
        "tp" => Ok(4), "t0" => Ok(5), "t1" => Ok(6), "t2" => Ok(7),
        "s0" | "fp" => Ok(8), "s1" => Ok(9),
        "a0" => Ok(10), "a1" => Ok(11), "a2" => Ok(12), "a3" => Ok(13),
        "a4" => Ok(14), "a5" => Ok(15), "a6" => Ok(16), "a7" => Ok(17),
        "s2" => Ok(18), "s3" => Ok(19), "s4" => Ok(20), "s5" => Ok(21),
        "s6" => Ok(22), "s7" => Ok(23), "s8" => Ok(24), "s9" => Ok(25),
        "s10" => Ok(26), "s11" => Ok(27),
        "t3" => Ok(28), "t4" => Ok(29), "t5" => Ok(30), "t6" => Ok(31),
        _ => {
            // x0-x31
            if s.starts_with('x') {
                let n: u8 = s[1..].parse().map_err(|_| format!("Invalid register: {}", s))?;
                if n > 31 { return Err(format!("Register out of range: {}", s)); }
                Ok(n)
            } else {
                Err(format!("Unknown register: {}", s))
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Assembler
// ═══════════════════════════════════════════════════════════════════════════

struct RiscvAssembler {
    text: Vec<u8>,
    rodata: Vec<u8>,
    symbols: Vec<(String, u64)>,
    rodata_symbols: Vec<(String, u64)>,
    relocations: Vec<(u64, String, u8)>,
    current_section: Section,
    label_offsets: HashMap<String, u64>,
    pending_branches: Vec<(u64, String, u8)>, // (text_offset, label, reloc_type)
}

#[derive(PartialEq)]
enum Section {
    Text,
    Rodata,
}

impl RiscvAssembler {
    fn new() -> Self {
        RiscvAssembler {
            text: Vec::new(),
            rodata: Vec::new(),
            symbols: Vec::new(),
            rodata_symbols: Vec::new(),
            relocations: Vec::new(),
            current_section: Section::Text,
            label_offsets: HashMap::new(),
            pending_branches: Vec::new(),
        }
    }

    fn assemble(&mut self, asm: &str) -> Result<(), String> {
        for line in asm.lines() {
            let line = line.trim();
            if line.is_empty() || line.starts_with('#') {
                continue;
            }

            // Strip inline comments (# ...)
            let line = if let Some(pos) = line.find('#') {
                line[..pos].trim()
            } else {
                line
            };
            if line.is_empty() { continue; }

            // Handle labels
            if line.ends_with(':') && !line.starts_with(' ') {
                let label = line[..line.len()-1].trim().to_string();
                self.define_label(&label);
                continue;
            }

            // Handle directives
            if line.starts_with('.') {
                self.handle_directive(line)?;
                continue;
            }

            // Handle instruction
            self.handle_instruction(line)?;
        }

        // Resolve pending branches
        self.resolve_branches()?;

        Ok(())
    }

    fn define_label(&mut self, label: &str) {
        match self.current_section {
            Section::Text => {
                let offset = self.text.len() as u64;
                self.label_offsets.insert(label.to_string(), offset);
                self.symbols.push((label.to_string(), offset));
            }
            Section::Rodata => {
                let offset = self.rodata.len() as u64;
                self.rodata_symbols.push((label.to_string(), offset));
            }
        }
    }

    fn handle_directive(&mut self, line: &str) -> Result<(), String> {
        let parts: Vec<&str> = line.splitn(2, ' ').collect();
        match parts[0] {
            ".section" => {
                if parts.len() > 1 && parts[1].contains(".rodata") {
                    self.current_section = Section::Rodata;
                } else {
                    self.current_section = Section::Text;
                }
            }
            ".globl" | ".global" => {}
            ".asciz" => {
                if parts.len() > 1 {
                    let s = parts[1].trim();
                    if s.starts_with('"') && s.ends_with('"') {
                        let content = &s[1..s.len()-1];
                        let decoded = decode_string(content);
                        self.rodata.extend_from_slice(&decoded);
                        self.rodata.push(0);
                    }
                }
            }
            ".ascii" => {
                if parts.len() > 1 {
                    let s = parts[1].trim();
                    if s.starts_with('"') && s.ends_with('"') {
                        let content = &s[1..s.len()-1];
                        let decoded = decode_string(content);
                        self.rodata.extend_from_slice(&decoded);
                    }
                }
            }
            ".quad" => {
                if parts.len() > 1 {
                    let val: i64 = parts[1].trim().parse()
                        .map_err(|_| format!("Invalid .quad value: {}", parts[1]))?;
                    self.rodata.extend_from_slice(&val.to_le_bytes());
                }
            }
            ".byte" => {
                if parts.len() > 1 {
                    let val: u8 = parts[1].trim().parse()
                        .map_err(|_| format!("Invalid .byte value: {}", parts[1]))?;
                    self.rodata.push(val);
                }
            }
            ".align" | ".p2align" => {
                if parts.len() > 1 {
                    let n: u32 = parts[1].trim().parse().unwrap_or(0);
                    let align = 1usize << n;
                    if self.current_section == Section::Text {
                        while self.text.len() % align != 0 {
                            self.text.extend_from_slice(&0x13u32.to_le_bytes()); // NOP
                        }
                    } else {
                        while self.rodata.len() % align != 0 {
                            self.rodata.push(0);
                        }
                    }
                }
            }
            _ => {}
        }
        Ok(())
    }

    fn handle_instruction(&mut self, line: &str) -> Result<(), String> {
        if self.current_section != Section::Text {
            return Ok(());
        }

        let encoded = self.encode_instruction(line)?;
        let offset = self.text.len() as u64;
        for (rel_label, rel_type) in encoded.relocations {
            self.pending_branches.push((offset, rel_label, rel_type));
        }
        self.text.extend_from_slice(&encoded.bytes);
        Ok(())
    }

    fn resolve_branches(&mut self) -> Result<(), String> {
        for (text_offset, label, rel_type) in &self.pending_branches {
            if let Some(&target_offset) = self.label_offsets.get(label) {
                let delta = target_offset as i64 - *text_offset as i64;
                let idx = *text_offset as usize;
                if idx + 4 <= self.text.len() {
                    let instr = u32::from_le_bytes([
                        self.text[idx], self.text[idx+1], self.text[idx+2], self.text[idx+3]
                    ]);
                    match rel_type {
                        20 => {
                            // JAL: 20-bit immediate, offset = delta
                            let imm = delta as u32;
                            // JAL encoding: imm[20|10:1|11|19:12] | rd | opcode
                            let imm20 = (imm >> 20) & 1;
                            let imm10_1 = (imm >> 1) & 0x3FF;
                            let imm11 = (imm >> 11) & 1;
                            let imm19_12 = (imm >> 12) & 0xFF;
                            let patched = (instr & 0x00000FFF)
                                | (imm20 << 31)
                                | (imm10_1 << 21)
                                | (imm11 << 20)
                                | (imm19_12 << 12);
                            self.text[idx..idx+4].copy_from_slice(&patched.to_le_bytes());
                        }
                        21 => {
                            // Branch: 13-bit immediate (bits [12:1])
                            let imm = delta as u32;
                            let imm12 = (imm >> 12) & 1;
                            let imm10_5 = (imm >> 5) & 0x3F;
                            let imm4_1 = (imm >> 1) & 0xF;
                            let imm11 = (imm >> 11) & 1;
                            let patched = (instr & 0x01FFF07F)
                                | (imm12 << 31)
                                | (imm10_5 << 25)
                                | (imm4_1 << 8)
                                | (imm11 << 7);
                            self.text[idx..idx+4].copy_from_slice(&patched.to_le_bytes());
                        }
                        _ => {}
                    }
                }
            } else {
                // External symbol
                self.relocations.push((*text_offset, label.clone(), *rel_type));
            }
        }
        Ok(())
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Instruction encoding
    // ═══════════════════════════════════════════════════════════════════════════

    fn encode_instruction(&self, line: &str) -> Result<EncodedInst, String> {
        let line = line.trim();
        let (mnemonic, rest) = split_mnemonic(line);
        let operands = parse_operands(rest);

        match mnemonic {
            // ── R-type arithmetic ──
            "add" => self.enc_r_type(&operands, 0x33, 0x00, 0x00),
            "sub" => self.enc_r_type(&operands, 0x33, 0x20, 0x00),
            "mul" => self.enc_r_type(&operands, 0x33, 0x01, 0x00),
            "div" => self.enc_r_type(&operands, 0x33, 0x01, 0x04),
            "rem" => self.enc_r_type(&operands, 0x33, 0x01, 0x06),
            "and" => self.enc_r_type(&operands, 0x33, 0x00, 0x07),
            "or" => self.enc_r_type(&operands, 0x33, 0x00, 0x06),
            "xor" => self.enc_r_type(&operands, 0x33, 0x00, 0x04),
            "sll" => self.enc_r_type(&operands, 0x33, 0x00, 0x01),
            "sra" => self.enc_r_type(&operands, 0x33, 0x20, 0x05),
            "srl" => self.enc_r_type(&operands, 0x33, 0x00, 0x05),
            "slt" => self.enc_r_type(&operands, 0x33, 0x00, 0x02),

            // ── I-type arithmetic ──
            "addi" => self.enc_i_type(&operands, 0x13, 0x00),
            "andi" => self.enc_i_type(&operands, 0x13, 0x07),
            "ori" => self.enc_i_type(&operands, 0x13, 0x06),
            "xori" => self.enc_i_type(&operands, 0x13, 0x04),
            "slti" => self.enc_i_type(&operands, 0x13, 0x02),
            "slli" => self.enc_shift_imm(&operands, 0x13, 0x01, false),
            "srai" => self.enc_shift_imm(&operands, 0x13, 0x05, true),
            "srli" => self.enc_shift_imm(&operands, 0x13, 0x05, false),

            // ── Load/Store ──
            "ld" => self.enc_load(&operands, 0x03, 0x03),   // ld: funct3=011
            "lw" => self.enc_load(&operands, 0x03, 0x02),   // lw: funct3=010
            "lwu" => self.enc_load(&operands, 0x03, 0x06),  // lwu: funct3=110
            "lb" => self.enc_load(&operands, 0x03, 0x00),
            "sd" => self.enc_store(&operands, 0x23, 0x03),
            "sw" => self.enc_store(&operands, 0x23, 0x02),
            "sb" => self.enc_store(&operands, 0x23, 0x00),

            // ── Branches ──
            "beq" => self.enc_branch(&operands, 0x63, 0x00),
            "bne" => self.enc_branch(&operands, 0x63, 0x01),
            "blt" => self.enc_branch(&operands, 0x63, 0x04),
            "bge" => self.enc_branch(&operands, 0x63, 0x05),
            "beqz" => self.enc_branch_zero(&operands, 0x63, 0x00),
            "bnez" => self.enc_branch_zero(&operands, 0x63, 0x01),
            "bgez" => self.enc_branch_zero_rs(&operands, 0x63, 0x05), // BGE x0, rs
            "bltz" => self.enc_branch_zero_rs2(&operands, 0x63, 0x04), // BLT rs, x0
            "bgtz" => self.enc_branch_zero_rs2(&operands, 0x63, 0x05), // BGE x0, rs (inverted)
            "blez" => self.enc_branch_zero_rs(&operands, 0x63, 0x04), // BLT x0, rs

            // ── Jump ──
            "j" => self.enc_j(&operands),
            "jal" => self.enc_jal(&operands),
            "jalr" => self.enc_jalr(&operands),
            "call" => self.enc_call(&operands),
            "ret" => Ok(EncodedInst::bytes(0x00008067)), // JALR x0, 0(ra)

            // ── Pseudo ──
            "li" => self.enc_li(&operands),
            "lui" => self.enc_lui(&operands),
            "mv" => self.enc_mv(&operands),
            "neg" => self.enc_neg(&operands),
            "not" => self.enc_not(&operands),
            "seqz" => self.enc_seqz(&operands),
            "snez" => self.enc_snez(&operands),
            "sext.w" => self.enc_sextw(&operands),

            // ── System ──
            "ecall" => Ok(EncodedInst::bytes(0x00000073)),
            "ebreak" => Ok(EncodedInst::bytes(0x00100073)),
            "nop" => Ok(EncodedInst::bytes(0x00000013)), // ADDI x0, x0, 0

            // ── Float (stubs — would need FPU support) ──
            "fcvt.s.l" => self.enc_fcvt_s_l(&operands),
            "fcvt.l.s" => self.enc_fcvt_l_s(&operands),

            _ => Err(format!("Unknown RISC-V instruction: {}", mnemonic)),
        }
    }

    // ── R-type: funct3, funct7, opcode ──
    fn enc_r_type(&self, ops: &[String], opcode: u32, funct7: u32, funct3: u32) -> Result<EncodedInst, String> {
        if ops.len() < 3 { return Err(format!("R-type needs 3 operands, got {}", ops.len())); }
        let rd = parse_reg(&ops[0])? as u32;
        let rs1 = parse_reg(&ops[1])? as u32;
        let rs2 = parse_reg(&ops[2])? as u32;
        let inst = (funct7 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;
        Ok(EncodedInst::bytes(inst))
    }

    // ── I-type: imm[11:0], rs1, funct3, rd, opcode ──
    fn enc_i_type(&self, ops: &[String], opcode: u32, funct3: u32) -> Result<EncodedInst, String> {
        if ops.len() < 3 { return Err(format!("I-type needs 3 operands, got {}", ops.len())); }
        let rd = parse_reg(&ops[0])? as u32;
        let rs1 = parse_reg(&ops[1])? as u32;
        let imm: i32 = parse_imm(&ops[2])?;
        let imm12 = (imm as u32) & 0xFFF;
        let inst = (imm12 << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;
        Ok(EncodedInst::bytes(inst))
    }

    // ── Shift immediate (SLLI/SRAI/SRLI) ──
    fn enc_shift_imm(&self, ops: &[String], opcode: u32, funct3: u32, is_arith: bool) -> Result<EncodedInst, String> {
        if ops.len() < 3 { return Err(format!("Shift needs 3 operands, got {}", ops.len())); }
        let rd = parse_reg(&ops[0])? as u32;
        let rs1 = parse_reg(&ops[1])? as u32;
        let shamt: u32 = parse_imm(&ops[2])? as u32 & 0x3F;
        let funct7 = if is_arith { 0x20 } else { 0x00 };
        let inst = (funct7 << 25) | (shamt << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;
        Ok(EncodedInst::bytes(inst))
    }

    // ── Load: ld rd, imm(rs1) ──
    fn enc_load(&self, ops: &[String], opcode: u32, funct3: u32) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("Load needs 2 operands".into()); }
        let rd = parse_reg(&ops[0])? as u32;
        let (imm, rs1_str) = parse_mem_operand(&ops[1])?;
        let rs1 = parse_reg(&rs1_str)? as u32;
        let imm12 = (imm as u32) & 0xFFF;
        let inst = (imm12 << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;
        Ok(EncodedInst::bytes(inst))
    }

    // ── Store: sd rs2, imm(rs1) ──
    fn enc_store(&self, ops: &[String], opcode: u32, funct3: u32) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("Store needs 2 operands".into()); }
        let rs2 = parse_reg(&ops[0])? as u32;
        let (imm, rs1_str) = parse_mem_operand(&ops[1])?;
        let rs1 = parse_reg(&rs1_str)? as u32;
        let imm = imm as u32;
        let imm11_5 = (imm >> 5) & 0x7F;
        let imm4_0 = imm & 0x1F;
        let inst = (imm11_5 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (imm4_0 << 7) | opcode;
        Ok(EncodedInst::bytes(inst))
    }

    // ── Branch: beq rs1, rs2, label ──
    fn enc_branch(&self, ops: &[String], opcode: u32, funct3: u32) -> Result<EncodedInst, String> {
        if ops.len() < 3 { return Err("Branch needs 3 operands".into()); }
        let rs1 = parse_reg(&ops[0])? as u32;
        let rs2 = parse_reg(&ops[1])? as u32;
        let label = &ops[2];
        let inst = (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | opcode;
        Ok(EncodedInst::with_reloc(inst, label, 21))
    }

    // ── Branch zero: beqz rs, label (beq rs, x0, label) ──
    fn enc_branch_zero(&self, ops: &[String], opcode: u32, funct3: u32) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("Branch-zero needs 2 operands".into()); }
        let rs1 = parse_reg(&ops[0])? as u32;
        let label = &ops[1];
        let inst = (0u32 << 20) | (rs1 << 15) | (funct3 << 12) | opcode;
        Ok(EncodedInst::with_reloc(inst, label, 21))
    }

    // ── bgez rs, label = bge x0, rs, label ──
    fn enc_branch_zero_rs(&self, ops: &[String], opcode: u32, funct3: u32) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("bgez needs 2 operands".into()); }
        let rs = parse_reg(&ops[0])? as u32;
        let label = &ops[1];
        // bge x0, rs: rs1=0, rs2=rs
        let inst = (rs << 20) | (0u32 << 15) | (funct3 << 12) | opcode;
        Ok(EncodedInst::with_reloc(inst, label, 21))
    }

    // ── bltz rs, label = blt rs, x0, label ──
    fn enc_branch_zero_rs2(&self, ops: &[String], opcode: u32, funct3: u32) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("bltz needs 2 operands".into()); }
        let rs = parse_reg(&ops[0])? as u32;
        let label = &ops[1];
        // blt rs, x0: rs1=rs, rs2=0
        let inst = (0u32 << 20) | (rs << 15) | (funct3 << 12) | opcode;
        Ok(EncodedInst::with_reloc(inst, label, 21))
    }

    // ── J (pseudo: JAL x0, label) ──
    fn enc_j(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.is_empty() { return Err("J needs 1 operand".into()); }
        let label = &ops[0];
        // JAL x0, offset: opcode=0x6F, rd=0
        let inst = 0x6Fu32; // JAL x0, 0
        Ok(EncodedInst::with_reloc(inst, label, 20))
    }

    // ── JAL ──
    fn enc_jal(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.is_empty() { return Err("JAL needs 1 operand".into()); }
        if ops.len() == 1 {
            // JAL label (rd=ra implied? No, JAL without rd is pseudo for JAL ra, label)
            let label = &ops[0];
            let inst = 0x6Fu32 | (1u32 << 7); // JAL ra, 0
            return Ok(EncodedInst::with_reloc(inst, label, 20));
        }
        // JAL rd, label
        let rd = parse_reg(&ops[0])? as u32;
        let label = &ops[1];
        let inst = 0x6Fu32 | (rd << 7);
        Ok(EncodedInst::with_reloc(inst, label, 20))
    }

    // ── JALR ──
    fn enc_jalr(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.is_empty() { return Err("JALR needs 1 operand".into()); }
        if ops.len() == 1 {
            // JALR rs (pseudo: JALR x0, 0(rs))
            // Actually JALR rs = JALR ra, 0(rs) for call
            // But "jalr ptr" in our codegen means "jalr x0, 0(ptr)" (tail call)
            let rs1 = parse_reg(&ops[0])? as u32;
            let inst = 0x67u32 | (rs1 << 15); // JALR x0, 0(rs)
            return Ok(EncodedInst::bytes(inst));
        }
        // JALR rd, rs1 or JALR rd, imm(rs1)
        let rd = parse_reg(&ops[0])? as u32;
        if ops.len() >= 2 {
            if ops[1].contains('(') {
                let (imm, rs1_str) = parse_mem_operand(&ops[1])?;
                let rs1 = parse_reg(&rs1_str)? as u32;
                let inst = ((imm as u32 & 0xFFF) << 20) | (rs1 << 15) | (rd << 7) | 0x67;
                return Ok(EncodedInst::bytes(inst));
            } else {
                let rs1 = parse_reg(&ops[1])? as u32;
                let inst = (rs1 << 15) | (rd << 7) | 0x67;
                return Ok(EncodedInst::bytes(inst));
            }
        }
        Err("JALR: invalid operands".into())
    }

    // ── CALL (pseudo: AUIPC + JALR) ──
    fn enc_call(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.is_empty() { return Err("CALL needs 1 operand".into()); }
        let label = &ops[0];
        // CALL = AUIPC ra, 0 + JALR ra, 0(ra)
        // But for simplicity, use JAL ra, label (if within range)
        // For external calls, emit AUIPC + JALR with relocation
        // Actually, let's use JAL for now (20-bit range = ±1MB)
        let inst = 0x6Fu32 | (1u32 << 7); // JAL ra, 0
        Ok(EncodedInst::with_reloc(inst, label, 20))
    }

    // ── LI (load immediate) ──
    fn enc_li(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("LI needs 2 operands".into()); }
        let rd = parse_reg(&ops[0])? as u32;
        let imm: i64 = parse_imm_i64(&ops[1])?;

        if imm >= -2048 && imm <= 2047 {
            // ADDI rd, x0, imm
            let inst = ((imm as u32 & 0xFFF) << 20) | (0u32 << 15) | (0u32 << 12) | (rd << 7) | 0x13;
            Ok(EncodedInst::bytes(inst))
        } else if imm >= 0 && imm <= 0xFFFF_FFFF_FFFF {
            // LUI + ADDI
            let hi = ((imm + 0x800) >> 12) as u32 & 0xFFFFF;
            let lo = (imm as u32) & 0xFFF;
            let lui = (hi << 12) | (rd << 7) | 0x37;
            let addi = (lo << 20) | (rd << 15) | (0u32 << 12) | (rd << 7) | 0x13;
            let mut bytes = Vec::new();
            bytes.extend_from_slice(&lui.to_le_bytes());
            bytes.extend_from_slice(&addi.to_le_bytes());
            Ok(EncodedInst { bytes, relocations: Vec::new() })
        } else {
            // Negative large: LUI + ADDI with sign extension
            let imm_u = imm as u64;
            let hi = ((imm_u as i64 + 0x800) >> 12) as u32 & 0xFFFFF;
            let lo = (imm_u as u32) & 0xFFF;
            let lui = (hi << 12) | (rd << 7) | 0x37;
            let addi = (lo << 20) | (rd << 15) | (0u32 << 12) | (rd << 7) | 0x13;
            let mut bytes = Vec::new();
            bytes.extend_from_slice(&lui.to_le_bytes());
            bytes.extend_from_slice(&addi.to_le_bytes());
            Ok(EncodedInst { bytes, relocations: Vec::new() })
        }
    }

    // ── LUI ──
    fn enc_lui(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("LUI needs 2 operands".into()); }
        let rd = parse_reg(&ops[0])? as u32;
        let imm: u32 = parse_imm(&ops[1])? as u32 & 0xFFFFF;
        let inst = (imm << 12) | (rd << 7) | 0x37;
        Ok(EncodedInst::bytes(inst))
    }

    // ── MV (pseudo: ADDI rd, rs, 0) ──
    fn enc_mv(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("MV needs 2 operands".into()); }
        let rd = parse_reg(&ops[0])? as u32;
        let rs1 = parse_reg(&ops[1])? as u32;
        let inst = (0u32 << 20) | (rs1 << 15) | (0u32 << 12) | (rd << 7) | 0x13;
        Ok(EncodedInst::bytes(inst))
    }

    // ── NEG (pseudo: SUB rd, x0, rs) ──
    fn enc_neg(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("NEG needs 2 operands".into()); }
        let rd = parse_reg(&ops[0])? as u32;
        let rs = parse_reg(&ops[1])? as u32;
        // SUB rd, x0, rs: funct7=0x20, rs2=rs, rs1=x0, funct3=0, rd, opcode=0x33
        let inst = (0x20u32 << 25) | (rs << 20) | (0u32 << 15) | (0u32 << 12) | (rd << 7) | 0x33;
        Ok(EncodedInst::bytes(inst))
    }

    // ── NOT (pseudo: XORI rd, rs, -1) ──
    fn enc_not(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("NOT needs 2 operands".into()); }
        let rd = parse_reg(&ops[0])? as u32;
        let rs = parse_reg(&ops[1])? as u32;
        // XORI rd, rs, -1: imm=0xFFF, funct3=4, opcode=0x13
        let inst = (0xFFFu32 << 20) | (rs << 15) | (4u32 << 12) | (rd << 7) | 0x13;
        Ok(EncodedInst::bytes(inst))
    }

    // ── SEQZ (pseudo: SLTIU rd, rs, 1) ──
    fn enc_seqz(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("SEQZ needs 2 operands".into()); }
        let rd = parse_reg(&ops[0])? as u32;
        let rs = parse_reg(&ops[1])? as u32;
        // SLTIU rd, rs, 1: imm=1, funct3=3, opcode=0x13
        let inst = (1u32 << 20) | (rs << 15) | (3u32 << 12) | (rd << 7) | 0x13;
        Ok(EncodedInst::bytes(inst))
    }

    // ── SNEZ (pseudo: SLTU rd, x0, rs) ──
    fn enc_snez(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("SNEZ needs 2 operands".into()); }
        let rd = parse_reg(&ops[0])? as u32;
        let rs = parse_reg(&ops[1])? as u32;
        // SLTU rd, x0, rs: funct3=3, opcode=0x33
        let inst = (rs << 20) | (0u32 << 15) | (3u32 << 12) | (rd << 7) | 0x33;
        Ok(EncodedInst::bytes(inst))
    }

    // ── SEXT.W (pseudo: ADDIW rd, rs, 0) ──
    fn enc_sextw(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("SEXT.W needs 2 operands".into()); }
        let rd = parse_reg(&ops[0])? as u32;
        let rs = parse_reg(&ops[1])? as u32;
        // ADDIW rd, rs, 0: imm=0, funct3=0, opcode=0x1B (RV64I)
        let inst = (0u32 << 20) | (rs << 15) | (0u32 << 12) | (rd << 7) | 0x1B;
        Ok(EncodedInst::bytes(inst))
    }

    // ── FCVT.S.L (int to float) ──
    fn enc_fcvt_s_l(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("FCVT.S.L needs 2 operands".into()); }
        let rd = parse_reg(&ops[0])? as u32;
        let rs1 = parse_reg(&ops[1])? as u32;
        // FCVT.S.L: funct7=0x60, rs2=0x02, funct3=0x00, opcode=0x53
        let inst = (0x60u32 << 25) | (0x02u32 << 20) | (rs1 << 15) | (0u32 << 12) | (rd << 7) | 0x53;
        Ok(EncodedInst::bytes(inst))
    }

    // ── FCVT.L.S (float to int) ──
    fn enc_fcvt_l_s(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("FCVT.L.S needs 2 operands".into()); }
        let rd = parse_reg(&ops[0])? as u32;
        let rs1 = parse_reg(&ops[1])? as u32;
        // FCVT.L.S: funct7=0x60, rs2=0x02, funct3=0x00, rm=0x00, opcode=0x53
        // Actually: FCVT.L.S rd, rs1, rm: funct7=0x60, rs2=0x02, opcode=0x53
        let inst = (0x60u32 << 25) | (0x02u32 << 20) | (rs1 << 15) | (0u32 << 12) | (rd << 7) | 0x53;
        Ok(EncodedInst::bytes(inst))
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Helper types and functions
// ═══════════════════════════════════════════════════════════════════════════

struct EncodedInst {
    bytes: Vec<u8>,
    relocations: Vec<(String, u8)>,
}

impl EncodedInst {
    fn bytes(inst: u32) -> Self {
        EncodedInst { bytes: inst.to_le_bytes().to_vec(), relocations: Vec::new() }
    }
    fn with_reloc(inst: u32, label: &str, reloc_type: u8) -> Self {
        EncodedInst {
            bytes: inst.to_le_bytes().to_vec(),
            relocations: vec![(label.to_string(), reloc_type)],
        }
    }
}

fn parse_imm(s: &str) -> Result<i32, String> {
    let s = s.trim();
    let s = s.strip_prefix('#').unwrap_or(s);
    // Handle hex
    if s.starts_with("0x") || s.starts_with("-0x") {
        let neg = s.starts_with('-');
        let hex = if neg { &s[1..] } else { s };
        let val = i64::from_str_radix(&hex[2..], 16).map_err(|_| format!("Invalid hex: {}", s))?;
        Ok(if neg { -val } else { val } as i32)
    } else {
        s.parse().map_err(|_| format!("Invalid immediate: {}", s))
    }
}

fn parse_imm_i64(s: &str) -> Result<i64, String> {
    let s = s.trim();
    let s = s.strip_prefix('#').unwrap_or(s);
    if s.starts_with("0x") || s.starts_with("-0x") {
        let neg = s.starts_with('-');
        let hex = if neg { &s[1..] } else { s };
        let val = i64::from_str_radix(&hex[2..], 16).map_err(|_| format!("Invalid hex: {}", s))?;
        Ok(if neg { -val } else { val })
    } else {
        s.parse().map_err(|_| format!("Invalid immediate: {}", s))
    }
}

fn parse_mem_operand(s: &str) -> Result<(i32, String), String> {
    // Format: imm(reg) or just reg (imm=0)
    let s = s.trim();
    if let Some(open) = s.find('(') {
        let close = s.find(')').ok_or("Missing ) in memory operand")?;
        let imm_str = s[..open].trim();
        let reg = s[open+1..close].trim();
        let imm = if imm_str.is_empty() { 0 } else { parse_imm(imm_str)? };
        Ok((imm, reg.to_string()))
    } else {
        Ok((0, s.to_string()))
    }
}

fn split_mnemonic(line: &str) -> (&str, &str) {
    match line.find(char::is_whitespace) {
        Some(pos) => (&line[..pos], line[pos..].trim()),
        None => (line, ""),
    }
}

fn parse_operands(s: &str) -> Vec<String> {
    if s.is_empty() { return Vec::new(); }
    let mut result = Vec::new();
    let mut current = String::new();
    let mut paren_depth = 0;

    for c in s.chars() {
        match c {
            '(' => { paren_depth += 1; current.push(c); }
            ')' => { paren_depth -= 1; current.push(c); }
            ',' if paren_depth == 0 => {
                result.push(current.trim().to_string());
                current.clear();
            }
            _ => current.push(c),
        }
    }
    if !current.trim().is_empty() {
        result.push(current.trim().to_string());
    }
    result
}

fn decode_string(s: &str) -> Vec<u8> {
    let mut result = Vec::new();
    let mut chars = s.chars().peekable();
    while let Some(c) = chars.next() {
        if c == '\\' {
            match chars.next() {
                Some('n') => result.push(b'\n'),
                Some('t') => result.push(b'\t'),
                Some('r') => result.push(b'\r'),
                Some('0') => result.push(0),
                Some('\\') => result.push(b'\\'),
                Some('"') => result.push(b'"'),
                Some('\'') => result.push(b'\''),
                Some(c) => { result.push(b'\\'); result.push(c as u8); }
                None => result.push(b'\\'),
            }
        } else {
            result.push(c as u8);
        }
    }
    result
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_encode_ret() {
        let (text, _, _) = assemble("ret").unwrap();
        // JALR x0, 0(ra) = 0x00008067
        assert_eq!(text, vec![0x67, 0x80, 0x00, 0x00]);
    }

    #[test]
    fn test_encode_nop() {
        let (text, _, _) = assemble("nop").unwrap();
        // ADDI x0, x0, 0 = 0x00000013
        assert_eq!(text, vec![0x13, 0x00, 0x00, 0x00]);
    }

    #[test]
    fn test_encode_add() {
        // ADD a0, a1, a2 → x10 = x11 + x12
        let (text, _, _) = assemble("add a0, a1, a2").unwrap();
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // R-type: funct7=0, rs2=12, rs1=11, funct3=0, rd=10, opcode=0x33
        // = (12 << 20) | (11 << 15) | (0 << 12) | (10 << 7) | 0x33
        // = 0x00C58533
        assert_eq!(inst, 0x00C58533);
    }

    #[test]
    fn test_encode_addi() {
        // ADDI sp, sp, -16
        let (text, _, _) = assemble("addi sp, sp, -16").unwrap();
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // I-type: imm=-16=0xFF0, rs1=2(sp), funct3=0, rd=2(sp), opcode=0x13
        // = (0xFF0 << 20) | (2 << 15) | (0 << 12) | (2 << 7) | 0x13
        // = 0xFF010113
        assert_eq!(inst, 0xFF010113);
    }

    #[test]
    fn test_encode_li() {
        // LI a0, 42
        let (text, _, _) = assemble("li a0, 42").unwrap();
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // ADDI a0, x0, 42 = (42 << 20) | (0 << 15) | (0 << 12) | (10 << 7) | 0x13
        // = 0x02A00513
        assert_eq!(inst, 0x02A00513);
    }

    #[test]
    fn test_encode_mv() {
        // MV a0, a1 → ADDI a0, a1, 0
        let (text, _, _) = assemble("mv a0, a1").unwrap();
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // (0 << 20) | (11 << 15) | (0 << 12) | (10 << 7) | 0x13
        // = 0x00058513
        assert_eq!(inst, 0x00058513);
    }

    #[test]
    fn test_encode_sd() {
        // SD ra, 8(sp)
        let (text, _, _) = assemble("sd ra, 8(sp)").unwrap();
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // S-type: imm[11:5]=0, rs2=1(ra), rs1=2(sp), funct3=3, imm[4:0]=8, opcode=0x23
        // = (0 << 25) | (1 << 20) | (2 << 15) | (3 << 12) | (8 << 7) | 0x23
        // = 0x00113423 ... wait let me recalculate
        // imm=8: imm[11:5]=0, imm[4:0]=8
        // = (0 << 25) | (1 << 20) | (2 << 15) | (3 << 12) | (8 << 7) | 0x23
        // = 0x00113... let me compute: 0b 0000000 00001 00010 011 01000 0110011
        // = 0x00000000 | (1 << 20) | (2 << 15) | (3 << 12) | (8 << 7) | 0x23
        // = 0x001000 + 0x10000 + 0x3000 + 0x400 + 0x23 = 0x00113423
        assert_eq!(inst, 0x00113423);
    }

    #[test]
    fn test_encode_ld() {
        // LD a0, 0(sp)
        let (text, _, _) = assemble("ld a0, 0(sp)").unwrap();
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // I-type: imm=0, rs1=2(sp), funct3=3, rd=10(a0), opcode=0x03
        // = (0 << 20) | (2 << 15) | (3 << 12) | (10 << 7) | 0x03
        // = 0x00013503
        assert_eq!(inst, 0x00013503);
        // CALL p_print_int — external, should produce relocation
        let (_, _, relocs) = assemble("call p_print_int").unwrap();
        assert_eq!(relocs.len(), 1);
        assert_eq!(relocs[0].1, "p_print_int");
        assert_eq!(relocs[0].2, 20); // JAL reloc type
    }

    #[test]
    fn test_encode_j_label() {
        // J .Llabel — local label
        let asm = ".Llabel:\n    j .Llabel";
        let (text, _, relocs) = assemble(asm).unwrap();
        assert!(relocs.is_empty());
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // JAL x0, 0 = 0x0000006F
        assert_eq!(inst, 0x0000006F);
    }

    #[test]
    fn test_encode_ebreak() {
        let (text, _, _) = assemble("ebreak").unwrap();
        assert_eq!(text, vec![0x73, 0x00, 0x10, 0x00]);
    }

    #[test]
    fn test_encode_ecall() {
        let (text, _, _) = assemble("ecall").unwrap();
        assert_eq!(text, vec![0x73, 0x00, 0x00, 0x00]);
    }

    #[test]
    fn test_encode_mul() {
        // MUL a0, a1, a2
        let (text, _, _) = assemble("mul a0, a1, a2").unwrap();
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // R-type: funct7=1, rs2=12, rs1=11, funct3=0, rd=10, opcode=0x33
        // = (1 << 25) | (12 << 20) | (11 << 15) | (0 << 12) | (10 << 7) | 0x33
        // = 0x02C58533
        assert_eq!(inst, 0x02C58533);
    }

    #[test]
    fn test_encode_div() {
        // DIV a0, a1, a2
        let (text, _, _) = assemble("div a0, a1, a2").unwrap();
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // R-type: funct7=1, rs2=12, rs1=11, funct3=4, rd=10, opcode=0x33
        // = (1 << 25) | (12 << 20) | (11 << 15) | (4 << 12) | (10 << 7) | 0x33
        // = 0x02C5C533
        assert_eq!(inst, 0x02C5C533);
    }

    #[test]
    fn test_full_function() {
        let asm = r#"
    .section .text
    .globl main
main:
    sd ra, 8(sp)
    sd s0, 0(sp)
    addi sp, sp, -16
    mv s0, sp
    li a0, 42
    call p_print_int
    li a0, 0
    addi sp, sp, 16
    ld ra, 8(sp)
    ld s0, 0(sp)
    ret
"#;
        let (text, syms, relocs) = assemble(asm).unwrap();
        assert!(!text.is_empty());
        assert!(syms.iter().any(|(n, _)| n == "main"));
        assert!(relocs.iter().any(|(_, n, _)| n == "p_print_int"));
        // All instructions should be 4 bytes
        assert_eq!(text.len() % 4, 0);
    }
}
