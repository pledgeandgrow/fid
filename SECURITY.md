# Security Policy

## Reporting a vulnerability

Please report security problems **privately**, not in a public issue:

1. Go to the repository's **Security** tab and choose **Report a
   vulnerability** (GitHub private vulnerability reporting), or
2. if that is not possible, open a minimal public issue that says only
   "security report, please contact me" and we will reach out.

Include what you can of:

- the `fid version` output and how you installed it (installer, built from
  source, which commit);
- the backend: interpreter (`fid run`), C (`fid ccompile`), native Windows
  (`fid emit`), native Linux (`fid emit --target ...`);
- a small `.fd` program that shows the problem, the command you ran, and
  what happened versus what you expected.

We aim to acknowledge a report within **7 days** and to tell you within
**30 days** whether we accept it and when a fix is planned. We credit
reporters in the release notes unless you ask us not to.

## What counts

Fides makes safety promises, so a broken promise is a security bug even
when no exploit is shown:

- **Miscompilation of a safety guarantee**: a bounds or overflow check
  removed when it can fail, a borrow-check or escape-check hole that lets
  safe code reach freed memory, `@secret` data reaching an output, a
  `sandbox {}` reaching a capability it was not granted.
- **Memory safety in the runtimes**: use after free, double free, buffer
  overflow or data races in the C runtime (`compiler/codegen_c.fd`), the
  native runtimes (`stdlib/pe_runtime/`, `stdlib/linux_runtime/`) or core
  (`stdlib/core.fd`) reachable from safe Fides code.
- **The standard library's parsers and network code**: HTTP, TLS, WebSocket,
  JSON, compression decoders and the like, given hostile input.
- **The installer and release artifacts**: anything that lets a release or
  `Fides-Setup-*.exe` run code the user did not ask for.
- **The compiler itself** on hostile source: a crash is a normal bug, but
  code execution, writing files outside the output path, or reading secrets
  is a security bug.

Not security bugs: code inside `unsafe` or `extern "C"` behaving as C does,
known limits listed in [docs/reference/known-bugs.md](docs/reference/known-bugs.md)
(§2g lists the open ones), and the frozen Rust bootstrap compiler `fidesc`
on its own (it is a build tool, not a supported compiler).

## Supported versions

| Version | Supported |
|---------|-----------|
| `main` (latest commit) | yes: fixes land here first |
| latest release | yes |
| older releases | no |

## Not for safety-critical use

Fides is not certified for medical, avionics, automotive or other
safety-critical systems. `fid qualify` writes evidence about a program; it
is not a certification. See [docs/reference/safety-rules.md](docs/reference/safety-rules.md)
for exactly which rules the compiler enforces.
