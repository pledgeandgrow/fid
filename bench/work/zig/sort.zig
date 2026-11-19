// sort (BM4): see bench/work/fides/sort.fd.
const std = @import("std");

pub fn main() !void {
    const n: usize = 5000000;
    const alloc = std.heap.page_allocator;
    const v = try alloc.alloc(i64, n);
    defer alloc.free(v);
    var x: i64 = 12345;
    for (v) |*e| {
        x = @mod(x * 1103515245 + 12345, 2147483648);
        e.* = x;
    }
    std.mem.sort(i64, v, {}, std.sort.asc(i64));
    var s: i64 = 0;
    for (v, 0..) |e, i| {
        s = @mod(s + e * @as(i64, @intCast(i % 1000)), 1000000007);
    }
    std.debug.print("{d}\n", .{s});
}
