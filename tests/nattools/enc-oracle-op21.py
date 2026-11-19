#!/usr/bin/env python3
"""enc-oracle-op21.py — the vector (SIMD) forms of competitive-roadmap OP21
and the Thumb-2 forms of PL5 that compiler/nat_enc.fd encodes, checked byte
for byte against clang's assembler (`zig cc`), as scripts/enc-oracle.py does
for the scalar forms:

  x86-64   SSE2/SSE4.1 x64_vec (paddq .. divpd, pmulld), movdqu x64_vld /
           x64_vst, pshufd, punpcklqdq; AVX/AVX2 x64_vvec (128 and 256-bit,
           two- and three-byte VEX), vmovdqu x64_vvld / x64_vvst
  AArch64  NEON a64_vec (.2d/.4s/.16b), ldr/str q, dup, addp, umov, ins
  RISC-V   V 1.0 vsetvli, vle/vse, rv_vec (.vv), vredsum, vmv.x.s, vmv.v.x
  Thumb-2  movw/movt, addw/subw, the wide ALU / shift / mov / cmp forms,
           mul, sdiv/udiv, ldr.w/str.w/ldrb.w/strb.w, push.w/pop.w, b.w,
           bl, b<cc>.w, bx, nop, udf, bkpt (ARMv7-M, Cortex-M3)

Each form is encoded by nat_enc.fd through `fid run` and assembled by clang;
every byte must agree.

Usage: python3 tests/nattools/enc-oracle-op21.py [--fid release/fid.exe] [--zig zig]
Exit status: the number of mismatches (at most 125).
"""
import argparse
import importlib.util
import os
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
spec = importlib.util.spec_from_file_location('enc_oracle', os.path.join(ROOT, 'scripts', 'enc-oracle.py'))
base = importlib.util.module_from_spec(spec)
spec.loader.exec_module(base)

R64 = ['rax', 'rcx', 'rdx', 'rbx', 'rsp', 'rbp', 'rsi', 'rdi',
       'r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15']
X64_NAMES = ['paddq', 'psubq', 'paddd', 'psubd', 'pmulld', 'pand', 'por', 'pxor',
             'addpd', 'subpd', 'mulpd', 'divpd']
A64_NAMES = [('add', '2d'), ('sub', '2d'), ('add', '4s'), ('sub', '4s'), ('mul', '4s'),
             ('and', '16b'), ('orr', '16b'), ('eor', '16b'),
             ('fadd', '2d'), ('fsub', '2d'), ('fmul', '2d'), ('fdiv', '2d')]
RV_NAMES = ['vadd.vv', 'vsub.vv', 'vadd.vv', 'vsub.vv', 'vmul.vv', 'vand.vv', 'vor.vv', 'vxor.vv',
            'vfadd.vv', 'vfsub.vv', 'vfmul.vv', 'vfdiv.vv']
NKINDS = 12


def mem(bse, disp):
    if disp == 0:
        return '[%s]' % R64[bse]
    if disp < 0:
        return '[%s - %d]' % (R64[bse], -disp)
    return '[%s + %d]' % (R64[bse], disp)


def x64_forms():
    f = []
    for k in range(NKINDS):
        for a, b in [(0, 1), (8, 3), (3, 12), (15, 15), (7, 9)]:
            f.append(('x64_vec(&mut e, %d, %d, %d)' % (k, a, b), '%s xmm%d, xmm%d' % (X64_NAMES[k], a, b)))
    for bse in [0, 4, 5, 12, 13, 9]:
        for disp in [0, 16, -32, 4096]:
            for x in [1, 10]:
                f.append(('x64_vld(&mut e, %d, %d, %d)' % (x, bse, disp), 'movdqu xmm%d, xmmword ptr %s' % (x, mem(bse, disp))))
                f.append(('x64_vst(&mut e, %d, %d, %d)' % (bse, disp, x), 'movdqu xmmword ptr %s, xmm%d' % (mem(bse, disp), x)))
    for a, b, imm in [(0, 1, 0x4E), (9, 2, 0xEE), (3, 14, 0)]:
        f.append(('x64_pshufd(&mut e, %d, %d, %d)' % (a, b, imm), 'pshufd xmm%d, xmm%d, %d' % (a, b, imm)))
        f.append(('x64_punpcklqdq(&mut e, %d, %d)' % (a, b), 'punpcklqdq xmm%d, xmm%d' % (a, b)))
    for k in range(NKINDS):
        for d, a, b in [(0, 1, 2), (8, 9, 10), (3, 4, 12), (12, 3, 5), (1, 9, 2), (2, 11, 13)]:
            for l256 in [False, True]:
                rn = 'ymm' if l256 else 'xmm'
                f.append(('x64_vvec(&mut e, %d, %d, %d, %d, %s)' % (k, d, a, b, 'true' if l256 else 'false'),
                          'v%s %s%d, %s%d, %s%d' % (X64_NAMES[k], rn, d, rn, a, rn, b)))
    for bse in [0, 4, 13, 9]:
        for disp in [0, 32, -64]:
            for x in [2, 11]:
                for l256 in [False, True]:
                    rn = 'ymm' if l256 else 'xmm'
                    w = 'ymmword' if l256 else 'xmmword'
                    lb = 'true' if l256 else 'false'
                    f.append(('x64_vvld(&mut e, %d, %d, %d, %s)' % (x, bse, disp, lb), 'vmovdqu %s%d, %s ptr %s' % (rn, x, w, mem(bse, disp))))
                    f.append(('x64_vvst(&mut e, %d, %d, %d, %s)' % (bse, disp, x, lb), 'vmovdqu %s ptr %s, %s%d' % (w, mem(bse, disp), rn, x)))
    return f


def xr(n):
    return 'sp' if n == 31 else 'x%d' % n


def a64_forms():
    f = []
    for k in range(NKINDS):
        mn, arr = A64_NAMES[k]
        for d, n, m in [(0, 1, 2), (31, 30, 29), (16, 7, 24), (5, 5, 5)]:
            f.append(('a64_vec(&mut e, %d, %d, %d, %d)' % (k, d, n, m), '%s v%d.%s, v%d.%s, v%d.%s' % (mn, d, arr, n, arr, m, arr)))
    for t, n, off in [(0, 1, 0), (3, 31, 16), (31, 9, 65520), (17, 28, 4096)]:
        f.append(('a64_ldr_q(&mut e, %d, %d, %d)' % (t, n, off), 'ldr q%d, [%s, #%d]' % (t, xr(n), off)))
        f.append(('a64_str_q(&mut e, %d, %d, %d)' % (t, n, off), 'str q%d, [%s, #%d]' % (t, xr(n), off)))
    for d, n in [(0, 1), (31, 30), (7, 19)]:
        f.append(('a64_dup_2d(&mut e, %d, %d)' % (d, n), 'dup v%d.2d, x%d' % (d, n)))
        f.append(('a64_addp_d(&mut e, %d, %d)' % (d, n), 'addp d%d, v%d.2d' % (d, n)))
        for i in [0, 1]:
            f.append(('a64_umov_d(&mut e, %d, %d, %d)' % (d, n, i), 'umov x%d, v%d.d[%d]' % (d, n, i)))
            f.append(('a64_ins_d(&mut e, %d, %d, %d)' % (d, i, n), 'ins v%d.d[%d], x%d' % (d, i, n)))
    return f


def rv_forms():
    f = []
    for rd, rs1, vt, txt in [(5, 10, 0xD8, 'e64, m1, ta, ma'), (10, 11, 0xD0, 'e32, m1, ta, ma'), (0, 12, 0x98, 'e64, m1, tu, ma'), (6, 7, 0x58, 'e64, m1, ta, mu')]:
        f.append(('rv_vsetvli(&mut e, %d, %d, %d)' % (rd, rs1, vt), 'vsetvli x%d, x%d, %s' % (rd, rs1, txt)))
    for vd, rs1 in [(1, 10), (31, 2), (8, 5)]:
        for sew in [8, 16, 32, 64]:
            f.append(('rv_vle(&mut e, %d, %d, %d)' % (vd, rs1, sew), 'vle%d.v v%d, (x%d)' % (sew, vd, rs1)))
            f.append(('rv_vse(&mut e, %d, %d, %d)' % (vd, rs1, sew), 'vse%d.v v%d, (x%d)' % (sew, vd, rs1)))
    for k in range(NKINDS):
        for vd, a, b in [(1, 2, 3), (31, 30, 29), (8, 16, 24)]:
            f.append(('rv_vec(&mut e, %d, %d, %d, %d)' % (k, vd, a, b), '%s v%d, v%d, v%d' % (RV_NAMES[k], vd, a, b)))
    for vd, a, b in [(1, 2, 3), (4, 8, 4)]:
        f.append(('rv_vredsum(&mut e, %d, %d, %d)' % (vd, a, b), 'vredsum.vs v%d, v%d, v%d' % (vd, a, b)))
    for r, v in [(10, 1), (5, 31)]:
        f.append(('rv_vmv_x_s(&mut e, %d, %d)' % (r, v), 'vmv.x.s x%d, v%d' % (r, v)))
        f.append(('rv_vmv_v_x(&mut e, %d, %d)' % (v, r), 'vmv.v.x v%d, x%d' % (v, r)))
    f[0] = (f[0][0], '.option arch, +v\n' + f[0][1])
    return f


T2_ALU = [(2, 'add.w'), (3, 'sub.w'), (5, 'and.w'), (6, 'orr.w'), (7, 'eor.w'), (4, 'mul')]
T2_CC = ['eq', 'ne', 'hs', 'lo', 'mi', 'pl', 'vs', 'vc', 'hi', 'ls', 'ge', 'lt', 'gt', 'le']


def tr(n):
    return {13: 'sp', 14: 'lr', 15: 'pc'}.get(n, 'r%d' % n)


def t2_forms():
    f = []
    for d, imm in [(0, 0), (1, 0x1234), (12, 0xFFFF), (7, 0x8001), (3, 0x0FF0)]:
        f.append(('t2_movw(&mut e, %d, %d)' % (d, imm), 'movw r%d, #%d' % (d, imm)))
        f.append(('t2_movt(&mut e, %d, %d)' % (d, imm), 'movt r%d, #%d' % (d, imm)))
    for d, n, imm in [(0, 1, 0), (2, 13, 8), (12, 11, 4095), (4, 4, 0x801)]:
        f.append(('t2_addw(&mut e, %d, %d, %d)' % (d, n, imm), 'addw %s, %s, #%d' % (tr(d), tr(n), imm)))
        f.append(('t2_subw(&mut e, %d, %d, %d)' % (d, n, imm), 'subw %s, %s, #%d' % (tr(d), tr(n), imm)))
    for k, mn in T2_ALU:
        for d, n, m in [(0, 1, 2), (12, 11, 10), (8, 0, 7), (3, 3, 3)]:
            f.append(('t2_alu(&mut e, %d, %d, %d, %d)' % (k, d, n, m), '%s r%d, r%d, r%d' % (mn, d, n, m)))
    for d, n, m in [(0, 1, 2), (12, 9, 4)]:
        f.append(('t2_div(&mut e, %d, %d, %d, false)' % (d, n, m), 'sdiv r%d, r%d, r%d' % (d, n, m)))
        f.append(('t2_div(&mut e, %d, %d, %d, true)' % (d, n, m), 'udiv r%d, r%d, r%d' % (d, n, m)))
        for k, mn in [(0, 'lsl.w'), (1, 'lsr.w'), (2, 'asr.w')]:
            f.append(('t2_shift(&mut e, %d, %d, %d, %d)' % (k, d, n, m), '%s r%d, r%d, r%d' % (mn, d, n, m)))
        f.append(('t2_mov(&mut e, %d, %d)' % (d, m), 'mov.w r%d, r%d' % (d, m)))
        f.append(('t2_cmp(&mut e, %d, %d)' % (n, m), 'cmp.w r%d, r%d' % (n, m)))
    for t, n, imm in [(0, 1, 0), (2, 13, 8), (12, 11, 4095), (14, 6, 0x123)]:
        for load in [True, False]:
            for byte in [False, True]:
                if byte and t == 14:
                    continue
                mn = ('ldr' if load else 'str') + ('b' if byte else '') + '.w'
                f.append(('t2_ldst(&mut e, %s, %s, %d, %d, %d)' % ('true' if load else 'false', 'true' if byte else 'false', t, n, imm),
                          '%s %s, [%s, #%d]' % (mn, tr(t), tr(n), imm)))
    for mask, txt in [((1 << 4) | (1 << 5) | (1 << 14), '{r4, r5, lr}'), ((1 << 4) | (1 << 14), '{r4, lr}'),
                      (0x0FF0 | (1 << 14), '{r4, r5, r6, r7, r8, r9, r10, r11, lr}')]:
        f.append(('t2_push(&mut e, %d)' % mask, 'push.w ' + txt))
    for mask, txt in [((1 << 4) | (1 << 5) | (1 << 15), '{r4, r5, pc}'), ((1 << 4) | (1 << 15), '{r4, pc}')]:
        f.append(('t2_pop(&mut e, %d)' % mask, 'pop.w ' + txt))
    for off in [0, 8, -8, 0x1000, -0x2000, 0x7FFFFE, -0x800000, 0x123456]:
        f.append(('t2_b(&mut e, %d)' % off, 'b.w . + %d' % (4 + off)))
        f.append(('t2_bl(&mut e, %d)' % off, 'bl . + %d' % (4 + off)))
    f += [('t_bx(&mut e, 14)', 'bx lr'), ('t_bx(&mut e, 3)', 'bx r3'), ('t_nop(&mut e)', 'nop'),
          ('t_udf(&mut e)', 'udf #0'), ('t_bkpt(&mut e, 171)', 'bkpt #0xab')]
    return f


def t2b_forms():
    # b<cc>.w: the assembler takes no `. + n` target for a conditional
    # branch, so each is assembled alone against a label n bytes away
    f = []
    for cc, off in [(c, 8) for c in range(14)] + [(0, -8), (1, 0x40000), (10, -0x100000), (13, 0xFFFFE), (11, 0), (12, -4)]:
        if off >= 0:
            asm = 'b%s.w 1f\n.space %d\n1:' % (T2_CC[cc], off)
            at = 0
        else:
            asm = '1:\n.space %d\nb%s.w 1b' % (-off - 4, T2_CC[cc])
            at = -off - 4
        f.append(('t2_bcc(&mut e, %d, %d)' % (cc, off), (asm, at)))
    return f


def elf_text(path):
    data = open(path, 'rb').read()
    if data[4] == 2:
        return base.elf_text(path)
    shoff = struct.unpack_from('<I', data, 0x20)[0]
    shentsize, shnum, shstrndx = struct.unpack_from('<HHH', data, 0x2E)

    def sh(i):
        return struct.unpack_from('<IIIIIIIIII', data, shoff + i * shentsize)
    strtab = sh(shstrndx)
    for i in range(shnum):
        s = sh(i)
        name_off = strtab[4] + s[0]
        name = data[name_off:data.index(b'\0', name_off)].decode()
        if name == '.text':
            return data[s[4]:s[4] + s[5]]
    raise SystemExit('no .text in ' + path)


def assemble(zig, out, name, target, head, lines, extra=()):
    s_path = os.path.join(out, name + '.s')
    o_path = os.path.join(out, name + '.o')
    with open(s_path, 'w', newline='\n') as fh:
        fh.write(head + ''.join(l + '\n' for l in lines))
    r = subprocess.run([zig, 'cc', '-target', target] + list(extra) + ['-c', s_path, '-o', o_path],
                       capture_output=True, text=True)
    if r.returncode != 0:
        sys.stderr.write(r.stderr[-4000:])
        raise SystemExit('the assembler refused ' + s_path)
    return elf_text(o_path)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--fid', default='release/fid.exe' if os.name == 'nt' else 'release/fid')
    ap.add_argument('--zig', default='zig')
    ap.add_argument('--reuse', action='store_true', help='reuse the last encoder run (same forms)')
    args = ap.parse_args()
    out = os.path.join('build', 'enc-oracle-op21')
    os.makedirs(out, exist_ok=True)
    forms = [('x64', c, a) for c, a in x64_forms()] + [('a64', c, a) for c, a in a64_forms()] \
        + [('rv', c, a) for c, a in rv_forms()] + [('t2', c, a) for c, a in t2_forms()] \
        + [('t2b', c, a) for c, a in t2b_forms()]
    prog = [base.PROGRAM_HEAD]
    for arch, call, _ in forms:
        prog.append('    e = NatEnc_new(); %s; print("%s|" + bytes(&e) + "\\n");\n' % (call, arch))
    prog.append('    return 0;\n}\n')
    src = os.path.join(out, 'oracle.fd')
    open(src, 'w', newline='\n').write(''.join(prog))
    cache = os.path.join(out, 'ours.txt')
    if args.reuse and os.path.exists(cache):
        stdout, stderr = open(cache).read(), ''
    else:
        r = subprocess.run([args.fid, 'run', src], capture_output=True, text=True)
        stdout, stderr = r.stdout, r.stderr
        open(cache, 'w').write(stdout)
    lines = [l for l in stdout.splitlines() if '|' in l]
    if len(lines) != len(forms):
        sys.stderr.write(stdout[-3000:] + stderr[-3000:])
        raise SystemExit('the encoder program printed %d of %d forms' % (len(lines), len(forms)))
    ours = {}
    for (arch, call, asm), line in zip(forms, lines):
        hexb, n = line.split('|')[1].split()
        ob = bytes.fromhex(hexb)
        assert len(ob) == int(n)
        ours.setdefault(arch, []).append((call, asm, ob))
    bad = 0
    total = 0
    # x86-64 and Thumb-2: each form in its own 16-byte (8-byte) slot, the
    # assembler's padding after it; AArch64 and RISC-V: one word each
    x64_text = assemble(args.zig, out, 'x64', 'x86_64-linux-musl', '.intel_syntax noprefix\n.text\n',
                        ['.p2align 4, 0x90\n' + a for _, a, _ in ours['x64']] + ['.p2align 4, 0x90'])
    t2_text = assemble(args.zig, out, 't2', 'thumb-freestanding-eabi', '.syntax unified\n.thumb\n.text\n',
                       ['.p2align 3\n' + a for _, a, _ in ours['t2']] + ['.p2align 3'], ['-mcpu=cortex_m3'])
    for arch, text, slot in [('x64', x64_text, 16), ('t2', t2_text, 8)]:
        for i, (call, asm, ob) in enumerate(ours[arch]):
            total += 1
            theirs = text[slot * i:slot * i + len(ob)]
            pad = text[slot * i + len(ob):slot * (i + 1)]
            pad_ok = all(b == 0x90 for b in pad) if arch == 'x64' else pad in (b'', b'\x00\xbf', b'\x00\xbf\x00\xbf', b'\x00\xbf\x00\xbf\x00\xbf')
            if theirs != ob or not pad_ok:
                bad += 1
                print('MISMATCH %-4s %-44s ours %s  clang %s   %s' % (arch, asm.replace('\n', ' '), ob.hex(),
                                                                       text[slot * i:slot * (i + 1)].hex(), call))
    for i, (call, (asm, at), ob) in enumerate(ours['t2b']):
        total += 1
        text = assemble(args.zig, out, 't2b%d' % i, 'thumb-freestanding-eabi', '.syntax unified\n.thumb\n.text\n',
                        [asm], ['-mcpu=cortex_m3'])
        theirs = text[at:at + 4]
        if theirs != ob:
            bad += 1
            print('MISMATCH t2   %-44s ours %s  clang %s   %s' % (asm.replace('\n', ' '), ob.hex(), theirs.hex(), call))
    for arch, target, head in [('a64', 'aarch64-linux-musl', '.text\n'), ('rv', 'riscv64-linux-musl', '.text\n.option norvc\n')]:
        text = assemble(args.zig, out, arch, target, head, [a for _, a, _ in ours[arch]])
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
