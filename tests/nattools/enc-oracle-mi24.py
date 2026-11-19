#!/usr/bin/env python3
"""enc-oracle-mi24.py — tests/nattools/enc-oracle-mi22.py (and so the
scripts it extends) with the conditional-select forms llvm-roadmap MI24
added to compiler/nat_enc.fd:

  x86-64   cmovcc (x64_cmov): every condition, high and low registers
  AArch64  csel (a64_csel): eq / ne and other conditions, xzr operands
  RISC-V   Zicond czero.eqz / czero.nez (rv_czero_eqz, rv_czero_nez)

Each form is encoded by nat_enc.fd (through `fid run`) and assembled by
clang (`zig cc`); every byte must agree, as in the scripts it extends.

Usage: python3 tests/nattools/enc-oracle-mi24.py [--only] [--fid release/fid.exe] [--zig zig]
--only: the MI24 forms with one form of each base list (as enc-oracle-mi22.py).
"""
import importlib.util
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location('enc_oracle_mi22', os.path.join(HERE, 'enc-oracle-mi22.py'))
mi22 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(mi22)
oracle = mi22.oracle

R64 = mi22.R64
CC = ['o', 'no', 'b', 'ae', 'e', 'ne', 'be', 'a', 's', 'ns', 'p', 'np', 'l', 'ge', 'le', 'g']
A64_CC = ['eq', 'ne', 'hs', 'lo', 'mi', 'pl', 'vs', 'vc', 'hi', 'ls', 'ge', 'lt', 'gt', 'le']

base_a64 = oracle.a64_forms
base_rv = oracle.rv_forms
base_x64 = oracle.x64_forms


def x64_forms():
    f = base_x64()
    for d, s in [(0, 3), (3, 12), (15, 7), (10, 11), (6, 13), (8, 5), (4, 9)]:
        for cc in range(16):
            f.append(('x64_cmov(&mut e, %d, %d, %d)' % (cc, d, s), 'cmov%s %s, %s' % (CC[cc], R64[d], R64[s])))
    return f


def a64_forms():
    f = base_a64()
    for d, n, m in [(0, 1, 2), (9, 10, 11), (19, 28, 30), (16, 17, 5), (3, 31, 4), (7, 8, 31)]:
        for cond in range(14):
            rn = 'xzr' if n == 31 else 'x%d' % n
            rm = 'xzr' if m == 31 else 'x%d' % m
            f.append(('a64_csel(&mut e, %d, %d, %d, %d)' % (d, n, m, cond),
                      'csel x%d, %s, %s, %s' % (d, rn, rm, A64_CC[cond])))
    return f


def rv_forms():
    f = base_rv()
    first = True
    for d, a, c in [(5, 6, 7), (10, 11, 12), (28, 29, 31), (1, 8, 9), (30, 6, 28)]:
        asm = 'czero.eqz x%d, x%d, x%d' % (d, a, c)
        if first:
            # the assembler accepts Zicond from here on (no bytes)
            asm = '.option arch, +zicond\n' + asm
            first = False
        f.append(('rv_czero_eqz(&mut e, %d, %d, %d)' % (d, a, c), asm))
        f.append(('rv_czero_nez(&mut e, %d, %d, %d)' % (d, a, c), 'czero.nez x%d, x%d, x%d' % (d, a, c)))
    return f


oracle.a64_forms = a64_forms
oracle.rv_forms = rv_forms
oracle.x64_forms = x64_forms

if __name__ == '__main__':
    sys.exit(min(oracle.main(), 125))
