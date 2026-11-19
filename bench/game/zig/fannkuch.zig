// fannkuch-redux (Benchmarks Game, BM3): see bench/game/fides/fannkuch.fd.
const std = @import("std");

fn fannkuch(comptime n: usize) i64 {
    var perm1: [n]usize = undefined;
    for (&perm1, 0..) |*p, i| p.* = i;
    var count = [_]usize{0} ** n;
    var perm: [n]usize = undefined;
    var maxflips: i64 = 0;
    var checksum: i64 = 0;
    var permcount: i64 = 0;
    var r: usize = n;
    while (true) {
        while (r != 1) : (r -= 1) count[r - 1] = r;
        perm = perm1;
        var flips: i64 = 0;
        var k = perm[0];
        while (k != 0) : (k = perm[0]) {
            std.mem.reverse(usize, perm[0 .. k + 1]);
            flips += 1;
        }
        if (flips > maxflips) maxflips = flips;
        if (@mod(permcount, 2) == 0) {
            checksum += flips;
        } else {
            checksum -= flips;
        }
        while (true) {
            if (r == n) return checksum * 1000 + maxflips;
            std.mem.rotate(usize, perm1[0 .. r + 1], 1);
            count[r] -= 1;
            if (count[r] > 0) break;
            r += 1;
        }
        permcount += 1;
    }
}

pub fn main() void {
    std.debug.print("{d}\n", .{fannkuch(11)});
}
