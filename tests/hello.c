// Auto-generated C code from Fides IR
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
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
void p_vec_set(PVec* v, int64_t idx, void* val);
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
void p_cli(void);
void p_sti(void);
void p_hlt(void);
int64_t p_cr0_read(void);
int64_t p_cr3_read(void);
int64_t p_cr4_read(void);
void p_cr3_write(int64_t val);
void p_invlpg(int64_t addr);
void p_page_map(int64_t cr3, int64_t virt, int64_t phys, int64_t flags);
void p_page_unmap(int64_t cr3, int64_t virt);
int64_t p_phys_read64(int64_t addr);
void p_phys_write64(int64_t addr, int64_t val);
void* p_simd_add(void* a, void* b);
void* p_simd_sub(void* a, void* b);
void* p_simd_mul(void* a, void* b);
void* p_simd_div(void* a, void* b);
double p_simd_dot(void* a, void* b);
double p_simd_sum(void* a);
void* p_simd_scale(void* a, double s);
void* p_simd_from_vec(void* v);
char* p_simd_to_str(void* a);
void* p_tensor_new(int64_t ndim, int64_t* shape);
void* p_tensor_from(void* data, int64_t ndim, int64_t* shape);
void* p_tensor_zeros(void* shape_vec);
void* p_tensor_ones(void* shape_vec);
void* p_tensor_filled(void* shape_vec, double val);
void* p_tensor_from_vec(void* v, void* shape_vec);
void* p_tensor_clone(void* t);
double p_tensor_at(void* t, int64_t idx);
double p_tensor_get(void* t, void* idx_vec);
void p_tensor_set(void* t, void* idx_vec, double val);
void* p_tensor_add(void* a, void* b);
void* p_tensor_sub(void* a, void* b);
void* p_tensor_mul(void* a, void* b);
void* p_tensor_matmul(void* a, void* b);
void* p_tensor_transpose(void* t);
void* p_tensor_relu(void* t);
void* p_tensor_sigmoid(void* t);
void* p_tensor_softmax(void* t);
void* p_tensor_scale(void* t, double s);
double p_tensor_sum(void* t);
double p_tensor_mean(void* t);
double p_tensor_min(void* t);
double p_tensor_max(void* t);
double p_tensor_dot(void* a, void* b);
void* p_tensor_reshape(void* t, void* shape_vec);
int64_t p_tensor_len(void* t);
int64_t p_tensor_ndim(void* t);
void* p_tensor_shape(void* t);
int64_t p_tensor_size(void* t);
char* p_tensor_to_str(void* t);
char* p_tensor_to_json(void* t);
char* p_json_parse(const char* s);
void* p_json_new_object(void);
void* p_json_new_array(void);
void p_json_set(void* j, const char* key, const char* val);
void p_json_set_int(void* j, const char* key, int64_t val);
void p_json_set_float(void* j, const char* key, double val);
void p_json_set_bool(void* j, const char* key, int64_t val);
void p_json_append(void* j, const char* val);
void p_json_append_int(void* j, int64_t val);
void p_json_append_float(void* j, double val);
char* p_json_dump(void* j);
char* p_json_dump_pretty(void* j);
char* p_json_pretty(void* j);
char* p_json_string(void* j);
const char* p_json_type(void* j);
char* p_json_get(void* j, const char* key);
int64_t p_json_get_int(void* j, const char* key);
double p_json_get_float(void* j, const char* key);
int64_t p_json_get_bool(void* j, const char* key);
void* p_json_at(void* j, int64_t idx);
int64_t p_json_len(void* j);
void* p_json_object_get(void* j, const char* key);
int64_t p_http_listen(int64_t port);
char* p_http_recv_request(int64_t fd);
void p_http_send_response(int64_t fd, int64_t status, const char* body);
char* p_http_build_response(int64_t status, const char* content_type, const char* body);
char* p_http_parse_request(const char* request);
void* p_db_connect(const char* conn_str);
void p_db_exec(void* db, const char* query);
void* p_db_query(void* db, const char* query);
char* p_db_get(void* db, const char* key);
void p_db_set(void* db, const char* key, const char* val);
void* p_db_mem_open(void);
int64_t p_db_mem_exec(void* db, const char* query);
char* p_db_mem_query(void* db, const char* query);
void p_db_mem_close(void* db);
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
char* p_float_to_str(double val);
char* p_bool_to_str(bool val);
char* p_vec_to_str(PVec* v);
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
char* p_file_read(const char* path);
int64_t p_file_write(const char* path, const char* content);
bool p_file_exists(const char* path);
int64_t p_file_append(const char* path, const char* content);
char* p_getenv(const char* name);
int64_t p_argc(void);
char* p_argv(int64_t index);
int64_t p_system(const char* cmd);
void p_exit(int64_t code);
void* p_allocator_create(void* arg);
int64_t p_allocator_alloc(void* alloc, int64_t size);
void p_allocator_free(void* alloc, int64_t ptr);
int64_t p_allocator_stats(void* alloc);
int64_t p_socket_listen(int64_t port);
int64_t p_socket_accept(int64_t fd);
int64_t p_socket_read(int64_t fd, char* buf, int64_t max_len);
int64_t p_socket_write(int64_t fd, const char* buf, int64_t len);
int64_t p_socket_write_str(int64_t fd, const char* buf);
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
static const char _str0[] = "Assertion failed\n";
static const char _str1[] = "called unwrap on Err";
static const char _str2[] = "Hello, Pledge!";
static const char _str3[] = "y is big";
static const char _str4[] = "y is small";
static const char _str5[] = "\n";
static const char _str6[] = "called unwrap on None";

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
void assert(void* v0, bool v1);
int64_t p_abs(void* v0, int64_t v1);
bool Option_is_some(void* v0, void* v1);
int64_t allocator_bytes_used(void* v0, void* v1);
int64_t Result_unwrap(void* v0, void* v1);
int64_t Result_unwrap_or(void* v0, void* v1, int64_t v2);
void Option_map(void* v0, void* v1, void* v2, int64_t v3);
void* create_allocator(void* v0, void* v1);
int64_t main_entry(void* v0);
bool Option_is_none(void* v0, void* v1);
int64_t Option_unwrap_or(void* v0, void* v1, int64_t v2);
int64_t clamp(void* v0, int64_t v1, int64_t v2, int64_t v3);
void println(void* v0, void* v1);
int64_t allocator_alloc(void* v0, void* v1, int64_t v2);
void allocator_free(void* v0, void* v1, int64_t v2);
int64_t Option_unwrap(void* v0, void* v1);
void Result_err(void* v0, void* v1, void* v2);
bool Result_is_err(void* v0, void* v1);
int64_t p_max(void* v0, int64_t v1, int64_t v2);
void Result_ok(void* v0, void* v1, void* v2);
int64_t p_min(void* v0, int64_t v1, int64_t v2);
bool Result_is_ok(void* v0, void* v1);

void assert(void* v0, bool v1) {
    void* v4;
    int64_t v2;
    int64_t v3; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    v2 = !v1;
    if (v2) goto bb1;
    else goto bb2;
  bb1:
    v4 = _str0;
    p_print_str(v4);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    return;
    return;
}

int64_t p_abs(void* v0, int64_t v1) {
    int64_t v5;
    int64_t v4; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v2;
    bool v3;
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

bool Option_is_some(void* v0, void* v1) {
    void* v2;
    bool v16;
    void* v11;
    bool v5;
    bool v14;
    int64_t v4;
    int64_t v12;
    int64_t v6; /* placeholder for void */
    int64_t v13;
    bool v9;
    int64_t v15; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v8;
    bool v19;
    int64_t v18; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v7;
    int64_t v3;
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

int64_t allocator_bytes_used(void* v0, void* v1) {
    int64_t v2;
    int64_t v3; /* placeholder for void */
    v2 = (int64_t)p_allocator_stats(v1);
    return v2;
    return 0;
}

int64_t Result_unwrap(void* v0, void* v1) {
    int64_t v18; /* placeholder for void */
    void* v10;
    int64_t v12;
    int64_t v16;
    int64_t v9; /* placeholder for void */
    int64_t v21;
    void* v2;
    int64_t v11;
    int64_t v19; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    void* v15;
    int64_t v14; /* placeholder for void */
    int64_t v4;
    void* v17;
    int64_t v8;
    int64_t v20; /* placeholder for void */
    int64_t v3;
    void* v7;
    bool v13;
    int64_t v22; /* placeholder for void */
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
    v17 = _str1;
    p_panic(v17);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

int64_t Result_unwrap_or(void* v0, void* v1, int64_t v2) {
    int64_t v7; /* placeholder for void */
    int64_t v12;
    int64_t v10; /* placeholder for void */
    bool v6;
    int64_t v9;
    bool v14;
    int64_t v5;
    void* v8;
    int64_t v13;
    int64_t v15; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    void* v16;
    void* v11;
    int64_t v4;
    int64_t v19; /* placeholder for void */
    void* v3;
    int64_t v17;
    int64_t v20;
    int64_t v21; /* placeholder for void */
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

void Option_map(void* v0, void* v1, void* v2, int64_t v3) {
    void* v23;
    int64_t v29;
    int64_t v8; /* placeholder for void */
    void* v37;
    void* v16;
    int64_t v31; /* placeholder for void */
    int64_t v24;
    int64_t v20;
    int64_t v25; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    void* v36;
    void* v11;
    void* v4;
    int64_t v6;
    void* v13;
    void* v19;
    int64_t v39; /* placeholder for void */
    bool v30;
    void* v18;
    int64_t v5;
    int64_t v12;
    int64_t v44; /* placeholder for void */
    void* v32;
    bool v7;
    void* v41;
    int64_t v33;
    void* v34;
    int64_t v17; /* placeholder for void */
    void* v40;
    void* v22;
    void* v27;
    int64_t v28;
    int64_t v14; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v42;
    int64_t v38;
    int64_t v45; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v15;
    int64_t v10;
    void* v9;
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

void* create_allocator(void* v0, void* v1) {
    void* v2;
    int64_t v3; /* placeholder for void */
    v2 = p_allocator_create(v1);
    return v2;
    return 0;
}

int64_t main_entry(void* v0) {
    int64_t v22; /* placeholder for void */
    bool v15;
    int64_t v19; /* placeholder for void */
    int64_t v3;
    void* v20;
    void* v1;
    int64_t v18; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v6;
    int64_t v11;
    int64_t v8;
    int64_t v13;
    void* v4;
    int64_t v21; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v7;
    int64_t v14;
    int64_t v23; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    void* v9;
    int64_t v2; /* placeholder for void */
    void* v17;
    v1 = _str2;
    p_print_str(v1);
    v3 = 42LL;
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    v7 = 8LL;
    v8 = v6 + v7;
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = *(int64_t*)v9;
    p_print_int((int64_t)v11);
    v13 = *(int64_t*)v9;
    v14 = 40LL;
    v15 = v13 > v14;
    if (v15) goto bb1;
    else goto bb2;
  bb1:
    v17 = _str3;
    p_print_str(v17);
    goto bb3;
  bb2:
    v20 = _str4;
    p_print_str(v20);
    goto bb3;
  bb3:
    return 0;
    return 0;
}

bool Option_is_none(void* v0, void* v1) {
    int64_t v13;
    int64_t v15; /* placeholder for void */
    int64_t v8;
    int64_t v17; /* placeholder for void */
    int64_t v3;
    bool v16;
    void* v7;
    bool v9;
    bool v14;
    int64_t v10; /* placeholder for void */
    void* v2;
    int64_t v6; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    bool v19;
    bool v5;
    int64_t v4;
    int64_t v12;
    int64_t v20; /* placeholder for void */
    void* v11;
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

int64_t Option_unwrap_or(void* v0, void* v1, int64_t v2) {
    int64_t v10; /* placeholder for void */
    int64_t v9;
    int64_t v12;
    bool v14;
    int64_t v15; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v18;
    int64_t v4;
    int64_t v5;
    int64_t v19; /* placeholder for void */
    bool v6;
    void* v8;
    void* v3;
    int64_t v13;
    void* v11;
    int64_t v17; /* placeholder for void */
    int64_t v7; /* placeholder for void */
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

int64_t clamp(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    bool v8;
    int64_t v6; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    bool v4;
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

void println(void* v0, void* v1) {
    int64_t v2; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    void* v3;
    int64_t v5; /* placeholder for void */
    p_print_str(v1);
    v3 = _str5;
    p_print_str(v3);
    return;
    return;
}

int64_t allocator_alloc(void* v0, void* v1, int64_t v2) {
    int64_t v4; /* placeholder for void */
    int64_t v3;
    v3 = (int64_t)p_allocator_alloc(v1, v2);
    return v3;
    return 0;
}

void allocator_free(void* v0, void* v1, int64_t v2) {
    int64_t v3; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    p_allocator_free(v1, v2);
    return;
    return;
}

int64_t Option_unwrap(void* v0, void* v1) {
    int64_t v4;
    int64_t v12;
    int64_t v14; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v2;
    void* v10;
    int64_t v20; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v11;
    int64_t v16; /* placeholder for void */
    int64_t v8;
    bool v13;
    bool v5;
    int64_t v6; /* placeholder for void */
    void* v7;
    void* v15;
    int64_t v19;
    int64_t v3;
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
    v15 = _str6;
    p_panic(v15);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

void Result_err(void* v0, void* v1, void* v2) {
    void* v14;
    void* v36;
    void* v3;
    int64_t v13; /* placeholder for void */
    int64_t v31;
    int64_t v38;
    int64_t v5;
    int64_t v17; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    void* v40;
    int64_t v24;
    int64_t v21; /* placeholder for void */
    int64_t v11;
    int64_t v25;
    int64_t v29;
    int64_t v27; /* placeholder for void */
    void* v34;
    int64_t v20;
    int64_t v44; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v30;
    void* v8;
    void* v32;
    int64_t v16;
    void* v23;
    void* v15;
    void* v10;
    int64_t v33; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    void* v41;
    void* v12;
    int64_t v39; /* placeholder for void */
    int64_t v4;
    void* v37;
    void* v19;
    int64_t v45; /* placeholder for void */
    bool v6;
    void* v28;
    void* v18;
    bool v26;
    int64_t v9;
    int64_t v22; /* placeholder for void */
    int64_t v42;
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

bool Result_is_err(void* v0, void* v1) {
    int64_t v12;
    bool v18;
    int64_t v19; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v4;
    bool v9;
    void* v7;
    int64_t v8;
    bool v5;
    int64_t v13;
    bool v14;
    void* v11;
    int64_t v15; /* placeholder for void */
    int64_t v17;
    int64_t v22; /* placeholder for void */
    int64_t v3;
    int64_t v6; /* placeholder for void */
    void* v2;
    void* v16;
    bool v21;
    int64_t v10; /* placeholder for void */
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

int64_t p_max(void* v0, int64_t v1, int64_t v2) {
    int64_t v7; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    bool v3;
    int64_t v4; /* placeholder for void */
    int64_t v6; /* placeholder for void */
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

void Result_ok(void* v0, void* v1, void* v2) {
    int64_t v27;
    int64_t v4;
    int64_t v9;
    int64_t v22;
    int64_t v33;
    int64_t v26;
    void* v34;
    int64_t v46; /* placeholder for void */
    void* v16;
    int64_t v18;
    bool v6;
    int64_t v24; /* placeholder for void */
    int64_t v38;
    void* v10;
    void* v12;
    int64_t v7; /* placeholder for void */
    void* v32;
    int64_t v15; /* placeholder for void */
    void* v20;
    int64_t v13; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    void* v30;
    int64_t v23; /* placeholder for void */
    void* v17;
    int64_t v31;
    int64_t v35; /* placeholder for void */
    void* v40;
    void* v14;
    void* v41;
    void* v21;
    void* v37;
    int64_t v42;
    int64_t v44; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v11;
    void* v25;
    void* v36;
    void* v8;
    int64_t v5;
    bool v28;
    int64_t v29; /* placeholder for void */
    void* v3;
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

int64_t p_min(void* v0, int64_t v1, int64_t v2) {
    int64_t v7; /* placeholder for void */
    bool v3;
    int64_t v5; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v4; /* placeholder for void */
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

bool Result_is_ok(void* v0, void* v1) {
    int64_t v19; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    bool v18;
    bool v21;
    int64_t v10; /* placeholder for void */
    int64_t v3;
    int64_t v4;
    void* v7;
    bool v5;
    int64_t v13;
    int64_t v15; /* placeholder for void */
    void* v11;
    int64_t v8;
    bool v14;
    void* v2;
    void* v16;
    int64_t v17;
    bool v9;
    int64_t v22; /* placeholder for void */
    int64_t v12;
    int64_t v6; /* placeholder for void */
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

// ─── C entry point ───
int main(int argc, char** argv) {
    __p_argc = argc; __p_argv = argv;
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

void p_vec_set(PVec* v, int64_t idx, void* val) {{
    if (!v || idx < 0 || idx >= v->len) return;
    v->data[idx] = val;
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
char* p_file_read(const char* path) {{
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    char* buf = (char*)malloc((size_t)sz + 1);
    if (!buf) {{ fclose(f); return NULL; }}
    size_t rd = fread(buf, 1, (size_t)sz, f); buf[rd] = 0; fclose(f);
    return buf;
}}
int64_t p_file_write(const char* path, const char* content) {{
    FILE* f = fopen(path, "wb");
    if (!f) return -1;
    fputs(content, f); fclose(f); return 0;
}}
bool p_file_exists(const char* path) {{ FILE* f = fopen(path, "rb"); if (f) {{ fclose(f); return true; }} return false; }}
int64_t p_file_append(const char* path, const char* content) {{
    FILE* f = fopen(path, "ab");
    if (!f) return -1;
    fputs(content, f); fclose(f); return 0;
}}
char* p_getenv(const char* name) {{ return getenv(name); }}
static int __p_argc = 0;
static char** __p_argv = NULL;
int64_t p_argc(void) {{ return (int64_t)__p_argc; }}
char* p_argv(int64_t index) {{ if (index < 0 || index >= __p_argc) return NULL; return __p_argv[index]; }}
int64_t p_system(const char* cmd) {{ return (int64_t)system(cmd); }}
void p_exit(int64_t code) {{ exit((int)code); }}
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
    if (!m) return strdup("{}");
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
char* p_float_to_str(double val) {{ return p_to_str_float(val); }}
char* p_bool_to_str(bool val) {{ return p_to_str_bool(val); }}
char* p_vec_to_str(PVec* v) {{ return p_to_str_vec(v); }}

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

int64_t p_socket_write_str(int64_t fd, const char* buf) {
    return p_socket_write(fd, buf, strlen(buf));
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

// Bare metal stubs — no-ops in C codegen (use --bare for real hardware)
void p_cli(void) {{}}
void p_sti(void) {{}}
void p_hlt(void) {{}}
int64_t p_cr0_read(void) {{ return 0; }}
int64_t p_cr3_read(void) {{ return 0; }}
int64_t p_cr4_read(void) {{ return 0; }}
void p_cr3_write(int64_t val) {{ (void)val; }}
void p_invlpg(int64_t addr) {{ (void)addr; }}
void p_page_map(int64_t cr3, int64_t virt, int64_t phys, int64_t flags) {{ (void)cr3; (void)virt; (void)phys; (void)flags; }}
void p_page_unmap(int64_t cr3, int64_t virt) {{ (void)cr3; (void)virt; }}
static int64_t _phys_mem[65536];
int64_t p_phys_read64(int64_t addr) {{ return _phys_mem[(addr/8) & 65535]; }}
void p_phys_write64(int64_t addr, int64_t val) {{ _phys_mem[(addr/8) & 65535] = val; }}

// SIMD builtins — operate on PVec (Vec<f64>)
// Helper: read double from PVec element (stored as void* via type punning)
static double _pvec_get_f64(PVec* v, int i) {
    if (!v || i < 0 || i >= v->len) return 0;
    union { void* p; double d; } u; u.p = v->data[i]; return u.d;
}
static void _pvec_push_f64(PVec* v, double d) {
    union { void* p; double d; } u; u.d = d; p_vec_push(v, u.p);
}
void* p_simd_add(void* a, void* b) {{
    PVec* va = (PVec*)a; PVec* vb = (PVec*)b; PVec* r = p_vec_new();
    int n = va ? va->len : 0;
    for (int i = 0; i < n; i++) _pvec_push_f64(r, _pvec_get_f64(va,i) + _pvec_get_f64(vb,i));
    return r;
}}
void* p_simd_sub(void* a, void* b) {{
    PVec* va = (PVec*)a; PVec* vb = (PVec*)b; PVec* r = p_vec_new();
    int n = va ? va->len : 0;
    for (int i = 0; i < n; i++) _pvec_push_f64(r, _pvec_get_f64(va,i) - _pvec_get_f64(vb,i));
    return r;
}}
void* p_simd_mul(void* a, void* b) {{
    PVec* va = (PVec*)a; PVec* vb = (PVec*)b; PVec* r = p_vec_new();
    int n = va ? va->len : 0;
    for (int i = 0; i < n; i++) _pvec_push_f64(r, _pvec_get_f64(va,i) * _pvec_get_f64(vb,i));
    return r;
}}
void* p_simd_div(void* a, void* b) {{
    PVec* va = (PVec*)a; PVec* vb = (PVec*)b; PVec* r = p_vec_new();
    int n = va ? va->len : 0;
    for (int i = 0; i < n; i++) {{ double db = _pvec_get_f64(vb,i); _pvec_push_f64(r, db != 0 ? _pvec_get_f64(va,i) / db : 0); }}
    return r;
}}
double p_simd_dot(void* a, void* b) {{
    PVec* va = (PVec*)a; PVec* vb = (PVec*)b;
    int n = va ? va->len : 0; double s = 0;
    for (int i = 0; i < n; i++) s += _pvec_get_f64(va,i) * _pvec_get_f64(vb,i);
    return s;
}}
double p_simd_sum(void* a) {{
    PVec* va = (PVec*)a;
    int n = va ? va->len : 0; double s = 0;
    for (int i = 0; i < n; i++) s += _pvec_get_f64(va,i);
    return s;
}}
void* p_simd_scale(void* a, double s) {{
    PVec* va = (PVec*)a; PVec* r = p_vec_new();
    int n = va ? va->len : 0;
    for (int i = 0; i < n; i++) _pvec_push_f64(r, _pvec_get_f64(va,i) * s);
    return r;
}}
void* p_simd_from_vec(void* v) {{ (void)v; return p_vec_new(); }}
char* p_simd_to_str(void* a) {{
    PVec* va = (PVec*)a;
    if (!va) return strdup("[]");
    size_t cap = 64; char* result = (char*)malloc(cap); size_t pos = 0;
    result[pos++] = '[';
    for (int64_t i = 0; i < va->len; i++) {{
        if (i > 0) {{ if (pos+2>=cap){{cap*=2;result=(char*)realloc(result,cap);}} result[pos++]=','; result[pos++]=' '; }}
        double d = _pvec_get_f64(va, i);
        char buf[32];
        if (d == (int64_t)d) snprintf(buf,sizeof(buf),"%lld",(long long)d);
        else snprintf(buf,sizeof(buf),"%g",d);
        size_t bl=strlen(buf);
        if (pos+bl+1>=cap){{cap=(cap+bl)*2;result=(char*)realloc(result,cap);}}
        memcpy(result+pos,buf,bl); pos+=bl;
    }}
    if (pos+2>=cap){{cap+=2;result=(char*)realloc(result,cap);}}
    result[pos++]=']'; result[pos]=0;
    return result;
}}

// Tensor builtins — simple flat array with shape metadata
typedef struct { double* data; int64_t len; int64_t ndim; int64_t shape[8]; } PTensor;
void* p_tensor_new(int64_t ndim, int64_t* shape) {{
    PTensor* t = (PTensor*)malloc(sizeof(PTensor));
    t->ndim = ndim;
    t->len = 1;
    for (int64_t i = 0; i < ndim && i < 8; i++) {{ t->shape[i] = shape[i]; t->len *= shape[i]; }}
    t->data = (double*)calloc(t->len, sizeof(double));
    return t;
}}
void* p_tensor_from(void* data, int64_t ndim, int64_t* shape) {{
    (void)data;
    return p_tensor_new(ndim, shape);
}}
double p_tensor_at(void* t, int64_t idx) {{
    PTensor* pt = (PTensor*)t;
    if (!pt || idx < 0 || idx >= pt->len) return 0;
    return pt->data[idx];
}}
void* p_tensor_add(void* a, void* b) {{
    PTensor* ta = (PTensor*)a; PTensor* tb = (PTensor*)b;
    if (!ta || !tb) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = ta->ndim; r->len = ta->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = ta->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = ta->data[i] + tb->data[i];
    return r;
}}
void* p_tensor_mul(void* a, void* b) {{
    PTensor* ta = (PTensor*)a; PTensor* tb = (PTensor*)b;
    if (!ta || !tb) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = ta->ndim; r->len = ta->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = ta->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = ta->data[i] * tb->data[i];
    return r;
}}
void* p_tensor_matmul(void* a, void* b) {{
    PTensor* ta = (PTensor*)a; PTensor* tb = (PTensor*)b;
    if (!ta || !tb || ta->ndim < 2 || tb->ndim < 2) return NULL;
    int64_t M = ta->shape[0], K = ta->shape[1], N = tb->shape[1];
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = 2; r->len = M * N; r->shape[0] = M; r->shape[1] = N;
    r->data = (double*)calloc(r->len, sizeof(double));
    for (int64_t i = 0; i < M; i++)
        for (int64_t j = 0; j < N; j++)
            for (int64_t k = 0; k < K; k++)
                r->data[i*N+j] += ta->data[i*K+k] * tb->data[k*N+j];
    return r;
}}
void* p_tensor_transpose(void* t) {{
    PTensor* pt = (PTensor*)t;
    if (!pt || pt->ndim < 2) return t;
    int64_t M = pt->shape[0], N = pt->shape[1];
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = 2; r->len = M * N; r->shape[0] = N; r->shape[1] = M;
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < M; i++)
        for (int64_t j = 0; j < N; j++)
            r->data[j*M+i] = pt->data[i*N+j];
    return r;
}}
void* p_tensor_relu(void* t) {{
    PTensor* pt = (PTensor*)t;
    if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = pt->data[i] > 0 ? pt->data[i] : 0;
    return r;
}}
void* p_tensor_softmax(void* t) {{
    PTensor* pt = (PTensor*)t;
    if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    double max = pt->data[0];
    for (int64_t i = 1; i < pt->len; i++) if (pt->data[i] > max) max = pt->data[i];
    double sum = 0;
    for (int64_t i = 0; i < pt->len; i++) {{ r->data[i] = exp(pt->data[i] - max); sum += r->data[i]; }}
    for (int64_t i = 0; i < pt->len; i++) r->data[i] /= sum;
    return r;
}}
int64_t p_tensor_len(void* t) {{ PTensor* pt = (PTensor*)t; return pt ? pt->len : 0; }}
int64_t p_tensor_ndim(void* t) {{ PTensor* pt = (PTensor*)t; return pt ? pt->ndim : 0; }}
void* p_tensor_shape(void* t) {{
    PTensor* pt = (PTensor*)t;
    PVec* v = (PVec*)p_vec_new();
    if (pt) for (int64_t i = 0; i < pt->ndim; i++) {{ int64_t d = pt->shape[i]; p_vec_push(v, (void*)d); }}
    return v;
}}
int64_t p_tensor_size(void* t) {{ return p_tensor_len(t); }}
void* p_tensor_zeros(void* shape_vec) {{
    PVec* sv = (PVec*)shape_vec;
    if (!sv) return p_tensor_new(0, NULL);
    int64_t shape[8]; int64_t ndim = sv->len < 8 ? sv->len : 8;
    for (int64_t i = 0; i < ndim; i++) shape[i] = (int64_t)sv->data[i];
    return p_tensor_new(ndim, shape);
}}
void* p_tensor_ones(void* shape_vec) {{
    PVec* sv = (PVec*)shape_vec;
    if (!sv) return p_tensor_new(0, NULL);
    int64_t shape[8]; int64_t ndim = sv->len < 8 ? sv->len : 8;
    for (int64_t i = 0; i < ndim; i++) shape[i] = (int64_t)sv->data[i];
    PTensor* t = (PTensor*)p_tensor_new(ndim, shape);
    if (t) for (int64_t i = 0; i < t->len; i++) t->data[i] = 1.0;
    return t;
}}
void* p_tensor_filled(void* shape_vec, double val) {{
    PVec* sv = (PVec*)shape_vec;
    if (!sv) return p_tensor_new(0, NULL);
    int64_t shape[8]; int64_t ndim = sv->len < 8 ? sv->len : 8;
    for (int64_t i = 0; i < ndim; i++) shape[i] = (int64_t)sv->data[i];
    PTensor* t = (PTensor*)p_tensor_new(ndim, shape);
    if (t) for (int64_t i = 0; i < t->len; i++) t->data[i] = val;
    return t;
}}
void* p_tensor_from_vec(void* v, void* shape_vec) {{
    PVec* sv = (PVec*)shape_vec;
    if (!sv) return p_tensor_new(0, NULL);
    int64_t shape[8]; int64_t ndim = sv->len < 8 ? sv->len : 8;
    for (int64_t i = 0; i < ndim; i++) shape[i] = (int64_t)sv->data[i];
    PTensor* t = (PTensor*)p_tensor_new(ndim, shape);
    PVec* pv = (PVec*)v;
    if (t && pv) for (int64_t i = 0; i < t->len && i < pv->len; i++) t->data[i] = (double)(int64_t)pv->data[i];
    return t;
}}
void* p_tensor_clone(void* t) {{
    PTensor* pt = (PTensor*)t;
    if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = pt->data[i];
    return r;
}}
double p_tensor_get(void* t, void* idx_vec) {{
    PTensor* pt = (PTensor*)t;
    PVec* iv = (PVec*)idx_vec;
    if (!pt || !iv || iv->len == 0) return 0;
    int64_t idx = 0;
    for (int64_t i = 0; i < pt->ndim && i < (int64_t)iv->len; i++) {{
        int64_t dim = (int64_t)iv->data[i];
        int64_t stride = 1;
        for (int64_t j = i+1; j < pt->ndim; j++) stride *= pt->shape[j];
        idx += dim * stride;
    }}
    if (idx < 0 || idx >= pt->len) return 0;
    return pt->data[idx];
}}
void p_tensor_set(void* t, void* idx_vec, double val) {{
    PTensor* pt = (PTensor*)t;
    PVec* iv = (PVec*)idx_vec;
    if (!pt || !iv || iv->len == 0) return;
    int64_t idx = 0;
    for (int64_t i = 0; i < pt->ndim && i < (int64_t)iv->len; i++) {{
        int64_t dim = (int64_t)iv->data[i];
        int64_t stride = 1;
        for (int64_t j = i+1; j < pt->ndim; j++) stride *= pt->shape[j];
        idx += dim * stride;
    }}
    if (idx < 0 || idx >= pt->len) return;
    pt->data[idx] = val;
}}
void* p_tensor_sub(void* a, void* b) {{
    PTensor* ta = (PTensor*)a; PTensor* tb = (PTensor*)b;
    if (!ta || !tb) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = ta->ndim; r->len = ta->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = ta->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = ta->data[i] - tb->data[i];
    return r;
}}
void* p_tensor_sigmoid(void* t) {{
    PTensor* pt = (PTensor*)t;
    if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = 1.0 / (1.0 + exp(-pt->data[i]));
    return r;
}}
void* p_tensor_scale(void* t, double s) {{
    PTensor* pt = (PTensor*)t;
    if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = pt->data[i] * s;
    return r;
}}
double p_tensor_sum(void* t) {{ PTensor* pt = (PTensor*)t; if (!pt) return 0; double s=0; for (int64_t i=0;i<pt->len;i++) s+=pt->data[i]; return s; }}
double p_tensor_mean(void* t) {{ PTensor* pt = (PTensor*)t; if (!pt || pt->len==0) return 0; return p_tensor_sum(t)/pt->len; }}
double p_tensor_min(void* t) {{ PTensor* pt = (PTensor*)t; if (!pt || pt->len==0) return 0; double m=pt->data[0]; for (int64_t i=1;i<pt->len;i++) if (pt->data[i]<m) m=pt->data[i]; return m; }}
double p_tensor_max(void* t) {{ PTensor* pt = (PTensor*)t; if (!pt || pt->len==0) return 0; double m=pt->data[0]; for (int64_t i=1;i<pt->len;i++) if (pt->data[i]>m) m=pt->data[i]; return m; }}
double p_tensor_dot(void* a, void* b) {{ PTensor* ta=(PTensor*)a; PTensor* tb=(PTensor*)b; if (!ta||!tb) return 0; double s=0; for (int64_t i=0;i<ta->len&&i<tb->len;i++) s+=ta->data[i]*tb->data[i]; return s; }}
void* p_tensor_reshape(void* t, void* shape_vec) {{
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    PVec* sv = (PVec*)shape_vec;
    PTensor* r = (PTensor*)p_tensor_clone(t);
    if (sv) {{
        r->ndim = sv->len < 8 ? sv->len : 8; r->len = 1;
        for (int64_t i = 0; i < r->ndim; i++) {{ r->shape[i] = (int64_t)sv->data[i]; r->len *= r->shape[i]; }}
    }}
    return r;
}}
char* p_tensor_to_json(void* t) {{
    PTensor* pt = (PTensor*)t;
    if (!pt) return strdup("{}");
    size_t cap = 128; char* result = (char*)malloc(cap); size_t pos = 0;
    pos += snprintf(result+pos, cap-pos, "{\"shape\":[");
    for (int64_t i = 0; i < pt->ndim; i++) {{
        if (i > 0) pos += snprintf(result+pos, cap-pos, ",");
        pos += snprintf(result+pos, cap-pos, "%lld", (long long)pt->shape[i]);
    }}
    pos += snprintf(result+pos, cap-pos, "],\"data\":[");
    for (int64_t i = 0; i < pt->len; i++) {{
        if (i > 0) pos += snprintf(result+pos, cap-pos, ",");
        double d = pt->data[i];
        if (d == (int64_t)d && d < 1e15 && d > -1e15) pos += snprintf(result+pos, cap-pos, "%lld", (long long)d);
        else pos += snprintf(result+pos, cap-pos, "%g", d);
    }}
    pos += snprintf(result+pos, cap-pos, "]}");
    return result;
}}
char* p_tensor_to_str(void* t) {{
    PTensor* pt = (PTensor*)t;
    if (!pt) return strdup("[]");
    size_t cap = 128; char* result = (char*)malloc(cap); size_t pos = 0;
    if (pt->ndim <= 1) {{
        result[pos++] = '[';
        for (int64_t i = 0; i < pt->len; i++) {{
            if (i > 0) {{ if (pos+2>=cap){{cap*=2;result=(char*)realloc(result,cap);}} result[pos++]=','; result[pos++]=' '; }}
            char buf[32]; snprintf(buf,sizeof(buf),"%g",pt->data[i]); size_t bl=strlen(buf);
            if (pos+bl+1>=cap){{cap=(cap+bl)*2;result=(char*)realloc(result,cap);}}
            memcpy(result+pos,buf,bl); pos+=bl;
        }}
        if (pos+2>=cap){{cap+=2;result=(char*)realloc(result,cap);}}
        result[pos++]=']'; result[pos]=0;
        return result;
    }}
    // 2D tensor: print as nested brackets
    int64_t rows = pt->shape[0], cols = pt->ndim >= 2 ? pt->shape[1] : 1;
    result[pos++] = '[';
    for (int64_t r = 0; r < rows; r++) {{
        if (r > 0) {{ if (pos+2>=cap){{cap*=2;result=(char*)realloc(result,cap);}} result[pos++]=','; result[pos++]=' '; }}
        if (pos+1>=cap){{cap+=2;result=(char*)realloc(result,cap);}} result[pos++]='[';
        for (int64_t c = 0; c < cols; c++) {{
            if (c > 0) {{ if (pos+2>=cap){{cap*=2;result=(char*)realloc(result,cap);}} result[pos++]=','; result[pos++]=' '; }}
            char buf[32]; snprintf(buf,sizeof(buf),"%g",pt->data[r*cols+c]); size_t bl=strlen(buf);
            if (pos+bl+1>=cap){{cap=(cap+bl)*2;result=(char*)realloc(result,cap);}}
            memcpy(result+pos,buf,bl); pos+=bl;
        }}
        if (pos+1>=cap){{cap+=2;result=(char*)realloc(result,cap);}} result[pos++]=']';
    }}
    if (pos+2>=cap){{cap+=2;result=(char*)realloc(result,cap);}}
    result[pos++]=']'; result[pos]=0;
    return result;
}}

// JSON builtins — simple key-value store
typedef struct { char** keys; char** vals; int64_t len; int64_t cap; int64_t is_array; } PJson;
static char* _json_dump_obj(PJson* pj); // forward decl
void _json_append(PJson* j, const char* key, const char* val) {{
    if (j->len >= j->cap) {{ j->cap = j->cap == 0 ? 8 : j->cap * 2; j->keys = (char**)realloc(j->keys, j->cap * sizeof(char*)); j->vals = (char**)realloc(j->vals, j->cap * sizeof(char*)); }}
    j->keys[j->len] = key ? strdup(key) : strdup(""); j->vals[j->len] = val ? strdup(val) : strdup(""); j->len++;
}}
char* p_json_parse(const char* s) {{
    if (!s) return strdup("");
    // Simple JSON parser: extracts string and number values
    PJson* j = (PJson*)p_json_new_object();
    const char* p = s;
    while (*p) {{
        // Skip whitespace and structural chars
        while (*p && *p != '"' && *p != '-' && (*p < '0' || *p > '9') && *p != 't' && *p != 'f' && *p != 'n') p++;
        if (!*p) break;
        // Try to parse a key (string followed by colon)
        if (*p == '"' && p > s && p[-1] != ':') {{
            p++; const char* kstart = p;
            while (*p && *p != '"') p++;
            size_t klen = p - kstart;
            if (*p) p++;
            // Skip whitespace and colon
            while (*p && *p != ':') p++;
            if (*p) p++;
            while (*p == ' ' || *p == '\t') p++;
            // Parse value
            char* key = (char*)malloc(klen+1); memcpy(key, kstart, klen); key[klen] = 0;
            if (*p == '"') {{
                p++; const char* vstart = p;
                while (*p && *p != '"') p++;
                size_t vlen = p - vstart;
                if (*p) p++;
                char* val = (char*)malloc(vlen+1); memcpy(val, vstart, vlen); val[vlen] = 0;
                _json_append(j, key, val); free(key); free(val);
            }} else if (*p == 't') {{
                _json_append(j, key, "true"); free(key); p += 4;
            }} else if (*p == 'f') {{
                _json_append(j, key, "false"); free(key); p += 5;
            }} else if (*p == 'n') {{
                _json_append(j, key, "null"); free(key); p += 4;
            }} else {{
                const char* vstart = p;
                while (*p && *p != ',' && *p != '}' && *p != ']' && *p != ' ') p++;
                size_t vlen = p - vstart;
                char* val = (char*)malloc(vlen+1); memcpy(val, vstart, vlen); val[vlen] = 0;
                _json_append(j, key, val); free(key); free(val);
            }}
        }} else {{
            p++;
        }}
    }}
    char* result = _json_dump_obj(j);
    return result;
}}
void* p_json_new_object(void) {{
    PJson* j = (PJson*)malloc(sizeof(PJson));
    j->keys = NULL; j->vals = NULL; j->len = 0; j->cap = 0; j->is_array = 0;
    return j;
}}
void* p_json_new_array(void) {{
    PJson* j = (PJson*)p_json_new_object(); j->is_array = 1; return j;
}}
void p_json_set(void* j, const char* key, const char* val) {{ _json_append((PJson*)j, key, val); }}
void p_json_set_int(void* j, const char* key, int64_t val) {{ char buf[32]; snprintf(buf,sizeof(buf),"%lld",(long long)val); _json_append((PJson*)j, key, buf); }}
void p_json_set_float(void* j, const char* key, double val) {{ char buf[32]; snprintf(buf,sizeof(buf),"%g",val); _json_append((PJson*)j, key, buf); }}
void p_json_set_bool(void* j, const char* key, int64_t val) {{ _json_append((PJson*)j, key, val ? "true" : "false"); }}
void p_json_append(void* j, const char* val) {{ _json_append((PJson*)j, NULL, val); }}
void p_json_append_int(void* j, int64_t val) {{ char buf[32]; snprintf(buf,sizeof(buf),"%lld",(long long)val); _json_append((PJson*)j, NULL, buf); }}
void p_json_append_float(void* j, double val) {{ char buf[32]; snprintf(buf,sizeof(buf),"%g",val); _json_append((PJson*)j, NULL, buf); }}
// Internal: dump PJson object to string (sorted keys, no quotes on numbers/bools)
static char* _json_dump_obj(PJson* pj) {{
    if (!pj) return strdup("{}");
    // Sort keys (simple insertion sort) for objects
    if (!pj->is_array) {{
        for (int64_t i = 1; i < pj->len; i++) {{
            char* k = pj->keys[i]; char* v = pj->vals[i];
            int64_t j = i - 1;
            while (j >= 0 && strcmp(pj->keys[j], k) > 0) {{ pj->keys[j+1] = pj->keys[j]; pj->vals[j+1] = pj->vals[j]; j--; }}
            pj->keys[j+1] = k; pj->vals[j+1] = v;
        }}
    }}
    size_t cap = 64; char* result = (char*)malloc(cap); size_t pos = 0;
    result[pos++] = pj->is_array ? '[' : '{';
    for (int64_t i = 0; i < pj->len; i++) {{
        if (i > 0) {{ if (pos+1>=cap){{cap*=2;result=(char*)realloc(result,cap);}} result[pos++]=','; }}
        if (!pj->is_array) {{ size_t kl=strlen(pj->keys[i]); if (pos+kl+4>=cap){{cap=(cap+kl)*2;result=(char*)realloc(result,cap);}} result[pos++]='"'; memcpy(result+pos,pj->keys[i],kl); pos+=kl; result[pos++]='"'; result[pos++]=':'; }}
        // Don't quote numbers, booleans, null
        const char* v = pj->vals[i];
        bool is_numeric = (*v >= '0' && *v <= '9') || *v == '-';
        bool is_bool = strcmp(v, "true") == 0 || strcmp(v, "false") == 0;
        bool is_null = strcmp(v, "null") == 0;
        bool need_quotes = !is_numeric && !is_bool && !is_null;
        size_t vl=strlen(v); if (pos+vl+3>=cap){{cap=(cap+vl)*2;result=(char*)realloc(result,cap);}}
        if (need_quotes) {{ result[pos++]='"'; memcpy(result+pos,v,vl); pos+=vl; result[pos++]='"'; }}
        else {{ memcpy(result+pos,v,vl); pos+=vl; }}
    }}
    if (pos+2>=cap){{cap+=2;result=(char*)realloc(result,cap);}}
    result[pos++] = pj->is_array ? ']' : '}'; result[pos] = 0;
    return result;
}}
// User-facing: parse JSON string and re-dump in canonical form
char* p_json_dump(void* j) {{
    return p_json_parse((const char*)j);
}}
char* p_json_dump_pretty(void* j) {{ return p_json_dump(j); }}
char* p_json_pretty(void* j) {{
    // Pretty-print with 2-space indentation (canonicalize first)
    char* canonical = p_json_parse((const char*)j);
    const char* s = canonical;
    if (!s) return strdup("");
    size_t cap = strlen(s) * 4 + 64; char* result = (char*)malloc(cap); size_t pos = 0;
    int indent = 0; bool in_str = false;
    while (*s) {{
        if (*s == '"' && (s == (const char*)j || s[-1] != '\\')) in_str = !in_str;
        if (!in_str) {{
            if (*s == '{' || *s == '[') {{
                result[pos++] = *s++; result[pos++] = '\n';
                indent++;
                for (int i = 0; i < indent*2; i++) result[pos++] = ' ';
                continue;
            }}
            if (*s == '}' || *s == ']') {{
                result[pos++] = '\n'; indent--;
                for (int i = 0; i < indent*2; i++) result[pos++] = ' ';
                result[pos++] = *s++;
                continue;
            }}
            if (*s == ',') {{
                result[pos++] = *s++; result[pos++] = '\n';
                for (int i = 0; i < indent*2; i++) result[pos++] = ' ';
                continue;
            }}
            if (*s == ':') {{ result[pos++] = *s++; result[pos++] = ' '; continue; }}
        }}
        if (pos >= cap-1) {{ cap *= 2; result = (char*)realloc(result, cap); }}
        result[pos++] = *s++;
    }}
    result[pos] = 0;
    return result;
}}
char* p_json_string(void* j) {{
    // Serialize a plain string as a JSON string literal (with quotes + escaping)
    const char* s = (const char*)j;
    if (!s) return strdup("\"\"");
    size_t cap = strlen(s) * 2 + 4; char* result = (char*)malloc(cap); size_t pos = 0;
    result[pos++] = '"';
    while (*s) {{
        if (*s == '"' || *s == '\\') {{ if (pos >= cap-2) {{ cap *= 2; result = (char*)realloc(result, cap); }} result[pos++] = '\\'; }}
        if (*s == '\n') {{ if (pos >= cap-2) {{ cap *= 2; result = (char*)realloc(result, cap); }} result[pos++] = '\\'; result[pos++] = 'n'; s++; continue; }}
        if (pos >= cap-1) {{ cap *= 2; result = (char*)realloc(result, cap); }}
        result[pos++] = *s++;
    }}
    result[pos++] = '"'; result[pos] = 0;
    return result;
}}
const char* p_json_type(void* j) {{
    const char* s = (const char*)j;
    if (!s) return "null";
    while (*s == ' ') s++;
    if (*s == '{') return "object";
    if (*s == '[') return "array";
    if (*s == '"') return "string";
    if (*s == 't' || *s == 'f') return "boolean";
    if (*s == 'n') return "null";
    return "number";
}}
char* p_json_get(void* j, const char* key) {{
    const char* s = (const char*)j;
    if (!s || !key) return strdup("null");
    char pattern[256]; snprintf(pattern, sizeof(pattern), "\"%s\":", key);
    const char* found = strstr(s, pattern);
    if (!found) return strdup("null");
    found += strlen(pattern);
    while (*found == ' ') found++;
    // Return the raw JSON value (with quotes for strings, as json::dump would)
    if (*found == '"') {{
        // String value — return with quotes (handle escaped quotes)
        const char* end = found + 1;
        while (*end) {{ if (*end == '\\' && end[1]) end += 2; else if (*end == '"') break; else end++; }}
        size_t len = end - found + 1; // include closing quote
        char* result = (char*)malloc(len+1); memcpy(result, found, len); result[len] = 0;
        return result;
    }}
    if (*found == '{' || *found == '[') {{
        // Nested object/array — find matching close brace
        char open = *found; char close = open == '{' ? '}' : ']';
        int depth = 1; const char* end = found + 1;
        while (*end && depth > 0) {{
            if (*end == open) depth++;
            if (*end == close) depth--;
            if (depth > 0) end++;
        }}
        size_t len = end - found + 1;
        char* result = (char*)malloc(len+1); memcpy(result, found, len); result[len] = 0;
        // Canonicalize by parsing and re-dumping
        char* canonical = p_json_parse(result); free(result);
        return canonical;
    }}
    // Number, boolean, or null
    const char* end = found;
    while (*end && *end != ',' && *end != '}' && *end != ']' && *end != ' ') end++;
    size_t len = end - found;
    char* result = (char*)malloc(len+1); memcpy(result, found, len); result[len] = 0;
    return result;
}}
int64_t p_json_get_int(void* j, const char* key) {{ char* v = p_json_get(j, key); int64_t r = atoll(v); free(v); return r; }}
double p_json_get_float(void* j, const char* key) {{ char* v = p_json_get(j, key); double r = atof(v); free(v); return r; }}
int64_t p_json_get_bool(void* j, const char* key) {{ char* v = p_json_get(j, key); int64_t r = strcmp(v, "true") == 0; free(v); return r; }}
void* p_json_at(void* j, int64_t idx) {{
    const char* s = (const char*)j;
    if (!s || idx < 0) return strdup("null");
    // Find array start
    while (*s && *s != '[') s++;
    if (*s != '[') return strdup("null");
    s++; // skip '['
    int64_t cur = 0;
    while (*s) {{
        while (*s == ' ' || *s == '\t') s++;
        if (*s == ']' || !*s) return strdup("null");
        if (cur == idx) {{
            // Found the element — extract it
            if (*s == '"') {{
                const char* end = s + 1;
                while (*end && *end != '"') end++;
                size_t len = end - s + 1; // include closing quote
                char* result = (char*)malloc(len+1); memcpy(result, s, len); result[len] = 0;
                return result;
            }}
            const char* end = s;
            while (*end && *end != ',' && *end != ']') end++;
            size_t len = end - s;
            char* result = (char*)malloc(len+1); memcpy(result, s, len); result[len] = 0;
            return result;
        }}
        // Skip this element
        if (*s == '"') {{
            s++; while (*s && *s != '"') s++; if (*s) s++;
        }} else {{
            while (*s && *s != ',' && *s != ']') s++;
        }}
        if (*s == ',') s++;
        cur++;
    }}
    return strdup("null");
}}
int64_t p_json_len(void* j) {{
    const char* s = (const char*)j;
    if (!s) return 0;
    int64_t count = 0; int depth = 0; bool in_str = false;
    while (*s) {{
        if (*s == '"' && (s == (const char*)j || s[-1] != '\\')) in_str = !in_str;
        if (!in_str) {{
            if (*s == '{' || *s == '[') depth++;
            if (*s == '}' || *s == ']') depth--;
            if (*s == ',' && depth == 1) count++;
        }}
        s++;
    }}
    return count + 1;
}}
void* p_json_object_get(void* j, const char* key) {{ return p_json_get(j, key); }}

int64_t p_http_listen(int64_t port) {{ return p_socket_listen(port); }}
char* p_http_recv_request(int64_t fd) {{ return p_socket_recv_str(fd); }}
void p_http_send_response(int64_t fd, int64_t status, const char* body) {{
    char header[256];
    snprintf(header, sizeof(header), "HTTP/1.1 %lld OK\r\nContent-Type: text/plain\r\nContent-Length: %zu\r\n\r\n", (long long)status, strlen(body));
    p_socket_write(fd, header, strlen(header));
    p_socket_write(fd, body, strlen(body));
}}
char* p_http_build_response(int64_t status, const char* content_type, const char* body) {{
    (void)content_type;
    size_t blen = strlen(body);
    char* resp = (char*)malloc(blen + 256);
    snprintf(resp, blen+256, "HTTP/1.1 %lld OK\r\nContent-Length: %zu\r\n\r\n%s", (long long)status, blen, body);
    return resp;
}}
char* p_http_parse_request(const char* request) {{
    if (!request) return strdup("{}");
    // Parse request line: METHOD PATH HTTP/VERSION
    const char* p = request;
    const char* line_end = strstr(p, "\r\n");
    if (!line_end) line_end = p + strlen(p);
    // Extract method
    const char* sp1 = strchr(p, ' ');
    char method[16] = {{0}};
    if (sp1 && sp1 < line_end) {{ size_t ml = sp1 - p; if (ml > 15) ml = 15; memcpy(method, p, ml); }}
    // Extract path
    const char* path_start = sp1 ? sp1 + 1 : p;
    const char* sp2 = strchr(path_start, ' ');
    char path[1024] = {{0}};
    if (sp2 && sp2 < line_end) {{ size_t pl = sp2 - path_start; if (pl > 1023) pl = 1023; memcpy(path, path_start, pl); }}
    // Extract version
    const char* ver_start = sp2 ? sp2 + 1 : p;
    char version[16] = {{0}};
    if (ver_start < line_end) {{ size_t vl = line_end - ver_start; if (vl > 15) vl = 15; memcpy(version, ver_start, vl); }}
    // Strip HTTP/ prefix
    const char* v = version;
    if (strncmp(v, "HTTP/", 5) == 0) v += 5;
    // Parse headers until empty line
    p = line_end + 2; // skip \r\n
    size_t cap = 1024; char* result = (char*)malloc(cap); size_t pos = 0;
    pos += snprintf(result+pos, cap-pos, "{\"method\":\"%s\",\"path\":\"%s\",\"version\":\"%s\",\"headers\":{", method, path, v);
    int first_hdr = 1;
    while (*p) {{
        const char* next_line = strstr(p, "\r\n");
        if (!next_line) break;
        if (next_line == p) {{ p += 2; break; }} // empty line = end of headers
        // Find colon
        const char* colon = strchr(p, ':');
        if (colon && colon < next_line) {{
            size_t kl = colon - p;
            const char* val_start = colon + 1;
            while (*val_start == ' ') val_start++;
            size_t vl = next_line - val_start;
            if (!first_hdr) pos += snprintf(result+pos, cap-pos, ",");
            first_hdr = 0;
            if (pos + kl + vl + 10 >= cap) {{ cap = (cap + kl + vl) * 2; result = (char*)realloc(result, cap); }}
            result[pos++] = '"'; memcpy(result+pos, p, kl); pos += kl; result[pos++] = '"'; result[pos++] = ':'; result[pos++] = '"'; memcpy(result+pos, val_start, vl); pos += vl; result[pos++] = '"';
        }}
        p = next_line + 2;
    }}
    pos += snprintf(result+pos, cap-pos, "},\"body\":\"");
    // Extract body (everything after the empty line) and escape for JSON
    if (*p) {{
        while (*p) {{
            if (pos >= cap-2) {{ cap *= 2; result = (char*)realloc(result, cap); }}
            if (*p == '"') {{ result[pos++] = '\\'; result[pos++] = '"'; }}
            else if (*p == '\\') {{ result[pos++] = '\\'; result[pos++] = '\\'; }}
            else if (*p == '\n') {{ result[pos++] = '\\'; result[pos++] = 'n'; }}
            else if (*p == '\r') {{ result[pos++] = '\\'; result[pos++] = 'r'; }}
            else if (*p == '\t') {{ result[pos++] = '\\'; result[pos++] = 't'; }}
            else result[pos++] = *p;
            p++;
        }}
    }}
    pos += snprintf(result+pos, cap-pos, "\"}");
    return result;
}}

// DB builtins — in-memory key-value store
typedef struct { char** keys; char** vals; int64_t len; int64_t cap; } PDB;
void* p_db_connect(const char* conn_str) {{
    (void)conn_str;
    PDB* db = (PDB*)malloc(sizeof(PDB));
    db->keys = NULL; db->vals = NULL; db->len = 0; db->cap = 0;
    return db;
}}
void p_db_exec(void* db, const char* query) {{ (void)db; (void)query; }}
void* p_db_query(void* db, const char* query) {{ (void)query; return db; }}
char* p_db_get(void* db, const char* key) {{
    PDB* pdb = (PDB*)db;
    if (!pdb) return strdup("");
    for (int64_t i = 0; i < pdb->len; i++) if (strcmp(pdb->keys[i], key) == 0) return strdup(pdb->vals[i]);
    return strdup("");
}}
void p_db_set(void* db, const char* key, const char* val) {{
    PDB* pdb = (PDB*)db;
    if (!pdb) return;
    for (int64_t i = 0; i < pdb->len; i++) if (strcmp(pdb->keys[i], key) == 0) {{ free(pdb->vals[i]); pdb->vals[i] = strdup(val); return; }}
    if (pdb->len >= pdb->cap) {{ pdb->cap = pdb->cap == 0 ? 8 : pdb->cap * 2; pdb->keys = (char**)realloc(pdb->keys, pdb->cap * sizeof(char*)); pdb->vals = (char**)realloc(pdb->vals, pdb->cap * sizeof(char*)); }}
    pdb->keys[pdb->len] = strdup(key); pdb->vals[pdb->len] = strdup(val); pdb->len++;
}}
typedef struct { char** keys; char** vals; int64_t len; int64_t cap; } PMemDB;
void* p_db_mem_open(void) {{
    PMemDB* db = (PMemDB*)calloc(1, sizeof(PMemDB));
    return db;
}}
int64_t p_db_mem_exec(void* db, const char* query) {{
    PMemDB* mdb = (PMemDB*)db;
    if (!mdb || !query) return 0;
    const char* p = query;
    while (*p == ' ') p++;
    char cmd[16] = {{0}}; int ci = 0;
    while (*p && *p != ' ' && ci < 15) cmd[ci++] = tolower(*p++);
    while (*p == ' ') p++;
    const char* kstart = p;
    while (*p && *p != ' ') p++;
    size_t klen = p - kstart;
    while (*p == ' ') p++;
    const char* vstart = p;
    while (*p) p++;
    size_t vlen = p - vstart;
    if ((strcmp(cmd, "insert") == 0 || strcmp(cmd, "set") == 0) && klen > 0) {{
        if (mdb->len >= mdb->cap) {{ mdb->cap = mdb->cap ? mdb->cap*2 : 8; mdb->keys = (char**)realloc(mdb->keys, mdb->cap*sizeof(char*)); mdb->vals = (char**)realloc(mdb->vals, mdb->cap*sizeof(char*)); }}
        mdb->keys[mdb->len] = (char*)malloc(klen+1); memcpy(mdb->keys[mdb->len], kstart, klen); mdb->keys[mdb->len][klen] = 0;
        mdb->vals[mdb->len] = (char*)malloc(vlen+1); memcpy(mdb->vals[mdb->len], vstart, vlen); mdb->vals[mdb->len][vlen] = 0;
        mdb->len++;
        return 1;
    }}
    return 0;
}}
char* p_db_mem_query(void* db, const char* query) {{
    (void)query;
    PMemDB* mdb = (PMemDB*)db;
    if (!mdb) return strdup("[]");
    size_t cap = 64; char* result = (char*)malloc(cap); size_t pos = 0;
    result[pos++] = '[';
    for (int64_t i = 0; i < mdb->len; i++) {{
        if (i > 0) result[pos++] = ',';
        size_t kl = strlen(mdb->keys[i]);
        if (pos+kl+4 >= cap) {{ cap = (cap+kl)*2; result = (char*)realloc(result, cap); }}
        result[pos++] = '"'; memcpy(result+pos, mdb->keys[i], kl); pos += kl; result[pos++] = '"';
    }}
    result[pos++] = ']'; result[pos] = 0;
    return result;
}}
void p_db_mem_close(void* db) {{
    PMemDB* mdb = (PMemDB*)db;
    if (mdb) {{ for (int64_t i = 0; i < mdb->len; i++) {{ free(mdb->keys[i]); free(mdb->vals[i]); }} free(mdb->keys); free(mdb->vals); free(mdb); }}
}}
