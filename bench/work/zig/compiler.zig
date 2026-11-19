// compiler (BM4, real work): see bench/work/fides/compiler.fd. Nodes come
// one at a time from the general-purpose allocator (std.heap.smp_allocator)
// and each line's tree is freed after it is evaluated.
const std = @import("std");

const gpa = std.heap.smp_allocator;

const Expr = struct {
    kind: u8, // 0 number, 1 variable, 2 binary
    val: i64,
    op: u8,
    l: ?*Expr,
    r: ?*Expr,
};

const Buf = std.ArrayListUnmanaged(u8);

fn rnd(s: *i64) i64 {
    s.* = @mod(s.* * 1103515245 + 12345, 2147483648);
    return s.* >> 8;
}

fn putDec(out: *Buf, v: i64) void {
    if (v >= 10) putDec(out, @divTrunc(v, 10));
    out.append(gpa, @intCast(48 + @mod(v, 10))) catch @panic("oom");
}

fn genExpr(out: *Buf, s: *i64, d: u32, k: i64) void {
    const r = rnd(s);
    if (d == 0 or @mod(r, 4) == 0) {
        const r2 = rnd(s);
        if (k > 0 and @mod(r2, 5) == 0) {
            out.append(gpa, 'v') catch @panic("oom");
            putDec(out, @mod(r2, k));
        } else {
            putDec(out, @mod(r2, 1000));
        }
        return;
    }
    out.append(gpa, '(') catch @panic("oom");
    genExpr(out, s, d - 1, k);
    const op = "+-*"[@intCast(@mod(rnd(s), 3))];
    out.appendSlice(gpa, &[_]u8{ ' ', op, ' ' }) catch @panic("oom");
    genExpr(out, s, d - 1, k);
    out.append(gpa, ')') catch @panic("oom");
}

fn generate(lines: i64, depth: u32) Buf {
    var out: Buf = .empty;
    var s: i64 = 1;
    var k: i64 = 0;
    while (k < lines) : (k += 1) {
        out.appendSlice(gpa, "let v") catch @panic("oom");
        putDec(&out, k);
        out.appendSlice(gpa, " = ") catch @panic("oom");
        genExpr(&out, &s, depth, k);
        out.appendSlice(gpa, ";\n") catch @panic("oom");
    }
    return out;
}

// token kinds: 1 number, 2 variable, 3 `let`, 4 punctuation (its byte)
const Parser = struct {
    kinds: std.ArrayListUnmanaged(u8) = .empty,
    vals: std.ArrayListUnmanaged(i64) = .empty,
    pos: usize = 0,

    fn push(p: *Parser, kind: u8, v: i64) void {
        p.kinds.append(gpa, kind) catch @panic("oom");
        p.vals.append(gpa, v) catch @panic("oom");
    }

    fn lex(p: *Parser, src: []const u8) void {
        const n = src.len;
        var i: usize = 0;
        while (i < n) {
            const c = src[i];
            if (c == ' ' or c == '\n') {
                i += 1;
            } else if ((c >= '0' and c <= '9') or c == 'v') {
                var kind: u8 = 1;
                if (c == 'v') {
                    kind = 2;
                    i += 1;
                }
                var v: i64 = 0;
                while (i < n and src[i] >= '0' and src[i] <= '9') : (i += 1) v = v * 10 + (src[i] - '0');
                p.push(kind, v);
            } else if (c == 'l') {
                i += 3;
                p.push(3, 0);
            } else {
                p.push(4, c);
                i += 1;
            }
        }
        p.push(0, 0);
    }

    fn isOp(p: *const Parser, op: u8) bool {
        return p.kinds.items[p.pos] == 4 and p.vals.items[p.pos] == op;
    }

    fn node(kind: u8, val: i64, op: u8, l: ?*Expr, r: ?*Expr) *Expr {
        const e = gpa.create(Expr) catch @panic("oom");
        e.* = .{ .kind = kind, .val = val, .op = op, .l = l, .r = r };
        return e;
    }

    fn atom(p: *Parser) *Expr {
        const q = p.pos;
        p.pos += 1;
        switch (p.kinds.items[q]) {
            1 => return node(0, p.vals.items[q], 0, null, null),
            2 => return node(1, p.vals.items[q], 0, null, null),
            else => {
                const e = p.expr();
                p.pos += 1;
                return e;
            },
        }
    }

    fn term(p: *Parser) *Expr {
        var a = p.atom();
        while (p.isOp('*')) {
            p.pos += 1;
            a = node(2, 0, '*', a, p.atom());
        }
        return a;
    }

    fn expr(p: *Parser) *Expr {
        var a = p.term();
        while (p.isOp('+') or p.isOp('-')) {
            const op: u8 = @intCast(p.vals.items[p.pos]);
            p.pos += 1;
            a = node(2, 0, op, a, p.term());
        }
        return a;
    }
};

fn eval(e: *const Expr, env: []const i64) i64 {
    switch (e.kind) {
        0 => return e.val,
        1 => return env[@intCast(e.val)],
        else => {
            const a = eval(e.l.?, env);
            const b = eval(e.r.?, env);
            return switch (e.op) {
                '+' => @mod(a + b, 1000003),
                '-' => @mod(a - b + 1000003, 1000003),
                else => @mod(a * b, 1000003),
            };
        },
    }
}

fn drop(e: ?*Expr) void {
    const n = e orelse return;
    drop(n.l);
    drop(n.r);
    gpa.destroy(n);
}

fn compileRun(src: []const u8) i64 {
    var p: Parser = .{};
    p.lex(src);
    var env: std.ArrayListUnmanaged(i64) = .empty;
    var total: i64 = 0;
    while (p.kinds.items[p.pos] == 3) {
        p.pos += 3;
        const e = p.expr();
        p.pos += 1;
        const x = eval(e, env.items);
        drop(e);
        env.append(gpa, x) catch @panic("oom");
        total += x;
    }
    return total;
}

pub fn main() void {
    const src = generate(50000, 8);
    std.debug.print("{d}\n", .{compileRun(src.items)});
}
