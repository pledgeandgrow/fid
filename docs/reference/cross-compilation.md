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
| `aarch64-ios` / `ios-arm64` (target roadmap XT77) | the machine layer, runtime linux_runtime/darwin.fd | a Mach-O, LC_BUILD_VERSION platform iOS (2), ad-hoc signature: structure checked, not run (below) |
| `aarch64-ios-simulator` / `iossim-arm64`, `x86_64-ios-simulator` (XT77) | the same | a Mach-O, platform iOS simulator (7) |
| `aarch64-windows` / `windows-arm64` (XT22) | the machine layer, once it writes ARM64 PEs (XT14) | none yet: an error naming XT14 (below) |
| `aarch64-android` / `android-arm64`, `aarch64-android<api>`, NDK's `aarch64-linux-android<api>` (XT64) | the machine layer, runtime stdlib/linux_runtime/ (Android is the Linux kernel) | a static PIE (ET_DYN, no PT_INTERP), `.note.android.ident` with the API level (34 by default): structure checked, not run (below) |
| `x86_64-freebsd` / `freebsd-x64`, `aarch64-freebsd` (XT44, XT50) | the machine layer; no FreeBSD runtime yet (XT43) | none yet: an error naming XT43; `FID_IMAGE_ONLY=1` writes the branded ELF (EI_OSABI FreeBSD, `.note.tag`) over the Linux runtime, for its headers only (below) |
| `x86_64-openbsd` / `openbsd-x64`, `aarch64-openbsd` (XT54) | the machine layer; no OpenBSD runtime (XT53, XT55) | none yet: an error naming XT53; `FID_IMAGE_ONLY=1` writes the ELF with `.note.openbsd.ident` and `PT_OPENBSD_NOBTCFI` (below) |

The OS-first names (`linux-arm64`, `windows-x64`, `darwin-x64`, `ios-arm64`,
`iossim-arm64`, `freebsd-x64`, `openbsd-x64`, `android-arm64`, also `amd64`
and `x64` for x86-64) are read by
`normalize_target` in compiler/main.fd and mean the same as the
`<arch>-<os>` form.

### iOS (XT77)

`--target aarch64-ios` (a device) and `--target aarch64-ios-simulator`
(also `x86_64-ios-simulator`, the simulator on an Intel Mac) build the same
Mach-O as `aarch64-macos`: the machine layer's code, the darwin runtime
(libSystem calls only, no raw system calls), dyld binding libSystem, LC_MAIN,
an ad-hoc signature. Only LC_BUILD_VERSION differs: platform 2
(PLATFORM_IOS) or 7 (PLATFORM_IOSSIMULATOR), minimum iOS 15.0, SDK 17.0. A
minimum version may follow `ios`: `aarch64-ios17.2`,
`aarch64-ios16.4-simulator`, also LLVM's spelling `arm64-apple-ios15.0`
(the SDK field is then 17.0 or the minimum, whichever is newer). `@cfg`
sees `macos` on these targets (the same libSystem). `fid ccompile --target
aarch64-ios` is refused: the C route has no iOS SDK.

Checked by `tests/nattools/xt_ios_target_check.sh` (llvm-objdump
`--macho --private-headers`, `scripts/macho-check.py --platform=2|7`), on
any host; nothing has run one on the simulator or a device yet. What a real
iOS program still needs: a device accepts only a real signature with
entitlements (XT81), apps are `.app` bundles (XT82), the simulator run in CI
(XT80), and the runtime's App Store limits (XT79): darwin.fd calls
`__ulock_wait`/`__ulock_wake` (private), `fork`/`execve` (denied in the
sandbox), and `extern "C" link` checks other dylibs against the macOS SDK.

### Windows on ARM64 (XT22)

`--target aarch64-windows` (also `windows-arm64`, `arm64-windows`, with or
without `--nat`, and `fid emit-ir --target aarch64-windows`) belongs to the
machine layer, which has AArch64 code but writes x86-64 PEs only (nat_enc.fd
`nat_pe_img`, machine 0x8664); pe_ir is x86-64 only. So the build stops with
an error naming the missing pieces (XT14: the ARM64 PE/COFF headers,
machine 0xAA64; XT15-XT18: the calling convention, unwind data, imports,
runtime) and writes nothing; before XT22 it said "Windows executables are
x86-64". `fid ccompile <file> --target aarch64-windows` builds one through
zig cc. When XT14 lands, the same check requires the PE's machine to be
0xAA64.

### FreeBSD, OpenBSD and Android ELFs (XT44, XT50, XT54, XT64)

These targets go to the machine layer (compiler/main.fd
`nat_elf_target_os`; before, `fid build|emit` with them fell through to
pe_ir and silently wrote a Windows PE). Its ELF writer (nat_enc.fd
`nat_elf_img`, `g_nat_elf_os`) brands the image for the system; a Linux
image is unchanged. The note sits at file offset 0xF00, in the header page
the first PT_LOAD maps (read-only), with a PT_NOTE and an SHT_NOTE section
of its own; every note is namesz 8, descsz 4, type 1.

| Target | EI_OSABI | Note (section, owner, word) | Other program headers |
|---|---|---|---|
| `*-freebsd` | 9 (ELFOSABI_FREEBSD) | `.note.tag`, `FreeBSD`, NT_FREEBSD_ABI_TAG = `__FreeBSD_version` 1400097 (14.0-RELEASE) | none |
| `*-openbsd` | 0 (as OpenBSD's own linker; the kernel takes the note) | `.note.openbsd.ident`, `OpenBSD`, 0 | `PT_OPENBSD_NOBTCFI` (the code has no endbr64 / BTI landing pads); `PT_OPENBSD_WXNEEDED` only when code and data share one RWX segment (`FID_NAT_OFF=wxsplit`): by default the code is an R+X PT_LOAD and the data an RW one, so W^X holds. No `PT_OPENBSD_RANDOMIZE`: the image has no random-data section |
| `*-android<api>` | 0 | `.note.android.ident`, `Android`, the API level (34 when the target names none; 21-99) | the Linux ones: ET_DYN, R+X code, RW data, PT_GNU_STACK |

**Android** is the Linux kernel, so the Linux runtime's raw system calls
run there and the image is complete: a static PIE, ET_DYN with no
PT_INTERP, which the kernel maps at a random address and whose `_start`
adds the load bias to its absolute words itself (TG14, nat_ir.fd
`ng_gen_pie_reloc`). PIE is forced (`--no-pie` is an error): the
alternative, an ET_DYN with PT_INTERP `/system/bin/linker64`, needs the
dynamic linking against bionic of XT65 (DT_NEEDED libc.so, the linker's
rules), and nothing in a program that calls no C function needs it. `@cfg`
sees `linux`. A program that calls C functions (`extern "C"`) is refused
on these three systems: the machine layer links those for glibc's loader
only (XT65 for bionic, XT53 for OpenBSD, XT43-XT45 for FreeBSD).

**FreeBSD** needs its own runtime (XT43: FreeBSD's system call numbers,
errors in the carry flag) before an image runs; the build stops with an
error naming XT43 (`fid ccompile --target x86_64-freebsd`, through zig cc,
is the working route). **OpenBSD** takes system calls only from libc
(pinsyscalls; a static executable with raw system calls is refused since
7.5), so its images need the dynamic libc link of XT53 and the runtime of
XT55; the build stops with an error naming XT53. For both,
`FID_IMAGE_ONLY=1` writes the branded image over the Linux runtime so its
headers can be checked: it does not run on that system. `@cfg` sees
`freebsd` or `openbsd`. Riscv64 is refused for the three.

Checked by `tests/nattools/xt_elf_notes_check.sh` (the bytes of EI_OSABI,
the program headers and the notes; `llvm-readelf -h -l -n`;
scripts/image-lint.py), on any host; nothing has run one on FreeBSD,
OpenBSD or Android yet. Open on the real systems: a FreeBSD kernel
accepting the brand (XT51's VM), OpenBSD's execute-only text (the machine
layer may read constants from its R+X segment), Android under `adb shell`
(XT68).

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
(tier 2; the machine layer writes its branded image but has no FreeBSD
runtime yet, XT43). zig ships no libc for OpenBSD or Android (bionic needs
the NDK), so `ccompile --target x86_64-openbsd|aarch64-android` stops in
zig. `ccompile` with no `--target` uses the host's C compiler (CC, else
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
