# L7E style lints: naming and import order (Nov 18 2026)

Roadmap L7E, "Style lints: naming conventions, import ordering". The lints
are in compiler/compiler.fd `lint_source_scan` (with `lx_lint_uses`,
`lx_lint_fn`, `lx_fn_name_ok`, `lx_to_snake`, `lx_to_camel`, `lx_upper`),
in the same section as the unused-variable lint. This note records the
first runs of them and what is left.

## What `fid lint` reports

- a function that is not snake_case (`Type_method` free functions and
  parameterless constant-like functions such as `fn CHAN_CLOSED()` are
  allowed; names starting with `_` are skipped), with the snake_case name;
- a variable (`var`/`val`/`let`/`for`) or parameter that is not
  snake_case (parameters of methods in an `impl Cap for T` are skipped:
  the cap gives them);
- a struct, enum or trait that is not UpperCamelCase (`@repr(C)` and
  `@packed` types mirror C names and are skipped);
- a `const` that is not SCREAMING_SNAKE_CASE;
- a `use` line out of order within a group of `use` lines (adjacent, no
  blank line between): `core`, `vec`, `string` first, in that order, then
  by path.

## Checks

`tests/tooling/lint-style-check.sh` (FID=..., STDLIB=1 adds
scripts/lint-check.sh, EXAMPLES=1 adds scripts/examples-check.sh):

1. tests/bootstrap/test_style_lint.fd: exactly the five style warnings of
   its header, in order, the unused import, nothing for `PointPair_first`
   and `LIMIT_DEFAULT()`, "6 lint warning(s).", output 7 and 3.
2. tests/tooling/lint_style/edge.fd (new): eleven warnings covering
   `parseHTTPHeader` -> `parse_http_header`, `HTTPServer_portNumber` ->
   `http_server_port_number`, a function with a parameter named like a
   constant, a camelCase parameter and loop variable, an enum, a cap, a
   second import group (not compared with the first), and the names that
   must not be reported (`HTTPServer`, `@repr(C) struct c_point`,
   `HTTPServer_port`, `DEFAULT_PORT()`, `_InternalHelper`, `_skipMe`, the
   cap method's `scaleBy`). Output 16 5 17 86 17.
3. scripts/verify/lint-scan-predict.py (the scan copied in Python) gives
   the same style warnings as fid on both files and on the five examples.
4. The five examples of scripts/examples-check.sh print "No lint warnings".

## Run with the Nov 16 stage-0 compiler

The Nov 16 compiler already holds the scan. With it (FID_HOME = the
working tree):

- lint-style-check.sh: every check passes except the `cap` line of
  edge.fd (see below): 26 of 28.
- The five examples: no lint warnings. scripts/examples-check.sh then fails
  on *running* lexer.fd: the working tree's core.fd calls `p_region_word`
  (the regions work of another session), which this compiler does not
  have. Not a lint result; rerun with the final compiler.
- scripts/lint-check.sh on the 403 stdlib modules: 198 clean, 202 with
  warnings, 3 unchecked. Against scripts/lint-check.baseline, the modules
  the baseline records fail only for reasons that are not style lints:
  `crypto_bind` and `crypto_bind_unix` (a new unused import
  `crypto_bind_rsa`), `scada` (was unchecked, now two unused variables).
  The other failures are the 138 modules not in the baseline (the 200
  modules another session is writing today).

## The baseline already holds the style warnings of the older modules

scripts/lint-check.baseline (Nov 17) was saved with a compiler that had the
scan, so its counts include the style warnings:

- public API kept as it is (renaming would break users): `math` (`pub
  struct vec2/vec3/vec4/mat4/quat`, 5), `sdl2_h` (SDL's own constant names,
  33), `shape` (`pub const shape_1x1` ..., 10 of its 12);
- import order of 15 modules: dns, dtls, ed25519, http, http2, http2_tls,
  http3, http3_udp, http_client, quic_conn, quic_tls, quic_udp, srtp,
  tls_crypto, wss. These are not API: they should be sorted (the hunks are
  below) and the baseline then saved again (`scripts/lint-check.sh --save`
  after a clean run), which lowers those counts.

`lint-scan-predict.py --gate` adds the scan's warnings on top of the
baseline, as if the baseline had none, so with this baseline it reports
modules that the real gate passes (math, shape, sdl2_h and the 15 above).
Use scripts/lint-check.sh with a compiler for the gate.

## Left

1. A `cap` (Fid's trait keyword) is not checked: the scan tests for
   `trait`, which is not a keyword. compiler/compiler.fd
   `lint_source_scan`, the struct/enum/trait branch, should read

       } else if (lx_is(toks, i, "struct") || lx_is(toks, i, "enum") || lx_is(toks, i, "trait") || (lx_is(toks, i, "cap") && !lx_is(toks, i - 1, ".") && (lx_is(toks, i + 2, "{") || lx_is(toks, i + 2, ":") || lx_is(toks, i + 2, "<")))) && lx_ident_at(toks, i + 1) {

   (the guard keeps a `.cap` field and `cap as i64` out). The predictor
   already has this rule; edge.fd's line 58 warning tests it. No stdlib,
   example or test cap breaks the convention, so nothing else changes.
2. Import order in the 15 older stdlib modules and three examples
   (examples/http2_server.fd, minimal_game.fd, neural_network.fd; none of
   the five that examples-check.sh lints): sort each group.
3. 43 of the new modules get style warnings (most are import order; the
   names: `openadr` camelCase parameters such as `requestID`, `mysql`
   variables `S0`/`S1`): acme 3, audit_log 1, bms 1, dsp 2, ecdsa 1,
   forecast 1, grpc 2, historian 1, hpke 1, initrd 1, jose 5, kalman 1,
   kinematics 1, lidar 1, lp_solver 1, lwm2m 1, mavlink 1, microgrid 1,
   middleware 1, mongodb 3, mysql 5, noise_proto 2, oauth2 2, odometry 1,
   opcua 1, openadr 16, pem 1, power_flow 2, pv_model 2, rsa 2, secrets 1,
   secure_store 1, senml 1, share 1, ssh 3, svd 1, synchrophasor 2,
   tariff 2, thermal 1, webrtc 1, wind_model 2, wireguard 1, x509 4. They
   belong to the session writing them; they fail the gate as new modules
   in any case until each is clean.
4. No `fid fix` for these warnings.
