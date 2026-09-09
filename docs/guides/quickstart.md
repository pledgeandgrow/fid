# Fides Quick Start

## Installation

### Windows (recommended)

1. Download `Fides-Setup-1.0.0.exe` from the
   [releases page](https://github.com/pledgeandgrow/p-lang/releases).
2. Run the installer.
3. Open a new Command Prompt or PowerShell window.
4. Verify installation:
   ```
   fides --version
   ```

The installer adds `fides` to your PATH, associates `.fd` files, and creates
Start Menu shortcuts. You can compile `.fd` files immediately.

### Build from Source

```bash
git clone https://github.com/pledgeandgrow/p-lang.git
cd p-lang
cargo build --release
```

This produces `target/release/fidesc` (the Rust bootstrap compiler). Use it
to compile Fides programs:

```bash
./target/release/fidesc ccompile hello.fd -o hello
./hello
```

To build the self-hosted compiler (c3), see
[build-pipeline.md](../selfhost/build-pipeline.md).

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

This generates C code, compiles it with gcc, and produces a native executable.

## Run in Interpreter

```bash
fides run hello.fd
```

## Use the Standard Library

```fides
use vec;
use string;
use math;

fn main() -> void {
    var v = Vec::new();
    v.push(1);
    v.push(2);
    v.push(3);

    print("Vec size: " + int_to_str(v.len()));
    print("Sum: " + int_to_str(v.get(0) + v.get(1) + v.get(2)));
}
```

## Windows Installer Details

The installer (`Fides-Setup-1.0.0.exe`, 2.5 MB) includes:

| Component | Description | Optional |
|-----------|-------------|----------|
| Fides Compiler | `fides.exe` — the compiler binary | No (required) |
| Standard Library | 99 modules (core, vec, string, math, etc.) | Yes |
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

- Read the [language reference](../reference/)
- Browse the [standard library](../../stdlib/)
- Check the [roadmap](../roadmap/)
- See the [self-hosting docs](../selfhost/) for compiler internals
