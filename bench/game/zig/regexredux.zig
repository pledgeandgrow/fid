// regex-redux (Benchmarks Game, BM3): see bench/game/fides/regexredux.fd.
// Zig's standard library has no regular expressions, so this is the same
// small backtracking matcher every language of bench/game/ runs.
const std = @import("std");

const alu = "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

const gpa = std.heap.smp_allocator;
const Bytes = std.ArrayListUnmanaged(u8);

const Item = struct {
    set: [256]bool,
    star: bool,
};

// a compiled pattern: the items of every alternative one after the other,
// alts[a] .. alts[a + 1] - 1 those of alternative a
const Pat = struct {
    items: [32]Item,
    alts: [8]usize,
    nalts: usize,
    first: [256]bool,
};

fn compile(b: []const u8) Pat {
    var p: Pat = undefined;
    p.alts[0] = 0;
    p.nalts = 1;
    var nitems: usize = 0;
    var i: usize = 0;
    while (i < b.len) {
        const c = b[i];
        if (c == '|') {
            p.alts[p.nalts] = nitems;
            p.nalts += 1;
            i += 1;
            continue;
        }
        var it = Item{ .set = [_]bool{false} ** 256, .star = false };
        if (c == '[') {
            i += 1;
            const neg = b[i] == '^';
            if (neg) i += 1;
            while (b[i] != ']') : (i += 1) it.set[b[i]] = true;
            i += 1;
            if (neg) {
                for (&it.set) |*e| e.* = !e.*;
            }
        } else if (c == '\\') {
            it.set[b[i + 1]] = true;
            i += 2;
        } else if (c == '.') {
            for (&it.set, 0..) |*e, z| e.* = z != '\n';
            i += 1;
        } else {
            it.set[c] = true;
            i += 1;
        }
        if (i < b.len and b[i] == '*') {
            it.star = true;
            i += 1;
        }
        p.items[nitems] = it;
        nitems += 1;
    }
    p.alts[p.nalts] = nitems;
    p.first = [_]bool{false} ** 256;
    var a: usize = 0;
    while (a < p.nalts) : (a += 1) {
        const f = &p.items[p.alts[a]];
        for (&p.first, 0..) |*e, z| {
            if (f.star or f.set[z]) e.* = true;
        }
    }
    return p;
}

// where a match of items[0..] that starts at pos ends, or null
fn matchAt(items: []const Item, s: []const u8, pos: usize) ?usize {
    if (items.len == 0) return pos;
    const it = &items[0];
    if (it.star) {
        var j = pos;
        while (j < s.len and it.set[s[j]]) j += 1;
        while (true) {
            if (matchAt(items[1..], s, j)) |e| return e;
            if (j == pos) return null;
            j -= 1;
        }
    }
    if (pos < s.len and it.set[s[pos]]) return matchAt(items[1..], s, pos + 1);
    return null;
}

// where the match that starts at pos ends (the first alternative that
// matches), or null
fn findAt(p: *const Pat, s: []const u8, pos: usize) ?usize {
    if (!p.first[s[pos]]) return null;
    var a: usize = 0;
    while (a < p.nalts) : (a += 1) {
        if (matchAt(p.items[p.alts[a]..p.alts[a + 1]], s, pos)) |e| {
            if (e > pos) return e;
        }
    }
    return null;
}

fn countMatches(pattern: []const u8, s: []const u8) u64 {
    const p = compile(pattern);
    var n: u64 = 0;
    var pos: usize = 0;
    while (pos < s.len) {
        if (findAt(&p, s, pos)) |e| {
            n += 1;
            pos = e;
        } else {
            pos += 1;
        }
    }
    return n;
}

fn replaceAll(pattern: []const u8, s: []const u8, rep: []const u8) []u8 {
    const p = compile(pattern);
    var out: Bytes = .empty;
    out.ensureTotalCapacity(gpa, s.len) catch @panic("out of memory");
    var pos: usize = 0;
    while (pos < s.len) {
        if (findAt(&p, s, pos)) |e| {
            out.appendSlice(gpa, rep) catch @panic("out of memory");
            pos = e;
        } else {
            out.append(gpa, s[pos]) catch @panic("out of memory");
            pos += 1;
        }
    }
    return out.toOwnedSlice(gpa) catch @panic("out of memory");
}

fn randomSection(seed: *i64, chars: []const u8, probs: []const f64, m: usize) []u8 {
    var cum: [16]f64 = undefined;
    var p: f64 = 0.0;
    for (probs, 0..) |q, j| {
        p += q;
        cum[j] = p;
    }
    const last_i = probs.len - 1;
    var last = seed.*;
    const out = gpa.alloc(u8, m) catch @panic("out of memory");
    for (out) |*c| {
        last = @mod(last * 3877 + 29573, 139968);
        const r = @as(f64, @floatFromInt(last)) / 139968.0;
        var i: usize = 0;
        while (i < last_i and r >= cum[i]) i += 1;
        c.* = chars[i];
    }
    seed.* = last;
    return out;
}

// a header line, then the section in lines of 60 characters
fn putSection(out: *Bytes, title: []const u8, sec: []const u8) void {
    out.appendSlice(gpa, title) catch @panic("out of memory");
    out.append(gpa, '\n') catch @panic("out of memory");
    var i: usize = 0;
    while (i < sec.len) : (i += 60) {
        const e = @min(i + 60, sec.len);
        out.appendSlice(gpa, sec[i..e]) catch @panic("out of memory");
        out.append(gpa, '\n') catch @panic("out of memory");
    }
}

fn fastaText(n: usize) []u8 {
    var seed: i64 = 42;
    const iub = [_]f64{ 0.27, 0.12, 0.12, 0.27 } ++ [_]f64{0.02} ** 11;
    const homo = [_]f64{ 0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008 };
    const one = gpa.alloc(u8, 2 * n) catch @panic("out of memory");
    defer gpa.free(one);
    for (one, 0..) |*c, k| c.* = alu[k % alu.len];
    const two = randomSection(&seed, "acgtBDHKMNRSVWY", &iub, 3 * n);
    defer gpa.free(two);
    const three = randomSection(&seed, "acgt", &homo, 5 * n);
    defer gpa.free(three);
    var out: Bytes = .empty;
    putSection(&out, ">ONE Homo sapiens alu", one);
    putSection(&out, ">TWO IUB ambiguity codes", two);
    putSection(&out, ">THREE Homo sapiens frequency", three);
    return out.toOwnedSlice(gpa) catch @panic("out of memory");
}

fn fold(h: u64, v: u64) u64 {
    return (h * 31 + v) & 0xFFFFFFFF;
}

fn regexredux(n: usize) u64 {
    const text = fastaText(n);
    defer gpa.free(text);
    const seq = replaceAll(">.*\n|\n", text, "");
    defer gpa.free(seq);
    var h: u64 = 0;
    const variants = [_][]const u8{
        "agggtaaa|tttaccct",
        "[cgt]gggtaaa|tttaccc[acg]",
        "a[act]ggtaaa|tttacc[agt]t",
        "ag[act]gtaaa|tttac[agt]ct",
        "agg[act]taaa|ttta[agt]cct",
        "aggg[acg]aaa|ttt[cgt]ccct",
        "agggt[cgt]aa|tt[acg]accct",
        "agggta[cgt]a|t[acg]taccct",
        "agggtaa[cgt]|[acg]ttaccct",
    };
    for (variants) |v| h = fold(h, countMatches(v, seq));
    const subst = [_][2][]const u8{
        .{ "tHa[Nt]", "<4>" },
        .{ "aND|caN|Ha[DS]|WaS", "<3>" },
        .{ "a[NSt]|BY", "<2>" },
        .{ "<[^>]*>", "|" },
        .{ "\\|[^|][^|]*\\|", "-" },
    };
    var s = gpa.dupe(u8, seq) catch @panic("out of memory");
    for (subst) |sub| {
        const next = replaceAll(sub[0], s, sub[1]);
        gpa.free(s);
        s = next;
    }
    defer gpa.free(s);
    h = fold(h, text.len);
    h = fold(h, seq.len);
    return fold(h, s.len);
}

pub fn main() void {
    std.debug.print("{d}\n", .{regexredux(500000)});
}
