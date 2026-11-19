// The smallest program (roadmap BM7): see bench/size/fides/hello.fd.
const std = @import("std");
pub fn main() void {
    std.debug.print("hello\n", .{});
}
