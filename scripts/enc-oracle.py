#!/usr/bin/env python3
"""enc-oracle.py — our AArch64 and RISC-V encoders against clang's assembler
(own-backend roadmap TG4, RL8).

Every instruction form compiler/nat_enc.fd emits is encoded with several
operand choices by a generated Fides program (run with `fid run`), and the
same instructions are assembled by clang's integrated assembler through
`zig cc -c`. The words must be equal.

Usage: python3 scripts/enc-oracle.py [--fid release/fid.exe] [--zig zig]
Run from the repository root. Writes build/enc-oracle/; prints each mismatch
and a summary; the exit status is the number of mismatches (0: all agree).
"""
import argparse
import os
import struct
import subprocess
import sys

CC = {'eq': 0, 'ne': 1, 'hs': 2, 'lo': 3, 'mi': 4, 'pl': 5, 'hi': 8, 'ls': 9, 'ge': 10, 'lt': 11, 'gt': 12, 'le': 13}


def a64_forms():
    f = []
    regs = [(0, 1, 2), (9, 10, 11), (19, 28, 30), (16, 17, 5)]
    for d, n, m in regs:
        for op, asm in [('add', 'add'), ('adds', 'adds'), ('sub', 'sub'), ('subs', 'subs'), ('and', 'and'),
                        ('orr', 'orr'), ('eor', 'eor'), ('mul', 'mul'), ('smulh', 'smulh'), ('umulh', 'umulh'),
                        ('sdiv', 'sdiv'), ('udiv', 'udiv'), ('lslv', 'lsl'), ('lsrv', 'lsr'), ('asrv', 'asr')]:
            f.append(('a64_%s(&mut e, %d, %d, %d)' % (op, d, n, m), '%s x%d, x%d, x%d' % (asm, d, n, m)))
        f.append(('a64_msub(&mut e, %d, %d, %d, 3)' % (d, n, m), 'msub x%d, x%d, x%d, x3' % (d, n, m)))
        f.append(('a64_mov(&mut e, %d, %d)' % (d, n), 'mov x%d, x%d' % (d, n)))
        f.append(('a64_mvn(&mut e, %d, %d)' % (d, n), 'mvn x%d, x%d' % (d, n)))
        f.append(('a64_neg(&mut e, %d, %d)' % (d, n), 'neg x%d, x%d' % (d, n)))
        f.append(('a64_cmp(&mut e, %d, %d)' % (n, m), 'cmp x%d, x%d' % (n, m)))
        for ext, asm in [('sxtb', 'sxtb x%d, w%d'), ('sxth', 'sxth x%d, w%d'), ('sxtw', 'sxtw x%d, w%d'),
                         ('uxtb', 'uxtb w%d, w%d'), ('uxth', 'uxth w%d, w%d'), ('uxtw', 'mov w%d, w%d')]:
            f.append(('a64_%s(&mut e, %d, %d)' % (ext, d, n), asm % (d, n)))
        f.append(('a64_asr63(&mut e, %d, %d)' % (d, n), 'asr x%d, x%d, #63' % (d, n)))
        for c, v in CC.items():
            f.append(('a64_cset(&mut e, %d, %d)' % (d, v), 'cset x%d, %s' % (d, c)))
        f.append(('a64_blr(&mut e, %d)' % n, 'blr x%d' % n))
        f.append(('a64_ldaxr(&mut e, %d, %d)' % (d, n), 'ldaxr x%d, [x%d]' % (d, n)))
        f.append(('a64_stlxr(&mut e, 11, %d, %d)' % (d, n), 'stlxr w11, x%d, [x%d]' % (d, n)))
        f.append(('a64_ldar(&mut e, %d, %d)' % (d, n), 'ldar x%d, [x%d]' % (d, n)))
        f.append(('a64_stlr(&mut e, %d, %d)' % (d, n), 'stlr x%d, [x%d]' % (d, n)))
    for d, n, imm in [(0, 1, 0), (9, 31, 16), (31, 31, 4080), (31, 1, 0), (5, 6, 4095)]:
        rd = 'sp' if d == 31 else 'x%d' % d
        rn = 'sp' if n == 31 else 'x%d' % n
        f.append(('a64_add_imm(&mut e, %d, %d, %d)' % (d, n, imm), 'add %s, %s, #%d' % (rd, rn, imm)))
        f.append(('a64_sub_imm(&mut e, %d, %d, %d)' % (d, n, imm), 'sub %s, %s, #%d' % (rd, rn, imm)))
    f.append(('a64_add_ext(&mut e, 9, 31, 9)', 'add x9, sp, x9'))
    f.append(('a64_sub_ext(&mut e, 31, 31, 16)', 'sub sp, sp, x16'))
    for t, n, imm in [(0, 1, 0), (9, 31, 8), (19, 16, 4088), (3, 29, 64)]:
        rn = 'sp' if n == 31 else 'x%d' % n
        f.append(('a64_ldr_sized(&mut e, %d, %d, %d, 8, false)' % (t, n, imm), 'ldr x%d, [%s, #%d]' % (t, rn, imm)))
        f.append(('a64_str_sized(&mut e, %d, %d, %d, 8)' % (t, n, imm), 'str x%d, [%s, #%d]' % (t, rn, imm)))
        i4 = (imm // 2) & ~3
        f.append(('a64_ldr_sized(&mut e, %d, %d, %d, 4, false)' % (t, n, i4), 'ldr w%d, [%s, #%d]' % (t, rn, i4)))
        f.append(('a64_ldr_sized(&mut e, %d, %d, %d, 4, true)' % (t, n, i4), 'ldrsw x%d, [%s, #%d]' % (t, rn, i4)))
        f.append(('a64_str_sized(&mut e, %d, %d, %d, 4)' % (t, n, i4), 'str w%d, [%s, #%d]' % (t, rn, i4)))
        i2 = (imm // 4) & ~1
        f.append(('a64_ldr_sized(&mut e, %d, %d, %d, 2, false)' % (t, n, i2), 'ldrh w%d, [%s, #%d]' % (t, rn, i2)))
        f.append(('a64_ldr_sized(&mut e, %d, %d, %d, 2, true)' % (t, n, i2), 'ldrsh x%d, [%s, #%d]' % (t, rn, i2)))
        f.append(('a64_str_sized(&mut e, %d, %d, %d, 2)' % (t, n, i2), 'strh w%d, [%s, #%d]' % (t, rn, i2)))
        i1 = imm // 8
        f.append(('a64_ldr_sized(&mut e, %d, %d, %d, 1, false)' % (t, n, i1), 'ldrb w%d, [%s, #%d]' % (t, rn, i1)))
        f.append(('a64_ldr_sized(&mut e, %d, %d, %d, 1, true)' % (t, n, i1), 'ldrsb x%d, [%s, #%d]' % (t, rn, i1)))
        f.append(('a64_str_sized(&mut e, %d, %d, %d, 1)' % (t, n, i1), 'strb w%d, [%s, #%d]' % (t, rn, i1)))
    for d, n, m in [(0, 1, 2), (7, 3, 31)]:
        for op in ['fadd', 'fsub', 'fmul', 'fdiv']:
            f.append(('a64_%s(&mut e, %d, %d, %d)' % (op, d, n, m), '%s d%d, d%d, d%d' % (op, d, n, m)))
        f.append(('a64_fcmp(&mut e, %d, %d)' % (n, m), 'fcmp d%d, d%d' % (n, m)))
        for op in ['fsqrt', 'fabs', 'frintm', 'frintp', 'frintz', 'frinta']:
            f.append(('a64_%s(&mut e, %d, %d)' % (op, d, n), '%s d%d, d%d' % (op, d, n)))
        f.append(('a64_fcvt_ds(&mut e, %d, %d)' % (d, n), 'fcvt d%d, s%d' % (d, n)))
        f.append(('a64_fcvt_sd(&mut e, %d, %d)' % (d, n), 'fcvt s%d, d%d' % (d, n)))
    for d, n in [(0, 9), (2, 30)]:
        f.append(('a64_fmov_dx(&mut e, %d, %d)' % (d, n), 'fmov d%d, x%d' % (d, n)))
        f.append(('a64_fmov_xd(&mut e, %d, %d)' % (n, d), 'fmov x%d, d%d' % (n, d)))
        f.append(('a64_fmov_sw(&mut e, %d, %d)' % (d, n), 'fmov s%d, w%d' % (d, n)))
        f.append(('a64_fmov_ws(&mut e, %d, %d)' % (n, d), 'fmov w%d, s%d' % (n, d)))
        f.append(('a64_scvtf(&mut e, %d, %d)' % (d, n), 'scvtf d%d, x%d' % (d, n)))
        f.append(('a64_fcvtzs(&mut e, %d, %d)' % (n, d), 'fcvtzs x%d, d%d' % (n, d)))
    f += [('a64_ret(&mut e)', 'ret'), ('a64_svc0(&mut e)', 'svc #0'), ('a64_brk(&mut e)', 'brk #0'),
          ('a64_clrex(&mut e)', 'clrex'), ('a64_dmb(&mut e)', 'dmb ish'),
          ('a64_cbnz_w_words(&mut e, 11, 0 - 3)', 'cbnz w11, .-12'),
          ('a64_cbnz_words(&mut e, 4, 7)', 'cbnz x4, .+28'),
          ('a64_bcond_words(&mut e, 1, 5)', 'b.ne .+20'),
          ('a64_bcond_words(&mut e, 11, 0 - 2)', 'b.lt .-8')]
    return f


def rv_forms():
    f = []
    for d, a, b in [(5, 6, 7), (10, 11, 12), (28, 29, 31), (1, 8, 9)]:
        for op in ['add', 'sub', 'sll', 'slt', 'sltu', 'xor', 'srl', 'sra', 'or', 'and', 'mul', 'mulh', 'mulhu',
                   'div', 'divu', 'rem', 'remu']:
            f.append(('rv_%s(&mut e, %d, %d, %d)' % (op, d, a, b), '%s x%d, x%d, x%d' % (op, d, a, b)))
        for imm in [0, 1, -1, 2047, -2048]:
            f.append(('rv_addi(&mut e, %d, %d, %d)' % (d, a, imm), 'addi x%d, x%d, %d' % (d, a, imm)))
            f.append(('rv_xori(&mut e, %d, %d, %d)' % (d, a, imm), 'xori x%d, x%d, %d' % (d, a, imm)))
            f.append(('rv_andi(&mut e, %d, %d, %d)' % (d, a, imm), 'andi x%d, x%d, %d' % (d, a, imm)))
            f.append(('rv_sltiu(&mut e, %d, %d, %d)' % (d, a, imm), 'sltiu x%d, x%d, %d' % (d, a, imm)))
            f.append(('rv_addiw(&mut e, %d, %d, %d)' % (d, a, imm), 'addiw x%d, x%d, %d' % (d, a, imm)))
        for sh in [0, 1, 32, 63]:
            for op in ['slli', 'srli', 'srai']:
                f.append(('rv_%s(&mut e, %d, %d, %d)' % (op, d, a, sh), '%s x%d, x%d, %d' % (op, d, a, sh)))
        f.append(('rv_mv(&mut e, %d, %d)' % (d, a), 'addi x%d, x%d, 0' % (d, a)))
        f.append(('rv_lui(&mut e, %d, 74565)' % d, 'lui x%d, 74565' % d))
        f.append(('rv_jalr(&mut e, 1, %d, 0)' % a, 'jalr x1, 0(x%d)' % a))
        for imm in [0, 8, -8, 2040]:
            for size, l, s in [(8, 'ld', 'sd'), (4, 'lw', 'sw'), (2, 'lh', 'sh'), (1, 'lb', 'sb')]:
                f.append(('rv_load(&mut e, %d, %d, %d, %d, true)' % (d, a, imm, size), '%s x%d, %d(x%d)' % (l, d, imm, a)))
                if size < 8:
                    f.append(('rv_load(&mut e, %d, %d, %d, %d, false)' % (d, a, imm, size),
                              '%su x%d, %d(x%d)' % (l, d, imm, a)))
                f.append(('rv_store(&mut e, %d, %d, %d, %d)' % (b, a, imm, size), '%s x%d, %d(x%d)' % (s, b, imm, a)))
        f.append(('rv_amoadd_d(&mut e, %d, %d, %d)' % (d, b, a), 'amoadd.d x%d, x%d, (x%d)' % (d, b, a)))
        f.append(('rv_lr_d(&mut e, %d, %d)' % (d, a), 'lr.d x%d, (x%d)' % (d, a)))
        f.append(('rv_sc_d(&mut e, %d, %d, %d)' % (d, b, a), 'sc.d x%d, x%d, (x%d)' % (d, b, a)))
        f.append(('rv_b_off(&mut e, 0, %d, %d, 12)' % (a, b), 'beq x%d, x%d, .+12' % (a, b)))
        f.append(('rv_b_off(&mut e, 1, %d, %d, 0 - 12)' % (a, b), 'bne x%d, x%d, .-12' % (a, b)))
    for d, a, b in [(0, 1, 2), (3, 31, 7)]:
        for op in ['fadd_d', 'fsub_d', 'fmul_d', 'fdiv_d', 'fsgnj_d', 'fsgnjx_d']:
            f.append(('rv_%s(&mut e, %d, %d, %d)' % (op, d, a, b), '%s f%d, f%d, f%d' % (op.replace('_', '.'), d, a, b)))
        for op in ['feq_d', 'flt_d', 'fle_d']:
            f.append(('rv_%s(&mut e, 10, %d, %d)' % (op, a, b), '%s x10, f%d, f%d' % (op.replace('_', '.'), a, b)))
        f.append(('rv_fsqrt_d(&mut e, %d, %d)' % (d, a), 'fsqrt.d f%d, f%d' % (d, a)))
        f.append(('rv_fmv_dx(&mut e, %d, 10)' % d, 'fmv.d.x f%d, x10' % d))
        f.append(('rv_fmv_xd(&mut e, 10, %d)' % d, 'fmv.x.d x10, f%d' % d))
        f.append(('rv_fmv_wx(&mut e, %d, 10)' % d, 'fmv.w.x f%d, x10' % d))
        f.append(('rv_fmv_xw(&mut e, 10, %d)' % d, 'fmv.x.w x10, f%d' % d))
        f.append(('rv_fcvt_dl(&mut e, %d, 11)' % d, 'fcvt.d.l f%d, x11' % d))
        f.append(('rv_fcvt_ld(&mut e, 11, %d)' % d, 'fcvt.l.d x11, f%d, rtz' % d))
        f.append(('rv_fcvt_ds(&mut e, %d, %d)' % (d, a), 'fcvt.d.s f%d, f%d' % (d, a)))
        f.append(('rv_fcvt_sd(&mut e, %d, %d)' % (d, a), 'fcvt.s.d f%d, f%d' % (d, a)))
        for rm, name in [(1, 'rtz'), (2, 'rdn'), (3, 'rup'), (4, 'rmm')]:
            f.append(('rv_fcvt_ld_rm(&mut e, 12, %d, %d)' % (d, rm), 'fcvt.l.d x12, f%d, %s' % (d, name)))
    f += [('rv_ret(&mut e)', 'ret'), ('rv_ecall(&mut e)', 'ecall'), ('rv_ebreak(&mut e)', 'ebreak'),
          ('rv_fence(&mut e)', 'fence rw, rw')]
    return f


PROGRAM_HEAD = '''// Generated by scripts/enc-oracle.py: each line is "arch|word" for one form.
use core;
use vec;
use string;
use compiler::nat_enc;

fn hex(v: i64) -> str {
    var digits: str = "0123456789abcdef";
    var out: str = "";
    var i: i64 = 7;
    while i >= 0 {
        var d: i64 = (v >> (i * 4)) & 15;
        out = out + str_slice(digits, d, d + 1);
        i = i - 1;
    }
    return out;
}

fn word(e: &NatEnc) -> str {
    var w: i64 = e.code.get(0) | (e.code.get(1) << 8) | (e.code.get(2) << 16) | (e.code.get(3) << 24);
    var n: str = int_to_str(e.code.len() / 4);
    return hex(w) + " " + n;
}

fn main() -> i64 {
    var e: NatEnc = NatEnc_new();
'''


def elf_text(path):
    data = open(path, 'rb').read()
    shoff = struct.unpack_from('<Q', data, 0x28)[0]
    shentsize, shnum, shstrndx = struct.unpack_from('<HHH', data, 0x3A)
    def sh(i):
        return struct.unpack_from('<IIQQQQIIQQ', data, shoff + i * shentsize)
    strtab = sh(shstrndx)
    for i in range(shnum):
        s = sh(i)
        name_off = strtab[4] + s[0]
        name = data[name_off:data.index(b'\0', name_off)].decode()
        if name == '.text':
            return data[s[4]:s[4] + s[5]]
    raise SystemExit('no .text in ' + path)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--fid', default='release/fid.exe' if os.name == 'nt' else 'release/fid')
    ap.add_argument('--zig', default='zig')
    args = ap.parse_args()
    out = os.path.join('build', 'enc-oracle')
    os.makedirs(out, exist_ok=True)
    forms = [('a64', c, a) for c, a in a64_forms()] + [('rv', c, a) for c, a in rv_forms()]
    prog = [PROGRAM_HEAD]
    for arch, call, _ in forms:
        prog.append('    e = NatEnc_new(); %s; print("%s|" + word(&e) + "\\n");\n' % (call, arch))
    prog.append('    return 0;\n}\n')
    src = os.path.join(out, 'oracle.fd')
    open(src, 'w', newline='\n').write(''.join(prog))
    r = subprocess.run([args.fid, 'run', src], capture_output=True, text=True)
    lines = [l for l in r.stdout.splitlines() if '|' in l]
    if len(lines) != len(forms):
        sys.stderr.write(r.stdout[-2000:] + r.stderr[-2000:])
        raise SystemExit('the encoder program printed %d of %d words' % (len(lines), len(forms)))
    ours = {}
    for (arch, call, asm), line in zip(forms, lines):
        hexw, nwords = line.split('|')[1].split()
        ours.setdefault(arch, []).append((call, asm, int(hexw, 16), int(nwords)))
    bad = 0
    total = 0
    for arch, target, head in [('a64', 'aarch64-linux-musl', '.text\n'),
                               ('rv', 'riscv64-linux-musl', '.text\n.option norvc\n')]:
        s_path = os.path.join(out, arch + '.s')
        o_path = os.path.join(out, arch + '.o')
        with open(s_path, 'w', newline='\n') as fh:
            fh.write(head)
            for _, asm, _, _ in ours[arch]:
                fh.write(asm + '\n')
        cmd = [args.zig, 'cc', '-target', target, '-c', s_path, '-o', o_path]
        if arch == 'rv':
            cmd[4:4] = ['-march=rv64imafd']
        r2 = subprocess.run(cmd, capture_output=True, text=True)
        if r2.returncode != 0:
            sys.stderr.write(r2.stderr)
            raise SystemExit('the assembler refused ' + s_path)
        text = elf_text(o_path)
        for i, (call, asm, w, nwords) in enumerate(ours[arch]):
            total += 1
            theirs = struct.unpack_from('<I', text, 4 * i)[0] if 4 * i + 4 <= len(text) else -1
            if nwords != 1 or theirs != w:
                bad += 1
                print('MISMATCH %-4s %-40s ours %08x (%d words)  clang %08x   %s' % (arch, asm, w, nwords, theirs, call))
    print('%d forms, %d mismatches' % (total, bad))
    return bad


if __name__ == '__main__':
    sys.exit(min(main(), 125))
