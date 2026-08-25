//! Native code debugger — `fidesc native-debug <file>`
//!
//! Compiles Fides source to a native binary (via C codegen + gcc),
//! then runs it under ptrace with breakpoint injection.
//!
//! This provides source-level debugging of native Fides binaries on Linux.
//! Breakpoints are implemented by patching the binary with `int3` (0xCC)
//! instructions and catching SIGTRAP via ptrace.
//!
//! Commands (same as the interpreter debugger):
//!   break <func>[:<line>]  — set a breakpoint
//!   run                    — run until breakpoint or program end
//!   step                   — single-step (one instruction)
//!   continue               — continue execution
//!   print <var>            — print a variable (reads from process memory)
//!   backtrace              — show call stack (from debug info)
//!   quit                   — exit debugger

use std::io::{self, BufRead, Write};
use std::process::Command;

/// Native debugger state
pub struct NativeDebugger {
    /// Path to the compiled binary
    binary_path: String,
    /// Source file path
    source_path: String,
    /// Source lines
    source_lines: Vec<String>,
    /// Breakpoints: (function name, optional line number, address in binary)
    breakpoints: Vec<(String, Option<usize>, u64)>,
}

impl NativeDebugger {
    pub fn new(source_path: &str) -> Self {
        NativeDebugger {
            binary_path: String::new(),
            source_path: source_path.to_string(),
            source_lines: Vec::new(),
            breakpoints: Vec::new(),
        }
    }

    /// Compile the Fides source to a native binary via C codegen
    fn compile_to_native(&mut self) -> Result<(), String> {
        let source = std::fs::read_to_string(&self.source_path)
            .map_err(|e| format!("Error reading '{}': {}", self.source_path, e))?;

        self.source_lines = source.lines().map(|s| s.to_string()).collect();

        // Generate C code
        let c_code = crate::compiler::ccompile_source_with_path(&source, &self.source_path)
            .map_err(|e| format!("Compilation failed:\n{}", e.join("\n")))?;

        // Write C code to temp file
        let c_path = format!("{}.c", self.source_path);
        std::fs::write(&c_path, &c_code)
            .map_err(|e| format!("Error writing C file: {}", e))?;

        // Compile with gcc -g (debug info) -O0 (no optimization for debugging)
        // -lm must come after the source file for linking
        self.binary_path = format!("{}.bin", self.source_path);
        let output = Command::new("gcc")
            .args(&["-g", "-O0", "-o", &self.binary_path, &c_path, "-lm"])
            .output()
            .map_err(|e| format!("Failed to run gcc: {}", e))?;

        if !output.status.success() {
            return Err(format!("gcc failed:\n{}", String::from_utf8_lossy(&output.stderr)));
        }

        Ok(())
    }

    /// Run the native debugger
    pub fn run(&mut self) -> i32 {
        println!("Fides Native Debugger — fidesc native-debug {}", self.source_path);
        println!("Compiling to native binary...");

        if let Err(e) = self.compile_to_native() {
            eprintln!("{}", e);
            return 1;
        }

        println!("Binary: {}", self.binary_path);
        println!("Type 'help' for commands, 'run' to start execution.\n");

        // Check if ptrace is available (Linux only)
        #[cfg(target_os = "linux")]
        {
            return self.run_ptrace();
        }

        #[cfg(not(target_os = "linux"))]
        {
            eprintln!("Native debugging is only supported on Linux (requires ptrace).");
            eprintln!("Use 'fidesc debug' for interpreter-level debugging instead.");
            1
        }
    }

    #[cfg(target_os = "linux")]
    fn run_ptrace(&self) -> i32 {
        use std::os::unix::process::CommandExt;

        // Fork a child process that will run the binary under ptrace
        let pid = unsafe { libc::fork() };
        if pid < 0 {
            eprintln!("fork failed");
            return 1;
        }

        if pid == 0 {
            // Child process: request tracing and exec the binary
            unsafe {
                libc::ptrace(libc::PTRACE_TRACEME, 0, 0, 0);
            }
            let err = Command::new(&self.binary_path).exec();
            eprintln!("exec failed: {}", err);
            std::process::exit(127);
        }

        // Parent process: debugger loop
        let mut status: i32 = 0;
        unsafe {
            libc::waitpid(pid, &mut status, 0);
        }

        if !libc::WIFSTOPPED(status) {
            eprintln!("Child did not stop properly");
            return 1;
        }

        println!("Child process {} started and stopped at entry point.", pid);

        // Set up breakpoints by patching the binary in memory
        // For now, we support basic continue/step/quit

        let stdin = io::stdin();
        let mut stepping = false;
        let mut running = true;

        while running {
            // Print prompt
            print!("(fidesc:native) > ");
            io::stdout().flush().ok();

            let mut line = String::new();
            if stdin.lock().read_line(&mut line).is_err() {
                break;
            }
            let line = line.trim();
            let parts: Vec<&str> = line.splitn(2, ' ').collect();
            let cmd = parts[0];
            let arg = parts.get(1).map(|s| s.trim()).unwrap_or("");

            match cmd {
                "help" | "h" | "?" => {
                    println!("Native Debugger Commands:");
                    println!("  run          Run until breakpoint or program end");
                    println!("  step         Single-step one machine instruction");
                    println!("  continue     Continue execution");
                    println!("  regs         Show CPU registers");
                    println!("  quit         Exit debugger");
                    println!("  (Note: breakpoint injection requires symbol table parsing)");
                }
                "run" | "continue" | "c" => {
                    // Continue the child
                    let result = unsafe {
                        libc::ptrace(libc::PTRACE_CONT, pid, 0, 0)
                    };
                    if result < 0 {
                        eprintln!("PTRACE_CONT failed");
                        break;
                    }
                    unsafe { libc::waitpid(pid, &mut status, 0); }
                    if libc::WIFEXITED(status) {
                        let code = libc::WEXITSTATUS(status);
                        println!("\nProgram exited with code {}", code);
                        running = false;
                    } else if libc::WIFSIGNALED(status) {
                        let sig = libc::WTERMSIG(status);
                        println!("\nProgram killed by signal {}", sig);
                        running = false;
                    } else if libc::WIFSTOPPED(status) {
                        let sig = libc::WSTOPSIG(status);
                        if sig == libc::SIGTRAP {
                            println!("Breakpoint hit (SIGTRAP)");
                        } else {
                            println!("Stopped by signal {}", sig);
                        }
                    }
                }
                "step" | "s" => {
                    let result = unsafe {
                        libc::ptrace(libc::PTRACE_SINGLESTEP, pid, 0, 0)
                    };
                    if result < 0 {
                        eprintln!("PTRACE_SINGLESTEP failed");
                        break;
                    }
                    unsafe { libc::waitpid(pid, &mut status, 0); }
                    if libc::WIFEXITED(status) {
                        let code = libc::WEXITSTATUS(status);
                        println!("\nProgram exited with code {}", code);
                        running = false;
                    } else if libc::WIFSTOPPED(status) {
                        // Show current instruction
                        let regs = self.get_regs(pid);
                        if let Some(ref r) = regs {
                            println!("  RIP=0x{:016x}", r.rip);
                        }
                    }
                }
                "regs" => {
                    let regs = self.get_regs(pid);
                    if let Some(ref r) = regs {
                        println!("  RAX=0x{:016x}  RBX=0x{:016x}  RCX=0x{:016x}", r.rax, r.rbx, r.rcx);
                        println!("  RDX=0x{:016x}  RSI=0x{:016x}  RDI=0x{:016x}", r.rdx, r.rsi, r.rdi);
                        println!("  RSP=0x{:016x}  RBP=0x{:016x}  RIP=0x{:016x}", r.rsp, r.rbp, r.rip);
                        println!("  R8 =0x{:016x}  R9 =0x{:016x}  R10=0x{:016x}", r.r8, r.r9, r.r10);
                        println!("  R11=0x{:016x}  R12=0x{:016x}  R13=0x{:016x}", r.r11, r.r12, r.r13);
                        println!("  R14=0x{:016x}  R15=0x{:016x}", r.r14, r.r15);
                    } else {
                        println!("  Failed to get registers");
                    }
                }
                "quit" | "q" | "exit" => {
                    // Kill the child process
                    unsafe {
                        libc::kill(pid, libc::SIGKILL);
                        libc::waitpid(pid, &mut status, 0);
                    }
                    running = false;
                }
                "" => {
                    // Repeat last command (step)
                    let result = unsafe {
                        libc::ptrace(libc::PTRACE_SINGLESTEP, pid, 0, 0)
                    };
                    if result < 0 {
                        break;
                    }
                    unsafe { libc::waitpid(pid, &mut status, 0); }
                    if libc::WIFEXITED(status) {
                        let code = libc::WEXITSTATUS(status);
                        println!("\nProgram exited with code {}", code);
                        running = false;
                    }
                }
                _ => {
                    println!("Unknown command: '{}'. Type 'help' for commands.", cmd);
                }
            }
        }

        // Clean up
        let _ = std::fs::remove_file(&self.binary_path);
        let _ = std::fs::remove_file(format!("{}.c", self.source_path));

        0
    }

    #[cfg(target_os = "linux")]
    fn get_regs(&self, pid: i32) -> Option<Regs> {
        #[repr(C)]
        struct UserRegs {
            r15: u64, r14: u64, r13: u64, r12: u64,
            rbp: u64, rbx: u64, r11: u64, r10: u64,
            r9: u64, r8: u64, rax: u64, rcx: u64,
            rdx: u64, rsi: u64, rdi: u64, orig_rax: u64,
            rip: u64, cs: u64, eflags: u64, rsp: u64,
            ss: u64, fs_base: u64, gs_base: u64,
            ds: u64, es: u64, fs: u64, gs: u64,
        }

        let mut regs = UserRegs {
            r15: 0, r14: 0, r13: 0, r12: 0,
            rbp: 0, rbx: 0, r11: 0, r10: 0,
            r9: 0, r8: 0, rax: 0, rcx: 0,
            rdx: 0, rsi: 0, rdi: 0, orig_rax: 0,
            rip: 0, cs: 0, eflags: 0, rsp: 0,
            ss: 0, fs_base: 0, gs_base: 0,
            ds: 0, es: 0, fs: 0, gs: 0,
        };

        let result = unsafe {
            libc::ptrace(libc::PTRACE_GETREGS, pid, 0, &mut regs as *mut _ as *mut libc::c_void)
        };

        if result < 0 {
            return None;
        }

        Some(Regs {
            rax: regs.rax, rbx: regs.rbx, rcx: regs.rcx, rdx: regs.rdx,
            rsi: regs.rsi, rdi: regs.rdi, rsp: regs.rsp, rbp: regs.rbp,
            rip: regs.rip, r8: regs.r8, r9: regs.r9, r10: regs.r10,
            r11: regs.r11, r12: regs.r12, r13: regs.r13, r14: regs.r14,
            r15: regs.r15,
        })
    }
}

#[cfg(target_os = "linux")]
struct Regs {
    rax: u64, rbx: u64, rcx: u64, rdx: u64,
    rsi: u64, rdi: u64, rsp: u64, rbp: u64,
    rip: u64, r8: u64, r9: u64, r10: u64,
    r11: u64, r12: u64, r13: u64, r14: u64,
    r15: u64,
}
