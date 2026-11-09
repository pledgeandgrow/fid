#!/usr/bin/env python3
"""enc-oracle.py — our AArch64 and RISC-V encoders against clang's assembler
(own-backend roadmap TG4, RL8).

Every instruction form compiler/nat_enc.fd emits is encoded with several
operand choices by a generated Fides program (run with `fid run`), and the
same instructions are assembled by clang's integrated assembler through
`zig cc -c`. The words must be equal.

Usage: python3 scripts/enc-oracle.py [--fid release/fid.exe] [--zig zig]
Run from the repository root. Writes build/enc-oracle/; prints each mismatch
and a summary; the exit status is the number of mismatches (0: all agree).
"""
import argparse
import os
import struct
import subprocess
import sys

CC = {'eq': 0, 'ne': 1, 'hs': 2, 'lo': 3, 'mi': 4, 'pl': 5, 'hi': 8, 'ls': 9, 'ge': 10, 'lt': 11, 'gt': 12, 'le': 13}


def a64_forms():
    f = []
    regs = [(0, 1, 2), (9, 10, 11), (19, 28, 30), (16, 17, 5)]
    for d, n, m in regs:
        for op, asm in [('add', 'add'), ('adds', 'adds'), ('sub', 'sub'), ('subs', 'subs'), ('and', 'and'),
                        ('orr', 'orr'), ('eor', 'eor'), ('mul', 'mul'), ('smulh', 'smulh'), ('umulh', 'umulh'),
                        ('sdiv', 'sdiv'), ('udiv', 'udiv'), ('lslv', 'lsl'), ('lsrv', 'lsr'), ('asrv', 'asr')]:
            f.append(('a64_%s(&mut e, %d, %d, %d)' % (op, d, n, m), '%s x%d, x%d, x%d' % (asm, d, n, m)))
        f.append(('a64_msub(&mut e, %d, %d, %d, 3)' % (d, n, m), 'msub x%d, x%d, x%d, x3' % (d, n, m)))
        f.append(('a64_mov(&mut e, %d, %d)' % (d, n), 'mov x%d, x%d' % (d, n)))
        f.append(('a64_mvn(&mut e, %d, %d)' % (d, n), 'mvn x%d, x%d' % (d, n)))
        f.append(('a64_neg(&mut e, %d, %d)' % (d, n), 'neg x%d, x%d' % (d, n)))
        f.append(('a64_cmp(&mut e, %d, %d)' % (n, m), 'cmp x%d, x%d' % (n, m)))
        for ext, asm in [('sxtb', 'sxtb x%d, w%d'), ('sxth', 'sxth x%d, w%d'), ('sxtw', 'sxtw x%d, w%d'),
                         ('uxtb', 'ubfx x%d, x%d, #0, #8'), ('uxth', 'ubfx x%d, x%d, #0, #16'),
                         ('uxtw', 'ubfx x%d, x%d, #0, #32')]:
            f.append(('a64_%s(&mut e, %d, %d)' % (ext, d, n), asm % (d, n)))
        f.append(('a64_asr63(&mut e, %d, %d)' % (d, n), 'asr x%d, x%d, #63' % (d, n)))
        for c, v in CC.items():
            f.append(('a64_cset(&mut e, %d, %d)' % (d, v), 'cset x%d, %s' % (d, c)))
        f.append(('a64_blr(&mut e, %d)' % n, 'blr x%d' % n))
        f.append(('a64_ldaxr(&mut e, %d, %d)' % (d, n), 'ldaxr x%d, [x%d]' % (d, n)))
        f.append(('a64_stlxr(&mut e, 11, %d, %d)' % (d, n), 'stlxr w11, x%d, [x%d]' % (d, n)))
        f.append(('a64_ldar(&mut e, %d, %d)' % (d, n), 'ldar x%d, [x%d]' % (d, n)))
        f.append(('a64_stlr(&mut e, %d, %d)' % (d, n), 'stlr x%d, [x%d]' % (d, n)))
    for d, n, imm in [(0, 1, 0), (9, 31, 16), (31, 31, 4080), (31, 1, 0), (5, 6, 4095)]:
        rd = 'sp' if d == 31 else 'x%d' % d
        rn = 'sp' if n == 31 else 'x%d' % n
        f.append(('a64_add_imm(&mut e, %d, %d, %d)' % (d, n, imm), 'add %s, %s, #%d' % (rd, rn, imm)))
        f.append(('a64_sub_imm(&mut e, %d, %d, %d)' % (d, n, imm), 'sub %s, %s, #%d' % (rd, rn, imm)))
    f.append(('a64_add_ext(&mut e, 9, 31, 9)', 'add x9, sp, x9'))
    f.append(('a64_sub_ext(&mut e, 31, 31, 16)', 'sub sp, sp, x16'))
    for t, n, imm in [(0, 1, 0), (9, 31, 8), (19, 16, 4088), (3, 29, 64)]:
        rn = 'sp' if n == 31 else 'x%d' % n
        f.append(('a64_ldr_sized(&mut e, %d, %d, %d, 8, false)' % (t, n, imm), 'ldr x%d, [%s, #%d]' % (t, rn, imm)))
        f.append(('a64_str_sized(&mut e, %d, %d, %d, 8)' % (t, n, imm), 'str x%d, [%s, #%d]' % (t, rn, imm)))
        i4 = (imm // 2) & ~3
        f.append(('a64_ldr_sized(&mut e, %d, %d, %d, 4, false)' % (t, n, i4), 'ldr w%d, [%s, #%d]' % (t, rn, i4)))
        f.append(('a64_ldr_sized(&mut e, %d, %d, %d, 4, true)' % (t, n, i4), 'ldrsw x%d, [%s, #%d]' % (t, rn, i4)))
        f.append(('a64_str_sized(&mut e, %d, %d, %d, 4)' % (t, n, i4), 'str w%d, [%s, #%d]' % (t, rn, i4)))
        i2 = (imm // 4) & ~1
        f.append(('a64_ldr_sized(&mut e, %d, %d, %d, 2, false)' % (t, n, i2), 'ldrh w%d, [%s, #%d]' % (t, rn, i2)))
        f.append(('a64_ldr_sized(&mut e, %d, %d, %d, 2, true)' % (t, n, i2), 'ldrsh x%d, [%s, #%d]' % (t, rn, i2)))
        f.append(('a64_str_sized(&mut e, %d, %d, %d, 2)' % (t, n, i2), 'strh w%d, [%s, #%d]' % (t, rn, i2)))
        i1 = imm // 8
        f.append(('a64_ldr_sized(&mut e, %d, %d, %d, 1, false)' % (t, n, i1), 'ldrb w%d, [%s, #%d]' % (t, rn, i1)))
        f.append(('a64_ldr_sized(&mut e, %d, %d, %d, 1, true)' % (t, n, i1), 'ldrsb x%d, [%s, #%d]' % (t, rn, i1)))
        f.append(('a64_str_sized(&mut e, %d, %d, %d, 1)' % (t, n, i1), 'strb w%d, [%s, #%d]' % (t, rn, i1)))
    for d, n, m in [(0, 1, 2), (7, 3, 31)]:
        for op in ['fadd', 'fsub', 'fmul', 'fdiv']:
            f.append(('a64_%s(&mut e, %d, %d, %d)' % (op, d, n, m), '%s d%d, d%d, d%d' % (op, d, n, m)))
        f.append(('a64_fcmp(&mut e, %d, %d)' % (n, m), 'fcmp d%d, d%d' % (n, m)))
        for op in ['fsqrt', 'fabs', 'frintm', 'frintp', 'frintz', 'frinta']:
            f.append(('a64_%s(&mut e, %d, %d)' % (op, d, n), '%s d%d, d%d' % (op, d, n)))
        f.append(('a64_fcvt_ds(&mut e, %d, %d)' % (d, n), 'fcvt d%d, s%d' % (d, n)))
        f.append(('a64_fcvt_sd(&mut e, %d, %d)' % (d, n), 'fcvt s%d, d%d' % (d, n)))
    for d, n in [(0, 9), (2, 30)]:
        f.append(('a64_fmov_dx(&mut e, %d, %d)' % (d, n), 'fmov d%d, x%d' % (d, n)))
        f.append(('a64_fmov_xd(&mut e, %d, %d)' % (n, d), 'fmov x%d, d%d' % (n, d)))
        f.append(('a64_fmov_sw(&mut e, %d, %d)' % (d, n), 'fmov s%d, w%d' % (d, n)))
        f.append(('a64_fmov_ws(&mut e, %d, %d)' % (n, d), 'fmov w%d, s%d' % (n, d)))
        f.append(('a64_scvtf(&mut e, %d, %d)' % (d, n), 'scvtf d%d, x%d' % (d, n)))
        f.append(('a64_fcvtzs(&mut e, %d, %d)' % (n, d), 'fcvtzs x%d, d%d' % (n, d)))
    f += [('a64_ret(&mut e)', 'ret'), ('a64_svc0(&mut e)', 'svc #0'), ('a64_brk(&mut e)', 'brk #0'),
          ('a64_clrex(&mut e)', 'clrex'), ('a64_dmb(&mut e)', 'dmb ish'),
          ('a64_cbnz_w_words(&mut e, 11, 0 - 3)', 'cbnz w11, .-12'),
          ('a64_cbnz_words(&mut e, 4, 7)', 'cbnz x4, .+28'),
          ('a64_bcond_words(&mut e, 1, 5)', 'b.ne .+20'),
          ('a64_bcond_words(&mut e, 11, 0 - 2)', 'b.lt .-8')]
    return f


def rv_forms():
    f = []
    for d, a, b in [(5, 6, 7), (10, 11, 12), (28, 29, 31), (1, 8, 9)]:
        for op in ['add', 'sub', 'sll', 'slt', 'sltu', 'xor', 'srl', 'sra', 'or', 'and', 'mul', 'mulh', 'mulhu',
                   'div', 'divu', 'rem', 'remu']:
            f.append(('rv_%s(&mut e, %d, %d, %d)' % (op, d, a, b), '%s x%d, x%d, x%d' % (op, d, a, b)))
        for imm in [0, 1, -1, 2047, -2048]:
            f.append(('rv_addi(&mut e, %d, %d, %d)' % (d, a, imm), 'addi x%d, x%d, %d' % (d, a, imm)))
            f.append(('rv_xori(&mut e, %d, %d, %d)' % (d, a, imm), 'xori x%d, x%d, %d' % (d, a, imm)))
            f.append(('rv_andi(&mut e, %d, %d, %d)' % (d, a, imm), 'andi x%d, x%d, %d' % (d, a, imm)))
            f.append(('rv_sltiu(&mut e, %d, %d, %d)' % (d, a, imm), 'sltiu x%d, x%d, %d' % (d, a, imm)))
            f.append(('rv_addiw(&mut e, %d, %d, %d)' % (d, a, imm), 'addiw x%d, x%d, %d' % (d, a, imm)))
        for sh in [0, 1, 32, 63]:
            for op in ['slli', 'srli', 'srai']:
                f.append(('rv_%s(&mut e, %d, %d, %d)' % (op, d, a, sh), '%s x%d, x%d, %d' % (op, d, a, sh)))
        f.append(('rv_mv(&mut e, %d, %d)' % (d, a), 'addi x%d, x%d, 0' % (d, a)))
        f.append(('rv_lui(&mut e, %d, 74565)' % d, 'lui x%d, 74565' % d))
        f.append(('rv_jalr(&mut e, 1, %d, 0)' % a, 'jalr x1, 0(x%d)' % a))
        for imm in [0, 8, -8, 2040]:
            for size, l, s in [(8, 'ld', 'sd'), (4, 'lw', 'sw'), (2, 'lh', 'sh'), (1, 'lb', 'sb')]:
                f.append(('rv_load(&mut e, %d, %d, %d, %d, true)' % (d, a, imm, size), '%s x%d, %d(x%d)' % (l, d, imm, a)))
                if size < 8:
                    f.append(('rv_load(&mut e, %d, %d, %d, %d, false)' % (d, a, imm, size),
                              '%su x%d, %d(x%d)' % (l, d, imm, a)))
                f.append(('rv_store(&mut e, %d, %d, %d, %d)' % (b, a, imm, size), '%s x%d, %d(x%d)' % (s, b, imm, a)))
        f.append(('rv_amoadd_d(&mut e, %d, %d, %d)' % (d, b, a), 'amoadd.d.aqrl x%d, x%d, (x%d)' % (d, b, a)))
        f.append(('rv_lr_d(&mut e, %d, %d)' % (d, a), 'lr.d.aqrl x%d, (x%d)' % (d, a)))
        f.append(('rv_sc_d(&mut e, %d, %d, %d)' % (d, b, a), 'sc.d.aqrl x%d, x%d, (x%d)' % (d, b, a)))
        f.append(('rv_b_off(&mut e, 0, %d, %d, 12)' % (a, b), 'beq x%d, x%d, .+12' % (a, b)))
        f.append(('rv_b_off(&mut e, 1, %d, %d, 0 - 12)' % (a, b), 'bne x%d, x%d, .-12' % (a, b)))
    for d, a, b in [(0, 1, 2), (3, 31, 7)]:
        for op in ['fadd_d', 'fsub_d', 'fmul_d', 'fdiv_d', 'fsgnj_d', 'fsgnjx_d']:
            f.append(('rv_%s(&mut e, %d, %d, %d)' % (op, d, a, b), '%s f%d, f%d, f%d' % (op.replace('_', '.'), d, a, b)))
        for op in ['feq_d', 'flt_d', 'fle_d']:
            f.append(('rv_%s(&mut e, 10, %d, %d)' % (op, a, b), '%s x10, f%d, f%d' % (op.replace('_', '.'), a, b)))
        f.append(('rv_fsqrt_d(&mut e, %d, %d)' % (d, a), 'fsqrt.d f%d, f%d' % (d, a)))
        f.append(('rv_fmv_dx(&mut e, %d, 10)' % d, 'fmv.d.x f%d, x10' % d))
        f.append(('rv_fmv_xd(&mut e, 10, %d)' % d, 'fmv.x.d x10, f%d' % d))
        f.append(('rv_fmv_wx(&mut e, %d, 10)' % d, 'fmv.w.x f%d, x10' % d))
        f.append(('rv_fmv_xw(&mut e, 10, %d)' % d, 'fmv.x.w x10, f%d' % d))
        f.append(('rv_fcvt_dl(&mut e, %d, 11)' % d, 'fcvt.d.l f%d, x11' % d))
        f.append(('rv_fcvt_ld(&mut e, 11, %d)' % d, 'fcvt.l.d x11, f%d, rtz' % d))
        f.append(('rv_fcvt_ds(&mut e, %d, %d)' % (d, a), 'fcvt.d.s f%d, f%d' % (d, a)))
        f.append(('rv_fcvt_sd(&mut e, %d, %d)' % (d, a), 'fcvt.s.d f%d, f%d' % (d, a)))
        for rm, name in [(1, 'rtz'), (2, 'rdn'), (3, 'rup'), (4, 'rmm')]:
            f.append(('rv_fcvt_ld_rm(&mut e, 12, %d, %d)' % (d, rm), 'fcvt.l.d x12, f%d, %s' % (d, name)))
    f += [('rv_ret(&mut e)', 'ret'), ('rv_ecall(&mut e)', 'ecall'), ('rv_ebreak(&mut e)', 'ebreak'),
          ('rv_fence(&mut e)', 'fence rw, rw')]
    return f


def x64_forms():
    """x86-64: Intel syntax, compared as whole instructions (each padded to
    16 bytes with nops on both sides)."""
    f = []
    R = ['rax', 'rcx', 'rdx', 'rbx', 'rsp', 'rbp', 'rsi', 'rdi', 'r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15']
    R32 = ['eax', 'ecx', 'edx', 'ebx', 'esp', 'ebp', 'esi', 'edi', 'r8d', 'r9d', 'r10d', 'r11d', 'r12d', 'r13d', 'r14d', 'r15d']
    R16 = ['ax', 'cx', 'dx', 'bx', 'sp', 'bp', 'si', 'di', 'r8w', 'r9w', 'r10w', 'r11w', 'r12w', 'r13w', 'r14w', 'r15w']
    R8 = ['al', 'cl', 'dl', 'bl', 'spl', 'bpl', 'sil', 'dil', 'r8b', 'r9b', 'r10b', 'r11b', 'r12b', 'r13b', 'r14b', 'r15b']
    for d, s_ in [(0, 1), (3, 12), (15, 7), (10, 11), (6, 13), (8, 5)]:
        f.append(('x64_mov(&mut e, %d, %d)' % (d, s_), 'mov %s, %s' % (R[d], R[s_])))
        f.append(('x64_mov32(&mut e, %d, %d)' % (d, s_), 'mov %s, %s' % (R32[d], R32[s_])))
        for code, name in [(0x01, 'add'), (0x09, 'or'), (0x21, 'and'), (0x29, 'sub'), (0x31, 'xor'), (0x39, 'cmp')]:
            f.append(('x64_alu(&mut e, %d, %d, %d)' % (code, d, s_), '%s %s, %s' % (name, R[d], R[s_])))
        f.append(('x64_imul(&mut e, %d, %d)' % (d, s_), 'imul %s, %s' % (R[d], R[s_])))
        f.append(('x64_test(&mut e, %d, %d)' % (d, s_), 'test %s, %s' % (R[d], R[s_])))
        f.append(('x64_movx(&mut e, 182, %d, %d)' % (d, s_), 'movzx %s, %s' % (R[d], R8[s_])))
        f.append(('x64_movx(&mut e, 183, %d, %d)' % (d, s_), 'movzx %s, %s' % (R[d], R16[s_])))
        f.append(('x64_movx(&mut e, 190, %d, %d)' % (d, s_), 'movsx %s, %s' % (R[d], R8[s_])))
        f.append(('x64_movx(&mut e, 191, %d, %d)' % (d, s_), 'movsx %s, %s' % (R[d], R16[s_])))
        f.append(('x64_movx(&mut e, 99, %d, %d)' % (d, s_), 'movsxd %s, %s' % (R[d], R32[s_])))
    for r in [0, 3, 7, 9, 12, 15]:
        for ext, name in [(2, 'not'), (3, 'neg'), (4, 'mul'), (5, 'imul'), (6, 'div'), (7, 'idiv')]:
            f.append(('x64_f7(&mut e, %d, %d)' % (ext, r), '%s %s' % (name, R[r])))
        for ext, name in [(4, 'shl'), (5, 'shr'), (7, 'sar')]:
            f.append(('x64_shift_cl(&mut e, %d, %d)' % (ext, r), '%s %s, cl' % (name, R[r])))
        f.append(('x64_shift_imm(&mut e, 7, %d, 63)' % r, 'sar %s, 63' % R[r]))
        f.append(('x64_call_r(&mut e, %d)' % r, 'call %s' % R[r]))
        f.append(('x64_push(&mut e, %d)' % r, 'push %s' % R[r]))
        f.append(('x64_pop(&mut e, %d)' % r, 'pop %s' % R[r]))
        for cc, name in [(4, 'e'), (5, 'ne'), (12, 'l'), (13, 'ge'), (14, 'le'), (15, 'g'), (2, 'b'), (7, 'a')]:
            f.append(('x64_setcc(&mut e, %d, %d)' % (cc, r), 'set%s %s' % (name, R8[r])))
        for v in [0, 1, 127, 4294967295, -1, -2147483648, 9223372036854775807, 4294967296]:
            vv = str(v) if v >= 0 else '0 - %d' % (-v)
            if v == 0:
                asm = 'xor %s, %s' % (R32[r], R32[r])
            elif 0 < v <= 4294967295:
                asm = 'mov %s, %d' % (R32[r], v)
            elif v in (-1, -2147483648):
                asm = 'mov %s, %d' % (R[r], v)
            else:
                asm = 'movabs %s, %d' % (R[r], v)
            f.append(('x64_li(&mut e, %d, %s)' % (r, vv), asm))
        f.append(('x64_add_imm(&mut e, %d, 8)' % r, 'add %s, 8' % R[r]))
        f.append(('x64_sub_imm(&mut e, %d, 1000)' % r, 'sub %s, 1000' % R[r]))
    for d, b, disp in [(0, 4, 0), (3, 5, 16), (12, 13, 0), (7, 4, 4096), (15, 12, -8), (9, 10, 200)]:
        m = '[%s%+d]' % (R[b], disp) if disp else ('[%s+0]' % R[b] if b in (5, 13) else '[%s]' % R[b])
        f.append(('x64_load(&mut e, %d, %d, %d, 8, true)' % (d, b, disp), 'mov %s, qword ptr %s' % (R[d], m)))
        f.append(('x64_load(&mut e, %d, %d, %d, 4, true)' % (d, b, disp), 'movsxd %s, dword ptr %s' % (R[d], m)))
        f.append(('x64_load(&mut e, %d, %d, %d, 4, false)' % (d, b, disp), 'mov %s, dword ptr %s' % (R32[d], m)))
        f.append(('x64_load(&mut e, %d, %d, %d, 2, true)' % (d, b, disp), 'movsx %s, word ptr %s' % (R[d], m)))
        f.append(('x64_load(&mut e, %d, %d, %d, 2, false)' % (d, b, disp), 'movzx %s, word ptr %s' % (R[d], m)))
        f.append(('x64_load(&mut e, %d, %d, %d, 1, true)' % (d, b, disp), 'movsx %s, byte ptr %s' % (R[d], m)))
        f.append(('x64_load(&mut e, %d, %d, %d, 1, false)' % (d, b, disp), 'movzx %s, byte ptr %s' % (R[d], m)))
        f.append(('x64_store(&mut e, %d, %d, %d, 8)' % (d, b, disp), 'mov qword ptr %s, %s' % (m, R[d])))
        f.append(('x64_store(&mut e, %d, %d, %d, 4)' % (d, b, disp), 'mov dword ptr %s, %s' % (m, R32[d])))
        f.append(('x64_store(&mut e, %d, %d, %d, 2)' % (d, b, disp), 'mov word ptr %s, %s' % (m, R16[d])))
        f.append(('x64_store(&mut e, %d, %d, %d, 1)' % (d, b, disp), 'mov byte ptr %s, %s' % (m, R8[d])))
        f.append(('x64_lea(&mut e, %d, %d, %d)' % (d, b, disp), 'lea %s, %s' % (R[d], m)))
        mb = '[%s+0]' % R[b] if b in (5, 13) else '[%s]' % R[b]
        f.append(('x64_xchg_mem(&mut e, %d, %d)' % (b, d), 'xchg qword ptr %s, %s' % (mb, R[d])))
        f.append(('x64_lock_xadd(&mut e, %d, %d)' % (b, d), 'lock xadd qword ptr %s, %s' % (mb, R[d])))
        f.append(('x64_lock_cmpxchg(&mut e, %d, %d)' % (b, d), 'lock cmpxchg qword ptr %s, %s' % (mb, R[d])))
    for x, r in [(0, 7), (1, 6), (2, 12), (9, 3)]:
        f.append(('x64_movq_xr(&mut e, %d, %d, true)' % (x, r), 'movq xmm%d, %s' % (x, R[r])))
        f.append(('x64_movq_rx(&mut e, %d, %d, true)' % (r, x), 'movq %s, xmm%d' % (R[r], x)))
        f.append(('x64_movq_xr(&mut e, %d, %d, false)' % (x, r), 'movd xmm%d, %s' % (x, R32[r])))
        f.append(('x64_movq_rx(&mut e, %d, %d, false)' % (r, x), 'movd %s, xmm%d' % (R32[r], x)))
        f.append(('x64_cvtsi2sd(&mut e, %d, %d)' % (x, r), 'cvtsi2sd xmm%d, %s' % (x, R[r])))
        f.append(('x64_cvttsd2si(&mut e, %d, %d)' % (r, x), 'cvttsd2si %s, xmm%d' % (R[r], x)))
    for a, b in [(0, 1), (2, 3), (1, 0)]:
        for pre, op, name in [(0xF2, 0x58, 'addsd'), (0xF2, 0x5C, 'subsd'), (0xF2, 0x59, 'mulsd'), (0xF2, 0x5E, 'divsd'),
                              (0xF2, 0x51, 'sqrtsd'), (0xF3, 0x5A, 'cvtss2sd'), (0xF2, 0x5A, 'cvtsd2ss'), (0x66, 0x2E, 'ucomisd')]:
            f.append(('x64_sse(&mut e, %d, %d, %d, %d)' % (pre, op, a, b), '%s xmm%d, xmm%d' % (name, a, b)))
        for mode in [9, 10, 11]:
            f.append(('x64_roundsd(&mut e, %d, %d, %d)' % (a, b, mode), 'roundsd xmm%d, xmm%d, %d' % (a, b, mode)))
    f += [('x64_cqo(&mut e)', 'cqo'), ('x64_ret(&mut e)', 'ret'), ('x64_ud2(&mut e)', 'ud2'),
          ('x64_syscall(&mut e)', 'syscall'), ('x64_mfence(&mut e)', 'mfence'),
          ('x64_vfmadd213sd_012(&mut e)', 'vfmadd213sd xmm0, xmm1, xmm2'),
          ('x64_load_fs(&mut e, 7, 64)', 'mov rdi, qword ptr fs:[64]')]
    return f


PROGRAM_HEAD = '''// Generated by scripts/enc-oracle.py: each line is "arch|word" for one form.
use core;
use vec;
use string;
use compiler::nat_enc;

fn hex(v: i64) -> str {
    var digits: str = "0123456789abcdef";
    var out: str = "";
    var i: i64 = 7;
    while i >= 0 {
        var d: i64 = (v >> (i * 4)) & 15;
        out = out + str_slice(digits, d, d + 1);
        i = i - 1;
    }
    return out;
}

fn word(e: &NatEnc) -> str {
    var w: i64 = e.code.get(0) | (e.code.get(1) << 8) | (e.code.get(2) << 16) | (e.code.get(3) << 24);
    var n: str = int_to_str(e.code.len() / 4);
    return hex(w) + " " + n;
}

fn bytes(e: &NatEnc) -> str {
    var out: str = "";
    var i: i64 = 0;
    while i < e.code.len() {
        out = out + str_slice(hex(e.code.get(i)), 6, 8);
        i = i + 1;
    }
    return out + " " + int_to_str(e.code.len());
}

fn main() -> i64 {
    var e: NatEnc = NatEnc_new();
'''


def elf_text(path):
    data = open(path, 'rb').read()
    shoff = struct.unpack_from('<Q', data, 0x28)[0]
    shentsize, shnum, shstrndx = struct.unpack_from('<HHH', data, 0x3A)
    def sh(i):
        return struct.unpack_from('<IIQQQQIIQQ', data, shoff + i * shentsize)
    strtab = sh(shstrndx)
    for i in range(shnum):
        s = sh(i)
        name_off = strtab[4] + s[0]
        name = data[name_off:data.index(b'\0', name_off)].decode()
        if name == '.text':
            return data[s[4]:s[4] + s[5]]
    raise SystemExit('no .text in ' + path)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--fid', default='release/fid.exe' if os.name == 'nt' else 'release/fid')
    ap.add_argument('--zig', default='zig')
    args = ap.parse_args()
    out = os.path.join('build', 'enc-oracle')
    os.makedirs(out, exist_ok=True)
    forms = [('a64', c, a) for c, a in a64_forms()] + [('rv', c, a) for c, a in rv_forms()] \
        + [('x64', c, a) for c, a in x64_forms()]
    prog = [PROGRAM_HEAD]
    for arch, call, _ in forms:
        fn = 'bytes' if arch == 'x64' else 'word'
        prog.append('    e = NatEnc_new(); %s; print("%s|" + %s(&e) + "\\n");\n' % (call, arch, fn))
    prog.append('    return 0;\n}\n')
    src = os.path.join(out, 'oracle.fd')
    open(src, 'w', newline='\n').write(''.join(prog))
    r = subprocess.run([args.fid, 'run', src], capture_output=True, text=True)
    lines = [l for l in r.stdout.splitlines() if '|' in l]
    if len(lines) != len(forms):
        sys.stderr.write(r.stdout[-2000:] + r.stderr[-2000:])
        raise SystemExit('the encoder program printed %d of %d words' % (len(lines), len(forms)))
    ours = {}
    for (arch, call, asm), line in zip(forms, lines):
        hexw, nwords = line.split('|')[1].split()
        if arch == 'x64':
            ours.setdefault(arch, []).append((call, asm, bytes.fromhex(hexw), int(nwords)))
        else:
            ours.setdefault(arch, []).append((call, asm, int(hexw, 16), int(nwords)))
    bad = 0
    total = 0
    for arch, target, head in [('a64', 'aarch64-linux-musl', '.text\n'),
                               ('rv', 'riscv64-linux-musl', '.text\n.option norvc\n')]:
        s_path = os.path.join(out, arch + '.s')
        o_path = os.path.join(out, arch + '.o')
        with open(s_path, 'w', newline='\n') as fh:
            fh.write(head)
            for _, asm, _, _ in ours[arch]:
                fh.write(asm + '\n')
        cmd = [args.zig, 'cc', '-target', target, '-c', s_path, '-o', o_path]
        r2 = subprocess.run(cmd, capture_output=True, text=True)
        if r2.returncode != 0:
            sys.stderr.write(r2.stderr)
            raise SystemExit('the assembler refused ' + s_path)
        text = elf_text(o_path)
        for i, (call, asm, w, nwords) in enumerate(ours[arch]):
            total += 1
            theirs = struct.unpack_from('<I', text, 4 * i)[0] if 4 * i + 4 <= len(text) else -1
            if nwords != 1 or theirs != w:
                bad += 1
                print('MISMATCH %-4s %-40s ours %08x (%d words)  clang %08x   %s' % (arch, asm, w, nwords, theirs, call))
    # x86-64: each instruction at a 16-byte boundary, nops after it
    s_path = os.path.join(out, 'x64.s')
    o_path = os.path.join(out, 'x64.o')
    with open(s_path, 'w', newline='\n') as fh:
        fh.write('.intel_syntax noprefix\n.text\n')
        for _, asm, _, _ in ours['x64']:
            fh.write('.p2align 4, 0x90\n' + asm + '\n')
        fh.write('.p2align 4, 0x90\n')
    r3 = subprocess.run([args.zig, 'cc', '-target', 'x86_64-linux-musl', '-c', s_path, '-o', o_path],
                        capture_output=True, text=True)
    if r3.returncode != 0:
        sys.stderr.write(r3.stderr)
        raise SystemExit('the assembler refused ' + s_path)
    text = elf_text(o_path)
    for i, (call, asm, ob, n) in enumerate(ours['x64']):
        total += 1
        theirs = text[16 * i:16 * i + 16]
        mine = (ob + b'\x90' * 16)[:16]
        if theirs != mine:
            bad += 1
            print('MISMATCH x64  %-40s ours %s  clang %s   %s' % (asm, ob.hex(), theirs.rstrip(b'\x90').hex(), call))
    print('%d forms, %d mismatches' % (total, bad))
    return bad


if __name__ == '__main__':
    sys.exit(min(main(), 125))
