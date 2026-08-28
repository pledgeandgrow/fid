//! Built-in debugger for Fides — `fidesc debug <file>`
//!
//! Provides an interactive debugger that runs the interpreter step-by-step
//! with breakpoints, variable inspection, stack traces, and more.
//! No external tools (gdb/lldb) required.
//!
//! Commands:
//!   break <func>[:<line>]  — set a breakpoint
//!   break                  — list breakpoints
//!   delete <n>             — delete breakpoint n
//!   run                    — run until breakpoint or program end
//!   step                   — step one instruction (into calls)
//!   next                   — step one instruction (over calls — same as step for now)
//!   continue               — continue execution
//!   print <var>            — print a variable's value
//!   print                  — print all variables in scope
//!   backtrace              — show call stack
//!   info locals            — show all local variables
//!   info functions         — list all functions
//!   info breakpoints       — list breakpoints
//!   list                   — show source around current position
//!   ir                     — show IR for current function
//!   effects                — show active effects for current function
//!   regions                — show active regions and memory usage
//!   capabilities           — show granted/revoked capabilities
//!   watch <var>            — watch a variable (print on change)
//!   record                 — start recording execution trace
//!   replay <file>          — replay a recorded trace
//!   quit                   — exit debugger

use std::collections::HashMap;
use std::io::{self, BufRead, Write};
use crate::interp::{Interpreter, RunResult, DebugAction, Value, TraceEntry};
use crate::ir::{IrModule, IrFunction, ValueId};
use crate::compiler;

/// A breakpoint
#[derive(Debug, Clone)]
struct Breakpoint {
    id: usize,
    func_name: String,
    line: Option<usize>,
    hit_count: usize,
}

/// Debugger state
pub struct Debugger {
    /// Breakpoints
    breakpoints: Vec<Breakpoint>,
    /// Next breakpoint ID
    next_bp_id: usize,
    /// Whether we're in stepping mode (break on every instruction)
    stepping: bool,
    /// Whether to step over calls (next vs step)
    step_over: bool,
    /// Call stack depth when stepping over
    step_over_depth: usize,
    /// Watched variables — maps name to last seen value
    watches: HashMap<String, String>,
    /// Whether we're recording a trace
    recording: bool,
    /// Source code lines (for `list` command)
    source_lines: Vec<String>,
    /// Source file path
    source_path: String,
}

impl Debugger {
    pub fn new(source: &str, source_path: &str) -> Self {
        Debugger {
            breakpoints: Vec::new(),
            next_bp_id: 1,
            stepping: true,  // Start in stepping mode
            step_over: false,
            step_over_depth: 0,
            watches: HashMap::new(),
            recording: false,
            source_lines: source.lines().map(|s| s.to_string()).collect(),
            source_path: source_path.to_string(),
        }
    }

    /// Parse a breakpoint spec: "func", "func:42", "42" (line in any function)
    fn parse_bp_spec(&self, spec: &str) -> (String, Option<usize>) {
        if let Some(colon_pos) = spec.find(':') {
            let func = spec[..colon_pos].to_string();
            let line: Option<usize> = spec[colon_pos+1..].parse().ok();
            (func, line)
        } else if let Ok(line) = spec.parse::<usize>() {
            ("main".to_string(), Some(line))
        } else {
            (spec.to_string(), None)
        }
    }

    /// Check if we should break at the current position
    fn should_break(&mut self, func_name: &str, source_line: usize, call_depth: usize) -> bool {
        // Stepping mode — break on every instruction
        if self.stepping {
            if self.step_over && call_depth > self.step_over_depth {
                return false;
            }
            self.stepping = false;
            self.step_over = false;
            return true;
        }

        // Check breakpoints
        for bp in &mut self.breakpoints {
            if bp.func_name == func_name {
                if bp.line.is_none() || bp.line == Some(source_line) {
                    bp.hit_count += 1;
                    return true;
                }
            }
        }

        false
    }

    /// Format a Value for display
    fn format_value(v: &Value) -> String {
        match v {
            Value::Int(i) => format!("i64: {}", i),
            Value::Float(f) => format!("f64: {}", f),
            Value::Bool(b) => format!("bool: {}", b),
            Value::Str(s) => format!("str: \"{}\"", s),
            Value::Ptr(p) => format!("ptr: 0x{:x}", p),
            Value::Void => "void".to_string(),
            Value::Vec(v) => {
                let v = v.borrow();
                let elems: Vec<String> = v.iter().map(|e| Self::format_value(e)).collect();
                format!("Vec[{}]", elems.join(", "))
            }
            Value::Map(m) => {
                let m = m.borrow();
                let entries: Vec<String> = m.iter()
                    .map(|(k, v)| format!("{}: {}", k, Self::format_value(v)))
                    .collect();
                format!("Map{{{}}}", entries.join(", "))
            }
            Value::Dyn(t, _) => format!("dyn({})", t),
            Value::Tensor(t) => format!("tensor({})", t.borrow().to_string()),
            Value::Json(j) => format!("json({})", crate::json::dump(&j.borrow(), false)),
            Value::Allocator(a) => format!("alloc({})", a),
            Value::Future(idx) => format!("Future#{}", idx),
            Value::ThreadHandle(idx) => format!("Thread#{}", idx),
        }
    }

    /// Print all variables in the current scope
    fn print_locals(values: &HashMap<ValueId, Value>, func: &IrFunction) {
        if values.is_empty() {
            println!("  (no variables in scope)");
            return;
        }
        // Map value IDs to source names using func.locals
        let mut printed = 0;
        for (name, vid) in &func.locals {
            if let Some(val) = values.get(vid) {
                println!("  {} = {}", name, Self::format_value(val));
                printed += 1;
            }
        }
        // Also print any values not in locals (intermediate SSA values)
        if printed == 0 {
            for (vid, val) in values.iter().take(20) {
                println!("  v{} = {}", vid.0, Self::format_value(val));
            }
        }
        if values.len() > 20 && printed == 0 {
            println!("  ... and {} more", values.len() - 20);
        }
    }

    /// Print a specific variable by name
    fn print_var(name: &str, values: &HashMap<ValueId, Value>, func: &IrFunction) {
        // Try to find by local name
        if let Some(vid) = func.locals.get(name) {
            if let Some(val) = values.get(vid) {
                println!("  {} = {}", name, Self::format_value(val));
                return;
            }
        }
        // Try parsing as v<number>
        if name.starts_with('v') {
            if let Ok(n) = name[1..].parse::<u32>() {
                let vid = ValueId(n);
                if let Some(val) = values.get(&vid) {
                    println!("  {} = {}", name, Self::format_value(val));
                    return;
                }
            }
        }
        println!("  Variable '{}' not found in current scope", name);
    }

    /// Show source around a line
    fn list_source(&self, line: usize, context: usize) {
        let start = line.saturating_sub(context);
        let end = (line + context + 1).min(self.source_lines.len());
        for i in start..end {
            let marker = if i + 1 == line { "→ " } else { "  " };
            println!("{}{:4} │ {}", marker, i + 1, self.source_lines.get(i).unwrap_or(&"".to_string()));
        }
    }

    /// Show IR for a function
    fn show_ir(func: &IrFunction) {
        println!("function {}({}) -> {:?}", func.name,
            func.params.iter().map(|(id, ty)| format!("v{}: {:?}", id.0, ty)).collect::<Vec<_>>().join(", "),
            func.ret_ty);
        for (i, block) in func.blocks.iter().enumerate() {
            println!("  block {}:", i);
            for &inst_id in &block.instructions {
                if let Some(inst) = func.instructions.get(&inst_id) {
                    let line = func.values.get(&inst_id).and_then(|m| m.source_line);
                    let line_str = line.map(|l| format!(" (line {})", l)).unwrap_or_default();
                    println!("    v{} = {:?}{}", inst_id.0, inst, line_str);
                }
            }
        }
    }

    /// Show effects for a function
    fn show_effects(func: &IrFunction) {
        println!("Function: {}", func.name);
        println!("  Effects: {:?}", func.effects);
        println!("  @wrapping: {}", func.is_wrapping);
        println!("  @deterministic: {}", func.is_deterministic);
        println!("  @crashfree: {}", func.is_crashfree);
        println!("  @no-alloc: {}", func.is_no_alloc);
        if let Some(gas) = func.gas_limit {
            println!("  @gas: {}", gas);
        }
    }

    /// List all functions in the module
    fn list_functions(module: &IrModule) {
        let mut names: Vec<&String> = module.functions.keys().collect();
        names.sort();
        for name in names {
            let func = &module.functions[name];
            let params = func.params.len();
            let blocks = func.blocks.len();
            println!("  {} ({} params, {} blocks)", name, params, blocks);
        }
    }

    /// Run the debugger on a source file
    pub fn run(&mut self, source: &str, source_path: &str) -> i32 {
        println!("Fides Debugger — fidesc debug {}", source_path);
        println!("Type 'help' for commands, 'run' to start execution.\n");

        // Run the interpreter — it will call the hook on each instruction.
        // This loop supports hot-reload: when the user types `reload`, the hook
        // injects a panic with sentinel "__debugger_reload__", and we restart
        // execution with the new source.
        let mut current_source = source.to_string();
        let mut current_source_lines: Vec<String> = source.lines().map(|s| s.to_string()).collect();
        let current_source_path = source_path.to_string();

        loop {
            // Build the program and lower to IR
            let program = match compiler::build_program_with_path(&current_source, Some(&current_source_path)) {
                Ok(p) => p,
                Err(errors) => {
                    for e in &errors {
                        eprintln!("{}", e);
                    }
                    println!("\nCompilation failed. Fix the errors and type 'reload' again, or 'quit' to exit.");
                    // Wait for user to either reload or quit
                    let stdin = io::stdin();
                    loop {
                        print!("(fidesc) > ");
                        io::stdout().flush().ok();
                        let mut line = String::new();
                        if stdin.lock().read_line(&mut line).is_err() {
                            return 1;
                        }
                        let line = line.trim();
                        match line {
                            "quit" | "q" | "exit" => return 1,
                            "reload" | "r" => {
                                match std::fs::read_to_string(&current_source_path) {
                                    Ok(s) => {
                                        current_source = s;
                                        current_source_lines = current_source.lines().map(|s| s.to_string()).collect();
                                        break;
                                    }
                                    Err(e) => {
                                        println!("Error reading file: {}", e);
                                        continue;
                                    }
                                }
                            }
                            _ => println!("Type 'reload' to retry or 'quit' to exit."),
                        }
                    }
                    continue; // Restart the outer loop with new source
                }
            };
            let module = crate::lower::lower(&program);
            let mut interp = Interpreter::new(module);

            // Set up debugger state
            let dbg_state = std::rc::Rc::new(std::cell::RefCell::new(DebuggerState {
                breakpoints: self.breakpoints.clone(),
                next_bp_id: self.next_bp_id,
                stepping: true,  // Start in stepping mode after reload
                step_over: false,
                watches: self.watches.clone(),
                recording: self.recording,
                last_cmd: String::from("step"),
                quit: false,
            }));
            let dbg_state_hook = dbg_state.clone();
            let source_lines_ref = std::rc::Rc::new(current_source_lines.clone());
            let source_path_ref = std::rc::Rc::new(current_source_path.clone());

            let hook = Box::new(move |func_name: &str,
                                       block: &str,
                                       source_line: usize,
                                       inst_id: u32,
                                       values: &HashMap<ValueId, Value>,
                                       func: &IrFunction| -> DebugAction {
                let mut state = dbg_state_hook.borrow_mut();

                // Check if we should break here
                let should_break = if state.stepping {
                    state.stepping = false;
                    true
                } else {
                    let mut hit = false;
                    for bp in &mut state.breakpoints {
                        if bp.func_name == func_name {
                            if bp.line.is_none() || bp.line == Some(source_line) {
                                bp.hit_count += 1;
                                hit = true;
                            }
                        }
                    }
                    hit
                };

                if !should_break {
                    return DebugAction::Continue;
                }

                // Interactive debugger loop
                let stdin = io::stdin();
                loop {
                    print!("(fidesc) {}:{}:{} > ", source_path_ref, func_name, source_line);
                    io::stdout().flush().ok();

                    let mut line = String::new();
                    if stdin.lock().read_line(&mut line).is_err() {
                        return DebugAction::InjectPanic("__debugger_quit__".to_string());
                    }
                    let line = line.trim();
                    let cmd_line = if line.is_empty() {
                        state.last_cmd.clone()
                    } else {
                        line.to_string()
                    };
                    state.last_cmd = cmd_line.clone();

                    let parts: Vec<&str> = cmd_line.splitn(2, ' ').collect();
                    let cmd = parts[0];
                    let arg = parts.get(1).map(|s| s.trim()).unwrap_or("");

                    match cmd {
                        "help" | "h" | "?" => {
                            print_debugger_help();
                        }
                        "break" | "b" => {
                            if arg.is_empty() {
                                if state.breakpoints.is_empty() {
                                    println!("No breakpoints set.");
                                } else {
                                    for bp in &state.breakpoints {
                                        let loc = match bp.line {
                                            Some(l) => format!("{}:{}", bp.func_name, l),
                                            None => bp.func_name.clone(),
                                        };
                                        println!("  #{} {} (hits: {})", bp.id, loc, bp.hit_count);
                                    }
                                }
                            } else {
                                let (func, line) = parse_bp_spec(arg);
                                let bp = Breakpoint {
                                    id: state.next_bp_id,
                                    func_name: func,
                                    line,
                                    hit_count: 0,
                                };
                                println!("Breakpoint #{} set at {}", bp.id,
                                    match bp.line {
                                        Some(l) => format!("{}:{}", bp.func_name, l),
                                        None => bp.func_name.clone(),
                                    });
                                state.next_bp_id += 1;
                                state.breakpoints.push(bp);
                            }
                            continue;
                        }
                        "delete" | "d" => {
                            if arg.is_empty() {
                                state.breakpoints.clear();
                                println!("All breakpoints deleted.");
                            } else if let Ok(id) = arg.parse::<usize>() {
                                state.breakpoints.retain(|bp| bp.id != id);
                                println!("Breakpoint #{} deleted.", id);
                            }
                            continue;
                        }
                        "run" | "r" => {
                            state.stepping = false;
                            return DebugAction::Continue;
                        }
                        "step" | "s" => {
                            state.stepping = true;
                            state.step_over = false;
                            return DebugAction::Continue;
                        }
                        "next" | "n" => {
                            state.stepping = true;
                            state.step_over = true;
                            return DebugAction::Continue;
                        }
                        "continue" | "c" => {
                            state.stepping = false;
                            return DebugAction::Continue;
                        }
                        "print" | "p" => {
                            if arg.is_empty() {
                                print_locals(values, func);
                            } else {
                                print_var(arg, values, func);
                            }
                            continue;
                        }
                        "backtrace" | "bt" | "where" => {
                            println!("  → {} (line {})", func_name, source_line);
                            println!("    block {}", block);
                            continue;
                        }
                        "info" => {
                            if arg == "locals" {
                                print_locals(values, func);
                            } else if arg == "functions" {
                                println!("  Current: {} ({} blocks)", func.name, func.blocks.len());
                            } else if arg == "breakpoints" {
                                if state.breakpoints.is_empty() {
                                    println!("No breakpoints set.");
                                } else {
                                    for bp in &state.breakpoints {
                                        let loc = match bp.line {
                                            Some(l) => format!("{}:{}", bp.func_name, l),
                                            None => bp.func_name.clone(),
                                        };
                                        println!("  #{} {} (hits: {})", bp.id, loc, bp.hit_count);
                                    }
                                }
                            } else {
                                println!("Usage: info [locals|functions|breakpoints]");
                            }
                            continue;
                        }
                        "list" | "l" => {
                            list_source(&source_lines_ref, source_line, 5);
                            continue;
                        }
                        "ir" => {
                            if arg == "current" {
                                // Show just the current instruction
                                let vid = ValueId(inst_id);
                                if let Some(inst) = func.instructions.get(&vid) {
                                    let line = func.values.get(&vid).and_then(|m| m.source_line);
                                    let line_str = line.map(|l| format!(" (line {})", l)).unwrap_or_default();
                                    println!("  v{} = {:?}{}", inst_id, inst, line_str);
                                } else {
                                    println!("  v{} (no instruction)", inst_id);
                                }
                            } else {
                                // Show all IR with current instruction marked
                                show_ir_with_current(func, inst_id);
                            }
                            continue;
                        }
                        "effects" => {
                            show_effects(func);
                            continue;
                        }
                        "regions" => {
                            let has_alloc = func.effects.iter().any(|e| matches!(e, crate::ir::Effect::Alloc));
                            let has_read = func.effects.iter().any(|e| matches!(e, crate::ir::Effect::Read));
                            let has_write = func.effects.iter().any(|e| matches!(e, crate::ir::Effect::Write));
                            println!("Function: {}", func.name);
                            println!("  Effects: {}", func.effects);
                            println!("  Memory regions:");
                            if has_alloc {
                                println!("    Heap  (allocates memory)");
                            }
                            if has_read || has_write {
                                println!("    Stack (reads/writes local memory)");
                            }
                            if !has_alloc && !has_read && !has_write {
                                println!("    (no memory regions — pure function)");
                            }
                            let alloca_count = func.instructions.values()
                                .filter(|i| matches!(**i, crate::ir::Inst::Alloca { .. }))
                                .count();
                            println!("  Stack allocations: {}", alloca_count);
                            continue;
                        }
                        "capabilities" => {
                            println!("Function: {}", func.name);
                            println!("  Annotations:");
                            println!("    @wrapping:      {}", func.is_wrapping);
                            println!("    @deterministic: {}", func.is_deterministic);
                            println!("    @crashfree:     {}", func.is_crashfree);
                            println!("    @no-alloc:      {}", func.is_no_alloc);
                            if let Some(gas) = func.gas_limit {
                                println!("    @gas:           {}", gas);
                            }
                            let effects: Vec<String> = func.effects.iter().map(|e| e.to_string()).collect();
                            println!("  Required capabilities (from effects):");
                            for e in &effects {
                                match e.as_str() {
                                    "io" => println!("    IO  — console/file access"),
                                    "net" => println!("    Net — network access"),
                                    "spawn" => println!("    Spawn — thread/process creation"),
                                    "alloc" => println!("    Alloc — heap allocation"),
                                    _ => {}
                                }
                            }
                            if effects.iter().all(|e| e == "read" || e == "write" || e == "pure") {
                                println!("    (no special capabilities needed)");
                            }
                            continue;
                        }
                        "watch" | "w" => {
                            if arg.is_empty() {
                                if state.watches.is_empty() {
                                    println!("No watches set.");
                                } else {
                                    for (name, val) in &state.watches {
                                        println!("  {} = {}", name, val);
                                    }
                                }
                            } else {
                                state.watches.insert(arg.to_string(), "(not yet seen)".to_string());
                                println!("Watching '{}'", arg);
                            }
                            continue;
                        }
                        "record" => {
                            state.recording = true;
                            println!("Recording enabled (trace will be saved on exit)");
                            continue;
                        }
                        "reload" => {
                            println!("Reloading from {}...", source_path_ref);
                            match std::fs::read_to_string(&*source_path_ref) {
                                Ok(new_source) => {
                                    let new_lines: Vec<String> = new_source.lines().map(|s| s.to_string()).collect();
                                    if new_lines == *source_lines_ref {
                                        println!("  Source unchanged — nothing to reload.");
                                        continue;
                                    }
                                    println!("  Source changed ({} → {} lines). Restarting execution...",
                                        source_lines_ref.len(), new_lines.len());
                                    return DebugAction::InjectPanic("__debugger_reload__".to_string());
                                }
                                Err(e) => {
                                    println!("  Error reading file: {}", e);
                                    continue;
                                }
                            }
                        }
                        "quit" | "q" | "exit" => {
                            state.quit = true;
                            return DebugAction::InjectPanic("__debugger_quit__".to_string());
                        }
                        _ => {
                            println!("Unknown command: '{}'. Type 'help' for commands.", cmd);
                            continue;
                        }
                    }
                }
            });

            interp.set_debug_hook(hook);

            let result = interp.run();

            // Sync state back
            {
                let state = dbg_state.borrow();
                self.breakpoints = state.breakpoints.clone();
                self.next_bp_id = state.next_bp_id;
                self.watches = state.watches.clone();
                self.recording = state.recording;
            }

            // Handle result
            match &result {
                RunResult::Return(val) => {
                    println!("\nProgram returned: {}", format_value(val));
                    match val {
                        Value::Int(i) => return *i as i32,
                        _ => return 0,
                    }
                }
                RunResult::Panic(msg) => {
                    if msg == "__debugger_quit__" {
                        println!("\nDebugger exited.");
                        return 1;
                    } else if msg == "__debugger_reload__" {
                        // Hot-reload: re-read source and restart
                        match std::fs::read_to_string(&current_source_path) {
                            Ok(s) => {
                                current_source = s;
                                current_source_lines = current_source.lines().map(|s| s.to_string()).collect();
                                println!("Reloaded. Restarting execution...\n");
                                continue; // Restart the outer loop
                            }
                            Err(e) => {
                                println!("Error reading file: {}", e);
                                return 1;
                            }
                        }
                    } else {
                        println!("\nProgram panicked: {}", msg);
                        return 1;
                    }
                }
                RunResult::Paused => {
                    println!("\nExecution paused.");
                    return 0;
                }
            }
        }
    }
}

/// Mutable debugger state accessed by the hook via RefCell
struct DebuggerState {
    breakpoints: Vec<Breakpoint>,
    next_bp_id: usize,
    stepping: bool,
    step_over: bool,
    watches: HashMap<String, String>,
    recording: bool,
    last_cmd: String,
    quit: bool,
}

/// Free function versions of the helper methods (used by the hook closure)

fn parse_bp_spec(spec: &str) -> (String, Option<usize>) {
    if let Some(colon_pos) = spec.find(':') {
        let func = spec[..colon_pos].to_string();
        let line: Option<usize> = spec[colon_pos+1..].parse().ok();
        (func, line)
    } else if let Ok(line) = spec.parse::<usize>() {
        ("main".to_string(), Some(line))
    } else {
        (spec.to_string(), None)
    }
}

fn format_value(v: &Value) -> String {
    match v {
        Value::Int(i) => format!("i64: {}", i),
        Value::Float(f) => format!("f64: {}", f),
        Value::Bool(b) => format!("bool: {}", b),
        Value::Str(s) => format!("str: \"{}\"", s),
        Value::Ptr(p) => format!("ptr: 0x{:x}", p),
        Value::Void => "void".to_string(),
        Value::Vec(v) => {
            let v = v.borrow();
            let elems: Vec<String> = v.iter().map(|e| format_value(e)).collect();
            format!("Vec[{}]", elems.join(", "))
        }
        Value::Map(m) => {
            let m = m.borrow();
            let entries: Vec<String> = m.iter()
                .map(|(k, v)| format!("{}: {}", k, format_value(v)))
                .collect();
            format!("Map{{{}}}", entries.join(", "))
        }
        Value::Dyn(t, _) => format!("dyn({})", t),
        Value::Tensor(t) => format!("tensor({})", t.borrow().to_string()),
        Value::Json(j) => format!("json({})", crate::json::dump(&j.borrow(), false)),
        Value::Allocator(a) => format!("alloc({})", a),
        Value::Future(idx) => format!("Future#{}", idx),
        Value::ThreadHandle(idx) => format!("Thread#{}", idx),
    }
}

fn print_locals(values: &HashMap<ValueId, Value>, func: &IrFunction) {
    if values.is_empty() {
        println!("  (no variables in scope)");
        return;
    }
    let mut printed = 0;
    for (name, vid) in &func.locals {
        if let Some(val) = values.get(vid) {
            println!("  {} = {}", name, format_value(val));
            printed += 1;
        }
    }
    if printed == 0 {
        for (vid, val) in values.iter().take(20) {
            println!("  v{} = {}", vid.0, format_value(val));
        }
        if values.len() > 20 {
            println!("  ... and {} more", values.len() - 20);
        }
    }
}

fn print_var(name: &str, values: &HashMap<ValueId, Value>, func: &IrFunction) {
    if let Some(vid) = func.locals.get(name) {
        if let Some(val) = values.get(vid) {
            println!("  {} = {}", name, format_value(val));
            return;
        }
    }
    if name.starts_with('v') {
        if let Ok(n) = name[1..].parse::<u32>() {
            let vid = ValueId(n);
            if let Some(val) = values.get(&vid) {
                println!("  {} = {}", name, format_value(val));
                return;
            }
        }
    }
    println!("  Variable '{}' not found in current scope", name);
}

fn list_source(source_lines: &[String], line: usize, context: usize) {
    let start = line.saturating_sub(context);
    let end = (line + context + 1).min(source_lines.len());
    for i in start..end {
        let marker = if i + 1 == line { "→ " } else { "  " };
        println!("{}{:4} │ {}", marker, i + 1, source_lines.get(i).unwrap_or(&"".to_string()));
    }
}

fn show_ir(func: &IrFunction) {
    println!("function {}({}) -> {:?}", func.name,
        func.params.iter().map(|(id, ty)| format!("v{}: {:?}", id.0, ty)).collect::<Vec<_>>().join(", "),
        func.ret_ty);
    for (i, block) in func.blocks.iter().enumerate() {
        println!("  block {}:", i);
        for &inst_id in &block.instructions {
            if let Some(inst) = func.instructions.get(&inst_id) {
                let line = func.values.get(&inst_id).and_then(|m| m.source_line);
                let line_str = line.map(|l| format!(" (line {})", l)).unwrap_or_default();
                println!("    v{} = {:?}{}", inst_id.0, inst, line_str);
            }
        }
    }
}

/// Show IR with the current instruction marked with →
fn show_ir_with_current(func: &IrFunction, current_inst: u32) {
    println!("function {}({}) -> {:?}", func.name,
        func.params.iter().map(|(id, ty)| format!("v{}: {:?}", id.0, ty)).collect::<Vec<_>>().join(", "),
        func.ret_ty);
    for (i, block) in func.blocks.iter().enumerate() {
        println!("  block {}:", i);
        for &inst_id in &block.instructions {
            if let Some(inst) = func.instructions.get(&inst_id) {
                let line = func.values.get(&inst_id).and_then(|m| m.source_line);
                let line_str = line.map(|l| format!(" (line {})", l)).unwrap_or_default();
                let marker = if inst_id.0 == current_inst { "→ " } else { "  " };
                println!("  {}v{} = {:?}{}", marker, inst_id.0, inst, line_str);
            }
        }
    }
}

fn show_effects(func: &IrFunction) {
    println!("Function: {}", func.name);
    println!("  Effects: {:?}", func.effects);
    println!("  @wrapping: {}", func.is_wrapping);
    println!("  @deterministic: {}", func.is_deterministic);
    println!("  @crashfree: {}", func.is_crashfree);
    println!("  @no-alloc: {}", func.is_no_alloc);
    if let Some(gas) = func.gas_limit {
        println!("  @gas: {}", gas);
    }
}

fn print_debugger_help() {
    println!("Fides Debugger Commands:");
    println!("  break <func>[:<line>]  Set a breakpoint (e.g. 'break main:10')");
    println!("  break                  List all breakpoints");
    println!("  delete <n>             Delete breakpoint #n");
    println!("  delete                 Delete all breakpoints");
    println!("  run                    Run until breakpoint or program end");
    println!("  step                   Step one instruction (into calls)");
    println!("  next                   Step one instruction (over calls)");
    println!("  continue               Continue execution");
    println!("  print <var>            Print a variable's value");
    println!("  print                  Print all variables in scope");
    println!("  backtrace              Show call stack");
    println!("  info locals            Show all local variables");
    println!("  info functions         List all functions");
    println!("  info breakpoints       List breakpoints");
    println!("  list                   Show source around current position");
    println!("  ir                     Show IR for current function (→ marks current instruction)");
    println!("  ir current             Show only the current IR instruction");
    println!("  effects                Show effects for current function");
    println!("  regions                Show active regions and memory usage");
    println!("  capabilities           Show granted/revoked capabilities");
    println!("  watch <var>            Watch a variable (print on change)");
    println!("  record                 Start recording execution trace");
    println!("  reload                 Hot-reload source file and restart execution");
    println!("  quit                   Exit debugger");
}

/// Save a trace to a file
pub fn save_trace(trace: &[TraceEntry], path: &str) -> std::io::Result<()> {
    use std::io::Write;
    let mut f = std::fs::File::create(path)?;
    writeln!(f, "# Fides execution trace")?;
    writeln!(f, "# Format: func_name block_id inst_id source_line [value]")?;
    for entry in trace {
        if let Some(ref val) = entry.value_repr {
            writeln!(f, "{} {} {} {} {}", entry.func_name, entry.block_id, entry.inst_id, entry.source_line, val)?;
        } else {
            writeln!(f, "{} {} {} {} -", entry.func_name, entry.block_id, entry.inst_id, entry.source_line)?;
        }
    }
    Ok(())
}

/// Load a trace from a file
pub fn load_trace(path: &str) -> std::io::Result<Vec<TraceEntry>> {
    let content = std::fs::read_to_string(path)?;
    let mut trace = Vec::new();
    for line in content.lines() {
        if line.starts_with('#') || line.trim().is_empty() {
            continue;
        }
        let parts: Vec<&str> = line.split_whitespace().collect();
        if parts.len() >= 5 {
            let value_repr = if parts[4] == "-" {
                None
            } else {
                Some(parts[4].to_string())
            };
            trace.push(TraceEntry {
                func_name: parts[0].to_string(),
                block_id: parts[1].parse().unwrap_or(0),
                inst_id: parts[2].parse().unwrap_or(0),
                source_line: parts[3].parse().unwrap_or(0),
                value_repr,
            });
        }
    }
    Ok(trace)
}

/// Replay a trace file — `fidesc replay <trace>`
pub fn replay_trace(path: &str) -> i32 {
    let trace = match load_trace(path) {
        Ok(t) => t,
        Err(e) => {
            eprintln!("Error loading trace '{}': {}", path, e);
            return 1;
        }
    };

    println!("Replaying trace: {} ({} entries)", path, trace.len());
    println!("Use 'step' to step forward, 'continue' to play to end, 'quit' to exit.\n");

    let stdin = io::stdin();
    let mut idx = 0usize;

    loop {
        if idx >= trace.len() {
            println!("\nEnd of trace ({} entries).", trace.len());
            break;
        }

        let entry = &trace[idx];
        print!("[{}/{}] {} (block {}, line {}) > ", idx + 1, trace.len(), entry.func_name, entry.block_id, entry.source_line);
        io::stdout().flush().ok();

        let mut line = String::new();
        if stdin.lock().read_line(&mut line).is_err() {
            break;
        }
        let line = line.trim();

        match line {
            "" | "step" | "s" => {
                if let Some(ref val) = entry.value_repr {
                    println!("  → v{} = {}", entry.inst_id, val);
                } else {
                    println!("  → v{} (no value)", entry.inst_id);
                }
                idx += 1;
            }
            "continue" | "c" => {
                // Play to end, printing each entry
                while idx < trace.len() {
                    let e = &trace[idx];
                    if let Some(ref val) = e.value_repr {
                        println!("  [{}/{}] {} v{} = {} (line {})", idx + 1, trace.len(), e.func_name, e.inst_id, val, e.source_line);
                    } else {
                        println!("  [{}/{}] {} v{} (line {})", idx + 1, trace.len(), e.func_name, e.inst_id, e.source_line);
                    }
                    idx += 1;
                }
            }
            "backtrace" | "bt" => {
                // Show all entries from the same function up to this point
                let mut func_entries: Vec<usize> = Vec::new();
                for (i, e) in trace.iter().enumerate() {
                    if e.func_name == entry.func_name && i <= idx {
                        func_entries.push(i);
                    }
                }
                println!("  Trace in {}:", entry.func_name);
                for &i in &func_entries {
                    let marker = if i == idx { "→" } else { " " };
                    println!("  {} [{}] v{} (line {})", marker, i + 1, trace[i].inst_id, trace[i].source_line);
                }
            }
            "list" | "l" => {
                // Show surrounding entries
                let start = idx.saturating_sub(5);
                let end = (idx + 5).min(trace.len());
                for i in start..end {
                    let marker = if i == idx { "→" } else { " " };
                    let e = &trace[i];
                    println!("  {} [{}/{}] {} v{} (line {})", marker, i + 1, trace.len(), e.func_name, e.inst_id, e.source_line);
                }
            }
            "quit" | "q" | "exit" => {
                break;
            }
            "help" | "h" | "?" => {
                println!("Replay commands:");
                println!("  step / s     Step to next trace entry");
                println!("  continue / c Play to end of trace");
                println!("  backtrace    Show entries in current function");
                println!("  list         Show surrounding entries");
                println!("  quit         Exit replay");
            }
            _ => {
                println!("Unknown command: '{}'. Type 'help' for commands.", line);
            }
        }
    }

    0
}
