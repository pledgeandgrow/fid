# scripts/verify/sanitize-patches.sed: the fixes of sanitizer findings S1-S3
# (docs/verification/infra-runs-2026-11-16.md) applied to generated C, so
# that a run looks past them before compiler/codegen_c.fd has them:
#   PATCH=scripts/verify/sanitize-patches.sed bash scripts/sanitize-wsl.sh
# A line that no longer matches (the fix is in) changes nothing.
# S1: two more words in front of a large / sanitizer allocation
s/uint64_t\* big = (uint64_t\*)calloc(1, total);/uint64_t* big = (uint64_t*)calloc(1, total + 16); if (big) big += 2;/
# (the header word stays at b - 1; the block starts two words before it)
/if (k == P_CLASS_LARGE)/s/free(b - 1);/free(b - 3);/
s/uint64_t\* b = (uint64_t\*)calloc(1, (size_t)size + 8); if (!b) { p_panic("p_shared_alloc: out of memory"); }/uint64_t* b = (uint64_t*)calloc(1, (size_t)size + 24); if (!b) { p_panic("p_shared_alloc: out of memory"); } b += 2;/
# S2: p_tensor_to_str copies its libc buffer into a counted string
# S3: a global str with a literal "" initializer gets the static header
s/^char\* p_tensor_to_str(void\* t) { return p_str_own_c(p_tensor_to_str__raw(t)); }/char* p_tensor_to_str(void* t) { char* r = p_tensor_to_str__raw(t); char* o = p_str_from_bytes_c(r, strlen(r)); free(r); return o; }/
s/^static void\* \(_g_[A-Za-z_0-9]*\) = "";$/static const struct { int64_t m; int64_t l; char s[1]; } \1_h = { P_STR_STATIC_C, 0LL, "" }; static void* \1 = (void*)\1_h.s;/
