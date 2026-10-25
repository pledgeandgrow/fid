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

    /// Record a successful build. Nothing is written unless the build
    /// recorded its inputs (a pipeline that does not record them must
    /// never be reported up to date).
    pub fn commit(&self, c_hash: Option<u64>) {
        let deps = deps_snapshot();
        if !deps.files.iter().any(|(p, _)| *p == self.main_input) {
            let _ = std::fs::remove_file(&self.stamp_path);
            return;
        }
        let mut s = format!("fides-stamp 1\nkey {}\n", self.key);
        if let Some(c) = c_hash {
            s.push_str(&format!("c {:016x}\n", c));
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
    if lines.next()? != "fides-stamp 1" {
        return None;
    }
    let mut key = None;
    let mut c = None;
    let mut deps = Deps::default();
    for line in lines {
        let (tag, rest) = line.split_once(' ')?;
        match tag {
            "key" => key = Some(rest.to_string()),
            "c" => c = Some(u64::from_str_radix(rest, 16).ok()?),
            "file" => {
                let (h, p) = rest.split_once(' ')?;
                deps.files.push((p.to_string(), u64::from_str_radix(h, 16).ok()?));
            }
            "absent" => deps.absent.push(rest.to_string()),
            _ => return None,
        }
    }
    Some((key?, deps, c))
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
    fn test_global_cache() {
        let cache = global_cache();
        let h = hash_source("global test");
        // Should not panic and should be reusable.
        assert!(cache.get("global.fd", h).is_none());
    }
}
