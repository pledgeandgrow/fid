// DWARF Debug Info Generation — Phase 5 (Platform)
//
// Generates DWARF v4 debug information for ELF and PE object files.
// This enables gdb/lldb debugging of Fides-compiled binaries with
// source-level line number information.
//
// DWARF sections generated:
//   .debug_abbrev — abbreviation table (DIE templates)
//   .debug_info   — compilation unit + subprogram DIEs
//   .debug_line   — line number program (address → source line mapping)
//   .debug_str    — string table (file names, function names)
//
// The DWARF format is a binary encoding used by all major debuggers on
// Linux/ELF. For PE/COFF (Windows), the same DWARF sections can be
// embedded (some Windows tools support DWARF; CodeView is the native
// format but DWARF is sufficient for cross-platform debugging).

use std::collections::HashMap;

// ═══════════════════════════════════════════════════════════════════════════
// DWARF Constants
// ═══════════════════════════════════════════════════════════════════════════

// DWARF tags
const DW_TAG_COMPILE_UNIT: u64 = 0x11;
const DW_TAG_SUBPROGRAM: u64 = 0x2e;

// DWARF attributes
const DW_AT_NAME: u64 = 0x03;
const DW_AT_COMP_DIR: u64 = 0x1b;
const DW_AT_LOW_PC: u64 = 0x11;
const DW_AT_HIGH_PC: u64 = 0x12;
const DW_AT_STMT_LIST: u64 = 0x10;
const DW_AT_EXTERNAL: u64 = 0x3f;

// DWARF attribute forms
const DW_FORM_STRING: u64 = 0x08;    // inline null-terminated string
const DW_FORM_ADDR: u64 = 0x01;      // machine address
const DW_FORM_DATA4: u64 = 0x06;     // 4-byte constant
const DW_FORM_FLAG: u64 = 0x19;      // 1-byte flag

// DWARF line number program opcodes
const DW_LNS_COPY: u8 = 1;
const DW_LNS_ADVANCE_PC: u8 = 2;
const DW_LNS_ADVANCE_LINE: u8 = 3;
const DW_LNS_SET_FILE: u8 = 4;
const DW_LNS_SET_COLUMN: u8 = 5;
const DW_LNS_NEGATE_STMT: u8 = 6;
const DW_LNS_SET_BASIC_BLOCK: u8 = 7;
const DW_LNS_CONST_ADD_PC: u8 = 8;
const DW_LNS_FIXED_ADVANCE_PC: u8 = 9;
const DW_LNE_END_SEQUENCE: u8 = 0;

// Special opcode base: opcode 0 is extended, 1-12 are standard, 13+ are special
const SPECIAL_OPCODE_BASE: u8 = 13;
const LINE_BASE: i32 = -5;
const LINE_RANGE: u32 = 14;

// ═══════════════════════════════════════════════════════════════════════════
// DwarfBuilder
// ═══════════════════════════════════════════════════════════════════════════

/// Collects debug info during codegen and produces DWARF section bytes.
pub struct DwarfBuilder {
    /// Source file path for the compilation unit
    source_file: String,
    /// Compilation directory
    comp_dir: String,
    /// Per-function debug info: (name, start_offset, end_offset, start_line)
    functions: Vec<FunctionDebugInfo>,
    /// Line number entries: (text_offset, source_line)
    line_entries: Vec<LineEntry>,
}

#[derive(Debug, Clone)]
struct FunctionDebugInfo {
    name: String,
    low_pc: u64,
    high_pc: u64,
    is_external: bool,
}

#[derive(Debug, Clone)]
struct LineEntry {
    address: u64,
    line: u32,
    column: u32,
}

impl DwarfBuilder {
    pub fn new(source_file: &str, comp_dir: &str) -> Self {
        DwarfBuilder {
            source_file: source_file.to_string(),
            comp_dir: comp_dir.to_string(),
            functions: Vec::new(),
            line_entries: Vec::new(),
        }
    }

    /// Register a function's address range for DWARF subprogram DIEs.
    pub fn add_function(&mut self, name: &str, low_pc: u64, high_pc: u64, is_external: bool) {
        self.functions.push(FunctionDebugInfo {
            name: name.to_string(),
            low_pc,
            high_pc,
            is_external,
        });
    }

    /// Add a line number mapping: (code offset in .text, source line number).
    pub fn add_line_entry(&mut self, address: u64, line: u32, column: u32) {
        self.line_entries.push(LineEntry { address, line, column });
    }

    /// Generate all DWARF sections as a map of section name → bytes.
    /// Returns (debug_abbrev, debug_info, debug_line, debug_str).
    pub fn build(&self) -> DwarfSections {
        let debug_str = self.build_debug_str();
        let debug_abbrev = self.build_debug_abbrev();
        let debug_info = self.build_debug_info(&debug_str, &debug_abbrev);
        let debug_line = self.build_debug_line();
        DwarfSections {
            debug_abbrev,
            debug_info,
            debug_line,
            debug_str,
        }
    }

    // ─── .debug_str ──────────────────────────────────────────────────────
    fn build_debug_str(&self) -> Vec<u8> {
        let mut buf = Vec::new();
        // .debug_str is a sequence of null-terminated strings.
        // We deduplicate by collecting unique strings in order.
        // String offsets are tracked by the caller.
        buf.extend_from_slice(self.source_file.as_bytes());
        buf.push(0);
        buf.extend_from_slice(self.comp_dir.as_bytes());
        buf.push(0);
        for func in &self.functions {
            buf.extend_from_slice(func.name.as_bytes());
            buf.push(0);
        }
        buf
    }

    /// Find the offset of a string in the .debug_str section.
    fn str_offset(&self, s: &str) -> u32 {
        let mut offset = 0u32;
        // source_file
        if s == self.source_file {
            return 0;
        }
        offset += self.source_file.len() as u32 + 1;
        // comp_dir
        if s == self.comp_dir {
            return offset;
        }
        offset += self.comp_dir.len() as u32 + 1;
        // function names
        for func in &self.functions {
            if s == func.name {
                return offset;
            }
            offset += func.name.len() as u32 + 1;
        }
        0
    }

    // ─── .debug_abbrev ───────────────────────────────────────────────────
    fn build_debug_abbrev(&self) -> Vec<u8> {
        let mut buf = Vec::new();
        // Abbreviation 1: DW_TAG_compile_unit
        encode_uleb128(&mut buf, 1); // abbrev code 1
        encode_uleb128(&mut buf, DW_TAG_COMPILE_UNIT);
        buf.push(0); // DW_CHILDREN_yes
        // Attributes:
        encode_uleb128(&mut buf, DW_AT_NAME);
        encode_uleb128(&mut buf, DW_FORM_STRING);
        encode_uleb128(&mut buf, DW_AT_COMP_DIR);
        encode_uleb128(&mut buf, DW_FORM_STRING);
        encode_uleb128(&mut buf, DW_AT_LOW_PC);
        encode_uleb128(&mut buf, DW_FORM_ADDR);
        encode_uleb128(&mut buf, DW_AT_HIGH_PC);
        encode_uleb128(&mut buf, DW_FORM_DATA4);
        encode_uleb128(&mut buf, DW_AT_STMT_LIST);
        encode_uleb128(&mut buf, DW_FORM_DATA4);
        // Terminator
        encode_uleb128(&mut buf, 0);
        encode_uleb128(&mut buf, 0);

        // Abbreviation 2: DW_TAG_subprogram
        encode_uleb128(&mut buf, 2); // abbrev code 2
        encode_uleb128(&mut buf, DW_TAG_SUBPROGRAM);
        buf.push(0); // DW_CHILDREN_no
        // Attributes:
        encode_uleb128(&mut buf, DW_AT_NAME);
        encode_uleb128(&mut buf, DW_FORM_STRING);
        encode_uleb128(&mut buf, DW_AT_LOW_PC);
        encode_uleb128(&mut buf, DW_FORM_ADDR);
        encode_uleb128(&mut buf, DW_AT_HIGH_PC);
        encode_uleb128(&mut buf, DW_FORM_DATA4);
        encode_uleb128(&mut buf, DW_AT_EXTERNAL);
        encode_uleb128(&mut buf, DW_FORM_FLAG);
        // Terminator
        encode_uleb128(&mut buf, 0);
        encode_uleb128(&mut buf, 0);

        // End of abbreviation table
        encode_uleb128(&mut buf, 0);
        buf
    }

    // ─── .debug_info ─────────────────────────────────────────────────────
    fn build_debug_info(&self, _debug_str: &[u8], _debug_abbrev: &[u8]) -> Vec<u8> {
        let mut buf = Vec::new();

        // Unit header (DWARF v4, 32-bit format)
        let unit_length_offset = buf.len();
        buf.extend_from_slice(&[0, 0, 0, 0]); // unit_length placeholder (4 bytes)
        buf.extend_from_slice(&[0x04, 0x00]); // version = 4
        buf.extend_from_slice(&[0x00, 0x00, 0x00, 0x00]); // debug_abbrev_offset placeholder
        buf.push(8); // address_size = 8 (64-bit)

        // DIE: DW_TAG_compile_unit (abbrev code 1)
        encode_uleb128(&mut buf, 1);
        // DW_AT_NAME (inline string)
        buf.extend_from_slice(self.source_file.as_bytes());
        buf.push(0);
        // DW_AT_COMP_DIR (inline string)
        buf.extend_from_slice(self.comp_dir.as_bytes());
        buf.push(0);
        // DW_AT_LOW_PC (address — first function start, or 0)
        let low_pc = self.functions.first().map(|f| f.low_pc).unwrap_or(0);
        buf.extend_from_slice(&low_pc.to_le_bytes());
        // DW_AT_HIGH_PC (data4 — offset from low_pc to end)
        let high_pc = self.functions.last().map(|f| f.high_pc - low_pc).unwrap_or(0);
        buf.extend_from_slice(&(high_pc as u32).to_le_bytes());
        // DW_AT_STMT_LIST (data4 — offset of .debug_line, always 0 since we emit one CU)
        buf.extend_from_slice(&0u32.to_le_bytes());

        // DIEs for each function: DW_TAG_subprogram (abbrev code 2)
        for func in &self.functions {
            encode_uleb128(&mut buf, 2);
            // DW_AT_NAME (inline string)
            buf.extend_from_slice(func.name.as_bytes());
            buf.push(0);
            // DW_AT_LOW_PC (address)
            buf.extend_from_slice(&func.low_pc.to_le_bytes());
            // DW_AT_HIGH_PC (data4 — offset from low_pc)
            let size = (func.high_pc - func.low_pc) as u32;
            buf.extend_from_slice(&size.to_le_bytes());
            // DW_AT_EXTERNAL (flag)
            buf.push(if func.is_external { 1 } else { 0 });
        }

        // End of DIE list (null abbrev code)
        encode_uleb128(&mut buf, 0);

        // Patch unit_length
        let unit_length = (buf.len() - unit_length_offset - 4) as u32;
        buf[unit_length_offset..unit_length_offset + 4]
            .copy_from_slice(&unit_length.to_le_bytes());

        buf
    }

    // ─── .debug_line ─────────────────────────────────────────────────────
    fn build_debug_line(&self) -> Vec<u8> {
        let mut buf = Vec::new();

        // Line number program header (DWARF v4, 32-bit format)
        let unit_length_offset = buf.len();
        buf.extend_from_slice(&[0, 0, 0, 0]); // unit_length placeholder
        buf.extend_from_slice(&[0x04, 0x00]); // version = 4
        buf.extend_from_slice(&[0, 0, 0, 0]); // header_length placeholder

        let header_start = buf.len();

        // minimum_instruction_length
        buf.push(1);
        // maximum_operations_per_instruction (DWARF v4)
        buf.push(1);
        // default_is_stmt
        buf.push(1);
        // line_base (signed)
        buf.push(LINE_BASE as u8);
        // line_range
        buf.push(LINE_RANGE as u8);
        // opcode_base
        buf.push(SPECIAL_OPCODE_BASE);

        // standard_opcode_lengths for opcodes 1..12
        let std_opcode_lengths: [u8; 12] = [0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1];
        buf.extend_from_slice(&std_opcode_lengths);

        // Include directories (null-terminated list of null-terminated strings)
        buf.push(0); // empty list (no include directories)

        // File names (null-terminated list of entries)
        // Each entry: string + dir_index (uleb128) + mtime (uleb128) + size (uleb128)
        buf.extend_from_slice(self.source_file.as_bytes());
        buf.push(0); // null terminator for string
        encode_uleb128(&mut buf, 0); // dir_index = 0
        encode_uleb128(&mut buf, 0); // mtime = 0
        encode_uleb128(&mut buf, 0); // size = 0
        buf.push(0); // end of file list

        // Patch header_length
        let header_length = (buf.len() - header_start) as u32;
        buf[header_start - 4..header_start]
            .copy_from_slice(&header_length.to_le_bytes());

        // Line number program state machine
        let mut current_address: u64 = 0;
        let mut current_line: i32 = 1;
        let mut current_file: u32 = 1;

        for entry in &self.line_entries {
            // Set file
            if current_file != 1 {
                buf.push(DW_LNS_SET_FILE);
                encode_uleb128(&mut buf, 1);
                current_file = 1;
            }

            // Advance PC
            let addr_diff = entry.address.saturating_sub(current_address);
            if addr_diff > 0 {
                // Try special opcode first (encodes both address and line advance)
                let line_diff = entry.line as i32 - current_line;
                let adjusted_line = line_diff - LINE_BASE;
                if adjusted_line >= 0 && (adjusted_line as u32) < LINE_RANGE {
                    let opcode = SPECIAL_OPCODE_BASE
                        + (addr_diff as u32 * LINE_RANGE + adjusted_line as u32) as u8;
                    if (opcode as u32) < 256 && addr_diff < 256 {
                        buf.push(opcode);
                        current_address = entry.address;
                        current_line = entry.line as i32;
                        continue;
                    }
                }
                // Fall back to DW_LNS_ADVANCE_PC
                if addr_diff < 256 {
                    buf.push(DW_LNS_ADVANCE_PC);
                    encode_uleb128(&mut buf, addr_diff as u64);
                } else {
                    buf.push(DW_LNS_FIXED_ADVANCE_PC);
                    buf.extend_from_slice(&(addr_diff as u16).to_le_bytes());
                }
            }

            // Advance line
            let line_diff = entry.line as i32 - current_line;
            if line_diff != 0 {
                buf.push(DW_LNS_ADVANCE_LINE);
                encode_sleb128(&mut buf, line_diff as i64);
            }

            // Copy (emit a row in the line table)
            buf.push(DW_LNS_COPY);
            current_address = entry.address;
            current_line = entry.line as i32;
        }

        // End of sequence
        buf.push(0); // extended opcode marker
        encode_uleb128(&mut buf, 1); // length of extended opcode
        buf.push(DW_LNE_END_SEQUENCE);

        // Patch unit_length
        let unit_length = (buf.len() - unit_length_offset - 4) as u32;
        buf[unit_length_offset..unit_length_offset + 4]
            .copy_from_slice(&unit_length.to_le_bytes());

        buf
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// DWARF Sections Output
// ═══════════════════════════════════════════════════════════════════════════

pub struct DwarfSections {
    pub debug_abbrev: Vec<u8>,
    pub debug_info: Vec<u8>,
    pub debug_line: Vec<u8>,
    pub debug_str: Vec<u8>,
}

impl DwarfSections {
    pub fn is_empty(&self) -> bool {
        self.debug_info.is_empty()
    }

    /// Returns the section names and data for ELF section creation.
    pub fn sections(&self) -> Vec<(&'static str, &[u8])> {
        vec![
            (".debug_abbrev", &self.debug_abbrev),
            (".debug_info", &self.debug_info),
            (".debug_line", &self.debug_line),
            (".debug_str", &self.debug_str),
        ]
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// LEB128 Encoding
// ═══════════════════════════════════════════════════════════════════════════

/// Encode an unsigned integer as ULEB128.
fn encode_uleb128(buf: &mut Vec<u8>, mut value: u64) {
    loop {
        let mut byte = (value & 0x7f) as u8;
        value >>= 7;
        if value != 0 {
            byte |= 0x80;
        }
        buf.push(byte);
        if value == 0 {
            break;
        }
    }
}

/// Encode a signed integer as SLEB128.
fn encode_sleb128(buf: &mut Vec<u8>, mut value: i64) {
    loop {
        let byte = (value & 0x7f) as u8;
        value >>= 7;
        let sign_bit = byte & 0x40;
        if (value == 0 && sign_bit == 0) || (value == -1 && sign_bit != 0) {
            buf.push(byte);
            break;
        }
        buf.push(byte | 0x80);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_dwarf_basic() {
        let mut builder = DwarfBuilder::new("test.fd", "/home/user");
        builder.add_function("main", 0x1000, 0x1050, true);
        builder.add_function("helper", 0x1050, 0x1080, false);
        builder.add_line_entry(0x1000, 1, 0);
        builder.add_line_entry(0x1010, 5, 0);
        builder.add_line_entry(0x1050, 10, 0);

        let sections = builder.build();
        assert!(!sections.debug_info.is_empty());
        assert!(!sections.debug_abbrev.is_empty());
        assert!(!sections.debug_line.is_empty());
        assert!(!sections.debug_str.is_empty());

        // Debug info should start with unit length (4 bytes) + version (2 bytes)
        assert_eq!(&sections.debug_info[4..6], &[0x04, 0x00]); // DWARF v4
    }

    #[test]
    fn test_uleb128() {
        let mut buf = Vec::new();
        encode_uleb128(&mut buf, 0);
        assert_eq!(buf, vec![0]);

        buf.clear();
        encode_uleb128(&mut buf, 127);
        assert_eq!(buf, vec![127]);

        buf.clear();
        encode_uleb128(&mut buf, 128);
        assert_eq!(buf, vec![0x80, 0x01]);

        buf.clear();
        encode_uleb128(&mut buf, 0x3FFF);
        assert_eq!(buf, vec![0xFF, 0x7F]);
    }

    #[test]
    fn test_sleb128() {
        let mut buf = Vec::new();
        encode_sleb128(&mut buf, 0);
        assert_eq!(buf, vec![0]);

        buf.clear();
        encode_sleb128(&mut buf, -1);
        assert_eq!(buf, vec![0x7F]);

        buf.clear();
        encode_sleb128(&mut buf, 63);
        assert_eq!(buf, vec![63]);

        buf.clear();
        encode_sleb128(&mut buf, -64);
        assert_eq!(buf, vec![0x40]);
    }

    #[test]
    fn test_dwarf_sections() {
        let builder = DwarfBuilder::new("test.fd", "/home/user");
        let sections = builder.build();
        let s = sections.sections();
        assert_eq!(s.len(), 4);
        assert_eq!(s[0].0, ".debug_abbrev");
        assert_eq!(s[1].0, ".debug_info");
        assert_eq!(s[2].0, ".debug_line");
        assert_eq!(s[3].0, ".debug_str");
    }
}
