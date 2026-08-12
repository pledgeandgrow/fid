// Auto-generated C code from Pledge IR
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#ifdef abs
#undef abs
#endif

// ─── Runtime types ───
typedef struct { void** data; int64_t len; int64_t cap; } PVec;
typedef struct { char** keys; int64_t* vals; int64_t len; int64_t cap; } PMap;

// ─── Runtime function declarations ───
PVec* p_vec_new(void);
void p_vec_push(PVec* v, void* val);
void* p_vec_get(PVec* v, int64_t idx);
int64_t p_vec_len(PVec* v);
int64_t p_vec_pop(PVec* v);
bool p_vec_is_empty(PVec* v);
void p_vec_clear(PVec* v);
PMap* p_map_new(void);
void p_map_insert(PMap* m, const char* key, int64_t val);
int64_t p_map_get(PMap* m, const char* key);
bool p_map_contains(PMap* m, const char* key);
void p_map_remove(PMap* m, const char* key);
int64_t p_map_len(PMap* m);
bool p_map_is_empty(PMap* m);
void p_map_clear(PMap* m);
PVec* p_map_keys(PMap* m);
PVec* p_map_values(PMap* m);
int64_t p_string_len(const char* s);
bool p_string_is_empty(const char* s);
char* p_string_push_str(const char* s, const char* other);
char* p_string_from(const char* s);
char* p_string_slice(const char* s, int64_t start, int64_t end);
bool p_string_contains(const char* s, const char* sub);
bool p_string_starts_with(const char* s, const char* prefix);
bool p_string_ends_with(const char* s, const char* suffix);
int64_t p_string_find(const char* s, const char* sub);
char* p_string_trim(const char* s);
char* p_string_to_uppercase(const char* s);
char* p_string_to_lowercase(const char* s);
char* p_string_repeat(const char* s, int64_t n);
char* p_string_replace(const char* s, const char* from, const char* to);
PVec* p_string_split(const char* s, const char* delim);
int64_t p_string_parse_int(const char* s);
char* p_string_char_at(const char* s, int64_t idx);
char* p_string_substring(const char* s, int64_t start, int64_t end);
char* p_to_str(int64_t val);
char* p_to_str_vec(PVec* v);
char* p_to_str_str(const char* s);
char* p_to_str_bool(bool val);
char* p_to_str_float(double val);
char* p_to_str_map(PMap* m);
char* p_str_concat(const char* a, const char* b);
void p_print(int64_t val);
void p_print_str(const char* s);
void p_print_int(int64_t val);
void p_print_bool(bool val);
void p_print_float(double val);
void p_print_newline(void);
void p_panic(const char* msg);
void* p_malloc(int64_t size);
void p_free(void* ptr);
void* p_allocator_create(void* arg);
int64_t p_allocator_alloc(void* alloc, int64_t size);
void p_allocator_free(void* alloc, int64_t ptr);
int64_t p_allocator_stats(void* alloc);
int64_t p_socket_listen(int64_t port);
int64_t p_socket_accept(int64_t fd);
int64_t p_socket_read(int64_t fd, char* buf, int64_t max_len);
int64_t p_socket_write(int64_t fd, const char* buf, int64_t len);
void p_socket_close(int64_t fd);
char* p_socket_recv_str(int64_t fd);
int64_t print(int64_t val);
int64_t print_str(const char* s);
void p_vec_clear(PVec* v);
PVec* p_vec_clone(PVec* v);
bool p_vec_contains(PVec* v, void* val);
void p_vec_extend(PVec* v, PVec* other);
void* p_vec_first(PVec* v);
void* p_vec_last(PVec* v);
void p_vec_insert(PVec* v, int64_t idx, void* val);
void* p_vec_remove(PVec* v, int64_t idx);
void p_vec_reverse(PVec* v);
void p_vec_sort(PVec* v);
void p_vec_truncate(PVec* v, int64_t len);
PMap* p_map_clone(PMap* m);
int64_t p_map_get_or(PMap* m, const char* key, int64_t default_val);

// ─── String constants ───
static const char _str0[] = "Hello, World!";
static const char _str1[] = "Pledge";
static const char _str2[] = "Hello, ";
static const char _str3[] = "!";
static const char _str4[] = "x = ";
static const char _str5[] = "called unwrap on None";
static const char _str6[] = "Assertion failed\n";
static const char _str7[] = "\n";
static const char _str8[] = "called unwrap on Err";

// ─── Struct definitions ───
struct Option_s {
    int64_t field_0;
    int64_t field_1;
};

struct Result_s {
    int64_t field_0;
    int64_t field_1;
};

// ─── Function prototypes ───
void* create_allocator(void* v0, void* v1);
int64_t main_entry(void* v0);
int64_t Result_unwrap_or(void* v0, void* v1, int64_t v2);
void Result_err(void* v0, void* v1, void* v2);
void Result_ok(void* v0, void* v1, void* v2);
int64_t clamp(void* v0, int64_t v1, int64_t v2, int64_t v3);
void allocator_free(void* v0, void* v1, int64_t v2);
int64_t allocator_bytes_used(void* v0, void* v1);
void Option_map(void* v0, void* v1, void* v2, int64_t v3);
int64_t p_min(void* v0, int64_t v1, int64_t v2);
bool Result_is_err(void* v0, void* v1);
int64_t p_abs(void* v0, int64_t v1);
int64_t allocator_alloc(void* v0, void* v1, int64_t v2);
bool Option_is_some(void* v0, void* v1);
int64_t Option_unwrap(void* v0, void* v1);
int64_t Option_unwrap_or(void* v0, void* v1, int64_t v2);
bool Option_is_none(void* v0, void* v1);
void assert(void* v0, bool v1);
void println(void* v0, void* v1);
bool Result_is_ok(void* v0, void* v1);
int64_t Result_unwrap(void* v0, void* v1);
int64_t p_max(void* v0, int64_t v1, int64_t v2);

void* create_allocator(void* v0, void* v1) {
    void* v2;
    int64_t v3; /* placeholder for void */
    v2 = p_allocator_create(v1);
    return v2;
    return 0;
}

int64_t main_entry(void* v0) {
    void* v20;
    void* v4;
    void* v6;
    int64_t v7; /* placeholder for void */
    void* v18;
    void* v12;
    void* v3;
    int64_t v2; /* placeholder for void */
    int64_t v21;
    void* v8;
    int64_t v5; /* placeholder for void */
    void* v13;
    void* v15;
    void* v22;
    int64_t v25; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    void* v10;
    void* v1;
    int64_t v24; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v17;
    void* v23;
    void* v11;
    void* v9;
    v1 = _str0;
    p_print_str(v1);
    v3 = _str1;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = *(void**)v4;
    p_print_str(v6);
    v8 = _str2;
    v9 = *(void**)v4;
    v10 = p_str_concat(v8, v9);
    v11 = _str3;
    v12 = p_str_concat(v10, v11);
    v13 = malloc(8);
    *(void**)v13 = (void*)v12;
    v15 = *(void**)v13;
    p_print_str(v15);
    v17 = 42LL;
    v18 = malloc(8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = _str4;
    v21 = *(int64_t*)v18;
    v22 = (void*)p_to_str((int64_t)v21);
    v23 = p_str_concat(v20, v22);
    p_print_str(v23);
    return 0;
    return 0;
}

int64_t Result_unwrap_or(void* v0, void* v1, int64_t v2) {
    int64_t v7; /* placeholder for void */
    bool v14;
    int64_t v10; /* placeholder for void */
    void* v3;
    int64_t v9;
    int64_t v13;
    int64_t v4;
    int64_t v17;
    int64_t v20;
    int64_t v21; /* placeholder for void */
    void* v8;
    int64_t v12;
    void* v11;
    int64_t v15; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    bool v6;
    int64_t v5;
    void* v16;
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    v20 = 0LL;
    return v20;
  bb2:
    v8 = (char*)v1 + 8;
    v9 = *(int64_t*)v8;
    return v9;
  bb3:
    v11 = (char*)v1 + 0;
    v12 = *(int64_t*)v11;
    v13 = 1LL;
    v14 = v12 == v13;
    if (v14) goto bb4;
    else goto bb5;
  bb4:
    v16 = (char*)v1 + 8;
    v17 = *(int64_t*)v16;
    return v2;
  bb5:
    goto bb1;
    return 0;
}

void Result_err(void* v0, void* v1, void* v2) {
    int64_t v29;
    void* v34;
    int64_t v9;
    int64_t v16;
    int64_t v35; /* placeholder for void */
    int64_t v25;
    int64_t v4;
    int64_t v13; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    void* v10;
    int64_t v33; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v24;
    void* v28;
    void* v8;
    void* v36;
    int64_t v7; /* placeholder for void */
    bool v6;
    int64_t v31;
    void* v3;
    int64_t v38;
    void* v19;
    int64_t v20;
    void* v37;
    bool v26;
    int64_t v42;
    int64_t v45; /* placeholder for void */
    void* v18;
    int64_t v21; /* placeholder for void */
    int64_t v11;
    int64_t v17; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v5;
    void* v40;
    void* v32;
    void* v14;
    void* v41;
    int64_t v44; /* placeholder for void */
    void* v23;
    void* v15;
    int64_t v27; /* placeholder for void */
    void* v30;
    void* v12;
    v3 = (char*)v2 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    return;
  bb2:
    v8 = (char*)v2 + 8;
    v9 = *(int64_t*)v8;
    v10 = malloc(16);
    v11 = 1LL;
    v12 = (char*)v10 + 0;
    *(int64_t*)v12 = (int64_t)v11;
    v14 = (char*)v10 + 0;
    v15 = (char*)v0 + 0;
    v16 = *(int64_t*)v14;
    *(int64_t*)v15 = (int64_t)v16;
    v18 = (char*)v10 + 8;
    v19 = (char*)v0 + 8;
    v20 = *(int64_t*)v18;
    *(int64_t*)v19 = (int64_t)v20;
    return;
  bb3:
    v23 = (char*)v2 + 0;
    v24 = *(int64_t*)v23;
    v25 = 1LL;
    v26 = v24 == v25;
    if (v26) goto bb4;
    else goto bb5;
  bb4:
    v28 = (char*)v2 + 8;
    v29 = *(int64_t*)v28;
    v30 = malloc(16);
    v31 = 0LL;
    v32 = (char*)v30 + 0;
    *(int64_t*)v32 = (int64_t)v31;
    v34 = (char*)v30 + 8;
    *(int64_t*)v34 = (int64_t)v29;
    v36 = (char*)v30 + 0;
    v37 = (char*)v0 + 0;
    v38 = *(int64_t*)v36;
    *(int64_t*)v37 = (int64_t)v38;
    v40 = (char*)v30 + 8;
    v41 = (char*)v0 + 8;
    v42 = *(int64_t*)v40;
    *(int64_t*)v41 = (int64_t)v42;
    return;
  bb5:
    goto bb1;
    return;
}

void Result_ok(void* v0, void* v1, void* v2) {
    void* v3;
    int64_t v7; /* placeholder for void */
    int64_t v38;
    void* v16;
    int64_t v26;
    int64_t v27;
    int64_t v24; /* placeholder for void */
    void* v10;
    int64_t v13; /* placeholder for void */
    int64_t v5;
    int64_t v19; /* placeholder for void */
    bool v28;
    void* v34;
    int64_t v35; /* placeholder for void */
    void* v37;
    int64_t v39; /* placeholder for void */
    void* v40;
    int64_t v4;
    void* v20;
    void* v17;
    void* v32;
    int64_t v22;
    void* v14;
    void* v30;
    void* v36;
    int64_t v11;
    void* v25;
    int64_t v9;
    int64_t v31;
    void* v41;
    int64_t v29; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v33;
    int64_t v42;
    void* v8;
    int64_t v45; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    bool v6;
    void* v12;
    int64_t v18;
    void* v21;
    int64_t v43; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    v3 = (char*)v2 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    return;
  bb2:
    v8 = (char*)v2 + 8;
    v9 = *(int64_t*)v8;
    v10 = malloc(16);
    v11 = 0LL;
    v12 = (char*)v10 + 0;
    *(int64_t*)v12 = (int64_t)v11;
    v14 = (char*)v10 + 8;
    *(int64_t*)v14 = (int64_t)v9;
    v16 = (char*)v10 + 0;
    v17 = (char*)v0 + 0;
    v18 = *(int64_t*)v16;
    *(int64_t*)v17 = (int64_t)v18;
    v20 = (char*)v10 + 8;
    v21 = (char*)v0 + 8;
    v22 = *(int64_t*)v20;
    *(int64_t*)v21 = (int64_t)v22;
    return;
  bb3:
    v25 = (char*)v2 + 0;
    v26 = *(int64_t*)v25;
    v27 = 1LL;
    v28 = v26 == v27;
    if (v28) goto bb4;
    else goto bb5;
  bb4:
    v30 = (char*)v2 + 8;
    v31 = *(int64_t*)v30;
    v32 = malloc(16);
    v33 = 1LL;
    v34 = (char*)v32 + 0;
    *(int64_t*)v34 = (int64_t)v33;
    v36 = (char*)v32 + 0;
    v37 = (char*)v0 + 0;
    v38 = *(int64_t*)v36;
    *(int64_t*)v37 = (int64_t)v38;
    v40 = (char*)v32 + 8;
    v41 = (char*)v0 + 8;
    v42 = *(int64_t*)v40;
    *(int64_t*)v41 = (int64_t)v42;
    return;
  bb5:
    goto bb1;
    return;
}

int64_t clamp(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    bool v4;
    int64_t v12; /* placeholder for void */
    bool v8;
    int64_t v11; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    v4 = v1 < v2;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    return v2;
  bb2:
    goto bb3;
  bb3:
    v8 = v1 > v3;
    if (v8) goto bb4;
    else goto bb5;
  bb4:
    return v3;
  bb5:
    goto bb6;
  bb6:
    return v1;
    return 0;
}

void allocator_free(void* v0, void* v1, int64_t v2) {
    int64_t v4; /* placeholder for void */
    int64_t v3; /* placeholder for void */
    p_allocator_free(v1, v2);
    return;
    return;
}

int64_t allocator_bytes_used(void* v0, void* v1) {
    int64_t v2;
    int64_t v3; /* placeholder for void */
    v2 = (int64_t)p_allocator_stats(v1);
    return v2;
    return 0;
}

void Option_map(void* v0, void* v1, void* v2, int64_t v3) {
    void* v9;
    int64_t v20;
    int64_t v45; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    void* v19;
    void* v40;
    int64_t v44; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    void* v34;
    bool v7;
    void* v27;
    void* v16;
    int64_t v43; /* placeholder for void */
    void* v4;
    int64_t v6;
    int64_t v8; /* placeholder for void */
    int64_t v29;
    int64_t v33;
    int64_t v35; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    void* v36;
    void* v41;
    int64_t v42;
    int64_t v14; /* placeholder for void */
    void* v23;
    void* v32;
    int64_t v38;
    int64_t v31; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v12;
    void* v11;
    void* v18;
    int64_t v28;
    int64_t v46; /* placeholder for void */
    int64_t v10;
    int64_t v15;
    bool v30;
    void* v37;
    int64_t v25; /* placeholder for void */
    int64_t v5;
    void* v13;
    void* v22;
    int64_t v24;
    v4 = (char*)v2 + 0;
    v5 = *(int64_t*)v4;
    v6 = 0LL;
    v7 = v5 == v6;
    if (v7) goto bb2;
    else goto bb3;
  bb1:
    return;
  bb2:
    v9 = (char*)v2 + 8;
    v10 = *(int64_t*)v9;
    v11 = malloc(16);
    v12 = 0LL;
    v13 = (char*)v11 + 0;
    *(int64_t*)v13 = (int64_t)v12;
    v15 = v10 + v3;
    v16 = (char*)v11 + 8;
    *(int64_t*)v16 = (int64_t)v15;
    v18 = (char*)v11 + 0;
    v19 = (char*)v0 + 0;
    v20 = *(int64_t*)v18;
    *(int64_t*)v19 = (int64_t)v20;
    v22 = (char*)v11 + 8;
    v23 = (char*)v0 + 8;
    v24 = *(int64_t*)v22;
    *(int64_t*)v23 = (int64_t)v24;
    return;
  bb3:
    v27 = (char*)v2 + 0;
    v28 = *(int64_t*)v27;
    v29 = 1LL;
    v30 = v28 == v29;
    if (v30) goto bb4;
    else goto bb5;
  bb4:
    v32 = malloc(16);
    v33 = 1LL;
    v34 = (char*)v32 + 0;
    *(int64_t*)v34 = (int64_t)v33;
    v36 = (char*)v32 + 0;
    v37 = (char*)v0 + 0;
    v38 = *(int64_t*)v36;
    *(int64_t*)v37 = (int64_t)v38;
    v40 = (char*)v32 + 8;
    v41 = (char*)v0 + 8;
    v42 = *(int64_t*)v40;
    *(int64_t*)v41 = (int64_t)v42;
    return;
  bb5:
    goto bb1;
    return;
}

int64_t p_min(void* v0, int64_t v1, int64_t v2) {
    int64_t v5; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    bool v3;
    int64_t v6; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    v3 = v1 < v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    return v1;
  bb2:
    goto bb3;
  bb3:
    return v2;
    return 0;
}

bool Result_is_err(void* v0, void* v1) {
    int64_t v3;
    int64_t v17;
    bool v18;
    int64_t v19; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v12;
    bool v5;
    int64_t v13;
    void* v2;
    int64_t v4;
    int64_t v22; /* placeholder for void */
    bool v9;
    bool v21;
    void* v16;
    void* v7;
    int64_t v6; /* placeholder for void */
    void* v11;
    bool v14;
    int64_t v15; /* placeholder for void */
    int64_t v8;
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v21 = false;
    return v21;
  bb2:
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    v9 = false;
    return v9;
  bb3:
    v11 = (char*)v1 + 0;
    v12 = *(int64_t*)v11;
    v13 = 1LL;
    v14 = v12 == v13;
    if (v14) goto bb4;
    else goto bb5;
  bb4:
    v16 = (char*)v1 + 8;
    v17 = *(int64_t*)v16;
    v18 = true;
    return v18;
  bb5:
    goto bb1;
    return 0;
}

int64_t p_abs(void* v0, int64_t v1) {
    int64_t v4; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v2;
    int64_t v6; /* placeholder for void */
    bool v3;
    int64_t v5;
    int64_t v7; /* placeholder for void */
    v2 = 0LL;
    v3 = v1 < v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = -v1;
    return v5;
  bb2:
    goto bb3;
  bb3:
    return v1;
    return 0;
}

int64_t allocator_alloc(void* v0, void* v1, int64_t v2) {
    int64_t v4; /* placeholder for void */
    int64_t v3;
    v3 = (int64_t)p_allocator_alloc(v1, v2);
    return v3;
    return 0;
}

bool Option_is_some(void* v0, void* v1) {
    int64_t v15; /* placeholder for void */
    void* v11;
    bool v16;
    bool v14;
    int64_t v3;
    int64_t v12;
    int64_t v20; /* placeholder for void */
    int64_t v8;
    int64_t v18; /* placeholder for void */
    int64_t v13;
    int64_t v10; /* placeholder for void */
    bool v19;
    int64_t v17; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    void* v2;
    bool v9;
    int64_t v4;
    bool v5;
    void* v7;
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v19 = false;
    return v19;
  bb2:
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    v9 = true;
    return v9;
  bb3:
    v11 = (char*)v1 + 0;
    v12 = *(int64_t*)v11;
    v13 = 1LL;
    v14 = v12 == v13;
    if (v14) goto bb4;
    else goto bb5;
  bb4:
    v16 = false;
    return v16;
  bb5:
    goto bb1;
    return 0;
}

int64_t Option_unwrap(void* v0, void* v1) {
    void* v2;
    int64_t v12;
    int64_t v14; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v19;
    int64_t v20; /* placeholder for void */
    void* v15;
    bool v5;
    void* v7;
    int64_t v3;
    void* v10;
    bool v13;
    int64_t v6; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v4;
    int64_t v11;
    int64_t v8;
    int64_t v16; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v19 = 0LL;
    return v19;
  bb2:
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    return v8;
  bb3:
    v10 = (char*)v1 + 0;
    v11 = *(int64_t*)v10;
    v12 = 1LL;
    v13 = v11 == v12;
    if (v13) goto bb4;
    else goto bb5;
  bb4:
    v15 = _str5;
    p_panic(v15);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

int64_t Option_unwrap_or(void* v0, void* v1, int64_t v2) {
    int64_t v5;
    int64_t v16; /* placeholder for void */
    int64_t v4;
    int64_t v10; /* placeholder for void */
    bool v6;
    int64_t v19; /* placeholder for void */
    void* v11;
    void* v3;
    int64_t v15; /* placeholder for void */
    bool v14;
    int64_t v17; /* placeholder for void */
    int64_t v18;
    int64_t v13;
    int64_t v7; /* placeholder for void */
    void* v8;
    int64_t v9;
    int64_t v12;
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    v18 = 0LL;
    return v18;
  bb2:
    v8 = (char*)v1 + 8;
    v9 = *(int64_t*)v8;
    return v9;
  bb3:
    v11 = (char*)v1 + 0;
    v12 = *(int64_t*)v11;
    v13 = 1LL;
    v14 = v12 == v13;
    if (v14) goto bb4;
    else goto bb5;
  bb4:
    return v2;
  bb5:
    goto bb1;
    return 0;
}

bool Option_is_none(void* v0, void* v1) {
    bool v19;
    int64_t v15; /* placeholder for void */
    bool v5;
    bool v9;
    int64_t v8;
    void* v2;
    int64_t v20; /* placeholder for void */
    bool v16;
    int64_t v6; /* placeholder for void */
    int64_t v12;
    void* v11;
    bool v14;
    int64_t v10; /* placeholder for void */
    int64_t v4;
    void* v7;
    int64_t v13;
    int64_t v3;
    int64_t v17; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v19 = false;
    return v19;
  bb2:
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    v9 = false;
    return v9;
  bb3:
    v11 = (char*)v1 + 0;
    v12 = *(int64_t*)v11;
    v13 = 1LL;
    v14 = v12 == v13;
    if (v14) goto bb4;
    else goto bb5;
  bb4:
    v16 = true;
    return v16;
  bb5:
    goto bb1;
    return 0;
}

void assert(void* v0, bool v1) {
    int64_t v3; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    void* v4;
    int64_t v2;
    int64_t v7; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    v2 = !v1;
    if (v2) goto bb1;
    else goto bb2;
  bb1:
    v4 = _str6;
    p_print_str(v4);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    return;
    return;
}

void println(void* v0, void* v1) {
    int64_t v5; /* placeholder for void */
    int64_t v2; /* placeholder for void */
    void* v3;
    int64_t v4; /* placeholder for void */
    p_print_str(v1);
    v3 = _str7;
    p_print_str(v3);
    return;
    return;
}

bool Result_is_ok(void* v0, void* v1) {
    int64_t v20; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v13;
    bool v14;
    bool v18;
    void* v2;
    bool v5;
    bool v21;
    int64_t v3;
    void* v11;
    void* v16;
    int64_t v6; /* placeholder for void */
    void* v7;
    int64_t v12;
    int64_t v4;
    int64_t v22; /* placeholder for void */
    int64_t v8;
    int64_t v17;
    int64_t v10; /* placeholder for void */
    bool v9;
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v21 = false;
    return v21;
  bb2:
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    v9 = true;
    return v9;
  bb3:
    v11 = (char*)v1 + 0;
    v12 = *(int64_t*)v11;
    v13 = 1LL;
    v14 = v12 == v13;
    if (v14) goto bb4;
    else goto bb5;
  bb4:
    v16 = (char*)v1 + 8;
    v17 = *(int64_t*)v16;
    v18 = false;
    return v18;
  bb5:
    goto bb1;
    return 0;
}

int64_t Result_unwrap(void* v0, void* v1) {
    int64_t v4;
    int64_t v20; /* placeholder for void */
    void* v15;
    int64_t v19; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v16;
    int64_t v6; /* placeholder for void */
    void* v10;
    int64_t v8;
    int64_t v3;
    void* v17;
    int64_t v21;
    void* v2;
    int64_t v9; /* placeholder for void */
    void* v7;
    int64_t v14; /* placeholder for void */
    int64_t v11;
    int64_t v18; /* placeholder for void */
    bool v13;
    int64_t v12;
    bool v5;
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v21 = 0LL;
    return v21;
  bb2:
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    return v8;
  bb3:
    v10 = (char*)v1 + 0;
    v11 = *(int64_t*)v10;
    v12 = 1LL;
    v13 = v11 == v12;
    if (v13) goto bb4;
    else goto bb5;
  bb4:
    v15 = (char*)v1 + 8;
    v16 = *(int64_t*)v15;
    v17 = _str8;
    p_panic(v17);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

int64_t p_max(void* v0, int64_t v1, int64_t v2) {
    int64_t v6; /* placeholder for void */
    bool v3;
    int64_t v7; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    v3 = v1 > v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    return v1;
  bb2:
    goto bb3;
  bb3:
    return v2;
    return 0;
}

// ─── C entry point ───
int main(int argc, char** argv) {
    int64_t result = main_entry(NULL);
    return (int)result;
}


// ─── Runtime implementation ───

PVec* p_vec_new(void) {{
    PVec* v = (PVec*)malloc(sizeof(PVec));
    v->data = NULL; v->len = 0; v->cap = 0;
    return v;
}}

void p_vec_push(PVec* v, void* val) {{
    if (!v) return;
    if (v->len >= v->cap) {{
        v->cap = v->cap == 0 ? 8 : v->cap * 2;
        v->data = (void**)realloc(v->data, v->cap * sizeof(void*));
    }}
    v->data[v->len++] = val;
}}

void* p_vec_get(PVec* v, int64_t idx) {{
    if (!v || idx < 0 || idx >= v->len) return NULL;
    return v->data[idx];
}}

int64_t p_vec_len(PVec* v) {{ return v ? v->len : 0; }}
bool p_vec_is_empty(PVec* v) {{ return v ? v->len == 0 : true; }}
void p_vec_clear(PVec* v) {{ if (v) v->len = 0; }}
int64_t p_vec_pop(PVec* v) {{
    if (!v || v->len == 0) return 0;
    v->len--;
    return (int64_t)v->data[v->len];
}}

PMap* p_map_new(void) {{
    PMap* m = (PMap*)malloc(sizeof(PMap));
    m->keys = NULL; m->vals = NULL; m->len = 0; m->cap = 0;
    return m;
}}

void p_map_insert(PMap* m, const char* key, int64_t val) {{
    if (!m || !key) return;
    for (int64_t i = 0; i < m->len; i++) {{
        if (strcmp(m->keys[i], key) == 0) {{ m->vals[i] = val; return; }}
    }}
    if (m->len >= m->cap) {{
        m->cap = m->cap == 0 ? 8 : m->cap * 2;
        m->keys = (char**)realloc(m->keys, m->cap * sizeof(char*));
        m->vals = (int64_t*)realloc(m->vals, m->cap * sizeof(int64_t));
    }}
    m->keys[m->len] = strdup(key);
    m->vals[m->len] = val;
    m->len++;
}}

int64_t p_map_get(PMap* m, const char* key) {{
    if (!m || !key) return 0;
    for (int64_t i = 0; i < m->len; i++) {{
        if (strcmp(m->keys[i], key) == 0) return m->vals[i];
    }}
    return 0;
}}

bool p_map_contains(PMap* m, const char* key) {{
    if (!m || !key) return false;
    for (int64_t i = 0; i < m->len; i++) {{
        if (strcmp(m->keys[i], key) == 0) return true;
    }}
    return false;
}}

void p_map_remove(PMap* m, const char* key) {{
    if (!m || !key) return;
    for (int64_t i = 0; i < m->len; i++) {{
        if (strcmp(m->keys[i], key) == 0) {{
            free(m->keys[i]);
            m->keys[i] = m->keys[m->len-1];
            m->vals[i] = m->vals[m->len-1];
            m->len--;
            return;
        }}
    }}
}}

int64_t p_map_len(PMap* m) {{ return m ? m->len : 0; }}
bool p_map_is_empty(PMap* m) {{ return m ? m->len == 0 : true; }}
void p_map_clear(PMap* m) {{ if (m) m->len = 0; }}

PVec* p_map_keys(PMap* m) {{
    PVec* v = p_vec_new();
    for (int64_t i = 0; i < m->len; i++)
        p_vec_push(v, (void*)m->keys[i]);
    return v;
}}

PVec* p_map_values(PMap* m) {{
    PVec* v = p_vec_new();
    for (int64_t i = 0; i < m->len; i++) {{
    int64_t* p = (int64_t*)malloc(sizeof(int64_t));
    *p = m->vals[i];
    p_vec_push(v, p);
    }}
    return v;
}}

int64_t p_string_len(const char* s) {{ return s ? (int64_t)strlen(s) : 0; }}
bool p_string_is_empty(const char* s) {{ return !s || s[0] == 0; }}

char* p_string_push_str(const char* s, const char* other) {{
    if (!s) s = ""; if (!other) other = "";
    size_t len = strlen(s) + strlen(other);
    char* result = (char*)malloc(len + 1);
    strcpy(result, s);
    strcat(result, other);
    return result;
}}

char* p_string_from(const char* s) {{ return s ? strdup(s) : strdup(""); }}

char* p_string_slice(const char* s, int64_t start, int64_t end) {{
    if (!s) return strdup("");
    int64_t len = (int64_t)strlen(s);
    if (start < 0) start = 0;
    if (end > len) end = len;
    if (start > end) start = end;
    int64_t slen = end - start;
    char* result = (char*)malloc(slen + 1);
    memcpy(result, s + start, slen);
    result[slen] = 0;
    return result;
}}

char* p_to_str(int64_t val) {{
    char* buf = (char*)malloc(32);
    snprintf(buf, 32, "%lld", (long long)val);
    return buf;
}}

char* p_str_concat(const char* a, const char* b) {{
    size_t len = strlen(a) + strlen(b);
    char* result = (char*)malloc(len + 1);
    strcpy(result, a);
    strcat(result, b);
    return result;
}}

void p_print(int64_t val) { printf("%lld", (long long)val); }
void p_print_str(const char* s) { printf("%s", s); }
void p_print_int(int64_t val) { printf("%lld", (long long)val); }
void p_print_bool(bool val) { printf("%s", val ? "true" : "false"); }
void p_print_float(double val) { printf("%g", val); }
void p_print_newline(void) { printf("\n"); }
void p_panic(const char* msg) { fprintf(stderr, "%s\n", msg); abort(); }
void* p_malloc(int64_t size) { return malloc((size_t)size); }
void p_free(void* ptr) { free(ptr); }
void* p_allocator_create(void* arg) { (void)arg; return (void*)1; }
int64_t p_allocator_alloc(void* alloc, int64_t size) { (void)alloc; return (int64_t)(intptr_t)malloc((size_t)size); }
void p_allocator_free(void* alloc, int64_t ptr) { (void)alloc; free((void*)(intptr_t)ptr); }
int64_t p_allocator_stats(void* alloc) { (void)alloc; return 0; }
int64_t print(int64_t val) { printf("%lld", (long long)val); return 0; }
int64_t print_str(const char* s) { printf("%s", s); return 0; }

bool p_string_contains(const char* s, const char* sub) {{ return s && sub && strstr(s, sub) != NULL; }}
bool p_string_starts_with(const char* s, const char* prefix) {{
    if (!s || !prefix) return false;
    size_t plen = strlen(prefix);
    return strncmp(s, prefix, plen) == 0;
}}
bool p_string_ends_with(const char* s, const char* suffix) {{
    if (!s || !suffix) return false;
    size_t slen = strlen(s), flen = strlen(suffix);
    if (flen > slen) return false;
    return strcmp(s + slen - flen, suffix) == 0;
}}
int64_t p_string_find(const char* s, const char* sub) {{
    if (!s || !sub) return -1;
    const char* p = strstr(s, sub);
    return p ? (int64_t)(p - s) : -1;
}}
char* p_string_trim(const char* s) {{
    if (!s) return strdup("");
    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') s++;
    size_t len = strlen(s);
    while (len > 0 && (s[len-1] == ' ' || s[len-1] == '\t' || s[len-1] == '\n' || s[len-1] == '\r')) len--;
    char* result = (char*)malloc(len + 1);
    memcpy(result, s, len);
    result[len] = 0;
    return result;
}}
char* p_string_to_uppercase(const char* s) {{
    if (!s) return strdup("");
    size_t len = strlen(s);
    char* result = (char*)malloc(len + 1);
    for (size_t i = 0; i < len; i++) result[i] = toupper((unsigned char)s[i]);
    result[len] = 0;
    return result;
}}
char* p_string_to_lowercase(const char* s) {{
    if (!s) return strdup("");
    size_t len = strlen(s);
    char* result = (char*)malloc(len + 1);
    for (size_t i = 0; i < len; i++) result[i] = tolower((unsigned char)s[i]);
    result[len] = 0;
    return result;
}}
char* p_string_repeat(const char* s, int64_t n) {{
    if (!s) return strdup("");
    size_t slen = strlen(s);
    char* result = (char*)malloc(slen * (size_t)n + 1);
    result[0] = 0;
    for (int64_t i = 0; i < n; i++) strcat(result, s);
    return result;
}}
char* p_string_replace(const char* s, const char* from, const char* to) {{
    if (!s) return strdup("");
    if (!from || !to || !*from) return strdup(s);
    size_t slen = strlen(s), flen = strlen(from), tlen = strlen(to);
    char* result = (char*)malloc(slen * (tlen > flen ? tlen : flen) + 1);
    size_t ri = 0; size_t si = 0;
    while (si < slen) {{
        if (slen - si >= flen && strncmp(s + si, from, flen) == 0) {{
            memcpy(result + ri, to, tlen); ri += tlen; si += flen;
        }} else {{ result[ri++] = s[si++]; }}
    }}
    result[ri] = 0;
    return result;
}}
PVec* p_string_split(const char* s, const char* delim) {{
    PVec* v = p_vec_new();
    if (!s || !delim) return v;
    const char* start = s;
    size_t dlen = strlen(delim);
    while (*start) {{
        const char* p = strstr(start, delim);
        if (!p) {{ p_vec_push(v, strdup(start)); break; }}
        size_t len = p - start;
        char* part = (char*)malloc(len + 1);
        memcpy(part, start, len); part[len] = 0;
        p_vec_push(v, part);
        start = p + dlen;
    }}
    return v;
}}
int64_t p_string_parse_int(const char* s) {{ return s ? (int64_t)atoll(s) : 0; }}
char* p_string_char_at(const char* s, int64_t idx) {{
    if (!s) return strdup("");
    char* result = (char*)malloc(2);
    result[0] = s[idx]; result[1] = 0;
    return result;
}}
char* p_string_substring(const char* s, int64_t start, int64_t end) {{
    return p_string_slice(s, start, end);
}}

PVec* p_vec_clone(PVec* v) {{
    PVec* c = p_vec_new();
    for (int64_t i = 0; i < v->len; i++) p_vec_push(c, v->data[i]);
    return c;
}}
bool p_vec_contains(PVec* v, void* val) {{
    for (int64_t i = 0; i < v->len; i++) if (v->data[i] == val) return true;
    return false;
}}
void p_vec_extend(PVec* v, PVec* other) {{
    for (int64_t i = 0; i < other->len; i++) p_vec_push(v, other->data[i]);
}}
void* p_vec_first(PVec* v) {{ return v->len > 0 ? v->data[0] : NULL; }}
void* p_vec_last(PVec* v) {{ return v->len > 0 ? v->data[v->len - 1] : NULL; }}
void p_vec_insert(PVec* v, int64_t idx, void* val) {{
    if (idx < 0 || idx > v->len) return;
    if (v->len >= v->cap) {{ v->cap = v->cap == 0 ? 8 : v->cap * 2; v->data = (void**)realloc(v->data, v->cap * sizeof(void*)); }}
    memmove(&v->data[idx+1], &v->data[idx], (v->len - idx) * sizeof(void*));
    v->data[idx] = val; v->len++;
}}
void* p_vec_remove(PVec* v, int64_t idx) {{
    if (idx < 0 || idx >= v->len) return NULL;
    void* val = v->data[idx];
    memmove(&v->data[idx], &v->data[idx+1], (v->len - idx - 1) * sizeof(void*));
    v->len--;
    return val;
}}
void p_vec_reverse(PVec* v) {{
    for (int64_t i = 0, j = v->len - 1; i < j; i++, j--) {{ void* t = v->data[i]; v->data[i] = v->data[j]; v->data[j] = t; }}
}}
void p_vec_sort(PVec* v) {{
    for (int64_t i = 0; i < v->len; i++)
        for (int64_t j = i+1; j < v->len; j++)
            if ((int64_t)v->data[i] > (int64_t)v->data[j]) {{ void* t = v->data[i]; v->data[i] = v->data[j]; v->data[j] = t; }}
}}
void p_vec_truncate(PVec* v, int64_t len) {{ if (len < v->len) v->len = len; }}

PMap* p_map_clone(PMap* m) {{
    PMap* c = p_map_new();
    for (int64_t i = 0; i < m->len; i++) p_map_insert(c, m->keys[i], m->vals[i]);
    return c;
}}
int64_t p_map_get_or(PMap* m, const char* key, int64_t default_val) {{
    for (int64_t i = 0; i < m->len; i++) if (strcmp(m->keys[i], key) == 0) return m->vals[i];
    return default_val;
}}

char* p_to_str_vec(PVec* v) {{
    char* result = strdup("[");
    for (int64_t i = 0; i < v->len; i++) {{
        if (i > 0) {{ char* tmp = p_str_concat(result, ", "); free(result); result = tmp; }}
        char* s = p_to_str((int64_t)v->data[i]);
        char* tmp = p_str_concat(result, s); free(result); free(s); result = tmp;
    }}
    char* tmp = p_str_concat(result, "]"); free(result); result = tmp;
    return result;
}}
char* p_to_str_map(PMap* m) {{
    if (!m) return strdup("{{}}");
    // Sort keys for deterministic output
    int64_t* order = (int64_t*)malloc(m->len * sizeof(int64_t));
    for (int64_t i = 0; i < m->len; i++) order[i] = i;
    for (int64_t i = 0; i < m->len; i++)
        for (int64_t j = i+1; j < m->len; j++)
            if (strcmp(m->keys[order[i]], m->keys[order[j]]) > 0) {{ int64_t t = order[i]; order[i] = order[j]; order[j] = t; }}
    char* result = strdup("{");
    for (int64_t i = 0; i < m->len; i++) {{
        if (i > 0) {{ char* tmp = p_str_concat(result, ", "); free(result); result = tmp; }}
        char* tmp = p_str_concat(result, m->keys[order[i]]); free(result); result = tmp;
        tmp = p_str_concat(result, ": "); free(result); result = tmp;
        char* vs = p_to_str(m->vals[order[i]]);
        tmp = p_str_concat(result, vs); free(result); free(vs); result = tmp;
    }}
    char* tmp = p_str_concat(result, "}"); free(result); result = tmp;
    free(order);
    return result;
}}
char* p_to_str_str(const char* s) {{ return strdup(s); }}
char* p_to_str_bool(bool val) {{ return strdup(val ? "true" : "false"); }}
char* p_to_str_float(double val) {{
    char* buf = (char*)malloc(32);
    snprintf(buf, 32, "%g", val);
    return buf;
}}

// Socket operations — native POSIX/Winsock implementation
#ifdef _WIN32
static int ws_initialized = 0;
static void init_winsock(void) {
    if (!ws_initialized) { WSADATA d; WSAStartup(MAKEWORD(2,2), &d); ws_initialized = 1; }
}
#endif

int64_t p_socket_listen(int64_t port) {
#ifdef _WIN32
    init_winsock();
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) return -1;
#else
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return -1;
#endif
    int opt = 1;
    setsockopt((int)s, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons((int)port);
    if (bind((int)s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
#ifdef _WIN32
        closesocket(s);
#else
        close(s);
#endif
        return -1;
    }
    if (listen((int)s, 10) < 0) {
#ifdef _WIN32
        closesocket(s);
#else
        close(s);
#endif
        return -1;
    }
    return (int64_t)s;
}

int64_t p_socket_accept(int64_t fd) {
    struct sockaddr_in client;
#ifdef _WIN32
    int clen = sizeof(client);
    SOCKET c = accept((SOCKET)fd, (struct sockaddr*)&client, &clen);
    return (c == INVALID_SOCKET) ? -1 : (int64_t)c;
#else
    socklen_t clen = sizeof(client);
    int c = accept((int)fd, (struct sockaddr*)&client, &clen);
    return (int64_t)c;
#endif
}

int64_t p_socket_read(int64_t fd, char* buf, int64_t max_len) {
#ifdef _WIN32
    int n = recv((SOCKET)fd, buf, (int)max_len, 0);
#else
    ssize_t n = read((int)fd, buf, (size_t)max_len);
#endif
    return (n < 0) ? -1 : (int64_t)n;
}

int64_t p_socket_write(int64_t fd, const char* buf, int64_t len) {
#ifdef _WIN32
    int n = send((SOCKET)fd, buf, (int)len, 0);
#else
    ssize_t n = write((int)fd, buf, (size_t)len);
#endif
    return (n < 0) ? -1 : (int64_t)n;
}

void p_socket_close(int64_t fd) {
#ifdef _WIN32
    closesocket((SOCKET)fd);
#else
    close((int)fd);
#endif
}

char* p_socket_recv_str(int64_t fd) {
    char buf[4096];
    int64_t n = p_socket_read(fd, buf, sizeof(buf)-1);
    if (n <= 0) return strdup("");
    buf[n] = 0;
    return strdup(buf);
}
