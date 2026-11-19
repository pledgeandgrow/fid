// reverse-complement (Benchmarks Game, BM3): see bench/game/fides/revcomp.fd.
const std = @import("std");

const alu = "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

const gpa = std.heap.smp_allocator;

fn repeatSection(m: usize) []u8 {
    const out = gpa.alloc(u8, m) catch @panic("out of memory");
    for (out, 0..) |*c, k| c.* = alu[k % alu.len];
    return out;
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

fn compTable() [256]u8 {
    var t: [256]u8 = undefined;
    for (&t, 0..) |*e, c| e.* = @intCast(c);
    const from = "ACBDGHKMNSRUTWVYacbdghkmnsrutwvy";
    const to = "TGVHCDMKNSYAAWBRTGVHCDMKNSYAAWBR";
    for (from, to) |f, c| t[f] = c;
    return t;
}

// in place: two indexes meet in the middle
fn reverseComplement(seq: []u8, t: *const [256]u8) void {
    if (seq.len == 0) return;
    var i: usize = 0;
    var j: usize = seq.len - 1;
    while (i < j) {
        const a = seq[i];
        const b = seq[j];
        seq[i] = t[b];
        seq[j] = t[a];
        i += 1;
        j -= 1;
    }
    if (i == j) seq[i] = t[seq[i]];
}

fn hashSeq(h0: u64, seq: []const u8) u64 {
    var h = h0;
    for (seq) |c| h = (h * 31 + c) & 0xFFFFFFFF;
    return h;
}

fn revcomp(n: usize) u64 {
    const t = compTable();
    var seed: i64 = 42;
    const iub = [_]f64{ 0.27, 0.12, 0.12, 0.27 } ++ [_]f64{0.02} ** 11;
    const homo = [_]f64{ 0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008 };
    const one = repeatSection(2 * n);
    defer gpa.free(one);
    const two = randomSection(&seed, "acgtBDHKMNRSVWY", &iub, 3 * n);
    defer gpa.free(two);
    const three = randomSection(&seed, "acgt", &homo, 5 * n);
    defer gpa.free(three);
    reverseComplement(one, &t);
    reverseComplement(two, &t);
    reverseComplement(three, &t);
    var h = hashSeq(0, one);
    h = hashSeq(h, two);
    return hashSeq(h, three);
}

pub fn main() void {
    std.debug.print("{d}\n", .{revcomp(2500000)});
}
