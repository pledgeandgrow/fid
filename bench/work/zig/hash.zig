// hash (BM4): see bench/work/fides/hash.fd.
const std = @import("std");

fn fnv(data: []const u8, rounds: usize) u64 {
    var h: u64 = 0xcbf29ce484222325;
    var r: usize = 0;
    while (r < rounds) : (r += 1) {
        for (data) |b| h = (h ^ @as(u64, b)) *% 0x100000001b3;
    }
    return h;
}

pub fn main() !void {
    const n: usize = 50000000;
    const alloc = std.heap.page_allocator;
    const data = try alloc.alloc(u8, n);
    defer alloc.free(data);
    for (data, 0..) |*d, i| d.* = @intCast((i * 7 + i / 256) % 256);
    const h: i64 = @bitCast(fnv(data, 4));
    std.debug.print("{d}\n", .{h});
}
