#!/usr/bin/env python3
"""TG15 check: a macOS executable written by the machine layer binds the C
functions it imports through its own tables: LC_LOAD_DYLIB commands
(libSystem first) and dyld's bind opcodes in LC_DYLD_INFO_ONLY, one
pointer of __DATA per import (compiler/nat_enc.fd nat_macho_img,
nat_macho_import; compiler/image_writer.fd img_macho_bind_ords).

    fid emit tests/bootstrap/test_machine_ldyn_targets.fd --target aarch64-macos -o x
    python3 tests/nattools/macho_imports_check.py x atoi hypot ldexp

Reads a thin 64-bit Mach-O or each slice of a universal (fat) binary.
Prints, per slice, its CPU, the dylibs and each bind as
"<ordinal> <dylib> <symbol> <segment>+<offset>". Checks that
  - the header is MH_EXECUTE with a sane load-command area;
  - every bind's ordinal names an LC_LOAD_DYLIB, its segment is __DATA and
    its pointer lies inside that segment, 8-byte aligned;
  - no two binds share a pointer;
  - every name given on the command line (C names, without the leading
    underscore; default: none) is bound in every slice, and every path
    given (an argument holding "/" or ending in ".dylib") is one of its
    dylibs.
With `otool` (or `llvm-otool`) on PATH also runs `otool -l` on the file,
and with `dyld_info` runs `dyld_info -fixups` (they must parse it).
Exit 0 when every check holds."""
import shutil
import struct
import subprocess
import sys

CPU = {0x01000007: 'x86_64', 0x0100000C: 'arm64'}


def cstr(d, at):
    end = d.index(b'\0', at)
    return d[at:end].decode('latin-1')


def uleb(d, i):
    v = 0
    shift = 0
    while True:
        c = d[i]
        i += 1
        v |= (c & 0x7F) << shift
        shift += 7
        if not c & 0x80:
            return v, i


def sleb(d, i):
    v = 0
    shift = 0
    while True:
        c = d[i]
        i += 1
        v |= (c & 0x7F) << shift
        shift += 7
        if not c & 0x80:
            if c & 0x40:
                v -= 1 << shift
            return v, i


def check_slice(d, base, want, fails):
    magic, cpu, sub, ftype, ncmds, sizeofcmds = struct.unpack_from('<IIIIII', d, base)
    name = CPU.get(cpu, '%08x' % cpu)
    if magic != 0xFEEDFACF:
        fails.append('%s: not a 64-bit Mach-O (magic %08x)' % (name, magic))
        return
    if ftype != 2:
        fails.append('%s: file type %d, not MH_EXECUTE' % (name, ftype))
    if 32 + sizeofcmds > len(d) - base:
        fails.append('%s: load commands past the end' % name)
        return
    dylibs = []
    segs = []
    bind_off = bind_size = 0
    at = base + 32
    for _ in range(ncmds):
        cmd, size = struct.unpack_from('<II', d, at)
        if size < 8 or at + size > base + 32 + sizeofcmds:
            fails.append('%s: bad load command size %d' % (name, size))
            return
        if cmd == 0xC:
            off = struct.unpack_from('<I', d, at + 8)[0]
            dylibs.append(cstr(d, at + off))
        elif cmd == 0x19:
            segname = d[at + 8:at + 24].rstrip(b'\0').decode()
            vmaddr, vmsize, fileoff, filesize = struct.unpack_from('<QQQQ', d, at + 24)
            segs.append((segname, vmaddr, vmsize))
        elif cmd in (0x22, 0x80000022):
            bind_off, bind_size = struct.unpack_from('<II', d, at + 16)
        at += size
    print('%s: dylibs %s' % (name, ', '.join(dylibs)))
    if not dylibs or dylibs[0] != '/usr/lib/libSystem.B.dylib':
        fails.append('%s: libSystem is not the first dylib' % name)
    binds = []
    ordinal = 0
    sym = ''
    seg = 0
    off = 0
    i = base + bind_off
    end = i + bind_size
    while i < end:
        b = d[i]
        op, imm = b & 0xF0, b & 0x0F
        i += 1
        if op == 0x00:
            break
        elif op == 0x10:
            ordinal = imm
        elif op == 0x20:
            ordinal, i = uleb(d, i)
        elif op == 0x30:
            ordinal = -imm if imm else 0
        elif op == 0x40:
            sym = cstr(d, i)
            i += len(sym) + 1
        elif op == 0x50:
            pass
        elif op == 0x60:
            _, i = sleb(d, i)
        elif op == 0x70:
            seg = imm
            off, i = uleb(d, i)
        elif op == 0x80:
            v, i = uleb(d, i)
            off += v
        elif op == 0x90:
            binds.append((ordinal, sym, seg, off))
            off += 8
        else:
            fails.append('%s: bind opcode %02x not expected' % (name, b))
            break
    seen = set()
    bound = set()
    for ordinal, sym, seg, off in binds:
        lib = dylibs[ordinal - 1] if 1 <= ordinal <= len(dylibs) else '?'
        print('  %d %s %s %d+%d' % (ordinal, lib, sym, seg, off))
        if lib == '?':
            fails.append('%s: %s bound to dylib ordinal %d (%d dylibs)' % (name, sym, ordinal, len(dylibs)))
        if seg >= len(segs) or segs[seg][0] != '__DATA':
            fails.append('%s: %s bound in segment %d, not __DATA' % (name, sym, seg))
        elif off % 8 or off + 8 > segs[seg][2]:
            fails.append('%s: %s pointer at %d outside __DATA or unaligned' % (name, sym, off))
        if (seg, off) in seen:
            fails.append('%s: two binds at %d+%d' % (name, seg, off))
        seen.add((seg, off))
        bound.add(sym)
    for w in want:
        if '/' in w or w.endswith('.dylib'):
            # a dylib path (Git Bash on Windows turns /usr/lib/x into
            # C:/Program Files/Git/usr/lib/x: its tail still matches)
            w2 = w
            if not any(w2 == p or w2.endswith(p) or p.endswith(w2) for p in dylibs):
                fails.append('%s: %s is not loaded' % (name, w))
            continue
        if '_' + w not in bound and '_' + w + '$INODE64' not in bound:
            fails.append('%s: %s is not bound' % (name, w))


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return 2
    path = sys.argv[1]
    want = sys.argv[2:]
    d = open(path, 'rb').read()
    fails = []
    if len(d) >= 8 and struct.unpack_from('>I', d, 0)[0] == 0xCAFEBABE:
        n = struct.unpack_from('>I', d, 4)[0]
        for k in range(n):
            cpu, sub, off, size, align = struct.unpack_from('>IIIII', d, 8 + 20 * k)
            check_slice(d, off, want, fails)
    else:
        check_slice(d, 0, want, fails)
    for tool, args in (('otool', ['-l']), ('llvm-otool', ['-l']), ('dyld_info', ['-fixups'])):
        exe = shutil.which(tool)
        if exe:
            r = subprocess.run([exe] + args + [path], capture_output=True, text=True)
            if r.returncode != 0:
                fails.append('%s %s failed: %s' % (tool, ' '.join(args), r.stderr.strip()[:200]))
    for f in fails:
        print('FAIL: ' + f)
    if not fails:
        print('macho_imports_check: ok')
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
