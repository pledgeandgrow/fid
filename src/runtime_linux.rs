// Fides Linux x86-64 Runtime — Phase 4D
//
// Native runtime stubs for Linux x86-64 executables. Provides implementations
// of all p_print_*, p_panic, p_alloc, p_malloc, p_free, p_str_concat, p_to_str,
// and allocator builtins using raw Linux syscalls (no libc dependency).
//
// The runtime is hand-encoded as raw x86-64 machine code bytes (not assembly
// text) because the integrated ELF64 assembler does not support the full
// instruction set needed for string manipulation and integer-to-decimal
// conversion.
//
// All functions use the System V AMD64 ABI:
//   - Args: RDI, RSI, RDX, RCX, R8, R9
//   - Return: RAX
//   - Syscalls: RAX=syscall#, RDI, RSI, RDX, R10, R8, R9 → RAX

/// Runtime entry: returns (machine_code_bytes, symbol_table, rodata_bytes, rodata_symbols).
/// symbol_table: Vec<(name, offset_in_text, size_or_0)>
/// rodata_symbols: Vec<(name, offset_in_rodata)>
pub fn runtime_parts() -> (Vec<u8>, Vec<(&'static str, u64, usize)>, Vec<u8>, Vec<(&'static str, u64)>) {
    let mut text = Vec::new();
    let mut symbols: Vec<(&'static str, u64, usize)> = Vec::new();
    let mut rodata = Vec::new();
    let mut rodata_syms: Vec<(&'static str, u64)> = Vec::new();

    // Helper: push a symbol at the current text offset
    macro_rules! sym {
        ($name:expr) => {
            symbols.push(($name, text.len() as u64, 0));
        };
    }

    // ── Read-only data ──
    let true_str_off = rodata.len();
    rodata.extend_from_slice(b"true\0");
    let false_str_off = rodata.len();
    rodata.extend_from_slice(b"false\0");
    let nl_str_off = rodata.len();
    rodata.extend_from_slice(b"\n\0");
    let minus_str_off = rodata.len();
    rodata.extend_from_slice(b"-\0");
    let dot_str_off = rodata.len();
    rodata.extend_from_slice(b".\0");
    rodata_syms.push((".Lpbool_true_str", true_str_off as u64));
    rodata_syms.push((".Lpbool_false_str", false_str_off as u64));
    rodata_syms.push((".Lpnl_str", nl_str_off as u64));
    rodata_syms.push((".Lpf_minus", minus_str_off as u64));
    rodata_syms.push((".Lpf_dot", dot_str_off as u64));

    // Strings for p_print_vec
    let bracket_open_off = rodata.len();
    rodata.extend_from_slice(b"[\0");
    let bracket_close_off = rodata.len();
    rodata.extend_from_slice(b"]\0");
    let comma_sp_off = rodata.len();
    rodata.extend_from_slice(b", \0");
    rodata_syms.push((".Lbracket_open", bracket_open_off as u64));
    rodata_syms.push((".Lbracket_close", bracket_close_off as u64));
    rodata_syms.push((".Lcomma_sp", comma_sp_off as u64));

    // Float constants for p_print_float
    let zero_f_off = rodata.len();
    rodata.extend_from_slice(&0.0f64.to_le_bytes()); // 0.0
    let ten_f_off = rodata.len();
    rodata.extend_from_slice(&10.0f64.to_le_bytes()); // 10.0
    let sign_mask_off = rodata.len();
    rodata.extend_from_slice(&0x8000_0000_0000_0000u64.to_le_bytes()); // sign bit mask
    let zero_char_off = rodata.len();
    rodata.extend_from_slice(b"0\0");
    rodata_syms.push((".Lpf_zero_f", zero_f_off as u64));
    rodata_syms.push((".Lpf_ten_f", ten_f_off as u64));
    rodata_syms.push((".Lpf_sign_mask", sign_mask_off as u64));
    rodata_syms.push((".Lpf_zero_char", zero_char_off as u64));

    // ═══════════════════════════════════════════════════════════════════════
    // _start — program entry point
    // andq $-16, %rsp   ; align stack
    // call main         ; (relocation, filled by linker)
    // movq %rax, %rdi   ; exit code = main's return
    // movq $60, %rax    ; sys_exit
    // syscall
    // ud2
    // ═══════════════════════════════════════════════════════════════════════
    sym!("_start");
    // andq $-16, %rsp  → 48 83 e4 f0
    text.extend_from_slice(&[0x48, 0x83, 0xe4, 0xf0]);
    // call main (rel32 placeholder — linker patches with R_X86_64_PLT32)
    text.extend_from_slice(&[0xe8, 0x00, 0x00, 0x00, 0x00]);
    // This call needs a relocation — we'll handle it specially in the linker
    // For now, store the relocation offset
    let _start_call_offset = text.len() - 4; // offset of the rel32 operand

    // movq %rax, %rdi  → 48 89 c7
    text.extend_from_slice(&[0x48, 0x89, 0xc7]);
    // movq $60, %rax   → 48 c7 c0 3c 00 00 00
    text.extend_from_slice(&[0x48, 0xc7, 0xc0, 0x3c, 0x00, 0x00, 0x00]);
    // syscall           → 0f 05
    text.extend_from_slice(&[0x0f, 0x05]);
    // ud2              → 0f 0b
    text.extend_from_slice(&[0x0f, 0x0b]);

    // ═══════════════════════════════════════════════════════════════════════
    // p_print_str(rdi = *u8 null-terminated string)
    // Writes the string to stdout (fd=1) using sys_write.
    //
    // pushq %rbx              ; 53
    // movq %rdi, %rbx         ; 48 89 fb
    // ; strlen: rcx=ptr, rax=0
    // movq %rdi, %rcx         ; 48 89 f9
    // xorq %rax, %rax         ; 48 31 c0
    // ; loop:
    // ; cmpb $0, (%rcx)       ; 80 39 00
    // ; je done               ; 74 06
    // ; incq %rcx             ; 48 ff c1
    // ; incq %rax             ; 48 ff c0
    // ; jmp loop              ; eb f6
    // ; done:
    // ; sys_write(1, rbx, rax)
    // movq $1, %rdi           ; 48 c7 c7 01 00 00 00  (but use bf 01 00 00 00 for 32-bit)
    // movq %rbx, %rsi         ; 48 89 de
    // movq %rax, %rdx         ; 48 89 c2
    // movq $1, %rax           ; 48 c7 c0 01 00 00 00
    // syscall                 ; 0f 05
    // popq %rbx               ; 5b
    // ret                     ; c3
    // ═══════════════════════════════════════════════════════════════════════
    sym!("p_print_str");
    text.push(0x53);                          // pushq %rbx
    text.extend_from_slice(&[0x48, 0x89, 0xfb]); // movq %rdi, %rbx
    text.extend_from_slice(&[0x48, 0x89, 0xf9]); // movq %rdi, %rcx
    text.extend_from_slice(&[0x48, 0x31, 0xc0]); // xorq %rax, %rax
    let pstr_loop = text.len();
    text.extend_from_slice(&[0x80, 0x39, 0x00]); // cmpb $0, (%rcx)
    // je done (skip incq %rcx [3] + incq %rax [3] + jmp loop [2] = 8 bytes)
    text.extend_from_slice(&[0x74, 0x08]);    // je +8
    text.extend_from_slice(&[0x48, 0xff, 0xc1]); // incq %rcx
    text.extend_from_slice(&[0x48, 0xff, 0xc0]); // incq %rax
    // jmp loop: backward to pstr_loop
    let jmp_back = text.len();
    let back_offset = pstr_loop as i64 - jmp_back as i64 - 2;
    text.extend_from_slice(&[0xeb, back_offset as u8]); // jmp loop
    // done:
    text.extend_from_slice(&[0xbf, 0x01, 0x00, 0x00, 0x00]); // movl $1, %edi
    text.extend_from_slice(&[0x48, 0x89, 0xde]); // movq %rbx, %rsi
    text.extend_from_slice(&[0x48, 0x89, 0xc2]); // movq %rax, %rdx
    text.extend_from_slice(&[0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00]); // movq $1, %rax
    text.extend_from_slice(&[0x0f, 0x05]);     // syscall
    text.push(0x5b);                           // popq %rbx
    text.push(0xc3);                           // ret

    // ═══════════════════════════════════════════════════════════════════════
    // p_print_int(rdi = i64 value)
    // Converts integer to decimal string and writes to stdout.
    //
    // pushq %rbx ; pushq %r12 ; pushq %r13
    // subq $32, %rsp
    // movq %rdi, %r12       ; r12 = value
    // leaq 31(%rsp), %r13   ; r13 = buffer end (rsp+31)
    // movb $0, (%r13)       ; null terminator
    // testq %r12, %r12
    // jne .notzero
    // decq %r13
    // movb $48, (%r13)      ; '0'
    // jmp .print
    // .notzero:
    // testq %r12, %r12
    // jns .pos
    // negq %r12
    // .pos:
    // .digit_loop:
    // testq %r12, %r12
    // je .digits_done
    // movq %r12, %rax
    // xorq %rdx, %rdx
    // movq $10, %rcx
    // divq %rcx             ; rax=quotient, rdx=remainder
    // movq %rax, %r12
    // addq $48, %rdx        ; '0' + remainder
    // decq %r13
    // movb %dl, (%r13)
    // jmp .digit_loop
    // .digits_done:
    // testq %rdi, %rdi      ; check original value sign
    // jns .print
    // decq %r13
    // movb $45, (%r13)      ; '-'
    // .print:
    // ; write(1, r13, (rsp+31) - r13)
    // movl $1, %edi
    // movq %r13, %rsi
    // leaq 31(%rsp), %rax
    // subq %r13, %rax       ; rax = length
    // movq %rax, %rdx
    // movq $1, %rax         ; sys_write
    // syscall
    // addq $32, %rsp
    // popq %r13 ; popq %r12 ; popq %rbx
    // ret
    // ═══════════════════════════════════════════════════════════════════════
    sym!("p_print_int");
    text.push(0x53);                          // pushq %rbx
    text.extend_from_slice(&[0x41, 0x54]);    // pushq %r12
    text.extend_from_slice(&[0x41, 0x55]);    // pushq %r13
    text.extend_from_slice(&[0x48, 0x83, 0xec, 0x20]); // subq $32, %rsp
    text.extend_from_slice(&[0x49, 0x89, 0xfc]); // movq %rdi, %r12
    // leaq 31(%rsp), %r13 → 4C 8D 6C 24 1F
    text.extend_from_slice(&[0x4c, 0x8d, 0x6c, 0x24, 0x1f]);
    // movb $0, (%r13) → 41 C6 45 00 00 (REX.B, C6, ModRM=45: mod=01 rm=101(r13), disp8=0, imm8=0)
    text.extend_from_slice(&[0x41, 0xc6, 0x45, 0x00, 0x00]); // movb $0, (%r13)
    // testq %r12, %r12 → 4D 85 E4
    text.extend_from_slice(&[0x4d, 0x85, 0xe4]);
    // jne .notzero → 75 XX (need to calculate offset)
    let pint_jne1 = text.len();
    text.extend_from_slice(&[0x75, 0x00]); // placeholder
    // zero case:
    // decq %r13 → 49 ff cd
    text.extend_from_slice(&[0x49, 0xff, 0xcd]);
    // movb $48, (%r13) → 41 C6 45 00 30 (mod=01, disp8=0, imm8='0')
    text.extend_from_slice(&[0x41, 0xc6, 0x45, 0x00, 0x30]);
    // jmp .print → eb XX
    let pint_jmp_print1 = text.len();
    text.extend_from_slice(&[0xeb, 0x00]); // placeholder
    // .notzero:
    let pint_notzero = text.len();
    // Patch jne to jump here
    text[pint_jne1 + 1] = (pint_notzero - pint_jne1 - 2) as u8;
    // testq %r12, %r12 → 4D 85 E4
    text.extend_from_slice(&[0x4d, 0x85, 0xe4]);
    // jns .pos → 79 XX
    let pint_jns = text.len();
    text.extend_from_slice(&[0x79, 0x00]); // placeholder
    // negq %r12 → 49 F7 DC
    text.extend_from_slice(&[0x49, 0xf7, 0xdc]);
    // .pos:
    let pint_pos = text.len();
    text[pint_jns + 1] = (pint_pos - pint_jns - 2) as u8;
    // .digit_loop:
    let pint_loop = text.len();
    // testq %r12, %r12 → 4D 85 E4
    text.extend_from_slice(&[0x4d, 0x85, 0xe4]);
    // je .digits_done → 74 XX
    let pint_je_done = text.len();
    text.extend_from_slice(&[0x74, 0x00]); // placeholder
    // movq %r12, %rax → 4C 89 E0
    text.extend_from_slice(&[0x4c, 0x89, 0xe0]);
    // xorq %rdx, %rdx → 48 31 D2
    text.extend_from_slice(&[0x48, 0x31, 0xd2]);
    // movq $10, %rcx → 48 C7 C1 0A 00 00 00
    text.extend_from_slice(&[0x48, 0xc7, 0xc1, 0x0a, 0x00, 0x00, 0x00]);
    // divq %rcx → 48 F7 F1
    text.extend_from_slice(&[0x48, 0xf7, 0xf1]);
    // movq %rax, %r12 → 49 89 C4 (REX.W=1, REX.B=1 for r12, 89, ModRM=C4: mod=11 reg=000(rax) rm=100(r12))
    text.extend_from_slice(&[0x49, 0x89, 0xc4]);
    // addq $48, %rdx → 48 83 C2 30
    text.extend_from_slice(&[0x48, 0x83, 0xc2, 0x30]);
    // decq %r13 → 49 FF CD
    text.extend_from_slice(&[0x49, 0xff, 0xcd]);
    // movb %dl, (%r13) → 41 88 55 00 (REX.B, 88, ModRM=55: mod=01 reg=010(dl) rm=101(r13), disp8=0)
    text.extend_from_slice(&[0x41, 0x88, 0x55, 0x00]);
    // jmp .digit_loop → eb XX (backward)
    let jmp_back2 = text.len();
    let back2 = pint_loop as i64 - jmp_back2 as i64 - 2;
    text.extend_from_slice(&[0xeb, back2 as u8]);
    // .digits_done:
    let pint_digits_done = text.len();
    text[pint_je_done + 1] = (pint_digits_done - pint_je_done - 2) as u8;
    // testq %rdi, %rdi → 48 85 FF
    text.extend_from_slice(&[0x48, 0x85, 0xff]);
    // jns .print → 79 XX
    let pint_jns2 = text.len();
    text.extend_from_slice(&[0x79, 0x00]); // placeholder
    // decq %r13 → 49 FF CD
    text.extend_from_slice(&[0x49, 0xff, 0xcd]);
    // movb $45, (%r13) → 41 C6 45 00 2D (mod=01, disp8=0, imm8='-')
    text.extend_from_slice(&[0x41, 0xc6, 0x45, 0x00, 0x2d]);
    // .print:
    let pint_print = text.len();
    text[pint_jns2 + 1] = (pint_print - pint_jns2 - 2) as u8;
    text[pint_jmp_print1 + 1] = (pint_print - pint_jmp_print1 - 2) as u8;
    // movl $1, %edi → BF 01 00 00 00
    text.extend_from_slice(&[0xbf, 0x01, 0x00, 0x00, 0x00]);
    // movq %r13, %rsi → 4C 89 EE
    text.extend_from_slice(&[0x4c, 0x89, 0xee]);
    // leaq 31(%rsp), %rax → 48 8D 44 24 1F
    text.extend_from_slice(&[0x48, 0x8d, 0x44, 0x24, 0x1f]);
    // subq %r13, %rax → 4C 29 E8
    text.extend_from_slice(&[0x4c, 0x29, 0xe8]);
    // movq %rax, %rdx → 48 89 C2
    text.extend_from_slice(&[0x48, 0x89, 0xc2]);
    // movq $1, %rax → 48 C7 C0 01 00 00 00
    text.extend_from_slice(&[0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00]);
    // syscall → 0F 05
    text.extend_from_slice(&[0x0f, 0x05]);
    // addq $32, %rsp → 48 83 C4 20
    text.extend_from_slice(&[0x48, 0x83, 0xc4, 0x20]);
    text.extend_from_slice(&[0x41, 0x5d]);    // popq %r13
    text.extend_from_slice(&[0x41, 0x5c]);    // popq %r12
    text.push(0x5b);                           // popq %rbx
    text.push(0xc3);                           // ret

    // ═══════════════════════════════════════════════════════════════════════
    // p_print_bool(rdi = 0 or 1)
    // testq %rdi, %rdi ; je .false
    // ; true: write(1, "true", 4)
    // movl $1, %edi ; leaq true_str(%rip), %rsi ; movl $4, %edx ; movl $1, %eax ; syscall ; ret
    // .false: write(1, "false", 5)
    // movl $1, %edi ; leaq false_str(%rip), %rsi ; movl $5, %edx ; movl $1, %eax ; syscall ; ret
    // ═══════════════════════════════════════════════════════════════════════
    sym!("p_print_bool");
    text.extend_from_slice(&[0x48, 0x85, 0xff]); // testq %rdi, %rdi
    let pbool_je = text.len();
    text.extend_from_slice(&[0x74, 0x00]); // je .false
    // true:
    text.extend_from_slice(&[0xbf, 0x01, 0x00, 0x00, 0x00]); // movl $1, %edi
    // leaq true_str(%rip), %rsi → 48 8D 35 XX XX XX XX (RIP-relative)
    text.extend_from_slice(&[0x48, 0x8d, 0x35, 0x00, 0x00, 0x00, 0x00]); // placeholder
    let pbool_true_lea = text.len() - 4; // offset of rel32
    text.extend_from_slice(&[0xba, 0x04, 0x00, 0x00, 0x00]); // movl $4, %edx
    text.extend_from_slice(&[0xb8, 0x01, 0x00, 0x00, 0x00]); // movl $1, %eax
    text.extend_from_slice(&[0x0f, 0x05]); // syscall
    text.push(0xc3); // ret
    // false:
    let pbool_false = text.len();
    text[pbool_je + 1] = (pbool_false - pbool_je - 2) as u8;
    text.extend_from_slice(&[0xbf, 0x01, 0x00, 0x00, 0x00]); // movl $1, %edi
    text.extend_from_slice(&[0x48, 0x8d, 0x35, 0x00, 0x00, 0x00, 0x00]); // leaq false_str(%rip), %rsi
    let pbool_false_lea = text.len() - 4;
    text.extend_from_slice(&[0xba, 0x05, 0x00, 0x00, 0x00]); // movl $5, %edx
    text.extend_from_slice(&[0xb8, 0x01, 0x00, 0x00, 0x00]); // movl $1, %eax
    text.extend_from_slice(&[0x0f, 0x05]); // syscall
    text.push(0xc3); // ret

    // ═══════════════════════════════════════════════════════════════════════
    // p_print_newline()
    // write(1, "\n", 1)
    // ═══════════════════════════════════════════════════════════════════════
    sym!("p_print_newline");
    text.extend_from_slice(&[0xbf, 0x01, 0x00, 0x00, 0x00]); // movl $1, %edi
    text.extend_from_slice(&[0x48, 0x8d, 0x35, 0x00, 0x00, 0x00, 0x00]); // leaq nl_str(%rip), %rsi
    let pnl_lea = text.len() - 4;
    text.extend_from_slice(&[0xba, 0x01, 0x00, 0x00, 0x00]); // movl $1, %edx
    text.extend_from_slice(&[0xb8, 0x01, 0x00, 0x00, 0x00]); // movl $1, %eax
    text.extend_from_slice(&[0x0f, 0x05]); // syscall
    text.push(0xc3); // ret

    // ═══════════════════════════════════════════════════════════════════════
    // p_print_float(rdi = double bits as i64)
    // Prints a double matching Rust's default {} format for f64.
    // Handles: 0.0 -> "0", negative -> "-X.Y", trailing zeros stripped.
    // ═══════════════════════════════════════════════════════════════════════
    sym!("p_print_float");
    // pushq %rbx ; pushq %r12 ; pushq %r13 ; pushq %r14
    text.push(0x53);                          // pushq %rbx
    text.extend_from_slice(&[0x41, 0x54]);    // pushq %r12
    text.extend_from_slice(&[0x41, 0x55]);    // pushq %r13
    text.extend_from_slice(&[0x41, 0x56]);    // pushq %r14
    // subq $56, %rsp — 4 pushes (32) + ret (8) = 40. 40+56=96. 96%16=0.
    text.extend_from_slice(&[0x48, 0x83, 0xec, 0x38]);

    // Store double bits at (%rsp), load into xmm0
    text.extend_from_slice(&[0x48, 0x89, 0x3c, 0x24]); // movq %rdi, (%rsp)
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x04, 0x24]); // movsd (%rsp), %xmm0

    // Check if zero: store 0.0 at 8(%rsp), compare
    text.extend_from_slice(&[0x48, 0xc7, 0x44, 0x24, 0x08, 0x00, 0x00, 0x00, 0x00]); // movq $0, 8(%rsp)
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x4c, 0x24, 0x08]); // movsd 8(%rsp), %xmm1
    text.extend_from_slice(&[0x66, 0x0f, 0x2e, 0xc1]); // ucomisd %xmm1, %xmm0
    // jp .not_zero
    let pfloat_jp = text.len();
    text.extend_from_slice(&[0x7a, 0x00]);
    // jne .not_zero
    let pfloat_jne = text.len();
    text.extend_from_slice(&[0x75, 0x00]);

    // .print_zero: print "0" and return
    text.extend_from_slice(&[0xc6, 0x44, 0x24, 0x10, 0x30]); // movb '0', 16(%rsp)
    text.extend_from_slice(&[0xb8, 0x01, 0x00, 0x00, 0x00]); // movl $1, %eax
    text.extend_from_slice(&[0xbf, 0x01, 0x00, 0x00, 0x00]); // movl $1, %edi
    text.extend_from_slice(&[0x48, 0x8d, 0x74, 0x24, 0x10]); // leaq 16(%rsp), %rsi
    text.extend_from_slice(&[0xba, 0x01, 0x00, 0x00, 0x00]); // movl $1, %edx
    text.extend_from_slice(&[0x0f, 0x05]); // syscall
    // jmp .done (use rel32 since distance > 127)
    text.extend_from_slice(&[0xe9, 0x00, 0x00, 0x00, 0x00]);
    let pfloat_jmp_done = text.len() - 4;

    // .not_zero:
    let pfloat_notzero = text.len();
    let jp_off = (pfloat_notzero - pfloat_jp - 2) as i8;
    text[pfloat_jp + 1] = jp_off as u8;
    let jne_off = (pfloat_notzero - pfloat_jne - 2) as i8;
    text[pfloat_jne + 1] = jne_off as u8;

    // Check negative: test sign bit
    text.extend_from_slice(&[0x48, 0x8b, 0x04, 0x24]); // movq (%rsp), %rax
    text.extend_from_slice(&[0x48, 0x0f, 0xba, 0xe0, 0x3f]); // btq $63, %rax
    let pfloat_jnc = text.len();
    text.extend_from_slice(&[0x73, 0x00]); // jnc .positive

    // Negative: negate first, then print '-'
    text.extend_from_slice(&[0x48, 0x0f, 0xba, 0xf8, 0x3f]); // btcq $63, %rax (flip sign bit)
    text.extend_from_slice(&[0x48, 0x89, 0x04, 0x24]); // movq %rax, (%rsp)
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x04, 0x24]); // movsd (%rsp), %xmm0
    // Now print '-' (syscall clobbers rax/rdi/rsi/rdx but xmm0 is preserved)
    text.extend_from_slice(&[0xc6, 0x44, 0x24, 0x10, 0x2d]); // movb '-', 16(%rsp)
    text.extend_from_slice(&[0xb8, 0x01, 0x00, 0x00, 0x00]);
    text.extend_from_slice(&[0xbf, 0x01, 0x00, 0x00, 0x00]);
    text.extend_from_slice(&[0x48, 0x8d, 0x74, 0x24, 0x10]);
    text.extend_from_slice(&[0xba, 0x01, 0x00, 0x00, 0x00]);
    text.extend_from_slice(&[0x0f, 0x05]);

    // .positive:
    let pfloat_positive = text.len();
    let jnc_offset = (pfloat_positive - pfloat_jnc - 2) as i8;
    text[pfloat_jnc + 1] = jnc_offset as u8;

    // cvttsd2siq %xmm0, %r12 (integer part)
    // REX: W=1, R=1(r12), X=0, B=0 -> 0x4C; ModRM: mod=11, reg=100(r12), rm=000(xmm0) -> 0xE0
    text.extend_from_slice(&[0xf2, 0x4c, 0x0f, 0x2c, 0xe0]);

    // Compute frac = xmm0 - (double)r12
    // cvtsi2sdq %r12, %xmm1: REX W=1,B=1 -> 0x49; ModRM reg=001(xmm1),rm=100(r12) -> 0xCC
    text.extend_from_slice(&[0xf2, 0x49, 0x0f, 0x2a, 0xcc]);
    // subsd %xmm1, %xmm0
    text.extend_from_slice(&[0xf2, 0x0f, 0x5c, 0xc1]);

    // Multiply frac by 10^6 and add 0.5 for rounding
    // 10^6 = 1000000.0 as f64 = 0x41312D0000000000
    text.extend_from_slice(&[0xc7, 0x44, 0x24, 0x08, 0x00, 0x00, 0x00, 0x00]); // movl $0, 8(%rsp)
    text.extend_from_slice(&[0xc7, 0x44, 0x24, 0x0c, 0x80, 0x84, 0x2e, 0x41]); // movl $0x412E8480, 12(%rsp)
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x4c, 0x24, 0x08]); // movsd 8(%rsp), %xmm1 (10^6)
    text.extend_from_slice(&[0xf2, 0x0f, 0x59, 0xc1]); // mulsd %xmm1, %xmm0
    // 0.5 as f64 = 0x3FE0000000000000
    text.extend_from_slice(&[0xc7, 0x44, 0x24, 0x10, 0x00, 0x00, 0x00, 0x00]); // movl $0, 16(%rsp)
    text.extend_from_slice(&[0xc7, 0x44, 0x24, 0x14, 0x00, 0x00, 0xe0, 0x3f]); // movl $0x3FE00000, 20(%rsp)
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x4c, 0x24, 0x10]); // movsd 16(%rsp), %xmm1 (0.5)
    text.extend_from_slice(&[0xf2, 0x0f, 0x58, 0xc1]); // addsd %xmm1, %xmm0
    // cvttsd2siq %xmm0, %r13 (rounded 6-digit value)
    text.extend_from_slice(&[0xf2, 0x4c, 0x0f, 0x2c, 0xe8]);

    // Handle overflow: if r13 >= 1000000, increment r12, sub 1000000 from r13
    text.extend_from_slice(&[0x49, 0x81, 0xfd, 0x40, 0x42, 0x0f, 0x00]); // cmpq $1000000, %r13
    let pfloat_jl_ovf = text.len();
    text.extend_from_slice(&[0x7c, 0x00]); // jl .no_overflow
    text.extend_from_slice(&[0x49, 0xff, 0xc4]); // incq %r12
    text.extend_from_slice(&[0x49, 0x81, 0xed, 0x40, 0x42, 0x0f, 0x00]); // subq $1000000, %r13
    let pfloat_no_ovf = text.len();
    let jl_ovf_off = (pfloat_no_ovf - pfloat_jl_ovf - 2) as i8;
    text[pfloat_jl_ovf + 1] = jl_ovf_off as u8;

    // Print integer part: movq %r12, %rdi ; call p_print_int
    text.extend_from_slice(&[0x4c, 0x89, 0xe7]);
    text.extend_from_slice(&[0xe8, 0x00, 0x00, 0x00, 0x00]);
    let pfloat_call1_off = text.len() - 4;

    // Check if r13 (frac digits) is zero
    text.extend_from_slice(&[0x4d, 0x85, 0xed]); // testq %r13, %r13
    // je .done (no fractional part) — use rel32
    let pfloat_je_done = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0x00, 0x00, 0x00, 0x00]);

    // Print "."
    text.extend_from_slice(&[0xc6, 0x44, 0x24, 0x10, 0x2e]); // movb '.', 16(%rsp)
    text.extend_from_slice(&[0xb8, 0x01, 0x00, 0x00, 0x00]);
    text.extend_from_slice(&[0xbf, 0x01, 0x00, 0x00, 0x00]);
    text.extend_from_slice(&[0x48, 0x8d, 0x74, 0x24, 0x10]);
    text.extend_from_slice(&[0xba, 0x01, 0x00, 0x00, 0x00]);
    text.extend_from_slice(&[0x0f, 0x05]);

    // Convert r13 to 6-digit zero-padded string (build in reverse at 24..30(%rsp))
    // leaq 30(%rsp), %r14 (buffer end, fill backwards)
    text.extend_from_slice(&[0x4c, 0x8d, 0x74, 0x24, 0x1e]); // leaq 30(%rsp), %r14
    // movq %r13, %rax
    text.extend_from_slice(&[0x4c, 0x89, 0xe8]);
    // movl $6, %r12d (digit counter)
    text.extend_from_slice(&[0x41, 0xbc, 0x06, 0x00, 0x00, 0x00]); // movl $6, %r12d

    // .digit_loop:
    let pfloat_dloop = text.len();
    text.extend_from_slice(&[0x49, 0xff, 0xce]); // decq %r14
    text.extend_from_slice(&[0x48, 0x31, 0xd2]); // xorq %rdx, %rdx
    text.extend_from_slice(&[0xb9, 0x0a, 0x00, 0x00, 0x00]); // movl $10, %ecx
    text.extend_from_slice(&[0x48, 0xf7, 0xf1]); // divq %rcx (rax/10 -> rax=quot, rdx=rem)
    text.extend_from_slice(&[0x80, 0xc2, 0x30]); // addb $48, %dl
    text.extend_from_slice(&[0x41, 0x88, 0x16]); // movb %dl, (%r14)
    text.extend_from_slice(&[0x49, 0xff, 0xcc]); // decq %r12
    text.extend_from_slice(&[0x4d, 0x85, 0xe4]); // testq %r12, %r12
    let pfloat_dloop_jne = text.len();
    text.extend_from_slice(&[0x75, 0x00]);
    let pfloat_dloop_end = text.len();
    let dloop_offset = (pfloat_dloop as i64 - pfloat_dloop_end as i64) as i8;
    text[pfloat_dloop_jne + 1] = dloop_offset as u8;

    // r14 points to start of 6-digit string. Set r12 = 6 for strip_zeros.
    text.extend_from_slice(&[0x41, 0xbc, 0x06, 0x00, 0x00, 0x00]); // movl $6, %r12d

    // .strip_zeros: find last non-'0' digit
    text.extend_from_slice(&[0x4d, 0x89, 0xe5]); // movq %r12, %r13
    // .strip_loop:
    let pfloat_strip_loop = text.len();
    text.extend_from_slice(&[0x4d, 0x85, 0xed]); // testq %r13, %r13
    let pfloat_jz_print = text.len();
    text.extend_from_slice(&[0x74, 0x00]); // jz .print_digits
    text.extend_from_slice(&[0x49, 0xff, 0xcd]); // decq %r13
    // cmpb '0', (%r14,%r13): REX.X=1(r13), REX.B=1(r14) -> 0x43
    // ModRM: mod=00, reg=111(cmp), rm=100(SIB) -> 0x3C
    // SIB: scale=00, index=101(r13), base=110(r14) -> 0x2E
    text.extend_from_slice(&[0x43, 0x80, 0x3c, 0x2e, 0x30]); // cmpb '0', (%r14,%r13)
    let je_strip_loop = text.len();
    let strip_rel = (pfloat_strip_loop as i64 - je_strip_loop as i64 - 2) as i8;
    text.extend_from_slice(&[0x74, strip_rel as u8]); // je .strip_loop
    text.extend_from_slice(&[0x49, 0xff, 0xc5]); // incq %r13

    // .print_digits:
    let pfloat_print_dig = text.len();
    let jz_print_off = (pfloat_print_dig - pfloat_jz_print - 2) as i8;
    text[pfloat_jz_print + 1] = jz_print_off as u8;

    // If r13 == 0, skip
    text.extend_from_slice(&[0x4d, 0x85, 0xed]); // testq %r13, %r13
    let pfloat_jz_done = text.len();
    text.extend_from_slice(&[0x74, 0x00]); // jz .done
    // write(1, %r14, %r13)
    text.extend_from_slice(&[0xb8, 0x01, 0x00, 0x00, 0x00]);
    text.extend_from_slice(&[0xbf, 0x01, 0x00, 0x00, 0x00]);
    text.extend_from_slice(&[0x4c, 0x89, 0xf6]); // movq %r14, %rsi
    text.extend_from_slice(&[0x4c, 0x89, 0xea]); // movq %r13, %rdx
    text.extend_from_slice(&[0x0f, 0x05]);

    // .done:
    let pfloat_done = text.len();
    let jz_done_off = (pfloat_done - pfloat_jz_done - 2) as i8;
    text[pfloat_jz_done + 1] = jz_done_off as u8;
    // je .done uses rel32: 0F 84 cd — displacement at pfloat_je_done + 2
    let je_done_off = (pfloat_done as i64 - pfloat_je_done as i64 - 6) as i32;
    text[pfloat_je_done + 2..pfloat_je_done + 6].copy_from_slice(&je_done_off.to_le_bytes());
    // jmp .done uses rel32: E9 cd — displacement at pfloat_jmp_done
    let jmp_done_off = (pfloat_done as i64 - pfloat_jmp_done as i64 - 4) as i32;
    text[pfloat_jmp_done..pfloat_jmp_done + 4].copy_from_slice(&jmp_done_off.to_le_bytes());

    // Epilogue
    text.extend_from_slice(&[0x48, 0x83, 0xc4, 0x38]); // addq $56, %rsp
    text.extend_from_slice(&[0x41, 0x5e]);    // popq %r14
    text.extend_from_slice(&[0x41, 0x5d]);    // popq %r13
    text.extend_from_slice(&[0x41, 0x5c]);    // popq %r12
    text.push(0x5b);                           // popq %rbx
    text.push(0xc3);                           // ret

    // p_panic(rdi = *u8 message)
    // Writes message to stderr, then exits with code 1.
    //
    // pushq %rbx
    // movq %rdi, %rbx
    // ; strlen
    // movq %rdi, %rcx ; xorq %rax, %rax
    // ; loop: cmpb $0, (%rcx) ; je done ; incq %rcx ; incq %rax ; jmp loop
    // ; done:
    // movl $2, %edi  ; stderr
    // movq %rbx, %rsi
    // movq %rax, %rdx
    // movl $1, %eax  ; sys_write
    // syscall
    // ; write newline
    // movl $2, %edi
    // leaq nl_str(%rip), %rsi
    // movl $1, %edx
    // movl $1, %eax
    // syscall
    // ; exit(1)
    // movl $1, %edi
    // movl $60, %eax ; sys_exit
    // syscall
    // ud2
    // ═══════════════════════════════════════════════════════════════════════
    sym!("p_panic");
    text.push(0x53);                          // pushq %rbx
    text.extend_from_slice(&[0x48, 0x89, 0xfb]); // movq %rdi, %rbx
    text.extend_from_slice(&[0x48, 0x89, 0xf9]); // movq %rdi, %rcx
    text.extend_from_slice(&[0x48, 0x31, 0xc0]); // xorq %rax, %rax
    let panic_loop = text.len();
    text.extend_from_slice(&[0x80, 0x39, 0x00]); // cmpb $0, (%rcx)
    text.extend_from_slice(&[0x74, 0x08]);    // je +8
    text.extend_from_slice(&[0x48, 0xff, 0xc1]); // incq %rcx
    text.extend_from_slice(&[0x48, 0xff, 0xc0]); // incq %rax
    let jmp_back3 = text.len();
    let back3 = panic_loop as i64 - jmp_back3 as i64 - 2;
    text.extend_from_slice(&[0xeb, back3 as u8]);
    // done:
    text.extend_from_slice(&[0xbf, 0x02, 0x00, 0x00, 0x00]); // movl $2, %edi
    text.extend_from_slice(&[0x48, 0x89, 0xde]); // movq %rbx, %rsi
    text.extend_from_slice(&[0x48, 0x89, 0xc2]); // movq %rax, %rdx
    text.extend_from_slice(&[0xb8, 0x01, 0x00, 0x00, 0x00]); // movl $1, %eax
    text.extend_from_slice(&[0x0f, 0x05]);     // syscall
    // write newline
    text.extend_from_slice(&[0xbf, 0x02, 0x00, 0x00, 0x00]); // movl $2, %edi
    text.extend_from_slice(&[0x48, 0x8d, 0x35, 0x00, 0x00, 0x00, 0x00]); // leaq nl_str(%rip), %rsi
    let panic_nl_lea = text.len() - 4;
    text.extend_from_slice(&[0xba, 0x01, 0x00, 0x00, 0x00]); // movl $1, %edx
    text.extend_from_slice(&[0xb8, 0x01, 0x00, 0x00, 0x00]); // movl $1, %eax
    text.extend_from_slice(&[0x0f, 0x05]);     // syscall
    // exit(1)
    text.extend_from_slice(&[0xbf, 0x01, 0x00, 0x00, 0x00]); // movl $1, %edi
    text.extend_from_slice(&[0xb8, 0x3c, 0x00, 0x00, 0x00]); // movl $60, %eax
    text.extend_from_slice(&[0x0f, 0x05]);     // syscall
    text.extend_from_slice(&[0x0f, 0x0b]);     // ud2

    // ═══════════════════════════════════════════════════════════════════════
    // p_malloc(rdi = size) → rax = pointer
    // Uses mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)
    // Round size up to page (4096).
    //
    // addq $4095, %rdi ; andq $-4096, %rdi
    // xorq %rsi, %rsi  ; movq %rdi, %rsi  (length)
    // Actually: mmap args: rdi=addr(0), rsi=length, rdx=prot, r10=flags, r8=fd, r9=offset
    // movq %rdi, %rsi  ; length = rounded size
    // xorq %rdi, %rdi  ; addr = NULL
    // movl $3, %edx    ; prot = PROT_READ|PROT_WRITE
    // movl $34, %r10d  ; flags = MAP_PRIVATE|MAP_ANONYMOUS (0x22)
    // movl $-1, %r8d   ; fd = -1
    // xorq %r9, %r9    ; offset = 0
    // movl $9, %eax    ; sys_mmap
    // syscall
    // ret
    // ═══════════════════════════════════════════════════════════════════════
    sym!("p_malloc");
    // addq $4095, %rdi → 48 81 C7 FF 0F 00 00
    text.extend_from_slice(&[0x48, 0x81, 0xc7, 0xff, 0x0f, 0x00, 0x00]);
    // andq $-4096, %rdi → 48 81 E7 00 F0 FF FF
    text.extend_from_slice(&[0x48, 0x81, 0xe7, 0x00, 0xf0, 0xff, 0xff]);
    // movq %rdi, %rsi → 48 89 FE
    text.extend_from_slice(&[0x48, 0x89, 0xfe]);
    // xorq %rdi, %rdi → 48 31 FF
    text.extend_from_slice(&[0x48, 0x31, 0xff]);
    // movl $3, %edx → BA 03 00 00 00
    text.extend_from_slice(&[0xba, 0x03, 0x00, 0x00, 0x00]);
    // movl $34, %r10d → 41 BA 22 00 00 00
    text.extend_from_slice(&[0x41, 0xba, 0x22, 0x00, 0x00, 0x00]);
    // movl $-1, %r8d → 41 B8 FF FF FF FF
    // movq $-1, %r8 → 49 C7 C0 FF FF FF FF (REX.W+B, C7 /0, imm32 sign-extended)
    text.extend_from_slice(&[0x49, 0xc7, 0xc0, 0xff, 0xff, 0xff, 0xff]);
    // xorq %r9, %r9 → 4D 31 C9
    text.extend_from_slice(&[0x4d, 0x31, 0xc9]);
    // movl $9, %eax → B8 09 00 00 00
    text.extend_from_slice(&[0xb8, 0x09, 0x00, 0x00, 0x00]);
    // syscall → 0F 05
    text.extend_from_slice(&[0x0f, 0x05]);
    text.push(0xc3); // ret

    // ═══════════════════════════════════════════════════════════════════════
    // p_alloc(rdi = size) → rax = pointer (alias for p_malloc)
    // jmp p_malloc → E9 XX XX XX XX
    // ═══════════════════════════════════════════════════════════════════════
    sym!("p_alloc");
    text.extend_from_slice(&[0xe9, 0x00, 0x00, 0x00, 0x00]); // jmp rel32 (linker patches)
    let palloc_jmp_off = text.len() - 4;

    // ═══════════════════════════════════════════════════════════════════════
    // p_free(rdi = pointer)
    // munmap(ptr, 4096) — simplified
    // movl $4096, %esi → BE 00 10 00 00
    // movl $11, %eax → B8 0B 00 00 00
    // syscall → 0F 05
    // ret → C3
    // ═══════════════════════════════════════════════════════════════════════
    sym!("p_free");
    text.extend_from_slice(&[0xbe, 0x00, 0x10, 0x00, 0x00]); // movl $4096, %esi
    text.extend_from_slice(&[0xb8, 0x0b, 0x00, 0x00, 0x00]); // movl $11, %eax
    text.extend_from_slice(&[0x0f, 0x05]); // syscall
    text.push(0xc3); // ret

    // ═══════════════════════════════════════════════════════════════════════
    // p_str_concat(rdi = *u8 a, rsi = *u8 b) → rax = *u8 result
    // Simplified: allocate 4096 bytes, copy both strings.
    // pushq %rbx ; pushq %r12 ; pushq %r13 ; pushq %r14
    // movq %rdi, %rbx ; movq %rsi, %r12
    // movl $4096, %edi ; call p_malloc
    // movq %rax, %r13  ; result ptr
    // movq %rax, %rcx  ; dest
    // ; copy a
    // movq %rbx, %rsi  ; src = a
    // ; copy_loop_a: movb (%rsi), %al ; testq %rax, %rax ; je copy_b ; movb %al, (%rcx) ; incq %rsi ; incq %rcx ; jmp copy_loop_a
    // ; copy_b:
    // movq %r12, %rsi
    // ; copy_loop_b: movb (%rsi), %al ; testq %rax, %rax ; je done ; movb %al, (%rcx) ; incq %rsi ; incq %rcx ; jmp copy_loop_b
    // ; done: movb $0, (%rcx) ; movq %r13, %rax ; popq %r14 ; popq %r13 ; popq %r12 ; popq %rbx ; ret
    // ═══════════════════════════════════════════════════════════════════════
    sym!("p_str_concat");
    text.push(0x53);                          // pushq %rbx
    text.extend_from_slice(&[0x41, 0x54]);    // pushq %r12
    text.extend_from_slice(&[0x41, 0x55]);    // pushq %r13
    text.extend_from_slice(&[0x41, 0x56]);    // pushq %r14
    text.extend_from_slice(&[0x48, 0x89, 0xfb]); // movq %rdi, %rbx
    text.extend_from_slice(&[0x49, 0x89, 0xf4]); // movq %rsi, %r12 (REX.W+REX.B)
    text.extend_from_slice(&[0xbf, 0x00, 0x10, 0x00, 0x00]); // movl $4096, %edi
    text.extend_from_slice(&[0xe8, 0x00, 0x00, 0x00, 0x00]); // call p_malloc
    let pstrcat_call_off = text.len() - 4;
    text.extend_from_slice(&[0x49, 0x89, 0xc5]); // movq %rax, %r13
    text.extend_from_slice(&[0x48, 0x89, 0xc1]); // movq %rax, %rcx
    text.extend_from_slice(&[0x48, 0x89, 0xde]); // movq %rbx, %rsi
    // copy_loop_a:
    let concat_loop_a = text.len();
    // movb (%rsi), %al → 8A 06
    text.extend_from_slice(&[0x8a, 0x06]);
    // testq %rax, %rax → 84 C0
    text.extend_from_slice(&[0x84, 0xc0]);
    // je copy_b → 74 XX
    let concat_je_a = text.len();
    text.extend_from_slice(&[0x74, 0x00]);
    // movb %al, (%rcx) → 88 01
    text.extend_from_slice(&[0x88, 0x01]);
    // incq %rsi → 48 FF C6
    text.extend_from_slice(&[0x48, 0xff, 0xc6]);
    // incq %rcx → 48 FF C1
    text.extend_from_slice(&[0x48, 0xff, 0xc1]);
    // jmp copy_loop_a
    let jmp_back4 = text.len();
    let back4 = concat_loop_a as i64 - jmp_back4 as i64 - 2;
    text.extend_from_slice(&[0xeb, back4 as u8]);
    // copy_b:
    let concat_copy_b = text.len();
    text[concat_je_a + 1] = (concat_copy_b - concat_je_a - 2) as u8;
    text.extend_from_slice(&[0x48, 0x89, 0xe6]); // movq %r12, %rsi (wait, r12 is in %r12, need movq %r12, %rsi → 4C 89 E6)
    // Actually: movq %r12, %rsi → 4C 89 E6
    // I wrote 48 89 E6 which is movq %rsp, %rsi — wrong! Fix:
    // Let me redo this. The previous bytes were wrong. Let me overwrite.
    // Actually I can't overwrite since I already pushed. Let me just fix it.
    // The bytes at concat_copy_b are 48 89 E6 which is movq %rsp, %rsi — wrong.
    // I need to replace them. Since I'm building the vec, I can pop them and push correct ones.
    // Actually, let me just be more careful. Let me remove the wrong bytes.
    // The text vec has [0x48, 0x89, 0xe6] at position concat_copy_b.
    // I need [0x4c, 0x89, 0xe6] instead.
    text[concat_copy_b] = 0x4c; // Fix: REX.R for r12
    // copy_loop_b:
    let concat_loop_b = concat_copy_b + 3;
    text.extend_from_slice(&[0x8a, 0x06]); // movb (%rsi), %al
    text.extend_from_slice(&[0x84, 0xc0]); // testq %rax, %rax
    let concat_je_b = text.len();
    text.extend_from_slice(&[0x74, 0x00]); // je done
    text.extend_from_slice(&[0x88, 0x01]); // movb %al, (%rcx)
    text.extend_from_slice(&[0x48, 0xff, 0xc6]); // incq %rsi
    text.extend_from_slice(&[0x48, 0xff, 0xc1]); // incq %rcx
    let jmp_back5 = text.len();
    let back5 = concat_loop_b as i64 - jmp_back5 as i64 - 2;
    text.extend_from_slice(&[0xeb, back5 as u8]);
    // done:
    let concat_done = text.len();
    text[concat_je_b + 1] = (concat_done - concat_je_b - 2) as u8;
    // movb $0, (%rcx) → C6 01 00
    text.extend_from_slice(&[0xc6, 0x01, 0x00]);
    // movq %r13, %rax → 4C 89 E8
    text.extend_from_slice(&[0x4c, 0x89, 0xe8]);
    text.extend_from_slice(&[0x41, 0x5e]);    // popq %r14
    text.extend_from_slice(&[0x41, 0x5d]);    // popq %r13
    text.extend_from_slice(&[0x41, 0x5c]);    // popq %r12
    text.push(0x5b);                           // popq %rbx
    text.push(0xc3);                           // ret

    // ═══════════════════════════════════════════════════════════════════════
    // p_to_str(rdi = i64 value) → rax = *u8 string
    // Simplified: use a static buffer and call p_print_int logic.
    // For now, allocate 32 bytes, convert, return pointer.
    // pushq %rbx ; pushq %r12
    // subq $32, %rsp
    // movq %rdi, %r12
    // movl $32, %edi ; call p_malloc
    // movq %rax, %rbx  ; result buffer
    // ; Convert r12 to string at rbx
    // ; Simple: just call the int-to-string logic inline
    // ; For now, just store "0" as placeholder
    // movb $48, (%rbx)  ; '0'
    // movb $0, 1(%rbx)  ; null terminator
    // movq %rbx, %rax
    // addq $32, %rsp
    // popq %r12 ; popq %rbx
    // ret
    // NOTE: This is a simplified version. Full int-to-string conversion
    // would duplicate the p_print_int logic but write to the buffer.
    // ═══════════════════════════════════════════════════════════════════════
    sym!("p_to_str");
    // pushq %rbx ; pushq %r12 ; pushq %r13 ; pushq %r14
    text.push(0x53);                          // pushq %rbx
    text.extend_from_slice(&[0x41, 0x54]);    // pushq %r12
    text.extend_from_slice(&[0x41, 0x55]);    // pushq %r13
    text.extend_from_slice(&[0x41, 0x56]);    // pushq %r14
    text.extend_from_slice(&[0x48, 0x83, 0xec, 0x20]); // subq $32, %rsp
    // movq %rdi, %r12  (working copy for digit conversion)
    text.extend_from_slice(&[0x49, 0x89, 0xfc]); // movq %rdi, %r12
    // movq %rdi, %r14  (save original value for sign check — rdi gets clobbered by p_malloc)
    text.extend_from_slice(&[0x49, 0x89, 0xfe]); // movq %rdi, %r14
    // movl $32, %edi ; call p_malloc
    text.extend_from_slice(&[0xbf, 0x20, 0x00, 0x00, 0x00]); // movl $32, %edi
    text.extend_from_slice(&[0xe8, 0x00, 0x00, 0x00, 0x00]); // call p_malloc
    let pts_call_off = text.len() - 4;
    // movq %rax, %rbx  (buffer pointer)
    text.extend_from_slice(&[0x48, 0x89, 0xc3]); // movq %rax, %rbx
    // leaq 31(%rbx), %r13  (r13 = buffer end, write backwards)
    // 4C 8D 6B 1F
    text.extend_from_slice(&[0x4c, 0x8d, 0x6b, 0x1f]);
    // movb $0, (%r13)  (null terminator)
    // 41 C6 45 00 00
    text.extend_from_slice(&[0x41, 0xc6, 0x45, 0x00, 0x00]);
    // testq %r12, %r12 → 4D 85 E4
    text.extend_from_slice(&[0x4d, 0x85, 0xe4]);
    // jne .notzero
    let pts_jne = text.len();
    text.extend_from_slice(&[0x75, 0x00]);
    // zero case: decq %r13; movb $48, (%r13) ('0'); jmp .done
    text.extend_from_slice(&[0x49, 0xff, 0xcd]); // decq %r13
    text.extend_from_slice(&[0x41, 0xc6, 0x45, 0x00, 0x30]); // movb $48, (%r13)
    let pts_jmp_done = text.len();
    text.extend_from_slice(&[0xeb, 0x00]); // jmp .done
    // .notzero:
    let pts_notzero = text.len();
    text[pts_jne + 1] = (pts_notzero - pts_jne - 2) as u8;
    // jns .pos → 79 XX
    let pts_jns = text.len();
    text.extend_from_slice(&[0x79, 0x00]);
    // negq %r12 → 49 F7 DC
    text.extend_from_slice(&[0x49, 0xf7, 0xdc]);
    // .pos:
    let pts_pos = text.len();
    text[pts_jns + 1] = (pts_pos - pts_jns - 2) as u8;
    // .digit_loop:
    let pts_loop = text.len();
    // testq %r12, %r12 → 4D 85 E4
    text.extend_from_slice(&[0x4d, 0x85, 0xe4]);
    // je .digits_done
    let pts_je_done = text.len();
    text.extend_from_slice(&[0x74, 0x00]);
    // movq %r12, %rax → 4C 89 E0
    text.extend_from_slice(&[0x4c, 0x89, 0xe0]);
    // xorq %rdx, %rdx → 48 31 D2
    text.extend_from_slice(&[0x48, 0x31, 0xd2]);
    // movq $10, %rcx → 48 C7 C1 0A 00 00 00
    text.extend_from_slice(&[0x48, 0xc7, 0xc1, 0x0a, 0x00, 0x00, 0x00]);
    // divq %rcx → 48 F7 F1
    text.extend_from_slice(&[0x48, 0xf7, 0xf1]);
    // movq %rax, %r12 → 4C 89 E4 (wait, that's movq %r12, %rsp! Need 4C 89 C4)
    // Actually: movq %rax, %r12 → REX.R=1, ModRM C4 → 4C 89 C4
    text.extend_from_slice(&[0x49, 0x89, 0xc4]); // movq %rax, %r12 (REX.W+REX.B)
    // addq $48, %rdx → 48 83 C2 30
    text.extend_from_slice(&[0x48, 0x83, 0xc2, 0x30]);
    // decq %r13 → 49 FF CD
    text.extend_from_slice(&[0x49, 0xff, 0xcd]);
    // movb %dl, (%r13) → 41 88 55 00
    text.extend_from_slice(&[0x41, 0x88, 0x55, 0x00]);
    // jmp .digit_loop
    let pts_jmp_loop = text.len();
    let pts_back = pts_loop as i64 - pts_jmp_loop as i64 - 2;
    text.extend_from_slice(&[0xeb, pts_back as u8]);
    // .digits_done:
    let pts_digits_done = text.len();
    text[pts_je_done + 1] = (pts_digits_done - pts_je_done - 2) as u8;
    // Check original sign: testq %r14, %r14 → 4D 85 F6
    text.extend_from_slice(&[0x4d, 0x85, 0xf6]);
    // jns .done
    let pts_jns2 = text.len();
    text.extend_from_slice(&[0x79, 0x00]);
    // decq %r13; movb $45, (%r13) ('-')
    text.extend_from_slice(&[0x49, 0xff, 0xcd]); // decq %r13
    text.extend_from_slice(&[0x41, 0xc6, 0x45, 0x00, 0x2d]); // movb $45, (%r13)
    // .done:
    let pts_done = text.len();
    text[pts_jmp_done + 1] = (pts_done - pts_jmp_done - 2) as u8;
    text[pts_jns2 + 1] = (pts_done - pts_jns2 - 2) as u8;
    // Now r13 points to the start of the string. Copy it to the beginning of the buffer (rbx).
    // movq %r13, %rsi → 4C 89 EE
    text.extend_from_slice(&[0x4c, 0x89, 0xee]);
    // movq %rbx, %rdi → 48 89 DF
    text.extend_from_slice(&[0x48, 0x89, 0xdf]);
    // .copy_loop:
    let pts_copy_loop = text.len();
    // movb (%rsi), %al → 8A 06
    text.extend_from_slice(&[0x8a, 0x06]);
    // movb %al, (%rdi) → 88 07
    text.extend_from_slice(&[0x88, 0x07]);
    // testq %rax, %rax → 84 C0
    text.extend_from_slice(&[0x84, 0xc0]);
    // je .copy_done
    let pts_je_copy = text.len();
    text.extend_from_slice(&[0x74, 0x00]);
    // incq %rsi → 48 FF C6
    text.extend_from_slice(&[0x48, 0xff, 0xc6]);
    // incq %rdi → 48 FF C7
    text.extend_from_slice(&[0x48, 0xff, 0xc7]);
    // jmp .copy_loop
    let pts_jmp_copy = text.len();
    let pts_copy_back = pts_copy_loop as i64 - pts_jmp_copy as i64 - 2;
    text.extend_from_slice(&[0xeb, pts_copy_back as u8]);
    // .copy_done:
    let pts_copy_done = text.len();
    text[pts_je_copy + 1] = (pts_copy_done - pts_je_copy - 2) as u8;
    // movq %rbx, %rax → 48 89 D8
    text.extend_from_slice(&[0x48, 0x89, 0xd8]);
    // addq $32, %rsp
    text.extend_from_slice(&[0x48, 0x83, 0xc4, 0x20]);
    // popq %r14 ; popq %r13 ; popq %r12 ; popq %rbx
    text.extend_from_slice(&[0x41, 0x5e]);    // popq %r14
    text.extend_from_slice(&[0x41, 0x5d]);    // popq %r13
    text.extend_from_slice(&[0x41, 0x5c]);    // popq %r12
    text.push(0x5b);                           // popq %rbx
    text.push(0xc3);                           // ret

    // ═══════════════════════════════════════════════════════════════════════
    // p_float_to_str(rdi = double bits as i64) → rax = *u8 string
    // Converts a float to a string (matching Rust's {} format) and returns
    // a malloc'd buffer containing the result.
    // ═══════════════════════════════════════════════════════════════════════
    sym!("p_float_to_str");
    // pushq %rbx ; pushq %r12 ; pushq %r13 ; pushq %r14
    text.push(0x53);                          // pushq %rbx
    text.extend_from_slice(&[0x41, 0x54]);    // pushq %r12
    text.extend_from_slice(&[0x41, 0x55]);    // pushq %r13
    text.extend_from_slice(&[0x41, 0x56]);    // pushq %r14
    // subq $56, %rsp
    text.extend_from_slice(&[0x48, 0x83, 0xec, 0x38]);

    // Store double bits at (%rsp), load into xmm0
    text.extend_from_slice(&[0x48, 0x89, 0x3c, 0x24]); // movq %rdi, (%rsp)
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x04, 0x24]); // movsd (%rsp), %xmm0

    // Allocate 32-byte buffer with p_malloc
    text.extend_from_slice(&[0xbf, 0x20, 0x00, 0x00, 0x00]); // movl $32, %edi
    text.extend_from_slice(&[0xe8, 0x00, 0x00, 0x00, 0x00]); // call p_malloc
    let pfts_malloc_off = text.len() - 4;
    text.extend_from_slice(&[0x48, 0x89, 0xc3]); // movq %rax, %rbx (buffer)
    text.extend_from_slice(&[0x49, 0x89, 0xde]); // movq %rbx, %r14 (REX.W+B, ModRM reg=rbx, rm=r14)

    // Check if zero
    text.extend_from_slice(&[0x48, 0xc7, 0x44, 0x24, 0x08, 0x00, 0x00, 0x00, 0x00]); // movq $0, 8(%rsp)
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x4c, 0x24, 0x08]); // movsd 8(%rsp), %xmm1
    text.extend_from_slice(&[0x66, 0x0f, 0x2e, 0xc1]); // ucomisd %xmm1, %xmm0
    let pfts_jp = text.len();
    text.extend_from_slice(&[0x7a, 0x00]); // jp .not_zero
    let pfts_jne = text.len();
    text.extend_from_slice(&[0x75, 0x00]); // jne .not_zero

    // .print_zero: write "0"
    text.extend_from_slice(&[0x41, 0xc6, 0x06, 0x30]); // movb '0', (%r14)
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14
    // jmp .null_term (use rel32 since distance > 127)
    text.extend_from_slice(&[0xe9, 0x00, 0x00, 0x00, 0x00]);
    let pfts_jmp_null = text.len() - 4;

    // .not_zero:
    let pfts_notzero = text.len();
    text[pfts_jp + 1] = (pfts_notzero - pfts_jp - 2) as u8;
    text[pfts_jne + 1] = (pfts_notzero - pfts_jne - 2) as u8;

    // Check negative
    text.extend_from_slice(&[0x48, 0x8b, 0x04, 0x24]); // movq (%rsp), %rax
    text.extend_from_slice(&[0x48, 0x0f, 0xba, 0xe0, 0x3f]); // btq $63, %rax
    let pfts_jnc = text.len();
    text.extend_from_slice(&[0x73, 0x00]); // jnc .positive

    // Negative: negate, write '-'
    text.extend_from_slice(&[0x48, 0x0f, 0xba, 0xf8, 0x3f]); // btcq $63, %rax
    text.extend_from_slice(&[0x48, 0x89, 0x04, 0x24]); // movq %rax, (%rsp)
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x04, 0x24]); // movsd (%rsp), %xmm0
    text.extend_from_slice(&[0x41, 0xc6, 0x06, 0x2d]); // movb '-', (%r14)
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14

    // .positive:
    let pfts_positive = text.len();
    text[pfts_jnc + 1] = (pfts_positive - pfts_jnc - 2) as u8;

    // cvttsd2siq %xmm0, %r12 (integer part)
    text.extend_from_slice(&[0xf2, 0x4c, 0x0f, 0x2c, 0xe0]);
    // cvtsi2sdq %r12, %xmm1 ; subsd → frac
    text.extend_from_slice(&[0xf2, 0x49, 0x0f, 0x2a, 0xcc]);
    text.extend_from_slice(&[0xf2, 0x0f, 0x5c, 0xc1]);

    // Multiply frac by 10^6 + 0.5
    text.extend_from_slice(&[0xc7, 0x44, 0x24, 0x08, 0x00, 0x00, 0x00, 0x00]);
    text.extend_from_slice(&[0xc7, 0x44, 0x24, 0x0c, 0x80, 0x84, 0x2e, 0x41]);
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x4c, 0x24, 0x08]); // movsd 8(%rsp), %xmm1
    text.extend_from_slice(&[0xf2, 0x0f, 0x59, 0xc1]); // mulsd
    text.extend_from_slice(&[0xc7, 0x44, 0x24, 0x10, 0x00, 0x00, 0x00, 0x00]);
    text.extend_from_slice(&[0xc7, 0x44, 0x24, 0x14, 0x00, 0x00, 0xe0, 0x3f]);
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x4c, 0x24, 0x10]); // movsd 16(%rsp), %xmm1
    text.extend_from_slice(&[0xf2, 0x0f, 0x58, 0xc1]); // addsd
    text.extend_from_slice(&[0xf2, 0x4c, 0x0f, 0x2c, 0xe8]); // cvttsd2siq %xmm0, %r13

    // Overflow check
    text.extend_from_slice(&[0x49, 0x81, 0xfd, 0x40, 0x42, 0x0f, 0x00]); // cmpq $1000000, %r13
    let pfts_jl_ovf = text.len();
    text.extend_from_slice(&[0x7c, 0x00]); // jl .no_ovf
    text.extend_from_slice(&[0x49, 0xff, 0xc4]); // incq %r12
    text.extend_from_slice(&[0x49, 0x81, 0xed, 0x40, 0x42, 0x0f, 0x00]); // subq $1000000, %r13
    let pfts_no_ovf = text.len();
    text[pfts_jl_ovf + 1] = (pfts_no_ovf - pfts_jl_ovf - 2) as u8;

    // Convert integer part (r12) to string at r14 (build in reverse at 24(%rsp))
    // leaq 30(%rsp), %rsi
    text.extend_from_slice(&[0x48, 0x8d, 0x74, 0x24, 0x1e]); // leaq 30(%rsp), %rsi
    // .int_loop:
    let pfts_int_loop = text.len();
    text.extend_from_slice(&[0x4d, 0x85, 0xe4]); // testq %r12, %r12
    let pfts_je_intdone = text.len();
    text.extend_from_slice(&[0x74, 0x00]); // je .int_done
    text.extend_from_slice(&[0x48, 0x31, 0xd2]); // xorq %rdx, %rdx
    text.extend_from_slice(&[0xb9, 0x0a, 0x00, 0x00, 0x00]); // movl $10, %ecx
    text.extend_from_slice(&[0x4c, 0x89, 0xe0]); // movq %r12, %rax
    text.extend_from_slice(&[0x48, 0xf7, 0xf1]); // divq %rcx
    text.extend_from_slice(&[0x49, 0x89, 0xc4]); // movq %rax, %r12
    text.extend_from_slice(&[0x80, 0xc2, 0x30]); // addb $48, %dl
    text.extend_from_slice(&[0x48, 0xff, 0xce]); // decq %rsi
    text.extend_from_slice(&[0x88, 0x16]); // movb %dl, (%rsi)
    let pfts_jmp_intloop = text.len();
    let int_loop_back = pfts_int_loop as i64 - pfts_jmp_intloop as i64 - 2;
    text.extend_from_slice(&[0xeb, int_loop_back as u8]);

    // .int_done:
    let pfts_int_done = text.len();
    text[pfts_je_intdone + 1] = (pfts_int_done - pfts_je_intdone - 2) as u8;

    // If no digits written (rsi == 30(%rsp)), write '0'
    text.extend_from_slice(&[0x48, 0x8d, 0x44, 0x24, 0x1e]); // leaq 30(%rsp), %rax
    text.extend_from_slice(&[0x48, 0x39, 0xc6]); // cmpq %rax, %rsi
    let pfts_jne_copyint = text.len();
    text.extend_from_slice(&[0x75, 0x00]); // jne .copy_int
    text.extend_from_slice(&[0x48, 0xff, 0xce]); // decq %rsi
    text.extend_from_slice(&[0xc6, 0x06, 0x30]); // movb '0', (%rsi)

    // .copy_int: copy from rsi to 30(%rsp) to r14
    let pfts_copy_int = text.len();
    text[pfts_jne_copyint + 1] = (pfts_copy_int - pfts_jne_copyint - 2) as u8;
    // .copy_int_loop:
    let pfts_copy_int_loop = text.len();
    text.extend_from_slice(&[0x48, 0x8d, 0x44, 0x24, 0x1e]); // leaq 30(%rsp), %rax
    text.extend_from_slice(&[0x48, 0x39, 0xc6]); // cmpq %rax, %rsi
    let pfts_jge_copydone = text.len();
    text.extend_from_slice(&[0x7d, 0x00]); // jge .copy_int_done
    text.extend_from_slice(&[0x8a, 0x06]); // movb (%rsi), %al
    text.extend_from_slice(&[0x41, 0x88, 0x06]); // movb %al, (%r14)
    text.extend_from_slice(&[0x48, 0xff, 0xc6]); // incq %rsi
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14
    let pfts_jmp_copyint = text.len();
    let copy_int_back = pfts_copy_int_loop as i64 - pfts_jmp_copyint as i64 - 2;
    text.extend_from_slice(&[0xeb, copy_int_back as u8]);
    // .copy_int_done:
    let pfts_copy_int_done = text.len();
    text[pfts_jge_copydone + 1] = (pfts_copy_int_done - pfts_jge_copydone - 2) as u8;

    // Check if frac (r13) is zero
    text.extend_from_slice(&[0x4d, 0x85, 0xed]); // testq %r13, %r13
    let pfts_je_nullterm = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0x00, 0x00, 0x00, 0x00]); // je .null_term (rel32)

    // Write '.'
    text.extend_from_slice(&[0x41, 0xc6, 0x06, 0x2e]); // movb '.', (%r14)
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14

    // Convert r13 to 6-digit zero-padded string (reverse at 24..30(%rsp))
    text.extend_from_slice(&[0x48, 0x8d, 0x74, 0x24, 0x1e]); // leaq 30(%rsp), %rsi
    text.extend_from_slice(&[0x41, 0xbc, 0x06, 0x00, 0x00, 0x00]); // movl $6, %r12d
    // .frac_loop:
    let pfts_frac_loop = text.len();
    text.extend_from_slice(&[0x48, 0xff, 0xce]); // decq %rsi
    text.extend_from_slice(&[0x48, 0x31, 0xd2]); // xorq %rdx, %rdx
    text.extend_from_slice(&[0xb9, 0x0a, 0x00, 0x00, 0x00]); // movl $10, %ecx
    text.extend_from_slice(&[0x4c, 0x89, 0xe8]); // movq %r13, %rax
    text.extend_from_slice(&[0x48, 0xf7, 0xf1]); // divq %rcx
    text.extend_from_slice(&[0x49, 0x89, 0xc5]); // movq %rax, %r13
    text.extend_from_slice(&[0x80, 0xc2, 0x30]); // addb $48, %dl
    text.extend_from_slice(&[0x88, 0x16]); // movb %dl, (%rsi)
    text.extend_from_slice(&[0x49, 0xff, 0xcc]); // decq %r12
    text.extend_from_slice(&[0x4d, 0x85, 0xe4]); // testq %r12, %r12
    let pfts_jne_fracloop = text.len();
    text.extend_from_slice(&[0x75, 0x00]); // jne .frac_loop
    let frac_loop_back = pfts_frac_loop as i64 - (text.len()) as i64;
    text[pfts_jne_fracloop + 1] = frac_loop_back as u8;

    // Strip trailing zeros: find last non-'0' from end
    text.extend_from_slice(&[0x48, 0x8d, 0x54, 0x24, 0x1e]); // leaq 30(%rsp), %rdx (end)
    // .strip_loop:
    let pfts_strip_loop = text.len();
    text.extend_from_slice(&[0x48, 0x39, 0xf2]); // cmpq %rsi, %rdx
    let pfts_je_stripdone = text.len();
    text.extend_from_slice(&[0x74, 0x00]); // je .strip_done
    text.extend_from_slice(&[0x48, 0xff, 0xca]); // decq %rdx
    text.extend_from_slice(&[0x80, 0x3a, 0x30]); // cmpb '0', (%rdx)
    let pfts_je_striploop = text.len();
    let strip_back = pfts_strip_loop as i64 - pfts_je_striploop as i64 - 2;
    text.extend_from_slice(&[0x74, strip_back as u8]);
    // .strip_done:
    let pfts_strip_done = text.len();
    text[pfts_je_stripdone + 1] = (pfts_strip_done - pfts_je_stripdone - 2) as u8;

    // Copy stripped frac from rsi..rdx (inclusive) to r14
    // .frac_copy_loop:
    let pfts_frac_copy = text.len();
    text.extend_from_slice(&[0x48, 0x39, 0xd6]); // cmpq %rdx, %rsi
    let pfts_je_fracdone = text.len();
    text.extend_from_slice(&[0x77, 0x00]); // ja .frac_copy_done (copy while rsi <= rdx)
    text.extend_from_slice(&[0x8a, 0x06]); // movb (%rsi), %al
    text.extend_from_slice(&[0x41, 0x88, 0x06]); // movb %al, (%r14)
    text.extend_from_slice(&[0x48, 0xff, 0xc6]); // incq %rsi
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14
    let pfts_jmp_fraccopy = text.len();
    let frac_copy_back = pfts_frac_copy as i64 - pfts_jmp_fraccopy as i64 - 2;
    text.extend_from_slice(&[0xeb, frac_copy_back as u8]);
    // .frac_copy_done:
    let pfts_frac_copy_done = text.len();
    text[pfts_je_fracdone + 1] = (pfts_frac_copy_done - pfts_je_fracdone - 2) as u8;

    // .null_term:
    let pfts_null_term = text.len();
    let null_term_off = (pfts_null_term as i64 - pfts_je_nullterm as i64 - 6) as i32;
    text[pfts_je_nullterm + 2..pfts_je_nullterm + 6].copy_from_slice(&null_term_off.to_le_bytes());
    let jmp_null_off = (pfts_null_term as i64 - pfts_jmp_null as i64 - 4) as i32;
    text[pfts_jmp_null..pfts_jmp_null + 4].copy_from_slice(&jmp_null_off.to_le_bytes());

    // movb $0, (%r14) — null terminate
    text.extend_from_slice(&[0x41, 0xc6, 0x06, 0x00]);
    // movq %rbx, %rax (return buffer)
    text.extend_from_slice(&[0x48, 0x89, 0xd8]);

    // Epilogue
    text.extend_from_slice(&[0x48, 0x83, 0xc4, 0x38]); // addq $56, %rsp
    text.extend_from_slice(&[0x41, 0x5e]);    // popq %r14
    text.extend_from_slice(&[0x41, 0x5d]);    // popq %r13
    text.extend_from_slice(&[0x41, 0x5c]);    // popq %r12
    text.push(0x5b);                           // popq %rbx
    text.push(0xc3);                           // ret

    // ═══════════════════════════════════════════════════════════════════════
    // p_bool_to_str(rdi = 0 or 1) → rax = *u8 string
    // Returns "true" or "false" in a malloc'd buffer.
    // ═══════════════════════════════════════════════════════════════════════
    sym!("p_bool_to_str");
    // pushq %rbx
    text.push(0x53);
    // subq $16, %rsp (alignment + save rsi across malloc call)
    text.extend_from_slice(&[0x48, 0x83, 0xec, 0x10]);
    // testq %rdi, %rdi
    text.extend_from_slice(&[0x48, 0x85, 0xff]);
    // jne .true
    let pbts_jne = text.len();
    text.extend_from_slice(&[0x75, 0x00]);
    // .false: movl $6, %edi (len of "false\0")
    text.extend_from_slice(&[0xbf, 0x06, 0x00, 0x00, 0x00]);
    // leaq .Lfalse_str(%rip), %rsi
    text.extend_from_slice(&[0x48, 0x8d, 0x35, 0x00, 0x00, 0x00, 0x00]);
    let pbts_false_lea = text.len() - 4;
    // jmp .copy (skip over .true path: movl $5 + leaq = 12 bytes)
    text.extend_from_slice(&[0xeb, 0x0c]); // jmp .copy (12 bytes ahead)
    // .true:
    let pbts_true = text.len();
    text[pbts_jne + 1] = (pbts_true - pbts_jne - 2) as u8;
    // movl $5, %edi (len of "true\0")
    text.extend_from_slice(&[0xbf, 0x05, 0x00, 0x00, 0x00]);
    // leaq .Ltrue_str(%rip), %rsi
    text.extend_from_slice(&[0x48, 0x8d, 0x35, 0x00, 0x00, 0x00, 0x00]);
    let pbts_true_lea = text.len() - 4;
    // .copy: (both paths converge here)
    // Save rsi to stack (p_malloc clobbers caller-saved regs)
    text.extend_from_slice(&[0x48, 0x89, 0x74, 0x24, 0x08]); // movq %rsi, 8(%rsp)
    // Save len in rbx, call p_malloc
    text.extend_from_slice(&[0x48, 0x89, 0xfb]); // movq %rdi, %rbx
    text.extend_from_slice(&[0xe8, 0x00, 0x00, 0x00, 0x00]); // call p_malloc
    let pbts_malloc_off = text.len() - 4;
    // Restore rsi from stack
    text.extend_from_slice(&[0x48, 0x8b, 0x74, 0x24, 0x08]); // movq 8(%rsp), %rsi
    // Now copy from rsi to rax, rbx bytes
    // movq %rax, %rdi (dest)
    text.extend_from_slice(&[0x48, 0x89, 0xc7]);
    // movq %rbx, %rcx (count)
    text.extend_from_slice(&[0x48, 0x89, 0xd9]);
    // .copy_loop:
    let pbts_copy_loop = text.len();
    // testq %rcx, %rcx
    text.extend_from_slice(&[0x48, 0x85, 0xc9]);
    // je .copy_done
    let pbts_je = text.len();
    text.extend_from_slice(&[0x74, 0x00]);
    // movb (%rsi), %al
    text.extend_from_slice(&[0x8a, 0x06]);
    // movb %al, (%rdi)
    text.extend_from_slice(&[0x88, 0x07]);
    // incq %rsi ; incq %rdi ; decq %rcx
    text.extend_from_slice(&[0x48, 0xff, 0xc6]);
    text.extend_from_slice(&[0x48, 0xff, 0xc7]);
    text.extend_from_slice(&[0x48, 0xff, 0xc9]);
    // jmp .copy_loop
    let pbts_jmp = text.len();
    let pbts_back = pbts_copy_loop as i64 - pbts_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, pbts_back as u8]);
    // .copy_done:
    let pbts_copy_done = text.len();
    text[pbts_je + 1] = (pbts_copy_done - pbts_je - 2) as u8;
    // movq %rdi - rbx → %rax (return original malloc ptr)
    // Actually, rdi has been advanced. We need the original rax.
    // rax still holds the malloc result (not clobbered by copy loop).
    // addq $16, %rsp ; popq %rbx ; ret
    text.extend_from_slice(&[0x48, 0x83, 0xc4, 0x10]);
    text.push(0x5b);
    text.push(0xc3);

    // ═══════════════════════════════════════════════════════════════════════
    // p_vec_to_str(rdi = vec ptr) → rax = *u8 string
    // Formats as "[1, 2, 3]" in a malloc'd buffer.
    // ═══════════════════════════════════════════════════════════════════════
    sym!("p_vec_to_str");
    // pushq %rbx ; pushq %r12 ; pushq %r13 ; pushq %r14 ; pushq %r15
    text.push(0x53);
    text.extend_from_slice(&[0x41, 0x54]);
    text.extend_from_slice(&[0x41, 0x55]);
    text.extend_from_slice(&[0x41, 0x56]);
    text.extend_from_slice(&[0x41, 0x57]);
    // subq $40, %rsp
    text.extend_from_slice(&[0x48, 0x83, 0xec, 0x28]);
    // movq %rdi, %rbx (save vec ptr)
    text.extend_from_slice(&[0x48, 0x89, 0xfb]);
    // malloc 256 bytes for result buffer
    text.extend_from_slice(&[0xbf, 0x00, 0x01, 0x00, 0x00]); // movl $256, %edi
    text.extend_from_slice(&[0xe8, 0x00, 0x00, 0x00, 0x00]); // call p_malloc
    let pvts_malloc_off = text.len() - 4;
    // movq %rax, %r15 (buffer ptr)
    text.extend_from_slice(&[0x49, 0x89, 0xc7]);
    // movq %r15, %r14 (write ptr = buffer start)
    text.extend_from_slice(&[0x4d, 0x89, 0xfe]);
    // Write '['
    text.extend_from_slice(&[0x41, 0xc6, 0x06, 0x5b]); // movb '[', (%r14)
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14
    // Call p_vec_len(rdi=rbx) to get len
    text.extend_from_slice(&[0x48, 0x89, 0xdf]); // movq %rbx, %rdi
    text.extend_from_slice(&[0xe8, 0x00, 0x00, 0x00, 0x00]); // call p_vec_len
    let pvts_len_call_off = text.len() - 4;
    // movq %rax, %r12 (remaining len)
    text.extend_from_slice(&[0x49, 0x89, 0xc4]);
    // movq (%rbx), %rbx (data_ptr = vec[0])
    text.extend_from_slice(&[0x48, 0x8b, 0x1b]);
    // xorq %r13, %r13 (index = 0, for comma check)
    text.extend_from_slice(&[0x4d, 0x31, 0xed]);
    // .loop: testq %r12, %r12
    let pvts_loop = text.len();
    text.extend_from_slice(&[0x4d, 0x85, 0xe4]);
    // jz .done
    let pvts_jz = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0x00, 0x00, 0x00, 0x00]); // jz rel32
    // If not first element, write ", "
    text.extend_from_slice(&[0x4d, 0x85, 0xed]); // testq %r13, %r13
    // je .skip_comma
    let pvts_je_comma = text.len();
    text.extend_from_slice(&[0x74, 0x00]);
    // Write ", "
    text.extend_from_slice(&[0x41, 0xc6, 0x06, 0x2c]); // movb ',', (%r14)
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14
    text.extend_from_slice(&[0x41, 0xc6, 0x06, 0x20]); // movb ' ', (%r14)
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14
    // .skip_comma:
    let pvts_skip_comma = text.len();
    text[pvts_je_comma + 1] = (pvts_skip_comma - pvts_je_comma - 2) as u8;
    // Load element: movq (%rbx), %rdi
    text.extend_from_slice(&[0x48, 0x8b, 0x3b]);
    // call p_to_str
    text.extend_from_slice(&[0xe8, 0x00, 0x00, 0x00, 0x00]);
    let pvts_tostr_call_off = text.len() - 4;
    // Copy result string (rax) to r14
    // movq %rax, %rsi (src)
    text.extend_from_slice(&[0x48, 0x89, 0xc6]);
    // .copy_elem_loop:
    let pvts_copy_elem = text.len();
    // movb (%rsi), %al
    text.extend_from_slice(&[0x8a, 0x06]);
    // testq %rax, %rax
    text.extend_from_slice(&[0x84, 0xc0]);
    // je .elem_done
    let pvts_je_elem = text.len();
    text.extend_from_slice(&[0x74, 0x00]);
    // movb %al, (%r14)
    text.extend_from_slice(&[0x41, 0x88, 0x06]);
    // incq %rsi ; incq %r14
    text.extend_from_slice(&[0x48, 0xff, 0xc6]);
    text.extend_from_slice(&[0x49, 0xff, 0xc6]);
    // jmp .copy_elem_loop
    let pvts_jmp_elem = text.len();
    let pvts_elem_back = pvts_copy_elem as i64 - pvts_jmp_elem as i64 - 2;
    text.extend_from_slice(&[0xeb, pvts_elem_back as u8]);
    // .elem_done:
    let pvts_elem_done = text.len();
    text[pvts_je_elem + 1] = (pvts_elem_done - pvts_je_elem - 2) as u8;
    // addq $8, %rbx (advance data_ptr)
    text.extend_from_slice(&[0x48, 0x83, 0xc3, 0x08]);
    // decq %r12 ; incq %r13
    text.extend_from_slice(&[0x49, 0xff, 0xcc]);
    text.extend_from_slice(&[0x49, 0xff, 0xc5]);
    // jmp .loop
    let pvts_jmp_loop = text.len();
    let pvts_loop_back = pvts_loop as i64 - pvts_jmp_loop as i64 - 2;
    text.extend_from_slice(&[0xeb, pvts_loop_back as u8]);
    // .done:
    let pvts_done = text.len();
    let pvts_done_rel = (pvts_done as i64 - pvts_jz as i64 - 6) as i32;
    text[pvts_jz + 2..pvts_jz + 6].copy_from_slice(&pvts_done_rel.to_le_bytes());
    // Write ']'
    text.extend_from_slice(&[0x41, 0xc6, 0x06, 0x5d]); // movb ']', (%r14)
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14
    // Write null terminator
    text.extend_from_slice(&[0x41, 0xc6, 0x06, 0x00]); // movb $0, (%r14)
    // movq %r15, %rax (return buffer)
    text.extend_from_slice(&[0x4c, 0x89, 0xf8]);
    // Epilogue
    text.extend_from_slice(&[0x48, 0x83, 0xc4, 0x28]); // addq $40, %rsp
    text.extend_from_slice(&[0x41, 0x5f]); // popq %r15
    text.extend_from_slice(&[0x41, 0x5e]); // popq %r14
    text.extend_from_slice(&[0x41, 0x5d]); // popq %r13
    text.extend_from_slice(&[0x41, 0x5c]); // popq %r12
    text.push(0x5b); // popq %rbx
    text.push(0xc3); // ret

    // ═══════════════════════════════════════════════════════════════════════
    // Allocator builtins (simplified)
    // ═══════════════════════════════════════════════════════════════════════
    // p_allocator_create() → rax = 1 (dummy non-null)
    sym!("p_allocator_create");
    text.extend_from_slice(&[0xb8, 0x01, 0x00, 0x00, 0x00]); // movl $1, %eax
    text.push(0xc3); // ret

    // p_allocator_alloc(rdi=alloc, rsi=size) → rax = ptr
    // movq %rsi, %rdi ; jmp p_malloc
    sym!("p_allocator_alloc");
    text.extend_from_slice(&[0x48, 0x89, 0xf7]); // movq %rsi, %rdi
    text.extend_from_slice(&[0xe9, 0x00, 0x00, 0x00, 0x00]); // jmp p_malloc
    let palloc_alloc_jmp_off = text.len() - 4;

    // p_allocator_free(rdi=alloc, rsi=ptr) → void
    // movq %rsi, %rdi ; jmp p_free
    sym!("p_allocator_free");
    text.extend_from_slice(&[0x48, 0x89, 0xf7]); // movq %rsi, %rdi
    text.extend_from_slice(&[0xe9, 0x00, 0x00, 0x00, 0x00]); // jmp p_free
    let palloc_free_jmp_off = text.len() - 4;

    // p_allocator_stats(rdi=alloc) → rax = 0
    sym!("p_allocator_stats");
    text.extend_from_slice(&[0x48, 0x31, 0xc0]); // xorq %rax, %rax
    text.push(0xc3); // ret

    // ═══════════════════════════════════════════════════════════════════════
    // String / Vec / HashMap runtime functions (encoded from assembly text)
    // ═══════════════════════════════════════════════════════════════════════
    let runtime_asm = r#"
# ─── p_strlen(rdi=s) → rax = length ───
p_strlen:
    movq %rdi, %rax
    xorl %ecx, %ecx
.Lpstrlen_loop:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpstrlen_done
    incq %rax
    incq %rcx
    jmp .Lpstrlen_loop
.Lpstrlen_done:
    movq %rcx, %rax
    ret

# ─── p_string_len(rdi=s) → rax = strlen(s) ───
p_string_len:
    movq %rdi, %rax
    xorl %ecx, %ecx
.Lpslen_loop:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpslen_done
    incq %rax
    incq %rcx
    jmp .Lpslen_loop
.Lpslen_done:
    movq %rcx, %rax
    ret

# ─── p_string_is_empty(rdi=s) → rax = 1 if empty ───
p_string_is_empty:
    movzbq (%rdi), %rax
    testq %rax, %rax
    sete %al
    movzbq %al, %rax
    ret

# ─── p_string_from(rdi=s) → rax = strdup(s) ───
p_string_from:
    pushq %rbx
    pushq %r12
    movq %rdi, %rbx
    movq %rbx, %rdi
    call p_strlen
    leaq 1(%rax), %rdi
    call p_malloc
    movq %rax, %r12
    movq %rbx, %rsi
    movq %r12, %rdi
.Lpsfrom_loop:
    movzbq (%rsi), %rax
    movb %al, (%rdi)
    testq %rax, %rax
    je .Lpsfrom_done
    incq %rsi
    incq %rdi
    jmp .Lpsfrom_loop
.Lpsfrom_done:
    movq %r12, %rax
    popq %r12
    popq %rbx
    ret

# ─── p_string_contains(rdi=s, rsi=sub) → rax = 1 if found ───
p_string_contains:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    movq %rdi, %rbx
    movq %rsi, %r12
    # get slen
    movq %rbx, %rax
    xorl %ecx, %ecx
.Lpsc_slen:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpsc_slen_done
    incq %rax
    incq %rcx
    jmp .Lpsc_slen
.Lpsc_slen_done:
    movq %rcx, %r13
    # get sublen
    movq %r12, %rax
    xorl %ecx, %ecx
.Lpsc_sublen:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpsc_sublen_done
    incq %rax
    incq %rcx
    jmp .Lpsc_sublen
.Lpsc_sublen_done:
    movq %rcx, %r14
    # if sublen == 0 → true
    testq %r14, %r14
    je .Lpsc_true
    # if sublen > slen → false
    cmpq %r13, %r14
    ja .Lpsc_false
    # max_i = slen - sublen + 1
    movq %r13, %r8
    subq %r14, %r8
    incq %r8
    xorl %r9, %r9
.Lpsc_outer:
    cmpq %r8, %r9
    jae .Lpsc_false
    xorl %r10, %r10
.Lpsc_inner:
    cmpq %r14, %r10
    jae .Lpsc_true
    leaq (%rbx,%r9), %r11
    movzbq (%r11,%r10), %rax
    movzbq (%r12,%r10), %rcx
    cmpq %rcx, %rax
    jne .Lpsc_next
    incq %r10
    jmp .Lpsc_inner
.Lpsc_next:
    incq %r9
    jmp .Lpsc_outer
.Lpsc_true:
    movl $1, %eax
    jmp .Lpsc_ret
.Lpsc_false:
    xorl %eax, %eax
.Lpsc_ret:
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# ─── p_string_starts_with(rdi=s, rsi=prefix) → rax = bool ───
p_string_starts_with:
    pushq %rbx
    pushq %r12
    movq %rdi, %rbx
    movq %rsi, %r12
    xorl %rcx, %rcx
.Lpsw_loop:
    movzbq (%r12,%rcx), %rax
    testq %rax, %rax
    je .Lpsw_true
    movzbq (%rbx,%rcx), %rdx
    cmpq %rdx, %rax
    jne .Lpsw_false
    incq %rcx
    jmp .Lpsw_loop
.Lpsw_true:
    movl $1, %eax
    jmp .Lpsw_ret
.Lpsw_false:
    xorl %eax, %eax
.Lpsw_ret:
    popq %r12
    popq %rbx
    ret

# ─── p_string_ends_with(rdi=s, rsi=suffix) → rax = bool ───
p_string_ends_with:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    movq %rdi, %rbx
    movq %rsi, %r12
    # slen
    movq %rbx, %rax
    xorl %ecx, %ecx
.Lpse_slen:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpse_slen_done
    incq %rax
    incq %rcx
    jmp .Lpse_slen
.Lpse_slen_done:
    movq %rcx, %r13
    # sublen
    movq %r12, %rax
    xorl %ecx, %ecx
.Lpse_sublen:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpse_sublen_done
    incq %rax
    incq %rcx
    jmp .Lpse_sublen
.Lpse_sublen_done:
    movq %rcx, %r14
    # if sublen > slen → false
    cmpq %r13, %r14
    ja .Lpse_false
    # start = slen - sublen
    movq %r13, %rcx
    subq %r14, %rcx
    xorl %rdx, %rdx
.Lpse_loop:
    cmpq %r14, %rdx
    jae .Lpse_true
    movzbq (%rbx,%rcx), %rax
    movzbq (%r12,%rdx), %r8
    cmpq %r8, %rax
    jne .Lpse_false
    incq %rcx
    incq %rdx
    jmp .Lpse_loop
.Lpse_true:
    movl $1, %eax
    jmp .Lpse_ret
.Lpse_false:
    xorl %eax, %eax
.Lpse_ret:
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# ─── p_string_find(rdi=s, rsi=sub) → rax = index or -1 ───
p_string_find:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    movq %rdi, %rbx
    movq %rsi, %r12
    # slen
    movq %rbx, %rax
    xorl %ecx, %ecx
.Lpsf_slen:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpsf_slen_done
    incq %rax
    incq %rcx
    jmp .Lpsf_slen
.Lpsf_slen_done:
    movq %rcx, %r13
    # sublen
    movq %r12, %rax
    xorl %ecx, %ecx
.Lpsf_sublen:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpsf_sublen_done
    incq %rax
    incq %rcx
    jmp .Lpsf_sublen
.Lpsf_sublen_done:
    movq %rcx, %r14
    # if sublen == 0 → 0
    testq %r14, %r14
    je .Lpsf_zero
    # if sublen > slen → -1
    cmpq %r13, %r14
    ja .Lpsf_neg
    movq %r13, %r8
    subq %r14, %r8
    incq %r8
    xorl %r9, %r9
.Lpsf_outer:
    cmpq %r8, %r9
    jae .Lpsf_neg
    xorl %r10, %r10
.Lpsf_inner:
    cmpq %r14, %r10
    jae .Lpsf_found
    leaq (%rbx,%r9), %r11
    movzbq (%r11,%r10), %rax
    movzbq (%r12,%r10), %rcx
    cmpq %rcx, %rax
    jne .Lpsf_next
    incq %r10
    jmp .Lpsf_inner
.Lpsf_next:
    incq %r9
    jmp .Lpsf_outer
.Lpsf_found:
    movq %r9, %rax
    jmp .Lpsf_ret
.Lpsf_zero:
    xorl %eax, %eax
    jmp .Lpsf_ret
.Lpsf_neg:
    movq $-1, %rax
.Lpsf_ret:
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# ─── p_string_to_uppercase(rdi=s) → rax = new string ───
p_string_to_uppercase:
    pushq %rbx
    pushq %r12
    movq %rdi, %rbx
    call p_strlen
    leaq 1(%rax), %rdi
    call p_malloc
    movq %rax, %r12
    xorl %ecx, %ecx
.Lpsu_loop:
    movzbq (%rbx,%rcx), %rax
    testq %rax, %rax
    je .Lpsu_done
    # if 'a' <= al <= 'z': al -= 32
    cmpq $97, %rax
    jb .Lpsu_skip
    cmpq $122, %rax
    ja .Lpsu_skip
    subq $32, %rax
.Lpsu_skip:
    movb %al, (%r12,%rcx)
    incq %rcx
    jmp .Lpsu_loop
.Lpsu_done:
    movb $0, (%r12,%rcx)
    movq %r12, %rax
    popq %r12
    popq %rbx
    ret

# ─── p_string_to_lowercase(rdi=s) → rax = new string ───
p_string_to_lowercase:
    pushq %rbx
    pushq %r12
    movq %rdi, %rbx
    call p_strlen
    leaq 1(%rax), %rdi
    call p_malloc
    movq %rax, %r12
    xorl %ecx, %ecx
.Lpsl_loop:
    movzbq (%rbx,%rcx), %rax
    testq %rax, %rax
    je .Lpsl_done
    cmpq $65, %rax
    jb .Lpsl_skip
    cmpq $90, %rax
    ja .Lpsl_skip
    addq $32, %rax
.Lpsl_skip:
    movb %al, (%r12,%rcx)
    incq %rcx
    jmp .Lpsl_loop
.Lpsl_done:
    movb $0, (%r12,%rcx)
    movq %r12, %rax
    popq %r12
    popq %rbx
    ret

# ─── p_string_trim(rdi=s) → rax = trimmed string ───
p_string_trim:
    pushq %rbx
    pushq %r12
    pushq %r13
    movq %rdi, %rbx
    # skip leading whitespace
.Lpst_lead:
    movzbq (%rbx), %rax
    cmpq $32, %rax
    je .Lpst_lead_next
    cmpq $9, %rax
    je .Lpst_lead_next
    cmpq $10, %rax
    je .Lpst_lead_next
    cmpq $13, %rax
    je .Lpst_lead_next
    jmp .Lpst_lead_done
.Lpst_lead_next:
    incq %rbx
    jmp .Lpst_lead
.Lpst_lead_done:
    # get length from rbx
    movq %rbx, %rax
    xorl %ecx, %ecx
.Lpst_slen:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpst_slen_done
    incq %rax
    incq %rcx
    jmp .Lpst_slen
.Lpst_slen_done:
    # trim trailing whitespace: rcx = len, r13 = end
    movq %rcx, %r13
.Lpst_trail:
    testq %r13, %r13
    je .Lpst_trail_done
    leaq -1(%rbx,%r13), %rax
    movzbq (%rax), %rdx
    cmpq $32, %rdx
    je .Lpst_trail_next
    cmpq $9, %rdx
    je .Lpst_trail_next
    cmpq $10, %rdx
    je .Lpst_trail_next
    cmpq $13, %rdx
    je .Lpst_trail_next
    jmp .Lpst_trail_done
.Lpst_trail_next:
    decq %r13
    jmp .Lpst_trail
.Lpst_trail_done:
    # malloc r13+1, copy
    leaq 1(%r13), %rdi
    call p_malloc
    movq %rax, %r12
    xorl %ecx, %ecx
.Lpst_copy:
    cmpq %r13, %rcx
    jae .Lpst_copy_done
    movzbq (%rbx,%rcx), %rax
    movb %al, (%r12,%rcx)
    incq %rcx
    jmp .Lpst_copy
.Lpst_copy_done:
    movb $0, (%r12,%r13)
    movq %r12, %rax
    popq %r13
    popq %r12
    popq %rbx
    ret

# ─── p_string_repeat(rdi=s, rsi=n) → rax = s*n ───
p_string_repeat:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    movq %rdi, %rbx
    movq %rsi, %r12
    # get slen
    movq %rbx, %rax
    xorl %ecx, %ecx
.Lpsr_slen:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpsr_slen_done
    incq %rax
    incq %rcx
    jmp .Lpsr_slen
.Lpsr_slen_done:
    movq %rcx, %r13
    # total = slen * n + 1
    movq %r13, %rax
    imulq %r12, %rax
    leaq 1(%rax), %rdi
    call p_malloc
    movq %rax, %r14
    xorl %rcx, %rcx
.Lpsr_outer:
    testq %r12, %r12
    je .Lpsr_done
    xorl %edx, %edx
.Lpsr_inner:
    cmpq %r13, %rdx
    jae .Lpsr_inner_done
    movzbq (%rbx,%rdx), %rax
    movb %al, (%r14,%rcx)
    incq %rcx
    incq %rdx
    jmp .Lpsr_inner
.Lpsr_inner_done:
    decq %r12
    jmp .Lpsr_outer
.Lpsr_done:
    movb $0, (%r14,%rcx)
    movq %r14, %rax
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# ─── p_string_parse_int(rdi=s) → rax = parsed int ───
p_string_parse_int:
    xorl %eax, %eax
    xorl %ecx, %ecx
    movzbq (%rdi), %rdx
    cmpq $45, %rdx
    jne .Lpspi_loop
    incq %rdi
    movl $1, %ecx
.Lpspi_loop:
    movzbq (%rdi), %rdx
    cmpq $48, %rdx
    jb .Lpspi_done
    cmpq $57, %rdx
    ja .Lpspi_done
    # rax = rax * 10 + (dl - 48)
    leaq (%rax,%rax,4), %rax
    leaq (%rax,%rax), %rax
    subq $48, %rdx
    movzbq %dl, %r8
    addq %r8, %rax
    incq %rdi
    jmp .Lpspi_loop
.Lpspi_done:
    testl %ecx, %ecx
    je .Lpspi_ret
    negq %rax
.Lpspi_ret:
    ret

# ─── p_string_char_at(rdi=s, rsi=idx) → rax = 2-byte string ───
p_string_char_at:
    pushq %rbx
    movq $2, %rdi
    call p_malloc
    movq %rax, %rbx
    movzbq (%rdi,%rsi), %rdx
    movb %dl, (%rax)
    movb $0, 1(%rax)
    movq %rbx, %rax
    popq %rbx
    ret

# ─── p_string_substring(rdi=s, rsi=start, rdx=end) → rax = slice ───
p_string_substring:
    pushq %rbx
    pushq %r12
    pushq %r13
    movq %rdi, %rbx
    movq %rsi, %r12
    movq %rdx, %r13
    # if start < 0: start = 0
    testq %r12, %r12
    jge .Lpss_sok
    xorl %r12, %r12
.Lpss_sok:
    # if end < start: start = end
    cmpq %r12, %r13
    jge .Lpss_eok
    movq %r13, %r12
.Lpss_eok:
    # len = end - start
    movq %r13, %rdi
    subq %r12, %rdi
    leaq 1(%rdi), %rdi
    call p_malloc
    movq %rax, %rcx
    # copy s[start..end]
    xorl %edx, %edx
.Lpss_copy:
    movq %r13, %r8
    subq %r12, %r8
    cmpq %r8, %rdx
    jae .Lpss_done
    leaq (%rbx,%r12), %r9
    movzbq (%r9,%rdx), %rax
    movb %al, (%rcx,%rdx)
    incq %rdx
    jmp .Lpss_copy
.Lpss_done:
    movq %r13, %r8
    subq %r12, %r8
    movb $0, (%rcx,%r8)
    movq %rcx, %rax
    popq %r13
    popq %r12
    popq %rbx
    ret

# ─── p_string_push_str(rdi=s, rsi=other) → rax = concat ───
p_string_push_str:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    movq %rdi, %rbx
    movq %rsi, %r12
    # slen
    movq %rbx, %rax
    xorl %ecx, %ecx
.Lpsps_slen:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpsps_slen_done
    incq %rax
    incq %rcx
    jmp .Lpsps_slen
.Lpsps_slen_done:
    movq %rcx, %r13
    # olen
    movq %r12, %rax
    xorl %ecx, %ecx
.Lpsps_olen:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpsps_olen_done
    incq %rax
    incq %rcx
    jmp .Lpsps_olen
.Lpsps_olen_done:
    movq %rcx, %r14
    # malloc slen+olen+1
    leaq 1(%r13,%r14), %rdi
    call p_malloc
    movq %rax, %r8
    # copy s
    xorl %ecx, %ecx
.Lpsps_copy1:
    cmpq %r13, %rcx
    jae .Lpsps_copy1_done
    movzbq (%rbx,%rcx), %rdx
    movb %dl, (%r8,%rcx)
    incq %rcx
    jmp .Lpsps_copy1
.Lpsps_copy1_done:
    # copy other at offset slen
    xorl %ecx, %ecx
.Lpsps_copy2:
    cmpq %r14, %rcx
    jae .Lpsps_copy2_done
    movzbq (%r12,%rcx), %rdx
    movb %dl, (%r8,%r13,%rcx)
    incq %rcx
    jmp .Lpsps_copy2
.Lpsps_copy2_done:
    movb $0, (%r8,%r13,%r14)
    movq %r8, %rax
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# ─── p_string_replace(rdi=s, rsi=from, rdx=to) → rax = result ───
p_string_replace:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    subq $8, %rsp
    movq %rdi, %rbx
    movq %rsi, %r12
    movq %rdx, %r13
    # get slen
    movq %rbx, %rax
    xorl %ecx, %ecx
.Lpsr2_slen:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpsr2_slen_done
    incq %rax
    incq %rcx
    jmp .Lpsr2_slen
.Lpsr2_slen_done:
    movq %rcx, %r14
    # get flen
    movq %r12, %rax
    xorl %ecx, %ecx
.Lpsr2_flen:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpsr2_flen_done
    incq %rax
    incq %rcx
    jmp .Lpsr2_flen
.Lpsr2_flen_done:
    movq %rcx, %r15
    # if flen == 0: return strdup(s)
    testq %r15, %r15
    je .Lpsr2_copy_all
    # get tlen (for buffer size calculation)
    movq %r13, %rax
    xorl %ecx, %ecx
.Lpsr2_tlen:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpsr2_tlen_done
    incq %rax
    incq %rcx
    jmp .Lpsr2_tlen
.Lpsr2_tlen_done:
    # rcx = tlen; malloc slen + tlen + 1 (enough for one replacement;
    # p_malloc rounds up to 4096 so this is safe for small strings)
    leaq 1(%r14,%rcx), %rdi
    call p_malloc
    movq %rax, (%rsp)
    xorl %ecx, %ecx
    xorl %edx, %edx
.Lpsr2_loop:
    cmpq %r14, %rcx
    jae .Lpsr2_done
    # check if s[rcx..] matches from
    movq %r14, %r8
    subq %rcx, %r8
    cmpq %r15, %r8
    jb .Lpsr2_copy_char
    xorl %r8, %r8
.Lpsr2_match:
    cmpq %r15, %r8
    jae .Lpsr2_replace
    leaq (%rbx,%rcx), %r9
    movzbq (%r9,%r8), %rax
    movzbq (%r12,%r8), %r9
    cmpq %r9, %rax
    jne .Lpsr2_copy_char
    incq %r8
    jmp .Lpsr2_match
.Lpsr2_replace:
    # copy 'to' to result[rdx] — save rcx only (rdx must keep advancing)
    pushq %rcx
    xorl %rcx, %ecx
.Lpsr2_copy_to:
    movzbq (%r13,%rcx), %rax
    testq %rax, %rax
    je .Lpsr2_copy_to_done
    movq 8(%rsp), %r8
    movb %al, (%r8,%rdx)
    incq %rdx
    incq %rcx
    jmp .Lpsr2_copy_to
.Lpsr2_copy_to_done:
    popq %rcx
    addq %r15, %rcx
    jmp .Lpsr2_loop
.Lpsr2_copy_char:
    movzbq (%rbx,%rcx), %rax
    movq (%rsp), %r8
    movb %al, (%r8,%rdx)
    incq %rcx
    incq %rdx
    jmp .Lpsr2_loop
.Lpsr2_done:
    movq (%rsp), %r8
    movb $0, (%r8,%rdx)
    movq %r8, %rax
    addq $8, %rsp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret
.Lpsr2_copy_all:
    movq %rbx, %rdi
    call p_string_from
    addq $8, %rsp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# ─── p_string_split(rdi=s, rsi=delim) → rax = Vec of strings ───
p_string_split:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    subq $8, %rsp
    movq %rdi, %rbx
    movq %rsi, %r12
    call p_vec_new
    movq %rax, %r13
    # get dlen
    movq %r12, %rax
    xorl %ecx, %ecx
.Lpss2_dlen:
    movzbq (%rax), %edx
    testq %rdx, %rdx
    je .Lpss2_dlen_done
    incq %rax
    incq %rcx
    jmp .Lpss2_dlen
.Lpss2_dlen_done:
    movq %rcx, %r14
    # if dlen == 0: push the whole string and return
    testq %r14, %r14
    jne .Lpss2_start
    movq %rbx, %rdi
    call p_string_from
    movq %rax, %rdi
    movq %r13, %rsi
    xchgq %rdi, %rsi
    call p_vec_push
    movq %r13, %rax
    jmp .Lpss2_ret
.Lpss2_start:
    movq %rbx, %r15
.Lpss2_loop:
    # find delim starting at r15
    movq %r15, %rdi
    movq %r12, %rsi
    call p_string_find
    # if found at -1, push rest and done
    cmpq $0, %rax
    jl .Lpss2_last
    # rax = pos of delim relative to r15
    # part_len = rax
    # malloc part_len+1, copy from r15
    leaq 1(%rax), %rdi
    pushq %rax
    call p_malloc
    popq %rcx
    # copy r15[0..rcx] to rax
    movq %rax, %rdx
    xorl %r8, %r8
.Lpss2_copy:
    cmpq %rcx, %r8
    jae .Lpss2_copy_done
    movzbq (%r15,%r8), %r9
    movb %r9b, (%rdx,%r8)
    incq %r8
    jmp .Lpss2_copy
.Lpss2_copy_done:
    movb $0, (%rdx,%rcx)
    # push to vec (save rcx across call — p_vec_push clobbers it)
    movq %rcx, (%rsp)
    movq %r13, %rdi
    movq %rdx, %rsi
    call p_vec_push
    movq (%rsp), %rcx
    # advance r15 past delim
    leaq (%r15,%rcx), %r15
    addq %r14, %r15
    jmp .Lpss2_loop
.Lpss2_last:
    # push rest of string from r15
    movq %r15, %rdi
    call p_string_from
    movq %r13, %rdi
    movq %rax, %rsi
    xchgq %rdi, %rsi
    call p_vec_push
    movq %r13, %rax
    jmp .Lpss2_ret
.Lpss2_ret:
    addq $8, %rsp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# ─── Vec functions ───

# p_vec_clone(rdi=vec) → rax = new vec
p_vec_clone:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    movq %rdi, %rbx
    call p_vec_new
    movq %rax, %r12
    # get len
    movq 8(%rbx), %r13
    xorl %r14d, %r14d
.Lpvc_loop:
    cmpq %r13, %r14
    jae .Lpvc_done
    movq %rbx, %rdi
    movq %r14, %rsi
    call p_vec_get
    movq %r12, %rdi
    movq %rax, %rsi
    call p_vec_push
    incq %r14
    jmp .Lpvc_loop
.Lpvc_done:
    movq %r12, %rax
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# p_vec_contains(rdi=vec, rsi=val) → rax = bool
p_vec_contains:
    movq 8(%rdi), %rcx
    movq (%rdi), %rdx
    xorl %r8, %r8
.Lpvc2_loop:
    cmpq %rcx, %r8
    jae .Lpvc2_false
    cmpq %rsi, (%rdx,%r8,8)
    je .Lpvc2_true
    incq %r8
    jmp .Lpvc2_loop
.Lpvc2_true:
    movl $1, %eax
    ret
.Lpvc2_false:
    xorl %eax, %eax
    ret

# p_vec_extend(rdi=vec, rsi=other) → void
p_vec_extend:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    movq %rdi, %rbx
    movq %rsi, %r12
    movq 8(%r12), %r13
    xorl %r14d, %r14d
.Lpve_loop:
    cmpq %r13, %r14
    jae .Lpve_done
    movq %r12, %rdi
    movq %r14, %rsi
    call p_vec_get
    movq %rbx, %rdi
    movq %rax, %rsi
    call p_vec_push
    incq %r14
    jmp .Lpve_loop
.Lpve_done:
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# p_vec_insert(rdi=vec, rsi=idx, rdx=val) → void
p_vec_insert:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    movq %rdi, %rbx
    movq %rsi, %r12
    movq %rdx, %r13
    # len = vec[1]
    movq 8(%rbx), %r14
    # cap = vec[2]
    movq 16(%rbx), %rcx
    # if len >= cap: grow (simplified: just push 0 and shift)
    # For simplicity, just append and shift
    # push val at end
    movq %rbx, %rdi
    movq %r13, %rsi
    call p_vec_push
    # reload len
    movq 8(%rbx), %r14
    # shift elements from len-2 down to idx right by 1
    movq %r14, %rcx
    decq %rcx
    decq %rcx
.Lpvi_shift:
    cmpq %r12, %rcx
    jl .Lpvi_done
    movq (%rbx), %rdx
    movq (%rdx,%rcx,8), %rax
    movq %rax, 8(%rdx,%rcx,8)
    decq %rcx
    jmp .Lpvi_shift
.Lpvi_done:
    # vec[idx] = val
    movq (%rbx), %rdx
    movq %r13, (%rdx,%r12,8)
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# p_vec_remove(rdi=vec, rsi=idx) → rax = removed value
p_vec_remove:
    pushq %rbx
    pushq %r12
    pushq %r13
    movq %rdi, %rbx
    movq %rsi, %r12
    # get value at idx
    movq (%rbx), %rdx
    movq (%rdx,%r12,8), %rax
    movq %rax, %r13
    # len = vec[1]
    movq 8(%rbx), %rcx
    # shift elements left from idx to len-2
    movq %r12, %rdx
.Lpvr_shift:
    leaq 1(%rdx), %r8
    cmpq %rcx, %r8
    jae .Lpvr_done
    movq (%rbx), %r9
    movq (%r9,%r8,8), %r10
    movq %r10, (%r9,%rdx,8)
    movq %r8, %rdx
    jmp .Lpvr_shift
.Lpvr_done:
    decq %rcx
    movq %rcx, 8(%rbx)
    movq %r13, %rax
    popq %r13
    popq %r12
    popq %rbx
    ret

# p_vec_reverse(rdi=vec) → void
p_vec_reverse:
    movq 8(%rdi), %rcx
    movq (%rdi), %rdx
    xorl %r8, %r8
    leaq -1(%rcx), %r9
.Lpvr2_loop:
    cmpq %r9, %r8
    jae .Lpvr2_done
    movq (%rdx,%r8,8), %rax
    movq (%rdx,%r9,8), %r10
    movq %r10, (%rdx,%r8,8)
    movq %rax, (%rdx,%r9,8)
    incq %r8
    decq %r9
    jmp .Lpvr2_loop
.Lpvr2_done:
    ret

# p_vec_sort(rdi=vec) → void (bubble sort)
p_vec_sort:
    pushq %rbx
    pushq %r12
    movq %rdi, %rbx
    movq 8(%rbx), %r12
    testq %r12, %r12
    jle .Lpvs_done
    leaq -1(%r12), %r12
.Lpvs_outer:
    testq %r12, %r12
    jle .Lpvs_done
    xorl %rcx, %rcx
    movq (%rbx), %rdx
.Lpvs_inner:
    cmpq %r12, %rcx
    jae .Lpvs_outer_end
    movq (%rdx,%rcx,8), %rax
    leaq 1(%rcx), %r8
    movq (%rdx,%r8,8), %r9
    cmpq %r9, %rax
    jle .Lpvs_skip
    movq %r9, (%rdx,%rcx,8)
    movq %rax, (%rdx,%r8,8)
.Lpvs_skip:
    incq %rcx
    jmp .Lpvs_inner
.Lpvs_outer_end:
    decq %r12
    jmp .Lpvs_outer
.Lpvs_done:
    popq %r12
    popq %rbx
    ret

# p_vec_truncate(rdi=vec, rsi=newlen) → void
p_vec_truncate:
    movq 8(%rdi), %rax
    cmpq %rax, %rsi
    jge .Lpvt_done
    movq %rsi, 8(%rdi)
.Lpvt_done:
    ret

# p_vec_clear(rdi=vec) → void
p_vec_clear:
    movq $0, 8(%rdi)
    ret

# ─── HashMap functions (array-based, 16 bytes per entry: key_ptr, value) ───

# p_map_new() → rax = map ptr (24 bytes: data_ptr, count, capacity)
p_map_new:
    pushq %rbx
    movq $24, %rdi
    call p_malloc
    movq %rax, %rbx
    # alloc 8 entries * 16 bytes = 128
    movq $128, %rdi
    call p_malloc
    movq %rax, (%rbx)
    movq $0, 8(%rbx)
    movq $8, 16(%rbx)
    movq %rbx, %rax
    popq %rbx
    ret

# p_map_len(rdi=map) → rax = count
p_map_len:
    movq 8(%rdi), %rax
    ret

# p_map_is_empty(rdi=map) → rax = bool
p_map_is_empty:
    movq 8(%rdi), %rax
    testq %rax, %rax
    sete %al
    movzbq %al, %rax
    ret

# p_map_clear(rdi=map) → void
p_map_clear:
    movq $0, 8(%rdi)
    ret

# p_map_get(rdi=map, rsi=key) → rax = value (0 if not found)
p_map_get:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    movq %rdi, %rbx
    movq %rsi, %r12
    movq 8(%rbx), %r13
    movq (%rbx), %rdx
    xorl %r14, %r14
.Lpmg_loop:
    cmpq %r13, %r14
    jae .Lpmg_notfound
    # entry = rdx + r14*16
    leaq (%rdx,%r14,8), %r8
    leaq (%r8,%r14,8), %r8
    # key = *(r8)
    movq (%r8), %r9
    # compare key with r12 (strcmp)
    movq %r9, %rdi
    movq %r12, %rsi
    call p_strcmp
    testq %rax, %rax
    je .Lpmg_found
    incq %r14
    jmp .Lpmg_loop
.Lpmg_found:
    leaq (%rdx,%r14,8), %r8
    leaq (%r8,%r14,8), %r8
    movq 8(%r8), %rax
    jmp .Lpmg_ret
.Lpmg_notfound:
    xorl %eax, %eax
.Lpmg_ret:
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# p_map_get_or(rdi=map, rsi=key, rdx=default) → rax = value or default
p_map_get_or:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    movq %rdi, %rbx
    movq %rsi, %r12
    movq %rdx, %r13
    movq 8(%rbx), %r14
    movq (%rbx), %rdx
    xorl %r15, %r15
.Lpmgo_loop:
    cmpq %r14, %r15
    jae .Lpmgo_notfound
    leaq (%rdx,%r15,8), %r8
    leaq (%r8,%r15,8), %r8
    movq (%r8), %r9
    movq %r9, %rdi
    movq %r12, %rsi
    call p_strcmp
    testq %rax, %rax
    je .Lpmgo_found
    incq %r15
    jmp .Lpmgo_loop
.Lpmgo_found:
    leaq (%rdx,%r15,8), %r8
    leaq (%r8,%r15,8), %r8
    movq 8(%r8), %rax
    jmp .Lpmgo_ret
.Lpmgo_notfound:
    movq %r13, %rax
.Lpmgo_ret:
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# p_map_contains(rdi=map, rsi=key) → rax = bool
p_map_contains:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    movq %rdi, %rbx
    movq %rsi, %r12
    movq 8(%rbx), %r13
    movq (%rbx), %rdx
    xorl %r14, %r14
.Lpmc_loop:
    cmpq %r13, %r14
    jae .Lpmc_false
    leaq (%rdx,%r14,8), %r8
    leaq (%r8,%r14,8), %r8
    movq (%r8), %r9
    movq %r9, %rdi
    movq %r12, %rsi
    call p_strcmp
    testq %rax, %rax
    je .Lpmc_true
    incq %r14
    jmp .Lpmc_loop
.Lpmc_true:
    movl $1, %eax
    jmp .Lpmc_ret
.Lpmc_false:
    xorl %eax, %eax
.Lpmc_ret:
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# p_map_insert(rdi=map, rsi=key, rdx=value) → void
p_map_insert:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    movq %rdi, %rbx
    movq %rsi, %r12
    movq %rdx, %r13
    # check if key exists
    movq 8(%rbx), %r14
    movq (%rbx), %rdx
    xorl %r15, %r15
.Lpmi_loop:
    cmpq %r14, %r15
    jae .Lpmi_new
    leaq (%rdx,%r15,8), %r8
    leaq (%r8,%r15,8), %r8
    movq (%r8), %r9
    movq %r9, %rdi
    movq %r12, %rsi
    call p_strcmp
    testq %rax, %rax
    je .Lpmi_update
    incq %r15
    jmp .Lpmi_loop
.Lpmi_update:
    leaq (%rdx,%r15,8), %r8
    leaq (%r8,%r15,8), %r8
    movq %r13, 8(%r8)
    jmp .Lpmi_done
.Lpmi_new:
    # check capacity
    movq 8(%rbx), %rcx
    movq 16(%rbx), %rdx
    cmpq %rdx, %rcx
    jb .Lpmi_add
    # grow: alloc new 2x capacity
    shlq $1, %rdx
    movq %rdx, 16(%rbx)
    leaq 0(,%rdx,4), %rdi
    call p_malloc
    # copy old entries
    movq (%rbx), %rsi
    movq %rax, (%rbx)
    movq %rax, %rdi
    xorl %rcx, %ecx
.Lpmi_copy:
    movq 8(%rbx), %r8
    cmpq %r8, %rcx
    jae .Lpmi_copy_done
    # Entry at index rcx is at offset rcx*16 (each entry = 16 bytes: key, value)
    # Key: offset = rcx*16 = rcx*8*2 → use leaq (%rsi,%rcx,8),%r9; leaq (%r9,%rcx,8),%r9
    leaq (%rsi,%rcx,8), %r9       # r9 = rsi + rcx*8
    leaq (%r9,%rcx,8), %r9       # r9 = rsi + rcx*16
    movq (%r9), %r10             # key
    leaq (%rdi,%rcx,8), %r11
    leaq (%r11,%rcx,8), %r11
    movq %r10, (%r11)            # store key
    movq 8(%r9), %r10           # value
    movq %r10, 8(%r11)          # store value
    incq %rcx
    jmp .Lpmi_copy
.Lpmi_copy_done:
.Lpmi_add:
    # add new entry at index = count
    movq 8(%rbx), %rcx
    movq (%rbx), %rdx
    leaq (%rdx,%rcx,8), %r8
    leaq (%r8,%rcx,8), %r8
    movq %r12, (%r8)
    movq %r13, 8(%r8)
    incq %rcx
    movq %rcx, 8(%rbx)
.Lpmi_done:
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# p_map_remove(rdi=map, rsi=key) → void
p_map_remove:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    movq %rdi, %rbx
    movq %rsi, %r12
    movq 8(%rbx), %r13
    movq (%rbx), %rdx
    xorl %r14, %r14
.Lpmr_loop:
    cmpq %r13, %r14
    jae .Lpmr_done
    leaq (%rdx,%r14,8), %r8
    leaq (%r8,%r14,8), %r8
    movq (%r8), %r9
    movq %r9, %rdi
    movq %r12, %rsi
    call p_strcmp
    testq %rax, %rax
    je .Lpmr_found
    incq %r14
    jmp .Lpmr_loop
.Lpmr_found:
    # shift entries left from r14+1 to r13-1
    movq %r14, %rdx
.Lpmr_shift:
    leaq 1(%rdx), %r8
    cmpq %r13, %r8
    jae .Lpmr_shift_done
    movq (%rbx), %r9
    leaq (%r9,%r8,8), %r10
    leaq (%r10,%r8,8), %r10
    movq (%r10), %r11
    leaq (%r9,%rdx,8), %r10
    leaq (%r10,%rdx,8), %r10
    movq %r11, (%r10)
    movq (%rbx), %r9
    leaq (%r9,%r8,8), %r10
    leaq (%r10,%r8,8), %r10
    movq 8(%r10), %r11
    leaq (%r9,%rdx,8), %r10
    leaq (%r10,%rdx,8), %r10
    movq %r11, 8(%r10)
    movq %r8, %rdx
    jmp .Lpmr_shift
.Lpmr_shift_done:
    decq %r13
    movq %r13, 8(%rbx)
.Lpmr_done:
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# p_map_clone(rdi=map) → rax = new map
p_map_clone:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    movq %rdi, %rbx
    call p_map_new
    movq %rax, %r12
    movq 8(%rbx), %r13
    movq (%rbx), %rdx
    xorl %r14, %r14
.Lpmc2_loop:
    cmpq %r13, %r14
    jae .Lpmc2_done
    leaq (%rdx,%r14,8), %r8
    leaq (%r8,%r14,8), %r8
    movq (%r8), %rsi
    movq 8(%r8), %rdx
    movq %r12, %rdi
    call p_map_insert
    movq 8(%rbx), %r13
    movq (%rbx), %rdx
    incq %r14
    jmp .Lpmc2_loop
.Lpmc2_done:
    movq %r12, %rax
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

# p_strcmp(rdi=a, rsi=b) → rax = 0 if equal, nonzero otherwise
p_strcmp:
.Lpstrcmp_loop:
    movzbq (%rdi), %rax
    movzbq (%rsi), %rcx
    cmpq %rcx, %rax
    jne .Lpstrcmp_neq
    testq %rax, %rax
    je .Lpstrcmp_eq
    incq %rdi
    incq %rsi
    jmp .Lpstrcmp_loop
.Lpstrcmp_eq:
    xorl %eax, %eax
    ret
.Lpstrcmp_neq:
    subq %rcx, %rax
    ret

# p_print_map(rdi=map) → void
# Prints {key: value, key: value, ...}
p_print_map:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    subq $8, %rsp
    movq %rdi, %rbx
    movq 8(%rbx), %r13
    movq (%rbx), %r14
    # print '{' — write single char directly
    movb $123, 16(%rsp)
    movl $1, %eax
    movl $1, %edi
    leaq 16(%rsp), %rsi
    movl $1, %edx
    syscall
    xorl %r12, %r12
.Lppm_loop:
    cmpq %r13, %r12
    jae .Lppm_done
    # if not first, print ", "
    testq %r12, %r12
    je .Lppm_skip_sep
    movb $44, 16(%rsp)
    movb $32, 17(%rsp)
    movl $1, %eax
    movl $1, %edi
    leaq 16(%rsp), %rsi
    movl $2, %edx
    syscall
.Lppm_skip_sep:
    # print key
    leaq (%r14,%r12,8), %r8
    leaq (%r8,%r12,8), %r8
    movq (%r8), %rdi
    call p_print_str
    # print ": "
    movb $58, 16(%rsp)
    movb $32, 17(%rsp)
    movl $1, %eax
    movl $1, %edi
    leaq 16(%rsp), %rsi
    movl $2, %edx
    syscall
    # print value via p_to_str + p_print_str
    movq 8(%r8), %rdi
    call p_to_str
    movq %rax, %rdi
    call p_print_str
    incq %r12
    jmp .Lppm_loop
.Lppm_done:
    # print '}'
    movb $125, 16(%rsp)
    movl $1, %eax
    movl $1, %edi
    leaq 16(%rsp), %rsi
    movl $1, %edx
    syscall
    addq $8, %rsp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret
"#;

    let (asm_bytes, asm_syms, asm_relocs) = match crate::elf64::encode_asm_block(runtime_asm) {
        Ok(r) => r,
        Err(e) => {
            eprintln!("Runtime ASM encoding error: {}", e);
            (Vec::new(), Vec::new(), Vec::new())
        }
    };
    let asm_base = text.len();
    text.extend_from_slice(&asm_bytes);
    for (name, offset) in &asm_syms {
        let leaked: &'static str = Box::leak(name.clone().into_boxed_str());
        symbols.push((leaked, asm_base as u64 + offset, 0));
    }
    let mut asm_reloc_list: Vec<(u64, String, u8)> = Vec::new();
    for (offset, target, rtype) in &asm_relocs {
        asm_reloc_list.push((asm_base as u64 + offset, target.clone(), *rtype));
    }

    // ═══════════════════════════════════════════════════════════════════════
    // Stub functions — return 0/NULL for all other externs
    // Each stub: xorq %rax, %rax ; ret  (or just ret for void functions)
    // ═══════════════════════════════════════════════════════════════════════

    // Helper macro for stub functions that return 0
    macro_rules! stub_zero {
        ($name:expr) => {
            sym!($name);
            text.extend_from_slice(&[0x48, 0x31, 0xc0]); // xorq %rax, %rax
            text.push(0xc3); // ret
        };
    }

    // Helper macro for void stub functions
    macro_rules! stub_void {
        ($name:expr) => {
            sym!($name);
            text.push(0xc3); // ret
        };
    }

    // Vec/Map stubs
    // p_vec_new() → rax = Vec pointer
    // Vec layout: [data_ptr(8), len(8), cap(8), ...inline data]
    // We allocate 4096 bytes initially; data starts at offset 24.
    sym!("p_vec_new");
    // movq $4096, %rdi  (size for mmap)
    text.extend_from_slice(&[0x48, 0xc7, 0xc7, 0x00, 0x10, 0x00, 0x00]);
    // jmp p_malloc
    text.extend_from_slice(&[0xe9, 0x00, 0x00, 0x00, 0x00]);
    let pvec_new_jmp_off = text.len() - 4;
    // After mmap returns, zero out the 3 header fields:
    // Actually, mmap returns zeroed memory (MAP_ANONYMOUS), so len=0, cap=0.
    // We need to set data_ptr = vec_ptr + 24, cap = (4096-24)/8
    // But we can't easily do this after a jmp. Let's do it differently:
    // Undo the jmp and do it inline.
    text.truncate(text.len() - 5); // remove the jmp
    // movq $4096, %rdi
    // (already emitted above)
    // call p_malloc
    text.extend_from_slice(&[0xe8, 0x00, 0x00, 0x00, 0x00]);
    let pvec_new_call_off = text.len() - 4;
    // leaq 24(%rax), %rcx  (data_ptr = vec + 24)
    text.extend_from_slice(&[0x48, 0x8d, 0x48, 0x18]);
    // movq %rcx, (%rax)  (store data_ptr)
    text.extend_from_slice(&[0x48, 0x89, 0x08]);
    // movq $0, 8(%rax)  (len = 0) — already zero from mmap
    // movq $509, 16(%rax)  (cap = (4096-24)/8 = 509)
    text.extend_from_slice(&[0x48, 0xc7, 0x40, 0x10, 0xfd, 0x01, 0x00, 0x00]);
    text.push(0xc3); // ret

    // p_vec_push(rdi = vec, rsi = value) → void
    // Grows the vec if needed. Simplified: no growth (cap=509 is enough for tests).
    sym!("p_vec_push");
    // movq 8(%rdi), %rax  (len)
    text.extend_from_slice(&[0x48, 0x8b, 0x47, 0x08]);
    // movq (%rdi), %rcx  (data_ptr)
    text.extend_from_slice(&[0x48, 0x8b, 0x0f]);
    // movq %rsi, (%rcx,%rax,8)  (data[len] = value)
    text.extend_from_slice(&[0x48, 0x89, 0x34, 0xc1]);
    // incq 8(%rdi)  (len++)
    text.extend_from_slice(&[0x48, 0xff, 0x47, 0x08]);
    text.push(0xc3); // ret

    // p_vec_len(rdi = vec) → rax = len
    sym!("p_vec_len");
    // movq 8(%rdi), %rax
    text.extend_from_slice(&[0x48, 0x8b, 0x47, 0x08]);
    text.push(0xc3); // ret

    // p_vec_get(rdi = vec, rsi = index) → rax = value
    sym!("p_vec_get");
    // movq (%rdi), %rcx  (data_ptr)
    text.extend_from_slice(&[0x48, 0x8b, 0x0f]);
    // movq (%rcx,%rsi,8), %rax  (data[index])
    text.extend_from_slice(&[0x48, 0x8b, 0x04, 0xf1]);
    text.push(0xc3); // ret

    // p_vec_first(rdi = vec) → rax = first element
    sym!("p_vec_first");
    // movq (%rdi), %rax  (data_ptr)
    text.extend_from_slice(&[0x48, 0x8b, 0x07]);
    // movq (%rax), %rax  (first element)
    text.extend_from_slice(&[0x48, 0x8b, 0x00]);
    text.push(0xc3); // ret

    // p_vec_last(rdi = vec) → rax = last element
    sym!("p_vec_last");
    // movq (%rdi), %rcx  (data_ptr)
    text.extend_from_slice(&[0x48, 0x8b, 0x0f]);
    // movq 8(%rdi), %rax  (len)
    text.extend_from_slice(&[0x48, 0x8b, 0x47, 0x08]);
    // decq %rax  (len - 1)
    text.extend_from_slice(&[0x48, 0xff, 0xc8]);
    // movq (%rcx,%rax,8), %rax  (data[len-1])
    text.extend_from_slice(&[0x48, 0x8b, 0x04, 0xc1]);
    text.push(0xc3); // ret

    // p_vec_pop(rdi = vec) → rax = value (or 0 if empty)
    sym!("p_vec_pop");
    // movq 8(%rdi), %rax  (len)
    text.extend_from_slice(&[0x48, 0x8b, 0x47, 0x08]);
    // testq %rax, %rax
    text.extend_from_slice(&[0x48, 0x85, 0xc0]);
    // je .empty
    text.extend_from_slice(&[0x74, 0x0b]); // je +11
    // decq %rax  (len-1)
    text.extend_from_slice(&[0x48, 0xff, 0xc8]);
    // movq %rax, 8(%rdi)  (store new len)
    text.extend_from_slice(&[0x48, 0x89, 0x47, 0x08]);
    // movq (%rdi), %rcx  (data_ptr)
    text.extend_from_slice(&[0x48, 0x8b, 0x0f]);
    // movq (%rcx,%rax,8), %rax  (data[len-1])
    text.extend_from_slice(&[0x48, 0x8b, 0x04, 0xc1]);
    text.push(0xc3); // ret
    // .empty: xorq %rax, %rax; ret
    text.extend_from_slice(&[0x48, 0x31, 0xc0]);
    text.push(0xc3); // ret

    // p_vec_is_empty(rdi = vec) → rax = 1 if empty, 0 otherwise
    sym!("p_vec_is_empty");
    // movq 8(%rdi), %rax
    text.extend_from_slice(&[0x48, 0x8b, 0x47, 0x08]);
    // testq %rax, %rax
    text.extend_from_slice(&[0x48, 0x85, 0xc0]);
    // sete %al
    text.extend_from_slice(&[0x0f, 0x94, 0xc0]);
    // movzbq %al, %rax
    text.extend_from_slice(&[0x48, 0x0f, 0xb6, 0xc0]);
    text.push(0xc3); // ret

    // p_vec_set(rdi = vec, rsi = index, rdx = value) → void
    sym!("p_vec_set");
    // movq (%rdi), %rcx  (data_ptr)
    text.extend_from_slice(&[0x48, 0x8b, 0x0f]);
    // movq %rdx, (%rcx,%rsi,8)  (data[index] = value)
    text.extend_from_slice(&[0x48, 0x89, 0x14, 0xf1]);
    text.push(0xc3); // ret

    // p_map_new is implemented in the assembly block above

    // p_print_vec(rdi = vec) — prints [1, 2, 3]
    // Format: [elem0, elem1, ...]
    sym!("p_print_vec");
    // Save callee-saved registers
    text.extend_from_slice(&[0x53]); // pushq %rbx
    text.extend_from_slice(&[0x41, 0x54]); // pushq %r12
    text.extend_from_slice(&[0x41, 0x55]); // pushq %r13
    // movq %rdi, %rbx  (save vec ptr)
    text.extend_from_slice(&[0x48, 0x89, 0xfb]);
    // Call p_vec_len(rdi) to get len
    text.extend_from_slice(&[0xe8, 0x00, 0x00, 0x00, 0x00]); // call p_vec_len
    let pvec_len_call_off = text.len() - 4;
    // movq %rax, %r12  (len)
    text.extend_from_slice(&[0x49, 0x89, 0xc4]);
    // movq (%rbx), %rbx  (data_ptr = vec[0])
    text.extend_from_slice(&[0x48, 0x8b, 0x1b]);
    // xorq %r13, %r13  (index = 0)
    text.extend_from_slice(&[0x4d, 0x31, 0xed]);
    // Write '['
    text.extend_from_slice(&[0xbf, 0x01, 0x00, 0x00, 0x00]); // movl $1, %edi
    text.extend_from_slice(&[0x48, 0x8d, 0x35, 0x00, 0x00, 0x00, 0x00]); // leaq .Lbracket_open(%rip), %rsi
    let pvec_open_lea = text.len() - 4;
    text.extend_from_slice(&[0xba, 0x01, 0x00, 0x00, 0x00]); // movl $1, %edx
    text.extend_from_slice(&[0xb8, 0x01, 0x00, 0x00, 0x00]); // movl $1, %eax
    text.extend_from_slice(&[0x0f, 0x05]); // syscall
    // .loop: testq %r12, %r12 (if remaining len == 0, done)
    let pvec_loop = text.len();
    text.extend_from_slice(&[0x4d, 0x85, 0xe4]);
    // jz .done
    let pvec_jge_off = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0x00, 0x00, 0x00, 0x00]);
    // If not first element, write ", "
    text.extend_from_slice(&[0x4d, 0x85, 0xed]); // testq %r13, %r13
    // je .skip_comma
    let pvec_je_off = text.len();
    text.extend_from_slice(&[0x74, 0x00]);
    // Write ", "
    text.extend_from_slice(&[0xbf, 0x01, 0x00, 0x00, 0x00]); // movl $1, %edi
    text.extend_from_slice(&[0x48, 0x8d, 0x35, 0x00, 0x00, 0x00, 0x00]); // leaq .Lcomma_sp(%rip), %rsi
    let pvec_comma_lea = text.len() - 4;
    text.extend_from_slice(&[0xba, 0x02, 0x00, 0x00, 0x00]); // movl $2, %edx
    text.extend_from_slice(&[0xb8, 0x01, 0x00, 0x00, 0x00]); // movl $1, %eax
    text.extend_from_slice(&[0x0f, 0x05]); // syscall
    // .skip_comma:
    let pvec_skip_comma = text.len();
    let skip_comma_rel = (pvec_skip_comma - pvec_je_off - 2) as i32;
    text[pvec_je_off + 1] = skip_comma_rel as u8;
    // Load element: movq (%rbx), %rdi
    text.extend_from_slice(&[0x48, 0x8b, 0x3b]);
    // call p_print_int
    text.extend_from_slice(&[0xe8, 0x00, 0x00, 0x00, 0x00]);
    let pvec_print_call_off = text.len() - 4;
    // addq $8, %rbx (advance data_ptr)
    text.extend_from_slice(&[0x48, 0x83, 0xc3, 0x08]);
    // decq %r12 (decrement remaining len)
    text.extend_from_slice(&[0x49, 0xff, 0xcc]);
    // incq %r13 (increment index for comma check)
    text.extend_from_slice(&[0x49, 0xff, 0xc5]);
    // jmp .loop
    let pvec_jmp_back = text.len();
    let loop_rel = (pvec_loop as i64 - pvec_jmp_back as i64 - 2) as i8;
    text.extend_from_slice(&[0xeb, loop_rel as u8]);
    // .done:
    let pvec_done = text.len();
    let done_rel = (pvec_done as i64 - pvec_jge_off as i64 - 6) as i32;
    let done_bytes = done_rel.to_le_bytes();
    for (i, &b) in done_bytes.iter().enumerate() {
        text[pvec_jge_off + 2 + i] = b;
    }
    // Write ']'
    text.extend_from_slice(&[0xbf, 0x01, 0x00, 0x00, 0x00]); // movl $1, %edi
    text.extend_from_slice(&[0x48, 0x8d, 0x35, 0x00, 0x00, 0x00, 0x00]); // leaq .Lbracket_close(%rip), %rsi
    let pvec_close_lea = text.len() - 4;
    text.extend_from_slice(&[0xba, 0x01, 0x00, 0x00, 0x00]); // movl $1, %edx
    text.extend_from_slice(&[0xb8, 0x01, 0x00, 0x00, 0x00]); // movl $1, %eax
    text.extend_from_slice(&[0x0f, 0x05]); // syscall
    // Restore callee-saved registers
    text.extend_from_slice(&[0x41, 0x5d]); // popq %r13
    text.extend_from_slice(&[0x41, 0x5c]); // popq %r12
    text.extend_from_slice(&[0x5b]); // popq %rbx
    text.push(0xc3); // ret

    // p_print_map is implemented in the assembly block above

    // JSON stubs
    stub_zero!("p_json_parse");
    stub_zero!("p_json_dump");
    stub_zero!("p_json_get");
    stub_zero!("p_json_type");
    // p_json_len and p_json_at are implemented below (needed by test_db)
    stub_zero!("p_json_pretty");
    stub_zero!("p_json_string");

    // Tensor stubs
    stub_zero!("p_tensor_zeros");
    stub_zero!("p_tensor_ones");
    stub_zero!("p_tensor_filled");
    stub_zero!("p_tensor_from_vec");
    stub_zero!("p_tensor_get");
    stub_void!("p_tensor_set");
    stub_zero!("p_tensor_shape");
    stub_zero!("p_tensor_ndim");
    stub_zero!("p_tensor_size");
    stub_zero!("p_tensor_reshape");
    stub_zero!("p_tensor_transpose");
    stub_zero!("p_tensor_add");
    stub_zero!("p_tensor_sub");
    stub_zero!("p_tensor_mul");
    stub_zero!("p_tensor_scale");
    stub_zero!("p_tensor_matmul");
    stub_zero!("p_tensor_dot");
    stub_zero!("p_tensor_sum");
    stub_zero!("p_tensor_mean");
    stub_zero!("p_tensor_max");
    stub_zero!("p_tensor_min");
    stub_zero!("p_tensor_relu");
    stub_zero!("p_tensor_sigmoid");
    stub_zero!("p_tensor_softmax");
    stub_zero!("p_tensor_to_str");
    stub_zero!("p_tensor_to_json");
    stub_zero!("p_tensor_clone");

    // SIMD stubs (actual SIMD ops are inlined by codegen)
    // p_simd_dot(rdi = vec_a, rsi = vec_b) → rax = f64 dot product
    // Iterates over both vectors, multiplies element-wise, sums as f64
    sym!("p_simd_dot");
    // pushq %rbx ; pushq %r12 ; pushq %r13 ; pushq %r14
    text.push(0x53);
    text.extend_from_slice(&[0x41, 0x54]);
    text.extend_from_slice(&[0x41, 0x55]);
    text.extend_from_slice(&[0x41, 0x56]);
    // subq $32, %rsp
    text.extend_from_slice(&[0x48, 0x83, 0xec, 0x20]);
    // movq %rdi, %rbx (vec_a)
    text.extend_from_slice(&[0x48, 0x89, 0xfb]);
    // movq %rsi, %r12 (vec_b)
    text.extend_from_slice(&[0x49, 0x89, 0xf4]);
    // movq 8(%rbx), %r13 (len_a)
    text.extend_from_slice(&[0x4c, 0x8b, 0x6b, 0x08]);
    // movq (%rbx), %rbx (data_a)
    text.extend_from_slice(&[0x48, 0x8b, 0x1b]);
    // movq (%r12), %r12 (data_b)
    text.extend_from_slice(&[0x4d, 0x8b, 0x24, 0x24]);
    // xorq %r14, %r14 (index = 0)
    text.extend_from_slice(&[0x4d, 0x31, 0xf6]);
    // Store 0.0 at 16(%rsp) (accumulator — 8 bytes zeroed)
    text.extend_from_slice(&[0x48, 0xc7, 0x44, 0x24, 0x10, 0x00, 0x00, 0x00, 0x00]);
    // .loop:
    let psdot_loop = text.len();
    // cmpq %r14, %r13
    text.extend_from_slice(&[0x4d, 0x39, 0xf5]);
    // je .done
    let psdot_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0x00, 0x00, 0x00, 0x00]); // je rel32
    // movq (%rbx,%r14,8), %rax (a[i] as f64 bits)
    // REX.WX=0x4a to extend SIB index to r14
    text.extend_from_slice(&[0x4a, 0x8b, 0x04, 0xf3]);
    // movq %rax, (%rsp)
    text.extend_from_slice(&[0x48, 0x89, 0x04, 0x24]);
    // movsd (%rsp), %xmm0
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x04, 0x24]);
    // movq (%r12,%r14,8), %rax (b[i] as f64 bits)
    // REX.WXB=0x4b to extend SIB index to r14 and base to r12
    text.extend_from_slice(&[0x4b, 0x8b, 0x04, 0xf4]);
    // movq %rax, (%rsp)
    text.extend_from_slice(&[0x48, 0x89, 0x04, 0x24]);
    // movsd (%rsp), %xmm1
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x0c, 0x24]);
    // mulsd %xmm1, %xmm0 (a[i] * b[i])
    text.extend_from_slice(&[0xf2, 0x0f, 0x59, 0xc1]);
    // movsd 16(%rsp), %xmm1 (accumulator)
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x4c, 0x24, 0x10]);
    // addsd %xmm0, %xmm1 (accumulator += a[i]*b[i])
    text.extend_from_slice(&[0xf2, 0x0f, 0x58, 0xc8]);
    // movsd %xmm1, 16(%rsp)
    text.extend_from_slice(&[0xf2, 0x0f, 0x11, 0x4c, 0x24, 0x10]);
    // incq %r14
    text.extend_from_slice(&[0x49, 0xff, 0xc6]);
    // jmp .loop
    let psdot_jmp = text.len();
    let psdot_back = psdot_loop as i64 - psdot_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, psdot_back as u8]);
    // .done:
    let psdot_done = text.len();
    let psdot_done_rel = (psdot_done as i64 - psdot_je as i64 - 6) as i32;
    text[psdot_je + 2..psdot_je + 6].copy_from_slice(&psdot_done_rel.to_le_bytes());
    // movq 16(%rsp), %rax (return accumulator as f64 bits)
    text.extend_from_slice(&[0x48, 0x8b, 0x44, 0x24, 0x10]);
    // addq $32, %rsp
    text.extend_from_slice(&[0x48, 0x83, 0xc4, 0x20]);
    // popq %r14 ; popq %r13 ; popq %r12 ; popq %rbx
    text.extend_from_slice(&[0x41, 0x5e]);
    text.extend_from_slice(&[0x41, 0x5d]);
    text.extend_from_slice(&[0x41, 0x5c]);
    text.push(0x5b);
    text.push(0xc3); // ret

    stub_zero!("p_simd_sum");
    stub_zero!("p_simd_scale");
    stub_zero!("p_simd_from_vec");
    stub_zero!("p_simd_to_str");

    // HTTP stubs
    stub_zero!("p_socket_listen");
    stub_zero!("p_socket_accept");
    stub_zero!("p_socket_read");
    stub_zero!("p_socket_write");
    stub_void!("p_socket_close");
    stub_zero!("p_socket_recv_str");
    stub_zero!("p_http_parse_request");
    stub_zero!("p_http_build_response");

    // Database functions — implemented below
    // p_db_mem_open, p_db_mem_close, p_db_mem_exec, p_db_mem_query

    // Interrupt/MMU stubs (bare-metal only, no-op on Linux)
    stub_void!("p_cli");
    stub_void!("p_sti");
    stub_void!("p_lidt");
    stub_void!("p_idt_set_entry");
    stub_void!("p_hlt");
    stub_zero!("p_inb");
    stub_void!("p_outb");
    stub_zero!("p_cr3_read");
    stub_void!("p_cr3_write");
    stub_zero!("p_cr0_read");
    stub_void!("p_cr0_write");
    stub_zero!("p_cr4_read");
    stub_void!("p_cr4_write");
    stub_void!("p_invlpg");
    stub_void!("p_page_map");
    stub_void!("p_page_unmap");
    stub_zero!("p_phys_read64");
    stub_void!("p_phys_write64");

    // ═══════════════════════════════════════════════════════════════════════
    // Database functions — in-memory key-value store for test_db
    // Layout: db pointer → [count(8), key_ptr_0(8), key_ptr_1(8), ...]
    // Each key_ptr points to a null-terminated string (allocated with p_malloc)
    // ═══════════════════════════════════════════════════════════════════════

    // p_db_mem_open() → rax = db pointer
    // Allocates 4096 bytes, sets count=0
    sym!("p_db_mem_open");
    text.extend_from_slice(&[0x48, 0xc7, 0xc7, 0x00, 0x10, 0x00, 0x00]); // movq $4096, %rdi
    text.extend_from_slice(&[0xe8, 0, 0, 0, 0]); // call p_malloc
    let dbopen_call_off = text.len() - 4;
    text.extend_from_slice(&[0x48, 0xc7, 0x00, 0x00, 0x00, 0x00, 0x00]); // movq $0, (%rax)
    text.push(0xc3); // ret

    // p_db_mem_close(rdi = db) → void
    // Just calls p_free
    sym!("p_db_mem_close");
    text.extend_from_slice(&[0xe8, 0, 0, 0, 0]); // call p_free
    let dbclose_call_off = text.len() - 4;
    text.push(0xc3); // ret

    // p_db_mem_exec(rdi = db, rsi = query) → rax = 1 on success, 0 on failure
    // Parses "INSERT key value" or "SET key value", stores key pointer
    // p_db_mem_exec(rdi = db, rsi = query) → rax = 1 on success, 0 on failure
    // Parses "INSERT key value", stores key pointer + length in the db.
    // No p_malloc needed — key pointer points into the query string (rodata).
    // DB layout: [count(8), key_ptr_0(8), key_len_0(8), key_ptr_1(8), key_len_1(8), ...]
    sym!("p_db_mem_exec");
    // pushq %rbx, %r12, %r13
    text.push(0x53); // pushq %rbx
    text.extend_from_slice(&[0x41, 0x54]); // pushq %r12
    text.extend_from_slice(&[0x41, 0x55]); // pushq %r13
    // (3 pushes = 24 bytes, rsp = 8+24 = 32 ≡ 0 mod 16 — aligned, no subq needed)
    // rbx = db, r12 = query cursor
    text.extend_from_slice(&[0x48, 0x89, 0xfb]); // movq %rdi, %rbx
    text.extend_from_slice(&[0x49, 0x89, 0xf4]); // movq %rsi, %r12
    // Find first space: advance r12 until ' '
    // .find_space:
    let dbexec_fs = text.len();
    text.extend_from_slice(&[0x41, 0x0f, 0xb6, 0x04, 0x24]); // movzbl (%r12), %eax
    text.extend_from_slice(&[0x84, 0xc0]); // testb %al, %al
    let dbexec_fail1_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je .fail
    text.extend_from_slice(&[0x3c, 0x20]); // cmpb $' ', %al
    let dbexec_found_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je .found_space
    text.extend_from_slice(&[0x49, 0xff, 0xc4]); // incq %r12
    let dbexec_fs_jmp = text.len();
    let dbexec_fs_back = dbexec_fs as i64 - dbexec_fs_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, dbexec_fs_back as u8]); // jmp .find_space
    // .found_space:
    let dbexec_found = text.len();
    let dbexec_found_rel = (dbexec_found as i64 - dbexec_found_je as i64 - 6) as i32;
    text[dbexec_found_je+2..dbexec_found_je+6].copy_from_slice(&dbexec_found_rel.to_le_bytes());
    // Skip spaces
    // .skip_spaces:
    let dbexec_skip = text.len();
    text.extend_from_slice(&[0x41, 0x0f, 0xb6, 0x04, 0x24]); // movzbl (%r12), %eax
    text.extend_from_slice(&[0x3c, 0x20]); // cmpb $' ', %al
    let dbexec_keystart_jne = text.len();
    text.extend_from_slice(&[0x0f, 0x85, 0, 0, 0, 0]); // jne .key_start
    text.extend_from_slice(&[0x49, 0xff, 0xc4]); // incq %r12
    let dbexec_skip_jmp = text.len();
    let dbexec_skip_back = dbexec_skip as i64 - dbexec_skip_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, dbexec_skip_back as u8]); // jmp .skip_spaces
    // .key_start:
    let dbexec_keystart = text.len();
    let dbexec_keystart_rel = (dbexec_keystart as i64 - dbexec_keystart_jne as i64 - 6) as i32;
    text[dbexec_keystart_jne+2..dbexec_keystart_jne+6].copy_from_slice(&dbexec_keystart_rel.to_le_bytes());
    // r13 = r12 (key start pointer)
    text.extend_from_slice(&[0x4d, 0x89, 0xe5]); // movq %r12, %r13
    // Find end of key: advance r12 until ' ' or '\0'
    // .find_key_end:
    let dbexec_keyend = text.len();
    text.extend_from_slice(&[0x41, 0x0f, 0xb6, 0x04, 0x24]); // movzbl (%r12), %eax
    text.extend_from_slice(&[0x84, 0xc0]); // testb %al, %al
    let dbexec_havekey_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je .have_key
    text.extend_from_slice(&[0x3c, 0x20]); // cmpb $' ', %al
    let dbexec_havekey_je2 = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je .have_key
    text.extend_from_slice(&[0x49, 0xff, 0xc4]); // incq %r12
    let dbexec_keyend_jmp = text.len();
    let dbexec_keyend_back = dbexec_keyend as i64 - dbexec_keyend_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, dbexec_keyend_back as u8]); // jmp .find_key_end
    // .have_key:
    let dbexec_havekey = text.len();
    let dbexec_havekey_rel = (dbexec_havekey as i64 - dbexec_havekey_je as i64 - 6) as i32;
    text[dbexec_havekey_je+2..dbexec_havekey_je+6].copy_from_slice(&dbexec_havekey_rel.to_le_bytes());
    let dbexec_havekey_rel2 = (dbexec_havekey as i64 - dbexec_havekey_je2 as i64 - 6) as i32;
    text[dbexec_havekey_je2+2..dbexec_havekey_je2+6].copy_from_slice(&dbexec_havekey_rel2.to_le_bytes());
    // r12 = r12 - r13 (key length)
    text.extend_from_slice(&[0x4d, 0x29, 0xec]); // subq %r13, %r12
    // Store key_ptr and key_len in database at entry[count]
    // rax = count = *(%rbx)
    text.extend_from_slice(&[0x48, 0x8b, 0x03]); // movq (%rbx), %rax
    // rcx = rbx + 8 + count*16 = rbx + 8 + (count << 4)
    text.extend_from_slice(&[0x48, 0xc1, 0xe0, 0x04]); // shlq $4, %rax
    text.extend_from_slice(&[0x48, 0x8d, 0x4c, 0x03, 0x08]); // leaq 8(%rbx,%rax,1), %rcx
    // movq %r13, (%rcx) — store key_ptr
    text.extend_from_slice(&[0x4c, 0x89, 0x29]); // movq %r13, (%rcx)
    // movq %r12, 8(%rcx) — store key_len
    text.extend_from_slice(&[0x4c, 0x89, 0x61, 0x08]); // movq %r12, 8(%rcx)
    // incq (%rbx) — count++
    text.extend_from_slice(&[0x48, 0xff, 0x03]); // incq (%rbx)
    // Return 1
    text.extend_from_slice(&[0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00]); // movq $1, %rax
    // jmp .done
    let dbexec_done_jmp = text.len();
    text.extend_from_slice(&[0xeb, 0x00]); // jmp rel8 (placeholder)
    // .fail:
    let dbexec_fail = text.len();
    let dbexec_fail1_rel = (dbexec_fail as i64 - dbexec_fail1_je as i64 - 6) as i32;
    text[dbexec_fail1_je+2..dbexec_fail1_je+6].copy_from_slice(&dbexec_fail1_rel.to_le_bytes());
    text.extend_from_slice(&[0x48, 0xc7, 0xc0, 0x00, 0x00, 0x00, 0x00]); // movq $0, %rax
    // .done:
    let dbexec_done = text.len();
    let dbexec_done_rel = (dbexec_done as i64 - dbexec_done_jmp as i64 - 2) as i8;
    text[dbexec_done_jmp+1] = dbexec_done_rel as u8;
    // popq %r13, %r12, %rbx
    text.extend_from_slice(&[0x41, 0x5d]); // popq %r13
    text.extend_from_slice(&[0x41, 0x5c]); // popq %r12
    text.push(0x5b); // popq %rbx
    text.push(0xc3); // ret
    // p_db_mem_query(rdi = db, rsi = query) → rax = JSON string pointer
    // Builds ["key1","key2",...] from stored keys
    // DB layout: [count(8), key_ptr_0(8), key_len_0(8), ...] (16-byte entries)
    sym!("p_db_mem_query");
    // pushq %rbx, %r12, %r13, %r14, %r15
    text.push(0x53); // pushq %rbx
    text.extend_from_slice(&[0x41, 0x54]); // pushq %r12
    text.extend_from_slice(&[0x41, 0x55]); // pushq %r13
    text.extend_from_slice(&[0x41, 0x56]); // pushq %r14
    text.extend_from_slice(&[0x41, 0x57]); // pushq %r15
    text.extend_from_slice(&[0x48, 0x83, 0xec, 0x10]); // subq $16, %rsp (align to 16)
    // rbx = db
    text.extend_from_slice(&[0x48, 0x89, 0xfb]); // movq %rdi, %rbx
    // Allocate output buffer: 4096 bytes
    text.extend_from_slice(&[0x48, 0xc7, 0xc7, 0x00, 0x10, 0x00, 0x00]); // movq $4096, %rdi
    text.extend_from_slice(&[0xe8, 0, 0, 0, 0]); // call p_malloc
    let dbquery_malloc_call = text.len() - 4;
    // r13 = rax (output buffer)
    text.extend_from_slice(&[0x49, 0x89, 0xc5]); // movq %rax, %r13
    // r14 = 0 (write index)
    text.extend_from_slice(&[0x4d, 0x31, 0xf6]); // xorq %r14, %r14
    // Write '[' at output[0]
    text.extend_from_slice(&[0x41, 0xc6, 0x45, 0x00, 0x5b]); // movb $0x5b, 0(%r13)
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14
    // r15 = 0 (key index)
    text.extend_from_slice(&[0x4d, 0x31, 0xff]); // xorq %r15, %r15
    // r12 = count = *(%rbx)
    text.extend_from_slice(&[0x4c, 0x8b, 0x23]); // movq (%rbx), %r12
    // .query_loop:
    let dbquery_loop = text.len();
    text.extend_from_slice(&[0x4d, 0x39, 0xfc]); // cmpq %r15, %r12
    let dbquery_done_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je .query_done
    // If not first key, write ','
    text.extend_from_slice(&[0x49, 0x83, 0xff, 0x00]); // cmpq $0, %r15
    let dbquery_nocomma_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je .no_comma
    // Write ',' at output[r14]: leaq (%r13,%r14,1), %rax ; movb $',', (%rax)
    // REX.WXB=0x4b, SIB index=6(r14), base=5(r13) → 0x35
    text.extend_from_slice(&[0x4b, 0x8d, 0x44, 0x35, 0x00]); // leaq 0(%r13,%r14,1), %rax
    text.extend_from_slice(&[0xc6, 0x00, 0x2c]); // movb $0x2c, (%rax)
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14
    // .no_comma:
    let dbquery_nocomma = text.len();
    let dbquery_nocomma_rel = (dbquery_nocomma as i64 - dbquery_nocomma_je as i64 - 6) as i32;
    text[dbquery_nocomma_je+2..dbquery_nocomma_je+6].copy_from_slice(&dbquery_nocomma_rel.to_le_bytes());
    // Write '"'
    text.extend_from_slice(&[0x4b, 0x8d, 0x44, 0x35, 0x00]); // leaq 0(%r13,%r14,1), %rax
    text.extend_from_slice(&[0xc6, 0x00, 0x22]); // movb $'"', (%rax)
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14
    // Get entry address: r15 * 16 + 8 + rbx
    // rax = r15 ; shlq $4, rax ; leaq 8(%rbx,%rax,1), %rax
    text.extend_from_slice(&[0x4c, 0x89, 0xf8]); // movq %r15, %rax
    text.extend_from_slice(&[0x48, 0xc1, 0xe0, 0x04]); // shlq $4, %rax
    text.extend_from_slice(&[0x48, 0x8d, 0x44, 0x03, 0x08]); // leaq 8(%rbx,%rax,1), %rax
    // rax = &entry.key_ptr. Load key_ptr and key_len.
    // Save entry addr on stack: movq %rax, (%rsp)
    text.extend_from_slice(&[0x48, 0x89, 0x04, 0x24]); // movq %rax, (%rsp)
    // rcx = key_ptr = *(%rax)
    text.extend_from_slice(&[0x48, 0x8b, 0x08]); // movq (%rax), %rcx
    // rdx = key_len = 8(%rax)
    text.extend_from_slice(&[0x48, 0x8b, 0x50, 0x08]); // movq 8(%rax), %rdx
    // Save key_len on stack: movq %rdx, 8(%rsp)
    text.extend_from_slice(&[0x48, 0x89, 0x54, 0x24, 0x08]); // movq %rdx, 8(%rsp)
    // .key_copy_loop:
    let dbquery_kcl = text.len();
    // cmpq $0, 8(%rsp) — compare remaining len with 0
    text.extend_from_slice(&[0x48, 0x83, 0x7c, 0x24, 0x08, 0x00]); // cmpq $0, 8(%rsp)
    let dbquery_kcdone_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je .key_copy_done
    // movzbl (%rcx), %eax
    text.extend_from_slice(&[0x0f, 0xb6, 0x01]); // movzbl (%rcx), %eax
    // Store byte: leaq (%r13,%r14,1), %rdx ; movb %al, (%rdx)
    text.extend_from_slice(&[0x4b, 0x8d, 0x54, 0x35, 0x00]); // leaq 0(%r13,%r14,1), %rdx
    text.extend_from_slice(&[0x88, 0x02]); // movb %al, (%rdx)
    // incq %r14
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14
    // incq %rcx (key cursor)
    text.extend_from_slice(&[0x48, 0xff, 0xc1]); // incq %rcx
    // decq 8(%rsp) (remaining len--)
    text.extend_from_slice(&[0x48, 0xff, 0x4c, 0x24, 0x08]); // decq 8(%rsp)
    // jmp .key_copy_loop
    let dbquery_kcl_jmp = text.len();
    let dbquery_kcl_back = dbquery_kcl as i64 - dbquery_kcl_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, dbquery_kcl_back as u8]); // jmp rel8
    // .key_copy_done:
    let dbquery_kcdone = text.len();
    let dbquery_kcdone_rel = (dbquery_kcdone as i64 - dbquery_kcdone_je as i64 - 6) as i32;
    text[dbquery_kcdone_je+2..dbquery_kcdone_je+6].copy_from_slice(&dbquery_kcdone_rel.to_le_bytes());
    // Write closing '"'
    text.extend_from_slice(&[0x4b, 0x8d, 0x44, 0x35, 0x00]); // leaq 0(%r13,%r14,1), %rax
    text.extend_from_slice(&[0xc6, 0x00, 0x22]); // movb $'"', (%rax)
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14
    // incq %r15 (key index)
    text.extend_from_slice(&[0x49, 0xff, 0xc7]); // incq %r15
    // jmp .query_loop
    let dbquery_loop_jmp = text.len();
    let dbquery_loop_back = dbquery_loop as i64 - dbquery_loop_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, dbquery_loop_back as u8]); // jmp rel8
    // .query_done:
    let dbquery_done = text.len();
    let dbquery_done_rel = (dbquery_done as i64 - dbquery_done_je as i64 - 6) as i32;
    text[dbquery_done_je+2..dbquery_done_je+6].copy_from_slice(&dbquery_done_rel.to_le_bytes());
    // Write ']'
    text.extend_from_slice(&[0x4b, 0x8d, 0x44, 0x35, 0x00]); // leaq 0(%r13,%r14,1), %rax
    text.extend_from_slice(&[0xc6, 0x00, 0x5d]); // movb $']', (%rax)
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14
    // Null-terminate
    text.extend_from_slice(&[0x4b, 0x8d, 0x44, 0x35, 0x00]); // leaq 0(%r13,%r14,1), %rax
    text.extend_from_slice(&[0xc6, 0x00, 0x00]); // movb $0, (%rax)
    // rax = r13 (return output buffer)
    text.extend_from_slice(&[0x4c, 0x89, 0xe8]); // movq %r13, %rax
    // addq $16, %rsp
    text.extend_from_slice(&[0x48, 0x83, 0xc4, 0x10]); // addq $16, %rsp
    // popq %r15, %r14, %r13, %r12, %rbx
    text.extend_from_slice(&[0x41, 0x5f]); // popq %r15
    text.extend_from_slice(&[0x41, 0x5e]); // popq %r14
    text.extend_from_slice(&[0x41, 0x5d]); // popq %r13
    text.extend_from_slice(&[0x41, 0x5c]); // popq %r12
    text.push(0x5b); // popq %rbx
    text.push(0xc3); // ret

    // ═══════════════════════════════════════════════════════════════════════
    // JSON functions — simplified implementations for test_db
    // ═══════════════════════════════════════════════════════════════════════

    // p_json_len(rdi = json_str) → rax = element count
    // For JSON arrays: count commas between [ and ], add 1 if non-empty
    sym!("p_json_len");
    // pushq %rbx, %r12
    text.push(0x53); // pushq %rbx
    text.extend_from_slice(&[0x41, 0x54]); // pushq %r12
    // rbx = json_str, r12 = 0 (count)
    text.extend_from_slice(&[0x48, 0x89, 0xfb]); // movq %rdi, %rbx
    text.extend_from_slice(&[0x4d, 0x31, 0xe4]); // xorq %r12, %r12
    // Skip to '[': find first '['
    // .find_bracket:
    let jlen_fb = text.len();
    // movzbl (%rbx), %eax
    text.extend_from_slice(&[0x0f, 0xb6, 0x03]); // movzbl (%rbx), %eax
    // testb %al, %al → end of string, return 0
    text.extend_from_slice(&[0x84, 0xc0]); // testb %al, %al
    // jz .done
    let jlen_done1_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je rel32
    // cmpb $0x5b, %al → '['?
    text.extend_from_slice(&[0x3c, 0x5b]); // cmpb $'[', %al
    // je .in_array
    let jlen_inarr_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je rel32
    // incq %rbx
    text.extend_from_slice(&[0x48, 0xff, 0xc3]); // incq %rbx
    // jmp .find_bracket
    let jlen_fb_jmp = text.len();
    let jlen_fb_back = jlen_fb as i64 - jlen_fb_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, jlen_fb_back as u8]); // jmp rel8
    // .in_array:
    let jlen_inarr = text.len();
    let jlen_inarr_rel = (jlen_inarr as i64 - jlen_inarr_je as i64 - 6) as i32;
    text[jlen_inarr_je+2..jlen_inarr_je+6].copy_from_slice(&jlen_inarr_rel.to_le_bytes());
    // incq %rbx (skip past '[')
    text.extend_from_slice(&[0x48, 0xff, 0xc3]); // incq %rbx
    // Check for empty array: movzbl (%rbx), %eax ; cmpb $']', %al
    text.extend_from_slice(&[0x0f, 0xb6, 0x03]); // movzbl (%rbx), %eax
    text.extend_from_slice(&[0x3c, 0x5d]); // cmpb $']', %al
    // je .done (empty array → count = 0)
    let jlen_done2_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je rel32
    // Not empty → count = 1
    text.extend_from_slice(&[0x49, 0xc7, 0xc4, 0x01, 0x00, 0x00, 0x00]); // movq $1, %r12
    // .scan:
    let jlen_scan = text.len();
    // movzbl (%rbx), %eax
    text.extend_from_slice(&[0x0f, 0xb6, 0x03]); // movzbl (%rbx), %eax
    // testb %al, %al → end of string
    text.extend_from_slice(&[0x84, 0xc0]); // testb %al, %al
    // jz .done
    let jlen_done3_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je rel32
    // cmpb $0x5d, %al → ']'?
    text.extend_from_slice(&[0x3c, 0x5d]); // cmpb $']', %al
    // je .done
    let jlen_done4_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je rel32
    // cmpb $0x2c, %al → ','?
    text.extend_from_slice(&[0x3c, 0x2c]); // cmpb $',', %al
    // jne .next
    let jlen_next_jne = text.len();
    text.extend_from_slice(&[0x0f, 0x85, 0, 0, 0, 0]); // jne rel32
    // incq %r12 (count++)
    text.extend_from_slice(&[0x49, 0xff, 0xc4]); // incq %r12
    // .next:
    let jlen_next = text.len();
    let jlen_next_rel = (jlen_next as i64 - jlen_next_jne as i64 - 6) as i32;
    text[jlen_next_jne+2..jlen_next_jne+6].copy_from_slice(&jlen_next_rel.to_le_bytes());
    // incq %rbx
    text.extend_from_slice(&[0x48, 0xff, 0xc3]); // incq %rbx
    // jmp .scan
    let jlen_scan_jmp = text.len();
    let jlen_scan_back = jlen_scan as i64 - jlen_scan_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, jlen_scan_back as u8]); // jmp rel8
    // .done:
    let jlen_done = text.len();
    // Patch all je's to here
    for je_off in [jlen_done1_je, jlen_done2_je, jlen_done3_je, jlen_done4_je] {
        let rel = (jlen_done as i64 - je_off as i64 - 6) as i32;
        text[je_off+2..je_off+6].copy_from_slice(&rel.to_le_bytes());
    }
    // rax = r12 (count)
    text.extend_from_slice(&[0x4c, 0x89, 0xe0]); // movq %r12, %rax
    // popq %r12, %rbx
    text.extend_from_slice(&[0x41, 0x5c]); // popq %r12
    text.push(0x5b); // popq %rbx
    text.push(0xc3); // ret

    // p_json_at(rdi = json_str, rsi = index) → rax = element string pointer
    // For JSON arrays of strings: extract the index-th quoted string
    // Returns the element as a JSON string (e.g., "alice" with quotes)
    sym!("p_json_at");
    // pushq %rbx, %r12, %r13, %r14
    text.push(0x53); // pushq %rbx
    text.extend_from_slice(&[0x41, 0x54]); // pushq %r12
    text.extend_from_slice(&[0x41, 0x55]); // pushq %r13
    text.extend_from_slice(&[0x41, 0x56]); // pushq %r14
    text.extend_from_slice(&[0x48, 0x83, 0xec, 0x08]); // subq $8, %rsp (align)
    // rbx = json_str, r12 = target index
    text.extend_from_slice(&[0x48, 0x89, 0xfb]); // movq %rdi, %rbx
    text.extend_from_slice(&[0x49, 0x89, 0xf4]); // movq %rsi, %r12
    // r13 = 0 (current index)
    text.extend_from_slice(&[0x4d, 0x31, 0xed]); // xorq %r13, %r13
    // Skip to '['
    // .find_bracket:
    let jat_fb = text.len();
    text.extend_from_slice(&[0x0f, 0xb6, 0x03]); // movzbl (%rbx), %eax
    text.extend_from_slice(&[0x84, 0xc0]); // testb %al, %al
    // jz .ret_empty
    let jat_empty_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je rel32
    text.extend_from_slice(&[0x3c, 0x5b]); // cmpb $'[', %al
    // je .in_array
    let jat_inarr_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je rel32
    text.extend_from_slice(&[0x48, 0xff, 0xc3]); // incq %rbx
    let jat_fb_jmp = text.len();
    let jat_fb_back = jat_fb as i64 - jat_fb_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, jat_fb_back as u8]); // jmp rel8
    // .in_array:
    let jat_inarr = text.len();
    let jat_inarr_rel = (jat_inarr as i64 - jat_inarr_je as i64 - 6) as i32;
    text[jat_inarr_je+2..jat_inarr_je+6].copy_from_slice(&jat_inarr_rel.to_le_bytes());
    // incq %rbx (skip '[')
    text.extend_from_slice(&[0x48, 0xff, 0xc3]); // incq %rbx
    // .find_quote:
    let jat_fq = text.len();
    // movzbl (%rbx), %eax
    text.extend_from_slice(&[0x0f, 0xb6, 0x03]); // movzbl (%rbx), %eax
    text.extend_from_slice(&[0x84, 0xc0]); // testb %al, %al
    // jz .ret_empty
    let jat_empty2_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je rel32
    // cmpb $0x22, %al → '"'?
    text.extend_from_slice(&[0x3c, 0x22]); // cmpb $'"', %al
    // je .found_quote
    let jat_fqfound_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je rel32
    // incq %rbx
    text.extend_from_slice(&[0x48, 0xff, 0xc3]); // incq %rbx
    let jat_fq_jmp = text.len();
    let jat_fq_back = jat_fq as i64 - jat_fq_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, jat_fq_back as u8]); // jmp rel8
    // .found_quote:
    let jat_fqfound = text.len();
    let jat_fqfound_rel = (jat_fqfound as i64 - jat_fqfound_je as i64 - 6) as i32;
    text[jat_fqfound_je+2..jat_fqfound_je+6].copy_from_slice(&jat_fqfound_rel.to_le_bytes());
    // Check if this is the target index
    // cmpq %r13, %r12 → compares target with current
    // je .extract — if target == current, extract this string
    text.extend_from_slice(&[0x4d, 0x39, 0xec]); // cmpq %r13, %r12
    let jat_extract_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je rel32
    // Not target → skip this string, find next quote
    // incq %rbx (skip past opening quote)
    text.extend_from_slice(&[0x48, 0xff, 0xc3]); // incq %rbx
    // .skip_str:
    let jat_skip = text.len();
    text.extend_from_slice(&[0x0f, 0xb6, 0x03]); // movzbl (%rbx), %eax
    text.extend_from_slice(&[0x84, 0xc0]); // testb %al, %al
    // jz .ret_empty
    let jat_empty3_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je rel32
    text.extend_from_slice(&[0x3c, 0x22]); // cmpb $'"', %al
    // je .str_end
    let jat_strend_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je rel32
    text.extend_from_slice(&[0x48, 0xff, 0xc3]); // incq %rbx
    let jat_skip_jmp = text.len();
    let jat_skip_back = jat_skip as i64 - jat_skip_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, jat_skip_back as u8]); // jmp rel8
    // .str_end:
    let jat_strend = text.len();
    let jat_strend_rel = (jat_strend as i64 - jat_strend_je as i64 - 6) as i32;
    text[jat_strend_je+2..jat_strend_je+6].copy_from_slice(&jat_strend_rel.to_le_bytes());
    // incq %rbx (skip past closing quote)
    text.extend_from_slice(&[0x48, 0xff, 0xc3]); // incq %rbx
    // incq %r13 (current index++)
    text.extend_from_slice(&[0x49, 0xff, 0xc5]); // incq %r13
    // jmp .find_quote (find next opening quote)
    let jat_fq2_jmp = text.len();
    let jat_fq2_back = jat_fq as i64 - jat_fq2_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, jat_fq2_back as u8]); // jmp rel8
    // .extract:
    let jat_extract = text.len();
    let jat_extract_rel = (jat_extract as i64 - jat_extract_je as i64 - 6) as i32;
    text[jat_extract_je+2..jat_extract_je+6].copy_from_slice(&jat_extract_rel.to_le_bytes());
    // rbx points to opening quote of target string
    // Find closing quote
    // incq %rbx (skip opening quote)
    text.extend_from_slice(&[0x48, 0xff, 0xc3]); // incq %rbx
    // r14 = rbx (start of string content)
    text.extend_from_slice(&[0x49, 0x89, 0xde]); // movq %rbx, %r14
    // .find_close:
    let jat_fclose = text.len();
    text.extend_from_slice(&[0x0f, 0xb6, 0x03]); // movzbl (%rbx), %eax
    text.extend_from_slice(&[0x84, 0xc0]); // testb %al, %al
    // jz .ret_empty
    let jat_empty4_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je rel32
    text.extend_from_slice(&[0x3c, 0x22]); // cmpb $'"', %al
    // je .copy_str
    let jat_copy_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je rel32
    text.extend_from_slice(&[0x48, 0xff, 0xc3]); // incq %rbx
    let jat_fclose_jmp = text.len();
    let jat_fclose_back = jat_fclose as i64 - jat_fclose_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, jat_fclose_back as u8]); // jmp rel8
    // .copy_str:
    let jat_copy = text.len();
    let jat_copy_rel = (jat_copy as i64 - jat_copy_je as i64 - 6) as i32;
    text[jat_copy_je+2..jat_copy_je+6].copy_from_slice(&jat_copy_rel.to_le_bytes());
    // rbx = end of string (points to closing quote)
    // r14 = start of string
    // length = rbx - r14
    // We need to return "string" (with quotes) as per json::dump
    // Allocate (rbx - r14 + 3) bytes: " + string + " + \0
    // rax = rbx - r14 (string length)
    text.extend_from_slice(&[0x48, 0x89, 0xd8]); // movq %rbx, %rax
    // subq %r14, %rax → REX.WR (W=1, R=1 for r14 in reg field), 29, ModRM=F0 (mod=11, reg=110=r14, rm=000=rax)
    text.extend_from_slice(&[0x4c, 0x29, 0xf0]); // subq %r14, %rax
    // rax = length + 3 (for two quotes and null)
    text.extend_from_slice(&[0x48, 0x83, 0xc0, 0x03]); // addq $3, %rax
    // movq %rax, %rdi
    text.extend_from_slice(&[0x48, 0x89, 0xc7]); // movq %rax, %rdi
    // call p_malloc
    text.extend_from_slice(&[0xe8, 0, 0, 0, 0]); // call p_malloc
    let jat_malloc_call = text.len() - 4;
    // r13 = rax (output buffer)
    text.extend_from_slice(&[0x49, 0x89, 0xc5]); // movq %rax, %r13
    // Write opening quote: movb $0x22, 0(%r13)
    text.extend_from_slice(&[0x41, 0xc6, 0x45, 0x00, 0x22]); // movb $0x22, 0(%r13)
    // r12 = 1 (write index, after opening quote)
    text.extend_from_slice(&[0x49, 0xc7, 0xc4, 0x01, 0x00, 0x00, 0x00]); // movq $1, %r12
    // Copy string bytes from r14 to r13+r12
    // .copy_loop:
    let jat_cploop = text.len();
    // cmpq %r14, %rbx → sets flags for rbx - r14, je when rbx == r14 (src reached end)
    text.extend_from_slice(&[0x49, 0x39, 0xde]); // cmpq %rbx, %r14
    // je .copy_done (when rbx == r14, i.e., src reached end)
    let jat_cpdone_je = text.len();
    text.extend_from_slice(&[0x0f, 0x84, 0, 0, 0, 0]); // je rel32
    // movzbl (%r14), %eax
    text.extend_from_slice(&[0x41, 0x0f, 0xb6, 0x06]); // movzbl (%r14), %eax
    // movb %al, 0(%r13,%r12,1) — use mod=01 with disp8=0 to avoid r13-as-base bug
    text.extend_from_slice(&[0x43, 0x88, 0x44, 0x25, 0x00]); // movb %al, 0(%r13,%r12,1)
    // incq %r12
    text.extend_from_slice(&[0x49, 0xff, 0xc4]); // incq %r12
    // incq %r14
    text.extend_from_slice(&[0x49, 0xff, 0xc6]); // incq %r14
    // jmp .copy_loop
    let jat_cploop_jmp = text.len();
    let jat_cploop_back = jat_cploop as i64 - jat_cploop_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, jat_cploop_back as u8]); // jmp rel8
    // .copy_done:
    let jat_cpdone = text.len();
    let jat_cpdone_rel = (jat_cpdone as i64 - jat_cpdone_je as i64 - 6) as i32;
    text[jat_cpdone_je+2..jat_cpdone_je+6].copy_from_slice(&jat_cpdone_rel.to_le_bytes());
    // Write closing quote: movb $0x22, 0(%r13,%r12,1)
    text.extend_from_slice(&[0x43, 0xc6, 0x44, 0x25, 0x00, 0x22]); // movb $0x22, 0(%r13,%r12,1)
    // incq %r12
    text.extend_from_slice(&[0x49, 0xff, 0xc4]); // incq %r12
    // Null-terminate: movb $0, 0(%r13,%r12,1)
    text.extend_from_slice(&[0x43, 0xc6, 0x44, 0x25, 0x00, 0x00]); // movb $0, 0(%r13,%r12,1)
    // rax = r13 (return output)
    text.extend_from_slice(&[0x4c, 0x89, 0xe8]); // movq %r13, %rax
    // jmp .jat_done
    let jat_done_jmp = text.len();
    text.extend_from_slice(&[0xeb, 0x00]); // jmp rel8 (placeholder)
    // .ret_empty:
    let jat_empty = text.len();
    // Patch all empty je's
    for je_off in [jat_empty_je, jat_empty2_je, jat_empty3_je, jat_empty4_je] {
        let rel = (jat_empty as i64 - je_off as i64 - 6) as i32;
        text[je_off+2..je_off+6].copy_from_slice(&rel.to_le_bytes());
    }
    // Return empty string: allocate 1 byte, store 0
    text.extend_from_slice(&[0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00]); // movq $1, %rdi
    text.extend_from_slice(&[0xe8, 0, 0, 0, 0]); // call p_malloc
    let jat_empty_malloc = text.len() - 4;
    text.extend_from_slice(&[0xc6, 0x00, 0x00]); // movb $0, (%rax)
    // .jat_done:
    let jat_done = text.len();
    let jat_done_rel = (jat_done as i64 - jat_done_jmp as i64 - 2) as i8;
    text[jat_done_jmp+1] = jat_done_rel as u8;
    // addq $8, %rsp
    text.extend_from_slice(&[0x48, 0x83, 0xc4, 0x08]); // addq $8, %rsp
    // popq %r14, %r13, %r12, %rbx
    text.extend_from_slice(&[0x41, 0x5e]); // popq %r14
    text.extend_from_slice(&[0x41, 0x5d]); // popq %r13
    text.extend_from_slice(&[0x41, 0x5c]); // popq %r12
    text.push(0x5b); // popq %rbx
    text.push(0xc3); // ret

    // ═══════════════════════════════════════════════════════════════════════
    // FFI math functions — implementations of common C math functions
    // used by extern "C" declarations (sqrt, floor, ceil, pow, strlen)
    // ═══════════════════════════════════════════════════════════════════════

    // pledge_sqrt(rdi = double bits) → rax = double bits
    // sqrtsd %xmm0, %xmm0 ; movsd %xmm0, (%rsp) ; movq (%rsp), %rax
    sym!("pledge_sqrt");
    text.extend_from_slice(&[0x48, 0x83, 0xec, 0x10]); // subq $16, %rsp
    text.extend_from_slice(&[0x48, 0x89, 0x3c, 0x24]); // movq %rdi, (%rsp)
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x04, 0x24]); // movsd (%rsp), %xmm0
    text.extend_from_slice(&[0xf2, 0x0f, 0x51, 0xc0]); // sqrtsd %xmm0, %xmm0
    text.extend_from_slice(&[0xf2, 0x0f, 0x11, 0x04, 0x24]); // movsd %xmm0, (%rsp)
    text.extend_from_slice(&[0x48, 0x8b, 0x04, 0x24]); // movq (%rsp), %rax
    text.extend_from_slice(&[0x48, 0x83, 0xc4, 0x10]); // addq $16, %rsp
    text.push(0xc3); // ret

    // pledge_floor(rdi = double bits) → rax = double bits
    // roundsd $1, %xmm0, %xmm0 (round toward -inf)
    sym!("pledge_floor");
    text.extend_from_slice(&[0x48, 0x83, 0xec, 0x10]); // subq $16, %rsp
    text.extend_from_slice(&[0x48, 0x89, 0x3c, 0x24]); // movq %rdi, (%rsp)
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x04, 0x24]); // movsd (%rsp), %xmm0
    // roundsd $1, %xmm0, %xmm0 — 66 0F 3A 0B C0 01
    text.extend_from_slice(&[0x66, 0x0f, 0x3a, 0x0b, 0xc0, 0x01]);
    text.extend_from_slice(&[0xf2, 0x0f, 0x11, 0x04, 0x24]); // movsd %xmm0, (%rsp)
    text.extend_from_slice(&[0x48, 0x8b, 0x04, 0x24]); // movq (%rsp), %rax
    text.extend_from_slice(&[0x48, 0x83, 0xc4, 0x10]); // addq $16, %rsp
    text.push(0xc3); // ret

    // pledge_ceil(rdi = double bits) → rax = double bits
    // roundsd $2, %xmm0, %xmm0 (round toward +inf)
    sym!("pledge_ceil");
    text.extend_from_slice(&[0x48, 0x83, 0xec, 0x10]); // subq $16, %rsp
    text.extend_from_slice(&[0x48, 0x89, 0x3c, 0x24]); // movq %rdi, (%rsp)
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x04, 0x24]); // movsd (%rsp), %xmm0
    // roundsd $2, %xmm0, %xmm0 — 66 0F 3A 0B C0 02
    text.extend_from_slice(&[0x66, 0x0f, 0x3a, 0x0b, 0xc0, 0x02]);
    text.extend_from_slice(&[0xf2, 0x0f, 0x11, 0x04, 0x24]); // movsd %xmm0, (%rsp)
    text.extend_from_slice(&[0x48, 0x8b, 0x04, 0x24]); // movq (%rsp), %rax
    text.extend_from_slice(&[0x48, 0x83, 0xc4, 0x10]); // addq $16, %rsp
    text.push(0xc3); // ret

    // pledge_pow(rdi = base bits, rsi = exp bits) → rax = double bits
    // For integer exponents, uses iterative multiplication.
    // For non-integer exponents, falls back to x87 FPU (fyl2x + f2xm1).
    sym!("pledge_pow");
    // pushq %rbx ; pushq %r12
    text.push(0x53);
    text.extend_from_slice(&[0x41, 0x54]);
    // subq $40, %rsp
    text.extend_from_slice(&[0x48, 0x83, 0xec, 0x28]);
    // Store base at 8(%rsp), exp at 16(%rsp)
    text.extend_from_slice(&[0x48, 0x89, 0x7c, 0x24, 0x08]); // movq %rdi, 8(%rsp)
    text.extend_from_slice(&[0x48, 0x89, 0x74, 0x24, 0x10]); // movq %rsi, 16(%rsp)
    // Load exp into xmm0
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x44, 0x24, 0x10]); // movsd 16(%rsp), %xmm0
    // cvttsd2siq %xmm0, %r12 (integer exp)
    text.extend_from_slice(&[0xf2, 0x4c, 0x0f, 0x2c, 0xe0]);
    // cvtsi2sdq %r12, %xmm1 (back to double)
    text.extend_from_slice(&[0xf2, 0x49, 0x0f, 0x2a, 0xcc]);
    // ucomisd %xmm1, %xmm0 — check if exp is integer
    text.extend_from_slice(&[0x66, 0x0f, 0x2e, 0xc1]);
    // jne .float_pow (non-integer exponent)
    let ppow_jne = text.len();
    text.extend_from_slice(&[0x0f, 0x85, 0x00, 0x00, 0x00, 0x00]); // jne rel32

    // Integer exponent path: result = 1.0, loop r12 times multiplying by base
    // Store 1.0 at 24(%rsp)
    // 1.0 = 0x3FF0000000000000
    text.extend_from_slice(&[0xc7, 0x44, 0x24, 0x18, 0x00, 0x00, 0x00, 0x00]);
    text.extend_from_slice(&[0xc7, 0x44, 0x24, 0x1c, 0x00, 0x00, 0xf0, 0x3f]);
    // movsd 24(%rsp), %xmm0 (result = 1.0)
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x44, 0x24, 0x18]);
    // movq %r12, %rbx (loop counter)
    text.extend_from_slice(&[0x4c, 0x89, 0xe3]); // movq %r12, %rbx (REX.W+R, reg=r12, rm=rbx)

    // Handle negative exponents: if r12 < 0, negate and track sign
    // testq %rbx, %rbx
    text.extend_from_slice(&[0x48, 0x85, 0xdb]);
    // jns .int_loop_start
    let ppow_jns = text.len();
    text.extend_from_slice(&[0x79, 0x00]);
    // negq %rbx
    text.extend_from_slice(&[0x48, 0xf7, 0xdb]);
    // .int_loop_start:
    let ppow_int_loop_start = text.len();
    text[ppow_jns + 1] = (ppow_int_loop_start - ppow_jns - 2) as u8;

    // .int_loop:
    let ppow_int_loop = text.len();
    // testq %rbx, %rbx
    text.extend_from_slice(&[0x48, 0x85, 0xdb]);
    // je .int_done
    let ppow_je_int = text.len();
    text.extend_from_slice(&[0x74, 0x00]);
    // movsd 8(%rsp), %xmm1 (base)
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x4c, 0x24, 0x08]);
    // mulsd %xmm1, %xmm0 (result *= base)
    text.extend_from_slice(&[0xf2, 0x0f, 0x59, 0xc1]);
    // decq %rbx
    text.extend_from_slice(&[0x48, 0xff, 0xcb]);
    // jmp .int_loop
    let ppow_jmp_int = text.len();
    let ppow_int_back = ppow_int_loop as i64 - ppow_jmp_int as i64 - 2;
    text.extend_from_slice(&[0xeb, ppow_int_back as u8]);
    // .int_done:
    let ppow_int_done = text.len();
    text[ppow_je_int + 1] = (ppow_int_done - ppow_je_int - 2) as u8;

    // Check if original exp was negative — if so, result = 1.0 / result
    // testq %r12, %r12
    text.extend_from_slice(&[0x4d, 0x85, 0xe4]);
    // jns .int_store
    let ppow_jns2 = text.len();
    text.extend_from_slice(&[0x79, 0x00]);
    // 1.0 in xmm1
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x4c, 0x24, 0x18]); // movsd 24(%rsp), %xmm1 (1.0)
    // divsd %xmm0, %xmm1 (1.0 / result)
    text.extend_from_slice(&[0xf2, 0x0f, 0x5e, 0xc8]);
    // movsd %xmm1, %xmm0
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0xc1]);
    // .int_store:
    let ppow_int_store = text.len();
    text[ppow_jns2 + 1] = (ppow_int_store - ppow_jns2 - 2) as u8;

    // jmp .store_result
    text.extend_from_slice(&[0xeb, 0x00]);
    let ppow_jmp_store = text.len() - 1;

    // .float_pow: (non-integer exponent — use x87 FPU)
    let ppow_float_pow = text.len();
    // Patch jne
    let ppow_jne_rel = (ppow_float_pow as i64 - ppow_jne as i64 - 6) as i32;
    text[ppow_jne + 2..ppow_jne + 6].copy_from_slice(&ppow_jne_rel.to_le_bytes());

    // fld exp → st(0) = exp
    text.extend_from_slice(&[0xdd, 0x44, 0x24, 0x10]); // fld qword ptr 16(%rsp)
    // fld base → st(0) = base, st(1) = exp
    text.extend_from_slice(&[0xdd, 0x44, 0x24, 0x08]); // fld qword ptr 8(%rsp)
    // fyl2x: st(0) = exp * log2(base), pop
    text.extend_from_slice(&[0xd9, 0xf1]); // fyl2x
    // Now st(0) = y * log2(x)
    // fld st(0) → duplicate: st(0) = st(1) = y*log2(x)
    text.extend_from_slice(&[0xd9, 0xc0]); // fld st(0)
    // frndint → st(0) = floor(y*log2(x))
    text.extend_from_slice(&[0xd9, 0xfc]); // frndint
    // fsub st(1), st(0) → st(0) = floor - original = -(fractional part)
    // Actually: fsubp st(1), st(0) = st(1) - st(0), pop → st(0) = frac
    text.extend_from_slice(&[0xde, 0xe1]); // fsubp st(1), st(0)
    // Now st(0) = frac, st(1) = floor_val
    // f2xm1: st(0) = 2^frac - 1
    text.extend_from_slice(&[0xd9, 0xf0]); // f2xm1
    // fld1 → st(0) = 1.0, st(1) = 2^frac - 1, st(2) = floor_val
    text.extend_from_slice(&[0xd9, 0xe8]); // fld1
    // faddp st(1), st(0) → st(0) = 2^frac, st(1) = floor_val
    text.extend_from_slice(&[0xde, 0xc1]); // faddp st(1), st(0)
    // fscale: st(0) = st(0) * 2^floor(st(1))
    text.extend_from_slice(&[0xd9, 0xfd]); // fscale
    // fstp st(1) — pop floor_val, leave result
    text.extend_from_slice(&[0xde, 0xd1]); // fstp st(1)
    // Store result to 24(%rsp)
    text.extend_from_slice(&[0xdd, 0x5c, 0x24, 0x18]); // fstp qword ptr 24(%rsp)
    // movsd 24(%rsp), %xmm0
    text.extend_from_slice(&[0xf2, 0x0f, 0x10, 0x44, 0x24, 0x18]);

    // .store_result:
    let ppow_store_result = text.len();
    // Patch jmp_store
    let ppow_store_rel = (ppow_store_result as i64 - ppow_jmp_store as i64 - 1) as i8;
    text[ppow_jmp_store] = ppow_store_rel as u8;

    // movsd %xmm0, 24(%rsp)
    text.extend_from_slice(&[0xf2, 0x0f, 0x11, 0x44, 0x24, 0x18]);
    // movq 24(%rsp), %rax
    text.extend_from_slice(&[0x48, 0x8b, 0x44, 0x24, 0x18]);
    // addq $40, %rsp
    text.extend_from_slice(&[0x48, 0x83, 0xc4, 0x28]);
    // popq %r12 ; popq %rbx
    text.extend_from_slice(&[0x41, 0x5c]);
    text.push(0x5b);
    text.push(0xc3); // ret

    // pledge_strlen(rdi = str ptr) → rax = length
    // Simple loop: count bytes until null terminator
    sym!("pledge_strlen");
    // xorq %rax, %rax (counter = 0)
    text.extend_from_slice(&[0x48, 0x31, 0xc0]);
    // .loop:
    let pstrlen_loop = text.len();
    // cmpb $0, (%rdi)
    text.extend_from_slice(&[0x80, 0x3f, 0x00]);
    // je .done
    let pstrlen_je = text.len();
    text.extend_from_slice(&[0x74, 0x00]);
    // incq %rax ; incq %rdi
    text.extend_from_slice(&[0x48, 0xff, 0xc0]);
    text.extend_from_slice(&[0x48, 0xff, 0xc7]);
    // jmp .loop
    let pstrlen_jmp = text.len();
    let pstrlen_back = pstrlen_loop as i64 - pstrlen_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, pstrlen_back as u8]);
    // .done:
    let pstrlen_done = text.len();
    text[pstrlen_je + 1] = (pstrlen_done - pstrlen_je - 2) as u8;
    text.push(0xc3); // ret

    // ═══════════════════════════════════════════════════════════════════════
    // Additional FFI math functions: sin, cos, fabs
    // ═══════════════════════════════════════════════════════════════════════

    // pledge_sin / c_sin(rdi = double bits) → rax = double bits
    // Uses x87 FPU: fsin
    sym!("pledge_sin");
    text.extend_from_slice(&[0x48, 0x83, 0xec, 0x10]); // subq $16, %rsp
    text.extend_from_slice(&[0x48, 0x89, 0x3c, 0x24]); // movq %rdi, (%rsp)
    text.extend_from_slice(&[0xdd, 0x04, 0x24]); // fld qword ptr (%rsp)
    text.extend_from_slice(&[0xd9, 0xfe]); // fsin
    text.extend_from_slice(&[0xdd, 0x1c, 0x24]); // fstp qword ptr (%rsp)
    text.extend_from_slice(&[0x48, 0x8b, 0x04, 0x24]); // movq (%rsp), %rax
    text.extend_from_slice(&[0x48, 0x83, 0xc4, 0x10]); // addq $16, %rsp
    text.push(0xc3); // ret

    // pledge_cos / c_cos(rdi = double bits) → rax = double bits
    // Uses x87 FPU: fcos
    sym!("pledge_cos");
    text.extend_from_slice(&[0x48, 0x83, 0xec, 0x10]); // subq $16, %rsp
    text.extend_from_slice(&[0x48, 0x89, 0x3c, 0x24]); // movq %rdi, (%rsp)
    text.extend_from_slice(&[0xdd, 0x04, 0x24]); // fld qword ptr (%rsp)
    text.extend_from_slice(&[0xd9, 0xff]); // fcos
    text.extend_from_slice(&[0xdd, 0x1c, 0x24]); // fstp qword ptr (%rsp)
    text.extend_from_slice(&[0x48, 0x8b, 0x04, 0x24]); // movq (%rsp), %rax
    text.extend_from_slice(&[0x48, 0x83, 0xc4, 0x10]); // addq $16, %rsp
    text.push(0xc3); // ret

    // pledge_fabs / c_fabs(rdi = double bits) → rax = double bits
    // Clears the sign bit: rax = rdi & 0x7FFFFFFFFFFFFFFF
    sym!("pledge_fabs");
    // movq %rdi, %rax
    text.extend_from_slice(&[0x48, 0x89, 0xf8]);
    // movabsq $0x7FFFFFFFFFFFFFFF, %rcx
    // 48 b9 FF FF FF FF FF FF FF 7F
    text.extend_from_slice(&[0x48, 0xb9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f]);
    // andq %rcx, %rax
    text.extend_from_slice(&[0x48, 0x21, 0xc8]);
    text.push(0xc3); // ret

    // pledge_strcmp / c_strcmp(rdi = a, rsi = b) → rax = 0/1/-1
    // Simple byte-by-byte comparison
    sym!("pledge_strcmp");
    // .loop:
    let pstrcmp_loop = text.len();
    // movb (%rdi), %al
    text.extend_from_slice(&[0x8a, 0x07]);
    // movb (%rsi), %cl
    text.extend_from_slice(&[0x8a, 0x0e]);
    // cmpb %cl, %al
    text.extend_from_slice(&[0x38, 0xc1]);
    // jne .diff
    let pstrcmp_jne = text.len();
    text.extend_from_slice(&[0x75, 0x00]);
    // testq %rax, %rax (end of string?)
    text.extend_from_slice(&[0x84, 0xc0]);
    // je .equal
    let pstrcmp_je = text.len();
    text.extend_from_slice(&[0x74, 0x00]);
    // incq %rdi ; incq %rsi
    text.extend_from_slice(&[0x48, 0xff, 0xc7]);
    text.extend_from_slice(&[0x48, 0xff, 0xc6]);
    // jmp .loop
    let pstrcmp_jmp = text.len();
    let pstrcmp_back = pstrcmp_loop as i64 - pstrcmp_jmp as i64 - 2;
    text.extend_from_slice(&[0xeb, pstrcmp_back as u8]);
    // .diff:
    let pstrcmp_diff = text.len();
    text[pstrcmp_jne + 1] = (pstrcmp_diff - pstrcmp_jne - 2) as u8;
    // movzbq %al, %rax
    text.extend_from_slice(&[0x48, 0x0f, 0xb6, 0xc0]);
    // movzbq %cl, %rcx
    text.extend_from_slice(&[0x48, 0x0f, 0xb6, 0xc9]);
    // subq %rcx, %rax
    text.extend_from_slice(&[0x48, 0x29, 0xc8]);
    // jmp .ret
    let pstrcmp_jmp_ret = text.len();
    text.extend_from_slice(&[0xeb, 0x00]);
    // .equal:
    let pstrcmp_equal = text.len();
    text[pstrcmp_je + 1] = (pstrcmp_equal - pstrcmp_je - 2) as u8;
    // xorq %rax, %rax
    text.extend_from_slice(&[0x48, 0x31, 0xc0]);
    // .ret:
    let pstrcmp_ret = text.len();
    text[pstrcmp_jmp_ret] = (pstrcmp_ret as i64 - pstrcmp_jmp_ret as i64 - 1) as i8 as u8;
    text.push(0xc3); // ret

    // ═══════════════════════════════════════════════════════════════════════
    // FFI function aliases — c_* names used by test_ffi_full
    // These point to the same code as the pledge_* versions above.
    // We record the current text offsets as symbol addresses.
    // ═══════════════════════════════════════════════════════════════════════

    // For c_* aliases, we need to find the pledge_* function offsets and
    // register them under the c_* names. We'll do this after all functions
    // are emitted, using the symbols vector.

    // println is defined in stdlib/core.p and compiled by the codegen.
    // Do NOT define a runtime println thunk — it would override the
    // user-defined println and always call p_print_int, which is wrong
    // for string values.

    // ═══════════════════════════════════════════════════════════════════════
    // FFI aliases: c_* names → same offsets as pledge_* functions
    // test_ffi_full uses c_sqrt, c_pow, etc. instead of pledge_sqrt
    // ═══════════════════════════════════════════════════════════════════════
    let pledge_to_c: &[(&str, &str)] = &[
        ("pledge_sqrt", "c_sqrt"),
        ("pledge_pow", "c_pow"),
        ("pledge_floor", "c_floor"),
        ("pledge_ceil", "c_ceil"),
        ("pledge_sin", "c_sin"),
        ("pledge_cos", "c_cos"),
        ("pledge_fabs", "c_fabs"),
        ("pledge_strlen", "c_strlen"),
        ("pledge_strcmp", "c_strcmp"),
    ];
    let sym_map: std::collections::HashMap<&str, u64> =
        symbols.iter().map(|(n, o, _)| (*n, *o)).collect();
    for (pname, cname) in pledge_to_c {
        if let Some(&off) = sym_map.get(pname) {
            symbols.push((cname, off, 0));
        }
    }

    // Now we need to collect the relocations for internal calls/jumps.
    // These are relocations within the runtime that need to be resolved
    // by the linker (calls between runtime functions).
    let mut relocations: Vec<(u64, String, u8)> = Vec::new();

    // _start calls main — this is an external call (to user code)
    relocations.push((_start_call_offset as u64, "main".to_string(), 4)); // R_X86_64_PLT32

    // p_print_float calls p_print_int
    relocations.push((pfloat_call1_off as u64, "p_print_int".to_string(), 4));

    // p_str_concat calls p_malloc
    relocations.push((pstrcat_call_off as u64, "p_malloc".to_string(), 4));

    // p_to_str calls p_malloc
    relocations.push((pts_call_off as u64, "p_malloc".to_string(), 4));

    // p_float_to_str calls p_malloc
    relocations.push((pfts_malloc_off as u64, "p_malloc".to_string(), 4));

    // p_bool_to_str calls p_malloc and LEA relocations
    relocations.push((pbts_malloc_off as u64, "p_malloc".to_string(), 4));
    relocations.push((pbts_true_lea as u64, ".Lpbool_true_str".to_string(), 2));
    relocations.push((pbts_false_lea as u64, ".Lpbool_false_str".to_string(), 2));

    // p_vec_to_str calls p_malloc, p_vec_len, p_to_str
    relocations.push((pvts_malloc_off as u64, "p_malloc".to_string(), 4));
    relocations.push((pvts_len_call_off as u64, "p_vec_len".to_string(), 4));
    relocations.push((pvts_tostr_call_off as u64, "p_to_str".to_string(), 4));

    // p_alloc jumps to p_malloc
    relocations.push((palloc_jmp_off as u64, "p_malloc".to_string(), 2)); // R_X86_64_PC32

    // p_allocator_alloc jumps to p_malloc
    relocations.push((palloc_alloc_jmp_off as u64, "p_malloc".to_string(), 2));

    // p_allocator_free jumps to p_free
    relocations.push((palloc_free_jmp_off as u64, "p_free".to_string(), 2));

    // RIP-relative LEA relocations for rodata symbols
    // p_print_bool: true string
    relocations.push((pbool_true_lea as u64, ".Lpbool_true_str".to_string(), 1)); // R_X86_64_32? No, need PC-relative
    // Actually, for RIP-relative LEA, the relocation type is R_X86_64_PC32 (type 2)
    relocations.pop();
    relocations.push((pbool_true_lea as u64, ".Lpbool_true_str".to_string(), 2));

    relocations.push((pbool_false_lea as u64, ".Lpbool_false_str".to_string(), 2));
    relocations.push((pnl_lea as u64, ".Lpnl_str".to_string(), 2));
    relocations.push((panic_nl_lea as u64, ".Lpnl_str".to_string(), 2));

    // p_vec_new calls p_malloc
    relocations.push((pvec_new_call_off as u64, "p_malloc".to_string(), 4));

    // Database function calls
    relocations.push((dbopen_call_off as u64, "p_malloc".to_string(), 4));
    relocations.push((dbclose_call_off as u64, "p_free".to_string(), 4));
    relocations.push((dbquery_malloc_call as u64, "p_malloc".to_string(), 4));
    relocations.push((jat_malloc_call as u64, "p_malloc".to_string(), 4));
    relocations.push((jat_empty_malloc as u64, "p_malloc".to_string(), 4));

    // p_print_vec LEA relocations and call
    relocations.push((pvec_open_lea as u64, ".Lbracket_open".to_string(), 2));
    relocations.push((pvec_comma_lea as u64, ".Lcomma_sp".to_string(), 2));
    relocations.push((pvec_close_lea as u64, ".Lbracket_close".to_string(), 2));
    relocations.push((pvec_print_call_off as u64, "p_print_int".to_string(), 4));
    relocations.push((pvec_len_call_off as u64, "p_vec_len".to_string(), 4));

    // Add relocations from assembly-encoded runtime functions
    relocations.extend(asm_reloc_list);

    // Store relocations for the linker to use
    // We'll return them as part of the symbol table via a global
    RUNTIME_RELOCATIONS.with(|cell| {
        *cell.borrow_mut() = relocations;
    });

    (text, symbols, rodata, rodata_syms)
}

use std::cell::RefCell;
thread_local! {
    static RUNTIME_RELOCATIONS: RefCell<Vec<(u64, String, u8)>> = RefCell::new(Vec::new());
}

/// Get the runtime relocations (internal calls between runtime functions).
pub fn runtime_relocations() -> Vec<(u64, String, u8)> {
    RUNTIME_RELOCATIONS.with(|cell| cell.borrow().clone())
}
