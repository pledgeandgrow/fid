# Stdlib depth audit (Nov 18, 2026)

Every module of `stdlib/` (403) was read by one of eight reviewers against what its name and header promise: is it complete, does it fake anything, is it tested. Read-only; nothing was changed by the audit. Verdicts:

- **COMPLETE**: does what it promises, nothing faked, tested.
- **SOLID-GAPS**: real and working, but missing what a user of that domain would expect, or partly untested.
- **THIN**: a small core; most of what the name promises is missing.
- **STUB**: placeholder code (constant results, ignored inputs, fake work) in a significant part.
- **BROKEN**: cannot work as written, or gives wrong results.

Priority: **P1** core or widely used and thin/stubbed/wrong (users hit it); **P2** domain module with real gaps; **P3** polish. Effort to complete: S < 1 h, M a few hours, L a day or more. Platform seams (I/O passed in, `extern "C"` to a system library) are not stubs when the pure side is real.

How it was checked: every file read (the largest well-tested ones by header, every `pub fn` and spot-read bodies, which the per-module sections say); every module without a C-sweep result type-checked with a one-stage compiler of Nov 17; several findings confirmed by running small programs (hash, bits, collections, core, mesh, log, asset). Findings come from reading unless a section says it was run: verify a finding before relying on it (one, sunspec model 203, is marked as unverified).

## Totals

| Verdict | Modules |
|---|---|
| BROKEN | 12 |
| STUB | 22 |
| THIN | 33 |
| SOLID-GAPS | 258 |
| COMPLETE | 78 |

Priority: P1 29, P2 172, P3 202. Effort of the 325 that are not complete: S 115, M 186, L 24.

By age (git: tracked before Nov 16 vs added Nov 16-17):

| | Modules | BROKEN | STUB | THIN | SOLID-GAPS | COMPLETE |
|---|---|---|---|---|---|---|
| Before Nov 16 | 169 | 12 | 22 | 30 | 96 | 9 |
| Added Nov 16-17 | 234 | 0 | 0 | 3 | 162 | 69 |

The weak spots are the older modules, many of them core ones (collections, string, hash, iter, vec, sort, random, os, env, sys). The new ones were written to "no placeholders" rules and tested against published vectors; their gaps are missing features, not fake parts.

## Fix first: P1 (users hit these)

| module | verdict | effort | what is wrong |
|---|---|---|---|
| `collections` | BROKEN | L | HashSet treats 0/-1 as empty/tombstone (contains(0) true, insert(0) lost); leaks; i64-only, fake Deque<T>; O(n) push_front |
| `hash` | BROKEN | S | hash_djb2/hash_djb2_bytes panic on integer overflow for inputs of about 11+ bytes (confirmed by a run); hash_combine is XOR, so hash_vec ignores order and cancels pairs; no 64-bit or keyed (SipHash) hash, no streaming hasher |
| `uuid` | BROKEN | M | v4 comes from the global LCG seeded with 1: every process makes the same UUID sequence; no v1/v3/v5/v7; lax parser, no Option |
| `arena` | STUB | M | region_* allocate untracked and region_free is a C no-op (memory never freed); arena_used/region_count return 0; one global arena |
| `db` | STUB | M | MemoryDB is a key-value store, not SQL; helpers make injectable or wrong SQL; query_scalar fake |
| `parallel` | STUB | M | par_map ignores its "function" and returns squares; only 2-way sum/dot; fill/range/hint sequential or no-op |
| `string` | STUB | M | Rope is fake (concat drops children, to_str gives ""); SmallStr never inline; int_to_hex/int_to_bin give "" for negatives; join is O(n^2); no test of its own |
| `sys` | STUB | S | sys_arch hard-coded "x86_64"; endianness always little; sys_is_windows false on Windows and sys_is_macos never true (os_name); no hostname/OS version/uptime |
| `env` | THIN | M | no set/unset/list, unset vs empty conflated, a stale "native stub" comment, duplicates os.fd |
| `iter` | THIN | M | i64 only although generics exist; no zip/enumerate/fold to another type/sort_by/rev/windows/min_by/max_by/position/flat_map; min/max of an empty Vec give 0, not Option; eager Vec copies only |
| `os` | THIN | M | system info is env-var guesses (os_name "linux" default, cwd from $PWD, hostname from $HOSTNAME); Unix-only separators; no setenv/chdir/pid |
| `queue` | THIN | S | `Queue<T>` holds i64 only; dequeue/peek return 0 when empty (no Option); no iteration |
| `sqlite` | THIN | M | panics on every error; binds text only; BLOB reads as null; no typed rows, no reusable prepared statements; step errors silently cut results; header says it implements db's `Database` cap, but it does not |
| `typed_strings` | THIN | M | sql_bind can be injected: it does not quote the value and it replaces every `?`. Any str can be wrapped. json_escape leaves most control characters. path_safe accepts absolute paths |
| `core` | SOLID-GAPS | L | Vec::sort compares as i64 (str by address, f64 by bits); str_parse_int panics on long input, accepts junk; no fallible parse; get() -> 0 on miss |
| `derive_json` | SOLID-GAPS | M | Vec/Option/enum/map fields silently default; no surrogate pairs; int accepts "1.5"; no range check |
| `dns` | SOLID-GAPS | M | shell injection in dns_resolve_system; fixed query id, reply not matched; A/AAAA only; no TCP fallback |
| `fmt` | SOLID-GAPS | M | fmt_float truncates (no rounding); hex/bin/octal of negatives give ""; header's fmt_table/fmt_pad don't exist; no scientific/significant-digit or NaN/inf formatting |
| `fs` | SOLID-GAPS | M | fs_rmdir_all and fs_walk follow symlinks (rm -rf through a link deletes the target's contents; a link cycle recurses forever); temp files not created exclusively (check-then-write race, predictable names); no stat/mtime/permissions/symlink API; read errors are "" like an empty file; File loads the whole file and rewrites the whole file on every flush that holds a NUL; untracked test_fs_stream.fd calls fs_modified_ms and a streaming File that fs.fd lacks |
| `http` | SOLID-GAPS | M | fake arena + pass-through middleware + exact-match "router" (superseded by router.fd/middleware.fd); unbounded chunk-size parse (overflow panic = remote crash); no query/url-decode helpers; no 100-continue, no streamed/chunked responses; short status table |
| `https` | SOLID-GAPS | M | no test at all; 303 keeps POST; Host header drops non-default port; own URL splitter breaks on `?` without path, IPv6, userinfo; no timeout/redirect knobs; redirects re-send body |
| `json` | SOLID-GAPS | M | validation path has no depth limit (stack overflow on hostile input); every string accessor re-validates the whole text; quadratic json_string/unescape/builders; no f64 builder |
| `math` | SOLID-GAPS | M | perspective() uses 1/sin instead of 1/tan; look_at() rotation transposed; no tan/atan/atan2/asin/acos/round/trunc/fmod/hypot (5 modules reimplement them); slerp is nlerp; projections untested |
| `quic_tls` | SOLID-GAPS | L | no server authentication without a pin: no chain/CA/hostname check, RSA/ECDSA CertificateVerify accepted unchecked; Ed25519-only server certs; X25519 only; no 0-RTT anti-replay |
| `random` | SOLID-GAPS | M | 31-bit LCG: random_bool strictly alternates, random_byte has period 256; random_float can return 1.0; modulo bias, nothing above 2^31; one global state (RngState unused), so 9 modules carry their own xorshift; no gaussian or u64 |
| `sort` | SOLID-GAPS | S | last-element-pivot recursive quicksort: O(n^2) and recursion depth n on sorted input (stack overflow on large Vecs), while core's Vec_sort is a heapsort; no stable sort, no sort_by_key; i64 only |
| `sync` | SOLID-GAPS | L | RwLock serializes readers (a mutex in disguise); no condvar, semaphore, once, barrier, try_lock; handles never freed |
| `vec` | SOLID-GAPS | M | every helper is typed `Vec<i64>`; chunk/window return indices; zip/enumerate return flat Vecs; `vec_chunk(v, 0)` never ends; `find` returns -1 instead of Option; O(n^2) sort_by; the adapters are tested only by doc examples |
| `world` | SOLID-GAPS | S-M | the socket fake is not connected: stdlib/net.fd never calls world_sock_*; binary files cannot be faked; no listen/accept or IPv6 in the socket fake |

## Broken and stubbed (any priority)

- **BROKEN** (12): `collections`, `hash`, `uuid`, `arinc`, `bits`, `can`, `clinical`, `font`, `gl`, `mesh`, `sdl`, `serialize`
- **STUB** (22): `arena`, `db`, `parallel`, `string`, `sys`, `asset`, `audio`, `audio_mixer`, `data`, `ecs`, `events`, `game`, `grid`, `image`, `input`, `medical`, `ota`, `quant`, `shader`, `skel`, `streaming`, `ui`
- **THIN** (33): `env`, `iter`, `os`, `queue`, `sqlite`, `typed_strings`, `alloc`, `banking`, `bpe`, `chunk`, `dp`, `ecu`, `editor`, `elf`, `fdt`, `navmesh`, `ocpp`, `openadr`, `plot`, `rc`, `result`, `scada`, `set`, `slab`, `stack`, `storage`, `sunspec`, `swift`, `template`, `tensor`, `virtio`, `memo`, `shape`

## Across modules

These recur across modules; fixing each once, centrally, removes many gaps.

**Security bugs (fix whatever the module's priority):**
- Injection: `dns_resolve_system` passes the host name to a shell unescaped; `typed_strings.sql_bind` does not quote and replaces every `?`; `db.qb_param` substitutes raw text; `smtp` lets CR/LF through (header and command injection); `markdown` passes `javascript:` links.
- Missing verification: `quic_tls` accepts RSA/ECDSA certificates without checking CertificateVerify, and checks no chain or host name (x509 already has both); `ota` compares an FNV hash against a `sha256:` checksum and never checks the signature; `storage`'s Merkle tree uses FNV-1a.
- Untrusted input crashes or exhausts: `json_is_valid` has no depth limit (stack overflow); `http` chunk-size overflow panics the server; `ws` allocates whatever length a peer sends and skips RFC 6455's frame checks; `srtp` grows per-SSRC state before authenticating; `core.str_parse_int` panics on 20 digits; `iec61850` GOOSE and `gltf` accessors have no bounds checks.
- Other: `ssh` can reuse a nonce (no automatic re-key); `fs_rmdir_all`/`fs_walk` follow symlinks; temp files use predictable names; `initrd` unpacking allows `..` paths; `ecdsa` signing and `pqc` are not constant time.

**Randomness:** `random` is a global 31-bit LCG (`random_bool` alternates, `random_byte` repeats every 256), `uuid_v4` uses it unseeded so every run gives the same UUIDs, `ws` masks with it, and at least nine modules carry their own xorshift. One good seeded generator (and `crypto`'s OS randomness for keys and nonces) should replace them.

**Generics not used:** `collections`, `iter`, `vec`, `queue` and `Deque<T>` store `i64` only although the compiler has generics; `core`'s `Vec::sort` compares every element as an `i64` (a `Vec<str>` sorts by address, `Vec<f64>` wrongly); empty `iter_min`/`HashMap.get`/`queue` return 0 instead of an `Option`.

**The same algorithm written several times:** SHA-1 in kdf, mysql, matchmaking, webrtc and ws; SHA-256 again in mysql; CRC-32 in about eight modules besides `crc`; an AES inverse cipher in lorawan and jose; DEFLATE in middleware besides `compress`. Name clashes: `scada` redefines `modbus_crc16`/`modbus_tcp_frame`; `skel` and `sprite` both define `Animation`; `env` and `os` both define `os_getenv`; `result` and `rc` duplicate core's `Result` and counted `Box`.

**Runtime pieces that do not exist or are not linked:** `pledge_asset.h` and `pledge_serialize.h` exist nowhere (the C generator turns their externs into functions returning 0), `d3d12`'s `fides_com` shim exists only in a comment, `audio` and `sdl` input's C files are never linked, `mmap` and `quant` builtins exist only in the C runtime, and `p_region_free` (used by `arena`) does nothing.

**Platform detection:** `os`, `sys` and `env` read the platform from environment variables and fall back to "linux": `sys_is_windows` is false on Windows, `sys_is_macos` never true, `sys_arch` is always "x86_64", path separators are hard-coded.

**Tests:** 121 modules have no test file of their own. Two tests lock bugs in (`can`'s reversed J1939 rule, `ota`'s fake checksum). The untracked `tests/bootstrap/test_fs_stream.fd` expects a streaming `File` and `fs_modified_ms` that `fs.fd` does not have, so it cannot compile yet.

**Compiler bug found:** a struct literal that names a field twice and leaves another out type-checks and runs (`mesh.fd` cube and plane faces set `v1` twice and never set `v2`).


## All modules

| module | lines | verdict | priority | effort | tests | what is missing (short) |
|---|---|---|---|---|---|---|
| `collections` | 1345 | BROKEN | P1 | L | thin | HashSet treats 0/-1 as empty/tombstone (contains(0) true, insert(0) lost); leaks; i64-only, fake Deque<T>; O(n) push_front |
| `hash` | 203 | BROKEN | P1 | S | thin (hash_str equality only, test_new_stdlib) | hash_djb2/hash_djb2_bytes panic on integer overflow for inputs of about 11+ bytes (confirmed by a run); hash_combine is XOR, so hash_vec ignores order and cancels pairs; no 64-bit or keyed (SipHash) hash, no streaming hasher |
| `uuid` | 256 | BROKEN | P1 | M | thin (test_new_stdlib 4 asserts + doc examples) | v4 comes from the global LCG seeded with 1: every process makes the same UUID sequence; no v1/v3/v5/v7; lax parser, no Option |
| `arena` | 309 | STUB | P1 | M | thin | region_* allocate untracked and region_free is a C no-op (memory never freed); arena_used/region_count return 0; one global arena |
| `db` | 260 | STUB | P1 | M | thin | MemoryDB is a key-value store, not SQL; helpers make injectable or wrong SQL; query_scalar fake |
| `parallel` | 170 | STUB | P1 | M | thin | par_map ignores its "function" and returns squares; only 2-way sum/dot; fill/range/hint sequential or no-op |
| `string` | 849 | STUB | P1 | M | thin (doc examples; str_join/int_to_hex used indirectly by 40+ tests) | Rope is fake (concat drops children, to_str gives ""); SmallStr never inline; int_to_hex/int_to_bin give "" for negatives; join is O(n^2); no test of its own |
| `sys` | 450 | STUB | P1 | S | thin (invariants only) | sys_arch hard-coded "x86_64"; endianness always little; sys_is_windows false on Windows and sys_is_macos never true (os_name); no hostname/OS version/uptime |
| `env` | 75 | THIN | P1 | M | none (doctests) | no set/unset/list, unset vs empty conflated, a stale "native stub" comment, duplicates os.fd |
| `iter` | 440 | THIN | P1 | M | thin (sum/min/max/product/range in test_new_stdlib; test_iter.fd tests the language's for, not this module) | i64 only although generics exist; no zip/enumerate/fold to another type/sort_by/rev/windows/min_by/max_by/position/flat_map; min/max of an empty Vec give 0, not Option; eager Vec copies only |
| `os` | 255 | THIN | P1 | M | thin | system info is env-var guesses (os_name "linux" default, cwd from $PWD, hostname from $HOSTNAME); Unix-only separators; no setenv/chdir/pid |
| `queue` | 125 | THIN | P1 | S | thin | `Queue<T>` holds i64 only; dequeue/peek return 0 when empty (no Option); no iteration |
| `sqlite` | 328 | THIN | P1 | M | thin (test_sqlite, test_sqlite_params, test_pe_sqlite; need libsqlite3) | panics on every error; binds text only; BLOB reads as null; no typed rows, no reusable prepared statements; step errors silently cut results; header says it implements db's `Database` cap, but it does not |
| `typed_strings` | 337 | THIN (has security bugs) | P1 | M | thin | sql_bind can be injected: it does not quote the value and it replaces every `?`. Any str can be wrapped. json_escape leaves most control characters. path_safe accepts absolute paths |
| `core` | 7289 | SOLID-GAPS | P1 | L | thin (no own test) | Vec::sort compares as i64 (str by address, f64 by bits); str_parse_int panics on long input, accepts junk; no fallible parse; get() -> 0 on miss |
| `derive_json` | 286 | SOLID-GAPS | P1 | M | good (frontend test) | Vec/Option/enum/map fields silently default; no surrogate pairs; int accepts "1.5"; no range check |
| `dns` | 528 | SOLID-GAPS | P1 | M | thin | shell injection in dns_resolve_system; fixed query id, reply not matched; A/AAAA only; no TCP fallback |
| `fmt` | 345 | SOLID-GAPS | P1 | M | thin | fmt_float truncates (no rounding); hex/bin/octal of negatives give ""; header's fmt_table/fmt_pad don't exist; no scientific/significant-digit or NaN/inf formatting |
| `fs` | 1090 | SOLID-GAPS | P1 | M | good (test_fs, test_fs_handles, test_lib_fs*, world tests) | fs_rmdir_all and fs_walk follow symlinks (rm -rf through a link deletes the target's contents; a link cycle recurses forever); temp files not created exclusively (check-then-write race, predictable names); no stat/mtime/permissions/symlink API; read errors are "" like an empty file; File loads the whole file and rewrites the whole file on every flush that holds a NUL; untracked test_fs_stream.fd calls fs_modified_ms and a streaming File that fs.fd lacks |
| `http` | 1024 | SOLID-GAPS | P1 | M | good (server core); legacy router/arena/middleware untested | fake arena + pass-through middleware + exact-match "router" (superseded by router.fd/middleware.fd); unbounded chunk-size parse (overflow panic = remote crash); no query/url-decode helpers; no 100-continue, no streamed/chunked responses; short status table |
| `https` | 185 | SOLID-GAPS | P1 | M | none | no test at all; 303 keeps POST; Host header drops non-default port; own URL splitter breaks on `?` without path, IPv6, userinfo; no timeout/redirect knobs; redirects re-send body |
| `json` | 1731 | SOLID-GAPS | P1 | M | good | validation path has no depth limit (stack overflow on hostile input); every string accessor re-validates the whole text; quadratic json_string/unescape/builders; no f64 builder |
| `math` | 709 | SOLID-GAPS | P1 | M | thin | perspective() uses 1/sin instead of 1/tan; look_at() rotation transposed; no tan/atan/atan2/asin/acos/round/trunc/fmod/hypot (5 modules reimplement them); slerp is nlerp; projections untested |
| `quic_tls` | 1968 | SOLID-GAPS | P1 | L | indirect, good for the handshake (quic_conn, quic_more, http3; C only) | no server authentication without a pin: no chain/CA/hostname check, RSA/ECDSA CertificateVerify accepted unchecked; Ed25519-only server certs; X25519 only; no 0-RTT anti-replay |
| `random` | 242 | SOLID-GAPS | P1 | M | thin (2 calls in test_new_stdlib, seeding in test_stdlib_test_clock) | 31-bit LCG: random_bool strictly alternates, random_byte has period 256; random_float can return 1.0; modulo bias, nothing above 2^31; one global state (RngState unused), so 9 modules carry their own xorshift; no gaussian or u64 |
| `sort` | 372 | SOLID-GAPS | P1 | S | thin (small Vecs in test_new_stdlib) | last-element-pivot recursive quicksort: O(n^2) and recursion depth n on sorted input (stack overflow on large Vecs), while core's Vec_sort is a heapsort; no stable sort, no sort_by_key; i64 only |
| `sync` | 499 | SOLID-GAPS | P1 | L | good for mutex/atomic/tls/channel; no concurrent-reader test | RwLock serializes readers (a mutex in disguise); no condvar, semaphore, once, barrier, try_lock; handles never freed |
| `vec` | 431 | SOLID-GAPS | P1 | M | thin | every helper is typed `Vec<i64>`; chunk/window return indices; zip/enumerate return flat Vecs; `vec_chunk(v, 0)` never ends; `find` returns -1 instead of Option; O(n^2) sort_by; the adapters are tested only by doc examples |
| `world` | 965 | SOLID-GAPS | P1 | S-M | good (10 files), but test_world_sockets cannot pass | the socket fake is not connected: stdlib/net.fd never calls world_sock_*; binary files cannot be faked; no listen/accept or IPv6 in the socket fake |
| `arinc` | 533 | BROKEN | P2 | M | good | queuing port receive never dequeues (port fills forever); priority, size and direction ignored; no APEX buffers/semaphores/process/time services |
| `bits` | 350 | BROKEN | P2 | S | none | rotations wrong for negative values (arithmetic >>); bit_highest_set loops forever on negative input; no bswap/parity |
| `can` | 556 | BROKEN | P2 | M | good (asserts wrong behaviour) | J1939 PDU1/PDU2 and PGN reversed; no ISO-TP so UDS/OBD frames are wrong on a real bus; wrong ECU request ids; no validation |
| `clinical` | 404 | BROKEN | P2 | M | thin | NEWS2 bands wrong (SBP, temperature, SpO2 scale 2), O2 and consciousness terms missing; promised Framingham/ASCVD absent |
| `font` | 372 | BROKEN | P2 | M | thin | TrueType path can never work: C returns a pointer on success, Fides treats non-zero as failure; glyph out-params passed as 0; metrics hard-coded; "bitmap font" draws a box for every character |
| `gl` | 393 | BROKEN | P2 | M | thin | gl_gen_*/gl_delete_* pass NULL to glGen*/glDelete* (always return 0, never delete); GL 3.3 entry points used without a loader (not exported by opengl32 on Windows); glBlendFunc and shader logs missing |
| `mesh` | 480 | BROKEN | P2 | M | thin (smoke) | cube/plane face literals set v1 twice and never v2 (faces 0-2-0 etc., zero normals, confirmed by run); OBJ loader takes the first 3 vertices of an n-gon, no negative indices, splits on single spaces |
| `sdl` | 400 | BROKEN | P2 | M | thin (Fides-side state tracker only) | pledge_input.c assumes Vecs inline in SdlInputState (wrong layout) and the runtime C files are not linked by fid ("still stubs", release-roadmap.md:468); no rect builder, so fill_rect is unusable; no event queue |
| `serialize` | 418 | BROKEN | P2 | M | thin (JSON writer only, test_stdlib_repaired) | pledge_serialize.h/.c do not exist, so f64 and save/load cannot build; save passes a null buffer, load returns zeros; JSON writer does no escaping; the "JSON reader" is a substring search |
| `asset` | 364 | STUB | P2 | M | thin | texture/sound/font/mesh go to a pledge_asset runtime that does not exist (stubs return 0); text content discarded; hot reload reloads everything |
| `audio` | 221 | STUB | P2 | M | none | all calls reach pledge_audio_* only if the header is in the build dir and nothing links the C file: no-ops in practice |
| `audio_mixer` | 321 | STUB | P2 | M | thin | promised music streaming/fades and panning absent; each play sets the volume of all channels; stop_group stops everything |
| `data` | 306 | STUB | P2 | S (fold into csv/json) / M | none | self-declared placeholders; infer_shapes never matches an op; CSV parser mishandles "" and multi-line |
| `ecs` | 204 | STUB | P2 | M | thin | register/run systems and dispatch are no-ops; destroy/reset panic "not implemented" |
| `events` | 47 | STUB | P2 | M | thin | events_dispatch is an empty body; no queue, no registration |
| `game` | 228 | STUB | P2 | L | thin | render_*/compile_script/hot_reload are no-ops; load_* return a raw file address; input_* drop the key; game_loop can never stop |
| `grid` | 445 | STUB | P2 | M | thin (checks the fake wire format) | GOOSE is a made-up byte format with no decoder; stNum/sqNum backwards; most LNs/CDCs are names only; protection ignores its time delay; duplicates the real stdlib/iec61850.fd |
| `image` | 357 | STUB | P2 | M | good (pure part) | loaders return blank pixels (C helper never copies them); image_to_texture uploads no pixels; no decoder of its own (PNG/BMP/TGA), no save; only 3/4 channels |
| `input` | 437 | STUB | P2 | M | good (pure part) | input_update never polls the platform (pledge_input_* never called); no text/char events, no gamepad released; arrows+WASD sum to 2; GAMEPAD_* numbering disagrees with gamepad.fd |
| `medical` | 316 | STUB | P2 | L | thin | hard-coded timestamp/app/doctor, fake discharge PID, parser never reads MSH-9 (type always ADT^A01); no HL7 escaping/components/ACK; "FHIR resource helpers" are one name table |
| `ota` | 356 | STUB | P2 | M | good (enshrines the fake) | session verify compares an FNV-1a hash with a "sha256:" checksum; no signature check; diff has no data, no patch; apply/commit/rollback only set a state |
| `quant` | 47 | STUB | P2 | M | thin | header promises int8 storage, q4, VNNI/SDOT/GPU kernels; runtime stores f64, qmatmul is a plain f64 matmul ignoring scales; C backend only |
| `shader` | 342 | STUB | P2 | M | thin (constants and GLSL text) | mat4 uniforms uploaded from a null pointer (MVP never set); the location cache is never stored; blend, depth-write and front-cull are stored, never applied; no shader graph despite the header; the cache leaks programs |
| `skel` | 412 | STUB | P2 | M | thin (one keyframe check) | inverse bind matrices are the identity "for now"; skin_vertex math is not skinning; bones without keys reset to the origin; no channel split or glTF import; Animation name clashes with sprite |
| `streaming` | 283 | STUB | P2 | M | none | unload never happens (chunks stay loaded forever); last_used/LRU never updated; unload_count goes up again on every update; negative coordinates go to the wrong chunk; `streaming_get_loaded_chunks` is in the usage text but does not exist |
| `ui` | 352 | STUB | P2 | M | thin (test_lib_ui: context/begin/end only; widgets never run) | text drawing is a 3-pixel placeholder per char; text input does no editing; no font, layout, keyboard focus |
| `alloc` | 40 | THIN | P2 | S | thin | four wrappers only: no realloc/calloc/aligned; overflow panics instead of the documented 0 |
| `banking` | 314 | THIN | P2 | M | thin | withdraw/transfer allow overdraft and negative amounts, not @conserve though Money_take now allows it; no history |
| `bpe` | 186 | THIN | P2 | M | thin | linear merge-rank scan (unusable at 50k merges); no GPT-2 byte mapping/pre-tokenizer/special tokens; unknown -> id 0 silently |
| `chunk` | 407 | THIN | P2 | M | thin | promised greedy meshing and serialization absent; header API names do not exist; streamed chunks empty, unloads drop edits |
| `dp` | 32 | THIN | P2 | M | thin | 3 builtin wrappers; no Laplace, clipping, budget accounting, sigma from (eps, delta) |
| `ecu` | 295 | THIN | P2 | M | good | "state machine" is names only; no DTC memory, no calibration maps; basic PID duplicating pid.fd |
| `editor` | 373 | THIN | P2 | M | good | no undo/redo (max_undo unused), no hierarchy, picking, multi-select, save/load; id+10000 duplicates |
| `elf` | 149 | THIN | P2 | S-M | thin | no bounds checks; no symbols, relocations, notes, section lookup by name |
| `fdt` | 201 | THIN | P2 | M | thin | no path lookup, child iteration, phandles, reg decoding, memory reservations; few bounds checks |
| `navmesh` | 431 | THIN | P2 | M | thin | polygon navmesh is only centers + manual links (no polygons, no build from geometry, no locate, no funnel); grid A* scans every cell per step, coords packed x*10000+y, heuristic scale 1/10 of costs |
| `ocpp` | 358 | THIN | P2 | M | thin | ~10 of 28 OCPP 1.6 messages, 1 of 2.0.1; no payload readers; central system answers MeterValues/StatusNotification "NotImplemented"; one connector |
| `openadr` | 361 | THIN | P2 | M | thin | no oadrCreatePartyRegistration (which ven_next_request asks for), no oadrUpdateReport; one signal per event, one event per payload; no ISO 8601 duration handling |
| `plot` | 345 | THIN | P2 | M | thin | no axis ticks or values, one series, fixed size/colours, no legend; title/labels not XML-escaped |
| `rc` | 130 | THIN | P2 | M | thin (test_rc_arc) | counts beside an i64, never owns or drops the value; a double drop frees twice; Rc is atomic too; no Weak, no generics; core's counted Box already covers this |
| `result` | 241 | THIN | P2 | S | thin (5 asserts in test_new_stdlib) | i64-only legacy duplicate of core's generic Result<T,E>; wrong panic message in unwrap_err; expect ignores msg in the panic |
| `scada` | 398 | THIN | P2 | S | good for what exists (test_lib_scada) | superseded by modbus.fd: FC 02/04/0F builders promised but missing; no CRC check on parse; no server; modbus_crc16 and modbus_tcp_frame clash by name with modbus.fd |
| `set` | 87 | THIN | P2 | S | none (no importer anywhere) | str-only wrappers over HashMap; no iteration, to_vec, union, intersection or difference; no `module` line |
| `slab` | 113 | THIN | P2 | M | thin (5 checks in test_os_stdlib.fd, no test_std_slab) | no double-free or range checks; byte store is a `Vec<i64>` with one element per byte; no typed object access; no size classes or caches, no growth; no real-memory backend despite the "kernel memory" claim |
| `stack` | 121 | THIN | P2 | S | thin (test_lib_stack) | Stack<T> is i64-only (T unused); pop/peek return 0 when empty instead of Option |
| `storage` | 344 | THIN | P2 | M | good for what exists | Merkle tree hashed with FNV-1a, not SHA-256 (no tamper resistance); no inclusion proofs; no leaf/node domain separation; rebuild duplicates nodes; KV store linear, i64 only, -1 sentinel, no persistence |
| `sunspec` | 319 | THIN | P2 | M | thin | the meter (203) offsets do not match the SunSpec map. Its "not implemented" values are read as data. No bounds checks. Only models 1/101-103/203, read-only |
| `swift` | 384 | THIN | P2 | L | good for what exists | no IBAN mod-97 check (the comment promises one); :32A: amount written as raw cents; no block 3/5, no message parser, only an MT103 builder of 12 named types; BIC check lax |
| `template` | 212 | THIN | P2 | M | thin (one render line in test_stdlib_repaired + doc examples) | no if/for (documented "not yet implemented"); `default` filter is a no-op; no HTML escaping; no dotted paths; comptime_template is a placeholder |
| `tensor` | 161 | THIN | P2 | M-L | good (for what exists) | from_vec takes only integers. No div/exp/log, broadcasting, axis reductions, slicing or concat. Generic names collide |
| `virtio` | 187 | THIN | P2 | M | none | only split-ring offsets plus avail push / used read; no device status or feature negotiation, no ring flags or event_idx, no indirect tables, no packed ring, no descriptor readers; no test |
| `animation2d` | 237 | SOLID-GAPS | P2 | S | thin | update advances one frame max per call; source_rect ignores row wrap; no real state machine/transitions/events |
| `args` | 444 | SOLID-GAPS | P2 | M | thin | no combined/attached short options, unknown-option errors, help generation or subcommands |
| `async` | 147 | SOLID-GAPS | P2 | M | none | io_wait/io_poll wait on the wrong condition / first fd only; spawn passes one arg; no real poll set; untested |
| `atomic` | 105 | SOLID-GAPS | P2 | S | thin | no fetch_and/or/xor/max/min, no observed-value CAS, no AtomicBool/ptr, atomics never freed |
| `bigint` | 735 | SOLID-GAPS | P2 | M | thin | silent junk-skipping parse, i64::MIN overflow, negative modpow residue; no gcd/modinv/bytes/bit ops; slow decimal limbs |
| `buffer` | 364 | SOLID-GAPS | P2 | M | thin | read_string_lp/read_remaining_string do not advance the caller's cursor; -1 sentinel ambiguous; no signed/float/varint reads |
| `camera` | 321 | SOLID-GAPS | P2 | M | thin | 2D rotation ignored; 3D screen_to_ray wrong when pitched; no inverse/world_to_screen/frustum; shake uses wall clock |
| `cobol` | 671 | SOLID-GAPS | P2 | M | thin | no EBCDIC; REDEFINES and group OCCURS silently give wrong layouts; no ODO/SIGN SEPARATE/COMP-1/2 |
| `collision_broad` | 324 | SOLID-GAPS | P2 | M | thin | quadratic dedup with a key that collides at id 1e6; SAP on x only; stale sap_update; no remove; 2D only |
| `compress` | 1536 | SOLID-GAPS | P2 | M | thin | decompressors return partial output on corrupt input and never verify Adler-32/CRC-32; default compress functions do not compress |
| `crypto` | 528 | SOLID-GAPS | P2 | S | good | SHA-256/HMAC/PBKDF2 only, no incremental hasher; header's "only approved algorithms here" contradicts sha512/kdf/aes_gcm modules |
| `crypto_bind_unix` | 961 | SOLID-GAPS | P2 | S | none in suite | only checked by scripts/verify (needs libcrypto); GCM failure vs empty plaintext ambiguous; deprecated EC_KEY |
| `d3d12` | 1055 | SOLID-GAPS | P2 | M | good (no GPU) | needs C shim "fides_com" that is not shipped (C sweep NOBUILD); no device/swapchain/fence/PSO flow |
| `datetime` | 1805 | SOLID-GAPS | P2 | M | good | seconds only (fractions dropped); dur_parse has no ms/us; no RFC 2822 / HTTP-date parsing |
| `dds` | 1099 | SOLID-GAPS | P2 | L | good | no SEDP endpoint discovery, no DATA_FRAG, no CDR serializer, no QoS: cannot interoperate with ROS 2 |
| `decimal` | 393 | SOLID-GAPS | P2 | S | thin | dec_parse returns 0 on garbage, ignores trailing junk; no Result parse, f64 conversion, HALF_DOWN, rem/pow |
| `dlms` | 624 | SOLID-GAPS | P2 | L | good | GET only: no SET/ACTION, block transfer, HLS auth or GCM ciphering (most meters need them) |
| `dnp3` | 662 | SOLID-GAPS | P2 | M | good | no event classes 1-3/unsolicited, time sync, analog output/float groups, Secure Authentication |
| `ecdsa` | 844 | SOLID-GAPS | P2 | M | good | signing is not constant time (stated); fine for verify |
| `encoding` | 804 | SOLID-GAPS | P2 | S | good | no base64url/base32 (re-implemented in http2/iap); utf8_decode accepts overlongs/surrogates; no strict decode |
| `flowfield` | 320 | SOLID-GAPS | P2 | M | thin | flowfield_add_cost is fake (edits distances after the fact); 4-neighbour BFS only, no cost field |
| `gamepad` | 746 | SOLID-GAPS | P2 | S | good | unconditional `link "SDL2"` makes the pure module unbuildable without SDL2 (C sweep NOBUILD); no mapping database (lookup by GUID/platform, SDL_GameControllerDB file); no per-frame pressed/released; no sensors/touchpad |
| `gltf` | 435 | SOLID-GAPS | P2 | M | good | no bounds checks on accessor reads; signed-type uint read off by 256/65536; one buffer only (bufferView.buffer ignored), no data: URIs; no nodes/transforms/scenes; triangles only; no `module` line |
| `graphql` | 1513 | SOLID-GAPS | P2 | L | good (parse, SDL, exec, fragments, unions, introspection basics) | no validation phase (§5); null does not propagate to the nearest nullable parent; no variable/argument input coercion or scalar result coercion; subscriptions run on the query root; `extend type` duplicates; introspection partial |
| `grpc` | 868 | SOLID-GAPS | P2 | M | good (framing, deframer, unary round trips, gzip, errors, CONTINUATION) | streaming RPCs (server/client/bidi) not driven; no connection driver over http2's H2Conn; `:scheme` fixed to http; no `-bin` metadata base64 helper; only gzip/identity |
| `hpke` | 294 | SOLID-GAPS | P2 | M | good for base mode (RFC 9180 A.2.1); psk/auth only round-tripped | one ciphersuite only (no AES-GCM, P-256, SHA-384/512); no AuthPSK setup functions; no export-only mode; no sequence limit |
| `html` | 662 | SOLID-GAPS | P2 | M | good (builder, parser, queries); script render untested | html_render escapes script/style text (corrupts them); 19 named entities only, no numeric-ref range checks; no html/head/body insertion or misnested-formatting recovery; no CSS-selector query; O(n^2) string building |
| `http2` | 2761 | SOLID-GAPS | P2 | M | good (5 suite tests, ~1100 lines, + tests/net/http2_live.fd) | no response trailers (client drops them, server cannot send); client ignores peer MAX_CONCURRENT_STREAMS; whole-body buffering only, no streaming; one global handler per process; fixed timeouts/limits |
| `http3_udp` | 204 | SOLID-GAPS | P2 | M | live only (tests/net/http3_udp_loopback.fd, http3_public.fd, http3_curl_server.fd; none in suite) | client cannot authenticate a public server (quic_tls has no CA store/chain check: any RSA/ECDSA server accepted); IP text only, no host-name resolution |
| `http_client` | 333 | SOLID-GAPS | P2 | M | thin (header lookup, URL refusal; parsing only via world fakes) | no TLS (header says tls.fd "not yet available": stale, `https` exists); no timeouts, redirects, keep-alive or error text; reads socket a byte at a time; duplicates https.fetch |
| `i18n` | 574 | SOLID-GAPS | P2 | M | good | no ordinal plurals (selectordinal), no ICU quoting/offset; no currency/percent/time formatting; no catalog loading (PO/JSON); ~17 languages only |
| `iec104` | 489 | SOLID-GAPS | P2 | M | thin | no typed object decoders (the header promises them); no N(S) sequence check; per-frame t1 tracking missing; only 5 of the common type ids; SQ=1 and quality bits not handled |
| `iec61850` | 543 | SOLID-GAPS | P2 | M | thin | BER decoder has no bounds checks (a malformed GOOSE frame panics); INTEGER encoding wrong for values with the top bit set and for negatives; no typed allData decoding; SV is single-ASDU with no 9-2LE sample layout or 0x88BA frame |
| `imap` | 1267 | SOLID-GAPS | P2 | M | good (RFC 3501 examples, scripted session) | header promises LIST/COPY/EXPUNGE/NOOP builders that do not exist; no LIST/STATUS parsing, APPEND, CREATE/DELETE/RENAME, MOVE, CLOSE, AUTHENTICATE, STARTTLS; no modified UTF-7 mailbox names; quadratic re-parse of the input buffer |
| `infer` | 245 | SOLID-GAPS | P2 | M | thin (test_ai_stdlib: matvec, rmsnorm, softmax, argmax, top_k, attn, rope; sampling untested) | no SiLU/GELU, LayerNorm, embedding lookup, residual/elementwise ops; attention is single-head, no causal mask, no multi-head/GQA batch form; no temperature/repetition penalty; RoPE only in the interleaved convention |
| `ini` | 298 | SOLID-GAPS | P2 | S | none | no test; comments and empty sections lost on round trip; no escapes or line continuations; no remove; values with a `"` serialize to broken quoting; junk lines silently skipped; ini_get_float takes non-numbers |
| `io` | 799 | SOLID-GAPS | P2 | M | good for streams (test_io_streams, test_io_stdin); io_* helpers untested | no Read/Write interface (Reader/BufReader/Writer/BufWriter are separate structs with duplicated code, nothing generic over a source); no seek or positional read, files read whole at open; stdout/stderr Writers stop at a NUL; each file write with a NUL rereads and rewrites the file; io_readln returns "" at EOF (same as an empty line) |
| `iso15118` | 1541 | SOLID-GAPS | P2 | L | good (DIN) | DIN 70121 is complete; ISO 15118-2 messages have no codec (seam); the ISO-2 state order lacks Authorization, CableCheck/PreCharge/Welding and AC ChargingStatus; one timeout for every state; EV side only |
| `jni` | 870 | SOLID-GAPS | P2 | M | good (fake VM) | no wrappers for field get/set, arrays (New/Get/Set<T>ArrayRegion), NewObject, RegisterNatives, GetObjectClass, JavaVM AttachCurrentThread/GetEnv, CreateJavaVM (externs declared, unused); no f32-bits-to-f64 for float returns; signature mismatch returns 0 silently; PopLocalFrame leaves local_refs counted |
| `jsonschema` | 556 | SOLID-GAPS | P2 | M | thin | no dependentRequired/dependentSchemas/unevaluated*; `$ref` cycles recurse forever; every keyword re-validates schema text (string JSON API) |
| `log` | 491 | SOLID-GAPS | P2 | S | good | no custom sink/callback, no rotation, second-precision timestamps, logfmt keys not escaped, quadratic line building |
| `lwm2m` | 1136 | SOLID-GAPS | P2 | M | good | no Observe/Notify or Write-Attributes (Information Reporting), no Create; DTLS/bootstrap out of scope |
| `markdown` | 542 | SOLID-GAPS | P2 | M | none | `[x](javascript:...)` emitted as-is despite the "untrusted Markdown cannot inject" claim; no tables/nested lists/ref links/setext/indented code/hard breaks; quadratic concat |
| `matter` | 1299 | SOLID-GAPS | P2 | L | good | Interaction Model has Read only: no Write, Invoke (commands), Subscribe; no PASE/CASE/sessions (documented) |
| `mavlink` | 902 | SOLID-GAPS | P2 | M | good | no array fields (char[16], float[4]) so STATUSTEXT/PARAM_*/MISSION_* cannot be defined; 7 common messages only |
| `metrics` | 235 | SOLID-GAPS | P2 | M | thin | no labels; integer values only (no float gauges/seconds); HELP not escaped; one global served set |
| `middleware` | 1014 | SOLID-GAPS | P2 | S | good | rate-limit buckets: linear scan, never evicted (one per distinct header value); own DEFLATE duplicates compress.deflate_compress |
| `mmap` | 181 | SOLID-GAPS | P2 | M | none | p_mmap_* exist only in the C runtime (fid run / emit cannot run it); no offset/length or read-only maps; byte-at-a-time volatile copies |
| `mongodb` | 2001 | SOLID-GAPS | P2 | M | good | no TLS transport, no OP_COMPRESSED, no sessions/transactions, single server |
| `mysql` | 1620 | SOLID-GAPS | P2 | M | good | no TLS (CLIENT_SSL), no multi-resultsets, no compression; private SHA-1/SHA-256 copies |
| `net` | 706 | SOLID-GAPS | P2 | M | good | IPv4 literals only (no DNS), no listen/accept, reliable layer has fixed 200 ms RTO, no retry limit, no ordering; net_set_reuseaddr is a no-op |
| `opcua` | 3053 | SOLID-GAPS | P2 | L | good | only the Read service: no Browse, Write, Call, subscriptions/monitored items, GetEndpoints; one security policy |
| `os_sandbox` | 616 | SOLID-GAPS | P2 | M | thin | builds seccomp/Landlock/Job bytes but nothing applies them (the "seam" is only a comment); short syscall tables; no macOS |
| `particles` | 269 | SOLID-GAPS | P2 | S | thin | randomness from the ms clock: a burst emits identical particles; render function in header does not exist; 2D only |
| `path` | 409 | SOLID-GAPS | P2 | S | thin | no relative-to/strip-prefix/starts-with/with-file-name; path_join("a","/b") gives "a/b" against its own comment |
| `pci` | 167 | SOLID-GAPS | P2 | M | thin | type-0 header only (no bridge/type-1), no PCIe extended capabilities (0x100+), no MSI/MSI-X decode, no subsystem/interrupt fields |
| `pem` | 381 | SOLID-GAPS | P2 | M | thin | encrypted PKCS#8 (PBES2) and PKCS#12 only detected, never decrypted; no standalone PKCS#1 "RSA PRIVATE KEY" / SEC1 "EC PRIVATE KEY" |
| `physics` | 546 | SOLID-GAPS | P2 | M | thin | no world/step, no friction, restitution ignored except elastic; elastic collision does not skip separating bodies and divides by a zero mass |
| `physics3d` | 489 | SOLID-GAPS | P2 | M | good | no rotation (no inertia/angular velocity), box-plane contacts missing, AABB only despite "SAT"; colliders stale after positional correction |
| `pmm` | 178 | SOLID-GAPS | P2 | S | thin | pmm_alloc returns -1 with free frames below the hint; no contiguous/aligned allocation; no NX/accessed/dirty/global flags; no bounds checks |
| `postgres` | 1344 | SOLID-GAPS | P2 | M | good | no COPY, LISTEN/NOTIFY delivery, cancel request, binary format, named prepared statements; only the last result set of a multi-statement query; bytes after ReadyForQuery dropped |
| `pqc` | 1435 | SOLID-GAPS | P2 | M | good | ML-KEM-768 only (no 512/1024), no ML-DSA-87, no HashML-DSA, no SLH-DSA; not constant time |
| `privacy` | 91 | SOLID-GAPS | P2 | S | thin | consent key is hash(subject) XOR hash(purpose): symmetric and collision-prone, so consent can match the wrong pair; test struct PrivCounter in the stdlib |
| `profiler` | 405 | SOLID-GAPS | P2 | S | good | scopes timed with a millisecond clock (sub-ms scopes read 0); no "GPU timing" despite header; no aggregation of repeated scopes |
| `protobuf` | 334 | SOLID-GAPS | P2 | S | thin | no float (fixed32 as f32), no sfixed32 sign extension, packed varints only; string read via char_to_str per byte (non-ASCII unverified) |
| `regex` | 1489 | SOLID-GAPS | P2 | M | good (8 files, comptime too) | no reusable compiled Regex (every call recompiles); no lazy `*?`, named groups, (?m)/(?s); bytes only (no UTF-8); no \xHH; bad patterns fail silently; open overflow bug in `{n}`; quadratic find_all/replace with top-level ` |
| `replay` | 439 | SOLID-GAPS | P2 | M | thin (one frame round-trip) | only 4 keys a frame kept; seed and all fields written as 32 bits (64-bit seeds lose determinism); name, version and time never saved; version unchecked; 3600-frame default cap silently stops recording |
| `rsa` | 938 | SOLID-GAPS | P2 | M | thin-good (one 768-bit OpenSSL key, SHA-256 only) | no SHA-384/224 (x509.fd re-implements verify for it); no blinding or fault check on private ops; weak key validation; no RSAES-PKCS1-v1_5 |
| `rtos` | 616 | SOLID-GAPS | P2 | M | good (scenarios) | timers have no callback (they only log strings); no task delete or priority change; a blocked task cannot be suspended; logs grow without bound in a "static-allocation" RTOS; the header names RT_BLOCKED but the code returns RT_PENDING; suspend/yield/recursive mutex untested |
| `safetensors` | 350 | SOLID-GAPS | P2 | M | thin (only inside test_ai_stdlib.fd) | no writer; whole file as Vec<i64>, no mmap; zero-size dims rejected; F4/F6/F8_E8M0/C64 fail; element reads not dtype- or bounds-checked; O(n^2) lookups |
| `sdcard` | 493 | SOLID-GAPS | P2 | M | good for what exists | `SdBus` is wired to the simulator only, so a real SPI transport cannot be plugged in; no multi-block CMD18/CMD25, no CSD/CID (no card size), no erase; a busy timeout is reported as success |
| `secrets` | 403 | SOLID-GAPS | P2 | S | good | silent empty blob when IV is not 12 bytes; salt passed through str; iteration count from the file unbounded; keychain seam has no backend and no remove wrapper; no zeroize of keys |
| `secure_store` | 479 | SOLID-GAPS | P2 | M | good | iterations 0 seals a file that can never be opened; generator lengths not checked (silent empty blob); SecItem externs declared but never called; no Keystore decrypt steps; save not atomic; duplicate HMAC/PBKDF2 |
| `semihosting` | 541 | SOLID-GAPS | P2 | M | good for what exists | the target side runs only against the host model: no pluggable trap, and memory is a Vec; host does not implement TMPNAM/RENAME/SYSTEM/HEAPINFO; no target wrappers for remove/rename/time/clock/istty/readc; no `:semihosting-features` file |
| `semver` | 600 | SOLID-GAPS | P2 | S | good (test_lib_semver) | `~1` and `^0.0` give wrong upper bounds; x-ranges (1.x, 1.2.*) match nothing; `>= 1.2.3` with a space fails; pre-release rule looser than npm; no increment helpers |
| `sensor_drivers` | 775 | SOLID-GAPS | P2 | M | good | VL53L0X has no init and no stop-variable read (stated in the header); BME280 has no reset, no settings API and no forced-mode status poll; init write failures are ignored; ina219_read turns bus errors into plausible values |
| `sha512` | 304 | SOLID-GAPS | P2 | S | good (FIPS vectors, padding edges, RFC 4231) | no SHA-384 / SHA-512/256 (SHA-384 is a copy of this compression in ecdsa.fd), no streaming API |
| `sigmatch` | 1045 | SOLID-GAPS | P2 | M | good | no uint8/16/32() reads, rule references, private/global, import; no regex modifiers, xor/base64; fullword is wrong for wide strings; negative meta ints fail; scans are naive |
| `signal` | 66 | SOLID-GAPS | P2 | S | thin (test_signal_flags) | Linux signal numbers on macOS (SIGUSR1 = SIGBUS there); no unregister, ignore or default; no SIGCHLD/SIGWINCH; polling only |
| `smp` | 374 | SOLID-GAPS | P2 | M | good | no trampoline machine code; the per-CPU area is a Vec model with no real-memory write path; topology handles x2APIC leaf shifts only |
| `smtp` | 944 | SOLID-GAPS | P2 | S | good | CR/LF not refused: header injection (Subject, custom headers) and SMTP command injection (addresses); no XOAUTH2/CRAM-MD5; no SMTPUTF8/BODY=8BITMIME; RFC 2231 filenames; no header folding |
| `spi` | 835 | SOLID-GAPS | P2 | S | good (bit-bang xfer untested) | the register helpers hard-code read bit 0x80 and address mask 0x7F; the 3WIRE/READY/DUAL/QUAD flags are never honoured (tx_nbits/rx_nbits never set); `spi_bitbang_xfer` is untested |
| `sql_builder` | 601 | SOLID-GAPS | P2 | M | good | no MySQL dialect (stdlib has mysql_db); no ON CONFLICT DO UPDATE, subqueries, UNION, CTE; JOIN ON is one equality only; a column with a space or '(' is copied raw (injection when user-chosen); no way to run SqlOut through sqlite/postgres |
| `srtp` | 713 | SOLID-GAPS | P2 | M | good (RFC 3711/7714 vectors, libsrtp packet) | AES-256 profiles; per-SSRC state made before authentication (memory DoS); index/ROC wrap without key-lifetime stop; no MKI, no RFC 6904 |
| `ssh` | 1183 | SOLID-GAPS | P2 | L | good (one in-memory client/server run) | one cipher suite and Ed25519 only (no RSA/ECDSA keys); no exit-status/pty/env/signal channel requests; no port forwarding or global requests; no key-file/authorized_keys parsing; no rekey limits |
| `state_machine` | 522 | SOLID-GAPS | P2 | L | good | no orthogonal regions, final states, completion transitions or local transitions; events carry no payload; global outbox shared across machines; ids not validated |
| `static_serve` | 679 | SOLID-GAPS | P2 | M | good | no ready FileStore (stdlib fs has no mtime); no glue to http/router; whole file read into a Vec<i64> (8 bytes per byte); no redirect from dir to dir/; no If-Match / If-Unmodified-Since |
| `syscall_abi` | 649 | SOLID-GAPS | P2 | M | good | small call table (no readv/writev/poll/epoll/rt_sig*/newfstatat/dup3/...); sockaddr and iovec pointers not validated; errno table partial |
| `task` | 426 | SOLID-GAPS | P2 | M | good core, thin scope/timeout | scope_join waits for every task in the process; tokens and scope flags never freed; join_timeout's -1 is ambiguous; 1 ms polling waits; scope, join_timeout and fd-cancellable untested |
| `tcpip` | 1450 | SOLID-GAPS | P2 | L | good (two stacks over an in-memory link) | no persist timer (zero-window deadlock), no congestion avoidance/ssthresh, no fast retransmit (dup_acks counted, unused), predictable ISN, closed conns never reclaimed, no ephemeral ports |
| `term` | 307 | SOLID-GAPS | P2 | M | none (doc example only) | nested styles reset each other; no Windows VT enable or isatty check; no size, raw mode or key input; width counts bytes, not columns; OSC not stripped |
| `terrain` | 365 | SOLID-GAPS | P2 | M | thin (flat terrain in test_stdlib_repaired + doc example) | header promises LOD and loading from a grayscale image: neither exists; "noise" is a position hash, not smooth noise; min/max heights go stale |
| `test` | 409 | SOLID-GAPS | P2 | M | good (snapshot untested) | approx_eq failure prints floats as ints; snapshot path is cwd-relative (doc says next to the test) and a mismatch shows no diff; asserts are i64-only; no assert_panics |
| `tiled` | 239 | SOLID-GAPS | P2 | M | good for CSV/array maps | base64 layers load empty (zlib and base64 exist in stdlib now); no tileset/firstgid API though the header promises it; no opacity; no infinite maps, group/image layers or polygon objects |
| `tilemap` | 411 | SOLID-GAPS | P2 | M | none | move_x/move_y snap bug (box ends inside the wall); negative pixel coords map to the wrong tile; solidity rules disagree between set and from_grid; no layers or tile properties; bad doc example |
| `time` | 536 | SOLID-GAPS | P2 | S | good | `time_new` accepts invalid dates; `time_diff_days` rounds toward zero; `clock_elapsed_*` uses the wall clock, not a monotonic one; the clock functions have no test |
| `timeseries` | 475 | SOLID-GAPS | P2 | S | good, but misses the boundary values | Gorilla: delta-of-delta of exactly 64, 256 or 2048 decodes as -64/-256/-2048 (silent corruption) |
| `tls13` | 249 | SOLID-GAPS | P2 | S-M | good | no SHA-384 suite, 0-RTT secrets, exporter function, ticket-to-PSK step or binder value. Key schedule only, no TCP handshake |
| `tls_crypto` | 553 | SOLID-GAPS | P2 | S-M | good (except x25519) | x25519 has no test and no caller, does not mask bit 255 and is not constant-time. Poly1305 is done with bigint. A failed decrypt looks the same as an empty plaintext. Duplicates chacha20poly1305.fd and x25519.fd |
| `tls_record` | 322 | SOLID-GAPS | P2 | M | thin | only TLS_CHACHA20_POLY1305 (no AES-128-GCM, the cipher suite TLS 1.3 requires); no splitting of records over 2^14 bytes when sending; ChangeCipherSpec body not checked; no RFC 8448 known-answer test |
| `tls_unix` | 555 | SOLID-GAPS | P2 | S | thin (same test) | tls_connect_unchecked leaks an SSL_CTX per call (:344); no error reason; no mTLS/custom CA/peer cert/resumption |
| `tls_windows` | 1125 | SOLID-GAPS | P2 | M | thin (test_tls13_local, C/native on Win11 only) | no close_notify on close; no error reason; no mTLS/peer cert/custom CA/resumption; fixed 30 s handshake timeout; socket-ownership docs contradict; second tls_server_alpn leaks |
| `tz` | 792 | SOLID-GAPS | P2 | M | good (test_tz, test_tz_julian, test_tzif vs zoneinfo/glibc) | on Windows (no zoneinfo) only a 50-zone table of 2026 rules, no history; unknown names silently give UTC |
| `udp` | 1026 | SOLID-GAPS | P2 | M | thin (addresses only in suite) | no connect, broadcast, multicast or reuseaddr; recv blocks the OS worker (no task_wait_read); an empty datagram looks like a timeout; truncation is silent |
| `unicode` | 788 | SOLID-GAPS | P2 | L | good | case mapping and character classes cover only a few scripts (and İ/ı are wrong). No case folding. Graphemes are approximate (no CRLF, no Hangul). Accepts invalid UTF-8. Header is stale |
| `units` | 733 | SOLID-GAPS | P2 | M | thin (42-line test + frontend dimension tests) | no electrical/angle/other derived units; operators and scale/lt/ratio on only some types; few non-SI conversions; no eq/to_str; no temperature-difference type |
| `unix_socket` | 131 | SOLID-GAPS | P2 | S | good (one-process round trip) | bind deletes any file at the path (:52); text-only I/O (no bytes/timeout/read_exact); no datagram sockets, no peer credentials |
| `url` | 540 | SOLID-GAPS | P2 | M | good | path decoded at parse, so %2F is lost on round-trip; no port range check and no error flag; normalize drops empty segments; resolve drops base userinfo; no repeated-key query API; no IDNA |
| `usb_device` | 533 | SOLID-GAPS | P2 | M | good (exact bytes, enumeration, CDC) | interface requests hard-wired to 2 interfaces/alt 0 (CDC-ACM only); one configuration; class dispatch only CDC; SET_LINE_CODING unchecked parity/stop -> panic in usb_line_coding_str |
| `usb_hid` | 550 | SOLID-GAPS | P2 | M | good | array fields not decoded to usages; no generic output/feature report encoder; usage page bound at the wrong moment; unit, exponent and physical items dropped; keypad, F-key and arrow keys have no ASCII |
| `vision` | 1222 | SOLID-GAPS | P2 | M | good | no resize, crop or pyramid (ORB is not scale-invariant); no RANSAC; no perspective warp of images; no Harris/Shi-Tomasi; no color ops |
| `vulkan` | 1239 | SOLID-GAPS | P2 | L | good for the pure part | layouts and builders for only about 14 structs (none for pipelines, command pool/buffer, descriptor pool/writes, sync, rendering info); no swapchain or surface entry points; draw/copy commands missing (indexed draw, buffer-to-image copy, push constants, viewport); no helper that picks the memory type |
| `webgpu` | 851 | SOLID-GAPS | P2 | M-L | good for the pure part | the extern seam cannot get a device (no RequestAdapter/RequestDevice), map a buffer, present to a surface, draw indexed, set a render-pass bind group or write a texture; no descriptor builders |
| `webrtc` | 2025 | SOLID-GAPS | P2 | L | good (428 lines, RFC 5769, two peers) | no SCTP data channels, TURN, peer-reflexive candidates, consent freshness, ICE restart or trickle ICE (documented out of scope); SHA-1, HMAC and CRC-32 copied in the module |
| `wireguard` | 706 | SOLID-GAPS | P2 | M | good (byte-exact against Node) | no cryptokey routing (AllowedIPs) or multi-peer device; no wg config or key helpers; timer-driven initiations put nsec 0 in their timestamp; no rekey jitter; an unconfirmed `next` keypair never expires |
| `ws` | 576 | SOLID-GAPS | P2 (security) | M | good for framing and loopback | the peer controls the frame length, so it controls an unbounded allocation; RFC 6455 MUST checks are missing (masking, RSV bits, opcode, control frame length, UTF-8, close codes); mask and key use a non-crypto RNG; loose handshake checks; no subprotocols, headers or timeouts |
| `x509` | 1116 | SOLID-GAPS | P2 | M-L | good (472 lines, OpenSSL chains) | no extendedKeyUsage; no path building (only the first anchor with a matching name is tried); IPv6 SAN text not normalized; byte-exact name compare; no OCSP, policies or AIA/CDP (documented) |
| `xml` | 2841 | SOLID-GAPS | P2 | M | good (parser, DTD, namespaces) | the tree loses the order of mixed content, and xml_to_str puts all text before the children; no comments, PIs or CDATA in the tree; no pretty printing or declaration output; UTF-8 only |
| `yaml` | 2110 | SOLID-GAPS | P2 | M | good (parse) | no emitter or serializer at all; keys containing "." collide in the flat path model; no API to list a mapping's keys; .inf and .nan unreadable through get_float; mappings inside flow sequences and tab indentation unsupported |
| `zigbee` | 918 | SOLID-GAPS | P2 | M-L | good for what exists | the radio seam takes no destination address; no Basic or Identify cluster; no reporting engine, and the server answers Configure Reporting with UNSUP; Level has Move-to-Level only; no ZDO; no APS ack or fragmentation |
| `zstd` | 2489 | SOLID-GAPS | P2 | L | good (decode and compress) | no dictionaries; no streaming API; the whole output in memory as Vec<i64> (8 bytes per byte); the compressor uses only the predefined FSE tables (low ratio) |
| `memo` | 56 | THIN | P3 | S | thin | i64 -> i64 only, linear scan lookup, no capacity/eviction, no module line |
| `shape` | 139 | THIN | P3 | S | good for what exists (test_shape, test_shape_verify) | "compile-time later" never done; usage's assert_shape does not exist; 2D matmul only, no broadcasting, batch or concat |
| `aarch64_sys` | 707 | SOLID-GAPS | P3 | S | good | no GICv3 distributor/ICC init sequence (header promises it); TTBR0/4K only |
| `acme` | 740 | SOLID-GAPS | P3 | S | good | no key rollover, no tls-alpn-01; CSR only for P-256/Ed25519 |
| `acpi` | 262 | SOLID-GAPS | P3 | S | thin | MADT walk hangs on a 0-length entry and has no bounds checks; no find-table-by-signature or FADT/HPET decoders |
| `ahci` | 693 | SOLID-GAPS | P3 | M | good | no NCQ issue path, ATAPI path or error recovery; polling only; one sector per BlockDev request |
| `android_ndk` | 656 | SOLID-GAPS | P3 | M | good | unconditional link "android" blocks host C builds; device-side seam (looper/input/activity glue) not written |
| `apic` | 499 | SOLID-GAPS | P3 | S | good | per-CPU and x2APIC NMI MADT entries ignored; calibration truncates |
| `apriltag` | 960 | SOLID-GAPS | P3 | M | good | tag36h11 only; global threshold; no pose refinement |
| `asn1` | 797 | SOLID-GAPS | P3 | S | good | children read as BER under asn1_read_der; >8-byte INTEGER truncated silently; no constructed-string reassembly |
| `audit_log` | 242 | SOLID-GAPS | P3 | S | good | line trimming corrupts data ending in spaces; non-32-byte key silently unkeyed; no incremental append/verify |
| `background` | 353 | SOLID-GAPS | P3 | M | good | device scheduling exists only as strings; no chaining/data/expedited work or desktop scheduler |
| `behavior_tree` | 943 | SOLID-GAPS | P3 | M | good | missing BT.CPP v4 nodes (Delay, RunOnce, KeepRunning, IfThenElse, Script, pre/post conditions) and SubTree remapping |
| `biometrics` | 328 | SOLID-GAPS | P3 | M | good | platform calls only as data; no CryptoObject-bound auth; lockout expiry not reset |
| `bitfield` | 160 | SOLID-GAPS | P3 | S | thin | width 64 computes 1 << 64; no range checks |
| `bitset` | 472 | SOLID-GAPS | P3 | S | good | "growable" but no resize; per-bit loops instead of word ops |
| `blake3` | 354 | SOLID-GAPS | P3 | S | good | one-shot only, no streaming hasher; header cites a missing test file |
| `ble` | 1202 | SOLID-GAPS | P3 | M | good | ATT Find By Type Value, Read Multiple, long writes unsupported; no GATT client procedures |
| `bms` | 388 | SOLID-GAPS | P3 | S | good | temperature unused; no SoP/limits/fault detection |
| `bsdiff` | 489 | SOLID-GAPS | P3 | M | good | old file capped at 1 MiB; own format only; no streaming apply |
| `cache` | 435 | SOLID-GAPS | P3 | S | good | string-only K/V; "" on miss; no clear, no LFU/TTL remove; entry-count capacity only |
| `channel` | 79 | SOLID-GAPS | P3 | S | good | documents CHAN_PENDING which does not exist; no try_send/timeouts; i64 payload only |
| `coap` | 1007 | SOLID-GAPS | P3 | S | good | no CoRE link format (/.well-known/core), no CoAP over TCP/WS |
| `complex` | 655 | SOLID-GAPS | P3 | S | good | no tan, inverse trig, hyperbolic (public) or log10 |
| `config` | 259 | SOLID-GAPS | P3 | S | good | floats with exponents rejected; panics instead of a Result API; no list/duration types |
| `crdt` | 616 | SOLID-GAPS | P3 | M | good | no LWW-Map/OR-Map/MV-register; tombstones never collected; O(n^2) RGA; merge drops orphan elements silently |
| `crypto_bind` | 827 | SOLID-GAPS | P3 | S | good | GCM decrypt failure vs empty plaintext ambiguous; P-256 only; no ChaCha20-Poly1305/HKDF/X25519 |
| `css` | 564 | SOLID-GAPS | P3 | M | thin | minifier mangles non-ASCII (char_to_str) and keeps spaces around combinators; no value shortening, no selector matching |
| `csv` | 707 | SOLID-GAPS | P3 | S | good | byte-at-a-time field concat in csv_parse_dialect; no BOM; QUOTE_NONE output not readable back |
| `diff` | 253 | SOLID-GAPS | P3 | M | none (doctest) | O(n*m) memory LCS (no Myers); no patch apply, no word/char diff |
| `dsp` | 719 | SOLID-GAPS | P3 | M | good | no higher-order IIR design (Butterworth/Chebyshev), resampling, Welch PSD, correlation |
| `dtls` | 1651 | SOLID-GAPS | P3 | L | good | (stated) no resumption, other suites, DTLS 1.3, CID; no chain validation (fine for WebRTC) |
| `elf_loader` | 711 | SOLID-GAPS | P3 | M | good | no TLS, IRELATIVE, COPY, R_X86_64_32/32S; PC32 overflow unchecked; unbounded memsz allocation |
| `entropy` | 456 | SOLID-GAPS | P3 | M | good | SP 800-22 subset (no spectral, approx. entropy, rank, ...); no SP 800-90B health tests (RCT/APT) |
| `exi` | 2296 | SOLID-GAPS | P3 | L | good | (stated) no Float/Decimal/date-time datatypes, compression, xsi:type, undeclared productions |
| `ext4` | 3652 | SOLID-GAPS | P3 | L | good | (stated) writes not journaled, no htree updates, no xattr writes, inline data/meta_bg refused |
| `fat32` | 2090 | SOLID-GAPS | P3 | M | good | every vfs write/truncate rereads and rewrites the whole file |
| `file` | 154 | SOLID-GAPS | P3 | S | good | no wrappers for p_file_read_at / p_file_append_vec / p_file_mtime_ms; file_write_raw == file_write |
| `forecast` | 733 | SOLID-GAPS | P3 | M | good (NIST SES table; numpy-reproduced AR/MA) | no prediction intervals (sigma2 computed, unused); no damped trend, seasonal ARIMA or order selection; short inputs panic |
| `framebuffer` | 665 | SOLID-GAPS | P3 | S | good | ANSI subset (no K, cursor moves, bold); no cursor; fb_copy_rect unclipped; channels wider than 8 bits break fb_chan_pack; byte-at-a-time volatile stores |
| `fuzz` | 644 | SOLID-GAPS | P3 | M | good | no corpus save/load to a directory; edge novelty only (no AFL hit-count buckets); no hang/timeout notion; not connected to `fid fuzz` (compiler/main.fd:250 runs its own random seeds); coverage and crash reporting are the target's job (documented seam) |
| `gameloop` | 181 | SOLID-GAPS | P3 | S | good | ms clock only, no injectable clock, no sleep/frame limiting; FPS counts capped time; header typos |
| `gcode` | 670 | SOLID-GAPS | P3 | M | good (checksum vector, arcs checked against a Python replica) | M codes ignored and not reported; G4 dwell time lost; no work offsets, canned cycles, tool/spindle/temperature state |
| `geolocation` | 481 | SOLID-GAPS | P3 | M | good (Vincenty, geohash and haversine reference vectors) | antimeridian and pole cases wrong (polygon test, Kalman filter); no Vincenty direct, cross-track distance, area or bbox |
| `goap` | 365 | SOLID-GAPS | P3 | S | good | heuristic can overestimate when one effect satisfies two goal conditions on the same fact; O(n^2) open list; cost > 0 not checked |
| `gpio` | 718 | SOLID-GAPS | P3 | M | good on the pure side; Linux cdev path untested | no line reconfigure, chip info or line info despite ioctl numbers; one config per request; blocking event read only; 64-line mask overflow |
| `haptics` | 410 | SOLID-GAPS | P3 | S | good | Android composition primitives are constants only (no builder); intensity curves ignored by the Android waveform; no sharpness curves |
| `historian` | 340 | SOLID-GAPS | P3 | S | good (swinging-door error bound, aggregates) | out-of-order samples silently dropped; no quality flags, step (digital) tags or persistence; description cannot be set |
| `hpet` | 440 | SOLID-GAPS | P3 | S | good (fake register file, ACPI table, mult/shift) | FSB/MSI routing not programmable; one-shot ignores 32-bit comparators and missed deadlines; pit_ch2_latch divides by zero above 1000 ms |
| `http3` | 1432 | SOLID-GAPS | P3 | S-M | good (test_stdlib_http3 571 lines over a lossy simulated net, qpack tests, live curl/public tests) | no cap on a request/response body (grows without limit); bodies only whole, no streaming; no handler-style server |
| `i2c` | 791 | SOLID-GAPS | P3 | S | good | no SMBus block read (count byte, I2C_M_RECV_LEN); errno lost on the Linux path; no I2C_FUNCS query; the Linux transport is untested (needs hardware) |
| `iap` | 653 | SOLID-GAPS | P3 | S | good | no JWSRenewalInfo decoding (grace end must be supplied by hand); Play PAUSED/PENDING subscription states map to expired; no history paging parse; a refunded consumable can never be bought again |
| `imu` | 339 | SOLID-GAPS | P3 | S | good | soft iron axis-aligned only (no ellipsoid fit); no initial attitude from accel+mag; mag_cal divides by zero on an unexcited axis; no Madgwick gyro-drift term |
| `initrd` | 455 | SOLID-GAPS | P3 | S | good | no compressed initramfs (gzip etc.) though compress.fd has inflate; ".." in entry names is not rejected (unpacking can escape `dest`: security); hard links copied, mtime/uid/gid not applied |
| `ipc` | 482 | SOLID-GAPS | P3 | S | good | receive does not check the thread's state (a thread blocked on send can also queue to receive); shmring capacity not checked to be a power of two <= 65536; no endpoint destroy, thread cancel or notification objects; a queued send still delivers after its cap was revoked |
| `jobs` | 819 | SOLID-GAPS | P3 | M | good | cron in UTC only (no time zone); no cancel/delete, lease heartbeat, unique keys; reserve is not atomic across processes although the header suggests sharing a backend; cron entries not persisted (jobs_load loses them); a long outage enqueues every missed occurrence (no misfire policy) |
| `jose` | 1164 | SOLID-GAPS | P3 | M | good | RS384/PS384, ECDH-ES+A*KW, PBES2, A*CBC-HS*, JSON serialization; fractional NumericDate rejected |
| `kdf` | 380 | SOLID-GAPS | P3 | S | good | PBKDF2 recomputes HMAC pads each iteration (~2x slower); iterations < 1 not rejected; no PHC hash/verify helper |
| `kv_cache` | 52 | SOLID-GAPS | P3 | S | good | no free/destroy (registry grows forever), separate K/V get, append to a bad layer silently ignored |
| `log_compact` | 366 | SOLID-GAPS | P3 | S | good | logtable_import reads without bounds checks (panics on a truncated table) |
| `lorawan` | 915 | SOLID-GAPS | P3 | M | good | EU868 only; no LoRaWAN 1.1 keys, class B/C, rejoin |
| `lp_solver` | 667 | SOLID-GAPS | P3 | S | good | variables are x >= 0 only (no free or lower-bounded variables) |
| `lqr` | 670 | SOLID-GAPS | P3 | S | thin | discrete only: no continuous LQR (CARE), finite horizon or cross term N |
| `mdns` | 831 | SOLID-GAPS | P3 | M | good | no probing/conflict resolution, no browse cache with TTL expiry, no NSEC |
| `metal` | 693 | SOLID-GAPS | P3 | M | good (pure part) | no render encoder/draw, no textures; leaks C strings and MTLSize blocks; dispatch broken on x86-64 Macs (documented) |
| `migrate` | 398 | SOLID-GAPS | P3 | S | good | no directory loader, no lock for concurrent deploys, no ready postgres/sqlite binding |
| `mime` | 330 | SOLID-GAPS | P3 | S | good | rebuilds both tables per lookup; mime_param splits inside quoted values; no sniffing/Accept matching |
| `modbus` | 773 | SOLID-GAPS | P3 | S | good | no ASCII framing, FC 22/43, no 32-bit int/float register helpers |
| `money` | 118 | SOLID-GAPS | P3 | S | good | Money_print wrong for negatives ("$0.0-5"); no split-n/allocate, to_str, parse, currency; Money_take can go negative |
| `motion` | 361 | SOLID-GAPS | P3 | S | good | NDK sensor seam declared but never wrapped (no open/enable/poll); compass point panics on negative degrees |
| `multiboot` | 187 | SOLID-GAPS | P3 | S | thin | a tag with size 0 loops forever; no bounds checks; no module/meminfo/ACPI/EFI tag decoders |
| `mysql_db` | 245 | SOLID-GAPS | P3 | S | good | Database.query returns "[]" on error (indistinguishable from no rows) |
| `nmea` | 617 | SOLID-GAPS | P3 | S | good | no GLL, ZDA, GST, HDT; "!" accepted but no AIS |
| `noise` | 274 | SOLID-GAPS | P3 | S | thin | Perlin + fBm + Poisson only: no simplex/value/Worley, 1-D, ridged/turbulence; lacunarity fixed; poisson2 limited to w,h < 100000 |
| `noise_proto` | 782 | SOLID-GAPS | P3 | S | good | one-way patterns N/K/X and deferred patterns, BLAKE2b, Curve448 |
| `nvme` | 692 | SOLID-GAPS | P3 | S | good | Get Log Page (opcode defined, no builder), Abort, Dataset Management (TRIM), Async Events, interrupts |
| `oauth2` | 538 | SOLID-GAPS | P3 | M | good | device authorization grant (RFC 8628), token introspection (RFC 7662), PAR, DPoP |
| `objc` | 621 | SOLID-GAPS | P3 | M | good | messages with float/struct args or results (by design, KVC workaround); blocks only as encodings |
| `openapi` | 620 | SOLID-GAPS | P3 | M | good | no security schemes, no response validation, JSON bodies only |
| `permissions` | 431 | SOLID-GAPS | P3 | S | good | contacts, calendar, storage, SMS/phone, motion/body sensors |
| `plc` | 1779 | SOLID-GAPS | P3 | L | good | ST subset: no user FUNCTION/FUNCTION_BLOCK, arrays, STRUCT, STRING; no IL/FBD/SFC |
| `pointcloud` | 650 | SOLID-GAPS | P3 | M | good | no RANSAC plane fit, outlier removal, point-to-plane ICP, PLY/PCD I/O |
| `postgres_db` | 33 | SOLID-GAPS | P3 | S | none | the adapter is right but untested; query() returns the rows text even on error |
| `push` | 562 | SOLID-GAPS | P3 | S | good | FCM v1 OAuth access token (service account), APNs/FCM error-response parsing |
| `qpack` | 838 | SOLID-GAPS | P3 | M | good | static table only (valid for capacity 0, but no dynamic-table compression) |
| `qr` | 896 | SOLID-GAPS | P3 | M | good | decoder has no Reed-Solomon error correction (syndrome check only); no Kanji mode, no structured append |
| `quic_conn` | 3360 | SOLID-GAPS | P3 | M | good | no ECN, PMTU discovery (fixed 1200 B), pacing, Cubic; 0-RTT without anti-replay |
| `quic_frame` | 891 | SOLID-GAPS | P3 | S | good (every frame type, params bounds) | preferred_address parsed then dropped, never encodable; non-minimal frame-type encoding accepted; no DATAGRAM / newer transport params |
| `quic_udp` | 385 | SOLID-GAPS | P3 | S | net-only (4 programs in tests/net, not in the suite) | no suite test; IP literals only; O(conns) routing copying each QuicConn; no connection limit or accept notification |
| `raycast` | 452 | SOLID-GAPS | P3 | M | good (test_lib_raycast) | hard 999999 range, no max-distance; wrong AABB normals when starting inside; no grid DDA, capsule, OBB or nearest-of-many |
| `redis` | 1158 | SOLID-GAPS | P3 | S | good | RESP3 push frames mixed with replies get out of step; an integer reply at the i64 edge panics (overflow); no AUTH/SELECT/UNSUBSCRIBE/PSUBSCRIBE/MULTI helpers; inbuf decoded again from the start on every read |
| `regmap` | 575 | SOLID-GAPS | P3 | S | good | access kinds stop at RO/RW/WO/W1C (no W1S, read-to-clear, W0C, write-once); `regmap_write_fields` does not check that names and values have the same length |
| `ringbuf` | 262 | SOLID-GAPS | P3 | S | good (test_std_ringbuf) | negative max in spsc_pop_batch moves tail backwards; peek on empty returns a stale slot; i64 only |
| `riscv_csr` | 807 | SOLID-GAPS | P3 | M | good | CsrFile: S-mode CSRs are not aliased to mstatus/mie/mip and writes to them are silently dropped; unimplemented CSRs do not trap; no SRET and no medeleg/mideleg delegation; no PMP access check; SD bit not maintained |
| `ros2_msg` | 886 | SOLID-GAPS | P3 | M | good (lengths checked against Python struct) | missing common types (sensor_msgs/Image, CameraInfo, JointState, NavSatFix, std_msgs primitives, nav_msgs/Path, OccupancyGrid); no DDS type names; no signed i8/i16/i64 readers; no generic string/struct sequences |
| `router` | 661 | SOLID-GAPS | P3 | S | good | no middleware/before-after hooks; no parameter constraints; the serve adapter keeps a copy of each router forever (works around a compiler closure bug) |
| `save_game` | 533 | SOLID-GAPS | P3 | S | good | no fsync, so "crash-safe" covers process crashes only; no platform save directory; no slot list or metadata; names over 64 KiB silently cut |
| `scancode` | 91 | SOLID-GAPS | P3 | S | thin (5 checks in test_os_stdlib, test_std_ps2) | no E0 extended keys (arrows, right Ctrl/Alt); Ctrl/Alt/NumLock not tracked (SC_LCTRL unused); ASCII only, no F-keys |
| `scene` | 370 | SOLID-GAPS | P3 | M | good (test_game_stdlib, test_scene_entity_updates) | despawned entities never removed and lookups linear (O(n^2) updates); no update-all; tag/flags unused; no hierarchy or save/load |
| `sched` | 454 | SOLID-GAPS | P3 | S | good | no renice or priority change for a live task; one policy per scheduler (no RT class above fair); `dt*1024/weight` truncates to 0 for heavy tasks with small dt |
| `shard` | 64 | SOLID-GAPS | P3 | S | good (test_shard, tests/tp) | header says no-ops, but the ranks are real (stale); all_reduce is sum only; no reduce_scatter, barrier or peer rank for send/recv |
| `share` | 370 | SOLID-GAPS | P3 | S | good | `excluded` field never used; no XML escaping in file_paths; chooser drops the target's details; iOS subject ignored; ClipData not read on receive |
| `simd` | 187 | SOLID-GAPS | P3 | M | good (4 tests) | header names f32x4/f32x8, but f64 only; no min/max/sqrt/fma/compare/select/shuffle; i64 has no mul or shift |
| `spatial_audio` | 479 | SOLID-GAPS | P3 | M | good | no sound cones; clip and output must share a rate; binaural ignores elevation and front/back; no occlusion or air absorption; atan/asin belong in math |
| `sprite` | 342 | SOLID-GAPS | P3 | S | good (test_game_stdlib, test_sprite_stream_updates) | frame_duration <= 0 loops forever; uniform durations, frames not tied to atlas regions; no atlas loader; Animation name clashes with skel |
| `svd` | 361 | SOLID-GAPS | P3 | S | good | `<interrupt>` ignored; no `<dimArrayIndex>`; no derivedFrom on registers or fields, nor dotted paths; field-level `dim`, `isDefault` enum values and `addressBlock` are dropped; derivedFrom only reaches an earlier peripheral |
| `synchrophasor` | 786 | SOLID-GAPS | P3 | M | good (frames checked against an independent encoder) | no CFG-3 frames; decoders trust counts (CRC-valid but inconsistent frame panics); no STAT/time helpers |
| `tariff` | 407 | SOLID-GAPS | P3 | S-M | good | panics when there are fewer dynamic prices than intervals. Time-of-use hours are whole hours only. No seasons, holidays, taxes or minimum bill. Tiers cannot be combined with time-of-use |
| `tlsf` | 384 | SOLID-GAPS | P3 | S | good | no memalign and no add_pool (standard TLSF API); realloc never shrinks; realloc(p, 0) does not free |
| `toml` | 1752 | SOLID-GAPS | P3 | M | good | no writer/serializer. Document accessors return text only (no typed int/float/bool/date) |
| `touch` | 419 | SOLID-GAPS | P3 | S | good (test_lib_touch calls every pub fn) | tap duration threshold stored but never used (events carry no end time); no long-press, double-tap, rotation; swipe direction never reset |
| `trajectory` | 536 | SOLID-GAPS | P3 | M | good | no input checks (dt<=0 never stops). No non-zero start/end speed for trapezoid and S-curve. No multi-axis sync |
| `uart` | 114 | SOLID-GAPS | P3 | S | thin (2 checks in test_os_stdlib + doc examples) | baud > 115200 gives divisor 0; only 8N1; no DLL/DLM, IIR/MCR/FCR bit constants |
| `vfs` | 1268 | SOLID-GAPS | P3 | S | good | no hard links, chmod/chown/utimes, statfs/fsync; no permission checks (EACCES unused); rename into own subtree unchecked; unlink/dup/mkdir_p/pwrite untested |
| `vmm` | 806 | SOLID-GAPS | P3 | S-M | good | no range unmap/protect, no destroy of an address space, no rollback when a map fails, no large-page split. 5-level paging (LA57/Sv57) missing |
| `volatile` | 192 | SOLID-GAPS | P3 | S | indirect (os/kernel/unsafe tests) | no ptr_read/write_volatile16; no fences; no bit read-modify-write helpers; "Volatile<T>" is not generic |
| `vorbis` | 2146 | SOLID-GAPS | P3 | M | good | reads only the first logical stream (no chained files); no seeking; a stream whose first granule is not 0 is not trimmed at the start; the way PCM is gathered per channel may copy everything on each packet |
| `voxel_mesh` | 845 | SOLID-GAPS | P3 | S-M | good | no neighbouring-chunk lookup (border faces always emitted, AO open at borders); no transparent or non-culling materials |
| `wav` | 523 | SOLID-GAPS | P3 | S | good (ffmpeg files, byte-exact) | wav_encode does not validate format/bits (float with bits != 32 or PCM with bits 12 writes a corrupt file); no float64, A-law/µ-law, metadata (LIST/INFO, cue) or RF64 |
| `webview` | 392 | SOLID-GAPS | P3 | M | good | Android and iOS only (no WebView2, WebKitGTK or macOS); handlers are synchronous and never see the origin; wildcard origins never match explicit ports |
| `wfc` | 653 | SOLID-GAPS | P3 | M | good | propagation scans T² per neighbour (no AC-4 support counts); every decision snapshots the full state (memory); tiled model has no tile symmetry classes; 2D only |
| `wss` | 145 | SOLID-GAPS | P3 | S | none | not tested anywhere; Host header has no non-default port; URL parser mishandles "?query" with no path and userinfo |
| `x86_cpu` | 587 | SOLID-GAPS | P3 | M | good | CPUID topology (4/0xB/0x1F), XSAVE (0xD) and address-size (0x80000008) leaves missing; no CR0/CR4/RFLAGS decode; no CMOS RTC date decode; no LAPIC/IOAPIC layouts |
| `xhci` | 766 | SOLID-GAPS | P3 | M | good (modelled controller) | no scratchpad buffer setup; PAGESIZE not used; no Isoch or Event Data TRB builders; endpoint context lacks Max ESIT Payload/Mult/streams; slot context lacks TT fields; no enumeration flow |
| `adc` | 450 | COMPLETE | P3 | - | good | transfer fns, oversampling, filters, calibration, NTC, IIO sysfs + scan types all real |
| `aes_gcm` | 440 | COMPLETE | P3 | - | good | scope stated: AES encrypt + GCM with 96-bit IV and 16-byte tag; not constant-time S-box (documented 29-33); non-96-bit IVs / truncated tags not supported |
| `app_lifecycle` | 406 | COMPLETE | P3 | - | good | transition table, tolerant dispatch, observers, launch kinds, both platforms' callback maps |
| `argon2` | 518 | COMPLETE | P3 | - | good | PHC verify accepts only v=19 (498: legacy v=16 / missing v= strings rejected); no needs-rehash helper |
| `banking_batch` | 390 | COMPLETE | P3 | - | good | proved posting steps, control totals, COBOL file read/write with header/trailer checks |
| `bootloader` | 421 | COMPLETE | P3 | - | good | signed A/B images, trial boots, rollback, anti-rollback counter, power-safe metadata; (single public key, no key rotation or encrypted images) |
| `brotli` | 2452 | COMPLETE | P3 | - | good | full RFC 7932 decoder checked against the reference on 522+1800 streams; encoder documented as basic (no dictionary refs, context maps or block splitting, 1626-1640); decoder is bit-by-bit and slow, no streaming (documented) |
| `brotli_dict` | 160 | COMPLETE | P3 | - | good | generated static dictionary data |
| `bvh` | 749 | COMPLETE | P3 | - | good | SAH build, refit, closest/any hit, AABB query, triangle meshes |
| `calendar` | 659 | COMPLETE | P3 | - | good | Gregorian/Julian/ISO week/ordinal/Easter/Islamic tabular/Hebrew; (no Persian, Chinese, Ethiopian, Indian calendars) |
| `cbor` | 1074 | COMPLETE | P3 | - | good | full RFC 8949 encode/decode, well-formedness checks, diagnostic notation; (no deterministic/canonical map-key ordering helper, RFC 8949 4.2.1) |
| `chacha20poly1305` | 341 | COMPLETE | P3 | - | good | ChaCha20, Poly1305, AEAD, XChaCha20 |
| `cookie` | 664 | COMPLETE | P3 | - | good | RFC 6265 parse/serialize with prefix rules, signed values, session store; (no client-side cookie jar) |
| `cortex_m` | 559 | COMPLETE | - | - | good | ARMv7-M SCS scope met (ARMv8-M/DWT would be extensions) |
| `crc` | 275 | COMPLETE | - | - | good | Rocksoft model, catalogue check values tested |
| `crypto_bind_rsa` | 291 | COMPLETE | - | - | good |  |
| `deep_link` | 474 | COMPLETE | - | - | good |  |
| `dhcp` | 771 | COMPLETE | - | - | good |  |
| `dialogue` | 636 | COMPLETE | - | - | good |  |
| `dma` | 475 | COMPLETE | - | - | good |  |
| `dynlib` | 153 | COMPLETE | - | - | good |  |
| `e1000` | 561 | COMPLETE | - | - | good |  |
| `ed25519` | 489 | COMPLETE | - | - | good |  |
| `fixed_point` | 389 | COMPLETE | - | - | good |  |
| `flash` | 544 | COMPLETE | - | - | good | (style: its own CRC-32 copy, flash.fd:43, instead of crc.fd) |
| `framing` | 273 | COMPLETE | - | - | good | COBS + SLIP as the header promises: paper/RFC vectors, 254/255-byte block edges, random round trips, streaming resync |
| `hpack` | 1284 | COMPLETE | - | - | good (every RFC 7541 Appendix C example decoded and re-encoded byte-exact, all 256 Huffman codes, padding/EOS errors, size updates, eviction) | - (polish: encoder table lookups are linear scans, hpack.fd:1191, 1210) |
| `http2_tls` | 210 | COMPLETE | - | - | live only (tests/tls/http2_tls_live.fd: ALPN choices, h1 and h2 over TLS; needs a certificate, not in suite) | - (thin glue by design: ALPN "h2,http/1.1", HTTP/1.1 fallback; one server per process via global `h2t_creds`, http2_tls.fd:37, inherited from http2) |
| `kalman` | 398 | COMPLETE | - | - | good | KF/EKF/UKF with Joseph form and NIS |
| `kinematics` | 336 | COMPLETE | - | - | good | standard DH only (no modified DH) |
| `lidar` | 381 | COMPLETE | - | - | good |  |
| `matchmaking` | 1162 | COMPLETE | P3 | S | good | polish: private SHA-1/HMAC/CRC-32 copies |
| `md5` | 185 | COMPLETE | - | - | good | RFC 1321 suite in test_blake3_md5.fd; no streaming API |
| `microgrid` | 494 | COMPLETE | - | - | good | one battery; gen pmin not in LP (documented) |
| `motor` | 692 | COMPLETE | - | - | good |  |
| `mp3` | 2317 | COMPLETE | - | - | good | Layers I-III, checked against ffmpeg |
| `mqtt` | 1238 | COMPLETE | - | - | good | 3.1.1 + 5.0 client |
| `multipart` | 573 | COMPLETE | - | - | good | streaming parser with header limit |
| `netcode` | 823 | COMPLETE | - | - | good |  |
| `nfc` | 467 | COMPLETE | - | - | good |  |
| `occupancy_grid` | 347 | COMPLETE | - | - | good |  |
| `odometry` | 281 | COMPLETE | - | - | good |  |
| `onewire` | 818 | COMPLETE | - | - | good | the "fake" markers are the test bus model, a legitimate double |
| `packet` | 1287 | COMPLETE | - | - | good | DNS is header + question by design (answers are dns.fd's) |
| `particle_filter` | 365 | COMPLETE | - | - | good |  |
| `pcap` | 636 | COMPLETE | - | - | good |  |
| `pid` | 272 | COMPLETE | - | - | good |  |
| `planner` | 1211 | COMPLETE | - | - | good |  |
| `postgres_tls` | 66 | COMPLETE | - | - | thin | a thin seam over tls; real check needs a server |
| `power` | 434 | COMPLETE | - | - | good | STM32F4 clock tree and low-power modes (the name is broad, the header is precise) |
| `power_flow` | 908 | COMPLETE | - | - | good |  |
| `process` | 832 | COMPLETE | - | - | good | header still says "Rust's std::process"; Command.output() on Windows expands %NAME% (documented; spawn() avoids it) |
| `procgen` | 810 | COMPLETE | - | - | good |  |
| `ps2` | 539 | COMPLETE | - | - | good |  |
| `pv_model` | 392 | COMPLETE | - | - | good |  |
| `pwm` | 480 | COMPLETE | - | - | good | the fake chip is the test model of the kernel rules |
| `quat` | 623 | COMPLETE | - | - | good |  |
| `quic` | 866 | COMPLETE | - | - | good |  |
| `ramfs` | 504 | COMPLETE | P3 | S | good | (minor) negative offsets reach Vec indexing; no instance destroy (global table only grows); no generation numbers, so a stale inode number can name a reused inode |
| `rtc` | 432 | COMPLETE | P3 | S | good | (minor) no helper that runs the UIP read loop over a port-read function; alarm registers BCD 24h only |
| `sdl2_h` | 894 | COMPLETE | - | - | thin (test_parse_header_sdl reads constants) | generated from SDL 2.32.10 (852 functions, 26 listed as not expressible); note: C `long` is taken as 32 bits (Windows), and no struct layouts (SDL_Rect, SDL_Event) |
| `senml` | 781 | COMPLETE | - | - | good | JSON + CBOR as the header promises, with resolution against the RFC 8428 examples (XML/EXI forms and SenSML streaming not covered: polish) |
| `sqlite3_h` | 382 | COMPLETE | - | - | thin (test_parse_header_sqlite, needs libsqlite3) | generated by parse-header from 3.54.1: 356 functions, 11 varargs ones listed as not expressible; sqlite.fd does not use it (see sqlite) |
| `sse` | 289 | COMPLETE | - | - | good | follows the spec's parse algorithm exactly; encoder, BOM, CR/LF/CRLF split across chunks, Last-Event-ID; all tested |
| `steering` | 434 | COMPLETE | P3 | S | good | (polish: wall avoidance, hide, interpose, offset pursuit, spatial index for O(n²) flocking) |
| `tar` | 632 | COMPLETE | P3 | S | good | (minor) negative base-256 numbers are misread; no GNU sparse ('S'); in memory only, which the header states |
| `thermal` | 315 | COMPLETE | - | - | good | all header items checked against ASHRAE/ISO figures (polish: wet bulb below 0 C uses the over-water formula) |
| `timeline` | 379 | COMPLETE | - | - | good | (polish only: no key removal, no rotation/quaternion track) |
| `tls_alpn` | 146 | COMPLETE | - | - | good | RFC 7301 wire format, validation, the server's choice. Polish: the 65535-byte total-list limit is not checked |
| `totp` | 264 | COMPLETE | - | - | good | RFC 4226/6238/4648 vectors, drift window, replay check, otpauth URIs. Polish: no hotp URI writer, T0 fixed at 0 |
| `trace` | 242 | COMPLETE | P3 | S | good | (polish: more than 64 open spans silently get the wrong parent; span_end prints after tracing is switched off) |
| `transform3d` | 385 | COMPLETE | - | - | good | (minor) no tf3_to_xyz_rpy and no way to remove a frame; checked against numpy |
| `tween` | 480 | COMPLETE | - | - | good | all 31 Penner easings checked against easings.net values; delay, repeat, yoyo and groups tested |
| `unicode_data` | 165 | COMPLETE | - | - | good (indirect) | generated Unicode 16 decomposition/CCC/composition tables plus lookup; tested through unicode.fd's NFC/NFD cases in test_lib_unicode.fd |
| `utility_ai` | 268 | COMPLETE | - | - | good | (minor) only 4 curve kinds (no gaussian, sine or piecewise), no weighted-random top-N pick |
| `watchdog` | 295 | COMPLETE | - | - | good | IWDG, WWDG, generic simulation and kick checker all real (it has no WWDG register-write sequence, but promises none) |
| `wind_model` | 313 | COMPLETE | - | - | good (closed forms, numpy) | small doc mismatch: wind_aep's comment (192-194) names a `steps` parameter that does not exist; the function uses a fixed 4000 |
| `x25519` | 559 | COMPLETE | - | - | good (RFC 7748 §5.2, §6.1, 1000 iterations) | constant-time ref10 field arithmetic and ladder; its limits are documented |

## Per module

Gaps with file:line, by review group (modules in name order within each group).

### collections

BROKEN, P1, effort L. Tests: test_collections (106), test_collections_btree; doctests.

Complete means: generic, correct containers (Deque, LinkedList, HashSet, BTreeMap/Set, RingBuffer) with Option results and no leaks.

Gaps:
- HashSet uses 0 as "empty" and -1 as "tombstone" in the key array (438, 523-535): a set that holds anything reports contains(0) == true, insert(0) is dropped (ran: insert 5, contains(0) -> true; insert(0) leaves len 1), -1 aliases the tombstone
- HashSet::grow never frees the old key block (549-629) and nothing frees the table (no drop): leaks
- everything is i64-only: `Deque<T>` declares T but stores Vec<i64> and impl Deque is not generic (31-35), no generic LinkedList/Set/HashSet/BTreeMap
- Deque::push_front with head 0 shifts the whole Vec (141-154): O(n) per push, so the "O(1) amortized" claim (18) fails for repeated push_front
- empty pops/gets return 0 (indistinguishable from a stored 0) instead of Option
- LinkedList has no remove/insert/pop_back and never reuses popped slots (219-230)
- Set is O(n) per op (306-430)
- "BTreeMap" is a sorted Vec with O(n) insert/remove (878-879)
- RingBuffer header says O(1) at both ends (1193) but has no push_front/pop_back, and RingBuffer::new(0) divides by zero on push (1235)

Test gaps: no test inserts 0 or -1 into HashSet, none checks leaks, none does many push_front calls.

### arena

STUB, P1, effort M. Tests: test_arena_marks (marks only).

Complete means: independent arenas and regions that really own and free their allocations, with usage stats.

Gaps:
- arena_alloc ignores arena (52-56; works only because C p_malloc is mark-backed)
- region_new=p_malloc(0) (93-98), region_alloc untracked (102-106), region_free->p_region_free which is a C no-op (codegen_c.fd:11432) => region memory never freed
- region_free_one plain p_free
- arena_used returns 0 (187-192)
- region_count returns 0 (196-198)
- arenas are one global stack not independent arenas (doc 43)
- frame_end takes FrameArena by value fine

Test gaps: test_arena_marks covers save/restore marks only; region_*, arena_used, region_count and the frame allocator are untested (region memory is never freed).

### core

SOLID-GAPS, P1, effort L. Tests: no dedicated test (exercised by every test; 30+ tests `use core`).

Complete means: a correct prelude: Vec/HashMap/Option/Result/str with type-correct sort, fallible parsing, and Option-returning lookups.

Gaps:
- Vec::sort compares every element as an i64 (3153-3197 "elements compared as i64"): Vec<str> sorts by pointer address (ran: ["bx","ax"].sort() -> "bx" first) and Vec<f64> by bit pattern (ran: [-1,-2,-3] -> -1 first, -3 last)
- no sort_by/comparator
- str_parse_int accepts trailing junk ("12abc" -> 12), returns 0 for "abc" with no error signal, and panics on a 20-digit input (ran: "Integer overflow on multiplication" in String_parse_int 6449-6486) - untrusted input can crash a program
- no fallible parse (Option/Result) for ints or floats in core (str_parse_float returns 0.0 on failure, 7273-7276)
- str_parse_hex wraps silently on overflow (7103)
- HashMap.get of a missing key returns 0 (5080-5089), no Option-returning get, no entry API or get_mut
- Option lacks filter/ok_or/take/zip, Result lacks or_else/unwrap_err
- p_sandbox_enter/exit are empty in compiled programs (5433-5439, documented: compile-time check only)

Test gaps: no dedicated core test: Vec<str>/Vec<f64> sort, parse overflow and parse of junk are untested.

### arinc

BROKEN, P2, effort M. Tests: test_arinc (222), test_partitioned.

Complete means: ARINC 653 APEX: queuing ports that dequeue (FIFO/priority), sampling ports, buffers/blackboards/semaphores/events, process and time management, size/direction checks.

Gaps:
- queuing port receive never dequeues (183-194, comment 181-182 "Does not remove from queue"): once max_nb_message sent the port is full forever
- PRIORITY discipline ignored
- max_message_size not checked on send (164) or sampling write (242)
- direction not enforced (receive on SOURCE ok)
- no blackboards/buffers/semaphores/events, no process mgmt (CREATE_PROCESS/START/PERIODIC_WAIT), time mgmt omitted (327-334)
- schedule has no overlap/cycle validation

Test gaps: no test that a second receive returns the second message (it cannot).

### bits

BROKEN, P2, effort S. Tests: none direct (test_mem_routines, test_new_stdlib use it).

Complete means: correct 64-bit bit operations for all inputs (incl. negative), plus byte swaps and parity.

Gaps:
- bit_rotate_left/right use arithmetic >> (160,173): wrong for any value with bit 63 set (ran: bit_rotate_left(-2,1) = -1, should be -3)
- header example "bit_rotate_left(0x80000000,1) // 0x00000001" (9) is 32-bit and wrong
- bit_highest_set loops forever on negative n (127-133, arithmetic >> never reaches 0)
- bit_reverse same issue OK (masked)
- no byte swap (bswap16/32/64), no parity, no 32-bit variants

Test gaps: no own test; doctests use only non-negative inputs, so the rotate/highest-set bugs are not caught.

### can

BROKEN, P2, effort M. Tests: test_can (341).

Complete means: correct CAN/CAN FD frames, J1939 IDs, ISO-TP segmentation and UDS/OBD-II as they appear on the wire, plus a bus seam.

Gaps:
- J1939 PDU1/PDU2 reversed: PDU1 is PF < 240 (PS = destination), PDU2 PF >= 240 (PS = group extension), but j1939_is_pdu1 returns PF >= 240 (190-192) and j1939_pgn drops PS for PF >= 240 and keeps it below (202-210), the opposite of SAE J1939-21
- no ISO-TP (ISO 15765-2) framing: UDS/OBD frames omit the PCI length byte (obd_engine_rpm_request builds [01 0C], 374-379, real bus needs [02 01 0C]; decoders expect [41 0C ..] at byte 0, 383-396) and uds_response_positive puts any length into one frame (259-271, >8 bytes is not a CAN frame)
- obd_request_to_ecu sends to the response id 0x7E8+n instead of the request id 0x7E0+n (452-457)
- can_frame_print prints "0x" then a decimal id (534-538)
- constructors never validate id range or DLC <= 8 (62-88)
- no CAN FD DLC codes, no SocketCAN/driver seam, no DBC signal decoding

Test gaps: test_can asserts the reversed J1939 PDU1/PDU2 semantics and PCI-less OBD frames, locking the bugs in.

### clinical

BROKEN, P2, effort M. Tests: test_lib_clinical only.

Complete means: clinically correct scores (NEWS2, MELD/MELD-Na, CHA2DS2-VASc, eGFR, Framingham/ASCVD) matching their published charts.

Gaps:
- NEWS2 gives wrong scores (RCP 2017 chart): SBP 201-219 scores 1 (363-364, should be 0)
- temperature bands wrong (379-385: 39.1+ gives 3 not 2, 38.1-39.0 gives 2 not 1, 35.6-36.0 gives 0 not 1, 37.6-38.0 gives 1 not 0, 35.0 gives 1 not 3)
- SpO2 scale 2 on oxygen wrong (345-353: 93-96 and >=97 on O2 must score 1/2/3)
- the +2 for supplemental oxygen and the consciousness (ACVPU) parameter are missing (321)
- clinical_news2_category calls 1-4 "Low-Medium" (396-397: 0-4 is low, low-medium only when one parameter scores 3)
- header promises Framingham and ASCVD risk scores (9) that do not exist
- MELD omits the creatinine cap at 4.0 and MELD-Na despite the comment (188-224)
- header usage calls clinical_gfr_egfr with 4 args (19) vs 3
- CKD stage returns 3 for both 3a and 3b (126-131)

Test gaps: test_lib_clinical does not check NEWS2 against the published bands, so the bugs pass.

### asset

STUB, P2, effort M. Tests: test_lib_asset (text assets only).

Complete means: loading/caching/ref-counting of textures, sounds, fonts, meshes and text through a runtime that exists, with working hot reload.

Gaps:
- textures/sounds/fonts/meshes load through pledge_asset_* from "pledge_asset.h" (356-363), which exists nowhere in the repo: the C build substitutes `return 0` bodies (ran ccompile on test_lib_asset: generated C lines 86-91 are such stubs) so every non-text load returns -1
- asset_load_text discards the text it reads (204-215: handle 0, no accessor for the content)
- last_modified is always 0 and never updated (118, 210, asset_reload 275-305) so asset_hot_reload reloads every asset on every call
- asset_reload loses a font's size (293: 16.0) and cannot reload text
- root_path and auto_reload are stored but never used (80-87)

Test gaps: only text assets; texture/sound/font/mesh loading and hot reload untested (they cannot work).

### audio

STUB, P2, effort M. Tests: test_game_stdlib only.

Complete means: sound playback that actually reaches an audio device in a normal build.

Gaps:
- every call goes to pledge_audio_* (35-49) from include "pledge_audio.h" with link "": codegen_c.fd:2575-2583 includes a header only if it exists relative to the build cwd, else (2698-2718) emits bodies that return 0 -> outside runtime/ every audio call is a no-op (audio_init "succeeds", audio_load returns 0)
- runtime/pledge_audio.c (SDL_mixer) is never compiled or linked (link "")
- header 7-8,27-30 admits "no-ops that return 0"
- only AudioState bookkeeping (141-220) is real

Test gaps: no own test; nothing verifies sound reaches a device.

### audio_mixer

STUB, P2, effort M. Tests: test_lib_audio_mixer (load failure path only).

Complete means: grouped mixing with per-channel volume, panning, 3D attenuation, music streaming with fades, per-group stop.

Gaps:
- header promises music streaming with loop/fade (11) and panning (9): no music or fade functions, no panning call anywhere
- every play sets the volume of ALL channels (sdl_audio_volume(-1, ..) at 126, 142, 165) instead of the channel it starts, changing sounds already playing
- audio_mixer_stop_group ignores the group and stops everything (258-262)
- header usage calls play_3d with 6 args (19) vs 4-arg signature (148)

Test gaps: covers only "load fails without SDL"; volume/3D/group paths untested.

### alloc

THIN, P2, effort S. Tests: test_alloc (runtime).

Complete means: malloc/free/realloc/calloc/aligned wrappers with overflow-checked sizes and a documented failure result.

Gaps:
- no realloc/calloc (zeroed)/aligned alloc
- __alloc_array's count*elem_size (38) overflows into a checked-arithmetic panic instead of the documented 0 on failure (11)
- __alloc_bytes duplicates __alloc
- nothing to query a block's size

Test gaps: test_alloc covers the runtime allocator, not __alloc_array overflow.

### banking

THIN, P2, effort M. Tests: test_banking (+2 expected-fail).

Complete means: accounts with validated, conservation-proved deposit/withdraw/transfer, overdraft rules and history.

Gaps:
- withdraw/transfer allow overdraft and negative amounts with no check (62-70, 86-98, 202-225: balance goes negative silently)
- withdraw/transfer/interest are not @conserve, blamed on a tuple-layout bug (59-61) although money's Money_take now does the proved split (used by banking_batch.fd:122)
- no transaction history, no currency, no fees/limits
- Account_* getters consume the account (117-125) so an account cannot be read and kept

Test gaps: happy path plus two expected-fail @conserve cases; overdraft/negative amounts untested.

### bpe

THIN, P2, effort M. Tests: test_ai_stdlib only.

Complete means: a GPT-2-compatible byte-level BPE tokenizer (byte mapping, pre-tokenizer, ranked merges via a hash map, special tokens).

Gaps:
- merge_rank linear scan per pair (115-125) -> O(n*merges) per step, unusable at GPT-2 50k merges
- bpe_id calls json_has/json_get_int on vocab text per token (106-111)
- no GPT-2 byte-to-unicode mapping or pre-tokenizer regex split
- unknown silently -> id 0 (166-168)
- no special tokens
- splits by byte, so UTF-8 multibyte merges depend on caller

Test gaps: only via test_ai_stdlib with a toy vocab; no real GPT-2 vocab/merges, no UTF-8 text.

### chunk

THIN, P2, effort M. Tests: test_lib_chunk only.

Complete means: voxel chunk storage with generation callbacks, persistence on unload, greedy meshing and serialization.

Gaps:
- header promises greedy meshing and chunk serialization (6, 8): neither exists (only a face count, 363-406, and vertex_count is never set)
- header usage names chunk_world_create(16,256,16), chunk_set/chunk_get and a 4-arg chunk_world_update (14-17) that do not exist (real: chunk_world_create(max_chunks), chunk_world_set/get, 3-arg update)
- "infinite terrain via streaming" loads empty chunks with no generator callback (204) and unloading drops edited chunks with no save hook (224-227)
- chunk lookup is a linear scan per access, and the face counter calls it per voxel neighbour

Test gaps: only test_lib_chunk; no unload/reload data-loss test.

### animation2d

SOLID-GAPS, P2, effort S. Tests: test_lib_animation2d only.

Complete means: sprite-sheet clips with per-frame timing, multi-frame catch-up, ping-pong/events, and real state-machine transitions.

Gaps:
- anim2d_update advances at most one frame per call (141 `if`, not a loop) -> wrong when dt > frame_time
- anim2d_source_rect wraps frame%cols but never adds frame/cols rows (177-178)
- header usage calls anim2d_create() with no args (8) vs (frame_w, frame_h)
- "state machine support" (3) is just named clips: no transitions/conditions, no per-frame durations, ping-pong, frame events or callbacks

Test gaps: no test for dt spanning several frames, source_rect row wrap, non-looping finish.

### args

SOLID-GAPS, P2, effort M. Tests: test_args (57 lines).

Complete means: a command-line parser: flags, options (attached/combined forms), repeats, subcommands, validation and generated help.

Gaps:
- no combined short flags (-abc), no attached short value (-ofile), no repeated-option list getter except opts_present, no unknown-option error, no auto help/usage generation from spec, no subcommands
- args_int no overflow check (177-198)

Test gaps: 57-line test: no "--" interplay with options, attached values, repeated options.

### async

SOLID-GAPS, P2, effort M. Tests: none (test_async*.fd test the language, none `use async`).

Complete means: an executor plus a real readiness poll over many fds (poll/epoll/IOCP) with timers.

Gaps:
- async_spawn takes 1 arg though doc says up to 8 (39-43)
- io_wait polls readable with 0 timeout then blocks only on writable (87-98) -> misses readability arriving during wait
- io_poll blocks on fds[0] only (133-136), timeout<0 becomes 1 s not forever (110-113)
- no real poll/epoll set
- no timers

Test gaps: no test does `use async`; io_wait/io_poll untested.

### atomic

SOLID-GAPS, P2, effort S. Tests: test_lib_atomic.

Complete means: atomic integers/bools/pointers with the full fetch_* set, compare_exchange returning the observed value, and freeing.

Gaps:
- no fetch_and/or/xor/max/min, no compare_exchange returning observed value, no AtomicBool/ptr, no free (p_shared_alloc leaks per atomic_new:55), orders ignored (documented SeqCst - ok), memory_fence empty (66-67, documented)

Test gaps: only doctest-level coverage (test_lib_atomic); no multi-thread ordering test.

### bigint

SOLID-GAPS, P2, effort M. Tests: test_bigint (50), used by rsa/mysql tests.

Complete means: arbitrary-precision integers with checked parsing, bytes/hex I/O, gcd/modinv, bit ops and fast modpow.

Gaps:
- bigint_from_str skips any non-digit silently ("12a3" -> 123, 131-136) with no error result
- bigint_from_i64(i64::MIN) overflows at 0 - v (81-84)
- bigint_to_i64 wraps silently (194-206)
- bigint_modpow with a negative base returns a negative residue because bigint_mod is truncated (686-688, 719)
- no gcd/modinv/extended gcd, no bit ops/shifts, no from/to big-endian bytes, no hex parsing (only to_hex, and that by repeated division, 169-188), no isqrt
- base-1e9 limbs make modpow divide e by 2 each step (723-731): very slow for RSA-size numbers

Test gaps: 50 lines: no invalid input, i64::MIN, negative-base modpow, large division stress.

### buffer

SOLID-GAPS, P2, effort M. Tests: test_stdlib_repaired only.

Complete means: a byte cursor with signed/float/varint reads, peek/patch, and an unambiguous end-of-data signal.

Gaps:
- read_string_lp and read_remaining_string take self by value (284,308) so the caller's cursor does not advance
- -1 "at end" sentinel collides with read_u64 value -1 / signed data (242-269)
- no signed reads (i8/i16/i32), no f32/f64, no varint/LEB128, no peek, no write-at-offset/patch
- bytes stored one per i64 (8x memory)

Test gaps: only test_stdlib_repaired; the read_string_lp cursor bug and -1 sentinel are untested.

### camera

SOLID-GAPS, P2, effort M. Tests: test_lib_camera only.

Complete means: 2D (with rotation) and 3D cameras with correct unprojection, world-to-screen, frustum tests, ortho mode.

Gaps:
- Camera2D.rotation (31) is stored but never applied in screen_to_world/world_to_screen/view_offset
- camera3d_screen_to_ray (296-313) uses world up (0,1,0) instead of the camera's orthogonal up, so rays are wrong once the camera pitches, and its doc says 0..1 while the code expects -1..1 (295 vs 297)
- comment admits no mat4 inverse (298) and math.fd has none
- screen shake reads the wall clock (149) not dt/seed: not deterministic
- orbit starts from yaw/pitch 0 rather than the current position (249-264)
- no 3D world_to_screen, frustum test, or orthographic camera

Test gaps: only test_lib_camera; no rotation, pitched-ray or orbit test.

### cobol

SOLID-GAPS, P2, effort M. Tests: indirect only (test_lib_banking_batch; doctest 70-80).

Complete means: copybook layouts incl. REDEFINES, group OCCURS, OCCURS DEPENDING ON, sign clauses, COMP-1/2, and EBCDIC conversion.

Gaps:
- no EBCDIC (header 3 "EBCDIC aside") although mainframe files are EBCDIC (no CP037/1047 tables)
- REDEFINES is not recognised (413-436 skip unknown words) so a redefining field gets a fresh offset and the layout is silently wrong
- OCCURS on a group item is ignored (437-440) -> wrong layout
- no OCCURS DEPENDING ON, SIGN LEADING/SEPARATE, SYNC, COMP-1/COMP-2
- edited pictures refused (368)
- VALUE clauses ignored (433)

Test gaps: no direct test of the module; REDEFINES/group OCCURS layouts untested.

### collision_broad

SOLID-GAPS, P2, effort M. Tests: test_lib_collision_broad only.

Complete means: 2D/3D broad phase (grid + SAP on all axes) in near-linear time with remove/update.

Gaps:
- pair dedup key a.id * 1000000 + id_b (164) collides once ids reach 1e6 and is checked by a linear scan of all pairs found (167-173), and spatial_hash_query dedups by linear scan (128-136): both quadratic, defeating a broad phase
- sweep-and-prune works on x only (224: y forced to 0), so 2D boxes apart in y are reported
- sap_update leaves sap.aabbs stale (306-323)
- no remove/move for the spatial hash, fixed 256 buckets (62)
- cell coords use truncation not floor ((x / size) as i64, 84-87), merging the two cells around 0
- 2D only, no 3D

Test gaps: only test_lib_collision_broad; no large ids, no y-separated SAP case.

### compress

SOLID-GAPS, P2, effort M. Tests: test_compress (38) + test_lib_compress, test_deflate_huffman, zstd/brotli tests.

Complete means: DEFLATE/zlib/gzip that compress by default and report corrupt or checksum-mismatched input as errors, with streaming.

Gaps:
- decompressors never report errors: a corrupt or truncated stream just stops and returns the partial output (inflate_from breaks out, 683-727; zlib/gzip return an empty Vec on header problems) with no ok flag/Result
- checksums are not verified: zlib's Adler-32 is "advisory - return data regardless" (756), gzip's CRC-32 and ISIZE are never checked (761-800)
- only the first gzip member is read
- FHCRC skipped unchecked
- the default zlib_compress/gzip_compress write stored (uncompressed) blocks (header 9-10) so the obvious call does not compress
- no streaming (incremental) inflate/deflate
- untyped `Vec` signatures (675)

Test gaps: 38-line test; no corrupted-stream or bad-checksum test, no multi-member gzip.

### aarch64_sys

SOLID-GAPS, P3, effort S. Tests: test_std_aarch64_sys (165).

Complete means: encoders/decoders for EL/PSTATE/ESR/sysregs, timer, VMSAv8-64 tables, and GICv2 and GICv3 bring-up sequences.

Gaps:
- header promises GICv3 "standard init sequences" (16-17) but no GICv3 distributor init (GICD_CTLR ARE/EnableGrp1) or ICC_* sequence, only wake/enable_ppi/route (682-706)
- walker covers TTBR0 only (491-492), 4K granule only
- sysreg name table covers ~27 regs

Test gaps: GICv3 distributor/CPU-interface init (does not exist); TTBR1 walks.

### acme

SOLID-GAPS, P3, effort S. Tests: test_std_acme (278).

Complete means: a full RFC 8555 client: accounts (incl. key rollover), orders, http-01/dns-01/tls-alpn-01, finalize, revoke.

Gaps:
- no account key rollover (keyChange parsed at 182 but no §7.3.5 request), no tls-alpn-01 challenge, CSR only for P-256/Ed25519 keys (372-375, 433) though RS256 accounts are supported
- revocation/deactivation payloads exist but the state machine has no flow for them

Test gaps: no test of revocation/deactivation requests, the EAB-required error, RS256 accounts; the ES256 flow only from a C build.

### acpi

SOLID-GAPS, P3, effort S. Tests: indirect only (test_std_apic, test_std_hpet, test_os_stdlib).

Complete means: safe parsing of RSDP/RSDT/XSDT, table lookup by signature, MADT/MCFG/FADT/HPET decoders that never hang on bad tables.

Gaps:
- MADT walks loop forever on an entry length 0 (205,219,235) and read past the Vec when header length > buffer (no bounds)
- no helper to find a table by signature over RSDT/XSDT
- no FADT/HPET/SRAT decoders, MADT entry fields left to caller
- acpi_u* no bounds checks

Test gaps: no own test: malformed MADT (entry length 0), truncated tables, rsdp_find over EBDA/BIOS areas.

### ahci

SOLID-GAPS, P3, effort M. Tests: test_std_ahci (282).

Complete means: an AHCI driver: init, IDENTIFY, DMA read/write, NCQ, ATAPI, error recovery, BlockDev.

Gaps:
- NCQ: only READ FPDMA FIS builder (255), no WRITE FPDMA, no PxSACT issue/complete path
- no ATAPI packet command path (acmd unused by ahci_exec, 592)
- no error recovery (COMRESET via PxSCTL.DET, PxCMD.CLO, port restart after TFES)
- polling only, no interrupt handling
- BlockDev does 1 sector per request (650-668)
- no TRIM/DSM

Test gaps: error paths (TFES, timeouts) and multi-PRD transfers over 4 MiB lightly covered; no NCQ/ATAPI to test.

### android_ndk

SOLID-GAPS, P3, effort M. Tests: test_std_android_ndk (211; C build NOBUILD).

Complete means: NDK constants and layouts plus a working device seam (looper/input wrappers, activity glue), buildable off-device.

Gaps:
- extern blocks link "android"/"log" unconditionally (46, 91): any `use android_ndk` program fails to C-compile off-device (csweep: zig "unable to find dynamic system library 'android'"), so the pure part cannot be built/tested natively on a host
- the device side of the seam is not written: nothing builds an NdkLoopSeam over ALooper_pollOnce/AInputQueue_getEvent/AMotionEvent_* (read_input/poll are left to the caller, 532-540), no ANativeActivity_onCreate glue registering callbacks and a command pipe, AInputQueue_preDispatch/finishEvent never called by the loop
- key-code table partial (170-186)

Test gaps: cannot be C-compiled on a host (link "android"), so only the interpreter runs it; the device seam is untested.

### apic

SOLID-GAPS, P3, effort S. Tests: test_std_apic (243).

Complete means: xAPIC/x2APIC and IOAPIC programming with complete MADT decoding (all NMI/override entries) and accurate timer math.

Gaps:
- madt_parse ignores per-CPU LAPIC NMI entries (uid != 0xFF, 442) and their polarity/trigger flags, and x2APIC NMI (type 0xA)
- apic_timer_calibrate truncates 1e9/ref_ns (188: ref 3 ms -> 333 not 333.3)
- no x2APIC enable helper (APIC_BASE write order), no TSC-deadline arm helper beyond the MSR number

Test gaps: no test for per-CPU NMI MADT entries or calibration precision.

### apriltag

SOLID-GAPS, P3, effort M. Tests: test_std_apriltag (149).

Complete means: detection of the common AprilTag families under real lighting (adaptive threshold) with refined pose.

Gaps:
- tag36h11 only (no 16h5/25h9/tagStandard41h12/Circle families)
- global Otsu threshold, no decimation/adaptive threshold (documented 28-30) so uneven lighting and touching tags fail
- pose has no iterative refinement / ambiguity (two-solution) resolution (920-959)

Test gaps: only synthetic renders; no real photo, low contrast or touching tags.

### asn1

SOLID-GAPS, P3, effort S. Tests: test_std_asn1 (106) + x509/pem/acme users.

Complete means: DER/BER encode/decode with recursive DER validation, big-integer access, constructed strings.

Gaps:
- DER strictness only at the element read with asn1_read_der: asn1_children reads children with BER asn1_read (190), so nested non-DER encodings pass
- asn1_get_int silently truncates INTEGERs over 8 bytes (238) with no overflow signal
- no BER constructed-string reassembly (constructed OCTET/BIT STRING content returned raw)
- no UniversalString (28), REAL, or time-with-offset support (406: refused, OK for RFC 5280)

Test gaps: nested non-DER inside DER; INTEGERs over 8 bytes through asn1_get_int.

### audit_log

SOLID-GAPS, P3, effort S. Tests: test_std_audit_log (114).

Complete means: a tamper-evident log that round-trips every field exactly, refuses bad keys, and supports incremental append/verify.

Gaps:
- audit_from_text trims each line's end (225) so a data field ending in spaces/tabs changes and the record then fails verification (spaces are not escaped, 170-183)
- a key that is not exactly 32 bytes silently falls back to the unkeyed chain (101) instead of being refused
- no incremental append-to-file / verify-new-tail-from-anchor helper
- no signed (Ed25519) checkpoints

Test gaps: no test for data with trailing spaces or a 16-byte key.

### background

SOLID-GAPS, P3, effort M. Tests: test_std_background (105).

Complete means: background-task specs plus a device seam that actually schedules work on Android/iOS (and a desktop scheduler).

Gaps:
- model + simulation only, as the header says: the device seam is not built - bg_workmanager_calls returns descriptive strings (130-150), nothing enqueues through jni/objc, no BGTaskScheduler registration
- no work chaining/continuations, input/output data, expedited work, or in-process desktop scheduler

Test gaps: device mapping only checked as strings.

### behavior_tree

SOLID-GAPS, P3, effort M. Tests: test_std_behavior_tree (227).

Complete means: BehaviorTree.CPP v4's node set incl. scripting pre/post conditions and SubTree port remapping.

Gaps:
- header claims BT.CPP v4's node set (3) but misses KeepRunningUntilFailure, Delay, RunOnce, Sleep, IfThenElse/WhileDoElse/Switch, Script/ScriptCondition and the _skipIf/_successIf/_failureIf pre/post conditions
- SubTree port remapping/_autoremap ignored (881-882: subtrees share the parent blackboard, attributes dropped)
- blackboards are global parallel Vecs searched linearly across all boards (97-120)

Test gaps: no SubTree port remapping test.

### biometrics

SOLID-GAPS, P3, effort M. Tests: test_std_biometrics (83).

Complete means: a cross-platform biometric flow that really calls the platform APIs, incl. keystore-bound (CryptoObject) auth.

Gaps:
- platform calls exist only as data (bio_android_calls/bio_ios_calls 306-327): nothing invokes BiometricPrompt/LAContext through jni/objc
- no CryptoObject / keystore-bound authentication (the part that makes biometric auth secure)
- bio_session_start leaves failures counted after a temporary lockout expires (240-244)
- no Windows Hello / Linux fprintd

Test gaps: pure mapping tested; no platform call.

### bitfield

SOLID-GAPS, P3, effort S. Tests: indirect (test_os_features, test_tier1_kernel).

Complete means: register field get/set that is correct for every width 1..64 with range checks.

Gaps:
- get/set with width=64 do 1<<64 (69,80) -> wrong mask
- no bounds check of offset+width
- i64 only

Test gaps: no own test; width 64 untested.

### bitset

SOLID-GAPS, P3, effort S. Tests: test_bitset.

Complete means: a growable bitset with word-level popcount/iteration and in-place set algebra.

Gaps:
- header says "growable" (3) but no resize/grow/push
- count/any/all/none walk every bit (200-235) instead of word popcount
- not uses per-bit loops (423)
- no in-place union/intersect, no iterator over set bits beyond next_set

Test gaps: no large multi-word cases.

### blake3

SOLID-GAPS, P3, effort S. Tests: test_blake3_md5, test_blake3_tree.

Complete means: BLAKE3 hash/keyed/derive_key with an incremental (streaming) hasher.

Gaps:
- one-shot only: no incremental hasher (new/update/finalize) for streams or large files, no XOF seek
- header cites tests/bootstrap/test_blake3.fd (12), which does not exist

Test gaps: header names a test file that does not exist; streaming not testable.

### ble

SOLID-GAPS, P3, effort M. Tests: test_std_ble (152).

Complete means: BLE advertising, a complete ATT/GATT server (incl. Find By Type Value, long writes) and client procedures.

Gaps:
- ATT server lacks Find By Type Value (0x06, used to discover a service by UUID), Read Multiple, Prepare/Execute Write (long writes): they get REQUEST_NOT_SUPPORTED (858)
- no GATT client procedures beyond PDU builders
- no extended (BT5) advertising, no SMP/pairing (documented 25)

Test gaps: unsupported ATT requests not exercised.

### bms

SOLID-GAPS, P3, effort S. Tests: test_std_bms (146).

Complete means: cell models, SoC/SoH estimation and balancing with temperature effects, limits and fault detection.

Gaps:
- header says the caller supplies temperatures (4) but nothing uses temperature: no temperature-dependent OCV/R0/RC, no over/under-temperature limits
- no state-of-power / current limits, no active balancing, no fault detection (over/under-voltage, overcurrent)

Test gaps: no temperature cases (none modelled).

### bsdiff

SOLID-GAPS, P3, effort M. Tests: test_std_bsdiff (166).

Complete means: binary diff/patch for firmware-sized inputs, ideally BSDIFF40-compatible, with streaming apply.

Gaps:
- old file capped at 1 MiB (BSDIFF_MAX_OLD, 35; 29: rank pairs packed in an i64), too small for many firmware images
- own FDBSDIF1 format only, cannot read/write BSDIFF40
- whole buffers in memory, no streaming apply

Test gaps: no input near the 1 MiB cap.

### cache

SOLID-GAPS, P3, effort S. Tests: test_std_cache (96).

Complete means: generic LRU/LFU/TTL caches with Option-returning get, remove/clear for all, and weighted capacity.

Gaps:
- string keys and values only (no generic K/V)
- a miss returns "" (lru_get/ttl_get), indistinguishable from a stored empty value without a second has() call
- no clear for any cache and no remove for LFU/TTL (only lru_remove, 178)
- capacity counts entries, not bytes/weights

Test gaps: no remove/clear tests for LFU/TTL (functions missing).

### channel

SOLID-GAPS, P3, effort S. Tests: test_channel.

Complete means: typed bounded channels with send/try_send/recv/try_recv/timeouts and named result constants.

Gaps:
- doc references CHAN_PENDING (63) that channel.fd never defines (sync.fd:241 has SYNC_CHAN_PENDING)
- no try_send, no recv timeout, i64 payload only, CHAN_CLOSED is a fn not const

Test gaps: the CHAN_PENDING case is untested.

### coap

SOLID-GAPS, P3, effort S. Tests: test_std_coap (282), test_std_lwm2m.

Complete means: CoAP messaging plus CoRE link-format discovery and (optionally) CoAP over TCP.

Gaps:
- no CoRE Link Format (RFC 6690, /.well-known/core discovery), no CoAP over TCP/WebSockets (RFC 8323), no multicast/group handling
- security (DTLS/OSCORE) left to other modules

Test gaps: -

### complex

SOLID-GAPS, P3, effort S. Tests: test_complex (108), dsp/power-flow/synchrophasor tests.

Complete means: complex arithmetic plus the full set of elementary functions (tan, inverse trig and hyperbolic).

Gaps:
- careful abs/div/sqrt/arg
- but "elementary functions" (4) stop at exp/ln/pow/sqrt/sin/cos: no tan, asin/acos/atan, sinh/cosh/tanh (cx_sinh/cx_cosh exist only as private helpers 584-602), asinh/acosh/atanh, log10
- no complex parsing from text

Test gaps: -

### config

SOLID-GAPS, P3, effort S. Tests: test_config(+fail).

Complete means: typed config from flags/env/file with complete number syntax and a non-panicking API.

Gaps:
- float rejects exponents "1e5" (100-124)
- no config_has / list keys
- errors panic (no Result API)
- no list/duration types

Test gaps: no exponent floats.

### db
Complete means: a database trait plus helpers that build correct, parameterized SQL, and an
in-memory driver that executes the SQL subset the helpers produce (or none at all).
- db.fd:6-8, 28-61: `MemoryDB` is a key-value store. The C runtime (compiler/codegen_c.fd:15971-15995)
  understands only `INSERT key value` / `SET key value`; `query` returns a JSON array of the keys
  (codegen_c.fd:16009-16018), or the single value for `key=`, not rows as objects as the trait's doc
  says (db.fd:19).
- db.fd:74-83: `query_scalar` is documented as "first column of first row" but returns the whole row.
- db.fd:117-142: `query_builder`/`qb_param` replace `?` with the raw text. The doc says "Parameters
  are passed as JSON to avoid SQL injection" (db.fd:118), which is false. There are no prepared statements.
- db.fd:86-104, 235-237: `insert`/`delete`/`update`/`table_exists` concatenate caller text into
  SQL, so `table_exists` is injectable. `get_schema`/`table_exists` are SQLite-only (PRAGMA,
  sqlite_master).
- db.fd:153-175: `build_insert` puts `json_at` raw JSON in the SQL, so string values become
  `"Alice"`, an identifier in standard SQL. db.fd:251-254: `create_table` uses `json_get` raw
  text, which produces `"id" "INTEGER PRIMARY KEY"` (a quoted type, so the constraint is lost or the SQL is invalid).
- Missing: transactions, typed rows, escaping helpers, connection errors.
Tests: thin. test_db.fd (43 lines) covers the KV MemoryDB only; no helper is tested.

### env
Complete means: get (Option/default), set, unset, list all, on every backend, with a clear unset
vs empty distinction.
- env.fd:6: the header says native `p_getenv` is a stub. It no longer is (stdlib/pe_runtime/base.fd:757,
  stdlib/linux_runtime/base.fd), so the comment is stale.
- env.fd:54-63: `env_get` returns "" for both unset and empty. There is no `env_get_or` here (os.fd:52 has one).
- No `env_set`/`env_unset`/`env_vars`: no `p_setenv`/environ builtin exists anywhere (grep of
  compiler/ and stdlib/), so a new builtin is needed on C, pe_ir, nat_ir and the interpreter.
- env.fd:95-105: `os_getenv`/`os_hasenv` duplicate os.fd:35,61, so a program importing both is
  ambiguous.
Tests: none of its own (doctests; used by test_config etc.).

### dns
Complete means: a safe stub resolver: random IDs matched against replies, A/AAAA/CNAME/MX/TXT/SRV
parsing, TCP fallback on truncation, the system resolver configuration, and no shelling out with untrusted text.
- dns.fd:494-498: `dns_resolve_system` builds `nslookup <hostname> > tmp` and runs it with
  `os_system`, so the hostname is shell-injectable. `dns_lookup` (dns.fd:519, the "recommended entry
  point") reaches it. It also writes a fixed temp file "fides_dns_tmp.txt" in the current directory (dns.fd:492), which races between processes.
- dns.fd:22-24: the query ID is always 24817. dns.fd:366-370: the reply's ID, question, source
  address and RCODE are not checked, which allows trivial spoofing and cache poisoning. There are no retries.
- dns.fd:4-5 promises CNAME answers; dns.fd:218-227 parses only A and AAAA. dns.fd:27 lists MX/TXT
  qtypes, but their answers are dropped.
- No TC-bit/TCP fallback, no EDNS0. Hardcoded 1.1.1.1 (dns.fd:381, 408), no resolv.conf.
Tests: thin. test_dns.fd covers the builder, parser and cache; nothing covers the resolver paths.

### derive_json
Complete means: readers for every field type @derive(Deserialize) accepts (numbers, bool, str,
nested structs, Vec, Option, enums), correct JSON unescaping, and errors for out-of-range values.
- compiler/compiler.fd:9912-9971 (`cc_deserialize_field`): Vec, Option, enum and HashMap fields fall
  through to `default_expr_for_type`. They are silently empty, and `_from_json_error` does not report
  them. derive_json.fd has no array or option reader to call.
- derive_json.fd:174-182, 213-215: `\uXXXX` handles only the BMP. A surrogate pair (`😀`)
  becomes two invalid 3-byte sequences.
- derive_json.fd:152-158: `djson_int` takes `str_parse_int` of "1.5" or "1e3" without an error.
  Narrow integer fields are only cast (compiler.fd:9969), with no range check.
- derive_json.fd:111: keys are compared without unescaping. derive_json.fd:144-150: "-" counts as a number.
Tests: good for scalars and nested structs (test_frontend_derive_deser.fd); no Vec/Option/
surrogate/overflow case.

### events
Complete means: an event queue with registration, deferred and immediate dispatch, and
priorities/batching as the header promises.
- events.fd:148-151: `events_dispatch` has an empty body. The test (test_lib_events.fd) checks that
  it "does nothing yet".
- events.fd:131-136: the "simple event queue" the header promises does not exist. Handlers run
  only when called by their mangled names (`__on_x`).
Tests: thin (it checks the placeholder).

### ecs
Complete means: entities with destroy/reuse, component add/get/remove, real queries, registered
systems run per frame, and world reset.
- ecs.fd:67-69 `ecs_dispatch_event`, 84-86 `ecs_register_component`, 91-93 `ecs_register_system`
  (handler ignored), and 96-98 `ecs_run_systems` are all empty bodies. `ecs_frame` (163-166) therefore does nothing.
- ecs.fd:142-149, 152-154: `ecs_destroy_entity` and `ecs_reset` panic "not implemented by the runtime".
- ecs.fd:103-125: `ecs_query_entities` takes a JSON string of names and assumes entity ids are 0..n-1.
  It is O(entities * components) with JSON parsing per entity.
Tests: thin (test_lib_ecs.fd, 56 lines).

### data
Complete means: either remove it (csv.fd and json.fd cover the real parts) or make its loaders
correct and its schema/shape inference real.
- Its own comments call it a placeholder: data.fd:6, 177, 227, 253, 268.
- data.fd:10-16: the usage names `data_load_csv`/`data_csv_row`/..., which do not exist (the
  functions are `load_csv`, `csv_row`, ...).
- data.fd:91-122: `parse_csv_row` does not handle `""` escapes (a quote is lost). data.fd:40 splits on
  "\n" first, so multi-line quoted fields break. csv.fd does both correctly.
- data.fd:296-305: `infer_shapes` passes `json_at` raw text (`"\"relu\""`, quotes included) to
  `infer_op_shape`, so no op ever matches and the input shape always comes back. data.fd:267-290: unknown ops
  return the input shape (a fake result). Only flatten changes a shape.
- data.fd:230-251: `comptime_csv_schema` types every column "string". Header names are not
  JSON-escaped (data.fd:246, and the keys at data.fd:205).
Tests: none of its own (test_stdlib_repaired.fd touches it).

### dp
Complete means: DP mechanisms (Gaussian and Laplace), per-example gradient clipping, a privacy
accountant (epsilon/delta budget, composition), and sigma from (epsilon, delta, sensitivity).
- dp.fd:19-31: three one-line wrappers over `p_dp_noise`/`p_gaussian_noise`/`p_dropout`.
- dp.fd:7-8: the documented formula for sigma is never computed. The caller passes sigma.
- No Laplace mechanism, clipping, budget tracking or randomized response. (`dropout` is not a DP primitive.)
Tests: thin (test_dp.fd checks bounds of the noise only).

### ecu
Complete means: an ECU state machine with legal transitions, fault/DTC memory, calibration maps
(1D/2D lookup with interpolation), sensor plausibility and debouncing, and actuator limits.
- ecu.fd:8 promises a state machine. ecu.fd:23-42 has only constants and names: no transition
  function, no fault -> limp-home logic.
- ecu.fd:180-206: the PID differentiates the error (derivative kick on setpoint steps), has no
  derivative filter, and uses clamp-only anti-windup. It duplicates stdlib/pid.fd.
- No DTC storage/aging, no calibration tables (only scalar CalParam ecu.fd:220-255), no debounce,
  no CAN signal mapping (can.fd exists but is not used).
Tests: good for what exists (test_lib_ecu.fd, 144 lines).

### editor
Complete means: an editor model with selection (multi), transform ops with snapping, undo/redo,
hierarchy (parenting), duplicate with fresh ids, and scene save/load.
- editor.fd:51, 370-372: `max_undo` is stored and set, but there is no undo or redo at all.
- editor.fd:283-293: duplicate uses `id + 10000`, which collides on a second duplicate or with real ids.
  It does not select the copy.
- editor.fd:49 `show_hierarchy`, but there is no parent/child. editor.fd:231 `gizmo_mode` is stored and unused.
- No multi-select, no picking/ray test, no serialization of the scene.
Tests: good for the existing API (test_lib_editor.fd, 192 lines).

### elf
Complete means: a robust ELF parser: 32/64-bit, both endiannesses (or a clear refusal), bounds-checked
headers, section lookup by name, symbol tables (symtab/dynsym), notes, relocations.
- elf.fd:87-115: `elf_phdr`/`elf_shdr` read at `phoff + i*phentsize` with no bounds check, so a
  truncated or hostile file panics. (elf_loader checks the phdr table itself.)
- elf.fd:2-3: ELF64 LE only.
- No symbol, string-table-by-section, note or relocation parsing, and no "section by name" lookup.
- elf.fd:129-141: `elf_str_at` uses `str_from_bytes` (codepoint conversion), which mangles non-ASCII names.
Tests: thin (test_elf_native.fd + indirectly test_std_elf_loader.fd).

### fdt
Complete means: walk and query a DTB: node by path, children/properties iteration, phandle
lookup, `reg`/`ranges` decoding with the parent's #address-cells/#size-cells, string lists, the
memory reservation block, and bounds/version checks.
- fdt.fd:49-70: no check of version/last_comp_version, or that off_struct/off_strings/size_struct
  lie inside totalsize. fdt.fd:65 reads `off_mem_rsv`, but nothing parses the reservation block.
- fdt.fd:172-188: the only query is "prop in this node". There is no path lookup, child iteration or phandle.
- fdt.fd:195-200: `fdt_cell` handles 1 or 2 cells, but nothing reads `#address-cells`.
- fdt.fd:22-36: token kinds are functions returning constants instead of `pub const`.
Tests: thin (test_os_stdlib.fd:188-196, six checks on a synthetic blob).

### flowfield
Complete means: a cost field, an integration field (Dijkstra over costs), a flow field with 8
directions and line-of-sight smoothing, and multiple targets.
- flowfield.fd:294-303: `flowfield_add_cost` adds to an already computed distance, does not
  recompute directions, and is wiped by the next `flowfield_compute` (flowfield.fd:123-129). It
  is a fake "integration cost".
- flowfield.fd:143-180: unit-cost 4-neighbour BFS only. No per-cell cost, no diagonals, one target.
- flowfield.fd:29-32: the comments say directions are "scaled by 1000", but the values are -1/0/1.
Tests: thin (test_game_stdlib.fd:139-163, a handful of asserts).

### encoding
Complete means: base64 (standard and URL-safe, strict and lenient), base32, hex, percent-encoding,
and UTF-8 encode/decode/validate that agree with each other, with error-reporting decoders.
- encoding.fd:70-72: standard alphabet only. Base64url is re-implemented in http2.fd:2009-2075
  and iap.fd:382 (and base32 in totp.fd) because it is missing here.
- encoding.fd:657-712: `utf8_decode` checks only continuation bytes. Overlongs (E0 80..9F),
  surrogates (ED A0..) and F4 90+ decode to code points, while `utf8_valid_bytes`
  (encoding.fd:582-643) rejects them, so the two disagree.
- encoding.fd:158-209, 337-363: decoders stop silently at the first bad character. There is no strict
  variant that returns an error.
- encoding.fd:38-55: `enc_bytes_to_str` returns a raw `p_malloc` string, not a counted one
  (ownership layout 0004), and truncates at a 0x00 byte. The `_bytes` variants are binary-safe.
Tests: good (test_encoding.fd and about 100 importers).

### decimal
Complete means: exact decimals with checked parsing, all common rounding modes, arithmetic
including remainder/pow/min/max, and conversions to and from i64/f64/string.
- decimal.fd:82-143: `dec_parse` returns 0 for invalid text and ignores trailing junk ("12abc"
  gives 12). There is no `Result`/Option parse.
- decimal.fd:390-392: `dec_to_i64` gives no overflow report. There is no `dec_from_f64`/`dec_to_f64`.
- decimal.fd:16-22: no HALF_DOWN/HALF_CEILING. No `dec_rem`, `dec_pow`, `dec_min`/`dec_max`, or `dec_sqrt`.
Tests: thin (test_decimal.fd, 48 lines, plus doctests).

### datetime
Complete means: ISO/RFC parsing and formatting at sub-second precision, durations down to ms, and
RFC 2822 / HTTP-date parsing.
- datetime.fd:168: fractional seconds are "parsed, ignored". LogicalTime holds whole seconds.
- datetime.fd:1406: `dur_parse` does not accept "ms"/"us" (a longest-match parse would remove the
  stated ambiguity).
- datetime.fd:1214: RFC 2822 is only formatted, never parsed (needed for HTTP/email dates).
Tests: good overall (test_datetime.fd is thin at 71 lines, but strptime and languages are in
test_stdlib_calendar*.fd and durations in test_iso_duration.fd).

### crypto
Complete means: the vetted-primitives facade the header describes: SHA-2 family, HMAC, PBKDF2 and
HKDF, AEAD, random, constant-time compare, with incremental hashing.
- crypto.fd:8-10, 491-500: it claims users "can only use algorithms from this module", but SHA-512
  (sha512.fd), HKDF (kdf.fd) and AEAD (aes_gcm.fd, chacha20poly1305.fd) live elsewhere and are not
  re-exported. The facade is incomplete or the claim is stale.
- crypto.fd:76-224: one-shot SHA-256 only. There is no init/update/finish for streams.
Tests: good (NIST and RFC 4231 vectors).

### crypto_bind_unix
Complete means: the same API as crypto_bind over OpenSSL, checked in the suite (or in CI with libcrypto).
- Not in the test suite. Its only check is scripts/verify/crypto_bind_unix_check.fd (header lines 27-29).
- crypto_bind_unix.fd:299-345: `cb_gcm_decrypt` returns an empty Vec on authentication failure, the
  same as a valid empty plaintext (also crypto_bind.fd:316).
- EC_KEY/ECDSA_do_* are deprecated in OpenSSL 3 (header line 22).
Tests: none in suite.

### d3d12
Complete means: bindings usable out of the box: the COM call path ships, and there is a minimal flow
(device, queue, swap chain, fence, PSO from a shader blob).
- d3d12.fd:69 links library "fides_com", whose C source exists only in a comment (d3d12.fd:12-20).
  It is in no repository file (grep), so a ccompile of a user program fails (the C sweep's NOBUILD). Either ship
  the shim or use a typed function-pointer call (dynlib.fd:31-35 shows `fn` types bound to
  addresses).
- No helpers for device/queue/swap-chain creation, fence waits, descriptor heap allocation or
  shader compilation. Untested on a GPU (stated).
Tests: good for layouts and builders (test_std_d3d12.fd); none for calls.

### dds
Complete means: RTPS that can talk to another DDS: SPDP plus SEDP endpoint discovery and matching,
DATA_FRAG, a CDR/XCDR2 serializer for user types, and keyed topics.
- dds.fd:64-67: SEDP entity ids are defined, but no DiscoveredWriterData/ReaderData encode/decode
  exists, so endpoints must be matched by hand (`rtps_writer_add_reader`).
- No DATA_FRAG (0x16) submessage (grep), so payloads above one datagram cannot be sent.
- No CDR serializer for payloads, no key hash/keyed instances, no QoS beyond reliable history.
Tests: good for the wire protocol and reliability (test_std_dds.fd, 211 lines).

### dlms
Complete means: a usable DLMS client: GET/SET/ACTION with block transfer, LN referencing, HLS
authentication and GCM ciphering (security suite 0).
- dlms.fd:28-29: "Ciphering (GCM) and the full xDLMS APDU set beyond GET are out of scope". Most
  deployed meters require HLS and ciphering, and SET/ACTION are basic operations.
- No GET-with-list, no GET-next (block) handling. AARQ carries no authentication mechanism value.
Tests: good for what exists (test_std_dlms.fd).

### dnp3
Complete means: master and outstation for common profiles: class 0-3 polls with events, unsolicited
responses, time sync (g50), DIRECT_OPERATE, analog outputs (g40/g41), float analogs (g30v5).
- dnp3.fd:443-466: only Class-0 static polls are built. No event buffers or classes 1-3.
- DNP3_FC_ENABLE_UNSOL/UNSOL_RESPONSE are defined (dnp3.fd:56-59) but unused. No time objects.
- Objects are limited to g1/g2/g12/g20/g30v1/g32v1. There is no Secure Authentication (SAv5).
Tests: good for what exists (test_std_dnp3.fd).

### ecdsa
Complete means: P-256/384/521 sign/verify/ECDH with constant-time signing.
- ecdsa.fd:16-18: signing is not constant time (window lookups and infinity checks branch on secret
  data). This is stated, but a stdlib signer should not leak. The key-validation path is otherwise fine.
Tests: good (RFC 6979 and OpenSSL vectors).

### crdt
Complete means: the common CRDT set (counters, registers including MV, OR-Set, sequence/text, maps)
with delta sync and tombstone garbage collection.
- No LWW-Map/OR-Map or multi-value register (header lines 5-19).
- ORSet tombstones (crdt.fd:276-283) and RGA deleted elements are never collected.
- crdt.fd:443-452: `rga_index` is a linear scan, so text editing is O(n^2).
- crdt.fd:556-569: `rga_merge` ignores `rga_integrate`'s false (unknown origin), so elements of a
  partial delta are dropped silently.
Tests: good (test_std_crdt.fd, 218 lines, convergence under shuffles).

### crypto_bind
Complete means: AEAD, KDFs, ECDSA/RSA through BCrypt, with distinguishable failures.
- crypto_bind.fd:316-360: GCM decrypt failure is the same as an empty plaintext (an empty Vec).
- P-256 only. No ChaCha20-Poly1305, HKDF, X25519/Ed25519 (BCrypt has some of them).
Tests: good (test_crypto_bind.fd, test_crypto_bind_rsa.fd).

### css
Complete means: tokenizer and parser, specificity, a correct minifier, and selector parsing/matching
enough for a cascade query.
- css.fd:467-477: `css_collapse_ws` uses `char_to_str` per byte, which re-encodes bytes >= 128 as code points and mangles
  UTF-8 in selectors. Its comment says "none around combinators", but the spaces around `>`, `+` and `~` are kept.
- The header's "collapse to the shortest equivalent text" does not happen: values are not
  shortened (#ffffff, 0px, quotes).
- css.fd:504-505: duplicate `return o;` (dead code).
- No selector matching or cascade, and no value parsing (colors, lengths).
Tests: thin (test_std_css.fd, 82 lines).

### csv
Complete means: an RFC 4180 reader and writer with dialects, strict errors, BOM handling, and
round-trippable quoting styles.
- csv.fd:60-102: `csv_parse_dialect` appends one byte at a time (`field + str_slice(...)`),
  quadratic on long fields. `csv_read` slices segments and is the better model.
- No UTF-8 BOM skip. csv.fd:616-634 writes CSV_QUOTE_NONE with an escape byte, but `csv_read` has no
  escape option to read it back.
- csv.fd:242-266: `csv_serialize_dialect` always quotes with '"'.
Tests: good (test_csv.fd and test_csv_dialects.fd, 197 lines).

### diff
Complete means: line diff with linear-space Myers, unified output, patch apply, and optional
word/char granularity.
- diff.fd:45-75: the O(n*m) LCS table uses quadratic memory (a 10k-line file needs 10^8 cells).
- No patch apply or reverse, no word/char diff. A hunk with old_count 0 should print
  `-(line-1),0` per the unified format (diff.fd:237).
Tests: none (doctests only).

### dsp
Complete means: FFT, windows, FIR/IIR design and filtering, plus common analysis (PSD,
correlation) and resampling.
- No order-N IIR design (Butterworth/Chebyshev): only RBJ biquads (dsp.fd:532-593).
- No resampling/decimation, Welch PSD, STFT, auto/cross-correlation.
Tests: good (test_std_dsp.fd, 249 lines).

### dtls
Complete means: what WebRTC needs (met), plus the stated limits.
- Header lines 43-44: no resumption, renegotiation, other suites/curves, DTLS 1.3 or CID. No
  certificate chain or name validation (fine for fingerprint-pinned WebRTC, not for general DTLS).
Tests: good.

### elf_loader
Complete means: load and relocate typical x86-64/AArch64/RISC-V PIE and shared objects, including
TLS, IRELATIVE and COPY.
- elf_loader.fd:291-293: PT_TLS is refused. elf_loader.fd:572-588: no TLS relocations, IRELATIVE,
  R_X86_64_COPY, R_X86_64_32/32S, or RISC-V 32-bit/TLS types.
- elf_loader.fd:618 (kind 4): PC32 is truncated with `& 0xffffffff` and no overflow check.
- elf_loader.fd:296: `Vec::filled(vmax - vmin, 0)` has no cap on memsz, so a crafted header can
  request an enormous allocation.
Tests: good (test_std_elf_loader.fd, 229 lines).

### entropy
Complete means: screening tests for an entropy source: the full SP 800-22 battery or a clear
subset, plus SP 800-90B estimators and the RCT/APT health tests.
- Header lines 5-7: 5 of the 15 SP 800-22 tests. No DFT, approximate entropy, serial, rank, template,
  Maurer, random excursions, linear complexity.
- Only the MCV estimator of SP 800-90B. No repetition-count and adaptive-proportion health tests.
Tests: good (NIST worked examples).

### exi
Complete means: the EXI needed by ISO 15118-2/-20 and general schema-informed use.
- exi.fd:1120-1124, 1948, 2039: no compression, selfContained, preserve options, or undeclared
  productions. No Float/Decimal/date-time datatypes or xsi:type (header lines 51-60). All refuse with an
  error rather than produce a wrong stream.
Tests: good (and 36 DIN 70121 streams via iso15118).

### ext4
Complete means: the stated read/write driver, plus journaled writes and htree maintenance.
- Header lines 46-58: writes are not journaled. Indexed directories are converted to linear ones. No
  xattr writes, inline data, meta_bg, external journal, fast-commit or orphan processing.
Tests: good (test_std_ext4.fd, 1017 lines).

### fat32
Complete means: the stated FAT12/16/32 driver with efficient partial writes.
- fat32.fd:1886-1912: `fat_fs_write` reads the whole file, patches it and rewrites it
  (`fv_replace`). fat32.fd:1915-1925: truncate does the same. Each write costs O(file size), so
  streaming writes are quadratic and a crash mid-write loses the old content.
Tests: good (test_std_fat32.fd).

### file
Complete means: whole-file helpers plus the binary-safe positional and append operations the
runtime already has.
- No wrappers for `p_file_read_at`, `p_file_append_vec` or `p_file_mtime_ms` (tests/bootstrap/
  test_file_read_at.fd uses the raw builtins).
- file.fd:105-110: `file_write_raw` claims binary mode but is `p_file_write`, the same as `file_write`.
- file.fd:24-37: `file_read` returns "" for missing and empty files alike (`file_read_result` exists).
Tests: good.

---

COMPLETE notes: cortex_m covers the ARMv7-M SCS (NVIC, SysTick, SCB, PMSAv7 MPU, fault decoding)
over a RegBus seam. dhcp covers the full RFC 2131 client and server state machines. crc is the Rocksoft
model with catalogue check values. flash duplicates CRC-32 (flash.fd:43), which crc.fd already
provides: a style point only.

### hash
Complete means: fast non-cryptographic hashes (32- and 64-bit FNV-1a, a strong mixer such as wyhash/xxHash, a keyed SipHash for hash-flooding resistance), an order-sensitive combine, CRC32, all overflow-safe.
- hash.fd:105-113 and 121-129: `hash_djb2`/`hash_djb2_bytes` compute `hash*33 + c` in a plain function. Fid integer arithmetic panics on overflow (reference.md:561), and the function has no `@wrapping` and no mask. A probe (`hash_djb2("abcdefghijklmnopqrstuvwxyz")`) died with "Panic: Integer overflow on addition ... at line 109"; any input of about 11 or more bytes panics. Fix: `@wrapping` plus `& 0xFFFFFFFF` (or mask each step).
- hash.fd:76-78: `hash_combine` is `(a ^ b) & 0xFFFFFFFF`: symmetric, so `hash_vec([1,2]) == hash_vec([2,1])` (probe: 3912802342 both) and `[x, x]` cancels. Use the boost/FNV-style `a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2))` (wrapping).
- Only 32-bit outputs; no 64-bit FNV, no keyed hash (SipHash/HalfSipHash) for maps exposed to untrusted keys, no incremental hasher (update/finish) for streaming data.
- hash.fd:133-147: `hash_checksum` is a byte sum mod 256, labelled "NOT a real checksum": fine as named, but Adler-32/Fletcher are the expected simple checksums.
- Tests: test_new_stdlib.fd:84-89 checks only that hash_str is deterministic and differs for two words. No test of djb2 on a long input, CRC32 vectors (the doc's "123456789" -> 3421780262 is right, probe confirmed), hash_vec order, or hash_i64 of negatives.

### iter
Complete means: generic (Vec<T>) adapters and reductions: map/filter/fold/reduce, zip, enumerate, take/skip/chain, any/all/find/position, min/max (Option), sort/sort_by, rev, windows/chunks, dedup, flat_map, sum/product for i64 and f64; optionally a lazy iterator form.
- iter.fd:23-439: every function takes and returns `Vec<i64>` with `fn(i64) -> ...` closures. The compiler has generics (proposal 0002) but this module was never moved to them, so mapping a Vec<str> or a Vec of structs is not possible.
- Missing adapters a user expects: zip, enumerate, fold into another type, flat_map, position/rposition, rev, windows, sort/sort_by/sort_by_key, min_by/max_by, unique (non-consecutive), sum/product of f64, group_by.
- iter.fd:102-136: `iter_min`/`iter_max` return 0 for an empty Vec, which cannot be told from a real 0: should return Option<T>.
- iter.fd:86-94, 72-80: `iter_sum`/`iter_product` panic on overflow (checked arithmetic) with no checked variant: acceptable, but undocumented.
- iter.fd:233-246: `iter_flatten` uses `p_vec_len`/`p_vec_get` raw builtins on the inner Vec instead of its methods (works, but bypasses the typed API).
- Everything is eager and copies: no lazy chain, so `iter_take(iter_map(big, f), 3)` maps the whole Vec.
- Tests: only sum/min/max/product/range (test_new_stdlib.fd:91-101). map/filter/reduce/find/any/all/count/flatten/take/skip/chain/range_step/repeat/partition/dedup/chunk are untested (their doc examples are not run: there is no doctest runner).

### fmt
Complete means: integer (all bases, signed, widths), float (fixed, rounded, scientific, NaN/inf), padding and the table helpers the header promises, all correct for negatives and edge values.
- fmt.fd:129-148 `fmt_float` truncates digit by digit: `fmt_float(1.999, 2)` gives "1.99", not "2.00"; `fmt_float(2.5, 0)` gives "2". Values over 2^63 overflow the `as i64` cast; NaN/inf are not handled.
- fmt.fd:59, 83, 100: `fmt_hex`, `fmt_bin`, `fmt_octal` of a negative number return "" (`while value > 0`; string.fd:428/454 `int_to_hex`/`int_to_bin` have the same loop). No two's-complement or "-" form.
- fmt.fd:223 `fmt_commas(i64 MIN)`: `-value` overflows (panic under checked arithmetic).
- fmt.fd:10-11: the header documents `fmt_pad("hi", 6, ' ')` and `fmt_table(rows)`. Neither function exists. The table helpers (`fmt_row`, `fmt_separator`, `fmt_row_widths`) don't compute column widths.
- Missing: scientific/significant-digit float formatting, a thousands separator for floats, and `fmt_bytes` in SI or IEC units ("500.0 B" is odd).
- Tests: thin. 7 asserts in test_new_stdlib.fd:64-70 plus one fmt_float use in test_vec_math.fd. Floats, octal, binary, percent, bytes, tables and negatives are untested.

### fs
Complete means: read/write/append text and bytes, copy/rename/delete, directories (list, mkdir -p, rm -rf, walk, glob), metadata (size, mtime, permissions, kind incl. symlink), temp files created safely, streaming file handles, and errors that say why.
- fs.fd:291-313 `fs_rmdir_all` and fs.fd:319-331 `fs_walk_into` decide "directory" with `fs_is_dir`, which is `p_is_dir` = `stat()` (compiler/codegen_c.fd:12151-12154), so they follow symbolic links. `fs_rmdir_all("build")` with `build/home -> /home/user` deletes the contents of /home/user; a link cycle makes fs_walk/fs_glob recurse until the stack overflows. Needs an lstat-based "is link" builtin and not descending into links. Security bug.
- fs.fd:444-478 `fs_temp_file`/`fs_temp_dir`: check `fs_exists` then create (time-of-check/time-of-use race), with a predictable name (pid + microsecond clock). In a shared temp directory another user can pre-create a symlink at the name. Needs O_EXCL / CreateFile CREATE_NEW and a random component.
- No metadata API: no mtime (the runtime has `p_file_mtime_ms`, codegen_c.fd:12158, but fs.fd exposes nothing), no permissions/chmod, no is_symlink/readlink/symlink, no canonical/absolute path.
- fs.fd:39-64, 87-93: `fs_read_text` returns "" for a missing file and for an empty one; all functions report failure as bool/""/-1 with no reason (no Result with an error kind). `fs_read_lines` keeps a trailing "" for a final newline and does not strip "\r".
- fs.fd:654-677 `file_put`: a flush whose pending bytes hold a NUL reads the whole file and rewrites it (p_file_write_vec), so writing a large binary file through `File` is O(n^2) (one full rewrite per 64 KB flush). fs.fd:714-725: `File::open` loads the whole file. The runtime now has `p_file_read_at` and `p_file_append_vec` (codegen_c.fd:9141, 9170) that would make both streaming, but fs.fd does not use them.
- tests/bootstrap/test_fs_stream.fd (untracked) describes exactly that streaming File (FILE_READ_CHUNK, p_file_read_at, p_file_append_vec) and calls `fs_modified_ms` (test line 177), which no stdlib module defines: the test cannot compile against the current fs.fd. Either the fs.fd half of that change is pending or it was lost.
- fs.fd:250-281 `fs_mkdir_all` splits on "/" only: a Windows path with backslashes ("C:\a\b") is treated as one segment.
- fs.fd:366-368 `fs_glob_match` is `p_regex_match` (only `*` and `?`): no `[abc]` classes or `{a,b}` alternatives.
- Missing: copy of a directory tree, move across devices (rename fails), file locking.
- Tests: good for the existing surface (test_fs.fd covers walk/glob/temp/mkdir_all/rmdir_all; test_fs_handles.fd, test_lib_fs*.fd, test_files_spec.fd, test_world_dirs.fd). Untested: symlinks, binary File writes with NULs, Windows paths, fs_read_lines with CRLF.

### http
Complete means: an HTTP/1.1 server and message layer (parse/serialize, chunked both ways, keep-alive, limits, query/form/url-decoding helpers, full status table) with no placeholder APIs.
- http.fd:882-919 "request arena": `http_request_alloc` ignores the arena and calls p_malloc (http.fd:894-896); `http_parse_request_arena` and `http_build_response_arena` ignore the arena (907-919); the comment refers to a `p_http_parse_request` that is not used. stdlib/arena.fd:290-306 defines the same names (`http_request_arena_new`, `http_request_alloc`, `http_request_arena_free`), so a program importing both gets duplicate definitions.
- http.fd:1006-1023 middleware is placeholder: `logging_middleware` and `cors_middleware` return the handler unchanged ("pass-through for now"); `apply_middleware` only applies one function. stdlib/middleware.fd and stdlib/router.fd now provide the real thing; this section should be removed or forwarded to them.
- http.fd:950-974 the router's doc promises patterns ("/users/:id") but `router_dispatch` matches the path exactly (str_equal, 966), and handlers receive only the path or request JSON, not an HttpRequest.
- http.fd:531-548/554 `hc_hex` has no digit limit: 16+ hex digits on a chunk-size line overflow i64. Under Fid's checked arithmetic that is an overflow panic, so one request from a remote client can crash the server; where arithmetic wraps, the negative size passes the `HTTP_MAX_BODY` check (558) and reaches `p_str_buf(negative)`. Security bug; cap at 15 hex digits and reject.
- No duplicate/conflicting Content-Length rejection (http.fd:593-604); no `Expect: 100-continue`; no header-count limit separate from head size.
- No helpers to split/decode the query string or url-decode the path (HttpRequest.query is raw text).
- Responses are always whole strings with Content-Length (http.fd:242-244): no chunked/streamed response, no HEAD body suppression, no Date header.
- `status_text` (247-315) lacks 100, 206, 307, 308, 410, 411, 412, 415, 416, 422, 426, 428, 451 ("Unknown").
- `http_request_free` (198-213) is dead code.
- Tests: test_http, test_http_server (keep-alive, chunked, headers), test_http_health, test_stdlib_http_binary are good for the server core. Untested: arena/router/middleware section, oversize/overflow chunk sizes, http_stop draining, scoped serving.

### io
Complete means: stdin/stdout/stderr helpers plus byte streams (memory, file, standard streams) with buffering behind one Read/Write interface, seek on files, binary-safe output, and clear end-of-input signalling.
- io.fd:234-798: Reader, BufReader, Writer and BufWriter are four unrelated structs; BufReader copies the Reader's fields (io.fd:418) rather than wrapping a source, and nothing (no trait or function value) lets a function take "any reader" or "any writer". A BufReader over a BufReader, or a user-defined source (a socket, a decompressor), is not possible.
- io.fd:289-305: a file Reader reads the whole file at open (documented: no positional read on every backend); no seek/tell on Reader. The runtime now has `p_file_read_at` (C) which could back a streaming file source.
- io.fd:556-563: stdout/stderr sinks go through `print`/`p_eprint_str` of a str, so a write stops at the first NUL (documented): binary output to stdout (piping a file, an image) is impossible.
- io.fd:568-587: a file write whose bytes hold a NUL rereads the file and rewrites it whole (O(file size) per write).
- io.fd:92-101: `io_readln` returns "" at end of input, the same as an empty line; core's `read_line` (Option) exists but io_readln is the documented entry point. `io_read_all`/`io_read_lines` read with getchar one byte at a time.
- io.fd:143-144: `io_flush_err` is an empty body (stderr is unbuffered in the C runtime: fine, but it is a no-op by design).
- io.fd:149-161: `io_print_args` only prints Vec<i64>.
- Tests: test_io_streams.fd and test_io_stdin.fd cover every stream method well. Untested: io_getc, io_readln, io_read_all, io_read_lines, io_print_header/separator, file Writer with NUL bytes.

### https
Complete means: fetch over http/https with correct redirects, timeouts, URL handling and response headers, plus an HTTPS server, with tests (a local TLS loopback or the world fakes).
- No test file uses `https` at all (grep "use https" in tests/bootstrap: none).
- https.fd:137-146 redirects: 303 (and 301/302 for POST, as browsers do) should switch to GET and drop the body; here the same method and body are re-sent. A relative Location not starting with "/" is appended to host:port as is (145).
- https.fd:112 Host header is `host` only; a non-default port must be `host:port`.
- https.fd:34-67 own URL splitter: `example.com?x=1` (no path) puts the query into the host; IPv6 `[::1]:8443` splits on the first ':'; userinfo `user@host` is kept in the host. stdlib/url (used by http_client) should be used instead.
- https.fd:120 fixed 30 s timeout, no way to set it; no max-body or header access helper besides `message` (an HttpRequest whose `target` is the status).
- Certificate validation depends entirely on stdlib tls (`tls_client(fd, host)`); nothing here exposes verification options or errors beyond "TLS handshake failed".
- Tests: none. Needs at least URL splitting, redirect resolution and request building tests (all pure), and a loopback run.

### ini
Complete means: parse sections/keys/values with comments, quoting and escapes, typed getters, set/remove, and a serializer that round-trips a file (comments and order kept), with tests.
- No test anywhere uses `ini` (grep `ini_parse`/`ini_get` in tests/ and examples/: none); only doc examples, which are not run.
- ini.fd:84-126: comments and blank lines are dropped, and a section exists only through its entries, so `ini_serialize(ini_parse(text))` loses comments and empty `[sections]` (ini.fd:203-214).
- ini.fd:32-42, 251-259: no escape sequences (`\n`, `\"`, `\;`) and no line continuation; `ini_needs_quotes` does not check for `"`, so a value containing a quote and a `;` serializes as `"a"b;c"`, which re-parses differently.
- No `ini_remove`/`ini_remove_section`; duplicate keys: `ini_get` returns the first, while most INI readers (Python configparser in non-strict mode, git config) take the last.
- ini.fd:115-117: a line without `=`/`:` is skipped silently; there is no error or line number for malformed input (an unterminated `[section`).
- ini.fd:181-187: `ini_get_float` returns `str_parse_float` of any non-empty text, without the numeric check `ini_get_int` has.
- Tests: none. Effort S: a parse/serialize round-trip test, typed getters, comment and quote edge cases.

### infer
Complete means: the kernels of a decoder-only transformer forward pass on f64 Vecs: matvec/matmul, RMSNorm and LayerNorm, SiLU/GELU and gated MLP, RoPE in both conventions, multi-head (and grouped-query) causal attention over a KV cache, softmax, and sampling (temperature, top-k, top-p).
- Missing activations and norms a transformer needs: SiLU, GELU, SwiGLU (elementwise gate), LayerNorm (with bias), elementwise add/mul for residuals, embedding row lookup.
- infer.fd:121-149: `infer_attn` is one head for one query with no causal mask and no scaling option; no multi-head or grouped-query form (heads sharing K/V), so a caller has to slice per head by hand.
- infer.fd:45-62: RoPE rotates adjacent pairs (x[2i], x[2i+1]) only; Hugging Face Llama/Mistral checkpoints use the rotate-half convention (x[i], x[i+d/2]), so their weights give wrong results without a permutation.
- Sampling: no temperature, repetition penalty or min-p; `infer_sample_top_p` needs a separately computed `order`.
- infer.fd:25-40: `infer_rmsnorm` of an empty Vec divides by zero (NaN), unchecked.
- Tests: test_ai_stdlib.fd:88-111 covers matvec, rmsnorm, softmax, argmax, top_k, attn and rope at tiny sizes. `infer_sample` and `infer_sample_top_p` are untested.

### jni
Complete means: the table layout, descriptors, mangling and modified UTF-8 (all present), plus wrappers for the JNI calls an app actually makes: classes/methods/fields, every Call*/Get*/Set* family, strings, arrays, object creation, RegisterNatives, references and frames, and the JavaVM attach/detach.
- No field access: `jni_get_field_id` exists (jni.fd:777) and the slot helpers (`jni_get_field_slot`, `jni_set_field_slot`, 399-409) exist, but there is no `jni_get_field`/`jni_set_field` that calls them.
- No array helpers: New<T>Array, Get/Set<T>ArrayRegion, GetArrayLength (slot 171 is a constant only), Get/SetObjectArrayElement. Passing a byte[] to or from Java is not possible without hand-written slot calls.
- No `jni_new_object` (NewObjectA, slot 30 constant only), no GetObjectClass (31), no RegisterNatives (215), no GetJavaVM (219), no weak references (226/227).
- jni.fd:52-56: `JNI_CreateJavaVM`/`JNI_GetCreatedJavaVMs`/`JNI_GetDefaultJavaVMInitArgs` are declared with no Fides wrapper; the JavaVM table (AttachCurrentThread, GetEnv, DetachCurrentThread) has names (242-250) but no call helpers, so a native thread cannot get a JNIEnv.
- jni.fd:785-794: `jni_call_method` returns 0 when the signature is malformed or the argument count does not match: the same as a null or a zero result. A float return comes back as raw single-precision bits and there is no inverse of `jni_f32_bits` to turn it into an f64.
- jni.fd:860-864: `jni_pop_local_frame` decrements `frames` but leaves `local_refs` counting the references the frame freed, so `jni_leak_report` reports leaks after correct frame use.
- The `call` trampoline seam (jni.fd:24-30) is documented, but the module ships no C trampoline, so every Android user must write one.
- Tests: test_std_jni.fd (230 lines, fake VM) covers the table, descriptors, mangling, MUTF-8, jvalues, strings, exceptions, refs and frames. Untested: jni_get_field_id, jni_get_method_id, jni_jvalue_array, jni_utf16_units with surrogates through mangling.

### fuzz
Complete means: the mutation engine, corpus and minimization (present), plus persistence of a corpus and crashes, hit-count coverage, and use from `fid fuzz`.
- No corpus or crash persistence: nothing writes the corpus or crashing inputs to a directory or loads a seed directory (libFuzzer/AFL users expect `corpus/` and `crash-<hash>` files).
- fuzz.fd:379-391: coverage novelty is "edge id not seen before" only; AFL's hit-count buckets (an edge taken 1, 2, 3, 4-7, ... times) are not modelled, so loops that reach new depths are not kept.
- No hang detection (an execution budget or step counter in the target contract).
- compiler/main.fd:249-255: `fid fuzz <file>` runs the file's `fuzz_*` functions through the test runner with random seeds; it does not use this module's mutators, corpus or minimizer.
- fuzz.fd:22-31: the target reports its own edges and crashes (no compiler instrumentation, no panic catching): a documented seam, not a stub.
- Tests: good (test_std_fuzz.fd covers rng, mutators, run, corpus minimize, ddmin, simplify). Untested: `fuzz_mut_dup_chunk`, `fuzz_mut_arith`, `fuzz_mut_interesting`, `fuzz_corpus_cover` order, `stop_on_crash = false`.

### ipc
Complete means: the seL4-style endpoint IPC state machine and capabilities (present) with consistent state checks on every operation, endpoint and thread teardown, notification objects, and a validated shared ring.
- ipc.fd:351-374 `ipc_do_recv` checks only the cap, not the thread's state: a thread already blocked on send (or waiting for a reply) can also be queued on an endpoint's receive queue and be resumed twice. `ipc_do_send` does check (305-307).
- ipc.fd:429-433 `shmring_new` does not check that `capacity` is a power of two (the slot mask `head & (cap - 1)` assumes it) or at most 65536 (indices wrap at 2^16): capacity 3 overwrites live descriptors.
- ipc.fd:82-86: `Endpoint.alive` is checked but nothing ever sets it false: no endpoint destroy that fails the threads queued on it with an error; no thread delete/cancel of a blocked operation.
- ipc.fd:359-363: a sender queued on an endpoint is delivered even if its cap was deleted or revoked in between (ipc_transfer reads a CAP_NULL cap and uses badge 0).
- No notification objects (seL4's asynchronous signal/wait with a badge word), which most seL4-style drivers use for interrupts.
- ipc.fd:405-408 `ipc_reply_recv` ignores the reply's result.
- Tests: good (test_std_ipc.fd exercises every public function except ipc_poll, ipc_cap_delete, ipc_thread, shmring_space, shmring_set_event). Untested: the double-blocking case above, non-power-of-two rings, index wrap at 65535.

### jobs
Complete means: a durable job queue (priorities, delays, retries with backoff, dead letters, leases, cron with time zones), with cancel/heartbeat/uniqueness, safe use by several workers through a backend, and persisted recurring jobs.
- jobs.fd:778-818: cron is evaluated in UTC only; "0 6 * * MON-FRI" means 06:00 UTC, with no time-zone or DST handling (stdlib tz exists).
- No `jobs_cancel`/`jobs_delete`, no lease extension (`jobs_heartbeat`/touch) for long jobs, no unique/dedup key, no per-queue concurrency limit, no progress field.
- jobs.fd:31-32 says workers may be "another process sharing the backend", but jobs.fd:327-334 reserves from the in-memory index and writes through with `put`: two processes with their own JobQueue over one backend can reserve the same job. The JobBackend interface has no compare-and-set/claim operation.
- jobs.fd:233-256 `jobs_load` restores jobs but not cron entries (they live only in the JobQueue's parallel lists, jobs.fd:209-214).
- jobs.fd:497-509 `jobs_tick` after a long pause enqueues every missed occurrence (up to 1000 per entry): no misfire policy (skip / run once).
- jobs.fd:86-95, 265-274: the in-memory backend and `jq_index` are linear scans (fine for tests; a real backend is the seam).
- Tests: good (test_std_jobs.fd, 241 lines: priorities, backoff determinism, dead letters, leases, cron fields/macros/Vixie rule/Feb 30, load from backend). Untested: cron step on day-of-week with 7, `keep_done`, error records in `jobs_load`.

### gl
Complete means: working object create/delete, shader compile with the error log, uniforms/attributes, blending/depth state, and a function loader so GL 3.3 works on Windows/Linux.
- gl.fd:122-167: `gl_gen_buffer`, `gl_gen_vertex_array` and `gl_gen_texture` call `glGen*(1, 0)` with a NULL out-pointer and return the untouched local 0. They never return a real name, and some drivers crash on the NULL. The `gl_delete_*` wrappers pass NULL too, so nothing is deleted. The comment "native: pass &buf[0]" admits the gap. (`gl_out_block` at :262 already shows the right technique.)
- gl.fd:40/228: GL 2.0+/3.x functions (glCreateShader, glGenVertexArrays, glUniform*, glActiveTexture) are bound straight from `<GL/gl.h>` and libGL. On Windows, opengl32.dll exports only GL 1.1, so these neither compile nor link without a loader (wglGetProcAddress/GLAD). No loader is present.
- gl.fd:188-191: `gl_enable_blend` does not call glBlendFunc ("needs FFI").
- gl.fd:303-306, 343-349: when compile or link fails, the info log is not read (`gl_get_shader_info_log` is declared but unused), so the user gets 0 with no message.
- gl.fd:11-28: the header usage calls `gl_create_program(vert_src, frag_src)`, which is the 0-argument FFI function. The real one is `gl_create_program_from_src`.
- Missing: glDepthFunc/glCullFace/glFrontFace, glUniform*fv, glGenerateMipmap, framebuffers/renderbuffers, glTexSubImage2D.
- Tests: thin. test_lib_gl.fd checks only constants and `gl_error_string`. No context-free check exists of the wrappers' argument passing.


### font
Complete means: load a TTF (file or memory), real metrics, glyph bitmaps, kerning, text measuring and rendering, and a real built-in bitmap font for when no TTF is available.
- font.fd:99-111: `pledge_font_load` returns the PledgeFont pointer on success (runtime/pledge_font.c:89) and -1 on failure. font.fd treats `result != 0` as failure, so every successful load returns `info = 0` and leaks the C font. `info` is never assigned anywhere.
- font.fd:105-109: ascent/descent/line gap are hard-coded at 0.8/0.2/0.1 of the pixel size ("would be filled by pledge_font_load"). descent is positive while the C side's is negative.
- font.fd:139-151: `font_get_glyph` passes 0 for every out-pointer, so width/height/advance stay 0, and it fills the bitmap with zeros ("actual pixels would be filled"). runtime/pledge_font.c:160-169 would also write i64s straight into a Vec header ("A full implementation would call the Fides Vec push method").
- font.fd:71: `pledge_font_load_mem` is declared but no Fides function exposes it. `Font.data` is never filled.
- font.fd:307-335: `font_bitmap_char` draws the same outlined box for every printable character ("simplified", :325). There is no glyph table. framebuffer.fd:318 has a real 8x16 ASCII font that could be reused.
- stb_truetype.h is not in runtime/ (pledge_font.c:13 includes it). A user must download it.
- Tests: thin. test_lib_font.fd checks only that the box is drawn, and its header says the TrueType path is not run.


### image
Complete means: decode the common formats (at least PNG and BMP; JPEG through a seam is fine) into real pixels, encode/save, convert between channel counts, upload to a texture, and the basic pixel operations.
- image.fd:134-140 and 149-188: loading returns an Image of the right size with blank pixels. The comment says it outright: "pledge_image.c does not write the pixels (it never touches out_data)". `out_data` is passed as 0 at :164 and :166.
- image.fd:221-236: `image_to_texture` passes 0 as the pixel data, so the texture is empty.
- image.fd:211-216: BMP goes through the PNG entry point. There is no format sniffing by magic bytes, and an unknown extension is "try PNG".
- image.fd:45-66: `image_create` only handles 3 or 4 channels. With 1 or 2 it loops in steps of 3 and makes a buffer of the wrong size.
- Missing: any decoder or encoder in Fides (stdlib/compress.fd could supply inflate for PNG), save/write, bilinear resize, blit/alpha-blend between images, grayscale/convert.
- Tests: good for the pure pixel operations (test_lib_image.fd). Loading and texture upload are untested.


### input
Complete means: per-frame keyboard/mouse/gamepad state fed by a real platform poll (SDL or OS), with edge detection, text input, and one consistent set of codes.
- input.fd:137-168: `input_update` only copies current to previous. "Poll platform (stub ...) For now, the state stays as-is". runtime/pledge_input.c exists (pledge_input_poll, pledge_sdl_poll_events), but no Fides code calls it; `input_init` at :87 is the only FFI call.
- input.fd:77-82: the FFI declares `* u8` pointers for x/y/axes, where the C side writes words or doubles. Nothing calls it today.
- input.fd:23-25: the comment says "USB HID / SDL2 scancodes", but the values are GLFW key codes and match neither.
- input.fd:31 vs gamepad.fd:75-99: GAMEPAD_LEFT_BUMPER = 4 here but GP_BUTTON_LEFTSHOULDER = 9 in gamepad.fd. The two modules number the buttons differently.
- input.fd:380-435: in `input_direction`, Left plus A gives dx = -2 (only diagonals are normalised). The stick fully overrides the keyboard.
- Missing: gamepad released and mouse-button released for the gamepad, text/char input events, key repeat, several gamepads, and the dead zone (gamepad.fd has one).
- Every query takes `InputState` by value (:173 etc.), copying four Vecs per call.
- Tests: good for the setter/query logic (test_game_stdlib.fd). The platform poll is untested and unimplemented.


### game
Complete means: either a real minimal 2D engine layer (window, render primitives via sdl/gl, typed asset loading, input dispatch with the key, a loop that can stop) or removal in favour of gameloop/input/sdl.
- game.fd:152-170: `render_clear`, `render_rect`, `render_circle`, `render_text` and `render_present` are no-ops ("No-op placeholder").
- game.fd:101-107: `load_asset` returns `p_file_read`'s raw address as the handle ("For now, return the file content as a handle"). `load_texture`, `load_sound` and `load_model` are the same call.
- game.fd:132-144: `input_key_down`, `input_key_up` and `input_mouse_move` ignore the key and coordinates and dispatch a fixed event name. The names also clash with the concept in input.fd.
- game.fd:192, 204: `compile_script` and `hot_reload` are empty placeholders.
- game.fd:71-80: `game_loop` takes `config` by value and the callbacks cannot reach it, so `while config.running` never ends. There is no delta time (fixed frame_time) and no timing.
- game.fd:130: the INPUT_* constants are private and unused.
- Tests: thin. test_lib_game.fd tests the config and physics helpers and that the placeholders "do nothing". `game_loop` and `game_frame` cannot be compiled through C, per the test's header.


### grid
Complete means: IEC 61850 logical nodes with working behaviour (breaker, measurement, protection timing), plus real GOOSE encode/decode. That could be a thin layer over stdlib/iec61850.fd, which already has BER GOOSE/SV, or the module could be merged into it.
- grid.fd:331-358 `iec61850_goose_serialize` is a "simplified format". It is not the IEC 61850-8-1 frame: no Ethertype 0x88B8/APPID/length header and no BER goosePdu. gocbRef, datSet, goID, timeAllowedtoLive, confRev and test are missing. Values are cut to 32 bits (352-355) and the entry count to 1 byte (348).
- grid.fd:10 says the module does "GOOSE message encoding/decoding", but there is no decoder at all.
- grid.fd:303-316 has the stNum/sqNum semantics backwards. Adding a dataset entry bumps stNum, and changing a value bumps sqNum. The standard does the opposite: a state change gives stNum+1 and sqNum=0, and a retransmission gives sqNum+1.
- grid.fd:24-34: PTUV, PDIS, RREC, RBRF, CSWI, CILO and XSWI exist only as name constants, with no structs or behaviour. grid.fd:76-84: the CDCs are names only, with no SPS/DPS/MV value, quality or timestamp types.
- grid.fd:380-382: `iec61850_control_ref` appends "CO" to the object name. That is not a valid reference (CO is a functional constraint, not part of the name).
- grid.fd:409-415: `iec61850_ptoc_check` trips at once when the current reaches pickup. It ignores time_delay and the inverse curve, and nothing accumulates time.
- grid.fd:426-444: only the IEC standard-inverse curve exists. Very-inverse, extremely-inverse and long-time inverse are missing, as are the IEEE C37.112 curves and a reset time.
- grid.fd:271: `total_power` is the sum of V*I products and ignores the phase angle. It is labelled "approx".
- Tests: test_lib_grid.fd checks names, the breaker counter and the fake serialize bytes. Nothing tests interop with a real GOOSE frame.


### gamepad
Complete means: the current pure core, plus buildability without SDL2, a mapping database and per-frame edge detection.
- gamepad.fd:44: the `extern "C" ... link "SDL2"` block is unconditional, so any program that uses only the pure part needs SDL2 to link. That is why test_std_gamepad was NOBUILD in the C sweep (csw9/test_std_gamepad.build: "unable to find dynamic system library 'SDL2'"). The seam should be split out (a gamepad_sdl module, or @cfg).
- Missing: a mapping database (parse a gamecontrollerdb.txt, pick by GUID and `platform:`), SDL 2.26+ GUID fields (the crc is decoded but unused), and the paddles (16-19 have names but no constants).
- Missing: previous-state tracking for pressed/released, plus gyro/accelerometer and touchpad (SDL_GameControllerGetSensorData).
- Tests: good. All pure functions are covered; `gp_sdl_poll` and `gp_rumble_send` are not.


### gltf
Complete means: a robust glTF 2.0 mesh and scene loader: all buffers (including data: URIs), bounds-checked accessors, node hierarchy with TRS/matrix, all primitive modes, and material/texture references.
- gltf.fd:246-264: for BYTE/SHORT, `gltf_acc_uint_at` returns `rd_i8 + 256` / `rd_i16 + 65536` even for non-negative values (byte 5 reads as 261).
- gltf.fd:236, 246-313: accessor offset, count and stride are never checked against the buffer or `bin_len`. A malformed file panics on `Vec.get` instead of returning valid = false.
- gltf.fd:224-236: `bufferView.buffer` is ignored and one `base` is used for all views. Several buffers and embedded `data:` base64 URIs are not supported.
- gltf.fd:145: there is only `gltf_node_count`. No accessor exists for nodes, children, translation/rotation/scale/matrix or scenes, so the meshes of a multi-node file cannot be placed.
- gltf.fd:369-372: only mode 4 (triangles). Strips and fans (5/6) are rejected rather than converted.
- gltf.fd:290-291: in the float decode, NaN reads as +inf.
- gltf.fd:14: there is no `module gltf;` line (every other module has one).
- The header lists skins, animations, sparse accessors, KHR extensions and Draco as out of scope. That is acceptable, but sparse accessors and texture indices are commonly needed.
- Tests: good (test_gaming_stdlib.fd and test_stdlib_jsondoc_loaders.fd cover GLB, JSON+bin, accessors, missing meshes and materials). Signed/normalized accessors, strides and malformed lengths are untested.


### i18n
Complete means: catalogs with fallback, full ICU MessageFormat (plural/selectordinal/select, quoting), CLDR plurals for the supported locales, and number, percent, currency, date and time formatting.
- i18n.fd:529-568: no `selectordinal` (ordinal rules: 1st/2nd), no `offset:` and no ICU apostrophe quoting. A missing argument prints "{name}" (:471), with no error report.
- i18n.fd:510: an exact match compares the raw string, so a value "1.0" does not match "=1".
- i18n.fd:294-323: decimals only. There is no currency (symbol placement, ISO codes), no percent and no compact notation. Input like "1e5" is not handled.
- i18n.fd:339-404: dates only. No times, no weekdays, no medium/full styles, and no time zones.
- i18n.fd:123: pt uses the Brazilian rule; pt-PT is not distinguished.
- There is no way to load a catalog from a file (gettext .po, JSON). Entries can only be added one by one.
- Tests: good (plurals across locales, numbers, 22 dates, messages). Selects and nested plurals are tested lightly; there are no error cases.


### http2

Complete means: RFC 9113 frames, connection and stream state, flow control, HPACK, h2c (prior knowledge and Upgrade) and h2 via http2_tls, a server on http's handlers and a client with requests, responses including trailers, respecting the peer's settings. Push and priorities are deliberately left out (documented, as browsers do) and are not counted as gaps.

Gaps:
- http2.fd:1327-1337: trailers (a second HEADERS) are validated, then thrown away on both ends. `Http2Response` (2427) has no trailer fields, and `h2_respond` (1696) cannot send trailers. A user of HTTP/2 who needs trailers (gRPC status, checksums) cannot get them from this API. stdlib/grpc.fd works around it on the frame layer.
- http2.fd:2493-2541: `http2_request` never checks `c.peer_max_streams`, which is stored at 1046 and never read. `http2_get_many` (2740) with more paths than the server's MAX_CONCURRENT_STREAMS sends them all, and the extra streams come back REFUSED_STREAM instead of being queued.
- Bodies are buffered whole. The handler returns one `str`, and the request body sits in `H2Stream.body`, capped at H2_MAX_BODY 16 MB (73). There is no streaming request or response body and no server-sent stream (long poll, SSE over h2).
- http2.fd:1621, 1905: the handler, the "handler on a task" flag and the stop flag are process globals, so two HTTP/2 servers with different handlers cannot run in one process.
- http2.fd:73-76: the idle timeout (30 s), client response timeout (15 s), stream and connection windows, max streams and max body are constants with no setter.
- Performance: bytes are `Vec<i64>` with a per-byte copy at every layer: `h2_flush` (837) does `p_volatile_store8` per byte, `h2_fill` (860) and `h2_sub` (215) copy byte by byte, and `h2_pump` re-slices `pending` for every frame (1596, O(n^2) for large bodies).
- http2.fd:1174: CONNECT is accepted as a request but there is no tunnel, and there is no extended CONNECT (RFC 8441, WebSocket over h2). This is P3.

Test gaps: GOAWAY is covered, and flow control is covered as frames plus hand-driven responses. Not covered: INITIAL_WINDOW_SIZE changes moving open streams' windows (1028-1042), CONTINUATION split on send (1533-1542, where only the receive side is exercised), PUSH_PROMISE rejection, and a client `http2_wait` across a GOAWAY that refuses streams (1489-1493). The live tests (tests/net/http2_live.fd, tests/tls/http2_tls_live.fd) are not in the suite.


### http3_udp

Complete means: a usable HTTP/3 client and server over real UDP: connect by host name, authenticate the server, single-shot fetch and a polled server.

Gaps:
- The client authenticates nothing (from quic_tls.fd:16-17, 1893-1909): with no pinned Ed25519 key, any server is accepted. tests/net/http3_public.fd says it runs "like `curl -k`". `h3_udp_fetch` (http3_udp.fd:95) against a public server is therefore unauthenticated. The fix belongs in quic_tls (certificate chain and CA store, or a system verifier seam), but this is the module a user reaches for.
- http3_udp.fd:67, 95: it takes an IP literal (quic_udp_connect / udp_addr, quic_udp.fd:24 "host names: resolve them first"). `h3_udp_fetch("example.org", ...)` fails, so the caller must call dns themselves and pass the name again as `authority`.
- http3_udp.fd:138-182: every poll rebuilds the per-connection state vectors with a cids x conns byte comparison, and copies each QuicConn and H3Conn out and back. This is polish.

Test gaps: only live programs (tests/net/http3_udp_loopback.fd, http3_public.fd, http3_curl_server.fd); nothing in tests/bootstrap. The loopback test needs no network and could join the suite on C/native builds.

### http_client
Complete means: a client for http:// (and, via tls, https://) with timeouts, redirects, chunked/length bodies, errors reported, and a usable response type; or it should be folded into https.fetch.
- http_client.fd:7-10 header says "no TLS ... tls.fd (not yet available)": stale; stdlib tls and https exist. `http_get_url` etc. return status 0 for https:// (264-266, 285-287, 306-308) instead of delegating.
- http_client.fd:53-70 reads the socket into a Vec one byte at a time via `p_volatile_load8`, then str_from_bytes: slow for large bodies.
- No timeout on connect/read (227-232), no redirect following, no keep-alive; failures give status 0 with no reason (no `error` field, unlike https.FetchResult).
- hc_parse splits headers with `str_lines` and trims (132-162): obs-fold and duplicate headers are not handled; Content-Length is ignored (body is everything to EOF), so a server that keeps the connection open hangs the client.
- Two client APIs (`http_client.HttpResponse` and `https.FetchResult`) with different shapes for the same job.
Tests: test_lib_http_client covers header lookup, https refusal and body type; response parsing/de-chunking is reached only through test_world_net/test_world_fakes (world fake network). No test of chunked bodies with extensions or of real socket reads.


### graphql
Complete means: lexer, parser, SDL, a validator for the core §5 rules, and an executor with correct input/result coercion and null propagation, plus the introspection fields GraphiQL's standard query asks for.
- No validation phase: unknown fields are reported at execution with partial data (test "unknown field" returns data with `"mass":null`; spec: validation error, no data); unknown arguments, wrong argument types, undefined variables, unused fragments, fragment cycles (only guarded by `visited`, graphql.fd:1199-1218) are not rejected.
- graphql.fd:1326-1333 non-null violation records an error but returns "null" to the parent, which keeps it; spec §6.4.4 requires the null to propagate to the nearest nullable parent field.
- graphql.fd:1354-1356 scalar and enum results are returned as resolved, without coercion (an Int field returning "3" or 3.5 passes; an enum value is not checked against the enum).
- graphql.fd:1459-1481 variables are taken as given: no coercion/validation against their declared type (§6.1.2), no input-object field checks; arguments likewise (1292-1324).
- graphql.fd:1486-1492 a `subscription` operation runs against the query root; the `schema { }` block ignores `subscription:` (845-856).
- graphql.fd:842-844 `extend type X` pushes a second type X instead of merging fields.
- Introspection (1000-1086): no `description`, `interfaces`, `inputFields`, `isDeprecated`, `directives`, `subscriptionType`; descriptions are discarded by the SDL parser (779-783).
- graphql.fd:70-83 `\u` escapes encode surrogate halves separately instead of combining a pair (no 4-byte UTF-8 path).
- Errors carry `path` but no `locations`.
Tests: test_std_graphql is broad (parse, SDL, execution, fragments, unions, mutations, introspection). Untested: non-null propagation, list-of-non-null errors, variable type errors, subscriptions, extend.


### imap
Complete means: an IMAP4rev1 client covering the RFC 3501 command set (incl. LIST/STATUS/APPEND/mailbox management, AUTHENTICATE, STARTTLS hook) with parsed results, modified UTF-7 mailbox names, and an efficient incremental reader.
- imap.fd:5-7 header lists builders for LIST, COPY, EXPUNGE, NOOP: none exist (function list has no imap_list/imap_copy/imap_expunge/imap_noop); only the generic `imap_command`. No parser for `* LIST`/`* LSUB`/`* STATUS` results.
- Missing commands: APPEND (needs the literal path), CREATE, DELETE, RENAME, SUBSCRIBE, STATUS, CLOSE, UNSELECT, MOVE, UID STORE/COPY/EXPUNGE, AUTHENTICATE (PLAIN, XOAUTH2), STARTTLS (transport upgrade hook), ID, ENABLE.
- No modified UTF-7 (RFC 3501 §5.1.3) encode/decode for mailbox names: `imap_select(c, "Entwürfe")` sends raw UTF-8 as a literal.
- imap.fd:1134-1185 `im_body` does not handle message/rfc822 parts (envelope + nested body + lines): the nested structure is lost; extension data (disposition, language) ignored.
- imap.fd:566-600 every read reparses the whole input buffer from 0 and copies the remainder into a new Vec after each response: quadratic for large FETCH literals (and imap_parse_response copies the Vec by value).
- imap.fd:999-1070 RFC 2047 decoding only for UTF-8/US-ASCII/ISO-8859-1 (no Windows-1252, ISO-8859-15).
Tests: test_std_imap covers RFC examples, partial literals, astring, encoded words and a scripted session with LITERAL+. Untested: the non-LITERAL+ continuation path (imap.fd:625-643), IDLE with intervening responses, error/BYE handling.


### html
Complete means: a safe builder plus an HTML parser that round-trips real pages (all named references, raw-text elements kept verbatim on output, implied html/head/body), and element queries including simple CSS selectors.
- html.fd:640-644 `html_render` escapes every text node with `h_escape`, including the raw text of script/style: `<script>var x = 1 < 2;</script>` renders as `1 &lt; 2`, breaking the script. Bug; untested (test renders only a div).
- html.fd:160-181 only 19 named references; HTML5 has 2231, and legacy refs without ';' (`&copy`, `&amp`) are not decoded.
- html.fd:238-266 numeric references: no range checks (0, surrogates, > 0x10FFFF produce invalid UTF-8 via h_utf8), no Windows-1252 remap for 0x80-0x9F, overflow on a 30-digit number.
- html.fd:90-130 builder does not validate tag or attribute names (an attribute name with `"` or `>` injects markup); no helper for safe script/style content.
- html.fd:397-531 parser: no implied html/head/body, no adoption-agency handling of misnested formatting (`<b><i></b></i>`), no foreign content (svg/math), no CDATA; `</p>` with no open p is ignored rather than producing an empty p.
- Queries are by tag/id/class only; no selector engine, no parent/child/sibling helpers beyond the raw arena.
- html.fd:30-57, 211-276, 321-331, 495 build strings one char at a time with `+`, and `h_lower(rest)` lowers the whole remaining document for each raw-text element: O(n^2) on large pages.
Tests: test_std_html covers builder escaping, attributes, comments, references, implied tags, queries and a simple render. Untested: rendering of script/style, numeric reference edge cases, misnested markup.


### grpc
Complete means: gRPC framing, metadata, status and unary plus streaming calls over stdlib http2, with a ready way to run a client/server on an H2Conn.
- grpc.fd:27-30 streaming calls (server-streaming, client-streaming, bidi) are not driven: `grpc_server_handle` rejects anything but exactly one message (762-764) and the server API has no streaming handler type.
- No glue to stdlib http2's connection (H2Conn): the user has to run preface/SETTINGS/flow control and pass a stream's frames; a documented seam, but there is no helper even for the common "serve GrpcServer on an H2 connection" case.
- grpc.fd:297 `:scheme` is always "http" (no https for TLS channels).
- grpc.fd:314-318 `-bin` metadata says values are base64 but no helper encodes/decodes them; no reserved-header checks (users can add `:path`, `content-type`).
- Only identity and gzip encodings (751); no `grpc-accept-encoding` on requests; deadlines are parsed but not enforced (left to the handler).
Tests: test_std_grpc is good (framing, byte-at-a-time deframer, status mapping, timeouts, unary round trips, gzip, unknown service/method/encoding, 415, trailers-only, CONTINUATION, cut stream). Untested: 405 path, multiple messages in a unary request, reply trailers on error.

### hpke
Complete means: RFC 9180 with the common suites (at least X25519 and P-256 with AES-128-GCM, AES-256-GCM and ChaCha20Poly1305), all four modes with their setup functions, and the export-only AEAD.
- hpke.fd:1-3, 84-91: the suite is hard-wired to 0x0020/0x0001/0x0003. The AEADs AES-128-GCM (0x0001) and AES-256-GCM (0x0002) are missing, even though stdlib/aes_gcm.fd exists; this is the suite OHTTP/ECH/MLS commonly use. DHKEM(P-256) and the HKDF-SHA384/512 KDFs are missing too.
- hpke.fd:175 `hpke_key_schedule` accepts mode 3 (auth_psk), but there is no `hpke_setup_auth_psk_s/_r`. The header claims all four modes.
- The export-only AEAD (0xFFFF) is missing.
- hpke.fd:256: `seq` is never checked against the message limit (RFC 9180 §5.2 MessageLimitReachedError).
- hpke.fd:108: `hpke_derive_keypair` does not reject short ikm.
- Tests: psk and auth modes are only round-tripped and rejection-checked. They are not checked against the RFC 9180 A.2.2 (psk), A.2.3 (auth) and A.2.4 (auth_psk) vectors.


### iec61850
Complete means: object references, BER that encodes INTEGERs correctly and decodes untrusted frames safely, GOOSE and 9-2LE SV encode/decode, including the Ethernet framing for both and typed data values.
- iec61850.fd:144-152: the comment says a leading 0x00 is needed when the top bit is set, but `ber_uint_bytes` never adds it. confRev/stNum/sqNum of 128..255, 32768.., etc. therefore encode as negative INTEGERs to any conformant decoder.
- iec61850.fd:135-153, 157: negative values give an empty stack, so `ber_int(tag, -5)` emits a zero-length INTEGER. The header promises "unsigned/signed integer" encoders (line 12); there is no signed one.
- The header (line 13) promises an octet-string encoder; none exists.
- iec61850.fd:232-253: `ber_read` does not check the long-form length bytes or `content + length` against `b.len()`. `nbytes` can be up to 127, so the shift overflows. `goose_decode_pdu` (383-431) then calls `b.get` past the end (e.g. `t.content + 7` at line 408, `ber_str`). A malformed frame from the network panics.
- `goose_decode_pdu` does not check that the outer tag is 0x61. `goose_frame_parse` (462-472) does not check `length` against the buffer.
- There is no decoder for allData values (boolean, int, float32, bit-string, UtcTime): callers walk the TLVs by hand (see the test). `ber_float32` has no inverse.
- iec61850.fd:488-542: SV encodes and decodes only one ASDU (noASDU is fixed at 1). The optional datSet/refrTm/smpRate/smpMod are not supported. smpCnt/confRev/smpSynch use minimal-length ints, not 9-2LE's fixed 2/4/1 octets. seqData is opaque: the 9-2LE layout of 8 x (INT32 value, quality) has no encode or decode. There is no SV Ethernet header (0x88BA) builder or parser.
- GOOSE has no publisher state (stNum/sqNum rules, retransmission back-off, TAL).
- iec61850.fd:39-55: the FC list lacks OR, BL, SR, US, MS, LG and GO. Line 40 has an unused `known` variable. The f32 code duplicates iec104.fd.
- Test gaps: round trips use only small values. Nothing covers top-bit INTEGERs, negative ints, long-form lengths in decode, or malformed or truncated input.


### iec104
Complete means: APCI with full receive validation, typed encode and decode for the common monitor and control type ids (including SQ=1 and quality descriptors), CP56Time2a with its flags, and a k/w/t1-t3 state that detects sequence errors.
- iec104.fd:20-24: the header says "Encoders and decoders for each" type id. There are only encoders (`iec104_sp_na`, `me_nc`, `me_tf`, `sc_na`, `interrogation`). Decoding is raw `get_ioa`/`get_f32` by offset, as the test shows.
- Constants with no encoder: M_DP_NA_1, C_DC_NA_1, C_CI_NA_1, C_CS_NA_1 (lines 37-44). Missing outright: M_ME_NA/NB (normalized/scaled), time-tagged M_SP_TB_1/M_DP_TB_1, M_IT_NA_1 counters, C_SE setpoints, M_EI_NA_1 end of init.
- The builders always use SQ=0 with one object. There is no multi-object or SQ=1 sequence encoding or decoding, and no QDS/SIQ quality-bit helpers (IV, NT, SB, BL, OV).
- iec104.fd:137-169: `iec104_parse` does not check the APDU length (4..253) and does not check that a U-frame has length 4 and one of the six functions.
- iec104.fd:444-458: `iec104_on_recv_i` does not check `ns == vr`. A sequence error should close the connection; here it silently resyncs. N(R) is not checked to lie in [va, vs] (also line 466).
- iec104.fd:397-400: the doc mentions `unacked_at` per frame, but the struct has no such field. t1 is approximated by resetting `oldest_unacked_tick` to the receive tick on a partial ack (line 450). t1 for an unanswered STARTDT/TESTFR act is not tracked.
- iec104.fd:307-339: CP56 encode has no IV/SU bits; decode drops IV, SU and day-of-week.
- Test gaps: no malformed frames, no t2/t3 expiry, no sequence-number wrap at 32767, no sequence error.


### iso15118
Complete means: V2GTP and SDP for both sides, an EXI codec for both DIN 70121 and ISO 15118-2 (AC and DC), state machines for EV and EVSE that follow each standard's message order with per-message timeouts.
- iso15118.fd:54-61: no ISO 15118-2 grammar tables; the module's namesake protocol goes through a caller-supplied codec (`v2g_step`). This is declared, but it is the core of the standard. SalesTariff, xmldsig Signature, PaymentDetails, CertificateInstallation/Update, ChargingStatus and MeteringReceipt are refused (Plug & Charge is not possible).
- iso15118.fd:1279-1305: the ISO 15118-2 order is SessionSetup → ServiceDiscovery → PaymentServiceSelection → ChargeParameterDiscovery → PowerDelivery → CurrentDemand → ... It lacks Authorization, which is mandatory in -2. For DC it lacks CableCheck/PreCharge/WeldingDetection, and AC charging (ChargingStatus loop) has no profile.
- iso15118.fd:1273, 1355-1357: one `timeout` for every state. The header (line 41) promises per-state timeouts; the standard's per-message timeouts (e.g. CurrentDemand vs ChargeParameterDiscovery) and sequence timeouts are not modelled.
- iso15118.fd:1375: a timeout is noticed only when a late response arrives. There is no `v2g_timed_out(s, tick)` poll, so a missing response never fails the session.
- EVSE (SECC) side: builders for responses exist, but there is no SECC sequencer that validates incoming requests and sessions. There is no SDP request parser (only the request builder and the response builder/parser).
- Test gaps: DIN is very well covered (36 captured vectors, 8 builder reproductions, a full session). The ISO-2 profile is tested only for a wrong-message failure, and timeouts are not tested.


### http3

Complete means: RFC 9114 request/response over a QUIC connection without I/O, with control and QPACK streams, SETTINGS/GOAWAY, trailers, interim responses, the §4 message rules, and resource limits. Push, the QPACK dynamic table (it announces capacity 0, which is compliant), extended CONNECT, datagrams and priorities are left out as documented (http3.fd:20-23) and are not counted.

Gaps:
- http3.fd:986-1002: DATA is appended to `s.msg.body` with no size limit. QUIC stream flow control only paces the peer: the window reopens as h3_stream_run reads, so a peer can make a server hold an arbitrarily large body. http2 has H2_MAX_BODY; http3 has nothing like it, so a request larger than a cap should fail with H3_EXCESSIVE_LOAD or a 413.
- http3.fd:1254-1269, 1344-1374: a message is encoded whole, with one HEADERS frame, one DATA frame of the entire body, then trailers. There is no incremental body API for large or streamed responses or uploads.
- There is no handler-style server entry point (http2 has `http2_serve(port, handler)`). Serving needs the poll loop of http3_udp (h3_udp_server_poll / h3_udp_respond) and a translation to module http's HttpRequest written by hand. This is P3.

Test gaps: the suite test covers settings errors, critical-stream closes, frame-cut errors, GOAWAY, trailers and loss. The body-size behaviour and `h3_respond_interim` with real 103 hints have no visible assertions; I did not search exhaustively.


### gameloop
Complete means: a fixed-timestep loop as now, plus a sub-millisecond or injected clock, optional frame limiting, and correct FPS.
- gameloop.fd:74-75, 157: uses the `p_clock_now_ms` clock. Millisecond granularity jitters a 144 Hz step, and no clock can be injected for deterministic tests (the test works around this).
- gameloop.fd:94: the FPS timer accumulates the capped, truncated frame time, so FPS is wrong after a hitch.
- There is no sleep or frame-limit helper and no negative-delta guard.
- gameloop.fd:3, 12: header typos ("accumulator game gl", the usage loops on `gl.running` but names the variable `loop`).
- Tests: good within what the clock allows.


### haptics
Complete means: the current pattern/waveform/AHAP model, plus the Android composition API and curves applied on Android.
- haptics.fd:38-45: ANDROID_PRIMITIVE_* are constants only. No builder exists for `VibrationEffect.startComposition().addPrimitive(...)`, which the header (:12-13) claims ("composition primitives").
- haptics.fd:191-220: intensity curves (`p.curves`) are ignored by `haptic_android_waveform`, so a curve changes iOS output only.
- haptics.fd:301: curves are HapticIntensityControl only. HapticSharpnessControl and AHAP audio events are not modelled, and `haptic_parse_ahap` ignores the ParameterID of a curve.
- haptics.fd:210-217: `repeat_from_ms` silently stays -1 unless a segment starts exactly at that time.
- Tests: good for the API surface (73 lines touch every public function). Parsing malformed AHAP and curve validation are untested.


### framebuffer
Complete means: the current drawing and console layer, plus a fuller VT subset (erase line, cursor movement, bold to bright), a visible cursor and clipping everywhere.
- framebuffer.fd:604-617 (`con_csi_final`): only `m`, `2J` and `H`. Missing: `K`, cursor up/down/forward/back (A-D), `1`/bold mapped to bright colours, 38;5/38;2 colours, and saving/restoring the cursor.
- framebuffer.fd:276-294: `fb_copy_rect` doesn't clip ("Both must be on screen").
- framebuffer.fd:62-64: `fb_chan_pack` shifts by `8 - len`, which is negative for channels wider than 8 bits (30-bit 2:10:10:10 formats).
- framebuffer.fd:127-133: mapped framebuffers are written one volatile byte at a time. That works but is slow for clear and scroll (no word stores).
- UTF-8 bytes each draw as '?'. There is no cursor rendering.
- Tests: good (formats, pixels, blit, copy, scroll, console escapes).


### goap
Complete means: as now, with an admissible heuristic in all cases and a heap-based open list.
- goap.fd:267, 307: the heuristic is ceil(unmet / max_eff) * min_cost. It assumes each effect fixes at most one condition, but two unmet conditions on one fact (e.g. `x >= 2` and `x != 0` with x = 0) are both fixed by one SET, so h can overestimate and the plan may not be optimal.
- goap.fd:273-283: the open list is scanned linearly per expansion, giving O(n^2) in nodes.
- goap.fd:139: `cost > 0` is documented but not enforced (cost 0 makes h0 = 0; a negative cost breaks A*).
- There are no procedural or context preconditions. That is typical of GOAP and is optional.
- Tests: good (6 plans, simulate, state keys).

### gcode
Complete means: an RS274/Marlin interpreter front end that surfaces every command to the caller (motion, dwell, M codes), keeps the common modal groups, and has the generator.
- gcode.fd:344-349: M codes other than M82/M83 are silently ignored. The caller gets no event for M3/M5 spindle, M104/M109 temperatures, M106 fan, M0/M2/M30 program end and the like, so it must re-parse the line itself.
- gcode.fd:338, 384-386: G4 dwell returns no moves, and its P/S duration is lost.
- gcode.fd:340-342: G53-G59 (work coordinate systems), G10, G40-G43 (cutter/tool length compensation), G80-G83 canned cycles and G93/G94 feed modes are all rejected as "unsupported".
- gcode.fd:370-383: G28 always homes to 0. There are no stored reference positions (G28.1/G30).
- gcode.fd:350-351: F is scaled by the unit as soon as it is read, so `F10 G20` on one line uses the old unit (the word order should not matter).
- Tests are good. Untested: G19 arcs, G28, G4, arcs under G91, and the "unsupported G" error.


### gpio
Complete means: the pure model plus a usable Linux cdev seam (request, reconfigure, get/set, events with timeout, chip and line info). The sysfs path stays.
- gpio.fd:92-94: the `GPIO_V2_LINE_SET_CONFIG` ioctl number is defined, but no function reconfigures a live request (for example switching direction).
- gpio.fd:84-86: the chipinfo ioctl is never used. There is no `gpio_linux_chip_info` and no line info or watch (`GPIO_V2_GET_LINEINFO_IOCTL`), so there is no way to find lines by name.
- gpio.fd:446-476: every line in a request shares one config. The v2 ABI allows per-line attributes (masks), but only the output values and debounce use masks.
- gpio.fd:581: `(1 << n) - 1` overflows for n = 64, which GPIO_V2_LINES_MAX allows.
- gpio.fd:596-611: `gpio_linux_read_events` blocks with no timeout and no non-blocking variant (poll).
- Tests are good for the fake chip, debouncer, request image and event decode. Untested: every `@cfg(linux)` function (it could run under gpio-sim), `gpio_sysfs_apply`, `gpio_sysfs_read` and `gpio_sysfs_write`.


### geolocation
Complete means: correct geodesy everywhere on the globe (dateline and poles), direct and inverse problems, geohash, geofences and filtering.
- geolocation.fd:352-370: ray casting runs in raw lat/lon, so a polygon that crosses ±180° or contains a pole is wrong. geolocation.fd:381-383 (edge distance) uses `lon - p.lon` without wrapping.
- geolocation.fd:470-471: the Kalman filter averages longitude linearly, so fixes at 179.99° and -179.99° pull the estimate to about 0°.
- Vincenty's direct problem (destination on the ellipsoid) is missing. Only the spherical `geo_destination` exists.
- Cross-track/along-track distance, polygon area, bounding box/radius search and ECEF/UTM conversion are all missing.
- geolocation.fd:232: `geohash_encode` does not clamp precision to 1..12 as its doc says.
- Tests are good. Untested: `geo_dms`, polygon hysteresis near an edge, and every antimeridian case.


### forecast
Complete means: SES/Holt/Holt-Winters/ARIMA point forecasts, prediction intervals, fitting and metrics, with errors instead of panics on short series.
- forecast.fd:430, 569: `sigma2` is computed but not used. No function gives prediction intervals for any model.
- Holt has no damped-trend variant, and there is no ETS model selection.
- forecast.fd:513: ARIMA is non-seasonal only (no SARIMA). There is no automatic order selection (AIC/BIC), and MA coefficients come only from Hannan-Rissanen, with no likelihood refinement.
- forecast.fd:76, 143-144: an empty or one-point series panics on `Vec.get` in `fc_ses_smooth`/`fc_holt`. forecast.fd:173 panics instead of returning an error for Holt-Winters on fewer than two seasons.
- Tests are good. Untested: `fc_fit` for FC_HOLT and FC_HW_ADD, ARIMA with both p > 0 and q > 0, d = 2, backtesting of HW and ARIMA, and `fc_one_step` for Holt directly.


### historian
Complete means: tag archive with exception and swinging-door compression, quality flags, step tags, retrieval, aggregates and a persistence hook.
- historian.fd:116-118: a sample at or before the last archived time is silently dropped and not counted. Neither the received/passed counters nor an error report it.
- historian.fd:36-63: samples carry no quality/status, so a bad or substituted value cannot be stored.
- historian.fd:241-257: interpolation is linear only. Step (digital or state) tags are missing, as is "value at or before" retrieval.
- historian.fd:83: `description` is always "" and no function sets it.
- There is no archive serialization or retention/trim. The header leaves persistence to the caller, but gives no format.
- Aggregates have no standard deviation, range, total, time in state or percent-good.
- Tests are good (ramp, step, sine error bound, aggregates). Untested: `historian_flush` on a held sample followed by more data, and out-of-order input.


### hpet
Complete means: HPET discovery and programming for every timer kind (32- and 64-bit, IO-APIC and FSB), TSC calibration and the PIT. Most of this is there.
- hpet.fd:56-58: `hpet_tn_fsb` is defined, but no function programs FSB/MSI delivery (HPET_TN_FSB_EN plus the address/data register).
- hpet.fd:276-283: `hpet_oneshot` does not mask the target to 32 bits for a timer without SIZE_CAP (or in TN_32MODE). It also does not detect a deadline the counter has already passed (Linux's `-ETIME` check), which loses the interrupt.
- hpet.fd:401-403: `pit_ch2_latch` divides by zero for ms > 1000. Above about 54 ms the latch exceeds the 16-bit counter without a check.
- Tests are good. Untested: `hpet_restart`, and 32-bit counters.

### i2c
Complete means: messages, 7/10-bit addressing, register helpers, the SMBus transactions (with PEC and block reads), and a Linux transport that reports errors faithfully.
- No SMBus helpers beyond the PEC CRC: no quick, read/write byte/word data with PEC appended or checked, or block read with a count byte. `I2C_M_RECV_LEN` (line 62) is declared, but neither the fake bus (323-380) nor `i2c_rdwr_image` (670-694) handles it.
- i2c.fd:745-748: the Linux transport returns -5 (EIO) for any ioctl failure. errno (ENXIO for a NAK, ETIMEDOUT, ...) is lost.
- No I2C_FUNCS query, no I2C_SMBUS ioctl path (constant at line 56 only), and no I2C_TIMEOUT/I2C_RETRIES setters.
- Test gaps: everything pure is tested. `i2c_linux_xfer`/`open`/`close` are untested (Linux hardware).


### iap
Complete means: products and prices, the purchase state machine, App Store Server API (transactions, renewal info, notifications, history) and Play Developer API shapes, with signature checks as seams.
- No JWSRenewalInfoDecodedPayload decoding (autoRenewStatus, gracePeriodExpiresDate, expirationIntent). `iap_subscription_status` (348) therefore needs `billing_retry` and `grace_until_ms` from the caller, although the notification already carries `signed_renewal` (485, 494).
- iap.fd:611-627: SUBSCRIPTION_STATE_PAUSED and _PENDING (and _PENDING_PURCHASE_CANCELED) fall through to "expired".
- iap.fd:376-380: there is a history URL with revision, but no parse of the response's `signedTransactions`/`hasMore`/`revision` for paging.
- iap.fd:240-270: a REFUNDED purchase has no exit, so a refunded consumable can never be bought again.
- Test gaps: good coverage. Not covered: refund of a consumable followed by a rebuy, and Play states other than CANCELED and IN_GRACE_PERIOD.


### imu
Complete means: Madgwick and Mahony AHRS (6/9-axis), gyro bias, accel and mag calibration (hard and soft iron), heading and gravity removal. The header's promises are all met and tested.
- imu.fd:296-305: soft iron is axis-aligned only (documented). There is no ellipsoid fit or 3x3 soft-iron matrix, which is what most magnetometers need. It divides by zero when an axis range is 0 (an axis not excited, or no samples: lo/hi stay at ±1e300).
- No initial attitude from accel+mag. `imu_tilt_from_accel` (334) gives roll/pitch only, so the filters start at identity and must converge.
- No gyro-drift compensation term (Madgwick's zeta) in `madgwick_update`. Mahony's integral term covers bias only for that filter.
- Test gaps: good. Edge cases are untested (zero accel and mag inputs, a degenerate mag_cal).


### initrd
Complete means: newc/crc cpio read and write, and unpacking any initramfs the kernel accepts (including compressed images) safely into a vfs.
- The kernel accepts gzip/xz/lz4/zstd-compressed initramfs (the usual form). initrd only reads raw cpio, though stdlib/compress.fd has inflate. A user handing it a real `/boot/initrd.img` gets "bad magic".
- initrd.fd:311-326, 378: `initrd_clean` strips only leading "/" and "./". An entry named "../x" or "a/../../x" is joined to `dest`, and vfs resolves `..` (vfs.fd:337-348), so unpacking can write outside `dest`.
- initrd.fd:394-420: hard links are written as separate copies (vfs has no link). mtime, uid and gid are not applied. An existing directory keeps its old mode (386). These are partly vfs limits, documented for devices only.
- Test gaps: good. Not covered: path traversal, a truncated header in the middle, and junk after the trailer.

### json
Complete means: parse/validate, serialize with escapes, unicode, safe error
paths, bounded resources on hostile input, efficient building.
- json.fd:347-405 j_scan_arr_n/j_scan_obj_n recurse through j_scan_val_n with no
  depth limit; json_is_valid (used by every json_* call) overflows the stack on
  deeply nested input. Only json_index (jd_value, :1370) has JSON_MAX_DEPTH.
- json.fd:801-866 json_get/json_at/json_type/json_len call json_is_valid on the
  whole text each time: k lookups cost O(k*n).
- json.fd:641, :670 json_string and json_unescape build output with `out = out + ...`
  per byte (quadratic); json_obj_set/json_arr_push (:1226, :1261) copy the whole
  text per call, so building an n-element array is O(n^2).
- json.fd:1278 json_num takes i64 only: no f64 number builder.
- json.fd:761 a lone high surrogate is encoded as CESU-style UTF-8 (invalid UTF-8).
Tests: good (test_json, test_json2, test_lib_json, test_json_swar,
test_stdlib_jsondoc_loaders); untested: deep nesting, large documents.

### math
Complete means: the libm functions (trig, inverse trig, rounding, logs), and
correct vector/matrix/quaternion helpers.
- math.fd:637 perspective: `f = 1.0 / sin(fov/2)`; must be cos/sin (1/tan). At
  fov 90 degrees the scale is off by sqrt 2.
- math.fd:655-667 look_at: the rotation part is the transpose of the row-major
  convention the rest of the module uses (m01 = u.x where it should be s.y);
  with eye (5,0,0) looking at the origin, point (0,0,1) maps to x = +1 instead
  of -1.
- math.fd:19-29 only sqrt, sin, cos, floor, ceil, pow, fabs, log, exp: no tan,
  atan, atan2, asin, acos, sinh..., round, trunc, fmod, hypot, log10, log2,
  cbrt, min/max for i64, isnan. geolocation.fd:48, motion.fd:141, quat.fd:77,
  spatial_audio.fd:129 each carry their own atan series.
- math.fd:600 slerp is nlerp (says so; points to stdlib/quat).
- No mat4 inverse/transpose/determinant.
- math.fd:1-7 header still describes "hand-encoded x86-64 implementations".
Tests: thin. test_stdlib_math.fd covers 5 scalar functions; perspective,
orthographic, look_at, rotation_*, to_mat4 are called by no test.

### mesh
Complete means: OBJ load (polygons triangulated, all index forms), primitives,
normals (flat and smooth), transforms.
- mesh.fd:134-172 and :199-202: every Face literal in mesh_create_cube and
  mesh_create_plane writes `v1:` twice and omits `v2`; run shows faces
  "0 2 0", "0 3 0", "1 6 0" and a zero normal. The same pattern at :291 is
  harmless only because the parser then sets v2.
- mesh.fd:289-296 faces take parts 0..2 only: quads and n-gons are not
  triangulated though the header says "triangulated"; negative (relative)
  indices give wrong indices.
- mesh.fd:249, :262, :288 str_split(..., " ") breaks on repeated spaces or tabs
  (empty fields parse as 0).
- mesh.fd:88 doc says 24 vertices, the cube has 8; mesh_compute_normals (:394)
  is flat only and replaces normals the OBJ supplied; no smooth normals,
  no materials (mtllib/usemtl), no OBJ writer.
Tests: thin (test_stdlib_repaired.fd checks the cube's triangle count only).

### medical
Complete means: HL7 v2 parse/build with delimiters, escaping, components and
ACKs; the promised FHIR JSON resource helpers; code validators that check the
format.
- medical.fd:162, :167, :177, :180 timestamp "20260826120000", app "HMS",
  doctor "DR001" and msg_id = patient_id are hard-coded in the message builders.
- medical.fd:181 hl7_adt_a03 writes a fake PID ("DISCHARGED", "19000101", "U").
- medical.fd:208 hl7_parse never reads MSH-9: msg_type is always ADT_A01;
  receiving app/facility are not parsed.
- medical.fd:206 CRLF input ("\r\n" -> "\n\n") and a trailing separator add empty
  segments.
- medical.fd:239 hl7_get_field counts MSH fields one off (MSH-1 is the field
  separator itself); no component (^), repetition (~) or escape (\F\ \S\ \T\
  \R\ \E\) handling; values with '|' corrupt messages.
- medical.fd:137 hl7_msh_segment slices the type code at fixed offsets
  ("UNKNOWN" gives "UNK^NOW").
- medical.fd:252-276 "FHIR (JSON-based) resource helpers" are a name table; no
  resource is built or read.
- medical.fd:281 icd10_validate checks only the first three characters (no dot
  position, no 4th-7th character rules).
- No ORU^R01 builder although the type exists; no ACK/NAK.
Tests: thin (test_lib_medical.fd, test_medical.fd).

### navmesh
Complete means: a navmesh built from polygons (shared-edge adjacency), point
location, A* over polygons, path smoothing (funnel); an efficient grid A*.
- navmesh.fd:263-287 NavPoly has a center and neighbor ids only: no vertices,
  no build from geometry, no point-in-polygon locate, no funnel/string pulling;
  links are manual (navmesh_connect).
- navmesh.fd:128-145 grid A* scans all width*height cells to find the best
  open node each iteration (O(cells^2)); no binary heap.
- navmesh.fd:137 heuristic in unit steps while costs are 10/14: admissible but
  ~10x too weak (near Dijkstra).
- navmesh.fd:66 nav_grid_index packs x*10000+y: wrong for heights >= 10000.
- navmesh.fd:122 start outside the grid indexes out of range; a blocked start
  is not rejected.
- navmesh.fd:313-411 polygon A* uses linear open/closed lists.
Tests: thin (test_lib_navmesh.fd, small grids).

### markdown
Complete means: CommonMark core (lists incl. nesting, code blocks, emphasis,
links incl. reference style, hard breaks), GFM tables, safe output.
- markdown.fd:248, :238 link href and img src are only HTML-escaped:
  `[x](javascript:alert(1))` produces a working javascript: link, contrary to
  the header (:10-11). No scheme allow-list.
- markdown.fd:9-11 tables, footnotes, nested lists, reference links not
  supported; also missing setext headings, 4-space indented code, hard line
  breaks, strikethrough, fence info string (language class dropped, :420).
- markdown.fd:269-298 link parts packed with \x07 and a 4-digit length
  (int_to_str_padded(..., 4)): a link destination over 9999 bytes breaks.
- markdown.fd:22-41 and md_inline build strings byte by byte (quadratic).
Tests: none (tools/site.fd uses the module).

### jsonschema
Complete means: 2020-12 validation vocabulary incl. dependent*, unevaluated*,
local $ref/$anchor, safe on recursive schemas.
- jsonschema.fd:23-25 no dependentRequired, dependentSchemas,
  unevaluatedProperties/Items, $anchor, $dynamicRef, format.
- jsonschema.fd:170-173 `$ref` is followed with no cycle or depth guard
  (`{"$ref":"#"}` recurses forever).
- jsonschema.fd:276 multipleOf via `q as i64` is wrong for large quotients.
- Every keyword lookup goes through json_has/json_get_safe on schema text
  (each a full validation).
Tests: thin (123 lines; no recursion, no dependent keywords).

### log
Complete means: levels, structured fields, text/JSON, configurable sinks
(stdout/stderr/file/custom), timestamps with ms.
- log.fd:302-316 log_emit has three fixed sinks; no callback/writer sink, no
  rotation or size limit.
- log.fd:183-190 time_to_iso of seconds: no millisecond precision.
- log.fd:276 logfmt keys are written raw (a key with a space or '=' breaks the
  line).
- log.fd:270-281, :291-310 quadratic `line = line + ...`.
(Module levels keyed by `module as i64` were probed with a built string and
work under fid run.)
Tests: good.

### metrics
Complete means: counters, gauges, histograms (summaries optional) with labels,
float values, correct exposition escaping.
- metrics.fd:18-30 no label support: one series per name.
- metrics.fd:136-167 values are i64 (no float gauges or seconds histograms).
- metrics.fd:186 HELP text not escaped (\\ and \n).
- metrics.fd:215 one module-global served Metrics.
Tests: thin (28 + 24 lines).

### mmap
Complete means: map a file (ro/rw, offset, length), anonymous maps, flush,
unmap, on every backend; bulk copies.
- p_mmap_file/p_mmap_anon/p_munmap/p_msync/p_mmap_len exist only in
  compiler/codegen_c.fd:14565-14607: not in compiler/interp.fd, pe_runtime or
  linux_runtime, so `fid run` and `fid emit` builds cannot use the module.
- mmap.fd:30 no read-only, offset or length arguments; mmap.fd:116-134 copies
  byte by byte through p_volatile_load8/store8.
- mmap.fd:100-105 mmap_get returns 0 out of range (indistinguishable from a 0 byte).
Tests: none.

### net
Complete means: UDP and TCP (connect, listen/accept), name resolution, IPv4/6,
socket options, and a usable reliable layer.
- net.fd:66 net_addr_create takes an IPv4 literal (inet_addr): no hostname
  resolution, no IPv6.
- No listen/accept in the module (TCP servers go through p_socket_* elsewhere).
- net.fd:118-121 net_set_reuseaddr is a no-op (bind sets it); SO_BROADCAST is
  defined (:53) but cannot be set.
- net.fd:482-486, :670-686 retransmission timeout fixed at 200 ms and never
  derived from the RTT estimate; no retry limit or dead-peer detection; no
  ordered delivery; received_ids scanned linearly.
- pledge_net_* is C runtime + linux_runtime/sys.fd only (no pe_ir Windows native).
Tests: good (test_lib_net, test_stdlib_net_reliable, test_world_sockets).

### lwm2m
Complete means: the device side of Registration, Device Management and
Information Reporting (Observe/Notify with pmin/pmax), TLV/SenML.
- No Observe/Notify, Write-Attributes or Cancel Observation anywhere in
  lwm2m.fd (grep: no "observe"); no Create (POST on an object).
- cbor imported but SenML-CBOR not produced (one cbor_ reference).
- DTLS, bootstrap, Security object: out of scope (documented, lwm2m.fd:21-22).
Tests: good.

### matter
Complete means: TLV, data model, the Interaction Model (Read, Write, Invoke,
Subscribe), onboarding payloads; sessions may be a documented seam.
- matter.fd:14-19 only ReadRequest/ReportData: no WriteRequest, InvokeRequest
  (cluster commands), SubscribeRequest (grep finds none).
- matter.fd:23-27 PASE/CASE, AES-CCM message layer, MRP: out of scope
  (documented), so the module cannot talk to a real device on its own.
Tests: good.

### mavlink
Complete means: v1/v2 framing, signing, a parser, and a message model able to
express all of common.xml.
- mavlink.fd:76-145 field types have no array length: char[N]/float[N] fields
  cannot be defined, so STATUSTEXT, PARAM_VALUE/SET, MISSION_ITEM_INT,
  ATTITUDE_QUATERNION etc. are impossible (mavgen also feeds the array length
  into CRC_EXTRA).
- mavlink.fd:202-305 seven messages defined; no XML dialect loader.
Tests: good.

### middleware
Complete means: the listed layers, safe under load.
- middleware.fd:553-575 rate-limit buckets live in parallel Vecs searched
  linearly and never evicted: every new key header value adds a bucket forever
  (memory growth an attacker controls).
- middleware.fd:290-400 (mw_deflate, mw_gzip, mw_zlib) a second DEFLATE compressor; compress.fd:1443
  deflate_compress already exists.
- No client-address keying (header only); no request body size limit.
Tests: good.

### mysql
Complete means: protocol 4.1 client with auth, queries, prepared statements,
TLS, multi-results.
- mysql.fd:40-41 no CLIENT_SSL/TLS (postgres_tls.fd shows the pattern), no
  compression, no multi-resultsets past the first, no LOAD DATA LOCAL.
- mysql.fd:140, :214 own SHA-1 and SHA-256 while kdf and crypto are imported.
- mysql.fd:760-781 parameters: LONGLONG, DOUBLE, strings, NULL only (no BLOB or
  date/time parameter types).
Tests: good (808 lines).

### mongodb
Complete means: BSON, OP_MSG, CRUD with cursors, SCRAM, TLS.
- mongodb.fd:44-47 no TLS (required by Atlas and most deployments), no
  OP_COMPRESSED, no sessions/transactions/retryable writes, one server only.
- mongodb.fd:36-38 SASLprep without NFKC.
- No helpers for createIndexes/findAndModify/distinct (mongo_command is generic).
Tests: good.

### money
Complete means: exact amounts with conservation, formatting, allocation
without losing cents.
- money.fd:212-222 Money_print for -5 cents prints "$0.0-5", for -150 "$-1.-50".
- No Money_split(n)/allocate by ratios (the conserving operation users need),
  no to_str/parse, no currency.
- money.fd:173 Money_take lets the source go negative.
Tests: good (8 tests incl. 6 must-fail).

### kv_cache
Complete means: create/append/get K and V/len/evict/free with checked inputs.
- kv_cache.fd:24-51 thin wrappers over p_kv_cache_* (runtime real:
  pe_runtime/tensor.fd:1748, codegen_c, interp); no kv_cache_free (the registry,
  tensor.fd:1721 pet_kv_registry, only grows); no separate K or V getter.
- tensor.fd:1771 append to an out-of-range layer is silently ignored.
Tests: good (test_kv_cache, test_kv_cache_fail, test_ml_edge_cases).

### jose
Complete means: JWS/JWK/JWT/JWE for the RFC 7518 algorithms people meet.
- jose.fd:17-20 not supported: RS384, PS384, ES256K, ECDH-ES+A*KW, PBES2,
  A*CBC-HS*, zip, JSON serializations.
- jose.fd:718-726 jwt_num uses str_parse_int: a fractional or exponent NumericDate
  ("1.7e9") reads as 1 and the token is rejected as expired.
Tests: good.

### kdf
Complete means: HMAC, HKDF, PBKDF2, scrypt (Argon2 lives in argon2.fd).
- kdf.fd:228-256 each PBKDF2 iteration rebuilds and rehashes the ipad/opad
  blocks (no precomputed state): about twice the work per iteration.
- kdf.fd:228 iterations < 1 behaves as 1 instead of failing.
- No encoded hash/verify helper (PHC "$pbkdf2-sha256$...", "$scrypt$...").
Tests: good.

### log_compact
Complete means: as the header says, plus a decoder robust to bad input.
- log_compact.fd:171-186 logtable_import reads r.bytes.get(r.pos) and the
  string bytes without length checks: a truncated table panics.
- log_compact.fd:3-6 "built at compile time" is aspiration: the table is built
  at run time.
Tests: good.

### lorawan
Complete means: a 1.0.x MAC with the regional plans users deploy in.
- lorawan.fd:790-830 only EU868 data rates/payload limits (no US915, AS923, ...).
- No LoRaWAN 1.1 key hierarchy, class B/C, rejoin.
- lorawan.fd:144 own AES inverse cipher (shared need with jose).
Tests: good.

### lp_solver
Complete means: LP/MIP with general bounds.
- lp_solver.fd:1-7 every variable is x >= 0; free or negative-bounded variables
  must be split by the caller; no lower bounds API.
Tests: good.

### lqr
Complete means: dense matrix helpers plus discrete and continuous LQR.
- lqr.fd:9-20 discrete only: no CARE/continuous lqr(), finite-horizon LQR or
  cross-weight N; DARE by plain fixed-point iteration (slow near marginal
  stability).
Tests: thin (103 lines).

### mdns
Complete means: messages, DNS-SD records, responder and querier per RFC 6762.
- No probing and conflict resolution (RFC 6762 §8), no announcement schedule,
  no querier cache with TTL expiry, no NSEC negative answers.
- mdns.fd:22-26 no multicast join available in stdlib net (documented).
Tests: good.

### metal
Complete means: device, buffers, compute and render pipelines, textures,
encoders, error messages.
- No render command encoder, render pass descriptor or draw calls (a render
  pipeline can be made but not used); no textures.
- metal.fd:135 mtl_nsstring leaks the mtl_cstr copy; metal.fd:245 leaks both
  mtl_size_mem blocks per dispatch.
- metal.fd:15-17 MTLSize-by-pointer only works on AArch64 (documented).
- NSError text not extracted from error_slot.
Tests: good for the pure part; NOBUILD in the C sweep (needs Metal.framework).

### migrate
Complete means: plan/run/status plus loading a migrations folder and safe
concurrent runs.
- No loader for a directory of NNNN_name.sql files (migrate_parse_file is per
  file).
- No lock (advisory lock / lock row) against two deployers.
- migrate.fd:161 the applied() seam is "a|b|c|d" lines: a name with '|' breaks.
- No ready MigDb binding for stdlib postgres/sqlite.
Tests: good.

### modbus
Complete means: RTU/TCP (ASCII optional), common function codes, data-type
helpers.
- No ASCII framing, no FC 22 (mask write), 43/14 (device identification), 7/8.
- No helpers to read/write 32-bit ints and IEEE floats across two registers
  with word order.
Tests: good.

### motion
Complete means: sensor processing plus a usable platform read path.
- motion.fd:31-41 the NDK functions are declared but no function uses them:
  there is no API to open, enable or poll sensors.
- motion.fd:269-272 motion_compass_point with a negative heading indexes
  names.get(negative).
- motion.fd:141-168 own atan/atan2 (math has none).
Tests: good.

### multiboot
Complete means: walk the Multiboot2 tag list safely and decode the common tags.
- multiboot.fd:247, :267, :285 a tag whose size field is 0 never advances `o`:
  infinite loop on a corrupt mbi.
- multiboot.fd:187-189 mb_u32 does no bounds check (panics past the end).
- No decoders for modules (3), basic meminfo (4), boot device (5), ACPI RSDP
  (14/15), EFI tags; tag ids are functions instead of consts.
Tests: thin (a few lines in test_os_stdlib.fd).

### mime
Complete means: extension <-> type, Content-Type parsing (quoted params),
classification; sniffing optional.
- mime.fd:223-227 every lookup builds both 80-entry tables again.
- mime.fd:278 mime_param splits on ';' inside quoted values
  (boundary="a;b").
- No content sniffing or Accept matching.
Tests: good (test_lib_mime.fd).

### mysql_db
Complete means: JSON rows and a Database binding that reports errors.
- mysql_db.fd:226 (Database.query via mysql_query_json): a failed query returns "[]", the same
  as an empty result.
- tinyint(1)/BIT not mapped to JSON booleans.
Tests: good.

### nmea
Complete means: framing plus the sentences receivers commonly emit.
- No GLL, ZDA (date/time), GST (accuracy), HDT; "!" is accepted but AIS is not
  decoded.
Tests: good.

### memo
Complete means: a memoizer with any hashable key, O(1) lookup, optional bound.
- memo.fd:86-90 linear scan per lookup (O(n)); keys and values are i64 only;
  no capacity/eviction; the file has no `module memo;` line.
Tests: thin (42 lines).

### parallel
Complete means: data-parallel helpers over real threads (par_map/par_for with a function value, reductions, chunking by core count), falling back to sequential where threads cannot run.
- parallel.fd:59-66 `par_map(n)` takes no function and returns `i*i`: the documented "apply a function to each element" is fake.
- parallel.fd:81-87, 106-112: par_sum/par_dot always split in 2, whatever the number of cores; i64 only.
- parallel.fd:129-150 par_fill/par_range sequential (stated); parallel.fd:167-168 `parallel_hint` is a no-op.
- parallel.fd:152-165 `is_parallelizable` checks a bit mask the compiler never produces.
- No par_for, par_reduce with a closure, par_sort, worker-count control.
Tests: test_parallel.fd and test_parallel_concurrent.fd exercise sum/dot/fill/range and the faked par_map(5) output; nothing exercises a large (threaded) input on every backend.

### os
Complete means: environment get/set/unset/list, args, exit, pid, cwd/chdir, real hostname/user/home/temp/OS name per platform, the platform's path and list separators.
- os.fd:121-128 `os_name` returns $OS or "linux" (macOS reports "linux").
- os.fd:140-146 `os_cwd` reads $PWD (stale after chdir, unset under cmd.exe; "." fallback).
- os.fd:149-160 `os_user` reads USER/LOGNAME, not USERNAME on Windows; os.fd:131-137 home from $HOME, not USERPROFILE.
- os.fd:162-168 `os_hostname` reads $HOSTNAME, "localhost" otherwise (wrong on most systems).
- os.fd:189-195 `os_path` splits on ":" and os.fd:219-226 separators are hard-coded "/" and ":" (wrong on Windows; and "C:\x" splits).
- No setenv/unsetenv, environment listing, chdir, getpid (sys.fd has getpid), os_getenv also defined in env.fd:64 (two modules, same names).
- os.fd:28-30 declares printf, unused.
Tests: test_lib_os.fd only checks values are non-empty; no check against the real system.

### queue
Complete means: a generic FIFO (`Queue<T>`) with Option-returning dequeue/peek, len, clear, iteration, drain.
- queue.fd:29-31 `Queue<T>` stores `Vec<i64>`; every method takes/returns i64, T is unused.
- queue.fd:44-56, 59-64 dequeue/peek return 0 when empty: indistinguishable from a stored 0.
- No iterator/for-each, no capacity hint. collections.fd's Deque has the same i64-only shape.
Tests: test_lib_queue.fd covers order, compaction, contains, drain; nothing for non-i64 elements.

### ota
Complete means: manifest with real SHA-256 and a verified signature, version comparison, a session that verifies payloads, a diff/patch pair that round-trips, and a storage seam (A/B slots) for apply/commit/rollback.
- ota.fd:235 `ota_session_verify` hashes with FNV-1a (`ota_hash_hex`, ota.fd:32-48, 16 hex digits) and compares with the manifest's "sha256:" value; a real SHA-256 can never match. The test enshrines it (test_lib_ota.fd:46 "sha256:a430d84680aabd0b"). ota_verify_payload (ota.fd:136) does use SHA-256: two inconsistent verifications.
- No signature verification anywhere (header step 2; `signature` field never read).
- ota.fd:170, 176 version compatibility by `str_compare` ("1.10" < "1.9"); semver.fd exists.
- ota.fd:296-334 `ota_binary_diff` records offset/length only, no data, so no patch can be applied; no apply function; `DiffEntry` (ota.fd:289) unused; a new image shorter than the old is not represented.
- ota.fd:20 header calls `ota_apply(manifest)`, which does not exist; ota_session_apply/commit/rollback (ota.fd:251-266) only set the state.
Tests: test_lib_ota.fd (191 lines) is good on the state machine but asserts the FNV behaviour.

### quant
Complete means: real int8 (and the promised int4) quantized tensors with scale/zero-point, per-channel parameters, a quantized matmul that accumulates in int32 and rescales, on every backend.
- quant.fd:4-9 promises q8/q4 types, the `~>` cast and memory savings; codegen_c.fd:15497-15505 stores the quantized values as `double` (no memory saved), there is no q4.
- quant.fd:34-40 promises VNNI/SDOT/GPU kernels; codegen_c.fd:15515-15523 is a plain double triple loop, and it ignores scale and zero point, so `qmatmul(q, q)` returns products of integer codes, not the f32 result the doc promises.
- quant.fd:43-46 `find_quant_params` returns "scale,zero_point" as text; zero point is always 0 (symmetric only); no per-channel.
- Runtime exists only in the C backend and the interpreter (no pe_ir/nat_ir symbols found).
Tests: test_ml_edge_cases.fd covers round-trip only.

### ocpp
Complete means: OCPP 1.6J core profile (all ~28 messages with builders and readers), the smart-charging and remote-trigger messages, a 2.0.1 equivalent set, and a central system that answers every charge-point-initiated message.
- ocpp.fd:179-252 builders only for Boot, Heartbeat, Authorize, Start/StopTransaction, StatusNotification, MeterValues, RemoteStart/Stop and 2.0.1 TransactionEvent; missing Reset, ChangeConfiguration, GetConfiguration, ChangeAvailability, UnlockConnector, DataTransfer, TriggerMessage, SetChargingProfile, ClearCache, firmware/diagnostics notifications, 2.0.1 Boot/Authorize/StatusNotification.
- The header's "payload readers" do not exist; callers use json_get_str by hand.
- ocpp.fd:330-356 `cs_handle` answers MeterValues, StatusNotification and DataTransfer with CALLERROR NotImplemented (a real charge point sends them at boot and while charging); StopTransaction ignores the transaction id.
- ocpp.fd:224-228 MeterValues takes one energy sample only.
- ChargePoint models one connector.
Tests: test_std_ocpp.fd is 82 lines.

### openadr
Complete means: OpenADR 2.0b VEN side: party registration (create/created/cancel), events with several signals and several events per payload, opt, report register/update/cancel, oadrResponse, ISO 8601 durations to find the active interval.
- openadr.fd:334-338 `ven_next_request` returns "oadrCreatePartyRegistration" but there is no builder or parser for it (nor oadrCreatedPartyRegistration).
- No oadrUpdateReport / oadrCancelReport (telemetry cannot be sent); no generic oadrResponse parser.
- openadr.fd:84-97 `OadrEvent.signal` is a single signal, despite "one or more signals" (header line 10); one event per oadrDistributeEvent.
- openadr.fd:238-243 `openadr_signal_value_at` takes an interval index; the header's "for a given time offset" needs duration parsing that is absent.
- ven_register (openadr.fd:328) just sets a flag.
Tests: test_std_openadr.fd 90 lines.

### plot
Complete means: line/bar/scatter (and histogram) with axis ticks and numeric labels, several series with a legend, configurable size and colours, escaped text.
- plot.fd:85-109 `plot_frame` draws two axes and two labels: no tick marks, no values, so a chart has no scale.
- plot.fd:79, 102, 107, 273, 330, 334: title, axis labels, bar labels inserted unescaped into SVG/HTML ("<" or "&" breaks the file; injection in HTML output).
- One series per chart, fixed size (PLOT_W/PLOT_H) and colours, no legend, no histogram/area.
- plot_debug_* (plot.fd:112-135) are test hooks in the public API.
Tests: test_plot.fd is 45 lines.

### opcua
Complete means: a client (and server side) for the common services: Browse, Read, Write, Call, CreateSubscription/MonitoredItems/Publish, GetEndpoints, with the current security policies.
- Header (opcua.fd "Not done"): services other than Read and the session services; Browse, Write, Call and subscriptions are what most OPC UA clients use.
- Only None and Basic256Sha256 (deprecated by the spec); no Aes128_Sha256_RsaOaep / Aes256_Sha256_RsaPss; no RSA > 2048 in OPN; no X.509 identity tokens.
- Variant arrays one-dimensional only; opcua.fd:740 other Variant types rejected.
Tests: test_std_opcua.fd + test_std_opcua_secure.fd (826 lines): good for what exists.

### os_sandbox
Complete means: build the policies (seccomp, Landlock, job objects) and apply them to the current or a child process through a declared seam, plus a complete syscall-name table per architecture.
- os_sandbox.fd:7, 26-28: the "extern seam" exists only as a comment; no function installs a seccomp filter (prctl/seccomp), creates a Landlock ruleset, or assigns a job object. The module cannot confine anything by itself.
- os_sandbox.fd:507-512 `ll_path_beneath_attr` always writes fd -1; the caller must patch bytes.
- os_sandbox.fd:333-400 syscall tables cover a few dozen calls per arch; `sb_call_x64` of anything else is unknown.
- No macOS sandbox profile; AppContainer only as data.
Tests: test_std_os_sandbox.fd 97 lines.

### pqc
Complete means: ML-KEM-512/768/1024, ML-DSA-44/65/87 (and HashML-DSA), ideally SLH-DSA, constant-time arithmetic.
- pqc.fd:35 ML-DSA-87 and HashML-DSA not implemented; ML-KEM only the 768 set (pqc.fd:555-660); no SLH-DSA (FIPS 205).
- pqc.fd:16-18, 33-34: not constant time (% and branches on secret data); a side channel for a KEM.
Tests: test_std_pqc.fd + test_std_pqc_mldsa.fd: good (OpenSSL cross-checks).

### postgres
Complete means: the wire protocol incl. COPY, notifications, cancellation, binary results, named prepared statements, every result set of a multi-statement query.
- postgres.fd:46-52 (LIMITATIONS): no COPY, LISTEN/NOTIFY dropped, no cancel requests, text results only, no SCRAM-PLUS, no SASLprep.
- postgres.fd:663 `parts = Vec::new()` on each RowDescription: only the last statement's rows survive a multi-statement pg_query.
- postgres.fd:437-441, 1267-1277: each round trip reads with a fresh buffer, so bytes after ReadyForQuery are dropped (stated).
- Results only as a JSON text (`rows`); no typed row/column access.
- pg_hex (postgres.fd:874) calls `crypto_sha256_hex`, which is a plain hex encoder (correct, misleading name).
Tests: 4 files (579 lines): good.

### pem
Complete means: PEM armor, PKCS#8/SPKI, plus decrypting PBES2-encrypted PKCS#8 and reading the classic PKCS#1/SEC1 PEM labels.
- pem.fd:8, 358-365: EncryptedPrivateKeyInfo only names its algorithm; no PBKDF2 + AES-CBC decryption.
- pem.fd:366-378: PKCS#12 detected only.
- No parser for "RSA PRIVATE KEY" / "RSA PUBLIC KEY" (PKCS#1) or standalone "EC PRIVATE KEY" (SEC1), the most common legacy labels.
Tests: test_std_pem.fd 59 lines (thin for the formats claimed).

### pci
Complete means: type-0 and type-1 headers, all capability lists incl. PCIe extended capabilities, MSI/MSI-X decode, BAR sizing incl. 64-bit, class names.
- pci.fd:82-107 BAR decode only for type-0 headers; no type-1 (bridge: bus numbers, windows).
- pci.fd:131-145 capability walk covers the legacy list only; no extended capabilities at 0x100+ despite the header's 4-KiB ECAM.
- No MSI/MSI-X structure decode, subsystem IDs, interrupt line/pin; pci_bar_size (pci.fd:112-125) handles 32-bit masks only.
Tests: test_os_stdlib.fd, about 8 checks.

### physics
Complete means: 2D bodies and shapes with a world step, impulse resolution with restitution and friction, static bodies, and a broad phase.
- physics.fd:433-458, 463-498: collision response zeroes velocity; `restitution` (physics.fd:376) is ignored there.
- physics.fd:503-544 `physics_elastic_collision` applies an impulse even when bodies separate, and divides by a.mass/b.mass, so a static (mass 0) body gives inf.
- No world/step, no friction, no polygons or rotation, no broad phase.
Tests: test_game_stdlib.fd (about 23 calls).

### physics3d
Complete means: rigid bodies with orientation and inertia, sphere/box/plane in every pairing, a step that integrates, detects and resolves, OBB or SAT as advertised.
- No orientation, angular velocity or inertia (struct at physics3d.fd:44-64): not a rigid body in the usual sense.
- physics3d.fd:431-459 `phys3d_check_collision` has no box-plane case (plane collider advertised, physics3d.fd:4); boxes are axis aligned only though the header says "SAT for boxes".
- physics3d.fd:363-371: two static bodies (inv_mass 0) divide by zero.
- After positional correction the sphere/box colliders are not refreshed (phys3d_step refreshes them, resolve_contact does not).
Tests: test_lib_physics3d.fd 192 lines.

### particles
Complete means: seeded randomness per particle, 2D (and the advertised 3D) emitters with interpolation, and a draw helper or draw data.
- particles.fd:136-138 angle and speed from `p_clock_now_ms()`: every particle emitted in the same millisecond gets the same angle and speed, so a burst is one point.
- particles.fd:19 `particle_emitter_render` in the usage does not exist; particles.fd:3 "2D and 3D", only 2D.
- particle_emit_one is O(max_particles) per particle (linear search for a free slot).
Tests: test_lib_particles.fd 93 lines.

### path
Complete means: join/split/normalize/extension plus relative paths (relative_to, strip_prefix, starts_with), with_file_name, absolute (seam to cwd).
- path.fd:99-102 comment says "an absolute b replaces a", code replaces only for a drive letter; "/bar" is appended (path_join("foo","/bar") == "foo/bar", unlike every OS API).
- No path_relative, strip_prefix, starts_with, with_file_name, expand "~", absolute.
- path_components (path.fd:353-355) keeps empty parts for "//" and the leading "".
Tests: test_path.fd 38 lines.

### pmm
Complete means: a frame allocator with single, contiguous and aligned allocation, safe free, and complete x86-64 entry flags.
- pmm.fd:92-95 `pmm_alloc` returns -1 as soon as a bit past `total` is reached in the hint's word, without wrapping to frames below the hint: allocate all 100 frames, free frame 3, the next alloc fails.
- pmm.fd:46-78 mark/free read `p.map.get(f/64)` with no bounds check.
- No pmm_alloc_contiguous / aligned (DMA, huge pages); PT_* lack NX (bit 63), accessed, dirty, global.
Tests: test_os_stdlib.fd, 8 checks; the wrap case is untested.

### privacy
Complete means: a consent registry keyed exactly by (subject, purpose), release that can fail without panicking, and no test fixtures in the public module.
- privacy.fd:43-45 `consent_key` = hash(subject) XOR hash(purpose): symmetric (consent of "ann" for "marketing" equals consent of "marketing" for "ann") and any hash collision grants consent. For an audited release this is a real hole.
- privacy.fd:7-29 PrivCounter is a field-privacy test struct shipped in the stdlib.
- privacy.fd:85-90 release panics; no Option-returning variant; registry is global and not per thread.
Tests: test_sensitive.fd uses grant/release/revoke once; no collision case.

### profiler
Complete means: scoped CPU timing with a sub-millisecond clock, aggregation per scope name, frame statistics, and the sampling API.
- profiler.fd:126, 141, 161, 194: `p_clock_now_ms` everywhere; any scope under 1 ms reads 0 (time.fd:491 has clock_now_ns).
- Header promises "CPU/GPU timing"; nothing measures GPU time.
- profiler.fd:152-165 profiler_end ignores the name (no mismatch check); repeated scopes are separate samples, profiler_get_sample (profiler.fd:276) finds the first only.
- profiler.fd:205-216 average recomputed over the whole history each frame; hist remove(0) per frame.
Tests: test_lib_profiler.fd 157 lines, test_prof_sampling.fd.

### protobuf
Complete means: every scalar wire type (incl. float, sfixed32/64 with sign), packed fields of every scalar type, nested-message helpers, UTF-8 strings.
- No pb_write_float / pb_read_float though header line 10 lists float.
- protobuf.fd:248-250 `pb_read_fixed32` does not sign-extend sfixed32.
- protobuf.fd:153-160, 286-298 packed fields are varints only (no packed fixed32/64, double, sint).
- protobuf.fd:274-282 `pb_read_string` builds the string with char_to_str per byte and O(n^2) concatenation; non-ASCII text is not tested.
Tests: test_protobuf.fd 83 lines.

### noise
Complete means: gradient noise in 1-3 D, simplex, value/Worley, fBm/ridged/turbulence with lacunarity, Poisson sampling.
- Only noise2/noise3 (noise.fd:75, 111), fbm2/fbm3 with fixed lacunarity 2 (noise.fd:138, 158), poisson2.
- noise.fd:204-205 points packed as x*100000+y: w, h must be below 100000 (undocumented).
Tests: test_ai_stdlib.fd: determinism, range and Poisson spacing only.

### noise_proto
Complete means: the Noise framework incl. one-way patterns.
- One-way patterns N, K, X (spec §7.4) and deferred patterns are absent (noise_pattern, noise_proto.fd:379-458); BLAKE2b and 448 unsupported (stated).
Tests: good (cross-checked with Node.js).

### nvme
Complete means: also Get Log Page (SMART/errors), Abort, Dataset Management (TRIM), Async Event Requests, interrupt-driven completion.
- nvme.fd:55, 59: NVME_ADMIN_GET_LOG_PAGE / ABORT constants exist with no command builder; no DSM; polling only.
Tests: good (simulated controller, 328 lines).

### oauth2
Complete means: also the device authorization grant (RFC 8628) and token introspection (RFC 7662); PAR and DPoP are optional.
- None of these four exist (grep finds no device_code/introspect).
Tests: good.

### objc
Complete means: messaging for every argument/return kind and blocks.
- By design no float/struct messages (objc.fd header): KVC only covers properties; blocks exist only as encodings; no NSData/NSError helpers.
- C-sweep NOBUILD is libobjc missing on the Windows host (objc.fd:51), not a defect.
Tests: good (interpreter, fake runtime).

### openapi
Complete means: also securitySchemes and per-operation security, response validation, non-JSON bodies, path-level parameters and $ref to parameters/responses.
- No "security" handling anywhere; openapi_check_request (openapi.fd:558) validates requests only.
Tests: good (217 lines).

### permissions
Complete means: also contacts, calendar, storage/media, phone/SMS, body sensors/motion.
- The permission set (permissions.fd:40-56) stops at camera, location, microphone, notifications, bluetooth, photos.
Tests: good.

### plc
Complete means: a fuller IEC 61131-3 ST (user FUNCTION/FUNCTION_BLOCK, arrays, STRUCT, STRING) and at least one more language.
- Header: "a Structured Text subset"; plc_compile (plc.fd:1088) has no POU definitions other than PROGRAM, no arrays/structs/strings; ladder is data only, no IL/FBD/SFC.
Tests: good (274 lines).

### pointcloud
Complete means: also RANSAC plane segmentation, statistical/radius outlier removal, point-to-plane ICP, PLY/PCD I/O.
- None present (pub fns at pointcloud.fd:43-589).
Tests: good.

### postgres_db
Complete means: the Database adapter, tested.
- postgres_db.fd:19-22 returns `r.rows` without looking at `r.ok` ("[]" on error is by design but silent).
Tests: none.

### push
Complete means: also the FCM v1 OAuth2 access token from a service account (RS256 JWT), and parsing APNs/FCM error responses (BadDeviceToken, UNREGISTERED) to drop dead tokens.
- Neither exists (no access_token/reason handling in push.fd).
Tests: good.

### qpack
Complete means: RFC 9204 with the dynamic table on both sides.
- qpack.fd:1-21: static table only; dynamic insertions refused (qpack.fd:804). Interoperable, no compression of repeated headers.
Tests: good.

### qr
Complete means: decoding that corrects errors with Reed-Solomon, Kanji mode, structured append.
- qr.fd:19, 680 decoder checks syndromes and counts bad blocks; no Berlekamp-Massey/Forney correction, so one wrong module fails a decode.
- No Kanji mode, no structured append.
Tests: good.

### quic_conn
Complete means: also ECN, PMTU discovery, pacing, a modern congestion controller, and 0-RTT anti-replay.
- Header "Not here": ECN, PMTU (every datagram <= 1200 bytes), pacing, preferred address, 0-RTT anti-replay; NewReno only.
Tests: good (5 files, ~2200 lines with quic).

---

P1: parallel (STUB), os (THIN), queue (THIN).

### quic_tls

Complete means: a TLS 1.3 handshake for QUIC that authenticates the server the way a client of public servers needs (certificate chain to trust anchors, hostname, CertificateVerify with the common schemes), and serves certificates browsers accept, with the usual groups and suites.

Gaps:
- quic_tls.fd:1891-1898: without a pinned key, a certificate whose key is not Ed25519 is accepted with no check; quic_tls.fd:1932-1936: its CertificateVerify (ECDSA 0x0403/0x0503, RSA-PSS 0x0804-0x0806, PKCS#1 0x0401/0x0501/0x0601, offered at 871-882) is then skipped. The Finished only proves the peer finished a handshake with us, so any MITM passes. tests/net/http3_public.fd:4-6 documents this as "like curl -k".
- quic_tls.fd:1904-1911: even for Ed25519, without a pin there is no identity check at all; with a pin only a self-signed Ed25519 cert is accepted. No chain, no trust anchors, no hostname match (header 16-20). The pieces exist and are unused: x509.fd:637 `x509_verify_chain`, x509.fd:1095 `x509_matches_host`, x509.fd:577 `x509_verify_sig`, ecdsa.fd, rsa.fd.
- quic_tls.fd:1883-1886: only the first certificate of the Certificate message is read; the rest of the chain is dropped.
- quic_tls.fd:1159-1173, 1348: the server signs only with Ed25519 and refuses clients that do not offer 0x0807. Browsers (Chrome) do not take Ed25519 certificates in TLS, so a Fides HTTP/3 server cannot serve them. Header line 35 lists "RSA/ECDSA certificates of our own" as not done.
- quic_tls.fd:865-867, 1192, 1729: X25519 is the only group (no secp256r1). A client offering only P-256 fails at 1205-1206.
- quic_tls.fd:61-62: only the SHA-256 suites; TLS_AES_256_GCM_SHA384 is missing.
- quic_tls.fd:1508-1522: 0-RTT is accepted on ticket age skew alone. There is no anti-replay (single-use tickets or a ClientHello cache, RFC 8446 §8), so 0-RTT data can be replayed within the lifetime.
- Client certificates and external PSKs are missing (header 35). The exporter secret is not kept.
- quic_tls.fd:1250-1255: the SNI is read as everything from byte 5, with no check of the list or name structure.

Test gaps: the handshake is exercised only indirectly through test_stdlib_quic_conn/quic_more/http3, which need C (too slow for the interpreter). The unpinned RSA/ECDSA path runs only in tests/net/http3_public.fd (network, not in the suite). Malformed-message alerts (decode_error, illegal_parameter paths), the server's rejection of a client without Ed25519, and the SNI parse are untested.

### ssh

Complete means: an SSH-2 transport and client/server message layer that interoperates with common OpenSSH setups: RSA and ECDSA host/user keys besides Ed25519, the usual channel requests (pty-req, env, exit-status, signal, window-change), port forwarding, and reading OpenSSH public-key/authorized_keys lines.

Gaps:
- ssh.fd:524-534, 586-589: one suite only (curve25519-sha256, ssh-ed25519, chacha20-poly1305). A server with only RSA/ECDSA host keys, or a FIPS peer (aes*-gcm, ecdh-sha2-nistp256), fails negotiation. rsa.fd and ecdsa.fd exist but are not used (rsa-sha2-256/512, ecdsa-sha2-nistp256).
- ssh.fd:930-947, 984: publickey user authentication is Ed25519 only. Most user keys are RSA or ECDSA.
- ssh.fd:1129-1134: `ssh_channel_request` encodes arguments only for exec and subsystem. pty-req, env, exit-status, exit-signal, signal and window-change cannot be built.
- ssh.fd:1174-1178: `ssh_parse_channel` throws away the data of every other request. A client cannot read the remote command's exit-status.
- ssh.fd:1047-1055: CHANNEL_OPEN is "session" only. There is no direct-tcpip or forwarded-tcpip (port forwarding).
- ssh.fd:70: SSH_MSG_GLOBAL_REQUEST is defined with no builder or parser. REQUEST_SUCCESS/FAILURE (81/82) are missing, so there is no keepalive or tcpip-forward.
- Channel windows and flow control are entirely the caller's. There is no channel state (open/eof/close bookkeeping).
- ssh.fd:564-591: `first_kex_packet_follows` is never read. A peer that guesses and sends a wrong guessed packet would have it processed.
- ssh.fd:422, 465: sequence numbers wrap at 2^32 and there is no automatic re-key after a data or packet limit (RFC 4253 §9, RFC 4344). With chacha20-poly1305 the nonce is the sequence number, so a long session without a manual `ssh_rekey` reuses nonces.
- ssh.fd:432-437: `ssh_fail` closes locally but sends no DISCONNECT. The caller does not get the bytes to tell the peer.
- Missing: keyboard-interactive (RFC 4256), ext-info/server-sig-algs (RFC 8308), parsing of OpenSSH key files, authorized_keys and known_hosts lines.

Test gaps: test_std_ssh.fd covers the happy path, a re-key, a tampered packet and an untrusted host key. Untested: negotiation failure (no common algorithm), strict-KEX violations (ssh.fd:705, 819), banner and DISCONNECT handling, CHANNEL_OPEN_FAILURE, WINDOW_ADJUST, an overlong identification line, a bad padding or length. There is no run against a real OpenSSH (the vectors come from Node).

### srtp

Complete means: RFC 3711/5764/7714 protection for the profiles WebRTC negotiates (AES-128 and AES-256, CM and GCM), with replay and ROC state that an attacker cannot inflate, and key-lifetime limits.

Gaps:
- srtp.fd:49-51, 74-77, 270, 281: only 128-bit profiles. SRTP_AEAD_AES_256_GCM (RFC 7714) and AES_256_CM_HMAC_SHA1_80/32 (RFC 6188) are missing, although aes_gcm.fd:127 already takes 32-byte keys.
- srtp.fd:499 and 639: `srp_slot` makes per-SSRC state (two 64-entry windows) before the packet authenticates. Forged packets with random SSRCs grow `ssrcs`/`wins` without bound (memory DoS), and the linear search at 301-308 slows every packet.
- srtp.fd:458: the sender's ROC wraps at 2^32, and srtp.fd:588 wraps the SRTCP index at 2^31. Neither stops or signals at the RFC 3711 §9.2 key lifetime (2^48 SRTP / 2^31 SRTCP packets), so the keystream or GCM nonce is reused after the wrap. Header line 24 states key-lifetime counting is not done.
- No MKI and no f8 or NULL cipher (header 23). RFC 6904 encrypted header extensions are missing.
- srtp.fd:189-214: kdr > 0 derivation exists, but contexts never re-derive keys (kdr fixed to 0 at 292-297).

Test gaps: test_std_srtp.fd is good on vectors and the replay window. Untested: kdr > 0, `srtp_set_roc`, `srtcp_set_index`, `srtp_is_rtcp`, truncated or garbage packets, many-SSRC behaviour.

### rsa

Complete means: RFC 8017 (PKCS#1 v2.2) RSA with every common hash (SHA-1/224/256/384/512), signing safe enough to face attackers (blinding, fault check), key validation on import, and key generation that follows FIPS 186-5.

Gaps:
- rsa.fd:648-651 with kdf.fd:31-33: hashes are only SHA-1, SHA-256 and SHA-512. SHA-384 (and SHA-224) are absent from DigestInfo, PSS, OAEP and MGF1. x509.fd:522-572 re-implements PKCS#1 and PSS verification with ecdsa's hash dispatch to get SHA-384 (duplicate logic).
- rsa.fd:586-591: no base blinding in RSADP/RSASP1. The header (19-23) says private operations leak timing. Blinding with the public exponent is cheap and expected for any signing or decrypting service.
- rsa.fd:680-684, 756-761: no fault check after the CRT signature (verify before release). A CRT fault reveals p (Bellcore attack).
- rsa.fd:468-471: `rsa_public` accepts any e (even, 1) and any modulus of 64 bits or more. rsa.fd:503-522: `rsa_private_from_der` checks nothing about consistency (n = pq, e odd, d range), and the caller must remember `rsa_private_check`.
- rsa.fd:926-937: `rsa_generate` enforces neither the documented minimum ("at least 128") nor a sane one (2048). There is no |p - q| distance check (FIPS 186-5 B.3.3), and an odd `bits` value is not refused.
- RSAES-PKCS1-v1_5 encryption/decryption (legacy but still met in TLS 1.2 and JWE RSA1_5) is missing.
- rsa.fd:896: Miller-Rabin squares through `bigint_mod` (slow divide) instead of the Montgomery engine. rsa.fd:96-136: `rsa_bit_len` converts through repeated bigint divisions. This is a performance gap only.

Test gaps: test_std_rsa.fd uses one 768-bit key and SHA-256 only. SHA-1 and SHA-512 paths, `rsa_private_to_der`, `rsa_private_from_der` on a PKCS#1 ("RSA PRIVATE KEY") block, malformed or negative DER rejection, and 2048/4096-bit keys are untested. There are no RFC 8017 or Wycheproof vectors, and keygen runs at 256 bits only.

### sha512

Complete means: the SHA-512 family of FIPS 180-4 (SHA-512, SHA-384, SHA-512/256, SHA-512/224) with one-shot and incremental (update/finish) APIs, plus HMAC.

Gaps:
- SHA-384 is missing here, and ecdsa.fd:123-361 carries a full private copy of this compression (constants, round function and padding: "mirrors sha512.fd (whose helpers are private)") to get it. sha512.fd should expose the compression with a chosen IV and output length so that ecdsa, x509 and tls13 share one implementation.
- No SHA-512/256 or SHA-512/224.
- No streaming context: sha512.fd:171-229 copies the whole message into a padded vector (twice the memory) and cannot hash data that arrives in pieces.
- sha512.fd:24-107, 204: the 80 constants are rebuilt into a new Vec on every hash (twice per HMAC). This is a performance gap only.
- No HMAC verification helper with a constant-time compare (callers compare MACs themselves).

Test gaps: test_sha512.fd and test_hmac_sha512.fd are good. Nothing tests messages above about 1 KB.

### quic_frame

Complete means: every RFC 9000 §19 frame and §18 transport parameter encoded and parsed with the RFC's checks. That is done, apart from the points below.

Gaps:
- quic_frame.fd:692, 799-801: preferred_address (0x0d) is refused from a client but, from a server, parsed and dropped. There is no QuicParams field and `quic_params_encode` (728-760) cannot send it, so a server cannot offer a preferred address and a client cannot use one.
- quic_frame.fd:575: the frame type is read as any varint. A non-minimal encoding (for example 0x4006 for CRYPTO) is accepted, though RFC 9000 §12.4 makes it a PROTOCOL_VIOLATION.
- No DATAGRAM frames (0x30/0x31, RFC 9221) and no newer transport parameters (max_datagram_frame_size, grease_quic_bit RFC 9287, version_information RFC 9368). Unknown frame types correctly give FRAME_ENCODING_ERROR at 553-555.

Test gaps: test_stdlib_quic_frames.fd covers nearly every type. PATH_RESPONSE, MAX_STREAMS_BIDI, STREAMS_BLOCKED_BIDI/UNI and the 2^60 bound on them, and a server's preferred_address in parameters are untested.

### quic_udp

Complete means: a UDP driver for quic_conn (client and server) that a user can point at a host name, that scales past a handful of connections, and that is tested in the suite or at least in CI.

Gaps:
- quic_udp.fd:24, 111-113: only IP literals ("resolve them first"). dns.fd exists, but there is no `quic_udp_connect_host`.
- quic_udp.fd:220-236: each datagram scans every connection and copies each `QuicConn` out of the Vec (value semantics) to compare IDs: O(conns × connection size) per datagram. There is no CID-to-index map.
- quic_udp.fd:305-308: an Initial (without Retry) always makes a new connection. There is no cap on `sv.conns` and no accept queue or "new connection" signal (the user scans `sv.conns`).
- quic_udp.fd:183-197: a failed bind leaves `sock` -1 with no error text. quic_udp.fd:83-90: at most 256 datagrams per flush, silently.
- No ECN, GSO or path-MTU discovery (polish).

Test gaps: nothing in tests/bootstrap. tests/net/quic_udp_echo.fd, quic_udp_more.fd, http3_udp_loopback.fd and quic_curl_interop.fd use real sockets and run by hand only.

### sqlite

Complete means: open (with flags and a busy timeout), exec, prepared statements with typed binds (int, real, text, blob, null) and typed row access (not just JSON text), errors returned as values, transactions, last rowid/changes, and an implementation of db's `Database` cap.

- sqlite.fd:4-5: the header says the module implements db.fd's `Database` capability "interchangeably with the in-memory stub". There is no `impl Database for SQLite`. MemoryDB, postgres_db.fd:18 and mysql_db.fd:225 do implement it.
- sqlite.fd:113, 157, 163, 240: every failure (open, prepare, bind, exec) calls `p_panic`. A bad SQL string or a constraint violation kills the program, and there is no Result API.
- sqlite.fd:172-176: the step loop breaks on any code other than SQLITE_ROW. An SQLITE_ERROR or SQLITE_BUSY in the middle of a query returns the rows read so far as if the query had succeeded; only the trace span records `rc == SQLITE_DONE`.
- sqlite.fd:54, 146-166: only `sqlite3_bind_text` is declared, so `query_params` takes Vec<str>. Integers need `CAST(? AS INTEGER)` in the SQL (as the test does), and NULL, real or blob cannot be bound. This also means sql_builder's SqlVal output cannot be bound faithfully.
- sqlite.fd:205-207: a BLOB column comes back as JSON null, with no blob read API.
- sqlite.fd:137-218: results exist only as a JSON string. There is no typed row/column access, no prepared statement a caller can keep (`reset`/`clear_bindings`), and no `open_v2` flags (read-only) or `busy_timeout`.
- sqlite.fd:43-79: the hand-written declarations return `i64` where the C functions return `int`. sqlite3_h.fd:39-132 has them as `i32`. This works only while the callee leaves the upper half of rax zero. The module duplicates a subset of sqlite3_h instead of using it.
- sqlite.fd:26: `use json` is imported but never used (`json_escape` is a local copy, sqlite.fd:274).
- Test gaps: no test of error paths (bad SQL, constraint failure), NULL/REAL output, BLOB, or a failing step. All three tests need the external library.

### regex

Complete means: a compiled-regex API with error messages; the usual syntax (lazy quantifiers, named groups, flags i/m/s, \xHH, \f \v, POSIX classes optional); UTF-8 aware matching; find/find_all/replace/split/captures, all in linear time.

- regex.fd:54-59, 741: `regex_compile` is pub, but `Regex`'s fields are private and no pub function takes a `Regex`. Every regex_* call compiles its pattern again (regex.fd:1089, 1119, 1260, 1298, 1332, 1420), so a pattern used in a loop is recompiled on each call.
- regex.fd:1090, 1121, 1262, 1300, 1334: a pattern that does not compile looks like "no match" (false / -1 / input unchanged / []). There is no error text.
- regex.fd:370 (`rv_atom_end` refuses a quantifier char as an atom): `a*?`, `a+?` and `a??` fail to compile, so there are no lazy quantifiers. There are also no named groups `(?P<name>...)` / `(?<name>...)`.
- regex.fd:420-422, 747: the only flag is a leading `(?i)`. There is no `(?m)` (so `^`/`$` match only at the string's ends, regex.fd:796-803), no `(?s)` (`.` never matches `\n`, regex.fd:938/1026), and no flags mid-pattern.
- regex.fd:149-163: escapes are \n \t \r \0 only. `\x41`, `\u{...}`, `\f` and `\v` turn into literal letters.
- regex.fd:938, 1026, 74-85: matching is on bytes, so `.` and `[é]` see UTF-8 bytes. `.` matches half of a two-byte character, and find_all can split a code point.
- regex.fd:230-233, 242-244: `rx_parse_brace` builds `mn`/`mx` with no bound, so `a{39999999999999999999999999}` overflows and panics. This is the open bug tests/verify/bugs/p2_regex_brace_overflow.fd (it expects "ok"). The RV_MAX_REPEAT check at regex.fd:671 comes too late to prevent it.
- regex.fd:1063-1079 with 1039-1047: `rv_find` runs each top-level alternative separately, and a non-matching alternative scans to the end of the string. In find_all/replace/split/replace_groups (one `rv_find` per match), a top-level `x|y` over a long string is O(n·matches), which breaks the linear-time claim at regex.fd:260-262.
- regex.fd:48-50: a stale comment says "'|' inside a group is unsupported and fails compilation". This contradicts regex.fd:10 and 268, and the tests (alternation inside groups works).
- regex.fd:205-215: a class range cannot use an escape as an endpoint (`[\x00-\x1f]`), and a reversed range (`[z-a]`) is accepted as empty, not rejected.
- Test gaps: no tests of non-ASCII subjects or of the overflow case (the verify bug file aside); no timing test of find_all with alternation.

### smtp

Complete means: the RFC 5321 client conversation (EHLO/HELO, STARTTLS, AUTH PLAIN/LOGIN/XOAUTH2, SIZE, SMTPUTF8/8BITMIME, pipelining optional) and a safe RFC 5322/MIME builder (encoded words, QP/base64, attachments with RFC 2231 names, line folding, no CR/LF injection).

- smtp.fd:658-668, 672-675, 905: `sm_ascii` accepts CR and LF, so `mail_encode_word` returns an ASCII subject unchanged. A subject such as `"hi\r\nBcc: x@evil"` injects a header. smtp.fd:636-639 and 920 (`mail_add_header`) and smtp.fd:895-904 (From/Reply-To) are not checked either.
- smtp.fd:244-251, 523, 542: `smtp_addr_spec` passes CR/LF through into `MAIL FROM:`/`RCPT TO:`, which allows SMTP command injection (an extra RCPT, or DATA) from an address string.
- smtp.fd:485-521: only PLAIN and LOGIN. XOAUTH2 (needed by Gmail and Outlook) and CRAM-MD5 are missing.
- smtp.fd:220-221, 523-533: SMTPUTF8 and 8BITMIME are parsed from EHLO but never used. MAIL FROM never sends `SMTPUTF8` or `BODY=8BITMIME`, so non-ASCII addresses cannot be delivered.
- smtp.fd:937-938: an attachment filename is written as an RFC 2047 encoded-word inside a quoted parameter, which RFC 2047 §5 forbids (RFC 2231 `filename*=` is the correct form). A `"` in the name is not escaped.
- smtp.fd:905, 895-923: header lines are never folded, so a long ASCII subject or a custom header can exceed the 998-byte line limit.
- PIPELINING is parsed but commands are sent one at a time (acceptable), and there is no multipart/related for inline images (polish).
- Test gaps: no test with CR/LF in a subject/address/header, no non-ASCII attachment name, no long-header case.

### static_serve

Complete means: GET/HEAD of files under a root, safely (traversal, dotfiles), with MIME, validators, conditionals (all of RFC 9110 §13), ranges, precompressed variants, directory handling, a FileStore bound to the real file system, and glue to stdlib http/router.

- static_serve.fd:28-31: the header says "bind them to the fs module", but no module provides an mtime (fs.fd and file.fd have size only; grep finds mtime only in vfs/tar/ext4/asset). No stdlib FileStore exists, so the seam cannot be filled from the stdlib, and every user must write stat/read by hand.
- static_serve.fd:56-69, 543: `ServeRequest`/`ServeResponse` are standalone types, with nothing that converts from `HttpRequest` or to a response string for http_serve or router (unlike router's `http_router_serve_fn`).
- static_serve.fd:675-677 (and 646, 658): the whole file is read into `r.body: Vec<i64>`, 8 bytes of memory per file byte. There is no streaming path, even though `r.file` is set.
- static_serve.fd:556-563: a request for a directory without a trailing slash (`/docs`) is a 404. Servers usually answer 301 to `/docs/`.
- static_serve.fd:602-614: If-Match and If-Unmodified-Since (412) are not handled. When identity is refused (`identity;q=0`) and no variant fits, the server still sends identity instead of a 406.
- static_serve.fd:423-438: `ss_qvalue` does not validate digits (`q=0.x9` gives garbage). static_serve.fd:536-538: `ss_stat` assumes a `|` (an empty or malformed stat string misparses).
- Test gaps: good coverage of the pure logic; nothing for a directory without a slash, a large file, or If-Match.

### sql_builder

Complete means: SELECT/INSERT/UPDATE/DELETE for the dialects the stdlib ships drivers for (PostgreSQL, SQLite, MySQL), with upsert, subqueries, joins on arbitrary conditions, every value bound, and a direct way to run the result through those drivers.

- sql_builder.fd:36-37, 294-296: only PostgreSQL and SQLite. MySQL (backtick identifiers) is missing although stdlib/mysql_db.fd exists.
- sql_builder.fd:553-555: only `ON CONFLICT DO NOTHING`. There is no `DO UPDATE SET ... = excluded.x` (upsert) and no conflict target.
- sql_builder.fd:66-76: no subquery condition (`IN (SELECT ...)`, `EXISTS`), no UNION, no CTE (`WITH`), no column-to-column comparison other than the JOIN's single equality.
- sql_builder.fd:112-118, 497-499: JOIN ON is one `left = right`, with no compound or non-equality conditions.
- sql_builder.fd:300-306: any column or table text holding `(` or a space is copied into the SQL as-is (used by sq_columns, sq_order, sq_group_by, the table name). An ORDER BY column taken from a request (`"name; DROP TABLE x --"` contains spaces) goes in raw. This is not documented as a trust boundary.
- sql_builder.fd:392-404: in `sc_raw`, a `?` inside a quoted string literal of the fragment is also replaced.
- No glue: sqlite's `query_params` takes Vec<str> (sqlite.fd:146), so SqlVal NULL/int/bool cannot be passed through. There is no `sqlite_run(SqlOut)` / postgres equivalent.
- No `NULLS FIRST/LAST`, and no LIMIT/OFFSET as bound parameters (polish).
- Test gaps: no test of a column with spaces/parentheses from untrusted input, or of raw fragments with `?` in quotes.

### redis

Complete means: RESP2/3 codec, a pipelining client that keeps pushes apart from replies, typed helpers for the common commands (incl. AUTH/SELECT/transactions/pub-sub management), and robust input handling.

- redis.fd:917-949: `redis_read_reply` returns the next frame whatever it is. In RESP3 a `>` push (pub/sub message, invalidation) that arrives before a command's reply is taken as that reply, and every later reply is then off by one.
- redis.fd:336-340: the overflow guard lets `v = 922337203685477580` through, then `v * 10 + 9` overflows. A reply `:9223372036854775809` (or a length line) panics the client, and `-9223372036854775808` cannot be parsed.
- redis.fd:918-947: each read decodes `inbuf` again from byte 0 and copies the rest. A large reply arriving in many chunks costs quadratic time.
- redis.fd:901-914, 955: after a send failure `queued` keeps the bytes while `nqueued` is zeroed, and `rs_write` does not check `c.ok`. The next flush sends the stale commands again.
- Missing helpers (generic `redis_call` works): AUTH (RESP2), SELECT, PING, UNSUBSCRIBE, PSUBSCRIBE/PUNSUBSCRIBE, MULTI/EXEC/WATCH, MGET/MSET, SCAN iteration.
- Test gaps: no RESP3 push interleaved with replies, no integer at the i64 edge, no send failure.

### router

Complete means: method + pattern routing with params, wildcards, specificity, groups, 404/405/HEAD/OPTIONS, and middleware (before/after hooks per router or group).

- No middleware: nothing runs before or after handlers (auth, logging, CORS), and groups carry only a prefix (router.fd:343-367).
- router.fd:245-283: `:id` takes any non-empty segment. There is no constraint (`:id(\d+)`, typed params).
- router.fd:606-621: `http_router_serve_fn` pushes a copy of the router into the global `RT_SERVED` (a workaround for a closure-capture bug, per the comment), and that copy is never freed.
- router.fd:470-472, 536-551: a `*` route on a path ends up as `*` in the Allow list if that path ever produces a 405 (harmless today: `*` matches every method).
- Test gaps: good. Not tested: a `*` method route together with HEAD fallback, deep group nesting.

### ros2_msg

Complete means: CDR (XCDR1) read/write of all primitive and sequence kinds plus the commonly used ROS 2 message types, with the DDS type names that topics need.

- ros2_msg.fd:14-22: missing common message types: sensor_msgs/Image, CompressedImage, CameraInfo, JointState, NavSatFix, BatteryState; std_msgs Bool/Int32/Float64/Empty/ColorRGBA; geometry_msgs Accel, Wrench, PoseArray, Polygon, PointStamped; nav_msgs/Path and OccupancyGrid.
- ros2_msg.fd:190-206: readers for u8/u16/u32/i32/u64/f32/f64 only. There are no signed i8/i16/i64 readers (u64 doubles as i64), and no f64/string/struct sequence helpers. Writers lack f64_seq and string_seq (ros2_msg.fd:78-132).
- No DDS type names or topic prefixes (`std_msgs::msg::dds_::String_`, `rt/`), and stdlib/dds.fd has none either, so wiring a message to a topic is left to the user.
- ros2_msg.fd:853-870: the PointCloud2 accessor reads float32 fields only (no other ROS_PF_* datatypes).
- Test gaps: good for what exists. No message with sequence-of-struct alignment beyond PointField.

### slab
Complete means: a fixed-size object allocator with checked alloc/free, typed access to object bytes or words, several caches by size, and a backend that can sit on real memory (the header promises "kernel memory in an OS").
- slab.fd:89-97 `slab_free` does not check that `i` is in range or currently allocated. A double free makes the free list cyclic and drives `used` negative.
- slab.fd:14 and slab.fd:49-52: the backing store is a `Vec<i64>` with one byte per 8-byte element, so it takes 8x the memory, and no real memory is involved. Nothing can place the slab at a kernel address, which contradicts slab.fd:2-3.
- No way to read or write an object's contents: only `slab_off` (slab.fd:100) and `slab_clear` (slab.fd:105) exist, so callers must poke `s.backing` byte by byte.
- No caches by size class (kmem_cache style), no growth by adding slabs, no constructors, no `is_allocated` and no iteration over live objects.
- No alignment control: `obj_size` is used as given, rounded up only to a minimum of 4.

Test gaps: no test_std_slab.fd. Only test_os_stdlib.fd:222-230 (alloc/free/reuse/full); `slab_clear`, `slab_off` and capacity 0 are untested.

### sdcard
Complete means: an SPI-mode SD driver that runs over a real SPI exchange, with init, single- and multi-block read/write, the card's size (CSD), erase and error/timeout reporting, plus the simulator for tests.
- sdcard.fd:308-324: `SdBus` holds only an `SdCardSim`, and `sd_xfer` always calls `sdsim_xfer`. The seam described at sdcard.fd:21-26 has no hook (no fn value, no `SpiBus`), so the driver can never talk to a real card. This is the main gap; the fix itself is small (S).
- No CMD18/CMD12 multi-block read and no CMD25 multi-block write with the 0xFC/0xFD tokens.
- No CMD9/CMD10 (CSD/CID). `sd_init` (sdcard.fd:372-418) never learns the capacity, so a block device built on it cannot report its size.
- No erase (CMD32/33/38) and no CMD59 (CRC on). ACMD41 is bounded by a try count rather than a time limit.
- sdcard.fd:489-491: the busy wait gives up after 100000 bytes and still returns "" (success).
- sdcard.fd:445-447: data error tokens (0x0X) are reported only as "no start token", with the error bits not decoded.
- CS stays asserted after commands (no deselect plus extra clocks), which matters on a shared bus.

Test gaps: the busy timeout, write CRC rejection (0x0B path at sdcard.fd:283) and error tokens are untested.

### semihosting
Complete means: firmware can make semihosting calls on real targets (a trap plus real memory), every call of the Arm spec 2.0 is implemented on the host side, and there are target wrappers for the common calls.
- semihosting.fd:405-419: `SemiTarget` embeds `SemiHost`, and `semi_trap` always calls the model. Target memory is `SemiMem`, a Vec (semihosting.fd:116-125). Firmware has no way to supply a real trap function (for example a C-backend shim) or to build blocks in real RAM, so the module cannot be used on hardware. The header (21-27) admits this, but no seam is actually provided.
- semihosting.fd:397: SYS_TMPNAM, SYS_RENAME, SYS_SYSTEM and SYS_HEAPINFO fall through to EINVAL on the host side.
- No target wrappers for remove, rename, time, clock, istty, readc, errno, heapinfo or system.
- The `:semihosting-features` magic file is not supported. AArch32 `semi_exit(status != 0)` uses SYS_EXIT_EXTENDED (semihosting.fd:517) without checking the extension bit.
- semihosting.fd:296-318: SYS_READ does not check that the handle was opened readable (mode "w"/"a" without "+").
- semihosting.fd:335-342: SYS_SEEK accepts positions past the end of the file.

Test gaps: ISTTY, ISERROR, READC, REMOVE, CLOCK/TIME and the error paths of CLOSE/WRITE on bad handles are untested.

### smp
Complete means: x86 AP bring-up that a kernel can use as is, meaning the trampoline code, a parameter block written to real memory, the INIT-SIPI-SIPI sequence, a per-CPU area at real addresses, and CPU numbering and topology.
- No trampoline code bytes (real mode, then protected, then long mode). smp.fd:20-22 leaves it to "the platform", but it is the core of SMP bring-up, and the parameter block layout (smp.fd:65-71) assumes code the module does not provide.
- smp.fd:312-345: `PerCpu.words` is a Vec model. `percpu_get` and `percpu_set` never touch the address `percpu_base` gives to GS_BASE, and there is no volatile or real-memory path (regmap has one for comparison).
- smp.fd:280-284: topology covers only x2APIC leaf 0xB/0x1F shifts. There is no legacy CPUID 1/4 decoding and no die/module levels, and `core_shift <= smt_shift` is not guarded.
- No helpers to write the parameter block into the trampoline page or to set up the GDT/IDT for the AP; `percpu_init` (smp.fd:348-357) leaves PCPU_TSS and PCPU_CURRENT unset.

Test gaps: good coverage of the plan, fake APs, MADT numbering and percpu layout. Nothing on real memory exists to test.

### rtos
Complete means: a small RTOS kernel API with tasks (create, delete, suspend, priority change), semaphores, mutexes with inheritance, queues, timers with callbacks and fixed memory use.
- rtos.fd:568-577: software timers only increment `fired` and push a string into `timer_events`. There is no callback or event to the owning task, so "run from the tick" (rtos.fd:24) means nothing beyond a log.
- rtos.fd:117-118, 142 and 572: `log` and `timer_events` grow on every switch and expiry with no bound, which contradicts "every object table sized at creation" (rtos.fd:4-6).
- No `rtos_task_delete` and no `rtos_task_set_priority`. There is no queue peek or count, and no semaphore count query.
- rtos.fd:261: `rtos_suspend` of a BLOCKED task returns silently and does nothing (FreeRTOS allows it).
- rtos.fd:32: the header says a blocking call "returns RT_BLOCKED", but blocking calls return RT_PENDING (rtos.fd:50). RT_BLOCKED is the task state 2, which equals RT_TIMEOUT's value.
- Queues carry i64 messages only (rtos.fd:86-91), with no item size.

Test gaps: suspend/resume, yield, recursive mutex depth, unlock by a non-owner (RT_ERROR), sem give at max, queue send timeout and timer stop are untested.

### sensor_drivers
Complete means: drivers that bring each listed sensor up from reset to correct readings on real hardware, report bus errors, and expose the device's main settings.
- VL53L0X: no init. There is no data_init/static_init, no SPAD or reference calibration and no tuning table (stated at sensor_drivers.fd:12-15). There is also no function to read the stop variable that `vl53l0x_start_single` (sensor_drivers.fd:476) requires, so a real module cannot even be started with this API alone.
- BME280: the header (sensor_drivers.fd:7) promises "reset", but nothing writes `BME280_REG_RESET` (sensor_drivers.fd:138). `bme280_init` fixes x1 oversampling, filter off and normal mode (sensor_drivers.fd:336-339), with no setter for oversampling, filter or standby. There is no forced-mode trigger and no poll of the status measuring bit.
- sensor_drivers.fd:336-339 and 415-417: init write results are discarded, so a NAK is not reported.
- sensor_drivers.fd:767-773: `ina219_read` returns a Vec and passes `i2c_read_reg16_be`'s -1 into the conversions. A bus error becomes 32764 mV and similar plausible values. `ina219_init` also ignores the result of the calibration write (762).
- sensor_drivers.fd:82-95: `sensor_command` and `sensor_read_raw` always use I2C, even for a SensorBus made with `sensor_on_spi` (harmless for SHT3x/BH1750, but silent).
- MPU6050: no DLPF/sample-rate settings and no FIFO (acceptable for scope).
- SHT3x: no periodic mode and no status-word decode, though the constants exist (sensor_drivers.fd:529-533).

Test gaps: good. BME280 float vs. integer cross-checked against datasheet values, all six sensors exercised on fake buses. Error paths of INA219 are untested.

### spi
Complete means: SPI configuration, transfers and register helpers that work for common devices, the Linux spidev transport, a bit-banged master, and the flags it accepts actually honoured.
- spi.fd:615-620 and 634-636: `spi_read_regs` and `spi_write_regs` hard-code read bit 0x80 and address mask 0x7F. The header (spi.fd:11-12) says "0x80 by default", but nothing can change it. Devices with a multi-byte bit (ADXL345 0x40) or a different read convention are not served.
- spi.fd:49-56: SPI_3WIRE, SPI_READY and TX/RX_DUAL/QUAD are accepted in `mode` but not acted on. `spi_ioc_image` never writes tx_nbits/rx_nbits (offsets 28/29, spi.fd:675-689), and `spi_bitbang_xfer` ignores 3WIRE.
- spi.fd:622-631: `spi_read_reg8` returns -1 on error, mixing errors with data.
- spi.fd:764-768: a failed ioctl returns -5 regardless of errno.
- No inter-word delay (word_delay_usecs at offset 30 is always 0), and the RD_MODE/bits/speed read-backs are never used.

Test gaps: `spi_bitbang_xfer` over SpiPins (spi.fd:286-331) is untested, as are multi-frame `cs_change` splitting in the fake bus and the Linux path (it needs hardware, which is acceptable).

### riscv_csr
Complete means: CSR numbers, field codecs, Zicsr encodings, and a CSR file that behaves as a hart for the CSRs it names (M- and S-level), plus PLIC and CLINT.
- riscv_csr.fd:494-520: CsrFile has no write mask for sstatus, sie, sip, stvec, sepc, scause, stval or sscratch. A write to them is silently dropped (riscv_csr.fd:547-549 with mask 0), and they are not aliased to mstatus/mie/mip as on real harts.
- riscv_csr.fd:538-552: accesses to CSRs the file does not implement neither trap nor store. A real hart raises illegal instruction.
- riscv_csr.fd:568-580: `csrfile_trap` ignores medeleg/mideleg, even though both have masks at 508-509. There is no S-mode trap and no SRET (only `csrfile_mret`).
- mcycle and minstret are not writable, and no counter advances.
- PMP: encoders and decoders only (riscv_csr.fd:376-412). There is no check of an access against the PMP entries (TOR/NA4/NAPOT matching with priority).
- `rv_mstatus_sd` decodes SD, but CsrFile never sets SD when FS becomes Dirty.

Test gaps: `plic_disable`, `clint_set_timer64`, `clint_clear_ipi`, the exception path of `csrfile_trap`, `rv_mtval_is_address` and the PMP TOR/NA4 decodes are untested.

### regmap
Complete means: register-block descriptions with every common access kind, safe read-modify-write, a fake bus that models them, and an MMIO bus.
- regmap.fd:38-46: only RO/RW/WO/W1C. SVD's oneToSet (W1S), zeroToClear (W0C), readAction=clear (read-to-clear) and writeOnce are missing, and the fake bus (regmap.fd:396-430) cannot model reads with side effects. stdlib/svd.fd:172/199 maps only oneToClear.
- regmap.fd:487-501: `regmap_write_fields` does not check `fields.len() == vals.len()`.
- regmap.fd:176-178: `regfield_set` rejects every negative `x`, which also rejects legitimate full-width 64-bit values with bit 63 set.

Test gaps: the MMIO path cannot be tested (acceptable). Otherwise good.

### sched
Complete means: RR, priority-with-aging and CFS-like policies as a deterministic policy layer, plus the operations a kernel needs on live tasks.
- No `sched_set_nice` or `sched_set_prio` for an existing task: weight and prio are fixed at `sched_spawn` (sched.fd:273-284).
- sched.fd:55: one policy per `Sched`. There are no scheduling classes (RT tasks above fair tasks), which a kernel scheduler usually has.
- sched.fd:377: `dt * 1024 / t.weight` truncates. With dt=1 and weight above 1024, vruntime does not advance (Linux uses a scaled inverse weight).
- Exited tasks stay in `tasks` forever and ids are never reused (acceptable for a simulation).

Test gaps: `sched_yield` and FAIR wakeup with a non-preempting task are untested; otherwise good.

(rtc and ramfs are COMPLETE: rtc covers the calendar, Unix time, CMOS decode/encode with UIP discipline, alarms and prescalers, all tested against known vectors. ramfs implements every vfs FsOps operation with the right errnos and is tested directly and through vfs. Their minor items are in the table.)

### safetensors
Complete means: open and validate a .safetensors file per the spec (every dtype, zero-size and scalar tensors, offsets that are contiguous with no holes or overlap), read tensors by name with dtype- and bounds-checked element and bulk reads, work on large files through mmap or offset reads, and write a file.
- safetensors.fd:36 `st_open(b: Vec<i64>)` takes the whole file as one i64 per byte, so a 1 GB checkpoint needs 8 GB of memory. There is no mmap or offset-reader seam (stdlib/mmap.fd exists), so real model files are impractical.
- safetensors.fd:126-133 each `st_tensor` calls `json_has`, which runs `json_keys` over the whole header (json.fd:1101), then `json_get_safe`. Lookups are O(header) each and O(n^2) across a model; there is no name→entry index.
- safetensors.fd:154 `if d <= 0 { return t; }` rejects zero-size dimensions, which the format allows (empty tensors with `data_offsets [a,a]`).
- safetensors.fd:104-110 F4 counts as 1 byte per element, but F4 packs 2 elements per byte, so every F4 tensor fails the size check at :170. F8_E8M0, F6_E2M3, F6_E3M2 and C64 are unknown (size 0), so those tensors come back found=false.
- safetensors.fd:36-72 does not validate what the spec requires: the header starts with `{`, a header size limit, and data_offsets covering the data region with no holes and no overlaps. Two tensors may alias the same bytes.
- safetensors.fd:231-337 element reads check neither the dtype nor `i < st_elems(t)`, although the header (line 4) says "All access is bounds-checked":
  - `st_int` on U8, U16, U32 or BOOL falls through to the 8-byte I64 path at :317-323 and returns garbage.
  - An index past the tensor silently reads the next tensor's bytes.
- safetensors.fd:326 `st_uint` cuts U64 to its low 32 bits. The comment "document it" is a leftover note.
- safetensors.fd:206-218 the comment says inf and NaN give "the largest finite magnitude", but the code returns `2^1024` (= inf), and NaN becomes ±inf. `p_f64_from_bits`, already used at :290, could decode f32 exactly.
- Missing pieces: no F8_E4M3/F8_E5M2 element decoders, no BOOL read, no bulk read (tensor → Vec<f64>), no writer (`st_save` / serialize).
- Test gaps: there is no test_std_safetensors.fd. Only tests/bootstrap/test_ai_stdlib.fd:34-67 covers it: one F32 [2,2] and one I64 [2] tensor, metadata, and the f16/bf16 scalar converters. Untested:
  - `st_f16`, `st_bf16`, `st_f64` and `st_uint` element reads, and I8/I16/I32;
  - malformed or short headers, header_len > 4 GiB, offsets past the end, shape/offset mismatch;
  - a missing tensor name, `st_names`.

### secrets
Complete means: PBKDF2 plus a sealed vault that fails loudly on any bad parameter and bounds attacker-controlled fields, wipes key material, and a keychain seam with at least one working backend (or a clean pointer to secure_store).
- secrets.fd:278 the result of `aes_gcm_seal` is not checked. aes_gcm.fd:391 returns `[]` when the IV is not 12 bytes, so `vault_seal_with` with a bad IV silently produces a header-only blob and loses the data.
- secrets.fd:295-298 `vault_seal_with` writes an IV of any length, but `vault_open` assumes 12 bytes (:358).
- secrets.fd:275 and :361 derive the key with `pbkdf2_sha512_str(passphrase, str_from_bytes(salt), ...)`: binary salt bytes go through a `str` and back. The bytes should be passed straight to `pbkdf2_sha512` (:47). This only works while every back end keeps NUL and high bytes in strings (the test salt starts with 0x00).
- secrets.fd:349 the iteration count is read from the file with no upper or lower bound: a crafted file with 2^32-1 iterations hangs `vault_open`, and 0 is accepted.
- secrets.fd:288 `o.push(salt.len())` stores the salt length in one byte: a salt over 255 bytes corrupts the file.
- secrets.fd:199 a label over 65535 bytes is silently cut by `sc_be16`.
- secrets.fd:21-22 the header says "an extern seam (declared, not called here)", but no extern is declared.
- secrets.fd:387-403 `Keychain` is a struct of function values with no backend. There is no wrapper for its `remove` field. DPAPI, the macOS Keychain and libsecret appear only in comments (:375-380).
- secrets.fd:330-368 / :274-305 `zeroize` is provided but never used on the derived key or the plaintext.
- Overlap: secure_store.fd has a second PBKDF2 and a second vault format (FSS1 vs FIDVAULT). One should build on the other.
- Polish: `hmac_sha512_bytes` recomputes the key pads on every iteration. A precomputed inner/outer state would halve the PBKDF2 cost.
- Test gaps: the keychain seam, `vault_decode` truncation errors, a version mismatch, out-of-range iterations, and a bad IV length are untested.

### secure_store
Complete means: an encrypted key-value file that round-trips under every valid parameter, rejects bad ones loudly and saves atomically, plus Keystore and Keychain plans that cover put/get/delete on both platforms with a working bridge from the query data to the declared calls.
- secure_store.fd:192/:254 vs :282 `secure_file_new(pass, 0, ...)` is accepted and seals with iterations 0 written in the header, but `secure_file_open` rejects `iters < 1` as SECURE_CORRUPT. Such a file can never be opened.
- secure_store.fd:281 there is no upper bound on the iteration count read from the file (a hang on a crafted file).
- secure_store.fd:248-258 the lengths returned by the random function are not checked. A generator that returns fewer than 12 nonce bytes makes `aes_gcm_seal` return `[]` (aes_gcm.fd:391), so the blob is header-only and `secure_file_save` reports success.
- secure_store.fd:271 `ss_no_random` is dead code.
- secure_store.fd:48-53 the `SecItemAdd`, `SecItemCopyMatching`, `SecItemUpdate` and `SecItemDelete` externs are declared but never called. No function turns a `KeychainQuery` into an NSDictionary (objc.fd) and calls them, so every user has to write the bridge.
- secure_store.fd:363-401 there are `keystore_create_key_steps` and `keystore_encrypt_steps` but no decrypt steps (GCMParameterSpec, DECRYPT_MODE 2), although the comment at :393-394 describes them. There is also no key-delete step.
- secure_store.fd:313-316 `secure_file_save` writes the file in place, with no temp file and rename (save_game does this right): a crash during the write destroys the store.
- secure_store.fd:186 the passphrase is kept in the struct for its whole life. Neither the derived key (:257, :287) nor the plaintext is zeroized.
- secure_store.fd:89-133 its own HMAC-SHA512 and PBKDF2 duplicate sha512.fd:255 `hmac_sha512_bytes` and secrets.fd:47. They also lack the `@secret` annotations, so the secrecy checker does not cover this key path.
- secure_store.fd:237 a key over 65535 bytes is silently cut (the u16 length).
- Scope: there is no desktop backend (Windows DPAPI, Linux Secret Service); macOS uses the same SecItem calls but is not mentioned.
- Test gaps: `secure_file_rekey`, iterations 0, a short random function, `keystore_create_key_steps(..., true)`, and the update/delete query text are untested.

### sigmatch
Complete means: the commonly used YARA subset compiles and scans correctly: integer reads at offsets, rule references, private/global rules, regex modifiers, xor/base64/fullword-wide string modifiers, and an atom-based scanner fast enough for real files.
- sigmatch.fd:642-643 any other word in a condition is an error. So there is no `uint8/16/32(be)(offset)`, which is the most common YARA idiom (`uint16(0) == 0x5A4D`), no reference to another rule by name, no arithmetic, no `for ... of`, no `!a[i]` (match length), no `entrypoint`.
- sigmatch.fd:763 only `rule` can start a declaration: `private rule`, `global rule`, `import "pe"` and `include` fail with "expected 'rule'", so real rule sets do not compile.
- sigmatch.fd:400-421 and header :25-26 regexes take no `/i` or `/s` modifiers. There are no `xor` or `base64` string modifiers (:447-455).
- sigmatch.fd:813 `fullword` checks the single byte before and after the match. For a `wide` string the byte before is the previous character's 0 high byte, so the left boundary always passes and `"kernel" wide fullword` matches inside `"xkernel"` in UTF-16.
- sigmatch.fd:720-724 a negative meta integer (`score = -5`) pushes an empty value, then fails with "expected ':'".
- sigmatch.fd:379-384 hex alternatives take only bytes and wildcards. YARA allows jumps and nested groups inside `( | )`. There is no `~` (not) byte.
- sigmatch.fd:911-920 / :821-869 the hex search tries `sg_hex_at` at every offset with backtracking jumps; the text search (:796-819) is naive. There is no atom prefilter, so the cost is O(rules × strings × n × pattern) and worse with `[-]`.
- sigmatch.fd:871-872 each regex string copies the whole buffer into a `str` again.
- sigmatch.fd:993-1029 `sig_rule_matches` and `sig_scan` duplicate the per-string search.
- Anonymous strings (`$ = "..."`) collide as duplicate `$`.
- Test gaps: `@a[i]` with an index, `!=`, `<=`, hex `[n-]`, `\x` escapes, comments inside hex, `nocase` + `wide`, the `N of them` threshold, the error for an empty alternative, and the missing-condition error are untested.

### state_machine
Complete means: what its header promises, "UML-style statecharts", for robotics use: orthogonal regions, final states and completion transitions, local and external transitions, events with data, validation of the model, and per-machine event posting.
- state_machine.fd:83 `cur` holds one active leaf and HsmState has one parent (:47-61), so orthogonal (parallel) regions cannot be expressed. Statecharts usually need them for concurrent subsystems.
- state_machine.fd:410-424 transitions need a named event, and timed ones need `after`. There are no final states, no completion (eventless) transitions, and no choice or junction pseudo-states.
- state_machine.fd:395-399 a transition into a substate always exits and re-enters the source (external). There is no `local` kind.
- state_machine.fd:84 and :26-27 events are plain strings: no payload. Guards and actions see only `ctx`, so a guard cannot test event data. There are no deferred events.
- state_machine.fd:230-245 `hsm_outbox` is one global for every machine. An action of machine A that runs machine B, or two machines on different tasks, mix their emitted events.
- state_machine.fd:103-166 state, parent, source and target ids are not validated; there is no `hsm_check` for things like an initial child that is not a child.
- state_machine.fd:449-450 `queue.remove(0)` makes a long queue O(n^2).
- Test gaps: `hsm_dispatch` directly, `hsm_run_limit` hitting its limit, timed transitions on an ancestor, guards on timed transitions, `hsm_find`, `hsm_set_initial`, and exit actions are untested.

### spatial_audio
Complete means: OpenAL-equivalent 3D audio (distance models, directional sound cones, Doppler, panning) with a renderer that handles clip and output rates, plus the binaural cues it claims.
- spatial_audio.fd:63-73 the emitter has no direction or cone (inner/outer angle, outer gain), so OpenAL's directional sources are missing.
- spatial_audio.fd:404 and :447 the clip is assumed to be at the output rate: `step` is pitch × Doppler only, with no source-rate parameter.
- spatial_audio.fd:376-399 interpolation is linear, with no band-limiting when Doppler or pitch > 1 (aliasing).
- spatial_audio.fd:272-274 and :417-432 the binaural mode uses only the lateral angle: no elevation cue and front/back identical (documented as an approximation). There are no HRTF data (SOFA) hooks.
- There is no occlusion, air absorption (distance low-pass) or reverb send, and no helper that mixes several voices onto one bus.
- spatial_audio.fd:249-252 `den <= 0` returns 1.0 where the shift should be clamped high (only reached at vss == SS/DF).
- spatial_audio.fd:128-175 `spatial_atan`, `spatial_atan2` and `spatial_asin` are general math: stdlib/math.fd has no atan or asin, and they belong there.
- There is no listener `look_at` / orientation helper.
- Test gaps: the looping render path, `spatial_elevation` below the horizon, the linear model when max <= ref, rolloff 0, and the Doppler clamp at the speed of sound are untested.

### save_game
Complete means: versioned, checksummed, migratable saves that survive process crashes and power loss, with a standard per-platform save location and slot management.
- save_game.fd:484-511 with stdlib/fs.fd:122-131: `fs_write_bytes` → `p_file_write_vec` never fsyncs or flushes to disk. The read-back check at :487 reads the page cache, so the "crash-safe" claim (:23-29) holds for process crashes, not power loss.
- There is no platform save directory (APPDATA, ~/Library/Application Support, XDG_DATA_HOME, mobile app-data), and no slot list or slot metadata (timestamp, play time, thumbnail section) helpers.
- save_game.fd:310 and :317 a section name over 65535 bytes or data over 4 GiB is silently cut by `sv_put16`/`sv_put32`. The same applies to `save_put_str` (:134).
- save_game.fd:516-533 after a crash between the two renames, the verified `path.tmp` is ignored; only `.bak` is tried. This is acceptable (the old save) but undocumented.
- There is no optional compression or integrity key (HMAC) against casual editing; CRC-32 only detects accidents.
- Test gaps: a failure in the middle of `save_game_write_file` (rename failing), the `section ... runs past the end` decode errors, and `save_get_str` with a bad length are untested.

### share
Complete means: what its header promises: Android send/send-multiple/chooser intents with FileProvider, incoming shares, and an iOS activity sheet including excluded activity types.
- share.fd:68 `ShareRequest.excluded` is never read. There is no setter, and `share_ios_items` / `share_ios_calls` never apply it, although the header (:18-19) promises "excluded activity types".
- share.fd:160 `share_file_paths_xml` does not XML-escape `name` or `path`.
- share.fd:238-246 the chooser describes its target only by its action string: the target's MIME type, extras and grant are lost. `share_android_am_command` (:252-270) skips `intent` extras silently.
- share.fd:127 the path check rejects only `/../` and a trailing `/..`. `./`, `//` and relative paths are not normalized.
- share.fd:319-330 the subject is ignored on iOS (Mail subject needs UIActivityItemSource).
- share.fd:274-298 incoming shares ignore EXTRA_SUBJECT and ClipData. Newer apps may send URIs only in ClipData.
- Test gaps: `share_android_received` with SEND_MULTIPLE and a uri-list, text over SHARE_TEXT_WARN_BYTES (warning), `share_ios_check` with no items, and XML with special characters are untested.

### random
Complete means: a seedable PRNG with good statistical quality (per-instance state plus a global convenience), unbiased ranges, full 64-bit output, floats in [0,1), shuffle/choice, and the usual distributions (normal, exponential).
- random.fd:73 the generator is a 31-bit LCG (`state*1103515245+12345 & 0x7FFFFFFF`). With an odd multiplier and an odd increment modulo 2^31, the low bit alternates on every call, so `random_bool` (random.fd:163, `next_raw() & 1`) returns true, false, true, false... exactly. `random_byte` (:223) has a period of 256 and `random_hex` (:237) a period of 16 in each nibble.
- random.fd:126 `random_float` divides by 2147483647.0, and the state can equal 0x7FFFFFFF, so it can return 1.0. That breaks the documented [0.0, 1.0).
- random.fd:100, :114 modulo bias, and anything above 2^31 can never be produced: `random_int(1<<40)` only covers [0, 2^31). `random_int_range` overflows for spans near i64.
- random.fd:16-25 `RngState` exists, but no function takes one. All state is one global, so there are no independent streams (threads, reproducible subsystems) and no `random_u64`. At least nine modules carry their own xorshift/splitmix for this reason (fuzz, jobs, noise, procgen, wfc, particle_filter, planner, steering, middleware).
- No normal/gaussian, exponential, weighted choice, sampling without replacement, or `random_fill_bytes`. random.fd:174 `random_choice` is i64-only and returns 0 for an empty Vec (ambiguous).
- Tests: none of its own. test_new_stdlib.fd:134-140 calls `random_int` twice, and test_stdlib_test_clock.fd covers seeding. Untested: bool and byte distribution, the float range, shuffle, hex.

### sort
Complete means: fast in-place sorts with O(n log n) worst case, stable sort, sort_by for any comparator and key, binary search, and work on str, f64 and struct Vecs (or a generic Vec<T>).
- sort.fd:285-307 quicksort with the last element as pivot and plain recursion. An already-sorted or reverse-sorted Vec takes O(n^2) time and recursion depth n, so `sort_quick` on 100k sorted elements is a likely stack overflow. The same holds for sort_quick_desc_range (:316) and sort_by_range (:349). core's `Vec_sort` (core.fd:3155) is a heapsort, and this module does not use it.
- No stable sort (merge or Timsort), no sort_by_key, no partial or nth_element, no dedup. Everything is `Vec<i64>` only, with no str, f64 or struct sorting.
- sort.fd:57-128 insertion, bubble and selection sort are exported as user API, which is teaching code rather than library code.
- sort.fd:234 `sort_binary_search` returns any matching index, not the first. Only a lower bound exists (no upper_bound).
- Tests: test_new_stdlib.fd:19-35 (quick, insertion, bubble, binary search on a small Vec). Untested: sorted or adversarial inputs, large n, sort_by, desc, lower_bound.

### serialize
Complete means: a binary writer and reader (fixed-width ints of both endiannesses, f64, varints, strings, bytes) with error reporting, file save and load, and a correct JSON writer with escaping. (A JSON reader belongs to json.fd.)
- serialize.fd:411-417 every native helper (`pledge_f64_to_bits`, `pledge_bits_to_f64`, `pledge_serialize_save`, `pledge_serialize_load`) comes from `pledge_serialize.h`, which does not exist. runtime/ has pledge_audio, font, image, input, net and sdl, but no pledge_serialize.{c,h}. So `serialize_write_f64`, `serialize_read_f64`, `serialize_save` and `serialize_load` cannot build on the C path and have no symbol natively.
- serialize.fd:113-116 `serialize_save` passes `data = 0`, not the buffer: even with a runtime it would write nothing.
- serialize.fd:142-160 `serialize_load` calls the loader twice with a null buffer and returns a Vec of zeros. The comment "native fills data" is false.
- serialize.fd:302, :309 `json_key` and `json_write_string` do no escaping (a quote, backslash or control character produces invalid JSON). Commas are manual (`json_comma`).
- serialize.fd:350-405 the "JSON reader" is a substring search for `"key":` with no whitespace tolerance. It matches nested or earlier occurrences, `json_parse_string` stops at the first `"` (escapes ignored), and the `JsonValue` struct is never used. json.fd already exists.
- serialize.fd:163-168 reads past the end return 0 silently (no error state). No i64/u32/i16, little-endian or varint primitives.
- Tests: test_stdlib_repaired.fd:47-52 checks only the JSON writer. Untested: the binary writer/reader, f64, save/load (which cannot build).

### sdl
Complete means: SDL2 window, renderer, events, keyboard and mouse state, textures, rects and audio, usable from Fides without the user writing C.
- runtime/pledge_input.c:138-150 `pledge_sdl_poll_events` assumes `SdlInputState` holds each Vec inline as `{data,len,cap}` (11 words). In fid a Vec field is one handle to a 40-byte header (AGENTS.md known bug 25), so the C code writes key state through the wrong words. `sdl_poll_events` (sdl.fd:229-249) cannot fill the state correctly.
- sdl.fd:257, :342 the helpers live in runtime/pledge_sdl.c and pledge_input.c. Nothing in compiler/ links them (no reference to pledge_sdl or pledge_input in compiler/*.fd), and docs/roadmap/release-roadmap.md:468 says they are "still stubs returning 0 natively, as on the C path". Input and texture drawing therefore do not work without hand-building the C files.
- sdl.fd:231-236 also copies keys to prev_keys before the C helper copies them again (double work; with the layout fixed it would still be redundant).
- sdl.fd:48, :158 `sdl_fill_rect(ren, rect: i64)` takes an `SDL_Rect*`, but the module has no way to build a rect, so it is unusable. There is also no SDL_RenderCopy src/dst rect, rotation or flip, and no event queue (only polled state: no text input, window resize or wheel).
- sdl.fd:56, :183 `SDL_GetError` returns a borrowed `const char*` typed as an owned `str`, an ownership hazard under proposal 0004.
- sdl.fd:13-30 the usage block is stale: `sdl_poll_events()` and `sdl_key_pressed(SDL_SCANCODE_ESCAPE)` are shown without the state argument.
- Tests: test_lib_sdl.fd (90 lines) covers only the Fides-side state tracker with fields set by hand. Nothing calls SDL (no SDL2 on the machine).

### shader
Complete means: materials with every uniform kind actually uploaded, render state (blend, cull, depth write) applied, a cached uniform location, and a cache that frees programs.
- shader.fd:234-236 mat4 uniforms are uploaded as `gl_uniform_matrix_4fv(u.location, 1, 0, 0)`, a null pointer ("native: pass &mat_val"). The model, view and projection matrices of every built-in shader are never set, so nothing renders where it should. This is the core feature, faked.
- shader.fd:221-224 the location "cache" writes into a local copy `u` of a `&Material` and is never stored, so `glGetUniformLocation` runs every bind.
- shader.fd:204-215 vs :246-259 `blend_mode` and `depth_write` are stored but never applied (no glBlendFunc or glDepthMask). `CULL_FRONT` is never applied either (no glCullFace). gl.fd:190 notes glBlendFunc "needs FFI".
- shader.fd:3-8 the header promises a "shader graph". There is none.
- shader.fd:333-336 `material_cache_clear` drops materials without `gl_delete_program` (leak). No vec2 or array uniforms, and no shader compile error reporting (`valid` only).
- Tests: test_lib_shader.fd checks constants, GLSL text and an empty cache; nothing with a GL context. Untested: bind and uniform upload.

### skel
Complete means: a bone hierarchy with bind pose and inverse bind matrices, keyframe sampling (separate T/R/S channels, step/linear/cubic), linear blend skinning that returns the bind-pose vertex when the pose equals the bind pose, blending, and ideally glTF skin loading.
- skel.fd:123-130 `skel_finalize` sets every `offset_matrix` to the identity ("mat4 has no inverse yet, so it is the identity for now"). There is no inverse bind pose.
- skel.fd:352-374 `skel_skin_vertex` computes `global_rot * (pos - global_pos)`. That is not skinning (it should be global * inverse_bind * pos): in the bind pose it does not return `pos`. The result is wrong for any bone that is not at the origin with no rotation.
- skel.fd:237-241 a bone with no keyframes is set to the identity transform, not left at its bind pose, so a partial animation collapses unanimated bones to the origin.
- skel.fd:96-117 `skel_compute_globals` assumes parents come before children in the Vec (nothing checks or sorts). skel.fd:181-199 keyframes must be added in time order (no sort). There is one combined T/R/S keyframe per time, no step or cubic interpolation, and no glTF import.
- skel.fd:25 the usage block shows `var pose = skel_compute_pose(&skeleton, ...)`, but the function returns void and takes `&mut`.
- `pub struct Animation` (skel.fd:156) clashes with sprite.fd:16 `pub struct Animation` if both are imported.
- Tests: test_stdlib_repaired.fd:28-31 only checks that a keyframe lands in a track. Untested: globals, sampling, skinning, blending.

### scada
Complete means: either a facade over the real industrial-protocol modules (modbus, dnp3, iec104, iec61850) or nothing. As a Modbus implementation it duplicates and lags modbus.fd.
- scada.fd:7-11 the header promises FC 0x02, 0x04 and 0x0F, but there is no `modbus_read_discrete_inputs`, `modbus_read_input_registers` or `modbus_write_multiple_coils` builder.
- scada.fd:285 `modbus_decode_registers` accepts only FC 03 (input registers, FC 04, are rejected). scada.fd:308-322 `modbus_decode_coils` accepts only FC 01 and returns the padding bits of the last byte (no quantity). scada.fd:291 `byte_count` is read and never used.
- scada.fd:257-275 `modbus_frame_deserialize` does not verify the CRC, and a short frame comes back as `is_exception: true` (error and exception conflated). scada.fd:381-396 the TCP parser ignores the protocol id and length.
- There is no server side, no RTU frame-length logic and no client matching. stdlib/modbus.fd (772 lines) has all of these.
- scada.fd:111 `modbus_crc16(Vec<i64>)` and scada.fd:353 `modbus_tcp_frame(...)` have the same names as modbus.fd:50 and :510 with different signatures, so a program importing both modules gets clashing definitions.
- Tests: test_lib_scada.fd (184 lines) covers the builders, CRC and decode for what exists.

### rc
Complete means: shared ownership of a value (`Rc<T>` and `Arc<T>`) that drops the value with the last owner, plus `get_mut`/`make_mut`, weak references and a strong/weak count.
- rc.fd:274-277, :332-335 the "pointer" is an i64 value next to a counter. The counted value is never owned or dropped: rc.fd:322-327 frees only the counter. It is a refcount beside a value, not shared ownership. Core already has counted Boxes (`LOWER_BOX_RC`, `Box_dup`/`Box_drop_t`, AGENTS.md known bug 25).
- rc.fd:323-326, :366-369 a double drop is not detected: `old <= 1` frees on 1, 0 and any negative count, so dropping twice frees the counter twice.
- Rc uses atomics as well (rc.fd:296-305), so it is the same as Arc, with no non-atomic fast path. There is no Weak, no `try_unwrap`, no `ptr_eq`, and nothing generic.
- Tests: test_rc_arc.fd covers count, clone and drop on i64 values. Untested: double drop, drop of the held value.

### set
Complete means: a hash set (generic, or at least str and i64) with insert/contains/remove, iteration or to_vec, union, intersection, difference, subset, and from_vec.
- set.fd:1-87 seven wrappers over `HashMap<str, i64>`: str elements only, with no iteration, no to_vec, and no set algebra (union, intersection, difference, is_subset).
- set.fd has no `module set;` line (the only module in this group without one).
- Tests: none. Nothing in tests/, stdlib/, examples/ or compiler/ imports `set`.

### stack
Complete means: a generic LIFO with push, pop and peek returning Option, len, clear and iteration.
- stack.fd:31-33 `pub struct Stack<T>` declares a type parameter, but `data` is `Vec<i64>` and every method takes or returns i64, so the generic is decorative.
- stack.fd:49-54, :57-63, :86-92 `pop`, `peek` and `peek_at` return 0 on an empty stack. That cannot be told apart from a stored 0 (no Option, no panic variant).
- stack.fd:111-119 `to_vec` copies element by element. `Vec::clone` would do.
- Tests: test_lib_stack.fd (52 lines) covers the basic operations.

### result
Complete means: superseded. core.fd:193 has a generic `enum Result<T, E>`, so this i64-only `ResultValue` is a legacy duplicate that should be deprecated or turned into helpers over core's Result.
- result.fd:398-403 `ResultValue` holds only i64 values and errors (the header says "Fides generics are limited", which is no longer true since proposal 0002).
- result.fd:466 `resultvalue_unwrap_err` panics with "resultvalue_unwrap: called on Err value" when the value is Ok (wrong message). result.fd:499-503 `resultvalue_expect` prints `msg` and then panics with the generic message, not `msg`.
- result.fd:561, :575 return the untyped `Option`.
- Tests: test_new_stdlib.fd:74-80 (ok, err, unwrap_or). Untested: the combinators (map, and_then, or_else), expect, to_str.

### shape
Complete means: tensor shape assertions covering ndim, dims, broadcasting, matmul (batched included), reshape and concat, ideally checked at compile time as the header promises.
- shape.fd:3-5 "When comptime evaluation is available (Phase 3A), these assertions will be checked at compile time. For now, they're runtime checks." Comptime exists now and is not used.
- shape.fd:9 the usage shows `assert_shape(a, "2x3")`, which does not exist. Nothing asserts an exact shape (only ndim, size, same-shape, matmul and reshape).
- shape.fd:89-112 matmul checks 2D only. There are no broadcasting rules, no batched matmul, and no concat/stack/transpose checks.
- shape.fd:131 ten fixed shape constants (`shape_2x3`...) are of little use as API.
- Tests: test_shape.fd and test_shape_verify.fd (63 and 41 lines).

### replay
Complete means: deterministic input recording and playback that captures everything a frame needs (all keys, mouse, analog axes, dt, RNG seed at full width), saves and loads a versioned file with its metadata, and supports seeking.
- replay.fd:124-139 only the first four held keys per frame are kept: further keys are silently dropped (`key_count` is stored, but the keys are not).
- replay.fd:298-315, :416-421 every field, the 64-bit seed included, is written as 32 bits. A 64-bit seed or timestamp is truncated, so playback is not deterministic for such seeds.
- replay.fd:357-358 `replay_load` drops `game_name`, `game_version` and `recorded_at` (never written by `replay_save` either), and `version` is read but never checked.
- replay.fd:91, :120 the default `max_frames` of 3600 (60 s at 60 fps) silently stops recording.
- replay.fd:283-284 `replay_seek` on an empty replay sets `current_frame` to -1. There are no analog or float inputs and no delta compression.
- Tests: test_stdlib_repaired.fd:57-67 runs one frame through save and load. Untested: more than four keys, negative or large seeds, seek, playback end.

### semver
Complete means: SemVer 2.0.0 parse, compare and format, and npm-style ranges (^ ~ x-ranges, hyphen, ||, comparators) with the pre-release rule, plus increment helpers.
- semver.fd:507-519 `~1` (major only) should mean >=1.0.0 <2.0.0 (npm), but the code gives <1.1.0 because `sv_pad` turns it into 1.0.0. Likewise `^0.0` should be <0.1.0 and the code gives <0.0.1 (:494-503). The partial-version information is lost by `sv_pad` (:336-356), although the comment at :312-313 claims prefix bounds.
- semver.fd:336-356, :542 x-ranges (`1.x`, `1.2.*`, `1.X`) do not parse: "1.x" pads to "1.x.0", which is invalid, so the range matches nothing.
- semver.fd:423 a comparator written with a space after the operator (`>= 1.2.3`) splits into two tokens, and `>=` alone fails.
- semver.fd:418 the npm pre-release rule is looser than npm's: any pre-release in the range admits every pre-release, not only those on the same major.minor.patch tuple.
- No `semver_inc` (bump major, minor or patch).
- Tests: test_lib_semver.fd (126 lines: parse, precedence, ^ ~ hyphen ||, max). Untested: partial ~, x-ranges, the spaced operator.

### signal
Complete means: register, check, ignore and restore signals portably, with correct per-OS numbers.
- signal.fd:137 the constants are Linux numbers. On macOS (supported: .github/workflows/macos.yml) SIGBUS is 10, SIGUSR1 30 and SIGUSR2 31, so `sig_register(SIGUSR1)` registers SIGBUS there.
- No way to unregister, ignore (SIG_IGN) or restore the default. No SIGCHLD, SIGWINCH or SIGCONT constants, no blocking mask, and no wait-for-signal (only polling).
- The C runtime (codegen_c.fd:14554) uses `signal()`, not `sigaction`.
- Tests: test_signal_flags.fd (41 lines, SIGINT/SIGTERM raise and check).

### scene
Complete means: within the header's scope (entity templates and spawning), O(1) id lookup, real despawn and slot reuse, update-all, and save/load. A "scene" in most engines is also a transform hierarchy, which is absent.
- scene.fd:156-169 despawn is a soft delete, and nothing ever removes inactive entities: the Vec grows forever, and every lookup (:174, :189) is a linear scan, so an update loop is O(n^2).
- scene.fd:246-255 `scene_update_entity` moves one entity, and inactive ones too. There is no `scene_update_all`.
- scene.fd:61 `next_tag` is never used. Template `flags` (solid, hostile, static) are never consulted. scene.fd:339 `scene_nearest` ignores entities farther than sqrt(999999999).
- No hierarchy, no scene save/load, no per-entity component data beyond the fixed fields.
- Tests: test_game_stdlib.fd and test_scene_entity_updates.fd (68 lines).

### raycast
Complete means: 2D/3D ray versus AABB, circle/sphere, plane, segment, triangle and capsule/OBB, with correct normals, a max distance, inside-start handling, and a grid DDA (tile maps).
- raycast.fd:62, :232 `tmax = 999999.0`: no max-distance parameter, and anything farther is missed.
- raycast.fd:117-127 the 2D AABB normal is chosen by comparing the hit point with face coordinates within 0.001. When the origin is inside the box (tmin = 0) it returns +y. raycast.fd:233, :315-320 3D AABB: when the origin is inside or x is parallel, `hit_axis` stays 0 and the normal is wrong.
- raycast.fd:154-157, :359-362 an inside start clamps t to 0. There is no exit hit and no "inside" flag.
- Missing: a grid/DDA raycast, capsule, OBB, ray versus mesh or list (nearest of many), and a 2D segment-segment intersection with a parameter.
- Tests: test_lib_raycast.fd (132 lines).

### ringbuf
Complete means: as the header states (SPSC, MPMC (Vyukov), overwrite ring); it essentially is.
- ringbuf.fd:114-126 `spsc_pop_batch` with a negative `max` makes `n` negative and moves `tail` backwards (corrupts the ring). It needs `if n < 0 { n = 0 }`.
- ringbuf.fd:83-86 `spsc_peek` on an empty ring returns a stale slot rather than signalling empty.
- i64 elements only. OverwriteRing (:231-261) has no pop or clear.
- Tests: test_std_ringbuf.fd (114 lines, interleaved protocol runs).

### scancode
Complete means: PS/2 set-1 decoding including E0-prefixed extended keys (arrows, right Ctrl/Alt, keypad Enter or /), Ctrl/Alt/NumLock state, and key codes for non-ASCII keys.
- scancode.fd:372-386 `sc_mod_update` tracks only Shift and CapsLock. `SC_LCTRL` (:346) is defined and never used, and Alt and NumLock are absent.
- No E0 prefix handling: arrows, Home/End and right modifiers decode as keypad digits (SC_MAP 71-83).
- scancode.fd:399-414 no key codes for F1-F12 or arrows (only ASCII). No set-2 translation. test_std_ps2.fd suggests ps2.fd covers part of this; check before extending.
- Tests: test_os_stdlib.fd:198-205 (5 checks) and test_std_ps2.fd.

### simd
Complete means: vector ops over f64 and f32 and integer lanes: arithmetic, min/max, abs, sqrt, fma, compares and select, horizontal reductions, with native lowering.
- simd.fd:154-158 the header lists f32x4 and f32x8 widths, but there are no f32 operations: everything is Vec<f64>.
- No min, max, abs, sqrt, fma, compare or select, shuffle, or i64 mul/shift. `sum` (:208) has no native vector lowering (only add, sub, mul, div, scale and dot per :144-150).
- Lengths are not checked for the f64 builtins (behaviour on unequal lengths is undocumented). The i64 ones panic.
- Tests: test_simd.fd (49 lines), test_simd_i64.fd (167), test_machine_simd.fd, test_matrix_mul.fd.

### shard
Complete means: tensor-parallel collectives (all_reduce with a reduction op, broadcast, all_gather, scatter, reduce_scatter, barrier, send/recv to a chosen rank) over real ranks.
- shard.fd:186-190 the header says "In single-device mode (interpreter), these are no-ops". That is stale: the C runtime runs one process per rank (codegen_c.fd:4233-4587), as do linux_runtime/sys.fd:3665 and pe_runtime/conc.fd:4079.
- `all_reduce` (:202) has no op (sum only, with no mean/max/min). There is no reduce_scatter, no barrier, and no peer rank for `send_tensor`/`recv_tensor` (:222-229).
- Tests: test_shard.fd (30 lines) and tests/tp/ (multirank, rank_exit, rank_fail).

### sprite
Complete means: frame animation with per-frame durations and atlas-region frames, ping-pong/once/loop modes and events, atlas loading (a packer JSON), palette swap, and a controller.
- sprite.fd:77 `frame_duration <= 0` loops forever in `sprite_anim_update` (`while elapsed >= 0`).
- sprite.fd:16-30 frames are bare indices 0..n with one uniform duration: an animation cannot name atlas regions or hold per-frame durations. No ping-pong, no reverse, no frame events.
- No atlas type (a set of named regions) and no atlas loader (TexturePacker/Aseprite JSON). No drawing or batching (left to the renderer, but not documented).
- sprite.fd:268-277 palette lookup is a linear scan per pixel (O(pixels x entries)).
- sprite.fd:16 `Animation` clashes with skel.fd:156.
- Tests: test_game_stdlib.fd and test_sprite_stream_updates.fd (51 lines).

### vec
Complete means: generic higher-order helpers for any `Vec<T>` (map, filter, fold, find→Option, zip→pairs, chunks→sub-Vecs, windows, dedup, a stable O(n log n) sort_by, min/max/sum/count/retain/partition/join) on top of core's Vec_*.
- vec.fd:45-199: every method and free function takes and returns `Vec<i64>` with `fn(i64)` callbacks. Generics have existed since 0002, but the module still relies on the compiler treating elements as raw words. `map` on `Vec<str>` is special-cased and tested (test_vec_map_change_type.fd, known-bugs 2g.66); `filter`, `find`, `any`, `fold` and the others on str or struct elements are not checked.
- vec.fd:129-140: `find` returns -1 when nothing matches, so it cannot be told apart from an element equal to -1. It should return Option.
- vec.fd:263, 289: `vec_zip` and `vec_enumerate` give interleaved flat Vecs, not pairs.
- vec.fd:303: `vec_flat_map` takes `fn(i64) -> i64` and treats the result as a Vec handle, which is not type safe.
- vec.fd:328-335: `vec_chunk` returns chunk start indices, not chunks, and loops forever when `size <= 0` (`i = i + size`; vec_window has the guard at :350, chunk does not).
- vec.fd:347: `vec_window` also returns indices, not windows.
- vec.fd:395-411: `vec_sort_by` is an insertion sort, O(n^2).
- vec.fd:20 and :157-159: stale comments (a 3-word PVec layout, where the header is now 40 bytes; "Vec_new() returns i64").
- Missing: min/max/sum, count, retain, partition, reduce without an initial value, binary_search, join, last/rposition.
- Tests: map, filter and fold are covered by test_lang_features.fd, test_closure_devirt.fd and test_vec_map_change_type.fd. take, skip, zip, enumerate, flat_map, chunk, window, dedup, sort_by and contains_val are covered only by their doc examples; no bootstrap test uses them.

### streaming
Complete means: a chunk manager that loads chunks within the radius and really unloads (or evicts by LRU) chunks outside it, with load/unload hooks or a list of state changes the caller acts on, correct handling of negative coordinates, and a test.
- streaming.fd:163-168: the branch that should refresh `last_used` is an empty "for now" stub, so LRU never works.
- streaming.fd:173-189: chunks outside the radius are never unloaded. The comment says so ("Can't mutate in-place, so we skip"). Their `state` stays 2, so `streaming_is_chunk_loaded` keeps returning true after the focus moves away, and memory grows without bound.
- streaming.fd:185: `unload_count` and `changes` go up on every call for every far-away chunk, so the counter is wrong.
- streaming.fd:82, 124-125, 227: `x / chunk_size` rounds toward zero, so world x = -1 maps to chunk 0 instead of -1.
- streaming.fd:13: `streaming_get_loaded_chunks` is in the usage text but does not exist. There are no load/unload callbacks or events, so a game cannot learn which chunks to fill or save.
- streaming.fd:93: chunk lookup is a linear scan (no map), so each update costs O(chunks x radius^2).
- Tests: none (only the doc example).

### virtio
Complete means: a driver-side virtio library: split-virtqueue layout, descriptor reads and writes, avail/used rings with flags and event_idx (VIRTIO_F_EVENT_IDX), indirect descriptor tables, free-list management, device status and feature-bit negotiation constants (and optionally the packed ring), with a test that runs a fake device.
- virtio.fd:22-58: only the offsets and sizes are computed (legacy 4096-byte used-ring alignment is not offered).
- virtio.fd:85-95: there is a descriptor writer, but the only reader is `vq_desc_flags`. Nothing reads a descriptor's addr, len or next.
- virtio.fd:98-123: avail/used flags (VRING_AVAIL_F_NO_INTERRUPT, VRING_USED_F_NO_NOTIFY) and `used_event`/`avail_event` are not handled. There is no "consume used entries since last_seen" helper (the driver has to track it).
- virtio.fd:15: VQ_F_INDIRECT is defined, but nothing builds or reads indirect tables.
- Missing entirely: device status bits (ACKNOWLEDGE, DRIVER, FEATURES_OK, DRIVER_OK, FAILED), feature bits (VERSION_1 and the rest), a negotiation helper, and the packed virtqueue.
- virtio.fd:79-82: `wu64` gets the high word with `v / 4294967296`, which is wrong for addresses with bit 63 set. Use a shift and a mask.
- virtio.fd:115-116: a stale doc comment describes a combined id/len read that no longer exists.
- Tests: none (doc example only).

### time
Complete means: deterministic UTC civil-time math (construct, validate, extract, add, diff, compare, format) plus a monotonic clock for elapsed time. Parsing, durations and formatting live in datetime.fd, which is fine.
- time.fd:46: `time_new` does not check its inputs. Month 13, day 31 in February and hour 25 are all accepted and silently normalized. There is no Result or Option form.
- time.fd:290-292: `time_diff_days` uses truncating division, so a difference of -1 s counts as 0 days. It is inconsistent with `time_floor_days` at :119.
- time.fd:427-430: the day-of-year calculation goes through the same truncation. It is fine for valid inputs.
- time.fd:491-535: `clock_elapsed_*` is built on the wall clock (realtime), which can jump. A monotonic `clock_mono_ns` would suit profiling. (The C runtime already uses CLOCK_MONOTONIC and QueryPerformanceCounter elsewhere, in codegen_c.fd:8147/8511.)
- time.fd:438-479: `days_in_month` returns 30 for an invalid month instead of signalling an error.
- No add_months or add_years with end-of-month clamping (datetime.fd may cover this; not in time.fd).
- Tests: test_time.fd, test_time_edge.fd and test_stdlib_test_clock.fd cover construction, extraction, ISO output, leap years and the weekday at negative times. `clock_now_*`/`clock_elapsed_*` (apart from the fake clock), `time_sub_seconds`, `time_add_minutes`/`hours`, `time_after`, `time_eq` and `time_before_eq` are covered only by doc examples.

### tls_record
Complete means: the RFC 8446 §5 record layer for the cipher suites TLS 1.3 requires (AES-128-GCM-SHA256 at least, plus ChaCha20-Poly1305), sending and receiving with fragmentation and size limits, and known-answer tests.
- tls_record.fd:262, 285: seal and open support only TLS_CHACHA20_POLY1305_SHA256. RFC 8446 §9.1 makes TLS_AES_128_GCM_SHA256 mandatory, and tls_crypto.fd has no AES-GCM at all (its public API is chacha20/poly1305/hkdf/x25519).
- tls_record.fd:160-169: `tls_record_encode` writes any fragment length into a 16-bit field with no splitting and no check against 2^14. A handshake message over 16 KiB (a certificate chain) yields an oversized or wrapped record.
- tls_record.fd:262-281: `tls_record_seal` has the same problem: no `content + pad <= 2^14 + 1` check.
- tls_record.fd:125-157: a ChangeCipherSpec record is not checked to be exactly the single byte 0x01 (RFC 8446 §5).
- No key update or per-direction sequence-number state (the caller passes `seq`); this is an acceptable seam.
- Tests: test_runtime_tls_record.fd (137 lines) checks parsing, the error codes and the seal/open round trip, but only against itself. There is no RFC 8448 record vector. Fuzzing is via tests/fuzz/parsers.fd.

### vorbis
Complete means: an Ogg Vorbis I decoder (done: all floor/residue/codebook types, verified against ffmpeg) plus whole-file handling of chained streams, a correct start offset and seeking.
- vorbis.fd:243-262: `ogg_read` keeps only the first logical stream (`if p.serial != st.serial { continue; }`). Chained Ogg files (a common case for concatenated tracks or radio dumps) lose everything after the first link.
- vorbis.fd:2085-2140: no seeking or granule-to-page index. Output is trimmed only at the end (`last_granule`). A stream whose first audio page has a granule that is not 0 (cut streams, RFC 7845-style start offset) is not trimmed at the start.
- vorbis.fd:2119-2128: each packet does `vf.pcm.get(c)` then push, then `set`. Under copy-on-write sharing this can copy the whole channel buffer on every packet, which is quadratic. The header already says files are "too slow for the interpreter".
- Comments are kept as raw strings; there is no `KEY=value` lookup (minor).
- Tests: good (test_std_vorbis.fd, 532 lines: real libvorbis files, coupled stereo, floor 0, bark map, CRC re-encode). Chained streams and the start offset are untested.

### svd
Complete means: a CMSIS-SVD 1.3 reader that turns everything a driver generator uses (peripherals, registers, clusters, fields, enums, interrupts, arrays, every derivedFrom form) into regmap.
- svd.fd:24-25: by its own header, `<dimArrayIndex>`, derivedFrom on registers and fields, `<writeConstraint>` and `<interrupt>` are not supported. Dropping interrupts loses the IRQ numbers drivers need.
- svd.fd:299-304: a derived peripheral copies only `regs`. It does not inherit the source's description or properties, does not accept dotted paths ("PERIPH.REG"), and does not resolve a peripheral defined later in the file (forward reference fails at :302).
- svd.fd:151-192: field-level `dim` arrays are not expanded. `<enumeratedValue><isDefault>` and `usage` are ignored, and a pattern value with 'x' is skipped rather than expanded.
- svd.fd:237: a cluster array's `dimIncrement` defaults to 0, so all copies land at one offset when it is omitted (it is required by the schema, so this is minor).
- `<addressBlock>`, `<cpu>` and `<headerStructName>` are dropped.
- Tests: good (test_std_svd.fd covers every supported feature). Nothing unsupported is tested.

### tar (COMPLETE)
Reads and writes ustar/pax/GNU-long-name archives in memory, byte for byte against Python's tarfile. Minor points only:
- tar.fd:125-133: base-256 drops the sign bit (bit 6 of the first byte), so negative values are misread.
- No GNU sparse ('S') entries.
- tar.fd:459: a long directory name ending in "/" is never split; it falls back to a pax header, which is still valid.

### typed_strings

Complete means: typed wrappers that really prevent injection. Bind quotes and escapes each value in order. Escapes are correct for SQL, shell, HTML (text and attribute), JSON and path. A wrapped value can only be built from trusted text or through an escaper.

- typed_strings.fd:120-126 `sql_bind`:
  - The doc says it replaces the first `?`, but `str_replace` replaces every `?`. After one bind no placeholder is left, so a query cannot take two parameters.
  - A `?` inside a quoted literal of the query is replaced too.
  - The value is not quoted; only `'` is doubled. `sql_bind(sql_query("... WHERE id = ?"), "1 OR 1=1")` gives `id = 1 OR 1=1`. That is SQL injection through the function documented as "the safe way". MySQL-style backslash escapes are not handled either.
- typed_strings.fd:129 `sql_bind_int` also replaces every `?`.
- typed_strings.fd:37-39, 94, 101: `value` is `pub`, and `sql_query`/`sql_literal` accept any `str`. So `sql_query(user_input)` and `SQLString { value: user_input }` both compile. The compile-time check only stops a raw `str` passed straight to a parameter typed SQLString.
- typed_strings.fd:283-291 `json_escape`: only `\\ \" \n \t` are escaped. `\r`, `\b`, `\f` and the other characters below 0x20 pass through raw, which gives invalid JSON. The comment at 282 claims control characters are handled.
- typed_strings.fd:320-329 `path_safe`:
  - It does not reject absolute paths (`/etc/passwd`, `C:\x`, `\\server\share`), although line 297 promises it does.
  - It rejects any `..` substring, so a legitimate name like `a..b` is refused.
  - Failure is an empty PathString; an empty path can mean the current directory.
- typed_strings.fd:58-83, 158-191 `is_safe`: blacklist heuristics. The shell version misses `\r`, glob characters, `{`, `!` and `#`.
- typed_strings.fd:9-27: the header example uses APIs that do not exist (`value` keyword, `db.query`, `MemoryDB`, `p_exec`).
- Tests: test_typed_strings.fd covers only the happy paths, and it locks in the unquoted bind output (`id = 42`). There is no adversarial input (a quote or `OR` in a bound value, two placeholders, control characters in JSON, absolute paths). test_typed_strings_fail.fd checks only the raw-str-to-SQLString error.

### sunspec

Complete means:
- marker scan and model walk with bounds checks;
- every point decoded, with SunSpec's "not implemented" values handled;
- the common inverter, meter, nameplate, settings, status, controls, storage, MPPT and battery models;
- writes to the control models;
- reading a live device over a ModbusClient.

- sunspec.fd:258-277 `sunspec_meter`: the offsets do not follow the model 203 point list. I checked from memory of the list; verify it against the SunSpec model JSON.

  | point | module reads at | 203 map |
  |---|---|---|
  | PhV (average line-neutral voltage) | 8 (PhVphC in the map) | 5 |
  | V_SF | 15 (Hz_SF in the map) | 13 |
  | Hz / Hz_SF | 16 / 17 | 14 / 15 |
  | W / W_SF | 18 / 22 | 16 / 20 |
  | TotWhExp | 38 (TotWhExpPhA in the map) | 36 |
  | TotWhImp | 42 (TotWhExpPhC in the map) | 44 |
  | TotWh_SF | 54 | 52 |

  So a real meter would be read wrong, and no test covers this function.
- sunspec.fd:54-86: SunSpec's "not implemented" values (int16 0x8000, uint16 0xFFFF, int32 0x80000000, acc32 0, scale factor 0x8000) are not recognised. They come back as real readings, and a scale factor of -32768 loops 32768 times.
- sunspec.fd:151-164, 224: `sunspec_walk_models` does not check that a model's length fits in `regs`. The `sunspec_inverter` and `sunspec_meter` readers do not check the model length either. A short or truncated model makes them panic.
- sunspec.fd:44-49: only models 1, 101-103 and 203.
  - Missing: 120-124 (nameplate, settings, status, controls, storage), 160 (MPPT), 111-113 (float inverters), 201/202/204 (other meters), 7xx (DER), 802 (battery).
  - In the inverter models, the DC, temperature, operating state and event points (offsets 25-39) are not decoded.
- No write path: controls such as WMaxLimPct and Conn cannot be set.
- sunspec.fd:283: works only on a ModbusMap image. There is no read over a ModbusClient and no probing of the usual base addresses (0, 40000, 50000).
- Tests: test_std_sunspec.fd covers the marker, the walk, the common model and model 103. Missing: the meter, models 101/102, not-implemented values, truncated models.

### tensor

Complete means:
- create from f64 data, plus zeros, ones, eye, arange and random;
- elementwise maths (+ - * /, exp, log, tanh, pow) with broadcasting;
- reductions over an axis, argmax;
- slicing, reshape, transpose, concat;
- matmul and dot, activations;
- conversion to and from Vec<f64>.

- tensor.fd:28 `from_vec(data: Vec<i64>, ...)`: the builtin converts each element as an integer (`(double)(int64_t)` at compiler/codegen_c.fd:15073, `x as f64` at stdlib/pe_runtime/tensor.fd:497). The only way to load f64 data is `set()` one element at a time.
- Missing operations: elementwise div, exp/log/tanh/pow, adding a scalar, broadcasting, sum/mean/max along an axis, argmax, slicing or views, concat/stack, eye/arange/random init, comparisons, conv, flat access, to/from Vec<f64>.
- tensor.fd:13-160: the names `get`, `set`, `add`, `sub`, `max`, `min`, `size`, `clone`, `to_str` are unprefixed and collide with other modules a program imports.
- tensor.fd:1-8: the header is stale ("Pledge", "Phase 3A").
- Tests: test_tensor.fd (114 lines) covers the existing wrappers. Untested: to_json, clone, and float data, which cannot be loaded anyway.

### unicode

Complete means: strict UTF-8 decode and validation, full simple and special case mapping, case folding, general category, normalization (present), UAX #29 grapheme clusters, and display width.

- unicode.fd:7-13: the header says there is no full NFC/NFKC, but `uni_nfc`, `uni_nfd`, `uni_nfkc` and `uni_nfkd` (730-787) are full, from the Unicode 16 tables in unicode_data. The header is stale.
- unicode.fd:130-248, case mapping:
  - Covered: ASCII, Latin-1, Latin Extended-A, basic Greek, part of Cyrillic.
  - Missing: Latin Extended-B (except U+0187), Latin Extended Additional (Vietnamese U+1E00-1EFF), accented Greek (U+0386, U+0388-038F and their lowercase), Cyrillic U+048A-04FF, Armenian, Georgian, Cherokee.
  - Wrong at 140: U+0130 İ lowercases to U+0131 ı (should be i).
  - Wrong at 208: U+0131 ı uppercases to U+0130 (should be I).
  - At 204: ß goes to ẞ, which is not Unicode's simple mapping.
  - No special casing (ß to SS).
- unicode.fd:273: no case folding. `uni_equal_fold` compares lowercased strings, so `ς` vs `Σ` compare unequal.
- unicode.fd:284-323, classes:
  - `uni_is_alpha` misses Thai, Lao, Georgian, Ethiopic, the Indic scripts other than Devanagari, Latin Extended Additional, CJK Extension A (U+3400-4DBF) and later, and Hangul Jamo.
  - `uni_is_digit` misses most Nd ranges.
  - No general-category query.
- unicode.fd:37-68, 107-125, decoding:
  - `uni_to_codepoints` accepts overlong forms (C0 80), surrogates (ED A0 80) and F5-F7 lead bytes.
  - `uni_len` counts a stray continuation byte as a 2-byte character, so it disagrees with `uni_to_codepoints` on invalid text.
  - No validation function.
- unicode.fd:349-392, graphemes (only an approximation of UAX #29):
  - No CR LF rule (GB3).
  - No Hangul L/V/T rules (GB6-8), so `uni_nfd` of a Hangul syllable splits into 2-3 clusters.
  - No SpacingMark or Prepend: Indic matras are not in `uni_is_combining` (284).
  - After a ZWJ any codepoint joins, where GB11 requires Extended_Pictographic.
- Missing: East Asian width / display width (terminals), word boundaries.
- Untyped `Vec` throughout.
- Tests: test_lib_unicode.fd (214 lines) covers what exists. No İ/ı, CRLF, Hangul-cluster or invalid-UTF-8 cases.

### tls_crypto

Complete means: ChaCha20, Poly1305, AEAD, HKDF and X25519 that are correct, constant-time where keys are involved, tested against RFC vectors, with an explicit ok/fail result from open.

- tls_crypto.fd:472-552 `x25519`:
  - No test vector and no caller: the tls13 test and quic_tls use stdlib/x25519.fd's `x25519_scalarmult`.
  - Line 489 does not mask bit 255 of the u-coordinate, which RFC 7748 §5 requires. RFC vector 2's u ends in 0x93, so it would give a wrong result.
  - Line 506 branches on key bits, and bigint arithmetic is variable-time, so it leaks the key through timing.
  - No all-zero output check (RFC 8446 §7.4.2).
- tls_crypto.fd:126-200 `poly1305_tag`: built on bigint, so slow and variable-time. stdlib/chacha20poly1305.fd has its own implementation.
- tls_crypto.fd:278-313 `chacha20poly_decrypt`: returns an empty Vec on a bad tag, which looks the same as a valid empty plaintext. tls_record.fd:305 and quic.fd:403 work around it because their plaintexts are never empty. chacha20poly1305.fd's `aead_chacha20poly1305_open` already returns ok/data.
- tls_crypto.fd:66: the block counter silently wraps at 2^32.
- tls_crypto.fd:387: `hkdf_expand` does not enforce the 255×32-byte limit.
- tls_crypto.fd:11-14: the header is stale; it says the handshake and X.509 are still to be done.
- Suggested fix: make this a thin facade over chacha20poly1305.fd and x25519.fd and keep the API.
- Tests: test_crypto_tls_vectors.fd covers RFC 8439 (block, stream, Poly1305, AEAD) and RFC 5869 case 1. x25519 is untested. `hkdf_expand_label` is tested only indirectly, through the tls13 test.

### tls13

Complete means: the whole RFC 8446 §7 key schedule for both hash sizes, including 0-RTT, the exporter, resumption and binders. A handshake for TCP would belong in a separate module.

- tls13.fd:31, 38: SHA-256 only. TLS_AES_256_GCM_SHA384 is not supported.
- Not derived:
  - the client early traffic secret ("c e traffic") and the early exporter ("e exp master"), so no 0-RTT;
  - the TLS-Exporter function (§7.5): only its secret (220-232);
  - the resumption PSK from a ticket nonce (§4.6.1, label "resumption");
  - the PSK binder value: only the key, at 135.
- No handshake messages or state machine for TLS over TCP. quic_tls.fd has them for QUIC; TCP TLS uses tls_unix/tls_windows.
- Tests: test_stdlib_tls13_schedule.fd checks RFC 8448 §3 end to end, which is good. Not tested directly: `tls13_binder_key`, `tls13_next_traffic_secret`, `tls13_early_secret` with a non-empty PSK.

### toml

Complete means: a strict TOML 1.0 parser (present), a serializer, typed accessors, and date values as structs.

- No writer. Nothing turns a TomlDoc or `Vec<TomlEntry>` back into TOML text: `toml_to_json` (1571) only gives the toml-test JSON form.
- toml.fd:1341-1446: the document accessors return text only. There is no `toml_node_int`, `toml_node_float` or `toml_node_bool`, and dates and times stay strings with no struct.
- toml.fd:1683: in the flat form, `toml_get_int` silently truncates a float.
- No API to build or edit a document (set or insert).
- Tests: good. test_toml_spec.fd has the spec examples; test_toml_suite_spec.fd has 24 valid toml-test files and 129 invalid ones; test_lib_toml.fd covers the flat API; tests/fuzz/parsers.fd also uses it.

### vmm

Complete means: map, unmap and protect over ranges, address-space teardown and clone, rollback when a map fails, and large-page split and merge, for all four page-table formats.

- vmm.fd:710: `vmm_unmap` removes one mapping per call; there is no range unmap.
- vmm.fd:753: `vmm_protect` changes one leaf. It cannot change part of a large page, because there is no split.
- vmm.fd:645-661: `vmm_map` leaves earlier pages mapped when a later one fails (no rollback).
- No `vmm_destroy`: the table frames of a whole address space are never freed. No address-space clone (fork).
- No 5-level paging (x86 LA57, Sv57). No PAT/MAIR choice beyond normal and device memory (no write-combining).
- Tests: test_std_vmm.fd covers x86-64, AArch64, Sv39 and Sv48 for map, translate, unmap, protect, the large-page choice and the register values. `phys_read_le`/`phys_write_le`/`phys_read_bytes` are not tested here; e1000 and xhci tests use them.

### trajectory

Complete means: 1-D profiles (present), plus validated inputs, non-zero start and end speeds, waypoint blends, and synchronisation of several axes.

- trajectory.fd:473-486: `traj_times` never stops when `dt <= 0` and `total > 0`; it keeps pushing until memory runs out.
- trajectory.fd:171-182: a spline with fewer than 2 knots panics (`t.get(1)`). Repeated knot times divide by zero.
- trajectory.fd:289-303, 374: amax or vmax <= 0 gives NaN or infinity; nothing is checked.
- trajectory.fd:289, 374: trapezoid and S-curve are rest-to-rest only, with no start or end velocity.
- Missing: blends through several waypoints with velocity limits (linear segments with parabolic blends), syncing several axes to one duration, and scaling a profile to a given duration.
- Tests: test_std_trajectory.fd covers each profile's evaluation. Not tested: `traj_sample_spline`, `traj_sample_trapezoid`, `traj_times` edge cases, and every invalid input.

### tariff

Complete means: flat, time-of-use, tiered and dynamic energy charges, plus demand charges and export credits (present), with validated inputs, sub-hour time-of-use, seasons and holidays, taxes and a minimum bill.

- tariff.fd:275 and 331: `dynamic_prices.get(i)` and `dynamic_export.get(i)` panic when there are fewer prices than intervals. tariff.fd:162 lets `import_wh` and `export_wh` be built per interval, but nothing checks the lengths.
- tariff.fd:168-185: time-of-use periods are whole hours. Boundaries like 16:30 cannot be expressed, and a 15-minute interval is matched only by its hour.
- tariff.fd:80-85: a profile has only a start day of the week. There are no months or seasons (summer/winter rates) and no holidays.
- tariff.fd:271-279: the modes exclude each other, so tiers cannot be combined with time-of-use. Tier limits are not prorated to the billing days.
- No taxes or VAT lines, no minimum bill, and only one demand charge (143).
- tariff.fd:365: `interval_minutes` of 0 divides by zero.
- Tests: test_std_tariff.fd (150 lines) covers every mode and export option. No test of mismatched lengths or bad input.

### uuid
Complete means: RFC 9562/4122 UUIDs: v4 from a secure or at least per-process-seeded source, v7 (time-ordered) and v5/v3 (name-based), strict parse returning Option, format/compare.
- uuid.fd:34-50 `uuid_v4` takes `random_int_range(0, 255)`, which is random.fd's global 31-bit LCG whose state starts at 1 (random.fd:23-25, 70-75). Nothing seeds it, so every run of every program yields the identical UUID sequence: UUIDs collide across processes, which defeats their purpose. Also `% 256` of an LCG keeps its weakest low bits. Fix: draw bytes from `p_os_random_u64` / `secure_os_random` (secure_store.fd:138).
- uuid.fd:4-5 the header admits "not cryptographically secure" but offers no secure variant.
- No v1, v3/v5 (md5.fd exists; no sha1 found), v7.
- uuid.fd:140-176 `uuid_parse` skips any non-hex byte, so junk around or inside (e.g. "zz123e4567-...zz", 33+ hex digits) is accepted, and a lone nibble before a non-hex byte becomes `hi<<4`; failure returns the nil UUID instead of an Option, so a real nil UUID and an error look the same.
- No ordering compare (uuid_cmp), no from/to u128 or two i64.
Tests: test_new_stdlib.fd:125-132 (length, version, nil) and doc examples. Untested: parse edge cases, uniqueness across runs, variant bits on many samples.

### sync
Complete means: threads plus the usual primitives: mutex (lock/try_lock/destroy), a reader-writer lock that really admits concurrent readers, condition variable, semaphore, once, barrier, atomics, TLS, channels, with typed handles.
- sync.fd:133-166 `sync_rwlock_read_lock` holds the mutex for the whole read section of the first reader; a second reader blocks on `p_mutex_lock` until the reader count returns to 0. Readers are therefore serialized; the "multiple concurrent readers" promise (sync.fd:111, 360) does not hold.
- No condition variable, semaphore, once/lazy init, barrier or wait group anywhere in stdlib (grep of stdlib: only rtos/xhci/vulkan mention semaphores, for their own domains); the runtime has only `p_mutex_init/lock/unlock`. Needs new builtins on every backend.
- No `try_lock`, no `sync_mutex_destroy`, no free for `p_shared_alloc` slots (sync.fd:35, 61, 115-120): every mutex, rwlock and atomic slot leaks.
- sync.fd:260-265 doc says `try_recv` returns 0 when closed and empty while the comment at 229-238 and `SyncChannel.try_recv` (486-489) say SYNC_CHAN_PENDING; 0 is also a valid value. Channels carry only i64.
- sync.fd:218-220 `sync_spawn` takes a function by name string (untyped).
- sync.fd:6 header says native threads are "pending" (stale or true, should be checked against pe_ir/nat_ir).
Tests: mutex, atomics, TLS, channels, rwlock write path exercised (test_lock_order_sync*, real_threads, test_async_threads ...). Untested: two concurrent readers, try_recv on a closed channel, the typed Sync* wrappers (one use each).

### ui
Complete means: an immediate-mode GUI that draws readable text (through font.fd's bitmap or TTF fonts), editable text fields with keyboard input, basic layout (rows/columns/windows), and the standard widgets.
- ui.fd:107-124 `ui_draw_text` is a declared placeholder ("This is a placeholder — real text rendering uses font.fd"): it draws three pixels per non-space character, so every button label, checkbox label and `ui_label` is unreadable. font.fd has `font_bitmap_text` / `font_render_text` (font.fd:193, 338) that could be used.
- ui.fd:305-334 `ui_text_input` returns `text` unchanged ("no editing yet"); no keyboard state is read in `ui_begin`.
- ui.fd:88-95 rectangles are filled with one line per row (no `SDL_RenderFillRect`), slow for large panels.
- No layout helpers beyond `ui_next_row`, no windows/scroll, no keyboard navigation, no style setters (colors only via struct fields).
- ui.fd:83 hover test ignores widget overlap (no hot-id arbitration across the frame).
Tests: test_lib_ui.fd covers context defaults, `ui_begin` mouse reading, `ui_end`, `ui_next_row`. No widget is ever called (they need an SDL renderer); a fake renderer seam would let button/slider/checkbox logic be tested.

### template
Complete means: a text template engine with variable substitution (dotted paths), filters including `default` and HTML escaping, conditionals, loops, clear errors for unclosed tags/unknown filters.
- template.fd:22-23 `{% if %}` and `{% for %}` documented as "not yet implemented"; `{%` is not parsed at all (it stays literal text).
- template.fd:162-167 the `default` filter is a no-op stub ("not yet supported").
- template.fd:206-211 `comptime_template` is a placeholder that just calls the runtime parser; header lines 3-10 and 191-204 promise comptime generation.
- No HTML/attribute escaping filter: a web template renders user data raw (XSS risk).
- template.fd:135 `json_get` panics when `data` is not a JSON object (json.fd:802-803) and revalidates the whole JSON document for every variable (O(vars x len)); no dotted paths (`{{user.name}}`), no arrays.
- template.fd:152-170 unknown filters and filter arguments (`{{x|default:"n/a"}}`) are silently ignored.
- template.fd:97-100 an unclosed `{{` is literal text without any error.
Tests: one render line in test_stdlib_repaired.fd:26 plus doc examples. Untested: missing keys, unclosed tags, unknown filters, render_file.

### timeseries
Complete means: ring storage, range queries, downsampling, and a lossless Gorilla codec that round-trips every input as the header claims.
- timeseries.fd:337-345 encoder ranges are -63..64, -255..256, -2047..2048, but the decoder (441-445) sign-extends 7/9/12 bits (`ts_sext`, 292-300), whose ranges are -64..63, -256..255, -2048..2047. A delta-of-delta of exactly 64, 256 or 2048 is written as bit pattern 1000000 etc. and decoded as -64/-256/-2048: timestamps silently corrupted (e.g. times 0, 0, 64). Fix: use ranges -63..63 / -255..255 / -2047..2047 (or offset coding as in the paper).
- No resampling/interpolation, rate/derivative, or percentile per bucket (polish).
- `ts_ring_clear` (84-87) leaves `dropped` as is (minor).
Tests: test_std_timeseries.fd round-trips odd values and four dod buckets but none of the three boundary values, which is why the bug is not caught.

### tcpip
Complete means: an OS-independent IPv4 stack whose TCP is safe to use on a real network: RFC 5681 congestion control (slow start + avoidance + fast retransmit/recovery), persist and keepalive timers, unpredictable ISNs (RFC 6528), connection reclamation, ephemeral ports; ARP, IPv4 fragmentation, ICMP, UDP. (IPv6, SACK, window scaling are honestly listed as not done, tcpip.fd:22-24.)
- Persist timer missing: when all data is acked the retransmission timer is turned off (tcpip.fd:1271-1272), and `tcp_output` does not arm anything when the window is 0 (893-895). The zero-window probe at 1381-1387 only runs from `tcp_retransmit`, so after a zero window with a lost window update the connection stalls forever.
- Congestion control: cwnd grows by one MSS per ACK forever (1268) and drops to 1 MSS on timeout (1357); no ssthresh, no congestion avoidance. `dup_acks` is counted (1282-1284) and never used (no fast retransmit, as the header admits).
- ISN (831) is MAC bytes + now*250 + conns*64000: predictable, RFC 6528 recommends a keyed hash.
- Connections are indices into `s.tcp` and are never removed (CLOSED entries pile up, `tcp_find` scans them all); no UDP unbind; `tcp_connect` needs the caller to pick a local port (no ephemeral allocation or collision check, 935).
- Reassembly keeps unlimited buffers per source (519-541): no cap on buffers or total size (memory exhaustion by fragments). Datagram to own IP goes out as an ARP request for itself (no loopback).
- No delayed ACK, keepalive, ICMP time-exceeded/frag-needed handling, DHCP.
Tests: test_std_tcpip.fd is good (checksum, ARP, ping, UDP, unreachable, fragmentation both orders, handshake, loss + retransmit, close in both orders, RST, timers). Untested: zero window, many connections, simultaneous open/close (CLOSING), RTO backoff to "timed out".

### tz
Complete means: named zones with full history and current rules on every supported OS, POSIX TZ strings, local<->UTC with gap/overlap rules, formatting.
- tz.fd:568-582 `tz_load` reads TZif files only from Unix paths (547-563). On Windows (the project's main platform) every zone falls back to the built-in table (123-194): 50 zones, 2026 rules only, so any date before a zone's current rules is converted with today's rules. Options: ship tzdata with the stdlib, or map Windows zone names/registry.
- tz.fd:198-218 an unknown name silently returns UTC (`tz_known` exists, but `tz_find` gives no error/Option).
- Table approximations: Chile (139-142) uses first Sunday 04:00 UTC instead of the real rule; Casablanca (160) is fixed +01 though it changes around Ramadan.
- `tz_names` lists only the table, not the system database.
Tests: test_tz.fd, test_tz_julian.fd, test_tzif.fd are strong (checked against zoneinfo and glibc). Untested: Windows fallback path for historical dates, malformed TZif bytes.

### usb_device
Complete means: a generic chapter-9 device core that serves any configuration it is given (any number of interfaces and alternate settings, optional several configurations), with class requests dispatched to the class that owns the interface, plus CDC-ACM as one such class.
- usb_device.fd:403-413 GET/SET_INTERFACE stalls for interface index > 1 and any alternate setting != 0: only the two-interface CDC-ACM layout works, although the descriptor builders are generic.
- usb_device.fd:367-369 and 391 one configuration only.
- usb_device.fd:459-484 class requests all go to the CDC handler and only for interface 0; usb_hid.fd does not plug into it (no dispatch hook); vendor requests always stall (358-359), so no Microsoft OS / WinUSB descriptors.
- usb_device.fd:463-471 SET_LINE_CODING accepts any stop-bits (only 0-2 valid) and parity (only 0-4 valid); `usb_line_coding_str` (500-504) then indexes `par.get(d.parity)` / `stops.get(d.stop_bits)` out of range and panics on a host-sent value.
- usb_device.fd:421-423 interface GET_STATUS does not check the interface index.
- usb_device.fd:177-188 string descriptors encode bytes as Latin-1, so UTF-8 names are mangled.
- usb_device.fd:9 header says the CDC configuration is 67 bytes; the test (and the code) give 75.
Tests: test_std_usb_device.fd checks exact bytes and an enumeration sequence. Untested: bad line coding values, multi-interface devices, interface status.

### terrain
Complete means: a heightmap with height/normal queries, generators (flat, smooth noise such as value/Perlin/fBm), editing brushes, mesh export with UVs, and the features its header promises: LOD and loading from a grayscale image.
- terrain.fd:3-5 header promises "simple LOD support" and "loads from a grayscale image": neither exists (no LOD, no image import, though stdlib image.fd exists).
- terrain.fd:177-201 `terrain_generate_noise` is `(gx*7919 + gy*6151) % 1000`: a position hash with visible linear patterns, not continuous noise.
- terrain.fd:64-69, 204-241, 322-364 `set_height`, `raise_circle` and `smooth` do not update `min_height`/`max_height`, which go stale.
- terrain.fd:244-280 mesh has no texture coordinates; no ray/height-field intersection, no flatten/lower brushes, no erosion.
- terrain.fd:99-106 a 1-wide terrain gives ix = -1 (falls back to 0.0 through bounds checks rather than an error).
Tests: only `terrain_create`/`generate_flat`/`get_height_grid` in test_stdlib_repaired.fd:42-44 plus a doc example. Untested: normals, mesh export, raise/smooth, sine/noise generators.

### synchrophasor
Complete means: IEEE C37.118.2-2011 frames (data, header, command, CFG-1/2 and CFG-3), robust decoding of untrusted bytes, and the C37.118.1 estimation helpers.
- synchrophasor.fd:48, 56 CFG-3 is declared as a type and a command, but there is no CFG-3 encoder/decoder (the 2011 standard's variable-length configuration with PMU position, phasor scale/angle adjustments, window lengths).
- synchrophasor.fd:516-567 and 652-723 decoders read NUM_PMU, PHNMR, ANNMR, DGNMR and offsets without bounds checks; a CRC-valid frame with inconsistent counts (or a data frame decoded against the wrong config) panics on Vec index instead of returning an error. `c37_decode_header_frame` (365-373) does not check the frame type.
- No STAT word decoding helpers, no FRACSEC/TIME_BASE -> seconds helper, no TCP/UDP framing helper (reading FRAMESIZE from a stream).
- Phasor estimation is a plain one-cycle DFT (no P/M-class filtering); acceptable but limited.
Tests: test_std_synchrophasor.fd is good (independent-encoder bytes, floats, estimation). Untested: malformed frames, CFG-1, version-1 frames.

### touch
Complete means: multi-touch tracking plus the usual gesture recognizers (tap with time limit, double tap, long press, pan, swipe with velocity, pinch with center, two-finger rotation).
- touch.fd:394-397 `tap_max_duration` is stored but never read; `touch_end` (160-168) takes no timestamp, so a tap cannot be timed and a long hold counts as a tap.
- No double-tap, long-press, rotation, pinch center; swipe has no velocity/duration check.
- touch.fd:218-248 `swipe_direction` keeps the last value after the swipe ends (only `swipe_active` is reset).
- touch.fd:123-127 a begin beyond MAX_TOUCHES is dropped silently; a begin with an id already active is not detected.
Tests: test_lib_touch.fd calls every pub function. Untested: tap timing (cannot exist yet).

### uart
Complete means: a 16550 register model with all register offsets (including DLL/DLM), the bit fields of LSR/IIR/FCR/MCR/MSR, an LCR builder for any data/parity/stop setting, correct divisor math with range checks, and the init sequence.
- uart.fd:72-77, 80-85 `uart_divisor(230400)` = 0 and `uart_divisor_clk` truncates; a divisor of 0 is written into DLL/DLM by `uart_init_seq` (95-113) without a check. No rounding to the nearest divisor.
- uart.fd:87-90 only 8N1; no LCR builder for 7E1 etc.
- uart.fd:48 DLL/DLM offsets only in a comment; LSR has 3 of 8 bits (no OE/PE/FE/BI/RXFE), no IIR/FCR/MCR/MSR bit constants.
- uart.fd:92-94 comment says "divisor 3 (38400)" though the baud is a parameter.
Tests: test_os_stdlib.fd:199-200 (two checks) plus doc examples. Untested: baud above 115200, odd baud rates.

### timeline
COMPLETE: step/linear/cubic float and vec3 tracks, events fired once per crossing in both directions and across loops, seeking; test_std_timeline.fd checks values and event order including multi-lap and backwards wraps. Polish only: no key removal/editing, no rotation (slerp) track.

### tilemap
Complete means: a tile grid with per-tile properties, correct AABB collision and movement resolution (negative coordinates included), auto-tiling and line of sight, with a test.
- tilemap.fd:186-187 and 204-205, wrong snap in move_x/move_y. When `new_x + pw` is a multiple of `tile_size`, `(new_x+pw)/ts` names the tile after the blocking one, and the returned x leaves the box inside the wall. Example: ts=16, wall at tile 2, pw=16, px=10, dx=22 gives 32, inside the wall. The snap should use `(new_x+pw-1)/ts`.
- tilemap.fd:180-195, a large dx tunnels through thin walls: only the destination is tested. With dx == 0 and an existing overlap, the code takes the "moving left" branch and teleports the box.
- tilemap.fd:141-142, 147-150 and 159-162, pixel-to-tile conversion uses `/`, which truncates toward zero. px=-5 maps to tile 0 instead of -1, so boxes partly off-map are misjudged.
- tilemap.fd:103, `tilemap_set` hardcodes which ids are passable (EMPTY, FLOOR, GRASS). tilemap.fd:70, `tilemap_from_grid` makes every non-empty tile solid, FLOOR included. The two disagree, and there is no per-id property table.
- Missing: layers, per-tile properties or metadata, visible-region iteration for rendering, flood fill, and 47-tile blob reduction (the corner bits in `tilemap_auto_tile` are not masked by their edges).
- tilemap.fd:8-11, the header example uses `value` (not a keyword) and passes `tm` by value to `tilemap_set`, which takes `&mut`.
- Tests: none.

### term
Complete means: ANSI styling that composes, cursor and screen control, a capability check (isatty, NO_COLOR, Windows VT mode), terminal size, display width, and raw key input, with a test.
- term.fd:28-141, every style wraps with `ESC[0m`, so nested styles cancel each other. In `term_bold(term_red("a") + "b")`, "b" is not bold. Wrappers should close with the matching SGR off code (22, 23, 24, 39, 49).
- term.fd:5 and 294-306, the header says Windows 10+ consoles work, but nothing calls SetConsoleMode(ENABLE_VIRTUAL_TERMINAL_PROCESSING). `term_supports_ansi` is an environment heuristic only, with no isatty check, so output piped to a file gets escapes whenever TERM is set.
- term.fd:287-289, `term_width` counts bytes, which is wrong for UTF-8 and wide characters.
- term.fd:274-277, `term_strip` handles CSI and two-byte escapes only. OSC sequences (`ESC ] ... BEL/ST`, used for hyperlinks and titles) are left in.
- Missing: terminal size, raw/cbreak mode, key and escape-sequence reading, alternate screen, `bg_black` and bright backgrounds, scroll regions.
- term.fd:203, the doc says the bar is drawn with block characters, but it draws `#` and `-`.
- Tests: no bootstrap test. One `/// Example:` is run by scripts/doctest.py (strip, width, progress_bar).

### test
Complete means: assertions with correct failure messages for ints, floats, strings and collections; a runner that counts; table tests; snapshots stored next to the test with a diff; expected-panic checks.
- test.fd:141-145, `test_assert_approx_eq` prints `int_to_str(b as i64)`, so a float failure reads "expected ~3, got 3".
- test.fd:380-385, snapshots are documented as stored "next to the test that calls it", but the path is `name + ".snap"`, relative to the current directory.
- test.fd:404-406, a snapshot mismatch prints no diff and not even the two values.
- test.fd:211-247, the Vec asserts take `Vec<i64>` only. There is no f64 equality message, no `assert_str_contains`, and nothing generic.
- test.fd:86-247, the `test_assert_*` functions return bool and do not feed `TestRunner`. The caller must call `test_pass`/`test_fail` by hand.
- Missing: assert-panics / expect-error.
- Tests: test_lib_test.fd (73 lines) and test_stdlib_table.fd (49 lines) cover asserts, the runner and tables. `expect_snapshot` and the FIDES_ROW filter are untested in the suite.

### task
Complete means: green-thread spawn, yield, sleep, join and fd waits; structured scopes that join and cancel their own children; cancellation tokens with a lifetime; timeouts with unambiguous results; all of it tested.
- task.fd:166-167, `task_scope_join` calls `task_run()`, which waits for every task in the process, not just the scope's children. A scope cannot be joined while unrelated long-lived tasks run.
- task.fd:219-225 and 110, tokens and scope flags come from `p_shared_alloc` and are never freed. There is no `task_token_free` and no scope drop.
- task.fd:141-161 and 187-189, `task_join_timeout` / `task_timeout` return -1 on timeout, which a task's real result of -1 cannot be told apart from. They should return Option.
- task.fd:152, 386 and 401, joins and channel receives poll with 1 ms sleeps instead of parking on completion.
- task.fd:31, tasks are named by string and take one i64. This is a runtime limitation: no closures, no typed results.
- Tests: the core (go/run/yield/sleep/detach/tokens/sleep_cancellable/recv_cancellable) is well covered by about 15 test_task*/test_timeout*/test_runtime_cancel files. Untested: `task_scope_*`, `task_join_timeout`, `task_timeout`, `task_wait_write`, `task_wait_*_cancellable`, `task_scope_sleep`. `task_pool_call` is checked only in tests/tooling/ca10.

### url
Complete means: RFC 3986 parse and serialize that round-trip, percent-encoding, RFC 5.2 resolution, a query API (get, get-all, build, set), and errors for invalid input (bad port, bad host).
- url.fd:269 and 321, the path is percent-decoded at parse and re-encoded at serialize, so `/a%2Fb` becomes `/a/b`, a different resource. The same lossiness applies to `%3F` and `%23`.
- url.fd:249 and 258, the port is not range-checked (`:99999` gives 99999). Parsing is lenient with no validity flag or error, so a caller cannot reject garbage.
- url.fd:510, `url_normalize_path` drops empty segments (`/a//b` becomes `/a/b`). RFC 3986 §5.2.4 keeps them.
- url.fd:464-468, `url_resolve` drops the base userinfo for relative references.
- Missing: get-all for repeated keys, query set/remove, a pairs list, default ports per scheme, host lowercasing and validation, IDNA/punycode.
- Tests: test_lib_url.fd (169 lines, every function) plus the port-overflow fuzz regression in test_verify_vf15_parsers.fd. Untested: `%2F` path round-trip, empty segments, port range.

### udp
Complete means: dual-stack UDP sockets on every backend with bind, send, recv and timeouts, plus connect, broadcast, multicast and socket options; scheduler-friendly waits; tested sockets.
- No `connect` (connected UDP, ICMP errors), no SO_BROADCAST, no multicast join/leave/TTL/loop, no SO_REUSEADDR/REUSEPORT, no buffer-size options. Only IPV6_V6ONLY is set (udp.fd:860-862).
- udp.fd:360-377 (unix) and 315-326 (Windows), `udp_recv`'s wait is a blocking recvfrom/WSAPoll on the calling OS thread. It does not use `task_wait_read`/`p_co_wait_fd`, so a waiting task stalls its worker. The unix path also leaves SO_RCVTIMEO set on the socket after the call.
- udp.fd:997-1013, a zero-length datagram returns [] without setting `from`, the same as a timeout. A datagram longer than `max` is silently truncated (unix) or dropped as an error (Windows WSAEMSGSIZE).
- Tests: test_stdlib_udp_addr.fd (81 lines) covers address parse/format only. Socket I/O is exercised only in tests/net/*.fd (udp_loopback, quic_udp_*), which are not in the bootstrap suite because the interpreter cannot call sockets.

### vision
Complete means: grayscale and float images with filtering, edges, thresholds, morphology, components, features (FAST/ORB/Harris), matching, camera model, and robust geometry (RANSAC homography, warping), plus resize and pyramids.
- No resize, crop or image pyramid, so the ORB descriptors (vision.fd:769-808) are not scale-invariant, despite "ORB-style".
- vision.fd:1072-1176, the homography is a least-squares DLT only. There is no RANSAC, so matches with outliers (the normal output of `vis_match`) give a wrong H.
- No perspective/affine warp of an image (only `vis_undistort`, 954-968). No Harris/Shi-Tomasi corners, integral image, template matching, color-space conversion beyond gray, or adaptive threshold.
- vision.fd:24-26, the BRIEF pattern is a fixed LCG, not ORB's learned table. This is documented, and descriptors are not interoperable with OpenCV's.
- Tests: test_std_vision.fd (311 lines) exercises almost every public function. `vis_bilinear`, `vis_convolve_sep`, `vis_close` and `vis_hamming` are covered only indirectly.

### usb_hid
Complete means: a full HID 1.11 report-descriptor parser (all global and local items), field decode and encode for input, output and feature reports (array and variable), boot keyboard and mouse, class requests.
- usb_hid.fd:266-317, there is no decoding of array fields into usages. In report protocol, a keyboard's key array holds usage indexes (offset by usage_min). `hid_usage_value` treats every field as variable.
- No generic report encoder for output and feature reports. Only `hid_led_report` (432-436) builds one.
- usb_hid.fd:164, 16-bit usages take the usage page in force when the Usage item is read. HID 1.11 §6.2.2.8 resolves them at the main item, so a Usage Page after its Usages misattributes them.
- usb_hid.fd:131-160, global items Physical Min/Max, Unit Exponent and Unit (tags 3-6) are dropped, and Push/Pop saves only the six tracked values. The Delimiter local item is ignored.
- usb_hid.fd:344-354, `hid_key_ascii` covers 0x04-0x38 only. There is no keypad (0x54-0x63), no F-keys, no arrows (no key-name table either), and only the US layout.
- Tests: test_std_usb_hid.fd (116 lines) covers parse, find, usage_value, kbd update, mouse, requests and config. Untested: Push/Pop, long items, signed fields with report IDs, `hid_report_bits` across feature reports.

### syscall_abi
Complete means: the registers, numbers and trap bytes of the three ABIs, a call table covering what a small libc or musl static binary uses, full user-pointer validation (including sockaddr/len pairs and iovec arrays), and the full errno table.
- syscall_abi.fd:360-410, the table has about 50 calls. Missing ones static binaries hit at startup and in normal I/O: readv/writev, poll/ppoll, epoll_*, select/pselect6, rt_sigaction, rt_sigprocmask, rt_sigreturn, set_tid_address, arch_prctl (x86), prlimit64, newfstatat/fstatat, faccessat, readlinkat, dup2/dup3, renameat, madvise, sigaltstack, exit paths beyond exit/exit_group.
- syscall_abi.fd:381, 383 and 390, the addr/addrlen pairs of accept and recvfrom and the rusage of wait4 are SARG_ADDR (not validated). execve's argv/envp arrays (388) are not walked. There is no argument kind for an in/out length pointer or an array of user pointers (iovec).
- syscall_abi.fd:87-132, the errno table has about 45 entries. Missing include EWOULDBLOCK (alias), ENOTCONN 107, EINPROGRESS 115, EALREADY 114, EISCONN 106, EMSGSIZE 90, EAFNOSUPPORT 97, ENETUNREACH 101, EHOSTUNREACH 113, ECONNABORTED 103, ENOBUFS 105, EOVERFLOW 75. `errno_name` returns "E?" for these.
- syscall_abi.fd:35-45, the comments of the argument-kind constants are shifted one line (the SARG_ADDR text sits above SARG_IN, and so on). Cosmetic.
- Tests: test_std_syscall_abi.fd (166 lines) covers names, regs, trap, table, trap dispatch and uaccess. `syscall_validate` is exercised through `syscall_trap`. `uaccess_strlen` returning ENAMETOOLONG and nullable outptr are untested.

### volatile
Complete means: volatile loads and stores of 8, 16, 32 and 64 bits through a wrapper and raw pointers, memory fences, and bit set/clear helpers for MMIO registers.
- volatile.fd:139-165, there is no `ptr_read_volatile16` / `ptr_write_volatile16`, although `Volatile.load16/store16` (71-79) exist.
- No fence or barrier functions (ordering between MMIO writes and DMA). No read-modify-write helpers (set_bits, clear_bits, modify).
- volatile.fd:1 and 25, "Volatile<T>" is an untyped i64 address, not a generic.
- Tests: indirect, via test_os_features, test_tier1_kernel and test_unsafe_raw_apis (new, load, store, load8/32, ptr_*). load16/store16 are untested.

### string
Complete means: the string helpers beyond core (join, pad, case, count, find, predicates, hex/bin, views) correct for every input, a real rope (or none), and its own test.
- string.fd:595-601 `rope_concat` drops both children: `left: 0, right: 0` with the comments "would store pointer to left/right". Only the length is kept.
- string.fd:609-618 `rope_to_str` returns "" for any rope made by concat ("A full implementation would use a Vec-based stack"). So `rope_to_str(rope_concat(rope_new("a"), rope_new("b")))` is "" and data is lost without an error. string.fd:622-626 `rope_substring` goes through it, so it is wrong for every concatenated rope too.
- string.fd:494-540 SmallStr is not a small-string optimisation. Every value is a heap copy (`p_string_from`), `smallstr_is_inline` always returns false (:538), and the "inline if <= 14 bytes" usage text (:480-492) is false. The doc example (:522) checks `!smallstr_is_inline`, which locks the fake behaviour in.
- string.fd:428-447 `int_to_hex` and :454-478 `int_to_bin` return "" for any negative number. The loop runs `while value > 0`, so there is no two's-complement output and no "-" sign.
- string.fd:25-38 `str_join` grows the result by `result = result + ...`, which is O(n^2) bytes copied. `str_join_fast` (:48-69) only splits the work into batches of 1000 and is still quadratic inside each batch. core has a `StringBuilder` (core.fd:6588) that would make join linear.
- string.fd:118-166 `str_capitalize` and `str_title` are ASCII only (no UTF-8 awareness, and nothing points to unicode.fd). string.fd:225 `str_count` counts non-overlapping matches only, which is fine but not documented.
- Tests: none of its own. The only coverage is doc examples run by scripts/doctest.py and indirect use (str_join and int_to_hex are used by 40+ tests). str_title, str_center, str_find_nth, str_strip_char, rope_* and smallstr_* are untested except by doc examples. Nothing tests negative hex or rope concat, and either would fail.

### sys
Complete means: real host facts (architecture, OS, endianness, CPU count, page size, memory, pid/ppid/uid/gid, plus hostname, OS version and uptime) on every supported OS.
- sys.fd:247-254 `sys_arch` returns "x86_64" unless the environment sets `PLEDGE_ARCH`. So it is wrong on aarch64 and riscv64, which the compiler targets (main.fd sets `g_cfg_arch`, and `@cfg(<arch>)` exists, compiler.fd:8899-8908). `sys_is_64bit` and `sys_is_32bit` (:379-386) are derived from it.
- sys.fd:391-405 `sys_is_little_endian` returns true on every path, and `sys_is_big_endian` is always false. It is a fake check.
- sys.fd:362-376 platform detection is broken through `os_name` (os.fd:121-128), which returns the `OS` environment variable or else "linux":
  - On Windows `OS` is "Windows_NT", so `sys_is_windows` (which compares with "windows"/"win32") is false on Windows.
  - On macOS `OS` is unset, so `os_name` is "linux": `sys_is_linux` is true on macOS and `sys_is_macos` is never true.
  - The module already selects per-OS code with `@cfg(windows|linux|macos)` (:87-243), so the right answer is known at compile time.
- sys.fd:155-166 Windows `sys_ppid`, `sys_uid` and `sys_gid` are 0. This is documented and acceptable. There is no username or SID either.
- Missing: hostname, OS version or kernel release, uptime, load average, current user name, and process memory use (RSS).
- sys.fd:302-309 macOS has no `_SC_AVPHYS_PAGES`, so `sys_available_memory` is 0 there. That is documented only inside sx_sysconf.
- Tests: test_lib_sys.fd checks invariants only: opposite answers disagree, at most one platform matches, the memory conversions agree. It cannot see any of the bugs above. It accepts zero platforms matching, which is what happens on Windows and macOS today.

### storage
Complete means: a Merkle tree with a cryptographic hash, domain separation, inclusion proofs and their verification, plus a key-value store that is at least hashed (and, given the name, persistable).
- storage.fd:18-53 every hash is a pair of FNV-1a 64-bit hashes ("For production use, replace with crypto_sha256"). That is not collision resistant, so the "tamper detection, data integrity" the header promises (:4-5) is not provided. The stated reason, that transitive imports are not resolved (:20-21), is stale: other stdlib modules import crypto modules.
- storage.fd:153 the comment says "SHA-256(left || right)" but the code is FNV.
- No inclusion proofs (no proof path, no `merkle_verify_proof`), which is the main use of a Merkle tree. `merkle_tree_verify_data` (:232-248) only scans the leaf hashes.
- storage.fd:154-163 internal nodes hash `left || right` with no leaf/node prefix, which allows second-preimage attacks. An odd last node is paired with itself (:182-185), which allows Bitcoin-style CVE-2012-2459 duplicate-leaf ambiguity.
- storage.fd:167 calling `merkle_tree_build` twice appends a second set of internal nodes; nothing resets them.
- storage.fd:250-331 the KV store does a linear scan per operation (its own comment says a HashMap would be better). Values are i64 only, `kv_store_get` returns -1 for a missing key (which cannot be told apart from a stored -1), and there is no persistence or iteration.
- Tests: test_lib_storage.fd (155 lines) is good for what exists: the known FNV values, node numbering, order sensitivity, the KV operations.

### swift
Complete means: valid BIC and IBAN checks (including mod-97), building and parsing MT messages (blocks 1-5, fields, multi-line values), at least MT103 both ways and MT940 parsing, with correct amount and charset rules.
- swift.fd:138-199 IBAN validation has no mod-97 check, although the comment at :142 says "Validation: mod-97 check". `swift_iban_validate_length` knows 15 countries and accepts any length from 15 to 34 for the rest, so a mistyped IBAN passes.
- swift.fd:99-112 `swift_bic_validate` accepts lowercase letters and does not check that characters 7-11 (location and branch) are alphanumeric.
- swift.fd:281 field `:32A:` is written as `int_to_str(cents)`. SWIFT wants the amount in major units with a decimal comma ("1000,00"), so the serialized MT103 is wrong for every amount.
- swift.fd:268-306 the serializer:
  - writes LF, not CRLF;
  - fixes block 1's session and sequence to zeros;
  - writes no block 3 (UETR, which is mandatory for MT103 under SWIFT gpi since 2018) and no block 5;
  - always writes `:71A:OUR`;
  - sets no `:33B:`;
  - writes multi-line party fields (`:50K:`, `:59:`) as one line.
- swift.fd:25-36 twelve message types are named, but only MT103 can be built, and nothing parses a message. `swift_extract_field` (:366-383) takes the first ":TAG:" anywhere in the text and cuts the value at the first newline, so multi-line fields lose their tail.
- swift.fd:309-311 `swift_mt103_amount_major` divides i64 cents by 100.0.
- Tests: test_lib_swift.fd (106 lines) covers BIC, IBAN length, field parsing, extraction and MT names. Nothing compares the serializer output with a known MT103 text, and nothing checks the mod-97 path, which does not exist.

### tls_windows
Complete means: Schannel client and server with certificate checking, TLS 1.2/1.3, ALPN, a clean close_notify shutdown, error reporting, and common options (client certificates, a custom trust store).
- tls_windows.fd:1117-1124 `tls_session_close` deletes the context without sending close_notify (no ApplyControlToken with SCHANNEL_SHUTDOWN followed by a final InitializeSecurityContext/AcceptSecurityContext). The peer sees a truncated stream. tls_unix sends it through SSL_shutdown.
- tls_windows.fd:521-532 says "The session owns the socket", but tls_session_close (:1115-1116) leaves the socket open. tls_unix (:324) does not say this. The two docs disagree on who closes the socket after tls_connect.
- No error reason anywhere: every failure is 0 or -1, and the SECURITY_STATUS is dropped (for example :349-376 and :921). A user cannot tell a certificate error from a network error.
- Missing: client certificates (mutual TLS), access to the peer certificate, session resumption control, a custom CA or pinning, and a configurable handshake timeout (TS_TIMEOUT is fixed at 30 s, :89, :317).
- tls_windows.fd:809-821 calling `tls_server_alpn` a second time overwrites the block without freeing the old one.
- tls_windows.fd:630-633 a .pfx file larger than 1 MiB is silently truncated (fread caps at `cap`).
- Tests: tests/bootstrap/test_tls13_local.fd (C and native builds on Windows 11, loopback, self-signed certificate) covers tls_server13, tls_connect13_unchecked, ALPN and version. tests/tls/*.fd are live tests outside the harness. The checked client (`tls_connect` with certificate checking), TLS 1.2, post-handshake messages (KeyUpdate) and close are not covered by the harness.

### tls_unix
Complete means: the same API as tls_windows over OpenSSL: certificate and host checks, TLS 1.3 pinning, ALPN, clean shutdown, error reporting.
- tls_unix.fd:339-349 `tls_connect_unchecked` creates a new SSL_CTX on every call and never frees it (a leak per connection). The 1.3 and ALPN variants were fixed to cache theirs (:379, :433-451); this one was not.
- No error reason: failures return 0 or -1, and the `SSL_get_error` and `ERR_get_error` text is never exposed.
- Missing: mutual TLS (client certificate), a custom CA file or directory (only `SSL_CTX_set_default_verify_paths`), peer certificate access, session resumption and a configurable handshake timeout (30 s fixed).
- `tls_session_protocol` exists only on Windows (documented at :9-11). Code using it is not portable.
- tls_unix.fd:549-554 `SSL_shutdown` is called once, without waiting for the peer's close_notify. This is acceptable for most uses.
- Tests: the same test_tls13_local.fd on Linux/macOS with OpenSSL. The checked `tls_connect`/`tls_client` path and TLS 1.2 are not tested in the harness.

### units
Complete means: SI base and common derived units (mechanical, electrical, thermal, angle) as distinct types, with the same set of operations on every unit (add, sub, scale, compare, ratio), and the common non-SI conversions.
- Derived units stop at mechanics: no Volts, Ohms, Coulombs, Farads, Henries, Teslas or Webers, no Radians or Degrees, no Lux, Becquerels or Grays, nor density, torque, momentum, specific energy or flow rate. Amperes exist, but nothing combines them (Watts = V*A, Coulombs = A*s).
- units.fd:581-732 operator impls (`+ - * /`) exist only for Meters, Seconds, Kilograms, Newtons, Joules and Watts (all four ops), and Kelvin, Amperes, Moles, Candelas, MetersPerSecond, Pascals and Hertz (+ and - only). MetersPerSecond2, SquareMeters and CubicMeters have no impl at all.
- units.fd:482-535 `*_scale`, `*_lt` and `*_ratio` exist for those same 6 types only. There is no equality or `le/gt`, and no `to_str` with a unit suffix.
- Missing conversions: km/h, mph and knots, bar, psi and atm, kWh and calories, eV, horsepower, degrees and radians, liters and gallons, nautical miles.
- `kelvin_add` adds two absolute temperatures. The module has no temperature-difference type, which a units library is expected to separate.
- Tests: test_stdlib_units.fd (42 lines) covers velocity, travel, force, mass, kelvin_add, scale, lt and ratio. The rejection test and the frontend dimension tests check the type-level guarantee. Most of the conversion functions and all the constants are untested.

### tiled
Complete means: load any TMJ map Tiled writes: CSV and base64 (raw, zlib, gzip) tile data, infinite (chunked) maps, group and image layers, all object shapes, tileset references and firstgid resolution, layer properties.
- tiled.fd:6-7 base64 layers "return empty tiles (no zlib in stdlib)". That reason is stale: stdlib has `zlib_decompress`/`inflate` (compress.fd:731-738) and base64 decoding (encoding.fd). Even uncompressed base64, which needs no zlib, gives no tiles. Tiled offers base64 (plain, zlib, gzip or zstd) as a layer format, and a map saved that way loads empty with no error.
- tiled.fd:5 the header promises "tileset references and firstgid arithmetic", but no function reads `tilesets`, finds the tileset of a gid, or gives a local tile id. A user must do it from the raw JSON.
- tiled.fd:100 the doc says "name/type/visibility/opacity", but there is no opacity function, and no offsetx/offsety, tint or parallax either.
- Infinite maps (`chunks` instead of `data`) are not handled: `tiled_tile_raw` (:115-127) reads `data` only. Group layers are not walked, so layer indexes only count the top level. Image layers have no accessors.
- tiled.fd:187-208 objects: polygon, polyline, ellipse and text shapes are not represented (only point/rect/tile), and neither are rotation or visible.
- tiled.fd:212-227 property values are returned as text only, and the property `type` is ignored. Tileset and layer properties cannot be read.
- Tests: test_gaming_stdlib.fd and test_stdlib_jsondoc_loaders.fd cover the open/tile/flip/object/property functions well for CSV/array data. Base64 maps and infinite maps are not covered.

### unix_socket
Complete means: Unix stream sockets (bind/accept/connect/close) with byte-level and text I/O, timeouts, task-friendly waiting, and safe handling of a stale socket path; datagram sockets if the name is to stand for the family.
- unix_socket.fd:52-54 `UnixListener::bind` deletes any file at `path` before binding, not only a stale socket. A mistaken path deletes the user's regular file. It should check the file is a socket, or try to connect first.
- unix_socket.fd:98-121 the only I/O is `write_str` and `read_str`. There is no byte (Vec or buffer) read/write, so binary protocols cannot be spoken unless `p_socket_recv_str` keeps NUL bytes. There is also no read_exact, read with a timeout, or non-blocking/task-aware read.
- Missing: datagram sockets (SOCK_DGRAM), peer credentials (SO_PEERCRED / getpeereid), the Linux abstract namespace, and shutdown of one direction.
- unix_socket.fd:31 UNIX_PATH_MAX is 103 for all OSes; Linux and Windows allow 107 (documented as the lowest common limit, fine).
- Tests: test_unix_socket.fd covers bind, refused double bind, connect, accept, an echo, a second client and close-removes-path. It runs in one process (`fid run` simulates it). Binary data and error paths beyond "no listener" are not tested.

### vfs
Complete means: a POSIX-like VFS: path walking with mounts and symlinks, the descriptor table, the full set of path operations including links, metadata and permissions, and a block-device seam.
- No hard links (`link`), no `chmod`/`chown`/`utimes`, and no `statfs`/`fsync`/`sync`. `FsOps` (:101-117) has no slot for them, so no file system can provide them.
- vfs.fd:40 permission checks: `VFS_EACCES` is defined but never returned. The mode bits are stored but access modes are checked only against the open flags (:625, :664), not the inode's mode or a user.
- vfs.fd:902-933 `vfs_rename` does not refuse moving a directory into its own subtree (EINVAL in POSIX). It relies on the file system to catch that.
- vfs.fd:600-614 `dup` copies the offset rather than sharing it. This is documented as a limit of the table.
- vfs.fd:1210-1222 memdisk_zero empties a slot but never reuses it (`md_store` only grows). This is minor.
- Tests: test_std_vfs.fd (186 lines) is good: mounts, `..` across mounts, symlinks and ELOOP, EXCL, APPEND, EXDEV, EBUSY, read-only, readdir and chdir/getcwd. memdisk and blk_* are tested through the fat32/ext4/ramfs/nvme/ahci tests. `vfs_unlink`, `vfs_dup`, `vfs_mkdir_p` and `vfs_pwrite` are never called by test_std_vfs.

### tlsf
Complete means: the TLSF API (malloc/free/realloc/memalign, add_pool, stats/check) with bounded time, and the memory seam documented.
- Missing `tlsf_memalign` (aligned allocation, part of the reference TLSF API and needed by drivers and DMA) and `tlsf_add_pool` (a second arena).
- tlsf.fd:256-261 `tlsf_realloc` never shrinks. When the new size fits it returns `p` without splitting off the tail, so the space is lost until free. `realloc(p, 0)` does not free (the reference implementation frees).
- tlsf.fd:221-223 `tlsf_free` checks only the FREE flag and range. A pointer into the middle of a block is accepted and corrupts the arena. The reference implementation asserts the same way, so this is polish.
- Tests: test_std_tlsf.fd is good: the mapping, a 2000-operation random workload with integrity checks every 250 operations, coalescing, and realloc in place and moved.

### thermal (COMPLETE)
Everything in the header is implemented and checked against ASHRAE/ISO figures in test_std_thermal.fd. Polish: `thermal_w_from_wet_bulb` (:293-296) uses the over-water equation below 0 C (ASHRAE eq. 35 for ice is missing).

### world
**Complete means:** a test can fake files, clock, HTTP and raw sockets, and every effectful stdlib function honours the fakes.

Gaps:
- **The socket fake is not connected.** world.fd:584-601 says stdlib/net's socket functions "ask this module first". They do not:
  - `stdlib/net.fd` never mentions world, in the working copy or at HEAD;
  - `git log -S world_sock_open -- stdlib/net.fd` finds nothing;
  - no file outside world.fd calls `world_sock_open/bind/connect/send/send_to/recv/close`.

  So world.fd:602-943 is dead code. tests/bootstrap/test_world_sockets.fd says it "Needs the stdlib/world hooks in stdlib/net.fd" (line 9), so as written it opens real sockets instead of faking them, and its expected output cannot hold.
- The fake file system is text only: a NUL byte ends a file written through fs_write_bytes (world.fd:34-35). Binary formats (wav, zstd, images) cannot be tested hermetically.
- The socket fake handles IPv4 dotted quads only (world_ip_value 684-704), and has no listen/accept, so TCP servers cannot be faked.
- No fakes for environment variables, randomness, stdin/stdout or process spawning. A test marked @hermetic still reaches them.
- Every lookup is a linear scan over all paths (100-108, 222-229). Fine for tests, slow for large seeded trees.

Test gaps: the files, dirs, clock, HTTP and hermetic tests are good. test_world_sockets is effectively broken until net.fd is wired.

### ws
**Complete means:** an RFC 6455 client and server that enforces the protocol's MUST rules, bounds resources, and negotiates subprotocols and close codes.

Gaps:
- **Remote DoS.** ws_recv reads a 64-bit length from the peer (ws.fd:466-477), and ws_read_n then calls `p_malloc(n + 4)` with it (413). There is no maximum message or frame size.
- Protocol MUST checks are missing:
  - a server does not reject unmasked client frames (and a client does not reject masked server frames) (486-492);
  - RSV1-3 bits are never checked;
  - reserved opcodes 3-7 and 11-15 are accepted;
  - control frames longer than 125 bytes, or fragmented, are accepted;
  - text frames are not checked as valid UTF-8 (close code 1007);
  - the close payload's status code is neither parsed nor sent: ws_close sends an empty close (568) and does not wait for the peer's close.
- Mask keys and Sec-WebSocket-Key come from `random_int_range` (186, 377). RFC 6455 §10.3 requires a strong entropy source; crypto_random_bytes should be used.
- The client accepts any response containing "101" anywhere (283).
- The server does not check the GET method, `Connection: Upgrade` or `Sec-WebSocket-Version: 13` (315-330).
- No Sec-WebSocket-Protocol (subprotocol) negotiation, no Origin check, and no way to add headers (auth, cookies) to the client request.
- No read timeouts. WsConn is passed by value, so `open` never changes after close.
- The doc comment at 560 refers to a nonexistent `ws_recv_loop`.
- SHA-1 is reimplemented (26-132) instead of using the crypto module.
- No permessage-deflate (documented, 10-11).

Test gaps: none of the protocol-violation paths are tested (unmasked frames, oversized lengths, bad opcodes, invalid UTF-8), and neither are close codes.

### wss
**Complete means:** wss:// client and server over tls, with URL handling, tested.

Gaps:
- No test anywhere: nothing in tests/ uses `wss_`, `ws_connect_url` or `ws_url_part`.
- The handshake's Host header has no port (ws.fd:280), so non-default ports break virtual hosting (RFC 6455 4.1 requires host:port when the port is not the default).
- ws_url_part (wss.fd:82-128):
  - `wss://host?x=1`, a query with no path, leaves the query in the host;
  - userinfo (`user@host`) is kept in the host;
  - an empty port gives `str_parse_int("")`.
- Every TLS limit of module tls applies; ALPN is not checked.

Test gaps: needs at least a URL-parsing test, plus a loopback wss test like test_ws_local, using a test certificate.

### yaml
**Complete means:** parse and emit YAML 1.2 (core schema) config documents, with a navigable model.

Gaps:
- **No serializer.** No `yaml_dump`, `yaml_to_str` or emitter exists (the pub API is load, parse and getters only: yaml.fd:1947-2091). Programs cannot write YAML.
- The flat dotted-path model joins segments with "." without escaping (yaml.fd:521). A key like `example.com:` collides with nested `example: com:`.
- No way to list the keys of a mapping; callers must scan entries.
- yaml_get_float returns the default for `.inf`, `-.inf` and `.nan` (2076), which the core schema reads as floats.
- Unsupported (yaml.fd:51-54): a mapping inside a flow sequence (`[a: b]`, common in Kubernetes and GitHub Actions files), `?` keys in flow collections, and tab indentation.
- yaml_get_int's hex/octal path has no overflow or digit-validity check (2052-2063).
- Each lookup is a reverse linear scan (1999-2008).

Test gaps: parsing is well covered (test_lib_yaml, test_yaml_spec). Emitting would need round-trip tests.

### xml
**Complete means:** a well-formed and validating XML 1.0 parser (it is) plus a faithful tree and serializer.

Gaps:
- **Mixed content order is lost.** XmlNode.text concatenates every text run of an element (xml.fd:64-78). xml_to_str writes all text before the children (2810-2815), so `<p>a<b>x</b>c</p>` serializes as `<p>ac<b>x</b></p>`. XHTML, DocBook and SVG text cannot be read faithfully or round-tripped.
- Comments, processing instructions and CDATA are dropped from the tree, so they cannot be preserved or generated.
- The serializer has no XML declaration, no indentation or pretty printing, and no namespace-aware output.
- Attribute values are escaped without `&#9;`, `&#10;` or `&#13;` (161-180), so tabs and newlines in values turn into spaces on reparse.
- xml_escape and xml_to_str build strings one character at a time: quadratic on large documents.
- xml_text has dead code: `sep` is always "" (2786-2789).
- Not supported (44-46): parameter-entity references inside declarations of an external subset, and encodings other than UTF-8 (no UTF-16 or Latin-1 input).

Test gaps: the parser is thoroughly tested. Mixed-content round trip and serialization of escapes are untested.

### x509
**Complete means:** parse certificates and CRLs and validate a chain for TLS use: names, EKU, path building, revocation.

Gaps:
- extendedKeyUsage (2.5.29.37) is not parsed. x509_extension (202-290) has no branch for it: a critical EKU fails as an unknown critical extension (286-287), and a non-critical one is ignored, so a TLS client cannot require serverAuth.
- No path building. The chain must come in order. When the last certificate is checked, only the first anchor whose name matches is tried; a bad signature there fails without trying the next anchor with the same name, as happens during cross-signing and key rollover (664-669).
- IPv6 SANs are rendered uncompressed (x509_ip_text 182-200) and compared as strings with `host` (1097-1100), so `2001:db8::1` never matches.
- Names are compared byte for byte (424-426) instead of with RFC 5280's comparison.
- When there is no SAN, the CN fallback still applies (1106-1108); modern TLS clients refuse it.
- Not done (documented 24-30): OCSP, delta/indirect CRLs, policy processing, name constraints on URIs; AIA and CRL distribution points are not parsed either.

Test gaps: good coverage. Untested: EKU, multiple anchors with the same name, IPv6 hosts.

### zstd
**Complete means:** an RFC 8878 decoder and encoder with dictionary support and a streaming interface.

Gaps:
- No dictionaries for decoding (zstd.fd:1198) or encoding.
- No streaming or incremental API: the whole input and output stay in memory as Vec<i64>, 8 bytes per byte (header 25-27).
- The compressor sends only the predefined FSE tables, never treeless literals or custom tables (1487-1503), so ratios fall below the zstd tool's at the same level. The window is capped at 8 MB.

Test gaps: decode and compress are well tested. Large inputs (above 8 MB) are untested.

### webrtc
**Complete means:** a peer connection with ICE, DTLS-SRTP and data channels, able to talk to a browser.

Gaps (most are documented as out of scope, webrtc.fd:51-56):
- No SCTP, so no RTCDataChannel: the most common non-media use of WebRTC. SDP data-channel sections are carried but cannot be used.
- No TURN (relay candidates, long-term credentials), and no candidate gathering.
- No peer-reflexive candidates, ICE restart or trickle ICE.
- No consent freshness (RFC 7675). Media keeps flowing to a peer that stopped consenting, which is a security requirement in WebRTC.
- SHA-1, HMAC-SHA1 and CRC-32 are reimplemented here (668-832) rather than shared.

Test gaps: good, including failure paths. Data-channel SDP has no interop test.

### wireguard
**Complete means:** WireGuard protocol state plus the device layer: peers, AllowedIPs routing and configuration.

Gaps:
- No cryptokey routing: nothing maps AllowedIPs to peers for outgoing packets or checks the source address of decrypted packets. That check is a core WireGuard security property (paper §3).
- No multi-peer device structure, no wg/wg-quick config parsing, no base64 key helpers.
- wg_timers creates initiations with `nsec = 0` (679, 688, 693), so the TAI64N timestamp only has one-second resolution, and two initiations in the same second are rejected by the responder as replays (wg_bytes_greater, 251-259).
- No rekey jitter (REKEY_TIMEOUT plus up to 333 ms).
- The responder's unconfirmed `next` keypair (474) never expires.
- Handshake rate limiting and endpoint roaming are left to the device (documented 28-30), but no device exists.

Test gaps: good. Untested: multi-peer use, and timestamps within the same second.

### vulkan
**Complete means:** loader bindings plus struct builders enough to create a device, buffers, images, pipelines, and record, submit and present work.

Gaps:
- vk_layout_spec (488-532) knows 14 structs. There is nothing for VkGraphicsPipelineCreateInfo, VkComputePipelineCreateInfo, VkPipelineShaderStageCreateInfo, VkPipelineLayoutCreateInfo, VkCommandPoolCreateInfo, VkCommandBufferAllocateInfo, VkDescriptorPoolCreateInfo, VkWriteDescriptorSet, VkImageViewCreateInfo, VkFenceCreateInfo, VkRenderingInfo or VkSubmitInfo2. Their sType constants exist (170-201), but the user must lay them out by hand.
- No builder for a shader module (SPIR-V code), command pool/buffer, pipeline, descriptor pool or writes, or sync objects.
- The extern block (49-114) lacks:
  - VK_KHR_surface and VK_KHR_swapchain: vkCreateSwapchainKHR, vkAcquireNextImageKHR, vkQueuePresentKHR;
  - vkCmdBindIndexBuffer, vkCmdDrawIndexed, vkCmdCopyBufferToImage, vkCmdPushConstants, vkCmdSetViewport/Scissor;
  - vkCreateSampler, vkFlushMappedMemoryRanges, vkEnumerateDeviceExtensionProperties, vkFreeCommandBuffers, vkResetCommandBuffer.
- No helper that picks the memory type from requirements and properties.
- vk_device_create_info supports one queue family only (824-845).
- `link "vulkan"` needs a renamed vulkan-1.lib on Windows (documented at line 5, but fragile).

Test gaps: the pure parts are well tested. Nothing exercises the loader, even optionally.

### webgpu
**Complete means:** webgpu.h bindings usable end to end (adapter, device, buffers, pipelines, passes, readback, present) plus the WGSL helpers.

Gaps:
- The extern block (52-92) cannot produce a device: wgpuInstanceRequestAdapter and wgpuAdapterRequestDevice (with their callbacks) are missing, so none of the declared functions can be reached in practice.
- Also missing:
  - buffer readback: wgpuBufferMapAsync, wgpuBufferGetConstMappedRange;
  - surfaces: wgpuInstanceCreateSurface, wgpuSurfaceConfigure, GetCurrentTexture, Present;
  - render pass: SetBindGroup, SetIndexBuffer, DrawIndexed, SetViewport/Scissor;
  - textures: wgpuQueueWriteTexture, CopyBufferToTexture, CopyTextureToBuffer.
- No descriptor builders: the user must lay out every descriptor's bytes ("build the descriptor bytes with your own layout code", 8-9).
- wgsl_layout has no shorthand aliases (`vec3f`, `mat4x4f`), and f16 needs the `enable f16` path.

Test gaps: the pure parts are good. No optional GPU smoke test.

### zigbee
**Complete means:** ZCL with the mandatory clusters, a reporting engine, ZDO basics, and an addressable send seam.

Gaps:
- **The send seam cannot address a device:** `ZigbeeRadio.tx(handle, aps_frame)` takes no destination short address or group (900-917), although the comment says the seam carries "the network addressing a coordinator API needs".
- No Basic (0x0000) attribute set and no Identify (0x0003) cluster; every Zigbee device must implement both (only the constant exists, 85).
- The server endpoint handles Read and Write only. Configure Reporting (0x06), Write Undivided (0x03) and Discover Attributes fall into UNSUP_GENERAL_COMMAND (784).
- No reporting engine (min/max interval, reportable change).
- Level Control has Move to Level only (683-703): no Move, Step or Stop. On/Off has no Off-with-effect or On-with-timed-off.
- APS: no ack frames, no fragmentation (875-878 skip the extended header), no APS commands.
- No ZDO (device announce, discovery, bind, Match_Desc).

Test gaps: the existing parts are good. Manufacturer-specific reads and malformed APS frames are lightly tested.

### wav
**Complete means:** read and write every common WAV variant (PCM 8-32, float 32/64, A-law/µ-law, extensible, metadata), with validation.

Gaps:
- wav_encode (352-418) does not validate `format`/`bits`:
  - float writes 4-byte words (407-408) whatever `bits` is, while the header says `bits`, so WAV_FLOAT with 64 gives a corrupt file;
  - PCM with bits 12 or 20 gives a bps truncated by `bits / 8` (354).
- No 64-bit float, and no A-law (6) or µ-law (7): the decoder rejects them (251-263).
- No metadata (LIST/INFO, cue, smpl): skipped when reading, impossible to write.
- No RF64/BW64 for files above 4 GB.

Test gaps: good (real ffmpeg files). Invalid encode parameters are untested.

### voxel_mesh
**Complete means:** chunk meshing usable in a voxel engine, which takes neighbouring chunks into account, plus marching cubes.

Gaps:
- vox_get returns 0 (empty) outside the chunk (194-199). Faces on chunk borders are always emitted (429, 460) and AO treats the outside as open (334-336). No neighbour-chunk callback or padded chunk is supported, so chunked worlds get seams and hidden faces.
- No transparent or non-culling materials (glass, water): every non-zero voxel hides its neighbour's faces.
- Marching cubes carries no material; there is no MC33 or ambiguity handling (acceptable).

Test gaps: good. Chunk borders are untested.

### wfc
**Complete means:** WFC simple-tiled and overlapping models that are efficient enough for real grids.

Gaps:
- wfc_propagate rescans all T×T pattern pairs for each neighbour (366-404). Gumin's implementation keeps compatible-support counts (AC-4); this version is O(T²) per visit and slow for overlapping models with hundreds of patterns.
- Each observation pushes full copies of wave, count, sum_w and sum_wlogw (516-521), so memory is O(decisions × W × H × T).
- The tiled model has no tile symmetry classes (X/L/T/I/\ with automatic rotations, as Gumin's SimpleTiled XML has). Every rotation is added by hand (92-113).
- 2D only. Overlapping output is non-periodic only (587-617).

Test gaps: good. No performance test.

### webview
**Complete means:** a JS-to-native bridge for the platforms that have web views, including desktop.

Gaps:
- Android and iOS only (333-391). Nothing for WebView2 (Windows), WebKitGTK (Linux) or WKWebView on macOS.
- The platform calls are data only (fine as a seam), but no glue in jni.fd/objc.fd performs them.
- Handlers are synchronous `fn(str) -> str` (131-133) and never see the calling origin, so per-origin authorization and async results are impossible.
- A wildcard origin (`https://*.example.com`) never matches a host with an explicit port (76-95).

Test gaps: good for the protocol. Nothing exercises a platform.

### x86_cpu
**Complete means:** CPUID, MSR, descriptor and legacy-device helpers, enough for a hobby kernel's bring-up.

Gaps:
- CPUID decoding covers leaves 0, 1, 7.0 and 0x80000001-4 only (54-157). Missing: cache and topology (4, 0xB, 0x1F), XSAVE sizes (0xD), physical/linear address width (0x80000008), frequency (0x16).
- No decoders for CR0, CR3, CR4, RFLAGS or XCR0.
- CMOS has raw access only (578-586): no RTC date/time decode (BCD, 12/24 h, status register B).
- No Local APIC or IOAPIC register layouts; x2APIC MSRs are named only (198).

Test gaps: good for what exists.

### xhci
**Complete means:** everything needed to bring up an xHCI controller and enumerate and use devices.

Gaps:
- HCSPARAMS2's scratchpad count is decoded (594), but nothing builds the scratchpad buffer array or installs it in DCBAA[0]; many controllers require it.
- The PAGESIZE register is never read.
- No Isoch or Event Data TRB builders: only the TRB_ISOCH constant (93) exists.
- xhci_ep_ctx (491-503) omits Max ESIT Payload, Mult, MaxPStreams and LSA, which interrupt and isochronous endpoints need.
- xhci_slot_ctx (479-486) omits the TT hub slot/port, MTT and interrupter target, needed for LS/FS devices behind HS hubs.
- Rings have one segment only.
- No USB enumeration (GET_DESCRIPTOR, SET_CONFIGURATION, descriptor parsing).
- The legacy handoff does not clear the SMI enables in USBLEGCTLSTS (730-741).

Test gaps: good against the modelled controller.

### Complete modules (one line each)
- **x25519**: RFC 7748 vectors, the 1000-iteration vector and small-order rejection all pass; its constant-time limits are documented.
- **watchdog**: everything in its header is real and tested (the WWDG has a simulation and timing math; MMIO writes go through the caller's bus, as documented).
- **wind_model**: everything in its header is real and checked against closed forms. Polish: the wind_aep doc at 192-194 names a `steps` parameter that does not exist (the function uses a fixed 4000).

