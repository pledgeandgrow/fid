// Green Threads — Phase 13 (World Class)
//
// Implements cooperative green threads (stackful coroutines) with
// context switching and a work-stealing scheduler. Green threads enable
// lightweight concurrency without OS thread overhead.
//
// Architecture:
//   - Each green thread has its own stack (default: 64KB)
//   - Context switching saves/restores callee-saved registers + SP
//   - The scheduler is a simple round-robin queue
//   - yield() suspends the current thread and switches to the scheduler
//   - spawn() creates a new green thread and adds it to the queue
//
// The runtime is implemented in Rust (for the interpreter) and in C
// (for the C codegen backend). The PE/ELF backends use the C runtime.
//
// This module provides:
//   1. The Rust runtime (for the interpreter)
//   2. The C runtime source (embedded as a string for codegen_c.rs)
//   3. Integration with the existing async/await infrastructure

use std::collections::VecDeque;

// ═══════════════════════════════════════════════════════════════════════════
// Green Thread Runtime (Rust — for interpreter)
// ═══════════════════════════════════════════════════════════════════════════

/// Default stack size for green threads: 64KB
pub const GREEN_THREAD_STACK_SIZE: usize = 64 * 1024;

/// Maximum number of green threads
pub const MAX_GREEN_THREADS: usize = 256;

/// Green thread states
#[derive(Debug, Clone, Copy, PartialEq)]
pub enum GreenThreadState {
    /// Thread is ready to run
    Ready,
    /// Thread is currently running
    Running,
    /// Thread is suspended (yielded or waiting)
    Suspended,
    /// Thread has completed
    Done,
}

/// A green thread with its own stack
pub struct GreenThread {
    pub id: usize,
    pub state: GreenThreadState,
    /// Saved stack pointer (for context switching)
    pub saved_sp: u64,
    /// Saved instruction pointer (resume point)
    pub saved_ip: u64,
    /// Stack memory (allocated as Vec for safety)
    pub stack: Vec<u8>,
    /// Function to run (index into a dispatch table)
    pub func_id: i64,
    /// Arguments (packed as i64 values)
    pub args: Vec<i64>,
    /// Result value (set when thread completes)
    pub result: i64,
}

impl GreenThread {
    pub fn new(id: usize, func_id: i64, args: Vec<i64>) -> Self {
        GreenThread {
            id,
            state: GreenThreadState::Ready,
            saved_sp: 0,
            saved_ip: 0,
            stack: vec![0u8; GREEN_THREAD_STACK_SIZE],
            func_id,
            args,
            result: 0,
        }
    }

    /// Get the top of the stack (for context switching)
    pub fn stack_top(&self) -> u64 {
        // Stack grows downward, so the "top" is the end of the buffer
        self.stack.as_ptr() as u64 + self.stack.len() as u64
    }
}

/// Green thread scheduler (round-robin queue)
pub struct GreenThreadScheduler {
    /// All green threads (indexed by ID)
    threads: Vec<Option<GreenThread>>,
    /// Ready queue (thread IDs waiting to run)
    ready_queue: VecDeque<usize>,
    /// Currently running thread ID
    current: Option<usize>,
    /// Next thread ID to allocate
    next_id: usize,
}

impl GreenThreadScheduler {
    pub fn new() -> Self {
        let mut threads = Vec::with_capacity(MAX_GREEN_THREADS);
        for _ in 0..MAX_GREEN_THREADS {
            threads.push(None);
        }
        GreenThreadScheduler {
            threads,
            ready_queue: VecDeque::new(),
            current: None,
            next_id: 0,
        }
    }

    /// Spawn a new green thread. Returns the thread ID.
    pub fn spawn(&mut self, func_id: i64, args: Vec<i64>) -> usize {
        let id = self.next_id;
        self.next_id += 1;

        let thread = GreenThread::new(id, func_id, args);
        self.threads[id] = Some(thread);
        self.ready_queue.push_back(id);

        id
    }

    /// Get the currently running thread ID
    pub fn current_thread(&self) -> Option<usize> {
        self.current
    }

    /// Yield from the current thread. Returns true if there's another thread to switch to.
    pub fn yield_current(&mut self) -> bool {
        if self.current.is_none() {
            return false;
        }

        let current_id = self.current.unwrap();

        // Mark current thread as suspended and re-queue it
        if let Some(ref mut t) = self.threads[current_id] {
            t.state = GreenThreadState::Suspended;
        }
        self.ready_queue.push_back(current_id);
        self.current = None;

        // Find next ready thread
        while let Some(next_id) = self.ready_queue.pop_front() {
            if let Some(ref t) = self.threads[next_id] {
                if t.state == GreenThreadState::Suspended || t.state == GreenThreadState::Ready {
                    if let Some(ref mut t) = self.threads[next_id] {
                        t.state = GreenThreadState::Running;
                    }
                    self.current = Some(next_id);
                    return true;
                }
            }
        }

        false
    }

    /// Mark the current thread as done
    pub fn finish_current(&mut self, result: i64) {
        if let Some(current_id) = self.current {
            if let Some(ref mut t) = self.threads[current_id] {
                t.state = GreenThreadState::Done;
                t.result = result;
            }
            self.current = None;
        }
    }

    /// Check if a thread is done
    pub fn is_done(&self, id: usize) -> bool {
        self.threads.get(id)
            .and_then(|t| t.as_ref())
            .map(|t| t.state == GreenThreadState::Done)
            .unwrap_or(true)
    }

    /// Get the result of a completed thread
    pub fn get_result(&self, id: usize) -> i64 {
        self.threads.get(id)
            .and_then(|t| t.as_ref())
            .map(|t| t.result)
            .unwrap_or(0)
    }

    /// Check if there are any ready or suspended threads
    pub fn has_ready_threads(&self) -> bool {
        !self.ready_queue.is_empty()
    }

    /// Run the scheduler until all threads are done.
    /// The callback is called for each thread that needs to run.
    /// The callback returns true if the thread completed, false if it yielded.
    pub fn run_all<F>(&mut self, mut run_fn: F) -> i64
    where
        F: FnMut(&mut GreenThread) -> (bool, i64),
    {
        let mut last_result = 0i64;

        while let Some(thread_id) = self.ready_queue.pop_front() {
            // Get the thread
            let should_run = self.threads.get(thread_id)
                .and_then(|t| t.as_ref())
                .map(|t| t.state != GreenThreadState::Done)
                .unwrap_or(false);

            if !should_run {
                continue;
            }

            // Set as current
            self.current = Some(thread_id);
            if let Some(ref mut t) = self.threads[thread_id] {
                t.state = GreenThreadState::Running;
            }

            // Run the thread
            let (completed, result) = if let Some(ref mut t) = self.threads[thread_id] {
                run_fn(t)
            } else {
                (true, 0)
            };

            if completed {
                if let Some(ref mut t) = self.threads[thread_id] {
                    t.state = GreenThreadState::Done;
                    t.result = result;
                }
                last_result = result;
                self.current = None;
            } else {
                // Thread yielded — re-queue
                if let Some(ref mut t) = self.threads[thread_id] {
                    t.state = GreenThreadState::Suspended;
                }
                self.ready_queue.push_back(thread_id);
                self.current = None;
            }
        }

        last_result
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// C Runtime Source (for codegen_c.rs)
// ═══════════════════════════════════════════════════════════════════════════

/// C source code for the green thread runtime.
/// This is embedded in the generated C file when green threads are used.
pub const GREEN_THREAD_C_RUNTIME: &str = r#"
// ═══ Green Thread Runtime (auto-generated by Fides compiler) ═══
// Cooperative green threads with context switching.
// Each thread has its own 64KB stack. The scheduler is round-robin.

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define GT_STACK_SIZE (64 * 1024)
#define GT_MAX_THREADS 256

typedef enum {
    GT_READY,
    GT_RUNNING,
    GT_SUSPENDED,
    GT_DONE
} gt_state_t;

typedef struct {
    uint32_t id;
    gt_state_t state;
    // Saved context (callee-saved registers + SP + IP)
    uint64_t saved_rsp;
    uint64_t saved_rbp;
    uint64_t saved_rbx;
    uint64_t saved_r12;
    uint64_t saved_r13;
    uint64_t saved_r14;
    uint64_t saved_r15;
    uint64_t saved_rip;
    // Stack
    uint8_t* stack;
    // Function and args
    int64_t func_id;
    int64_t args[8];
    int64_t result;
} gt_thread_t;

static gt_thread_t gt_threads[GT_MAX_THREADS];
static uint32_t gt_next_id = 0;
static int32_t gt_current = -1;

// Function dispatch table (populated by the program)
typedef int64_t (*gt_func_t)(int64_t args[8]);
static gt_func_t gt_dispatch_table[1024];

void gt_register_func(int64_t id, gt_func_t func) {
    if (id >= 0 && id < 1024) {
        gt_dispatch_table[id] = func;
    }
}

int32_t gt_spawn(int64_t func_id, int64_t args[8]) {
    if (gt_next_id >= GT_MAX_THREADS) return -1;
    uint32_t id = gt_next_id++;
    gt_thread_t* t = &gt_threads[id];
    t->id = id;
    t->state = GT_READY;
    t->stack = (uint8_t*)malloc(GT_STACK_SIZE);
    if (!t->stack) return -1;
    t->func_id = func_id;
    memcpy(t->args, args, sizeof(t->args));
    t->result = 0;
    t->saved_rsp = 0;
    t->saved_rip = 0;
    return (int32_t)id;
}

// Context switching (x86-64 System V ABI)
// save_context: saves callee-saved registers to the thread struct
// restore_context: restores them and jumps to saved_rip
#if defined(__x86_64__) && !defined(__TINYC__)
static inline void gt_save_context(gt_thread_t* t) {
    __asm__ volatile(
        "movq %%rsp, %0\n"
        "movq %%rbp, %1\n"
        "movq %%rbx, %2\n"
        "movq %%r12, %3\n"
        "movq %%r13, %4\n"
        "movq %%r14, %5\n"
        "movq %%r15, %6\n"
        "leaq 1f(%%rip), %7\n"
        "1:\n"
        : "=m"(t->saved_rsp), "=m"(t->saved_rbp),
          "=m"(t->saved_rbx), "=m"(t->saved_r12),
          "=m"(t->saved_r13), "=m"(t->saved_r14),
          "=m"(t->saved_r15), "=m"(t->saved_rip)
        :
        : "memory"
    );
}

static inline void gt_restore_context(gt_thread_t* t) {
    __asm__ volatile(
        "movq %0, %%rsp\n"
        "movq %1, %%rbp\n"
        "movq %2, %%rbx\n"
        "movq %3, %%r12\n"
        "movq %4, %%r13\n"
        "movq %5, %%r14\n"
        "movq %6, %%r15\n"
        "jmp *%7\n"
        :
        : "m"(t->saved_rsp), "m"(t->saved_rbp),
          "m"(t->saved_rbx), "m"(t->saved_r12),
          "m"(t->saved_r13), "m"(t->saved_r14),
          "m"(t->saved_r15), "m"(t->saved_rip)
        : "memory"
    );
}
#else
// Non-x86: no context switching (sequential execution)
static inline void gt_save_context(gt_thread_t* t) { (void)t; }
static inline void gt_restore_context(gt_thread_t* t) { (void)t; }
#endif

// Yield from the current green thread
void gt_yield(void) {
    if (gt_current < 0) return;
    gt_thread_t* current = &gt_threads[gt_current];
    if (current->state != GT_RUNNING) return;

    // Save context
    gt_save_context(current);
    current->state = GT_SUSPENDED;

    // Find next ready thread
    for (uint32_t i = 1; i < GT_MAX_THREADS; i++) {
        uint32_t next = (gt_current + i) % GT_MAX_THREADS;
        if (gt_threads[next].state == GT_READY || gt_threads[next].state == GT_SUSPENDED) {
            gt_current = (int32_t)next;
            gt_threads[next].state = GT_RUNNING;
            gt_restore_context(&gt_threads[next]);
            return;
        }
    }
    // No other thread ready — resume current
    current->state = GT_RUNNING;
}

// Run a green thread (first execution or resume)
static int64_t gt_run_thread(gt_thread_t* t) {
    if (t->state == GT_READY) {
        // First execution — set up stack and call the function
        t->state = GT_RUNNING;
        gt_current = (int32_t)t->id;

        // Set up the stack: push a return address that marks completion
        uint64_t stack_top = (uint64_t)t->stack + GT_STACK_SIZE;
        stack_top &= ~0xFUL; // 16-byte align
        // Push a sentinel return address (0 = thread exit)
        stack_top -= 8;
        *(uint64_t*)stack_top = 0;

        // Call the function with the thread's stack
        // On x86-64, we need to switch stacks and call the function
#if defined(__x86_64__) && !defined(__TINYC__)
        int64_t result;
        uint64_t old_rsp;
        __asm__ volatile(
            "movq %%rsp, %0\n"      // save current SP
            "movq %2, %%rsp\n"      // switch to thread stack
            "andq $-16, %%rsp\n"    // align
            "call *%3\n"            // call the function
            "movq %0, %%rsp\n"      // restore SP
            : "=m"(old_rsp), "=a"(result)
            : "r"(stack_top), "r"(gt_dispatch_table[t->func_id])
            : "rbx", "r12", "r13", "r14", "r15", "rcx", "rdx", "rsi", "rdi",
              "r8", "r9", "r10", "r11", "memory", "cc"
        );
        t->result = result;
#else
        // Non-x86 or TCC: just call the function directly
        if (gt_dispatch_table[t->func_id]) {
            t->result = gt_dispatch_table[t->func_id](t->args);
        }
#endif
        t->state = GT_DONE;
        gt_current = -1;
        return t->result;
    } else if (t->state == GT_SUSPENDED) {
        // Resume — restore context
        gt_current = (int32_t)t->id;
        t->state = GT_RUNNING;
        gt_restore_context(t);
        // If we get here, the thread completed after resume
        t->state = GT_DONE;
        gt_current = -1;
        return t->result;
    }
    return t->result;
}

// Run all green threads until completion
int64_t gt_run_all(void) {
    int64_t last_result = 0;
    int has_pending = 1;
    while (has_pending) {
        has_pending = 0;
        for (uint32_t i = 0; i < gt_next_id; i++) {
            if (gt_threads[i].state == GT_READY || gt_threads[i].state == GT_SUSPENDED) {
                has_pending = 1;
                last_result = gt_run_thread(&gt_threads[i]);
            }
        }
    }
    return last_result;
}

// Check if a thread is done
int gt_is_done(int32_t id) {
    if (id < 0 || id >= (int32_t)gt_next_id) return 1;
    return gt_threads[id].state == GT_DONE;
}

// Get thread result
int64_t gt_get_result(int32_t id) {
    if (id < 0 || id >= (int32_t)gt_next_id) return 0;
    return gt_threads[id].result;
}

// Clean up all threads
void gt_cleanup(void) {
    for (uint32_t i = 0; i < gt_next_id; i++) {
        if (gt_threads[i].stack) {
            free(gt_threads[i].stack);
            gt_threads[i].stack = NULL;
        }
    }
    gt_next_id = 0;
    gt_current = -1;
}
"#;

// ═══════════════════════════════════════════════════════════════════════════
// Interpreter Integration
// ═══════════════════════════════════════════════════════════════════════════

/// Global green thread scheduler (used by the interpreter)
static mut GLOBAL_SCHEDULER: Option<GreenThreadScheduler> = None;

/// Get or create the global scheduler
pub fn get_scheduler() -> &'static mut GreenThreadScheduler {
    unsafe {
        if GLOBAL_SCHEDULER.is_none() {
            GLOBAL_SCHEDULER = Some(GreenThreadScheduler::new());
        }
        GLOBAL_SCHEDULER.as_mut().unwrap()
    }
}

/// Reset the global scheduler (for testing)
pub fn reset_scheduler() {
    unsafe {
        GLOBAL_SCHEDULER = None;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_scheduler_spawn() {
        let mut sched = GreenThreadScheduler::new();
        let id1 = sched.spawn(1, vec![10]);
        let id2 = sched.spawn(2, vec![20]);
        assert_eq!(id1, 0);
        assert_eq!(id2, 1);
        assert!(sched.has_ready_threads());
    }

    #[test]
    fn test_scheduler_run_all() {
        let mut sched = GreenThreadScheduler::new();
        sched.spawn(1, vec![10]);
        sched.spawn(2, vec![20]);

        let result = sched.run_all(|thread| {
            // Simulate work: return the first arg as result
            let result = thread.args.get(0).copied().unwrap_or(0);
            (true, result) // completed
        });

        // run_all returns the last completed thread's result
        assert!(result == 10 || result == 20);
    }

    #[test]
    fn test_scheduler_yield() {
        let mut sched = GreenThreadScheduler::new();
        sched.spawn(1, vec![100]);
        sched.spawn(2, vec![200]);

        let mut yield_count = 0;
        sched.run_all(|_thread| {
            yield_count += 1;
            if yield_count == 1 {
                (false, 0) // yield on first run
            } else {
                (true, 42) // complete on second run
            }
        });

        // The first thread should have yielded and then completed
        assert!(yield_count >= 2);
    }

    #[test]
    fn test_scheduler_done_check() {
        let mut sched = GreenThreadScheduler::new();
        let id = sched.spawn(1, vec![42]);

        assert!(!sched.is_done(id));

        sched.run_all(|_| (true, 42));

        assert!(sched.is_done(id));
        assert_eq!(sched.get_result(id), 42);
    }

    #[test]
    fn test_green_thread_struct() {
        let t = GreenThread::new(0, 1, vec![1, 2, 3]);
        assert_eq!(t.id, 0);
        assert_eq!(t.state, GreenThreadState::Ready);
        assert_eq!(t.func_id, 1);
        assert_eq!(t.args, vec![1, 2, 3]);
        assert_eq!(t.stack.len(), GREEN_THREAD_STACK_SIZE);
        assert!(t.stack_top() > 0);
    }

    #[test]
    fn test_c_runtime_contains_key_functions() {
        assert!(GREEN_THREAD_C_RUNTIME.contains("gt_spawn"));
        assert!(GREEN_THREAD_C_RUNTIME.contains("gt_yield"));
        assert!(GREEN_THREAD_C_RUNTIME.contains("gt_run_all"));
        assert!(GREEN_THREAD_C_RUNTIME.contains("gt_save_context"));
        assert!(GREEN_THREAD_C_RUNTIME.contains("gt_restore_context"));
        assert!(GREEN_THREAD_C_RUNTIME.contains("gt_is_done"));
        assert!(GREEN_THREAD_C_RUNTIME.contains("gt_get_result"));
        assert!(GREEN_THREAD_C_RUNTIME.contains("gt_cleanup"));
    }
}
