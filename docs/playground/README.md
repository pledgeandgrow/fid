# Embeddable playground (roadmap L10D)

Runnable Fides code in any page, like the Rust Playground's embeds. The
compiler is the playground's WASI build of `fidesc` (`fidesc.wasm`, with the
standard library as `stdlib.json`), compiled once per page and run in a
module worker; nothing is sent to a server.

| File | What it is |
|---|---|
| `embed.js` | The script: turns every `.fides-playground` element into an editor with Run, a mode menu, Reset, "Open in playground" and output |
| `embed.html` | One playground filling the page, for an `<iframe>`; its code comes from the address |
| `example.html` | A documentation page with three presets and an iframe |
| `../../scripts/playground-embed-check.mjs` | Runs `embed.js`'s worker under Node on `example.html`'s presets |

## Script

```html
<pre class="fides-playground" data-mode="run">
fn main() {
    println("Hello from an embedded playground");
}
</pre>
<script src="https://fides-lang.org/playground/embed.js"
        data-assets="https://fides-lang.org/playground/"></script>
```

The element's text is the preset (indentation its lines share is removed).
Attributes of the element: `data-mode` (`run`, `tokens`, `ast`, `ir`,
`effects`), `data-modes="run,ir"` (the modes offered), `data-autorun`,
`data-stdin`, `data-rows`, `data-code-url` (load the preset from a file).
Attributes of the script: `data-assets` (where `fidesc.wasm` and
`stdlib.json` are; default the script's own directory), `data-shim` (the
`@bjorn3/browser_wasi_shim` module; default jsDelivr, 0.4.2),
`data-playground` (the target of "Open in playground"). Ctrl+Enter runs.
A run over 15 s ends the worker; the next run starts a new one. Each run
sends a `fides-result` event (`detail`: `stdout`, `stderr`, `exitCode`, or
`error`) from the element; `FidesEmbed.mount(element, code)` and
`FidesEmbed.mountAll()` mount elements added later.

## Iframe

```html
<iframe src="https://fides-lang.org/playground/embed.html#<share hash>"
        width="100%" height="360" style="border:0" title="Fides playground"></iframe>
```

The hash is the full playground's share-link form (`btoa(encodeURIComponent(code))`),
so a playground "Share" link becomes an embed by putting `embed.html` in
place of `playground`. `?code=<URL-encoded code>` works too, and
`?mode=`, `?modes=`, `?autorun`, `?stdin=`, `?theme=light|dark`.

## Hosting

`embed.js` and `embed.html` must be served next to `fidesc.wasm` and
`stdlib.json` (the website's `public/playground/`). A page on another
origin that uses the script form fetches those two files across origins, so
they need `Access-Control-Allow-Origin: *`; the iframe form needs nothing.

## Checked

- `node scripts/playground-embed-check.mjs` (Nov 16): the helpers (share
  links in the playground's form, dedent, modes), init, the three presets
  of `example.html` (hello prints, the loop prints 5050, the type error is
  rejected with E044), the four inspection modes, an unknown mode. It needs
  the website checkout beside this repository (`../fides-lang`) or
  `--assets`/`--shim`.
- In a browser (Nov 16), `example.html` served with the assets from
  `localhost`: the autorun preset printed its line, IR mode printed the
  lowered module, the type error showed its diagnostic, the iframe ran its
  `?code=` program, "Open in playground" linked the share hash.

Left: copying `embed.js`/`embed.html` into the website's `public/playground/`
(its `scripts/sync-playground-assets.mjs`) and the CORS header there, so the
`fides-lang.org` URLs above serve them.
