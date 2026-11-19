#!/usr/bin/env python3
"""tg15_win64_stub_check.py — TG15 (docs/roadmap/llvm-roadmap.md): the
Windows x64 stubs through which `--nat` executables call C functions of
DLLs (compiler/nat_enc.fd nat_pe_win64_stub) against clang's assembler.

Runs tests/nattools/tg15_win64_stub_unit.fd with `fid run`, which prints
each stub's bytes and the position of its rip-relative IAT word. For every
signature this script writes, independently, the instructions the stub
must be:

    push rbp; mov rbp, rsp; and rsp, -16; sub rsp, 32 + 8 * stack args
    (rounded to 16); each stack argument j >= 4 from [rbp + 16 + 8(j-4)]
    to [rsp + 32 + 8(j-4)] through r11; then positions 3..0: a float
    argument to xmm j (movq / movd) and, as every argument, to rcx, rdx,
    r8, r9 from rdi, rsi, rdx, rcx; mov rax, [rip + slot]; call rax; the
    result to rdi (movq / movd from xmm0, movsxd / movsx / movzx / mov
    from rax); mov rsp, rbp; pop rbp; ret

assembles them with `zig cc -c` and compares every byte (the IAT word's
four displacement bytes are masked in both). It also checks the DLL lines.

The Apple arm64 stubs of macOS executables (nat_macho_a64_stub) are
checked the same way (aarch64 assembly; the ADRP + ADD immediates of the
GOT slot's address masked): stp x29, x30, [sp, #-16]!; mov x29, sp; the
variadic arguments (after `.` in the signature) to 8-byte stack slots
(an f32 promoted to double); the fixed ones by AAPCS64; adrp/add/ldr/blr
x16; mov sp, x29; ldp; the result extended in x0; ret.

Usage: python3 tests/nattools/tg15_win64_stub_check.py [--fid release/fid.exe] [--zig zig]
Exit 0 and "tg15_win64_stub_check: ok (N Windows x64 stubs, M Apple arm64
stubs)" when all agree.
"""
import argparse
import os
import struct
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
UNIT = os.path.join('tests', 'nattools', 'tg15_win64_stub_unit.fd')

DLLS = {
    '': 'msvcrt.dll', 'c': 'msvcrt.dll', 'm': 'ucrtbase.dll',
    'kernel32': 'kernel32.dll', 'user32': 'user32.dll', 'foo.dll': 'foo.dll',
    'GL': 'opengl32.dll',
}

SRC = ['rdi', 'rsi', 'rdx', 'rcx']
DST = ['rcx', 'rdx', 'r8', 'r9']
SRC32 = ['edi', 'esi', 'edx', 'ecx']


def stub_asm(sig):
    gt = sig.index('>')
    args, res = sig[:gt], sig[gt + 1:] or 'i'
    # a variadic call's `.` changes nothing on Windows x64 (floats already
    # go to their integer register too)
    args = args.replace('.', '')
    n = len(args)
    nstack = max(0, n - 4)
    frame = (32 + 8 * nstack + 15) & ~15
    a = ['push rbp', 'mov rbp, rsp', 'and rsp, -16', 'sub rsp, %d' % frame]
    for j in range(4, n):
        a.append('mov r11, qword ptr [rbp + %d]' % (16 + 8 * (j - 4)))
        a.append('mov qword ptr [rsp + %d], r11' % (32 + 8 * (j - 4)))
    for j in range(min(n, 4) - 1, -1, -1):
        c = args[j]
        if c == 'd':
            a.append('movq xmm%d, %s' % (j, SRC[j]))
        elif c == 's':
            a.append('movd xmm%d, %s' % (j, SRC32[j]))
        if DST[j] != SRC[j]:
            a.append('mov %s, %s' % (DST[j], SRC[j]))
    a.append('mov rax, qword ptr [rip + slot]')
    a.append('call rax')
    a.append({
        'd': 'movq rdi, xmm0',
        's': 'movd edi, xmm0',
        'w': 'movsxd rdi, eax',
        'W': 'mov edi, eax',
        'h': 'movsx rdi, ax',
        'H': 'movzx rdi, ax',
        'b': 'movsx rdi, al',
        'B': 'movzx rdi, al',
    }.get(res, 'mov rdi, rax'))
    a += ['mov rsp, rbp', 'pop rbp', 'ret']
    return a


def a64_stub_asm(sig):
    """The Apple arm64 stub (nat_macho_a64_stub) as instructions."""
    gt = sig.index('>')
    args, res = sig[:gt], sig[gt + 1:] or 'i'
    if '.' in args:
        fixed, var = args.split('.')
    else:
        fixed, var = args, ''
    a = ['stp x29, x30, [sp, #-16]!', 'mov x29, sp']
    area = (8 * len(var) + 15) & ~15
    if area:
        a.append('sub sp, sp, #%d' % area)

    def src(j):
        if j < 8:
            return 'x%d' % j, 'w%d' % j
        a.append('ldr x16, [x29, #%d]' % (16 + 8 * (j - 8)))
        return 'x16', 'w16'

    for v, c in enumerate(var):
        x, w = src(len(fixed) + v)
        if c == 's':
            a += ['fmov s16, %s' % w, 'fcvt d16, s16', 'fmov x17, d16']
            x = 'x17'
        a.append('str %s, [sp, #%d]' % (x, 8 * v))
    ii = fi = 0
    for k, c in enumerate(fixed):
        x, w = src(k)
        if c == 'd':
            a.append('fmov d%d, %s' % (fi, x))
            fi += 1
        elif c == 's':
            a.append('fmov s%d, %s' % (fi, w))
            fi += 1
        else:
            if 'x%d' % ii != x:
                a.append('mov x%d, %s' % (ii, x))
            ii += 1
    a += ['adrp x16, slot', 'add x16, x16, :lo12:slot', 'ldr x16, [x16]', 'blr x16',
          'mov sp, x29', 'ldp x29, x30, [sp], #16']
    r = {
        'd': 'fmov x0, d0',
        's': 'fmov w0, s0',
        'w': 'sxtw x0, w0',
        'W': 'ubfx x0, x0, #0, #32',
        'h': 'sxth x0, w0',
        'H': 'ubfx x0, x0, #0, #16',
        'b': 'sxtb x0, w0',
        'B': 'ubfx x0, x0, #0, #8',
    }.get(res)
    if r:
        a.append(r)
    a.append('ret')
    return a


def a64_mask(buf, pos):
    """Zero the immediates of the ADRP + ADD pair at `pos` (the slot's
    address, which the code generator resolves; clang leaves a relocation)."""
    if pos + 8 > len(buf):
        return
    w1 = struct.unpack_from('<I', buf, pos)[0] & ~((3 << 29) | (0x7FFFF << 5))
    w2 = struct.unpack_from('<I', buf, pos + 4)[0] & ~(0xFFF << 10)
    struct.pack_into('<II', buf, pos, w1, w2)


def elf_text(path):
    b = open(path, 'rb').read()
    shoff = struct.unpack_from('<Q', b, 0x28)[0]
    shentsize, shnum, shstrndx = struct.unpack_from('<HHH', b, 0x3A)
    secs = []
    for i in range(shnum):
        o = shoff + i * shentsize
        name, typ, flags, addr, off, size = struct.unpack_from('<IIQQQQ', b, o)
        secs.append((name, off, size))
    stro = secs[shstrndx][1]
    for name, off, size in secs:
        nm = b[stro + name:b.index(b'\0', stro + name)]
        if nm == b'.text':
            return b[off:off + size]
    raise SystemExit('no .text in ' + path)


def assemble(zig, lines, tmp, k, a64=False):
    s = os.path.join(tmp, 's%d.s' % k)
    o = os.path.join(tmp, 's%d.o' % k)
    with open(s, 'w') as f:
        f.write('.text\n' if a64 else '.intel_syntax noprefix\n.text\n')
        for ln in lines:
            f.write('    ' + ln + '\n')
        f.write('slot:\n    .quad 0\n')
    target = 'aarch64-linux-musl' if a64 else 'x86_64-linux-musl'
    r = subprocess.run([zig, 'cc', '-target', target, '-c', s, '-o', o],
                       capture_output=True, text=True)
    if r.returncode != 0:
        raise SystemExit('zig cc failed:\n' + r.stderr + '\n' + '\n'.join(lines))
    return elf_text(o)[:-8]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--fid', default=os.environ.get('FID', 'release/fid.exe'))
    ap.add_argument('--zig', default='zig')
    args = ap.parse_args()
    r = subprocess.run([args.fid, 'run', UNIT], capture_output=True, text=True, cwd=ROOT)
    out = [ln for ln in r.stdout.splitlines() if ln.strip()]
    if r.returncode != 0 or not out:
        print(r.stderr[-3000:])
        raise SystemExit('tg15_win64_stub_check: the unit did not run')
    bad = 0
    dll_seen = 0
    stubs = {}
    fixes = {}
    a64s = {}
    a64fixes = {}
    for ln in out:
        p = ln.split(' ')
        if p[0] == 'dll':
            lib, dll = (p[1], p[2]) if len(p) == 3 else ('', p[-1])
            dll_seen += 1
            if DLLS.get(lib) != dll:
                print('MISMATCH dll %r: %s, expected %s' % (lib, dll, DLLS.get(lib)))
                bad += 1
        elif p[0] == 'stub':
            stubs[p[1]] = bytes.fromhex(p[2])
        elif p[0] == 'fix':
            fixes.setdefault(p[1], []).append(int(p[2]))
        elif p[0] == 'a64':
            a64s[p[1]] = bytes.fromhex(p[2])
        elif p[0] == 'a64fix':
            a64fixes.setdefault(p[1], []).append(int(p[2]))
    if dll_seen != len(DLLS):
        print('expected %d dll lines, got %d' % (len(DLLS), dll_seen))
        bad += 1
    with tempfile.TemporaryDirectory() as tmp:
        for k, (sig, ours) in enumerate(sorted(stubs.items())):
            asm = stub_asm(sig)
            theirs = assemble(args.zig, asm, tmp, k)
            fx = fixes.get(sig, [])
            if len(fx) != 1:
                print('MISMATCH %s: %d fixups, expected 1 (the IAT word)' % (sig, len(fx)))
                bad += 1
                continue
            ours_m = bytearray(ours)
            theirs_m = bytearray(theirs)
            for buf in (ours_m, theirs_m):
                if fx[0] + 4 <= len(buf):
                    buf[fx[0]:fx[0] + 4] = b'\0\0\0\0'
            if bytes(ours_m) != bytes(theirs_m):
                print('MISMATCH %s\n  ours  %s\n  clang %s\n  %s' % (sig, ours.hex(), theirs.hex(), '; '.join(asm)))
                bad += 1
        for k, (sig, ours) in enumerate(sorted(a64s.items())):
            asm = a64_stub_asm(sig)
            theirs = assemble(args.zig, asm, tmp, 1000 + k, a64=True)
            fx = a64fixes.get(sig, [])
            if len(fx) != 1:
                print('MISMATCH a64 %s: %d fixups, expected 1 (the GOT slot)' % (sig, len(fx)))
                bad += 1
                continue
            ours_m = bytearray(ours)
            theirs_m = bytearray(theirs)
            a64_mask(ours_m, fx[0])
            a64_mask(theirs_m, fx[0])
            if bytes(ours_m) != bytes(theirs_m):
                print('MISMATCH a64 %s\n  ours  %s\n  clang %s\n  %s' % (sig, ours.hex(), theirs.hex(), '; '.join(asm)))
                bad += 1
    if bad or not stubs or not a64s:
        raise SystemExit('tg15_win64_stub_check: %d mismatches' % bad)
    print('tg15_win64_stub_check: ok (%d Windows x64 stubs, %d Apple arm64 stubs)' % (len(stubs), len(a64s)))


if __name__ == '__main__':
    main()
