# TLS 1.3 in `tls` (Schannel and OpenSSL)

Competitive roadmap SL4 / release I7. `use tls` is `stdlib/tls_windows.fd`
(Schannel) on Windows and `stdlib/tls_unix.fd` (OpenSSL) elsewhere.

## What a program gets

- `tls_connect`, `tls_client`, `tls_server`: the system's choice of
  version. On Windows 11 / Server 2022 this now includes TLS 1.3: the
  credentials go through `SCH_CREDENTIALS` (the legacy `SCHANNEL_CRED`
  stops at 1.2; a Windows release that does not know `SCH_CREDENTIALS`
  gets `SCHANNEL_CRED` as before). OpenSSL always offered 1.3.
- `tls_connect13`, `tls_client13`, `tls_server13`,
  `tls_connect13_unchecked`: TLS 1.3 only (Schannel:
  `grbitDisabledProtocols` = every bit but `SP_PROT_TLS1_3`; OpenSSL:
  `SSL_CTX_set_min_proto_version`).
- `tls_session_version(s)`: 772 for TLS 1.3, 771 for 1.2, on both.
- ALPN: `tls_connect_alpn`, `tls_server_alpn`, `tls_session_alpn`
  (`stdlib/tls_alpn.fd` for the wire format).
- `tls_server_close(server)`: frees a server. On Windows it also deletes
  the private key that `tls_server` imported into the user's key store
  (Schannel cannot use a key held only in memory, so the .pfx is imported
  as a CNG key, `PKCS12_ALWAYS_CNG_KSP`: TLS 1.3 signs with RSA-PSS, which
  the legacy CryptoAPI providers cannot do).
- `tls_library()`: "schannel" or "openssl".

## Schannel details fixed for 1.3 (Nov 17)

- After a TLS 1.3 handshake the server sends NewSessionTicket;
  `DecryptMessage` returns `SEC_I_RENEGOTIATE` for it. `tls_session_recv`
  now gives those bytes to `InitializeSecurityContext` (or
  `AcceptSecurityContext` on a server) and goes on with what is left
  (`ts_post_handshake`); before, the first read of a 1.3 session failed.
  The same path serves KeyUpdate and a TLS 1.2 renegotiation.
- The ALPN choice is read when the handshake ends and kept: a 1.3 client's
  query answers "none" once it has taken the ticket.

## Checks

- `tests/bootstrap/test_tls13_local.fd` (suite, no network): a built-in
  self-signed certificate, a `tls_server13` with ALPN, a 1.3-only client
  and a default client offering "h2"; version 772 and one message each way
  on both sides. Nov 17: passes with `fid ccompile` and `fid emit` on
  Windows 11 and with OpenSSL 3.6.2 in WSL; the imported key is deleted.
- `scripts/verify/tls13_live.fd` (network): `tls_connect` and
  `tls_connect13` to a public host, a GET over each. Nov 17, github.com and
  www.cloudflare.com: 772 and an HTTP answer on both connections, Windows
  (C and native) and Linux.
- `tests/tls/tls13_server_live.fd`: the same as the suite test with a
  certificate of your own.
