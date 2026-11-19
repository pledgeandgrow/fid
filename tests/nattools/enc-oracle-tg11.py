#!/usr/bin/env python3
"""enc-oracle-tg11.py — the vector forms that llvm-roadmap TG11 (the machine
IR's vector operations, compiler/nat_ir.fd MO_V*) added to
compiler/nat_enc.fd, checked byte for byte against clang's assembler
(`zig cc`), as tests/nattools/enc-oracle-op21.py does for OP21's forms:

  x86-64   cmppd, vcmppd (128/256), vpbroadcastq (128/256), vzeroupper
  AArch64  fcmeq .2d #0.0, bic .16b
  RISC-V   vsll.vi, vmseq.vi, vmerge.vim, vslidedown.vi

Each form is encoded by nat_enc.fd through `fid run` and assembled by clang;
every byte must agree. Usage:

  python3 tests/nattools/enc-oracle-tg11.py [--fid release/fid.exe] [--zig zig]

Exit status: the number of mismatches (at most 125).
"""
import argparse
import importlib.util
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location('enc_oracle_op21', os.path.join(HERE, 'enc-oracle-op21.py'))
op21 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(op21)
base = op21.base


def x64_forms():
    f = []
    for a, b in [(0, 1), (12, 13), (15, 3), (7, 9), (14, 14)]:
        for imm, mn in [(0, 'cmpeqpd'), (4, 'cmpneqpd'), (1, 'cmpltpd')]:
            f.append(('x64_cmppd(&mut e, %d, %d, %d)' % (a, b, imm), '%s xmm%d, xmm%d' % (mn, a, b)))
    for d, a, b in [(0, 1, 2), (12, 12, 13), (15, 15, 12), (3, 9, 14), (8, 2, 5)]:
        for l256 in [False, True]:
            rn = 'ymm' if l256 else 'xmm'
            lb = 'true' if l256 else 'false'
            f.append(('x64_vcmppd(&mut e, %d, %d, %d, 4, %s)' % (d, a, b, lb),
                      'vcmpneqpd %s%d, %s%d, %s%d' % (rn, d, rn, a, rn, b)))
            f.append(('x64_vcmppd(&mut e, %d, %d, %d, 0, %s)' % (d, a, b, lb),
                      'vcmpeqpd %s%d, %s%d, %s%d' % (rn, d, rn, a, rn, b)))
    for d, s in [(0, 1), (12, 12), (13, 3), (2, 15), (15, 15)]:
        for l256 in [False, True]:
            rn = 'ymm' if l256 else 'xmm'
            lb = 'true' if l256 else 'false'
            f.append(('x64_vpbroadcastq(&mut e, %d, %d, %s)' % (d, s, lb), 'vpbroadcastq %s%d, xmm%d' % (rn, d, s)))
    f.append(('x64_vzeroupper(&mut e)', 'vzeroupper'))
    for x, bse, disp in [(0, 0, 0), (1, 4, 16), (13, 5, -32), (1, 12, 4096), (0, 13, 0), (9, 9, 8)]:
        f.append(('x64_cmppd_m(&mut e, %d, %d, %d, 4)' % (x, bse, disp), 'cmpneqpd xmm%d, xmmword ptr %s' % (x, op21.mem(bse, disp))))
        for l256 in [False, True]:
            rn = 'ymm' if l256 else 'xmm'
            w = 'ymmword' if l256 else 'xmmword'
            lb = 'true' if l256 else 'false'
            f.append(('x64_vcmppd_m(&mut e, %d, %d, %d, %d, 4, %s)' % (x, x, bse, disp, lb),
                      'vcmpneqpd %s%d, %s%d, %s ptr %s' % (rn, x, rn, x, w, op21.mem(bse, disp))))
    return f


def a64_forms():
    f = []
    for d, n in [(0, 1), (31, 30), (28, 29), (7, 19)]:
        f.append(('a64_fcmeq_zero_2d(&mut e, %d, %d)' % (d, n), 'fcmeq v%d.2d, v%d.2d, #0.0' % (d, n)))
    for d, n, m in [(0, 1, 2), (28, 28, 31), (31, 30, 29), (5, 5, 5)]:
        f.append(('a64_bic_16b(&mut e, %d, %d, %d)' % (d, n, m), 'bic v%d.16b, v%d.16b, v%d.16b' % (d, n, m)))
    for d, n in [(0, 1), (2, 3), (31, 16), (17, 2)]:
        for i in [0, 1]:
            f.append(('a64_dup_elem_2d(&mut e, %d, %d, %d)' % (d, n, i), 'dup v%d.2d, v%d.d[%d]' % (d, n, i)))
            f.append(('a64_dup_d_elem(&mut e, %d, %d, %d)' % (d, n, i), 'mov d%d, v%d.d[%d]' % (d, n, i)))
    return f


def rv_forms():
    f = []
    for vd, vs2, imm in [(1, 2, 1), (4, 3, 31), (31, 30, 0), (8, 16, 5)]:
        f.append(('rv_vsll_vi(&mut e, %d, %d, %d)' % (vd, vs2, imm), 'vsll.vi v%d, v%d, %d' % (vd, vs2, imm)))
        f.append(('rv_vslidedown_vi(&mut e, %d, %d, %d)' % (vd, vs2, imm), 'vslidedown.vi v%d, v%d, %d' % (vd, vs2, imm)))
    for vd, vs2, imm in [(0, 4, 0), (0, 1, 3), (2, 7, 15), (0, 31, -16), (5, 9, -1)]:
        f.append(('rv_vmseq_vi(&mut e, %d, %d, %d)' % (vd, vs2, imm), 'vmseq.vi v%d, v%d, %d' % (vd, vs2, imm)))
    for vd, vs2, imm in [(1, 1, 0), (3, 4, 7), (31, 2, -16), (8, 8, -1)]:
        f.append(('rv_vmerge_vim(&mut e, %d, %d, %d)' % (vd, vs2, imm), 'vmerge.vim v%d, v%d, %d, v0' % (vd, vs2, imm)))
    for fr, v in [(0, 1), (31, 2), (12, 4), (2, 31)]:
        f.append(('rv_vfmv_f_s(&mut e, %d, %d)' % (fr, v), 'vfmv.f.s f%d, v%d' % (fr, v)))
        f.append(('rv_vfmv_v_f(&mut e, %d, %d)' % (v, fr), 'vfmv.v.f v%d, f%d' % (v, fr)))
    f[0] = (f[0][0], '.option arch, +v\n' + f[0][1])
    return f


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--fid', default='release/fid.exe' if os.name == 'nt' else 'release/fid')
    ap.add_argument('--zig', default='zig')
    args = ap.parse_args()
    out = os.path.join('build', 'enc-oracle-tg11')
    os.makedirs(out, exist_ok=True)
    forms = [('x64', c, a) for c, a in x64_forms()] + [('a64', c, a) for c, a in a64_forms()] \
        + [('rv', c, a) for c, a in rv_forms()]
    prog = [base.PROGRAM_HEAD]
    for arch, call, _ in forms:
        prog.append('    e = NatEnc_new(); %s; print("%s|" + bytes(&e) + "\\n");\n' % (call, arch))
    prog.append('    return 0;\n}\n')
    src = os.path.join(out, 'oracle.fd')
    open(src, 'w', newline='\n').write(''.join(prog))
    r = subprocess.run([args.fid, 'run', src], capture_output=True, text=True)
    lines = [l for l in r.stdout.splitlines() if '|' in l]
    if len(lines) != len(forms):
        sys.stderr.write(r.stdout[-3000:] + r.stderr[-3000:])
        raise SystemExit('the encoder program printed %d of %d forms' % (len(lines), len(forms)))
    ours = {}
    for (arch, call, asm), line in zip(forms, lines):
        hexb, n = line.split('|')[1].split()
        ob = bytes.fromhex(hexb)
        assert len(ob) == int(n)
        ours.setdefault(arch, []).append((call, asm, ob))
    bad = 0
    total = 0
    x64_text = op21.assemble(args.zig, out, 'x64', 'x86_64-linux-musl', '.intel_syntax noprefix\n.text\n',
                             ['.p2align 4, 0x90\n' + a for _, a, _ in ours['x64']] + ['.p2align 4, 0x90'])
    for i, (call, asm, ob) in enumerate(ours['x64']):
        total += 1
        theirs = x64_text[16 * i:16 * i + len(ob)]
        pad = x64_text[16 * i + len(ob):16 * (i + 1)]
        if theirs != ob or not all(b == 0x90 for b in pad):
            bad += 1
            print('MISMATCH x64  %-44s ours %s  clang %s   %s' % (asm, ob.hex(), x64_text[16 * i:16 * (i + 1)].hex(), call))
    for arch, target, head in [('a64', 'aarch64-linux-musl', '.text\n'), ('rv', 'riscv64-linux-musl', '.text\n.option norvc\n')]:
        text = op21.assemble(args.zig, out, arch, target, head, [a for _, a, _ in ours[arch]])
        for i, (call, asm, ob) in enumerate(ours[arch]):
            total += 1
            theirs = text[4 * i:4 * i + 4]
            if len(ob) != 4 or theirs != ob:
                bad += 1
                print('MISMATCH %-4s %-44s ours %s  clang %s   %s' % (arch, asm.replace('\n', ' '), ob.hex(), theirs.hex(), call))
    print('%d forms, %d mismatches' % (total, bad))
    return bad


if __name__ == '__main__':
    sys.exit(min(main(), 125))
