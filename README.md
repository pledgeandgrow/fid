<p align="center">
  <a href="https://fid-lang.org">
    <img src="docs/fid-logo.svg" alt="Fid" width="96">
  </a>
</p>

<h1 align="center">Fid</h1>

<p align="center">
  <b>A systems language that checks what it promises.</b><br>
  Borrow checking without lifetime annotations, capability grants on imports, effect tracking.<br>
  Its compiler, <b>Fides</b>, is written in Fid and builds itself to a byte-identical fixpoint. No LLVM. No GC.
</p>

<p align="center">
  <a href="https://fid-lang.org/playground"><img alt="Try it in your browser" src="https://img.shields.io/badge/try%20it-in%20your%20browser-C2255C?style=for-the-badge"></a>
  <a href="https://fid-lang.org/docs/install"><img alt="Install" src="https://img.shields.io/badge/install-v0.8.6--unstable-262626?style=for-the-badge"></a>
  <a href="https://fid-lang.org/docs"><img alt="Documentation" src="https://img.shields.io/badge/read-the%20docs-262626?style=for-the-badge"></a>
</p>

<p align="center">
  <a href="https://github.com/pledgeandgrow/fid/actions/workflows/ci.yml"><img alt="CI" src="https://github.com/pledgeandgrow/fid/actions/workflows/ci.yml/badge.svg"></a>
  <a href="https://github.com/pledgeandgrow/fid/releases"><img alt="Release" src="https://img.shields.io/github/v/release/pledgeandgrow/fid?include_prereleases&color=C2255C"></a>
  <a href="LICENSE"><img alt="License: MIT" src="https://img.shields.io/badge/license-MIT-C2255C"></a>
  <a href="https://github.com/pledgeandgrow/fid/discussions"><img alt="Discussions" src="https://img.shields.io/badge/chat-discussions-262626"></a>
</p>

<p align="center">
  <a href="#thirty-seconds-of-fid">Taste</a> ·
  <a href="#what-the-compiler-refuses">What it catches</a> ·
  <a href="#install">Install</a> ·
  <a href="#the-toolchain">Toolchain</a> ·
  <a href="#where-it-runs">Platforms</a> ·
  <a href="#how-it-works">How it works</a> ·
  <a href="#honest-status">Status</a> ·
  <a href="#contributing">Contributing</a>
</p>

---

## Thirty seconds of Fid

```rust
// hello.fd
fn main() {
    val name = "world";
    var total = 0;

    for i in 1..=100 {
        total += i;
    }

    println(f"Hello, {name}! total = {total}");
}
```

```bash
fid run hello.fd                 # interpret it: no build step
fid emit hello.fd -o hello.exe   # native executable, written by the compiler itself
fid ccompile hello.fd -o hello   # or through C, for every other target
```

`val` is immutable, `var` is mutable, types are inferred, and `f"..."` strings are part of the language. If you know Rust, Go, Zig or C, you can read Fid today: [what carries over and what differs](https://fid-lang.org/docs/coming-from).

## What the compiler refuses

Three real programs and what `fid` says about each (messages shortened to fit).

<table>
<tr>
<td width="33%" valign="top">

**A loop cannot change what it walks**

```rust
var v: Vec<i64> = Vec::new();
v.push(1);
for x in v {
    v.push(x);
}
```

```
error: borrow error in main:
cannot change v while a `for`
loop iterates over it
```

</td>
<td width="33%" valign="top">

**A module only gets what you grant**

```rust
use logger with { FS };

fn main() -> i64 {
    log_line("started");
    return 0;
}
```

```
Capability error: 'println()'
requires 'IO' capability (not
granted to module 'logger')
```

</td>
<td width="33%" valign="top">

**A pure function stays pure**

```rust
fn pure_sum(a: i64, b: i64)
    -> i64 !{} {
    return a + b + helper();
}
```

```
Effect error: function
'pure_sum' has inferred
effect '!IO'
```

</td>
</tr>
</table>

Every message is explained on the [Compiler Errors](https://fid-lang.org/docs/errors) page.

## Why Fid

|     |     |
| --- | --- |
| **No lifetime annotations** | References are second-class: they cannot be stored or returned from a local, so there is nothing to annotate. Assignment copies; nothing moves out from under you. |
| **Capabilities on imports** | `use lib with { IO }` and the compiler rejects that module's calls to the network or filesystem. Your `use` lines show what can touch what. |
| **Effects in signatures** | `!IO`, `!Net`, `!{}`: inferred across the call graph, and under-declaring is a compile error. Handlers are resumable. |
| **21 safety rules, each with a test that must fail** | `@no-alloc`, `@bounded-stack`, `@constant_time`, `@secret` and more. `fid qualify` writes the evidence as JSON. |
| **Checks that stay on** | Bounds, integer overflow and double frees panic in every build, not only in debug. |
| **No macros** | `comptime` runs ordinary Fid code at compile time, with reflection over types. |
| **No LLVM** | `fid emit` encodes machine code and writes Windows and Linux executables itself: no assembler, no linker, no C compiler. |
| **Self-hosted** | About 361K lines of Fid compile themselves. The 3-stage release bootstrap checks they rebuild to byte-identical output (last reached Nov 21); a self-compile to C takes 44 s (Nov 27). |

## Install

**Windows.** Download [`Fid-Setup-0.8.6.exe`](https://github.com/pledgeandgrow/fid/releases/tag/v0.8.6-unstable) (older versions on the [releases page](https://github.com/pledgeandgrow/fid/releases)). It puts `fid.exe`, `ark`, `scribe` and `veil` on your PATH with the standard library beside them. The installer is not code-signed yet, so Windows will ask you to confirm. `fid ccompile` needs a C compiler on PATH (`zig cc`, gcc, clang or tcc); `fid run` and `fid emit` need none.

**Linux and macOS, or from source anywhere.** You need git, Rust and a C compiler (`zig cc` 0.16 is the tested one):

```bash
git clone https://github.com/pledgeandgrow/fid && cd fid
cargo build --release                          # the Rust bootstrap compiler and tools
RELEASE=1 CC="zig cc" ./scripts/bootstrap.sh   # builds fid in 3 stages and checks the fixpoint
./release/fid run tests/bootstrap/hello.fd
```

Stage 0 is a pinned self-hosted `fid`: `bootstrap.sh` downloads it from the [`stage0` release](https://github.com/pledgeandgrow/fid/releases/tag/stage0) and checks its hash (`scripts/stage0-fetch.sh`). Binaries are published there for Windows x64 and ARM64, Linux x64 and ARM64, and macOS x64 and ARM64. The Linux ARM64 and macOS ones were cross-built and pinned on Nov 27 and have not yet built `fid` in CI. On a host with no pinned binary, it falls back to the Rust compiler, and that cold start stops at stage 2: a `fid` built by `fidesc` runs out of memory compiling the whole tree ([known bug](docs/reference/known-bugs.md) 2g.79). There, set `STAGE0` to a self-hosted `fid` built elsewhere.

**Nothing to install.** The [playground](https://fid-lang.org/playground) runs in your browser.

Editor support is in [`editors/`](editors/README.md): configurations for VS Code, Neovim, Helix, Emacs and Sublime, and a Zed extension with a Tree-sitter grammar. `fid lsp` speaks the Language Server Protocol (references, rename, signature help, inlay hints, code actions, formatting), checked by `scripts/tl/lsp_*.py`. The CI job that starts Neovim, Emacs and Helix against it has not passed yet.

## The toolchain

One binary, `fid`, is the compiler, interpreter, test runner, formatter, linter and language server.

| Command | What it does |
| --- | --- |
| `fid run file.fd` | Interpret a program. `--leaks` reports memory never freed |
| `fid emit file.fd -o out` | Native executable with no other tools. `--target x86_64-linux`, `aarch64-linux` or `riscv64-linux` for Linux, `universal` for a macOS fat binary; `--bare`/`--boot` bare metal, `--uefi` a UEFI image |
| `fid ccompile file.fd -o out` | Through C; `--target` cross-compiles with `zig cc` |
| `fid wasm file.fd` | WebAssembly/WAT, no C compiler |
| `fid check file.fd` | Type, borrow and effect checks only |
| `fid explain-error E044` | What an error code means, with an example and the fix |
| `fid fix file.fd` | Apply the compiler's suggested fixes |
| `fid test file.fd` | Run every `test_*` function, each in its own process |
| `fid fuzz file.fd` | Run its `fuzz_*` functions on generated inputs |
| `fid fmt` / `fid lint` | Format; unused code and `unwrap()` outside tests |
| `fid audit file.fd` | List every `unsafe` block the program reaches |
| `fid qualify file.fd` | Safety evidence as JSON: rules, their tests, annotated functions |
| `fid explain file.fd` | Which run-time checks were removed or kept, and why |
| `fid debug file.fd` | Interactive debugger (breakpoints, stepping, variables) |
| `fid lsp` | Language server over stdio |

Beside it: `ark` (packages: a path, a git repository, or [the pacts index](https://github.com/pledgeandgrow/pacts) — `fid ark add textkit` installs the first published pact), `scribe` (documentation from `///` comments) and `veil` (coverage). The full list is in the [CLI reference](https://fid-lang.org/docs/cli).

## Where it runs

| Platform          | Interpreter | Through C | Native (`fid emit`) |
| ----------------- | :---------: | :-------: | :-----------------: |
| Windows x64       |     🟢      |    🟢     |         🟢          |
| Linux x64         |     🟢      |    🟢     |         🟡          |
| Linux ARM64       |     🟡 ¹    |    🟡     |         🟡          |
| Linux RISC-V      |     🟡 ¹    |    🟡     |         🟡          |
| macOS x64 + ARM64 |     🟡      |    🟡     |         ⚪          |
| WebAssembly       |             |    🟢     |         🟡          |
| Bare metal x86-64 |             |           |         🟢 ²        |

¹ Under QEMU in CI. ² Multiboot kernels booted and verified under QEMU in CI (`bare-metal.yml`, last green Nov 22). macOS: Mach-O executables build from any host, and CI runs the suite on Apple runners (the `macos-arm64` lane of `tier1.yml`, and `macos-native.yml` for native executables), but neither has passed yet, so nothing has been confirmed on a Mac.

🟢 supported · 🟡 partly tested · ⚪ experimental. Details and test counts: [platforms](docs/reference/platforms.md).

## How it works

```mermaid
flowchart LR
    S([main.fd]) --> L[Lexer] --> P[Parser] --> C[Checks<br/>types · borrows · effects]
    C --> M[Monomorphizer] --> IR[SSA IR<br/>optimizer]
    IR --> I[Interpreter<br/>fid run]
    IR --> CB[C backend<br/>fid ccompile]
    IR --> N[Native emitter<br/>fid emit]
    CB --> CC[Any C compiler]
    N --> EXE([PE and ELF executables])
```

One front end and one IR, three ways out. The native emitter has its own instruction encoders and register allocator, so no assembler or linker is involved.

The compiler checks itself as hard as it checks your code:

- **Fixpoint.** Stage 2 and stage 3 of the bootstrap must produce byte-identical output on every release build.
- **Proved rewrites.** The optimizer's 27 integer rewrite rules and the e-graph's rewrites are proved with z3 at 8, 16, 32 and 64 bits: 253 obligations, all proved (Nov 25).
- **Checked encoders.** The instruction encoders are compared with clang's assembler across 1,200 forms, with no mismatch (Nov 25).
- **Conformance.** 1,621 programs each check one rule of the [language reference](docs/reference/reference.md) (§1–§13).

More in [verification](docs/verification/README.md) and the [contributor walkthrough](docs/reference/compiler-walkthrough.md).

## Standard library

403 modules, written in Fid. A sample:

| Area | Modules |
| --- | --- |
| Core and collections | `core`, `vec`, `collections`, `string`, `rc`, `arena`, `iter`, `sort` |
| Text and data | `json`, `toml`, `yaml`, `csv`, `regex`, `unicode`, `protobuf`, `cbor` |
| Network | `http`, `http2`, `http3`, `quic`, `net`, `dns`, `ws`, `grpc`, `ssh`, `webrtc`, `mqtt` |
| Security | `crypto`, `tls13`, `x509`, `sha512`, `ed25519`, `x25519`, `wireguard`, `pqc`, `hpke`, `typed_strings`, `privacy` |
| Databases | `sqlite`, `postgres`, `mysql`, `mongodb`, `redis`, `db`, `sql_builder` |
| Concurrency | `task`, `channel`, `sync`, `parallel`, `async` |
| Numbers and models | `math`, `bigint`, `decimal`, `tensor`, `simd`, `infer`, `safetensors`, `bpe` |
| Industrial and energy | `modbus`, `opcua`, `iec104`, `iec61850`, `dnp3`, `scada`, `plc`, `iso15118`, `matter` |
| Automotive and robotics | `can`, `ecu`, `mavlink`, `ros2_msg`, `dds`, `kalman`, `planner`, `lidar` |
| Systems and embedded | `fs`, `process`, `mmap`, `elf`, `pci`, `uart`, `virtio`, `nvme`, `ext4`, `gpio`, `i2c`, `spi` |
| Games and graphics | `ecs`, `physics`, `mesh`, `gltf`, `tilemap`, `shader`, `sdl`, `vulkan`, `d3d12`, `webgpu` |
| Finance | `money`, `banking`, `quant`, `decimal` |

Signatures for every module: [API reference](https://fid-lang.org/reference).

## Honest status

Fid is young and moving fast.

- **Releases:** the newest is the **[v0.8.6-unstable](https://github.com/pledgeandgrow/fid/releases/tag/v0.8.6-unstable)** prerelease (Nov 27). Its `fid.exe` is a one-stage build from a self-hosted `fid` (all 16 C units at -O2), not a 3-stage fixpoint. The first published build, Sep 9, was tagged v1.0.0 and is now listed as 0.7. The stable 1.0 is planned for December; `fid version` reports `1.0.0+dev`.
- **Generated code is slower than Rust's.** These are the Nov 17 runs on a busy Windows machine, against Rust, Zig, C++ and Go.
  - **Through C:**
    - level with the fastest on a small compiler and on JSON (within 15%);
    - faster than Rust and Go on binary-trees;
    - 1.2x behind on hashing, 1.5x on regex, 2–4x on string building, 3x on file reading and 5x on sorting;
    - 11–18x behind on pidigits, where its big integers are slow.
  - **Natively (`fid emit`):** slower than through C on every program. But the executables are the smallest in the table (40 KB for hello, against 123 KB for Rust), and they start as fast as Zig's.
  - [Benchmarks, losses included](docs/benchmarks.md).
- **The package index is just starting.** `ark` installs from a path, a git repository or [the pacts index](https://github.com/pledgeandgrow/pacts) (`fid ark add textkit`); few pacts are published yet. The CI steps that install from the index on Linux, Windows and macOS have not passed yet.
- **The fixpoint was last verified on Nov 21, 2026:** stage 2 and stage 3 were byte-identical on that day's ~361K-line tree. Builds since (Nov 25, Nov 27, v0.8.5/v0.8.6-unstable) are one stage from a self-hosted `fid`; the Nov 25 one passes the leak gate (11 of 11).
- **The whole test suite on each backend.**
  - **Interpreter (Nov 25):** 1,897 of 1,983 pass on `run`, with `fid` as the compiler. 82 of the failures fail the same with the earlier compiler; 4 are timeouts.
  - **Native, both backends (Nov 27, the first such run):** 2,011 of the 2,056 programs in `tests/bootstrap` pass on `emit` and on `--nat`.
    - 38 fail on both: 13 don't compile, 8 hit PE generation errors, and the rest are bare-metal or boot programs and tests that need SDL2 or a C header.
    - 7 fail on `--nat` only.
    - Details in the [LLVM roadmap](docs/roadmap/llvm-roadmap.md), WP14.
  - **C:** not rerun.
- **CI is red.**
  - None of the runs GitHub still has for `ci.yml` (88), `tier1.yml` (89) or `macos-native.yml` (20), going back to Nov 21, has passed.
  - Most jobs stop while building `fid`, on the stage-0 binaries and the memory of GitHub's runners.
  - `bare-metal.yml` and `pages.yml` last passed on Nov 22.
  - The checks above were run by hand on a Windows machine.
- **Open bugs.** Four whole-repo reviews (Nov 22, Nov 24 and two on Nov 27) logged 489 entries in the [bugs roadmap](docs/roadmap/bugs-roadmap.md): 232 are fixed with tests, 27 partly, 230 open. Most of the open ones come from the Nov 27 reviews and are still being worked down.
- **Not for safety-critical use.** `fid qualify` produces evidence for review, not a certification.

The details: [status](docs/reference/status.md) · [known bugs](docs/reference/known-bugs.md) · [roadmap](docs/roadmap/roadmap.md).

## Learn

| | |
| --- | --- |
| [The Fid Book](docs/book/01-intro.md) | First program to a network service |
| [University course](docs/course/README.md) | 13 weeks of lecture notes: memory safety, effect systems, capability security, systems programming; a [lab per week](docs/course/labs/README.md) that opens in the playground |
| [Coming from Rust](docs/guides/from-rust.md) · [Go](docs/guides/from-go.md) · [Zig](docs/guides/from-zig.md) · [C](docs/guides/from-c.md) · [Python](docs/guides/from-python.md) | What you already know, mapped onto Fid |
| [Language reference](docs/reference/reference.md) | Every rule, with the compiler function that enforces it |
| [Cookbook](docs/guides/cookbook.md) | Files, JSON, HTTP, tasks |
| [Examples](examples/) | A JSON parser, an HTTP server, a mini compiler, a neural network, a game loop |
| [INDEX.md](INDEX.md) | A map of every document and compiler module |

## Repository layout

```
compiler/    the compiler, in Fid: 65 modules, about 361K lines
stdlib/      the standard library: 403 modules
src/         the earlier compiler in Rust, frozen; it builds stage 1 and runs the test harness
tests/       bootstrap (2,131 programs), conformance (1,621), selfhost (47),
             plus projects, verify, nattools, tooling and leak checks
examples/    twelve complete programs
tools/       ark, scribe, veil and friends, written in Fid
docs/        book, guides, reference, proposals, roadmap, verification
editors/     VS Code, Neovim, Helix, Emacs, Sublime, Zed
installer/   the Windows installer
bench/       benchmark programs in Fid, Rust, Zig, C++ and Go
scripts/     bootstrap, cross-target tests, fuzzing, verification
```

## Contributing

Contributions are welcome, from a test program to a compiler change.

1. Read [CONTRIBUTING.md](CONTRIBUTING.md) and, before touching `compiler/*.fd`, [AGENTS.md](AGENTS.md) and the [compiler internals](docs/reference/compiler-internals.md).
2. Build with the two commands under [Install](#install).
3. Keep a change small: one fix, one test. `RELEASE=1 ./scripts/bootstrap.sh` must still reach its fixpoint.
4. Language changes start as a [proposal](docs/proposals/0000-template.md).

Questions go to [Discussions](https://github.com/pledgeandgrow/fid/discussions), bugs to [issues](https://github.com/pledgeandgrow/fid/issues), and security problems privately through [SECURITY.md](SECURITY.md). Miscompile reports get priority.

## Questions people ask

<details>
<summary><b>How is Fid different from Rust?</b></summary>

No lifetime annotations: references cannot be stored or returned from a local. Assignment copies instead of moving. No macros; `comptime` instead. Capability grants and effect annotations are built in. Fid is younger, its package index is just starting, and its generated code is slower. [Full comparison](docs/guides/from-rust.md).

</details>

<details>
<summary><b>How is Fid different from Zig?</b></summary>

Both have `comptime` and their own backends. Zig leaves memory safety to the engineer; Fid checks borrows and reference escapes at compile time and keeps bounds and overflow checks in every build. [Full comparison](docs/guides/from-zig.md).

</details>

<details>
<summary><b>Why build a toolchain without LLVM?</b></summary>

Control. The IR carries Fid's own semantics (regions, effects, capabilities), and there is no external toolchain to pin. The cost is optimizer maturity: generated code is slower than LLVM's today.

</details>

<details>
<summary><b>Is it production-ready?</b></summary>

Not yet for most teams. Pilot it on a bounded tool and read [the status page](docs/reference/status.md) first.

</details>

<details>
<summary><b>What do the names mean?</b></summary>

<b>Fid</b> is the language. <b>Fides</b>, the Latin word for trust, is its compiler. <code>fid</code> is the command you type.

</details>

## Community and licence

[Website](https://fid-lang.org) · [Discussions](https://github.com/pledgeandgrow/fid/discussions) · [Changelog](docs/reference/changelog.md) · [Code of conduct](CODE_OF_CONDUCT.md) · [Support](SUPPORT.md)

Fid is maintained by Mehdi Berel at [Pledge & Grow](https://github.com/pledgeandgrow) and released under the [MIT licence](LICENSE).

<p align="center">
  <sub>If Fid is useful to you, a star helps other people find it.</sub>
</p>
