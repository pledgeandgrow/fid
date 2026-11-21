# SP8: a one-line edit rebuilt by a warm `fid serve`

Goal (docs/roadmap/llvm-roadmap.md, Track SP): a one-line edit rebuilds in
under 100 ms, with the stdlib, the lowered functions and the machine code
kept in a warm `fid serve`, and only the changed module or function checked
and lowered again. On Nov 13, hello took about 0.5 s with every cache warm
(CS6).

## What a build request of `fid serve` reuses

A build request is `emit <file> [flags]`, `build <file> [flags]` or
`ccompile <file> [flags]` (compiler/main.fd `serve_build`). Between
requests the server keeps:

| What | Where | Since |
|------|-------|-------|
| core and the runtime, parsed and merged | compiler.fd `astcache_load`, `astcache_memo_merged` | SP4, SP18 |
| core's checked bodies are skipped | inc.fd `inc_core_checked` (D9) | D9 |
| imported modules' parses | compiler.fd `modcache_get` | CB2 |
| **the program's checked bodies** | compiler.fd `cb2_request_build` (this goal) | SP8 |
| machine code of each unchanged function | nat_ir.fd `g_nat_fncache_mem` | SP7, SP18 |

The new part: a build request runs CB2's incremental body check
(docs/incremental-check.md). The server compares the program's modules with
the last build whose checks passed. It checks again only the function
bodies that an edit since then can reach: the edited items, and items that
mention a name they define, followed to a fixpoint. Unlike a check request,
the build goes on past the borrow check: comptime substitution,
monomorphization, lowering and code generation.

Skipping a body's checks does not change what gets lowered. The checkers
read the Program and do not change it. Core's bodies are skipped the same
way on every build (D9).

A check request and a build request differ in their CB2 settings (the
runtime merged, the @cfg target, `--debug`; compiler.fd `cb2_config`). So
the first build after a check, or after a change of target, checks every
body.

Each build request writes one line on the server's stderr:

    fid serve: emit prog.fd: 84 ms, 2 of 5 bodies checked

This is the time from the request to the written output, followed by CB2's
note. Over the socket (`fid serve --port`) the request's note also carries
the time and CB2's note. Set `FID_CB2=off` to check every body on every
request.

Lints are not run in an incremental build request (as in a check request).
Warnings that come from the checks themselves, such as a call to a
`@deprecated` function, appear only when the body holding the call is
checked again.

## Measuring

    FID=release/fid.exe bash tests/tooling/sp8-edit-rebuild-check.sh
    T=nat ...            # Windows x64 through the machine layer (--nat)
    MODE=debug ...       # only the debug mode (no mid-end, fast allocator)
    STRICT_TIME=1 ...    # a rebuild of 100 ms or more fails the script

The script edits one line of one function, then one line of another, and
rebuilds after each edit through one server, in release mode and in debug
mode. It checks that:

- each rebuilt image is byte-identical to the one-shot `fid emit` of the
  edited file;
- CB2 checked fewer bodies than the program has;
- a type error is reported, and the image is right again once the error is
  taken out.

It prints each request's time and whether the 100 ms target was met.

Known bug (Nov 21, 2026, known-bugs 2g.82): the machine layer's function
cache does not keep images byte-identical. A cache-hit image differs from a
cache-off one, and `fid serve` rebuilds differ from a one-shot emit, so the
first check above fails. Every image still runs the same.

## What is left

- **Lowering.** Every build lowers every reachable function again. The
  lowered-IR cache (ir_ser.fd `irc_*`, FID_IRCACHE) is file-based, and its
  records make files far too large for an edit loop. The in-memory mode it
  needs (records kept between `lower()` calls of one process, the salt
  compared in memory) is a change in compiler/ir_ser.fd. The server would
  then turn it on, as it does `g_nat_fncache_mem`.
- **The mid-end.** iropt, the rules and the e-graph run on the whole module
  on every build. In release mode this is most of a warm rebuild. A cache
  per function has to respect iropt's whole-module passes (inlining,
  identical-code folding, facts about globals), so it belongs in iropt.fd.
  In debug mode (`--debug`) there is no mid-end, so the edit loop's
  target is measured there.
- **Relinking.** The image is still laid out again in full. Only the
  machine code of unchanged functions is replayed (SP7).
