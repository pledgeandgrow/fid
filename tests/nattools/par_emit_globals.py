#!/usr/bin/env python3
"""SP5 (docs/roadmap/llvm-roadmap.md): the parallel emission's worker
threads must not write shared state.

compiler/nat_ir.fd spawns ng_par_thread (FID_NAT_PAR=<n>); every function
it reaches runs on several threads at once. Owner counts and module
globals are not atomic, so none of them may

  - assign or change a module-level variable (`g_x = ...`, `g_x.push(..)`,
    `g_x.set(..)`, ...): what one function's emission keeps belongs in
    NatGen (sw_pro, ldfwd_n, dbg_mpos, wipe_on, ...), and tables are made
    before the threads start (ng_emit_tables);
  - insert into or clear a map of the generator other than the job's own
    pl_keys (`HashMap_insert(g.<field>, ..)`): the generator's maps are raw
    handles shared by every thread (ng_par_gen).

The type checker's race check (TypeChecker_verify_no_races) sees only the
spawns of a program's main file, so this scan stands in for it. Like the
race check it follows calls by name, over compiler/*.fd and the stdlib
core modules (compiler/runtime.fd's allocator, which no compiler module
imports, is left out).

    python3 tests/nattools/par_emit_globals.py [repo root]

Prints each finding and exits 1 if there is one; else prints how many
functions it checked and "ok". `--self-test` also checks that the scan
finds a write planted into a copy of the sources (so a scan that finds
nothing is not a scan that cannot find anything).
"""
import os
import re
import sys

ROOTS = ['ng_par_thread']
SKIP_FILES = {'runtime.fd'}
# maps of the generator the emission may write: each job's own
OWN_MAPS = {'pl_keys'}


def load(root, plant=None):
    files = [os.path.join(root, 'stdlib', f) for f in ('core.fd', 'vec.fd', 'string.fd')]
    cdir = os.path.join(root, 'compiler')
    files += [os.path.join(cdir, f) for f in sorted(os.listdir(cdir))
              if f.endswith('.fd') and f not in SKIP_FILES]
    funcs = {}
    globs = set()
    for f in files:
        if not os.path.exists(f):
            continue
        src = open(f, encoding='utf-8', errors='replace').read()
        if plant and f.endswith(os.path.join('compiler', 'nat_ir.fd')):
            src = plant(src)
        lines = src.split('\n')
        for line in lines:
            if line.startswith((' ', '\t')):
                continue
            for m in re.finditer(r'(?:^|;\s*)(?:pub\s+)?var\s+(\w+)\s*:', line):
                globs.add(m.group(1))
        i = 0
        while i < len(lines):
            line = lines[i]
            m = None
            if not line.startswith((' ', '\t')):
                m = re.match(r'(?:@\w+(?:\([^)]*\))?\s+)*(?:pub\s+)?fn\s+(\w+)\s*\(', line) \
                    or re.search(r'[;}]\s*(?:pub\s+)?fn\s+(\w+)\s*\(', line)
            if m:
                j = i + 1
                if not line.rstrip().endswith('}'):
                    while j < len(lines) and not lines[j].startswith('}'):
                        j += 1
                name = m.group(1)
                if name not in funcs:
                    funcs[name] = (f, i + 1, lines[i:j + 1])
                i = j + 1
            else:
                i += 1
    return funcs, globs


def reach(funcs):
    seen = set()
    work = list(ROOTS)
    while work:
        n = work.pop()
        if n in seen or n not in funcs:
            continue
        seen.add(n)
        body = re.sub(r'//[^\n]*', '', '\n'.join(funcs[n][2]))
        for c in re.findall(r'\b(\w+)\s*\(', body):
            if c in funcs and c not in seen:
                work.append(c)
            # a method call `v.push(..)` runs Vec_push, `m.len()` ...
            for pre in ('Vec_', 'HashMap_', 'Str_'):
                if pre + c in funcs and pre + c not in seen:
                    work.append(pre + c)
    return seen


def scan(root, plant=None):
    funcs, globs = load(root, plant)
    missing = [r for r in ROOTS if r not in funcs]
    if missing:
        return None, ['root function not found: ' + ', '.join(missing)]
    seen = reach(funcs)
    out = []
    for n in sorted(seen):
        f, first, body = funcs[n]
        for k, line in enumerate(body):
            s = re.sub(r'//.*', '', line)
            m = re.search(r'(?<![\.\w])(\w+)\s*(=(?!=)|\.(?:push|set|pop|clear|truncate|insert|remove)\()', s)
            if m and m.group(1) in globs and not re.search(r'\bvar\s+' + re.escape(m.group(1)) + r'\b', s):
                out.append('%s:%d: %s writes the module variable %s: %s' % (
                    os.path.relpath(f, root), first + k, n, m.group(1), s.strip()[:100]))
            for mm in re.finditer(r'HashMap_(?:insert|remove|clear|set)\w*\(\s*(\w+)\.(\w+)\s*[,)]', s):
                if mm.group(1) in ('g', 'w', 'gen') and mm.group(2) not in OWN_MAPS:
                    out.append('%s:%d: %s changes the shared map %s.%s: %s' % (
                        os.path.relpath(f, root), first + k, n, mm.group(1), mm.group(2), s.strip()[:100]))
            for mm in re.finditer(r'HashMap_(?:insert|remove|clear|set)\w*\(\s*(g_\w+)\s*[,)]', s):
                out.append('%s:%d: %s changes the module map %s: %s' % (
                    os.path.relpath(f, root), first + k, n, mm.group(1), s.strip()[:100]))
    return len(seen), out


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    root = args[0] if args else '.'
    n, out = scan(root)
    if n is None:
        print('FAIL ' + out[0])
        return 1
    for o in out:
        print('FAIL ' + o)
    if '--self-test' in sys.argv:
        def plant(src):
            key = 'fn ng_mir_emit_w(g: & mut NatGen, rec: & NatFnRec, al: & NatAlloc) {\n'
            if key not in src:
                return src
            return src.replace(key, key + '    g_nat_ldfwd_n = 0;\n    HashMap_insert(g.fn_labels, "x", 1);\n', 1)
        _, planted = scan(root, plant)
        found_var = any('g_nat_ldfwd_n' in p for p in planted)
        found_map = any('g.fn_labels' in p for p in planted)
        if found_var and found_map:
            print('ok   self-test: a planted global write and map insert are found')
        else:
            print('FAIL self-test: the planted write was not found (variable %s, map %s)' % (found_var, found_map))
            out.append('self-test')
    if out:
        print('%d finding(s) over %d functions reached from %s' % (len(out), n, ', '.join(ROOTS)))
        return 1
    print('ok   %d functions reached from %s write no module variable and no shared map' % (n, ', '.join(ROOTS)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
