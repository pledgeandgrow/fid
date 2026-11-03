// ARM64 (AArch64) Instruction Encoder — Cross-Compilation Phase
//
// Assembles ARM64 assembly text into raw machine code bytes.
// All ARM64 instructions are fixed 4 bytes (little-endian).
//
// Supports the instruction subset produced by codegen_arm64.rs:
//   - Data processing: mov, add, sub, mul, sdiv, msub, and, orr, eor, lsl, asr, neg, mvn
//   - Compare: cmp (reg, imm)
//   - Conditional set: cset
//   - Load/Store: ldr, str, stp, ldp
//   - Branches: b, bl, blr, b.cond, brk, ret
//   - Immediate: movz, movk
//   - Float: scvtf, fcvtzs
//   - Extend: sxtw, mov w
//   - Directives: .section, .globl, .asciz, .quad, labels
//
// Calling convention: AAPCS64
//   - Args: X0-X7, Return: X0
//   - Callee-saved: X19-X28, SP
//   - Stack 16-byte aligned

use std::collections::HashMap;

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

/// Assemble ARM64 assembly text into machine code.
/// Returns (text_bytes, symbols, relocations).
/// symbols: Vec<(name, offset_in_text)>
/// relocations: Vec<(offset_in_text, symbol_name, reloc_type)>
///   reloc_type: 10 = BL, 11 = B, 12 = B.cond, 13 = ADRP+ADD pair
pub fn assemble(asm: &str) -> Result<(Vec<u8>, Vec<(String, u64)>, Vec<(u64, String, u8)>), String> {
    let mut enc = Arm64Assembler::new();
    enc.assemble(asm)?;
    Ok((enc.text, enc.symbols, enc.relocations))
}

// ═══════════════════════════════════════════════════════════════════════════
// ARM64 Register encoding
// ═══════════════════════════════════════════════════════════════════════════

fn parse_reg(s: &str) -> Result<u8, String> {
    let s = s.trim();
    match s {
        "sp" | "SP" => Ok(31),
        "xzr" | "XZR" | "wzr" | "WZR" => Ok(31),
        _ => {
            if s.starts_with('x') || s.starts_with('X') {
                let n: u8 = s[1..].parse().map_err(|_| format!("Invalid register: {}", s))?;
                if n > 30 { return Err(format!("Register out of range: {}", s)); }
                Ok(n)
            } else if s.starts_with('w') || s.starts_with('W') {
                let n: u8 = s[1..].parse().map_err(|_| format!("Invalid register: {}", s))?;
                if n > 30 { return Err(format!("Register out of range: {}", s)); }
                Ok(n)
            } else {
                Err(format!("Unknown register: {}", s))
            }
        }
    }
}

fn is_64bit_reg(s: &str) -> bool {
    s.starts_with('x') || s.starts_with('X') || s == "sp" || s == "SP"
}

// Condition codes for B.cond / CSET
fn parse_cond(s: &str) -> Result<u8, String> {
    match s {
        "eq" => Ok(0), "ne" => Ok(1), "cs" | "hs" => Ok(2), "cc" | "lo" => Ok(3),
        "mi" => Ok(4), "pl" => Ok(5), "vs" => Ok(6), "vc" => Ok(7),
        "hi" => Ok(8), "ls" => Ok(9), "ge" => Ok(10), "lt" => Ok(11),
        "gt" => Ok(12), "le" => Ok(13), "al" => Ok(14), "nv" => Ok(15),
        _ => Err(format!("Unknown condition: {}", s)),
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Assembler
// ═══════════════════════════════════════════════════════════════════════════

struct Arm64Assembler {
    text: Vec<u8>,
    rodata: Vec<u8>,
    symbols: Vec<(String, u64)>,
    rodata_symbols: Vec<(String, u64)>,
    relocations: Vec<(u64, String, u8)>,
    current_section: Section,
    pending_labels: Vec<String>,
    // For resolving labels within text
    label_offsets: HashMap<String, u64>,
    // For resolving forward references (branches)
    pending_branches: Vec<(u64, String, u8)>, // (text_offset, label, reloc_type)
}

#[derive(PartialEq)]
enum Section {
    Text,
    Rodata,
}

impl Arm64Assembler {
    fn new() -> Self {
        Arm64Assembler {
            text: Vec::new(),
            rodata: Vec::new(),
            symbols: Vec::new(),
            rodata_symbols: Vec::new(),
            relocations: Vec::new(),
            current_section: Section::Text,
            pending_labels: Vec::new(),
            label_offsets: HashMap::new(),
            pending_branches: Vec::new(),
        }
    }

    fn assemble(&mut self, asm: &str) -> Result<(), String> {
        for line in asm.lines() {
            let line = line.trim();
            if line.is_empty() || line.starts_with("//") || line.starts_with('#') {
                continue;
            }

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
            // Strip inline comments (// ...)
            let line = if let Some(pos) = line.find("//") {
                line[..pos].trim()
            } else {
                line
            };
            if line.is_empty() { continue; }
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
                // If it's a global label, add to symbols
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
            ".globl" | ".global" => {
                // Mark as global — we already add all labels to symbols
            }
            ".asciz" => {
                // .asciz "string" — null-terminated string
                if parts.len() > 1 {
                    let s = parts[1].trim();
                    if s.starts_with('"') && s.ends_with('"') {
                        let content = &s[1..s.len()-1];
                        let decoded = decode_string(content);
                        self.rodata.extend_from_slice(&decoded);
                        self.rodata.push(0); // null terminator
                    }
                }
            }
            ".ascii" => {
                // .ascii "string" — no null terminator
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
                // .quad value — 8-byte value
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
                // Align to power of 2
                if parts.len() > 1 {
                    let n: u32 = parts[1].trim().parse().unwrap_or(0);
                    let align = 1usize << n;
                    if self.current_section == Section::Text {
                        while self.text.len() % align != 0 {
                            self.text.push(0);
                        }
                    } else {
                        while self.rodata.len() % align != 0 {
                            self.rodata.push(0);
                        }
                    }
                }
            }
            _ => {} // Ignore unknown directives
        }
        Ok(())
    }

    fn handle_instruction(&mut self, line: &str) -> Result<(), String> {
        // Only encode in text section
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
                        10 => {
                            // BL: 26-bit immediate, offset = delta/4
                            let imm26 = ((delta / 4) as u32) & 0x03FFFFFF;
                            let patched = (instr & 0xFC000000) | imm26;
                            self.text[idx..idx+4].copy_from_slice(&patched.to_le_bytes());
                        }
                        11 => {
                            // B: 26-bit immediate, offset = delta/4
                            let imm26 = ((delta / 4) as u32) & 0x03FFFFFF;
                            let patched = (instr & 0xFC000000) | imm26;
                            self.text[idx..idx+4].copy_from_slice(&patched.to_le_bytes());
                        }
                        12 => {
                            // B.cond: 19-bit immediate, offset = delta/4
                            let imm19 = ((delta / 4) as u32) & 0x0007FFFF;
                            let patched = (instr & 0xFF00001F) | (imm19 << 5);
                            self.text[idx..idx+4].copy_from_slice(&patched.to_le_bytes());
                        }
                        _ => {}
                    }
                }
            } else {
                // External symbol — add to relocations for linker
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
        // Parse mnemonic and operands
        let (mnemonic, rest) = split_mnemonic(line);
        let operands = parse_operands(rest);

        // Handle b.cond (b.eq, b.ne, b.gt, etc.)
        if mnemonic.starts_with("b.") {
            let cond = &mnemonic[2..];
            return self.enc_bcond(cond, &operands);
        }

        match mnemonic {
            // ── Stack frame ──
            "stp" => self.enc_stp(&operands),
            "ldp" => self.enc_ldp(&operands),

            // ── Move ──
            "mov" => self.enc_mov(&operands),
            "movz" => self.enc_movz(&operands),
            "movk" => self.enc_movk(&operands),

            // ── Arithmetic ──
            "add" => self.enc_add(&operands),
            "sub" => self.enc_sub(&operands),
            "mul" => self.enc_mul(&operands),
            "sdiv" => self.enc_sdiv(&operands),
            "udiv" => self.enc_udiv(&operands),
            "msub" => self.enc_msub(&operands),
            "neg" => self.enc_neg(&operands),

            // ── Logical ──
            "and" => self.enc_and(&operands),
            "orr" => self.enc_orr(&operands),
            "eor" => self.enc_eor(&operands),
            "mvn" => self.enc_mvn(&operands),

            // ── Shift ──
            "lsl" => self.enc_lsl(&operands),
            "asr" => self.enc_asr(&operands),

            // ── Compare ──
            "cmp" => self.enc_cmp(&operands),

            // ── Conditional ──
            "cset" => self.enc_cset(&operands),

            // ── Load/Store ──
            "ldr" => self.enc_ldr(&operands),
            "str" => self.enc_str(&operands),
            "ldrb" => self.enc_ldrb(&operands),
            "strb" => self.enc_strb(&operands),

            // ── Branch ──
            "b" => self.enc_b(&operands),
            "bl" => self.enc_bl(&operands),
            "blr" => self.enc_blr(&operands),
            "br" => self.enc_br(&operands),
            "ret" => self.enc_ret(&operands),
            "brk" => self.enc_brk(&operands),

            // ── System ──
            "svc" => self.enc_svc(&operands),

            // ── Float convert ──
            "scvtf" => self.enc_scvtf(&operands),
            "fcvtzs" => self.enc_fcvtzs(&operands),

            // ── Extend ──
            "sxtw" => self.enc_sxtw(&operands),
            "adrp" => self.enc_adrp(&operands),

            // ── Other ──
            "nop" => Ok(EncodedInst::bytes(0xD503201F)), // NOP
            _ => Err(format!("Unknown ARM64 instruction: {}", mnemonic)),
        }
    }

    // ── STP/LDP ──
    fn enc_stp(&self, ops: &[String]) -> Result<EncodedInst, String> {
        // STP Xt1, Xt2, [Xn, #imm]!  (pre-index)
        // STP Xt1, Xt2, [Xn], #imm   (post-index)
        // STP Xt1, Xt2, [Xn, #imm]   (signed offset)
        let (rt1, rt2, rn, imm, mode) = parse_stp_ldp_operands(ops)?;
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let imm7 = ((imm / 8) as u32) & 0x7F;
        let rt1 = parse_reg(&rt1)? as u32;
        let rt2 = parse_reg(&rt2)? as u32;
        let rn = parse_reg(&rn)? as u32;

        let base = if sf == 1 { 0xA900_0000u32 } else { 0x2900_0000u32 };
        let opcode = match mode {
            StpLdpMode::PreIndex => base | 0x0080_0000,
            StpLdpMode::PostIndex => base | 0x0080_0000 >> 1, // Wait, this is wrong
            StpLdpMode::SignedOffset => base,
        };
        // Actually:
        // STP pre-index:  0xA9800000 (64-bit) | 0x29800000 (32-bit)
        // STP post-index: 0xA8800000 (64-bit) | 0x28800000 (32-bit)
        // STP signed:     0xA9000000 (64-bit) | 0x29000000 (32-bit)
        let opcode = match mode {
            StpLdpMode::PreIndex => if sf == 1 { 0xA980_0000 } else { 0x2980_0000 },
            StpLdpMode::PostIndex => if sf == 1 { 0xA880_0000 } else { 0x2880_0000 },
            StpLdpMode::SignedOffset => if sf == 1 { 0xA900_0000 } else { 0x2900_0000 },
        };
        let inst = opcode | (imm7 << 15) | (rt2 << 10) | (rn << 5) | rt1;
        Ok(EncodedInst::bytes(inst))
    }

    fn enc_ldp(&self, ops: &[String]) -> Result<EncodedInst, String> {
        let (rt1, rt2, rn, imm, mode) = parse_stp_ldp_operands(ops)?;
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let imm7 = ((imm / 8) as u32) & 0x7F;
        let rt1 = parse_reg(&rt1)? as u32;
        let rt2 = parse_reg(&rt2)? as u32;
        let rn = parse_reg(&rn)? as u32;

        let opcode = match mode {
            StpLdpMode::PreIndex => if sf == 1 { 0xA9C0_0000 } else { 0x29C0_0000 },
            StpLdpMode::PostIndex => if sf == 1 { 0xA8C0_0000 } else { 0x28C0_0000 },
            StpLdpMode::SignedOffset => if sf == 1 { 0xA940_0000 } else { 0x2940_0000 },
        };
        let inst = opcode | (imm7 << 15) | (rt2 << 10) | (rn << 5) | rt1;
        Ok(EncodedInst::bytes(inst))
    }

    // ── MOV ──
    fn enc_mov(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("MOV needs 2 operands".into()); }
        let dst = &ops[0];
        let src = &ops[1];

        // MOV reg, #imm → MOVZ
        if src.starts_with('#') {
            let imm: i64 = src[1..].parse().map_err(|_| format!("Invalid immediate: {}", src))?;
            return self.emit_movz_imm(dst, imm);
        }

        // MOV reg, reg → ORR reg, XZR, reg
        let sf = if is_64bit_reg(dst) { 1u32 } else { 0u32 };
        let rd = parse_reg(dst)? as u32;
        let rm = parse_reg(src)? as u32;
        // ORR Xd, XZR, Xm: sf 01 01010 00 00 0 Rm 000000 11111 Rd
        // 64-bit: 0xAA0003E0 | (Rm << 16) | Rd
        // 32-bit: 0x2A0003E0 | (Rm << 16) | Rd
        let base = if sf == 1 { 0xAA00_03E0u32 } else { 0x2A00_03E0u32 };
        let inst = base | (rm << 16) | rd;
        Ok(EncodedInst::bytes(inst))
    }

    fn emit_movz(&self, ops: &[String]) -> Result<EncodedInst, String> {
        // MOVZ Xd, #imm16, lsl #shift
        let dst = &ops[0];
        let imm_str = &ops[1];
        let imm: u32 = imm_str.strip_prefix('#').unwrap_or(imm_str).parse()
            .map_err(|_| format!("Invalid MOVZ immediate: {}", imm_str))?;
        let shift = if ops.len() > 3 {
            ops[3].strip_prefix('#').and_then(|s| s.parse().ok()).unwrap_or(0)
        } else { 0 };
        let sf = if is_64bit_reg(dst) { 1u32 } else { 0u32 };
        let rd = parse_reg(dst)? as u32;
        let hw = (shift / 16) as u32;
        // MOVZ: sf 10 100101 00 hw imm16 Rd
        // 64-bit base: 0xD2800000, 32-bit base: 0x52800000
        let base = if sf == 1 { 0xD280_0000u32 } else { 0x5280_0000u32 };
        let inst = base | (hw << 21) | ((imm & 0xFFFF) << 5) | rd;
        Ok(EncodedInst::bytes(inst))
    }

    fn emit_movz_imm(&self, dst: &str, imm: i64) -> Result<EncodedInst, String> {
        let sf = if is_64bit_reg(dst) { 1u32 } else { 0u32 };
        let rd = parse_reg(dst)? as u32;
        let base = if sf == 1 { 0xD280_0000u32 } else { 0x5280_0000u32 };
        let movk_base = if sf == 1 { 0xF280_0000u32 } else { 0x7280_0000u32 };
        // For small immediates (0-65535), use MOVZ with shift=0
        if imm >= 0 && imm <= 0xFFFF {
            let inst = base | ((imm as u32 & 0xFFFF) << 5) | rd;
            return Ok(EncodedInst::bytes(inst));
        }
        // For larger, emit MOVZ + MOVK sequence
        let mut bytes = Vec::new();
        let v = imm as u64;
        // MOVZ Xd, #(v & 0xFFFF)
        let inst1 = base | ((v as u32 & 0xFFFF) << 5) | rd;
        bytes.extend_from_slice(&inst1.to_le_bytes());
        // MOVK Xd, #((v >> 16) & 0xFFFF), lsl #16
        if v > 0xFFFF {
            let inst2 = movk_base | (1u32 << 21) | (((v >> 16) as u32 & 0xFFFF) << 5) | rd;
            bytes.extend_from_slice(&inst2.to_le_bytes());
        }
        // MOVK Xd, #((v >> 32) & 0xFFFF), lsl #32
        if v > 0xFFFF_FFFF {
            let inst3 = movk_base | (2u32 << 21) | (((v >> 32) as u32 & 0xFFFF) << 5) | rd;
            bytes.extend_from_slice(&inst3.to_le_bytes());
        }
        // MOVK Xd, #((v >> 48) & 0xFFFF), lsl #48
        if v > 0xFFFF_FFFF_FFFF {
            let inst4 = movk_base | (3u32 << 21) | (((v >> 48) as u32 & 0xFFFF) << 5) | rd;
            bytes.extend_from_slice(&inst4.to_le_bytes());
        }
        Ok(EncodedInst { bytes, relocations: Vec::new() })
    }

    fn enc_movz(&self, ops: &[String]) -> Result<EncodedInst, String> {
        self.emit_movz(ops)
    }

    fn enc_movk(&self, ops: &[String]) -> Result<EncodedInst, String> {
        // MOVK Xd, #imm16, lsl #shift
        let dst = &ops[0];
        let imm_str = &ops[1];
        let imm: u32 = imm_str.strip_prefix('#').unwrap_or(imm_str).parse()
            .map_err(|_| format!("Invalid MOVK immediate: {}", imm_str))?;
        let shift = if ops.len() > 3 {
            ops[3].strip_prefix('#').and_then(|s| s.parse().ok()).unwrap_or(0)
        } else { 0 };
        let sf = if is_64bit_reg(dst) { 1u32 } else { 0u32 };
        let rd = parse_reg(dst)? as u32;
        let hw = (shift / 16) as u32;
        // MOVK: sf 11 100101 00 hw imm16 Rd
        // 64-bit base: 0xF2800000, 32-bit base: 0x72800000
        let base = if sf == 1 { 0xF280_0000u32 } else { 0x7280_0000u32 };
        let inst = base | (hw << 21) | ((imm & 0xFFFF) << 5) | rd;
        Ok(EncodedInst::bytes(inst))
    }

    // ── ADD ──
    fn enc_add(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 3 { return Err("ADD needs 3 operands".into()); }
        let dst = &ops[0];
        let rn_str = &ops[1];
        let rm_str = &ops[2];
        let sf = if is_64bit_reg(dst) { 1u32 } else { 0u32 };
        let rd = parse_reg(dst)? as u32;
        let rn = parse_reg(rn_str)? as u32;

        // Check for :lo12: prefix (ADD Xd, Xn, :lo12:label)
        if rm_str.starts_with(":lo12:") {
            let label = &rm_str[6..];
            // ADD Xd, Xn, #imm12 — but with label, we emit 0 and add a relocation
            // ADD (immediate): sf 0010001 0 imm12 Rn Rd
            let inst = (sf << 31) | 0x1100_0000 | (rn << 5) | rd;
            return Ok(EncodedInst::with_reloc(inst, label, 15));
        }

        if rm_str.starts_with('#') {
            // ADD Xd, Xn, #imm12
            let imm: u32 = rm_str[1..].parse().map_err(|_| format!("Invalid immediate: {}", rm_str))?;
            let inst = (sf << 31) | 0x1100_0000 | ((imm & 0xFFF) << 10) | (rn << 5) | rd;
            Ok(EncodedInst::bytes(inst))
        } else {
            // ADD Xd, Xn, Xm
            let rm = parse_reg(rm_str)? as u32;
            let inst = (sf << 31) | 0x0B00_0000 | (rm << 16) | (rn << 5) | rd;
            Ok(EncodedInst::bytes(inst))
        }
    }

    // ── SUB ──
    fn enc_sub(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 3 { return Err("SUB needs 3 operands".into()); }
        let dst = &ops[0];
        let rn_str = &ops[1];
        let rm_str = &ops[2];
        let sf = if is_64bit_reg(dst) { 1u32 } else { 0u32 };
        let rd = parse_reg(dst)? as u32;
        let rn = parse_reg(rn_str)? as u32;

        if rm_str.starts_with('#') {
            let imm: u32 = rm_str[1..].parse().map_err(|_| format!("Invalid immediate: {}", rm_str))?;
            let inst = (sf << 31) | 0x5100_0000 | ((imm & 0xFFF) << 10) | (rn << 5) | rd;
            Ok(EncodedInst::bytes(inst))
        } else {
            let rm = parse_reg(rm_str)? as u32;
            let inst = (sf << 31) | 0x4B00_0000 | (rm << 16) | (rn << 5) | rd;
            Ok(EncodedInst::bytes(inst))
        }
    }

    // ── MUL (MADD Xd, Xn, Xm, XZR) ──
    fn enc_mul(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 3 { return Err("MUL needs 3 operands".into()); }
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let rd = parse_reg(&ops[0])? as u32;
        let rn = parse_reg(&ops[1])? as u32;
        let rm = parse_reg(&ops[2])? as u32;
        // MADD Xd, Xn, Xm, XZR: sf 00 11011 000 Rm 11111 Rn Rd
        let inst = (sf << 31) | 0x1B00_7C00 | (rm << 16) | (rn << 5) | rd;
        Ok(EncodedInst::bytes(inst))
    }

    // ── SDIV ──
    fn enc_sdiv(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 3 { return Err("SDIV needs 3 operands".into()); }
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let rd = parse_reg(&ops[0])? as u32;
        let rn = parse_reg(&ops[1])? as u32;
        let rm = parse_reg(&ops[2])? as u32;
        // SDIV Xd, Xn, Xm: sf 00 11010 110 Rm 00011 Rn Rd
        let inst = (sf << 31) | 0x1AC0_0C00 | (rm << 16) | (rn << 5) | rd;
        Ok(EncodedInst::bytes(inst))
    }

    // ── UDIV ──
    fn enc_udiv(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 3 { return Err("UDIV needs 3 operands".into()); }
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let rd = parse_reg(&ops[0])? as u32;
        let rn = parse_reg(&ops[1])? as u32;
        let rm = parse_reg(&ops[2])? as u32;
        // UDIV Xd, Xn, Xm: sf 00 11010 110 Rm 00010 Rn Rd
        let inst = (sf << 31) | 0x1AC0_0800 | (rm << 16) | (rn << 5) | rd;
        Ok(EncodedInst::bytes(inst))
    }

    // ── MSUB ──
    fn enc_msub(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 4 { return Err("MSUB needs 4 operands".into()); }
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let rd = parse_reg(&ops[0])? as u32;
        let rn = parse_reg(&ops[1])? as u32;
        let rm = parse_reg(&ops[2])? as u32;
        let ra = parse_reg(&ops[3])? as u32;
        // MSUB Xd, Xn, Xm, Ra: sf 00 11011 000 Rm Ra Rn Rd
        let inst = (sf << 31) | 0x1B00_8000 | (rm << 16) | (ra << 10) | (rn << 5) | rd;
        Ok(EncodedInst::bytes(inst))
    }

    // ── NEG (SUB Xd, XZR, Xm) ──
    fn enc_neg(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("NEG needs 2 operands".into()); }
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let rd = parse_reg(&ops[0])? as u32;
        let rm = parse_reg(&ops[1])? as u32;
        let inst = (sf << 31) | 0x4B00_03E0 | (rm << 16) | rd;
        Ok(EncodedInst::bytes(inst))
    }

    // ── AND ──
    fn enc_and(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 3 { return Err("AND needs 3 operands".into()); }
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let rd = parse_reg(&ops[0])? as u32;
        let rn = parse_reg(&ops[1])? as u32;

        if ops[2].starts_with('#') {
            // AND Xd, Xn, #imm — use AND (immediate)
            // For simple masks, encode as AND immediate
            // This is complex (needs encoder for arbitrary bit patterns)
            // For the common case of 0xFFFFFFFFFFFFF000 (page mask), use a workaround:
            // We'll encode it as MOVZ + AND or just handle common cases
            // Actually, let's use a simpler approach: emit AND with the immediate
            // AND (immediate): sf 00 100100 N immr:6 imms:6 Rn:5 Rd:5
            // For 0xFFFFFFFFFFFFF000 (64-bit): N=1, immr=0, imms=51 (52 ones shifted)
            // This is a valid logical immediate: 52 ones with 12 zeros at bottom
            // Actually, let's just handle this by converting to a movk/movz pattern
            // For now, let's use a simpler approach: if the immediate is a power-of-2 mask,
            // we can compute it. But this is getting complex.
            // Simplest: for 0xFFFFFFFFFFFFF000, we can use:
            //   MOV X3, #-4096  (which is 0xFFFFFFFFFFFFF000)
            //   AND X0, X0, X3
            // But we can't emit multiple instructions from one.
            // Let's just handle the common page-mask case specially.
            let imm_str = &ops[2][1..];
            if imm_str == "0xFFFFFFFFFFFFF000" {
                // Use MOV X3, #-4096 then AND X0, X0, X3
                // But we can only emit one instruction. So let's emit the AND
                // with a register that we pre-fill.
                // Actually, the simplest approach: encode AND (immediate) properly.
                // 0xFFFFFFFFFFFFF000 = 52 ones followed by 12 zeros
                // N=1, immr=12, imms=63 → but that's not a valid encoding
                // Valid: N=1, immr=0, imms=51 (mask = 52 ones at top, 12 zeros at bottom)
                // Actually the encoding is: N:1 immr:6 imms:6
                // For 0xFFFFFFFFFFFFF000:
                //   N=1, immr=12, imms=63
                //   But imms=63 with N=1 means all ones (M=64), and immr=12 rotates
                //   The pattern is 64 ones rotated right by 12 = 12 zeros at bottom
                //   N=1, immr=12, imms=63
                let n = 1u32;
                let immr = 12u32;
                let imms = 63u32;
                let inst = (sf << 31) | (n << 22) | (immr << 16) | (imms << 10) | (rn << 5) | rd;
                return Ok(EncodedInst::bytes(inst));
            }
            // For other immediates, try to encode as AND (immediate)
            // Fall back to AND with register (emit a MOV first — but we can't)
            // For now, just error
            return Err(format!("AND with immediate {} not supported", ops[2]));
        } else {
            let rm = parse_reg(&ops[2])? as u32;
            let inst = (sf << 31) | 0x0A00_0000 | (rm << 16) | (rn << 5) | rd;
            Ok(EncodedInst::bytes(inst))
        }
    }

    // ── ORR ──
    fn enc_orr(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 3 { return Err("ORR needs 3 operands".into()); }
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let rd = parse_reg(&ops[0])? as u32;
        let rn = parse_reg(&ops[1])? as u32;
        let rm = parse_reg(&ops[2])? as u32;
        let inst = (sf << 31) | 0x2A00_0000 | (rm << 16) | (rn << 5) | rd;
        Ok(EncodedInst::bytes(inst))
    }

    // ── EOR ──
    fn enc_eor(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 3 { return Err("EOR needs 3 operands".into()); }
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let rd = parse_reg(&ops[0])? as u32;
        let rn = parse_reg(&ops[1])? as u32;
        let rm = parse_reg(&ops[2])? as u32;
        let inst = (sf << 31) | 0x4A00_0000 | (rm << 16) | (rn << 5) | rd;
        Ok(EncodedInst::bytes(inst))
    }

    // ── MVN (ORN Xd, XZR, Xm) ──
    fn enc_mvn(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("MVN needs 2 operands".into()); }
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let rd = parse_reg(&ops[0])? as u32;
        let rm = parse_reg(&ops[1])? as u32;
        let inst = (sf << 31) | 0x2A20_03E0 | (rm << 16) | rd;
        Ok(EncodedInst::bytes(inst))
    }

    // ── LSL (register: LSLV) ──
    fn enc_lsl(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 3 { return Err("LSL needs 3 operands".into()); }
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let rd = parse_reg(&ops[0])? as u32;
        let rn = parse_reg(&ops[1])? as u32;

        if ops[2].starts_with('#') {
            // LSL Xd, Xn, #shift → UBFM Xd, Xn, #(-shift mod 64), #(63-shift)
            let shift: u32 = ops[2][1..].parse().map_err(|_| "Invalid LSL shift")?;
            if sf == 1 {
                let immr = (64 - shift) % 64;
                let imms = 63 - shift;
                let inst = 0xD340_0000u32 | (imms << 16) | (immr << 6) | (rn << 5) | rd;
                Ok(EncodedInst::bytes(inst))
            } else {
                let immr = (32 - shift) % 32;
                let imms = 31 - shift;
                let inst = 0x5300_0000u32 | (imms << 16) | (immr << 6) | (rn << 5) | rd;
                Ok(EncodedInst::bytes(inst))
            }
        } else {
            // LSL Xd, Xn, Xm → LSLV
            let rm = parse_reg(&ops[2])? as u32;
            let inst = (sf << 31) | 0x1AC0_2000 | (rm << 16) | (rn << 5) | rd;
            Ok(EncodedInst::bytes(inst))
        }
    }

    // ── ASR (register: ASRV) ──
    fn enc_asr(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 3 { return Err("ASR needs 3 operands".into()); }
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let rd = parse_reg(&ops[0])? as u32;
        let rn = parse_reg(&ops[1])? as u32;

        if ops[2].starts_with('#') {
            // ASR Xd, Xn, #shift → SBFM Xd, Xn, #shift, #(63 or 31)
            let shift: u32 = ops[2][1..].parse().map_err(|_| "Invalid ASR shift")?;
            if sf == 1 {
                let inst = 0xD340_0000u32 | (63u32 << 16) | (shift << 6) | (rn << 5) | rd;
                Ok(EncodedInst::bytes(inst))
            } else {
                let inst = 0x1300_0000u32 | (31u32 << 16) | (shift << 6) | (rn << 5) | rd;
                Ok(EncodedInst::bytes(inst))
            }
        } else {
            // ASR Xd, Xn, Xm → ASRV
            let rm = parse_reg(&ops[2])? as u32;
            let inst = (sf << 31) | 0x1AC0_2800 | (rm << 16) | (rn << 5) | rd;
            Ok(EncodedInst::bytes(inst))
        }
    }

    // ── CMP (SUBS XZR, Xn, Xm/#imm) ──
    fn enc_cmp(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("CMP needs 2 operands".into()); }
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let rn = parse_reg(&ops[0])? as u32;

        if ops[1].starts_with('#') {
            let imm: u32 = ops[1][1..].parse().map_err(|_| "Invalid CMP immediate")?;
            let inst = (sf << 31) | 0x7100_003F | ((imm & 0xFFF) << 10) | (rn << 5);
            Ok(EncodedInst::bytes(inst))
        } else {
            let rm = parse_reg(&ops[1])? as u32;
            let inst = (sf << 31) | 0x6B00_001F | (rm << 16) | (rn << 5);
            Ok(EncodedInst::bytes(inst))
        }
    }

    // ── CSET (CSINC Xd, XZR, XZR, invert(cond)) ──
    fn enc_cset(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("CSET needs 2 operands".into()); }
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let rd = parse_reg(&ops[0])? as u32;
        let cond = parse_cond(&ops[1])? as u32;
        let inv_cond = cond ^ 1;
        let inst = (sf << 31) | 0x1A80_0000 | (inv_cond << 12) | (31u32 << 5) | rd;
        // Actually CSET = CSINC Xd, XZR, XZR, invert(cond)
        // CSINC: sf 1 1010100 0 Rm 0 cond 01 Rn Rd
        // = 0x1A800400 | (inv_cond << 12) | (Rm << 16) | (Rn << 5) | Rd
        // With Rm=31, Rn=31:
        let inst = (sf << 31) | 0x1A80_07E0 | (inv_cond << 12) | rd;
        // Hmm, let me recalculate:
        // CSINC Xd, XZR, XZR, cond:
        //   1 1 101010 0 0 11111 0 cond 01 11111 Rd
        // = 0x9A9F07E0 | (cond << 12) | Rd  (for 64-bit)
        // CSET inverts the condition:
        let inst = if sf == 1 {
            0x9A9F_07E0u32 | (inv_cond << 12) | rd
        } else {
            0x1A9F_07E0u32 | (inv_cond << 12) | rd
        };
        Ok(EncodedInst::bytes(inst))
    }

    // ── LDR ──
    fn enc_ldr(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("LDR needs 2 operands".into()); }
        let rt = parse_reg(&ops[0])? as u32;
        let (rn, imm) = parse_mem_operand(&ops[1])?;
        let rn = parse_reg(&rn)? as u32;
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };

        if imm == 0 && !ops[1].contains('!') {
            // LDR Xt, [Xn] → LDR Xt, [Xn, #0]
            let inst = if sf == 1 { 0xF940_0000u32 } else { 0xB940_0000u32 };
            let inst = inst | (rn << 5) | rt;
            Ok(EncodedInst::bytes(inst))
        } else if imm < 0 {
            // Negative offsets are unscaled: LDUR Xt, [Xn, #simm9]
            if !(-256..0).contains(&imm) {
                return Err(format!("LDR offset out of range: {}", imm));
            }
            let inst = if sf == 1 { 0xF840_0000u32 } else { 0xB840_0000u32 };
            let inst = inst | (((imm as u32) & 0x1FF) << 12) | (rn << 5) | rt;
            Ok(EncodedInst::bytes(inst))
        } else {
            // LDR Xt, [Xn, #imm]
            let imm12 = (imm / (if sf == 1 { 8 } else { 4 })) as u32;
            let inst = if sf == 1 { 0xF940_0000u32 } else { 0xB940_0000u32 };
            let inst = inst | (imm12 << 10) | (rn << 5) | rt;
            Ok(EncodedInst::bytes(inst))
        }
    }

    // ── STR ──
    fn enc_str(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("STR needs 2 operands".into()); }
        let rt = parse_reg(&ops[0])? as u32;
        let (rn, imm) = parse_mem_operand(&ops[1])?;
        let rn = parse_reg(&rn)? as u32;
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };

        if imm == 0 {
            let inst = if sf == 1 { 0xF900_0000u32 } else { 0xB900_0000u32 };
            let inst = inst | (rn << 5) | rt;
            Ok(EncodedInst::bytes(inst))
        } else if imm < 0 {
            // STUR Xt, [Xn, #simm9] — unscaled signed offset
            if !(-256..0).contains(&imm) {
                return Err(format!("STR offset out of range: {}", imm));
            }
            let inst = if sf == 1 { 0xF800_0000u32 } else { 0xB800_0000u32 };
            let inst = inst | (((imm as u32) & 0x1FF) << 12) | (rn << 5) | rt;
            Ok(EncodedInst::bytes(inst))
        } else {
            let imm12 = (imm / (if sf == 1 { 8 } else { 4 })) as u32;
            let inst = if sf == 1 { 0xF900_0000u32 } else { 0xB900_0000u32 };
            let inst = inst | (imm12 << 10) | (rn << 5) | rt;
            Ok(EncodedInst::bytes(inst))
        }
    }

    // ── LDRB (load byte) ──
    fn enc_ldrb(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("LDRB needs 2 operands".into()); }
        let rt = parse_reg(&ops[0])? as u32;
        let (rn, imm) = parse_mem_operand(&ops[1])?;
        let rn = parse_reg(&rn)? as u32;
        // LDRB Wt, [Xn, #imm]: 0x39400000 | (imm12 << 10) | (rn << 5) | rt
        let inst = 0x3940_0000u32 | ((imm as u32 & 0xFFF) << 10) | (rn << 5) | rt;
        Ok(EncodedInst::bytes(inst))
    }

    // ── STRB (store byte) ──
    fn enc_strb(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("STRB needs 2 operands".into()); }
        let rt = parse_reg(&ops[0])? as u32;
        let (rn, imm) = parse_mem_operand(&ops[1])?;
        let rn = parse_reg(&rn)? as u32;
        // STRB Wt, [Xn, #imm]: 0x39000000 | (imm12 << 10) | (rn << 5) | rt
        let inst = 0x3900_0000u32 | ((imm as u32 & 0xFFF) << 10) | (rn << 5) | rt;
        Ok(EncodedInst::bytes(inst))
    }

    // ── SVC (supervisor call / syscall) ──
    fn enc_svc(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.is_empty() { return Err("SVC needs 1 operand".into()); }
        let imm: u32 = ops[0].strip_prefix('#').unwrap_or(&ops[0]).parse()
            .map_err(|_| "Invalid SVC immediate")?;
        // SVC #imm16: 0xD4000001 | (imm16 << 5)
        let inst = 0xD400_0001u32 | ((imm & 0xFFFF) << 5);
        Ok(EncodedInst::bytes(inst))
    }

    // ── B.cond (conditional branch) ──
    fn enc_bcond(&self, cond: &str, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.is_empty() { return Err("B.cond needs 1 operand".into()); }
        let target = &ops[0];
        let cond_val = parse_cond(cond)? as u32;
        // B.cond label: 0x54000000 | (cond << 0)
        // We emit the base with cond and add a relocation for the label
        let inst = 0x5400_0000u32 | cond_val;
        Ok(EncodedInst::with_reloc(inst, target, 12))
    }

    // ── B (unconditional branch) ──
    fn enc_b(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.is_empty() { return Err("B needs 1 operand".into()); }
        let target = &ops[0];
        // B label: 0x14000000 | (offset/4 & 0x03FFFFFF)
        // We emit 0x14000000 and add a relocation
        Ok(EncodedInst::with_reloc(0x1400_0000, target, 11))
    }

    // ── BL (branch with link) ──
    fn enc_bl(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.is_empty() { return Err("BL needs 1 operand".into()); }
        let target = &ops[0];
        Ok(EncodedInst::with_reloc(0x9400_0000, target, 10))
    }

    // ── BLR ──
    fn enc_blr(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.is_empty() { return Err("BLR needs 1 operand".into()); }
        let rn = parse_reg(&ops[0])? as u32;
        let inst = 0xD63F_0000u32 | (rn << 5);
        Ok(EncodedInst::bytes(inst))
    }

    // ── BR ──
    fn enc_br(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.is_empty() { return Err("BR needs 1 operand".into()); }
        let rn = parse_reg(&ops[0])? as u32;
        let inst = 0xD61F_0000u32 | (rn << 5);
        Ok(EncodedInst::bytes(inst))
    }

    // ── RET ──
    fn enc_ret(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.is_empty() {
            // RET (uses X30/LR by default)
            Ok(EncodedInst::bytes(0xD65F_03C0))
        } else {
            let rn = parse_reg(&ops[0])? as u32;
            let inst = 0xD65F_0000u32 | (rn << 5);
            Ok(EncodedInst::bytes(inst))
        }
    }

    // ── BRK ──
    fn enc_brk(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.is_empty() { return Err("BRK needs 1 operand".into()); }
        let imm: u32 = ops[0].strip_prefix('#').unwrap_or(&ops[0]).parse()
            .map_err(|_| "Invalid BRK immediate")?;
        let inst = 0xD420_0000u32 | ((imm & 0xFFFF) << 5);
        Ok(EncodedInst::bytes(inst))
    }

    // ── SCVTF (int to float) ──
    fn enc_scvtf(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("SCVTF needs 2 operands".into()); }
        // SCVTF Dd, Xn: 1 00 11110 0 1 1 001 10000 Rn Rd
        // For 64-bit int to 64-bit float:
        let rd = parse_float_reg(&ops[0])? as u32;
        let rn = parse_reg(&ops[1])? as u32;
        let inst = 0x9E62_0000u32 | (rn << 5) | rd;
        Ok(EncodedInst::bytes(inst))
    }

    // ── FCVTZS (float to int) ──
    fn enc_fcvtzs(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("FCVTZS needs 2 operands".into()); }
        // FCVTZS Xd, Dn: 1 00 11110 0 1 1 001 11000 Rn Rd
        let rd = parse_reg(&ops[0])? as u32;
        let rn = parse_float_reg(&ops[1])? as u32;
        let inst = 0x9E38_0000u32 | (rn << 5) | rd;
        Ok(EncodedInst::bytes(inst))
    }

    // ── ADRP (compute PC-relative page address) ──
    // ADRP Xd, label — produces a relocation for the linker
    fn enc_adrp(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("ADRP needs 2 operands".into()); }
        let sf = if is_64bit_reg(&ops[0]) { 1u32 } else { 0u32 };
        let rd = parse_reg(&ops[0])? as u32;
        let label = &ops[1];
        // ADRP Xd, label: 1 immlo:2 10000 immhi:19 Rd:5
        // Base: 0x90000000 (64-bit)
        // We emit 0 and add a relocation of type 14 (ADRP)
        let inst = (sf << 31) | 0x9000_0000 | rd;
        Ok(EncodedInst::with_reloc(inst, label, 14))
    }

    // ── SXTW (sign extend word to doubleword) ──
    fn enc_sxtw(&self, ops: &[String]) -> Result<EncodedInst, String> {
        if ops.len() < 2 { return Err("SXTW needs 2 operands".into()); }
        // SXTW Xd, Wn → SBFM Xd, Xn, #0, #31
        let rd = parse_reg(&ops[0])? as u32;
        let rn = parse_reg(&ops[1])? as u32;
        let inst = 0xD340_7C00u32 | (rn << 5) | rd;
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

enum StpLdpMode {
    PreIndex,
    PostIndex,
    SignedOffset,
}

fn parse_stp_ldp_operands(ops: &[String]) -> Result<(String, String, String, i64, StpLdpMode), String> {
    // STP Xt1, Xt2, [Xn, #imm]!  → ops = ["Xt1", "Xt2", "[Xn, #-imm]!"]
    // STP Xt1, Xt2, [Xn], #imm   → ops = ["Xt1", "Xt2", "[Xn]", "#imm"]  (comma split by parse_operands)
    // STP Xt1, Xt2, [Xn, #imm]   → ops = ["Xt1", "Xt2", "[Xn, #imm]"]
    // STP Xt1, Xt2, [Xn]         → ops = ["Xt1", "Xt2", "[Xn]"]
    if ops.len() < 3 { return Err("STP/LDP needs at least 3 operands".into()); }
    let rt1 = ops[0].clone();
    let rt2 = ops[1].clone();
    let mem = ops[2].trim();

    if mem.ends_with('!') {
        // Pre-index: [Xn, #imm]!
        let inner = &mem[1..mem.len()-2]; // strip [ and ]!
        let parts: Vec<&str> = inner.split(',').map(|s| s.trim()).collect();
        let rn = parts[0].to_string();
        let imm = if parts.len() > 1 {
            parts[1].strip_prefix('#').unwrap_or(parts[1]).parse().unwrap_or(0)
        } else { 0 };
        Ok((rt1, rt2, rn, imm, StpLdpMode::PreIndex))
    } else if ops.len() > 3 {
        // Post-index: [Xn], #imm  (comma was split by parse_operands)
        let rn = mem.trim_start_matches('[').trim_end_matches(']').trim().to_string();
        let imm_str = ops[3].trim();
        let imm = imm_str.strip_prefix('#').unwrap_or(imm_str).parse().unwrap_or(0);
        Ok((rt1, rt2, rn, imm, StpLdpMode::PostIndex))
    } else {
        // Signed offset: [Xn, #imm] or [Xn]
        let inner = &mem[1..mem.len()-1]; // strip [ and ]
        let parts: Vec<&str> = inner.split(',').map(|s| s.trim()).collect();
        let rn = parts[0].to_string();
        let imm = if parts.len() > 1 {
            parts[1].strip_prefix('#').unwrap_or(parts[1]).parse().unwrap_or(0)
        } else { 0 };
        Ok((rt1, rt2, rn, imm, StpLdpMode::SignedOffset))
    }
}

fn parse_mem_operand(s: &str) -> Result<(String, i64), String> {
    // [Xn] or [Xn, #imm]
    let s = s.trim();
    if !s.starts_with('[') || !s.ends_with(']') {
        return Err(format!("Invalid memory operand: {}", s));
    }
    let inner = &s[1..s.len()-1];
    let parts: Vec<&str> = inner.split(',').map(|s| s.trim()).collect();
    let rn = parts[0].to_string();
    let imm = if parts.len() > 1 {
        parts[1].strip_prefix('#').unwrap_or(parts[1]).parse().unwrap_or(0)
    } else { 0 };
    Ok((rn, imm))
}

fn parse_float_reg(s: &str) -> Result<u8, String> {
    let s = s.trim();
    if s.starts_with('d') || s.starts_with('D') {
        let n: u8 = s[1..].parse().map_err(|_| format!("Invalid float register: {}", s))?;
        if n > 31 { return Err(format!("Register out of range: {}", s)); }
        Ok(n)
    } else if s.starts_with('v') || s.starts_with('V') {
        let n: u8 = s[1..].parse().map_err(|_| format!("Invalid float register: {}", s))?;
        if n > 31 { return Err(format!("Register out of range: {}", s)); }
        Ok(n)
    } else {
        Err(format!("Not a float register: {}", s))
    }
}

fn split_mnemonic(line: &str) -> (&str, &str) {
    // Find the first space
    match line.find(char::is_whitespace) {
        Some(pos) => (&line[..pos], line[pos..].trim()),
        None => (line, ""),
    }
}

fn parse_operands(s: &str) -> Vec<String> {
    if s.is_empty() { return Vec::new(); }

    // Split by comma, but respect brackets
    let mut result = Vec::new();
    let mut current = String::new();
    let mut bracket_depth = 0;

    for c in s.chars() {
        match c {
            '[' => { bracket_depth += 1; current.push(c); }
            ']' => { bracket_depth -= 1; current.push(c); }
            ',' if bracket_depth == 0 => {
                result.push(current.trim().to_string());
                current.clear();
            }
            _ => current.push(c),
        }
    }
    if !current.trim().is_empty() {
        result.push(current.trim().to_string());
    }

    // Filter out "lsl" keyword from operands (it's part of the shift syntax)
    // e.g., "movk x0, #1234, lsl #16" → ["x0", "#1234", "lsl", "#16"]
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
        assert_eq!(text, vec![0xC0, 0x03, 0x5F, 0xD6]); // RET
    }

    #[test]
    fn test_encode_nop() {
        let (text, _, _) = assemble("nop").unwrap();
        assert_eq!(text, vec![0x1F, 0x20, 0x03, 0xD5]); // NOP
    }

    #[test]
    fn test_encode_mov_reg() {
        // MOV X0, X1 → ORR X0, XZR, X1
        let (text, _, _) = assemble("mov x0, x1").unwrap();
        // ORR X0, XZR, X1: 0xAA0003E0 | (1 << 16) | 0 = 0xAA0103E0
        assert_eq!(text, vec![0xE0, 0x03, 0x01, 0xAA]);
    }

    #[test]
    fn test_encode_add_reg() {
        // ADD X0, X1, X2
        let (text, _, _) = assemble("add x0, x1, x2").unwrap();
        // 0x8B000000 | (2 << 16) | (1 << 5) | 0 = 0x8B020020
        assert_eq!(text, vec![0x20, 0x00, 0x02, 0x8B]);
    }

    #[test]
    fn test_encode_add_imm() {
        // ADD X0, X1, #16
        let (text, _, _) = assemble("add x0, x1, #16").unwrap();
        // 0x91000000 | (16 << 10) | (1 << 5) | 0 = 0x91004020
        assert_eq!(text, vec![0x20, 0x40, 0x00, 0x91]);
    }

    #[test]
    fn test_encode_stp() {
        // STP X29, X30, [SP, #-16]!
        let (text, _, _) = assemble("stp x29, x30, [sp, #-16]!").unwrap();
        // Pre-index, 64-bit: 0xA9800000 | ((-16/8 & 0x7F) << 15) | (30 << 10) | (31 << 5) | 29
        // -16/8 = -2, as u7 = 0x7E
        let inst = 0xA980_0000u32 | (0x7Eu32 << 15) | (30u32 << 10) | (31u32 << 5) | 29;
        assert_eq!(text, inst.to_le_bytes());
    }

    #[test]
    fn test_encode_ldp() {
        // LDP X29, X30, [SP], #16
        let (text, _, _) = assemble("ldp x29, x30, [sp], #16").unwrap();
        // Post-index, 64-bit: 0xA8C00000 | ((16/8 & 0x7F) << 15) | (30 << 10) | (31 << 5) | 29
        let inst = 0xA8C0_0000u32 | (2u32 << 15) | (30u32 << 10) | (31u32 << 5) | 29;
        assert_eq!(text, inst.to_le_bytes());
    }

    #[test]
    fn test_encode_bl_external() {
        // BL p_print_int — external symbol, should produce relocation
        let (_, _, relocs) = assemble("bl p_print_int").unwrap();
        assert_eq!(relocs.len(), 1);
        assert_eq!(relocs[0].1, "p_print_int");
        assert_eq!(relocs[0].2, 10); // BL reloc type
    }

    #[test]
    fn test_encode_b_label() {
        // B .Llabel — local label, should resolve
        let asm = ".Llabel:\n    b .Llabel";
        let (text, _, relocs) = assemble(asm).unwrap();
        assert!(relocs.is_empty()); // No external relocations
        // B to self: offset = 0
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        assert_eq!(inst, 0x1400_0000); // B #0
    }

    #[test]
    fn test_encode_movz() {
        // MOVZ X0, #42
        let (text, _, _) = assemble("movz x0, #42").unwrap();
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // 0xD2800000 | (42 << 5) | 0 = 0xD2800540
        assert_eq!(inst, 0xD280_0540);
    }

    #[test]
    fn test_encode_cmp_imm() {
        // CMP X0, #0
        let (text, _, _) = assemble("cmp x0, #0").unwrap();
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // SUBS XZR, X0, #0: 0xF100003F | (0 << 10) | (0 << 5) = 0xF100003F
        assert_eq!(inst, 0xF100_003F);
    }

    #[test]
    fn test_encode_brk() {
        // BRK #1
        let (text, _, _) = assemble("brk #1").unwrap();
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // 0xD4200000 | (1 << 5) = 0xD4200020
        assert_eq!(inst, 0xD420_0020);
    }

    #[test]
    fn test_encode_ldr() {
        // LDR X0, [X1]
        let (text, _, _) = assemble("ldr x0, [x1]").unwrap();
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // 0xF9400000 | (1 << 5) | 0 = 0xF9400020
        assert_eq!(inst, 0xF940_0020);
    }

    #[test]
    fn test_encode_str() {
        // STR X0, [X1]
        let (text, _, _) = assemble("str x0, [x1]").unwrap();
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // 0xF9000000 | (1 << 5) | 0 = 0xF9000020
        assert_eq!(inst, 0xF900_0020);
    }

    #[test]
    fn test_encode_mul() {
        // MUL X0, X1, X2 → MADD X0, X1, X2, XZR
        let (text, _, _) = assemble("mul x0, x1, x2").unwrap();
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // MADD X0, X1, X2, XZR: sf=1, 0x1B007C00 | (2 << 16) | (1 << 5) | 0
        // = 0x80000000 | 0x1B007C00 | 0x20000 | 0x20 = 0x9B027C20
        assert_eq!(inst, 0x9B02_7C20);
    }

    #[test]
    fn test_encode_sdiv() {
        // SDIV X0, X1, X2
        let (text, _, _) = assemble("sdiv x0, x1, x2").unwrap();
        let inst = u32::from_le_bytes([text[0], text[1], text[2], text[3]]);
        // SDIV: sf=1, 0x1AC00C00 | (2 << 16) | (1 << 5) | 0
        // = 0x80000000 | 0x1AC00C00 | 0x20000 | 0x20 = 0x9AC20C20
        assert_eq!(inst, 0x9AC2_0C20);
    }

    #[test]
    fn test_full_function() {
        let asm = r#"
    .section .text
    .globl main
main:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    mov x0, #42
    bl p_print_int
    mov x0, #0
    ldp x29, x30, [sp], #16
    ret
"#;
        let (text, syms, relocs) = assemble(asm).unwrap();
        assert!(!text.is_empty());
        assert!(syms.iter().any(|(n, _)| n == "main"));
        assert!(relocs.iter().any(|(_, n, _)| n == "p_print_int"));
    }
}
