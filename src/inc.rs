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
    fn test_global_cache() {
        let cache = global_cache();
        let h = hash_source("global test");
        // Should not panic and should be reusable.
        assert!(cache.get("global.fd", h).is_none());
    }
}
