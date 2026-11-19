#!/usr/bin/env python3
"""stage0-literals-check.py -- struct literals the stage-0 compiler cannot parse.

release-roadmap A4, next to scripts/stage0-names-check.sh. The frozen Rust
compiler (fidesc) builds stage 1 from compiler/main.fd. Its parser takes
`Name { ... }` as a struct literal only when Name is a struct it already
knows (src/parser.rs is_struct_name), and its import loader
(src/compiler.rs build_program_with_runtime) knows:
  - core's structs and the root file's,
  - the structs of every module loaded before, and
  - a module's own structs, but only when a first "pre-scan" parse of that
    module succeeds; that parse is also what finds the module's imports so
    they are loaded first. A pre-scan that fails (a literal of a type not
    known yet) loads nothing first, and the full parse fails the same way.
So a module may build a struct by literal only if the type is declared
earlier in the same file, or loaded before the module is first reached
(main.fd's `use` order, then each module's own `use` order), or declared
later in the same file with the pre-scan otherwise clean.

This script replays that loader on the sources (regular expressions, not a
parser) and prints each literal that would fail as `file:line: Name {`.

Usage: python3 tests/tooling/stage0-literals-check.py [root.fd]
       (default compiler/main.fd; run from the repository root)
       SELFTEST=1: check the replay on HEAD-shaped samples.
Exit 1 when a literal would fail.
"""
import os
import re
import sys


def strip(src):
    """Comments and string/char contents blanked, line breaks kept."""
    out = []
    i = 0
    n = len(src)
    while i < n:
        c = src[i]
        if c == '/' and i + 1 < n and src[i + 1] == '/':
            while i < n and src[i] != '\n':
                out.append(' ')
                i += 1
            continue
        if c == '"':
            out.append('"')
            i += 1
            while i < n and src[i] != '"':
                if src[i] == '\\' and i + 1 < n:
                    out.append(' ')
                    i += 1
                out.append('\n' if src[i] == '\n' else ' ')
                i += 1
            out.append('"')
            i += 1
            continue
        if c == "'" and i + 2 < n:
            if src[i + 1] == '\\' and i + 3 < n and src[i + 3] == "'":
                out.append('    ')
                i += 4
                continue
            if src[i + 2] == "'":
                out.append('   ')
                i += 3
                continue
        out.append(c)
        i += 1
    return ''.join(out)


DECL = re.compile(r'\bstruct\s+([A-Za-z_]\w*)')
LIT = re.compile(r'([A-Za-z_]\w*)\s*\{')
USE = re.compile(r'\buse\s+([A-Za-z_]\w*(?:::[A-Za-z_]\w*)*)\s*;')
# what may stand before `Name {` when it is not a struct literal
NOT_LIT = re.compile(
    r'(\bif|\bwhile|\bin|==|!=|<=|>=|<|>|&&|\|\||\bstruct|\benum|\bimpl|\btrait|'
    r'\bfor|\bfn|\bmatch|\bunion|\bcomponent|->|::|\bmod|\bmodule|\belse|\bloop|'
    r'\bunsafe|\bcomptime|\bsandbox|\bregion|\btxn|\bparallel|\bdefer|\bdo|\bgen|'
    r'\basync|\bselect)\s*$')

_cache = {}


def info(path):
    if path in _cache:
        return _cache[path]
    with open(path, encoding='utf-8', errors='replace') as fh:
        src = strip(fh.read())
    decls = [(m.start(), m.group(1)) for m in DECL.finditer(src)]
    lits = []
    for m in LIT.finditer(src):
        name = m.group(1)
        # struct names are CamelCase; ALL_CAPS before `{` is a constant
        if not name[0].isupper() or name.upper() == name:
            continue
        before = src[max(0, m.start() - 40):m.start()]
        if NOT_LIT.search(before):
            continue
        lits.append((m.start(), name, src.count('\n', 0, m.start()) + 1))
    uses = [m.group(1) for m in USE.finditer(src)]
    _cache[path] = (decls, lits, uses)
    return _cache[path]


def resolve(imp, base):
    p = imp.replace('::', '/')
    for c in (os.path.join(base, p + '.fd'), os.path.join(base, p, 'mod.fd'),
              os.path.join('stdlib', p + '.fd'), os.path.join('stdlib', p, 'mod.fd'),
              p + '.fd', os.path.join(p, 'mod.fd')):
        if os.path.isfile(c):
            return c
    return None


def unknown(path, names):
    decls, lits, _ = info(path)
    bad = []
    for pos, name, line in lits:
        if name in names:
            continue
        if any(dp < pos and dn == name for dp, dn in decls):
            continue
        bad.append((name, line))
    return bad


def replay(root):
    """[(file, line, name)] of literals the loader would fail on."""
    names = set(n for _, n in info(os.path.join('stdlib', 'core.fd'))[0])
    rdecls, _, ruses = info(root)
    names |= set(n for _, n in rdecls)
    base = os.path.dirname(root) or '.'
    stack = list(reversed(ruses))
    loaded = set()
    fails = []
    steps = 0
    while stack:
        steps += 1
        if steps > 1000000:
            fails.append((root, 0, '<import cycle>'))
            break
        imp = stack.pop()
        if imp == 'core' or imp in loaded:
            continue
        f = resolve(imp, base)
        if f is None:
            continue
        decls, _, uses = info(f)
        if not unknown(f, names):
            names |= set(n for _, n in decls)
            unl = [u for u in uses if u not in loaded and u != 'core']
            if unl:
                stack.append(imp)
                stack.extend(unl)
                continue
        for name, line in unknown(f, names):
            fails.append((f, line, name))
        loaded.add(imp)
        names |= set(n for _, n in decls)
        stack.extend(u for u in uses if u not in loaded)
    return fails


def selftest():
    import tempfile
    ok = True
    with tempfile.TemporaryDirectory() as t:
        os.makedirs(os.path.join(t, 'compiler'))
        os.makedirs(os.path.join(t, 'stdlib'))

        def w(rel, text):
            with open(os.path.join(t, rel), 'w') as fh:
                fh.write(text)
        w('stdlib/core.fd', 'pub struct Core { pub a: i64 }\n')
        w('compiler/m.fd', 'module compiler::m;\npub struct Mod { pub a: i64 }\n')
        # uses Mod before main reaches compiler::m: fails
        w('compiler/a.fd', 'module compiler::a; use compiler::m;\n'
          'fn f() -> Mod { return Mod { a: 1 }; }\n')
        # forward reference to its own struct: fails
        w('compiler/b.fd', 'module compiler::b;\n'
          'fn g() -> i64 { var x: Bee = Bee { a: 1 }; return x.a; }\n'
          'pub struct Bee { pub a: i64 }\n')
        # declared first, then built (also nested in a field); Core from
        # core: fine
        w('compiler/c.fd', 'module compiler::c;\npub struct Cee { pub a: i64 }\n'
          'pub struct Two { pub c: Cee }\n'
          'fn t() -> Two { return Two { c: Cee { a: 3 } }; }\n'
          'fn h() -> i64 { var c: Cee = Cee { a: 1 }; var k: Core = Core { a: 2 }; '
          'if c.a < LIMIT { return 0; } return k.a; }\n')
        w('compiler/main1.fd', 'module compiler::main1; use compiler::a; use compiler::b; '
          'use compiler::c;\nfn main() -> i64 { return 0; }\n')
        w('compiler/main2.fd', 'module compiler::main2; use compiler::m; use compiler::a; '
          'use compiler::c;\nfn main() -> i64 { return 0; }\n')
        cwd = os.getcwd()
        os.chdir(t)
        try:
            _cache.clear()
            got = sorted((f.replace('\\', '/').lstrip('./'), n) for f, _, n in replay('compiler/main1.fd'))
            want = [('compiler/a.fd', 'Mod'), ('compiler/b.fd', 'Bee')]
            if got != want:
                print('selftest: main1 expected %s, got %s' % (want, got))
                ok = False
            _cache.clear()
            got = replay('compiler/main2.fd')
            if got:
                print('selftest: main2 expected no failures, got %s' % got)
                ok = False
        finally:
            os.chdir(cwd)
            _cache.clear()
    print('stage0-literals-check selftest: %s' % ('PASS' if ok else 'FAIL'))
    return 0 if ok else 1


def main():
    if os.environ.get('SELFTEST') == '1':
        return selftest()
    root = sys.argv[1] if len(sys.argv) > 1 else os.path.join('compiler', 'main.fd')
    if not os.path.isfile(root):
        print('stage0-literals-check: no %s' % root)
        return 1
    fails = replay(root)
    for f, line, name in fails:
        print('%s:%d: %s {' % (f.replace('\\', '/'), line, name))
    if fails:
        print('stage0-literals-check: FAIL (%d struct literals the stage-0 compiler '
              '(fidesc) cannot parse: build the value through a function declared '
              'after the struct, or import the declaring module earlier)' % len(fails))
        return 1
    print('stage0-literals-check: PASS (%s)' % root)
    return 0


if __name__ == '__main__':
    sys.exit(main())
