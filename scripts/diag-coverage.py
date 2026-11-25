#!/usr/bin/env python3
"""Every compiler error has a code, a Fix line and a test (complaint CE4,
competitive TL2).

Lists the error messages of the self-hosted compiler (compiler/*.fd: the
string literals of each `errors.push(...)` and of every literal that starts
like an error, "Type error", "borrow error", ...), and for each one:

  code   the ErrCode classify_error (compiler/compiler.fd) gives its text
         ("Custom" when no rule matches: it prints as plain `error:`)
  fix    whether the message carries a `Fix:` (or `Hint:`) line
  test   whether a test expects it: a `// expect-error: <text>` line in
         tests/**/*.fd, a `//! error: <text>` line in tests/conformance, or
         a `has "<label>" <log> "<text>"` / `grep -qF -- "<text>"` check of
         a tooling test (tests/**/*.sh: errors that need a project or a
         backend, ark.toml pacts, `emit --bare`), whose text contains a
         literal piece of the message (or is contained in one), or is a
         piece of the message with its values filled in (`no method
         'frobnicate' on Rc` for "no method '" + m + "' on Rc", 60% of the
         message's own text or 24 characters of it matched; the class
         prefix "Type error (line N): " does not count)

The message's text includes a local it starts with (`head + "is generic"`
after `var head: str = "Event error: @event '" + ...`). A message that is
only a class prefix ("Parse error at " + a helper's text) is not counted,
nor are the self-checks of the compiler's tables (INTERNAL_MODULES).

scripts/diag-tests-check.py runs the tests and checks that each prints the
text it expects (this script only reads them).

  python3 scripts/diag-coverage.py                 # summary + ratchet check
  python3 scripts/diag-coverage.py --list untested # the messages no test expects
  python3 scripts/diag-coverage.py --list nofix    # the messages with no Fix line
  python3 scripts/diag-coverage.py --list custom   # the messages with no code
  python3 scripts/diag-coverage.py --save          # write the ratchet baseline

The check is a ratchet (scripts/diag-coverage-baseline.txt): it fails when
fewer messages are tested, or more lack a Fix line or a code, than in the
baseline. Adding a message that has no test therefore fails until a test
for it (tests/bootstrap/test_<name>_fail.fd with `// expect-error:`) is
written, or the baseline is saved again on purpose.
"""
import argparse
import glob
import os
import re
import sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
BASELINE = os.path.join(ROOT, "scripts", "diag-coverage-baseline.txt")

ERROR_PREFIXES = (
    "Type error", "type error", "borrow error", "Borrow error", "Parse error",
    "Effect error", "Capability error", "Linear error", "Init error",
    "Sandbox error", "Conservation error", "Gas error", "Safety error",
    "Escape error", "escape error", "Region error", "Secret error",
    "Firmware error", "Send error", "Generic error", "Lint error",
)

# Modules whose `errors.push` is a self-check of the compiler's own tables,
# never an error about a program: disasm.fd checks the rows of its
# instruction-encoding table (g_ef_errors, "row N `tmpl`: no field {x}").
INTERNAL_MODULES = ("disasm.fd",)

LIT = re.compile(r'"((?:[^"\\]|\\.)*)"')


def literals(line):
    return [m.group(1) for m in LIT.finditer(line)]


def unescape(s):
    return s.replace("\\n", "\n").replace('\\"', '"').replace("\\t", "\t").replace("\\\\", "\\")


def message_pieces(lits):
    """The literal pieces of the message part (before any Fix/Hint line)."""
    text = unescape("\x00".join(lits))
    for cut in ("\n    Fix:", "\nFix:", "\n    Hint:", "\nHint:", "Fix: ", "Hint: "):
        i = text.find(cut)
        if i >= 0:
            text = text[:i]
    pieces = []
    for p in text.split("\x00"):
        p = p.strip()
        # the line prefix "Type error (line " / "): " is not the message
        p = re.sub(r"^[A-Za-z ]*error \(line\s*$", "", p).strip()
        p = re.sub(r"^\)\s*:\s*", "", p).strip()
        p = p.strip(" :.'`")
        # a bare class prefix ("Parse error at", "Type error (in") is in
        # every message of its class: it says nothing about this one (a
        # message made only of it gets its text elsewhere, a helper's
        # return value, and is not counted here)
        if re.fullmatch(r"[A-Za-z -]*error( at| \(in| \(line)?\s*\(?", p):
            continue
        if len(p) >= 8 and re.search(r"[a-z]{3}", p):
            pieces.append(p)
    return pieces


# CE4: what compiler.fd classify_error gives ErrCode::OtherCheck (E078)
OTHER_CHECK_PREFIXES = (
    "Event error", "Redundancy error", "Contract error", "Thread safety error",
    "Partition error", "Transaction error", "Serverless error",
    "Differential privacy error", "Comptime error", "Shard error",
    "Quantization error", "Linear type error", "Checkpoint error",
    "Budget error", "Visibility error", "Privacy error", "Unknown annotation",
    "Misplaced annotation", "Differentiability error", "DP error",
    "Adapter error", "Hermetic error", "Allocator error",
    "KV cache region error", "Injection error",
)


def classify(text):
    """A Python mirror of classify_error in compiler/compiler.fd."""
    n = text
    m = re.match(r"Type error \(line [^)]*\): ", n)
    if m:
        n = "Type error: " + n[m.end():]
    rules = [
        (lambda: n.startswith("Type error: undefined variable"), "UndefinedVariable"),
        (lambda: n.startswith("Type error: undefined function"), "UndefinedFunction"),
        (lambda: n.startswith("Type error: undefined struct") or n.startswith("Type error: unknown struct"), "UndefinedStruct"),
        (lambda: "can reach the end of its body without returning" in n, "MissingReturn"),
        (lambda: n.startswith("Type error: struct") and "has no field" in n, "UndefinedField"),
        (lambda: "cannot assign" in n, "TypeMismatch"),
        (lambda: n.startswith("Type error: if condition must be bool"), "IfConditionNotBool"),
        (lambda: "expects" in n and "args" in n, "ArgCountMismatch"),
        (lambda: "missing return" in n, "MissingReturn"),
        (lambda: n.startswith("Init error"), "InitBeforeAssign"),
        (lambda: n.startswith("Linear error") or "linear" in n, "NotLinear"),
        (lambda: n.startswith("Capability error"), "CapabilityDenied"),
        (lambda: n.startswith("Sandbox error"), "SandboxViolation"),
        (lambda: n.startswith("Conservation error"), "ConservationViolation"),
        (lambda: n.startswith("Gas error"), "GasExceeded"),
        (lambda: n.startswith("Effect error"), "EffectMismatch"),
        (lambda: n.startswith("Parse error"), "ExpectedToken"),
        (lambda: lexer_class(n) is not None, None),
        (lambda: n.startswith("empty char literal") or n.startswith("unexpected character"), "UnexpectedChar"),
        (lambda: n.startswith("borrow error") or n.startswith("Borrow error"), "BorrowConflict"),
        (lambda: n.startswith("Panic:"), "NoMain"),
        (lambda: n.startswith("Duplicate definition") or "conflicting implementations" in n or n.startswith("Ambiguous name:"), "DuplicateDefinition"),
        (lambda: n.startswith("Taint error"), "TaintViolation"),
        (lambda: "non-exhaustive match" in n, "NonExhaustiveMatch"),
        (lambda: n.startswith("Race error"), "DataRace"),
        (lambda: n.startswith("Deadlock error"), "Deadlock"),
        (lambda: n.startswith("Concurrency error"), "UnjoinedTask"),
        (lambda: n.startswith("Constant-time error") or n.startswith("Security error (constant_time)"), "ConstantTimeViolation"),
        (lambda: n.startswith("Realtime error") or n.startswith("Real-time error"), "RealtimeViolation"),
        # TL2: escape.fd and secrecy.fd (E058, E059)
        (lambda: n.startswith("Reference error"), "ReferenceEscape"),
        (lambda: n.startswith("Security error (@secret)") or n.startswith("Security error (@sensitive)"), "SecrecyViolation"),
        (lambda: n.startswith("Safety error") or n.startswith("Determinism error"), "SafetyViolation"),
        # CE4: the other check classes (compiler.fd ErrCode::OtherCheck, E078)
        (lambda: n.startswith(OTHER_CHECK_PREFIXES), "OtherCheck"),
        (lambda: n.startswith("Type error"), "OtherTypeError"),
        # TL2: inline assembly (pe_ir.fd), and checks of the compiler's own
        # output (ir.fd's verifier, the C split's units): E078, E999
        (lambda: n.startswith("asm! in "), "OtherCheck"),
        (lambda: n.startswith("function '") and ("' has no blocks" in n or "': block " in n
                                                 or "' is defined twice (units " in n), "InternalError"),
    ]
    for cond, code in rules:
        if cond():
            return code if code is not None else lexer_class(n)
    return "Custom"


def lexer_class(n):
    """classify_error's lexer rule: "Error:L:C: <message>" and the lexer's
    bare messages, by message (None when the rule does not apply)."""
    if not (n.startswith("Error:") or n.startswith("unterminated ")
            or n.startswith("unknown escape") or n.startswith("invalid hex digit")):
        return None
    if "unterminated f-string" in n:
        return "UnterminatedFString"
    if ("unterminated string" in n or "unterminated raw string" in n
            or "unterminated char literal" in n):
        return "UnterminatedString"
    if "escape" in n or "\\x expects" in n or "to close \\u{" in n:
        return "UnknownEscape"
    return "UnexpectedChar"


# TL2: helpers that prefix a message with its class (escape.fd, secrecy.fd,
# borrowck.fd); a call `helper(self, ..., "text" + ...)` is a message
HELPER = re.compile(r"\b(esc_error|sec_error|violation)\(self\s*,")
HELPER_CLASS = {
    "esc_error": "Reference error (line 1): ",
    "sec_error": "Security error (@secret) (line 1): ",
    "violation": "borrow error (line 1): ",
}


def odd_quotes(s):
    """Does s leave a string literal open (an odd number of unescaped `"`)?"""
    n = 0
    k = 0
    while k < len(s):
        c = s[k]
        if c == "\\":
            k += 2
            continue
        if c == '"':
            n += 1
        k += 1
    return n % 2 == 1


FIX_CALL = re.compile(r"\b\w+_fix\((?:[^()]|\([^()]*\))*\)")


def strip_fix_calls(s):
    """s without the arguments of its *_fix(...) calls (the candidates of
    `tc_method_fix(method, "push pop ...")` are no part of the message)."""
    return FIX_CALL.sub("_fix()", s)


def fix_elsewhere(lines, i, stmt):
    """TL2: a Fix line the message gets from outside its own literals:
      - from a helper named *_fix (`+ tc_method_fix(m, ...)`), which returns
        a Fix line on every path;
      - from a local it ends with (`+ hint`, `+ fix`) that a literal with a
        Fix line is given above it (`var hint: str = "\\n    Fix: ...";`);
      - in a local it is built in (`var e: str = "Type error ..."`) that is
        completed with a Fix line below it (`e = e + "\\n    Fix: ..."`)."""
    if re.search(r"\b\w+_fix\(", stmt):
        return True
    for name in re.findall(r"\+\s*([a-z_][a-z0-9_]*)\s*(?=[+);])", stmt):
        assign = re.compile(r"(?:\b(?:var|val)\s+" + re.escape(name) + r"\s*:\s*str\s*=|^\s*" + re.escape(name) + r"\s*=)(.*)$")
        for k in range(i - 1, max(-1, i - 40), -1):
            am = assign.search(lines[k])
            if am and any(("Fix:" in l or "Hint:" in l) for l in literals(am.group(1))):
                return True
    dm = re.match(r"\s*(?:var|val)\s+([a-z_][a-z0-9_]*)\s*:\s*str\s*=", lines[i])
    if dm:
        grow = re.compile(r"^\s*" + re.escape(dm.group(1)) + r"\s*=\s*" + re.escape(dm.group(1)) + r"\s*\+(.*)$")
        for k in range(i + 1, min(len(lines), i + 12)):
            gm = grow.search(lines[k])
            if gm and any(("Fix:" in l or "Hint:" in l) for l in literals(gm.group(1))):
                return True
    return False


def collect_messages():
    msgs = []
    for path in sorted(glob.glob(os.path.join(ROOT, "compiler", "*.fd"))):
        mod = os.path.basename(path)
        if mod in INTERNAL_MODULES:
            continue
        with open(path, encoding="utf-8", errors="replace") as f:
            lines = f.read().split("\n")
        for i, line in enumerate(lines):
            s = line.strip()
            if s.startswith("//"):
                continue
            lits = literals(strip_fix_calls(line))
            if not lits:
                continue
            is_push = "errors.push(" in line or "errors_push(" in line
            # TL2: the checkers that build their messages in a helper
            # (escape.fd esc_error, secrecy.fd sec_error, borrowck.fd
            # violation): each call's text is a message of the helper's class
            helper = HELPER.search(line)
            if helper and helper.group(1) == "sec_error":
                lits = lits[1:]  # the first literal is the kind ("@secret")
            starts = any(l.startswith(ERROR_PREFIXES) for l in lits)
            if not (is_push or starts or helper):
                continue
            # a test of an error's text (classify_error, enrich_errors) is
            # not a message
            if not is_push and re.search(r"\b(str_starts_with|str_contains|str_find|starts_with|contains|fix_between)\(", line):
                continue
            # a message may go on over the next lines ("..." +\n "...")
            # (TL2: or in a literal that holds a line break, an odd number
            # of quotes on the line)
            j = i
            stmt = line
            while (line.rstrip().endswith("+") or odd_quotes(stmt)) and j + 1 < len(lines) and j - i < 8:
                j += 1
                line = lines[j]
                stmt = stmt + "\n" + line
            if j > i:
                lits = literals(strip_fix_calls(stmt))
                if helper and helper.group(1) == "sec_error":
                    lits = lits[1:]
            # a message that starts with a local made a few lines above
            # (`var head: str = "Event error: @event '" + func.name + "' ";`
            # then `errors.push(head + "is generic. ...")`): its text too
            hm = re.search(r"errors(?:\.push|_push)\(\s*([a-z_][a-z0-9_]*)\s*\+", lines[i])
            if hm:
                decl = re.compile(r"\b(?:var|val)\s+" + re.escape(hm.group(1)) + r"\s*:\s*str\s*=\s*(.*)$")
                for k in range(i - 1, max(-1, i - 80), -1):
                    dm = decl.search(lines[k])
                    if dm:
                        lits = literals(dm.group(1)) + lits
                        break
            full = unescape("".join(lits))
            pieces = message_pieces(lits)
            if not pieces:
                continue
            fix = ("Fix:" in full) or ("Hint:" in full) or fix_elsewhere(lines, i, stmt)
            # borrowck.fd violation() gives a Fix line to a message that has none
            if helper and helper.group(1) == "violation":
                fix = True
            cls = full
            if helper:
                cls = HELPER_CLASS[helper.group(1)] + full
            msgs.append({
                "where": f"{mod}:{i + 1}",
                "text": cls,
                "pieces": pieces,
                "templates": templates_of(lits),
                "fix": fix,
                # lex_error(line, col, msg) prints "Error:L:C: msg"
                "code": classify(("Error:1:1: " + full) if "lex_error(" in stmt else cls),
            })
    return msgs


def collect_expectations():
    exp = []
    pats = [os.path.join(ROOT, "tests", "**", "*.fd")]
    for pat in pats:
        for path in glob.glob(pat, recursive=True):
            try:
                with open(path, encoding="utf-8", errors="replace") as f:
                    head = f.read()
            except OSError:
                continue
            for m in re.finditer(r"^\s*//!?\s*(?:expect-error|error)\s*:\s*(.+)$", head, re.M):
                t = m.group(1).strip()
                if len(t) >= 6:
                    exp.append((t, os.path.relpath(path, ROOT).replace("\\", "/")))
    # a tooling test (tests/**/*.sh) that checks the text of an error in
    # the output of a run it sets up (a project with ark.toml, several
    # files): `has "<label>" <log> "<text>"` (the helper of
    # tests/tooling/cd1-direct-caps-check.sh and others) or
    # `grep -qF -- "<text>" <log>`
    sh_pats = (
        re.compile(r"""^\s*has\s+(?:"[^"]*"|'[^']*')\s+\S+\s+(?:"([^"]+)"|'([^']+)')\s*$"""),
        re.compile(r"""grep\s+-qF\s+(?:--\s+)?(?:"([^"$]+)"|'([^']+)')"""),
    )
    for path in glob.glob(os.path.join(ROOT, "tests", "**", "*.sh"), recursive=True):
        try:
            with open(path, encoding="utf-8", errors="replace") as f:
                lines = f.read().split("\n")
        except OSError:
            continue
        rel = os.path.relpath(path, ROOT).replace("\\", "/")
        for line in lines:
            for p in sh_pats:
                for m in p.finditer(line):
                    t = (m.group(1) or m.group(2) or "").strip()
                    if len(t) >= 12:
                        exp.append((t, rel))
    return exp


def norm(s):
    return re.sub(r"\s+", " ", s).strip().lower()


WILD = "\x00"


def templates_of(lits):
    """The message as a template: its literal text (normalized like an
    expectation) with WILD where a value goes in, cut before its Fix/Hint
    line, without its class prefix ("Type error (line N): "). A value may
    come before the first literal and after the last (`... on type " +
    type_to_str(t)`), so the template starts and ends with WILD."""
    text = unescape(WILD.join(lits))
    for cut in ("\n    Fix:", "\nFix:", "\n    Hint:", "\nHint:", "Fix: ", "Hint: "):
        i = text.find(cut)
        if i >= 0:
            text = text[:i]
    t = norm(text)
    t = re.sub(r"\s*\x00\s*", lambda m: (" " if m.group(0).startswith(" ") else "") + WILD
               + (" " if m.group(0).endswith(" ") else ""), t)
    # the class prefix ("Type error (line N): ") is in every message of its
    # class: only the text after it tells this message from the others
    return [WILD + strip_class(t) + WILD]


CLASS = re.compile(r"^[a-z -]*error(?: \((?:line|in) [^)]*\))?\s*:\s*")


def strip_class(t):
    return CLASS.sub("", t, count=1)


def template_match(t, tpl):
    """The number of the template's literal characters an expectation t
    matches when t is a piece of some instance of the template (a value,
    WILD, stands for any text), or -1. t may start and end anywhere in the
    template."""
    n, m = len(t), len(tpl)
    NEG = -1
    # best[j]: the most literal characters matched with t[:i] ending just
    # before template position j (j = m: at its end)
    best = [0] * (m + 1)  # i = 0: t's start may be anywhere
    for i in range(n):
        nxt = [NEG] * (m + 1)
        c = t[i]
        for j in range(m + 1):
            b = best[j]
            if b < 0:
                continue
            if j < m:
                if tpl[j] == WILD:
                    # the value takes t[i] (and stays open), or ends here;
                    # a value does not hold the quote or bracket that
                    # closes it (`'` in "no method '<m>' on Rc")
                    if j + 1 < m and tpl[j + 1] == c and c in "'`\"),:]":
                        pass
                    elif b > nxt[j]:
                        nxt[j] = b
                elif tpl[j] == c:
                    if b + 1 > nxt[j + 1]:
                        nxt[j + 1] = b + 1
        # a value may also end without taking more of t: move past WILDs
        for j in range(m):
            if tpl[j] == WILD and nxt[j] > nxt[j + 1]:
                nxt[j + 1] = nxt[j]
        best = nxt
        if max(best) < 0:
            return -1
    return max(best)


_SHINGLES = {}


def shingles(s, k=5):
    """The k-character runs of s that hold no WILD (cached)."""
    got = _SHINGLES.get(s)
    if got is None:
        got = {s[i:i + k] for i in range(len(s) - k + 1) if WILD not in s[i:i + k]}
        _SHINGLES[s] = got
    return got


def tested(msg, exps):
    lit_len = max(len(tpl.replace(WILD, "")) for tpl in msg["templates"])
    for t, where in exps:
        nt = norm(t)
        for p in msg["pieces"]:
            np_ = norm(p)
            if np_ in nt or (len(nt) >= 12 and nt in np_):
                return where
        # the expectation is a piece of an instance of the message: values
        # filled in (`no method 'frobnicate' on Rc`), with enough of the
        # message's own text matched to tell it from its neighbours
        nt = strip_class(nt)
        if len(nt) < 12:
            continue
        sh_t = shingles(nt)
        for tpl in msg["templates"]:
            # a match of 10 literal characters shares a run of 5 with t
            if not (sh_t & shingles(tpl)):
                continue
            k = template_match(nt, tpl)
            if k >= 24 or (k >= 10 and k * 10 >= lit_len * 6):
                return where
    return None


def self_test():
    """Fixed cases of the matching: an expectation counts for a message when
    it is a piece of an instance of it, and not for its neighbours."""
    def msg(*lits):
        lits = list(lits)
        return {"pieces": message_pieces(lits), "templates": templates_of(lits)}
    rc = msg("Type error: no method '", "' on Rc")
    arc = msg("Type error: no method '", "' on Arc")
    ty = msg("Type error: cannot call method '", "' on type ")
    va = msg("Type error (line ", "): a variadic argument expects '", "', got '", "' in '", "'")
    sb = msg("Capability error (line ", "): ", "() cannot be called inside sandbox: the sandbox's revocation is managed by the compiler")
    ev = msg("Event error: @event '", "' ", "is generic.\n    An event's signature (its topic) is fixed.\n    Fix: declare one event per payload type.")
    cases = [
        (rc, "no method 'frobnicate' on Rc", True),
        (arc, "no method 'frobnicate' on Rc", False),
        (rc, "no method 'frobnicate' on Arc", False),
        (ty, "cannot call method 'flip' on type bool", True),
        (va, "a variadic argument expects 'i64', got 'str' in 'main'", True),
        (va, "expects 'dyn Animal', got 'Dog'", False),
        (sb, "p_sandbox_exit() cannot be called inside sandbox", True),
        (ev, "Event error: @event 'Moved' is generic. (line 7)", True),
        # the class prefix alone says nothing about the message
        (va, "Type error (line 9): arithmetic requires numeric types. Left: bool", False),
    ]
    bad = 0
    for m, t, want in cases:
        got = tested(m, [(t, "case")]) is not None
        if got != want:
            bad += 1
            print(f"FAIL: {t!r} {'matched' if got else 'did not match'} {m['templates']}")
    # a message whose text starts with a local is read with the local's text
    lines = ['    var head: str = "Event error: @event \'" + func.name + "\' ";',
             '    if func.is_async {',
             '        self.errors.push(head + "is async.\\n    Fix: remove `async`.");']
    hm = re.search(r"errors(?:\.push|_push)\(\s*([a-z_][a-z0-9_]*)\s*\+", lines[2])
    if not (hm and hm.group(1) == "head"):
        bad += 1
        print("FAIL: the local before a message is not found")
    print("self-test: " + ("ok" if bad == 0 else f"{bad} failed") + f" ({len(cases) + 1} cases)")
    return 1 if bad else 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--list", choices=["untested", "nofix", "custom", "all"])
    ap.add_argument("--save", action="store_true")
    ap.add_argument("--self-test", action="store_true",
                    help="check the matching of expectations to messages on fixed cases")
    a = ap.parse_args()
    if a.self_test:
        return self_test()

    msgs = collect_messages()
    exps = collect_expectations()
    for m in msgs:
        m["test"] = tested(m, exps)
    total = len(msgs)
    n_test = sum(1 for m in msgs if m["test"])
    n_fix = sum(1 for m in msgs if m["fix"])
    n_code = sum(1 for m in msgs if m["code"] != "Custom")
    print(f"{total} error messages in compiler/*.fd, {len(exps)} expected errors in tests/")
    print(f"  with a test:     {n_test} ({100 * n_test // max(total, 1)}%)")
    print(f"  with a Fix line: {n_fix} ({100 * n_fix // max(total, 1)}%)")
    print(f"  with a code:     {n_code} ({100 * n_code // max(total, 1)}%)")
    by_mod = {}
    for m in msgs:
        mod = m["where"].split(":")[0]
        t = by_mod.setdefault(mod, [0, 0])
        t[0] += 1
        t[1] += 1 if m["test"] else 0
    print("  per module (tested/messages): " + ", ".join(
        f"{k} {v[1]}/{v[0]}" for k, v in sorted(by_mod.items(), key=lambda kv: -kv[1][0])))

    if a.list:
        for m in msgs:
            show = (a.list == "all" or (a.list == "untested" and not m["test"])
                    or (a.list == "nofix" and not m["fix"])
                    or (a.list == "custom" and m["code"] == "Custom"))
            if show:
                first = m["text"].split("\n")[0]
                print(f"{m['where']}: [{m['code']}{', fix' if m['fix'] else ''}"
                      f"{', test ' + m['test'] if m['test'] else ''}] {first[:140]}")

    if a.save:
        with open(BASELINE, "w", newline="\n") as f:
            f.write(f"messages {total}\ntested {n_test}\nfix {n_fix}\ncode {n_code}\n")
        print(f"saved {os.path.relpath(BASELINE, ROOT)}")
        return 0
    if not os.path.exists(BASELINE):
        print("no baseline (scripts/diag-coverage-baseline.txt): run with --save")
        return 0
    base = {}
    with open(BASELINE) as f:
        for line in f:
            k, _, v = line.partition(" ")
            if v.strip().isdigit():
                base[k] = int(v)
    bad = []
    # untested / unfixed / uncoded messages may not grow
    if total - n_test > base.get("messages", 0) - base.get("tested", 0):
        bad.append(f"untested messages {total - n_test} > baseline "
                   f"{base.get('messages', 0) - base.get('tested', 0)}")
    if total - n_fix > base.get("messages", 0) - base.get("fix", 0):
        bad.append(f"messages without a Fix line {total - n_fix} > baseline "
                   f"{base.get('messages', 0) - base.get('fix', 0)}")
    if total - n_code > base.get("messages", 0) - base.get("code", 0):
        bad.append(f"messages without a code {total - n_code} > baseline "
                   f"{base.get('messages', 0) - base.get('code', 0)}")
    if bad:
        for b in bad:
            print("FAIL: " + b)
        print("(list them with --list untested|nofix|custom; --save after an intended change)")
        return 1
    print("OK: no more untested, Fix-less or code-less messages than the baseline")
    return 0


if __name__ == "__main__":
    sys.exit(main())
