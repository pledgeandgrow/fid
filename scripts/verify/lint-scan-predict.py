#!/usr/bin/env python3
"""lint-scan-predict.py: the L7E source scan of `fid lint`, outside fid.

compiler/compiler.fd `lint_source_scan` (with lx_tokens, lx_lint_fn,
lx_lint_uses and their helpers) adds three kinds of warnings to `fid lint`:
unused variables and parameters, names that break the naming conventions,
and unsorted `use` lines. This script is a line-for-line copy of that scan,
so that the warnings a compiler with the scan will give can be listed with
any compiler, or none (roadmap L7E, "unused variables"). It is a predictor:
the compiler is the reference, and tests/tooling/lint-unused-check.sh
compares the two on tests/bootstrap/test_unused_var_lint.fd and on the
files given to it, and tests/tooling/lint-style-check.sh on the style
fixtures.

One rule is ahead of the compiler (Nov 18): a `cap` (Fid's trait keyword;
the compiler's scan tests `trait`, which is not a keyword) is checked for
UpperCamelCase like a struct; the compiler hunk is named in
docs/verification/l7e-style-lints-2026-11-18.md.

Usage:
  python3 scripts/verify/lint-scan-predict.py FILE.fd...
      the scan's warnings of each file, as `fid lint` prints them
      ("warning: line N: ..."; the "Fix:" line of each is left out unless
      --fix-lines)
  python3 scripts/verify/lint-scan-predict.py --gate [--baseline B] [DIR]
      the stdlib/*.fd modules (DIR, default stdlib) that scripts/lint-check.sh
      will fail on for the scan's warnings alone: a module the baseline
      (scripts/lint-check.baseline, recorded with a compiler that has the
      scan) records clean that gets any, one with warnings whose scan
      warnings alone exceed its count, a new module with any. The other
      lints (unused functions and imports, unwrap) can only add to these.
      Modules recorded `unchecked` are left out. Exit 1 when one would fail.
  python3 scripts/verify/lint-scan-predict.py --examples
      the five examples of scripts/examples-check.sh, which must stay at
      "No lint warnings" (exit 1 when one would not)
  --kinds unused,naming,imports   only these kinds (default all)
  --count                         one `file|count` line per file
"""
import os
import sys

# --- lx_tokens --------------------------------------------------------------


def is_alpha(c):
    return (65 <= c <= 90) or (97 <= c <= 122) or c == 95


def is_alnum(c):
    return is_alpha(c) or (48 <= c <= 57)


class Tok:
    __slots__ = ("text", "kind", "line")

    def __init__(self, text, kind, line):
        self.text = text
        self.kind = kind
        self.line = line


def tokens(src):
    """lx_tokens: identifiers (0), punctuation bytes (1), string contents (2),
    numbers (3), each with its line. `src` is bytes."""
    out = []
    n = len(src)
    line = 1
    i = 0
    while i < n:
        c = src[i]
        if c == 10:
            line += 1
            i += 1
        elif c in (32, 9, 13):
            i += 1
        elif c == 47 and i + 1 < n and src[i + 1] == 47:
            while i < n and src[i] != 10:
                i += 1
        elif c == 47 and i + 1 < n and src[i + 1] == 42:
            i += 2
            while i < n and not (src[i] == 42 and i + 1 < n and src[i + 1] == 47):
                if src[i] == 10:
                    line += 1
                i += 1
            i += 2
        elif c == 34:
            start_line = line
            j = i + 1
            while j < n and src[j] != 34:
                if src[j] == 92:
                    j += 1
                if j < n and src[j] == 10:
                    line += 1
                j += 1
            end = min(j, n)
            out.append(Tok(src[i + 1:end].decode("utf-8", "replace"), 2, start_line))
            i = j + 1
        elif is_alpha(c):
            j = i
            while j < n and is_alnum(src[j]):
                j += 1
            out.append(Tok(src[i:j].decode("ascii"), 0, line))
            i = j
        elif 48 <= c <= 57:
            j = i
            while j < n and (is_alnum(src[j]) or (src[j] == 46 and j + 1 < n and 48 <= src[j + 1] <= 57)):
                j += 1
            out.append(Tok(src[i:j].decode("ascii"), 3, line))
            i = j
        else:
            out.append(Tok(src[i:i + 1].decode("latin-1"), 1, line))
            i += 1
    return out


# --- helpers ----------------------------------------------------------------


def lx_is(t, i, text):
    if i < 0 or i >= len(t):
        return False
    return t[i].kind <= 1 and t[i].text == text


def ident_at(t, i):
    return 0 <= i < len(t) and t[i].kind == 0


def word_in(s, name):
    """lx_word_in on str (the compiler's is on bytes; names are ASCII)."""
    sb = s.encode("utf-8")
    nb = name.encode("ascii")
    frm = 0
    nl = len(nb)
    sl = len(sb)
    while frm + nl <= sl:
        at = sb.find(nb, frm)
        if at < 0:
            return False
        ok = True
        if at > 0 and is_alnum(sb[at - 1]):
            ok = False
        if at + nl < sl and is_alnum(sb[at + nl]):
            ok = False
        if ok:
            return True
        frm = at + 1
    return False


def not_a_read(t, i):
    if lx_is(t, i - 1, ".") and not lx_is(t, i - 2, "."):
        return True
    k = i - 1
    if lx_is(t, k, "mut"):
        k -= 1
    if lx_is(t, k, "var") or lx_is(t, k, "val") or lx_is(t, k, "let") or lx_is(t, k, "for"):
        return True
    if lx_is(t, i + 1, ":") and not lx_is(t, i + 2, ":") and not lx_is(t, i - 1, ":"):
        return True
    return False


def reads(t, b0, b1, name):
    count = 0
    for i in range(b0, b1):
        x = t[i]
        if x.kind == 0 and x.text == name and not not_a_read(t, i):
            count += 1
        elif x.kind == 2 and word_in(x.text, name):
            count += 1
    return count


def close(t, open_, o, c):
    depth = 0
    i = open_
    while i < len(t):
        if lx_is(t, i, o):
            depth += 1
        elif lx_is(t, i, c):
            depth -= 1
            if depth == 0:
                return i + 1
        i += 1
    return len(t)


def is_snake(s):
    return not any("A" <= ch <= "Z" for ch in s)


def is_camel(s):
    if not s:
        return False
    return "A" <= s[0] <= "Z" and "_" not in s


def is_upper_snake(s):
    return not any("a" <= ch <= "z" for ch in s)


def fn_name_ok(s):
    if is_snake(s):
        return True
    us = s.find("_")
    if us <= 0:
        return False
    head = s[:us]
    tail = s[us + 1:]
    return is_camel(head) and len(tail) > 0 and is_snake(tail)


def to_snake(s):
    out = ""
    for i, ch in enumerate(s):
        if "A" <= ch <= "Z":
            if i > 0:
                pc = s[i - 1]
                prev_lower = ("a" <= pc <= "z") or ("0" <= pc <= "9")
                next_lower = i + 1 < len(s) and "a" <= s[i + 1] <= "z"
                if prev_lower or ("A" <= pc <= "Z" and next_lower):
                    out += "_"
            out += ch.lower()
        else:
            out += ch
    return out


def to_camel(s):
    out = ""
    up = True
    for ch in s:
        if ch == "_":
            up = True
        elif up and "a" <= ch <= "z":
            out += ch.upper()
            up = False
        else:
            out += ch
            up = False
    return out


def upper(s):
    return "".join(ch.upper() if "a" <= ch <= "z" else ch for ch in s)


def open_of(t, cl):
    depth = 0
    i = cl
    while i >= 0:
        if lx_is(t, i, ")"):
            depth += 1
        elif lx_is(t, i, "("):
            depth -= 1
            if depth == 0:
                return i
        i -= 1
    return 0


def annotations_before(t, i):
    out = ""
    k = i - 1
    if lx_is(t, k, "pub"):
        k -= 1
    steps = 0
    while k >= 0 and steps < 16:
        if lx_is(t, k, ")"):
            k = open_of(t, k) - 1
        if ident_at(t, k) and lx_is(t, k - 1, "@"):
            out += " " + t[k].text
            k -= 2
        else:
            return out
        steps += 1
    return out


# --- the warnings -----------------------------------------------------------
# Each warning is (kind, text, fix): kind is unused / naming / imports; text
# the first line `fid lint` prints after "warning: "; fix the "Fix:" line.


def lint_fn(t, name, p0, p1, b0, b1, check_params, out):
    dnames = []
    i = b0
    while i < b1:
        if lx_is(t, i, "var") or lx_is(t, i, "val") or lx_is(t, i, "let") or lx_is(t, i, "for"):
            k = i + 1
            if lx_is(t, k, "mut"):
                k += 1
            if ident_at(t, k) and (not lx_is(t, i, "for") or lx_is(t, k + 1, "in")):
                dnames.append((t[k].text, t[k].line))
        i += 1
    pnames = []
    depth = 0
    j = p0
    while j < p1:
        if lx_is(t, j, "(") or lx_is(t, j, "<") or lx_is(t, j, "["):
            depth += 1
        elif lx_is(t, j, ")") or (lx_is(t, j, ">") and not lx_is(t, j - 1, "-")) or lx_is(t, j, "]"):
            depth -= 1
        elif depth == 1 and ident_at(t, j) and lx_is(t, j + 1, ":") and not lx_is(t, j + 2, ":") and (
                lx_is(t, j - 1, "(") or lx_is(t, j - 1, ",") or lx_is(t, j - 1, "mut")):
            pnames.append((t[j].text, t[j].line))
        j += 1
    for dn, dl in dnames:
        if not dn.startswith("_"):
            if reads(t, b0, b1, dn) == 0:
                out.append(("unused", "line %d: unused variable '%s' in '%s' — it is declared but never read." % (dl, dn, name),
                            "Fix: remove it, or name it _%s if it must stay." % dn))
            if not is_snake(dn):
                out.append(("naming", "line %d: variable '%s' in '%s' should be snake_case: %s" % (dl, dn, name, to_snake(dn)), None))
    for pn, pl in pnames:
        if check_params and pn != "self" and not pn.startswith("_"):
            if reads(t, b0, b1, pn) == 0:
                out.append(("unused", "line %d: unused parameter '%s' of '%s' — it is never read." % (pl, pn, name),
                            "Fix: remove it, or name it _%s if the signature needs it." % pn))
            if not is_snake(pn):
                out.append(("naming", "line %d: parameter '%s' of '%s' should be snake_case: %s" % (pl, pn, name, to_snake(pn)), None))


def use_key(path):
    if path == "core":
        return "0"
    if path == "vec":
        return "1"
    if path == "string":
        return "2"
    return "3" + path


def lint_uses(t, out):
    depth = 0
    prev_path = ""
    prev_end_line = -10
    prev_end_tok = -10
    i = 0
    while i < len(t):
        if lx_is(t, i, "{"):
            depth += 1
        elif lx_is(t, i, "}"):
            depth -= 1
        elif depth == 0 and lx_is(t, i, "use"):
            start = i
            if lx_is(t, i - 1, "pub"):
                start = i - 1
            path = ""
            k = i + 1
            while k < len(t) and not lx_is(t, k, ";") and not lx_is(t, k, "{"):
                path += t[k].text
                k += 1
            st = t[start]
            same_group = prev_end_tok == start - 1 and st.line - prev_end_line <= 1
            # str_compare: byte order
            if same_group and use_key(path).encode() < use_key(prev_path).encode():
                out.append(("imports", "line %d: imports are not sorted: 'use %s' should come before 'use %s'." % (st.line, path, prev_path),
                            "Fix: sort the use lines of this group by path (core, vec and string first)."))
            prev_path = path
            if k < len(t) and lx_is(t, k, ";"):
                prev_end_line = t[k].line
                prev_end_tok = k
            else:
                prev_end_tok = -10
            i = k
        i += 1


def scan(src):
    """lint_source_scan."""
    out = []
    t = tokens(src)
    lint_uses(t, out)
    tstarts = []
    tends = []
    ti = 0
    while ti < len(t):
        if lx_is(t, ti, "impl"):
            k2 = ti + 1
            is_trait = False
            while k2 < len(t) and not lx_is(t, k2, "{") and not lx_is(t, k2, ";"):
                if lx_is(t, k2, "for"):
                    is_trait = True
                k2 += 1
            if is_trait and k2 < len(t) and lx_is(t, k2, "{"):
                tstarts.append(k2)
                tends.append(close(t, k2, "{", "}"))
        ti += 1

    def in_ranges(i):
        return any(s <= i < e for s, e in zip(tstarts, tends))

    i = 0
    while i < len(t):
        if lx_is(t, i, "fn") and ident_at(t, i + 1) and not lx_is(t, i - 1, "."):
            nt = t[i + 1]
            name = nt.text
            k = i + 2
            if lx_is(t, k, "<"):
                gd = 0
                while k < len(t):
                    if lx_is(t, k, "<"):
                        gd += 1
                    elif lx_is(t, k, ">") and not lx_is(t, k - 1, "-"):
                        gd -= 1
                        if gd == 0:
                            k += 1
                            break
                    k += 1
            if lx_is(t, k, "("):
                p0 = k
                p1 = close(t, k, "(", ")")
                b = p1
                while b < len(t) and not lx_is(t, b, "{") and not lx_is(t, b, ";"):
                    b += 1
                if b < len(t) and lx_is(t, b, "{"):
                    b1 = close(t, b, "{", "}")
                    const_fn = p1 == p0 + 2 and is_upper_snake(name)
                    if not fn_name_ok(name) and not name.startswith("_") and not const_fn:
                        out.append(("naming", "line %d: function '%s' should be snake_case: %s" % (nt.line, name, to_snake(name)), None))
                    lint_fn(t, name, p0, p1, b + 1, b1 - 1, not in_ranges(i), out)
        # `cap` is Fid's trait keyword (`trait` is kept as the compiler has
        # it); a `cap` item is `cap Name {`, `cap Name: Super {` or
        # `cap Name<T> {` (not a `.cap` field)
        elif (lx_is(t, i, "struct") or lx_is(t, i, "enum") or lx_is(t, i, "trait")
              or (lx_is(t, i, "cap") and not lx_is(t, i - 1, ".")
                  and (lx_is(t, i + 2, "{") or lx_is(t, i + 2, ":") or lx_is(t, i + 2, "<")))) and ident_at(t, i + 1):
            tt = t[i + 1]
            kw = t[i]
            ann = annotations_before(t, i)
            if not is_camel(tt.text) and "repr" not in ann and "packed" not in ann and not tt.text.startswith("_"):
                out.append(("naming", "line %d: %s '%s' should be UpperCamelCase: %s" % (tt.line, kw.text, tt.text, to_camel(tt.text)), None))
        elif lx_is(t, i, "const") and ident_at(t, i + 1) and lx_is(t, i + 2, ":"):
            ct = t[i + 1]
            if not is_upper_snake(ct.text):
                out.append(("naming", "line %d: constant '%s' should be SCREAMING_SNAKE_CASE: %s" % (ct.line, ct.text, upper(to_snake(ct.text))), None))
        i += 1
    return out


def scan_file(path, kinds):
    with open(path, "rb") as f:
        src = f.read()
    return [w for w in scan(src) if w[0] in kinds]


EXAMPLES = ["lexer", "parser", "json_parser", "codegen", "mini_compiler"]


def read_baseline(path):
    base = {}
    if not os.path.exists(path):
        return base
    with open(path, encoding="utf-8") as f:
        for ln in f:
            ln = ln.strip()
            if not ln or "|" not in ln:
                continue
            m, state, count = (ln.split("|") + ["0"])[:3]
            base[m] = (state, int(count or 0))
    return base


def main(argv):
    # fid prints the warnings' em dash as UTF-8 bytes; so does this
    try:
        sys.stdout.reconfigure(encoding="utf-8", newline="\n")
    except (AttributeError, ValueError):
        pass
    kinds = {"unused", "naming", "imports"}
    fix_lines = False
    count_only = False
    mode = "files"
    baseline = "scripts/lint-check.baseline"
    rest = []
    i = 0
    while i < len(argv):
        a = argv[i]
        if a == "--kinds":
            kinds = set(argv[i + 1].split(","))
            i += 1
        elif a == "--fix-lines":
            fix_lines = True
        elif a == "--count":
            count_only = True
        elif a == "--gate":
            mode = "gate"
        elif a == "--examples":
            mode = "examples"
        elif a == "--baseline":
            baseline = argv[i + 1]
            i += 1
        elif a in ("-h", "--help"):
            print(__doc__)
            return 0
        else:
            rest.append(a)
        i += 1

    if mode == "files":
        if not rest:
            print(__doc__)
            return 2
        for p in rest:
            ws = scan_file(p, kinds)
            if count_only:
                print("%s|%d" % (p, len(ws)))
                continue
            for _k, text, fix in ws:
                print("warning: " + text)
                if fix_lines and fix:
                    print("    " + fix)
        return 0

    if mode == "examples":
        bad = 0
        for e in EXAMPLES:
            p = os.path.join("examples", e + ".fd")
            ws = scan_file(p, kinds)
            if ws:
                bad = 1
                print("WOULD FAIL  examples/%s.fd: %d scan warning(s)" % (e, len(ws)))
                for _k, text, _f in ws:
                    print("    " + text)
            else:
                print("ok          examples/%s.fd" % e)
        print("lint-scan-predict examples: %s" % ("FAIL" if bad else "PASS"))
        return bad

    # gate
    d = rest[0] if rest else "stdlib"
    base = read_baseline(baseline)
    bad = 0
    n_mod = 0
    n_warn = 0
    for name in sorted(os.listdir(d)):
        if not name.endswith(".fd"):
            continue
        m = name[:-3]
        ws = scan_file(os.path.join(d, name), kinds)
        n_mod += 1
        if not ws:
            continue
        n_warn += 1
        state, count = base.get(m, ("new", 0))
        if state == "unchecked":
            continue
        # the baseline's count already holds the scan's warnings (it was
        # recorded with a compiler that has the scan): a module fails for
        # sure only when the scan alone gives more than it allows
        if state == "warnings" and len(ws) <= count:
            continue
        if state == "clean":
            why = "was clean"
        elif state == "new":
            why = "new module"
        else:
            why = "baseline allows %d" % count
        bad = 1
        print("WOULD FAIL  %s (%s): %d scan warning(s)" % (m, why, len(ws)))
        for _k, text, _f in ws:
            print("    " + text)
    print("lint-scan-predict gate: %d of %d modules get scan warnings; %s" % (n_warn, n_mod, "FAIL" if bad else "PASS"))
    return bad


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
