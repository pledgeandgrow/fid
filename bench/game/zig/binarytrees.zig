// binary-trees (Benchmarks Game, BM3): see bench/game/fides/binarytrees.fd.
// Each node is its own allocation from the general-purpose allocator
// (std.heap.smp_allocator), freed when its tree is dropped; a Leaf is null.
const std = @import("std");

const Node = struct {
    l: ?*Node,
    r: ?*Node,
};

const gpa = std.heap.smp_allocator;

fn make(d: u32) *Node {
    const n = gpa.create(Node) catch @panic("out of memory");
    if (d == 0) {
        n.* = .{ .l = null, .r = null };
    } else {
        n.* = .{ .l = make(d - 1), .r = make(d - 1) };
    }
    return n;
}

fn check(t: ?*Node) i64 {
    const n = t orelse return 0;
    return 1 + check(n.l) + check(n.r);
}

fn drop(t: ?*Node) void {
    const n = t orelse return;
    drop(n.l);
    drop(n.r);
    gpa.destroy(n);
}

fn trees(min_depth: u32, max_depth: u32) i64 {
    const s = make(max_depth + 1);
    var total = check(s);
    drop(s);
    const long_lived = make(max_depth);
    var d = min_depth;
    while (d <= max_depth) : (d += 2) {
        const iters = @as(i64, 1) << @intCast(max_depth - d + min_depth);
        var i: i64 = 0;
        while (i < iters) : (i += 1) {
            const t = make(d);
            total += check(t);
            drop(t);
        }
    }
    total += check(long_lived);
    drop(long_lived);
    return total;
}

pub fn main() void {
    std.debug.print("{d}\n", .{trees(4, 16)});
}
