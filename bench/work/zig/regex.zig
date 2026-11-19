// regex (BM4): see bench/work/fides/regex.fd. A hand-written Thompson NFA
// (Zig's standard library has no regex).
const std = @import("std");

const gpa = std.heap.smp_allocator;
const Buf = std.ArrayListUnmanaged(u8);
const List = std.ArrayListUnmanaged(i64);

var seed: i64 = 1;

fn rnd() i64 {
    seed = @mod(seed * 1103515245 + 12345, 2147483648);
    return seed >> 8;
}

fn putByte(o: *Buf, b: u8) void {
    o.append(gpa, b) catch @panic("oom");
}

fn push(l: *List, v: i64) void {
    l.append(gpa, v) catch @panic("oom");
}

const dict = [_][]const u8{ "cat", "dogs", "fox", "bird", "king", "queen", "xyz", "banana" };

fn genText(lines: i64) Buf {
    var o: Buf = .empty;
    var l: i64 = 0;
    while (l < lines) : (l += 1) {
        const w = 3 + @mod(rnd(), 8);
        var j: i64 = 0;
        while (j < w) : (j += 1) {
            if (j > 0) putByte(&o, ' ');
            const k = @mod(rnd(), 10);
            if (k < 6) {
                const n = 2 + @mod(rnd(), 7);
                var i: i64 = 0;
                while (i < n) : (i += 1) putByte(&o, @intCast(97 + @mod(rnd(), 26)));
            } else if (k < 8) {
                const n = 1 + @mod(rnd(), 5);
                var i: i64 = 0;
                while (i < n) : (i += 1) putByte(&o, @intCast(48 + @mod(rnd(), 10)));
            } else if (k == 8) {
                o.appendSlice(gpa, dict[@intCast(@mod(rnd(), 8))]) catch @panic("oom");
            } else {
                var n = 1 + @mod(rnd(), 3);
                var i: i64 = 0;
                while (i < n) : (i += 1) putByte(&o, @intCast(48 + @mod(rnd(), 10)));
                putByte(&o, '-');
                n = 1 + @mod(rnd(), 4);
                i = 0;
                while (i < n) : (i += 1) putByte(&o, @intCast(48 + @mod(rnd(), 10)));
            }
        }
        putByte(&o, '\n');
    }
    return o;
}

const patterns = [_][]const u8{
    "ab+c",
    "(cat|dogs?|fox) [a-z]+",
    "[0-9][0-9]-[0-9]+",
    "x.*y.*z",
    "q[^u]",
    "[aeiou][aeiou][aeiou]",
    "k(e|i)?ng",
    "\\d+-\\d\\d\\d\\d",
};

// state kinds: 0 byte (arg), 1 any but newline, 2 class (arg), 3 split,
// 4 empty, 5 match
const S_CHAR: i64 = 0;
const S_ANY: i64 = 1;
const S_CLASS: i64 = 2;
const S_SPLIT: i64 = 3;
const S_EPS: i64 = 4;
const S_MATCH: i64 = 5;

const Nfa = struct {
    kind: List = .empty,
    arg: List = .empty,
    out1: List = .empty,
    out2: List = .empty,
    cls: Buf = .empty,

    fn deinit(n: *Nfa) void {
        n.kind.deinit(gpa);
        n.arg.deinit(gpa);
        n.out1.deinit(gpa);
        n.out2.deinit(gpa);
        n.cls.deinit(gpa);
    }

    fn st(n: *Nfa, k: i64, a: i64, o1: i64, o2: i64) i64 {
        push(&n.kind, k);
        push(&n.arg, a);
        push(&n.out1, o1);
        push(&n.out2, o2);
        return @as(i64, @intCast(n.kind.items.len)) - 1;
    }

    fn newClass(n: *Nfa) i64 {
        const id: i64 = @intCast(n.cls.items.len / 256);
        n.cls.appendNTimes(gpa, 0, 256) catch @panic("oom");
        return id;
    }

    fn classAddEsc(n: *Nfa, base: usize, e: u8) void {
        if (e == 'd') {
            var c: usize = 48;
            while (c < 58) : (c += 1) n.cls.items[base + c] = 1;
        } else if (e == 'w') {
            var c: usize = 0;
            while (c < 256) : (c += 1) {
                if ((c >= 48 and c <= 57) or (c >= 65 and c <= 90) or (c >= 97 and c <= 122) or c == 95) n.cls.items[base + c] = 1;
            }
        } else if (e == 's') {
            n.cls.items[base + 32] = 1;
            n.cls.items[base + 9] = 1;
            n.cls.items[base + 10] = 1;
            n.cls.items[base + 13] = 1;
        } else {
            n.cls.items[base + e] = 1;
        }
    }
};

const Frag = struct { s: i64, e: i64 };

const Compiler = struct {
    p: []const u8,
    pos: usize = 0,
    n: Nfa = .{},

    fn setOut(c: *Compiler, s: i64, to: i64) void {
        c.n.out1.items[@intCast(s)] = to;
    }

    fn atom(c: *Compiler) Frag {
        const ch = c.p[c.pos];
        if (ch == '(') {
            c.pos += 1;
            const f = c.alt();
            c.pos += 1;
            return f;
        }
        const e = c.n.st(S_EPS, 0, -1, -1);
        if (ch == '.') {
            c.pos += 1;
            return .{ .s = c.n.st(S_ANY, 0, e, -1), .e = e };
        }
        if (ch == '[') {
            c.pos += 1;
            const id = c.n.newClass();
            const base: usize = @as(usize, @intCast(id)) * 256;
            var neg = false;
            if (c.p[c.pos] == '^') {
                neg = true;
                c.pos += 1;
            }
            while (c.p[c.pos] != ']') {
                const a = c.p[c.pos];
                if (a == '\\') {
                    c.n.classAddEsc(base, c.p[c.pos + 1]);
                    c.pos += 2;
                } else if (c.pos + 2 < c.p.len and c.p[c.pos + 1] == '-' and c.p[c.pos + 2] != ']') {
                    const b = c.p[c.pos + 2];
                    var x: usize = a;
                    while (x <= b) : (x += 1) c.n.cls.items[base + x] = 1;
                    c.pos += 3;
                } else {
                    c.n.cls.items[base + a] = 1;
                    c.pos += 1;
                }
            }
            c.pos += 1;
            if (neg) {
                var x: usize = 0;
                while (x < 256) : (x += 1) c.n.cls.items[base + x] = 1 - c.n.cls.items[base + x];
                c.n.cls.items[base + 10] = 0;
            }
            return .{ .s = c.n.st(S_CLASS, id, e, -1), .e = e };
        }
        if (ch == '\\') {
            const esc = c.p[c.pos + 1];
            c.pos += 2;
            if (esc == 'd' or esc == 'w' or esc == 's') {
                const id = c.n.newClass();
                c.n.classAddEsc(@as(usize, @intCast(id)) * 256, esc);
                return .{ .s = c.n.st(S_CLASS, id, e, -1), .e = e };
            }
            return .{ .s = c.n.st(S_CHAR, esc, e, -1), .e = e };
        }
        c.pos += 1;
        return .{ .s = c.n.st(S_CHAR, ch, e, -1), .e = e };
    }

    fn repeat(c: *Compiler) Frag {
        var f = c.atom();
        while (c.pos < c.p.len and (c.p[c.pos] == '*' or c.p[c.pos] == '+' or c.p[c.pos] == '?')) {
            const op = c.p[c.pos];
            c.pos += 1;
            const e = c.n.st(S_EPS, 0, -1, -1);
            const s = c.n.st(S_SPLIT, 0, f.s, e);
            if (op == '*') {
                c.setOut(f.e, s);
                f = .{ .s = s, .e = e };
            } else if (op == '+') {
                c.setOut(f.e, s);
                f = .{ .s = f.s, .e = e };
            } else {
                c.setOut(f.e, e);
                f = .{ .s = s, .e = e };
            }
        }
        return f;
    }

    fn concat(c: *Compiler) Frag {
        var have = false;
        var f: Frag = .{ .s = 0, .e = 0 };
        while (c.pos < c.p.len and c.p[c.pos] != '|' and c.p[c.pos] != ')') {
            const g = c.repeat();
            if (!have) {
                f = g;
                have = true;
            } else {
                c.setOut(f.e, g.s);
                f = .{ .s = f.s, .e = g.e };
            }
        }
        if (!have) {
            const e = c.n.st(S_EPS, 0, -1, -1);
            f = .{ .s = e, .e = e };
        }
        return f;
    }

    fn alt(c: *Compiler) Frag {
        var f = c.concat();
        while (c.pos < c.p.len and c.p[c.pos] == '|') {
            c.pos += 1;
            const g = c.concat();
            const e = c.n.st(S_EPS, 0, -1, -1);
            const s = c.n.st(S_SPLIT, 0, f.s, g.s);
            c.setOut(f.e, e);
            c.setOut(g.e, e);
            f = .{ .s = s, .e = e };
        }
        return f;
    }
};

const Sim = struct {
    n: *const Nfa,
    mark: []i64,
    gen: i64 = 0,
    matched: bool = false,

    fn add(sim: *Sim, list: *List, s: i64) void {
        if (s < 0) return;
        const i: usize = @intCast(s);
        if (sim.mark[i] == sim.gen) return;
        sim.mark[i] = sim.gen;
        const k = sim.n.kind.items[i];
        if (k == S_SPLIT) {
            sim.add(list, sim.n.out1.items[i]);
            sim.add(list, sim.n.out2.items[i]);
        } else if (k == S_EPS) {
            sim.add(list, sim.n.out1.items[i]);
        } else if (k == S_MATCH) {
            sim.matched = true;
        } else {
            push(list, s);
        }
    }
};

fn count(pat: []const u8, t: []const u8) i64 {
    var c = Compiler{ .p = pat };
    defer c.n.deinit();
    const f = c.alt();
    const m = c.n.st(S_MATCH, 0, -1, -1);
    c.setOut(f.e, m);
    const n = &c.n;
    const start = f.s;
    const mark = gpa.alloc(i64, n.kind.items.len) catch @panic("oom");
    defer gpa.free(mark);
    @memset(mark, 0);
    var sim = Sim{ .n = n, .mark = mark };
    var cl: List = .empty;
    var nl: List = .empty;
    defer cl.deinit(gpa);
    defer nl.deinit(gpa);
    var cnt: i64 = 0;
    var a: usize = 0;
    while (a < t.len) {
        var b = a;
        while (t[b] != '\n') b += 1;
        sim.gen += 1;
        sim.matched = false;
        cl.clearRetainingCapacity();
        var i = a;
        while (true) {
            sim.add(&cl, start);
            if (sim.matched or i == b) break;
            const ch = t[i];
            sim.gen += 1;
            nl.clearRetainingCapacity();
            for (cl.items) |s| {
                const si: usize = @intCast(s);
                const k = n.kind.items[si];
                const arg = n.arg.items[si];
                if ((k == S_CHAR and arg == ch) or (k == S_ANY and ch != '\n') or
                    (k == S_CLASS and n.cls.items[@as(usize, @intCast(arg)) * 256 + ch] == 1))
                {
                    sim.add(&nl, n.out1.items[si]);
                }
            }
            if (sim.matched) break;
            const tmp = cl;
            cl = nl;
            nl = tmp;
            i += 1;
        }
        if (sim.matched) cnt += 1;
        a = b + 1;
    }
    return cnt;
}

pub fn main() !void {
    var t = genText(200000);
    defer t.deinit(gpa);
    var total: i64 = 0;
    for (patterns, 0..) |p, k| {
        total += @as(i64, @intCast(k + 1)) * count(p, t.items);
    }
    std.debug.print("{d}\n", .{total});
}
