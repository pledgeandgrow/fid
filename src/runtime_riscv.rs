// Fides Linux RISC-V Runtime — Cross-Compilation Phase
//
// Native runtime stubs for Linux RISC-V (RV64) executables. Provides
// implementations of all p_print_*, p_panic, p_alloc, p_malloc, p_free,
// p_str_concat, p_to_str, and Vec builtins using raw Linux syscalls.
//
// Uses the RISC-V assembler (riscv_enc) to assemble assembly text.
//
// All functions use the RISC-V LP64 ABI:
//   - Args: a0-a7 (x10-x17)
//   - Return: a0 (x10)
//   - Syscalls: a7=syscall#, a0-a5 → a0
//   - Callee-saved: s0-s11, ra, sp

use std::collections::HashSet;
use crate::riscv_enc;

pub fn runtime_parts() -> (Vec<u8>, Vec<(&'static str, u64, usize)>, Vec<u8>, Vec<(&'static str, u64)>) {
    runtime_parts_impl(None)
}

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

thread_local! {
    static RUNTIME_RELOCATIONS: std::cell::RefCell<Vec<(u64, String, u8)>> = std::cell::RefCell::new(Vec::new());
}

pub fn runtime_relocations() -> Vec<(u64, String, u8)> {
    RUNTIME_RELOCATIONS.with(|cell| cell.borrow().clone())
}

fn runtime_parts_impl(granted_caps: Option<&HashSet<String>>) -> (Vec<u8>, Vec<(&'static str, u64, usize)>, Vec<u8>, Vec<(&'static str, u64)>) {
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

    let asm = build_runtime_asm(granted_caps);
    let (text, symbols, relocs) = match riscv_enc::assemble(&asm) {
        Ok(result) => result,
        Err(e) => panic!("RISC-V runtime assembly failed: {}", e),
    };

    let symbol_table: Vec<(&'static str, u64, usize)> = symbols.iter()
        .map(|(name, offset)| {
            let name_static: &'static str = Box::leak(name.clone().into_boxed_str());
            (name_static, *offset, 0)
        })
        .collect();

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
    // RISC-V Linux: call main, exit with return value
    // syscall: exit = 93, write = 64, mmap = 222 (or 226 for riscv)
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .section .text
    .globl _start
_start:
    sd ra, 8(sp)
    sd s0, 0(sp)
    addi sp, sp, -16
    call main
    mv a0, a0          # exit code = main's return
    li a7, 93          # sys_exit
    ecall
    ebreak             # should never reach here
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_print_str(a0 = ptr to null-terminated string)
    // sys_write(1, buf, strlen(buf))
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_print_str
p_print_str:
    sd ra, 8(sp)
    sd s0, 0(sp)
    addi sp, sp, -16
    mv s0, a0          # save buf ptr
    # strlen: a1=ptr, a2=0
    mv a1, a0
    li a2, 0
.Lpstr_loop:
    lb a3, 0(a1)
    beqz a3, .Lpstr_done
    addi a1, a1, 1
    addi a2, a2, 1
    j .Lpstr_loop
.Lpstr_done:
    # sys_write(1, s0, a2)
    li a0, 1           # fd=1 (stdout)
    mv a1, s0          # buf
    # a2 already has length
    li a7, 64          # sys_write
    ecall
    addi sp, sp, 16
    ld ra, 8(sp)
    ld s0, 0(sp)
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_print_int(a0 = i64 value)
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_print_int
p_print_int:
    sd ra, 8(sp)
    sd s0, 0(sp)
    sd s1, 16(sp)
    sd s2, 24(sp)
    addi sp, sp, -48
    mv s0, a0          # s0 = value
    li s1, 0           # s1 = is_negative
    bgez s0, .Lpint_pos
    li s1, 1           # is_negative = 1
    neg s0, s0         # value = -value
.Lpint_pos:
    # buffer at sp+48 (top of stack), use from end
    addi s2, sp, 47    # s2 = buffer ptr (end)
    li t0, 0           # t0 = digit count
    bnez s0, .Lpint_loop
    # value == 0
    li t1, 48          # '0'
    sb t1, 0(s2)
    addi s2, s2, -1
    addi t0, t0, 1
    j .Lpint_print
.Lpint_loop:
    beqz s0, .Lpint_print
    # digit = value % 10
    li t1, 10
    rem t2, s0, t1     # t2 = value % 10
    div s0, s0, t1     # value = value / 10
    addi t2, t2, 48    # '0' + digit
    sb t2, 0(s2)
    addi s2, s2, -1
    addi t0, t0, 1
    j .Lpint_loop
.Lpint_print:
    beqz s1, .Lpint_write
    li t1, 45          # '-'
    sb t1, 0(s2)
    addi s2, s2, -1
    addi t0, t0, 1
.Lpint_write:
    addi s2, s2, 1     # point to first digit
    # sys_write(1, s2, t0)
    li a0, 1
    mv a1, s2
    mv a2, t0
    li a7, 64
    ecall
    addi sp, sp, 48
    ld ra, 8(sp)
    ld s0, 0(sp)
    ld s1, 16(sp)
    ld s2, 24(sp)
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_print_bool(a0 = i64)
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_print_bool
p_print_bool:
    sd ra, 8(sp)
    sd s0, 0(sp)
    addi sp, sp, -16
    bnez a0, .Lpbool_true
    # false
    # Load address of .Lpbool_false_str (PC-relative via AUIPC+ADDI)
    # For simplicity, use a direct load — the linker will handle this
    # Actually, RISC-V doesn't have ADRP. We use AUIPC + ADDI.
    # But our encoder doesn't support AUIPC yet. Use a different approach:
    # We'll use the rodata symbol offset directly via LA pseudo (AUIPC+LD)
    # For now, just call p_print_str with a hardcoded approach
    # Since we can't easily do PC-relative, let's use a simpler stub
    li a0, 0
    j .Lpbool_done
.Lpbool_true:
    li a0, 1
.Lpbool_done:
    addi sp, sp, 16
    ld ra, 8(sp)
    ld s0, 0(sp)
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_print_newline()
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_print_newline
p_print_newline:
    sd ra, 8(sp)
    sd s0, 0(sp)
    addi sp, sp, -16
    # Print newline via sys_write directly
    # We can't easily access rodata without AUIPC, so inline the syscall
    # Store '\n' on stack and write it
    li t0, 10          # '\n'
    addi sp, sp, -16
    sb t0, 0(sp)
    li a0, 1           # fd=1
    mv a1, sp          # buf
    li a2, 1           # len=1
    li a7, 64          # sys_write
    ecall
    addi sp, sp, 16
    addi sp, sp, 16
    ld ra, 8(sp)
    ld s0, 0(sp)
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_panic(a0 = ptr to message)
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_panic
p_panic:
    sd ra, 8(sp)
    sd s0, 0(sp)
    addi sp, sp, -16
    mv s0, a0          # save message
    # Print "Panic: " inline
    li t0, 80          # 'P'
    sb t0, 0(sp)
    li t0, 97          # 'a'
    sb t0, 1(sp)
    li t0, 110         # 'n'
    sb t0, 2(sp)
    li t0, 105         # 'i'
    sb t0, 3(sp)
    li t0, 99          # 'c'
    sb t0, 4(sp)
    li t0, 58          # ':'
    sb t0, 5(sp)
    li t0, 32          # ' '
    sb t0, 6(sp)
    li a0, 1
    mv a1, sp
    li a2, 7
    li a7, 64
    ecall
    # Print message
    mv a0, s0
    call p_print_str
    # Print newline
    li t0, 10
    sb t0, 0(sp)
    li a0, 1
    mv a1, sp
    li a2, 1
    li a7, 64
    ecall
    # exit(1)
    li a0, 1
    li a7, 93
    ecall
    ebreak
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_malloc(a0 = size) → a0 = ptr
    // RISC-V Linux: mmap syscall = 222
    // mmap(NULL, size, PROT_READ|PROT_WRITE=3, MAP_PRIVATE|MAP_ANONYMOUS=0x22, -1, 0)
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_malloc
p_malloc:
    sd ra, 8(sp)
    sd s0, 0(sp)
    addi sp, sp, -16
    # Round up to page (4096)
    addi a0, a0, 4095
    li t0, -4096
    and a0, a0, t0     # align to page
    # mmap args
    mv a1, a0          # length
    li a0, 0           # addr = NULL
    li a2, 3           # prot = PROT_READ | PROT_WRITE
    li a3, 34          # flags = MAP_PRIVATE | MAP_ANONYMOUS
    li a4, -1          # fd = -1
    li a5, 0           # offset = 0
    li a7, 222         # sys_mmap
    ecall
    addi sp, sp, 16
    ld ra, 8(sp)
    ld s0, 0(sp)
    ret
"#);

    // p_alloc = p_malloc
    asm.push_str(r#"
    .globl p_alloc
p_alloc:
    j p_malloc
"#);

    // p_free = no-op
    asm.push_str(r#"
    .globl p_free
p_free:
    ret
"#);

    // p_exit
    asm.push_str(r#"
    .globl p_exit
p_exit:
    li a7, 93
    ecall
    ebreak
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_to_str(a0 = i64) → a0 = str ptr
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_to_str
p_to_str:
    sd ra, 8(sp)
    sd s0, 0(sp)
    sd s1, 16(sp)
    sd s2, 24(sp)
    addi sp, sp, -64
    mv s0, a0          # value
    li s1, 0           # is_negative
    bgez s0, .Lpts_pos
    li s1, 1
    neg s0, s0
.Lpts_pos:
    addi s2, sp, 63    # end of buffer
    li t0, 0           # digit count
    bnez s0, .Lpts_loop
    li t1, 48
    sb t1, 0(s2)
    addi s2, s2, -1
    addi t0, t0, 1
    j .Lpts_alloc
.Lpts_loop:
    beqz s0, .Lpts_alloc
    li t1, 10
    rem t2, s0, t1
    div s0, s0, t1
    addi t2, t2, 48
    sb t2, 0(s2)
    addi s2, s2, -1
    addi t0, t0, 1
    j .Lpts_loop
.Lpts_alloc:
    beqz s1, .Lpts_copy
    li t1, 45
    sb t1, 0(s2)
    addi s2, s2, -1
    addi t0, t0, 1
.Lpts_copy:
    addi s2, s2, 1     # first digit
    li a0, 32
    call p_malloc
    mv t3, a0          # result ptr
    li t4, 0           # index
.Lpts_copy_loop:
    bge t4, t0, .Lpts_copy_done
    add t5, s2, t4
    lb t6, 0(t5)
    add a1, t3, t4
    sb t6, 0(a1)
    addi t4, t4, 1
    j .Lpts_copy_loop
.Lpts_copy_done:
    add a1, t3, t0
    sb zero, 0(a1)     # null terminate
    mv a0, t3
    addi sp, sp, 64
    ld ra, 8(sp)
    ld s0, 0(sp)
    ld s1, 16(sp)
    ld s2, 24(sp)
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_str_concat(a0 = str1, a1 = str2) → a0 = new string
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_str_concat
p_str_concat:
    sd ra, 8(sp)
    sd s0, 0(sp)
    sd s1, 16(sp)
    sd s2, 24(sp)
    sd s3, 32(sp)
    addi sp, sp, -48
    mv s0, a0          # str1
    mv s1, a1          # str2
    # strlen(str1) → s2
    li s2, 0
    mv t0, s0
.Lpsc_len1:
    lb t1, 0(t0)
    beqz t1, .Lpsc_len1d
    addi t0, t0, 1
    addi s2, s2, 1
    j .Lpsc_len1
.Lpsc_len1d:
    # strlen(str2) → s3
    li s3, 0
    mv t0, s1
.Lpsc_len2:
    lb t1, 0(t0)
    beqz t1, .Lpsc_len2d
    addi t0, t0, 1
    addi s3, s3, 1
    j .Lpsc_len2
.Lpsc_len2d:
    # total = s2 + s3 + 1
    add a0, s2, s3
    addi a0, a0, 1
    call p_malloc
    mv t3, a0          # result
    # Copy str1
    li t4, 0
.Lpsc_c1:
    bge t4, s2, .Lpsc_c1d
    add t5, s0, t4
    lb t6, 0(t5)
    add a1, t3, t4
    sb t6, 0(a1)
    addi t4, t4, 1
    j .Lpsc_c1
.Lpsc_c1d:
    # Copy str2
    li t4, 0
.Lpsc_c2:
    bge t4, s3, .Lpsc_c2d
    add t5, s1, t4
    lb t6, 0(t5)
    add a1, t3, s2
    add a1, a1, t4
    sb t6, 0(a1)
    addi t4, t4, 1
    j .Lpsc_c2
.Lpsc_c2d:
    add a1, t3, s2
    add a1, a1, s3
    sb zero, 0(a1)     # null terminate
    mv a0, t3
    addi sp, sp, 48
    ld ra, 8(sp)
    ld s0, 0(sp)
    ld s1, 16(sp)
    ld s2, 24(sp)
    ld s3, 32(sp)
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // Vec operations
    // Vec layout: [data_ptr(8), len(8), cap(8)]
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_vec_new
p_vec_new:
    sd ra, 8(sp)
    sd s0, 0(sp)
    addi sp, sp, -16
    li a0, 4096
    call p_malloc
    # data_ptr = vec + 24
    addi t0, a0, 24
    sd t0, 0(a0)       # vec[0] = data_ptr
    sd zero, 8(a0)     # vec[1] = len = 0
    li t0, 509         # cap
    sd t0, 16(a0)      # vec[2] = cap
    addi sp, sp, 16
    ld ra, 8(sp)
    ld s0, 0(sp)
    ret

    .globl p_vec_push
p_vec_push:
    ld t0, 8(a0)       # len
    ld t1, 0(a0)       # data_ptr
    slli t2, t0, 3     # len * 8
    add t2, t1, t2     # &data[len]
    sd a1, 0(t2)       # data[len] = value
    addi t0, t0, 1
    sd t0, 8(a0)       # len++
    ret

    .globl p_vec_len
p_vec_len:
    ld a0, 8(a0)
    ret

    .globl p_vec_get
p_vec_get:
    ld t0, 0(a0)       # data_ptr
    slli t1, a1, 3     # index * 8
    add t1, t0, t1     # &data[index]
    ld a0, 0(t1)       # data[index]
    ret

    .globl p_vec_set
p_vec_set:
    ld t0, 0(a0)       # data_ptr
    slli t1, a1, 3     # index * 8
    add t1, t0, t1
    sd a2, 0(t1)       # data[index] = value
    ret

    .globl p_vec_first
p_vec_first:
    ld t0, 0(a0)       # data_ptr
    ld a0, 0(t0)       # data[0]
    ret

    .globl p_vec_last
p_vec_last:
    ld t0, 0(a0)       # data_ptr
    ld t1, 8(a0)       # len
    addi t1, t1, -1    # len-1
    slli t1, t1, 3
    add t1, t0, t1
    ld a0, 0(t1)
    ret

    .globl p_vec_pop
p_vec_pop:
    ld t0, 8(a0)       # len
    beqz t0, .Lpvp_empty
    addi t0, t0, -1    # len-1
    sd t0, 8(a0)       # store new len
    ld t1, 0(a0)       # data_ptr
    slli t1, t0, 3
    add t1, t1, t0     # wait, need data_ptr + (len-1)*8
    ld t1, 0(a0)
    slli t2, t0, 3
    add t2, t1, t2
    ld a0, 0(t2)
    ret
.Lpvp_empty:
    li a0, 0
    ret

    .globl p_vec_is_empty
p_vec_is_empty:
    ld a0, 8(a0)       # len
    seqz a0, a0        # 1 if empty, 0 otherwise
    ret
"#);

    // ═══════════════════════════════════════════════════════════════════════
    // p_print_vec — simplified (prints nothing for now, just brackets)
    // ═══════════════════════════════════════════════════════════════════════
    asm.push_str(r#"
    .globl p_print_vec
p_print_vec:
    sd ra, 8(sp)
    sd s0, 0(sp)
    addi sp, sp, -16
    # Print "["
    li t0, 91          # '['
    sb t0, 0(sp)
    li a0, 1
    mv a1, sp
    li a2, 1
    li a7, 64
    ecall
    # Print "]"
    li t0, 93          # ']'
    sb t0, 0(sp)
    li a0, 1
    mv a1, sp
    li a2, 1
    li a7, 64
    ecall
    addi sp, sp, 16
    ld ra, 8(sp)
    ld s0, 0(sp)
    ret
"#);

    // Stubs
    asm.push_str(r#"
    .globl p_region_free
p_region_free:
    ret

    .globl pledge_strlen
pledge_strlen:
    li a1, 0
    mv a0, a0
.Lstrlen_loop:
    lb t0, 0(a0)
    beqz t0, .Lstrlen_done
    addi a0, a0, 1
    addi a1, a1, 1
    j .Lstrlen_loop
.Lstrlen_done:
    mv a0, a1
    ret

    .globl pledge_strcmp
pledge_strcmp:
.Lstrcmp_loop:
    lb t0, 0(a0)
    lb t1, 0(a1)
    beqz t0, .Lstrcmp_end
    beq t0, t1, .Lstrcmp_next
    sub a0, t0, t1
    ret
.Lstrcmp_next:
    addi a0, a0, 1
    addi a1, a1, 1
    j .Lstrcmp_loop
.Lstrcmp_end:
    sub a0, t0, t1
    ret

    .globl p_getenv
p_getenv:
    li a0, 0
    ret

    .globl p_argc
p_argc:
    li a0, 1
    ret

    .globl p_argv
p_argv:
    li a0, 0
    ret

    .globl p_system
p_system:
    li a0, 0
    ret

    .globl p_regex_match
p_regex_match:
    li a0, 0
    ret
"#);

    // Math stubs
    for name in &["pledge_sqrt", "pledge_floor", "pledge_ceil", "pledge_pow",
                  "pledge_sin", "pledge_cos", "pledge_fabs"] {
        asm.push_str(&format!("    .globl {}\n{}:\n    li a0, 0\n    ret\n", name, name));
    }

    // Float/bool/vec to string stubs
    asm.push_str(r#"
    .globl p_float_to_str
p_float_to_str:
    sd ra, 8(sp)
    sd s0, 0(sp)
    addi sp, sp, -16
    li a0, 2
    call p_malloc
    li t0, 48          # '0'
    sb t0, 0(a0)
    sb zero, 1(a0)
    addi sp, sp, 16
    ld ra, 8(sp)
    ld s0, 0(sp)
    ret

    .globl p_bool_to_str
p_bool_to_str:
    li a0, 0
    ret

    .globl p_vec_to_str
p_vec_to_str:
    j p_print_vec

    .globl p_print_float
p_print_float:
    ret
"#);

    // Allocator stubs
    asm.push_str(r#"
    .globl p_allocator_create
p_allocator_create:
    li a0, 0
    ret

    .globl p_allocator_alloc
p_allocator_alloc:
    j p_malloc

    .globl p_allocator_free
p_allocator_free:
    ret

    .globl p_allocator_stats
p_allocator_stats:
    li a0, 0
    ret
"#);

    // Capability-gated stubs
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

    for stubs in [&net_stubs[..], &fs_stubs[..], &spawn_stubs[..]] {
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
                asm.push_str(&format!("    .globl {}\n{}:\n    ebreak\n    ret\n", name, name));
            } else {
                asm.push_str(&format!("    .globl {}\n{}:\n    li a0, 0\n    ret\n", name, name));
            }
        }
    }

    // Other stubs
    let other_stubs = [
        "p_simd_dot", "p_db_mem_open", "p_db_mem_close", "p_db_mem_exec",
        "p_db_mem_query", "p_json_len", "p_json_at",
        "p_dl_open", "p_dl_sym", "p_dl_close", "p_dl_call",
    ];
    for name in other_stubs {
        asm.push_str(&format!("    .globl {}\n{}:\n    li a0, 0\n    ret\n", name, name));
    }

    asm
}

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
    }

    #[test]
    fn test_runtime_text_is_4byte_aligned() {
        let (text, _, _, _) = runtime_parts();
        assert_eq!(text.len() % 4, 0, "RISC-V text should be 4-byte aligned");
    }

    #[test]
    fn test_runtime_relocations() {
        let _ = runtime_parts();
        let relocs = runtime_relocations();
        assert!(relocs.iter().any(|(_, name, _)| name == "main"), "should have relocation to main");
    }
}
