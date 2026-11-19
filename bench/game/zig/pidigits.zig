// pidigits (Benchmarks Game, BM3): see bench/game/fides/pidigits.fd.
// A small signed big integer here, 32-bit limbs, the same as the Rust and
// C++ programs' (add, multiply by a word, compare, a quotient below 10 by
// subtraction): std.math.big would do, but the three hand-written ones stay
// alike.
const std = @import("std");

const gpa = std.heap.smp_allocator;
const Mag = std.ArrayListUnmanaged(u32); // little-endian, no high zero limbs; zero is empty

const Big = struct {
    neg: bool = false,
    mag: Mag = .empty,
};

fn cmpMag(a: []const u32, b: []const u32) std.math.Order {
    if (a.len != b.len) return std.math.order(a.len, b.len);
    var i = a.len;
    while (i > 0) {
        i -= 1;
        if (a[i] != b[i]) return std.math.order(a[i], b[i]);
    }
    return .eq;
}

fn trim(a: *Mag) void {
    while (a.items.len > 0 and a.items[a.items.len - 1] == 0) a.items.len -= 1;
}

fn push(a: *Mag, v: u32) void {
    a.append(gpa, v) catch @panic("out of memory");
}

fn resize(a: *Mag, n: usize) void {
    const old = a.items.len;
    a.resize(gpa, n) catch @panic("out of memory");
    if (n > old) @memset(a.items[old..], 0);
}

// a += b
fn addMag(a: *Mag, b: []const u32) void {
    if (a.items.len < b.len) resize(a, b.len);
    var carry: u64 = 0;
    for (a.items, 0..) |*x, i| {
        if (i >= b.len and carry == 0) break;
        const s = @as(u64, x.*) + (if (i < b.len) @as(u64, b[i]) else 0) + carry;
        x.* = @truncate(s);
        carry = s >> 32;
    }
    if (carry != 0) push(a, @intCast(carry));
}

// a -= b, a >= b
fn subMag(a: *Mag, b: []const u32) void {
    var borrow: i64 = 0;
    for (a.items, 0..) |*x, i| {
        if (i >= b.len and borrow == 0) break;
        const d = @as(i64, x.*) - (if (i < b.len) @as(i64, b[i]) else 0) - borrow;
        if (d < 0) {
            x.* = @intCast(d + (@as(i64, 1) << 32));
            borrow = 1;
        } else {
            x.* = @intCast(d);
            borrow = 0;
        }
    }
    trim(a);
}

// a = b - a, b > a
fn rsubMag(a: *Mag, b: []const u32) void {
    resize(a, b.len);
    var borrow: i64 = 0;
    for (a.items, 0..) |*x, i| {
        const d = @as(i64, b[i]) - @as(i64, x.*) - borrow;
        if (d < 0) {
            x.* = @intCast(d + (@as(i64, 1) << 32));
            borrow = 1;
        } else {
            x.* = @intCast(d);
            borrow = 0;
        }
    }
    trim(a);
}

// x += b, or -= b when b_neg
fn add(x: *Big, b_neg: bool, b: []const u32) void {
    if (x.mag.items.len == 0) x.neg = b_neg;
    if (x.neg == b_neg) {
        addMag(&x.mag, b);
    } else if (cmpMag(x.mag.items, b) != .lt) {
        subMag(&x.mag, b);
    } else {
        rsubMag(&x.mag, b);
        x.neg = b_neg;
    }
    if (x.mag.items.len == 0) x.neg = false;
}

fn mulSmall(x: *Big, m: u32) void {
    if (m == 0) {
        x.mag.items.len = 0;
        x.neg = false;
        return;
    }
    var carry: u64 = 0;
    for (x.mag.items) |*l| {
        const p = @as(u64, l.*) * m + carry;
        l.* = @truncate(p);
        carry = p >> 32;
    }
    if (carry != 0) push(&x.mag, @intCast(carry));
}

fn gt(a: *const Big, b: *const Big) bool {
    if (a.neg != b.neg) return !a.neg;
    const c = cmpMag(a.mag.items, b.mag.items);
    return if (a.neg) c == .lt else c == .gt;
}

fn copy(dst: *Big, src: *const Big) void {
    dst.neg = src.neg;
    dst.mag.clearRetainingCapacity();
    dst.mag.appendSlice(gpa, src.mag.items) catch @panic("out of memory");
}

// (num * nth + acc) / den, the quotient being a digit (num * nth + acc >= 0)
fn extract(num: *const Big, acc: *const Big, den: *const Big, nth: u32, t: *Big) u32 {
    copy(t, num);
    mulSmall(t, nth);
    add(t, acc.neg, acc.mag.items);
    var q: u32 = 0;
    while (cmpMag(t.mag.items, den.mag.items) != .lt) {
        subMag(&t.mag, den.mag.items);
        q += 1;
    }
    return q;
}

fn pidigits(count: usize) u64 {
    var num: Big = .{};
    var acc: Big = .{};
    var den: Big = .{};
    var t: Big = .{};
    push(&num.mag, 1);
    push(&den.mag, 1);
    var k: u32 = 0;
    var i: usize = 0;
    var h: u64 = 0;
    while (i < count) {
        k += 1;
        const k2 = 2 * k + 1;
        copy(&t, &num);
        mulSmall(&t, 2);
        add(&acc, false, t.mag.items);
        mulSmall(&acc, k2);
        mulSmall(&den, k2);
        mulSmall(&num, k);
        if (gt(&num, &acc)) continue;
        const d = extract(&num, &acc, &den, 3, &t);
        if (d != extract(&num, &acc, &den, 4, &t)) continue;
        h = (h * 31 + d) & 0xFFFFFFFF;
        i += 1;
        copy(&t, &den);
        mulSmall(&t, d);
        add(&acc, true, t.mag.items);
        mulSmall(&acc, 10);
        mulSmall(&num, 10);
    }
    num.mag.deinit(gpa);
    acc.mag.deinit(gpa);
    den.mag.deinit(gpa);
    t.mag.deinit(gpa);
    return h;
}

pub fn main() void {
    std.debug.print("{d}\n", .{pidigits(10000)});
}
