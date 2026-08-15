// Test: HTTP stdlib — Phase 5D.3
// Tests HTTP request parsing and response building.
// The actual server (serve function) is blocking, so we test the
// parsing/building functions which are the core logic.

use http;
use json;

pf main() {
    // ─── Parse HTTP request ──────────────────────────────────────────
    pv raw = "GET /api/users?id=42 HTTP/1.1\r\nHost: localhost:8080\r\nAccept: application/json\r\n\r\n";
    pv req = parse_request(raw);
    print(req);
    print("\n");

    // ─── Extract fields from parsed request ──────────────────────────
    pv method = json_get(req, "method");
    print(method);
    print("\n");

    pv path = json_get(req, "path");
    print(path);
    print("\n");

    pv version = json_get(req, "version");
    print(version);
    print("\n");

    // ─── Build HTTP response ─────────────────────────────────────────
    pv body = "{\"status\":\"ok\"}";
    pv response = build_response(200, "application/json", body);
    print(response);
    print("\n");

    // ─── Status text ─────────────────────────────────────────────────
    pv s200 = status_text(200);
    print(s200);
    print("\n");

    pv s404 = status_text(404);
    print(s404);
    print("\n");

    // ─── Parse POST request with body ────────────────────────────────
    pv post_raw = "POST /api/submit HTTP/1.1\r\nContent-Type: application/json\r\nContent-Length: 15\r\n\r\n{\"name\":\"alice\"}";
    pv post_req = parse_request(post_raw);
    pv post_method = json_get(post_req, "method");
    print(post_method);
    print("\n");

    pv post_body = json_get(post_req, "body");
    print(post_body);
    print("\n");

    ret 0;
}
