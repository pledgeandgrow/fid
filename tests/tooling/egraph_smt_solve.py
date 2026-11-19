#!/usr/bin/env python3
"""egraph_smt_solve.py - ask z3 about SMT-LIB proof obligations in one process.

Usage: egraph_smt_solve.py <dir> [prefix]
  Every <dir>/<prefix>*.smt2 (default prefix "eg_": the e-graph's
  obligations, compiler/egraph.fd egraph_smt) is checked; each asserts that a
  rewrite has a counterexample, so `unsat` proves it.
Prints one line per file that is not `unsat` ("NOT PROVED (<result>): <file>")
and a summary "<n> files, <p> proved, <b> not". Exit status: 2 without the
z3 module (pip install z3-solver), else 1 if any file is not proved.
"""
import glob
import os
import sys


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return 2
    d = sys.argv[1]
    prefix = sys.argv[2] if len(sys.argv) > 2 else "eg_"
    try:
        import z3
    except ImportError:
        print("no z3 module (pip install z3-solver)")
        return 2
    files = sorted(glob.glob(os.path.join(d, prefix + "*.smt2")))
    bad = 0
    for f in files:
        s = z3.Solver()
        try:
            s.from_file(f)
            r = str(s.check())
        except z3.Z3Exception as e:  # a file z3 cannot read is not a proof
            r = "error: " + str(e).strip()[:200]
        if r != "unsat":
            print("NOT PROVED (%s): %s" % (r, os.path.basename(f)))
            bad += 1
    print("%d files, %d proved, %d not" % (len(files), len(files) - bad, bad))
    return 1 if bad or not files else 0


if __name__ == "__main__":
    sys.exit(main())
