# 9. Writing a service

A real HTTP service end to end: routes, JSON, testing the routes without
a socket, configuration, background work, building it for a server, and
what happens when it crashes. The service is three files: the routes in a
module, a program that checks them, and the program that serves them.

## The routes

```fides
// routes.fd
module routes;
use http;
use json;

pub fn route(req: HttpRequest) -> str {
    if req.path == "/" && req.method == "GET" {
        return build_response(200, "text/plain", "up\n");
    }
    if req.path == "/version" && req.method == "GET" {
        var body: str = json_obj_set("{}", "name", json_string("svc"));
        body = json_obj_set(body, "version", json_num(1));
        return build_response(200, "application/json", body);
    }
    if req.path == "/hello" && req.method == "POST" {
        if !json_is_valid(req.body) {
            return build_response(400, "text/plain", "bad json\n");
        }
        val user: str = json_get_str(req.body, "user");
        if str_len(user) == 0 {
            return build_response(400, "text/plain", "missing user\n");
        }
        return build_response(200, "text/plain", "hello, " + user + "\n");
    }
    return build_response(404, "text/plain", "Not Found\n");
}
```

A handler is a function from `HttpRequest` to the response text. The
request has `method`, `path`, `query`, `body` and the headers
(`http_header(req, "Content-Type")`); `build_response(status,
content_type, body)` writes the status line, `Content-Type` and
`Content-Length` for you, and `build_response_headers` takes extra
headers.

JSON is text in Fid's `json` module: `json_obj_set(obj, key, value)`
adds a field to an object, `json_string` and `json_num` make values,
`json_is_valid` checks a body, and `json_get_str` / `json_get_int` /
`json_get_bool` read a field with the quotes and escapes taken off
(`json_get` gives the raw JSON text of the field).

## Testing the routes without a socket

Because a route is a plain function, a test can build requests and look at
the answers. `http_parse_head` turns request text into an `HttpRequest`:

```fides
// route_check.fd
use http;
use routes;

fn call(method: str, path: str, body: str) -> str {
    var req: HttpRequest = http_parse_head(method + " " + path + " HTTP/1.1\r\nHost: x\r\n\r\n");
    req.body = body;
    val response: str = route(req);
    // "HTTP/1.1 200 OK\r\n<headers>\r\n\r\n<body>" -> "200 OK <body>"
    val status: str = str_slice(response, 9, str_find(response, "\r\n"));
    val at: i64 = str_find(response, "\r\n\r\n");
    return status + " " + str_trim(str_slice(response, at + 4, str_len(response)));
}

fn main() -> i64 {
    println(call("GET", "/", ""));
    println(call("GET", "/version", ""));
    println(call("POST", "/hello", "{\"user\": \"ada\"}"));
    println(call("POST", "/hello", "{not json"));
    println(call("POST", "/hello", "{}"));
    println(call("GET", "/nope", ""));
    return 0;
}
```

```
$ fid run route_check.fd
200 OK up
200 OK {"name":"svc","version":1}
200 OK hello, ada
400 Bad Request bad json
400 Bad Request missing user
404 Not Found Not Found
```

This is the test to run on every change (`fid run`, or as a `fid test`
file, chapter 7); `docs/guides/testing-without-mocks.md` has more of the
pattern.

## Serving, configuration and background work

```fides
// svc.fd
use http;
use args;
use env;
use toml;
use task;
use routes;

fn sweeper(every_ms: i64) -> i64 {
    while true {
        task_sleep(every_ms);
        println("[sweeper] tick");
    }
    return 0;
}

fn main() -> i64 {
    // precedence: --port flag, then SVC_PORT, then svc.toml, then 8080
    val cfg: Vec<TomlEntry> = toml_parse(read_file("svc.toml") ?? "");
    var port: i64 = toml_get_int(cfg, "port", 8080);
    val env_port: str = env_get("SVC_PORT");
    if str_len(env_port) > 0 { port = str_parse_int(env_port); }
    port = args_int_in(args_all(), "-p", "--port", port);
    val sweep_ms: i64 = toml_get_int(cfg, "sweep_ms", 60000);

    val t: i64 = task_go("sweeper", sweep_ms);
    task_detach(t);
    println("listening on :" + int_to_str(port));
    if !http_serve(port, route) {
        println("port " + int_to_str(port) + " is busy");
        return 1;
    }
    return 0;
}
```

Run it and talk to it from another terminal:

```bash
printf 'port = 8080\nsweep_ms = 5000\n' > svc.toml
fid emit svc.fd -o svc.exe && ./svc.exe        # or through C: CC="zig cc" fid ccompile svc.fd -o svc.exe
curl http://127.0.0.1:8080/version             # {"name":"svc","version":1}
curl -d '{"user":"ada"}' http://127.0.0.1:8080/hello   # hello, ada
curl http://127.0.0.1:8080/health              # {"status":"ok"}
./svc.exe --port 9000                          # the flag wins over svc.toml and SVC_PORT
```

What the server does for you:

- **Each connection is a task** (chapter 8). One thread serves many
  clients, keep-alive and pipelined requests included, and a slow client
  holds up only its own connection.
- **`/health`** is answered by the server itself with
  `{"status":"ok"}`, for load balancers; `http_set_health(false)` turns it
  off.
- **Limits**: a request head over 64 KiB or a body over 16 MiB is
  refused, and a connection silent for 30 s is closed.
- **Stopping**: `http_stop()`, from any task, ends the accept loop
  within about 200 ms and lets the open connections finish.

`http_serve` returns `false` at once when it cannot listen on the port.
The `sweeper` is ordinary background work: `task_go("sweeper", arg)`
starts a task by its function's name with one `i64` argument, and
`task_detach` lets it run for the life of the program. A task gives the
others their turn whenever it waits (`task_sleep`, a socket, a channel).

## When it panics

A panic in a handler ends the whole server, not just that request: it
prints `Panic: <message>` and a trace on stderr and exits with status 1
(chapter 6). So supervise the process (systemd `Restart=`, a shell loop,
a process manager), keep stderr in your logs, and let a handler that can
fail return a 4xx/5xx instead of panicking. `fid` has no post-mortem
recorder (`--post-mortem` and `replay` belong to the frozen `fidesc`);
reproduce a crash under `fid run`, or with a `route_check.fd`-style test,
for a trace with line numbers.

For logic that must never panic, annotate the function that holds it:

```fides
// health.fd
use http;

// The decision is crashfree; building the response is not (it allocates
// and calls unannotated stdlib code), so it stays outside.
@crashfree fn health_status(db_up: bool, queue_len: i64) -> i64 {
    if !db_up { return 503; }
    if queue_len > 10000 { return 429; }
    return 200;
}

fn health(req: HttpRequest) -> str {
    val code: i64 = health_status(true, 3);
    return build_response(code, "text/plain", "ok\n");
}

fn main() -> i64 {
    println(int_to_str(health_status(false, 0)));
    println(int_to_str(health_status(true, 20000)));
    println(int_to_str(health_status(true, 3)));
    return 0;
}
```

```
$ fid run health.fd
503
429
200
```

`@crashfree` is checked by the compiler on every build: no `unwrap`, no
`v.get(i)`, no division by anything but a non-zero literal, no checked
`+ - *` (unless `@wrapping`), no `while`, and calls only to functions that
are safety-annotated too (chapter 5). So a whole handler can't be
`@crashfree`: `build_response` is ordinary stdlib code, and the compiler
rejects the call.

```fides
// crash_fail.fd
use http;

@crashfree fn handler(req: HttpRequest) -> str {
    return build_response(200, "text/plain", "ok\n");    // error: build_response is not crashfree
}

fn main() -> i64 { return 0; }
```

```
$ fid check crash_fail.fd
...
error[E097]: Safety error (crashfree): function 'handler' is safety-annotated but calls 'build_response', (line 4)
...
```

## Building for the server

```bash
# A static Linux executable from any host: no docker, no cross toolchain
fid emit svc.fd --target x86_64-linux -o svc
scp svc svc.toml prod:/srv/svc/ && ssh prod 'systemctl restart svc'
```

Or through C (`zig cc` cross-compiles too, as a static musl executable):

```bash
CC="zig cc" fid ccompile svc.fd --target x86_64-linux -o svc
```

`svc.fd` reads `svc.toml` from the directory it runs in: set the
service's working directory, or pass `--port` and `SVC_PORT` instead.

## A checklist before it faces traffic

- The route checks pass: `fid run route_check.fd`, and the same through
  the build you ship.
- `fid audit svc.fd`: know what unsafe code you reach.
- `CC="zig cc" fid run svc.fd --checked-alloc` under a load test: memory
  bugs fail loudly instead of corrupting a response.
- Decide your error contract: which routes may `panic` (a bug: the
  process restarts) and which answer 4xx/5xx.
- `@crashfree` only on decisions you can argue by inspection.
- Pin the toolchain: `fid version` in your build log.

## Common mistakes

- **Building JSON with `json_pair`.** `json_pair(k, v)` makes a whole
  object, `{"k":v}`, not a member: joining two of them with `,` inside
  `{ }` is not JSON. Build objects with `json_obj_set`.
- **`json_get` for a string field.** It gives the raw JSON text,
  quotes and escapes included (`"ada"`, with the quotes). Use
  `json_get_str`, `json_get_int` or `json_get_bool`.
- **Expecting your route to see `/health`.** The server answers it first;
  call `http_set_health(false)` to serve your own.
- **Long computations in a handler.** Connections are tasks on one
  thread: a handler that computes for a second holds up every other
  connection for that second. Hand heavy work to a thread (`spawn`,
  chapter 8).
- **Trusting the port from the environment.** `str_parse_int("80x")` is
  80 and `str_parse_int("")` is 0; check the number before you use it.
