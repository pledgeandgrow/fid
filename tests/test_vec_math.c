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

#include <math.h>

// PLINK: m
// ─── User-defined extern declarations (FFI) ───
double sqrt(double x);
#define pledge_sqrt sqrt
double sin(double x);
#define pledge_sin sin
double cos(double x);
#define pledge_cos cos
double floor(double x);
#define pledge_floor floor
double ceil(double x);
#define pledge_ceil ceil
double pow(double base, double exp);
#define pledge_pow pow
double fabs(double x);
#define pledge_fabs fabs
double log(double x);
#define pledge_log log
double exp(double x);
#define pledge_exp exp

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
static const char _str0[] = "";
static const char _str1[] = "Vec index out of bounds";
static const char _str2[] = "{";
static const char _str3[] = "}";
static const char _str4[] = ", ";
static const char _str5[] = ": ";
static const char _str6[] = "Vec is empty";
static const char _str7[] = "called unwrap on Err";
static const char _str8[] = "called unwrap on None";
static const char _str9[] = "\n";
static const char _str10[] = "[";
static const char _str11[] = "]";
static const char _str12[] = "Assertion failed\n";
static const char _str13[] = "Vec insert index out of bounds";
static const char _str14[] = "vec2 add: ";
static const char _str15[] = "vec2 dot: ";
static const char _str16[] = "vec3 add: ";
static const char _str17[] = "vec3 sub: ";
static const char _str18[] = "vec3 scale: ";
static const char _str19[] = "vec3 dot: ";
static const char _str20[] = "vec3 cross: ";
static const char _str21[] = "operator a + b: ";
static const char _str22[] = "operator a - b: ";
static const char _str23[] = "operator a * 2: ";
static const char _str24[] = "operator 3 * a: ";
static const char _str25[] = "operator a / 2: ";
static const char _str26[] = "vec3 length: ";
static const char _str27[] = "vec3 normalize: ";
static const char _str28[] = "vec3 lerp: ";
static const char _str29[] = "vec4 add: ";
static const char _str30[] = "vec4 dot: ";
static const char _str31[] = "vec4.xyz: ";
static const char _str32[] = "swizzle .rgb: ";
static const char _str33[] = "swizzle .xy: ";
static const char _str34[] = "swizzle .yzx: ";
static const char _str35[] = "swizzle .xxx: ";
static const char _str36[] = "mat4 identity transform: ";
static const char _str37[] = "translate: ";
static const char _str38[] = "scale: ";
static const char _str39[] = "translate*mat_scale: ";
static const char _str40[] = "quat w: ";
static const char _str41[] = "quat x: ";
static const char _str42[] = "quat y: ";
static const char _str43[] = "quat z: ";
static const char _str44[] = "rotated: ";
static const char _str45[] = "radians(90): ";
static const char _str46[] = "degrees(3.14159): ";
static const char _str47[] = "lerp(0, 10, 0.5): ";
static const char _str48[] = "distance: ";
static const char _str49[] = "OK\n";
static const char _str50[] = "Vec remove index out of bounds";

// ─── Struct definitions ───
struct Result_s {
    int64_t field_0;
    int64_t field_1;
};

struct Option_s {
    int64_t field_0;
    int64_t field_1;
};

struct mat4_s {
    double field_0;
    double field_1;
    double field_2;
    double field_3;
    double field_4;
    double field_5;
    double field_6;
    double field_7;
    double field_8;
    double field_9;
    double field_10;
    double field_11;
    double field_12;
    double field_13;
    double field_14;
    double field_15;
};

struct vec3_s {
    double field_0;
    double field_1;
    double field_2;
};

struct quat_s {
    double field_0;
    double field_1;
    double field_2;
    double field_3;
};

struct ByteBuffer_s {
    void* field_0;
    int64_t field_1;
};

struct StringBuilder_s {
    void* field_0;
    int64_t field_1;
};

struct vec4_s {
    double field_0;
    double field_1;
    double field_2;
    double field_3;
};

struct vec2_s {
    double field_0;
    double field_1;
};

// ─── Function prototypes ───
void p_thread_register(const char* name, int64_t (*fn)());
void* ByteBuffer_to_str(void* v0, void* v1);
double math_pi(void* v0);
bool String_contains(void* v0, int64_t v1, int64_t v2);
int64_t allocator_alloc(void* v0, void* v1, int64_t v2);
void* str_slice(void* v0, void* v1, int64_t v2, int64_t v3);
int64_t Result_unwrap_or_else(void* v0, void* v1, void* v2);
int64_t String_len(void* v0, int64_t v1);
int64_t String_push_str(void* v0, int64_t v1, int64_t v2);
bool str_is_empty(void* v0, void* v1);
void ByteBuffer_write_byte(void* v0, void* v1, int64_t v2);
void* char_to_str(void* v0, int64_t v1);
void ByteBuffer_set_byte(void* v0, void* v1, int64_t v2, int64_t v3);
int64_t str_parse_hex(void* v0, void* v1);
void HashMap_insert(void* v0, int64_t v1, int64_t v2, int64_t v3);
void vec3_cross(void* v0, void* v1, void* v2, void* v3);
void quat(void* v0, void* v1, double v2, double v3, double v4, double v5);
int64_t HashMap_get_or(void* v0, int64_t v1, int64_t v2, int64_t v3);
void* str_to_upper(void* v0, void* v1);
int64_t Vec_capacity(void* v0, int64_t v1);
void vec2_add(void* v0, void* v1, void* v2, void* v3);
int64_t p_abs(void* v0, int64_t v1);
int64_t String_to_uppercase(void* v0, int64_t v1);
int64_t String_split(void* v0, int64_t v1, int64_t v2);
void Option_map(void* v0, void* v1, void* v2, void* v3);
void ByteBuffer_new(void* v0, void* v1, int64_t v2);
int64_t HashMap_len(void* v0, int64_t v1);
void vec4_xyz(void* v0, void* v1, void* v2);
void vec4_add(void* v0, void* v1, void* v2, void* v3);
void StringBuilder_append_int(void* v0, void* v1, int64_t v2);
int64_t ByteBuffer_read_byte(void* v0, void* v1, int64_t v2);
bool char_is_lower(void* v0, int64_t v1);
void StringBuilder_new(void* v0, void* v1);
double vec4_dot(void* v0, void* v1, void* v2);
void HashMap_clear(void* v0, int64_t v1);
double vec3_dot(void* v0, void* v1, void* v2);
void vec3_normalize(void* v0, void* v1, void* v2);
int64_t clamp(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t String_to_lowercase(void* v0, int64_t v1);
void Vec_set(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t p_min(void* v0, int64_t v1, int64_t v2);
bool Vec_contains(void* v0, int64_t v1, int64_t v2);
bool String_ends_with(void* v0, int64_t v1, int64_t v2);
int64_t math_abs_i64(void* v0, int64_t v1);
void* str_bytes(void* v0, void* v1);
int64_t String_from(void* v0, int64_t v1);
int64_t p_str_length(void* v0, int64_t v1);
void* str_from_bytes(void* v0, void* v1);
void vec2_scale(void* v0, void* v1, void* v2, double v3);
void vec3_sub(void* v0, void* v1, void* v2, void* v3);
double vec2_length(void* v0, void* v1);
void* create_allocator(void* v0, void* v1);
void vec3_scale(void* v0, void* v1, void* v2, double v3);
double lerp(void* v0, double v1, double v2, double v3);
void HashMap_grow(void* v0, int64_t v1);
void* HashMap_to_str(void* v0, int64_t v1);
bool Result_is_ok(void* v0, void* v1);
void mat4_mul(void* v0, void* v1, void* v2, void* v3);
void Vec_push(void* v0, int64_t v1, int64_t v2);
int64_t String_bytes(void* v0, int64_t v1);
void quat_rotate_vec3(void* v0, void* v1, void* v2, void* v3);
int64_t p_max(void* v0, int64_t v1, int64_t v2);
void* int_to_str(void* v0, int64_t v1);
void quat_from_axis_angle(void* v0, void* v1, void* v2, double v3);
int64_t Option_unwrap_or_else(void* v0, void* v1, void* v2);
void Vec_truncate(void* v0, int64_t v1, int64_t v2);
double radians(void* v0, double v1);
int64_t String_find(void* v0, int64_t v1, int64_t v2);
void allocator_free(void* v0, void* v1, int64_t v2);
int64_t Vec_last(void* v0, int64_t v1);
int64_t Result_unwrap(void* v0, void* v1);
void Result_map(void* v0, void* v1, void* v2, void* v3);
int64_t String_char_at(void* v0, int64_t v1, int64_t v2);
void* str_trim_end(void* v0, void* v1);
int64_t Result_expect(void* v0, void* v1, void* v2);
void Result_map_err(void* v0, void* v1, void* v2, void* v3);
double math_sin(void* v0, double v1);
void* byte_to_str(void* v0, int64_t v1);
void vec4_scale(void* v0, void* v1, void* v2, double v3);
int64_t Option_unwrap(void* v0, void* v1);
int64_t p_str_hash(void* v0, int64_t v1);
int64_t str_byte_at(void* v0, void* v1, int64_t v2);
void* str_split_char(void* v0, void* v1, int64_t v2);
void mat4_transform_vec4(void* v0, void* v1, void* v2, void* v3);
int64_t String_rfind(void* v0, int64_t v1, int64_t v2);
int64_t Vec_get(void* v0, int64_t v1, int64_t v2);
bool str_starts_with(void* v0, void* v1, void* v2);
bool HashMap_contains(void* v0, int64_t v1, int64_t v2);
double math_fabs(void* v0, double v1);
double math_e(void* v0);
void Vec_reverse(void* v0, int64_t v1);
void StringBuilder_append_char(void* v0, void* v1, int64_t v2);
void* str_concat(void* v0, void* v1, void* v2);
int64_t char_to_lower(void* v0, int64_t v1);
int64_t HashMap_values(void* v0, int64_t v1);
int64_t HashMap_get(void* v0, int64_t v1, int64_t v2);
int64_t StringBuilder_len(void* v0, void* v1);
void println(void* v0, void* v1);
void p_str_copy_n(void* v0, int64_t v1, int64_t v2, int64_t v3);
double math_floor(void* v0, double v1);
int64_t str_find(void* v0, void* v1, void* v2);
bool HashMap_is_empty(void* v0, int64_t v1);
void* str_trim_start(void* v0, void* v1);
void vec2_normalize(void* v0, void* v1, void* v2);
void vec2_sub(void* v0, void* v1, void* v2, void* v3);
void vec4_lerp(void* v0, void* v1, void* v2, void* v3, double v4);
void mat4_transform_vec3(void* v0, void* v1, void* v2, void* v3);
int64_t String_trim(void* v0, int64_t v1);
bool Option_is_none(void* v0, void* v1);
void ByteBuffer_from_string(void* v0, void* v1, void* v2);
bool ByteBuffer_is_empty(void* v0, void* v1);
void vec3_lerp(void* v0, void* v1, void* v2, void* v3, double v4);
double math_sqrt(void* v0, double v1);
void* str_split(void* v0, void* v1, void* v2);
void* Vec_to_str(void* v0, int64_t v1);
void* str_to_lower(void* v0, void* v1);
void* str_repeat(void* v0, void* v1, int64_t v2);
void assert(void* v0, bool v1);
double math_max_f64(void* v0, double v1, double v2);
int64_t Vec_clone(void* v0, int64_t v1);
int64_t String_to_string(void* v0, int64_t v1);
int64_t String_byte_at(void* v0, int64_t v1, int64_t v2);
bool str_equal(void* v0, void* v1, void* v2);
void vec4_normalize(void* v0, void* v1, void* v2);
double math_exp(void* v0, double v1);
void mat4_identity(void* v0, void* v1);
int64_t Option_unwrap_or(void* v0, void* v1, int64_t v2);
bool Result_is_err(void* v0, void* v1);
int64_t HashMap_keys(void* v0, int64_t v1);
void* StringBuilder_to_str(void* v0, void* v1);
bool str_ends_with(void* v0, void* v1, void* v2);
void StringBuilder_append_str(void* v0, void* v1, void* v2);
double vec3_length(void* v0, void* v1);
void Vec_insert(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t Vec_first(void* v0, int64_t v1);
int64_t allocator_bytes_used(void* v0, void* v1);
bool char_is_alpha(void* v0, int64_t v1);
void HashMap_remove(void* v0, int64_t v1, int64_t v2);
bool char_is_upper(void* v0, int64_t v1);
void Option_and_then(void* v0, void* v1, void* v2, void* v3);
bool char_is_digit(void* v0, int64_t v1);
int64_t str_parse_int_range(void* v0, void* v1, int64_t v2, int64_t v3);
double degrees(void* v0, double v1);
int64_t main_entry(void* v0);
double math_cos(void* v0, double v1);
bool String_is_empty(void* v0, int64_t v1);
bool p_str_equal(void* v0, int64_t v1, int64_t v2);
int64_t String_parse_int(void* v0, int64_t v1);
int64_t str_len(void* v0, void* v1);
bool str_contains(void* v0, void* v1, void* v2);
void* str_trim(void* v0, void* v1);
void vec2(void* v0, void* v1, double v2, double v3);
void Result_and_then(void* v0, void* v1, void* v2, void* v3);
void Vec_sort(void* v0, int64_t v1);
void Vec_extend(void* v0, int64_t v1, int64_t v2);
void vec4_sub(void* v0, void* v1, void* v2, void* v3);
void StringBuilder_clear(void* v0, void* v1);
void vec3(void* v0, void* v1, double v2, double v3, double v4);
int64_t Option_expect(void* v0, void* v1, void* v2);
int64_t ByteBuffer_len(void* v0, void* v1);
int64_t String_clone(void* v0, int64_t v1);
bool StringBuilder_is_empty(void* v0, void* v1);
void* p_str_from_ptr(void* v0, int64_t v1);
int64_t Vec_pop(void* v0, int64_t v1);
double math_min_f64(void* v0, double v1, double v2);
void ByteBuffer_write_bytes(void* v0, void* v1, int64_t v2, int64_t v3);
int64_t HashMap_new(void* v0);
void* str_replace(void* v0, void* v1, void* v2, void* v3);
int64_t str_parse_int(void* v0, void* v1);
double distance(void* v0, void* v1, void* v2);
int64_t Vec_new(void* v0);
double math_log(void* v0, double v1);
int64_t Result_unwrap_or(void* v0, void* v1, int64_t v2);
int64_t HashMap_clone(void* v0, int64_t v1);
void vec2_lerp(void* v0, void* v1, void* v2, void* v3, double v4);
int64_t Vec_remove(void* v0, int64_t v1, int64_t v2);
int64_t p_str_compare(void* v0, int64_t v1, int64_t v2);
int64_t Vec_len(void* v0, int64_t v1);
bool char_is_whitespace(void* v0, int64_t v1);
int64_t char_to_upper(void* v0, int64_t v1);
double math_clamp_f64(void* v0, double v1, double v2, double v3);
int64_t String_slice(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t str_rfind(void* v0, void* v1, void* v2);
void Option_or_else(void* v0, void* v1, void* v2, void* v3);
int64_t String_char_count(void* v0, int64_t v1);
void translate(void* v0, void* v1, void* v2);
void vec4(void* v0, void* v1, double v2, double v3, double v4, double v5);
void mat_scale(void* v0, void* v1, double v2);
bool Vec_is_empty(void* v0, int64_t v1);
bool String_starts_with(void* v0, int64_t v1, int64_t v2);
void Result_ok(void* v0, void* v1, void* v2);
int64_t str_compare(void* v0, void* v1, void* v2);
int64_t HashMap_find_slot(void* v0, int64_t v1, int64_t v2);
void Vec_clear(void* v0, int64_t v1);
double vec2_dot(void* v0, void* v1, void* v2);
int64_t String_repeat(void* v0, int64_t v1, int64_t v2);
double vec4_length(void* v0, void* v1);
double math_ceil(void* v0, double v1);
double math_pow(void* v0, double v1, double v2);
int64_t String_replace(void* v0, int64_t v1, int64_t v2, int64_t v3);
void vec3_add(void* v0, void* v1, void* v2, void* v3);
bool Option_is_some(void* v0, void* v1);
void Result_err(void* v0, void* v1, void* v2);
void ByteBuffer_clear(void* v0, void* v1);

void* ByteBuffer_to_str(void* v0, void* v1) {
    p_call_stack_push("ByteBuffer_to_str");
    int64_t v9;
    int64_t v20; /* placeholder for void */
    void* v14;
    void* v24;
    void* v21;
    int64_t v29; /* placeholder for void */
    int64_t v5;
    void* v31;
    bool v12;
    void* v10;
    void* v6;
    void* v23;
    int64_t v7; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    void* v15;
    int64_t v32; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    void* v3;
    int64_t v25; /* placeholder for void */
    int64_t v11;
    int64_t v17;
    int64_t v16;
    int64_t v18;
    void* v19;
    int64_t v27;
    int64_t v22;
    int64_t v13; /* placeholder for void */
    int64_t v26;
    void* v2;
    int64_t v8; /* placeholder for void */
    int64_t v28;
    v2 = _str0;
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

double math_pi(void* v0) {
    p_call_stack_push("math_pi");
    int64_t v2; /* placeholder for void */
    double v1;
    v1 = ((union { uint64_t u; double f; }){ .u = 4614256656552045848ULL }).f;
    p_call_stack_pop();
    return v1;
    return 0;
}

bool String_contains(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_contains");
    bool v6;
    int64_t v73; /* placeholder for void */
    int64_t v61;
    int64_t v65;
    bool v108;
    int64_t v5;
    int64_t v102; /* placeholder for void */
    int64_t v52;
    int64_t v105;
    int64_t v33; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    int64_t v68;
    int64_t v90; /* placeholder for void */
    void* v78;
    int64_t v109; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    void* v69;
    int64_t v17;
    int64_t v46; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    bool v87;
    int64_t v13;
    int64_t v43; /* placeholder for void */
    int64_t v93;
    void* v18;
    int64_t v51;
    int64_t v11; /* placeholder for void */
    int64_t v56;
    int64_t v57;
    uint8_t v82;
    int64_t v88; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v62;
    int64_t v94;
    int64_t v15; /* placeholder for void */
    bool v42;
    bool v31;
    int64_t v58;
    int64_t v96; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    void* v59;
    void* v48;
    bool v53;
    int64_t v38;
    int64_t v106; /* placeholder for void */
    void* v72;
    int64_t v55;
    int64_t v20;
    bool v100;
    int64_t v70; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    bool v29;
    int64_t v26; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    bool v4;
    int64_t v27;
    int64_t v39;
    int64_t v41;
    int64_t v34;
    int64_t v50; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    void* v66;
    int64_t v28;
    bool v24;
    int64_t v71;
    bool v44;
    int64_t v85;
    int64_t v74;
    int64_t v86;
    int64_t v101; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v103;
    int64_t v107; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    int64_t v99; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v47;
    int64_t v80; /* placeholder for void */
    bool v89;
    void* v35;
    int64_t v21;
    int64_t v3;
    bool v7;
    bool v22;
    int64_t v95;
    int64_t v30; /* placeholder for void */
    int64_t v104;
    void* v75;
    uint8_t v79;
    int64_t v12;
    void* v45;
    int64_t v49; /* placeholder for void */
    bool v98;
    int64_t v10; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v16;
    int64_t v23; /* placeholder for void */
    void* v14;
    int64_t v32; /* placeholder for void */
    int64_t v40;
    void* v81;
    int64_t v37; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    bool v9;
    void* v63;
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

int64_t allocator_alloc(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("allocator_alloc");
    int64_t v4; /* placeholder for void */
    int64_t v3;
    v3 = (int64_t)p_allocator_alloc(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

void* str_slice(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("str_slice");
    int64_t v17; /* placeholder for void */
    void* v10;
    bool v30;
    bool v22;
    int64_t v26; /* placeholder for void */
    int64_t v5;
    int64_t v15; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    void* v6;
    int64_t v37;
    int64_t v39; /* placeholder for void */
    int64_t v28;
    int64_t v23; /* placeholder for void */
    int64_t v36;
    int64_t v21;
    int64_t v29;
    int64_t v20;
    int64_t v24;
    int64_t v16;
    int64_t v4;
    int64_t v9; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v13;
    void* v8;
    int64_t v7; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v35;
    int64_t v12;
    void* v32;
    int64_t v38;
    bool v14;
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
    v32 = _str0;
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

int64_t Result_unwrap_or_else(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_unwrap_or_else");
    int64_t v5;
    int64_t v17;
    void* v16;
    bool v6;
    void* v8;
    int64_t v9;
    int64_t v15; /* placeholder for void */
    void* v3;
    int64_t v21;
    int64_t v13;
    bool v14;
    int64_t v12;
    int64_t v18;
    int64_t v22; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v4;
    int64_t v10; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v11;
    int64_t v20; /* placeholder for void */
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

int64_t String_len(void* v0, int64_t v1) {
    p_call_stack_push("String_len");
    int64_t v3;
    int64_t v2;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t String_push_str(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_push_str");
    int64_t v41;
    void* v37;
    void* v40;
    int64_t v28; /* placeholder for void */
    int64_t v44;
    int64_t v8;
    int64_t v6; /* placeholder for void */
    int64_t v25;
    void* v16;
    int64_t v35; /* placeholder for void */
    int64_t v36;
    int64_t v3;
    int64_t v19;
    int64_t v27;
    int64_t v42; /* placeholder for void */
    int64_t v23;
    int64_t v26;
    int64_t v12;
    void* v5;
    int64_t v29;
    int64_t v7;
    int64_t v31;
    int64_t v33;
    int64_t v15;
    int64_t v20;
    int64_t v38; /* placeholder for void */
    int64_t v30;
    int64_t v13;
    void* v9;
    int64_t v45; /* placeholder for void */
    void* v34;
    int64_t v43; /* placeholder for void */
    int64_t v24;
    int64_t v10; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v4;
    int64_t v21;
    int64_t v14;
    void* v17;
    int64_t v22; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v11;
    int64_t v32;
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

bool str_is_empty(void* v0, void* v1) {
    p_call_stack_push("str_is_empty");
    int64_t v2;
    bool v3;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (bool)String_is_empty((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

void ByteBuffer_write_byte(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("ByteBuffer_write_byte");
    void* v8;
    void* v3;
    int64_t v10;
    void* v4;
    int64_t v11;
    int64_t v13; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v9;
    void* v7;
    int64_t v5;
    int64_t v12; /* placeholder for void */
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

void* char_to_str(void* v0, int64_t v1) {
    p_call_stack_push("char_to_str");
    int64_t v3; /* placeholder for void */
    void* v2;
    v2 = p_char_to_str(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void ByteBuffer_set_byte(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("ByteBuffer_set_byte");
    bool v9;
    int64_t v4;
    int64_t v17; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    bool v5;
    void* v6;
    int64_t v12; /* placeholder for void */
    int64_t v15;
    void* v14;
    int64_t v7;
    bool v8;
    int64_t v10; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    void* v13;
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

int64_t str_parse_hex(void* v0, void* v1) {
    p_call_stack_push("str_parse_hex");
    int64_t v196;
    uint8_t v173;
    int64_t v125;
    int64_t v226;
    int64_t v209;
    int64_t v62;
    bool v186;
    int64_t v234; /* placeholder for void */
    void* v129;
    int64_t v21; /* placeholder for void */
    void* v29;
    int64_t v238; /* placeholder for void */
    int64_t v167; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v54;
    int64_t v192; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    uint8_t v116;
    int64_t v107; /* placeholder for void */
    void* v132;
    int64_t v105;
    void* v172;
    int64_t v150;
    int64_t v40; /* placeholder for void */
    int64_t v31;
    void* v106;
    int64_t v34;
    bool v211;
    int64_t v190;
    bool v182;
    int64_t v2;
    int64_t v221; /* placeholder for void */
    int64_t v101;
    bool v195;
    int64_t v202;
    void* v178;
    int64_t v7;
    void* v123;
    int64_t v174; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v117; /* placeholder for void */
    int64_t v42;
    void* v35;
    int64_t v181;
    bool v212;
    int64_t v82;
    int64_t v114; /* placeholder for void */
    int64_t v68;
    int64_t v110; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v77;
    int64_t v179; /* placeholder for void */
    bool v8;
    int64_t v160; /* placeholder for void */
    int64_t v206;
    int64_t v65;
    void* v69;
    int64_t v95; /* placeholder for void */
    bool v145;
    int64_t v231; /* placeholder for void */
    int64_t v239; /* placeholder for void */
    int64_t v157;
    int64_t v148;
    int64_t v5; /* placeholder for void */
    int64_t v137;
    bool v89;
    int64_t v152; /* placeholder for void */
    int64_t v92;
    int64_t v228;
    int64_t v22; /* placeholder for void */
    int64_t v91;
    int64_t v147; /* placeholder for void */
    int64_t v24;
    int64_t v153; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v50;
    int64_t v193;
    int64_t v10;
    int64_t v93;
    int64_t v111;
    int64_t v168;
    int64_t v207;
    int64_t v236;
    int64_t v79; /* placeholder for void */
    int64_t v124; /* placeholder for void */
    int64_t v130; /* placeholder for void */
    void* v112;
    bool v159;
    void* v63;
    bool v78;
    int64_t v9; /* placeholder for void */
    int64_t v84;
    int64_t v142;
    int64_t v214;
    bool v19;
    int64_t v27;
    int64_t v139;
    int64_t v184;
    int64_t v210;
    int64_t v121;
    bool v144;
    int64_t v220; /* placeholder for void */
    int64_t v165;
    int64_t v219; /* placeholder for void */
    int64_t v180;
    int64_t v187; /* placeholder for void */
    int64_t v76;
    int64_t v98;
    int64_t v143;
    int64_t v104;
    int64_t v154; /* placeholder for void */
    int64_t v215;
    int64_t v28;
    bool v198;
    void* v17;
    int64_t v218; /* placeholder for void */
    int64_t v203;
    bool v185;
    int64_t v88;
    void* v32;
    int64_t v48; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v161;
    int64_t v75; /* placeholder for void */
    int64_t v55;
    int64_t v53; /* placeholder for void */
    int64_t v227; /* placeholder for void */
    int64_t v242; /* placeholder for void */
    bool v25;
    int64_t v164; /* placeholder for void */
    int64_t v171; /* placeholder for void */
    int64_t v216;
    bool v56;
    int64_t v47; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    bool v199;
    int64_t v13;
    int64_t v86; /* placeholder for void */
    int64_t v194;
    int64_t v213; /* placeholder for void */
    int64_t v127; /* placeholder for void */
    int64_t v149;
    int64_t v135; /* placeholder for void */
    int64_t v119;
    int64_t v204; /* placeholder for void */
    int64_t v49;
    int64_t v58;
    int64_t v94; /* placeholder for void */
    bool v208;
    int64_t v191; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v87;
    int64_t v100;
    int64_t v51;
    int64_t v188;
    int64_t v223;
    void* v126;
    int64_t v134; /* placeholder for void */
    void* v115;
    int64_t v201;
    int64_t v217; /* placeholder for void */
    bool v44;
    int64_t v103; /* placeholder for void */
    void* v20;
    bool v138;
    uint8_t v133;
    int64_t v59; /* placeholder for void */
    int64_t v189;
    int64_t v170; /* placeholder for void */
    void* v4;
    int64_t v37; /* placeholder for void */
    int64_t v120;
    int64_t v151; /* placeholder for void */
    bool v233;
    int64_t v122;
    int64_t v136;
    int64_t v177;
    int64_t v45;
    void* v72;
    int64_t v158;
    bool v80;
    int64_t v128;
    int64_t v240; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    void* v14;
    int64_t v224;
    bool v146;
    int64_t v162;
    int64_t v131; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    void* v163;
    int64_t v176;
    void* v169;
    int64_t v11; /* placeholder for void */
    int64_t v99;
    bool v102;
    int64_t v241;
    int64_t v229;
    int64_t v232; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v197;
    int64_t v140;
    uint8_t v73;
    int64_t v175; /* placeholder for void */
    int64_t v23;
    int64_t v108;
    int64_t v61;
    int64_t v237;
    int64_t v6;
    int64_t v90; /* placeholder for void */
    int64_t v3;
    int64_t v16;
    void* v166;
    int64_t v222;
    int64_t v96; /* placeholder for void */
    void* v109;
    bool v141;
    int64_t v235;
    int64_t v43;
    int64_t v71; /* placeholder for void */
    int64_t v156; /* placeholder for void */
    void* v66;
    int64_t v155; /* placeholder for void */
    int64_t v118; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v183;
    void* v38;
    int64_t v200; /* placeholder for void */
    int64_t v85; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    uint8_t v39;
    int64_t v225;
    int64_t v230;
    int64_t v83;
    int64_t v205; /* placeholder for void */
    int64_t v113; /* placeholder for void */
    int64_t v64; /* placeholder for void */
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

void HashMap_insert(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("HashMap_insert");
    int64_t v126; /* placeholder for void */
    int64_t v134;
    int64_t v20;
    int64_t v50; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v88; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    int64_t v92;
    void* v107;
    int64_t v139; /* placeholder for void */
    int64_t v123;
    int64_t v125; /* placeholder for void */
    int64_t v130;
    int64_t v12;
    void* v13;
    bool v36;
    int64_t v141; /* placeholder for void */
    int64_t v54;
    int64_t v124; /* placeholder for void */
    int64_t v102;
    int64_t v44; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    void* v61;
    int64_t v78;
    int64_t v68;
    int64_t v118;
    int64_t v37; /* placeholder for void */
    int64_t v27;
    int64_t v24; /* placeholder for void */
    int64_t v8;
    int64_t v73;
    int64_t v5;
    int64_t v14; /* placeholder for void */
    int64_t v15;
    int64_t v94;
    void* v138;
    bool v96;
    void* v6;
    int64_t v93;
    int64_t v11; /* placeholder for void */
    int64_t v42;
    int64_t v17; /* placeholder for void */
    int64_t v33;
    int64_t v66; /* placeholder for void */
    void* v99;
    int64_t v46; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v105;
    int64_t v111; /* placeholder for void */
    int64_t v117;
    int64_t v29;
    int64_t v40; /* placeholder for void */
    int64_t v133; /* placeholder for void */
    int64_t v81;
    int64_t v65;
    void* v132;
    int64_t v114; /* placeholder for void */
    int64_t v30;
    int64_t v90;
    int64_t v131;
    int64_t v25; /* placeholder for void */
    int64_t v80; /* placeholder for void */
    int64_t v103;
    int64_t v98; /* placeholder for void */
    void* v67;
    void* v19;
    void* v55;
    int64_t v26;
    int64_t v112; /* placeholder for void */
    int64_t v136; /* placeholder for void */
    int64_t v87; /* placeholder for void */
    int64_t v9;
    int64_t v23;
    int64_t v21; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    void* v82;
    int64_t v57;
    int64_t v4;
    void* v79;
    int64_t v83; /* placeholder for void */
    void* v85;
    int64_t v106;
    bool v28;
    int64_t v137; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v72;
    int64_t v47;
    int64_t v48;
    void* v49;
    int64_t v56; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v129;
    int64_t v128;
    int64_t v115; /* placeholder for void */
    int64_t v34;
    void* v41;
    void* v119;
    void* v135;
    void* v16;
    int64_t v71;
    int64_t v32;
    int64_t v63; /* placeholder for void */
    int64_t v31;
    int64_t v121; /* placeholder for void */
    void* v76;
    void* v22;
    int64_t v53;
    void* v64;
    int64_t v116;
    void* v58;
    int64_t v109;
    int64_t v127;
    bool v35;
    int64_t v108; /* placeholder for void */
    void* v113;
    int64_t v52; /* placeholder for void */
    int64_t v38;
    int64_t v104;
    int64_t v75;
    int64_t v97; /* placeholder for void */
    bool v95;
    int64_t v74;
    void* v122;
    void* v51;
    int64_t v60;
    int64_t v142; /* placeholder for void */
    bool v91;
    void* v110;
    int64_t v86;
    int64_t v45; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    void* v10;
    int64_t v89;
    int64_t v100; /* placeholder for void */
    int64_t v120; /* placeholder for void */
    int64_t v140; /* placeholder for void */
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

void vec3_cross(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("vec3_cross");
    double v39;
    double v11;
    void* v23;
    double v22;
    void* v42;
    double v8;
    void* v49;
    int64_t v30; /* placeholder for void */
    double v6;
    double v37;
    void* v52;
    int64_t v17; /* placeholder for void */
    double v32;
    void* v36;
    int64_t v43; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    void* v5;
    void* v45;
    void* v16;
    double v13;
    void* v18;
    double v40;
    void* v12;
    void* v29;
    void* v33;
    double v26;
    double v46;
    double v19;
    void* v20;
    double v34;
    double v35;
    double v21;
    void* v38;
    double v14;
    double v15;
    double v9;
    double v41;
    double v28;
    void* v44;
    void* v10;
    void* v53;
    double v50;
    int64_t v55; /* placeholder for void */
    double v54;
    double v27;
    void* v25;
    void* v31;
    void* v48;
    int64_t v56; /* placeholder for void */
    void* v4;
    void* v7;
    int64_t v51; /* placeholder for void */
    double v24;
    v4 = malloc(24);
    v5 = (char*)v2 + 8;
    v6 = *(double*)v5;
    v7 = (char*)v3 + 16;
    v8 = *(double*)v7;
    v9 = v6 * v8;
    v10 = (char*)v2 + 16;
    v11 = *(double*)v10;
    v12 = (char*)v3 + 8;
    v13 = *(double*)v12;
    v14 = v11 * v13;
    v15 = v9 - v14;
    v16 = (char*)v4 + 0;
    *(double*)v16 = v15;
    v18 = (char*)v2 + 16;
    v19 = *(double*)v18;
    v20 = (char*)v3 + 0;
    v21 = *(double*)v20;
    v22 = v19 * v21;
    v23 = (char*)v2 + 0;
    v24 = *(double*)v23;
    v25 = (char*)v3 + 16;
    v26 = *(double*)v25;
    v27 = v24 * v26;
    v28 = v22 - v27;
    v29 = (char*)v4 + 8;
    *(double*)v29 = v28;
    v31 = (char*)v2 + 0;
    v32 = *(double*)v31;
    v33 = (char*)v3 + 8;
    v34 = *(double*)v33;
    v35 = v32 * v34;
    v36 = (char*)v2 + 8;
    v37 = *(double*)v36;
    v38 = (char*)v3 + 0;
    v39 = *(double*)v38;
    v40 = v37 * v39;
    v41 = v35 - v40;
    v42 = (char*)v4 + 16;
    *(double*)v42 = v41;
    v44 = (char*)v4 + 0;
    v45 = (char*)v0 + 0;
    v46 = *(double*)v44;
    *(double*)v45 = v46;
    v48 = (char*)v4 + 8;
    v49 = (char*)v0 + 8;
    v50 = *(double*)v48;
    *(double*)v49 = v50;
    v52 = (char*)v4 + 16;
    v53 = (char*)v0 + 16;
    v54 = *(double*)v52;
    *(double*)v53 = v54;
    p_call_stack_pop();
    return;
    return;
}

void quat(void* v0, void* v1, double v2, double v3, double v4, double v5) {
    p_call_stack_push("quat");
    void* v28;
    double v25;
    void* v6;
    void* v11;
    int64_t v12; /* placeholder for void */
    double v17;
    void* v7;
    double v21;
    void* v24;
    int64_t v14; /* placeholder for void */
    void* v15;
    void* v16;
    void* v27;
    int64_t v31; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    double v29;
    int64_t v22; /* placeholder for void */
    void* v9;
    void* v20;
    int64_t v30; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v23;
    void* v13;
    void* v19;
    int64_t v10; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    v6 = malloc(32);
    v7 = (char*)v6 + 0;
    *(double*)v7 = v2;
    v9 = (char*)v6 + 8;
    *(double*)v9 = v3;
    v11 = (char*)v6 + 16;
    *(double*)v11 = v4;
    v13 = (char*)v6 + 24;
    *(double*)v13 = v5;
    v15 = (char*)v6 + 0;
    v16 = (char*)v0 + 0;
    v17 = *(double*)v15;
    *(double*)v16 = v17;
    v19 = (char*)v6 + 8;
    v20 = (char*)v0 + 8;
    v21 = *(double*)v19;
    *(double*)v20 = v21;
    v23 = (char*)v6 + 16;
    v24 = (char*)v0 + 16;
    v25 = *(double*)v23;
    *(double*)v24 = v25;
    v27 = (char*)v6 + 24;
    v28 = (char*)v0 + 24;
    v29 = *(double*)v27;
    *(double*)v28 = v29;
    p_call_stack_pop();
    return;
    return;
}

int64_t HashMap_get_or(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("HashMap_get_or");
    int64_t v19; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    bool v61;
    int64_t v43;
    int64_t v84;
    void* v73;
    int64_t v90; /* placeholder for void */
    void* v29;
    int64_t v83; /* placeholder for void */
    int64_t v81;
    int64_t v92;
    void* v91;
    int64_t v65;
    int64_t v48; /* placeholder for void */
    int64_t v87;
    int64_t v94; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v95;
    void* v26;
    int64_t v45; /* placeholder for void */
    int64_t v80;
    int64_t v34; /* placeholder for void */
    int64_t v40;
    int64_t v37;
    bool v60;
    void* v67;
    int64_t v75; /* placeholder for void */
    void* v6;
    void* v70;
    int64_t v7; /* placeholder for void */
    int64_t v54;
    int64_t v59;
    void* v24;
    int64_t v4;
    int64_t v23;
    int64_t v25; /* placeholder for void */
    int64_t v39;
    int64_t v64; /* placeholder for void */
    int64_t v57;
    void* v9;
    int64_t v49; /* placeholder for void */
    int64_t v38;
    int64_t v8;
    void* v41;
    int64_t v72; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    int64_t v69;
    void* v85;
    int64_t v28;
    int64_t v62; /* placeholder for void */
    int64_t v74;
    int64_t v79;
    void* v12;
    int64_t v86; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v46;
    int64_t v22;
    void* v44;
    int64_t v63; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v50;
    int64_t v52; /* placeholder for void */
    int64_t v77;
    int64_t v96; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v5;
    bool v18;
    void* v88;
    int64_t v14; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v51;
    int64_t v89; /* placeholder for void */
    int64_t v58;
    int64_t v78;
    int64_t v55;
    int64_t v16;
    bool v56;
    int64_t v53; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v36;
    int64_t v42; /* placeholder for void */
    void* v47;
    int64_t v33;
    void* v82;
    int64_t v13;
    int64_t v31; /* placeholder for void */
    int64_t v17;
    int64_t v93; /* placeholder for void */
    void* v32;
    int64_t v66;
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

void* str_to_upper(void* v0, void* v1) {
    p_call_stack_push("str_to_upper");
    int64_t v2;
    int64_t v3;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_to_uppercase((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return (void*)v3;
    return 0;
}

int64_t Vec_capacity(void* v0, int64_t v1) {
    p_call_stack_push("Vec_capacity");
    int64_t v12;
    void* v8;
    int64_t v13; /* placeholder for void */
    int64_t v2;
    int64_t v9;
    void* v4;
    void* v6;
    int64_t v10; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v3;
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

void vec2_add(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("vec2_add");
    double v8;
    void* v20;
    double v16;
    int64_t v22; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    void* v5;
    void* v7;
    void* v17;
    void* v23;
    double v13;
    void* v10;
    void* v14;
    double v15;
    double v9;
    double v6;
    double v21;
    void* v24;
    double v25;
    void* v19;
    void* v12;
    int64_t v26; /* placeholder for void */
    void* v4;
    v4 = malloc(16);
    v5 = (char*)v2 + 0;
    v6 = *(double*)v5;
    v7 = (char*)v3 + 0;
    v8 = *(double*)v7;
    v9 = v6 + v8;
    v10 = (char*)v4 + 0;
    *(double*)v10 = v9;
    v12 = (char*)v2 + 8;
    v13 = *(double*)v12;
    v14 = (char*)v3 + 8;
    v15 = *(double*)v14;
    v16 = v13 + v15;
    v17 = (char*)v4 + 8;
    *(double*)v17 = v16;
    v19 = (char*)v4 + 0;
    v20 = (char*)v0 + 0;
    v21 = *(double*)v19;
    *(double*)v20 = v21;
    v23 = (char*)v4 + 8;
    v24 = (char*)v0 + 8;
    v25 = *(double*)v23;
    *(double*)v24 = v25;
    p_call_stack_pop();
    return;
    return;
}

int64_t p_abs(void* v0, int64_t v1) {
    p_call_stack_push("abs");
    int64_t v4; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v5;
    int64_t v2;
    bool v3;
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

int64_t String_to_uppercase(void* v0, int64_t v1) {
    p_call_stack_push("String_to_uppercase");
    void* v10;
    int64_t v99; /* placeholder for void */
    int64_t v53;
    bool v66;
    int64_t v81;
    void* v51;
    int64_t v20;
    int64_t v34;
    int64_t v73; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    void* v91;
    int64_t v9;
    int64_t v35;
    void* v54;
    void* v45;
    int64_t v95;
    int64_t v84; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v42;
    int64_t v71;
    void* v13;
    int64_t v55; /* placeholder for void */
    int64_t v14;
    int64_t v26;
    void* v76;
    int64_t v83; /* placeholder for void */
    void* v27;
    void* v48;
    int64_t v93; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v25;
    bool v3;
    int64_t v38;
    void* v31;
    int64_t v39;
    int64_t v50;
    int64_t v61;
    int64_t v69;
    int64_t v77;
    int64_t v90;
    void* v94;
    int64_t v65;
    int64_t v43;
    int64_t v15; /* placeholder for void */
    int64_t v47;
    int64_t v5;
    int64_t v24;
    int64_t v33; /* placeholder for void */
    int64_t v17;
    int64_t v68; /* placeholder for void */
    int64_t v80;
    void* v40;
    bool v63;
    void* v88;
    bool v67;
    int64_t v92; /* placeholder for void */
    void* v6;
    void* v7;
    int64_t v70;
    uint8_t v58;
    int64_t v18; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v30;
    int64_t v78; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v87;
    int64_t v8; /* placeholder for void */
    bool v36;
    int64_t v56; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v64;
    int64_t v23; /* placeholder for void */
    int64_t v72; /* placeholder for void */
    int64_t v89; /* placeholder for void */
    void* v28;
    int64_t v86;
    int64_t v85;
    int64_t v74; /* placeholder for void */
    int64_t v44;
    int64_t v98;
    int64_t v82;
    int64_t v2;
    void* v22;
    int64_t v12; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v21;
    int64_t v52; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    void* v57;
    int64_t v59; /* placeholder for void */
    int64_t v62;
    int64_t v79; /* placeholder for void */
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

int64_t String_split(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_split");
    int64_t v49; /* placeholder for void */
    int64_t v186; /* placeholder for void */
    bool v87;
    int64_t v23; /* placeholder for void */
    int64_t v181;
    void* v18;
    void* v48;
    void* v45;
    void* v28;
    int64_t v65;
    int64_t v91; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    int64_t v143;
    int64_t v21;
    int64_t v97; /* placeholder for void */
    uint8_t v79;
    int64_t v175; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    bool v98;
    int64_t v180; /* placeholder for void */
    int64_t v40;
    void* v75;
    int64_t v107;
    int64_t v170;
    int64_t v84; /* placeholder for void */
    int64_t v142;
    int64_t v51;
    void* v120;
    int64_t v136;
    int64_t v185; /* placeholder for void */
    void* v108;
    int64_t v43; /* placeholder for void */
    int64_t v71;
    int64_t v166;
    int64_t v151; /* placeholder for void */
    int64_t v4;
    int64_t v15; /* placeholder for void */
    int64_t v52;
    int64_t v16;
    int64_t v37; /* placeholder for void */
    int64_t v58;
    int64_t v133; /* placeholder for void */
    int64_t v134;
    int64_t v135;
    void* v72;
    bool v8;
    void* v150;
    int64_t v165;
    void* v5;
    int64_t v60; /* placeholder for void */
    int64_t v118;
    int64_t v115;
    int64_t v74;
    int64_t v148;
    int64_t v172; /* placeholder for void */
    int64_t v129; /* placeholder for void */
    int64_t v184; /* placeholder for void */
    void* v159;
    void* v69;
    void* v109;
    int64_t v112;
    int64_t v139; /* placeholder for void */
    int64_t v130;
    int64_t v141;
    int64_t v106;
    int64_t v47;
    void* v103;
    int64_t v56;
    int64_t v95;
    int64_t v124; /* placeholder for void */
    int64_t v183;
    int64_t v83; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v73; /* placeholder for void */
    int64_t v168;
    int64_t v13;
    int64_t v76; /* placeholder for void */
    int64_t v80; /* placeholder for void */
    int64_t v7;
    bool v42;
    void* v59;
    bool v11;
    int64_t v68;
    void* v177;
    int64_t v31;
    int64_t v102;
    int64_t v113;
    int64_t v86;
    int64_t v99; /* placeholder for void */
    int64_t v116; /* placeholder for void */
    int64_t v41;
    int64_t v157;
    int64_t v173;
    int64_t v61;
    int64_t v132;
    int64_t v153;
    int64_t v162;
    bool v44;
    void* v78;
    int64_t v29; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    int64_t v121; /* placeholder for void */
    int64_t v137; /* placeholder for void */
    int64_t v138;
    int64_t v12; /* placeholder for void */
    int64_t v163;
    int64_t v14; /* placeholder for void */
    int64_t v24;
    int64_t v34;
    int64_t v88; /* placeholder for void */
    int64_t v122;
    int64_t v152;
    void* v160;
    int64_t v25; /* placeholder for void */
    int64_t v93;
    int64_t v119;
    int64_t v20;
    int64_t v145; /* placeholder for void */
    void* v174;
    int64_t v158;
    void* v123;
    int64_t v57;
    int64_t v94;
    int64_t v117;
    int64_t v155; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v146; /* placeholder for void */
    int64_t v100;
    int64_t v9;
    bool v154;
    int64_t v39;
    int64_t v110; /* placeholder for void */
    int64_t v27;
    int64_t v3;
    int64_t v6; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v149;
    void* v171;
    int64_t v178;
    int64_t v140; /* placeholder for void */
    int64_t v188; /* placeholder for void */
    int64_t v85;
    int64_t v167; /* placeholder for void */
    int64_t v182;
    int64_t v90; /* placeholder for void */
    int64_t v30;
    int64_t v19; /* placeholder for void */
    int64_t v144; /* placeholder for void */
    int64_t v105;
    bool v53;
    int64_t v147;
    int64_t v38;
    void* v35;
    bool v10;
    void* v81;
    void* v32;
    void* v66;
    int64_t v128; /* placeholder for void */
    int64_t v156;
    int64_t v179; /* placeholder for void */
    int64_t v114;
    int64_t v46; /* placeholder for void */
    int64_t v62;
    int64_t v164;
    int64_t v54; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    int64_t v127;
    bool v89;
    int64_t v187;
    void* v63;
    int64_t v17;
    uint8_t v82;
    int64_t v111;
    void* v126;
    int64_t v161; /* placeholder for void */
    bool v22;
    int64_t v55;
    int64_t v77; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v101;
    int64_t v131;
    int64_t v169;
    int64_t v176; /* placeholder for void */
    int64_t v125; /* placeholder for void */
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

void Option_map(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Option_map");
    int64_t v12;
    void* v23;
    void* v40;
    int64_t v15;
    bool v30;
    int64_t v43; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    void* v41;
    void* v18;
    int64_t v8; /* placeholder for void */
    int64_t v38;
    int64_t v46; /* placeholder for void */
    bool v7;
    void* v22;
    void* v11;
    void* v27;
    int64_t v44; /* placeholder for void */
    void* v16;
    void* v37;
    int64_t v35; /* placeholder for void */
    int64_t v6;
    void* v13;
    void* v9;
    int64_t v21; /* placeholder for void */
    int64_t v33;
    void* v32;
    int64_t v45; /* placeholder for void */
    void* v19;
    int64_t v25; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    void* v34;
    int64_t v10;
    int64_t v5;
    int64_t v17; /* placeholder for void */
    int64_t v20;
    void* v36;
    void* v4;
    int64_t v28;
    int64_t v26; /* placeholder for void */
    int64_t v29;
    int64_t v39; /* placeholder for void */
    int64_t v42;
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
        if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "math_abs_i64") == 0) {
            v15 = math_abs_i64((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
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

void ByteBuffer_new(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("ByteBuffer_new");
    int64_t v14; /* placeholder for void */
    void* v12;
    void* v6;
    int64_t v4;
    void* v13;
    int64_t v10; /* placeholder for void */
    int64_t v17;
    void* v5;
    int64_t v19; /* placeholder for void */
    void* v9;
    void* v11;
    int64_t v8;
    void* v15;
    int64_t v18; /* placeholder for void */
    void* v16;
    void* v3;
    int64_t v7; /* placeholder for void */
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

int64_t HashMap_len(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_len");
    int64_t v5; /* placeholder for void */
    int64_t v14;
    void* v7;
    void* v4;
    int64_t v8; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    void* v10;
    int64_t v12; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v3;
    int64_t v15; /* placeholder for void */
    int64_t v11;
    int64_t v6;
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

void vec4_xyz(void* v0, void* v1, void* v2) {
    p_call_stack_push("vec4_xyz");
    void* v20;
    int64_t v15; /* placeholder for void */
    void* v24;
    void* v4;
    double v22;
    void* v14;
    void* v16;
    void* v12;
    double v5;
    void* v6;
    int64_t v27; /* placeholder for void */
    void* v25;
    void* v21;
    void* v17;
    int64_t v19; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    double v9;
    double v26;
    void* v3;
    double v18;
    void* v10;
    double v13;
    void* v8;
    int64_t v23; /* placeholder for void */
    v3 = malloc(24);
    v4 = (char*)v2 + 0;
    v5 = *(double*)v4;
    v6 = (char*)v3 + 0;
    *(double*)v6 = v5;
    v8 = (char*)v2 + 8;
    v9 = *(double*)v8;
    v10 = (char*)v3 + 8;
    *(double*)v10 = v9;
    v12 = (char*)v2 + 16;
    v13 = *(double*)v12;
    v14 = (char*)v3 + 16;
    *(double*)v14 = v13;
    v16 = (char*)v3 + 0;
    v17 = (char*)v0 + 0;
    v18 = *(double*)v16;
    *(double*)v17 = v18;
    v20 = (char*)v3 + 8;
    v21 = (char*)v0 + 8;
    v22 = *(double*)v20;
    *(double*)v21 = v22;
    v24 = (char*)v3 + 16;
    v25 = (char*)v0 + 16;
    v26 = *(double*)v24;
    *(double*)v25 = v26;
    p_call_stack_pop();
    return;
    return;
}

void vec4_add(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("vec4_add");
    void* v17;
    void* v12;
    int64_t v18; /* placeholder for void */
    void* v4;
    void* v24;
    double v27;
    void* v34;
    double v23;
    void* v5;
    int64_t v40; /* placeholder for void */
    void* v28;
    double v13;
    void* v7;
    void* v37;
    void* v45;
    int64_t v25; /* placeholder for void */
    double v30;
    int64_t v11; /* placeholder for void */
    double v8;
    void* v33;
    void* v21;
    double v16;
    double v39;
    void* v41;
    void* v46;
    void* v42;
    void* v10;
    void* v26;
    int64_t v32; /* placeholder for void */
    double v35;
    double v20;
    int64_t v36; /* placeholder for void */
    void* v38;
    int64_t v44; /* placeholder for void */
    double v47;
    void* v19;
    double v22;
    double v9;
    void* v31;
    double v43;
    double v29;
    int64_t v48; /* placeholder for void */
    double v15;
    void* v14;
    double v6;
    int64_t v49; /* placeholder for void */
    v4 = malloc(32);
    v5 = (char*)v2 + 0;
    v6 = *(double*)v5;
    v7 = (char*)v3 + 0;
    v8 = *(double*)v7;
    v9 = v6 + v8;
    v10 = (char*)v4 + 0;
    *(double*)v10 = v9;
    v12 = (char*)v2 + 8;
    v13 = *(double*)v12;
    v14 = (char*)v3 + 8;
    v15 = *(double*)v14;
    v16 = v13 + v15;
    v17 = (char*)v4 + 8;
    *(double*)v17 = v16;
    v19 = (char*)v2 + 16;
    v20 = *(double*)v19;
    v21 = (char*)v3 + 16;
    v22 = *(double*)v21;
    v23 = v20 + v22;
    v24 = (char*)v4 + 16;
    *(double*)v24 = v23;
    v26 = (char*)v2 + 24;
    v27 = *(double*)v26;
    v28 = (char*)v3 + 24;
    v29 = *(double*)v28;
    v30 = v27 + v29;
    v31 = (char*)v4 + 24;
    *(double*)v31 = v30;
    v33 = (char*)v4 + 0;
    v34 = (char*)v0 + 0;
    v35 = *(double*)v33;
    *(double*)v34 = v35;
    v37 = (char*)v4 + 8;
    v38 = (char*)v0 + 8;
    v39 = *(double*)v37;
    *(double*)v38 = v39;
    v41 = (char*)v4 + 16;
    v42 = (char*)v0 + 16;
    v43 = *(double*)v41;
    *(double*)v42 = v43;
    v45 = (char*)v4 + 24;
    v46 = (char*)v0 + 24;
    v47 = *(double*)v45;
    *(double*)v46 = v47;
    p_call_stack_pop();
    return;
    return;
}

void StringBuilder_append_int(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("StringBuilder_append_int");
    void* v3;
    void* v5;
    void* v6;
    int64_t v8; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v11;
    void* v10;
    void* v4;
    int64_t v13;
    int64_t v12;
    void* v9;
    int64_t v14; /* placeholder for void */
    void* v7;
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

int64_t ByteBuffer_read_byte(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("ByteBuffer_read_byte");
    bool v4;
    int64_t v10;
    void* v5;
    int64_t v18; /* placeholder for void */
    bool v8;
    int64_t v9; /* placeholder for void */
    int64_t v6;
    int64_t v3;
    int64_t v11;
    int64_t v12; /* placeholder for void */
    int64_t v16;
    int64_t v13; /* placeholder for void */
    void* v14;
    void* v15;
    int64_t v17;
    bool v7;
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

bool char_is_lower(void* v0, int64_t v1) {
    p_call_stack_push("char_is_lower");
    int64_t v4;
    bool v6;
    bool v5;
    int64_t v2;
    bool v3;
    int64_t v7; /* placeholder for void */
    v2 = 97LL;
    v3 = v1 >= v2;
    v4 = 122LL;
    v5 = v1 <= v4;
    v6 = v3 && v5;
    p_call_stack_pop();
    return v6;
    return 0;
}

void StringBuilder_new(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_new");
    void* v3;
    int64_t v17; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    void* v7;
    void* v11;
    int64_t v5; /* placeholder for void */
    void* v13;
    void* v4;
    int64_t v8; /* placeholder for void */
    void* v14;
    int64_t v12; /* placeholder for void */
    int64_t v6;
    void* v10;
    int64_t v15;
    void* v2;
    void* v9;
    v2 = malloc(16);
    v3 = _str0;
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

double vec4_dot(void* v0, void* v1, void* v2) {
    p_call_stack_push("vec4_dot");
    double v18;
    void* v8;
    double v24;
    void* v3;
    double v17;
    double v21;
    void* v5;
    double v4;
    double v6;
    double v9;
    double v11;
    double v13;
    void* v14;
    void* v16;
    double v12;
    void* v10;
    void* v20;
    double v15;
    double v23;
    double v25;
    double v7;
    double v19;
    int64_t v26; /* placeholder for void */
    void* v22;
    v3 = (char*)v1 + 0;
    v4 = *(double*)v3;
    v5 = (char*)v2 + 0;
    v6 = *(double*)v5;
    v7 = v4 * v6;
    v8 = (char*)v1 + 8;
    v9 = *(double*)v8;
    v10 = (char*)v2 + 8;
    v11 = *(double*)v10;
    v12 = v9 * v11;
    v13 = v7 + v12;
    v14 = (char*)v1 + 16;
    v15 = *(double*)v14;
    v16 = (char*)v2 + 16;
    v17 = *(double*)v16;
    v18 = v15 * v17;
    v19 = v13 + v18;
    v20 = (char*)v1 + 24;
    v21 = *(double*)v20;
    v22 = (char*)v2 + 24;
    v23 = *(double*)v22;
    v24 = v21 * v23;
    v25 = v19 + v24;
    p_call_stack_pop();
    return v25;
    return 0;
}

void HashMap_clear(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_clear");
    int64_t v54;
    int64_t v57; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v64;
    int64_t v6; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v35;
    int64_t v34;
    int64_t v63;
    int64_t v68; /* placeholder for void */
    int64_t v2;
    int64_t v19; /* placeholder for void */
    int64_t v8;
    void* v27;
    void* v52;
    int64_t v51;
    int64_t v12; /* placeholder for void */
    int64_t v47;
    int64_t v48;
    int64_t v10; /* placeholder for void */
    bool v33;
    int64_t v49;
    int64_t v3;
    int64_t v16; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v43;
    int64_t v13;
    int64_t v9; /* placeholder for void */
    int64_t v20;
    bool v37;
    int64_t v42; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v39;
    int64_t v38; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    void* v11;
    void* v18;
    int64_t v50;
    int64_t v69; /* placeholder for void */
    void* v24;
    int64_t v25;
    void* v40;
    bool v45;
    int64_t v46; /* placeholder for void */
    int64_t v61; /* placeholder for void */
    int64_t v31;
    int64_t v66; /* placeholder for void */
    int64_t v44;
    void* v55;
    int64_t v5; /* placeholder for void */
    void* v58;
    int64_t v59;
    int64_t v23; /* placeholder for void */
    int64_t v14;
    int64_t v26; /* placeholder for void */
    int64_t v32;
    int64_t v53; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    void* v4;
    void* v7;
    int64_t v17;
    int64_t v62;
    int64_t v65; /* placeholder for void */
    void* v21;
    void* v15;
    int64_t v28;
    bool v36;
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

double vec3_dot(void* v0, void* v1, void* v2) {
    p_call_stack_push("vec3_dot");
    double v18;
    double v7;
    double v9;
    void* v16;
    double v12;
    void* v8;
    int64_t v20; /* placeholder for void */
    double v4;
    void* v5;
    double v6;
    double v19;
    double v15;
    void* v10;
    double v13;
    double v11;
    void* v3;
    double v17;
    void* v14;
    v3 = (char*)v1 + 0;
    v4 = *(double*)v3;
    v5 = (char*)v2 + 0;
    v6 = *(double*)v5;
    v7 = v4 * v6;
    v8 = (char*)v1 + 8;
    v9 = *(double*)v8;
    v10 = (char*)v2 + 8;
    v11 = *(double*)v10;
    v12 = v9 * v11;
    v13 = v7 + v12;
    v14 = (char*)v1 + 16;
    v15 = *(double*)v14;
    v16 = (char*)v2 + 16;
    v17 = *(double*)v16;
    v18 = v15 * v17;
    v19 = v13 + v18;
    p_call_stack_pop();
    return v19;
    return 0;
}

void vec3_normalize(void* v0, void* v1, void* v2) {
    p_call_stack_push("vec3_normalize");
    void* v5;
    double v15;
    void* v58;
    int64_t v6; /* placeholder for void */
    void* v22;
    void* v62;
    void* v63;
    double v45;
    double v23;
    double v37;
    double v50;
    void* v59;
    bool v9;
    double v43;
    double v49;
    int64_t v57; /* placeholder for void */
    int64_t v65; /* placeholder for void */
    void* v48;
    void* v21;
    int64_t v32; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    double v44;
    void* v52;
    double v39;
    double v12;
    int64_t v14; /* placeholder for void */
    double v64;
    int64_t v61; /* placeholder for void */
    void* v46;
    void* v40;
    int64_t v10; /* placeholder for void */
    void* v13;
    int64_t v20; /* placeholder for void */
    void* v11;
    void* v26;
    int64_t v34; /* placeholder for void */
    void* v42;
    void* v30;
    void* v35;
    int64_t v3;
    double v60;
    int64_t v28; /* placeholder for void */
    double v56;
    int64_t v66; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    void* v16;
    double v18;
    double v4;
    double v31;
    double v38;
    int64_t v47; /* placeholder for void */
    void* v19;
    double v27;
    int64_t v33; /* placeholder for void */
    void* v36;
    double v8;
    void* v54;
    int64_t v17; /* placeholder for void */
    void* v55;
    double v7;
    double v51;
    void* v29;
    void* v25;
    int64_t v41; /* placeholder for void */
    v3 = 0LL;
    v4 = (double)vec3_length((void*)v3, v2);
    v5 = malloc(8);
    *(double*)v5 = v4;
    v7 = *(double*)v5;
    v8 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v9 = v7 == v8;
    if (v9) goto bb1;
    else goto bb2;
  bb1:
    v11 = malloc(24);
    v12 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v13 = (char*)v11 + 0;
    *(double*)v13 = v12;
    v15 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v16 = (char*)v11 + 8;
    *(double*)v16 = v15;
    v18 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v19 = (char*)v11 + 16;
    *(double*)v19 = v18;
    v21 = (char*)v11 + 0;
    v22 = (char*)v0 + 0;
    v23 = *(double*)v21;
    *(double*)v22 = v23;
    v25 = (char*)v11 + 8;
    v26 = (char*)v0 + 8;
    v27 = *(double*)v25;
    *(double*)v26 = v27;
    v29 = (char*)v11 + 16;
    v30 = (char*)v0 + 16;
    v31 = *(double*)v29;
    *(double*)v30 = v31;
    p_call_stack_pop();
    return;
  bb2:
    goto bb3;
  bb3:
    v35 = malloc(24);
    v36 = (char*)v2 + 0;
    v37 = *(double*)v36;
    v38 = *(double*)v5;
    v39 = v37 / v38;
    v40 = (char*)v35 + 0;
    *(double*)v40 = v39;
    v42 = (char*)v2 + 8;
    v43 = *(double*)v42;
    v44 = *(double*)v5;
    v45 = v43 / v44;
    v46 = (char*)v35 + 8;
    *(double*)v46 = v45;
    v48 = (char*)v2 + 16;
    v49 = *(double*)v48;
    v50 = *(double*)v5;
    v51 = v49 / v50;
    v52 = (char*)v35 + 16;
    *(double*)v52 = v51;
    v54 = (char*)v35 + 0;
    v55 = (char*)v0 + 0;
    v56 = *(double*)v54;
    *(double*)v55 = v56;
    v58 = (char*)v35 + 8;
    v59 = (char*)v0 + 8;
    v60 = *(double*)v58;
    *(double*)v59 = v60;
    v62 = (char*)v35 + 16;
    v63 = (char*)v0 + 16;
    v64 = *(double*)v62;
    *(double*)v63 = v64;
    p_call_stack_pop();
    return;
    return;
}

int64_t clamp(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("clamp");
    int64_t v12; /* placeholder for void */
    bool v8;
    int64_t v5; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    bool v4;
    int64_t v7; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v11; /* placeholder for void */
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

int64_t String_to_lowercase(void* v0, int64_t v1) {
    p_call_stack_push("String_to_lowercase");
    int64_t v9;
    void* v13;
    int64_t v64;
    int64_t v71;
    int64_t v80;
    int64_t v78; /* placeholder for void */
    int64_t v5;
    int64_t v4; /* placeholder for void */
    int64_t v85;
    int64_t v89; /* placeholder for void */
    int64_t v81;
    int64_t v23; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v98;
    int64_t v41; /* placeholder for void */
    int64_t v44;
    int64_t v70;
    int64_t v86;
    int64_t v93; /* placeholder for void */
    int64_t v26;
    int64_t v97; /* placeholder for void */
    void* v31;
    bool v36;
    int64_t v82;
    int64_t v30;
    int64_t v59; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    void* v94;
    int64_t v11; /* placeholder for void */
    int64_t v50;
    int64_t v49; /* placeholder for void */
    int64_t v38;
    bool v66;
    void* v91;
    void* v22;
    int64_t v87;
    int64_t v19; /* placeholder for void */
    void* v48;
    bool v3;
    int64_t v34;
    void* v40;
    int64_t v90;
    int64_t v96; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    bool v63;
    int64_t v53;
    int64_t v72; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v65;
    int64_t v73; /* placeholder for void */
    int64_t v42;
    uint8_t v58;
    int64_t v69;
    int64_t v77;
    void* v54;
    int64_t v2;
    int64_t v37; /* placeholder for void */
    void* v57;
    int64_t v61;
    int64_t v52; /* placeholder for void */
    int64_t v24;
    int64_t v62;
    bool v67;
    int64_t v74; /* placeholder for void */
    int64_t v21;
    void* v6;
    int64_t v33; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    void* v76;
    int64_t v16; /* placeholder for void */
    void* v51;
    int64_t v35;
    int64_t v14;
    int64_t v15; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    int64_t v39;
    int64_t v20;
    void* v88;
    void* v45;
    int64_t v99; /* placeholder for void */
    void* v28;
    int64_t v55; /* placeholder for void */
    int64_t v17;
    int64_t v95;
    int64_t v43;
    int64_t v84; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    void* v27;
    void* v10;
    void* v7;
    int64_t v25;
    int64_t v47;
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

void Vec_set(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("Vec_set");
    int64_t v49; /* placeholder for void */
    int64_t v36;
    int64_t v45; /* placeholder for void */
    void* v10;
    void* v32;
    void* v8;
    int64_t v5;
    int64_t v11; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    void* v43;
    int64_t v9; /* placeholder for void */
    int64_t v33;
    bool v24;
    int64_t v23;
    int64_t v41; /* placeholder for void */
    void* v46;
    void* v6;
    int64_t v22; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    void* v26;
    void* v19;
    void* v12;
    int64_t v7; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v13;
    int64_t v16;
    bool v17;
    void* v30;
    int64_t v4;
    int64_t v18; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v37;
    void* v40;
    int64_t v28; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v38;
    int64_t v48; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v39;
    int64_t v29; /* placeholder for void */
    int64_t v42;
    int64_t v47; /* placeholder for void */
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
    v19 = _str1;
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
    v26 = _str1;
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

int64_t p_min(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("min");
    int64_t v5; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    bool v3;
    v3 = v1 < v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return v1;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return v2;
    return 0;
}

bool Vec_contains(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("Vec_contains");
    int64_t v3;
    void* v34;
    int64_t v8; /* placeholder for void */
    void* v5;
    int64_t v18;
    void* v22;
    int64_t v10; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    void* v37;
    int64_t v53;
    int64_t v41; /* placeholder for void */
    int64_t v36;
    int64_t v12;
    void* v17;
    int64_t v42; /* placeholder for void */
    int64_t v54;
    int64_t v44;
    int64_t v31;
    int64_t v55;
    int64_t v59; /* placeholder for void */
    void* v40;
    bool v58;
    int64_t v26;
    void* v11;
    int64_t v29;
    int64_t v35; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    bool v27;
    int64_t v23; /* placeholder for void */
    int64_t v39;
    int64_t v47;
    int64_t v52; /* placeholder for void */
    void* v7;
    void* v9;
    int64_t v21;
    int64_t v4;
    int64_t v30;
    void* v43;
    int64_t v49; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    bool v50;
    int64_t v51; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    void* v15;
    int64_t v46; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    bool v48;
    int64_t v33;
    int64_t v25;
    int64_t v32;
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

bool String_ends_with(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_ends_with");
    int64_t v76; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    bool v89;
    int64_t v33; /* placeholder for void */
    int64_t v77;
    int64_t v50;
    uint8_t v74;
    void* v18;
    int64_t v39;
    int64_t v32; /* placeholder for void */
    void* v40;
    int64_t v47;
    int64_t v49;
    int64_t v21;
    int64_t v20;
    int64_t v26; /* placeholder for void */
    int64_t v3;
    int64_t v17;
    int64_t v41; /* placeholder for void */
    void* v64;
    void* v37;
    void* v55;
    void* v73;
    bool v9;
    void* v14;
    int64_t v48;
    int64_t v57;
    int64_t v63;
    bool v24;
    int64_t v30; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    void* v61;
    int64_t v65; /* placeholder for void */
    int64_t v80; /* placeholder for void */
    bool v4;
    int64_t v56; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v27;
    bool v29;
    int64_t v43;
    int64_t v88; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v36;
    int64_t v66;
    int64_t v69; /* placeholder for void */
    int64_t v35;
    void* v58;
    bool v81;
    int64_t v10; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v85;
    int64_t v90; /* placeholder for void */
    bool v45;
    int64_t v82; /* placeholder for void */
    int64_t v87; /* placeholder for void */
    int64_t v13;
    bool v31;
    int64_t v5;
    int64_t v34;
    int64_t v44;
    int64_t v60;
    int64_t v42; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    void* v70;
    int64_t v84;
    int64_t v86;
    bool v79;
    uint8_t v71;
    int64_t v75; /* placeholder for void */
    int64_t v53;
    bool v22;
    int64_t v78;
    int64_t v19; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v12;
    int64_t v16;
    bool v6;
    int64_t v8; /* placeholder for void */
    void* v51;
    int64_t v54;
    int64_t v28;
    int64_t v59; /* placeholder for void */
    void* v67;
    int64_t v72; /* placeholder for void */
    bool v7;
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

int64_t math_abs_i64(void* v0, int64_t v1) {
    p_call_stack_push("math_abs_i64");
    int64_t v2;
    int64_t v7; /* placeholder for void */
    bool v3;
    int64_t v5;
    int64_t v4; /* placeholder for void */
    int64_t v8; /* placeholder for void */
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

void* str_bytes(void* v0, void* v1) {
    p_call_stack_push("str_bytes");
    int64_t v9; /* placeholder for void */
    int64_t v25;
    int64_t v10;
    int64_t v7;
    int64_t v15;
    int64_t v36; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v22;
    int64_t v14;
    int64_t v37;
    int64_t v39;
    void* v33;
    int64_t v5; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    void* v23;
    void* v29;
    void* v11;
    int64_t v24; /* placeholder for void */
    int64_t v35;
    void* v8;
    int64_t v13; /* placeholder for void */
    bool v16;
    void* v42;
    void* v4;
    int64_t v43; /* placeholder for void */
    int64_t v2;
    void* v3;
    int64_t v12; /* placeholder for void */
    void* v20;
    int64_t v27; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v34;
    int64_t v41; /* placeholder for void */
    int64_t v6;
    int64_t v32; /* placeholder for void */
    int64_t v19;
    int64_t v21; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v38;
    uint8_t v30;
    int64_t v18;
    void* v26;
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

int64_t String_from(void* v0, int64_t v1) {
    p_call_stack_push("String_from");
    int64_t v46; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v34;
    int64_t v38; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v2;
    void* v6;
    int64_t v14;
    int64_t v5;
    void* v10;
    int64_t v33; /* placeholder for void */
    int64_t v36;
    int64_t v21;
    int64_t v45; /* placeholder for void */
    void* v13;
    int64_t v12; /* placeholder for void */
    int64_t v20;
    void* v7;
    int64_t v8; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    void* v28;
    int64_t v15; /* placeholder for void */
    int64_t v17;
    int64_t v26;
    void* v37;
    int64_t v44;
    int64_t v47;
    bool v3;
    int64_t v35;
    int64_t v39;
    int64_t v25;
    int64_t v9;
    int64_t v30;
    void* v27;
    int64_t v31;
    int64_t v19; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    void* v40;
    void* v22;
    int64_t v24;
    void* v43;
    int64_t v32;
    int64_t v18; /* placeholder for void */
    int64_t v42; /* placeholder for void */
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

int64_t p_str_length(void* v0, int64_t v1) {
    p_call_stack_push("p_str_length");
    bool v24;
    int64_t v4; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v14;
    int64_t v42; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    void* v11;
    int64_t v31;
    uint8_t v37;
    int64_t v30;
    void* v17;
    int64_t v35; /* placeholder for void */
    uint8_t v18;
    bool v3;
    int64_t v32;
    int64_t v33; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v22;
    int64_t v23;
    int64_t v13;
    int64_t v9; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v34;
    int64_t v29; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v26;
    int64_t v5;
    int64_t v28;
    void* v19;
    int64_t v39; /* placeholder for void */
    void* v8;
    int64_t v10;
    int64_t v15; /* placeholder for void */
    int64_t v27;
    int64_t v2;
    int64_t v12; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    void* v36;
    int64_t v41;
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

void* str_from_bytes(void* v0, void* v1) {
    p_call_stack_push("str_from_bytes");
    int64_t v20;
    int64_t v14;
    int64_t v26;
    int64_t v27; /* placeholder for void */
    int64_t v19;
    int64_t v25;
    void* v21;
    void* v29;
    int64_t v28; /* placeholder for void */
    void* v3;
    int64_t v13;
    int64_t v24;
    int64_t v30; /* placeholder for void */
    void* v17;
    int64_t v11; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v8;
    int64_t v12; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    bool v15;
    int64_t v5;
    void* v2;
    int64_t v23; /* placeholder for void */
    void* v6;
    int64_t v7; /* placeholder for void */
    void* v22;
    int64_t v18;
    void* v10;
    int64_t v9;
    v2 = _str0;
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

void vec2_scale(void* v0, void* v1, void* v2, double v3) {
    p_call_stack_push("vec2_scale");
    int64_t v23; /* placeholder for void */
    void* v16;
    void* v10;
    int64_t v22; /* placeholder for void */
    void* v8;
    void* v15;
    void* v5;
    int64_t v9; /* placeholder for void */
    double v17;
    int64_t v14; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    void* v19;
    void* v20;
    void* v4;
    double v7;
    double v6;
    double v21;
    double v12;
    void* v13;
    double v11;
    v4 = malloc(16);
    v5 = (char*)v2 + 0;
    v6 = *(double*)v5;
    v7 = v6 * v3;
    v8 = (char*)v4 + 0;
    *(double*)v8 = v7;
    v10 = (char*)v2 + 8;
    v11 = *(double*)v10;
    v12 = v11 * v3;
    v13 = (char*)v4 + 8;
    *(double*)v13 = v12;
    v15 = (char*)v4 + 0;
    v16 = (char*)v0 + 0;
    v17 = *(double*)v15;
    *(double*)v16 = v17;
    v19 = (char*)v4 + 8;
    v20 = (char*)v0 + 8;
    v21 = *(double*)v19;
    *(double*)v20 = v21;
    p_call_stack_pop();
    return;
    return;
}

void vec3_sub(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("vec3_sub");
    int64_t v29; /* placeholder for void */
    void* v34;
    double v13;
    void* v17;
    void* v24;
    double v16;
    int64_t v37; /* placeholder for void */
    double v23;
    void* v31;
    void* v5;
    void* v26;
    void* v14;
    double v28;
    double v32;
    double v8;
    void* v7;
    int64_t v38; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    void* v10;
    double v20;
    int64_t v33; /* placeholder for void */
    void* v19;
    int64_t v11; /* placeholder for void */
    void* v12;
    int64_t v25; /* placeholder for void */
    void* v30;
    double v6;
    void* v4;
    void* v35;
    void* v27;
    double v36;
    double v22;
    void* v21;
    double v9;
    double v15;
    v4 = malloc(24);
    v5 = (char*)v2 + 0;
    v6 = *(double*)v5;
    v7 = (char*)v3 + 0;
    v8 = *(double*)v7;
    v9 = v6 - v8;
    v10 = (char*)v4 + 0;
    *(double*)v10 = v9;
    v12 = (char*)v2 + 8;
    v13 = *(double*)v12;
    v14 = (char*)v3 + 8;
    v15 = *(double*)v14;
    v16 = v13 - v15;
    v17 = (char*)v4 + 8;
    *(double*)v17 = v16;
    v19 = (char*)v2 + 16;
    v20 = *(double*)v19;
    v21 = (char*)v3 + 16;
    v22 = *(double*)v21;
    v23 = v20 - v22;
    v24 = (char*)v4 + 16;
    *(double*)v24 = v23;
    v26 = (char*)v4 + 0;
    v27 = (char*)v0 + 0;
    v28 = *(double*)v26;
    *(double*)v27 = v28;
    v30 = (char*)v4 + 8;
    v31 = (char*)v0 + 8;
    v32 = *(double*)v30;
    *(double*)v31 = v32;
    v34 = (char*)v4 + 16;
    v35 = (char*)v0 + 16;
    v36 = *(double*)v34;
    *(double*)v35 = v36;
    p_call_stack_pop();
    return;
    return;
}

double vec2_length(void* v0, void* v1) {
    p_call_stack_push("vec2_length");
    double v3;
    double v10;
    void* v7;
    void* v2;
    double v5;
    void* v9;
    double v8;
    void* v4;
    int64_t v14; /* placeholder for void */
    double v6;
    double v12;
    double v13;
    double v11;
    v2 = (char*)v1 + 0;
    v3 = *(double*)v2;
    v4 = (char*)v1 + 0;
    v5 = *(double*)v4;
    v6 = v3 * v5;
    v7 = (char*)v1 + 8;
    v8 = *(double*)v7;
    v9 = (char*)v1 + 8;
    v10 = *(double*)v9;
    v11 = v8 * v10;
    v12 = v6 + v11;
    v13 = (double)pledge_sqrt(v12);
    p_call_stack_pop();
    return v13;
    return 0;
}

void* create_allocator(void* v0, void* v1) {
    p_call_stack_push("create_allocator");
    void* v2;
    int64_t v3; /* placeholder for void */
    v2 = p_allocator_create(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void vec3_scale(void* v0, void* v1, void* v2, double v3) {
    p_call_stack_push("vec3_scale");
    void* v5;
    void* v28;
    void* v4;
    int64_t v14; /* placeholder for void */
    double v22;
    void* v20;
    int64_t v9; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    void* v15;
    int64_t v32; /* placeholder for void */
    double v7;
    void* v8;
    double v6;
    void* v10;
    double v17;
    double v16;
    int64_t v27; /* placeholder for void */
    void* v24;
    int64_t v23; /* placeholder for void */
    void* v25;
    void* v29;
    void* v18;
    double v26;
    double v11;
    void* v13;
    void* v21;
    double v30;
    double v12;
    int64_t v19; /* placeholder for void */
    v4 = malloc(24);
    v5 = (char*)v2 + 0;
    v6 = *(double*)v5;
    v7 = v6 * v3;
    v8 = (char*)v4 + 0;
    *(double*)v8 = v7;
    v10 = (char*)v2 + 8;
    v11 = *(double*)v10;
    v12 = v11 * v3;
    v13 = (char*)v4 + 8;
    *(double*)v13 = v12;
    v15 = (char*)v2 + 16;
    v16 = *(double*)v15;
    v17 = v16 * v3;
    v18 = (char*)v4 + 16;
    *(double*)v18 = v17;
    v20 = (char*)v4 + 0;
    v21 = (char*)v0 + 0;
    v22 = *(double*)v20;
    *(double*)v21 = v22;
    v24 = (char*)v4 + 8;
    v25 = (char*)v0 + 8;
    v26 = *(double*)v24;
    *(double*)v25 = v26;
    v28 = (char*)v4 + 16;
    v29 = (char*)v0 + 16;
    v30 = *(double*)v28;
    *(double*)v29 = v30;
    p_call_stack_pop();
    return;
    return;
}

double lerp(void* v0, double v1, double v2, double v3) {
    p_call_stack_push("lerp");
    double v4;
    double v5;
    int64_t v7; /* placeholder for void */
    double v6;
    v4 = v2 - v1;
    v5 = v4 * v3;
    v6 = v1 + v5;
    p_call_stack_pop();
    return v6;
    return 0;
}

void HashMap_grow(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_grow");
    int64_t v21; /* placeholder for void */
    int64_t v65;
    int64_t v104; /* placeholder for void */
    int64_t v123;
    int64_t v47; /* placeholder for void */
    int64_t v19;
    int64_t v212; /* placeholder for void */
    int64_t v80;
    int64_t v269;
    int64_t v227; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v252;
    void* v53;
    int64_t v245;
    int64_t v51;
    int64_t v64; /* placeholder for void */
    int64_t v200;
    int64_t v173;
    int64_t v275;
    int64_t v131;
    int64_t v233;
    int64_t v188; /* placeholder for void */
    int64_t v225;
    void* v23;
    int64_t v48; /* placeholder for void */
    int64_t v210;
    int64_t v240; /* placeholder for void */
    void* v124;
    int64_t v219; /* placeholder for void */
    int64_t v253; /* placeholder for void */
    int64_t v126; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v89; /* placeholder for void */
    int64_t v140;
    int64_t v264; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    int64_t v136;
    int64_t v207;
    bool v41;
    void* v106;
    int64_t v178;
    int64_t v28; /* placeholder for void */
    int64_t v205;
    int64_t v76;
    int64_t v172;
    void* v90;
    int64_t v157; /* placeholder for void */
    int64_t v9;
    int64_t v132;
    int64_t v175; /* placeholder for void */
    int64_t v134;
    int64_t v152; /* placeholder for void */
    bool v67;
    int64_t v100; /* placeholder for void */
    int64_t v112; /* placeholder for void */
    bool v174;
    int64_t v71;
    bool v111;
    int64_t v155;
    void* v14;
    int64_t v83;
    int64_t v272;
    int64_t v38; /* placeholder for void */
    void* v199;
    void* v151;
    int64_t v163;
    int64_t v33;
    bool v258;
    int64_t v49;
    int64_t v149; /* placeholder for void */
    int64_t v192; /* placeholder for void */
    int64_t v195;
    int64_t v241; /* placeholder for void */
    int64_t v128;
    int64_t v35; /* placeholder for void */
    void* v154;
    int64_t v55;
    int64_t v160;
    int64_t v237;
    int64_t v223; /* placeholder for void */
    int64_t v273; /* placeholder for void */
    int64_t v164;
    int64_t v102;
    int64_t v236; /* placeholder for void */
    int64_t v86;
    int64_t v168;
    int64_t v251; /* placeholder for void */
    int64_t v177;
    int64_t v16;
    void* v268;
    void* v77;
    void* v166;
    int64_t v158;
    void* v4;
    bool v138;
    int64_t v254; /* placeholder for void */
    bool v133;
    int64_t v68; /* placeholder for void */
    int64_t v244;
    int64_t v260;
    int64_t v278; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v39;
    int64_t v40;
    int64_t v146; /* placeholder for void */
    int64_t v50;
    int64_t v257;
    int64_t v18; /* placeholder for void */
    void* v32;
    int64_t v150;
    int64_t v183;
    int64_t v69;
    int64_t v256;
    int64_t v203;
    int64_t v242; /* placeholder for void */
    int64_t v113;
    void* v118;
    void* v181;
    int64_t v3;
    int64_t v246; /* placeholder for void */
    int64_t v220;
    void* v6;
    int64_t v276; /* placeholder for void */
    int64_t v180;
    int64_t v144;
    int64_t v270; /* placeholder for void */
    int64_t v122; /* placeholder for void */
    int64_t v36;
    int64_t v79;
    int64_t v119; /* placeholder for void */
    void* v29;
    int64_t v116;
    int64_t v247; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v94;
    int64_t v120;
    int64_t v230; /* placeholder for void */
    void* v214;
    int64_t v231; /* placeholder for void */
    void* v169;
    int64_t v108; /* placeholder for void */
    void* v62;
    int64_t v259; /* placeholder for void */
    int64_t v91;
    int64_t v82;
    int64_t v179;
    int64_t v239;
    void* v271;
    void* v87;
    int64_t v135;
    int64_t v189; /* placeholder for void */
    void* v37;
    void* v20;
    int64_t v249;
    int64_t v24; /* placeholder for void */
    int64_t v12;
    void* v17;
    int64_t v191;
    int64_t v243;
    int64_t v263; /* placeholder for void */
    int64_t v198; /* placeholder for void */
    int64_t v238;
    int64_t v45;
    bool v196;
    int64_t v31; /* placeholder for void */
    int64_t v81;
    int64_t v194;
    void* v52;
    int64_t v88; /* placeholder for void */
    int64_t v204;
    int64_t v229; /* placeholder for void */
    int64_t v27;
    int64_t v130; /* placeholder for void */
    int64_t v221;
    int64_t v66;
    int64_t v7; /* placeholder for void */
    int64_t v255; /* placeholder for void */
    int64_t v261; /* placeholder for void */
    int64_t v267; /* placeholder for void */
    int64_t v156; /* placeholder for void */
    void* v127;
    int64_t v142;
    bool v250;
    int64_t v13;
    int64_t v11; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    void* v103;
    int64_t v213; /* placeholder for void */
    void* v187;
    int64_t v2;
    int64_t v30;
    int64_t v99;
    void* v184;
    int64_t v232;
    int64_t v109;
    int64_t v61;
    int64_t v141;
    bool v137;
    int64_t v209; /* placeholder for void */
    int64_t v262; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    void* v93;
    int64_t v165;
    void* v84;
    int64_t v129; /* placeholder for void */
    int64_t v97;
    void* v59;
    int64_t v70;
    int64_t v167; /* placeholder for void */
    int64_t v193; /* placeholder for void */
    int64_t v115;
    int64_t v125; /* placeholder for void */
    int64_t v107; /* placeholder for void */
    void* v208;
    int64_t v159;
    int64_t v56;
    int64_t v46; /* placeholder for void */
    int64_t v171; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v197; /* placeholder for void */
    int64_t v110;
    int64_t v176;
    int64_t v228;
    int64_t v44;
    int64_t v182; /* placeholder for void */
    int64_t v8;
    int64_t v139; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    void* v265;
    int64_t v206;
    int64_t v162; /* placeholder for void */
    void* v190;
    int64_t v215;
    int64_t v235; /* placeholder for void */
    int64_t v266;
    int64_t v42; /* placeholder for void */
    int64_t v57;
    int64_t v73;
    void* v211;
    int64_t v43;
    void* v74;
    int64_t v202; /* placeholder for void */
    int64_t v216; /* placeholder for void */
    int64_t v85; /* placeholder for void */
    void* v26;
    int64_t v222;
    void* v58;
    void* v10;
    int64_t v98;
    int64_t v114;
    int64_t v147;
    int64_t v185; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    int64_t v186;
    int64_t v72;
    void* v121;
    int64_t v217; /* placeholder for void */
    int64_t v224;
    int64_t v248;
    void* v145;
    int64_t v117;
    int64_t v5; /* placeholder for void */
    int64_t v22;
    int64_t v105;
    int64_t v153; /* placeholder for void */
    int64_t v201; /* placeholder for void */
    bool v226;
    void* v274;
    int64_t v143;
    int64_t v277; /* placeholder for void */
    void* v148;
    int64_t v170; /* placeholder for void */
    int64_t v218; /* placeholder for void */
    int64_t v234;
    void* v161;
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

void* HashMap_to_str(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_to_str");
    int64_t v22; /* placeholder for void */
    int64_t v68;
    int64_t v25;
    int64_t v48;
    int64_t v97;
    int64_t v59;
    int64_t v103;
    int64_t v62;
    int64_t v32;
    int64_t v135; /* placeholder for void */
    void* v92;
    int64_t v89; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    void* v118;
    int64_t v124; /* placeholder for void */
    int64_t v133; /* placeholder for void */
    void* v35;
    int64_t v40;
    int64_t v81; /* placeholder for void */
    int64_t v104;
    int64_t v93; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    int64_t v42;
    int64_t v21; /* placeholder for void */
    int64_t v67;
    int64_t v31;
    void* v76;
    int64_t v66;
    int64_t v127;
    bool v29;
    int64_t v54;
    void* v112;
    int64_t v19;
    int64_t v107;
    void* v109;
    int64_t v111; /* placeholder for void */
    int64_t v47;
    int64_t v74; /* placeholder for void */
    void* v77;
    int64_t v16;
    bool v84;
    int64_t v73;
    int64_t v79;
    int64_t v102;
    int64_t v36; /* placeholder for void */
    int64_t v28;
    int64_t v86;
    int64_t v57;
    int64_t v52;
    int64_t v10; /* placeholder for void */
    int64_t v61;
    int64_t v39;
    int64_t v41;
    int64_t v44; /* placeholder for void */
    void* v116;
    int64_t v23;
    int64_t v45;
    int64_t v125;
    int64_t v14; /* placeholder for void */
    int64_t v27;
    int64_t v58;
    int64_t v65; /* placeholder for void */
    int64_t v96;
    int64_t v99;
    int64_t v115; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v6;
    int64_t v64; /* placeholder for void */
    int64_t v37;
    int64_t v11;
    void* v4;
    int64_t v5; /* placeholder for void */
    void* v100;
    int64_t v108;
    void* v130;
    int64_t v24;
    int64_t v3;
    bool v88;
    int64_t v49;
    int64_t v55;
    void* v131;
    void* v9;
    int64_t v30; /* placeholder for void */
    int64_t v82;
    int64_t v7;
    int64_t v46;
    int64_t v70; /* placeholder for void */
    int64_t v72;
    void* v134;
    int64_t v18; /* placeholder for void */
    bool v50;
    int64_t v80; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    void* v117;
    int64_t v98;
    int64_t v8;
    int64_t v15;
    int64_t v94; /* placeholder for void */
    int64_t v34;
    bool v17;
    int64_t v60;
    void* v90;
    void* v113;
    void* v114;
    void* v91;
    int64_t v13; /* placeholder for void */
    int64_t v119; /* placeholder for void */
    void* v123;
    void* v120;
    void* v132;
    void* v110;
    void* v122;
    int64_t v53;
    int64_t v2;
    void* v20;
    int64_t v85; /* placeholder for void */
    void* v12;
    int64_t v71;
    int64_t v95; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v129; /* placeholder for void */
    int64_t v128; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    void* v43;
    int64_t v121;
    int64_t v38;
    int64_t v126;
    int64_t v83;
    int64_t v87;
    int64_t v33;
    int64_t v26;
    int64_t v78; /* placeholder for void */
    void* v105;
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
    v76 = _str2;
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
    v131 = _str3;
    v132 = p_str_concat(v130, v131);
    *(void**)v77 = (void*)v132;
    v134 = *(void**)v77;
    p_call_stack_pop();
    return v134;
  bb13:
    v90 = *(void**)v77;
    v91 = _str4;
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
    v117 = _str5;
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

bool Result_is_ok(void* v0, void* v1) {
    p_call_stack_push("Result_is_ok");
    int64_t v22; /* placeholder for void */
    void* v11;
    void* v2;
    bool v5;
    bool v14;
    int64_t v13;
    int64_t v15; /* placeholder for void */
    bool v9;
    int64_t v20; /* placeholder for void */
    bool v21;
    int64_t v6; /* placeholder for void */
    int64_t v12;
    int64_t v3;
    void* v7;
    int64_t v8;
    int64_t v10; /* placeholder for void */
    void* v16;
    int64_t v17;
    int64_t v19; /* placeholder for void */
    bool v18;
    int64_t v4;
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

void mat4_mul(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("mat4_mul");
    double v356;
    void* v372;
    void* v316;
    double v345;
    double v14;
    double v443;
    void* v112;
    void* v10;
    double v326;
    double v284;
    void* v62;
    double v383;
    double v11;
    void* v105;
    void* v232;
    double v84;
    void* v222;
    double v320;
    void* v343;
    double v407;
    double v89;
    void* v378;
    double v195;
    void* v193;
    int64_t v129; /* placeholder for void */
    void* v66;
    void* v303;
    void* v374;
    double v8;
    double v156;
    int64_t v452; /* placeholder for void */
    double v50;
    double v115;
    void* v47;
    void* v172;
    double v48;
    double v246;
    int64_t v416; /* placeholder for void */
    double v70;
    double v321;
    double v98;
    double v25;
    int64_t v408; /* placeholder for void */
    double v73;
    void* v135;
    double v227;
    double v402;
    void* v207;
    void* v330;
    double v77;
    void* v216;
    double v269;
    void* v53;
    double v9;
    double v200;
    void* v28;
    double v171;
    void* v224;
    double v401;
    double v263;
    double v6;
    double v294;
    double v65;
    void* v433;
    double v388;
    double v177;
    double v34;
    double v251;
    double v152;
    int64_t v468; /* placeholder for void */
    void* v110;
    double v90;
    double v119;
    int64_t v432; /* placeholder for void */
    double v290;
    double v369;
    double v238;
    double v314;
    void* v118;
    double v121;
    void* v274;
    double v270;
    double v239;
    void* v349;
    void* v410;
    void* v187;
    int64_t v204; /* placeholder for void */
    double v113;
    double v151;
    void* v282;
    void* v253;
    void* v322;
    void* v385;
    void* v462;
    double v339;
    double v45;
    void* v360;
    double v19;
    double v111;
    double v146;
    double v186;
    double v276;
    void* v297;
    void* v426;
    double v334;
    void* v153;
    double v134;
    void* v337;
    double v365;
    void* v147;
    void* v366;
    void* v429;
    void* v124;
    int64_t v229; /* placeholder for void */
    void* v285;
    void* v235;
    double v165;
    double v359;
    void* v387;
    void* v441;
    void* v97;
    double v169;
    double v36;
    double v267;
    void* v60;
    void* v249;
    void* v16;
    double v17;
    void* v197;
    double v348;
    int64_t v412; /* placeholder for void */
    double v102;
    void* v5;
    double v136;
    int64_t v444; /* placeholder for void */
    int64_t v154; /* placeholder for void */
    void* v357;
    double v375;
    double v344;
    double v61;
    double v64;
    double v194;
    double v88;
    void* v162;
    double v211;
    double v144;
    double v120;
    double v15;
    double v265;
    double v333;
    int64_t v279; /* placeholder for void */
    double v467;
    void* v68;
    void* v24;
    void* v180;
    double v311;
    double v350;
    void* v122;
    double v275;
    void* v4;
    double v208;
    double v83;
    double v252;
    void* v99;
    double v13;
    double v221;
    void* v116;
    double v51;
    void* v155;
    double v106;
    double v109;
    double v240;
    void* v406;
    void* v397;
    double v58;
    void* v243;
    double v163;
    double v302;
    void* v461;
    void* v37;
    void* v49;
    void* v85;
    void* v168;
    double v170;
    double v271;
    void* v178;
    double v117;
    void* v453;
    double v94;
    double v363;
    void* v380;
    double v223;
    double v114;
    double v323;
    double v142;
    void* v241;
    void* v72;
    double v33;
    void* v418;
    double v140;
    void* v299;
    double v398;
    double v346;
    double v71;
    double v150;
    double v419;
    void* v107;
    double v370;
    int64_t v420; /* placeholder for void */
    int64_t v464; /* placeholder for void */
    void* v57;
    double v52;
    double v309;
    double v245;
    double v63;
    double v127;
    void* v332;
    void* v422;
    void* v185;
    double v396;
    double v342;
    void* v130;
    void* v278;
    double v376;
    double v69;
    void* v437;
    int64_t v469; /* placeholder for void */
    double v92;
    void* v272;
    double v23;
    double v244;
    void* v260;
    int64_t v460; /* placeholder for void */
    double v26;
    double v75;
    double v234;
    double v261;
    double v38;
    double v411;
    void* v324;
    void* v87;
    void* v91;
    void* v228;
    double v183;
    double v451;
    int64_t v304; /* placeholder for void */
    void* v310;
    double v325;
    void* v237;
    double v258;
    void* v182;
    double v125;
    void* v143;
    double v373;
    void* v132;
    void* v405;
    double v201;
    void* v403;
    int64_t v179; /* placeholder for void */
    void* v22;
    void* v82;
    int64_t v436; /* placeholder for void */
    void* v205;
    int64_t v404; /* placeholder for void */
    int64_t v379; /* placeholder for void */
    void* v160;
    void* v368;
    double v286;
    double v196;
    double v386;
    double v336;
    void* v434;
    double v264;
    double v202;
    double v225;
    double v101;
    double v308;
    void* v30;
    double v317;
    int64_t v329; /* placeholder for void */
    double v377;
    void* v391;
    double v447;
    double v95;
    void* v287;
    double v281;
    double v131;
    double v295;
    void* v417;
    double v167;
    int64_t v254; /* placeholder for void */
    double v301;
    void* v103;
    double v139;
    void* v230;
    double v175;
    void* v409;
    void* v210;
    int64_t v424; /* placeholder for void */
    void* v137;
    void* v293;
    double v39;
    void* v305;
    double v427;
    double v371;
    void* v157;
    double v233;
    void* v32;
    void* v262;
    double v313;
    double v158;
    void* v353;
    double v338;
    double v59;
    double v306;
    double v27;
    double v248;
    void* v166;
    double v236;
    double v181;
    void* v307;
    double v364;
    void* v7;
    void* v355;
    double v384;
    double v42;
    double v298;
    double v283;
    void* v255;
    void* v291;
    double v340;
    double v206;
    double v126;
    void* v445;
    void* v149;
    void* v454;
    double v176;
    double v435;
    void* v55;
    int64_t v440; /* placeholder for void */
    double v108;
    double v395;
    int64_t v54; /* placeholder for void */
    double v415;
    double v189;
    double v219;
    double v226;
    void* v382;
    void* v35;
    double v192;
    double v315;
    double v381;
    double v423;
    double v184;
    void* v335;
    double v319;
    double v217;
    double v164;
    double v256;
    void* v425;
    void* v466;
    void* v414;
    double v431;
    double v292;
    void* v465;
    double v213;
    double v20;
    int64_t v104; /* placeholder for void */
    void* v266;
    double v96;
    double v190;
    double v259;
    void* v449;
    double v296;
    void* v430;
    void* v328;
    void* v438;
    double v145;
    double v133;
    int64_t v456; /* placeholder for void */
    double v188;
    void* v199;
    void* v212;
    double v215;
    double v327;
    void* v341;
    void* v280;
    double v123;
    void* v141;
    double v288;
    void* v93;
    int64_t v354; /* placeholder for void */
    double v394;
    double v392;
    double v159;
    double v44;
    double v67;
    double v81;
    double v198;
    void* v268;
    void* v399;
    void* v458;
    double v361;
    double v86;
    void* v421;
    double v173;
    void* v18;
    int64_t v29; /* placeholder for void */
    void* v78;
    double v161;
    void* v174;
    double v352;
    double v400;
    double v273;
    double v231;
    double v331;
    double v459;
    double v289;
    void* v218;
    double v242;
    double v463;
    void* v203;
    double v46;
    double v439;
    void* v80;
    double v40;
    int64_t v448; /* placeholder for void */
    void* v247;
    void* v413;
    void* v347;
    void* v393;
    void* v442;
    double v390;
    void* v450;
    int64_t v428; /* placeholder for void */
    void* v43;
    double v367;
    void* v446;
    int64_t v79; /* placeholder for void */
    double v250;
    void* v312;
    double v351;
    double v358;
    double v209;
    double v300;
    double v56;
    void* v318;
    double v138;
    void* v457;
    double v214;
    void* v74;
    double v76;
    double v220;
    void* v257;
    void* v41;
    double v277;
    void* v362;
    double v21;
    void* v128;
    double v455;
    void* v191;
    double v148;
    void* v12;
    double v389;
    double v31;
    double v100;
    v4 = malloc(128);
    v5 = (char*)v2 + 0;
    v6 = *(double*)v5;
    v7 = (char*)v3 + 0;
    v8 = *(double*)v7;
    v9 = v6 * v8;
    v10 = (char*)v2 + 8;
    v11 = *(double*)v10;
    v12 = (char*)v3 + 32;
    v13 = *(double*)v12;
    v14 = v11 * v13;
    v15 = v9 + v14;
    v16 = (char*)v2 + 16;
    v17 = *(double*)v16;
    v18 = (char*)v3 + 64;
    v19 = *(double*)v18;
    v20 = v17 * v19;
    v21 = v15 + v20;
    v22 = (char*)v2 + 24;
    v23 = *(double*)v22;
    v24 = (char*)v3 + 96;
    v25 = *(double*)v24;
    v26 = v23 * v25;
    v27 = v21 + v26;
    v28 = (char*)v4 + 0;
    *(double*)v28 = v27;
    v30 = (char*)v2 + 0;
    v31 = *(double*)v30;
    v32 = (char*)v3 + 8;
    v33 = *(double*)v32;
    v34 = v31 * v33;
    v35 = (char*)v2 + 8;
    v36 = *(double*)v35;
    v37 = (char*)v3 + 40;
    v38 = *(double*)v37;
    v39 = v36 * v38;
    v40 = v34 + v39;
    v41 = (char*)v2 + 16;
    v42 = *(double*)v41;
    v43 = (char*)v3 + 72;
    v44 = *(double*)v43;
    v45 = v42 * v44;
    v46 = v40 + v45;
    v47 = (char*)v2 + 24;
    v48 = *(double*)v47;
    v49 = (char*)v3 + 104;
    v50 = *(double*)v49;
    v51 = v48 * v50;
    v52 = v46 + v51;
    v53 = (char*)v4 + 8;
    *(double*)v53 = v52;
    v55 = (char*)v2 + 0;
    v56 = *(double*)v55;
    v57 = (char*)v3 + 16;
    v58 = *(double*)v57;
    v59 = v56 * v58;
    v60 = (char*)v2 + 8;
    v61 = *(double*)v60;
    v62 = (char*)v3 + 48;
    v63 = *(double*)v62;
    v64 = v61 * v63;
    v65 = v59 + v64;
    v66 = (char*)v2 + 16;
    v67 = *(double*)v66;
    v68 = (char*)v3 + 80;
    v69 = *(double*)v68;
    v70 = v67 * v69;
    v71 = v65 + v70;
    v72 = (char*)v2 + 24;
    v73 = *(double*)v72;
    v74 = (char*)v3 + 112;
    v75 = *(double*)v74;
    v76 = v73 * v75;
    v77 = v71 + v76;
    v78 = (char*)v4 + 16;
    *(double*)v78 = v77;
    v80 = (char*)v2 + 0;
    v81 = *(double*)v80;
    v82 = (char*)v3 + 24;
    v83 = *(double*)v82;
    v84 = v81 * v83;
    v85 = (char*)v2 + 8;
    v86 = *(double*)v85;
    v87 = (char*)v3 + 56;
    v88 = *(double*)v87;
    v89 = v86 * v88;
    v90 = v84 + v89;
    v91 = (char*)v2 + 16;
    v92 = *(double*)v91;
    v93 = (char*)v3 + 88;
    v94 = *(double*)v93;
    v95 = v92 * v94;
    v96 = v90 + v95;
    v97 = (char*)v2 + 24;
    v98 = *(double*)v97;
    v99 = (char*)v3 + 120;
    v100 = *(double*)v99;
    v101 = v98 * v100;
    v102 = v96 + v101;
    v103 = (char*)v4 + 24;
    *(double*)v103 = v102;
    v105 = (char*)v2 + 32;
    v106 = *(double*)v105;
    v107 = (char*)v3 + 0;
    v108 = *(double*)v107;
    v109 = v106 * v108;
    v110 = (char*)v2 + 40;
    v111 = *(double*)v110;
    v112 = (char*)v3 + 32;
    v113 = *(double*)v112;
    v114 = v111 * v113;
    v115 = v109 + v114;
    v116 = (char*)v2 + 48;
    v117 = *(double*)v116;
    v118 = (char*)v3 + 64;
    v119 = *(double*)v118;
    v120 = v117 * v119;
    v121 = v115 + v120;
    v122 = (char*)v2 + 56;
    v123 = *(double*)v122;
    v124 = (char*)v3 + 96;
    v125 = *(double*)v124;
    v126 = v123 * v125;
    v127 = v121 + v126;
    v128 = (char*)v4 + 32;
    *(double*)v128 = v127;
    v130 = (char*)v2 + 32;
    v131 = *(double*)v130;
    v132 = (char*)v3 + 8;
    v133 = *(double*)v132;
    v134 = v131 * v133;
    v135 = (char*)v2 + 40;
    v136 = *(double*)v135;
    v137 = (char*)v3 + 40;
    v138 = *(double*)v137;
    v139 = v136 * v138;
    v140 = v134 + v139;
    v141 = (char*)v2 + 48;
    v142 = *(double*)v141;
    v143 = (char*)v3 + 72;
    v144 = *(double*)v143;
    v145 = v142 * v144;
    v146 = v140 + v145;
    v147 = (char*)v2 + 56;
    v148 = *(double*)v147;
    v149 = (char*)v3 + 104;
    v150 = *(double*)v149;
    v151 = v148 * v150;
    v152 = v146 + v151;
    v153 = (char*)v4 + 40;
    *(double*)v153 = v152;
    v155 = (char*)v2 + 32;
    v156 = *(double*)v155;
    v157 = (char*)v3 + 16;
    v158 = *(double*)v157;
    v159 = v156 * v158;
    v160 = (char*)v2 + 40;
    v161 = *(double*)v160;
    v162 = (char*)v3 + 48;
    v163 = *(double*)v162;
    v164 = v161 * v163;
    v165 = v159 + v164;
    v166 = (char*)v2 + 48;
    v167 = *(double*)v166;
    v168 = (char*)v3 + 80;
    v169 = *(double*)v168;
    v170 = v167 * v169;
    v171 = v165 + v170;
    v172 = (char*)v2 + 56;
    v173 = *(double*)v172;
    v174 = (char*)v3 + 112;
    v175 = *(double*)v174;
    v176 = v173 * v175;
    v177 = v171 + v176;
    v178 = (char*)v4 + 48;
    *(double*)v178 = v177;
    v180 = (char*)v2 + 32;
    v181 = *(double*)v180;
    v182 = (char*)v3 + 24;
    v183 = *(double*)v182;
    v184 = v181 * v183;
    v185 = (char*)v2 + 40;
    v186 = *(double*)v185;
    v187 = (char*)v3 + 56;
    v188 = *(double*)v187;
    v189 = v186 * v188;
    v190 = v184 + v189;
    v191 = (char*)v2 + 48;
    v192 = *(double*)v191;
    v193 = (char*)v3 + 88;
    v194 = *(double*)v193;
    v195 = v192 * v194;
    v196 = v190 + v195;
    v197 = (char*)v2 + 56;
    v198 = *(double*)v197;
    v199 = (char*)v3 + 120;
    v200 = *(double*)v199;
    v201 = v198 * v200;
    v202 = v196 + v201;
    v203 = (char*)v4 + 56;
    *(double*)v203 = v202;
    v205 = (char*)v2 + 64;
    v206 = *(double*)v205;
    v207 = (char*)v3 + 0;
    v208 = *(double*)v207;
    v209 = v206 * v208;
    v210 = (char*)v2 + 72;
    v211 = *(double*)v210;
    v212 = (char*)v3 + 32;
    v213 = *(double*)v212;
    v214 = v211 * v213;
    v215 = v209 + v214;
    v216 = (char*)v2 + 80;
    v217 = *(double*)v216;
    v218 = (char*)v3 + 64;
    v219 = *(double*)v218;
    v220 = v217 * v219;
    v221 = v215 + v220;
    v222 = (char*)v2 + 88;
    v223 = *(double*)v222;
    v224 = (char*)v3 + 96;
    v225 = *(double*)v224;
    v226 = v223 * v225;
    v227 = v221 + v226;
    v228 = (char*)v4 + 64;
    *(double*)v228 = v227;
    v230 = (char*)v2 + 64;
    v231 = *(double*)v230;
    v232 = (char*)v3 + 8;
    v233 = *(double*)v232;
    v234 = v231 * v233;
    v235 = (char*)v2 + 72;
    v236 = *(double*)v235;
    v237 = (char*)v3 + 40;
    v238 = *(double*)v237;
    v239 = v236 * v238;
    v240 = v234 + v239;
    v241 = (char*)v2 + 80;
    v242 = *(double*)v241;
    v243 = (char*)v3 + 72;
    v244 = *(double*)v243;
    v245 = v242 * v244;
    v246 = v240 + v245;
    v247 = (char*)v2 + 88;
    v248 = *(double*)v247;
    v249 = (char*)v3 + 104;
    v250 = *(double*)v249;
    v251 = v248 * v250;
    v252 = v246 + v251;
    v253 = (char*)v4 + 72;
    *(double*)v253 = v252;
    v255 = (char*)v2 + 64;
    v256 = *(double*)v255;
    v257 = (char*)v3 + 16;
    v258 = *(double*)v257;
    v259 = v256 * v258;
    v260 = (char*)v2 + 72;
    v261 = *(double*)v260;
    v262 = (char*)v3 + 48;
    v263 = *(double*)v262;
    v264 = v261 * v263;
    v265 = v259 + v264;
    v266 = (char*)v2 + 80;
    v267 = *(double*)v266;
    v268 = (char*)v3 + 80;
    v269 = *(double*)v268;
    v270 = v267 * v269;
    v271 = v265 + v270;
    v272 = (char*)v2 + 88;
    v273 = *(double*)v272;
    v274 = (char*)v3 + 112;
    v275 = *(double*)v274;
    v276 = v273 * v275;
    v277 = v271 + v276;
    v278 = (char*)v4 + 80;
    *(double*)v278 = v277;
    v280 = (char*)v2 + 64;
    v281 = *(double*)v280;
    v282 = (char*)v3 + 24;
    v283 = *(double*)v282;
    v284 = v281 * v283;
    v285 = (char*)v2 + 72;
    v286 = *(double*)v285;
    v287 = (char*)v3 + 56;
    v288 = *(double*)v287;
    v289 = v286 * v288;
    v290 = v284 + v289;
    v291 = (char*)v2 + 80;
    v292 = *(double*)v291;
    v293 = (char*)v3 + 88;
    v294 = *(double*)v293;
    v295 = v292 * v294;
    v296 = v290 + v295;
    v297 = (char*)v2 + 88;
    v298 = *(double*)v297;
    v299 = (char*)v3 + 120;
    v300 = *(double*)v299;
    v301 = v298 * v300;
    v302 = v296 + v301;
    v303 = (char*)v4 + 88;
    *(double*)v303 = v302;
    v305 = (char*)v2 + 96;
    v306 = *(double*)v305;
    v307 = (char*)v3 + 0;
    v308 = *(double*)v307;
    v309 = v306 * v308;
    v310 = (char*)v2 + 104;
    v311 = *(double*)v310;
    v312 = (char*)v3 + 32;
    v313 = *(double*)v312;
    v314 = v311 * v313;
    v315 = v309 + v314;
    v316 = (char*)v2 + 112;
    v317 = *(double*)v316;
    v318 = (char*)v3 + 64;
    v319 = *(double*)v318;
    v320 = v317 * v319;
    v321 = v315 + v320;
    v322 = (char*)v2 + 120;
    v323 = *(double*)v322;
    v324 = (char*)v3 + 96;
    v325 = *(double*)v324;
    v326 = v323 * v325;
    v327 = v321 + v326;
    v328 = (char*)v4 + 96;
    *(double*)v328 = v327;
    v330 = (char*)v2 + 96;
    v331 = *(double*)v330;
    v332 = (char*)v3 + 8;
    v333 = *(double*)v332;
    v334 = v331 * v333;
    v335 = (char*)v2 + 104;
    v336 = *(double*)v335;
    v337 = (char*)v3 + 40;
    v338 = *(double*)v337;
    v339 = v336 * v338;
    v340 = v334 + v339;
    v341 = (char*)v2 + 112;
    v342 = *(double*)v341;
    v343 = (char*)v3 + 72;
    v344 = *(double*)v343;
    v345 = v342 * v344;
    v346 = v340 + v345;
    v347 = (char*)v2 + 120;
    v348 = *(double*)v347;
    v349 = (char*)v3 + 104;
    v350 = *(double*)v349;
    v351 = v348 * v350;
    v352 = v346 + v351;
    v353 = (char*)v4 + 104;
    *(double*)v353 = v352;
    v355 = (char*)v2 + 96;
    v356 = *(double*)v355;
    v357 = (char*)v3 + 16;
    v358 = *(double*)v357;
    v359 = v356 * v358;
    v360 = (char*)v2 + 104;
    v361 = *(double*)v360;
    v362 = (char*)v3 + 48;
    v363 = *(double*)v362;
    v364 = v361 * v363;
    v365 = v359 + v364;
    v366 = (char*)v2 + 112;
    v367 = *(double*)v366;
    v368 = (char*)v3 + 80;
    v369 = *(double*)v368;
    v370 = v367 * v369;
    v371 = v365 + v370;
    v372 = (char*)v2 + 120;
    v373 = *(double*)v372;
    v374 = (char*)v3 + 112;
    v375 = *(double*)v374;
    v376 = v373 * v375;
    v377 = v371 + v376;
    v378 = (char*)v4 + 112;
    *(double*)v378 = v377;
    v380 = (char*)v2 + 96;
    v381 = *(double*)v380;
    v382 = (char*)v3 + 24;
    v383 = *(double*)v382;
    v384 = v381 * v383;
    v385 = (char*)v2 + 104;
    v386 = *(double*)v385;
    v387 = (char*)v3 + 56;
    v388 = *(double*)v387;
    v389 = v386 * v388;
    v390 = v384 + v389;
    v391 = (char*)v2 + 112;
    v392 = *(double*)v391;
    v393 = (char*)v3 + 88;
    v394 = *(double*)v393;
    v395 = v392 * v394;
    v396 = v390 + v395;
    v397 = (char*)v2 + 120;
    v398 = *(double*)v397;
    v399 = (char*)v3 + 120;
    v400 = *(double*)v399;
    v401 = v398 * v400;
    v402 = v396 + v401;
    v403 = (char*)v4 + 120;
    *(double*)v403 = v402;
    v405 = (char*)v4 + 0;
    v406 = (char*)v0 + 0;
    v407 = *(double*)v405;
    *(double*)v406 = v407;
    v409 = (char*)v4 + 8;
    v410 = (char*)v0 + 8;
    v411 = *(double*)v409;
    *(double*)v410 = v411;
    v413 = (char*)v4 + 16;
    v414 = (char*)v0 + 16;
    v415 = *(double*)v413;
    *(double*)v414 = v415;
    v417 = (char*)v4 + 24;
    v418 = (char*)v0 + 24;
    v419 = *(double*)v417;
    *(double*)v418 = v419;
    v421 = (char*)v4 + 32;
    v422 = (char*)v0 + 32;
    v423 = *(double*)v421;
    *(double*)v422 = v423;
    v425 = (char*)v4 + 40;
    v426 = (char*)v0 + 40;
    v427 = *(double*)v425;
    *(double*)v426 = v427;
    v429 = (char*)v4 + 48;
    v430 = (char*)v0 + 48;
    v431 = *(double*)v429;
    *(double*)v430 = v431;
    v433 = (char*)v4 + 56;
    v434 = (char*)v0 + 56;
    v435 = *(double*)v433;
    *(double*)v434 = v435;
    v437 = (char*)v4 + 64;
    v438 = (char*)v0 + 64;
    v439 = *(double*)v437;
    *(double*)v438 = v439;
    v441 = (char*)v4 + 72;
    v442 = (char*)v0 + 72;
    v443 = *(double*)v441;
    *(double*)v442 = v443;
    v445 = (char*)v4 + 80;
    v446 = (char*)v0 + 80;
    v447 = *(double*)v445;
    *(double*)v446 = v447;
    v449 = (char*)v4 + 88;
    v450 = (char*)v0 + 88;
    v451 = *(double*)v449;
    *(double*)v450 = v451;
    v453 = (char*)v4 + 96;
    v454 = (char*)v0 + 96;
    v455 = *(double*)v453;
    *(double*)v454 = v455;
    v457 = (char*)v4 + 104;
    v458 = (char*)v0 + 104;
    v459 = *(double*)v457;
    *(double*)v458 = v459;
    v461 = (char*)v4 + 112;
    v462 = (char*)v0 + 112;
    v463 = *(double*)v461;
    *(double*)v462 = v463;
    v465 = (char*)v4 + 120;
    v466 = (char*)v0 + 120;
    v467 = *(double*)v465;
    *(double*)v466 = v467;
    p_call_stack_pop();
    return;
    return;
}

void Vec_push(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("Vec_push");
    int64_t v62;
    int64_t v98; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    bool v29;
    int64_t v65;
    int64_t v46;
    int64_t v100; /* placeholder for void */
    void* v47;
    int64_t v84;
    int64_t v30; /* placeholder for void */
    int64_t v102;
    int64_t v33; /* placeholder for void */
    int64_t v87;
    int64_t v43; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    void* v11;
    int64_t v3;
    int64_t v60;
    int64_t v85; /* placeholder for void */
    int64_t v68;
    int64_t v114; /* placeholder for void */
    int64_t v118;
    int64_t v24;
    void* v83;
    int64_t v93;
    int64_t v25; /* placeholder for void */
    void* v51;
    int64_t v58;
    int64_t v91; /* placeholder for void */
    int64_t v88;
    int64_t v119;
    int64_t v96;
    int64_t v69;
    void* v32;
    void* v48;
    int64_t v10; /* placeholder for void */
    void* v123;
    int64_t v134; /* placeholder for void */
    int64_t v130;
    bool v94;
    int64_t v128; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v115;
    int64_t v6; /* placeholder for void */
    void* v17;
    int64_t v54;
    int64_t v125; /* placeholder for void */
    void* v66;
    int64_t v42; /* placeholder for void */
    int64_t v75;
    void* v81;
    void* v63;
    void* v13;
    int64_t v90; /* placeholder for void */
    int64_t v113; /* placeholder for void */
    void* v132;
    int64_t v106; /* placeholder for void */
    int64_t v137; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    void* v126;
    int64_t v4;
    int64_t v103; /* placeholder for void */
    int64_t v55;
    int64_t v107; /* placeholder for void */
    void* v20;
    void* v5;
    int64_t v136;
    void* v79;
    void* v15;
    int64_t v19; /* placeholder for void */
    int64_t v80;
    int64_t v108; /* placeholder for void */
    bool v36;
    int64_t v22; /* placeholder for void */
    int64_t v121; /* placeholder for void */
    void* v135;
    int64_t v138; /* placeholder for void */
    int64_t v139; /* placeholder for void */
    void* v9;
    void* v23;
    int64_t v21;
    int64_t v27;
    int64_t v38;
    int64_t v64; /* placeholder for void */
    void* v104;
    void* v76;
    int64_t v129;
    int64_t v122;
    void* v120;
    int64_t v99; /* placeholder for void */
    int64_t v59;
    int64_t v8;
    int64_t v39;
    int64_t v86; /* placeholder for void */
    void* v101;
    int64_t v44;
    int64_t v97; /* placeholder for void */
    void* v111;
    int64_t v124; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v116;
    void* v73;
    int64_t v52; /* placeholder for void */
    int64_t v127; /* placeholder for void */
    int64_t v117;
    int64_t v89;
    int64_t v61;
    int64_t v67; /* placeholder for void */
    int64_t v34;
    int64_t v74; /* placeholder for void */
    int64_t v72;
    int64_t v45;
    int64_t v131;
    int64_t v12; /* placeholder for void */
    int64_t v28;
    int64_t v31;
    int64_t v16; /* placeholder for void */
    int64_t v40;
    int64_t v82; /* placeholder for void */
    int64_t v70;
    int64_t v110; /* placeholder for void */
    int64_t v7;
    int64_t v50;
    int64_t v105;
    int64_t v18;
    int64_t v133; /* placeholder for void */
    int64_t v35;
    int64_t v71;
    int64_t v95; /* placeholder for void */
    void* v14;
    int64_t v41; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    bool v56;
    int64_t v92;
    int64_t v109; /* placeholder for void */
    int64_t v112;
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

int64_t String_bytes(void* v0, int64_t v1) {
    p_call_stack_push("String_bytes");
    int64_t v2;
    int64_t v4; /* placeholder for void */
    int64_t v3;
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

void quat_rotate_vec3(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("quat_rotate_vec3");
    double v46;
    double v69;
    double v77;
    double v102;
    double v78;
    double v39;
    double v76;
    double v62;
    void* v119;
    double v54;
    double v117;
    double v13;
    int64_t v118; /* placeholder for void */
    void* v4;
    int64_t v11; /* placeholder for void */
    void* v22;
    double v25;
    int64_t v123; /* placeholder for void */
    double v59;
    int64_t v42; /* placeholder for void */
    void* v18;
    double v71;
    void* v94;
    double v55;
    void* v52;
    int64_t v114; /* placeholder for void */
    double v98;
    double v87;
    void* v28;
    int64_t v53; /* placeholder for void */
    void* v109;
    int64_t v64; /* placeholder for void */
    double v70;
    double v35;
    int64_t v110; /* placeholder for void */
    double v72;
    double v74;
    double v85;
    void* v12;
    void* v24;
    double v89;
    void* v14;
    double v34;
    int64_t v15; /* placeholder for void */
    double v103;
    int64_t v31; /* placeholder for void */
    double v91;
    double v97;
    double v40;
    double v100;
    double v99;
    void* v79;
    double v51;
    void* v20;
    double v90;
    double v68;
    double v47;
    double v9;
    void* v63;
    double v108;
    double v32;
    double v113;
    void* v116;
    double v93;
    double v21;
    double v43;
    double v88;
    double v49;
    double v82;
    double v67;
    int64_t v95; /* placeholder for void */
    double v101;
    void* v6;
    double v104;
    double v48;
    int64_t v7; /* placeholder for void */
    double v58;
    int64_t v80; /* placeholder for void */
    double v121;
    double v37;
    void* v8;
    void* v65;
    void* v120;
    void* v111;
    double v92;
    double v45;
    void* v30;
    double v36;
    double v96;
    double v5;
    double v66;
    double v44;
    double v107;
    double v17;
    int64_t v27; /* placeholder for void */
    double v56;
    double v50;
    double v29;
    double v83;
    double v84;
    double v60;
    void* v26;
    void* v112;
    void* v115;
    void* v16;
    int64_t v23; /* placeholder for void */
    double v57;
    double v61;
    double v75;
    int64_t v19; /* placeholder for void */
    double v38;
    int64_t v122; /* placeholder for void */
    void* v41;
    double v106;
    double v81;
    double v105;
    void* v10;
    double v33;
    double v73;
    double v86;
    v4 = (char*)v2 + 8;
    v5 = *(double*)v4;
    v6 = malloc(8);
    *(double*)v6 = v5;
    v8 = (char*)v2 + 16;
    v9 = *(double*)v8;
    v10 = malloc(8);
    *(double*)v10 = v9;
    v12 = (char*)v2 + 24;
    v13 = *(double*)v12;
    v14 = malloc(8);
    *(double*)v14 = v13;
    v16 = (char*)v2 + 0;
    v17 = *(double*)v16;
    v18 = malloc(8);
    *(double*)v18 = v17;
    v20 = (char*)v3 + 0;
    v21 = *(double*)v20;
    v22 = malloc(8);
    *(double*)v22 = v21;
    v24 = (char*)v3 + 8;
    v25 = *(double*)v24;
    v26 = malloc(8);
    *(double*)v26 = v25;
    v28 = (char*)v3 + 16;
    v29 = *(double*)v28;
    v30 = malloc(8);
    *(double*)v30 = v29;
    v32 = ((union { uint64_t u; double f; }){ .u = 4611686018427387904ULL }).f;
    v33 = *(double*)v10;
    v34 = *(double*)v30;
    v35 = v33 * v34;
    v36 = *(double*)v14;
    v37 = *(double*)v26;
    v38 = v36 * v37;
    v39 = v35 - v38;
    v40 = v32 * v39;
    v41 = malloc(8);
    *(double*)v41 = v40;
    v43 = ((union { uint64_t u; double f; }){ .u = 4611686018427387904ULL }).f;
    v44 = *(double*)v14;
    v45 = *(double*)v22;
    v46 = v44 * v45;
    v47 = *(double*)v6;
    v48 = *(double*)v30;
    v49 = v47 * v48;
    v50 = v46 - v49;
    v51 = v43 * v50;
    v52 = malloc(8);
    *(double*)v52 = v51;
    v54 = ((union { uint64_t u; double f; }){ .u = 4611686018427387904ULL }).f;
    v55 = *(double*)v6;
    v56 = *(double*)v26;
    v57 = v55 * v56;
    v58 = *(double*)v10;
    v59 = *(double*)v22;
    v60 = v58 * v59;
    v61 = v57 - v60;
    v62 = v54 * v61;
    v63 = malloc(8);
    *(double*)v63 = v62;
    v65 = malloc(24);
    v66 = *(double*)v22;
    v67 = *(double*)v18;
    v68 = *(double*)v41;
    v69 = v67 * v68;
    v70 = v66 + v69;
    v71 = *(double*)v10;
    v72 = *(double*)v63;
    v73 = v71 * v72;
    v74 = *(double*)v14;
    v75 = *(double*)v52;
    v76 = v74 * v75;
    v77 = v73 - v76;
    v78 = v70 + v77;
    v79 = (char*)v65 + 0;
    *(double*)v79 = v78;
    v81 = *(double*)v26;
    v82 = *(double*)v18;
    v83 = *(double*)v52;
    v84 = v82 * v83;
    v85 = v81 + v84;
    v86 = *(double*)v14;
    v87 = *(double*)v41;
    v88 = v86 * v87;
    v89 = *(double*)v6;
    v90 = *(double*)v63;
    v91 = v89 * v90;
    v92 = v88 - v91;
    v93 = v85 + v92;
    v94 = (char*)v65 + 8;
    *(double*)v94 = v93;
    v96 = *(double*)v30;
    v97 = *(double*)v18;
    v98 = *(double*)v63;
    v99 = v97 * v98;
    v100 = v96 + v99;
    v101 = *(double*)v6;
    v102 = *(double*)v52;
    v103 = v101 * v102;
    v104 = *(double*)v10;
    v105 = *(double*)v41;
    v106 = v104 * v105;
    v107 = v103 - v106;
    v108 = v100 + v107;
    v109 = (char*)v65 + 16;
    *(double*)v109 = v108;
    v111 = (char*)v65 + 0;
    v112 = (char*)v0 + 0;
    v113 = *(double*)v111;
    *(double*)v112 = v113;
    v115 = (char*)v65 + 8;
    v116 = (char*)v0 + 8;
    v117 = *(double*)v115;
    *(double*)v116 = v117;
    v119 = (char*)v65 + 16;
    v120 = (char*)v0 + 16;
    v121 = *(double*)v119;
    *(double*)v120 = v121;
    p_call_stack_pop();
    return;
    return;
}

int64_t p_max(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("max");
    int64_t v5; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    bool v3;
    v3 = v1 > v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return v1;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return v2;
    return 0;
}

void* int_to_str(void* v0, int64_t v1) {
    p_call_stack_push("int_to_str");
    void* v2;
    int64_t v3; /* placeholder for void */
    v2 = (void*)p_to_str((int64_t)v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void quat_from_axis_angle(void* v0, void* v1, void* v2, double v3) {
    p_call_stack_push("quat_from_axis_angle");
    double v26;
    void* v39;
    double v4;
    void* v35;
    double v41;
    int64_t v46; /* placeholder for void */
    double v18;
    double v31;
    void* v48;
    void* v10;
    void* v21;
    void* v44;
    void* v12;
    double v32;
    void* v6;
    void* v29;
    double v9;
    int64_t v11; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    double v25;
    void* v36;
    void* v17;
    int64_t v28; /* placeholder for void */
    double v24;
    double v13;
    void* v15;
    double v8;
    void* v23;
    double v30;
    int64_t v34; /* placeholder for void */
    double v37;
    int64_t v42; /* placeholder for void */
    void* v43;
    double v14;
    int64_t v22; /* placeholder for void */
    void* v40;
    double v49;
    double v19;
    int64_t v7; /* placeholder for void */
    double v20;
    double v45;
    void* v47;
    int64_t v51; /* placeholder for void */
    void* v27;
    void* v33;
    double v5;
    v4 = ((union { uint64_t u; double f; }){ .u = 4602678819172646912ULL }).f;
    v5 = v3 * v4;
    v6 = malloc(8);
    *(double*)v6 = v5;
    v8 = *(double*)v6;
    v9 = (double)pledge_sin(v8);
    v10 = malloc(8);
    *(double*)v10 = v9;
    v12 = malloc(32);
    v13 = *(double*)v6;
    v14 = (double)pledge_cos(v13);
    v15 = (char*)v12 + 0;
    *(double*)v15 = v14;
    v17 = (char*)v2 + 0;
    v18 = *(double*)v17;
    v19 = *(double*)v10;
    v20 = v18 * v19;
    v21 = (char*)v12 + 8;
    *(double*)v21 = v20;
    v23 = (char*)v2 + 8;
    v24 = *(double*)v23;
    v25 = *(double*)v10;
    v26 = v24 * v25;
    v27 = (char*)v12 + 16;
    *(double*)v27 = v26;
    v29 = (char*)v2 + 16;
    v30 = *(double*)v29;
    v31 = *(double*)v10;
    v32 = v30 * v31;
    v33 = (char*)v12 + 24;
    *(double*)v33 = v32;
    v35 = (char*)v12 + 0;
    v36 = (char*)v0 + 0;
    v37 = *(double*)v35;
    *(double*)v36 = v37;
    v39 = (char*)v12 + 8;
    v40 = (char*)v0 + 8;
    v41 = *(double*)v39;
    *(double*)v40 = v41;
    v43 = (char*)v12 + 16;
    v44 = (char*)v0 + 16;
    v45 = *(double*)v43;
    *(double*)v44 = v45;
    v47 = (char*)v12 + 24;
    v48 = (char*)v0 + 24;
    v49 = *(double*)v47;
    *(double*)v48 = v49;
    p_call_stack_pop();
    return;
    return;
}

int64_t Option_unwrap_or_else(void* v0, void* v1, void* v2) {
    p_call_stack_push("Option_unwrap_or_else");
    int64_t v10; /* placeholder for void */
    int64_t v13;
    int64_t v17; /* placeholder for void */
    int64_t v12;
    void* v3;
    int64_t v4;
    void* v8;
    int64_t v5;
    int64_t v7; /* placeholder for void */
    bool v14;
    bool v6;
    int64_t v20; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v16;
    void* v11;
    int64_t v9;
    int64_t v15; /* placeholder for void */
    int64_t v19;
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

void Vec_truncate(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("Vec_truncate");
    int64_t v20; /* placeholder for void */
    int64_t v4;
    int64_t v6; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    bool v14;
    void* v9;
    int64_t v11; /* placeholder for void */
    void* v5;
    int64_t v15; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v3;
    int64_t v18; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    void* v7;
    int64_t v10;
    int64_t v8; /* placeholder for void */
    void* v17;
    int64_t v13;
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

double radians(void* v0, double v1) {
    p_call_stack_push("radians");
    double v2;
    double v3;
    int64_t v4; /* placeholder for void */
    v2 = ((union { uint64_t u; double f; }){ .u = 4580687790476533049ULL }).f;
    v3 = v1 * v2;
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t String_find(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_find");
    int64_t v8; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v49;
    int64_t v35; /* placeholder for void */
    void* v68;
    int64_t v31; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    void* v19;
    int64_t v79; /* placeholder for void */
    int64_t v87;
    bool v30;
    int64_t v13;
    int64_t v5;
    bool v6;
    int64_t v63;
    bool v89;
    int64_t v101; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    void* v71;
    bool v46;
    int64_t v12; /* placeholder for void */
    int64_t v67;
    void* v15;
    int64_t v62; /* placeholder for void */
    int64_t v96;
    int64_t v25;
    bool v4;
    int64_t v20; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v54;
    int64_t v86; /* placeholder for void */
    int64_t v33;
    int64_t v40;
    void* v77;
    int64_t v21;
    void* v50;
    int64_t v3;
    int64_t v76;
    int64_t v56; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v36;
    int64_t v14;
    int64_t v93; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v42;
    void* v83;
    int64_t v45; /* placeholder for void */
    int64_t v53;
    int64_t v95;
    int64_t v38; /* placeholder for void */
    uint8_t v84;
    int64_t v105;
    int64_t v111;
    int64_t v27; /* placeholder for void */
    int64_t v60;
    int64_t v17;
    void* v61;
    int64_t v22;
    int64_t v112; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v107;
    int64_t v11; /* placeholder for void */
    int64_t v109; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    int64_t v18;
    int64_t v57;
    int64_t v99; /* placeholder for void */
    int64_t v58;
    int64_t v64;
    int64_t v72; /* placeholder for void */
    bool v23;
    int64_t v108; /* placeholder for void */
    void* v74;
    int64_t v110;
    int64_t v104; /* placeholder for void */
    int64_t v73;
    int64_t v103; /* placeholder for void */
    void* v47;
    void* v80;
    int64_t v34; /* placeholder for void */
    void* v37;
    int64_t v106;
    int64_t v41;
    int64_t v97;
    int64_t v28;
    int64_t v90; /* placeholder for void */
    int64_t v85; /* placeholder for void */
    int64_t v10;
    uint8_t v81;
    int64_t v102;
    int64_t v94; /* placeholder for void */
    int64_t v70;
    int64_t v88;
    bool v55;
    int64_t v29;
    int64_t v51; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    bool v100;
    int64_t v32;
    void* v65;
    int64_t v75; /* placeholder for void */
    bool v7;
    bool v44;
    int64_t v43;
    int64_t v59;
    bool v91;
    int64_t v16; /* placeholder for void */
    int64_t v9;
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

void allocator_free(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("allocator_free");
    int64_t v3; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    p_allocator_free(v1, v2);
    p_call_stack_pop();
    return;
    return;
}

int64_t Vec_last(void* v0, int64_t v1) {
    p_call_stack_push("Vec_last");
    int64_t v30;
    int64_t v3;
    bool v16;
    int64_t v27; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v11;
    void* v44;
    int64_t v49; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v14;
    int64_t v23; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    void* v8;
    int64_t v33;
    int64_t v37;
    int64_t v19; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v15;
    void* v38;
    int64_t v48;
    int64_t v25;
    void* v10;
    int64_t v5; /* placeholder for void */
    int64_t v29;
    int64_t v42; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    void* v35;
    int64_t v7; /* placeholder for void */
    int64_t v28;
    int64_t v31;
    int64_t v36; /* placeholder for void */
    int64_t v40;
    int64_t v9; /* placeholder for void */
    void* v24;
    int64_t v32;
    int64_t v34;
    int64_t v17; /* placeholder for void */
    void* v4;
    int64_t v2;
    void* v22;
    int64_t v12; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    void* v6;
    void* v18;
    int64_t v20; /* placeholder for void */
    int64_t v45;
    void* v41;
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
    v18 = _str6;
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

int64_t Result_unwrap(void* v0, void* v1) {
    p_call_stack_push("Result_unwrap");
    void* v2;
    bool v5;
    int64_t v8;
    bool v13;
    void* v17;
    int64_t v9; /* placeholder for void */
    void* v10;
    void* v15;
    int64_t v21;
    int64_t v19; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v16;
    int64_t v14; /* placeholder for void */
    int64_t v11;
    int64_t v12;
    void* v7;
    int64_t v20; /* placeholder for void */
    int64_t v3;
    int64_t v22; /* placeholder for void */
    int64_t v4;
    int64_t v6; /* placeholder for void */
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
    v17 = _str7;
    p_panic(v17);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

void Result_map(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Result_map");
    int64_t v31; /* placeholder for void */
    int64_t v29;
    int64_t v43; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    bool v7;
    void* v22;
    int64_t v10;
    void* v23;
    void* v27;
    int64_t v46;
    void* v44;
    int64_t v17; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v5;
    int64_t v21; /* placeholder for void */
    void* v40;
    int64_t v48; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v20;
    int64_t v28;
    void* v18;
    int64_t v42;
    void* v45;
    bool v30;
    void* v4;
    int64_t v8; /* placeholder for void */
    int64_t v6;
    int64_t v24;
    void* v16;
    void* v11;
    void* v9;
    int64_t v15;
    int64_t v33;
    int64_t v12;
    void* v36;
    void* v38;
    int64_t v35;
    int64_t v14; /* placeholder for void */
    void* v13;
    int64_t v39; /* placeholder for void */
    void* v41;
    void* v32;
    int64_t v50; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    void* v19;
    void* v34;
    int64_t v47; /* placeholder for void */
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
        if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "math_abs_i64") == 0) {
            v15 = math_abs_i64((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
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

int64_t String_char_at(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_char_at");
    int64_t v62; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    void* v41;
    int64_t v60; /* placeholder for void */
    int64_t v6;
    int64_t v27; /* placeholder for void */
    int64_t v25;
    int64_t v47; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v53;
    int64_t v10;
    void* v57;
    int64_t v59; /* placeholder for void */
    int64_t v9;
    bool v16;
    bool v17;
    int64_t v19;
    int64_t v35; /* placeholder for void */
    int64_t v36;
    int64_t v46; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v28;
    int64_t v20; /* placeholder for void */
    int64_t v15;
    int64_t v42; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v22;
    void* v26;
    void* v37;
    void* v51;
    int64_t v18; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    void* v38;
    void* v32;
    int64_t v21; /* placeholder for void */
    bool v4;
    int64_t v40;
    void* v23;
    void* v44;
    void* v11;
    uint8_t v33;
    int64_t v45;
    void* v54;
    int64_t v43; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v50;
    int64_t v49;
    int64_t v8; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v3;
    int64_t v13;
    int64_t v30; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v48;
    int64_t v61;
    bool v14;
    void* v29;
    int64_t v58;
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

void* str_trim_end(void* v0, void* v1) {
    p_call_stack_push("str_trim_end");
    int64_t v16;
    bool v12;
    int64_t v11;
    int64_t v25; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v10;
    int64_t v14;
    uint8_t v28;
    void* v24;
    int64_t v23;
    void* v18;
    int64_t v19; /* placeholder for void */
    int64_t v17;
    int64_t v38;
    int64_t v5; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v31;
    int64_t v44;
    void* v4;
    int64_t v15;
    void* v27;
    int64_t v43;
    void* v7;
    void* v21;
    bool v33;
    int64_t v46; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v50;
    int64_t v53;
    int64_t v42; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v39;
    int64_t v13; /* placeholder for void */
    int64_t v34;
    int64_t v8; /* placeholder for void */
    void* v47;
    int64_t v40;
    int64_t v3;
    int64_t v26; /* placeholder for void */
    int64_t v32;
    int64_t v52;
    int64_t v30; /* placeholder for void */
    int64_t v6;
    int64_t v35; /* placeholder for void */
    int64_t v20;
    int64_t v54; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    bool v45;
    int64_t v51;
    int64_t v2;
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
    v47 = _str0;
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

int64_t Result_expect(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_expect");
    void* v16;
    bool v6;
    int64_t v15; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v13;
    void* v3;
    int64_t v9;
    int64_t v21;
    int64_t v5;
    int64_t v12;
    int64_t v17;
    int64_t v18; /* placeholder for void */
    int64_t v4;
    int64_t v10; /* placeholder for void */
    void* v8;
    int64_t v20; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    bool v14;
    void* v11;
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

void Result_map_err(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Result_map_err");
    void* v11;
    int64_t v19;
    int64_t v6;
    void* v40;
    int64_t v14; /* placeholder for void */
    void* v35;
    void* v17;
    void* v38;
    int64_t v10;
    int64_t v5;
    int64_t v16; /* placeholder for void */
    int64_t v46;
    int64_t v24; /* placeholder for void */
    int64_t v27;
    int64_t v37;
    int64_t v47; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    void* v44;
    int64_t v49; /* placeholder for void */
    int64_t v12;
    void* v31;
    void* v18;
    void* v9;
    int64_t v43; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    void* v21;
    int64_t v28;
    void* v26;
    void* v33;
    int64_t v34;
    void* v15;
    void* v45;
    int64_t v42;
    int64_t v50; /* placeholder for void */
    void* v13;
    int64_t v23;
    int64_t v32;
    int64_t v36; /* placeholder for void */
    void* v41;
    bool v7;
    int64_t v30; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    void* v22;
    int64_t v8; /* placeholder for void */
    bool v29;
    int64_t v20; /* placeholder for void */
    void* v4;
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
        if (strcmp(_fn_name, "String_parse_int") == 0) {
            v37 = String_parse_int((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v37 = ByteBuffer_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v37 = Vec_len((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v37 = Option_unwrap((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v37 = HashMap_keys((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v37 = p_str_length((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v37 = String_trim((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v37 = String_char_count((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v37 = String_len((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v37 = char_to_upper((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v37 = Vec_last((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v37 = Result_unwrap((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v37 = str_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v37 = p_abs((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v37 = p_str_hash((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v37 = str_parse_int((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v37 = Vec_pop((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v37 = String_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v37 = String_to_uppercase((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v37 = String_bytes((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v37 = Vec_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v37 = str_parse_hex((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v37 = HashMap_values((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v37 = StringBuilder_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v37 = Vec_capacity((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v37 = HashMap_len((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v37 = String_to_lowercase((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v37 = allocator_bytes_used((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v37 = String_from((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v37 = HashMap_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v37 = char_to_lower((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "math_abs_i64") == 0) {
            v37 = math_abs_i64((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v37 = Vec_first((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v37 = String_to_string((void*)v3, (int64_t)v32);
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

double math_sin(void* v0, double v1) {
    p_call_stack_push("math_sin");
    double v2;
    int64_t v3; /* placeholder for void */
    v2 = (double)pledge_sin(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void* byte_to_str(void* v0, int64_t v1) {
    p_call_stack_push("byte_to_str");
    void* v2;
    int64_t v3; /* placeholder for void */
    v2 = p_char_to_str(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void vec4_scale(void* v0, void* v1, void* v2, double v3) {
    p_call_stack_push("vec4_scale");
    double v16;
    int64_t v14; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    void* v4;
    void* v10;
    double v22;
    double v12;
    double v35;
    int64_t v24; /* placeholder for void */
    void* v37;
    double v27;
    int64_t v40; /* placeholder for void */
    void* v38;
    void* v8;
    double v11;
    double v39;
    void* v26;
    void* v13;
    void* v15;
    double v21;
    void* v33;
    void* v29;
    int64_t v9; /* placeholder for void */
    void* v25;
    void* v5;
    double v6;
    double v31;
    int64_t v28; /* placeholder for void */
    void* v18;
    void* v34;
    int64_t v19; /* placeholder for void */
    double v17;
    void* v30;
    int64_t v36; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    void* v23;
    double v7;
    void* v20;
    v4 = malloc(32);
    v5 = (char*)v2 + 0;
    v6 = *(double*)v5;
    v7 = v6 * v3;
    v8 = (char*)v4 + 0;
    *(double*)v8 = v7;
    v10 = (char*)v2 + 8;
    v11 = *(double*)v10;
    v12 = v11 * v3;
    v13 = (char*)v4 + 8;
    *(double*)v13 = v12;
    v15 = (char*)v2 + 16;
    v16 = *(double*)v15;
    v17 = v16 * v3;
    v18 = (char*)v4 + 16;
    *(double*)v18 = v17;
    v20 = (char*)v2 + 24;
    v21 = *(double*)v20;
    v22 = v21 * v3;
    v23 = (char*)v4 + 24;
    *(double*)v23 = v22;
    v25 = (char*)v4 + 0;
    v26 = (char*)v0 + 0;
    v27 = *(double*)v25;
    *(double*)v26 = v27;
    v29 = (char*)v4 + 8;
    v30 = (char*)v0 + 8;
    v31 = *(double*)v29;
    *(double*)v30 = v31;
    v33 = (char*)v4 + 16;
    v34 = (char*)v0 + 16;
    v35 = *(double*)v33;
    *(double*)v34 = v35;
    v37 = (char*)v4 + 24;
    v38 = (char*)v0 + 24;
    v39 = *(double*)v37;
    *(double*)v38 = v39;
    p_call_stack_pop();
    return;
    return;
}

int64_t Option_unwrap(void* v0, void* v1) {
    p_call_stack_push("Option_unwrap");
    int64_t v6; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    void* v2;
    bool v13;
    int64_t v19;
    int64_t v20; /* placeholder for void */
    int64_t v11;
    void* v7;
    void* v10;
    int64_t v4;
    int64_t v8;
    int64_t v3;
    void* v15;
    int64_t v9; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    bool v5;
    int64_t v17; /* placeholder for void */
    int64_t v12;
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
    v15 = _str8;
    p_panic(v15);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

int64_t p_str_hash(void* v0, int64_t v1) {
    p_call_stack_push("p_str_hash");
    int64_t v18;
    int64_t v30;
    void* v35;
    uint8_t v13;
    void* v6;
    int64_t v27;
    bool v19;
    int64_t v29;
    int64_t v16; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    void* v4;
    int64_t v7; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v23;
    int64_t v31;
    int64_t v32; /* placeholder for void */
    int64_t v33;
    uint8_t v36;
    int64_t v37; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v40;
    int64_t v43; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v22;
    int64_t v20; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v3;
    void* v14;
    int64_t v41;
    int64_t v46; /* placeholder for void */
    int64_t v48;
    void* v9;
    int64_t v17;
    int64_t v5; /* placeholder for void */
    int64_t v8;
    int64_t v21;
    void* v12;
    int64_t v25;
    int64_t v47; /* placeholder for void */
    int64_t v26;
    int64_t v2;
    int64_t v44;
    int64_t v49; /* placeholder for void */
    int64_t v45;
    bool v42;
    int64_t v34; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v10; /* placeholder for void */
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

int64_t str_byte_at(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("str_byte_at");
    int64_t v3;
    int64_t v4;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)String_byte_at((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

void* str_split_char(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("str_split_char");
    int64_t v17; /* placeholder for void */
    void* v8;
    void* v5;
    void* v24;
    void* v65;
    int64_t v47;
    int64_t v31; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v18;
    int64_t v55;
    int64_t v13; /* placeholder for void */
    void* v40;
    int64_t v32; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v37;
    void* v15;
    int64_t v23;
    int64_t v39; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v44;
    int64_t v9; /* placeholder for void */
    int64_t v53;
    int64_t v11;
    int64_t v26;
    int64_t v52; /* placeholder for void */
    void* v27;
    int64_t v64; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    bool v38;
    int64_t v46; /* placeholder for void */
    int64_t v49;
    int64_t v57; /* placeholder for void */
    int64_t v62;
    int64_t v25; /* placeholder for void */
    int64_t v3;
    bool v20;
    void* v33;
    int64_t v41;
    int64_t v45;
    void* v58;
    int64_t v48;
    int64_t v10;
    int64_t v54;
    int64_t v63;
    void* v4;
    uint8_t v34;
    int64_t v42;
    int64_t v21; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    int64_t v22;
    int64_t v61;
    int64_t v14;
    int64_t v43;
    void* v30;
    int64_t v56; /* placeholder for void */
    int64_t v19;
    int64_t v50; /* placeholder for void */
    int64_t v7;
    void* v12;
    int64_t v29;
    int64_t v59;
    int64_t v35; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v60;
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

void mat4_transform_vec4(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("mat4_transform_vec4");
    double v77;
    double v46;
    double v15;
    int64_t v112; /* placeholder for void */
    double v95;
    void* v78;
    int64_t v121; /* placeholder for void */
    int64_t v108; /* placeholder for void */
    void* v105;
    void* v55;
    double v27;
    void* v85;
    double v13;
    double v38;
    void* v57;
    void* v110;
    double v98;
    double v107;
    void* v5;
    double v9;
    void* v97;
    void* v106;
    double v21;
    void* v16;
    double v40;
    void* v62;
    double v52;
    double v65;
    int64_t v104; /* placeholder for void */
    void* v37;
    double v67;
    double v102;
    void* v35;
    double v115;
    double v100;
    void* v53;
    double v17;
    double v26;
    void* v82;
    double v51;
    void* v109;
    void* v32;
    void* v22;
    double v64;
    double v58;
    double v71;
    double v61;
    double v76;
    double v11;
    double v8;
    void* v43;
    double v69;
    void* v60;
    void* v68;
    void* v114;
    double v70;
    void* v74;
    int64_t v120; /* placeholder for void */
    double v39;
    double v63;
    double v56;
    double v92;
    double v94;
    double v119;
    double v84;
    int64_t v79; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    double v96;
    double v48;
    void* v41;
    double v19;
    double v45;
    void* v24;
    void* v28;
    double v42;
    void* v93;
    double v31;
    double v44;
    double v23;
    double v20;
    void* v12;
    double v34;
    void* v80;
    void* v91;
    void* v72;
    void* v49;
    void* v117;
    double v88;
    int64_t v54; /* placeholder for void */
    double v33;
    int64_t v116; /* placeholder for void */
    double v101;
    void* v30;
    void* v47;
    double v73;
    double v75;
    double v90;
    void* v7;
    void* v18;
    double v59;
    double v89;
    void* v103;
    void* v4;
    double v86;
    void* v10;
    double v36;
    void* v87;
    double v50;
    void* v113;
    void* v66;
    double v6;
    double v25;
    void* v99;
    void* v118;
    double v14;
    double v111;
    double v83;
    double v81;
    v4 = malloc(32);
    v5 = (char*)v2 + 0;
    v6 = *(double*)v5;
    v7 = (char*)v3 + 0;
    v8 = *(double*)v7;
    v9 = v6 * v8;
    v10 = (char*)v2 + 8;
    v11 = *(double*)v10;
    v12 = (char*)v3 + 8;
    v13 = *(double*)v12;
    v14 = v11 * v13;
    v15 = v9 + v14;
    v16 = (char*)v2 + 16;
    v17 = *(double*)v16;
    v18 = (char*)v3 + 16;
    v19 = *(double*)v18;
    v20 = v17 * v19;
    v21 = v15 + v20;
    v22 = (char*)v2 + 24;
    v23 = *(double*)v22;
    v24 = (char*)v3 + 24;
    v25 = *(double*)v24;
    v26 = v23 * v25;
    v27 = v21 + v26;
    v28 = (char*)v4 + 0;
    *(double*)v28 = v27;
    v30 = (char*)v2 + 32;
    v31 = *(double*)v30;
    v32 = (char*)v3 + 0;
    v33 = *(double*)v32;
    v34 = v31 * v33;
    v35 = (char*)v2 + 40;
    v36 = *(double*)v35;
    v37 = (char*)v3 + 8;
    v38 = *(double*)v37;
    v39 = v36 * v38;
    v40 = v34 + v39;
    v41 = (char*)v2 + 48;
    v42 = *(double*)v41;
    v43 = (char*)v3 + 16;
    v44 = *(double*)v43;
    v45 = v42 * v44;
    v46 = v40 + v45;
    v47 = (char*)v2 + 56;
    v48 = *(double*)v47;
    v49 = (char*)v3 + 24;
    v50 = *(double*)v49;
    v51 = v48 * v50;
    v52 = v46 + v51;
    v53 = (char*)v4 + 8;
    *(double*)v53 = v52;
    v55 = (char*)v2 + 64;
    v56 = *(double*)v55;
    v57 = (char*)v3 + 0;
    v58 = *(double*)v57;
    v59 = v56 * v58;
    v60 = (char*)v2 + 72;
    v61 = *(double*)v60;
    v62 = (char*)v3 + 8;
    v63 = *(double*)v62;
    v64 = v61 * v63;
    v65 = v59 + v64;
    v66 = (char*)v2 + 80;
    v67 = *(double*)v66;
    v68 = (char*)v3 + 16;
    v69 = *(double*)v68;
    v70 = v67 * v69;
    v71 = v65 + v70;
    v72 = (char*)v2 + 88;
    v73 = *(double*)v72;
    v74 = (char*)v3 + 24;
    v75 = *(double*)v74;
    v76 = v73 * v75;
    v77 = v71 + v76;
    v78 = (char*)v4 + 16;
    *(double*)v78 = v77;
    v80 = (char*)v2 + 96;
    v81 = *(double*)v80;
    v82 = (char*)v3 + 0;
    v83 = *(double*)v82;
    v84 = v81 * v83;
    v85 = (char*)v2 + 104;
    v86 = *(double*)v85;
    v87 = (char*)v3 + 8;
    v88 = *(double*)v87;
    v89 = v86 * v88;
    v90 = v84 + v89;
    v91 = (char*)v2 + 112;
    v92 = *(double*)v91;
    v93 = (char*)v3 + 16;
    v94 = *(double*)v93;
    v95 = v92 * v94;
    v96 = v90 + v95;
    v97 = (char*)v2 + 120;
    v98 = *(double*)v97;
    v99 = (char*)v3 + 24;
    v100 = *(double*)v99;
    v101 = v98 * v100;
    v102 = v96 + v101;
    v103 = (char*)v4 + 24;
    *(double*)v103 = v102;
    v105 = (char*)v4 + 0;
    v106 = (char*)v0 + 0;
    v107 = *(double*)v105;
    *(double*)v106 = v107;
    v109 = (char*)v4 + 8;
    v110 = (char*)v0 + 8;
    v111 = *(double*)v109;
    *(double*)v110 = v111;
    v113 = (char*)v4 + 16;
    v114 = (char*)v0 + 16;
    v115 = *(double*)v113;
    *(double*)v114 = v115;
    v117 = (char*)v4 + 24;
    v118 = (char*)v0 + 24;
    v119 = *(double*)v117;
    *(double*)v118 = v119;
    p_call_stack_pop();
    return;
    return;
}

int64_t String_rfind(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_rfind");
    int64_t v3;
    int64_t v105;
    int64_t v10;
    void* v65;
    int64_t v26; /* placeholder for void */
    int64_t v22;
    void* v74;
    int64_t v37;
    int64_t v73;
    int64_t v97;
    int64_t v43;
    int64_t v110;
    int64_t v16; /* placeholder for void */
    int64_t v63;
    int64_t v25;
    void* v68;
    int64_t v85; /* placeholder for void */
    bool v100;
    int64_t v45; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    int64_t v111;
    int64_t v32;
    int64_t v49;
    int64_t v93; /* placeholder for void */
    int64_t v106;
    int64_t v58;
    int64_t v62; /* placeholder for void */
    int64_t v28;
    int64_t v72; /* placeholder for void */
    void* v50;
    void* v15;
    int64_t v57;
    int64_t v5;
    bool v91;
    int64_t v95;
    int64_t v99; /* placeholder for void */
    int64_t v87;
    int64_t v69; /* placeholder for void */
    int64_t v103; /* placeholder for void */
    void* v47;
    int64_t v53;
    int64_t v98; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    uint8_t v84;
    int64_t v17;
    void* v80;
    void* v83;
    int64_t v52; /* placeholder for void */
    bool v55;
    int64_t v60;
    uint8_t v81;
    bool v30;
    int64_t v31; /* placeholder for void */
    bool v23;
    int64_t v96;
    bool v6;
    bool v46;
    int64_t v12; /* placeholder for void */
    int64_t v42;
    void* v77;
    int64_t v11; /* placeholder for void */
    bool v44;
    void* v71;
    int64_t v38;
    bool v7;
    int64_t v88;
    int64_t v107;
    int64_t v14;
    int64_t v112; /* placeholder for void */
    int64_t v67;
    int64_t v56; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v21;
    int64_t v92; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v102;
    int64_t v109; /* placeholder for void */
    bool v4;
    int64_t v40; /* placeholder for void */
    bool v89;
    int64_t v9;
    int64_t v41; /* placeholder for void */
    int64_t v64;
    int64_t v94; /* placeholder for void */
    int64_t v108; /* placeholder for void */
    void* v39;
    int64_t v36;
    int64_t v54;
    int64_t v59;
    int64_t v75; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v61;
    int64_t v76;
    int64_t v13;
    void* v19;
    int64_t v70;
    int64_t v33;
    int64_t v18;
    int64_t v20; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v29;
    int64_t v82; /* placeholder for void */
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

int64_t Vec_get(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("Vec_get");
    int64_t v21; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v32;
    int64_t v36;
    int64_t v12;
    int64_t v44;
    void* v5;
    int64_t v14; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v15;
    void* v25;
    void* v31;
    int64_t v35;
    int64_t v37;
    int64_t v50; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v9;
    int64_t v28; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v22;
    int64_t v27; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    void* v42;
    void* v39;
    void* v11;
    void* v18;
    int64_t v33; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v38;
    int64_t v41;
    void* v48;
    int64_t v47; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    void* v45;
    int64_t v49;
    void* v29;
    int64_t v34; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    void* v7;
    bool v16;
    int64_t v43; /* placeholder for void */
    int64_t v3;
    bool v23;
    int64_t v30; /* placeholder for void */
    int64_t v4;
    int64_t v52;
    int64_t v40; /* placeholder for void */
    int64_t v26; /* placeholder for void */
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
    v18 = _str1;
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
    v25 = _str1;
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

bool str_starts_with(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_starts_with");
    int64_t v3;
    bool v4;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (bool)String_starts_with((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

bool HashMap_contains(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_contains");
    int64_t v52; /* placeholder for void */
    int64_t v3;
    int64_t v13; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    int64_t v58;
    bool v61;
    int64_t v55;
    int64_t v4;
    int64_t v21; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v23;
    int64_t v12;
    int64_t v6; /* placeholder for void */
    int64_t v36;
    int64_t v57;
    int64_t v35; /* placeholder for void */
    bool v17;
    void* v47;
    int64_t v51;
    bool v60;
    bool v19;
    int64_t v7;
    int64_t v37;
    int64_t v39;
    int64_t v38;
    int64_t v28;
    int64_t v31; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v46;
    int64_t v53; /* placeholder for void */
    int64_t v22;
    void* v32;
    void* v44;
    int64_t v9; /* placeholder for void */
    void* v5;
    int64_t v40;
    void* v29;
    bool v56;
    void* v50;
    int64_t v25; /* placeholder for void */
    int64_t v54;
    int64_t v14; /* placeholder for void */
    void* v26;
    int64_t v49; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    void* v41;
    int64_t v18; /* placeholder for void */
    void* v24;
    int64_t v48; /* placeholder for void */
    int64_t v43;
    int64_t v16;
    int64_t v33;
    int64_t v15;
    int64_t v59;
    void* v11;
    void* v8;
    int64_t v62; /* placeholder for void */
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

double math_fabs(void* v0, double v1) {
    p_call_stack_push("math_fabs");
    double v2;
    int64_t v3; /* placeholder for void */
    v2 = (double)pledge_fabs(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

double math_e(void* v0) {
    p_call_stack_push("math_e");
    int64_t v2; /* placeholder for void */
    double v1;
    v1 = ((union { uint64_t u; double f; }){ .u = 4613303445314885481ULL }).f;
    p_call_stack_pop();
    return v1;
    return 0;
}

void Vec_reverse(void* v0, int64_t v1) {
    p_call_stack_push("Vec_reverse");
    int64_t v58; /* placeholder for void */
    int64_t v44;
    void* v63;
    int64_t v41;
    int64_t v74; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v2;
    void* v14;
    int64_t v48;
    int64_t v32;
    void* v49;
    void* v57;
    void* v6;
    int64_t v75; /* placeholder for void */
    void* v61;
    int64_t v65; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v17;
    int64_t v39;
    void* v46;
    int64_t v20;
    int64_t v22; /* placeholder for void */
    int64_t v71;
    int64_t v13; /* placeholder for void */
    int64_t v33;
    int64_t v72;
    void* v59;
    int64_t v38;
    int64_t v62; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    void* v21;
    void* v66;
    int64_t v27;
    int64_t v18; /* placeholder for void */
    int64_t v60;
    int64_t v54; /* placeholder for void */
    void* v4;
    int64_t v42;
    int64_t v69; /* placeholder for void */
    void* v10;
    int64_t v34;
    void* v35;
    int64_t v24;
    int64_t v45;
    int64_t v50; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v67;
    int64_t v30;
    int64_t v53; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v70;
    int64_t v47; /* placeholder for void */
    int64_t v3;
    int64_t v40;
    int64_t v43;
    void* v55;
    int64_t v64;
    void* v52;
    int64_t v19; /* placeholder for void */
    void* v8;
    void* v16;
    int64_t v29; /* placeholder for void */
    int64_t v31;
    int64_t v23; /* placeholder for void */
    int64_t v37;
    int64_t v73; /* placeholder for void */
    int64_t v51;
    int64_t v25;
    bool v28;
    int64_t v11;
    int64_t v9; /* placeholder for void */
    int64_t v26;
    int64_t v56;
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

void StringBuilder_append_char(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("StringBuilder_append_char");
    int64_t v11;
    int64_t v12;
    int64_t v13;
    void* v9;
    int64_t v8; /* placeholder for void */
    void* v5;
    void* v4;
    void* v6;
    int64_t v15; /* placeholder for void */
    void* v3;
    void* v7;
    void* v10;
    int64_t v14; /* placeholder for void */
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

void* str_concat(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_concat");
    void* v3;
    int64_t v4; /* placeholder for void */
    v3 = p_str_concat(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t char_to_lower(void* v0, int64_t v1) {
    p_call_stack_push("char_to_lower");
    int64_t v11; /* placeholder for void */
    int64_t v4;
    int64_t v10; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    bool v6;
    bool v5;
    int64_t v8;
    int64_t v2;
    bool v3;
    int64_t v7; /* placeholder for void */
    int64_t v9;
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

int64_t HashMap_values(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_values");
    int64_t v82;
    void* v4;
    int64_t v29;
    int64_t v36;
    int64_t v88; /* placeholder for void */
    int64_t v111;
    int64_t v21; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    int64_t v105; /* placeholder for void */
    int64_t v108;
    void* v96;
    int64_t v32;
    void* v66;
    int64_t v58;
    int64_t v49; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    int64_t v26;
    bool v38;
    int64_t v103; /* placeholder for void */
    int64_t v51;
    int64_t v85;
    void* v93;
    bool v42;
    int64_t v31; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    void* v19;
    int64_t v64; /* placeholder for void */
    int64_t v57;
    int64_t v44;
    int64_t v62;
    int64_t v74;
    void* v28;
    void* v34;
    int64_t v109; /* placeholder for void */
    int64_t v61; /* placeholder for void */
    int64_t v73;
    void* v87;
    int64_t v24; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v99; /* placeholder for void */
    bool v53;
    int64_t v101;
    int64_t v110; /* placeholder for void */
    bool v75;
    int64_t v68; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v40;
    bool v41;
    int64_t v102;
    int64_t v7; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v92;
    int64_t v56;
    int64_t v76;
    int64_t v86;
    int64_t v17; /* placeholder for void */
    int64_t v107;
    void* v6;
    void* v13;
    int64_t v91; /* placeholder for void */
    void* v10;
    int64_t v84;
    int64_t v15;
    void* v90;
    int64_t v23;
    int64_t v94; /* placeholder for void */
    bool v79;
    bool v80;
    int64_t v89;
    int64_t v106;
    void* v63;
    int64_t v100;
    int64_t v55;
    int64_t v46; /* placeholder for void */
    int64_t v9;
    void* v69;
    int64_t v37;
    int64_t v2;
    int64_t v14; /* placeholder for void */
    int64_t v72; /* placeholder for void */
    int64_t v3;
    int64_t v78;
    void* v16;
    int64_t v8;
    int64_t v5; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    void* v22;
    void* v25;
    int64_t v67; /* placeholder for void */
    int64_t v83;
    int64_t v59;
    int64_t v65;
    int64_t v47;
    void* v48;
    int64_t v27; /* placeholder for void */
    int64_t v77;
    int64_t v39;
    int64_t v70;
    int64_t v12;
    int64_t v45; /* placeholder for void */
    int64_t v112; /* placeholder for void */
    void* v60;
    int64_t v52;
    int64_t v35; /* placeholder for void */
    int64_t v97;
    int64_t v33;
    int64_t v81; /* placeholder for void */
    int64_t v18;
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

int64_t HashMap_get(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_get");
    int64_t v19;
    void* v32;
    int64_t v62; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v66;
    int64_t v55;
    int64_t v48; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v44;
    int64_t v46;
    int64_t v7;
    int64_t v31; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v63;
    int64_t v20; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    void* v74;
    void* v89;
    int64_t v37;
    int64_t v52; /* placeholder for void */
    int64_t v72; /* placeholder for void */
    void* v50;
    int64_t v82;
    void* v92;
    int64_t v65; /* placeholder for void */
    void* v24;
    int64_t v28;
    int64_t v35; /* placeholder for void */
    int64_t v85;
    int64_t v94; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    int64_t v73; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v87; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    int64_t v75;
    int64_t v93;
    bool v56;
    int64_t v18; /* placeholder for void */
    int64_t v23;
    int64_t v70;
    void* v29;
    int64_t v43;
    int64_t v38;
    int64_t v13; /* placeholder for void */
    int64_t v4;
    void* v11;
    int64_t v12;
    int64_t v30; /* placeholder for void */
    void* v47;
    bool v17;
    int64_t v3;
    int64_t v57;
    void* v41;
    int64_t v59;
    bool v61;
    void* v71;
    int64_t v16;
    int64_t v39;
    int64_t v40;
    int64_t v45; /* placeholder for void */
    void* v68;
    int64_t v6; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v36;
    int64_t v42; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v58;
    bool v60;
    int64_t v80;
    int64_t v96;
    void* v26;
    int64_t v54;
    int64_t v15;
    int64_t v21; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v67;
    int64_t v77; /* placeholder for void */
    void* v8;
    void* v5;
    int64_t v79;
    void* v83;
    int64_t v33;
    int64_t v53; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    void* v86;
    int64_t v88;
    int64_t v22;
    int64_t v51;
    int64_t v81;
    int64_t v14; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v78;
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

int64_t StringBuilder_len(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_len");
    void* v2;
    int64_t v3;
    int64_t v4; /* placeholder for void */
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    p_call_stack_pop();
    return v3;
    return 0;
}

void println(void* v0, void* v1) {
    p_call_stack_push("println");
    void* v3;
    int64_t v4; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v2; /* placeholder for void */
    p_print_str(v1);
    v3 = _str9;
    p_print_str(v3);
    p_call_stack_pop();
    return;
    return;
}

void p_str_copy_n(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("p_str_copy_n");
    int64_t v27;
    int64_t v34;
    int64_t v40;
    int64_t v12;
    int64_t v5; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v9;
    int64_t v37; /* placeholder for void */
    void* v16;
    int64_t v8;
    void* v19;
    int64_t v35;
    int64_t v28; /* placeholder for void */
    void* v22;
    void* v4;
    int64_t v33; /* placeholder for void */
    uint8_t v23;
    void* v6;
    void* v26;
    int64_t v25; /* placeholder for void */
    int64_t v30;
    int64_t v32;
    int64_t v11; /* placeholder for void */
    int64_t v15;
    int64_t v18;
    bool v13;
    int64_t v29; /* placeholder for void */
    void* v24;
    int64_t v31;
    int64_t v36;
    int64_t v38;
    int64_t v21; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v39;
    int64_t v14; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v41; /* placeholder for void */
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

double math_floor(void* v0, double v1) {
    p_call_stack_push("math_floor");
    int64_t v3; /* placeholder for void */
    double v2;
    v2 = (double)pledge_floor(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t str_find(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_find");
    int64_t v3;
    int64_t v4;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)String_find((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

bool HashMap_is_empty(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_is_empty");
    int64_t v9; /* placeholder for void */
    void* v7;
    void* v4;
    int64_t v13; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v14;
    int64_t v8; /* placeholder for void */
    bool v16;
    int64_t v5; /* placeholder for void */
    int64_t v15;
    void* v10;
    int64_t v17; /* placeholder for void */
    int64_t v6;
    int64_t v3;
    int64_t v2;
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

void* str_trim_start(void* v0, void* v1) {
    p_call_stack_push("str_trim_start");
    int64_t v23; /* placeholder for void */
    int64_t v2;
    int64_t v27; /* placeholder for void */
    int64_t v29;
    bool v43;
    int64_t v6;
    int64_t v46; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v49;
    void* v16;
    bool v12;
    int64_t v3;
    int64_t v21;
    void* v4;
    int64_t v36;
    uint8_t v26;
    int64_t v30;
    int64_t v15;
    int64_t v17; /* placeholder for void */
    void* v19;
    int64_t v11;
    int64_t v14;
    int64_t v5; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    void* v22;
    int64_t v24; /* placeholder for void */
    int64_t v10;
    int64_t v37;
    int64_t v9; /* placeholder for void */
    int64_t v42;
    void* v45;
    int64_t v34; /* placeholder for void */
    int64_t v50;
    int64_t v44; /* placeholder for void */
    int64_t v38;
    void* v7;
    int64_t v32;
    void* v25;
    bool v31;
    int64_t v51;
    int64_t v18;
    int64_t v52; /* placeholder for void */
    int64_t v41;
    int64_t v8; /* placeholder for void */
    int64_t v48;
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
    v45 = _str0;
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

void vec2_normalize(void* v0, void* v1, void* v2) {
    p_call_stack_push("vec2_normalize");
    void* v16;
    double v12;
    double v47;
    int64_t v48; /* placeholder for void */
    void* v5;
    int64_t v26; /* placeholder for void */
    void* v29;
    void* v35;
    void* v39;
    void* v18;
    int64_t v27; /* placeholder for void */
    void* v28;
    void* v41;
    double v20;
    double v37;
    int64_t v49; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    void* v33;
    void* v19;
    double v38;
    bool v9;
    void* v11;
    double v36;
    int64_t v10; /* placeholder for void */
    double v32;
    int64_t v3;
    double v8;
    double v31;
    double v43;
    double v15;
    int64_t v44; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    double v7;
    void* v42;
    int64_t v25; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    void* v45;
    void* v46;
    void* v23;
    double v30;
    double v4;
    void* v22;
    double v24;
    void* v13;
    v3 = 0LL;
    v4 = (double)vec2_length((void*)v3, v2);
    v5 = malloc(8);
    *(double*)v5 = v4;
    v7 = *(double*)v5;
    v8 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v9 = v7 == v8;
    if (v9) goto bb1;
    else goto bb2;
  bb1:
    v11 = malloc(16);
    v12 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v13 = (char*)v11 + 0;
    *(double*)v13 = v12;
    v15 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v16 = (char*)v11 + 8;
    *(double*)v16 = v15;
    v18 = (char*)v11 + 0;
    v19 = (char*)v0 + 0;
    v20 = *(double*)v18;
    *(double*)v19 = v20;
    v22 = (char*)v11 + 8;
    v23 = (char*)v0 + 8;
    v24 = *(double*)v22;
    *(double*)v23 = v24;
    p_call_stack_pop();
    return;
  bb2:
    goto bb3;
  bb3:
    v28 = malloc(16);
    v29 = (char*)v2 + 0;
    v30 = *(double*)v29;
    v31 = *(double*)v5;
    v32 = v30 / v31;
    v33 = (char*)v28 + 0;
    *(double*)v33 = v32;
    v35 = (char*)v2 + 8;
    v36 = *(double*)v35;
    v37 = *(double*)v5;
    v38 = v36 / v37;
    v39 = (char*)v28 + 8;
    *(double*)v39 = v38;
    v41 = (char*)v28 + 0;
    v42 = (char*)v0 + 0;
    v43 = *(double*)v41;
    *(double*)v42 = v43;
    v45 = (char*)v28 + 8;
    v46 = (char*)v0 + 8;
    v47 = *(double*)v45;
    *(double*)v46 = v47;
    p_call_stack_pop();
    return;
    return;
}

void vec2_sub(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("vec2_sub");
    void* v14;
    double v15;
    double v6;
    void* v17;
    void* v12;
    int64_t v18; /* placeholder for void */
    void* v19;
    double v9;
    void* v20;
    int64_t v22; /* placeholder for void */
    void* v23;
    void* v24;
    double v25;
    double v8;
    int64_t v26; /* placeholder for void */
    double v21;
    int64_t v27; /* placeholder for void */
    void* v7;
    double v16;
    void* v5;
    double v13;
    void* v10;
    int64_t v11; /* placeholder for void */
    void* v4;
    v4 = malloc(16);
    v5 = (char*)v2 + 0;
    v6 = *(double*)v5;
    v7 = (char*)v3 + 0;
    v8 = *(double*)v7;
    v9 = v6 - v8;
    v10 = (char*)v4 + 0;
    *(double*)v10 = v9;
    v12 = (char*)v2 + 8;
    v13 = *(double*)v12;
    v14 = (char*)v3 + 8;
    v15 = *(double*)v14;
    v16 = v13 - v15;
    v17 = (char*)v4 + 8;
    *(double*)v17 = v16;
    v19 = (char*)v4 + 0;
    v20 = (char*)v0 + 0;
    v21 = *(double*)v19;
    *(double*)v20 = v21;
    v23 = (char*)v4 + 8;
    v24 = (char*)v0 + 8;
    v25 = *(double*)v23;
    *(double*)v24 = v25;
    p_call_stack_pop();
    return;
    return;
}

void vec4_lerp(void* v0, void* v1, void* v2, void* v3, double v4) {
    p_call_stack_push("vec4_lerp");
    void* v19;
    double v7;
    double v35;
    void* v54;
    double v13;
    double v47;
    double v23;
    int64_t v66; /* placeholder for void */
    void* v17;
    int64_t v27; /* placeholder for void */
    double v36;
    int64_t v53; /* placeholder for void */
    double v44;
    double v45;
    double v20;
    double v9;
    double v22;
    void* v48;
    double v64;
    void* v39;
    void* v43;
    double v14;
    double v24;
    void* v51;
    void* v59;
    double v34;
    void* v50;
    void* v55;
    double v42;
    void* v5;
    void* v26;
    int64_t v57; /* placeholder for void */
    double v40;
    int64_t v38; /* placeholder for void */
    void* v10;
    void* v30;
    void* v58;
    void* v63;
    void* v37;
    double v56;
    double v18;
    int64_t v61; /* placeholder for void */
    void* v28;
    void* v15;
    void* v32;
    double v33;
    double v52;
    int64_t v16; /* placeholder for void */
    void* v41;
    int64_t v65; /* placeholder for void */
    void* v6;
    double v12;
    double v25;
    double v29;
    double v46;
    int64_t v49; /* placeholder for void */
    double v11;
    void* v8;
    void* v21;
    void* v62;
    double v31;
    double v60;
    v5 = malloc(32);
    v6 = (char*)v2 + 0;
    v7 = *(double*)v6;
    v8 = (char*)v3 + 0;
    v9 = *(double*)v8;
    v10 = (char*)v2 + 0;
    v11 = *(double*)v10;
    v12 = v9 - v11;
    v13 = v12 * v4;
    v14 = v7 + v13;
    v15 = (char*)v5 + 0;
    *(double*)v15 = v14;
    v17 = (char*)v2 + 8;
    v18 = *(double*)v17;
    v19 = (char*)v3 + 8;
    v20 = *(double*)v19;
    v21 = (char*)v2 + 8;
    v22 = *(double*)v21;
    v23 = v20 - v22;
    v24 = v23 * v4;
    v25 = v18 + v24;
    v26 = (char*)v5 + 8;
    *(double*)v26 = v25;
    v28 = (char*)v2 + 16;
    v29 = *(double*)v28;
    v30 = (char*)v3 + 16;
    v31 = *(double*)v30;
    v32 = (char*)v2 + 16;
    v33 = *(double*)v32;
    v34 = v31 - v33;
    v35 = v34 * v4;
    v36 = v29 + v35;
    v37 = (char*)v5 + 16;
    *(double*)v37 = v36;
    v39 = (char*)v2 + 24;
    v40 = *(double*)v39;
    v41 = (char*)v3 + 24;
    v42 = *(double*)v41;
    v43 = (char*)v2 + 24;
    v44 = *(double*)v43;
    v45 = v42 - v44;
    v46 = v45 * v4;
    v47 = v40 + v46;
    v48 = (char*)v5 + 24;
    *(double*)v48 = v47;
    v50 = (char*)v5 + 0;
    v51 = (char*)v0 + 0;
    v52 = *(double*)v50;
    *(double*)v51 = v52;
    v54 = (char*)v5 + 8;
    v55 = (char*)v0 + 8;
    v56 = *(double*)v54;
    *(double*)v55 = v56;
    v58 = (char*)v5 + 16;
    v59 = (char*)v0 + 16;
    v60 = *(double*)v58;
    *(double*)v59 = v60;
    v62 = (char*)v5 + 24;
    v63 = (char*)v0 + 24;
    v64 = *(double*)v62;
    *(double*)v63 = v64;
    p_call_stack_pop();
    return;
    return;
}

void mat4_transform_vec3(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("mat4_transform_vec3");
    void* v52;
    int64_t v106; /* placeholder for void */
    void* v76;
    double v13;
    void* v40;
    double v109;
    double v89;
    void* v116;
    double v41;
    void* v21;
    double v56;
    double v44;
    void* v9;
    void* v112;
    double v102;
    double v103;
    double v26;
    double v19;
    double v71;
    double v36;
    int64_t v110; /* placeholder for void */
    void* v105;
    double v60;
    void* v57;
    double v27;
    double v14;
    double v45;
    void* v111;
    int64_t v32; /* placeholder for void */
    double v8;
    void* v72;
    double v104;
    void* v115;
    double v77;
    double v50;
    int64_t v29; /* placeholder for void */
    double v43;
    void* v61;
    void* v46;
    double v63;
    void* v66;
    void* v100;
    double v87;
    double v22;
    double v7;
    double v97;
    void* v107;
    double v55;
    double v92;
    double v23;
    double v79;
    double v12;
    int64_t v25; /* placeholder for void */
    double v5;
    double v10;
    double v75;
    int64_t v118; /* placeholder for void */
    int64_t v114; /* placeholder for void */
    double v62;
    int64_t v82; /* placeholder for void */
    double v49;
    void* v83;
    void* v88;
    int64_t v119; /* placeholder for void */
    bool v28;
    double v65;
    void* v42;
    double v101;
    double v80;
    double v117;
    double v30;
    void* v108;
    int64_t v33; /* placeholder for void */
    double v98;
    void* v15;
    void* v81;
    double v99;
    double v68;
    double v69;
    double v53;
    double v18;
    double v47;
    double v73;
    void* v90;
    void* v17;
    void* v4;
    double v51;
    double v54;
    double v91;
    void* v35;
    void* v96;
    double v113;
    double v20;
    double v74;
    double v95;
    double v38;
    int64_t v31; /* placeholder for void */
    double v84;
    double v86;
    void* v37;
    void* v70;
    void* v6;
    void* v64;
    double v67;
    void* v34;
    void* v94;
    void* v11;
    int64_t v58; /* placeholder for void */
    void* v59;
    void* v24;
    void* v48;
    double v78;
    double v93;
    void* v85;
    double v39;
    double v16;
    v4 = (char*)v2 + 96;
    v5 = *(double*)v4;
    v6 = (char*)v3 + 0;
    v7 = *(double*)v6;
    v8 = v5 * v7;
    v9 = (char*)v2 + 104;
    v10 = *(double*)v9;
    v11 = (char*)v3 + 8;
    v12 = *(double*)v11;
    v13 = v10 * v12;
    v14 = v8 + v13;
    v15 = (char*)v2 + 112;
    v16 = *(double*)v15;
    v17 = (char*)v3 + 16;
    v18 = *(double*)v17;
    v19 = v16 * v18;
    v20 = v14 + v19;
    v21 = (char*)v2 + 120;
    v22 = *(double*)v21;
    v23 = v20 + v22;
    v24 = malloc(8);
    *(double*)v24 = v23;
    v26 = *(double*)v24;
    v27 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v28 = v26 == v27;
    if (v28) goto bb1;
    else goto bb2;
  bb1:
    v30 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    *(double*)v24 = v30;
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v34 = malloc(24);
    v35 = (char*)v2 + 0;
    v36 = *(double*)v35;
    v37 = (char*)v3 + 0;
    v38 = *(double*)v37;
    v39 = v36 * v38;
    v40 = (char*)v2 + 8;
    v41 = *(double*)v40;
    v42 = (char*)v3 + 8;
    v43 = *(double*)v42;
    v44 = v41 * v43;
    v45 = v39 + v44;
    v46 = (char*)v2 + 16;
    v47 = *(double*)v46;
    v48 = (char*)v3 + 16;
    v49 = *(double*)v48;
    v50 = v47 * v49;
    v51 = v45 + v50;
    v52 = (char*)v2 + 24;
    v53 = *(double*)v52;
    v54 = v51 + v53;
    v55 = *(double*)v24;
    v56 = v54 / v55;
    v57 = (char*)v34 + 0;
    *(double*)v57 = v56;
    v59 = (char*)v2 + 32;
    v60 = *(double*)v59;
    v61 = (char*)v3 + 0;
    v62 = *(double*)v61;
    v63 = v60 * v62;
    v64 = (char*)v2 + 40;
    v65 = *(double*)v64;
    v66 = (char*)v3 + 8;
    v67 = *(double*)v66;
    v68 = v65 * v67;
    v69 = v63 + v68;
    v70 = (char*)v2 + 48;
    v71 = *(double*)v70;
    v72 = (char*)v3 + 16;
    v73 = *(double*)v72;
    v74 = v71 * v73;
    v75 = v69 + v74;
    v76 = (char*)v2 + 56;
    v77 = *(double*)v76;
    v78 = v75 + v77;
    v79 = *(double*)v24;
    v80 = v78 / v79;
    v81 = (char*)v34 + 8;
    *(double*)v81 = v80;
    v83 = (char*)v2 + 64;
    v84 = *(double*)v83;
    v85 = (char*)v3 + 0;
    v86 = *(double*)v85;
    v87 = v84 * v86;
    v88 = (char*)v2 + 72;
    v89 = *(double*)v88;
    v90 = (char*)v3 + 8;
    v91 = *(double*)v90;
    v92 = v89 * v91;
    v93 = v87 + v92;
    v94 = (char*)v2 + 80;
    v95 = *(double*)v94;
    v96 = (char*)v3 + 16;
    v97 = *(double*)v96;
    v98 = v95 * v97;
    v99 = v93 + v98;
    v100 = (char*)v2 + 88;
    v101 = *(double*)v100;
    v102 = v99 + v101;
    v103 = *(double*)v24;
    v104 = v102 / v103;
    v105 = (char*)v34 + 16;
    *(double*)v105 = v104;
    v107 = (char*)v34 + 0;
    v108 = (char*)v0 + 0;
    v109 = *(double*)v107;
    *(double*)v108 = v109;
    v111 = (char*)v34 + 8;
    v112 = (char*)v0 + 8;
    v113 = *(double*)v111;
    *(double*)v112 = v113;
    v115 = (char*)v34 + 16;
    v116 = (char*)v0 + 16;
    v117 = *(double*)v115;
    *(double*)v116 = v117;
    p_call_stack_pop();
    return;
    return;
}

int64_t String_trim(void* v0, int64_t v1) {
    p_call_stack_push("String_trim");
    int64_t v45;
    int64_t v27;
    int64_t v70; /* placeholder for void */
    int64_t v141; /* placeholder for void */
    void* v33;
    int64_t v138; /* placeholder for void */
    bool v128;
    int64_t v63;
    int64_t v31; /* placeholder for void */
    bool v115;
    int64_t v14;
    int64_t v152; /* placeholder for void */
    int64_t v136; /* placeholder for void */
    int64_t v142; /* placeholder for void */
    int64_t v97;
    void* v30;
    int64_t v9;
    int64_t v41;
    int64_t v117;
    int64_t v168;
    int64_t v176;
    int64_t v182;
    int64_t v157;
    int64_t v34; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    int64_t v106;
    int64_t v184; /* placeholder for void */
    int64_t v131; /* placeholder for void */
    uint8_t v102;
    int64_t v92;
    int64_t v11; /* placeholder for void */
    void* v161;
    int64_t v26; /* placeholder for void */
    void* v81;
    void* v103;
    int64_t v58;
    void* v25;
    void* v36;
    int64_t v104; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v59;
    bool v74;
    int64_t v89; /* placeholder for void */
    int64_t v165;
    int64_t v181;
    int64_t v23; /* placeholder for void */
    int64_t v53;
    int64_t v174;
    int64_t v132;
    int64_t v16; /* placeholder for void */
    int64_t v32;
    bool v111;
    int64_t v185;
    void* v38;
    void* v77;
    int64_t v96; /* placeholder for void */
    int64_t v109;
    int64_t v192; /* placeholder for void */
    int64_t v62;
    int64_t v167; /* placeholder for void */
    int64_t v143; /* placeholder for void */
    void* v158;
    int64_t v48;
    int64_t v130; /* placeholder for void */
    int64_t v124;
    int64_t v129; /* placeholder for void */
    int64_t v49;
    void* v98;
    int64_t v88;
    int64_t v113;
    bool v47;
    int64_t v170;
    int64_t v42;
    int64_t v17;
    int64_t v179; /* placeholder for void */
    bool v43;
    int64_t v162;
    int64_t v85;
    int64_t v144;
    bool v112;
    int64_t v126;
    int64_t v145;
    void* v10;
    int64_t v100; /* placeholder for void */
    int64_t v178;
    void* v7;
    int64_t v175;
    int64_t v19; /* placeholder for void */
    int64_t v5;
    int64_t v24;
    bool v46;
    int64_t v105; /* placeholder for void */
    int64_t v57;
    int64_t v80;
    int64_t v173; /* placeholder for void */
    void* v95;
    bool v108;
    void* v155;
    bool v116;
    int64_t v166; /* placeholder for void */
    void* v171;
    void* v172;
    int64_t v177;
    int64_t v66; /* placeholder for void */
    int64_t v180;
    void* v183;
    void* v147;
    bool v119;
    void* v67;
    int64_t v75; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v148; /* placeholder for void */
    int64_t v2;
    uint8_t v140;
    void* v154;
    int64_t v190;
    int64_t v64; /* placeholder for void */
    int64_t v122;
    int64_t v133;
    int64_t v125; /* placeholder for void */
    int64_t v52;
    bool v55;
    bool v3;
    int64_t v94;
    int64_t v150;
    int64_t v20;
    bool v54;
    int64_t v61;
    int64_t v65;
    int64_t v153;
    int64_t v107;
    int64_t v44;
    int64_t v91;
    int64_t v159; /* placeholder for void */
    int64_t v188; /* placeholder for void */
    int64_t v191; /* placeholder for void */
    int64_t v146;
    int64_t v160; /* placeholder for void */
    void* v28;
    int64_t v76;
    uint8_t v37;
    void* v13;
    int64_t v118;
    int64_t v121; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v194; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v21;
    int64_t v114;
    int64_t v134;
    int64_t v135;
    int64_t v137;
    void* v101;
    int64_t v110;
    void* v78;
    int64_t v99; /* placeholder for void */
    int64_t v164; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v149;
    int64_t v93;
    int64_t v156; /* placeholder for void */
    int64_t v73;
    bool v120;
    bool v151;
    int64_t v71; /* placeholder for void */
    bool v51;
    int64_t v169;
    bool v50;
    int64_t v72;
    int64_t v18; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    void* v6;
    int64_t v29; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    void* v84;
    void* v22;
    int64_t v83; /* placeholder for void */
    int64_t v163; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v123;
    int64_t v187; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    void* v189;
    int64_t v193;
    void* v186;
    int64_t v60; /* placeholder for void */
    int64_t v127;
    int64_t v87; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    uint8_t v68;
    void* v139;
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

bool Option_is_none(void* v0, void* v1) {
    p_call_stack_push("Option_is_none");
    void* v11;
    int64_t v6; /* placeholder for void */
    void* v2;
    bool v9;
    bool v16;
    int64_t v17; /* placeholder for void */
    int64_t v8;
    int64_t v12;
    bool v5;
    int64_t v20; /* placeholder for void */
    int64_t v4;
    int64_t v10; /* placeholder for void */
    bool v14;
    void* v7;
    int64_t v13;
    int64_t v15; /* placeholder for void */
    bool v19;
    int64_t v3;
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

void ByteBuffer_from_string(void* v0, void* v1, void* v2) {
    p_call_stack_push("ByteBuffer_from_string");
    void* v46;
    void* v7;
    void* v47;
    void* v48;
    int64_t v15; /* placeholder for void */
    int64_t v17;
    int64_t v25;
    uint8_t v33;
    void* v36;
    int64_t v40;
    int64_t v41;
    void* v51;
    int64_t v53; /* placeholder for void */
    void* v23;
    int64_t v20; /* placeholder for void */
    int64_t v38;
    int64_t v44; /* placeholder for void */
    void* v32;
    void* v29;
    void* v50;
    int64_t v3;
    int64_t v35; /* placeholder for void */
    int64_t v5;
    int64_t v24; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    void* v11;
    void* v4;
    int64_t v12; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v52;
    int64_t v22;
    int64_t v21;
    int64_t v10;
    bool v19;
    int64_t v18;
    int64_t v28;
    int64_t v37;
    int64_t v42;
    int64_t v49; /* placeholder for void */
    int64_t v13;
    int64_t v27; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    void* v14;
    void* v26;
    int64_t v9;
    int64_t v43; /* placeholder for void */
    void* v45;
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

bool ByteBuffer_is_empty(void* v0, void* v1) {
    p_call_stack_push("ByteBuffer_is_empty");
    int64_t v3;
    int64_t v6; /* placeholder for void */
    void* v2;
    int64_t v4;
    bool v5;
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    p_call_stack_pop();
    return v5;
    return 0;
}

void vec3_lerp(void* v0, void* v1, void* v2, void* v3, double v4) {
    p_call_stack_push("vec3_lerp");
    void* v47;
    double v20;
    int64_t v38; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    double v23;
    double v11;
    double v22;
    void* v5;
    void* v40;
    void* v43;
    double v13;
    void* v17;
    void* v8;
    double v33;
    double v45;
    void* v26;
    double v29;
    void* v37;
    void* v48;
    double v41;
    void* v10;
    void* v21;
    double v49;
    int64_t v50; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    double v7;
    void* v15;
    int64_t v16; /* placeholder for void */
    void* v19;
    double v24;
    void* v30;
    double v31;
    void* v32;
    void* v28;
    double v35;
    void* v6;
    double v25;
    double v14;
    double v9;
    double v12;
    double v34;
    double v18;
    double v36;
    void* v39;
    int64_t v42; /* placeholder for void */
    void* v44;
    int64_t v46; /* placeholder for void */
    v5 = malloc(24);
    v6 = (char*)v2 + 0;
    v7 = *(double*)v6;
    v8 = (char*)v3 + 0;
    v9 = *(double*)v8;
    v10 = (char*)v2 + 0;
    v11 = *(double*)v10;
    v12 = v9 - v11;
    v13 = v12 * v4;
    v14 = v7 + v13;
    v15 = (char*)v5 + 0;
    *(double*)v15 = v14;
    v17 = (char*)v2 + 8;
    v18 = *(double*)v17;
    v19 = (char*)v3 + 8;
    v20 = *(double*)v19;
    v21 = (char*)v2 + 8;
    v22 = *(double*)v21;
    v23 = v20 - v22;
    v24 = v23 * v4;
    v25 = v18 + v24;
    v26 = (char*)v5 + 8;
    *(double*)v26 = v25;
    v28 = (char*)v2 + 16;
    v29 = *(double*)v28;
    v30 = (char*)v3 + 16;
    v31 = *(double*)v30;
    v32 = (char*)v2 + 16;
    v33 = *(double*)v32;
    v34 = v31 - v33;
    v35 = v34 * v4;
    v36 = v29 + v35;
    v37 = (char*)v5 + 16;
    *(double*)v37 = v36;
    v39 = (char*)v5 + 0;
    v40 = (char*)v0 + 0;
    v41 = *(double*)v39;
    *(double*)v40 = v41;
    v43 = (char*)v5 + 8;
    v44 = (char*)v0 + 8;
    v45 = *(double*)v43;
    *(double*)v44 = v45;
    v47 = (char*)v5 + 16;
    v48 = (char*)v0 + 16;
    v49 = *(double*)v47;
    *(double*)v48 = v49;
    p_call_stack_pop();
    return;
    return;
}

double math_sqrt(void* v0, double v1) {
    p_call_stack_push("math_sqrt");
    int64_t v3; /* placeholder for void */
    double v2;
    v2 = (double)pledge_sqrt(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void* str_split(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_split");
    int64_t v106;
    int64_t v33;
    int64_t v47;
    int64_t v7;
    int64_t v87; /* placeholder for void */
    void* v70;
    void* v12;
    void* v43;
    int64_t v28; /* placeholder for void */
    bool v10;
    void* v4;
    int64_t v24; /* placeholder for void */
    int64_t v119;
    int64_t v51;
    bool v82;
    int64_t v107; /* placeholder for void */
    int64_t v120;
    int64_t v8;
    int64_t v20; /* placeholder for void */
    int64_t v102;
    int64_t v101; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    void* v61;
    int64_t v35;
    int64_t v97;
    bool v39;
    int64_t v83; /* placeholder for void */
    int64_t v117;
    void* v122;
    int64_t v29;
    int64_t v68; /* placeholder for void */
    void* v23;
    uint8_t v74;
    bool v37;
    void* v95;
    int64_t v92; /* placeholder for void */
    int64_t v98;
    int64_t v72; /* placeholder for void */
    int64_t v57;
    int64_t v38; /* placeholder for void */
    bool v84;
    int64_t v32; /* placeholder for void */
    void* v27;
    int64_t v36;
    void* v73;
    int64_t v75; /* placeholder for void */
    int64_t v9;
    uint8_t v77;
    int64_t v89;
    int64_t v91; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v66;
    int64_t v81;
    int64_t v111;
    int64_t v94; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v3;
    int64_t v42;
    void* v30;
    int64_t v26;
    void* v67;
    int64_t v60;
    void* v15;
    int64_t v62; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    void* v64;
    int64_t v25;
    int64_t v99;
    int64_t v45; /* placeholder for void */
    int64_t v121; /* placeholder for void */
    void* v5;
    bool v93;
    int64_t v44; /* placeholder for void */
    bool v48;
    int64_t v109;
    int64_t v13;
    int64_t v85; /* placeholder for void */
    int64_t v114; /* placeholder for void */
    int64_t v18;
    void* v58;
    int64_t v86; /* placeholder for void */
    int64_t v34;
    int64_t v79; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v118;
    int64_t v63;
    int64_t v104;
    int64_t v53;
    int64_t v112; /* placeholder for void */
    int64_t v113; /* placeholder for void */
    void* v76;
    int64_t v90;
    int64_t v80;
    int64_t v31; /* placeholder for void */
    int64_t v105; /* placeholder for void */
    void* v40;
    int64_t v52;
    int64_t v22;
    int64_t v88;
    void* v115;
    void* v54;
    int64_t v65; /* placeholder for void */
    int64_t v103;
    int64_t v69;
    void* v19;
    int64_t v110;
    int64_t v21;
    int64_t v41; /* placeholder for void */
    int64_t v123; /* placeholder for void */
    int64_t v116;
    int64_t v50;
    int64_t v96;
    int64_t v108; /* placeholder for void */
    int64_t v46;
    int64_t v100;
    int64_t v56;
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

void* Vec_to_str(void* v0, int64_t v1) {
    p_call_stack_push("Vec_to_str");
    int64_t v68; /* placeholder for void */
    void* v61;
    int64_t v65;
    void* v14;
    bool v33;
    int64_t v19; /* placeholder for void */
    int64_t v2;
    int64_t v28;
    int64_t v43;
    int64_t v44;
    int64_t v45;
    void* v71;
    void* v20;
    int64_t v5; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v56;
    int64_t v50; /* placeholder for void */
    void* v4;
    void* v21;
    int64_t v51;
    int64_t v15; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v32;
    int64_t v31;
    int64_t v54; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    bool v29;
    void* v55;
    void* v46;
    int64_t v47; /* placeholder for void */
    int64_t v60;
    int64_t v74; /* placeholder for void */
    int64_t v42;
    void* v49;
    int64_t v58; /* placeholder for void */
    int64_t v48;
    int64_t v39; /* placeholder for void */
    void* v73;
    int64_t v9; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    void* v37;
    void* v16;
    int64_t v53; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v23;
    int64_t v27;
    void* v70;
    int64_t v72; /* placeholder for void */
    void* v36;
    int64_t v64;
    int64_t v66;
    int64_t v18; /* placeholder for void */
    int64_t v3;
    void* v8;
    void* v35;
    void* v10;
    int64_t v17;
    int64_t v11;
    void* v24;
    int64_t v41;
    void* v69;
    int64_t v26; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    void* v59;
    int64_t v63; /* placeholder for void */
    void* v6;
    int64_t v22; /* placeholder for void */
    void* v62;
    void* v52;
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
    v20 = _str10;
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
    v70 = _str11;
    v71 = p_str_concat(v69, v70);
    *(void**)v21 = (void*)v71;
    v73 = *(void**)v21;
    p_call_stack_pop();
    return v73;
  bb4:
    v35 = *(void**)v21;
    v36 = _str4;
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

void* str_repeat(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("str_repeat");
    int64_t v3;
    int64_t v4;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)String_repeat((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return (void*)v4;
    return 0;
}

void assert(void* v0, bool v1) {
    p_call_stack_push("assert");
    void* v4;
    int64_t v6; /* placeholder for void */
    int64_t v2;
    int64_t v8; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v3; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    v2 = !v1;
    if (v2) goto bb1;
    else goto bb2;
  bb1:
    v4 = _str12;
    p_print_str(v4);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return;
    return;
}

double math_max_f64(void* v0, double v1, double v2) {
    p_call_stack_push("math_max_f64");
    int64_t v4; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    bool v3;
    int64_t v7; /* placeholder for void */
    v3 = v1 > v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return v1;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t Vec_clone(void* v0, int64_t v1) {
    p_call_stack_push("Vec_clone");
    int64_t v50;
    int64_t v5; /* placeholder for void */
    int64_t v20;
    int64_t v12; /* placeholder for void */
    void* v22;
    int64_t v32;
    int64_t v7; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v34;
    int64_t v39;
    int64_t v49; /* placeholder for void */
    int64_t v11;
    int64_t v54;
    int64_t v47;
    int64_t v48; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    void* v37;
    int64_t v53; /* placeholder for void */
    int64_t v29;
    int64_t v58; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    void* v14;
    int64_t v21;
    int64_t v24;
    int64_t v33;
    void* v25;
    int64_t v44; /* placeholder for void */
    int64_t v51;
    int64_t v35;
    bool v30;
    void* v43;
    void* v46;
    int64_t v42;
    int64_t v28;
    int64_t v19; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v36;
    void* v16;
    void* v40;
    int64_t v59;
    void* v4;
    int64_t v55;
    int64_t v17;
    int64_t v26; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v2;
    int64_t v38; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v52;
    int64_t v57; /* placeholder for void */
    void* v10;
    void* v8;
    void* v6;
    int64_t v3;
    int64_t v56;
    int64_t v60; /* placeholder for void */
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

int64_t String_to_string(void* v0, int64_t v1) {
    p_call_stack_push("String_to_string");
    int64_t v4; /* placeholder for void */
    int64_t v3;
    int64_t v2;
    v2 = 0LL;
    v3 = (int64_t)String_from((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t String_byte_at(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_byte_at");
    void* v34;
    int64_t v29; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v27;
    int64_t v5; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v7;
    bool v4;
    int64_t v11;
    int64_t v21;
    bool v15;
    int64_t v13; /* placeholder for void */
    int64_t v20;
    int64_t v30;
    bool v18;
    uint8_t v35;
    void* v31;
    int64_t v36; /* placeholder for void */
    int64_t v6;
    bool v17;
    int64_t v22; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v24;
    int64_t v9; /* placeholder for void */
    int64_t v10;
    void* v28;
    int64_t v3;
    void* v12;
    int64_t v16;
    int64_t v14;
    int64_t v37; /* placeholder for void */
    int64_t v38;
    int64_t v39; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    void* v25;
    int64_t v26; /* placeholder for void */
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

bool str_equal(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_equal");
    int64_t v5; /* placeholder for void */
    bool v4;
    int64_t v3;
    v3 = 0LL;
    v4 = (bool)p_str_equal((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

void vec4_normalize(void* v0, void* v1, void* v2) {
    p_call_stack_push("vec4_normalize");
    double v58;
    int64_t v23; /* placeholder for void */
    void* v67;
    void* v68;
    void* v13;
    double v62;
    double v81;
    double v12;
    double v52;
    double v63;
    double v44;
    int64_t v39; /* placeholder for void */
    int64_t v3;
    int64_t v60; /* placeholder for void */
    double v15;
    double v77;
    void* v22;
    void* v71;
    void* v29;
    void* v65;
    double v38;
    int64_t v17; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    void* v76;
    double v30;
    double v46;
    int64_t v70; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    double v50;
    int64_t v31; /* placeholder for void */
    void* v11;
    void* v42;
    void* v16;
    int64_t v82; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    void* v47;
    int64_t v40; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    double v64;
    void* v59;
    void* v61;
    void* v5;
    void* v75;
    void* v19;
    double v69;
    double v18;
    int64_t v10; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    double v56;
    void* v79;
    void* v55;
    void* v72;
    double v45;
    void* v36;
    void* v37;
    bool v9;
    double v21;
    void* v24;
    void* v28;
    int64_t v48; /* placeholder for void */
    void* v32;
    double v4;
    double v8;
    int64_t v14; /* placeholder for void */
    double v51;
    double v73;
    int64_t v66; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    double v7;
    void* v25;
    void* v43;
    void* v80;
    double v26;
    double v57;
    int64_t v20; /* placeholder for void */
    void* v53;
    double v34;
    int64_t v54; /* placeholder for void */
    void* v33;
    void* v49;
    v3 = 0LL;
    v4 = (double)vec4_length((void*)v3, v2);
    v5 = malloc(8);
    *(double*)v5 = v4;
    v7 = *(double*)v5;
    v8 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v9 = v7 == v8;
    if (v9) goto bb1;
    else goto bb2;
  bb1:
    v11 = malloc(32);
    v12 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v13 = (char*)v11 + 0;
    *(double*)v13 = v12;
    v15 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v16 = (char*)v11 + 8;
    *(double*)v16 = v15;
    v18 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v19 = (char*)v11 + 16;
    *(double*)v19 = v18;
    v21 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v22 = (char*)v11 + 24;
    *(double*)v22 = v21;
    v24 = (char*)v11 + 0;
    v25 = (char*)v0 + 0;
    v26 = *(double*)v24;
    *(double*)v25 = v26;
    v28 = (char*)v11 + 8;
    v29 = (char*)v0 + 8;
    v30 = *(double*)v28;
    *(double*)v29 = v30;
    v32 = (char*)v11 + 16;
    v33 = (char*)v0 + 16;
    v34 = *(double*)v32;
    *(double*)v33 = v34;
    v36 = (char*)v11 + 24;
    v37 = (char*)v0 + 24;
    v38 = *(double*)v36;
    *(double*)v37 = v38;
    p_call_stack_pop();
    return;
  bb2:
    goto bb3;
  bb3:
    v42 = malloc(32);
    v43 = (char*)v2 + 0;
    v44 = *(double*)v43;
    v45 = *(double*)v5;
    v46 = v44 / v45;
    v47 = (char*)v42 + 0;
    *(double*)v47 = v46;
    v49 = (char*)v2 + 8;
    v50 = *(double*)v49;
    v51 = *(double*)v5;
    v52 = v50 / v51;
    v53 = (char*)v42 + 8;
    *(double*)v53 = v52;
    v55 = (char*)v2 + 16;
    v56 = *(double*)v55;
    v57 = *(double*)v5;
    v58 = v56 / v57;
    v59 = (char*)v42 + 16;
    *(double*)v59 = v58;
    v61 = (char*)v2 + 24;
    v62 = *(double*)v61;
    v63 = *(double*)v5;
    v64 = v62 / v63;
    v65 = (char*)v42 + 24;
    *(double*)v65 = v64;
    v67 = (char*)v42 + 0;
    v68 = (char*)v0 + 0;
    v69 = *(double*)v67;
    *(double*)v68 = v69;
    v71 = (char*)v42 + 8;
    v72 = (char*)v0 + 8;
    v73 = *(double*)v71;
    *(double*)v72 = v73;
    v75 = (char*)v42 + 16;
    v76 = (char*)v0 + 16;
    v77 = *(double*)v75;
    *(double*)v76 = v77;
    v79 = (char*)v42 + 24;
    v80 = (char*)v0 + 24;
    v81 = *(double*)v79;
    *(double*)v80 = v81;
    p_call_stack_pop();
    return;
    return;
}

double math_exp(void* v0, double v1) {
    p_call_stack_push("math_exp");
    int64_t v3; /* placeholder for void */
    double v2;
    v2 = (double)pledge_exp(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void mat4_identity(void* v0, void* v1) {
    p_call_stack_push("mat4_identity");
    double v45;
    void* v72;
    double v39;
    void* v100;
    void* v52;
    void* v88;
    int64_t v102; /* placeholder for void */
    double v61;
    double v109;
    double v89;
    int64_t v50; /* placeholder for void */
    int64_t v110; /* placeholder for void */
    int64_t v114; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    void* v64;
    double v69;
    void* v4;
    void* v2;
    void* v63;
    double v105;
    int64_t v94; /* placeholder for void */
    double v101;
    void* v80;
    void* v95;
    void* v111;
    void* v112;
    void* v83;
    int64_t v70; /* placeholder for void */
    void* v75;
    double v18;
    int64_t v26; /* placeholder for void */
    void* v107;
    void* v55;
    void* v25;
    void* v60;
    int64_t v54; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    double v36;
    double v57;
    int64_t v58; /* placeholder for void */
    void* v59;
    double v42;
    double v9;
    void* v31;
    double v33;
    void* v51;
    double v48;
    int64_t v5; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    void* v43;
    double v113;
    void* v19;
    double v6;
    void* v40;
    int64_t v86; /* placeholder for void */
    void* v37;
    void* v7;
    void* v79;
    void* v67;
    double v21;
    int64_t v90; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    void* v49;
    double v77;
    void* v16;
    void* v71;
    void* v84;
    double v93;
    void* v96;
    double v73;
    double v30;
    double v65;
    int64_t v8; /* placeholder for void */
    double v24;
    int64_t v47; /* placeholder for void */
    double v53;
    int64_t v41; /* placeholder for void */
    void* v46;
    double v97;
    int64_t v44; /* placeholder for void */
    void* v99;
    void* v10;
    double v15;
    int64_t v115; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    void* v56;
    int64_t v17; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    void* v108;
    void* v34;
    int64_t v23; /* placeholder for void */
    void* v22;
    int64_t v78; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    void* v92;
    void* v68;
    int64_t v35; /* placeholder for void */
    double v3;
    void* v28;
    void* v104;
    void* v76;
    double v81;
    void* v87;
    void* v91;
    int64_t v62; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    double v12;
    int64_t v32; /* placeholder for void */
    void* v103;
    int64_t v82; /* placeholder for void */
    double v27;
    void* v13;
    double v85;
    v2 = malloc(128);
    v3 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v4 = (char*)v2 + 0;
    *(double*)v4 = v3;
    v6 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v7 = (char*)v2 + 8;
    *(double*)v7 = v6;
    v9 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v10 = (char*)v2 + 16;
    *(double*)v10 = v9;
    v12 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v13 = (char*)v2 + 24;
    *(double*)v13 = v12;
    v15 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v16 = (char*)v2 + 32;
    *(double*)v16 = v15;
    v18 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v19 = (char*)v2 + 40;
    *(double*)v19 = v18;
    v21 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v22 = (char*)v2 + 48;
    *(double*)v22 = v21;
    v24 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v25 = (char*)v2 + 56;
    *(double*)v25 = v24;
    v27 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v28 = (char*)v2 + 64;
    *(double*)v28 = v27;
    v30 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v31 = (char*)v2 + 72;
    *(double*)v31 = v30;
    v33 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v34 = (char*)v2 + 80;
    *(double*)v34 = v33;
    v36 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v37 = (char*)v2 + 88;
    *(double*)v37 = v36;
    v39 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v40 = (char*)v2 + 96;
    *(double*)v40 = v39;
    v42 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v43 = (char*)v2 + 104;
    *(double*)v43 = v42;
    v45 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v46 = (char*)v2 + 112;
    *(double*)v46 = v45;
    v48 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v49 = (char*)v2 + 120;
    *(double*)v49 = v48;
    v51 = (char*)v2 + 0;
    v52 = (char*)v0 + 0;
    v53 = *(double*)v51;
    *(double*)v52 = v53;
    v55 = (char*)v2 + 8;
    v56 = (char*)v0 + 8;
    v57 = *(double*)v55;
    *(double*)v56 = v57;
    v59 = (char*)v2 + 16;
    v60 = (char*)v0 + 16;
    v61 = *(double*)v59;
    *(double*)v60 = v61;
    v63 = (char*)v2 + 24;
    v64 = (char*)v0 + 24;
    v65 = *(double*)v63;
    *(double*)v64 = v65;
    v67 = (char*)v2 + 32;
    v68 = (char*)v0 + 32;
    v69 = *(double*)v67;
    *(double*)v68 = v69;
    v71 = (char*)v2 + 40;
    v72 = (char*)v0 + 40;
    v73 = *(double*)v71;
    *(double*)v72 = v73;
    v75 = (char*)v2 + 48;
    v76 = (char*)v0 + 48;
    v77 = *(double*)v75;
    *(double*)v76 = v77;
    v79 = (char*)v2 + 56;
    v80 = (char*)v0 + 56;
    v81 = *(double*)v79;
    *(double*)v80 = v81;
    v83 = (char*)v2 + 64;
    v84 = (char*)v0 + 64;
    v85 = *(double*)v83;
    *(double*)v84 = v85;
    v87 = (char*)v2 + 72;
    v88 = (char*)v0 + 72;
    v89 = *(double*)v87;
    *(double*)v88 = v89;
    v91 = (char*)v2 + 80;
    v92 = (char*)v0 + 80;
    v93 = *(double*)v91;
    *(double*)v92 = v93;
    v95 = (char*)v2 + 88;
    v96 = (char*)v0 + 88;
    v97 = *(double*)v95;
    *(double*)v96 = v97;
    v99 = (char*)v2 + 96;
    v100 = (char*)v0 + 96;
    v101 = *(double*)v99;
    *(double*)v100 = v101;
    v103 = (char*)v2 + 104;
    v104 = (char*)v0 + 104;
    v105 = *(double*)v103;
    *(double*)v104 = v105;
    v107 = (char*)v2 + 112;
    v108 = (char*)v0 + 112;
    v109 = *(double*)v107;
    *(double*)v108 = v109;
    v111 = (char*)v2 + 120;
    v112 = (char*)v0 + 120;
    v113 = *(double*)v111;
    *(double*)v112 = v113;
    p_call_stack_pop();
    return;
    return;
}

int64_t Option_unwrap_or(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Option_unwrap_or");
    void* v8;
    bool v6;
    int64_t v4;
    int64_t v10; /* placeholder for void */
    int64_t v12;
    int64_t v16; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v9;
    int64_t v19; /* placeholder for void */
    void* v11;
    int64_t v18;
    int64_t v17; /* placeholder for void */
    int64_t v13;
    int64_t v15; /* placeholder for void */
    bool v14;
    int64_t v5;
    void* v3;
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

bool Result_is_err(void* v0, void* v1) {
    p_call_stack_push("Result_is_err");
    int64_t v12;
    int64_t v15; /* placeholder for void */
    void* v2;
    void* v7;
    int64_t v17;
    int64_t v10; /* placeholder for void */
    bool v5;
    bool v9;
    int64_t v6; /* placeholder for void */
    int64_t v13;
    bool v14;
    int64_t v20; /* placeholder for void */
    bool v21;
    int64_t v22; /* placeholder for void */
    void* v16;
    bool v18;
    int64_t v19; /* placeholder for void */
    int64_t v3;
    void* v11;
    int64_t v4;
    int64_t v8;
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

int64_t HashMap_keys(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_keys");
    int64_t v49;
    void* v9;
    int64_t v47;
    int64_t v79;
    void* v6;
    void* v24;
    int64_t v81; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    bool v31;
    int64_t v62; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v26;
    int64_t v29;
    int64_t v35; /* placeholder for void */
    bool v65;
    int64_t v78;
    int64_t v84; /* placeholder for void */
    int64_t v2;
    int64_t v22;
    bool v32;
    int64_t v68;
    int64_t v37;
    int64_t v39; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    void* v50;
    int64_t v5; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v58; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    void* v15;
    int64_t v60;
    bool v70;
    int64_t v66;
    int64_t v55;
    int64_t v30;
    int64_t v76; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    void* v38;
    int64_t v52;
    int64_t v54; /* placeholder for void */
    bool v69;
    int64_t v73;
    int64_t v82; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v8;
    void* v12;
    int64_t v71; /* placeholder for void */
    int64_t v3;
    int64_t v17; /* placeholder for void */
    int64_t v63;
    void* v4;
    int64_t v33; /* placeholder for void */
    int64_t v72;
    int64_t v75; /* placeholder for void */
    int64_t v41;
    int64_t v23;
    int64_t v7; /* placeholder for void */
    int64_t v11;
    int64_t v34;
    void* v53;
    int64_t v36; /* placeholder for void */
    int64_t v19;
    int64_t v42;
    bool v43;
    int64_t v16;
    int64_t v48;
    int64_t v64;
    int64_t v45;
    int64_t v67;
    int64_t v74;
    int64_t v14; /* placeholder for void */
    int64_t v27;
    void* v18;
    void* v59;
    int64_t v44; /* placeholder for void */
    int64_t v61; /* placeholder for void */
    bool v28;
    int64_t v46;
    int64_t v80;
    int64_t v77; /* placeholder for void */
    void* v56;
    int64_t v83;
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

void* StringBuilder_to_str(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_to_str");
    void* v3;
    int64_t v4; /* placeholder for void */
    void* v2;
    v2 = (char*)v1 + 0;
    v3 = *(void**)v2;
    p_call_stack_pop();
    return v3;
    return 0;
}

bool str_ends_with(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_ends_with");
    int64_t v3;
    bool v4;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (bool)String_ends_with((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

void StringBuilder_append_str(void* v0, void* v1, void* v2) {
    p_call_stack_push("StringBuilder_append_str");
    int64_t v14; /* placeholder for void */
    int64_t v10;
    int64_t v12;
    void* v3;
    void* v8;
    void* v6;
    void* v5;
    void* v4;
    void* v9;
    int64_t v11;
    int64_t v15; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v13;
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

double vec3_length(void* v0, void* v1) {
    p_call_stack_push("vec3_length");
    double v17;
    void* v7;
    double v6;
    double v18;
    double v8;
    double v16;
    double v10;
    void* v4;
    double v5;
    void* v2;
    void* v13;
    double v19;
    double v3;
    double v14;
    int64_t v20; /* placeholder for void */
    void* v15;
    double v12;
    double v11;
    void* v9;
    v2 = (char*)v1 + 0;
    v3 = *(double*)v2;
    v4 = (char*)v1 + 0;
    v5 = *(double*)v4;
    v6 = v3 * v5;
    v7 = (char*)v1 + 8;
    v8 = *(double*)v7;
    v9 = (char*)v1 + 8;
    v10 = *(double*)v9;
    v11 = v8 * v10;
    v12 = v6 + v11;
    v13 = (char*)v1 + 16;
    v14 = *(double*)v13;
    v15 = (char*)v1 + 16;
    v16 = *(double*)v15;
    v17 = v14 * v16;
    v18 = v12 + v17;
    v19 = (double)pledge_sqrt(v18);
    p_call_stack_pop();
    return v19;
    return 0;
}

void Vec_insert(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("Vec_insert");
    int64_t v4;
    int64_t v49; /* placeholder for void */
    int64_t v46;
    int64_t v65;
    int64_t v66;
    int64_t v102; /* placeholder for void */
    int64_t v23;
    void* v58;
    void* v73;
    int64_t v74; /* placeholder for void */
    void* v10;
    int64_t v87; /* placeholder for void */
    int64_t v92;
    int64_t v60;
    int64_t v77;
    int64_t v50;
    void* v47;
    int64_t v57;
    int64_t v59; /* placeholder for void */
    void* v67;
    int64_t v44;
    int64_t v53;
    int64_t v63;
    int64_t v88; /* placeholder for void */
    int64_t v84;
    int64_t v95;
    int64_t v36; /* placeholder for void */
    int64_t v62;
    int64_t v43; /* placeholder for void */
    void* v34;
    void* v19;
    int64_t v68; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    bool v17;
    int64_t v25; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v90;
    int64_t v55;
    int64_t v31;
    void* v26;
    int64_t v35;
    void* v40;
    int64_t v22; /* placeholder for void */
    void* v78;
    int64_t v64;
    int64_t v83; /* placeholder for void */
    int64_t v86;
    int64_t v94; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v91;
    int64_t v61;
    void* v8;
    int64_t v32; /* placeholder for void */
    int64_t v5;
    int64_t v11; /* placeholder for void */
    int64_t v41;
    int64_t v18; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v56;
    void* v6;
    int64_t v21; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    int64_t v81;
    int64_t v69;
    int64_t v33; /* placeholder for void */
    void* v96;
    void* v99;
    int64_t v14; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v30;
    void* v93;
    int64_t v98; /* placeholder for void */
    bool v24;
    void* v76;
    void* v80;
    int64_t v82; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    int64_t v85;
    int64_t v79; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v100; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v70;
    int64_t v45;
    bool v51;
    int64_t v54;
    void* v38;
    int64_t v15; /* placeholder for void */
    int64_t v16;
    void* v12;
    int64_t v89;
    int64_t v72;
    int64_t v13;
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
    v19 = _str13;
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
    v26 = _str13;
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

int64_t Vec_first(void* v0, int64_t v1) {
    p_call_stack_push("Vec_first");
    int64_t v14;
    int64_t v20; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v36;
    int64_t v19; /* placeholder for void */
    void* v8;
    int64_t v30; /* placeholder for void */
    void* v4;
    int64_t v5; /* placeholder for void */
    int64_t v15;
    int64_t v7; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v35;
    int64_t v39;
    void* v29;
    void* v10;
    int64_t v28;
    int64_t v27; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    void* v22;
    void* v18;
    int64_t v3;
    int64_t v21; /* placeholder for void */
    int64_t v25;
    int64_t v11;
    int64_t v12; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    bool v16;
    int64_t v2;
    int64_t v23; /* placeholder for void */
    int64_t v31;
    void* v32;
    int64_t v13; /* placeholder for void */
    void* v6;
    int64_t v37; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    void* v24;
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
    v18 = _str6;
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

int64_t allocator_bytes_used(void* v0, void* v1) {
    p_call_stack_push("allocator_bytes_used");
    int64_t v2;
    int64_t v3; /* placeholder for void */
    v2 = (int64_t)p_allocator_stats(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

bool char_is_alpha(void* v0, int64_t v1) {
    p_call_stack_push("char_is_alpha");
    bool v11;
    bool v12;
    bool v5;
    bool v6;
    int64_t v2;
    bool v3;
    int64_t v7;
    bool v8;
    int64_t v13; /* placeholder for void */
    int64_t v9;
    bool v10;
    int64_t v4;
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

void HashMap_remove(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_remove");
    void* v53;
    void* v12;
    int64_t v14; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    int64_t v65; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v85;
    int64_t v70;
    int64_t v11;
    int64_t v22; /* placeholder for void */
    int64_t v41;
    int64_t v46;
    bool v59;
    int64_t v37; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    void* v69;
    void* v5;
    void* v15;
    void* v9;
    int64_t v56; /* placeholder for void */
    int64_t v80; /* placeholder for void */
    int64_t v31;
    int64_t v66; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    void* v29;
    bool v63;
    int64_t v57;
    int64_t v73; /* placeholder for void */
    int64_t v3;
    int64_t v6; /* placeholder for void */
    void* v47;
    int64_t v25;
    int64_t v76; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    void* v78;
    int64_t v43;
    int64_t v92; /* placeholder for void */
    void* v32;
    int64_t v4;
    int64_t v36;
    int64_t v45; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    void* v75;
    int64_t v62;
    int64_t v34; /* placeholder for void */
    int64_t v84;
    int64_t v86; /* placeholder for void */
    void* v50;
    int64_t v90; /* placeholder for void */
    int64_t v60;
    void* v88;
    int64_t v48; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v7;
    int64_t v23; /* placeholder for void */
    void* v44;
    int64_t v71;
    int64_t v30; /* placeholder for void */
    int64_t v42;
    int64_t v72; /* placeholder for void */
    int64_t v58;
    int64_t v16;
    int64_t v83;
    int64_t v54;
    int64_t v82;
    int64_t v40;
    int64_t v20;
    bool v21;
    int64_t v49;
    int64_t v24; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    int64_t v8;
    void* v27;
    int64_t v52; /* placeholder for void */
    int64_t v74;
    int64_t v79;
    int64_t v87; /* placeholder for void */
    void* v35;
    int64_t v13; /* placeholder for void */
    int64_t v19;
    int64_t v39;
    int64_t v61;
    bool v64;
    int64_t v26;
    int64_t v38; /* placeholder for void */
    int64_t v89;
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

bool char_is_upper(void* v0, int64_t v1) {
    p_call_stack_push("char_is_upper");
    int64_t v4;
    bool v5;
    int64_t v7; /* placeholder for void */
    int64_t v2;
    bool v3;
    bool v6;
    v2 = 65LL;
    v3 = v1 >= v2;
    v4 = 90LL;
    v5 = v1 <= v4;
    v6 = v3 && v5;
    p_call_stack_pop();
    return v6;
    return 0;
}

void Option_and_then(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Option_and_then");
    int64_t v46; /* placeholder for void */
    int64_t v24;
    void* v27;
    void* v32;
    void* v4;
    int64_t v15;
    int64_t v28;
    void* v13;
    void* v41;
    void* v37;
    void* v16;
    int64_t v25; /* placeholder for void */
    int64_t v29;
    int64_t v26; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v20;
    int64_t v21; /* placeholder for void */
    bool v7;
    int64_t v35; /* placeholder for void */
    void* v9;
    void* v19;
    int64_t v38;
    void* v22;
    int64_t v39; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v10;
    int64_t v17; /* placeholder for void */
    bool v30;
    void* v23;
    int64_t v6;
    int64_t v33;
    void* v40;
    int64_t v5;
    int64_t v42;
    void* v36;
    int64_t v12;
    int64_t v45; /* placeholder for void */
    void* v11;
    void* v18;
    int64_t v43; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v34;
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
        if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "math_abs_i64") == 0) {
            v15 = math_abs_i64((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
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

bool char_is_digit(void* v0, int64_t v1) {
    p_call_stack_push("char_is_digit");
    bool v6;
    int64_t v7; /* placeholder for void */
    int64_t v4;
    int64_t v2;
    bool v5;
    bool v3;
    v2 = 48LL;
    v3 = v1 >= v2;
    v4 = 57LL;
    v5 = v1 <= v4;
    v6 = v3 && v5;
    p_call_stack_pop();
    return v6;
    return 0;
}

int64_t str_parse_int_range(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("str_parse_int_range");
    int64_t v21;
    bool v22;
    int64_t v72;
    int64_t v38;
    int64_t v61;
    void* v51;
    int64_t v11; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v80;
    int64_t v40; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    bool v30;
    int64_t v43;
    int64_t v45; /* placeholder for void */
    int64_t v73;
    int64_t v24;
    void* v48;
    int64_t v16;
    int64_t v56; /* placeholder for void */
    int64_t v74;
    int64_t v84;
    int64_t v32;
    int64_t v65;
    bool v66;
    int64_t v69; /* placeholder for void */
    uint8_t v58;
    bool v67;
    int64_t v33; /* placeholder for void */
    int64_t v35;
    int64_t v53;
    int64_t v23; /* placeholder for void */
    int64_t v46;
    void* v39;
    int64_t v47;
    void* v36;
    bool v44;
    int64_t v13;
    int64_t v78; /* placeholder for void */
    void* v8;
    int64_t v26; /* placeholder for void */
    int64_t v85; /* placeholder for void */
    void* v54;
    int64_t v49; /* placeholder for void */
    int64_t v20;
    void* v57;
    int64_t v4;
    int64_t v12;
    int64_t v31; /* placeholder for void */
    int64_t v77;
    int64_t v28;
    int64_t v71;
    int64_t v83; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v62;
    int64_t v34; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    bool v63;
    int64_t v70; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v64;
    void* v10;
    int64_t v50;
    int64_t v27; /* placeholder for void */
    int64_t v75;
    int64_t v18; /* placeholder for void */
    int64_t v29;
    int64_t v55; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    bool v14;
    int64_t v37; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    int64_t v79;
    int64_t v81;
    int64_t v41; /* placeholder for void */
    int64_t v42;
    int64_t v76;
    int64_t v5;
    void* v6;
    int64_t v52; /* placeholder for void */
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

double degrees(void* v0, double v1) {
    p_call_stack_push("degrees");
    double v2;
    double v3;
    int64_t v4; /* placeholder for void */
    v2 = ((union { uint64_t u; double f; }){ .u = 4633260481411531256ULL }).f;
    v3 = v1 * v2;
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t main_entry(void* v0) {
    p_call_stack_push("main");
    int64_t v45; /* placeholder for void */
    int64_t v403; /* placeholder for void */
    void* v467;
    void* v220;
    double v869;
    void* v145;
    int64_t v797; /* placeholder for void */
    double v421;
    double v141;
    int64_t v539; /* placeholder for void */
    void* v621;
    int64_t v298; /* placeholder for void */
    void* v686;
    double v239;
    int64_t v53; /* placeholder for void */
    int64_t v603; /* placeholder for void */
    void* v612;
    double v8;
    int64_t v161; /* placeholder for void */
    int64_t v664; /* placeholder for void */
    void* v186;
    double v1;
    void* v326;
    void* v558;
    void* v534;
    int64_t v331; /* placeholder for void */
    int64_t v652; /* placeholder for void */
    void* v498;
    void* v854;
    int64_t v208; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v371; /* placeholder for void */
    int64_t v619;
    void* v484;
    void* v247;
    int64_t v269; /* placeholder for void */
    void* v532;
    void* v722;
    void* v404;
    int64_t v527; /* placeholder for void */
    int64_t v595; /* placeholder for void */
    void* v432;
    void* v752;
    void* v732;
    void* v66;
    void* v206;
    int64_t v785;
    void* v216;
    void* v767;
    void* v173;
    int64_t v456; /* placeholder for void */
    void* v139;
    void* v338;
    double v780;
    int64_t v559; /* placeholder for void */
    void* v372;
    void* v585;
    void* v508;
    int64_t v416; /* placeholder for void */
    double v397;
    void* v587;
    int64_t v327; /* placeholder for void */
    int64_t v597; /* placeholder for void */
    void* v226;
    void* v699;
    int64_t v769; /* placeholder for void */
    void* v166;
    int64_t v38;
    void* v20;
    void* v245;
    double v614;
    void* v346;
    double v826;
    void* v218;
    int64_t v493; /* placeholder for void */
    int64_t v852; /* placeholder for void */
    void* v668;
    void* v457;
    void* v485;
    int64_t v658; /* placeholder for void */
    void* v366;
    void* v450;
    void* v727;
    void* v749;
    int64_t v94;
    void* v179;
    int64_t v436; /* placeholder for void */
    int64_t v165; /* placeholder for void */
    int64_t v517; /* placeholder for void */
    int64_t v424; /* placeholder for void */
    void* v709;
    int64_t v688; /* placeholder for void */
    void* v745;
    void* v800;
    int64_t v198; /* placeholder for void */
    void* v158;
    int64_t v533; /* placeholder for void */
    void* v657;
    int64_t v408; /* placeholder for void */
    int64_t v175; /* placeholder for void */
    void* v464;
    int64_t v763; /* placeholder for void */
    int64_t v385; /* placeholder for void */
    double v572;
    int64_t v638; /* placeholder for void */
    double v623;
    int64_t v88; /* placeholder for void */
    double v370;
    void* v96;
    void* v51;
    void* v795;
    void* v642;
    void* v332;
    double v643;
    void* v110;
    void* v340;
    int64_t v644; /* placeholder for void */
    double v228;
    void* v700;
    int64_t v670; /* placeholder for void */
    void* v83;
    void* v645;
    double v861;
    void* v904;
    int64_t v68; /* placeholder for void */
    void* v544;
    void* v224;
    void* v554;
    void* v189;
    int64_t v90; /* placeholder for void */
    void* v654;
    int64_t v126; /* placeholder for void */
    void* v726;
    void* v535;
    void* v263;
    int64_t v4;
    double v207;
    void* v350;
    void* v489;
    double v624;
    void* v37;
    int64_t v47; /* placeholder for void */
    void* v811;
    void* v443;
    void* v547;
    void* v685;
    int64_t v76; /* placeholder for void */
    int64_t v439;
    void* v157;
    int64_t v219; /* placeholder for void */
    void* v712;
    void* v336;
    void* v131;
    int64_t v72; /* placeholder for void */
    void* v429;
    void* v494;
    int64_t v444; /* placeholder for void */
    void* v596;
    void* v463;
    void* v776;
    int64_t v171; /* placeholder for void */
    double v465;
    int64_t v901; /* placeholder for void */
    void* v841;
    void* v128;
    double v804;
    void* v512;
    int64_t v250; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v351; /* placeholder for void */
    void* v479;
    void* v607;
    void* v902;
    double v649;
    void* v802;
    void* v639;
    void* v236;
    int64_t v714; /* placeholder for void */
    double v44;
    void* v313;
    int64_t v27; /* placeholder for void */
    double v398;
    void* v414;
    int64_t v373; /* placeholder for void */
    double v824;
    int64_t v599; /* placeholder for void */
    void* v104;
    double v578;
    double v328;
    int64_t v640; /* placeholder for void */
    void* v660;
    int64_t v318; /* placeholder for void */
    void* v417;
    void* v453;
    int64_t v460; /* placeholder for void */
    void* v251;
    int64_t v567; /* placeholder for void */
    void* v290;
    void* v867;
    void* v873;
    int64_t v217; /* placeholder for void */
    double v471;
    void* v133;
    void* v193;
    int64_t v868; /* placeholder for void */
    int64_t v635;
    int64_t v60;
    void* v140;
    void* v170;
    double v893;
    void* v42;
    int64_t v401; /* placeholder for void */
    int64_t v650; /* placeholder for void */
    void* v794;
    void* v299;
    void* v715;
    int64_t v84; /* placeholder for void */
    int64_t v117; /* placeholder for void */
    int64_t v537; /* placeholder for void */
    double v586;
    double v695;
    int64_t v52;
    double v855;
    int64_t v33; /* placeholder for void */
    void* v684;
    double v504;
    double v878;
    int64_t v892; /* placeholder for void */
    void* v764;
    int64_t v63; /* placeholder for void */
    void* v738;
    void* v304;
    void* v197;
    void* v205;
    void* v842;
    void* v883;
    int64_t v357; /* placeholder for void */
    void* v706;
    double v305;
    void* v470;
    void* v254;
    void* v589;
    void* v69;
    int64_t v543; /* placeholder for void */
    void* v613;
    int64_t v858; /* placeholder for void */
    double v87;
    void* v303;
    void* v292;
    double v626;
    int64_t v529; /* placeholder for void */
    void* v243;
    void* v265;
    int64_t v805; /* placeholder for void */
    void* v65;
    void* v86;
    double v524;
    int64_t v793; /* placeholder for void */
    void* v814;
    int64_t v783; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v897;
    int64_t v765; /* placeholder for void */
    int64_t v480;
    void* v509;
    int64_t v241;
    int64_t v681; /* placeholder for void */
    void* v281;
    void* v560;
    int64_t v729; /* placeholder for void */
    void* v604;
    void* v469;
    int64_t v821; /* placeholder for void */
    void* v374;
    int64_t v322;
    void* v30;
    double v602;
    void* v641;
    void* v648;
    double v293;
    double v584;
    void* v806;
    int64_t v856; /* placeholder for void */
    int64_t v545; /* placeholder for void */
    int64_t v675; /* placeholder for void */
    double v348;
    void* v790;
    int64_t v507; /* placeholder for void */
    void* v719;
    int64_t v775; /* placeholder for void */
    void* v851;
    void* v875;
    void* v546;
    double v889;
    void* v352;
    int64_t v379; /* placeholder for void */
    void* v260;
    double v395;
    void* v176;
    double v812;
    int64_t v43; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    int64_t v631; /* placeholder for void */
    int64_t v882; /* placeholder for void */
    int64_t v422; /* placeholder for void */
    double v478;
    int64_t v495; /* placeholder for void */
    int64_t v662; /* placeholder for void */
    int64_t v557; /* placeholder for void */
    int64_t v152; /* placeholder for void */
    int64_t v481; /* placeholder for void */
    void* v409;
    void* v885;
    int64_t v499; /* placeholder for void */
    int64_t v285; /* placeholder for void */
    void* v62;
    void* v178;
    void* v598;
    int64_t v160;
    int64_t v777; /* placeholder for void */
    int64_t v898; /* placeholder for void */
    void* v143;
    double v168;
    int64_t v320; /* placeholder for void */
    void* v633;
    int64_t v41; /* placeholder for void */
    int64_t v903; /* placeholder for void */
    int64_t v306; /* placeholder for void */
    int64_t v18;
    int64_t v192; /* placeholder for void */
    int64_t v430; /* placeholder for void */
    int64_t v672;
    int64_t v628;
    int64_t v21; /* placeholder for void */
    int64_t v629; /* placeholder for void */
    void* v286;
    void* v137;
    void* v238;
    int64_t v291; /* placeholder for void */
    void* v502;
    void* v756;
    int64_t v899;
    void* v500;
    void* v742;
    int64_t v829; /* placeholder for void */
    void* v330;
    double v486;
    int64_t v181; /* placeholder for void */
    void* v123;
    int64_t v262; /* placeholder for void */
    int64_t v575; /* placeholder for void */
    void* v411;
    void* v36;
    void* v227;
    int64_t v418; /* placeholder for void */
    void* v582;
    void* v590;
    void* v718;
    int64_t v300; /* placeholder for void */
    int64_t v872; /* placeholder for void */
    double v900;
    double v566;
    int64_t v813; /* placeholder for void */
    double v56;
    int64_t v225; /* placeholder for void */
    void* v267;
    void* v40;
    void* v523;
    double v825;
    void* v491;
    int64_t v611; /* placeholder for void */
    double v625;
    int64_t v308; /* placeholder for void */
    void* v274;
    double v550;
    void* v344;
    int64_t v874; /* placeholder for void */
    void* v278;
    int64_t v696; /* placeholder for void */
    int64_t v876; /* placeholder for void */
    int64_t v434; /* placeholder for void */
    int64_t v407;
    void* v637;
    double v666;
    int64_t v886; /* placeholder for void */
    double v445;
    int64_t v735; /* placeholder for void */
    double v895;
    void* v134;
    void* v553;
    int64_t v698; /* placeholder for void */
    void* v159;
    int64_t v264; /* placeholder for void */
    void* v435;
    int64_t v501; /* placeholder for void */
    void* v73;
    void* v127;
    int64_t v646; /* placeholder for void */
    void* v803;
    double v57;
    double v871;
    int64_t v109; /* placeholder for void */
    double v387;
    void* v378;
    void* v730;
    void* v810;
    void* v857;
    int64_t v144; /* placeholder for void */
    void* v15;
    int64_t v692; /* placeholder for void */
    int64_t v741; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    void* v594;
    double v2;
    int64_t v555; /* placeholder for void */
    void* v562;
    int64_t v680;
    double v9;
    double v510;
    double v677;
    double v174;
    int64_t v864; /* placeholder for void */
    void* v358;
    void* v280;
    int64_t v483; /* placeholder for void */
    void* v31;
    void* v149;
    int64_t v329; /* placeholder for void */
    void* v28;
    double v820;
    int64_t v82; /* placeholder for void */
    int64_t v105; /* placeholder for void */
    double v386;
    int64_t v394; /* placeholder for void */
    void* v315;
    void* v3;
    int64_t v242; /* placeholder for void */
    void* v381;
    double v388;
    double v701;
    int64_t v229; /* placeholder for void */
    int64_t v383; /* placeholder for void */
    int64_t v334;
    int64_t v289; /* placeholder for void */
    void* v487;
    int64_t v807; /* placeholder for void */
    void* v232;
    int64_t v115; /* placeholder for void */
    double v48;
    double v667;
    double v678;
    void* v819;
    void* v694;
    double v879;
    int64_t v367; /* placeholder for void */
    void* v818;
    void* v248;
    int64_t v838; /* placeholder for void */
    void* v568;
    void* v647;
    void* v601;
    int64_t v142; /* placeholder for void */
    int64_t v748; /* placeholder for void */
    int64_t v866; /* placeholder for void */
    void* v100;
    double v129;
    void* v431;
    void* v616;
    void* v792;
    int64_t v884; /* placeholder for void */
    int64_t v244; /* placeholder for void */
    void* v272;
    void* v717;
    int64_t v755; /* placeholder for void */
    double v354;
    int64_t v335; /* placeholder for void */
    void* v296;
    void* v74;
    void* v341;
    int64_t v789; /* placeholder for void */
    int64_t v11;
    int64_t v363; /* placeholder for void */
    double v877;
    void* v182;
    void* v101;
    int64_t v801; /* placeholder for void */
    void* v520;
    void* v211;
    void* v549;
    double v707;
    void* v541;
    double v779;
    void* v528;
    void* v162;
    void* v54;
    int64_t v271; /* placeholder for void */
    void* v85;
    int64_t v771; /* placeholder for void */
    int64_t v343; /* placeholder for void */
    void* v441;
    int64_t v410; /* placeholder for void */
    void* v746;
    void* v653;
    int64_t v35; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    double v288;
    double v843;
    void* v576;
    void* v353;
    int64_t v472; /* placeholder for void */
    double v881;
    void* v194;
    void* v570;
    double v276;
    void* v113;
    int64_t v721; /* placeholder for void */
    void* v309;
    int64_t v339; /* placeholder for void */
    int64_t v860; /* placeholder for void */
    int64_t v365; /* placeholder for void */
    void* v674;
    void* v125;
    int64_t v474; /* placeholder for void */
    void* v565;
    int64_t v297;
    double v786;
    void* v405;
    int64_t v124; /* placeholder for void */
    int64_t v823; /* placeholder for void */
    void* v77;
    int64_t v704; /* placeholder for void */
    int64_t v391;
    void* v760;
    int64_t v561; /* placeholder for void */
    int64_t v283; /* placeholder for void */
    int64_t v828;
    void* v301;
    double v148;
    void* v632;
    int64_t v836; /* placeholder for void */
    void* v772;
    int64_t v710; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    double v49;
    void* v380;
    int64_t v29; /* placeholder for void */
    int64_t v103; /* placeholder for void */
    int64_t v782;
    void* v845;
    void* v257;
    void* v200;
    int64_t v345; /* placeholder for void */
    int64_t v581; /* placeholder for void */
    void* v890;
    void* v240;
    int64_t v122; /* placeholder for void */
    double v58;
    void* v369;
    double v342;
    int64_t v55; /* placeholder for void */
    double v734;
    int64_t v169; /* placeholder for void */
    int64_t v605; /* placeholder for void */
    double v75;
    void* v832;
    void* v10;
    void* v209;
    void* v390;
    void* v515;
    int64_t v656; /* placeholder for void */
    int64_t v452; /* placeholder for void */
    void* v13;
    int64_t v177; /* placeholder for void */
    int64_t v817; /* placeholder for void */
    double v475;
    int64_t v708; /* placeholder for void */
    void* v724;
    double v311;
    void* v98;
    int64_t v442; /* placeholder for void */
    double v360;
    void* v571;
    double v389;
    void* v384;
    int64_t v905; /* placeholder for void */
    void* v671;
    int64_t v870;
    int64_t v844; /* placeholder for void */
    void* v59;
    void* v356;
    void* v577;
    int64_t v563; /* placeholder for void */
    void* v310;
    void* v413;
    void* v375;
    double v476;
    int64_t v620; /* placeholder for void */
    int64_t v731; /* placeholder for void */
    int64_t v210; /* placeholder for void */
    double v778;
    int64_t v462; /* placeholder for void */
    void* v107;
    double v222;
    double v323;
    int64_t v7; /* placeholder for void */
    int64_t v156; /* placeholder for void */
    void* v848;
    void* v691;
    int64_t v258; /* placeholder for void */
    double v488;
    double v711;
    void* v164;
    void* v185;
    void* v203;
    int64_t v454; /* placeholder for void */
    void* v751;
    double v440;
    void* v600;
    double v396;
    int64_t v835;
    int64_t v132; /* placeholder for void */
    void* v736;
    void* v618;
    int64_t v497; /* placeholder for void */
    double v135;
    int64_t v743; /* placeholder for void */
    int64_t v716; /* placeholder for void */
    void* v146;
    void* v506;
    void* v606;
    void* v837;
    void* v24;
    void* v853;
    double v768;
    int64_t v163; /* placeholder for void */
    double v50;
    double v255;
    int64_t v702; /* placeholder for void */
    void* v847;
    void* v592;
    void* v212;
    int64_t v214;
    void* v697;
    int64_t v747;
    int64_t v840; /* placeholder for void */
    void* v540;
    double v119;
    void* v347;
    double v888;
    void* v406;
    int64_t v880;
    void* v91;
    int64_t v622; /* placeholder for void */
    void* v25;
    double v608;
    void* v761;
    void* v770;
    double v796;
    double v728;
    int64_t v136; /* placeholder for void */
    void* v574;
    int64_t v273; /* placeholder for void */
    void* v106;
    double v433;
    int64_t v14; /* placeholder for void */
    void* v93;
    void* v758;
    void* v766;
    double v294;
    double v382;
    void* v525;
    int64_t v362;
    void* v473;
    int64_t v713;
    double v195;
    void* v530;
    void* v827;
    int64_t v569; /* placeholder for void */
    int64_t v725; /* placeholder for void */
    void* v830;
    int64_t v513; /* placeholder for void */
    void* v705;
    void* v426;
    void* v324;
    int64_t v325; /* placeholder for void */
    double v427;
    double v153;
    int64_t v312; /* placeholder for void */
    int64_t v188; /* placeholder for void */
    void* v773;
    int64_t v573; /* placeholder for void */
    void* v663;
    void* v521;
    int64_t v111; /* placeholder for void */
    int64_t v279; /* placeholder for void */
    int64_t v466; /* placeholder for void */
    void* v6;
    int64_t v147;
    void* v564;
    int64_t v412; /* placeholder for void */
    void* v253;
    void* v822;
    double v740;
    double v849;
    double v542;
    double v102;
    void* v270;
    int64_t v750; /* placeholder for void */
    double v894;
    void* v199;
    int64_t v759; /* placeholder for void */
    int64_t v428; /* placeholder for void */
    void* v744;
    void* v447;
    void* v151;
    int64_t v451;
    int64_t v223; /* placeholder for void */
    double v295;
    void* v425;
    void* v419;
    double v261;
    void* v213;
    int64_t v591; /* placeholder for void */
    void* v630;
    int64_t v519; /* placeholder for void */
    void* v580;
    void* v787;
    double v234;
    double v536;
    int64_t v130; /* placeholder for void */
    void* v556;
    void* v503;
    void* v839;
    double v180;
    void* v834;
    void* v284;
    void* v275;
    void* v230;
    void* v399;
    void* v496;
    void* v64;
    void* v319;
    int64_t v377; /* placeholder for void */
    int64_t v121;
    int64_t v505; /* placeholder for void */
    void* v526;
    int64_t v593; /* placeholder for void */
    int64_t v720;
    int64_t v61; /* placeholder for void */
    int64_t v737; /* placeholder for void */
    void* v321;
    int64_t v754;
    void* v693;
    double v249;
    double v676;
    void* v22;
    double v665;
    double v114;
    int64_t v256; /* placeholder for void */
    double v516;
    int64_t v636; /* placeholder for void */
    void* v167;
    int64_t v183; /* placeholder for void */
    double v774;
    int64_t v757; /* placeholder for void */
    int64_t v231; /* placeholder for void */
    void* v402;
    void* v583;
    void* v833;
    void* v333;
    double v655;
    int64_t v809; /* placeholder for void */
    int64_t v154; /* placeholder for void */
    double v548;
    int64_t v448; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    void* v359;
    int64_t v204; /* placeholder for void */
    void* v16;
    int64_t v235; /* placeholder for void */
    int64_t v815; /* placeholder for void */
    void* v233;
    double v376;
    int64_t v468; /* placeholder for void */
    int64_t v579; /* placeholder for void */
    int64_t v723; /* placeholder for void */
    void* v79;
    int64_t v788;
    int64_t v831; /* placeholder for void */
    int64_t v846; /* placeholder for void */
    int64_t v67;
    double v266;
    int64_t v150; /* placeholder for void */
    double v81;
    void* v364;
    int64_t v669;
    double v282;
    double v201;
    void* v482;
    int64_t v337; /* placeholder for void */
    int64_t v187;
    void* v627;
    void* v679;
    int64_t v799; /* placeholder for void */
    void* v116;
    double v317;
    void* v307;
    void* v455;
    void* v80;
    void* v221;
    void* v438;
    void* v461;
    int64_t v690; /* placeholder for void */
    void* v808;
    void* v896;
    void* v798;
    void* v89;
    void* v17;
    void* v449;
    int64_t v850; /* placeholder for void */
    void* v689;
    double v39;
    int64_t v531; /* placeholder for void */
    int64_t v237; /* placeholder for void */
    int64_t v609; /* placeholder for void */
    int64_t v673; /* placeholder for void */
    int64_t v246; /* placeholder for void */
    void* v458;
    int64_t v302; /* placeholder for void */
    void* v71;
    void* v518;
    void* v753;
    void* v112;
    int64_t v252; /* placeholder for void */
    int64_t v268;
    int64_t v906; /* placeholder for void */
    int64_t v862;
    void* v420;
    int64_t v617; /* placeholder for void */
    void* v287;
    int64_t v138; /* placeholder for void */
    int64_t v392; /* placeholder for void */
    void* v816;
    int64_t v196; /* placeholder for void */
    void* v437;
    void* v859;
    void* v634;
    void* v316;
    double v522;
    int64_t v683; /* placeholder for void */
    int64_t v190; /* placeholder for void */
    void* v551;
    void* v492;
    void* v172;
    double v863;
    void* v191;
    void* v46;
    void* v703;
    double v477;
    void* v781;
    int64_t v791; /* placeholder for void */
    double v26;
    int64_t v355; /* placeholder for void */
    void* v92;
    void* v361;
    double v552;
    void* v733;
    double v661;
    void* v118;
    void* v120;
    void* v259;
    int64_t v202; /* placeholder for void */
    void* v514;
    double v588;
    void* v865;
    int64_t v277; /* placeholder for void */
    void* v184;
    void* v423;
    double v490;
    void* v538;
    double v108;
    void* v651;
    double v762;
    void* v368;
    void* v739;
    double v784;
    void* v393;
    int64_t v314; /* placeholder for void */
    void* v682;
    int64_t v446; /* placeholder for void */
    double v459;
    void* v34;
    double v887;
    int64_t v511; /* placeholder for void */
    void* v610;
    int64_t v615; /* placeholder for void */
    int64_t v400;
    int64_t v215; /* placeholder for void */
    int64_t v891;
    int64_t v99; /* placeholder for void */
    void* v659;
    int64_t v687;
    int64_t v349; /* placeholder for void */
    double v415;
    void* v155;
    double v32;
    v1 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v2 = ((union { uint64_t u; double f; }){ .u = 4611686018427387904ULL }).f;
    v3 = malloc(16);
    v4 = 0LL;
    vec2(v3, (void*)v4, v1, v2);
    v6 = malloc(8);
    *(void**)v6 = (void*)v3;
    v8 = ((union { uint64_t u; double f; }){ .u = 4613937818241073152ULL }).f;
    v9 = ((union { uint64_t u; double f; }){ .u = 4616189618054758400ULL }).f;
    v10 = malloc(16);
    v11 = 0LL;
    vec2(v10, (void*)v11, v8, v9);
    v13 = malloc(8);
    *(void**)v13 = (void*)v10;
    v15 = *(void**)v6;
    v16 = *(void**)v13;
    v17 = malloc(16);
    v18 = 0LL;
    vec2_add(v17, (void*)v18, v15, v16);
    v20 = malloc(8);
    *(void**)v20 = (void*)v17;
    v22 = _str14;
    p_print_str(v22);
    v24 = *(void**)v20;
    v25 = (char*)v24 + 0;
    v26 = *(double*)v25;
    p_print_float(v26);
    v28 = _str4;
    p_print_str(v28);
    v30 = *(void**)v20;
    v31 = (char*)v30 + 8;
    v32 = *(double*)v31;
    p_print_float(v32);
    v34 = _str9;
    p_print_str(v34);
    v36 = *(void**)v6;
    v37 = *(void**)v13;
    v38 = 0LL;
    v39 = (double)vec2_dot((void*)v38, v36, v37);
    v40 = malloc(8);
    *(double*)v40 = v39;
    v42 = _str15;
    p_print_str(v42);
    v44 = *(double*)v40;
    p_print_float(v44);
    v46 = _str9;
    p_print_str(v46);
    v48 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v49 = ((union { uint64_t u; double f; }){ .u = 4611686018427387904ULL }).f;
    v50 = ((union { uint64_t u; double f; }){ .u = 4613937818241073152ULL }).f;
    v51 = malloc(24);
    v52 = 0LL;
    vec3(v51, (void*)v52, v48, v49, v50);
    v54 = malloc(8);
    *(void**)v54 = (void*)v51;
    v56 = ((union { uint64_t u; double f; }){ .u = 4616189618054758400ULL }).f;
    v57 = ((union { uint64_t u; double f; }){ .u = 4617315517961601024ULL }).f;
    v58 = ((union { uint64_t u; double f; }){ .u = 4618441417868443648ULL }).f;
    v59 = malloc(24);
    v60 = 0LL;
    vec3(v59, (void*)v60, v56, v57, v58);
    v62 = malloc(8);
    *(void**)v62 = (void*)v59;
    v64 = *(void**)v54;
    v65 = *(void**)v62;
    v66 = malloc(24);
    v67 = 0LL;
    vec3_add(v66, (void*)v67, v64, v65);
    v69 = malloc(8);
    *(void**)v69 = (void*)v66;
    v71 = _str16;
    p_print_str(v71);
    v73 = *(void**)v69;
    v74 = (char*)v73 + 0;
    v75 = *(double*)v74;
    p_print_float(v75);
    v77 = _str4;
    p_print_str(v77);
    v79 = *(void**)v69;
    v80 = (char*)v79 + 8;
    v81 = *(double*)v80;
    p_print_float(v81);
    v83 = _str4;
    p_print_str(v83);
    v85 = *(void**)v69;
    v86 = (char*)v85 + 16;
    v87 = *(double*)v86;
    p_print_float(v87);
    v89 = _str9;
    p_print_str(v89);
    v91 = *(void**)v54;
    v92 = *(void**)v62;
    v93 = malloc(24);
    v94 = 0LL;
    vec3_sub(v93, (void*)v94, v91, v92);
    v96 = malloc(8);
    *(void**)v96 = (void*)v93;
    v98 = _str17;
    p_print_str(v98);
    v100 = *(void**)v96;
    v101 = (char*)v100 + 0;
    v102 = *(double*)v101;
    p_print_float(v102);
    v104 = _str4;
    p_print_str(v104);
    v106 = *(void**)v96;
    v107 = (char*)v106 + 8;
    v108 = *(double*)v107;
    p_print_float(v108);
    v110 = _str4;
    p_print_str(v110);
    v112 = *(void**)v96;
    v113 = (char*)v112 + 16;
    v114 = *(double*)v113;
    p_print_float(v114);
    v116 = _str9;
    p_print_str(v116);
    v118 = *(void**)v54;
    v119 = ((union { uint64_t u; double f; }){ .u = 4611686018427387904ULL }).f;
    v120 = malloc(24);
    v121 = 0LL;
    vec3_scale(v120, (void*)v121, v118, v119);
    v123 = malloc(8);
    *(void**)v123 = (void*)v120;
    v125 = _str18;
    p_print_str(v125);
    v127 = *(void**)v123;
    v128 = (char*)v127 + 0;
    v129 = *(double*)v128;
    p_print_float(v129);
    v131 = _str4;
    p_print_str(v131);
    v133 = *(void**)v123;
    v134 = (char*)v133 + 8;
    v135 = *(double*)v134;
    p_print_float(v135);
    v137 = _str4;
    p_print_str(v137);
    v139 = *(void**)v123;
    v140 = (char*)v139 + 16;
    v141 = *(double*)v140;
    p_print_float(v141);
    v143 = _str9;
    p_print_str(v143);
    v145 = *(void**)v54;
    v146 = *(void**)v62;
    v147 = 0LL;
    v148 = (double)vec3_dot((void*)v147, v145, v146);
    v149 = malloc(8);
    *(double*)v149 = v148;
    v151 = _str19;
    p_print_str(v151);
    v153 = *(double*)v149;
    p_print_float(v153);
    v155 = _str9;
    p_print_str(v155);
    v157 = *(void**)v54;
    v158 = *(void**)v62;
    v159 = malloc(24);
    v160 = 0LL;
    vec3_cross(v159, (void*)v160, v157, v158);
    v162 = malloc(8);
    *(void**)v162 = (void*)v159;
    v164 = _str20;
    p_print_str(v164);
    v166 = *(void**)v162;
    v167 = (char*)v166 + 0;
    v168 = *(double*)v167;
    p_print_float(v168);
    v170 = _str4;
    p_print_str(v170);
    v172 = *(void**)v162;
    v173 = (char*)v172 + 8;
    v174 = *(double*)v173;
    p_print_float(v174);
    v176 = _str4;
    p_print_str(v176);
    v178 = *(void**)v162;
    v179 = (char*)v178 + 16;
    v180 = *(double*)v179;
    p_print_float(v180);
    v182 = _str9;
    p_print_str(v182);
    v184 = *(void**)v54;
    v185 = *(void**)v62;
    v186 = malloc(24);
    v187 = 0LL;
    vec3_add(v186, (void*)v187, v184, v185);
    v189 = malloc(8);
    *(void**)v189 = (void*)v186;
    v191 = _str21;
    p_print_str(v191);
    v193 = *(void**)v189;
    v194 = (char*)v193 + 0;
    v195 = *(double*)v194;
    p_print_float(v195);
    v197 = _str4;
    p_print_str(v197);
    v199 = *(void**)v189;
    v200 = (char*)v199 + 8;
    v201 = *(double*)v200;
    p_print_float(v201);
    v203 = _str4;
    p_print_str(v203);
    v205 = *(void**)v189;
    v206 = (char*)v205 + 16;
    v207 = *(double*)v206;
    p_print_float(v207);
    v209 = _str9;
    p_print_str(v209);
    v211 = *(void**)v54;
    v212 = *(void**)v62;
    v213 = malloc(24);
    v214 = 0LL;
    vec3_sub(v213, (void*)v214, v211, v212);
    v216 = malloc(8);
    *(void**)v216 = (void*)v213;
    v218 = _str22;
    p_print_str(v218);
    v220 = *(void**)v216;
    v221 = (char*)v220 + 0;
    v222 = *(double*)v221;
    p_print_float(v222);
    v224 = _str4;
    p_print_str(v224);
    v226 = *(void**)v216;
    v227 = (char*)v226 + 8;
    v228 = *(double*)v227;
    p_print_float(v228);
    v230 = _str4;
    p_print_str(v230);
    v232 = *(void**)v216;
    v233 = (char*)v232 + 16;
    v234 = *(double*)v233;
    p_print_float(v234);
    v236 = _str9;
    p_print_str(v236);
    v238 = *(void**)v54;
    v239 = ((union { uint64_t u; double f; }){ .u = 4611686018427387904ULL }).f;
    v240 = malloc(24);
    v241 = 0LL;
    vec3_scale(v240, (void*)v241, v238, v239);
    v243 = malloc(8);
    *(void**)v243 = (void*)v240;
    v245 = _str23;
    p_print_str(v245);
    v247 = *(void**)v243;
    v248 = (char*)v247 + 0;
    v249 = *(double*)v248;
    p_print_float(v249);
    v251 = _str4;
    p_print_str(v251);
    v253 = *(void**)v243;
    v254 = (char*)v253 + 8;
    v255 = *(double*)v254;
    p_print_float(v255);
    v257 = _str4;
    p_print_str(v257);
    v259 = *(void**)v243;
    v260 = (char*)v259 + 16;
    v261 = *(double*)v260;
    p_print_float(v261);
    v263 = _str9;
    p_print_str(v263);
    v265 = *(void**)v54;
    v266 = ((union { uint64_t u; double f; }){ .u = 4613937818241073152ULL }).f;
    v267 = malloc(24);
    v268 = 0LL;
    vec3_scale(v267, (void*)v268, v265, v266);
    v270 = malloc(8);
    *(void**)v270 = (void*)v267;
    v272 = _str24;
    p_print_str(v272);
    v274 = *(void**)v270;
    v275 = (char*)v274 + 0;
    v276 = *(double*)v275;
    p_print_float(v276);
    v278 = _str4;
    p_print_str(v278);
    v280 = *(void**)v270;
    v281 = (char*)v280 + 8;
    v282 = *(double*)v281;
    p_print_float(v282);
    v284 = _str4;
    p_print_str(v284);
    v286 = *(void**)v270;
    v287 = (char*)v286 + 16;
    v288 = *(double*)v287;
    p_print_float(v288);
    v290 = _str9;
    p_print_str(v290);
    v292 = *(void**)v54;
    v293 = ((union { uint64_t u; double f; }){ .u = 4611686018427387904ULL }).f;
    v294 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v295 = v294 / v293;
    v296 = malloc(24);
    v297 = 0LL;
    vec3_scale(v296, (void*)v297, v292, v295);
    v299 = malloc(8);
    *(void**)v299 = (void*)v296;
    v301 = _str25;
    p_print_str(v301);
    v303 = *(void**)v299;
    v304 = (char*)v303 + 0;
    v305 = *(double*)v304;
    p_print_float(v305);
    v307 = _str4;
    p_print_str(v307);
    v309 = *(void**)v299;
    v310 = (char*)v309 + 8;
    v311 = *(double*)v310;
    p_print_float(v311);
    v313 = _str4;
    p_print_str(v313);
    v315 = *(void**)v299;
    v316 = (char*)v315 + 16;
    v317 = *(double*)v316;
    p_print_float(v317);
    v319 = _str9;
    p_print_str(v319);
    v321 = *(void**)v54;
    v322 = 0LL;
    v323 = (double)vec3_length((void*)v322, v321);
    v324 = malloc(8);
    *(double*)v324 = v323;
    v326 = _str26;
    p_print_str(v326);
    v328 = *(double*)v324;
    p_print_float(v328);
    v330 = _str9;
    p_print_str(v330);
    v332 = *(void**)v54;
    v333 = malloc(24);
    v334 = 0LL;
    vec3_normalize(v333, (void*)v334, v332);
    v336 = malloc(8);
    *(void**)v336 = (void*)v333;
    v338 = _str27;
    p_print_str(v338);
    v340 = *(void**)v336;
    v341 = (char*)v340 + 0;
    v342 = *(double*)v341;
    p_print_float(v342);
    v344 = _str4;
    p_print_str(v344);
    v346 = *(void**)v336;
    v347 = (char*)v346 + 8;
    v348 = *(double*)v347;
    p_print_float(v348);
    v350 = _str4;
    p_print_str(v350);
    v352 = *(void**)v336;
    v353 = (char*)v352 + 16;
    v354 = *(double*)v353;
    p_print_float(v354);
    v356 = _str9;
    p_print_str(v356);
    v358 = *(void**)v54;
    v359 = *(void**)v62;
    v360 = ((union { uint64_t u; double f; }){ .u = 4602678819172646912ULL }).f;
    v361 = malloc(24);
    v362 = 0LL;
    vec3_lerp(v361, (void*)v362, v358, v359, v360);
    v364 = malloc(8);
    *(void**)v364 = (void*)v361;
    v366 = _str28;
    p_print_str(v366);
    v368 = *(void**)v364;
    v369 = (char*)v368 + 0;
    v370 = *(double*)v369;
    p_print_float(v370);
    v372 = _str4;
    p_print_str(v372);
    v374 = *(void**)v364;
    v375 = (char*)v374 + 8;
    v376 = *(double*)v375;
    p_print_float(v376);
    v378 = _str4;
    p_print_str(v378);
    v380 = *(void**)v364;
    v381 = (char*)v380 + 16;
    v382 = *(double*)v381;
    p_print_float(v382);
    v384 = _str9;
    p_print_str(v384);
    v386 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v387 = ((union { uint64_t u; double f; }){ .u = 4611686018427387904ULL }).f;
    v388 = ((union { uint64_t u; double f; }){ .u = 4613937818241073152ULL }).f;
    v389 = ((union { uint64_t u; double f; }){ .u = 4616189618054758400ULL }).f;
    v390 = malloc(32);
    v391 = 0LL;
    vec4(v390, (void*)v391, v386, v387, v388, v389);
    v393 = malloc(8);
    *(void**)v393 = (void*)v390;
    v395 = ((union { uint64_t u; double f; }){ .u = 4617315517961601024ULL }).f;
    v396 = ((union { uint64_t u; double f; }){ .u = 4618441417868443648ULL }).f;
    v397 = ((union { uint64_t u; double f; }){ .u = 4619567317775286272ULL }).f;
    v398 = ((union { uint64_t u; double f; }){ .u = 4620693217682128896ULL }).f;
    v399 = malloc(32);
    v400 = 0LL;
    vec4(v399, (void*)v400, v395, v396, v397, v398);
    v402 = malloc(8);
    *(void**)v402 = (void*)v399;
    v404 = *(void**)v393;
    v405 = *(void**)v402;
    v406 = malloc(32);
    v407 = 0LL;
    vec4_add(v406, (void*)v407, v404, v405);
    v409 = malloc(8);
    *(void**)v409 = (void*)v406;
    v411 = _str29;
    p_print_str(v411);
    v413 = *(void**)v409;
    v414 = (char*)v413 + 0;
    v415 = *(double*)v414;
    p_print_float(v415);
    v417 = _str4;
    p_print_str(v417);
    v419 = *(void**)v409;
    v420 = (char*)v419 + 8;
    v421 = *(double*)v420;
    p_print_float(v421);
    v423 = _str4;
    p_print_str(v423);
    v425 = *(void**)v409;
    v426 = (char*)v425 + 16;
    v427 = *(double*)v426;
    p_print_float(v427);
    v429 = _str4;
    p_print_str(v429);
    v431 = *(void**)v409;
    v432 = (char*)v431 + 24;
    v433 = *(double*)v432;
    p_print_float(v433);
    v435 = _str9;
    p_print_str(v435);
    v437 = *(void**)v393;
    v438 = *(void**)v402;
    v439 = 0LL;
    v440 = (double)vec4_dot((void*)v439, v437, v438);
    v441 = malloc(8);
    *(double*)v441 = v440;
    v443 = _str30;
    p_print_str(v443);
    v445 = *(double*)v441;
    p_print_float(v445);
    v447 = _str9;
    p_print_str(v447);
    v449 = *(void**)v393;
    v450 = malloc(24);
    v451 = 0LL;
    vec4_xyz(v450, (void*)v451, v449);
    v453 = malloc(8);
    *(void**)v453 = (void*)v450;
    v455 = _str31;
    p_print_str(v455);
    v457 = *(void**)v453;
    v458 = (char*)v457 + 0;
    v459 = *(double*)v458;
    p_print_float(v459);
    v461 = _str4;
    p_print_str(v461);
    v463 = *(void**)v453;
    v464 = (char*)v463 + 8;
    v465 = *(double*)v464;
    p_print_float(v465);
    v467 = _str4;
    p_print_str(v467);
    v469 = *(void**)v453;
    v470 = (char*)v469 + 16;
    v471 = *(double*)v470;
    p_print_float(v471);
    v473 = _str9;
    p_print_str(v473);
    v475 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v476 = ((union { uint64_t u; double f; }){ .u = 4602678819172646912ULL }).f;
    v477 = ((union { uint64_t u; double f; }){ .u = 4596373779694328218ULL }).f;
    v478 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v479 = malloc(32);
    v480 = 0LL;
    vec4(v479, (void*)v480, v475, v476, v477, v478);
    v482 = malloc(8);
    *(void**)v482 = (void*)v479;
    v484 = *(void**)v482;
    v485 = (char*)v484 + 0;
    v486 = *(double*)v485;
    v487 = (char*)v484 + 8;
    v488 = *(double*)v487;
    v489 = (char*)v484 + 16;
    v490 = *(double*)v489;
    v491 = malloc(24);
    v492 = (char*)v491 + 0;
    *(double*)v492 = v486;
    v494 = (char*)v491 + 8;
    *(double*)v494 = v488;
    v496 = (char*)v491 + 16;
    *(double*)v496 = v490;
    v498 = malloc(8);
    *(void**)v498 = (void*)v491;
    v500 = _str32;
    p_print_str(v500);
    v502 = *(void**)v498;
    v503 = (char*)v502 + 0;
    v504 = *(double*)v503;
    p_print_float(v504);
    v506 = _str4;
    p_print_str(v506);
    v508 = *(void**)v498;
    v509 = (char*)v508 + 8;
    v510 = *(double*)v509;
    p_print_float(v510);
    v512 = _str4;
    p_print_str(v512);
    v514 = *(void**)v498;
    v515 = (char*)v514 + 16;
    v516 = *(double*)v515;
    p_print_float(v516);
    v518 = _str9;
    p_print_str(v518);
    v520 = *(void**)v482;
    v521 = (char*)v520 + 0;
    v522 = *(double*)v521;
    v523 = (char*)v520 + 8;
    v524 = *(double*)v523;
    v525 = malloc(16);
    v526 = (char*)v525 + 0;
    *(double*)v526 = v522;
    v528 = (char*)v525 + 8;
    *(double*)v528 = v524;
    v530 = malloc(8);
    *(void**)v530 = (void*)v525;
    v532 = _str33;
    p_print_str(v532);
    v534 = *(void**)v530;
    v535 = (char*)v534 + 0;
    v536 = *(double*)v535;
    p_print_float(v536);
    v538 = _str4;
    p_print_str(v538);
    v540 = *(void**)v530;
    v541 = (char*)v540 + 8;
    v542 = *(double*)v541;
    p_print_float(v542);
    v544 = _str9;
    p_print_str(v544);
    v546 = *(void**)v482;
    v547 = (char*)v546 + 8;
    v548 = *(double*)v547;
    v549 = (char*)v546 + 16;
    v550 = *(double*)v549;
    v551 = (char*)v546 + 0;
    v552 = *(double*)v551;
    v553 = malloc(24);
    v554 = (char*)v553 + 0;
    *(double*)v554 = v548;
    v556 = (char*)v553 + 8;
    *(double*)v556 = v550;
    v558 = (char*)v553 + 16;
    *(double*)v558 = v552;
    v560 = malloc(8);
    *(void**)v560 = (void*)v553;
    v562 = _str34;
    p_print_str(v562);
    v564 = *(void**)v560;
    v565 = (char*)v564 + 0;
    v566 = *(double*)v565;
    p_print_float(v566);
    v568 = _str4;
    p_print_str(v568);
    v570 = *(void**)v560;
    v571 = (char*)v570 + 8;
    v572 = *(double*)v571;
    p_print_float(v572);
    v574 = _str4;
    p_print_str(v574);
    v576 = *(void**)v560;
    v577 = (char*)v576 + 16;
    v578 = *(double*)v577;
    p_print_float(v578);
    v580 = _str9;
    p_print_str(v580);
    v582 = *(void**)v482;
    v583 = (char*)v582 + 0;
    v584 = *(double*)v583;
    v585 = (char*)v582 + 0;
    v586 = *(double*)v585;
    v587 = (char*)v582 + 0;
    v588 = *(double*)v587;
    v589 = malloc(24);
    v590 = (char*)v589 + 0;
    *(double*)v590 = v584;
    v592 = (char*)v589 + 8;
    *(double*)v592 = v586;
    v594 = (char*)v589 + 16;
    *(double*)v594 = v588;
    v596 = malloc(8);
    *(void**)v596 = (void*)v589;
    v598 = _str35;
    p_print_str(v598);
    v600 = *(void**)v596;
    v601 = (char*)v600 + 0;
    v602 = *(double*)v601;
    p_print_float(v602);
    v604 = _str4;
    p_print_str(v604);
    v606 = *(void**)v596;
    v607 = (char*)v606 + 8;
    v608 = *(double*)v607;
    p_print_float(v608);
    v610 = _str4;
    p_print_str(v610);
    v612 = *(void**)v596;
    v613 = (char*)v612 + 16;
    v614 = *(double*)v613;
    p_print_float(v614);
    v616 = _str9;
    p_print_str(v616);
    v618 = malloc(128);
    v619 = 0LL;
    mat4_identity(v618, (void*)v619);
    v621 = malloc(8);
    *(void**)v621 = (void*)v618;
    v623 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v624 = ((union { uint64_t u; double f; }){ .u = 4611686018427387904ULL }).f;
    v625 = ((union { uint64_t u; double f; }){ .u = 4613937818241073152ULL }).f;
    v626 = ((union { uint64_t u; double f; }){ .u = 4616189618054758400ULL }).f;
    v627 = malloc(32);
    v628 = 0LL;
    vec4(v627, (void*)v628, v623, v624, v625, v626);
    v630 = malloc(8);
    *(void**)v630 = (void*)v627;
    v632 = *(void**)v621;
    v633 = *(void**)v630;
    v634 = malloc(32);
    v635 = 0LL;
    mat4_transform_vec4(v634, (void*)v635, v632, v633);
    v637 = malloc(8);
    *(void**)v637 = (void*)v634;
    v639 = _str36;
    p_print_str(v639);
    v641 = *(void**)v637;
    v642 = (char*)v641 + 0;
    v643 = *(double*)v642;
    p_print_float(v643);
    v645 = _str4;
    p_print_str(v645);
    v647 = *(void**)v637;
    v648 = (char*)v647 + 8;
    v649 = *(double*)v648;
    p_print_float(v649);
    v651 = _str4;
    p_print_str(v651);
    v653 = *(void**)v637;
    v654 = (char*)v653 + 16;
    v655 = *(double*)v654;
    p_print_float(v655);
    v657 = _str4;
    p_print_str(v657);
    v659 = *(void**)v637;
    v660 = (char*)v659 + 24;
    v661 = *(double*)v660;
    p_print_float(v661);
    v663 = _str9;
    p_print_str(v663);
    v665 = ((union { uint64_t u; double f; }){ .u = 4621819117588971520ULL }).f;
    v666 = ((union { uint64_t u; double f; }){ .u = 4626322717216342016ULL }).f;
    v667 = ((union { uint64_t u; double f; }){ .u = 4629137466983448576ULL }).f;
    v668 = malloc(24);
    v669 = 0LL;
    vec3(v668, (void*)v669, v665, v666, v667);
    v671 = malloc(128);
    v672 = 0LL;
    translate(v671, (void*)v672, v668);
    v674 = malloc(8);
    *(void**)v674 = (void*)v671;
    v676 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v677 = ((union { uint64_t u; double f; }){ .u = 4611686018427387904ULL }).f;
    v678 = ((union { uint64_t u; double f; }){ .u = 4613937818241073152ULL }).f;
    v679 = malloc(24);
    v680 = 0LL;
    vec3(v679, (void*)v680, v676, v677, v678);
    v682 = malloc(8);
    *(void**)v682 = (void*)v679;
    v684 = *(void**)v674;
    v685 = *(void**)v682;
    v686 = malloc(24);
    v687 = 0LL;
    mat4_transform_vec3(v686, (void*)v687, v684, v685);
    v689 = malloc(8);
    *(void**)v689 = (void*)v686;
    v691 = _str37;
    p_print_str(v691);
    v693 = *(void**)v689;
    v694 = (char*)v693 + 0;
    v695 = *(double*)v694;
    p_print_float(v695);
    v697 = _str4;
    p_print_str(v697);
    v699 = *(void**)v689;
    v700 = (char*)v699 + 8;
    v701 = *(double*)v700;
    p_print_float(v701);
    v703 = _str4;
    p_print_str(v703);
    v705 = *(void**)v689;
    v706 = (char*)v705 + 16;
    v707 = *(double*)v706;
    p_print_float(v707);
    v709 = _str9;
    p_print_str(v709);
    v711 = ((union { uint64_t u; double f; }){ .u = 4611686018427387904ULL }).f;
    v712 = malloc(128);
    v713 = 0LL;
    mat_scale(v712, (void*)v713, v711);
    v715 = malloc(8);
    *(void**)v715 = (void*)v712;
    v717 = *(void**)v715;
    v718 = *(void**)v682;
    v719 = malloc(24);
    v720 = 0LL;
    mat4_transform_vec3(v719, (void*)v720, v717, v718);
    v722 = malloc(8);
    *(void**)v722 = (void*)v719;
    v724 = _str38;
    p_print_str(v724);
    v726 = *(void**)v722;
    v727 = (char*)v726 + 0;
    v728 = *(double*)v727;
    p_print_float(v728);
    v730 = _str4;
    p_print_str(v730);
    v732 = *(void**)v722;
    v733 = (char*)v732 + 8;
    v734 = *(double*)v733;
    p_print_float(v734);
    v736 = _str4;
    p_print_str(v736);
    v738 = *(void**)v722;
    v739 = (char*)v738 + 16;
    v740 = *(double*)v739;
    p_print_float(v740);
    v742 = _str9;
    p_print_str(v742);
    v744 = *(void**)v674;
    v745 = *(void**)v715;
    v746 = malloc(128);
    v747 = 0LL;
    mat4_mul(v746, (void*)v747, v744, v745);
    v749 = malloc(8);
    *(void**)v749 = (void*)v746;
    v751 = *(void**)v749;
    v752 = *(void**)v682;
    v753 = malloc(24);
    v754 = 0LL;
    mat4_transform_vec3(v753, (void*)v754, v751, v752);
    v756 = malloc(8);
    *(void**)v756 = (void*)v753;
    v758 = _str39;
    p_print_str(v758);
    v760 = *(void**)v756;
    v761 = (char*)v760 + 0;
    v762 = *(double*)v761;
    p_print_float(v762);
    v764 = _str4;
    p_print_str(v764);
    v766 = *(void**)v756;
    v767 = (char*)v766 + 8;
    v768 = *(double*)v767;
    p_print_float(v768);
    v770 = _str4;
    p_print_str(v770);
    v772 = *(void**)v756;
    v773 = (char*)v772 + 16;
    v774 = *(double*)v773;
    p_print_float(v774);
    v776 = _str9;
    p_print_str(v776);
    v778 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v779 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v780 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v781 = malloc(24);
    v782 = 0LL;
    vec3(v781, (void*)v782, v778, v779, v780);
    v784 = ((union { uint64_t u; double f; }){ .u = 4636033603912859648ULL }).f;
    v785 = 0LL;
    v786 = (double)radians((void*)v785, v784);
    v787 = malloc(32);
    v788 = 0LL;
    quat_from_axis_angle(v787, (void*)v788, v781, v786);
    v790 = malloc(8);
    *(void**)v790 = (void*)v787;
    v792 = _str40;
    p_print_str(v792);
    v794 = *(void**)v790;
    v795 = (char*)v794 + 0;
    v796 = *(double*)v795;
    p_print_float(v796);
    v798 = _str9;
    p_print_str(v798);
    v800 = _str41;
    p_print_str(v800);
    v802 = *(void**)v790;
    v803 = (char*)v802 + 8;
    v804 = *(double*)v803;
    p_print_float(v804);
    v806 = _str9;
    p_print_str(v806);
    v808 = _str42;
    p_print_str(v808);
    v810 = *(void**)v790;
    v811 = (char*)v810 + 16;
    v812 = *(double*)v811;
    p_print_float(v812);
    v814 = _str9;
    p_print_str(v814);
    v816 = _str43;
    p_print_str(v816);
    v818 = *(void**)v790;
    v819 = (char*)v818 + 24;
    v820 = *(double*)v819;
    p_print_float(v820);
    v822 = _str9;
    p_print_str(v822);
    v824 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v825 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v826 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v827 = malloc(24);
    v828 = 0LL;
    vec3(v827, (void*)v828, v824, v825, v826);
    v830 = malloc(8);
    *(void**)v830 = (void*)v827;
    v832 = *(void**)v790;
    v833 = *(void**)v830;
    v834 = malloc(24);
    v835 = 0LL;
    quat_rotate_vec3(v834, (void*)v835, v832, v833);
    v837 = malloc(8);
    *(void**)v837 = (void*)v834;
    v839 = _str44;
    p_print_str(v839);
    v841 = *(void**)v837;
    v842 = (char*)v841 + 0;
    v843 = *(double*)v842;
    p_print_float(v843);
    v845 = _str4;
    p_print_str(v845);
    v847 = *(void**)v837;
    v848 = (char*)v847 + 8;
    v849 = *(double*)v848;
    p_print_float(v849);
    v851 = _str4;
    p_print_str(v851);
    v853 = *(void**)v837;
    v854 = (char*)v853 + 16;
    v855 = *(double*)v854;
    p_print_float(v855);
    v857 = _str9;
    p_print_str(v857);
    v859 = _str45;
    p_print_str(v859);
    v861 = ((union { uint64_t u; double f; }){ .u = 4636033603912859648ULL }).f;
    v862 = 0LL;
    v863 = (double)radians((void*)v862, v861);
    p_print_float(v863);
    v865 = _str9;
    p_print_str(v865);
    v867 = _str46;
    p_print_str(v867);
    v869 = ((union { uint64_t u; double f; }){ .u = 4614256650576692846ULL }).f;
    v870 = 0LL;
    v871 = (double)degrees((void*)v870, v869);
    p_print_float(v871);
    v873 = _str9;
    p_print_str(v873);
    v875 = _str47;
    p_print_str(v875);
    v877 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v878 = ((union { uint64_t u; double f; }){ .u = 4621819117588971520ULL }).f;
    v879 = ((union { uint64_t u; double f; }){ .u = 4602678819172646912ULL }).f;
    v880 = 0LL;
    v881 = (double)lerp((void*)v880, v877, v878, v879);
    p_print_float(v881);
    v883 = _str9;
    p_print_str(v883);
    v885 = _str48;
    p_print_str(v885);
    v887 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v888 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v889 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v890 = malloc(24);
    v891 = 0LL;
    vec3(v890, (void*)v891, v887, v888, v889);
    v893 = ((union { uint64_t u; double f; }){ .u = 4613937818241073152ULL }).f;
    v894 = ((union { uint64_t u; double f; }){ .u = 4616189618054758400ULL }).f;
    v895 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v896 = malloc(24);
    v897 = 0LL;
    vec3(v896, (void*)v897, v893, v894, v895);
    v899 = 0LL;
    v900 = (double)distance((void*)v899, v890, v896);
    p_print_float(v900);
    v902 = _str9;
    p_print_str(v902);
    v904 = _str49;
    p_print_str(v904);
    p_call_stack_pop();
    return 0;
    return 0;
}

double math_cos(void* v0, double v1) {
    p_call_stack_push("math_cos");
    int64_t v3; /* placeholder for void */
    double v2;
    v2 = (double)pledge_cos(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

bool String_is_empty(void* v0, int64_t v1) {
    p_call_stack_push("String_is_empty");
    int64_t v7; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v10;
    int64_t v6; /* placeholder for void */
    int64_t v18;
    bool v5;
    int64_t v13; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    uint8_t v15;
    int64_t v21; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    void* v14;
    bool v3;
    void* v8;
    int64_t v2;
    int64_t v19;
    int64_t v17; /* placeholder for void */
    bool v20;
    void* v11;
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

bool p_str_equal(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("p_str_equal");
    int64_t v14;
    int64_t v7; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    void* v20;
    int64_t v49;
    uint8_t v71;
    int64_t v21; /* placeholder for void */
    bool v50;
    bool v5;
    bool v42;
    int64_t v4; /* placeholder for void */
    int64_t v8;
    int64_t v16; /* placeholder for void */
    void* v35;
    uint8_t v36;
    void* v37;
    bool v46;
    int64_t v41;
    int64_t v29; /* placeholder for void */
    bool v9;
    bool v11;
    int64_t v19; /* placeholder for void */
    bool v45;
    int64_t v24;
    int64_t v18; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v59;
    void* v31;
    uint8_t v68;
    int64_t v27;
    int64_t v56;
    bool v3;
    void* v28;
    int64_t v72; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    bool v76;
    int64_t v53; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v58; /* placeholder for void */
    uint8_t v32;
    bool v52;
    int64_t v30; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    bool v15;
    int64_t v57;
    int64_t v63;
    int64_t v55;
    int64_t v47; /* placeholder for void */
    int64_t v65;
    int64_t v10; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v44;
    int64_t v62; /* placeholder for void */
    void* v67;
    int64_t v54; /* placeholder for void */
    int64_t v60;
    int64_t v48;
    int64_t v73; /* placeholder for void */
    void* v25;
    int64_t v61;
    int64_t v74;
    int64_t v6; /* placeholder for void */
    void* v70;
    int64_t v40;
    int64_t v75;
    int64_t v43;
    int64_t v66; /* placeholder for void */
    void* v22;
    int64_t v23; /* placeholder for void */
    bool v17;
    int64_t v12; /* placeholder for void */
    void* v33;
    int64_t v39; /* placeholder for void */
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

int64_t String_parse_int(void* v0, int64_t v1) {
    p_call_stack_push("String_parse_int");
    int64_t v52;
    int64_t v60;
    bool v3;
    int64_t v84; /* placeholder for void */
    int64_t v85;
    bool v27;
    int64_t v92; /* placeholder for void */
    int64_t v2;
    int64_t v89;
    int64_t v96;
    bool v91;
    int64_t v28;
    int64_t v93;
    int64_t v75; /* placeholder for void */
    void* v78;
    int64_t v18; /* placeholder for void */
    int64_t v99;
    int64_t v113;
    int64_t v111; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    bool v39;
    int64_t v58;
    int64_t v33;
    int64_t v76;
    void* v9;
    int64_t v109; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v73;
    int64_t v115; /* placeholder for void */
    int64_t v36;
    int64_t v86;
    int64_t v61; /* placeholder for void */
    int64_t v74;
    int64_t v50; /* placeholder for void */
    void* v17;
    int64_t v41;
    int64_t v4; /* placeholder for void */
    bool v53;
    int64_t v62;
    int64_t v42;
    int64_t v57; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    int64_t v29;
    int64_t v69;
    uint8_t v79;
    int64_t v80; /* placeholder for void */
    int64_t v110; /* placeholder for void */
    int64_t v94;
    int64_t v54; /* placeholder for void */
    uint8_t v21;
    void* v14;
    int64_t v101;
    int64_t v5;
    int64_t v55;
    int64_t v56;
    void* v64;
    int64_t v67; /* placeholder for void */
    bool v90;
    int64_t v72;
    void* v107;
    int64_t v26;
    int64_t v49; /* placeholder for void */
    void* v47;
    int64_t v95;
    int64_t v43;
    uint8_t v48;
    int64_t v88;
    int64_t v77; /* placeholder for void */
    bool v87;
    int64_t v81; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    int64_t v103;
    int64_t v8;
    int64_t v7; /* placeholder for void */
    bool v70;
    int64_t v46; /* placeholder for void */
    int64_t v59;
    int64_t v83; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    bool v38;
    int64_t v6; /* placeholder for void */
    void* v12;
    int64_t v15; /* placeholder for void */
    int64_t v37;
    int64_t v51;
    int64_t v100; /* placeholder for void */
    int64_t v97;
    void* v22;
    int64_t v40; /* placeholder for void */
    uint8_t v108;
    int64_t v45;
    void* v20;
    bool v31;
    int64_t v10; /* placeholder for void */
    bool v30;
    int64_t v32;
    int64_t v66; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    uint8_t v65;
    int64_t v105;
    bool v34;
    int64_t v25;
    int64_t v68;
    bool v35;
    int64_t v98;
    int64_t v82; /* placeholder for void */
    int64_t v112;
    int64_t v11;
    int64_t v102;
    int64_t v114;
    int64_t v13; /* placeholder for void */
    int64_t v16;
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

int64_t str_len(void* v0, void* v1) {
    p_call_stack_push("str_len");
    int64_t v2;
    int64_t v3;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_len((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

bool str_contains(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_contains");
    int64_t v3;
    bool v4;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (bool)String_contains((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

void* str_trim(void* v0, void* v1) {
    p_call_stack_push("str_trim");
    int64_t v4; /* placeholder for void */
    int64_t v2;
    int64_t v3;
    v2 = 0LL;
    v3 = (int64_t)String_trim((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return (void*)v3;
    return 0;
}

void vec2(void* v0, void* v1, double v2, double v3) {
    p_call_stack_push("vec2");
    double v11;
    int64_t v8; /* placeholder for void */
    double v15;
    void* v13;
    int64_t v16; /* placeholder for void */
    void* v14;
    void* v4;
    int64_t v17; /* placeholder for void */
    void* v5;
    void* v10;
    int64_t v12; /* placeholder for void */
    void* v7;
    void* v9;
    int64_t v6; /* placeholder for void */
    v4 = malloc(16);
    v5 = (char*)v4 + 0;
    *(double*)v5 = v2;
    v7 = (char*)v4 + 8;
    *(double*)v7 = v3;
    v9 = (char*)v4 + 0;
    v10 = (char*)v0 + 0;
    v11 = *(double*)v9;
    *(double*)v10 = v11;
    v13 = (char*)v4 + 8;
    v14 = (char*)v0 + 8;
    v15 = *(double*)v13;
    *(double*)v14 = v15;
    p_call_stack_pop();
    return;
    return;
}

void Result_and_then(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Result_and_then");
    void* v4;
    void* v16;
    int64_t v12;
    void* v13;
    void* v18;
    int64_t v8; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v5;
    int64_t v29;
    int64_t v33;
    void* v36;
    int64_t v37; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    bool v30;
    void* v22;
    int64_t v35;
    void* v38;
    int64_t v42;
    void* v45;
    void* v11;
    int64_t v10;
    int64_t v39; /* placeholder for void */
    int64_t v46;
    void* v23;
    void* v9;
    int64_t v50; /* placeholder for void */
    int64_t v15;
    void* v19;
    void* v34;
    void* v40;
    int64_t v25; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v28;
    int64_t v48; /* placeholder for void */
    void* v27;
    int64_t v17; /* placeholder for void */
    bool v7;
    int64_t v49; /* placeholder for void */
    int64_t v6;
    int64_t v47; /* placeholder for void */
    int64_t v20;
    void* v32;
    void* v41;
    int64_t v24;
    int64_t v26; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    void* v44;
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
        if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "math_abs_i64") == 0) {
            v15 = math_abs_i64((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
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

void Vec_sort(void* v0, int64_t v1) {
    p_call_stack_push("Vec_sort");
    void* v4;
    void* v64;
    int64_t v13; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v21;
    int64_t v45;
    int64_t v46;
    void* v49;
    int64_t v59; /* placeholder for void */
    int64_t v44;
    void* v71;
    int64_t v48;
    int64_t v72;
    bool v77;
    bool v85;
    int64_t v17;
    int64_t v89; /* placeholder for void */
    int64_t v55;
    int64_t v93; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v47;
    int64_t v37; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v34;
    int64_t v94; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    void* v27;
    int64_t v75;
    int64_t v86; /* placeholder for void */
    bool v26;
    bool v30;
    int64_t v23; /* placeholder for void */
    void* v58;
    int64_t v84; /* placeholder for void */
    void* v73;
    void* v10;
    int64_t v80;
    int64_t v91;
    void* v14;
    void* v61;
    int64_t v62; /* placeholder for void */
    int64_t v76;
    int64_t v53;
    int64_t v2;
    int64_t v38;
    int64_t v66; /* placeholder for void */
    int64_t v54;
    int64_t v40;
    int64_t v43; /* placeholder for void */
    int64_t v52;
    int64_t v70; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v90;
    int64_t v20;
    int64_t v7; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v41;
    int64_t v11;
    int64_t v31; /* placeholder for void */
    void* v79;
    int64_t v95; /* placeholder for void */
    int64_t v3;
    bool v42;
    int64_t v92;
    int64_t v87; /* placeholder for void */
    bool v22;
    int64_t v39;
    int64_t v60;
    void* v69;
    int64_t v68;
    int64_t v63;
    int64_t v65; /* placeholder for void */
    void* v8;
    void* v35;
    int64_t v9; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    int64_t v88; /* placeholder for void */
    int64_t v56;
    int64_t v25; /* placeholder for void */
    void* v16;
    int64_t v51;
    int64_t v12; /* placeholder for void */
    void* v67;
    void* v6;
    void* v82;
    bool v32;
    int64_t v18; /* placeholder for void */
    int64_t v57;
    int64_t v83;
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

void Vec_extend(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("Vec_extend");
    int64_t v24; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    void* v34;
    int64_t v50;
    int64_t v35; /* placeholder for void */
    void* v43;
    void* v11;
    int64_t v4;
    int64_t v28; /* placeholder for void */
    int64_t v36;
    int64_t v13; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    int64_t v25;
    int64_t v3;
    int64_t v16; /* placeholder for void */
    int64_t v29;
    int64_t v47;
    int64_t v54; /* placeholder for void */
    int64_t v30;
    int64_t v12;
    void* v17;
    int64_t v33;
    int64_t v42; /* placeholder for void */
    int64_t v39;
    int64_t v52;
    void* v22;
    bool v27;
    int64_t v14; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    void* v40;
    int64_t v51;
    int64_t v10; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v44;
    int64_t v41; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    void* v9;
    int64_t v26;
    int64_t v18;
    int64_t v21;
    void* v5;
    void* v15;
    int64_t v31;
    int64_t v32;
    int64_t v45; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    void* v7;
    void* v37;
    int64_t v48;
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

void vec4_sub(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("vec4_sub");
    void* v31;
    double v39;
    int64_t v11; /* placeholder for void */
    double v22;
    double v43;
    double v47;
    double v16;
    void* v42;
    void* v12;
    int64_t v48; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    double v13;
    int64_t v44; /* placeholder for void */
    double v30;
    void* v45;
    double v35;
    void* v46;
    int64_t v49; /* placeholder for void */
    void* v24;
    void* v28;
    void* v33;
    int64_t v40; /* placeholder for void */
    void* v5;
    void* v21;
    void* v4;
    double v8;
    double v20;
    double v27;
    int64_t v18; /* placeholder for void */
    void* v17;
    double v23;
    double v6;
    int64_t v25; /* placeholder for void */
    double v15;
    void* v34;
    void* v7;
    void* v37;
    double v9;
    void* v41;
    void* v26;
    double v29;
    void* v14;
    void* v19;
    void* v38;
    void* v10;
    v4 = malloc(32);
    v5 = (char*)v2 + 0;
    v6 = *(double*)v5;
    v7 = (char*)v3 + 0;
    v8 = *(double*)v7;
    v9 = v6 - v8;
    v10 = (char*)v4 + 0;
    *(double*)v10 = v9;
    v12 = (char*)v2 + 8;
    v13 = *(double*)v12;
    v14 = (char*)v3 + 8;
    v15 = *(double*)v14;
    v16 = v13 - v15;
    v17 = (char*)v4 + 8;
    *(double*)v17 = v16;
    v19 = (char*)v2 + 16;
    v20 = *(double*)v19;
    v21 = (char*)v3 + 16;
    v22 = *(double*)v21;
    v23 = v20 - v22;
    v24 = (char*)v4 + 16;
    *(double*)v24 = v23;
    v26 = (char*)v2 + 24;
    v27 = *(double*)v26;
    v28 = (char*)v3 + 24;
    v29 = *(double*)v28;
    v30 = v27 - v29;
    v31 = (char*)v4 + 24;
    *(double*)v31 = v30;
    v33 = (char*)v4 + 0;
    v34 = (char*)v0 + 0;
    v35 = *(double*)v33;
    *(double*)v34 = v35;
    v37 = (char*)v4 + 8;
    v38 = (char*)v0 + 8;
    v39 = *(double*)v37;
    *(double*)v38 = v39;
    v41 = (char*)v4 + 16;
    v42 = (char*)v0 + 16;
    v43 = *(double*)v41;
    *(double*)v42 = v43;
    v45 = (char*)v4 + 24;
    v46 = (char*)v0 + 24;
    v47 = *(double*)v45;
    *(double*)v46 = v47;
    p_call_stack_pop();
    return;
    return;
}

void StringBuilder_clear(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_clear");
    int64_t v4; /* placeholder for void */
    int64_t v6;
    int64_t v8; /* placeholder for void */
    void* v3;
    void* v2;
    void* v5;
    int64_t v7; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = _str0;
    *(void**)v2 = (void*)v3;
    v5 = (char*)v1 + 8;
    v6 = 0LL;
    *(int64_t*)v5 = (int64_t)v6;
    p_call_stack_pop();
    return;
    return;
}

void vec3(void* v0, void* v1, double v2, double v3, double v4) {
    p_call_stack_push("vec3");
    int64_t v15; /* placeholder for void */
    void* v16;
    int64_t v7; /* placeholder for void */
    void* v13;
    int64_t v9; /* placeholder for void */
    void* v8;
    void* v5;
    double v14;
    double v18;
    void* v6;
    int64_t v19; /* placeholder for void */
    void* v10;
    void* v20;
    void* v12;
    void* v21;
    double v22;
    int64_t v23; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    void* v17;
    v5 = malloc(24);
    v6 = (char*)v5 + 0;
    *(double*)v6 = v2;
    v8 = (char*)v5 + 8;
    *(double*)v8 = v3;
    v10 = (char*)v5 + 16;
    *(double*)v10 = v4;
    v12 = (char*)v5 + 0;
    v13 = (char*)v0 + 0;
    v14 = *(double*)v12;
    *(double*)v13 = v14;
    v16 = (char*)v5 + 8;
    v17 = (char*)v0 + 8;
    v18 = *(double*)v16;
    *(double*)v17 = v18;
    v20 = (char*)v5 + 16;
    v21 = (char*)v0 + 16;
    v22 = *(double*)v20;
    *(double*)v21 = v22;
    p_call_stack_pop();
    return;
    return;
}

int64_t Option_expect(void* v0, void* v1, void* v2) {
    p_call_stack_push("Option_expect");
    int64_t v17; /* placeholder for void */
    int64_t v4;
    int64_t v10; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v12;
    void* v11;
    int64_t v9;
    bool v14;
    void* v8;
    int64_t v7; /* placeholder for void */
    void* v3;
    int64_t v19;
    int64_t v13;
    int64_t v15; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    bool v6;
    int64_t v5;
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

int64_t ByteBuffer_len(void* v0, void* v1) {
    p_call_stack_push("ByteBuffer_len");
    int64_t v4; /* placeholder for void */
    void* v2;
    int64_t v3;
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t String_clone(void* v0, int64_t v1) {
    p_call_stack_push("String_clone");
    int64_t v4; /* placeholder for void */
    int64_t v3;
    int64_t v2;
    v2 = 0LL;
    v3 = (int64_t)String_from((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

bool StringBuilder_is_empty(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_is_empty");
    int64_t v3;
    int64_t v4;
    void* v2;
    bool v5;
    int64_t v6; /* placeholder for void */
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    p_call_stack_pop();
    return v5;
    return 0;
}

void* p_str_from_ptr(void* v0, int64_t v1) {
    p_call_stack_push("p_str_from_ptr");
    void* v26;
    int64_t v87;
    void* v76;
    int64_t v101; /* placeholder for void */
    int64_t v39;
    int64_t v73;
    int64_t v11; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v68;
    int64_t v105; /* placeholder for void */
    int64_t v9;
    int64_t v55;
    int64_t v24; /* placeholder for void */
    int64_t v61; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    void* v60;
    int64_t v88;
    int64_t v21; /* placeholder for void */
    uint8_t v30;
    void* v62;
    int64_t v89;
    int64_t v98;
    int64_t v51; /* placeholder for void */
    int64_t v96;
    void* v103;
    int64_t v86; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    bool v74;
    void* v31;
    int64_t v72;
    void* v80;
    int64_t v52; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    void* v10;
    int64_t v12; /* placeholder for void */
    void* v20;
    void* v17;
    int64_t v33; /* placeholder for void */
    int64_t v44;
    int64_t v46;
    int64_t v64;
    int64_t v67; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    void* v23;
    int64_t v40;
    int64_t v16; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    uint8_t v77;
    int64_t v84;
    int64_t v34;
    int64_t v2;
    int64_t v85;
    int64_t v100;
    int64_t v109; /* placeholder for void */
    int64_t v5;
    void* v93;
    int64_t v32; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    bool v3;
    bool v36;
    int64_t v90; /* placeholder for void */
    void* v69;
    void* v110;
    int64_t v107;
    int64_t v42;
    int64_t v43;
    void* v108;
    int64_t v111; /* placeholder for void */
    int64_t v104;
    int64_t v38;
    int64_t v25;
    void* v7;
    void* v13;
    int64_t v27; /* placeholder for void */
    int64_t v83;
    int64_t v35;
    uint8_t v49;
    void* v65;
    int64_t v22;
    int64_t v41; /* placeholder for void */
    void* v6;
    int64_t v50; /* placeholder for void */
    int64_t v58; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    void* v56;
    int64_t v102; /* placeholder for void */
    int64_t v54;
    int64_t v106; /* placeholder for void */
    void* v29;
    int64_t v53;
    int64_t v14;
    void* v57;
    int64_t v81;
    int64_t v63; /* placeholder for void */
    int64_t v91;
    void* v48;
    int64_t v97;
    int64_t v59;
    void* v78;
    int64_t v92; /* placeholder for void */
    void* v94;
    int64_t v99; /* placeholder for void */
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

int64_t Vec_pop(void* v0, int64_t v1) {
    p_call_stack_push("Vec_pop");
    int64_t v27;
    int64_t v19; /* placeholder for void */
    int64_t v2;
    int64_t v11;
    int64_t v22; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v52;
    int64_t v15;
    int64_t v24;
    int64_t v25; /* placeholder for void */
    int64_t v46;
    int64_t v40; /* placeholder for void */
    void* v6;
    int64_t v5; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    void* v37;
    int64_t v3;
    int64_t v56; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    int64_t v14;
    void* v8;
    int64_t v31;
    void* v10;
    int64_t v35; /* placeholder for void */
    bool v16;
    int64_t v18;
    int64_t v50; /* placeholder for void */
    int64_t v47;
    int64_t v28;
    int64_t v29;
    int64_t v53; /* placeholder for void */
    int64_t v30;
    int64_t v55;
    int64_t v32;
    int64_t v33;
    int64_t v45;
    int64_t v26; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v48;
    void* v23;
    void* v39;
    int64_t v36;
    void* v41;
    int64_t v38; /* placeholder for void */
    void* v4;
    void* v21;
    int64_t v43; /* placeholder for void */
    void* v34;
    int64_t v20; /* placeholder for void */
    void* v51;
    int64_t v42;
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

double math_min_f64(void* v0, double v1, double v2) {
    p_call_stack_push("math_min_f64");
    int64_t v4; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    bool v3;
    int64_t v5; /* placeholder for void */
    v3 = v1 < v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return v1;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return v2;
    return 0;
}

void ByteBuffer_write_bytes(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("ByteBuffer_write_bytes");
    void* v5;
    int64_t v11;
    int64_t v14; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    void* v13;
    int64_t v15;
    int64_t v27;
    int64_t v33; /* placeholder for void */
    void* v16;
    uint8_t v23;
    int64_t v8;
    bool v9;
    int64_t v34; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v4;
    int64_t v12;
    int64_t v21; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v29;
    int64_t v7; /* placeholder for void */
    int64_t v31;
    int64_t v10; /* placeholder for void */
    int64_t v18;
    void* v22;
    int64_t v30;
    int64_t v26;
    int64_t v6; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    void* v19;
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

int64_t HashMap_new(void* v0) {
    p_call_stack_push("HashMap_new");
    int64_t v15;
    int64_t v22; /* placeholder for void */
    int64_t v18;
    int64_t v36; /* placeholder for void */
    void* v6;
    void* v16;
    void* v33;
    int64_t v38; /* placeholder for void */
    int64_t v28;
    int64_t v12; /* placeholder for void */
    int64_t v37;
    int64_t v13;
    int64_t v14;
    int64_t v1;
    void* v21;
    int64_t v4; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v3;
    int64_t v25;
    int64_t v29; /* placeholder for void */
    int64_t v10;
    void* v11;
    int64_t v23; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v9;
    int64_t v32; /* placeholder for void */
    void* v30;
    int64_t v35; /* placeholder for void */
    int64_t v31;
    int64_t v5;
    int64_t v34;
    void* v2;
    int64_t v20;
    void* v24;
    int64_t v8;
    int64_t v19;
    int64_t v17; /* placeholder for void */
    void* v27;
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

void* str_replace(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("str_replace");
    int64_t v6; /* placeholder for void */
    int64_t v4;
    int64_t v5;
    v4 = 0LL;
    v5 = (int64_t)String_replace((void*)v4, (int64_t)v1, (int64_t)v2, (int64_t)v3);
    p_call_stack_pop();
    return (void*)v5;
    return 0;
}

int64_t str_parse_int(void* v0, void* v1) {
    p_call_stack_push("str_parse_int");
    int64_t v3;
    int64_t v4; /* placeholder for void */
    int64_t v2;
    v2 = 0LL;
    v3 = (int64_t)String_parse_int((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

double distance(void* v0, void* v1, void* v2) {
    p_call_stack_push("distance");
    double v18;
    void* v12;
    int64_t v16; /* placeholder for void */
    double v21;
    void* v8;
    void* v22;
    double v35;
    int64_t v23; /* placeholder for void */
    void* v15;
    double v28;
    double v31;
    double v13;
    double v27;
    double v25;
    double v6;
    double v26;
    void* v3;
    double v30;
    double v11;
    void* v10;
    double v33;
    double v29;
    double v24;
    double v32;
    int64_t v36; /* placeholder for void */
    double v20;
    double v7;
    double v14;
    double v34;
    int64_t v9; /* placeholder for void */
    void* v17;
    double v4;
    void* v5;
    void* v19;
    v3 = (char*)v1 + 0;
    v4 = *(double*)v3;
    v5 = (char*)v2 + 0;
    v6 = *(double*)v5;
    v7 = v4 - v6;
    v8 = malloc(8);
    *(double*)v8 = v7;
    v10 = (char*)v1 + 8;
    v11 = *(double*)v10;
    v12 = (char*)v2 + 8;
    v13 = *(double*)v12;
    v14 = v11 - v13;
    v15 = malloc(8);
    *(double*)v15 = v14;
    v17 = (char*)v1 + 16;
    v18 = *(double*)v17;
    v19 = (char*)v2 + 16;
    v20 = *(double*)v19;
    v21 = v18 - v20;
    v22 = malloc(8);
    *(double*)v22 = v21;
    v24 = *(double*)v8;
    v25 = *(double*)v8;
    v26 = v24 * v25;
    v27 = *(double*)v15;
    v28 = *(double*)v15;
    v29 = v27 * v28;
    v30 = v26 + v29;
    v31 = *(double*)v22;
    v32 = *(double*)v22;
    v33 = v31 * v32;
    v34 = v30 + v33;
    v35 = (double)pledge_sqrt(v34);
    p_call_stack_pop();
    return v35;
    return 0;
}

int64_t Vec_new(void* v0) {
    p_call_stack_push("Vec_new");
    int64_t v5;
    void* v3;
    int64_t v13;
    int64_t v24; /* placeholder for void */
    int64_t v23;
    int64_t v28; /* placeholder for void */
    int64_t v15;
    int64_t v4; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v14;
    void* v25;
    int64_t v8;
    int64_t v27; /* placeholder for void */
    void* v6;
    int64_t v9;
    int64_t v7; /* placeholder for void */
    int64_t v29;
    int64_t v26;
    int64_t v30; /* placeholder for void */
    void* v2;
    int64_t v10;
    int64_t v21; /* placeholder for void */
    void* v16;
    void* v22;
    int64_t v1;
    int64_t v18; /* placeholder for void */
    int64_t v20;
    void* v19;
    void* v11;
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

double math_log(void* v0, double v1) {
    p_call_stack_push("math_log");
    int64_t v3; /* placeholder for void */
    double v2;
    v2 = (double)pledge_log(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t Result_unwrap_or(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Result_unwrap_or");
    int64_t v4;
    int64_t v10; /* placeholder for void */
    bool v14;
    int64_t v18; /* placeholder for void */
    void* v3;
    int64_t v19; /* placeholder for void */
    int64_t v9;
    int64_t v17;
    int64_t v5;
    void* v11;
    int64_t v20;
    int64_t v13;
    int64_t v7; /* placeholder for void */
    bool v6;
    void* v8;
    int64_t v12;
    void* v16;
    int64_t v21; /* placeholder for void */
    int64_t v15; /* placeholder for void */
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

int64_t HashMap_clone(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_clone");
    void* v4;
    void* v10;
    int64_t v73;
    int64_t v108;
    int64_t v16;
    int64_t v94; /* placeholder for void */
    int64_t v12;
    int64_t v30;
    bool v79;
    int64_t v59;
    int64_t v106; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v102;
    void* v48;
    int64_t v85;
    void* v87;
    int64_t v82;
    int64_t v13;
    int64_t v84;
    void* v32;
    int64_t v89;
    void* v26;
    int64_t v97;
    int64_t v95; /* placeholder for void */
    void* v20;
    int64_t v52;
    int64_t v25; /* placeholder for void */
    bool v75;
    void* v8;
    bool v53;
    int64_t v98; /* placeholder for void */
    int64_t v65;
    int64_t v22;
    int64_t v15; /* placeholder for void */
    void* v66;
    int64_t v100;
    int64_t v103;
    int64_t v111; /* placeholder for void */
    void* v69;
    bool v41;
    int64_t v101;
    int64_t v109;
    int64_t v2;
    int64_t v31; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    void* v93;
    int64_t v9; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v37;
    int64_t v91; /* placeholder for void */
    int64_t v7;
    int64_t v47;
    void* v63;
    int64_t v44;
    int64_t v57;
    int64_t v74;
    int64_t v6;
    int64_t v50; /* placeholder for void */
    void* v17;
    int64_t v5; /* placeholder for void */
    int64_t v107;
    int64_t v3;
    int64_t v70;
    int64_t v112;
    int64_t v67; /* placeholder for void */
    int64_t v40;
    int64_t v62;
    int64_t v105; /* placeholder for void */
    void* v90;
    bool v42;
    int64_t v104; /* placeholder for void */
    int64_t v88; /* placeholder for void */
    int64_t v56;
    int64_t v99; /* placeholder for void */
    int64_t v110; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    int64_t v55;
    int64_t v43; /* placeholder for void */
    int64_t v39;
    int64_t v11; /* placeholder for void */
    void* v14;
    int64_t v58;
    int64_t v54; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v36;
    int64_t v61; /* placeholder for void */
    int64_t v33;
    int64_t v18; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v83;
    void* v96;
    int64_t v113; /* placeholder for void */
    int64_t v51;
    int64_t v77;
    int64_t v86;
    int64_t v92;
    int64_t v49; /* placeholder for void */
    void* v60;
    int64_t v27;
    int64_t v76;
    void* v29;
    bool v80;
    int64_t v78;
    bool v38;
    int64_t v72; /* placeholder for void */
    int64_t v19;
    int64_t v24; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    void* v23;
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

void vec2_lerp(void* v0, void* v1, void* v2, void* v3, double v4) {
    p_call_stack_push("vec2_lerp");
    void* v17;
    void* v26;
    double v18;
    void* v10;
    double v12;
    double v7;
    void* v8;
    void* v28;
    void* v15;
    int64_t v35; /* placeholder for void */
    void* v6;
    double v34;
    double v9;
    double v13;
    double v25;
    double v30;
    void* v32;
    void* v19;
    int64_t v27; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    double v20;
    int64_t v36; /* placeholder for void */
    void* v21;
    int64_t v31; /* placeholder for void */
    double v22;
    double v24;
    void* v29;
    double v11;
    double v14;
    double v23;
    void* v5;
    void* v33;
    v5 = malloc(16);
    v6 = (char*)v2 + 0;
    v7 = *(double*)v6;
    v8 = (char*)v3 + 0;
    v9 = *(double*)v8;
    v10 = (char*)v2 + 0;
    v11 = *(double*)v10;
    v12 = v9 - v11;
    v13 = v12 * v4;
    v14 = v7 + v13;
    v15 = (char*)v5 + 0;
    *(double*)v15 = v14;
    v17 = (char*)v2 + 8;
    v18 = *(double*)v17;
    v19 = (char*)v3 + 8;
    v20 = *(double*)v19;
    v21 = (char*)v2 + 8;
    v22 = *(double*)v21;
    v23 = v20 - v22;
    v24 = v23 * v4;
    v25 = v18 + v24;
    v26 = (char*)v5 + 8;
    *(double*)v26 = v25;
    v28 = (char*)v5 + 0;
    v29 = (char*)v0 + 0;
    v30 = *(double*)v28;
    *(double*)v29 = v30;
    v32 = (char*)v5 + 8;
    v33 = (char*)v0 + 8;
    v34 = *(double*)v32;
    *(double*)v33 = v34;
    p_call_stack_pop();
    return;
    return;
}

int64_t Vec_remove(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("Vec_remove");
    int64_t v24; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    int64_t v89;
    int64_t v101; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    int64_t v107;
    void* v25;
    int64_t v58;
    int64_t v32;
    void* v84;
    int64_t v108; /* placeholder for void */
    void* v45;
    int64_t v38;
    int64_t v65;
    int64_t v72;
    int64_t v83; /* placeholder for void */
    int64_t v22;
    int64_t v40; /* placeholder for void */
    int64_t v68;
    int64_t v35;
    bool v59;
    int64_t v91; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v12;
    int64_t v50; /* placeholder for void */
    int64_t v77;
    int64_t v15;
    int64_t v74;
    int64_t v94;
    void* v48;
    int64_t v3;
    int64_t v69;
    int64_t v79; /* placeholder for void */
    int64_t v63;
    int64_t v85;
    int64_t v44;
    void* v5;
    int64_t v41;
    int64_t v73;
    int64_t v60; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    void* v86;
    int64_t v93;
    int64_t v97;
    void* v7;
    void* v11;
    int64_t v43; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v49;
    int64_t v64;
    void* v78;
    int64_t v98;
    int64_t v62;
    int64_t v53; /* placeholder for void */
    void* v66;
    int64_t v8; /* placeholder for void */
    void* v81;
    int64_t v87; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    void* v75;
    int64_t v57;
    int64_t v76; /* placeholder for void */
    void* v18;
    int64_t v80;
    int64_t v36;
    bool v16;
    void* v31;
    void* v9;
    int64_t v28; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v4;
    int64_t v55;
    int64_t v37;
    int64_t v54; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    void* v88;
    void* v29;
    int64_t v34; /* placeholder for void */
    int64_t v92;
    int64_t v96; /* placeholder for void */
    int64_t v56;
    int64_t v99;
    int64_t v14; /* placeholder for void */
    bool v23;
    int64_t v51; /* placeholder for void */
    int64_t v61;
    void* v52;
    void* v42;
    void* v103;
    int64_t v104;
    int64_t v105; /* placeholder for void */
    int64_t v102; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    void* v100;
    int64_t v70;
    int64_t v21; /* placeholder for void */
    void* v39;
    int64_t v71;
    int64_t v33; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v67; /* placeholder for void */
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
    v18 = _str50;
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
    v25 = _str50;
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

int64_t p_str_compare(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("p_str_compare");
    int64_t v63;
    int64_t v13; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    void* v70;
    bool v43;
    int64_t v19; /* placeholder for void */
    bool v47;
    int64_t v18;
    uint8_t v74;
    int64_t v78;
    int64_t v11;
    int64_t v64;
    uint8_t v33;
    int64_t v53;
    int64_t v79;
    int64_t v41;
    int64_t v68;
    int64_t v17; /* placeholder for void */
    int64_t v49;
    int64_t v61; /* placeholder for void */
    int64_t v77;
    int64_t v72; /* placeholder for void */
    uint8_t v37;
    int64_t v52; /* placeholder for void */
    bool v9;
    int64_t v42;
    int64_t v4; /* placeholder for void */
    int64_t v54;
    int64_t v44;
    int64_t v50;
    bool v3;
    int64_t v28;
    void* v38;
    void* v32;
    int64_t v59;
    int64_t v57; /* placeholder for void */
    int64_t v80; /* placeholder for void */
    bool v51;
    bool v16;
    int64_t v6; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    void* v36;
    int64_t v40; /* placeholder for void */
    bool v46;
    int64_t v39; /* placeholder for void */
    int64_t v60;
    int64_t v75; /* placeholder for void */
    int64_t v5;
    void* v29;
    int64_t v66;
    int64_t v81; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v12;
    int64_t v45;
    int64_t v62;
    void* v34;
    int64_t v10; /* placeholder for void */
    int64_t v15;
    int64_t v24; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    void* v23;
    int64_t v55;
    int64_t v8;
    void* v21;
    int64_t v56; /* placeholder for void */
    void* v73;
    int64_t v58;
    int64_t v69; /* placeholder for void */
    uint8_t v71;
    void* v26;
    int64_t v27; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v65; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v25;
    int64_t v76; /* placeholder for void */
    int64_t v30; /* placeholder for void */
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

int64_t Vec_len(void* v0, int64_t v1) {
    p_call_stack_push("Vec_len");
    int64_t v7; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v9;
    int64_t v10; /* placeholder for void */
    int64_t v12;
    int64_t v2;
    void* v6;
    void* v8;
    int64_t v3;
    void* v4;
    int64_t v13; /* placeholder for void */
    int64_t v11; /* placeholder for void */
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

bool char_is_whitespace(void* v0, int64_t v1) {
    p_call_stack_push("char_is_whitespace");
    bool v3;
    bool v6;
    int64_t v4;
    int64_t v10;
    bool v12;
    int64_t v13; /* placeholder for void */
    int64_t v7;
    int64_t v2;
    bool v5;
    bool v8;
    bool v9;
    bool v11;
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

int64_t char_to_upper(void* v0, int64_t v1) {
    p_call_stack_push("char_to_upper");
    int64_t v9;
    bool v3;
    bool v6;
    int64_t v4;
    bool v5;
    int64_t v10; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v8;
    int64_t v12; /* placeholder for void */
    int64_t v2;
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

double math_clamp_f64(void* v0, double v1, double v2, double v3) {
    p_call_stack_push("math_clamp_f64");
    bool v4;
    int64_t v11; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    bool v8;
    int64_t v5; /* placeholder for void */
    int64_t v10; /* placeholder for void */
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

int64_t String_slice(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("String_slice");
    int64_t v25; /* placeholder for void */
    void* v15;
    int64_t v18; /* placeholder for void */
    int64_t v23;
    void* v28;
    int64_t v59;
    void* v24;
    int64_t v46;
    int64_t v14; /* placeholder for void */
    int64_t v71;
    int64_t v16;
    int64_t v45; /* placeholder for void */
    int64_t v47;
    int64_t v56; /* placeholder for void */
    int64_t v72;
    int64_t v93; /* placeholder for void */
    bool v5;
    void* v8;
    int64_t v54;
    void* v58;
    int64_t v67;
    bool v32;
    int64_t v92;
    int64_t v95;
    int64_t v6; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v9;
    void* v51;
    void* v55;
    int64_t v80;
    int64_t v19;
    int64_t v30;
    int64_t v57; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    void* v73;
    int64_t v29; /* placeholder for void */
    int64_t v76;
    int64_t v13; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    int64_t v79;
    int64_t v96; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v50;
    int64_t v34;
    int64_t v31;
    int64_t v11;
    void* v85;
    int64_t v37; /* placeholder for void */
    int64_t v78;
    int64_t v81; /* placeholder for void */
    void* v12;
    int64_t v35; /* placeholder for void */
    void* v68;
    int64_t v70;
    int64_t v83;
    int64_t v7;
    int64_t v66;
    void* v74;
    int64_t v53; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v62;
    int64_t v17; /* placeholder for void */
    int64_t v77;
    int64_t v22;
    int64_t v69; /* placeholder for void */
    int64_t v82;
    int64_t v84;
    void* v91;
    int64_t v42;
    int64_t v21; /* placeholder for void */
    bool v48;
    void* v52;
    int64_t v89; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v38;
    int64_t v75; /* placeholder for void */
    void* v88;
    int64_t v39;
    int64_t v61; /* placeholder for void */
    int64_t v65;
    void* v26;
    int64_t v87;
    int64_t v4;
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

int64_t str_rfind(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_rfind");
    int64_t v5; /* placeholder for void */
    int64_t v4;
    int64_t v3;
    v3 = 0LL;
    v4 = (int64_t)String_rfind((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

void Option_or_else(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Option_or_else");
    void* v39;
    void* v43;
    int64_t v44;
    void* v9;
    int64_t v10;
    int64_t v28;
    int64_t v34; /* placeholder for void */
    void* v38;
    int64_t v16; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    void* v42;
    void* v15;
    void* v26;
    int64_t v47; /* placeholder for void */
    void* v13;
    bool v7;
    int64_t v25; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    void* v21;
    int64_t v37; /* placeholder for void */
    int64_t v19;
    void* v17;
    void* v31;
    void* v33;
    int64_t v35;
    int64_t v32;
    void* v4;
    int64_t v5;
    int64_t v14; /* placeholder for void */
    int64_t v23;
    int64_t v40;
    int64_t v8; /* placeholder for void */
    int64_t v27;
    int64_t v20; /* placeholder for void */
    void* v22;
    bool v29;
    void* v11;
    int64_t v48; /* placeholder for void */
    int64_t v12;
    int64_t v6;
    int64_t v45; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    void* v36;
    void* v18;
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

void translate(void* v0, void* v1, void* v2) {
    p_call_stack_push("translate");
    void* v99;
    void* v8;
    void* v68;
    double v7;
    int64_t v6; /* placeholder for void */
    double v77;
    int64_t v38; /* placeholder for void */
    double v65;
    int64_t v54; /* placeholder for void */
    double v14;
    void* v116;
    int64_t v66; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    void* v67;
    double v113;
    double v52;
    int64_t v70; /* placeholder for void */
    void* v53;
    void* v72;
    int64_t v22; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    double v97;
    int64_t v45; /* placeholder for void */
    void* v31;
    void* v47;
    int64_t v48; /* placeholder for void */
    void* v79;
    int64_t v119; /* placeholder for void */
    double v40;
    void* v96;
    void* v107;
    double v23;
    int64_t v110; /* placeholder for void */
    void* v84;
    double v89;
    void* v55;
    double v27;
    int64_t v106; /* placeholder for void */
    void* v103;
    void* v95;
    void* v39;
    void* v15;
    double v17;
    void* v18;
    void* v5;
    void* v3;
    void* v60;
    void* v76;
    void* v115;
    int64_t v82; /* placeholder for void */
    void* v111;
    void* v80;
    int64_t v12; /* placeholder for void */
    void* v37;
    double v81;
    void* v59;
    void* v83;
    double v101;
    double v4;
    double v85;
    void* v28;
    double v61;
    void* v63;
    void* v75;
    int64_t v98; /* placeholder for void */
    double v117;
    double v109;
    double v93;
    void* v34;
    void* v11;
    int64_t v32; /* placeholder for void */
    double v20;
    double v36;
    void* v108;
    int64_t v9; /* placeholder for void */
    double v33;
    void* v88;
    void* v112;
    void* v104;
    double v46;
    void* v71;
    double v43;
    int64_t v78; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    void* v44;
    double v69;
    int64_t v114; /* placeholder for void */
    void* v91;
    void* v56;
    double v57;
    double v105;
    int64_t v118; /* placeholder for void */
    double v30;
    void* v13;
    void* v92;
    void* v26;
    int64_t v42; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    void* v21;
    int64_t v102; /* placeholder for void */
    double v73;
    int64_t v19; /* placeholder for void */
    int64_t v58; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    void* v87;
    double v49;
    double v10;
    int64_t v29; /* placeholder for void */
    void* v24;
    void* v41;
    int64_t v94; /* placeholder for void */
    void* v100;
    int64_t v74; /* placeholder for void */
    void* v64;
    void* v50;
    v3 = malloc(128);
    v4 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v5 = (char*)v3 + 0;
    *(double*)v5 = v4;
    v7 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v8 = (char*)v3 + 8;
    *(double*)v8 = v7;
    v10 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v11 = (char*)v3 + 16;
    *(double*)v11 = v10;
    v13 = (char*)v2 + 0;
    v14 = *(double*)v13;
    v15 = (char*)v3 + 24;
    *(double*)v15 = v14;
    v17 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v18 = (char*)v3 + 32;
    *(double*)v18 = v17;
    v20 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v21 = (char*)v3 + 40;
    *(double*)v21 = v20;
    v23 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v24 = (char*)v3 + 48;
    *(double*)v24 = v23;
    v26 = (char*)v2 + 8;
    v27 = *(double*)v26;
    v28 = (char*)v3 + 56;
    *(double*)v28 = v27;
    v30 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v31 = (char*)v3 + 64;
    *(double*)v31 = v30;
    v33 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v34 = (char*)v3 + 72;
    *(double*)v34 = v33;
    v36 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v37 = (char*)v3 + 80;
    *(double*)v37 = v36;
    v39 = (char*)v2 + 16;
    v40 = *(double*)v39;
    v41 = (char*)v3 + 88;
    *(double*)v41 = v40;
    v43 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v44 = (char*)v3 + 96;
    *(double*)v44 = v43;
    v46 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v47 = (char*)v3 + 104;
    *(double*)v47 = v46;
    v49 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v50 = (char*)v3 + 112;
    *(double*)v50 = v49;
    v52 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v53 = (char*)v3 + 120;
    *(double*)v53 = v52;
    v55 = (char*)v3 + 0;
    v56 = (char*)v0 + 0;
    v57 = *(double*)v55;
    *(double*)v56 = v57;
    v59 = (char*)v3 + 8;
    v60 = (char*)v0 + 8;
    v61 = *(double*)v59;
    *(double*)v60 = v61;
    v63 = (char*)v3 + 16;
    v64 = (char*)v0 + 16;
    v65 = *(double*)v63;
    *(double*)v64 = v65;
    v67 = (char*)v3 + 24;
    v68 = (char*)v0 + 24;
    v69 = *(double*)v67;
    *(double*)v68 = v69;
    v71 = (char*)v3 + 32;
    v72 = (char*)v0 + 32;
    v73 = *(double*)v71;
    *(double*)v72 = v73;
    v75 = (char*)v3 + 40;
    v76 = (char*)v0 + 40;
    v77 = *(double*)v75;
    *(double*)v76 = v77;
    v79 = (char*)v3 + 48;
    v80 = (char*)v0 + 48;
    v81 = *(double*)v79;
    *(double*)v80 = v81;
    v83 = (char*)v3 + 56;
    v84 = (char*)v0 + 56;
    v85 = *(double*)v83;
    *(double*)v84 = v85;
    v87 = (char*)v3 + 64;
    v88 = (char*)v0 + 64;
    v89 = *(double*)v87;
    *(double*)v88 = v89;
    v91 = (char*)v3 + 72;
    v92 = (char*)v0 + 72;
    v93 = *(double*)v91;
    *(double*)v92 = v93;
    v95 = (char*)v3 + 80;
    v96 = (char*)v0 + 80;
    v97 = *(double*)v95;
    *(double*)v96 = v97;
    v99 = (char*)v3 + 88;
    v100 = (char*)v0 + 88;
    v101 = *(double*)v99;
    *(double*)v100 = v101;
    v103 = (char*)v3 + 96;
    v104 = (char*)v0 + 96;
    v105 = *(double*)v103;
    *(double*)v104 = v105;
    v107 = (char*)v3 + 104;
    v108 = (char*)v0 + 104;
    v109 = *(double*)v107;
    *(double*)v108 = v109;
    v111 = (char*)v3 + 112;
    v112 = (char*)v0 + 112;
    v113 = *(double*)v111;
    *(double*)v112 = v113;
    v115 = (char*)v3 + 120;
    v116 = (char*)v0 + 120;
    v117 = *(double*)v115;
    *(double*)v116 = v117;
    p_call_stack_pop();
    return;
    return;
}

void vec4(void* v0, void* v1, double v2, double v3, double v4, double v5) {
    p_call_stack_push("vec4");
    void* v15;
    int64_t v22; /* placeholder for void */
    double v25;
    void* v16;
    double v21;
    int64_t v18; /* placeholder for void */
    void* v20;
    void* v27;
    void* v13;
    void* v11;
    int64_t v10; /* placeholder for void */
    void* v19;
    void* v23;
    void* v7;
    double v29;
    double v17;
    int64_t v26; /* placeholder for void */
    void* v24;
    void* v9;
    int64_t v31; /* placeholder for void */
    void* v6;
    int64_t v8; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    void* v28;
    int64_t v30; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    v6 = malloc(32);
    v7 = (char*)v6 + 0;
    *(double*)v7 = v2;
    v9 = (char*)v6 + 8;
    *(double*)v9 = v3;
    v11 = (char*)v6 + 16;
    *(double*)v11 = v4;
    v13 = (char*)v6 + 24;
    *(double*)v13 = v5;
    v15 = (char*)v6 + 0;
    v16 = (char*)v0 + 0;
    v17 = *(double*)v15;
    *(double*)v16 = v17;
    v19 = (char*)v6 + 8;
    v20 = (char*)v0 + 8;
    v21 = *(double*)v19;
    *(double*)v20 = v21;
    v23 = (char*)v6 + 16;
    v24 = (char*)v0 + 16;
    v25 = *(double*)v23;
    *(double*)v24 = v25;
    v27 = (char*)v6 + 24;
    v28 = (char*)v0 + 24;
    v29 = *(double*)v27;
    *(double*)v28 = v29;
    p_call_stack_pop();
    return;
    return;
}

void mat_scale(void* v0, void* v1, double v2) {
    p_call_stack_push("mat_scale");
    void* v4;
    void* v44;
    int64_t v112; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    double v15;
    void* v106;
    void* v54;
    void* v93;
    void* v109;
    void* v35;
    double v40;
    double v75;
    void* v102;
    void* v18;
    int64_t v45; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    void* v57;
    double v87;
    double v99;
    double v51;
    int64_t v39; /* placeholder for void */
    void* v66;
    double v67;
    double v107;
    void* v38;
    void* v110;
    void* v69;
    void* v78;
    void* v82;
    int64_t v28; /* placeholder for void */
    void* v49;
    void* v21;
    int64_t v25; /* placeholder for void */
    void* v97;
    int64_t v88; /* placeholder for void */
    void* v105;
    void* v62;
    int64_t v14; /* placeholder for void */
    void* v61;
    void* v90;
    void* v65;
    double v63;
    double v71;
    void* v77;
    int64_t v80; /* placeholder for void */
    int64_t v113; /* placeholder for void */
    double v55;
    int64_t v104; /* placeholder for void */
    void* v41;
    double v12;
    double v9;
    double v20;
    void* v30;
    void* v7;
    double v43;
    int64_t v100; /* placeholder for void */
    void* v86;
    double v59;
    double v111;
    int64_t v5; /* placeholder for void */
    void* v70;
    int64_t v108; /* placeholder for void */
    void* v13;
    int64_t v19; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    void* v16;
    int64_t v8; /* placeholder for void */
    void* v81;
    int64_t v68; /* placeholder for void */
    void* v98;
    double v95;
    void* v101;
    int64_t v84; /* placeholder for void */
    void* v53;
    double v83;
    int64_t v96; /* placeholder for void */
    void* v58;
    double v29;
    double v46;
    int64_t v52; /* placeholder for void */
    int64_t v72; /* placeholder for void */
    void* v32;
    double v23;
    void* v73;
    void* v27;
    int64_t v48; /* placeholder for void */
    void* v50;
    void* v3;
    void* v24;
    int64_t v36; /* placeholder for void */
    double v91;
    double v6;
    int64_t v42; /* placeholder for void */
    void* v74;
    int64_t v33; /* placeholder for void */
    void* v89;
    int64_t v64; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    double v26;
    int64_t v17; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    void* v10;
    double v34;
    void* v94;
    double v103;
    int64_t v22; /* placeholder for void */
    void* v47;
    double v37;
    double v79;
    void* v85;
    v3 = malloc(128);
    v4 = (char*)v3 + 0;
    *(double*)v4 = v2;
    v6 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v7 = (char*)v3 + 8;
    *(double*)v7 = v6;
    v9 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v10 = (char*)v3 + 16;
    *(double*)v10 = v9;
    v12 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v13 = (char*)v3 + 24;
    *(double*)v13 = v12;
    v15 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v16 = (char*)v3 + 32;
    *(double*)v16 = v15;
    v18 = (char*)v3 + 40;
    *(double*)v18 = v2;
    v20 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v21 = (char*)v3 + 48;
    *(double*)v21 = v20;
    v23 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v24 = (char*)v3 + 56;
    *(double*)v24 = v23;
    v26 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v27 = (char*)v3 + 64;
    *(double*)v27 = v26;
    v29 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v30 = (char*)v3 + 72;
    *(double*)v30 = v29;
    v32 = (char*)v3 + 80;
    *(double*)v32 = v2;
    v34 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v35 = (char*)v3 + 88;
    *(double*)v35 = v34;
    v37 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v38 = (char*)v3 + 96;
    *(double*)v38 = v37;
    v40 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v41 = (char*)v3 + 104;
    *(double*)v41 = v40;
    v43 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v44 = (char*)v3 + 112;
    *(double*)v44 = v43;
    v46 = ((union { uint64_t u; double f; }){ .u = 4607182418800017408ULL }).f;
    v47 = (char*)v3 + 120;
    *(double*)v47 = v46;
    v49 = (char*)v3 + 0;
    v50 = (char*)v0 + 0;
    v51 = *(double*)v49;
    *(double*)v50 = v51;
    v53 = (char*)v3 + 8;
    v54 = (char*)v0 + 8;
    v55 = *(double*)v53;
    *(double*)v54 = v55;
    v57 = (char*)v3 + 16;
    v58 = (char*)v0 + 16;
    v59 = *(double*)v57;
    *(double*)v58 = v59;
    v61 = (char*)v3 + 24;
    v62 = (char*)v0 + 24;
    v63 = *(double*)v61;
    *(double*)v62 = v63;
    v65 = (char*)v3 + 32;
    v66 = (char*)v0 + 32;
    v67 = *(double*)v65;
    *(double*)v66 = v67;
    v69 = (char*)v3 + 40;
    v70 = (char*)v0 + 40;
    v71 = *(double*)v69;
    *(double*)v70 = v71;
    v73 = (char*)v3 + 48;
    v74 = (char*)v0 + 48;
    v75 = *(double*)v73;
    *(double*)v74 = v75;
    v77 = (char*)v3 + 56;
    v78 = (char*)v0 + 56;
    v79 = *(double*)v77;
    *(double*)v78 = v79;
    v81 = (char*)v3 + 64;
    v82 = (char*)v0 + 64;
    v83 = *(double*)v81;
    *(double*)v82 = v83;
    v85 = (char*)v3 + 72;
    v86 = (char*)v0 + 72;
    v87 = *(double*)v85;
    *(double*)v86 = v87;
    v89 = (char*)v3 + 80;
    v90 = (char*)v0 + 80;
    v91 = *(double*)v89;
    *(double*)v90 = v91;
    v93 = (char*)v3 + 88;
    v94 = (char*)v0 + 88;
    v95 = *(double*)v93;
    *(double*)v94 = v95;
    v97 = (char*)v3 + 96;
    v98 = (char*)v0 + 96;
    v99 = *(double*)v97;
    *(double*)v98 = v99;
    v101 = (char*)v3 + 104;
    v102 = (char*)v0 + 104;
    v103 = *(double*)v101;
    *(double*)v102 = v103;
    v105 = (char*)v3 + 112;
    v106 = (char*)v0 + 112;
    v107 = *(double*)v105;
    *(double*)v106 = v107;
    v109 = (char*)v3 + 120;
    v110 = (char*)v0 + 120;
    v111 = *(double*)v109;
    *(double*)v110 = v111;
    p_call_stack_pop();
    return;
    return;
}

bool Vec_is_empty(void* v0, int64_t v1) {
    p_call_stack_push("Vec_is_empty");
    void* v8;
    int64_t v11; /* placeholder for void */
    int64_t v12;
    int64_t v2;
    int64_t v3;
    int64_t v13;
    int64_t v15; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    void* v4;
    int64_t v7; /* placeholder for void */
    int64_t v9;
    void* v6;
    int64_t v10; /* placeholder for void */
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

bool String_starts_with(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_starts_with");
    int64_t v30; /* placeholder for void */
    int64_t v73; /* placeholder for void */
    int64_t v49;
    bool v24;
    int64_t v71;
    int64_t v42; /* placeholder for void */
    int64_t v12;
    int64_t v48; /* placeholder for void */
    void* v53;
    int64_t v23; /* placeholder for void */
    int64_t v32;
    bool v33;
    void* v44;
    void* v14;
    int64_t v13;
    void* v47;
    void* v50;
    int64_t v26; /* placeholder for void */
    void* v41;
    int64_t v11; /* placeholder for void */
    int64_t v36;
    int64_t v40;
    int64_t v76; /* placeholder for void */
    int64_t v39;
    int64_t v38; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v72;
    int64_t v54; /* placeholder for void */
    int64_t v58; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    uint8_t v60;
    bool v6;
    bool v22;
    void* v59;
    int64_t v61; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v43;
    int64_t v20;
    int64_t v64;
    int64_t v17;
    int64_t v3;
    int64_t v21;
    int64_t v29; /* placeholder for void */
    void* v56;
    int64_t v66; /* placeholder for void */
    bool v4;
    int64_t v51; /* placeholder for void */
    bool v65;
    int64_t v34; /* placeholder for void */
    bool v7;
    void* v18;
    int64_t v35;
    int64_t v5;
    int64_t v68; /* placeholder for void */
    int64_t v70;
    uint8_t v57;
    int64_t v19; /* placeholder for void */
    void* v28;
    bool v67;
    int64_t v52;
    int64_t v55; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v27;
    int64_t v16;
    void* v37;
    int64_t v46;
    int64_t v62; /* placeholder for void */
    int64_t v63;
    bool v75;
    bool v9;
    int64_t v31;
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

void Result_ok(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_ok");
    int64_t v35; /* placeholder for void */
    void* v10;
    int64_t v44; /* placeholder for void */
    void* v21;
    int64_t v13; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v27;
    void* v8;
    int64_t v9;
    int64_t v22;
    int64_t v24; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    bool v6;
    void* v41;
    int64_t v43; /* placeholder for void */
    int64_t v5;
    int64_t v7; /* placeholder for void */
    void* v17;
    int64_t v11;
    int64_t v19; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v4;
    int64_t v15; /* placeholder for void */
    void* v16;
    int64_t v18;
    void* v20;
    void* v30;
    void* v36;
    int64_t v45; /* placeholder for void */
    void* v40;
    void* v3;
    int64_t v38;
    void* v14;
    void* v25;
    int64_t v26;
    bool v28;
    void* v34;
    void* v37;
    int64_t v42;
    int64_t v31;
    void* v32;
    int64_t v46; /* placeholder for void */
    void* v12;
    int64_t v33;
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

int64_t str_compare(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_compare");
    int64_t v3;
    int64_t v4;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)p_str_compare((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

int64_t HashMap_find_slot(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_find_slot");
    int64_t v74; /* placeholder for void */
    int64_t v102; /* placeholder for void */
    int64_t v93; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v100;
    int64_t v91;
    int64_t v129; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v21;
    int64_t v18;
    int64_t v24;
    int64_t v109;
    int64_t v95; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    void* v39;
    int64_t v44; /* placeholder for void */
    int64_t v54;
    int64_t v85;
    void* v64;
    int64_t v6; /* placeholder for void */
    bool v113;
    int64_t v4;
    int64_t v78; /* placeholder for void */
    int64_t v128;
    int64_t v63;
    int64_t v33;
    int64_t v124;
    int64_t v55;
    int64_t v110; /* placeholder for void */
    void* v5;
    int64_t v96; /* placeholder for void */
    int64_t v99;
    int64_t v66; /* placeholder for void */
    int64_t v86;
    int64_t v90;
    int64_t v132; /* placeholder for void */
    bool v51;
    int64_t v123; /* placeholder for void */
    int64_t v37;
    int64_t v38;
    int64_t v111;
    int64_t v116; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    void* v28;
    int64_t v9;
    int64_t v14;
    int64_t v118; /* placeholder for void */
    bool v77;
    int64_t v32;
    void* v43;
    int64_t v50;
    int64_t v80; /* placeholder for void */
    void* v61;
    int64_t v84; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v117; /* placeholder for void */
    int64_t v103;
    void* v58;
    int64_t v97; /* placeholder for void */
    int64_t v105; /* placeholder for void */
    int64_t v131;
    bool v19;
    bool v92;
    int64_t v65; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v49;
    int64_t v122; /* placeholder for void */
    int64_t v75;
    int64_t v83; /* placeholder for void */
    bool v73;
    int64_t v114; /* placeholder for void */
    void* v67;
    bool v101;
    int64_t v26; /* placeholder for void */
    int64_t v3;
    int64_t v69; /* placeholder for void */
    int64_t v57;
    int64_t v29;
    int64_t v127; /* placeholder for void */
    int64_t v17;
    int64_t v60;
    int64_t v16; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v107;
    bool v126;
    bool v88;
    int64_t v70; /* placeholder for void */
    void* v25;
    int64_t v112;
    int64_t v119;
    int64_t v59; /* placeholder for void */
    void* v13;
    int64_t v41;
    int64_t v121;
    int64_t v12; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v45;
    int64_t v72;
    int64_t v71;
    int64_t v125;
    int64_t v87;
    int64_t v94;
    int64_t v56;
    int64_t v47; /* placeholder for void */
    int64_t v36;
    int64_t v89; /* placeholder for void */
    void* v46;
    int64_t v104; /* placeholder for void */
    int64_t v79;
    int64_t v53;
    int64_t v35; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    void* v34;
    int64_t v81; /* placeholder for void */
    int64_t v42;
    int64_t v82;
    int64_t v98; /* placeholder for void */
    int64_t v76;
    int64_t v120;
    int64_t v130; /* placeholder for void */
    int64_t v115;
    int64_t v31; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    int64_t v108;
    int64_t v68;
    void* v10;
    void* v7;
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

void Vec_clear(void* v0, int64_t v1) {
    p_call_stack_push("Vec_clear");
    int64_t v10; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v3;
    void* v4;
    int64_t v11; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v8;
    int64_t v2;
    void* v7;
    int64_t v9; /* placeholder for void */
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

double vec2_dot(void* v0, void* v1, void* v2) {
    p_call_stack_push("vec2_dot");
    void* v3;
    int64_t v14; /* placeholder for void */
    double v7;
    double v6;
    void* v8;
    double v9;
    double v11;
    double v13;
    void* v10;
    void* v5;
    double v4;
    double v12;
    v3 = (char*)v1 + 0;
    v4 = *(double*)v3;
    v5 = (char*)v2 + 0;
    v6 = *(double*)v5;
    v7 = v4 * v6;
    v8 = (char*)v1 + 8;
    v9 = *(double*)v8;
    v10 = (char*)v2 + 8;
    v11 = *(double*)v10;
    v12 = v9 * v11;
    v13 = v7 + v12;
    p_call_stack_pop();
    return v13;
    return 0;
}

int64_t String_repeat(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_repeat");
    int64_t v66;
    int64_t v68; /* placeholder for void */
    bool v6;
    void* v26;
    int64_t v30;
    int64_t v21;
    int64_t v25;
    int64_t v50;
    int64_t v49; /* placeholder for void */
    void* v14;
    int64_t v29;
    int64_t v70; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v24;
    bool v4;
    int64_t v35;
    int64_t v47;
    int64_t v28;
    int64_t v51;
    int64_t v13;
    bool v40;
    void* v36;
    void* v32;
    int64_t v42;
    int64_t v43;
    int64_t v57;
    int64_t v5;
    int64_t v52;
    int64_t v46;
    int64_t v41; /* placeholder for void */
    int64_t v61;
    int64_t v37; /* placeholder for void */
    void* v11;
    void* v62;
    int64_t v34; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v56;
    bool v7;
    int64_t v44;
    int64_t v39;
    int64_t v58;
    int64_t v23; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    void* v59;
    int64_t v18;
    int64_t v27; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    void* v17;
    int64_t v54; /* placeholder for void */
    int64_t v9;
    int64_t v15; /* placeholder for void */
    int64_t v69;
    int64_t v63; /* placeholder for void */
    int64_t v55;
    int64_t v19; /* placeholder for void */
    int64_t v3;
    void* v65;
    int64_t v31;
    int64_t v45;
    int64_t v60; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v10;
    int64_t v53; /* placeholder for void */
    int64_t v48;
    void* v33;
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

double vec4_length(void* v0, void* v1) {
    p_call_stack_push("vec4_length");
    void* v19;
    double v22;
    double v24;
    double v25;
    void* v9;
    double v12;
    double v5;
    double v20;
    int64_t v26; /* placeholder for void */
    void* v2;
    double v23;
    double v6;
    void* v13;
    void* v15;
    double v11;
    double v14;
    double v17;
    double v3;
    void* v21;
    double v10;
    void* v7;
    double v8;
    void* v4;
    double v16;
    double v18;
    v2 = (char*)v1 + 0;
    v3 = *(double*)v2;
    v4 = (char*)v1 + 0;
    v5 = *(double*)v4;
    v6 = v3 * v5;
    v7 = (char*)v1 + 8;
    v8 = *(double*)v7;
    v9 = (char*)v1 + 8;
    v10 = *(double*)v9;
    v11 = v8 * v10;
    v12 = v6 + v11;
    v13 = (char*)v1 + 16;
    v14 = *(double*)v13;
    v15 = (char*)v1 + 16;
    v16 = *(double*)v15;
    v17 = v14 * v16;
    v18 = v12 + v17;
    v19 = (char*)v1 + 24;
    v20 = *(double*)v19;
    v21 = (char*)v1 + 24;
    v22 = *(double*)v21;
    v23 = v20 * v22;
    v24 = v18 + v23;
    v25 = (double)pledge_sqrt(v24);
    p_call_stack_pop();
    return v25;
    return 0;
}

double math_ceil(void* v0, double v1) {
    p_call_stack_push("math_ceil");
    int64_t v3; /* placeholder for void */
    double v2;
    v2 = (double)pledge_ceil(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

double math_pow(void* v0, double v1, double v2) {
    p_call_stack_push("math_pow");
    int64_t v4; /* placeholder for void */
    double v3;
    v3 = (double)pledge_pow(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t String_replace(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("String_replace");
    int64_t v24;
    void* v69;
    uint8_t v116;
    bool v121;
    void* v176;
    int64_t v137; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v33;
    int64_t v35; /* placeholder for void */
    int64_t v71;
    uint8_t v155;
    int64_t v39; /* placeholder for void */
    void* v45;
    int64_t v75;
    int64_t v91;
    void* v106;
    uint8_t v113;
    int64_t v108;
    int64_t v4;
    int64_t v40;
    int64_t v54; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v107; /* placeholder for void */
    int64_t v64;
    int64_t v126; /* placeholder for void */
    int64_t v95;
    bool v85;
    int64_t v156; /* placeholder for void */
    int64_t v191;
    int64_t v175;
    int64_t v128;
    int64_t v58; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v181; /* placeholder for void */
    int64_t v41;
    bool v25;
    void* v72;
    int64_t v130; /* placeholder for void */
    int64_t v165; /* placeholder for void */
    int64_t v96;
    void* v163;
    int64_t v171;
    int64_t v29;
    void* v179;
    void* v34;
    int64_t v147;
    int64_t v74; /* placeholder for void */
    int64_t v135;
    bool v26;
    int64_t v51; /* placeholder for void */
    int64_t v142; /* placeholder for void */
    bool v23;
    int64_t v44;
    int64_t v59;
    int64_t v146; /* placeholder for void */
    void* v46;
    int64_t v81;
    int64_t v76;
    int64_t v7;
    void* v52;
    int64_t v177; /* placeholder for void */
    int64_t v192;
    int64_t v117; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v144;
    int64_t v36;
    bool v66;
    int64_t v139;
    int64_t v180;
    int64_t v190;
    int64_t v168; /* placeholder for void */
    int64_t v56;
    void* v173;
    int64_t v133; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    void* v8;
    int64_t v189; /* placeholder for void */
    int64_t v194; /* placeholder for void */
    int64_t v19;
    int64_t v125; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v99;
    int64_t v10; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    void* v57;
    int64_t v92;
    int64_t v31; /* placeholder for void */
    int64_t v196;
    int64_t v158;
    int64_t v27; /* placeholder for void */
    int64_t v120;
    int64_t v161; /* placeholder for void */
    int64_t v178; /* placeholder for void */
    bool v38;
    int64_t v60;
    void* v93;
    int64_t v98; /* placeholder for void */
    int64_t v141; /* placeholder for void */
    int64_t v48;
    int64_t v14; /* placeholder for void */
    void* v49;
    int64_t v50; /* placeholder for void */
    void* v112;
    int64_t v169; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    void* v160;
    int64_t v150;
    int64_t v187; /* placeholder for void */
    int64_t v162;
    void* v145;
    int64_t v32;
    int64_t v114; /* placeholder for void */
    int64_t v188;
    int64_t v73; /* placeholder for void */
    void* v61;
    int64_t v89;
    bool v77;
    int64_t v129;
    int64_t v90;
    int64_t v79;
    int64_t v53;
    void* v148;
    int64_t v136;
    void* v100;
    int64_t v67; /* placeholder for void */
    int64_t v157; /* placeholder for void */
    bool v5;
    int64_t v183;
    int64_t v88; /* placeholder for void */
    void* v109;
    int64_t v149; /* placeholder for void */
    int64_t v110; /* placeholder for void */
    int64_t v65;
    int64_t v197; /* placeholder for void */
    int64_t v87; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v28;
    int64_t v82;
    int64_t v111; /* placeholder for void */
    void* v151;
    int64_t v55; /* placeholder for void */
    int64_t v152; /* placeholder for void */
    int64_t v16;
    int64_t v42; /* placeholder for void */
    int64_t v80;
    int64_t v122; /* placeholder for void */
    int64_t v140;
    int64_t v124; /* placeholder for void */
    int64_t v182; /* placeholder for void */
    int64_t v185;
    int64_t v105;
    bool v68;
    void* v97;
    int64_t v138;
    int64_t v17; /* placeholder for void */
    int64_t v119;
    bool v132;
    int64_t v94; /* placeholder for void */
    void* v159;
    void* v154;
    int64_t v172;
    void* v115;
    int64_t v70; /* placeholder for void */
    int64_t v184;
    void* v15;
    int64_t v186;
    int64_t v193; /* placeholder for void */
    int64_t v118; /* placeholder for void */
    void* v9;
    int64_t v102;
    int64_t v18; /* placeholder for void */
    int64_t v195; /* placeholder for void */
    int64_t v131; /* placeholder for void */
    bool v123;
    int64_t v170;
    int64_t v143;
    int64_t v167;
    int64_t v174; /* placeholder for void */
    void* v103;
    int64_t v78; /* placeholder for void */
    int64_t v153; /* placeholder for void */
    int64_t v37;
    int64_t v11;
    int64_t v22;
    int64_t v101; /* placeholder for void */
    bool v83;
    int64_t v134;
    int64_t v164; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    void* v12;
    int64_t v127;
    void* v166;
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

void vec3_add(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("vec3_add");
    double v28;
    void* v10;
    double v22;
    void* v14;
    double v6;
    void* v30;
    double v36;
    int64_t v37; /* placeholder for void */
    void* v35;
    int64_t v18; /* placeholder for void */
    double v15;
    double v23;
    void* v19;
    int64_t v33; /* placeholder for void */
    void* v17;
    double v20;
    double v16;
    int64_t v11; /* placeholder for void */
    void* v4;
    void* v12;
    double v32;
    int64_t v25; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    void* v31;
    int64_t v29; /* placeholder for void */
    void* v5;
    double v13;
    void* v27;
    double v9;
    void* v24;
    void* v34;
    void* v7;
    void* v21;
    double v8;
    void* v26;
    v4 = malloc(24);
    v5 = (char*)v2 + 0;
    v6 = *(double*)v5;
    v7 = (char*)v3 + 0;
    v8 = *(double*)v7;
    v9 = v6 + v8;
    v10 = (char*)v4 + 0;
    *(double*)v10 = v9;
    v12 = (char*)v2 + 8;
    v13 = *(double*)v12;
    v14 = (char*)v3 + 8;
    v15 = *(double*)v14;
    v16 = v13 + v15;
    v17 = (char*)v4 + 8;
    *(double*)v17 = v16;
    v19 = (char*)v2 + 16;
    v20 = *(double*)v19;
    v21 = (char*)v3 + 16;
    v22 = *(double*)v21;
    v23 = v20 + v22;
    v24 = (char*)v4 + 16;
    *(double*)v24 = v23;
    v26 = (char*)v4 + 0;
    v27 = (char*)v0 + 0;
    v28 = *(double*)v26;
    *(double*)v27 = v28;
    v30 = (char*)v4 + 8;
    v31 = (char*)v0 + 8;
    v32 = *(double*)v30;
    *(double*)v31 = v32;
    v34 = (char*)v4 + 16;
    v35 = (char*)v0 + 16;
    v36 = *(double*)v34;
    *(double*)v35 = v36;
    p_call_stack_pop();
    return;
    return;
}

bool Option_is_some(void* v0, void* v1) {
    p_call_stack_push("Option_is_some");
    int64_t v18; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v12;
    int64_t v3;
    int64_t v13;
    int64_t v6; /* placeholder for void */
    bool v19;
    int64_t v20; /* placeholder for void */
    bool v5;
    int64_t v17; /* placeholder for void */
    bool v9;
    void* v11;
    bool v14;
    void* v7;
    int64_t v8;
    int64_t v10; /* placeholder for void */
    bool v16;
    int64_t v4;
    void* v2;
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

void Result_err(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_err");
    int64_t v29;
    int64_t v42;
    void* v28;
    void* v15;
    int64_t v9;
    int64_t v25;
    void* v8;
    void* v10;
    bool v26;
    int64_t v31;
    void* v40;
    int64_t v13; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    void* v30;
    int64_t v27; /* placeholder for void */
    void* v32;
    int64_t v38;
    int64_t v22; /* placeholder for void */
    int64_t v11;
    void* v36;
    void* v23;
    void* v18;
    int64_t v16;
    int64_t v21; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    void* v3;
    int64_t v4;
    bool v6;
    void* v19;
    int64_t v24;
    int64_t v7; /* placeholder for void */
    void* v34;
    int64_t v35; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    void* v41;
    int64_t v43; /* placeholder for void */
    void* v14;
    int64_t v20;
    void* v37;
    void* v12;
    int64_t v5;
    int64_t v17; /* placeholder for void */
    int64_t v46; /* placeholder for void */
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

void ByteBuffer_clear(void* v0, void* v1) {
    p_call_stack_push("ByteBuffer_clear");
    int64_t v8; /* placeholder for void */
    void* v3;
    int64_t v4;
    void* v2;
    int64_t v5; /* placeholder for void */
    int64_t v7;
    int64_t v9; /* placeholder for void */
    void* v6;
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

// ─── C entry point ───
int main(int argc, char** argv) {
    __p_argc = argc; __p_argv = argv;
    // Register functions for p_thread_spawn lookup
    p_thread_register("ByteBuffer_to_str", (int64_t(*)())ByteBuffer_to_str);
    p_thread_register("math_pi", (int64_t(*)())math_pi);
    p_thread_register("String_contains", (int64_t(*)())String_contains);
    p_thread_register("allocator_alloc", (int64_t(*)())allocator_alloc);
    p_thread_register("str_slice", (int64_t(*)())str_slice);
    p_thread_register("Result_unwrap_or_else", (int64_t(*)())Result_unwrap_or_else);
    p_thread_register("String_len", (int64_t(*)())String_len);
    p_thread_register("String_push_str", (int64_t(*)())String_push_str);
    p_thread_register("str_is_empty", (int64_t(*)())str_is_empty);
    p_thread_register("ByteBuffer_write_byte", (int64_t(*)())ByteBuffer_write_byte);
    p_thread_register("char_to_str", (int64_t(*)())char_to_str);
    p_thread_register("ByteBuffer_set_byte", (int64_t(*)())ByteBuffer_set_byte);
    p_thread_register("str_parse_hex", (int64_t(*)())str_parse_hex);
    p_thread_register("HashMap_insert", (int64_t(*)())HashMap_insert);
    p_thread_register("vec3_cross", (int64_t(*)())vec3_cross);
    p_thread_register("quat", (int64_t(*)())quat);
    p_thread_register("HashMap_get_or", (int64_t(*)())HashMap_get_or);
    p_thread_register("str_to_upper", (int64_t(*)())str_to_upper);
    p_thread_register("Vec_capacity", (int64_t(*)())Vec_capacity);
    p_thread_register("vec2_add", (int64_t(*)())vec2_add);
    p_thread_register("abs", (int64_t(*)())p_abs);
    p_thread_register("String_to_uppercase", (int64_t(*)())String_to_uppercase);
    p_thread_register("String_split", (int64_t(*)())String_split);
    p_thread_register("Option_map", (int64_t(*)())Option_map);
    p_thread_register("ByteBuffer_new", (int64_t(*)())ByteBuffer_new);
    p_thread_register("HashMap_len", (int64_t(*)())HashMap_len);
    p_thread_register("vec4_xyz", (int64_t(*)())vec4_xyz);
    p_thread_register("vec4_add", (int64_t(*)())vec4_add);
    p_thread_register("StringBuilder_append_int", (int64_t(*)())StringBuilder_append_int);
    p_thread_register("ByteBuffer_read_byte", (int64_t(*)())ByteBuffer_read_byte);
    p_thread_register("char_is_lower", (int64_t(*)())char_is_lower);
    p_thread_register("StringBuilder_new", (int64_t(*)())StringBuilder_new);
    p_thread_register("vec4_dot", (int64_t(*)())vec4_dot);
    p_thread_register("HashMap_clear", (int64_t(*)())HashMap_clear);
    p_thread_register("vec3_dot", (int64_t(*)())vec3_dot);
    p_thread_register("vec3_normalize", (int64_t(*)())vec3_normalize);
    p_thread_register("clamp", (int64_t(*)())clamp);
    p_thread_register("String_to_lowercase", (int64_t(*)())String_to_lowercase);
    p_thread_register("Vec_set", (int64_t(*)())Vec_set);
    p_thread_register("min", (int64_t(*)())p_min);
    p_thread_register("Vec_contains", (int64_t(*)())Vec_contains);
    p_thread_register("String_ends_with", (int64_t(*)())String_ends_with);
    p_thread_register("math_abs_i64", (int64_t(*)())math_abs_i64);
    p_thread_register("str_bytes", (int64_t(*)())str_bytes);
    p_thread_register("String_from", (int64_t(*)())String_from);
    p_thread_register("p_str_length", (int64_t(*)())p_str_length);
    p_thread_register("str_from_bytes", (int64_t(*)())str_from_bytes);
    p_thread_register("vec2_scale", (int64_t(*)())vec2_scale);
    p_thread_register("vec3_sub", (int64_t(*)())vec3_sub);
    p_thread_register("vec2_length", (int64_t(*)())vec2_length);
    p_thread_register("create_allocator", (int64_t(*)())create_allocator);
    p_thread_register("vec3_scale", (int64_t(*)())vec3_scale);
    p_thread_register("lerp", (int64_t(*)())lerp);
    p_thread_register("HashMap_grow", (int64_t(*)())HashMap_grow);
    p_thread_register("HashMap_to_str", (int64_t(*)())HashMap_to_str);
    p_thread_register("Result_is_ok", (int64_t(*)())Result_is_ok);
    p_thread_register("mat4_mul", (int64_t(*)())mat4_mul);
    p_thread_register("Vec_push", (int64_t(*)())Vec_push);
    p_thread_register("String_bytes", (int64_t(*)())String_bytes);
    p_thread_register("quat_rotate_vec3", (int64_t(*)())quat_rotate_vec3);
    p_thread_register("max", (int64_t(*)())p_max);
    p_thread_register("int_to_str", (int64_t(*)())int_to_str);
    p_thread_register("quat_from_axis_angle", (int64_t(*)())quat_from_axis_angle);
    p_thread_register("Option_unwrap_or_else", (int64_t(*)())Option_unwrap_or_else);
    p_thread_register("Vec_truncate", (int64_t(*)())Vec_truncate);
    p_thread_register("radians", (int64_t(*)())radians);
    p_thread_register("String_find", (int64_t(*)())String_find);
    p_thread_register("allocator_free", (int64_t(*)())allocator_free);
    p_thread_register("Vec_last", (int64_t(*)())Vec_last);
    p_thread_register("Result_unwrap", (int64_t(*)())Result_unwrap);
    p_thread_register("Result_map", (int64_t(*)())Result_map);
    p_thread_register("String_char_at", (int64_t(*)())String_char_at);
    p_thread_register("str_trim_end", (int64_t(*)())str_trim_end);
    p_thread_register("Result_expect", (int64_t(*)())Result_expect);
    p_thread_register("Result_map_err", (int64_t(*)())Result_map_err);
    p_thread_register("math_sin", (int64_t(*)())math_sin);
    p_thread_register("byte_to_str", (int64_t(*)())byte_to_str);
    p_thread_register("vec4_scale", (int64_t(*)())vec4_scale);
    p_thread_register("Option_unwrap", (int64_t(*)())Option_unwrap);
    p_thread_register("p_str_hash", (int64_t(*)())p_str_hash);
    p_thread_register("str_byte_at", (int64_t(*)())str_byte_at);
    p_thread_register("str_split_char", (int64_t(*)())str_split_char);
    p_thread_register("mat4_transform_vec4", (int64_t(*)())mat4_transform_vec4);
    p_thread_register("String_rfind", (int64_t(*)())String_rfind);
    p_thread_register("Vec_get", (int64_t(*)())Vec_get);
    p_thread_register("str_starts_with", (int64_t(*)())str_starts_with);
    p_thread_register("HashMap_contains", (int64_t(*)())HashMap_contains);
    p_thread_register("math_fabs", (int64_t(*)())math_fabs);
    p_thread_register("math_e", (int64_t(*)())math_e);
    p_thread_register("Vec_reverse", (int64_t(*)())Vec_reverse);
    p_thread_register("StringBuilder_append_char", (int64_t(*)())StringBuilder_append_char);
    p_thread_register("str_concat", (int64_t(*)())str_concat);
    p_thread_register("char_to_lower", (int64_t(*)())char_to_lower);
    p_thread_register("HashMap_values", (int64_t(*)())HashMap_values);
    p_thread_register("HashMap_get", (int64_t(*)())HashMap_get);
    p_thread_register("StringBuilder_len", (int64_t(*)())StringBuilder_len);
    p_thread_register("println", (int64_t(*)())println);
    p_thread_register("p_str_copy_n", (int64_t(*)())p_str_copy_n);
    p_thread_register("math_floor", (int64_t(*)())math_floor);
    p_thread_register("str_find", (int64_t(*)())str_find);
    p_thread_register("HashMap_is_empty", (int64_t(*)())HashMap_is_empty);
    p_thread_register("str_trim_start", (int64_t(*)())str_trim_start);
    p_thread_register("vec2_normalize", (int64_t(*)())vec2_normalize);
    p_thread_register("vec2_sub", (int64_t(*)())vec2_sub);
    p_thread_register("vec4_lerp", (int64_t(*)())vec4_lerp);
    p_thread_register("mat4_transform_vec3", (int64_t(*)())mat4_transform_vec3);
    p_thread_register("String_trim", (int64_t(*)())String_trim);
    p_thread_register("Option_is_none", (int64_t(*)())Option_is_none);
    p_thread_register("ByteBuffer_from_string", (int64_t(*)())ByteBuffer_from_string);
    p_thread_register("ByteBuffer_is_empty", (int64_t(*)())ByteBuffer_is_empty);
    p_thread_register("vec3_lerp", (int64_t(*)())vec3_lerp);
    p_thread_register("math_sqrt", (int64_t(*)())math_sqrt);
    p_thread_register("str_split", (int64_t(*)())str_split);
    p_thread_register("Vec_to_str", (int64_t(*)())Vec_to_str);
    p_thread_register("str_to_lower", (int64_t(*)())str_to_lower);
    p_thread_register("str_repeat", (int64_t(*)())str_repeat);
    p_thread_register("assert", (int64_t(*)())assert);
    p_thread_register("math_max_f64", (int64_t(*)())math_max_f64);
    p_thread_register("Vec_clone", (int64_t(*)())Vec_clone);
    p_thread_register("String_to_string", (int64_t(*)())String_to_string);
    p_thread_register("String_byte_at", (int64_t(*)())String_byte_at);
    p_thread_register("str_equal", (int64_t(*)())str_equal);
    p_thread_register("vec4_normalize", (int64_t(*)())vec4_normalize);
    p_thread_register("math_exp", (int64_t(*)())math_exp);
    p_thread_register("mat4_identity", (int64_t(*)())mat4_identity);
    p_thread_register("Option_unwrap_or", (int64_t(*)())Option_unwrap_or);
    p_thread_register("Result_is_err", (int64_t(*)())Result_is_err);
    p_thread_register("HashMap_keys", (int64_t(*)())HashMap_keys);
    p_thread_register("StringBuilder_to_str", (int64_t(*)())StringBuilder_to_str);
    p_thread_register("str_ends_with", (int64_t(*)())str_ends_with);
    p_thread_register("StringBuilder_append_str", (int64_t(*)())StringBuilder_append_str);
    p_thread_register("vec3_length", (int64_t(*)())vec3_length);
    p_thread_register("Vec_insert", (int64_t(*)())Vec_insert);
    p_thread_register("Vec_first", (int64_t(*)())Vec_first);
    p_thread_register("allocator_bytes_used", (int64_t(*)())allocator_bytes_used);
    p_thread_register("char_is_alpha", (int64_t(*)())char_is_alpha);
    p_thread_register("HashMap_remove", (int64_t(*)())HashMap_remove);
    p_thread_register("char_is_upper", (int64_t(*)())char_is_upper);
    p_thread_register("Option_and_then", (int64_t(*)())Option_and_then);
    p_thread_register("char_is_digit", (int64_t(*)())char_is_digit);
    p_thread_register("str_parse_int_range", (int64_t(*)())str_parse_int_range);
    p_thread_register("degrees", (int64_t(*)())degrees);
    p_thread_register("math_cos", (int64_t(*)())math_cos);
    p_thread_register("String_is_empty", (int64_t(*)())String_is_empty);
    p_thread_register("p_str_equal", (int64_t(*)())p_str_equal);
    p_thread_register("String_parse_int", (int64_t(*)())String_parse_int);
    p_thread_register("str_len", (int64_t(*)())str_len);
    p_thread_register("str_contains", (int64_t(*)())str_contains);
    p_thread_register("str_trim", (int64_t(*)())str_trim);
    p_thread_register("vec2", (int64_t(*)())vec2);
    p_thread_register("Result_and_then", (int64_t(*)())Result_and_then);
    p_thread_register("Vec_sort", (int64_t(*)())Vec_sort);
    p_thread_register("Vec_extend", (int64_t(*)())Vec_extend);
    p_thread_register("vec4_sub", (int64_t(*)())vec4_sub);
    p_thread_register("StringBuilder_clear", (int64_t(*)())StringBuilder_clear);
    p_thread_register("vec3", (int64_t(*)())vec3);
    p_thread_register("Option_expect", (int64_t(*)())Option_expect);
    p_thread_register("ByteBuffer_len", (int64_t(*)())ByteBuffer_len);
    p_thread_register("String_clone", (int64_t(*)())String_clone);
    p_thread_register("StringBuilder_is_empty", (int64_t(*)())StringBuilder_is_empty);
    p_thread_register("p_str_from_ptr", (int64_t(*)())p_str_from_ptr);
    p_thread_register("Vec_pop", (int64_t(*)())Vec_pop);
    p_thread_register("math_min_f64", (int64_t(*)())math_min_f64);
    p_thread_register("ByteBuffer_write_bytes", (int64_t(*)())ByteBuffer_write_bytes);
    p_thread_register("HashMap_new", (int64_t(*)())HashMap_new);
    p_thread_register("str_replace", (int64_t(*)())str_replace);
    p_thread_register("str_parse_int", (int64_t(*)())str_parse_int);
    p_thread_register("distance", (int64_t(*)())distance);
    p_thread_register("Vec_new", (int64_t(*)())Vec_new);
    p_thread_register("math_log", (int64_t(*)())math_log);
    p_thread_register("Result_unwrap_or", (int64_t(*)())Result_unwrap_or);
    p_thread_register("HashMap_clone", (int64_t(*)())HashMap_clone);
    p_thread_register("vec2_lerp", (int64_t(*)())vec2_lerp);
    p_thread_register("Vec_remove", (int64_t(*)())Vec_remove);
    p_thread_register("p_str_compare", (int64_t(*)())p_str_compare);
    p_thread_register("Vec_len", (int64_t(*)())Vec_len);
    p_thread_register("char_is_whitespace", (int64_t(*)())char_is_whitespace);
    p_thread_register("char_to_upper", (int64_t(*)())char_to_upper);
    p_thread_register("math_clamp_f64", (int64_t(*)())math_clamp_f64);
    p_thread_register("String_slice", (int64_t(*)())String_slice);
    p_thread_register("str_rfind", (int64_t(*)())str_rfind);
    p_thread_register("Option_or_else", (int64_t(*)())Option_or_else);
    p_thread_register("String_char_count", (int64_t(*)())String_char_count);
    p_thread_register("translate", (int64_t(*)())translate);
    p_thread_register("vec4", (int64_t(*)())vec4);
    p_thread_register("mat_scale", (int64_t(*)())mat_scale);
    p_thread_register("Vec_is_empty", (int64_t(*)())Vec_is_empty);
    p_thread_register("String_starts_with", (int64_t(*)())String_starts_with);
    p_thread_register("Result_ok", (int64_t(*)())Result_ok);
    p_thread_register("str_compare", (int64_t(*)())str_compare);
    p_thread_register("HashMap_find_slot", (int64_t(*)())HashMap_find_slot);
    p_thread_register("Vec_clear", (int64_t(*)())Vec_clear);
    p_thread_register("vec2_dot", (int64_t(*)())vec2_dot);
    p_thread_register("String_repeat", (int64_t(*)())String_repeat);
    p_thread_register("vec4_length", (int64_t(*)())vec4_length);
    p_thread_register("math_ceil", (int64_t(*)())math_ceil);
    p_thread_register("math_pow", (int64_t(*)())math_pow);
    p_thread_register("String_replace", (int64_t(*)())String_replace);
    p_thread_register("vec3_add", (int64_t(*)())vec3_add);
    p_thread_register("Option_is_some", (int64_t(*)())Option_is_some);
    p_thread_register("Result_err", (int64_t(*)())Result_err);
    p_thread_register("ByteBuffer_clear", (int64_t(*)())ByteBuffer_clear);
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
