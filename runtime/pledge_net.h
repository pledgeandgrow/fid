// pledge_net.h — Network helpers for Fides FFI

#ifndef PLEDGE_NET_H
#define PLEDGE_NET_H

long long pledge_net_socket(long long domain, long long type, long long protocol);
long long pledge_net_bind(long long sock, long long port);
long long pledge_net_set_nonblocking(long long sock);
long long pledge_net_set_recv_timeout(long long sock, long long timeout_ms);
long long pledge_net_sendto(long long sock, void* data, long long len, long long ip, long long port);
long long pledge_net_recvfrom(long long sock, void* buf, long long len, void* from_addr);
long long pledge_net_connect(long long sock, long long ip, long long port);
long long pledge_net_send(long long sock, void* data, long long len);
long long pledge_net_recv(long long sock, void* buf, long long len);
long long pledge_net_close(long long sock);
long long pledge_net_inet_addr(char* ip_str);

#endif // PLEDGE_NET_H
