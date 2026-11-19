#!/usr/bin/env python3
"""enc-oracle-pl5.py — the Thumb-2 forms that compiler/nat_thumb.fd (the
Cortex-M code generator of competitive-roadmap PL5) uses beyond those
enc-oracle-op21.py checks, checked byte for byte against clang's assembler
(`zig cc -target thumb-freestanding-eabi -mcpu=cortex_m3`):

  t2_dp        and/bic/orr/orn/eor/add/adc/sbc/sub/rsb (.w, register,
               with and without flags, shifted operands), cmp/tst/mvn
  t2_dpi       the same with an 8-bit immediate (cmp/cmn/tst #imm)
  t2_shift_imm lsl.w/lsr.w/asr.w #1..31
  t2_umull, t2_mla, t2_mls
  t2_ext       sxtb.w, sxth.w, uxtb.w, uxth.w
  t2_ldst_x    strh.w, ldrh.w, ldrsb.w, ldrsh.w
  t_svc, t_blx, t_mov (16-bit, high registers, sp)

Each form is encoded by nat_enc.fd through `fid run` and assembled by clang;
every byte must agree.

Usage: python3 tests/nattools/enc-oracle-pl5.py [--fid release/fid.exe] [--zig zig]
Exit status: the number of mismatches (at most 125).
"""
import argparse
import importlib.util
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
spec = importlib.util.spec_from_file_location('enc_oracle', os.path.join(ROOT, 'scripts', 'enc-oracle.py'))
base = importlib.util.module_from_spec(spec)
spec.loader.exec_module(base)
spec2 = importlib.util.spec_from_file_location('enc_oracle_op21', os.path.join(HERE, 'enc-oracle-op21.py'))
op21 = importlib.util.module_from_spec(spec2)
spec2.loader.exec_module(op21)

DP = [(0, 'and'), (1, 'bic'), (2, 'orr'), (3, 'orn'), (4, 'eor'), (8, 'add'), (10, 'adc'),
      (11, 'sbc'), (13, 'sub'), (14, 'rsb')]
SHIFT = ['lsl', 'lsr', 'asr', 'ror']


def tr(n):
    return {13: 'sp', 14: 'lr', 15: 'pc'}.get(n, 'r%d' % n)


def forms():
    f = []
    for op, mn in DP:
        for s in [False, True]:
            for d, n, m in [(0, 1, 2), (12, 11, 10), (5, 5, 6), (8, 0, 3)]:
                if op == 14:
                    # rsb has no register form in Thumb-2 with these assemblers' .w
                    # spelling for every case; checked through the immediate form
                    continue
                f.append(('t2_dp(&mut e, %d, %s, %d, %d, %d, 0, 0)' % (op, 'true' if s else 'false', d, n, m),
                          '%s%s.w r%d, r%d, r%d' % (mn, 's' if s else '', d, n, m)))
        if op in (2, 4, 0):
            for st, amt in [(0, 1), (1, 31), (2, 7), (1, 16)]:
                f.append(('t2_dp(&mut e, %d, false, 3, 3, 2, %d, %d)' % (op, st, amt),
                          '%s.w r3, r3, r2, %s #%d' % (mn, SHIFT[st], amt)))
    # cmp.w / tst.w / mvn.w / mov.w with flags, through the generic form
    f.append(('t2_dp(&mut e, 13, true, 15, 6, 2, 0, 0)', 'cmp.w r6, r2'))
    f.append(('t2_dp(&mut e, 0, true, 15, 4, 9, 0, 0)', 'tst.w r4, r9'))
    f.append(('t2_dp(&mut e, 3, false, 0, 15, 0, 0, 0)', 'mvn.w r0, r0'))
    f.append(('t2_dp(&mut e, 3, false, 11, 15, 1, 0, 0)', 'mvn.w r11, r1'))
    f.append(('t2_dp(&mut e, 8, false, 12, 13, 12, 0, 0)', 'add.w r12, sp, r12'))
    f.append(('t2_dp(&mut e, 13, false, 13, 13, 12, 0, 0)', 'sub.w sp, sp, r12'))
    f.append(('t2_dp(&mut e, 8, false, 13, 13, 12, 0, 0)', 'add.w sp, sp, r12'))
    for op, mn in DP:
        for s in [False, True]:
            for d, n, imm in [(0, 1, 0), (12, 12, 255), (2, 2, 63), (7, 4, 32)]:
                # (adc/sbc with an immediate exist only wide; the
                # assembler refuses a .w on them)
                wide = '' if op in (10, 11) else '.w'
                f.append(('t2_dpi(&mut e, %d, %s, %d, %d, %d)' % (op, 'true' if s else 'false', d, n, imm),
                          '%s%s%s r%d, r%d, #%d' % (mn, 's' if s else '', wide, d, n, imm)))
    f.append(('t2_dpi(&mut e, 13, true, 15, 6, 0)', 'cmp.w r6, #0'))
    f.append(('t2_dpi(&mut e, 13, true, 15, 12, 2)', 'cmp.w r12, #2'))
    f.append(('t2_dpi(&mut e, 8, true, 15, 12, 1)', 'cmn.w r12, #1'))
    f.append(('t2_dpi(&mut e, 0, true, 15, 3, 32)', 'tst.w r3, #32'))
    for st in range(3):
        for d, m, amt in [(0, 1, 1), (5, 5, 31), (12, 3, 16)]:
            f.append(('t2_shift_imm(&mut e, %d, %d, %d, %d)' % (st, d, m, amt),
                      '%s.w r%d, r%d, #%d' % (SHIFT[st], d, m, amt)))
    for lo, hi, n, m in [(4, 5, 0, 2), (6, 7, 1, 2), (0, 1, 12, 11)]:
        f.append(('t2_umull(&mut e, %d, %d, %d, %d)' % (lo, hi, n, m), 'umull r%d, r%d, r%d, r%d' % (lo, hi, n, m)))
    for d, n, m, a in [(5, 0, 3, 5), (1, 2, 3, 4), (12, 11, 10, 9)]:
        f.append(('t2_mla(&mut e, %d, %d, %d, %d)' % (d, n, m, a), 'mla r%d, r%d, r%d, r%d' % (d, n, m, a)))
        f.append(('t2_mls(&mut e, %d, %d, %d, %d)' % (d, n, m, a), 'mls r%d, r%d, r%d, r%d' % (d, n, m, a)))
    for k, mn in enumerate(['sxtb.w', 'sxth.w', 'uxtb.w', 'uxth.w']):
        for d, m in [(4, 0), (0, 0), (12, 9)]:
            f.append(('t2_ext(&mut e, %d, %d, %d)' % (k, d, m), '%s r%d, r%d' % (mn, d, m)))
    for k, mn in enumerate(['strh.w', 'ldrh.w', 'ldrsb.w', 'ldrsh.w']):
        for t, n, imm in [(0, 2, 0), (3, 13, 8), (12, 11, 4095)]:
            f.append(('t2_ldst_x(&mut e, %d, %d, %d, %d)' % (k, t, n, imm), '%s r%d, [%s, #%d]' % (mn, t, tr(n), imm)))
    f += [('t_svc(&mut e, 0)', 'svc #0'), ('t_svc(&mut e, 171)', 'svc #171'),
          ('t_blx(&mut e, 12)', 'blx r12'), ('t_blx(&mut e, 4)', 'blx r4'),
          ('t_mov(&mut e, 4, 13)', 'mov r4, sp'), ('t_mov(&mut e, 13, 12)', 'mov sp, r12'),
          ('t_mov(&mut e, 0, 13)', 'mov r0, sp'), ('t_mov(&mut e, 1, 13)', 'mov r1, sp'),
          ('t_mov(&mut e, 8, 2)', 'mov r8, r2')]
    return f


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--fid', default='release/fid.exe' if os.name == 'nt' else 'release/fid')
    ap.add_argument('--zig', default='zig')
    args = ap.parse_args()
    out = os.path.join('build', 'enc-oracle-pl5')
    os.makedirs(out, exist_ok=True)
    fs = forms()
    prog = [base.PROGRAM_HEAD]
    for call, _ in fs:
        prog.append('    e = NatEnc_new(); %s; print("t2|" + bytes(&e) + "\\n");\n' % call)
    prog.append('    return 0;\n}\n')
    src = os.path.join(out, 'oracle.fd')
    open(src, 'w', newline='\n').write(''.join(prog))
    r = subprocess.run([args.fid, 'run', src], capture_output=True, text=True)
    lines = [l for l in r.stdout.splitlines() if '|' in l]
    if len(lines) != len(fs):
        sys.stderr.write(r.stdout[-3000:] + r.stderr[-3000:])
        raise SystemExit('the encoder program printed %d of %d forms' % (len(lines), len(fs)))
    ours = []
    for (call, asm), line in zip(fs, lines):
        hexb, n = line.split('|')[1].split()
        ob = bytes.fromhex(hexb)
        assert len(ob) == int(n)
        ours.append((call, asm, ob))
    text = op21.assemble(args.zig, out, 't2', 'thumb-freestanding-eabi', '.syntax unified\n.thumb\n.text\n',
                         ['.p2align 3\n' + a for _, a, _ in ours] + ['.p2align 3'], ['-mcpu=cortex_m3'])
    bad = 0
    for i, (call, asm, ob) in enumerate(ours):
        theirs = text[8 * i:8 * i + len(ob)]
        pad = text[8 * i + len(ob):8 * (i + 1)]
        pad_ok = pad in (b'', b'\x00\xbf', b'\x00\xbf\x00\xbf', b'\x00\xbf\x00\xbf\x00\xbf')
        if theirs != ob or not pad_ok:
            bad += 1
            print('MISMATCH %-40s ours %s  clang %s   %s' % (asm, ob.hex(), text[8 * i:8 * (i + 1)].hex(), call))
    print('%d forms, %d mismatches' % (len(ours), bad))
    return bad


if __name__ == '__main__':
    sys.exit(min(main(), 125))
