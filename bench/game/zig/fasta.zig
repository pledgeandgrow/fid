// fasta (Benchmarks Game, BM3): see bench/game/fides/fasta.fd.
const std = @import("std");

const alu = "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

fn randomHash(h0: u64, seed: *i64, chars: []const u8, probs: []const f64, m: usize) u64 {
    var cum: [16]f64 = undefined;
    var p: f64 = 0.0;
    for (probs, 0..) |q, j| {
        p += q;
        cum[j] = p;
    }
    const last_i = probs.len - 1;
    var last = seed.*;
    var h = h0;
    var k: usize = 0;
    while (k < m) : (k += 1) {
        last = @mod(last * 3877 + 29573, 139968);
        const r = @as(f64, @floatFromInt(last)) / 139968.0;
        var i: usize = 0;
        while (i < last_i and r >= cum[i]) i += 1;
        h = (h * 31 + chars[i]) & 0xFFFFFFFF;
    }
    seed.* = last;
    return h;
}

fn fasta(n: usize) u64 {
    var h: u64 = 0;
    var k: usize = 0;
    while (k < 2 * n) : (k += 1) h = (h * 31 + alu[k % alu.len]) & 0xFFFFFFFF;
    var seed: i64 = 42;
    const iub = [_]f64{ 0.27, 0.12, 0.12, 0.27 } ++ [_]f64{0.02} ** 11;
    h = randomHash(h, &seed, "acgtBDHKMNRSVWY", &iub, 3 * n);
    const homo = [_]f64{ 0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008 };
    return randomHash(h, &seed, "acgt", &homo, 5 * n);
}

pub fn main() void {
    std.debug.print("{d}\n", .{fasta(2500000)});
}
