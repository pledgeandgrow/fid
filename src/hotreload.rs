// Hot Reload System — Phase 4E
//
// Provides fast development iteration by:
//   1. Native JIT: compiles PIR L3 → ELF64 → execute (no external tools)
//   2. Function-level change detection: hash L2 IR to detect which functions changed
//   3. State preservation: global state survives reloads via temp file
//   4. Watch mode: auto-recompiles and runs on file change
//   5. Debugger integration: breakpoints and stepping tracking (structural)
//
// The JIT uses the existing build_executable pipeline (PIR → codegen → peephole
// → ELF64) to produce a native executable, then runs it as a child process.
// This reuses the working codegen + linker rather than trying to JIT individual
// functions (which would require resolving relocations between user code and
// the runtime — a much harder problem).
//
// Function-level hot-swap detection works by hashing each L2 function's debug
// representation. When a file changes, we recompile to L2 and compare hashes
// to report which functions were added, modified, or removed.

use std::collections::HashMap;
use std::path::PathBuf;
use std::time::{Duration, Instant, SystemTime};

// ═══════════════════════════════════════════════════════════════════════════
// Function Change Detection
// ═══════════════════════════════════════════════════════════════════════════

/// A snapshot of function hashes from a compilation.
/// Used to detect which functions changed between reloads.
pub struct FunctionSnapshot {
    /// Map from function name to content hash
    hashes: HashMap<String, u64>,
}

impl FunctionSnapshot {
    /// Build a snapshot from an L2 module by hashing each function's debug repr.
    pub fn from_l2(module: &crate::pir::L2Module) -> Self {
        let mut hashes = HashMap::new();
        for func in &module.functions {
            // Hash the function name + blocks + values
            let mut content = format!("{}|{:?}|{:?}|{:?}", func.name, func.params, func.ret_ty, func.entry);
            for block in &func.blocks {
                content.push_str(&format!("|{}|{:?}|{:?}", block.id, block.instructions, block.params));
            }
            // Sort value entries for deterministic hashing
            let mut entries: Vec<_> = func.values.iter().collect();
            entries.sort_by_key(|(k, _)| format!("{:?}", k));
            for (k, v) in entries {
                content.push_str(&format!("|{:?}:{:?}", k, v));
            }
            hashes.insert(func.name.clone(), hash_str(&content));
        }
        FunctionSnapshot { hashes }
    }

    /// Compare this snapshot with a previous one and return the diff.
    pub fn diff(&self, prev: &FunctionSnapshot) -> FunctionDiff {
        let mut added = Vec::new();
        let mut modified = Vec::new();
        let mut removed = Vec::new();

        for (name, hash) in &self.hashes {
            match prev.hashes.get(name) {
                None => added.push(name.clone()),
                Some(prev_hash) if prev_hash != hash => modified.push(name.clone()),
                _ => {}
            }
        }

        for name in prev.hashes.keys() {
            if !self.hashes.contains_key(name) {
                removed.push(name.clone());
            }
        }

        added.sort();
        modified.sort();
        removed.sort();

        FunctionDiff { added, modified, removed }
    }

    /// Get all function names in this snapshot.
    pub fn function_names(&self) -> Vec<&str> {
        self.hashes.keys().map(|s| s.as_str()).collect()
    }
}

/// The result of comparing two function snapshots.
#[derive(Debug, Clone)]
pub struct FunctionDiff {
    /// Functions that exist in the new snapshot but not the old.
    pub added: Vec<String>,
    /// Functions that exist in both but have different content.
    pub modified: Vec<String>,
    /// Functions that existed in the old snapshot but not the new.
    pub removed: Vec<String>,
}

impl FunctionDiff {
    /// Returns true if nothing changed.
    pub fn is_empty(&self) -> bool {
        self.added.is_empty() && self.modified.is_empty() && self.removed.is_empty()
    }

    /// Total number of changed functions.
    pub fn total_changes(&self) -> usize {
        self.added.len() + self.modified.len() + self.removed.len()
    }
}

/// Simple FNV-1a hash for strings.
fn hash_str(s: &str) -> u64 {
    let mut hash: u64 = 0xcbf29ce484222325;
    for byte in s.bytes() {
        hash ^= byte as u64;
        hash = hash.wrapping_mul(0x100000001b3);
    }
    hash
}

// ═══════════════════════════════════════════════════════════════════════════
// Native JIT Execution
// ═══════════════════════════════════════════════════════════════════════════

/// Result of a JIT execution.
#[derive(Debug)]
pub struct JitResult {
    /// Exit code of the program
    pub exit_code: i32,
    /// Stdout output
    pub stdout: String,
    /// Stderr output
    pub stderr: String,
    /// Time taken to compile (in milliseconds)
    pub compile_time_ms: u64,
    /// Time taken to execute (in milliseconds)
    pub exec_time_ms: u64,
    /// Size of the compiled binary (in bytes)
    pub binary_size: usize,
}

/// Compile source code to a native ELF64 executable and run it.
/// Uses the full PIR pipeline (L1→L2→L3 → codegen → peephole → ELF64).
/// Zero external tools required.
pub fn compile_and_run(source: &str, source_path: &str, do_opt: bool) -> Result<JitResult, String> {
    let compile_start = Instant::now();

    let binary = crate::compiler::build_executable(source, source_path, "x86-64", do_opt, None)
        .map_err(|errors| errors.join("\n"))?;

    let compile_time_ms = compile_start.elapsed().as_millis() as u64;
    let binary_size = binary.len();

    // Write to temp file, make executable, run it
    let tmp_path = std::env::temp_dir().join(format!(
        "pledge_jit_{}.bin",
        std::process::id()
    ));

    std::fs::write(&tmp_path, &binary)
        .map_err(|e| format!("Failed to write JIT binary: {}", e))?;

    // Set executable permissions (Unix only)
    #[cfg(unix)]
    {
        use std::os::unix::fs::PermissionsExt;
        let mut perms = std::fs::metadata(&tmp_path)
            .map_err(|e| format!("Failed to stat JIT binary: {}", e))?
            .permissions();
        perms.set_mode(0o755);
        std::fs::set_permissions(&tmp_path, perms)
            .map_err(|e| format!("Failed to chmod JIT binary: {}", e))?;
    }

    let exec_start = Instant::now();

    let output = std::process::Command::new(&tmp_path)
        .output()
        .map_err(|e| {
            // Clean up temp file on error
            let _ = std::fs::remove_file(&tmp_path);
            format!("Failed to execute JIT binary: {}", e)
        })?;

    let exec_time_ms = exec_start.elapsed().as_millis() as u64;

    // Clean up temp file
    let _ = std::fs::remove_file(&tmp_path);

    let stdout = String::from_utf8_lossy(&output.stdout).to_string();
    let stderr = String::from_utf8_lossy(&output.stderr).to_string();
    let exit_code = output.status.code().unwrap_or(-1);

    Ok(JitResult {
        exit_code,
        stdout,
        stderr,
        compile_time_ms,
        exec_time_ms,
        binary_size,
    })
}

// ═══════════════════════════════════════════════════════════════════════════
// State Preservation
// ═══════════════════════════════════════════════════════════════════════════

/// State that persists across hot reloads.
/// Stored in a temp file so it survives between child process executions.
pub struct PersistentState {
    /// Path to the state file
    state_file: PathBuf,
    /// Key-value state
    data: HashMap<String, String>,
}

impl PersistentState {
    /// Create a new persistent state for the given source file.
    pub fn for_source(source_path: &str) -> Self {
        let state_file = std::env::temp_dir().join(format!(
            "pledge_state_{}.json",
            hash_str(source_path)
        ));

        let mut state = PersistentState {
            state_file,
            data: HashMap::new(),
        };
        state.load();
        state
    }

    /// Load state from the temp file.
    fn load(&mut self) {
        if let Ok(content) = std::fs::read_to_string(&self.state_file) {
            // Simple line-based format: key=value
            for line in content.lines() {
                if let Some(eq_pos) = line.find('=') {
                    let key = &line[..eq_pos];
                    let value = &line[eq_pos + 1..];
                    self.data.insert(key.to_string(), value.to_string());
                }
            }
        }
    }

    /// Save state to the temp file.
    pub fn save(&self) {
        let content: String = self.data.iter()
            .map(|(k, v)| format!("{}={}", k, v))
            .collect::<Vec<_>>()
            .join("\n");
        let _ = std::fs::write(&self.state_file, content);
    }

    /// Set a state value.
    pub fn set(&mut self, key: &str, value: &str) {
        self.data.insert(key.to_string(), value.to_string());
    }

    /// Get a state value.
    pub fn get(&self, key: &str) -> Option<&str> {
        self.data.get(key).map(|s| s.as_str())
    }

    /// Clear all state.
    pub fn clear(&mut self) {
        self.data.clear();
        let _ = std::fs::remove_file(&self.state_file);
    }
}

impl Drop for PersistentState {
    fn drop(&mut self) {
        self.save();
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Watch Mode
// ═══════════════════════════════════════════════════════════════════════════

/// File watcher: monitors source files for changes and triggers recompilation.
pub struct Watcher {
    files: Vec<(PathBuf, SystemTime)>,
    callback: Box<dyn Fn(&str) + Send>,
}

impl Watcher {
    pub fn new(callback: impl Fn(&str) + Send + 'static) -> Self {
        Watcher {
            files: Vec::new(),
            callback: Box::new(callback),
        }
    }

    /// Add a file to watch
    pub fn watch(&mut self, path: impl AsRef<std::path::Path>) {
        let path = path.as_ref().to_path_buf();
        if let Ok(metadata) = std::fs::metadata(&path) {
            if let Ok(modified) = metadata.modified() {
                self.files.push((path.clone(), modified));
            }
        }
    }

    /// Check all watched files for changes. Returns true if any file changed.
    pub fn poll(&mut self) -> bool {
        let mut changed = false;
        for (path, last_modified) in &mut self.files {
            if let Ok(metadata) = std::fs::metadata(&*path) {
                if let Ok(modified) = metadata.modified() {
                    if modified != *last_modified {
                        *last_modified = modified;
                        changed = true;
                        let path_str = path.to_str().unwrap_or("").to_string();
                        (self.callback)(&path_str);
                    }
                }
            }
        }
        changed
    }

    /// Run the watcher loop, polling at the given interval (in milliseconds).
    /// Calls the callback for each changed file.
    pub fn run(&mut self, interval_ms: u64) {
        loop {
            self.poll();
            std::thread::sleep(Duration::from_millis(interval_ms));
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Debugger Integration
// ═══════════════════════════════════════════════════════════════════════════

/// Debug state for the hot-reload debugger.
/// Tracks call stack and current position for debugging hot-reloaded code.
pub struct Debugger {
    /// Current function being debugged
    current_function: Option<String>,
    /// Current instruction offset
    current_offset: usize,
    /// Whether to step into function calls
    step_into: bool,
    /// Call stack (function names)
    call_stack: Vec<String>,
}

impl Debugger {
    pub fn new() -> Self {
        Debugger {
            current_function: None,
            current_offset: 0,
            step_into: false,
            call_stack: Vec::new(),
        }
    }

    /// Enter a function (push onto call stack)
    pub fn enter_function(&mut self, name: &str) {
        self.call_stack.push(name.to_string());
        self.current_function = Some(name.to_string());
        self.current_offset = 0;
    }

    /// Exit the current function (pop from call stack)
    pub fn exit_function(&mut self) {
        self.call_stack.pop();
        self.current_function = self.call_stack.last().cloned();
        self.current_offset = 0;
    }

    /// Advance the instruction pointer
    pub fn step(&mut self) {
        self.current_offset += 1;
    }

    /// Get the current call stack depth
    pub fn stack_depth(&self) -> usize {
        self.call_stack.len()
    }

    /// Get the current function name
    pub fn current_function(&self) -> Option<&str> {
        self.current_function.as_deref()
    }

    /// Get the current instruction offset
    pub fn current_offset(&self) -> usize {
        self.current_offset
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_function_snapshot_diff() {
        // Build a snapshot with two functions
        let mut hashes1 = HashMap::new();
        hashes1.insert("main".to_string(), 1u64);
        hashes1.insert("helper".to_string(), 2u64);
        let snap1 = FunctionSnapshot { hashes: hashes1 };

        // Build a snapshot with one modified, one added, one removed
        let mut hashes2 = HashMap::new();
        hashes2.insert("main".to_string(), 1u64);      // unchanged
        hashes2.insert("helper".to_string(), 99u64);   // modified
        hashes2.insert("new_func".to_string(), 3u64);  // added
        let snap2 = FunctionSnapshot { hashes: hashes2 };

        let diff = snap2.diff(&snap1);
        assert_eq!(diff.added, vec!["new_func".to_string()]);
        assert_eq!(diff.modified, vec!["helper".to_string()]);
        assert!(diff.removed.is_empty());
        assert!(!diff.is_empty());
        assert_eq!(diff.total_changes(), 2);
    }

    #[test]
    fn test_function_snapshot_no_changes() {
        let mut hashes = HashMap::new();
        hashes.insert("main".to_string(), 1u64);
        let snap1 = FunctionSnapshot { hashes: hashes.clone() };
        let snap2 = FunctionSnapshot { hashes };
        let diff = snap2.diff(&snap1);
        assert!(diff.is_empty());
    }

    #[test]
    fn test_function_snapshot_removed() {
        let mut hashes1 = HashMap::new();
        hashes1.insert("main".to_string(), 1u64);
        hashes1.insert("old_func".to_string(), 2u64);
        let snap1 = FunctionSnapshot { hashes: hashes1 };

        let mut hashes2 = HashMap::new();
        hashes2.insert("main".to_string(), 1u64);
        let snap2 = FunctionSnapshot { hashes: hashes2 };

        let diff = snap2.diff(&snap1);
        assert_eq!(diff.removed, vec!["old_func".to_string()]);
        assert!(diff.added.is_empty());
        assert!(diff.modified.is_empty());
    }

    #[test]
    fn test_persistent_state() {
        let state_path = "/tmp/pledge_test_state_unique_12345.json";
        let _ = std::fs::remove_file(state_path);

        {
            let mut state = PersistentState {
                state_file: PathBuf::from(state_path),
                data: HashMap::new(),
            };
            state.set("counter", "42");
            state.set("name", "pledge");
            state.save();
        }

        // Load in a new instance
        let state = PersistentState {
            state_file: PathBuf::from(state_path),
            data: HashMap::new(),
        };
        let mut state = state;
        state.load();
        assert_eq!(state.get("counter"), Some("42"));
        assert_eq!(state.get("name"), Some("pledge"));
        assert_eq!(state.get("nonexistent"), None);

        state.clear();
        assert_eq!(state.get("counter"), None);
    }

    #[test]
    fn test_compile_and_run_hello() {
        let source = "pf main() {\n    print(\"hot reload works!\");\n}\n";
        let result = compile_and_run(source, "test.p", false);
        assert!(result.is_ok(), "compile_and_run should succeed");
        let result = result.unwrap();
        assert_eq!(result.exit_code, 17); // print returns bytes written ("hot reload works!" = 17 chars)
        assert!(result.stdout.contains("hot reload works!"));
        assert!(result.binary_size > 0);
    }

    #[test]
    fn test_compile_and_run_with_opt() {
        let source = "pf main() {\n    print(\"optimized!\");\n}\n";
        let result = compile_and_run(source, "test.p", true);
        assert!(result.is_ok(), "compile_and_run with opt should succeed");
        let result = result.unwrap();
        assert!(result.stdout.contains("optimized!"));
    }

    #[test]
    fn test_debugger() {
        let mut dbg = Debugger::new();
        assert_eq!(dbg.stack_depth(), 0);

        dbg.enter_function("main");
        assert_eq!(dbg.stack_depth(), 1);
        assert_eq!(dbg.current_function(), Some("main"));
        assert_eq!(dbg.current_offset(), 0);

        dbg.step();
        dbg.step();
        assert_eq!(dbg.current_offset(), 2);

        dbg.enter_function("helper");
        assert_eq!(dbg.stack_depth(), 2);
        assert_eq!(dbg.current_function(), Some("helper"));

        dbg.exit_function();
        assert_eq!(dbg.stack_depth(), 1);
        assert_eq!(dbg.current_function(), Some("main"));
    }

    #[test]
    fn test_watcher() {
        let mut watcher = Watcher::new(|_path| {});
        // Just verify it doesn't crash
        watcher.watch("/tmp/nonexistent_file_for_test");
        assert!(!watcher.poll()); // file doesn't exist, no change
    }

    #[test]
    fn test_watcher_detects_changes() {
        // Create a temp file, watch it, modify it, verify poll detects the change
        let tmp = std::env::temp_dir().join("pledge_watcher_test.txt");
        std::fs::write(&tmp, "initial").unwrap();

        let mut watcher = Watcher::new(|_| {});
        watcher.watch(&tmp);

        // No change yet
        assert!(!watcher.poll());

        // Wait a moment so the modification time differs
        std::thread::sleep(std::time::Duration::from_millis(50));
        std::fs::write(&tmp, "modified").unwrap();

        // Should detect the change
        assert!(watcher.poll());

        // Cleanup
        let _ = std::fs::remove_file(&tmp);
    }
}
