// Fides IR Interpreter
//
// Executes Fides IR directly without generating machine code.
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
use crate::tensor::Tensor;
use crate::json;
use std::collections::{HashMap, HashSet};
use std::cell::RefCell;
use std::rc::Rc;
use std::sync::{Arc, Mutex};
use smallvec::SmallVec;
use compact_str::CompactString;

/// A runtime value in the interpreter
#[derive(Debug, Clone)]
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
    /// Trait object — (concrete_type_name, data_pointer)
    Dyn(String, u64),
    /// Tensor — n-dimensional array (Phase 5C.1)
    Tensor(Rc<RefCell<Tensor>>),
    /// JSON value (Phase 5D.2)
    Json(Rc<RefCell<json::Json>>),
    /// Allocator — explicit memory allocator (Phase 2A)
    /// Stores the region name it was created in. Used for allocator-as-parameter verification.
    Allocator(CompactString),
    /// Future — async/await (Phase 7.6)
    /// A Future wraps a completed value (eager evaluation model).
    /// The Future handle is an index into the interpreter's future table.
    Future(usize),
    /// Thread handle — for spawn/dynamic loading (Phase 7.7)
    ThreadHandle(usize),
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
            Value::Vec(v) => {
                let v = v.borrow();
                write!(f, "[")?;
                for (i, elem) in v.iter().enumerate() {
                    if i > 0 { write!(f, ", ")?; }
                    write!(f, "{}", elem)?;
                }
                write!(f, "]")
            }
            Value::Map(m) => {
                let m = m.borrow();
                // Sort keys for deterministic output
                let mut keys: Vec<&String> = m.keys().collect();
                keys.sort();
                write!(f, "{{")?;
                for (i, key) in keys.iter().enumerate() {
                    if i > 0 { write!(f, ", ")?; }
                    write!(f, "{}: {}", key, m.get(*key).unwrap())?;
                }
                write!(f, "}}")
            }
            Value::Dyn(t, _) => write!(f, "dyn({})", t),
            Value::Tensor(t) => write!(f, "{}", t.borrow().to_string()),
            Value::Json(j) => write!(f, "{}", json::dump(&j.borrow(), false)),
            Value::Allocator(a) => write!(f, "alloc({})", a),
            Value::Future(idx) => write!(f, "Future#{}", idx),
            Value::ThreadHandle(idx) => write!(f, "Thread#{}", idx),
        }
    }
}

impl PartialEq for Value {
    fn eq(&self, other: &Self) -> bool {
        match (self, other) {
            (Value::Int(a), Value::Int(b)) => a == b,
            (Value::Float(a), Value::Float(b)) => a == b,
            (Value::Bool(a), Value::Bool(b)) => a == b,
            (Value::Str(a), Value::Str(b)) => a == b,
            (Value::Ptr(a), Value::Ptr(b)) => a == b,
            (Value::Void, Value::Void) => true,
            (Value::Vec(a), Value::Vec(b)) => {
                a.borrow().as_slice() == b.borrow().as_slice()
            }
            (Value::Map(a), Value::Map(b)) => {
                a.borrow().eq(&*b.borrow())
            }
            // Opaque types: reference identity
            (Value::Dyn(a, _), Value::Dyn(b, _)) => a == b,
            (Value::Tensor(a), Value::Tensor(b)) => {
                a.as_ptr() == b.as_ptr()
            }
            (Value::Json(a), Value::Json(b)) => {
                a.as_ptr() == b.as_ptr()
            }
            (Value::Future(a), Value::Future(b)) => a == b,
            (Value::ThreadHandle(a), Value::ThreadHandle(b)) => a == b,
            _ => false,
        }
    }
}

/// Interpreter memory — simple byte-addressed array
#[derive(Clone)]
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
    /// Debug mode — enables runtime safety checks (overflow, bounds)
    debug_mode: bool,
    /// Array bounds tracking — maps base address to array size
    array_bounds: HashMap<u64, usize>,
    /// In-memory databases for p_db_* builtins (Phase 5D.4)
    databases: HashMap<u64, hashbrown::HashMap<String, Value>>,
    /// Next database handle
    next_db_handle: u64,
    /// Persistent state for `persist` variables (F2)
    /// Maps variable name → saved value. Survives across interpreter runs
    /// when using `fidesc watch` (hot reload).
    persist_state: HashMap<String, String>,
    /// Transaction snapshot stack — for `txn {}` rollback.
    /// Each entry is a snapshot of (memory, opaque_values) taken at TxnBegin.
    /// On TxnEnd, the top snapshot is discarded (commit).
    /// On panic, the top snapshot is restored (rollback).
    txn_snapshots: Vec<(Memory, HashMap<u64, Value>)>,
    /// Functions annotated with @wrapping — use wrap-around arithmetic instead
    /// of checked arithmetic (F65). Checked arithmetic is the default (F64).
    wrapping_funcs: HashSet<String>,
    /// Current function name — tracked to determine if we're in a @wrapping function
    current_func_name: String,
    /// Unsafe block depth in the currently executing code.
    /// When > 0, bounds checks are skipped (F62: bounds checking lock —
    /// can only be bypassed inside unsafe {}).
    interp_unsafe_depth: usize,
    /// Future table — maps Future handle (index) to FutureState (Phase 7.6 + Phase 8A).
    /// Ready futures have their value available immediately.
    /// Pending futures have a JoinHandle that will produce a value when joined.
    futures: Vec<FutureState>,
    /// Shared memory for atomics — maps address to value.
    /// Shared across threads via Arc<Mutex>.
    shared_memory: Arc<Mutex<HashMap<i64, i64>>>,
    /// Next shared memory address (starts at 0x10000000 to avoid collision with stack)
    next_shared_addr: i64,
    /// Mutex pool — maps handle to Arc<Mutex<()>>
    mutexes: Arc<Mutex<HashMap<i64, Arc<Mutex<()>>>>>,
    /// Next mutex handle
    next_mutex_handle: i64,
    /// Dynamic loading handles — maps handle to library path
    dl_handles: HashMap<i64, String>,
    /// Next dynamic loading handle
    next_dl_handle: i64,
}

/// Result of running a function
#[derive(Debug)]
pub enum RunResult {
    /// Function returned normally with a value
    Return(Value),
    /// Function panicked
    Panic(String),
}

/// State of a Future — either Ready (completed with a value) or Pending
/// (a thread is running and will produce a value when joined).
enum FutureState {
    /// Future is ready — value is available
    Ready(Value),
    /// Future is pending — a thread is running.
    /// The JoinHandle returns an i64 (threads communicate via i64 since
    /// Value contains non-Send Rc types).
    Pending(Option<std::thread::JoinHandle<i64>>),
}

impl Interpreter {
    pub fn new(module: IrModule) -> Self {
        let mut interp = Interpreter {
            module,
            memory: Memory::new(),
            string_addrs: HashMap::new(),
            opaque_values: HashMap::new(),
            verbose: false,
            debug_mode: true,
            array_bounds: HashMap::new(),
            databases: HashMap::new(),
            next_db_handle: 1,
            persist_state: HashMap::new(),
            txn_snapshots: Vec::new(),
            wrapping_funcs: HashSet::new(),
            current_func_name: String::new(),
            interp_unsafe_depth: 0,
            futures: Vec::new(),
            shared_memory: Arc::new(Mutex::new(HashMap::new())),
            next_shared_addr: 0x10000000,
            mutexes: Arc::new(Mutex::new(HashMap::new())),
            next_mutex_handle: 1,
            dl_handles: HashMap::new(),
            next_dl_handle: 1,
        };
        // Pre-allocate string constants
        interp.init_strings();
        interp
    }

    pub fn verbose(mut self) -> Self {
        self.verbose = true;
        self
    }

    /// Convert a Value to a String, handling both Value::Str and Value::Ptr
    /// (Ptr reads a null-terminated C string from memory)
    fn value_to_string(&self, v: &Value) -> String {
        match v {
            Value::Str(s) => s.to_string(),
            Value::Ptr(addr) => {
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
            Value::Int(i) => i.to_string(),
            _ => String::new(),
        }
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
        // Track current function name for @wrapping check (F64/F65)
        let saved_func_name = self.current_func_name.clone();
        self.current_func_name = func.name.clone();
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
                        self.current_func_name = saved_func_name;
                        return RunResult::Return(val);
                    }
                    ExecOutcome::Panic(msg) => {
                        // Transaction rollback: if we're inside a txn block,
                        // restore the most recent snapshot before propagating.
                        if let Some((mem_snap, opaque_snap)) = self.txn_snapshots.pop() {
                            self.memory = mem_snap;
                            self.opaque_values = opaque_snap;
                        }
                        self.current_func_name = saved_func_name;
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
                    self.current_func_name = saved_func_name;
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
                // F64: checked arithmetic lock — overflow/div-by-zero ALWAYS panics
                // (not just in debug mode). @wrapping functions never hit this
                // because they use wrapping arithmetic in eval_binop.
                if result == Value::Void {
                    let msg = match op {
                        BinOp::Add => format!("Integer overflow on addition: {} + {}", 
                            if let Value::Int(a) = l { a.to_string() } else { "?".to_string() },
                            if let Value::Int(b) = r { b.to_string() } else { "?".to_string() }),
                        BinOp::Sub => format!("Integer overflow on subtraction: {} - {}",
                            if let Value::Int(a) = l { a.to_string() } else { "?".to_string() },
                            if let Value::Int(b) = r { b.to_string() } else { "?".to_string() }),
                        BinOp::Mul => format!("Integer overflow on multiplication: {} * {}",
                            if let Value::Int(a) = l { a.to_string() } else { "?".to_string() },
                            if let Value::Int(b) = r { b.to_string() } else { "?".to_string() }),
                        BinOp::Div => "Division by zero".to_string(),
                        BinOp::Mod => "Modulo by zero".to_string(),
                        _ => "Arithmetic error".to_string(),
                    };
                    return ExecOutcome::Panic(msg);
                }
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
                // Track array bounds for bounds checking
                if let IrType::Array(_, arr_size) = ty {
                    self.array_bounds.insert(addr, *arr_size);
                }
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
                        // Check side-table first — the value might be a stored pointer
                        // or an opaque value (Vec, Map, etc.) that was stored from a
                        // for-loop iteration variable.
                        if let Some(v) = self.opaque_values.get(&addr) {
                            if matches!(v, Value::Ptr(_) | Value::Vec(_) | Value::Map(_) | Value::Tensor(_) | Value::Json(_) | Value::Dyn(_, _)) {
                                return ExecOutcome::Value(v.clone());
                            }
                        }
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
                        // Check opaque side-table first — the value might be a
                        // stored Str (str fields have IR type Ptr(U8))
                        if let Some(v) = self.opaque_values.get(&addr) {
                            if matches!(v, Value::Str(_) | Value::Ptr(_)) {
                                return ExecOutcome::Value(v.clone());
                            }
                        }
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
                            // Tensor, Json, Dyn, Vec, Map are already in side-table via opaque_values.insert above
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
                    Some(Value::Future(idx)) => {
                        // Store Future handle as i64 index in byte memory
                        self.memory.store_i64(addr, *idx as i64);
                    }
                    Some(Value::ThreadHandle(idx)) => {
                        self.memory.store_i64(addr, *idx as i64);
                    }
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
                    Some(Value::Vec(_)) | Some(Value::Map(_)) | Some(Value::Tensor(_)) | Some(Value::Json(_)) | Some(Value::Dyn(_, _)) => {
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
                let idx_val = match values.get(index) {
                    Some(Value::Int(i)) => *i,
                    _ => 0,
                };
                // F62: bounds checking lock — bounds checks ALWAYS fire in safe mode.
                // Only skippable inside unsafe {} blocks (interp_unsafe_depth > 0).
                if self.interp_unsafe_depth == 0 {
                    if idx_val < 0 {
                        return ExecOutcome::Panic(format!(
                            "Array index out of bounds: negative index {}",
                            idx_val
                        ));
                    }
                    if let Some(arr_size) = self.array_bounds.get(&base) {
                        if (idx_val as usize) >= *arr_size {
                            return ExecOutcome::Panic(format!(
                                "Array index out of bounds: index {} but array has {} elements",
                                idx_val, arr_size
                            ));
                        }
                    }
                }
                let elem_size = self.module.type_size(elem_ty).max(1);
                ExecOutcome::Value(Value::Ptr(base + idx_val as u64 * elem_size))
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

            Inst::MakeDyn { value, type_name, .. } => {
                let data_ptr = match values.get(value) {
                    Some(Value::Ptr(p)) => *p,
                    Some(Value::Int(i)) => *i as u64,
                    _ => 0,
                };
                ExecOutcome::Value(Value::Dyn(type_name.clone(), data_ptr))
            }

            Inst::DynCall { trait_name, method, args, .. } => {
                let arg_vals: Vec<Value> = args.iter()
                    .map(|a| values.get(a).cloned().unwrap_or(Value::Void))
                    .collect();

                let (type_name, mut all_args) = match arg_vals.first() {
                    Some(Value::Dyn(t, p)) => {
                        let mut aa = arg_vals.clone();
                        aa[0] = Value::Ptr(*p);
                        (t.clone(), aa)
                    }
                    Some(Value::Str(s)) => (s.to_string(), arg_vals.clone()),
                    _ => return ExecOutcome::Panic(format!(
                        "DynCall: receiver is not a trait object ({}::{})",
                        trait_name, method
                    )),
                };

                let mangled = format!("{}_{}", type_name, method);

                if let Some(callee) = self.module.functions.get(&mangled).cloned() {
                    if callee.params.len() > all_args.len() {
                        all_args.insert(0, Value::Int(0));
                    }
                    return match self.run_function(&callee, &all_args) {
                        RunResult::Return(v) => ExecOutcome::Value(v),
                        RunResult::Panic(msg) => ExecOutcome::Panic(msg),
                    };
                }

                ExecOutcome::Panic(format!(
                    "DynCall: no method '{}' on type '{}' (looked for '{}')",
                    method, type_name, mangled
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

            // ─── Fides-Specific ───
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
                    Value::Ptr(addr) => {
                        // Try to load string from memory (null-terminated)
                        let mut bytes = Vec::new();
                        let mut offset = *addr;
                        loop {
                            let b = self.memory.load(offset, 1);
                            if b[0] == 0 { break; }
                            bytes.push(b[0]);
                            offset += 1;
                        }
                        if bytes.is_empty() {
                            // Not a string — print as pointer
                            print!("ptr(0x{:x})", addr);
                        } else {
                            print!("{}", String::from_utf8_lossy(&bytes));
                        }
                    }
                    _ => print!("{}", v),
                }
                let _ = ty;
                ExecOutcome::Continue
            }

            Inst::Panic { message } => {
                ExecOutcome::Panic(message.clone())
            }
            Inst::Asm { template, inputs, outputs, ret_ty } => {
                // Interpreter: no actual assembly execution
                // For input operands, values are already evaluated
                // For output operands, we store 0 (no-op in interpreter)
                if self.debug_mode {
                    eprintln!("[asm] {} (inputs: {}, outputs: {})",
                        template, inputs.len(), outputs.len());
                }
                match ret_ty {
                    crate::ir::IrType::Void => ExecOutcome::Continue,
                    _ => ExecOutcome::Value(Value::Int(0)),
                }
            }
            Inst::RegionBegin { name } => {
                // Begin a named region — in the interpreter, this is a no-op
                // (memory is managed by the interpreter's allocation tracking).
                // Return a fake region handle.
                if self.debug_mode {
                    eprintln!("[region] begin {}", name);
                }
                ExecOutcome::Value(Value::Int(0))
            }
            Inst::RegionEnd { name } => {
                // End a named region — in the interpreter, this is a no-op.
                // In a native backend, this would free all memory in the region.
                if self.debug_mode {
                    eprintln!("[region] end {}", name);
                }
                ExecOutcome::Continue
            }
            Inst::PersistLoad { name, default, .. } => {
                // Check if we have a saved value for this variable
                if let Some(saved) = self.persist_state.get(name) {
                    // Parse the saved string back into a Value
                    let default_val = values.get(default).cloned().unwrap_or(Value::Int(0));
                    let val = self.parse_persisted_value(saved, &default_val);
                    ExecOutcome::Value(val)
                } else {
                    // No saved value — use the default
                    let val = values.get(default).cloned().unwrap_or(Value::Int(0));
                    ExecOutcome::Value(val)
                }
            }
            Inst::PersistSave { name, value } => {
                let val = values.get(value).cloned().unwrap_or(Value::Void);
                let s = self.stringify_value(&val);
                self.persist_state.insert(name.clone(), s);
                ExecOutcome::Continue
            }
            Inst::TxnBegin => {
                // Snapshot memory and opaque values for potential rollback
                let mem_snapshot = self.memory.clone();
                let opaque_snapshot = self.opaque_values.clone();
                self.txn_snapshots.push((mem_snapshot, opaque_snapshot));
                ExecOutcome::Continue
            }
            Inst::TxnEnd => {
                // Commit — discard the snapshot
                self.txn_snapshots.pop();
                ExecOutcome::Continue
            }
            Inst::UnsafeBegin => {
                // F62: enter unsafe block — skip bounds checks
                self.interp_unsafe_depth += 1;
                ExecOutcome::Continue
            }
            Inst::UnsafeEnd => {
                // F62: exit unsafe block — resume bounds checks
                if self.interp_unsafe_depth > 0 {
                    self.interp_unsafe_depth -= 1;
                }
                ExecOutcome::Continue
            }
        }
    }

    /// Convert a Value to a string for persistence storage
    fn stringify_value(&self, val: &Value) -> String {
        match val {
            Value::Int(n) => format!("i:{}", n),
            Value::Bool(b) => format!("b:{}", b),
            Value::Float(f) => format!("f:{}", f),
            Value::Str(s) => format!("s:{}", s),
            _ => format!("v:{}", val),
        }
    }

    /// Parse a persisted string back into a Value, using the default's type as a hint
    fn parse_persisted_value(&self, saved: &str, default: &Value) -> Value {
        if let Some(colon) = saved.find(':') {
            let (prefix, rest) = saved.split_at(colon + 1);
            match prefix {
                "i:" => return Value::Int(rest.parse().unwrap_or(0)),
                "b:" => return Value::Bool(rest == "true"),
                "f:" => return Value::Float(rest.parse().unwrap_or(0.0)),
                "s:" => return Value::Str(CompactString::from(rest)),
                _ => {}
            }
        }
        // Fallback: use the default value
        default.clone()
    }

    /// Evaluate a binary operation
    fn eval_binop(&self, op: &BinOp, l: &Value, r: &Value) -> Value {
        // F64/F65: checked arithmetic is the default. @wrapping functions
        // use wrap-around arithmetic. Division by zero always panics.
        let use_wrapping = self.is_wrapping_func();
        match (l, r) {
            (Value::Int(a), Value::Int(b)) => {
                match op {
                    BinOp::Add => {
                        if use_wrapping {
                            Value::Int(a.wrapping_add(*b))
                        } else if let Some(r) = a.checked_add(*b) {
                            Value::Int(r)
                        } else {
                            return Value::Void; // panic handled by caller
                        }
                    }
                    BinOp::Sub => {
                        if use_wrapping {
                            Value::Int(a.wrapping_sub(*b))
                        } else if let Some(r) = a.checked_sub(*b) {
                            Value::Int(r)
                        } else {
                            return Value::Void;
                        }
                    }
                    BinOp::Mul => {
                        if use_wrapping {
                            Value::Int(a.wrapping_mul(*b))
                        } else if let Some(r) = a.checked_mul(*b) {
                            Value::Int(r)
                        } else {
                            return Value::Void;
                        }
                    }
                    BinOp::Div => {
                        if *b == 0 {
                            return Value::Void; // panic: division by zero (always)
                        } else {
                            Value::Int(a.wrapping_div(*b))
                        }
                    }
                    BinOp::Mod => {
                        if *b == 0 {
                            return Value::Void; // panic: modulo by zero (always)
                        } else {
                            Value::Int(a.wrapping_rem(*b))
                        }
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
            (Value::Str(a), Value::Int(b)) => {
                // String compared to integer (e.g. str == 0 for null check)
                match op {
                    BinOp::Eq => Value::Bool(a.is_empty() && *b == 0),
                    BinOp::Neq => Value::Bool(!(a.is_empty() && *b == 0)),
                    _ => Value::Void,
                }
            }
            (Value::Int(a), Value::Str(b)) => {
                match op {
                    BinOp::Eq => Value::Bool(b.is_empty() && *a == 0),
                    BinOp::Neq => Value::Bool(!(b.is_empty() && *a == 0)),
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
            (Value::Ptr(a), Value::Int(b)) => {
                // Pointer compared to integer (e.g. ptr == 0 for null check)
                match op {
                    BinOp::Eq => Value::Bool(*a as i64 == *b),
                    BinOp::Neq => Value::Bool(*a as i64 != *b),
                    _ => Value::Void,
                }
            }
            (Value::Int(a), Value::Ptr(b)) => {
                match op {
                    BinOp::Eq => Value::Bool(*a == *b as i64),
                    BinOp::Neq => Value::Bool(*a != *b as i64),
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

    /// Check if the currently executing function is @wrapping (F65).
    /// @wrapping functions use wrap-around arithmetic instead of checked.
    fn is_wrapping_func(&self) -> bool {
        if self.current_func_name.is_empty() {
            return false;
        }
        self.module.functions.get(&self.current_func_name)
            .map(|f| f.is_wrapping)
            .unwrap_or(false)
    }

    /// Check if a function name is a runtime function
    fn is_runtime_func(&self, name: &str) -> bool {
        self.module.extern_functions.contains_key(name)
            || name == "print"
            || name.starts_with("p_vec_")
            || name.starts_with("p_map_")
            || name.starts_with("p_string_")
            || name.starts_with("p_json_")
            || name.starts_with("p_tensor_")
            || name.starts_with("p_simd_")
            || name.starts_with("p_socket_")
            || name.starts_with("p_http_")
            || name.starts_with("p_db_")
            || name.starts_with("p_file_")
            || name.starts_with("p_thread_")
            || name.starts_with("p_atomic_")
            || name.starts_with("p_mutex_")
            || name.starts_with("p_shared_")
            || name.starts_with("p_future_")
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
            "p_float_to_str" => {
                let s = match args.first() {
                    Some(Value::Float(f)) => f.to_string(),
                    _ => String::new(),
                };
                ExecOutcome::Value(Value::Str(CompactString::from(s)))
            }
            "p_bool_to_str" => {
                let s = match args.first() {
                    Some(Value::Bool(b)) => b.to_string(),
                    _ => String::new(),
                };
                ExecOutcome::Value(Value::Str(CompactString::from(s)))
            }
            "p_vec_to_str" => {
                let s = match args.first() {
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
                    // F62: bounds checking lock — skip in unsafe blocks
                    if self.interp_unsafe_depth == 0 {
                        if *idx < 0 || (*idx as usize) >= borrow.len() {
                            return ExecOutcome::Panic(format!(
                                "Vec index out of bounds: index {} but vec has {} elements",
                                idx, borrow.len()
                            ));
                        }
                    }
                    let idx_clamped = if borrow.is_empty() { 0 } else { (*idx as usize).min(borrow.len() - 1) };
                    return ExecOutcome::Value(borrow[idx_clamped].clone());
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_set" => {
                if let (Some(Value::Vec(arr)), Some(Value::Int(idx)), Some(val)) = (args.first(), args.get(1), args.get(2)) {
                    let mut borrow = arr.borrow_mut();
                    // F62: bounds checking lock — skip in unsafe blocks
                    if self.interp_unsafe_depth == 0 {
                        if *idx < 0 || (*idx as usize) >= borrow.len() {
                            return ExecOutcome::Panic(format!(
                                "Vec index out of bounds: index {} but vec has {} elements",
                                idx, borrow.len()
                            ));
                        }
                    }
                    let idx_clamped = if borrow.is_empty() { 0 } else { (*idx as usize).min(borrow.len() - 1) };
                    borrow[idx_clamped] = val.clone();
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
                    let len = s.len() as i64;
                    if *start < 0 || *end > len || *start > *end {
                        return ExecOutcome::Panic(format!(
                            "String slice out of bounds: start={} end={} but string has {} chars",
                            start, end, len
                        ));
                    }
                    return ExecOutcome::Value(Value::Str(CompactString::from(&s[*start as usize..*end as usize])));
                }
                ExecOutcome::Value(Value::Str(CompactString::from("")))
            }
            "p_string_contains" => {
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let sub = self.value_to_string(args.get(1).unwrap_or(&Value::Int(0)));
                return ExecOutcome::Value(Value::Bool(s.contains(sub.as_str())));
            }
            "p_string_starts_with" => {
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let prefix = self.value_to_string(args.get(1).unwrap_or(&Value::Int(0)));
                return ExecOutcome::Value(Value::Bool(s.starts_with(prefix.as_str())));
            }
            "p_string_ends_with" => {
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let suffix = self.value_to_string(args.get(1).unwrap_or(&Value::Int(0)));
                return ExecOutcome::Value(Value::Bool(s.ends_with(suffix.as_str())));
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
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let from = self.value_to_string(args.get(1).unwrap_or(&Value::Int(0)));
                let to = self.value_to_string(args.get(2).unwrap_or(&Value::Int(0)));
                return ExecOutcome::Value(Value::Str(CompactString::from(s.replace(from.as_str(), to.as_str()))));
            }
            "p_string_char_at" => {
                if let (Some(Value::Str(s)), Some(Value::Int(idx))) = (args.first(), args.get(1)) {
                    let char_count = s.chars().count() as i64;
                    if *idx < 0 || *idx >= char_count {
                        return ExecOutcome::Panic(format!(
                            "String char_at out of bounds: index {} but string has {} chars",
                            idx, char_count
                        ));
                    }
                    let c = s.chars().nth(*idx as usize).unwrap();
                    return ExecOutcome::Value(Value::Str(CompactString::from(c.to_string())));
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
                    let len = arr.borrow().len() as i64;
                    if *idx < 0 || *idx > len {
                        return ExecOutcome::Panic(format!(
                            "Vec insert out of bounds: index {} but vec has {} elements",
                            idx, len
                        ));
                    }
                    arr.borrow_mut().insert(*idx as usize, val.clone());
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_remove" => {
                if let (Some(Value::Vec(arr)), Some(Value::Int(idx))) = (args.first(), args.get(1)) {
                    let len = arr.borrow().len() as i64;
                    if *idx < 0 || *idx >= len {
                        return ExecOutcome::Panic(format!(
                            "Vec remove out of bounds: index {} but vec has {} elements",
                            idx, len
                        ));
                    }
                    return ExecOutcome::Value(arr.borrow_mut().remove(*idx as usize));
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
                    return ExecOutcome::Panic("Vec first() on empty vec".to_string());
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_last" => {
                if let Some(Value::Vec(arr)) = args.first() {
                    if let Some(v) = arr.borrow().last() {
                        return ExecOutcome::Value(v.clone());
                    }
                    return ExecOutcome::Panic("Vec last() on empty vec".to_string());
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

            // ─── Phase 5D.2: JSON builtins (string-based) ───
            "p_json_parse" => {
                if let Some(s) = args.first() {
                    let text = match self.arg_string(s) {
                        Some(t) => t,
                        None => return ExecOutcome::Panic("json_parse: expected string".to_string()),
                    };
                    match json::parse(&text) {
                        Ok(j) => ExecOutcome::Value(Value::Str(CompactString::from(json::dump(&j, false)))),
                        Err(e) => ExecOutcome::Panic(format!("json_parse: {}", e)),
                    }
                } else {
                    ExecOutcome::Panic("json_parse: missing argument".to_string())
                }
            }
            "p_json_dump" => {
                if let Some(s) = args.first() {
                    let text = match self.arg_string(s) {
                        Some(t) => t,
                        None => return ExecOutcome::Panic("json_dump: expected string".to_string()),
                    };
                    match json::parse(&text) {
                        Ok(j) => ExecOutcome::Value(Value::Str(CompactString::from(json::dump(&j, false)))),
                        Err(e) => ExecOutcome::Panic(format!("json_dump: {}", e)),
                    }
                } else {
                    ExecOutcome::Panic("json_dump: missing argument".to_string())
                }
            }
            "p_json_get" => {
                if let (Some(s), Some(key)) = (args.get(0), args.get(1)) {
                    let text = match self.arg_string(s) {
                        Some(t) => t,
                        None => return ExecOutcome::Panic("json_get: expected string".to_string()),
                    };
                    let key_str = match self.arg_string(key) {
                        Some(k) => k,
                        None => return ExecOutcome::Panic("json_get: key must be string".to_string()),
                    };
                    match json::parse(&text) {
                        Ok(json::Json::Object(map)) => {
                            if let Some(v) = map.get(&key_str) {
                                ExecOutcome::Value(Value::Str(CompactString::from(json::dump(v, false))))
                            } else {
                                ExecOutcome::Value(Value::Str(CompactString::from("null")))
                            }
                        }
                        _ => ExecOutcome::Panic("json_get: not an object".to_string()),
                    }
                } else {
                    ExecOutcome::Panic("json_get: missing args".to_string())
                }
            }
            "p_json_type" => {
                if let Some(s) = args.first() {
                    let text = match self.arg_string(s) {
                        Some(t) => t,
                        None => return ExecOutcome::Panic("json_type: expected string".to_string()),
                    };
                    match json::parse(&text) {
                        Ok(j) => {
                            let ty = match j {
                                json::Json::Null => "null",
                                json::Json::Bool(_) => "boolean",
                                json::Json::Number(_) => "number",
                                json::Json::String(_) => "string",
                                json::Json::Array(_) => "array",
                                json::Json::Object(_) => "object",
                            };
                            ExecOutcome::Value(Value::Str(CompactString::from(ty)))
                        }
                        Err(e) => ExecOutcome::Panic(format!("json_type: {}", e)),
                    }
                } else {
                    ExecOutcome::Panic("json_type: missing argument".to_string())
                }
            }
            "p_json_len" => {
                if let Some(s) = args.first() {
                    let text = match self.arg_string(s) {
                        Some(t) => t,
                        None => return ExecOutcome::Panic("json_len: expected string".to_string()),
                    };
                    match json::parse(&text) {
                        Ok(json::Json::Array(a)) => ExecOutcome::Value(Value::Int(a.len() as i64)),
                        Ok(json::Json::Object(o)) => ExecOutcome::Value(Value::Int(o.len() as i64)),
                        _ => ExecOutcome::Panic("json_len: not array or object".to_string()),
                    }
                } else {
                    ExecOutcome::Panic("json_len: missing argument".to_string())
                }
            }
            "p_json_at" => {
                if let (Some(s), Some(Value::Int(idx))) = (args.get(0), args.get(1)) {
                    let text = match self.arg_string(s) {
                        Some(t) => t,
                        None => return ExecOutcome::Panic("json_at: expected string".to_string()),
                    };
                    match json::parse(&text) {
                        Ok(json::Json::Array(a)) => {
                            if let Some(v) = a.get(*idx as usize) {
                                ExecOutcome::Value(Value::Str(CompactString::from(json::dump(v, false))))
                            } else {
                                ExecOutcome::Panic(format!("json_at: index {} out of bounds", idx))
                            }
                        }
                        _ => ExecOutcome::Panic("json_at: not an array".to_string()),
                    }
                } else {
                    ExecOutcome::Panic("json_at: missing args".to_string())
                }
            }
            "p_json_pretty" => {
                if let Some(s) = args.first() {
                    let text = match self.arg_string(s) {
                        Some(t) => t,
                        None => return ExecOutcome::Panic("json_pretty: expected string".to_string()),
                    };
                    match json::parse(&text) {
                        Ok(j) => ExecOutcome::Value(Value::Str(CompactString::from(json::dump(&j, true)))),
                        Err(e) => ExecOutcome::Panic(format!("json_pretty: {}", e)),
                    }
                } else {
                    ExecOutcome::Panic("json_pretty: missing argument".to_string())
                }
            }
            "p_json_string" => {
                // Serialize a plain string as a JSON string literal (with quotes + escaping)
                if let Some(s) = args.first() {
                    let text = match self.arg_string(s) {
                        Some(t) => t,
                        None => return ExecOutcome::Panic("json_string: expected string".to_string()),
                    };
                    // Escape and wrap in quotes
                    let mut out = String::with_capacity(text.len() + 2);
                    out.push('"');
                    for c in text.chars() {
                        match c {
                            '"' => out.push_str("\\\""),
                            '\\' => out.push_str("\\\\"),
                            '\n' => out.push_str("\\n"),
                            '\r' => out.push_str("\\r"),
                            '\t' => out.push_str("\\t"),
                            c if (c as u32) < 0x20 => out.push_str(&format!("\\u{:04x}", c as u32)),
                            c => out.push(c),
                        }
                    }
                    out.push('"');
                    ExecOutcome::Value(Value::Str(CompactString::from(out)))
                } else {
                    ExecOutcome::Panic("json_string: missing argument".to_string())
                }
            }

            // ─── Phase 5C.1: Tensor builtins ───
            "p_tensor_zeros" => {
                if let Some(v) = arg_vec_usize(args, 0) {
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(Tensor::zeros(v)))))
                } else {
                    ExecOutcome::Panic("tensor_zeros: expected Vec".to_string())
                }
            }
            "p_tensor_ones" => {
                if let Some(v) = arg_vec_usize(args, 0) {
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(Tensor::ones(v)))))
                } else {
                    ExecOutcome::Panic("tensor_ones: expected Vec".to_string())
                }
            }
            "p_tensor_filled" => {
                if let (Some(v), Some(Value::Float(f))) = (arg_vec_usize(args, 0), args.get(1)) {
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(Tensor::filled(v, *f)))))
                } else {
                    ExecOutcome::Panic("tensor_filled: expected Vec, f64".to_string())
                }
            }
            "p_tensor_from_vec" => {
                // stdlib calls from_vec(data, shape) — args are (data, shape)
                if let (Some(data), Some(shape)) = (arg_vec_f64(args, 0), arg_vec_usize(args, 1)) {
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(Tensor::from_vec(shape, data)))))
                } else {
                    ExecOutcome::Panic("tensor_from_vec: expected Vec, Vec".to_string())
                }
            }
            "p_tensor_get" => {
                if let (Some(Value::Tensor(t)), Some(idx)) = (args.get(0), arg_vec_i64(args, 1)) {
                    ExecOutcome::Value(Value::Float(t.borrow().get(&idx)))
                } else {
                    ExecOutcome::Panic("tensor_get: expected Tensor, Vec".to_string())
                }
            }
            "p_tensor_set" => {
                if let (Some(Value::Tensor(t)), Some(idx), Some(Value::Float(v))) = (args.get(0), arg_vec_i64(args, 1), args.get(2)) {
                    t.borrow_mut().set(&idx, *v);
                    ExecOutcome::Continue
                } else {
                    ExecOutcome::Panic("tensor_set: expected Tensor, Vec, f64".to_string())
                }
            }
            "p_tensor_shape" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    let shape: SmallVec<[Value; 8]> = t.borrow().shape.iter()
                        .map(|&d| Value::Int(d as i64))
                        .collect();
                    ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(shape))))
                } else {
                    ExecOutcome::Panic("tensor_shape: expected Tensor".to_string())
                }
            }
            "p_tensor_ndim" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    ExecOutcome::Value(Value::Int(t.borrow().ndim() as i64))
                } else {
                    ExecOutcome::Panic("tensor_ndim: expected Tensor".to_string())
                }
            }
            "p_tensor_size" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    ExecOutcome::Value(Value::Int(t.borrow().size() as i64))
                } else {
                    ExecOutcome::Panic("tensor_size: expected Tensor".to_string())
                }
            }
            "p_tensor_reshape" => {
                if let (Some(Value::Tensor(t)), Some(new_shape)) = (args.get(0), arg_vec_usize(args, 1)) {
                    match t.borrow().reshape(new_shape) {
                        Ok(nt) => ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(nt)))),
                        Err(e) => ExecOutcome::Panic(e),
                    }
                } else {
                    ExecOutcome::Panic("tensor_reshape: expected Tensor, Vec".to_string())
                }
            }
            "p_tensor_transpose" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(t.borrow().transpose()))))
                } else {
                    ExecOutcome::Panic("tensor_transpose: expected Tensor".to_string())
                }
            }
            "p_tensor_add" => {
                if let (Some(Value::Tensor(a)), Some(Value::Tensor(b))) = (args.get(0), args.get(1)) {
                    match a.borrow().add(&b.borrow()) {
                        Ok(r) => ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(r)))),
                        Err(e) => ExecOutcome::Panic(e),
                    }
                } else {
                    ExecOutcome::Panic("tensor_add: expected Tensor, Tensor".to_string())
                }
            }
            "p_tensor_sub" => {
                if let (Some(Value::Tensor(a)), Some(Value::Tensor(b))) = (args.get(0), args.get(1)) {
                    match a.borrow().sub(&b.borrow()) {
                        Ok(r) => ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(r)))),
                        Err(e) => ExecOutcome::Panic(e),
                    }
                } else {
                    ExecOutcome::Panic("tensor_sub: expected Tensor, Tensor".to_string())
                }
            }
            "p_tensor_mul" => {
                if let (Some(Value::Tensor(a)), Some(Value::Tensor(b))) = (args.get(0), args.get(1)) {
                    match a.borrow().mul(&b.borrow()) {
                        Ok(r) => ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(r)))),
                        Err(e) => ExecOutcome::Panic(e),
                    }
                } else {
                    ExecOutcome::Panic("tensor_mul: expected Tensor, Tensor".to_string())
                }
            }
            "p_tensor_scale" => {
                if let (Some(Value::Tensor(t)), Some(Value::Float(s))) = (args.get(0), args.get(1)) {
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(t.borrow().scale(*s)))))
                } else {
                    ExecOutcome::Panic("tensor_scale: expected Tensor, f64".to_string())
                }
            }
            "p_tensor_matmul" => {
                if let (Some(Value::Tensor(a)), Some(Value::Tensor(b))) = (args.get(0), args.get(1)) {
                    match a.borrow().matmul(&b.borrow()) {
                        Ok(r) => ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(r)))),
                        Err(e) => ExecOutcome::Panic(e),
                    }
                } else {
                    ExecOutcome::Panic("tensor_matmul: expected Tensor, Tensor".to_string())
                }
            }
            "p_tensor_dot" => {
                if let (Some(Value::Tensor(a)), Some(Value::Tensor(b))) = (args.get(0), args.get(1)) {
                    match a.borrow().dot(&b.borrow()) {
                        Ok(r) => ExecOutcome::Value(Value::Float(r)),
                        Err(e) => ExecOutcome::Panic(e),
                    }
                } else {
                    ExecOutcome::Panic("tensor_dot: expected Tensor, Tensor".to_string())
                }
            }
            "p_tensor_sum" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    ExecOutcome::Value(Value::Float(t.borrow().sum()))
                } else {
                    ExecOutcome::Panic("tensor_sum: expected Tensor".to_string())
                }
            }
            "p_tensor_mean" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    ExecOutcome::Value(Value::Float(t.borrow().mean()))
                } else {
                    ExecOutcome::Panic("tensor_mean: expected Tensor".to_string())
                }
            }
            "p_tensor_max" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    ExecOutcome::Value(Value::Float(t.borrow().max()))
                } else {
                    ExecOutcome::Panic("tensor_max: expected Tensor".to_string())
                }
            }
            "p_tensor_min" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    ExecOutcome::Value(Value::Float(t.borrow().min()))
                } else {
                    ExecOutcome::Panic("tensor_min: expected Tensor".to_string())
                }
            }
            "p_tensor_relu" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(t.borrow().relu()))))
                } else {
                    ExecOutcome::Panic("tensor_relu: expected Tensor".to_string())
                }
            }
            "p_tensor_sigmoid" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(t.borrow().sigmoid()))))
                } else {
                    ExecOutcome::Panic("tensor_sigmoid: expected Tensor".to_string())
                }
            }
            "p_tensor_softmax" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(t.borrow().softmax()))))
                } else {
                    ExecOutcome::Panic("tensor_softmax: expected Tensor".to_string())
                }
            }
            "p_tensor_to_str" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    ExecOutcome::Value(Value::Str(CompactString::from(t.borrow().to_string())))
                } else {
                    ExecOutcome::Panic("tensor_to_str: expected Tensor".to_string())
                }
            }
            "p_tensor_to_json" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    ExecOutcome::Value(Value::Str(CompactString::from(t.borrow().to_json())))
                } else {
                    ExecOutcome::Panic("tensor_to_json: expected Tensor".to_string())
                }
            }
            "p_tensor_clone" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(t.borrow().clone()))))
                } else {
                    ExecOutcome::Panic("tensor_clone: expected Tensor".to_string())
                }
            }

            // ─── Phase 5C.2: SIMD builtins ───
            "p_simd_add" => simd_binary(args, |a, b| a + b),
            "p_simd_sub" => simd_binary(args, |a, b| a - b),
            "p_simd_mul" => simd_binary(args, |a, b| a * b),
            "p_simd_div" => simd_binary(args, |a, b| if b != 0.0 { a / b } else { 0.0 }),
            "p_simd_dot" => {
                if let (Some(a), Some(b)) = (arg_vec_f64(args, 0), arg_vec_f64(args, 1)) {
                    let dot: f64 = a.iter().zip(b.iter()).map(|(x, y)| x * y).sum();
                    ExecOutcome::Value(Value::Float(dot))
                } else {
                    ExecOutcome::Panic("simd_dot: expected Vec, Vec".to_string())
                }
            }
            "p_simd_sum" => {
                if let Some(v) = arg_vec_f64(args, 0) {
                    ExecOutcome::Value(Value::Float(v.iter().sum()))
                } else {
                    ExecOutcome::Panic("simd_sum: expected Vec".to_string())
                }
            }
            "p_simd_scale" => {
                if let (Some(v), Some(Value::Float(s))) = (arg_vec_f64(args, 0), args.get(1)) {
                    let result: SmallVec<[Value; 8]> = v.iter().map(|x| Value::Float(x * s)).collect();
                    ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(result))))
                } else {
                    ExecOutcome::Panic("simd_scale: expected Vec, f64".to_string())
                }
            }
            "p_simd_from_vec" => {
                if let Some(Value::Vec(v)) = args.first() {
                    let result: SmallVec<[Value; 8]> = v.borrow().iter().map(|x| match x {
                        Value::Int(i) => Value::Float(*i as f64),
                        Value::Float(f) => Value::Float(*f),
                        other => other.clone(),
                    }).collect();
                    ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(result))))
                } else {
                    ExecOutcome::Panic("simd_from_vec: expected Vec".to_string())
                }
            }
            "p_simd_to_str" => {
                if let Some(v) = arg_vec_f64(args, 0) {
                    let s: Vec<String> = v.iter().map(|x| format!("{}", x)).collect();
                    ExecOutcome::Value(Value::Str(CompactString::from(format!("[{}]", s.join(", ")))))
                } else {
                    ExecOutcome::Panic("simd_to_str: expected Vec".to_string())
                }
            }

            // ─── Phase 5D.3: Socket/HTTP builtins (simulated) ───
            "p_socket_listen" => {
                // Simulated socket — return a handle
                ExecOutcome::Value(Value::Int(1))
            }
            "p_socket_accept" => {
                // Simulated — return a client handle
                ExecOutcome::Value(Value::Int(2))
            }
            "p_socket_read" => {
                // Simulated — return empty vec
                ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(SmallVec::new()))))
            }
            "p_socket_write" => {
                // Simulated — return bytes "written"
                if let Some(Value::Vec(v)) = args.get(1) {
                    ExecOutcome::Value(Value::Int(v.borrow().len() as i64))
                } else {
                    ExecOutcome::Value(Value::Int(0))
                }
            }
            "p_socket_close" => ExecOutcome::Continue,
            "p_socket_recv_str" => {
                // Simulated — return empty string
                ExecOutcome::Value(Value::Str(CompactString::from("")))
            }
            "p_http_parse_request" => {
                // Parse raw HTTP request into a JSON string: {"method","path","version","headers":{...},"body"}
                let raw = match args.first().and_then(|v| self.arg_string(v)) {
                    Some(s) => s,
                    None => return ExecOutcome::Panic("http_parse_request: expected string".to_string()),
                };
                // Parse request line: METHOD PATH HTTP/VERSION
                let mut lines = raw.split("\r\n");
                let req_line = lines.next().unwrap_or("");
                let parts: Vec<&str> = req_line.splitn(3, ' ').collect();
                let method = parts.first().copied().unwrap_or("GET");
                let path = parts.get(1).copied().unwrap_or("/");
                let version = parts.get(2).copied().unwrap_or("HTTP/1.1").strip_prefix("HTTP/").unwrap_or("1.1");

                // Parse headers until empty line
                let mut headers = String::from("{");
                let mut first_hdr = true;
                let mut body_start = 0;
                let mut idx = 0;
                for line in lines {
                    idx += 1;
                    if line.is_empty() {
                        body_start = idx; // body starts after this
                        break;
                    }
                    if let Some(colon) = line.find(':') {
                        let key = line[..colon].trim();
                        let val = line[colon+1..].trim();
                        if !first_hdr { headers.push(','); }
                        first_hdr = false;
                        headers.push('"');
                        headers.push_str(key);
                        headers.push_str("\":\"");
                        headers.push_str(val);
                        headers.push('"');
                    }
                }
                headers.push('}');

                // Body is everything after the empty line
                let body = if body_start > 0 {
                    // Re-split to get body
                    raw.split("\r\n\r\n").nth(1).unwrap_or("")
                } else { "" };

                // Build JSON: {"method":"...","path":"...","version":"...","headers":{...},"body":"..."}
                // Escape string values for JSON
                let esc = |s: &str| -> String {
                    let mut out = String::with_capacity(s.len());
                    for c in s.chars() {
                        match c {
                            '"' => out.push_str("\\\""),
                            '\\' => out.push_str("\\\\"),
                            '\n' => out.push_str("\\n"),
                            '\r' => out.push_str("\\r"),
                            '\t' => out.push_str("\\t"),
                            c if (c as u32) < 0x20 => out.push_str(&format!("\\u{:04x}", c as u32)),
                            c => out.push(c),
                        }
                    }
                    out
                };
                let json = format!(
                    "{{\"method\":\"{}\",\"path\":\"{}\",\"version\":\"{}\",\"headers\":{},\"body\":\"{}\"}}",
                    esc(method), esc(path), esc(version), headers, esc(body)
                );
                ExecOutcome::Value(Value::Str(CompactString::from(json)))
            }
            "p_http_build_response" => {
                if let (Some(Value::Int(status)), Some(body)) = (args.get(0), args.get(2)) {
                    let body_str = match self.arg_string(body) {
                        Some(s) => s,
                        None => "".to_string(),
                    };
                    let response = format!("HTTP/1.1 {} OK\r\nContent-Length: {}\r\n\r\n{}",
                        status, body_str.len(), body_str);
                    ExecOutcome::Value(Value::Str(CompactString::from(response)))
                } else {
                    ExecOutcome::Value(Value::Str(CompactString::from("HTTP/1.1 500 Internal Server Error\r\n\r\n")))
                }
            }

            // ─── Phase 5D.4: Database builtins (in-memory KV) ───
            "p_db_mem_open" => {
                let handle = self.next_db_handle;
                self.next_db_handle += 1;
                self.databases.insert(handle, hashbrown::HashMap::new());
                ExecOutcome::Value(Value::Int(handle as i64))
            }
            "p_db_mem_close" => {
                if let Some(Value::Int(h)) = args.first() {
                    self.databases.remove(&(*h as u64));
                }
                ExecOutcome::Continue
            }
            "p_db_mem_query" => {
                // Query: SELECT key FROM db -> return JSON array of all keys
                // Sort keys for deterministic output (matches C codegen's insertion-order array)
                if let Some(Value::Int(h)) = args.first() {
                    if let Some(db) = self.databases.get(&(*h as u64)) {
                        let mut keys: Vec<&str> = db.keys().map(|k| k.as_str()).collect();
                        keys.sort();
                        let arr: Vec<json::Json> = keys.into_iter()
                            .map(|k| json::Json::String(k.to_string()))
                            .collect();
                        let j = json::Json::Array(arr);
                        return ExecOutcome::Value(Value::Str(CompactString::from(json::dump(&j, false))));
                    }
                }
                ExecOutcome::Value(Value::Str(CompactString::from("[]")))
            }
            "p_db_mem_exec" => {
                // Exec: INSERT/SET key=value
                if let (Some(Value::Int(h)), Some(query)) = (args.get(0), args.get(1)) {
                    let query_str = match query {
                        Value::Str(s) => s.to_string(),
                        _ => return ExecOutcome::Value(Value::Int(0)),
                    };
                    // Simple: "SET key value" or "INSERT key value"
                    let parts: Vec<&str> = query_str.splitn(3, ' ').collect();
                    if parts.len() >= 3 && (parts[0].eq_ignore_ascii_case("set") || parts[0].eq_ignore_ascii_case("insert")) {
                        if let Some(db) = self.databases.get_mut(&(*h as u64)) {
                            db.insert(parts[1].to_string(), Value::Str(CompactString::from(parts[2])));
                            return ExecOutcome::Value(Value::Int(1));
                        }
                    }
                }
                ExecOutcome::Value(Value::Int(0))
            }

            // ─── File I/O builtins ───
            "p_file_read" => {
                if let Some(Value::Str(path)) = args.get(0) {
                    let path_str = path.to_string();
                    match std::fs::read_to_string(&path_str) {
                        Ok(content) => {
                            let addr = self.memory.alloc(content.len() as u64 + 1);
                            self.memory.store(addr, content.as_bytes());
                            self.memory.store(addr + content.len() as u64, &[0]);
                            let val = Value::Str(CompactString::from(content));
                            self.opaque_values.insert(addr, val.clone());
                            return ExecOutcome::Value(Value::Ptr(addr));
                        }
                        Err(_) => return ExecOutcome::Value(Value::Ptr(0)),
                    }
                }
                ExecOutcome::Value(Value::Ptr(0))
            }
            "p_file_write" => {
                if let (Some(Value::Str(path)), Some(content)) = (args.get(0), args.get(1)) {
                    let path_str = path.to_string();
                    let content_str = match content {
                        Value::Str(s) => s.to_string(),
                        Value::Ptr(addr) => {
                            let bytes = self.memory.load(*addr, 4096);
                            let end = bytes.iter().position(|&b| b == 0).unwrap_or(bytes.len());
                            String::from_utf8_lossy(&bytes[..end]).to_string()
                        }
                        _ => return ExecOutcome::Value(Value::Int(-1)),
                    };
                    match std::fs::write(&path_str, &content_str) {
                        Ok(_) => return ExecOutcome::Value(Value::Int(0)),
                        Err(_) => return ExecOutcome::Value(Value::Int(-1)),
                    }
                }
                ExecOutcome::Value(Value::Int(-1))
            }
            "p_file_exists" => {
                if let Some(Value::Str(path)) = args.get(0) {
                    let path_str = path.to_string();
                    return ExecOutcome::Value(Value::Bool(std::path::Path::new(&path_str).exists()));
                }
                ExecOutcome::Value(Value::Bool(false))
            }
            "p_file_append" => {
                if let (Some(Value::Str(path)), Some(content)) = (args.get(0), args.get(1)) {
                    let path_str = path.to_string();
                    let content_str = match content {
                        Value::Str(s) => s.to_string(),
                        Value::Ptr(addr) => {
                            let bytes = self.memory.load(*addr, 4096);
                            let end = bytes.iter().position(|&b| b == 0).unwrap_or(bytes.len());
                            String::from_utf8_lossy(&bytes[..end]).to_string()
                        }
                        _ => return ExecOutcome::Value(Value::Int(-1)),
                    };
                    use std::io::Write;
                    match std::fs::OpenOptions::new().append(true).create(true).open(&path_str) {
                        Ok(mut f) => {
                            match f.write_all(content_str.as_bytes()) {
                                Ok(_) => return ExecOutcome::Value(Value::Int(0)),
                                Err(_) => return ExecOutcome::Value(Value::Int(-1)),
                            }
                        }
                        Err(_) => return ExecOutcome::Value(Value::Int(-1)),
                    }
                }
                ExecOutcome::Value(Value::Int(-1))
            }

            // ─── Phase 5A.3: Interrupt handler builtins (bare-metal) ───
            "p_cli" => {
                // Clear interrupt flag — simulated
                ExecOutcome::Value(Value::Void)
            }
            "p_sti" => {
                // Set interrupt flag — simulated
                ExecOutcome::Value(Value::Void)
            }
            "p_lidt" => {
                // Load IDT register — simulated (arg is pointer to IDT descriptor)
                ExecOutcome::Value(Value::Void)
            }
            "p_idt_set_entry" => {
                // Set IDT entry: (vector, handler_addr) — simulated
                ExecOutcome::Value(Value::Void)
            }
            "p_hlt" => {
                // Halt CPU — simulated
                ExecOutcome::Value(Value::Void)
            }
            "p_inb" => {
                // Read byte from I/O port — simulated, returns 0
                ExecOutcome::Value(Value::Int(0))
            }
            "p_outb" => {
                // Write byte to I/O port — simulated
                ExecOutcome::Value(Value::Void)
            }

            // ─── Phase 5A.4: MMU/page table management (bare-metal) ───
            "p_cr3_read" => {
                // Read CR3 — simulated, return a fake page table base
                ExecOutcome::Value(Value::Int(0x10000))
            }
            "p_cr3_write" => {
                // Write CR3 — simulated
                ExecOutcome::Value(Value::Void)
            }
            "p_cr0_read" => {
                // Read CR0 — simulated
                ExecOutcome::Value(Value::Int(0x80000011))
            }
            "p_cr0_write" => {
                // Write CR0 — simulated
                ExecOutcome::Value(Value::Void)
            }
            "p_cr4_read" => {
                // Read CR4 — simulated
                ExecOutcome::Value(Value::Int(0x20))
            }
            "p_cr4_write" => {
                // Write CR4 — simulated
                ExecOutcome::Value(Value::Void)
            }
            "p_invlpg" => {
                // Invalidate TLB entry — simulated
                ExecOutcome::Value(Value::Void)
            }
            "p_page_map" => {
                // Map virtual page to physical — simulated
                // args: page_table, virt_addr, phys_addr, flags
                ExecOutcome::Value(Value::Void)
            }
            "p_page_unmap" => {
                // Unmap virtual page — simulated
                ExecOutcome::Value(Value::Void)
            }
            "p_phys_read64" => {
                // Read 8 bytes from physical address — simulated, returns 0
                ExecOutcome::Value(Value::Int(0))
            }
            "p_phys_write64" => {
                // Write 8 bytes to physical address — simulated
                ExecOutcome::Value(Value::Void)
            }

            // ─── Phase 2A.8: Allocator builtins ───
            "p_allocator_create" => {
                if let Some(Value::Str(name)) = args.first() {
                    ExecOutcome::Value(Value::Allocator(name.clone()))
                } else {
                    ExecOutcome::Value(Value::Allocator(CompactString::from("default")))
                }
            }
            "p_allocator_alloc" => {
                // In interpreter mode, allocation just returns a synthetic pointer
                if let Some(Value::Int(size)) = args.get(1) {
                    // Simulate allocation — return a non-zero pointer
                    static mut ALLOC_PTR: u64 = 0x10000;
                    unsafe { ALLOC_PTR += *size as u64; }
                    ExecOutcome::Value(Value::Ptr(unsafe { ALLOC_PTR }))
                } else {
                    ExecOutcome::Value(Value::Ptr(0))
                }
            }
            "p_allocator_free" => {
                // No-op in interpreter mode
                ExecOutcome::Value(Value::Void)
            }
            "p_allocator_stats" => {
                // Return bytes allocated (simulated)
                ExecOutcome::Value(Value::Int(0))
            }

            // ─── Environment variable builtins ───
            "p_getenv" => {
                if let Some(Value::Str(name)) = args.first() {
                    let name_str = name.to_string();
                    match std::env::var(&name_str) {
                        Ok(val) => return ExecOutcome::Value(Value::Str(CompactString::from(val))),
                        Err(_) => return ExecOutcome::Value(Value::Ptr(0)),
                    }
                }
                ExecOutcome::Value(Value::Ptr(0))
            }

            // ─── Command-line argument builtins ───
            "p_argc" => {
                ExecOutcome::Value(Value::Int(std::env::args().count() as i64))
            }
            "p_argv" => {
                if let Some(Value::Int(idx)) = args.first() {
                    let args_vec: Vec<String> = std::env::args().collect();
                    if *idx >= 0 && (*idx as usize) < args_vec.len() {
                        return ExecOutcome::Value(Value::Str(CompactString::from(&args_vec[*idx as usize])));
                    }
                }
                ExecOutcome::Value(Value::Ptr(0))
            }

            // ─── Process control builtins ───
            "p_system" => {
                if let Some(Value::Str(cmd)) = args.first() {
                    let cmd_str = cmd.to_string();
                    let status = std::process::Command::new("sh")
                        .arg("-c")
                        .arg(&cmd_str)
                        .status();
                    match status {
                        Ok(s) => {
                            use std::os::unix::process::ExitStatusExt;
                            return ExecOutcome::Value(Value::Int(s.code().unwrap_or(s.signal().unwrap_or(0)) as i64));
                        }
                        Err(_) => return ExecOutcome::Value(Value::Int(-1)),
                    }
                }
                ExecOutcome::Value(Value::Int(-1))
            }
            "p_exit" => {
                if let Some(Value::Int(code)) = args.first() {
                    std::process::exit(*code as i32);
                }
                std::process::exit(0);
            }

            // ─── Regex builtin ───
            "p_regex_match" => {
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let pattern = self.value_to_string(args.get(1).unwrap_or(&Value::Int(0)));
                ExecOutcome::Value(Value::Bool(wildcard_match(&s, &pattern)))
            }

            // ─── Phase 7.6 + 8A: Async/await + threads runtime ───
            "p_future_ready" => {
                // Wrap a value in a ready Future. The value is stored in the
                // futures table and a Future handle (index) is returned.
                let value = args.first().cloned().unwrap_or(Value::Int(0));
                let idx = self.futures.len();
                self.futures.push(FutureState::Ready(value));
                ExecOutcome::Value(Value::Future(idx))
            }
            "p_future_poll" => {
                // Poll a Future — extract its value.
                // If Ready, return the value immediately.
                // If Pending (thread running), block until the thread completes.
                let idx = match args.first() {
                    Some(Value::Future(idx)) => *idx,
                    Some(Value::Int(n)) => *n as usize,
                    _ => return ExecOutcome::Panic("Future poll: not a Future value".to_string()),
                };
                if idx >= self.futures.len() {
                    return ExecOutcome::Panic(format!("Future poll: invalid handle {}", idx));
                }
                // Check if already Ready (take the value without consuming the slot)
                match &self.futures[idx] {
                    FutureState::Ready(v) => return ExecOutcome::Value(v.clone()),
                    FutureState::Pending(_) => {}
                }
                // Pending — join the thread
                if let FutureState::Pending(Some(handle)) =
                    std::mem::replace(&mut self.futures[idx], FutureState::Ready(Value::Int(0)))
                {
                    match handle.join() {
                        Ok(n) => {
                            let v = Value::Int(n);
                            self.futures[idx] = FutureState::Ready(v.clone());
                            ExecOutcome::Value(v)
                        }
                        Err(_) => {
                            self.futures[idx] = FutureState::Ready(Value::Int(0));
                            ExecOutcome::Panic("Thread panicked during join".to_string())
                        }
                    }
                } else {
                    ExecOutcome::Panic("Future poll: already consumed".to_string())
                }
            }

            // ─── Phase 8A: Real OS threads ───
            "p_thread_spawn" => {
                // p_thread_spawn(func_name: *u8, num_args: i64, args...) -> i64
                // Spawns an OS thread that runs the named function with the given args.
                // Returns a Future handle. Use p_future_poll to join.
                // Note: Only primitive values (i64, f64, bool, str) can be passed
                // to threads. Complex types (Vec, Map, etc.) are converted to i64(0).
                let func_name = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let num_args = match args.get(1) {
                    Some(Value::Int(n)) => *n as usize,
                    _ => 0,
                };
                // Convert args to Send-safe primitives (i64 only — threads get raw values)
                let thread_args: Vec<i64> = if args.len() >= 2 + num_args {
                    args[2..2 + num_args].iter().map(|v| match v {
                        Value::Int(n) => *n,
                        Value::Bool(b) => *b as i64,
                        Value::Float(f) => f.to_bits() as i64,
                        Value::Ptr(p) => *p as i64,
                        _ => 0,
                    }).collect()
                } else {
                    args[2..].iter().map(|v| match v {
                        Value::Int(n) => *n,
                        Value::Bool(b) => *b as i64,
                        Value::Float(f) => f.to_bits() as i64,
                        Value::Ptr(p) => *p as i64,
                        _ => 0,
                    }).collect()
                };

                // Clone the module (function table) for the thread
                let module = self.module.clone();
                // Clone shared state for the thread
                let shared_memory = self.shared_memory.clone();
                let mutexes = self.mutexes.clone();

                // Allocate a Future slot (Pending)
                let future_idx = self.futures.len();
                self.futures.push(FutureState::Pending(None));

                let handle = std::thread::spawn(move || {
                    let mut interp = Interpreter::new(module);
                    // Share the same shared_memory and mutexes
                    interp.shared_memory = shared_memory;
                    interp.mutexes = mutexes;

                    // Convert i64 args back to Values
                    // Prepend a dummy env value (user functions have __env as first param)
                    let mut thread_values: Vec<Value> = vec![Value::Int(0)];
                    for n in &thread_args {
                        thread_values.push(Value::Int(*n));
                    }

                    // Look up and run the function
                    let result = if let Some(func) = interp.module.functions.get(&func_name).cloned()
                    {
                        match interp.run_function(&func, &thread_values) {
                            RunResult::Return(v) => v,
                            RunResult::Panic(msg) => {
                                eprintln!("Thread panic: {}", msg);
                                Value::Int(0)
                            }
                        }
                    } else {
                        eprintln!("Thread: unknown function '{}'", func_name);
                        Value::Int(0)
                    };
                    // Convert result to Send-safe i64 (threads communicate via i64)
                    match result {
                        Value::Int(n) => n,
                        Value::Bool(b) => b as i64,
                        Value::Float(f) => f.to_bits() as i64,
                        Value::Ptr(p) => p as i64,
                        _ => 0,
                    }
                });

                // Store the JoinHandle in the Pending slot
                self.futures[future_idx] = FutureState::Pending(Some(handle));
                ExecOutcome::Value(Value::Future(future_idx))
            }

            // ─── Phase 8A: Shared memory + atomics ───
            "p_shared_alloc" => {
                // p_shared_alloc(size: i64) -> i64
                // Allocates a slot in shared memory. For now, allocates a single i64 slot
                // (size is ignored — each allocation is one i64).
                let addr = self.next_shared_addr;
                self.next_shared_addr += 8;
                // Initialize to 0
                self.shared_memory.lock().unwrap().insert(addr, 0);
                ExecOutcome::Value(Value::Int(addr))
            }
            "p_atomic_load" => {
                // p_atomic_load(addr: i64) -> i64
                let addr = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let val = self
                    .shared_memory
                    .lock()
                    .unwrap()
                    .get(&addr)
                    .copied()
                    .unwrap_or(0);
                ExecOutcome::Value(Value::Int(val))
            }
            "p_atomic_store" => {
                // p_atomic_store(addr: i64, val: i64) -> void
                let addr = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let val = match args.get(1) {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                self.shared_memory.lock().unwrap().insert(addr, val);
                ExecOutcome::Continue
            }
            "p_atomic_cas" => {
                // p_atomic_cas(addr: i64, expected: i64, new: i64) -> bool
                let addr = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let expected = match args.get(1) {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let new = match args.get(2) {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let mut shared = self.shared_memory.lock().unwrap();
                let current = shared.get(&addr).copied().unwrap_or(0);
                if current == expected {
                    shared.insert(addr, new);
                    ExecOutcome::Value(Value::Bool(true))
                } else {
                    ExecOutcome::Value(Value::Bool(false))
                }
            }
            "p_atomic_add" => {
                // p_atomic_add(addr: i64, val: i64) -> i64 (returns old value)
                let addr = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let val = match args.get(1) {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let mut shared = self.shared_memory.lock().unwrap();
                let old = shared.get(&addr).copied().unwrap_or(0);
                shared.insert(addr, old + val);
                ExecOutcome::Value(Value::Int(old))
            }

            // ─── Phase 8A: Mutex ───
            "p_mutex_init" => {
                // p_mutex_init() -> i64 — create a mutex, return handle
                let handle = self.next_mutex_handle;
                self.next_mutex_handle += 1;
                // Use shared_memory with the handle as address for the lock flag
                self.shared_memory.lock().unwrap().insert(handle, 0);
                ExecOutcome::Value(Value::Int(handle))
            }
            "p_mutex_lock" => {
                // p_mutex_lock(m: i64) -> void — lock a mutex (blocks if contended)
                // Uses a spinlock on shared_memory: spin until flag is 0, then set to 1.
                let m = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let shared = self.shared_memory.clone();
                loop {
                    let mut shared_lock = shared.lock().unwrap();
                    let current = shared_lock.get(&m).copied().unwrap_or(0);
                    if current == 0 {
                        shared_lock.insert(m, 1);
                        break;
                    }
                    drop(shared_lock);
                    std::thread::yield_now();
                }
                ExecOutcome::Continue
            }
            "p_mutex_unlock" => {
                // p_mutex_unlock(m: i64) -> void — unlock a mutex
                let m = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                self.shared_memory.lock().unwrap().insert(m, 0);
                ExecOutcome::Continue
            }

            // ─── C library function stubs (for extern FFI in interpreter mode) ───
            "abs" => {
                if let Some(Value::Int(n)) = args.first() {
                    ExecOutcome::Value(Value::Int(n.abs()))
                } else {
                    ExecOutcome::Value(Value::Int(0))
                }
            }
            "labs" => {
                if let Some(Value::Int(n)) = args.first() {
                    ExecOutcome::Value(Value::Int(n.abs()))
                } else {
                    ExecOutcome::Value(Value::Int(0))
                }
            }
            "sqrt" | "c_sqrt" | "pledge_sqrt" => {
                if let Some(Value::Float(f)) = args.first() {
                    ExecOutcome::Value(Value::Float(f.sqrt()))
                } else if let Some(Value::Int(i)) = args.first() {
                    ExecOutcome::Value(Value::Float((*i as f64).sqrt()))
                } else {
                    ExecOutcome::Value(Value::Float(0.0))
                }
            }
            "pow" | "c_pow" | "pledge_pow" => {
                if let (Some(Value::Float(a)), Some(Value::Float(b))) = (args.get(0), args.get(1)) {
                    ExecOutcome::Value(Value::Float(a.powf(*b)))
                } else {
                    ExecOutcome::Value(Value::Float(0.0))
                }
            }
            "floor" | "c_floor" | "pledge_floor" => {
                if let Some(Value::Float(f)) = args.first() {
                    ExecOutcome::Value(Value::Float(f.floor()))
                } else {
                    ExecOutcome::Value(Value::Float(0.0))
                }
            }
            "ceil" | "c_ceil" | "pledge_ceil" => {
                if let Some(Value::Float(f)) = args.first() {
                    ExecOutcome::Value(Value::Float(f.ceil()))
                } else {
                    ExecOutcome::Value(Value::Float(0.0))
                }
            }
            "sin" | "c_sin" | "pledge_sin" => {
                if let Some(Value::Float(f)) = args.first() {
                    ExecOutcome::Value(Value::Float(f.sin()))
                } else {
                    ExecOutcome::Value(Value::Float(0.0))
                }
            }
            "cos" | "c_cos" | "pledge_cos" => {
                if let Some(Value::Float(f)) = args.first() {
                    ExecOutcome::Value(Value::Float(f.cos()))
                } else {
                    ExecOutcome::Value(Value::Float(0.0))
                }
            }
            "fabs" | "c_fabs" | "pledge_fabs" => {
                if let Some(Value::Float(f)) = args.first() {
                    ExecOutcome::Value(Value::Float(f.abs()))
                } else {
                    ExecOutcome::Value(Value::Float(0.0))
                }
            }
            "strcmp" | "c_strcmp" => {
                if let (Some(Value::Str(a)), Some(Value::Str(b))) = (args.get(0), args.get(1)) {
                    ExecOutcome::Value(Value::Int(a.cmp(b) as i64))
                } else {
                    ExecOutcome::Value(Value::Int(0))
                }
            }
            "strlen" | "c_strlen" | "pledge_strlen" => {
                if let Some(Value::Str(s)) = args.first() {
                    ExecOutcome::Value(Value::Int(s.len() as i64))
                } else {
                    ExecOutcome::Value(Value::Int(0))
                }
            }
            "printf" => {
                // Simplified printf — just print the first arg if it's a string
                if let Some(Value::Str(s)) = args.first() {
                    print!("{}", s);
                }
                ExecOutcome::Value(Value::Int(0))
            }

            // ─── Phase 7.7: Dynamic loading (dlopen/dlsym) ───
            "p_dl_open" => {
                // p_dl_open(path: str) -> i64 — open a shared library, return handle
                // Returns 0 on failure (simulated in interpreter)
                let path = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                // In interpreter mode, we simulate dlopen by returning a non-zero handle
                // Real dynamic loading happens in C codegen mode
                if path.is_empty() {
                    ExecOutcome::Value(Value::Int(0))
                } else {
                    // Return a simulated handle (non-zero)
                    let handle = self.next_dl_handle;
                    self.next_dl_handle += 1;
                    self.dl_handles.insert(handle, path);
                    ExecOutcome::Value(Value::Int(handle))
                }
            }
            "p_dl_sym" => {
                // p_dl_sym(handle: i64, symbol: str) -> i64 — look up a symbol
                // Returns 0 on failure (simulated in interpreter)
                let handle = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let symbol = self.value_to_string(args.get(1).unwrap_or(&Value::Int(0)));
                if handle == 0 || !self.dl_handles.contains_key(&handle) {
                    ExecOutcome::Value(Value::Int(0))
                } else if symbol.is_empty() {
                    ExecOutcome::Value(Value::Int(0))
                } else {
                    // Return a simulated function pointer (non-zero)
                    ExecOutcome::Value(Value::Int(0x1000 + handle * 0x100))
                }
            }
            "p_dl_close" => {
                // p_dl_close(handle: i64) -> void — close a shared library
                let handle = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                self.dl_handles.remove(&handle);
                ExecOutcome::Continue
            }
            "p_dl_call" => {
                // p_dl_call(func_ptr: i64, arg: i64) -> i64 — call a dynamically loaded function
                // In interpreter mode, this is simulated (returns the arg)
                let arg = match args.get(1) {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                ExecOutcome::Value(Value::Int(arg))
            }

            _ => ExecOutcome::Panic(format!("Unknown runtime function: {}", name)),
        }
    }
}

// ─── Helper functions for Phase 5C/5D builtins ──────────────────────

/// Extract a string from a Value (Str or Ptr to null-terminated string in memory)
impl Interpreter {
    fn arg_string(&self, v: &Value) -> Option<String> {
        match v {
            Value::Str(s) => Some(s.to_string()),
            Value::Ptr(addr) => {
                let mut bytes = Vec::new();
                let mut offset = *addr;
                loop {
                    let b = self.memory.load(offset, 1);
                    if b[0] == 0 { break; }
                    bytes.push(b[0]);
                    offset += 1;
                }
                Some(String::from_utf8_lossy(&bytes).to_string())
            }
            _ => None,
        }
    }
}

/// Simple wildcard pattern matching: ? matches any single char, * matches zero or more chars.
fn wildcard_match(s: &str, pattern: &str) -> bool {
    let s_bytes = s.as_bytes();
    let p_bytes = pattern.as_bytes();
    let s_len = s_bytes.len();
    let p_len = p_bytes.len();
    let mut s_idx = 0usize;
    let mut p_idx = 0usize;
    let mut star_s = None; // position in s when we last saw a *
    let mut star_p = None; // position in p of the last *

    while s_idx < s_len {
        if p_idx < p_len && (p_bytes[p_idx] == b'?' || p_bytes[p_idx] == s_bytes[s_idx]) {
            s_idx += 1;
            p_idx += 1;
        } else if p_idx < p_len && p_bytes[p_idx] == b'*' {
            star_p = Some(p_idx);
            star_s = Some(s_idx);
            p_idx += 1;
        } else if let Some(sp) = star_p {
            p_idx = sp + 1;
            s_idx = star_s.unwrap() + 1;
            star_s = Some(s_idx);
        } else {
            return false;
        }
    }
    while p_idx < p_len && p_bytes[p_idx] == b'*' {
        p_idx += 1;
    }
    p_idx == p_len
}

/// Extract a Vec<usize> from an argument
fn arg_vec_usize(args: &[Value], idx: usize) -> Option<Vec<usize>> {
    match args.get(idx) {
        Some(Value::Vec(v)) => {
            Some(v.borrow().iter().map(|x| match x {
                Value::Int(i) => *i as usize,
                _ => 0,
            }).collect())
        }
        _ => None,
    }
}

/// Extract a Vec<i64> from an argument
fn arg_vec_i64(args: &[Value], idx: usize) -> Option<Vec<i64>> {
    match args.get(idx) {
        Some(Value::Vec(v)) => {
            Some(v.borrow().iter().map(|x| match x {
                Value::Int(i) => *i,
                _ => 0,
            }).collect())
        }
        _ => None,
    }
}

/// Extract a Vec<f64> from an argument
fn arg_vec_f64(args: &[Value], idx: usize) -> Option<Vec<f64>> {
    match args.get(idx) {
        Some(Value::Vec(v)) => {
            Some(v.borrow().iter().map(|x| match x {
                Value::Int(i) => *i as f64,
                Value::Float(f) => *f,
                _ => 0.0,
            }).collect())
        }
        _ => None,
    }
}

/// SIMD binary operation helper
fn simd_binary(args: &[Value], f: impl Fn(f64, f64) -> f64) -> ExecOutcome {
    if let (Some(a), Some(b)) = (arg_vec_f64(args, 0), arg_vec_f64(args, 1)) {
        let result: SmallVec<[Value; 8]> = a.iter().zip(b.iter())
            .map(|(x, y)| Value::Float(f(*x, *y)))
            .collect();
        ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(result))))
    } else {
        ExecOutcome::Panic("simd: expected Vec, Vec".to_string())
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
pub fn interpret(module: IrModule, verbose: bool, release: bool) -> RunResult {
    let mut interp = Interpreter::new(module);
    if verbose {
        interp.verbose = true;
    }
    if release {
        interp.debug_mode = false;
    }
    interp.run()
}
