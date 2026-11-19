# Cross-compilation: what needs a toolchain and what does not

`fid build` and `fid emit` write executables for another operating system
from any host with nothing else installed: no C compiler, no linker, no
sysroot, no download. `fid ccompile --target` is the C route and needs
[zig](https://ziglang.org) by design. This page says which command takes
which route and why (competitive roadmap TL6; platforms.md has the status of
each target).

## The no-toolchain path: `fid build` / `fid emit`

| `--target` (either spelling) | Backend | Output |
|---|---|---|
| none, on Windows | pe_ir (compiler/pe_ir*.fd) | a PE for this machine |
| none, on Linux or macOS | the machine layer (compiler/nat_ir.fd, nat_enc.fd) | an ELF or a signed Mach-O for this machine |
| `x86_64-linux` / `linux-x64`, `aarch64-linux` / `linux-arm64`, `riscv64-linux` / `linux-riscv64` | the machine layer, runtime stdlib/linux_runtime/ | a static ELF |
| `x86_64-macos` / `macos-x64`, `aarch64-macos` / `macos-arm64` | the machine layer, runtime linux_runtime/darwin.fd | a Mach-O (PIE, ad-hoc signature) |
| `x86_64-windows` / `windows-x64` | pe_ir, runtime stdlib/pe_runtime/ | a PE; `@cfg` sees Windows; `use tls` takes the Windows module |
| `x86_64-windows` with `--nat` | the machine layer over kernel32 (linux_runtime/win.fd) | a PE |

The OS-first names (`linux-arm64`, `windows-x64`, `darwin-x64`, also `amd64`
and `x64` for x86-64) are read by `normalize_target` in compiler/main.fd and
mean the same as the `<arch>-<os>` form.

Every backend in this table is Fides code inside `fid`: machine code, the
object layout (ELF, PE, Mach-O) and the runtime are written by the compiler,
so the same `fid` builds every target on every host. A Linux `fid` builds a
Windows program the same way a Windows `fid` does:

```bash
fid build hello.fd --target windows-x64 -o hello.exe   # on Linux or macOS
fid build hello.fd --target linux-arm64 -o hello        # on Windows or macOS
```

## The C route: `fid ccompile --target` needs zig

`fid ccompile` writes C and hands it to a C compiler. Writing the C is the
same for every target, but compiling it for another OS needs a C compiler
that targets that OS **and** that OS's C library headers, startup files and
import libraries: musl for static Linux executables, MinGW for Windows, the
macOS SDK stubs, wasi-libc for WebAssembly, FreeBSD's libc stubs. `zig cc`
ships all of them for every target in one download, so `ccompile --target`
uses `zig cc -target <triple>` (`cross_zig_triple` in compiler/main.fd:
`x86_64-linux-musl`, `x86_64-windows-gnu`, `aarch64-macos`, `wasm32-wasi`,
`x86_64-freebsd`, ...). `fid` does not bundle a C toolchain: that would
make it a C distribution, and the no-toolchain path above already covers
the targets most people build for.

Without zig, `ccompile --target <t>` looks for `<t>-gcc` or `<t>-clang` on
PATH; when neither exists it warns and falls back to the host's C compiler,
which builds for the host, not for `<t>`. Use `fid build --target <t>` then.

Targets only the C route reaches today: `wasm32-wasi` through C (the native
`fid wasm` needs no C compiler), `aarch64-windows`, `x86_64-freebsd`
(tier 2). `ccompile` with no `--target` uses the host's C compiler (CC, else
gcc, clang, zig cc).

## How it is checked

- `tests/tooling/build-no-toolchain-check.sh`: `fid build` with a PATH that
  holds no cc, gcc, clang, zig or ld; on Linux or macOS it also builds
  `--target windows-x64` and runs the PE under wine. CI (ci.yml `selfhost`)
  runs it on Linux with wine and `REQUIRE_WINE=1`.
- `scripts/cross-from-linux.sh`: on a Windows host with WSL, builds a Linux
  `fid` (`fid emit compiler/main.fd --target x86_64-linux`), runs it in WSL to
  build hello and the suite for Windows (`cross-test.sh x86_64-windows-pe`,
  built only), runs every PE on Windows itself and compares each with
  `fid run`; every test that passes when built on Windows must pass when
  built on Linux. (WSL's interop is not used to run the PEs: it does not
  pass the exit code back.) On a Linux host it runs the same steps with the
  PEs under wine.
- `scripts/cross-test.sh x86_64-windows-pe` / `x86_64-windows` with
  `NATIVE=1` on Linux or macOS: the suite as Windows PEs, run under wine when
  it is installed (CI: ci.yml `natgate`, x86_64-linux, reported).
  `tests/tooling/tl6-cross-route-check.sh` checks that routing with a
  stand-in fid and wine.
- `scripts/tl-check.sh`: hello built for each OS-first target name.
