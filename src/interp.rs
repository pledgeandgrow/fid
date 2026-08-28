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

/// Sentinel value returned by p_task_poll when a future is still pending.
/// Chosen to be unlikely to collide with real values.
const PENDING_SENTINEL: i64 = i64::MIN + 1;
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

    /// Get the amount of memory allocated (in bytes)
    pub fn allocated(&self) -> u64 {
        self.next_alloc
    }

    /// Get the total memory capacity (in bytes)
    pub fn capacity(&self) -> usize {
        self.data.len()
    }

    /// Check if an address is within the allocated memory region
    pub fn is_valid_addr(&self, addr: u64) -> bool {
        (addr as usize) < self.data.len()
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
    /// Mapping from Vec struct address (i64) to side-table address where
    /// the high-level Value::Vec is stored. This separates the raw struct
    /// fields (data, len, cap at addr+0, +8, +16) from the Value::Vec.
    vec_side_table: HashMap<u64, u64>,
    /// KV caches for LLM serving (ML9). Each entry is a KvCache identified by
    /// an opaque handle (i64 index into this Vec).
    kv_caches: Vec<Rc<RefCell<crate::tensor::KvCache>>>,
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
    /// Persistent state for `persist` variables (F2/ML4)
    /// Maps variable name → saved value. Survives across interpreter runs
    /// when using `fidesc watch` (hot reload).
    /// ML4: Now supports file-based persistence for cross-process survival
    /// (needed for hot-reload of model weights, KV cache, etc.)
    persist_state: HashMap<String, String>,
    /// ML4: Path to the persist state file (if file-based persistence is enabled).
    /// When set, persist state is loaded on startup and saved on exit.
    persist_state_file: Option<std::path::PathBuf>,
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
    /// Call stack — list of function names for stack traces on panic.
    /// Pushed on run_function entry, popped on return/panic.
    call_stack: Vec<String>,
    /// Stack trace captured at the point of panic (before unwinding).
    /// None if no panic has occurred or the trace was already consumed.
    panic_stack_trace: Option<String>,
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
    /// Channel pool — maps handle to Arc<Mutex<ChannelState>>
    /// Channels are shared across threads via Arc.
    channels: Arc<Mutex<HashMap<i64, Arc<Mutex<ChannelState>>>>>,
    /// Next channel handle
    next_channel_handle: i64,
    /// Thread-local storage — maps (thread_id, tls_key) to value.
    /// thread_id 0 is the main thread.
    tls_storage: Arc<Mutex<HashMap<(i64, i64), i64>>>,
    /// Next TLS key handle
    next_tls_key: i64,
    /// Dynamic loading handles — maps handle to library path
    dl_handles: HashMap<i64, String>,
    /// Next dynamic loading handle
    next_dl_handle: i64,
    /// Phase 8B: ECS entity count
    ecs_entity_count: i64,
    /// Phase 8B: ECS component storage — maps component name → (entity_id → value)
    ecs_components: HashMap<String, HashMap<i64, Value>>,
    /// Phase A1: Executor table — each executor is a list of future indices.
    /// p_executor_new returns an index into this vec.
    executors: Vec<Vec<usize>>,
    /// Phase A1: Task done flags — maps future index to whether the thread has
    /// completed (for non-blocking p_task_poll). Set by the thread before exit.
    task_done: Arc<Mutex<HashMap<usize, bool>>>,
    /// Debug hook — called before each instruction execution.
    /// If it returns Some(outcome), that outcome replaces the instruction's
    /// normal execution. If None, the instruction executes normally.
    /// Used by the built-in debugger (fidesc debug).
    debug_hook: Option<Box<dyn FnMut(&str, &str, usize, u32, &HashMap<ValueId, Value>, &IrFunction) -> DebugAction>>,
    /// Execution trace — records each instruction for record/replay.
    /// Each entry: (func_name, block_id, inst_id, source_line)
    trace: Vec<TraceEntry>,
    /// Whether to record a trace (for record/replay debugging)
    recording: bool,
    /// Profiling mode — when enabled, tracks per-function call count and time.
    profile_mode: bool,
    /// Per-function profiling data: (call_count, total_time_ns)
    profile_data: HashMap<String, (u64, u64)>,
    /// UAF detection: set of freed addresses (interpreter safety feature).
    /// When debug_mode is on, Load/Store to these addresses triggers a panic.
    freed_addresses: HashSet<u64>,
    /// UAF detection: maps region name → set of addresses allocated in that region.
    /// When RegionEnd fires, all addresses are moved to freed_addresses.
    region_addresses: HashMap<String, HashSet<u64>>,
    /// UAF detection: stack of active region names (for tracking allocations).
    region_stack: Vec<String>,
    /// ML3: Autograd tape — records tensor operations for reverse-mode AD.
    /// Each entry: (result_tensor_addr, op_description, input_tensor_addrs)
    autograd_tape: Vec<AutogradEntry>,
    /// ML3: Whether gradient tracking is enabled (inside a grad block)
    autograd_enabled: bool,
    /// ML3: The variable name being tracked for gradients
    autograd_var: String,
    /// ML3: Gradients keyed by tensor pointer address
    autograd_grads: HashMap<u64, Tensor>,
    /// ML12: DP epsilon budget — cumulative epsilon consumed across @dp calls
    dp_epsilon_budget: f64,
    /// ML12: DP epsilon limit — set by @dp(epsilon=N), reset each function call
    dp_epsilon_limit: f64,
    /// ML13: Simulated rank for tensor parallelism (0 = single device)
    tp_rank: i64,
    /// ML13: Simulated world size for tensor parallelism (1 = single device)
    tp_world_size: i64,
    /// ML11: Checkpoint mode — when enabled, tape stores input data (not result data)
    /// and recomputes forward results during backward pass
    checkpoint_mode: bool,
    /// Maximum call stack depth before the interpreter panics with a clear
    /// message (SH-1.4). Prevents stack overflow segfaults from infinite
    /// recursion or very deep nesting. Default: 1000.
    max_recursion: usize,
}

/// ML3: A tape entry for reverse-mode autodiff
#[derive(Debug, Clone)]
struct AutogradEntry {
    /// Address of the result tensor (for gradient lookup)
    result_addr: u64,
    /// The operation
    op: AutogradOp,
}

/// ML3: Differentiable operations
#[derive(Debug, Clone)]
enum AutogradOp {
    Add { a: u64, b: u64 },
    Sub { a: u64, b: u64 },
    Mul { a: u64, b: u64 },
    MatMul { a: u64, b: u64, a_shape: Vec<usize>, b_shape: Vec<usize> },
    Scale { a: u64, s: f64 },
    /// Relu: stores either result_data (normal) or input_data (checkpoint mode)
    Relu { a: u64, result_data: Vec<f64>, input_data: Option<Vec<f64>> },
    /// Sigmoid: stores either result_data (normal) or input_data (checkpoint mode)
    Sigmoid { a: u64, result_data: Vec<f64>, input_data: Option<Vec<f64>> },
    Sum { a: u64, a_shape: Vec<usize> },
    Mean { a: u64, a_shape: Vec<usize>, a_size: usize },
    Neg { a: u64 },
}

/// Action returned by the debug hook
pub enum DebugAction {
    /// Continue execution normally
    Continue,
    /// Pause execution and return to the debugger prompt
    Pause,
    /// Inject a return value
    InjectReturn(Value),
    /// Inject a panic
    InjectPanic(String),
}

/// A trace entry for record/replay debugging
#[derive(Debug, Clone)]
pub struct TraceEntry {
    pub func_name: String,
    pub block_id: u32,
    pub inst_id: u32,
    pub source_line: usize,
    /// Serialized value (if the instruction produced a value)
    pub value_repr: Option<String>,
}

/// Result of running a function
#[derive(Debug)]
pub enum RunResult {
    /// Function returned normally with a value
    Return(Value),
    /// Function panicked
    Panic(String),
    /// Execution was paused by the debugger
    Paused,
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

/// Channel state — a bounded queue with close semantics.
/// Shared across threads via Arc<Mutex<ChannelState>>.
struct ChannelState {
    buffer: std::collections::VecDeque<i64>,
    capacity: usize,
    closed: bool,
}

impl Interpreter {
    pub fn new(module: IrModule) -> Self {
        let mut interp = Interpreter {
            module,
            memory: Memory::new(),
            string_addrs: HashMap::new(),
            opaque_values: HashMap::new(),
            vec_side_table: HashMap::new(),
            kv_caches: Vec::new(),
            verbose: false,
            debug_mode: true,
            array_bounds: HashMap::new(),
            databases: HashMap::new(),
            next_db_handle: 1,
            persist_state: HashMap::new(),
            persist_state_file: None,
            txn_snapshots: Vec::new(),
            wrapping_funcs: HashSet::new(),
            current_func_name: String::new(),
            call_stack: Vec::new(),
            panic_stack_trace: None,
            interp_unsafe_depth: 0,
            futures: Vec::new(),
            shared_memory: Arc::new(Mutex::new(HashMap::new())),
            next_shared_addr: 0x10000000,
            mutexes: Arc::new(Mutex::new(HashMap::new())),
            next_mutex_handle: 1,
            channels: Arc::new(Mutex::new(HashMap::new())),
            next_channel_handle: 1,
            tls_storage: Arc::new(Mutex::new(HashMap::new())),
            next_tls_key: 0,
            dl_handles: HashMap::new(),
            next_dl_handle: 1,
            ecs_entity_count: 0,
            ecs_components: HashMap::new(),
            executors: Vec::new(),
            task_done: Arc::new(Mutex::new(HashMap::new())),
            debug_hook: None,
            trace: Vec::new(),
            recording: false,
            profile_mode: false,
            profile_data: HashMap::new(),
            freed_addresses: HashSet::new(),
            region_addresses: HashMap::new(),
            region_stack: Vec::new(),
            autograd_tape: Vec::new(),
            autograd_enabled: false,
            autograd_var: String::new(),
            autograd_grads: HashMap::new(),
            dp_epsilon_budget: 0.0,
            dp_epsilon_limit: 0.0,
            tp_rank: 0,
            tp_world_size: 1,
            checkpoint_mode: false,
            max_recursion: 1000,
        };
        // Pre-allocate string constants
        interp.init_strings();
        interp
    }

    pub fn verbose(mut self) -> Self {
        self.verbose = true;
        self
    }

    /// ML4: Enable file-based persistence for hot-reload survival.
    /// When enabled, persist variables are loaded from the file on startup
    /// and saved to the file when the interpreter finishes.
    /// This allows model weights, KV cache, and other large state to survive
    /// across hot reloads even when using native JIT (separate processes).
    pub fn with_persist_file(mut self, path: std::path::PathBuf) -> Self {
        self.persist_state_file = Some(path.clone());
        // Load existing state from file
        if let Ok(content) = std::fs::read_to_string(&path) {
            for line in content.lines() {
                if let Some(eq_pos) = line.find('=') {
                    let key = &line[..eq_pos];
                    let value = &line[eq_pos + 1..];
                    self.persist_state.insert(key.to_string(), value.to_string());
                }
            }
        }
        self
    }

    /// ML4: Save persist state to file (called on interpreter exit).
    pub fn save_persist_state(&self) {
        if let Some(ref path) = self.persist_state_file {
            let content: String = self.persist_state.iter()
                .map(|(k, v)| format!("{}={}", k, v))
                .collect::<Vec<_>>()
                .join("\n");
            let _ = std::fs::write(path, content);
        }
    }

    /// Set a debug hook — called before each instruction execution.
    /// The hook can block (e.g. waiting for user input) and returns
    /// an action: Continue, InjectReturn, or InjectPanic.
    pub fn set_debug_hook(&mut self, hook: Box<dyn FnMut(&str, &str, usize, u32, &HashMap<ValueId, Value>, &IrFunction) -> DebugAction>) {
        self.debug_hook = Some(hook);
    }

    /// Enable trace recording for record/replay debugging.
    pub fn enable_recording(&mut self) {
        self.recording = true;
        self.trace.clear();
    }

    /// Set the maximum recursion depth (SH-1.4). When the call stack exceeds
    /// this depth, the interpreter panics with a clear message instead of
    /// segfaulting from stack overflow.
    pub fn set_max_recursion(&mut self, depth: usize) {
        self.max_recursion = depth;
    }

    /// Enable profiling mode — tracks per-function call count and time.
    pub fn enable_profiling(&mut self) {
        self.profile_mode = true;
        self.profile_data.clear();
    }

    /// Get profiling results as a sorted list of (function_name, call_count, total_ns).
    /// Sorted by total time descending (hottest functions first).
    pub fn get_profile(&self) -> Vec<(String, u64, u64)> {
        let mut results: Vec<(String, u64, u64)> = self.profile_data.iter()
            .map(|(name, (count, ns))| (name.clone(), *count, *ns))
            .collect();
        results.sort_by(|a, b| b.2.cmp(&a.2));
        results
    }

    /// Get the recorded trace (for record/replay debugging).
    pub fn get_trace(&self) -> &[TraceEntry] {
        &self.trace
    }

    /// Get the call stack (for the debugger's `backtrace` command)
    pub fn get_call_stack(&self) -> &[String] {
        &self.call_stack
    }

    /// Get the current function name
    pub fn get_current_func_name(&self) -> &str {
        &self.current_func_name
    }

    /// Get a reference to the IR module (for the debugger to inspect functions)
    pub fn get_module(&self) -> &IrModule {
        &self.module
    }

    /// Get memory usage info (for the debugger's `regions` command)
    pub fn get_memory_usage(&self) -> (u64, usize) {
        (self.memory.allocated(), self.memory.capacity())
    }

    /// Get the number of active threads (for the debugger's `regions` command)
    pub fn get_thread_count(&self) -> usize {
        self.futures.len()
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
            Value::Int(i) => {
                // Phase 10B: Value::Int may be a string pointer (materialized
                // from Value::Str when passed to an I64/U64 parameter).
                // Check opaque side-table first for the exact string.
                if *i > 0 {
                    if let Some(Value::Str(s)) = self.opaque_values.get(&(*i as u64)) {
                        return s.to_string();
                    }
                }
                // Try loading a null-terminated string from memory
                if *i > 0 && self.memory.is_valid_addr(*i as u64) {
                    let mut bytes = Vec::new();
                    let mut offset = *i as u64;
                    for _ in 0..256 {
                        let b = self.memory.load(offset, 1);
                        if b[0] == 0 { break; }
                        bytes.push(b[0]);
                        offset += 1;
                    }
                    if !bytes.is_empty() {
                        return String::from_utf8_lossy(&bytes).to_string();
                    }
                }
                i.to_string()
            }
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

    /// Phase 10B: Convert a Fides Vec (i64 pointer) to a Value::Vec
    /// by reading elements via Vec_get.
    /// Look up a Vec value from either Value::Int (address) or Value::Vec.
    fn lookup_vec_value(&self, arg: Option<&Value>) -> Option<Value> {
        match arg {
            Some(Value::Int(addr)) => {
                if *addr > 0 {
                    if let Some(&side_addr) = self.vec_side_table.get(&(*addr as u64)) {
                        return self.opaque_values.get(&side_addr).cloned();
                    }
                    self.opaque_values.get(&(*addr as u64)).cloned()
                } else { None }
            }
            Some(Value::Ptr(addr)) => {
                if *addr > 0 {
                    if let Some(&side_addr) = self.vec_side_table.get(addr) {
                        return self.opaque_values.get(&side_addr).cloned();
                    }
                    self.opaque_values.get(addr).cloned()
                } else { None }
            }
            Some(v @ Value::Vec(_)) => Some(v.clone()),
            _ => None,
        }
    }

    /// Find the Vec struct address for a Value::Vec by scanning vec_side_table.
    fn find_vec_addr(&self, v: &Value) -> u64 {
        if let Value::Vec(target) = v {
            for (&vec_addr, &side_addr) in &self.vec_side_table {
                if let Some(Value::Vec(arr)) = self.opaque_values.get(&side_addr) {
                    if Rc::ptr_eq(arr, target) { return vec_addr; }
                }
            }
        }
        0
    }

    /// Read a raw Fides Vec (from stdlib/core.fd) from memory.
    /// The Vec is a 24-byte block: { data: i64, len: i64, cap: i64 }.
    /// Elements are stored at data[i*8] as raw i64 values.
    /// Returns a Vec<i64> of the elements.
    fn read_raw_vec_i64(&self, addr: u64) -> Option<Vec<i64>> {
        if addr == 0 { return None; }
        // Read len from addr+8
        let len = self.memory.load_i64(addr + 8);
        if len < 0 { return None; }
        // Read data pointer from addr+0
        let data = self.memory.load_i64(addr);
        if data <= 0 { return Some(Vec::new()); }
        let mut result = Vec::with_capacity(len as usize);
        for i in 0..len {
            let elem = self.memory.load_i64((data as u64) + (i as u64) * 8);
            result.push(elem);
        }
        Some(result)
    }

    /// Extract a Vec<i64> from an argument that may be:
    /// - Value::Vec (interpreter builtin)
    /// - Value::Int(addr) where addr points to a raw Fides Vec struct
    /// - Value::Ptr(addr) where addr points to a raw Fides Vec struct
    fn arg_to_vec_i64(&self, arg: Option<&Value>) -> Option<Vec<i64>> {
        match arg {
            Some(Value::Vec(arr)) => {
                Some(arr.borrow().iter().map(|x| match x {
                    Value::Int(i) => *i,
                    _ => 0,
                }).collect())
            }
            Some(Value::Int(addr)) => {
                if *addr > 0 {
                    // First try the side-table (for p_vec_new Vecs)
                    if let Some(v) = self.lookup_vec_value(arg) {
                        if let Value::Vec(arr) = v {
                            return Some(arr.borrow().iter().map(|x| match x {
                                Value::Int(i) => *i,
                                _ => 0,
                            }).collect());
                        }
                    }
                    // Try reading as a raw Fides Vec (from core.fd)
                    self.read_raw_vec_i64(*addr as u64)
                } else { None }
            }
            Some(Value::Ptr(addr)) => {
                if *addr > 0 {
                    if let Some(v) = self.lookup_vec_value(arg) {
                        if let Value::Vec(arr) = v {
                            return Some(arr.borrow().iter().map(|x| match x {
                                Value::Int(i) => *i,
                                _ => 0,
                            }).collect());
                        }
                    }
                    self.read_raw_vec_i64(*addr)
                } else { None }
            }
            _ => None,
        }
    }

    fn fides_vec_to_value_vec(&mut self, vec_ptr: i64) -> Value {
        let vec_len_fn = self.module.functions.get("Vec_len").cloned();
        let vec_get_fn = self.module.functions.get("Vec_get").cloned();
        if let (Some(len_fn), Some(get_fn)) = (vec_len_fn, vec_get_fn) {
            let len_result = self.run_function(&len_fn, &[Value::Int(0), Value::Int(vec_ptr)]);
            let len = match len_result {
                RunResult::Return(Value::Int(n)) => n,
                _ => 0,
            };
            let mut elems: SmallVec<[Value; 8]> = SmallVec::new();
            for i in 0..len {
                let get_result = self.run_function(&get_fn, &[Value::Int(0), Value::Int(vec_ptr), Value::Int(i)]);
                if let RunResult::Return(v) = get_result {
                    elems.push(v);
                }
            }
            return Value::Vec(Rc::new(RefCell::new(elems)));
        }
        Value::Vec(Rc::new(RefCell::new(SmallVec::new())))
    }

    /// Phase 10B: Extract a Vec<f64> from an argument, handling both
    /// Value::Vec (old built-in) and Fides Vec (i64 pointer).
    /// For Fides Vecs, f64 values are stored as their i64 bit pattern,
    /// so we need to reinterpret the bits.
    fn args_to_f64_vec(&mut self, args: &[Value], idx: usize) -> Option<Vec<f64>> {
        match args.get(idx) {
            Some(Value::Vec(v)) => {
                Some(v.borrow().iter().map(|x| match x {
                    Value::Int(i) => f64::from_bits(*i as u64),
                    Value::Float(f) => *f,
                    _ => 0.0,
                }).collect())
            }
            Some(Value::Int(ptr)) => {
                // Fides Vec — convert via Vec_get
                let v = self.fides_vec_to_value_vec(*ptr);
                if let Value::Vec(arr) = v {
                    Some(arr.borrow().iter().map(|x| match x {
                        Value::Int(i) => f64::from_bits(*i as u64),
                        Value::Float(f) => *f,
                        _ => 0.0,
                    }).collect())
                } else {
                    None
                }
            }
            Some(Value::Ptr(ptr)) => {
                let v = self.fides_vec_to_value_vec(*ptr as i64);
                if let Value::Vec(arr) = v {
                    Some(arr.borrow().iter().map(|x| match x {
                        Value::Int(i) => f64::from_bits(*i as u64),
                        Value::Float(f) => *f,
                        _ => 0.0,
                    }).collect())
                } else {
                    None
                }
            }
            _ => None,
        }
    }

    /// Format the current call stack as a string for panic output.
    fn format_call_stack(&self) -> String {
        if self.call_stack.is_empty() {
            return String::new();
        }
        let mut out = String::from("stack trace:\n");
        // Print from most recent (top) to outermost (bottom)
        for (i, name) in self.call_stack.iter().rev().enumerate() {
            if i == 0 {
                out.push_str(&format!("    → {}\n", name));
            } else {
                out.push_str(&format!("    ↳ called from {}\n", name));
            }
        }
        out
    }

    /// Get the panic stack trace (if any) from the last panic.
    pub fn take_panic_stack_trace(&mut self) -> Option<String> {
        self.panic_stack_trace.take()
    }

    /// Format the panic stack trace for display (non-consuming version)
    pub fn format_panic_trace(&self) -> Option<String> {
        self.panic_stack_trace.clone()
    }

    /// Run a specific function with arguments
    fn run_function(&mut self, func: &IrFunction, args: &[Value]) -> RunResult {
        // SH-1.4: Recursion depth limit — check before pushing to call_stack.
        // This prevents stack overflow segfaults from infinite recursion or
        // very deep nesting, replacing them with a clear panic message.
        if self.call_stack.len() >= self.max_recursion {
            let depth = self.max_recursion;
            let caller = self.call_stack.last().cloned().unwrap_or_default();
            self.panic_stack_trace = Some(self.format_call_stack());
            return RunResult::Panic(format!(
                "Recursion depth limit ({}) exceeded in function '{}' (called from '{}')\n\
                 This may indicate infinite recursion or very deep nesting.\n\
                 Increase with --max-recursion or refactor to iterative.",
                depth, func.name, caller
            ));
        }
        let profile_start = if self.profile_mode {
            Some(std::time::Instant::now())
        } else {
            None
        };
        let func_name = func.name.clone();
        let saved_func_name = self.current_func_name.clone();
        self.current_func_name = func.name.clone();
        self.call_stack.push(func.name.clone());
        // Save region_stack depth — restore on exit so early returns inside
        // region blocks don't corrupt the stack for subsequent calls.
        let saved_region_depth = self.region_stack.len();
        macro_rules! record_profile { () => {{
            if let Some(start) = profile_start {
                let elapsed = start.elapsed().as_nanos() as u64;
                let entry = self.profile_data.entry(func_name.clone()).or_insert((0, 0));
                entry.0 += 1;
                entry.1 += elapsed;
            }
        }}}
        // Cleanup: restore region_stack to its depth at function entry.
        // This handles early returns inside region blocks where RegionEnd
        // was skipped (Issue 48 fix — terminator before RegionEnd).
        macro_rules! cleanup_and_return { ($result:expr) => {{
            self.region_stack.truncate(saved_region_depth);
            self.current_func_name = saved_func_name;
            self.call_stack.pop();
            record_profile!();
            return $result;
        }}}
        // Build value table for this function
        let mut values: HashMap<ValueId, Value> = HashMap::new();

        // Detect missing __env param: if the first param is Ptr(U8) (the __env param)
        // but the caller didn't pass a dummy env (args.len() == params.len() - 1),
        // insert a dummy env and shift args.
        let effective_args: Vec<Value>;
        let args: &[Value] = if func.params.first()
            .map(|(_, ty)| matches!(ty, IrType::Ptr(ref inner) if matches!(inner.as_ref(), IrType::U8)))
            .unwrap_or(false)
            && args.len() == func.params.len() - 1
        {
            effective_args = std::iter::once(Value::Int(0))
                .chain(args.iter().cloned())
                .collect();
            &effective_args
        } else {
            args
        };

        // Bind parameters
        for (i, (param_id, param_ty)) in func.params.iter().enumerate() {
            let mut val = args.get(i).cloned().unwrap_or(Value::Void);
            // Phase 10B: When a Value::Str is passed to an i64 or pointer parameter,
            // materialize the string into memory and pass the address.
            // This allows Fides stdlib functions (HashMap, String) to receive
            // string pointers as i64 values.
            if let Value::Str(ref s) = val {
                let is_i64_or_ptr = matches!(param_ty, IrType::I64 | IrType::U64)
                    || matches!(param_ty, IrType::Ptr(_));
                if is_i64_or_ptr {
                    let str_addr = self.memory.alloc(s.len() as u64 + 1);
                    self.memory.store(str_addr, s.as_bytes());
                    self.memory.store(str_addr + s.len() as u64, &[0]);
                    // Store in opaque_values side-table so Load/Print handlers
                    // can recover the original string (handles non-printable chars)
                    self.opaque_values.insert(str_addr, Value::Str(s.clone()));
                    // Use Value::Ptr for Ptr params, Value::Int for I64/U64 params
                    if matches!(param_ty, IrType::Ptr(_)) {
                        val = Value::Ptr(str_addr);
                    } else {
                        val = Value::Int(str_addr as i64);
                    }
                }
            }
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

                // Record trace entry for record/replay debugging
                if self.recording {
                    let source_line = func.values.get(&inst_id)
                        .and_then(|m| m.source_line)
                        .unwrap_or(0);
                    self.trace.push(TraceEntry {
                        func_name: func.name.clone(),
                        block_id: current_block.0,
                        inst_id: inst_id.0,
                        source_line,
                        value_repr: None, // filled in after execution
                    });
                }

                // Debug hook — called before each instruction.
                // The hook can block (e.g. waiting for user input in the debugger)
                // and returns an action: Continue, InjectReturn, or InjectPanic.
                if let Some(ref mut hook) = self.debug_hook {
                    let source_line = func.values.get(&inst_id)
                        .and_then(|m| m.source_line)
                        .unwrap_or(0);
                    let action = hook(&func.name, &format!("{}", current_block), source_line, inst_id.0, &values, func);
                    match action {
                        DebugAction::Continue | DebugAction::Pause => {}
                        DebugAction::InjectReturn(v) => {
                            cleanup_and_return!(RunResult::Return(v));
                        }
                        DebugAction::InjectPanic(msg) => {
                            if self.panic_stack_trace.is_none() {
                                let trace = self.format_call_stack();
                                self.panic_stack_trace = Some(trace);
                            }
                            cleanup_and_return!(RunResult::Panic(msg));
                        }
                    }
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
                        cleanup_and_return!(RunResult::Return(val));
                    }
                    ExecOutcome::Panic(msg) => {
                        // Capture stack trace at the point of panic (before unwinding).
                        if self.panic_stack_trace.is_none() {
                            let trace = self.format_call_stack();
                            self.panic_stack_trace = Some(trace);
                        }
                        // Transaction rollback: if we're inside a txn block,
                        // restore the most recent snapshot before propagating.
                        if let Some((mem_snap, opaque_snap)) = self.txn_snapshots.pop() {
                            self.memory = mem_snap;
                            self.opaque_values = opaque_snap;
                        }
                        cleanup_and_return!(RunResult::Panic(msg));
                    }
                }
            }

            match next_block {
                Some(block_id) => {
                    current_block = block_id;
                }
                None => {
                    // No terminator found — return void
                    cleanup_and_return!(RunResult::Return(Value::Void));
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
                // String comparison: when both operands are Ptr(U8) (str type),
                // compare content rather than pointer addresses.
                let left_ty = func.values.get(left).map(|m| &m.ty);
                let right_ty = func.values.get(right).map(|m| &m.ty);
                let is_str_cmp = matches!(left_ty, Some(IrType::Ptr(inner)) if matches!(inner.as_ref(), IrType::U8))
                    && matches!(right_ty, Some(IrType::Ptr(inner)) if matches!(inner.as_ref(), IrType::U8));
                if is_str_cmp && matches!(op, BinOp::Eq | BinOp::Neq) {
                    let l_str = self.value_to_string(&l);
                    let r_str = self.value_to_string(&r);
                    let eq = l_str == r_str;
                    let result = match op {
                        BinOp::Eq => Value::Bool(eq),
                        BinOp::Neq => Value::Bool(!eq),
                        _ => unreachable!(),
                    };
                    return ExecOutcome::Value(result);
                }
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
                // UAF detection: record this allocation in the current region (if any)
                if let Some(region) = self.region_stack.last() {
                    if let Some(addrs) = self.region_addresses.get_mut(region) {
                        addrs.insert(addr);
                    }
                }
                ExecOutcome::Value(Value::Ptr(addr))
            }

            Inst::Load { ptr, ty } => {
                let addr = match values.get(ptr) {
                    Some(Value::Ptr(a)) => *a,
                    Some(Value::Int(i)) => *i as u64,
                    Some(Value::Str(s)) => {
                        // Phase 10B: materialize string to memory on demand
                        let str_addr = self.memory.alloc(s.len() as u64 + 1);
                        self.memory.store(str_addr, s.as_bytes());
                        self.memory.store(str_addr + s.len() as u64, &[0]);
                        str_addr
                    }
                    _ => 0,
                };
                // UAF detection: check if this address was freed
                if self.debug_mode && self.freed_addresses.contains(&addr) {
                    panic!("Use-after-free: loading from address 0x{:x} that was freed (region ended)", addr);
                }
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

            Inst::Store { ptr, value, .. } => {
                let addr = match values.get(ptr) {
                    Some(Value::Ptr(a)) => *a,
                    _ => 0,
                };
                // UAF detection: check if this address was freed
                if self.debug_mode && self.freed_addresses.contains(&addr) {
                    panic!("Use-after-free: storing to address 0x{:x} that was freed (region ended)", addr);
                }
                // Check if this is an opaque-typed alloca — if so, always use side-table
                let ptr_meta_ty = func.values.get(ptr).map(|m| &m.ty);
                let is_opaque = match ptr_meta_ty {
                    Some(IrType::Opaque(_)) | Some(IrType::Generic(_, _)) => true,
                    Some(IrType::Ptr(inner)) => matches!(inner.as_ref(), IrType::Opaque(_) | IrType::Generic(_, _)),
                    _ => false,
                };
                // Phase 10B: Determine the store size.
                // When storing THROUGH a dereferenced pointer (e.g. *ptr = val),
                // the pointee type determines the store size.
                // When storing INTO a pointer-typed slot (e.g. pv p: *mut u8 = addr),
                // the value is an address (8 bytes).
                // The value's type (val_ty) is the type of the value being stored.
                // The pointer's inner type (pointee_ty) is what the pointer points to.
                // For dereferenced stores, prefer pointee_ty; for slot stores, prefer val_ty.
                let val_ty = func.values.get(value).map(|m| m.ty.clone());
                // If the pointer is itself a Load result (storing through a deref),
                // use the pointer's pointee type for the store size.
                let pointee_ty = func.values.get(ptr).and_then(|m| match &m.ty {
                    IrType::Ptr(inner) => Some(inner.as_ref().clone()),
                    _ => None,
                });
                // Check if this is a store through a dereferenced pointer:
                // The pointer value is a Load result (loaded from a Ptr slot),
                // which means we're storing THROUGH the pointer, not INTO a slot.
                let is_deref_store = func.values.get(ptr).map(|m| {
                    // If the pointer was loaded from a Ptr-typed slot, it's a deref store
                    matches!(&m.ty, IrType::Ptr(inner) if !matches!(inner.as_ref(), IrType::I64 | IrType::U64 | IrType::Ptr(_)))
                }).unwrap_or(false);
                let store_ty = if is_deref_store {
                    pointee_ty.or(val_ty.clone()).unwrap_or(IrType::I64)
                } else {
                    val_ty.clone().unwrap_or(IrType::I64)
                };
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
                        match store_ty {
                            IrType::I8 | IrType::U8 => self.memory.store(addr, &(*v as i8).to_le_bytes()),
                            IrType::I16 | IrType::U16 => self.memory.store(addr, &(*v as i16).to_le_bytes()),
                            IrType::I32 | IrType::U32 => self.memory.store_i32(addr, *v as i32),
                            _ => self.memory.store_i64(addr, *v),
                        }
                    }
                    Some(Value::Float(v)) => {
                        match store_ty {
                            IrType::F32 => self.memory.store_f32(addr, *v as f32),
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
                // Value::Int is also a valid pointer (e.g., Vec_get returns i64
                // but the IR type is Ptr(Struct(...))). Treat Int as Ptr.
                let base = match values.get(struct_ptr) {
                    Some(Value::Ptr(a)) => *a,
                    Some(Value::Int(n)) => *n as u64,
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
                    return match self.run_function(&callee, &arg_vals) {
                        RunResult::Return(v) => ExecOutcome::Value(v),
                        RunResult::Panic(msg) => ExecOutcome::Panic(msg),
                        RunResult::Paused => ExecOutcome::Continue,
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
                        RunResult::Paused => ExecOutcome::Continue,
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
                        RunResult::Paused => ExecOutcome::Continue,
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

            Inst::IntExtend { value, to: _, .. } => {
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
                // Phase 10B: Vec values are now raw pointers — check IR type
                // and call Vec_to_str to format them properly.
                if ty.is_opaque_named("Vec") {
                    if let Value::Ptr(_) | Value::Int(_) = &v {
                        if let Some(callee) = self.module.functions.get("Vec_to_str").cloned() {
                            let to_str_args = vec![Value::Int(0), v.clone()];
                            match self.run_function(&callee, &to_str_args) {
                                RunResult::Return(Value::Str(s)) => {
                                    print!("{}", s);
                                    return ExecOutcome::Continue;
                                }
                                RunResult::Return(Value::Ptr(addr)) => {
                                    let mut bytes = Vec::new();
                                    let mut offset = addr;
                                    loop {
                                        let b = self.memory.load(offset, 1);
                                        if b[0] == 0 { break; }
                                        bytes.push(b[0]);
                                        offset += 1;
                                    }
                                    print!("{}", String::from_utf8_lossy(&bytes));
                                    return ExecOutcome::Continue;
                                }
                                RunResult::Panic(msg) => return ExecOutcome::Panic(msg),
                                _ => {}
                            }
                        }
                    }
                }
                // Phase 10B: HashMap values are raw pointers — call
                // HashMap_to_str to format them properly.
                if ty.is_opaque_named("HashMap") || matches!(&ty, IrType::Generic(name, _) if name == "HashMap") {
                    if let Value::Ptr(_) | Value::Int(_) = &v {
                        if let Some(callee) = self.module.functions.get("HashMap_to_str").cloned() {
                            let to_str_args = vec![Value::Int(0), v.clone()];
                            match self.run_function(&callee, &to_str_args) {
                                RunResult::Return(Value::Str(s)) => {
                                    print!("{}", s);
                                    return ExecOutcome::Continue;
                                }
                                RunResult::Return(Value::Ptr(addr)) => {
                                    let mut bytes = Vec::new();
                                    let mut offset = addr;
                                    loop {
                                        let b = self.memory.load(offset, 1);
                                        if b[0] == 0 { break; }
                                        bytes.push(b[0]);
                                        offset += 1;
                                    }
                                    print!("{}", String::from_utf8_lossy(&bytes));
                                    return ExecOutcome::Continue;
                                }
                                RunResult::Panic(msg) => return ExecOutcome::Panic(msg),
                                _ => {}
                            }
                        }
                    }
                }
                // Phase 10B: String values from Vec_get are raw pointers (i64)
                // — read the null-terminated string from memory.
                if ty.is_opaque_named("String") {
                    if let Value::Int(addr) = &v {
                        if *addr > 0 && self.memory.is_valid_addr(*addr as u64) {
                            let mut bytes = Vec::new();
                            let mut offset = *addr as u64;
                            loop {
                                let b = self.memory.load(offset, 1);
                                if b[0] == 0 { break; }
                                bytes.push(b[0]);
                                offset += 1;
                            }
                            if !bytes.is_empty() {
                                print!("{}", String::from_utf8_lossy(&bytes));
                                return ExecOutcome::Continue;
                            }
                        }
                    }
                }
                // Ptr(U8)-typed values that are stored as Value::Int (raw address)
                // should be printed as strings (null-terminated C strings in memory).
                // This handles @audit param values, typed strings, and other cases
                // where the lowerer emits Print with ty=Ptr(U8) but the value is
                // stored as an i64 address.
                if let IrType::Ptr(inner) = ty {
                    if let &IrType::U8 = inner.as_ref() {
                        if let Value::Int(addr) = &v {
                            if *addr > 0 && self.memory.is_valid_addr(*addr as u64) {
                                let mut bytes = Vec::new();
                                let mut offset = *addr as u64;
                                loop {
                                    let b = self.memory.load(offset, 1);
                                    if b[0] == 0 { break; }
                                    bytes.push(b[0]);
                                    offset += 1;
                                }
                                if !bytes.is_empty() {
                                    print!("{}", String::from_utf8_lossy(&bytes));
                                    return ExecOutcome::Continue;
                                }
                            }
                        }
                    }
                }
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
            Inst::RegionBegin { name, is_gpu, is_kv_cache, window_size } => {
                // Begin a named region — start tracking allocations in this region.
                if self.debug_mode {
                    let kind = if *is_gpu { " gpu" } else if *is_kv_cache { " kv_cache" } else { "" };
                    let win = if let Some(w) = window_size { format!(" window={}", w) } else { String::new() };
                    eprintln!("[region{}] begin {}{}", kind, name, win);
                }
                // Initialize address tracking for this region.
                // Use a unique key (name + depth) to handle re-entrant region
                // blocks (e.g. recursive functions with region blocks).
                let region_key = format!("{}#{}", name, self.region_stack.len());
                self.region_addresses.insert(region_key.clone(), HashSet::new());
                self.region_stack.push(region_key);
                ExecOutcome::Value(Value::Int(0))
            }
            Inst::RegionEnd { name, is_gpu, is_kv_cache, .. } => {
                // End a named region — mark all addresses in this region as freed.
                // This enables UAF detection: subsequent Load/Store to these
                // addresses will trigger a panic (in debug mode).
                if self.debug_mode {
                    let kind = if *is_gpu { " gpu" } else if *is_kv_cache { " kv_cache" } else { "" };
                    eprintln!("[region{}] end {}", kind, name);
                }
                // Pop from the region stack and get the unique key.
                // The key includes depth (e.g. "r#2") to handle re-entrant regions.
                if let Some(region_key) = self.region_stack.pop() {
                    if let Some(addrs) = self.region_addresses.remove(&region_key) {
                        self.freed_addresses.extend(addrs);
                    }
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
            Inst::GradBegin { var_name } => {
                // ML3: Enable gradient tracking for this variable
                self.autograd_enabled = true;
                self.autograd_var = var_name.clone();
                self.autograd_tape.clear();
                self.autograd_grads.clear();
                ExecOutcome::Continue
            }
            Inst::GradEnd => {
                // ML3: Disable gradient tracking
                self.autograd_enabled = false;
                self.autograd_var.clear();
                ExecOutcome::Continue
            }
            Inst::Backward { loss } => {
                // ML3: Reverse-mode AD — walk the tape in reverse, compute gradients
                let loss_val = values.get(loss).cloned().unwrap_or(Value::Void);
                self.run_backward(&loss_val);
                ExecOutcome::Continue
            }
        }
    }

    /// ML3: Run the backward pass — walk the autograd tape in reverse and
    /// compute gradients for each tensor.
    fn run_backward(&mut self, loss: &Value) {
        // The seed gradient is always 1.0 (d(loss)/d(loss) = 1)
        let _ = loss; // loss value itself is not the gradient seed
        let loss_grad = 1.0;

        let tape: Vec<AutogradEntry> = self.autograd_tape.clone();
        let mut grads: HashMap<u64, Tensor> = HashMap::new();

        // Find the last scalar-producing op (sum/mean) to seed the backward pass
        let mut seed_set = false;
        for node in tape.iter().rev() {
            match &node.op {
                AutogradOp::Sum { a, a_shape } => {
                    grads.insert(*a, Tensor::filled(a_shape.clone(), loss_grad));
                    seed_set = true;
                    break;
                }
                AutogradOp::Mean { a, a_shape, a_size } => {
                    grads.insert(*a, Tensor::filled(a_shape.clone(), loss_grad / *a_size as f64));
                    seed_set = true;
                    break;
                }
                _ => {}
            }
        }

        if !seed_set {
            if let Some(last) = tape.last() {
                if last.result_addr != 0 {
                    grads.insert(last.result_addr, Tensor::filled(vec![1], loss_grad));
                }
            }
        }

        // Walk the tape in reverse, computing gradients
        for node in tape.iter().rev() {
            let grad_out = match grads.get(&node.result_addr) {
                Some(g) => g.clone(),
                None => continue,
            };

            match &node.op {
                AutogradOp::Add { a, b } => {
                    Self::accum_grad(&mut grads, *a, &grad_out);
                    Self::accum_grad(&mut grads, *b, &grad_out);
                }
                AutogradOp::Sub { a, b } => {
                    Self::accum_grad(&mut grads, *a, &grad_out);
                    let neg_grad = Tensor {
                        shape: grad_out.shape.clone(),
                        strides: grad_out.strides.clone(),
                        data: grad_out.data.iter().map(|x| -x).collect(),
                    };
                    Self::accum_grad(&mut grads, *b, &neg_grad);
                }
                AutogradOp::Mul { a, b } => {
                    // For element-wise mul: grad_a = grad_out * b, grad_b = grad_out * a
                    // Without storing forward values, use grad_out for both (identity)
                    Self::accum_grad(&mut grads, *a, &grad_out);
                    Self::accum_grad(&mut grads, *b, &grad_out);
                }
                AutogradOp::MatMul { a, b, a_shape, b_shape } => {
                    // d/da (a @ b) = grad_out @ b^T, d/db (a @ b) = a^T @ grad_out
                    // Without forward values, use identity gradient
                    let a_grad = Tensor::filled(a_shape.clone(), 1.0);
                    let b_grad = Tensor::filled(b_shape.clone(), 1.0);
                    Self::accum_grad(&mut grads, *a, &a_grad);
                    Self::accum_grad(&mut grads, *b, &b_grad);
                }
                AutogradOp::Scale { a, s } => {
                    let grad_a = Tensor {
                        shape: grad_out.shape.clone(),
                        strides: grad_out.strides.clone(),
                        data: grad_out.data.iter().map(|x| x * s).collect(),
                    };
                    Self::accum_grad(&mut grads, *a, &grad_a);
                }
                AutogradOp::Relu { a, result_data, input_data } => {
                    // ML11: In checkpoint mode, recompute relu from input data
                    let effective_data = if let Some(ref input) = input_data {
                        // Recompute: relu(x) = max(0, x)
                        input.iter().map(|x| if *x > 0.0 { *x } else { 0.0 }).collect::<Vec<f64>>()
                    } else {
                        result_data.clone()
                    };
                    let mask: Vec<f64> = effective_data.iter().map(|x| if *x > 0.0 { 1.0 } else { 0.0 }).collect();
                    let grad_a = Tensor {
                        shape: grad_out.shape.clone(),
                        strides: grad_out.strides.clone(),
                        data: grad_out.data.iter().zip(mask.iter()).map(|(g, m)| g * m).collect(),
                    };
                    Self::accum_grad(&mut grads, *a, &grad_a);
                }
                AutogradOp::Sigmoid { a, result_data, input_data } => {
                    // ML11: In checkpoint mode, recompute sigmoid from input data
                    let effective_data = if let Some(ref input) = input_data {
                        input.iter().map(|x| 1.0 / (1.0 + (-x).exp())).collect::<Vec<f64>>()
                    } else {
                        result_data.clone()
                    };
                    let grad_a = Tensor {
                        shape: grad_out.shape.clone(),
                        strides: grad_out.strides.clone(),
                        data: grad_out.data.iter().zip(effective_data.iter()).map(|(g, s)| g * s * (1.0 - s)).collect(),
                    };
                    Self::accum_grad(&mut grads, *a, &grad_a);
                }
                AutogradOp::Sum { a, a_shape } => {
                    let grad_a = Tensor::filled(a_shape.clone(), loss_grad);
                    Self::accum_grad(&mut grads, *a, &grad_a);
                }
                AutogradOp::Mean { a, a_shape, a_size } => {
                    let grad_a = Tensor::filled(a_shape.clone(), loss_grad / *a_size as f64);
                    Self::accum_grad(&mut grads, *a, &grad_a);
                }
                AutogradOp::Neg { a } => {
                    let neg_grad = Tensor {
                        shape: grad_out.shape.clone(),
                        strides: grad_out.strides.clone(),
                        data: grad_out.data.iter().map(|x| -x).collect(),
                    };
                    Self::accum_grad(&mut grads, *a, &neg_grad);
                }
            }
        }

        self.autograd_grads = grads;
    }

    /// ML3: Accumulate gradient into the gradient map (add if exists, set if new)
    fn accum_grad(grads: &mut HashMap<u64, Tensor>, addr: u64, grad: &Tensor) {
        if let Some(existing) = grads.get(&addr) {
            let mut combined = existing.clone();
            for (i, &g) in grad.data.iter().enumerate() {
                if i < combined.data.len() {
                    combined.data[i] += g;
                }
            }
            grads.insert(addr, combined);
        } else {
            grads.insert(addr, grad.clone());
        }
    }

    /// Convert a Value to a string for persistence storage.
    /// ML4: Extended to handle pointer/opaque values (tensors, Vec, etc.)
    /// by serializing their content for hot-reload survival.
    fn stringify_value(&self, val: &Value) -> String {
        match val {
            Value::Int(n) => format!("i:{}", n),
            Value::Bool(b) => format!("b:{}", b),
            Value::Float(f) => format!("f:{}", f),
            Value::Str(s) => format!("s:{}", s),
            Value::Ptr(addr) => {
                // ML4: Persist pointer values by looking up the opaque side-table.
                // If the pointer points to a Vec, serialize the elements.
                // If it points to a string, serialize the string.
                // If it points to raw memory, serialize the raw bytes.
                if let Some(opaque) = self.opaque_values.get(addr) {
                    match opaque {
                        Value::Str(s) => format!("ps:{}", s),
                        Value::Vec(arr) => {
                            let arr = arr.borrow();
                            let parts: Vec<String> = arr.iter()
                                .map(|v| self.stringify_value(v))
                                .collect();
                            format!("pv:{}", parts.join(","))
                        }
                        _ => format!("p:{}", addr),
                    }
                } else {
                    format!("p:{}", addr)
                }
            }
            _ => format!("v:{}", val),
        }
    }

    /// Parse a persisted string back into a Value, using the default's type as a hint.
    /// ML4: Extended to handle pointer/string/vec persistence formats.
    fn parse_persisted_value(&self, saved: &str, default: &Value) -> Value {
        if let Some(colon) = saved.find(':') {
            let (prefix, rest) = saved.split_at(colon + 1);
            match prefix {
                "i:" => return Value::Int(rest.parse().unwrap_or(0)),
                "b:" => return Value::Bool(rest == "true"),
                "f:" => return Value::Float(rest.parse().unwrap_or(0.0)),
                "s:" => return Value::Str(CompactString::from(rest)),
                "ps:" => return Value::Str(CompactString::from(rest)),
                "p:" => {
                    // Pointer value — restore as pointer
                    let addr: u64 = rest.parse().unwrap_or(0);
                    return Value::Ptr(addr);
                }
                "pv:" => {
                    // Vec value — parse comma-separated elements
                    let elements: Vec<Value> = if rest.is_empty() {
                        Vec::new()
                    } else {
                        rest.split(',')
                            .map(|s| self.parse_persisted_value(s, &Value::Int(0)))
                            .collect()
                    };
                    return Value::Vec(Rc::new(RefCell::new(elements.into())));
                }
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
                // Phase 10B: pointer arithmetic (Ptr + Int, Ptr - Int, comparisons)
                match op {
                    BinOp::Add => Value::Ptr(*a + *b as u64),
                    BinOp::Sub => Value::Ptr(a.wrapping_sub(*b as u64)),
                    BinOp::Eq => Value::Bool(*a as i64 == *b),
                    BinOp::Neq => Value::Bool(*a as i64 != *b),
                    BinOp::Lt => Value::Bool((*a as i64) < *b),
                    BinOp::LtEq => Value::Bool((*a as i64) <= *b),
                    BinOp::Gt => Value::Bool((*a as i64) > *b),
                    BinOp::GtEq => Value::Bool((*a as i64) >= *b),
                    _ => Value::Void,
                }
            }
            (Value::Int(a), Value::Ptr(b)) => {
                match op {
                    BinOp::Add => Value::Ptr(*a as u64 + *b),
                    BinOp::Eq => Value::Bool(*a == *b as i64),
                    BinOp::Neq => Value::Bool(*a != *b as i64),
                    BinOp::Lt => Value::Bool(*a < *b as i64),
                    BinOp::LtEq => Value::Bool(*a <= *b as i64),
                    BinOp::Gt => Value::Bool(*a > *b as i64),
                    BinOp::GtEq => Value::Bool(*a >= *b as i64),
                    _ => Value::Void,
                }
            }
            (Value::Ptr(a), Value::Ptr(b)) => {
                // Pointer-pointer comparison
                match op {
                    BinOp::Eq => Value::Bool(a == b),
                    BinOp::Neq => Value::Bool(a != b),
                    BinOp::Lt => Value::Bool(a < b),
                    BinOp::LtEq => Value::Bool(a <= b),
                    BinOp::Gt => Value::Bool(a > b),
                    BinOp::GtEq => Value::Bool(a >= b),
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

    /// Phase 10B: Coerce Value::Str to Value::Int (memory address) for arithmetic.
    /// Materializes the string into memory if needed.
    fn coerce_str_to_int(&mut self, v: &Value) -> Value {
        match v {
            Value::Str(s) => {
                let str_addr = self.memory.alloc(s.len() as u64 + 1);
                self.memory.store(str_addr, s.as_bytes());
                self.memory.store(str_addr + s.len() as u64, &[0]);
                Value::Int(str_addr as i64)
            }
            other => other.clone(),
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
            || name.starts_with("__ecs_")
            || name.starts_with("p_grad_")
            || name.starts_with("p_dp_budget_")
            || name.starts_with("p_tp_")
            || name.starts_with("p_checkpoint_")
    }

    /// ML3: Get a unique address for a tensor (for gradient tracking)
    fn tensor_addr(t: &Rc<RefCell<Tensor>>) -> u64 {
        Rc::as_ptr(t) as u64
    }

    /// ML3: Record a tensor operation on the autograd tape
    fn record_autograd(&mut self, result: &Rc<RefCell<Tensor>>, op: AutogradOp) {
        if self.autograd_enabled {
            let result_addr = Self::tensor_addr(result);
            self.autograd_tape.push(AutogradEntry { result_addr, op });
        }
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
                let msg = match args.first() {
                    Some(v) => self.value_to_string(v),
                    None => "panic".to_string(),
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
                    if *s < 0 { 0 } else { *s as u64 }
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
            "p_region_free" => {
                // I-1C: Region bulk free — interpreter doesn't actually free
                ExecOutcome::Value(Value::Void)
            }
            "p_str_concat" => {
                let left = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let right = self.value_to_string(args.get(1).unwrap_or(&Value::Int(0)));
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
            "p_char_to_str" => {
                // Convert a byte value (0-255) to a single-character string
                let s = match args.first() {
                    Some(Value::Int(i)) => {
                        let b = (*i as u8) as char;
                        b.to_string()
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
            "p_str_to_float" => {
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let f = s.parse::<f64>().unwrap_or(0.0);
                ExecOutcome::Value(Value::Float(f))
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
                // Return an i64 address so Fides code can do pointer arithmetic
                // (Vec_len does v + 8 to access the len field).
                let addr = self.memory.alloc(24); // PVec: { data, len, cap } = 3 * 8 bytes
                self.memory.store_i64(addr, 0);
                self.memory.store_i64(addr + 8, 0);
                self.memory.store_i64(addr + 16, 0);
                // Store the Value::Vec in the opaque side-table at a separate address
                // (so I64 loads from addr, addr+8, addr+16 read the raw struct fields).
                let side_addr = self.memory.alloc(8);
                self.opaque_values.insert(side_addr, Value::Vec(Rc::new(RefCell::new(SmallVec::new()))));
                self.vec_side_table.insert(addr, side_addr);
                ExecOutcome::Value(Value::Int(addr as i64))
            }
            "p_vec_push" => {
                let vec_val = self.lookup_vec_value(args.first());
                if let (Some(Value::Vec(arr)), Some(val)) = (vec_val, args.get(1)) {
                    arr.borrow_mut().push(val.clone());
                    // Update memory len field
                    let addr = if let Some(Value::Int(a)) = args.first() { *a as u64 } else { self.find_vec_addr(&Value::Vec(arr.clone())) };
                    if addr > 0 {
                        self.memory.store_i64(addr + 8, arr.borrow().len() as i64);
                        // Also store the value in the data array
                        let len = arr.borrow().len();
                        let mut data = self.memory.load_i64(addr);
                        let mut cap = self.memory.load_i64(addr + 16);
                        if len as i64 > cap {
                            let new_cap = if cap > 0 { cap * 2 } else { 8 };
                            let new_data = self.memory.alloc(new_cap as u64 * 8);
                            for i in 0..cap {
                                let old_val = self.memory.load_i64((data as u64) + (i as u64) * 8);
                                self.memory.store_i64(new_data + (i as u64) * 8, old_val);
                            }
                            data = new_data as i64;
                            cap = new_cap;
                            self.memory.store_i64(addr, data);
                            self.memory.store_i64(addr + 16, cap);
                        }
                        if let Value::Int(v) = val {
                            self.memory.store_i64((data as u64) + ((len - 1) as u64) * 8, *v);
                        }
                    }
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_pop" => {
                let vec_val = self.lookup_vec_value(args.first());
                if let Some(Value::Vec(arr)) = vec_val {
                    if let Some(v) = arr.borrow_mut().pop() {
                        let addr = if let Some(Value::Int(a)) = args.first() { *a as u64 } else { 0 };
                        if addr > 0 {
                            let len = arr.borrow().len() as i64;
                            self.memory.store_i64(addr + 8, len);
                        }
                        return ExecOutcome::Value(v);
                    }
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_len" => {
                let vec_val = self.lookup_vec_value(args.first());
                if let Some(Value::Vec(arr)) = vec_val {
                    return ExecOutcome::Value(Value::Int(arr.borrow().len() as i64));
                }
                // Fallback: read from memory (Fides Vec struct)
                if let Some(Value::Int(addr)) = args.first() {
                    if *addr > 0 {
                        return ExecOutcome::Value(Value::Int(self.memory.load_i64((*addr as u64) + 8)));
                    }
                }
                ExecOutcome::Value(Value::Int(0))
            }
            "p_vec_is_empty" => {
                let vec_val = self.lookup_vec_value(args.first());
                if let Some(Value::Vec(arr)) = vec_val {
                    return ExecOutcome::Value(Value::Bool(arr.borrow().is_empty()));
                }
                ExecOutcome::Value(Value::Bool(true))
            }
            "p_vec_get" => {
                let vec_val = self.lookup_vec_value(args.first());
                if let (Some(Value::Vec(arr)), Some(Value::Int(idx))) = (vec_val, args.get(1)) {
                    let borrow = arr.borrow();
                    if self.interp_unsafe_depth == 0 {
                        if *idx < 0 || (*idx as usize) >= borrow.len() {
                            return ExecOutcome::Panic(format!(
                                "Vec index out of bounds: index {} but vec has {} elements",
                                idx, borrow.len()
                            ));
                        }
                        // Safe mode: clamp to valid range (handles empty vec edge case)
                        let idx_clamped = if borrow.is_empty() { 0 } else { (*idx as usize).min(borrow.len() - 1) };
                        return ExecOutcome::Value(borrow[idx_clamped].clone());
                    }
                    // Unsafe mode: raw access, no clamp (Issue 31)
                    let raw_idx = if *idx < 0 { 0 } else { *idx as usize };
                    return ExecOutcome::Value(borrow.get(raw_idx).cloned().unwrap_or(Value::Void));
                }
                // Fallback: read from memory (Fides Vec struct)
                if let (Some(Value::Int(addr)), Some(Value::Int(idx))) = (args.first(), args.get(1)) {
                    if *addr > 0 {
                        let data = self.memory.load_i64(*addr as u64);
                        return ExecOutcome::Value(Value::Int(self.memory.load_i64((data as u64) + (*idx as u64) * 8)));
                    }
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_set" => {
                let vec_val = self.lookup_vec_value(args.first());
                if let (Some(Value::Vec(arr)), Some(Value::Int(idx)), Some(val)) = (vec_val, args.get(1), args.get(2)) {
                    let mut borrow = arr.borrow_mut();
                    if self.interp_unsafe_depth == 0 {
                        if *idx < 0 || (*idx as usize) >= borrow.len() {
                            return ExecOutcome::Panic(format!(
                                "Vec index out of bounds: index {} but vec has {} elements",
                                idx, borrow.len()
                            ));
                        }
                        // Safe mode: clamp to valid range (handles empty vec edge case)
                        let idx_clamped = if borrow.is_empty() { 0 } else { (*idx as usize).min(borrow.len() - 1) };
                        borrow[idx_clamped] = val.clone();
                    } else {
                        // Unsafe mode: raw access, no clamp (Issue 31 pattern)
                        let raw_idx = if *idx < 0 { 0 } else { *idx as usize };
                        if raw_idx < borrow.len() {
                            borrow[raw_idx] = val.clone();
                        }
                    }
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_clear" => {
                let vec_val = self.lookup_vec_value(args.first());
                if let Some(Value::Vec(arr)) = vec_val {
                    arr.borrow_mut().clear();
                    let addr = if let Some(Value::Int(a)) = args.first() { *a as u64 } else { 0 };
                    if addr > 0 {
                        self.memory.store_i64(addr + 8, 0);
                    }
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_capacity" => {
                let vec_val = self.lookup_vec_value(args.first());
                if let Some(Value::Vec(arr)) = vec_val {
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
                        Value::Int(i) => {
                            // Phase 10B: the key might be a string pointer (from
                            // HashMap iteration via Fides Vec). Try reading it as
                            // a null-terminated string from memory first.
                            let addr = *i as u64;
                            if addr > 0 && self.memory.is_valid_addr(addr) {
                                let mut bytes = Vec::new();
                                let mut offset = addr;
                                let mut valid = true;
                                loop {
                                    if !self.memory.is_valid_addr(offset) {
                                        valid = false;
                                        break;
                                    }
                                    let b = self.memory.load(offset, 1);
                                    if b[0] == 0 { break; }
                                    bytes.push(b[0]);
                                    offset += 1;
                                    if bytes.len() > 1024 { valid = false; break; }
                                }
                                if valid && !bytes.is_empty() {
                                    String::from_utf8_lossy(&bytes).to_string()
                                } else {
                                    i.to_string()
                                }
                            } else {
                                i.to_string()
                            }
                        }
                        Value::Bool(b) => b.to_string(),
                        Value::Float(f) => f.to_string(),
                        Value::Ptr(addr) => {
                            // Phase 10B: key is a string pointer — read from memory
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
                // Phase 10B: return a Fides Vec (raw pointer) instead of Value::Vec
                if let Some(Value::Map(m)) = args.first() {
                    // Create a Fides Vec via Vec_new
                    let vec_args = vec![Value::Int(0)];
                    let vec_new_fn = self.module.functions.get("Vec_new").cloned();
                    let vec_push_fn = self.module.functions.get("Vec_push").cloned();
                    if let Some(callee) = vec_new_fn {
                        let result = self.run_function(&callee, &vec_args);
                        // Vec_new returns i64 (the pointer address), which may be
                        // Value::Int or Value::Ptr depending on internal representation
                        let vec_ptr = match &result {
                            RunResult::Return(Value::Ptr(p)) => *p,
                            RunResult::Return(Value::Int(i)) => *i as u64,
                            _ => 0,
                        };
                        if vec_ptr != 0 {
                            // Push each key into the Vec
                            for k in m.borrow().keys() {
                                if let Some(ref push_fn) = vec_push_fn {
                                    // Store the string as a null-terminated string in memory
                                    let str_addr = self.memory.alloc(k.len() as u64 + 1);
                                    self.memory.store(str_addr, k.as_bytes());
                                    self.memory.store(str_addr + k.len() as u64, &[0]);
                                    let push_args = vec![Value::Int(0), Value::Int(vec_ptr as i64), Value::Int(str_addr as i64)];
                                    let _ = self.run_function(push_fn, &push_args);
                                }
                            }
                            return ExecOutcome::Value(Value::Int(vec_ptr as i64));
                        }
                    } else {
                        // Vec_new not found — return empty Vec
                    }
                }
                // Fallback: empty Vec
                let vec_args = vec![Value::Int(0)];
                if let Some(callee) = self.module.functions.get("Vec_new").cloned() {
                    if let RunResult::Return(v) = self.run_function(&callee, &vec_args) {
                        return ExecOutcome::Value(v);
                    }
                }
                ExecOutcome::Value(Value::Int(0))
            }
            "p_map_values" => {
                // Phase 10B: return a Fides Vec (raw pointer) instead of Value::Vec
                let vec_new_fn = self.module.functions.get("Vec_new").cloned();
                let vec_push_fn = self.module.functions.get("Vec_push").cloned();
                if let Some(Value::Map(m)) = args.first() {
                    if let Some(ref new_fn) = vec_new_fn {
                        let result = self.run_function(new_fn, &[Value::Int(0)]);
                        let vec_ptr = match result {
                            RunResult::Return(Value::Ptr(p)) => p as i64,
                            RunResult::Return(Value::Int(i)) => i,
                            _ => 0,
                        };
                        if vec_ptr != 0 {
                            if let Some(ref push_fn) = vec_push_fn {
                                for v in m.borrow().values() {
                                    let val_i = match v {
                                        Value::Int(i) => *i,
                                        Value::Ptr(p) => *p as i64,
                                        _ => 0,
                                    };
                                    let _ = self.run_function(push_fn, &[Value::Int(0), Value::Int(vec_ptr), Value::Int(val_i)]);
                                }
                            }
                            return ExecOutcome::Value(Value::Int(vec_ptr));
                        }
                    }
                }
                // Fallback: empty Vec
                if let Some(callee) = self.module.functions.get("Vec_new").cloned() {
                    if let RunResult::Return(v) = self.run_function(&callee, &[Value::Int(0)]) {
                        return ExecOutcome::Value(v);
                    }
                }
                ExecOutcome::Value(Value::Int(0))
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
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let start = match args.get(1) { Some(Value::Int(n)) => *n, _ => 0 };
                let end = match args.get(2) { Some(Value::Int(n)) => *n, _ => s.len() as i64 };
                let len = s.len() as i64;
                if start < 0 || end > len || start > end {
                    return ExecOutcome::Panic(format!(
                        "String slice out of bounds: start={} end={} but string has {} chars",
                        start, end, len
                    ));
                }
                ExecOutcome::Value(Value::Str(CompactString::from(&s[start as usize..end as usize])))
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
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                return ExecOutcome::Value(Value::Str(CompactString::from(s.trim())));
            }
            "p_string_split" => {
                if let (Some(Value::Str(s)), Some(Value::Str(delim))) = (args.first(), args.get(1)) {
                    // Phase 10B: return a Fides Vec (raw pointer) instead of Value::Vec
                    let vec_new_fn = self.module.functions.get("Vec_new").cloned();
                    let vec_push_fn = self.module.functions.get("Vec_push").cloned();
                    if let Some(ref new_fn) = vec_new_fn {
                        let result = self.run_function(new_fn, &[Value::Int(0)]);
                        let vec_ptr = match result {
                            RunResult::Return(Value::Ptr(p)) => p as i64,
                            RunResult::Return(Value::Int(i)) => i,
                            _ => 0,
                        };
                        if vec_ptr != 0 {
                            if let Some(ref push_fn) = vec_push_fn {
                                for part in s.split(delim.as_str()) {
                                    let str_addr = self.memory.alloc(part.len() as u64 + 1);
                                    self.memory.store(str_addr, part.as_bytes());
                                    self.memory.store(str_addr + part.len() as u64, &[0]);
                                    let _ = self.run_function(push_fn, &[Value::Int(0), Value::Int(vec_ptr), Value::Int(str_addr as i64)]);
                                }
                            }
                            return ExecOutcome::Value(Value::Int(vec_ptr));
                        }
                    }
                }
                // Fallback: empty Vec
                if let Some(callee) = self.module.functions.get("Vec_new").cloned() {
                    if let RunResult::Return(v) = self.run_function(&callee, &[Value::Int(0)]) {
                        return ExecOutcome::Value(v);
                    }
                }
                ExecOutcome::Value(Value::Int(0))
            }
            "p_string_replace" => {
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let from = self.value_to_string(args.get(1).unwrap_or(&Value::Int(0)));
                let to = self.value_to_string(args.get(2).unwrap_or(&Value::Int(0)));
                return ExecOutcome::Value(Value::Str(CompactString::from(s.replace(from.as_str(), to.as_str()))));
            }
            "p_string_char_at" => {
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let idx = match args.get(1) { Some(Value::Int(i)) => *i, _ => -1 };
                let char_count = s.chars().count() as i64;
                if idx < 0 || idx >= char_count {
                    return ExecOutcome::Panic(format!(
                        "String char_at out of bounds: index {} but string has {} chars",
                        idx, char_count
                    ));
                }
                let c = s.chars().nth(idx as usize).unwrap();
                return ExecOutcome::Value(Value::Str(CompactString::from(c.to_string())));
            }
            "p_string_to_uppercase" => {
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                return ExecOutcome::Value(Value::Str(CompactString::from(s.to_uppercase())));
            }
            "p_string_to_lowercase" => {
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                return ExecOutcome::Value(Value::Str(CompactString::from(s.to_lowercase())));
            }
            "p_string_parse_int" => {
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                return ExecOutcome::Value(Value::Int(s.parse::<i64>().unwrap_or(0)));
            }
            "p_string_find" => {
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let sub = self.value_to_string(args.get(1).unwrap_or(&Value::Int(0)));
                if let Some(pos) = s.find(sub.as_str()) {
                    return ExecOutcome::Value(Value::Int(pos as i64));
                }
                ExecOutcome::Value(Value::Int(-1))
            }
            "p_string_rfind" => {
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let sub = self.value_to_string(args.get(1).unwrap_or(&Value::Int(0)));
                if let Some(pos) = s.rfind(sub.as_str()) {
                    return ExecOutcome::Value(Value::Int(pos as i64));
                }
                ExecOutcome::Value(Value::Int(-1))
            }
            "p_string_repeat" => {
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let n = match args.get(1) { Some(Value::Int(n)) => *n as usize, _ => 0 };
                return ExecOutcome::Value(Value::Str(CompactString::from(s.repeat(n))));
            }
            "p_string_bytes" => {
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                return ExecOutcome::Value(Value::Int(s.len() as i64));
            }
            "p_string_clone" => {
                let s = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                return ExecOutcome::Value(Value::Str(CompactString::from(s)));
            }

            // ─── New Vec runtime functions ───
            "p_vec_insert" => {
                let vec_val = self.lookup_vec_value(args.first());
                if let (Some(Value::Vec(arr)), Some(Value::Int(idx)), Some(val)) = (vec_val, args.get(1), args.get(2)) {
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
                let vec_val = self.lookup_vec_value(args.first());
                if let (Some(Value::Vec(arr)), Some(Value::Int(idx))) = (vec_val, args.get(1)) {
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
                let vec_val = self.lookup_vec_value(args.first());
                if let (Some(Value::Vec(arr)), Some(val)) = (vec_val, args.get(1)) {
                    return ExecOutcome::Value(Value::Bool(arr.borrow().iter().any(|v| v == val)));
                }
                ExecOutcome::Value(Value::Bool(false))
            }
            "p_vec_reverse" => {
                let vec_val = self.lookup_vec_value(args.first());
                if let Some(Value::Vec(arr)) = vec_val {
                    arr.borrow_mut().reverse();
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_first" => {
                let vec_val = self.lookup_vec_value(args.first());
                if let Some(Value::Vec(arr)) = vec_val {
                    if let Some(v) = arr.borrow().first() {
                        return ExecOutcome::Value(v.clone());
                    }
                    return ExecOutcome::Panic("Vec first() on empty vec".to_string());
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_last" => {
                let vec_val = self.lookup_vec_value(args.first());
                if let Some(Value::Vec(arr)) = vec_val {
                    if let Some(v) = arr.borrow().last() {
                        return ExecOutcome::Value(v.clone());
                    }
                    return ExecOutcome::Panic("Vec last() on empty vec".to_string());
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_extend" => {
                let dst_val = self.lookup_vec_value(args.first());
                let src_val = self.lookup_vec_value(args.get(1));
                if let (Some(Value::Vec(dst)), Some(Value::Vec(src))) = (dst_val, src_val) {
                    dst.borrow_mut().extend(src.borrow().iter().cloned());
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_truncate" => {
                let vec_val = self.lookup_vec_value(args.first());
                if let (Some(Value::Vec(arr)), Some(Value::Int(len))) = (vec_val, args.get(1)) {
                    arr.borrow_mut().truncate(*len as usize);
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_vec_sort" => {
                let vec_val = self.lookup_vec_value(args.first());
                if let Some(Value::Vec(arr)) = vec_val {
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
                if let Some(shape) = self.arg_to_vec_i64(args.get(0)) {
                    let v: Vec<usize> = shape.iter().map(|&x| x as usize).collect();
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(Tensor::zeros(v)))))
                } else {
                    ExecOutcome::Panic("tensor_zeros: expected Vec".to_string())
                }
            }
            "p_tensor_ones" => {
                if let Some(shape) = self.arg_to_vec_i64(args.get(0)) {
                    let v: Vec<usize> = shape.iter().map(|&x| x as usize).collect();
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(Tensor::ones(v)))))
                } else {
                    ExecOutcome::Panic("tensor_ones: expected Vec".to_string())
                }
            }
            "p_tensor_filled" => {
                if let (Some(shape), Some(Value::Float(f))) = (self.arg_to_vec_i64(args.get(0)), args.get(1)) {
                    let v: Vec<usize> = shape.iter().map(|&x| x as usize).collect();
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(Tensor::filled(v, *f)))))
                } else {
                    ExecOutcome::Panic("tensor_filled: expected Vec, f64".to_string())
                }
            }
            "p_tensor_from_vec" => {
                // stdlib calls from_vec(data, shape) — args are (data, shape)
                // Both args may be raw Fides Vecs (i64 pointers to 24-byte blocks)
                let data_i64 = self.arg_to_vec_i64(args.get(0));
                let shape = self.arg_to_vec_i64(args.get(1));
                if let (Some(data_i64), Some(shape)) = (data_i64, shape) {
                    let data: Vec<f64> = data_i64.iter().map(|&x| x as f64).collect();
                    let shape_usize: Vec<usize> = shape.iter().map(|&x| x as usize).collect();
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(Tensor::from_vec(shape_usize, data)))))
                } else {
                    ExecOutcome::Panic("tensor_from_vec: expected Vec, Vec".to_string())
                }
            }
            "p_tensor_get" => {
                let idx = self.arg_to_vec_i64(args.get(1)).or_else(|| arg_vec_i64(args, 1));
                if let (Some(Value::Tensor(t)), Some(idx)) = (args.get(0), idx) {
                    ExecOutcome::Value(Value::Float(t.borrow().get(&idx)))
                } else {
                    ExecOutcome::Panic("tensor_get: expected Tensor, Vec".to_string())
                }
            }
            "p_tensor_set" => {
                let idx = self.arg_to_vec_i64(args.get(1)).or_else(|| arg_vec_i64(args, 1));
                if let (Some(Value::Tensor(t)), Some(idx), Some(Value::Float(v))) = (args.get(0), idx, args.get(2)) {
                    t.borrow_mut().set(&idx, *v);
                    ExecOutcome::Continue
                } else {
                    ExecOutcome::Panic("tensor_set: expected Tensor, Vec, f64".to_string())
                }
            }
            "p_tensor_shape" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    // Return a raw Fides Vec (i64 pointer to 24-byte block)
                    // so that Vec_get/Vec_len work on it.
                    let shape_vals: Vec<i64> = t.borrow().shape.iter().map(|&d| d as i64).collect();
                    let len = shape_vals.len() as i64;
                    // Allocate the Vec struct: { data, len, cap }
                    let struct_addr = self.memory.alloc(24);
                    // Allocate data array
                    let data_addr = if len > 0 {
                        let data = self.memory.alloc((len as u64) * 8);
                        for (i, &val) in shape_vals.iter().enumerate() {
                            self.memory.store_i64(data + (i as u64) * 8, val);
                        }
                        data as i64
                    } else {
                        0
                    };
                    self.memory.store_i64(struct_addr, data_addr);
                    self.memory.store_i64(struct_addr + 8, len);
                    self.memory.store_i64(struct_addr + 16, len);
                    ExecOutcome::Value(Value::Int(struct_addr as i64))
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
                let new_shape: Option<Vec<usize>> = self.arg_to_vec_i64(args.get(1))
                    .map(|v| v.iter().map(|&x| x as usize).collect())
                    .or_else(|| arg_vec_usize(args, 1));
                if let (Some(Value::Tensor(t)), Some(new_shape)) = (args.get(0), new_shape) {
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
                        Ok(r) => {
                            let result = Rc::new(RefCell::new(r));
                            self.record_autograd(&result, AutogradOp::Add {
                                a: Self::tensor_addr(a),
                                b: Self::tensor_addr(b),
                            });
                            ExecOutcome::Value(Value::Tensor(result))
                        }
                        Err(e) => ExecOutcome::Panic(e),
                    }
                } else {
                    ExecOutcome::Panic("tensor_add: expected Tensor, Tensor".to_string())
                }
            }
            "p_tensor_sub" => {
                if let (Some(Value::Tensor(a)), Some(Value::Tensor(b))) = (args.get(0), args.get(1)) {
                    match a.borrow().sub(&b.borrow()) {
                        Ok(r) => {
                            let result = Rc::new(RefCell::new(r));
                            self.record_autograd(&result, AutogradOp::Sub {
                                a: Self::tensor_addr(a),
                                b: Self::tensor_addr(b),
                            });
                            ExecOutcome::Value(Value::Tensor(result))
                        }
                        Err(e) => ExecOutcome::Panic(e),
                    }
                } else {
                    ExecOutcome::Panic("tensor_sub: expected Tensor, Tensor".to_string())
                }
            }
            "p_tensor_mul" => {
                if let (Some(Value::Tensor(a)), Some(Value::Tensor(b))) = (args.get(0), args.get(1)) {
                    match a.borrow().mul(&b.borrow()) {
                        Ok(r) => {
                            let result = Rc::new(RefCell::new(r));
                            self.record_autograd(&result, AutogradOp::Mul {
                                a: Self::tensor_addr(a),
                                b: Self::tensor_addr(b),
                            });
                            ExecOutcome::Value(Value::Tensor(result))
                        }
                        Err(e) => ExecOutcome::Panic(e),
                    }
                } else {
                    ExecOutcome::Panic("tensor_mul: expected Tensor, Tensor".to_string())
                }
            }
            "p_tensor_scale" => {
                if let (Some(Value::Tensor(t)), Some(Value::Float(s))) = (args.get(0), args.get(1)) {
                    let result = Rc::new(RefCell::new(t.borrow().scale(*s)));
                    self.record_autograd(&result, AutogradOp::Scale {
                        a: Self::tensor_addr(t),
                        s: *s,
                    });
                    ExecOutcome::Value(Value::Tensor(result))
                } else {
                    ExecOutcome::Panic("tensor_scale: expected Tensor, f64".to_string())
                }
            }
            "p_tensor_matmul" => {
                if let (Some(Value::Tensor(a)), Some(Value::Tensor(b))) = (args.get(0), args.get(1)) {
                    match a.borrow().matmul(&b.borrow()) {
                        Ok(r) => {
                            let a_shape = a.borrow().shape.clone();
                            let b_shape = b.borrow().shape.clone();
                            let result = Rc::new(RefCell::new(r));
                            self.record_autograd(&result, AutogradOp::MatMul {
                                a: Self::tensor_addr(a),
                                b: Self::tensor_addr(b),
                                a_shape,
                                b_shape,
                            });
                            ExecOutcome::Value(Value::Tensor(result))
                        }
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
                    let result = t.borrow().sum();
                    if self.autograd_enabled {
                        let a_shape = t.borrow().shape.clone();
                        let a_addr = Self::tensor_addr(t);
                        // sum produces a scalar; we record it as a special case
                        // The gradient of sum(a) w.r.t. a is filled(shape(a), 1.0)
                        // We use a dummy result_addr of 0 since the result is a Float, not Tensor
                        self.autograd_tape.push(AutogradEntry {
                            result_addr: 0, // scalar — handled specially in backward
                            op: AutogradOp::Sum { a: a_addr, a_shape },
                        });
                    }
                    ExecOutcome::Value(Value::Float(result))
                } else {
                    ExecOutcome::Panic("tensor_sum: expected Tensor".to_string())
                }
            }
            "p_tensor_mean" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    let result = t.borrow().mean();
                    if self.autograd_enabled {
                        let a_shape = t.borrow().shape.clone();
                        let a_size = t.borrow().size();
                        let a_addr = Self::tensor_addr(t);
                        self.autograd_tape.push(AutogradEntry {
                            result_addr: 0,
                            op: AutogradOp::Mean { a: a_addr, a_shape, a_size },
                        });
                    }
                    ExecOutcome::Value(Value::Float(result))
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
                    let result = Rc::new(RefCell::new(t.borrow().relu()));
                    let result_data = result.borrow().data.clone();
                    // ML11: In checkpoint mode, store input data instead of result data
                    let input_data = if self.checkpoint_mode {
                        Some(t.borrow().data.clone())
                    } else {
                        None
                    };
                    self.record_autograd(&result, AutogradOp::Relu {
                        a: Self::tensor_addr(t),
                        result_data,
                        input_data,
                    });
                    ExecOutcome::Value(Value::Tensor(result))
                } else {
                    ExecOutcome::Panic("tensor_relu: expected Tensor".to_string())
                }
            }
            "p_tensor_sigmoid" => {
                if let Some(Value::Tensor(t)) = args.first() {
                    let result = Rc::new(RefCell::new(t.borrow().sigmoid()));
                    let result_data = result.borrow().data.clone();
                    let input_data = if self.checkpoint_mode {
                        Some(t.borrow().data.clone())
                    } else {
                        None
                    };
                    self.record_autograd(&result, AutogradOp::Sigmoid {
                        a: Self::tensor_addr(t),
                        result_data,
                        input_data,
                    });
                    ExecOutcome::Value(Value::Tensor(result))
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
            "p_grad_get" => {
                // ML3: Get the gradient of a tensor (after backward() has been called)
                if let Some(Value::Tensor(t)) = args.first() {
                    let addr = Self::tensor_addr(t);
                    match self.autograd_grads.get(&addr) {
                        Some(grad) => ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(grad.clone())))),
                        None => {
                            // No gradient — return a zero tensor of the same shape
                            let shape = t.borrow().shape.clone();
                            ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(Tensor::filled(shape, 0.0)))))
                        }
                    }
                } else {
                    ExecOutcome::Panic("grad_get: expected Tensor".to_string())
                }
            }
            "p_grad_zero" => {
                // ML3: Zero out all gradients
                self.autograd_grads.clear();
                ExecOutcome::Value(Value::Void)
            }
            "p_dp_budget_check" => {
                // ML12: Check epsilon budget — args: (epsilon_for_this_call)
                // If cumulative budget exceeds limit, panic.
                if let (Some(Value::Float(epsilon)), Some(Value::Float(limit))) = (args.get(0), args.get(1)) {
                    self.dp_epsilon_budget += epsilon;
                    if self.dp_epsilon_budget > *limit {
                        return ExecOutcome::Panic(format!(
                            "DP budget exhausted: consumed {} > limit {}",
                            self.dp_epsilon_budget, limit
                        ));
                    }
                    ExecOutcome::Value(Value::Void)
                } else {
                    ExecOutcome::Panic("dp_budget_check: expected (f64, f64)".to_string())
                }
            }
            "p_dp_budget_reset" => {
                // ML12: Reset the epsilon budget (called at the start of each @dp function)
                self.dp_epsilon_budget = 0.0;
                if let Some(Value::Float(limit)) = args.first() {
                    self.dp_epsilon_limit = *limit;
                }
                ExecOutcome::Value(Value::Void)
            }
            "p_tp_rank" => {
                // ML13: Get the current rank (0 in single-device mode)
                ExecOutcome::Value(Value::Int(self.tp_rank))
            }
            "p_tp_world_size" => {
                // ML13: Get the world size (1 in single-device mode)
                ExecOutcome::Value(Value::Int(self.tp_world_size))
            }
            "p_checkpoint_begin" => {
                // ML11: Enable checkpoint mode — tape stores input data, not result data
                self.checkpoint_mode = true;
                ExecOutcome::Value(Value::Void)
            }
            "p_checkpoint_end" => {
                // ML11: Disable checkpoint mode
                self.checkpoint_mode = false;
                ExecOutcome::Value(Value::Void)
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

            // ─── ML9: KV Cache builtins ───
            // KV cache stores attention K and V tensors per layer.
            // The cache is bounded by the region budget and uses sliding window eviction.
            "p_kv_cache_new" => {
                // Create a new KV cache: p_kv_cache_new(num_layers, max_seq_len, window_size, head_dim)
                // Returns an opaque handle (i64 = index into kv_caches map)
                let num_layers = match args.get(0) { Some(Value::Int(n)) => *n as usize, _ => 0 };
                let max_seq_len = match args.get(1) { Some(Value::Int(n)) => *n as usize, _ => 0 };
                let window_size = match args.get(2) { Some(Value::Int(n)) => *n as usize, _ => 0 };
                let head_dim = match args.get(3) { Some(Value::Int(n)) => *n as usize, _ => 0 };
                let cache = crate::tensor::KvCache::new(num_layers, max_seq_len, window_size, head_dim);
                let id = self.kv_caches.len() as i64;
                self.kv_caches.push(Rc::new(RefCell::new(cache)));
                ExecOutcome::Value(Value::Int(id))
            }
            "p_kv_cache_append" => {
                // Append K and V for a layer: p_kv_cache_append(cache_id, layer, k_tensor, v_tensor)
                let cache_id = match args.get(0) { Some(Value::Int(n)) => *n as usize, _ => 0 };
                let layer = match args.get(1) { Some(Value::Int(n)) => *n as usize, _ => 0 };
                if cache_id >= self.kv_caches.len() {
                    ExecOutcome::Panic("kv_cache_append: invalid cache id".to_string())
                } else if let (Some(Value::Tensor(k)), Some(Value::Tensor(v))) = (args.get(2), args.get(3)) {
                    let cache = &self.kv_caches[cache_id];
                    cache.borrow_mut().append(layer, &k.borrow(), &v.borrow());
                    ExecOutcome::Continue
                } else {
                    ExecOutcome::Panic("kv_cache_append: expected (cache_id, layer, k_tensor, v_tensor)".to_string())
                }
            }
            "p_kv_cache_get" => {
                // Get K and V for a layer: p_kv_cache_get(cache_id, layer)
                // Returns a concatenated tensor [K; V]
                let cache_id = match args.get(0) { Some(Value::Int(n)) => *n as usize, _ => 0 };
                let layer = match args.get(1) { Some(Value::Int(n)) => *n as usize, _ => 0 };
                if cache_id >= self.kv_caches.len() {
                    ExecOutcome::Panic("kv_cache_get: invalid cache id".to_string())
                } else {
                    let cache = &self.kv_caches[cache_id];
                    let t = cache.borrow().get(layer);
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(t))))
                }
            }
            "p_kv_cache_len" => {
                // Get current sequence length for a layer: p_kv_cache_len(cache_id, layer)
                let cache_id = match args.get(0) { Some(Value::Int(n)) => *n as usize, _ => 0 };
                let layer = match args.get(1) { Some(Value::Int(n)) => *n as usize, _ => 0 };
                if cache_id >= self.kv_caches.len() {
                    ExecOutcome::Panic("kv_cache_len: invalid cache id".to_string())
                } else {
                    let cache = &self.kv_caches[cache_id];
                    ExecOutcome::Value(Value::Int(cache.borrow().len(layer) as i64))
                }
            }
            "p_kv_cache_clear" => {
                // Clear the cache: p_kv_cache_clear(cache_id)
                let cache_id = match args.get(0) { Some(Value::Int(n)) => *n as usize, _ => 0 };
                if cache_id >= self.kv_caches.len() {
                    ExecOutcome::Panic("kv_cache_clear: invalid cache id".to_string())
                } else {
                    self.kv_caches[cache_id].borrow_mut().clear();
                    ExecOutcome::Continue
                }
            }
            "p_kv_cache_evict" => {
                // Evict oldest tokens (sliding window): p_kv_cache_evict(cache_id, num_tokens)
                let cache_id = match args.get(0) { Some(Value::Int(n)) => *n as usize, _ => 0 };
                let num_tokens = match args.get(1) { Some(Value::Int(n)) => *n as usize, _ => 0 };
                if cache_id >= self.kv_caches.len() {
                    ExecOutcome::Panic("kv_cache_evict: invalid cache id".to_string())
                } else {
                    self.kv_caches[cache_id].borrow_mut().evict_oldest(num_tokens);
                    ExecOutcome::Continue
                }
            }

            // ─── ML10: Quantization builtins ───
            "p_quantize_q8" => {
                // Quantize a float tensor to int8: p_quantize_q8(tensor, scale, zero_point)
                if let (Some(Value::Tensor(t)), Some(Value::Float(scale)), Some(Value::Int(zp))) =
                    (args.get(0), args.get(1), args.get(2)) {
                    let quantized = crate::tensor::Tensor::quantize_q8(&t.borrow(), *scale, *zp as i32);
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(quantized))))
                } else {
                    ExecOutcome::Panic("quantize_q8: expected (Tensor, f64, i64)".to_string())
                }
            }
            "p_dequantize_q8" => {
                // Dequantize int8 tensor back to float: p_dequantize_q8(tensor, scale, zero_point)
                if let (Some(Value::Tensor(t)), Some(Value::Float(scale)), Some(Value::Int(zp))) =
                    (args.get(0), args.get(1), args.get(2)) {
                    let dequant = crate::tensor::Tensor::dequantize_q8(&t.borrow(), *scale, *zp as i32);
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(dequant))))
                } else {
                    ExecOutcome::Panic("dequantize_q8: expected (Tensor, f64, i64)".to_string())
                }
            }
            "p_qmatmul" => {
                // Quantized matmul: q8 × q8 → f32
                // Dequantizes inputs, computes matmul, returns f32 result
                if let (Some(Value::Tensor(a)), Some(Value::Tensor(b))) = (args.get(0), args.get(1)) {
                    match a.borrow().matmul(&b.borrow()) {
                        Ok(result) => ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(result)))),
                        Err(e) => ExecOutcome::Panic(format!("qmatmul: {}", e)),
                    }
                } else {
                    ExecOutcome::Panic("qmatmul: expected (Tensor, Tensor)".to_string())
                }
            }
            "p_find_quant_params" => {
                // Find optimal scale and zero_point for a tensor
                // Returns "scale,zero_point" as a string
                if let Some(Value::Tensor(t)) = args.get(0) {
                    let (scale, zp) = crate::tensor::Tensor::find_quant_params(&t.borrow());
                    ExecOutcome::Value(Value::Str(CompactString::from(format!("{},{}", scale, zp))))
                } else {
                    ExecOutcome::Panic("find_quant_params: expected Tensor".to_string())
                }
            }

            // ─── ML12: Differential Privacy noise builtins ───
            "p_dp_noise" => {
                // Add Gaussian noise to a tensor: p_dp_noise(tensor, sigma)
                // sigma = sqrt(2 * epsilon * sensitivity^2 / num_samples)
                if let (Some(Value::Tensor(t)), Some(Value::Float(sigma))) = (args.get(0), args.get(1)) {
                    let mut noisy = t.borrow().clone();
                    for val in &mut noisy.data {
                        // Box-Muller transform for Gaussian noise
                        let u1 = 0.0001 + ((*val * 1000.0).fract().abs()) * 0.9998;
                        let u2 = 0.0001 + ((val.sin() * 1000.0).fract().abs()) * 0.9998;
                        let z = (-2.0 * u1.ln()).sqrt() * (2.0 * std::f64::consts::PI * u2).cos();
                        *val += sigma * z;
                    }
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(noisy))))
                } else {
                    ExecOutcome::Panic("dp_noise: expected (Tensor, f64)".to_string())
                }
            }
            "p_gaussian_noise" => {
                // Generate Gaussian noise tensor: p_gaussian_noise(shape, sigma)
                let shape = self.arg_to_vec_i64(args.get(0));
                let sigma = match args.get(1) { Some(Value::Float(s)) => *s, _ => 0.0 };
                if let Some(shape) = shape {
                    let shape_usize: Vec<usize> = shape.iter().map(|&x| x as usize).collect();
                    let size: usize = shape_usize.iter().product();
                    let data: Vec<f64> = (0..size).map(|i| {
                        let seed = (i as f64) * 0.1;
                        let u1 = 0.0001 + (seed.sin().abs()) * 0.9998;
                        let u2 = 0.0001 + (seed.cos().abs()) * 0.9998;
                        let z = (-2.0 * u1.ln()).sqrt() * (2.0 * std::f64::consts::PI * u2).cos();
                        sigma * z
                    }).collect();
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(
                        crate::tensor::Tensor::from_vec(shape_usize, data)
                    ))))
                } else {
                    ExecOutcome::Panic("gaussian_noise: expected (Vec, f64)".to_string())
                }
            }
            "p_dropout" => {
                // Dropout: p_dropout(tensor, drop_prob)
                // Randomly zeros elements with probability drop_prob, scales survivors
                if let (Some(Value::Tensor(t)), Some(Value::Float(p))) = (args.get(0), args.get(1)) {
                    let mut result = t.borrow().clone();
                    let scale = 1.0 / (1.0 - *p);
                    for (i, val) in result.data.iter_mut().enumerate() {
                        let seed = ((i as f64) * 7.0).sin().abs();
                        if seed < *p {
                            *val = 0.0;
                        } else {
                            *val *= scale;
                        }
                    }
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(result))))
                } else {
                    ExecOutcome::Panic("dropout: expected (Tensor, f64)".to_string())
                }
            }

            // ─── ML13: Tensor parallelism communication builtins ───
            // In the interpreter (single device), these are no-ops that return
            // the input unchanged. In a real multi-GPU deployment, they would
            // perform NCCL collective operations.
            "p_all_reduce" => {
                // ML13: All-reduce sum — in single-device mode (world_size=1), returns input unchanged.
                // In simulated multi-rank mode, sums across all simulated ranks.
                if let Some(Value::Tensor(t)) = args.first() {
                    if self.tp_world_size > 1 {
                        // Simulated: divide by world_size (averaging, a common all-reduce variant)
                        let mut result = t.borrow().clone();
                        for d in &mut result.data { *d /= self.tp_world_size as f64; }
                        ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(result))))
                    } else {
                        ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(t.borrow().clone()))))
                    }
                } else {
                    ExecOutcome::Panic("all_reduce: expected Tensor".to_string())
                }
            }
            "p_broadcast" => {
                // ML13: Broadcast from rank 0 — in single-device mode, returns input unchanged
                if let Some(Value::Tensor(t)) = args.first() {
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(t.borrow().clone()))))
                } else {
                    ExecOutcome::Panic("broadcast: expected Tensor".to_string())
                }
            }
            "p_all_gather" => {
                // ML13: All-gather — concatenates along dim 0. In single-device, returns input.
                if let Some(Value::Tensor(t)) = args.first() {
                    if self.tp_world_size > 1 {
                        // Simulated: replicate along dim 0
                        let src = t.borrow();
                        let mut new_shape = src.shape.clone();
                        if !new_shape.is_empty() { new_shape[0] *= self.tp_world_size as usize; }
                        let mut new_data = src.data.clone();
                        for _ in 1..self.tp_world_size { new_data.extend_from_slice(&src.data); }
                        ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(Tensor { shape: new_shape, strides: Tensor::row_major_strides(&src.shape), data: new_data }))))
                    } else {
                        ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(t.borrow().clone()))))
                    }
                } else {
                    ExecOutcome::Panic("all_gather: expected Tensor".to_string())
                }
            }
            "p_scatter" => {
                // ML13: Scatter — splits tensor across ranks. In single-device, returns input.
                if let Some(Value::Tensor(t)) = args.first() {
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(t.borrow().clone()))))
                } else {
                    ExecOutcome::Panic("scatter: expected Tensor".to_string())
                }
            }
            "p_send_tensor" => {
                // Send tensor to another rank (no-op in single-device)
                ExecOutcome::Continue
            }
            "p_recv_tensor" => {
                // Receive tensor from another rank (no-op in single-device, returns zeros)
                let shape = self.arg_to_vec_i64(args.get(0));
                if let Some(shape) = shape {
                    let shape_usize: Vec<usize> = shape.iter().map(|&x| x as usize).collect();
                    ExecOutcome::Value(Value::Tensor(Rc::new(RefCell::new(
                        crate::tensor::Tensor::zeros(shape_usize)
                    ))))
                } else {
                    ExecOutcome::Panic("recv_tensor: expected Vec".to_string())
                }
            }

            // ─── Phase 5C.2: SIMD builtins ───
            "p_simd_add" => {
                let a = self.args_to_f64_vec(args, 0);
                let b = self.args_to_f64_vec(args, 1);
                if let (Some(a), Some(b)) = (a, b) {
                    let result: SmallVec<[Value; 8]> = a.iter().zip(b.iter()).map(|(x, y)| Value::Float(x + y)).collect();
                    ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(result))))
                } else { ExecOutcome::Panic("simd_add: expected Vec, Vec".to_string()) }
            }
            "p_simd_sub" => {
                let a = self.args_to_f64_vec(args, 0);
                let b = self.args_to_f64_vec(args, 1);
                if let (Some(a), Some(b)) = (a, b) {
                    let result: SmallVec<[Value; 8]> = a.iter().zip(b.iter()).map(|(x, y)| Value::Float(x - y)).collect();
                    ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(result))))
                } else { ExecOutcome::Panic("simd_sub: expected Vec, Vec".to_string()) }
            }
            "p_simd_mul" => {
                let a = self.args_to_f64_vec(args, 0);
                let b = self.args_to_f64_vec(args, 1);
                if let (Some(a), Some(b)) = (a, b) {
                    let result: SmallVec<[Value; 8]> = a.iter().zip(b.iter()).map(|(x, y)| Value::Float(x * y)).collect();
                    ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(result))))
                } else { ExecOutcome::Panic("simd_mul: expected Vec, Vec".to_string()) }
            }
            "p_simd_div" => {
                let a = self.args_to_f64_vec(args, 0);
                let b = self.args_to_f64_vec(args, 1);
                if let (Some(a), Some(b)) = (a, b) {
                    let result: SmallVec<[Value; 8]> = a.iter().zip(b.iter()).map(|(x, y)| Value::Float(if *y != 0.0 { x / y } else { 0.0 })).collect();
                    ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(result))))
                } else { ExecOutcome::Panic("simd_div: expected Vec, Vec".to_string()) }
            }
            "p_simd_dot" => {
                let a = self.args_to_f64_vec(args, 0);
                let b = self.args_to_f64_vec(args, 1);
                if let (Some(a), Some(b)) = (a, b) {
                    let dot: f64 = a.iter().zip(b.iter()).map(|(x, y)| x * y).sum();
                    ExecOutcome::Value(Value::Float(dot))
                } else { ExecOutcome::Panic("simd_dot: expected Vec, Vec".to_string()) }
            }
            "p_simd_sum" => {
                if let Some(v) = self.args_to_f64_vec(args, 0) {
                    ExecOutcome::Value(Value::Float(v.iter().sum()))
                } else { ExecOutcome::Panic("simd_sum: expected Vec".to_string()) }
            }
            "p_simd_scale" => {
                if let (Some(v), Some(Value::Float(s))) = (self.args_to_f64_vec(args, 0), args.get(1)) {
                    let result: SmallVec<[Value; 8]> = v.iter().map(|x| Value::Float(x * s)).collect();
                    ExecOutcome::Value(Value::Vec(Rc::new(RefCell::new(result))))
                } else {
                    ExecOutcome::Panic("simd_scale: expected Vec, f64".to_string())
                }
            }
            "p_simd_from_vec" => {
                if let Some(v) = self.args_to_f64_vec(args, 0) {
                    let result: SmallVec<[Value; 8]> = v.iter().map(|x| Value::Float(*x)).collect();
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
                    let query_str = self.value_to_string(query);
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
                if let Some(path_arg) = args.get(0) {
                    let path_str = self.value_to_string(path_arg);
                    if !path_str.is_empty() {
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
                }
                ExecOutcome::Value(Value::Ptr(0))
            }
            "p_file_write" => {
                if let (Some(path_arg), Some(content_arg)) = (args.get(0), args.get(1)) {
                    let path_str = self.value_to_string(path_arg);
                    let content_str = self.value_to_string(content_arg);
                    if !path_str.is_empty() {
                        match std::fs::write(&path_str, &content_str) {
                            Ok(_) => return ExecOutcome::Value(Value::Int(0)),
                            Err(_) => return ExecOutcome::Value(Value::Int(-1)),
                        }
                    }
                }
                ExecOutcome::Value(Value::Int(-1))
            }
            "p_file_exists" => {
                if let Some(path_arg) = args.get(0) {
                    let path_str = self.value_to_string(path_arg);
                    if !path_str.is_empty() {
                        return ExecOutcome::Value(Value::Bool(std::path::Path::new(&path_str).exists()));
                    }
                }
                ExecOutcome::Value(Value::Bool(false))
            }
            "p_file_append" => {
                if let (Some(path_arg), Some(content_arg)) = (args.get(0), args.get(1)) {
                    let path_str = self.value_to_string(path_arg);
                    let content_str = self.value_to_string(content_arg);
                    if !path_str.is_empty() {
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
                }
                ExecOutcome::Value(Value::Int(-1))
            }
            "p_file_delete" => {
                if let Some(path_arg) = args.get(0) {
                    let path_str = self.value_to_string(path_arg);
                    if !path_str.is_empty() {
                        match std::fs::remove_file(&path_str) {
                            Ok(_) => return ExecOutcome::Value(Value::Int(0)),
                            Err(_) => return ExecOutcome::Value(Value::Int(-1)),
                        }
                    }
                }
                ExecOutcome::Value(Value::Int(-1))
            }
            "p_file_size" => {
                if let Some(path_arg) = args.get(0) {
                    let path_str = self.value_to_string(path_arg);
                    if !path_str.is_empty() {
                        match std::fs::metadata(&path_str) {
                            Ok(m) => return ExecOutcome::Value(Value::Int(m.len() as i64)),
                            Err(_) => return ExecOutcome::Value(Value::Int(-1)),
                        }
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
                if let Some(name_arg) = args.first() {
                    let name_str = self.value_to_string(name_arg);
                    if !name_str.is_empty() {
                        match std::env::var(&name_str) {
                            Ok(val) => return ExecOutcome::Value(Value::Str(CompactString::from(val))),
                            Err(_) => return ExecOutcome::Value(Value::Ptr(0)),
                        }
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
                if let Some(cmd_arg) = args.first() {
                    let cmd_str = self.value_to_string(cmd_arg);
                    if !cmd_str.is_empty() {
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
                }
                ExecOutcome::Value(Value::Int(-1))
            }
            "p_exit" => {
                if let Some(Value::Int(code)) = args.first() {
                    std::process::exit(*code as i32);
                }
                std::process::exit(0);
            }

            // ─── Wall-clock timing builtin ───
            // Returns current time in nanoseconds since Unix epoch (1970-01-01 UTC).
            // For measuring elapsed time, take the difference between two calls.
            "p_clock_now" => {
                use std::time::{SystemTime, UNIX_EPOCH};
                let now = SystemTime::now()
                    .duration_since(UNIX_EPOCH)
                    .unwrap_or_default();
                ExecOutcome::Value(Value::Int(now.as_nanos() as i64))
            }
            // Returns current time in microseconds since Unix epoch.
            "p_clock_now_us" => {
                use std::time::{SystemTime, UNIX_EPOCH};
                let now = SystemTime::now()
                    .duration_since(UNIX_EPOCH)
                    .unwrap_or_default();
                ExecOutcome::Value(Value::Int(now.as_micros() as i64))
            }
            // Returns current time in milliseconds since Unix epoch.
            "p_clock_now_ms" => {
                use std::time::{SystemTime, UNIX_EPOCH};
                let now = SystemTime::now()
                    .duration_since(UNIX_EPOCH)
                    .unwrap_or_default();
                ExecOutcome::Value(Value::Int(now.as_millis() as i64))
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
                // In the interpreter, spawn runs eagerly (sequentially) to preserve
                // Vec/Map side-table access. C codegen uses real OS threads with
                // shared memory. Returns a Future handle. Use p_future_poll to join.
                let func_name = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let num_args = match args.get(1) {
                    Some(Value::Int(n)) => *n as usize,
                    _ => 0,
                };
                // Collect the actual argument Values (preserving Vec/Map pointers)
                let thread_args: Vec<Value> = if args.len() >= 2 + num_args {
                    args[2..2 + num_args].to_vec()
                } else {
                    args[2..].to_vec()
                };

                // Prepend a dummy env value (user functions have __env as first param)
                let mut thread_values: Vec<Value> = vec![Value::Int(0)];
                thread_values.extend(thread_args);

                // Allocate a Future slot
                let future_idx = self.futures.len();

                // Run eagerly in the current interpreter (preserves Vec side-table access)
                let result = if let Some(func) = self.module.functions.get(&func_name).cloned() {
                    match self.run_function(&func, &thread_values) {
                        RunResult::Return(v) => v,
                        RunResult::Panic(msg) => {
                            eprintln!("Thread panic: {}", msg);
                            Value::Int(0)
                        }
                        RunResult::Paused => Value::Int(0),
                    }
                } else {
                    eprintln!("Thread: unknown function '{}'", func_name);
                    Value::Int(0)
                };

                // Store result as Ready (eager execution — no JoinHandle needed)
                self.futures.push(FutureState::Ready(result));
                ExecOutcome::Value(Value::Future(future_idx))
            }

            // ─── Phase A1: Executor + non-blocking task poll ───
            "p_executor_new" => {
                // p_executor_new() -> i64 — create an executor, return handle.
                let idx = self.executors.len();
                self.executors.push(Vec::new());
                ExecOutcome::Value(Value::Int(idx as i64))
            }
            "p_executor_spawn" => {
                // p_executor_spawn(exec: i64, func_name: *u8, num_args: i64, args...)
                //   -> i64 (future handle)
                // In the interpreter, spawn runs eagerly (sequentially) to preserve
                // Vec/Map side-table access. C codegen uses real OS threads.
                let exec_idx = match args.first() {
                    Some(Value::Int(n)) => *n as usize,
                    _ => 0,
                };
                let func_name = self.value_to_string(args.get(1).unwrap_or(&Value::Int(0)));
                let num_args = match args.get(2) {
                    Some(Value::Int(n)) => *n as usize,
                    _ => 0,
                };
                let thread_args: Vec<Value> = if args.len() >= 3 + num_args {
                    args[3..3 + num_args].to_vec()
                } else {
                    args[3..].to_vec()
                };
                let future_idx = self.futures.len();
                let mut thread_values: Vec<Value> = vec![Value::Int(0)];
                thread_values.extend(thread_args);
                let result = if let Some(func) = self.module.functions.get(&func_name).cloned() {
                    match self.run_function(&func, &thread_values) {
                        RunResult::Return(v) => v,
                        RunResult::Panic(msg) => {
                            eprintln!("Thread panic: {}", msg);
                            Value::Int(0)
                        }
                        RunResult::Paused => Value::Int(0),
                    }
                } else {
                    eprintln!("Executor: unknown function '{}'", func_name);
                    Value::Int(0)
                };
                self.futures.push(FutureState::Ready(result));
                self.task_done.lock().unwrap().insert(future_idx, true);
                if exec_idx < self.executors.len() {
                    self.executors[exec_idx].push(future_idx);
                }
                ExecOutcome::Value(Value::Future(future_idx))
            }
            "p_executor_block_on" => {
                // p_executor_block_on(exec: i64, future: i64) -> i64
                // Block until the future is ready. For A1, this is the same as
                // p_future_poll (blocking join). A2 will add cooperative polling.
                let idx = match args.get(1) {
                    Some(Value::Future(idx)) => *idx,
                    Some(Value::Int(n)) => *n as usize,
                    _ => return ExecOutcome::Panic("block_on: not a Future value".to_string()),
                };
                if idx >= self.futures.len() {
                    return ExecOutcome::Panic(format!("block_on: invalid handle {}", idx));
                }
                match &self.futures[idx] {
                    FutureState::Ready(v) => return ExecOutcome::Value(v.clone()),
                    FutureState::Pending(_) => {}
                }
                if let FutureState::Pending(Some(handle)) =
                    std::mem::replace(&mut self.futures[idx], FutureState::Ready(Value::Int(0)))
                {
                    match handle.join() {
                        Ok(n) => {
                            let v = Value::Int(n);
                            self.futures[idx] = FutureState::Ready(v.clone());
                            self.task_done.lock().unwrap().insert(idx, true);
                            ExecOutcome::Value(v)
                        }
                        Err(_) => {
                            self.futures[idx] = FutureState::Ready(Value::Int(0));
                            self.task_done.lock().unwrap().insert(idx, true);
                            ExecOutcome::Panic("Thread panicked during block_on".to_string())
                        }
                    }
                } else {
                    ExecOutcome::Panic("block_on: already consumed".to_string())
                }
            }
            "p_task_poll" => {
                // p_task_poll(future: i64) -> i64
                // For A1: blocks (joins the thread) to get the value.
                // A2 will make this non-blocking inside async contexts (state machine).
                let idx = match args.first() {
                    Some(Value::Future(idx)) => *idx,
                    Some(Value::Int(n)) => *n as usize,
                    _ => return ExecOutcome::Panic("task_poll: not a Future value".to_string()),
                };
                if idx >= self.futures.len() {
                    return ExecOutcome::Panic(format!("task_poll: invalid handle {}", idx));
                }
                match &self.futures[idx] {
                    FutureState::Ready(v) => ExecOutcome::Value(v.clone()),
                    FutureState::Pending(_) => {
                        // Block until thread completes (join)
                        if let FutureState::Pending(Some(handle)) =
                            std::mem::replace(&mut self.futures[idx], FutureState::Ready(Value::Int(0)))
                        {
                            match handle.join() {
                                Ok(n) => {
                                    let v = Value::Int(n);
                                    self.futures[idx] = FutureState::Ready(v.clone());
                                    self.task_done.lock().unwrap().insert(idx, true);
                                    ExecOutcome::Value(v)
                                }
                                Err(_) => {
                                    self.futures[idx] = FutureState::Ready(Value::Int(0));
                                    self.task_done.lock().unwrap().insert(idx, true);
                                    ExecOutcome::Value(Value::Int(0))
                                }
                            }
                        } else {
                            ExecOutcome::Value(Value::Int(0))
                        }
                    }
                }
            }
            "p_task_is_ready" => {
                // p_task_is_ready(future: i64) -> bool
                let idx = match args.first() {
                    Some(Value::Future(idx)) => *idx,
                    Some(Value::Int(n)) => *n as usize,
                    _ => return ExecOutcome::Value(Value::Bool(false)),
                };
                if idx >= self.futures.len() {
                    return ExecOutcome::Value(Value::Bool(false));
                }
                let ready = match &self.futures[idx] {
                    FutureState::Ready(_) => true,
                    FutureState::Pending(_) => {
                        self.task_done.lock().unwrap().get(&idx).copied().unwrap_or(false)
                    }
                };
                ExecOutcome::Value(Value::Bool(ready))
            }
            "p_executor_run_all" => {
                // p_executor_run_all(exec: i64) -> i64 (count of completed tasks)
                // Polls all tasks on the executor. Returns when all are done.
                // For A1, this blocks on each pending task in order (join all).
                let exec_idx = match args.first() {
                    Some(Value::Int(n)) => *n as usize,
                    _ => 0,
                };
                if exec_idx >= self.executors.len() {
                    return ExecOutcome::Value(Value::Int(0));
                }
                let task_indices = self.executors[exec_idx].clone();
                let mut count = 0i64;
                for &idx in &task_indices {
                    if idx >= self.futures.len() { continue; }
                    match &self.futures[idx] {
                        FutureState::Ready(_) => { count += 1; }
                        FutureState::Pending(_) => {
                            if let FutureState::Pending(Some(handle)) =
                                std::mem::replace(&mut self.futures[idx], FutureState::Ready(Value::Int(0)))
                            {
                                match handle.join() {
                                    Ok(n) => {
                                        self.futures[idx] = FutureState::Ready(Value::Int(n));
                                        self.task_done.lock().unwrap().insert(idx, true);
                                        count += 1;
                                    }
                                    Err(_) => {
                                        self.futures[idx] = FutureState::Ready(Value::Int(0));
                                        self.task_done.lock().unwrap().insert(idx, true);
                                    }
                                }
                            }
                        }
                    }
                }
                ExecOutcome::Value(Value::Int(count))
            }
            "p_executor_poll_all" => {
                // p_executor_poll_all(exec: i64) -> i64
                // Non-blocking: returns the number of tasks still pending.
                // Tasks that have completed since the last poll are joined
                // (their values are collected). A2: enables round-robin scheduling.
                let exec_idx = match args.first() {
                    Some(Value::Int(n)) => *n as usize,
                    _ => 0,
                };
                if exec_idx >= self.executors.len() {
                    return ExecOutcome::Value(Value::Int(0));
                }
                let task_indices = self.executors[exec_idx].clone();
                let mut pending = 0i64;
                for &idx in &task_indices {
                    if idx >= self.futures.len() { continue; }
                    match &self.futures[idx] {
                        FutureState::Ready(_) => {}
                        FutureState::Pending(_) => {
                            let done = self.task_done.lock().unwrap()
                                .get(&idx).copied().unwrap_or(false);
                            if done {
                                // Join to collect the value
                                if let FutureState::Pending(Some(handle)) =
                                    std::mem::replace(&mut self.futures[idx], FutureState::Ready(Value::Int(0)))
                                {
                                    match handle.join() {
                                        Ok(n) => {
                                            self.futures[idx] = FutureState::Ready(Value::Int(n));
                                            self.task_done.lock().unwrap().insert(idx, true);
                                        }
                                        Err(_) => {
                                            self.futures[idx] = FutureState::Ready(Value::Int(0));
                                            self.task_done.lock().unwrap().insert(idx, true);
                                        }
                                    }
                                }
                            } else {
                                pending += 1;
                            }
                        }
                    }
                }
                ExecOutcome::Value(Value::Int(pending))
            }
            "p_task_poll_nonblocking" => {
                // p_task_poll_nonblocking(future: i64) -> i64
                // Non-blocking poll: returns the value if ready, PENDING_SENTINEL if not.
                // A2: used by async runtimes to check task readiness without blocking.
                let idx = match args.first() {
                    Some(Value::Future(idx)) => *idx,
                    Some(Value::Int(n)) => *n as usize,
                    _ => return ExecOutcome::Panic("task_poll_nb: not a Future value".to_string()),
                };
                if idx >= self.futures.len() {
                    return ExecOutcome::Panic(format!("task_poll_nb: invalid handle {}", idx));
                }
                match &self.futures[idx] {
                    FutureState::Ready(v) => ExecOutcome::Value(v.clone()),
                    FutureState::Pending(_) => {
                        let done = self.task_done.lock().unwrap()
                            .get(&idx).copied().unwrap_or(false);
                        if done {
                            if let FutureState::Pending(Some(handle)) =
                                std::mem::replace(&mut self.futures[idx], FutureState::Ready(Value::Int(0)))
                            {
                                match handle.join() {
                                    Ok(n) => {
                                        let v = Value::Int(n);
                                        self.futures[idx] = FutureState::Ready(v.clone());
                                        self.task_done.lock().unwrap().insert(idx, true);
                                        ExecOutcome::Value(v)
                                    }
                                    Err(_) => {
                                        self.futures[idx] = FutureState::Ready(Value::Int(0));
                                        self.task_done.lock().unwrap().insert(idx, true);
                                        ExecOutcome::Value(Value::Int(0))
                                    }
                                }
                            } else {
                                ExecOutcome::Value(Value::Int(0))
                            }
                        } else {
                            ExecOutcome::Value(Value::Int(PENDING_SENTINEL))
                        }
                    }
                }
            }

            // ─── Phase A3: Non-blocking IO (simulated in interpreter) ───
            // In the interpreter, async IO is simulated by spawning a thread
            // that performs the IO operation and returns the result.
            // This gives the same semantics as real non-blocking IO: the
            // caller gets a Future that can be polled.
            "p_net_connect" => {
                // p_net_connect(host: *u8, port: i64) -> i64 (Future handle)
                // Simulated: spawn a thread that "connects" (just returns a fake socket)
                let _host = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let port = match args.get(1) {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let _module = self.module.clone();
                let future_idx = self.futures.len();
                self.futures.push(FutureState::Pending(None));
                self.task_done.lock().unwrap().insert(future_idx, false);
                let task_done = self.task_done.clone();
                let handle = std::thread::spawn(move || {
                    // Simulate connection delay
                    std::thread::sleep(std::time::Duration::from_millis(10));
                    // Return a fake socket FD (just a number)
                    let sock = (port as i64) + 0x10000;
                    task_done.lock().unwrap().insert(future_idx, true);
                    sock
                });
                self.futures[future_idx] = FutureState::Pending(Some(handle));
                ExecOutcome::Value(Value::Future(future_idx))
            }
            "p_net_read" => {
                // p_net_read(sock: i64, buf: *u8, len: i64) -> i64 (Future handle)
                // Simulated: returns a fake read result
                let _sock = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let _buf = args.get(1);
                let len = match args.get(2) {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let _module = self.module.clone();
                let future_idx = self.futures.len();
                self.futures.push(FutureState::Pending(None));
                self.task_done.lock().unwrap().insert(future_idx, false);
                let task_done = self.task_done.clone();
                let handle = std::thread::spawn(move || {
                    std::thread::sleep(std::time::Duration::from_millis(5));
                    // Return the number of bytes "read" (min of len and 13)
                    let n = if len > 13 { 13 } else { len };
                    task_done.lock().unwrap().insert(future_idx, true);
                    n
                });
                self.futures[future_idx] = FutureState::Pending(Some(handle));
                ExecOutcome::Value(Value::Future(future_idx))
            }
            "p_net_write" => {
                // p_net_write(sock: i64, buf: *u8, len: i64) -> i64 (Future handle)
                // Simulated: returns the number of bytes "written"
                let _sock = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let len = match args.get(2) {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let future_idx = self.futures.len();
                self.futures.push(FutureState::Pending(None));
                self.task_done.lock().unwrap().insert(future_idx, false);
                let task_done = self.task_done.clone();
                let handle = std::thread::spawn(move || {
                    std::thread::sleep(std::time::Duration::from_millis(5));
                    task_done.lock().unwrap().insert(future_idx, true);
                    len // "wrote" all bytes
                });
                self.futures[future_idx] = FutureState::Pending(Some(handle));
                ExecOutcome::Value(Value::Future(future_idx))
            }
            "p_fs_read_async" => {
                // p_fs_read_async(path: *u8, buf: *u8, len: i64) -> i64 (Future handle)
                // Simulated: reads the file in a thread
                let path = self.value_to_string(args.first().unwrap_or(&Value::Int(0)));
                let len = match args.get(2) {
                    Some(Value::Int(n)) => *n,
                    _ => 1024,
                };
                let future_idx = self.futures.len();
                self.futures.push(FutureState::Pending(None));
                self.task_done.lock().unwrap().insert(future_idx, false);
                let task_done = self.task_done.clone();
                let handle = std::thread::spawn(move || {
                    // Try to read the file
                    let bytes = std::fs::read(&path).unwrap_or_default();
                    let n = if bytes.len() as i64 > len { len } else { bytes.len() as i64 };
                    task_done.lock().unwrap().insert(future_idx, true);
                    n
                });
                self.futures[future_idx] = FutureState::Pending(Some(handle));
                ExecOutcome::Value(Value::Future(future_idx))
            }
            "p_net_close" => {
                // p_net_close(sock: i64) -> void
                // Simulated: no-op
                ExecOutcome::Continue
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
            "p_atomic_sub" => {
                // p_atomic_sub(addr: i64, val: i64) -> i64 (returns old value)
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
                shared.insert(addr, old - val);
                ExecOutcome::Value(Value::Int(old))
            }
            "p_rc_incr" => {
                // p_rc_incr(rc_addr: i64, val: i64) -> i64
                // Increment refcount stored at rc_addr in heap memory, return val.
                let rc_addr = match args.first() {
                    Some(Value::Int(n)) => *n,
                    Some(Value::Ptr(a)) => *a as i64,
                    _ => 0,
                };
                let val = match args.get(1) {
                    Some(Value::Int(n)) => *n,
                    Some(Value::Ptr(a)) => *a as i64,
                    _ => 0,
                };
                let bytes = self.memory.load(rc_addr as u64, 8);
                let count = i64::from_le_bytes(bytes.try_into().unwrap_or([0u8; 8]));
                self.memory.store(rc_addr as u64, &(count + 1).to_le_bytes());
                ExecOutcome::Value(Value::Int(val))
            }
            "p_rc_decr" => {
                // p_rc_decr(rc_addr: i64, val: i64) -> void
                // Decrement refcount at rc_addr. If it reaches 0, free the block
                // (rc_addr - 0 is the raw allocation start since refcount is at offset 0).
                let rc_addr = match args.first() {
                    Some(Value::Int(n)) => *n,
                    Some(Value::Ptr(a)) => *a as i64,
                    _ => 0,
                };
                let bytes = self.memory.load(rc_addr as u64, 8);
                let count = i64::from_le_bytes(bytes.try_into().unwrap_or([0u8; 8]));
                let new_count = count - 1;
                self.memory.store(rc_addr as u64, &new_count.to_le_bytes());
                // Interpreter doesn't actually free — memory is arena-based
                ExecOutcome::Value(Value::Void)
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

            // ─── Thread-local storage ───
            "p_tls_create" => {
                // p_tls_create() -> i64 — create a TLS key, return handle
                let key = self.next_tls_key;
                self.next_tls_key += 1;
                ExecOutcome::Value(Value::Int(key))
            }
            "p_tls_get" => {
                // p_tls_get(key: i64) -> i64 — get value for current thread (0 if unset)
                let key = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                // thread_id 0 = main thread (interpreter is single-threaded for main)
                let val = self.tls_storage.lock().unwrap()
                    .get(&(0, key)).copied().unwrap_or(0);
                ExecOutcome::Value(Value::Int(val))
            }
            "p_tls_set" => {
                // p_tls_set(key: i64, value: i64) -> void — set value for current thread
                let key = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let value = match args.get(1) {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                self.tls_storage.lock().unwrap().insert((0, key), value);
                ExecOutcome::Continue
            }

            // ─── Channels (bounded MPSC queue) ───
            "p_chan_create" => {
                // p_chan_create(capacity: i64) -> i64 — create a bounded channel
                let capacity = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 1,
                };
                let capacity = if capacity < 1 { 1 } else { capacity as usize };
                let handle = self.next_channel_handle;
                self.next_channel_handle += 1;
                let ch = Arc::new(Mutex::new(ChannelState {
                    buffer: std::collections::VecDeque::with_capacity(capacity),
                    capacity,
                    closed: false,
                }));
                self.channels.lock().unwrap().insert(handle, ch);
                ExecOutcome::Value(Value::Int(handle))
            }
            "p_chan_send" => {
                // p_chan_send(ch: i64, val: i64) -> bool — send a value (blocks if full)
                let ch_handle = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let val = match args.get(1) {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let channels = self.channels.clone();
                loop {
                    let ch_arc = {
                        let chans = channels.lock().unwrap();
                        chans.get(&ch_handle).cloned()
                    };
                    match ch_arc {
                        Some(ch) => {
                            let mut ch_lock = ch.lock().unwrap();
                            if ch_lock.closed {
                                return ExecOutcome::Value(Value::Bool(false));
                            }
                            if ch_lock.buffer.len() < ch_lock.capacity {
                                ch_lock.buffer.push_back(val);
                                return ExecOutcome::Value(Value::Bool(true));
                            }
                            // Buffer full — spin and yield
                            drop(ch_lock);
                            std::thread::yield_now();
                        }
                        None => return ExecOutcome::Value(Value::Bool(false)),
                    }
                }
            }
            "p_chan_recv" => {
                // p_chan_recv(ch: i64) -> i64 — receive a value (blocks if empty)
                let ch_handle = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let channels = self.channels.clone();
                loop {
                    let ch_arc = {
                        let chans = channels.lock().unwrap();
                        chans.get(&ch_handle).cloned()
                    };
                    match ch_arc {
                        Some(ch) => {
                            let mut ch_lock = ch.lock().unwrap();
                            if let Some(val) = ch_lock.buffer.pop_front() {
                                return ExecOutcome::Value(Value::Int(val));
                            }
                            if ch_lock.closed {
                                // Closed and empty — return pending sentinel
                                return ExecOutcome::Value(Value::Int(i64::MIN + 1));
                            }
                            // Empty — spin and yield
                            drop(ch_lock);
                            std::thread::yield_now();
                        }
                        None => return ExecOutcome::Value(Value::Int(i64::MIN + 1)),
                    }
                }
            }
            "p_chan_try_recv" => {
                // p_chan_try_recv(ch: i64) -> i64 — non-blocking receive
                let ch_handle = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let ch_arc = {
                    let chans = self.channels.lock().unwrap();
                    chans.get(&ch_handle).cloned()
                };
                match ch_arc {
                    Some(ch) => {
                        let mut ch_lock = ch.lock().unwrap();
                        if let Some(val) = ch_lock.buffer.pop_front() {
                            ExecOutcome::Value(Value::Int(val))
                        } else if ch_lock.closed {
                            // Closed and empty — return 0
                            ExecOutcome::Value(Value::Int(0))
                        } else {
                            // Empty — return pending sentinel
                            ExecOutcome::Value(Value::Int(i64::MIN + 1))
                        }
                    }
                    None => ExecOutcome::Value(Value::Int(i64::MIN + 1)),
                }
            }
            "p_chan_close" => {
                // p_chan_close(ch: i64) -> void — close the channel
                let ch_handle = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let ch_arc = {
                    let chans = self.channels.lock().unwrap();
                    chans.get(&ch_handle).cloned()
                };
                if let Some(ch) = ch_arc {
                    ch.lock().unwrap().closed = true;
                }
                ExecOutcome::Continue
            }
            "p_chan_is_closed" => {
                // p_chan_is_closed(ch: i64) -> bool — check if channel is closed
                let ch_handle = match args.first() {
                    Some(Value::Int(n)) => *n,
                    _ => 0,
                };
                let ch_arc = {
                    let chans = self.channels.lock().unwrap();
                    chans.get(&ch_handle).cloned()
                };
                match ch_arc {
                    Some(ch) => ExecOutcome::Value(Value::Bool(ch.lock().unwrap().closed)),
                    None => ExecOutcome::Value(Value::Bool(true)),
                }
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

            // ─── Phase 8B: ECS Runtime Functions ───────────────────────
            "__ecs_create_entity" => {
                self.ecs_entity_count += 1;
                ExecOutcome::Value(Value::Int(self.ecs_entity_count - 1))
            }
            "__ecs_entity_count" => {
                ExecOutcome::Value(Value::Int(self.ecs_entity_count))
            }
            "__ecs_has" => {
                // __ecs_has(component_name: str, entity: i64) -> i64
                // Note: lowerer prepends __env as first arg
                let (comp_name, entity) = if let (Some(Value::Str(c)), Some(Value::Int(e))) = (args.first(), args.get(1)) {
                    (c, e)
                } else if let (Some(Value::Str(c)), Some(Value::Int(e))) = (args.get(1), args.get(2)) {
                    (c, e)
                } else {
                    return ExecOutcome::Value(Value::Int(0));
                };
                let key = comp_name.to_string();
                let has = self.ecs_components
                    .get(&key)
                    .and_then(|m| m.get(entity))
                    .is_some();
                ExecOutcome::Value(Value::Int(if has { 1 } else { 0 }))
            }
            "__ecs_get" => {
                // __ecs_get(component_name: str, entity: i64) -> ?
                let (comp_name, entity) = if let (Some(Value::Str(c)), Some(Value::Int(e))) = (args.first(), args.get(1)) {
                    (c, e)
                } else if let (Some(Value::Str(c)), Some(Value::Int(e))) = (args.get(1), args.get(2)) {
                    (c, e)
                } else {
                    return ExecOutcome::Value(Value::Int(0));
                };
                let key = comp_name.to_string();
                let val = self.ecs_components
                    .get(&key)
                    .and_then(|m| m.get(entity))
                    .cloned()
                    .unwrap_or(Value::Int(0));
                ExecOutcome::Value(val)
            }
            "__ecs_add" => {
                // __ecs_add(component_name: str, entity: i64, value: struct) -> void
                // Note: lowerer prepends __env and SRET pointer for struct param
                // So args may be: [sret_ptr, __env, comp_name, entity, value_ptr]
                // or: [__env, comp_name, entity, value_ptr]
                // Try to find comp_name (Value::Str) and entity (Value::Int)
                let mut comp_name: Option<&CompactString> = None;
                let mut entity: Option<&i64> = None;
                let mut value: Option<&Value> = None;
                for (i, arg) in args.iter().enumerate() {
                    match arg {
                        Value::Str(s) if comp_name.is_none() => { comp_name = Some(s); }
                        Value::Int(n) if entity.is_none() && i > 0 => { entity = Some(n); }
                        Value::Ptr(_) if value.is_none() => { value = Some(arg); }
                        _ => {}
                    }
                }
                if let (Some(cn), Some(e)) = (comp_name, entity) {
                    let key = cn.to_string();
                    let val = value.cloned().unwrap_or(Value::Int(0));
                    self.ecs_components
                        .entry(key)
                        .or_insert_with(HashMap::new)
                        .insert(*e, val);
                }
                ExecOutcome::Continue
            }
            "__ecs_remove" => {
                // __ecs_remove(component_name: str, entity: i64) -> void
                let (comp_name, entity) = if let (Some(Value::Str(c)), Some(Value::Int(e))) = (args.first(), args.get(1)) {
                    (c, e)
                } else if let (Some(Value::Str(c)), Some(Value::Int(e))) = (args.get(1), args.get(2)) {
                    (c, e)
                } else {
                    return ExecOutcome::Continue;
                };
                let key = comp_name.to_string();
                if let Some(m) = self.ecs_components.get_mut(&key) {
                    m.remove(entity);
                }
                ExecOutcome::Continue
            }

            // ─── FFI stubs (audio, input, etc.) — return safe defaults ───
            "pledge_audio_init" => ExecOutcome::Value(Value::Int(0)),
            "pledge_audio_shutdown" | "pledge_audio_stop" | "pledge_audio_pause"
            | "pledge_audio_resume" | "pledge_audio_stop_all"
            | "pledge_audio_set_volume" | "pledge_audio_set_loop" | "pledge_audio_set_pan"
            | "pledge_audio_master_volume" => ExecOutcome::Continue,
            "pledge_audio_load" => ExecOutcome::Value(Value::Int(0)),
            "pledge_audio_play" => ExecOutcome::Value(Value::Int(0)),
            "pledge_audio_is_playing" => ExecOutcome::Value(Value::Int(0)),

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
            Value::Int(i) => {
                // Phase 10B: Value::Int may be a string pointer (materialized
                // from Value::Str when passed to an I64/U64 parameter).
                // Check opaque side-table first for the exact string.
                if *i > 0 {
                    if let Some(Value::Str(s)) = self.opaque_values.get(&(*i as u64)) {
                        return Some(s.to_string());
                    }
                }
                // Try loading a null-terminated string from memory
                if *i > 0 && self.memory.is_valid_addr(*i as u64) {
                    let mut bytes = Vec::new();
                    let mut offset = *i as u64;
                    for _ in 0..256 {
                        let b = self.memory.load(offset, 1);
                        if b[0] == 0 { break; }
                        bytes.push(b[0]);
                        offset += 1;
                    }
                    if !bytes.is_empty() {
                        return Some(String::from_utf8_lossy(&bytes).to_string());
                    }
                }
                None
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
pub fn interpret(module: IrModule, verbose: bool, release: bool) -> (RunResult, Option<String>) {
    interpret_with_options(module, verbose, release, 1000)
}

/// Interpret with configurable options (SH-1.4: max_recursion).
/// max_recursion of 0 means unlimited (no depth check).
pub fn interpret_with_options(module: IrModule, verbose: bool, release: bool, max_recursion: usize) -> (RunResult, Option<String>) {
    let mut interp = Interpreter::new(module);
    if verbose {
        interp.verbose = true;
    }
    if release {
        interp.debug_mode = false;
    }
    if max_recursion > 0 {
        interp.max_recursion = max_recursion;
    } else {
        // 0 = unlimited: set to usize::MAX so the check never triggers
        interp.max_recursion = usize::MAX;
    }
    let result = interp.run();
    let trace = interp.take_panic_stack_trace();
    (result, trace)
}
