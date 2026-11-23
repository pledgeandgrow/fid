# End Roadmap — Final-Gate Goals

> Goals moved here on Nov 22 because they are best done at the very end,
> once the compiler and stdlib stop changing: release cadence and LTS
> machinery, crater-style builds, soak tests, CI slowdown gates, docs and
> benchmark publishing, and hardware validation. Doing them earlier wastes
> the run — a soak or a reproducible-build record of a moving tree ages
> immediately.
>
> Each entry names the roadmap it came from; the source keeps a `[>]`
> pointer. When the tree freezes for a stable release, work this file top
> to bottom.

## From competitive-roadmap.md

- [~] ST4. A release every six weeks, on a date, whatever is ready (M).
      Nov 22 audit: PROCESS/FUTURE — the policy is written
      (stability.md:101-102) and four dated prereleases exist (v0.7
      through v0.8.2-unstable), but none demonstrates the cadence (9-week
      gap, then same-day tags) and release.yml (which would mechanize it)
      has 0 runs. Cannot be code-done.
      Done: the policy. Left: the first dated release.

- [~] ST5. Long-term support releases with security fixes (M).
      Nov 22 audit: PROCESS/FUTURE — the LTS policy exists
      (stability.md:103) but no LTS release can exist until releases
      accumulate; nothing to verify yet.
      Done: the policy. Left: the first LTS release.

- [~] ST6. Before each release, build every known public Fides project, as Rust's
      Nov 22 audit: mostly done — scripts/crater.sh + projects.txt +
      baseline.txt (saved Nov 17 from the 0.8.0 fid; the "no baseline"
      Left is stale) + the crater job in release.yml:222-254. Precise
      gaps: projects.txt holds only this repo's own fixtures (no known
      external project yet), and release.yml has ZERO runs ever — all
      four tags were cut without the gate.
      crater does (L).
      Done: the rule. Nov 15: the list, `scripts/crater/projects.txt` (local or
      git source, command, files), and `scripts/crater.sh`, which copies or
      clones each, runs `ark install` where there is an ark.toml, builds each
      file (`check`, `run` or `ccompile`) and fails on a file that built with
      the last release (`scripts/crater/baseline.txt`) and does not now; the
      `crater` job of `.github/workflows/release.yml` runs it with the new fid.
      Left: no public project outside this repository is known, so the list
      holds this repository's examples and tests/projects; the first baseline
      (`scripts/crater.sh --save` with a release's fid).

- [~] ST10. Signed checksums for every download (S).
      Nov 22 audit: mostly done — release.yml:278-299 makes SHA256SUMS
      over installer+tar.gz+docs and attests provenance per file; the
      root SHA256SUMS is tracked (1fee238) and v0.8.2 carries the asset.
      Precise gaps: never proven on a real release (release.yml 0 runs;
      nothing signed to verify), and Authenticode still has no
      certificate (FIDES_SIGN_PFX).
      Done: `SHA256SUMS` and a Sigstore build provenance attestation per archive in `release.yml`. Left: proven on a real release.
      Nov 15 (checked): that release.yml was gone — commit 9f2ba40 (Nov 2)
      deleted it and build-installer.yml, and it never had an attestation
      step. A new `.github/workflows/release.yml` builds the installer and the
      Linux archive, writes `SHA256SUMS` over both and one
      `actions/attest-build-provenance` attestation per file, and attaches
      them to a draft release of the tag; `scripts/release-checksums.sh`
      stays for a local build. Left: proven on a real release; Authenticode
      signing of the installer needs a certificate (installer/build-installer.sh
      `FIDES_SIGN_PFX`).

**Acceptance:** two years of releases with no unannounced break.

## Track AD — Real use

Only this produces true production years. It starts now, at small scale.

**Using it ourselves**

- [~] BM9. CI fails on a slowdown over 5%, per benchmark (release D7, tighter) (M).
      Done (Nov 4): `bench/check.sh` fails on a slowdown over `LIMIT_PCT` against
      `bench/baseline.txt`. Verified Nov 15: `.github/workflows/bench.yml`
      already ran it in CI (base commit records, head checked, 10%). Nov 15:
      `check.sh --ab` runs the base and head executables in turn on the
      runner and re-measures a program over the limit before failing;
      bench.yml now uses it with `LIMIT_PCT=5`. Left: CI runs to show 5%
      holds without false failures; the BM3/BM4 sets in the gate
      (`SUITES="game work"`) once they have built.

- [~] BM10. A public page of results over time, losses included (M).
      Verified Nov 15: docs/benchmarks.md is that page (Nov 2, 3 and 15
      runs, weak spots, a corrections policy). Nov 15: `bench/history.sh`
      adds a run's tables to it unchanged, newest first. Left: on the
      website, and kept up (the weekly Linux tables are artifacts, added by
      hand).

**Acceptance:** anyone can reproduce every number with one command.

## Track OP — Optimizer

Today the native path keeps most values in stack slots (seven callee-saved
registers hold the hottest values), runs a small IR optimizer and leaves floats
in memory. This track is where most of the speed is.

**Foundations**

- [~] AD13. Publish the benchmarks (BM10), and invite corrections (S).
      Done (Nov 15): `docs/benchmarks.md` (a page of the site): what is
      measured and how to reproduce it, every result in the repo (Nov 2,
      Nov 3, a Nov 15 run through C on a busy machine), where Fides loses,
      and a corrections section pointing to GitHub issues. Left: putting it
      online (AD2's hosting), a quiet-machine run with native numbers and
      the spread (BM8), results kept over time (BM10).

- [~] SL19. Every public function documented with an example that runs in CI (L).
      Nov 22 audit: mostly done — scripts/doctest.py + tools/doctest.fd,
      CI-wired (ci.yml:123,:128, enforced by ci-wiring-check.sh CU7);
      737 /// Example: blocks over 147 files (was 528/31). Precise gap:
      examples on the rest (quic.fd 0, fs.fd 1, ~6000 pub items) —
      mechanical writing, no code or CI work left.
      Done: `scripts/doctest.py` extracts `/// Example:` code blocks from
      .fd sources and runs each as a program through `fid run` — CI-gatable
      (exit 1 on any failure); `(native only)` marker skips FFI examples on
      the interpreter. Nov 15: 528 examples (519 run, 0 fail, 9 native
      only) over 6706 pub items; each checks its result with `assert` or
      `assert_eq` (doctest.py looks only at the exit code). The 31 most used
      modules have an example for nearly every public function: string, vec,
      collections, iter, fmt, math, json, encoding, path, time, datetime,
      sort, set, queue, stack, bigint, decimal, regex, hash, random, result,
      csv, uuid, semver, url, log, metrics, bits, bitset, args, env. Writing
      them fixed `utf8_encode` for U+0080-U+00FF, `random`'s state (a `pub
      const` never advanced: every `uuid_v4()` was the same), `bit_mask(63)`,
      `fmt_bytes`, and the smallstr and rope lengths. Left: the other
      modules; examples on methods (doctest.py reads `/// Example:` at
      column 0 only); doctest.py's failure line is the first `[build]`
      line rather than the error.
      examples (`scribe` renders docs only).

- [~] VF16. Week-long soak tests of a service under load, with flat memory (release
      I6 is 24 hours) (M).
      Done (Nov 7): `scripts/verify/soak.py` runs `tests/verify/soak_service.fd`
      (HTTP head parse, JSON, a bounded cache) and fits the memory slope of
      the second half. Run: fails at once — L1, ~3.9 KB leaked per request on
      the C path, out of memory after ~1 M requests (15 s); `fid run --leaks`
      names the sites. Checked Nov 16 by reading: L1 was measured on Nov 7,
      before ownership (proposal 0004, Nov 8) made the lowering drop every
      string, Vec and map it names (the sites `--leaks` listed are core's
      and the lowering's: String_slice, concatenation, int_to_str, the Vecs
      of http_parse_head and json_get_*); http.fd and json.fd keep no string
      outside it, and core's HashMap_remove and Vec_clear drop what they
      remove. Nov 16-17, a real socket server under load
      (`scripts/verify/soak_http.py`, docs/verification/soak-http.md): it
      leaked per request on kept-alive connections; found and fixed Nov 17
      (docs/verification/soak-http-leak.md: a handler's result called
      through a function value, and `hc_text`, each kept a second owner).
      After it `soak.py` and `soak_http.py` are flat; a 30-minute socket
      soak took 10.1 million requests, 0 failed, at 6.0 MB (slope -0.04
      MB/hour). Left: the 24-hour run (started Nov 17 16:46,
      build/soak-http-24h/) and the week-long run.

## From release-roadmap.md

- [~] G4. `fides-os` builds with the released compiler, in its own CI (M).
      Nov 15: fides-os builds with `fid` (`emit --boot --load 0x1000000`):
      minimal.fd prints "Hello from Fides OS!" and the shell kernel reaches
      its prompt under QEMU. Its CI (fides-os .github/workflows/build.yml)
      builds fid from pledgeandgrow/fid and boots both; done when it has run
      green there. GRUB from an ISO, user mode, disk and network are not
      tested.
      Nov 22: the released `release/fid.exe` emitted the current shell kernel
      (439,736 bytes); QEMU booted it and printed `Fides OS v0.1` and the
      shell prompt. The public `pledgeandgrow/fides-os` repository currently
      has no GitHub Actions workflows, so the required own-CI run is absent;
      this used the current worktree with uncommitted kernel edits, not a
      clean checkout. Leave partly.

**Acceptance:** a kernel boots and prints under QEMU, from a clean checkout.

## Track H — Concurrency

- [~] I6. A sample service survives a 24-hour load test (M). A short run:
      160,000 requests stay at 5.2 MB with `http_serve_scoped`. The harness
      exists: `scripts/verify/soak.py` (--duration 86400) samples memory and
      heartbeats of `tests/verify/soak_service.fd`, an in-process request
      loop (no socket). Nov 16-17: `scripts/verify/soak_http.py` soaks the
      service over real sockets (docs/verification/soak-http.md); its
      per-request leak on kept-alive connections is fixed
      (docs/verification/soak-http-leak.md), and a 30-minute socket soak
      passed flat: 10.1 million requests, 0 failed, 6.0 MB, slope -0.04
      MB/hour. Nov 22 audit: the recorded `build/soak-http-24h/` result
      directory is absent, so there is no 24-hour result to verify. Left:
      a 24-hour run and a rerun with the release compiler of this tree.
      Keep partial.

- [~] N9. Source-level debugging of native Windows builds: line tables a standard
      debugger can read (L).
      Done: `fidesc emit -g` writes DWARF v4 sections (`.debug_line` etc.) into
      the PE with per-instruction line tables; `ccompile -g` annotates generated
      C with `#line` to `.fd` sources. Left: same emission in the self-hosted
      `fid` (compiler/*.fd), variable DIEs, verify with gdb/lldb/VS debugger.
      Checked Nov 15: `fid` writes `.debug_line` (DWARF 4) on the three Linux
      targets (compiler/nat_enc.fd nat_debug_line; competitive TL5). For
      Windows, still left in the backend's files: CodeView/PDB or DWARF from
      pe_ir, `#line` in compiler/codegen_c.fd's C, variable locations.
      Nov 16 (not run yet): pe_ir writes DWARF 4 (.debug_abbrev,
      .debug_info, .debug_line) into the PE of `fid emit --debug`/`-g`
      for the input file's functions: gdb and lldb on Windows read it
      (llvm DB3; scripts/pe-dwarf-check.py). Left: a PDB for the Visual
      Studio debugger and WinDbg, variable locations (llvm DB4), checking
      it in gdb/lldb on a Windows host.
      Nov 16: `fid ccompile -g` writes `#line` to the .fd source and builds
      with -g (competitive TL5), so gdb/lldb on a C build shows .fd lines;
      a panic also prints the failing frame's values (complaint CE8). Left:
      pe_ir's CodeView/PDB, variable locations, a check with a debugger.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — pe_dbg_vars_check.sh: 9 passed, 3 failed. --debug builds
      are right (DWARF decoded, pe-dwarf-check, a scripted debugger run
      reads 25 of 25 values, with steps and function breakpoints), and -g
      output and its pe-dwarf-check pass; objdump in WSL reads the
      sections with no warning (the script's own objdump step fails on a
      Git Bash path). Left: -g (optimized) shows wrong values for main's
      locals (total = 0, want 45), and inlined functions' lines have no
      rows; with FID_PE_DEBUG_RA=1 parameters show as optimized out; no
      gdb/lldb session (not installed).
      Nov 21 (c082ee6, 2g.104): `emit -g` reads only the instructions in a function's blocks (peir_dbg_live) — an inst the optimizer detached showed stale slots for main's locals; FID_PE_DEBUG_RA=1 keeps parameters in their slots.
      Nov 21 evening: the PDB leftover is covered — compiler/pdb.fd writes
      a PDB for `emit -g` on the pe_ir and --nat paths (llvm DB3, marked
      done today). Variable locations in optimized builds and a real
      gdb/lldb/WinDbg session stay open.

## From roadmap.md

- [~] Real hardware testing (QEMU ARM64, QEMU RISC-V, real UEFI firmware)
  Nov 22: QEMU ARM64/RISC-V/x86-64 and OVMF UEFI all work (release F8/G1);
  real boards and real UEFI firmware need hardware CI runners (competitive PL3/PL5).
  — QEMU ARM64 and RISC-V done (release F8), x86-64 bare images under qemu-system-x86_64 (release G1). UEFI images (above) boot under OVMF in QEMU (`tests/uefi/run.sh`). Left: real boards (competitive PL3, PL5) and real UEFI firmware (not tried).

