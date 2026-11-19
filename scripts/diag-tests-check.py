#!/usr/bin/env python3
"""Each expected-fail test fails with the error it says it expects (CE4,
competitive TL2).

scripts/diag-coverage.py counts a compiler message as tested when a test's
`// expect-error: <text>` line matches the message's text; scripts/tier1.sh
checks that such a test is refused. Neither runs the compiler to see that the
test is refused *for that reason*: a test whose program has a second, earlier
error (or whose message changed) still counts. This script does: for every
test with `// expect-error:` lines it runs `fid check` on it (then `fid run`
when check accepts it or does not print the text: some errors come from
monomorphization or at run time) and requires every expected text in the
output.

The comparison ignores differences of white space (a message's explanation
lines are indented, an expectation is one line) and the `[build]`/`[prof]`
progress lines.

  python3 scripts/diag-tests-check.py --fid release/fid.exe
  python3 scripts/diag-tests-check.py --fid $FID tests/bootstrap/test_diag_*_fail.fd
  python3 scripts/diag-tests-check.py --fid $FID --jobs 4 --verbose

Exit status 0 when every test printed every text it expects, 1 otherwise
(the misses are listed, with the first lines the compiler printed).
"""
import argparse
import concurrent.futures
import glob
import os
import re
import subprocess
import sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
EXPECT = re.compile(r"^\s*//\s*expect-error\s*:\s*(.+?)\s*$", re.M)


def ws(s):
    return re.sub(r"\s+", " ", s).strip()


def expectations(path):
    try:
        with open(path, encoding="utf-8", errors="replace") as f:
            text = f.read()
    except OSError:
        return []
    return [m.group(1) for m in EXPECT.finditer(text) if len(m.group(1)) >= 3]


def run(fid, cmd, path, timeout):
    env = dict(os.environ)
    env.setdefault("FID_HOME", ROOT)
    try:
        p = subprocess.run([fid, cmd, path], cwd=ROOT, env=env, stdin=subprocess.DEVNULL,
                           stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=timeout)
        out = p.stdout.decode("utf-8", errors="replace")
        rc = p.returncode
    except subprocess.TimeoutExpired as e:
        out = (e.stdout or b"").decode("utf-8", errors="replace") + "\n(timed out)"
        rc = -1
    lines = [l for l in out.splitlines() if not re.match(r"^\[(build|prof|time)\]", l)]
    return rc, "\n".join(lines)


def check_one(fid, path, timeout):
    exps = expectations(path)
    rc, out = run(fid, "check", path, timeout)
    missing = [e for e in exps if ws(e) not in ws(out)]
    used = "check"
    if missing or rc == 0:
        rc2, out2 = run(fid, "run", path, timeout)
        missing2 = [e for e in exps if ws(e) not in ws(out2)]
        if len(missing2) < len(missing) or (rc == 0 and rc2 != 0):
            rc, out, missing, used = rc2, out2, missing2, "run"
    if rc == 0:
        status = "ACCEPTED"
    elif missing:
        status = "MISS"
    else:
        status = "OK"
    return path, status, missing, out, used


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("files", nargs="*", help="tests (default: tests/bootstrap/**/*.fd with expect-error)")
    ap.add_argument("--fid", default=os.environ.get("FID", ""))
    ap.add_argument("--jobs", "-j", type=int, default=1)
    ap.add_argument("--timeout", type=int, default=120)
    ap.add_argument("--verbose", "-v", action="store_true")
    a = ap.parse_args()
    try:
        sys.stdout.reconfigure(errors="replace")
    except (AttributeError, ValueError):
        pass
    fid = a.fid
    if not fid:
        for c in ("release/fid.exe", "release/fid"):
            if os.path.exists(os.path.join(ROOT, c)):
                fid = c
                break
    if not fid:
        print("no fid: pass --fid or set FID")
        return 2
    if not os.path.isabs(fid) and os.path.exists(os.path.join(ROOT, fid)):
        fid = os.path.join(ROOT, fid)

    files = a.files or sorted(glob.glob(os.path.join(ROOT, "tests", "bootstrap", "**", "*.fd"), recursive=True))
    files = [os.path.relpath(os.path.abspath(f), ROOT).replace("\\", "/") for f in files]
    files = [f for f in files if expectations(os.path.join(ROOT, f))]
    if not files:
        print("no test with an expect-error line")
        return 2

    results = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=max(1, a.jobs)) as ex:
        futs = [ex.submit(check_one, fid, f, a.timeout) for f in files]
        for fu in concurrent.futures.as_completed(futs):
            results.append(fu.result())
    results.sort()
    bad = 0
    for path, status, missing, out, used in results:
        if status == "OK":
            if a.verbose:
                print(f"OK        {path} ({used})")
            continue
        bad += 1
        print(f"{status:9} {path} ({used})")
        for m in missing:
            print(f"    expected: {m}")
        shown = [l for l in out.splitlines() if l.strip()][:6]
        for l in shown:
            print(f"    got:      {l[:200]}")
    print(f"{len(results) - bad} of {len(results)} expected-fail tests print what they expect")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
