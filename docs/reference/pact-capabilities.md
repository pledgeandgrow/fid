# Pact capabilities in every build (CD1)

`ark.toml` grants each dependency (pact) the capabilities it may use:

```toml
[capabilities]
json        = []                 # pure code: nothing
http_client = ["net"]
tool        = ["fs", "process"]  # "*" grants everything
```

Once the section is there, a pact it does not list is granted nothing.
`ark install`, `ark build`, `ark test`, `ark audit` and `ark update` check
the pacts installed for the project (compiler/ark.fd `ark_cap_check`, a scan
of each pact's text; `ark update` prints the capability diff).

## Direct builds

`fid run`, `fid check`, `fid build`, `fid ccompile` (every command that goes
through `build_program_with_path_borrowck` in compiler/compiler.fd) check the
same grants for the pacts the program imports, so a build run without `ark`
fails the same way:

```
error: Capability error: pact 'tmplpact' uses capabilities ark.toml does not grant it:
  fs  ./ark_modules/tmplpact/src/lib.fd (page -> render_file -> p_file_read)
  grant them under [capabilities] (tmplpact = ["fs"]) or drop the pact (./ark.toml)
```

A pact is a module the import loop found in `ark_modules/` (or
`ark_modules/.dev/`, or `vendor/`) of the project or of its workspace root
(`cd1_pact_of`). In a workspace member the root's `ark.toml` grants, as for
`ark`.

What a pact uses is taken from the checked program, not from its text
(compiler/compiler.fd, section "CD1"):

| capability | how it is found |
|---|---|
| net, fs, process, env, ffi | the calls of the pact's functions, followed through the stdlib and core down to the runtime builtins (`cd1_builtin_cap`: `p_socket_*`, `p_tcp_*`, `p_file_*`, `p_dir_*`, `p_system`, `p_getenv`, `p_dl_*`, ...). Methods are followed by name when the type checker inferred IO or Net for them. The walk stops at another pact's functions: each pact answers for its own. |
| net | also when the type checker inferred the Net effect for a pact function (calls through function values and methods the walk does not resolve) |
| ffi | an `extern` declaration in the pact |
| io, crypto, ... | what the pact's imports ask for: `use core with { IO }` |
| unsafe | `unsafe {` in the pact's files |

So a pact that reaches the file system through a stdlib module (here
`template::render_file`) needs `fs`, though its own text names no file
function; a pact that only mentions `p_socket_connect` in a comment needs no
`net`. The error shows the call path that showed each capability.

Differences from `ark`'s check: only the pacts the program imports are
checked (an installed pact that is not imported is not linked into the
program; `ark audit` still checks it). `FID_ARK_CAPS=off` skips the check of
a direct build.

Test: `tests/tooling/cd1-direct-caps-check.sh` (FID=<fid>).
