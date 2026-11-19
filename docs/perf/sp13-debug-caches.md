# SP13: the caches of a debug build

Roadmap: docs/roadmap/llvm-roadmap.md SP13 ("a debug mode with no mid-end
and the fast allocator: 5x faster than release mode"). The first
measurement is docs/perf/sp13-debug-vs-release.md: `--debug` makes the part
after lowering 6.5x faster, the whole build 1.62x, because the front end
and lowering are the same work in both modes.

## What a debug build keeps (Nov 17)

`fid emit --debug` on the machine layer (a Linux or macOS target, or
`--nat`) keeps per-function caches beside its output, so the next debug
build of an edited program redoes only what the edit changed
(compiler/main.fd `debug_cache_setup`):

| Cache | File | Default |
|---|---|---|
| machine code of each function (nat_ir.fd CS6/SP7, `FID_FNCACHE`) | `<output>.fides-fncache` | on |
| lowered IR of each function (ir_ser.fd CS4, `FID_IRCACHE`) | `<output>.fides-ircache` | `--ir-cache` or `FID_DEBUG_IRCACHE=1` |

- The driver gives the variables their values inside the process only
  (compiler/inc.fd `env_override_set`, read by `env_or_empty` before the
  real environment): no child process sees them.
- A variable the user set wins. `FID_FNCACHE=0` (or off, no, none, false)
  turns the function cache off; `--no-cache`, `--no-fncache` and
  `FIDES_NO_CACHE=1` keep both off, as for the Windows cache (pe_ir B5).
- A release build (no `--debug`) is unchanged.
- Under `fid serve` the function cache stays in the server's memory
  (`g_nat_fncache_mem`, SP18), so no file is written.
- The function cache is keyed by each function's machine IR and salted by
  the back end's configuration (nat_ir.fd `nc_cfg_salt`: target, CPU, the
  fast allocator, `--size`, ...): a debug entry is never replayed into a
  release build. SP20 (nat_ir.fd `ng_fold_key_ids`) made a warm image
  byte-identical to a fresh one; tests/tooling/sp13-debug-cache-check.sh
  checks that.

## Why the lowered-IR cache is opt-in

ir_ser.fd writes the bytes of a record with `iw_vec_i64`, eight bytes a
byte, and the cache file nests that twice. With the Nov 16 compiler
(stage0 of the Nov 17 run) on tests/bootstrap/test_tooling_explain.fd,
`FID_IRCACHE` wrote a 357 MB file; lowering took 11.2 s instead of 2.4 s,
and the next build ran out of memory reading it (`p_malloc: OOM allocating
4294967304 bytes`). The fix (bytes as bytes, a new file version) is in
ir_ser.fd, not a driver file; once it is in, `--ir-cache` can be measured
(IRCACHE=1 TIME=1 tests/tooling/sp13-debug-cache-check.sh) and made the
default of debug builds in `debug_cache_setup`.

## Measurements so far (Nov 17, a busy machine, stage0 = Nov 16 compiler)

`emit test_tooling_explain.fd --target x86_64-linux --time`, ms:

| Build | front end | lower | mid-end | native code | total |
|---|---|---|---|---|---|
| release | 2333 | 3624 | 4909 | 2006 | 12877 |
| debug, no cache | 1683 | 2411 | - | 1978 | 6329 |
| debug, function cache warm | 2077 | 3245 | - | 718 | 6044 |

The function cache cuts the native-code stage about 2.7x on a warm
rebuild; the whole build stays near 2x of release. The rest of the 5x is
lowering (2.4-3.6 s; lowering everything, `FID_LOWER_ALL=1`, took 3.4 s
against 3.0 s with reach.fd's filter, so its cost is not in the functions
reach.fd leaves out) and the front end. The warm run above was also made
with the Nov 16 compiler, before SP20's fold-key fix: its image had one
function more than a fresh build's; this run's compiler must show equal
images (the check script fails otherwise).
With the back end as it is, 5x needs front end plus lowering at about
0.4 s on this program: that takes the lowered-IR cache (above) and a
cheaper fixed cost of `Lowerer_lower_program` (lower.fd).

To measure with this run's compiler:

    TIME=1 FID=release/fid.exe bash tests/tooling/sp13-debug-cache-check.sh
    IRCACHE=1 TIME=1 FID=release/fid.exe bash tests/tooling/sp13-debug-cache-check.sh

`ratio release/debug: cold ...x, warm ...x` is the SP13 number.
