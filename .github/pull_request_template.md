## What and why

<!-- One or two sentences: what changes and why. Link the issue if there is one. -->

## Checks

- [ ] Changes to the language or compiler are in `compiler/*.fd` (`src/*.rs` is frozen)
- [ ] A test in `tests/bootstrap/` covers it (`test_<name>.fd`; `_fail.fd` for a program that must be rejected)
- [ ] Checked on the backends it touches: `fid run`, `fid ccompile`, `fid emit`
- [ ] A change to ownership or the runtimes passes `scripts/leak-check.sh`
- [ ] Docs updated if behaviour changed (`docs/reference/reference.md` for the language, `changelog.md`)

<!-- See CONTRIBUTING.md for how to build and test. -->
