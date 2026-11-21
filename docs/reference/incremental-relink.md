# Incremental relinking in memory (SP7)

A long-lived `fid` process (`fid serve`, or any driver that builds the same
program more than once) keeps the machine layer's per-function cache in
memory between builds (`g_nc_mem`, docs/reference/native-fncache.md). SP7
keeps the layout of the last build there too, so that the next build after
an edit does not lay every function out again: the functions that did not
change keep their bytes at their offsets, the changed ones go back into
their old places when they still fit, and only the references (fixups) are
resolved again.

Code: compiler/nat_ir.fd, section "SP7: incremental relinking in memory"
(`nl_*`), with its hooks in `ng_mir_finish` (the loop that places the
functions), `nc_replay` (its `at` argument), `nc_start` (`nl_reset`) and
`nat_generate_image1` (`nl_commit`, and the fresh retry).

## What a build keeps

After the loop that places the functions (`nl_loop_end`), a build whose
fixups then resolve keeps (`nl_commit`):

- what it was: the cache path, the cache salt (target, configuration,
  program layout: `nc_cfg_salt`, `nc_mod_salt`), the architecture and
  object format;
- where its functions start (`pos0`: the code in front of them, the
  primitives, is that long) and end;
- per placed function its start and its slot: from its start to the next
  function's start (the last one's to the end of the functions), so the
  alignment padding and any gap after it belong to it.

## What the next build does with it

`nl_begin` uses the kept layout when this build has the cache on, the same
cache, salt, architecture and format, the same code in front of the
functions, and at least half of the kept functions. It then reserves the
kept code (zeros, as alignment padding is) and places each function, in
the usual order:

| Function | Where it goes |
|---|---|
| a cache hit whose bytes fit its slot | its recorded bytes replayed at its old offset (`nc_replay(g, e, at)`), its fixups registered against that offset |
| compiled again, fits its slot | emitted after the reserved code as in any build, then moved into its slot (`nl_move`: bytes, the labels placed in it, its fixups, line and frame rows); on x86-64 a function with a padded loop head (MI30) moves only by a multiple of 16, else it stays after the code |
| no slot (new), or outgrew it | after the reserved code, as in a fresh build; its old slot stays zeros (never executed) |
| folded onto identical code | as before: its label points at that code |

Every fixup is then resolved by `NatEnc_resolve` as in a fresh build: a call
to a function that moved reaches it at its new place, and the unchanged
functions' bytes are not touched except where they refer to something that
moved.

The panic-trace table (`[address, name]` pairs a panic searches), the
symbols, the DWARF line program and the frame rows (.debug_frame,
.eh_frame, PE unwind data) want address order. With a kept layout used,
`nl_loop_end` sorts the placed names by start and the line and frame rows
by the start of the function that made them (a function's rows keep their
order; a hit replayed into its slot ends its line sequence at its own end,
`nl_line_close_at`).

## When it starts afresh

- RISC-V: its short calls (TG6) and RVC (TG9/TG19) depend on where every
  function is; object files; the parallel emission (never with the cache);
  `FID_NAT_OFF=relink`. `--stats` says why: `[relink] a fresh layout (...)`.
- A reference out of reach in the kept layout (a short branch to code that
  is now further away): `NatEnc_resolve` fails, the layout is dropped and
  the same build runs again laid out afresh (`nat_generate_image1`).
- A layout whose code is less than half used (many functions grew or went
  away) is not kept: the next build starts afresh and compact.

An image built this way is not the same bytes as a fresh build (functions
sit at other offsets, with gaps), but it runs the same. When nothing
changed, every function is a hit in its own slot and the image is the same
bytes as the build before (tests/nattools/machine_batch7_check.sh, SP7).

## `--stats`

```
[relink] a fresh layout (no earlier build in this process)
[relink] the last build's layout: 131 functions in place, 3 compiled again into their places, 1 placed after them
[relink] less than half of the kept layout is code: the next build starts afresh
[relink] the kept layout does not resolve (...): laid out afresh
```

`nat_relink_counts()` gives the last build's numbers ([used, in place,
moved into place, placed after]) and `nat_relink_start(name)` a function's
start in the kept layout, for a driver or a test.

## Checks

`bash tests/nattools/relink_check.sh [fid]`: one `fid serve` builds
tests/bootstrap/test_machine_relink.fd, then two edits of it
(tests/nattools/relink_edit_b.fd, relink_edit_c.fd) and the program again
from the same path, with `FID_VERIFY=1`, for x86-64 Linux, AArch64 Linux
and `--nat` Windows. It checks the `[relink]` counts of each build, that
each image runs as a fresh build of its source (output and exit code),
that the unchanged functions keep their addresses
(tests/nattools/relink_syms.py), that readelf decodes the line program,
and that `FID_NAT_OFF=relink` lays the code out afresh.

Nov 21 (roadmap SP7 check, fid built from a self-hosted stage 0): relinking
in memory works when the layout is kept (AArch64, build 4: 241 functions in
place, 1 compiled again), but `relink_check.sh` fails 10 checks: an edited
program never reuses the kept layout ("a fresh layout (the last build was
of another target, cache or program)"), 149 of 241 unchanged functions
move, and on x86-64 and `--nat` the changed function goes after the
others.
