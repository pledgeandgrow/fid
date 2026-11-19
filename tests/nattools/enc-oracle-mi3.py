#!/usr/bin/env python3
"""enc-oracle-mi3.py — scripts/enc-oracle-tg.py (and so scripts/enc-oracle.py)
with the instruction forms that llvm-roadmap MI3 / competitive-roadmap
OP20 added to compiler/nat_enc.fd:

  AArch64  madd (a64_madd), bic (a64_bic)
  RISC-V   Zbb andn (rv_andn)
  x86-64   BMI1 andn (x64_andn), blsr (x64_blsr)

Each form is encoded by nat_enc.fd (through `fid run`) and assembled by
clang (`zig cc`); every byte must agree, as in the scripts it extends.

Usage: python3 tests/nattools/enc-oracle-mi3.py [--fid release/fid.exe] [--zig zig]
"""
import importlib.util
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
spec = importlib.util.spec_from_file_location('enc_oracle_tg', os.path.join(ROOT, 'scripts', 'enc-oracle-tg.py'))
tg = importlib.util.module_from_spec(spec)
spec.loader.exec_module(tg)
oracle = tg.oracle

R = tg.R

base_a64 = oracle.a64_forms
base_rv = oracle.rv_forms
base_x64 = oracle.x64_forms


def a64_forms():
    f = base_a64()
    for d, n, m, a in [(0, 1, 2, 3), (9, 10, 11, 12), (19, 28, 30, 5), (16, 17, 5, 31)]:
        f.append(('a64_madd(&mut e, %d, %d, %d, %d)' % (d, n, m, a),
                  'madd x%d, x%d, x%d, %s' % (d, n, m, 'xzr' if a == 31 else 'x%d' % a)))
        f.append(('a64_bic(&mut e, %d, %d, %d)' % (d, n, m), 'bic x%d, x%d, x%d' % (d, n, m)))
    return f


def rv_forms():
    f = base_rv()
    # (enc-oracle-tg's forms already turned Zba/Zbb on in the assembler)
    for d, a, b in [(5, 6, 7), (10, 11, 12), (28, 29, 31), (1, 8, 9)]:
        f.append(('rv_andn(&mut e, %d, %d, %d)' % (d, a, b), 'andn x%d, x%d, x%d' % (d, a, b)))
    return f


def x64_forms():
    f = base_x64()
    for d, s, o in [(0, 3, 1), (3, 12, 7), (15, 7, 8), (10, 11, 2), (6, 13, 14), (8, 5, 9)]:
        f.append(('x64_andn(&mut e, %d, %d, %d)' % (d, s, o), 'andn %s, %s, %s' % (R[d], R[s], R[o])))
        f.append(('x64_blsr(&mut e, %d, %d)' % (d, o), 'blsr %s, %s' % (R[d], R[o])))
    return f


oracle.a64_forms = a64_forms
oracle.rv_forms = rv_forms
oracle.x64_forms = x64_forms

if __name__ == '__main__':
    sys.exit(min(oracle.main(), 125))
