// n-body (Benchmarks Game, BM3): see bench/game/fides/nbody.fd.
const std = @import("std");

// Typed f64 constants: a comptime_float product would be computed in
// higher precision and round differently from the other languages.
const pi: f64 = 3.141592653589793;
const solar_mass: f64 = 4.0 * pi * pi;
const days_per_year: f64 = 365.24;

const Body = struct { x: f64, y: f64, z: f64, vx: f64, vy: f64, vz: f64, m: f64 };

fn energy(b: *const [5]Body) f64 {
    var e: f64 = 0.0;
    for (0..5) |i| {
        e += 0.5 * b[i].m * (b[i].vx * b[i].vx + b[i].vy * b[i].vy + b[i].vz * b[i].vz);
        for (i + 1..5) |j| {
            const dx = b[i].x - b[j].x;
            const dy = b[i].y - b[j].y;
            const dz = b[i].z - b[j].z;
            e -= b[i].m * b[j].m / @sqrt(dx * dx + dy * dy + dz * dz);
        }
    }
    return e;
}

fn advance(b: *[5]Body, dt: f64) void {
    for (0..5) |i| {
        for (i + 1..5) |j| {
            const dx = b[i].x - b[j].x;
            const dy = b[i].y - b[j].y;
            const dz = b[i].z - b[j].z;
            const d2 = dx * dx + dy * dy + dz * dz;
            const mag = dt / (d2 * @sqrt(d2));
            const mj = b[j].m * mag;
            const mi = b[i].m * mag;
            b[i].vx -= dx * mj;
            b[i].vy -= dy * mj;
            b[i].vz -= dz * mj;
            b[j].vx += dx * mi;
            b[j].vy += dy * mi;
            b[j].vz += dz * mi;
        }
    }
    for (b) |*bi| {
        bi.x += dt * bi.vx;
        bi.y += dt * bi.vy;
        bi.z += dt * bi.vz;
    }
}

pub fn main() void {
    var b = [5]Body{
        .{ .x = 0, .y = 0, .z = 0, .vx = 0, .vy = 0, .vz = 0, .m = solar_mass },
        .{ .x = 4.84143144246472090e+00, .y = -1.16032004402742839e+00, .z = -1.03622044471123109e-01, .vx = 1.66007664274403694e-03 * days_per_year, .vy = 7.69901118419740425e-03 * days_per_year, .vz = -6.90460016972063023e-05 * days_per_year, .m = 9.54791938424326609e-04 * solar_mass },
        .{ .x = 8.34336671824457987e+00, .y = 4.12479856412430479e+00, .z = -4.03523417114321381e-01, .vx = -2.76742510726862411e-03 * days_per_year, .vy = 4.99852801234917238e-03 * days_per_year, .vz = 2.30417297573763929e-05 * days_per_year, .m = 2.85885980666130812e-04 * solar_mass },
        .{ .x = 1.28943695621391310e+01, .y = -1.51111514016986312e+01, .z = -2.23307578892655734e-01, .vx = 2.96460137564761618e-03 * days_per_year, .vy = 2.37847173959480950e-03 * days_per_year, .vz = -2.96589568540237556e-05 * days_per_year, .m = 4.36624404335156298e-05 * solar_mass },
        .{ .x = 1.53796971148509165e+01, .y = -2.59193146099879641e+01, .z = 1.79258772950371181e-01, .vx = 2.68067772490389322e-03 * days_per_year, .vy = 1.62824170038242295e-03 * days_per_year, .vz = -9.51592254519715870e-05 * days_per_year, .m = 5.15138902046611451e-05 * solar_mass },
    };
    var px: f64 = 0.0;
    var py: f64 = 0.0;
    var pz: f64 = 0.0;
    for (b) |bi| {
        px += bi.vx * bi.m;
        py += bi.vy * bi.m;
        pz += bi.vz * bi.m;
    }
    b[0].vx = -px / solar_mass;
    b[0].vy = -py / solar_mass;
    b[0].vz = -pz / solar_mass;
    var s: usize = 0;
    while (s < 5000000) : (s += 1) advance(&b, 0.01);
    const e: i64 = @intFromFloat(energy(&b) * 1e9);
    std.debug.print("{d}\n", .{e});
}
