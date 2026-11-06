# Stability, releases and language changes

This page is the promise Fides makes to the people who use it, and the rules
the project follows to keep it. It covers roadmap items ST1–ST9.

## The stability promise

From 1.0 on, a program that compiles with a 1.x release compiles with every
later 1.x release and means the same thing. "Means the same thing" covers
output, the exit status, which programs are rejected, and every guarantee the
language reference documents (memory safety, effects, capabilities, safety
annotations).

Three kinds of change are allowed inside 1.x, and each is named in the release
notes:

- **Soundness fixes.** A program the compiler accepted that breaks a documented
  guarantee may be rejected. The release notes show the program and the fix.
- **Bug fixes that change wrong output into right output.** If a backend
  disagreed with the interpreter or the reference, it is brought in line.
- **New warnings.** A warning never stops a build unless the project asked for
  that.

What is not covered: the internals of the compiler and the runtime (`p_*`
builtins, generated C, object file layout), the exact text of error messages,
the speed of a program, and anything marked experimental in the reference.

## Versions

Releases follow semantic versioning: `1.x.y` where `y` changes for fixes only,
`x` for additions that keep the promise above, and the major number only with a
new edition.

## Editions

A change that would break 1.x code goes into an edition (planned for 2.0, not
yet built; item ST3). A file states its edition; the compiler accepts every
edition it knows, and code of different editions links together. An edition
changes syntax or defaults, never the meaning of compiled code across a module
boundary.

## The two compilers

`fid` (the compiler written in Fides, `compiler/*.fd`) is the compiler that
ships. From Nov 3, 2026 the Rust compiler (`src/*.rs`, `fidesc`) is frozen
(item ST1):

- New language and library features land in `compiler/*.fd` only.
- `fid` is built from a pinned earlier build of itself, as Go and Rust are:
  `release/stage0/fid.exe`, checked against `scripts/stage0.sha256`.
  `NATIVE=1 ./scripts/bootstrap.sh` builds three stages with `fid emit` and
  checks that stages 2 and 3 are byte-identical, with no C compiler;
  `PIN=1` then makes the checked compiler the next stage 0.
- The source of `fid` may use a new feature only after a stage 0 that knows
  it has been pinned: the compiler learns a change first, its own source
  uses it after the next pin.
- `src/*.rs` keeps the test harness (`fidesc test --compiler <fid>` runs the
  suite on any `fid`) and a cold start when no stage 0 is pinned; it gets no
  new features.

Status (Nov 6): the native stages and pinning are written. No stage 0 is
pinned yet (there is no `release/stage0/fid.exe` or `scripts/stage0.sha256`),
so builds start cold from `fidesc`; the C-mode bootstrap reaches the
byte-identical fixpoint (stage 2 C == stage 3 C, 49 MB). On Nov 3 the
blocker for pinning was that a natively built `fid` stopped with "Invalid
free" while string length headers (roadmap RT2) were unfinished in the native
runtime; that has not been rechecked since.

The language is what `fid` accepts, described in [reference.md](reference.md).
`fidesc` checks less (see
[language-guide.md § 10](language-guide.md#where-fid-and-fidesc-differ-nov-4)).
Two known inconsistencies: `fid version` prints `fid 0.1.0` while the release
is v1.0.0, and the work since v1.0.0 is not yet in a release.

## Releases

- **Cadence** (ST4): a release every six weeks, on a date fixed in advance,
  with whatever is ready. A feature that misses the date waits for the next one.
- **Long-term support** (ST5): every fourth release is an LTS release and gets
  security fixes for a year, and fixes for wrong code the reference forbids.
- **Before each release** (ST6): every known public Fides project is built with
  the release candidate, and a regression blocks the release.
- **Release notes** (ST9) list every change that can affect a working program
  under a **Breaking** heading, even when the promise above allows it: the old
  behaviour, the new one, and what to do.
- **Downloads** (ST10) come with `SHA256SUMS` and a signed build provenance
  attestation for each archive, checked with
  `gh attestation verify <archive> --repo pledgeandgrow/p-lang`.

## Changing the language

A change to the language, to the meaning of a program, or to a public stdlib
API follows this process (ST7):

1. **Proposal.** A file `docs/proposals/NNNN-short-name.md`, in a pull request:
   the problem, the change, examples, what it breaks, the alternatives
   considered, and how both compilers will implement it.
2. **Discussion** on the pull request, for at least two weeks.
3. **Decision** by the maintainers, written into the proposal: accepted,
   rejected, or postponed, with the reasons.
4. **Implementation** behind a flag first when it can break code; the flag goes
   away in the release that ships it, with release notes.

A bug fix, a new stdlib function, or a new warning needs no proposal.

## Security

`SECURITY.md` says how to report a vulnerability, how fast the project
answers, which versions get fixes, and how disclosure works. Fixed
vulnerabilities are published as GitHub security advisories (ST8).
