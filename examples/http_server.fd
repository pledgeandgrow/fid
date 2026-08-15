// Example: HTTP server in Pledge — Phase 5D.3
// Run with: pledgec run examples/http_server.p
// Then: curl http://localhost:8080/

use http;
use json;

// Override the default handler with our custom route handler.
pf handle_request(raw: str) -> str {
    pv req = parse_request(raw);
    pv path = json_get(req, "path");
    pv method = json_get(req, "method");

    // Route: GET / — hello world
    if path == "\"/\"" && method == "\"GET\"" {
        ret build_response(200, "text/plain", "Hello, Pledge!");
    }

    // Route: GET /json — JSON response
    if path == "\"/json\"" && method == "\"GET\"" {
        ret build_response(200, "application/json", "{\"message\":\"Hello from Pledge HTTP\",\"version\":\"0.1\"}");
    }

    // Route: GET /health — health check
    if path == "\"/health\"" {
        ret build_response(200, "application/json", "{\"status\":\"healthy\"}");
    }

    // 404 for everything else
    ret build_response(404, "text/plain", "Not Found\n");
}

pf main() {
    serve(8080);
}
