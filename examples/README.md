# Fides Example Projects

These examples exercise the patterns a self-hosted Fides compiler would need.

## Examples

| Example | Description | Interp | C Codegen | PE Emitter |
|---------|-------------|--------|-----------|------------|
| `lexer.fd` | Tokenizer for arithmetic expressions | PASS | PASS | PARTIAL |
| `parser.fd` | Recursive-descent parser with Box<T> AST | PASS | PASS | FAIL |
| `codegen.fd` | x86-64 assembly text emitter | PASS | PASS | PARTIAL |
| `mini_compiler.fd` | Full lex→parse→codegen pipeline | PASS | PASS | FAIL |
| `json_parser.fd` | JSON parser with arrays/objects | PASS | PASS | FAIL |

## PE Emitter Issues

The PE emitter has 3 critical bugs that prevent self-hosting on that backend
(see `docs/roadmap/roadmap-issues.md` Issues 67-69):

1. **Issue 67**: `val` inside a loop body corrupts the loop variable
   (stack slot overlap). Workaround: use `var` inside loops.
2. **Issue 68**: `str_split` returns an empty Vec in the PE emitter.
3. **Issue 69**: Recursive `Box<T>` causes segfault in complex programs.

The interpreter and C codegen backends work correctly for all examples.

## Running

```bash
# Interpreter (all work)
fidesc run examples/lexer.fd
fidesc run examples/parser.fd
fidesc run examples/codegen.fd
fidesc run examples/mini_compiler.fd
fidesc run examples/json_parser.fd

# C codegen (all work)
fidesc ccompile examples/mini_compiler.fd -o examples/mini_compiler
./examples/mini_compiler

# PE emitter (lexer + codegen partially work; others segfault)
fidesc build examples/lexer.fd -o examples/lexer
./examples/lexer
```

## Self-Host Readiness

The examples demonstrate that Fides can express all the core patterns
needed for a self-hosted compiler:

- **Tokenization**: Character scanning, string building, Vec<T> ✅
- **Parsing**: Recursive descent, precedence climbing, Box<T> AST ✅
- **Code generation**: String concatenation, recursive traversal ✅
- **Full pipeline**: Lex → parse → eval → codegen in one program ✅
- **Real-world parsing**: JSON with arrays, objects, escape sequences ✅

**Conclusion**: Self-hosting is ready to start on the **C codegen backend**.
The PE emitter needs Issues 67-69 fixed first.
