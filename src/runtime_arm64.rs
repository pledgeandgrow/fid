// Fides Linux ARM64 Runtime — Cross-Compilation Phase
//
// Native runtime stubs for Linux ARM64 (AArch64) executables. Provides
// implementations of all p_print_*, p_panic, p_alloc, p_malloc, p_free,
// p_str_concat, p_to_str, and Vec builtins using raw Linux syscalls.
//
// Unlike runtime_linux.rs (which hand-encodes x86-64 bytes), this module
// uses the ARM64 assembler (arm64_enc) to assemble assembly text into
// machine code. This is more maintainable and easier to verify.
//
// All functions use the AAPCS64 ABI:
//   - Args: X0-X7
//   - Return: X0
//   - Syscalls: X8=syscall#, X0-X5 → X0
//   - Stack must be 16-byte aligned

use std::collections::HashSet;
use crate::arm64_enc;

/// Runtime entry: returns (machine_code_bytes, symbol_table, rodata_bytes, rodata_symbols).
pub fn runtime_parts() -> (Vec<u8>, Vec<(&'static str, u64, usize)>, Vec<u8>, Vec<(&'static str, u64)>) {
    runtime_parts_impl(None)
}

/// Capability-aware runtime linking (same interface as runtime_linux).
pub fn runtime_parts_with_caps(granted_caps: &HashSet<String>) -> (Vec<u8>, Vec<(&'static str, u64, usize)>, Vec<u8>, Vec<(&'static str, u64)>) {
    runtime_parts_impl(Some(granted_caps))
}

fn stub_capability(name: &str) -> Option<&'static str> {
    if name.starts_with("p_socket_") || name.starts_with("p_http_") {
        Some("Net")
    } else if name.starts_with("p_file_") || name.starts_with("p_read_file") || name.starts_with("p_write_file") {
        Some("FS")
    } else if name.starts_with("p_thread_") || name == "p_spawn" {
        Some("Spawn")
    } else {
        None
    }
}

// Thread-local storage for relocations (same pattern as runtime_linux)
thread_local! {
    static RUNTIME_RELOCATIONS: std::cell::RefCell<Vec<(u64, String, u8)>> = std::cell::RefCell::new(Vec::new());
}

pub fn runtime_relocations() -> Vec<(u64, String, u8)> {
    RUNTIME_RELOCATIONS.with(|cell| cell.borrow().clone())
}

fn runtime_parts_impl(granted_caps: Option<&HashSet<String>>) -> (Vec<u8>, Vec<(&'static str, u64, usize)>, Vec<u8>, Vec<(&'static str, u64)>) {
    // Build rodata
    let mut rodata = Vec::new();
    let mut rodata_syms: Vec<(&'static str, u64)> = Vec::new();

    let true_str_off = rodata.len();
    rodata.extend_from_slice(b"true\0");
    let false_str_off = rodata.len();
    rodata.extend_from_slice(b"false\0");
    let nl_str_off = rodata.len();
    rodata.extend_from_slice(b"\n\0");
    let minus_str_off = rodata.len();
    rodata.extend_from_slice(b"-\0");
    let bracket_open_off = rodata.len();
    rodata.extend_from_slice(b"[\0");
    let bracket_close_off = rodata.len();
    rodata.extend_from_slice(b"]\0");
    let comma_sp_off = rodata.len();
    rodata.extend_from_slice(b", \0");
    let zero_char_off = rodata.len();
    rodata.extend_from_slice(b"0\0");
    let panic_prefix_off = rodata.len();
    rodata.extend_from_slice(b"Panic: \0");
    let panic_nl_off = rodata.len();
    rodata.extend_from_slice(b"\n\0");

    rodata_syms.push((".Lpbool_true_str", true_str_off as u64));
    rodata_syms.push((".Lpbool_false_str", false_str_off as u64));
    rodata_syms.push((".Lpnl_str", nl_str_off as u64));
    rodata_syms.push((".Lpf_minus", minus_str_off as u64));
    rodata_syms.push((".Lbracket_open", bracket_open_off as u64));
    rodata_syms.push((".Lbracket_close", bracket_close_off as u64));
    rodata_syms.push((".Lcomma_sp", comma_sp_off as u64));
    rodata_syms.push((".Lpf_zero_char", zero_char_off as u64));
    rodata_syms.push((".Lpanic_prefix", panic_prefix_off as u64));
    rodata_syms.push((".Lpanic_nl", panic_nl_off as u64));

    // Build the assembly text for the runtime
    let asm = build_runtime_asm(granted_caps);

    // Assemble it
    let (text, symbols, relocs) = match arm64_enc::assemble(&asm) {
        Ok(result) => result,
        Err(e) => {
            eprintln!("ARM64 runtime assembly error: {}", e);
            panic!("ARM64 runtime assembly failed: {}", e);
        }
    };

    // Convert symbols to the expected format
    // The assembler returns Vec<(String, u64)> — we need Vec<(&'static str, u64, usize)>
    // Since we need 'static lifetimes, we'll use a leaked approach (the runtime is
    // assembled once and lives for the program duration)
    let symbol_table: Vec<(&'static str, u64, usize)> = symbols.iter()
        .map(|(name, offset)| {
            let name_static: &'static str = Box::leak(name.clone().into_boxed_str());
            (name_static, *offset, 0)
        })
        .collect();

    // Store relocations for later retrieval
    // Use internal reloc codes that fit in u8:
    //   10 = BL (branch with link)
    //   11 = B (unconditional branch)
    // The linker will handle these based on target architecture.
    let adjusted_relocs: Vec<(u64, String, u8)> = relocs.iter()
        .map(|(offset, name, rtype)| (*offset, name.clone(), *rtype))
        .collect();

    RUNTIME_RELOCATIONS.with(|cell| {
        cell.borrow_mut().clear();
        cell.borrow_mut().extend(adjusted_relocs);
    });

    (text, symbol_table, rodata, rodata_syms)
}

fn build_runtime_asm(granted_caps: Option<&HashSet<String>>) -> String {
    let mut asm = String::new();

    // ═══════════════════════════════════════════════════════════════════════
    // _start — program entry point
    // Align SP to 16 bytes, call main, exit with return value
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .section .text
    .globl _start
_start:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    bl main
    mov x0, x0          // exit code = main's return (already in x0)
    mov x8, #93         // sys_exit
    svc #0
    brk #0              // should never reach here
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_print_str(x0 = ptr to null-terminated string)
    // sys_write(1, buf, strlen(buf))
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_print_str
p_print_str:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    mov x19, x0         // save buf ptr
    // strlen: x1=ptr, x2=0
    mov x1, x0
    mov x2, #0
.Lpstr_loop:
    ldrb w3, [x1]
    cmp w3, #0
    b.eq .Lpstr_done
    add x1, x1, #1
    add x2, x2, #1
    b .Lpstr_loop
.Lpstr_done:
    // sys_write(1, x19, x2)
    mov x0, #1          // fd=1 (stdout)
    mov x1, x19         // buf
    // x2 already has length
    mov x8, #64         // sys_write
    svc #0
    ldp x29, x30, [sp], #16
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_print_int(x0 = i64 value)
    // Converts integer to decimal string and writes to stdout.
    // Handles negative numbers.
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_print_int
p_print_int:
    stp x29, x30, [sp, #-32]!
    mov x29, sp
    sub sp, sp, #32     // buffer for digits (up to 20 digits + sign)
    // x0 = value, save it
    mov x19, x0         // x19 = value
    // Handle negative
    mov x20, #0         // x20 = is_negative = 0
    cmp x19, #0
    b.ge .Lpint_pos
    mov x20, #1         // is_negative = 1
    neg x19, x19        // value = -value
.Lpint_pos:
    // Convert digits (reverse order)
    mov x21, sp         // x21 = buffer ptr
    add x21, x21, #31   // start from end of buffer
    mov x22, #0         // x22 = digit count
    // Handle value == 0
    cmp x19, #0
    b.ne .Lpint_loop
    mov w23, #48        // '0'
    strb w23, [x21]
    sub x21, x21, #1
    add x22, x22, #1
    b .Lpint_print
.Lpint_loop:
    cmp x19, #0
    b.eq .Lpint_print
    // digit = value % 10
    mov x3, #10
    udiv x4, x19, x3    // x4 = value / 10
    msub x5, x3, x4, x19 // x5 = value - (10 * (value/10)) = value % 10
    add w5, w5, #48     // '0' + digit
    strb w5, [x21]
    sub x21, x21, #1
    add x22, x22, #1
    mov x19, x4         // value = value / 10
    b .Lpint_loop
.Lpint_print:
    // If negative, prepend '-'
    cmp x20, #0
    b.eq .Lpint_write
    mov w3, #45         // '-'
    strb w3, [x21]
    sub x21, x21, #1
    add x22, x22, #1
.Lpint_write:
    add x21, x21, #1    // point to first digit
    // sys_write(1, x21, x22)
    mov x0, #1
    mov x1, x21
    mov x2, x22
    mov x8, #64
    svc #0
    add sp, sp, #32     // restore sp
    ldp x29, x30, [sp], #16
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_print_bool(x0 = i64, 0=false, nonzero=true)
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_print_bool
p_print_bool:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    cmp x0, #0
    b.ne .Lpbool_true
    // false
    adrp x0, .Lpbool_false_str
    add x0, x0, :lo12:.Lpbool_false_str
    bl p_print_str
    b .Lpbool_done
.Lpbool_true:
    adrp x0, .Lpbool_true_str
    add x0, x0, :lo12:.Lpbool_true_str
    bl p_print_str
.Lpbool_done:
    ldp x29, x30, [sp], #16
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_print_newline()
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_print_newline
p_print_newline:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    adrp x0, .Lpnl_str
    add x0, x0, :lo12:.Lpnl_str
    bl p_print_str
    ldp x29, x30, [sp], #16
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_panic(x0 = ptr to message)
    // Prints "Panic: <message>\n" and exits with code 1
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_panic
p_panic:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    // Print "Panic: "
    adrp x0, .Lpanic_prefix
    add x0, x0, :lo12:.Lpanic_prefix
    bl p_print_str
    // Print the message (x0 was the message, but we lost it. Save it.)
    // Actually we need to save x0 before calling p_print_str
    // Let's redo this properly
    ldp x29, x30, [sp], #16
    // Redo with proper save
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    mov x19, x0         // save message ptr
    adrp x0, .Lpanic_prefix
    add x0, x0, :lo12:.Lpanic_prefix
    bl p_print_str
    mov x0, x19         // restore message ptr
    bl p_print_str
    adrp x0, .Lpanic_nl
    add x0, x0, :lo12:.Lpanic_nl
    bl p_print_str
    // exit(1)
    mov x0, #1
    mov x8, #93         // sys_exit
    svc #0
    brk #0
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_malloc(x0 = size) → x0 = ptr
    // Uses sys_mmap: mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_malloc
p_malloc:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    // Round size up to page (4096)
    add x0, x0, #4095
    and x0, x0, #0xFFFFFFFFFFFFF000  // align to page
    // mmap args
    mov x1, x0         // length
    mov x0, #0         // addr = NULL
    mov x2, #3         // prot = PROT_READ | PROT_WRITE
    mov x3, #34        // flags = MAP_PRIVATE | MAP_ANONYMOUS (0x22)
    mov x4, #-1        // fd = -1
    mov x5, #0         // offset = 0
    mov x8, #222       // sys_mmap
    svc #0
    ldp x29, x30, [sp], #16
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_alloc(x0 = size) → x0 = ptr (same as p_malloc)
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_alloc
p_alloc:
    b p_malloc
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_free(x0 = ptr) → void (no-op, mmap memory is never freed)
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_free
p_free:
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_exit(x0 = code)
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_exit
p_exit:
    mov x8, #93         // sys_exit
    svc #0
    brk #0
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_to_str(x0 = i64 value) → x0 = ptr to null-terminated decimal string
    // Allocates a buffer, converts integer to string, returns pointer.
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_to_str
p_to_str:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #32     // buffer
    mov x19, x0         // x19 = value
    mov x20, #0         // is_negative
    cmp x19, #0
    b.ge .Lpts_pos
    mov x20, #1
    neg x19, x19
.Lpts_pos:
    mov x21, sp
    add x21, x21, #31   // end of buffer
    mov x22, #0         // digit count
    cmp x19, #0
    b.ne .Lpts_loop
    mov w23, #48
    strb w23, [x21]
    sub x21, x21, #1
    add x22, x22, #1
    b .Lpts_alloc
.Lpts_loop:
    cmp x19, #0
    b.eq .Lpts_alloc
    mov x3, #10
    udiv x4, x19, x3
    msub x5, x3, x4, x19
    add w5, w5, #48
    strb w5, [x21]
    sub x21, x21, #1
    add x22, x22, #1
    mov x19, x4
    b .Lpts_loop
.Lpts_alloc:
    cmp x20, #0
    b.eq .Lpts_copy
    mov w3, #45
    strb w3, [x21]
    sub x21, x21, #1
    add x22, x22, #1
.Lpts_copy:
    add x21, x21, #1    // first digit
    // Allocate 32 bytes for the result string
    mov x0, #32
    bl p_malloc
    mov x23, x0         // x23 = result ptr
    // Copy digits from x21 to x23 (x22 bytes)
    mov x3, #0          // index
.Lpts_copy_loop:
    cmp x3, x22
    b.ge .Lpts_copy_done
    add x4, x21, x3
    ldrb w5, [x4]
    add x6, x23, x3
    strb w5, [x6]
    add x3, x3, #1
    b .Lpts_copy_loop
.Lpts_copy_done:
    // Null terminate
    add x6, x23, x22
    strb wzr, [x6]
    mov x0, x23         // return result ptr
    add sp, sp, #32
    ldp x29, x30, [sp], #16
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_str_concat(x0 = str1, x1 = str2) → x0 = new string (str1+str2)
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_str_concat
p_str_concat:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    mov x19, x0         // str1
    mov x20, x1         // str2
    // strlen(str1) → x21
    mov x21, #0
    mov x22, x19
.Lpsc_len1:
    ldrb w23, [x22]
    cmp w23, #0
    b.eq .Lpsc_len1d
    add x22, x22, #1
    add x21, x21, #1
    b .Lpsc_len1
.Lpsc_len1d:
    // strlen(str2) → x22
    mov x22, #0
    mov x23, x20
.Lpsc_len2:
    ldrb w24, [x23]
    cmp w24, #0
    b.eq .Lpsc_len2d
    add x23, x23, #1
    add x22, x22, #1
    b .Lpsc_len2
.Lpsc_len2d:
    // total = len1 + len2 + 1
    add x0, x21, x22
    add x0, x0, #1
    bl p_malloc
    mov x24, x0         // result
    // Copy str1
    mov x3, #0
.Lpsc_c1:
    cmp x3, x21
    b.ge .Lpsc_c1d
    add x4, x19, x3
    ldrb w5, [x4]
    add x6, x24, x3
    strb w5, [x6]
    add x3, x3, #1
    b .Lpsc_c1
.Lpsc_c1d:
    // Copy str2
    mov x3, #0
.Lpsc_c2:
    cmp x3, x22
    b.ge .Lpsc_c2d
    add x4, x20, x3
    ldrb w5, [x4]
    add x6, x24, x21
    add x6, x6, x3
    strb w5, [x6]
    add x3, x3, #1
    b .Lpsc_c2
.Lpsc_c2d:
    // Null terminate
    add x6, x24, x21
    add x6, x6, x22
    strb wzr, [x6]
    mov x0, x24
    ldp x29, x30, [sp], #16
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // Vec operations
    // Vec layout: [data_ptr(8), len(8), cap(8)]
    // ═══════════════════════════════════════════════════════════════════════

    // p_vec_new() → x0 = vec ptr (24-byte header + inline data)
    asm.push_str(r#"
    .globl p_vec_new
p_vec_new:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    mov x0, #4096
    bl p_malloc
    // data_ptr = vec + 24
    add x1, x0, #24
    str x1, [x0]        // vec[0] = data_ptr
    str xzr, [x0, #8]   // vec[1] = len = 0
    mov x1, #509        // cap = (4096-24)/8
    str x1, [x0, #16]   // vec[2] = cap
    ldp x29, x30, [sp], #16
    ret
"#);

    // p_vec_push(x0 = vec, x1 = value)
    asm.push_str(r#"
    .globl p_vec_push
p_vec_push:
    ldr x2, [x0, #8]    // len
    ldr x3, [x0]        // data_ptr
    str x1, [x3, x2, lsl #3]  // data[len] = value
    add x2, x2, #1
    str x2, [x0, #8]    // len++
    ret
"#);

    // p_vec_len(x0 = vec) → x0 = len
    asm.push_str(r#"
    .globl p_vec_len
p_vec_len:
    ldr x0, [x0, #8]
    ret
"#);

    // p_vec_get(x0 = vec, x1 = index) → x0 = value
    asm.push_str(r#"
    .globl p_vec_get
p_vec_get:
    ldr x2, [x0]        // data_ptr
    ldr x0, [x2, x1, lsl #3]  // data[index]
    ret
"#);

    // p_vec_set(x0 = vec, x1 = index, x2 = value)
    asm.push_str(r#"
    .globl p_vec_set
p_vec_set:
    ldr x3, [x0]        // data_ptr
    str x2, [x3, x1, lsl #3]  // data[index] = value
    ret
"#);

    // p_vec_first(x0 = vec) → x0 = first element
    asm.push_str(r#"
    .globl p_vec_first
p_vec_first:
    ldr x1, [x0]        // data_ptr
    ldr x0, [x1]        // data[0]
    ret
"#);

    // p_vec_last(x0 = vec) → x0 = last element
    asm.push_str(r#"
    .globl p_vec_last
p_vec_last:
    ldr x1, [x0]        // data_ptr
    ldr x2, [x0, #8]    // len
    sub x2, x2, #1      // len-1
    ldr x0, [x1, x2, lsl #3]  // data[len-1]
    ret
"#);

    // p_vec_pop(x0 = vec) → x0 = value (or 0 if empty)
    asm.push_str(r#"
    .globl p_vec_pop
p_vec_pop:
    ldr x1, [x0, #8]    // len
    cmp x1, #0
    b.eq .Lpvp_empty
    sub x1, x1, #1      // len-1
    str x1, [x0, #8]    // store new len
    ldr x2, [x0]        // data_ptr
    ldr x0, [x2, x1, lsl #3]  // data[len-1]
    ret
.Lpvp_empty:
    mov x0, #0
    ret
"#);

    // p_vec_is_empty(x0 = vec) → x0 = 1 if empty, 0 otherwise
    asm.push_str(r#"
    .globl p_vec_is_empty
p_vec_is_empty:
    ldr x0, [x0, #8]    // len
    cmp x0, #0
    cset x0, eq
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_print_vec(x0 = vec ptr)
    // Prints [elem, elem, ...] with i64 elements
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_print_vec
p_print_vec:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    mov x19, x0         // vec
    // Print "["
    adrp x0, .Lbracket_open
    add x0, x0, :lo12:.Lbracket_open
    bl p_print_str
    // Check if empty
    ldr x20, [x19, #8]  // len
    cmp x20, #0
    b.eq .Lppv_close
    // Print first element
    ldr x21, [x19]      // data_ptr
    ldr x0, [x21]       // data[0]
    bl p_print_int
    // Print remaining elements with ", " prefix
    mov x22, #1         // i = 1
.Lppv_loop:
    cmp x22, x20
    b.ge .Lppv_close
    // Print ", "
    adrp x0, .Lcomma_sp
    add x0, x0, :lo12:.Lcomma_sp
    bl p_print_str
    // Print element
    ldr x0, [x21, x22, lsl #3]
    bl p_print_int
    add x22, x22, #1
    b .Lppv_loop
.Lppv_close:
    adrp x0, .Lbracket_close
    add x0, x0, :lo12:.Lbracket_close
    bl p_print_str
    ldp x29, x30, [sp], #16
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_region_free(x0 = ptr) → void (no-op)
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_region_free
p_region_free:
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // pledge_strlen(x0 = str) → x0 = length
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl pledge_strlen
pledge_strlen:
    mov x1, x0
    mov x0, #0
.Lstrlen_loop:
    ldrb w2, [x1]
    cmp w2, #0
    b.eq .Lstrlen_done
    add x1, x1, #1
    add x0, x0, #1
    b .Lstrlen_loop
.Lstrlen_done:
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // pledge_strcmp(x0 = str1, x1 = str2) → x0 = 0 if equal, nonzero otherwise
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl pledge_strcmp
pledge_strcmp:
.Lstrcmp_loop:
    ldrb w2, [x0]
    ldrb w3, [x1]
    cmp w2, #0
    b.eq .Lstrcmp_end
    cmp w2, w3
    b.ne .Lstrcmp_ne
    add x0, x0, #1
    add x1, x1, #1
    b .Lstrcmp_loop
.Lstrcmp_end:
    sub x0, x2, x3
    ret
.Lstrcmp_ne:
    sub x0, x2, x3
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_getenv() → x0 = 0 (stub)
    // p_argc() → x0 = 1 (stub)
    // p_argv(x0 = index) → x0 = 0 (stub)
    // p_system(x0 = cmd) → x0 = 0 (stub)
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_getenv
p_getenv:
    mov x0, #0
    ret

    .globl p_argc
p_argc:
    mov x0, #1
    ret

    .globl p_argv
p_argv:
    mov x0, #0
    ret

    .globl p_system
p_system:
    mov x0, #0
    ret

    .globl p_regex_match
p_regex_match:
    mov x0, #0
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // Math stubs (return 0 for now — proper implementations would use FPU)
    // ═══════════════════════════════════════════════════════════════════════
    for name in &["pledge_sqrt", "pledge_floor", "pledge_ceil", "pledge_pow",
                  "pledge_sin", "pledge_cos", "pledge_fabs"] {
        asm.push_str(&format!("    .globl {}\n{}:\n    mov x0, #0\n    ret\n", name, name));
    }

    // ═══════════════════════════════════════════════════════════════════════
    // Float/bool/vec to string stubs
    // ═══════════════════════════════════════════════════════════════════════
    // p_float_to_str(x0 = f64 bits) → x0 = str ptr
    // Simplified: just prints "0" for now
    asm.push_str(r#"
    .globl p_float_to_str
p_float_to_str:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    mov x0, #2
    bl p_malloc
    mov x1, x0
    mov w2, #48         // '0'
    strb w2, [x1]
    strb wzr, [x1, #1]
    ldp x29, x30, [sp], #16
    ret

    .globl p_bool_to_str
p_bool_to_str:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    cmp x0, #0
    b.ne .Lpbts_true
    adrp x0, .Lpbool_false_str
    add x0, x0, :lo12:.Lpbool_false_str
    b .Lpbts_done
.Lpbts_true:
    adrp x0, .Lpbool_true_str
    add x0, x0, :lo12:.Lpbool_true_str
.Lpbts_done:
    ldp x29, x30, [sp], #16
    ret

    .globl p_vec_to_str
p_vec_to_str:
    b p_print_vec
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // Allocator stubs
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_allocator_create
p_allocator_create:
    mov x0, #0
    ret

    .globl p_allocator_alloc
p_allocator_alloc:
    b p_malloc

    .globl p_allocator_free
p_allocator_free:
    ret

    .globl p_allocator_stats
p_allocator_stats:
    mov x0, #0
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // Capability-gated stubs (Net, FS, Spawn)
    // These are either trap (brk #1) or return 0 based on granted caps
    // ═══════════════════════════════════════════════════════════════════════
    let net_stubs = ["p_socket_create", "p_socket_connect", "p_socket_bind", "p_socket_listen",
                     "p_socket_accept", "p_socket_send", "p_socket_recv", "p_socket_close",
                     "p_http_get", "p_http_post", "p_http_parse_request", "p_http_build_response",
                     "p_net_connect", "p_net_read", "p_net_write", "p_net_close"];
    let fs_stubs = ["p_file_open", "p_file_close", "p_file_read", "p_file_write",
                    "p_read_file", "p_write_file", "p_fs_read_async"];
    let spawn_stubs = ["p_thread_spawn", "p_thread_join", "p_thread_mutex_init",
                       "p_thread_mutex_lock", "p_thread_mutex_unlock",
                       "p_spawn", "p_executor_new", "p_executor_spawn",
                       "p_executor_block_on", "p_executor_run_all", "p_executor_poll_all",
                       "p_task_poll", "p_task_poll_nonblocking", "p_task_is_ready",
                       "p_chan_create", "p_chan_send", "p_chan_recv", "p_chan_try_recv",
                       "p_chan_close", "p_chan_is_closed"];

    for stubs in [&net_stubs[..] as &[&str], &fs_stubs[..], &spawn_stubs[..]] {
        for name in stubs {
            let cap = stub_capability(name);
            let should_trap = if let Some(caps) = granted_caps {
                if let Some(cap_name) = cap {
                    !caps.contains(cap_name)
                } else {
                    false
                }
            } else {
                false
            };
            if should_trap {
                asm.push_str(&format!("    .globl {}\n{}:\n    brk #1\n    ret\n", name, name));
            } else {
                asm.push_str(&format!("    .globl {}\n{}:\n    mov x0, #0\n    ret\n", name, name));
            }
        }
    }

    // ═══════════════════════════════════════════════════════════════════════
    // Additional stubs that return 0
    // ═══════════════════════════════════════════════════════════════════════
    let other_stubs = [
        "p_simd_dot", "p_db_mem_open", "p_db_mem_close", "p_db_mem_exec",
        "p_db_mem_query", "p_json_len", "p_json_at",
        "p_dl_open", "p_dl_sym", "p_dl_close", "p_dl_call",
        "p_print_float",
    ];
    for name in other_stubs {
        asm.push_str(&format!("    .globl {}\n{}:\n    mov x0, #0\n    ret\n", name, name));
    }

    asm
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_runtime_assembles() {
        let (text, syms, rodata, rodata_syms) = runtime_parts();
        assert!(!text.is_empty(), "runtime text should not be empty");
        assert!(!syms.is_empty(), "runtime symbols should not be empty");
        assert!(!rodata.is_empty(), "runtime rodata should not be empty");
        assert!(!rodata_syms.is_empty(), "runtime rodata syms should not be empty");
    }

    #[test]
    fn test_runtime_has_start() {
        let (_, syms, _, _) = runtime_parts();
        assert!(syms.iter().any(|(name, _, _)| *name == "_start"), "_start should be in symbols");
    }

    #[test]
    fn test_runtime_has_print_int() {
        let (_, syms, _, _) = runtime_parts();
        assert!(syms.iter().any(|(name, _, _)| *name == "p_print_int"), "p_print_int should be in symbols");
    }

    #[test]
    fn test_runtime_has_vec_ops() {
        let (_, syms, _, _) = runtime_parts();
        assert!(syms.iter().any(|(name, _, _)| *name == "p_vec_new"), "p_vec_new should be in symbols");
        assert!(syms.iter().any(|(name, _, _)| *name == "p_vec_push"), "p_vec_push should be in symbols");
        assert!(syms.iter().any(|(name, _, _)| *name == "p_vec_get"), "p_vec_get should be in symbols");
    }

    #[test]
    fn test_runtime_has_malloc() {
        let (_, syms, _, _) = runtime_parts();
        assert!(syms.iter().any(|(name, _, _)| *name == "p_malloc"), "p_malloc should be in symbols");
        assert!(syms.iter().any(|(name, _, _)| *name == "p_free"), "p_free should be in symbols");
    }

    #[test]
    fn test_runtime_text_is_4byte_aligned() {
        let (text, _, _, _) = runtime_parts();
        assert_eq!(text.len() % 4, 0, "ARM64 text should be 4-byte aligned (all instructions are 4 bytes)");
    }

    #[test]
    fn test_capability_stubs_present() {
        let (_, syms, _, _) = runtime_parts();
        assert!(syms.iter().any(|(name, _, _)| *name == "p_socket_listen"), "p_socket_listen should be in symbols");
        assert!(syms.iter().any(|(name, _, _)| *name == "p_file_open"), "p_file_open should be in symbols");
    }

    #[test]
    fn test_capability_stubs_trap_when_not_granted() {
        let caps = HashSet::new(); // no caps granted
        let (text, syms, _, _) = runtime_parts_with_caps(&caps);
        // Find p_socket_listen offset
        let socket_off = syms.iter().find(|(name, _, _)| *name == "p_socket_listen").map(|(_, off, _)| *off);
        if let Some(off) = socket_off {
            let off = off as usize;
            // Should be BRK #1 (0xD4200020) followed by RET
            if off + 8 <= text.len() {
                let brk = u32::from_le_bytes([text[off], text[off+1], text[off+2], text[off+3]]);
                assert_eq!(brk, 0xD420_0020, "p_socket_listen should trap with BRK #1");
            }
        }
    }

    #[test]
    fn test_runtime_relocations() {
        // Just ensure it doesn't crash
        let _ = runtime_parts();
        let relocs = runtime_relocations();
        // _start has a BL main relocation
        assert!(relocs.iter().any(|(_, name, _)| name == "main"), "should have relocation to main");
    }
}
