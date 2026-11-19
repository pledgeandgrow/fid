// fileread (BM4): see bench/work/fides/fileread.fd. Reads a 50 MB text
// file three times and sums the numbers in it; writes the file first when
// it is not there.
const std = @import("std");

const path = "out/fileread.dat";
const lines: i64 = 2150000;
const size: u64 = 49994158;
const gpa = std.heap.smp_allocator;

fn generate(io: std.Io) !void {
    var o: std.ArrayListUnmanaged(u8) = .empty;
    defer o.deinit(gpa);
    try o.ensureTotalCapacity(gpa, size);
    var s: i64 = 1;
    var line: [64]u8 = undefined;
    var k: i64 = 0;
    while (k < lines) : (k += 1) {
        s = @mod(s * 1103515245 + 12345, 2147483648);
        const r1 = s >> 8;
        s = @mod(s * 1103515245 + 12345, 2147483648);
        const r2 = s >> 8;
        const t = try std.fmt.bufPrint(&line, "rec{d} {d} x{d}\n", .{ k, @mod(r1, 1000000), @mod(r2, 10000) });
        try o.appendSlice(gpa, t);
    }
    try std.Io.Dir.cwd().writeFile(io, .{ .sub_path = path, .data = o.items });
}

fn scan(d: []const u8) i64 {
    var tot: i64 = 0;
    var cur: i64 = 0;
    var in = false;
    var nl: i64 = 0;
    for (d) |b| {
        if (b >= '0' and b <= '9') {
            cur = cur * 10 + (b - 48);
            in = true;
        } else {
            if (in) {
                tot += cur;
                cur = 0;
                in = false;
            }
            if (b == '\n') nl += 1;
        }
    }
    if (in) tot += cur;
    return tot + nl;
}

pub fn main(init: std.process.Init) !void {
    const io = init.io;
    const cwd = std.Io.Dir.cwd();
    const have: u64 = if (cwd.statFile(io, path, .{})) |st| st.size else |_| 0;
    if (have != size) try generate(io);
    var total: i64 = 0;
    var r: usize = 0;
    while (r < 3) : (r += 1) {
        const d = try cwd.readFileAlloc(io, path, gpa, .unlimited);
        defer gpa.free(d);
        total += scan(d);
    }
    std.debug.print("{d}\n", .{total});
}
