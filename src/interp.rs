// Pledge IR Interpreter
//
// Executes Pledge IR directly without generating machine code.
// This is used for:
// - comptime: running code at compile time
// - Debugging: stepping through IR to verify correctness
// - Hot reload: interpreting until the recompiled function is ready
// - Testing: verifying IR semantics without a backend
//
// The interpreter is deliberately simple — it's not meant to be fast.
// It walks basic blocks, evaluates instructions, and maintains a
// value table. When we hit a Call to a runtime function, it prints
// to stdout (for print functions) or panics (for panic).

use crate::ir::*;
use crate::ast::BinOp;
use std::collections::HashMap;
use std::cell::RefCell;
use std::rc::Rc;
use smallvec::SmallVec;
use compact_str::CompactString;

/// A runtime value in the interpreter
#[derive(Debug, Clone, PartialEq)]
pub enum Value {
    Int(i64),
    Float(f64),
    Bool(bool),
    Str(CompactString),
    Ptr(u64),         // Simulated pointer — index into memory
    Void,
    /// Dynamic array — SmallVec inline-optimizes up to 8 elements on the stack
    Vec(Rc<RefCell<SmallVec<[Value; 8]>>>),
    /// Hash map — backed by hashbrown's SwissTable implementation
    Map(Rc<RefCell<hashbrown::HashMap<String, Value>>>),
}

impl std::fmt::Display for Value {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Value::Int(i) => write!(f, "{}", i),
            Value::Float(fl) => write!(f, "{}", fl),
            Value::Bool(b) => write!(f, "{}", b),
            Value::Str(s) => write!(f, "{}", s),
            Value::Ptr(p) => write!(f, "ptr(0x{:x})", p),
            Value::Void => write!(f, "void"),
            Value::Vec(_) => write!(f, "[...]"),
            Value::Map(_) => write!(f, "{{...}}"),
        }
    }
}

/// Interpreter memory — simple byte-addressed array
pub struct Memory {
    data: Vec<u8>,
    /// Next allocation address
    next_alloc: u64,
}

impl Memory {
    pub fn new() -> Self {
        Memory {
            data: vec![0u8; 1024 * 1024],  // 1MB simulated memory
            next_alloc: 0x1000,             // Start allocations at 0x1000
        }
    }

    pub fn alloc(&mut self, size: u64) -> u64 {
        let addr = self.next_alloc;
        self.next_alloc += size;
        if self.next_alloc as usize > self.data.len() {
            self.data.resize(self.next_alloc as usize + 1024, 0);
        }
        addr
    }

    pub fn store(&mut self, addr: u64, bytes: &[u8]) {
        let start = addr as usize;
        let end = start + bytes.len();
        if end > self.data.len() {
            self.data.resize(end + 1024, 0);
        }
        self.data[start..end].copy_from_slice(bytes);
    }

    pub fn load(&self, addr: u64, size: usize) -> Vec<u8> {
        let start = addr as usize;
        let end = start + size;
        if end <= self.data.len() {
            self.data[start..end].to_vec()
        } else {
            vec![0u8; size]
        }
    }

    pub fn store_i64(&mut self, addr: u64, val: i64) {
        self.store(addr, &val.to_le_bytes());
    }

    pub fn load_i64(&self, addr: u64) -> i64 {
        let bytes = self.load(addr, 8);
        i64::from_le_bytes(bytes[..8].try_into().unwrap_or([0u8; 8]))
    }

    pub fn store_f64(&mut self, addr: u64, val: f64) {
        self.store(addr, &val.to_le_bytes());
    }

    pub fn load_f64(&self, addr: u64) -> f64 {
        let bytes = self.load(addr, 8);
        f64::from_le_bytes(bytes[..8].try_into().unwrap_or([0u8; 8]))
    }

    pub fn store_bool(&mut self, addr: u64, val: bool) {
        self.store(addr, &[if val { 1 } else { 0 }]);
    }

    pub fn load_bool(&self, addr: u64) -> bool {
        let bytes = self.load(addr, 1);
        bytes[0] != 0
    }

    pub fn store_i32(&mut self, addr: u64, val: i32) {
        self.store(addr, &val.to_le_bytes());
    }

    pub fn load_i32(&self, addr: u64) -> i32 {
        let bytes = self.load(addr, 4);
        i32::from_le_bytes(bytes[..4].try_into().unwrap_or([0u8; 4]))
    }

    pub fn store_f32(&mut self, addr: u64, val: f32) {
        self.store(addr, &val.to_le_bytes());
    }

    pub fn load_f32(&self, addr: u64) -> f32 {
        let bytes = self.load(addr, 4);
        f32::from_le_bytes(bytes[..4].try_into().unwrap_or([0u8; 4]))
    }
}

/// The IR interpreter
pub struct Interpreter {
    /// The IR module to execute
    module: IrModule,
    /// Simulated memory
    memory: Memory,
    /// String constant addresses — maps string content to memory address
    string_addrs: HashMap<String, u64>,
    /// Opaque values (Vec, HashMap) stored by alloca address —
    /// these can't be serialized to the byte-addressed memory
    opaque_values: HashMap<u64, Value>,
    /// Verbose mode — print each instruction as it executes
    verbose: bool,
}

/// Result of running a function
#[derive(Debug)]
pub enum RunResult {
    /// Function returned normally with a value
    Return(Value),
    /// Function panicked
    Panic(String),
}

impl Interpreter {
    pub fn new(module: IrModule) -> Self {
        let mut interp = Interpreter {
            module,
            memory: Memory::new(),
            string_addrs: HashMap::new(),
            opaque_values: HashMap::new(),
            verbose: false,
        };
        // Pre-allocate string constants
        interp.init_strings();
        interp
    }

    pub fn verbose(mut self) -> Self {
        self.verbose = true;
        self
    }

    /// Initialize string constants in memory
    fn init_strings(&mut self) {
        // Collect strings first to avoid borrow issues
        let strings: Vec<(String, String)> = self.module.string_constants
            .iter()
            .map(|(s, label)| (s.clone(), label.clone()))
            .collect();

        for (s, _label) in &strings {
            let addr = self.memory.alloc(s.len() as u64 + 1); // +1 for null terminator
            let mut bytes = s.as_bytes().to_vec();
            bytes.push(0); // null terminator
            self.memory.store(addr, &bytes);
            self.string_addrs.insert(s.clone(), addr);
        }
    }

    /// Run the program — starts from `main` function
    pub fn run(&mut self) -> RunResult {
        if let Some(main) = self.module.functions.get("main").cloned() {
            self.run_function(&main, &[])
        } else {
            RunResult::Panic("No main function found".to_string())
        }
    }

    /// Run a specific function with arguments
    fn run_function(&mut self, func: &IrFunction, args: &[Value]) -> RunResult {
        // Build value table for this function
        let mut values: HashMap<ValueId, Value> = HashMap::new();

        // Bind parameters
        for (i, (param_id, _)) in func.params.iter().enumerate() {
            let val = args.get(i).cloned().unwrap_or(Value::Void);
            values.insert(*param_id, val);
        }

        // Start at entry block
        let mut current_block = func.entry;

        loop {
            let block = &func.blocks[current_block.0 as usize];

            if self.verbose {
                eprintln!("  → {}", current_block);
            }

            // Execute instructions in this block
            let mut next_block: Option<BlockId> = None;

            for &inst_id in &block.instructions {
                let inst = match func.instructions.get(&inst_id) {
                    Some(i) => i.clone(),
                    None => continue,
                };

                if self.verbose {
                    eprintln!("    {} = {:?}", inst_id, inst);
                }

                match self.execute_inst(&inst, inst_id, &mut values, func) {
                    ExecOutcome::Continue => {}
                    ExecOutcome::Value(v) => {
                        values.insert(inst_id, v);
                    }
                    ExecOutcome::Jump(block_id) => {
                        next_block = Some(block_id);
                        break;
                    }
                    ExecOutcome::Return(val) => {
                        return RunResult::Return(val);
                    }
                    ExecOutcome::Panic(msg) => {
                        return RunResult::Panic(msg);
                    }
                }
            }

            match next_block {
                Some(block_id) => {
                    current_block = block_id;
                }
                None => {
                    // No terminator found — return void
                    return RunResult::Return(Value::Void);
                }
            }
        }
    }

    /// Execute a single instruction
    fn execute_inst(
        &mut self,
        inst: &Inst,
        inst_id: ValueId,
        values: &mut HashMap<ValueId, Value>,
        func: &IrFunction,
    ) -> ExecOutcome {
        match inst {
            // ─── Constants ───
            Inst::ConstInt { value, ty } => {
                let v = match ty {
                    IrType::Bool => Value::Bool(*value != 0),
                    _ => Value::Int(*value),
                };
                // Store in the value that was just created
                // The caller already has the ValueId, so we need a different approach
                // Actually, we return the value and the caller stores it
                // Hmm, this design needs adjustment. Let me store it differently.
                // The inst_id is known by the caller, so let's use a side channel.
                // For now, we'll handle this by having execute_inst return a value
                // and the caller stores it.
                // But we need the ValueId... Let me restructure.
                // Actually, the simplest approach: we don't need to store values
                // for constants — we can recompute them. But that's wasteful.
                // Let me just store values as we go.
                // The problem is we don't have the ValueId here.
                // Fix: pass the ValueId to execute_inst.
                // For now, let's use a different approach: return the value.
                ExecOutcome::Value(v)
            }

            Inst::ConstFloat { bits, ty } => {
                let f = if *ty == IrType::F32 {
                    f32::from_bits(*bits as u32) as f64
                } else {
                    f64::from_bits(*bits)
                };
                ExecOutcome::Value(Value::Float(f))
            }

            Inst::ConstBool { value } => {
                ExecOutcome::Value(Value::Bool(*value))
            }

            Inst::ConstStr { value } => {
                // Return the string directly (or its memory address)
                ExecOutcome::Value(Value::Str(CompactString::from(value.as_str())))
            }

            Inst::ConstNull { .. } => {
                ExecOutcome::Value(Value::Ptr(0))
            }

            // ─── Arithmetic ───
            Inst::BinOp { op, left, right, .. } => {
                let l = values.get(left).cloned().unwrap_or(Value::Void);
                let r = values.get(right).cloned().unwrap_or(Value::Void);
                let result = self.eval_binop(op, &l, &r);
                ExecOutcome::Value(result)
            }

            Inst::UnaryOp { op, operand, .. } => {
                let v = values.get(operand).cloned().unwrap_or(Value::Void);
                let result = self.eval_unaryop(op, &v);
                ExecOutcome::Value(result)
            }

            // ─── Memory ───
            Inst::Alloca { ty, .. } => {
                let size = self.module.type_size(ty);
                let addr = self.memory.alloc(size.max(1));
                ExecOutcome::Value(Value::Ptr(addr))
            }

            Inst::Load { ptr, ty } => {
                let addr = match values.get(ptr) {
                    Some(Value::Ptr(a)) => *a,
                    _ => 0,
                };
                let v = match ty {
                    IrType::I8 | IrType::U8 => {
                        let bytes = self.memory.load(addr, 1);
                        Value::Int(if *ty == IrType::I8 { i8::from_le_bytes([bytes[0]]) as i64 } else { bytes[0] as i64 })
                    }
                    IrType::I16 | IrType::U16 => {
                        let bytes = self.memory.load(addr, 2);
                        Value::Int(if *ty == IrType::I16 { i16::from_le_bytes(bytes[..2].try_into().unwrap_or([0u8; 2])) as i64 } else { u16::from_le_bytes(bytes[..2].try_into().unwrap_or([0u8; 2])) as i64 })
                    }
                    IrType::I32 | IrType::U32 => {
                        Value::Int(self.memory.load_i32(addr) as i64)
                    }
                    IrType::I64 | IrType::U64 => {
                        Value::Int(self.memory.load_i64(addr))
                    }
                    IrType::F32 => {
                        Value::Float(self.memory.load_f32(addr) as f64)
                    }
                    IrType::F64 => {
                        Value::Float(self.memory.load_f64(addr))
                    }
                    IrType::Bool => {
                        Value::Bool(self.memory.load_bool(addr))
                    }
                    IrType::Ptr(_) => {
                        Value::Ptr(self.memory.load_i64(addr) as u64)
                    }
                    IrType::Opaque(_) | IrType::Generic(_, _) => {
                        // Check opaque side-table first (Vec, HashMap, String)
                        if let Some(v) = self.opaque_values.get(&addr) {
                            v.clone()
                        } else {
                            // Fall through: load as pointer from byte memory
                            // (handles closure env stored as pointer)
                            Value::Ptr(self.memory.load_i64(addr) as u64)
                        }
                    }
                    _ => Value::Ptr(addr),
                };
                ExecOutcome::Value(v)
            }

            Inst::Store { ptr, value } => {
                let addr = match values.get(ptr) {
                    Some(Value::Ptr(a)) => *a,
                    _ => 0,
                };
                // Check if this is an opaque-typed alloca — if so, always use side-table
                let ptr_ty = func.values.get(ptr).map(|m| &m.ty);
                let is_opaque = match ptr_ty {
                    Some(IrType::Opaque(_)) | Some(IrType::Generic(_, _)) => true,
                    Some(IrType::Ptr(inner)) => matches!(inner.as_ref(), IrType::Opaque(_) | IrType::Generic(_, _)),
                    _ => false,
                };
                // Get the value's type to store the correct number of bytes
                let val_ty = func.values.get(value).map(|m| m.ty.clone());
                if is_opaque {
                    // For opaque-typed slots, always store in side-table
                    if let Some(v) = values.get(value) {
                        self.opaque_values.insert(addr, v.clone());
                        // Also store primitives in byte memory for fallback
                        match v {
                            Value::Int(i) => self.memory.store_i64(addr, *i),
                            Value::Float(f) => self.memory.store_f64(addr, *f),
                            Value::Bool(b) => self.memory.store_bool(addr, *b),
                            Value::Ptr(p) => self.memory.store_i64(addr, *p as i64),
                            Value::Str(s) => {
                                let str_addr = self.memory.alloc(s.len() as u64 + 1);
                                self.memory.store(str_addr, s.as_bytes());
                                self.memory.store(str_addr + s.len() as u64, &[0]);
                                self.memory.store_i64(addr, str_addr as i64);
                            }
                            _ => {}
                        }
                    }
                    return ExecOutcome::Continue;
                }
                match values.get(value) {
                    Some(Value::Int(v)) => {
                        match val_ty {
                            Some(IrType::I8) | Some(IrType::U8) => self.memory.store(addr, &(*v as i8).to_le_bytes()),
                            Some(IrType::I16) | Some(IrType::U16) => self.memory.store(addr, &(*v as i16).to_le_bytes()),
                            Some(IrType::I32) | Some(IrType::U32) => self.memory.store_i32(addr, *v as i32),
                            _ => self.memory.store_i64(addr, *v),
                        }
                    }
                    Some(Value::Float(v)) => {
                        match val_ty {
                            Some(IrType::F32) => self.memory.store_f32(addr, *v as f32),
                            _ => self.memory.store_f64(addr, *v),
                        }
                    }
                    Some(Value::Bool(v)) => self.memory.store_bool(addr, *v),
                    Some(Value::Ptr(v)) => self.memory.store_i64(addr, *v as i64),
                    Some(Value::Str(s)) => {
                        // Store in byte memory as pointer (for closure env compat)
                        let str_addr = self.memory.alloc(s.len() as u64 + 1);
                        self.memory.store(str_addr, s.as_bytes());
                        self.memory.store(str_addr + s.len() as u64, &[0]);
                        self.memory.store_i64(addr, str_addr as i64);
                        // Also store in opaque side-table (for String type load)
                        if let Some(v) = values.get(value) {
                            self.opaque_values.insert(addr, v.clone());
                        }
                    }
                    Some(Value::Vec(_)) | Some(Value::Map(_)) => {
                        // Opaque values can't be serialized to bytes — store in side-table
                        if let Some(v) = values.get(value) {
                            self.opaque_values.insert(addr, v.clone());
                        }
                    }
                    _ => {}
                }
                ExecOutcome::Continue
            }

            Inst::GetFieldPtr { struct_ptr, field_index, field_ty, .. } => {
                let base = match values.get(struct_ptr) {
                    Some(Value::Ptr(a)) => *a,
                    _ => 0,
                };
                // Look up real field offset from struct layout
                let struct_name = func.values.get(struct_ptr).and_then(|m| {
                    match &m.ty {
                        IrType::Ptr(inner) => match inner.as_ref() {
                            IrType::Struct(name) => Some(name.as_str()),
                            _ => None,
                        },
                        IrType::Struct(name) => Some(name.as_str()),
                        _ => None,
                    }
                });
                let offset = if let Some(name) = struct_name {
                    if let Some(layout) = self.module.get_struct(name) {
                        if let Some((_, _, off)) = layout.fields.get(*field_index as usize) {
                            *off as u64
                        } else {
                            *field_index as u64 * 8
                        }
                    } else {
                        *field_index as u64 * 8
                    }
                } else {
                    *field_index as u64 * 8
                };
                let _ = field_ty;
                ExecOutcome::Value(Value::Ptr(base + offset))
            }

            Inst::GetElemPtr { array_ptr, index, elem_ty, .. } => {
                let base = match values.get(array_ptr) {
                    Some(Value::Ptr(a)) => *a,
                    _ => 0,
                };
                let idx = match values.get(index) {
                    Some(Value::Int(i)) => *i as u64,
                    _ => 0,
                };
                let elem_size = self.module.type_size(elem_ty).max(1);
                ExecOutcome::Value(Value::Ptr(base + idx * elem_size))
            }

            // ─── Control Flow ───
            Inst::Jump { target } => {
                ExecOutcome::Jump(*target)
            }

            Inst::Branch { cond, then, else_ } => {
                let c = match values.get(cond) {
                    Some(Value::Bool(b)) => *b,
                    Some(Value::Int(i)) => *i != 0,
                    _ => false,
                };
                if c {
                    ExecOutcome::Jump(*then)
                } else {
                    ExecOutcome::Jump(*else_)
                }
            }

            Inst::Return { value } => {
                let v = match value {
                    Some(id) => values.get(id).cloned().unwrap_or(Value::Void),
                    None => Value::Void,
                };
                ExecOutcome::Return(v)
            }

            Inst::Unreachable => {
                let line = func.values.get(&inst_id).and_then(|m| m.source_line);
                ExecOutcome::Panic(format!(
                    "Reached unreachable instruction{}",
                    line.map(|l| format!(" at line {}", l)).unwrap_or_default()
                ))
            }

            // ─── Function Calls ───
            Inst::Call { func: name, args, .. } => {
                let arg_vals: Vec<Value> = args.iter()
                    .map(|a| values.get(a).cloned().unwrap_or(Value::Void))
                    .collect();

                // Check if it's a runtime function
                if self.is_runtime_func(name) {
                    return self.call_runtime(name, &arg_vals);
                }

                // Check if it's a user function
                if let Some(callee) = self.module.functions.get(name).cloned() {
                    // Run the callee
                    return match self.run_function(&callee, &arg_vals) {
                        RunResult::Return(v) => ExecOutcome::Value(v),
                        RunResult::Panic(msg) => ExecOutcome::Panic(msg),
                    };
                }

                // Unknown function
                ExecOutcome::Panic(format!(
                    "Unknown function '{}'{}",
                    name,
                    func.values.get(&inst_id)
                        .and_then(|m| m.source_line)
                        .map(|l| format!(" at line {}", l))
                        .unwrap_or_default()
                ))
            }

            Inst::CallPtr { fn_ptr, args, .. } => {
                let arg_vals: Vec<Value> = args.iter()
                    .map(|a| values.get(a).cloned().unwrap_or(Value::Void))
                    .collect();

                // All function pointers are env structs: field 0 = fn addr string, fields 1..N = captures
                let (fn_name, env_ptr) = match values.get(fn_ptr) {
                    Some(Value::Str(s)) => {
                        // Legacy: plain function name string (shouldn't happen with new lowering)
                        (s.to_string(), None)
                    }
                    Some(Value::Ptr(addr)) => {
                        // Env struct — read fn name from field 0 (stored as string pointer)
                        let fn_ptr_bytes = self.memory.load(*addr, 8);
                        let fn_ptr_addr = i64::from_le_bytes(fn_ptr_bytes.try_into().unwrap_or([0; 8])) as u64;
                        // Read function name string from that pointer
                        let mut name_bytes = Vec::new();
                        let mut name_offset = fn_ptr_addr;
                        loop {
                            let b = self.memory.load(name_offset, 1);
                            if b[0] == 0 { break; }
                            name_bytes.push(b[0]);
                            name_offset += 1;
                        }
                        let name = String::from_utf8_lossy(&name_bytes).to_string();
                        (name, Some(*addr))
                    }
                    _ => return ExecOutcome::Panic("CallPtr: function pointer is not a string or pointer".to_string()),
                };

                // Check if it's a runtime function
                if self.is_runtime_func(&fn_name) {
                    return self.call_runtime(&fn_name, &arg_vals);
                }

                // Check if it's a user function
                if let Some(callee) = self.module.functions.get(&fn_name).cloned() {
                    let mut all_args = arg_vals;
                    // Only prepend env_ptr if function expects it (has more params than call args)
                    if callee.params.len() > all_args.len() {
                        if let Some(env) = env_ptr {
                            all_args.insert(0, Value::Ptr(env));
                        } else {
                            all_args.insert(0, Value::Int(0));
                        }
                    }
                    return match self.run_function(&callee, &all_args) {
                        RunResult::Return(v) => ExecOutcome::Value(v),
                        RunResult::Panic(msg) => ExecOutcome::Panic(msg),
                    };
                }

                ExecOutcome::Panic(format!(
                    "Unknown function in CallPtr: '{}'{}",
                    fn_name,
                    func.values.get(&inst_id)
                        .and_then(|m| m.source_line)
                        .map(|l| format!(" at line {}", l))
                        .unwrap_or_default()
                ))
            }

            // ─── Conversions ───
            Inst::BitCast { value, to, .. } => {
                let v = values.get(value).cloned().unwrap_or(Value::Void);
                ExecOutcome::Value(self.cast_value(&v, to))
            }

            Inst::IntExtend { value, to, .. } => {
                let v = values.get(value).cloned().unwrap_or(Value::Int(0));
                let i = match v { Value::Int(i) => i, _ => 0 };
                ExecOutcome::Value(Value::Int(i))
            }

            Inst::IntTrunc { value, to, .. } => {
                let v = values.get(value).cloned().unwrap_or(Value::Int(0));
                let i = match v { Value::Int(i) => i, _ => 0 };
                let truncated = match to {
                    IrType::I8 => i as i8 as i64,
                    IrType::I16 => i as i16 as i64,
                    IrType::I32 => i as i32 as i64,
                    IrType::U8 => i as u8 as i64,
                    IrType::U16 => i as u16 as i64,
                    IrType::U32 => i as u32 as i64,
                    _ => i,
                };
                ExecOutcome::Value(Value::Int(truncated))
            }

            Inst::IntToFloat { value, .. } => {
                let v = values.get(value).cloned().unwrap_or(Value::Int(0));
                let i = match v { Value::Int(i) => i, _ => 0 };
                ExecOutcome::Value(Value::Float(i as f64))
            }

            Inst::FloatToInt { value, .. } => {
                let v = values.get(value).cloned().unwrap_or(Value::Float(0.0));
                let f = match v { Value::Float(f) => f, _ => 0.0 };
                ExecOutcome::Value(Value::Int(f as i64))
            }

            // ─── Pledge-Specific ───
            Inst::TryUnwrap { value, ok_block, err_block: _, .. } => {
                // Simplified — just continue to ok_block
                let _ = values.get(value);
                ExecOutcome::Jump(*ok_block)
            }

            Inst::Print { value, ty } => {
                let v = values.get(value).cloned().unwrap_or(Value::Void);
                match &v {
                    Value::Int(i) => print!("{}", i),
                    Value::Float(f) => print!("{}", f),
                    Value::Bool(b) => print!("{}", b),
                    Value::Str(s) => print!("{}", s),
                    _ => print!("{:?}", v),
                }
                let _ = ty;
                ExecOutcome::Continue
            }

            Inst::Panic { message } => {
                ExecOutcome::Panic(message.clone())
            }
        }
    }

    /// Evaluate a binary operation
    fn eval_binop(&self, op: &BinOp, l: &Value, r: &Value) -> Value {
        match (l, r) {
            (Value::Int(a), Value::Int(b)) => {
                match op {
                    BinOp::Add => Value::Int(a.wrapping_add(*b)),
                    BinOp::Sub => Value::Int(a.wrapping_sub(*b)),
                    BinOp::Mul => Value::Int(a.wrapping_mul(*b)),
                    BinOp::Div => {
                        if *b == 0 { Value::Int(0) }
                        else { Value::Int(a.wrapping_div(*b)) }
                    }
                    BinOp::Mod => {
                        if *b == 0 { Value::Int(0) }
                        else { Value::Int(a.wrapping_rem(*b)) }
                    }
                    BinOp::Eq => Value::Bool(a == b),
                    BinOp::Neq => Value::Bool(a != b),
                    BinOp::Lt => Value::Bool(a < b),
                    BinOp::Gt => Value::Bool(a > b),
                    BinOp::LtEq => Value::Bool(a <= b),
                    BinOp::GtEq => Value::Bool(a >= b),
                    BinOp::And => Value::Bool(*a != 0 && *b != 0),
                    BinOp::Or => Value::Bool(*a != 0 || *b != 0),
                    BinOp::BitAnd => Value::Int(a & b),
                    BinOp::BitOr => Value::Int(a | b),
                    BinOp::BitXor => Value::Int(a ^ b),
                    BinOp::Shl => Value::Int(a.wrapping_shl(*b as u32)),
                    BinOp::Shr => Value::Int(a.wrapping_shr(*b as u32)),
                }
            }
            (Value::Float(a), Value::Float(b)) => {
                match op {
                    BinOp::Add => Value::Float(a + b),
                    BinOp::Sub => Value::Float(a - b),
                    BinOp::Mul => Value::Float(a * b),
                    BinOp::Div => Value::Float(a / b),
                    BinOp::Mod => Value::Float(a % b),
                    BinOp::Eq => Value::Bool(a == b),
                    BinOp::Neq => Value::Bool(a != b),
                    BinOp::Lt => Value::Bool(a < b),
                    BinOp::Gt => Value::Bool(a > b),
                    BinOp::LtEq => Value::Bool(a <= b),
                    BinOp::GtEq => Value::Bool(a >= b),
                    _ => Value::Float(0.0),
                }
            }
            (Value::Bool(a), Value::Bool(b)) => {
                match op {
                    BinOp::Eq => Value::Bool(a == b),
                    BinOp::Neq => Value::Bool(a != b),
                    BinOp::And => Value::Bool(*a && *b),
                    BinOp::Or => Value::Bool(*a || *b),
                    _ => Value::Bool(false),
                }
            }
            (Value::Str(a), Value::Str(b)) => {
                match op {
                    BinOp::Eq => Value::Bool(a == b),
                    BinOp::Neq => Value::Bool(a != b),
                    _ => Value::Void,
                }
            }
            (Value::Ptr(addr), Value::Str(s)) => {
                // Read null-terminated string from memory at addr
                let mut bytes = Vec::new();
                let mut offset = *addr;
                loop {
                    let b = self.memory.load(offset, 1);
                    if b[0] == 0 { break; }
                    bytes.push(b[0]);
                    offset += 1;
                }
                let ptr_str = String::from_utf8_lossy(&bytes);
                match op {
                    BinOp::Eq => Value::Bool(&*ptr_str == &**s),
                    BinOp::Neq => Value::Bool(&*ptr_str != &**s),
                    _ => Value::Void,
                }
            }
            (Value::Str(s), Value::Ptr(addr)) => {
                let mut bytes = Vec::new();
                let mut offset = *addr;
                loop {
                    let b = self.memory.load(offset, 1);
                    if b[0] == 0 { break; }
                    bytes.push(b[0]);
                    offset += 1;
                }
                let ptr_str = String::from_utf8_lossy(&bytes);
                match op {
                    BinOp::Eq => Value::Bool(&**s == &*ptr_str),
                    BinOp::Neq => Value::Bool(&**s != &*ptr_str),
                    _ => Value::Void,
                }
            }
            _ => Value::Void,
        }
    }

    /// Evaluate a unary operation
    fn eval_unaryop(&self, op: &crate::ast::UnaryOp, v: &Value) -> Value {
        match v {
            Value::Int(i) => {
                match op {
                    crate::ast::UnaryOp::Neg => Value::Int(-i),
                    crate::ast::UnaryOp::Not => Value::Bool(*i == 0),
                    crate::ast::UnaryOp::BitNot => Value::Int(!i),
                }
            }
            Value::Float(f) => {
                match op {
                    crate::ast::UnaryOp::Neg => Value::Float(-f),
                    _ => Value::Float(0.0),
                }
            }
            Value::Bool(b) => {
                match op {
                    crate::ast::UnaryOp::Not => Value::Bool(!b),
                    _ => Value::Bool(false),
                }
            }
            _ => Value::Void,
        }
    }

    /// Cast a value to a different type
    fn cast_value(&self, v: &Value, to: &IrType) -> Value {
        match (v, to) {
            (Value::Int(i), IrType::F32) => Value::Float(*i as f64),
            (Value::Int(i), IrType::F64) => Value::Float(*i as f64),
            (Value::Float(f), IrType::I64) => Value::Int(*f as i64),
            (Value::Float(f), IrType::I32) => Value::Int(*f as i32 as i64),
            (Value::Bool(b), IrType::I64) => Value::Int(if *b { 1 } else { 0 }),
            _ => v.clone(),
        }
    }

    /// Check if a function name is a runtime function
    fn is_runtime_func(&self, name: &str) -> bool {
        self.module.extern_functions.contains_key(name)
            || name == "print"
            || name.starts_with("p_vec_")
            || name.starts_with("p_map_")
            || name.starts_with("p_string_")
    }

    /// Call a runtime function
    fn call_runtime(&mut self, name: &str, args: &[Value]) -> ExecOutcome {
        match name {
            "p_print_int" => {
                if let Some(Value::Int(i)) = args.first() {
                    print!("{}", i);
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_print_float" => {
                if let Some(Value::Float(f)) = args.first() {
                    print!("{}", f);
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_print_bool" => {
                if let Some(Value::Bool(b)) = args.first() {
                    print!("{}", b);
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_print_str" => {
                if let Some(v) = args.first() {
                    match v {
                        Value::Str(s) => print!("{}", s),
                        Value::Ptr(addr) => {
                            // Load string from memory
                            let mut bytes = Vec::new();
                            let mut offset = *addr;
                            loop {
                                let b = self.memory.load(offset, 1);
                                if b[0] == 0 { break; }
                                bytes.push(b[0]);
                                offset += 1;
                            }
                            print!("{}", String::from_utf8_lossy(&bytes));
                        }
                        _ => print!("{:?}", v),
                    }
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_print_newline" => {
                println!();
                ExecOutcome::Value(Value::Void)
            }
            "p_panic" => {
                let msg = if let Some(Value::Str(s)) = args.first() {
                    s.to_string()
                } else {
                    "panic".to_string()
                };
                ExecOutcome::Panic(msg)
            }
            "p_alloc" => {
                let size = if let Some(Value::Int(s)) = args.first() {
                    *s as u64
                } else {
                    0
                };
                let addr = self.memory.alloc(size);
                ExecOutcome::Value(Value::Ptr(addr))
            }
            "p_malloc" => {
                let size = if let Some(Value::Int(s)) = args.first() {
                    *s as u64
                } else {
                    0
                };
                let addr = self.memory.alloc(size);
                ExecOutcome::Value(Value::Ptr(addr))
            }
            "p_free" => {
                // Interpreter doesn't actually free — memory is arena-based
                ExecOutcome::Value(Value::Void)
            }
            "p_str_concat" => {
                let left = match args.first() {
                    Some(Value::Str(s)) => s.to_string(),
                    Some(Value::Ptr(addr)) => {
                        let mut bytes = Vec::new();
                        let mut offset = *addr;
                        loop {
                            let b = self.memory.load(offset, 1);
                            if b[0] == 0 { break; }
                            bytes.push(b[0]);
                            offset += 1;
                        }
                        String::from_utf8_lossy(&bytes).to_string()
                    }
                    _ => String::new(),
                };
                let right = match args.get(1) {
                    Some(Value::Str(s)) => s.to_string(),
                    Some(Value::Ptr(addr)) => {
                        let mut bytes = Vec::new();
                        let mut offset = *addr;
                        loop {
                            let b = self.memory.load(offset, 1);
                            if b[0] == 0 { break; }
                            bytes.push(b[0]);
                            offset += 1;
                        }
                        String::from_utf8_lossy(&bytes).to_string()
                    }
                    _ => String::new(),
                };
                ExecOutcome::Value(Value::Str(CompactString::from(format!("{}{}", left, right))))
            }
            "p_to_str" => {
                let s = match args.first() {
                    Some(Value::Int(i)) => i.to_string(),
                    Some(Value::Float(f)) => f.to_string(),
                    Some(Value::Bool(b)) => b.to_string(),
                    Some(Value::Str(s)) => s.to_string(),
                    Some(Value::Vec(arr)) => {
                        let borrow = arr.borrow();
                        let mut s = String::from("[");
                        for (i, v) in borrow.iter().enumerate() {
                            if i > 0 { s.push_str(", "); }
                            match v {
                                Value::Str(inner) => { s.push('"'); s.push_str(inner); s.push('"'); }
                                _ => s.push_str(&format!("{}", v)),
                            }
                        }
                        s.push(']');
                        s
                    }
                    Some(Value::Map(m)) => {
                        let borrow = m.borrow();
                        // Sort keys for deterministic output
                        let mut entries: Vec<_> = borrow.iter().collect();
                        entries.sort_by(|a, b| a.0.cmp(b.0));
                        let mut s = String::from("{");
                        for (i, (k, v)) in entries.iter().enumerate() {
                            if i > 0 { s.push_str(", "); }
                            s.push_str(k);
                            s.push_str(": ");
                            s.push_str(&format!("{}", v));
                        }
                        s.push('}');
                        s
                    }
                    _ => String::new(),
                };
                ExecOutcome::Value(Value::Str(CompactString::from(s)))
            }
            // User-defined print function — map to runtime
            "print" => {
                if let Some(v) = args.first() {
                    match v {
                        Value::Int(i) => print!("{}", i),
                        Value::Float(f) => print!("{}", f),
                        Value::Bool(b) => print!("{}", b),
                        Value::Str(s) => print!("{}", s),
                        Value::Vec(arr) => {
                            let borrow = arr.borrow();
                            print!("[");
                            for (i, v) in borrow.iter().enumerate() {
                                if i > 0 { print!(", "); }
                                match v {
                                    Value::Str(s) => print!("\"{}\"", s),
                                    _ => print!("{}", v),
                                }
                            }
                            print!("]");
                        }
                        Value::Map(m) => {
                            let borrow = m.borrow();
                            // Sort keys for deterministic output
                            let mut entries: Vec<_> = borrow.iter().collect();
                            entries.sort_by(|a, b| a.0.cmp(b.0));
                            print!("{{");
                            for (i, (k, v)) in entries.iter().enumerate() {
                                if i > 0 { print!(", "); }
                                print!("{}: {}", k, v);
                            }
                            print!("}}");
                        }
                        Value::Ptr(addr) => {
                            // Read null-terminated string from memory
                            let mut bytes = Vec::new();
                            let mut offset = *addr;
                            loop {
                                let b = self.memory.load(offset, 1);
                                if b[0] == 0 { break; }
                                bytes.push(b[0]);
                                offset += 1;
                            }
                            let s = String::from_utf8_lossy(&bytes);
                            print!("{}", s);
                        }
                        _ => print!("{:?}", v),
                    }
                }
                // Note: print does NOT add a newline — println does that.
                ExecOutcome::Value(Value::Void)
            }

            // ─── Vec<T> runtime functions ───
            "p_vec_new" => {
                ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(SmallVec::new()))))
            }
            "p_vec_push" => {
                if let (Some(Value::Vec(arr)), Some(val)) = (args.first(), args.get(1)) {
                    arr.borrow_mut().push(val.clone());
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_pop" => {
                if let Some(Value::Vec(arr)) = args.first() {
                    if let Some(v) = arr.borrow_mut().pop() {
                        return ExecOutcome::Value(v);
                    }
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_len" => {
                if let Some(Value::Vec(arr)) = args.first() {
                    return ExecOutcome::Value(Value::Int(arr.borrow().len() as i64));
                }
                ExecOutcome::Value(Value::Int(0))
            }
            "p_vec_is_empty" => {
                if let Some(Value::Vec(arr)) = args.first() {
                    return ExecOutcome::Value(Value::Bool(arr.borrow().is_empty()));
                }
                ExecOutcome::Value(Value::Bool(true))
            }
            "p_vec_get" => {
                if let (Some(Value::Vec(arr)), Some(Value::Int(idx))) = (args.first(), args.get(1)) {
                    let borrow = arr.borrow();
                    if let Some(v) = borrow.get(*idx as usize) {
                        return ExecOutcome::Value(v.clone());
                    }
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_set" => {
                if let (Some(Value::Vec(arr)), Some(Value::Int(idx)), Some(val)) = (args.first(), args.get(1), args.get(2)) {
                    let mut borrow = arr.borrow_mut();
                    if let Some(slot) = borrow.get_mut(*idx as usize) {
                        *slot = val.clone();
                    }
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_clear" => {
                if let Some(Value::Vec(arr)) = args.first() {
                    arr.borrow_mut().clear();
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_capacity" => {
                if let Some(Value::Vec(arr)) = args.first() {
                    return ExecOutcome::Value(Value::Int(arr.borrow().capacity() as i64));
                }
                ExecOutcome::Value(Value::Int(0))
            }

            // ─── HashMap<K, V> runtime functions ───
            "p_map_new" => {
                ExecOutcome::Value(Value::Map(Rc::new(RefCell::new(hashbrown::HashMap::new()))))
            }
            "p_map_insert" => {
                if let (Some(Value::Map(m)), Some(key), Some(val)) = (args.first(), args.get(1), args.get(2)) {
                    let key_str = match key {
                        Value::Str(s) => s.to_string(),
                        Value::Int(i) => i.to_string(),
                        Value::Bool(b) => b.to_string(),
                        Value::Float(f) => f.to_string(),
                        _ => format!("{:?}", key),
                    };
                    m.borrow_mut().insert(key_str, val.clone());
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_map_get" => {
                if let (Some(Value::Map(m)), Some(key)) = (args.first(), args.get(1)) {
                    let key_str = match key {
                        Value::Str(s) => s.to_string(),
                        Value::Int(i) => i.to_string(),
                        Value::Bool(b) => b.to_string(),
                        Value::Float(f) => f.to_string(),
                        _ => format!("{:?}", key),
                    };
                    let borrow = m.borrow();
                    if let Some(v) = borrow.get(&key_str) {
                        return ExecOutcome::Value(v.clone());
                    }
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_map_contains" => {
                if let (Some(Value::Map(m)), Some(key)) = (args.first(), args.get(1)) {
                    let key_str = match key {
                        Value::Str(s) => s.to_string(),
                        Value::Int(i) => i.to_string(),
                        Value::Bool(b) => b.to_string(),
                        Value::Float(f) => f.to_string(),
                        _ => format!("{:?}", key),
                    };
                    return ExecOutcome::Value(Value::Bool(m.borrow().contains_key(&key_str)));
                }
                ExecOutcome::Value(Value::Bool(false))
            }
            "p_map_remove" => {
                if let (Some(Value::Map(m)), Some(key)) = (args.first(), args.get(1)) {
                    let key_str = match key {
                        Value::Str(s) => s.to_string(),
                        Value::Int(i) => i.to_string(),
                        Value::Bool(b) => b.to_string(),
                        Value::Float(f) => f.to_string(),
                        _ => format!("{:?}", key),
                    };
                    m.borrow_mut().remove(&key_str);
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_map_len" => {
                if let Some(Value::Map(m)) = args.first() {
                    return ExecOutcome::Value(Value::Int(m.borrow().len() as i64));
                }
                ExecOutcome::Value(Value::Int(0))
            }
            "p_map_is_empty" => {
                if let Some(Value::Map(m)) = args.first() {
                    return ExecOutcome::Value(Value::Bool(m.borrow().is_empty()));
                }
                ExecOutcome::Value(Value::Bool(true))
            }
            "p_map_clear" => {
                if let Some(Value::Map(m)) = args.first() {
                    m.borrow_mut().clear();
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_map_keys" => {
                if let Some(Value::Map(m)) = args.first() {
                    let keys: SmallVec<[Value; 8]> = m.borrow()
                        .keys()
                        .map(|k| Value::Str(CompactString::from(k.as_str())))
                        .collect();
                    return ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(keys))));
                }
                ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(SmallVec::new()))))
            }
            "p_map_values" => {
                if let Some(Value::Map(m)) = args.first() {
                    let vals: SmallVec<[Value; 8]> = m.borrow()
                        .values()
                        .cloned()
                        .collect();
                    return ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(vals))));
                }
                ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(SmallVec::new()))))
            }

            // ─── String runtime functions ───
            "p_string_from" => {
                let s = match args.first() {
                    Some(Value::Str(s)) => s.to_string(),
                    Some(Value::Int(i)) => i.to_string(),
                    Some(Value::Float(f)) => f.to_string(),
                    Some(Value::Bool(b)) => b.to_string(),
                    _ => String::new(),
                };
                ExecOutcome::Value(Value::Str(CompactString::from(s)))
            }
            "p_string_len" => {
                if let Some(Value::Str(s)) = args.first() {
                    return ExecOutcome::Value(Value::Int(s.len() as i64));
                }
                ExecOutcome::Value(Value::Int(0))
            }
            "p_string_is_empty" => {
                if let Some(Value::Str(s)) = args.first() {
                    return ExecOutcome::Value(Value::Bool(s.is_empty()));
                }
                ExecOutcome::Value(Value::Bool(true))
            }
            "p_string_push_str" => {
                if let (Some(Value::Str(left)), Some(Value::Str(right))) = (args.first(), args.get(1)) {
                    let mut s = left.to_string();
                    s.push_str(right.as_str());
                    return ExecOutcome::Value(Value::Str(CompactString::from(s)));
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_string_to_string" => {
                let s = match args.first() {
                    Some(Value::Str(s)) => s.to_string(),
                    Some(Value::Int(i)) => i.to_string(),
                    Some(Value::Float(f)) => f.to_string(),
                    Some(Value::Bool(b)) => b.to_string(),
                    _ => String::new(),
                };
                ExecOutcome::Value(Value::Str(CompactString::from(s)))
            }
            "p_string_slice" => {
                if let (Some(Value::Str(s)), Some(Value::Int(start)), Some(Value::Int(end))) = (args.first(), args.get(1), args.get(2)) {
                    let start = (*start as usize).min(s.len());
                    let end = (*end as usize).min(s.len());
                    if start <= end {
                        return ExecOutcome::Value(Value::Str(CompactString::from(&s[start..end])));
                    }
                }
                ExecOutcome::Value(Value::Str(CompactString::from("")))
            }
            "p_string_contains" => {
                if let (Some(Value::Str(s)), Some(Value::Str(sub))) = (args.first(), args.get(1)) {
                    return ExecOutcome::Value(Value::Bool(s.contains(sub.as_str())));
                }
                ExecOutcome::Value(Value::Bool(false))
            }
            "p_string_starts_with" => {
                if let (Some(Value::Str(s)), Some(Value::Str(prefix))) = (args.first(), args.get(1)) {
                    return ExecOutcome::Value(Value::Bool(s.starts_with(prefix.as_str())));
                }
                ExecOutcome::Value(Value::Bool(false))
            }
            "p_string_ends_with" => {
                if let (Some(Value::Str(s)), Some(Value::Str(suffix))) = (args.first(), args.get(1)) {
                    return ExecOutcome::Value(Value::Bool(s.ends_with(suffix.as_str())));
                }
                ExecOutcome::Value(Value::Bool(false))
            }
            "p_string_trim" => {
                if let Some(Value::Str(s)) = args.first() {
                    return ExecOutcome::Value(Value::Str(CompactString::from(s.trim())));
                }
                ExecOutcome::Value(Value::Str(CompactString::from("")))
            }
            "p_string_split" => {
                if let (Some(Value::Str(s)), Some(Value::Str(delim))) = (args.first(), args.get(1)) {
                    let parts: SmallVec<[Value; 8]> = s.split(delim.as_str())
                        .map(|p| Value::Str(CompactString::from(p)))
                        .collect();
                    return ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(parts))));
                }
                ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(SmallVec::new()))))
            }
            "p_string_replace" => {
                if let (Some(Value::Str(s)), Some(Value::Str(from)), Some(Value::Str(to))) = (args.first(), args.get(1), args.get(2)) {
                    return ExecOutcome::Value(Value::Str(CompactString::from(s.replace(from.as_str(), to.as_str()))));
                }
                ExecOutcome::Value(Value::Str(CompactString::from("")))
            }
            "p_string_char_at" => {
                if let (Some(Value::Str(s)), Some(Value::Int(idx))) = (args.first(), args.get(1)) {
                    if let Some(c) = s.chars().nth(*idx as usize) {
                        return ExecOutcome::Value(Value::Str(CompactString::from(c.to_string())));
                    }
                }
                ExecOutcome::Value(Value::Str(CompactString::from("")))
            }
            "p_string_to_uppercase" => {
                if let Some(Value::Str(s)) = args.first() {
                    return ExecOutcome::Value(Value::Str(CompactString::from(s.to_uppercase())));
                }
                ExecOutcome::Value(Value::Str(CompactString::from("")))
            }
            "p_string_to_lowercase" => {
                if let Some(Value::Str(s)) = args.first() {
                    return ExecOutcome::Value(Value::Str(CompactString::from(s.to_lowercase())));
                }
                ExecOutcome::Value(Value::Str(CompactString::from("")))
            }
            "p_string_parse_int" => {
                if let Some(Value::Str(s)) = args.first() {
                    return ExecOutcome::Value(Value::Int(s.parse::<i64>().unwrap_or(0)));
                }
                ExecOutcome::Value(Value::Int(0))
            }
            "p_string_find" => {
                if let (Some(Value::Str(s)), Some(Value::Str(sub))) = (args.first(), args.get(1)) {
                    if let Some(pos) = s.find(sub.as_str()) {
                        return ExecOutcome::Value(Value::Int(pos as i64));
                    }
                }
                ExecOutcome::Value(Value::Int(-1))
            }
            "p_string_rfind" => {
                if let (Some(Value::Str(s)), Some(Value::Str(sub))) = (args.first(), args.get(1)) {
                    if let Some(pos) = s.rfind(sub.as_str()) {
                        return ExecOutcome::Value(Value::Int(pos as i64));
                    }
                }
                ExecOutcome::Value(Value::Int(-1))
            }
            "p_string_repeat" => {
                if let (Some(Value::Str(s)), Some(Value::Int(n))) = (args.first(), args.get(1)) {
                    return ExecOutcome::Value(Value::Str(CompactString::from(s.repeat(*n as usize))));
                }
                ExecOutcome::Value(Value::Str(CompactString::from("")))
            }
            "p_string_bytes" => {
                if let Some(Value::Str(s)) = args.first() {
                    return ExecOutcome::Value(Value::Int(s.len() as i64));
                }
                ExecOutcome::Value(Value::Int(0))
            }
            "p_string_clone" => {
                if let Some(Value::Str(s)) = args.first() {
                    return ExecOutcome::Value(Value::Str(s.clone()));
                }
                ExecOutcome::Value(Value::Str(CompactString::from("")))
            }

            // ─── New Vec runtime functions ───
            "p_vec_insert" => {
                if let (Some(Value::Vec(arr)), Some(Value::Int(idx)), Some(val)) = (args.first(), args.get(1), args.get(2)) {
                    arr.borrow_mut().insert(*idx as usize, val.clone());
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_remove" => {
                if let (Some(Value::Vec(arr)), Some(Value::Int(idx))) = (args.first(), args.get(1)) {
                    if (*idx as usize) < arr.borrow().len() {
                        return ExecOutcome::Value(arr.borrow_mut().remove(*idx as usize));
                    }
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_contains" => {
                if let (Some(Value::Vec(arr)), Some(val)) = (args.first(), args.get(1)) {
                    return ExecOutcome::Value(Value::Bool(arr.borrow().iter().any(|v| v == val)));
                }
                ExecOutcome::Value(Value::Bool(false))
            }
            "p_vec_reverse" => {
                if let Some(Value::Vec(arr)) = args.first() {
                    arr.borrow_mut().reverse();
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_first" => {
                if let Some(Value::Vec(arr)) = args.first() {
                    if let Some(v) = arr.borrow().first() {
                        return ExecOutcome::Value(v.clone());
                    }
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_last" => {
                if let Some(Value::Vec(arr)) = args.first() {
                    if let Some(v) = arr.borrow().last() {
                        return ExecOutcome::Value(v.clone());
                    }
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_extend" => {
                if let (Some(Value::Vec(dst)), Some(Value::Vec(src))) = (args.first(), args.get(1)) {
                    dst.borrow_mut().extend(src.borrow().iter().cloned());
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_truncate" => {
                if let (Some(Value::Vec(arr)), Some(Value::Int(len))) = (args.first(), args.get(1)) {
                    arr.borrow_mut().truncate(*len as usize);
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_sort" => {
                if let Some(Value::Vec(arr)) = args.first() {
                    arr.borrow_mut().sort_by(|a, b| {
                        match (a, b) {
                            (Value::Int(a), Value::Int(b)) => a.cmp(b),
                            (Value::Float(a), Value::Float(b)) => a.partial_cmp(b).unwrap_or(std::cmp::Ordering::Equal),
                            (Value::Str(a), Value::Str(b)) => a.cmp(b),
                            _ => std::cmp::Ordering::Equal,
                        }
                    });
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_clone" => {
                if let Some(Value::Vec(arr)) = args.first() {
                    return ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(arr.borrow().clone()))));
                }
                ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(SmallVec::new()))))
            }

            // ─── New HashMap runtime functions ───
            "p_map_get_or" => {
                if let (Some(Value::Map(m)), Some(key), Some(default)) = (args.first(), args.get(1), args.get(2)) {
                    let key_str = match key {
                        Value::Str(s) => s.to_string(),
                        Value::Int(i) => i.to_string(),
                        Value::Bool(b) => b.to_string(),
                        Value::Float(f) => f.to_string(),
                        _ => format!("{:?}", key),
                    };
                    let borrow = m.borrow();
                    if let Some(v) = borrow.get(&key_str) {
                        return ExecOutcome::Value(v.clone());
                    }
                    return ExecOutcome::Value(default.clone());
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_map_clone" => {
                if let Some(Value::Map(m)) = args.first() {
                    return ExecOutcome::Value(Value::Map(Rc::new(RefCell::new(m.borrow().clone()))));
                }
                ExecOutcome::Value(Value::Map(Rc::new(RefCell::new(hashbrown::HashMap::new()))))
            }

            _ => ExecOutcome::Panic(format!("Unknown runtime function: {}", name)),
        }
    }
}

/// Outcome of executing a single instruction
enum ExecOutcome {
    /// Continue to next instruction (no value produced)
    Continue,
    /// A value was produced — store it and continue
    Value(Value),
    /// Jump to a different block
    Jump(BlockId),
    /// Return from function
    Return(Value),
    /// Panic — abort execution
    Panic(String),
}

/// Convenience function — lower and interpret a program
pub fn interpret(module: IrModule, verbose: bool) -> RunResult {
    let interp = if verbose {
        Interpreter::new(module).verbose()
    } else {
        Interpreter::new(module)
    };
    // We need mutable interp to run, but verbose() consumes self
    let mut interp = interp;
    interp.run()
}
