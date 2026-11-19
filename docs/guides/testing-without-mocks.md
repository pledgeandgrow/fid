# Testing without mocks: files, the clock and the network

A test replaces the file system, the clock or the network for the code it
tests, and that code does not change: no interface to inject, no mocking
framework. `stdlib/world.fd` holds the fakes, and the standard library's
effectful functions ask it first.

| Effect the code has | What it touches | Fake | Functions that use the fake |
|---|---|---|---|
| `IO` | files | `world_fake_files()`, `world_file_put` | fs: `fs_read_text`, `fs_read_lines`, `fs_read_bytes`, `fs_write_text`, `fs_write_bytes`, `fs_append_text`, `fs_exists`, `fs_is_file`, `fs_file_size`, `fs_delete`; io: `Reader::open`, `Writer::create`, `Writer::append` |
| `IO` | the clock | `world_fake_clock(ms)`, `world_clock_advance`, `world_clock_auto_step` | time: `clock_now_ms/us/ns`, `clock_elapsed_*`; datetime: `dt_now` |
| `Net` | HTTP | `world_fake_network(handler)`, `world_no_network()` | every http_client request (`http_get`, `http_post`, ...) |

A function whose effects (reference §9.1) have neither `IO` nor `Net`
touches none of these, so there is nothing to fake for it.

```fides
use fs;
use world;

fn count_lines(path: str) -> i64 {          // the code under test
    return fs_read_lines(path).len();
}

@hermetic
fn test_count() {
    world_hermetic();                         // every fake on, all empty
    world_file_put("in.txt", "a\nb\n");
    assert(count_lines("in.txt") == 3);
    assert(world_file_reads() == 1);
}
```

## What a test can check

- Files: `world_file_text(path)`, `world_file_list()`, `world_file_reads()`,
  `world_file_writes()`.
- The clock: `world_clock_read_count()`, `world_clock_peek()`. With
  `world_clock_auto_step(ms)` every reading moves the clock, so code that
  waits for time to pass sees it pass, without sleeping.
- The network: the handler gets the raw request (`world_req_method`,
  `world_req_path`, `world_req_header`, `world_req_body`) and returns a raw
  response (`world_http_response(status, body)`; `""` is a failed
  connection). `world_net_count()`, `world_net_host(i)` and
  `world_net_request(i)` list what was sent; `world_no_network()` refuses
  everything and still records each attempt.

`world_real_files()`, `world_real_clock()`, `world_real_network()` and
`world_real()` turn the fakes off again.

## `@hermetic`: checked at compile time

A function marked `@hermetic` (a test) may reach files, the clock and the
network only through functions that consult `stdlib/world`
(`compiler/typechecker.fd`, `TypeChecker_verify_hermetic`). The checker
follows its calls, including the method calls it resolved, and enters only
callees whose inferred effects have `IO` or `Net`. A direct runtime call
on the way is an error that names the call path:

```
Hermetic error: 'test_load' is @hermetic but reaches files directly:
'read_file' calls 'p_file_read' (effect IO), on the path
test_load -> load_config -> read_file.
```

Such a call is `p_file_*`, `p_dir_*`, `p_mkdir`/`p_remove`/`p_rename`,
`p_clock_*`/`p_time_now`, `p_socket_*`/`p_net_*`/`p_tcp_*`/`p_udp_*`/
`p_dns_*`, or `p_http_*` other than the build and parse helpers. Core's
`read_file` is one of them; `fs_read_text` is the faked one.

Not covered yet: fs's `File` type, `fs_list_dir`, `fs_mkdir` and raw
TCP/UDP sockets (stdlib/net.fd) still use the real system, so a
`@hermetic` test that reaches them is rejected. A function passed as a
value (a callback) is not followed by the check.

Tests: `test_world_files`, `test_world_clock`, `test_world_net` (one per
effect), `test_world_fakes` (all three together), `test_world_hermetic`,
and the rejected `test_world_hermetic_fail`,
`test_world_hermetic_clock_fail`, `test_world_hermetic_net_fail`.
