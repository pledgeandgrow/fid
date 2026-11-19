#!/usr/bin/env python3
"""enc-oracle-tg.py — scripts/enc-oracle.py with the instruction forms that
Track TG (docs/roadmap/llvm-roadmap.md) added to compiler/nat_enc.fd:

  TG7  x86-64 BMI2 SHLX / SARX / SHRX (x64_shiftx)
  TG9  RISC-V Zba sh1add / sh2add / sh3add, zext.w (rv_shadd, rv_zext_w)
       and Zbb sext.b / sext.h / zext.h (rv_sext_b, rv_sext_h, rv_zext_h)

It runs enc-oracle.py's own comparison (every form of that script too)
with these appended, so one run checks both. The branch short forms of
TG6 (x64_jmp_short, x64_jcc_short, rv_b_label, rv_call_near) take their
offset from a label fixup and are left to the suite.

Usage: python3 scripts/enc-oracle-tg.py [--fid release/fid.exe] [--zig zig]
"""
import importlib.util
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location('enc_oracle', os.path.join(HERE, 'enc-oracle.py'))
oracle = importlib.util.module_from_spec(spec)
spec.loader.exec_module(oracle)

R = ['rax', 'rcx', 'rdx', 'rbx', 'rsp', 'rbp', 'rsi', 'rdi', 'r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15']

base_rv = oracle.rv_forms
base_x64 = oracle.x64_forms


def rv_forms():
    f = base_rv()
    first = True
    for d, a, b in [(5, 6, 7), (10, 11, 12), (28, 29, 31), (1, 8, 9)]:
        for n in [1, 2, 3]:
            asm = 'sh%dadd x%d, x%d, x%d' % (n, d, b, a)
            if first:
                # the assembler accepts Zba/Zbb from here on (no bytes)
                asm = '.option arch, +zba, +zbb\n' + asm
                first = False
            f.append(('rv_shadd(&mut e, %d, %d, %d, %d)' % (d, a, b, n), asm))
        f.append(('rv_zext_w(&mut e, %d, %d)' % (d, a), 'zext.w x%d, x%d' % (d, a)))
        f.append(('rv_sext_b(&mut e, %d, %d)' % (d, a), 'sext.b x%d, x%d' % (d, a)))
        f.append(('rv_sext_h(&mut e, %d, %d)' % (d, a), 'sext.h x%d, x%d' % (d, a)))
        f.append(('rv_zext_h(&mut e, %d, %d)' % (d, a), 'zext.h x%d, x%d' % (d, a)))
    return f


def x64_forms():
    f = base_x64()
    for d, a, b in [(0, 3, 1), (3, 12, 7), (15, 7, 8), (10, 11, 2), (6, 13, 14), (8, 5, 9)]:
        for pp, name in [(1, 'shlx'), (2, 'sarx'), (3, 'shrx')]:
            f.append(('x64_shiftx(&mut e, %d, %d, %d, %d)' % (pp, d, a, b), '%s %s, %s, %s' % (name, R[d], R[a], R[b])))
    return f


oracle.rv_forms = rv_forms
oracle.x64_forms = x64_forms

if __name__ == '__main__':
    sys.exit(min(oracle.main(), 125))
