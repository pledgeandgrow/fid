const std = @import("std");
pub fn main() void {
    var n: i64 = 2_000_000_000;
    _ = &n;
    var s: i64 = 0;
    var i: i64 = 0;
    while (i < n) : (i += 1) s += @mod(i, 7) * 3 - 1;
    std.debug.print("{d}\n", .{s});
}
