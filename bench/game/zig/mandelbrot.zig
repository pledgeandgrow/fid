// mandelbrot (Benchmarks Game, BM3): see bench/game/fides/mandelbrot.fd.
const std = @import("std");

pub fn main() void {
    const n: usize = 4000;
    const nf: f64 = @floatFromInt(n);
    var count: u64 = 0;
    for (0..n) |y| {
        const ci: f64 = 2.0 * @as(f64, @floatFromInt(y)) / nf - 1.0;
        for (0..n) |x| {
            const cr: f64 = 2.0 * @as(f64, @floatFromInt(x)) / nf - 1.5;
            var zr: f64 = 0.0;
            var zi: f64 = 0.0;
            var tr: f64 = 0.0;
            var ti: f64 = 0.0;
            var i: u32 = 0;
            while (i < 50 and tr + ti <= 4.0) : (i += 1) {
                zi = 2.0 * zr * zi + ci;
                zr = tr - ti + cr;
                tr = zr * zr;
                ti = zi * zi;
            }
            if (tr + ti <= 4.0) count += 1;
        }
    }
    std.debug.print("{d}\n", .{count});
}
