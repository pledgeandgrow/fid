#!/usr/bin/env python3
"""enc-oracle-mi22.py — tests/nattools/enc-oracle-mi3.py (and so
scripts/enc-oracle-tg.py and scripts/enc-oracle.py) with the indexed
access forms llvm-roadmap MI22 added to compiler/nat_enc.fd:

  x86-64   loads and stores through a SIB operand (x64_load_idx,
           x64_store_idx): [base + index * 2^s + disp], every access size,
           rbp / r13 bases (a displacement even when 0), r12 as the index
  AArch64  the register-offset forms (a64_ldr_roff, a64_str_roff):
           [n, m] and [n, m, lsl #log2(size)], every size, signed loads,
           sp as the base

Each form is encoded by nat_enc.fd (through `fid run`) and assembled by
clang (`zig cc`); every byte must agree, as in the scripts it extends.

Usage: python3 tests/nattools/enc-oracle-mi22.py [--only] [--fid release/fid.exe] [--zig zig]
--only: the MI22 forms alone (one form of each base list kept), a run of
minutes through an interpreting `fid run` instead of the whole catalogue.
"""
import importlib.util
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location('enc_oracle_mi3', os.path.join(HERE, 'enc-oracle-mi3.py'))
mi3 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(mi3)
oracle = mi3.oracle

R64 = ['rax', 'rcx', 'rdx', 'rbx', 'rsp', 'rbp', 'rsi', 'rdi',
       'r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15']
R32 = ['eax', 'ecx', 'edx', 'ebx', 'esp', 'ebp', 'esi', 'edi',
       'r8d', 'r9d', 'r10d', 'r11d', 'r12d', 'r13d', 'r14d', 'r15d']
R16 = ['ax', 'cx', 'dx', 'bx', 'sp', 'bp', 'si', 'di',
       'r8w', 'r9w', 'r10w', 'r11w', 'r12w', 'r13w', 'r14w', 'r15w']
R8 = ['al', 'cl', 'dl', 'bl', 'spl', 'bpl', 'sil', 'dil',
      'r8b', 'r9b', 'r10b', 'r11b', 'r12b', 'r13b', 'r14b', 'r15b']
PTR = {1: 'byte', 2: 'word', 4: 'dword', 8: 'qword'}

ONLY = '--only' in sys.argv
if ONLY:
    sys.argv.remove('--only')
    _a64, _rv, _x64 = oracle.a64_forms, oracle.rv_forms, oracle.x64_forms
    oracle.rv_forms = lambda: _rv()[:1]
    base_a64 = lambda: _a64()[:1]
    base_x64 = lambda: _x64()[:1]
else:
    base_a64 = oracle.a64_forms
    base_x64 = oracle.x64_forms


def x64_mem(b, i, s, disp):
    m = '[%s + %s*%d' % (R64[b], R64[i], 1 << s)
    if disp > 0:
        m += ' + %d' % disp
    elif disp < 0:
        m += ' - %d' % (-disp)
    return m + ']'


def x64_forms():
    f = base_x64()
    cases = [(0, 3, 1, 0, 0), (3, 12, 7, 3, 16), (15, 5, 12, 2, 0), (10, 13, 2, 1, 0),
             (6, 4, 14, 3, -8), (8, 1, 9, 0, 1000), (7, 15, 3, 3, -4096), (12, 0, 5, 1, 127)]
    for d, b, i, s, disp in cases:
        mem = x64_mem(b, i, s, disp)
        call = '(&mut e, %d, %d, %d, %d, %d, %%d, %%s)' % (d, b, i, s, disp)
        f.append(('x64_load_idx' + call % (8, 'false'), 'mov %s, qword ptr %s' % (R64[d], mem)))
        f.append(('x64_load_idx' + call % (4, 'false'), 'mov %s, dword ptr %s' % (R32[d], mem)))
        f.append(('x64_load_idx' + call % (4, 'true'), 'movsxd %s, dword ptr %s' % (R64[d], mem)))
        f.append(('x64_load_idx' + call % (2, 'false'), 'movzx %s, word ptr %s' % (R64[d], mem)))
        f.append(('x64_load_idx' + call % (2, 'true'), 'movsx %s, word ptr %s' % (R64[d], mem)))
        f.append(('x64_load_idx' + call % (1, 'false'), 'movzx %s, byte ptr %s' % (R64[d], mem)))
        f.append(('x64_load_idx' + call % (1, 'true'), 'movsx %s, byte ptr %s' % (R64[d], mem)))
        scall = '(&mut e, %d, %d, %d, %d, %d, %%d)' % (d, b, i, s, disp)
        for size, names in [(8, R64), (4, R32), (2, R16), (1, R8)]:
            f.append(('x64_store_idx' + scall % size, 'mov %s ptr %s, %s' % (PTR[size], mem, names[d])))
    return f


def a64_forms():
    f = base_a64()
    for t, n, m in [(0, 1, 2), (9, 31, 10), (19, 28, 30), (16, 17, 5), (30, 3, 29)]:
        rn = 'sp' if n == 31 else 'x%d' % n
        for size, sh, ld, ldsx, st, reg in [(8, 3, 'ldr', None, 'str', 'x'), (4, 2, 'ldr', 'ldrsw', 'str', 'w'),
                                            (2, 1, 'ldrh', 'ldrsh', 'strh', 'w'), (1, 0, 'ldrb', 'ldrsb', 'strb', 'w')]:
            for scaled in (False, True):
                if size == 1 and scaled:
                    continue
                mem = '[%s, x%d%s]' % (rn, m, ', lsl #%d' % sh if scaled else '')
                sc = 'true' if scaled else 'false'
                f.append(('a64_ldr_roff(&mut e, %d, %d, %d, %d, false, %s)' % (t, n, m, size, sc),
                          '%s %s%d, %s' % (ld, reg, t, mem)))
                if ldsx:
                    f.append(('a64_ldr_roff(&mut e, %d, %d, %d, %d, true, %s)' % (t, n, m, size, sc),
                              '%s x%d, %s' % (ldsx, t, mem)))
                f.append(('a64_str_roff(&mut e, %d, %d, %d, %d, %s)' % (t, n, m, size, sc),
                          '%s %s%d, %s' % (st, reg, t, mem)))
    return f


oracle.a64_forms = a64_forms
oracle.x64_forms = x64_forms

if __name__ == '__main__':
    sys.exit(min(oracle.main(), 125))
