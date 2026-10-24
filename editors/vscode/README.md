# Fid Language — VS Code extension

Syntax highlighting and language configuration for Fid (`.fd`) files:
keywords, f-strings, effect annotations (`!IO`), safety annotations
(`@conserve`), doc comments, plus comment toggling, bracket matching, and
auto-closing pairs. The grammar (`syntaxes/fid.tmLanguage.json`) is the same
TextMate grammar that highlights the code blocks on
[fides-lang.org](https://fides-lang.org).

The extension is not on the Marketplace yet. Build and install it from this
directory in two commands:

```bash
npx @vscode/vsce package
```

```bash
code --install-extension fid-lang-0.1.0.vsix
```

## Diagnostics

Highlighting does not need the compiler. Diagnostics do, and they come from
the built-in language server rather than from this extension: point any LSP
client extension at `fidesc lsp` (JSON-RPC 2.0 over stdio). With a generic
client, for example:

```json
{
  "fides.languageServerPath": "/path/to/fidesc",
  "fides.languageServerArgs": ["lsp"]
}
```

See <https://fides-lang.org/docs/editor-setup> for VS Code, Neovim, and
other editors.

## Keeping the grammar in sync

The website repository (`fides-lang`, `src/grammars/fid.tmLanguage.json`) and
this directory carry the same file. When the language gains a keyword, update
both.
