# Security Policy

Fides markets compile-time security guarantees, so the project has to be
exemplary about its own. This file says how to report a problem and what
happens next. The same policy is published at
<https://fides-lang.org/security>.

## Scope

In scope:

- The compiler (`fidesc`) and the self-hosted compiler in `compiler/*.fd`:
  wrong-code bugs that defeat a documented guarantee (memory safety,
  capability grants, effect checking, a safety annotation such as
  `@constant_time` or `@secret`).
- The standard library, in particular `crypto`, `typed_strings`, `http`,
  `json`, `privacy`, and anything the docs describe as a security boundary.
- The companion tools (`ark`, `scribe`, `veil`), the install scripts, and
  release artifacts.
- The website (fides-lang.org) and the browser playground.

Out of scope: denial of service against the playground from pathological
inputs (it runs entirely in the visitor's browser), issues in third-party
dependencies that are already public, and findings that require an already
compromised machine.

## How to report

**Please do not open a public issue for a security-sensitive finding.**

1. Use [GitHub's private vulnerability reporting](https://github.com/pledgeandgrow/p-lang/security/advisories/new)
   on this repository. This creates a draft advisory only the maintainers can
   see.
2. If that is unavailable to you, open a
   [Discussion](https://github.com/pledgeandgrow/p-lang/discussions) titled
   "Security contact request" with no details; a maintainer will reply with a
   private channel.

Include the smallest `.fd` program (or request, or input) that reproduces the
problem, the command you ran, the compiler version (`fidesc version`), and
what you expected the guarantee to prevent.

## What to expect

- **Acknowledgement within 3 business days.**
- **An initial assessment within 10 business days**: confirmed, not
  reproducible, or out of scope, with reasoning.
- **A fix or mitigation** on the `main` branch, a changelog entry that says
  what was wrong, and a tagged release when the fix affects shipped binaries.
  Changelog entries are never rewritten to look better in hindsight.
- **Credit**, if you want it, in the changelog and the advisory.

There is no bug bounty programme at this stage.

## Supported versions

| Version                   | Security fixes                                                |
| ------------------------- | ------------------------------------------------------------- |
| `main` and latest release | Yes                                                           |
| Older tagged releases     | No — upgrade; the v1.0 syntax is frozen, so upgrades are safe |

## Disclosure

Please give the project a reasonable window (90 days is the norm) before
publishing. The project will coordinate a publication date with you when a
fix ships, and will credit the report unless you ask otherwise.
