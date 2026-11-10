# Support & Community

## Questions

Use **GitHub Discussions** — https://github.com/pledgeandgrow/fid/discussions

- **Q&A**: usage questions, "how do I…", error messages you can't decode.
- **Ideas**: language/library proposals — expect honest feedback on scope.
- **Show and tell**: things you built with Fides.

## Bugs

Use **Issues** — https://github.com/pledgeandgrow/fid/issues — with a
minimal reproducing `.fd` file, the `fid` command you ran, `fid version`
output, and expected vs actual behavior.

Miscompile reports (wrong output on any backend) get priority — include
the input and both outputs if you can. See
`docs/verification/miscompile-log.md` for how these are tracked.

## Contributing

- `good first issue` label on the issue tracker — kept current.
- Read `AGENTS.md` and `docs/compiler-internals.md` before touching
  `compiler/*.fd`.
- The bar: `RELEASE=1 ./scripts/bootstrap.sh` fixpoint must stay green;
  interpreter/`emit`/`ccompile` must agree on the test suite.

## Not covered here

Security disclosures: do NOT file public issues for exploitable bugs —
contact the maintainers privately first (see the security policy in
`docs/roadmap/security-roadmap.md`).
