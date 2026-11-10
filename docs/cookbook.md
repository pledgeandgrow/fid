# Fides Cookbook

> Common tasks, short recipes. Everything here is plain `.fd` source —
> run it with `fid run file.fd` or build it with `fid emit file.fd`.
> API details: `docs/reference/reference.md`, `docs/stdlib/`.

## Contents

- [Program shape](#program-shape)
- [Command-line arguments](#command-line-arguments)
- [Files](#files)
- [Collections](#collections)
- [Strings](#strings)
- [JSON](#json)
- [Error handling](#error-handling)
- [HTTP server](#http-server)
- [HTTP client](#http-client)
- [Config files (TOML/INI)](#config-files)
- [Concurrency (tasks)](#concurrency)
- [Time and benchmarking](#time-and-benchmarking)
- [Crypto / hashing](#crypto--hashing)
- [Testing](#testing)

## Program shape

Every program is a file with `fn main() -> i64` (the exit code):

```fides
use core;

fn main() -> i64 {
    println("Hello, Fides!");
    return 0;
}
```

`val` binds an immutable, `var` a mutable local. Imports are `use <module>;`
— stdlib modules by name (`use json;`), project/compiler modules by path
(`use compiler::ir;`).

## Command-line arguments

```fides
use core;
use args;

fn main() -> i64 {
    // Positional access
    var i: i64 = 0;
    while i < args_len() {
        println(args_get(i));
        i = i + 1;
    }

    // Flags and options: --verbose, --output out.txt, --jobs 4
    if args_flag("-v", "--verbose") { println("verbose on"); }
    var out: str = args_option("-o", "--output", "a.out");
    var jobs: i64 = args_int_in(args_all(), "-j", "--jobs", 1);
    var rest: Vec<str> = args_positionals_in(args_all());
    println(out + " jobs=" + int_to_str(jobs) + " files=" + int_to_str(rest.len()));
    return 0;
}
```

## Files

```fides
use core;
use file;

fn main() -> i64 {
    if file_write("out.txt", "line one\n") != 0 { return 1; }
    if !file_exists("out.txt") { return 1; }
    var text: str = file_read("out.txt");
    print(text);

    // Append, remove, size also live in `use file;`
    // file_append("out.txt", "line two\n");
    return 0;
}
```

For env vars: `use env;` (`env_get`, `env_has`), or the low-level
`p_getenv("NAME")` which returns an empty str when unset — always check.

## Collections

`Vec<T>` for sequences, `HashMap<K,V>` for maps (see `stdlib/vec.fd`,
`stdlib/collections.fd` for Deque/LinkedList/Set variants):

```fides
use core;
use vec;
use collections;

fn main() -> i64 {
    var v: Vec<i64> = Vec::new();
    v.push(1); v.push(2); v.push(3);
    var i: i64 = 0;
    while i < v.len() { println(int_to_str(v.get(i))); i = i + 1; }

    val m = HashMap::new();
    m.insert("alice", 30);
    m.insert("bob", 25);
    if m.contains("alice") { println(int_to_str(m.get("alice"))); }
    m.remove("bob");
    return 0;
}
```

## Strings

`use string;` — `str_len`, `str_slice`, `str_split`, `str_find`,
`str_starts_with`, `str_trim`, `str_replace`, `str_parse_int`,
`str_join`. Strings are immutable; build with `+` or `StringBuilder`
for loops:

```fides
var sb: StringBuilder = StringBuilder::new();
var i: i64 = 0;
while i < 10 {
    sb.append_str("x");
    i = i + 1;
}
var s: str = sb.to_str();
```

## JSON

`use json;` — the API takes/returns JSON text:

```fides
use core;
use json;

fn main() -> i64 {
    var doc: str = "{\"name\":\"fid\",\"stars\":[1,2,3]}";
    if !json_is_valid(doc) { return 1; }
    println(json_get(doc, "name"));        // "fid"
    var stars: str = json_get(doc, "stars"); // [1,2,3] as text
    println(json_at(stars, 0));            // 1 — index into an array
    println(json_pretty(doc));
    var built: str = "{" + json_pair("ok", "true") + "}";
    println(json_dump(built));
    return 0;
}
```

## Error handling

Fallible calls return `Option<T>`/`Result<T,E>` (see `stdlib/result.fd`);
checked arithmetic uses `checked_add`/`checked_mul` etc.:

```fides
use core;
use result;

fn parse_port(s: str) -> Option {
    if str_len(s) == 0 { return Option::None; }
    var n: i64 = str_parse_int(s);
    if n <= 0 || n > 65535 { return Option::None; }
    return Option::Some(n);
}

// Callers unwrap with match or the ?? coalesce:
//   val port = parse_port(a) ?? 8080;
```

Annotate functions that must not panic with `@crashfree`, that must not
allocate with `@no-alloc`, and wrapping arithmetic with `@wrapping` —
the compiler enforces/uses them (see `docs/reference/safety-rules.md`).

## HTTP server

From `examples/http_server.fd`:

```fides
use http;

fn route(req: HttpRequest) -> str {
    if req.path == "/" && req.method == "GET" {
        return build_response(200, "text/plain", "Hello!\n");
    }
    if req.path == "/echo" && req.method == "POST" {
        return build_response(200, "text/plain", req.body);
    }
    return build_response(404, "text/plain", "Not Found\n");
}

fn main() -> i64 {
    if !http_serve(8080, route) { println("Cannot open port 8080"); return 1; }
    return 0;
}
```

## HTTP client

`use http_client;` (`stdlib/http_client.fd`) — `http_get`, `http_post`,
`http_request` returning status + headers + body. TLS variants live in
`stdlib/https.fd` / `stdlib/tls_windows.fd` / `stdlib/tls_unix.fd`.

## Config files

- TOML: `use toml;` — `toml_parse`, `toml_get`
- INI: `use ini;` — `ini_parse`, `ini_get`
- YAML/JSON/XML/CSV/protobuf also have modules (`yaml.fd`, `xml.fd`,
  `csv.fd`, `protobuf.fd`).

## Concurrency

Tasks are green threads (`use task;`); `spawn`/`parallel {}` also exist —
see `docs/reference/reference.md` for syntax:

```fides
use core;
use task;

fn worker() -> i64 {
    task_sleep(10);
    return 42;
}

fn main() -> i64 {
    var t: i64 = task_go("worker", 0);
    while !task_done(t) { task_yield(); }
    println(int_to_str(task_result(t)));   // 42
    return task_run();
}
```

For parallelism, `parallel { a | b }` runs branches on worker threads;
`use channel;`/`use sync;` provide channels and locks.

## Time and benchmarking

```fides
use core;

fn main() -> i64 {
    var t0: i64 = p_clock_now_ms();
    // ... work ...
    println("took " + int_to_str(p_clock_now_ms() - t0) + "ms");
    return 0;
}
```

`fid bench <file> [-n count]` and `fid profile <file>` are built in.

## Crypto / hashing

`use crypto;` — SHA-256/HMAC (NIST and RFC 4231 vectors tested);
`use hash;` for non-crypto hashes; `p_os_random_u64()` for OS randomness.

## Testing

```fides
use test;
```

plus the suite conventions: programs under `tests/bootstrap/` assert by
exit code and printed output (see `docs/reference/testing.md`). `fid run`,
`fid emit`, and `fid ccompile` should all agree — that's the project's own
differential test.
