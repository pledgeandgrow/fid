# Fides Example Projects

Small complete programs: compiler pieces, a JSON parser, a neural network,
a server, a game and bare-metal blinkers.

## Examples

Results on Oct 27, 2026. "Same" means the output equals the interpreter's
(`fidesc run`); `fid` is the self-hosted compiler.

| Example | Description | `fidesc` C | `fidesc` PE | `fid run` | `fid ccompile` |
|---------|-------------|-----------|-------------|-------------|------------------|
| `lexer.fd` | Tokenizer for arithmetic expressions | same | same | same | same |
| `parser.fd` | Recursive-descent parser with Box<T> AST | same | same | same | same |
| `codegen.fd` | x86-64 assembly text emitter | same | same | same | same |
| `mini_compiler.fd` | Full lex→parse→codegen pipeline | same | same | same | same |
| `json_parser.fd` | JSON parser with arrays/objects | same | same | same | same |
| `neural_network.fd` | Neural network forward pass | same | same | same | same |
| `http_server.fd` | HTTP server (runs until stopped) | same | no sockets in the PE runtime | same | same |
| `riscv_blink.fd` | Bare-metal LED blink, SiFive HiFive1 (loops forever) | same | same | same | same |
| `rpi_led_blink.fd` | Bare-metal LED blink, Raspberry Pi 3 (loops forever) | same | same | same | same |
| `minimal_game.fd` | SDL2 game vertical slice | needs SDL2 | needs SDL2 | same | needs SDL2 |

The programs that run forever are compared on what they print before a
timeout. Without SDL2 installed, `minimal_game` fails the same way on every
compiler.

## Running

```bash
# Interpreter
fid run examples/json_parser.fd          # self-hosted
fidesc run examples/json_parser.fd         # Rust compiler

# C backend (needs a C compiler: $CC, gcc, clang, zig cc or tcc)
fid ccompile examples/mini_compiler.fd -o mini_compiler.exe
./mini_compiler.exe

# Native Windows executable, no C compiler needed
fidesc emit examples/lexer.fd -o lexer.exe
./lexer.exe
```
