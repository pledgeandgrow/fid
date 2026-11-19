// k-nucleotide (Benchmarks Game, BM3): see bench/game/fides/knucleotide.fd.
const std = @import("std");

const gpa = std.heap.smp_allocator;
const Map = std.AutoHashMapUnmanaged(u64, u64);

// the THREE section as codes: the index of each drawn character in "acgt"
fn threeCodes(n: usize) []u8 {
    var last: i64 = 42;
    var k: usize = 0;
    while (k < 3 * n) : (k += 1) last = @mod(last * 3877 + 29573, 139968);
    const probs = [_]f64{ 0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008 };
    var cum: [4]f64 = undefined;
    var p: f64 = 0.0;
    for (probs, 0..) |q, j| {
        p += q;
        cum[j] = p;
    }
    const last_i = probs.len - 1;
    const out = gpa.alloc(u8, 5 * n) catch @panic("out of memory");
    for (out) |*c| {
        last = @mod(last * 3877 + 29573, 139968);
        const r = @as(f64, @floatFromInt(last)) / 139968.0;
        var i: usize = 0;
        while (i < last_i and r >= cum[i]) i += 1;
        c.* = @intCast(i);
    }
    return out;
}

fn countKmers(codes: []const u8, k: usize) Map {
    const mask: u64 = (@as(u64, 1) << @intCast(2 * k)) - 1;
    var m: Map = .empty;
    var key: u64 = 0;
    for (codes, 0..) |c, i| {
        key = ((key << 2) | c) & mask;
        if (i + 1 >= k) {
            const gop = m.getOrPut(gpa, key) catch @panic("out of memory");
            if (!gop.found_existing) gop.value_ptr.* = 0;
            gop.value_ptr.* += 1;
        }
    }
    return m;
}

fn encode(s: []const u8) u64 {
    var key: u64 = 0;
    for (s) |c| {
        const v: u64 = switch (c) {
            'C' => 1,
            'G' => 2,
            'T' => 3,
            else => 0,
        };
        key = (key << 2) | v;
    }
    return key;
}

fn fold(h: u64, v: u64) u64 {
    return (h * 31 + v) & 0xFFFFFFFF;
}

fn frequencies(h0: u64, codes: []const u8, k: usize) u64 {
    var m = countKmers(codes, k);
    defer m.deinit(gpa);
    var h = fold(h0, m.count());
    var key: u64 = 0;
    while (key < (@as(u64, 1) << @intCast(2 * k))) : (key += 1) h = fold(h, m.get(key) orelse 0);
    return h;
}

fn countOf(h0: u64, codes: []const u8, s: []const u8) u64 {
    var m = countKmers(codes, s.len);
    defer m.deinit(gpa);
    const h = fold(h0, m.count());
    return fold(h, m.get(encode(s)) orelse 0);
}

fn knucleotide(n: usize) u64 {
    const codes = threeCodes(n);
    defer gpa.free(codes);
    var h = frequencies(0, codes, 1);
    h = frequencies(h, codes, 2);
    const queries = [_][]const u8{ "GGT", "GGTA", "GGTATT", "GGTATTTTAATT", "GGTATTTTAATTTATAGT" };
    for (queries) |s| h = countOf(h, codes, s);
    return h;
}

pub fn main() void {
    std.debug.print("{d}\n", .{knucleotide(1000000)});
}
