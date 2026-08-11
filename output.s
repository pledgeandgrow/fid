.section .text
.globl _start

_start:
    bl p_main
    mov x8, #93
    mov x0, #0
    svc #0

p_main:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    mov x0, #8080
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    bl p_p_socket_listen
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_0
    mov x0, #1
    neg x0, x0
    mov x1, x0
    mov x2, #64
    mov x0, #1
    mov x8, #64
    svc #0
    mov x0, #0
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .endif_1
.else_0:
.endif_1:
    ldr x0, [sp, #0]
    mov x1, x0
    mov x2, #64
    mov x0, #1
    mov x8, #64
    svc #0
    mov x0, #0
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_socket_close
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_println:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    mov x1, x0
    mov x2, #64
    mov x0, #1
    mov x8, #64
    svc #0
    mov x0, #0
    adr x0, .str_0
    mov x1, x0
    mov x2, #64
    mov x0, #1
    mov x8, #64
    svc #0
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_assert:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    cmp x0, #0
    cset x0, eq
    cmp x0, #0
    beq .else_2
    adr x0, .str_1
    mov x1, x0
    mov x2, #64
    mov x0, #1
    mov x8, #64
    svc #0
    mov x0, #0
    b .endif_3
.else_2:
.endif_3:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_max:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_4
    ldr x0, [sp, #0]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .endif_5
.else_4:
.endif_5:
    ldr x0, [sp, #8]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_min:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_6
    ldr x0, [sp, #0]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .endif_7
.else_6:
.endif_7:
    ldr x0, [sp, #8]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_abs:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_8
    ldr x0, [sp, #0]
    neg x0, x0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .endif_9
.else_8:
.endif_9:
    ldr x0, [sp, #0]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_clamp:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #96
    str x0, [sp, #0]
    str x1, [sp, #8]
    str x2, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_10
    ldr x0, [sp, #8]
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    ret
    b .endif_11
.else_10:
.endif_11:
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_12
    ldr x0, [sp, #16]
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    ret
    b .endif_13
.else_12:
.endif_13:
    ldr x0, [sp, #0]
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    ret
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    ret

p_create_allocator:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_allocator_create
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_allocator_alloc:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x1, [sp, #24]
    ldr x0, [sp, #16]
    bl p_p_allocator_alloc
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_allocator_free:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x1, [sp, #24]
    ldr x0, [sp, #16]
    bl p_p_allocator_free
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_allocator_bytes_used:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_allocator_stats
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_Option_is_some:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_15
    b .match_arm_1_16
    b .match_end_14
.match_arm_0_15:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_14
.match_arm_1_16:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_14
.match_end_14:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_Option_is_none:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_18
    b .match_arm_1_19
    b .match_end_17
.match_arm_0_18:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_17
.match_arm_1_19:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_17
.match_end_17:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_Option_unwrap:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_21
    b .match_arm_1_22
    b .match_end_20
.match_arm_0_21:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_20
.match_arm_1_22:
    adr x0, .str_2
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_p_panic
    b .match_end_20
.match_end_20:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_Option_unwrap_or:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    b .match_arm_0_24
    b .match_arm_1_25
    b .match_end_23
.match_arm_0_24:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_23
.match_arm_1_25:
    ldr x0, [sp, #8]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_23
.match_end_23:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_Option_map:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    b .match_arm_0_27
    b .match_arm_1_28
    b .match_end_26
.match_arm_0_27:
    mov x0, #0
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #24]
    add x0, x0, x1
    str x0, [sp, #24]
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_26
.match_arm_1_28:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_26
.match_end_26:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_Result_is_ok:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_30
    b .match_arm_1_31
    b .match_end_29
.match_arm_0_30:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_29
.match_arm_1_31:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_29
.match_end_29:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_Result_is_err:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_33
    b .match_arm_1_34
    b .match_end_32
.match_arm_0_33:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_32
.match_arm_1_34:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_32
.match_end_32:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_Result_unwrap:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_36
    b .match_arm_1_37
    b .match_end_35
.match_arm_0_36:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_35
.match_arm_1_37:
    adr x0, .str_3
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_p_panic
    b .match_end_35
.match_end_35:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_Result_unwrap_or:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    b .match_arm_0_39
    b .match_arm_1_40
    b .match_end_38
.match_arm_0_39:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_38
.match_arm_1_40:
    ldr x0, [sp, #8]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_38
.match_end_38:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_Result_ok:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_42
    b .match_arm_1_43
    b .match_end_41
.match_arm_0_42:
    mov x0, #0
    str x0, [sp, #16]
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_41
.match_arm_1_43:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_41
.match_end_41:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret

p_Result_err:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_45
    b .match_arm_1_46
    b .match_end_44
.match_arm_0_45:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_44
.match_arm_1_46:
    mov x0, #0
    str x0, [sp, #16]
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret
    b .match_end_44
.match_end_44:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    ret


.section .rodata
called unwrap on None:
    .asciz ".str_2"

:
    .asciz ".str_0"
called unwrap on Err:
    .asciz ".str_3"
Assertion failed
:
    .asciz ".str_1"
