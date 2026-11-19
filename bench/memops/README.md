# Runtime memory routines (competitive roadmap OP23)

`memops.fd` times the runtime's byte search, block compare, block copy,
string compare and string hash (core's `p_mem_find_byte`, `p_bytes_eq`,
`p_mem_copy`, `str_compare`, `p_str_hash`) against the same work done by a
byte loop in the program, on the same 1 MB blocks and 64 KB strings, and
checks that both give the same answers.

```bash
bash bench/memops/run.sh                       # every backend here
FID=release/fid.exe TARGETS="c native" bash bench/memops/run.sh
```

What each backend runs for those routines:

| Backend | Routines |
|---|---|
| C (`ccompile`) | the C library: `memchr`, `memcmp`, `memmove`; a word-at-a-time compare and hash (codegen_c.fd fast core bodies) |
| pe_ir (`emit` on Windows) | msvcrt's `memchr`, `memcmp`, `memcpy` (stdlib/pe_runtime/base.fd) |
| machine layer (Linux targets, `--nat`) | stdlib/linux_runtime/base.fd: two words (16 bytes) per step for the search, words for compare, copy, fill and hash |
| interpreter | core's byte loops (not timed: the interpreter is not a speed target) |

## Numbers

Nov 16 2026, Windows 11 x64 laptop under load (other builds running), the
stage-0 compiler of that day with the working tree's runtimes; microseconds
for 64 passes over 1 MB (1,024 over 64 KB for the strings), best of
several runs, and how many times faster than the byte loop:

| Case | pe_ir (msvcrt) | `--nat` (linux_runtime words) |
|---|---|---|
| memchr | 3,999 us, 36x | 28,357 us, 4.9x |
| memcmp | 3,995 us, 45x | 15,473 us, 14x |
| memcpy | 4,008 us, 61x | 51,968 us, 6.9x |
| strcmp | 58,332 us, 7.5x | 91,446 us, 6.6x |
| strhash | 27,416 us, 9.1x | 47,487 us, 4.5x |

The two-words-a-step search took memchr on `--nat` from 33.7 ms to 28.4 ms
(best of six each).

Nov 17 2026, same laptop under load, same stage-0 compiler, `--nat`, best
of three or more each, against a build of the Nov 16 runtime (run
alternately with it):
the 16-byte block routines (`pert_vblk_copy`, `pert_vblk_diff`,
`pert_vblk_cmpz`, `pert_vblk_find`) and the hash read their words through
pointers instead of a `pe_ld` call per word:

| Case | Nov 16 runtime | Nov 17 runtime | Gain |
|---|---|---|---|
| memchr | 49,293 us | 31,417 us | 1.6x |
| memcmp | 27,199 us | 12,016 us | 2.3x |
| memcpy | 49,159 us | 8,151 us | 6.0x |
| strcmp | 99,207 us | 76,018 us | 1.3x |
| strhash | 51,829 us | 29,132 us | 1.8x |

Still word loops: the block routines are where compiler/nat_ir.fd can put
vector code (SSE2/AVX2, NEON, RVV) by lowering a call of one by its name,
as it does the simd module's `simd_lanes_*` functions. The gap to msvcrt (16 bytes per instruction with SSE2)
is what real vector code in the machine layer's runtime would close: see
competitive-roadmap OP23 and docs/reference/vector-mir.md. The C column
needs the compiler built from the same tree (its fast core bodies for
`p_mem_find_byte`, `p_mem_copy` and `p_str_compare` are newer than the
stage-0 compiler used here).
