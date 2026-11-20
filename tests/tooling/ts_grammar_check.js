// ts_grammar_check.js -- checks of editors/tree-sitter-fides that need no
// tree-sitter CLI (TL10; tests/tooling/zed-check.sh runs it):
//   - grammar.js loads with a stand-in of tree-sitter's DSL, every $.rule it
//     names is defined, `word` and the conflicts name defined rules;
//   - every node, field and anonymous token the query files name
//     (tree-sitter-fides/queries/*.scm, zed/languages/fides/*.scm) exists in
//     the grammar: Zed and Neovim refuse a query with an unknown name;
//   - every node the test corpus names exists;
//   - every keyword of the lexer (compiler/lexer.fd keyword_or_ident_*) that
//     the grammar uses is highlighted as a keyword, a type or a boolean.
// Usage: node tests/tooling/ts_grammar_check.js <repo root>
'use strict';
const fs = require('fs');
const path = require('path');

const root = process.argv[2] || '.';
const gdir = path.join(root, 'editors', 'tree-sitter-fides');
let fails = 0;
function check(label, cond, extra) {
  console.log((cond ? 'ok   ' : 'FAIL ') + label + (cond || !extra ? '' : ' (' + extra + ')'));
  if (!cond) fails++;
}

// ---- a stand-in for the DSL: rules become plain trees -------------------
const refs = new Set();
function norm(x) {
  if (typeof x === 'string') return { t: 'str', v: x };
  if (x instanceof RegExp) return { t: 're', v: x.source };
  if (x && x.t) return x;
  throw new Error('bad rule element: ' + String(x));
}
const dsl = {
  seq: (...a) => ({ t: 'seq', m: a.map(norm) }),
  choice: (...a) => ({ t: 'choice', m: a.map(norm) }),
  repeat: (a) => ({ t: 'repeat', m: [norm(a)] }),
  repeat1: (a) => ({ t: 'repeat1', m: [norm(a)] }),
  optional: (a) => ({ t: 'optional', m: [norm(a)] }),
  field: (n, a) => ({ t: 'field', n, m: [norm(a)] }),
  alias: (a, n) => ({ t: 'alias', n: typeof n === 'string' ? n : n.v, named: typeof n !== 'string', m: [norm(a)] }),
  blank: () => ({ t: 'blank', m: [] }),
};
function precf(kind) {
  return (p, a) => (a === undefined ? { t: kind, m: [norm(p)] } : { t: kind, p, m: [norm(a)] });
}
const prec = precf('prec');
prec.left = precf('prec');
prec.right = precf('prec');
prec.dynamic = precf('prec');
const token = (a) => ({ t: 'token', m: [norm(a)] });
token.immediate = (a) => ({ t: 'token', imm: true, m: [norm(a)] });
let spec = null;
global.grammar = (s) => { spec = s; return s; };
Object.assign(global, dsl, { prec, token });

const sym = new Proxy({}, { get: (_, name) => { refs.add(String(name)); return { t: 'sym', v: String(name) }; } });

try {
  require(path.resolve(gdir, 'grammar.js'));
} catch (e) {
  check('grammar.js loads', false, e.message);
  process.exit(1);
}
check('grammar.js loads', spec !== null && spec.name === 'fides', spec && spec.name);

const rules = {};
for (const [name, fn] of Object.entries(spec.rules)) {
  rules[name] = norm(fn(sym));
}
const ruleNames = new Set(Object.keys(rules));
const extras = (spec.extras ? spec.extras(sym) : []).map(norm);
const word = spec.word ? spec.word(sym) : null;
const conflicts = spec.conflicts ? spec.conflicts(sym) : [];
// alias($.x, $.name) may name a node that is no rule
const aliasNames = new Set();
(function collect(rs) {
  for (const r of rs) {
    if (r.t === 'alias' && r.named) aliasNames.add(r.n);
    collect(r.m || []);
  }
})(Object.values(rules));
const undef = [...refs].filter((r) => !ruleNames.has(r) && !aliasNames.has(r));
check('every $.rule named is defined', undef.length === 0, undef.join(', '));
check('word is a rule', word && ruleNames.has(word.v));
check('conflicts name rules', conflicts.every((c) => c.every((s) => ruleNames.has(s.v))));
check('the first rule is source_file', Object.keys(spec.rules)[0] === 'source_file');

// node types, fields and anonymous tokens
const named = new Set();
const fields = new Set();
const anon = new Set();
for (const n of ruleNames) if (!n.startsWith('_')) named.add(n);
function walk(r, inToken) {
  if (r.t === 'str') {
    if (!inToken) anon.add(r.v);
    return;
  }
  if (r.t === 'field') fields.add(r.n);
  if (r.t === 'alias') {
    if (r.named) named.add(r.n);
    else anon.add(r.n);
  }
  for (const m of r.m || []) walk(m, inToken || r.t === 'token' && !r.imm ? true : inToken || false);
}
for (const r of Object.values(rules)) walk(r, false);
for (const r of extras) walk(r, false);
// token.immediate('x') is the anonymous node "x" too
function walkImm(r) {
  if (r.t === 'token' && r.imm && r.m[0].t === 'str') anon.add(r.m[0].v);
  for (const m of r.m || []) walkImm(m);
}
for (const r of Object.values(rules)) walkImm(r);

// ---- queries ---------------------------------------------------------------
function queryNames(text) {
  const t = text.replace(/;[^\n]*/g, '');
  const strs = [];
  const noStr = t.replace(/"((?:[^"\\]|\\.)*)"/g, (_, s) => { strs.push(s.replace(/\\(.)/g, '$1')); return ' '; });
  const nodes = [...noStr.matchAll(/\(\s*([A-Za-z_][A-Za-z0-9_.]*)/g)].map((m) => m[1]).filter((n) => n !== '_');
  const flds = [...noStr.matchAll(/([A-Za-z_][A-Za-z0-9_]*)\s*:/g)].map((m) => m[1]);
  return { nodes, flds, strs, predicates: [...noStr.matchAll(/#([a-z-]+[?!])/g)].map((m) => m[1]) };
}
const queryFiles = [];
for (const d of [path.join(gdir, 'queries'), path.join(root, 'editors', 'zed', 'languages', 'fides')]) {
  if (fs.existsSync(d)) for (const f of fs.readdirSync(d)) if (f.endsWith('.scm')) queryFiles.push(path.join(d, f));
}
check('query files found', queryFiles.length >= 5, queryFiles.length);
for (const f of queryFiles) {
  const q = queryNames(fs.readFileSync(f, 'utf8'));
  const badNodes = q.nodes.filter((n) => !named.has(n));
  const badFields = q.flds.filter((n) => !fields.has(n));
  const strsOutsidePred = fs.readFileSync(f, 'utf8').replace(/;[^\n]*/g, '').replace(/\(#[a-z-]+[?!][^)]*\)/g, ' ');
  const s2 = [];
  strsOutsidePred.replace(/"((?:[^"\\]|\\.)*)"/g, (_, s) => { s2.push(s.replace(/\\(.)/g, '$1')); return ''; });
  const badStrs = s2.filter((s) => !anon.has(s));
  const badPred = q.predicates.filter((p) => !['match?', 'eq?', 'not-match?', 'not-eq?', 'any-of?', 'set!'].includes(p));
  const rel = path.relative(root, f).replace(/\\/g, '/');
  check(rel + ': node names exist', badNodes.length === 0, badNodes.join(', '));
  check(rel + ': field names exist', badFields.length === 0, badFields.join(', '));
  check(rel + ': anonymous tokens exist', badStrs.length === 0, badStrs.join(' '));
  check(rel + ': predicates known', badPred.length === 0, badPred.join(' '));
}
const h1 = path.join(gdir, 'queries', 'highlights.scm');
const h2 = path.join(root, 'editors', 'zed', 'languages', 'fides', 'highlights.scm');
check('zed highlights.scm is the grammar\'s', fs.existsSync(h1) && fs.existsSync(h2) &&
  fs.readFileSync(h1, 'utf8').replace(/\r/g, '') === fs.readFileSync(h2, 'utf8').replace(/\r/g, ''));

// ---- corpus ------------------------------------------------------------------
const cdir = path.join(gdir, 'test', 'corpus');
let ncases = 0;
for (const f of fs.existsSync(cdir) ? fs.readdirSync(cdir) : []) {
  const text = fs.readFileSync(path.join(cdir, f), 'utf8').replace(/\r/g, '');
  const parts = text.split(/^={3,}\n(.*)\n={3,}\n/m);
  for (let i = 1; i + 1 < parts.length; i += 2) {
    ncases++;
    const [, expected] = parts[i + 1].split(/^-{3,}\n/m);
    const used = [...(expected || '').matchAll(/\(([A-Za-z_][A-Za-z0-9_]*)/g)].map((m) => m[1]);
    const bad = used.filter((n) => !named.has(n) && n !== 'ERROR' && n !== 'MISSING');
    check('corpus "' + parts[i].trim() + '": node names exist', expected !== undefined && bad.length === 0, bad.join(', '));
    let depth = 0;
    for (const c of expected || '') { if (c === '(') depth++; if (c === ')') depth--; if (depth < 0) break; }
    check('corpus "' + parts[i].trim() + '": parentheses balance', depth === 0);
  }
}
check('corpus has cases', ncases >= 3, ncases);

// ---- keywords --------------------------------------------------------------
const lexer = fs.readFileSync(path.join(root, 'compiler', 'lexer.fd'), 'utf8');
const kws = new Set([...lexer.matchAll(/s == "([a-z_0-9]+)"/g)].map((m) => m[1]));
const hl = fs.readFileSync(h1, 'utf8');
const highlighted = new Set();
hl.replace(/"([a-z_0-9]+)"/g, (_, s) => { highlighted.add(s); return ''; });
// a named-node capture ((visibility) @keyword) highlights the keyword too
hl.replace(/\(([a-z_0-9]+)\)/g, (_, s) => { highlighted.add(s); return ''; });
const prims = new Set(['i8', 'i16', 'i32', 'i64', 'i128', 'u8', 'u16', 'u32', 'u64', 'u128', 'f32', 'f64', 'bool', 'str', 'void', 'q8', 'q4', 'true', 'false']);
// the grammar wraps some keywords in a named rule ('pub' -> visibility):
// the query matches the node, not the anonymous token
const kwNode = { pub: 'visibility' };
const missing = [...kws].filter((k) => anon.has(k) && !highlighted.has(k) && !highlighted.has(kwNode[k] || k) && !prims.has(k));
check('every lexer keyword the grammar uses is highlighted', missing.length === 0, missing.join(' '));
check('the grammar knows the main keywords', ['fn', 'val', 'var', 'struct', 'enum', 'impl', 'cap', 'match', 'if', 'while', 'for', 'return', 'use', 'module'].every((k) => anon.has(k)));

console.log('ts_grammar_check: ' + named.size + ' node types, ' + fields.size + ' fields, ' + anon.size + ' tokens');
process.exit(fails ? 1 : 0);
