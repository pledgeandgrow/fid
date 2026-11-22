# Week 11 — Systems programming I: data in memory

**Reading:** proposal 0004, section "Layout"
(`docs/proposals/0004-ownership-and-layout.md`); reference §3.1 and §11
(`@repr(C)`, `@packed`); `docs/reference/language-guide.md` §3.5
("Compile-time reflection") and §7.5 ("Enum Representation");
`docs/stdlib/vec.md`.

**After this week you can:** compute the size and field offsets of a
struct under Fid's three layouts; draw a `str`, a `Vec` and an enum in
memory; explain what the ownership rule of week 5 costs in bytes; pack
and unpack binary formats with shifts and masks.

## 1. Why layout matters

High-level code never sees addresses. Systems code does, all the time:
a network packet, a file header, a device register, a struct shared with
a C library or the operating system all have a layout fixed by someone
else. And performance is mostly about memory: how many bytes a value
takes, how many cache lines a loop touches.

## 2. Three struct layouts

| Layout | Rule | Use |
|---|---|---|
| default | one 8-byte word per field (an enum field: `1 + payload` words) | ordinary Fid code |
| `@repr(C)` | the platform C ABI: each field at its alignment, padding between, size rounded to the largest alignment | structs shared with C, the OS, hardware |
| `@packed` | fields one after another, no padding | wire formats; unaligned access |

`comptime struct_size(T)` and `comptime field_offset(T.f)` are replaced by
the numbers the compiler uses, so they can be printed and tested:

```fides
// layouts.fd
@repr(C)
struct Header {
    tag: u8,
    len: u32,
    id: u64,
}

@packed
struct Wire {
    tag: u8,
    len: u32,
    id: u64,
}

struct Plain {
    tag: u8,
    len: u32,
    id: u64,
}

enum Msg { Quit, Move(i64, i64), Write(str) }

struct Holder { tag: u8, m: Msg, n: i64 }

fn main() -> i64 {
    println("repr(C): size " + int_to_str(comptime struct_size(Header))
        + ", len at " + int_to_str(comptime field_offset(Header.len))
        + ", id at " + int_to_str(comptime field_offset(Header.id)));
    println("packed:  size " + int_to_str(comptime struct_size(Wire))
        + ", len at " + int_to_str(comptime field_offset(Wire.len))
        + ", id at " + int_to_str(comptime field_offset(Wire.id)));
    println("plain:   size " + int_to_str(comptime struct_size(Plain))
        + ", len at " + int_to_str(comptime field_offset(Plain.len))
        + ", id at " + int_to_str(comptime field_offset(Plain.id)));
    println("holder:  size " + int_to_str(comptime struct_size(Holder))
        + ", m at " + int_to_str(comptime field_offset(Holder.m))
        + ", n at " + int_to_str(comptime field_offset(Holder.n)));
    println("Msg::Write is variant " + int_to_str(comptime enum_discriminant(Msg.Write)));
    return 0;
}
```

```
$ fid run layouts.fd
repr(C): size 16, len at 4, id at 8
packed:  size 13, len at 1, id at 5
plain:   size 24, len at 8, id at 16
holder:  size 40, m at 8, n at 32
Msg::Write is variant 2
```

`Header` puts `len` at 4, its alignment, after three bytes of padding;
`Wire` puts it right after `tag`, at an odd address. In `Holder`, `m`
takes 24 bytes, from 8 to 32: `Msg`'s largest payload is `Move(i64, i64)`,
two words, plus one word for the tag.

The default layout wastes space (`Plain` is 24 bytes where 13 would do)
and buys simplicity: every field is one word, every load is aligned, and
the lowering never has to reason about narrow fields.

## 3. Managed values in memory

Proposal 0004 fixes one layout for each managed type on every back end
(the interpreter simulates the same memory):

```
str (heap)     s-16                 s-8         s
               [ P_STR_RC + count ][ length ]  bytes ... NUL
str (literal)  [ P_STR_STATIC     ][ length ]  bytes ... NUL

Vec<T>         v ──► { data, len, cap, code, tid }        40 bytes
                        │                     └ how to drop/copy an element
                        └──► [ elements ... ]   code = element size or kind

HashMap<K,V>   { keys, vals, len, cap, shared, intkeys, types }   56 bytes
Box<T>         [ owners ] value ...    (the Box is the address after the count)
enum           [ tag ][ payload words ... ]   (1 + largest payload) words
```

A `str` value is the address of its first byte, so it can be passed to C
as a `char *` (it is NUL-terminated), and its length is one load away.
`unsafe` code can read these words; doing it is a good way to check the
picture, and a bad way to write programs:

```fides
// peek.fd
fn main() -> i64 {
    val lit: str = "hello";
    val heap: str = lit + ", world";
    var n1: i64 = 0;
    var n2: i64 = 0;
    var b0: i64 = 0;
    // SAFETY: reads the length word in front of a string (proposal 0004 layout)
    unsafe {
        val p1: *mut i64 = ((lit as i64) - 8) as *mut i64;
        val p2: *mut i64 = ((heap as i64) - 8) as *mut i64;
        n1 = *p1;
        n2 = *p2;
        val q: *mut u8 = (heap as i64) as *mut u8;
        val byte: u8 = *q;
        b0 = byte as i64;
    }
    println("lengths " + int_to_str(n1) + " and " + int_to_str(n2) + ", first byte " + int_to_str(b0));
    var v: Vec<i64> = Vec::new();
    v.push(5);
    v.push(6);
    v.push(7);
    var len: i64 = 0;
    var cap: i64 = 0;
    // SAFETY: a Vec value is the address of its header {data, len, cap, code, tid}
    unsafe {
        val pl: *mut i64 = ((v as i64) + 8) as *mut i64;
        val pc: *mut i64 = ((v as i64) + 16) as *mut i64;
        len = *pl;
        cap = *pc;
    }
    val room: bool = cap >= len;
    println(f"len {len}, room for all: {room}");
    return 0;
}
```

```
$ fid run peek.fd
lengths 5 and 12, first byte 104
len 3, room for all: true
```

The ownership rule of week 5 shows up here as the `count` word of a heap
string and the `owners` word of a shared `Vec` buffer or a `Box`: that is
what a "copy" updates. Since values sent to another thread are copied
(proposal 0004, rule 8), the counts are never shared between threads and
need no atomic instructions.

## 4. Bits and bytes

Narrow integer types (`u8`, `u16`, `u32`, `i32`, …) are checked at their
width (week 1), and a `Vec<u8>` stores one byte per element. Shifts and
masks pack fields into words; explicit byte order makes a format
portable:

```fides
// bits.fd
// RGB565: 5 bits red, 6 green, 5 blue, in one u16.
fn pack565(r: i64, g: i64, b: i64) -> u16 {
    val v: i64 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    return v as u16;
}

fn red(p: u16) -> i64 { return ((p as i64) >> 11) << 3; }
fn green(p: u16) -> i64 { return (((p as i64) >> 5) & 63) << 2; }
fn blue(p: u16) -> i64 { return ((p as i64) & 31) << 3; }

fn hex(x: i64, digits: i64) -> str {
    val chars: str = "0123456789abcdef";
    var out: str = "";
    var i: i64 = digits - 1;
    while i >= 0 {
        val d: i64 = (x >> (i * 4)) & 15;
        out = out + str_slice(chars, d, d + 1);
        i = i - 1;
    }
    return out;
}

fn put_u32_le(buf: &mut Vec<u8>, x: i64) {
    for i in 0..4 { buf.push(((x >> (i * 8)) & 255) as u8); }
}

fn get_u32_le(buf: &Vec<u8>, at: i64) -> i64 {
    var x: i64 = 0;
    for i in 0..4 { x = x | ((buf.get(at + i) as i64) << (i * 8)); }
    return x;
}

fn main() -> i64 {
    val p: u16 = pack565(255, 128, 64);
    println("rgb565 0x" + hex(p as i64, 4) + " -> " + int_to_str(red(p)) + " " + int_to_str(green(p)) + " " + int_to_str(blue(p)));
    var buf: Vec<u8> = Vec::new();
    put_u32_le(&mut buf, 3735928559);
    put_u32_le(&mut buf, 1);
    var bytes: str = "";
    for b in buf { bytes = bytes + hex(b as i64, 2) + " "; }
    println(bytes);
    println("0x" + hex(get_u32_le(&buf, 0), 8) + " " + int_to_str(get_u32_le(&buf, 4)));
    return 0;
}
```

```
$ fid run bits.fd
rgb565 0xfc08 -> 248 128 64
ef be ad de 01 00 00 00
0xdeadbeef 1
```

`3735928559` is `0xdeadbeef`; little-endian puts its lowest byte, `ef`,
first.

## Exercises

The lab for this week is [Lab 11 — Layouts in memory and on the wire](labs/lab11-wire/README.md).

1. Reorder `Header`'s fields to minimise its `@repr(C)` size. State the
   rule you used, and check it with `struct_size`.
2. Draw `Holder { tag: 1, m: Msg::Write("hi"), n: 3 }` in memory, word by
   word, including the string it points to. Which words does dropping it
   read, and which does it write?
3. Measure (`fid bench`, or a C build and `time`) summing a
   `Vec<u8>` and a `Vec<i64>` of ten million elements. Explain the
   difference with cache lines.
4. Write `encode(h: Header) -> Vec<u8>` and `decode(b: &Vec<u8>) ->
   Option<Header>` for the `@packed` layout, little-endian, without
   `unsafe`. Test that `decode(encode(h))` gives `h` back for edge values
   (0, 255, `0xffffffff`).
5. Why does proposal 0004 put the length of a string *before* its first
   byte instead of in a separate header struct? What would change for
   FFI?

## Discussion

C lets the compiler reorder nothing and pad freely; Rust reorders fields
by default and promises nothing without `#[repr(C)]`; Fid gives every
field a word. Which choice makes the compiler simplest, which the
programs fastest, and which FFI easiest?
