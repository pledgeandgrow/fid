const std = @import("std");
pub fn main() !void {
    const n: usize = 20_000_000;
    const is = try std.heap.page_allocator.alloc(bool, n);
    defer std.heap.page_allocator.free(is);
    @memset(is, true);
    is[0] = false; is[1] = false;
    var p: usize = 2;
    while (p * p < n) : (p += 1) {
        if (is[p]) { var m = p * p; while (m < n) : (m += p) is[m] = false; }
    }
    var count: usize = 0;
    for (is) |b| { if (b) count += 1; }
    std.debug.print("{d}\n", .{count});
}
