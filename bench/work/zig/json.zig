// json (BM4): see bench/work/fides/json.fd. A hand-written parser into an
// arena of nodes and a compact printer, not std.json.
const std = @import("std");

const gpa = std.heap.smp_allocator;
const Buf = std.ArrayListUnmanaged(u8);
const List = std.ArrayListUnmanaged(i64);

var seed: i64 = 1;

fn rnd() i64 {
    seed = @mod(seed * 1103515245 + 12345, 2147483648);
    return seed >> 8;
}

fn put(o: *Buf, s: []const u8) void {
    o.appendSlice(gpa, s) catch @panic("oom");
}

fn putByte(o: *Buf, b: u8) void {
    o.append(gpa, b) catch @panic("oom");
}

fn putInt(o: *Buf, v: i64) void {
    var tmp: [24]u8 = undefined;
    const t = std.fmt.bufPrint(&tmp, "{d}", .{v}) catch unreachable;
    put(o, t);
}

fn genString(o: *Buf) void {
    putByte(o, '"');
    const n = @mod(rnd(), 12);
    var i: i64 = 0;
    while (i < n) : (i += 1) {
        const c = @mod(rnd(), 40);
        if (c < 26) {
            putByte(o, @intCast(97 + c));
        } else if (c == 26) {
            put(o, "\\n");
        } else if (c == 27) {
            put(o, "\\\"");
        } else if (c == 28) {
            put(o, "\\\\");
        } else if (c == 29) {
            put(o, "\\u00e9");
        } else if (c == 30) {
            put(o, "\\t");
        } else if (c == 31) {
            put(o, "\\u0041");
        } else if (c < 36) {
            putByte(o, ' ');
        } else {
            putByte(o, @intCast(48 + c - 36));
        }
    }
    putByte(o, '"');
}

fn genValue(o: *Buf, d: i64) void {
    var r = @mod(rnd(), 8);
    if (d == 0) r = @mod(r, 5);
    if (r == 0) {
        const v = @mod(rnd(), 2000000) - 1000000;
        putInt(o, v);
    } else if (r == 1 or r == 4) {
        genString(o);
    } else if (r == 2) {
        const t = @mod(rnd(), 3);
        put(o, if (t == 0) "true" else if (t == 1) "false" else "null");
    } else if (r == 3) {
        const v = @mod(rnd(), 100);
        putInt(o, v);
    } else if (r == 5 or r == 6) {
        const n = @mod(rnd(), 5);
        putByte(o, '[');
        var i: i64 = 0;
        while (i < n) : (i += 1) {
            if (i > 0) put(o, ", ");
            genValue(o, d - 1);
        }
        putByte(o, ']');
    } else {
        const n = @mod(rnd(), 5);
        putByte(o, '{');
        var i: i64 = 0;
        while (i < n) : (i += 1) {
            if (i > 0) put(o, ",\n  ");
            genString(o);
            put(o, ": ");
            genValue(o, d - 1);
        }
        putByte(o, '}');
    }
}

fn generate(items: i64, depth: i64) Buf {
    var o: Buf = .empty;
    putByte(&o, '[');
    var i: i64 = 0;
    while (i < items) : (i += 1) {
        if (i > 0) put(&o, ",\n");
        genValue(&o, depth);
    }
    putByte(&o, ']');
    putByte(&o, '\n');
    return o;
}

fn push(l: *List, v: i64) void {
    l.append(gpa, v) catch @panic("oom");
}

// node kinds: 0 null, 1 false, 2 true, 3 number, 4 string, 5 array, 6 object
const Parser = struct {
    t: []const u8,
    pos: usize = 0,
    kind: List = .empty,
    num: List = .empty,
    first: List = .empty,
    next: List = .empty,
    key: List = .empty,
    pool: Buf = .empty,
    ps: List = .empty,
    pl: List = .empty,

    fn deinit(p: *Parser) void {
        p.kind.deinit(gpa);
        p.num.deinit(gpa);
        p.first.deinit(gpa);
        p.next.deinit(gpa);
        p.key.deinit(gpa);
        p.pool.deinit(gpa);
        p.ps.deinit(gpa);
        p.pl.deinit(gpa);
    }

    fn ws(p: *Parser) void {
        while (true) {
            const c = p.t[p.pos];
            if (c != ' ' and c != '\n' and c != '\r' and c != '\t') return;
            p.pos += 1;
        }
    }

    fn node(p: *Parser, k: i64, v: i64) i64 {
        push(&p.kind, k);
        push(&p.num, v);
        push(&p.first, -1);
        push(&p.next, -1);
        push(&p.key, -1);
        return @as(i64, @intCast(p.kind.items.len)) - 1;
    }

    fn str(p: *Parser) i64 {
        p.pos += 1;
        const st = p.pool.items.len;
        while (true) {
            const c = p.t[p.pos];
            if (c == '"') {
                p.pos += 1;
                break;
            }
            if (c == '\\') {
                const e = p.t[p.pos + 1];
                p.pos += 2;
                switch (e) {
                    'n' => putByte(&p.pool, 10),
                    't' => putByte(&p.pool, 9),
                    'r' => putByte(&p.pool, 13),
                    'b' => putByte(&p.pool, 8),
                    'f' => putByte(&p.pool, 12),
                    'u' => {
                        var cp: i64 = 0;
                        var j: usize = 0;
                        while (j < 4) : (j += 1) {
                            const h: i64 = p.t[p.pos];
                            p.pos += 1;
                            const v: i64 = if (h >= 48 and h <= 57) h - 48 else if (h >= 97 and h <= 102) h - 87 else h - 55;
                            cp = cp * 16 + v;
                        }
                        if (cp < 128) {
                            putByte(&p.pool, @intCast(cp));
                        } else if (cp < 2048) {
                            putByte(&p.pool, @intCast(192 | (cp >> 6)));
                            putByte(&p.pool, @intCast(128 | (cp & 63)));
                        } else {
                            putByte(&p.pool, @intCast(224 | (cp >> 12)));
                            putByte(&p.pool, @intCast(128 | ((cp >> 6) & 63)));
                            putByte(&p.pool, @intCast(128 | (cp & 63)));
                        }
                    },
                    else => putByte(&p.pool, e),
                }
            } else {
                putByte(&p.pool, c);
                p.pos += 1;
            }
        }
        push(&p.ps, @intCast(st));
        push(&p.pl, @intCast(p.pool.items.len - st));
        return @as(i64, @intCast(p.ps.items.len)) - 1;
    }

    fn value(p: *Parser) i64 {
        p.ws();
        const c = p.t[p.pos];
        if (c == '{' or c == '[') {
            const close: u8 = if (c == '{') '}' else ']';
            const n = p.node(if (c == '{') 6 else 5, 0);
            p.pos += 1;
            var prev: i64 = -1;
            p.ws();
            if (p.t[p.pos] == close) {
                p.pos += 1;
                return n;
            }
            while (true) {
                var k: i64 = -1;
                if (c == '{') {
                    p.ws();
                    k = p.str();
                    p.ws();
                    p.pos += 1;
                }
                const ch = p.value();
                p.key.items[@intCast(ch)] = k;
                if (prev < 0) {
                    p.first.items[@intCast(n)] = ch;
                } else {
                    p.next.items[@intCast(prev)] = ch;
                }
                prev = ch;
                p.ws();
                if (p.t[p.pos] == ',') {
                    p.pos += 1;
                } else {
                    p.pos += 1;
                    return n;
                }
            }
        }
        if (c == '"') {
            const s = p.str();
            return p.node(4, s);
        }
        if (c == 't') {
            p.pos += 4;
            return p.node(2, 0);
        }
        if (c == 'f') {
            p.pos += 5;
            return p.node(1, 0);
        }
        if (c == 'n') {
            p.pos += 4;
            return p.node(0, 0);
        }
        var neg = false;
        if (c == '-') {
            neg = true;
            p.pos += 1;
        }
        var v: i64 = 0;
        while (p.t[p.pos] >= '0' and p.t[p.pos] <= '9') {
            v = v * 10 + (p.t[p.pos] - 48);
            p.pos += 1;
        }
        return p.node(3, if (neg) -v else v);
    }
};

const hex = "0123456789abcdef";

fn printStr(o: *Buf, p: *const Parser, sid: i64) void {
    putByte(o, '"');
    const st: usize = @intCast(p.ps.items[@intCast(sid)]);
    const len: usize = @intCast(p.pl.items[@intCast(sid)]);
    for (p.pool.items[st .. st + len]) |b| {
        if (b == '"') {
            put(o, "\\\"");
        } else if (b == '\\') {
            put(o, "\\\\");
        } else if (b == '\n') {
            put(o, "\\n");
        } else if (b == '\t') {
            put(o, "\\t");
        } else if (b < 32) {
            put(o, "\\u00");
            putByte(o, hex[b >> 4]);
            putByte(o, hex[b & 15]);
        } else {
            putByte(o, b);
        }
    }
    putByte(o, '"');
}

fn printNode(o: *Buf, p: *const Parser, n: i64) void {
    const i: usize = @intCast(n);
    const k = p.kind.items[i];
    switch (k) {
        0 => put(o, "null"),
        1 => put(o, "false"),
        2 => put(o, "true"),
        3 => putInt(o, p.num.items[i]),
        4 => printStr(o, p, p.num.items[i]),
        else => {
            putByte(o, if (k == 5) '[' else '{');
            var c = p.first.items[i];
            var f = true;
            while (c >= 0) {
                if (!f) putByte(o, ',');
                f = false;
                if (k == 6) {
                    printStr(o, p, p.key.items[@intCast(c)]);
                    putByte(o, ':');
                }
                printNode(o, p, c);
                c = p.next.items[@intCast(c)];
            }
            putByte(o, if (k == 5) ']' else '}');
        },
    }
}

fn run(items: i64, depth: i64, rounds: i64) i64 {
    var text = generate(items, depth);
    var total: i64 = 0;
    var r: i64 = 0;
    while (r < rounds) : (r += 1) {
        var p = Parser{ .t = text.items };
        const root = p.value();
        var o: Buf = .empty;
        printNode(&o, &p, root);
        var h: i64 = 0;
        for (o.items) |b| h = @mod(h * 31 + b, 1000000007);
        total += h + @as(i64, @intCast(p.kind.items.len));
        p.deinit();
        text.deinit(gpa);
        text = o;
    }
    text.deinit(gpa);
    return total;
}

pub fn main() !void {
    std.debug.print("{d}\n", .{run(500000, 6, 3)});
}
