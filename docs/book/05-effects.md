# 05-effects. Effects and annotations

> Stub — outline only. Expand as usage grows.

Every function carries an EffectSet (alloc, read, write, io, diverge). pure/!read unlock optimizer merges; @crashfree promises no panic (trusted today — elision is opt-in); @no-alloc asserts allocation-free; @wrapping switches to wrap arithmetic; @send gates task arguments; @secret/@zeroize for crypto; @repr(C)/@packed for layout. docs/reference/safety-rules.md is the contract.
