# Fides Quick Start

## Installation

### Windows (recommended)

1. Download `Fides-Setup-0.8.0.exe` from the
   [releases page](https://github.com/pledgeandgrow/fid/releases).
2. Run the installer.
3. Open a new Command Prompt or PowerShell window.
4. Verify installation:
   ```
   fides --version
   ```

The installer adds `fides` to your PATH, associates `.fd` files, and creates
Start Menu shortcuts. You can compile `.fd` files immediately.

The v1.0.0 installer (Sep 9, 2026) names the compiler `fides.exe`. An
installer built from the current tree (`installer/fides.nsi`) ships the
self-hosted compiler as `fid.exe`, with `fidesc.exe`, `ark.exe`, `scribe.exe`
and `veil.exe`; the commands below work the same with `fid` in place of
`fides`. The work since v1.0.0 is not yet in a release.

### Build from Source

```bash
git clone https://github.com/pledgeandgrow/fid.git
cd fid
cargo build --release
```

This produces `target/release/fidesc`, the frozen Rust compiler that builds
stage 1 of the self-hosted compiler. Then build `fid`, the release compiler:

```bash
RELEASE=1 CC="zig cc" ./scripts/bootstrap.sh    # 3 stages, fixpoint checked
./release/fid ccompile hello.fd -o hello
./hello
```

`fid` defines the language ([reference.md](reference.md)); `fidesc` checks
less. See [selfhost.md](selfhost.md) § Build Pipeline.

## Your First Program

Create `hello.fd`:

```fides
fn main() -> void {
    print("Hello, Fides!");
}
```

Compile and run:

```bash
fides ccompile hello.fd -o hello.exe
./hello.exe
```

Output:
```
Hello, Fides!
```

## Compile to C

```bash
fides ccompile hello.fd -o hello.exe
```

This generates C code, compiles it with a C compiler (zig cc, clang, gcc or
cl), and produces a native executable.

## Native Windows Executable

```bash
fid emit hello.fd -o hello.exe
```

This writes the executable directly, with no C compiler.

## Run in Interpreter

```bash
fides run hello.fd
```

## Use the Standard Library

```fides
fn main() -> void {
    var v: Vec<i64> = Vec::new();
    v.push(1);
    v.push(2);
    v.push(3);

    print("Vec size: " + int_to_str(v.len()));
    print("Sum: " + int_to_str(v.get(0) + v.get(1) + v.get(2)));
}
```

## Windows Installer Details

The installer (`Fides-Setup-0.8.0.exe`, 6.6 MB) includes:

| Component | Description | Optional |
|-----------|-------------|----------|
| Fides Compiler | `fides.exe` — the compiler binary (`fid.exe` in installers built from the current tree) | No (required) |
| Standard Library | 99 modules in v1.0.0 (137 in the tree on Nov 6) | Yes |
| C Runtime | Runtime support files | Yes |
| Documentation | Language reference, guides, roadmap | Yes |
| Examples | Sample `.fd` programs | Yes |
| File Association | Open `.fd` files with Fides | Yes |
| Start Menu | Shortcuts for compiler and uninstaller | Yes |

### Uninstall

Use **Add/Remove Programs** in Windows Settings, or run:
```
"C:\Program Files\Fides\uninstall.exe"
```

## Next Steps

- Read the [language reference](reference.md)
- Browse the [standard library](../../stdlib/)
- Check the [roadmap](../roadmap/)
- See the [self-hosting docs](selfhost.md) for compiler internals
