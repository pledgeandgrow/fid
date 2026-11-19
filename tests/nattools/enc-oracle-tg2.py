#!/usr/bin/env python3
"""enc-oracle-tg2.py — tests/nattools/enc-oracle-mi24.py (and so the
scripts it extends) with the forms llvm-roadmap TG7, TG8 and TG9 added to
compiler/nat_enc.fd in batch 6:

  x86-64   x64_test_bit: TEST r8, imm8 (bits 0..7), TEST r32, imm32
           (8..31), BT r64, imm8 (32..63); x64_alu_mem: add / or / and /
           sub / xor / cmp / imul with a [base + disp] source (TG7)
  AArch64  a64_tbz_words: TBZ / TBNZ, low and high bits, both directions
           (TG8)
  RISC-V   rv_addw / rv_subw / rv_mulw (TG9)

Each form is encoded by nat_enc.fd (through `fid run`) and assembled by
clang (`zig cc`); every byte must agree, as in the scripts it extends.

Usage: python3 tests/nattools/enc-oracle-tg2.py [--only] [--fid release/fid.exe] [--zig zig]
"""
import importlib.util
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ONLY = '--only' in sys.argv  # (enc-oracle-mi22.py removes it from argv)
spec =importlib.util.spec_from_file_location('enc_oracle_mi24', os.path.join(HERE, 'enc-oracle-mi24.py'))
mi24 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(mi24)
oracle = mi24.oracle

R64 = ['rax', 'rcx', 'rdx', 'rbx', 'rsp', 'rbp', 'rsi', 'rdi', 'r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15']
R32 = ['eax', 'ecx', 'edx', 'ebx', 'esp', 'ebp', 'esi', 'edi', 'r8d', 'r9d', 'r10d', 'r11d', 'r12d', 'r13d', 'r14d', 'r15d']
R8 = ['al', 'cl', 'dl', 'bl', 'spl', 'bpl', 'sil', 'dil', 'r8b', 'r9b', 'r10b', 'r11b', 'r12b', 'r13b', 'r14b', 'r15b']

if ONLY:
    # the batch-6 forms with one form of each base list (a full run is
    # longer than `fid run` of one function holds on some builds)
    _a64, _rv, _x64 = oracle.a64_forms, oracle.rv_forms, oracle.x64_forms
    base_a64 = lambda: _a64()[:1]
    base_rv = lambda: _rv()[:1]
    base_x64 = lambda: _x64()[:1]
else:
    base_a64 = oracle.a64_forms
    base_rv = oracle.rv_forms
    base_x64 = oracle.x64_forms


def x64_forms():
    f = base_x64()
    for r in [0, 1, 3, 4, 6, 7, 9, 12, 15]:
        for bit in [0, 3, 7, 8, 20, 31, 32, 45, 63]:
            if bit < 8:
                asm = 'test %s, %d' % (R8[r], 1 << bit)
            elif bit < 32:
                asm = 'test %s, %d' % (R32[r], 1 << bit)
            else:
                asm = 'bt %s, %d' % (R64[r], bit)
            f.append(('x64_test_bit(&mut e, %d, %d)' % (r, bit), asm))
    ops = [(0x03, 'add'), (0x0B, 'or'), (0x23, 'and'), (0x2B, 'sub'), (0x33, 'xor'), (0x3B, 'cmp'), (0x0FAF, 'imul')]
    for d, b, disp in [(0, 4, 16), (9, 4, 8), (3, 4, 0), (15, 4, 4096), (7, 5, -8), (12, 13, 200)]:
        if disp:
            m = '[%s%+d]' % (R64[b], disp)
        elif b in (5, 13):
            m = '[%s+0]' % R64[b]
        else:
            m = '[%s]' % R64[b]
        for code, name in ops:
            f.append(('x64_alu_mem(&mut e, %d, %d, %d, %d)' % (code, d, b, disp), '%s %s, qword ptr %s' % (name, R64[d], m)))
    return f


def a64_forms():
    f = base_a64()
    for t in [0, 3, 17, 30]:
        for bit in [0, 5, 31, 32, 37, 63]:
            for nz in [False, True]:
                for words in [2, 7, -3]:
                    name = 'tbnz' if nz else 'tbz'
                    rn = ('x%d' % t) if bit >= 32 else ('w%d' % t)
                    off = '.+%d' % (words * 4) if words >= 0 else '.-%d' % (-words * 4)
                    w = str(words) if words >= 0 else '0 - %d' % (-words)
                    f.append(('a64_tbz_words(&mut e, %d, %d, %s, %s)' % (t, bit, 'true' if nz else 'false', w),
                              '%s %s, #%d, %s' % (name, rn, bit, off)))
    return f


def rv_forms():
    f = base_rv()
    for d, a, b in [(10, 11, 12), (5, 6, 7), (28, 29, 31), (1, 8, 9), (30, 0, 6)]:
        f.append(('rv_addw(&mut e, %d, %d, %d)' % (d, a, b), 'addw x%d, x%d, x%d' % (d, a, b)))
        f.append(('rv_subw(&mut e, %d, %d, %d)' % (d, a, b), 'subw x%d, x%d, x%d' % (d, a, b)))
        f.append(('rv_mulw(&mut e, %d, %d, %d)' % (d, a, b), 'mulw x%d, x%d, x%d' % (d, a, b)))
    return f


oracle.a64_forms = a64_forms
oracle.rv_forms = rv_forms
oracle.x64_forms = x64_forms

if __name__ == '__main__':
    sys.exit(min(oracle.main(), 125))
