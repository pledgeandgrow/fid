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
int64_t p_vec_capacity(PVec* v);
PMap* p_map_new(void);
void p_map_insert(PMap* m, const char* key, int64_t val);
int64_t p_map_get(PMap* m, const char* key);
bool p_map_contains(PMap* m, const char* key);
void p_map_remove(PMap* m, const char* key);
int64_t p_map_len(PMap* m);
bool p_map_is_empty(PMap* m);
void p_map_clear(PMap* m);
int64_t p_map_keys(PMap* m);
int64_t p_map_values(PMap* m);
int64_t __ecs_create_entity(void);
int64_t __ecs_entity_count(void);
int64_t __ecs_has(const char* comp_name, int64_t entity);
void* __ecs_get(const char* comp_name, int64_t entity);
int64_t __ecs_add(const char* comp_name, int64_t entity, void* value);
int64_t __ecs_remove(const char* comp_name, int64_t entity);
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
int64_t p_simd_add(int64_t a, int64_t b);
int64_t p_simd_sub(int64_t a, int64_t b);
int64_t p_simd_mul(int64_t a, int64_t b);
int64_t p_simd_div(int64_t a, int64_t b);
double p_simd_dot(int64_t a, int64_t b);
double p_simd_sum(int64_t a);
int64_t p_simd_scale(int64_t a, double s);
int64_t p_simd_from_vec(int64_t v);
char* p_simd_to_str(int64_t a);
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
int64_t p_string_split(const char* s, const char* delim);
int64_t p_string_parse_int(const char* s);
char* p_string_char_at(const char* s, int64_t idx);
char* p_string_substring(const char* s, int64_t start, int64_t end);
char* p_to_str(int64_t val);
char* p_char_to_str(int64_t val);
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
// Call stack for panic traces
static const char* p_call_stack[256];
static int p_call_stack_depth = 0;
void p_call_stack_push(const char* name) {{ if (p_call_stack_depth < 256) p_call_stack[p_call_stack_depth++] = name; }}
void p_call_stack_pop(void) {{ if (p_call_stack_depth > 0) p_call_stack_depth--; }}
void* p_malloc(int64_t size);
void p_free(void* ptr);
char* p_file_read(const char* path);
int64_t p_file_write(const char* path, const char* content);
bool p_file_exists(const char* path);
int64_t p_file_append(const char* path, const char* content);
int64_t p_file_delete(const char* path);
int64_t p_file_size(const char* path);
char* p_getenv(const char* name);
static int __p_argc = 0;
static char** __p_argv = NULL;
int64_t p_argc(void);
char* p_argv(int64_t index);
int64_t p_system(const char* cmd);
void p_exit(int64_t code);
bool p_regex_match(const char* s, const char* pattern);
int64_t p_future_ready(int64_t value);
int64_t p_future_poll(int64_t future);
int64_t p_thread_spawn(const char* func_name, int64_t num_args, ...);
int64_t p_executor_new(void);
int64_t p_executor_spawn(int64_t exec, const char* func_name, int64_t num_args, ...);
int64_t p_executor_block_on(int64_t exec, int64_t future);
int64_t p_task_poll(int64_t future);
bool p_task_is_ready(int64_t future);
int64_t p_executor_run_all(int64_t exec);
int64_t p_executor_poll_all(int64_t exec);
int64_t p_task_poll_nonblocking(int64_t future);
int64_t p_net_connect(const char* host, int64_t port);
int64_t p_net_read(int64_t sock, char* buf, int64_t len);
int64_t p_net_write(int64_t sock, const char* buf, int64_t len);
void p_net_close(int64_t sock);
int64_t p_fs_read_async(const char* path, char* buf, int64_t len);
int64_t p_shared_alloc(int64_t size);
int64_t p_atomic_load(int64_t addr);
void p_atomic_store(int64_t addr, int64_t val);
bool p_atomic_cas(int64_t addr, int64_t expected, int64_t new_val);
int64_t p_atomic_add(int64_t addr, int64_t val);
int64_t p_atomic_sub(int64_t addr, int64_t val);
int64_t p_rc_incr(int64_t rc_addr, int64_t val);
void p_rc_decr(int64_t rc_addr, int64_t val);
int64_t p_mutex_init(void);
void p_mutex_lock(int64_t m);
void p_mutex_unlock(int64_t m);
int64_t p_chan_create(int64_t capacity);
bool p_chan_send(int64_t ch, int64_t val);
int64_t p_chan_recv(int64_t ch);
int64_t p_chan_try_recv(int64_t ch);
void p_chan_close(int64_t ch);
bool p_chan_is_closed(int64_t ch);
void p_thread_register(const char* name, int64_t (*fn)());
int64_t p_dl_open(const char* path);
int64_t p_dl_sym(int64_t handle, const char* symbol);
void p_dl_close(int64_t handle);
int64_t p_dl_call(int64_t func_ptr, int64_t arg);
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
static const char _str0[] = "Vec index out of bounds";
static const char _str1[] = "[";
static const char _str2[] = "]";
static const char _str3[] = ", ";
static const char _str4[] = "\n";
static const char _str5[] = "called unwrap on None";
static const char _str6[] = "Vec remove index out of bounds";
static const char _str7[] = "Vec insert index out of bounds";
static const char _str8[] = "";
static const char _str9[] = "{";
static const char _str10[] = "}";
static const char _str11[] = ": ";
static const char _str12[] = "Vec is empty";
static const char _str13[] = "Assertion failed\n";
static const char _str14[] = "called unwrap on Err";

// ─── Struct definitions ───
struct Result_s {
    int64_t field_0;
    int64_t field_1;
};

struct ByteBuffer_s {
    void* field_0;
    int64_t field_1;
};

struct Option_s {
    int64_t field_0;
    int64_t field_1;
};

struct StringBuilder_s {
    void* field_0;
    int64_t field_1;
};

// ─── Function prototypes ───
void p_thread_register(const char* name, int64_t (*fn)());
int64_t Vec_len(void* v0, int64_t v1);
double p_max(void* v0, void* v1);
void* clone(void* v0, void* v1);
int64_t allocator_alloc(void* v0, void* v1, int64_t v2);
int64_t String_char_at(void* v0, int64_t v1, int64_t v2);
void ByteBuffer_new(void* v0, void* v1, int64_t v2);
bool Vec_contains(void* v0, int64_t v1, int64_t v2);
bool char_is_digit(void* v0, int64_t v1);
bool Option_is_none(void* v0, void* v1);
void* mul(void* v0, void* v1, void* v2);
void Vec_reverse(void* v0, int64_t v1);
int64_t str_parse_hex(void* v0, void* v1);
int64_t Vec_get(void* v0, int64_t v1, int64_t v2);
int64_t String_to_uppercase(void* v0, int64_t v1);
bool p_str_equal(void* v0, int64_t v1, int64_t v2);
double get(void* v0, void* v1, void* v2);
void Result_map(void* v0, void* v1, void* v2, void* v3);
bool str_contains(void* v0, void* v1, void* v2);
bool String_ends_with(void* v0, int64_t v1, int64_t v2);
bool str_equal(void* v0, void* v1, void* v2);
bool Result_is_ok(void* v0, void* v1);
int64_t char_to_upper(void* v0, int64_t v1);
void* char_to_str(void* v0, int64_t v1);
void* Vec_to_str(void* v0, int64_t v1);
void* reshape(void* v0, void* v1, void* v2);
void println(void* v0, void* v1);
void set(void* v0, void* v1, void* v2, double v3);
void Vec_truncate(void* v0, int64_t v1, int64_t v2);
int64_t String_to_lowercase(void* v0, int64_t v1);
int64_t String_rfind(void* v0, int64_t v1, int64_t v2);
bool char_is_whitespace(void* v0, int64_t v1);
int64_t String_len(void* v0, int64_t v1);
void Option_or_else(void* v0, void* v1, void* v2, void* v3);
int64_t ndim(void* v0, void* v1);
int64_t size(void* v0, void* v1);
int64_t Result_expect(void* v0, void* v1, void* v2);
int64_t Option_unwrap(void* v0, void* v1);
int64_t Vec_remove(void* v0, int64_t v1, int64_t v2);
void* str_bytes(void* v0, void* v1);
int64_t Option_unwrap_or(void* v0, void* v1, int64_t v2);
void ByteBuffer_write_bytes(void* v0, void* v1, int64_t v2, int64_t v3);
int64_t Vec_new(void* v0);
bool HashMap_contains(void* v0, int64_t v1, int64_t v2);
int64_t String_trim(void* v0, int64_t v1);
void* to_json(void* v0, void* v1);
int64_t String_byte_at(void* v0, int64_t v1, int64_t v2);
int64_t String_char_count(void* v0, int64_t v1);
int64_t String_split(void* v0, int64_t v1, int64_t v2);
void p_str_copy_n(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t StringBuilder_len(void* v0, void* v1);
void Vec_push(void* v0, int64_t v1, int64_t v2);
int64_t String_find(void* v0, int64_t v1, int64_t v2);
int64_t HashMap_get_or(void* v0, int64_t v1, int64_t v2, int64_t v3);
void* str_to_lower(void* v0, void* v1);
double p_min(void* v0, void* v1);
bool HashMap_is_empty(void* v0, int64_t v1);
void Vec_set(void* v0, int64_t v1, int64_t v2, int64_t v3);
void Vec_insert(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t HashMap_values(void* v0, int64_t v1);
void HashMap_remove(void* v0, int64_t v1, int64_t v2);
int64_t String_replace(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t p_abs(void* v0, int64_t v1);
int64_t p_str_compare(void* v0, int64_t v1, int64_t v2);
int64_t Vec_capacity(void* v0, int64_t v1);
int64_t char_to_lower(void* v0, int64_t v1);
void* to_str(void* v0, void* v1);
int64_t String_parse_int(void* v0, int64_t v1);
void* str_trim_end(void* v0, void* v1);
void ByteBuffer_write_byte(void* v0, void* v1, int64_t v2);
int64_t HashMap_len(void* v0, int64_t v1);
int64_t str_find(void* v0, void* v1, void* v2);
void Option_and_then(void* v0, void* v1, void* v2, void* v3);
int64_t String_push_str(void* v0, int64_t v1, int64_t v2);
int64_t String_to_string(void* v0, int64_t v1);
void* ones(void* v0, void* v1);
bool char_is_lower(void* v0, int64_t v1);
void HashMap_grow(void* v0, int64_t v1);
int64_t clamp(void* v0, int64_t v1, int64_t v2, int64_t v3);
void* str_replace(void* v0, void* v1, void* v2, void* v3);
int64_t Option_unwrap_or_else(void* v0, void* v1, void* v2);
int64_t str_rfind(void* v0, void* v1, void* v2);
bool Result_is_err(void* v0, void* v1);
int64_t Result_unwrap_or(void* v0, void* v1, int64_t v2);
void* str_split_char(void* v0, void* v1, int64_t v2);
void* ByteBuffer_to_str(void* v0, void* v1);
void ByteBuffer_clear(void* v0, void* v1);
void* shape(void* v0, void* v1);
int64_t HashMap_keys(void* v0, int64_t v1);
void StringBuilder_append_char(void* v0, void* v1, int64_t v2);
int64_t String_clone(void* v0, int64_t v1);
bool str_is_empty(void* v0, void* v1);
void* sigmoid(void* v0, void* v1);
void* HashMap_to_str(void* v0, int64_t v1);
void* filled(void* v0, void* v1, double v2);
void* int_to_str(void* v0, int64_t v1);
void* sub(void* v0, void* v1, void* v2);
bool Vec_is_empty(void* v0, int64_t v1);
void Option_map(void* v0, void* v1, void* v2, void* v3);
int64_t HashMap_new(void* v0);
int64_t HashMap_find_slot(void* v0, int64_t v1, int64_t v2);
void* scale(void* v0, void* v1, double v2);
void Result_ok(void* v0, void* v1, void* v2);
void Vec_clear(void* v0, int64_t v1);
int64_t p_str_length(void* v0, int64_t v1);
void StringBuilder_append_str(void* v0, void* v1, void* v2);
bool Option_is_some(void* v0, void* v1);
void* str_slice(void* v0, void* v1, int64_t v2, int64_t v3);
void HashMap_clear(void* v0, int64_t v1);
void* from_vec(void* v0, void* v1, void* v2);
bool String_starts_with(void* v0, int64_t v1, int64_t v2);
int64_t Vec_last(void* v0, int64_t v1);
void* relu(void* v0, void* v1);
void Vec_sort(void* v0, int64_t v1);
int64_t ByteBuffer_read_byte(void* v0, void* v1, int64_t v2);
int64_t String_bytes(void* v0, int64_t v1);
bool str_ends_with(void* v0, void* v1, void* v2);
int64_t str_parse_int(void* v0, void* v1);
void* create_allocator(void* v0, void* v1);
int64_t str_len(void* v0, void* v1);
void* transpose(void* v0, void* v1);
int64_t HashMap_clone(void* v0, int64_t v1);
void assert(void* v0, bool v1);
int64_t String_slice(void* v0, int64_t v1, int64_t v2, int64_t v3);
bool char_is_upper(void* v0, int64_t v1);
int64_t Vec_pop(void* v0, int64_t v1);
int64_t str_parse_int_range(void* v0, void* v1, int64_t v2, int64_t v3);
void Result_err(void* v0, void* v1, void* v2);
void HashMap_insert(void* v0, int64_t v1, int64_t v2, int64_t v3);
double mean(void* v0, void* v1);
int64_t main_entry(void* v0);
void* softmax(void* v0, void* v1);
int64_t Option_expect(void* v0, void* v1, void* v2);
void allocator_free(void* v0, void* v1, int64_t v2);
bool String_contains(void* v0, int64_t v1, int64_t v2);
void* add(void* v0, void* v1, void* v2);
int64_t HashMap_get(void* v0, int64_t v1, int64_t v2);
bool String_is_empty(void* v0, int64_t v1);
bool str_starts_with(void* v0, void* v1, void* v2);
bool char_is_alpha(void* v0, int64_t v1);
void* StringBuilder_to_str(void* v0, void* v1);
bool StringBuilder_is_empty(void* v0, void* v1);
int64_t str_compare(void* v0, void* v1, void* v2);
bool ByteBuffer_is_empty(void* v0, void* v1);
int64_t allocator_bytes_used(void* v0, void* v1);
void StringBuilder_clear(void* v0, void* v1);
int64_t Vec_first(void* v0, int64_t v1);
int64_t Vec_clone(void* v0, int64_t v1);
void* zeros(void* v0, void* v1);
void* matmul(void* v0, void* v1, void* v2);
int64_t p_str_hash(void* v0, int64_t v1);
int64_t String_from(void* v0, int64_t v1);
void* str_repeat(void* v0, void* v1, int64_t v2);
void Result_and_then(void* v0, void* v1, void* v2, void* v3);
void* str_concat(void* v0, void* v1, void* v2);
void* str_trim(void* v0, void* v1);
int64_t Result_unwrap_or_else(void* v0, void* v1, void* v2);
void Vec_extend(void* v0, int64_t v1, int64_t v2);
void ByteBuffer_from_string(void* v0, void* v1, void* v2);
void* str_from_bytes(void* v0, void* v1);
void ByteBuffer_set_byte(void* v0, void* v1, int64_t v2, int64_t v3);
void* str_trim_start(void* v0, void* v1);
void Result_map_err(void* v0, void* v1, void* v2, void* v3);
void* str_split(void* v0, void* v1, void* v2);
void* p_str_from_ptr(void* v0, int64_t v1);
void* str_to_upper(void* v0, void* v1);
int64_t str_byte_at(void* v0, void* v1, int64_t v2);
double dot(void* v0, void* v1, void* v2);
double sum(void* v0, void* v1);
int64_t String_repeat(void* v0, int64_t v1, int64_t v2);
void* byte_to_str(void* v0, int64_t v1);
int64_t Result_unwrap(void* v0, void* v1);
void StringBuilder_append_int(void* v0, void* v1, int64_t v2);
int64_t ByteBuffer_len(void* v0, void* v1);
void StringBuilder_new(void* v0, void* v1);

int64_t Vec_len(void* v0, int64_t v1) {
    p_call_stack_push("Vec_len");
    void* v4;
    int64_t v5; /* placeholder for void */
    void* v8;
    int64_t v9;
    int64_t v11; /* placeholder for void */
    int64_t v2;
    int64_t v7; /* placeholder for void */
    int64_t v12;
    void* v6;
    int64_t v3;
    int64_t v10; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    v2 = 8LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = malloc(8);
    v8 = *(void**)v4;
    v9 = *(int64_t*)v8;
    *(int64_t*)v6 = (int64_t)v9;
    v12 = *(int64_t*)v6;
    p_call_stack_pop();
    return v12;
    return 0;
}

double p_max(void* v0, void* v1) {
    p_call_stack_push("max");
    double v2;
    int64_t v3; /* placeholder for void */
    v2 = (double)p_tensor_max(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void* clone(void* v0, void* v1) {
    p_call_stack_push("clone");
    int64_t v3; /* placeholder for void */
    void* v2;
    v2 = p_tensor_clone(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t allocator_alloc(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("allocator_alloc");
    int64_t v4; /* placeholder for void */
    int64_t v3;
    v3 = (int64_t)p_allocator_alloc(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t String_char_at(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_char_at");
    int64_t v6;
    bool v4;
    int64_t v52; /* placeholder for void */
    int64_t v28;
    void* v23;
    int64_t v30; /* placeholder for void */
    void* v54;
    int64_t v3;
    int64_t v27; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v36;
    int64_t v49;
    int64_t v40;
    uint8_t v33;
    int64_t v58;
    int64_t v22;
    int64_t v47; /* placeholder for void */
    int64_t v19;
    int64_t v21; /* placeholder for void */
    bool v16;
    int64_t v43; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    void* v44;
    int64_t v8; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    void* v11;
    int64_t v39; /* placeholder for void */
    int64_t v13;
    int64_t v5; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v25;
    int64_t v48;
    int64_t v59; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v61;
    void* v29;
    bool v17;
    bool v14;
    void* v38;
    int64_t v45;
    void* v51;
    void* v26;
    int64_t v62; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v32;
    int64_t v46; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    void* v37;
    int64_t v35; /* placeholder for void */
    int64_t v15;
    int64_t v42; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    void* v41;
    int64_t v10;
    int64_t v53;
    void* v57;
    int64_t v50;
    int64_t v9;
    int64_t v12; /* placeholder for void */
    v3 = 0LL;
    v4 = v1 == v3;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    v6 = 0LL;
    p_call_stack_pop();
    return v6;
  bb2:
    goto bb3;
  bb3:
    v9 = 0LL;
    v10 = (int64_t)p_str_length((void*)v9, v1);
    v11 = malloc(8);
    *(int64_t*)v11 = (int64_t)v10;
    v13 = 0LL;
    v14 = v2 < v13;
    v15 = *(int64_t*)v11;
    v16 = v2 >= v15;
    v17 = v14 || v16;
    if (v17) goto bb4;
    else goto bb5;
  bb4:
    v19 = 0LL;
    p_call_stack_pop();
    return v19;
  bb5:
    goto bb6;
  bb6:
    v22 = v1 + v2;
    v23 = malloc(8);
    *(int64_t*)v23 = (int64_t)v22;
    v25 = *(int64_t*)v23;
    v26 = malloc(8);
    *(void**)v26 = (void*)v25;
    v28 = 0LL;
    v29 = malloc(8);
    *(int64_t*)v29 = (int64_t)v28;
    v32 = *(void**)v26;
    v33 = *(uint8_t*)v32;
    *(int64_t*)v29 = (int64_t)v33;
    v36 = 2LL;
    v37 = p_malloc(v36);
    v38 = malloc(8);
    *(int64_t*)v38 = (int64_t)v37;
    v40 = *(int64_t*)v38;
    v41 = malloc(8);
    *(void**)v41 = (void*)v40;
    v44 = *(void**)v41;
    v45 = *(int64_t*)v29;
    *(uint8_t*)v44 = (uint8_t)v45;
    v48 = *(int64_t*)v38;
    v49 = 1LL;
    v50 = v48 + v49;
    v51 = malloc(8);
    *(int64_t*)v51 = (int64_t)v50;
    v53 = *(int64_t*)v51;
    v54 = malloc(8);
    *(void**)v54 = (void*)v53;
    v57 = *(void**)v54;
    v58 = 0LL;
    *(uint8_t*)v57 = (uint8_t)v58;
    v61 = *(int64_t*)v38;
    p_call_stack_pop();
    return v61;
    return 0;
}

void ByteBuffer_new(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("ByteBuffer_new");
    void* v11;
    int64_t v18; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v13;
    int64_t v8;
    int64_t v10; /* placeholder for void */
    void* v3;
    int64_t v4;
    void* v9;
    void* v5;
    void* v16;
    void* v6;
    void* v15;
    int64_t v17;
    void* v12;
    int64_t v19; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    v3 = malloc(16);
    v4 = 0LL;
    v5 = (void*)Vec_new((void*)v4);
    v6 = (char*)v3 + 0;
    *(void**)v6 = (void*)v5;
    v8 = 0LL;
    v9 = (char*)v3 + 8;
    *(int64_t*)v9 = (int64_t)v8;
    v11 = (char*)v3 + 0;
    v12 = (char*)v0 + 0;
    v13 = *(void**)v11;
    *(void**)v12 = (void*)v13;
    v15 = (char*)v3 + 8;
    v16 = (char*)v0 + 8;
    v17 = *(int64_t*)v15;
    *(int64_t*)v16 = (int64_t)v17;
    p_call_stack_pop();
    return;
    return;
}

bool Vec_contains(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("Vec_contains");
    int64_t v32;
    int64_t v6; /* placeholder for void */
    int64_t v25;
    int64_t v35; /* placeholder for void */
    void* v7;
    int64_t v52; /* placeholder for void */
    bool v50;
    int64_t v10; /* placeholder for void */
    void* v43;
    int64_t v36;
    bool v48;
    int64_t v19; /* placeholder for void */
    void* v15;
    int64_t v42; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v30;
    int64_t v47;
    int64_t v54;
    int64_t v16; /* placeholder for void */
    int64_t v21;
    int64_t v59; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    bool v58;
    int64_t v33;
    void* v22;
    int64_t v8; /* placeholder for void */
    void* v9;
    void* v17;
    int64_t v26;
    int64_t v14; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v29;
    int64_t v3;
    void* v37;
    void* v5;
    int64_t v23; /* placeholder for void */
    int64_t v31;
    int64_t v45; /* placeholder for void */
    int64_t v55;
    int64_t v53;
    int64_t v51; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    void* v11;
    int64_t v20; /* placeholder for void */
    int64_t v44;
    int64_t v12;
    int64_t v4;
    bool v27;
    int64_t v49; /* placeholder for void */
    void* v34;
    int64_t v39;
    int64_t v18;
    void* v40;
    int64_t v13; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    v3 = 8LL;
    v4 = v1 + v3;
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = malloc(8);
    *(void**)v7 = (void*)v1;
    v9 = malloc(8);
    v11 = *(void**)v5;
    v12 = *(int64_t*)v11;
    *(int64_t*)v9 = (int64_t)v12;
    v15 = malloc(8);
    v17 = *(void**)v7;
    v18 = *(int64_t*)v17;
    *(int64_t*)v15 = (int64_t)v18;
    v21 = 0LL;
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v21;
    goto bb1;
  bb1:
    v25 = *(int64_t*)v22;
    v26 = *(int64_t*)v9;
    v27 = v25 < v26;
    if (v27) goto bb2;
    else goto bb3;
  bb2:
    v29 = *(int64_t*)v15;
    v30 = *(int64_t*)v22;
    v31 = 8LL;
    v32 = v30 * v31;
    v33 = v29 + v32;
    v34 = malloc(8);
    *(int64_t*)v34 = (int64_t)v33;
    v36 = *(int64_t*)v34;
    v37 = malloc(8);
    *(void**)v37 = (void*)v36;
    v39 = 0LL;
    v40 = malloc(8);
    *(int64_t*)v40 = (int64_t)v39;
    v43 = *(void**)v37;
    v44 = *(int64_t*)v43;
    *(int64_t*)v40 = (int64_t)v44;
    v47 = *(int64_t*)v40;
    v48 = v47 == v2;
    if (v48) goto bb4;
    else goto bb5;
  bb3:
    v58 = false;
    p_call_stack_pop();
    return v58;
  bb4:
    v50 = true;
    p_call_stack_pop();
    return v50;
  bb5:
    goto bb6;
  bb6:
    v53 = *(int64_t*)v22;
    v54 = 1LL;
    v55 = v53 + v54;
    *(int64_t*)v22 = (int64_t)v55;
    goto bb1;
    return 0;
}

bool char_is_digit(void* v0, int64_t v1) {
    p_call_stack_push("char_is_digit");
    int64_t v7; /* placeholder for void */
    int64_t v2;
    bool v6;
    bool v3;
    int64_t v4;
    bool v5;
    v2 = 48LL;
    v3 = v1 >= v2;
    v4 = 57LL;
    v5 = v1 <= v4;
    v6 = v3 && v5;
    p_call_stack_pop();
    return v6;
    return 0;
}

bool Option_is_none(void* v0, void* v1) {
    p_call_stack_push("Option_is_none");
    void* v7;
    int64_t v4;
    int64_t v20; /* placeholder for void */
    void* v11;
    int64_t v3;
    bool v14;
    int64_t v15; /* placeholder for void */
    bool v5;
    int64_t v8;
    int64_t v17; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v2;
    int64_t v6; /* placeholder for void */
    bool v9;
    int64_t v13;
    bool v16;
    bool v19;
    int64_t v12;
    int64_t v18; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v19 = false;
    p_call_stack_pop();
    return v19;
  bb2:
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    v9 = false;
    p_call_stack_pop();
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
    p_call_stack_pop();
    return v16;
  bb5:
    goto bb1;
    return 0;
}

void* mul(void* v0, void* v1, void* v2) {
    p_call_stack_push("mul");
    void* v3;
    int64_t v4; /* placeholder for void */
    v3 = p_tensor_mul(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

void Vec_reverse(void* v0, int64_t v1) {
    p_call_stack_push("Vec_reverse");
    int64_t v40;
    int64_t v7; /* placeholder for void */
    int64_t v24;
    int64_t v43;
    int64_t v20;
    void* v57;
    int64_t v36; /* placeholder for void */
    int64_t v32;
    int64_t v54; /* placeholder for void */
    void* v63;
    int64_t v75; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v41;
    int64_t v65; /* placeholder for void */
    void* v6;
    int64_t v26;
    int64_t v18; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    int64_t v25;
    int64_t v27;
    int64_t v58; /* placeholder for void */
    int64_t v2;
    void* v8;
    void* v4;
    int64_t v56;
    int64_t v71;
    int64_t v74; /* placeholder for void */
    int64_t v72;
    int64_t v15; /* placeholder for void */
    void* v10;
    void* v21;
    int64_t v22; /* placeholder for void */
    void* v35;
    void* v49;
    int64_t v39;
    int64_t v23; /* placeholder for void */
    void* v55;
    int64_t v5; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    void* v59;
    int64_t v31;
    int64_t v33;
    int64_t v38;
    bool v28;
    void* v61;
    int64_t v44;
    int64_t v19; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    int64_t v64;
    int64_t v69; /* placeholder for void */
    int64_t v17;
    int64_t v73; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v34;
    int64_t v37;
    void* v46;
    int64_t v13; /* placeholder for void */
    int64_t v60;
    int64_t v3;
    int64_t v70;
    int64_t v9; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v45;
    int64_t v11;
    void* v14;
    int64_t v42;
    void* v66;
    int64_t v51;
    void* v16;
    int64_t v67;
    void* v52;
    int64_t v30;
    int64_t v48;
    v2 = 8LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = malloc(8);
    *(void**)v6 = (void*)v1;
    v8 = malloc(8);
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v8 = (int64_t)v11;
    v14 = malloc(8);
    v16 = *(void**)v6;
    v17 = *(int64_t*)v16;
    *(int64_t*)v14 = (int64_t)v17;
    v20 = 0LL;
    v21 = malloc(8);
    *(int64_t*)v21 = (int64_t)v20;
    goto bb1;
  bb1:
    v24 = *(int64_t*)v21;
    v25 = *(int64_t*)v8;
    v26 = 2LL;
    v27 = v25 / v26;
    v28 = v24 < v27;
    if (v28) goto bb2;
    else goto bb3;
  bb2:
    v30 = *(int64_t*)v14;
    v31 = *(int64_t*)v21;
    v32 = 8LL;
    v33 = v31 * v32;
    v34 = v30 + v33;
    v35 = malloc(8);
    *(int64_t*)v35 = (int64_t)v34;
    v37 = *(int64_t*)v14;
    v38 = *(int64_t*)v8;
    v39 = 1LL;
    v40 = v38 - v39;
    v41 = *(int64_t*)v21;
    v42 = v40 - v41;
    v43 = 8LL;
    v44 = v42 * v43;
    v45 = v37 + v44;
    v46 = malloc(8);
    *(int64_t*)v46 = (int64_t)v45;
    v48 = *(int64_t*)v35;
    v49 = malloc(8);
    *(void**)v49 = (void*)v48;
    v51 = *(int64_t*)v46;
    v52 = malloc(8);
    *(void**)v52 = (void*)v51;
    v55 = *(void**)v49;
    v56 = *(int64_t*)v55;
    v57 = malloc(8);
    *(int64_t*)v57 = (int64_t)v56;
    v59 = *(void**)v52;
    v60 = *(int64_t*)v59;
    v61 = malloc(8);
    *(int64_t*)v61 = (int64_t)v60;
    v63 = *(void**)v49;
    v64 = *(int64_t*)v61;
    *(int64_t*)v63 = (int64_t)v64;
    v66 = *(void**)v52;
    v67 = *(int64_t*)v57;
    *(int64_t*)v66 = (int64_t)v67;
    v70 = *(int64_t*)v21;
    v71 = 1LL;
    v72 = v70 + v71;
    *(int64_t*)v21 = (int64_t)v72;
    goto bb1;
  bb3:
    p_call_stack_pop();
    return;
    return;
}

int64_t str_parse_hex(void* v0, void* v1) {
    p_call_stack_push("str_parse_hex");
    int64_t v190;
    int64_t v24;
    void* v112;
    int64_t v147; /* placeholder for void */
    int64_t v203;
    int64_t v77;
    int64_t v149;
    int64_t v176;
    bool v89;
    int64_t v232; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v204; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v153; /* placeholder for void */
    int64_t v209;
    int64_t v60; /* placeholder for void */
    void* v72;
    bool v141;
    void* v14;
    bool v146;
    int64_t v37; /* placeholder for void */
    bool v80;
    void* v132;
    int64_t v94; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v118; /* placeholder for void */
    int64_t v225;
    int64_t v189;
    uint8_t v173;
    int64_t v103; /* placeholder for void */
    int64_t v155; /* placeholder for void */
    uint8_t v133;
    bool v199;
    int64_t v150;
    int64_t v92;
    int64_t v171; /* placeholder for void */
    int64_t v152; /* placeholder for void */
    void* v35;
    int64_t v235;
    int64_t v213; /* placeholder for void */
    int64_t v2;
    bool v102;
    bool v186;
    int64_t v230;
    int64_t v22; /* placeholder for void */
    int64_t v42;
    int64_t v83;
    int64_t v34;
    int64_t v105;
    int64_t v12; /* placeholder for void */
    int64_t v124; /* placeholder for void */
    int64_t v238; /* placeholder for void */
    int64_t v222;
    int64_t v26; /* placeholder for void */
    int64_t v192; /* placeholder for void */
    void* v106;
    int64_t v180;
    bool v198;
    int64_t v175; /* placeholder for void */
    bool v138;
    int64_t v10;
    int64_t v158;
    uint8_t v73;
    int64_t v242; /* placeholder for void */
    int64_t v143;
    int64_t v59; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    int64_t v100;
    int64_t v200; /* placeholder for void */
    int64_t v229;
    int64_t v52; /* placeholder for void */
    void* v32;
    int64_t v206;
    int64_t v237;
    int64_t v191; /* placeholder for void */
    int64_t v170; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v54;
    int64_t v154; /* placeholder for void */
    bool v233;
    int64_t v187; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    int64_t v23;
    int64_t v168;
    int64_t v188;
    int64_t v131; /* placeholder for void */
    int64_t v165;
    int64_t v113; /* placeholder for void */
    int64_t v202;
    int64_t v160; /* placeholder for void */
    int64_t v107; /* placeholder for void */
    bool v212;
    int64_t v6;
    int64_t v224;
    int64_t v99;
    void* v69;
    int64_t v183;
    int64_t v90; /* placeholder for void */
    int64_t v194;
    int64_t v214;
    int64_t v110; /* placeholder for void */
    int64_t v125;
    int64_t v167; /* placeholder for void */
    int64_t v179; /* placeholder for void */
    int64_t v13;
    void* v63;
    int64_t v239; /* placeholder for void */
    int64_t v216;
    int64_t v104;
    int64_t v119;
    int64_t v45;
    int64_t v48; /* placeholder for void */
    int64_t v88;
    int64_t v184;
    int64_t v228;
    int64_t v86; /* placeholder for void */
    int64_t v87;
    int64_t v76;
    int64_t v139;
    bool v185;
    int64_t v219; /* placeholder for void */
    bool v8;
    int64_t v21; /* placeholder for void */
    bool v25;
    void* v20;
    int64_t v55;
    int64_t v64; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v3;
    void* v66;
    int64_t v227; /* placeholder for void */
    void* v169;
    int64_t v9; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v68;
    int64_t v174; /* placeholder for void */
    void* v163;
    bool v44;
    bool v159;
    int64_t v156; /* placeholder for void */
    int64_t v130; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    int64_t v210;
    int64_t v127; /* placeholder for void */
    int64_t v218; /* placeholder for void */
    int64_t v122;
    int64_t v49;
    void* v4;
    int64_t v28;
    void* v129;
    int64_t v134; /* placeholder for void */
    int64_t v137;
    int64_t v164; /* placeholder for void */
    int64_t v205; /* placeholder for void */
    int64_t v221; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v196;
    int64_t v53; /* placeholder for void */
    int64_t v117; /* placeholder for void */
    int64_t v85; /* placeholder for void */
    int64_t v98;
    int64_t v121;
    int64_t v111;
    void* v166;
    int64_t v128;
    int64_t v43;
    int64_t v161;
    int64_t v71; /* placeholder for void */
    uint8_t v39;
    int64_t v157;
    int64_t v16;
    int64_t v84;
    bool v195;
    int64_t v162;
    int64_t v181;
    int64_t v136;
    int64_t v135; /* placeholder for void */
    int64_t v217; /* placeholder for void */
    bool v19;
    int64_t v240; /* placeholder for void */
    int64_t v108;
    bool v211;
    bool v56;
    int64_t v148;
    int64_t v27;
    int64_t v223;
    bool v208;
    void* v38;
    int64_t v62;
    int64_t v151; /* placeholder for void */
    int64_t v236;
    int64_t v231; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    int64_t v220; /* placeholder for void */
    int64_t v50;
    void* v172;
    void* v29;
    int64_t v33; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    bool v78;
    int64_t v142;
    int64_t v46; /* placeholder for void */
    int64_t v51;
    int64_t v47; /* placeholder for void */
    int64_t v93;
    int64_t v82;
    int64_t v31;
    int64_t v101;
    void* v109;
    int64_t v61;
    bool v145;
    int64_t v226;
    int64_t v234; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    int64_t v58;
    int64_t v65;
    int64_t v30; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    uint8_t v116;
    void* v17;
    void* v178;
    int64_t v193;
    void* v123;
    int64_t v177;
    int64_t v120;
    void* v126;
    int64_t v91;
    int64_t v81; /* placeholder for void */
    int64_t v197;
    int64_t v140;
    int64_t v114; /* placeholder for void */
    bool v144;
    int64_t v201;
    int64_t v215;
    int64_t v207;
    int64_t v7;
    void* v115;
    int64_t v241;
    bool v182;
    v2 = 0LL;
    v3 = (int64_t)String_len((void*)v2, (int64_t)v1);
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    v7 = 0LL;
    v8 = v6 == v7;
    if (v8) goto bb1;
    else goto bb2;
  bb1:
    v10 = 0LL;
    p_call_stack_pop();
    return v10;
  bb2:
    goto bb3;
  bb3:
    v13 = 0LL;
    v14 = malloc(8);
    *(int64_t*)v14 = (int64_t)v13;
    v16 = 0LL;
    v17 = malloc(8);
    *(int64_t*)v17 = (int64_t)v16;
    v19 = false;
    v20 = malloc(1);
    *(bool*)v20 = v19;
    goto bb4;
  bb4:
    v23 = *(int64_t*)v14;
    v24 = *(int64_t*)v4;
    v25 = v23 < v24;
    if (v25) goto bb5;
    else goto bb6;
  bb5:
    v27 = *(int64_t*)v14;
    v28 = (int64_t)v1 + v27;
    v29 = malloc(8);
    *(int64_t*)v29 = (int64_t)v28;
    v31 = *(int64_t*)v29;
    v32 = malloc(8);
    *(void**)v32 = (void*)v31;
    v34 = 0LL;
    v35 = malloc(8);
    *(int64_t*)v35 = (int64_t)v34;
    v38 = *(void**)v32;
    v39 = *(uint8_t*)v38;
    *(int64_t*)v35 = (int64_t)v39;
    v42 = *(int64_t*)v35;
    v43 = 0LL;
    v44 = (bool)char_is_whitespace((void*)v43, v42);
    v45 = !v44;
    if (v45) goto bb7;
    else goto bb8;
  bb6:
    v54 = *(int64_t*)v14;
    v55 = *(int64_t*)v4;
    v56 = v54 >= v55;
    if (v56) goto bb10;
    else goto bb11;
  bb7:
    goto bb6;
  bb8:
    goto bb9;
  bb9:
    v49 = *(int64_t*)v14;
    v50 = 1LL;
    v51 = v49 + v50;
    *(int64_t*)v14 = (int64_t)v51;
    goto bb4;
  bb10:
    v58 = 0LL;
    p_call_stack_pop();
    return v58;
  bb11:
    goto bb12;
  bb12:
    v61 = *(int64_t*)v14;
    v62 = (int64_t)v1 + v61;
    v63 = malloc(8);
    *(int64_t*)v63 = (int64_t)v62;
    v65 = *(int64_t*)v63;
    v66 = malloc(8);
    *(void**)v66 = (void*)v65;
    v68 = 0LL;
    v69 = malloc(8);
    *(int64_t*)v69 = (int64_t)v68;
    v72 = *(void**)v66;
    v73 = *(uint8_t*)v72;
    *(int64_t*)v69 = (int64_t)v73;
    v76 = *(int64_t*)v69;
    v77 = 45LL;
    v78 = v76 == v77;
    if (v78) goto bb13;
    else goto bb14;
  bb13:
    v80 = true;
    *(bool*)v20 = v80;
    v82 = *(int64_t*)v14;
    v83 = 1LL;
    v84 = v82 + v83;
    *(int64_t*)v14 = (int64_t)v84;
    goto bb15;
  bb14:
    v87 = *(int64_t*)v69;
    v88 = 43LL;
    v89 = v87 == v88;
    if (v89) goto bb16;
    else goto bb17;
  bb15:
    v98 = *(int64_t*)v14;
    v99 = 1LL;
    v100 = v98 + v99;
    v101 = *(int64_t*)v4;
    v102 = v100 < v101;
    if (v102) goto bb19;
    else goto bb20;
  bb16:
    v91 = *(int64_t*)v14;
    v92 = 1LL;
    v93 = v91 + v92;
    *(int64_t*)v14 = (int64_t)v93;
    goto bb18;
  bb17:
    goto bb18;
  bb18:
    goto bb15;
  bb19:
    v104 = *(int64_t*)v14;
    v105 = (int64_t)v1 + v104;
    v106 = malloc(8);
    *(int64_t*)v106 = (int64_t)v105;
    v108 = *(int64_t*)v106;
    v109 = malloc(8);
    *(void**)v109 = (void*)v108;
    v111 = 0LL;
    v112 = malloc(8);
    *(int64_t*)v112 = (int64_t)v111;
    v115 = *(void**)v109;
    v116 = *(uint8_t*)v115;
    *(int64_t*)v112 = (int64_t)v116;
    v119 = *(int64_t*)v14;
    v120 = (int64_t)v1 + v119;
    v121 = 1LL;
    v122 = v120 + v121;
    v123 = malloc(8);
    *(int64_t*)v123 = (int64_t)v122;
    v125 = *(int64_t*)v123;
    v126 = malloc(8);
    *(void**)v126 = (void*)v125;
    v128 = 0LL;
    v129 = malloc(8);
    *(int64_t*)v129 = (int64_t)v128;
    v132 = *(void**)v126;
    v133 = *(uint8_t*)v132;
    *(int64_t*)v129 = (int64_t)v133;
    v136 = *(int64_t*)v112;
    v137 = 48LL;
    v138 = v136 == v137;
    v139 = *(int64_t*)v129;
    v140 = 120LL;
    v141 = v139 == v140;
    v142 = *(int64_t*)v129;
    v143 = 88LL;
    v144 = v142 == v143;
    v145 = v141 || v144;
    v146 = v138 && v145;
    if (v146) goto bb22;
    else goto bb23;
  bb20:
    goto bb21;
  bb21:
    goto bb25;
  bb22:
    v148 = *(int64_t*)v14;
    v149 = 2LL;
    v150 = v148 + v149;
    *(int64_t*)v14 = (int64_t)v150;
    goto bb24;
  bb23:
    goto bb24;
  bb24:
    goto bb21;
  bb25:
    v157 = *(int64_t*)v14;
    v158 = *(int64_t*)v4;
    v159 = v157 < v158;
    if (v159) goto bb26;
    else goto bb27;
  bb26:
    v161 = *(int64_t*)v14;
    v162 = (int64_t)v1 + v161;
    v163 = malloc(8);
    *(int64_t*)v163 = (int64_t)v162;
    v165 = *(int64_t*)v163;
    v166 = malloc(8);
    *(void**)v166 = (void*)v165;
    v168 = 0LL;
    v169 = malloc(8);
    *(int64_t*)v169 = (int64_t)v168;
    v172 = *(void**)v166;
    v173 = *(uint8_t*)v172;
    *(int64_t*)v169 = (int64_t)v173;
    v176 = 1LL;
    v177 = -v176;
    v178 = malloc(8);
    *(int64_t*)v178 = (int64_t)v177;
    v180 = *(int64_t*)v169;
    v181 = 48LL;
    v182 = v180 >= v181;
    v183 = *(int64_t*)v169;
    v184 = 57LL;
    v185 = v183 <= v184;
    v186 = v182 && v185;
    if (v186) goto bb28;
    else goto bb29;
  bb27:
    v233 = *(bool*)v20;
    if (v233) goto bb37;
    else goto bb38;
  bb28:
    v188 = *(int64_t*)v169;
    v189 = 48LL;
    v190 = v188 - v189;
    *(int64_t*)v178 = (int64_t)v190;
    goto bb30;
  bb29:
    v193 = *(int64_t*)v169;
    v194 = 65LL;
    v195 = v193 >= v194;
    v196 = *(int64_t*)v169;
    v197 = 70LL;
    v198 = v196 <= v197;
    v199 = v195 && v198;
    if (v199) goto bb31;
    else goto bb32;
  bb30:
    v222 = *(int64_t*)v17;
    v223 = 16LL;
    v224 = v222 * v223;
    v225 = *(int64_t*)v178;
    v226 = v224 + v225;
    *(int64_t*)v17 = (int64_t)v226;
    v228 = *(int64_t*)v14;
    v229 = 1LL;
    v230 = v228 + v229;
    *(int64_t*)v14 = (int64_t)v230;
    goto bb25;
  bb31:
    v201 = *(int64_t*)v169;
    v202 = 55LL;
    v203 = v201 - v202;
    *(int64_t*)v178 = (int64_t)v203;
    goto bb33;
  bb32:
    v206 = *(int64_t*)v169;
    v207 = 97LL;
    v208 = v206 >= v207;
    v209 = *(int64_t*)v169;
    v210 = 102LL;
    v211 = v209 <= v210;
    v212 = v208 && v211;
    if (v212) goto bb34;
    else goto bb35;
  bb33:
    goto bb30;
  bb34:
    v214 = *(int64_t*)v169;
    v215 = 87LL;
    v216 = v214 - v215;
    *(int64_t*)v178 = (int64_t)v216;
    goto bb36;
  bb35:
    goto bb27;
  bb36:
    goto bb33;
  bb37:
    v235 = 0LL;
    v236 = *(int64_t*)v17;
    v237 = v235 - v236;
    *(int64_t*)v17 = (int64_t)v237;
    goto bb39;
  bb38:
    goto bb39;
  bb39:
    v241 = *(int64_t*)v17;
    p_call_stack_pop();
    return v241;
    return 0;
}

int64_t Vec_get(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("Vec_get");
    int64_t v40; /* placeholder for void */
    void* v25;
    int64_t v36;
    int64_t v41;
    int64_t v13; /* placeholder for void */
    void* v7;
    int64_t v14; /* placeholder for void */
    int64_t v15;
    int64_t v35;
    int64_t v38;
    int64_t v43; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    int64_t v3;
    int64_t v32;
    void* v18;
    void* v42;
    int64_t v46; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    void* v45;
    int64_t v49;
    int64_t v6; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v12;
    void* v11;
    bool v23;
    int64_t v26; /* placeholder for void */
    void* v29;
    int64_t v19; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    bool v16;
    int64_t v28; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v37;
    void* v39;
    int64_t v22;
    int64_t v4;
    int64_t v8; /* placeholder for void */
    void* v9;
    int64_t v44;
    void* v5;
    int64_t v10; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    void* v48;
    int64_t v51; /* placeholder for void */
    int64_t v52;
    void* v31;
    int64_t v27; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    v3 = 8LL;
    v4 = v1 + v3;
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = malloc(8);
    *(void**)v7 = (void*)v1;
    v9 = malloc(8);
    v11 = *(void**)v5;
    v12 = *(int64_t*)v11;
    *(int64_t*)v9 = (int64_t)v12;
    v15 = *(int64_t*)v9;
    v16 = v2 >= v15;
    if (v16) goto bb1;
    else goto bb2;
  bb1:
    v18 = _str0;
    p_panic(v18);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v22 = 0LL;
    v23 = v2 < v22;
    if (v23) goto bb4;
    else goto bb5;
  bb4:
    v25 = _str0;
    p_panic(v25);
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v29 = malloc(8);
    v31 = *(void**)v7;
    v32 = *(int64_t*)v31;
    *(int64_t*)v29 = (int64_t)v32;
    v35 = *(int64_t*)v29;
    v36 = 8LL;
    v37 = v2 * v36;
    v38 = v35 + v37;
    v39 = malloc(8);
    *(int64_t*)v39 = (int64_t)v38;
    v41 = *(int64_t*)v39;
    v42 = malloc(8);
    *(void**)v42 = (void*)v41;
    v44 = 0LL;
    v45 = malloc(8);
    *(int64_t*)v45 = (int64_t)v44;
    v48 = *(void**)v42;
    v49 = *(int64_t*)v48;
    *(int64_t*)v45 = (int64_t)v49;
    v52 = *(int64_t*)v45;
    p_call_stack_pop();
    return v52;
    return 0;
}

int64_t String_to_uppercase(void* v0, int64_t v1) {
    p_call_stack_push("String_to_uppercase");
    int64_t v43;
    int64_t v8; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v44;
    int64_t v23; /* placeholder for void */
    int64_t v93; /* placeholder for void */
    bool v66;
    int64_t v99; /* placeholder for void */
    int64_t v85;
    int64_t v30;
    int64_t v49; /* placeholder for void */
    int64_t v21;
    int64_t v78; /* placeholder for void */
    int64_t v90;
    int64_t v17;
    int64_t v61;
    void* v94;
    int64_t v96; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v65;
    int64_t v97; /* placeholder for void */
    bool v36;
    void* v40;
    int64_t v12; /* placeholder for void */
    int64_t v24;
    void* v7;
    int64_t v72; /* placeholder for void */
    int64_t v82;
    bool v3;
    int64_t v47;
    int64_t v55; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    uint8_t v58;
    int64_t v75; /* placeholder for void */
    void* v76;
    int64_t v18; /* placeholder for void */
    void* v27;
    int64_t v86;
    int64_t v92; /* placeholder for void */
    int64_t v80;
    bool v63;
    int64_t v46; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v26;
    void* v45;
    int64_t v37; /* placeholder for void */
    int64_t v42;
    int64_t v56; /* placeholder for void */
    int64_t v25;
    int64_t v53;
    int64_t v70;
    int64_t v50;
    int64_t v52; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    void* v57;
    int64_t v5;
    int64_t v9;
    int64_t v14;
    int64_t v20;
    int64_t v35;
    int64_t v74; /* placeholder for void */
    void* v88;
    int64_t v89; /* placeholder for void */
    void* v91;
    int64_t v38;
    int64_t v81;
    void* v31;
    int64_t v83; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v98;
    int64_t v59; /* placeholder for void */
    void* v10;
    int64_t v62;
    void* v51;
    int64_t v73; /* placeholder for void */
    int64_t v69;
    int64_t v71;
    int64_t v64;
    void* v54;
    void* v13;
    int64_t v33; /* placeholder for void */
    bool v67;
    int64_t v77;
    int64_t v15; /* placeholder for void */
    int64_t v95;
    int64_t v79; /* placeholder for void */
    int64_t v87;
    int64_t v68; /* placeholder for void */
    int64_t v2;
    int64_t v84; /* placeholder for void */
    int64_t v39;
    void* v6;
    void* v28;
    void* v48;
    int64_t v34;
    int64_t v4; /* placeholder for void */
    void* v22;
    int64_t v32; /* placeholder for void */
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 1LL;
    v6 = p_malloc(v5);
    v7 = malloc(8);
    *(int64_t*)v7 = (int64_t)v6;
    v9 = *(int64_t*)v7;
    v10 = malloc(8);
    *(void**)v10 = (void*)v9;
    v13 = *(void**)v10;
    v14 = 0LL;
    *(uint8_t*)v13 = (uint8_t)v14;
    v17 = *(int64_t*)v7;
    p_call_stack_pop();
    return v17;
  bb2:
    goto bb3;
  bb3:
    v20 = 0LL;
    v21 = (int64_t)p_str_length((void*)v20, v1);
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v21;
    v24 = *(int64_t*)v22;
    v25 = 1LL;
    v26 = v24 + v25;
    v27 = p_malloc(v26);
    v28 = malloc(8);
    *(int64_t*)v28 = (int64_t)v27;
    v30 = 0LL;
    v31 = malloc(8);
    *(int64_t*)v31 = (int64_t)v30;
    goto bb4;
  bb4:
    v34 = *(int64_t*)v31;
    v35 = *(int64_t*)v22;
    v36 = v34 < v35;
    if (v36) goto bb5;
    else goto bb6;
  bb5:
    v38 = *(int64_t*)v31;
    v39 = v1 + v38;
    v40 = malloc(8);
    *(int64_t*)v40 = (int64_t)v39;
    v42 = *(int64_t*)v28;
    v43 = *(int64_t*)v31;
    v44 = v42 + v43;
    v45 = malloc(8);
    *(int64_t*)v45 = (int64_t)v44;
    v47 = *(int64_t*)v40;
    v48 = malloc(8);
    *(void**)v48 = (void*)v47;
    v50 = *(int64_t*)v45;
    v51 = malloc(8);
    *(void**)v51 = (void*)v50;
    v53 = 0LL;
    v54 = malloc(8);
    *(int64_t*)v54 = (int64_t)v53;
    v57 = *(void**)v48;
    v58 = *(uint8_t*)v57;
    *(int64_t*)v54 = (int64_t)v58;
    v61 = *(int64_t*)v54;
    v62 = 97LL;
    v63 = v61 >= v62;
    v64 = *(int64_t*)v54;
    v65 = 122LL;
    v66 = v64 <= v65;
    v67 = v63 && v66;
    if (v67) goto bb7;
    else goto bb8;
  bb6:
    v85 = *(int64_t*)v28;
    v86 = *(int64_t*)v22;
    v87 = v85 + v86;
    v88 = malloc(8);
    *(int64_t*)v88 = (int64_t)v87;
    v90 = *(int64_t*)v88;
    v91 = malloc(8);
    *(void**)v91 = (void*)v90;
    v94 = *(void**)v91;
    v95 = 0LL;
    *(uint8_t*)v94 = (uint8_t)v95;
    v98 = *(int64_t*)v28;
    p_call_stack_pop();
    return v98;
  bb7:
    v69 = *(int64_t*)v54;
    v70 = 32LL;
    v71 = v69 - v70;
    *(int64_t*)v54 = (int64_t)v71;
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    v76 = *(void**)v51;
    v77 = *(int64_t*)v54;
    *(uint8_t*)v76 = (uint8_t)v77;
    v80 = *(int64_t*)v31;
    v81 = 1LL;
    v82 = v80 + v81;
    *(int64_t*)v31 = (int64_t)v82;
    goto bb4;
    return 0;
}

bool p_str_equal(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("p_str_equal");
    int64_t v57;
    int64_t v51; /* placeholder for void */
    int64_t v63;
    int64_t v65;
    int64_t v21; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    int64_t v58; /* placeholder for void */
    int64_t v41;
    int64_t v54; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    void* v20;
    int64_t v4; /* placeholder for void */
    bool v52;
    int64_t v61;
    void* v37;
    int64_t v38; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v40;
    bool v15;
    int64_t v48;
    int64_t v8;
    int64_t v74;
    int64_t v43;
    int64_t v47; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    bool v45;
    bool v5;
    int64_t v14;
    void* v35;
    void* v28;
    int64_t v59;
    uint8_t v36;
    bool v42;
    int64_t v6; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    uint8_t v68;
    int64_t v75;
    int64_t v23; /* placeholder for void */
    int64_t v24;
    void* v70;
    int64_t v44;
    int64_t v30; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v49;
    int64_t v39; /* placeholder for void */
    int64_t v60;
    int64_t v56;
    void* v67;
    int64_t v72; /* placeholder for void */
    bool v11;
    int64_t v73; /* placeholder for void */
    void* v25;
    void* v31;
    uint8_t v32;
    int64_t v55;
    int64_t v69; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    void* v33;
    bool v3;
    int64_t v27;
    int64_t v66; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    bool v50;
    int64_t v16; /* placeholder for void */
    bool v9;
    int64_t v12; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    void* v22;
    bool v76;
    uint8_t v71;
    bool v46;
    int64_t v18; /* placeholder for void */
    bool v17;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = true;
    p_call_stack_pop();
    return v5;
  bb2:
    goto bb3;
  bb3:
    v8 = 0LL;
    v9 = v1 == v8;
    if (v9) goto bb4;
    else goto bb5;
  bb4:
    v11 = false;
    p_call_stack_pop();
    return v11;
  bb5:
    goto bb6;
  bb6:
    v14 = 0LL;
    v15 = v2 == v14;
    if (v15) goto bb7;
    else goto bb8;
  bb7:
    v17 = false;
    p_call_stack_pop();
    return v17;
  bb8:
    goto bb9;
  bb9:
    v20 = malloc(8);
    *(int64_t*)v20 = (int64_t)v1;
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v2;
    v24 = *(int64_t*)v20;
    v25 = malloc(8);
    *(void**)v25 = (void*)v24;
    v27 = *(int64_t*)v22;
    v28 = malloc(8);
    *(void**)v28 = (void*)v27;
    v31 = *(void**)v25;
    v32 = *(uint8_t*)v31;
    v33 = malloc(8);
    *(int64_t*)v33 = (int64_t)v32;
    v35 = *(void**)v28;
    v36 = *(uint8_t*)v35;
    v37 = malloc(8);
    *(int64_t*)v37 = (int64_t)v36;
    goto bb10;
  bb10:
    v40 = *(int64_t*)v33;
    v41 = 0LL;
    v42 = v40 != v41;
    v43 = *(int64_t*)v37;
    v44 = 0LL;
    v45 = v43 != v44;
    v46 = v42 && v45;
    if (v46) goto bb11;
    else goto bb12;
  bb11:
    v48 = *(int64_t*)v33;
    v49 = *(int64_t*)v37;
    v50 = v48 != v49;
    if (v50) goto bb13;
    else goto bb14;
  bb12:
    v74 = *(int64_t*)v33;
    v75 = *(int64_t*)v37;
    v76 = v74 == v75;
    p_call_stack_pop();
    return v76;
  bb13:
    v52 = false;
    p_call_stack_pop();
    return v52;
  bb14:
    goto bb15;
  bb15:
    v55 = *(int64_t*)v20;
    v56 = 1LL;
    v57 = v55 + v56;
    *(int64_t*)v20 = (int64_t)v57;
    v59 = *(int64_t*)v22;
    v60 = 1LL;
    v61 = v59 + v60;
    *(int64_t*)v22 = (int64_t)v61;
    v63 = *(int64_t*)v20;
    *(void**)v25 = (void*)v63;
    v65 = *(int64_t*)v22;
    *(void**)v28 = (void*)v65;
    v67 = *(void**)v25;
    v68 = *(uint8_t*)v67;
    *(int64_t*)v33 = (int64_t)v68;
    v70 = *(void**)v28;
    v71 = *(uint8_t*)v70;
    *(int64_t*)v37 = (int64_t)v71;
    goto bb10;
    return 0;
}

double get(void* v0, void* v1, void* v2) {
    p_call_stack_push("get");
    double v3;
    int64_t v4; /* placeholder for void */
    v3 = (double)p_tensor_get(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

void Result_map(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Result_map");
    int64_t v31; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    void* v9;
    bool v30;
    int64_t v5;
    void* v11;
    int64_t v25; /* placeholder for void */
    void* v44;
    int64_t v6;
    bool v7;
    void* v32;
    void* v18;
    void* v34;
    int64_t v46;
    void* v4;
    int64_t v8; /* placeholder for void */
    int64_t v15;
    void* v45;
    int64_t v24;
    int64_t v47; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v35;
    int64_t v48; /* placeholder for void */
    int64_t v28;
    int64_t v21; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v27;
    void* v38;
    int64_t v39; /* placeholder for void */
    void* v22;
    void* v41;
    int64_t v43; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    void* v19;
    int64_t v12;
    int64_t v50; /* placeholder for void */
    int64_t v10;
    void* v16;
    int64_t v20;
    int64_t v42;
    void* v23;
    int64_t v29;
    void* v36;
    void* v40;
    int64_t v26; /* placeholder for void */
    int64_t v33;
    void* v13;
    v4 = (char*)v2 + 0;
    v5 = *(int64_t*)v4;
    v6 = 0LL;
    v7 = v5 == v6;
    if (v7) goto bb2;
    else goto bb3;
  bb1:
    p_call_stack_pop();
    return;
  bb2:
    v9 = (char*)v2 + 8;
    v10 = *(int64_t*)v9;
    v11 = malloc(16);
    v12 = 0LL;
    v13 = (char*)v11 + 0;
    *(int64_t*)v13 = (int64_t)v12;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ndim") == 0) {
            v15 = ndim((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "size") == 0) {
            v15 = size((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (int64_t)v10);
        } else { abort(); }
    }
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
    p_call_stack_pop();
    return;
  bb3:
    v27 = (char*)v2 + 0;
    v28 = *(int64_t*)v27;
    v29 = 1LL;
    v30 = v28 == v29;
    if (v30) goto bb4;
    else goto bb5;
  bb4:
    v32 = (char*)v2 + 8;
    v33 = *(int64_t*)v32;
    v34 = malloc(16);
    v35 = 1LL;
    v36 = (char*)v34 + 0;
    *(int64_t*)v36 = (int64_t)v35;
    v38 = (char*)v34 + 8;
    *(int64_t*)v38 = (int64_t)v33;
    v40 = (char*)v34 + 0;
    v41 = (char*)v0 + 0;
    v42 = *(int64_t*)v40;
    *(int64_t*)v41 = (int64_t)v42;
    v44 = (char*)v34 + 8;
    v45 = (char*)v0 + 8;
    v46 = *(int64_t*)v44;
    *(int64_t*)v45 = (int64_t)v46;
    p_call_stack_pop();
    return;
  bb5:
    goto bb1;
    return;
}

bool str_contains(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_contains");
    int64_t v5; /* placeholder for void */
    int64_t v3;
    bool v4;
    v3 = 0LL;
    v4 = (bool)String_contains((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

bool String_ends_with(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_ends_with");
    int64_t v88; /* placeholder for void */
    bool v45;
    int64_t v84;
    int64_t v39;
    int64_t v56; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    void* v64;
    int64_t v3;
    int64_t v38; /* placeholder for void */
    bool v7;
    int64_t v30; /* placeholder for void */
    void* v51;
    int64_t v69; /* placeholder for void */
    bool v79;
    int64_t v28;
    void* v14;
    int64_t v54;
    int64_t v46; /* placeholder for void */
    bool v6;
    int64_t v63;
    int64_t v21;
    int64_t v36;
    int64_t v49;
    int64_t v83; /* placeholder for void */
    int64_t v85;
    uint8_t v74;
    int64_t v8; /* placeholder for void */
    bool v24;
    bool v81;
    int64_t v19; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    bool v29;
    int64_t v26; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v87; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v66;
    int64_t v77;
    int64_t v42; /* placeholder for void */
    int64_t v78;
    int64_t v43;
    int64_t v35;
    int64_t v52; /* placeholder for void */
    void* v61;
    int64_t v65; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    bool v22;
    int64_t v57;
    int64_t v86;
    void* v67;
    void* v73;
    void* v55;
    int64_t v48;
    int64_t v16;
    int64_t v13;
    int64_t v10; /* placeholder for void */
    void* v40;
    int64_t v60;
    int64_t v33; /* placeholder for void */
    bool v31;
    int64_t v53;
    int64_t v27;
    void* v58;
    int64_t v12;
    bool v9;
    int64_t v17;
    void* v18;
    int64_t v50;
    void* v70;
    int64_t v82; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    bool v4;
    int64_t v5;
    void* v37;
    int64_t v76; /* placeholder for void */
    bool v89;
    int64_t v20;
    int64_t v32; /* placeholder for void */
    int64_t v47;
    uint8_t v71;
    int64_t v80; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v34;
    int64_t v68; /* placeholder for void */
    int64_t v72; /* placeholder for void */
    int64_t v44;
    v3 = 0LL;
    v4 = v1 == v3;
    v5 = 0LL;
    v6 = v2 == v5;
    v7 = v4 || v6;
    if (v7) goto bb1;
    else goto bb2;
  bb1:
    v9 = false;
    p_call_stack_pop();
    return v9;
  bb2:
    goto bb3;
  bb3:
    v12 = 0LL;
    v13 = (int64_t)p_str_length((void*)v12, v1);
    v14 = malloc(8);
    *(int64_t*)v14 = (int64_t)v13;
    v16 = 0LL;
    v17 = (int64_t)p_str_length((void*)v16, v2);
    v18 = malloc(8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = *(int64_t*)v18;
    v21 = *(int64_t*)v14;
    v22 = v20 > v21;
    if (v22) goto bb4;
    else goto bb5;
  bb4:
    v24 = false;
    p_call_stack_pop();
    return v24;
  bb5:
    goto bb6;
  bb6:
    v27 = *(int64_t*)v18;
    v28 = 0LL;
    v29 = v27 == v28;
    if (v29) goto bb7;
    else goto bb8;
  bb7:
    v31 = true;
    p_call_stack_pop();
    return v31;
  bb8:
    goto bb9;
  bb9:
    v34 = *(int64_t*)v14;
    v35 = *(int64_t*)v18;
    v36 = v34 - v35;
    v37 = malloc(8);
    *(int64_t*)v37 = (int64_t)v36;
    v39 = 0LL;
    v40 = malloc(8);
    *(int64_t*)v40 = (int64_t)v39;
    goto bb10;
  bb10:
    v43 = *(int64_t*)v40;
    v44 = *(int64_t*)v18;
    v45 = v43 < v44;
    if (v45) goto bb11;
    else goto bb12;
  bb11:
    v47 = *(int64_t*)v37;
    v48 = v1 + v47;
    v49 = *(int64_t*)v40;
    v50 = v48 + v49;
    v51 = malloc(8);
    *(int64_t*)v51 = (int64_t)v50;
    v53 = *(int64_t*)v40;
    v54 = v2 + v53;
    v55 = malloc(8);
    *(int64_t*)v55 = (int64_t)v54;
    v57 = *(int64_t*)v51;
    v58 = malloc(8);
    *(void**)v58 = (void*)v57;
    v60 = *(int64_t*)v55;
    v61 = malloc(8);
    *(void**)v61 = (void*)v60;
    v63 = 0LL;
    v64 = malloc(8);
    *(int64_t*)v64 = (int64_t)v63;
    v66 = 0LL;
    v67 = malloc(8);
    *(int64_t*)v67 = (int64_t)v66;
    v70 = *(void**)v58;
    v71 = *(uint8_t*)v70;
    *(int64_t*)v64 = (int64_t)v71;
    v73 = *(void**)v61;
    v74 = *(uint8_t*)v73;
    *(int64_t*)v67 = (int64_t)v74;
    v77 = *(int64_t*)v64;
    v78 = *(int64_t*)v67;
    v79 = v77 != v78;
    if (v79) goto bb13;
    else goto bb14;
  bb12:
    v89 = true;
    p_call_stack_pop();
    return v89;
  bb13:
    v81 = false;
    p_call_stack_pop();
    return v81;
  bb14:
    goto bb15;
  bb15:
    v84 = *(int64_t*)v40;
    v85 = 1LL;
    v86 = v84 + v85;
    *(int64_t*)v40 = (int64_t)v86;
    goto bb10;
    return 0;
}

bool str_equal(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_equal");
    bool v4;
    int64_t v5; /* placeholder for void */
    int64_t v3;
    v3 = 0LL;
    v4 = (bool)p_str_equal((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

bool Result_is_ok(void* v0, void* v1) {
    p_call_stack_push("Result_is_ok");
    int64_t v13;
    int64_t v20; /* placeholder for void */
    bool v5;
    int64_t v3;
    int64_t v4;
    bool v14;
    void* v16;
    int64_t v19; /* placeholder for void */
    void* v7;
    int64_t v10; /* placeholder for void */
    int64_t v8;
    void* v11;
    int64_t v12;
    bool v18;
    int64_t v17;
    int64_t v22; /* placeholder for void */
    void* v2;
    int64_t v6; /* placeholder for void */
    bool v21;
    bool v9;
    int64_t v15; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v21 = false;
    p_call_stack_pop();
    return v21;
  bb2:
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    v9 = true;
    p_call_stack_pop();
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
    p_call_stack_pop();
    return v18;
  bb5:
    goto bb1;
    return 0;
}

int64_t char_to_upper(void* v0, int64_t v1) {
    p_call_stack_push("char_to_upper");
    bool v6;
    bool v3;
    int64_t v8;
    int64_t v10; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v9;
    int64_t v2;
    bool v5;
    int64_t v11; /* placeholder for void */
    int64_t v4;
    v2 = 97LL;
    v3 = v1 >= v2;
    v4 = 122LL;
    v5 = v1 <= v4;
    v6 = v3 && v5;
    if (v6) goto bb1;
    else goto bb2;
  bb1:
    v8 = 32LL;
    v9 = v1 - v8;
    p_call_stack_pop();
    return v9;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return v1;
    return 0;
}

void* char_to_str(void* v0, int64_t v1) {
    p_call_stack_push("char_to_str");
    int64_t v3; /* placeholder for void */
    void* v2;
    v2 = p_char_to_str(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void* Vec_to_str(void* v0, int64_t v1) {
    p_call_stack_push("Vec_to_str");
    int64_t v38; /* placeholder for void */
    int64_t v72; /* placeholder for void */
    int64_t v51;
    void* v8;
    bool v29;
    int64_t v48;
    int64_t v67; /* placeholder for void */
    int64_t v66;
    void* v73;
    void* v69;
    int64_t v19; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v43;
    void* v61;
    void* v70;
    int64_t v45;
    void* v6;
    int64_t v27;
    int64_t v50; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    void* v21;
    int64_t v53; /* placeholder for void */
    void* v4;
    int64_t v2;
    int64_t v9; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v60;
    void* v55;
    int64_t v11;
    void* v24;
    int64_t v56;
    void* v71;
    int64_t v63; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v64;
    int64_t v28;
    int64_t v42;
    int64_t v74; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v58; /* placeholder for void */
    void* v52;
    void* v36;
    int64_t v47; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    int64_t v3;
    int64_t v7; /* placeholder for void */
    void* v10;
    int64_t v18; /* placeholder for void */
    bool v33;
    int64_t v17;
    void* v14;
    int64_t v40; /* placeholder for void */
    int64_t v44;
    int64_t v13; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    void* v20;
    void* v62;
    int64_t v65;
    void* v37;
    void* v59;
    int64_t v22; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    void* v16;
    int64_t v41;
    int64_t v31;
    int64_t v68; /* placeholder for void */
    int64_t v23;
    int64_t v32;
    int64_t v15; /* placeholder for void */
    void* v35;
    void* v46;
    void* v49;
    v2 = 8LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = malloc(8);
    *(void**)v6 = (void*)v1;
    v8 = malloc(8);
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v8 = (int64_t)v11;
    v14 = malloc(8);
    v16 = *(void**)v6;
    v17 = *(int64_t*)v16;
    *(int64_t*)v14 = (int64_t)v17;
    v20 = _str1;
    v21 = malloc(8);
    *(void**)v21 = (void*)v20;
    v23 = 0LL;
    v24 = malloc(8);
    *(int64_t*)v24 = (int64_t)v23;
    goto bb1;
  bb1:
    v27 = *(int64_t*)v24;
    v28 = *(int64_t*)v8;
    v29 = v27 < v28;
    if (v29) goto bb2;
    else goto bb3;
  bb2:
    v31 = *(int64_t*)v24;
    v32 = 0LL;
    v33 = v31 > v32;
    if (v33) goto bb4;
    else goto bb5;
  bb3:
    v69 = *(void**)v21;
    v70 = _str2;
    v71 = p_str_concat(v69, v70);
    *(void**)v21 = (void*)v71;
    v73 = *(void**)v21;
    p_call_stack_pop();
    return v73;
  bb4:
    v35 = *(void**)v21;
    v36 = _str3;
    v37 = p_str_concat(v35, v36);
    *(void**)v21 = (void*)v37;
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v41 = *(int64_t*)v14;
    v42 = *(int64_t*)v24;
    v43 = 8LL;
    v44 = v42 * v43;
    v45 = v41 + v44;
    v46 = malloc(8);
    *(int64_t*)v46 = (int64_t)v45;
    v48 = *(int64_t*)v46;
    v49 = malloc(8);
    *(void**)v49 = (void*)v48;
    v51 = 0LL;
    v52 = malloc(8);
    *(int64_t*)v52 = (int64_t)v51;
    v55 = *(void**)v49;
    v56 = *(int64_t*)v55;
    *(int64_t*)v52 = (int64_t)v56;
    v59 = *(void**)v21;
    v60 = *(int64_t*)v52;
    v61 = (void*)p_to_str((int64_t)v60);
    v62 = p_str_concat(v59, v61);
    *(void**)v21 = (void*)v62;
    v64 = *(int64_t*)v24;
    v65 = 1LL;
    v66 = v64 + v65;
    *(int64_t*)v24 = (int64_t)v66;
    goto bb1;
    return 0;
}

void* reshape(void* v0, void* v1, void* v2) {
    p_call_stack_push("reshape");
    int64_t v4; /* placeholder for void */
    void* v3;
    v3 = p_tensor_reshape(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

void println(void* v0, void* v1) {
    p_call_stack_push("println");
    int64_t v2; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    void* v3;
    int64_t v5; /* placeholder for void */
    p_print_str(v1);
    v3 = _str4;
    p_print_str(v3);
    p_call_stack_pop();
    return;
    return;
}

void set(void* v0, void* v1, void* v2, double v3) {
    p_call_stack_push("set");
    int64_t v4; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    p_tensor_set(v1, v2, v3);
    p_call_stack_pop();
    return;
    return;
}

void Vec_truncate(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("Vec_truncate");
    int64_t v11; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v3;
    int64_t v6; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v4;
    int64_t v10;
    int64_t v16; /* placeholder for void */
    int64_t v13;
    int64_t v8; /* placeholder for void */
    bool v14;
    void* v9;
    void* v17;
    void* v5;
    int64_t v22; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    void* v7;
    int64_t v12; /* placeholder for void */
    v3 = 8LL;
    v4 = v1 + v3;
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = malloc(8);
    v9 = *(void**)v5;
    v10 = *(int64_t*)v9;
    *(int64_t*)v7 = (int64_t)v10;
    v13 = *(int64_t*)v7;
    v14 = v2 < v13;
    if (v14) goto bb1;
    else goto bb2;
  bb1:
    v17 = *(void**)v5;
    *(int64_t*)v17 = (int64_t)v2;
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return;
    return;
}

int64_t String_to_lowercase(void* v0, int64_t v1) {
    p_call_stack_push("String_to_lowercase");
    void* v27;
    void* v54;
    void* v45;
    void* v91;
    void* v22;
    int64_t v19; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    int64_t v14;
    int64_t v70;
    int64_t v29; /* placeholder for void */
    int64_t v30;
    int64_t v79; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v38;
    void* v28;
    void* v7;
    int64_t v33; /* placeholder for void */
    int64_t v50;
    bool v63;
    int64_t v23; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v25;
    int64_t v87;
    bool v67;
    int64_t v5;
    int64_t v89; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v34;
    bool v3;
    int64_t v9;
    int64_t v59; /* placeholder for void */
    int64_t v44;
    void* v10;
    uint8_t v58;
    void* v94;
    int64_t v49; /* placeholder for void */
    void* v6;
    int64_t v73; /* placeholder for void */
    int64_t v2;
    int64_t v55; /* placeholder for void */
    int64_t v20;
    int64_t v43;
    int64_t v41; /* placeholder for void */
    int64_t v65;
    void* v31;
    int64_t v71;
    int64_t v4; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v82;
    void* v51;
    int64_t v72; /* placeholder for void */
    int64_t v53;
    int64_t v99; /* placeholder for void */
    int64_t v24;
    int64_t v17;
    int64_t v42;
    bool v36;
    void* v48;
    int64_t v62;
    void* v76;
    int64_t v77;
    int64_t v61;
    int64_t v60; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v85;
    int64_t v80;
    int64_t v18; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v90;
    int64_t v83; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    void* v57;
    int64_t v64;
    int64_t v93; /* placeholder for void */
    int64_t v69;
    int64_t v8; /* placeholder for void */
    int64_t v98;
    int64_t v39;
    void* v13;
    bool v66;
    int64_t v81;
    void* v40;
    int64_t v68; /* placeholder for void */
    int64_t v21;
    int64_t v86;
    int64_t v16; /* placeholder for void */
    int64_t v26;
    void* v88;
    int64_t v95;
    int64_t v47;
    int64_t v35;
    int64_t v15; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 1LL;
    v6 = p_malloc(v5);
    v7 = malloc(8);
    *(int64_t*)v7 = (int64_t)v6;
    v9 = *(int64_t*)v7;
    v10 = malloc(8);
    *(void**)v10 = (void*)v9;
    v13 = *(void**)v10;
    v14 = 0LL;
    *(uint8_t*)v13 = (uint8_t)v14;
    v17 = *(int64_t*)v7;
    p_call_stack_pop();
    return v17;
  bb2:
    goto bb3;
  bb3:
    v20 = 0LL;
    v21 = (int64_t)p_str_length((void*)v20, v1);
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v21;
    v24 = *(int64_t*)v22;
    v25 = 1LL;
    v26 = v24 + v25;
    v27 = p_malloc(v26);
    v28 = malloc(8);
    *(int64_t*)v28 = (int64_t)v27;
    v30 = 0LL;
    v31 = malloc(8);
    *(int64_t*)v31 = (int64_t)v30;
    goto bb4;
  bb4:
    v34 = *(int64_t*)v31;
    v35 = *(int64_t*)v22;
    v36 = v34 < v35;
    if (v36) goto bb5;
    else goto bb6;
  bb5:
    v38 = *(int64_t*)v31;
    v39 = v1 + v38;
    v40 = malloc(8);
    *(int64_t*)v40 = (int64_t)v39;
    v42 = *(int64_t*)v28;
    v43 = *(int64_t*)v31;
    v44 = v42 + v43;
    v45 = malloc(8);
    *(int64_t*)v45 = (int64_t)v44;
    v47 = *(int64_t*)v40;
    v48 = malloc(8);
    *(void**)v48 = (void*)v47;
    v50 = *(int64_t*)v45;
    v51 = malloc(8);
    *(void**)v51 = (void*)v50;
    v53 = 0LL;
    v54 = malloc(8);
    *(int64_t*)v54 = (int64_t)v53;
    v57 = *(void**)v48;
    v58 = *(uint8_t*)v57;
    *(int64_t*)v54 = (int64_t)v58;
    v61 = *(int64_t*)v54;
    v62 = 65LL;
    v63 = v61 >= v62;
    v64 = *(int64_t*)v54;
    v65 = 90LL;
    v66 = v64 <= v65;
    v67 = v63 && v66;
    if (v67) goto bb7;
    else goto bb8;
  bb6:
    v85 = *(int64_t*)v28;
    v86 = *(int64_t*)v22;
    v87 = v85 + v86;
    v88 = malloc(8);
    *(int64_t*)v88 = (int64_t)v87;
    v90 = *(int64_t*)v88;
    v91 = malloc(8);
    *(void**)v91 = (void*)v90;
    v94 = *(void**)v91;
    v95 = 0LL;
    *(uint8_t*)v94 = (uint8_t)v95;
    v98 = *(int64_t*)v28;
    p_call_stack_pop();
    return v98;
  bb7:
    v69 = *(int64_t*)v54;
    v70 = 32LL;
    v71 = v69 + v70;
    *(int64_t*)v54 = (int64_t)v71;
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    v76 = *(void**)v51;
    v77 = *(int64_t*)v54;
    *(uint8_t*)v76 = (uint8_t)v77;
    v80 = *(int64_t*)v31;
    v81 = 1LL;
    v82 = v80 + v81;
    *(int64_t*)v31 = (int64_t)v82;
    goto bb4;
    return 0;
}

int64_t String_rfind(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_rfind");
    uint8_t v81;
    int64_t v51; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    void* v15;
    int64_t v10;
    int64_t v96;
    int64_t v82; /* placeholder for void */
    int64_t v3;
    int64_t v40; /* placeholder for void */
    int64_t v112; /* placeholder for void */
    int64_t v76;
    int64_t v106;
    int64_t v45; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    void* v68;
    bool v6;
    int64_t v54;
    int64_t v63;
    int64_t v42;
    bool v89;
    bool v55;
    bool v46;
    int64_t v109; /* placeholder for void */
    int64_t v64;
    void* v83;
    int64_t v16; /* placeholder for void */
    bool v30;
    int64_t v31; /* placeholder for void */
    bool v23;
    int64_t v41; /* placeholder for void */
    int64_t v18;
    int64_t v43;
    int64_t v48; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v111;
    int64_t v57;
    uint8_t v84;
    int64_t v88;
    int64_t v56; /* placeholder for void */
    void* v77;
    void* v50;
    int64_t v73;
    void* v47;
    int64_t v59;
    int64_t v105;
    int64_t v32;
    int64_t v104; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    void* v65;
    bool v100;
    int64_t v17;
    int64_t v38;
    void* v74;
    int64_t v27; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    int64_t v87;
    int64_t v12; /* placeholder for void */
    int64_t v33;
    int64_t v58;
    int64_t v99; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    int64_t v72; /* placeholder for void */
    void* v19;
    void* v39;
    int64_t v25;
    int64_t v52; /* placeholder for void */
    bool v7;
    int64_t v67;
    bool v91;
    int64_t v21;
    int64_t v95;
    int64_t v108; /* placeholder for void */
    int64_t v22;
    int64_t v14;
    int64_t v107;
    int64_t v26; /* placeholder for void */
    int64_t v36;
    int64_t v8; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    int64_t v93; /* placeholder for void */
    void* v61;
    int64_t v97;
    int64_t v53;
    int64_t v103; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    bool v4;
    int64_t v102;
    int64_t v29;
    bool v44;
    int64_t v70;
    int64_t v13;
    int64_t v49;
    int64_t v101; /* placeholder for void */
    int64_t v60;
    void* v71;
    int64_t v20; /* placeholder for void */
    int64_t v5;
    int64_t v69; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v28;
    void* v80;
    int64_t v62; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v37;
    int64_t v110;
    int64_t v9;
    int64_t v90; /* placeholder for void */
    int64_t v85; /* placeholder for void */
    v3 = 0LL;
    v4 = v1 == v3;
    v5 = 0LL;
    v6 = v2 == v5;
    v7 = v4 || v6;
    if (v7) goto bb1;
    else goto bb2;
  bb1:
    v9 = 1LL;
    v10 = -v9;
    p_call_stack_pop();
    return v10;
  bb2:
    goto bb3;
  bb3:
    v13 = 0LL;
    v14 = (int64_t)p_str_length((void*)v13, v1);
    v15 = malloc(8);
    *(int64_t*)v15 = (int64_t)v14;
    v17 = 0LL;
    v18 = (int64_t)p_str_length((void*)v17, v2);
    v19 = malloc(8);
    *(int64_t*)v19 = (int64_t)v18;
    v21 = *(int64_t*)v19;
    v22 = 0LL;
    v23 = v21 == v22;
    if (v23) goto bb4;
    else goto bb5;
  bb4:
    v25 = *(int64_t*)v15;
    p_call_stack_pop();
    return v25;
  bb5:
    goto bb6;
  bb6:
    v28 = *(int64_t*)v19;
    v29 = *(int64_t*)v15;
    v30 = v28 > v29;
    if (v30) goto bb7;
    else goto bb8;
  bb7:
    v32 = 1LL;
    v33 = -v32;
    p_call_stack_pop();
    return v33;
  bb8:
    goto bb9;
  bb9:
    v36 = *(int64_t*)v15;
    v37 = *(int64_t*)v19;
    v38 = v36 - v37;
    v39 = malloc(8);
    *(int64_t*)v39 = (int64_t)v38;
    goto bb10;
  bb10:
    v42 = *(int64_t*)v39;
    v43 = 0LL;
    v44 = v42 >= v43;
    if (v44) goto bb11;
    else goto bb12;
  bb11:
    v46 = true;
    v47 = malloc(1);
    *(bool*)v47 = v46;
    v49 = 0LL;
    v50 = malloc(8);
    *(int64_t*)v50 = (int64_t)v49;
    goto bb13;
  bb12:
    v110 = 1LL;
    v111 = -v110;
    p_call_stack_pop();
    return v111;
  bb13:
    v53 = *(int64_t*)v50;
    v54 = *(int64_t*)v19;
    v55 = v53 < v54;
    if (v55) goto bb14;
    else goto bb15;
  bb14:
    v57 = *(int64_t*)v39;
    v58 = v1 + v57;
    v59 = *(int64_t*)v50;
    v60 = v58 + v59;
    v61 = malloc(8);
    *(int64_t*)v61 = (int64_t)v60;
    v63 = *(int64_t*)v50;
    v64 = v2 + v63;
    v65 = malloc(8);
    *(int64_t*)v65 = (int64_t)v64;
    v67 = *(int64_t*)v61;
    v68 = malloc(8);
    *(void**)v68 = (void*)v67;
    v70 = *(int64_t*)v65;
    v71 = malloc(8);
    *(void**)v71 = (void*)v70;
    v73 = 0LL;
    v74 = malloc(8);
    *(int64_t*)v74 = (int64_t)v73;
    v76 = 0LL;
    v77 = malloc(8);
    *(int64_t*)v77 = (int64_t)v76;
    v80 = *(void**)v68;
    v81 = *(uint8_t*)v80;
    *(int64_t*)v74 = (int64_t)v81;
    v83 = *(void**)v71;
    v84 = *(uint8_t*)v83;
    *(int64_t*)v77 = (int64_t)v84;
    v87 = *(int64_t*)v74;
    v88 = *(int64_t*)v77;
    v89 = v87 != v88;
    if (v89) goto bb16;
    else goto bb17;
  bb15:
    v100 = *(bool*)v47;
    if (v100) goto bb19;
    else goto bb20;
  bb16:
    v91 = false;
    *(bool*)v47 = v91;
    goto bb15;
  bb17:
    goto bb18;
  bb18:
    v95 = *(int64_t*)v50;
    v96 = 1LL;
    v97 = v95 + v96;
    *(int64_t*)v50 = (int64_t)v97;
    goto bb13;
  bb19:
    v102 = *(int64_t*)v39;
    p_call_stack_pop();
    return v102;
  bb20:
    goto bb21;
  bb21:
    v105 = *(int64_t*)v39;
    v106 = 1LL;
    v107 = v105 - v106;
    *(int64_t*)v39 = (int64_t)v107;
    goto bb10;
    return 0;
}

bool char_is_whitespace(void* v0, int64_t v1) {
    p_call_stack_push("char_is_whitespace");
    bool v5;
    int64_t v10;
    bool v12;
    bool v9;
    bool v3;
    bool v11;
    bool v6;
    int64_t v4;
    int64_t v7;
    int64_t v13; /* placeholder for void */
    int64_t v2;
    bool v8;
    v2 = 32LL;
    v3 = v1 == v2;
    v4 = 9LL;
    v5 = v1 == v4;
    v6 = v3 || v5;
    v7 = 10LL;
    v8 = v1 == v7;
    v9 = v6 || v8;
    v10 = 13LL;
    v11 = v1 == v10;
    v12 = v9 || v11;
    p_call_stack_pop();
    return v12;
    return 0;
}

int64_t String_len(void* v0, int64_t v1) {
    p_call_stack_push("String_len");
    int64_t v4; /* placeholder for void */
    int64_t v3;
    int64_t v2;
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

void Option_or_else(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Option_or_else");
    int64_t v37; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v44;
    void* v15;
    void* v39;
    bool v7;
    int64_t v48; /* placeholder for void */
    void* v26;
    int64_t v20; /* placeholder for void */
    void* v17;
    int64_t v27;
    int64_t v41; /* placeholder for void */
    void* v42;
    int64_t v10;
    int64_t v23;
    void* v31;
    void* v43;
    int64_t v46; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    void* v38;
    int64_t v24; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v19;
    int64_t v12;
    void* v21;
    void* v11;
    int64_t v16; /* placeholder for void */
    int64_t v28;
    void* v36;
    int64_t v6;
    void* v9;
    void* v13;
    void* v4;
    int64_t v5;
    void* v18;
    bool v29;
    void* v33;
    int64_t v35;
    int64_t v40;
    int64_t v45; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    int64_t v32;
    int64_t v34; /* placeholder for void */
    void* v22;
    int64_t v25; /* placeholder for void */
    v4 = (char*)v2 + 0;
    v5 = *(int64_t*)v4;
    v6 = 0LL;
    v7 = v5 == v6;
    if (v7) goto bb2;
    else goto bb3;
  bb1:
    p_call_stack_pop();
    return;
  bb2:
    v9 = (char*)v2 + 8;
    v10 = *(int64_t*)v9;
    v11 = malloc(16);
    v12 = 0LL;
    v13 = (char*)v11 + 0;
    *(int64_t*)v13 = (int64_t)v12;
    v15 = (char*)v11 + 8;
    *(int64_t*)v15 = (int64_t)v10;
    v17 = (char*)v11 + 0;
    v18 = (char*)v0 + 0;
    v19 = *(int64_t*)v17;
    *(int64_t*)v18 = (int64_t)v19;
    v21 = (char*)v11 + 8;
    v22 = (char*)v0 + 8;
    v23 = *(int64_t*)v21;
    *(int64_t*)v22 = (int64_t)v23;
    p_call_stack_pop();
    return;
  bb3:
    v26 = (char*)v2 + 0;
    v27 = *(int64_t*)v26;
    v28 = 1LL;
    v29 = v27 == v28;
    if (v29) goto bb4;
    else goto bb5;
  bb4:
    v31 = malloc(16);
    v32 = 0LL;
    v33 = (char*)v31 + 0;
    *(int64_t*)v33 = (int64_t)v32;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "Vec_new") == 0) {
            v35 = Vec_new((void*)v3);
        } else if (strcmp(_fn_name, "HashMap_new") == 0) {
            v35 = HashMap_new((void*)v3);
        } else { abort(); }
    }
    v36 = (char*)v31 + 8;
    *(int64_t*)v36 = (int64_t)v35;
    v38 = (char*)v31 + 0;
    v39 = (char*)v0 + 0;
    v40 = *(int64_t*)v38;
    *(int64_t*)v39 = (int64_t)v40;
    v42 = (char*)v31 + 8;
    v43 = (char*)v0 + 8;
    v44 = *(int64_t*)v42;
    *(int64_t*)v43 = (int64_t)v44;
    p_call_stack_pop();
    return;
  bb5:
    goto bb1;
    return;
}

int64_t ndim(void* v0, void* v1) {
    p_call_stack_push("ndim");
    int64_t v3; /* placeholder for void */
    int64_t v2;
    v2 = (int64_t)p_tensor_ndim(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t size(void* v0, void* v1) {
    p_call_stack_push("size");
    int64_t v3; /* placeholder for void */
    int64_t v2;
    v2 = (int64_t)p_tensor_size(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t Result_expect(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_expect");
    int64_t v5;
    void* v8;
    void* v11;
    int64_t v17;
    int64_t v21;
    int64_t v22; /* placeholder for void */
    void* v3;
    int64_t v7; /* placeholder for void */
    int64_t v4;
    int64_t v13;
    bool v6;
    int64_t v15; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v12;
    int64_t v20; /* placeholder for void */
    bool v14;
    void* v16;
    int64_t v9;
    int64_t v18; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    v21 = 0LL;
    p_call_stack_pop();
    return v21;
  bb2:
    v8 = (char*)v1 + 8;
    v9 = *(int64_t*)v8;
    p_call_stack_pop();
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
    p_panic(v2);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

int64_t Option_unwrap(void* v0, void* v1) {
    p_call_stack_push("Option_unwrap");
    int64_t v8;
    int64_t v6; /* placeholder for void */
    bool v5;
    void* v7;
    bool v13;
    void* v2;
    void* v10;
    void* v15;
    int64_t v16; /* placeholder for void */
    int64_t v12;
    int64_t v18; /* placeholder for void */
    int64_t v19;
    int64_t v20; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v11;
    int64_t v4;
    int64_t v3;
    int64_t v14; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v19 = 0LL;
    p_call_stack_pop();
    return v19;
  bb2:
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    p_call_stack_pop();
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

int64_t Vec_remove(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("Vec_remove");
    int64_t v96; /* placeholder for void */
    int64_t v38;
    int64_t v43; /* placeholder for void */
    void* v25;
    int64_t v41;
    void* v9;
    int64_t v95; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    int64_t v64;
    void* v7;
    int64_t v20; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    void* v39;
    int64_t v92;
    int64_t v102; /* placeholder for void */
    void* v5;
    void* v11;
    int64_t v17; /* placeholder for void */
    void* v29;
    void* v78;
    int64_t v58;
    int64_t v99;
    void* v84;
    int64_t v57;
    void* v52;
    void* v66;
    int64_t v36;
    int64_t v76; /* placeholder for void */
    int64_t v22;
    void* v86;
    int64_t v65;
    int64_t v70;
    int64_t v93;
    int64_t v98;
    int64_t v35;
    int64_t v56;
    bool v59;
    int64_t v105; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v63;
    int64_t v69;
    int64_t v32;
    void* v75;
    bool v23;
    int64_t v6; /* placeholder for void */
    int64_t v87; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v108; /* placeholder for void */
    void* v103;
    int64_t v30; /* placeholder for void */
    void* v88;
    int64_t v82; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    int64_t v49;
    void* v100;
    int64_t v97;
    int64_t v12;
    int64_t v37;
    int64_t v50; /* placeholder for void */
    int64_t v68;
    int64_t v79; /* placeholder for void */
    int64_t v104;
    int64_t v73;
    int64_t v19; /* placeholder for void */
    int64_t v4;
    void* v42;
    int64_t v3;
    int64_t v53; /* placeholder for void */
    int64_t v74;
    int64_t v80;
    int64_t v14; /* placeholder for void */
    int64_t v15;
    int64_t v71;
    void* v81;
    int64_t v83; /* placeholder for void */
    void* v48;
    int64_t v40; /* placeholder for void */
    int64_t v85;
    int64_t v94;
    int64_t v107;
    int64_t v21; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    int64_t v77;
    int64_t v26; /* placeholder for void */
    void* v31;
    void* v45;
    int64_t v89;
    int64_t v90; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v44;
    int64_t v34; /* placeholder for void */
    int64_t v55;
    int64_t v62;
    void* v18;
    int64_t v67; /* placeholder for void */
    bool v16;
    int64_t v24; /* placeholder for void */
    int64_t v72;
    int64_t v101; /* placeholder for void */
    int64_t v61;
    v3 = 8LL;
    v4 = v1 + v3;
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = malloc(8);
    *(void**)v7 = (void*)v1;
    v9 = malloc(8);
    v11 = *(void**)v5;
    v12 = *(int64_t*)v11;
    *(int64_t*)v9 = (int64_t)v12;
    v15 = 0LL;
    v16 = v2 < v15;
    if (v16) goto bb1;
    else goto bb2;
  bb1:
    v18 = _str6;
    p_panic(v18);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v22 = *(int64_t*)v9;
    v23 = v2 >= v22;
    if (v23) goto bb4;
    else goto bb5;
  bb4:
    v25 = _str6;
    p_panic(v25);
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v29 = malloc(8);
    v31 = *(void**)v7;
    v32 = *(int64_t*)v31;
    *(int64_t*)v29 = (int64_t)v32;
    v35 = *(int64_t*)v29;
    v36 = 8LL;
    v37 = v2 * v36;
    v38 = v35 + v37;
    v39 = malloc(8);
    *(int64_t*)v39 = (int64_t)v38;
    v41 = *(int64_t*)v39;
    v42 = malloc(8);
    *(void**)v42 = (void*)v41;
    v44 = 0LL;
    v45 = malloc(8);
    *(int64_t*)v45 = (int64_t)v44;
    v48 = *(void**)v42;
    v49 = *(int64_t*)v48;
    *(int64_t*)v45 = (int64_t)v49;
    v52 = malloc(8);
    *(int64_t*)v52 = (int64_t)v2;
    goto bb7;
  bb7:
    v55 = *(int64_t*)v52;
    v56 = *(int64_t*)v9;
    v57 = 1LL;
    v58 = v56 - v57;
    v59 = v55 < v58;
    if (v59) goto bb8;
    else goto bb9;
  bb8:
    v61 = *(int64_t*)v29;
    v62 = *(int64_t*)v52;
    v63 = 8LL;
    v64 = v62 * v63;
    v65 = v61 + v64;
    v66 = malloc(8);
    *(int64_t*)v66 = (int64_t)v65;
    v68 = *(int64_t*)v29;
    v69 = *(int64_t*)v52;
    v70 = 1LL;
    v71 = v69 + v70;
    v72 = 8LL;
    v73 = v71 * v72;
    v74 = v68 + v73;
    v75 = malloc(8);
    *(int64_t*)v75 = (int64_t)v74;
    v77 = *(int64_t*)v66;
    v78 = malloc(8);
    *(void**)v78 = (void*)v77;
    v80 = *(int64_t*)v75;
    v81 = malloc(8);
    *(void**)v81 = (void*)v80;
    v84 = *(void**)v81;
    v85 = *(int64_t*)v84;
    v86 = malloc(8);
    *(int64_t*)v86 = (int64_t)v85;
    v88 = *(void**)v78;
    v89 = *(int64_t*)v86;
    *(int64_t*)v88 = (int64_t)v89;
    v92 = *(int64_t*)v52;
    v93 = 1LL;
    v94 = v92 + v93;
    *(int64_t*)v52 = (int64_t)v94;
    goto bb7;
  bb9:
    v97 = *(int64_t*)v9;
    v98 = 1LL;
    v99 = v97 - v98;
    v100 = malloc(8);
    *(int64_t*)v100 = (int64_t)v99;
    v103 = *(void**)v5;
    v104 = *(int64_t*)v100;
    *(int64_t*)v103 = (int64_t)v104;
    v107 = *(int64_t*)v45;
    p_call_stack_pop();
    return v107;
    return 0;
}

void* str_bytes(void* v0, void* v1) {
    p_call_stack_push("str_bytes");
    int64_t v27; /* placeholder for void */
    int64_t v14;
    int64_t v17; /* placeholder for void */
    void* v11;
    int64_t v38;
    int64_t v40; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    void* v26;
    int64_t v43; /* placeholder for void */
    void* v33;
    int64_t v13; /* placeholder for void */
    int64_t v34;
    bool v16;
    int64_t v2;
    void* v42;
    uint8_t v30;
    int64_t v37;
    int64_t v22;
    int64_t v6;
    int64_t v41; /* placeholder for void */
    void* v3;
    int64_t v24; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    void* v8;
    int64_t v9; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v15;
    int64_t v18;
    void* v23;
    int64_t v25;
    int64_t v28; /* placeholder for void */
    void* v20;
    int64_t v31; /* placeholder for void */
    void* v4;
    int64_t v35;
    int64_t v36; /* placeholder for void */
    int64_t v39;
    int64_t v7;
    void* v29;
    int64_t v10;
    int64_t v19;
    v2 = 0LL;
    v3 = (void*)Vec_new((void*)v2);
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = 0LL;
    v7 = (int64_t)String_len((void*)v6, (int64_t)v1);
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    v10 = 0LL;
    v11 = malloc(8);
    *(int64_t*)v11 = (int64_t)v10;
    goto bb1;
  bb1:
    v14 = *(int64_t*)v11;
    v15 = *(int64_t*)v8;
    v16 = v14 < v15;
    if (v16) goto bb2;
    else goto bb3;
  bb2:
    v18 = *(int64_t*)v11;
    v19 = (int64_t)v1 + v18;
    v20 = malloc(8);
    *(int64_t*)v20 = (int64_t)v19;
    v22 = *(int64_t*)v20;
    v23 = malloc(8);
    *(void**)v23 = (void*)v22;
    v25 = 0LL;
    v26 = malloc(8);
    *(int64_t*)v26 = (int64_t)v25;
    v29 = *(void**)v23;
    v30 = *(uint8_t*)v29;
    *(int64_t*)v26 = (int64_t)v30;
    v33 = *(void**)v4;
    v34 = *(int64_t*)v26;
    v35 = 0LL;
    Vec_push((void*)v35, (int64_t)v33, v34);
    v37 = *(int64_t*)v11;
    v38 = 1LL;
    v39 = v37 + v38;
    *(int64_t*)v11 = (int64_t)v39;
    goto bb1;
  bb3:
    v42 = *(void**)v4;
    p_call_stack_pop();
    return v42;
    return 0;
}

int64_t Option_unwrap_or(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Option_unwrap_or");
    int64_t v4;
    void* v8;
    int64_t v7; /* placeholder for void */
    int64_t v13;
    bool v6;
    int64_t v12;
    void* v3;
    int64_t v10; /* placeholder for void */
    bool v14;
    int64_t v18;
    int64_t v19; /* placeholder for void */
    int64_t v9;
    int64_t v15; /* placeholder for void */
    void* v11;
    int64_t v17; /* placeholder for void */
    int64_t v5;
    int64_t v16; /* placeholder for void */
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    v18 = 0LL;
    p_call_stack_pop();
    return v18;
  bb2:
    v8 = (char*)v1 + 8;
    v9 = *(int64_t*)v8;
    p_call_stack_pop();
    return v9;
  bb3:
    v11 = (char*)v1 + 0;
    v12 = *(int64_t*)v11;
    v13 = 1LL;
    v14 = v12 == v13;
    if (v14) goto bb4;
    else goto bb5;
  bb4:
    p_call_stack_pop();
    return v2;
  bb5:
    goto bb1;
    return 0;
}

void ByteBuffer_write_bytes(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("ByteBuffer_write_bytes");
    int64_t v10; /* placeholder for void */
    void* v19;
    bool v9;
    void* v13;
    int64_t v17; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    uint8_t v23;
    int64_t v30;
    int64_t v34; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v15;
    int64_t v24; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v18;
    int64_t v11;
    int64_t v14; /* placeholder for void */
    int64_t v29;
    int64_t v33; /* placeholder for void */
    int64_t v8;
    void* v5;
    int64_t v4;
    void* v16;
    int64_t v20; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v26;
    int64_t v27;
    int64_t v31;
    int64_t v12;
    void* v22;
    int64_t v21; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    v4 = 0LL;
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    goto bb1;
  bb1:
    v8 = *(int64_t*)v5;
    v9 = v8 < v3;
    if (v9) goto bb2;
    else goto bb3;
  bb2:
    v11 = *(int64_t*)v5;
    v12 = v2 + v11;
    v13 = malloc(8);
    *(int64_t*)v13 = (int64_t)v12;
    v15 = *(int64_t*)v13;
    v16 = malloc(8);
    *(void**)v16 = (void*)v15;
    v18 = 0LL;
    v19 = malloc(8);
    *(int64_t*)v19 = (int64_t)v18;
    v22 = *(void**)v16;
    v23 = *(uint8_t*)v22;
    *(int64_t*)v19 = (int64_t)v23;
    v26 = *(int64_t*)v19;
    v27 = 0LL;
    ByteBuffer_write_byte((void*)v27, v1, v26);
    v29 = *(int64_t*)v5;
    v30 = 1LL;
    v31 = v29 + v30;
    *(int64_t*)v5 = (int64_t)v31;
    goto bb1;
  bb3:
    p_call_stack_pop();
    return;
    return;
}

int64_t Vec_new(void* v0) {
    p_call_stack_push("Vec_new");
    int64_t v10;
    int64_t v7; /* placeholder for void */
    int64_t v9;
    int64_t v8;
    void* v19;
    int64_t v20;
    int64_t v21; /* placeholder for void */
    void* v25;
    int64_t v28; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v1;
    int64_t v26;
    void* v3;
    void* v6;
    int64_t v15;
    int64_t v18; /* placeholder for void */
    int64_t v14;
    int64_t v5;
    int64_t v23;
    int64_t v24; /* placeholder for void */
    int64_t v13;
    int64_t v4; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    void* v22;
    int64_t v29;
    void* v16;
    void* v2;
    void* v11;
    int64_t v17; /* placeholder for void */
    v1 = 24LL;
    v2 = p_malloc(v1);
    v3 = malloc(8);
    *(int64_t*)v3 = (int64_t)v2;
    v5 = *(int64_t*)v3;
    v6 = malloc(8);
    *(void**)v6 = (void*)v5;
    v8 = *(int64_t*)v3;
    v9 = 8LL;
    v10 = v8 + v9;
    v11 = malloc(8);
    *(void**)v11 = (void*)v10;
    v13 = *(int64_t*)v3;
    v14 = 16LL;
    v15 = v13 + v14;
    v16 = malloc(8);
    *(void**)v16 = (void*)v15;
    v19 = *(void**)v6;
    v20 = 0LL;
    *(int64_t*)v19 = (int64_t)v20;
    v22 = *(void**)v11;
    v23 = 0LL;
    *(int64_t*)v22 = (int64_t)v23;
    v25 = *(void**)v16;
    v26 = 0LL;
    *(int64_t*)v25 = (int64_t)v26;
    v29 = *(int64_t*)v3;
    p_call_stack_pop();
    return v29;
    return 0;
}

bool HashMap_contains(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_contains");
    int64_t v46;
    int64_t v4;
    int64_t v6; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    void* v41;
    int64_t v37;
    int64_t v40;
    int64_t v3;
    int64_t v58;
    int64_t v34; /* placeholder for void */
    void* v44;
    int64_t v7;
    void* v8;
    int64_t v10; /* placeholder for void */
    void* v47;
    int64_t v25; /* placeholder for void */
    int64_t v28;
    int64_t v16;
    bool v17;
    void* v24;
    void* v26;
    int64_t v30; /* placeholder for void */
    int64_t v57;
    int64_t v62; /* placeholder for void */
    int64_t v43;
    int64_t v20; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    bool v56;
    int64_t v12;
    int64_t v59;
    void* v11;
    int64_t v48; /* placeholder for void */
    int64_t v15;
    void* v32;
    int64_t v9; /* placeholder for void */
    int64_t v38;
    int64_t v23;
    void* v29;
    int64_t v45; /* placeholder for void */
    int64_t v39;
    int64_t v49; /* placeholder for void */
    int64_t v51;
    int64_t v55;
    int64_t v54;
    void* v5;
    int64_t v33;
    bool v60;
    bool v61;
    bool v19;
    int64_t v42; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    void* v50;
    int64_t v36;
    int64_t v22;
    int64_t v31; /* placeholder for void */
    v3 = 24LL;
    v4 = v1 + v3;
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = 0LL;
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    v11 = *(void**)v5;
    v12 = *(int64_t*)v11;
    *(int64_t*)v8 = (int64_t)v12;
    v15 = *(int64_t*)v8;
    v16 = 0LL;
    v17 = v15 == v16;
    if (v17) goto bb1;
    else goto bb2;
  bb1:
    v19 = false;
    p_call_stack_pop();
    return v19;
  bb2:
    goto bb3;
  bb3:
    v22 = 0LL;
    v23 = (int64_t)HashMap_find_slot((void*)v22, v1, v2);
    v24 = malloc(8);
    *(int64_t*)v24 = (int64_t)v23;
    v26 = malloc(8);
    *(void**)v26 = (void*)v1;
    v28 = 0LL;
    v29 = malloc(8);
    *(int64_t*)v29 = (int64_t)v28;
    v32 = *(void**)v26;
    v33 = *(int64_t*)v32;
    *(int64_t*)v29 = (int64_t)v33;
    v36 = *(int64_t*)v29;
    v37 = *(int64_t*)v24;
    v38 = 8LL;
    v39 = v37 * v38;
    v40 = v36 + v39;
    v41 = malloc(8);
    *(int64_t*)v41 = (int64_t)v40;
    v43 = *(int64_t*)v41;
    v44 = malloc(8);
    *(void**)v44 = (void*)v43;
    v46 = 0LL;
    v47 = malloc(8);
    *(int64_t*)v47 = (int64_t)v46;
    v50 = *(void**)v44;
    v51 = *(int64_t*)v50;
    *(int64_t*)v47 = (int64_t)v51;
    v54 = *(int64_t*)v47;
    v55 = 0LL;
    v56 = v54 != v55;
    v57 = *(int64_t*)v47;
    v58 = 1LL;
    v59 = -v58;
    v60 = v57 != v59;
    v61 = v56 && v60;
    p_call_stack_pop();
    return v61;
    return 0;
}

int64_t String_trim(void* v0, int64_t v1) {
    p_call_stack_push("String_trim");
    int64_t v29; /* placeholder for void */
    int64_t v93;
    void* v28;
    int64_t v86; /* placeholder for void */
    uint8_t v37;
    int64_t v56; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    bool v116;
    bool v43;
    int64_t v153;
    int64_t v57;
    void* v155;
    int64_t v52;
    int64_t v117;
    int64_t v65;
    int64_t v70; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v166; /* placeholder for void */
    int64_t v175;
    bool v112;
    void* v189;
    void* v183;
    int64_t v94;
    int64_t v110;
    int64_t v75; /* placeholder for void */
    void* v77;
    void* v30;
    int64_t v192; /* placeholder for void */
    int64_t v136; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    void* v10;
    void* v171;
    int64_t v69; /* placeholder for void */
    int64_t v142; /* placeholder for void */
    void* v139;
    bool v151;
    bool v120;
    void* v161;
    void* v103;
    int64_t v177;
    uint8_t v68;
    int64_t v18; /* placeholder for void */
    int64_t v100; /* placeholder for void */
    int64_t v45;
    int64_t v53;
    int64_t v190;
    int64_t v99; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v194; /* placeholder for void */
    int64_t v133;
    int64_t v20;
    void* v84;
    int64_t v4; /* placeholder for void */
    void* v98;
    bool v111;
    int64_t v174;
    void* v36;
    int64_t v79; /* placeholder for void */
    int64_t v88;
    int64_t v159; /* placeholder for void */
    int64_t v173; /* placeholder for void */
    int64_t v127;
    void* v33;
    int64_t v26; /* placeholder for void */
    uint8_t v102;
    int64_t v178;
    int64_t v157;
    int64_t v160; /* placeholder for void */
    int64_t v109;
    int64_t v59;
    int64_t v44;
    void* v172;
    int64_t v167; /* placeholder for void */
    int64_t v113;
    int64_t v124;
    int64_t v126;
    int64_t v24;
    void* v95;
    int64_t v17;
    int64_t v104; /* placeholder for void */
    void* v25;
    int64_t v122;
    int64_t v92;
    void* v78;
    int64_t v83; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    bool v119;
    int64_t v150;
    int64_t v2;
    int64_t v48;
    int64_t v80;
    int64_t v134;
    void* v147;
    int64_t v32;
    int64_t v162;
    int64_t v168;
    int64_t v149;
    int64_t v72;
    int64_t v39; /* placeholder for void */
    int64_t v85;
    int64_t v121; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v125; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v131; /* placeholder for void */
    int64_t v180;
    int64_t v118;
    void* v13;
    bool v74;
    bool v51;
    int64_t v138; /* placeholder for void */
    int64_t v182;
    void* v186;
    int64_t v187; /* placeholder for void */
    int64_t v193;
    void* v38;
    bool v47;
    int64_t v105; /* placeholder for void */
    int64_t v170;
    int64_t v107;
    int64_t v9;
    int64_t v40; /* placeholder for void */
    int64_t v62;
    bool v54;
    int64_t v21;
    void* v101;
    int64_t v63;
    int64_t v148; /* placeholder for void */
    int64_t v185;
    void* v6;
    int64_t v12; /* placeholder for void */
    int64_t v114;
    int64_t v130; /* placeholder for void */
    int64_t v14;
    int64_t v19; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    bool v50;
    int64_t v64; /* placeholder for void */
    int64_t v41;
    int64_t v135;
    int64_t v11; /* placeholder for void */
    int64_t v42;
    int64_t v156; /* placeholder for void */
    int64_t v164; /* placeholder for void */
    int64_t v137;
    int64_t v163; /* placeholder for void */
    int64_t v191; /* placeholder for void */
    int64_t v179; /* placeholder for void */
    uint8_t v140;
    void* v22;
    int64_t v87; /* placeholder for void */
    int64_t v58;
    int64_t v144;
    int64_t v89; /* placeholder for void */
    void* v158;
    int64_t v16; /* placeholder for void */
    int64_t v106;
    int64_t v123;
    int64_t v61;
    int64_t v76;
    void* v81;
    int64_t v49;
    int64_t v35; /* placeholder for void */
    int64_t v91;
    int64_t v97;
    void* v7;
    int64_t v15; /* placeholder for void */
    int64_t v132;
    int64_t v143; /* placeholder for void */
    bool v46;
    bool v55;
    int64_t v73;
    int64_t v146;
    bool v3;
    bool v115;
    int64_t v145;
    int64_t v152; /* placeholder for void */
    int64_t v169;
    int64_t v27;
    int64_t v176;
    int64_t v165;
    int64_t v181;
    int64_t v71; /* placeholder for void */
    bool v108;
    void* v67;
    int64_t v141; /* placeholder for void */
    int64_t v184; /* placeholder for void */
    int64_t v188; /* placeholder for void */
    int64_t v129; /* placeholder for void */
    bool v128;
    void* v154;
    int64_t v5;
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 1LL;
    v6 = p_malloc(v5);
    v7 = malloc(8);
    *(int64_t*)v7 = (int64_t)v6;
    v9 = *(int64_t*)v7;
    v10 = malloc(8);
    *(void**)v10 = (void*)v9;
    v13 = *(void**)v10;
    v14 = 0LL;
    *(uint8_t*)v13 = (uint8_t)v14;
    v17 = *(int64_t*)v7;
    p_call_stack_pop();
    return v17;
  bb2:
    goto bb3;
  bb3:
    v20 = 0LL;
    v21 = (int64_t)p_str_length((void*)v20, v1);
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v21;
    v24 = 0LL;
    v25 = malloc(8);
    *(int64_t*)v25 = (int64_t)v24;
    v27 = *(int64_t*)v22;
    v28 = malloc(8);
    *(int64_t*)v28 = (int64_t)v27;
    v30 = malloc(8);
    *(int64_t*)v30 = (int64_t)v1;
    v32 = *(int64_t*)v30;
    v33 = malloc(8);
    *(void**)v33 = (void*)v32;
    v36 = *(void**)v33;
    v37 = *(uint8_t*)v36;
    v38 = malloc(8);
    *(int64_t*)v38 = (int64_t)v37;
    goto bb4;
  bb4:
    v41 = *(int64_t*)v38;
    v42 = 32LL;
    v43 = v41 == v42;
    v44 = *(int64_t*)v38;
    v45 = 9LL;
    v46 = v44 == v45;
    v47 = v43 || v46;
    v48 = *(int64_t*)v38;
    v49 = 10LL;
    v50 = v48 == v49;
    v51 = v47 || v50;
    v52 = *(int64_t*)v38;
    v53 = 13LL;
    v54 = v52 == v53;
    v55 = v51 || v54;
    if (v55) goto bb5;
    else goto bb6;
  bb5:
    v57 = *(int64_t*)v25;
    v58 = 1LL;
    v59 = v57 + v58;
    *(int64_t*)v25 = (int64_t)v59;
    v61 = *(int64_t*)v30;
    v62 = 1LL;
    v63 = v61 + v62;
    *(int64_t*)v30 = (int64_t)v63;
    v65 = *(int64_t*)v30;
    *(void**)v33 = (void*)v65;
    v67 = *(void**)v33;
    v68 = *(uint8_t*)v67;
    *(int64_t*)v38 = (int64_t)v68;
    goto bb4;
  bb6:
    v72 = *(int64_t*)v25;
    v73 = *(int64_t*)v28;
    v74 = v72 >= v73;
    if (v74) goto bb7;
    else goto bb8;
  bb7:
    v76 = 1LL;
    v77 = p_malloc(v76);
    v78 = malloc(8);
    *(int64_t*)v78 = (int64_t)v77;
    v80 = *(int64_t*)v78;
    v81 = malloc(8);
    *(void**)v81 = (void*)v80;
    v84 = *(void**)v81;
    v85 = 0LL;
    *(uint8_t*)v84 = (uint8_t)v85;
    v88 = *(int64_t*)v78;
    p_call_stack_pop();
    return v88;
  bb8:
    goto bb9;
  bb9:
    v91 = *(int64_t*)v28;
    v92 = v1 + v91;
    v93 = 1LL;
    v94 = v92 - v93;
    v95 = malloc(8);
    *(int64_t*)v95 = (int64_t)v94;
    v97 = *(int64_t*)v95;
    v98 = malloc(8);
    *(void**)v98 = (void*)v97;
    v101 = *(void**)v98;
    v102 = *(uint8_t*)v101;
    v103 = malloc(8);
    *(int64_t*)v103 = (int64_t)v102;
    goto bb10;
  bb10:
    v106 = *(int64_t*)v103;
    v107 = 32LL;
    v108 = v106 == v107;
    v109 = *(int64_t*)v103;
    v110 = 9LL;
    v111 = v109 == v110;
    v112 = v108 || v111;
    v113 = *(int64_t*)v103;
    v114 = 10LL;
    v115 = v113 == v114;
    v116 = v112 || v115;
    v117 = *(int64_t*)v103;
    v118 = 13LL;
    v119 = v117 == v118;
    v120 = v116 || v119;
    if (v120) goto bb11;
    else goto bb12;
  bb11:
    v122 = *(int64_t*)v28;
    v123 = 1LL;
    v124 = v122 - v123;
    *(int64_t*)v28 = (int64_t)v124;
    v126 = *(int64_t*)v28;
    v127 = *(int64_t*)v25;
    v128 = v126 <= v127;
    if (v128) goto bb13;
    else goto bb14;
  bb12:
    v144 = *(int64_t*)v28;
    v145 = *(int64_t*)v25;
    v146 = v144 - v145;
    v147 = malloc(8);
    *(int64_t*)v147 = (int64_t)v146;
    v149 = *(int64_t*)v147;
    v150 = 0LL;
    v151 = v149 <= v150;
    if (v151) goto bb16;
    else goto bb17;
  bb13:
    goto bb12;
  bb14:
    goto bb15;
  bb15:
    v132 = *(int64_t*)v28;
    v133 = v1 + v132;
    v134 = 1LL;
    v135 = v133 - v134;
    *(int64_t*)v95 = (int64_t)v135;
    v137 = *(int64_t*)v95;
    *(void**)v98 = (void*)v137;
    v139 = *(void**)v98;
    v140 = *(uint8_t*)v139;
    *(int64_t*)v103 = (int64_t)v140;
    goto bb10;
  bb16:
    v153 = 1LL;
    v154 = p_malloc(v153);
    v155 = malloc(8);
    *(int64_t*)v155 = (int64_t)v154;
    v157 = *(int64_t*)v155;
    v158 = malloc(8);
    *(void**)v158 = (void*)v157;
    v161 = *(void**)v158;
    v162 = 0LL;
    *(uint8_t*)v161 = (uint8_t)v162;
    v165 = *(int64_t*)v155;
    p_call_stack_pop();
    return v165;
  bb17:
    goto bb18;
  bb18:
    v168 = *(int64_t*)v147;
    v169 = 1LL;
    v170 = v168 + v169;
    v171 = p_malloc(v170);
    v172 = malloc(8);
    *(int64_t*)v172 = (int64_t)v171;
    v174 = *(int64_t*)v172;
    v175 = *(int64_t*)v25;
    v176 = v1 + v175;
    v177 = *(int64_t*)v147;
    v178 = 0LL;
    p_str_copy_n((void*)v178, v174, v176, v177);
    v180 = *(int64_t*)v172;
    v181 = *(int64_t*)v147;
    v182 = v180 + v181;
    v183 = malloc(8);
    *(int64_t*)v183 = (int64_t)v182;
    v185 = *(int64_t*)v183;
    v186 = malloc(8);
    *(void**)v186 = (void*)v185;
    v189 = *(void**)v186;
    v190 = 0LL;
    *(uint8_t*)v189 = (uint8_t)v190;
    v193 = *(int64_t*)v172;
    p_call_stack_pop();
    return v193;
    return 0;
}

void* to_json(void* v0, void* v1) {
    p_call_stack_push("to_json");
    void* v2;
    int64_t v3; /* placeholder for void */
    v2 = p_tensor_to_json(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t String_byte_at(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_byte_at");
    int64_t v8; /* placeholder for void */
    int64_t v6;
    int64_t v16;
    int64_t v29; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    void* v34;
    int64_t v24;
    int64_t v36; /* placeholder for void */
    int64_t v3;
    void* v25;
    void* v31;
    int64_t v20;
    int64_t v26; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v21;
    void* v28;
    int64_t v27;
    uint8_t v35;
    bool v18;
    int64_t v39; /* placeholder for void */
    int64_t v7;
    void* v12;
    int64_t v33; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    bool v17;
    int64_t v19; /* placeholder for void */
    int64_t v14;
    int64_t v32; /* placeholder for void */
    int64_t v30;
    bool v4;
    bool v15;
    int64_t v37; /* placeholder for void */
    int64_t v10;
    int64_t v11;
    int64_t v38;
    v3 = 0LL;
    v4 = v1 == v3;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    v6 = 1LL;
    v7 = -v6;
    p_call_stack_pop();
    return v7;
  bb2:
    goto bb3;
  bb3:
    v10 = 0LL;
    v11 = (int64_t)p_str_length((void*)v10, v1);
    v12 = malloc(8);
    *(int64_t*)v12 = (int64_t)v11;
    v14 = 0LL;
    v15 = v2 < v14;
    v16 = *(int64_t*)v12;
    v17 = v2 >= v16;
    v18 = v15 || v17;
    if (v18) goto bb4;
    else goto bb5;
  bb4:
    v20 = 1LL;
    v21 = -v20;
    p_call_stack_pop();
    return v21;
  bb5:
    goto bb6;
  bb6:
    v24 = v1 + v2;
    v25 = malloc(8);
    *(int64_t*)v25 = (int64_t)v24;
    v27 = *(int64_t*)v25;
    v28 = malloc(8);
    *(void**)v28 = (void*)v27;
    v30 = 0LL;
    v31 = malloc(8);
    *(int64_t*)v31 = (int64_t)v30;
    v34 = *(void**)v28;
    v35 = *(uint8_t*)v34;
    *(int64_t*)v31 = (int64_t)v35;
    v38 = *(int64_t*)v31;
    p_call_stack_pop();
    return v38;
    return 0;
}

int64_t String_char_count(void* v0, int64_t v1) {
    p_call_stack_push("String_char_count");
    int64_t v2;
    int64_t v3;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t String_split(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_split");
    int64_t v36; /* placeholder for void */
    int64_t v55;
    bool v89;
    int64_t v12; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    void* v72;
    int64_t v121; /* placeholder for void */
    int64_t v4;
    bool v11;
    int64_t v17;
    int64_t v30;
    bool v44;
    int64_t v3;
    bool v10;
    int64_t v167; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    int64_t v186; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    uint8_t v82;
    int64_t v136;
    int64_t v7;
    int64_t v141;
    void* v5;
    void* v120;
    int64_t v97; /* placeholder for void */
    int64_t v27;
    int64_t v176; /* placeholder for void */
    int64_t v114;
    int64_t v134;
    int64_t v188; /* placeholder for void */
    int64_t v57;
    void* v108;
    void* v177;
    int64_t v70; /* placeholder for void */
    int64_t v153;
    int64_t v73; /* placeholder for void */
    void* v150;
    int64_t v24;
    int64_t v38;
    int64_t v39;
    bool v22;
    int64_t v102;
    int64_t v64; /* placeholder for void */
    int64_t v107;
    void* v109;
    int64_t v49; /* placeholder for void */
    int64_t v131;
    int64_t v132;
    int64_t v148;
    int64_t v163;
    bool v98;
    int64_t v58;
    int64_t v124; /* placeholder for void */
    int64_t v116; /* placeholder for void */
    int64_t v41;
    bool v154;
    int64_t v20;
    int64_t v181;
    int64_t v47;
    int64_t v90; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    uint8_t v79;
    int64_t v183;
    void* v28;
    int64_t v122;
    int64_t v77; /* placeholder for void */
    int64_t v95;
    void* v159;
    int64_t v129; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v105;
    int64_t v143;
    int64_t v166;
    bool v8;
    int64_t v16;
    void* v35;
    int64_t v46; /* placeholder for void */
    void* v123;
    int64_t v146; /* placeholder for void */
    int64_t v147;
    int64_t v137; /* placeholder for void */
    int64_t v52;
    int64_t v26; /* placeholder for void */
    int64_t v56;
    void* v66;
    int64_t v104; /* placeholder for void */
    int64_t v115;
    int64_t v138;
    int64_t v151; /* placeholder for void */
    void* v45;
    void* v63;
    int64_t v113;
    int64_t v118;
    int64_t v100;
    int64_t v127;
    void* v48;
    int64_t v60; /* placeholder for void */
    int64_t v88; /* placeholder for void */
    int64_t v155; /* placeholder for void */
    int64_t v68;
    int64_t v110; /* placeholder for void */
    int64_t v161; /* placeholder for void */
    int64_t v165;
    int64_t v31;
    int64_t v74;
    int64_t v170;
    void* v171;
    int64_t v172; /* placeholder for void */
    void* v174;
    int64_t v175; /* placeholder for void */
    int64_t v179; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v119;
    int64_t v128; /* placeholder for void */
    int64_t v65;
    int64_t v133; /* placeholder for void */
    int64_t v145; /* placeholder for void */
    int64_t v13;
    int64_t v34;
    int64_t v187;
    int64_t v93;
    int64_t v83; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    int64_t v149;
    int64_t v111;
    int64_t v80; /* placeholder for void */
    int64_t v94;
    bool v87;
    void* v75;
    int64_t v130;
    int64_t v152;
    bool v42;
    void* v69;
    int64_t v86;
    int64_t v180; /* placeholder for void */
    void* v32;
    void* v59;
    int64_t v139; /* placeholder for void */
    int64_t v9;
    int64_t v182;
    int64_t v117;
    void* v126;
    int64_t v112;
    int64_t v50; /* placeholder for void */
    int64_t v62;
    int64_t v101;
    int64_t v158;
    int64_t v162;
    int64_t v157;
    int64_t v43; /* placeholder for void */
    int64_t v61;
    int64_t v84; /* placeholder for void */
    int64_t v135;
    int64_t v21;
    bool v53;
    int64_t v144; /* placeholder for void */
    int64_t v168;
    int64_t v142;
    int64_t v169;
    int64_t v106;
    int64_t v99; /* placeholder for void */
    int64_t v156;
    int64_t v173;
    int64_t v6; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    int64_t v71;
    int64_t v178;
    int64_t v185; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v140; /* placeholder for void */
    int64_t v40;
    void* v103;
    void* v160;
    int64_t v85;
    int64_t v51;
    int64_t v125; /* placeholder for void */
    void* v18;
    int64_t v184; /* placeholder for void */
    int64_t v164;
    void* v78;
    void* v81;
    v3 = 0LL;
    v4 = (int64_t)Vec_new((void*)v3);
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = 0LL;
    v8 = v1 == v7;
    v9 = 0LL;
    v10 = v2 == v9;
    v11 = v8 || v10;
    if (v11) goto bb1;
    else goto bb2;
  bb1:
    v13 = *(int64_t*)v5;
    p_call_stack_pop();
    return v13;
  bb2:
    goto bb3;
  bb3:
    v16 = 0LL;
    v17 = (int64_t)p_str_length((void*)v16, v2);
    v18 = malloc(8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = *(int64_t*)v18;
    v21 = 0LL;
    v22 = v20 == v21;
    if (v22) goto bb4;
    else goto bb5;
  bb4:
    v24 = *(int64_t*)v5;
    p_call_stack_pop();
    return v24;
  bb5:
    goto bb6;
  bb6:
    v27 = 0LL;
    v28 = malloc(8);
    *(int64_t*)v28 = (int64_t)v27;
    v30 = 0LL;
    v31 = (int64_t)p_str_length((void*)v30, v1);
    v32 = malloc(8);
    *(int64_t*)v32 = (int64_t)v31;
    v34 = 0LL;
    v35 = malloc(8);
    *(int64_t*)v35 = (int64_t)v34;
    goto bb7;
  bb7:
    v38 = *(int64_t*)v35;
    v39 = *(int64_t*)v32;
    v40 = *(int64_t*)v18;
    v41 = v39 - v40;
    v42 = v38 <= v41;
    if (v42) goto bb8;
    else goto bb9;
  bb8:
    v44 = true;
    v45 = malloc(1);
    *(bool*)v45 = v44;
    v47 = 0LL;
    v48 = malloc(8);
    *(int64_t*)v48 = (int64_t)v47;
    goto bb10;
  bb9:
    v147 = *(int64_t*)v32;
    v148 = *(int64_t*)v28;
    v149 = v147 - v148;
    v150 = malloc(8);
    *(int64_t*)v150 = (int64_t)v149;
    v152 = *(int64_t*)v150;
    v153 = 0LL;
    v154 = v152 >= v153;
    if (v154) goto bb19;
    else goto bb20;
  bb10:
    v51 = *(int64_t*)v48;
    v52 = *(int64_t*)v18;
    v53 = v51 < v52;
    if (v53) goto bb11;
    else goto bb12;
  bb11:
    v55 = *(int64_t*)v35;
    v56 = v1 + v55;
    v57 = *(int64_t*)v48;
    v58 = v56 + v57;
    v59 = malloc(8);
    *(int64_t*)v59 = (int64_t)v58;
    v61 = *(int64_t*)v48;
    v62 = v2 + v61;
    v63 = malloc(8);
    *(int64_t*)v63 = (int64_t)v62;
    v65 = *(int64_t*)v59;
    v66 = malloc(8);
    *(void**)v66 = (void*)v65;
    v68 = *(int64_t*)v63;
    v69 = malloc(8);
    *(void**)v69 = (void*)v68;
    v71 = 0LL;
    v72 = malloc(8);
    *(int64_t*)v72 = (int64_t)v71;
    v74 = 0LL;
    v75 = malloc(8);
    *(int64_t*)v75 = (int64_t)v74;
    v78 = *(void**)v66;
    v79 = *(uint8_t*)v78;
    *(int64_t*)v72 = (int64_t)v79;
    v81 = *(void**)v69;
    v82 = *(uint8_t*)v81;
    *(int64_t*)v75 = (int64_t)v82;
    v85 = *(int64_t*)v72;
    v86 = *(int64_t*)v75;
    v87 = v85 != v86;
    if (v87) goto bb13;
    else goto bb14;
  bb12:
    v98 = *(bool*)v45;
    if (v98) goto bb16;
    else goto bb17;
  bb13:
    v89 = false;
    *(bool*)v45 = v89;
    goto bb12;
  bb14:
    goto bb15;
  bb15:
    v93 = *(int64_t*)v48;
    v94 = 1LL;
    v95 = v93 + v94;
    *(int64_t*)v48 = (int64_t)v95;
    goto bb10;
  bb16:
    v100 = *(int64_t*)v35;
    v101 = *(int64_t*)v28;
    v102 = v100 - v101;
    v103 = malloc(8);
    *(int64_t*)v103 = (int64_t)v102;
    v105 = *(int64_t*)v103;
    v106 = 1LL;
    v107 = v105 + v106;
    v108 = p_malloc(v107);
    v109 = malloc(8);
    *(int64_t*)v109 = (int64_t)v108;
    v111 = *(int64_t*)v109;
    v112 = *(int64_t*)v28;
    v113 = v1 + v112;
    v114 = *(int64_t*)v103;
    v115 = 0LL;
    p_str_copy_n((void*)v115, v111, v113, v114);
    v117 = *(int64_t*)v109;
    v118 = *(int64_t*)v103;
    v119 = v117 + v118;
    v120 = malloc(8);
    *(int64_t*)v120 = (int64_t)v119;
    v122 = *(int64_t*)v120;
    v123 = malloc(8);
    *(void**)v123 = (void*)v122;
    v126 = *(void**)v123;
    v127 = 0LL;
    *(uint8_t*)v126 = (uint8_t)v127;
    v130 = *(int64_t*)v5;
    v131 = *(int64_t*)v109;
    v132 = 0LL;
    Vec_push((void*)v132, v130, v131);
    v134 = *(int64_t*)v35;
    v135 = *(int64_t*)v18;
    v136 = v134 + v135;
    *(int64_t*)v28 = (int64_t)v136;
    v138 = *(int64_t*)v28;
    *(int64_t*)v35 = (int64_t)v138;
    goto bb18;
  bb17:
    v141 = *(int64_t*)v35;
    v142 = 1LL;
    v143 = v141 + v142;
    *(int64_t*)v35 = (int64_t)v143;
    goto bb18;
  bb18:
    goto bb7;
  bb19:
    v156 = *(int64_t*)v150;
    v157 = 1LL;
    v158 = v156 + v157;
    v159 = p_malloc(v158);
    v160 = malloc(8);
    *(int64_t*)v160 = (int64_t)v159;
    v162 = *(int64_t*)v160;
    v163 = *(int64_t*)v28;
    v164 = v1 + v163;
    v165 = *(int64_t*)v150;
    v166 = 0LL;
    p_str_copy_n((void*)v166, v162, v164, v165);
    v168 = *(int64_t*)v160;
    v169 = *(int64_t*)v150;
    v170 = v168 + v169;
    v171 = malloc(8);
    *(int64_t*)v171 = (int64_t)v170;
    v173 = *(int64_t*)v171;
    v174 = malloc(8);
    *(void**)v174 = (void*)v173;
    v177 = *(void**)v174;
    v178 = 0LL;
    *(uint8_t*)v177 = (uint8_t)v178;
    v181 = *(int64_t*)v5;
    v182 = *(int64_t*)v160;
    v183 = 0LL;
    Vec_push((void*)v183, v181, v182);
    goto bb21;
  bb20:
    goto bb21;
  bb21:
    v187 = *(int64_t*)v5;
    p_call_stack_pop();
    return v187;
    return 0;
}

void p_str_copy_n(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("p_str_copy_n");
    int64_t v8;
    int64_t v11; /* placeholder for void */
    void* v16;
    void* v24;
    int64_t v28; /* placeholder for void */
    int64_t v12;
    int64_t v29; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v31;
    int64_t v14; /* placeholder for void */
    int64_t v15;
    int64_t v33; /* placeholder for void */
    int64_t v36;
    int64_t v35;
    int64_t v39;
    void* v6;
    int64_t v7; /* placeholder for void */
    void* v22;
    int64_t v40;
    int64_t v30;
    int64_t v34;
    void* v4;
    bool v13;
    void* v19;
    int64_t v21; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    int64_t v27;
    int64_t v38;
    int64_t v41; /* placeholder for void */
    void* v9;
    int64_t v10; /* placeholder for void */
    uint8_t v23;
    int64_t v43; /* placeholder for void */
    void* v26;
    int64_t v5; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v32;
    int64_t v18;
    int64_t v25; /* placeholder for void */
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v1;
    v6 = malloc(8);
    *(int64_t*)v6 = (int64_t)v2;
    v8 = 0LL;
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    goto bb1;
  bb1:
    v12 = *(int64_t*)v9;
    v13 = v12 < v3;
    if (v13) goto bb2;
    else goto bb3;
  bb2:
    v15 = *(int64_t*)v4;
    v16 = malloc(8);
    *(void**)v16 = (void*)v15;
    v18 = *(int64_t*)v6;
    v19 = malloc(8);
    *(void**)v19 = (void*)v18;
    v22 = *(void**)v19;
    v23 = *(uint8_t*)v22;
    v24 = malloc(8);
    *(int64_t*)v24 = (int64_t)v23;
    v26 = *(void**)v16;
    v27 = *(int64_t*)v24;
    *(uint8_t*)v26 = (uint8_t)v27;
    v30 = *(int64_t*)v4;
    v31 = 1LL;
    v32 = v30 + v31;
    *(int64_t*)v4 = (int64_t)v32;
    v34 = *(int64_t*)v6;
    v35 = 1LL;
    v36 = v34 + v35;
    *(int64_t*)v6 = (int64_t)v36;
    v38 = *(int64_t*)v9;
    v39 = 1LL;
    v40 = v38 + v39;
    *(int64_t*)v9 = (int64_t)v40;
    goto bb1;
  bb3:
    p_call_stack_pop();
    return;
    return;
}

int64_t StringBuilder_len(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_len");
    int64_t v4; /* placeholder for void */
    void* v2;
    int64_t v3;
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    p_call_stack_pop();
    return v3;
    return 0;
}

void Vec_push(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("Vec_push");
    int64_t v37; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    int64_t v72;
    int64_t v89;
    int64_t v25; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v45;
    void* v126;
    int64_t v7;
    void* v63;
    int64_t v124; /* placeholder for void */
    int64_t v128; /* placeholder for void */
    void* v73;
    bool v56;
    int64_t v62;
    int64_t v28;
    int64_t v34;
    int64_t v35;
    void* v76;
    int64_t v108; /* placeholder for void */
    int64_t v70;
    void* v81;
    int64_t v121; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v105;
    int64_t v69;
    int64_t v40;
    int64_t v80;
    int64_t v92;
    void* v17;
    int64_t v61;
    void* v23;
    int64_t v116;
    int64_t v22; /* placeholder for void */
    int64_t v27;
    int64_t v99; /* placeholder for void */
    int64_t v84;
    int64_t v57; /* placeholder for void */
    int64_t v119;
    int64_t v3;
    int64_t v114; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v131;
    int64_t v18;
    int64_t v103; /* placeholder for void */
    void* v135;
    int64_t v139; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v100; /* placeholder for void */
    void* v51;
    int64_t v77; /* placeholder for void */
    int64_t v58;
    void* v123;
    void* v11;
    void* v9;
    bool v36;
    void* v32;
    int64_t v30; /* placeholder for void */
    int64_t v107; /* placeholder for void */
    void* v120;
    int64_t v129;
    int64_t v137; /* placeholder for void */
    int64_t v38;
    int64_t v87;
    void* v14;
    void* v48;
    void* v66;
    int64_t v91; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v102;
    int64_t v86; /* placeholder for void */
    int64_t v115;
    void* v132;
    int64_t v46;
    void* v104;
    bool v29;
    int64_t v65;
    int64_t v31;
    int64_t v24;
    int64_t v6; /* placeholder for void */
    int64_t v39;
    int64_t v41; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    int64_t v60;
    int64_t v85; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    void* v15;
    int64_t v96;
    void* v13;
    int64_t v109; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v4;
    int64_t v54;
    int64_t v113; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    int64_t v130;
    int64_t v16; /* placeholder for void */
    int64_t v21;
    int64_t v127; /* placeholder for void */
    int64_t v133; /* placeholder for void */
    int64_t v59;
    int64_t v50;
    void* v83;
    void* v20;
    int64_t v117;
    int64_t v52; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    int64_t v71;
    int64_t v118;
    int64_t v33; /* placeholder for void */
    int64_t v125; /* placeholder for void */
    bool v94;
    int64_t v12; /* placeholder for void */
    int64_t v112;
    int64_t v8;
    int64_t v19; /* placeholder for void */
    int64_t v93;
    int64_t v106; /* placeholder for void */
    int64_t v138; /* placeholder for void */
    int64_t v88;
    int64_t v110; /* placeholder for void */
    void* v79;
    void* v47;
    int64_t v75;
    void* v101;
    int64_t v122;
    int64_t v78; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v44;
    int64_t v134; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v55;
    void* v111;
    int64_t v136;
    void* v5;
    int64_t v68;
    v3 = 8LL;
    v4 = v1 + v3;
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = 16LL;
    v8 = v1 + v7;
    v9 = malloc(8);
    *(void**)v9 = (void*)v8;
    v11 = malloc(8);
    *(void**)v11 = (void*)v1;
    v13 = malloc(8);
    v14 = malloc(8);
    v15 = malloc(8);
    v17 = *(void**)v5;
    v18 = *(int64_t*)v17;
    *(int64_t*)v13 = (int64_t)v18;
    v20 = *(void**)v9;
    v21 = *(int64_t*)v20;
    *(int64_t*)v14 = (int64_t)v21;
    v23 = *(void**)v11;
    v24 = *(int64_t*)v23;
    *(int64_t*)v15 = (int64_t)v24;
    v27 = *(int64_t*)v13;
    v28 = *(int64_t*)v14;
    v29 = v27 >= v28;
    if (v29) goto bb1;
    else goto bb2;
  bb1:
    v31 = 8LL;
    v32 = malloc(8);
    *(int64_t*)v32 = (int64_t)v31;
    v34 = *(int64_t*)v14;
    v35 = 0LL;
    v36 = v34 > v35;
    if (v36) goto bb4;
    else goto bb5;
  bb2:
    goto bb3;
  bb3:
    v111 = *(void**)v11;
    v112 = *(int64_t*)v111;
    *(int64_t*)v15 = (int64_t)v112;
    v115 = *(int64_t*)v15;
    v116 = *(int64_t*)v13;
    v117 = 8LL;
    v118 = v116 * v117;
    v119 = v115 + v118;
    v120 = malloc(8);
    *(int64_t*)v120 = (int64_t)v119;
    v122 = *(int64_t*)v120;
    v123 = malloc(8);
    *(void**)v123 = (void*)v122;
    v126 = *(void**)v123;
    *(int64_t*)v126 = (int64_t)v2;
    v129 = *(int64_t*)v13;
    v130 = 1LL;
    v131 = v129 + v130;
    v132 = malloc(8);
    *(int64_t*)v132 = (int64_t)v131;
    v135 = *(void**)v5;
    v136 = *(int64_t*)v132;
    *(int64_t*)v135 = (int64_t)v136;
    p_call_stack_pop();
    return;
  bb4:
    v38 = *(int64_t*)v14;
    v39 = 2LL;
    v40 = v38 * v39;
    *(int64_t*)v32 = (int64_t)v40;
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v44 = *(int64_t*)v32;
    v45 = 8LL;
    v46 = v44 * v45;
    v47 = p_malloc(v46);
    v48 = malloc(8);
    *(int64_t*)v48 = (int64_t)v47;
    v50 = 0LL;
    v51 = malloc(8);
    *(int64_t*)v51 = (int64_t)v50;
    goto bb7;
  bb7:
    v54 = *(int64_t*)v51;
    v55 = *(int64_t*)v13;
    v56 = v54 < v55;
    if (v56) goto bb8;
    else goto bb9;
  bb8:
    v58 = *(int64_t*)v15;
    v59 = *(int64_t*)v51;
    v60 = 8LL;
    v61 = v59 * v60;
    v62 = v58 + v61;
    v63 = malloc(8);
    *(int64_t*)v63 = (int64_t)v62;
    v65 = *(int64_t*)v63;
    v66 = malloc(8);
    *(void**)v66 = (void*)v65;
    v68 = *(int64_t*)v48;
    v69 = *(int64_t*)v51;
    v70 = 8LL;
    v71 = v69 * v70;
    v72 = v68 + v71;
    v73 = malloc(8);
    *(int64_t*)v73 = (int64_t)v72;
    v75 = *(int64_t*)v73;
    v76 = malloc(8);
    *(void**)v76 = (void*)v75;
    v79 = *(void**)v66;
    v80 = *(int64_t*)v79;
    v81 = malloc(8);
    *(int64_t*)v81 = (int64_t)v80;
    v83 = *(void**)v76;
    v84 = *(int64_t*)v81;
    *(int64_t*)v83 = (int64_t)v84;
    v87 = *(int64_t*)v51;
    v88 = 1LL;
    v89 = v87 + v88;
    *(int64_t*)v51 = (int64_t)v89;
    goto bb7;
  bb9:
    v92 = *(int64_t*)v15;
    v93 = 0LL;
    v94 = v92 != v93;
    if (v94) goto bb10;
    else goto bb11;
  bb10:
    v96 = *(int64_t*)v15;
    p_free((void*)v96);
    goto bb12;
  bb11:
    goto bb12;
  bb12:
    v101 = *(void**)v11;
    v102 = *(int64_t*)v48;
    *(int64_t*)v101 = (int64_t)v102;
    v104 = *(void**)v9;
    v105 = *(int64_t*)v32;
    *(int64_t*)v104 = (int64_t)v105;
    goto bb3;
    return;
}

int64_t String_find(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_find");
    int64_t v31; /* placeholder for void */
    int64_t v57;
    int64_t v107;
    int64_t v51; /* placeholder for void */
    int64_t v33;
    int64_t v95;
    void* v74;
    int64_t v42;
    int64_t v70;
    int64_t v26; /* placeholder for void */
    int64_t v3;
    int64_t v43;
    int64_t v52; /* placeholder for void */
    uint8_t v81;
    bool v46;
    int64_t v105;
    bool v91;
    int64_t v79; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    bool v4;
    int64_t v97;
    bool v44;
    int64_t v35; /* placeholder for void */
    int64_t v32;
    void* v50;
    int64_t v29;
    int64_t v108; /* placeholder for void */
    void* v68;
    int64_t v8; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    void* v77;
    int64_t v22;
    int64_t v20; /* placeholder for void */
    int64_t v67;
    int64_t v5;
    int64_t v18;
    void* v61;
    int64_t v54;
    int64_t v72; /* placeholder for void */
    void* v65;
    int64_t v13;
    int64_t v28;
    int64_t v90; /* placeholder for void */
    bool v7;
    uint8_t v84;
    int64_t v53;
    int64_t v78; /* placeholder for void */
    int64_t v25;
    int64_t v109; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v87;
    int64_t v96;
    int64_t v66; /* placeholder for void */
    bool v23;
    int64_t v49;
    int64_t v103; /* placeholder for void */
    int64_t v17;
    int64_t v104; /* placeholder for void */
    int64_t v9;
    int64_t v88;
    int64_t v82; /* placeholder for void */
    bool v55;
    int64_t v93; /* placeholder for void */
    int64_t v60;
    int64_t v76;
    int64_t v73;
    int64_t v98; /* placeholder for void */
    int64_t v102;
    int64_t v45; /* placeholder for void */
    int64_t v40;
    int64_t v111;
    bool v6;
    int64_t v64;
    void* v47;
    void* v71;
    bool v89;
    int64_t v85; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v99; /* placeholder for void */
    int64_t v106;
    int64_t v41;
    int64_t v86; /* placeholder for void */
    int64_t v21;
    void* v83;
    int64_t v14;
    void* v37;
    int64_t v58;
    int64_t v112; /* placeholder for void */
    int64_t v59;
    bool v30;
    int64_t v69; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v63;
    void* v80;
    int64_t v36;
    int64_t v48; /* placeholder for void */
    void* v19;
    int64_t v110;
    int64_t v10;
    bool v100;
    int64_t v16; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    void* v15;
    v3 = 0LL;
    v4 = v1 == v3;
    v5 = 0LL;
    v6 = v2 == v5;
    v7 = v4 || v6;
    if (v7) goto bb1;
    else goto bb2;
  bb1:
    v9 = 1LL;
    v10 = -v9;
    p_call_stack_pop();
    return v10;
  bb2:
    goto bb3;
  bb3:
    v13 = 0LL;
    v14 = (int64_t)p_str_length((void*)v13, v1);
    v15 = malloc(8);
    *(int64_t*)v15 = (int64_t)v14;
    v17 = 0LL;
    v18 = (int64_t)p_str_length((void*)v17, v2);
    v19 = malloc(8);
    *(int64_t*)v19 = (int64_t)v18;
    v21 = *(int64_t*)v19;
    v22 = 0LL;
    v23 = v21 == v22;
    if (v23) goto bb4;
    else goto bb5;
  bb4:
    v25 = 0LL;
    p_call_stack_pop();
    return v25;
  bb5:
    goto bb6;
  bb6:
    v28 = *(int64_t*)v19;
    v29 = *(int64_t*)v15;
    v30 = v28 > v29;
    if (v30) goto bb7;
    else goto bb8;
  bb7:
    v32 = 1LL;
    v33 = -v32;
    p_call_stack_pop();
    return v33;
  bb8:
    goto bb9;
  bb9:
    v36 = 0LL;
    v37 = malloc(8);
    *(int64_t*)v37 = (int64_t)v36;
    goto bb10;
  bb10:
    v40 = *(int64_t*)v37;
    v41 = *(int64_t*)v15;
    v42 = *(int64_t*)v19;
    v43 = v41 - v42;
    v44 = v40 <= v43;
    if (v44) goto bb11;
    else goto bb12;
  bb11:
    v46 = true;
    v47 = malloc(1);
    *(bool*)v47 = v46;
    v49 = 0LL;
    v50 = malloc(8);
    *(int64_t*)v50 = (int64_t)v49;
    goto bb13;
  bb12:
    v110 = 1LL;
    v111 = -v110;
    p_call_stack_pop();
    return v111;
  bb13:
    v53 = *(int64_t*)v50;
    v54 = *(int64_t*)v19;
    v55 = v53 < v54;
    if (v55) goto bb14;
    else goto bb15;
  bb14:
    v57 = *(int64_t*)v37;
    v58 = v1 + v57;
    v59 = *(int64_t*)v50;
    v60 = v58 + v59;
    v61 = malloc(8);
    *(int64_t*)v61 = (int64_t)v60;
    v63 = *(int64_t*)v50;
    v64 = v2 + v63;
    v65 = malloc(8);
    *(int64_t*)v65 = (int64_t)v64;
    v67 = *(int64_t*)v61;
    v68 = malloc(8);
    *(void**)v68 = (void*)v67;
    v70 = *(int64_t*)v65;
    v71 = malloc(8);
    *(void**)v71 = (void*)v70;
    v73 = 0LL;
    v74 = malloc(8);
    *(int64_t*)v74 = (int64_t)v73;
    v76 = 0LL;
    v77 = malloc(8);
    *(int64_t*)v77 = (int64_t)v76;
    v80 = *(void**)v68;
    v81 = *(uint8_t*)v80;
    *(int64_t*)v74 = (int64_t)v81;
    v83 = *(void**)v71;
    v84 = *(uint8_t*)v83;
    *(int64_t*)v77 = (int64_t)v84;
    v87 = *(int64_t*)v74;
    v88 = *(int64_t*)v77;
    v89 = v87 != v88;
    if (v89) goto bb16;
    else goto bb17;
  bb15:
    v100 = *(bool*)v47;
    if (v100) goto bb19;
    else goto bb20;
  bb16:
    v91 = false;
    *(bool*)v47 = v91;
    goto bb15;
  bb17:
    goto bb18;
  bb18:
    v95 = *(int64_t*)v50;
    v96 = 1LL;
    v97 = v95 + v96;
    *(int64_t*)v50 = (int64_t)v97;
    goto bb13;
  bb19:
    v102 = *(int64_t*)v37;
    p_call_stack_pop();
    return v102;
  bb20:
    goto bb21;
  bb21:
    v105 = *(int64_t*)v37;
    v106 = 1LL;
    v107 = v105 + v106;
    *(int64_t*)v37 = (int64_t)v107;
    goto bb10;
    return 0;
}

int64_t HashMap_get_or(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("HashMap_get_or");
    bool v60;
    void* v91;
    int64_t v80;
    void* v9;
    void* v24;
    int64_t v31; /* placeholder for void */
    void* v32;
    int64_t v74;
    int64_t v14; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v8;
    int64_t v15; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v58;
    int64_t v65;
    int64_t v69;
    int64_t v17;
    int64_t v75; /* placeholder for void */
    int64_t v89; /* placeholder for void */
    int64_t v57;
    int64_t v95;
    int64_t v37;
    int64_t v49; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v5;
    int64_t v71; /* placeholder for void */
    int64_t v81;
    void* v6;
    int64_t v35; /* placeholder for void */
    int64_t v54;
    int64_t v19; /* placeholder for void */
    int64_t v46;
    int64_t v76; /* placeholder for void */
    int64_t v78;
    void* v88;
    int64_t v63; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v77;
    int64_t v79;
    int64_t v86; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v84;
    int64_t v51;
    int64_t v93; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    int64_t v40;
    int64_t v43;
    void* v12;
    void* v41;
    int64_t v27; /* placeholder for void */
    void* v82;
    int64_t v55;
    int64_t v53; /* placeholder for void */
    int64_t v28;
    int64_t v4;
    int64_t v39;
    int64_t v10; /* placeholder for void */
    void* v44;
    void* v70;
    int64_t v92;
    bool v56;
    int64_t v38;
    int64_t v36;
    int64_t v62; /* placeholder for void */
    void* v85;
    int64_t v22;
    int64_t v16;
    int64_t v13;
    int64_t v72; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v66;
    int64_t v25; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    int64_t v23;
    bool v18;
    int64_t v59;
    int64_t v83; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    void* v50;
    bool v61;
    int64_t v20; /* placeholder for void */
    void* v73;
    void* v67;
    int64_t v33;
    void* v47;
    int64_t v52; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    void* v26;
    int64_t v64; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v87;
    int64_t v96; /* placeholder for void */
    void* v29;
    v4 = 24LL;
    v5 = v1 + v4;
    v6 = malloc(8);
    *(void**)v6 = (void*)v5;
    v8 = 0LL;
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v12 = *(void**)v6;
    v13 = *(int64_t*)v12;
    *(int64_t*)v9 = (int64_t)v13;
    v16 = *(int64_t*)v9;
    v17 = 0LL;
    v18 = v16 == v17;
    if (v18) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return v3;
  bb2:
    goto bb3;
  bb3:
    v22 = 0LL;
    v23 = (int64_t)HashMap_find_slot((void*)v22, v1, v2);
    v24 = malloc(8);
    *(int64_t*)v24 = (int64_t)v23;
    v26 = malloc(8);
    *(void**)v26 = (void*)v1;
    v28 = 0LL;
    v29 = malloc(8);
    *(int64_t*)v29 = (int64_t)v28;
    v32 = *(void**)v26;
    v33 = *(int64_t*)v32;
    *(int64_t*)v29 = (int64_t)v33;
    v36 = *(int64_t*)v29;
    v37 = *(int64_t*)v24;
    v38 = 8LL;
    v39 = v37 * v38;
    v40 = v36 + v39;
    v41 = malloc(8);
    *(int64_t*)v41 = (int64_t)v40;
    v43 = *(int64_t*)v41;
    v44 = malloc(8);
    *(void**)v44 = (void*)v43;
    v46 = 0LL;
    v47 = malloc(8);
    *(int64_t*)v47 = (int64_t)v46;
    v50 = *(void**)v44;
    v51 = *(int64_t*)v50;
    *(int64_t*)v47 = (int64_t)v51;
    v54 = *(int64_t*)v47;
    v55 = 0LL;
    v56 = v54 == v55;
    v57 = *(int64_t*)v47;
    v58 = 1LL;
    v59 = -v58;
    v60 = v57 == v59;
    v61 = v56 || v60;
    if (v61) goto bb4;
    else goto bb5;
  bb4:
    p_call_stack_pop();
    return v3;
  bb5:
    goto bb6;
  bb6:
    v65 = 8LL;
    v66 = v1 + v65;
    v67 = malloc(8);
    *(void**)v67 = (void*)v66;
    v69 = 0LL;
    v70 = malloc(8);
    *(int64_t*)v70 = (int64_t)v69;
    v73 = *(void**)v67;
    v74 = *(int64_t*)v73;
    *(int64_t*)v70 = (int64_t)v74;
    v77 = *(int64_t*)v70;
    v78 = *(int64_t*)v24;
    v79 = 8LL;
    v80 = v78 * v79;
    v81 = v77 + v80;
    v82 = malloc(8);
    *(int64_t*)v82 = (int64_t)v81;
    v84 = *(int64_t*)v82;
    v85 = malloc(8);
    *(void**)v85 = (void*)v84;
    v87 = 0LL;
    v88 = malloc(8);
    *(int64_t*)v88 = (int64_t)v87;
    v91 = *(void**)v85;
    v92 = *(int64_t*)v91;
    *(int64_t*)v88 = (int64_t)v92;
    v95 = *(int64_t*)v88;
    p_call_stack_pop();
    return v95;
    return 0;
}

void* str_to_lower(void* v0, void* v1) {
    p_call_stack_push("str_to_lower");
    int64_t v2;
    int64_t v3;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_to_lowercase((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return (void*)v3;
    return 0;
}

double p_min(void* v0, void* v1) {
    p_call_stack_push("min");
    double v2;
    int64_t v3; /* placeholder for void */
    v2 = (double)p_tensor_min(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

bool HashMap_is_empty(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_is_empty");
    int64_t v5; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    void* v7;
    int64_t v15;
    void* v10;
    int64_t v2;
    int64_t v8; /* placeholder for void */
    int64_t v6;
    int64_t v9; /* placeholder for void */
    int64_t v14;
    void* v4;
    int64_t v17; /* placeholder for void */
    bool v16;
    int64_t v12; /* placeholder for void */
    int64_t v3;
    int64_t v11;
    v2 = 16LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = 0LL;
    v7 = malloc(8);
    *(int64_t*)v7 = (int64_t)v6;
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v7 = (int64_t)v11;
    v14 = *(int64_t*)v7;
    v15 = 0LL;
    v16 = v14 == v15;
    p_call_stack_pop();
    return v16;
    return 0;
}

void Vec_set(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("Vec_set");
    void* v8;
    bool v17;
    void* v6;
    void* v40;
    int64_t v41; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    void* v19;
    int64_t v18; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    void* v26;
    int64_t v27; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v16;
    int64_t v14; /* placeholder for void */
    bool v24;
    void* v32;
    int64_t v36;
    int64_t v4;
    int64_t v39;
    int64_t v47; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v37;
    int64_t v33;
    int64_t v42;
    int64_t v21; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    void* v43;
    int64_t v48; /* placeholder for void */
    int64_t v5;
    int64_t v23;
    int64_t v13;
    void* v10;
    int64_t v28; /* placeholder for void */
    void* v12;
    void* v30;
    int64_t v38;
    void* v46;
    v4 = 8LL;
    v5 = v1 + v4;
    v6 = malloc(8);
    *(void**)v6 = (void*)v5;
    v8 = malloc(8);
    *(void**)v8 = (void*)v1;
    v10 = malloc(8);
    v12 = *(void**)v6;
    v13 = *(int64_t*)v12;
    *(int64_t*)v10 = (int64_t)v13;
    v16 = *(int64_t*)v10;
    v17 = v2 >= v16;
    if (v17) goto bb1;
    else goto bb2;
  bb1:
    v19 = _str0;
    p_panic(v19);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v23 = 0LL;
    v24 = v2 < v23;
    if (v24) goto bb4;
    else goto bb5;
  bb4:
    v26 = _str0;
    p_panic(v26);
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v30 = malloc(8);
    v32 = *(void**)v8;
    v33 = *(int64_t*)v32;
    *(int64_t*)v30 = (int64_t)v33;
    v36 = *(int64_t*)v30;
    v37 = 8LL;
    v38 = v2 * v37;
    v39 = v36 + v38;
    v40 = malloc(8);
    *(int64_t*)v40 = (int64_t)v39;
    v42 = *(int64_t*)v40;
    v43 = malloc(8);
    *(void**)v43 = (void*)v42;
    v46 = *(void**)v43;
    *(int64_t*)v46 = (int64_t)v3;
    p_call_stack_pop();
    return;
    return;
}

void Vec_insert(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("Vec_insert");
    int64_t v87; /* placeholder for void */
    int64_t v95;
    int64_t v97; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    void* v34;
    int64_t v18; /* placeholder for void */
    int64_t v44;
    int64_t v35;
    int64_t v83; /* placeholder for void */
    int64_t v45;
    int64_t v57;
    int64_t v64;
    int64_t v7; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    void* v26;
    int64_t v16;
    void* v38;
    void* v40;
    int64_t v68; /* placeholder for void */
    void* v70;
    int64_t v82; /* placeholder for void */
    int64_t v65;
    int64_t v60;
    int64_t v79; /* placeholder for void */
    int64_t v63;
    int64_t v4;
    int64_t v81;
    int64_t v66;
    int64_t v85;
    int64_t v14; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v91;
    void* v73;
    int64_t v20; /* placeholder for void */
    int64_t v41;
    void* v96;
    void* v58;
    void* v99;
    int64_t v46;
    void* v19;
    int64_t v53;
    int64_t v56;
    int64_t v74; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v88; /* placeholder for void */
    int64_t v89;
    int64_t v90;
    int64_t v25; /* placeholder for void */
    int64_t v5;
    void* v47;
    int64_t v52; /* placeholder for void */
    void* v93;
    int64_t v102; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v61;
    void* v10;
    int64_t v9; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v55;
    int64_t v72;
    int64_t v86;
    int64_t v11; /* placeholder for void */
    int64_t v100; /* placeholder for void */
    int64_t v54;
    int64_t v43; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    void* v12;
    int64_t v31;
    int64_t v42; /* placeholder for void */
    int64_t v50;
    int64_t v15; /* placeholder for void */
    int64_t v30;
    int64_t v37; /* placeholder for void */
    void* v6;
    int64_t v23;
    int64_t v13;
    bool v17;
    bool v24;
    void* v67;
    bool v51;
    int64_t v36; /* placeholder for void */
    int64_t v62;
    int64_t v71; /* placeholder for void */
    int64_t v77;
    void* v8;
    void* v76;
    void* v78;
    int64_t v59; /* placeholder for void */
    int64_t v69;
    void* v80;
    int64_t v92;
    int64_t v29; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    int64_t v84;
    v4 = 8LL;
    v5 = v1 + v4;
    v6 = malloc(8);
    *(void**)v6 = (void*)v5;
    v8 = malloc(8);
    *(void**)v8 = (void*)v1;
    v10 = malloc(8);
    v12 = *(void**)v6;
    v13 = *(int64_t*)v12;
    *(int64_t*)v10 = (int64_t)v13;
    v16 = 0LL;
    v17 = v2 < v16;
    if (v17) goto bb1;
    else goto bb2;
  bb1:
    v19 = _str7;
    p_panic(v19);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v23 = *(int64_t*)v10;
    v24 = v2 > v23;
    if (v24) goto bb4;
    else goto bb5;
  bb4:
    v26 = _str7;
    p_panic(v26);
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v30 = 0LL;
    v31 = 0LL;
    Vec_push((void*)v31, v1, v30);
    v34 = *(void**)v6;
    v35 = *(int64_t*)v34;
    *(int64_t*)v10 = (int64_t)v35;
    v38 = malloc(8);
    v40 = *(void**)v8;
    v41 = *(int64_t*)v40;
    *(int64_t*)v38 = (int64_t)v41;
    v44 = *(int64_t*)v10;
    v45 = 1LL;
    v46 = v44 - v45;
    v47 = malloc(8);
    *(int64_t*)v47 = (int64_t)v46;
    goto bb7;
  bb7:
    v50 = *(int64_t*)v47;
    v51 = v50 > v2;
    if (v51) goto bb8;
    else goto bb9;
  bb8:
    v53 = *(int64_t*)v38;
    v54 = *(int64_t*)v47;
    v55 = 8LL;
    v56 = v54 * v55;
    v57 = v53 + v56;
    v58 = malloc(8);
    *(int64_t*)v58 = (int64_t)v57;
    v60 = *(int64_t*)v38;
    v61 = *(int64_t*)v47;
    v62 = 1LL;
    v63 = v61 - v62;
    v64 = 8LL;
    v65 = v63 * v64;
    v66 = v60 + v65;
    v67 = malloc(8);
    *(int64_t*)v67 = (int64_t)v66;
    v69 = *(int64_t*)v58;
    v70 = malloc(8);
    *(void**)v70 = (void*)v69;
    v72 = *(int64_t*)v67;
    v73 = malloc(8);
    *(void**)v73 = (void*)v72;
    v76 = *(void**)v73;
    v77 = *(int64_t*)v76;
    v78 = malloc(8);
    *(int64_t*)v78 = (int64_t)v77;
    v80 = *(void**)v70;
    v81 = *(int64_t*)v78;
    *(int64_t*)v80 = (int64_t)v81;
    v84 = *(int64_t*)v47;
    v85 = 1LL;
    v86 = v84 - v85;
    *(int64_t*)v47 = (int64_t)v86;
    goto bb7;
  bb9:
    v89 = *(int64_t*)v38;
    v90 = 8LL;
    v91 = v2 * v90;
    v92 = v89 + v91;
    v93 = malloc(8);
    *(int64_t*)v93 = (int64_t)v92;
    v95 = *(int64_t*)v93;
    v96 = malloc(8);
    *(void**)v96 = (void*)v95;
    v99 = *(void**)v96;
    *(int64_t*)v99 = (int64_t)v3;
    p_call_stack_pop();
    return;
    return;
}

int64_t HashMap_values(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_values");
    int64_t v21; /* placeholder for void */
    void* v96;
    void* v93;
    int64_t v88; /* placeholder for void */
    int64_t v99; /* placeholder for void */
    int64_t v82;
    bool v41;
    int64_t v54; /* placeholder for void */
    bool v79;
    int64_t v84;
    void* v16;
    int64_t v15;
    int64_t v81; /* placeholder for void */
    void* v90;
    int64_t v89;
    int64_t v104; /* placeholder for void */
    int64_t v83;
    void* v19;
    int64_t v36;
    int64_t v98; /* placeholder for void */
    int64_t v112; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v37;
    void* v66;
    void* v6;
    int64_t v3;
    int64_t v35; /* placeholder for void */
    int64_t v40;
    int64_t v78;
    int64_t v12;
    int64_t v49; /* placeholder for void */
    bool v38;
    int64_t v32;
    int64_t v95; /* placeholder for void */
    int64_t v44;
    int64_t v20; /* placeholder for void */
    void* v87;
    int64_t v26;
    bool v42;
    int64_t v50; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v33;
    int64_t v17; /* placeholder for void */
    bool v53;
    int64_t v107;
    int64_t v92;
    int64_t v51;
    int64_t v73;
    int64_t v55;
    void* v10;
    int64_t v23;
    int64_t v68; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    int64_t v109; /* placeholder for void */
    int64_t v2;
    int64_t v7; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v52;
    int64_t v85;
    int64_t v47;
    int64_t v57;
    int64_t v27; /* placeholder for void */
    int64_t v70;
    void* v13;
    int64_t v76;
    int64_t v108;
    int64_t v72; /* placeholder for void */
    int64_t v65;
    void* v69;
    void* v4;
    int64_t v39;
    int64_t v56;
    void* v22;
    int64_t v97;
    int64_t v30; /* placeholder for void */
    int64_t v58;
    int64_t v43; /* placeholder for void */
    int64_t v105; /* placeholder for void */
    int64_t v29;
    void* v28;
    void* v63;
    int64_t v111;
    int64_t v100;
    int64_t v14; /* placeholder for void */
    int64_t v110; /* placeholder for void */
    int64_t v18;
    void* v60;
    int64_t v86;
    bool v80;
    bool v75;
    int64_t v101;
    int64_t v74;
    int64_t v46; /* placeholder for void */
    int64_t v103; /* placeholder for void */
    int64_t v61; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v102;
    int64_t v59;
    int64_t v106;
    int64_t v11; /* placeholder for void */
    int64_t v77;
    int64_t v9;
    int64_t v45; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v8;
    void* v48;
    void* v25;
    int64_t v71; /* placeholder for void */
    void* v34;
    int64_t v62;
    v2 = 24LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = malloc(8);
    *(void**)v6 = (void*)v1;
    v8 = 8LL;
    v9 = v1 + v8;
    v10 = malloc(8);
    *(void**)v10 = (void*)v9;
    v12 = 0LL;
    v13 = malloc(8);
    *(int64_t*)v13 = (int64_t)v12;
    v15 = 0LL;
    v16 = malloc(8);
    *(int64_t*)v16 = (int64_t)v15;
    v18 = 0LL;
    v19 = malloc(8);
    *(int64_t*)v19 = (int64_t)v18;
    v22 = *(void**)v4;
    v23 = *(int64_t*)v22;
    *(int64_t*)v13 = (int64_t)v23;
    v25 = *(void**)v6;
    v26 = *(int64_t*)v25;
    *(int64_t*)v16 = (int64_t)v26;
    v28 = *(void**)v10;
    v29 = *(int64_t*)v28;
    *(int64_t*)v19 = (int64_t)v29;
    v32 = 0LL;
    v33 = (int64_t)Vec_new((void*)v32);
    v34 = malloc(8);
    *(int64_t*)v34 = (int64_t)v33;
    v36 = *(int64_t*)v16;
    v37 = 0LL;
    v38 = v36 == v37;
    v39 = *(int64_t*)v13;
    v40 = 0LL;
    v41 = v39 == v40;
    v42 = v38 || v41;
    if (v42) goto bb1;
    else goto bb2;
  bb1:
    v44 = *(int64_t*)v34;
    p_call_stack_pop();
    return v44;
  bb2:
    goto bb3;
  bb3:
    v47 = 0LL;
    v48 = malloc(8);
    *(int64_t*)v48 = (int64_t)v47;
    goto bb4;
  bb4:
    v51 = *(int64_t*)v48;
    v52 = *(int64_t*)v13;
    v53 = v51 < v52;
    if (v53) goto bb5;
    else goto bb6;
  bb5:
    v55 = *(int64_t*)v16;
    v56 = *(int64_t*)v48;
    v57 = 8LL;
    v58 = v56 * v57;
    v59 = v55 + v58;
    v60 = malloc(8);
    *(int64_t*)v60 = (int64_t)v59;
    v62 = *(int64_t*)v60;
    v63 = malloc(8);
    *(void**)v63 = (void*)v62;
    v65 = 0LL;
    v66 = malloc(8);
    *(int64_t*)v66 = (int64_t)v65;
    v69 = *(void**)v63;
    v70 = *(int64_t*)v69;
    *(int64_t*)v66 = (int64_t)v70;
    v73 = *(int64_t*)v66;
    v74 = 0LL;
    v75 = v73 != v74;
    v76 = *(int64_t*)v66;
    v77 = 1LL;
    v78 = -v77;
    v79 = v76 != v78;
    v80 = v75 && v79;
    if (v80) goto bb7;
    else goto bb8;
  bb6:
    v111 = *(int64_t*)v34;
    p_call_stack_pop();
    return v111;
  bb7:
    v82 = *(int64_t*)v19;
    v83 = *(int64_t*)v48;
    v84 = 8LL;
    v85 = v83 * v84;
    v86 = v82 + v85;
    v87 = malloc(8);
    *(int64_t*)v87 = (int64_t)v86;
    v89 = *(int64_t*)v87;
    v90 = malloc(8);
    *(void**)v90 = (void*)v89;
    v92 = 0LL;
    v93 = malloc(8);
    *(int64_t*)v93 = (int64_t)v92;
    v96 = *(void**)v90;
    v97 = *(int64_t*)v96;
    *(int64_t*)v93 = (int64_t)v97;
    v100 = *(int64_t*)v34;
    v101 = *(int64_t*)v93;
    v102 = 0LL;
    Vec_push((void*)v102, v100, v101);
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    v106 = *(int64_t*)v48;
    v107 = 1LL;
    v108 = v106 + v107;
    *(int64_t*)v48 = (int64_t)v108;
    goto bb4;
    return 0;
}

void HashMap_remove(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_remove");
    int64_t v51; /* placeholder for void */
    void* v12;
    void* v47;
    int64_t v13; /* placeholder for void */
    int64_t v70;
    int64_t v76; /* placeholder for void */
    int64_t v46;
    int64_t v71;
    int64_t v68; /* placeholder for void */
    int64_t v25;
    int64_t v18; /* placeholder for void */
    int64_t v42;
    int64_t v49;
    int64_t v41;
    int64_t v16;
    int64_t v52; /* placeholder for void */
    int64_t v26;
    int64_t v67; /* placeholder for void */
    void* v78;
    int64_t v84;
    int64_t v92; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    void* v29;
    int64_t v40;
    int64_t v30; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v80; /* placeholder for void */
    int64_t v11;
    void* v27;
    int64_t v43;
    void* v5;
    bool v63;
    int64_t v66; /* placeholder for void */
    int64_t v58;
    bool v64;
    int64_t v82;
    int64_t v17; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v73; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    int64_t v3;
    void* v15;
    int64_t v57;
    int64_t v87; /* placeholder for void */
    void* v9;
    void* v88;
    int64_t v89;
    int64_t v14; /* placeholder for void */
    int64_t v19;
    int64_t v45; /* placeholder for void */
    int64_t v62;
    int64_t v20;
    void* v44;
    int64_t v65; /* placeholder for void */
    int64_t v7;
    int64_t v31;
    void* v69;
    void* v32;
    void* v75;
    int64_t v4;
    int64_t v83;
    int64_t v86; /* placeholder for void */
    bool v21;
    int64_t v10; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    void* v35;
    int64_t v36;
    int64_t v37; /* placeholder for void */
    bool v59;
    int64_t v72; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    int64_t v61;
    void* v53;
    int64_t v39;
    int64_t v56; /* placeholder for void */
    int64_t v60;
    int64_t v90; /* placeholder for void */
    void* v50;
    int64_t v54;
    int64_t v74;
    void* v85;
    int64_t v79;
    int64_t v22; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    int64_t v8;
    int64_t v24; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    v3 = 24LL;
    v4 = v1 + v3;
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = 16LL;
    v8 = v1 + v7;
    v9 = malloc(8);
    *(void**)v9 = (void*)v8;
    v11 = 0LL;
    v12 = malloc(8);
    *(int64_t*)v12 = (int64_t)v11;
    v15 = *(void**)v5;
    v16 = *(int64_t*)v15;
    *(int64_t*)v12 = (int64_t)v16;
    v19 = *(int64_t*)v12;
    v20 = 0LL;
    v21 = v19 == v20;
    if (v21) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return;
  bb2:
    goto bb3;
  bb3:
    v25 = 0LL;
    v26 = (int64_t)HashMap_find_slot((void*)v25, v1, v2);
    v27 = malloc(8);
    *(int64_t*)v27 = (int64_t)v26;
    v29 = malloc(8);
    *(void**)v29 = (void*)v1;
    v31 = 0LL;
    v32 = malloc(8);
    *(int64_t*)v32 = (int64_t)v31;
    v35 = *(void**)v29;
    v36 = *(int64_t*)v35;
    *(int64_t*)v32 = (int64_t)v36;
    v39 = *(int64_t*)v32;
    v40 = *(int64_t*)v27;
    v41 = 8LL;
    v42 = v40 * v41;
    v43 = v39 + v42;
    v44 = malloc(8);
    *(int64_t*)v44 = (int64_t)v43;
    v46 = *(int64_t*)v44;
    v47 = malloc(8);
    *(void**)v47 = (void*)v46;
    v49 = 0LL;
    v50 = malloc(8);
    *(int64_t*)v50 = (int64_t)v49;
    v53 = *(void**)v47;
    v54 = *(int64_t*)v53;
    *(int64_t*)v50 = (int64_t)v54;
    v57 = *(int64_t*)v50;
    v58 = 0LL;
    v59 = v57 == v58;
    v60 = *(int64_t*)v50;
    v61 = 1LL;
    v62 = -v61;
    v63 = v60 == v62;
    v64 = v59 || v63;
    if (v64) goto bb4;
    else goto bb5;
  bb4:
    p_call_stack_pop();
    return;
  bb5:
    goto bb6;
  bb6:
    v69 = *(void**)v47;
    v70 = 1LL;
    v71 = -v70;
    *(int64_t*)v69 = (int64_t)v71;
    v74 = 0LL;
    v75 = malloc(8);
    *(int64_t*)v75 = (int64_t)v74;
    v78 = *(void**)v9;
    v79 = *(int64_t*)v78;
    *(int64_t*)v75 = (int64_t)v79;
    v82 = *(int64_t*)v75;
    v83 = 1LL;
    v84 = v82 - v83;
    v85 = malloc(8);
    *(int64_t*)v85 = (int64_t)v84;
    v88 = *(void**)v9;
    v89 = *(int64_t*)v85;
    *(int64_t*)v88 = (int64_t)v89;
    p_call_stack_pop();
    return;
    return;
}

int64_t String_replace(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("String_replace");
    int64_t v71;
    int64_t v149; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v81;
    int64_t v137; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    bool v25;
    int64_t v76;
    int64_t v80;
    void* v106;
    int64_t v53;
    int64_t v87; /* placeholder for void */
    int64_t v127;
    int64_t v134;
    int64_t v131; /* placeholder for void */
    uint8_t v113;
    uint8_t v116;
    int64_t v42; /* placeholder for void */
    int64_t v33;
    void* v97;
    int64_t v47; /* placeholder for void */
    int64_t v128;
    int64_t v133; /* placeholder for void */
    void* v159;
    int64_t v138;
    int64_t v28;
    int64_t v84; /* placeholder for void */
    void* v115;
    int64_t v41;
    int64_t v18; /* placeholder for void */
    int64_t v141; /* placeholder for void */
    int64_t v161; /* placeholder for void */
    int64_t v165; /* placeholder for void */
    int64_t v169; /* placeholder for void */
    int64_t v174; /* placeholder for void */
    int64_t v175;
    int64_t v73; /* placeholder for void */
    void* v179;
    int64_t v164; /* placeholder for void */
    int64_t v196;
    int64_t v143;
    int64_t v144;
    int64_t v140;
    void* v112;
    int64_t v95;
    int64_t v75;
    int64_t v43; /* placeholder for void */
    int64_t v105;
    bool v38;
    int64_t v20; /* placeholder for void */
    int64_t v7;
    int64_t v104; /* placeholder for void */
    int64_t v147;
    int64_t v55; /* placeholder for void */
    int64_t v4;
    int64_t v11;
    int64_t v152; /* placeholder for void */
    int64_t v170;
    void* v9;
    int64_t v156; /* placeholder for void */
    void* v166;
    int64_t v183;
    int64_t v27; /* placeholder for void */
    void* v100;
    void* v176;
    int64_t v19;
    void* v45;
    bool v26;
    int64_t v64;
    int64_t v180;
    int64_t v184;
    int64_t v82;
    int64_t v114; /* placeholder for void */
    int64_t v142; /* placeholder for void */
    void* v163;
    int64_t v146; /* placeholder for void */
    int64_t v191;
    int64_t v16;
    bool v132;
    int64_t v65;
    int64_t v194; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    void* v15;
    int64_t v44;
    void* v93;
    int64_t v107; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v110; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v129;
    void* v145;
    int64_t v60;
    int64_t v181; /* placeholder for void */
    int64_t v111; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v118; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    bool v123;
    int64_t v187; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v92;
    void* v49;
    void* v103;
    int64_t v136;
    int64_t v126; /* placeholder for void */
    bool v121;
    void* v72;
    void* v154;
    void* v148;
    int64_t v189; /* placeholder for void */
    void* v151;
    int64_t v158;
    int64_t v108;
    int64_t v40;
    int64_t v36;
    int64_t v94; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v37;
    int64_t v188;
    void* v34;
    bool v23;
    int64_t v124; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v171;
    int64_t v153; /* placeholder for void */
    int64_t v177; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v117; /* placeholder for void */
    int64_t v139;
    int64_t v88; /* placeholder for void */
    int64_t v99;
    int64_t v102;
    int64_t v182; /* placeholder for void */
    int64_t v58; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v178; /* placeholder for void */
    void* v12;
    int64_t v63; /* placeholder for void */
    int64_t v162;
    int64_t v167;
    void* v69;
    int64_t v197; /* placeholder for void */
    void* v57;
    int64_t v90;
    int64_t v125; /* placeholder for void */
    int64_t v157; /* placeholder for void */
    int64_t v192;
    bool v5;
    int64_t v22;
    int64_t v190;
    int64_t v195; /* placeholder for void */
    uint8_t v155;
    int64_t v135;
    int64_t v130; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v172;
    int64_t v29;
    int64_t v30; /* placeholder for void */
    int64_t v48;
    int64_t v78; /* placeholder for void */
    int64_t v150;
    bool v77;
    void* v8;
    int64_t v79;
    int64_t v89;
    int64_t v10; /* placeholder for void */
    int64_t v91;
    int64_t v59;
    int64_t v96;
    bool v85;
    void* v173;
    int64_t v24;
    int64_t v185;
    int64_t v51; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    int64_t v193; /* placeholder for void */
    bool v83;
    int64_t v56;
    int64_t v70; /* placeholder for void */
    bool v68;
    void* v109;
    int64_t v119;
    void* v52;
    int64_t v120;
    int64_t v168; /* placeholder for void */
    int64_t v186;
    void* v61;
    int64_t v32;
    void* v160;
    void* v46;
    int64_t v122; /* placeholder for void */
    bool v66;
    v4 = 0LL;
    v5 = v1 == v4;
    if (v5) goto bb1;
    else goto bb2;
  bb1:
    v7 = 1LL;
    v8 = p_malloc(v7);
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = *(int64_t*)v9;
    v12 = malloc(8);
    *(void**)v12 = (void*)v11;
    v15 = *(void**)v12;
    v16 = 0LL;
    *(uint8_t*)v15 = (uint8_t)v16;
    v19 = *(int64_t*)v9;
    p_call_stack_pop();
    return v19;
  bb2:
    goto bb3;
  bb3:
    v22 = 0LL;
    v23 = v2 == v22;
    v24 = 0LL;
    v25 = v3 == v24;
    v26 = v23 || v25;
    if (v26) goto bb4;
    else goto bb5;
  bb4:
    v28 = 0LL;
    v29 = (int64_t)String_from((void*)v28, v1);
    p_call_stack_pop();
    return v29;
  bb5:
    goto bb6;
  bb6:
    v32 = 0LL;
    v33 = (int64_t)p_str_length((void*)v32, v2);
    v34 = malloc(8);
    *(int64_t*)v34 = (int64_t)v33;
    v36 = *(int64_t*)v34;
    v37 = 0LL;
    v38 = v36 == v37;
    if (v38) goto bb7;
    else goto bb8;
  bb7:
    v40 = 0LL;
    v41 = (int64_t)String_from((void*)v40, v1);
    p_call_stack_pop();
    return v41;
  bb8:
    goto bb9;
  bb9:
    v44 = 1LL;
    v45 = p_malloc(v44);
    v46 = malloc(8);
    *(int64_t*)v46 = (int64_t)v45;
    v48 = *(int64_t*)v46;
    v49 = malloc(8);
    *(void**)v49 = (void*)v48;
    v52 = *(void**)v49;
    v53 = 0LL;
    *(uint8_t*)v52 = (uint8_t)v53;
    v56 = 0LL;
    v57 = malloc(8);
    *(int64_t*)v57 = (int64_t)v56;
    v59 = 0LL;
    v60 = (int64_t)p_str_length((void*)v59, v1);
    v61 = malloc(8);
    *(int64_t*)v61 = (int64_t)v60;
    goto bb10;
  bb10:
    v64 = *(int64_t*)v57;
    v65 = *(int64_t*)v61;
    v66 = v64 < v65;
    if (v66) goto bb11;
    else goto bb12;
  bb11:
    v68 = true;
    v69 = malloc(1);
    *(bool*)v69 = v68;
    v71 = 0LL;
    v72 = malloc(8);
    *(int64_t*)v72 = (int64_t)v71;
    goto bb13;
  bb12:
    v196 = *(int64_t*)v46;
    p_call_stack_pop();
    return v196;
  bb13:
    v75 = *(int64_t*)v72;
    v76 = *(int64_t*)v34;
    v77 = v75 < v76;
    if (v77) goto bb14;
    else goto bb15;
  bb14:
    v79 = *(int64_t*)v57;
    v80 = *(int64_t*)v72;
    v81 = v79 + v80;
    v82 = *(int64_t*)v61;
    v83 = v81 >= v82;
    if (v83) goto bb16;
    else goto bb17;
  bb15:
    v132 = *(bool*)v69;
    if (v132) goto bb22;
    else goto bb23;
  bb16:
    v85 = false;
    *(bool*)v69 = v85;
    goto bb15;
  bb17:
    goto bb18;
  bb18:
    v89 = *(int64_t*)v57;
    v90 = v1 + v89;
    v91 = *(int64_t*)v72;
    v92 = v90 + v91;
    v93 = malloc(8);
    *(int64_t*)v93 = (int64_t)v92;
    v95 = *(int64_t*)v72;
    v96 = v2 + v95;
    v97 = malloc(8);
    *(int64_t*)v97 = (int64_t)v96;
    v99 = *(int64_t*)v93;
    v100 = malloc(8);
    *(void**)v100 = (void*)v99;
    v102 = *(int64_t*)v97;
    v103 = malloc(8);
    *(void**)v103 = (void*)v102;
    v105 = 0LL;
    v106 = malloc(8);
    *(int64_t*)v106 = (int64_t)v105;
    v108 = 0LL;
    v109 = malloc(8);
    *(int64_t*)v109 = (int64_t)v108;
    v112 = *(void**)v100;
    v113 = *(uint8_t*)v112;
    *(int64_t*)v106 = (int64_t)v113;
    v115 = *(void**)v103;
    v116 = *(uint8_t*)v115;
    *(int64_t*)v109 = (int64_t)v116;
    v119 = *(int64_t*)v106;
    v120 = *(int64_t*)v109;
    v121 = v119 != v120;
    if (v121) goto bb19;
    else goto bb20;
  bb19:
    v123 = false;
    *(bool*)v69 = v123;
    goto bb15;
  bb20:
    goto bb21;
  bb21:
    v127 = *(int64_t*)v72;
    v128 = 1LL;
    v129 = v127 + v128;
    *(int64_t*)v72 = (int64_t)v129;
    goto bb13;
  bb22:
    v134 = *(int64_t*)v46;
    v135 = 0LL;
    v136 = (int64_t)String_push_str((void*)v135, v134, v3);
    *(int64_t*)v46 = (int64_t)v136;
    v138 = *(int64_t*)v57;
    v139 = *(int64_t*)v34;
    v140 = v138 + v139;
    *(int64_t*)v57 = (int64_t)v140;
    goto bb24;
  bb23:
    v143 = *(int64_t*)v57;
    v144 = v1 + v143;
    v145 = malloc(8);
    *(int64_t*)v145 = (int64_t)v144;
    v147 = *(int64_t*)v145;
    v148 = malloc(8);
    *(void**)v148 = (void*)v147;
    v150 = 0LL;
    v151 = malloc(8);
    *(int64_t*)v151 = (int64_t)v150;
    v154 = *(void**)v148;
    v155 = *(uint8_t*)v154;
    *(int64_t*)v151 = (int64_t)v155;
    v158 = 2LL;
    v159 = p_malloc(v158);
    v160 = malloc(8);
    *(int64_t*)v160 = (int64_t)v159;
    v162 = *(int64_t*)v160;
    v163 = malloc(8);
    *(void**)v163 = (void*)v162;
    v166 = *(void**)v163;
    v167 = *(int64_t*)v151;
    *(uint8_t*)v166 = (uint8_t)v167;
    v170 = *(int64_t*)v160;
    v171 = 1LL;
    v172 = v170 + v171;
    v173 = malloc(8);
    *(int64_t*)v173 = (int64_t)v172;
    v175 = *(int64_t*)v173;
    v176 = malloc(8);
    *(void**)v176 = (void*)v175;
    v179 = *(void**)v176;
    v180 = 0LL;
    *(uint8_t*)v179 = (uint8_t)v180;
    v183 = *(int64_t*)v46;
    v184 = *(int64_t*)v160;
    v185 = 0LL;
    v186 = (int64_t)String_push_str((void*)v185, v183, v184);
    *(int64_t*)v46 = (int64_t)v186;
    v188 = *(int64_t*)v160;
    p_free((void*)v188);
    v190 = *(int64_t*)v57;
    v191 = 1LL;
    v192 = v190 + v191;
    *(int64_t*)v57 = (int64_t)v192;
    goto bb24;
  bb24:
    goto bb10;
    return 0;
}

int64_t p_abs(void* v0, int64_t v1) {
    p_call_stack_push("abs");
    bool v3;
    int64_t v7; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v2;
    int64_t v5;
    int64_t v6; /* placeholder for void */
    v2 = 0LL;
    v3 = v1 < v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = -v1;
    p_call_stack_pop();
    return v5;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return v1;
    return 0;
}

int64_t p_str_compare(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("p_str_compare");
    int64_t v25;
    void* v32;
    int64_t v42;
    int64_t v13; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v18;
    int64_t v35; /* placeholder for void */
    void* v36;
    int64_t v12;
    bool v3;
    int64_t v27; /* placeholder for void */
    int64_t v54;
    int64_t v72; /* placeholder for void */
    void* v73;
    int64_t v15;
    uint8_t v37;
    int64_t v22; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v45;
    bool v51;
    int64_t v59;
    int64_t v63;
    int64_t v65; /* placeholder for void */
    bool v16;
    int64_t v48; /* placeholder for void */
    int64_t v50;
    int64_t v76; /* placeholder for void */
    bool v9;
    int64_t v19; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    void* v29;
    int64_t v62;
    uint8_t v74;
    void* v23;
    int64_t v41;
    int64_t v24; /* placeholder for void */
    void* v34;
    int64_t v57; /* placeholder for void */
    void* v70;
    int64_t v80; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v5;
    int64_t v52; /* placeholder for void */
    int64_t v53;
    int64_t v7; /* placeholder for void */
    int64_t v77;
    int64_t v39; /* placeholder for void */
    int64_t v64;
    int64_t v68;
    int64_t v8;
    void* v26;
    int64_t v20; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    uint8_t v71;
    int64_t v17; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v66;
    int64_t v79;
    int64_t v11;
    int64_t v10; /* placeholder for void */
    int64_t v28;
    int64_t v78;
    uint8_t v33;
    bool v43;
    int64_t v4; /* placeholder for void */
    int64_t v44;
    int64_t v61; /* placeholder for void */
    bool v46;
    void* v38;
    bool v47;
    int64_t v58;
    int64_t v60;
    void* v21;
    int64_t v49;
    int64_t v55;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 0LL;
    p_call_stack_pop();
    return v5;
  bb2:
    goto bb3;
  bb3:
    v8 = 0LL;
    v9 = v1 == v8;
    if (v9) goto bb4;
    else goto bb5;
  bb4:
    v11 = 1LL;
    v12 = -v11;
    p_call_stack_pop();
    return v12;
  bb5:
    goto bb6;
  bb6:
    v15 = 0LL;
    v16 = v2 == v15;
    if (v16) goto bb7;
    else goto bb8;
  bb7:
    v18 = 1LL;
    p_call_stack_pop();
    return v18;
  bb8:
    goto bb9;
  bb9:
    v21 = malloc(8);
    *(int64_t*)v21 = (int64_t)v1;
    v23 = malloc(8);
    *(int64_t*)v23 = (int64_t)v2;
    v25 = *(int64_t*)v21;
    v26 = malloc(8);
    *(void**)v26 = (void*)v25;
    v28 = *(int64_t*)v23;
    v29 = malloc(8);
    *(void**)v29 = (void*)v28;
    v32 = *(void**)v26;
    v33 = *(uint8_t*)v32;
    v34 = malloc(8);
    *(int64_t*)v34 = (int64_t)v33;
    v36 = *(void**)v29;
    v37 = *(uint8_t*)v36;
    v38 = malloc(8);
    *(int64_t*)v38 = (int64_t)v37;
    goto bb10;
  bb10:
    v41 = *(int64_t*)v34;
    v42 = 0LL;
    v43 = v41 != v42;
    v44 = *(int64_t*)v38;
    v45 = 0LL;
    v46 = v44 != v45;
    v47 = v43 && v46;
    if (v47) goto bb11;
    else goto bb12;
  bb11:
    v49 = *(int64_t*)v34;
    v50 = *(int64_t*)v38;
    v51 = v49 != v50;
    if (v51) goto bb13;
    else goto bb14;
  bb12:
    v77 = *(int64_t*)v34;
    v78 = *(int64_t*)v38;
    v79 = v77 - v78;
    p_call_stack_pop();
    return v79;
  bb13:
    v53 = *(int64_t*)v34;
    v54 = *(int64_t*)v38;
    v55 = v53 - v54;
    p_call_stack_pop();
    return v55;
  bb14:
    goto bb15;
  bb15:
    v58 = *(int64_t*)v21;
    v59 = 1LL;
    v60 = v58 + v59;
    *(int64_t*)v21 = (int64_t)v60;
    v62 = *(int64_t*)v23;
    v63 = 1LL;
    v64 = v62 + v63;
    *(int64_t*)v23 = (int64_t)v64;
    v66 = *(int64_t*)v21;
    *(void**)v26 = (void*)v66;
    v68 = *(int64_t*)v23;
    *(void**)v29 = (void*)v68;
    v70 = *(void**)v26;
    v71 = *(uint8_t*)v70;
    *(int64_t*)v34 = (int64_t)v71;
    v73 = *(void**)v29;
    v74 = *(uint8_t*)v73;
    *(int64_t*)v38 = (int64_t)v74;
    goto bb10;
    return 0;
}

int64_t Vec_capacity(void* v0, int64_t v1) {
    p_call_stack_push("Vec_capacity");
    int64_t v12;
    int64_t v2;
    int64_t v5; /* placeholder for void */
    int64_t v9;
    int64_t v3;
    void* v4;
    void* v6;
    int64_t v7; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v8;
    int64_t v11; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    v2 = 16LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = malloc(8);
    v8 = *(void**)v4;
    v9 = *(int64_t*)v8;
    *(int64_t*)v6 = (int64_t)v9;
    v12 = *(int64_t*)v6;
    p_call_stack_pop();
    return v12;
    return 0;
}

int64_t char_to_lower(void* v0, int64_t v1) {
    p_call_stack_push("char_to_lower");
    int64_t v4;
    int64_t v7; /* placeholder for void */
    bool v3;
    bool v5;
    bool v6;
    int64_t v10; /* placeholder for void */
    int64_t v2;
    int64_t v11; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v9;
    int64_t v8;
    v2 = 65LL;
    v3 = v1 >= v2;
    v4 = 90LL;
    v5 = v1 <= v4;
    v6 = v3 && v5;
    if (v6) goto bb1;
    else goto bb2;
  bb1:
    v8 = 32LL;
    v9 = v1 + v8;
    p_call_stack_pop();
    return v9;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return v1;
    return 0;
}

void* to_str(void* v0, void* v1) {
    p_call_stack_push("to_str");
    int64_t v3; /* placeholder for void */
    void* v2;
    v2 = p_tensor_to_str(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t String_parse_int(void* v0, int64_t v1) {
    p_call_stack_push("String_parse_int");
    int64_t v82; /* placeholder for void */
    int64_t v37;
    int64_t v4; /* placeholder for void */
    int64_t v103;
    int64_t v11;
    void* v22;
    int64_t v81; /* placeholder for void */
    int64_t v76;
    int64_t v77; /* placeholder for void */
    bool v35;
    int64_t v95;
    bool v39;
    int64_t v104; /* placeholder for void */
    int64_t v25;
    void* v20;
    void* v17;
    int64_t v46; /* placeholder for void */
    bool v87;
    int64_t v8;
    void* v64;
    int64_t v55;
    int64_t v32;
    int64_t v115; /* placeholder for void */
    int64_t v72;
    int64_t v50; /* placeholder for void */
    int64_t v97;
    int64_t v114;
    int64_t v40; /* placeholder for void */
    bool v27;
    int64_t v66; /* placeholder for void */
    int64_t v73;
    int64_t v33;
    bool v38;
    uint8_t v108;
    void* v78;
    int64_t v51;
    int64_t v61; /* placeholder for void */
    int64_t v109; /* placeholder for void */
    int64_t v58;
    bool v3;
    bool v90;
    int64_t v24; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    void* v107;
    int64_t v83; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v112;
    int64_t v102;
    int64_t v10; /* placeholder for void */
    int64_t v110; /* placeholder for void */
    int64_t v100; /* placeholder for void */
    int64_t v43;
    uint8_t v48;
    bool v91;
    int64_t v113;
    int64_t v57; /* placeholder for void */
    int64_t v52;
    int64_t v18; /* placeholder for void */
    int64_t v42;
    int64_t v75; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v56;
    uint8_t v79;
    int64_t v92; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    uint8_t v21;
    int64_t v54; /* placeholder for void */
    int64_t v98;
    int64_t v26;
    int64_t v105;
    int64_t v62;
    int64_t v59;
    int64_t v88;
    bool v30;
    int64_t v99;
    int64_t v41;
    bool v34;
    int64_t v69;
    uint8_t v65;
    int64_t v16;
    int64_t v23; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    bool v53;
    int64_t v67; /* placeholder for void */
    int64_t v60;
    bool v31;
    int64_t v74;
    int64_t v85;
    int64_t v80; /* placeholder for void */
    int64_t v96;
    int64_t v2;
    int64_t v68;
    void* v14;
    int64_t v45;
    int64_t v94;
    int64_t v101;
    void* v12;
    int64_t v5;
    int64_t v28;
    int64_t v13; /* placeholder for void */
    int64_t v86;
    int64_t v44; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v36;
    int64_t v89;
    void* v47;
    void* v9;
    int64_t v29;
    bool v70;
    int64_t v93;
    int64_t v111; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 0LL;
    p_call_stack_pop();
    return v5;
  bb2:
    goto bb3;
  bb3:
    v8 = 0LL;
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 1LL;
    v12 = malloc(8);
    *(int64_t*)v12 = (int64_t)v11;
    v14 = malloc(8);
    *(int64_t*)v14 = (int64_t)v1;
    v16 = *(int64_t*)v14;
    v17 = malloc(8);
    *(void**)v17 = (void*)v16;
    v20 = *(void**)v17;
    v21 = *(uint8_t*)v20;
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v21;
    goto bb4;
  bb4:
    v25 = *(int64_t*)v22;
    v26 = 32LL;
    v27 = v25 == v26;
    v28 = *(int64_t*)v22;
    v29 = 9LL;
    v30 = v28 == v29;
    v31 = v27 || v30;
    v32 = *(int64_t*)v22;
    v33 = 10LL;
    v34 = v32 == v33;
    v35 = v31 || v34;
    v36 = *(int64_t*)v22;
    v37 = 13LL;
    v38 = v36 == v37;
    v39 = v35 || v38;
    if (v39) goto bb5;
    else goto bb6;
  bb5:
    v41 = *(int64_t*)v14;
    v42 = 1LL;
    v43 = v41 + v42;
    *(int64_t*)v14 = (int64_t)v43;
    v45 = *(int64_t*)v14;
    *(void**)v17 = (void*)v45;
    v47 = *(void**)v17;
    v48 = *(uint8_t*)v47;
    *(int64_t*)v22 = (int64_t)v48;
    goto bb4;
  bb6:
    v51 = *(int64_t*)v22;
    v52 = 45LL;
    v53 = v51 == v52;
    if (v53) goto bb7;
    else goto bb8;
  bb7:
    v55 = 1LL;
    v56 = -v55;
    *(int64_t*)v12 = (int64_t)v56;
    v58 = *(int64_t*)v14;
    v59 = 1LL;
    v60 = v58 + v59;
    *(int64_t*)v14 = (int64_t)v60;
    v62 = *(int64_t*)v14;
    *(void**)v17 = (void*)v62;
    v64 = *(void**)v17;
    v65 = *(uint8_t*)v64;
    *(int64_t*)v22 = (int64_t)v65;
    goto bb9;
  bb8:
    v68 = *(int64_t*)v22;
    v69 = 43LL;
    v70 = v68 == v69;
    if (v70) goto bb10;
    else goto bb11;
  bb9:
    goto bb13;
  bb10:
    v72 = *(int64_t*)v14;
    v73 = 1LL;
    v74 = v72 + v73;
    *(int64_t*)v14 = (int64_t)v74;
    v76 = *(int64_t*)v14;
    *(void**)v17 = (void*)v76;
    v78 = *(void**)v17;
    v79 = *(uint8_t*)v78;
    *(int64_t*)v22 = (int64_t)v79;
    goto bb12;
  bb11:
    goto bb12;
  bb12:
    goto bb9;
  bb13:
    v85 = *(int64_t*)v22;
    v86 = 48LL;
    v87 = v85 >= v86;
    v88 = *(int64_t*)v22;
    v89 = 57LL;
    v90 = v88 <= v89;
    v91 = v87 && v90;
    if (v91) goto bb14;
    else goto bb15;
  bb14:
    v93 = *(int64_t*)v9;
    v94 = 10LL;
    v95 = v93 * v94;
    v96 = *(int64_t*)v22;
    v97 = 48LL;
    v98 = v96 - v97;
    v99 = v95 + v98;
    *(int64_t*)v9 = (int64_t)v99;
    v101 = *(int64_t*)v14;
    v102 = 1LL;
    v103 = v101 + v102;
    *(int64_t*)v14 = (int64_t)v103;
    v105 = *(int64_t*)v14;
    *(void**)v17 = (void*)v105;
    v107 = *(void**)v17;
    v108 = *(uint8_t*)v107;
    *(int64_t*)v22 = (int64_t)v108;
    goto bb13;
  bb15:
    v112 = *(int64_t*)v9;
    v113 = *(int64_t*)v12;
    v114 = v112 * v113;
    p_call_stack_pop();
    return v114;
    return 0;
}

void* str_trim_end(void* v0, void* v1) {
    p_call_stack_push("str_trim_end");
    int64_t v9; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v20;
    int64_t v39;
    int64_t v26; /* placeholder for void */
    bool v12;
    int64_t v29; /* placeholder for void */
    bool v33;
    int64_t v37; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v34;
    void* v21;
    int64_t v44;
    int64_t v2;
    uint8_t v28;
    int64_t v42; /* placeholder for void */
    int64_t v23;
    int64_t v40;
    bool v45;
    int64_t v31;
    int64_t v52;
    int64_t v53;
    int64_t v14;
    void* v27;
    int64_t v11;
    int64_t v3;
    int64_t v16;
    int64_t v6;
    int64_t v25; /* placeholder for void */
    int64_t v15;
    int64_t v30; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v51;
    int64_t v54; /* placeholder for void */
    int64_t v17;
    int64_t v10;
    void* v4;
    int64_t v19; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v43;
    void* v47;
    int64_t v50;
    int64_t v49; /* placeholder for void */
    int64_t v32;
    int64_t v38;
    int64_t v13; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    void* v7;
    void* v18;
    int64_t v8; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    void* v24;
    v2 = 0LL;
    v3 = (int64_t)String_len((void*)v2, (int64_t)v1);
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    v7 = malloc(8);
    *(int64_t*)v7 = (int64_t)v6;
    goto bb1;
  bb1:
    v10 = *(int64_t*)v7;
    v11 = 0LL;
    v12 = v10 > v11;
    if (v12) goto bb2;
    else goto bb3;
  bb2:
    v14 = *(int64_t*)v7;
    v15 = (int64_t)v1 + v14;
    v16 = 1LL;
    v17 = v15 - v16;
    v18 = malloc(8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = *(int64_t*)v18;
    v21 = malloc(8);
    *(void**)v21 = (void*)v20;
    v23 = 0LL;
    v24 = malloc(8);
    *(int64_t*)v24 = (int64_t)v23;
    v27 = *(void**)v21;
    v28 = *(uint8_t*)v27;
    *(int64_t*)v24 = (int64_t)v28;
    v31 = *(int64_t*)v24;
    v32 = 0LL;
    v33 = (bool)char_is_whitespace((void*)v32, v31);
    v34 = !v33;
    if (v34) goto bb4;
    else goto bb5;
  bb3:
    v43 = *(int64_t*)v7;
    v44 = 0LL;
    v45 = v43 <= v44;
    if (v45) goto bb7;
    else goto bb8;
  bb4:
    goto bb3;
  bb5:
    goto bb6;
  bb6:
    v38 = *(int64_t*)v7;
    v39 = 1LL;
    v40 = v38 - v39;
    *(int64_t*)v7 = (int64_t)v40;
    goto bb1;
  bb7:
    v47 = _str8;
    p_call_stack_pop();
    return v47;
  bb8:
    goto bb9;
  bb9:
    v50 = 0LL;
    v51 = *(int64_t*)v7;
    v52 = 0LL;
    v53 = (int64_t)String_slice((void*)v52, (int64_t)v1, v50, v51);
    p_call_stack_pop();
    return (void*)v53;
    return 0;
}

void ByteBuffer_write_byte(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("ByteBuffer_write_byte");
    int64_t v11;
    int64_t v12; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    void* v3;
    int64_t v5;
    int64_t v9;
    void* v8;
    void* v4;
    void* v7;
    int64_t v6; /* placeholder for void */
    int64_t v10;
    v3 = (char*)v1 + 0;
    v4 = *(void**)v3;
    v5 = 0LL;
    Vec_push((void*)v5, (int64_t)v4, v2);
    v7 = (char*)v1 + 8;
    v8 = (char*)v1 + 8;
    v9 = *(int64_t*)v8;
    v10 = 1LL;
    v11 = v9 + v10;
    *(int64_t*)v7 = (int64_t)v11;
    p_call_stack_pop();
    return;
    return;
}

int64_t HashMap_len(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_len");
    int64_t v12; /* placeholder for void */
    void* v4;
    int64_t v9; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v3;
    int64_t v6;
    void* v7;
    int64_t v11;
    int64_t v14;
    int64_t v13; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v10;
    int64_t v2;
    v2 = 16LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = 0LL;
    v7 = malloc(8);
    *(int64_t*)v7 = (int64_t)v6;
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v7 = (int64_t)v11;
    v14 = *(int64_t*)v7;
    p_call_stack_pop();
    return v14;
    return 0;
}

int64_t str_find(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_find");
    int64_t v4;
    int64_t v5; /* placeholder for void */
    int64_t v3;
    v3 = 0LL;
    v4 = (int64_t)String_find((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

void Option_and_then(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Option_and_then");
    void* v36;
    int64_t v46; /* placeholder for void */
    int64_t v10;
    int64_t v14; /* placeholder for void */
    int64_t v28;
    int64_t v38;
    void* v4;
    int64_t v39; /* placeholder for void */
    int64_t v33;
    void* v18;
    int64_t v20;
    void* v13;
    int64_t v26; /* placeholder for void */
    void* v34;
    int64_t v35; /* placeholder for void */
    void* v40;
    int64_t v5;
    int64_t v12;
    int64_t v45; /* placeholder for void */
    int64_t v6;
    int64_t v21; /* placeholder for void */
    void* v19;
    int64_t v29;
    int64_t v42;
    bool v7;
    bool v30;
    void* v22;
    void* v32;
    void* v37;
    int64_t v24;
    int64_t v8; /* placeholder for void */
    void* v9;
    void* v23;
    void* v41;
    int64_t v15;
    int64_t v44; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    void* v27;
    int64_t v25; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v11;
    void* v16;
    int64_t v43; /* placeholder for void */
    v4 = (char*)v2 + 0;
    v5 = *(int64_t*)v4;
    v6 = 0LL;
    v7 = v5 == v6;
    if (v7) goto bb2;
    else goto bb3;
  bb1:
    p_call_stack_pop();
    return;
  bb2:
    v9 = (char*)v2 + 8;
    v10 = *(int64_t*)v9;
    v11 = malloc(16);
    v12 = 0LL;
    v13 = (char*)v11 + 0;
    *(int64_t*)v13 = (int64_t)v12;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ndim") == 0) {
            v15 = ndim((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "size") == 0) {
            v15 = size((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (int64_t)v10);
        } else { abort(); }
    }
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
    p_call_stack_pop();
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
    p_call_stack_pop();
    return;
  bb5:
    goto bb1;
    return;
}

int64_t String_push_str(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_push_str");
    int64_t v8;
    int64_t v27;
    int64_t v31;
    int64_t v19;
    void* v34;
    int64_t v36;
    int64_t v39; /* placeholder for void */
    int64_t v23;
    int64_t v33;
    int64_t v6; /* placeholder for void */
    int64_t v12;
    int64_t v7;
    void* v16;
    int64_t v25;
    int64_t v32;
    void* v37;
    int64_t v42; /* placeholder for void */
    int64_t v3;
    int64_t v44;
    int64_t v24;
    int64_t v30;
    int64_t v21;
    int64_t v11;
    int64_t v20;
    void* v9;
    int64_t v10; /* placeholder for void */
    int64_t v26;
    int64_t v29;
    int64_t v35; /* placeholder for void */
    void* v40;
    int64_t v45; /* placeholder for void */
    int64_t v41;
    void* v5;
    int64_t v4;
    int64_t v13;
    int64_t v43; /* placeholder for void */
    int64_t v15;
    void* v17;
    int64_t v14;
    int64_t v18; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)p_str_length((void*)v3, v1);
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = 0LL;
    v8 = (int64_t)p_str_length((void*)v7, v2);
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = *(int64_t*)v5;
    v12 = *(int64_t*)v9;
    v13 = v11 + v12;
    v14 = 1LL;
    v15 = v13 + v14;
    v16 = p_malloc(v15);
    v17 = malloc(8);
    *(int64_t*)v17 = (int64_t)v16;
    v19 = *(int64_t*)v17;
    v20 = *(int64_t*)v5;
    v21 = 0LL;
    p_str_copy_n((void*)v21, v19, v1, v20);
    v23 = *(int64_t*)v17;
    v24 = *(int64_t*)v5;
    v25 = v23 + v24;
    v26 = *(int64_t*)v9;
    v27 = 0LL;
    p_str_copy_n((void*)v27, v25, v2, v26);
    v29 = *(int64_t*)v17;
    v30 = *(int64_t*)v5;
    v31 = v29 + v30;
    v32 = *(int64_t*)v9;
    v33 = v31 + v32;
    v34 = malloc(8);
    *(int64_t*)v34 = (int64_t)v33;
    v36 = *(int64_t*)v34;
    v37 = malloc(8);
    *(void**)v37 = (void*)v36;
    v40 = *(void**)v37;
    v41 = 0LL;
    *(uint8_t*)v40 = (uint8_t)v41;
    v44 = *(int64_t*)v17;
    p_call_stack_pop();
    return v44;
    return 0;
}

int64_t String_to_string(void* v0, int64_t v1) {
    p_call_stack_push("String_to_string");
    int64_t v3;
    int64_t v4; /* placeholder for void */
    int64_t v2;
    v2 = 0LL;
    v3 = (int64_t)String_from((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

void* ones(void* v0, void* v1) {
    p_call_stack_push("ones");
    void* v2;
    int64_t v3; /* placeholder for void */
    v2 = p_tensor_ones(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

bool char_is_lower(void* v0, int64_t v1) {
    p_call_stack_push("char_is_lower");
    bool v5;
    bool v6;
    bool v3;
    int64_t v7; /* placeholder for void */
    int64_t v2;
    int64_t v4;
    v2 = 97LL;
    v3 = v1 >= v2;
    v4 = 122LL;
    v5 = v1 <= v4;
    v6 = v3 && v5;
    p_call_stack_pop();
    return v6;
    return 0;
}

void HashMap_grow(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_grow");
    int64_t v149; /* placeholder for void */
    void* v103;
    int64_t v131;
    int64_t v50;
    int64_t v156; /* placeholder for void */
    void* v53;
    int64_t v122; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v125; /* placeholder for void */
    int64_t v210;
    int64_t v215;
    int64_t v115;
    int64_t v70;
    int64_t v139; /* placeholder for void */
    int64_t v51;
    bool v41;
    void* v190;
    void* v214;
    int64_t v188; /* placeholder for void */
    int64_t v229; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    int64_t v157; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v107; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v267; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    int64_t v189; /* placeholder for void */
    void* v145;
    void* v151;
    int64_t v220;
    int64_t v207;
    void* v161;
    int64_t v216; /* placeholder for void */
    int64_t v8;
    int64_t v143;
    int64_t v162; /* placeholder for void */
    int64_t v217; /* placeholder for void */
    void* v84;
    int64_t v89; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    void* v74;
    int64_t v92; /* placeholder for void */
    int64_t v219; /* placeholder for void */
    int64_t v235; /* placeholder for void */
    void* v93;
    int64_t v182; /* placeholder for void */
    int64_t v43;
    int64_t v95; /* placeholder for void */
    int64_t v249;
    int64_t v224;
    int64_t v222;
    int64_t v116;
    void* v211;
    int64_t v261; /* placeholder for void */
    int64_t v171; /* placeholder for void */
    int64_t v120;
    int64_t v253; /* placeholder for void */
    int64_t v260;
    void* v166;
    int64_t v251; /* placeholder for void */
    int64_t v136;
    int64_t v231; /* placeholder for void */
    int64_t v202; /* placeholder for void */
    int64_t v135;
    int64_t v25; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v33;
    int64_t v68; /* placeholder for void */
    int64_t v76;
    int64_t v114;
    void* v118;
    int64_t v73;
    int64_t v134;
    int64_t v152; /* placeholder for void */
    void* v4;
    int64_t v146; /* placeholder for void */
    void* v169;
    int64_t v48; /* placeholder for void */
    int64_t v244;
    int64_t v254; /* placeholder for void */
    int64_t v140;
    bool v250;
    int64_t v221;
    int64_t v65;
    void* v181;
    bool v226;
    int64_t v270; /* placeholder for void */
    int64_t v180;
    int64_t v113;
    int64_t v163;
    int64_t v192; /* placeholder for void */
    int64_t v150;
    int64_t v223; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v119; /* placeholder for void */
    int64_t v82;
    int64_t v110;
    int64_t v230; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    bool v137;
    int64_t v164;
    void* v208;
    void* v23;
    void* v77;
    int64_t v266;
    int64_t v167; /* placeholder for void */
    void* v271;
    int64_t v194;
    int64_t v264; /* placeholder for void */
    int64_t v22;
    int64_t v86;
    int64_t v177;
    int64_t v186;
    bool v67;
    void* v58;
    int64_t v27;
    int64_t v193; /* placeholder for void */
    int64_t v72;
    int64_t v234;
    int64_t v241; /* placeholder for void */
    void* v265;
    int64_t v278; /* placeholder for void */
    int64_t v69;
    int64_t v225;
    int64_t v128;
    int64_t v49;
    int64_t v238;
    int64_t v81;
    int64_t v185; /* placeholder for void */
    int64_t v66;
    void* v148;
    void* v154;
    int64_t v243;
    int64_t v3;
    int64_t v28; /* placeholder for void */
    int64_t v159;
    int64_t v205;
    int64_t v36;
    int64_t v213; /* placeholder for void */
    int64_t v117;
    bool v174;
    void* v26;
    int64_t v209; /* placeholder for void */
    int64_t v252;
    void* v274;
    int64_t v212; /* placeholder for void */
    int64_t v102;
    int64_t v35; /* placeholder for void */
    int64_t v71;
    int64_t v233;
    int64_t v56;
    int64_t v61;
    int64_t v75; /* placeholder for void */
    int64_t v39;
    void* v52;
    int64_t v15; /* placeholder for void */
    int64_t v98;
    int64_t v155;
    int64_t v165;
    int64_t v183;
    void* v59;
    int64_t v112; /* placeholder for void */
    int64_t v236; /* placeholder for void */
    int64_t v273; /* placeholder for void */
    int64_t v276; /* placeholder for void */
    int64_t v277; /* placeholder for void */
    int64_t v144;
    void* v6;
    void* v20;
    int64_t v88; /* placeholder for void */
    int64_t v204;
    int64_t v262; /* placeholder for void */
    int64_t v242; /* placeholder for void */
    int64_t v160;
    int64_t v38; /* placeholder for void */
    int64_t v245;
    void* v268;
    int64_t v55;
    void* v124;
    void* v87;
    void* v106;
    int64_t v228;
    int64_t v240; /* placeholder for void */
    int64_t v237;
    int64_t v239;
    int64_t v142;
    int64_t v47; /* placeholder for void */
    int64_t v170; /* placeholder for void */
    int64_t v85; /* placeholder for void */
    int64_t v91;
    int64_t v175; /* placeholder for void */
    int64_t v132;
    int64_t v18; /* placeholder for void */
    int64_t v248;
    int64_t v19;
    bool v196;
    int64_t v78; /* placeholder for void */
    int64_t v2;
    void* v32;
    int64_t v123;
    bool v133;
    int64_t v257;
    int64_t v31; /* placeholder for void */
    int64_t v191;
    bool v258;
    int64_t v64; /* placeholder for void */
    int64_t v83;
    int64_t v105;
    void* v90;
    void* v17;
    int64_t v203;
    int64_t v168;
    int64_t v109;
    int64_t v195;
    int64_t v141;
    void* v29;
    int64_t v147;
    int64_t v227; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    void* v10;
    int64_t v40;
    int64_t v54; /* placeholder for void */
    void* v62;
    int64_t v255; /* placeholder for void */
    int64_t v259; /* placeholder for void */
    int64_t v129; /* placeholder for void */
    bool v111;
    int64_t v21; /* placeholder for void */
    int64_t v30;
    int64_t v256;
    int64_t v57;
    void* v37;
    void* v199;
    int64_t v263; /* placeholder for void */
    int64_t v275;
    int64_t v130; /* placeholder for void */
    int64_t v94;
    int64_t v99;
    int64_t v232;
    int64_t v269;
    int64_t v13;
    int64_t v178;
    int64_t v247; /* placeholder for void */
    int64_t v79;
    void* v184;
    int64_t v172;
    int64_t v179;
    int64_t v153; /* placeholder for void */
    void* v187;
    int64_t v16;
    int64_t v173;
    void* v127;
    int64_t v197; /* placeholder for void */
    void* v14;
    int64_t v218; /* placeholder for void */
    int64_t v12;
    int64_t v100; /* placeholder for void */
    int64_t v158;
    int64_t v198; /* placeholder for void */
    int64_t v45;
    int64_t v42; /* placeholder for void */
    bool v138;
    int64_t v200;
    int64_t v201; /* placeholder for void */
    int64_t v80;
    int64_t v176;
    int64_t v108; /* placeholder for void */
    int64_t v44;
    void* v121;
    int64_t v126; /* placeholder for void */
    int64_t v97;
    int64_t v9;
    int64_t v272;
    int64_t v206;
    int64_t v246; /* placeholder for void */
    v2 = 24LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = malloc(8);
    *(void**)v6 = (void*)v1;
    v8 = 8LL;
    v9 = v1 + v8;
    v10 = malloc(8);
    *(void**)v10 = (void*)v9;
    v12 = 16LL;
    v13 = v1 + v12;
    v14 = malloc(8);
    *(void**)v14 = (void*)v13;
    v16 = 0LL;
    v17 = malloc(8);
    *(int64_t*)v17 = (int64_t)v16;
    v19 = 0LL;
    v20 = malloc(8);
    *(int64_t*)v20 = (int64_t)v19;
    v22 = 0LL;
    v23 = malloc(8);
    *(int64_t*)v23 = (int64_t)v22;
    v26 = *(void**)v4;
    v27 = *(int64_t*)v26;
    *(int64_t*)v17 = (int64_t)v27;
    v29 = *(void**)v6;
    v30 = *(int64_t*)v29;
    *(int64_t*)v20 = (int64_t)v30;
    v32 = *(void**)v10;
    v33 = *(int64_t*)v32;
    *(int64_t*)v23 = (int64_t)v33;
    v36 = 8LL;
    v37 = malloc(8);
    *(int64_t*)v37 = (int64_t)v36;
    v39 = *(int64_t*)v17;
    v40 = 0LL;
    v41 = v39 > v40;
    if (v41) goto bb1;
    else goto bb2;
  bb1:
    v43 = *(int64_t*)v17;
    v44 = 2LL;
    v45 = v43 * v44;
    *(int64_t*)v37 = (int64_t)v45;
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v49 = *(int64_t*)v37;
    v50 = 8LL;
    v51 = v49 * v50;
    v52 = p_malloc(v51);
    v53 = malloc(8);
    *(int64_t*)v53 = (int64_t)v52;
    v55 = *(int64_t*)v37;
    v56 = 8LL;
    v57 = v55 * v56;
    v58 = p_malloc(v57);
    v59 = malloc(8);
    *(int64_t*)v59 = (int64_t)v58;
    v61 = 0LL;
    v62 = malloc(8);
    *(int64_t*)v62 = (int64_t)v61;
    goto bb4;
  bb4:
    v65 = *(int64_t*)v62;
    v66 = *(int64_t*)v37;
    v67 = v65 < v66;
    if (v67) goto bb5;
    else goto bb6;
  bb5:
    v69 = *(int64_t*)v53;
    v70 = *(int64_t*)v62;
    v71 = 8LL;
    v72 = v70 * v71;
    v73 = v69 + v72;
    v74 = malloc(8);
    *(int64_t*)v74 = (int64_t)v73;
    v76 = *(int64_t*)v74;
    v77 = malloc(8);
    *(void**)v77 = (void*)v76;
    v79 = *(int64_t*)v59;
    v80 = *(int64_t*)v62;
    v81 = 8LL;
    v82 = v80 * v81;
    v83 = v79 + v82;
    v84 = malloc(8);
    *(int64_t*)v84 = (int64_t)v83;
    v86 = *(int64_t*)v84;
    v87 = malloc(8);
    *(void**)v87 = (void*)v86;
    v90 = *(void**)v77;
    v91 = 0LL;
    *(int64_t*)v90 = (int64_t)v91;
    v93 = *(void**)v87;
    v94 = 0LL;
    *(int64_t*)v93 = (int64_t)v94;
    v97 = *(int64_t*)v62;
    v98 = 1LL;
    v99 = v97 + v98;
    *(int64_t*)v62 = (int64_t)v99;
    goto bb4;
  bb6:
    v102 = 0LL;
    v103 = malloc(8);
    *(int64_t*)v103 = (int64_t)v102;
    v105 = 0LL;
    v106 = malloc(8);
    *(int64_t*)v106 = (int64_t)v105;
    goto bb7;
  bb7:
    v109 = *(int64_t*)v106;
    v110 = *(int64_t*)v17;
    v111 = v109 < v110;
    if (v111) goto bb8;
    else goto bb9;
  bb8:
    v113 = *(int64_t*)v20;
    v114 = *(int64_t*)v106;
    v115 = 8LL;
    v116 = v114 * v115;
    v117 = v113 + v116;
    v118 = malloc(8);
    *(int64_t*)v118 = (int64_t)v117;
    v120 = *(int64_t*)v118;
    v121 = malloc(8);
    *(void**)v121 = (void*)v120;
    v123 = 0LL;
    v124 = malloc(8);
    *(int64_t*)v124 = (int64_t)v123;
    v127 = *(void**)v121;
    v128 = *(int64_t*)v127;
    *(int64_t*)v124 = (int64_t)v128;
    v131 = *(int64_t*)v124;
    v132 = 0LL;
    v133 = v131 != v132;
    v134 = *(int64_t*)v124;
    v135 = 1LL;
    v136 = -v135;
    v137 = v134 != v136;
    v138 = v133 && v137;
    if (v138) goto bb10;
    else goto bb11;
  bb9:
    v248 = *(int64_t*)v20;
    v249 = 0LL;
    v250 = v248 != v249;
    if (v250) goto bb22;
    else goto bb23;
  bb10:
    v140 = *(int64_t*)v23;
    v141 = *(int64_t*)v106;
    v142 = 8LL;
    v143 = v141 * v142;
    v144 = v140 + v143;
    v145 = malloc(8);
    *(int64_t*)v145 = (int64_t)v144;
    v147 = *(int64_t*)v145;
    v148 = malloc(8);
    *(void**)v148 = (void*)v147;
    v150 = 0LL;
    v151 = malloc(8);
    *(int64_t*)v151 = (int64_t)v150;
    v154 = *(void**)v148;
    v155 = *(int64_t*)v154;
    *(int64_t*)v151 = (int64_t)v155;
    v158 = *(int64_t*)v124;
    v159 = 0LL;
    v160 = (int64_t)p_str_hash((void*)v159, v158);
    v161 = malloc(8);
    *(int64_t*)v161 = (int64_t)v160;
    v163 = *(int64_t*)v161;
    v164 = *(int64_t*)v37;
    v165 = v163 % v164;
    v166 = malloc(8);
    *(int64_t*)v166 = (int64_t)v165;
    v168 = 0LL;
    v169 = malloc(8);
    *(int64_t*)v169 = (int64_t)v168;
    goto bb13;
  bb11:
    goto bb12;
  bb12:
    v243 = *(int64_t*)v106;
    v244 = 1LL;
    v245 = v243 + v244;
    *(int64_t*)v106 = (int64_t)v245;
    goto bb7;
  bb13:
    v172 = *(int64_t*)v169;
    v173 = *(int64_t*)v37;
    v174 = v172 < v173;
    if (v174) goto bb14;
    else goto bb15;
  bb14:
    v176 = *(int64_t*)v53;
    v177 = *(int64_t*)v166;
    v178 = 8LL;
    v179 = v177 * v178;
    v180 = v176 + v179;
    v181 = malloc(8);
    *(int64_t*)v181 = (int64_t)v180;
    v183 = *(int64_t*)v181;
    v184 = malloc(8);
    *(void**)v184 = (void*)v183;
    v186 = 0LL;
    v187 = malloc(8);
    *(int64_t*)v187 = (int64_t)v186;
    v190 = *(void**)v184;
    v191 = *(int64_t*)v190;
    *(int64_t*)v187 = (int64_t)v191;
    v194 = *(int64_t*)v187;
    v195 = 0LL;
    v196 = v194 == v195;
    if (v196) goto bb16;
    else goto bb17;
  bb15:
    v237 = *(int64_t*)v103;
    v238 = 1LL;
    v239 = v237 + v238;
    *(int64_t*)v103 = (int64_t)v239;
    goto bb12;
  bb16:
    v199 = *(void**)v184;
    v200 = *(int64_t*)v124;
    *(int64_t*)v199 = (int64_t)v200;
    v203 = *(int64_t*)v59;
    v204 = *(int64_t*)v166;
    v205 = 8LL;
    v206 = v204 * v205;
    v207 = v203 + v206;
    v208 = malloc(8);
    *(int64_t*)v208 = (int64_t)v207;
    v210 = *(int64_t*)v208;
    v211 = malloc(8);
    *(void**)v211 = (void*)v210;
    v214 = *(void**)v211;
    v215 = *(int64_t*)v151;
    *(int64_t*)v214 = (int64_t)v215;
    goto bb15;
  bb17:
    goto bb18;
  bb18:
    v220 = *(int64_t*)v166;
    v221 = 1LL;
    v222 = v220 + v221;
    *(int64_t*)v166 = (int64_t)v222;
    v224 = *(int64_t*)v166;
    v225 = *(int64_t*)v37;
    v226 = v224 >= v225;
    if (v226) goto bb19;
    else goto bb20;
  bb19:
    v228 = 0LL;
    *(int64_t*)v166 = (int64_t)v228;
    goto bb21;
  bb20:
    goto bb21;
  bb21:
    v232 = *(int64_t*)v169;
    v233 = 1LL;
    v234 = v232 + v233;
    *(int64_t*)v169 = (int64_t)v234;
    goto bb13;
  bb22:
    v252 = *(int64_t*)v20;
    p_free((void*)v252);
    goto bb24;
  bb23:
    goto bb24;
  bb24:
    v256 = *(int64_t*)v23;
    v257 = 0LL;
    v258 = v256 != v257;
    if (v258) goto bb25;
    else goto bb26;
  bb25:
    v260 = *(int64_t*)v23;
    p_free((void*)v260);
    goto bb27;
  bb26:
    goto bb27;
  bb27:
    v265 = *(void**)v6;
    v266 = *(int64_t*)v53;
    *(int64_t*)v265 = (int64_t)v266;
    v268 = *(void**)v10;
    v269 = *(int64_t*)v59;
    *(int64_t*)v268 = (int64_t)v269;
    v271 = *(void**)v14;
    v272 = *(int64_t*)v103;
    *(int64_t*)v271 = (int64_t)v272;
    v274 = *(void**)v4;
    v275 = *(int64_t*)v37;
    *(int64_t*)v274 = (int64_t)v275;
    p_call_stack_pop();
    return;
    return;
}

int64_t clamp(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("clamp");
    int64_t v10; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    bool v4;
    int64_t v6; /* placeholder for void */
    bool v8;
    v4 = v1 < v2;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return v2;
  bb2:
    goto bb3;
  bb3:
    v8 = v1 > v3;
    if (v8) goto bb4;
    else goto bb5;
  bb4:
    p_call_stack_pop();
    return v3;
  bb5:
    goto bb6;
  bb6:
    p_call_stack_pop();
    return v1;
    return 0;
}

void* str_replace(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("str_replace");
    int64_t v5;
    int64_t v4;
    int64_t v6; /* placeholder for void */
    v4 = 0LL;
    v5 = (int64_t)String_replace((void*)v4, (int64_t)v1, (int64_t)v2, (int64_t)v3);
    p_call_stack_pop();
    return (void*)v5;
    return 0;
}

int64_t Option_unwrap_or_else(void* v0, void* v1, void* v2) {
    p_call_stack_push("Option_unwrap_or_else");
    int64_t v4;
    int64_t v15; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    bool v14;
    int64_t v19;
    int64_t v20; /* placeholder for void */
    int64_t v12;
    bool v6;
    int64_t v17; /* placeholder for void */
    void* v8;
    int64_t v18; /* placeholder for void */
    int64_t v5;
    int64_t v10; /* placeholder for void */
    void* v3;
    int64_t v13;
    void* v11;
    int64_t v16;
    int64_t v9;
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    v19 = 0LL;
    p_call_stack_pop();
    return v19;
  bb2:
    v8 = (char*)v1 + 8;
    v9 = *(int64_t*)v8;
    p_call_stack_pop();
    return v9;
  bb3:
    v11 = (char*)v1 + 0;
    v12 = *(int64_t*)v11;
    v13 = 1LL;
    v14 = v12 == v13;
    if (v14) goto bb4;
    else goto bb5;
  bb4:
    {
        if (!v2) abort();
        char* _fn_name = *(char**)v2;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "Vec_new") == 0) {
            v16 = Vec_new((void*)v2);
        } else if (strcmp(_fn_name, "HashMap_new") == 0) {
            v16 = HashMap_new((void*)v2);
        } else { abort(); }
    }
    p_call_stack_pop();
    return v16;
  bb5:
    goto bb1;
    return 0;
}

int64_t str_rfind(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_rfind");
    int64_t v3;
    int64_t v4;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)String_rfind((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

bool Result_is_err(void* v0, void* v1) {
    p_call_stack_push("Result_is_err");
    int64_t v12;
    int64_t v8;
    int64_t v20; /* placeholder for void */
    int64_t v13;
    int64_t v4;
    int64_t v19; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v11;
    bool v18;
    int64_t v22; /* placeholder for void */
    bool v14;
    void* v2;
    bool v21;
    bool v5;
    bool v9;
    void* v16;
    void* v7;
    int64_t v3;
    int64_t v17;
    int64_t v15; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v21 = false;
    p_call_stack_pop();
    return v21;
  bb2:
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    v9 = false;
    p_call_stack_pop();
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
    p_call_stack_pop();
    return v18;
  bb5:
    goto bb1;
    return 0;
}

int64_t Result_unwrap_or(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Result_unwrap_or");
    int64_t v7; /* placeholder for void */
    int64_t v5;
    int64_t v19; /* placeholder for void */
    int64_t v4;
    int64_t v21; /* placeholder for void */
    int64_t v13;
    int64_t v18; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v12;
    bool v6;
    int64_t v20;
    int64_t v17;
    int64_t v9;
    int64_t v10; /* placeholder for void */
    void* v8;
    void* v11;
    void* v16;
    bool v14;
    void* v3;
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    v20 = 0LL;
    p_call_stack_pop();
    return v20;
  bb2:
    v8 = (char*)v1 + 8;
    v9 = *(int64_t*)v8;
    p_call_stack_pop();
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
    p_call_stack_pop();
    return v2;
  bb5:
    goto bb1;
    return 0;
}

void* str_split_char(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("str_split_char");
    int64_t v28; /* placeholder for void */
    int64_t v14;
    int64_t v16; /* placeholder for void */
    int64_t v49;
    int64_t v59;
    void* v27;
    int64_t v36; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    void* v8;
    int64_t v52; /* placeholder for void */
    int64_t v44;
    int64_t v18;
    bool v20;
    uint8_t v34;
    int64_t v22;
    int64_t v57; /* placeholder for void */
    int64_t v19;
    int64_t v13; /* placeholder for void */
    int64_t v11;
    int64_t v41;
    int64_t v62;
    int64_t v25; /* placeholder for void */
    int64_t v63;
    void* v5;
    int64_t v35; /* placeholder for void */
    int64_t v45;
    int64_t v48;
    int64_t v37;
    int64_t v64; /* placeholder for void */
    int64_t v60;
    void* v65;
    int64_t v3;
    int64_t v9; /* placeholder for void */
    void* v4;
    int64_t v31; /* placeholder for void */
    bool v38;
    int64_t v47;
    int64_t v53;
    int64_t v66; /* placeholder for void */
    int64_t v55;
    int64_t v61;
    void* v12;
    int64_t v21; /* placeholder for void */
    int64_t v26;
    int64_t v50; /* placeholder for void */
    void* v15;
    int64_t v54;
    int64_t v7;
    void* v33;
    int64_t v43;
    int64_t v10;
    int64_t v17; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    void* v40;
    int64_t v51; /* placeholder for void */
    int64_t v23;
    void* v58;
    void* v24;
    void* v30;
    int64_t v42;
    int64_t v46; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v29;
    int64_t v56; /* placeholder for void */
    v3 = 0LL;
    v4 = (void*)Vec_new((void*)v3);
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = 0LL;
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    v10 = 0LL;
    v11 = (int64_t)String_len((void*)v10, (int64_t)v1);
    v12 = malloc(8);
    *(int64_t*)v12 = (int64_t)v11;
    v14 = 0LL;
    v15 = malloc(8);
    *(int64_t*)v15 = (int64_t)v14;
    goto bb1;
  bb1:
    v18 = *(int64_t*)v15;
    v19 = *(int64_t*)v12;
    v20 = v18 < v19;
    if (v20) goto bb2;
    else goto bb3;
  bb2:
    v22 = *(int64_t*)v15;
    v23 = (int64_t)v1 + v22;
    v24 = malloc(8);
    *(int64_t*)v24 = (int64_t)v23;
    v26 = *(int64_t*)v24;
    v27 = malloc(8);
    *(void**)v27 = (void*)v26;
    v29 = 0LL;
    v30 = malloc(8);
    *(int64_t*)v30 = (int64_t)v29;
    v33 = *(void**)v27;
    v34 = *(uint8_t*)v33;
    *(int64_t*)v30 = (int64_t)v34;
    v37 = *(int64_t*)v30;
    v38 = v37 == v2;
    if (v38) goto bb4;
    else goto bb5;
  bb3:
    v58 = *(void**)v5;
    v59 = *(int64_t*)v8;
    v60 = *(int64_t*)v12;
    v61 = 0LL;
    v62 = (int64_t)String_slice((void*)v61, (int64_t)v1, v59, v60);
    v63 = 0LL;
    Vec_push((void*)v63, (int64_t)v58, v62);
    v65 = *(void**)v5;
    p_call_stack_pop();
    return v65;
  bb4:
    v40 = *(void**)v5;
    v41 = *(int64_t*)v8;
    v42 = *(int64_t*)v15;
    v43 = 0LL;
    v44 = (int64_t)String_slice((void*)v43, (int64_t)v1, v41, v42);
    v45 = 0LL;
    Vec_push((void*)v45, (int64_t)v40, v44);
    v47 = *(int64_t*)v15;
    v48 = 1LL;
    v49 = v47 + v48;
    *(int64_t*)v8 = (int64_t)v49;
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v53 = *(int64_t*)v15;
    v54 = 1LL;
    v55 = v53 + v54;
    *(int64_t*)v15 = (int64_t)v55;
    goto bb1;
    return 0;
}

void* ByteBuffer_to_str(void* v0, void* v1) {
    p_call_stack_push("ByteBuffer_to_str");
    int64_t v11;
    void* v19;
    int64_t v18;
    int64_t v8; /* placeholder for void */
    void* v2;
    void* v6;
    void* v21;
    int64_t v30; /* placeholder for void */
    void* v14;
    int64_t v22;
    int64_t v27;
    int64_t v20; /* placeholder for void */
    void* v10;
    void* v3;
    int64_t v4; /* placeholder for void */
    int64_t v26;
    int64_t v5;
    void* v23;
    int64_t v17;
    void* v24;
    int64_t v25; /* placeholder for void */
    int64_t v28;
    int64_t v16;
    int64_t v29; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    void* v31;
    bool v12;
    int64_t v9;
    int64_t v32; /* placeholder for void */
    void* v15;
    v2 = _str8;
    v3 = malloc(8);
    *(void**)v3 = (void*)v2;
    v5 = 0LL;
    v6 = malloc(8);
    *(int64_t*)v6 = (int64_t)v5;
    goto bb1;
  bb1:
    v9 = *(int64_t*)v6;
    v10 = (char*)v1 + 8;
    v11 = *(int64_t*)v10;
    v12 = v9 < v11;
    if (v12) goto bb2;
    else goto bb3;
  bb2:
    v14 = (char*)v1 + 0;
    v15 = *(void**)v14;
    v16 = *(int64_t*)v6;
    v17 = 0LL;
    v18 = (int64_t)Vec_get((void*)v17, (int64_t)v15, v16);
    v19 = malloc(8);
    *(int64_t*)v19 = (int64_t)v18;
    v21 = *(void**)v3;
    v22 = *(int64_t*)v19;
    v23 = p_char_to_str(v22);
    v24 = p_str_concat(v21, v23);
    *(void**)v3 = (void*)v24;
    v26 = *(int64_t*)v6;
    v27 = 1LL;
    v28 = v26 + v27;
    *(int64_t*)v6 = (int64_t)v28;
    goto bb1;
  bb3:
    v31 = *(void**)v3;
    p_call_stack_pop();
    return v31;
    return 0;
}

void ByteBuffer_clear(void* v0, void* v1) {
    p_call_stack_push("ByteBuffer_clear");
    void* v2;
    int64_t v4;
    int64_t v9; /* placeholder for void */
    void* v3;
    int64_t v7;
    void* v6;
    int64_t v8; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = *(void**)v2;
    v4 = 0LL;
    Vec_clear((void*)v4, (int64_t)v3);
    v6 = (char*)v1 + 8;
    v7 = 0LL;
    *(int64_t*)v6 = (int64_t)v7;
    p_call_stack_pop();
    return;
    return;
}

void* shape(void* v0, void* v1) {
    p_call_stack_push("shape");
    int64_t v3; /* placeholder for void */
    void* v2;
    v2 = p_tensor_shape(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t HashMap_keys(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_keys");
    int64_t v5; /* placeholder for void */
    int64_t v72;
    int64_t v23;
    int64_t v58; /* placeholder for void */
    int64_t v73;
    int64_t v60;
    int64_t v44; /* placeholder for void */
    void* v53;
    int64_t v51; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    void* v59;
    int64_t v11;
    int64_t v83;
    int64_t v63;
    int64_t v41;
    int64_t v66;
    int64_t v10; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v16;
    void* v18;
    int64_t v42;
    int64_t v30;
    int64_t v27;
    bool v31;
    int64_t v21; /* placeholder for void */
    void* v56;
    void* v12;
    void* v15;
    int64_t v25; /* placeholder for void */
    int64_t v48;
    void* v38;
    int64_t v37;
    int64_t v68;
    int64_t v55;
    int64_t v62; /* placeholder for void */
    bool v32;
    int64_t v71; /* placeholder for void */
    int64_t v2;
    void* v6;
    int64_t v8;
    void* v9;
    int64_t v34;
    int64_t v61; /* placeholder for void */
    bool v69;
    int64_t v57; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v22;
    int64_t v75; /* placeholder for void */
    int64_t v29;
    int64_t v45;
    int64_t v81; /* placeholder for void */
    int64_t v49;
    int64_t v52;
    int64_t v54; /* placeholder for void */
    int64_t v79;
    int64_t v3;
    bool v28;
    int64_t v80;
    int64_t v26;
    void* v4;
    int64_t v14; /* placeholder for void */
    void* v24;
    int64_t v46;
    bool v43;
    void* v50;
    bool v65;
    bool v70;
    int64_t v39; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v47;
    int64_t v67;
    int64_t v64;
    int64_t v84; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    int64_t v78;
    int64_t v19;
    int64_t v74;
    v2 = 24LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = malloc(8);
    *(void**)v6 = (void*)v1;
    v8 = 0LL;
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 0LL;
    v12 = malloc(8);
    *(int64_t*)v12 = (int64_t)v11;
    v15 = *(void**)v4;
    v16 = *(int64_t*)v15;
    *(int64_t*)v9 = (int64_t)v16;
    v18 = *(void**)v6;
    v19 = *(int64_t*)v18;
    *(int64_t*)v12 = (int64_t)v19;
    v22 = 0LL;
    v23 = (int64_t)Vec_new((void*)v22);
    v24 = malloc(8);
    *(int64_t*)v24 = (int64_t)v23;
    v26 = *(int64_t*)v12;
    v27 = 0LL;
    v28 = v26 == v27;
    v29 = *(int64_t*)v9;
    v30 = 0LL;
    v31 = v29 == v30;
    v32 = v28 || v31;
    if (v32) goto bb1;
    else goto bb2;
  bb1:
    v34 = *(int64_t*)v24;
    p_call_stack_pop();
    return v34;
  bb2:
    goto bb3;
  bb3:
    v37 = 0LL;
    v38 = malloc(8);
    *(int64_t*)v38 = (int64_t)v37;
    goto bb4;
  bb4:
    v41 = *(int64_t*)v38;
    v42 = *(int64_t*)v9;
    v43 = v41 < v42;
    if (v43) goto bb5;
    else goto bb6;
  bb5:
    v45 = *(int64_t*)v12;
    v46 = *(int64_t*)v38;
    v47 = 8LL;
    v48 = v46 * v47;
    v49 = v45 + v48;
    v50 = malloc(8);
    *(int64_t*)v50 = (int64_t)v49;
    v52 = *(int64_t*)v50;
    v53 = malloc(8);
    *(void**)v53 = (void*)v52;
    v55 = 0LL;
    v56 = malloc(8);
    *(int64_t*)v56 = (int64_t)v55;
    v59 = *(void**)v53;
    v60 = *(int64_t*)v59;
    *(int64_t*)v56 = (int64_t)v60;
    v63 = *(int64_t*)v56;
    v64 = 0LL;
    v65 = v63 != v64;
    v66 = *(int64_t*)v56;
    v67 = 1LL;
    v68 = -v67;
    v69 = v66 != v68;
    v70 = v65 && v69;
    if (v70) goto bb7;
    else goto bb8;
  bb6:
    v83 = *(int64_t*)v24;
    p_call_stack_pop();
    return v83;
  bb7:
    v72 = *(int64_t*)v24;
    v73 = *(int64_t*)v56;
    v74 = 0LL;
    Vec_push((void*)v74, v72, v73);
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    v78 = *(int64_t*)v38;
    v79 = 1LL;
    v80 = v78 + v79;
    *(int64_t*)v38 = (int64_t)v80;
    goto bb4;
    return 0;
}

void StringBuilder_append_char(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("StringBuilder_append_char");
    void* v9;
    int64_t v14; /* placeholder for void */
    void* v10;
    int64_t v8; /* placeholder for void */
    void* v5;
    int64_t v11;
    int64_t v13;
    int64_t v12;
    void* v6;
    void* v4;
    void* v3;
    int64_t v15; /* placeholder for void */
    void* v7;
    v3 = (char*)v1 + 0;
    v4 = (char*)v1 + 0;
    v5 = *(void**)v4;
    v6 = p_char_to_str(v2);
    v7 = p_str_concat(v5, v6);
    *(void**)v3 = (void*)v7;
    v9 = (char*)v1 + 8;
    v10 = (char*)v1 + 8;
    v11 = *(int64_t*)v10;
    v12 = 1LL;
    v13 = v11 + v12;
    *(int64_t*)v9 = (int64_t)v13;
    p_call_stack_pop();
    return;
    return;
}

int64_t String_clone(void* v0, int64_t v1) {
    p_call_stack_push("String_clone");
    int64_t v3;
    int64_t v4; /* placeholder for void */
    int64_t v2;
    v2 = 0LL;
    v3 = (int64_t)String_from((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

bool str_is_empty(void* v0, void* v1) {
    p_call_stack_push("str_is_empty");
    int64_t v4; /* placeholder for void */
    int64_t v2;
    bool v3;
    v2 = 0LL;
    v3 = (bool)String_is_empty((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

void* sigmoid(void* v0, void* v1) {
    p_call_stack_push("sigmoid");
    void* v2;
    int64_t v3; /* placeholder for void */
    v2 = p_tensor_sigmoid(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void* HashMap_to_str(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_to_str");
    void* v77;
    int64_t v36; /* placeholder for void */
    int64_t v48;
    int64_t v57;
    void* v122;
    void* v20;
    void* v109;
    int64_t v133; /* placeholder for void */
    int64_t v104;
    int64_t v34;
    int64_t v80; /* placeholder for void */
    void* v112;
    void* v91;
    int64_t v8;
    int64_t v59;
    int64_t v72;
    int64_t v65; /* placeholder for void */
    void* v92;
    int64_t v115; /* placeholder for void */
    void* v131;
    int64_t v96;
    bool v17;
    void* v76;
    void* v100;
    int64_t v58;
    int64_t v89; /* placeholder for void */
    int64_t v97;
    void* v120;
    bool v29;
    int64_t v75; /* placeholder for void */
    int64_t v23;
    int64_t v40;
    int64_t v42;
    void* v118;
    int64_t v127;
    int64_t v16;
    int64_t v30; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    int64_t v49;
    int64_t v74; /* placeholder for void */
    int64_t v3;
    void* v12;
    int64_t v68;
    int64_t v46;
    int64_t v85; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    void* v113;
    void* v130;
    int64_t v33;
    int64_t v5; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    void* v117;
    int64_t v61;
    int64_t v25;
    int64_t v53;
    int64_t v32;
    void* v116;
    int64_t v64; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v135; /* placeholder for void */
    int64_t v93; /* placeholder for void */
    void* v4;
    int64_t v28;
    bool v84;
    int64_t v103;
    int64_t v86;
    int64_t v83;
    int64_t v73;
    void* v90;
    int64_t v87;
    void* v114;
    void* v123;
    int64_t v15;
    int64_t v54;
    int64_t v7;
    int64_t v126;
    int64_t v98;
    int64_t v70; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    int64_t v119; /* placeholder for void */
    int64_t v129; /* placeholder for void */
    void* v9;
    void* v134;
    bool v50;
    int64_t v102;
    int64_t v125;
    int64_t v11;
    int64_t v99;
    int64_t v37;
    int64_t v47;
    int64_t v31;
    int64_t v107;
    int64_t v26;
    bool v88;
    int64_t v128; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    int64_t v79;
    int64_t v6;
    int64_t v55;
    int64_t v69; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v38;
    int64_t v24;
    void* v35;
    void* v110;
    int64_t v121;
    int64_t v63; /* placeholder for void */
    int64_t v39;
    int64_t v82;
    int64_t v18; /* placeholder for void */
    int64_t v62;
    void* v132;
    int64_t v66;
    void* v105;
    int64_t v111; /* placeholder for void */
    int64_t v67;
    int64_t v19;
    int64_t v52;
    int64_t v71;
    int64_t v41;
    int64_t v2;
    int64_t v45;
    int64_t v108;
    int64_t v14; /* placeholder for void */
    int64_t v60;
    int64_t v78; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v124; /* placeholder for void */
    void* v43;
    int64_t v10; /* placeholder for void */
    int64_t v27;
    int64_t v13; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)HashMap_keys((void*)v2, v1);
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    v7 = 0LL;
    v8 = (int64_t)Vec_len((void*)v7, v6);
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 0LL;
    v12 = malloc(8);
    *(int64_t*)v12 = (int64_t)v11;
    goto bb1;
  bb1:
    v15 = *(int64_t*)v12;
    v16 = *(int64_t*)v9;
    v17 = v15 < v16;
    if (v17) goto bb2;
    else goto bb3;
  bb2:
    v19 = 0LL;
    v20 = malloc(8);
    *(int64_t*)v20 = (int64_t)v19;
    goto bb4;
  bb3:
    v76 = _str9;
    v77 = malloc(8);
    *(void**)v77 = (void*)v76;
    v79 = 0LL;
    *(int64_t*)v12 = (int64_t)v79;
    goto bb10;
  bb4:
    v23 = *(int64_t*)v20;
    v24 = 1LL;
    v25 = v23 + v24;
    v26 = *(int64_t*)v9;
    v27 = *(int64_t*)v12;
    v28 = v26 - v27;
    v29 = v25 < v28;
    if (v29) goto bb5;
    else goto bb6;
  bb5:
    v31 = *(int64_t*)v4;
    v32 = *(int64_t*)v20;
    v33 = 0LL;
    v34 = (int64_t)Vec_get((void*)v33, v31, v32);
    v35 = malloc(8);
    *(int64_t*)v35 = (int64_t)v34;
    v37 = *(int64_t*)v4;
    v38 = *(int64_t*)v20;
    v39 = 1LL;
    v40 = v38 + v39;
    v41 = 0LL;
    v42 = (int64_t)Vec_get((void*)v41, v37, v40);
    v43 = malloc(8);
    *(int64_t*)v43 = (int64_t)v42;
    v45 = *(int64_t*)v35;
    v46 = *(int64_t*)v43;
    v47 = 0LL;
    v48 = (int64_t)p_str_compare((void*)v47, v45, v46);
    v49 = 0LL;
    v50 = v48 > v49;
    if (v50) goto bb7;
    else goto bb8;
  bb6:
    v71 = *(int64_t*)v12;
    v72 = 1LL;
    v73 = v71 + v72;
    *(int64_t*)v12 = (int64_t)v73;
    goto bb1;
  bb7:
    v52 = *(int64_t*)v4;
    v53 = *(int64_t*)v20;
    v54 = *(int64_t*)v43;
    v55 = 0LL;
    Vec_set((void*)v55, v52, v53, v54);
    v57 = *(int64_t*)v4;
    v58 = *(int64_t*)v20;
    v59 = 1LL;
    v60 = v58 + v59;
    v61 = *(int64_t*)v35;
    v62 = 0LL;
    Vec_set((void*)v62, v57, v60, v61);
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    v66 = *(int64_t*)v20;
    v67 = 1LL;
    v68 = v66 + v67;
    *(int64_t*)v20 = (int64_t)v68;
    goto bb4;
  bb10:
    v82 = *(int64_t*)v12;
    v83 = *(int64_t*)v9;
    v84 = v82 < v83;
    if (v84) goto bb11;
    else goto bb12;
  bb11:
    v86 = *(int64_t*)v12;
    v87 = 0LL;
    v88 = v86 > v87;
    if (v88) goto bb13;
    else goto bb14;
  bb12:
    v130 = *(void**)v77;
    v131 = _str10;
    v132 = p_str_concat(v130, v131);
    *(void**)v77 = (void*)v132;
    v134 = *(void**)v77;
    p_call_stack_pop();
    return v134;
  bb13:
    v90 = *(void**)v77;
    v91 = _str3;
    v92 = p_str_concat(v90, v91);
    *(void**)v77 = (void*)v92;
    goto bb15;
  bb14:
    goto bb15;
  bb15:
    v96 = *(int64_t*)v4;
    v97 = *(int64_t*)v12;
    v98 = 0LL;
    v99 = (int64_t)Vec_get((void*)v98, v96, v97);
    v100 = malloc(8);
    *(int64_t*)v100 = (int64_t)v99;
    v102 = *(int64_t*)v100;
    v103 = 0LL;
    v104 = (int64_t)HashMap_get((void*)v103, v1, v102);
    v105 = malloc(8);
    *(int64_t*)v105 = (int64_t)v104;
    v107 = *(int64_t*)v100;
    v108 = 0LL;
    v109 = p_str_from_ptr((void*)v108, v107);
    v110 = malloc(8);
    *(void**)v110 = (void*)v109;
    v112 = *(void**)v77;
    v113 = *(void**)v110;
    v114 = p_str_concat(v112, v113);
    *(void**)v77 = (void*)v114;
    v116 = *(void**)v77;
    v117 = _str11;
    v118 = p_str_concat(v116, v117);
    *(void**)v77 = (void*)v118;
    v120 = *(void**)v77;
    v121 = *(int64_t*)v105;
    v122 = (void*)p_to_str((int64_t)v121);
    v123 = p_str_concat(v120, v122);
    *(void**)v77 = (void*)v123;
    v125 = *(int64_t*)v12;
    v126 = 1LL;
    v127 = v125 + v126;
    *(int64_t*)v12 = (int64_t)v127;
    goto bb10;
    return 0;
}

void* filled(void* v0, void* v1, double v2) {
    p_call_stack_push("filled");
    int64_t v4; /* placeholder for void */
    void* v3;
    v3 = p_tensor_filled(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

void* int_to_str(void* v0, int64_t v1) {
    p_call_stack_push("int_to_str");
    int64_t v3; /* placeholder for void */
    void* v2;
    v2 = (void*)p_to_str((int64_t)v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void* sub(void* v0, void* v1, void* v2) {
    p_call_stack_push("sub");
    void* v3;
    int64_t v4; /* placeholder for void */
    v3 = p_tensor_sub(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

bool Vec_is_empty(void* v0, int64_t v1) {
    p_call_stack_push("Vec_is_empty");
    int64_t v2;
    void* v6;
    int64_t v12;
    void* v4;
    int64_t v9;
    void* v8;
    int64_t v15; /* placeholder for void */
    int64_t v3;
    int64_t v10; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v13;
    int64_t v7; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    bool v14;
    v2 = 8LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = malloc(8);
    v8 = *(void**)v4;
    v9 = *(int64_t*)v8;
    *(int64_t*)v6 = (int64_t)v9;
    v12 = *(int64_t*)v6;
    v13 = 0LL;
    v14 = v12 == v13;
    p_call_stack_pop();
    return v14;
    return 0;
}

void Option_map(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Option_map");
    void* v9;
    int64_t v38;
    void* v27;
    int64_t v26; /* placeholder for void */
    int64_t v29;
    void* v41;
    int64_t v12;
    int64_t v8; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    void* v19;
    int64_t v45; /* placeholder for void */
    int64_t v15;
    void* v23;
    int64_t v10;
    int64_t v43; /* placeholder for void */
    void* v34;
    void* v16;
    void* v32;
    int64_t v5;
    void* v37;
    void* v13;
    int64_t v17; /* placeholder for void */
    void* v22;
    int64_t v31; /* placeholder for void */
    int64_t v28;
    int64_t v33;
    void* v36;
    int64_t v42;
    int64_t v14; /* placeholder for void */
    void* v18;
    void* v40;
    int64_t v46; /* placeholder for void */
    int64_t v20;
    bool v30;
    int64_t v21; /* placeholder for void */
    int64_t v6;
    void* v4;
    void* v11;
    int64_t v25; /* placeholder for void */
    bool v7;
    int64_t v44; /* placeholder for void */
    int64_t v24;
    v4 = (char*)v2 + 0;
    v5 = *(int64_t*)v4;
    v6 = 0LL;
    v7 = v5 == v6;
    if (v7) goto bb2;
    else goto bb3;
  bb1:
    p_call_stack_pop();
    return;
  bb2:
    v9 = (char*)v2 + 8;
    v10 = *(int64_t*)v9;
    v11 = malloc(16);
    v12 = 0LL;
    v13 = (char*)v11 + 0;
    *(int64_t*)v13 = (int64_t)v12;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ndim") == 0) {
            v15 = ndim((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "size") == 0) {
            v15 = size((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (int64_t)v10);
        } else { abort(); }
    }
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
    p_call_stack_pop();
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
    p_call_stack_pop();
    return;
  bb5:
    goto bb1;
    return;
}

int64_t HashMap_new(void* v0) {
    p_call_stack_push("HashMap_new");
    int64_t v20;
    void* v21;
    int64_t v5;
    void* v33;
    void* v3;
    int64_t v9;
    int64_t v18;
    int64_t v28;
    int64_t v12; /* placeholder for void */
    int64_t v8;
    int64_t v14;
    void* v24;
    int64_t v15;
    int64_t v34;
    int64_t v36; /* placeholder for void */
    void* v30;
    int64_t v10;
    int64_t v29; /* placeholder for void */
    int64_t v37;
    int64_t v13;
    int64_t v1;
    int64_t v7; /* placeholder for void */
    int64_t v31;
    int64_t v4; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    void* v2;
    int64_t v17; /* placeholder for void */
    void* v27;
    void* v11;
    void* v16;
    int64_t v26; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v19;
    void* v6;
    int64_t v25;
    int64_t v38; /* placeholder for void */
    v1 = 32LL;
    v2 = p_malloc(v1);
    v3 = malloc(8);
    *(int64_t*)v3 = (int64_t)v2;
    v5 = *(int64_t*)v3;
    v6 = malloc(8);
    *(void**)v6 = (void*)v5;
    v8 = *(int64_t*)v3;
    v9 = 8LL;
    v10 = v8 + v9;
    v11 = malloc(8);
    *(void**)v11 = (void*)v10;
    v13 = *(int64_t*)v3;
    v14 = 16LL;
    v15 = v13 + v14;
    v16 = malloc(8);
    *(void**)v16 = (void*)v15;
    v18 = *(int64_t*)v3;
    v19 = 24LL;
    v20 = v18 + v19;
    v21 = malloc(8);
    *(void**)v21 = (void*)v20;
    v24 = *(void**)v6;
    v25 = 0LL;
    *(int64_t*)v24 = (int64_t)v25;
    v27 = *(void**)v11;
    v28 = 0LL;
    *(int64_t*)v27 = (int64_t)v28;
    v30 = *(void**)v16;
    v31 = 0LL;
    *(int64_t*)v30 = (int64_t)v31;
    v33 = *(void**)v21;
    v34 = 0LL;
    *(int64_t*)v33 = (int64_t)v34;
    v37 = *(int64_t*)v3;
    p_call_stack_pop();
    return v37;
    return 0;
}

int64_t HashMap_find_slot(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_find_slot");
    int64_t v45;
    int64_t v23; /* placeholder for void */
    int64_t v128;
    void* v58;
    int64_t v96; /* placeholder for void */
    int64_t v17;
    int64_t v11; /* placeholder for void */
    int64_t v122; /* placeholder for void */
    int64_t v3;
    int64_t v36;
    int64_t v54;
    int64_t v71;
    int64_t v76;
    int64_t v79;
    int64_t v110; /* placeholder for void */
    bool v113;
    void* v7;
    int64_t v87;
    int64_t v114; /* placeholder for void */
    int64_t v102; /* placeholder for void */
    void* v28;
    int64_t v90;
    int64_t v32;
    int64_t v29;
    int64_t v68;
    bool v92;
    int64_t v91;
    int64_t v131;
    int64_t v26; /* placeholder for void */
    int64_t v9;
    void* v25;
    int64_t v120;
    void* v61;
    void* v13;
    int64_t v18;
    int64_t v60;
    int64_t v38;
    int64_t v12; /* placeholder for void */
    bool v19;
    int64_t v22; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v65; /* placeholder for void */
    int64_t v109;
    int64_t v55;
    bool v73;
    int64_t v50;
    int64_t v75;
    int64_t v93; /* placeholder for void */
    int64_t v129; /* placeholder for void */
    int64_t v42;
    int64_t v4;
    int64_t v99;
    int64_t v112;
    int64_t v21;
    int64_t v47; /* placeholder for void */
    int64_t v119;
    bool v77;
    int64_t v33;
    void* v43;
    int64_t v127; /* placeholder for void */
    int64_t v63;
    int64_t v48; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    int64_t v117; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v56;
    int64_t v14;
    int64_t v111;
    int64_t v107;
    bool v101;
    int64_t v81; /* placeholder for void */
    int64_t v132; /* placeholder for void */
    int64_t v24;
    int64_t v72;
    int64_t v89; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    int64_t v57;
    int64_t v83; /* placeholder for void */
    int64_t v130; /* placeholder for void */
    int64_t v82;
    int64_t v41;
    int64_t v124;
    int64_t v49;
    int64_t v15; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    int64_t v85;
    int64_t v115;
    void* v10;
    void* v67;
    int64_t v118; /* placeholder for void */
    int64_t v53;
    bool v88;
    int64_t v94;
    int64_t v6; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    int64_t v108;
    int64_t v52; /* placeholder for void */
    int64_t v116; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v103;
    int64_t v20; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    void* v5;
    int64_t v105; /* placeholder for void */
    bool v51;
    int64_t v80; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v125;
    int64_t v70; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    int64_t v37;
    int64_t v44; /* placeholder for void */
    int64_t v86;
    int64_t v31; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    void* v46;
    int64_t v78; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v39;
    int64_t v100;
    int64_t v121;
    void* v34;
    int64_t v123; /* placeholder for void */
    void* v64;
    bool v126;
    v3 = 24LL;
    v4 = v1 + v3;
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = malloc(8);
    *(void**)v7 = (void*)v1;
    v9 = 0LL;
    v10 = malloc(8);
    *(int64_t*)v10 = (int64_t)v9;
    v13 = *(void**)v5;
    v14 = *(int64_t*)v13;
    *(int64_t*)v10 = (int64_t)v14;
    v17 = *(int64_t*)v10;
    v18 = 0LL;
    v19 = v17 == v18;
    if (v19) goto bb1;
    else goto bb2;
  bb1:
    v21 = 0LL;
    p_call_stack_pop();
    return v21;
  bb2:
    goto bb3;
  bb3:
    v24 = 0LL;
    v25 = malloc(8);
    *(int64_t*)v25 = (int64_t)v24;
    v28 = *(void**)v7;
    v29 = *(int64_t*)v28;
    *(int64_t*)v25 = (int64_t)v29;
    v32 = 0LL;
    v33 = (int64_t)p_str_hash((void*)v32, v2);
    v34 = malloc(8);
    *(int64_t*)v34 = (int64_t)v33;
    v36 = *(int64_t*)v34;
    v37 = *(int64_t*)v10;
    v38 = v36 % v37;
    v39 = malloc(8);
    *(int64_t*)v39 = (int64_t)v38;
    v41 = 1LL;
    v42 = -v41;
    v43 = malloc(8);
    *(int64_t*)v43 = (int64_t)v42;
    v45 = 0LL;
    v46 = malloc(8);
    *(int64_t*)v46 = (int64_t)v45;
    goto bb4;
  bb4:
    v49 = *(int64_t*)v46;
    v50 = *(int64_t*)v10;
    v51 = v49 < v50;
    if (v51) goto bb5;
    else goto bb6;
  bb5:
    v53 = *(int64_t*)v25;
    v54 = *(int64_t*)v39;
    v55 = 8LL;
    v56 = v54 * v55;
    v57 = v53 + v56;
    v58 = malloc(8);
    *(int64_t*)v58 = (int64_t)v57;
    v60 = *(int64_t*)v58;
    v61 = malloc(8);
    *(void**)v61 = (void*)v60;
    v63 = 0LL;
    v64 = malloc(8);
    *(int64_t*)v64 = (int64_t)v63;
    v67 = *(void**)v61;
    v68 = *(int64_t*)v67;
    *(int64_t*)v64 = (int64_t)v68;
    v71 = *(int64_t*)v64;
    v72 = 0LL;
    v73 = v71 == v72;
    if (v73) goto bb7;
    else goto bb8;
  bb6:
    v124 = *(int64_t*)v43;
    v125 = 0LL;
    v126 = v124 >= v125;
    if (v126) goto bb25;
    else goto bb26;
  bb7:
    v75 = *(int64_t*)v43;
    v76 = 0LL;
    v77 = v75 >= v76;
    if (v77) goto bb10;
    else goto bb11;
  bb8:
    goto bb9;
  bb9:
    v85 = *(int64_t*)v64;
    v86 = 1LL;
    v87 = -v86;
    v88 = v85 == v87;
    if (v88) goto bb13;
    else goto bb14;
  bb10:
    v79 = *(int64_t*)v43;
    p_call_stack_pop();
    return v79;
  bb11:
    goto bb12;
  bb12:
    v82 = *(int64_t*)v39;
    p_call_stack_pop();
    return v82;
  bb13:
    v90 = *(int64_t*)v43;
    v91 = 0LL;
    v92 = v90 < v91;
    if (v92) goto bb16;
    else goto bb17;
  bb14:
    v99 = *(int64_t*)v64;
    v100 = 0LL;
    v101 = (bool)p_str_equal((void*)v100, v99, v2);
    if (v101) goto bb19;
    else goto bb20;
  bb15:
    v107 = *(int64_t*)v39;
    v108 = 1LL;
    v109 = v107 + v108;
    *(int64_t*)v39 = (int64_t)v109;
    v111 = *(int64_t*)v39;
    v112 = *(int64_t*)v10;
    v113 = v111 >= v112;
    if (v113) goto bb22;
    else goto bb23;
  bb16:
    v94 = *(int64_t*)v39;
    *(int64_t*)v43 = (int64_t)v94;
    goto bb18;
  bb17:
    goto bb18;
  bb18:
    goto bb15;
  bb19:
    v103 = *(int64_t*)v39;
    p_call_stack_pop();
    return v103;
  bb20:
    goto bb21;
  bb21:
    goto bb15;
  bb22:
    v115 = 0LL;
    *(int64_t*)v39 = (int64_t)v115;
    goto bb24;
  bb23:
    goto bb24;
  bb24:
    v119 = *(int64_t*)v46;
    v120 = 1LL;
    v121 = v119 + v120;
    *(int64_t*)v46 = (int64_t)v121;
    goto bb4;
  bb25:
    v128 = *(int64_t*)v43;
    p_call_stack_pop();
    return v128;
  bb26:
    goto bb27;
  bb27:
    v131 = 0LL;
    p_call_stack_pop();
    return v131;
    return 0;
}

void* scale(void* v0, void* v1, double v2) {
    p_call_stack_push("scale");
    void* v3;
    int64_t v4; /* placeholder for void */
    v3 = p_tensor_scale(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

void Result_ok(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_ok");
    void* v40;
    void* v17;
    void* v10;
    int64_t v44; /* placeholder for void */
    int64_t v22;
    bool v28;
    void* v34;
    void* v8;
    int64_t v35; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v5;
    int64_t v27;
    void* v20;
    void* v32;
    int64_t v42;
    int64_t v46; /* placeholder for void */
    int64_t v26;
    int64_t v13; /* placeholder for void */
    int64_t v4;
    int64_t v19; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v9;
    int64_t v23; /* placeholder for void */
    int64_t v33;
    void* v36;
    void* v37;
    void* v14;
    void* v41;
    int64_t v29; /* placeholder for void */
    void* v16;
    int64_t v24; /* placeholder for void */
    void* v30;
    int64_t v31;
    void* v21;
    int64_t v38;
    bool v6;
    void* v25;
    void* v12;
    int64_t v43; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v18;
    void* v3;
    int64_t v11;
    v3 = (char*)v2 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    p_call_stack_pop();
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
    p_call_stack_pop();
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
    p_call_stack_pop();
    return;
  bb5:
    goto bb1;
    return;
}

void Vec_clear(void* v0, int64_t v1) {
    p_call_stack_push("Vec_clear");
    int64_t v11; /* placeholder for void */
    int64_t v2;
    void* v4;
    int64_t v6; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v3;
    int64_t v8;
    void* v7;
    int64_t v5; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    v2 = 8LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v7 = *(void**)v4;
    v8 = 0LL;
    *(int64_t*)v7 = (int64_t)v8;
    p_call_stack_pop();
    return;
    return;
}

int64_t p_str_length(void* v0, int64_t v1) {
    p_call_stack_push("p_str_length");
    int64_t v5;
    void* v11;
    int64_t v10;
    int64_t v4; /* placeholder for void */
    int64_t v28;
    int64_t v39; /* placeholder for void */
    int64_t v22;
    int64_t v23;
    int64_t v6; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v32;
    void* v19;
    uint8_t v18;
    int64_t v27;
    int64_t v40; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v2;
    int64_t v15; /* placeholder for void */
    int64_t v13;
    int64_t v31;
    int64_t v34;
    int64_t v9; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v30;
    void* v36;
    int64_t v38; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    bool v24;
    int64_t v41;
    int64_t v42; /* placeholder for void */
    bool v3;
    uint8_t v37;
    int64_t v26;
    int64_t v25; /* placeholder for void */
    void* v17;
    void* v14;
    void* v8;
    int64_t v29; /* placeholder for void */
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 0LL;
    p_call_stack_pop();
    return v5;
  bb2:
    goto bb3;
  bb3:
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v1;
    v10 = *(int64_t*)v8;
    v11 = malloc(8);
    *(void**)v11 = (void*)v10;
    v13 = 0LL;
    v14 = malloc(8);
    *(int64_t*)v14 = (int64_t)v13;
    v17 = *(void**)v11;
    v18 = *(uint8_t*)v17;
    v19 = malloc(8);
    *(int64_t*)v19 = (int64_t)v18;
    goto bb4;
  bb4:
    v22 = *(int64_t*)v19;
    v23 = 0LL;
    v24 = v22 != v23;
    if (v24) goto bb5;
    else goto bb6;
  bb5:
    v26 = *(int64_t*)v14;
    v27 = 1LL;
    v28 = v26 + v27;
    *(int64_t*)v14 = (int64_t)v28;
    v30 = *(int64_t*)v8;
    v31 = 1LL;
    v32 = v30 + v31;
    *(int64_t*)v8 = (int64_t)v32;
    v34 = *(int64_t*)v8;
    *(void**)v11 = (void*)v34;
    v36 = *(void**)v11;
    v37 = *(uint8_t*)v36;
    *(int64_t*)v19 = (int64_t)v37;
    goto bb4;
  bb6:
    v41 = *(int64_t*)v14;
    p_call_stack_pop();
    return v41;
    return 0;
}

void StringBuilder_append_str(void* v0, void* v1, void* v2) {
    p_call_stack_push("StringBuilder_append_str");
    void* v8;
    void* v6;
    void* v3;
    int64_t v7; /* placeholder for void */
    int64_t v10;
    void* v4;
    void* v5;
    int64_t v11;
    int64_t v14; /* placeholder for void */
    int64_t v12;
    int64_t v13;
    int64_t v15; /* placeholder for void */
    void* v9;
    v3 = (char*)v1 + 0;
    v4 = (char*)v1 + 0;
    v5 = *(void**)v4;
    v6 = p_str_concat(v5, v2);
    *(void**)v3 = (void*)v6;
    v8 = (char*)v1 + 8;
    v9 = (char*)v1 + 8;
    v10 = *(int64_t*)v9;
    v11 = 0LL;
    v12 = (int64_t)String_len((void*)v11, (int64_t)v2);
    v13 = v10 + v12;
    *(int64_t*)v8 = (int64_t)v13;
    p_call_stack_pop();
    return;
    return;
}

bool Option_is_some(void* v0, void* v1) {
    p_call_stack_push("Option_is_some");
    int64_t v10; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    void* v2;
    bool v9;
    void* v11;
    int64_t v13;
    bool v5;
    bool v14;
    int64_t v12;
    bool v16;
    int64_t v3;
    int64_t v18; /* placeholder for void */
    bool v19;
    int64_t v4;
    int64_t v15; /* placeholder for void */
    int64_t v8;
    void* v7;
    int64_t v20; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v19 = false;
    p_call_stack_pop();
    return v19;
  bb2:
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    v9 = true;
    p_call_stack_pop();
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
    p_call_stack_pop();
    return v16;
  bb5:
    goto bb1;
    return 0;
}

void* str_slice(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("str_slice");
    int64_t v24;
    bool v30;
    int64_t v11; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v5;
    int64_t v20;
    int64_t v17; /* placeholder for void */
    int64_t v4;
    int64_t v19; /* placeholder for void */
    int64_t v36;
    int64_t v7; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v16;
    void* v32;
    void* v6;
    int64_t v38;
    int64_t v12;
    int64_t v26; /* placeholder for void */
    int64_t v28;
    int64_t v13;
    void* v8;
    int64_t v37;
    int64_t v23; /* placeholder for void */
    int64_t v21;
    bool v14;
    bool v22;
    int64_t v33; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    void* v10;
    int64_t v35;
    int64_t v27; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v29;
    v4 = 0LL;
    v5 = (int64_t)String_len((void*)v4, (int64_t)v1);
    v6 = malloc(8);
    *(int64_t*)v6 = (int64_t)v5;
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v2;
    v10 = malloc(8);
    *(int64_t*)v10 = (int64_t)v3;
    v12 = *(int64_t*)v8;
    v13 = 0LL;
    v14 = v12 < v13;
    if (v14) goto bb1;
    else goto bb2;
  bb1:
    v16 = 0LL;
    *(int64_t*)v8 = (int64_t)v16;
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v20 = *(int64_t*)v10;
    v21 = *(int64_t*)v6;
    v22 = v20 > v21;
    if (v22) goto bb4;
    else goto bb5;
  bb4:
    v24 = *(int64_t*)v6;
    *(int64_t*)v10 = (int64_t)v24;
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v28 = *(int64_t*)v8;
    v29 = *(int64_t*)v10;
    v30 = v28 >= v29;
    if (v30) goto bb7;
    else goto bb8;
  bb7:
    v32 = _str8;
    p_call_stack_pop();
    return v32;
  bb8:
    goto bb9;
  bb9:
    v35 = *(int64_t*)v8;
    v36 = *(int64_t*)v10;
    v37 = 0LL;
    v38 = (int64_t)String_slice((void*)v37, (int64_t)v1, v35, v36);
    p_call_stack_pop();
    return (void*)v38;
    return 0;
}

void HashMap_clear(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_clear");
    void* v27;
    void* v11;
    void* v18;
    void* v21;
    int64_t v56; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v20;
    int64_t v35;
    int64_t v31;
    bool v33;
    int64_t v39;
    int64_t v2;
    int64_t v22; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    int64_t v14;
    int64_t v17;
    int64_t v63;
    int64_t v5; /* placeholder for void */
    int64_t v25;
    int64_t v43;
    void* v58;
    int64_t v10; /* placeholder for void */
    void* v55;
    bool v37;
    void* v7;
    int64_t v51;
    void* v4;
    int64_t v48;
    int64_t v3;
    int64_t v49;
    int64_t v60; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    int64_t v13;
    int64_t v23; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    bool v36;
    int64_t v28;
    int64_t v34;
    int64_t v44;
    int64_t v61; /* placeholder for void */
    int64_t v62;
    int64_t v59;
    int64_t v53; /* placeholder for void */
    bool v45;
    int64_t v54;
    void* v24;
    int64_t v32;
    int64_t v30; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    void* v52;
    int64_t v50;
    int64_t v41; /* placeholder for void */
    int64_t v8;
    void* v40;
    int64_t v47;
    int64_t v65; /* placeholder for void */
    void* v15;
    int64_t v38; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v64;
    int64_t v68; /* placeholder for void */
    v2 = 16LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v7 = *(void**)v4;
    v8 = 0LL;
    *(int64_t*)v7 = (int64_t)v8;
    v11 = malloc(8);
    *(void**)v11 = (void*)v1;
    v13 = 24LL;
    v14 = v1 + v13;
    v15 = malloc(8);
    *(void**)v15 = (void*)v14;
    v17 = 0LL;
    v18 = malloc(8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = 0LL;
    v21 = malloc(8);
    *(int64_t*)v21 = (int64_t)v20;
    v24 = *(void**)v11;
    v25 = *(int64_t*)v24;
    *(int64_t*)v18 = (int64_t)v25;
    v27 = *(void**)v15;
    v28 = *(int64_t*)v27;
    *(int64_t*)v21 = (int64_t)v28;
    v31 = *(int64_t*)v18;
    v32 = 0LL;
    v33 = v31 != v32;
    v34 = *(int64_t*)v21;
    v35 = 0LL;
    v36 = v34 > v35;
    v37 = v33 && v36;
    if (v37) goto bb1;
    else goto bb2;
  bb1:
    v39 = 0LL;
    v40 = malloc(8);
    *(int64_t*)v40 = (int64_t)v39;
    goto bb4;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return;
  bb4:
    v43 = *(int64_t*)v40;
    v44 = *(int64_t*)v21;
    v45 = v43 < v44;
    if (v45) goto bb5;
    else goto bb6;
  bb5:
    v47 = *(int64_t*)v18;
    v48 = *(int64_t*)v40;
    v49 = 8LL;
    v50 = v48 * v49;
    v51 = v47 + v50;
    v52 = malloc(8);
    *(int64_t*)v52 = (int64_t)v51;
    v54 = *(int64_t*)v52;
    v55 = malloc(8);
    *(void**)v55 = (void*)v54;
    v58 = *(void**)v55;
    v59 = 0LL;
    *(int64_t*)v58 = (int64_t)v59;
    v62 = *(int64_t*)v40;
    v63 = 1LL;
    v64 = v62 + v63;
    *(int64_t*)v40 = (int64_t)v64;
    goto bb4;
  bb6:
    goto bb3;
    return;
}

void* from_vec(void* v0, void* v1, void* v2) {
    p_call_stack_push("from_vec");
    void* v3;
    int64_t v4; /* placeholder for void */
    v3 = p_tensor_from_vec(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

bool String_starts_with(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_starts_with");
    int64_t v76; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    bool v4;
    int64_t v26; /* placeholder for void */
    int64_t v73; /* placeholder for void */
    int64_t v17;
    int64_t v46;
    int64_t v16;
    int64_t v27;
    int64_t v36;
    int64_t v42; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v5;
    int64_t v68; /* placeholder for void */
    int64_t v70;
    int64_t v13;
    int64_t v12;
    int64_t v31;
    void* v44;
    int64_t v61; /* placeholder for void */
    void* v18;
    void* v56;
    int64_t v32;
    int64_t v30; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    bool v67;
    uint8_t v57;
    int64_t v71;
    int64_t v11; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v35;
    int64_t v40;
    int64_t v58; /* placeholder for void */
    bool v7;
    int64_t v25; /* placeholder for void */
    int64_t v72;
    bool v22;
    int64_t v39;
    int64_t v38; /* placeholder for void */
    void* v53;
    bool v9;
    int64_t v63;
    int64_t v8; /* placeholder for void */
    void* v37;
    int64_t v20;
    int64_t v21;
    int64_t v45; /* placeholder for void */
    bool v75;
    int64_t v43;
    int64_t v54; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    bool v6;
    void* v47;
    uint8_t v60;
    bool v24;
    int64_t v29; /* placeholder for void */
    void* v50;
    void* v41;
    int64_t v48; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    bool v65;
    int64_t v64;
    void* v59;
    void* v28;
    int64_t v15; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v3;
    bool v33;
    int64_t v49;
    int64_t v10; /* placeholder for void */
    void* v14;
    int64_t v74; /* placeholder for void */
    int64_t v52;
    v3 = 0LL;
    v4 = v1 == v3;
    v5 = 0LL;
    v6 = v2 == v5;
    v7 = v4 || v6;
    if (v7) goto bb1;
    else goto bb2;
  bb1:
    v9 = false;
    p_call_stack_pop();
    return v9;
  bb2:
    goto bb3;
  bb3:
    v12 = 0LL;
    v13 = (int64_t)p_str_length((void*)v12, v1);
    v14 = malloc(8);
    *(int64_t*)v14 = (int64_t)v13;
    v16 = 0LL;
    v17 = (int64_t)p_str_length((void*)v16, v2);
    v18 = malloc(8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = *(int64_t*)v18;
    v21 = *(int64_t*)v14;
    v22 = v20 > v21;
    if (v22) goto bb4;
    else goto bb5;
  bb4:
    v24 = false;
    p_call_stack_pop();
    return v24;
  bb5:
    goto bb6;
  bb6:
    v27 = 0LL;
    v28 = malloc(8);
    *(int64_t*)v28 = (int64_t)v27;
    goto bb7;
  bb7:
    v31 = *(int64_t*)v28;
    v32 = *(int64_t*)v18;
    v33 = v31 < v32;
    if (v33) goto bb8;
    else goto bb9;
  bb8:
    v35 = *(int64_t*)v28;
    v36 = v1 + v35;
    v37 = malloc(8);
    *(int64_t*)v37 = (int64_t)v36;
    v39 = *(int64_t*)v28;
    v40 = v2 + v39;
    v41 = malloc(8);
    *(int64_t*)v41 = (int64_t)v40;
    v43 = *(int64_t*)v37;
    v44 = malloc(8);
    *(void**)v44 = (void*)v43;
    v46 = *(int64_t*)v41;
    v47 = malloc(8);
    *(void**)v47 = (void*)v46;
    v49 = 0LL;
    v50 = malloc(8);
    *(int64_t*)v50 = (int64_t)v49;
    v52 = 0LL;
    v53 = malloc(8);
    *(int64_t*)v53 = (int64_t)v52;
    v56 = *(void**)v44;
    v57 = *(uint8_t*)v56;
    *(int64_t*)v50 = (int64_t)v57;
    v59 = *(void**)v47;
    v60 = *(uint8_t*)v59;
    *(int64_t*)v53 = (int64_t)v60;
    v63 = *(int64_t*)v50;
    v64 = *(int64_t*)v53;
    v65 = v63 != v64;
    if (v65) goto bb10;
    else goto bb11;
  bb9:
    v75 = true;
    p_call_stack_pop();
    return v75;
  bb10:
    v67 = false;
    p_call_stack_pop();
    return v67;
  bb11:
    goto bb12;
  bb12:
    v70 = *(int64_t*)v28;
    v71 = 1LL;
    v72 = v70 + v71;
    *(int64_t*)v28 = (int64_t)v72;
    goto bb7;
    return 0;
}

int64_t Vec_last(void* v0, int64_t v1) {
    p_call_stack_push("Vec_last");
    int64_t v20; /* placeholder for void */
    void* v8;
    void* v24;
    void* v38;
    int64_t v9; /* placeholder for void */
    int64_t v14;
    int64_t v15;
    bool v16;
    int64_t v28;
    int64_t v32;
    int64_t v31;
    int64_t v12; /* placeholder for void */
    int64_t v29;
    int64_t v11;
    int64_t v37;
    int64_t v5; /* placeholder for void */
    void* v41;
    int64_t v42; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v22;
    int64_t v36; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v33;
    int64_t v49; /* placeholder for void */
    int64_t v30;
    int64_t v40;
    int64_t v17; /* placeholder for void */
    void* v35;
    int64_t v47; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v2;
    int64_t v48;
    void* v10;
    int64_t v3;
    void* v44;
    int64_t v25;
    void* v6;
    int64_t v34;
    int64_t v45;
    int64_t v13; /* placeholder for void */
    void* v4;
    void* v18;
    int64_t v39; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    v2 = 8LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = malloc(8);
    *(void**)v6 = (void*)v1;
    v8 = malloc(8);
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v8 = (int64_t)v11;
    v14 = *(int64_t*)v8;
    v15 = 0LL;
    v16 = v14 == v15;
    if (v16) goto bb1;
    else goto bb2;
  bb1:
    v18 = _str12;
    p_panic(v18);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v22 = malloc(8);
    v24 = *(void**)v6;
    v25 = *(int64_t*)v24;
    *(int64_t*)v22 = (int64_t)v25;
    v28 = *(int64_t*)v22;
    v29 = *(int64_t*)v8;
    v30 = 1LL;
    v31 = v29 - v30;
    v32 = 8LL;
    v33 = v31 * v32;
    v34 = v28 + v33;
    v35 = malloc(8);
    *(int64_t*)v35 = (int64_t)v34;
    v37 = *(int64_t*)v35;
    v38 = malloc(8);
    *(void**)v38 = (void*)v37;
    v40 = 0LL;
    v41 = malloc(8);
    *(int64_t*)v41 = (int64_t)v40;
    v44 = *(void**)v38;
    v45 = *(int64_t*)v44;
    *(int64_t*)v41 = (int64_t)v45;
    v48 = *(int64_t*)v41;
    p_call_stack_pop();
    return v48;
    return 0;
}

void* relu(void* v0, void* v1) {
    p_call_stack_push("relu");
    int64_t v3; /* placeholder for void */
    void* v2;
    v2 = p_tensor_relu(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void Vec_sort(void* v0, int64_t v1) {
    p_call_stack_push("Vec_sort");
    int64_t v59; /* placeholder for void */
    void* v4;
    void* v64;
    void* v58;
    int64_t v41;
    int64_t v70; /* placeholder for void */
    int64_t v72;
    void* v73;
    bool v85;
    bool v77;
    void* v14;
    int64_t v2;
    void* v6;
    int64_t v24; /* placeholder for void */
    bool v32;
    int64_t v45;
    int64_t v52;
    int64_t v29; /* placeholder for void */
    int64_t v47;
    int64_t v38;
    int64_t v89; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    void* v10;
    int64_t v31; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v20;
    int64_t v68;
    int64_t v13; /* placeholder for void */
    bool v30;
    int64_t v76;
    int64_t v90;
    int64_t v91;
    int64_t v56;
    int64_t v19; /* placeholder for void */
    int64_t v93; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v88; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v67;
    int64_t v5; /* placeholder for void */
    int64_t v39;
    bool v42;
    void* v69;
    int64_t v75;
    int64_t v23; /* placeholder for void */
    int64_t v11;
    int64_t v40;
    int64_t v48;
    int64_t v60;
    bool v22;
    int64_t v53;
    int64_t v66; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    void* v71;
    void* v79;
    int64_t v86; /* placeholder for void */
    int64_t v46;
    void* v35;
    int64_t v18; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v63;
    int64_t v80;
    int64_t v62; /* placeholder for void */
    int64_t v54;
    int64_t v81; /* placeholder for void */
    int64_t v92;
    int64_t v96; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v21;
    void* v27;
    int64_t v65; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v17;
    void* v82;
    bool v26;
    void* v8;
    int64_t v57;
    int64_t v87; /* placeholder for void */
    int64_t v3;
    int64_t v94; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v34;
    int64_t v44;
    int64_t v83;
    int64_t v55;
    void* v49;
    int64_t v43; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    void* v16;
    int64_t v28; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    void* v61;
    int64_t v95; /* placeholder for void */
    int64_t v51;
    v2 = 8LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = malloc(8);
    *(void**)v6 = (void*)v1;
    v8 = malloc(8);
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v8 = (int64_t)v11;
    v14 = malloc(8);
    v16 = *(void**)v6;
    v17 = *(int64_t*)v16;
    *(int64_t*)v14 = (int64_t)v17;
    v20 = *(int64_t*)v8;
    v21 = 1LL;
    v22 = v20 <= v21;
    if (v22) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return;
  bb2:
    goto bb3;
  bb3:
    v26 = true;
    v27 = malloc(1);
    *(bool*)v27 = v26;
    goto bb4;
  bb4:
    v30 = *(bool*)v27;
    if (v30) goto bb5;
    else goto bb6;
  bb5:
    v32 = false;
    *(bool*)v27 = v32;
    v34 = 0LL;
    v35 = malloc(8);
    *(int64_t*)v35 = (int64_t)v34;
    goto bb7;
  bb6:
    p_call_stack_pop();
    return;
  bb7:
    v38 = *(int64_t*)v35;
    v39 = *(int64_t*)v8;
    v40 = 1LL;
    v41 = v39 - v40;
    v42 = v38 < v41;
    if (v42) goto bb8;
    else goto bb9;
  bb8:
    v44 = *(int64_t*)v14;
    v45 = *(int64_t*)v35;
    v46 = 8LL;
    v47 = v45 * v46;
    v48 = v44 + v47;
    v49 = malloc(8);
    *(int64_t*)v49 = (int64_t)v48;
    v51 = *(int64_t*)v14;
    v52 = *(int64_t*)v35;
    v53 = 1LL;
    v54 = v52 + v53;
    v55 = 8LL;
    v56 = v54 * v55;
    v57 = v51 + v56;
    v58 = malloc(8);
    *(int64_t*)v58 = (int64_t)v57;
    v60 = *(int64_t*)v49;
    v61 = malloc(8);
    *(void**)v61 = (void*)v60;
    v63 = *(int64_t*)v58;
    v64 = malloc(8);
    *(void**)v64 = (void*)v63;
    v67 = *(void**)v61;
    v68 = *(int64_t*)v67;
    v69 = malloc(8);
    *(int64_t*)v69 = (int64_t)v68;
    v71 = *(void**)v64;
    v72 = *(int64_t*)v71;
    v73 = malloc(8);
    *(int64_t*)v73 = (int64_t)v72;
    v75 = *(int64_t*)v69;
    v76 = *(int64_t*)v73;
    v77 = v75 > v76;
    if (v77) goto bb10;
    else goto bb11;
  bb9:
    goto bb4;
  bb10:
    v79 = *(void**)v61;
    v80 = *(int64_t*)v73;
    *(int64_t*)v79 = (int64_t)v80;
    v82 = *(void**)v64;
    v83 = *(int64_t*)v69;
    *(int64_t*)v82 = (int64_t)v83;
    v85 = true;
    *(bool*)v27 = v85;
    goto bb12;
  bb11:
    goto bb12;
  bb12:
    v90 = *(int64_t*)v35;
    v91 = 1LL;
    v92 = v90 + v91;
    *(int64_t*)v35 = (int64_t)v92;
    goto bb7;
    return;
}

int64_t ByteBuffer_read_byte(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("ByteBuffer_read_byte");
    bool v8;
    int64_t v6;
    void* v14;
    int64_t v16;
    int64_t v18; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    bool v7;
    int64_t v17;
    int64_t v12; /* placeholder for void */
    int64_t v11;
    bool v4;
    void* v5;
    int64_t v10;
    int64_t v13; /* placeholder for void */
    void* v15;
    int64_t v3;
    v3 = 0LL;
    v4 = v2 < v3;
    v5 = (char*)v1 + 8;
    v6 = *(int64_t*)v5;
    v7 = v2 >= v6;
    v8 = v4 || v7;
    if (v8) goto bb1;
    else goto bb2;
  bb1:
    v10 = 1LL;
    v11 = -v10;
    p_call_stack_pop();
    return v11;
  bb2:
    goto bb3;
  bb3:
    v14 = (char*)v1 + 0;
    v15 = *(void**)v14;
    v16 = 0LL;
    v17 = (int64_t)Vec_get((void*)v16, (int64_t)v15, v2);
    p_call_stack_pop();
    return v17;
    return 0;
}

int64_t String_bytes(void* v0, int64_t v1) {
    p_call_stack_push("String_bytes");
    int64_t v2;
    int64_t v3;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

bool str_ends_with(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_ends_with");
    bool v4;
    int64_t v5; /* placeholder for void */
    int64_t v3;
    v3 = 0LL;
    v4 = (bool)String_ends_with((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

int64_t str_parse_int(void* v0, void* v1) {
    p_call_stack_push("str_parse_int");
    int64_t v2;
    int64_t v3;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_parse_int((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

void* create_allocator(void* v0, void* v1) {
    p_call_stack_push("create_allocator");
    int64_t v3; /* placeholder for void */
    void* v2;
    v2 = p_allocator_create(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t str_len(void* v0, void* v1) {
    p_call_stack_push("str_len");
    int64_t v3;
    int64_t v2;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_len((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

void* transpose(void* v0, void* v1) {
    p_call_stack_push("transpose");
    int64_t v3; /* placeholder for void */
    void* v2;
    v2 = p_tensor_transpose(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t HashMap_clone(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_clone");
    int64_t v50; /* placeholder for void */
    int64_t v6;
    int64_t v46; /* placeholder for void */
    void* v90;
    int64_t v54; /* placeholder for void */
    bool v38;
    int64_t v19;
    void* v17;
    int64_t v103;
    int64_t v55;
    int64_t v83;
    void* v87;
    int64_t v57;
    int64_t v43; /* placeholder for void */
    int64_t v108;
    int64_t v51;
    int64_t v49; /* placeholder for void */
    void* v66;
    int64_t v86;
    int64_t v102;
    int64_t v76;
    int64_t v33;
    int64_t v9; /* placeholder for void */
    void* v93;
    int64_t v67; /* placeholder for void */
    void* v4;
    void* v60;
    bool v79;
    int64_t v81; /* placeholder for void */
    int64_t v59;
    int64_t v74;
    int64_t v28; /* placeholder for void */
    bool v41;
    void* v96;
    int64_t v25; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v65;
    int64_t v37;
    int64_t v47;
    int64_t v95; /* placeholder for void */
    int64_t v40;
    int64_t v89;
    int64_t v56;
    int64_t v97;
    int64_t v34; /* placeholder for void */
    int64_t v13;
    int64_t v45; /* placeholder for void */
    int64_t v30;
    int64_t v107;
    int64_t v106; /* placeholder for void */
    int64_t v27;
    int64_t v52;
    int64_t v98; /* placeholder for void */
    int64_t v70;
    void* v69;
    int64_t v58;
    int64_t v104; /* placeholder for void */
    int64_t v105; /* placeholder for void */
    void* v63;
    int64_t v73;
    int64_t v101;
    int64_t v84;
    int64_t v72; /* placeholder for void */
    void* v48;
    void* v10;
    bool v80;
    int64_t v5; /* placeholder for void */
    int64_t v22;
    int64_t v100;
    int64_t v64; /* placeholder for void */
    void* v14;
    int64_t v15; /* placeholder for void */
    int64_t v77;
    int64_t v2;
    int64_t v111; /* placeholder for void */
    void* v20;
    int64_t v7;
    int64_t v24; /* placeholder for void */
    void* v32;
    void* v29;
    int64_t v16;
    int64_t v36;
    int64_t v12;
    int64_t v71; /* placeholder for void */
    int64_t v61; /* placeholder for void */
    int64_t v109;
    int64_t v112;
    bool v42;
    void* v23;
    int64_t v62;
    int64_t v85;
    int64_t v113; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    bool v75;
    int64_t v78;
    bool v53;
    int64_t v99; /* placeholder for void */
    int64_t v110; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v82;
    int64_t v21; /* placeholder for void */
    void* v8;
    int64_t v91; /* placeholder for void */
    void* v26;
    int64_t v3;
    int64_t v92;
    int64_t v88; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    int64_t v44;
    int64_t v39;
    int64_t v18; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)HashMap_new((void*)v2);
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = 24LL;
    v7 = v1 + v6;
    v8 = malloc(8);
    *(void**)v8 = (void*)v7;
    v10 = malloc(8);
    *(void**)v10 = (void*)v1;
    v12 = 8LL;
    v13 = v1 + v12;
    v14 = malloc(8);
    *(void**)v14 = (void*)v13;
    v16 = 0LL;
    v17 = malloc(8);
    *(int64_t*)v17 = (int64_t)v16;
    v19 = 0LL;
    v20 = malloc(8);
    *(int64_t*)v20 = (int64_t)v19;
    v22 = 0LL;
    v23 = malloc(8);
    *(int64_t*)v23 = (int64_t)v22;
    v26 = *(void**)v8;
    v27 = *(int64_t*)v26;
    *(int64_t*)v17 = (int64_t)v27;
    v29 = *(void**)v10;
    v30 = *(int64_t*)v29;
    *(int64_t*)v20 = (int64_t)v30;
    v32 = *(void**)v14;
    v33 = *(int64_t*)v32;
    *(int64_t*)v23 = (int64_t)v33;
    v36 = *(int64_t*)v20;
    v37 = 0LL;
    v38 = v36 == v37;
    v39 = *(int64_t*)v17;
    v40 = 0LL;
    v41 = v39 == v40;
    v42 = v38 || v41;
    if (v42) goto bb1;
    else goto bb2;
  bb1:
    v44 = *(int64_t*)v4;
    p_call_stack_pop();
    return v44;
  bb2:
    goto bb3;
  bb3:
    v47 = 0LL;
    v48 = malloc(8);
    *(int64_t*)v48 = (int64_t)v47;
    goto bb4;
  bb4:
    v51 = *(int64_t*)v48;
    v52 = *(int64_t*)v17;
    v53 = v51 < v52;
    if (v53) goto bb5;
    else goto bb6;
  bb5:
    v55 = *(int64_t*)v20;
    v56 = *(int64_t*)v48;
    v57 = 8LL;
    v58 = v56 * v57;
    v59 = v55 + v58;
    v60 = malloc(8);
    *(int64_t*)v60 = (int64_t)v59;
    v62 = *(int64_t*)v60;
    v63 = malloc(8);
    *(void**)v63 = (void*)v62;
    v65 = 0LL;
    v66 = malloc(8);
    *(int64_t*)v66 = (int64_t)v65;
    v69 = *(void**)v63;
    v70 = *(int64_t*)v69;
    *(int64_t*)v66 = (int64_t)v70;
    v73 = *(int64_t*)v66;
    v74 = 0LL;
    v75 = v73 != v74;
    v76 = *(int64_t*)v66;
    v77 = 1LL;
    v78 = -v77;
    v79 = v76 != v78;
    v80 = v75 && v79;
    if (v80) goto bb7;
    else goto bb8;
  bb6:
    v112 = *(int64_t*)v4;
    p_call_stack_pop();
    return v112;
  bb7:
    v82 = *(int64_t*)v23;
    v83 = *(int64_t*)v48;
    v84 = 8LL;
    v85 = v83 * v84;
    v86 = v82 + v85;
    v87 = malloc(8);
    *(int64_t*)v87 = (int64_t)v86;
    v89 = *(int64_t*)v87;
    v90 = malloc(8);
    *(void**)v90 = (void*)v89;
    v92 = 0LL;
    v93 = malloc(8);
    *(int64_t*)v93 = (int64_t)v92;
    v96 = *(void**)v90;
    v97 = *(int64_t*)v96;
    *(int64_t*)v93 = (int64_t)v97;
    v100 = *(int64_t*)v4;
    v101 = *(int64_t*)v66;
    v102 = *(int64_t*)v93;
    v103 = 0LL;
    HashMap_insert((void*)v103, v100, v101, v102);
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    v107 = *(int64_t*)v48;
    v108 = 1LL;
    v109 = v107 + v108;
    *(int64_t*)v48 = (int64_t)v109;
    goto bb4;
    return 0;
}

void assert(void* v0, bool v1) {
    p_call_stack_push("assert");
    void* v4;
    int64_t v6; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v3; /* placeholder for void */
    int64_t v2;
    v2 = !v1;
    if (v2) goto bb1;
    else goto bb2;
  bb1:
    v4 = _str13;
    p_print_str(v4);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return;
    return;
}

int64_t String_slice(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("String_slice");
    int64_t v42;
    int64_t v59;
    int64_t v79;
    int64_t v62;
    int64_t v45; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    bool v48;
    bool v32;
    int64_t v7;
    void* v91;
    int64_t v66;
    int64_t v46;
    int64_t v17; /* placeholder for void */
    int64_t v71;
    int64_t v94; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    void* v58;
    int64_t v43; /* placeholder for void */
    bool v5;
    int64_t v44; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v84;
    void* v8;
    int64_t v14; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v76;
    int64_t v4;
    void* v52;
    int64_t v92;
    void* v12;
    void* v88;
    int64_t v80;
    int64_t v11;
    void* v85;
    void* v55;
    int64_t v72;
    int64_t v70;
    int64_t v63; /* placeholder for void */
    int64_t v82;
    int64_t v77;
    int64_t v41; /* placeholder for void */
    void* v68;
    void* v15;
    int64_t v67;
    int64_t v37; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v54;
    int64_t v31;
    int64_t v61; /* placeholder for void */
    void* v9;
    int64_t v29; /* placeholder for void */
    int64_t v22;
    void* v73;
    void* v74;
    int64_t v78;
    int64_t v10; /* placeholder for void */
    int64_t v16;
    int64_t v20; /* placeholder for void */
    void* v28;
    int64_t v34;
    int64_t v38;
    int64_t v83;
    int64_t v86; /* placeholder for void */
    int64_t v87;
    int64_t v89; /* placeholder for void */
    int64_t v95;
    int64_t v19;
    int64_t v36; /* placeholder for void */
    int64_t v39;
    int64_t v30;
    int64_t v57; /* placeholder for void */
    void* v26;
    int64_t v65;
    int64_t v75; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v23;
    int64_t v53; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v50;
    int64_t v25; /* placeholder for void */
    int64_t v47;
    int64_t v93; /* placeholder for void */
    void* v24;
    int64_t v18; /* placeholder for void */
    void* v51;
    int64_t v33; /* placeholder for void */
    bool v40;
    v4 = 0LL;
    v5 = v1 == v4;
    if (v5) goto bb1;
    else goto bb2;
  bb1:
    v7 = 1LL;
    v8 = p_malloc(v7);
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = *(int64_t*)v9;
    v12 = malloc(8);
    *(void**)v12 = (void*)v11;
    v15 = *(void**)v12;
    v16 = 0LL;
    *(uint8_t*)v15 = (uint8_t)v16;
    v19 = *(int64_t*)v9;
    p_call_stack_pop();
    return v19;
  bb2:
    goto bb3;
  bb3:
    v22 = 0LL;
    v23 = (int64_t)p_str_length((void*)v22, v1);
    v24 = malloc(8);
    *(int64_t*)v24 = (int64_t)v23;
    v26 = malloc(8);
    *(int64_t*)v26 = (int64_t)v2;
    v28 = malloc(8);
    *(int64_t*)v28 = (int64_t)v3;
    v30 = *(int64_t*)v26;
    v31 = 0LL;
    v32 = v30 < v31;
    if (v32) goto bb4;
    else goto bb5;
  bb4:
    v34 = 0LL;
    *(int64_t*)v26 = (int64_t)v34;
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v38 = *(int64_t*)v28;
    v39 = *(int64_t*)v24;
    v40 = v38 > v39;
    if (v40) goto bb7;
    else goto bb8;
  bb7:
    v42 = *(int64_t*)v24;
    *(int64_t*)v28 = (int64_t)v42;
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    v46 = *(int64_t*)v26;
    v47 = *(int64_t*)v28;
    v48 = v46 >= v47;
    if (v48) goto bb10;
    else goto bb11;
  bb10:
    v50 = 1LL;
    v51 = p_malloc(v50);
    v52 = malloc(8);
    *(int64_t*)v52 = (int64_t)v51;
    v54 = *(int64_t*)v52;
    v55 = malloc(8);
    *(void**)v55 = (void*)v54;
    v58 = *(void**)v55;
    v59 = 0LL;
    *(uint8_t*)v58 = (uint8_t)v59;
    v62 = *(int64_t*)v52;
    p_call_stack_pop();
    return v62;
  bb11:
    goto bb12;
  bb12:
    v65 = *(int64_t*)v28;
    v66 = *(int64_t*)v26;
    v67 = v65 - v66;
    v68 = malloc(8);
    *(int64_t*)v68 = (int64_t)v67;
    v70 = *(int64_t*)v68;
    v71 = 1LL;
    v72 = v70 + v71;
    v73 = p_malloc(v72);
    v74 = malloc(8);
    *(int64_t*)v74 = (int64_t)v73;
    v76 = *(int64_t*)v74;
    v77 = *(int64_t*)v26;
    v78 = v1 + v77;
    v79 = *(int64_t*)v68;
    v80 = 0LL;
    p_str_copy_n((void*)v80, v76, v78, v79);
    v82 = *(int64_t*)v74;
    v83 = *(int64_t*)v68;
    v84 = v82 + v83;
    v85 = malloc(8);
    *(int64_t*)v85 = (int64_t)v84;
    v87 = *(int64_t*)v85;
    v88 = malloc(8);
    *(void**)v88 = (void*)v87;
    v91 = *(void**)v88;
    v92 = 0LL;
    *(uint8_t*)v91 = (uint8_t)v92;
    v95 = *(int64_t*)v74;
    p_call_stack_pop();
    return v95;
    return 0;
}

bool char_is_upper(void* v0, int64_t v1) {
    p_call_stack_push("char_is_upper");
    bool v3;
    bool v5;
    bool v6;
    int64_t v4;
    int64_t v2;
    int64_t v7; /* placeholder for void */
    v2 = 65LL;
    v3 = v1 >= v2;
    v4 = 90LL;
    v5 = v1 <= v4;
    v6 = v3 && v5;
    p_call_stack_pop();
    return v6;
    return 0;
}

int64_t Vec_pop(void* v0, int64_t v1) {
    p_call_stack_push("Vec_pop");
    int64_t v38; /* placeholder for void */
    int64_t v47;
    int64_t v27;
    int64_t v7; /* placeholder for void */
    int64_t v42;
    int64_t v19; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v48;
    int64_t v9; /* placeholder for void */
    void* v34;
    int64_t v25; /* placeholder for void */
    int64_t v2;
    int64_t v35; /* placeholder for void */
    int64_t v3;
    bool v16;
    void* v8;
    int64_t v24;
    int64_t v50; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    void* v41;
    void* v51;
    int64_t v54; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v33;
    int64_t v14;
    int64_t v53; /* placeholder for void */
    int64_t v11;
    int64_t v13; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    void* v39;
    int64_t v18;
    int64_t v32;
    int64_t v52;
    int64_t v31;
    int64_t v43; /* placeholder for void */
    void* v4;
    int64_t v45;
    int64_t v20; /* placeholder for void */
    int64_t v29;
    int64_t v55;
    int64_t v46;
    void* v21;
    void* v6;
    int64_t v30;
    int64_t v12; /* placeholder for void */
    void* v23;
    int64_t v5; /* placeholder for void */
    void* v10;
    int64_t v36;
    int64_t v28;
    int64_t v15;
    void* v37;
    v2 = 8LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = malloc(8);
    *(void**)v6 = (void*)v1;
    v8 = malloc(8);
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v8 = (int64_t)v11;
    v14 = *(int64_t*)v8;
    v15 = 0LL;
    v16 = v14 == v15;
    if (v16) goto bb1;
    else goto bb2;
  bb1:
    v18 = 0LL;
    p_call_stack_pop();
    return v18;
  bb2:
    goto bb3;
  bb3:
    v21 = malloc(8);
    v23 = *(void**)v6;
    v24 = *(int64_t*)v23;
    *(int64_t*)v21 = (int64_t)v24;
    v27 = *(int64_t*)v21;
    v28 = *(int64_t*)v8;
    v29 = 1LL;
    v30 = v28 - v29;
    v31 = 8LL;
    v32 = v30 * v31;
    v33 = v27 + v32;
    v34 = malloc(8);
    *(int64_t*)v34 = (int64_t)v33;
    v36 = *(int64_t*)v34;
    v37 = malloc(8);
    *(void**)v37 = (void*)v36;
    v39 = malloc(8);
    v41 = *(void**)v37;
    v42 = *(int64_t*)v41;
    *(int64_t*)v39 = (int64_t)v42;
    v45 = *(int64_t*)v8;
    v46 = 1LL;
    v47 = v45 - v46;
    v48 = malloc(8);
    *(int64_t*)v48 = (int64_t)v47;
    v51 = *(void**)v4;
    v52 = *(int64_t*)v48;
    *(int64_t*)v51 = (int64_t)v52;
    v55 = *(int64_t*)v39;
    p_call_stack_pop();
    return v55;
    return 0;
}

int64_t str_parse_int_range(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("str_parse_int_range");
    int64_t v85; /* placeholder for void */
    int64_t v21;
    int64_t v47;
    int64_t v23; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v35;
    bool v44;
    int64_t v52; /* placeholder for void */
    bool v30;
    int64_t v34; /* placeholder for void */
    int64_t v80;
    int64_t v5;
    int64_t v40; /* placeholder for void */
    bool v66;
    int64_t v16;
    int64_t v73;
    int64_t v45; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v29;
    int64_t v31; /* placeholder for void */
    uint8_t v58;
    bool v22;
    int64_t v62;
    int64_t v17; /* placeholder for void */
    void* v8;
    int64_t v77;
    int64_t v81;
    int64_t v83; /* placeholder for void */
    int64_t v64;
    int64_t v12;
    int64_t v37; /* placeholder for void */
    bool v67;
    int64_t v74;
    int64_t v4;
    int64_t v28;
    int64_t v53;
    int64_t v20;
    int64_t v50;
    int64_t v42;
    void* v48;
    int64_t v49; /* placeholder for void */
    int64_t v38;
    bool v14;
    int64_t v56; /* placeholder for void */
    void* v39;
    int64_t v41; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    int64_t v61;
    int64_t v68; /* placeholder for void */
    int64_t v72;
    int64_t v79;
    int64_t v69; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    void* v51;
    int64_t v46;
    int64_t v65;
    int64_t v71;
    int64_t v25; /* placeholder for void */
    void* v36;
    int64_t v11; /* placeholder for void */
    int64_t v32;
    int64_t v18; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v76;
    int64_t v82; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v13;
    void* v6;
    int64_t v24;
    int64_t v43;
    int64_t v70; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    void* v57;
    int64_t v75;
    int64_t v7; /* placeholder for void */
    int64_t v84;
    int64_t v78; /* placeholder for void */
    void* v54;
    void* v10;
    int64_t v55; /* placeholder for void */
    bool v63;
    v4 = 0LL;
    v5 = (int64_t)String_len((void*)v4, (int64_t)v1);
    v6 = malloc(8);
    *(int64_t*)v6 = (int64_t)v5;
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v2;
    v10 = malloc(8);
    *(int64_t*)v10 = (int64_t)v3;
    v12 = *(int64_t*)v8;
    v13 = 0LL;
    v14 = v12 < v13;
    if (v14) goto bb1;
    else goto bb2;
  bb1:
    v16 = 0LL;
    *(int64_t*)v8 = (int64_t)v16;
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v20 = *(int64_t*)v10;
    v21 = *(int64_t*)v6;
    v22 = v20 > v21;
    if (v22) goto bb4;
    else goto bb5;
  bb4:
    v24 = *(int64_t*)v6;
    *(int64_t*)v10 = (int64_t)v24;
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v28 = *(int64_t*)v8;
    v29 = *(int64_t*)v10;
    v30 = v28 >= v29;
    if (v30) goto bb7;
    else goto bb8;
  bb7:
    v32 = 0LL;
    p_call_stack_pop();
    return v32;
  bb8:
    goto bb9;
  bb9:
    v35 = 0LL;
    v36 = malloc(8);
    *(int64_t*)v36 = (int64_t)v35;
    v38 = *(int64_t*)v8;
    v39 = malloc(8);
    *(int64_t*)v39 = (int64_t)v38;
    goto bb10;
  bb10:
    v42 = *(int64_t*)v39;
    v43 = *(int64_t*)v10;
    v44 = v42 < v43;
    if (v44) goto bb11;
    else goto bb12;
  bb11:
    v46 = *(int64_t*)v39;
    v47 = (int64_t)v1 + v46;
    v48 = malloc(8);
    *(int64_t*)v48 = (int64_t)v47;
    v50 = *(int64_t*)v48;
    v51 = malloc(8);
    *(void**)v51 = (void*)v50;
    v53 = 0LL;
    v54 = malloc(8);
    *(int64_t*)v54 = (int64_t)v53;
    v57 = *(void**)v51;
    v58 = *(uint8_t*)v57;
    *(int64_t*)v54 = (int64_t)v58;
    v61 = *(int64_t*)v54;
    v62 = 48LL;
    v63 = v61 < v62;
    v64 = *(int64_t*)v54;
    v65 = 57LL;
    v66 = v64 > v65;
    v67 = v63 || v66;
    if (v67) goto bb13;
    else goto bb14;
  bb12:
    v84 = *(int64_t*)v36;
    p_call_stack_pop();
    return v84;
  bb13:
    goto bb12;
  bb14:
    goto bb15;
  bb15:
    v71 = *(int64_t*)v36;
    v72 = 10LL;
    v73 = v71 * v72;
    v74 = *(int64_t*)v54;
    v75 = 48LL;
    v76 = v74 - v75;
    v77 = v73 + v76;
    *(int64_t*)v36 = (int64_t)v77;
    v79 = *(int64_t*)v39;
    v80 = 1LL;
    v81 = v79 + v80;
    *(int64_t*)v39 = (int64_t)v81;
    goto bb10;
    return 0;
}

void Result_err(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_err");
    int64_t v44; /* placeholder for void */
    void* v18;
    bool v26;
    int64_t v9;
    int64_t v17; /* placeholder for void */
    int64_t v31;
    void* v32;
    void* v3;
    int64_t v39; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v38;
    void* v34;
    int64_t v16;
    void* v41;
    int64_t v21; /* placeholder for void */
    int64_t v20;
    void* v28;
    int64_t v33; /* placeholder for void */
    int64_t v24;
    void* v23;
    int64_t v4;
    void* v15;
    bool v6;
    void* v19;
    int64_t v11;
    int64_t v42;
    int64_t v27; /* placeholder for void */
    int64_t v5;
    void* v8;
    int64_t v29;
    void* v37;
    int64_t v7; /* placeholder for void */
    void* v14;
    int64_t v22; /* placeholder for void */
    void* v10;
    int64_t v45; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    void* v36;
    void* v30;
    int64_t v13; /* placeholder for void */
    int64_t v25;
    void* v40;
    int64_t v43; /* placeholder for void */
    void* v12;
    v3 = (char*)v2 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    p_call_stack_pop();
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
    p_call_stack_pop();
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
    p_call_stack_pop();
    return;
  bb5:
    goto bb1;
    return;
}

void HashMap_insert(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("HashMap_insert");
    int64_t v15;
    int64_t v73;
    void* v58;
    int64_t v42;
    int64_t v54;
    int64_t v21; /* placeholder for void */
    void* v67;
    int64_t v121; /* placeholder for void */
    bool v95;
    int64_t v69; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    int64_t v129;
    int64_t v60;
    int64_t v130;
    int64_t v14; /* placeholder for void */
    int64_t v89;
    void* v99;
    int64_t v18; /* placeholder for void */
    void* v49;
    int64_t v75;
    int64_t v30;
    int64_t v50; /* placeholder for void */
    int64_t v111; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v134;
    int64_t v52; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v105;
    bool v96;
    int64_t v106;
    void* v61;
    int64_t v118;
    int64_t v97; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v5;
    void* v119;
    int64_t v128;
    int64_t v87; /* placeholder for void */
    int64_t v32;
    int64_t v92;
    int64_t v103;
    int64_t v108; /* placeholder for void */
    int64_t v133; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v102;
    int64_t v78;
    void* v76;
    void* v110;
    int64_t v68;
    void* v82;
    bool v28;
    int64_t v140; /* placeholder for void */
    int64_t v33;
    int64_t v63; /* placeholder for void */
    void* v132;
    int64_t v31;
    int64_t v34;
    int64_t v93;
    int64_t v137; /* placeholder for void */
    int64_t v86;
    int64_t v98; /* placeholder for void */
    void* v22;
    int64_t v39; /* placeholder for void */
    int64_t v47;
    int64_t v46; /* placeholder for void */
    bool v91;
    int64_t v120; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v124; /* placeholder for void */
    int64_t v80; /* placeholder for void */
    int64_t v88; /* placeholder for void */
    int64_t v126; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v9;
    void* v16;
    int64_t v136; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v100; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    int64_t v71;
    int64_t v27;
    int64_t v25; /* placeholder for void */
    int64_t v104;
    void* v138;
    int64_t v94;
    void* v122;
    int64_t v131;
    int64_t v141; /* placeholder for void */
    void* v51;
    int64_t v56; /* placeholder for void */
    int64_t v90;
    int64_t v24; /* placeholder for void */
    int64_t v116;
    int64_t v112; /* placeholder for void */
    int64_t v72;
    void* v19;
    int64_t v20;
    int64_t v23;
    int64_t v29;
    void* v135;
    void* v55;
    int64_t v81;
    int64_t v101; /* placeholder for void */
    void* v6;
    bool v35;
    void* v64;
    void* v79;
    int64_t v117;
    int64_t v139; /* placeholder for void */
    int64_t v57;
    int64_t v74;
    int64_t v123;
    int64_t v114; /* placeholder for void */
    int64_t v142; /* placeholder for void */
    int64_t v8;
    int64_t v66; /* placeholder for void */
    int64_t v4;
    int64_t v127;
    int64_t v53;
    int64_t v37; /* placeholder for void */
    void* v85;
    void* v113;
    void* v41;
    int64_t v109;
    int64_t v125; /* placeholder for void */
    int64_t v48;
    int64_t v12;
    void* v107;
    int64_t v38;
    bool v36;
    void* v13;
    int64_t v65;
    int64_t v26;
    int64_t v70; /* placeholder for void */
    int64_t v115; /* placeholder for void */
    void* v10;
    v4 = 16LL;
    v5 = v1 + v4;
    v6 = malloc(8);
    *(void**)v6 = (void*)v5;
    v8 = 24LL;
    v9 = v1 + v8;
    v10 = malloc(8);
    *(void**)v10 = (void*)v9;
    v12 = 0LL;
    v13 = malloc(8);
    *(int64_t*)v13 = (int64_t)v12;
    v15 = 0LL;
    v16 = malloc(8);
    *(int64_t*)v16 = (int64_t)v15;
    v19 = *(void**)v6;
    v20 = *(int64_t*)v19;
    *(int64_t*)v13 = (int64_t)v20;
    v22 = *(void**)v10;
    v23 = *(int64_t*)v22;
    *(int64_t*)v16 = (int64_t)v23;
    v26 = *(int64_t*)v16;
    v27 = 0LL;
    v28 = v26 == v27;
    v29 = *(int64_t*)v13;
    v30 = 10LL;
    v31 = v29 * v30;
    v32 = *(int64_t*)v16;
    v33 = 7LL;
    v34 = v32 * v33;
    v35 = v31 >= v34;
    v36 = v28 || v35;
    if (v36) goto bb1;
    else goto bb2;
  bb1:
    v38 = 0LL;
    HashMap_grow((void*)v38, v1);
    v41 = *(void**)v10;
    v42 = *(int64_t*)v41;
    *(int64_t*)v16 = (int64_t)v42;
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v47 = 0LL;
    v48 = (int64_t)HashMap_find_slot((void*)v47, v1, v2);
    v49 = malloc(8);
    *(int64_t*)v49 = (int64_t)v48;
    v51 = malloc(8);
    *(void**)v51 = (void*)v1;
    v53 = 8LL;
    v54 = v1 + v53;
    v55 = malloc(8);
    *(void**)v55 = (void*)v54;
    v57 = 0LL;
    v58 = malloc(8);
    *(int64_t*)v58 = (int64_t)v57;
    v60 = 0LL;
    v61 = malloc(8);
    *(int64_t*)v61 = (int64_t)v60;
    v64 = *(void**)v51;
    v65 = *(int64_t*)v64;
    *(int64_t*)v58 = (int64_t)v65;
    v67 = *(void**)v55;
    v68 = *(int64_t*)v67;
    *(int64_t*)v61 = (int64_t)v68;
    v71 = *(int64_t*)v58;
    v72 = *(int64_t*)v49;
    v73 = 8LL;
    v74 = v72 * v73;
    v75 = v71 + v74;
    v76 = malloc(8);
    *(int64_t*)v76 = (int64_t)v75;
    v78 = *(int64_t*)v76;
    v79 = malloc(8);
    *(void**)v79 = (void*)v78;
    v81 = 0LL;
    v82 = malloc(8);
    *(int64_t*)v82 = (int64_t)v81;
    v85 = *(void**)v79;
    v86 = *(int64_t*)v85;
    *(int64_t*)v82 = (int64_t)v86;
    v89 = *(int64_t*)v82;
    v90 = 0LL;
    v91 = v89 == v90;
    v92 = *(int64_t*)v82;
    v93 = 1LL;
    v94 = -v93;
    v95 = v92 == v94;
    v96 = v91 || v95;
    if (v96) goto bb4;
    else goto bb5;
  bb4:
    v99 = *(void**)v79;
    *(int64_t*)v99 = (int64_t)v2;
    v102 = *(int64_t*)v61;
    v103 = *(int64_t*)v49;
    v104 = 8LL;
    v105 = v103 * v104;
    v106 = v102 + v105;
    v107 = malloc(8);
    *(int64_t*)v107 = (int64_t)v106;
    v109 = *(int64_t*)v107;
    v110 = malloc(8);
    *(void**)v110 = (void*)v109;
    v113 = *(void**)v110;
    *(int64_t*)v113 = (int64_t)v3;
    v116 = *(int64_t*)v13;
    v117 = 1LL;
    v118 = v116 + v117;
    v119 = malloc(8);
    *(int64_t*)v119 = (int64_t)v118;
    v122 = *(void**)v6;
    v123 = *(int64_t*)v119;
    *(int64_t*)v122 = (int64_t)v123;
    goto bb6;
  bb5:
    v127 = *(int64_t*)v61;
    v128 = *(int64_t*)v49;
    v129 = 8LL;
    v130 = v128 * v129;
    v131 = v127 + v130;
    v132 = malloc(8);
    *(int64_t*)v132 = (int64_t)v131;
    v134 = *(int64_t*)v132;
    v135 = malloc(8);
    *(void**)v135 = (void*)v134;
    v138 = *(void**)v135;
    *(int64_t*)v138 = (int64_t)v3;
    goto bb6;
  bb6:
    p_call_stack_pop();
    return;
    return;
}

double mean(void* v0, void* v1) {
    p_call_stack_push("mean");
    double v2;
    int64_t v3; /* placeholder for void */
    v2 = (double)p_tensor_mean(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t main_entry(void* v0) {
    p_call_stack_push("main");
    int64_t v218;
    void* v164;
    int64_t v22;
    int64_t v50; /* placeholder for void */
    void* v72;
    void* v170;
    int64_t v183;
    void* v9;
    int64_t v58;
    void* v107;
    int64_t v27;
    void* v87;
    void* v167;
    int64_t v240;
    int64_t v140;
    void* v216;
    int64_t v285;
    void* v234;
    double v102;
    void* v37;
    int64_t v26;
    int64_t v143;
    int64_t v141; /* placeholder for void */
    int64_t v127;
    int64_t v280;
    double v68;
    int64_t v166;
    void* v33;
    void* v146;
    void* v149;
    void* v47;
    int64_t v173; /* placeholder for void */
    int64_t v233; /* placeholder for void */
    int64_t v135;
    int64_t v175; /* placeholder for void */
    int64_t v203; /* placeholder for void */
    void* v262;
    int64_t v302; /* placeholder for void */
    void* v244;
    void* v293;
    void* v295;
    int64_t v282; /* placeholder for void */
    void* v162;
    int64_t v205; /* placeholder for void */
    int64_t v274; /* placeholder for void */
    int64_t v255;
    void* v138;
    int64_t v139;
    void* v49;
    int64_t v230;
    void* v194;
    int64_t v217;
    int64_t v11;
    int64_t v63;
    void* v3;
    int64_t v79;
    void* v299;
    int64_t v67;
    int64_t v73;
    int64_t v46; /* placeholder for void */
    int64_t v193; /* placeholder for void */
    int64_t v277;
    int64_t v101;
    void* v155;
    int64_t v64; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v129; /* placeholder for void */
    void* v176;
    void* v5;
    int64_t v80;
    int64_t v31;
    void* v41;
    void* v17;
    int64_t v124;
    int64_t v253; /* placeholder for void */
    int64_t v296;
    void* v86;
    void* v55;
    int64_t v103; /* placeholder for void */
    int64_t v111;
    void* v179;
    int64_t v241;
    int64_t v77; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v268; /* placeholder for void */
    int64_t v19;
    void* v182;
    void* v153;
    void* v120;
    int64_t v121; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v181; /* placeholder for void */
    int64_t v227;
    void* v82;
    void* v126;
    int64_t v250;
    int64_t v92; /* placeholder for void */
    int64_t v211; /* placeholder for void */
    void* v265;
    int64_t v301;
    int64_t v12; /* placeholder for void */
    void* v198;
    int64_t v290;
    double v91;
    int64_t v40; /* placeholder for void */
    int64_t v116;
    int64_t v60; /* placeholder for void */
    int64_t v118;
    int64_t v209; /* placeholder for void */
    void* v98;
    int64_t v52; /* placeholder for void */
    int64_t v298; /* placeholder for void */
    int64_t v169; /* placeholder for void */
    int64_t v163; /* placeholder for void */
    int64_t v10;
    int64_t v24; /* placeholder for void */
    int64_t v248; /* placeholder for void */
    void* v122;
    int64_t v207;
    void* v238;
    void* v263;
    void* v246;
    void* v284;
    int64_t v224; /* placeholder for void */
    int64_t v88; /* placeholder for void */
    void* v269;
    int64_t v7;
    void* v76;
    int64_t v148;
    int64_t v292; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v34;
    void* v297;
    void* v51;
    int64_t v271;
    int64_t v185; /* placeholder for void */
    int64_t v13;
    int64_t v242; /* placeholder for void */
    int64_t v48;
    void* v192;
    int64_t v212;
    void* v150;
    int64_t v266;
    void* v152;
    int64_t v245;
    int64_t v201;
    void* v221;
    int64_t v261;
    int64_t v4; /* placeholder for void */
    void* v93;
    int64_t v235;
    int64_t v161; /* placeholder for void */
    int64_t v228; /* placeholder for void */
    void* v100;
    void* v66;
    void* v287;
    void* v14;
    int64_t v136;
    void* v78;
    int64_t v8; /* placeholder for void */
    int64_t v145; /* placeholder for void */
    int64_t v106;
    int64_t v151; /* placeholder for void */
    void* v54;
    void* v108;
    int64_t v264; /* placeholder for void */
    int64_t v223;
    int64_t v115;
    int64_t v231;
    int64_t v96;
    int64_t v278; /* placeholder for void */
    int64_t v1;
    int64_t v191; /* placeholder for void */
    void* v114;
    void* v174;
    int64_t v239;
    int64_t v43;
    void* v2;
    void* v275;
    double v208;
    void* v279;
    int64_t v132;
    int64_t v171;
    int64_t v178;
    int64_t v74;
    int64_t v187; /* placeholder for void */
    int64_t v189;
    int64_t v144;
    int64_t v226;
    void* v89;
    void* v65;
    int64_t v30;
    void* v70;
    int64_t v113; /* placeholder for void */
    void* v180;
    int64_t v197; /* placeholder for void */
    void* v214;
    void* v273;
    void* v283;
    void* v256;
    int64_t v90;
    int64_t v133; /* placeholder for void */
    int64_t v159;
    int64_t v20; /* placeholder for void */
    int64_t v154;
    int64_t v281;
    void* v243;
    void* v84;
    void* v119;
    void* v168;
    int64_t v53;
    int64_t v6;
    void* v61;
    int64_t v123;
    void* v29;
    void* v186;
    void* v267;
    int64_t v137; /* placeholder for void */
    void* v44;
    void* v291;
    void* v165;
    void* v210;
    void* v158;
    int64_t v259; /* placeholder for void */
    int64_t v62;
    int64_t v157; /* placeholder for void */
    int64_t v38;
    double v196;
    int64_t v276;
    int64_t v128;
    int64_t v39;
    void* v130;
    void* v254;
    void* v213;
    int64_t v222;
    int64_t v257; /* placeholder for void */
    void* v134;
    void* v260;
    void* v204;
    int64_t v71; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    void* v147;
    int64_t v215; /* placeholder for void */
    int64_t v219;
    int64_t v237; /* placeholder for void */
    void* v272;
    int64_t v294; /* placeholder for void */
    void* v110;
    void* v156;
    int64_t v199; /* placeholder for void */
    int64_t v23;
    void* v289;
    int64_t v109; /* placeholder for void */
    double v177;
    int64_t v99; /* placeholder for void */
    void* v21;
    void* v25;
    int64_t v131;
    void* v200;
    void* v249;
    void* v142;
    void* v258;
    void* v229;
    int64_t v59;
    int64_t v81; /* placeholder for void */
    int64_t v232;
    void* v252;
    void* v184;
    void* v172;
    int64_t v125; /* placeholder for void */
    int64_t v195;
    int64_t v300; /* placeholder for void */
    void* v45;
    void* v160;
    void* v188;
    void* v57;
    int64_t v18;
    int64_t v94; /* placeholder for void */
    int64_t v35;
    double v190;
    int64_t v105; /* placeholder for void */
    void* v42;
    void* v95;
    int64_t v117; /* placeholder for void */
    int64_t v85;
    int64_t v220; /* placeholder for void */
    int64_t v236;
    int64_t v56; /* placeholder for void */
    void* v247;
    void* v251;
    void* v206;
    int64_t v270; /* placeholder for void */
    void* v286;
    int64_t v112;
    void* v225;
    int64_t v288; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    void* v104;
    int64_t v69; /* placeholder for void */
    double v202;
    void* v15;
    void* v97;
    v1 = 0LL;
    v2 = (void*)Vec_new((void*)v1);
    v3 = malloc(8);
    *(void**)v3 = (void*)v2;
    v5 = *(void**)v3;
    v6 = 2LL;
    v7 = 0LL;
    Vec_push((void*)v7, (int64_t)v5, v6);
    v9 = *(void**)v3;
    v10 = 3LL;
    v11 = 0LL;
    Vec_push((void*)v11, (int64_t)v9, v10);
    v13 = 0LL;
    v14 = (void*)Vec_new((void*)v13);
    v15 = malloc(8);
    *(void**)v15 = (void*)v14;
    v17 = *(void**)v15;
    v18 = 1LL;
    v19 = 0LL;
    Vec_push((void*)v19, (int64_t)v17, v18);
    v21 = *(void**)v15;
    v22 = 2LL;
    v23 = 0LL;
    Vec_push((void*)v23, (int64_t)v21, v22);
    v25 = *(void**)v15;
    v26 = 3LL;
    v27 = 0LL;
    Vec_push((void*)v27, (int64_t)v25, v26);
    v29 = *(void**)v15;
    v30 = 4LL;
    v31 = 0LL;
    Vec_push((void*)v31, (int64_t)v29, v30);
    v33 = *(void**)v15;
    v34 = 5LL;
    v35 = 0LL;
    Vec_push((void*)v35, (int64_t)v33, v34);
    v37 = *(void**)v15;
    v38 = 6LL;
    v39 = 0LL;
    Vec_push((void*)v39, (int64_t)v37, v38);
    v41 = *(void**)v15;
    v42 = *(void**)v3;
    v43 = 0LL;
    v44 = from_vec((void*)v43, v41, v42);
    v45 = malloc(8);
    *(void**)v45 = (void*)v44;
    v47 = *(void**)v45;
    v48 = 0LL;
    v49 = to_str((void*)v48, v47);
    p_print_str(v49);
    v51 = _str4;
    p_print_str(v51);
    v53 = 0LL;
    v54 = (void*)Vec_new((void*)v53);
    v55 = malloc(8);
    *(void**)v55 = (void*)v54;
    v57 = *(void**)v55;
    v58 = 1LL;
    v59 = 0LL;
    Vec_push((void*)v59, (int64_t)v57, v58);
    v61 = *(void**)v55;
    v62 = 2LL;
    v63 = 0LL;
    Vec_push((void*)v63, (int64_t)v61, v62);
    v65 = *(void**)v45;
    v66 = *(void**)v55;
    v67 = 0LL;
    v68 = (double)get((void*)v67, v65, v66);
    p_print_float(v68);
    v70 = _str4;
    p_print_str(v70);
    v72 = *(void**)v45;
    v73 = 0LL;
    v74 = (int64_t)ndim((void*)v73, v72);
    p_print_int((int64_t)v74);
    v76 = _str4;
    p_print_str(v76);
    v78 = *(void**)v45;
    v79 = 0LL;
    v80 = (int64_t)size((void*)v79, v78);
    p_print_int((int64_t)v80);
    v82 = _str4;
    p_print_str(v82);
    v84 = *(void**)v3;
    v85 = 0LL;
    v86 = zeros((void*)v85, v84);
    v87 = malloc(8);
    *(void**)v87 = (void*)v86;
    v89 = *(void**)v87;
    v90 = 0LL;
    v91 = (double)sum((void*)v90, v89);
    p_print_float(v91);
    v93 = _str4;
    p_print_str(v93);
    v95 = *(void**)v3;
    v96 = 0LL;
    v97 = ones((void*)v96, v95);
    v98 = malloc(8);
    *(void**)v98 = (void*)v97;
    v100 = *(void**)v98;
    v101 = 0LL;
    v102 = (double)sum((void*)v101, v100);
    p_print_float(v102);
    v104 = _str4;
    p_print_str(v104);
    v106 = 0LL;
    v107 = (void*)Vec_new((void*)v106);
    v108 = malloc(8);
    *(void**)v108 = (void*)v107;
    v110 = *(void**)v108;
    v111 = 3LL;
    v112 = 0LL;
    Vec_push((void*)v112, (int64_t)v110, v111);
    v114 = *(void**)v108;
    v115 = 2LL;
    v116 = 0LL;
    Vec_push((void*)v116, (int64_t)v114, v115);
    v118 = 0LL;
    v119 = (void*)Vec_new((void*)v118);
    v120 = malloc(8);
    *(void**)v120 = (void*)v119;
    v122 = *(void**)v120;
    v123 = 7LL;
    v124 = 0LL;
    Vec_push((void*)v124, (int64_t)v122, v123);
    v126 = *(void**)v120;
    v127 = 8LL;
    v128 = 0LL;
    Vec_push((void*)v128, (int64_t)v126, v127);
    v130 = *(void**)v120;
    v131 = 9LL;
    v132 = 0LL;
    Vec_push((void*)v132, (int64_t)v130, v131);
    v134 = *(void**)v120;
    v135 = 10LL;
    v136 = 0LL;
    Vec_push((void*)v136, (int64_t)v134, v135);
    v138 = *(void**)v120;
    v139 = 11LL;
    v140 = 0LL;
    Vec_push((void*)v140, (int64_t)v138, v139);
    v142 = *(void**)v120;
    v143 = 12LL;
    v144 = 0LL;
    Vec_push((void*)v144, (int64_t)v142, v143);
    v146 = *(void**)v120;
    v147 = *(void**)v108;
    v148 = 0LL;
    v149 = from_vec((void*)v148, v146, v147);
    v150 = malloc(8);
    *(void**)v150 = (void*)v149;
    v152 = *(void**)v45;
    v153 = *(void**)v150;
    v154 = 0LL;
    v155 = matmul((void*)v154, v152, v153);
    v156 = malloc(8);
    *(void**)v156 = (void*)v155;
    v158 = *(void**)v156;
    v159 = 0LL;
    v160 = to_str((void*)v159, v158);
    p_print_str(v160);
    v162 = _str4;
    p_print_str(v162);
    v164 = *(void**)v45;
    v165 = *(void**)v45;
    v166 = 0LL;
    v167 = add((void*)v166, v164, v165);
    v168 = malloc(8);
    *(void**)v168 = (void*)v167;
    v170 = *(void**)v168;
    v171 = 0LL;
    v172 = to_str((void*)v171, v170);
    p_print_str(v172);
    v174 = _str4;
    p_print_str(v174);
    v176 = *(void**)v45;
    v177 = ((union { uint64_t u; double f; }){ .u = 4611686018427387904ULL }).f;
    v178 = 0LL;
    v179 = scale((void*)v178, v176, v177);
    v180 = malloc(8);
    *(void**)v180 = (void*)v179;
    v182 = *(void**)v180;
    v183 = 0LL;
    v184 = to_str((void*)v183, v182);
    p_print_str(v184);
    v186 = _str4;
    p_print_str(v186);
    v188 = *(void**)v45;
    v189 = 0LL;
    v190 = (double)sum((void*)v189, v188);
    p_print_float(v190);
    v192 = _str4;
    p_print_str(v192);
    v194 = *(void**)v45;
    v195 = 0LL;
    v196 = (double)mean((void*)v195, v194);
    p_print_float(v196);
    v198 = _str4;
    p_print_str(v198);
    v200 = *(void**)v45;
    v201 = 0LL;
    v202 = (double)p_max((void*)v201, v200);
    p_print_float(v202);
    v204 = _str4;
    p_print_str(v204);
    v206 = *(void**)v45;
    v207 = 0LL;
    v208 = (double)p_min((void*)v207, v206);
    p_print_float(v208);
    v210 = _str4;
    p_print_str(v210);
    v212 = 0LL;
    v213 = (void*)Vec_new((void*)v212);
    v214 = malloc(8);
    *(void**)v214 = (void*)v213;
    v216 = *(void**)v214;
    v217 = 1LL;
    v218 = -v217;
    v219 = 0LL;
    Vec_push((void*)v219, (int64_t)v216, v218);
    v221 = *(void**)v214;
    v222 = 0LL;
    v223 = 0LL;
    Vec_push((void*)v223, (int64_t)v221, v222);
    v225 = *(void**)v214;
    v226 = 2LL;
    v227 = 0LL;
    Vec_push((void*)v227, (int64_t)v225, v226);
    v229 = *(void**)v214;
    v230 = 3LL;
    v231 = -v230;
    v232 = 0LL;
    Vec_push((void*)v232, (int64_t)v229, v231);
    v234 = *(void**)v214;
    v235 = 4LL;
    v236 = 0LL;
    Vec_push((void*)v236, (int64_t)v234, v235);
    v238 = *(void**)v214;
    v239 = 5LL;
    v240 = -v239;
    v241 = 0LL;
    Vec_push((void*)v241, (int64_t)v238, v240);
    v243 = *(void**)v214;
    v244 = *(void**)v3;
    v245 = 0LL;
    v246 = from_vec((void*)v245, v243, v244);
    v247 = malloc(8);
    *(void**)v247 = (void*)v246;
    v249 = *(void**)v247;
    v250 = 0LL;
    v251 = relu((void*)v250, v249);
    v252 = malloc(8);
    *(void**)v252 = (void*)v251;
    v254 = *(void**)v252;
    v255 = 0LL;
    v256 = to_str((void*)v255, v254);
    p_print_str(v256);
    v258 = _str4;
    p_print_str(v258);
    v260 = *(void**)v45;
    v261 = 0LL;
    v262 = transpose((void*)v261, v260);
    v263 = malloc(8);
    *(void**)v263 = (void*)v262;
    v265 = *(void**)v263;
    v266 = 0LL;
    v267 = to_str((void*)v266, v265);
    p_print_str(v267);
    v269 = _str4;
    p_print_str(v269);
    v271 = 0LL;
    v272 = (void*)Vec_new((void*)v271);
    v273 = malloc(8);
    *(void**)v273 = (void*)v272;
    v275 = *(void**)v273;
    v276 = 1LL;
    v277 = 0LL;
    Vec_push((void*)v277, (int64_t)v275, v276);
    v279 = *(void**)v273;
    v280 = 6LL;
    v281 = 0LL;
    Vec_push((void*)v281, (int64_t)v279, v280);
    v283 = *(void**)v45;
    v284 = *(void**)v273;
    v285 = 0LL;
    v286 = reshape((void*)v285, v283, v284);
    v287 = malloc(8);
    *(void**)v287 = (void*)v286;
    v289 = *(void**)v287;
    v290 = 0LL;
    v291 = to_str((void*)v290, v289);
    p_print_str(v291);
    v293 = _str4;
    p_print_str(v293);
    v295 = *(void**)v45;
    v296 = 0LL;
    v297 = to_json((void*)v296, v295);
    p_print_str(v297);
    v299 = _str4;
    p_print_str(v299);
    v301 = 0LL;
    p_call_stack_pop();
    return v301;
    return 0;
}

void* softmax(void* v0, void* v1) {
    p_call_stack_push("softmax");
    void* v2;
    int64_t v3; /* placeholder for void */
    v2 = p_tensor_softmax(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t Option_expect(void* v0, void* v1, void* v2) {
    p_call_stack_push("Option_expect");
    void* v11;
    bool v6;
    int64_t v12;
    int64_t v16; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v3;
    int64_t v4;
    int64_t v5;
    int64_t v13;
    bool v14;
    int64_t v19;
    int64_t v20; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v8;
    int64_t v15; /* placeholder for void */
    int64_t v9;
    int64_t v18; /* placeholder for void */
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    v19 = 0LL;
    p_call_stack_pop();
    return v19;
  bb2:
    v8 = (char*)v1 + 8;
    v9 = *(int64_t*)v8;
    p_call_stack_pop();
    return v9;
  bb3:
    v11 = (char*)v1 + 0;
    v12 = *(int64_t*)v11;
    v13 = 1LL;
    v14 = v12 == v13;
    if (v14) goto bb4;
    else goto bb5;
  bb4:
    p_panic(v2);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

void allocator_free(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("allocator_free");
    int64_t v3; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    p_allocator_free(v1, v2);
    p_call_stack_pop();
    return;
    return;
}

bool String_contains(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_contains");
    int64_t v94;
    int64_t v50; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    void* v66;
    int64_t v96; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    bool v98;
    int64_t v40;
    int64_t v99; /* placeholder for void */
    bool v108;
    void* v72;
    int64_t v43; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    int64_t v13;
    int64_t v76; /* placeholder for void */
    int64_t v17;
    int64_t v38;
    void* v78;
    int64_t v27;
    int64_t v58;
    bool v24;
    int64_t v51;
    int64_t v55;
    int64_t v21;
    int64_t v10; /* placeholder for void */
    int64_t v47;
    int64_t v65;
    int64_t v19; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    int64_t v73; /* placeholder for void */
    void* v75;
    bool v6;
    void* v18;
    int64_t v26; /* placeholder for void */
    int64_t v95;
    int64_t v91; /* placeholder for void */
    bool v100;
    int64_t v8; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    void* v14;
    bool v42;
    void* v59;
    int64_t v102; /* placeholder for void */
    int64_t v34;
    int64_t v103;
    int64_t v109; /* placeholder for void */
    int64_t v5;
    bool v29;
    bool v53;
    int64_t v56;
    int64_t v20;
    bool v22;
    int64_t v62;
    int64_t v74;
    int64_t v54; /* placeholder for void */
    void* v81;
    bool v44;
    int64_t v84; /* placeholder for void */
    int64_t v105;
    int64_t v107; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v52;
    bool v89;
    int64_t v86;
    bool v4;
    int64_t v39;
    int64_t v85;
    int64_t v11; /* placeholder for void */
    void* v48;
    uint8_t v79;
    int64_t v93;
    int64_t v104;
    int64_t v37; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v16;
    bool v31;
    int64_t v30; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    bool v87;
    void* v45;
    void* v69;
    int64_t v41;
    int64_t v64; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    uint8_t v82;
    int64_t v80; /* placeholder for void */
    int64_t v3;
    int64_t v28;
    int64_t v57;
    bool v7;
    int64_t v49; /* placeholder for void */
    int64_t v61;
    void* v63;
    int64_t v33; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v12;
    void* v35;
    int64_t v67; /* placeholder for void */
    bool v9;
    int64_t v68;
    int64_t v71;
    int64_t v88; /* placeholder for void */
    v3 = 0LL;
    v4 = v1 == v3;
    v5 = 0LL;
    v6 = v2 == v5;
    v7 = v4 || v6;
    if (v7) goto bb1;
    else goto bb2;
  bb1:
    v9 = false;
    p_call_stack_pop();
    return v9;
  bb2:
    goto bb3;
  bb3:
    v12 = 0LL;
    v13 = (int64_t)p_str_length((void*)v12, v1);
    v14 = malloc(8);
    *(int64_t*)v14 = (int64_t)v13;
    v16 = 0LL;
    v17 = (int64_t)p_str_length((void*)v16, v2);
    v18 = malloc(8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = *(int64_t*)v18;
    v21 = 0LL;
    v22 = v20 == v21;
    if (v22) goto bb4;
    else goto bb5;
  bb4:
    v24 = true;
    p_call_stack_pop();
    return v24;
  bb5:
    goto bb6;
  bb6:
    v27 = *(int64_t*)v18;
    v28 = *(int64_t*)v14;
    v29 = v27 > v28;
    if (v29) goto bb7;
    else goto bb8;
  bb7:
    v31 = false;
    p_call_stack_pop();
    return v31;
  bb8:
    goto bb9;
  bb9:
    v34 = 0LL;
    v35 = malloc(8);
    *(int64_t*)v35 = (int64_t)v34;
    goto bb10;
  bb10:
    v38 = *(int64_t*)v35;
    v39 = *(int64_t*)v14;
    v40 = *(int64_t*)v18;
    v41 = v39 - v40;
    v42 = v38 <= v41;
    if (v42) goto bb11;
    else goto bb12;
  bb11:
    v44 = true;
    v45 = malloc(1);
    *(bool*)v45 = v44;
    v47 = 0LL;
    v48 = malloc(8);
    *(int64_t*)v48 = (int64_t)v47;
    goto bb13;
  bb12:
    v108 = false;
    p_call_stack_pop();
    return v108;
  bb13:
    v51 = *(int64_t*)v48;
    v52 = *(int64_t*)v18;
    v53 = v51 < v52;
    if (v53) goto bb14;
    else goto bb15;
  bb14:
    v55 = *(int64_t*)v35;
    v56 = v1 + v55;
    v57 = *(int64_t*)v48;
    v58 = v56 + v57;
    v59 = malloc(8);
    *(int64_t*)v59 = (int64_t)v58;
    v61 = *(int64_t*)v48;
    v62 = v2 + v61;
    v63 = malloc(8);
    *(int64_t*)v63 = (int64_t)v62;
    v65 = *(int64_t*)v59;
    v66 = malloc(8);
    *(void**)v66 = (void*)v65;
    v68 = *(int64_t*)v63;
    v69 = malloc(8);
    *(void**)v69 = (void*)v68;
    v71 = 0LL;
    v72 = malloc(8);
    *(int64_t*)v72 = (int64_t)v71;
    v74 = 0LL;
    v75 = malloc(8);
    *(int64_t*)v75 = (int64_t)v74;
    v78 = *(void**)v66;
    v79 = *(uint8_t*)v78;
    *(int64_t*)v72 = (int64_t)v79;
    v81 = *(void**)v69;
    v82 = *(uint8_t*)v81;
    *(int64_t*)v75 = (int64_t)v82;
    v85 = *(int64_t*)v72;
    v86 = *(int64_t*)v75;
    v87 = v85 != v86;
    if (v87) goto bb16;
    else goto bb17;
  bb15:
    v98 = *(bool*)v45;
    if (v98) goto bb19;
    else goto bb20;
  bb16:
    v89 = false;
    *(bool*)v45 = v89;
    goto bb15;
  bb17:
    goto bb18;
  bb18:
    v93 = *(int64_t*)v48;
    v94 = 1LL;
    v95 = v93 + v94;
    *(int64_t*)v48 = (int64_t)v95;
    goto bb13;
  bb19:
    v100 = true;
    p_call_stack_pop();
    return v100;
  bb20:
    goto bb21;
  bb21:
    v103 = *(int64_t*)v35;
    v104 = 1LL;
    v105 = v103 + v104;
    *(int64_t*)v35 = (int64_t)v105;
    goto bb10;
    return 0;
}

void* add(void* v0, void* v1, void* v2) {
    p_call_stack_push("add");
    int64_t v4; /* placeholder for void */
    void* v3;
    v3 = p_tensor_add(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t HashMap_get(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_get");
    int64_t v97; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    void* v8;
    int64_t v57;
    int64_t v51;
    int64_t v33;
    int64_t v73; /* placeholder for void */
    void* v11;
    void* v41;
    int64_t v23;
    int64_t v69; /* placeholder for void */
    int64_t v19;
    int64_t v94; /* placeholder for void */
    bool v17;
    int64_t v34; /* placeholder for void */
    int64_t v37;
    int64_t v22;
    void* v50;
    int64_t v53; /* placeholder for void */
    int64_t v85;
    int64_t v10; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    void* v68;
    int64_t v7;
    bool v61;
    int64_t v93;
    int64_t v96;
    int64_t v31; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v58;
    int64_t v81;
    int64_t v13; /* placeholder for void */
    void* v86;
    int64_t v95; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    void* v83;
    int64_t v15;
    int64_t v40;
    int64_t v28;
    int64_t v78;
    int64_t v12;
    int64_t v49; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    int64_t v79;
    int64_t v39;
    int64_t v72; /* placeholder for void */
    int64_t v87; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    void* v5;
    void* v24;
    void* v89;
    void* v92;
    int64_t v9; /* placeholder for void */
    int64_t v4;
    int64_t v80;
    void* v71;
    bool v56;
    int64_t v54;
    int64_t v62; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v65; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v75;
    void* v44;
    int64_t v42; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    int64_t v3;
    int64_t v16;
    int64_t v63;
    void* v74;
    int64_t v67;
    int64_t v20; /* placeholder for void */
    void* v26;
    int64_t v27; /* placeholder for void */
    int64_t v38;
    int64_t v66;
    void* v32;
    int64_t v43;
    int64_t v59;
    void* v29;
    int64_t v36;
    int64_t v46;
    int64_t v45; /* placeholder for void */
    void* v47;
    int64_t v55;
    int64_t v48; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    bool v60;
    int64_t v70;
    int64_t v82;
    int64_t v88;
    int64_t v90; /* placeholder for void */
    v3 = 24LL;
    v4 = v1 + v3;
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = 0LL;
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    v11 = *(void**)v5;
    v12 = *(int64_t*)v11;
    *(int64_t*)v8 = (int64_t)v12;
    v15 = *(int64_t*)v8;
    v16 = 0LL;
    v17 = v15 == v16;
    if (v17) goto bb1;
    else goto bb2;
  bb1:
    v19 = 0LL;
    p_call_stack_pop();
    return v19;
  bb2:
    goto bb3;
  bb3:
    v22 = 0LL;
    v23 = (int64_t)HashMap_find_slot((void*)v22, v1, v2);
    v24 = malloc(8);
    *(int64_t*)v24 = (int64_t)v23;
    v26 = malloc(8);
    *(void**)v26 = (void*)v1;
    v28 = 0LL;
    v29 = malloc(8);
    *(int64_t*)v29 = (int64_t)v28;
    v32 = *(void**)v26;
    v33 = *(int64_t*)v32;
    *(int64_t*)v29 = (int64_t)v33;
    v36 = *(int64_t*)v29;
    v37 = *(int64_t*)v24;
    v38 = 8LL;
    v39 = v37 * v38;
    v40 = v36 + v39;
    v41 = malloc(8);
    *(int64_t*)v41 = (int64_t)v40;
    v43 = *(int64_t*)v41;
    v44 = malloc(8);
    *(void**)v44 = (void*)v43;
    v46 = 0LL;
    v47 = malloc(8);
    *(int64_t*)v47 = (int64_t)v46;
    v50 = *(void**)v44;
    v51 = *(int64_t*)v50;
    *(int64_t*)v47 = (int64_t)v51;
    v54 = *(int64_t*)v47;
    v55 = 0LL;
    v56 = v54 == v55;
    v57 = *(int64_t*)v47;
    v58 = 1LL;
    v59 = -v58;
    v60 = v57 == v59;
    v61 = v56 || v60;
    if (v61) goto bb4;
    else goto bb5;
  bb4:
    v63 = 0LL;
    p_call_stack_pop();
    return v63;
  bb5:
    goto bb6;
  bb6:
    v66 = 8LL;
    v67 = v1 + v66;
    v68 = malloc(8);
    *(void**)v68 = (void*)v67;
    v70 = 0LL;
    v71 = malloc(8);
    *(int64_t*)v71 = (int64_t)v70;
    v74 = *(void**)v68;
    v75 = *(int64_t*)v74;
    *(int64_t*)v71 = (int64_t)v75;
    v78 = *(int64_t*)v71;
    v79 = *(int64_t*)v24;
    v80 = 8LL;
    v81 = v79 * v80;
    v82 = v78 + v81;
    v83 = malloc(8);
    *(int64_t*)v83 = (int64_t)v82;
    v85 = *(int64_t*)v83;
    v86 = malloc(8);
    *(void**)v86 = (void*)v85;
    v88 = 0LL;
    v89 = malloc(8);
    *(int64_t*)v89 = (int64_t)v88;
    v92 = *(void**)v86;
    v93 = *(int64_t*)v92;
    *(int64_t*)v89 = (int64_t)v93;
    v96 = *(int64_t*)v89;
    p_call_stack_pop();
    return v96;
    return 0;
}

bool String_is_empty(void* v0, int64_t v1) {
    p_call_stack_push("String_is_empty");
    bool v5;
    int64_t v2;
    void* v8;
    int64_t v4; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    void* v11;
    bool v20;
    bool v3;
    int64_t v12; /* placeholder for void */
    uint8_t v15;
    int64_t v16; /* placeholder for void */
    int64_t v19;
    int64_t v18;
    int64_t v10;
    int64_t v6; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v14;
    int64_t v21; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = true;
    p_call_stack_pop();
    return v5;
  bb2:
    goto bb3;
  bb3:
    v8 = malloc(8);
    *(void**)v8 = (void*)v1;
    v10 = 0LL;
    v11 = malloc(8);
    *(int64_t*)v11 = (int64_t)v10;
    v14 = *(void**)v8;
    v15 = *(uint8_t*)v14;
    *(int64_t*)v11 = (int64_t)v15;
    v18 = *(int64_t*)v11;
    v19 = 0LL;
    v20 = v18 == v19;
    p_call_stack_pop();
    return v20;
    return 0;
}

bool str_starts_with(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_starts_with");
    bool v4;
    int64_t v3;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (bool)String_starts_with((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

bool char_is_alpha(void* v0, int64_t v1) {
    p_call_stack_push("char_is_alpha");
    bool v6;
    bool v3;
    int64_t v2;
    int64_t v4;
    bool v11;
    int64_t v13; /* placeholder for void */
    bool v10;
    bool v8;
    int64_t v7;
    int64_t v9;
    bool v12;
    bool v5;
    v2 = 65LL;
    v3 = v1 >= v2;
    v4 = 90LL;
    v5 = v1 <= v4;
    v6 = v3 && v5;
    v7 = 97LL;
    v8 = v1 >= v7;
    v9 = 122LL;
    v10 = v1 <= v9;
    v11 = v8 && v10;
    v12 = v6 || v11;
    p_call_stack_pop();
    return v12;
    return 0;
}

void* StringBuilder_to_str(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_to_str");
    void* v3;
    void* v2;
    int64_t v4; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = *(void**)v2;
    p_call_stack_pop();
    return v3;
    return 0;
}

bool StringBuilder_is_empty(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_is_empty");
    bool v5;
    void* v2;
    int64_t v4;
    int64_t v3;
    int64_t v6; /* placeholder for void */
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    p_call_stack_pop();
    return v5;
    return 0;
}

int64_t str_compare(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_compare");
    int64_t v4;
    int64_t v3;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)p_str_compare((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

bool ByteBuffer_is_empty(void* v0, void* v1) {
    p_call_stack_push("ByteBuffer_is_empty");
    int64_t v4;
    bool v5;
    int64_t v6; /* placeholder for void */
    int64_t v3;
    void* v2;
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    p_call_stack_pop();
    return v5;
    return 0;
}

int64_t allocator_bytes_used(void* v0, void* v1) {
    p_call_stack_push("allocator_bytes_used");
    int64_t v2;
    int64_t v3; /* placeholder for void */
    v2 = (int64_t)p_allocator_stats(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void StringBuilder_clear(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_clear");
    void* v2;
    void* v5;
    void* v3;
    int64_t v4; /* placeholder for void */
    int64_t v6;
    int64_t v7; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = _str8;
    *(void**)v2 = (void*)v3;
    v5 = (char*)v1 + 8;
    v6 = 0LL;
    *(int64_t*)v5 = (int64_t)v6;
    p_call_stack_pop();
    return;
    return;
}

int64_t Vec_first(void* v0, int64_t v1) {
    p_call_stack_push("Vec_first");
    int64_t v33; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v2;
    void* v24;
    int64_t v36;
    int64_t v30; /* placeholder for void */
    void* v4;
    int64_t v21; /* placeholder for void */
    void* v35;
    int64_t v17; /* placeholder for void */
    void* v18;
    int64_t v27; /* placeholder for void */
    int64_t v3;
    int64_t v9; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    void* v29;
    int64_t v34; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v25;
    int64_t v14;
    bool v16;
    int64_t v37; /* placeholder for void */
    int64_t v39;
    int64_t v12; /* placeholder for void */
    void* v22;
    void* v8;
    int64_t v23; /* placeholder for void */
    int64_t v31;
    int64_t v5; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    void* v32;
    int64_t v7; /* placeholder for void */
    int64_t v15;
    int64_t v11;
    void* v6;
    void* v10;
    int64_t v28;
    v2 = 8LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = malloc(8);
    *(void**)v6 = (void*)v1;
    v8 = malloc(8);
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v8 = (int64_t)v11;
    v14 = *(int64_t*)v8;
    v15 = 0LL;
    v16 = v14 == v15;
    if (v16) goto bb1;
    else goto bb2;
  bb1:
    v18 = _str12;
    p_panic(v18);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v22 = malloc(8);
    v24 = *(void**)v6;
    v25 = *(int64_t*)v24;
    *(int64_t*)v22 = (int64_t)v25;
    v28 = *(int64_t*)v22;
    v29 = malloc(8);
    *(void**)v29 = (void*)v28;
    v31 = 0LL;
    v32 = malloc(8);
    *(int64_t*)v32 = (int64_t)v31;
    v35 = *(void**)v29;
    v36 = *(int64_t*)v35;
    *(int64_t*)v32 = (int64_t)v36;
    v39 = *(int64_t*)v32;
    p_call_stack_pop();
    return v39;
    return 0;
}

int64_t Vec_clone(void* v0, int64_t v1) {
    p_call_stack_push("Vec_clone");
    int64_t v18; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v29;
    int64_t v7; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v33;
    void* v46;
    int64_t v44; /* placeholder for void */
    void* v25;
    bool v30;
    int64_t v17;
    void* v4;
    void* v16;
    int64_t v38; /* placeholder for void */
    void* v6;
    int64_t v3;
    void* v8;
    int64_t v13; /* placeholder for void */
    void* v37;
    int64_t v45; /* placeholder for void */
    int64_t v36;
    void* v40;
    int64_t v60; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    void* v22;
    int64_t v56;
    void* v10;
    int64_t v21;
    int64_t v57; /* placeholder for void */
    int64_t v32;
    int64_t v9; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v51;
    int64_t v11;
    int64_t v52;
    int64_t v55;
    int64_t v2;
    int64_t v58; /* placeholder for void */
    int64_t v35;
    void* v14;
    void* v43;
    int64_t v15; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v28;
    int64_t v34;
    int64_t v24;
    int64_t v42;
    int64_t v39;
    int64_t v50;
    int64_t v41; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v20;
    int64_t v59;
    int64_t v47;
    int64_t v54;
    v2 = 8LL;
    v3 = v1 + v2;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = malloc(8);
    *(void**)v6 = (void*)v1;
    v8 = malloc(8);
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v8 = (int64_t)v11;
    v14 = malloc(8);
    v16 = *(void**)v6;
    v17 = *(int64_t*)v16;
    *(int64_t*)v14 = (int64_t)v17;
    v20 = 0LL;
    v21 = (int64_t)Vec_new((void*)v20);
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v21;
    v24 = 0LL;
    v25 = malloc(8);
    *(int64_t*)v25 = (int64_t)v24;
    goto bb1;
  bb1:
    v28 = *(int64_t*)v25;
    v29 = *(int64_t*)v8;
    v30 = v28 < v29;
    if (v30) goto bb2;
    else goto bb3;
  bb2:
    v32 = *(int64_t*)v14;
    v33 = *(int64_t*)v25;
    v34 = 8LL;
    v35 = v33 * v34;
    v36 = v32 + v35;
    v37 = malloc(8);
    *(int64_t*)v37 = (int64_t)v36;
    v39 = *(int64_t*)v37;
    v40 = malloc(8);
    *(void**)v40 = (void*)v39;
    v42 = 0LL;
    v43 = malloc(8);
    *(int64_t*)v43 = (int64_t)v42;
    v46 = *(void**)v40;
    v47 = *(int64_t*)v46;
    *(int64_t*)v43 = (int64_t)v47;
    v50 = *(int64_t*)v22;
    v51 = *(int64_t*)v43;
    v52 = 0LL;
    Vec_push((void*)v52, v50, v51);
    v54 = *(int64_t*)v25;
    v55 = 1LL;
    v56 = v54 + v55;
    *(int64_t*)v25 = (int64_t)v56;
    goto bb1;
  bb3:
    v59 = *(int64_t*)v22;
    p_call_stack_pop();
    return v59;
    return 0;
}

void* zeros(void* v0, void* v1) {
    p_call_stack_push("zeros");
    int64_t v3; /* placeholder for void */
    void* v2;
    v2 = p_tensor_zeros(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void* matmul(void* v0, void* v1, void* v2) {
    p_call_stack_push("matmul");
    void* v3;
    int64_t v4; /* placeholder for void */
    v3 = p_tensor_matmul(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t p_str_hash(void* v0, int64_t v1) {
    p_call_stack_push("p_str_hash");
    void* v9;
    int64_t v32; /* placeholder for void */
    bool v42;
    uint8_t v36;
    int64_t v41;
    void* v6;
    void* v4;
    int64_t v7; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    bool v19;
    void* v35;
    uint8_t v13;
    int64_t v23;
    int64_t v11; /* placeholder for void */
    void* v14;
    int64_t v24; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v22;
    int64_t v48;
    int64_t v40;
    int64_t v8;
    int64_t v2;
    int64_t v10; /* placeholder for void */
    int64_t v17;
    int64_t v28; /* placeholder for void */
    int64_t v33;
    int64_t v39; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v44;
    int64_t v25;
    void* v12;
    int64_t v3;
    int64_t v31;
    int64_t v46; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v21;
    int64_t v29;
    int64_t v5; /* placeholder for void */
    int64_t v45;
    int64_t v18;
    int64_t v26;
    int64_t v30;
    int64_t v38; /* placeholder for void */
    int64_t v27;
    int64_t v47; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    v2 = 3750763034362895579LL;
    v3 = -v2;
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = malloc(8);
    *(int64_t*)v6 = (int64_t)v1;
    v8 = *(int64_t*)v6;
    v9 = malloc(8);
    *(void**)v9 = (void*)v8;
    v12 = *(void**)v9;
    v13 = *(uint8_t*)v12;
    v14 = malloc(8);
    *(int64_t*)v14 = (int64_t)v13;
    goto bb1;
  bb1:
    v17 = *(int64_t*)v14;
    v18 = 0LL;
    v19 = v17 != v18;
    if (v19) goto bb2;
    else goto bb3;
  bb2:
    v21 = *(int64_t*)v4;
    v22 = *(int64_t*)v14;
    v23 = v21 ^ v22;
    *(int64_t*)v4 = (int64_t)v23;
    v25 = *(int64_t*)v4;
    v26 = 1099511628211LL;
    v27 = v25 * v26;
    *(int64_t*)v4 = (int64_t)v27;
    v29 = *(int64_t*)v6;
    v30 = 1LL;
    v31 = v29 + v30;
    *(int64_t*)v6 = (int64_t)v31;
    v33 = *(int64_t*)v6;
    *(void**)v9 = (void*)v33;
    v35 = *(void**)v9;
    v36 = *(uint8_t*)v35;
    *(int64_t*)v14 = (int64_t)v36;
    goto bb1;
  bb3:
    v40 = *(int64_t*)v4;
    v41 = 0LL;
    v42 = v40 < v41;
    if (v42) goto bb4;
    else goto bb5;
  bb4:
    v44 = *(int64_t*)v4;
    v45 = -v44;
    p_call_stack_pop();
    return v45;
  bb5:
    goto bb6;
  bb6:
    v48 = *(int64_t*)v4;
    p_call_stack_pop();
    return v48;
    return 0;
}

int64_t String_from(void* v0, int64_t v1) {
    p_call_stack_push("String_from");
    void* v28;
    int64_t v34;
    void* v37;
    void* v43;
    int64_t v18; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    void* v6;
    int64_t v5;
    int64_t v8; /* placeholder for void */
    int64_t v9;
    int64_t v21;
    void* v27;
    int64_t v32;
    int64_t v48; /* placeholder for void */
    int64_t v17;
    int64_t v15; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v24;
    int64_t v41; /* placeholder for void */
    int64_t v44;
    void* v10;
    bool v3;
    int64_t v29; /* placeholder for void */
    int64_t v31;
    int64_t v20;
    int64_t v19; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v30;
    int64_t v35;
    int64_t v2;
    int64_t v46; /* placeholder for void */
    void* v7;
    int64_t v4; /* placeholder for void */
    int64_t v36;
    int64_t v42; /* placeholder for void */
    void* v13;
    int64_t v14;
    void* v40;
    int64_t v23; /* placeholder for void */
    int64_t v47;
    void* v22;
    int64_t v38; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v26;
    int64_t v39;
    int64_t v25;
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 1LL;
    v6 = p_malloc(v5);
    v7 = malloc(8);
    *(int64_t*)v7 = (int64_t)v6;
    v9 = *(int64_t*)v7;
    v10 = malloc(8);
    *(void**)v10 = (void*)v9;
    v13 = *(void**)v10;
    v14 = 0LL;
    *(uint8_t*)v13 = (uint8_t)v14;
    v17 = *(int64_t*)v7;
    p_call_stack_pop();
    return v17;
  bb2:
    goto bb3;
  bb3:
    v20 = 0LL;
    v21 = (int64_t)p_str_length((void*)v20, v1);
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v21;
    v24 = *(int64_t*)v22;
    v25 = 1LL;
    v26 = v24 + v25;
    v27 = p_malloc(v26);
    v28 = malloc(8);
    *(int64_t*)v28 = (int64_t)v27;
    v30 = *(int64_t*)v28;
    v31 = *(int64_t*)v22;
    v32 = 0LL;
    p_str_copy_n((void*)v32, v30, v1, v31);
    v34 = *(int64_t*)v28;
    v35 = *(int64_t*)v22;
    v36 = v34 + v35;
    v37 = malloc(8);
    *(int64_t*)v37 = (int64_t)v36;
    v39 = *(int64_t*)v37;
    v40 = malloc(8);
    *(void**)v40 = (void*)v39;
    v43 = *(void**)v40;
    v44 = 0LL;
    *(uint8_t*)v43 = (uint8_t)v44;
    v47 = *(int64_t*)v28;
    p_call_stack_pop();
    return v47;
    return 0;
}

void* str_repeat(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("str_repeat");
    int64_t v5; /* placeholder for void */
    int64_t v3;
    int64_t v4;
    v3 = 0LL;
    v4 = (int64_t)String_repeat((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return (void*)v4;
    return 0;
}

void Result_and_then(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Result_and_then");
    int64_t v39; /* placeholder for void */
    int64_t v42;
    void* v44;
    int64_t v46;
    void* v16;
    int64_t v26; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    void* v22;
    int64_t v17; /* placeholder for void */
    int64_t v15;
    int64_t v49; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    void* v4;
    void* v13;
    int64_t v47; /* placeholder for void */
    int64_t v5;
    void* v11;
    void* v23;
    void* v32;
    void* v19;
    int64_t v10;
    int64_t v29;
    int64_t v28;
    int64_t v6;
    int64_t v37; /* placeholder for void */
    int64_t v12;
    void* v38;
    int64_t v25; /* placeholder for void */
    void* v40;
    int64_t v50; /* placeholder for void */
    int64_t v33;
    void* v18;
    int64_t v8; /* placeholder for void */
    void* v9;
    int64_t v35;
    int64_t v21; /* placeholder for void */
    void* v34;
    void* v36;
    bool v7;
    int64_t v24;
    int64_t v43; /* placeholder for void */
    void* v45;
    int64_t v20;
    void* v41;
    void* v27;
    bool v30;
    v4 = (char*)v2 + 0;
    v5 = *(int64_t*)v4;
    v6 = 0LL;
    v7 = v5 == v6;
    if (v7) goto bb2;
    else goto bb3;
  bb1:
    p_call_stack_pop();
    return;
  bb2:
    v9 = (char*)v2 + 8;
    v10 = *(int64_t*)v9;
    v11 = malloc(16);
    v12 = 0LL;
    v13 = (char*)v11 + 0;
    *(int64_t*)v13 = (int64_t)v12;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ndim") == 0) {
            v15 = ndim((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "size") == 0) {
            v15 = size((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (int64_t)v10);
        } else { abort(); }
    }
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
    p_call_stack_pop();
    return;
  bb3:
    v27 = (char*)v2 + 0;
    v28 = *(int64_t*)v27;
    v29 = 1LL;
    v30 = v28 == v29;
    if (v30) goto bb4;
    else goto bb5;
  bb4:
    v32 = (char*)v2 + 8;
    v33 = *(int64_t*)v32;
    v34 = malloc(16);
    v35 = 1LL;
    v36 = (char*)v34 + 0;
    *(int64_t*)v36 = (int64_t)v35;
    v38 = (char*)v34 + 8;
    *(int64_t*)v38 = (int64_t)v33;
    v40 = (char*)v34 + 0;
    v41 = (char*)v0 + 0;
    v42 = *(int64_t*)v40;
    *(int64_t*)v41 = (int64_t)v42;
    v44 = (char*)v34 + 8;
    v45 = (char*)v0 + 8;
    v46 = *(int64_t*)v44;
    *(int64_t*)v45 = (int64_t)v46;
    p_call_stack_pop();
    return;
  bb5:
    goto bb1;
    return;
}

void* str_concat(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_concat");
    int64_t v4; /* placeholder for void */
    void* v3;
    v3 = p_str_concat(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

void* str_trim(void* v0, void* v1) {
    p_call_stack_push("str_trim");
    int64_t v3;
    int64_t v2;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_trim((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return (void*)v3;
    return 0;
}

int64_t Result_unwrap_or_else(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_unwrap_or_else");
    int64_t v10; /* placeholder for void */
    int64_t v13;
    bool v14;
    int64_t v22; /* placeholder for void */
    int64_t v9;
    void* v3;
    void* v8;
    int64_t v17;
    int64_t v18;
    int64_t v19; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v21;
    int64_t v7; /* placeholder for void */
    void* v11;
    void* v16;
    int64_t v20; /* placeholder for void */
    int64_t v12;
    bool v6;
    int64_t v4;
    int64_t v5;
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    v21 = 0LL;
    p_call_stack_pop();
    return v21;
  bb2:
    v8 = (char*)v1 + 8;
    v9 = *(int64_t*)v8;
    p_call_stack_pop();
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
    {
        if (!v2) abort();
        char* _fn_name = *(char**)v2;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "Vec_new") == 0) {
            v18 = Vec_new((void*)v2);
        } else if (strcmp(_fn_name, "HashMap_new") == 0) {
            v18 = HashMap_new((void*)v2);
        } else { abort(); }
    }
    p_call_stack_pop();
    return v18;
  bb5:
    goto bb1;
    return 0;
}

void Vec_extend(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("Vec_extend");
    void* v40;
    int64_t v44;
    int64_t v48;
    int64_t v50;
    int64_t v3;
    int64_t v18;
    void* v5;
    int64_t v13; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v29;
    void* v11;
    void* v9;
    void* v34;
    int64_t v26;
    int64_t v16; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    int64_t v31;
    int64_t v24; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    void* v15;
    int64_t v4;
    int64_t v35; /* placeholder for void */
    int64_t v36;
    int64_t v30;
    int64_t v39;
    int64_t v41; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v52;
    int64_t v54; /* placeholder for void */
    int64_t v25;
    void* v43;
    int64_t v10; /* placeholder for void */
    void* v7;
    int64_t v33;
    void* v37;
    int64_t v47;
    int64_t v49; /* placeholder for void */
    int64_t v21;
    bool v27;
    int64_t v19; /* placeholder for void */
    int64_t v51;
    int64_t v23; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    int64_t v32;
    void* v22;
    int64_t v14; /* placeholder for void */
    int64_t v12;
    int64_t v45; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v17;
    int64_t v20; /* placeholder for void */
    v3 = 8LL;
    v4 = v2 + v3;
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = malloc(8);
    *(void**)v7 = (void*)v2;
    v9 = malloc(8);
    v11 = *(void**)v5;
    v12 = *(int64_t*)v11;
    *(int64_t*)v9 = (int64_t)v12;
    v15 = malloc(8);
    v17 = *(void**)v7;
    v18 = *(int64_t*)v17;
    *(int64_t*)v15 = (int64_t)v18;
    v21 = 0LL;
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v21;
    goto bb1;
  bb1:
    v25 = *(int64_t*)v22;
    v26 = *(int64_t*)v9;
    v27 = v25 < v26;
    if (v27) goto bb2;
    else goto bb3;
  bb2:
    v29 = *(int64_t*)v15;
    v30 = *(int64_t*)v22;
    v31 = 8LL;
    v32 = v30 * v31;
    v33 = v29 + v32;
    v34 = malloc(8);
    *(int64_t*)v34 = (int64_t)v33;
    v36 = *(int64_t*)v34;
    v37 = malloc(8);
    *(void**)v37 = (void*)v36;
    v39 = 0LL;
    v40 = malloc(8);
    *(int64_t*)v40 = (int64_t)v39;
    v43 = *(void**)v37;
    v44 = *(int64_t*)v43;
    *(int64_t*)v40 = (int64_t)v44;
    v47 = *(int64_t*)v40;
    v48 = 0LL;
    Vec_push((void*)v48, v1, v47);
    v50 = *(int64_t*)v22;
    v51 = 1LL;
    v52 = v50 + v51;
    *(int64_t*)v22 = (int64_t)v52;
    goto bb1;
  bb3:
    p_call_stack_pop();
    return;
    return;
}

void ByteBuffer_from_string(void* v0, void* v1, void* v2) {
    p_call_stack_push("ByteBuffer_from_string");
    int64_t v54; /* placeholder for void */
    void* v14;
    int64_t v21;
    int64_t v42;
    void* v23;
    int64_t v6; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v13;
    int64_t v25;
    int64_t v5;
    int64_t v34; /* placeholder for void */
    void* v46;
    void* v50;
    void* v51;
    int64_t v52;
    int64_t v49; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v38;
    void* v7;
    int64_t v31; /* placeholder for void */
    void* v36;
    void* v47;
    int64_t v18;
    int64_t v39; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    uint8_t v33;
    int64_t v40;
    void* v4;
    int64_t v53; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v10;
    int64_t v20; /* placeholder for void */
    int64_t v17;
    int64_t v9;
    int64_t v22;
    void* v26;
    int64_t v3;
    void* v11;
    int64_t v37;
    void* v32;
    int64_t v41;
    int64_t v43; /* placeholder for void */
    bool v19;
    void* v48;
    int64_t v12; /* placeholder for void */
    void* v29;
    int64_t v44; /* placeholder for void */
    void* v45;
    int64_t v28;
    v3 = 0LL;
    v4 = malloc(16);
    v5 = 0LL;
    ByteBuffer_new(v4, (void*)v5, v3);
    v7 = malloc(8);
    *(void**)v7 = (void*)v4;
    v9 = 0LL;
    v10 = (int64_t)String_len((void*)v9, (int64_t)v2);
    v11 = malloc(8);
    *(int64_t*)v11 = (int64_t)v10;
    v13 = 0LL;
    v14 = malloc(8);
    *(int64_t*)v14 = (int64_t)v13;
    goto bb1;
  bb1:
    v17 = *(int64_t*)v14;
    v18 = *(int64_t*)v11;
    v19 = v17 < v18;
    if (v19) goto bb2;
    else goto bb3;
  bb2:
    v21 = *(int64_t*)v14;
    v22 = (int64_t)v2 + v21;
    v23 = malloc(8);
    *(int64_t*)v23 = (int64_t)v22;
    v25 = *(int64_t*)v23;
    v26 = malloc(8);
    *(void**)v26 = (void*)v25;
    v28 = 0LL;
    v29 = malloc(8);
    *(int64_t*)v29 = (int64_t)v28;
    v32 = *(void**)v26;
    v33 = *(uint8_t*)v32;
    *(int64_t*)v29 = (int64_t)v33;
    v36 = *(void**)v7;
    v37 = *(int64_t*)v29;
    v38 = 0LL;
    ByteBuffer_write_byte((void*)v38, v36, v37);
    v40 = *(int64_t*)v14;
    v41 = 1LL;
    v42 = v40 + v41;
    *(int64_t*)v14 = (int64_t)v42;
    goto bb1;
  bb3:
    v45 = *(void**)v7;
    v46 = (char*)v45 + 0;
    v47 = (char*)v0 + 0;
    v48 = *(void**)v46;
    *(void**)v47 = (void*)v48;
    v50 = (char*)v45 + 8;
    v51 = (char*)v0 + 8;
    v52 = *(int64_t*)v50;
    *(int64_t*)v51 = (int64_t)v52;
    p_call_stack_pop();
    return;
    return;
}

void* str_from_bytes(void* v0, void* v1) {
    p_call_stack_push("str_from_bytes");
    void* v10;
    void* v22;
    int64_t v11; /* placeholder for void */
    void* v2;
    int64_t v12; /* placeholder for void */
    int64_t v14;
    void* v21;
    int64_t v23; /* placeholder for void */
    void* v29;
    int64_t v4; /* placeholder for void */
    void* v6;
    int64_t v26;
    int64_t v7; /* placeholder for void */
    int64_t v19;
    int64_t v5;
    void* v3;
    int64_t v18;
    int64_t v16; /* placeholder for void */
    int64_t v8;
    int64_t v25;
    void* v17;
    int64_t v20;
    bool v15;
    int64_t v24;
    int64_t v27; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v13;
    int64_t v28; /* placeholder for void */
    int64_t v9;
    v2 = _str8;
    v3 = malloc(8);
    *(void**)v3 = (void*)v2;
    v5 = 0LL;
    v6 = malloc(8);
    *(int64_t*)v6 = (int64_t)v5;
    v8 = 0LL;
    v9 = (int64_t)Vec_len((void*)v8, (int64_t)v1);
    v10 = malloc(8);
    *(int64_t*)v10 = (int64_t)v9;
    goto bb1;
  bb1:
    v13 = *(int64_t*)v6;
    v14 = *(int64_t*)v10;
    v15 = v13 < v14;
    if (v15) goto bb2;
    else goto bb3;
  bb2:
    v17 = *(void**)v3;
    v18 = *(int64_t*)v6;
    v19 = 0LL;
    v20 = (int64_t)Vec_get((void*)v19, (int64_t)v1, v18);
    v21 = p_char_to_str(v20);
    v22 = p_str_concat(v17, v21);
    *(void**)v3 = (void*)v22;
    v24 = *(int64_t*)v6;
    v25 = 1LL;
    v26 = v24 + v25;
    *(int64_t*)v6 = (int64_t)v26;
    goto bb1;
  bb3:
    v29 = *(void**)v3;
    p_call_stack_pop();
    return v29;
    return 0;
}

void ByteBuffer_set_byte(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("ByteBuffer_set_byte");
    bool v5;
    void* v14;
    int64_t v15;
    int64_t v16; /* placeholder for void */
    void* v13;
    bool v9;
    int64_t v7;
    int64_t v10; /* placeholder for void */
    int64_t v4;
    void* v6;
    bool v8;
    int64_t v12; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    v4 = 0LL;
    v5 = v2 < v4;
    v6 = (char*)v1 + 8;
    v7 = *(int64_t*)v6;
    v8 = v2 >= v7;
    v9 = v5 || v8;
    if (v9) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return;
  bb2:
    goto bb3;
  bb3:
    v13 = (char*)v1 + 0;
    v14 = *(void**)v13;
    v15 = 0LL;
    Vec_set((void*)v15, (int64_t)v14, v2, v3);
    p_call_stack_pop();
    return;
    return;
}

void* str_trim_start(void* v0, void* v1) {
    p_call_stack_push("str_trim_start");
    int64_t v34; /* placeholder for void */
    int64_t v14;
    int64_t v39; /* placeholder for void */
    int64_t v3;
    int64_t v23; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v2;
    void* v7;
    int64_t v8; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    void* v22;
    void* v25;
    int64_t v27; /* placeholder for void */
    int64_t v6;
    int64_t v28; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v30;
    int64_t v35; /* placeholder for void */
    bool v43;
    void* v45;
    int64_t v41;
    int64_t v47; /* placeholder for void */
    int64_t v50;
    int64_t v51;
    bool v12;
    int64_t v52; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v49;
    int64_t v24; /* placeholder for void */
    int64_t v48;
    int64_t v44; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v29;
    uint8_t v26;
    int64_t v36;
    int64_t v38;
    int64_t v10;
    int64_t v15;
    void* v16;
    int64_t v21;
    void* v19;
    bool v31;
    int64_t v32;
    int64_t v33; /* placeholder for void */
    void* v4;
    int64_t v18;
    int64_t v11;
    int64_t v37;
    int64_t v40; /* placeholder for void */
    int64_t v42;
    v2 = 0LL;
    v3 = (int64_t)String_len((void*)v2, (int64_t)v1);
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = 0LL;
    v7 = malloc(8);
    *(int64_t*)v7 = (int64_t)v6;
    goto bb1;
  bb1:
    v10 = *(int64_t*)v7;
    v11 = *(int64_t*)v4;
    v12 = v10 < v11;
    if (v12) goto bb2;
    else goto bb3;
  bb2:
    v14 = *(int64_t*)v7;
    v15 = (int64_t)v1 + v14;
    v16 = malloc(8);
    *(int64_t*)v16 = (int64_t)v15;
    v18 = *(int64_t*)v16;
    v19 = malloc(8);
    *(void**)v19 = (void*)v18;
    v21 = 0LL;
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v21;
    v25 = *(void**)v19;
    v26 = *(uint8_t*)v25;
    *(int64_t*)v22 = (int64_t)v26;
    v29 = *(int64_t*)v22;
    v30 = 0LL;
    v31 = (bool)char_is_whitespace((void*)v30, v29);
    v32 = !v31;
    if (v32) goto bb4;
    else goto bb5;
  bb3:
    v41 = *(int64_t*)v7;
    v42 = *(int64_t*)v4;
    v43 = v41 >= v42;
    if (v43) goto bb7;
    else goto bb8;
  bb4:
    goto bb3;
  bb5:
    goto bb6;
  bb6:
    v36 = *(int64_t*)v7;
    v37 = 1LL;
    v38 = v36 + v37;
    *(int64_t*)v7 = (int64_t)v38;
    goto bb1;
  bb7:
    v45 = _str8;
    p_call_stack_pop();
    return v45;
  bb8:
    goto bb9;
  bb9:
    v48 = *(int64_t*)v7;
    v49 = *(int64_t*)v4;
    v50 = 0LL;
    v51 = (int64_t)String_slice((void*)v50, (int64_t)v1, v48, v49);
    p_call_stack_pop();
    return (void*)v51;
    return 0;
}

void Result_map_err(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Result_map_err");
    int64_t v14; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    void* v41;
    int64_t v5;
    void* v9;
    int64_t v43; /* placeholder for void */
    void* v4;
    int64_t v12;
    bool v7;
    void* v18;
    void* v44;
    void* v45;
    int64_t v47; /* placeholder for void */
    int64_t v46;
    int64_t v6;
    int64_t v32;
    int64_t v25; /* placeholder for void */
    void* v21;
    void* v13;
    int64_t v42;
    void* v17;
    int64_t v19;
    void* v26;
    void* v11;
    int64_t v20; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v23;
    bool v29;
    int64_t v30; /* placeholder for void */
    int64_t v34;
    int64_t v37;
    int64_t v24; /* placeholder for void */
    void* v38;
    int64_t v36; /* placeholder for void */
    void* v22;
    int64_t v27;
    int64_t v39; /* placeholder for void */
    int64_t v28;
    void* v40;
    int64_t v48; /* placeholder for void */
    int64_t v10;
    void* v15;
    void* v31;
    void* v33;
    void* v35;
    v4 = (char*)v2 + 0;
    v5 = *(int64_t*)v4;
    v6 = 0LL;
    v7 = v5 == v6;
    if (v7) goto bb2;
    else goto bb3;
  bb1:
    p_call_stack_pop();
    return;
  bb2:
    v9 = (char*)v2 + 8;
    v10 = *(int64_t*)v9;
    v11 = malloc(16);
    v12 = 0LL;
    v13 = (char*)v11 + 0;
    *(int64_t*)v13 = (int64_t)v12;
    v15 = (char*)v11 + 8;
    *(int64_t*)v15 = (int64_t)v10;
    v17 = (char*)v11 + 0;
    v18 = (char*)v0 + 0;
    v19 = *(int64_t*)v17;
    *(int64_t*)v18 = (int64_t)v19;
    v21 = (char*)v11 + 8;
    v22 = (char*)v0 + 8;
    v23 = *(int64_t*)v21;
    *(int64_t*)v22 = (int64_t)v23;
    p_call_stack_pop();
    return;
  bb3:
    v26 = (char*)v2 + 0;
    v27 = *(int64_t*)v26;
    v28 = 1LL;
    v29 = v27 == v28;
    if (v29) goto bb4;
    else goto bb5;
  bb4:
    v31 = (char*)v2 + 8;
    v32 = *(int64_t*)v31;
    v33 = malloc(16);
    v34 = 1LL;
    v35 = (char*)v33 + 0;
    *(int64_t*)v35 = (int64_t)v34;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "Vec_first") == 0) {
            v37 = Vec_first((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v37 = str_parse_int((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "ndim") == 0) {
            v37 = ndim((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v37 = HashMap_keys((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v37 = StringBuilder_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v37 = p_str_hash((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v37 = char_to_upper((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v37 = String_to_uppercase((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v37 = Vec_capacity((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v37 = String_to_lowercase((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v37 = ByteBuffer_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v37 = Option_unwrap((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v37 = char_to_lower((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v37 = Result_unwrap((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v37 = String_from((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v37 = HashMap_len((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v37 = String_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v37 = Vec_pop((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v37 = Vec_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v37 = String_trim((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v37 = String_bytes((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v37 = String_char_count((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v37 = String_parse_int((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v37 = HashMap_values((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v37 = str_parse_hex((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "size") == 0) {
            v37 = size((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v37 = Vec_len((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v37 = HashMap_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v37 = String_len((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v37 = p_abs((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v37 = allocator_bytes_used((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v37 = p_str_length((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v37 = str_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v37 = String_to_string((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v37 = Vec_last((void*)v3, (int64_t)v32);
        } else { abort(); }
    }
    v38 = (char*)v33 + 8;
    *(int64_t*)v38 = (int64_t)v37;
    v40 = (char*)v33 + 0;
    v41 = (char*)v0 + 0;
    v42 = *(int64_t*)v40;
    *(int64_t*)v41 = (int64_t)v42;
    v44 = (char*)v33 + 8;
    v45 = (char*)v0 + 8;
    v46 = *(int64_t*)v44;
    *(int64_t*)v45 = (int64_t)v46;
    p_call_stack_pop();
    return;
  bb5:
    goto bb1;
    return;
}

void* str_split(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_split");
    int64_t v100;
    int64_t v11; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v8;
    int64_t v86; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    int64_t v36;
    void* v115;
    int64_t v78; /* placeholder for void */
    int64_t v99;
    int64_t v3;
    int64_t v88;
    int64_t v105; /* placeholder for void */
    void* v12;
    int64_t v75; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v89;
    int64_t v46;
    int64_t v47;
    int64_t v116;
    void* v64;
    bool v10;
    int64_t v102;
    int64_t v7;
    int64_t v34;
    int64_t v56;
    int64_t v29;
    int64_t v90;
    int64_t v96;
    int64_t v38; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v97;
    int64_t v14; /* placeholder for void */
    void* v76;
    void* v19;
    int64_t v62; /* placeholder for void */
    bool v82;
    int64_t v65; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v60;
    bool v84;
    void* v61;
    int64_t v71; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v106;
    int64_t v121; /* placeholder for void */
    int64_t v42;
    void* v122;
    int64_t v53;
    int64_t v32; /* placeholder for void */
    int64_t v81;
    int64_t v68; /* placeholder for void */
    bool v48;
    int64_t v113; /* placeholder for void */
    void* v27;
    int64_t v80;
    int64_t v83; /* placeholder for void */
    void* v5;
    int64_t v57;
    bool v93;
    void* v70;
    bool v39;
    void* v43;
    int64_t v16; /* placeholder for void */
    int64_t v114; /* placeholder for void */
    int64_t v13;
    int64_t v98;
    int64_t v22;
    int64_t v26;
    int64_t v117;
    void* v23;
    bool v37;
    int64_t v87; /* placeholder for void */
    void* v4;
    int64_t v49; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    int64_t v108; /* placeholder for void */
    int64_t v25;
    int64_t v44; /* placeholder for void */
    int64_t v66;
    int64_t v69;
    uint8_t v77;
    int64_t v110;
    int64_t v94; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    void* v30;
    int64_t v63;
    void* v40;
    int64_t v51;
    int64_t v85; /* placeholder for void */
    int64_t v123; /* placeholder for void */
    uint8_t v74;
    int64_t v52;
    int64_t v101; /* placeholder for void */
    int64_t v104;
    int64_t v35;
    int64_t v72; /* placeholder for void */
    int64_t v107; /* placeholder for void */
    int64_t v109;
    int64_t v118;
    int64_t v120;
    void* v15;
    void* v58;
    int64_t v119;
    void* v95;
    int64_t v112; /* placeholder for void */
    int64_t v33;
    int64_t v50;
    int64_t v111;
    int64_t v9;
    void* v73;
    int64_t v21;
    void* v67;
    int64_t v103;
    int64_t v18;
    void* v54;
    v3 = 0LL;
    v4 = (void*)Vec_new((void*)v3);
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = 0LL;
    v8 = (int64_t)String_len((void*)v7, (int64_t)v2);
    v9 = 0LL;
    v10 = v8 == v9;
    if (v10) goto bb1;
    else goto bb2;
  bb1:
    v12 = *(void**)v5;
    v13 = 0LL;
    Vec_push((void*)v13, (int64_t)v12, (int64_t)v1);
    v15 = *(void**)v5;
    p_call_stack_pop();
    return v15;
  bb2:
    goto bb3;
  bb3:
    v18 = 0LL;
    v19 = malloc(8);
    *(int64_t*)v19 = (int64_t)v18;
    v21 = 0LL;
    v22 = (int64_t)String_len((void*)v21, (int64_t)v1);
    v23 = malloc(8);
    *(int64_t*)v23 = (int64_t)v22;
    v25 = 0LL;
    v26 = (int64_t)String_len((void*)v25, (int64_t)v2);
    v27 = malloc(8);
    *(int64_t*)v27 = (int64_t)v26;
    v29 = 0LL;
    v30 = malloc(8);
    *(int64_t*)v30 = (int64_t)v29;
    goto bb4;
  bb4:
    v33 = *(int64_t*)v30;
    v34 = *(int64_t*)v23;
    v35 = *(int64_t*)v27;
    v36 = v34 - v35;
    v37 = v33 <= v36;
    if (v37) goto bb5;
    else goto bb6;
  bb5:
    v39 = true;
    v40 = malloc(1);
    *(bool*)v40 = v39;
    v42 = 0LL;
    v43 = malloc(8);
    *(int64_t*)v43 = (int64_t)v42;
    goto bb7;
  bb6:
    v115 = *(void**)v5;
    v116 = *(int64_t*)v19;
    v117 = *(int64_t*)v23;
    v118 = 0LL;
    v119 = (int64_t)String_slice((void*)v118, (int64_t)v1, v116, v117);
    v120 = 0LL;
    Vec_push((void*)v120, (int64_t)v115, v119);
    v122 = *(void**)v5;
    p_call_stack_pop();
    return v122;
  bb7:
    v46 = *(int64_t*)v43;
    v47 = *(int64_t*)v27;
    v48 = v46 < v47;
    if (v48) goto bb8;
    else goto bb9;
  bb8:
    v50 = *(int64_t*)v30;
    v51 = (int64_t)v1 + v50;
    v52 = *(int64_t*)v43;
    v53 = v51 + v52;
    v54 = malloc(8);
    *(int64_t*)v54 = (int64_t)v53;
    v56 = *(int64_t*)v43;
    v57 = (int64_t)v2 + v56;
    v58 = malloc(8);
    *(int64_t*)v58 = (int64_t)v57;
    v60 = *(int64_t*)v54;
    v61 = malloc(8);
    *(void**)v61 = (void*)v60;
    v63 = *(int64_t*)v58;
    v64 = malloc(8);
    *(void**)v64 = (void*)v63;
    v66 = 0LL;
    v67 = malloc(8);
    *(int64_t*)v67 = (int64_t)v66;
    v69 = 0LL;
    v70 = malloc(8);
    *(int64_t*)v70 = (int64_t)v69;
    v73 = *(void**)v61;
    v74 = *(uint8_t*)v73;
    *(int64_t*)v67 = (int64_t)v74;
    v76 = *(void**)v64;
    v77 = *(uint8_t*)v76;
    *(int64_t*)v70 = (int64_t)v77;
    v80 = *(int64_t*)v67;
    v81 = *(int64_t*)v70;
    v82 = v80 != v81;
    if (v82) goto bb10;
    else goto bb11;
  bb9:
    v93 = *(bool*)v40;
    if (v93) goto bb13;
    else goto bb14;
  bb10:
    v84 = false;
    *(bool*)v40 = v84;
    goto bb9;
  bb11:
    goto bb12;
  bb12:
    v88 = *(int64_t*)v43;
    v89 = 1LL;
    v90 = v88 + v89;
    *(int64_t*)v43 = (int64_t)v90;
    goto bb7;
  bb13:
    v95 = *(void**)v5;
    v96 = *(int64_t*)v19;
    v97 = *(int64_t*)v30;
    v98 = 0LL;
    v99 = (int64_t)String_slice((void*)v98, (int64_t)v1, v96, v97);
    v100 = 0LL;
    Vec_push((void*)v100, (int64_t)v95, v99);
    v102 = *(int64_t*)v30;
    v103 = *(int64_t*)v27;
    v104 = v102 + v103;
    *(int64_t*)v19 = (int64_t)v104;
    v106 = *(int64_t*)v19;
    *(int64_t*)v30 = (int64_t)v106;
    goto bb15;
  bb14:
    v109 = *(int64_t*)v30;
    v110 = 1LL;
    v111 = v109 + v110;
    *(int64_t*)v30 = (int64_t)v111;
    goto bb15;
  bb15:
    goto bb4;
    return 0;
}

void* p_str_from_ptr(void* v0, int64_t v1) {
    p_call_stack_push("p_str_from_ptr");
    int64_t v27; /* placeholder for void */
    int64_t v39;
    int64_t v75; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    void* v93;
    void* v103;
    int64_t v104;
    int64_t v5;
    uint8_t v77;
    int64_t v106; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    int64_t v58; /* placeholder for void */
    bool v74;
    void* v76;
    int64_t v91;
    void* v110;
    int64_t v71; /* placeholder for void */
    int64_t v46;
    int64_t v73;
    int64_t v111; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    void* v94;
    int64_t v45; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    void* v78;
    int64_t v105; /* placeholder for void */
    int64_t v38;
    int64_t v107;
    void* v48;
    int64_t v109; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    void* v10;
    int64_t v68;
    int64_t v40;
    int64_t v52; /* placeholder for void */
    int64_t v22;
    int64_t v51; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v96;
    int64_t v19; /* placeholder for void */
    bool v36;
    void* v23;
    int64_t v41; /* placeholder for void */
    uint8_t v49;
    void* v6;
    void* v56;
    int64_t v64;
    int64_t v2;
    int64_t v34;
    void* v60;
    void* v31;
    int64_t v44;
    int64_t v12; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    int64_t v72;
    int64_t v14;
    void* v80;
    void* v17;
    int64_t v42;
    int64_t v43;
    void* v57;
    int64_t v81;
    int64_t v87;
    int64_t v9;
    int64_t v47; /* placeholder for void */
    void* v13;
    int64_t v16; /* placeholder for void */
    int64_t v55;
    int64_t v83;
    int64_t v4; /* placeholder for void */
    int64_t v84;
    int64_t v90; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    void* v65;
    bool v3;
    int64_t v15; /* placeholder for void */
    int64_t v59;
    int64_t v79; /* placeholder for void */
    int64_t v25;
    int64_t v95; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v69;
    int64_t v98;
    int64_t v28; /* placeholder for void */
    int64_t v100;
    int64_t v102; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    void* v20;
    int64_t v35;
    int64_t v50; /* placeholder for void */
    int64_t v97;
    void* v26;
    int64_t v99; /* placeholder for void */
    void* v62;
    void* v108;
    void* v29;
    int64_t v53;
    void* v7;
    int64_t v66; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v54;
    int64_t v24; /* placeholder for void */
    uint8_t v30;
    int64_t v61; /* placeholder for void */
    int64_t v85;
    int64_t v89;
    int64_t v88;
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 1LL;
    v6 = p_malloc(v5);
    v7 = malloc(8);
    *(int64_t*)v7 = (int64_t)v6;
    v9 = *(int64_t*)v7;
    v10 = malloc(8);
    *(void**)v10 = (void*)v9;
    v13 = *(void**)v10;
    v14 = 0LL;
    *(uint8_t*)v13 = (uint8_t)v14;
    v17 = *(void**)v10;
    p_call_stack_pop();
    return v17;
  bb2:
    goto bb3;
  bb3:
    v20 = malloc(8);
    *(int64_t*)v20 = (int64_t)v1;
    v22 = *(int64_t*)v20;
    v23 = malloc(8);
    *(void**)v23 = (void*)v22;
    v25 = 0LL;
    v26 = malloc(8);
    *(int64_t*)v26 = (int64_t)v25;
    v29 = *(void**)v23;
    v30 = *(uint8_t*)v29;
    v31 = malloc(8);
    *(int64_t*)v31 = (int64_t)v30;
    goto bb4;
  bb4:
    v34 = *(int64_t*)v31;
    v35 = 0LL;
    v36 = v34 != v35;
    if (v36) goto bb5;
    else goto bb6;
  bb5:
    v38 = *(int64_t*)v26;
    v39 = 1LL;
    v40 = v38 + v39;
    *(int64_t*)v26 = (int64_t)v40;
    v42 = *(int64_t*)v20;
    v43 = 1LL;
    v44 = v42 + v43;
    *(int64_t*)v20 = (int64_t)v44;
    v46 = *(int64_t*)v20;
    *(void**)v23 = (void*)v46;
    v48 = *(void**)v23;
    v49 = *(uint8_t*)v48;
    *(int64_t*)v31 = (int64_t)v49;
    goto bb4;
  bb6:
    v53 = *(int64_t*)v26;
    v54 = 1LL;
    v55 = v53 + v54;
    v56 = p_malloc(v55);
    v57 = malloc(8);
    *(int64_t*)v57 = (int64_t)v56;
    v59 = *(int64_t*)v57;
    v60 = malloc(8);
    *(void**)v60 = (void*)v59;
    v62 = malloc(8);
    *(int64_t*)v62 = (int64_t)v1;
    v64 = *(int64_t*)v62;
    v65 = malloc(8);
    *(void**)v65 = (void*)v64;
    v68 = 0LL;
    v69 = malloc(8);
    *(int64_t*)v69 = (int64_t)v68;
    goto bb7;
  bb7:
    v72 = *(int64_t*)v69;
    v73 = *(int64_t*)v26;
    v74 = v72 < v73;
    if (v74) goto bb8;
    else goto bb9;
  bb8:
    v76 = *(void**)v65;
    v77 = *(uint8_t*)v76;
    v78 = malloc(8);
    *(int64_t*)v78 = (int64_t)v77;
    v80 = *(void**)v60;
    v81 = *(int64_t*)v78;
    *(uint8_t*)v80 = (uint8_t)v81;
    v83 = *(int64_t*)v69;
    v84 = 1LL;
    v85 = v83 + v84;
    *(int64_t*)v69 = (int64_t)v85;
    v87 = *(int64_t*)v62;
    v88 = 1LL;
    v89 = v87 + v88;
    *(int64_t*)v62 = (int64_t)v89;
    v91 = *(int64_t*)v62;
    *(void**)v65 = (void*)v91;
    v93 = *(void**)v60;
    v94 = malloc(8);
    *(int64_t*)v94 = (int64_t)v93;
    v96 = *(int64_t*)v94;
    v97 = 1LL;
    v98 = v96 + v97;
    *(int64_t*)v94 = (int64_t)v98;
    v100 = *(int64_t*)v94;
    *(void**)v60 = (void*)v100;
    goto bb7;
  bb9:
    v103 = *(void**)v60;
    v104 = 0LL;
    *(uint8_t*)v103 = (uint8_t)v104;
    v107 = *(int64_t*)v57;
    v108 = malloc(8);
    *(void**)v108 = (void*)v107;
    v110 = *(void**)v108;
    p_call_stack_pop();
    return v110;
    return 0;
}

void* str_to_upper(void* v0, void* v1) {
    p_call_stack_push("str_to_upper");
    int64_t v2;
    int64_t v4; /* placeholder for void */
    int64_t v3;
    v2 = 0LL;
    v3 = (int64_t)String_to_uppercase((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return (void*)v3;
    return 0;
}

int64_t str_byte_at(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("str_byte_at");
    int64_t v4;
    int64_t v3;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)String_byte_at((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

double dot(void* v0, void* v1, void* v2) {
    p_call_stack_push("dot");
    int64_t v4; /* placeholder for void */
    double v3;
    v3 = (double)p_tensor_dot(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

double sum(void* v0, void* v1) {
    p_call_stack_push("sum");
    double v2;
    int64_t v3; /* placeholder for void */
    v2 = (double)p_tensor_sum(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t String_repeat(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_repeat");
    int64_t v51;
    int64_t v24;
    int64_t v25;
    int64_t v50;
    void* v14;
    int64_t v43;
    int64_t v57;
    void* v65;
    int64_t v37; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v55;
    int64_t v58;
    int64_t v44;
    int64_t v38; /* placeholder for void */
    int64_t v21;
    int64_t v52;
    int64_t v29;
    int64_t v27; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v9;
    int64_t v19; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v42;
    void* v10;
    int64_t v28;
    int64_t v39;
    int64_t v64; /* placeholder for void */
    bool v6;
    int64_t v49; /* placeholder for void */
    void* v36;
    void* v26;
    int64_t v20; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    int64_t v46;
    void* v11;
    void* v33;
    int64_t v35;
    int64_t v54; /* placeholder for void */
    int64_t v18;
    int64_t v60; /* placeholder for void */
    void* v32;
    int64_t v48;
    void* v17;
    int64_t v56;
    int64_t v8; /* placeholder for void */
    int64_t v31;
    int64_t v53; /* placeholder for void */
    int64_t v30;
    int64_t v5;
    int64_t v69;
    int64_t v66;
    int64_t v41; /* placeholder for void */
    bool v7;
    void* v59;
    int64_t v63; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v13;
    int64_t v47;
    void* v62;
    bool v40;
    int64_t v45;
    int64_t v3;
    int64_t v61;
    bool v4;
    v3 = 0LL;
    v4 = v1 == v3;
    v5 = 0LL;
    v6 = v2 <= v5;
    v7 = v4 || v6;
    if (v7) goto bb1;
    else goto bb2;
  bb1:
    v9 = 1LL;
    v10 = p_malloc(v9);
    v11 = malloc(8);
    *(int64_t*)v11 = (int64_t)v10;
    v13 = *(int64_t*)v11;
    v14 = malloc(8);
    *(void**)v14 = (void*)v13;
    v17 = *(void**)v14;
    v18 = 0LL;
    *(uint8_t*)v17 = (uint8_t)v18;
    v21 = *(int64_t*)v11;
    p_call_stack_pop();
    return v21;
  bb2:
    goto bb3;
  bb3:
    v24 = 0LL;
    v25 = (int64_t)p_str_length((void*)v24, v1);
    v26 = malloc(8);
    *(int64_t*)v26 = (int64_t)v25;
    v28 = *(int64_t*)v26;
    v29 = v28 * v2;
    v30 = 1LL;
    v31 = v29 + v30;
    v32 = p_malloc(v31);
    v33 = malloc(8);
    *(int64_t*)v33 = (int64_t)v32;
    v35 = 0LL;
    v36 = malloc(8);
    *(int64_t*)v36 = (int64_t)v35;
    goto bb4;
  bb4:
    v39 = *(int64_t*)v36;
    v40 = v39 < v2;
    if (v40) goto bb5;
    else goto bb6;
  bb5:
    v42 = *(int64_t*)v33;
    v43 = *(int64_t*)v36;
    v44 = *(int64_t*)v26;
    v45 = v43 * v44;
    v46 = v42 + v45;
    v47 = *(int64_t*)v26;
    v48 = 0LL;
    p_str_copy_n((void*)v48, v46, v1, v47);
    v50 = *(int64_t*)v36;
    v51 = 1LL;
    v52 = v50 + v51;
    *(int64_t*)v36 = (int64_t)v52;
    goto bb4;
  bb6:
    v55 = *(int64_t*)v33;
    v56 = *(int64_t*)v26;
    v57 = v56 * v2;
    v58 = v55 + v57;
    v59 = malloc(8);
    *(int64_t*)v59 = (int64_t)v58;
    v61 = *(int64_t*)v59;
    v62 = malloc(8);
    *(void**)v62 = (void*)v61;
    v65 = *(void**)v62;
    v66 = 0LL;
    *(uint8_t*)v65 = (uint8_t)v66;
    v69 = *(int64_t*)v33;
    p_call_stack_pop();
    return v69;
    return 0;
}

void* byte_to_str(void* v0, int64_t v1) {
    p_call_stack_push("byte_to_str");
    int64_t v3; /* placeholder for void */
    void* v2;
    v2 = p_char_to_str(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t Result_unwrap(void* v0, void* v1) {
    p_call_stack_push("Result_unwrap");
    int64_t v9; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v21;
    int64_t v22; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    void* v7;
    bool v13;
    void* v15;
    void* v17;
    int64_t v20; /* placeholder for void */
    int64_t v8;
    int64_t v4;
    int64_t v16;
    int64_t v11;
    void* v10;
    void* v2;
    int64_t v3;
    bool v5;
    int64_t v19; /* placeholder for void */
    int64_t v12;
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v21 = 0LL;
    p_call_stack_pop();
    return v21;
  bb2:
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    p_call_stack_pop();
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
    v17 = _str14;
    p_panic(v17);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

void StringBuilder_append_int(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("StringBuilder_append_int");
    int64_t v15; /* placeholder for void */
    void* v7;
    int64_t v11;
    void* v4;
    void* v6;
    void* v5;
    void* v3;
    int64_t v12;
    void* v9;
    int64_t v14; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v10;
    int64_t v13;
    v3 = (char*)v1 + 0;
    v4 = (char*)v1 + 0;
    v5 = *(void**)v4;
    v6 = (void*)p_to_str((int64_t)v2);
    v7 = p_str_concat(v5, v6);
    *(void**)v3 = (void*)v7;
    v9 = (char*)v1 + 8;
    v10 = (char*)v1 + 8;
    v11 = *(int64_t*)v10;
    v12 = 1LL;
    v13 = v11 + v12;
    *(int64_t*)v9 = (int64_t)v13;
    p_call_stack_pop();
    return;
    return;
}

int64_t ByteBuffer_len(void* v0, void* v1) {
    p_call_stack_push("ByteBuffer_len");
    int64_t v3;
    void* v2;
    int64_t v4; /* placeholder for void */
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    p_call_stack_pop();
    return v3;
    return 0;
}

void StringBuilder_new(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_new");
    void* v7;
    void* v10;
    void* v3;
    int64_t v12; /* placeholder for void */
    int64_t v6;
    void* v14;
    int64_t v16; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v2;
    void* v4;
    void* v11;
    int64_t v15;
    void* v9;
    void* v13;
    int64_t v8; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    v2 = malloc(16);
    v3 = _str8;
    v4 = (char*)v2 + 0;
    *(void**)v4 = (void*)v3;
    v6 = 0LL;
    v7 = (char*)v2 + 8;
    *(int64_t*)v7 = (int64_t)v6;
    v9 = (char*)v2 + 0;
    v10 = (char*)v0 + 0;
    v11 = *(void**)v9;
    *(void**)v10 = (void*)v11;
    v13 = (char*)v2 + 8;
    v14 = (char*)v0 + 8;
    v15 = *(int64_t*)v13;
    *(int64_t*)v14 = (int64_t)v15;
    p_call_stack_pop();
    return;
    return;
}

// ─── C entry point ───
int main(int argc, char** argv) {
    __p_argc = argc; __p_argv = argv;
    // Register functions for p_thread_spawn lookup
    p_thread_register("Vec_len", (int64_t(*)())Vec_len);
    p_thread_register("max", (int64_t(*)())p_max);
    p_thread_register("clone", (int64_t(*)())clone);
    p_thread_register("allocator_alloc", (int64_t(*)())allocator_alloc);
    p_thread_register("String_char_at", (int64_t(*)())String_char_at);
    p_thread_register("ByteBuffer_new", (int64_t(*)())ByteBuffer_new);
    p_thread_register("Vec_contains", (int64_t(*)())Vec_contains);
    p_thread_register("char_is_digit", (int64_t(*)())char_is_digit);
    p_thread_register("Option_is_none", (int64_t(*)())Option_is_none);
    p_thread_register("mul", (int64_t(*)())mul);
    p_thread_register("Vec_reverse", (int64_t(*)())Vec_reverse);
    p_thread_register("str_parse_hex", (int64_t(*)())str_parse_hex);
    p_thread_register("Vec_get", (int64_t(*)())Vec_get);
    p_thread_register("String_to_uppercase", (int64_t(*)())String_to_uppercase);
    p_thread_register("p_str_equal", (int64_t(*)())p_str_equal);
    p_thread_register("get", (int64_t(*)())get);
    p_thread_register("Result_map", (int64_t(*)())Result_map);
    p_thread_register("str_contains", (int64_t(*)())str_contains);
    p_thread_register("String_ends_with", (int64_t(*)())String_ends_with);
    p_thread_register("str_equal", (int64_t(*)())str_equal);
    p_thread_register("Result_is_ok", (int64_t(*)())Result_is_ok);
    p_thread_register("char_to_upper", (int64_t(*)())char_to_upper);
    p_thread_register("char_to_str", (int64_t(*)())char_to_str);
    p_thread_register("Vec_to_str", (int64_t(*)())Vec_to_str);
    p_thread_register("reshape", (int64_t(*)())reshape);
    p_thread_register("println", (int64_t(*)())println);
    p_thread_register("set", (int64_t(*)())set);
    p_thread_register("Vec_truncate", (int64_t(*)())Vec_truncate);
    p_thread_register("String_to_lowercase", (int64_t(*)())String_to_lowercase);
    p_thread_register("String_rfind", (int64_t(*)())String_rfind);
    p_thread_register("char_is_whitespace", (int64_t(*)())char_is_whitespace);
    p_thread_register("String_len", (int64_t(*)())String_len);
    p_thread_register("Option_or_else", (int64_t(*)())Option_or_else);
    p_thread_register("ndim", (int64_t(*)())ndim);
    p_thread_register("size", (int64_t(*)())size);
    p_thread_register("Result_expect", (int64_t(*)())Result_expect);
    p_thread_register("Option_unwrap", (int64_t(*)())Option_unwrap);
    p_thread_register("Vec_remove", (int64_t(*)())Vec_remove);
    p_thread_register("str_bytes", (int64_t(*)())str_bytes);
    p_thread_register("Option_unwrap_or", (int64_t(*)())Option_unwrap_or);
    p_thread_register("ByteBuffer_write_bytes", (int64_t(*)())ByteBuffer_write_bytes);
    p_thread_register("Vec_new", (int64_t(*)())Vec_new);
    p_thread_register("HashMap_contains", (int64_t(*)())HashMap_contains);
    p_thread_register("String_trim", (int64_t(*)())String_trim);
    p_thread_register("to_json", (int64_t(*)())to_json);
    p_thread_register("String_byte_at", (int64_t(*)())String_byte_at);
    p_thread_register("String_char_count", (int64_t(*)())String_char_count);
    p_thread_register("String_split", (int64_t(*)())String_split);
    p_thread_register("p_str_copy_n", (int64_t(*)())p_str_copy_n);
    p_thread_register("StringBuilder_len", (int64_t(*)())StringBuilder_len);
    p_thread_register("Vec_push", (int64_t(*)())Vec_push);
    p_thread_register("String_find", (int64_t(*)())String_find);
    p_thread_register("HashMap_get_or", (int64_t(*)())HashMap_get_or);
    p_thread_register("str_to_lower", (int64_t(*)())str_to_lower);
    p_thread_register("min", (int64_t(*)())p_min);
    p_thread_register("HashMap_is_empty", (int64_t(*)())HashMap_is_empty);
    p_thread_register("Vec_set", (int64_t(*)())Vec_set);
    p_thread_register("Vec_insert", (int64_t(*)())Vec_insert);
    p_thread_register("HashMap_values", (int64_t(*)())HashMap_values);
    p_thread_register("HashMap_remove", (int64_t(*)())HashMap_remove);
    p_thread_register("String_replace", (int64_t(*)())String_replace);
    p_thread_register("abs", (int64_t(*)())p_abs);
    p_thread_register("p_str_compare", (int64_t(*)())p_str_compare);
    p_thread_register("Vec_capacity", (int64_t(*)())Vec_capacity);
    p_thread_register("char_to_lower", (int64_t(*)())char_to_lower);
    p_thread_register("to_str", (int64_t(*)())to_str);
    p_thread_register("String_parse_int", (int64_t(*)())String_parse_int);
    p_thread_register("str_trim_end", (int64_t(*)())str_trim_end);
    p_thread_register("ByteBuffer_write_byte", (int64_t(*)())ByteBuffer_write_byte);
    p_thread_register("HashMap_len", (int64_t(*)())HashMap_len);
    p_thread_register("str_find", (int64_t(*)())str_find);
    p_thread_register("Option_and_then", (int64_t(*)())Option_and_then);
    p_thread_register("String_push_str", (int64_t(*)())String_push_str);
    p_thread_register("String_to_string", (int64_t(*)())String_to_string);
    p_thread_register("ones", (int64_t(*)())ones);
    p_thread_register("char_is_lower", (int64_t(*)())char_is_lower);
    p_thread_register("HashMap_grow", (int64_t(*)())HashMap_grow);
    p_thread_register("clamp", (int64_t(*)())clamp);
    p_thread_register("str_replace", (int64_t(*)())str_replace);
    p_thread_register("Option_unwrap_or_else", (int64_t(*)())Option_unwrap_or_else);
    p_thread_register("str_rfind", (int64_t(*)())str_rfind);
    p_thread_register("Result_is_err", (int64_t(*)())Result_is_err);
    p_thread_register("Result_unwrap_or", (int64_t(*)())Result_unwrap_or);
    p_thread_register("str_split_char", (int64_t(*)())str_split_char);
    p_thread_register("ByteBuffer_to_str", (int64_t(*)())ByteBuffer_to_str);
    p_thread_register("ByteBuffer_clear", (int64_t(*)())ByteBuffer_clear);
    p_thread_register("shape", (int64_t(*)())shape);
    p_thread_register("HashMap_keys", (int64_t(*)())HashMap_keys);
    p_thread_register("StringBuilder_append_char", (int64_t(*)())StringBuilder_append_char);
    p_thread_register("String_clone", (int64_t(*)())String_clone);
    p_thread_register("str_is_empty", (int64_t(*)())str_is_empty);
    p_thread_register("sigmoid", (int64_t(*)())sigmoid);
    p_thread_register("HashMap_to_str", (int64_t(*)())HashMap_to_str);
    p_thread_register("filled", (int64_t(*)())filled);
    p_thread_register("int_to_str", (int64_t(*)())int_to_str);
    p_thread_register("sub", (int64_t(*)())sub);
    p_thread_register("Vec_is_empty", (int64_t(*)())Vec_is_empty);
    p_thread_register("Option_map", (int64_t(*)())Option_map);
    p_thread_register("HashMap_new", (int64_t(*)())HashMap_new);
    p_thread_register("HashMap_find_slot", (int64_t(*)())HashMap_find_slot);
    p_thread_register("scale", (int64_t(*)())scale);
    p_thread_register("Result_ok", (int64_t(*)())Result_ok);
    p_thread_register("Vec_clear", (int64_t(*)())Vec_clear);
    p_thread_register("p_str_length", (int64_t(*)())p_str_length);
    p_thread_register("StringBuilder_append_str", (int64_t(*)())StringBuilder_append_str);
    p_thread_register("Option_is_some", (int64_t(*)())Option_is_some);
    p_thread_register("str_slice", (int64_t(*)())str_slice);
    p_thread_register("HashMap_clear", (int64_t(*)())HashMap_clear);
    p_thread_register("from_vec", (int64_t(*)())from_vec);
    p_thread_register("String_starts_with", (int64_t(*)())String_starts_with);
    p_thread_register("Vec_last", (int64_t(*)())Vec_last);
    p_thread_register("relu", (int64_t(*)())relu);
    p_thread_register("Vec_sort", (int64_t(*)())Vec_sort);
    p_thread_register("ByteBuffer_read_byte", (int64_t(*)())ByteBuffer_read_byte);
    p_thread_register("String_bytes", (int64_t(*)())String_bytes);
    p_thread_register("str_ends_with", (int64_t(*)())str_ends_with);
    p_thread_register("str_parse_int", (int64_t(*)())str_parse_int);
    p_thread_register("create_allocator", (int64_t(*)())create_allocator);
    p_thread_register("str_len", (int64_t(*)())str_len);
    p_thread_register("transpose", (int64_t(*)())transpose);
    p_thread_register("HashMap_clone", (int64_t(*)())HashMap_clone);
    p_thread_register("assert", (int64_t(*)())assert);
    p_thread_register("String_slice", (int64_t(*)())String_slice);
    p_thread_register("char_is_upper", (int64_t(*)())char_is_upper);
    p_thread_register("Vec_pop", (int64_t(*)())Vec_pop);
    p_thread_register("str_parse_int_range", (int64_t(*)())str_parse_int_range);
    p_thread_register("Result_err", (int64_t(*)())Result_err);
    p_thread_register("HashMap_insert", (int64_t(*)())HashMap_insert);
    p_thread_register("mean", (int64_t(*)())mean);
    p_thread_register("softmax", (int64_t(*)())softmax);
    p_thread_register("Option_expect", (int64_t(*)())Option_expect);
    p_thread_register("allocator_free", (int64_t(*)())allocator_free);
    p_thread_register("String_contains", (int64_t(*)())String_contains);
    p_thread_register("add", (int64_t(*)())add);
    p_thread_register("HashMap_get", (int64_t(*)())HashMap_get);
    p_thread_register("String_is_empty", (int64_t(*)())String_is_empty);
    p_thread_register("str_starts_with", (int64_t(*)())str_starts_with);
    p_thread_register("char_is_alpha", (int64_t(*)())char_is_alpha);
    p_thread_register("StringBuilder_to_str", (int64_t(*)())StringBuilder_to_str);
    p_thread_register("StringBuilder_is_empty", (int64_t(*)())StringBuilder_is_empty);
    p_thread_register("str_compare", (int64_t(*)())str_compare);
    p_thread_register("ByteBuffer_is_empty", (int64_t(*)())ByteBuffer_is_empty);
    p_thread_register("allocator_bytes_used", (int64_t(*)())allocator_bytes_used);
    p_thread_register("StringBuilder_clear", (int64_t(*)())StringBuilder_clear);
    p_thread_register("Vec_first", (int64_t(*)())Vec_first);
    p_thread_register("Vec_clone", (int64_t(*)())Vec_clone);
    p_thread_register("zeros", (int64_t(*)())zeros);
    p_thread_register("matmul", (int64_t(*)())matmul);
    p_thread_register("p_str_hash", (int64_t(*)())p_str_hash);
    p_thread_register("String_from", (int64_t(*)())String_from);
    p_thread_register("str_repeat", (int64_t(*)())str_repeat);
    p_thread_register("Result_and_then", (int64_t(*)())Result_and_then);
    p_thread_register("str_concat", (int64_t(*)())str_concat);
    p_thread_register("str_trim", (int64_t(*)())str_trim);
    p_thread_register("Result_unwrap_or_else", (int64_t(*)())Result_unwrap_or_else);
    p_thread_register("Vec_extend", (int64_t(*)())Vec_extend);
    p_thread_register("ByteBuffer_from_string", (int64_t(*)())ByteBuffer_from_string);
    p_thread_register("str_from_bytes", (int64_t(*)())str_from_bytes);
    p_thread_register("ByteBuffer_set_byte", (int64_t(*)())ByteBuffer_set_byte);
    p_thread_register("str_trim_start", (int64_t(*)())str_trim_start);
    p_thread_register("Result_map_err", (int64_t(*)())Result_map_err);
    p_thread_register("str_split", (int64_t(*)())str_split);
    p_thread_register("p_str_from_ptr", (int64_t(*)())p_str_from_ptr);
    p_thread_register("str_to_upper", (int64_t(*)())str_to_upper);
    p_thread_register("str_byte_at", (int64_t(*)())str_byte_at);
    p_thread_register("dot", (int64_t(*)())dot);
    p_thread_register("sum", (int64_t(*)())sum);
    p_thread_register("String_repeat", (int64_t(*)())String_repeat);
    p_thread_register("byte_to_str", (int64_t(*)())byte_to_str);
    p_thread_register("Result_unwrap", (int64_t(*)())Result_unwrap);
    p_thread_register("StringBuilder_append_int", (int64_t(*)())StringBuilder_append_int);
    p_thread_register("ByteBuffer_len", (int64_t(*)())ByteBuffer_len);
    p_thread_register("StringBuilder_new", (int64_t(*)())StringBuilder_new);
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

// Phase 10B: p_map_keys returns a Fides Vec (i64 pointer)
int64_t p_map_keys(PMap* m) {{
    PVec* v = p_vec_new();
    for (int64_t i = 0; i < m->len; i++)
        p_vec_push(v, (void*)m->keys[i]);
    return (int64_t)v;
}}

int64_t p_map_values(PMap* m) {{
    PVec* v = p_vec_new();
    for (int64_t i = 0; i < m->len; i++) {{
        p_vec_push(v, (void*)m->vals[i]);
    }}
    return (int64_t)v;
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
void p_panic(const char* msg) {{
    fprintf(stderr, "Panic: %s\n", msg);
    if (p_call_stack_depth > 0) {{
        fprintf(stderr, "stack trace:\n");
        for (int i = p_call_stack_depth - 1; i >= 0; i--) {{
            if (i == p_call_stack_depth - 1)
                fprintf(stderr, "    → %s\n", p_call_stack[i]);
            else
                fprintf(stderr, "    ↳ called from %s\n", p_call_stack[i]);
        }}
    }}
    abort();
}}
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
int64_t p_file_delete(const char* path) {{ return remove(path) == 0 ? 0 : -1; }}
int64_t p_file_size(const char* path) {{ FILE* f = fopen(path, "rb"); if (!f) return -1; fseek(f, 0, SEEK_END); int64_t sz = ftell(f); fclose(f); return sz; }}
char* p_getenv(const char* name) {{ return getenv(name); }}
int64_t p_argc(void) {{ return (int64_t)__p_argc; }}
char* p_argv(int64_t index) {{ if (index < 0 || index >= __p_argc) return NULL; return __p_argv[index]; }}
int64_t p_system(const char* cmd) {{ return (int64_t)system(cmd); }}
void p_exit(int64_t code) {{ exit((int)code); }}
bool p_regex_match(const char* s, const char* pattern) {{
    if (!s || !pattern) return false;
    int64_t si = 0, pi = 0, s_len = (int64_t)strlen(s), p_len = (int64_t)strlen(pattern);
    int64_t star_s = -1, star_p = -1;
    while (si < s_len) {{
        if (pi < p_len && (pattern[pi] == '?' || pattern[pi] == s[si])) {{ si++; pi++; }}
        else if (pi < p_len && pattern[pi] == '*') {{ star_p = pi; star_s = si; pi++; }}
        else if (star_p >= 0) {{ pi = star_p + 1; si = star_s + 1; star_s = si; }}
        else return false;
    }}
    while (pi < p_len && pattern[pi] == '*') pi++;
    return pi == p_len;
}}
#include <pthread.h>
int64_t p_future_ready(int64_t value) { return value; }
static pthread_t p_thread_handles[256];
static int p_thread_handle_count = 0;
static int64_t p_task_results[256];
int64_t p_future_poll(int64_t future) {
    if (future >= 0) return future; // eager future
    // Thread-based future: join and get result
    int idx = -(future + 1);
    if (idx < 0 || idx >= p_thread_handle_count) return 0;
    if (p_thread_handles[idx] == 0) return p_task_results[idx]; // already joined
    void* ret = NULL;
    pthread_join(p_thread_handles[idx], &ret);
    p_task_results[idx] = (int64_t)(intptr_t)ret;
    p_thread_handles[idx] = 0;
    return p_task_results[idx];
}
#include <stdatomic.h>
#include <stdarg.h>
typedef struct { const char* name; int64_t (*fn)(); } p_thread_entry_t;
static p_thread_entry_t p_thread_entries[256];
static int p_thread_entry_count = 0;
void p_thread_register(const char* name, int64_t (*fn)()) {
    if (p_thread_entry_count < 256) {
        p_thread_entries[p_thread_entry_count].name = name;
        p_thread_entries[p_thread_entry_count].fn = fn;
        p_thread_entry_count++;
    }
}
typedef struct { int64_t (*fn)(); int64_t nargs; int64_t args[8]; atomic_int* done; } p_thread_arg_t;
static void* p_thread_wrapper(void* arg) {
    p_thread_arg_t* ta = (p_thread_arg_t*)arg;
    int64_t result = 0;
    // Fides functions have an implicit __env param (void*) as first arg.
    // Pass NULL for __env, then the user-provided args.
    switch (ta->nargs) {
        case 0: { typedef int64_t (*f0)(void*); f0 f = (f0)ta->fn; result = f(NULL); break; }
        case 1: { typedef int64_t (*f1)(void*,int64_t); f1 f = (f1)ta->fn; result = f(NULL, ta->args[0]); break; }
        case 2: { typedef int64_t (*f2)(void*,int64_t,int64_t); f2 f = (f2)ta->fn; result = f(NULL, ta->args[0], ta->args[1]); break; }
        case 3: { typedef int64_t (*f3)(void*,int64_t,int64_t,int64_t); f3 f = (f3)ta->fn; result = f(NULL, ta->args[0], ta->args[1], ta->args[2]); break; }
        default: { typedef int64_t (*f4)(void*,int64_t,int64_t,int64_t,int64_t); f4 f = (f4)ta->fn; result = f(NULL, ta->args[0], ta->args[1], ta->args[2], ta->args[3]); break; }
    }
    if (ta->done) atomic_store(ta->done, 1);
    return (void*)(intptr_t)result;
}
static atomic_int p_task_done_flags[256];
static int p_executor_count = 0;
int64_t p_executor_new(void) { return (int64_t)(p_executor_count++); }
int64_t p_executor_spawn(int64_t exec, const char* func_name, int64_t num_args, ...) {
    (void)exec;
    int64_t (*fn)() = NULL;
    for (int i = 0; i < p_thread_entry_count; i++) {
        if (strcmp(p_thread_entries[i].name, func_name) == 0) { fn = p_thread_entries[i].fn; break; }
    }
    if (!fn) return -1;
    p_thread_arg_t* ta = (p_thread_arg_t*)malloc(sizeof(p_thread_arg_t));
    ta->fn = fn; ta->nargs = num_args;
    va_list ap; va_start(ap, num_args);
    for (int64_t i = 0; i < num_args && i < 8; i++) ta->args[i] = va_arg(ap, int64_t);
    va_end(ap);
    int idx = p_thread_handle_count++;
    atomic_init(&p_task_done_flags[idx], 0);
    ta->done = &p_task_done_flags[idx];
    pthread_t tid; pthread_create(&tid, NULL, p_thread_wrapper, ta);
    p_thread_handles[idx] = tid;
    return -(int64_t)(idx + 1);
}
int64_t p_task_poll(int64_t future) {
    if (future >= 0) return future; // eager future
    int idx = -(future + 1);
    if (idx < 0 || idx >= p_thread_handle_count) return 0;
    if (p_thread_handles[idx] == 0) return p_task_results[idx]; // already joined
    // A1: block until done (join). A2 will add non-blocking poll.
    void* ret = NULL; pthread_join(p_thread_handles[idx], &ret);
    p_task_results[idx] = (int64_t)(intptr_t)ret;
    p_thread_handles[idx] = 0;
    return p_task_results[idx];
}
bool p_task_is_ready(int64_t future) {
    if (future >= 0) return true;
    int idx = -(future + 1);
    if (idx < 0 || idx >= p_thread_handle_count) return false;
    return atomic_load(&p_task_done_flags[idx]) != 0;
}
int64_t p_executor_block_on(int64_t exec, int64_t future) {
    (void)exec;
    return p_future_poll(future);
}
int64_t p_executor_run_all(int64_t exec) {
    (void)exec;
    int64_t count = 0;
    for (int i = 0; i < p_thread_handle_count; i++) {
        if (p_thread_handles[i] == 0) continue; // already joined
        void* ret = NULL; pthread_join(p_thread_handles[i], &ret);
        p_task_results[i] = (int64_t)(intptr_t)ret;
        p_thread_handles[i] = 0; count++;
    }
    return count;
}
int64_t p_task_poll_nonblocking(int64_t future) {
    if (future >= 0) return future; // eager future
    int idx = -(future + 1);
    if (idx < 0 || idx >= p_thread_handle_count) return 0;
    if (atomic_load(&p_task_done_flags[idx]) == 0) {
        return (int64_t)0x8000000000000001LL; // PENDING_SENTINEL
    }
    if (p_thread_handles[idx] == 0) return p_task_results[idx]; // already joined
    void* ret = NULL; pthread_join(p_thread_handles[idx], &ret);
    p_task_results[idx] = (int64_t)(intptr_t)ret;
    p_thread_handles[idx] = 0;
    return p_task_results[idx];
}
int64_t p_executor_poll_all(int64_t exec) {
    (void)exec;
    int64_t pending = 0;
    for (int i = 0; i < p_thread_handle_count; i++) {
        if (p_thread_handles[i] == 0) continue; // already joined
        if (atomic_load(&p_task_done_flags[i]) == 0) { pending++; }
        else { void* ret = NULL; pthread_join(p_thread_handles[i], &ret); p_task_results[i] = (int64_t)(intptr_t)ret; p_thread_handles[i] = 0; }
    }
    return pending;
}
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
typedef struct { int64_t sock; char* buf; int64_t len; int64_t result; atomic_int* done; int op; } p_io_arg_t;
// op: 0=connect, 1=read, 2=write, 3=fs_read
static void* p_io_worker(void* arg) {
    p_io_arg_t* ia = (p_io_arg_t*)arg;
    int64_t result = 0;
    switch (ia->op) {
        case 1: { // read
            ssize_t n = recv((int)ia->sock, ia->buf, (size_t)ia->len, 0);
            result = (n < 0) ? -1 : (int64_t)n;
            break;
        }
        case 2: { // write
            ssize_t n = send((int)ia->sock, ia->buf, (size_t)ia->len, 0);
            result = (n < 0) ? -1 : (int64_t)n;
            break;
        }
        case 3: { // fs_read
            FILE* f = fopen((const char*)ia->sock, "rb");
            if (f) {
                size_t n = fread(ia->buf, 1, (size_t)ia->len, f);
                result = (int64_t)n;
                fclose(f);
            } else { result = -1; }
            break;
        }
        default: result = 0;
    }
    ia->result = result;
    if (ia->done) atomic_store(ia->done, 1);
    return (void*)(intptr_t)result;
}
// Helper: spawn an IO thread and return a Future handle
static int64_t p_io_spawn(int op, int64_t sock, char* buf, int64_t len) {
    int idx = p_thread_handle_count++;
    atomic_init(&p_task_done_flags[idx], 0);
    p_io_arg_t* ia = (p_io_arg_t*)malloc(sizeof(p_io_arg_t));
    ia->sock = sock; ia->buf = buf; ia->len = len; ia->result = 0;
    ia->done = &p_task_done_flags[idx]; ia->op = op;
    pthread_t tid; pthread_create(&tid, NULL, p_io_worker, ia);
    p_thread_handles[idx] = tid;
    p_task_results[idx] = 0; // will be set by p_task_poll after join
    return -(int64_t)(idx + 1);
}
int64_t p_net_connect(const char* host, int64_t port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    inet_pton(AF_INET, host, &addr.sin_addr);
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock); return -1;
    }
    // Return the socket FD as an eager future (positive = ready)
    return (int64_t)sock;
}
int64_t p_net_read(int64_t sock, char* buf, int64_t len) {
    return p_io_spawn(1, sock, buf, len);
}
int64_t p_net_write(int64_t sock, const char* buf, int64_t len) {
    return p_io_spawn(2, sock, (char*)buf, len);
}
void p_net_close(int64_t sock) {
    close((int)sock);
}
int64_t p_fs_read_async(const char* path, char* buf, int64_t len) {
    // Use sock field to pass the path pointer (op=3 reads it as path)
    return p_io_spawn(3, (int64_t)(intptr_t)path, buf, len);
}
int64_t p_thread_spawn(const char* func_name, int64_t num_args, ...) {
    // Look up function by name
    int64_t (*fn)() = NULL;
    for (int i = 0; i < p_thread_entry_count; i++) {
        if (strcmp(p_thread_entries[i].name, func_name) == 0) {
            fn = p_thread_entries[i].fn;
            break;
        }
    }
    if (!fn) return -1;
    p_thread_arg_t* ta = (p_thread_arg_t*)malloc(sizeof(p_thread_arg_t));
    ta->fn = fn;
    ta->nargs = num_args;
    va_list ap; va_start(ap, num_args);
    for (int64_t i = 0; i < num_args && i < 8; i++) ta->args[i] = va_arg(ap, int64_t);
    va_end(ap);
    int idx = p_thread_handle_count++;
    atomic_init(&p_task_done_flags[idx], 0);
    ta->done = &p_task_done_flags[idx];
    pthread_t tid;
    pthread_create(&tid, NULL, p_thread_wrapper, ta);
    // Store thread handle and return negative index as Future
    p_thread_handles[idx] = tid;
    return -(int64_t)(idx + 1);
}
int64_t p_shared_alloc(int64_t size) { return (int64_t)(intptr_t)malloc((size_t)size); }
int64_t p_atomic_load(int64_t addr) { return atomic_load((atomic_int_least64_t*)(intptr_t)addr); }
void p_atomic_store(int64_t addr, int64_t val) { atomic_store((atomic_int_least64_t*)(intptr_t)addr, val); }
bool p_atomic_cas(int64_t addr, int64_t expected, int64_t new_val) {
    return atomic_compare_exchange_strong((atomic_int_least64_t*)(intptr_t)addr, &expected, new_val);
}
int64_t p_atomic_add(int64_t addr, int64_t val) { return atomic_fetch_add((atomic_int_least64_t*)(intptr_t)addr, val); }
int64_t p_atomic_sub(int64_t addr, int64_t val) { return atomic_fetch_sub((atomic_int_least64_t*)(intptr_t)addr, val); }
int64_t p_rc_incr(int64_t rc_addr, int64_t val) { (*(int64_t*)(intptr_t)rc_addr)++; return val; }
void p_rc_decr(int64_t rc_addr, int64_t val) {
    (void)val;
    int64_t* rc = (int64_t*)(intptr_t)rc_addr;
    (*rc)--;
    if (*rc <= 0) { free((void*)(intptr_t)rc_addr); }
}
int64_t p_mutex_init(void) {
    pthread_mutex_t* m = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(m, NULL);
    return (int64_t)(intptr_t)m;
}
void p_mutex_lock(int64_t m) { pthread_mutex_lock((pthread_mutex_t*)(intptr_t)m); }
void p_mutex_unlock(int64_t m) { pthread_mutex_unlock((pthread_mutex_t*)(intptr_t)m); }
#include <pthread.h>
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    int64_t head;
    int64_t tail;
    int64_t count;
    int64_t capacity;
    int closed;
    int64_t buffer[]; // flexible array member
} p_channel_t;
// PENDING_SENTINEL — returned by p_chan_try_recv when channel is empty
#define P_CHAN_PENDING ((int64_t)0x8000000000000001LL)
int64_t p_chan_create(int64_t capacity) {
    if (capacity < 1) capacity = 1;
    p_channel_t* ch = (p_channel_t*)malloc(sizeof(p_channel_t) + capacity * sizeof(int64_t));
    pthread_mutex_init(&ch->mutex, NULL);
    pthread_cond_init(&ch->not_empty, NULL);
    pthread_cond_init(&ch->not_full, NULL);
    ch->head = 0; ch->tail = 0; ch->count = 0;
    ch->capacity = capacity;
    ch->closed = 0;
    return (int64_t)(intptr_t)ch;
}
bool p_chan_send(int64_t handle, int64_t val) {
    p_channel_t* ch = (p_channel_t*)(intptr_t)handle;
    if (!ch) return false;
    pthread_mutex_lock(&ch->mutex);
    // Block until there's space or the channel is closed
    while (ch->count == ch->capacity && !ch->closed) {
        pthread_cond_wait(&ch->not_full, &ch->mutex);
    }
    if (ch->closed) {
        pthread_mutex_unlock(&ch->mutex);
        return false;
    }
    ch->buffer[ch->tail] = val;
    ch->tail = (ch->tail + 1) % ch->capacity;
    ch->count++;
    pthread_cond_signal(&ch->not_empty);
    pthread_mutex_unlock(&ch->mutex);
    return true;
}
int64_t p_chan_recv(int64_t handle) {
    p_channel_t* ch = (p_channel_t*)(intptr_t)handle;
    if (!ch) return 0;
    pthread_mutex_lock(&ch->mutex);
    // Block until there's an item or the channel is closed and empty
    while (ch->count == 0 && !ch->closed) {
        pthread_cond_wait(&ch->not_empty, &ch->mutex);
    }
    if (ch->count == 0) {
        // Closed and empty
        pthread_mutex_unlock(&ch->mutex);
        return P_CHAN_PENDING;
    }
    int64_t val = ch->buffer[ch->head];
    ch->head = (ch->head + 1) % ch->capacity;
    ch->count--;
    pthread_cond_signal(&ch->not_full);
    pthread_mutex_unlock(&ch->mutex);
    return val;
}
int64_t p_chan_try_recv(int64_t handle) {
    p_channel_t* ch = (p_channel_t*)(intptr_t)handle;
    if (!ch) return P_CHAN_PENDING;
    pthread_mutex_lock(&ch->mutex);
    if (ch->count == 0) {
        int closed = ch->closed;
        pthread_mutex_unlock(&ch->mutex);
        return closed ? 0 : P_CHAN_PENDING;
    }
    int64_t val = ch->buffer[ch->head];
    ch->head = (ch->head + 1) % ch->capacity;
    ch->count--;
    pthread_cond_signal(&ch->not_full);
    pthread_mutex_unlock(&ch->mutex);
    return val;
}
void p_chan_close(int64_t handle) {
    p_channel_t* ch = (p_channel_t*)(intptr_t)handle;
    if (!ch) return;
    pthread_mutex_lock(&ch->mutex);
    ch->closed = 1;
    // Wake up all blocked senders and receivers
    pthread_cond_broadcast(&ch->not_empty);
    pthread_cond_broadcast(&ch->not_full);
    pthread_mutex_unlock(&ch->mutex);
}
bool p_chan_is_closed(int64_t handle) {
    p_channel_t* ch = (p_channel_t*)(intptr_t)handle;
    if (!ch) return true;
    pthread_mutex_lock(&ch->mutex);
    int closed = ch->closed;
    pthread_mutex_unlock(&ch->mutex);
    return closed;
}
#include <dlfcn.h>
int64_t p_dl_open(const char* path) {
    void* h = dlopen(path, RTLD_LAZY);
    return (int64_t)(intptr_t)h;
}
int64_t p_dl_sym(int64_t handle, const char* symbol) {
    void* h = (void*)(intptr_t)handle;
    if (!h) return 0;
    void* sym = dlsym(h, symbol);
    return (int64_t)(intptr_t)sym;
}
void p_dl_close(int64_t handle) {
    void* h = (void*)(intptr_t)handle;
    if (h) dlclose(h);
}
int64_t p_dl_call(int64_t func_ptr, int64_t arg) {
    typedef int64_t (*dl_fn_t)(int64_t);
    dl_fn_t fn = (dl_fn_t)(intptr_t)func_ptr;
    if (!fn) return 0;
    return fn(arg);
}
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
int64_t p_string_split(const char* s, const char* delim) {{
    PVec* v = p_vec_new();
    if (!s || !delim) return (int64_t)v;
    const char* start = s;
    size_t dlen = strlen(delim);
    while (*start) {{
        const char* p = strstr(start, delim);
        if (!p) {{ p_vec_push(v, (void*)strdup(start)); break; }}
        size_t len = p - start;
        char* part = (char*)malloc(len + 1);
        memcpy(part, start, len); part[len] = 0;
        p_vec_push(v, (void*)part);
        start = p + dlen;
    }}
    return (int64_t)v;
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

int64_t p_vec_capacity(PVec* v) {{ return v ? v->cap : 0; }}
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
char* p_char_to_str(int64_t val) {{ char* s = (char*)malloc(2); s[0] = (char)val; s[1] = 0; return s; }}
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

// SIMD builtins — operate on Fides Vec (i64 pointer, f64 stored as i64 bits)
// Helper: read double from Fides Vec element (stored as i64 bit pattern)
static double _fvec_get_f64(int64_t v, int i) {
    int64_t val = (int64_t)p_vec_get((PVec*)v, i);
    union { int64_t i; double d; } u; u.i = val; return u.d;
}
static int64_t _fvec_new() { return (int64_t)p_vec_new(); }
static void _fvec_push_f64(int64_t v, double d) {
    union { double d; int64_t i; } u; u.d = d; p_vec_push((PVec*)v, (void*)u.i);
}
int64_t p_simd_add(int64_t a, int64_t b) {{
    int64_t r = _fvec_new();
    int n = (int)p_vec_len((PVec*)a);
    for (int i = 0; i < n; i++) _fvec_push_f64(r, _fvec_get_f64(a,i) + _fvec_get_f64(b,i));
    return r;
}}
int64_t p_simd_sub(int64_t a, int64_t b) {{
    int64_t r = _fvec_new();
    int n = (int)p_vec_len((PVec*)a);
    for (int i = 0; i < n; i++) _fvec_push_f64(r, _fvec_get_f64(a,i) - _fvec_get_f64(b,i));
    return r;
}}
int64_t p_simd_mul(int64_t a, int64_t b) {{
    int64_t r = _fvec_new();
    int n = (int)p_vec_len((PVec*)a);
    for (int i = 0; i < n; i++) _fvec_push_f64(r, _fvec_get_f64(a,i) * _fvec_get_f64(b,i));
    return r;
}}
int64_t p_simd_div(int64_t a, int64_t b) {{
    int64_t r = _fvec_new();
    int n = (int)p_vec_len((PVec*)a);
    for (int i = 0; i < n; i++) {{ double db = _fvec_get_f64(b,i); _fvec_push_f64(r, db != 0 ? _fvec_get_f64(a,i) / db : 0); }}
    return r;
}}
double p_simd_dot(int64_t a, int64_t b) {{
    int n = (int)p_vec_len((PVec*)a); double s = 0;
    for (int i = 0; i < n; i++) s += _fvec_get_f64(a,i) * _fvec_get_f64(b,i);
    return s;
}}
double p_simd_sum(int64_t a) {{
    int n = (int)p_vec_len((PVec*)a); double s = 0;
    for (int i = 0; i < n; i++) s += _fvec_get_f64(a,i);
    return s;
}}
int64_t p_simd_scale(int64_t a, double s) {{
    int64_t r = _fvec_new();
    int n = (int)p_vec_len((PVec*)a);
    for (int i = 0; i < n; i++) _fvec_push_f64(r, _fvec_get_f64(a,i) * s);
    return r;
}}
int64_t p_simd_from_vec(int64_t v) {{
    int64_t r = _fvec_new();
    int n = (int)p_vec_len((PVec*)v);
    for (int i = 0; i < n; i++) _fvec_push_f64(r, (double)(int64_t)p_vec_get((PVec*)v, i));
    return r;
}}
char* p_simd_to_str(int64_t a) {{
    if (!a) return strdup("[]");
    size_t cap = 64; char* result = (char*)malloc(cap); size_t pos = 0;
    result[pos++] = '[';
    int64_t n = p_vec_len((PVec*)a);
    for (int64_t i = 0; i < n; i++) {{
        if (i > 0) {{ if (pos+2>=cap){{cap*=2;result=(char*)realloc(result,cap);}} result[pos++]=','; result[pos++]=' '; }}
        double d = _fvec_get_f64(a, i);
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

typedef struct { int64_t entity; void* value; } EcsEntry;
typedef struct { char name[64]; EcsEntry* entries; int64_t len; int64_t cap; } EcsComponent;
static EcsComponent* p_ecs_components = NULL;
static int64_t p_ecs_component_count = 0;
static int64_t p_ecs_component_cap = 0;
static int64_t p_ecs_entity_count = 0;

static EcsComponent* p_ecs_find_component(const char* name) {
    for (int64_t i = 0; i < p_ecs_component_count; i++) {
        if (strncmp(p_ecs_components[i].name, name, 63) == 0) return &p_ecs_components[i];
    }
    return NULL;
}

static EcsComponent* p_ecs_get_or_create_component(const char* name) {
    EcsComponent* c = p_ecs_find_component(name);
    if (c) return c;
    if (p_ecs_component_count >= p_ecs_component_cap) {
        p_ecs_component_cap = p_ecs_component_cap == 0 ? 8 : p_ecs_component_cap * 2;
        p_ecs_components = (EcsComponent*)realloc(p_ecs_components, p_ecs_component_cap * sizeof(EcsComponent));
    }
    c = &p_ecs_components[p_ecs_component_count++];
    strncpy(c->name, name, 63); c->name[63] = 0;
    c->entries = NULL; c->len = 0; c->cap = 0;
    return c;
}

int64_t __ecs_create_entity(void) {
    return p_ecs_entity_count++;
}

int64_t __ecs_entity_count(void) {
    return p_ecs_entity_count;
}

int64_t __ecs_has(const char* comp_name, int64_t entity) {
    EcsComponent* c = p_ecs_find_component(comp_name);
    if (!c) return 0;
    for (int64_t i = 0; i < c->len; i++) {
        if (c->entries[i].entity == entity) return 1;
    }
    return 0;
}

void* __ecs_get(const char* comp_name, int64_t entity) {
    EcsComponent* c = p_ecs_find_component(comp_name);
    if (!c) return NULL;
    for (int64_t i = 0; i < c->len; i++) {
        if (c->entries[i].entity == entity) return c->entries[i].value;
    }
    return NULL;
}

int64_t __ecs_add(const char* comp_name, int64_t entity, void* value) {
    EcsComponent* c = p_ecs_get_or_create_component(comp_name);
    // Check if entity already exists — update value
    for (int64_t i = 0; i < c->len; i++) {
        if (c->entries[i].entity == entity) { c->entries[i].value = value; return 0; }
    }
    // Add new entry
    if (c->len >= c->cap) {
        c->cap = c->cap == 0 ? 8 : c->cap * 2;
        c->entries = (EcsEntry*)realloc(c->entries, c->cap * sizeof(EcsEntry));
    }
    c->entries[c->len].entity = entity;
    c->entries[c->len].value = value;
    c->len++;
    return 0;
}

int64_t __ecs_remove(const char* comp_name, int64_t entity) {
    EcsComponent* c = p_ecs_find_component(comp_name);
    if (!c) return 0;
    for (int64_t i = 0; i < c->len; i++) {
        if (c->entries[i].entity == entity) {
            c->entries[i] = c->entries[c->len - 1];
            c->len--;
            return 0;
        }
    }
    return 0;
}
