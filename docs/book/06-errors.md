# 06-errors. Errors

> Stub — outline only. Expand as usage grows.

panic(msg) for the unrecoverable — prints a trace, exits nonzero. Option/Result + ?? for the recoverable. checked arithmetic by default; @crashfree functions promise no panic path. Post-mortem: fid run x.fd --post-mortem saves a trace on panic; fid replay <trace> replays it.
