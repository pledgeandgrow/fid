// spectral-norm (Benchmarks Game, BM3): see bench/game/fides/spectral.fd.
const std = @import("std");

fn a(i: usize, j: usize) f64 {
    return 1.0 / @as(f64, @floatFromInt((i + j) * (i + j + 1) / 2 + i + 1));
}

fn mulAv(v: []const f64, out: []f64) void {
    for (out, 0..) |*o, i| {
        var s: f64 = 0.0;
        for (v, 0..) |vj, j| s += a(i, j) * vj;
        o.* = s;
    }
}

fn mulAtv(v: []const f64, out: []f64) void {
    for (out, 0..) |*o, i| {
        var s: f64 = 0.0;
        for (v, 0..) |vj, j| s += a(j, i) * vj;
        o.* = s;
    }
}

fn mulAtAv(v: []const f64, out: []f64, tmp: []f64) void {
    mulAv(v, tmp);
    mulAtv(tmp, out);
}

pub fn main() !void {
    const n: usize = 2000;
    const alloc = std.heap.page_allocator;
    const u = try alloc.alloc(f64, n);
    defer alloc.free(u);
    const v = try alloc.alloc(f64, n);
    defer alloc.free(v);
    const tmp = try alloc.alloc(f64, n);
    defer alloc.free(tmp);
    @memset(u, 1.0);
    @memset(v, 0.0);
    var k: usize = 0;
    while (k < 10) : (k += 1) {
        mulAtAv(u, v, tmp);
        mulAtAv(v, u, tmp);
    }
    var vbv: f64 = 0.0;
    var vv: f64 = 0.0;
    for (u, v) |ui, vi| {
        vbv += ui * vi;
        vv += vi * vi;
    }
    const r: i64 = @intFromFloat(@sqrt(vbv / vv) * 1e9);
    std.debug.print("{d}\n", .{r});
}
