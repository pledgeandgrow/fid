# Example projects (release-roadmap N11)

Five small projects, one of each kind a new user starts with. CI builds
each the way a user would and compares what it prints with its
`expected.txt` (`scripts/projects-check.sh`; the library also through `ark`,
`scripts/ark-check.sh`).

| Project | Kind | Built with | Run |
|---------|------|------------|-----|
| `cli/` | command-line tool (`wc`) | `fid ccompile` | the binary, with two files as arguments |
| `library/` | a library pact (`textkit/`) and an app using it | `fid ark add/install`, `fid run`, `fid ccompile` | both |
| `service/` | line-protocol key-value service | `fid ccompile` | the binary, `requests.txt` on stdin (exit status 1: two requests are errors) |
| `game/` | Conway's Life in a fixed-step game loop | `fid run`, `fid ccompile` | both |
| `cbind/` | C binding: libc and libm through `extern "C"` | `fid ccompile` | the binary |

Every project's sources must also be clean under `fid lint` (the library's
app is not linted: `lint` does not resolve `use` of a pact).

The cli's input files are written by the script, not kept here, so their
byte counts do not depend on how git checks out line endings.
