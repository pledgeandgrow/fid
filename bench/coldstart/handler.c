/* bench/coldstart/handler.c: the handler of
 * tests/bootstrap/test_serverless_entry.fd written in C, the floor a
 * process start can reach (bench/coldstart.sh builds it with zig cc -O2).
 * The request is argv[1], else one line of stdin; the response on stdout. */
#include <stdio.h>
#include <string.h>

static const char *GREETING = "hello world";
enum { MAX_BODY = 1024 };

static int route(const char *path) {
    return strcmp(path, "/hello") == 0 ? 200 : 404;
}

int main(int argc, char **argv) {
    char line[4096];
    const char *path = "";
    if (argc > 1) {
        path = argv[1];
    } else if (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        path = line;
    }
    int status = route(path);
    long body_len = 0;
    if (status == 200) {
        body_len = (long)strlen(GREETING);
    }
    if (body_len > MAX_BODY) {
        body_len = MAX_BODY;
    }
    printf("%d %s %ld\n", status, path, body_len);
    return 0;
}
