// pledge_net.c — Network backend for Fides
//
// Implements pledge_net.h: socket helpers for UDP/TCP networking.
// Wraps POSIX socket functions to handle sockaddr_in construction.
//
// Build: gcc -c pledge_net.c -o pledge_net.o
// Link: (Linux: sockets in libc, no extra lib) (Windows: -lws2_32)

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int socklen_t;
#else
#include <sys/time.h>
#endif

// ─── Helpers ─────────────────────────────────────────────────────────────

static struct sockaddr_in make_addr(long long ip, long long port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)port);
    addr.sin_addr.s_addr = (ip == 0) ? INADDR_ANY : (in_addr_t)ip;
    return addr;
}

// ─── API ─────────────────────────────────────────────────────────────────

long long pledge_net_socket(long long domain, long long type, long long protocol) {
    return (long long)socket((int)domain, (int)type, (int)protocol);
}

long long pledge_net_close(long long sock) {
    return (long long)close((int)sock);
}

long long pledge_net_inet_addr(char* ip_str) {
    return (long long)inet_addr(ip_str);
}

long long pledge_net_bind(long long sock, long long port) {
    struct sockaddr_in addr = make_addr(0, port);
    int opt = 1;
    setsockopt((int)sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    return (long long)bind((int)sock, (struct sockaddr*)&addr, sizeof(addr));
}

long long pledge_net_set_nonblocking(long long sock) {
#ifdef _WIN32
    u_long mode = 1;
    return (long long)ioctlsocket((int)sock, FIONBIO, &mode);
#else
    int flags = fcntl((int)sock, F_GETFL, 0);
    return (long long)fcntl((int)sock, F_SETFL, flags | O_NONBLOCK);
#endif
}

long long pledge_net_set_recv_timeout(long long sock, long long timeout_ms) {
    struct timeval tv;
    tv.tv_sec = (time_t)(timeout_ms / 1000);
    tv.tv_usec = (suseconds_t)((timeout_ms % 1000) * 1000);
    return (long long)setsockopt((int)sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

long long pledge_net_sendto(long long sock, void* data, long long len, long long ip, long long port) {
    struct sockaddr_in addr = make_addr(ip, port);
    return (long long)sendto((int)sock, data, (size_t)len, 0,
                             (struct sockaddr*)&addr, sizeof(addr));
}

long long pledge_net_recvfrom(long long sock, void* buf, long long len, void* from_addr) {
    struct sockaddr_in src;
    socklen_t src_len = sizeof(src);
    long long n = (long long)recvfrom((int)sock, buf, (size_t)len, 0,
                                       (struct sockaddr*)&src, &src_len);
    // Store source address info into from_addr (Fides NetAddr struct)
    if (from_addr && n >= 0) {
        // NetAddr layout: { ip: i64, port: i64, raw: i64 }
        long long* addr = (long long*)from_addr;
        addr[0] = (long long)src.sin_addr.s_addr;
        addr[1] = (long long)ntohs(src.sin_port);
        addr[2] = 0;
    }
    return n;
}

long long pledge_net_connect(long long sock, long long ip, long long port) {
    struct sockaddr_in addr = make_addr(ip, port);
    return (long long)connect((int)sock, (struct sockaddr*)&addr, sizeof(addr));
}

long long pledge_net_send(long long sock, void* data, long long len) {
    return (long long)send((int)sock, data, (size_t)len, 0);
}

long long pledge_net_recv(long long sock, void* buf, long long len) {
    return (long long)recv((int)sock, buf, (size_t)len, 0);
}
