#!/usr/bin/env python3
"""own_model.py - a bounded, exhaustive check of the memory-safety claim on
the ownership calculus of docs/verification/memory-safety-core.md (VF11),
extended with copy-on-write, boxes held inside boxes (a Vec of Vecs) and
calls with `&mut` parameters.

It is mechanised in the model-checking sense, not a proof: every program of
the source calculus up to --size statements is enumerated, the compiler's
two checks are applied to it (the borrow rules of reference §8.4 and the
escape rule), and each accepted program is run twice:

  * lowered: the translation of proposal 0004 (dup where a value is kept,
    or a move at `let x = y` when nothing reads y again, by name or through
    a reference, as compiler/lower.fd's Lowerer_last_use; drop at scope end
    and before an assignment, copy on a shared write, borrowed
    parameters), on a heap of counted cells. A use of a freed cell,
    a drop of one, a count that differs from the number of owners after any
    statement (Inv 1), or a cell left at the end (a leak) is a failure;
  * by value: the same program on plain trees (no heap, no sharing).
    Everything the lowered run reads must be what the value run reads: the
    lowering means what the language says (reference §8.1).

So, up to the bound, "accepted => no use after free, no double free, no
leak, and the lowering is the value semantics" is checked for every program.

--self-test: each mutant below (one check or one lowering step removed or
reordered) must be caught by some program, and the real configuration by
none. A model that no mutant can fail would test nothing.

Extensions (--ext, default all; each explored on its own, beside the base
calculus, up to --ext-size statements, default --size):

  refs2    a second reference variable q (two borrows at once, of the same
           or of different owners)
  struct   a struct Pr { f: Box, g: Box } with managed fields: literal,
           copy, assignment, field read, field assignment, field write (CoW)
  enum     an enum Ob { Nb, Sb(Box) }: construction, copy, reassignment,
           a match that keeps the payload
  closure  closures capturing a box by value (fn() -> str), their copies
           and calls
  rc       Rc<Box>: Rc::new, clone, reassignment, get, count (shared, not
           copy-on-write; count = the number of live handles)

scripts/verify/own_diff.py translates the accepted programs to Fides and
runs them through `fid run`, `fid run --leaks` and --checked-alloc: that is
the differential check of compiler/lower.fd against this model.

Usage: python3 scripts/verify/own_model.py [--size N] [--ext all|none|a,b]
                                            [--ext-size N] [--self-test] [-v]
Exit status: 0 when the real configuration passes (and, with --self-test,
every mutant is caught); 1 otherwise.
"""
import argparse
import sys
import time

OWNERS = ("a", "b")
MUTANTS = {
    "no_borrowck": "borrow checker off: a borrowed variable may be changed",
    "no_escape": "escape check off: a reference may outlive its target's block",
    "drop_before_dup": "x = y drops x's old value before it duplicates y",
    "no_cow": "a write to a shared box writes in place (no copy)",
    "cow_no_dup": "the copy of a shared box does not count its elements",
    "no_scope_drop": "a block's owners are not dropped at its end",
    "drop_param": "a callee drops its borrowed parameter",
    "move_ignores_refs": "let x = y moves at y's last use by name though a reference to y is read later (OW7)",
    # extensions (EXT_OF: the extension whose programs can catch it)
    "one_ref_borrowck": "the borrow checker follows only the first reference, r",
    "struct_copy_no_dup": "copying a struct does not count its managed fields",
    "setf_no_drop": "s.f = y does not drop the field's old value",
    "setf_drop_before_dup": "s = t drops s's old fields before it counts t's",
    "match_no_dup": "a match that keeps the payload does not count it",
    "capture_no_dup": "a closure captures a box without counting it",
    "capture_by_ref": "a closure reads the variable, not the value captured",
    "rc_clone_no_incr": "Rc clone does not count one more handle",
    "rc_get_no_dup": "Rc get hands out what it holds without counting it",
}
EXT_OF = {
    "one_ref_borrowck": "refs2",
    "struct_copy_no_dup": "struct",
    "setf_no_drop": "struct",
    "setf_drop_before_dup": "struct",
    "match_no_dup": "enum",
    "capture_no_dup": "closure",
    "capture_by_ref": "closure",
    "rc_clone_no_incr": "rc",
    "rc_get_no_dup": "rc",
}
EXTS = ("refs2", "struct", "enum", "closure", "rc")
# names of each kind (a program's names are typed by their spelling, which
# is what lets own_diff.py give every variable a Fides type)
STRUCTS = ("s", "u")
ENUMS = ("o", "w")
CLOS = ("c", "d")
RCS = ("h", "k")
REF_NAMES = ("r", "q", "p")


def refname(st):
    """The reference a (re)borrow binds: r, or the one it names."""
    return st[3] if len(st) > 3 else "r"

# Callee bodies: f_k(p: &mut Box), with a local t
CALLEES = [
    [("write", "p")],
    [("setnew", "p")],
    [("push", "p", "p")],
    [("copy", "t", "p"), ("write", "t"), ("read", "t")],
    [("take", "p", "t"), ("read", "t")],
]


class Reject(Exception):
    pass


class Fail(Exception):
    pass


class Panic(Exception):
    pass


# ---------------------------------------------------------------- checker
def uses_of(st):
    """Names a statement reads or writes (not the names it declares)."""
    k = st[0]
    if k in ("copy", "take"):
        return [st[2]] if k == "copy" else [st[1]]
    if k in ("setnew", "write", "read"):
        return [st[1]]
    if k in ("set", "push"):
        return [st[1], st[2]]
    if k in ("bor", "rebor"):
        return [st[1]]
    if k == "call":
        return [st[2]]
    # extensions: (statement kind -> positions of the names it uses)
    pos = USE_POS.get(k)
    if pos is not None:
        return [st[i] for i in pos]
    return []


USE_POS = {
    "mkst": (2, 3), "cpst": (2,), "asst": (1, 2), "getf": (2,),
    "setf": (1, 3), "wrf": (1,), "readst": (1,),
    "mksome": (2,), "mknone": (), "cpen": (2,), "setsome": (1, 2),
    "setnone": (1,), "unwrap": (2,), "readen": (1,),
    "mkclo": (2,), "cpclo": (2,), "callc": (1,),
    "mkrc": (2,), "cprc": (2,), "asrc": (1, 2), "rcget": (2,),
    "rccount": (1,), "rcread": (1,),
}
# extension statements that declare a name: kind -> position of that name
DECL_POS = {
    "mkst": 1, "cpst": 1, "getf": 1, "mksome": 1, "mknone": 1, "cpen": 1,
    "unwrap": 1, "mkclo": 1, "cpclo": 1, "mkrc": 1, "cprc": 1, "rcget": 1,
}


def changes(st, x):
    """Does the statement change owner x directly (reference §8.4 rule 3),
    or take a &mut of it (rule 1)?"""
    k = st[0]
    if k in ("set", "setnew", "write", "push") and st[1] == x:
        return True
    if k == "take" and st[1] == x:
        return True
    if k == "call" and st[2] == x:
        return True
    if k in ("bor", "rebor") and st[1] == x and st[2]:
        return True
    return False


def flatten(prog, depth=0, out=None):
    out = [] if out is None else out
    for st in prog:
        if st[0] == "block":
            out.append(("{", depth + 1))
            flatten(st[1], depth + 1, out)
            out.append(("}", depth + 1))
        else:
            out.append((st, depth))
    return out


def check(prog, mut, params=()):
    """The front end: scopes, the borrow rules, the escape rule."""
    flat = flatten(prog)
    scopes = [{p: ("borrow", True, 0) for p in params}]
    rmut = {p: True for p in params}
    for i, (st, d) in enumerate(flat):
        if st == "{":
            scopes.append({})
            continue
        if st == "}":
            scopes.pop()
            continue
        vis = {}
        for s in scopes:
            vis.update(s)
        for n in uses_of(st):
            if n not in vis:
                raise Reject("undeclared " + n)
        k = st[0]
        if k in ("new", "copy", "take") or k in DECL_POS:
            x = st[1] if k != "take" else st[2]
            if x in vis or x in REF_NAMES:
                raise Reject("redeclared")
            scopes[-1][x] = ("owner", None, len(scopes) - 1)
        if k == "copy" and vis[st[2]][0] == "owner" and st[2] == st[1]:
            raise Reject("self")
        if k in ("bor", "rebor"):
            rn = refname(st)
            if vis[st[1]][0] != "owner":
                raise Reject("borrow of a borrow")
            if k == "bor":
                if rn in vis:
                    raise Reject("redeclared " + rn)
                scopes[-1][rn] = ("borrow", st[2], len(scopes) - 1)
            else:
                if rn not in vis:
                    raise Reject("undeclared " + rn)
                rd = vis[rn][2]
                xd = vis[st[1]][2]
                if xd > rd and "no_escape" not in mut:
                    raise Reject("escape: %s would outlive %s" % (rn, st[1]))
                for s in scopes:
                    if rn in s:
                        s[rn] = ("borrow", st[2], s[rn][2])
            rmut[rn] = st[2]
        # a change through a borrow needs a &mut
        if k in ("setnew", "write", "push", "take", "set") and vis.get(st[1], ("owner",))[0] == "borrow":
            if not vis[st[1]][1]:
                raise Reject("change through a shared borrow")
        if k == "call" and vis[st[2]][0] != "owner":
            raise Reject("call takes &mut of an owner")
        # NLL: a (re)borrow of x by r is live until r's last use before r is
        # bound again or leaves its block
        if k in ("bor", "rebor") and "no_borrowck" not in mut and not (
                "one_ref_borrowck" in mut and refname(st) != "r"):
            x, m = st[1], st[2]
            rn = refname(st)
            depth_r = len(scopes) - 1 if k == "bor" else vis[rn][2]
            last = None
            level = len(scopes) - 1
            for j in range(i + 1, len(flat)):
                sj, dj = flat[j]
                if sj == "}":
                    level -= 1
                    if level < depth_r:
                        break
                    continue
                if sj == "{":
                    level += 1
                    continue
                if sj[0] in ("bor", "rebor") and refname(sj) == rn:
                    break
                if rn in uses_of(sj):
                    last = j
            if last is not None:
                for j in range(i + 1, last + 1):
                    sj = flat[j][0]
                    if sj in ("{", "}"):
                        continue
                    # two-phase borrows (borrowchecker.md Phase 4): in
                    # x.push(*r), the call's &mut x is reserved while its
                    # argument reads r for the last time, then activated
                    if j == last and sj[0] == "push" and sj[1] == x and sj[2] == rn:
                        continue
                    if changes(sj, x):
                        raise Reject("borrowed " + x + " changed")
                    if m and sj[0] in ("bor", "rebor") and sj[1] == x:
                        raise Reject("&x while &mut x")


# ---------------------------------------------------------------- lowered
def held(s):
    """The cells a variable's slot owns. Slots: ("own", l) a box or an Rc
    handle; ("st", [f, g]) a struct, its fields owned; ("en", l or None) an
    enum and its payload; ("clo", l) a closure and the box it captured;
    ("mp", frame, name) / ("sa", l) / ("cref", frame, name) references and a
    by-reference capture (mutant), which own nothing."""
    k = s[0]
    if k in ("own", "clo"):
        return [s[1]]
    if k == "st":
        return list(s[1])
    if k == "en":
        return [s[1]] if s[1] is not None else []
    return []


class Machine:
    def __init__(self, mut):
        self.mut = mut
        self.H = {}
        self.next = 1
        self.frames = []
        self.obs = []
        self.flat = []    # the program as flatten() gives it (for last uses)
        self.pc = 0       # the next statement's index in self.flat
        self.cur = -1     # the index of the statement being run
        self.in_call = 0  # > 0 inside a callee (not in self.flat)

    def alloc(self):
        l = self.next
        self.next += 1
        self.H[l] = [1, 0, []]
        return l

    def live(self, l):
        if l not in self.H:
            raise Fail("use after free")
        return self.H[l]

    def dup(self, l):
        self.live(l)[0] += 1

    def drop(self, l):
        if l not in self.H:
            raise Fail("double free")
        c = self.H[l]
        c[0] -= 1
        if c[0] == 0:
            del self.H[l]
            for ch in c[2]:
                self.drop(ch)

    def slot(self, name):
        fr = self.frames[-1]
        s = fr["vars"].get(name)
        while s is not None and s[0] == "mp":
            tf = self.frames[s[1]] if s[1] < len(self.frames) else None
            if tf is None or s[2] not in tf["vars"]:
                raise Fail("dangling place")
            fr = tf
            s = tf["vars"][s[2]]
        if s is None:
            raise Fail("no slot " + name)
        return fr, s

    def val(self, name):
        fr, s = self.slot(name)
        if s[0] == "moved":
            raise Fail("use after move of " + name)
        l = s[1]
        self.live(l)
        return l

    def can_move(self, y):
        """compiler/lower.fd Lowerer_last_use: `let x = y` hands y's value
        over (no dup; y is left moved) when y is an owner of this frame that
        no later statement names. The compiler's condition must also see
        the references: no reference that points to y may be used later
        (OW7: without it a read through r after the move finds nothing).
        Only the main program moves (callee bodies are not in self.flat)."""
        if self.in_call or self.cur < 0:
            return False
        fr = self.frames[-1]
        s = fr["vars"].get(y)
        if s is None or s[0] != "own":
            return False
        later = [st for st, _ in self.flat[self.cur + 1:] if st not in ("{", "}")]
        for st in later:
            if y in uses_of(st) or (st[0] in ("bor", "rebor") and st[1] == y):
                return False
        if "move_ignores_refs" in self.mut:
            return True
        fi = len(self.frames) - 1
        for rn, rs in fr["vars"].items():
            if (rs[0] == "mp" and rs[1] == fi and rs[2] == y) or (rs[0] == "sa" and rs[1] == s[1]):
                if any(rn in uses_of(st) for st in later):
                    return False
        return True

    def place_set(self, name, l):
        fr = self.frames[-1]
        s = fr["vars"][name]
        key = name
        while s[0] == "mp":
            fr = self.frames[s[1]]
            key = s[2]
            s = fr["vars"][key]
        if s[0] != "own":
            raise Fail("assignment through a shared borrow")
        fr["vars"][key] = ("own", l)

    def unshare(self, l):
        """Copy-on-write: the cell to write instead of l (l itself when it
        has one owner); the copy counts its elements."""
        c = self.live(l)
        if c[0] > 1 and "no_cow" not in self.mut:
            n = self.alloc()
            self.H[n][1] = c[1]
            self.H[n][2] = list(c[2])
            if "cow_no_dup" not in self.mut:
                for ch in c[2]:
                    self.dup(ch)
            c[0] -= 1
            return n
        return l

    def cow(self, name):
        l = self.val(name)
        n = self.unshare(l)
        if n != l:
            self.place_set(name, n)

    def own_slot(self, name):
        """The slot of a struct, enum, closure or Rc variable (never behind
        a reference: references are to boxes only)."""
        s = self.frames[-1]["vars"].get(name)
        if s is None:
            raise Fail("no slot " + name)
        return s

    def set_slot(self, name, s):
        self.frames[-1]["vars"][name] = s

    def deep(self, l):
        c = self.live(l)
        return (c[1], tuple(self.deep(ch) for ch in c[2]))

    def declare(self, name, slot):
        fr = self.frames[-1]
        fr["vars"][name] = slot
        fr["blocks"][-1].append(name)

    def end_block(self, drop=True):
        fr = self.frames[-1]
        names = fr["blocks"].pop()
        for n in reversed(names):
            s = fr["vars"].pop(n)
            if drop:
                for l in held(s):
                    self.drop(l)

    def invariant(self):
        owners = {}
        for fr in self.frames:
            for s in fr["vars"].values():
                for l in held(s):
                    owners[l] = owners.get(l, 0) + 1
        for c in self.H.values():
            for ch in c[2]:
                owners[ch] = owners.get(ch, 0) + 1
        for l, n in owners.items():
            if l not in self.H:
                raise Fail("an owner holds a freed cell")
            if self.H[l][0] != n:
                raise Fail("count %d, owners %d" % (self.H[l][0], n))
        for l in self.H:
            if l not in owners:
                raise Fail("unreachable cell (count without owner)")

    def run(self, prog):
        self.frames.append({"vars": {}, "blocks": [[]]})
        self.flat = flatten(prog)
        self.pc = 0
        try:
            self.block(prog)
        except Panic:
            self.obs.append("panic")
            return self.obs
        self.end_block()
        self.frames.pop()
        if self.H:
            raise Fail("leak: %d cells left" % len(self.H))
        return self.obs

    def block(self, prog):
        for st in prog:
            self.stmt(st)
            self.invariant()

    def stmt(self, st):
        k = st[0]
        m = self.mut
        if not self.in_call:
            if k == "block":
                self.pc += 1          # its "{"
            else:
                self.cur = self.pc
                self.pc += 1
        if k == "new":
            self.declare(st[1], ("own", self.alloc()))
        elif k == "copy":
            if "no_move" not in m and self.can_move(st[2]):
                v = self.val(st[2])
                self.frames[-1]["vars"][st[2]] = ("moved",)
                self.declare(st[1], ("own", v))
                return
            v = self.val(st[2])
            self.dup(v)
            self.declare(st[1], ("own", v))
        elif k == "setnew":
            t = self.alloc()
            self.drop(self.val(st[1]))
            self.place_set(st[1], t)
        elif k == "set":
            if "drop_before_dup" in m:
                self.drop(self.val(st[1]))
                v = self.val(st[2])
                self.dup(v)
            else:
                v = self.val(st[2])
                self.dup(v)
                self.drop(self.val(st[1]))
            self.place_set(st[1], v)
        elif k == "write":
            self.cow(st[1])
            self.H[self.val(st[1])][1] += 1
        elif k == "read":
            self.obs.append(self.deep(self.val(st[1])))
        elif k == "push":
            v = self.val(st[2])
            self.dup(v)
            self.cow(st[1])
            self.H[self.val(st[1])][2].append(v)
        elif k == "take":
            self.cow(st[1])
            c = self.H[self.val(st[1])]
            if not c[2]:
                raise Panic()
            self.declare(st[2], ("own", c[2].pop()))
        elif k in ("bor", "rebor"):
            fi = len(self.frames) - 1
            s = ("mp", fi, st[1]) if st[2] else ("sa", self.val(st[1]))
            if k == "bor":
                self.declare(refname(st), s)
            else:
                self.frames[-1]["vars"][refname(st)] = s
        elif k == "block":
            self.frames[-1]["blocks"].append([])
            self.block(st[1])
            self.end_block(drop="no_scope_drop" not in m)
            if not self.in_call:
                self.pc += 1          # its "}"
        elif k == "call":
            fi = len(self.frames) - 1
            self.frames.append({"vars": {"p": ("mp", fi, st[2])}, "blocks": [[]]})
            self.frames[-1]["blocks"][-1].append("p")
            self.in_call += 1
            self.block(CALLEES[st[1]])
            self.in_call -= 1
            if "drop_param" in m:
                self.drop(self.val("p"))
            self.end_block()
            self.frames.pop()
        else:
            self.ext_stmt(st)

    def ext_stmt(self, st):
        """The statements of the extensions (struct, enum, closure, rc)."""
        k = st[0]
        m = self.mut
        if k == "mkst":
            f = self.val(st[2])
            self.dup(f)
            g = self.val(st[3])
            self.dup(g)
            self.declare(st[1], ("st", [f, g]))
        elif k == "cpst":
            fs = list(self.own_slot(st[2])[1])
            if "struct_copy_no_dup" not in m:
                for l in fs:
                    self.dup(l)
            self.declare(st[1], ("st", fs))
        elif k == "asst":
            old = self.own_slot(st[1])[1]
            if "setf_drop_before_dup" in m:
                for l in old:
                    self.drop(l)
                new = list(self.own_slot(st[2])[1])
                for l in new:
                    self.dup(l)
            else:
                new = list(self.own_slot(st[2])[1])
                for l in new:
                    self.dup(l)
                for l in old:
                    self.drop(l)
            self.set_slot(st[1], ("st", new))
        elif k == "getf":
            l = self.own_slot(st[2])[1][st[3]]
            self.dup(l)
            self.declare(st[1], ("own", l))
        elif k == "setf":
            fs = self.own_slot(st[1])[1]
            v = self.val(st[3])
            self.dup(v)
            if "setf_no_drop" not in m:
                self.drop(fs[st[2]])
            fs[st[2]] = v
        elif k == "wrf":
            fs = self.own_slot(st[1])[1]
            fs[st[2]] = self.unshare(fs[st[2]])
            self.H[fs[st[2]]][1] += 1
        elif k == "readst":
            fs = self.own_slot(st[1])[1]
            self.obs.append(("st", self.deep(fs[0]), self.deep(fs[1])))
        elif k == "mksome":
            v = self.val(st[2])
            self.dup(v)
            self.declare(st[1], ("en", v))
        elif k == "mknone":
            self.declare(st[1], ("en", None))
        elif k == "cpen":
            v = self.own_slot(st[2])[1]
            if v is not None:
                self.dup(v)
            self.declare(st[1], ("en", v))
        elif k == "setsome":
            v = self.val(st[2])
            self.dup(v)
            old = self.own_slot(st[1])[1]
            if old is not None:
                self.drop(old)
            self.set_slot(st[1], ("en", v))
        elif k == "setnone":
            old = self.own_slot(st[1])[1]
            if old is not None:
                self.drop(old)
            self.set_slot(st[1], ("en", None))
        elif k == "unwrap":
            v = self.own_slot(st[2])[1]
            if v is None:
                v = self.alloc()
            elif "match_no_dup" not in m:
                self.dup(v)
            self.declare(st[1], ("own", v))
        elif k == "readen":
            v = self.own_slot(st[1])[1]
            self.obs.append(("en", None if v is None else self.deep(v)))
        elif k == "mkclo":
            if "capture_by_ref" in m:
                self.declare(st[1], ("cref", len(self.frames) - 1, st[2]))
                return
            v = self.val(st[2])
            if "capture_no_dup" not in m:
                self.dup(v)
            self.declare(st[1], ("clo", v))
        elif k == "cpclo":
            s = self.own_slot(st[2])
            if s[0] == "clo":
                self.dup(s[1])
            self.declare(st[1], s)
        elif k == "callc":
            s = self.own_slot(st[1])
            if s[0] == "cref":
                fr = self.frames[s[1]]
                if s[2] not in fr["vars"]:
                    raise Fail("closure reads a variable out of scope")
                l = fr["vars"][s[2]][1]
            else:
                l = s[1]
            self.obs.append(self.deep(l))
        elif k == "mkrc":
            v = self.val(st[2])
            self.dup(v)
            cell = self.alloc()
            self.H[cell][1] = None
            self.H[cell][2] = [v]
            self.declare(st[1], ("own", cell))
        elif k == "cprc":
            c = self.val(st[2])
            if "rc_clone_no_incr" not in m:
                self.dup(c)
            self.declare(st[1], ("own", c))
        elif k == "asrc":
            c = self.val(st[2])
            self.dup(c)
            self.drop(self.val(st[1]))
            self.place_set(st[1], c)
        elif k == "rcget":
            v = self.H[self.val(st[2])][2][0]
            if "rc_get_no_dup" not in m:
                self.dup(v)
            self.declare(st[1], ("own", v))
        elif k == "rccount":
            self.obs.append(("count", self.H[self.val(st[1])][0]))
        elif k == "rcread":
            self.obs.append(self.deep(self.H[self.val(st[1])][2][0]))
        else:
            raise Fail("unknown statement " + k)


# ---------------------------------------------------------------- by value
def run_value(prog):
    frames = [[{}]]  # frames of scopes; a borrow is ("place", frame, name)
    obs = []

    def find(name):
        fi = len(frames) - 1
        while True:
            for sc in reversed(frames[fi]):
                if name in sc:
                    v = sc[name]
                    if isinstance(v, tuple) and v and v[0] == "place":
                        fi, name = v[1], v[2]
                        break
                    return sc, name
            else:
                raise Fail("value run: no " + name)

    def get(n):
        sc, k = find(n)
        return sc[k]

    def put(n, v):
        sc, k = find(n)
        sc[k] = v

    def block(stmts):
        for st in stmts:
            k = st[0]
            cur = frames[-1][-1]
            if k == "new":
                cur[st[1]] = (0, ())
            elif k == "copy":
                cur[st[1]] = get(st[2])
            elif k == "setnew":
                put(st[1], (0, ()))
            elif k == "set":
                put(st[1], get(st[2]))
            elif k == "write":
                d, c = get(st[1])
                put(st[1], (d + 1, c))
            elif k == "read":
                obs.append(get(st[1]))
            elif k == "push":
                v = get(st[2])
                d, c = get(st[1])
                put(st[1], (d, c + (v,)))
            elif k == "take":
                d, c = get(st[1])
                if not c:
                    raise Panic()
                put(st[1], (d, c[:-1]))
                cur[st[2]] = c[-1]
            elif k in ("bor", "rebor"):
                pl = ("place", len(frames) - 1, st[1])
                rn = refname(st)
                if k == "bor":
                    cur[rn] = pl
                else:
                    for sc in reversed(frames[-1]):
                        if rn in sc:
                            sc[rn] = pl
                            break
            elif k == "block":
                frames[-1].append({})
                block(st[1])
                frames[-1].pop()
            elif k == "call":
                frames.append([{"p": ("place", len(frames) - 1, st[2])}])
                block(CALLEES[st[1]])
                frames.pop()
            else:
                ext(st, cur)

    objs = {}  # Rc: id -> the value it holds (never changed: no CoW, no write)

    def handles(rid):
        return sum(1 for fr in frames for sc in fr for v in sc.values()
                   if v == ("R", rid))

    def ext(st, cur):
        k = st[0]
        if k == "mkst":
            cur[st[1]] = ("S", get(st[2]), get(st[3]))
        elif k == "cpst":
            cur[st[1]] = get(st[2])
        elif k == "asst":
            put(st[1], get(st[2]))
        elif k == "getf":
            cur[st[1]] = get(st[2])[1 + st[3]]
        elif k == "setf":
            s = list(get(st[1]))
            s[1 + st[2]] = get(st[3])
            put(st[1], tuple(s))
        elif k == "wrf":
            s = list(get(st[1]))
            d, c = s[1 + st[2]]
            s[1 + st[2]] = (d + 1, c)
            put(st[1], tuple(s))
        elif k == "readst":
            s = get(st[1])
            obs.append(("st", s[1], s[2]))
        elif k == "mksome":
            cur[st[1]] = ("E", get(st[2]))
        elif k == "mknone":
            cur[st[1]] = ("E", None)
        elif k == "cpen":
            cur[st[1]] = get(st[2])
        elif k == "setsome":
            put(st[1], ("E", get(st[2])))
        elif k == "setnone":
            put(st[1], ("E", None))
        elif k == "unwrap":
            v = get(st[2])[1]
            cur[st[1]] = (0, ()) if v is None else v
        elif k == "readen":
            obs.append(("en", get(st[1])[1]))
        elif k == "mkclo":
            cur[st[1]] = ("C", get(st[2]))
        elif k == "cpclo":
            cur[st[1]] = get(st[2])
        elif k == "callc":
            obs.append(get(st[1])[1])
        elif k == "mkrc":
            rid = len(objs)
            objs[rid] = get(st[2])
            cur[st[1]] = ("R", rid)
        elif k in ("cprc",):
            cur[st[1]] = get(st[2])
        elif k == "asrc":
            put(st[1], get(st[2]))
        elif k == "rcget":
            cur[st[1]] = objs[get(st[2])[1]]
        elif k == "rccount":
            obs.append(("count", handles(get(st[1])[1])))
        elif k == "rcread":
            obs.append(objs[get(st[1])[1]])
        else:
            raise Fail("value run: unknown statement " + k)

    try:
        block(prog)
    except Panic:
        obs.append("panic")
    return obs


# ---------------------------------------------------------------- programs
def ext_alphabet(ext):
    """The statements one extension adds (over owners a, b and ref r)."""
    boxes = OWNERS + ("r",)
    out = []
    if ext == "refs2":
        for x in OWNERS:
            for m in (False, True):
                out.append(("bor", x, m, "q"))
                out.append(("rebor", x, m, "q"))
        for st in (("setnew", "q"), ("write", "q"), ("read", "q"),
                   ("set", "q", "a"), ("set", "a", "q"), ("push", "a", "q"),
                   ("push", "q", "r"), ("copy", "b", "q"), ("take", "q", "b")):
            out.append(st)
    elif ext == "struct":
        for s in STRUCTS:
            # s = Pr { f: a, g: a } (one box, two fields), u = Pr { f: a, g: *r }
            out.append(("mkst", s, "a", "r") if s == "u" else ("mkst", s, "a", "a"))
            out.append(("readst", s))
            for t in STRUCTS:
                if t != s:
                    out.append(("cpst", s, t))
                out.append(("asst", s, t))
            for i in (0, 1):
                out.append(("wrf", s, i))
                for y in boxes:
                    out.append(("setf", s, i, y))
        out.append(("getf", "b", "s", 0))
        out.append(("getf", "b", "u", 1))
    elif ext == "enum":
        for o in ENUMS:
            for x in boxes:
                out.append(("mksome", o, x))
                out.append(("setsome", o, x))
            out.append(("mknone", o))
            out.append(("setnone", o))
            out.append(("readen", o))
            for w in ENUMS:
                if w != o:
                    out.append(("cpen", o, w))
        for x in OWNERS:
            for o in ENUMS:
                out.append(("unwrap", x, o))
    elif ext == "closure":
        for c in CLOS:
            for x in OWNERS:
                out.append(("mkclo", c, x))
            out.append(("callc", c))
            for d in CLOS:
                if d != c:
                    out.append(("cpclo", c, d))
    elif ext == "rc":
        for h in RCS:
            for x in boxes:
                out.append(("mkrc", h, x))
            out.append(("rccount", h))
            out.append(("rcread", h))
            for k in RCS:
                if k != h:
                    out.append(("cprc", h, k))
                out.append(("asrc", h, k))
        for x in OWNERS:
            for h in RCS:
                out.append(("rcget", x, h))
    return out


def ext_base():
    """The base statements beside an extension: the ones that make, copy,
    change and read boxes, one borrow kind each, one call. (The whole base
    alphabet beside an extension makes size 4 take minutes.)"""
    out = []
    for x in OWNERS:
        out.append(("new", x))
        out.append(("write", x))
        out.append(("read", x))
        out.append(("bor", x, True))
        out.append(("bor", x, False))
        out.append(("call", 2, x))
    out += [("copy", "b", "a"), ("set", "a", "b"), ("set", "a", "r"),
            ("push", "a", "b"), ("write", "r"), ("rebor", "b", True)]
    return out


def alphabet(ext=None):
    if ext is not None:
        return ext_base() + ext_alphabet(ext)
    names = OWNERS + ("r",)
    out = []
    for x in OWNERS:
        out.append(("new", x))
        for y in names:
            if y != x:
                out.append(("copy", x, y))
        for y in names:
            if y != x:
                out.append(("take", y, x))
        for m in (False, True):
            out.append(("bor", x, m))
            out.append(("rebor", x, m))
        for k in range(len(CALLEES)):
            out.append(("call", k, x))
    for x in names:
        out.append(("setnew", x))
        out.append(("write", x))
        out.append(("read", x))
        for y in names:
            out.append(("set", x, y))
            out.append(("push", x, y))
    return out


def declares(st):
    k = st[0]
    if k in ("new", "copy"):
        return {st[1]}
    if k == "take":
        return {st[2]}
    if k == "bor":
        return {refname(st)}
    if k in DECL_POS:
        return {st[DECL_POS[k]]}
    return set()


def programs(size, inner=2, ext=None):
    """Every statement list of up to `size` statements (a block's own
    statements count, the block does not) whose names are declared before
    use: the pruning keeps the enumeration small. `ext`: one extension's
    alphabet (ext_alphabet) beside ext_base, instead of the base one."""
    alpha = alphabet(ext)
    simple = [s for s in alpha if s[0] != "call"]

    def ok_names(st, declared):
        return all(n in declared for n in uses_of(st)) and not (declares(st) & declared)

    def blocks(declared, n):
        # bodies of up to n statements with no nested block or call
        res = []

        def go(body, decl, left):
            if body:
                res.append(list(body))
            if left == 0:
                return
            for st in simple:
                if ok_names(st, decl):
                    go(body + [st], decl | declares(st), left - 1)
        go([], declared, n)
        return res

    def go(prog, declared, left):
        yield prog
        if left == 0:
            return
        for st in alpha:
            if ok_names(st, declared):
                yield from go(prog + [st], declared | declares(st), left - 1)
        if left >= 2:
            for body in blocks(declared, min(inner, left)):
                yield from go(prog + [("block", body)], declared, left - len(body))
    yield from go([], frozenset(), size)


def show(prog):
    def one(st):
        k = st[0]
        if k == "new":
            return "let %s = new" % st[1]
        if k == "copy":
            return "let %s = %s" % (st[1], st[2])
        if k == "setnew":
            return "%s = new" % st[1]
        if k == "set":
            return "%s = %s" % (st[1], st[2])
        if k == "write":
            return "write %s" % st[1]
        if k == "read":
            return "read %s" % st[1]
        if k == "push":
            return "%s.push(%s)" % (st[1], st[2])
        if k == "take":
            return "let %s = %s.pop()" % (st[2], st[1])
        if k == "bor":
            return "let %s = &%s%s" % (refname(st), "mut " if st[2] else "", st[1])
        if k == "rebor":
            return "%s = &%s%s" % (refname(st), "mut " if st[2] else "", st[1])
        if k == "call":
            return "f%d(&mut %s)" % (st[1], st[2])
        if k == "block":
            return "{ " + "; ".join(one(s) for s in st[1]) + " }"
        fld = ("f", "g")
        forms = {
            "mkst": lambda: "let %s = Pr { f: %s, g: %s }" % st[1:4],
            "cpst": lambda: "let %s = %s" % st[1:3],
            "asst": lambda: "%s = %s" % st[1:3],
            "getf": lambda: "let %s = %s.%s" % (st[1], st[2], fld[st[3]]),
            "setf": lambda: "%s.%s = %s" % (st[1], fld[st[2]], st[3]),
            "wrf": lambda: "write %s.%s" % (st[1], fld[st[2]]),
            "readst": lambda: "read %s" % st[1],
            "mksome": lambda: "let %s = Sb(%s)" % st[1:3],
            "mknone": lambda: "let %s = Nb" % st[1],
            "cpen": lambda: "let %s = %s" % st[1:3],
            "setsome": lambda: "%s = Sb(%s)" % st[1:3],
            "setnone": lambda: "%s = Nb" % st[1],
            "unwrap": lambda: "let %s = match %s { Sb(v) => v, Nb => new }" % st[1:3],
            "readen": lambda: "read %s" % st[1],
            "mkclo": lambda: "let %s = || read %s" % st[1:3],
            "cpclo": lambda: "let %s = %s" % st[1:3],
            "callc": lambda: "%s()" % st[1],
            "mkrc": lambda: "let %s = Rc::new(%s)" % st[1:3],
            "cprc": lambda: "let %s = %s.clone()" % st[1:3],
            "asrc": lambda: "%s = %s.clone()" % st[1:3],
            "rcget": lambda: "let %s = %s.get()" % st[1:3],
            "rccount": lambda: "read %s.count()" % st[1],
            "rcread": lambda: "read %s.get()" % st[1],
        }
        return forms[k]() if k in forms else repr(st)
    return "; ".join(one(s) for s in prog)


def accepted(size, mut=frozenset(), ext=None):
    """Yield (program, lowered reads, value reads) for every program the
    checks accept; a Fail of the lowered run is yielded as reads = Fail."""
    for k in range(len(CALLEES)):
        check(CALLEES[k], mut, params=("p",))
    for prog in programs(size, ext=ext):
        try:
            check(prog, mut)
        except Reject:
            continue
        try:
            got = Machine(mut).run(prog)
        except Fail as e:
            got = e
        yield prog, got, run_value(prog)


def explore(size, mut, first_only=False, ext=None):
    """-> (programs, accepted, failures[(prog, why)])"""
    n = acc = 0
    fails = []
    for k in range(len(CALLEES)):
        check(CALLEES[k], mut, params=("p",))
    for prog in programs(size, ext=ext):
        n += 1
        try:
            check(prog, mut)
        except Reject:
            continue
        acc += 1
        try:
            got = Machine(mut).run(prog)
            want = run_value(prog)
            if got != want:
                raise Fail("reads %r, the value semantics reads %r" % (got, want))
        except Fail as e:
            fails.append((prog, str(e)))
            if first_only:
                break
    return n, acc, fails


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--size", type=int, default=4)
    ap.add_argument("--ext", default="all",
                    help="extensions to explore: all, none, or a comma list of " + ", ".join(EXTS))
    ap.add_argument("--ext-size", type=int, default=None,
                    help="statements per extension program (default --size)")
    ap.add_argument("--self-test", action="store_true")
    ap.add_argument("-v", action="store_true")
    a = ap.parse_args()
    if a.ext == "all":
        exts = list(EXTS)
    elif a.ext in ("none", ""):
        exts = []
    else:
        exts = [e for e in a.ext.split(",") if e]
        for e in exts:
            if e not in EXTS:
                ap.error("unknown extension %r (have %s)" % (e, ", ".join(EXTS)))
    esize = a.ext_size if a.ext_size is not None else a.size
    t0 = time.time()
    n, acc, fails = explore(a.size, set())
    print("own_model: size %d, %d programs, %d accepted, %d failures (%.1f s)"
          % (a.size, n, acc, len(fails), time.time() - t0))
    for prog, why in fails[:10]:
        print("  FAIL %s: %s" % (show(prog), why))
    bad = 1 if fails else 0
    for e in exts:
        t0 = time.time()
        n, acc, fails = explore(esize, set(), ext=e)
        print("own_model: ext %-7s size %d, %d programs, %d accepted, %d failures (%.1f s)"
              % (e, esize, n, acc, len(fails), time.time() - t0))
        for prog, why in fails[:10]:
            print("  FAIL %s: %s" % (show(prog), why))
        if fails:
            bad = 1
    if a.self_test:
        for name, what in MUTANTS.items():
            e = EXT_OF.get(name)
            if e is not None and e not in exts:
                continue
            if e is None:
                sz = max(a.size, 5) if name == "no_escape" else a.size
            else:
                sz = max(esize, 4)
            _, _, mf = explore(sz, {name}, first_only=True, ext=e)
            if mf:
                print("  mutant %-20s caught: %s -> %s" % (name, show(mf[0][0]), mf[0][1]))
            else:
                print("  mutant %-20s NOT caught (%s)" % (name, what))
                bad = 1
    return bad


if __name__ == "__main__":
    sys.exit(main())
