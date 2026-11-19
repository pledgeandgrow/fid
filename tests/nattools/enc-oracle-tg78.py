#!/usr/bin/env python3
"""enc-oracle-tg78.py — tests/nattools/enc-oracle-op18.py (and so the
scripts it extends) with the forms llvm-roadmap TG7 and TG8 added to
compiler/nat_enc.fd and use from compiler/nat_ir.fd:

  x86-64   BMI1 blsmsk (x64_blsmsk), blsi (x64_blsi); the frame-slot forms
           of the TG7 rules cmpslot and rmwslot: cmp r, qword [rsp + d]
           (x64_alu_mem 0x3B), cmp qword [rsp + d], 0 (x64_cmp_mem_imm),
           op qword [rsp + d], r (x64_alu_to_mem), op qword [rsp + d], imm
           (x64_alu_mem_imm), short and long displacements
  AArch64  loads and stores with writeback (TG8): a64_ldr_wb / a64_str_wb,
           post-indexed ([n], #c) and pre-indexed ([n, #c]!), every size,
           the sign-extending loads, steps -256..255, sp as the base

Each form is encoded by nat_enc.fd (through `fid run`) and assembled by
clang (`zig cc`); every byte must agree, as in the scripts it extends.

Usage: python3 tests/nattools/enc-oracle-tg78.py [--only] [--fid release/fid.exe] [--zig zig]
--only: the TG7 / TG8 forms with one form of each base list (as
enc-oracle-op18.py), a run of minutes through an interpreting `fid run`.
"""
import importlib.util
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ONLY = '--only' in sys.argv  # (enc-oracle-mi22.py removes it from argv)
spec = importlib.util.spec_from_file_location('enc_oracle_op18', os.path.join(HERE, 'enc-oracle-op18.py'))
op18 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(op18)
oracle = op18.oracle

R64 = ['rax', 'rcx', 'rdx', 'rbx', 'rsp', 'rbp', 'rsi', 'rdi', 'r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15']

if ONLY:
    _a64, _rv, _x64 = oracle.a64_forms, oracle.rv_forms, oracle.x64_forms
    base_a64 = lambda: _a64()[:1]
    base_rv = lambda: _rv()[:1]
    base_x64 = lambda: _x64()[:1]
    oracle.rv_forms = base_rv
else:
    base_a64 = oracle.a64_forms
    base_x64 = oracle.x64_forms


def slot(disp):
    return '[rsp + %d]' % disp if disp else '[rsp]'


def x64_forms():
    f = base_x64()
    for d, a in [(0, 1), (3, 12), (15, 7), (10, 11), (6, 13), (8, 5), (2, 9), (14, 0)]:
        f.append(('x64_blsmsk(&mut e, %d, %d)' % (d, a), 'blsmsk %s, %s' % (R64[d], R64[a])))
        f.append(('x64_blsi(&mut e, %d, %d)' % (d, a), 'blsi %s, %s' % (R64[d], R64[a])))
    ops = [(0x01, 'add'), (0x29, 'sub'), (0x21, 'and'), (0x09, 'or'), (0x31, 'xor')]
    exts = [(0, 'add'), (5, 'sub'), (4, 'and'), (1, 'or'), (6, 'xor')]
    for disp in (0, 8, 120, 128, 4096, 70000):
        for r in (0, 3, 9, 15):
            f.append(('x64_alu_mem(&mut e, %d, %d, 4, %d)' % (0x3B, r, disp),
                      'cmp %s, qword ptr %s' % (R64[r], slot(disp))))
        for r, (op, name) in zip((0, 3, 9, 15, 6), ops):
            f.append(('x64_alu_to_mem(&mut e, %d, %d, 4, %d)' % (op, r, disp),
                      '%s qword ptr %s, %s' % (name, slot(disp), R64[r])))
        for (ext, name), imm in zip(exts, (1, 0 - 1, 300, 0 - 300, 2147483647)):
            f.append(('x64_alu_mem_imm(&mut e, %d, 4, %d, %d)' % (ext, disp, imm),
                      '%s qword ptr %s, %d' % (name, slot(disp), imm)))
        f.append(('x64_cmp_mem_imm(&mut e, 8, 4, %d, 0)' % disp, 'cmp qword ptr %s, 0' % slot(disp)))
    return f


def a64_reg(r, wide):
    if r == 31:
        return 'sp'
    return ('x%d' if wide else 'w%d') % r


def a64_forms():
    f = base_a64()
    loads = [(8, False, 'ldr', True), (4, False, 'ldr', False), (4, True, 'ldrsw', True),
             (2, False, 'ldrh', False), (2, True, 'ldrsh', True),
             (1, False, 'ldrb', False), (1, True, 'ldrsb', True)]
    stores = [(8, 'str', True), (4, 'str', False), (2, 'strh', False), (1, 'strb', False)]
    cases = [(0, 1, 8), (9, 10, 0 - 8), (19, 28, 255), (2, 31, 0 - 256), (30, 5, 1), (7, 3, 16)]
    for t, n, c in cases:
        for pre in (False, True):
            for size, signed, name, wide in loads:
                if pre:
                    asm = '%s %s, [%s, #%d]!' % (name, a64_reg(t, wide), a64_reg(n, True), c)
                else:
                    asm = '%s %s, [%s], #%d' % (name, a64_reg(t, wide), a64_reg(n, True), c)
                f.append(('a64_ldr_wb(&mut e, %d, %d, %d, %d, %s, %s)' % (t, n, c, size, 'true' if signed else 'false', 'true' if pre else 'false'), asm))
            for size, name, wide in stores:
                if pre:
                    asm = '%s %s, [%s, #%d]!' % (name, a64_reg(t, wide), a64_reg(n, True), c)
                else:
                    asm = '%s %s, [%s], #%d' % (name, a64_reg(t, wide), a64_reg(n, True), c)
                f.append(('a64_str_wb(&mut e, %d, %d, %d, %d, %s)' % (t, n, c, size, 'true' if pre else 'false'), asm))
    return f


oracle.x64_forms = x64_forms
oracle.a64_forms = a64_forms

if __name__ == '__main__':
    sys.exit(min(oracle.main(), 125))
