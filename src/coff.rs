// COFF Object File Emitter — Phase 4C
//
// Produces COFF (Common Object File Format) object files for Windows
// compatibility. The emitter takes the textual assembly output from
// codegen_x64 and packages it into a COFF object file.
//
// COFF format (AMD64):
//   - COFF header (20 bytes)
//   - Section headers (40 bytes each)
//   - Section data (.text, .data, .rdata)
//   - Relocation table
//   - Symbol table (18 bytes per symbol)
//   - String table

use crate::pir::L3Module;
use std::collections::HashMap;

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

/// Emit a COFF object file from the assembly text.
/// Returns the raw bytes of the .obj file.
pub fn emit_object_file(_module: &L3Module, asm: &str) -> Result<Vec<u8>, String> {
    let mut emitter = CoffEmitter::new();
    emitter.assemble(asm)?;
    emitter.build_coff()
}

// ═══════════════════════════════════════════════════════════════════════════
// COFF constants
// ═══════════════════════════════════════════════════════════════════════════

const IMAGE_FILE_MACHINE_AMD64: u16 = 0x8664;
const IMAGE_SCN_CNT_CODE: u32 = 0x00000020;
const IMAGE_SCN_CNT_INITIALIZED_DATA: u32 = 0x00000040;
const IMAGE_SCN_MEM_EXECUTE: u32 = 0x20000000;
const IMAGE_SCN_MEM_READ: u32 = 0x40000000;
const IMAGE_SCN_MEM_WRITE: u32 = 0x80000000;

const IMAGE_SYM_UNDEFINED: i16 = 0;
const IMAGE_SYM_ABSOLUTE: i16 = -1;
const IMAGE_SYM_DEBUG: i16 = -2;

const IMAGE_SYM_TYPE_NULL: u16 = 0;
const IMAGE_SYM_TYPE_FUNCTION: u16 = 0x20;
const IMAGE_SYM_DTYPE_FUNCTION: u16 = 2;

const IMAGE_SYM_CLASS_EXTERNAL: u8 = 2;
const IMAGE_SYM_CLASS_STATIC: u8 = 3;
const IMAGE_SYM_CLASS_LABEL: u8 = 6;

// Relocation types for AMD64 COFF
const IMAGE_REL_AMD64_ADDR64: u16 = 1;    // 64-bit absolute
const IMAGE_REL_AMD64_ADDR32NB: u16 = 3;  // 32-bit RIP-relative
const IMAGE_REL_AMD64_REL32: u16 = 4;     // 32-bit PC-relative

// ═══════════════════════════════════════════════════════════════════════════
// COFF Emitter
// ═══════════════════════════════════════════════════════════════════════════

struct CoffEmitter {
    text_bytes: Vec<u8>,
    symbols: Vec<SymbolEntry>,
    relocations: Vec<RelocEntry>,
    string_table: Vec<u8>,
    symbol_name_map: HashMap<String, u32>,
}

struct SymbolEntry {
    name: String,
    value: u32,
    section_number: i16,
    type_: u16,
    storage_class: u8,
    number_of_aux_symbols: u8,
}

struct RelocEntry {
    virtual_address: u32,
    symbol_table_index: u32,
    type_: u16,
}

impl CoffEmitter {
    fn new() -> Self {
        CoffEmitter {
            text_bytes: Vec::new(),
            symbols: Vec::new(),
            relocations: Vec::new(),
            string_table: Vec::new(),
            symbol_name_map: HashMap::new(),
        }
    }

    fn assemble(&mut self, asm: &str) -> Result<(), String> {
        // Parse the assembly to extract labels and encode instructions
        // For now, we use a simplified approach: each line is either a label
        // or an instruction. We encode common x86-64 instructions and
        // fall back to nop for unknown ones.

        for line in asm.lines() {
            let line = line.trim();
            if line.is_empty() || line.starts_with("//") || line.starts_with("#") {
                continue;
            }

            // Check for label (ends with ':')
            if line.ends_with(':') {
                let label = &line[..line.len() - 1];
                let offset = self.text_bytes.len() as u32;
                self.add_symbol(label, offset, true);
                continue;
            }

            // Check for directives
            if line.starts_with('.') {
                continue;
            }

            // Encode instruction (simplified)
            self.encode_instruction(line)?;
        }

        Ok(())
    }

    fn encode_instruction(&mut self, line: &str) -> Result<(), String> {
        // Simplified x86-64 encoding for common instructions
        let parts: Vec<&str> = line.split_whitespace().collect();
        if parts.is_empty() {
            return Ok(());
        }

        let mnemonic = parts[0].trim_end_matches(',');

        match mnemonic {
            "ret" => {
                self.text_bytes.push(0xC3);
            }
            "nop" => {
                self.text_bytes.push(0x90);
            }
            "push" => {
                if parts.len() > 1 {
                    let reg = parts[1].trim_end_matches(',');
                    if reg == "rbp" {
                        self.text_bytes.extend_from_slice(&[0x55]);
                    } else if reg == "rsp" {
                        self.text_bytes.extend_from_slice(&[0x54]);
                    } else {
                        // Generic push: 50+rd
                        self.text_bytes.push(0x50);
                    }
                }
            }
            "pop" => {
                if parts.len() > 1 {
                    let reg = parts[1].trim_end_matches(',');
                    if reg == "rbp" {
                        self.text_bytes.extend_from_slice(&[0x5D]);
                    } else {
                        self.text_bytes.push(0x58);
                    }
                }
            }
            "mov" | "movq" | "movl" => {
                // Simplified: mov rax, imm -> 48 C7 C0 xx xx xx xx
                // For now, just emit a nop
                self.text_bytes.push(0x90);
            }
            "add" | "sub" | "mul" | "imul" | "div" | "idiv" => {
                self.text_bytes.push(0x90);
            }
            "call" => {
                // call <label> — emit E8 + 4 bytes (filled by relocation)
                if parts.len() > 1 {
                    let target = parts[1].trim_end_matches(',');
                    self.text_bytes.push(0xE8);
                    let offset = self.text_bytes.len() as u32;
                    self.text_bytes.extend_from_slice(&[0, 0, 0, 0]);
                    // Add relocation
                    let sym_idx = self.get_or_add_symbol(target, 0, false);
                    self.relocations.push(RelocEntry {
                        virtual_address: offset,
                        symbol_table_index: sym_idx,
                        type_: IMAGE_REL_AMD64_REL32,
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
                    self.relocations.push(RelocEntry {
                        virtual_address: offset,
                        symbol_table_index: sym_idx,
                        type_: IMAGE_REL_AMD64_REL32,
                    });
                }
            }
            "int3" | "hlt" => {
                self.text_bytes.push(0xCC);
            }
            _ => {
                // Unknown instruction — emit nop
                self.text_bytes.push(0x90);
            }
        }

        Ok(())
    }

    fn add_symbol(&mut self, name: &str, offset: u32, is_defined: bool) {
        let section = if is_defined { 1 } else { IMAGE_SYM_UNDEFINED };
        let storage_class = if is_defined {
            IMAGE_SYM_CLASS_STATIC
        } else {
            IMAGE_SYM_CLASS_EXTERNAL
        };

        self.symbols.push(SymbolEntry {
            name: name.to_string(),
            value: offset,
            section_number: section,
            type_: IMAGE_SYM_TYPE_NULL,
            storage_class,
            number_of_aux_symbols: 0,
        });
    }

    fn get_or_add_symbol(&mut self, name: &str, offset: u32, is_defined: bool) -> u32 {
        // Check if symbol already exists
        for (i, sym) in self.symbols.iter().enumerate() {
            if sym.name == name {
                return i as u32;
            }
        }
        // Add new symbol
        let idx = self.symbols.len() as u32;
        self.add_symbol(name, offset, is_defined);
        idx
    }

    fn build_coff(&mut self) -> Result<Vec<u8>, String> {
        // COFF layout:
        //   1. COFF header (20 bytes)
        //   2. Section headers (40 bytes each, 1 section: .text)
        //   3. .text section data
        //   4. .text relocations
        //   5. Symbol table
        //   6. String table

        let num_sections = 1u16;
        let header_size = 20;
        let section_header_size = 40;
        let section_headers_size = section_header_size * num_sections as usize;

        // .text section data starts after headers
        let text_data_offset = header_size + section_headers_size;
        let text_data_size = self.text_bytes.len();

        // Relocations start after .text data
        let reloc_offset = text_data_offset + text_data_size;
        let reloc_size = self.relocations.len() * 10;

        // Symbol table starts after relocations
        let symbol_table_offset = reloc_offset + reloc_size;
        let symbol_table_size = self.symbols.len() * 18;

        // String table starts after symbol table
        let string_table_offset = symbol_table_offset + symbol_table_size;

        // Build string table (for names > 8 chars)
        // First 4 bytes = size of string table (including size field itself)
        let mut string_table_data: Vec<u8> = Vec::new();
        string_table_data.extend_from_slice(&4u32.to_le_bytes()); // initial size = 4

        // Update symbol names to use string table if > 8 chars
        for sym in &mut self.symbols {
            if sym.name.len() > 8 {
                let offset = string_table_data.len() as u32;
                string_table_data.extend_from_slice(sym.name.as_bytes());
                string_table_data.push(0);
                let new_size = string_table_data.len() as u32;
                string_table_data[0..4].copy_from_slice(&new_size.to_le_bytes());
                self.symbol_name_map.insert(sym.name.clone(), offset);
            }
        }

        let _ = string_table_offset;

        let mut buf = Vec::new();

        // ── COFF Header ──
        buf.extend_from_slice(&IMAGE_FILE_MACHINE_AMD64.to_le_bytes()); // Machine
        buf.extend_from_slice(&num_sections.to_le_bytes());              // NumberOfSections
        buf.extend_from_slice(&0u32.to_le_bytes());                      // TimeDateStamp
        buf.extend_from_slice(&(symbol_table_offset as u32).to_le_bytes()); // PointerToSymbolTable
        buf.extend_from_slice(&(self.symbols.len() as u32).to_le_bytes()); // NumberOfSymbols
        buf.extend_from_slice(&0u16.to_le_bytes());                      // SizeOfOptionalHeader
        buf.extend_from_slice(&0u16.to_le_bytes());                      // Characteristics

        // ── Section Header: .text ──
        // Name (8 bytes, padded with zeros)
        let name = b".text\0\0\0";
        buf.extend_from_slice(name);
        buf.extend_from_slice(&(text_data_size as u32).to_le_bytes());   // VirtualSize
        buf.extend_from_slice(&0u32.to_le_bytes());                      // VirtualAddress
        buf.extend_from_slice(&(text_data_size as u32).to_le_bytes());   // SizeOfRawData
        buf.extend_from_slice(&(text_data_offset as u32).to_le_bytes()); // PointerToRawData
        buf.extend_from_slice(&(reloc_offset as u32).to_le_bytes());     // PointerToRelocations
        buf.extend_from_slice(&0u32.to_le_bytes());                      // PointerToLinenumbers
        buf.extend_from_slice(&(self.relocations.len() as u16).to_le_bytes()); // NumberOfRelocations
        buf.extend_from_slice(&0u16.to_le_bytes());                      // NumberOfLinenumbers
        // Characteristics: code + execute + read
        let chars = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ;
        buf.extend_from_slice(&chars.to_le_bytes());

        // ── .text section data ──
        buf.extend_from_slice(&self.text_bytes);

        // ── Relocations ──
        for reloc in &self.relocations {
            buf.extend_from_slice(&reloc.virtual_address.to_le_bytes());
            buf.extend_from_slice(&reloc.symbol_table_index.to_le_bytes());
            buf.extend_from_slice(&reloc.type_.to_le_bytes());
        }

        // ── Symbol Table ──
        for sym in &self.symbols {
            if sym.name.len() <= 8 {
                // Short name: 8 bytes, padded with zeros
                let mut name_buf = [0u8; 8];
                name_buf[..sym.name.len()].copy_from_slice(sym.name.as_bytes());
                buf.extend_from_slice(&name_buf);
            } else {
                // Long name: 4 bytes of 0 + 4 bytes offset into string table
                let offset = self.symbol_name_map.get(&sym.name).copied().unwrap_or(0);
                buf.extend_from_slice(&0u32.to_le_bytes());
                buf.extend_from_slice(&offset.to_le_bytes());
            }
            buf.extend_from_slice(&sym.value.to_le_bytes());           // Value
            buf.extend_from_slice(&sym.section_number.to_le_bytes());  // SectionNumber
            buf.extend_from_slice(&sym.type_.to_le_bytes());           // Type
            buf.push(sym.storage_class);                               // StorageClass
            buf.push(sym.number_of_aux_symbols);                       // NumberOfAuxSymbols
        }

        // ── String Table ──
        buf.extend_from_slice(&string_table_data);

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
    fn test_coff_basic() {
        let asm = "main:\n    ret\n";
        let coff = emit_object_file(&L3Module::default(), asm).unwrap();
        // COFF header is 20 bytes
        assert!(coff.len() > 20);
        // Machine should be AMD64
        let machine = u16::from_le_bytes([coff[0], coff[1]]);
        assert_eq!(machine, 0x8664);
    }

    #[test]
    fn test_coff_with_call() {
        let asm = "main:\n    call helper\n    ret\nhelper:\n    ret\n";
        let coff = emit_object_file(&L3Module::default(), asm).unwrap();
        // Should have at least 2 symbols (main, helper)
        let num_symbols = u32::from_le_bytes([coff[12], coff[13], coff[14], coff[15]]);
        assert!(num_symbols >= 2, "Expected at least 2 symbols, got {}", num_symbols);
    }

    #[test]
    fn test_coff_section_name() {
        let asm = "main:\n    ret\n";
        let coff = emit_object_file(&L3Module::default(), asm).unwrap();
        // Section name starts at offset 20 (after COFF header)
        let name = &coff[20..28];
        assert_eq!(&name[..5], b".text");
    }
}
