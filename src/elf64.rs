// ELF64 Object File Emitter — Phase 4C
//
// Produces ELF64 relocatable object files (.o) for Linux x86-64. The emitter
// takes the textual assembly output from codegen_x64 and:
//   1. Encodes each instruction to raw x86-64 machine code bytes.
//   2. Collects symbols (function labels) and relocations (call targets,
//      RIP-relative data references).
//   3. Packages everything into an ELF64 relocatable object file.
//
// The resulting .o file can be linked with `ld` or `gcc` to produce an
// executable. External runtime functions (p_print_int, p_alloc, etc.) are
// left as undefined symbols that the linker resolves.
//
// ELF64 format overview:
//   - ELF header (64 bytes)
//   - Section headers (array of Elf64Shdr, 64 bytes each)
//   - Sections: .text (code), .rela.text (relocations), .symtab, .strtab,
//     .shstrtab (section name string table)
//
// This is a minimal but valid ELF64 relocatable object. It does NOT do
// full instruction encoding (that would be a full assembler). Instead, it
// encodes the common subset of x86-64 instructions that our codegen emits,
// and falls back to emitting a nop + relocation for unsupported instructions.

use crate::pir::L3Module;
use std::collections::HashMap;

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

/// Emit an ELF64 relocatable object file from the assembly text.
/// Returns the raw bytes of the .o file.
pub fn emit_object_file(_module: &L3Module, asm: &str) -> Result<Vec<u8>, String> {
    let mut emitter = ElfEmitter::new();
    emitter.assemble(asm)?;
    emitter.build_elf()
}

/// Dump symbol table from an ELF object file (for debugging).
pub fn dump_symbols(elf: &[u8]) {
    if elf.len() < 64 {
        return;
    }
    let e_shoff = u64::from_le_bytes(elf[40..48].try_into().unwrap()) as usize;
    let e_shentsize = u16::from_le_bytes(elf[58..60].try_into().unwrap()) as usize;
    let e_shnum = u16::from_le_bytes(elf[60..62].try_into().unwrap()) as usize;
    let e_shstrndx = u16::from_le_bytes(elf[62..64].try_into().unwrap()) as usize;

    if e_shoff == 0 || e_shnum == 0 {
        return;
    }

    // Get section header string table
    let shstr_off = e_shoff + e_shstrndx * e_shentsize;
    let shstr_offset = u64::from_le_bytes(elf[shstr_off + 24..shstr_off + 32].try_into().unwrap()) as usize;

    for i in 0..e_shnum {
        let sh_off = e_shoff + i * e_shentsize;
        let sh_name = u32::from_le_bytes(elf[sh_off..sh_off+4].try_into().unwrap()) as usize;
        let sh_type = u32::from_le_bytes(elf[sh_off+4..sh_off+8].try_into().unwrap());
        let sh_offset = u64::from_le_bytes(elf[sh_off+24..sh_off+32].try_into().unwrap()) as usize;
        let sh_size = u64::from_le_bytes(elf[sh_off+32..sh_off+40].try_into().unwrap()) as usize;
        let sh_link = u32::from_le_bytes(elf[sh_off+40..sh_off+44].try_into().unwrap()) as usize;

        let name_start = shstr_offset + sh_name;
        let name_end = elf[name_start..].iter().position(|&b| b == 0).unwrap_or(0);
        let name = String::from_utf8_lossy(&elf[name_start..name_start + name_end]);

        if name == ".symtab" {
            let strtab_off = e_shoff + sh_link * e_shentsize;
            let strtab_offset = u64::from_le_bytes(elf[strtab_off+24..strtab_off+32].try_into().unwrap()) as usize;

            let num_syms = sh_size / 24;
            for j in 0..num_syms {
                let sym_off = sh_offset + j * 24;
                if sym_off + 24 > elf.len() { break; }
                let st_name = u32::from_le_bytes(elf[sym_off..sym_off+4].try_into().unwrap()) as usize;
                let st_value = u64::from_le_bytes(elf[sym_off+8..sym_off+16].try_into().unwrap());
                let st_info = elf[sym_off + 4];

                let name_start = strtab_offset + st_name;
                let name_end = elf[name_start..].iter().position(|&b| b == 0).unwrap_or(0);
                let sym_name = String::from_utf8_lossy(&elf[name_start..name_start + name_end]);

                if !sym_name.is_empty() && (st_info & 0xf) == 2 {
                    let _ = (sym_name, st_value, st_info); // debugging only
                }
            }
        }
    }
}

/// Emit an ELF64 object file for a specific target architecture.
/// Supports x86-64 (full assembler), arm64 and riscv (raw binary with correct ELF header).
pub fn emit_object_file_for_target(_module: &L3Module, asm: &str, target: &str) -> Result<Vec<u8>, String> {
    match target {
        "x86-64" | "x64" => emit_object_file(_module, asm),
        "arm64" | "aarch64" => {
            // ARM64: we can't assemble the text ourselves, but we can produce
            // a valid ELF64 relocatable object with the assembly as .comment
            // and a note that it needs external assembly.
            // For now, produce a valid ELF with e_machine=EM_AARCH64.
            let mut emitter = ElfEmitter::new();
            emitter.machine = EM_AARCH64;
            // Store the assembly text as a .comment section (not executable)
            emitter.text = Vec::new(); // no machine code (would need ARM64 assembler)
            emitter.comment = asm.as_bytes().to_vec();
            emitter.build_elf()
        }
        "riscv" | "riscv64" => {
            let mut emitter = ElfEmitter::new();
            emitter.machine = EM_RISCV;
            emitter.text = Vec::new();
            emitter.comment = asm.as_bytes().to_vec();
            emitter.build_elf()
        }
        _ => Err(format!("Unknown target: {}", target)),
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// ELF64 constants
// ═══════════════════════════════════════════════════════════════════════════

const ELFMAG: [u8; 4] = [0x7f, b'E', b'L', b'F'];
const ELFCLASS64: u8 = 2;
const ELFDATA2LSB: u8 = 1; // little-endian
const EV_CURRENT: u8 = 1;
const ELFOSABI_NONE: u8 = 0;
const ET_REL: u16 = 1; // relocatable file
const EM_X86_64: u16 = 62;
const EM_AARCH64: u16 = 183;
const EM_RISCV: u16 = 243;

// Section types
const SHT_NULL: u32 = 0;
const SHT_PROGBITS: u32 = 1;
const SHT_SYMTAB: u32 = 2;
const SHT_STRTAB: u32 = 3;
const SHT_RELA: u32 = 4;

// Section flags
const SHF_ALLOC: u64 = 0x2;
const SHF_EXECINSTR: u64 = 0x4;
const SHF_INFO_LINK: u64 = 0x40;

// Symbol binding
const STB_LOCAL: u8 = 0;
const STB_GLOBAL: u8 = 1;

// Symbol types
const STT_NOTYPE: u8 = 0;
const STT_FUNC: u8 = 2;
const STT_SECTION: u8 = 3;
const STT_FILE: u8 = 4;

// Special section indices
const SHN_UNDEF: u16 = 0;
const SHN_ABS: u16 = 0xfff1;

// Relocation types (x86-64)
const R_X86_64_PC32: u32 = 2;   // PC-relative 32-bit
const R_X86_64_PLT32: u32 = 4;  // PC-relative PLT (like PC32 for static link)
const R_X86_64_64: u32 = 1;     // absolute 64-bit

// ═══════════════════════════════════════════════════════════════════════════
// ELF emitter
// ═══════════════════════════════════════════════════════════════════════════

struct ElfEmitter {
    /// Encoded .text section bytes.
    text: Vec<u8>,
    /// ELF machine type (EM_X86_64, EM_AARCH64, EM_RISCV).
    machine: u16,
    /// .comment section bytes (used for non-x86 targets to store assembly text).
    comment: Vec<u8>,
    /// Symbols: name → (offset in .text, size, binding, type, is_defined).
    symbols: Vec<Symbol>,
    /// Symbol name → index in symbols vec.
    symbol_index: HashMap<String, usize>,
    /// Relocations: (offset in .text, symbol index, type).
    relocations: Vec<Relocation>,
    /// String table for symbol names (.strtab).
    strtab: Vec<u8>,
    /// Map from symbol name to strtab offset.
    strtab_offsets: HashMap<String, u32>,
    /// Section header string table (.shstrtab).
    shstrtab: Vec<u8>,
    /// Map from section name to shstrtab offset.
    shstrtab_offsets: HashMap<String, u32>,
    /// .rodata section bytes (string constants).
    rodata: Vec<u8>,
    /// Symbols for rodata (string labels).
    rodata_symbols: Vec<(String, u32)>,
    /// Relocations for rodata (e.g., vtable function pointers).
    rodata_relocations: Vec<Relocation>,
}

#[derive(Debug, Clone)]
struct Symbol {
    name: String,
    value: u64,    // offset in section
    size: u64,
    binding: u8,
    sym_type: u8,
    section: u16,  // section index (1 = .text, 2 = .rodata)
    is_defined: bool,
}

#[derive(Debug, Clone)]
struct Relocation {
    offset: u64,   // offset in .text where the relocation applies
    symbol: usize, // index into symbols vec
    rtype: u32,
    addend: i64,
}

impl ElfEmitter {
    fn new() -> Self {
        let mut e = ElfEmitter {
            text: Vec::new(),
            machine: EM_X86_64,
            comment: Vec::new(),
            symbols: Vec::new(),
            symbol_index: HashMap::new(),
            relocations: Vec::new(),
            strtab: vec![0], // strtab starts with a null byte
            strtab_offsets: HashMap::new(),
            shstrtab: vec![0],
            shstrtab_offsets: HashMap::new(),
            rodata: Vec::new(),
            rodata_symbols: Vec::new(),
            rodata_relocations: Vec::new(),
        };
        // Pre-populate section name string table.
        e.add_shstrtab("");
        e.add_shstrtab(".text");
        e.add_shstrtab(".rodata");
        e.add_shstrtab(".symtab");
        e.add_shstrtab(".strtab");
        e.add_shstrtab(".shstrtab");
        e.add_shstrtab(".rela.text");
        e.add_shstrtab(".rela.rodata");
        e
    }

    fn add_shstrtab(&mut self, s: &str) -> u32 {
        if let Some(&off) = self.shstrtab_offsets.get(s) {
            return off;
        }
        let off = self.shstrtab.len() as u32;
        self.shstrtab.extend_from_slice(s.as_bytes());
        self.shstrtab.push(0);
        self.shstrtab_offsets.insert(s.to_string(), off);
        off
    }

    fn add_strtab(&mut self, s: &str) -> u32 {
        if let Some(&off) = self.strtab_offsets.get(s) {
            return off;
        }
        let off = self.strtab.len() as u32;
        self.strtab.extend_from_slice(s.as_bytes());
        self.strtab.push(0);
        self.strtab_offsets.insert(s.to_string(), off);
        off
    }

    fn add_symbol(&mut self, name: &str, value: u64, size: u64, binding: u8, sym_type: u8, section: u16, is_defined: bool) {
        // If this symbol already exists (e.g., added as undefined by get_or_add_local_label),
        // update it in place rather than creating a duplicate.
        if let Some(&idx) = self.symbol_index.get(name) {
            self.symbols[idx].value = value;
            self.symbols[idx].size = size;
            self.symbols[idx].binding = binding;
            self.symbols[idx].sym_type = sym_type;
            self.symbols[idx].section = section;
            self.symbols[idx].is_defined = is_defined;
            return;
        }
        let idx = self.symbols.len();
        self.symbols.push(Symbol {
            name: name.to_string(),
            value,
            size,
            binding,
            sym_type,
            section,
            is_defined,
        });
        self.symbol_index.insert(name.to_string(), idx);
    }

    fn get_or_add_undef_symbol(&mut self, name: &str) -> usize {
        if let Some(&idx) = self.symbol_index.get(name) {
            return idx;
        }
        let idx = self.symbols.len();
        self.symbols.push(Symbol {
            name: name.to_string(),
            value: 0,
            size: 0,
            binding: STB_GLOBAL,
            sym_type: STT_NOTYPE,
            section: SHN_UNDEF,
            is_defined: false,
        });
        self.symbol_index.insert(name.to_string(), idx);
        idx
    }

    // ─── Assembly parsing + encoding ───────────────────────────────────────

    /// Parse the assembly text and encode it into .text bytes + symbols + relocations.
    fn assemble(&mut self, asm: &str) -> Result<(), String> {
        let mut current_func_start: Option<u64> = None;
        let mut current_func_name: Option<String> = None;

        for line in asm.lines() {
            let trimmed = line.trim();

            // Skip empty lines and comments.
            if trimmed.is_empty() || trimmed.starts_with('#') || trimmed.starts_with("//") {
                continue;
            }

            // Handle labels first (labels can start with '.' like .Lmain_bb1:)
            if trimmed.ends_with(':') {
                let label = trimmed.trim_end_matches(':').to_string();

                // Labels in .rodata (vtables, etc.)
                if label.starts_with(".vtable_") || label.starts_with(".str_") || label.starts_with(".flt_") {
                    // Close previous function if any.
                    if let (Some(start), Some(name)) = (current_func_start, &current_func_name) {
                        let size = self.text.len() as u64 - start;
                        if let Some(&idx) = self.symbol_index.get(name) {
                            self.symbols[idx].size = size;
                        }
                    }
                    current_func_start = None;
                    current_func_name = None;
                    // Record as a rodata symbol.
                    self.rodata_symbols.push((label, self.rodata.len() as u32));
                    continue;
                }

                // Check if this is a function label (not starting with .L).
                if !label.starts_with(".L") {
                    // Close previous function if any.
                    if let (Some(start), Some(name)) = (current_func_start, &current_func_name) {
                        let size = self.text.len() as u64 - start;
                        // Update the symbol size.
                        if let Some(&idx) = self.symbol_index.get(name) {
                            self.symbols[idx].size = size;
                        }
                    }
                    // Start new function.
                    current_func_start = Some(self.text.len() as u64);
                    current_func_name = Some(label.clone());
                    self.add_symbol(&label, self.text.len() as u64, 0, STB_GLOBAL, STT_FUNC, 1, true);
                } else {
                    // Local label — record as a local symbol.
                    self.add_symbol(&label, self.text.len() as u64, 0, STB_LOCAL, STT_NOTYPE, 1, true);
                }
                continue;
            }

            // Handle directives (lines starting with '.' that are not labels).
            if trimmed.starts_with('.') {
                self.handle_directive(trimmed)?;
                continue;
            }

            // Encode the instruction.
            let text_len_before = self.text.len();
            if let Err(e) = self.encode_instruction(trimmed) {
                return Err(format!("{} — in instruction: '{}'", e, trimmed));
            }

            // If we're in a function, we could track size, but we handle that
            // at the next label or EOF.
            let _ = text_len_before;
        }

        // Close last function.
        if let (Some(start), Some(name)) = (current_func_start, &current_func_name) {
            let size = self.text.len() as u64 - start;
            if let Some(&idx) = self.symbol_index.get(name) {
                self.symbols[idx].size = size;
            }
        }

        Ok(())
    }

    fn handle_directive(&mut self, line: &str) -> Result<(), String> {
        // We mostly ignore directives — the actual code is what matters.
        // String constants (.asciz) and float constants (.quad) go into .rodata.
        // Format: "label: .asciz \"value\"" or "label: .quad bits"
        if let Some(colon_idx) = line.find(':') {
            let label = line[..colon_idx].trim().to_string();
            let rest = &line[colon_idx + 1..].trim();

            if rest.starts_with(".asciz") || rest.starts_with(".ascii") {
                // Extract the quoted string.
                let val = extract_string_literal(rest);
                let offset = self.rodata.len() as u32;
                self.rodata.extend_from_slice(val.as_bytes());
                if rest.starts_with(".asciz") {
                    self.rodata.push(0); // null terminator
                }
                self.rodata_symbols.push((label, offset));
            } else if rest.starts_with(".quad") {
                // A .quad value: either a numeric constant (float bits) or
                // a symbol reference (vtable function pointer).
                let parts: Vec<&str> = rest.split_whitespace().collect();
                if parts.len() >= 2 {
                    let offset = self.rodata.len() as u32;
                    if let Ok(bits) = parts[1].parse::<u64>() {
                        // Numeric constant (e.g., float bits)
                        self.rodata.extend_from_slice(&bits.to_le_bytes());
                        self.rodata_symbols.push((label, offset));
                    } else {
                        // Symbol reference (e.g., vtable entry)
                        // Emit 8 placeholder bytes and add a relocation.
                        let sym_idx = if parts[1].starts_with(".L") {
                            self.get_or_add_local_label(parts[1])
                        } else {
                            self.get_or_add_undef_symbol(parts[1])
                        };
                        self.rodata.extend_from_slice(&[0; 8]);
                        self.rodata_symbols.push((label, offset));
                        self.rodata_relocations.push(Relocation {
                            offset: offset as u64,
                            symbol: sym_idx,
                            rtype: R_X86_64_64,
                            addend: 0,
                        });
                    }
                }
            }
        }
        // Handle .quad without a label (e.g., vtable entries after a label line).
        if line.starts_with(".quad") {
            let parts: Vec<&str> = line.split_whitespace().collect();
            if parts.len() >= 2 {
                if let Ok(bits) = parts[1].parse::<u64>() {
                    self.rodata.extend_from_slice(&bits.to_le_bytes());
                } else {
                    // Symbol reference
                    let sym_idx = if parts[1].starts_with(".L") {
                        self.get_or_add_local_label(parts[1])
                    } else {
                        self.get_or_add_undef_symbol(parts[1])
                    };
                    let offset = self.rodata.len() as u64;
                    self.rodata.extend_from_slice(&[0; 8]);
                    self.rodata_relocations.push(Relocation {
                        offset,
                        symbol: sym_idx,
                        rtype: R_X86_64_64,
                        addend: 0,
                    });
                }
            }
        }
        // .globl, .type, .size, .text, .section, .align — all ignored for
        // encoding purposes. Symbols are tracked from labels.
        Ok(())
    }

    /// Encode a single x86-64 instruction to machine code bytes.
    fn encode_instruction(&mut self, line: &str) -> Result<(), String> {
        // Strip trailing comment.
        let line = match line.find('#') {
            Some(idx) => line[..idx].trim(),
            None => line.trim(),
        };

        // Split mnemonic and operands.
        let (mnemonic, operands_str) = if let Some(space_idx) = line.find(char::is_whitespace) {
            (line[..space_idx].to_string(), line[space_idx..].trim().to_string())
        } else {
            (line.to_string(), String::new())
        };

        // Split operands by comma, respecting parentheses (commas inside parens
        // are part of the operand, e.g. "(%rbx,%rcx,8)" is one operand).
        let operand_strings: Vec<String> = if operands_str.is_empty() {
            Vec::new()
        } else {
            let mut result = Vec::new();
            let mut current = String::new();
            let mut paren_depth = 0;
            for ch in operands_str.chars() {
                match ch {
                    '(' => { paren_depth += 1; current.push(ch); }
                    ')' => { paren_depth -= 1; current.push(ch); }
                    ',' if paren_depth == 0 => {
                        result.push(current.trim().to_string());
                        current.clear();
                    }
                    _ => current.push(ch),
                }
            }
            if !current.trim().is_empty() {
                result.push(current.trim().to_string());
            }
            result
        };
        let operands: Vec<&str> = operand_strings.iter().map(|s| s.as_str()).collect();

        match mnemonic.as_str() {
            // ─── No-operand instructions ───
            "ret" => self.text.push(0xc3),
            "leave" => { self.text.push(0xc9); }
            "nop" => self.text.push(0x90),
            "cqto" => { self.text.extend_from_slice(&[0x48, 0x99]); }
            "cdq" => { self.text.push(0x99); }
            "ud2" => { self.text.extend_from_slice(&[0x0f, 0x0b]); }
            "syscall" => { self.text.extend_from_slice(&[0x0f, 0x05]); }

            // ─── pushq / popq ───
            "pushq" => {
                if operands.len() == 1 {
                    self.encode_push(operands[0])?;
                }
            }
            "popq" => {
                if operands.len() == 1 {
                    self.encode_pop(operands[0])?;
                }
            }

            // ─── movq ───
            "movq" => {
                if operands.len() == 2 {
                    self.encode_mov(operands[0], operands[1], 8)?;
                }
            }
            "movl" => {
                if operands.len() == 2 {
                    self.encode_mov(operands[0], operands[1], 4)?;
                }
            }
            "movb" => {
                if operands.len() == 2 {
                    self.encode_mov(operands[0], operands[1], 1)?;
                }
            }

            // ─── Arithmetic ───
            "addq" => self.encode_alu(0x00, operands, 8)?,
            "subq" => self.encode_alu(0x28, operands, 8)?,
            "andq" => self.encode_alu(0x20, operands, 8)?,
            "orq"  => self.encode_alu(0x08, operands, 8)?,
            "xorq" => self.encode_alu(0x30, operands, 8)?,
            "addl" => self.encode_alu(0x00, operands, 4)?,
            "subl" => self.encode_alu(0x28, operands, 4)?,
            "andl" => self.encode_alu(0x20, operands, 4)?,
            "orl"  => self.encode_alu(0x08, operands, 4)?,
            "xorl" => self.encode_alu(0x30, operands, 4)?,
            "andb" => self.encode_alu_8bit(0x20, operands)?,
            "orb"  => self.encode_alu_8bit(0x08, operands)?,
            "xorb" => self.encode_alu_8bit(0x30, operands)?,
            "addb" => self.encode_alu_8bit(0x00, operands)?,
            "subb" => self.encode_alu_8bit(0x28, operands)?,

            "imulq" => {
                // imulq %src, %dst → REX.W 0F AF /r
                // AT&T syntax: dst = dst * src
                // IMUL encoding: reg field = dst, rm field = src
                if operands.len() == 2 {
                    let src = parse_reg(operands[0])?;
                    let dst = parse_reg(operands[1])?;
                    let rex = 0x48 | ((dst >> 3) << 2) | ((src >> 3) << 0);
                    let modrm = 0xc0 | ((dst & 7) << 3) | (src & 7);
                    self.text.extend_from_slice(&[rex, 0x0f, 0xaf, modrm]);
                }
            }

            "negq" => {
                if operands.len() == 1 {
                    let r = parse_reg(operands[0])?;
                    let rex = 0x48 | ((r >> 3) << 0);
                    let modrm = 0xd8 | (r & 7);
                    self.text.extend_from_slice(&[rex, 0xf7, modrm]);
                }
            }
            "incq" => {
                if operands.len() == 1 {
                    if let Some(r) = try_parse_reg(operands[0]) {
                        let rex = 0x48 | ((r >> 3) << 0);
                        let modrm = 0xc0 | (r & 7);
                        self.text.extend_from_slice(&[rex, 0xff, modrm]);
                    } else if let Some((off, base)) = try_parse_mem(operands[0]) {
                        let rex = 0x48 | ((base >> 3) << 0);
                        self.text.push(rex);
                        self.text.push(0xff);
                        self.encode_modrm_mem(0, off, base);
                    }
                }
            }
            "decq" => {
                if operands.len() == 1 {
                    if let Some(r) = try_parse_reg(operands[0]) {
                        let rex = 0x48 | ((r >> 3) << 0);
                        let modrm = 0xc8 | (r & 7);
                        self.text.extend_from_slice(&[rex, 0xff, modrm]);
                    } else if let Some((off, base)) = try_parse_mem(operands[0]) {
                        let rex = 0x48 | ((base >> 3) << 0);
                        self.text.push(rex);
                        self.text.push(0xff);
                        self.encode_modrm_mem(1, off, base);
                    }
                }
            }
            "notq" => {
                if operands.len() == 1 {
                    let r = parse_reg(operands[0])?;
                    let rex = 0x48 | ((r >> 3) << 0);
                    let modrm = 0xd0 | (r & 7);
                    self.text.extend_from_slice(&[rex, 0xf7, modrm]);
                }
            }

            "idivq" => {
                if operands.len() == 1 {
                    if let Some(r) = try_parse_reg(operands[0]) {
                        let rex = 0x48 | ((r >> 3) << 0);
                        let modrm = 0xf8 | (r & 7);
                        self.text.extend_from_slice(&[rex, 0xf7, modrm]);
                    } else {
                        // Memory operand — emit a placeholder.
                        self.text.extend_from_slice(&[0x48, 0xf7, 0x38]);
                    }
                }
            }

            // ─── Shifts ───
            "shlq" => self.encode_shift(0xe0, operands, 8)?,
            "shrq" => self.encode_shift(0xe8, operands, 8)?,
            "sarq" => self.encode_shift(0xf8, operands, 8)?,

            // ─── testq ───
            "testq" => {
                if operands.len() == 2 {
                    let a = parse_reg(operands[0])?;
                    let b = parse_reg(operands[1])?;
                    let rex = 0x48 | ((a >> 3) << 2) | ((b >> 3) << 0);
                    let modrm = 0xc0 | ((a & 7) << 3) | (b & 7);
                    self.text.extend_from_slice(&[rex, 0x85, modrm]);
                }
            }

            // ─── testl ───
            "testl" => {
                if operands.len() == 2 {
                    let a = parse_reg(operands[0])?;
                    let b = parse_reg(operands[1])?;
                    // No REX.W for 32-bit. REX only if extended registers.
                    if a >= 8 || b >= 8 {
                        let rex = ((a >> 3) << 2) | ((b >> 3) << 0);
                        self.text.push(rex);
                    }
                    let modrm = 0xc0 | ((a & 7) << 3) | (b & 7);
                    self.text.extend_from_slice(&[0x85, modrm]);
                }
            }

            // ─── cmpq ───
            "cmpq" => {
                if operands.len() == 2 {
                    self.encode_cmp(operands[0], operands[1], 8)?;
                }
            }

            // ─── setcc ───
            "sete"  => self.encode_setcc(0x94, operands)?,
            "setne" => self.encode_setcc(0x95, operands)?,
            "setl"  => self.encode_setcc(0x9c, operands)?,
            "setg"  => self.encode_setcc(0x9f, operands)?,
            "setle" => self.encode_setcc(0x9e, operands)?,
            "setge" => self.encode_setcc(0x9d, operands)?,
            "setb"  => self.encode_setcc(0x92, operands)?,
            "seta"  => self.encode_setcc(0x97, operands)?,
            "setbe" => self.encode_setcc(0x96, operands)?,
            "setae" => self.encode_setcc(0x93, operands)?,

            // ─── movzbq / movzwq / movslq ───
            "movzbq" => self.encode_movzx(0xb6, operands, 8)?,
            "movzwq" => self.encode_movzx(0xb7, operands, 8)?,
            "movsbq" => self.encode_movsx(0xbe, operands)?,
            "movswq" => self.encode_movsx(0xbf, operands)?,
            "movslq" => {
                if operands.len() == 2 {
                    // movslq %reg, %reg → REX.W 0x63 /r
                    if let (Some(s), Some(d)) = (try_parse_reg(operands[0]), try_parse_reg(operands[1])) {
                        let rex = 0x48 | ((s >> 3) << 2) | ((d >> 3) << 0);
                        let modrm = 0xc0 | ((s & 7) << 3) | (d & 7);
                        self.text.extend_from_slice(&[rex, 0x63, modrm]);
                    } else if let (Some((off, base)), Some(d)) = (try_parse_mem(operands[0]), try_parse_reg(operands[1])) {
                        // movslq off(%base), %reg → REX.W 0x63 /r with memory operand
                        let rex = 0x48 | ((d >> 3) << 2) | ((base >> 3) << 0);
                        self.text.push(rex);
                        self.text.push(0x63);
                        self.encode_modrm_mem(d, off, base);
                    } else if let (Some(s), Some(d)) = (try_parse_reg(operands[0]), try_parse_reg(operands[1])) {
                        let rex = 0x48 | ((s >> 3) << 2) | ((d >> 3) << 0);
                        let modrm = 0xc0 | ((s & 7) << 3) | (d & 7);
                        self.text.extend_from_slice(&[rex, 0x63, modrm]);
                    }
                }
            }

            // ─── Jumps ───
            "jmp" => {
                if operands.len() == 1 {
                    self.encode_jmp(operands[0])?;
                }
            }
            "jne" => self.encode_jcc(0x85, operands)?,
            "je"  => self.encode_jcc(0x84, operands)?,
            "jz"  => self.encode_jcc(0x84, operands)?,
            "jnz" => self.encode_jcc(0x85, operands)?,
            "jl"  => self.encode_jcc(0x8c, operands)?,
            "jg"  => self.encode_jcc(0x8f, operands)?,
            "jle" => self.encode_jcc(0x8e, operands)?,
            "jge" => self.encode_jcc(0x8d, operands)?,
            "jb"  => self.encode_jcc(0x82, operands)?,
            "ja"  => self.encode_jcc(0x87, operands)?,
            "jbe" => self.encode_jcc(0x86, operands)?,
            "jae" => self.encode_jcc(0x83, operands)?,

            // ─── call ───
            "call" => {
                if operands.len() == 1 {
                    self.encode_call(operands[0])?;
                }
            }

            // ─── leaq ───
            "leaq" => {
                if operands.len() == 2 {
                    self.encode_lea(operands[0], operands[1])?;
                }
            }

            // ─── CVT (float conversions) ───
            "cvtsi2sdq" => {
                // REX.W 0xF2 0F 2A /r
                if operands.len() == 2 {
                    let src = parse_reg(operands[0])?;
                    let dst_xmm = parse_xmm_reg(operands[1])?;
                    let rex = 0x48 | ((src >> 3) << 0);
                    let modrm = 0xc0 | ((dst_xmm & 7) << 3) | (src & 7);
                    self.text.extend_from_slice(&[0xf2, rex, 0x0f, 0x2a, modrm]);
                }
            }
            "cvttsd2siq" => {
                // F2 REX.W 0F 2C /r
                if operands.len() == 2 {
                    let src_xmm = parse_xmm_reg(operands[0])?;
                    let dst = parse_reg(operands[1])?;
                    let rex = 0x48 | ((src_xmm >> 3) << 2) | ((dst >> 3) << 0);
                    let modrm = 0xc0 | ((src_xmm & 7) << 3) | (dst & 7);
                    self.text.extend_from_slice(&[0xf2, rex, 0x0f, 0x2c, modrm]);
                }
            }
            "movsd" => {
                if operands.len() == 2 {
                    self.encode_movsd(operands[0], operands[1])?;
                }
            }
            "addsd" => {
                if operands.len() == 2 {
                    self.encode_sse2_arith(0x58, operands[0], operands[1])?;
                }
            }
            "subsd" => {
                if operands.len() == 2 {
                    self.encode_sse2_arith(0x5c, operands[0], operands[1])?;
                }
            }
            "mulsd" => {
                if operands.len() == 2 {
                    self.encode_sse2_arith(0x59, operands[0], operands[1])?;
                }
            }
            "divsd" => {
                if operands.len() == 2 {
                    self.encode_sse2_arith(0x5e, operands[0], operands[1])?;
                }
            }
            "ucomisd" => {
                if operands.len() == 2 {
                    self.encode_sse2_arith_66(0x2e, operands[0], operands[1])?;
                }
            }

            // ─── Single-precision float (SSE) ───
            "movss" => {
                if operands.len() == 2 {
                    self.encode_movss(operands[0], operands[1])?;
                }
            }
            "addss" => {
                if operands.len() == 2 {
                    self.encode_sse_arith(0x58, operands[0], operands[1])?;
                }
            }
            "subss" => {
                if operands.len() == 2 {
                    self.encode_sse_arith(0x5c, operands[0], operands[1])?;
                }
            }
            "mulss" => {
                if operands.len() == 2 {
                    self.encode_sse_arith(0x59, operands[0], operands[1])?;
                }
            }
            "divss" => {
                if operands.len() == 2 {
                    self.encode_sse_arith(0x5e, operands[0], operands[1])?;
                }
            }
            "ucomiss" => {
                if operands.len() == 2 {
                    self.encode_sse_arith_66(0x2e, operands[0], operands[1])?;
                }
            }

            // ─── Unknown instruction — emit a nop placeholder ───
            _ => {
                // For any instruction we don't recognize, emit a 1-byte nop.
                // This is a fallback; the resulting object file may not be
                // fully functional, but it will be structurally valid.
                self.text.push(0x90);
            }
        }

        Ok(())
    }

    // ─── Instruction encoders ──────────────────────────────────────────────

    fn encode_push(&mut self, op: &str) -> Result<(), String> {
        if let Some(r) = try_parse_reg(op) {
            // pushq %reg → 0x50 + r (with REX.B for r8-r15)
            if r >= 8 {
                self.text.push(0x41); // REX.B
                self.text.push(0x50 + (r - 8));
            } else {
                self.text.push(0x50 + r);
            }
        } else if op.starts_with('$') {
            // pushq $imm8 or $imm32
            if let Ok(v) = op[1..].parse::<i32>() {
                if v >= -128 && v <= 127 {
                    self.text.push(0x6a);
                    self.text.push(v as u8);
                } else {
                    self.text.push(0x68);
                    self.text.extend_from_slice(&v.to_le_bytes());
                }
            }
        }
        Ok(())
    }

    fn encode_pop(&mut self, op: &str) -> Result<(), String> {
        if let Some(r) = try_parse_reg(op) {
            if r >= 8 {
                self.text.push(0x41); // REX.B
                self.text.push(0x58 + (r - 8));
            } else {
                self.text.push(0x58 + r);
            }
        }
        Ok(())
    }

    fn encode_mov(&mut self, src: &str, dst: &str, size: usize) -> Result<(), String> {
        // movq $imm, %reg
        if src.starts_with('$') {
            if let Some(r) = try_parse_reg(dst) {
                let imm_str = &src[1..];
                // Try parsing as i64 first, then u64 (for values like 0x8000000000000000)
                let imm: i64 = if let Ok(v) = imm_str.parse::<i64>() {
                    v
                } else if let Ok(v) = imm_str.parse::<u64>() {
                    v as i64
                } else if imm_str.starts_with("0x") || imm_str.starts_with("0X") {
                    if let Ok(v) = i64::from_str_radix(&imm_str[2..], 16) {
                        v
                    } else if let Ok(v) = u64::from_str_radix(&imm_str[2..], 16) {
                        v as i64
                    } else {
                        0
                    }
                } else {
                    0
                };
                if size == 8 {
                    // REX.W 0xB8+r imm64
                    let rex = 0x48 | ((r >> 3) << 0);
                    self.text.push(rex);
                    self.text.push(0xb8 + (r & 7));
                    self.text.extend_from_slice(&imm.to_le_bytes());
                } else {
                    // 0xB8+r imm32
                    if r >= 8 { self.text.push(0x41); }
                    self.text.push(0xb8 + (r & 7));
                    self.text.extend_from_slice(&(imm as i32).to_le_bytes());
                }
                return Ok(());
            }
        }

        // movq %xmmN, %gpr  →  66 [REX.W] 0F 7E /r  (MOVQ r64, xmm)
        // Note: mandatory prefix (66) comes BEFORE REX prefix.
        // 0F 7E with 66 prefix moves from XMM to r/m64.
        if let (Ok(s_xmm), Some(d)) = (parse_xmm_reg(src), try_parse_reg(dst)) {
            let rex = 0x48 | ((s_xmm >> 3) << 2) | ((d >> 3) << 0);
            self.text.push(0x66);
            self.text.push(rex);
            self.text.push(0x0f);
            self.text.push(0x7e);
            let modrm = 0xc0 | ((s_xmm & 7) << 3) | (d & 7);
            self.text.push(modrm);
            return Ok(());
        }

        // movq %gpr, %xmmN  →  66 [REX.W] 0F 6E /r  (MOVQ xmm, r64)
        // Note: mandatory prefix (66) comes BEFORE REX prefix.
        // 0F 6E with 66 prefix moves from r/m64 to XMM.
        if let (Some(s), Ok(d_xmm)) = (try_parse_reg(src), parse_xmm_reg(dst)) {
            let rex = 0x48 | ((d_xmm >> 3) << 2) | ((s >> 3) << 0);
            self.text.push(0x66);
            self.text.push(rex);
            self.text.push(0x0f);
            self.text.push(0x6e);
            let modrm = 0xc0 | ((d_xmm & 7) << 3) | (s & 7);
            self.text.push(modrm);
            return Ok(());
        }

        // movq %reg, %reg
        if let (Some(s), Some(d)) = (try_parse_reg(src), try_parse_reg(dst)) {
            let rex = rex_w(size) | ((s >> 3) << 2) | ((d >> 3) << 0);
            self.text.push(rex);
            self.text.push(0x89);
            let modrm = 0xc0 | ((s & 7) << 3) | (d & 7);
            self.text.push(modrm);
            return Ok(());
        }

        // movq %reg, mem  (store to memory)
        // Handles 8-bit register names (al, cl, etc.) for movb
        // Handles SIB addressing: movq %reg, (%base,%index,scale)
        let src_reg = try_parse_reg(src).or_else(|| try_parse_reg8(src));
        // Try SIB first: movq %reg, (%base,%index,scale)
        if let (Some(s), Some((off, base, index, sc))) = (src_reg, try_parse_mem_indexed(dst)) {
            if size == 1 {
                let rex = 0x40 | ((s >> 3) << 2) | ((index >> 3) << 1) | ((base >> 3) << 0);
                self.text.push(rex);
                self.text.push(0x88);
                self.encode_modrm_sib(s, off, base, index, sc);
            } else {
                let rex = rex_w(size) | ((s >> 3) << 2) | ((index >> 3) << 1) | ((base >> 3) << 0);
                self.text.push(rex);
                self.text.push(0x89);
                self.encode_modrm_sib(s, off, base, index, sc);
            }
            return Ok(());
        }
        if let (Some(s), Some((off, base))) = (src_reg, try_parse_mem(dst)) {
            if size == 1 {
                let rex = 0x40 | ((s >> 3) << 2) | ((base >> 3) << 0);
                self.text.push(rex);
                self.text.push(0x88);
                self.encode_modrm_mem(s, off, base);
            } else {
                let rex = rex_w(size) | ((s >> 3) << 2) | ((base >> 3) << 0);
                self.text.push(rex);
                self.text.push(0x89);
                self.encode_modrm_mem(s, off, base);
            }
            return Ok(());
        }

        // movq mem, %reg  (load from memory)
        // Handles 8-bit register names for movb
        // Handles SIB addressing: movq (%base,%index,scale), %reg
        let dst_reg = try_parse_reg(dst).or_else(|| try_parse_reg8(dst));
        if let (Some((off, base, index, sc)), Some(d)) = (try_parse_mem_indexed(src), dst_reg) {
            if size == 1 {
                let rex = 0x40 | ((d >> 3) << 2) | ((index >> 3) << 1) | ((base >> 3) << 0);
                self.text.push(rex);
                self.text.push(0x8a);
                self.encode_modrm_sib(d, off, base, index, sc);
            } else {
                let rex = rex_w(size) | ((d >> 3) << 2) | ((index >> 3) << 1) | ((base >> 3) << 0);
                self.text.push(rex);
                self.text.push(0x8b);
                self.encode_modrm_sib(d, off, base, index, sc);
            }
            return Ok(());
        }
        if let (Some((off, base)), Some(d)) = (try_parse_mem(src), dst_reg) {
            if size == 1 {
                let rex = 0x40 | ((d >> 3) << 2) | ((base >> 3) << 0);
                self.text.push(rex);
                self.text.push(0x8a);
                self.encode_modrm_mem(d, off, base);
            } else {
                let rex = rex_w(size) | ((d >> 3) << 2) | ((base >> 3) << 0);
                self.text.push(rex);
                self.text.push(0x8b);
                self.encode_modrm_mem(d, off, base);
            }
            return Ok(());
        }

        // movq $imm, mem (immediate to memory)
        if src.starts_with('$') {
            if let Some((off, base, index, sc)) = try_parse_mem_indexed(dst) {
                let imm: i64 = src[1..].parse().unwrap_or(0);
                if size == 1 {
                    let rex = 0x40 | ((index >> 3) << 1) | ((base >> 3) << 0);
                    self.text.push(rex);
                    self.text.push(0xc6);
                    self.encode_modrm_sib(0, off, base, index, sc);
                    self.text.push(imm as u8);
                } else {
                    let rex = rex_w(size) | ((index >> 3) << 1) | ((base >> 3) << 0);
                    self.text.push(rex);
                    self.text.push(0xc7);
                    self.encode_modrm_sib(0, off, base, index, sc);
                    self.text.extend_from_slice(&(imm as i32).to_le_bytes());
                }
                return Ok(());
            }
            if let Some((off, base)) = try_parse_mem(dst) {
                let imm: i64 = src[1..].parse().unwrap_or(0);
                if size == 1 {
                    let rex = 0x40 | ((base >> 3) << 0);
                    self.text.push(rex);
                    self.text.push(0xc6);
                    self.encode_modrm_mem(0, off, base);
                    self.text.push(imm as u8);
                } else {
                    let rex = rex_w(size) | ((base >> 3) << 0);
                    self.text.push(rex);
                    self.text.push(0xc7);
                    self.encode_modrm_mem(0, off, base);
                    self.text.extend_from_slice(&(imm as i32).to_le_bytes());
                }
                return Ok(());
            }
        }

        // Fallback: nop
        self.text.push(0x90);
        Ok(())
    }

    /// Encode 8-bit ALU ops (andb, orb, xorb) using byte registers (al, cl, etc.)
    /// Encoding: [REX] opcode_base+1 ModRM  (reg, r/m)
    fn encode_alu_8bit(&mut self, opcode_base: u8, operands: Vec<&str>) -> Result<(), String> {
        if operands.len() != 2 {
            return Ok(());
        }
        let src = operands[0];
        let dst = operands[1];

        // Parse 8-bit register names to register indices
        let byte_regs = ["al", "cl", "dl", "bl", "spl", "bpl", "sil", "dil",
                         "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"];
        let parse_byte_reg = |s: &str| -> Option<usize> {
            byte_regs.iter().position(|r| *r == s.trim_start_matches('%'))
        };

        // reg, reg
        if let (Some(s), Some(d)) = (parse_byte_reg(src), parse_byte_reg(dst)) {
            // REX needed if either reg is >= 4 (spl/bpl/sil/dil) or >= 8
            let need_rex = s >= 4 || d >= 4;
            if need_rex {
                let rex = (((s >> 3) << 2) | ((d >> 3) << 0)) as u8;
                // For spl/bpl/sil/dil (indices 4-7), REX is required even without R/R extensions
                // A plain REX (0x40) is sufficient
                self.text.push(rex);
            }
            self.text.push(opcode_base | 0x01);
            let modrm = (0xc0 | ((s & 7) << 3) | (d & 7)) as u8;
            self.text.push(modrm);
        }
        Ok(())
    }

    fn encode_alu(&mut self, opcode_base: u8, operands: Vec<&str>, size: usize) -> Result<(), String> {
        if operands.len() != 2 {
            return Ok(());
        }
        let src = operands[0];
        let dst = operands[1];

        // Determine if operands are 32-bit (e.g., %eax instead of %rax).
        // For 32-bit ops, we don't emit REX.W.
        let op_is_32bit = is_reg32(src) || is_reg32(dst);
        let effective_size = if op_is_32bit { 4 } else { size };

        // ALU %reg, %reg
        if let (Some(s), Some(d)) = (try_parse_reg(src), try_parse_reg(dst)) {
            // Only emit REX prefix if needed (W bit for 64-bit, or B/R for extended regs).
            let need_rex = effective_size == 8 || s >= 8 || d >= 8;
            let text_len_before = self.text.len();
            if need_rex {
                let rex = rex_w(effective_size) | ((s >> 3) << 2) | ((d >> 3) << 0);
                self.text.push(rex);
            }
            self.text.push(opcode_base | 0x01);
            let modrm = 0xc0 | ((s & 7) << 3) | (d & 7);
            self.text.push(modrm);
            return Ok(());
        }

        // ALU $imm, %reg
        if src.starts_with('$') {
            let imm: i32 = src[1..].parse().unwrap_or(0);
            if let Some(d) = try_parse_reg(dst) {
                let dst_is_32 = is_reg32(dst);
                let esize = if dst_is_32 { 4 } else { effective_size };
                let need_rex = esize == 8 || d >= 8;
                if imm >= -128 && imm <= 127 {
                    if need_rex {
                        let rex = rex_w(esize) | ((d >> 3) << 0);
                        self.text.push(rex);
                    }
                    self.text.push(0x83);
                    let reg_field = (opcode_base >> 3) & 7;
                    let modrm = 0xc0 | (reg_field << 3) | (d & 7);
                    self.text.push(modrm);
                    self.text.push(imm as u8);
                } else {
                    if need_rex {
                        let rex = rex_w(esize) | ((d >> 3) << 0);
                        self.text.push(rex);
                    }
                    self.text.push(0x81);
                    let reg_field = (opcode_base >> 3) & 7;
                    let modrm = 0xc0 | (reg_field << 3) | (d & 7);
                    self.text.push(modrm);
                    self.text.extend_from_slice(&imm.to_le_bytes());
                }
                return Ok(());
            }
        }

        // ALU %reg, mem / ALU mem, %reg — simplified
        if let (Some(s), Some((off, base))) = (try_parse_reg(src), try_parse_mem(dst)) {
            let rex = rex_w(size) | ((s >> 3) << 2) | ((base >> 3) << 0);
            self.text.push(rex);
            self.text.push(opcode_base | 0x01);
            self.encode_modrm_mem(s, off, base);
            return Ok(());
        }
        if let (Some((off, base)), Some(d)) = (try_parse_mem(src), try_parse_reg(dst)) {
            let rex = rex_w(size) | ((d >> 3) << 2) | ((base >> 3) << 0);
            self.text.push(rex);
            self.text.push(opcode_base | 0x03);
            self.encode_modrm_mem(d, off, base);
            return Ok(());
        }

        self.text.push(0x90);
        Ok(())
    }

    fn encode_shift(&mut self, modrm_base: u8, operands: Vec<&str>, size: usize) -> Result<(), String> {
        if operands.len() != 2 {
            return Ok(());
        }
        let count = operands[0];
        let dst = operands[1];

        if let Some(d) = try_parse_reg(dst) {
            let rex = rex_w(size) | ((d >> 3) << 0);
            self.text.push(rex);
            if count.starts_with('$') {
                // shlq $imm, %reg → 0xC1 /r ib
                self.text.push(0xc1);
                let modrm = 0xc0 | modrm_base | (d & 7);
                self.text.push(modrm);
                let imm: u8 = count[1..].parse().unwrap_or(0);
                self.text.push(imm);
            } else if count == "%cl" {
                // shlq %cl, %reg → 0xD3 /r
                self.text.push(0xd3);
                let modrm = 0xc0 | modrm_base | (d & 7);
                self.text.push(modrm);
            }
        }
        Ok(())
    }

    fn encode_cmp(&mut self, src: &str, dst: &str, size: usize) -> Result<(), String> {
        // cmpq $imm, %reg
        if src.starts_with('$') {
            if let Some(d) = try_parse_reg(dst) {
                let imm: i32 = src[1..].parse().unwrap_or(0);
                if imm >= -128 && imm <= 127 {
                    let rex = rex_w(size) | ((d >> 3) << 0);
                    self.text.push(rex);
                    self.text.push(0x83);
                    let modrm = 0xc0 | (7 << 3) | (d & 7);
                    self.text.push(modrm);
                    self.text.push(imm as u8);
                } else {
                    let rex = rex_w(size) | ((d >> 3) << 0);
                    self.text.push(rex);
                    self.text.push(0x81);
                    let modrm = 0xc0 | (7 << 3) | (d & 7);
                    self.text.push(modrm);
                    self.text.extend_from_slice(&imm.to_le_bytes());
                }
                return Ok(());
            }
        }

        // cmpq %reg, %reg
        if let (Some(s), Some(d)) = (try_parse_reg(src), try_parse_reg(dst)) {
            let rex = rex_w(size) | ((s >> 3) << 2) | ((d >> 3) << 0);
            self.text.push(rex);
            self.text.push(0x39);
            let modrm = 0xc0 | ((s & 7) << 3) | (d & 7);
            self.text.push(modrm);
            return Ok(());
        }

        // cmpq mem, %reg
        // Try SIB first: cmpq (%base,%index,scale), %reg
        if let (Some((off, base, index, sc)), Some(d)) = (try_parse_mem_indexed(src), try_parse_reg(dst)) {
            let rex = rex_w(size) | ((d >> 3) << 2) | ((index >> 3) << 1) | ((base >> 3) << 0);
            self.text.push(rex);
            self.text.push(0x3b);
            self.encode_modrm_sib(d, off, base, index, sc);
            return Ok(());
        }
        if let (Some((off, base)), Some(d)) = (try_parse_mem(src), try_parse_reg(dst)) {
            let rex = rex_w(size) | ((d >> 3) << 2) | ((base >> 3) << 0);
            self.text.push(rex);
            self.text.push(0x3b);
            self.encode_modrm_mem(d, off, base);
            return Ok(());
        }

        // cmpq %reg, mem  (register source, memory destination)
        // Uses opcode 0x39 (CMP r/m, r): computes mem - reg
        // Try SIB first: cmpq %reg, (%base,%index,scale)
        if let (Some(s), Some((off, base, index, sc))) = (try_parse_reg(src), try_parse_mem_indexed(dst)) {
            let rex = rex_w(size) | ((s >> 3) << 2) | ((index >> 3) << 1) | ((base >> 3) << 0);
            self.text.push(rex);
            self.text.push(0x39);
            self.encode_modrm_sib(s, off, base, index, sc);
            return Ok(());
        }
        if let (Some(s), Some((off, base))) = (try_parse_reg(src), try_parse_mem(dst)) {
            let rex = rex_w(size) | ((s >> 3) << 2) | ((base >> 3) << 0);
            self.text.push(rex);
            self.text.push(0x39);
            self.encode_modrm_mem(s, off, base);
            return Ok(());
        }

        self.text.push(0x90);
        Ok(())
    }

    fn encode_setcc(&mut self, opcode: u8, operands: Vec<&str>) -> Result<(), String> {
        if operands.len() != 1 {
            return Ok(());
        }
        // setcc %al → 0x0F opcode 0xC0
        // setcc %r8b → 0x41 0x0F opcode 0xC0
        if let Some(r) = try_parse_reg8(operands[0]) {
            if r >= 4 && r < 8 {
                // sil, dil, bpl, spl need REX prefix
                self.text.push(0x40);
            }
            if r >= 8 {
                self.text.push(0x41);
            }
            self.text.push(0x0f);
            self.text.push(opcode);
            self.text.push(0xc0 | (r & 7));
        }
        Ok(())
    }

    /// Encode movsx (sign-extend): movsbq/movswq.
    /// Encoding: REX.W 0F opcode /r  (same as movzx but with 0x63-style sign extension)
    fn encode_movsx(&mut self, opcode: u8, operands: Vec<&str>) -> Result<(), String> {
        if operands.len() != 2 {
            return Ok(());
        }
        let d_opt = try_parse_reg(operands[1]);

        // Memory source: movsbq offset(%base), %dst
        if let (Some((off, base)), Some(d)) = (try_parse_mem(operands[0]), d_opt) {
            let rex = 0x48 | (((d >> 3) & 1) << 2) | (((base >> 3) & 1) << 0);
            self.text.push(rex);
            self.text.push(0x0f);
            self.text.push(opcode);
            self.encode_modrm_mem(d, off, base);
            return Ok(());
        }

        // Register source: movsbq %al, %rdx → REX.W 0F BE /r
        let s_opt = try_parse_reg8(operands[0]).or_else(|| try_parse_reg(operands[0]));
        if let (Some(s), Some(d)) = (s_opt, d_opt) {
            let rex = 0x48 | (((d >> 3) & 1) << 2) | (((s >> 3) & 1) << 0);
            self.text.push(rex);
            self.text.push(0x0f);
            self.text.push(opcode);
            let modrm = 0xc0 | ((d & 7) << 3) | (s & 7);
            self.text.push(modrm);
        }
        Ok(())
    }

    fn encode_movzx(&mut self, opcode: u8, operands: Vec<&str>, _size: usize) -> Result<(), String> {
        if operands.len() != 2 {
            return Ok(());
        }
        let d_opt = try_parse_reg(operands[1]);

        // Try SIB memory source: movzbq (%base,%index,scale), %dst
        if let (Some((off, base, index, sc)), Some(d)) = (try_parse_mem_indexed(operands[0]), d_opt) {
            let rex = 0x48 | (((d >> 3) & 1) << 2) | (((index >> 3) & 1) << 1) | (((base >> 3) & 1) << 0);
            self.text.push(rex);
            self.text.push(0x0f);
            self.text.push(opcode);
            self.encode_modrm_sib(d, off, base, index, sc);
            return Ok(());
        }

        // Try memory source: movzbq offset(%base), %dst
        if let (Some((off, base)), Some(d)) = (try_parse_mem(operands[0]), d_opt) {
            let rex = 0x48 | (((d >> 3) & 1) << 2) | (((base >> 3) & 1) << 0);
            self.text.push(rex);
            self.text.push(0x0f);
            self.text.push(opcode);
            self.encode_modrm_mem(d, off, base);
            return Ok(());
        }

        // Register source: movzbq %al, %rdx → REX.W 0F B6 /r
        let s_opt = try_parse_reg8(operands[0]).or_else(|| try_parse_reg(operands[0]));
        if let (Some(s), Some(d)) = (s_opt, d_opt) {
            let rex = 0x48 | (((d >> 3) & 1) << 2) | (((s >> 3) & 1) << 0);
            self.text.push(rex);
            self.text.push(0x0f);
            self.text.push(opcode);
            let modrm = 0xc0 | ((d & 7) << 3) | (s & 7);
            self.text.push(modrm);
        }
        Ok(())
    }

    fn encode_jmp(&mut self, target: &str) -> Result<(), String> {
        if target.starts_with(".L") || target.starts_with(".L") {
            // Local jump — compute relative offset.
            // We'll emit a placeholder and fix up later.
            // For now, emit jmp rel32 with 0 offset and add a relocation
            // against the local label.
            let sym_idx = self.get_or_add_local_label(target);
            let reloc_offset = self.text.len() as u64 + 1; // after the 0xE9 byte
            self.relocations.push(Relocation {
                offset: reloc_offset,
                symbol: sym_idx,
                rtype: R_X86_64_PC32,
                addend: -4, // rel32 is relative to end of instruction
            });
            self.text.push(0xe9);
            self.text.extend_from_slice(&[0, 0, 0, 0]); // placeholder
        } else {
            // External jump (rare) — treat as call-like.
            self.text.push(0xe9);
            self.text.extend_from_slice(&[0, 0, 0, 0]);
        }
        Ok(())
    }

    fn encode_jcc(&mut self, opcode: u8, operands: Vec<&str>) -> Result<(), String> {
        if operands.len() != 1 {
            return Ok(());
        }
        let target = operands[0];
        // jcc rel32 → 0x0F opcode rel32
        let sym_idx = self.get_or_add_local_label(target);
        let reloc_offset = self.text.len() as u64 + 2; // after 0x0F opcode
        self.relocations.push(Relocation {
            offset: reloc_offset,
            symbol: sym_idx,
            rtype: R_X86_64_PC32,
            addend: -4,
        });
        self.text.push(0x0f);
        self.text.push(opcode);
        self.text.extend_from_slice(&[0, 0, 0, 0]);
        Ok(())
    }

    fn encode_call(&mut self, target: &str) -> Result<(), String> {
        // call *%reg → indirect call through register
        // Encoding: [REX.B] FF /2  (ModRM mod=11, reg=2, rm=reg)
        //   r0-r7:  FF D0|reg
        //   r8-r15: 41 FF D0|(reg-8)
        if target.starts_with("*%") {
            let reg_str = &target[1..]; // keep the % prefix for try_parse_reg
            if let Some(r) = try_parse_reg(reg_str) {
                if r >= 8 {
                    self.text.push(0x41); // REX.B
                }
                self.text.push(0xff);
                let modrm = 0xd0 | (r & 7); // mod=11, reg=2 (CALL), rm=reg
                self.text.push(modrm);
                return Ok(());
            }
        }

        // call *offset(%reg) → indirect call through memory
        // Encoding: [REX] FF /2  (ModRM mod=01, reg=2, rm=reg) disp8
        if target.starts_with("*") {
            let mem_str = &target[1..]; // e.g. "0(%r11)" or "8(%r11)"
            if let Some((off, base)) = try_parse_mem(mem_str) {
                let rex = if base >= 8 { 0x41 } else { 0x00 };
                if rex != 0 { self.text.push(rex); }
                self.text.push(0xff);
                // mod=01 (disp8), reg=2 (CALL), rm=base&7
                let modrm = 0x50 | (2 << 3) | (base & 7); // 0x50 = mod=01
                self.text.push(modrm);
                self.text.push(off as u8); // disp8
                return Ok(());
            }
        }

        // call rel32 → 0xE8 rel32
        // For external functions, use R_X86_64_PLT32 relocation.
        // For local functions, use R_X86_64_PC32.
        if target.starts_with(".L") {
            let sym_idx = self.get_or_add_local_label(target);
            let reloc_offset = self.text.len() as u64 + 1;
            self.relocations.push(Relocation {
                offset: reloc_offset,
                symbol: sym_idx,
                rtype: R_X86_64_PC32,
                addend: -4,
            });
        } else {
            // External function call.
            let sym_idx = self.get_or_add_undef_symbol(target);
            let reloc_offset = self.text.len() as u64 + 1;
            self.relocations.push(Relocation {
                offset: reloc_offset,
                symbol: sym_idx,
                rtype: R_X86_64_PLT32,
                addend: -4,
            });
        }
        self.text.push(0xe8);
        self.text.extend_from_slice(&[0, 0, 0, 0]);
        Ok(())
    }

    fn encode_lea(&mut self, src: &str, dst: &str) -> Result<(), String> {
        // leaq label(%rip), %reg → REX.W 0x8D /r with RIP-relative addressing
        if let Some(d) = try_parse_reg(dst) {
            // Check for label(%rip) pattern.
            if src.contains("(%rip)") {
                let label = src.trim_end_matches("(%rip)").to_string();
                let sym_idx = if label.starts_with(".L") {
                    self.get_or_add_local_label(&label)
                } else {
                    self.get_or_add_undef_symbol(&label)
                };
                let rex = 0x48 | ((d >> 3) << 2);
                self.text.push(rex);
                self.text.push(0x8d);
                // RIP-relative: mod=00, r/m=101
                let modrm = 0x05 | ((d & 7) << 3);
                self.text.push(modrm);
                let reloc_offset = self.text.len() as u64;
                self.relocations.push(Relocation {
                    offset: reloc_offset,
                    symbol: sym_idx,
                    rtype: R_X86_64_PC32,
                    addend: -4,
                });
                self.text.extend_from_slice(&[0, 0, 0, 0]);
                return Ok(());
            }

            // leaq (%base,%index,scale), %reg — SIB addressing
            if let Some((off, base, index, sc)) = try_parse_mem_indexed(src) {
                let rex = 0x48 | ((d >> 3) << 2) | ((index >> 3) << 1) | ((base >> 3) << 0);
                self.text.push(rex);
                self.text.push(0x8d);
                self.encode_modrm_sib(d, off, base, index, sc);
                return Ok(());
            }

            // leaq -offset(%rbp), %reg
            if let Some((off, base)) = try_parse_mem(src) {
                let rex = 0x48 | ((d >> 3) << 2) | ((base >> 3) << 0);
                self.text.push(rex);
                self.text.push(0x8d);
                self.encode_modrm_mem(d, off, base);
                return Ok(());
            }
        }
        self.text.push(0x90);
        Ok(())
    }

    /// Encode movsd: move scalar double-precision FP value.
    /// AT&T syntax: movsd src, dst
    /// Cases:
    ///   movsd .label(%rip), %xmmN  → load from memory (RIP-relative)
    ///   movsd %xmmN, %xmmM         → reg-to-reg move
    ///   movsd %xmmN, mem           → store to memory
    ///   movsd mem, %xmmN           → load from memory
    fn encode_movsd(&mut self, src: &str, dst: &str) -> Result<(), String> {
        // movsd label(%rip), %xmmN
        if src.contains("(%rip)") {
            let dst_xmm = parse_xmm_reg(dst)?;
            let label = src.trim_end_matches("(%rip)").to_string();
            let sym_idx = if label.starts_with(".L") {
                self.get_or_add_local_label(&label)
            } else {
                self.get_or_add_undef_symbol(&label)
            };
            // F2 0F 10 /r with RIP-relative
            // Note: mandatory prefix (F2) comes BEFORE REX prefix.
            let rex = if dst_xmm >= 8 { 0x44 } else { 0x40 }; // REX.R for xmm8-15
            self.text.push(0xf2);
            self.text.push(rex);
            self.text.push(0x0f);
            self.text.push(0x10);
            let modrm = 0x05 | ((dst_xmm & 7) << 3); // mod=00, reg=xmm, rm=101 (RIP)
            self.text.push(modrm);
            let reloc_offset = self.text.len() as u64;
            self.relocations.push(Relocation {
                offset: reloc_offset,
                symbol: sym_idx,
                rtype: R_X86_64_PC32,
                addend: -4,
            });
            self.text.extend_from_slice(&[0, 0, 0, 0]);
            return Ok(());
        }

        // movsd %xmmN, %xmmM (reg-to-reg)
        if let (Ok(s_xmm), Ok(d_xmm)) = (parse_xmm_reg(src), parse_xmm_reg(dst)) {
            let rex = if (s_xmm >= 8) || (d_xmm >= 8) {
                0x40 | ((s_xmm >> 3) << 2) | ((d_xmm >> 3) << 0)
            } else { 0x00 };
            self.text.push(0xf2);
            if rex != 0 { self.text.push(rex); }
            self.text.push(0x0f);
            self.text.push(0x10); // MOVSD xmm1, xmm2
            let modrm = 0xc0 | ((d_xmm & 7) << 3) | (s_xmm & 7);
            self.text.push(modrm);
            return Ok(());
        }

        // movsd %xmmN, mem (store)
        if let Ok(s_xmm) = parse_xmm_reg(src) {
            if let Some((off, base, index, sc)) = try_parse_mem_indexed(dst) {
                let rex = 0x40 | ((s_xmm >> 3) << 2) | ((index >> 3) << 1) | ((base >> 3) << 0);
                self.text.push(0xf2);
                self.text.push(0xf2);
                self.text.push(rex);
                self.text.push(0x0f);
                self.text.push(0x11); // MOVSD mem, xmm
                self.encode_modrm_sib(s_xmm, off, base, index, sc);
                return Ok(());
            }
            if let Some((off, base)) = try_parse_mem(dst) {
                let rex = 0x40 | ((s_xmm >> 3) << 2) | ((base >> 3) << 0);
                self.text.push(0xf2);
                self.text.push(rex);
                self.text.push(0x0f);
                self.text.push(0x11);
                self.encode_modrm_mem(s_xmm, off, base);
                return Ok(());
            }
        }

        // movsd mem, %xmmN (load from [reg+off])
        if let Ok(d_xmm) = parse_xmm_reg(dst) {
            if let Some((off, base, index, sc)) = try_parse_mem_indexed(src) {
                let rex = 0x40 | ((d_xmm >> 3) << 2) | ((index >> 3) << 1) | ((base >> 3) << 0);
                self.text.push(0xf2);
                self.text.push(rex);
                self.text.push(0x0f);
                self.text.push(0x10);
                self.encode_modrm_sib(d_xmm, off, base, index, sc);
                return Ok(());
            }
            if let Some((off, base)) = try_parse_mem(src) {
                let rex = 0x40 | ((d_xmm >> 3) << 2) | ((base >> 3) << 0);
                self.text.push(0xf2);
                self.text.push(rex);
                self.text.push(0x0f);
                self.text.push(0x10);
                self.encode_modrm_mem(d_xmm, off, base);
                return Ok(());
            }
        }

        self.text.push(0x90);
        Ok(())
    }

    /// Encode SSE2 arithmetic instructions (addsd, subsd, mulsd, divsd, ucomisd).
    /// AT&T syntax: op src, dst  →  dst = dst op src
    /// Encoding: F2 [REX] 0F opcode /r
    ///   reg field = dst_xmm, rm field = src_xmm
    ///   (In AT&T syntax, the first operand is src, second is dst.
    ///    In the ModRM byte, reg=dst, rm=src for SSE2 arithmetic.)
    fn encode_sse2_arith(&mut self, opcode: u8, src: &str, dst: &str) -> Result<(), String> {
        let s_xmm = parse_xmm_reg(src)?;
        let d_xmm = parse_xmm_reg(dst)?;
        let rex = if (s_xmm >= 8) || (d_xmm >= 8) {
            0x40 | ((d_xmm >> 3) << 2) | ((s_xmm >> 3) << 0)
        } else { 0x00 };
        self.text.push(0xf2);
        if rex != 0 { self.text.push(rex); }
        self.text.push(0x0f);
        self.text.push(opcode);
        let modrm = 0xc0 | ((d_xmm & 7) << 3) | (s_xmm & 7);
        self.text.push(modrm);
        Ok(())
    }

    /// Encode SSE2 arithmetic instructions with 66 prefix (ucomisd).
    /// AT&T syntax: op src, dst  →  ucomisd src, dst
    /// Encoding: 66 [REX] 0F opcode /r
    ///   reg field = dst_xmm, rm field = src_xmm
    fn encode_sse2_arith_66(&mut self, opcode: u8, src: &str, dst: &str) -> Result<(), String> {
        let s_xmm = parse_xmm_reg(src)?;
        let d_xmm = parse_xmm_reg(dst)?;
        let rex = if (s_xmm >= 8) || (d_xmm >= 8) {
            0x40 | ((d_xmm >> 3) << 2) | ((s_xmm >> 3) << 0)
        } else { 0x00 };
        self.text.push(0x66);
        if rex != 0 { self.text.push(rex); }
        self.text.push(0x0f);
        self.text.push(opcode);
        let modrm = 0xc0 | ((d_xmm & 7) << 3) | (s_xmm & 7);
        self.text.push(modrm);
        Ok(())
    }

    /// Encode SSE (single-precision) arithmetic: addss/subss/mulss/divss.
    /// Uses F3 prefix (instead of F2 for double).
    /// Encoding: F3 [REX] 0F opcode /r
    fn encode_sse_arith(&mut self, opcode: u8, src: &str, dst: &str) -> Result<(), String> {
        let s_xmm = parse_xmm_reg(src)?;
        let d_xmm = parse_xmm_reg(dst)?;
        let rex = if (s_xmm >= 8) || (d_xmm >= 8) {
            0x40 | ((d_xmm >> 3) << 2) | ((s_xmm >> 3) << 0)
        } else { 0x00 };
        self.text.push(0xf3);
        if rex != 0 { self.text.push(rex); }
        self.text.push(0x0f);
        self.text.push(opcode);
        let modrm = 0xc0 | ((d_xmm & 7) << 3) | (s_xmm & 7);
        self.text.push(modrm);
        Ok(())
    }

    /// Encode SSE (single-precision) with 66 prefix (ucomiss).
    fn encode_sse_arith_66(&mut self, opcode: u8, src: &str, dst: &str) -> Result<(), String> {
        let s_xmm = parse_xmm_reg(src)?;
        let d_xmm = parse_xmm_reg(dst)?;
        let rex = if (s_xmm >= 8) || (d_xmm >= 8) {
            0x40 | ((d_xmm >> 3) << 2) | ((s_xmm >> 3) << 0)
        } else { 0x00 };
        self.text.push(0x66);
        if rex != 0 { self.text.push(rex); }
        self.text.push(0x0f);
        self.text.push(opcode);
        let modrm = 0xc0 | ((d_xmm & 7) << 3) | (s_xmm & 7);
        self.text.push(modrm);
        Ok(())
    }

    /// Encode movss: move scalar single-precision FP value.
    fn encode_movss(&mut self, src: &str, dst: &str) -> Result<(), String> {
        // movss label(%rip), %xmmN
        if src.contains("(%rip)") {
            let dst_xmm = parse_xmm_reg(dst)?;
            let label = src.trim_end_matches("(%rip)").to_string();
            let sym_idx = if label.starts_with(".L") {
                self.get_or_add_local_label(&label)
            } else {
                self.get_or_add_undef_symbol(&label)
            };
            let rex = if dst_xmm >= 8 { 0x44 } else { 0x40 };
            self.text.push(0xf3);
            self.text.push(rex);
            self.text.push(0x0f);
            self.text.push(0x10);
            let modrm = 0x05 | ((dst_xmm & 7) << 3);
            self.text.push(modrm);
            let reloc_offset = self.text.len() as u64;
            self.relocations.push(Relocation {
                offset: reloc_offset,
                symbol: sym_idx,
                rtype: R_X86_64_PC32,
                addend: -4,
            });
            self.text.extend_from_slice(&[0, 0, 0, 0]);
            return Ok(());
        }
        // movss %xmmN, %xmmM (reg-to-reg)
        if let (Ok(s_xmm), Ok(d_xmm)) = (parse_xmm_reg(src), parse_xmm_reg(dst)) {
            let rex = if (s_xmm >= 8) || (d_xmm >= 8) {
                0x40 | ((s_xmm >> 3) << 2) | ((d_xmm >> 3) << 0)
            } else { 0x00 };
            self.text.push(0xf3);
            if rex != 0 { self.text.push(rex); }
            self.text.push(0x0f);
            self.text.push(0x10);
            let modrm = 0xc0 | ((d_xmm & 7) << 3) | (s_xmm & 7);
            self.text.push(modrm);
            return Ok(());
        }
        // movss %xmmN, mem (store)
        if let Ok(s_xmm) = parse_xmm_reg(src) {
            if let Some((off, base)) = try_parse_mem(dst) {
                let rex = 0x40 | ((s_xmm >> 3) << 2) | ((base >> 3) << 0);
                self.text.push(0xf3);
                self.text.push(rex);
                self.text.push(0x0f);
                self.text.push(0x11);
                self.encode_modrm_mem(s_xmm, off, base);
                return Ok(());
            }
        }
        // movss mem, %xmmN (load)
        if let Ok(d_xmm) = parse_xmm_reg(dst) {
            if let Some((off, base)) = try_parse_mem(src) {
                let rex = 0x40 | ((d_xmm >> 3) << 2) | ((base >> 3) << 0);
                self.text.push(0xf3);
                self.text.push(rex);
                self.text.push(0x0f);
                self.text.push(0x10);
                self.encode_modrm_mem(d_xmm, off, base);
                return Ok(());
            }
        }
        Ok(())
    }

    // ─── Helpers ───────────────────────────────────────────────────────────

    fn get_or_add_local_label(&mut self, label: &str) -> usize {
        if let Some(&idx) = self.symbol_index.get(label) {
            return idx;
        }
        // Local label — add as undefined for now; it will be defined when
        // we encounter the label.
        let idx = self.symbols.len();
        self.symbols.push(Symbol {
            name: label.to_string(),
            value: 0,
            size: 0,
            binding: STB_LOCAL,
            sym_type: STT_NOTYPE,
            section: SHN_UNDEF,
            is_defined: false,
        });
        self.symbol_index.insert(label.to_string(), idx);
        idx
    }

    fn encode_modrm_mem(&mut self, reg: u8, offset: i32, base: u8) {
        // Encode ModRM + SIB + displacement for [offset + base_reg]
        let reg_field = reg & 7;
        let base_field = base & 7;

        if offset == 0 && base_field != 5 {
            // [base] — mod=00
            let modrm = 0x00 | (reg_field << 3) | base_field;
            self.text.push(modrm);
            if base_field == 4 {
                // SIB needed for r12/rsp
                self.text.push(0x24);
            }
        } else if offset >= -128 && offset <= 127 {
            // [disp8 + base] — mod=01
            let modrm = 0x40 | (reg_field << 3) | base_field;
            self.text.push(modrm);
            if base_field == 4 {
                self.text.push(0x24);
            }
            self.text.push(offset as u8);
        } else {
            // [disp32 + base] — mod=10
            let modrm = 0x80 | (reg_field << 3) | base_field;
            self.text.push(modrm);
            if base_field == 4 {
                self.text.push(0x24);
            }
            self.text.extend_from_slice(&offset.to_le_bytes());
        }
    }

    /// Encode ModRM + SIB + displacement for [offset + base + index*scale]
    /// scale_bits: 0=1x, 1=2x, 2=4x, 3=8x
    fn encode_modrm_sib(&mut self, reg: u8, offset: i32, base: u8, index: u8, scale_bits: u8) {
        let reg_field = reg & 7;
        let base_field = base & 7;
        let index_field = index & 7;

        if offset == 0 && base_field != 5 {
            // [base + index*scale] — mod=00, r/m=100 (SIB)
            let modrm = 0x04 | (reg_field << 3);
            self.text.push(modrm);
            let sib = (scale_bits << 6) | (index_field << 3) | base_field;
            self.text.push(sib);
        } else if offset >= -128 && offset <= 127 {
            // [disp8 + base + index*scale] — mod=01, r/m=100 (SIB)
            let modrm = 0x44 | (reg_field << 3);
            self.text.push(modrm);
            let sib = (scale_bits << 6) | (index_field << 3) | base_field;
            self.text.push(sib);
            self.text.push(offset as u8);
        } else {
            // [disp32 + base + index*scale] — mod=10, r/m=100 (SIB)
            let modrm = 0x84 | (reg_field << 3);
            self.text.push(modrm);
            let sib = (scale_bits << 6) | (index_field << 3) | base_field;
            self.text.push(sib);
            self.text.extend_from_slice(&offset.to_le_bytes());
        }
    }

    // ─── ELF building ──────────────────────────────────────────────────────

    fn build_elf(&mut self) -> Result<Vec<u8>, String> {
        // Section layout:
        //   0: NULL
        //   1: .text
        //   2: .rodata (if non-empty)
        //   3: .symtab
        //   4: .strtab
        //   5: .shstrtab
        //   6: .rela.text (if relocations exist)

        let has_rodata = !self.rodata.is_empty();
        let has_relocations = !self.relocations.is_empty();
        let has_rodata_relocations = !self.rodata_relocations.is_empty();

        // Compute section indices.
        let text_idx: u16 = 1;
        let rodata_idx: u16 = if has_rodata { 2 } else { 0 };
        let symtab_idx: u16 = if has_rodata { 3 } else { 2 };
        let strtab_idx: u16 = if has_rodata { 4 } else { 3 };
        let shstrtab_idx: u16 = if has_rodata { 5 } else { 4 };
        let rela_text_idx: u16 = if has_relocations {
            if has_rodata { 6 } else { 5 }
        } else { 0 };
        let rela_rodata_idx: u16 = if has_rodata_relocations {
            let base = if has_rodata { 6 } else { 5 };
            if has_relocations { base + 1 } else { base }
        } else { 0 };

        // Pre-populate the strtab with all symbol names so we know its size
        // before computing the layout.
        self.add_strtab_static("fides.c"); // FILE symbol
        let sym_names: Vec<String> = self.symbols.iter().map(|s| s.name.clone()).collect();
        for name in &sym_names {
            self.add_strtab_static(name);
        }

        // Build symbol table.
        // First symbol must be STN_UNDEF (null symbol).
        let mut symtab: Vec<u8> = Vec::new();
        // Symbol 0: null
        symtab.extend_from_slice(&[0u8; 24]); // 24 bytes per Elf64Sym

        // Add FILE symbol as first local symbol.
        let file_strtab_off = self.add_strtab_static("fides.c");
        symtab.extend_from_slice(&Elf64Sym {
            st_name: file_strtab_off,
            st_info: (STB_LOCAL << 4) | STT_FILE,
            st_other: 0,
            st_shndx: SHN_ABS,
            st_value: 0,
            st_size: 0,
        }.to_bytes());

        // Add a section symbol for .text (needed for some relocations).
        symtab.extend_from_slice(&Elf64Sym {
            st_name: 0,
            st_info: (STB_LOCAL << 4) | STT_SECTION,
            st_other: 0,
            st_shndx: text_idx,
            st_value: 0,
            st_size: 0,
        }.to_bytes());

        // Now add all symbols from our symbols vec.
        // Local symbols first, then global.
        // Clone the needed data to avoid borrow conflicts with self.symbols
        // while we also call self.add_strtab_static (which borrows self mutably).
        #[derive(Clone)]
        struct SymInfo {
            name: String,
            binding: u8,
            sym_type: u8,
            value: u64,
            size: u64,
            is_defined: bool,
            old_idx: usize,
        }
        let local_symbols: Vec<SymInfo> = self.symbols.iter().enumerate()
            .filter(|(_, s)| s.binding == STB_LOCAL)
            .map(|(i, s)| SymInfo {
                name: s.name.clone(), binding: s.binding, sym_type: s.sym_type,
                value: s.value, size: s.size, is_defined: s.is_defined, old_idx: i,
            })
            .collect();
        let global_symbols: Vec<SymInfo> = self.symbols.iter().enumerate()
            .filter(|(_, s)| s.binding != STB_LOCAL)
            .map(|(i, s)| SymInfo {
                name: s.name.clone(), binding: s.binding, sym_type: s.sym_type,
                value: s.value, size: s.size, is_defined: s.is_defined, old_idx: i,
            })
            .collect();

        // Build a mapping from old index to new index.
        let mut new_index: HashMap<usize, usize> = HashMap::new();
        // Null symbol is at 0, FILE at 1, section at 2.
        let mut sym_idx = 3;

        for sym in &local_symbols {
            new_index.insert(sym.old_idx, sym_idx);
            sym_idx += 1;
        }
        let first_global = sym_idx;
        for sym in &global_symbols {
            new_index.insert(sym.old_idx, sym_idx);
            sym_idx += 1;
        }

        // Build a lookup map for rodata symbols (name → offset in rodata)
        let rodata_map: HashMap<String, u64> = self.rodata_symbols
            .iter()
            .map(|(name, off)| (name.clone(), *off as u64))
            .collect();

        // Emit local symbols.
        for sym in &local_symbols {
            let name_off = self.add_strtab_static(&sym.name);
            // Check if this is a rodata symbol (string constant label)
            let (section, value, is_def) = if let Some(&rod_off) = rodata_map.get(&sym.name) {
                (rodata_idx, rod_off, true)
            } else if sym.is_defined {
                if sym.name.starts_with(".str") || sym.name.starts_with(".flt") {
                    (rodata_idx, sym.value, true)
                } else {
                    (text_idx, sym.value, true)
                }
            } else {
                (SHN_UNDEF, sym.value, false)
            };
            let _ = is_def;
            symtab.extend_from_slice(&Elf64Sym {
                st_name: name_off,
                st_info: (sym.binding << 4) | sym.sym_type,
                st_other: 0,
                st_shndx: section,
                st_value: value,
                st_size: sym.size,
            }.to_bytes());
        }

        // Emit global symbols.
        for sym in &global_symbols {
            let name_off = self.add_strtab_static(&sym.name);
            // Check if this is a rodata symbol
            let (section, value) = if let Some(&rod_off) = rodata_map.get(&sym.name) {
                (rodata_idx, rod_off)
            } else if sym.is_defined {
                (text_idx, sym.value)
            } else {
                (SHN_UNDEF, sym.value)
            };
            symtab.extend_from_slice(&Elf64Sym {
                st_name: name_off,
                st_info: (sym.binding << 4) | sym.sym_type,
                st_other: 0,
                st_shndx: section,
                st_value: value,
                st_size: sym.size,
            }.to_bytes());
        }

        // Build relocation section (.rela.text).
        let mut rela_text: Vec<u8> = Vec::new();
        for reloc in &self.relocations {
            let new_sym = new_index.get(&reloc.symbol).copied().unwrap_or(0);
            rela_text.extend_from_slice(&Elf64Rela {
                r_offset: reloc.offset,
                r_info: ((new_sym as u64) << 32) | (reloc.rtype as u64),
                r_addend: reloc.addend,
            }.to_bytes());
        }

        // Build relocation section (.rela.rodata).
        let mut rela_rodata: Vec<u8> = Vec::new();
        for reloc in &self.rodata_relocations {
            let new_sym = new_index.get(&reloc.symbol).copied().unwrap_or(0);
            rela_rodata.extend_from_slice(&Elf64Rela {
                r_offset: reloc.offset,
                r_info: ((new_sym as u64) << 32) | (reloc.rtype as u64),
                r_addend: reloc.addend,
            }.to_bytes());
        }

        // ─── Layout ────────────────────────────────────────────────────────
        // ELF header: 64 bytes
        // Then sections in order:
        //   .text, .rodata, .symtab, .strtab, .shstrtab, .rela.text
        // Then section header table.

        let elf_header_size = 64u64;
        let mut offset = elf_header_size;

        // .text
        let text_offset = offset;
        let text_size = self.text.len() as u64;
        offset += text_size;

        // .rodata
        let (rodata_offset, rodata_size) = if has_rodata {
            let off = offset;
            let sz = self.rodata.len() as u64;
            offset += sz;
            (off, sz)
        } else {
            (0, 0)
        };

        // .symtab
        let symtab_offset = offset;
        let symtab_size = symtab.len() as u64;
        offset += symtab_size;

        // .strtab
        let strtab_offset = offset;
        let strtab_size = self.strtab.len() as u64;
        offset += strtab_size;

        // .shstrtab
        let shstrtab_offset = offset;
        let shstrtab_size = self.shstrtab.len() as u64;
        offset += shstrtab_size;

        // .rela.text
        let (rela_offset, rela_size) = if has_relocations {
            let off = offset;
            let sz = rela_text.len() as u64;
            offset += sz;
            (off, sz)
        } else {
            (0, 0)
        };

        // .rela.rodata
        let (rela_rodata_offset, rela_rodata_size) = if has_rodata_relocations {
            let off = offset;
            let sz = rela_rodata.len() as u64;
            offset += sz;
            (off, sz)
        } else {
            (0, 0)
        };

        // Section header table
        let shdr_offset = offset;
        let num_sections = 6u64
            + if has_rodata { 1 } else { 0 }
            + if has_relocations { 1 } else { 0 }
            + if has_rodata_relocations { 1 } else { 0 };
        let shdr_size = num_sections * 64;
        offset += shdr_size;

        // ─── Build the output ──────────────────────────────────────────────
        let mut out: Vec<u8> = Vec::with_capacity(offset as usize);

        // ELF header
        out.extend_from_slice(&ELFMAG);
        out.push(ELFCLASS64);
        out.push(ELFDATA2LSB);
        out.push(EV_CURRENT);
        out.push(ELFOSABI_NONE);
        out.extend_from_slice(&[0u8; 8]); // EI_ABIVERSION + padding
        out.extend_from_slice(&ET_REL.to_le_bytes()); // e_type
        out.extend_from_slice(&self.machine.to_le_bytes()); // e_machine
        out.extend_from_slice(&(EV_CURRENT as u32).to_le_bytes()); // e_version (u32)
        out.extend_from_slice(&0u64.to_le_bytes()); // e_entry
        out.extend_from_slice(&0u64.to_le_bytes()); // e_phoff
        out.extend_from_slice(&shdr_offset.to_le_bytes()); // e_shoff
        out.extend_from_slice(&0u32.to_le_bytes()); // e_flags
        out.extend_from_slice(&(64u16).to_le_bytes()); // e_ehsize
        out.extend_from_slice(&0u16.to_le_bytes()); // e_phentsize
        out.extend_from_slice(&0u16.to_le_bytes()); // e_phnum
        out.extend_from_slice(&(64u16).to_le_bytes()); // e_shentsize
        out.extend_from_slice(&(num_sections as u16).to_le_bytes()); // e_shnum
        out.extend_from_slice(&shstrtab_idx.to_le_bytes()); // e_shstrndx

        // Sections
        out.extend_from_slice(&self.text);
        if has_rodata {
            out.extend_from_slice(&self.rodata);
        }
        out.extend_from_slice(&symtab);
        out.extend_from_slice(&self.strtab);
        out.extend_from_slice(&self.shstrtab);
        if has_relocations {
            out.extend_from_slice(&rela_text);
        }
        if has_rodata_relocations {
            out.extend_from_slice(&rela_rodata);
        }

        // Section header table
        // Section 0: NULL
        out.extend_from_slice(&Elf64Shdr::null().to_bytes());

        // Section 1: .text
        out.extend_from_slice(&Elf64Shdr {
            sh_name: self.shstrtab_offsets[".text"],
            sh_type: SHT_PROGBITS,
            sh_flags: SHF_ALLOC | SHF_EXECINSTR,
            sh_addr: 0,
            sh_offset: text_offset,
            sh_size: text_size,
            sh_link: 0,
            sh_info: 0,
            sh_addralign: 16,
            sh_entsize: 0,
        }.to_bytes());

        // Section 2: .rodata (if present)
        if has_rodata {
            out.extend_from_slice(&Elf64Shdr {
                sh_name: self.shstrtab_offsets[".rodata"],
                sh_type: SHT_PROGBITS,
                sh_flags: SHF_ALLOC,
                sh_addr: 0,
                sh_offset: rodata_offset,
                sh_size: rodata_size,
                sh_link: 0,
                sh_info: 0,
                sh_addralign: 8,
                sh_entsize: 0,
            }.to_bytes());
        }

        // Section: .symtab
        out.extend_from_slice(&Elf64Shdr {
            sh_name: self.shstrtab_offsets[".symtab"],
            sh_type: SHT_SYMTAB,
            sh_flags: 0,
            sh_addr: 0,
            sh_offset: symtab_offset,
            sh_size: symtab_size,
            sh_link: strtab_idx as u32, // link to .strtab
            sh_info: first_global as u32, // index of first non-local symbol
            sh_addralign: 8,
            sh_entsize: 24, // sizeof(Elf64Sym)
        }.to_bytes());

        // Section: .strtab
        out.extend_from_slice(&Elf64Shdr {
            sh_name: self.shstrtab_offsets[".strtab"],
            sh_type: SHT_STRTAB,
            sh_flags: 0,
            sh_addr: 0,
            sh_offset: strtab_offset,
            sh_size: strtab_size,
            sh_link: 0,
            sh_info: 0,
            sh_addralign: 1,
            sh_entsize: 0,
        }.to_bytes());

        // Section: .shstrtab
        out.extend_from_slice(&Elf64Shdr {
            sh_name: self.shstrtab_offsets[".shstrtab"],
            sh_type: SHT_STRTAB,
            sh_flags: 0,
            sh_addr: 0,
            sh_offset: shstrtab_offset,
            sh_size: shstrtab_size,
            sh_link: 0,
            sh_info: 0,
            sh_addralign: 1,
            sh_entsize: 0,
        }.to_bytes());

        // Section: .rela.text (if present)
        if has_relocations {
            out.extend_from_slice(&Elf64Shdr {
                sh_name: self.shstrtab_offsets[".rela.text"],
                sh_type: SHT_RELA,
                sh_flags: SHF_INFO_LINK,
                sh_addr: 0,
                sh_offset: rela_offset,
                sh_size: rela_size,
                sh_link: symtab_idx as u32, // link to .symtab
                sh_info: text_idx as u32,   // link to .text
                sh_addralign: 8,
                sh_entsize: 24, // sizeof(Elf64Rela)
            }.to_bytes());
        }

        // Section: .rela.rodata (if present)
        if has_rodata_relocations {
            out.extend_from_slice(&Elf64Shdr {
                sh_name: self.shstrtab_offsets[".rela.rodata"],
                sh_type: SHT_RELA,
                sh_flags: SHF_INFO_LINK,
                sh_addr: 0,
                sh_offset: rela_rodata_offset,
                sh_size: rela_rodata_size,
                sh_link: symtab_idx as u32, // link to .symtab
                sh_info: rodata_idx as u32, // link to .rodata
                sh_addralign: 8,
                sh_entsize: 24,
            }.to_bytes());
        }

        Ok(out)
    }

    fn add_strtab_static(&mut self, s: &str) -> u32 {
        if let Some(&off) = self.strtab_offsets.get(s) {
            return off;
        }
        let off = self.strtab.len() as u32;
        self.strtab.extend_from_slice(s.as_bytes());
        self.strtab.push(0);
        self.strtab_offsets.insert(s.to_string(), off);
        off
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// ELF structures
// ═══════════════════════════════════════════════════════════════════════════

struct Elf64Shdr {
    sh_name: u32,
    sh_type: u32,
    sh_flags: u64,
    sh_addr: u64,
    sh_offset: u64,
    sh_size: u64,
    sh_link: u32,
    sh_info: u32,
    sh_addralign: u64,
    sh_entsize: u64,
}

impl Elf64Shdr {
    fn null() -> Self {
        Elf64Shdr {
            sh_name: 0,
            sh_type: SHT_NULL,
            sh_flags: 0,
            sh_addr: 0,
            sh_offset: 0,
            sh_size: 0,
            sh_link: 0,
            sh_info: 0,
            sh_addralign: 0,
            sh_entsize: 0,
        }
    }

    fn to_bytes(&self) -> Vec<u8> {
        let mut v = Vec::with_capacity(64);
        v.extend_from_slice(&self.sh_name.to_le_bytes());
        v.extend_from_slice(&self.sh_type.to_le_bytes());
        v.extend_from_slice(&self.sh_flags.to_le_bytes());
        v.extend_from_slice(&self.sh_addr.to_le_bytes());
        v.extend_from_slice(&self.sh_offset.to_le_bytes());
        v.extend_from_slice(&self.sh_size.to_le_bytes());
        v.extend_from_slice(&self.sh_link.to_le_bytes());
        v.extend_from_slice(&self.sh_info.to_le_bytes());
        v.extend_from_slice(&self.sh_addralign.to_le_bytes());
        v.extend_from_slice(&self.sh_entsize.to_le_bytes());
        v
    }
}

struct Elf64Sym {
    st_name: u32,
    st_info: u8,
    st_other: u8,
    st_shndx: u16,
    st_value: u64,
    st_size: u64,
}

impl Elf64Sym {
    fn to_bytes(&self) -> Vec<u8> {
        let mut v = Vec::with_capacity(24);
        v.extend_from_slice(&self.st_name.to_le_bytes());
        v.push(self.st_info);
        v.push(self.st_other);
        v.extend_from_slice(&self.st_shndx.to_le_bytes());
        v.extend_from_slice(&self.st_value.to_le_bytes());
        v.extend_from_slice(&self.st_size.to_le_bytes());
        v
    }
}

struct Elf64Rela {
    r_offset: u64,
    r_info: u64,
    r_addend: i64,
}

impl Elf64Rela {
    fn to_bytes(&self) -> Vec<u8> {
        let mut v = Vec::with_capacity(24);
        v.extend_from_slice(&self.r_offset.to_le_bytes());
        v.extend_from_slice(&self.r_info.to_le_bytes());
        v.extend_from_slice(&self.r_addend.to_le_bytes());
        v
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Register parsing
// ═══════════════════════════════════════════════════════════════════════════

/// Parse a register operand like "%rax" → Some(0), "%r8" → Some(8), etc.
/// Returns the register number (0-15) or None if not a register.
/// Also accepts 32-bit register names (%eax, %r8d, etc.) since the register
/// number is the same — the size is determined by the instruction prefix.
fn try_parse_reg(s: &str) -> Option<u8> {
    let s = s.trim();
    if !s.starts_with('%') {
        return None;
    }
    let r = &s[1..];
    let reg = match r {
        // 64-bit names
        "rax" => 0, "rcx" => 1, "rdx" => 2, "rbx" => 3,
        "rsp" => 4, "rbp" => 5, "rsi" => 6, "rdi" => 7,
        "r8" => 8, "r9" => 9, "r10" => 10, "r11" => 11,
        "r12" => 12, "r13" => 13, "r14" => 14, "r15" => 15,
        // 32-bit names (same register numbers)
        "eax" => 0, "ecx" => 1, "edx" => 2, "ebx" => 3,
        "esp" => 4, "ebp" => 5, "esi" => 6, "edi" => 7,
        "r8d" => 8, "r9d" => 9, "r10d" => 10, "r11d" => 11,
        "r12d" => 12, "r13d" => 13, "r14d" => 14, "r15d" => 15,
        _ => return None,
    };
    Some(reg)
}

/// Check if a register operand is 32-bit (e.g., "%eax", "%r8d").
fn is_reg32(s: &str) -> bool {
    let s = s.trim();
    if !s.starts_with('%') {
        return false;
    }
    let r = &s[1..];
    matches!(
        r,
        "eax" | "ecx" | "edx" | "ebx" | "esp" | "ebp" | "esi" | "edi"
            | "r8d" | "r9d" | "r10d" | "r11d" | "r12d" | "r13d" | "r14d" | "r15d"
    )
}

fn parse_reg(s: &str) -> Result<u8, String> {
    try_parse_reg(s).ok_or_else(|| format!("expected register, got '{}'", s))
}

/// Parse a 32-bit register (for setcc operands like %al, %eax, etc.)
fn try_parse_reg8(s: &str) -> Option<u8> {
    let s = s.trim();
    if !s.starts_with('%') {
        return None;
    }
    let r = &s[1..];
    let reg = match r {
        "al" => 0, "cl" => 1, "dl" => 2, "bl" => 3,
        "sil" => 6, "dil" => 7,
        "r8b" => 8, "r9b" => 9, "r10b" => 10, "r11b" => 11,
        "r12b" => 12, "r13b" => 13, "r14b" => 14, "r15b" => 15,
        _ => return None,
    };
    Some(reg)
}

/// Parse an XMM register.
fn parse_xmm_reg(s: &str) -> Result<u8, String> {
    let s = s.trim();
    if !s.starts_with('%') {
        return Err(format!("expected xmm register, got '{}'", s));
    }
    let r = &s[1..];
    if r.starts_with("xmm") {
        let n: u8 = r[3..].parse().unwrap_or(0);
        Ok(n)
    } else {
        Err(format!("expected xmm register, got '{}'", s))
    }
}

/// Parse a memory operand like "-16(%rbp)" → Some((-16, 5)).
/// Returns (offset, base_reg_number).
fn try_parse_mem(s: &str) -> Option<(i32, u8)> {
    let s = s.trim();
    // Pattern: offset(%reg) or (%reg) — but NOT (%reg,%reg) which is SIB
    if !s.contains("(%") {
        return None;
    }
    let paren_idx = s.find('(')?;
    let offset_str = &s[..paren_idx];
    let offset: i32 = if offset_str.is_empty() {
        0
    } else {
        offset_str.parse().ok()?
    };

    let rest = &s[paren_idx + 1..];
    let end_idx = rest.find(')')?;
    let reg_str = &rest[..end_idx];
    // Reject SIB patterns (containing comma)
    if reg_str.contains(',') {
        return None;
    }
    let base = try_parse_reg(reg_str)?;
    Some((offset, base))
}

/// Parse indexed memory operand: offset(%base,%index) or (%base,%index) or (%base,%index,scale)
/// Returns (offset, base, index, scale_bits) where scale_bits: 0=1x, 1=2x, 2=4x, 3=8x
fn try_parse_mem_indexed(s: &str) -> Option<(i32, u8, u8, u8)> {
    let s = s.trim();
    if !s.contains("(%") {
        return None;
    }
    let paren_idx = s.find('(')?;
    let offset_str = &s[..paren_idx];
    let offset: i32 = if offset_str.is_empty() {
        0
    } else {
        offset_str.parse().ok()?
    };

    let rest = &s[paren_idx + 1..];
    let end_idx = rest.find(')')?;
    let inner = &rest[..end_idx];
    // Must contain comma for SIB
    let comma_idx = inner.find(',')?;
    let base_str = inner[..comma_idx].trim();
    let remaining = inner[comma_idx + 1..].trim();
    // Check for scale: "index,scale" or just "index"
    let (index_str, scale) = if let Some(c2) = remaining.find(',') {
        let idx = remaining[..c2].trim();
        let sc: u8 = remaining[c2 + 1..].trim().parse().ok()?;
        (idx, sc)
    } else {
        (remaining, 1u8)
    };
    let base = try_parse_reg(base_str)?;
    let index = try_parse_reg(index_str)?;
    // Scale must be 1, 2, 4, or 8
    let scale_bits = match scale {
        1 => 0u8,
        2 => 1,
        4 => 2,
        8 => 3,
        _ => return None,
    };
    Some((offset, base, index, scale_bits))
}

/// Build a REX prefix with optional W bit.
fn rex_w(size: usize) -> u8 {
    if size == 8 { 0x48 } else { 0x40 }
}

/// Extract a string literal from a .asciz/.ascii directive.
fn extract_string_literal(s: &str) -> String {
    if let Some(start) = s.find('"') {
        if let Some(end) = s.rfind('"') {
            if end > start {
                let raw = &s[start + 1..end];
                // Unescape.
                return raw
                    .replace("\\n", "\n")
                    .replace("\\t", "\t")
                    .replace("\\r", "\r")
                    .replace("\\\"", "\"")
                    .replace("\\\\", "\\");
            }
        }
    }
    String::new()
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn elf_header_magic() {
        let asm = "main:\n    ret\n";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        // ELF magic.
        assert_eq!(&elf[..4], &[0x7f, b'E', b'L', b'F']);
        // 64-bit.
        assert_eq!(elf[4], ELFCLASS64);
        // Little-endian.
        assert_eq!(elf[5], ELFDATA2LSB);
        // Relocatable file.
        assert_eq!(u16::from_le_bytes([elf[16], elf[17]]), ET_REL);
        // x86-64.
        assert_eq!(u16::from_le_bytes([elf[18], elf[19]]), EM_X86_64);
    }

    #[test]
    fn elf_has_text_section() {
        let asm = "main:\n    ret\n";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        // Should be at least 64 bytes (header) + some sections.
        assert!(elf.len() > 64, "ELF should be larger than just the header");
    }

    #[test]
    fn elf_contains_ret_instruction() {
        let asm = "main:\n    ret\n";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        // The .text section starts right after the 64-byte ELF header.
        // The first byte of .text should be 0xc3 (ret).
        assert_eq!(elf[64], 0xc3, "first .text byte should be return (0xc3)");
    }

    #[test]
    fn elf_encodes_movq_imm() {
        let asm = "main:\n    movq $42, %rax\n    ret\n";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        // movq $42, %rax → REX.W 0xB8 + imm64
        // REX.W = 0x48, 0xB8, then 42 as 8 bytes
        assert_eq!(elf[64], 0x48, "REX.W prefix");
        assert_eq!(elf[65], 0xb8, "mov rax, imm64 opcode");
        let imm = i64::from_le_bytes([elf[66], elf[67], elf[68], elf[69], elf[70], elf[71], elf[72], elf[73]]);
        assert_eq!(imm, 42);
    }

    #[test]
    fn elf_encodes_pushq_rbp() {
        let asm = "main:\n    pushq %rbp\n    ret\n";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        // pushq %rbp → 0x55
        assert_eq!(elf[64], 0x55, "pushq %rbp should be 0x55");
    }

    #[test]
    fn elf_encodes_addq() {
        let asm = "main:\n    addq $5, %rax\n    ret\n";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        // addq $5, %rax → REX.W 0x83 0xC0 0x05
        assert_eq!(elf[64], 0x48);
        assert_eq!(elf[65], 0x83);
        assert_eq!(elf[66], 0xc0);
        assert_eq!(elf[67], 5);
    }

    #[test]
    fn elf_encodes_xorl_zero_idiom() {
        let asm = "main:\n    xorl %eax, %eax\n    ret\n";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        // xorl %eax, %eax → 0x31 0xC0
        assert_eq!(elf[64], 0x31);
        assert_eq!(elf[65], 0xc0);
    }

    #[test]
    fn elf_encodes_call_with_relocation() {
        let asm = "main:\n    call foo\n    ret\n";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        // call rel32 → 0xE8 + 4 bytes (should be 0s, with relocation)
        assert_eq!(elf[64], 0xe8, "call opcode");
        // The 4 bytes after should be zeros (placeholder for relocation).
        assert_eq!(&elf[65..69], &[0, 0, 0, 0], "call rel32 placeholder");
    }

    #[test]
    fn elf_encodes_local_jmp() {
        let asm = "main:\n    jmp .L1\n.L1:\n    ret\n";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        // jmp rel32 → 0xE9 + 4 bytes
        assert_eq!(elf[64], 0xe9, "jmp opcode");
    }

    #[test]
    fn elf_encodes_leave_ret() {
        let asm = "main:\n    leave\n    ret\n";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        assert_eq!(elf[64], 0xc9, "leave");
        assert_eq!(elf[65], 0xc3, "ret");
    }

    #[test]
    fn elf_has_multiple_sections() {
        let asm = "main:\n    ret\n";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        // Read e_shnum from header (offset 60, 2 bytes).
        let shnum = u16::from_le_bytes([elf[60], elf[61]]);
        assert!(shnum >= 5, "should have at least 5 sections, got {}", shnum);
    }

    #[test]
    fn elf_section_names_present() {
        let asm = "main:\n    ret\n";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        // The .shstrtab section should contain ".text", ".symtab", etc.
        // Find the shstrtab in the ELF.
        let elf_str = String::from_utf8_lossy(&elf);
        assert!(elf_str.contains(".text"), "shstrtab should contain .text");
        assert!(elf_str.contains(".symtab"), "shstrtab should contain .symtab");
        assert!(elf_str.contains(".strtab"), "shstrtab should contain .strtab");
    }

    #[test]
    fn elf_valid_with_rodata() {
        let asm = "main:\n    leaq .str_0(%rip), %rdi\n    ret\n.str_0: .asciz \"hello\"";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        // Should have .rodata section.
        let elf_str = String::from_utf8_lossy(&elf);
        assert!(elf_str.contains(".rodata"), "should have .rodata section");
        assert!(elf_str.contains("hello"), "should contain the string constant");
    }

    #[test]
    fn parse_reg_works() {
        assert_eq!(try_parse_reg("%rax"), Some(0));
        assert_eq!(try_parse_reg("%rbp"), Some(5));
        assert_eq!(try_parse_reg("%r8"), Some(8));
        assert_eq!(try_parse_reg("%r15"), Some(15));
        assert_eq!(try_parse_reg("rax"), None);
        assert_eq!(try_parse_reg("$5"), None);
    }

    #[test]
    fn parse_mem_works() {
        assert_eq!(try_parse_mem("-16(%rbp)"), Some((-16, 5)));
        assert_eq!(try_parse_mem("8(%rbp)"), Some((8, 5)));
        assert_eq!(try_parse_mem("(%rbp)"), Some((0, 5)));
        assert_eq!(try_parse_mem("%rax"), None);
    }

    #[test]
    fn extract_string_literal_works() {
        assert_eq!(extract_string_literal(".asciz \"hello\""), "hello");
        assert_eq!(extract_string_literal(".asciz \"a\\nb\""), "a\nb");
        assert_eq!(extract_string_literal(".asciz \"\""), "");
    }

    #[test]
    fn elf_multiple_functions() {
        let asm = "main:\n    call helper\n    ret\nhelper:\n    ret\n";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        let elf_str = String::from_utf8_lossy(&elf);
        assert!(elf_str.contains("main"), "should have main symbol");
        assert!(elf_str.contains("helper"), "should have helper symbol");
    }

    #[test]
    fn elf_file_is_valid_size() {
        let asm = "main:\n    movq $42, %rax\n    ret\n";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        // Should be a reasonable size: header (64) + text (~10) + symtab + strtab + shstrtab + shdrs
        assert!(elf.len() > 200, "ELF should be at least 200 bytes, got {}", elf.len());
        assert!(elf.len() < 10000, "ELF should be under 10KB for this simple program, got {}", elf.len());
    }

    #[test]
    fn elf_readelf_compatible() {
        // This test checks that the ELF header fields are in valid ranges.
        let asm = "main:\n    ret\n";
        let elf = emit_object_file(&crate::pir::L3Module::default(), asm).unwrap();
        assert_eq!(&elf[..4], &[0x7f, b'E', b'L', b'F'], "magic");
        assert_eq!(elf[4], 2, "ELFCLASS64");
        assert_eq!(elf[5], 1, "ELFDATA2LSB");
        assert_eq!(elf[6], 1, "EV_CURRENT");
        let e_type = u16::from_le_bytes([elf[16], elf[17]]);
        assert_eq!(e_type, 1, "ET_REL");
        let e_machine = u16::from_le_bytes([elf[18], elf[19]]);
        assert_eq!(e_machine, 62, "EM_X86_64");
        let e_ehsize = u16::from_le_bytes([elf[52], elf[53]]);
        assert_eq!(e_ehsize, 64, "e_ehsize");
        let e_shentsize = u16::from_le_bytes([elf[58], elf[59]]);
        assert_eq!(e_shentsize, 64, "e_shentsize");
    }

    #[test]
    fn elf_arm64_machine_type() {
        let asm = "main:\n    ret\n";
        let elf = emit_object_file_for_target(&crate::pir::L3Module::default(), asm, "arm64").unwrap();
        assert_eq!(&elf[..4], &[0x7f, b'E', b'L', b'F'], "magic");
        let e_machine = u16::from_le_bytes([elf[18], elf[19]]);
        assert_eq!(e_machine, 183, "EM_AARCH64");
    }

    #[test]
    fn elf_riscv_machine_type() {
        let asm = "main:\n    ret\n";
        let elf = emit_object_file_for_target(&crate::pir::L3Module::default(), asm, "riscv").unwrap();
        assert_eq!(&elf[..4], &[0x7f, b'E', b'L', b'F'], "magic");
        let e_machine = u16::from_le_bytes([elf[18], elf[19]]);
        assert_eq!(e_machine, 243, "EM_RISCV");
    }
}

/// Encode a block of assembly text to raw bytes + symbols + relocations.
/// Used by runtime_linux.rs to encode runtime functions without hand-encoding bytes.
/// Returns (text_bytes, symbols, relocations) where:
///   symbols: (name, offset_in_text)
///   relocations: (offset_in_text, target_name, rtype_u8)  rtype: 1=64, 2=PC32, 4=PLT32
pub fn encode_asm_block(asm: &str) -> Result<(Vec<u8>, Vec<(String, u64)>, Vec<(u64, String, u8)>), String> {
    let mut enc = ElfEmitter::new();
    enc.assemble(asm)?;

    let symbols: Vec<(String, u64)> = enc.symbols.iter()
        .filter(|s| s.is_defined && s.section == 1)
        .map(|s| (s.name.clone(), s.value))
        .collect();

    let relocations: Vec<(u64, String, u8)> = enc.relocations.iter()
        .filter_map(|r| {
            enc.symbols.get(r.symbol).map(|s| {
                let rtype = match r.rtype {
                    R_X86_64_64 => 1,
                    R_X86_64_PC32 => 2,
                    R_X86_64_PLT32 => 4,
                    _ => 2,
                };
                (r.offset, s.name.clone(), rtype)
            })
        })
        .collect();

    Ok((enc.text, symbols, relocations))
}
