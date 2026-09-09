# Fides Quick Start

## Installation

### Windows

1. Download `Fides-Setup-1.0.0.exe` from the [releases page](https://github.com/medzer/pledge/p-lang/releases).
2. Run the installer.
3. Open a new Command Prompt or PowerShell window.
4. Verify installation:
   ```
   fides --version
   ```

### Build from Source

```bash
git clone https://github.com/medzer/pledge/p-lang.git
cd p-lang
cargo build --release
./target/release/fidesc build-selfhost
```

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

## Next Steps

- Read the [language reference](../reference/)
- Browse the [standard library](../stdlib/)
- Check the [roadmap](../roadmap/)
