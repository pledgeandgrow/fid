/*
 * docs/playground/embed.js — an embeddable Fides playground (roadmap L10D).
 *
 * Put runnable Fides code in any page:
 *
 *   <pre class="fides-playground" data-mode="run">
 *   fn main() {
 *       println("Hello from an embedded playground");
 *   }
 *   </pre>
 *   <script src="https://fides-lang.org/playground/embed.js"
 *           data-assets="https://fides-lang.org/playground/"></script>
 *
 * Every element with class `fides-playground` (or a `data-fides-run`
 * attribute) becomes an editor with Run, a mode menu (run, tokens, ast, ir,
 * effects), Reset, "Open in playground" and an output pane. The code in the
 * element is the preset (indentation common to its lines is removed);
 * `data-code-url` loads it from a file instead. Per element:
 *
 *   data-mode="run|tokens|ast|ir|effects"   the first mode (default run)
 *   data-modes="run,ir"                     the modes offered
 *   data-autorun                            run once when shown
 *   data-stdin="text"                       standard input of the program
 *   data-rows="12"                          height of the editor
 *
 * On the script tag:
 *
 *   data-assets   where fidesc.wasm and stdlib.json are (default: the
 *                 directory the script was loaded from). A page on another
 *                 origin needs those two files served with CORS
 *                 (Access-Control-Allow-Origin), or a copy of them.
 *   data-shim     the ES module of @bjorn3/browser_wasi_shim (default:
 *                 jsDelivr, version 0.4.2, the playground's)
 *   data-playground  the full playground, for "Open in playground"
 *                 (default https://fides-lang.org/playground)
 *
 * Without a script tag of its own, an iframe works the same: embed.html,
 * next to this file, is a page that is one playground filling the frame,
 * its code in the address after `#` (the full playground's share-link form:
 * base64 of encodeURIComponent(code)) or in `?code=`:
 *
 *   <iframe src="https://fides-lang.org/playground/embed.html#Zm4lMjBtYWlu..."
 *           width="100%" height="360" style="border:0"></iframe>
 *
 * The compiler is the playground's WASI build of fidesc, compiled once per
 * page and run in a module worker (one per page, shared by every embed); a
 * run over 15 s ends the worker, which the next run restarts.
 * scripts/playground-embed-check.mjs runs this worker under Node.
 */
(function (root) {
  'use strict';

  var MODES = ['run', 'tokens', 'ast', 'ir', 'effects'];
  var MODE_ARGS = {
    run: ['run', '/main.fd'],
    tokens: ['tokens', '/main.fd'],
    ast: ['ast', '/main.fd'],
    ir: ['ir', '/main.fd'],
    // the trailing 1 is fidesc's PIR detail level (as the playground)
    effects: ['pir', '/main.fd', '1'],
  };
  var TIMEOUT_MS = 15000;
  var DEFAULT_SHIM = 'https://cdn.jsdelivr.net/npm/@bjorn3/browser_wasi_shim@0.4.2/dist/index.js';
  var DEFAULT_PLAYGROUND = 'https://fides-lang.org/playground';

  // ── Pure helpers (tested by scripts/playground-embed-check.mjs) ─────

  /** The playground's share-link form: base64 of encodeURIComponent. */
  function encodeShare(code) {
    return btoa(encodeURIComponent(code));
  }

  function decodeShare(hash) {
    try {
      return decodeURIComponent(atob(hash.replace(/^#/, '')));
    } catch (e) {
      return null;
    }
  }

  /** Code from an element's text: no leading/trailing blank lines, and the
   * indentation every non-blank line shares removed. */
  function dedent(text) {
    var lines = String(text).replace(/\r\n?/g, '\n').split('\n');
    while (lines.length && lines[0].trim() === '') lines.shift();
    while (lines.length && lines[lines.length - 1].trim() === '') lines.pop();
    var cut = Infinity;
    lines.forEach(function (l) {
      if (l.trim() === '') return;
      var n = l.match(/^[ \t]*/)[0].length;
      if (n < cut) cut = n;
    });
    if (cut === Infinity) cut = 0;
    return lines.map(function (l) { return l.slice(cut); }).join('\n') + (lines.length ? '\n' : '');
  }

  function parseModes(attr) {
    if (!attr) return MODES.slice();
    var out = attr.split(',').map(function (m) { return m.trim(); })
      .filter(function (m) { return MODES.indexOf(m) >= 0; });
    return out.length ? out : MODES.slice();
  }

  // ── The worker (stringified into a Blob; under Node, called directly) ─
  //
  // init {type:'init', module|wasm, stdlib} -> {type:'ready'}
  // {mode, source, stdin} -> {type:'result', stdout, stderr, exitCode}
  // any failure -> {type:'error', message}
  function workerMain(self, shim, MODE_ARGS) {
    var enc = new TextEncoder();
    var assets = null;
    self.onmessage = async function (event) {
      try {
        var data = event.data;
        if (data && data.type === 'init') {
          if (!assets) {
            assets = (async function () {
              var module = data.module || (data.wasm ? await WebAssembly.compile(data.wasm) : null);
              if (!module) throw new Error('init carried neither a module nor wasm bytes');
              var stdlib = new Map(Object.entries(data.stdlib || {}).map(function (e) {
                return [e[0], new shim.File(enc.encode(e[1]))];
              }));
              return { module: module, stdlib: stdlib };
            })();
          }
          await assets;
          self.postMessage({ type: 'ready' });
          return;
        }
        if (!assets) throw new Error('worker received work before init');
        var a = await assets;
        var args = MODE_ARGS[data.mode];
        if (!args) throw new Error('unknown mode ' + data.mode);
        var root = new Map([
          ['main.fd', new shim.File(enc.encode(data.source))],
          ['stdlib', new shim.Directory(new Map(a.stdlib))],
        ]);
        var stdout = '', stderr = '';
        var od = new TextDecoder(), ed = new TextDecoder();
        var fds = [
          new shim.OpenFile(new shim.File(enc.encode(data.stdin || ''))),
          new shim.ConsoleStdout(function (b) { stdout += od.decode(b, { stream: true }); }),
          new shim.ConsoleStdout(function (b) { stderr += ed.decode(b, { stream: true }); }),
          new shim.PreopenDirectory('/', root),
        ];
        var wasi = new shim.WASI(['fidesc'].concat(args), [], fds);
        var instance = await WebAssembly.instantiate(a.module, { wasi_snapshot_preview1: wasi.wasiImport });
        var exitCode = wasi.start(instance);
        stdout += od.decode();
        stderr += ed.decode();
        self.postMessage({ type: 'result', stdout: stdout, stderr: stderr, exitCode: exitCode });
      } catch (error) {
        self.postMessage({ type: 'error', message: error && error.message ? error.message : String(error) });
      }
    };
  }

  var api = {
    MODES: MODES,
    MODE_ARGS: MODE_ARGS,
    encodeShare: encodeShare,
    decodeShare: decodeShare,
    dedent: dedent,
    parseModes: parseModes,
    workerMain: workerMain,
  };

  if (typeof module === 'object' && module.exports) {
    module.exports = api;
  }
  if (typeof document === 'undefined') return;

  // ── Browser side ───────────────────────────────────────────────────

  var script = document.currentScript;
  var scriptBase = script && script.src ? script.src.replace(/[^/]*$/, '') : './';
  var config = {
    assets: (script && script.getAttribute('data-assets')) || scriptBase,
    shim: (script && script.getAttribute('data-shim')) || DEFAULT_SHIM,
    playground: (script && script.getAttribute('data-playground')) || DEFAULT_PLAYGROUND,
  };
  if (!/\/$/.test(config.assets)) config.assets += '/';

  var assetsPromise = null;
  function loadAssets() {
    if (!assetsPromise) {
      assetsPromise = Promise.all([
        fetch(config.assets + 'fidesc.wasm').then(function (r) {
          if (!r.ok) throw new Error('fidesc.wasm: HTTP ' + r.status);
          return WebAssembly.compileStreaming
            ? WebAssembly.compileStreaming(r).catch(function () {
                return fetch(config.assets + 'fidesc.wasm').then(function (r2) { return r2.arrayBuffer(); })
                  .then(function (b) { return WebAssembly.compile(b); });
              })
            : r.arrayBuffer().then(function (b) { return WebAssembly.compile(b); });
        }),
        fetch(config.assets + 'stdlib.json').then(function (r) {
          if (!r.ok) throw new Error('stdlib.json: HTTP ' + r.status);
          return r.json();
        }),
      ]).then(function (v) { return { module: v[0], stdlib: v[1] }; });
      assetsPromise.catch(function () { assetsPromise = null; });
    }
    return assetsPromise;
  }

  // One worker for the page, restarted after a timeout or a crash.
  var worker = null, workerReady = null, queue = Promise.resolve();
  function startWorker() {
    var src = 'import * as shim from ' + JSON.stringify(config.shim) + ';\n(' +
      workerMain.toString() + ')(self, shim, ' + JSON.stringify(MODE_ARGS) + ');\n';
    var url = URL.createObjectURL(new Blob([src], { type: 'text/javascript' }));
    var w = new Worker(url, { type: 'module' });
    worker = w;
    workerReady = loadAssets().then(function (a) {
      return new Promise(function (resolve, reject) {
        w.onmessage = function (e) {
          if (e.data.type === 'ready') resolve(w);
          else if (e.data.type === 'error') reject(new Error(e.data.message));
        };
        w.onerror = function (e) { reject(new Error(e.message || 'the compiler worker failed to start')); };
        w.postMessage({ type: 'init', module: a.module, stdlib: a.stdlib });
      });
    });
    workerReady.catch(function () { stopWorker(); });
    return workerReady;
  }
  function stopWorker() {
    if (worker) worker.terminate();
    worker = null;
    workerReady = null;
  }

  /** Run one job; jobs from every embed on the page go one at a time. */
  function compile(mode, source, stdin) {
    var job = queue.then(function () {
      return (workerReady || startWorker()).then(function (w) {
        return new Promise(function (resolve, reject) {
          var timer = setTimeout(function () {
            stopWorker();
            reject(new Error('Execution timed out (' + TIMEOUT_MS / 1000 + ' s). Compiler restarted.'));
          }, TIMEOUT_MS);
          w.onmessage = function (e) {
            if (e.data.type === 'result') { clearTimeout(timer); resolve(e.data); }
            else if (e.data.type === 'error') { clearTimeout(timer); stopWorker(); reject(new Error(e.data.message)); }
          };
          w.onerror = function (e) {
            clearTimeout(timer); stopWorker();
            reject(new Error(e.message || 'the compiler worker failed'));
          };
          w.postMessage({ mode: mode, source: source, stdin: stdin || '' });
        });
      });
    });
    queue = job.catch(function () {});
    return job;
  }

  var STYLE = [
    '.fpe{font:14px/1.45 system-ui,sans-serif;border:1px solid var(--fpe-border,#d0d4dc);border-radius:8px;overflow:hidden;margin:1em 0;background:var(--fpe-bg,#fff);color:var(--fpe-fg,#1d2330)}',
    '.fpe-bar{display:flex;flex-wrap:wrap;gap:6px;align-items:center;padding:6px 8px;background:var(--fpe-bar,#f2f4f8);border-bottom:1px solid var(--fpe-border,#d0d4dc)}',
    '.fpe-bar button,.fpe-bar select{font:inherit;padding:3px 10px;border:1px solid var(--fpe-border,#c3c8d2);border-radius:5px;background:var(--fpe-bg,#fff);color:inherit;cursor:pointer}',
    '.fpe-bar .fpe-run{background:#2f6fde;border-color:#2f6fde;color:#fff}',
    '.fpe-bar .fpe-run:disabled{opacity:.6;cursor:wait}',
    '.fpe-bar a{margin-left:auto;color:#2f6fde;text-decoration:none;font-size:13px}',
    '.fpe-code{display:block;width:100%;box-sizing:border-box;border:0;padding:10px 12px;font:13px/1.5 ui-monospace,SFMono-Regular,Consolas,monospace;resize:vertical;tab-size:4;background:transparent;color:inherit;outline:none}',
    '.fpe-out{margin:0;padding:8px 12px;max-height:22em;overflow:auto;font:12.5px/1.45 ui-monospace,SFMono-Regular,Consolas,monospace;white-space:pre-wrap;word-break:break-word;border-top:1px solid var(--fpe-border,#d0d4dc);background:var(--fpe-out,#f8f9fb)}',
    '.fpe-out:empty{display:none}',
    '.fpe-err{color:#b3261e}',
    '.fpe-meta{color:#6b7280;font-size:12px}',
    '@media (prefers-color-scheme:dark){.fpe{--fpe-bg:#171a21;--fpe-fg:#e6e8ee;--fpe-bar:#20242d;--fpe-border:#343a46;--fpe-out:#12151b}.fpe-err{color:#ff8a80}}',
  ].join('\n');

  function el(tag, cls, text) {
    var e = document.createElement(tag);
    if (cls) e.className = cls;
    if (text != null) e.textContent = text;
    return e;
  }

  function mount(host, preset) {
    if (host.__fidesEmbed) return host.__fidesEmbed;
    var modes = parseModes(host.getAttribute('data-modes'));
    var first = host.getAttribute('data-mode');
    if (modes.indexOf(first) < 0) first = modes[0];
    var stdin = host.getAttribute('data-stdin') || '';
    var rows = parseInt(host.getAttribute('data-rows') || '0', 10);

    var box = el('div', 'fpe');
    var bar = el('div', 'fpe-bar');
    var runBtn = el('button', 'fpe-run', 'Run');
    runBtn.type = 'button';
    var modeSel = el('select');
    modeSel.setAttribute('aria-label', 'What to show');
    modes.forEach(function (m) {
      var o = el('option', null, m === 'run' ? 'Run' : m === 'ir' ? 'SSA IR' : m === 'ast' ? 'AST' : m.charAt(0).toUpperCase() + m.slice(1));
      o.value = m;
      modeSel.appendChild(o);
    });
    modeSel.value = first;
    if (modes.length < 2) modeSel.style.display = 'none';
    var resetBtn = el('button', null, 'Reset');
    resetBtn.type = 'button';
    var meta = el('span', 'fpe-meta');
    var open = el('a', null, 'Open in playground ↗');
    open.target = '_blank';
    open.rel = 'noopener';
    var code = el('textarea', 'fpe-code');
    code.spellcheck = false;
    code.setAttribute('aria-label', 'Fides code');
    code.value = preset;
    code.rows = rows > 0 ? rows : Math.min(24, Math.max(4, preset.split('\n').length));
    var out = el('pre', 'fpe-out');
    out.setAttribute('aria-live', 'polite');

    function syncLink() { open.href = config.playground + '#' + encodeShare(code.value); }
    syncLink();
    code.addEventListener('input', syncLink);
    code.addEventListener('keydown', function (e) {
      if (e.key === 'Tab' && !e.shiftKey && !e.ctrlKey && !e.metaKey) {
        e.preventDefault();
        var s = code.selectionStart, t = code.selectionEnd;
        code.value = code.value.slice(0, s) + '    ' + code.value.slice(t);
        code.selectionStart = code.selectionEnd = s + 4;
        syncLink();
      } else if (e.key === 'Enter' && (e.ctrlKey || e.metaKey)) {
        e.preventDefault();
        run();
      }
    });

    function show(text, isErr) {
      out.textContent = '';
      var span = el('span', isErr ? 'fpe-err' : null, text);
      out.appendChild(span);
    }

    function run() {
      runBtn.disabled = true;
      meta.textContent = assetsPromise ? 'running…' : 'loading the compiler (8 MB)…';
      var t0 = performance.now();
      return compile(modeSel.value, code.value, stdin).then(function (r) {
        out.textContent = '';
        if (r.stdout) out.appendChild(el('span', null, r.stdout));
        // fidesc colours its diagnostics; the pane shows plain text
        if (r.stderr) out.appendChild(el('span', 'fpe-err', r.stderr.replace(/\x1b\[[0-9;]*m/g, '')));
        if (!r.stdout && !r.stderr) out.appendChild(el('span', 'fpe-meta', '(no output)'));
        meta.textContent = 'exit ' + r.exitCode + ' · ' + Math.round(performance.now() - t0) + ' ms';
        host.dispatchEvent(new CustomEvent('fides-result', { detail: r, bubbles: true }));
        return r;
      }, function (err) {
        show(err.message, true);
        meta.textContent = '';
        host.dispatchEvent(new CustomEvent('fides-result', { detail: { error: err.message }, bubbles: true }));
      }).then(function (r) { runBtn.disabled = false; return r; });
    }

    runBtn.addEventListener('click', run);
    resetBtn.addEventListener('click', function () { code.value = preset; syncLink(); out.textContent = ''; meta.textContent = ''; });
    bar.appendChild(runBtn);
    bar.appendChild(modeSel);
    bar.appendChild(resetBtn);
    bar.appendChild(meta);
    bar.appendChild(open);
    box.appendChild(bar);
    box.appendChild(code);
    box.appendChild(out);
    host.textContent = '';
    host.appendChild(box);
    host.classList.add('fides-playground-ready');
    var handle = { run: run, element: box, getCode: function () { return code.value; }, setCode: function (s) { code.value = s; syncLink(); } };
    host.__fidesEmbed = handle;
    if (host.hasAttribute('data-autorun')) run();
    return handle;
  }

  /** Turn every not-yet-mounted embed under `scope` into a playground. */
  function mountAll(scope) {
    scope = scope || document;
    var hosts = scope.querySelectorAll('.fides-playground:not(.fides-playground-ready),[data-fides-run]:not(.fides-playground-ready)');
    return Promise.all(Array.prototype.map.call(hosts, function (host) {
      var url = host.getAttribute('data-code-url');
      var src = url
        ? fetch(url).then(function (r) { return r.text(); })
        : Promise.resolve(host.textContent);
      return src.then(function (text) { return mount(host, dedent(text)); });
    }));
  }

  if (!document.getElementById('fpe-style')) {
    var st = el('style');
    st.id = 'fpe-style';
    st.textContent = STYLE;
    document.head.appendChild(st);
  }
  api.mount = function (host, code) { return mount(host, dedent(code == null ? host.textContent : code)); };
  api.mountAll = mountAll;
  api.config = config;
  root.FidesEmbed = api;
  if (document.readyState === 'loading') document.addEventListener('DOMContentLoaded', function () { mountAll(); });
  else mountAll();
})(typeof globalThis !== 'undefined' ? globalThis : this);
