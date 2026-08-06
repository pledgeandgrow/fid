.data
str_0 BYTE "panic: called unwrap on Err\n", 0
str_1 BYTE "panic: called unwrap on None\n", 0
str_2 BYTE "\n", 0
str_3 BYTE "Assertion failed\n", 0
str_true BYTE "true", 0
str_false BYTE "false", 0
str_concat_buf BYTE 4096 DUP(?)
str_concat_tmp BYTE 4096 DUP(?)


.code
EXTERN GetStdHandle:PROC
EXTERN WriteFile:PROC
EXTERN ExitProcess:PROC
EXTERN GetProcessHeap:PROC
EXTERN HeapAlloc:PROC
EXTERN HeapFree:PROC

PUBLIC main
PUBLIC _main


; ─── Function: Option_is_some ───
Option_is_some:
    push rbp
    mov rbp, rsp
    sub rsp, 208
    mov [rbp-96], rcx
    mov [rbp-104], rdx

Option_is_some_bb_0:
    mov rax, [rbp-104]
    mov [rbp-56], rax
    mov rax, [rbp-56]
    mov rax, [rax]
    mov [rbp-144], rax
    mov rax, 0
    mov [rbp-136], rax
    mov rax, [rbp-144]
    mov rcx, [rbp-136]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-40], rax
    mov rax, [rbp-40]
    test rax, rax
    jnz Option_is_some_bb_2
    jmp Option_is_some_bb_3

Option_is_some_bb_1:
    mov rax, 0
    mov [rbp-120], rax
    mov rax, [rbp-120]
    mov rsp, rbp
    pop rbp
    ret

Option_is_some_bb_2:
    mov rax, [rbp-104]
    add rax, 8
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rax, [rax]
    mov [rbp-64], rax
    mov rax, 1
    mov [rbp-72], rax
    mov rax, [rbp-72]
    mov rsp, rbp
    pop rbp
    ret

Option_is_some_bb_3:
    mov rax, [rbp-104]
    mov [rbp-80], rax
    mov rax, [rbp-80]
    mov rax, [rax]
    mov [rbp-128], rax
    mov rax, 1
    mov [rbp-48], rax
    mov rax, [rbp-128]
    mov rcx, [rbp-48]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-168], rax
    mov rax, [rbp-168]
    test rax, rax
    jnz Option_is_some_bb_4
    jmp Option_is_some_bb_5

Option_is_some_bb_4:
    mov rax, 0
    mov [rbp-152], rax
    mov rax, [rbp-152]
    mov rsp, rbp
    pop rbp
    ret

Option_is_some_bb_5:
    jmp Option_is_some_bb_1
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: min ───
min:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    mov [rbp-16], rcx
    mov [rbp-24], rdx
    mov [rbp-48], r8

min_bb_0:
    mov rax, [rbp-24]
    mov rcx, [rbp-48]
    cmp rax, rcx
    setl al
    movzx eax, al
    mov [rbp-40], rax
    mov rax, [rbp-40]
    test rax, rax
    jnz min_bb_1
    jmp min_bb_2

min_bb_1:
    mov rax, [rbp-24]
    mov rsp, rbp
    pop rbp
    ret

min_bb_2:
    jmp min_bb_3

min_bb_3:
    mov rax, [rbp-48]
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: Point_new ───
Point_new:
    push rbp
    mov rbp, rsp
    sub rsp, 192
    mov [rbp-8], rcx
    mov [rbp-40], rdx
    mov [rbp-56], r8
    mov [rbp-88], r9

Point_new_bb_0:
    lea rax, [rbp-160]
    mov [rbp-136], rax
    mov rax, [rbp-136]
    mov [rbp-112], rax
    mov rax, [rbp-112]
    mov rcx, [rbp-56]
    mov [rax], rcx
    mov rax, [rbp-136]
    add rax, 8
    mov [rbp-120], rax
    mov rax, [rbp-120]
    mov rcx, [rbp-88]
    mov [rax], rcx
    mov rax, [rbp-136]
    mov [rbp-104], rax
    mov rax, [rbp-8]
    mov [rbp-32], rax
    mov rax, [rbp-104]
    mov rax, [rax]
    mov [rbp-48], rax
    mov rax, [rbp-32]
    mov rcx, [rbp-48]
    mov [rax], rcx
    mov rax, [rbp-136]
    add rax, 8
    mov [rbp-96], rax
    mov rax, [rbp-8]
    add rax, 8
    mov [rbp-64], rax
    mov rax, [rbp-96]
    mov rax, [rax]
    mov [rbp-24], rax
    mov rax, [rbp-64]
    mov rcx, [rbp-24]
    mov [rax], rcx
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: Option_unwrap_or ───
Option_unwrap_or:
    push rbp
    mov rbp, rsp
    sub rsp, 192
    mov [rbp-120], rcx
    mov [rbp-112], rdx
    mov [rbp-72], r8

Option_unwrap_or_bb_0:
    mov rax, [rbp-112]
    mov [rbp-80], rax
    mov rax, [rbp-80]
    mov rax, [rax]
    mov [rbp-96], rax
    mov rax, 0
    mov [rbp-16], rax
    mov rax, [rbp-96]
    mov rcx, [rbp-16]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-56], rax
    mov rax, [rbp-56]
    test rax, rax
    jnz Option_unwrap_or_bb_2
    jmp Option_unwrap_or_bb_3

Option_unwrap_or_bb_1:
    mov rax, 0
    mov [rbp-144], rax
    mov rax, [rbp-144]
    mov rsp, rbp
    pop rbp
    ret

Option_unwrap_or_bb_2:
    mov rax, [rbp-112]
    add rax, 8
    mov [rbp-32], rax
    mov rax, [rbp-32]
    mov rax, [rax]
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov rsp, rbp
    pop rbp
    ret

Option_unwrap_or_bb_3:
    mov rax, [rbp-112]
    mov [rbp-88], rax
    mov rax, [rbp-88]
    mov rax, [rax]
    mov [rbp-24], rax
    mov rax, 1
    mov [rbp-104], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-104]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-40], rax
    mov rax, [rbp-40]
    test rax, rax
    jnz Option_unwrap_or_bb_4
    jmp Option_unwrap_or_bb_5

Option_unwrap_or_bb_4:
    mov rax, [rbp-72]
    mov rsp, rbp
    pop rbp
    ret

Option_unwrap_or_bb_5:
    jmp Option_unwrap_or_bb_1
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: clamp ───
clamp:
    push rbp
    mov rbp, rsp
    sub rsp, 144
    mov [rbp-64], rcx
    mov [rbp-8], rdx
    mov [rbp-32], r8
    mov [rbp-88], r9

clamp_bb_0:
    mov rax, [rbp-8]
    mov rcx, [rbp-32]
    cmp rax, rcx
    setl al
    movzx eax, al
    mov [rbp-72], rax
    mov rax, [rbp-72]
    test rax, rax
    jnz clamp_bb_1
    jmp clamp_bb_2

clamp_bb_1:
    mov rax, [rbp-32]
    mov rsp, rbp
    pop rbp
    ret

clamp_bb_2:
    jmp clamp_bb_3

clamp_bb_3:
    mov rax, [rbp-8]
    mov rcx, [rbp-88]
    cmp rax, rcx
    setg al
    movzx eax, al
    mov [rbp-40], rax
    mov rax, [rbp-40]
    test rax, rax
    jnz clamp_bb_4
    jmp clamp_bb_5

clamp_bb_4:
    mov rax, [rbp-88]
    mov rsp, rbp
    pop rbp
    ret

clamp_bb_5:
    jmp clamp_bb_6

clamp_bb_6:
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: Result_unwrap ───
Result_unwrap:
    push rbp
    mov rbp, rsp
    sub rsp, 224
    mov [rbp-168], rcx
    mov [rbp-96], rdx

Result_unwrap_bb_0:
    mov rax, [rbp-96]
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rax, [rax]
    mov [rbp-80], rax
    mov rax, 0
    mov [rbp-176], rax
    mov rax, [rbp-80]
    mov rcx, [rbp-176]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-72], rax
    mov rax, [rbp-72]
    test rax, rax
    jnz Result_unwrap_bb_2
    jmp Result_unwrap_bb_3

Result_unwrap_bb_1:
    mov rax, 0
    mov [rbp-120], rax
    mov rax, [rbp-120]
    mov rsp, rbp
    pop rbp
    ret

Result_unwrap_bb_2:
    mov rax, [rbp-96]
    add rax, 8
    mov [rbp-32], rax
    mov rax, [rbp-32]
    mov rax, [rax]
    mov [rbp-184], rax
    mov rax, [rbp-184]
    mov rsp, rbp
    pop rbp
    ret

Result_unwrap_bb_3:
    mov rax, [rbp-96]
    mov [rbp-104], rax
    mov rax, [rbp-104]
    mov rax, [rax]
    mov [rbp-144], rax
    mov rax, 1
    mov [rbp-136], rax
    mov rax, [rbp-144]
    mov rcx, [rbp-136]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-152], rax
    mov rax, [rbp-152]
    test rax, rax
    jnz Result_unwrap_bb_4
    jmp Result_unwrap_bb_5

Result_unwrap_bb_4:
    mov rax, [rbp-96]
    add rax, 8
    mov [rbp-160], rax
    mov rax, [rbp-160]
    mov rax, [rax]
    mov [rbp-8], rax
    lea rax, str_0
    mov [rbp-128], rax
    mov rcx, [rbp-128]
    sub rsp, 32  ; shadow space + stack args
    call p_print_str
    add rsp, 32  ; cleanup
    mov [rbp-48], rax
    mov rax, 0
    mov [rbp-192], rax
    mov rax, [rbp-192]
    mov rsp, rbp
    pop rbp
    ret

Result_unwrap_bb_5:
    jmp Result_unwrap_bb_1
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: Point_format ───
Point_format:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    mov [rbp-56], rcx
    mov [rbp-16], rdx

Point_format_bb_0:
    mov rax, [rbp-16]
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rax, [rax]
    mov [rbp-32], rax
    mov rax, [rbp-16]
    add rax, 8
    mov [rbp-64], rax
    mov rax, [rbp-64]
    mov rax, [rax]
    mov [rbp-40], rax
    mov rax, [rbp-32]
    mov rcx, [rbp-40]
    add rax, rcx
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: Option_is_none ───
Option_is_none:
    push rbp
    mov rbp, rsp
    sub rsp, 208
    mov [rbp-32], rcx
    mov [rbp-8], rdx

Option_is_none_bb_0:
    mov rax, [rbp-8]
    mov [rbp-120], rax
    mov rax, [rbp-120]
    mov rax, [rax]
    mov [rbp-96], rax
    mov rax, 0
    mov [rbp-16], rax
    mov rax, [rbp-96]
    mov rcx, [rbp-16]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-104], rax
    mov rax, [rbp-104]
    test rax, rax
    jnz Option_is_none_bb_2
    jmp Option_is_none_bb_3

Option_is_none_bb_1:
    mov rax, 0
    mov [rbp-168], rax
    mov rax, [rbp-168]
    mov rsp, rbp
    pop rbp
    ret

Option_is_none_bb_2:
    mov rax, [rbp-8]
    add rax, 8
    mov [rbp-56], rax
    mov rax, [rbp-56]
    mov rax, [rax]
    mov [rbp-160], rax
    mov rax, 0
    mov [rbp-64], rax
    mov rax, [rbp-64]
    mov rsp, rbp
    pop rbp
    ret

Option_is_none_bb_3:
    mov rax, [rbp-8]
    mov [rbp-136], rax
    mov rax, [rbp-136]
    mov rax, [rax]
    mov [rbp-80], rax
    mov rax, 1
    mov [rbp-40], rax
    mov rax, [rbp-80]
    mov rcx, [rbp-40]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-24], rax
    mov rax, [rbp-24]
    test rax, rax
    jnz Option_is_none_bb_4
    jmp Option_is_none_bb_5

Option_is_none_bb_4:
    mov rax, 1
    mov [rbp-152], rax
    mov rax, [rbp-152]
    mov rsp, rbp
    pop rbp
    ret

Option_is_none_bb_5:
    jmp Option_is_none_bb_1
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: print_value__Point ───
print_value__Point:
    push rbp
    mov rbp, rsp
    sub rsp, 80
    mov [rbp-40], rcx
    mov [rbp-8], rdx

print_value__Point_bb_0:
    mov rax, 0
    mov [rbp-16], rax
    mov rcx, [rbp-16]
    mov rdx, [rbp-8]
    sub rsp, 32  ; shadow space + stack args
    call Point_format
    add rsp, 32  ; cleanup
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: Option_unwrap ───
Option_unwrap:
    push rbp
    mov rbp, rsp
    sub rsp, 208
    mov [rbp-88], rcx
    mov [rbp-24], rdx

Option_unwrap_bb_0:
    mov rax, [rbp-24]
    mov [rbp-56], rax
    mov rax, [rbp-56]
    mov rax, [rax]
    mov [rbp-160], rax
    mov rax, 0
    mov [rbp-16], rax
    mov rax, [rbp-160]
    mov rcx, [rbp-16]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-96], rax
    mov rax, [rbp-96]
    test rax, rax
    jnz Option_unwrap_bb_2
    jmp Option_unwrap_bb_3

Option_unwrap_bb_1:
    mov rax, 0
    mov [rbp-120], rax
    mov rax, [rbp-120]
    mov rsp, rbp
    pop rbp
    ret

Option_unwrap_bb_2:
    mov rax, [rbp-24]
    add rax, 8
    mov [rbp-136], rax
    mov rax, [rbp-136]
    mov rax, [rax]
    mov [rbp-112], rax
    mov rax, [rbp-112]
    mov rsp, rbp
    pop rbp
    ret

Option_unwrap_bb_3:
    mov rax, [rbp-24]
    mov [rbp-144], rax
    mov rax, [rbp-144]
    mov rax, [rax]
    mov [rbp-168], rax
    mov rax, 1
    mov [rbp-72], rax
    mov rax, [rbp-168]
    mov rcx, [rbp-72]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-128], rax
    mov rax, [rbp-128]
    test rax, rax
    jnz Option_unwrap_bb_4
    jmp Option_unwrap_bb_5

Option_unwrap_bb_4:
    lea rax, str_1
    mov [rbp-80], rax
    mov rcx, [rbp-80]
    sub rsp, 32  ; shadow space + stack args
    call p_print_str
    add rsp, 32  ; cleanup
    mov [rbp-8], rax
    mov rax, 0
    mov [rbp-40], rax
    mov rax, [rbp-40]
    mov rsp, rbp
    pop rbp
    ret

Option_unwrap_bb_5:
    jmp Option_unwrap_bb_1
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: add_value__Point ───
add_value__Point:
    push rbp
    mov rbp, rsp
    sub rsp, 80
    mov [rbp-8], rcx
    mov [rbp-16], rdx
    mov [rbp-40], r8

add_value__Point_bb_0:
    mov rax, 0
    mov [rbp-24], rax
    mov rcx, [rbp-24]
    mov rdx, [rbp-16]
    mov r8, [rbp-40]
    sub rsp, 32  ; shadow space + stack args
    call Point_add
    add rsp, 32  ; cleanup
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: max ───
max:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    mov [rbp-24], rcx
    mov [rbp-40], rdx
    mov [rbp-48], r8

max_bb_0:
    mov rax, [rbp-40]
    mov rcx, [rbp-48]
    cmp rax, rcx
    setg al
    movzx eax, al
    mov [rbp-64], rax
    mov rax, [rbp-64]
    test rax, rax
    jnz max_bb_1
    jmp max_bb_2

max_bb_1:
    mov rax, [rbp-40]
    mov rsp, rbp
    pop rbp
    ret

max_bb_2:
    jmp max_bb_3

max_bb_3:
    mov rax, [rbp-48]
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: Result_err ───
Result_err:
    push rbp
    mov rbp, rsp
    sub rsp, 448
    mov [rbp-136], rcx
    mov [rbp-312], rdx
    mov [rbp-352], r8

Result_err_bb_0:
    mov rax, [rbp-352]
    mov [rbp-280], rax
    mov rax, [rbp-280]
    mov rax, [rax]
    mov [rbp-200], rax
    mov rax, 0
    mov [rbp-128], rax
    mov rax, [rbp-200]
    mov rcx, [rbp-128]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-160], rax
    mov rax, [rbp-160]
    test rax, rax
    jnz Result_err_bb_2
    jmp Result_err_bb_3

Result_err_bb_1:
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

Result_err_bb_2:
    mov rax, [rbp-352]
    add rax, 8
    mov [rbp-168], rax
    mov rax, [rbp-168]
    mov rax, [rax]
    mov [rbp-104], rax
    lea rax, [rbp-392]
    mov [rbp-320], rax
    mov rax, 1
    mov [rbp-40], rax
    mov rax, [rbp-320]
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov rcx, [rbp-40]
    mov [rax], rcx
    mov rax, [rbp-320]
    mov [rbp-152], rax
    mov rax, [rbp-136]
    mov [rbp-256], rax
    mov rax, [rbp-152]
    mov rax, [rax]
    mov [rbp-328], rax
    mov rax, [rbp-256]
    mov rcx, [rbp-328]
    mov [rax], rcx
    mov rax, [rbp-320]
    add rax, 8
    mov [rbp-176], rax
    mov rax, [rbp-136]
    add rax, 8
    mov [rbp-272], rax
    mov rax, [rbp-176]
    mov rax, [rax]
    mov [rbp-376], rax
    mov rax, [rbp-272]
    mov rcx, [rbp-376]
    mov [rax], rcx
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

Result_err_bb_3:
    mov rax, [rbp-352]
    mov [rbp-192], rax
    mov rax, [rbp-192]
    mov rax, [rax]
    mov [rbp-368], rax
    mov rax, 1
    mov [rbp-360], rax
    mov rax, [rbp-368]
    mov rcx, [rbp-360]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-56], rax
    mov rax, [rbp-56]
    test rax, rax
    jnz Result_err_bb_4
    jmp Result_err_bb_5

Result_err_bb_4:
    mov rax, [rbp-352]
    add rax, 8
    mov [rbp-240], rax
    mov rax, [rbp-240]
    mov rax, [rax]
    mov [rbp-216], rax
    lea rax, [rbp-408]
    mov [rbp-184], rax
    mov rax, 0
    mov [rbp-208], rax
    mov rax, [rbp-184]
    mov [rbp-64], rax
    mov rax, [rbp-64]
    mov rcx, [rbp-208]
    mov [rax], rcx
    mov rax, [rbp-184]
    add rax, 8
    mov [rbp-144], rax
    mov rax, [rbp-144]
    mov rcx, [rbp-216]
    mov [rax], rcx
    mov rax, [rbp-184]
    mov [rbp-8], rax
    mov rax, [rbp-136]
    mov [rbp-296], rax
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-248], rax
    mov rax, [rbp-296]
    mov rcx, [rbp-248]
    mov [rax], rcx
    mov rax, [rbp-184]
    add rax, 8
    mov [rbp-72], rax
    mov rax, [rbp-136]
    add rax, 8
    mov [rbp-88], rax
    mov rax, [rbp-72]
    mov rax, [rax]
    mov [rbp-304], rax
    mov rax, [rbp-88]
    mov rcx, [rbp-304]
    mov [rax], rcx
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

Result_err_bb_5:
    jmp Result_err_bb_1
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: println ───
println:
    push rbp
    mov rbp, rsp
    sub rsp, 80
    mov [rbp-16], rcx
    mov [rbp-8], rdx

println_bb_0:
    mov rcx, [rbp-8]
    sub rsp, 32  ; shadow space + stack args
    call p_print_str
    add rsp, 32  ; cleanup
    mov [rbp-24], rax
    lea rax, str_2
    mov [rbp-32], rax
    mov rcx, [rbp-32]
    sub rsp, 32  ; shadow space + stack args
    call p_print_str
    add rsp, 32  ; cleanup
    mov [rbp-40], rax
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: Option_map ───
Option_map:
    push rbp
    mov rbp, rsp
    sub rsp, 448
    mov [rbp-176], rcx
    mov [rbp-304], rdx
    mov [rbp-120], r8
    mov [rbp-16], r9

Option_map_bb_0:
    mov rax, [rbp-120]
    mov [rbp-328], rax
    mov rax, [rbp-328]
    mov rax, [rax]
    mov [rbp-320], rax
    mov rax, 0
    mov [rbp-136], rax
    mov rax, [rbp-320]
    mov rcx, [rbp-136]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-296], rax
    mov rax, [rbp-296]
    test rax, rax
    jnz Option_map_bb_2
    jmp Option_map_bb_3

Option_map_bb_1:
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

Option_map_bb_2:
    mov rax, [rbp-120]
    add rax, 8
    mov [rbp-128], rax
    mov rax, [rbp-128]
    mov rax, [rax]
    mov [rbp-112], rax
    lea rax, [rbp-408]
    mov [rbp-208], rax
    mov rax, 0
    mov [rbp-216], rax
    mov rax, [rbp-208]
    mov [rbp-152], rax
    mov rax, [rbp-152]
    mov rcx, [rbp-216]
    mov [rax], rcx
    mov rax, [rbp-112]
    mov rcx, [rbp-16]
    add rax, rcx
    mov [rbp-200], rax
    mov rax, [rbp-208]
    add rax, 8
    mov [rbp-280], rax
    mov rax, [rbp-280]
    mov rcx, [rbp-200]
    mov [rax], rcx
    mov rax, [rbp-208]
    mov [rbp-8], rax
    mov rax, [rbp-176]
    mov [rbp-64], rax
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-336], rax
    mov rax, [rbp-64]
    mov rcx, [rbp-336]
    mov [rax], rcx
    mov rax, [rbp-208]
    add rax, 8
    mov [rbp-40], rax
    mov rax, [rbp-176]
    add rax, 8
    mov [rbp-368], rax
    mov rax, [rbp-40]
    mov rax, [rax]
    mov [rbp-144], rax
    mov rax, [rbp-368]
    mov rcx, [rbp-144]
    mov [rax], rcx
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

Option_map_bb_3:
    mov rax, [rbp-120]
    mov [rbp-168], rax
    mov rax, [rbp-168]
    mov rax, [rax]
    mov [rbp-344], rax
    mov rax, 1
    mov [rbp-24], rax
    mov rax, [rbp-344]
    mov rcx, [rbp-24]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-256], rax
    mov rax, [rbp-256]
    test rax, rax
    jnz Option_map_bb_4
    jmp Option_map_bb_5

Option_map_bb_4:
    lea rax, [rbp-392]
    mov [rbp-264], rax
    mov rax, 1
    mov [rbp-192], rax
    mov rax, [rbp-264]
    mov [rbp-240], rax
    mov rax, [rbp-240]
    mov rcx, [rbp-192]
    mov [rax], rcx
    mov rax, [rbp-264]
    mov [rbp-160], rax
    mov rax, [rbp-176]
    mov [rbp-376], rax
    mov rax, [rbp-160]
    mov rax, [rax]
    mov [rbp-248], rax
    mov rax, [rbp-376]
    mov rcx, [rbp-248]
    mov [rax], rcx
    mov rax, [rbp-264]
    add rax, 8
    mov [rbp-72], rax
    mov rax, [rbp-176]
    add rax, 8
    mov [rbp-272], rax
    mov rax, [rbp-72]
    mov rax, [rax]
    mov [rbp-288], rax
    mov rax, [rbp-272]
    mov rcx, [rbp-288]
    mov [rax], rcx
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

Option_map_bb_5:
    jmp Option_map_bb_1
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: Result_is_ok ───
Result_is_ok:
    push rbp
    mov rbp, rsp
    sub rsp, 224
    mov [rbp-160], rcx
    mov [rbp-56], rdx

Result_is_ok_bb_0:
    mov rax, [rbp-56]
    mov [rbp-64], rax
    mov rax, [rbp-64]
    mov rax, [rax]
    mov [rbp-8], rax
    mov rax, 0
    mov [rbp-88], rax
    mov rax, [rbp-8]
    mov rcx, [rbp-88]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-120], rax
    mov rax, [rbp-120]
    test rax, rax
    jnz Result_is_ok_bb_2
    jmp Result_is_ok_bb_3

Result_is_ok_bb_1:
    mov rax, 0
    mov [rbp-104], rax
    mov rax, [rbp-104]
    mov rsp, rbp
    pop rbp
    ret

Result_is_ok_bb_2:
    mov rax, [rbp-56]
    add rax, 8
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rax, [rax]
    mov [rbp-112], rax
    mov rax, 1
    mov [rbp-136], rax
    mov rax, [rbp-136]
    mov rsp, rbp
    pop rbp
    ret

Result_is_ok_bb_3:
    mov rax, [rbp-56]
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov rax, [rax]
    mov [rbp-184], rax
    mov rax, 1
    mov [rbp-32], rax
    mov rax, [rbp-184]
    mov rcx, [rbp-32]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-72], rax
    mov rax, [rbp-72]
    test rax, rax
    jnz Result_is_ok_bb_4
    jmp Result_is_ok_bb_5

Result_is_ok_bb_4:
    mov rax, [rbp-56]
    add rax, 8
    mov [rbp-40], rax
    mov rax, [rbp-40]
    mov rax, [rax]
    mov [rbp-176], rax
    mov rax, 0
    mov [rbp-152], rax
    mov rax, [rbp-152]
    mov rsp, rbp
    pop rbp
    ret

Result_is_ok_bb_5:
    jmp Result_is_ok_bb_1
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: abs ───
abs:
    push rbp
    mov rbp, rsp
    sub rsp, 112
    mov [rbp-8], rcx
    mov [rbp-64], rdx

abs_bb_0:
    mov rax, 0
    mov [rbp-32], rax
    mov rax, [rbp-64]
    mov rcx, [rbp-32]
    cmp rax, rcx
    setl al
    movzx eax, al
    mov [rbp-72], rax
    mov rax, [rbp-72]
    test rax, rax
    jnz abs_bb_1
    jmp abs_bb_2

abs_bb_1:
    mov rax, [rbp-64]
    neg rax
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rsp, rbp
    pop rbp
    ret

abs_bb_2:
    jmp abs_bb_3

abs_bb_3:
    mov rax, [rbp-64]
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: Result_ok ───
Result_ok:
    push rbp
    mov rbp, rsp
    sub rsp, 448
    mov [rbp-40], rcx
    mov [rbp-96], rdx
    mov [rbp-120], r8

Result_ok_bb_0:
    mov rax, [rbp-120]
    mov [rbp-304], rax
    mov rax, [rbp-304]
    mov rax, [rax]
    mov [rbp-280], rax
    mov rax, 0
    mov [rbp-232], rax
    mov rax, [rbp-280]
    mov rcx, [rbp-232]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-312], rax
    mov rax, [rbp-312]
    test rax, rax
    jnz Result_ok_bb_2
    jmp Result_ok_bb_3

Result_ok_bb_1:
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

Result_ok_bb_2:
    mov rax, [rbp-120]
    add rax, 8
    mov [rbp-88], rax
    mov rax, [rbp-88]
    mov rax, [rax]
    mov [rbp-320], rax
    lea rax, [rbp-392]
    mov [rbp-104], rax
    mov rax, 0
    mov [rbp-48], rax
    mov rax, [rbp-104]
    mov [rbp-328], rax
    mov rax, [rbp-328]
    mov rcx, [rbp-48]
    mov [rax], rcx
    mov rax, [rbp-104]
    add rax, 8
    mov [rbp-32], rax
    mov rax, [rbp-32]
    mov rcx, [rbp-320]
    mov [rax], rcx
    mov rax, [rbp-104]
    mov [rbp-136], rax
    mov rax, [rbp-40]
    mov [rbp-56], rax
    mov rax, [rbp-136]
    mov rax, [rax]
    mov [rbp-72], rax
    mov rax, [rbp-56]
    mov rcx, [rbp-72]
    mov [rax], rcx
    mov rax, [rbp-104]
    add rax, 8
    mov [rbp-200], rax
    mov rax, [rbp-40]
    add rax, 8
    mov [rbp-208], rax
    mov rax, [rbp-200]
    mov rax, [rax]
    mov [rbp-128], rax
    mov rax, [rbp-208]
    mov rcx, [rbp-128]
    mov [rax], rcx
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

Result_ok_bb_3:
    mov rax, [rbp-120]
    mov [rbp-64], rax
    mov rax, [rbp-64]
    mov rax, [rax]
    mov [rbp-240], rax
    mov rax, 1
    mov [rbp-112], rax
    mov rax, [rbp-240]
    mov rcx, [rbp-112]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-16], rax
    mov rax, [rbp-16]
    test rax, rax
    jnz Result_ok_bb_4
    jmp Result_ok_bb_5

Result_ok_bb_4:
    mov rax, [rbp-120]
    add rax, 8
    mov [rbp-80], rax
    mov rax, [rbp-80]
    mov rax, [rax]
    mov [rbp-152], rax
    lea rax, [rbp-408]
    mov [rbp-160], rax
    mov rax, 1
    mov [rbp-272], rax
    mov rax, [rbp-160]
    mov [rbp-184], rax
    mov rax, [rbp-184]
    mov rcx, [rbp-272]
    mov [rax], rcx
    mov rax, [rbp-160]
    mov [rbp-368], rax
    mov rax, [rbp-40]
    mov [rbp-168], rax
    mov rax, [rbp-368]
    mov rax, [rax]
    mov [rbp-8], rax
    mov rax, [rbp-168]
    mov rcx, [rbp-8]
    mov [rax], rcx
    mov rax, [rbp-160]
    add rax, 8
    mov [rbp-288], rax
    mov rax, [rbp-40]
    add rax, 8
    mov [rbp-248], rax
    mov rax, [rbp-288]
    mov rax, [rax]
    mov [rbp-24], rax
    mov rax, [rbp-248]
    mov rcx, [rbp-24]
    mov [rax], rcx
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

Result_ok_bb_5:
    jmp Result_ok_bb_1
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: Result_unwrap_or ───
Result_unwrap_or:
    push rbp
    mov rbp, rsp
    sub rsp, 208
    mov [rbp-40], rcx
    mov [rbp-8], rdx
    mov [rbp-144], r8

Result_unwrap_or_bb_0:
    mov rax, [rbp-8]
    mov [rbp-72], rax
    mov rax, [rbp-72]
    mov rax, [rax]
    mov [rbp-120], rax
    mov rax, 0
    mov [rbp-136], rax
    mov rax, [rbp-120]
    mov rcx, [rbp-136]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-128], rax
    mov rax, [rbp-128]
    test rax, rax
    jnz Result_unwrap_or_bb_2
    jmp Result_unwrap_or_bb_3

Result_unwrap_or_bb_1:
    mov rax, 0
    mov [rbp-176], rax
    mov rax, [rbp-176]
    mov rsp, rbp
    pop rbp
    ret

Result_unwrap_or_bb_2:
    mov rax, [rbp-8]
    add rax, 8
    mov [rbp-168], rax
    mov rax, [rbp-168]
    mov rax, [rax]
    mov [rbp-160], rax
    mov rax, [rbp-160]
    mov rsp, rbp
    pop rbp
    ret

Result_unwrap_or_bb_3:
    mov rax, [rbp-8]
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rax, [rax]
    mov [rbp-64], rax
    mov rax, 1
    mov [rbp-104], rax
    mov rax, [rbp-64]
    mov rcx, [rbp-104]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-112], rax
    mov rax, [rbp-112]
    test rax, rax
    jnz Result_unwrap_or_bb_4
    jmp Result_unwrap_or_bb_5

Result_unwrap_or_bb_4:
    mov rax, [rbp-8]
    add rax, 8
    mov [rbp-56], rax
    mov rax, [rbp-56]
    mov rax, [rax]
    mov [rbp-88], rax
    mov rax, [rbp-144]
    mov rsp, rbp
    pop rbp
    ret

Result_unwrap_or_bb_5:
    jmp Result_unwrap_or_bb_1
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: Result_is_err ───
Result_is_err:
    push rbp
    mov rbp, rsp
    sub rsp, 224
    mov [rbp-16], rcx
    mov [rbp-144], rdx

Result_is_err_bb_0:
    mov rax, [rbp-144]
    mov [rbp-56], rax
    mov rax, [rbp-56]
    mov rax, [rax]
    mov [rbp-24], rax
    mov rax, 0
    mov [rbp-184], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-184]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-152], rax
    mov rax, [rbp-152]
    test rax, rax
    jnz Result_is_err_bb_2
    jmp Result_is_err_bb_3

Result_is_err_bb_1:
    mov rax, 0
    mov [rbp-120], rax
    mov rax, [rbp-120]
    mov rsp, rbp
    pop rbp
    ret

Result_is_err_bb_2:
    mov rax, [rbp-144]
    add rax, 8
    mov [rbp-168], rax
    mov rax, [rbp-168]
    mov rax, [rax]
    mov [rbp-72], rax
    mov rax, 0
    mov [rbp-112], rax
    mov rax, [rbp-112]
    mov rsp, rbp
    pop rbp
    ret

Result_is_err_bb_3:
    mov rax, [rbp-144]
    mov [rbp-160], rax
    mov rax, [rbp-160]
    mov rax, [rax]
    mov [rbp-80], rax
    mov rax, 1
    mov [rbp-8], rax
    mov rax, [rbp-80]
    mov rcx, [rbp-8]
    cmp rax, rcx
    sete al
    movzx eax, al
    mov [rbp-40], rax
    mov rax, [rbp-40]
    test rax, rax
    jnz Result_is_err_bb_4
    jmp Result_is_err_bb_5

Result_is_err_bb_4:
    mov rax, [rbp-144]
    add rax, 8
    mov [rbp-88], rax
    mov rax, [rbp-88]
    mov rax, [rax]
    mov [rbp-96], rax
    mov rax, 1
    mov [rbp-136], rax
    mov rax, [rbp-136]
    mov rsp, rbp
    pop rbp
    ret

Result_is_err_bb_5:
    jmp Result_is_err_bb_1
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: main ───
main:
    push rbp
    mov rbp, rsp
    sub rsp, 352
    mov [rbp-264], rcx

main_bb_0:
    mov rax, 10
    mov [rbp-168], rax
    mov rax, 20
    mov [rbp-200], rax
    lea rax, [rbp-304]
    mov [rbp-128], rax
    mov rax, 0
    mov [rbp-24], rax
    mov rcx, [rbp-128]
    mov rdx, [rbp-24]
    mov r8, [rbp-168]
    mov r9, [rbp-200]
    sub rsp, 32  ; shadow space + stack args
    call Point_new
    add rsp, 32  ; cleanup
    mov [rbp-248], rax
    lea rax, [rbp-288]
    mov [rbp-256], rax
    mov rax, [rbp-256]
    mov rcx, [rbp-128]
    mov [rax], rcx
    mov rax, [rbp-256]
    mov rax, [rax]
    mov [rbp-240], rax
    mov rax, 0
    mov [rbp-112], rax
    mov rcx, [rbp-112]
    mov rdx, [rbp-240]
    sub rsp, 32  ; shadow space + stack args
    call Point_format
    add rsp, 32  ; cleanup
    mov [rbp-152], rax
    mov rcx, [rbp-152]
    sub rsp, 32  ; shadow space + stack args
    call p_print_int
    add rsp, 32  ; cleanup
    mov [rbp-88], rax
    mov rax, [rbp-256]
    mov rax, [rax]
    mov [rbp-208], rax
    mov rax, 5
    mov [rbp-144], rax
    mov rax, 0
    mov [rbp-80], rax
    mov rcx, [rbp-80]
    mov rdx, [rbp-208]
    mov r8, [rbp-144]
    sub rsp, 32  ; shadow space + stack args
    call Point_add
    add rsp, 32  ; cleanup
    mov [rbp-272], rax
    mov rcx, [rbp-272]
    sub rsp, 32  ; shadow space + stack args
    call p_print_int
    add rsp, 32  ; cleanup
    mov [rbp-232], rax
    mov rax, [rbp-256]
    mov rax, [rax]
    mov [rbp-184], rax
    mov rax, 0
    mov [rbp-72], rax
    mov rcx, [rbp-72]
    mov rdx, [rbp-184]
    sub rsp, 32  ; shadow space + stack args
    call print_value__Point
    add rsp, 32  ; cleanup
    mov [rbp-8], rax
    lea rax, [rbp-280]
    mov [rbp-160], rax
    mov rax, [rbp-160]
    mov rcx, [rbp-8]
    mov [rax], rcx
    mov rax, [rbp-160]
    mov rax, [rax]
    mov [rbp-48], rax
    mov rcx, [rbp-48]
    sub rsp, 32  ; shadow space + stack args
    call p_print_int
    add rsp, 32  ; cleanup
    mov [rbp-104], rax
    mov rax, [rbp-256]
    mov rax, [rax]
    mov [rbp-176], rax
    mov rax, 100
    mov [rbp-120], rax
    mov rax, 0
    mov [rbp-216], rax
    mov rcx, [rbp-216]
    mov rdx, [rbp-176]
    mov r8, [rbp-120]
    sub rsp, 32  ; shadow space + stack args
    call add_value__Point
    add rsp, 32  ; cleanup
    mov [rbp-136], rax
    lea rax, [rbp-312]
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-136]
    mov [rax], rcx
    mov rax, [rbp-16]
    mov rax, [rax]
    mov [rbp-56], rax
    mov rcx, [rbp-56]
    sub rsp, 32  ; shadow space + stack args
    call p_print_int
    add rsp, 32  ; cleanup
    mov [rbp-224], rax
    mov rax, 0
    mov [rbp-32], rax
    mov rax, [rbp-32]
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: assert ───
assert:
    push rbp
    mov rbp, rsp
    sub rsp, 112
    mov [rbp-40], rcx
    mov [rbp-48], rdx

assert_bb_0:
    mov rax, [rbp-48]
    xor rax, 1
    mov [rbp-16], rax
    mov rax, [rbp-16]
    test rax, rax
    jnz assert_bb_1
    jmp assert_bb_2

assert_bb_1:
    lea rax, str_3
    mov [rbp-32], rax
    mov rcx, [rbp-32]
    sub rsp, 32  ; shadow space + stack args
    call p_print_str
    add rsp, 32  ; cleanup
    mov [rbp-64], rax
    jmp assert_bb_3

assert_bb_2:
    jmp assert_bb_3

assert_bb_3:
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret

; ─── Function: Point_add ───
Point_add:
    push rbp
    mov rbp, rsp
    sub rsp, 112
    mov [rbp-64], rcx
    mov [rbp-8], rdx
    mov [rbp-56], r8

Point_add_bb_0:
    mov rax, [rbp-8]
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rax, [rax]
    mov [rbp-24], rax
    mov rax, [rbp-8]
    add rax, 8
    mov [rbp-80], rax
    mov rax, [rbp-80]
    mov rax, [rax]
    mov [rbp-32], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-32]
    add rax, rcx
    mov [rbp-72], rax
    mov rax, [rbp-72]
    mov rcx, [rbp-56]
    add rax, rcx
    mov [rbp-40], rax
    mov rax, [rbp-40]
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret

; ─── Runtime stubs (direct Windows API) ───
p_print_int:
    push rbp
    mov rbp, rsp
    push r12
    push r13
    sub rsp, 80  ; buffer + shadow space + alignment

    mov rax, rcx  ; value
    lea r8, [rbp-1]  ; buffer end pointer
    mov byte ptr [r8], 10  ; newline at end
    mov r9, 10  ; divisor

    test rax, rax
    jns pi_pos
    neg rax
pi_pos:
pi_loop:
    dec r8
    xor rdx, rdx
    div r9
    add dl, '0'
    mov [r8], dl
    test rax, rax
    jnz pi_loop

    test rcx, rcx
    jns pi_no_neg
    dec r8
    mov byte ptr [r8], '-'
pi_no_neg:

    ; WriteFile(stdout, r8, len, &written, NULL)
    mov ecx, -11
    sub rsp, 32
    call GetStdHandle
    add rsp, 32
    mov r12, rax
    lea r9, [rbp]  ; end (after newline)
    sub r9, r8  ; length
    mov rcx, r12
    mov rdx, r8
    mov r8, r9
    lea r9, [rbp-32]
    mov dword ptr [r9], 0
    sub rsp, 48  ; shadow space + 5th arg + alignment
    mov qword ptr [rsp+32], 0  ; lpOverlapped = NULL
    call WriteFile
    add rsp, 48
    xor rax, rax
    add rsp, 80
    pop r13
    pop r12
    mov rsp, rbp
    pop rbp
    ret

p_print_str:
    push rbp
    mov rbp, rsp
    push r12
    push r13
    sub rsp, 64
    mov r12, rcx  ; save string pointer

    ; Calculate string length
    xor r8, r8
ps_len:
    cmp byte ptr [r12 + r8], 0
    je ps_done
    inc r8
    jmp ps_len
ps_done:
    mov r13, r8  ; save length

    ; WriteFile string
    mov ecx, -11
    sub rsp, 32
    call GetStdHandle
    add rsp, 32
    mov rcx, rax  ; handle
    mov rdx, r12  ; buffer
    mov r8, r13  ; length
    lea r9, [rbp-32]
    mov dword ptr [r9], 0
    sub rsp, 48  ; shadow space + 5th arg + alignment
    mov qword ptr [rsp+32], 0  ; lpOverlapped = NULL
    call WriteFile
    add rsp, 48

    ; Print newline
    mov byte ptr [rbp-33], 10
    mov ecx, -11
    sub rsp, 32
    call GetStdHandle
    add rsp, 32
    mov rcx, rax
    lea rdx, [rbp-33]
    mov r8, 1
    lea r9, [rbp-40]
    mov dword ptr [r9], 0
    sub rsp, 48  ; shadow space + 5th arg + alignment
    mov qword ptr [rsp+32], 0  ; lpOverlapped = NULL
    call WriteFile
    add rsp, 48
    xor rax, rax
    add rsp, 64
    pop r13
    pop r12
    mov rsp, rbp
    pop rbp
    ret

p_print_bool:
    push rbp
    mov rbp, rsp
    push r12
    push r13
    sub rsp, 64
    test rcx, rcx
    jnz pb_true
    lea r12, str_false
    jmp pb_print
pb_true:
    lea r12, str_true
pb_print:
    xor r8, r8
pb_len:
    cmp byte ptr [r12 + r8], 0
    je pb_done
    inc r8
    jmp pb_len
pb_done:
    mov r13, r8
    mov ecx, -11
    sub rsp, 32
    call GetStdHandle
    add rsp, 32
    mov rcx, rax
    mov rdx, r12
    mov r8, r13
    lea r9, [rbp-32]
    mov dword ptr [r9], 0
    sub rsp, 48  ; shadow space + 5th arg + alignment
    mov qword ptr [rsp+32], 0  ; lpOverlapped = NULL
    call WriteFile
    add rsp, 48
    ; newline
    mov byte ptr [rbp-33], 10
    mov ecx, -11
    sub rsp, 32
    call GetStdHandle
    add rsp, 32
    mov rcx, rax
    lea rdx, [rbp-33]
    mov r8, 1
    lea r9, [rbp-40]
    mov dword ptr [r9], 0
    sub rsp, 48  ; shadow space + 5th arg + alignment
    mov qword ptr [rsp+32], 0  ; lpOverlapped = NULL
    call WriteFile
    add rsp, 48
    xor rax, rax
    add rsp, 64
    pop r13
    pop r12
    mov rsp, rbp
    pop rbp
    ret

p_print_float:
    push rbp
    mov rbp, rsp
    push r12
    push r13
    push r14
    sub rsp, 152  ; buffer(64) + temp(32) + locals(56), 16-byte aligned

    ; Save float bits and load as xmm0
    mov [rbp-72], rcx
    movq xmm0, qword ptr [rbp-72]

    ; r12 = buffer pointer, r13 = length
    lea r12, [rbp-64]
    xor r13, r13

    ; Check negative
    xorpd xmm1, xmm1
    ucomisd xmm0, xmm1
    jae pf_pos
    mov byte ptr [r12], '-'
    inc r12
    inc r13
    mov rax, 8000000000000000h
    mov [rbp-80], rax
    xorpd xmm0, xmmword ptr [rbp-80]

pf_pos:
    ; Save xmm0 (possibly negated)
    movq qword ptr [rbp-72], xmm0

    ; Get integer part
    cvttsd2si rax, xmm0
    mov r14, rax  ; save integer part

    ; Handle integer part == 0
    test rax, rax
    jnz pf_int_start
    mov byte ptr [r12], '0'
    inc r12
    inc r13
    jmp pf_dot

pf_int_start:
    ; Convert integer to reversed string at [rbp-128]
    lea r8, [rbp-128]
    xor r9, r9
pf_int_loop:
    xor rdx, rdx
    mov rcx, 10
    div rcx
    add dl, '0'
    mov [r8], dl
    inc r8
    inc r9
    test rax, rax
    jnz pf_int_loop

    ; Copy reversed digits to buffer in correct order
    dec r8
pf_int_copy:
    mov al, [r8]
    mov [r12], al
    inc r12
    inc r13
    dec r8
    dec r9
    jnz pf_int_copy

pf_dot:
    mov byte ptr [r12], '.'
    inc r12
    inc r13

    ; Get fractional part: xmm0 = original - integer_part
    movq xmm0, qword ptr [rbp-72]
    cvtsi2sd xmm1, r14
    subsd xmm0, xmm1
    ; Clamp negative to 0 (FP precision)
    xorpd xmm1, xmm1
    ucomisd xmm0, xmm1
    jae pf_frac_ok
    movq xmm0, xmm1
pf_frac_ok:

    ; Load 1000000.0 constant
    mov rax, 412E848000000000h
    mov [rbp-80], rax
    movq xmm2, qword ptr [rbp-80]  ; xmm2 = 1000000.0

    ; Multiply fractional part by 1000000
    mulsd xmm0, xmm2

    ; Add 0.5 for rounding
    mov rax, 3FE0000000000000h
    mov [rbp-80], rax
    movq xmm1, qword ptr [rbp-80]  ; xmm1 = 0.5
    addsd xmm0, xmm1

    ; Truncate to integer
    cvttsd2si rax, xmm0  ; rax = rounded fractional * 10^6

    ; Extract 6 digits from rax (reversed into temp buffer)
    lea r8, [rbp-128]
    mov rcx, 6
    mov r9, 10
pf_frac_loop:
    xor rdx, rdx
    div r9
    add dl, '0'
    mov [r8], dl
    inc r8
    dec rcx
    jnz pf_frac_loop

    ; Copy digits in reverse order to output buffer
    dec r8
    mov rcx, 6
pf_frac_copy:
    mov al, [r8]
    mov [r12], al
    inc r12
    inc r13
    dec r8
    dec rcx
    jnz pf_frac_copy

    ; Add newline
    mov byte ptr [r12], 10
    inc r13

    ; WriteFile(stdout, buffer, length, &written, NULL)
    mov ecx, -11
    sub rsp, 32
    call GetStdHandle
    add rsp, 32
    mov rcx, rax  ; handle
    lea rdx, [rbp-64]  ; buffer
    mov r8, r13  ; length
    lea r9, [rbp-32]
    mov dword ptr [r9], 0
    sub rsp, 48  ; shadow space + 5th arg + alignment
    mov qword ptr [rsp+32], 0  ; lpOverlapped = NULL
    call WriteFile
    add rsp, 48

    xor rax, rax
    add rsp, 152
    pop r14
    pop r13
    pop r12
    mov rsp, rbp
    pop rbp
    ret

p_to_str:
    push rbp
    mov rbp, rsp
    sub rsp, 64  ; buffer + shadow space
    lea r8, [rbp-1]  ; buffer end pointer
    mov byte ptr [r8], 0  ; null terminator
    mov rax, rcx  ; value
    mov r9, 10  ; divisor
    test rax, rax
    jns ts_pos
    neg rax
ts_pos:
ts_loop:
    dec r8
    xor rdx, rdx
    div r9
    add dl, '0'
    mov [r8], dl
    test rax, rax
    jnz ts_loop
    test rcx, rcx
    jns ts_no_neg
    dec r8
    mov byte ptr [r8], '-'
ts_no_neg:
    ; Handle zero case
    cmp byte ptr [r8], 0
    jne ts_done
    dec r8
    mov byte ptr [r8], '0'
ts_done:
    lea rax, [r8]  ; return pointer to start of string
    mov rsp, rbp
    pop rbp
    ret

p_str_concat:
    push rbp
    mov rbp, rsp
    sub rsp, 32  ; shadow space
    ; Copy left string to temp buffer first (avoid aliasing)
    mov r8, OFFSET str_concat_tmp  ; temp buffer
    mov r9, 0  ; index
sc_copy:
    mov al, byte ptr [rcx + r9]
    mov byte ptr [r8 + r9], al
    test al, al
    jz sc_copy_done
    inc r9
    jmp sc_copy
sc_copy_done:
    ; Now copy temp (left) to output buffer
    mov r8, OFFSET str_concat_buf  ; destination
    mov r11, OFFSET str_concat_tmp  ; source (left)
    mov rcx, 0  ; index for left
sc_left:
    mov al, byte ptr [r11 + rcx]
    test al, al
    jz sc_right
    mov byte ptr [r8 + rcx], al
    inc rcx
    jmp sc_left
sc_right:
    ; r9 = length of left (rcx), copy right string
    mov r9, rcx  ; left length
    mov r10, 0  ; index for right
sc_right_loop:
    mov al, byte ptr [rdx + r10]
    test al, al
    jz sc_end
    mov byte ptr [r8 + r9], al
    inc r9
    inc r10
    jmp sc_right_loop
sc_end:
    mov byte ptr [r8 + r9], 0  ; null terminate
    mov rax, OFFSET str_concat_buf  ; return pointer
    mov rsp, rbp
    pop rbp
    ret

p_panic:
    push rbp
    mov rbp, rsp
    sub rsp, 64

    ; Get stdout handle
    mov ecx, -11  ; STD_OUTPUT_HANDLE
    sub rsp, 32
    call GetStdHandle
    add rsp, 32
    mov r12, rax

    ; Calculate string length
    mov rdx, rcx  ; message
    xor r8, r8  ; length counter
panic_len:
    cmp byte ptr [rdx + r8], 0
    je panic_done
    inc r8
    jmp panic_len
panic_done:

    ; WriteFile
    mov rcx, r12  ; handle
    ; rdx already = buffer
    ; r8 already = length
    lea r9, [rbp-32]
    mov dword ptr [r9], 0
    sub rsp, 48  ; shadow space + 5th arg + alignment
    mov qword ptr [rsp+32], 0  ; lpOverlapped = NULL
    call WriteFile
    add rsp, 48

    ; ExitProcess(1)
    mov ecx, 1
    sub rsp, 32
    call ExitProcess
    add rsp, 32
    ud2  ; should not reach here

; ─── Heap allocation stubs ───
p_malloc:
    push rbp
    mov rbp, rsp
    sub rsp, 48  ; shadow space + alignment

    ; Save size in r12
    mov r12, rcx

    ; GetProcessHeap()
    sub rsp, 32
    call GetProcessHeap
    add rsp, 32
    mov r13, rax  ; heap handle

    ; HeapAlloc(heap, 0, size)
    mov rcx, r13  ; heap
    xor rdx, rdx  ; flags = 0
    mov r8, r12   ; size
    sub rsp, 32
    call HeapAlloc
    add rsp, 32
    ; rax = pointer
    mov rsp, rbp
    pop rbp
    ret

p_free:
    push rbp
    mov rbp, rsp
    sub rsp, 48  ; shadow space + alignment

    mov r12, rcx  ; save ptr

    ; GetProcessHeap()
    sub rsp, 32
    call GetProcessHeap
    add rsp, 32
    mov r13, rax  ; heap handle

    ; HeapFree(heap, 0, ptr)
    mov rcx, r13  ; heap
    xor rdx, rdx  ; flags = 0
    mov r8, r12   ; ptr
    sub rsp, 32
    call HeapFree
    add rsp, 32
    mov rsp, rbp
    pop rbp
    ret

; ─── Windows entry point ───
_main:
    sub rsp, 40  ; 32 shadow + 8 align (OS entry: rsp is 8mod16)
    call main
    mov ecx, eax  ; exit code from main
    call ExitProcess
    ud2  ; should not reach here
END
