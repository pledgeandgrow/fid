#!/usr/bin/env python3
"""enc-oracle-op18.py — tests/nattools/enc-oracle-mi24.py (and so the
scripts it extends) with the x86-64 memory-operand forms of competitive
OP18 in compiler/nat_enc.fd:

  cmp size ptr [b + d], imm   x64_cmp_mem_imm (byte, word, dword, qword;
                              short and long immediates)
  op qword ptr [b + d], r     x64_alu_to_mem (add, sub, and, or, xor)
  op qword ptr [b + d], imm   x64_alu_mem_imm
  cmp r, qword ptr [b + d]    x64_alu_mem with 0x3B

Each form is encoded by nat_enc.fd (through `fid run`) and assembled by
clang (`zig cc`); every byte must agree, as in the scripts it extends.
Bases cover rsp / r12 (a SIB byte), rbp / r13 (no form without a
displacement) and the REX.B registers; displacements 0, 8 bits and 32 bits.

Usage: python3 tests/nattools/enc-oracle-op18.py [--only] [--fid release/fid.exe] [--zig zig]
--only: the OP18 forms with one form of each base list (as enc-oracle-mi22.py).
"""
import importlib.util
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location('enc_oracle_mi24', os.path.join(HERE, 'enc-oracle-mi24.py'))
mi24 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(mi24)
oracle = mi24.oracle

R64 = mi24.R64
PTR = {1: 'byte', 2: 'word', 4: 'dword', 8: 'qword'}

base_x64 = oracle.x64_forms


def mem(b, disp):
    if disp > 0:
        return '[%s + %d]' % (R64[b], disp)
    if disp < 0:
        return '[%s - %d]' % (R64[b], -disp)
    return '[%s]' % R64[b]


def x64_forms():
    f = base_x64()
    bases = [(0, 0), (4, 8), (5, 0), (12, 16), (13, 0), (7, 200), (15, 4096), (3, 0 - 8), (9, 127), (6, 128)]
    imms = {
        1: [0, 1, 200, 255, 0 - 5, 0 - 128],
        2: [0, 0 - 3, 127, 200, 40000, 65535, 0 - 32768],
        4: [0, 100000, 0 - 5, 3000000000, 4294967295, 2147483647],
        8: [0, 0 - 100, 1000, 0 - 2147483648, 2147483647, 127, 0 - 129],
    }
    for b, disp in bases:
        for size in (1, 2, 4, 8):
            for imm in imms[size]:
                f.append(('x64_cmp_mem_imm(&mut e, %d, %d, %d, %d)' % (size, b, disp, imm),
                          'cmp %s ptr %s, %d' % (PTR[size], mem(b, disp), imm)))
    ops = [(0x01, 'add'), (0x29, 'sub'), (0x21, 'and'), (0x09, 'or'), (0x31, 'xor')]
    exts = [(0, 'add'), (5, 'sub'), (4, 'and'), (1, 'or'), (6, 'xor')]
    for s, b, disp in [(0, 7, 8), (9, 4, 16), (3, 12, 0), (15, 13, 0), (11, 5, 300), (2, 1, 0 - 24)]:
        for op, name in ops:
            f.append(('x64_alu_to_mem(&mut e, %d, %d, %d, %d)' % (op, s, b, disp),
                      '%s qword ptr %s, %s' % (name, mem(b, disp), R64[s])))
        f.append(('x64_alu_mem(&mut e, %d, %d, %d, %d)' % (0x3B, s, b, disp),
                  'cmp %s, qword ptr %s' % (R64[s], mem(b, disp))))
    for b, disp in [(7, 8), (4, 0), (12, 16), (13, 0), (0, 1000)]:
        for ext, name in exts:
            for imm in (4, 0 - 1, 200, 0 - 2147483648, 21845):
                f.append(('x64_alu_mem_imm(&mut e, %d, %d, %d, %d)' % (ext, b, disp, imm),
                          '%s qword ptr %s, %d' % (name, mem(b, disp), imm)))
    return f


oracle.x64_forms = x64_forms

if __name__ == '__main__':
    sys.exit(min(oracle.main(), 125))
