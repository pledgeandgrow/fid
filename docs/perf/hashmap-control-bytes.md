# HashMap control bytes, measured (RT4)

Competitive roadmap RT4 added control bytes to `HashMap` (stdlib/core.fd,
"Control bytes"): one byte per slot after the stored hashes, a probe
reading 8 slots' bytes as one word (SWAR). It also added a seeded hash
for maps that get flooded (core.fd, "Floods"). This page records what
the control bytes cost and save, how that was measured, and why the
native backends have no 16-lane SIMD probe.

## Setup

Measured on Nov 17 2026 on Windows x64. The C path was used:
`fid ccompile`, `zig cc`, -O2. The compiler was the stage-0 `fid` of the
day, which reads core.fd when it runs, so it compiled the core under
test. There were three builds of one program: `HM_CTRL_ON = true` (the
default), `HM_CTRL_ON = false` (the slot-by-slot probe) and a copy of
the C with a 16-lane SSE2 lookup written by hand. Other builds were
running on the machine at the same time, so each figure is the minimum
of 6 to 8 runs. Single runs varied by up to 30%.

Programs: `bench/fides/hashmap.fd` (1M string keys inserted, then each
looked up once) and two programs that time each phase with
`p_clock_now_us`:

- large: 2M integer keys `i * 7919` inserted; 3 x 2M hits; 2M absent
  keys; 2M churn steps (insert then remove a short-lived key); then 1M
  string keys inserted and looked up 3 times with the same string
  objects.
- small: 5000 keys (fit in cache); 400 rounds of string hits (lookups
  with equal strings that are separate objects), integer hits, and
  integer misses.

## Results (ms, minimum of runs)

| Phase | control bytes | slot by slot | control bytes, no stored-hash check on string hits |
|---|---|---|---|
| bench/fides/hashmap.fd (whole program) | 1232 | 1081 | 1167 |
| large: int insert 2M | 510 | 404 | 494 |
| large: int hits 6M | 910 | 853 | 791 |
| large: int misses 2M | 135 | 291 | 110 |
| large: churn 2M | 800 | 612 | 730 |
| large: string insert 1M | 334 | 241 | 289 |
| large: string hits 3M | 906 | 577 | 780 |
| small: string hits 2M | 144 | 132 | 136 |
| small: int hits 2M | 45 | 44 | 45 |
| small: int misses 2M | 34 | 65 | 43 |

What the numbers show:

- Absent keys cost about half as much at every size. A miss reads one
  control word per 8 slots and touches no key.
- In a map that fits in cache, hits cost the same.
- In a map larger than the caches, hits and inserts are 10-30% slower.
  The control bytes are a fourth region of memory, after the keys,
  values and hashes, so a probe pays one more cache miss and one more
  TLB entry. The probe's arithmetic is not the cost.
- Skipping the stored-hash check on string keys gets back part of the
  hit cost (906 -> 780 ms). This version is now in core: `hm_ctrl_find`
  and `hm_ctrl_lookup` compare the key as soon as 7 bits of the hash
  match. The stored hashes are still kept, because growing reads them
  instead of hashing every key again. A false match (1 in 128 per live
  lane) costs one string compare.
- A 16-lane SSE2 group (`_mm_cmpeq_epi8` and `_mm_movemask_epi8` over 16
  control bytes) gave no measurable gain over the 8-lane SWAR word:
  string hits 843 vs 779 ms, int misses 143 vs 125 ms, within the noise.
  It was written into a copy of the generated C for this test only. A
  probe is memory-bound once the 7-bit filter has done its work. For that
  reason no SSE2/NEON probe was added to pe_ir or nat_ir. It would pay
  off only with a layout that holds no hashes and keeps keys and values
  side by side, as hashbrown does. That is a different map.

### Checked again (Nov 17, afternoon)

The same comparison with core as it is now (stage-0 `fid`, `ccompile`,
`zig cc`; two copies of stdlib/ that differ only in `HM_CTRL_ON`; the
machine was busy, minimum of 5 runs each):

| Program / phase | control bytes | slot by slot |
|---|---|---|
| bench/fides/hashmap.fd (whole program) | 813 | 783 |
| 2M integer keys: insert | 450 | 387 |
| 3 x 2M integer hits | 782 | 732 |
| 3 x 2M integer misses | 205 | 865 |
| 20M insert + get on a map of 1000 keys | 1041 | 1261 |

The picture is the one above: misses about 4x cheaper, small maps
faster, inserts into a large map about 15% slower, hits on a large map
within 10%. The whole string benchmark is 4% slower.

`HM_CTRL_ON` stays on. A compiler's maps are mostly small, with many
lookups of absent names (scopes, interning, imports), so misses at half
the cost outweigh slower hits on maps of millions of keys. Programs that
only hit huge maps lose up to about 10% on the whole program
(`hashmap.fd`).

## Floods

A map is reseeded only after an insert lands 64 groups (512 slots) past
its key's home group. Ordinary keys never do this, so the seeded hash
has no cost until then. test_stdlib_hashmap_flood.fd builds 600 integer
keys and 600 string keys that all share one home group (it inverts
core's hash). It checks that each map takes a seed, that lookups,
removes, copies and clones stay right, and that `keys()` comes out in
the same order in every run.

What it saves: 30,000 integer keys that all share one home group,
inserted and then each looked up once, take 8 ms with the defense and
1622 ms without it (the same program built with `HM_FLOOD_GROUPS` set
too high to ever fire). Without it every insert walks the whole run of
keys before it, so the time grows with the square of the number of
keys.

Why a map takes a seed only when it is flooded, and not one seed per
process mixed into every hash: a per-process seed changes which slot
each key lands in on every run. `keys()`, `values()` and printing sort
the keys, so a program would print the same, but the order in which a
map's slots are walked (dropping, copying, `map_own`) would differ from
run to run. The compiler is built from its own C output and checked byte
for byte (stage 2 = stage 3), and the compiler is full of maps. Any
place where slot order reaches the output would make that check fail at
random. With a seed taken only on a flood, every ordinary map lays out
its keys the same way in every run, and the seed costs nothing. A map
that is flooded still gets a seed no outsider can know, from
`p_os_random_u64`, the same per-process random source a per-process
seed would use.

Limits: the keyed string hash (`hm_str_hash_keyed`) is FNV-1a started
from the seed, with the high bits folded down after each byte. It is not
a keyed PRF like SipHash. An attacker who can measure lookup times
against one long-lived seeded map could in principle learn about its
seed. Integer keys are mixed with the seed before and after the
splitmix finalizer, which is a bijection for any seed.

## Backends

Run on Nov 17 with the stage-0 `fid` and core as it is now. The output
matched the expected lines in both tests on each of these:

- C (`ccompile`, `zig cc`), Windows x64;
- pe_ir (`emit`), Windows x64;
- the machine layer on Windows (`emit --nat`);
- C cross-compiled for Linux AArch64 (`ccompile --target aarch64-linux`,
  run under qemu-aarch64 in WSL);
- `run --leaks` (built through C with the leak report): "every block
  was freed" for both tests.

These two could not be run with the stage-0 compiler, for reasons that
have nothing to do with the map: the interpreter (`run`), whose stage-0
build lacks the region builtin `p_region_word` that core now calls; and
`emit --target x86_64-linux|aarch64-linux`, where the stage-0 build
links against a runtime that needs `nat_tls_set`. The build of the day
checks both.

## Reproducing

```bash
# on and off: build with core as is, then with HM_CTRL_ON = false in a
# copy of stdlib/ (run fid from that copy's parent: ./stdlib comes first)
CC="zig cc" $FID ccompile bench/fides/hashmap.fd -o hm_on.exe
# ... in the copy with HM_CTRL_ON = false:
CC="zig cc" $FID ccompile <repo>/bench/fides/hashmap.fd -o hm_off.exe
for i in 1 2 3 4 5 6 7 8; do time ./hm_on.exe; time ./hm_off.exe; done
```
