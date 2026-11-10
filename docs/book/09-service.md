# 9. Writing a service

A real HTTP service end to end: routes, JSON, config, concurrency,
logging, building for Linux, and what happens when it crashes.

## The smallest service

```fides
// svc.fd — fid emit svc.fd && ./svc
use http;

fn route(req: HttpRequest) -> str {
    if req.path == "/" && req.method == "GET" {
        return build_response(200, "text/plain", "up\n");
    }
    return build_response(404, "text/plain", "Not Found\n");
}

fn main() -> i64 {
    if !http_serve(8080, route) { println("port 8080 busy"); return 1; }
    return 0;
}
```

`http_serve` takes a *function value* — `route` must be a top-level `fn`.
Each request maps to an `HttpRequest` (`method`, `path`, `body`,
headers); the handler returns a response string built with
`build_response(status, content_type, body)`.

## Adding a JSON API

```fides
use http;
use json;
use string;

fn route(req: HttpRequest) -> str {
    if req.path == "/json" && req.method == "GET" {
        var body: str = "{" + json_pair("status", json_string("ok"))
            + "," + json_pair("version", "1") + "}";
        return build_response(200, "application/json", body);
    }
    if req.path == "/echo" && req.method == "POST" {
        if !json_is_valid(req.body) {
            return build_response(400, "text/plain", "bad json\n");
        }
        println("echo from " + json_get(req.body, "user"));
        return build_response(200, "application/json", req.body);
    }
    return build_response(404, "text/plain", "Not Found\n");
}
```

`json_get` returns raw JSON text for the key — `json_unescape` it if the
value is a string literal you want to use directly.

## Config

```fides
use args;
use env;
use toml;

fn main() -> i64 {
    // precedence: flag > env > file default
    var port: i64 = args_int_in(args_all(), "-p", "--port", 8080);
    var envport: str = env_get("SVC_PORT");
    if str_len(envport) > 0 { port = str_parse_int(envport); }

    // TOML for the rest
    var cfg: str = file_read("svc.toml");   // use file;
    var workers: i64 = 4;
    if str_len(cfg) > 0 {
        var w: str = toml_get(cfg, "workers");
        if str_len(w) > 0 { workers = str_parse_int(w); }
    }
    println("listening on :" + int_to_str(port));
    if !http_serve(port, route) { return 1; }
    return 0;
}
```

## Work between requests

Handlers are serial; for background work use tasks (chapter 8):

```fides
use task;

fn sweeper() -> i64 {
    while true {
        task_sleep(60000);
        println("[sweeper] tick");
    }
    return 0;
}

fn main() -> i64 {
    var t: i64 = task_go("sweeper", 0);
    task_detach(t);
    if !http_serve(8080, route) { return 1; }
    return 0;
}
```

## When it panics

A `panic` prints a source-level trace and exits nonzero — supervise it
(systemd `Restart=`, a shell loop, a process manager). For post-mortems:

```bash
fid run svc.fd --post-mortem   # saves a trace on panic
fid replay <trace>             # step through what happened
```

For endpoints that must never panic, annotate:

```fides
@crashfree fn health(req: HttpRequest) -> str {
    return build_response(200, "text/plain", "ok\n");
}
```

`@crashfree` is a contract: the compiler trusts it (panic-check elision
behind `FID_NAT_ON=crashfree`); today the emit path does not prove it —
write the function so it genuinely can't panic (no indexing, no unwrap,
no arithmetic that can overflow — or handle those paths explicitly).

## Building for the server

```bash
# Static Linux binary from any host — no docker, no qemu
fid emit svc.fd --target x86_64-linux -o svc
scp svc prod:/srv/ && ssh prod 'systemctl restart svc'
```

Or the C path when you need a system OpenSSL/libc link:

```bash
fid ccompile svc.fd -o svc
```

## A checklist before it faces traffic

- `fid audit svc.fd` — know what unsafe code you reach.
- `fid run svc.fd --checked-alloc` + a load test — memory bugs fail loudly.
- Decide your error contract: which routes may `panic` (bug = restart),
  which return `Result`-mapped 4xx/5xx.
- `@crashfree` only on paths you can argue by inspection.
- Pin the toolchain: `fid version` in your build log.
