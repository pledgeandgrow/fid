# Security review package — J5–J9 evidence index

Prepared for the independent security review (release-roadmap J9). This
document is the reviewer-facing index: scope, threat model, where the
evidence lives, and how to reproduce every claim. It does not claim the
review has happened — J9 stays open until a qualified external reviewer
signs off.

## Scope for review

- `stdlib/crypto.fd` — SHA-256, HMAC-SHA256, constant-time compare,
  `crypto_random_*` (OS CSPRNG), nonce tracker.
- `stdlib/crypto_bind.fd` — AEAD (AES-GCM), signatures (ECDSA P-256) and a
  KDF (PBKDF2-HMAC-SHA256) via Windows BCrypt (CNG) bindings.
- Safety annotations: `@constant_time`, `@secret`, `@declassify`,
  `@zeroize`, `@wrapping`, `@crypto-approved` — checkers in
  `compiler/typechecker.fd` (source level) and the machine-level verifier.
- `@zeroize` lowering (`compiler/lower.fd`), the C runtime wipe
  (`compiler/codegen_c.fd`), the sensitive-Vec buffer registry, and the
  native-runtime equivalents (`stdlib/pe_runtime/base.fd`,
  `stdlib/linux_runtime/base.fd`).

## Threat model

- Secrets (keys, derived material) must not leak through freed buffers
  (J8), timing variation in annotated functions (J6/J7), or accidental
  output flows (`@secret`).
- Crypto correctness comes from binding the OS libraries (decision 2):
  BCrypt on Windows; on Linux and macOS `use crypto_bind` takes
  `stdlib/crypto_bind_unix.fd` (libcrypto EVP, written Nov 15) — **not yet
  built or run on Linux** (known gap).
- Interpreters and the JIT-ish paths are out of scope for timing claims:
  `@constant_time` is a compiled-code property.

## What exists today (J5–J8 status)

| Item | Status | Evidence |
|------|--------|----------|
| J5 AEAD/sig/KDF bindings | Implemented (Windows BCrypt) | `stdlib/crypto_bind.fd`, `tests/bootstrap/test_crypto_bind.fd` (NIST GCM tag, RFC PBKDF2, ECDSA sign/verify + negative) |
| J6 annotations applied | Done | `@constant_time` on the pure SHA-256 helpers; `@secret`/`@zeroize`/`@declassify` on key material in `stdlib/crypto.fd`; `fid check` clean; SHA/HMAC vectors pass (`test_crypto_vectors.fd`) |
| J7 native CT check | Implemented | `ng_ct_check` in `compiler/nat_ir.fd` (MIR-level; runs on `--nat` Windows and the Linux targets); `peir_ct_check` in `compiler/pe_ir.fd` (Nov 15: the default Windows backend, also `--bare`/`--elf`; `FID_PE_CT_OFF=1` turns it off; `scripts/pe-ct-check.sh`). `IrFunction.is_constant_time` serialized in `ir_ser.fd`. Source-level checker unchanged |
| J8 old-buffer zeroize | Implemented, verified on C path | sensitive-Vec registry: a `@zeroize` Vec's header is marked; every buffer it holds lives in a sensitive-buffer set with its byte size. `__p_vbuf_free` wipes set members (covers `__p_vec_grow` growth frees and shared buffers' last-owner frees); volatile writes. `pert_sensitive_*` mirrors it in both native runtimes. `tests/bootstrap/test_zeroize_vec.fd` reports **3 wipes**; `p_sensitive_wipes()` counter exposed for tests |
| J9 independent review | **Open** | this document is the package, not the review |

## Known limitations (be honest in review)

1. `p_sensitive_*` table is fixed at 256 buffers and is not thread-safe.
2. The native-runtime (`pert_*`) wipe is a plain byte loop — the native
   optimizer could in principle elide dead stores; the C runtime uses
   volatile writes. Hardening item for review.
3. The Linux/macOS AEAD/sign/KDF bindings (`stdlib/crypto_bind_unix.fd`,
   OpenSSL EVP, the same `cb_*` functions and key blobs) have not been run:
   their check, `scripts/verify/crypto_bind_unix_check.fd`, needs a Linux
   machine with libcrypto and is not in the suite.
4. Shared `cap < 0` buffers are wiped at their recorded size; a buffer
   grown while shared keeps its last registered size — byte-exact coverage
   of the shared tail is a review item.
5. Interpreter Vecs are managed values — `@zeroize` and the registry are
   no-ops there by design.
6. `crypto_bind` re-opens the provider per call — slow but simple; key
   objects are one-shot.
7. The native J8 path (pe_ir, `--nat`, the Linux targets) is verified by
   construction, not by an end-to-end run: `test_zeroize_vec.fd` has not
   been run on those backends (release-roadmap J8).
8. The earlier header-keyed `p_sensitive_free(v, buf, cap)` hooks remain in
   `stdlib/core.fd` for the Fides-level grow/drop paths; the C-runtime set
   covers the paths those hooks cannot see.

## Reproduce

```bash
# J5 — BCrypt bindings (Windows, needs -lbcrypt):
release/fid.exe ccompile tests/bootstrap/test_crypto_bind.fd -o cb.exe
./cb.exe                       # gcm-tag ok / gcm-decrypt ok / pbkdf2 ok / ecdsa ok

# J6 — annotations + vectors:
release/fid.exe check stdlib/crypto.fd
release/fid.exe ccompile tests/bootstrap/test_crypto_vectors.fd -o cv.exe && ./cv.exe

# J7 — native CT check (a CT fn that gains a conditional branch in MIR is
# rejected by `emit --nat`):
release/fid.exe emit <file-with-@constant_time> --nat -o out.exe

# J8 — growth wipes: a @zeroize Vec<i64> grown past capacity reports the
# number of wiped buffers via p_sensitive_wipes() (prints 3, "J8 ok"):
release/fid.exe ccompile tests/bootstrap/test_zeroize_vec.fd -o j8.exe && ./j8.exe
```

## Reviewer checklist suggestion

1. Audit the CT checker's coverage: MIR ops rejected vs. what the encoder
   can still emit (indexed addressing, cmov, division).
2. Audit `p_sensitive_free` call sites against every buffer-free path in
   `stdlib/core.fd` (VEC_BUF_RC paths included).
3. Review `crypto_bind` buffer lifetime: BCrypt reads `key_obj` while the
   key lives — the module deliberately keeps it; verify no use-after-free.
4. Compare the C-runtime wipe vs. compiler optimizations at -O2.
