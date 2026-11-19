# `fid serve`: a long-lived compiler

`fid serve` keeps the parsed standard library, the merged native runtime and
the parses of imported modules in memory between requests, so the second
check or build of a program does not lex, parse or check the library again.
The code is in `compiler/main.fd` (`cmd_serve`, `serve_request`,
`serve_build`, `serve_socket`) and `compiler/compiler.fd` (the client side,
`serve_client_ask`, and `check_text_errors`).

## On stdin

```
fid serve
```

One request a line; each answer ends with a line `done <status>` (0: no
errors, 1: errors, 2: a request it does not know) and stdout is flushed:

| Request | Does |
|---|---|
| `check <file.fd>` | the checks of `fid check <file.fd>` |
| `emit <file.fd> [flags]` | `fid emit <file.fd> [flags]`, same output files |
| `ccompile <file.fd> [flags]` | `fid ccompile <file.fd> [flags]`, same C and executable |
| `quit` | ends the server (so does the end of stdin) |

The words of an `emit` or `ccompile` request stand for the command line
while it runs (`inc.fd argv_at`), so every flag the one-shot command reads
works the same; the file comes first. After each one, what it set for the
build (the runtime merged into the library, the `@cfg` target, `--debug`,
`--time`) is put back as a new process has it
(`compiler.fd serve_reset_build_state`).

## On a TCP port: shared with `fid check` and the LSP

```
fid serve --port 7070
FID_SERVE=7070 fid check app.fd       # or: fid check app.fd --server 7070
FID_SERVE=7070 fid lsp                # the editor's diagnostics and quick fixes
```

The server writes a random key to `<home>/.fid/serve-<port>.key`
(`USERPROFILE` on Windows, else `HOME`) and removes it when it stops. A
client connects to 127.0.0.1, sends `key <key>`, one request and reads the
answer to `done <status>`. Lines of an answer:

- `e <error>`: an error, with `\` written `\\` and line ends `\n`/`\r`, so
  an error of several lines is one line;
- `o <note>`: anything else (the "no errors" line, the status of a build);
- `done <status>`.

Requests on the socket: `check <file.fd>`, `check-text <bytes> <path|->`
followed by that many bytes of text (what the LSP sends: the editor's
unsaved text), `emit ...`, `ccompile ...`, `quit` (ends the connection) and
`stop` (ends the server). One client is served at a time; the clients make
one connection per request.

`fid check` through the server prints the errors as it prints its own. When
no server answers (no key file, nothing listening, a wrong key) or the
server cannot read the file (it was started in another directory and the
path is relative), `fid check` and the LSP check by themselves, with the
same result.

The port listens on every interface; the key is what keeps out anyone who
cannot read the user's files. An `emit` or `ccompile` through the socket
prints its own output on the server's stdout; the client gets its status.

Checks: `tests/tooling/serve-modcache-check.sh` (module parses reused),
`tests/tooling/serve-build-check.sh` (emit and ccompile give the one-shot
outputs), `tests/tooling/serve-socket-check.sh` with
`tests/tooling/serve_lsp_probe.py` (`fid check` and the LSP through the
server: same diagnostics, the second check faster).

## The machine-code cache

Build requests share an in-memory machine-code cache (the machine layer's
per-function cache, `nat_ir.fd`; `cmd_serve` sets `g_nat_fncache_mem`): a
build for a nat_ir target (`--target *-linux`, `--target *-macos`, `--nat`)
replays every function unchanged since an earlier request instead of
generating it again, and no cache file is read or written. A request with
`--no-cache` or `--no-fncache` builds without it. `FID_FNCACHE=<file>`
still names a file instead. pe_ir builds (the Windows default) do not use
it.

After each request, the back ends' flags it set are reset
(`main.fd serve_reset_backend_state`): `--debug`, `--size`, `--stats`,
`--cpu`, `--leaks`, `--checked-alloc`, `--load` and `--remarks`, so one
request's flags never reach the next.

Check: `tests/tooling/serve-fncache-check.sh`.
