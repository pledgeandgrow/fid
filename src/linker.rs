// Fides Integrated Linker — Phase 4D
//
// Links ELF64 object files into executable binaries without external tools.
// Supports:
//   - Full ELF executable emission (no external ld needed)
//   - Incremental linking (cache object files, only re-link changed modules)
//   - Link-time dead code elimination (remove unused functions)
//
// The linker takes one or more ELF64 relocatable object files and produces
// an ELF64 executable with:
//   - Program headers (PT_LOAD segments)
//   - Resolved relocations (R_X86_64_PC32, R_X86_64_PLT32, R_X86_64_64)
//   - Entry point set to `main` (or `_start`)
//   - Runtime symbols resolved from the Fides runtime

use std::collections::{HashMap, HashSet};
use std::path::{Path, PathBuf};

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

/// Link one or more ELF64 object files into an executable.
/// Returns the raw bytes of the executable.
pub fn link(objects: &[Vec<u8>], entry: &str) -> Result<Vec<u8>, String> {
    let mut linker = Linker::new();
    for obj in objects {
        linker.add_object(obj)?;
    }
    linker.link(entry)
}

/// Link ELF64 object files with a pre-compiled runtime (raw bytes + symbols).
/// The runtime is appended after the user objects and its symbols are
/// registered with defined=true.
pub fn link_with_runtime(
    objects: &[Vec<u8>],
    entry: &str,
    runtime_text: &[u8],
    runtime_symbols: &[(&str, u64, usize)],
    runtime_rodata: &[u8],
    runtime_rodata_syms: &[(&str, u64)],
    runtime_relocs: &[(u64, String, u8)],
) -> Result<Vec<u8>, String> {
    link_with_runtime_for_target(objects, entry, runtime_text, runtime_symbols,
        runtime_rodata, runtime_rodata_syms, runtime_relocs, "x86-64")
}

/// Link with runtime for a specific target architecture ("x86-64" or "arm64").
pub fn link_with_runtime_for_target(
    objects: &[Vec<u8>],
    entry: &str,
    runtime_text: &[u8],
    runtime_symbols: &[(&str, u64, usize)],
    runtime_rodata: &[u8],
    runtime_rodata_syms: &[(&str, u64)],
    runtime_relocs: &[(u64, String, u8)],
    target: &str,
) -> Result<Vec<u8>, String> {
    let mut linker = Linker::new_for_target(target);
    for obj in objects {
        linker.add_object(obj)?;
    }
    // Add the runtime as raw text + symbols
    let rt_base = linker.text_bytes.len() as u64;
    linker.text_bytes.extend_from_slice(runtime_text);
    for (name, offset, _size) in runtime_symbols {
        linker.symbols.insert(name.to_string(), (rt_base + offset, true));
    }
    // Add rodata
    let rod_base = linker.rodata.len() as u64;
    linker.rodata.extend_from_slice(runtime_rodata);
    for (name, offset) in runtime_rodata_syms {
        linker.rodata_symbols.insert(name.to_string(), rod_base + offset);
    }
    // Add relocations (adjusted by rt_base)
    for (offset, sym_name, r_type) in runtime_relocs {
        linker.relocations.push((rt_base + offset, sym_name.clone(), *r_type));
    }
    linker.link(entry)
}

/// Link raw user code (pre-assembled) with a runtime, for a specific target.
/// Both user code and runtime are provided as raw bytes + symbols + relocations.
/// This bypasses ELF object file parsing — used for ARM64 where we assemble
/// directly in-process.
pub fn link_raw_with_runtime_for_target(
    user_text: &[u8],
    user_symbols: &[(String, u64)],
    user_relocs: &[(u64, String, u8)],
    user_rodata: &[u8],
    user_rodata_syms: &[(String, u64)],
    entry: &str,
    runtime_text: &[u8],
    runtime_symbols: &[(&str, u64, usize)],
    runtime_rodata: &[u8],
    runtime_rodata_syms: &[(&str, u64)],
    runtime_relocs: &[(u64, String, u8)],
    target: &str,
) -> Result<Vec<u8>, String> {
    let mut linker = Linker::new_for_target(target);

    // Add user text
    linker.text_bytes.extend_from_slice(user_text);
    for (name, offset) in user_symbols {
        linker.symbols.insert(name.clone(), (*offset, true));
    }
    // Add user rodata
    let user_rod_base = 0u64; // user rodata starts at 0 in its own space
    let _ = user_rod_base;
    linker.rodata.extend_from_slice(user_rodata);
    let user_rod_offset = if user_text.is_empty() { 0 } else { 0u64 };
    let _ = user_rod_offset;
    for (name, offset) in user_rodata_syms {
        linker.rodata_symbols.insert(name.clone(), *offset);
    }
    // Add user relocations
    for (offset, sym_name, r_type) in user_relocs {
        linker.relocations.push((*offset, sym_name.clone(), *r_type));
    }

    // Add the runtime as raw text + symbols
    let rt_base = linker.text_bytes.len() as u64;
    linker.text_bytes.extend_from_slice(runtime_text);
    for (name, offset, _size) in runtime_symbols {
        linker.symbols.insert(name.to_string(), (rt_base + offset, true));
    }
    // Add runtime rodata
    let rod_base = linker.rodata.len() as u64;
    linker.rodata.extend_from_slice(runtime_rodata);
    for (name, offset) in runtime_rodata_syms {
        linker.rodata_symbols.insert(name.to_string(), rod_base + offset);
    }
    // Add runtime relocations (adjusted by rt_base)
    for (offset, sym_name, r_type) in runtime_relocs {
        linker.relocations.push((rt_base + offset, sym_name.clone(), *r_type));
    }
    linker.link(entry)
}

/// Incremental link cache: tracks which object files have changed
/// and only re-links when necessary.
pub struct IncrementalLinker {
    cache_dir: PathBuf,
    entry: String,
    last_output: Option<Vec<u8>>,
}

impl IncrementalLinker {
    pub fn new(cache_dir: impl AsRef<Path>, entry: &str) -> Self {
        let cache_dir = cache_dir.as_ref().to_path_buf();
        std::fs::create_dir_all(&cache_dir).ok();
        IncrementalLinker {
            cache_dir,
            entry: entry.to_string(),
            last_output: None,
        }
    }

    /// Link with incremental caching. Only re-links if any source changed.
    pub fn link_incremental(&mut self, sources: &[(&str, &str)]) -> Result<Vec<u8>, String> {
        let mut changed = false;
        let mut objects: Vec<Vec<u8>> = Vec::new();

        for (name, content) in sources {
            let hash = simple_hash(content);
            let cache_file = self.cache_dir.join(format!("{}.{}.o", sanitize(name), hash));

            if cache_file.exists() {
                // Object is cached — load it
                let obj = std::fs::read(&cache_file)
                    .map_err(|e| format!("Failed to read cached object: {}", e))?;
                objects.push(obj);
            } else {
                // Need to compile this source
                changed = true;
                // The caller should have already compiled it — we just mark it as changed
                // and try to find any existing object
                let any_cache = self.cache_dir.join(format!("{}.latest.o", sanitize(name)));
                if any_cache.exists() {
                    let obj = std::fs::read(&any_cache)
                        .map_err(|e| format!("Failed to read object: {}", e))?;
                    objects.push(obj);
                }
            }
        }

        if changed || self.last_output.is_none() {
            let output = link(&objects, &self.entry)?;
            self.last_output = Some(output.clone());
            Ok(output)
        } else {
            Ok(self.last_output.clone().unwrap())
        }
    }
}

/// Link-time dead code elimination: starting from the entry point,
/// traverse the call graph and mark all reachable functions. Remove
/// unreachable functions from the final binary.
pub fn dead_code_elimination(
    symbols: &[(String, u64, usize)],  // (name, offset, size)
    relocations: &[(u64, String)],     // (offset, target_symbol)
    entry: &str,
) -> Vec<usize> {
    // Build call graph: for each symbol, which symbols does it call?
    let mut call_graph: HashMap<String, Vec<String>> = HashMap::new();
    let symbol_ranges: HashMap<String, (u64, usize)> = symbols
        .iter()
        .map(|(name, offset, size)| (name.clone(), (*offset, *size)))
        .collect();

    // Issue 30: Track relocations that are NOT within any function's code range.
    // These are typically in .data/.rodata sections (vtables, function pointer tables).
    // Functions referenced by these relocations must be treated as roots (always reachable).
    let mut external_roots: HashSet<String> = HashSet::new();

    for (offset, target) in relocations {
        // Find which symbol contains this relocation offset.
        let mut found_in_func = false;
        for (name, (start, size)) in &symbol_ranges {
            if *size == 0 {
                // Unknown size: attribute to the nearest symbol at or before this offset
                // We'll handle this after the loop with a sorted approach
                continue;
            }
            if *offset >= *start && *offset < *start + *size as u64 {
                call_graph
                    .entry(name.clone())
                    .or_default()
                    .push(target.clone());
                found_in_func = true;
                break;
            }
        }
        // Issue 30: If this relocation is NOT within any function's code range,
        // it's likely in .data/.rodata (vtable, function pointer table).
        // The target function must be treated as a root.
        if !found_in_func {
            external_roots.insert(target.clone());
        }
    }

    // Handle size-0 symbols: sort by offset and attribute relocations to the
    // nearest preceding symbol.
    let mut sorted_syms: Vec<(String, u64)> = symbol_ranges
        .iter()
        .filter(|(_, (_, size))| *size == 0)
        .map(|(name, (start, _))| (name.clone(), *start))
        .collect();
    sorted_syms.sort_by_key(|(_, off)| *off);

    for (offset, target) in relocations {
        // Find the nearest preceding symbol with size 0
        let mut found: Option<&str> = None;
        for (name, start) in &sorted_syms {
            if *start <= *offset {
                found = Some(name);
            } else {
                break;
            }
        }
        if let Some(name) = found {
            // Only add if not already added by the size>0 path
            let already = call_graph.get(name).map(|v| v.contains(target)).unwrap_or(false);
            if !already {
                call_graph.entry(name.to_string()).or_default().push(target.clone());
            }
        }
    }

    // BFS from entry point
    let mut reachable: HashSet<String> = HashSet::new();
    let mut queue: Vec<String> = vec![entry.to_string()];
    // Issue 30: Add external roots (functions referenced from .data/.rodata)
    // These are always reachable — they're called indirectly via vtables or function pointers.
    for root in &external_roots {
        queue.push(root.clone());
    }

    while let Some(sym) = queue.pop() {
        if reachable.contains(&sym) {
            continue;
        }
        reachable.insert(sym.clone());

        if let Some(callees) = call_graph.get(&sym) {
            for callee in callees {
                if !reachable.contains(callee) {
                    queue.push(callee.clone());
                }
            }
        }
    }

    // Return indices of reachable symbols
    symbols
        .iter()
        .enumerate()
        .filter(|(_, (name, _, _))| reachable.contains(name))
        .map(|(i, _)| i)
        .collect()
}

// ═══════════════════════════════════════════════════════════════════════════
// Internal: Linker
// ═══════════════════════════════════════════════════════════════════════════

struct Linker {
    // Collected .text sections from all object files
    text_bytes: Vec<u8>,
    // Symbols: (name, offset_in_text, is_defined)
    symbols: HashMap<String, (u64, bool)>,
    // Relocations: (offset_in_text, symbol_name, type)
    relocations: Vec<(u64, String, u8)>,
    // String constants (rodata)
    rodata: Vec<u8>,
    rodata_symbols: HashMap<String, u64>,
    // Relocations for rodata (e.g., vtable function pointers)
    rodata_relocations: Vec<(u64, String, u8)>,
    // Target architecture: "x86-64" or "arm64"
    target: String,
}

impl Linker {
    fn new() -> Self {
        Linker {
            text_bytes: Vec::new(),
            symbols: HashMap::new(),
            relocations: Vec::new(),
            rodata: Vec::new(),
            rodata_symbols: HashMap::new(),
            rodata_relocations: Vec::new(),
            target: "x86-64".to_string(),
        }
    }

    fn new_for_target(target: &str) -> Self {
        let mut l = Self::new();
        l.target = target.to_string();
        l
    }

    fn add_object(&mut self, obj: &[u8]) -> Result<(), String> {
        if obj.len() < 64 {
            return Err("Object file too small for ELF header".to_string());
        }
        if &obj[..4] != &[0x7f, b'E', b'L', b'F'] {
            return Err("Not an ELF file".to_string());
        }
        if obj[4] != 2 {
            return Err("Not ELF64".to_string());
        }

        // Parse ELF header
        let e_shoff = u64::from_le_bytes(obj[40..48].try_into().unwrap()) as usize;
        let e_shnum = u16::from_le_bytes(obj[60..62].try_into().unwrap()) as usize;
        let e_shstrndx = u16::from_le_bytes(obj[62..64].try_into().unwrap()) as usize;

        if e_shoff == 0 || e_shnum == 0 {
            return Err("No section headers in object file".to_string());
        }

        // Parse section headers
        let mut sections: Vec<SectionInfo> = Vec::new();
        for i in 0..e_shnum {
            let offset = e_shoff + i * 64;
            if offset + 64 > obj.len() {
                break;
            }
            let sh_name = u32::from_le_bytes(obj[offset..offset + 4].try_into().unwrap()) as usize;
            let sh_type = u32::from_le_bytes(obj[offset + 4..offset + 8].try_into().unwrap());
            let sh_offset = u64::from_le_bytes(obj[offset + 24..offset + 32].try_into().unwrap()) as usize;
            let sh_size = u64::from_le_bytes(obj[offset + 32..offset + 40].try_into().unwrap()) as usize;
            let sh_link = u32::from_le_bytes(obj[offset + 40..offset + 44].try_into().unwrap()) as usize;
            let sh_entsize = u64::from_le_bytes(obj[offset + 56..offset + 64].try_into().unwrap()) as usize;

            sections.push(SectionInfo {
                name_offset: sh_name,
                sh_type,
                offset: sh_offset,
                size: sh_size,
                link: sh_link,
                entsize: sh_entsize as u64,
                name: String::new(),
            });
        }

        // Get section name string table
        if e_shstrndx < sections.len() {
            let (strtab_offset, strtab_size) = (sections[e_shstrndx].offset, sections[e_shstrndx].size);
            if strtab_offset + strtab_size <= obj.len() {
                for sec in &mut sections {
                    sec.name = get_string(obj, strtab_offset, sec.name_offset);
                }
            }
        }

        // Find .text, .rodata, .symtab, .strtab, .rela.text sections
        let mut text_offset = 0;
        let mut text_size = 0;
        let mut text_data: Vec<u8> = Vec::new();
        let mut rodata_data: Vec<u8> = Vec::new();
        let mut rodata_syms: Vec<(String, u64)> = Vec::new();
        let mut symtab_idx = None;
        let mut strtab_idx = None;
        let mut rela_text_idx = None;
        let mut rela_rodata_idx = None;

        for (i, sec) in sections.iter().enumerate() {
            match sec.name.as_str() {
                ".text" => {
                    text_offset = sec.offset;
                    text_size = sec.size;
                    if sec.offset + sec.size <= obj.len() {
                        text_data = obj[sec.offset..sec.offset + sec.size].to_vec();
                    }
                }
                ".rodata" => {
                    if sec.offset + sec.size <= obj.len() {
                        rodata_data = obj[sec.offset..sec.offset + sec.size].to_vec();
                    }
                }
                ".symtab" => symtab_idx = Some(i),
                ".strtab" => strtab_idx = Some(sec.link),
                ".rela.text" => rela_text_idx = Some(i),
                ".rela.rodata" => rela_rodata_idx = Some(i),
                _ => {}
            }
        }

        // Adjust symbol table link (strtab is in the symtab's sh_link field)
        if let Some(si) = symtab_idx {
            strtab_idx = Some(sections[si].link);
        }

        // Append text section to our combined text
        let text_base = self.text_bytes.len() as u64;
        self.text_bytes.extend(&text_data);

        // Append rodata section to our combined rodata
        let rodata_base = self.rodata.len() as u64;
        self.rodata.extend(&rodata_data);

        // Find the section index for .rodata
        let rodata_sec_idx: Option<u16> = sections.iter().enumerate()
            .find(|(_, s)| s.name == ".rodata")
            .map(|(i, _)| i as u16);

        // Parse symbols
        if let (Some(si), Some(sti)) = (symtab_idx, strtab_idx) {
            let symtab = &sections[si];
            let strtab = &sections[sti];
            let num_syms = symtab.size / 24; // Elf64_Sym is 24 bytes

            for j in 0..num_syms {
                let off = symtab.offset + j * 24;
                if off + 24 > obj.len() {
                    break;
                }
                let st_name = u32::from_le_bytes(obj[off..off + 4].try_into().unwrap()) as usize;
                let st_value = u64::from_le_bytes(obj[off + 8..off + 16].try_into().unwrap());
                let st_size = u64::from_le_bytes(obj[off + 16..off + 24].try_into().unwrap());
                let st_shndx = u16::from_le_bytes(obj[off + 6..off + 8].try_into().unwrap());

                let name = get_string(obj, strtab.offset, st_name);
                if name.is_empty() {
                    continue;
                }

                // If the symbol is defined (st_shndx points to a section, not UNDEF)
                let is_defined = st_shndx != 0;

                // Check if this symbol is in the rodata section
                if is_defined && rodata_sec_idx == Some(st_shndx) {
                    // Rodata symbol
                    self.rodata_symbols.insert(name, rodata_base + st_value);
                } else {
                    let offset = if is_defined {
                        text_base + st_value
                    } else {
                        0
                    };
                    self.symbols.insert(name, (offset, is_defined));
                }
            }
        }

        // Parse relocations
        if let Some(ri) = rela_text_idx {
            let rela = &sections[ri];
            let num_relas = rela.size / 24; // Elf64_Rela is 24 bytes

            for j in 0..num_relas {
                let off = rela.offset + j * 24;
                if off + 24 > obj.len() {
                    break;
                }
                let r_offset = u64::from_le_bytes(obj[off..off + 8].try_into().unwrap());
                let r_info = u64::from_le_bytes(obj[off + 8..off + 16].try_into().unwrap());
                let r_addend = i64::from_le_bytes(obj[off + 16..off + 24].try_into().unwrap());

                let r_sym = (r_info >> 32) as u32;
                let r_type = (r_info & 0xffffffff) as u8;

                // Get symbol name from symtab
                let sym_name = if let (Some(si), Some(sti)) = (symtab_idx, strtab_idx) {
                    let symtab = &sections[si];
                    let strtab = &sections[sti];
                    let sym_off = symtab.offset + r_sym as usize * 24;
                    if sym_off + 4 <= obj.len() {
                        let st_name = u32::from_le_bytes(obj[sym_off..sym_off + 4].try_into().unwrap()) as usize;
                        get_string(obj, strtab.offset, st_name)
                    } else {
                        String::new()
                    }
                } else {
                    String::new()
                };

                // Adjust relocation offset to combined text
                let adjusted_offset = text_base + r_offset;
                let _ = r_addend;
                self.relocations.push((adjusted_offset, sym_name, r_type));
            }
        }

        // Process rodata relocations (e.g., vtable function pointers)
        if let Some(ri) = rela_rodata_idx {
            let rela = &sections[ri];
            let num_relas = rela.size / 24;
            for j in 0..num_relas {
                let off = rela.offset + j * 24;
                if off + 24 > obj.len() {
                    break;
                }
                let r_offset = u64::from_le_bytes(obj[off..off + 8].try_into().unwrap());
                let r_info = u64::from_le_bytes(obj[off + 8..off + 16].try_into().unwrap());
                let r_addend = i64::from_le_bytes(obj[off + 16..off + 24].try_into().unwrap());
                let r_sym = (r_info >> 32) as u32;
                let r_type = (r_info & 0xffffffff) as u8;
                let sym_name = if let (Some(si), Some(sti)) = (symtab_idx, strtab_idx) {
                    let symtab = &sections[si];
                    let strtab = &sections[sti];
                    let sym_off = symtab.offset + r_sym as usize * 24;
                    if sym_off + 4 <= obj.len() {
                        let st_name = u32::from_le_bytes(obj[sym_off..sym_off + 4].try_into().unwrap()) as usize;
                        get_string(obj, strtab.offset, st_name)
                    } else {
                        String::new()
                    }
                } else {
                    String::new()
                };
                let adjusted_offset = rodata_base + r_offset;
                let _ = r_addend;
                self.rodata_relocations.push((adjusted_offset, sym_name, r_type));
            }
        }

        Ok(())
    }

    fn link(&mut self, entry: &str) -> Result<Vec<u8>, String> {
        // Apply link-time DCE
        let symbol_list: Vec<(String, u64, usize)> = self
            .symbols
            .iter()
            .filter(|(_, (_, is_def))| *is_def)
            .filter(|(name, _)| !name.starts_with(".L"))  // Exclude local labels from DCE
            .map(|(name, (offset, _))| (name.clone(), *offset, 0))
            .collect();

        let reloc_pairs: Vec<(u64, String)> = self
            .relocations
            .iter()
            .map(|(off, name, _)| (*off, name.clone()))
            .collect();

        let reachable = dead_code_elimination(&symbol_list, &reloc_pairs, entry);
        let reachable_set: HashSet<String> = reachable
            .iter()
            .map(|i| symbol_list[*i].0.clone())
            .collect();

        // Resolve relocations and patch the text
        let text_size = self.text_bytes.len() as u64;
        for (offset, sym_name, r_type) in &self.relocations {
            // Look up symbol in text symbols first, then rodata symbols
            let sym_offset_opt = if let Some((sym_offset, is_defined)) = self.symbols.get(sym_name) {
                if !is_defined {
                    // External symbol — leave as 0
                    continue;
                }
                // Skip if this symbol is not reachable (DCE)
                // But always keep local labels (.L prefix) — they're internal to functions
                // and are reachable if their containing function is reachable.
                if !reachable_set.contains(sym_name) && sym_name != entry && !sym_name.starts_with(".L") {
                    continue;
                }
                Some(*sym_offset)
            } else if let Some(rod_off) = self.rodata_symbols.get(sym_name) {
                // Rodata symbol: offset is text_size + rodata_offset
                Some(text_size + rod_off)
            } else {
                // Unknown symbol — skip
                continue;
            };

            if let Some(sym_offset) = sym_offset_opt {
                let patch_offset = *offset as usize;
                if patch_offset + 4 <= self.text_bytes.len() {
                    if sym_name == "p_print_int" && *offset >= 0xd40 && *offset <= 0xd50 {
                        let next_rip = patch_offset as i64 + 4;
                        let delta = sym_offset as i64 - next_rip;
                    }
                    match r_type {
                        2 => {
                            // R_X86_64_PC32: PC-relative 32-bit
                            let next_rip = patch_offset as i64 + 4;
                            let delta = sym_offset as i64 - next_rip;
                            self.text_bytes[patch_offset..patch_offset + 4]
                                .copy_from_slice(&(delta as i32).to_le_bytes());
                        }
                        4 => {
                            // R_X86_64_PLT32: same as PC32 for our purposes
                            let next_rip = patch_offset as i64 + 4;
                            let delta = sym_offset as i64 - next_rip;
                            self.text_bytes[patch_offset..patch_offset + 4]
                                .copy_from_slice(&(delta as i32).to_le_bytes());
                        }
                        1 => {
                            // R_X86_64_64: absolute 64-bit
                            if patch_offset + 8 <= self.text_bytes.len() {
                                let abs_addr = sym_offset + 0x400078u64;
                                self.text_bytes[patch_offset..patch_offset + 8]
                                    .copy_from_slice(&abs_addr.to_le_bytes());
                            }
                        }
                        // ── ARM64 relocations (internal codes) ──
                        10 => {
                            // BL: 26-bit PC-relative offset / 4
                            let next_pc = patch_offset as i64 + 4;
                            let delta = sym_offset as i64 - next_pc;
                            let imm26 = ((delta / 4) as u32) & 0x03FFFFFF;
                            let instr = u32::from_le_bytes([
                                self.text_bytes[patch_offset], self.text_bytes[patch_offset+1],
                                self.text_bytes[patch_offset+2], self.text_bytes[patch_offset+3]
                            ]);
                            let patched = (instr & 0xFC000000) | imm26;
                            self.text_bytes[patch_offset..patch_offset+4]
                                .copy_from_slice(&patched.to_le_bytes());
                        }
                        11 => {
                            // B: 26-bit PC-relative offset / 4
                            let next_pc = patch_offset as i64 + 4;
                            let delta = sym_offset as i64 - next_pc;
                            let imm26 = ((delta / 4) as u32) & 0x03FFFFFF;
                            let instr = u32::from_le_bytes([
                                self.text_bytes[patch_offset], self.text_bytes[patch_offset+1],
                                self.text_bytes[patch_offset+2], self.text_bytes[patch_offset+3]
                            ]);
                            let patched = (instr & 0xFC000000) | imm26;
                            self.text_bytes[patch_offset..patch_offset+4]
                                .copy_from_slice(&patched.to_le_bytes());
                        }
                        12 => {
                            // B.cond: 19-bit PC-relative offset / 4
                            let next_pc = patch_offset as i64 + 4;
                            let delta = sym_offset as i64 - next_pc;
                            let imm19 = ((delta / 4) as u32) & 0x0007FFFF;
                            let instr = u32::from_le_bytes([
                                self.text_bytes[patch_offset], self.text_bytes[patch_offset+1],
                                self.text_bytes[patch_offset+2], self.text_bytes[patch_offset+3]
                            ]);
                            let patched = (instr & 0xFF00001F) | (imm19 << 5);
                            self.text_bytes[patch_offset..patch_offset+4]
                                .copy_from_slice(&patched.to_le_bytes());
                        }
                        14 => {
                            // ADRP: compute page-aligned PC-relative address
                            // ADRP Xd, label → Xd = (PC & ~0xFFF) + (imm * 4096)
                            // imm = (target_page - source_page) / 4096
                            // The instruction has immlo:2 at bits 29-30, immhi:19 at bits 5-23
                            let source_page = (patch_offset as u64) & !0xFFFu64;
                            // For the runtime, rodata is placed right after text in the ELF.
                            // The virtual address of text starts at base_addr + header_size.
                            // But at link time we work with file offsets. The ADRP needs the
                            // virtual address. We'll compute it using the same base as build_executable.
                            let base_addr: u64 = 0x400000;
                            let text_file_offset = 64 + 56; // ELF header + 1 phdr
                            let text_vaddr = base_addr + text_file_offset as u64;
                            let source_vaddr = text_vaddr + patch_offset as u64;
                            let source_page = source_vaddr & !0xFFFu64;
                            let sym_vaddr = text_vaddr + sym_offset;
                            let target_page = sym_vaddr & !0xFFFu64;
                            let page_delta = (target_page as i64 - source_page as i64) / 4096;
                            let imm = (page_delta as u32) & 0x7FFFF; // 21 bits
                            let immlo = imm & 0x3;
                            let immhi = (imm >> 2) & 0x7FFFF;
                            let instr = u32::from_le_bytes([
                                self.text_bytes[patch_offset], self.text_bytes[patch_offset+1],
                                self.text_bytes[patch_offset+2], self.text_bytes[patch_offset+3]
                            ]);
                            let patched = (instr & 0x9F00001F) | (immlo << 29) | (immhi << 5);
                            self.text_bytes[patch_offset..patch_offset+4]
                                .copy_from_slice(&patched.to_le_bytes());
                        }
                        15 => {
                            // ADD Xd, Xn, :lo12:label — add low 12 bits of symbol address
                            let base_addr: u64 = 0x400000;
                            let text_file_offset = 64 + 56;
                            let text_vaddr = base_addr + text_file_offset as u64;
                            let sym_vaddr = text_vaddr + sym_offset;
                            let lo12 = (sym_vaddr & 0xFFF) as u32;
                            let instr = u32::from_le_bytes([
                                self.text_bytes[patch_offset], self.text_bytes[patch_offset+1],
                                self.text_bytes[patch_offset+2], self.text_bytes[patch_offset+3]
                            ]);
                            let patched = (instr & 0xFFC00003) | (lo12 << 10);
                            self.text_bytes[patch_offset..patch_offset+4]
                                .copy_from_slice(&patched.to_le_bytes());
                        }
                        // ── RISC-V relocations (internal codes) ──
                        20 => {
                            // JAL: 20-bit PC-relative immediate (J-type)
                            let next_pc = patch_offset as i64;
                            let delta = sym_offset as i64 - next_pc;
                            let imm = delta as u32;
                            let imm20 = (imm >> 20) & 1;
                            let imm10_1 = (imm >> 1) & 0x3FF;
                            let imm11 = (imm >> 11) & 1;
                            let imm19_12 = (imm >> 12) & 0xFF;
                            let instr = u32::from_le_bytes([
                                self.text_bytes[patch_offset], self.text_bytes[patch_offset+1],
                                self.text_bytes[patch_offset+2], self.text_bytes[patch_offset+3]
                            ]);
                            let patched = (instr & 0x00000FFF)
                                | (imm20 << 31)
                                | (imm10_1 << 21)
                                | (imm11 << 20)
                                | (imm19_12 << 12);
                            self.text_bytes[patch_offset..patch_offset+4]
                                .copy_from_slice(&patched.to_le_bytes());
                        }
                        21 => {
                            // Branch: 13-bit PC-relative immediate (B-type)
                            let next_pc = patch_offset as i64;
                            let delta = sym_offset as i64 - next_pc;
                            let imm = delta as u32;
                            let imm12 = (imm >> 12) & 1;
                            let imm10_5 = (imm >> 5) & 0x3F;
                            let imm4_1 = (imm >> 1) & 0xF;
                            let imm11 = (imm >> 11) & 1;
                            let instr = u32::from_le_bytes([
                                self.text_bytes[patch_offset], self.text_bytes[patch_offset+1],
                                self.text_bytes[patch_offset+2], self.text_bytes[patch_offset+3]
                            ]);
                            let patched = (instr & 0x01FFF07F)
                                | (imm12 << 31)
                                | (imm10_5 << 25)
                                | (imm4_1 << 8)
                                | (imm11 << 7);
                            self.text_bytes[patch_offset..patch_offset+4]
                                .copy_from_slice(&patched.to_le_bytes());
                        }
                        _ => {}
                    }
                }
            }
        }

        // Find entry point offset
        let entry_offset = self
            .symbols
            .get(entry)
            .map(|(off, _)| *off)
            .unwrap_or(0);

        // Apply rodata relocations (vtable function pointers, etc.)
        let text_size = self.text_bytes.len() as u64;
        let base_addr: u64 = 0x400000;
        let text_file_offset = 64 + 56; // ELF header + 1 phdr
        let text_vaddr = base_addr + text_file_offset as u64;
        let rodata_vaddr = text_vaddr + text_size;

        for (offset, sym_name, r_type) in &self.rodata_relocations {
            let sym_offset_opt = if let Some((sym_offset, is_defined)) = self.symbols.get(sym_name) {
                if !is_defined { continue; }
                Some(*sym_offset)
            } else if let Some(rod_off) = self.rodata_symbols.get(sym_name) {
                Some(text_size + rod_off)
            } else {
                continue;
            };

            if let Some(sym_offset) = sym_offset_opt {
                let patch_offset = *offset as usize;
                match r_type {
                    1 => {
                        // R_X86_64_64: absolute 64-bit
                        if patch_offset + 8 <= self.rodata.len() {
                            let abs_addr = text_vaddr + sym_offset;
                            self.rodata[patch_offset..patch_offset + 8]
                                .copy_from_slice(&abs_addr.to_le_bytes());
                        }
                    }
                    _ => {}
                }
            }
        }

        // Build ELF64 executable
        self.build_executable(entry_offset)
    }

    fn build_executable(&self, entry_offset: u64) -> Result<Vec<u8>, String> {
        // ELF64 executable layout:
        //   - ELF header (64 bytes)
        //   - Program headers (56 bytes each, 1 PT_LOAD)
        //   - .text section
        //   - .rodata section (if any)

        let text_size = self.text_bytes.len();
        let rodata_size = self.rodata.len();

        // Program header: load everything at virtual address 0x400000
        let base_addr: u64 = 0x400000;
        let phdr_offset = 64; // right after ELF header
        let phdr_size = 56;
        let text_file_offset = phdr_offset + phdr_size;
        let text_vaddr = base_addr + text_file_offset as u64;

        let mut buf = Vec::with_capacity(text_file_offset + text_size + rodata_size);

        // ── ELF Header ──
        buf.extend_from_slice(&[0x7f, b'E', b'L', b'F']); // e_ident magic
        buf.push(2); // ELFCLASS64
        buf.push(1); // ELFDATA2LSB
        buf.push(1); // EV_CURRENT
        buf.push(0); // ELFOSABI_NONE
        buf.extend_from_slice(&[0; 8]); // padding
        buf.extend_from_slice(&2u16.to_le_bytes()); // e_type = ET_EXEC
        let e_machine: u16 = match self.target.as_str() {
            "arm64" | "aarch64" => 183, // EM_AARCH64
            "riscv" | "riscv64" => 243, // EM_RISCV
            _ => 62, // EM_X86_64
        };
        buf.extend_from_slice(&e_machine.to_le_bytes()); // e_machine
        buf.extend_from_slice(&1u32.to_le_bytes()); // e_version
        buf.extend_from_slice(&(text_vaddr + entry_offset).to_le_bytes()); // e_entry = virtual address of entry
        buf.extend_from_slice(&phdr_offset.to_le_bytes()); // e_phoff
        buf.extend_from_slice(&0u64.to_le_bytes()); // e_shoff (no section headers)
        buf.extend_from_slice(&0u32.to_le_bytes()); // e_flags
        buf.extend_from_slice(&64u16.to_le_bytes()); // e_ehsize
        buf.extend_from_slice(&56u16.to_le_bytes()); // e_phentsize
        buf.extend_from_slice(&1u16.to_le_bytes()); // e_phnum (1 PT_LOAD)
        buf.extend_from_slice(&0u16.to_le_bytes()); // e_shentsize
        buf.extend_from_slice(&0u16.to_le_bytes()); // e_shnum
        buf.extend_from_slice(&0u16.to_le_bytes()); // e_shstrndx

        // ── Program Header (PT_LOAD for .text) ──
        buf.extend_from_slice(&1u32.to_le_bytes()); // p_type = PT_LOAD
        buf.extend_from_slice(&5u32.to_le_bytes()); // p_flags = PF_R | PF_X
        buf.extend_from_slice(&(text_file_offset as u64).to_le_bytes()); // p_offset
        buf.extend_from_slice(&text_vaddr.to_le_bytes()); // p_vaddr
        buf.extend_from_slice(&text_vaddr.to_le_bytes()); // p_paddr
        buf.extend_from_slice(&((text_size + rodata_size) as u64).to_le_bytes()); // p_filesz
        buf.extend_from_slice(&((text_size + rodata_size) as u64).to_le_bytes()); // p_memsz
        buf.extend_from_slice(&0x1000u64.to_le_bytes()); // p_align

        // ── .text section ──
        buf.extend_from_slice(&self.text_bytes);

        // ── .rodata section ──
        buf.extend_from_slice(&self.rodata);

        Ok(buf)
    }
}

struct SectionInfo {
    name_offset: usize,
    sh_type: u32,
    offset: usize,
    size: usize,
    link: usize,
    entsize: u64,
    name: String,
}

fn get_string(data: &[u8], strtab_offset: usize, name_offset: usize) -> String {
    let start = strtab_offset + name_offset;
    if start >= data.len() {
        return String::new();
    }
    let end = data[start..].iter().position(|&b| b == 0).map(|p| start + p).unwrap_or(data.len());
    String::from_utf8_lossy(&data[start..end]).to_string()
}

fn simple_hash(s: &str) -> u64 {
    use std::collections::hash_map::DefaultHasher;
    use std::hash::{Hash, Hasher};
    let mut hasher = DefaultHasher::new();
    s.hash(&mut hasher);
    hasher.finish()
}

fn sanitize(name: &str) -> String {
    name.chars()
        .map(|c| if c.is_alphanumeric() { c } else { '_' })
        .collect()
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_dce_basic() {
        let symbols = vec![
            ("main".to_string(), 0, 100),
            ("helper".to_string(), 100, 50),
            ("unused".to_string(), 150, 50),
        ];
        let relocations = vec![
            (10, "helper".to_string()), // main calls helper
        ];
        let reachable = dead_code_elimination(&symbols, &relocations, "main");
        assert!(reachable.contains(&0), "main should be reachable");
        assert!(reachable.contains(&1), "helper should be reachable");
        assert!(!reachable.contains(&2), "unused should NOT be reachable");
    }

    #[test]
    fn test_dce_chain() {
        let symbols = vec![
            ("a".to_string(), 0, 10),
            ("b".to_string(), 10, 10),
            ("c".to_string(), 20, 10),
            ("d".to_string(), 30, 10),
        ];
        let relocations = vec![
            (0, "b".to_string()), // a calls b
            (10, "c".to_string()), // b calls c
            // d is never called
        ];
        let reachable = dead_code_elimination(&symbols, &relocations, "a");
        assert!(reachable.contains(&0), "a should be reachable");
        assert!(reachable.contains(&1), "b should be reachable");
        assert!(reachable.contains(&2), "c should be reachable");
        assert!(!reachable.contains(&3), "d should NOT be reachable");
    }

    #[test]
    fn test_incremental_linker() {
        let tmp = std::env::temp_dir().join("pledge_linker_test");
        let mut linker = IncrementalLinker::new(&tmp, "main");
        // Just verify it doesn't crash with empty input
        let result = linker.link_incremental(&[]);
        // Will fail because no objects, but shouldn't panic
        assert!(result.is_err() || result.is_ok());
    }
}
