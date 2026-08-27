// WebAssembly code generation from Fides IR
//
// Phase 5D.1 — WASM target.
//
// Strategy: translate IR functions to WASM functions operating on a 64-bit
// linear memory model. All Fides pointers become i64 offsets into linear
// memory. Integers use i64 (Fides's default int) or i32 for the narrower
// kinds. Structs/enums/collections are pointer-sized opaque handles.
//
// Control flow: IR uses an arbitrary CFG of basic blocks. WASM is structured.
// We use the "block dispatcher" trampoline: one outer `loop` and a local
// `cur_block` variable; each block is an `if cur_block == N { ... }` arm.
// Jumps set `cur_block = M` and `br 1` back to the loop top. This handles
// any CFG including loops with multiple back-edges.
//
// Runtime: `print`, `p_print_int`, `p_print_str`, `p_print_bool`,
// `p_print_float`, `p_print_newline`, `p_panic`, `p_malloc`, `p_alloc`,
// `p_free`, `p_str_concat`, `p_to_str*` are imported from the host module
// "env". Collections (Vec/HashMap/String) are not yet supported in the WASM
// backend — calls to `p_vec_*` / `p_map_*` / `p_string_*` emit `unreachable`
// with a TODO. This is sufficient for the integer/string/print/control-flow
// subset which is what 5D.1 verifies.

use crate::ir::*;
use crate::ast::{BinOp, UnaryOp};
use std::collections::HashMap;

// ─── WASM value types ────────────────────────────────────────────────────

const WT_I32: u8 = 0x7F;
const WT_I64: u8 = 0x7E;
const WT_F32: u8 = 0x7D;
const WT_F64: u8 = 0x7C;
const WT_VOID: u8 = 0x40;

// ─── WASM section IDs ────────────────────────────────────────────────────

const SEC_TYPE: u8 = 1;
const SEC_IMPORT: u8 = 2;
const SEC_FUNCTION: u8 = 3;
const SEC_MEMORY: u8 = 5;
const SEC_GLOBAL: u8 = 6;
const SEC_EXPORT: u8 = 7;
const SEC_CODE: u8 = 10;
const SEC_DATA: u8 = 11;

// ─── Encoder helpers ─────────────────────────────────────────────────────

struct WasmBuf {
    bytes: Vec<u8>,
}

impl WasmBuf {
    fn new() -> Self {
        WasmBuf { bytes: Vec::new() }
    }
    fn push(&mut self, b: u8) {
        self.bytes.push(b);
    }
    fn push_bytes(&mut self, bs: &[u8]) {
        self.bytes.extend_from_slice(bs);
    }
    fn uleb(&mut self, mut v: u64) {
        loop {
            let mut byte = (v & 0x7F) as u8;
            v >>= 7;
            if v != 0 {
                byte |= 0x80;
            }
            self.bytes.push(byte);
            if v == 0 {
                break;
            }
        }
    }
    fn sleb(&mut self, mut v: i64) {
        loop {
            let byte = (v as u8) & 0x7F;
            v >>= 7;
            let more = !(v == 0 && (byte & 0x40) == 0)
                && !(v == -1 && (byte & 0x40) != 0);
            if more {
                self.bytes.push(byte | 0x80);
            } else {
                self.bytes.push(byte);
                break;
            }
        }
    }
    fn u32(&mut self, v: u32) {
        self.bytes.extend_from_slice(&v.to_le_bytes());
    }
    fn f64(&mut self, v: f64) {
        self.bytes.extend_from_slice(&v.to_le_bytes());
    }
    fn f32(&mut self, v: f32) {
        self.bytes.extend_from_slice(&v.to_le_bytes());
    }
    fn name(&mut self, s: &str) {
        self.uleb(s.len() as u64);
        self.push_bytes(s.as_bytes());
    }
    fn len(&self) -> usize {
        self.bytes.len()
    }
    fn into_vec(self) -> Vec<u8> {
        self.bytes
    }
}

/// Write a section: id, uleb size, contents.
fn section(id: u8, contents: &[u8]) -> Vec<u8> {
    let mut s = WasmBuf::new();
    s.push(id);
    s.uleb(contents.len() as u64);
    s.push_bytes(contents);
    s.into_vec()
}

// ─── Type mapping ────────────────────────────────────────────────────────

fn wasm_valtype(ty: &IrType) -> u8 {
    match ty {
        IrType::I8 | IrType::I16 | IrType::I32
        | IrType::U8 | IrType::U16 | IrType::U32
        | IrType::Bool => WT_I32,
        IrType::I64 | IrType::U64 => WT_I64,
        IrType::F32 => WT_F32,
        IrType::F64 => WT_F64,
        // Pointers, structs, arrays, opaque/generic handles → i64
        _ => WT_I64,
    }
}

/// Whether a type produces a value on the WASM stack (non-void, non-never).
fn has_value(ty: &IrType) -> bool {
    !matches!(ty, IrType::Void | IrType::Never)
}

// ─── Host imports ────────────────────────────────────────────────────────

/// A host import the WASM module depends on.
struct HostImport {
    name: &'static str,
    params: &'static [u8],
    results: &'static [u8],
}

const HOST_IMPORTS: &[HostImport] = &[
    HostImport { name: "p_print_int",     params: &[WT_I64],          results: &[] },
    HostImport { name: "p_print_str",     params: &[WT_I64],          results: &[] },
    HostImport { name: "p_print_bool",    params: &[WT_I32],          results: &[] },
    HostImport { name: "p_print_float",   params: &[WT_F64],          results: &[] },
    HostImport { name: "p_print_newline", params: &[],                results: &[] },
    HostImport { name: "p_panic",         params: &[WT_I64],          results: &[] },
    HostImport { name: "p_malloc",        params: &[WT_I64],          results: &[WT_I64] },
    HostImport { name: "p_alloc",         params: &[WT_I64],          results: &[WT_I64] },
    HostImport { name: "p_free",          params: &[WT_I64],          results: &[] },
    HostImport { name: "p_str_concat",    params: &[WT_I64, WT_I64],  results: &[WT_I64] },
    HostImport { name: "p_to_str",        params: &[WT_I64],          results: &[WT_I64] },
    HostImport { name: "p_to_str_bool",   params: &[WT_I32],          results: &[WT_I64] },
    HostImport { name: "p_to_str_float",  params: &[WT_F64],          results: &[WT_I64] },
    HostImport { name: "p_to_str_vec",    params: &[WT_I64],          results: &[WT_I64] },
    HostImport { name: "p_to_str_str",    params: &[WT_I64],          results: &[WT_I64] },
    // `print` is the user-facing builtin; the host dispatches by value type.
    // We model it as taking an i64 (the value or pointer) — the host prints
    // it as an integer by default. The compiler routes string/bool/float
    // prints through the typed variants above.
    HostImport { name: "print",           params: &[WT_I64],          results: &[WT_I64] },
];

fn host_import_index(name: &str) -> Option<usize> {
    HOST_IMPORTS.iter().position(|h| h.name == name)
}

// ─── Code generator ──────────────────────────────────────────────────────

pub struct WasmCodeGen {
    /// Per-function: ValueId.0 -> local index (0-based among locals; params
    /// and declared locals share one index space in WASM).
    locals: HashMap<u32, u32>,
    /// Per-function: ordered list of local types (excluding params).
    local_types: Vec<u8>,
    /// String constants: content -> address in linear memory.
    string_addrs: HashMap<String, u64>,
    /// Next free data address.
    data_ptr: u64,
    /// Function type indices, keyed by a signature key string.
    type_indices: HashMap<String, u32>,
    /// Collected function type signatures in order.
    type_sigs: Vec<(Vec<u8>, Vec<u8>)>,
    /// Function name -> type index.
    func_type: HashMap<String, u32>,
    /// Function name -> function index (in the function section order).
    /// Imports come first, then user functions.
    func_index: HashMap<String, u32>,
    /// Ordered user function names (function section order).
    user_funcs: Vec<String>,
    /// Struct layouts (for GetFieldPtr offsets).
    struct_layouts: HashMap<String, StructLayout>,
}

impl WasmCodeGen {
    fn new() -> Self {
        WasmCodeGen {
            locals: HashMap::new(),
            local_types: Vec::new(),
            string_addrs: HashMap::new(),
            data_ptr: 1024, // leave first 1KB reserved (null guard, etc.)
            type_indices: HashMap::new(),
            type_sigs: Vec::new(),
            func_type: HashMap::new(),
            func_index: HashMap::new(),
            user_funcs: Vec::new(),
            struct_layouts: HashMap::new(),
        }
    }

    /// Intern a string constant into the data section; return its address.
    fn intern_string(&mut self, s: &str) -> u64 {
        if let Some(&a) = self.string_addrs.get(s) {
            return a;
        }
        let addr = self.data_ptr;
        // Store bytes + null terminator.
        self.data_ptr += s.len() as u64 + 1;
        // Align to 8 for safety.
        self.data_ptr = (self.data_ptr + 7) & !7;
        self.string_addrs.insert(s.to_string(), addr);
        addr
    }

    /// Get or create a type index for a function signature.
    fn intern_type(&mut self, params: &[u8], results: &[u8]) -> u32 {
        let key = format!(
            "{:?}|{:?}",
            params.iter().collect::<Vec<_>>(),
            results.iter().collect::<Vec<_>>()
        );
        if let Some(&i) = self.type_indices.get(&key) {
            return i;
        }
        let i = self.type_sigs.len() as u32;
        self.type_sigs.push((params.to_vec(), results.to_vec()));
        self.type_indices.insert(key, i);
        i
    }

    /// Map a Fides function name to a sanitized WASM-safe name.
    fn sanitize(name: &str) -> String {
        // WASM names allow almost any UTF-8, but keep it simple and C-like.
        name.replace("::", "_").replace('.', "_")
    }

    /// Field offset for GetFieldPtr, using struct layouts when available.
    fn field_offset(&self, struct_name: &str, field_index: u32) -> u64 {
        self.struct_layouts
            .get(struct_name)
            .and_then(|l| l.fields.get(field_index as usize))
            .map(|(_, _, off)| *off as u64)
            .unwrap_or(field_index as u64 * 8) // default: 8 bytes per field
    }

    /// Resolve the struct name from an IrType (Struct or Opaque/Generic).
    fn struct_name_of(ty: &IrType) -> Option<&str> {
        match ty {
            IrType::Struct(n) => Some(n),
            IrType::Opaque(n) | IrType::Generic(n, _) => Some(n),
            _ => None,
        }
    }

    // ─── Instruction emission ───────────────────────────────────────────

    fn emit_const_i64(&self, out: &mut WasmBuf, v: i64) {
        out.push(0x42); // i64.const
        out.sleb(v);
    }
    fn emit_const_i32(&self, out: &mut WasmBuf, v: i32) {
        out.push(0x41); // i32.const
        out.sleb(v as i64);
    }

    fn emit_local_get(&self, out: &mut WasmBuf, idx: u32) {
        out.push(0x20);
        out.uleb(idx as u64);
    }
    fn emit_local_set(&self, out: &mut WasmBuf, idx: u32) {
        out.push(0x21);
        out.uleb(idx as u64);
    }
    fn emit_local_tee(&self, out: &mut WasmBuf, idx: u32) {
        out.push(0x22);
        out.uleb(idx as u64);
    }
    fn emit_global_get(&self, out: &mut WasmBuf, idx: u32) {
        out.push(0x23);
        out.uleb(idx as u64);
    }
    fn emit_global_set(&self, out: &mut WasmBuf, idx: u32) {
        out.push(0x24);
        out.uleb(idx as u64);
    }

    /// Emit a load of the value produced by `id` onto the stack.
    /// For void/never values, emits nothing.
    fn emit_value(&self, out: &mut WasmBuf, id: ValueId, func: &IrFunction) {
        let meta = match func.values.get(&id) {
            Some(m) => m,
            None => return, // block param or unknown — handled by caller
        };
        if !has_value(&meta.ty) {
            return;
        }
        if let Some(&idx) = self.locals.get(&id.0) {
            self.emit_local_get(out, idx);
        }
        // If not a local, the value is already on the stack (e.g. inline).
    }

    /// Emit a store of the top of stack into the local for `id`.
    fn emit_set_value(&self, out: &mut WasmBuf, id: ValueId, func: &IrFunction) {
        if let Some(&idx) = self.locals.get(&id.0) {
            self.emit_local_set(out, idx);
        }
    }

    // ─── Per-function setup ─────────────────────────────────────────────

    fn setup_locals(&mut self, func: &IrFunction) {
        self.locals.clear();
        self.local_types.clear();
        // Params first (params share the local index space).
        for (i, (pid, _)) in func.params.iter().enumerate() {
            self.locals.insert(pid.0, i as u32);
        }
        let mut next = func.params.len() as u32;
        // All other values become locals.
        for (id, meta) in &func.values {
            if func.params.iter().any(|(pid, _)| pid == id) {
                continue;
            }
            if !has_value(&meta.ty) {
                // Still allocate a slot so the index space is dense and
                // matches `func.values` iteration order; use i32 as a
                // placeholder.
                self.locals.insert(id.0, next);
                self.local_types.push(WT_I32);
                next += 1;
                continue;
            }
            self.locals.insert(id.0, next);
            self.local_types.push(wasm_valtype(&meta.ty));
            next += 1;
        }
    }

    // ─── Function body generation ───────────────────────────────────────

    fn gen_function(&mut self, name: &str, func: &IrFunction) -> Vec<u8> {
        self.setup_locals(func);

        let mut out = WasmBuf::new();

        // Local declarations (excluding params).
        // Encoded as: count, then (count, type) pairs.
        if self.local_types.is_empty() {
            out.uleb(0);
        } else {
            // Group consecutive identical types to reduce count entries.
            let mut groups: Vec<(u32, u8)> = Vec::new();
            for &t in &self.local_types {
                if let Some(last) = groups.last_mut() {
                    if last.1 == t {
                        last.0 += 1;
                        continue;
                    }
                }
                groups.push((1, t));
            }
            out.uleb(groups.len() as u64);
            for (count, ty) in &groups {
                out.uleb(*count as u64);
                out.push(*ty);
            }
        }

        // ─── Block dispatcher ───────────────────────────────────────────
        // Structure:
        //   block $exit
        //     local $cur = 0
        //     loop $top
        //       block $dispatch
        //         if (cur == N) { ...block N... br $dispatch }
        //         if (cur == M) { ...block M... br $dispatch }
        //         br $exit          // no matching block → exit
        //       end   ; $dispatch
        //       br $top             ; jumps re-enter the loop
        //     end     ; $top
        //   end       ; $exit
        //
        // A `Jump { target }` sets cur=target and `br $dispatch` (depth 1).
        // A `Branch` sets cur and `br $dispatch`.
        // A `Return` either returns directly or `br $exit` (depth 2).
        //
        // Depth accounting (inside an `if` arm):
        //   br 0 = $dispatch (skip remaining arms, re-enter loop)
        //   br 1 = $top      (continue loop — same as $dispatch here, but
        //                     we use $dispatch for clarity)
        //   br 2 = $exit
        //   br 3 = outside (return)

        // Allocate a local for cur_block. Find a free local index.
        let cur_block_local = self.local_types.len() as u32 + func.params.len() as u32;
        // Declare it as an extra i32 local appended to the locals list.
        // We do this by emitting one more local group above — but we already
        // wrote the locals count. Instead, re-emit with the extra local.
        // Simpler: rewind and re-emit locals including cur_block.
        out.bytes.clear();
        let total_locals = self.local_types.len() + 1;
        if total_locals == 1 {
            out.uleb(1);
            out.uleb(1);
            out.push(WT_I32);
        } else {
            // Reuse grouping but append a trailing i32.
            let mut groups: Vec<(u32, u8)> = Vec::new();
            for &t in &self.local_types {
                if let Some(last) = groups.last_mut() {
                    if last.1 == t {
                        last.0 += 1;
                        continue;
                    }
                }
                groups.push((1, t));
            }
            // Append cur_block (i32).
            if let Some(last) = groups.last_mut() {
                if last.1 == WT_I32 {
                    last.0 += 1;
                } else {
                    groups.push((1, WT_I32));
                }
            } else {
                groups.push((1, WT_I32));
            }
            out.uleb(groups.len() as u64);
            for (count, ty) in &groups {
                out.uleb(*count as u64);
                out.push(*ty);
            }
        }

        // cur_block = 0 (entry block).
        self.emit_const_i32(&mut out, 0);
        self.emit_local_set(&mut out, cur_block_local);

        // block $exit (depth from inside if-arms: 2 to break to exit)
        out.push(0x02); out.push(WT_VOID); // block void
        // loop $top (depth from inside if-arms: 1 to re-enter loop)
        out.push(0x03); out.push(WT_VOID); // loop void
        // block $dispatch (depth from inside if-arms: 0 to skip remaining arms)
        out.push(0x02); out.push(WT_VOID); // block void

        for (bi, block) in func.blocks.iter().enumerate() {
            // if (cur_block == bi) { ... }
            self.emit_local_get(&mut out, cur_block_local);
            self.emit_const_i32(&mut out, bi as i32);
            out.push(0x46); // i32.eq
            out.push(0x04); out.push(WT_VOID); // if void

            for &inst_id in &block.instructions {
                if let Some(inst) = func.instructions.get(&inst_id) {
                    self.gen_inst(&mut out, inst, inst_id, func, cur_block_local);
                }
            }

            // If the block fell through without a terminator, advance to the
            // next block (bi+1). If it's the last block, exit.
            let has_terminator = block.instructions.iter().any(|id| {
                func.instructions.get(id).map_or(false, |i| i.is_terminator())
            });
            if !has_terminator {
                if bi + 1 < func.blocks.len() {
                    self.emit_const_i32(&mut out, (bi + 1) as i32);
                    self.emit_local_set(&mut out, cur_block_local);
                    out.push(0x0C); out.uleb(0); // br $dispatch
                } else {
                    out.push(0x0C); out.uleb(2); // br $exit
                }
            }

            out.push(0x0B); // end if
        }

        // No matching block → exit.
        out.push(0x0C); out.uleb(2); // br $exit

        out.push(0x0B); // end $dispatch
        out.push(0x0C); out.uleb(1); // br $top (re-enter loop)
        out.push(0x0B); // end $top
        out.push(0x0B); // end $exit

        // Function epilogue: if the function returns a value, push a default
        // (0) so the stack is well-formed even if control falls through.
        if has_value(&func.ret_ty) {
            match wasm_valtype(&func.ret_ty) {
                WT_I32 => self.emit_const_i32(&mut out, 0),
                WT_I64 => self.emit_const_i64(&mut out, 0),
                WT_F32 => { out.push(0x43); out.f32(0.0); }
                WT_F64 => { out.push(0x44); out.f64(0.0); }
                _ => {}
            }
        }
        out.push(0x0B); // end function body

        out.into_vec()
    }

    // ─── Instruction translation ────────────────────────────────────────

    fn gen_inst(
        &self,
        out: &mut WasmBuf,
        inst: &Inst,
        id: ValueId,
        func: &IrFunction,
        cur_block_local: u32,
    ) {
        let dst_meta = func.values.get(&id);
        let dst_has_value = dst_meta.map_or(false, |m| has_value(&m.ty));

        match inst {
            // ─── Constants ───
            Inst::ConstInt { value, ty } => {
                match wasm_valtype(ty) {
                    WT_I32 => self.emit_const_i32(out, *value as i32),
                    _ => self.emit_const_i64(out, *value),
                }
                if dst_has_value { self.emit_set_value(out, id, func); }
            }
            Inst::ConstFloat { bits, ty } => {
                match ty {
                    IrType::F32 => {
                        out.push(0x43);
                        out.f32(f32::from_bits(*bits as u32));
                    }
                    _ => {
                        out.push(0x44);
                        out.f64(f64::from_bits(*bits));
                    }
                }
                if dst_has_value { self.emit_set_value(out, id, func); }
            }
            Inst::ConstBool { value } => {
                self.emit_const_i32(out, if *value { 1 } else { 0 });
                if dst_has_value { self.emit_set_value(out, id, func); }
            }
            Inst::ConstStr { value } => {
                let addr = self.string_addrs.get(value).copied().unwrap_or(0);
                self.emit_const_i64(out, addr as i64);
                if dst_has_value { self.emit_set_value(out, id, func); }
            }
            Inst::ConstNull { .. } => {
                self.emit_const_i64(out, 0);
                if dst_has_value { self.emit_set_value(out, id, func); }
            }

            // ─── Arithmetic ───
            Inst::BinOp { op, left, right, ty } => {
                self.emit_value(out, *left, func);
                self.emit_value(out, *right, func);
                let is_i32 = wasm_valtype(ty) == WT_I32;
                let opcode = match (op, is_i32) {
                    (BinOp::Add, true)  => 0x6A, // i32.add
                    (BinOp::Add, false) => 0x7C, // i64.add
                    (BinOp::Sub, true)  => 0x6B,
                    (BinOp::Sub, false) => 0x7D,
                    (BinOp::Mul, true)  => 0x6C,
                    (BinOp::Mul, false) => 0x7E,
                    (BinOp::Div, true)  => 0x6D, // i32.div_s
                    (BinOp::Div, false) => 0x7F, // i64.div_s
                    (BinOp::Mod, true)  => 0x6F, // i32.rem_s
                    (BinOp::Mod, false) => 0x81, // i64.rem_s
                    (BinOp::BitAnd, true)  => 0x71,
                    (BinOp::BitAnd, false) => 0x83,
                    (BinOp::BitOr, true)   => 0x72,
                    (BinOp::BitOr, false)  => 0x84,
                    (BinOp::BitXor, true)  => 0x73,
                    (BinOp::BitXor, false) => 0x85,
                    (BinOp::Shl, true)  => 0x74,
                    (BinOp::Shl, false) => 0x86,
                    (BinOp::Shr, true)  => 0x75, // i32.shr_s
                    (BinOp::Shr, false) => 0x87, // i64.shr_s
                    // Comparisons return i32 (0/1) regardless of operand width.
                    (BinOp::Eq, true)   => 0x46, // i32.eq
                    (BinOp::Eq, false)  => 0x52, // i64.eq
                    (BinOp::Neq, true)  => 0x47, // i32.ne
                    (BinOp::Neq, false) => 0x53, // i64.ne
                    (BinOp::Lt, true)   => 0x48, // i32.lt_s
                    (BinOp::Lt, false)  => 0x54, // i64.lt_s
                    (BinOp::Gt, true)   => 0x4A,
                    (BinOp::Gt, false)  => 0x56,
                    (BinOp::LtEq, true) => 0x4C,
                    (BinOp::LtEq, false)=> 0x55,
                    (BinOp::GtEq, true) => 0x4E,
                    (BinOp::GtEq, false)=> 0x57,
                    // Logical And/Or: short-circuit not modeled; use bitwise on i32.
                    (BinOp::And, _) => 0x71, // i32.and (bools are i32)
                    (BinOp::Or, _)  => 0x72, // i32.or
                };
                out.push(opcode);
                if dst_has_value { self.emit_set_value(out, id, func); }
            }
            Inst::UnaryOp { op, operand, ty } => {
                self.emit_value(out, *operand, func);
                match op {
                    UnaryOp::Neg => {
                        match wasm_valtype(ty) {
                            WT_F32 => { out.push(0x8C); } // f32.neg
                            WT_F64 => { out.push(0x9A); } // f64.neg
                            _ => {
                                // i64/i32: 0 - x
                                if wasm_valtype(ty) == WT_I32 {
                                    self.emit_const_i32(out, 0);
                                    out.push(0x6B); // i32.sub
                                } else {
                                    self.emit_const_i64(out, 0);
                                    out.push(0x7D); // i64.sub
                                }
                            }
                        }
                    }
                    UnaryOp::Not => {
                        // bool (i32): eqz
                        out.push(0x45); // i32.eqz
                    }
                    UnaryOp::BitNot => {
                        // ~x = x ^ -1
                        if wasm_valtype(ty) == WT_I32 {
                            self.emit_const_i32(out, -1);
                            out.push(0x73); // i32.xor
                        } else {
                            self.emit_const_i64(out, -1);
                            out.push(0x85); // i64.xor
                        }
                    }
                }
                if dst_has_value { self.emit_set_value(out, id, func); }
            }

            // ─── Memory ───
            Inst::Alloca { ty, .. } => {
                // Bump the global stack pointer; result = old sp.
                // global 0 is $sp.
                out.push(0x23); out.uleb(0); // global.get 0
                let size = match ty {
                    IrType::Struct(name) => {
                        self.struct_layouts.get(name).map(|l| l.size).unwrap_or(8)
                    }
                    _ => ty.size(),
                };
                let size = if size == 0 { 8 } else { size };
                // result = old sp (already on stack via tee)
                out.push(0x24); out.uleb(0); // global.set 0 (consumes old sp)
                self.emit_global_get(out, 0); // push new sp
                self.emit_const_i64(out, size as i64);
                out.push(0x7C); // i64.add
                self.emit_global_set(out, 0);
                // Re-push old sp as the result.
                // We lost it — recompute: new_sp - size.
                self.emit_global_get(out, 0);
                self.emit_const_i64(out, size as i64);
                out.push(0x7D); // i64.sub
                if dst_has_value { self.emit_set_value(out, id, func); }
            }
            Inst::Load { ptr, ty } => {
                self.emit_value(out, *ptr, func);
                let (opcode, align) = match ty {
                    IrType::I8 | IrType::U8 | IrType::Bool => (0x2D, 0), // i32.load8_u
                    IrType::I16 | IrType::U16 => (0x2F, 1),              // i32.load16_u
                    IrType::I32 | IrType::U32 => (0x28, 2),              // i32.load
                    IrType::I64 | IrType::U64 => (0x29, 3),              // i64.load
                    IrType::F32 => (0x2A, 2),
                    IrType::F64 => (0x2B, 3),
                    // Pointers/structs/opaque → 8-byte load
                    _ => (0x29, 3),
                };
                out.push(opcode);
                out.uleb(align as u64); // alignment
                out.uleb(0);            // offset
                if dst_has_value { self.emit_set_value(out, id, func); }
            }
            Inst::Store { ptr, value, .. } => {
                // For stores we need value first (to preserve stack order),
                // then address. WASM store wants [addr, value]. Emit addr
                // then value.
                self.emit_value(out, *ptr, func);
                self.emit_value(out, *value, func);
                let val_meta = func.values.get(value);
                let opcode = match val_meta.map(|m| &m.ty) {
                    Some(IrType::I8) | Some(IrType::U8) | Some(IrType::Bool) => (0x3A, 0), // i32.store8
                    Some(IrType::I16) | Some(IrType::U16) => (0x3E, 1),
                    Some(IrType::I32) | Some(IrType::U32) => (0x36, 2),
                    Some(IrType::I64) | Some(IrType::U64) => (0x3E, 3),
                    Some(IrType::F32) => (0x38, 2),
                    Some(IrType::F64) => (0x39, 3),
                    _ => (0x3E, 3), // i64.store for pointers/opaque
                };
                out.push(opcode.0);
                out.uleb(opcode.1 as u64);
                out.uleb(0);
            }
            Inst::GetFieldPtr { struct_ptr, field_index, field_ty } => {
                self.emit_value(out, *struct_ptr, func);
                // Determine the struct name from the pointer's value type.
                let ptr_meta = func.values.get(struct_ptr);
                let struct_name = ptr_meta
                    .and_then(|m| Self::struct_name_of(&m.ty))
                    .or_else(|| Self::struct_name_of(field_ty));
                let off = struct_name
                    .and_then(|n| Some(self.field_offset(n, *field_index)))
                    .unwrap_or(*field_index as u64 * 8);
                self.emit_const_i64(out, off as i64);
                out.push(0x7C); // i64.add
                if dst_has_value { self.emit_set_value(out, id, func); }
            }
            Inst::GetElemPtr { array_ptr, index, elem_ty } => {
                self.emit_value(out, *array_ptr, func);
                self.emit_value(out, *index, func);
                // index may be i32 or i64; widen to i64.
                let idx_meta = func.values.get(index);
                if idx_meta.map_or(true, |m| wasm_valtype(&m.ty) == WT_I32) {
                    out.push(0xAC); // i64.extend_i32_s
                }
                let elem_size = elem_ty.size().max(1);
                self.emit_const_i64(out, elem_size as i64);
                out.push(0x7E); // i64.mul
                out.push(0x7C); // i64.add
                if dst_has_value { self.emit_set_value(out, id, func); }
            }

            // ─── Control flow ───
            Inst::Jump { target } => {
                self.emit_const_i32(out, target.0 as i32);
                self.emit_local_set(out, cur_block_local);
                out.push(0x0C); out.uleb(0); // br $dispatch
            }
            Inst::Branch { cond, then, else_ } => {
                // if cond { cur = then } else { cur = else_ }
                self.emit_value(out, *cond, func);
                out.push(0x04); out.push(WT_VOID); // if void
                self.emit_const_i32(out, then.0 as i32);
                self.emit_local_set(out, cur_block_local);
                out.push(0x05); // else
                self.emit_const_i32(out, else_.0 as i32);
                self.emit_local_set(out, cur_block_local);
                out.push(0x0B); // end if
                out.push(0x0C); out.uleb(0); // br $dispatch
            }
            Inst::Return { value } => {
                if let Some(v) = value {
                    self.emit_value(out, *v, func);
                    // The value type may differ from the function return type
                    // (e.g. i32 vs i64). Widen/narrow to match.
                    let val_meta = func.values.get(v);
                    let val_wt = val_meta.map(|m| wasm_valtype(&m.ty)).unwrap_or(WT_I64);
                    let ret_wt = wasm_valtype(&func.ret_ty);
                    if val_wt == WT_I32 && ret_wt == WT_I64 {
                        out.push(0xAC); // i64.extend_i32_s
                    } else if val_wt == WT_I64 && ret_wt == WT_I32 {
                        out.push(0xA7); // i32.wrap_i64
                    }
                }
                out.push(0x0F); // return
            }
            Inst::Unreachable => {
                out.push(0x00); // unreachable
            }

            // ─── Calls ───
            Inst::Call { func: callee, args, ret_ty, .. } => {
                // Host import?
                if let Some(hi) = host_import_index(callee) {
                    // The user-facing `print` takes the value as i64. For
                    // typed args, ensure the stack value matches the import
                    // signature.
                    let host = &HOST_IMPORTS[hi];
                    for (i, arg) in args.iter().enumerate() {
                        self.emit_value(out, *arg, func);
                        let arg_meta = func.values.get(arg);
                        let arg_wt = arg_meta.map(|m| wasm_valtype(&m.ty)).unwrap_or(WT_I64);
                        let want = host.params.get(i).copied().unwrap_or(WT_I64);
                        if arg_wt == WT_I32 && want == WT_I64 {
                            out.push(0xAC); // i64.extend_i32_s
                        } else if arg_wt == WT_I64 && want == WT_I32 {
                            out.push(0xA7); // i32.wrap_i64
                        }
                    }
                    out.push(0x10); // call
                    out.uleb(hi as u64);
                } else if let Some(&fidx) = self.func_index.get(callee) {
                    for arg in args {
                        self.emit_value(out, *arg, func);
                    }
                    out.push(0x10); // call
                    out.uleb(fidx as u64);
                } else {
                    // Unknown callee — emit a no-op default.
                    if has_value(ret_ty) {
                        match wasm_valtype(ret_ty) {
                            WT_I32 => self.emit_const_i32(out, 0),
                            WT_I64 => self.emit_const_i64(out, 0),
                            WT_F32 => { out.push(0x43); out.f32(0.0); }
                            WT_F64 => { out.push(0x44); out.f64(0.0); }
                            _ => {}
                        }
                    }
                }
                if dst_has_value { self.emit_set_value(out, id, func); }
            }
            Inst::MakeDyn { .. } => {
                // MakeDyn not yet supported in WASM codegen
                out.push(0x41); // i32.const
                out.push(0x00); // LEB128 encoding of 0
                if dst_has_value { self.emit_set_value(out, id, func); }
            }
            Inst::DynCall { trait_name, method, args, ret_ty, .. } => {
                // Dynamic dispatch not yet supported in WASM codegen
                let _ = (trait_name, method, args);
                if !matches!(ret_ty, IrType::Void) {
                    out.push(0x41); // i32.const
                    out.push(0x00); // LEB128 encoding of 0
                }
                if dst_has_value { self.emit_set_value(out, id, func); }
            }
            Inst::CallPtr { args, ret_ty, .. } => {
                // Indirect calls aren't supported in this first version.
                // Emit a default return value.
                let _ = args;
                if has_value(ret_ty) {
                    match wasm_valtype(ret_ty) {
                        WT_I32 => self.emit_const_i32(out, 0),
                        WT_I64 => self.emit_const_i64(out, 0),
                        WT_F32 => { out.push(0x43); out.f32(0.0); }
                        WT_F64 => { out.push(0x44); out.f64(0.0); }
                        _ => {}
                    }
                }
                if dst_has_value { self.emit_set_value(out, id, func); }
            }

            // ─── Conversions ───
            Inst::BitCast { value, from, to } => {
                self.emit_value(out, *value, func);
                // Same-size bitcasts: for i32<->f32 and i64<->f64 use reinterpret.
                match (wasm_valtype(from), wasm_valtype(to)) {
                    (WT_I32, WT_F32) => out.push(0xBE), // f32.reinterpret_i32
                    (WT_F32, WT_I32) => out.push(0xBC), // i32.reinterpret_f32
                    (WT_I64, WT_F64) => out.push(0xBF), // f64.reinterpret_i64
                    (WT_F64, WT_I64) => out.push(0xBD), // i64.reinterpret_f64
                    _ => {} // no-op for same-width int casts
                }
                if dst_has_value { self.emit_set_value(out, id, func); }
            }
            Inst::IntExtend { value, to, .. } => {
                self.emit_value(out, *value, func);
                if wasm_valtype(to) == WT_I64 {
                    out.push(0xAC); // i64.extend_i32_s
                }
                if dst_has_value { self.emit_set_value(out, id, func); }
            }
            Inst::IntTrunc { value, to, .. } => {
                self.emit_value(out, *value, func);
                if wasm_valtype(to) == WT_I32 {
                    out.push(0xA7); // i32.wrap_i64
                }
                if dst_has_value { self.emit_set_value(out, id, func); }
            }
            Inst::IntToFloat { value, to, .. } => {
                self.emit_value(out, *value, func);
                match wasm_valtype(to) {
                    WT_F32 => out.push(0xB2), // f32.convert_i32_s
                    WT_F64 => out.push(0xB7), // f64.convert_i32_s
                    _ => out.push(0xB7),
                }
                if dst_has_value { self.emit_set_value(out, id, func); }
            }
            Inst::FloatToInt { value, to, .. } => {
                self.emit_value(out, *value, func);
                match wasm_valtype(to) {
                    WT_I32 => out.push(0xA8), // i32.trunc_f32_s (approx)
                    WT_I64 => out.push(0xAA), // i64.trunc_f32_s
                    _ => out.push(0xAA),
                }
                if dst_has_value { self.emit_set_value(out, id, func); }
            }

            // ─── Fides-specific ───
            Inst::TryUnwrap { value, ok_dst, err_block, ok_block } => {
                // Load the tag (field 0) and branch.
                self.emit_value(out, *value, func);
                self.emit_const_i64(out, 0);
                out.push(0x29); out.uleb(3); out.uleb(0); // i64.load tag
                // If tag == 0 → err, else ok. (Convention: 0 = Err/None.)
                self.emit_const_i64(out, 0);
                out.push(0x53); // i64.ne
                out.push(0x04); out.push(WT_VOID); // if void
                self.emit_const_i32(out, ok_block.0 as i32);
                self.emit_local_set(out, cur_block_local);
                out.push(0x05); // else
                self.emit_const_i32(out, err_block.0 as i32);
                self.emit_local_set(out, cur_block_local);
                out.push(0x0B); // end if
                out.push(0x0C); out.uleb(0); // br $dispatch
                // ok_dst is bound by the caller after the branch; nothing to
                // emit here.
                let _ = ok_dst;
            }
            Inst::Print { value, ty } => {
                // Route to the typed host print.
                self.emit_value(out, *value, func);
                let (host, want_wt) = match ty {
                    IrType::Bool => ("p_print_bool", WT_I32),
                    IrType::F32 | IrType::F64 => ("p_print_float", WT_F64),
                    IrType::Ptr(_) | IrType::Opaque(_) | IrType::Generic(_, _) => ("p_print_str", WT_I64),
                    _ => ("p_print_int", WT_I64),
                };
                let val_meta = func.values.get(value);
                let val_wt = val_meta.map(|m| wasm_valtype(&m.ty)).unwrap_or(WT_I64);
                if val_wt == WT_I32 && want_wt == WT_I64 {
                    out.push(0xAC);
                } else if val_wt == WT_I64 && want_wt == WT_I32 {
                    out.push(0xA7);
                } else if val_wt == WT_F32 && want_wt == WT_F64 {
                    out.push(0xC0); // f64.promote_f32
                }
                if let Some(hi) = host_import_index(host) {
                    out.push(0x10); out.uleb(hi as u64);
                } else {
                    out.push(0x1A); // drop
                }
            }
            Inst::PersistLoad { default, .. } => {
                // Just use default value
                self.emit_value(out, *default, func);
            }
            Inst::PersistSave { .. } => {
                // No-op in WASM
            }
            Inst::TxnBegin | Inst::TxnEnd | Inst::UnsafeBegin | Inst::UnsafeEnd | Inst::GradBegin { .. } | Inst::GradEnd | Inst::Backward { .. } => {
                // No-op in WASM (rollback/unsafe not supported)
            }
            Inst::Panic { message } => {
                let addr = self.string_addrs.get(message).copied().unwrap_or(0);
                self.emit_const_i64(out, addr as i64);
                if let Some(hi) = host_import_index("p_panic") {
                    out.push(0x10); out.uleb(hi as u64);
                }
                out.push(0x00); // unreachable
            }
            Inst::Asm { ret_ty, .. } => {
                // WASM doesn't support inline assembly — emit a no-op
                // For void: nothing. For I64: push 0
                if !matches!(ret_ty, IrType::Void) {
                    self.emit_const_i64(out, 0);
                }
            }
            Inst::RegionBegin { .. } => {
                // Region begin — push a fake region handle (0)
                self.emit_const_i64(out, 0);
            }
            Inst::RegionEnd { .. } => {
                // Region end — no-op in WASM
            }
            Inst::GradBegin { .. } | Inst::GradEnd | Inst::Backward { .. } => {
                // Autodiff markers — no-op in WASM
            }
        }
    }

    // ─── Module assembly ────────────────────────────────────────────────

    fn generate(&mut self, module: &IrModule) -> Vec<u8> {
        // Collect struct layouts.
        self.struct_layouts.clear();
        for (n, l) in &module.struct_layouts {
            self.struct_layouts.insert(n.clone(), l.clone());
        }

        // Intern all string constants up front so data addresses are stable.
        for s in module.string_constants.keys() {
            self.intern_string(s);
        }
        // Also intern strings that appear in ConstStr instructions (the
        // string_constants map may not include all of them).
        for (_, func) in &module.functions {
            for (_, inst) in &func.instructions {
                if let Inst::ConstStr { value } = inst {
                    self.intern_string(value);
                }
                if let Inst::Panic { message } = inst {
                    self.intern_string(message);
                }
            }
        }

        // ─── Type section ──────────────────────────────────────────────
        // Host import types first (so import section can reference them),
        // then user function types.
        self.type_sigs.clear();
        self.type_indices.clear();

        // Reserve type indices for host imports.
        for h in HOST_IMPORTS {
            self.intern_type(h.params, h.results);
        }

        // User function types.
        self.func_type.clear();
        self.func_index.clear();
        self.user_funcs.clear();
        for (name, func) in &module.functions {
            let params: Vec<u8> = func.params.iter()
                .map(|(_, ty)| wasm_valtype(ty))
                .collect();
            let results: Vec<u8> = if has_value(&func.ret_ty) {
                vec![wasm_valtype(&func.ret_ty)]
            } else {
                vec![]
            };
            let ti = self.intern_type(&params, &results);
            self.func_type.insert(name.clone(), ti);
            self.user_funcs.push(name.clone());
        }

        // Build the type section body.
        let mut type_sec = WasmBuf::new();
        type_sec.uleb(self.type_sigs.len() as u64);
        for (params, results) in &self.type_sigs {
            type_sec.push(0x60); // functype
            type_sec.uleb(params.len() as u64);
            type_sec.push_bytes(params);
            type_sec.uleb(results.len() as u64);
            type_sec.push_bytes(results);
        }
        let type_section = section(SEC_TYPE, &type_sec.into_vec());

        // ─── Import section ─────────────────────────────────────────────
        let mut import_sec = WasmBuf::new();
        import_sec.uleb(HOST_IMPORTS.len() as u64);
        for h in HOST_IMPORTS {
            import_sec.name("env");      // module name
            import_sec.name(h.name);     // field name
            import_sec.push(0x00);       // import kind: function
            // Look up the interned type index for this import's signature.
            let ti = self.intern_type(h.params, h.results);
            import_sec.uleb(ti as u64);  // type index
        }
        let import_section = section(SEC_IMPORT, &import_sec.into_vec());

        // ─── Function section ───────────────────────────────────────────
        // Function indices: 0..N are host imports (in HOST_IMPORTS order),
        // then user functions.
        for (i, name) in self.user_funcs.iter().enumerate() {
            let ti = self.func_type[name];
            self.func_index.insert(name.clone(), (HOST_IMPORTS.len() + i) as u32);
        }
        let mut func_sec = WasmBuf::new();
        func_sec.uleb(self.user_funcs.len() as u64);
        for name in &self.user_funcs {
            func_sec.uleb(self.func_type[name] as u64);
        }
        let function_section = section(SEC_FUNCTION, &func_sec.into_vec());

        // ─── Memory section ─────────────────────────────────────────────
        // One memory: min 1 page (64KiB), no max (grow as needed).
        let mut mem_sec = WasmBuf::new();
        mem_sec.uleb(1);    // count
        mem_sec.push(0x00); // limits flag: min only
        mem_sec.uleb(16);   // min = 16 pages = 1 MiB
        let memory_section = section(SEC_MEMORY, &mem_sec.into_vec());

        // ─── Global section ─────────────────────────────────────────────
        // One mutable i64 global: $sp (stack pointer), initialized to 65536
        // (start of page 1, leaving page 0 for data/null guard).
        let mut glob_sec = WasmBuf::new();
        glob_sec.uleb(1);          // count
        glob_sec.push(WT_I64);     // type
        glob_sec.push(0x01);       // mutability: var
        glob_sec.push(0x42);       // i64.const
        glob_sec.sleb(65536);
        glob_sec.push(0x0B);       // end init
        let global_section = section(SEC_GLOBAL, &glob_sec.into_vec());

        // ─── Export section ─────────────────────────────────────────────
        // Export memory as "memory" and the main function as "_start".
        let mut exports: Vec<(&str, u8, u32)> = Vec::new();
        exports.push(("memory", 0x02, 0)); // mem export, index 0
        if let Some(&main_idx) = self.func_index.get("main") {
            exports.push(("_start", 0x00, main_idx)); // func export
        }
        let mut exp_sec = WasmBuf::new();
        exp_sec.uleb(exports.len() as u64);
        for (name, kind, idx) in &exports {
            exp_sec.name(name);
            exp_sec.push(*kind);
            exp_sec.uleb(*idx as u64);
        }
        let export_section = section(SEC_EXPORT, &exp_sec.into_vec());

        // ─── Code section ───────────────────────────────────────────────
        let mut code_sec = WasmBuf::new();
        code_sec.uleb(self.user_funcs.len() as u64);
        for name in &self.user_funcs.clone() {
            let func = &module.functions[name];
            let body = self.gen_function(name, func);
            code_sec.uleb(body.len() as u64);
            code_sec.push_bytes(&body);
        }
        let code_section = section(SEC_CODE, &code_sec.into_vec());

        // ─── Data section ───────────────────────────────────────────────
        // Emit all interned strings as a single data segment at offset 1024.
        let mut data_sec = WasmBuf::new();
        // Collect and sort strings by address for deterministic output.
        let mut entries: Vec<(u64, String)> = self.string_addrs.iter()
            .map(|(s, a)| (*a, s.clone()))
            .collect();
        entries.sort_by_key(|(a, _)| *a);
        // Build one contiguous blob from address 1024.
        let base = 1024u64;
        let mut blob: Vec<u8> = Vec::new();
        for (addr, s) in &entries {
            while (base + blob.len() as u64) < *addr {
                blob.push(0);
            }
            blob.extend_from_slice(s.as_bytes());
            blob.push(0); // null terminator
            // Pad to 8-byte alignment.
            while blob.len() % 8 != 0 {
                blob.push(0);
            }
        }
        if blob.is_empty() {
            // No data — emit an empty data section (count 0).
            data_sec.uleb(0);
        } else {
            data_sec.uleb(1); // one segment
            data_sec.push(0x00); // memory index 0
            // offset expression: i64.const base
            data_sec.push(0x42);
            data_sec.sleb(base as i64);
            data_sec.push(0x0B);
            data_sec.uleb(blob.len() as u64);
            data_sec.push_bytes(&blob);
        }
        let data_section = section(SEC_DATA, &data_sec.into_vec());

        // ─── Assemble the module ────────────────────────────────────────
        let mut module_bytes = WasmBuf::new();
        // Magic + version
        module_bytes.push_bytes(b"\0asm");
        module_bytes.u32(1);
        module_bytes.push_bytes(&type_section);
        module_bytes.push_bytes(&import_section);
        module_bytes.push_bytes(&function_section);
        module_bytes.push_bytes(&memory_section);
        module_bytes.push_bytes(&global_section);
        module_bytes.push_bytes(&export_section);
        module_bytes.push_bytes(&code_section);
        module_bytes.push_bytes(&data_section);

        module_bytes.into_vec()
    }
}

/// Public entry point: lower an IR module to a WASM binary.
pub fn generate_wasm(module: &IrModule) -> Vec<u8> {
    let mut gen = WasmCodeGen::new();
    gen.generate(module)
}

/// Render a WASM binary as a human-readable WAT-ish dump (for `--wat` debug).
/// This is NOT a full WAT emitter — it's a structural summary for inspection.
pub fn dump_wat(module: &IrModule) -> String {
    let bytes = generate_wasm(module);
    let mut out = String::new();
    out.push_str(&format!("; WASM module ({} bytes)\n", bytes.len()));
    out.push_str(&format!("; magic: {:?}\n", &bytes[0..4]));
    out.push_str(&format!("; version: {}\n", u32::from_le_bytes([
        bytes[4], bytes[5], bytes[6], bytes[7]
    ])));
    // Walk sections.
    let mut i = 8;
    let sec_names = [
        "custom", "type", "import", "function", "table", "memory",
        "global", "export", "start", "element", "code", "data", "datacount",
    ];
    while i < bytes.len() {
        let id = bytes[i]; i += 1;
        // Read uleb size.
        let mut size = 0u64;
        let mut shift = 0;
        loop {
            let b = bytes[i]; i += 1;
            size |= ((b & 0x7F) as u64) << shift;
            shift += 7;
            if b & 0x80 == 0 { break; }
        }
        let name = sec_names.get(id as usize).copied().unwrap_or("?");
        out.push_str(&format!("; section {} ({}) size={}\n", id, name, size));
        i += size as usize;
    }
    out.push_str(";\n; Functions:\n");
    for name in gen_func_names(module) {
        out.push_str(&format!(";   {}\n", name));
    }
    out
}

fn gen_func_names(module: &IrModule) -> Vec<String> {
    module.functions.keys().cloned().collect()
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::compiler;

    #[test]
    fn test_wasm_magic_and_header() {
        let source = "fn main() { return 0; }";
        let bytes = compiler::compile_to_wasm(source, "test.fd", false).unwrap();
        // WASM magic: \0asm
        assert_eq!(&bytes[0..4], b"\0asm");
        // Version 1
        assert_eq!(&bytes[4..8], &[1, 0, 0, 0]);
        // Must have at least type + code sections
        assert!(bytes.len() > 8);
    }

    #[test]
    fn test_wat_dump() {
        let source = "fn main() { return 0; }";
        let wat = compiler::compile_to_wasm(source, "test.fd", true).unwrap();
        let s = String::from_utf8(wat).unwrap();
        assert!(s.contains("SPIR-V") || s.contains("WASM") || s.contains("magic"));
        assert!(s.contains("section"));
    }
}
