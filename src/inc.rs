//! Incremental compilation cache for S-1B
//!
//! Caches the optimized L2 PIR module in memory, keyed by source file path
//! and a hash of the source content. On a cache hit, parsing, typechecking,
//! IR lowering, L1→L2 lowering, and L2 optimization are all skipped — the
//! cached L2 module is reused directly for code generation.
//!
//! This is an in-memory cache (no disk persistence) so it only helps within
//! a single long-lived process (e.g. the LSP server, `watch` mode, or the
//! test harness when compiling many files in one process). A future extension
//! could serialize the L2 module to disk for cross-process caching.

use crate::pir::L2Module;
use std::collections::HashMap;
use std::hash::Hash;
use std::sync::Mutex;

/// Cache key: (source_path, source_hash)
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
struct CacheKey {
    source_path: String,
    source_hash: u64,
}

/// Incremental compilation cache. Thread-safe via a single Mutex.
pub struct IncrementalCache {
    entries: Mutex<HashMap<CacheKey, L2Module>>,
}

impl IncrementalCache {
    /// Create a new empty cache.
    pub fn new() -> Self {
        IncrementalCache {
            entries: Mutex::new(HashMap::new()),
        }
    }

    /// Look up a cached L2 module by source path + hash.
    pub fn get(&self, source_path: &str, source_hash: u64) -> Option<L2Module> {
        let key = CacheKey {
            source_path: source_path.to_string(),
            source_hash,
        };
        let entries = self.entries.lock().unwrap();
        entries.get(&key).cloned()
    }

    /// Store an L2 module in the cache.
    pub fn store(&self, source_path: &str, source_hash: u64, l2: L2Module) {
        let key = CacheKey {
            source_path: source_path.to_string(),
            source_hash,
        };
        let mut entries = self.entries.lock().unwrap();
        entries.insert(key, l2);
    }

    /// Clear all cached entries.
    pub fn clear(&self) {
        let mut entries = self.entries.lock().unwrap();
        entries.clear();
    }

    /// Number of cached entries.
    pub fn len(&self) -> usize {
        let entries = self.entries.lock().unwrap();
        entries.len()
    }

    /// Whether the cache is empty.
    pub fn is_empty(&self) -> bool {
        self.len() == 0
    }
}

impl Default for IncrementalCache {
    fn default() -> Self {
        Self::new()
    }
}

/// Global incremental cache (lazy static).
static GLOBAL_CACHE: std::sync::OnceLock<IncrementalCache> = std::sync::OnceLock::new();

/// Get the global incremental compilation cache.
pub fn global_cache() -> &'static IncrementalCache {
    GLOBAL_CACHE.get_or_init(IncrementalCache::new)
}

/// Compute a hash of the source content (FNV-1a 64-bit).
pub fn hash_source(source: &str) -> u64 {
    let mut hash: u64 = 0xcbf29ce484222325;
    for byte in source.bytes() {
        hash ^= byte as u64;
        hash = hash.wrapping_mul(0x100000001b3);
    }
    hash
}

/// Simple hash function for strings (FNV-1a 64-bit).
pub fn hash_str(s: &str) -> u64 {
    hash_source(s)
}

// ─── Build inputs and on-disk stamps ───────────────────────────────────────
//
// A build records every file it reads (the program and each imported module)
// and every import candidate it found missing: creating that file would
// change which module the import resolves to. Two uses:
//   - the in-memory cache above is keyed on all of them, not only on the
//     main file (a changed import must not reuse stale code);
//   - `<output>.fides-stamp` records them with the compiler's identity and
//     the command line. A later build whose inputs are all unchanged, and
//     whose output still exists, is up to date and does no work. The stamp
//     also keeps a hash of the generated C, so a change that does not alter
//     the translation (comments, formatting) skips the C compiler.

/// The inputs one build read.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct Deps {
    /// (path, content hash) of each file read
    pub files: Vec<(String, u64)>,
    /// Import candidates that did not exist
    pub absent: Vec<String>,
}

impl Deps {
    /// One hash over every input (file contents and missing candidates).
    pub fn hash(&self) -> u64 {
        let mut h: u64 = 0xcbf29ce484222325;
        let mut mix = |s: &str| {
            for b in s.bytes() {
                h ^= b as u64;
                h = h.wrapping_mul(0x100000001b3);
            }
            h ^= 0xff;
            h = h.wrapping_mul(0x100000001b3);
        };
        for (path, fh) in &self.files {
            mix(path);
            mix(&format!("{:016x}", fh));
        }
        for path in &self.absent {
            mix(path);
        }
        h
    }

    /// Every recorded file still has its content and every missing
    /// candidate is still missing.
    pub fn unchanged(&self) -> bool {
        self.files.iter().all(|(path, fh)| {
            std::fs::read(path).map_or(false, |bytes| hash_bytes(&bytes) == *fh)
        }) && self.absent.iter().all(|path| !std::path::Path::new(path).exists())
    }
}

thread_local! {
    static DEPS: std::cell::RefCell<Deps> = std::cell::RefCell::new(Deps::default());
}

fn dep_path(path: &std::path::Path) -> String {
    let p = std::fs::canonicalize(path)
        .unwrap_or_else(|_| path.to_path_buf())
        .to_string_lossy()
        .to_string();
    // Windows canonical paths carry a verbatim prefix; plain paths read the same.
    match p.strip_prefix(r"\\?\") {
        Some(rest) if !rest.starts_with("UNC") => rest.to_string(),
        _ => p,
    }
}

/// Start recording the inputs of a new build.
pub fn deps_reset() {
    DEPS.with(|d| *d.borrow_mut() = Deps::default());
}

/// The inputs recorded since `deps_reset`.
pub fn deps_snapshot() -> Deps {
    DEPS.with(|d| d.borrow().clone())
}

/// Record that the build read `path` with these contents.
pub fn record_file(path: &std::path::Path, contents: &str) {
    let p = dep_path(path);
    let h = hash_source(contents);
    DEPS.with(|d| {
        let mut d = d.borrow_mut();
        if !d.files.iter().any(|(q, _)| *q == p) {
            d.files.push((p, h));
        }
    });
}

/// Record that the build looked for `path` and found nothing there.
pub fn record_absent(path: &std::path::Path) {
    let p = dep_path(path);
    DEPS.with(|d| {
        let mut d = d.borrow_mut();
        if !d.absent.contains(&p) {
            d.absent.push(p);
        }
    });
}

/// FNV-1a 64 over raw bytes (same as `hash_source` for UTF-8 text).
pub fn hash_bytes(bytes: &[u8]) -> u64 {
    let mut hash: u64 = 0xcbf29ce484222325;
    for &b in bytes {
        hash ^= b as u64;
        hash = hash.wrapping_mul(0x100000001b3);
    }
    hash
}

/// Identity of the running compiler: a rebuilt compiler invalidates stamps.
fn compiler_identity() -> String {
    let exe = std::env::current_exe().ok();
    let meta = exe.as_ref().and_then(|p| std::fs::metadata(p).ok());
    let len = meta.as_ref().map_or(0, |m| m.len());
    let mtime = meta
        .and_then(|m| m.modified().ok())
        .and_then(|t| t.duration_since(std::time::UNIX_EPOCH).ok())
        .map_or(0, |d| d.as_nanos());
    format!("{}:{}:{}", env!("CARGO_PKG_VERSION"), len, mtime)
}

/// Skips builds whose inputs have not changed (see the section comment).
pub struct BuildCache {
    key: String,
    stamp_path: String,
    output: String,
    main_input: String,
    old: Option<(String, Deps, Option<u64>)>,
}

impl BuildCache {
    /// Prepare a build of `input` writing `output` for `command args`.
    /// `--no-cache` (in `args`) or FIDES_NO_CACHE disables the cache.
    pub fn open(command: &str, args: &[String], input: &str, output: &str, env_vars: &[&str]) -> BuildCache {
        let disabled = args.iter().any(|a| a == "--no-cache") || std::env::var_os("FIDES_NO_CACHE").is_some();
        let mut key = format!("{} {}", compiler_identity(), command);
        for a in args.iter().filter(|a| *a != "--no-cache") {
            key.push(' ');
            key.push_str(a);
        }
        for v in env_vars {
            key.push_str(&format!(" {}={}", v, std::env::var(v).unwrap_or_default()));
        }
        let key = format!("{:016x}", hash_source(&key));
        let stamp_path = format!("{}.fides-stamp", output);
        let old = if disabled { None } else { read_stamp(&stamp_path) };
        deps_reset();
        BuildCache {
            key,
            stamp_path,
            output: output.to_string(),
            main_input: dep_path(std::path::Path::new(input)),
            old,
        }
    }

    fn same_key(&self) -> Option<&(String, Deps, Option<u64>)> {
        self.old.as_ref().filter(|(k, _, _)| *k == self.key)
    }

    /// Nothing changed since the build that wrote the output.
    pub fn up_to_date(&self) -> bool {
        std::path::Path::new(&self.output).exists()
            && self.same_key().map_or(false, |(_, deps, _)| !deps.files.is_empty() && deps.unchanged())
    }

    /// The previous build produced the output from exactly this C code.
    pub fn c_unchanged(&self, c_hash: u64) -> bool {
        std::path::Path::new(&self.output).exists()
            && self.same_key().map_or(false, |(_, _, c)| *c == Some(c_hash))
    }

    /// Inputs whose content hash differs between the previous build's
    /// stamp and the deps recorded since `open` — i.e. which files changed
    /// to force this rebuild. Call before `commit`.
    pub fn changed_inputs(&self) -> Vec<String> {
        let Some((_, old_deps, _)) = &self.old else { return Vec::new(); };
        let new_deps = deps_snapshot();
        let mut changed = Vec::new();
        for (p, h) in &new_deps.files {
            match old_deps.files.iter().find(|(q, _)| q == p) {
                Some((_, old_h)) if old_h == h => {}
                _ => changed.push(p.clone()),
            }
        }
        for (p, _) in &old_deps.files {
            if !new_deps.files.iter().any(|(q, _)| q == p) {
                changed.push(p.clone());
            }
        }
        changed.sort();
        changed.dedup();
        changed
    }

    /// Record a successful build. Nothing is written unless the build
    /// recorded its inputs (a pipeline that does not record them must
    /// never be reported up to date). `canon_hash` (Phase A) records the
    /// canonical C hash for diagnostics; older v1 stamps without it still
    /// parse (canon = the c hash, close enough for reporting).
    pub fn commit(&self, c_hash: Option<u64>) {
        self.commit_canon(c_hash, None);
    }

    pub fn commit_canon(&self, c_hash: Option<u64>, canon_hash: Option<u64>) {
        let deps = deps_snapshot();
        if !deps.files.iter().any(|(p, _)| *p == self.main_input) {
            let _ = std::fs::remove_file(&self.stamp_path);
            return;
        }
        let mut s = format!("fides-stamp 2\nkey {}\n", self.key);
        if let Some(c) = c_hash {
            s.push_str(&format!("c {:016x}\n", c));
        }
        if let Some(cn) = canon_hash {
            s.push_str(&format!("canon {:016x}\n", cn));
        }
        for (path, h) in &deps.files {
            s.push_str(&format!("file {:016x} {}\n", h, path));
        }
        for path in &deps.absent {
            s.push_str(&format!("absent {}\n", path));
        }
        let _ = std::fs::write(&self.stamp_path, s);
    }
}

fn read_stamp(path: &str) -> Option<(String, Deps, Option<u64>)> {
    let text = std::fs::read_to_string(path).ok()?;
    let mut lines = text.lines();
    match lines.next()? {
        "fides-stamp 1" | "fides-stamp 2" => {}
        _ => return None,
    }
    let mut key = None;
    let mut c = None;
    let mut deps = Deps::default();
    for line in lines {
        let (tag, rest) = line.split_once(' ')?;
        match tag {
            "key" => key = Some(rest.to_string()),
            "c" => c = Some(u64::from_str_radix(rest, 16).ok()?),
            "canon" => {}
            "file" => {
                let (h, p) = rest.split_once(' ')?;
                deps.files.push((p.to_string(), u64::from_str_radix(h, 16).ok()?));
            }
            "absent" => deps.absent.push(rest.to_string()),
            _ => {}
        }
    }
    Some((key?, deps, c))
}

// ─── Canonical C hashing and the on-disk artifact store ────────────────────
//
// `BuildCache` keys a build on its *inputs*. The artifact store keys a built
// binary on what the program *compiles to*: a canonical form of the
// generated C where comments and whitespace are gone, every identifier is
// alpha-renamed positionally (i0, i1, …), and unreferenced top-level
// function definitions are dropped. Two builds whose generated C differs
// only by renames, dead code, or formatting get the same canonical hash and
// reuse the same artifact — a rename or a comment edit never reruns the C
// compiler. C keywords, string/char literals, and preprocessor lines are
// kept verbatim so the hash stays semantics-exact.

const C_KEYWORDS: &[&str] = &[
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "inline", "int", "long", "register", "restrict", "return", "short",
    "signed", "sizeof", "static", "struct", "switch", "typedef", "union",
    "unsigned", "void", "volatile", "while", "_Bool", "_Alignas",
    "_Alignof", "_Atomic", "_Generic", "_Noreturn", "_Static_assert",
    "_Thread_local",
];

#[derive(Debug, Clone, PartialEq)]
enum CTok {
    /// Identifier (not yet renamed)
    Ident(String),
    /// Everything else: punctuation, numbers, string/char literals,
    /// whole preprocessor lines — kept verbatim.
    Lit(String),
}

fn lex_c(src: &str) -> Vec<CTok> {
    let b = src.as_bytes();
    let mut toks = Vec::new();
    let mut i = 0;
    let mut at_line_start = true;
    while i < b.len() {
        let c = b[i];
        if c == b'\n' || c == b'\r' || c == b' ' || c == b'\t' || c == 0x0b || c == 0x0c {
            at_line_start = c == b'\n';
            i += 1;
            continue;
        }
        if c == b'/' && i + 1 < b.len() && b[i + 1] == b'/' {
            while i < b.len() && b[i] != b'\n' { i += 1; }
            continue;
        }
        if c == b'/' && i + 1 < b.len() && b[i + 1] == b'*' {
            i += 2;
            while i + 1 < b.len() && !(b[i] == b'*' && b[i + 1] == b'/') { i += 1; }
            i = (i + 2).min(b.len());
            continue;
        }
        if c == b'#' && at_line_start {
            let start = i;
            while i < b.len() && b[i] != b'\n' { i += 1; }
            // A line ending in '\' continues the directive.
            while i < b.len() && b[..i].ends_with(b"\\") {
                i += 1;
                while i < b.len() && b[i] != b'\n' { i += 1; }
            }
            let line = std::str::from_utf8(&b[start..i]).unwrap_or("").trim().to_string();
            toks.push(CTok::Lit(line));
            continue;
        }
        at_line_start = false;
        if c.is_ascii_alphabetic() || c == b'_' {
            let start = i;
            while i < b.len() && (b[i].is_ascii_alphanumeric() || b[i] == b'_') { i += 1; }
            toks.push(CTok::Ident(String::from_utf8_lossy(&b[start..i]).to_string()));
            continue;
        }
        if c == b'"' || c == b'\'' {
            let q = c;
            let start = i;
            i += 1;
            while i < b.len() && b[i] != q {
                if b[i] == b'\\' { i += 1; }
                i += 1;
            }
            i = (i + 1).min(b.len());
            toks.push(CTok::Lit(String::from_utf8_lossy(&b[start..i]).to_string()));
            continue;
        }
        if c.is_ascii_digit() {
            let start = i;
            while i < b.len() && (b[i].is_ascii_alphanumeric() || b[i] == b'.' || b[i] == b'_') { i += 1; }
            toks.push(CTok::Lit(String::from_utf8_lossy(&b[start..i]).to_string()));
            continue;
        }
        toks.push(CTok::Lit((c as char).to_string()));
        i += 1;
    }
    toks
}

/// Canonicalize generated C: strip comments/whitespace, alpha-rename all
/// non-keyword identifiers positionally, drop unreferenced top-level
/// function definitions. Deterministic across renames and dead-code edits.
pub fn canonicalize_c(src: &str) -> String {
    let toks = lex_c(src);

    // Find top-level function definitions: Ident followed by (…){…} at
    // brace depth 0. Token indices of each def's name and [start,end) span.
    let mut defs: Vec<(String, usize, usize, usize)> = Vec::new(); // (name, name_idx, start, end)
    let mut depth: i64 = 0;
    let mut stmt_start = 0usize;
    let mut j = 0usize;
    while j < toks.len() {
        if let CTok::Lit(l) = &toks[j] {
            if l == "{" { depth += 1; }
            else if l == "}" {
                depth -= 1;
                if depth == 0 { stmt_start = j + 1; }
            } else if l == ";" && depth == 0 { stmt_start = j + 1; }
        }
        if depth == 0 {
            if let CTok::Ident(name) = &toks[j] {
                // is next token '(' with its match followed by '{'?
                if let Some(CTok::Lit(l)) = toks.get(j + 1) {
                    if l == "(" {
                        let mut d = 0i64;
                        let mut k = j + 1;
                        while k < toks.len() {
                            if let CTok::Lit(l2) = &toks[k] {
                                if l2 == "(" { d += 1; } else if l2 == ")" { d -= 1; }
                                if d == 0 { break; }
                            }
                            k += 1;
                        }
                        if matches!(toks.get(k + 1), Some(CTok::Lit(l3)) if l3 == "{") {
                            // find matching close brace
                            let mut d2 = 0i64;
                            let mut m = k + 1;
                            while m < toks.len() {
                                if let CTok::Lit(l4) = &toks[m] {
                                    if l4 == "{" { d2 += 1; } else if l4 == "}" { d2 -= 1; }
                                    if d2 == 0 { break; }
                                }
                                m += 1;
                            }
                            defs.push((name.clone(), j, stmt_start, (m + 1).min(toks.len())));
                            j = m + 1;
                            stmt_start = j;
                            continue;
                        }
                    }
                }
            }
        }
        j += 1;
    }

    // Liveness fixpoint: roots are `main` plus every identifier used
    // outside any def body (globals, declarations, initializers).
    let mut live: std::collections::HashSet<String> = std::collections::HashSet::new();
    live.insert("main".to_string());
    let mut in_body = vec![false; toks.len()];
    for (_, name_idx, start, end) in &defs {
        for t in *start..*end { in_body[t] = true; }
        let _ = name_idx;
    }
    // mark header (name through parens) as outside-body for ref purposes is
    // unnecessary — header only contains the def name and param names.
    for (t, tok) in toks.iter().enumerate() {
        if !in_body[t] {
            if let CTok::Ident(n) = tok { live.insert(n.clone()); }
        }
    }
    let def_names: std::collections::HashSet<String> = defs.iter().map(|d| d.0.clone()).collect();
    loop {
        let before = live.len();
        for (name, name_idx, _, end) in &defs {
            if !live.contains(name) { continue; }
            // refs inside this live def's tokens pull in more names
            for tok in toks.iter().take(*end).skip(*name_idx + 1) {
                if let CTok::Ident(n) = tok {
                    if def_names.contains(n) { live.insert(n.clone()); }
                }
            }
        }
        if live.len() == before { break; }
    }
    let dead_spans: std::collections::HashSet<usize> = defs.iter()
        .filter(|(n, _, _, _)| !live.contains(n))
        .flat_map(|(_, _, s, e)| *s..*e)
        .collect();

    // Alpha-rename non-keyword identifiers positionally; emit kept tokens.
    let mut out = String::with_capacity(src.len() / 4);
    let mut names: std::collections::HashMap<String, u64> = std::collections::HashMap::new();
    let mut next = 0u64;
    for (t, tok) in toks.iter().enumerate() {
        if dead_spans.contains(&t) { continue; }
        match tok {
            CTok::Ident(n) => {
                if C_KEYWORDS.contains(&n.as_str()) {
                    out.push_str(n);
                } else {
                    let id = *names.entry(n.clone()).or_insert_with(|| { let v = next; next += 1; v });
                    out.push_str(&format!("i{}", id));
                }
            }
            CTok::Lit(l) => out.push_str(l),
        }
        out.push(' ');
    }
    out
}

/// Content hash of the canonicalized C (see `canonicalize_c`).
pub fn canon_hash(c_code: &str) -> u64 {
    hash_source(&canonicalize_c(c_code))
}

/// On-disk content-addressed artifact store: `<dir>/.fidcache/<key>.exe`.
/// Stores built binaries keyed on (canonical C hash, toolchain, flags) so
/// any build — in this checkout or another, for this output or a different
/// one — reuses a byte-identical artifact instead of rerunning the C
/// compiler.
pub struct ArtifactStore {
    dir: std::path::PathBuf,
}

impl ArtifactStore {
    /// Store for artifacts beside `output`'s directory (`.fidcache/`).
    /// `None` when FIDES_NO_CACHE is set or --no-cache was passed.
    pub fn open(output: &str, disabled: bool) -> Option<ArtifactStore> {
        if disabled || std::env::var_os("FIDES_NO_CACHE").is_some() {
            return None;
        }
        let parent = std::path::Path::new(output).parent()
            .filter(|p| !p.as_os_str().is_empty())
            .unwrap_or_else(|| std::path::Path::new("."));
        Some(ArtifactStore { dir: parent.join(".fidcache") })
    }

    /// Artifact key for a canonical hash built by `tool` with `flags`.
    pub fn key(canon: u64, tool: &str, flags: &str) -> String {
        format!("{:016x}", hash_source(&format!("a1 {:016x} {} {}", canon, tool, flags)))
    }

    fn path(&self, key: &str) -> std::path::PathBuf {
        self.dir.join(format!("{}.exe", key))
    }

    /// A previously stored artifact for this key.
    pub fn get(&self, key: &str) -> Option<Vec<u8>> {
        std::fs::read(self.path(key)).ok()
    }

    /// Store `bytes` under `key`. Best-effort: cache failures never break a build.
    pub fn store(&self, key: &str, bytes: &[u8]) {
        let _ = std::fs::create_dir_all(&self.dir);
        let tmp = self.dir.join(format!("{}.tmp", key));
        if std::fs::write(&tmp, bytes).is_ok() {
            let _ = std::fs::rename(&tmp, self.path(key));
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_cache_store_and_get() {
        let cache = IncrementalCache::new();
        assert!(cache.is_empty());

        // Use a dummy L2 module (empty default).
        let l2 = L2Module::default();
        let h = hash_source("fn main() {}");
        cache.store("test.fd", h, l2.clone());

        assert_eq!(cache.len(), 1);
        let got = cache.get("test.fd", h);
        assert!(got.is_some());
        assert_eq!(got.unwrap().functions.len(), l2.functions.len());
    }

    #[test]
    fn test_cache_miss_on_different_hash() {
        let cache = IncrementalCache::new();
        let l2 = L2Module::default();
        let h1 = hash_source("fn main() {}");
        let h2 = hash_source("fn main() { x }");
        cache.store("test.fd", h1, l2);

        assert!(cache.get("test.fd", h1).is_some());
        assert!(cache.get("test.fd", h2).is_none());
    }

    #[test]
    fn test_cache_miss_on_different_path() {
        let cache = IncrementalCache::new();
        let l2 = L2Module::default();
        let h = hash_source("fn main() {}");
        cache.store("a.fd", h, l2);

        assert!(cache.get("a.fd", h).is_some());
        assert!(cache.get("b.fd", h).is_none());
    }

    #[test]
    fn test_cache_clear() {
        let cache = IncrementalCache::new();
        cache.store("a.fd", 1, L2Module::default());
        cache.store("b.fd", 2, L2Module::default());
        assert_eq!(cache.len(), 2);
        cache.clear();
        assert!(cache.is_empty());
    }

    #[test]
    fn test_hash_deterministic() {
        assert_eq!(hash_source("hello"), hash_source("hello"));
        assert_ne!(hash_source("hello"), hash_source("world"));
    }

    #[test]
    fn test_stamp_tracks_inputs() {
        let dir = std::env::temp_dir().join(format!("fides-inc-test-{}", std::process::id()));
        let _ = std::fs::create_dir_all(&dir);
        let src = dir.join("main.fd");
        let out = dir.join("main.exe");
        let missing = dir.join("other.fd");
        std::fs::write(&src, "fn main() {}").unwrap();
        std::fs::write(&out, "binary").unwrap();
        let args = vec![src.to_string_lossy().to_string()];
        let out_s = out.to_string_lossy().to_string();

        let cache = BuildCache::open("t", &args, &args[0], &out_s, &[]);
        assert!(!cache.up_to_date(), "no stamp yet");
        record_file(&src, "fn main() {}");
        record_absent(&missing);
        cache.commit(Some(7));

        let cache = BuildCache::open("t", &args, &args[0], &out_s, &[]);
        assert!(cache.up_to_date());
        assert!(cache.c_unchanged(7));
        assert!(!cache.c_unchanged(8));

        // A missing import candidate appearing invalidates the build.
        std::fs::write(&missing, "x").unwrap();
        assert!(!BuildCache::open("t", &args, &args[0], &out_s, &[]).up_to_date());
        let _ = std::fs::remove_file(&missing);

        // So does a changed input, or a different command line.
        assert!(BuildCache::open("t", &args, &args[0], &out_s, &[]).up_to_date());
        assert!(!BuildCache::open("u", &args, &args[0], &out_s, &[]).up_to_date());
        std::fs::write(&src, "fn main() { }").unwrap();
        assert!(!BuildCache::open("t", &args, &args[0], &out_s, &[]).up_to_date());
        let _ = std::fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_commit_needs_recorded_main_input() {
        let dir = std::env::temp_dir().join(format!("fides-inc-test2-{}", std::process::id()));
        let _ = std::fs::create_dir_all(&dir);
        let src = dir.join("main.fd");
        let out = dir.join("main.exe");
        std::fs::write(&src, "fn main() {}").unwrap();
        std::fs::write(&out, "binary").unwrap();
        let args = vec![src.to_string_lossy().to_string()];
        let out_s = out.to_string_lossy().to_string();
        // A pipeline that recorded nothing never becomes "up to date".
        BuildCache::open("t", &args, &args[0], &out_s, &[]).commit(None);
        assert!(!BuildCache::open("t", &args, &args[0], &out_s, &[]).up_to_date());
        let _ = std::fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_canon_comment_and_ws_invariant() {
        let a = "int main(void) {\n  return 0; // hi\n}\n";
        let b = "int   main(void)\n{\n/* block\ncomment */\nreturn 0;\n}\n";
        assert_eq!(canonicalize_c(a), canonicalize_c(b));
    }

    #[test]
    fn test_canon_rename_invariant() {
        let a = "int64_t helper(int64_t x) { return x + 1; }\nint main(void) { return helper(2); }\n";
        let b = "int64_t compute(int64_t v) { return v + 1; }\nint main(void) { return compute(2); }\n";
        assert_eq!(canonicalize_c(a), canonicalize_c(b));
    }

    #[test]
    fn test_canon_body_change_detected() {
        let a = "int f(void) { return 1; }\nint main(void) { return f(); }\n";
        let b = "int f(void) { return 2; }\nint main(void) { return f(); }\n";
        assert_ne!(canonicalize_c(a), canonicalize_c(b));
    }

    #[test]
    fn test_canon_dead_function_dropped() {
        let a = "int live(void) { return 1; }\nint main(void) { return live(); }\n";
        let b = "int dead(void) { return 9; }\nint live(void) { return 1; }\nint main(void) { return live(); }\n";
        assert_eq!(canonicalize_c(a), canonicalize_c(b));
    }

    #[test]
    fn test_canon_dead_chain_dropped() {
        // dead -> also_dead chain: both must go (fixpoint liveness)
        let a = "int live(void) { return 1; }\nint main(void) { return live(); }\n";
        let b = "int inner(void) { return 9; }\nint outer(void) { return inner(); }\nint live(void) { return 1; }\nint main(void) { return live(); }\n";
        assert_eq!(canonicalize_c(a), canonicalize_c(b));
    }

    #[test]
    fn test_canon_keeps_live_called_functions() {
        let a = "int g(void) { return 7; }\nint f(void) { return g(); }\nint main(void) { return f(); }\n";
        let b = "int g(void) { return 8; }\nint f(void) { return g(); }\nint main(void) { return f(); }\n";
        assert_ne!(canonicalize_c(a), canonicalize_c(b));
    }

    #[test]
    fn test_canon_string_literals_verbatim() {
        // A renamed identifier inside a string literal is data, not code.
        let a = "int main(void) { char* s = \"hello world\"; return 0; }\n";
        let canon = canonicalize_c(a);
        assert!(canon.contains("\"hello world\""), "canon: {}", canon);
    }

    #[test]
    fn test_artifact_store_roundtrip() {
        let dir = std::env::temp_dir().join(format!("fides-art-test-{}", std::process::id()));
        let out = dir.join("prog.exe");
        std::fs::create_dir_all(&dir).unwrap();
        let store = ArtifactStore::open(out.to_str().unwrap(), false).unwrap();
        let key = ArtifactStore::key(42, "gcc", "2 None");
        assert!(store.get(&key).is_none());
        store.store(&key, b"MZ-binary");
        assert_eq!(store.get(&key).unwrap(), b"MZ-binary");
        let _ = std::fs::remove_dir_all(&dir);
    }

    #[test]
    fn test_artifact_store_disabled() {
        assert!(ArtifactStore::open("x.exe", true).is_none());
    }

    #[test]
    fn test_global_cache() {
        let cache = global_cache();
        let h = hash_source("global test");
        // Should not panic and should be reusable.
        assert!(cache.get("global.fd", h).is_none());
    }
}
