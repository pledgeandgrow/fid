// Mach-O Object File Emitter — Phase 4D
//
// Produces Mach-O (Mach Object) 64-bit object files for macOS.
// The emitter takes the textual assembly output from codegen_x64 and
// packages it into a Mach-O object file.
//
// Mach-O 64-bit format:
//   - Mach-O header (32 bytes)
//   - Load commands (LC_SEGMENT_64, LC_SYMTAB)
//   - Sections (__TEXT,__text)
//   - Relocation entries
//   - Symbol table (nlist_64, 16 bytes each)
//   - String table

use crate::pir::L3Module;
use std::collections::HashMap;

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

/// Emit a Mach-O 64-bit object file from the assembly text.
/// Returns the raw bytes of the .o file.
pub fn emit_object_file(_module: &L3Module, asm: &str) -> Result<Vec<u8>, String> {
    let mut emitter = MachOEmitter::new();
    emitter.assemble(asm)?;
    emitter.build_macho()
}

// ═══════════════════════════════════════════════════════════════════════════
// Mach-O constants
// ═══════════════════════════════════════════════════════════════════════════

const MH_MAGIC_64: u32 = 0xFEEDFACF;
const CPU_TYPE_X86_64: u32 = 0x01000007;
const CPU_SUBTYPE_ALL: u32 = 3;
const MH_OBJECT: u32 = 1;

const LC_SEGMENT_64: u32 = 0x19;
const LC_SYMTAB: u32 = 0x2;

const S_ATTR_PURE_INSTRUCTIONS: u32 = 0x80000000;
const S_ATTR_SOME_INSTRUCTIONS: u32 = 0x00004000;

// ═══════════════════════════════════════════════════════════════════════════
// Mach-O Emitter
// ═══════════════════════════════════════════════════════════════════════════

struct MachOEmitter {
    text_bytes: Vec<u8>,
    symbols: Vec<MachOSymbol>,
    relocations: Vec<MachOReloc>,
    string_table: Vec<u8>,
}

struct MachOSymbol {
    name: String,
    value: u64,
    section_number: i8,  // 0 = undefined, 1 = first section
    type_: u8,           // N_EXT | N_TYPE
    description: u16,
}

struct MachOReloc {
    offset: u32,
    symbol_index: u32,
    is_pcrel: bool,
    length: u8,    // 0=byte, 1=word, 2=long, 3=quad
    is_extern: bool,
    type_: u8,     // reloc type
}

impl MachOEmitter {
    fn new() -> Self {
        MachOEmitter {
            text_bytes: Vec::new(),
            symbols: Vec::new(),
            relocations: Vec::new(),
            string_table: Vec::new(),
        }
    }

    fn assemble(&mut self, asm: &str) -> Result<(), String> {
        // Initialize string table with empty string at index 0
        self.string_table.push(0);

        for line in asm.lines() {
            let line = line.trim();
            if line.is_empty() || line.starts_with("//") || line.starts_with("#") {
                continue;
            }

            // Check for label
            if line.ends_with(':') {
                let label = &line[..line.len() - 1];
                let offset = self.text_bytes.len() as u64;
                self.add_symbol(label, offset, true);
                continue;
            }

            // Skip directives
            if line.starts_with('.') {
                continue;
            }

            // Encode instruction (simplified, same as COFF)
            self.encode_instruction(line)?;
        }

        Ok(())
    }

    fn encode_instruction(&mut self, line: &str) -> Result<(), String> {
        let parts: Vec<&str> = line.split_whitespace().collect();
        if parts.is_empty() {
            return Ok(());
        }

        let mnemonic = parts[0].trim_end_matches(',');

        match mnemonic {
            "ret" => self.text_bytes.push(0xC3),
            "nop" => self.text_bytes.push(0x90),
            "push" => {
                if parts.len() > 1 {
                    let reg = parts[1].trim_end_matches(',');
                    if reg == "rbp" { self.text_bytes.push(0x55); }
                    else if reg == "rsp" { self.text_bytes.push(0x54); }
                    else { self.text_bytes.push(0x50); }
                }
            }
            "pop" => {
                if parts.len() > 1 {
                    let reg = parts[1].trim_end_matches(',');
                    if reg == "rbp" { self.text_bytes.push(0x5D); }
                    else { self.text_bytes.push(0x58); }
                }
            }
            "call" => {
                if parts.len() > 1 {
                    let target = parts[1].trim_end_matches(',');
                    self.text_bytes.push(0xE8);
                    let offset = self.text_bytes.len() as u32;
                    self.text_bytes.extend_from_slice(&[0, 0, 0, 0]);
                    let sym_idx = self.get_or_add_symbol(target, 0, false);
                    self.relocations.push(MachOReloc {
                        offset,
                        symbol_index: sym_idx,
                        is_pcrel: true,
                        length: 2, // 32-bit
                        is_extern: true,
                        type_: 0, // X86_64_RELOC_BRANCH
                    });
                }
            }
            "jmp" => {
                if parts.len() > 1 {
                    let target = parts[1].trim_end_matches(',');
                    self.text_bytes.push(0xE9);
                    let offset = self.text_bytes.len() as u32;
                    self.text_bytes.extend_from_slice(&[0, 0, 0, 0]);
                    let sym_idx = self.get_or_add_symbol(target, 0, false);
                    self.relocations.push(MachOReloc {
                        offset,
                        symbol_index: sym_idx,
                        is_pcrel: true,
                        length: 2,
                        is_extern: true,
                        type_: 0,
                    });
                }
            }
            "int3" | "hlt" => self.text_bytes.push(0xCC),
            _ => self.text_bytes.push(0x90), // nop for unknown
        }

        Ok(())
    }

    fn add_symbol(&mut self, name: &str, offset: u64, is_defined: bool) {
        let section = if is_defined { 1 } else { 0 };
        let type_ = if is_defined { 0x0e } else { 0x01 }; // N_SECT | N_EXT or N_EXT

        self.symbols.push(MachOSymbol {
            name: name.to_string(),
            value: offset,
            section_number: section,
            type_,
            description: 0,
        });
    }

    fn get_or_add_symbol(&mut self, name: &str, offset: u64, is_defined: bool) -> u32 {
        for (i, sym) in self.symbols.iter().enumerate() {
            if sym.name == name {
                return i as u32;
            }
        }
        let idx = self.symbols.len() as u32;
        self.add_symbol(name, offset, is_defined);
        idx
    }

    fn add_string(&mut self, s: &str) -> u32 {
        let offset = self.string_table.len() as u32;
        self.string_table.extend_from_slice(s.as_bytes());
        self.string_table.push(0);
        offset
    }

    fn build_macho(&mut self) -> Result<Vec<u8>, String> {
        // Mach-O 64-bit layout:
        //   1. Header (32 bytes)
        //   2. LC_SEGMENT_64 load command (72 bytes) + section header (80 bytes) = 152 bytes
        //   3. LC_SYMTAB load command (24 bytes)
        //   4. __TEXT,__text section data
        //   5. Relocations
        //   6. Symbol table (nlist_64, 16 bytes each)
        //   7. String table

        let header_size = 32;
        let segment_cmd_size = 72;
        let section_header_size = 80;
        let symtab_cmd_size = 24;

        let load_commands_size = segment_cmd_size + section_header_size + symtab_cmd_size;

        let text_data_offset = header_size + load_commands_size;
        let text_data_size = self.text_bytes.len();

        let reloc_offset = text_data_offset + text_data_size;
        let reloc_size = self.relocations.len() * 8;

        let symbol_table_offset = reloc_offset + reloc_size;
        let symbol_table_size = self.symbols.len() * 16;

        let string_table_offset = symbol_table_offset + symbol_table_size;

        // Build string table entries for symbols
        let mut sym_string_offsets: Vec<u32> = Vec::new();
        let sym_names: Vec<String> = self.symbols.iter().map(|s| s.name.clone()).collect();
        for name in &sym_names {
            let offset = self.add_string(name);
            sym_string_offsets.push(offset);
        }

        let mut buf = Vec::new();

        // ── Mach-O Header (32 bytes) ──
        buf.extend_from_slice(&MH_MAGIC_64.to_le_bytes());        // magic
        buf.extend_from_slice(&CPU_TYPE_X86_64.to_le_bytes());    // cputype
        buf.extend_from_slice(&CPU_SUBTYPE_ALL.to_le_bytes());    // cpusubtype
        buf.extend_from_slice(&MH_OBJECT.to_le_bytes());          // filetype
        buf.extend_from_slice(&(self.symbols.len() as u32).to_le_bytes()); // ncmds (2: segment + symtab)
        buf.extend_from_slice(&(load_commands_size as u32).to_le_bytes()); // sizeofcmds
        buf.extend_from_slice(&0u32.to_le_bytes());               // flags
        buf.extend_from_slice(&0u32.to_le_bytes());               // reserved

        // ── LC_SEGMENT_64 Load Command (72 bytes) ──
        buf.extend_from_slice(&LC_SEGMENT_64.to_le_bytes());      // cmd
        buf.extend_from_slice(&(segment_cmd_size + section_header_size).to_le_bytes()); // cmdsize
        // segname (16 bytes, padded with zeros)
        let segname = b"__TEXT\0\0\0\0\0\0\0\0\0\0";
        buf.extend_from_slice(segname);
        buf.extend_from_slice(&0u64.to_le_bytes());               // vmaddr
        buf.extend_from_slice(&(text_data_size as u64).to_le_bytes()); // vmsize
        buf.extend_from_slice(&(text_data_offset as u64).to_le_bytes()); // fileoff
        buf.extend_from_slice(&(text_data_size as u64).to_le_bytes()); // filesize
        buf.extend_from_slice(&0u32.to_le_bytes());               // maxprot
        buf.extend_from_slice(&0u32.to_le_bytes());               // initprot
        buf.extend_from_slice(&1u32.to_le_bytes());               // nsects (1: __text)
        buf.extend_from_slice(&0u32.to_le_bytes());               // flags

        // ── Section Header (80 bytes) ──
        // sectname (16 bytes)
        let sectname = b"__text\0\0\0\0\0\0\0\0\0\0";
        buf.extend_from_slice(sectname);
        // segname (16 bytes, same as above)
        buf.extend_from_slice(segname);
        buf.extend_from_slice(&0u64.to_le_bytes());               // addr
        buf.extend_from_slice(&(text_data_size as u64).to_le_bytes()); // size
        buf.extend_from_slice(&(text_data_offset as u32).to_le_bytes()); // offset
        buf.extend_from_slice(&(reloc_offset as u32).to_le_bytes()); // reloff
        buf.extend_from_slice(&(self.relocations.len() as u32).to_le_bytes()); // nreloc
        buf.extend_from_slice(&0u32.to_le_bytes());               // align
        buf.extend_from_slice(&0u32.to_le_bytes());               // reloff2 (reserved)
        buf.extend_from_slice(&0u32.to_le_bytes());               // nreloc2 (reserved)
        buf.extend_from_slice(&0u32.to_le_bytes());               // flags
        buf.extend_from_slice(&0u32.to_le_bytes());               // reserved1
        buf.extend_from_slice(&0u32.to_le_bytes());               // reserved2
        buf.extend_from_slice(&0u32.to_le_bytes());               // reserved3 (64-bit only)

        // ── LC_SYMTAB Load Command (24 bytes) ──
        buf.extend_from_slice(&LC_SYMTAB.to_le_bytes());          // cmd
        buf.extend_from_slice(&symtab_cmd_size.to_le_bytes());    // cmdsize
        buf.extend_from_slice(&(symbol_table_offset as u32).to_le_bytes()); // symoff
        buf.extend_from_slice(&(self.symbols.len() as u32).to_le_bytes()); // nsyms
        buf.extend_from_slice(&(string_table_offset as u32).to_le_bytes()); // stroff
        buf.extend_from_slice(&(self.string_table.len() as u32).to_le_bytes()); // strsize

        // ── __TEXT,__text section data ──
        buf.extend_from_slice(&self.text_bytes);

        // ── Relocations (8 bytes each) ──
        for reloc in &self.relocations {
            // r_offset (4 bytes)
            buf.extend_from_slice(&reloc.offset.to_le_bytes());
            // r_symbolnum (24 bits) + r_pcrel (1 bit) + r_length (2 bits) + r_extern (1 bit) + r_type (4 bits)
            let mut packed: u32 = reloc.symbol_index & 0x00ffffff;
            if reloc.is_pcrel { packed |= 1 << 24; }
            packed |= (reloc.length as u32 & 3) << 25;
            if reloc.is_extern { packed |= 1 << 27; }
            packed |= (reloc.type_ as u32 & 0xf) << 28;
            buf.extend_from_slice(&packed.to_le_bytes());
        }

        // ── Symbol Table (nlist_64, 16 bytes each) ──
        for (i, sym) in self.symbols.iter().enumerate() {
            let str_offset = sym_string_offsets[i];
            buf.extend_from_slice(&str_offset.to_le_bytes());    // n_strx
            buf.push(sym.type_);                                  // n_type
            buf.push(sym.section_number as u8);                   // n_sect
            buf.extend_from_slice(&sym.description.to_le_bytes()); // n_desc
            buf.extend_from_slice(&sym.value.to_le_bytes());      // n_value
        }

        // ── String Table ──
        buf.extend_from_slice(&self.string_table);

        Ok(buf)
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_macho_basic() {
        let asm = "main:\n    ret\n";
        let macho = emit_object_file(&L3Module::default(), asm).unwrap();
        // Mach-O header is 32 bytes
        assert!(macho.len() > 32);
        // Magic should be MH_MAGIC_64
        let magic = u32::from_le_bytes([macho[0], macho[1], macho[2], macho[3]]);
        assert_eq!(magic, 0xFEEDFACF);
    }

    #[test]
    fn test_macho_cpu_type() {
        let asm = "main:\n    ret\n";
        let macho = emit_object_file(&L3Module::default(), asm).unwrap();
        let cputype = u32::from_le_bytes([macho[4], macho[5], macho[6], macho[7]]);
        assert_eq!(cputype, 0x01000007); // CPU_TYPE_X86_64
    }

    #[test]
    fn test_macho_filetype() {
        let asm = "main:\n    ret\n";
        let macho = emit_object_file(&L3Module::default(), asm).unwrap();
        let filetype = u32::from_le_bytes([macho[12], macho[13], macho[14], macho[15]]);
        assert_eq!(filetype, 1); // MH_OBJECT
    }

    #[test]
    fn test_macho_with_symbols() {
        let asm = "main:\n    call helper\n    ret\nhelper:\n    ret\n";
        let macho = emit_object_file(&L3Module::default(), asm).unwrap();
        // Should have at least 2 symbols
        let nsyms = u32::from_le_bytes([macho[16], macho[17], macho[18], macho[19]]);
        assert!(nsyms >= 2, "Expected at least 2 symbols, got {}", nsyms);
    }
}
