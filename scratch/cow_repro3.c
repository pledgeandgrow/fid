// Auto-generated C code from Fides IR
// IEEE arithmetic exactly as written: no fused multiply-add contraction.
#if defined(__clang__)
#pragma STDC FP_CONTRACT OFF
#elif defined(__GNUC__)
#pragma GCC optimize ("fp-contract=off")
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <alloca.h>
#else
#include <malloc.h>
#define alloca _alloca
#endif
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#define _Thread_local __declspec(thread)
#endif
#include <math.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif
#include <io.h>
#include <fcntl.h>
// PLINK: ws2_32
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
typedef int SOCKET;
#include <unistd.h>
#include <fcntl.h>
#endif
#include <sys/stat.h>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <process.h>
#else
#include <dirent.h>
#include <sys/wait.h>
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

typedef struct { double* data; int64_t len; int64_t ndim; int64_t shape[8]; } PTensor;

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
int64_t p_volatile_load(int64_t addr);
void p_volatile_store(int64_t addr, int64_t val);
int64_t p_volatile_load8(int64_t addr);
int64_t p_volatile_load16(int64_t addr);
int64_t p_volatile_load32(int64_t addr);
void p_volatile_store8(int64_t addr, int64_t val);
void p_volatile_store16(int64_t addr, int64_t val);
void p_volatile_store32(int64_t addr, int64_t val);
void p_lgdt(int64_t gdt_desc);
void p_gdt_set_entry(int64_t gdt_ptr, int64_t index, int64_t base, int64_t limit, int64_t flags);
void p_serial_init(void);
void p_serial_write(int64_t c);
int64_t p_serial_read(void);
int64_t p_serial_ready(void);
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
void* p_grad_get(void* t);
void p_grad_zero(void);
void p_autograd_begin(void);
void p_autograd_end(void);
void p_autograd_backward(void* loss);
void p_checkpoint_begin(void);
void p_checkpoint_end(void);
void* p_dp_noise(void* t, double sigma);
void* p_gaussian_noise(void* shape_vec, double sigma);
void* p_dropout(void* t, double drop_prob);
void p_dp_budget_reset(double epsilon);
void p_dp_budget_check(double epsilon, double limit);
int64_t p_tp_rank(void);
int64_t p_tp_world_size(void);
void* p_all_reduce(void* t);
void* p_broadcast(void* t);
void* p_all_gather(void* t);
void* p_scatter(void* t);
void p_send_tensor(void* t);
void* p_recv_tensor(void* shape_vec);
int64_t p_kv_cache_new(int64_t num_layers, int64_t max_seq_len, int64_t window_size, int64_t head_dim);
void p_kv_cache_append(int64_t cache_id, int64_t layer, void* k, void* v);
void* p_kv_cache_get(int64_t cache_id, int64_t layer);
int64_t p_kv_cache_len(int64_t cache_id, int64_t layer);
void p_kv_cache_clear(int64_t cache_id);
void p_kv_cache_evict(int64_t cache_id, int64_t num_tokens);
void* p_quantize_q8(void* t, double scale, int64_t zero_point);
void* p_dequantize_q8(void* t, double scale, int64_t zero_point);
void* p_qmatmul(void* a, void* b);
char* p_find_quant_params(void* t);
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
int64_t p_f64_to_bits(double val);
int64_t p_f32_to_bits(float val);
double p_f64_from_bits(int64_t bits);
float p_f32_from_bits(int64_t bits);
double p_int_to_float(int64_t val);
int64_t p_float_to_int(double val);
int64_t p_str_to_int(const char* s);
double p_str_to_float(const char* s);
char* p_bool_to_str(bool val);
char* p_vec_to_str(PVec* v);
char* p_str_concat(const char* a, const char* b);
char* p_str_join_n(PVec* parts);
void* p_sb_new(void);
void p_sb_append(void* sb, const char* s);
const char* p_sb_to_str(void* sb);
void p_print(int64_t val);
void p_print_str(const char* s);
void p_print_int(int64_t val);
void p_print_bool(bool val);
void p_print_float(double val);
void p_print_newline(void);
void p_eprint_str(const char* s);
void p_flush_stdout(void);
int64_t p_stdin_eof(void);
char* p_read_line(void);
char* p_read_stdin(int64_t n);
void p_panic(const char* msg);
#define P_OVF_ADD "Integer overflow on addition"
#define P_OVF_SUB "Integer overflow on subtraction"
#define P_OVF_MUL "Integer overflow on multiplication"
#if (defined(__GNUC__) || defined(__clang__)) && !defined(__TINYC__)
#define P_ARITH(T, N) \
static inline T p_add_##N(T a, T b) { T r; if (__builtin_add_overflow(a, b, &r)) p_panic(P_OVF_ADD); return r; } \
static inline T p_sub_##N(T a, T b) { T r; if (__builtin_sub_overflow(a, b, &r)) p_panic(P_OVF_SUB); return r; } \
static inline T p_mul_##N(T a, T b) { T r; if (__builtin_mul_overflow(a, b, &r)) p_panic(P_OVF_MUL); return r; }
P_ARITH(int8_t, i8) P_ARITH(int16_t, i16) P_ARITH(int32_t, i32) P_ARITH(int64_t, i64)
P_ARITH(uint8_t, u8) P_ARITH(uint16_t, u16) P_ARITH(uint32_t, u32) P_ARITH(uint64_t, u64)
#else
#define P_NARROW(T, N, LO, HI) \
static inline T p_add_##N(T a, T b) { int64_t r = (int64_t)a + b; if (r < (LO) || r > (HI)) p_panic(P_OVF_ADD); return (T)r; } \
static inline T p_sub_##N(T a, T b) { int64_t r = (int64_t)a - b; if (r < (LO) || r > (HI)) p_panic(P_OVF_SUB); return (T)r; }
#define P_NARROW_MUL(T, N, LO, HI) \
static inline T p_mul_##N(T a, T b) { int64_t r = (int64_t)a * b; if (r < (LO) || r > (HI)) p_panic(P_OVF_MUL); return (T)r; }
P_NARROW(int8_t, i8, -128, 127) P_NARROW_MUL(int8_t, i8, -128, 127)
P_NARROW(int16_t, i16, -32768, 32767) P_NARROW_MUL(int16_t, i16, -32768, 32767)
P_NARROW(int32_t, i32, -2147483648LL, 2147483647LL) P_NARROW_MUL(int32_t, i32, -2147483648LL, 2147483647LL)
P_NARROW(uint8_t, u8, 0, 255) P_NARROW_MUL(uint8_t, u8, 0, 255)
P_NARROW(uint16_t, u16, 0, 65535) P_NARROW_MUL(uint16_t, u16, 0, 65535)
P_NARROW(uint32_t, u32, 0, 4294967295LL)
#define P_WIDE \
static inline uint32_t p_mul_u32(uint32_t a, uint32_t b) { uint64_t r = (uint64_t)a * b; if (r > 4294967295ULL) p_panic(P_OVF_MUL); return (uint32_t)r; } \
static inline int64_t p_add_i64(int64_t a, int64_t b) { int64_t r = (int64_t)((uint64_t)a + (uint64_t)b); if (((a ^ r) & (b ^ r)) < 0) p_panic(P_OVF_ADD); return r; } \
static inline int64_t p_sub_i64(int64_t a, int64_t b) { int64_t r = (int64_t)((uint64_t)a - (uint64_t)b); if (((a ^ b) & (a ^ r)) < 0) p_panic(P_OVF_SUB); return r; } \
static inline int64_t p_mul_i64(int64_t a, int64_t b) { int64_t r = (int64_t)((uint64_t)a * (uint64_t)b); if (a != 0 && ((a == -1 && b == INT64_MIN) || r / a != b)) p_panic(P_OVF_MUL); return r; } \
static inline uint64_t p_add_u64(uint64_t a, uint64_t b) { uint64_t r = a + b; if (r < a) p_panic(P_OVF_ADD); return r; } \
static inline uint64_t p_sub_u64(uint64_t a, uint64_t b) { if (b > a) p_panic(P_OVF_SUB); return a - b; } \
static inline uint64_t p_mul_u64(uint64_t a, uint64_t b) { uint64_t r = a * b; if (a != 0 && r / a != b) p_panic(P_OVF_MUL); return r; }
P_WIDE
#endif
#define P_DIVS(T, N) \
static inline T p_div_##N(T a, T b) { if (b == 0) p_panic("Division by zero"); if (b == -1) { if (a == (T)((uint64_t)1 << (sizeof(T) * 8 - 1))) p_panic("Integer overflow on division"); return (T)(0 - (uint64_t)a); } return (T)(a / b); } \
static inline T p_wdiv_##N(T a, T b) { if (b == 0) p_panic("Division by zero"); if (b == -1) return (T)(0 - (uint64_t)a); return (T)(a / b); } \
static inline T p_mod_##N(T a, T b) { if (b == 0) p_panic("Modulo by zero"); if (b == -1) return 0; return (T)(a % b); }
#define P_DIVU(T, N) \
static inline T p_div_##N(T a, T b) { if (b == 0) p_panic("Division by zero"); return (T)(a / b); } \
static inline T p_wdiv_##N(T a, T b) { return p_div_##N(a, b); } \
static inline T p_mod_##N(T a, T b) { if (b == 0) p_panic("Modulo by zero"); return (T)(a % b); }
P_DIVS(int8_t, i8) P_DIVS(int16_t, i16) P_DIVS(int32_t, i32) P_DIVS(int64_t, i64)
P_DIVU(uint8_t, u8) P_DIVU(uint16_t, u16) P_DIVU(uint32_t, u32) P_DIVU(uint64_t, u64)
// Per-thread runtime state (arena, tasks): no locks needed.
#if defined(_MSC_VER)
#define P_THREAD_LOCAL __declspec(thread)
#elif defined(__TINYC__)
#define P_THREAD_LOCAL /* TCC has no thread-local storage */
#else
#define P_THREAD_LOCAL __thread
#endif
void p_set_max_recursion(int limit) { (void)limit; }
void* p_malloc(int64_t size);
void* p_alloc(int64_t size);
void p_free(void* ptr);
#if (defined(__GNUC__) || defined(__clang__)) && !defined(__TINYC__)
#define P_NOINLINE __attribute__((noinline))
#define P_UNLIKELY(x) __builtin_expect(!!(x), 0)
#elif defined(_MSC_VER)
#define P_NOINLINE __declspec(noinline)
#define P_UNLIKELY(x) (x)
#else
#define P_NOINLINE
#define P_UNLIKELY(x) (x)
#endif
/* A full Vec grows out of line, so push inlines to a store. */
P_NOINLINE void __p_vec_grow(int64_t* h) {
    int64_t cap = h[2], shared = cap < 0;
    if (shared) cap = -cap - 1;
    int64_t nc = h[1] < cap ? cap : (cap > 0 ? cap * 2 : 8);
    int64_t* nd = (int64_t*)p_malloc(nc * 8);
    int64_t* od = (int64_t*)(intptr_t)h[0];
    if (od) { memcpy(nd, od, (size_t)h[1] * 8); if (!shared) p_free(od); }
    h[0] = (int64_t)(intptr_t)nd;
    h[2] = nc;
}
/* Element access a counted loop proved in bounds (pe.rs unchecked_ops). */
#define P_VEC_UNCHECKED \
static inline int64_t __p_vec_get_u(void* e, int64_t v, int64_t i) { (void)e; return ((int64_t*)(intptr_t)((int64_t*)(intptr_t)v)[0])[i]; } \
static inline void __p_vec_set_u(void* e, int64_t v, int64_t i, int64_t x) { (void)e; int64_t* __hh = (int64_t*)(intptr_t)v; if (P_UNLIKELY(__hh[2] < 0)) __p_vec_grow(__hh); ((int64_t*)(intptr_t)__hh[0])[i] = x; } \
static inline void* __p_pvec_get_u(PVec* v, int64_t i) { return v->data[i]; } \
static inline void __p_pvec_set_u(PVec* v, int64_t i, void* x) { v->data[i] = x; }
P_VEC_UNCHECKED
int64_t p_arena_save(void);
void p_arena_restore(int64_t mark);
void p_region_free(void* region);
void p_memset(void* ptr, int val, int64_t size);
void p_str_zeroize(void* s);
void p_vec_zeroize(void* v);
char* p_file_read(const char* path);
int64_t p_file_write(const char* path, const char* content);
int64_t p_file_write_bin(const char* path, const char* data, int64_t len);
int64_t p_file_write_vec(const char* path, PVec* v);
PVec* p_file_read_bytes(const char* path);
bool p_file_exists(const char* path);
int64_t p_file_append(const char* path, const char* content);
int64_t p_file_delete(const char* path);
int64_t p_file_size(const char* path);
int64_t p_file_hash(const char* path);
char* p_getenv(const char* name);
static int __p_argc = 0;
static char** __p_argv = NULL;
int64_t p_argc(void);
char* p_argv(int64_t index);
int64_t p_system(const char* cmd);
int64_t p_process_run(const char* cmd);
char* p_process_stdout(int64_t h);
char* p_process_stderr(int64_t h);
int64_t p_process_code(int64_t h);
void p_process_close(int64_t h);
int64_t p_getpid(void);
void p_exit(int64_t code);
char* p_dir_list(const char* path);
bool p_is_dir(const char* path);
int64_t p_mkdir(const char* path);
int64_t p_rmdir(const char* path);
int64_t p_rename(const char* a, const char* b);
int64_t p_clock_now(void);
int64_t p_clock_now_us(void);
int64_t p_clock_now_ms(void);
int64_t p_os_random_u64(void);
bool p_regex_match(const char* s, const char* pattern);
const char* p_intern_str(const char* s);
int64_t p_future_ready(int64_t value);
int64_t p_future_poll(int64_t future);
int64_t p_thread_spawn(const char* func_name, int64_t num_args, ...);
int64_t p_executor_new(void);
int64_t p_co_go(const char* name, int64_t arg);
void p_co_yield(void);
int64_t p_co_run(void);
int64_t p_co_done(int64_t t);
int64_t p_co_result(int64_t t);
int64_t p_co_async(const char* name, int64_t nargs, ...);
int64_t p_co_spawn(const char* name, int64_t nargs, ...);
void p_co_tick(void);
int64_t p_co_pool_call(const char* name, int64_t nargs, const int64_t* argv);
int64_t p_co_sleep(int64_t ms);
int64_t p_co_wait_fd(int64_t fd, int64_t write, int64_t timeout_ms);
void p_co_detach(int64_t t);
int64_t p_co_accept(int64_t fd);
int64_t p_co_connect(const char* host, int64_t port);
int64_t p_co_recv(int64_t fd, int64_t buf, int64_t len, int64_t timeout_ms);
int64_t p_co_send(int64_t fd, int64_t buf, int64_t len);
int64_t p_executor_spawn(int64_t exec, const char* func_name, int64_t num_args, ...);
int64_t p_executor_block_on(int64_t exec, int64_t future);
int64_t p_task_poll(int64_t future);
bool p_task_is_ready(int64_t future);
int64_t p_executor_run_all(int64_t exec);
int64_t p_executor_cancel(int64_t exec);
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
int64_t p_tls_create(void);
int64_t p_tls_get(int64_t key);
void p_tls_set(int64_t key, int64_t value);
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
int64_t p_socket_nonblock(int64_t fd, int64_t on);
int64_t p_socket_poll(int64_t fd, int64_t timeout_ms, int64_t write_mode);
int64_t p_signal_register(int64_t sig);
int64_t p_signal_check(int64_t sig);
int64_t p_signal_raise(int64_t sig);
int64_t p_mmap_anon(int64_t len);
int64_t p_mmap_file(const char* path);
int64_t p_mmap_len(int64_t addr);
int64_t p_munmap(int64_t addr);
int64_t p_msync(int64_t addr);
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
static const char _str1[] = "{";
static const char _str2[] = "}";
static const char _str3[] = ", ";
static const char _str4[] = ": ";
static const char _str5[] = "called unwrap on None";
static const char _str6[] = "called unwrap on Err";
static const char _str7[] = "Vec is empty";
static const char _str8[] = "Vec index out of bounds";
static const char _str9[] = "[";
static const char _str10[] = "]";
static const char _str11[] = "Assertion failed";
static const char _str12[] = "HashMap: this integer key is reserved by the map's layout";
static const char _str13[] = "\n";
static const char _str14[] = "Vec insert index out of bounds";
static const char _str15[] = "Vec remove index out of bounds";

// ─── Struct definitions ───
struct ByteBuffer_s {
    void* field_0;
    int64_t field_1;
};

struct Expr_s {
    int64_t field_0;
    int64_t field_1;
};

struct IfExpr_s {
    int64_t field_0[2];
    void* field_1;
    void* field_2;
};

struct Option_s {
    int64_t field_0;
    int64_t field_1;
};

struct ParseResultIfExpr_s {
    int64_t field_0;
    int64_t field_1;
    int64_t field_2;
    int64_t field_3;
    int64_t field_4;
};

struct Result_s {
    int64_t field_0;
    int64_t field_1;
};

struct StringBuilder_s {
    int64_t field_0;
    int64_t field_1;
};

// ─── Function prototypes ───
void p_thread_register(const char* name, int64_t (*fn)());
void ByteBuffer_clear(void* v0, void* v1);
void ByteBuffer_from_string(void* v0, void* v1, void* v2);
bool ByteBuffer_is_empty(void* v0, void* v1);
int64_t ByteBuffer_len(void* v0, void* v1);
void ByteBuffer_new(void* v0, void* v1, int64_t v2);
int64_t ByteBuffer_read_byte(void* v0, void* v1, int64_t v2);
void ByteBuffer_set_byte(void* v0, void* v1, int64_t v2, int64_t v3);
void* ByteBuffer_to_str(void* v0, void* v1);
void ByteBuffer_write_byte(void* v0, void* v1, int64_t v2);
void ByteBuffer_write_bytes(void* v0, void* v1, int64_t v2, int64_t v3);
void HashMap_clear(void* v0, int64_t v1);
int64_t HashMap_clone(void* v0, int64_t v1);
bool HashMap_contains(void* v0, int64_t v1, int64_t v2);
bool HashMap_contains_i(void* v0, int64_t v1, int64_t v2);
void HashMap_drop(void* v0, int64_t v1);
int64_t HashMap_find_slot(void* v0, int64_t v1, int64_t v2);
int64_t HashMap_get(void* v0, int64_t v1, int64_t v2);
int64_t HashMap_get_i(void* v0, int64_t v1, int64_t v2);
int64_t HashMap_get_or(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t HashMap_get_or_i(void* v0, int64_t v1, int64_t v2, int64_t v3);
void HashMap_grow(void* v0, int64_t v1);
void HashMap_insert(void* v0, int64_t v1, int64_t v2, int64_t v3);
void HashMap_insert_i(void* v0, int64_t v1, int64_t v2, int64_t v3);
bool HashMap_is_empty(void* v0, int64_t v1);
int64_t HashMap_keys(void* v0, int64_t v1);
int64_t HashMap_keys_i(void* v0, int64_t v1);
int64_t HashMap_len(void* v0, int64_t v1);
int64_t HashMap_new(void* v0);
void HashMap_remove(void* v0, int64_t v1, int64_t v2);
void HashMap_remove_i(void* v0, int64_t v1, int64_t v2);
int64_t HashMap_share(void* v0, int64_t v1);
void* HashMap_to_str(void* v0, int64_t v1);
int64_t HashMap_values(void* v0, int64_t v1);
int64_t HashMap_values_i(void* v0, int64_t v1);
void Option_and_then(void* v0, void* v1, void* v2, void* v3);
void Option_context(void* v0, void* v1, void* v2, void* v3);
int64_t Option_expect(void* v0, void* v1, void* v2);
bool Option_is_none(void* v0, void* v1);
bool Option_is_some(void* v0, void* v1);
void Option_map(void* v0, void* v1, void* v2, void* v3);
void Option_or_else(void* v0, void* v1, void* v2, void* v3);
int64_t Option_unwrap(void* v0, void* v1);
int64_t Option_unwrap_or(void* v0, void* v1, int64_t v2);
int64_t Option_unwrap_or_else(void* v0, void* v1, void* v2);
void Result_and_then(void* v0, void* v1, void* v2, void* v3);
void Result_context(void* v0, void* v1, void* v2, void* v3);
void Result_err(void* v0, void* v1, void* v2);
int64_t Result_expect(void* v0, void* v1, void* v2);
bool Result_is_err(void* v0, void* v1);
bool Result_is_ok(void* v0, void* v1);
void Result_map(void* v0, void* v1, void* v2, void* v3);
void Result_map_err(void* v0, void* v1, void* v2, void* v3);
void Result_ok(void* v0, void* v1, void* v2);
int64_t Result_unwrap(void* v0, void* v1);
int64_t Result_unwrap_or(void* v0, void* v1, int64_t v2);
int64_t Result_unwrap_or_else(void* v0, void* v1, void* v2);
void Result_with_context(void* v0, void* v1, void* v2, void* v3);
void Str_drop(void* v0, int64_t v1);
void StringBuilder_append_char(void* v0, void* v1, int64_t v2);
void StringBuilder_append_int(void* v0, void* v1, int64_t v2);
void StringBuilder_append_str(void* v0, void* v1, void* v2);
void StringBuilder_clear(void* v0, void* v1);
bool StringBuilder_is_empty(void* v0, void* v1);
int64_t StringBuilder_len(void* v0, void* v1);
void StringBuilder_new(void* v0, void* v1);
void* StringBuilder_to_str(void* v0, void* v1);
int64_t String_byte_at(void* v0, int64_t v1, int64_t v2);
int64_t String_bytes(void* v0, int64_t v1);
int64_t String_char_at(void* v0, int64_t v1, int64_t v2);
int64_t String_char_count(void* v0, int64_t v1);
int64_t String_clone(void* v0, int64_t v1);
bool String_contains(void* v0, int64_t v1, int64_t v2);
bool String_ends_with(void* v0, int64_t v1, int64_t v2);
int64_t String_find(void* v0, int64_t v1, int64_t v2);
int64_t String_from(void* v0, int64_t v1);
bool String_is_empty(void* v0, int64_t v1);
int64_t String_len(void* v0, int64_t v1);
int64_t String_parse_int(void* v0, int64_t v1);
int64_t String_push_str(void* v0, int64_t v1, int64_t v2);
int64_t String_repeat(void* v0, int64_t v1, int64_t v2);
int64_t String_replace(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t String_rfind(void* v0, int64_t v1, int64_t v2);
int64_t String_slice(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t String_slice_len(void* v0, int64_t v1, int64_t v2, int64_t v3, int64_t v4);
int64_t String_split(void* v0, int64_t v1, int64_t v2);
bool String_starts_with(void* v0, int64_t v1, int64_t v2);
int64_t String_to_lowercase(void* v0, int64_t v1);
int64_t String_to_string(void* v0, int64_t v1);
int64_t String_to_uppercase(void* v0, int64_t v1);
int64_t String_trim(void* v0, int64_t v1);
int64_t Vec_capacity(void* v0, int64_t v1);
void Vec_clear(void* v0, int64_t v1);
int64_t Vec_clone(void* v0, int64_t v1);
bool Vec_contains(void* v0, int64_t v1, int64_t v2);
void Vec_drop(void* v0, int64_t v1);
void Vec_drop_strs(void* v0, int64_t v1);
void Vec_drop_vecs(void* v0, int64_t v1);
bool Vec_eq_strs(void* v0, int64_t v1, int64_t v2);
bool Vec_eq_words(void* v0, int64_t v1, int64_t v2);
void Vec_extend(void* v0, int64_t v1, int64_t v2);
int64_t Vec_first(void* v0, int64_t v1);
int64_t Vec_get(void* v0, int64_t v1, int64_t v2);
void Vec_insert(void* v0, int64_t v1, int64_t v2, int64_t v3);
bool Vec_is_empty(void* v0, int64_t v1);
int64_t Vec_last(void* v0, int64_t v1);
int64_t Vec_len(void* v0, int64_t v1);
int64_t Vec_new(void* v0);
int64_t Vec_pop(void* v0, int64_t v1);
void Vec_push(void* v0, int64_t v1, int64_t v2);
int64_t Vec_remove(void* v0, int64_t v1, int64_t v2);
void Vec_reverse(void* v0, int64_t v1);
void Vec_set(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t Vec_share(void* v0, int64_t v1);
int64_t Vec_share_deep(void* v0, int64_t v1, int64_t v2, int64_t v3, int64_t v4);
void Vec_sort(void* v0, int64_t v1);
void* Vec_to_str(void* v0, int64_t v1);
void Vec_truncate(void* v0, int64_t v1, int64_t v2);
bool __eq_ByteBuffer(void* v0, void* v1, void* v2);
bool __eq_Expr(void* v0, void* v1, void* v2);
bool __eq_IfExpr(void* v0, void* v1, void* v2);
bool __eq_ParseResultIfExpr(void* v0, void* v1, void* v2);
bool __eq_StringBuilder(void* v0, void* v1, void* v2);
int64_t p_abs(void* v0, int64_t v1);
int64_t allocator_alloc(void* v0, void* v1, int64_t v2);
int64_t allocator_bytes_used(void* v0, void* v1);
void allocator_free(void* v0, void* v1, int64_t v2);
void assert(void* v0, bool v1);
void assert_msg(void* v0, bool v1, void* v2);
void* byte_to_str(void* v0, int64_t v1);
bool char_is_alpha(void* v0, int64_t v1);
bool char_is_digit(void* v0, int64_t v1);
bool char_is_lower(void* v0, int64_t v1);
bool char_is_upper(void* v0, int64_t v1);
bool char_is_whitespace(void* v0, int64_t v1);
int64_t char_to_lower(void* v0, int64_t v1);
void* char_to_str(void* v0, int64_t v1);
int64_t char_to_upper(void* v0, int64_t v1);
int64_t clamp(void* v0, int64_t v1, int64_t v2, int64_t v3);
void* create_allocator(void* v0, void* v1);
void* float_to_str(void* v0, double v1);
bool hm_eq(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t hm_hash(void* v0, int64_t v1, int64_t v2);
int64_t hm_int_hash(void* v0, int64_t v1);
bool hm_is_int(void* v0, int64_t v1);
int64_t hm_store_i(void* v0, int64_t v1, int64_t v2);
void* int_to_str(void* v0, int64_t v1);
int64_t main_entry(void* v0);
void map_own(void* v0, int64_t v1);
int64_t p_max(void* v0, int64_t v1, int64_t v2);
int64_t p_min(void* v0, int64_t v1, int64_t v2);
int64_t p_str_alloc(void* v0, int64_t v1);
int64_t p_str_compare(void* v0, int64_t v1, int64_t v2);
void p_str_copy_n(void* v0, int64_t v1, int64_t v2, int64_t v3);
bool p_str_equal(void* v0, int64_t v1, int64_t v2);
void p_str_free(void* v0, int64_t v1);
void* p_str_from_ptr(void* v0, int64_t v1);
int64_t p_str_hash(void* v0, int64_t v1);
bool p_str_len_word(void* v0, int64_t v1);
int64_t p_str_length(void* v0, int64_t v1);
void panic(void* v0, void* v1);
void println(void* v0, void* v1);
int64_t str_byte_at(void* v0, void* v1, int64_t v2);
void* str_bytes(void* v0, void* v1);
void* str_clone(void* v0, void* v1);
int64_t str_compare(void* v0, void* v1, void* v2);
void* str_concat(void* v0, void* v1, void* v2);
bool str_contains(void* v0, void* v1, void* v2);
bool str_ends_with(void* v0, void* v1, void* v2);
bool str_equal(void* v0, void* v1, void* v2);
int64_t str_find(void* v0, void* v1, void* v2);
void* str_from_bytes(void* v0, void* v1);
bool str_is_empty(void* v0, void* v1);
int64_t str_len(void* v0, void* v1);
void* str_lines(void* v0, void* v1);
double str_parse_float(void* v0, void* v1);
int64_t str_parse_hex(void* v0, void* v1);
int64_t str_parse_int(void* v0, void* v1);
int64_t str_parse_int_range(void* v0, void* v1, int64_t v2, int64_t v3);
void* str_repeat(void* v0, void* v1, int64_t v2);
void* str_replace(void* v0, void* v1, void* v2, void* v3);
int64_t str_rfind(void* v0, void* v1, void* v2);
void* str_slice(void* v0, void* v1, int64_t v2, int64_t v3);
void* str_slice_len(void* v0, void* v1, int64_t v2, int64_t v3, int64_t v4);
void* str_split(void* v0, void* v1, void* v2);
void* str_split_char(void* v0, void* v1, int64_t v2);
bool str_starts_with(void* v0, void* v1, void* v2);
void* str_to_lower(void* v0, void* v1);
void* str_to_upper(void* v0, void* v1);
void* str_trim(void* v0, void* v1);
void* str_trim_end(void* v0, void* v1);
void* str_trim_end_char(void* v0, void* v1, int64_t v2);
void* str_trim_start(void* v0, void* v1);
void* u64_to_str(void* v0, uint64_t v1);
int64_t vec_get_or(void* v0, void* v1, int64_t v2, int64_t v3);
void vec_own(void* v0, int64_t v1);
void vec_raw_extend(void* v0, int64_t v1, int64_t v2);
void vec_raw_insert(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t vec_raw_remove(void* v0, int64_t v1, int64_t v2);
void vec_raw_reverse(void* v0, int64_t v1);
void vec_raw_sort(void* v0, int64_t v1);
void vec_sift_down(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t vec_slot(void* v0, int64_t v1, int64_t v2);
void vec_swap_slots(void* v0, int64_t v1, int64_t v2, int64_t v3);

void ByteBuffer_clear(void* v0, void* v1) {
    void* v2 = 0;
    void* v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    void* v6 = 0;
    int64_t v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = *(void**)v2;
    v4 = 0LL;
    Vec_clear((void*)v4, (int64_t)v3);
    v6 = (char*)v1 + 8;
    v7 = 0LL;
    *(int64_t*)v6 = (int64_t)v7;
    return;
    return;
}

void ByteBuffer_from_string(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    int64_t v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    int64_t v24 = 0;
    bool v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    int64_t v28 = 0;
    void* v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0;
    void* v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    void* v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0; /* placeholder for void */
    void* v38 = 0;
    uint8_t v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    void* v50 = 0;
    void* v51 = 0;
    void* v52 = 0;
    int64_t v53 = 0;
    void* v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    void* v56 = 0;
    void* v57 = 0;
    int64_t v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0; /* placeholder for void */
    v4 = alloca(16);
    v7 = alloca(16);
    v16 = alloca(8);
    v19 = alloca(8);
    v29 = alloca(8);
    v32 = alloca(8);
    v35 = alloca(8);
    v3 = 0LL;
    memset(v4, 0, 16);
    v5 = 0LL;
    ByteBuffer_new((void*)v4, (void*)v5, v3);
    memset(v7, 0, 16);
    memcpy(v7, v4, 16);
    v9 = (char*)v7 + 0;
    v10 = *(void**)v9;
    v11 = 0LL;
    v12 = (void*)Vec_share((void*)v11, (int64_t)v10);
    *(void**)v9 = (void*)v12;
    v14 = 0LL;
    v15 = (int64_t)String_len((void*)v14, (int64_t)v2);
    memset(v16, 0, 8);
    *(int64_t*)v16 = (int64_t)v15;
    v18 = 0LL;
    memset(v19, 0, 8);
    *(int64_t*)v19 = (int64_t)v18;
    goto bb1;
  bb1:
    p_co_tick();
    v23 = *(int64_t*)v19;
    v24 = *(int64_t*)v16;
    v25 = v23 < v24;
    if (v25) goto bb2;
    else goto bb3;
  bb2:
    v27 = *(int64_t*)v19;
    v28 = (int64_t)v2 + v27;
    memset(v29, 0, 8);
    *(int64_t*)v29 = (int64_t)v28;
    v31 = *(int64_t*)v29;
    memset(v32, 0, 8);
    *(void**)v32 = (void*)v31;
    v34 = 0LL;
    memset(v35, 0, 8);
    *(int64_t*)v35 = (int64_t)v34;
    v38 = *(void**)v32;
    v39 = *(uint8_t*)v38;
    *(int64_t*)v35 = (int64_t)v39;
    v42 = *(int64_t*)v35;
    v43 = 0LL;
    ByteBuffer_write_byte((void*)v43, (void*)v7, v42);
    v45 = *(int64_t*)v19;
    v46 = 1LL;
    v47 = (int64_t)((uint64_t)v45 + (uint64_t)v46);
    *(int64_t*)v19 = (int64_t)v47;
    goto bb1;
  bb3:
    v50 = (char*)v7 + 0;
    v51 = (char*)v0 + 0;
    v52 = *(void**)v50;
    v53 = 0LL;
    v54 = (void*)Vec_share((void*)v53, (int64_t)v52);
    *(void**)v51 = (void*)v54;
    v56 = (char*)v7 + 8;
    v57 = (char*)v0 + 8;
    v58 = *(int64_t*)v56;
    *(int64_t*)v57 = (int64_t)v58;
    return;
    return;
}

bool ByteBuffer_is_empty(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    return v5;
    return 0;
}

int64_t ByteBuffer_len(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    return v3;
    return 0;
}

void ByteBuffer_new(void* v0, void* v1, int64_t v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    void* v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    void* v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0;
    void* v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    void* v18 = 0;
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    v3 = alloca(16);
    memset(v3, 0, 16);
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
    v14 = 0LL;
    v15 = (void*)Vec_share((void*)v14, (int64_t)v13);
    *(void**)v12 = (void*)v15;
    v17 = (char*)v3 + 8;
    v18 = (char*)v0 + 8;
    v19 = *(int64_t*)v17;
    *(int64_t*)v18 = (int64_t)v19;
    return;
    return;
}

int64_t ByteBuffer_read_byte(void* v0, void* v1, int64_t v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    bool v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    bool v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0;
    int64_t v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    v3 = alloca(1);
    memset(v3, 0, 1);
    v4 = 0LL;
    v5 = v2 < v4;
    *(bool*)v3 = v5;
    if (v5) goto bb2;
    else goto bb1;
  bb1:
    v8 = (char*)v1 + 8;
    v9 = *(int64_t*)v8;
    v10 = v2 >= v9;
    *(bool*)v3 = v10;
    goto bb2;
  bb2:
    v13 = *(bool*)v3;
    if (v13) goto bb3;
    else goto bb4;
  bb3:
    v15 = -1LL;
    return v15;
  bb4:
    goto bb5;
  bb5:
    v18 = (char*)v1 + 0;
    v19 = *(void**)v18;
    v20 = 0LL;
    v21 = (int64_t)Vec_get((void*)v20, (int64_t)v19, v2);
    return v21;
    return 0;
}

void ByteBuffer_set_byte(void* v0, void* v1, int64_t v2, int64_t v3) {
    void* v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    bool v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    v4 = alloca(1);
    memset(v4, 0, 1);
    v5 = 0LL;
    v6 = v2 < v5;
    *(bool*)v4 = v6;
    if (v6) goto bb2;
    else goto bb1;
  bb1:
    v9 = (char*)v1 + 8;
    v10 = *(int64_t*)v9;
    v11 = v2 >= v10;
    *(bool*)v4 = v11;
    goto bb2;
  bb2:
    v14 = *(bool*)v4;
    if (v14) goto bb3;
    else goto bb4;
  bb3:
    return;
  bb4:
    goto bb5;
  bb5:
    v18 = (char*)v1 + 0;
    v19 = *(void**)v18;
    v20 = 0LL;
    Vec_set((void*)v20, (int64_t)v19, v2, v3);
    return;
    return;
}

void* ByteBuffer_to_str(void* v0, void* v1) {
    void* v2 = 0;
    void* v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0;
    bool v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0;
    int64_t v19 = 0;
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    int64_t v23 = 0;
    void* v24 = 0;
    void* v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    void* v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    v3 = alloca(8);
    v6 = alloca(8);
    v20 = alloca(8);
    v2 = _str0;
    memset(v3, 0, 8);
    *(void**)v3 = (void*)v2;
    v5 = 0LL;
    memset(v6, 0, 8);
    *(int64_t*)v6 = (int64_t)v5;
    goto bb1;
  bb1:
    p_co_tick();
    v10 = *(int64_t*)v6;
    v11 = (char*)v1 + 8;
    v12 = *(int64_t*)v11;
    v13 = v10 < v12;
    if (v13) goto bb2;
    else goto bb3;
  bb2:
    v15 = (char*)v1 + 0;
    v16 = *(void**)v15;
    v17 = *(int64_t*)v6;
    v18 = 0LL;
    v19 = (int64_t)Vec_get((void*)v18, (int64_t)v16, v17);
    memset(v20, 0, 8);
    *(int64_t*)v20 = (int64_t)v19;
    v22 = *(void**)v3;
    v23 = *(int64_t*)v20;
    v24 = p_char_to_str(v23);
    v25 = p_str_concat((void*)v22, (void*)v24);
    *(void**)v3 = (void*)v25;
    v27 = *(int64_t*)v6;
    v28 = 1LL;
    v29 = (int64_t)((uint64_t)v27 + (uint64_t)v28);
    *(int64_t*)v6 = (int64_t)v29;
    goto bb1;
  bb3:
    v32 = *(void**)v3;
    return v32;
    return 0;
}

void ByteBuffer_write_byte(void* v0, void* v1, int64_t v2) {
    void* v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    void* v8 = 0;
    int64_t v9 = 0;
    int64_t v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    v3 = (char*)v1 + 0;
    v4 = *(void**)v3;
    v5 = 0LL;
    Vec_push((void*)v5, (int64_t)v4, v2);
    v7 = (char*)v1 + 8;
    v8 = (char*)v1 + 8;
    v9 = *(int64_t*)v8;
    v10 = 1LL;
    v11 = p_add_i64(v9, v10);
    *(int64_t*)v7 = (int64_t)v11;
    return;
    return;
}

void ByteBuffer_write_bytes(void* v0, void* v1, int64_t v2, int64_t v3) {
    int64_t v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    bool v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0;
    int64_t v13 = 0;
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    void* v23 = 0;
    uint8_t v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    v5 = alloca(8);
    v14 = alloca(8);
    v17 = alloca(8);
    v20 = alloca(8);
    v4 = 0LL;
    memset(v5, 0, 8);
    *(int64_t*)v5 = (int64_t)v4;
    goto bb1;
  bb1:
    p_co_tick();
    v9 = *(int64_t*)v5;
    v10 = v9 < v3;
    if (v10) goto bb2;
    else goto bb3;
  bb2:
    v12 = *(int64_t*)v5;
    v13 = p_add_i64(v2, v12);
    memset(v14, 0, 8);
    *(int64_t*)v14 = (int64_t)v13;
    v16 = *(int64_t*)v14;
    memset(v17, 0, 8);
    *(void**)v17 = (void*)v16;
    v19 = 0LL;
    memset(v20, 0, 8);
    *(int64_t*)v20 = (int64_t)v19;
    v23 = *(void**)v17;
    v24 = *(uint8_t*)v23;
    *(int64_t*)v20 = (int64_t)v24;
    v27 = *(int64_t*)v20;
    v28 = 0LL;
    ByteBuffer_write_byte((void*)v28, (void*)v1, v27);
    v30 = *(int64_t*)v5;
    v31 = 1LL;
    v32 = (int64_t)((uint64_t)v30 + (uint64_t)v31);
    *(int64_t*)v5 = (int64_t)v32;
    goto bb1;
  bb3:
    return;
    return;
}

void HashMap_clear(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    int64_t v4 = 0;
    int64_t v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    int64_t v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    void* v29 = 0;
    int64_t v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0; /* placeholder for void */
    void* v33 = 0;
    int64_t v34 = 0;
    int64_t v35 = 0;
    bool v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0;
    bool v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    bool v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    void* v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0;
    int64_t v52 = 0;
    bool v53 = 0;
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0;
    int64_t v58 = 0;
    int64_t v59 = 0;
    void* v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0;
    void* v63 = 0;
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0; /* placeholder for void */
    void* v66 = 0;
    int64_t v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0;
    int64_t v71 = 0;
    int64_t v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0; /* placeholder for void */
    v6 = alloca(8);
    v13 = alloca(8);
    v17 = alloca(8);
    v20 = alloca(8);
    v23 = alloca(8);
    v33 = alloca(1);
    v47 = alloca(8);
    v60 = alloca(8);
    v63 = alloca(8);
    v2 = 0LL;
    map_own((void*)v2, v1);
    v4 = 16LL;
    v5 = p_add_i64(v1, v4);
    memset(v6, 0, 8);
    *(void**)v6 = (void*)v5;
    v9 = *(void**)v6;
    v10 = 0LL;
    *(int64_t*)v9 = (int64_t)v10;
    memset(v13, 0, 8);
    *(void**)v13 = (void*)v1;
    v15 = 24LL;
    v16 = p_add_i64(v1, v15);
    memset(v17, 0, 8);
    *(void**)v17 = (void*)v16;
    v19 = 0LL;
    memset(v20, 0, 8);
    *(int64_t*)v20 = (int64_t)v19;
    v22 = 0LL;
    memset(v23, 0, 8);
    *(int64_t*)v23 = (int64_t)v22;
    v26 = *(void**)v13;
    v27 = *(int64_t*)v26;
    *(int64_t*)v20 = (int64_t)v27;
    v29 = *(void**)v17;
    v30 = *(int64_t*)v29;
    *(int64_t*)v23 = (int64_t)v30;
    memset(v33, 0, 1);
    v34 = *(int64_t*)v20;
    v35 = 0LL;
    v36 = v34 != v35;
    *(bool*)v33 = v36;
    if (v36) goto bb1;
    else goto bb2;
  bb1:
    v39 = *(int64_t*)v23;
    v40 = 0LL;
    v41 = v39 > v40;
    *(bool*)v33 = v41;
    goto bb2;
  bb2:
    v44 = *(bool*)v33;
    if (v44) goto bb3;
    else goto bb4;
  bb3:
    v46 = 0LL;
    memset(v47, 0, 8);
    *(int64_t*)v47 = (int64_t)v46;
    goto bb6;
  bb4:
    goto bb5;
  bb5:
    return;
  bb6:
    p_co_tick();
    v51 = *(int64_t*)v47;
    v52 = *(int64_t*)v23;
    v53 = v51 < v52;
    if (v53) goto bb7;
    else goto bb8;
  bb7:
    v55 = *(int64_t*)v20;
    v56 = *(int64_t*)v47;
    v57 = 8LL;
    v58 = p_mul_i64(v56, v57);
    v59 = p_add_i64(v55, v58);
    memset(v60, 0, 8);
    *(int64_t*)v60 = (int64_t)v59;
    v62 = *(int64_t*)v60;
    memset(v63, 0, 8);
    *(void**)v63 = (void*)v62;
    v66 = *(void**)v63;
    v67 = 0LL;
    *(int64_t*)v66 = (int64_t)v67;
    v70 = *(int64_t*)v47;
    v71 = 1LL;
    v72 = (int64_t)((uint64_t)v70 + (uint64_t)v71);
    *(int64_t*)v47 = (int64_t)v72;
    goto bb6;
  bb8:
    goto bb5;
    return;
}

int64_t HashMap_clone(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    int64_t v10 = 0;
    int64_t v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    int64_t v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    void* v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    int64_t v28 = 0;
    void* v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0;
    void* v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    void* v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    void* v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0; /* placeholder for void */
    void* v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    void* v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    void* v47 = 0;
    int64_t v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    void* v51 = 0;
    int64_t v52 = 0;
    int64_t v53 = 0;
    bool v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0;
    int64_t v58 = 0;
    bool v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0; /* placeholder for void */
    bool v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0;
    void* v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0;
    int64_t v73 = 0;
    bool v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0;
    int64_t v77 = 0;
    int64_t v78 = 0;
    int64_t v79 = 0;
    int64_t v80 = 0;
    void* v81 = 0;
    int64_t v82 = 0; /* placeholder for void */
    int64_t v83 = 0;
    void* v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0;
    void* v87 = 0;
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0; /* placeholder for void */
    void* v90 = 0;
    int64_t v91 = 0;
    int64_t v92 = 0; /* placeholder for void */
    int64_t v93 = 0; /* placeholder for void */
    void* v94 = 0;
    int64_t v95 = 0;
    int64_t v96 = 0;
    bool v97 = 0;
    int64_t v98 = 0; /* placeholder for void */
    int64_t v99 = 0; /* placeholder for void */
    int64_t v100 = 0;
    int64_t v101 = 0;
    bool v102 = 0;
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0; /* placeholder for void */
    bool v105 = 0;
    int64_t v106 = 0; /* placeholder for void */
    int64_t v107 = 0;
    int64_t v108 = 0;
    int64_t v109 = 0;
    int64_t v110 = 0;
    int64_t v111 = 0;
    void* v112 = 0;
    int64_t v113 = 0; /* placeholder for void */
    int64_t v114 = 0;
    void* v115 = 0;
    int64_t v116 = 0; /* placeholder for void */
    int64_t v117 = 0;
    void* v118 = 0;
    int64_t v119 = 0; /* placeholder for void */
    int64_t v120 = 0; /* placeholder for void */
    void* v121 = 0;
    int64_t v122 = 0;
    int64_t v123 = 0; /* placeholder for void */
    int64_t v124 = 0; /* placeholder for void */
    int64_t v125 = 0;
    int64_t v126 = 0;
    int64_t v127 = 0;
    int64_t v128 = 0;
    int64_t v129 = 0; /* placeholder for void */
    int64_t v130 = 0; /* placeholder for void */
    int64_t v131 = 0; /* placeholder for void */
    int64_t v132 = 0;
    int64_t v133 = 0;
    int64_t v134 = 0;
    int64_t v135 = 0; /* placeholder for void */
    int64_t v136 = 0; /* placeholder for void */
    int64_t v137 = 0;
    int64_t v138 = 0; /* placeholder for void */
    v4 = alloca(8);
    v12 = alloca(8);
    v23 = alloca(8);
    v25 = alloca(8);
    v29 = alloca(8);
    v32 = alloca(8);
    v35 = alloca(8);
    v38 = alloca(8);
    v51 = alloca(1);
    v68 = alloca(8);
    v81 = alloca(8);
    v84 = alloca(8);
    v87 = alloca(8);
    v94 = alloca(1);
    v112 = alloca(8);
    v115 = alloca(8);
    v118 = alloca(8);
    v2 = 0LL;
    v3 = (int64_t)HashMap_new((void*)v2);
    memset(v4, 0, 8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = 0LL;
    v7 = (bool)hm_is_int((void*)v6, v1);
    if (v7) goto bb1;
    else goto bb2;
  bb1:
    v9 = *(int64_t*)v4;
    v10 = 40LL;
    v11 = p_add_i64(v9, v10);
    memset(v12, 0, 8);
    *(void**)v12 = (void*)v11;
    v15 = *(void**)v12;
    v16 = 1LL;
    *(int64_t*)v15 = (int64_t)v16;
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v21 = 24LL;
    v22 = p_add_i64(v1, v21);
    memset(v23, 0, 8);
    *(void**)v23 = (void*)v22;
    memset(v25, 0, 8);
    *(void**)v25 = (void*)v1;
    v27 = 8LL;
    v28 = p_add_i64(v1, v27);
    memset(v29, 0, 8);
    *(void**)v29 = (void*)v28;
    v31 = 0LL;
    memset(v32, 0, 8);
    *(int64_t*)v32 = (int64_t)v31;
    v34 = 0LL;
    memset(v35, 0, 8);
    *(int64_t*)v35 = (int64_t)v34;
    v37 = 0LL;
    memset(v38, 0, 8);
    *(int64_t*)v38 = (int64_t)v37;
    v41 = *(void**)v23;
    v42 = *(int64_t*)v41;
    *(int64_t*)v32 = (int64_t)v42;
    v44 = *(void**)v25;
    v45 = *(int64_t*)v44;
    *(int64_t*)v35 = (int64_t)v45;
    v47 = *(void**)v29;
    v48 = *(int64_t*)v47;
    *(int64_t*)v38 = (int64_t)v48;
    memset(v51, 0, 1);
    v52 = *(int64_t*)v35;
    v53 = 0LL;
    v54 = v52 == v53;
    *(bool*)v51 = v54;
    if (v54) goto bb5;
    else goto bb4;
  bb4:
    v57 = *(int64_t*)v32;
    v58 = 0LL;
    v59 = v57 == v58;
    *(bool*)v51 = v59;
    goto bb5;
  bb5:
    v62 = *(bool*)v51;
    if (v62) goto bb6;
    else goto bb7;
  bb6:
    v64 = *(int64_t*)v4;
    return v64;
  bb7:
    goto bb8;
  bb8:
    v67 = 0LL;
    memset(v68, 0, 8);
    *(int64_t*)v68 = (int64_t)v67;
    goto bb9;
  bb9:
    p_co_tick();
    v72 = *(int64_t*)v68;
    v73 = *(int64_t*)v32;
    v74 = v72 < v73;
    if (v74) goto bb10;
    else goto bb11;
  bb10:
    v76 = *(int64_t*)v35;
    v77 = *(int64_t*)v68;
    v78 = 8LL;
    v79 = p_mul_i64(v77, v78);
    v80 = p_add_i64(v76, v79);
    memset(v81, 0, 8);
    *(int64_t*)v81 = (int64_t)v80;
    v83 = *(int64_t*)v81;
    memset(v84, 0, 8);
    *(void**)v84 = (void*)v83;
    v86 = 0LL;
    memset(v87, 0, 8);
    *(int64_t*)v87 = (int64_t)v86;
    v90 = *(void**)v84;
    v91 = *(int64_t*)v90;
    *(int64_t*)v87 = (int64_t)v91;
    memset(v94, 0, 1);
    v95 = *(int64_t*)v87;
    v96 = 0LL;
    v97 = v95 != v96;
    *(bool*)v94 = v97;
    if (v97) goto bb12;
    else goto bb13;
  bb11:
    v137 = *(int64_t*)v4;
    return v137;
  bb12:
    v100 = *(int64_t*)v87;
    v101 = -1LL;
    v102 = v100 != v101;
    *(bool*)v94 = v102;
    goto bb13;
  bb13:
    v105 = *(bool*)v94;
    if (v105) goto bb14;
    else goto bb15;
  bb14:
    v107 = *(int64_t*)v38;
    v108 = *(int64_t*)v68;
    v109 = 8LL;
    v110 = p_mul_i64(v108, v109);
    v111 = p_add_i64(v107, v110);
    memset(v112, 0, 8);
    *(int64_t*)v112 = (int64_t)v111;
    v114 = *(int64_t*)v112;
    memset(v115, 0, 8);
    *(void**)v115 = (void*)v114;
    v117 = 0LL;
    memset(v118, 0, 8);
    *(int64_t*)v118 = (int64_t)v117;
    v121 = *(void**)v115;
    v122 = *(int64_t*)v121;
    *(int64_t*)v118 = (int64_t)v122;
    v125 = *(int64_t*)v4;
    v126 = *(int64_t*)v87;
    v127 = *(int64_t*)v118;
    v128 = 0LL;
    HashMap_insert((void*)v128, v125, v126, v127);
    goto bb16;
  bb15:
    goto bb16;
  bb16:
    v132 = *(int64_t*)v68;
    v133 = 1LL;
    v134 = (int64_t)((uint64_t)v132 + (uint64_t)v133);
    *(int64_t*)v68 = (int64_t)v134;
    goto bb9;
    return 0;
}

bool HashMap_contains(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    int64_t v16 = 0;
    bool v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    bool v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    int64_t v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0;
    void* v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    void* v32 = 0;
    int64_t v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    void* v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    void* v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    void* v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    void* v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    bool v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    int64_t v61 = 0;
    bool v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0; /* placeholder for void */
    bool v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    v5 = alloca(8);
    v8 = alloca(8);
    v24 = alloca(8);
    v26 = alloca(8);
    v29 = alloca(8);
    v41 = alloca(8);
    v44 = alloca(8);
    v47 = alloca(8);
    v54 = alloca(1);
    v3 = 24LL;
    v4 = p_add_i64(v1, v3);
    memset(v5, 0, 8);
    *(void**)v5 = (void*)v4;
    v7 = 0LL;
    memset(v8, 0, 8);
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
    return v19;
  bb2:
    goto bb3;
  bb3:
    v22 = 0LL;
    v23 = (int64_t)HashMap_find_slot((void*)v22, v1, v2);
    memset(v24, 0, 8);
    *(int64_t*)v24 = (int64_t)v23;
    memset(v26, 0, 8);
    *(void**)v26 = (void*)v1;
    v28 = 0LL;
    memset(v29, 0, 8);
    *(int64_t*)v29 = (int64_t)v28;
    v32 = *(void**)v26;
    v33 = *(int64_t*)v32;
    *(int64_t*)v29 = (int64_t)v33;
    v36 = *(int64_t*)v29;
    v37 = *(int64_t*)v24;
    v38 = 8LL;
    v39 = p_mul_i64(v37, v38);
    v40 = p_add_i64(v36, v39);
    memset(v41, 0, 8);
    *(int64_t*)v41 = (int64_t)v40;
    v43 = *(int64_t*)v41;
    memset(v44, 0, 8);
    *(void**)v44 = (void*)v43;
    v46 = 0LL;
    memset(v47, 0, 8);
    *(int64_t*)v47 = (int64_t)v46;
    v50 = *(void**)v44;
    v51 = *(int64_t*)v50;
    *(int64_t*)v47 = (int64_t)v51;
    memset(v54, 0, 1);
    v55 = *(int64_t*)v47;
    v56 = 0LL;
    v57 = v55 != v56;
    *(bool*)v54 = v57;
    if (v57) goto bb4;
    else goto bb5;
  bb4:
    v60 = *(int64_t*)v47;
    v61 = -1LL;
    v62 = v60 != v61;
    *(bool*)v54 = v62;
    goto bb5;
  bb5:
    v65 = *(bool*)v54;
    return v65;
    return 0;
}

bool HashMap_contains_i(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)hm_store_i((void*)v3, v1, v2);
    v5 = 0LL;
    v6 = (bool)HashMap_contains((void*)v5, v1, v4);
    return v6;
    return 0;
}

void HashMap_drop(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    int64_t v20 = 0;
    void* v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    void* v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    void* v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    void* v33 = 0;
    int64_t v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    int64_t v38 = 0;
    bool v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    int64_t v42 = 0;
    bool v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    int64_t v50 = 0;
    bool v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0; /* placeholder for void */
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0; /* placeholder for void */
    v7 = alloca(8);
    v11 = alloca(8);
    v14 = alloca(8);
    v17 = alloca(8);
    v21 = alloca(8);
    v24 = alloca(8);
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    return;
  bb2:
    goto bb3;
  bb3:
    memset(v7, 0, 8);
    *(void**)v7 = (void*)v1;
    v9 = 8LL;
    v10 = p_add_i64(v1, v9);
    memset(v11, 0, 8);
    *(void**)v11 = (void*)v10;
    v13 = 0LL;
    memset(v14, 0, 8);
    *(int64_t*)v14 = (int64_t)v13;
    v16 = 0LL;
    memset(v17, 0, 8);
    *(int64_t*)v17 = (int64_t)v16;
    v19 = 32LL;
    v20 = p_add_i64(v1, v19);
    memset(v21, 0, 8);
    *(void**)v21 = (void*)v20;
    v23 = 0LL;
    memset(v24, 0, 8);
    *(int64_t*)v24 = (int64_t)v23;
    v27 = *(void**)v7;
    v28 = *(int64_t*)v27;
    *(int64_t*)v14 = (int64_t)v28;
    v30 = *(void**)v11;
    v31 = *(int64_t*)v30;
    *(int64_t*)v17 = (int64_t)v31;
    v33 = *(void**)v21;
    v34 = *(int64_t*)v33;
    *(int64_t*)v24 = (int64_t)v34;
    v37 = *(int64_t*)v24;
    v38 = 0LL;
    v39 = v37 == v38;
    if (v39) goto bb4;
    else goto bb5;
  bb4:
    v41 = *(int64_t*)v14;
    v42 = 0LL;
    v43 = v41 != v42;
    if (v43) goto bb7;
    else goto bb8;
  bb5:
    goto bb6;
  bb6:
    p_free((void*)v1);
    return;
  bb7:
    v45 = *(int64_t*)v14;
    p_free((void*)v45);
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    v49 = *(int64_t*)v17;
    v50 = 0LL;
    v51 = v49 != v50;
    if (v51) goto bb10;
    else goto bb11;
  bb10:
    v53 = *(int64_t*)v17;
    p_free((void*)v53);
    goto bb12;
  bb11:
    goto bb12;
  bb12:
    goto bb6;
    return;
}

int64_t HashMap_find_slot(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    void* v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    int64_t v18 = 0;
    bool v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    void* v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    void* v28 = 0;
    int64_t v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    int64_t v33 = 0;
    void* v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0;
    void* v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    void* v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0;
    void* v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    int64_t v50 = 0;
    bool v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0;
    void* v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    void* v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    int64_t v63 = 0;
    void* v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0; /* placeholder for void */
    void* v67 = 0;
    int64_t v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0;
    int64_t v72 = 0;
    bool v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0;
    int64_t v76 = 0;
    bool v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0;
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0;
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0; /* placeholder for void */
    int64_t v85 = 0;
    int64_t v86 = 0;
    bool v87 = 0;
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0;
    int64_t v90 = 0;
    bool v91 = 0;
    int64_t v92 = 0; /* placeholder for void */
    int64_t v93 = 0;
    int64_t v94 = 0; /* placeholder for void */
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0;
    int64_t v99 = 0;
    bool v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0;
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0; /* placeholder for void */
    int64_t v105 = 0; /* placeholder for void */
    int64_t v106 = 0;
    int64_t v107 = 0;
    int64_t v108 = 0;
    int64_t v109 = 0; /* placeholder for void */
    int64_t v110 = 0;
    int64_t v111 = 0;
    bool v112 = 0;
    int64_t v113 = 0; /* placeholder for void */
    int64_t v114 = 0;
    int64_t v115 = 0; /* placeholder for void */
    int64_t v116 = 0; /* placeholder for void */
    int64_t v117 = 0; /* placeholder for void */
    int64_t v118 = 0;
    int64_t v119 = 0;
    int64_t v120 = 0;
    int64_t v121 = 0; /* placeholder for void */
    int64_t v122 = 0; /* placeholder for void */
    int64_t v123 = 0;
    int64_t v124 = 0;
    bool v125 = 0;
    int64_t v126 = 0; /* placeholder for void */
    int64_t v127 = 0;
    int64_t v128 = 0; /* placeholder for void */
    int64_t v129 = 0; /* placeholder for void */
    int64_t v130 = 0;
    int64_t v131 = 0; /* placeholder for void */
    v5 = alloca(8);
    v7 = alloca(8);
    v10 = alloca(8);
    v25 = alloca(8);
    v34 = alloca(8);
    v39 = alloca(8);
    v42 = alloca(8);
    v45 = alloca(8);
    v58 = alloca(8);
    v61 = alloca(8);
    v64 = alloca(8);
    v3 = 24LL;
    v4 = p_add_i64(v1, v3);
    memset(v5, 0, 8);
    *(void**)v5 = (void*)v4;
    memset(v7, 0, 8);
    *(void**)v7 = (void*)v1;
    v9 = 0LL;
    memset(v10, 0, 8);
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
    return v21;
  bb2:
    goto bb3;
  bb3:
    v24 = 0LL;
    memset(v25, 0, 8);
    *(int64_t*)v25 = (int64_t)v24;
    v28 = *(void**)v7;
    v29 = *(int64_t*)v28;
    *(int64_t*)v25 = (int64_t)v29;
    v32 = 0LL;
    v33 = (int64_t)hm_hash((void*)v32, v1, v2);
    memset(v34, 0, 8);
    *(int64_t*)v34 = (int64_t)v33;
    v36 = *(int64_t*)v34;
    v37 = *(int64_t*)v10;
    v38 = p_mod_i64(v36, v37);
    memset(v39, 0, 8);
    *(int64_t*)v39 = (int64_t)v38;
    v41 = -1LL;
    memset(v42, 0, 8);
    *(int64_t*)v42 = (int64_t)v41;
    v44 = 0LL;
    memset(v45, 0, 8);
    *(int64_t*)v45 = (int64_t)v44;
    goto bb4;
  bb4:
    p_co_tick();
    v49 = *(int64_t*)v45;
    v50 = *(int64_t*)v10;
    v51 = v49 < v50;
    if (v51) goto bb5;
    else goto bb6;
  bb5:
    v53 = *(int64_t*)v25;
    v54 = *(int64_t*)v39;
    v55 = 8LL;
    v56 = p_mul_i64(v54, v55);
    v57 = p_add_i64(v53, v56);
    memset(v58, 0, 8);
    *(int64_t*)v58 = (int64_t)v57;
    v60 = *(int64_t*)v58;
    memset(v61, 0, 8);
    *(void**)v61 = (void*)v60;
    v63 = 0LL;
    memset(v64, 0, 8);
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
    v123 = *(int64_t*)v42;
    v124 = 0LL;
    v125 = v123 >= v124;
    if (v125) goto bb25;
    else goto bb26;
  bb7:
    v75 = *(int64_t*)v42;
    v76 = 0LL;
    v77 = v75 >= v76;
    if (v77) goto bb10;
    else goto bb11;
  bb8:
    goto bb9;
  bb9:
    v85 = *(int64_t*)v64;
    v86 = -1LL;
    v87 = v85 == v86;
    if (v87) goto bb13;
    else goto bb14;
  bb10:
    v79 = *(int64_t*)v42;
    return v79;
  bb11:
    goto bb12;
  bb12:
    v82 = *(int64_t*)v39;
    return v82;
  bb13:
    v89 = *(int64_t*)v42;
    v90 = 0LL;
    v91 = v89 < v90;
    if (v91) goto bb16;
    else goto bb17;
  bb14:
    v98 = *(int64_t*)v64;
    v99 = 0LL;
    v100 = (bool)hm_eq((void*)v99, v1, v98, v2);
    if (v100) goto bb19;
    else goto bb20;
  bb15:
    v106 = *(int64_t*)v39;
    v107 = 1LL;
    v108 = p_add_i64(v106, v107);
    *(int64_t*)v39 = (int64_t)v108;
    v110 = *(int64_t*)v39;
    v111 = *(int64_t*)v10;
    v112 = v110 >= v111;
    if (v112) goto bb22;
    else goto bb23;
  bb16:
    v93 = *(int64_t*)v39;
    *(int64_t*)v42 = (int64_t)v93;
    goto bb18;
  bb17:
    goto bb18;
  bb18:
    goto bb15;
  bb19:
    v102 = *(int64_t*)v39;
    return v102;
  bb20:
    goto bb21;
  bb21:
    goto bb15;
  bb22:
    v114 = 0LL;
    *(int64_t*)v39 = (int64_t)v114;
    goto bb24;
  bb23:
    goto bb24;
  bb24:
    v118 = *(int64_t*)v45;
    v119 = 1LL;
    v120 = (int64_t)((uint64_t)v118 + (uint64_t)v119);
    *(int64_t*)v45 = (int64_t)v120;
    goto bb4;
  bb25:
    v127 = *(int64_t*)v42;
    return v127;
  bb26:
    goto bb27;
  bb27:
    v130 = 0LL;
    return v130;
    return 0;
}

int64_t HashMap_get(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    int64_t v16 = 0;
    bool v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    int64_t v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0;
    void* v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    void* v32 = 0;
    int64_t v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    void* v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    void* v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    void* v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    void* v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    bool v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    int64_t v61 = 0;
    bool v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0; /* placeholder for void */
    bool v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0;
    int64_t v71 = 0;
    void* v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    int64_t v74 = 0;
    void* v75 = 0;
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0; /* placeholder for void */
    void* v78 = 0;
    int64_t v79 = 0;
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0;
    int64_t v83 = 0;
    int64_t v84 = 0;
    int64_t v85 = 0;
    int64_t v86 = 0;
    void* v87 = 0;
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0;
    void* v90 = 0;
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0;
    void* v93 = 0;
    int64_t v94 = 0; /* placeholder for void */
    int64_t v95 = 0; /* placeholder for void */
    void* v96 = 0;
    int64_t v97 = 0;
    int64_t v98 = 0; /* placeholder for void */
    int64_t v99 = 0; /* placeholder for void */
    int64_t v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    v5 = alloca(8);
    v8 = alloca(8);
    v24 = alloca(8);
    v26 = alloca(8);
    v29 = alloca(8);
    v41 = alloca(8);
    v44 = alloca(8);
    v47 = alloca(8);
    v54 = alloca(1);
    v72 = alloca(8);
    v75 = alloca(8);
    v87 = alloca(8);
    v90 = alloca(8);
    v93 = alloca(8);
    v3 = 24LL;
    v4 = p_add_i64(v1, v3);
    memset(v5, 0, 8);
    *(void**)v5 = (void*)v4;
    v7 = 0LL;
    memset(v8, 0, 8);
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
    return v19;
  bb2:
    goto bb3;
  bb3:
    v22 = 0LL;
    v23 = (int64_t)HashMap_find_slot((void*)v22, v1, v2);
    memset(v24, 0, 8);
    *(int64_t*)v24 = (int64_t)v23;
    memset(v26, 0, 8);
    *(void**)v26 = (void*)v1;
    v28 = 0LL;
    memset(v29, 0, 8);
    *(int64_t*)v29 = (int64_t)v28;
    v32 = *(void**)v26;
    v33 = *(int64_t*)v32;
    *(int64_t*)v29 = (int64_t)v33;
    v36 = *(int64_t*)v29;
    v37 = *(int64_t*)v24;
    v38 = 8LL;
    v39 = p_mul_i64(v37, v38);
    v40 = p_add_i64(v36, v39);
    memset(v41, 0, 8);
    *(int64_t*)v41 = (int64_t)v40;
    v43 = *(int64_t*)v41;
    memset(v44, 0, 8);
    *(void**)v44 = (void*)v43;
    v46 = 0LL;
    memset(v47, 0, 8);
    *(int64_t*)v47 = (int64_t)v46;
    v50 = *(void**)v44;
    v51 = *(int64_t*)v50;
    *(int64_t*)v47 = (int64_t)v51;
    memset(v54, 0, 1);
    v55 = *(int64_t*)v47;
    v56 = 0LL;
    v57 = v55 == v56;
    *(bool*)v54 = v57;
    if (v57) goto bb5;
    else goto bb4;
  bb4:
    v60 = *(int64_t*)v47;
    v61 = -1LL;
    v62 = v60 == v61;
    *(bool*)v54 = v62;
    goto bb5;
  bb5:
    v65 = *(bool*)v54;
    if (v65) goto bb6;
    else goto bb7;
  bb6:
    v67 = 0LL;
    return v67;
  bb7:
    goto bb8;
  bb8:
    v70 = 8LL;
    v71 = p_add_i64(v1, v70);
    memset(v72, 0, 8);
    *(void**)v72 = (void*)v71;
    v74 = 0LL;
    memset(v75, 0, 8);
    *(int64_t*)v75 = (int64_t)v74;
    v78 = *(void**)v72;
    v79 = *(int64_t*)v78;
    *(int64_t*)v75 = (int64_t)v79;
    v82 = *(int64_t*)v75;
    v83 = *(int64_t*)v24;
    v84 = 8LL;
    v85 = p_mul_i64(v83, v84);
    v86 = p_add_i64(v82, v85);
    memset(v87, 0, 8);
    *(int64_t*)v87 = (int64_t)v86;
    v89 = *(int64_t*)v87;
    memset(v90, 0, 8);
    *(void**)v90 = (void*)v89;
    v92 = 0LL;
    memset(v93, 0, 8);
    *(int64_t*)v93 = (int64_t)v92;
    v96 = *(void**)v90;
    v97 = *(int64_t*)v96;
    *(int64_t*)v93 = (int64_t)v97;
    v100 = *(int64_t*)v93;
    return v100;
    return 0;
}

int64_t HashMap_get_i(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)hm_store_i((void*)v3, v1, v2);
    v5 = 0LL;
    v6 = (int64_t)HashMap_get((void*)v5, v1, v4);
    return v6;
    return 0;
}

int64_t HashMap_get_or(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    int64_t v4 = 0;
    int64_t v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0; /* placeholder for void */
    void* v12 = 0;
    int64_t v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    int64_t v17 = 0;
    bool v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    int64_t v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0;
    void* v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    void* v32 = 0;
    int64_t v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    void* v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    void* v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    void* v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    void* v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    bool v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    int64_t v61 = 0;
    bool v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0; /* placeholder for void */
    bool v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0;
    int64_t v70 = 0;
    void* v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0;
    void* v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0; /* placeholder for void */
    void* v77 = 0;
    int64_t v78 = 0;
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0;
    int64_t v82 = 0;
    int64_t v83 = 0;
    int64_t v84 = 0;
    int64_t v85 = 0;
    void* v86 = 0;
    int64_t v87 = 0; /* placeholder for void */
    int64_t v88 = 0;
    void* v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0;
    void* v92 = 0;
    int64_t v93 = 0; /* placeholder for void */
    int64_t v94 = 0; /* placeholder for void */
    void* v95 = 0;
    int64_t v96 = 0;
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0; /* placeholder for void */
    int64_t v99 = 0;
    int64_t v100 = 0; /* placeholder for void */
    v6 = alloca(8);
    v9 = alloca(8);
    v24 = alloca(8);
    v26 = alloca(8);
    v29 = alloca(8);
    v41 = alloca(8);
    v44 = alloca(8);
    v47 = alloca(8);
    v54 = alloca(1);
    v71 = alloca(8);
    v74 = alloca(8);
    v86 = alloca(8);
    v89 = alloca(8);
    v92 = alloca(8);
    v4 = 24LL;
    v5 = p_add_i64(v1, v4);
    memset(v6, 0, 8);
    *(void**)v6 = (void*)v5;
    v8 = 0LL;
    memset(v9, 0, 8);
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
    return v3;
  bb2:
    goto bb3;
  bb3:
    v22 = 0LL;
    v23 = (int64_t)HashMap_find_slot((void*)v22, v1, v2);
    memset(v24, 0, 8);
    *(int64_t*)v24 = (int64_t)v23;
    memset(v26, 0, 8);
    *(void**)v26 = (void*)v1;
    v28 = 0LL;
    memset(v29, 0, 8);
    *(int64_t*)v29 = (int64_t)v28;
    v32 = *(void**)v26;
    v33 = *(int64_t*)v32;
    *(int64_t*)v29 = (int64_t)v33;
    v36 = *(int64_t*)v29;
    v37 = *(int64_t*)v24;
    v38 = 8LL;
    v39 = p_mul_i64(v37, v38);
    v40 = p_add_i64(v36, v39);
    memset(v41, 0, 8);
    *(int64_t*)v41 = (int64_t)v40;
    v43 = *(int64_t*)v41;
    memset(v44, 0, 8);
    *(void**)v44 = (void*)v43;
    v46 = 0LL;
    memset(v47, 0, 8);
    *(int64_t*)v47 = (int64_t)v46;
    v50 = *(void**)v44;
    v51 = *(int64_t*)v50;
    *(int64_t*)v47 = (int64_t)v51;
    memset(v54, 0, 1);
    v55 = *(int64_t*)v47;
    v56 = 0LL;
    v57 = v55 == v56;
    *(bool*)v54 = v57;
    if (v57) goto bb5;
    else goto bb4;
  bb4:
    v60 = *(int64_t*)v47;
    v61 = -1LL;
    v62 = v60 == v61;
    *(bool*)v54 = v62;
    goto bb5;
  bb5:
    v65 = *(bool*)v54;
    if (v65) goto bb6;
    else goto bb7;
  bb6:
    return v3;
  bb7:
    goto bb8;
  bb8:
    v69 = 8LL;
    v70 = p_add_i64(v1, v69);
    memset(v71, 0, 8);
    *(void**)v71 = (void*)v70;
    v73 = 0LL;
    memset(v74, 0, 8);
    *(int64_t*)v74 = (int64_t)v73;
    v77 = *(void**)v71;
    v78 = *(int64_t*)v77;
    *(int64_t*)v74 = (int64_t)v78;
    v81 = *(int64_t*)v74;
    v82 = *(int64_t*)v24;
    v83 = 8LL;
    v84 = p_mul_i64(v82, v83);
    v85 = p_add_i64(v81, v84);
    memset(v86, 0, 8);
    *(int64_t*)v86 = (int64_t)v85;
    v88 = *(int64_t*)v86;
    memset(v89, 0, 8);
    *(void**)v89 = (void*)v88;
    v91 = 0LL;
    memset(v92, 0, 8);
    *(int64_t*)v92 = (int64_t)v91;
    v95 = *(void**)v89;
    v96 = *(int64_t*)v95;
    *(int64_t*)v92 = (int64_t)v96;
    v99 = *(int64_t*)v92;
    return v99;
    return 0;
}

int64_t HashMap_get_or_i(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    int64_t v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    int64_t v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    v4 = 0LL;
    v5 = (int64_t)hm_store_i((void*)v4, v1, v2);
    v6 = 0LL;
    v7 = (int64_t)HashMap_get_or((void*)v6, v1, v5, v3);
    return v7;
    return 0;
}

void HashMap_grow(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    int64_t v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0;
    int64_t v13 = 0;
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    void* v29 = 0;
    int64_t v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    void* v32 = 0;
    int64_t v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    void* v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0;
    bool v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    int64_t v50 = 0;
    int64_t v51 = 0;
    void* v52 = 0;
    void* v53 = 0;
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0;
    void* v58 = 0;
    void* v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0;
    void* v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0;
    int64_t v67 = 0;
    bool v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0;
    int64_t v71 = 0;
    int64_t v72 = 0;
    int64_t v73 = 0;
    int64_t v74 = 0;
    void* v75 = 0;
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0;
    void* v78 = 0;
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0;
    int64_t v81 = 0;
    int64_t v82 = 0;
    int64_t v83 = 0;
    int64_t v84 = 0;
    void* v85 = 0;
    int64_t v86 = 0; /* placeholder for void */
    int64_t v87 = 0;
    void* v88 = 0;
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0; /* placeholder for void */
    void* v91 = 0;
    int64_t v92 = 0;
    int64_t v93 = 0; /* placeholder for void */
    void* v94 = 0;
    int64_t v95 = 0;
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0;
    int64_t v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0; /* placeholder for void */
    int64_t v103 = 0;
    void* v104 = 0;
    int64_t v105 = 0; /* placeholder for void */
    int64_t v106 = 0;
    void* v107 = 0;
    int64_t v108 = 0; /* placeholder for void */
    int64_t v109 = 0; /* placeholder for void */
    int64_t v110 = 0; /* placeholder for void */
    int64_t v111 = 0;
    int64_t v112 = 0;
    bool v113 = 0;
    int64_t v114 = 0; /* placeholder for void */
    int64_t v115 = 0;
    int64_t v116 = 0;
    int64_t v117 = 0;
    int64_t v118 = 0;
    int64_t v119 = 0;
    void* v120 = 0;
    int64_t v121 = 0; /* placeholder for void */
    int64_t v122 = 0;
    void* v123 = 0;
    int64_t v124 = 0; /* placeholder for void */
    int64_t v125 = 0;
    void* v126 = 0;
    int64_t v127 = 0; /* placeholder for void */
    int64_t v128 = 0; /* placeholder for void */
    void* v129 = 0;
    int64_t v130 = 0;
    int64_t v131 = 0; /* placeholder for void */
    int64_t v132 = 0; /* placeholder for void */
    void* v133 = 0;
    int64_t v134 = 0;
    int64_t v135 = 0;
    bool v136 = 0;
    int64_t v137 = 0; /* placeholder for void */
    int64_t v138 = 0; /* placeholder for void */
    int64_t v139 = 0;
    int64_t v140 = 0;
    bool v141 = 0;
    int64_t v142 = 0; /* placeholder for void */
    int64_t v143 = 0; /* placeholder for void */
    bool v144 = 0;
    int64_t v145 = 0; /* placeholder for void */
    int64_t v146 = 0;
    int64_t v147 = 0;
    int64_t v148 = 0;
    int64_t v149 = 0;
    int64_t v150 = 0;
    void* v151 = 0;
    int64_t v152 = 0; /* placeholder for void */
    int64_t v153 = 0;
    void* v154 = 0;
    int64_t v155 = 0; /* placeholder for void */
    int64_t v156 = 0;
    void* v157 = 0;
    int64_t v158 = 0; /* placeholder for void */
    int64_t v159 = 0; /* placeholder for void */
    void* v160 = 0;
    int64_t v161 = 0;
    int64_t v162 = 0; /* placeholder for void */
    int64_t v163 = 0; /* placeholder for void */
    int64_t v164 = 0;
    int64_t v165 = 0;
    int64_t v166 = 0;
    void* v167 = 0;
    int64_t v168 = 0; /* placeholder for void */
    int64_t v169 = 0;
    int64_t v170 = 0;
    int64_t v171 = 0;
    void* v172 = 0;
    int64_t v173 = 0; /* placeholder for void */
    int64_t v174 = 0;
    void* v175 = 0;
    int64_t v176 = 0; /* placeholder for void */
    int64_t v177 = 0; /* placeholder for void */
    int64_t v178 = 0; /* placeholder for void */
    int64_t v179 = 0;
    int64_t v180 = 0;
    bool v181 = 0;
    int64_t v182 = 0; /* placeholder for void */
    int64_t v183 = 0;
    int64_t v184 = 0;
    int64_t v185 = 0;
    int64_t v186 = 0;
    int64_t v187 = 0;
    void* v188 = 0;
    int64_t v189 = 0; /* placeholder for void */
    int64_t v190 = 0;
    void* v191 = 0;
    int64_t v192 = 0; /* placeholder for void */
    int64_t v193 = 0;
    void* v194 = 0;
    int64_t v195 = 0; /* placeholder for void */
    int64_t v196 = 0; /* placeholder for void */
    void* v197 = 0;
    int64_t v198 = 0;
    int64_t v199 = 0; /* placeholder for void */
    int64_t v200 = 0; /* placeholder for void */
    int64_t v201 = 0;
    int64_t v202 = 0;
    bool v203 = 0;
    int64_t v204 = 0; /* placeholder for void */
    int64_t v205 = 0; /* placeholder for void */
    void* v206 = 0;
    int64_t v207 = 0;
    int64_t v208 = 0; /* placeholder for void */
    int64_t v209 = 0; /* placeholder for void */
    int64_t v210 = 0;
    int64_t v211 = 0;
    int64_t v212 = 0;
    int64_t v213 = 0;
    int64_t v214 = 0;
    void* v215 = 0;
    int64_t v216 = 0; /* placeholder for void */
    int64_t v217 = 0;
    void* v218 = 0;
    int64_t v219 = 0; /* placeholder for void */
    int64_t v220 = 0; /* placeholder for void */
    void* v221 = 0;
    int64_t v222 = 0;
    int64_t v223 = 0; /* placeholder for void */
    int64_t v224 = 0; /* placeholder for void */
    int64_t v225 = 0; /* placeholder for void */
    int64_t v226 = 0; /* placeholder for void */
    int64_t v227 = 0;
    int64_t v228 = 0;
    int64_t v229 = 0;
    int64_t v230 = 0; /* placeholder for void */
    int64_t v231 = 0;
    int64_t v232 = 0;
    bool v233 = 0;
    int64_t v234 = 0; /* placeholder for void */
    int64_t v235 = 0;
    int64_t v236 = 0; /* placeholder for void */
    int64_t v237 = 0; /* placeholder for void */
    int64_t v238 = 0; /* placeholder for void */
    int64_t v239 = 0;
    int64_t v240 = 0;
    int64_t v241 = 0;
    int64_t v242 = 0; /* placeholder for void */
    int64_t v243 = 0; /* placeholder for void */
    int64_t v244 = 0;
    int64_t v245 = 0;
    int64_t v246 = 0;
    int64_t v247 = 0; /* placeholder for void */
    int64_t v248 = 0; /* placeholder for void */
    int64_t v249 = 0; /* placeholder for void */
    int64_t v250 = 0;
    int64_t v251 = 0;
    int64_t v252 = 0;
    int64_t v253 = 0; /* placeholder for void */
    int64_t v254 = 0; /* placeholder for void */
    int64_t v255 = 0;
    int64_t v256 = 0;
    bool v257 = 0;
    int64_t v258 = 0; /* placeholder for void */
    int64_t v259 = 0;
    int64_t v260 = 0; /* placeholder for void */
    int64_t v261 = 0; /* placeholder for void */
    int64_t v262 = 0; /* placeholder for void */
    int64_t v263 = 0;
    int64_t v264 = 0;
    bool v265 = 0;
    int64_t v266 = 0; /* placeholder for void */
    int64_t v267 = 0;
    int64_t v268 = 0; /* placeholder for void */
    int64_t v269 = 0; /* placeholder for void */
    int64_t v270 = 0; /* placeholder for void */
    int64_t v271 = 0; /* placeholder for void */
    void* v272 = 0;
    int64_t v273 = 0;
    int64_t v274 = 0; /* placeholder for void */
    void* v275 = 0;
    int64_t v276 = 0;
    int64_t v277 = 0; /* placeholder for void */
    void* v278 = 0;
    int64_t v279 = 0;
    int64_t v280 = 0; /* placeholder for void */
    void* v281 = 0;
    int64_t v282 = 0;
    int64_t v283 = 0; /* placeholder for void */
    int64_t v284 = 0; /* placeholder for void */
    int64_t v285 = 0; /* placeholder for void */
    v4 = alloca(8);
    v6 = alloca(8);
    v10 = alloca(8);
    v14 = alloca(8);
    v17 = alloca(8);
    v20 = alloca(8);
    v23 = alloca(8);
    v37 = alloca(8);
    v53 = alloca(8);
    v59 = alloca(8);
    v62 = alloca(8);
    v75 = alloca(8);
    v78 = alloca(8);
    v85 = alloca(8);
    v88 = alloca(8);
    v104 = alloca(8);
    v107 = alloca(8);
    v120 = alloca(8);
    v123 = alloca(8);
    v126 = alloca(8);
    v133 = alloca(1);
    v151 = alloca(8);
    v154 = alloca(8);
    v157 = alloca(8);
    v167 = alloca(8);
    v172 = alloca(8);
    v175 = alloca(8);
    v188 = alloca(8);
    v191 = alloca(8);
    v194 = alloca(8);
    v215 = alloca(8);
    v218 = alloca(8);
    v2 = 24LL;
    v3 = (int64_t)((uint64_t)v1 + (uint64_t)v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    memset(v6, 0, 8);
    *(void**)v6 = (void*)v1;
    v8 = 8LL;
    v9 = (int64_t)((uint64_t)v1 + (uint64_t)v8);
    memset(v10, 0, 8);
    *(void**)v10 = (void*)v9;
    v12 = 16LL;
    v13 = (int64_t)((uint64_t)v1 + (uint64_t)v12);
    memset(v14, 0, 8);
    *(void**)v14 = (void*)v13;
    v16 = 0LL;
    memset(v17, 0, 8);
    *(int64_t*)v17 = (int64_t)v16;
    v19 = 0LL;
    memset(v20, 0, 8);
    *(int64_t*)v20 = (int64_t)v19;
    v22 = 0LL;
    memset(v23, 0, 8);
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
    memset(v37, 0, 8);
    *(int64_t*)v37 = (int64_t)v36;
    v39 = *(int64_t*)v17;
    v40 = 0LL;
    v41 = v39 > v40;
    if (v41) goto bb1;
    else goto bb2;
  bb1:
    v43 = *(int64_t*)v17;
    v44 = 2LL;
    v45 = (int64_t)((uint64_t)v43 * (uint64_t)v44);
    *(int64_t*)v37 = (int64_t)v45;
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v49 = *(int64_t*)v37;
    v50 = 8LL;
    v51 = (int64_t)((uint64_t)v49 * (uint64_t)v50);
    v52 = p_malloc(v51);
    memset(v53, 0, 8);
    *(int64_t*)v53 = (int64_t)v52;
    v55 = *(int64_t*)v37;
    v56 = 8LL;
    v57 = (int64_t)((uint64_t)v55 * (uint64_t)v56);
    v58 = p_malloc(v57);
    memset(v59, 0, 8);
    *(int64_t*)v59 = (int64_t)v58;
    v61 = 0LL;
    memset(v62, 0, 8);
    *(int64_t*)v62 = (int64_t)v61;
    goto bb4;
  bb4:
    p_co_tick();
    v66 = *(int64_t*)v62;
    v67 = *(int64_t*)v37;
    v68 = v66 < v67;
    if (v68) goto bb5;
    else goto bb6;
  bb5:
    v70 = *(int64_t*)v53;
    v71 = *(int64_t*)v62;
    v72 = 8LL;
    v73 = (int64_t)((uint64_t)v71 * (uint64_t)v72);
    v74 = (int64_t)((uint64_t)v70 + (uint64_t)v73);
    memset(v75, 0, 8);
    *(int64_t*)v75 = (int64_t)v74;
    v77 = *(int64_t*)v75;
    memset(v78, 0, 8);
    *(void**)v78 = (void*)v77;
    v80 = *(int64_t*)v59;
    v81 = *(int64_t*)v62;
    v82 = 8LL;
    v83 = (int64_t)((uint64_t)v81 * (uint64_t)v82);
    v84 = (int64_t)((uint64_t)v80 + (uint64_t)v83);
    memset(v85, 0, 8);
    *(int64_t*)v85 = (int64_t)v84;
    v87 = *(int64_t*)v85;
    memset(v88, 0, 8);
    *(void**)v88 = (void*)v87;
    v91 = *(void**)v78;
    v92 = 0LL;
    *(int64_t*)v91 = (int64_t)v92;
    v94 = *(void**)v88;
    v95 = 0LL;
    *(int64_t*)v94 = (int64_t)v95;
    v98 = *(int64_t*)v62;
    v99 = 1LL;
    v100 = (int64_t)((uint64_t)v98 + (uint64_t)v99);
    *(int64_t*)v62 = (int64_t)v100;
    goto bb4;
  bb6:
    v103 = 0LL;
    memset(v104, 0, 8);
    *(int64_t*)v104 = (int64_t)v103;
    v106 = 0LL;
    memset(v107, 0, 8);
    *(int64_t*)v107 = (int64_t)v106;
    goto bb7;
  bb7:
    p_co_tick();
    v111 = *(int64_t*)v107;
    v112 = *(int64_t*)v17;
    v113 = v111 < v112;
    if (v113) goto bb8;
    else goto bb9;
  bb8:
    v115 = *(int64_t*)v20;
    v116 = *(int64_t*)v107;
    v117 = 8LL;
    v118 = (int64_t)((uint64_t)v116 * (uint64_t)v117);
    v119 = (int64_t)((uint64_t)v115 + (uint64_t)v118);
    memset(v120, 0, 8);
    *(int64_t*)v120 = (int64_t)v119;
    v122 = *(int64_t*)v120;
    memset(v123, 0, 8);
    *(void**)v123 = (void*)v122;
    v125 = 0LL;
    memset(v126, 0, 8);
    *(int64_t*)v126 = (int64_t)v125;
    v129 = *(void**)v123;
    v130 = *(int64_t*)v129;
    *(int64_t*)v126 = (int64_t)v130;
    memset(v133, 0, 1);
    v134 = *(int64_t*)v126;
    v135 = 0LL;
    v136 = v134 != v135;
    *(bool*)v133 = v136;
    if (v136) goto bb10;
    else goto bb11;
  bb9:
    v255 = *(int64_t*)v20;
    v256 = 0LL;
    v257 = v255 != v256;
    if (v257) goto bb24;
    else goto bb25;
  bb10:
    v139 = *(int64_t*)v126;
    v140 = -1LL;
    v141 = v139 != v140;
    *(bool*)v133 = v141;
    goto bb11;
  bb11:
    v144 = *(bool*)v133;
    if (v144) goto bb12;
    else goto bb13;
  bb12:
    v146 = *(int64_t*)v23;
    v147 = *(int64_t*)v107;
    v148 = 8LL;
    v149 = (int64_t)((uint64_t)v147 * (uint64_t)v148);
    v150 = (int64_t)((uint64_t)v146 + (uint64_t)v149);
    memset(v151, 0, 8);
    *(int64_t*)v151 = (int64_t)v150;
    v153 = *(int64_t*)v151;
    memset(v154, 0, 8);
    *(void**)v154 = (void*)v153;
    v156 = 0LL;
    memset(v157, 0, 8);
    *(int64_t*)v157 = (int64_t)v156;
    v160 = *(void**)v154;
    v161 = *(int64_t*)v160;
    *(int64_t*)v157 = (int64_t)v161;
    v164 = *(int64_t*)v126;
    v165 = 0LL;
    v166 = (int64_t)hm_hash((void*)v165, v1, v164);
    memset(v167, 0, 8);
    *(int64_t*)v167 = (int64_t)v166;
    v169 = *(int64_t*)v167;
    v170 = *(int64_t*)v37;
    v171 = p_mod_i64(v169, v170);
    memset(v172, 0, 8);
    *(int64_t*)v172 = (int64_t)v171;
    v174 = 0LL;
    memset(v175, 0, 8);
    *(int64_t*)v175 = (int64_t)v174;
    goto bb15;
  bb13:
    goto bb14;
  bb14:
    v250 = *(int64_t*)v107;
    v251 = 1LL;
    v252 = (int64_t)((uint64_t)v250 + (uint64_t)v251);
    *(int64_t*)v107 = (int64_t)v252;
    goto bb7;
  bb15:
    p_co_tick();
    v179 = *(int64_t*)v175;
    v180 = *(int64_t*)v37;
    v181 = v179 < v180;
    if (v181) goto bb16;
    else goto bb17;
  bb16:
    v183 = *(int64_t*)v53;
    v184 = *(int64_t*)v172;
    v185 = 8LL;
    v186 = (int64_t)((uint64_t)v184 * (uint64_t)v185);
    v187 = (int64_t)((uint64_t)v183 + (uint64_t)v186);
    memset(v188, 0, 8);
    *(int64_t*)v188 = (int64_t)v187;
    v190 = *(int64_t*)v188;
    memset(v191, 0, 8);
    *(void**)v191 = (void*)v190;
    v193 = 0LL;
    memset(v194, 0, 8);
    *(int64_t*)v194 = (int64_t)v193;
    v197 = *(void**)v191;
    v198 = *(int64_t*)v197;
    *(int64_t*)v194 = (int64_t)v198;
    v201 = *(int64_t*)v194;
    v202 = 0LL;
    v203 = v201 == v202;
    if (v203) goto bb18;
    else goto bb19;
  bb17:
    v244 = *(int64_t*)v104;
    v245 = 1LL;
    v246 = (int64_t)((uint64_t)v244 + (uint64_t)v245);
    *(int64_t*)v104 = (int64_t)v246;
    goto bb14;
  bb18:
    v206 = *(void**)v191;
    v207 = *(int64_t*)v126;
    *(int64_t*)v206 = (int64_t)v207;
    v210 = *(int64_t*)v59;
    v211 = *(int64_t*)v172;
    v212 = 8LL;
    v213 = (int64_t)((uint64_t)v211 * (uint64_t)v212);
    v214 = (int64_t)((uint64_t)v210 + (uint64_t)v213);
    memset(v215, 0, 8);
    *(int64_t*)v215 = (int64_t)v214;
    v217 = *(int64_t*)v215;
    memset(v218, 0, 8);
    *(void**)v218 = (void*)v217;
    v221 = *(void**)v218;
    v222 = *(int64_t*)v157;
    *(int64_t*)v221 = (int64_t)v222;
    goto bb17;
  bb19:
    goto bb20;
  bb20:
    v227 = *(int64_t*)v172;
    v228 = 1LL;
    v229 = (int64_t)((uint64_t)v227 + (uint64_t)v228);
    *(int64_t*)v172 = (int64_t)v229;
    v231 = *(int64_t*)v172;
    v232 = *(int64_t*)v37;
    v233 = v231 >= v232;
    if (v233) goto bb21;
    else goto bb22;
  bb21:
    v235 = 0LL;
    *(int64_t*)v172 = (int64_t)v235;
    goto bb23;
  bb22:
    goto bb23;
  bb23:
    v239 = *(int64_t*)v175;
    v240 = 1LL;
    v241 = (int64_t)((uint64_t)v239 + (uint64_t)v240);
    *(int64_t*)v175 = (int64_t)v241;
    goto bb15;
  bb24:
    v259 = *(int64_t*)v20;
    p_free((void*)v259);
    goto bb26;
  bb25:
    goto bb26;
  bb26:
    v263 = *(int64_t*)v23;
    v264 = 0LL;
    v265 = v263 != v264;
    if (v265) goto bb27;
    else goto bb28;
  bb27:
    v267 = *(int64_t*)v23;
    p_free((void*)v267);
    goto bb29;
  bb28:
    goto bb29;
  bb29:
    v272 = *(void**)v6;
    v273 = *(int64_t*)v53;
    *(int64_t*)v272 = (int64_t)v273;
    v275 = *(void**)v10;
    v276 = *(int64_t*)v59;
    *(int64_t*)v275 = (int64_t)v276;
    v278 = *(void**)v14;
    v279 = *(int64_t*)v104;
    *(int64_t*)v278 = (int64_t)v279;
    v281 = *(void**)v4;
    v282 = *(int64_t*)v37;
    *(int64_t*)v281 = (int64_t)v282;
    return;
    return;
}

void HashMap_insert(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    int64_t v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    int64_t v7 = 0;
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0;
    int64_t v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    void* v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    void* v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    void* v21 = 0;
    int64_t v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    void* v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    void* v28 = 0;
    int64_t v29 = 0;
    int64_t v30 = 0;
    bool v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0;
    bool v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0; /* placeholder for void */
    bool v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    void* v48 = 0;
    int64_t v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    int64_t v55 = 0;
    void* v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    void* v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    int64_t v61 = 0;
    void* v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0;
    void* v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0;
    void* v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0; /* placeholder for void */
    void* v71 = 0;
    int64_t v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    void* v74 = 0;
    int64_t v75 = 0;
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0; /* placeholder for void */
    int64_t v78 = 0;
    int64_t v79 = 0;
    int64_t v80 = 0;
    int64_t v81 = 0;
    int64_t v82 = 0;
    void* v83 = 0;
    int64_t v84 = 0; /* placeholder for void */
    int64_t v85 = 0;
    void* v86 = 0;
    int64_t v87 = 0; /* placeholder for void */
    int64_t v88 = 0;
    void* v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0; /* placeholder for void */
    void* v92 = 0;
    int64_t v93 = 0;
    int64_t v94 = 0; /* placeholder for void */
    int64_t v95 = 0; /* placeholder for void */
    void* v96 = 0;
    int64_t v97 = 0;
    int64_t v98 = 0;
    bool v99 = 0;
    int64_t v100 = 0; /* placeholder for void */
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0;
    int64_t v103 = 0;
    bool v104 = 0;
    int64_t v105 = 0; /* placeholder for void */
    int64_t v106 = 0; /* placeholder for void */
    bool v107 = 0;
    int64_t v108 = 0; /* placeholder for void */
    int64_t v109 = 0; /* placeholder for void */
    void* v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    int64_t v112 = 0; /* placeholder for void */
    int64_t v113 = 0;
    int64_t v114 = 0;
    int64_t v115 = 0;
    int64_t v116 = 0;
    int64_t v117 = 0;
    void* v118 = 0;
    int64_t v119 = 0; /* placeholder for void */
    int64_t v120 = 0;
    void* v121 = 0;
    int64_t v122 = 0; /* placeholder for void */
    int64_t v123 = 0; /* placeholder for void */
    void* v124 = 0;
    int64_t v125 = 0; /* placeholder for void */
    int64_t v126 = 0; /* placeholder for void */
    int64_t v127 = 0;
    int64_t v128 = 0;
    int64_t v129 = 0;
    void* v130 = 0;
    int64_t v131 = 0; /* placeholder for void */
    int64_t v132 = 0; /* placeholder for void */
    void* v133 = 0;
    int64_t v134 = 0;
    int64_t v135 = 0; /* placeholder for void */
    int64_t v136 = 0; /* placeholder for void */
    int64_t v137 = 0; /* placeholder for void */
    int64_t v138 = 0;
    int64_t v139 = 0;
    int64_t v140 = 0;
    int64_t v141 = 0;
    int64_t v142 = 0;
    void* v143 = 0;
    int64_t v144 = 0; /* placeholder for void */
    int64_t v145 = 0;
    void* v146 = 0;
    int64_t v147 = 0; /* placeholder for void */
    int64_t v148 = 0; /* placeholder for void */
    void* v149 = 0;
    int64_t v150 = 0; /* placeholder for void */
    int64_t v151 = 0; /* placeholder for void */
    int64_t v152 = 0; /* placeholder for void */
    int64_t v153 = 0; /* placeholder for void */
    v8 = alloca(8);
    v12 = alloca(8);
    v15 = alloca(8);
    v18 = alloca(8);
    v28 = alloca(1);
    v56 = alloca(8);
    v58 = alloca(8);
    v62 = alloca(8);
    v65 = alloca(8);
    v68 = alloca(8);
    v83 = alloca(8);
    v86 = alloca(8);
    v89 = alloca(8);
    v96 = alloca(1);
    v118 = alloca(8);
    v121 = alloca(8);
    v130 = alloca(8);
    v143 = alloca(8);
    v146 = alloca(8);
    v4 = 0LL;
    map_own((void*)v4, v1);
    v6 = 16LL;
    v7 = p_add_i64(v1, v6);
    memset(v8, 0, 8);
    *(void**)v8 = (void*)v7;
    v10 = 24LL;
    v11 = p_add_i64(v1, v10);
    memset(v12, 0, 8);
    *(void**)v12 = (void*)v11;
    v14 = 0LL;
    memset(v15, 0, 8);
    *(int64_t*)v15 = (int64_t)v14;
    v17 = 0LL;
    memset(v18, 0, 8);
    *(int64_t*)v18 = (int64_t)v17;
    v21 = *(void**)v8;
    v22 = *(int64_t*)v21;
    *(int64_t*)v15 = (int64_t)v22;
    v24 = *(void**)v12;
    v25 = *(int64_t*)v24;
    *(int64_t*)v18 = (int64_t)v25;
    memset(v28, 0, 1);
    v29 = *(int64_t*)v18;
    v30 = 0LL;
    v31 = v29 == v30;
    *(bool*)v28 = v31;
    if (v31) goto bb2;
    else goto bb1;
  bb1:
    v34 = *(int64_t*)v15;
    v35 = 10LL;
    v36 = p_mul_i64(v34, v35);
    v37 = *(int64_t*)v18;
    v38 = 7LL;
    v39 = p_mul_i64(v37, v38);
    v40 = v36 >= v39;
    *(bool*)v28 = v40;
    goto bb2;
  bb2:
    v43 = *(bool*)v28;
    if (v43) goto bb3;
    else goto bb4;
  bb3:
    v45 = 0LL;
    HashMap_grow((void*)v45, v1);
    v48 = *(void**)v12;
    v49 = *(int64_t*)v48;
    *(int64_t*)v18 = (int64_t)v49;
    goto bb5;
  bb4:
    goto bb5;
  bb5:
    v54 = 0LL;
    v55 = (int64_t)HashMap_find_slot((void*)v54, v1, v2);
    memset(v56, 0, 8);
    *(int64_t*)v56 = (int64_t)v55;
    memset(v58, 0, 8);
    *(void**)v58 = (void*)v1;
    v60 = 8LL;
    v61 = p_add_i64(v1, v60);
    memset(v62, 0, 8);
    *(void**)v62 = (void*)v61;
    v64 = 0LL;
    memset(v65, 0, 8);
    *(int64_t*)v65 = (int64_t)v64;
    v67 = 0LL;
    memset(v68, 0, 8);
    *(int64_t*)v68 = (int64_t)v67;
    v71 = *(void**)v58;
    v72 = *(int64_t*)v71;
    *(int64_t*)v65 = (int64_t)v72;
    v74 = *(void**)v62;
    v75 = *(int64_t*)v74;
    *(int64_t*)v68 = (int64_t)v75;
    v78 = *(int64_t*)v65;
    v79 = *(int64_t*)v56;
    v80 = 8LL;
    v81 = p_mul_i64(v79, v80);
    v82 = p_add_i64(v78, v81);
    memset(v83, 0, 8);
    *(int64_t*)v83 = (int64_t)v82;
    v85 = *(int64_t*)v83;
    memset(v86, 0, 8);
    *(void**)v86 = (void*)v85;
    v88 = 0LL;
    memset(v89, 0, 8);
    *(int64_t*)v89 = (int64_t)v88;
    v92 = *(void**)v86;
    v93 = *(int64_t*)v92;
    *(int64_t*)v89 = (int64_t)v93;
    memset(v96, 0, 1);
    v97 = *(int64_t*)v89;
    v98 = 0LL;
    v99 = v97 == v98;
    *(bool*)v96 = v99;
    if (v99) goto bb7;
    else goto bb6;
  bb6:
    v102 = *(int64_t*)v89;
    v103 = -1LL;
    v104 = v102 == v103;
    *(bool*)v96 = v104;
    goto bb7;
  bb7:
    v107 = *(bool*)v96;
    if (v107) goto bb8;
    else goto bb9;
  bb8:
    v110 = *(void**)v86;
    *(int64_t*)v110 = (int64_t)v2;
    v113 = *(int64_t*)v68;
    v114 = *(int64_t*)v56;
    v115 = 8LL;
    v116 = p_mul_i64(v114, v115);
    v117 = p_add_i64(v113, v116);
    memset(v118, 0, 8);
    *(int64_t*)v118 = (int64_t)v117;
    v120 = *(int64_t*)v118;
    memset(v121, 0, 8);
    *(void**)v121 = (void*)v120;
    v124 = *(void**)v121;
    *(int64_t*)v124 = (int64_t)v3;
    v127 = *(int64_t*)v15;
    v128 = 1LL;
    v129 = p_add_i64(v127, v128);
    memset(v130, 0, 8);
    *(int64_t*)v130 = (int64_t)v129;
    v133 = *(void**)v8;
    v134 = *(int64_t*)v130;
    *(int64_t*)v133 = (int64_t)v134;
    goto bb10;
  bb9:
    v138 = *(int64_t*)v68;
    v139 = *(int64_t*)v56;
    v140 = 8LL;
    v141 = p_mul_i64(v139, v140);
    v142 = p_add_i64(v138, v141);
    memset(v143, 0, 8);
    *(int64_t*)v143 = (int64_t)v142;
    v145 = *(int64_t*)v143;
    memset(v146, 0, 8);
    *(void**)v146 = (void*)v145;
    v149 = *(void**)v146;
    *(int64_t*)v149 = (int64_t)v3;
    goto bb10;
  bb10:
    return;
    return;
}

void HashMap_insert_i(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    int64_t v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0; /* placeholder for void */
    v4 = 0LL;
    v5 = (int64_t)hm_store_i((void*)v4, v1, v2);
    v6 = 0LL;
    HashMap_insert((void*)v6, v1, v5, v3);
    return;
    return;
}

bool HashMap_is_empty(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    int64_t v15 = 0;
    bool v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    v4 = alloca(8);
    v7 = alloca(8);
    v2 = 16LL;
    v3 = p_add_i64(v1, v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    v6 = 0LL;
    memset(v7, 0, 8);
    *(int64_t*)v7 = (int64_t)v6;
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v7 = (int64_t)v11;
    v14 = *(int64_t*)v7;
    v15 = 0LL;
    v16 = v14 == v15;
    return v16;
    return 0;
}

int64_t HashMap_keys(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    int64_t v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    bool v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    int64_t v33 = 0;
    bool v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0; /* placeholder for void */
    bool v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0;
    void* v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0;
    int64_t v48 = 0;
    bool v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0;
    int64_t v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    void* v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    void* v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0;
    void* v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0; /* placeholder for void */
    void* v65 = 0;
    int64_t v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0; /* placeholder for void */
    void* v69 = 0;
    int64_t v70 = 0;
    int64_t v71 = 0;
    bool v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0;
    int64_t v76 = 0;
    bool v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0; /* placeholder for void */
    bool v80 = 0;
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0;
    bool v83 = 0;
    int64_t v84 = 0; /* placeholder for void */
    int64_t v85 = 0;
    int64_t v86 = 0;
    int64_t v87 = 0;
    int64_t v88 = 0;
    int64_t v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0;
    int64_t v93 = 0;
    int64_t v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0; /* placeholder for void */
    int64_t v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0;
    int64_t v102 = 0; /* placeholder for void */
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0;
    int64_t v105 = 0;
    int64_t v106 = 0; /* placeholder for void */
    int64_t v107 = 0;
    int64_t v108 = 0; /* placeholder for void */
    v4 = alloca(8);
    v6 = alloca(8);
    v9 = alloca(8);
    v12 = alloca(8);
    v24 = alloca(8);
    v26 = alloca(1);
    v43 = alloca(8);
    v56 = alloca(8);
    v59 = alloca(8);
    v62 = alloca(8);
    v69 = alloca(1);
    v2 = 24LL;
    v3 = p_add_i64(v1, v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    memset(v6, 0, 8);
    *(void**)v6 = (void*)v1;
    v8 = 0LL;
    memset(v9, 0, 8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 0LL;
    memset(v12, 0, 8);
    *(int64_t*)v12 = (int64_t)v11;
    v15 = *(void**)v4;
    v16 = *(int64_t*)v15;
    *(int64_t*)v9 = (int64_t)v16;
    v18 = *(void**)v6;
    v19 = *(int64_t*)v18;
    *(int64_t*)v12 = (int64_t)v19;
    v22 = 0LL;
    v23 = (int64_t)Vec_new((void*)v22);
    memset(v24, 0, 8);
    *(int64_t*)v24 = (int64_t)v23;
    memset(v26, 0, 1);
    v27 = *(int64_t*)v12;
    v28 = 0LL;
    v29 = v27 == v28;
    *(bool*)v26 = v29;
    if (v29) goto bb2;
    else goto bb1;
  bb1:
    v32 = *(int64_t*)v9;
    v33 = 0LL;
    v34 = v32 == v33;
    *(bool*)v26 = v34;
    goto bb2;
  bb2:
    v37 = *(bool*)v26;
    if (v37) goto bb3;
    else goto bb4;
  bb3:
    v39 = *(int64_t*)v24;
    return v39;
  bb4:
    goto bb5;
  bb5:
    v42 = 0LL;
    memset(v43, 0, 8);
    *(int64_t*)v43 = (int64_t)v42;
    goto bb6;
  bb6:
    p_co_tick();
    v47 = *(int64_t*)v43;
    v48 = *(int64_t*)v9;
    v49 = v47 < v48;
    if (v49) goto bb7;
    else goto bb8;
  bb7:
    v51 = *(int64_t*)v12;
    v52 = *(int64_t*)v43;
    v53 = 8LL;
    v54 = p_mul_i64(v52, v53);
    v55 = p_add_i64(v51, v54);
    memset(v56, 0, 8);
    *(int64_t*)v56 = (int64_t)v55;
    v58 = *(int64_t*)v56;
    memset(v59, 0, 8);
    *(void**)v59 = (void*)v58;
    v61 = 0LL;
    memset(v62, 0, 8);
    *(int64_t*)v62 = (int64_t)v61;
    v65 = *(void**)v59;
    v66 = *(int64_t*)v65;
    *(int64_t*)v62 = (int64_t)v66;
    memset(v69, 0, 1);
    v70 = *(int64_t*)v62;
    v71 = 0LL;
    v72 = v70 != v71;
    *(bool*)v69 = v72;
    if (v72) goto bb9;
    else goto bb10;
  bb8:
    v104 = *(int64_t*)v24;
    v105 = 0LL;
    Vec_sort((void*)v105, v104);
    v107 = *(int64_t*)v24;
    return v107;
  bb9:
    v75 = *(int64_t*)v62;
    v76 = -1LL;
    v77 = v75 != v76;
    *(bool*)v69 = v77;
    goto bb10;
  bb10:
    v80 = *(bool*)v69;
    if (v80) goto bb11;
    else goto bb12;
  bb11:
    v82 = 0LL;
    v83 = (bool)hm_is_int((void*)v82, v1);
    if (v83) goto bb14;
    else goto bb15;
  bb12:
    goto bb13;
  bb13:
    v99 = *(int64_t*)v43;
    v100 = 1LL;
    v101 = (int64_t)((uint64_t)v99 + (uint64_t)v100);
    *(int64_t*)v43 = (int64_t)v101;
    goto bb6;
  bb14:
    v85 = *(int64_t*)v24;
    v86 = *(int64_t*)v62;
    v87 = 6768574230975169895LL;
    v88 = v86 ^ v87;
    v89 = 0LL;
    Vec_push((void*)v89, v85, v88);
    goto bb16;
  bb15:
    v92 = *(int64_t*)v24;
    v93 = *(int64_t*)v62;
    v94 = 0LL;
    Vec_push((void*)v94, v92, v93);
    goto bb16;
  bb16:
    goto bb13;
    return 0;
}

int64_t HashMap_keys_i(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)HashMap_keys((void*)v2, v1);
    return v3;
    return 0;
}

int64_t HashMap_len(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    v4 = alloca(8);
    v7 = alloca(8);
    v2 = 16LL;
    v3 = p_add_i64(v1, v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    v6 = 0LL;
    memset(v7, 0, 8);
    *(int64_t*)v7 = (int64_t)v6;
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v7 = (int64_t)v11;
    v14 = *(int64_t*)v7;
    return v14;
    return 0;
}

int64_t HashMap_new(void* v0) {
    int64_t v1 = 0;
    void* v2 = 0;
    void* v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0;
    int64_t v11 = 0;
    bool v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    int64_t v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    v3 = alloca(8);
    v6 = alloca(8);
    v19 = alloca(8);
    v1 = 48LL;
    v2 = p_malloc(v1);
    memset(v3, 0, 8);
    *(int64_t*)v3 = (int64_t)v2;
    v5 = 0LL;
    memset(v6, 0, 8);
    *(int64_t*)v6 = (int64_t)v5;
    goto bb1;
  bb1:
    p_co_tick();
    v10 = *(int64_t*)v6;
    v11 = 6LL;
    v12 = v10 < v11;
    if (v12) goto bb2;
    else goto bb3;
  bb2:
    v14 = *(int64_t*)v3;
    v15 = *(int64_t*)v6;
    v16 = 8LL;
    v17 = p_mul_i64(v15, v16);
    v18 = p_add_i64(v14, v17);
    memset(v19, 0, 8);
    *(void**)v19 = (void*)v18;
    v22 = *(void**)v19;
    v23 = 0LL;
    *(int64_t*)v22 = (int64_t)v23;
    v26 = *(int64_t*)v6;
    v27 = 1LL;
    v28 = (int64_t)((uint64_t)v26 + (uint64_t)v27);
    *(int64_t*)v6 = (int64_t)v28;
    goto bb1;
  bb3:
    v31 = *(int64_t*)v3;
    return v31;
    return 0;
}

void HashMap_remove(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    int64_t v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    int64_t v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    int64_t v22 = 0;
    bool v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    int64_t v28 = 0;
    void* v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    void* v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0;
    void* v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0; /* placeholder for void */
    void* v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    void* v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    void* v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0;
    void* v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0; /* placeholder for void */
    void* v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0; /* placeholder for void */
    void* v59 = 0;
    int64_t v60 = 0;
    int64_t v61 = 0;
    bool v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0;
    int64_t v66 = 0;
    bool v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0; /* placeholder for void */
    bool v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0; /* placeholder for void */
    int64_t v74 = 0; /* placeholder for void */
    void* v75 = 0;
    int64_t v76 = 0;
    int64_t v77 = 0; /* placeholder for void */
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0;
    void* v80 = 0;
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0; /* placeholder for void */
    void* v83 = 0;
    int64_t v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0; /* placeholder for void */
    int64_t v87 = 0;
    int64_t v88 = 0;
    int64_t v89 = 0;
    void* v90 = 0;
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0; /* placeholder for void */
    void* v93 = 0;
    int64_t v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0; /* placeholder for void */
    v7 = alloca(8);
    v11 = alloca(8);
    v14 = alloca(8);
    v29 = alloca(8);
    v31 = alloca(8);
    v34 = alloca(8);
    v46 = alloca(8);
    v49 = alloca(8);
    v52 = alloca(8);
    v59 = alloca(1);
    v80 = alloca(8);
    v90 = alloca(8);
    v3 = 0LL;
    map_own((void*)v3, v1);
    v5 = 24LL;
    v6 = p_add_i64(v1, v5);
    memset(v7, 0, 8);
    *(void**)v7 = (void*)v6;
    v9 = 16LL;
    v10 = p_add_i64(v1, v9);
    memset(v11, 0, 8);
    *(void**)v11 = (void*)v10;
    v13 = 0LL;
    memset(v14, 0, 8);
    *(int64_t*)v14 = (int64_t)v13;
    v17 = *(void**)v7;
    v18 = *(int64_t*)v17;
    *(int64_t*)v14 = (int64_t)v18;
    v21 = *(int64_t*)v14;
    v22 = 0LL;
    v23 = v21 == v22;
    if (v23) goto bb1;
    else goto bb2;
  bb1:
    return;
  bb2:
    goto bb3;
  bb3:
    v27 = 0LL;
    v28 = (int64_t)HashMap_find_slot((void*)v27, v1, v2);
    memset(v29, 0, 8);
    *(int64_t*)v29 = (int64_t)v28;
    memset(v31, 0, 8);
    *(void**)v31 = (void*)v1;
    v33 = 0LL;
    memset(v34, 0, 8);
    *(int64_t*)v34 = (int64_t)v33;
    v37 = *(void**)v31;
    v38 = *(int64_t*)v37;
    *(int64_t*)v34 = (int64_t)v38;
    v41 = *(int64_t*)v34;
    v42 = *(int64_t*)v29;
    v43 = 8LL;
    v44 = p_mul_i64(v42, v43);
    v45 = p_add_i64(v41, v44);
    memset(v46, 0, 8);
    *(int64_t*)v46 = (int64_t)v45;
    v48 = *(int64_t*)v46;
    memset(v49, 0, 8);
    *(void**)v49 = (void*)v48;
    v51 = 0LL;
    memset(v52, 0, 8);
    *(int64_t*)v52 = (int64_t)v51;
    v55 = *(void**)v49;
    v56 = *(int64_t*)v55;
    *(int64_t*)v52 = (int64_t)v56;
    memset(v59, 0, 1);
    v60 = *(int64_t*)v52;
    v61 = 0LL;
    v62 = v60 == v61;
    *(bool*)v59 = v62;
    if (v62) goto bb5;
    else goto bb4;
  bb4:
    v65 = *(int64_t*)v52;
    v66 = -1LL;
    v67 = v65 == v66;
    *(bool*)v59 = v67;
    goto bb5;
  bb5:
    v70 = *(bool*)v59;
    if (v70) goto bb6;
    else goto bb7;
  bb6:
    return;
  bb7:
    goto bb8;
  bb8:
    v75 = *(void**)v49;
    v76 = -1LL;
    *(int64_t*)v75 = (int64_t)v76;
    v79 = 0LL;
    memset(v80, 0, 8);
    *(int64_t*)v80 = (int64_t)v79;
    v83 = *(void**)v11;
    v84 = *(int64_t*)v83;
    *(int64_t*)v80 = (int64_t)v84;
    v87 = *(int64_t*)v80;
    v88 = 1LL;
    v89 = p_sub_i64(v87, v88);
    memset(v90, 0, 8);
    *(int64_t*)v90 = (int64_t)v89;
    v93 = *(void**)v11;
    v94 = *(int64_t*)v90;
    *(int64_t*)v93 = (int64_t)v94;
    return;
    return;
}

void HashMap_remove_i(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)hm_store_i((void*)v3, v1, v2);
    v5 = 0LL;
    HashMap_remove((void*)v5, v1, v4);
    return;
    return;
}

int64_t HashMap_share(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    void* v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    int64_t v17 = 0;
    bool v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    int64_t v21 = 0;
    int64_t v22 = 0;
    int64_t v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0;
    int64_t v30 = 0;
    void* v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    void* v34 = 0;
    void* v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0;
    int64_t v45 = 0;
    void* v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    int64_t v49 = 0;
    int64_t v50 = 0;
    void* v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    void* v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
    void* v57 = 0;
    int64_t v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    v9 = alloca(8);
    v12 = alloca(8);
    v24 = alloca(8);
    v31 = alloca(8);
    v46 = alloca(8);
    v51 = alloca(8);
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    return v1;
  bb2:
    goto bb3;
  bb3:
    v7 = 48LL;
    v8 = p_malloc(v7);
    memset(v9, 0, 8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 0LL;
    memset(v12, 0, 8);
    *(int64_t*)v12 = (int64_t)v11;
    goto bb4;
  bb4:
    p_co_tick();
    v16 = *(int64_t*)v12;
    v17 = 6LL;
    v18 = v16 < v17;
    if (v18) goto bb5;
    else goto bb6;
  bb5:
    v20 = *(int64_t*)v12;
    v21 = 8LL;
    v22 = p_mul_i64(v20, v21);
    v23 = p_add_i64(v1, v22);
    memset(v24, 0, 8);
    *(void**)v24 = (void*)v23;
    v26 = *(int64_t*)v9;
    v27 = *(int64_t*)v12;
    v28 = 8LL;
    v29 = p_mul_i64(v27, v28);
    v30 = p_add_i64(v26, v29);
    memset(v31, 0, 8);
    *(void**)v31 = (void*)v30;
    v34 = *(void**)v31;
    v35 = *(void**)v24;
    v36 = *(int64_t*)v35;
    *(int64_t*)v34 = (int64_t)v36;
    v39 = *(int64_t*)v12;
    v40 = 1LL;
    v41 = (int64_t)((uint64_t)v39 + (uint64_t)v40);
    *(int64_t*)v12 = (int64_t)v41;
    goto bb4;
  bb6:
    v44 = 32LL;
    v45 = p_add_i64(v1, v44);
    memset(v46, 0, 8);
    *(void**)v46 = (void*)v45;
    v48 = *(int64_t*)v9;
    v49 = 32LL;
    v50 = p_add_i64(v48, v49);
    memset(v51, 0, 8);
    *(void**)v51 = (void*)v50;
    v54 = *(void**)v46;
    v55 = 1LL;
    *(int64_t*)v54 = (int64_t)v55;
    v57 = *(void**)v51;
    v58 = 1LL;
    *(int64_t*)v57 = (int64_t)v58;
    v61 = *(int64_t*)v9;
    return v61;
    return 0;
}

void* HashMap_to_str(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    int64_t v7 = 0;
    int64_t v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    bool v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    int64_t v24 = 0;
    bool v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    int64_t v33 = 0;
    int64_t v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0;
    bool v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0;
    void* v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    int64_t v47 = 0;
    int64_t v48 = 0;
    int64_t v49 = 0;
    int64_t v50 = 0;
    int64_t v51 = 0;
    void* v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0;
    int64_t v58 = 0;
    bool v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0;
    int64_t v67 = 0;
    int64_t v68 = 0;
    int64_t v69 = 0;
    int64_t v70 = 0;
    int64_t v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0; /* placeholder for void */
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0;
    int64_t v76 = 0;
    int64_t v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0;
    int64_t v81 = 0;
    int64_t v82 = 0;
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0; /* placeholder for void */
    void* v85 = 0;
    void* v86 = 0;
    int64_t v87 = 0; /* placeholder for void */
    int64_t v88 = 0;
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0;
    int64_t v93 = 0;
    bool v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0;
    int64_t v97 = 0;
    bool v98 = 0;
    int64_t v99 = 0; /* placeholder for void */
    void* v100 = 0;
    void* v101 = 0;
    void* v102 = 0;
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0; /* placeholder for void */
    int64_t v105 = 0; /* placeholder for void */
    int64_t v106 = 0;
    int64_t v107 = 0;
    int64_t v108 = 0;
    int64_t v109 = 0;
    void* v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    int64_t v112 = 0;
    bool v113 = 0;
    int64_t v114 = 0; /* placeholder for void */
    void* v115 = 0;
    int64_t v116 = 0;
    void* v117 = 0;
    void* v118 = 0;
    void* v119 = 0;
    void* v120 = 0;
    int64_t v121 = 0;
    int64_t v122 = 0;
    int64_t v123 = 0;
    int64_t v124 = 0;
    int64_t v125 = 0;
    void* v126 = 0;
    void* v127 = 0;
    int64_t v128 = 0; /* placeholder for void */
    int64_t v129 = 0;
    int64_t v130 = 0;
    int64_t v131 = 0;
    int64_t v132 = 0; /* placeholder for void */
    int64_t v133 = 0; /* placeholder for void */
    int64_t v134 = 0; /* placeholder for void */
    int64_t v135 = 0;
    int64_t v136 = 0;
    int64_t v137 = 0;
    void* v138 = 0;
    int64_t v139 = 0; /* placeholder for void */
    int64_t v140 = 0;
    int64_t v141 = 0;
    void* v142 = 0;
    void* v143 = 0;
    int64_t v144 = 0; /* placeholder for void */
    void* v145 = 0;
    void* v146 = 0;
    void* v147 = 0;
    int64_t v148 = 0; /* placeholder for void */
    void* v149 = 0;
    void* v150 = 0;
    void* v151 = 0;
    int64_t v152 = 0; /* placeholder for void */
    void* v153 = 0;
    int64_t v154 = 0;
    void* v155 = 0;
    void* v156 = 0;
    int64_t v157 = 0; /* placeholder for void */
    int64_t v158 = 0;
    int64_t v159 = 0;
    int64_t v160 = 0;
    int64_t v161 = 0; /* placeholder for void */
    int64_t v162 = 0; /* placeholder for void */
    void* v163 = 0;
    void* v164 = 0;
    void* v165 = 0;
    int64_t v166 = 0; /* placeholder for void */
    void* v167 = 0;
    int64_t v168 = 0; /* placeholder for void */
    int64_t v169 = 0;
    int64_t v170 = 0; /* placeholder for void */
    int64_t v171 = 0;
    int64_t v172 = 0; /* placeholder for void */
    v4 = alloca(8);
    v9 = alloca(8);
    v12 = alloca(8);
    v28 = alloca(8);
    v86 = alloca(8);
    v44 = alloca(8);
    v52 = alloca(8);
    v110 = alloca(8);
    v138 = alloca(8);
    v143 = alloca(8);
    v2 = 0LL;
    v3 = (int64_t)HashMap_keys((void*)v2, v1);
    memset(v4, 0, 8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    v7 = 0LL;
    v8 = (int64_t)Vec_len((void*)v7, v6);
    memset(v9, 0, 8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 0LL;
    memset(v12, 0, 8);
    *(int64_t*)v12 = (int64_t)v11;
    v14 = 0LL;
    v15 = (bool)hm_is_int((void*)v14, v1);
    if (v15) goto bb1;
    else goto bb2;
  bb1:
    v17 = *(int64_t*)v9;
    *(int64_t*)v12 = (int64_t)v17;
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    goto bb4;
  bb4:
    p_co_tick();
    v23 = *(int64_t*)v12;
    v24 = *(int64_t*)v9;
    v25 = v23 < v24;
    if (v25) goto bb5;
    else goto bb6;
  bb5:
    v27 = 0LL;
    memset(v28, 0, 8);
    *(int64_t*)v28 = (int64_t)v27;
    goto bb7;
  bb6:
    v85 = _str1;
    memset(v86, 0, 8);
    *(void**)v86 = (void*)v85;
    v88 = 0LL;
    *(int64_t*)v12 = (int64_t)v88;
    goto bb13;
  bb7:
    p_co_tick();
    v32 = *(int64_t*)v28;
    v33 = 1LL;
    v34 = p_add_i64(v32, v33);
    v35 = *(int64_t*)v9;
    v36 = *(int64_t*)v12;
    v37 = p_sub_i64(v35, v36);
    v38 = v34 < v37;
    if (v38) goto bb8;
    else goto bb9;
  bb8:
    v40 = *(int64_t*)v4;
    v41 = *(int64_t*)v28;
    v42 = 0LL;
    v43 = (int64_t)Vec_get((void*)v42, v40, v41);
    memset(v44, 0, 8);
    *(int64_t*)v44 = (int64_t)v43;
    v46 = *(int64_t*)v4;
    v47 = *(int64_t*)v28;
    v48 = 1LL;
    v49 = p_add_i64(v47, v48);
    v50 = 0LL;
    v51 = (int64_t)Vec_get((void*)v50, v46, v49);
    memset(v52, 0, 8);
    *(int64_t*)v52 = (int64_t)v51;
    v54 = *(int64_t*)v44;
    v55 = *(int64_t*)v52;
    v56 = 0LL;
    v57 = (int64_t)p_str_compare((void*)v56, v54, v55);
    v58 = 0LL;
    v59 = v57 > v58;
    if (v59) goto bb10;
    else goto bb11;
  bb9:
    v80 = *(int64_t*)v12;
    v81 = 1LL;
    v82 = (int64_t)((uint64_t)v80 + (uint64_t)v81);
    *(int64_t*)v12 = (int64_t)v82;
    goto bb4;
  bb10:
    v61 = *(int64_t*)v4;
    v62 = *(int64_t*)v28;
    v63 = *(int64_t*)v52;
    v64 = 0LL;
    Vec_set((void*)v64, v61, v62, v63);
    v66 = *(int64_t*)v4;
    v67 = *(int64_t*)v28;
    v68 = 1LL;
    v69 = p_add_i64(v67, v68);
    v70 = *(int64_t*)v44;
    v71 = 0LL;
    Vec_set((void*)v71, v66, v69, v70);
    goto bb12;
  bb11:
    goto bb12;
  bb12:
    v75 = *(int64_t*)v28;
    v76 = 1LL;
    v77 = p_add_i64(v75, v76);
    *(int64_t*)v28 = (int64_t)v77;
    goto bb7;
  bb13:
    p_co_tick();
    v92 = *(int64_t*)v12;
    v93 = *(int64_t*)v9;
    v94 = v92 < v93;
    if (v94) goto bb14;
    else goto bb15;
  bb14:
    v96 = *(int64_t*)v12;
    v97 = 0LL;
    v98 = v96 > v97;
    if (v98) goto bb16;
    else goto bb17;
  bb15:
    v163 = *(void**)v86;
    v164 = _str2;
    v165 = p_str_concat((void*)v163, (void*)v164);
    *(void**)v86 = (void*)v165;
    v167 = *(void**)v86;
    return v167;
  bb16:
    v100 = *(void**)v86;
    v101 = _str3;
    v102 = p_str_concat((void*)v100, (void*)v101);
    *(void**)v86 = (void*)v102;
    goto bb18;
  bb17:
    goto bb18;
  bb18:
    v106 = *(int64_t*)v4;
    v107 = *(int64_t*)v12;
    v108 = 0LL;
    v109 = (int64_t)Vec_get((void*)v108, v106, v107);
    memset(v110, 0, 8);
    *(int64_t*)v110 = (int64_t)v109;
    v112 = 0LL;
    v113 = (bool)hm_is_int((void*)v112, v1);
    if (v113) goto bb19;
    else goto bb20;
  bb19:
    v115 = *(void**)v86;
    v116 = *(int64_t*)v110;
    v117 = (void*)p_to_str((int64_t)v116);
    v118 = p_str_concat((void*)v115, (void*)v117);
    v119 = _str4;
    v120 = p_str_concat((void*)v118, (void*)v119);
    v169 = 0LL;
    Str_drop((void*)v169, (int64_t)v118);
    v121 = *(int64_t*)v110;
    v122 = 6768574230975169895LL;
    v123 = v121 ^ v122;
    v124 = 0LL;
    v125 = (int64_t)HashMap_get((void*)v124, v1, v123);
    v126 = (void*)p_to_str((int64_t)v125);
    v127 = p_str_concat((void*)v120, (void*)v126);
    v171 = 0LL;
    Str_drop((void*)v171, (int64_t)v120);
    *(void**)v86 = (void*)v127;
    v129 = *(int64_t*)v12;
    v130 = 1LL;
    v131 = p_add_i64(v129, v130);
    *(int64_t*)v12 = (int64_t)v131;
    goto bb13;
  bb20:
    goto bb21;
  bb21:
    v135 = *(int64_t*)v110;
    v136 = 0LL;
    v137 = (int64_t)HashMap_get((void*)v136, v1, v135);
    memset(v138, 0, 8);
    *(int64_t*)v138 = (int64_t)v137;
    v140 = *(int64_t*)v110;
    v141 = 0LL;
    v142 = p_str_from_ptr((void*)v141, v140);
    memset(v143, 0, 8);
    *(void**)v143 = (void*)v142;
    v145 = *(void**)v86;
    v146 = *(void**)v143;
    v147 = p_str_concat((void*)v145, (void*)v146);
    *(void**)v86 = (void*)v147;
    v149 = *(void**)v86;
    v150 = _str4;
    v151 = p_str_concat((void*)v149, (void*)v150);
    *(void**)v86 = (void*)v151;
    v153 = *(void**)v86;
    v154 = *(int64_t*)v138;
    v155 = (void*)p_to_str((int64_t)v154);
    v156 = p_str_concat((void*)v153, (void*)v155);
    *(void**)v86 = (void*)v156;
    v158 = *(int64_t*)v12;
    v159 = 1LL;
    v160 = p_add_i64(v158, v159);
    *(int64_t*)v12 = (int64_t)v160;
    goto bb13;
    return 0;
}

int64_t HashMap_values(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    int64_t v7 = 0;
    int64_t v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    int64_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    int64_t v21 = 0;
    bool v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0;
    int64_t v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0;
    void* v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0;
    bool v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    int64_t v49 = 0;
    int64_t v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
    v4 = alloca(8);
    v9 = alloca(8);
    v13 = alloca(8);
    v16 = alloca(8);
    v28 = alloca(8);
    v31 = alloca(8);
    v2 = 0LL;
    v3 = (int64_t)HashMap_keys((void*)v2, v1);
    memset(v4, 0, 8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    v7 = 0LL;
    v8 = (int64_t)Vec_len((void*)v7, v6);
    memset(v9, 0, 8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 0LL;
    v12 = (int64_t)Vec_new((void*)v11);
    memset(v13, 0, 8);
    *(int64_t*)v13 = (int64_t)v12;
    v15 = 0LL;
    memset(v16, 0, 8);
    *(int64_t*)v16 = (int64_t)v15;
    goto bb1;
  bb1:
    p_co_tick();
    v20 = *(int64_t*)v16;
    v21 = *(int64_t*)v9;
    v22 = v20 < v21;
    if (v22) goto bb2;
    else goto bb3;
  bb2:
    v24 = *(int64_t*)v4;
    v25 = *(int64_t*)v16;
    v26 = 0LL;
    v27 = (int64_t)Vec_get((void*)v26, v24, v25);
    memset(v28, 0, 8);
    *(int64_t*)v28 = (int64_t)v27;
    v30 = 0LL;
    memset(v31, 0, 8);
    *(int64_t*)v31 = (int64_t)v30;
    v33 = 0LL;
    v34 = (bool)hm_is_int((void*)v33, v1);
    if (v34) goto bb4;
    else goto bb5;
  bb3:
    v57 = *(int64_t*)v13;
    return v57;
  bb4:
    v36 = *(int64_t*)v28;
    v37 = 6768574230975169895LL;
    v38 = v36 ^ v37;
    v39 = 0LL;
    v40 = (int64_t)HashMap_get((void*)v39, v1, v38);
    *(int64_t*)v31 = (int64_t)v40;
    goto bb6;
  bb5:
    v43 = *(int64_t*)v28;
    v44 = 0LL;
    v45 = (int64_t)HashMap_get((void*)v44, v1, v43);
    *(int64_t*)v31 = (int64_t)v45;
    goto bb6;
  bb6:
    v48 = *(int64_t*)v13;
    v49 = *(int64_t*)v31;
    v50 = 0LL;
    Vec_push((void*)v50, v48, v49);
    v52 = *(int64_t*)v16;
    v53 = 1LL;
    v54 = (int64_t)((uint64_t)v52 + (uint64_t)v53);
    *(int64_t*)v16 = (int64_t)v54;
    goto bb1;
    return 0;
}

int64_t HashMap_values_i(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)HashMap_values((void*)v2, v1);
    return v3;
    return 0;
}

void Option_and_then(void* v0, void* v1, void* v2, void* v3) {
    void* v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    void* v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0;
    bool v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    void* v32 = 0;
    int64_t v33 = 0;
    void* v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    void* v36 = 0;
    void* v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    void* v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    v11 = alloca(16);
    v32 = alloca(16);
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
    memset(v11, 0, 16);
    v12 = 0LL;
    v13 = (char*)v11 + 0;
    *(int64_t*)v13 = (int64_t)v12;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = (int64_t)ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_to_str") == 0) {
            v15 = (int64_t)ByteBuffer_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = (int64_t)HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = (int64_t)HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys_i") == 0) {
            v15 = (int64_t)HashMap_keys_i((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = (int64_t)HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_share") == 0) {
            v15 = (int64_t)HashMap_share((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_to_str") == 0) {
            v15 = (int64_t)HashMap_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = (int64_t)HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values_i") == 0) {
            v15 = (int64_t)HashMap_values_i((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = (int64_t)Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = (int64_t)Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = (int64_t)StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_to_str") == 0) {
            v15 = (int64_t)StringBuilder_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = (int64_t)String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = (int64_t)String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = (int64_t)String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = (int64_t)String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = (int64_t)String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = (int64_t)String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = (int64_t)String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = (int64_t)String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = (int64_t)String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = (int64_t)String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = (int64_t)Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = (int64_t)Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = (int64_t)Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = (int64_t)Vec_last((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = (int64_t)Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = (int64_t)Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_share") == 0) {
            v15 = (int64_t)Vec_share((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_to_str") == 0) {
            v15 = (int64_t)Vec_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = (int64_t)p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = (int64_t)allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "byte_to_str") == 0) {
            v15 = (int64_t)byte_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = (int64_t)char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_str") == 0) {
            v15 = (int64_t)char_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = (int64_t)char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "create_allocator") == 0) {
            v15 = (int64_t)create_allocator((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "float_to_str") == 0) {
            v15 = (int64_t)float_to_str((void*)v3, (double)(intptr_t)v10);
        } else if (strcmp(_fn_name, "hm_int_hash") == 0) {
            v15 = (int64_t)hm_int_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "int_to_str") == 0) {
            v15 = (int64_t)int_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_alloc") == 0) {
            v15 = (int64_t)p_str_alloc((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_from_ptr") == 0) {
            v15 = (int64_t)p_str_from_ptr((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = (int64_t)p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = (int64_t)p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_bytes") == 0) {
            v15 = (int64_t)str_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_clone") == 0) {
            v15 = (int64_t)str_clone((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_from_bytes") == 0) {
            v15 = (int64_t)str_from_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = (int64_t)str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_lines") == 0) {
            v15 = (int64_t)str_lines((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = (int64_t)str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = (int64_t)str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_lower") == 0) {
            v15 = (int64_t)str_to_lower((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_upper") == 0) {
            v15 = (int64_t)str_to_upper((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim") == 0) {
            v15 = (int64_t)str_trim((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_end") == 0) {
            v15 = (int64_t)str_trim_end((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_start") == 0) {
            v15 = (int64_t)str_trim_start((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "u64_to_str") == 0) {
            v15 = (int64_t)u64_to_str((void*)v3, (uint64_t)v10);
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
    return;
  bb3:
    v27 = (char*)v2 + 0;
    v28 = *(int64_t*)v27;
    v29 = 1LL;
    v30 = v28 == v29;
    if (v30) goto bb4;
    else goto bb5;
  bb4:
    memset(v32, 0, 16);
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

void Option_context(void* v0, void* v1, void* v2, void* v3) {
    void* v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    void* v18 = 0;
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    void* v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    bool v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    void* v31 = 0;
    int64_t v32 = 0;
    void* v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    void* v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    void* v37 = 0;
    void* v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    void* v41 = 0;
    void* v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    v11 = alloca(16);
    v31 = alloca(16);
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
    memset(v11, 0, 16);
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
    return;
  bb3:
    v26 = (char*)v2 + 0;
    v27 = *(int64_t*)v26;
    v28 = 1LL;
    v29 = v27 == v28;
    if (v29) goto bb4;
    else goto bb5;
  bb4:
    memset(v31, 0, 16);
    v32 = 1LL;
    v33 = (char*)v31 + 0;
    *(int64_t*)v33 = (int64_t)v32;
    v35 = (char*)v31 + 8;
    *(void**)v35 = (void*)v3;
    v37 = (char*)v31 + 0;
    v38 = (char*)v0 + 0;
    v39 = *(int64_t*)v37;
    *(int64_t*)v38 = (int64_t)v39;
    v41 = (char*)v31 + 8;
    v42 = (char*)v0 + 8;
    v43 = *(int64_t*)v41;
    *(int64_t*)v42 = (int64_t)v43;
    return;
  bb5:
    goto bb1;
    return;
}

int64_t Option_expect(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    v19 = 0LL;
    return v19;
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
    p_panic((void*)v2);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

bool Option_is_none(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    bool v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    bool v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
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

bool Option_is_some(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    bool v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    bool v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
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

void Option_map(void* v0, void* v1, void* v2, void* v3) {
    void* v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    void* v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0;
    bool v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    void* v32 = 0;
    int64_t v33 = 0;
    void* v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    void* v36 = 0;
    void* v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    void* v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    v11 = alloca(16);
    v32 = alloca(16);
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
    memset(v11, 0, 16);
    v12 = 0LL;
    v13 = (char*)v11 + 0;
    *(int64_t*)v13 = (int64_t)v12;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = (int64_t)ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_to_str") == 0) {
            v15 = (int64_t)ByteBuffer_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = (int64_t)HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = (int64_t)HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys_i") == 0) {
            v15 = (int64_t)HashMap_keys_i((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = (int64_t)HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_share") == 0) {
            v15 = (int64_t)HashMap_share((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_to_str") == 0) {
            v15 = (int64_t)HashMap_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = (int64_t)HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values_i") == 0) {
            v15 = (int64_t)HashMap_values_i((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = (int64_t)Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = (int64_t)Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = (int64_t)StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_to_str") == 0) {
            v15 = (int64_t)StringBuilder_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = (int64_t)String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = (int64_t)String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = (int64_t)String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = (int64_t)String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = (int64_t)String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = (int64_t)String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = (int64_t)String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = (int64_t)String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = (int64_t)String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = (int64_t)String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = (int64_t)Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = (int64_t)Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = (int64_t)Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = (int64_t)Vec_last((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = (int64_t)Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = (int64_t)Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_share") == 0) {
            v15 = (int64_t)Vec_share((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_to_str") == 0) {
            v15 = (int64_t)Vec_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = (int64_t)p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = (int64_t)allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "byte_to_str") == 0) {
            v15 = (int64_t)byte_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = (int64_t)char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_str") == 0) {
            v15 = (int64_t)char_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = (int64_t)char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "create_allocator") == 0) {
            v15 = (int64_t)create_allocator((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "float_to_str") == 0) {
            v15 = (int64_t)float_to_str((void*)v3, (double)(intptr_t)v10);
        } else if (strcmp(_fn_name, "hm_int_hash") == 0) {
            v15 = (int64_t)hm_int_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "int_to_str") == 0) {
            v15 = (int64_t)int_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_alloc") == 0) {
            v15 = (int64_t)p_str_alloc((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_from_ptr") == 0) {
            v15 = (int64_t)p_str_from_ptr((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = (int64_t)p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = (int64_t)p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_bytes") == 0) {
            v15 = (int64_t)str_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_clone") == 0) {
            v15 = (int64_t)str_clone((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_from_bytes") == 0) {
            v15 = (int64_t)str_from_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = (int64_t)str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_lines") == 0) {
            v15 = (int64_t)str_lines((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = (int64_t)str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = (int64_t)str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_lower") == 0) {
            v15 = (int64_t)str_to_lower((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_upper") == 0) {
            v15 = (int64_t)str_to_upper((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim") == 0) {
            v15 = (int64_t)str_trim((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_end") == 0) {
            v15 = (int64_t)str_trim_end((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_start") == 0) {
            v15 = (int64_t)str_trim_start((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "u64_to_str") == 0) {
            v15 = (int64_t)u64_to_str((void*)v3, (uint64_t)v10);
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
    return;
  bb3:
    v27 = (char*)v2 + 0;
    v28 = *(int64_t*)v27;
    v29 = 1LL;
    v30 = v28 == v29;
    if (v30) goto bb4;
    else goto bb5;
  bb4:
    memset(v32, 0, 16);
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

void Option_or_else(void* v0, void* v1, void* v2, void* v3) {
    void* v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    void* v18 = 0;
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    void* v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    bool v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    void* v31 = 0;
    int64_t v32 = 0;
    void* v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0;
    void* v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    void* v38 = 0;
    void* v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    void* v42 = 0;
    void* v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    v11 = alloca(16);
    v31 = alloca(16);
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
    memset(v11, 0, 16);
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
    return;
  bb3:
    v26 = (char*)v2 + 0;
    v27 = *(int64_t*)v26;
    v28 = 1LL;
    v29 = v27 == v28;
    if (v29) goto bb4;
    else goto bb5;
  bb4:
    memset(v31, 0, 16);
    v32 = 0LL;
    v33 = (char*)v31 + 0;
    *(int64_t*)v33 = (int64_t)v32;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "HashMap_new") == 0) {
            v35 = (int64_t)HashMap_new((void*)v3);
        } else if (strcmp(_fn_name, "Vec_new") == 0) {
            v35 = (int64_t)Vec_new((void*)v3);
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
    return;
  bb5:
    goto bb1;
    return;
}

int64_t Option_unwrap(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0;
    bool v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
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
    p_panic((void*)v15);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

int64_t Option_unwrap_or(void* v0, void* v1, int64_t v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
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

int64_t Option_unwrap_or_else(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    v19 = 0LL;
    return v19;
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
    {
        if (!v2) abort();
        char* _fn_name = *(char**)v2;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "HashMap_new") == 0) {
            v16 = (int64_t)HashMap_new((void*)v2);
        } else if (strcmp(_fn_name, "Vec_new") == 0) {
            v16 = (int64_t)Vec_new((void*)v2);
        } else { abort(); }
    }
    return v16;
  bb5:
    goto bb1;
    return 0;
}

void Result_and_then(void* v0, void* v1, void* v2, void* v3) {
    void* v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    void* v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0;
    bool v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    void* v32 = 0;
    int64_t v33 = 0;
    void* v34 = 0;
    int64_t v35 = 0;
    void* v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    void* v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    void* v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    void* v44 = 0;
    void* v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    v11 = alloca(16);
    v34 = alloca(16);
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
    memset(v11, 0, 16);
    v12 = 0LL;
    v13 = (char*)v11 + 0;
    *(int64_t*)v13 = (int64_t)v12;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = (int64_t)ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_to_str") == 0) {
            v15 = (int64_t)ByteBuffer_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = (int64_t)HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = (int64_t)HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys_i") == 0) {
            v15 = (int64_t)HashMap_keys_i((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = (int64_t)HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_share") == 0) {
            v15 = (int64_t)HashMap_share((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_to_str") == 0) {
            v15 = (int64_t)HashMap_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = (int64_t)HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values_i") == 0) {
            v15 = (int64_t)HashMap_values_i((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = (int64_t)Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = (int64_t)Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = (int64_t)StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_to_str") == 0) {
            v15 = (int64_t)StringBuilder_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = (int64_t)String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = (int64_t)String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = (int64_t)String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = (int64_t)String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = (int64_t)String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = (int64_t)String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = (int64_t)String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = (int64_t)String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = (int64_t)String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = (int64_t)String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = (int64_t)Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = (int64_t)Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = (int64_t)Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = (int64_t)Vec_last((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = (int64_t)Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = (int64_t)Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_share") == 0) {
            v15 = (int64_t)Vec_share((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_to_str") == 0) {
            v15 = (int64_t)Vec_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = (int64_t)p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = (int64_t)allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "byte_to_str") == 0) {
            v15 = (int64_t)byte_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = (int64_t)char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_str") == 0) {
            v15 = (int64_t)char_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = (int64_t)char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "create_allocator") == 0) {
            v15 = (int64_t)create_allocator((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "float_to_str") == 0) {
            v15 = (int64_t)float_to_str((void*)v3, (double)(intptr_t)v10);
        } else if (strcmp(_fn_name, "hm_int_hash") == 0) {
            v15 = (int64_t)hm_int_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "int_to_str") == 0) {
            v15 = (int64_t)int_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_alloc") == 0) {
            v15 = (int64_t)p_str_alloc((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_from_ptr") == 0) {
            v15 = (int64_t)p_str_from_ptr((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = (int64_t)p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = (int64_t)p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_bytes") == 0) {
            v15 = (int64_t)str_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_clone") == 0) {
            v15 = (int64_t)str_clone((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_from_bytes") == 0) {
            v15 = (int64_t)str_from_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = (int64_t)str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_lines") == 0) {
            v15 = (int64_t)str_lines((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = (int64_t)str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = (int64_t)str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_lower") == 0) {
            v15 = (int64_t)str_to_lower((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_upper") == 0) {
            v15 = (int64_t)str_to_upper((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim") == 0) {
            v15 = (int64_t)str_trim((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_end") == 0) {
            v15 = (int64_t)str_trim_end((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_start") == 0) {
            v15 = (int64_t)str_trim_start((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "u64_to_str") == 0) {
            v15 = (int64_t)u64_to_str((void*)v3, (uint64_t)v10);
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
    memset(v34, 0, 16);
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
    return;
  bb5:
    goto bb1;
    return;
}

void Result_context(void* v0, void* v1, void* v2, void* v3) {
    void* v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    void* v18 = 0;
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    void* v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    bool v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    void* v31 = 0;
    void* v32 = 0;
    void* v33 = 0;
    int64_t v34 = 0;
    void* v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    void* v37 = 0;
    void* v38 = 0;
    void* v39 = 0;
    void* v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    void* v42 = 0;
    void* v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    void* v46 = 0;
    void* v47 = 0;
    int64_t v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    int64_t v54 = 0; /* placeholder for void */
    v11 = alloca(16);
    v33 = alloca(16);
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
    memset(v11, 0, 16);
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
    v32 = *(void**)v31;
    memset(v33, 0, 16);
    v34 = 1LL;
    v35 = (char*)v33 + 0;
    *(int64_t*)v35 = (int64_t)v34;
    v37 = _str4;
    v38 = p_str_concat((void*)v3, (void*)v37);
    v39 = p_str_concat((void*)v38, (void*)v32);
    v53 = 0LL;
    Str_drop((void*)v53, (int64_t)v38);
    v40 = (char*)v33 + 8;
    *(void**)v40 = (void*)v39;
    v42 = (char*)v33 + 0;
    v43 = (char*)v0 + 0;
    v44 = *(int64_t*)v42;
    *(int64_t*)v43 = (int64_t)v44;
    v46 = (char*)v33 + 8;
    v47 = (char*)v0 + 8;
    v48 = *(int64_t*)v46;
    *(int64_t*)v47 = (int64_t)v48;
    return;
  bb5:
    goto bb1;
    return;
}

void Result_err(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    void* v14 = 0;
    void* v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    void* v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0;
    bool v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    void* v28 = 0;
    int64_t v29 = 0;
    void* v30 = 0;
    int64_t v31 = 0;
    void* v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    void* v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    void* v36 = 0;
    void* v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    void* v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    v10 = alloca(16);
    v30 = alloca(16);
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
    memset(v10, 0, 16);
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
    memset(v30, 0, 16);
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

int64_t Result_expect(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    void* v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    v21 = 0LL;
    return v21;
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
    p_panic((void*)v2);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

bool Result_is_err(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    void* v16 = 0;
    int64_t v17 = 0;
    bool v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    bool v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
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

bool Result_is_ok(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    void* v16 = 0;
    int64_t v17 = 0;
    bool v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    bool v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
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

void Result_map(void* v0, void* v1, void* v2, void* v3) {
    void* v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    void* v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0;
    bool v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    void* v32 = 0;
    int64_t v33 = 0;
    void* v34 = 0;
    int64_t v35 = 0;
    void* v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    void* v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    void* v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    void* v44 = 0;
    void* v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    v11 = alloca(16);
    v34 = alloca(16);
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
    memset(v11, 0, 16);
    v12 = 0LL;
    v13 = (char*)v11 + 0;
    *(int64_t*)v13 = (int64_t)v12;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = (int64_t)ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_to_str") == 0) {
            v15 = (int64_t)ByteBuffer_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = (int64_t)HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = (int64_t)HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys_i") == 0) {
            v15 = (int64_t)HashMap_keys_i((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = (int64_t)HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_share") == 0) {
            v15 = (int64_t)HashMap_share((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_to_str") == 0) {
            v15 = (int64_t)HashMap_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = (int64_t)HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values_i") == 0) {
            v15 = (int64_t)HashMap_values_i((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = (int64_t)Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = (int64_t)Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = (int64_t)StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_to_str") == 0) {
            v15 = (int64_t)StringBuilder_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = (int64_t)String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = (int64_t)String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = (int64_t)String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = (int64_t)String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = (int64_t)String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = (int64_t)String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = (int64_t)String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = (int64_t)String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = (int64_t)String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = (int64_t)String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = (int64_t)Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = (int64_t)Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = (int64_t)Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = (int64_t)Vec_last((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = (int64_t)Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = (int64_t)Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_share") == 0) {
            v15 = (int64_t)Vec_share((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_to_str") == 0) {
            v15 = (int64_t)Vec_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = (int64_t)p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = (int64_t)allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "byte_to_str") == 0) {
            v15 = (int64_t)byte_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = (int64_t)char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_str") == 0) {
            v15 = (int64_t)char_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = (int64_t)char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "create_allocator") == 0) {
            v15 = (int64_t)create_allocator((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "float_to_str") == 0) {
            v15 = (int64_t)float_to_str((void*)v3, (double)(intptr_t)v10);
        } else if (strcmp(_fn_name, "hm_int_hash") == 0) {
            v15 = (int64_t)hm_int_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "int_to_str") == 0) {
            v15 = (int64_t)int_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_alloc") == 0) {
            v15 = (int64_t)p_str_alloc((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_from_ptr") == 0) {
            v15 = (int64_t)p_str_from_ptr((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = (int64_t)p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = (int64_t)p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_bytes") == 0) {
            v15 = (int64_t)str_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_clone") == 0) {
            v15 = (int64_t)str_clone((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_from_bytes") == 0) {
            v15 = (int64_t)str_from_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = (int64_t)str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_lines") == 0) {
            v15 = (int64_t)str_lines((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = (int64_t)str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = (int64_t)str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_lower") == 0) {
            v15 = (int64_t)str_to_lower((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_upper") == 0) {
            v15 = (int64_t)str_to_upper((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim") == 0) {
            v15 = (int64_t)str_trim((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_end") == 0) {
            v15 = (int64_t)str_trim_end((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_start") == 0) {
            v15 = (int64_t)str_trim_start((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "u64_to_str") == 0) {
            v15 = (int64_t)u64_to_str((void*)v3, (uint64_t)v10);
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
    memset(v34, 0, 16);
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
    return;
  bb5:
    goto bb1;
    return;
}

void Result_map_err(void* v0, void* v1, void* v2, void* v3) {
    void* v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    void* v18 = 0;
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    void* v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    bool v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    void* v31 = 0;
    int64_t v32 = 0;
    void* v33 = 0;
    int64_t v34 = 0;
    void* v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    void* v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    void* v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    void* v44 = 0;
    void* v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    v11 = alloca(16);
    v33 = alloca(16);
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
    memset(v11, 0, 16);
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
    memset(v33, 0, 16);
    v34 = 1LL;
    v35 = (char*)v33 + 0;
    *(int64_t*)v35 = (int64_t)v34;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v37 = (int64_t)ByteBuffer_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "ByteBuffer_to_str") == 0) {
            v37 = (int64_t)ByteBuffer_to_str((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v37 = (int64_t)HashMap_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v37 = (int64_t)HashMap_keys((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_keys_i") == 0) {
            v37 = (int64_t)HashMap_keys_i((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v37 = (int64_t)HashMap_len((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_share") == 0) {
            v37 = (int64_t)HashMap_share((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_to_str") == 0) {
            v37 = (int64_t)HashMap_to_str((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v37 = (int64_t)HashMap_values((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_values_i") == 0) {
            v37 = (int64_t)HashMap_values_i((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v37 = (int64_t)Option_unwrap((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v37 = (int64_t)Result_unwrap((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v37 = (int64_t)StringBuilder_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "StringBuilder_to_str") == 0) {
            v37 = (int64_t)StringBuilder_to_str((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v37 = (int64_t)String_bytes((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v37 = (int64_t)String_char_count((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v37 = (int64_t)String_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v37 = (int64_t)String_from((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v37 = (int64_t)String_len((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v37 = (int64_t)String_parse_int((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v37 = (int64_t)String_to_lowercase((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v37 = (int64_t)String_to_string((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v37 = (int64_t)String_to_uppercase((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v37 = (int64_t)String_trim((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v37 = (int64_t)Vec_capacity((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v37 = (int64_t)Vec_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v37 = (int64_t)Vec_first((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v37 = (int64_t)Vec_last((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v37 = (int64_t)Vec_len((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v37 = (int64_t)Vec_pop((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_share") == 0) {
            v37 = (int64_t)Vec_share((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_to_str") == 0) {
            v37 = (int64_t)Vec_to_str((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v37 = (int64_t)p_abs((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v37 = (int64_t)allocator_bytes_used((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "byte_to_str") == 0) {
            v37 = (int64_t)byte_to_str((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v37 = (int64_t)char_to_lower((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "char_to_str") == 0) {
            v37 = (int64_t)char_to_str((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v37 = (int64_t)char_to_upper((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "create_allocator") == 0) {
            v37 = (int64_t)create_allocator((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "float_to_str") == 0) {
            v37 = (int64_t)float_to_str((void*)v3, (double)(intptr_t)v32);
        } else if (strcmp(_fn_name, "hm_int_hash") == 0) {
            v37 = (int64_t)hm_int_hash((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "int_to_str") == 0) {
            v37 = (int64_t)int_to_str((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "p_str_alloc") == 0) {
            v37 = (int64_t)p_str_alloc((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "p_str_from_ptr") == 0) {
            v37 = (int64_t)p_str_from_ptr((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v37 = (int64_t)p_str_hash((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v37 = (int64_t)p_str_length((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "str_bytes") == 0) {
            v37 = (int64_t)str_bytes((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_clone") == 0) {
            v37 = (int64_t)str_clone((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_from_bytes") == 0) {
            v37 = (int64_t)str_from_bytes((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v37 = (int64_t)str_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_lines") == 0) {
            v37 = (int64_t)str_lines((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v37 = (int64_t)str_parse_hex((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v37 = (int64_t)str_parse_int((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_to_lower") == 0) {
            v37 = (int64_t)str_to_lower((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_to_upper") == 0) {
            v37 = (int64_t)str_to_upper((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_trim") == 0) {
            v37 = (int64_t)str_trim((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_trim_end") == 0) {
            v37 = (int64_t)str_trim_end((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_trim_start") == 0) {
            v37 = (int64_t)str_trim_start((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "u64_to_str") == 0) {
            v37 = (int64_t)u64_to_str((void*)v3, (uint64_t)v32);
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
    return;
  bb5:
    goto bb1;
    return;
}

void Result_ok(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    void* v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    void* v20 = 0;
    void* v21 = 0;
    int64_t v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0; /* placeholder for void */
    void* v25 = 0;
    int64_t v26 = 0;
    int64_t v27 = 0;
    bool v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    void* v30 = 0;
    int64_t v31 = 0;
    void* v32 = 0;
    int64_t v33 = 0;
    void* v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    void* v36 = 0;
    void* v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    void* v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    v10 = alloca(16);
    v32 = alloca(16);
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
    memset(v10, 0, 16);
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
    memset(v32, 0, 16);
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

int64_t Result_unwrap(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0;
    bool v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
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
    v17 = _str6;
    p_panic((void*)v17);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

int64_t Result_unwrap_or(void* v0, void* v1, int64_t v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    void* v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
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

int64_t Result_unwrap_or_else(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    void* v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    v21 = 0LL;
    return v21;
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
    {
        if (!v2) abort();
        char* _fn_name = *(char**)v2;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "HashMap_new") == 0) {
            v18 = (int64_t)HashMap_new((void*)v2);
        } else if (strcmp(_fn_name, "Vec_new") == 0) {
            v18 = (int64_t)Vec_new((void*)v2);
        } else { abort(); }
    }
    return v18;
  bb5:
    goto bb1;
    return 0;
}

void Result_with_context(void* v0, void* v1, void* v2, void* v3) {
    void* v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    void* v18 = 0;
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    void* v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    bool v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    void* v31 = 0;
    void* v32 = 0;
    void* v33 = 0;
    int64_t v34 = 0;
    void* v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    void* v37 = 0;
    void* v38 = 0;
    void* v39 = 0;
    void* v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    void* v43 = 0;
    void* v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    void* v47 = 0;
    void* v48 = 0;
    int64_t v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    v11 = alloca(16);
    v33 = alloca(16);
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
    memset(v11, 0, 16);
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
    v32 = *(void**)v31;
    memset(v33, 0, 16);
    v34 = 1LL;
    v35 = (char*)v33 + 0;
    *(int64_t*)v35 = (int64_t)v34;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "HashMap_new") == 0) {
            v37 = (void*)HashMap_new((void*)v3);
        } else if (strcmp(_fn_name, "Vec_new") == 0) {
            v37 = (void*)Vec_new((void*)v3);
        } else { abort(); }
    }
    v38 = _str4;
    v39 = p_str_concat((void*)v37, (void*)v38);
    v40 = p_str_concat((void*)v39, (void*)v32);
    v54 = 0LL;
    Str_drop((void*)v54, (int64_t)v39);
    v41 = (char*)v33 + 8;
    *(void**)v41 = (void*)v40;
    v43 = (char*)v33 + 0;
    v44 = (char*)v0 + 0;
    v45 = *(int64_t*)v43;
    *(int64_t*)v44 = (int64_t)v45;
    v47 = (char*)v33 + 8;
    v48 = (char*)v0 + 8;
    v49 = *(int64_t*)v47;
    *(int64_t*)v48 = (int64_t)v49;
    return;
  bb5:
    goto bb1;
    return;
}

void Str_drop(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    p_str_free((void*)v2, v1);
    return;
    return;
}

void StringBuilder_append_char(void* v0, void* v1, int64_t v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0; /* placeholder for void */
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = byte_to_str((void*)v5, v2);
    p_sb_append((void*)v4, (void*)v6);
    v8 = (char*)v1 + 8;
    v9 = (char*)v1 + 8;
    v10 = *(int64_t*)v9;
    v11 = 1LL;
    v12 = p_add_i64(v10, v11);
    *(int64_t*)v8 = (int64_t)v12;
    return;
    return;
}

void StringBuilder_append_int(void* v0, void* v1, int64_t v2) {
    int64_t v3 = 0;
    void* v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0;
    void* v14 = 0;
    int64_t v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    v5 = alloca(8);
    v3 = 0LL;
    v4 = int_to_str((void*)v3, v2);
    memset(v5, 0, 8);
    *(void**)v5 = (void*)v4;
    v7 = (char*)v1 + 0;
    v8 = *(int64_t*)v7;
    v9 = *(void**)v5;
    p_sb_append((void*)v8, (void*)v9);
    v11 = (char*)v1 + 8;
    v12 = (char*)v1 + 8;
    v13 = *(int64_t*)v12;
    v14 = *(void**)v5;
    v15 = 0LL;
    v16 = (int64_t)String_len((void*)v15, (int64_t)v14);
    v17 = p_add_i64(v13, v16);
    *(int64_t*)v11 = (int64_t)v17;
    return;
    return;
}

void StringBuilder_append_str(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    void* v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0;
    int64_t v9 = 0;
    int64_t v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    p_sb_append((void*)v4, (void*)v2);
    v6 = (char*)v1 + 8;
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    v9 = 0LL;
    v10 = (int64_t)String_len((void*)v9, (int64_t)v2);
    v11 = p_add_i64(v8, v10);
    *(int64_t*)v6 = (int64_t)v11;
    return;
    return;
}

void StringBuilder_clear(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    void* v5 = 0;
    int64_t v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = (int64_t)p_sb_new();
    *(int64_t*)v2 = (int64_t)v3;
    v5 = (char*)v1 + 8;
    v6 = 0LL;
    *(int64_t*)v5 = (int64_t)v6;
    return;
    return;
}

bool StringBuilder_is_empty(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    return v5;
    return 0;
}

int64_t StringBuilder_len(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    return v3;
    return 0;
}

void StringBuilder_new(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    void* v13 = 0;
    void* v14 = 0;
    int64_t v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    v2 = alloca(16);
    memset(v2, 0, 16);
    v3 = (int64_t)p_sb_new();
    v4 = (char*)v2 + 0;
    *(int64_t*)v4 = (int64_t)v3;
    v6 = 0LL;
    v7 = (char*)v2 + 8;
    *(int64_t*)v7 = (int64_t)v6;
    v9 = (char*)v2 + 0;
    v10 = (char*)v0 + 0;
    v11 = *(int64_t*)v9;
    *(int64_t*)v10 = (int64_t)v11;
    v13 = (char*)v2 + 8;
    v14 = (char*)v0 + 8;
    v15 = *(int64_t*)v13;
    *(int64_t*)v14 = (int64_t)v15;
    return;
    return;
}

void* StringBuilder_to_str(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = p_sb_to_str((void*)v3);
    return v4;
    return 0;
}

int64_t String_byte_at(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    bool v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0; /* placeholder for void */
    void* v25 = 0;
    uint8_t v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0;
    int64_t v30 = 0;
    bool v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    v16 = alloca(8);
    v19 = alloca(8);
    v22 = alloca(8);
    v3 = 0LL;
    v4 = v1 == v3;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    v6 = -1LL;
    return v6;
  bb2:
    goto bb3;
  bb3:
    v9 = 0LL;
    v10 = v2 < v9;
    if (v10) goto bb4;
    else goto bb5;
  bb4:
    v12 = -1LL;
    return v12;
  bb5:
    goto bb6;
  bb6:
    v15 = p_add_i64(v1, v2);
    memset(v16, 0, 8);
    *(int64_t*)v16 = (int64_t)v15;
    v18 = *(int64_t*)v16;
    memset(v19, 0, 8);
    *(void**)v19 = (void*)v18;
    v21 = 0LL;
    memset(v22, 0, 8);
    *(int64_t*)v22 = (int64_t)v21;
    v25 = *(void**)v19;
    v26 = *(uint8_t*)v25;
    *(int64_t*)v22 = (int64_t)v26;
    v29 = *(int64_t*)v22;
    v30 = 0LL;
    v31 = v29 == v30;
    if (v31) goto bb7;
    else goto bb8;
  bb7:
    v33 = -1LL;
    return v33;
  bb8:
    goto bb9;
  bb9:
    v36 = *(int64_t*)v22;
    return v36;
    return 0;
}

int64_t String_bytes(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    return v3;
    return 0;
}

int64_t String_char_at(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    void* v13 = 0;
    int64_t v14 = 0;
    bool v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0;
    bool v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    bool v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0;
    void* v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0;
    void* v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0; /* placeholder for void */
    void* v37 = 0;
    uint8_t v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    void* v42 = 0;
    void* v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0;
    void* v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    void* v49 = 0;
    int64_t v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    void* v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    void* v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0; /* placeholder for void */
    void* v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    v11 = alloca(8);
    v13 = alloca(1);
    v28 = alloca(8);
    v31 = alloca(8);
    v34 = alloca(8);
    v43 = alloca(8);
    v46 = alloca(8);
    v56 = alloca(8);
    v59 = alloca(8);
    v3 = 0LL;
    v4 = v1 == v3;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    v6 = 0LL;
    return v6;
  bb2:
    goto bb3;
  bb3:
    v9 = 0LL;
    v10 = (int64_t)p_str_length((void*)v9, v1);
    memset(v11, 0, 8);
    *(int64_t*)v11 = (int64_t)v10;
    memset(v13, 0, 1);
    v14 = 0LL;
    v15 = v2 < v14;
    *(bool*)v13 = v15;
    if (v15) goto bb5;
    else goto bb4;
  bb4:
    v18 = *(int64_t*)v11;
    v19 = v2 >= v18;
    *(bool*)v13 = v19;
    goto bb5;
  bb5:
    v22 = *(bool*)v13;
    if (v22) goto bb6;
    else goto bb7;
  bb6:
    v24 = 0LL;
    return v24;
  bb7:
    goto bb8;
  bb8:
    v27 = p_add_i64(v1, v2);
    memset(v28, 0, 8);
    *(int64_t*)v28 = (int64_t)v27;
    v30 = *(int64_t*)v28;
    memset(v31, 0, 8);
    *(void**)v31 = (void*)v30;
    v33 = 0LL;
    memset(v34, 0, 8);
    *(int64_t*)v34 = (int64_t)v33;
    v37 = *(void**)v31;
    v38 = *(uint8_t*)v37;
    *(int64_t*)v34 = (int64_t)v38;
    v41 = 2LL;
    v42 = p_malloc(v41);
    memset(v43, 0, 8);
    *(int64_t*)v43 = (int64_t)v42;
    v45 = *(int64_t*)v43;
    memset(v46, 0, 8);
    *(void**)v46 = (void*)v45;
    v49 = *(void**)v46;
    v50 = *(int64_t*)v34;
    *(uint8_t*)v49 = (uint8_t)v50;
    v53 = *(int64_t*)v43;
    v54 = 1LL;
    v55 = p_add_i64(v53, v54);
    memset(v56, 0, 8);
    *(int64_t*)v56 = (int64_t)v55;
    v58 = *(int64_t*)v56;
    memset(v59, 0, 8);
    *(void**)v59 = (void*)v58;
    v62 = *(void**)v59;
    v63 = 0LL;
    *(uint8_t*)v62 = (uint8_t)v63;
    v66 = *(int64_t*)v43;
    return v66;
    return 0;
}

int64_t String_char_count(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    return v3;
    return 0;
}

int64_t String_clone(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_from((void*)v2, v1);
    return v3;
    return 0;
}

bool String_contains(void* v0, int64_t v1, int64_t v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0; /* placeholder for void */
    bool v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    int64_t v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0;
    int64_t v26 = 0;
    bool v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    bool v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    int64_t v33 = 0;
    bool v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    bool v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    void* v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0;
    bool v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    bool v50 = 0;
    void* v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    void* v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    int64_t v59 = 0;
    bool v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0;
    int64_t v65 = 0;
    void* v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    int64_t v69 = 0;
    void* v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0;
    void* v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0;
    void* v76 = 0;
    int64_t v77 = 0; /* placeholder for void */
    int64_t v78 = 0;
    void* v79 = 0;
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0;
    void* v82 = 0;
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0; /* placeholder for void */
    void* v85 = 0;
    uint8_t v86 = 0;
    int64_t v87 = 0; /* placeholder for void */
    void* v88 = 0;
    uint8_t v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0;
    int64_t v93 = 0;
    bool v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    bool v96 = 0;
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0; /* placeholder for void */
    int64_t v99 = 0; /* placeholder for void */
    int64_t v100 = 0;
    int64_t v101 = 0;
    int64_t v102 = 0;
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0; /* placeholder for void */
    bool v105 = 0;
    int64_t v106 = 0; /* placeholder for void */
    bool v107 = 0;
    int64_t v108 = 0; /* placeholder for void */
    int64_t v109 = 0; /* placeholder for void */
    int64_t v110 = 0;
    int64_t v111 = 0;
    int64_t v112 = 0;
    int64_t v113 = 0; /* placeholder for void */
    int64_t v114 = 0; /* placeholder for void */
    bool v115 = 0;
    int64_t v116 = 0; /* placeholder for void */
    v3 = alloca(1);
    v19 = alloca(8);
    v23 = alloca(8);
    v40 = alloca(8);
    v51 = alloca(1);
    v54 = alloca(8);
    v66 = alloca(8);
    v70 = alloca(8);
    v73 = alloca(8);
    v76 = alloca(8);
    v79 = alloca(8);
    v82 = alloca(8);
    memset(v3, 0, 1);
    v4 = 0LL;
    v5 = v1 == v4;
    *(bool*)v3 = v5;
    if (v5) goto bb2;
    else goto bb1;
  bb1:
    v8 = 0LL;
    v9 = v2 == v8;
    *(bool*)v3 = v9;
    goto bb2;
  bb2:
    v12 = *(bool*)v3;
    if (v12) goto bb3;
    else goto bb4;
  bb3:
    v14 = false;
    return v14;
  bb4:
    goto bb5;
  bb5:
    v17 = 0LL;
    v18 = (int64_t)p_str_length((void*)v17, v1);
    memset(v19, 0, 8);
    *(int64_t*)v19 = (int64_t)v18;
    v21 = 0LL;
    v22 = (int64_t)p_str_length((void*)v21, v2);
    memset(v23, 0, 8);
    *(int64_t*)v23 = (int64_t)v22;
    v25 = *(int64_t*)v23;
    v26 = 0LL;
    v27 = v25 == v26;
    if (v27) goto bb6;
    else goto bb7;
  bb6:
    v29 = true;
    return v29;
  bb7:
    goto bb8;
  bb8:
    v32 = *(int64_t*)v23;
    v33 = *(int64_t*)v19;
    v34 = v32 > v33;
    if (v34) goto bb9;
    else goto bb10;
  bb9:
    v36 = false;
    return v36;
  bb10:
    goto bb11;
  bb11:
    v39 = 0LL;
    memset(v40, 0, 8);
    *(int64_t*)v40 = (int64_t)v39;
    goto bb12;
  bb12:
    p_co_tick();
    v44 = *(int64_t*)v40;
    v45 = *(int64_t*)v19;
    v46 = *(int64_t*)v23;
    v47 = p_sub_i64(v45, v46);
    v48 = v44 <= v47;
    if (v48) goto bb13;
    else goto bb14;
  bb13:
    v50 = true;
    memset(v51, 0, 1);
    *(bool*)v51 = v50;
    v53 = 0LL;
    memset(v54, 0, 8);
    *(int64_t*)v54 = (int64_t)v53;
    goto bb15;
  bb14:
    v115 = false;
    return v115;
  bb15:
    p_co_tick();
    v58 = *(int64_t*)v54;
    v59 = *(int64_t*)v23;
    v60 = v58 < v59;
    if (v60) goto bb16;
    else goto bb17;
  bb16:
    v62 = *(int64_t*)v40;
    v63 = p_add_i64(v1, v62);
    v64 = *(int64_t*)v54;
    v65 = p_add_i64(v63, v64);
    memset(v66, 0, 8);
    *(int64_t*)v66 = (int64_t)v65;
    v68 = *(int64_t*)v54;
    v69 = p_add_i64(v2, v68);
    memset(v70, 0, 8);
    *(int64_t*)v70 = (int64_t)v69;
    v72 = *(int64_t*)v66;
    memset(v73, 0, 8);
    *(void**)v73 = (void*)v72;
    v75 = *(int64_t*)v70;
    memset(v76, 0, 8);
    *(void**)v76 = (void*)v75;
    v78 = 0LL;
    memset(v79, 0, 8);
    *(int64_t*)v79 = (int64_t)v78;
    v81 = 0LL;
    memset(v82, 0, 8);
    *(int64_t*)v82 = (int64_t)v81;
    v85 = *(void**)v73;
    v86 = *(uint8_t*)v85;
    *(int64_t*)v79 = (int64_t)v86;
    v88 = *(void**)v76;
    v89 = *(uint8_t*)v88;
    *(int64_t*)v82 = (int64_t)v89;
    v92 = *(int64_t*)v79;
    v93 = *(int64_t*)v82;
    v94 = v92 != v93;
    if (v94) goto bb18;
    else goto bb19;
  bb17:
    v105 = *(bool*)v51;
    if (v105) goto bb21;
    else goto bb22;
  bb18:
    v96 = false;
    *(bool*)v51 = v96;
    goto bb17;
  bb19:
    goto bb20;
  bb20:
    v100 = *(int64_t*)v54;
    v101 = 1LL;
    v102 = (int64_t)((uint64_t)v100 + (uint64_t)v101);
    *(int64_t*)v54 = (int64_t)v102;
    goto bb15;
  bb21:
    v107 = true;
    return v107;
  bb22:
    goto bb23;
  bb23:
    v110 = *(int64_t*)v40;
    v111 = 1LL;
    v112 = p_add_i64(v110, v111);
    *(int64_t*)v40 = (int64_t)v112;
    goto bb12;
    return 0;
}

bool String_ends_with(void* v0, int64_t v1, int64_t v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0; /* placeholder for void */
    bool v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    int64_t v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0;
    int64_t v26 = 0;
    bool v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    bool v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    int64_t v33 = 0;
    bool v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    bool v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0;
    void* v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0;
    void* v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    int64_t v50 = 0;
    bool v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    void* v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
    int64_t v59 = 0;
    int64_t v60 = 0;
    void* v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    int64_t v63 = 0;
    void* v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0;
    void* v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0;
    void* v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0;
    void* v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0; /* placeholder for void */
    void* v76 = 0;
    uint8_t v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    void* v79 = 0;
    uint8_t v80 = 0;
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0; /* placeholder for void */
    int64_t v83 = 0;
    int64_t v84 = 0;
    bool v85 = 0;
    int64_t v86 = 0; /* placeholder for void */
    bool v87 = 0;
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0;
    int64_t v91 = 0;
    int64_t v92 = 0;
    int64_t v93 = 0; /* placeholder for void */
    int64_t v94 = 0; /* placeholder for void */
    bool v95 = 0;
    int64_t v96 = 0; /* placeholder for void */
    v3 = alloca(1);
    v19 = alloca(8);
    v23 = alloca(8);
    v42 = alloca(8);
    v45 = alloca(8);
    v57 = alloca(8);
    v61 = alloca(8);
    v64 = alloca(8);
    v67 = alloca(8);
    v70 = alloca(8);
    v73 = alloca(8);
    memset(v3, 0, 1);
    v4 = 0LL;
    v5 = v1 == v4;
    *(bool*)v3 = v5;
    if (v5) goto bb2;
    else goto bb1;
  bb1:
    v8 = 0LL;
    v9 = v2 == v8;
    *(bool*)v3 = v9;
    goto bb2;
  bb2:
    v12 = *(bool*)v3;
    if (v12) goto bb3;
    else goto bb4;
  bb3:
    v14 = false;
    return v14;
  bb4:
    goto bb5;
  bb5:
    v17 = 0LL;
    v18 = (int64_t)p_str_length((void*)v17, v1);
    memset(v19, 0, 8);
    *(int64_t*)v19 = (int64_t)v18;
    v21 = 0LL;
    v22 = (int64_t)p_str_length((void*)v21, v2);
    memset(v23, 0, 8);
    *(int64_t*)v23 = (int64_t)v22;
    v25 = *(int64_t*)v23;
    v26 = *(int64_t*)v19;
    v27 = v25 > v26;
    if (v27) goto bb6;
    else goto bb7;
  bb6:
    v29 = false;
    return v29;
  bb7:
    goto bb8;
  bb8:
    v32 = *(int64_t*)v23;
    v33 = 0LL;
    v34 = v32 == v33;
    if (v34) goto bb9;
    else goto bb10;
  bb9:
    v36 = true;
    return v36;
  bb10:
    goto bb11;
  bb11:
    v39 = *(int64_t*)v19;
    v40 = *(int64_t*)v23;
    v41 = p_sub_i64(v39, v40);
    memset(v42, 0, 8);
    *(int64_t*)v42 = (int64_t)v41;
    v44 = 0LL;
    memset(v45, 0, 8);
    *(int64_t*)v45 = (int64_t)v44;
    goto bb12;
  bb12:
    p_co_tick();
    v49 = *(int64_t*)v45;
    v50 = *(int64_t*)v23;
    v51 = v49 < v50;
    if (v51) goto bb13;
    else goto bb14;
  bb13:
    v53 = *(int64_t*)v42;
    v54 = p_add_i64(v1, v53);
    v55 = *(int64_t*)v45;
    v56 = p_add_i64(v54, v55);
    memset(v57, 0, 8);
    *(int64_t*)v57 = (int64_t)v56;
    v59 = *(int64_t*)v45;
    v60 = p_add_i64(v2, v59);
    memset(v61, 0, 8);
    *(int64_t*)v61 = (int64_t)v60;
    v63 = *(int64_t*)v57;
    memset(v64, 0, 8);
    *(void**)v64 = (void*)v63;
    v66 = *(int64_t*)v61;
    memset(v67, 0, 8);
    *(void**)v67 = (void*)v66;
    v69 = 0LL;
    memset(v70, 0, 8);
    *(int64_t*)v70 = (int64_t)v69;
    v72 = 0LL;
    memset(v73, 0, 8);
    *(int64_t*)v73 = (int64_t)v72;
    v76 = *(void**)v64;
    v77 = *(uint8_t*)v76;
    *(int64_t*)v70 = (int64_t)v77;
    v79 = *(void**)v67;
    v80 = *(uint8_t*)v79;
    *(int64_t*)v73 = (int64_t)v80;
    v83 = *(int64_t*)v70;
    v84 = *(int64_t*)v73;
    v85 = v83 != v84;
    if (v85) goto bb15;
    else goto bb16;
  bb14:
    v95 = true;
    return v95;
  bb15:
    v87 = false;
    return v87;
  bb16:
    goto bb17;
  bb17:
    v90 = *(int64_t*)v45;
    v91 = 1LL;
    v92 = (int64_t)((uint64_t)v90 + (uint64_t)v91);
    *(int64_t*)v45 = (int64_t)v92;
    goto bb12;
    return 0;
}

int64_t String_find(void* v0, int64_t v1, int64_t v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0; /* placeholder for void */
    bool v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    int64_t v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0;
    int64_t v26 = 0;
    bool v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    int64_t v33 = 0;
    bool v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    void* v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0;
    bool v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    bool v50 = 0;
    void* v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    void* v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    int64_t v59 = 0;
    bool v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0;
    int64_t v65 = 0;
    void* v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    int64_t v69 = 0;
    void* v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0;
    void* v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0;
    void* v76 = 0;
    int64_t v77 = 0; /* placeholder for void */
    int64_t v78 = 0;
    void* v79 = 0;
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0;
    void* v82 = 0;
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0; /* placeholder for void */
    void* v85 = 0;
    uint8_t v86 = 0;
    int64_t v87 = 0; /* placeholder for void */
    void* v88 = 0;
    uint8_t v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0;
    int64_t v93 = 0;
    bool v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    bool v96 = 0;
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0; /* placeholder for void */
    int64_t v99 = 0; /* placeholder for void */
    int64_t v100 = 0;
    int64_t v101 = 0;
    int64_t v102 = 0;
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0; /* placeholder for void */
    bool v105 = 0;
    int64_t v106 = 0; /* placeholder for void */
    int64_t v107 = 0;
    int64_t v108 = 0; /* placeholder for void */
    int64_t v109 = 0; /* placeholder for void */
    int64_t v110 = 0;
    int64_t v111 = 0;
    int64_t v112 = 0;
    int64_t v113 = 0; /* placeholder for void */
    int64_t v114 = 0; /* placeholder for void */
    int64_t v115 = 0;
    int64_t v116 = 0; /* placeholder for void */
    v3 = alloca(1);
    v19 = alloca(8);
    v23 = alloca(8);
    v40 = alloca(8);
    v51 = alloca(1);
    v54 = alloca(8);
    v66 = alloca(8);
    v70 = alloca(8);
    v73 = alloca(8);
    v76 = alloca(8);
    v79 = alloca(8);
    v82 = alloca(8);
    memset(v3, 0, 1);
    v4 = 0LL;
    v5 = v1 == v4;
    *(bool*)v3 = v5;
    if (v5) goto bb2;
    else goto bb1;
  bb1:
    v8 = 0LL;
    v9 = v2 == v8;
    *(bool*)v3 = v9;
    goto bb2;
  bb2:
    v12 = *(bool*)v3;
    if (v12) goto bb3;
    else goto bb4;
  bb3:
    v14 = -1LL;
    return v14;
  bb4:
    goto bb5;
  bb5:
    v17 = 0LL;
    v18 = (int64_t)p_str_length((void*)v17, v1);
    memset(v19, 0, 8);
    *(int64_t*)v19 = (int64_t)v18;
    v21 = 0LL;
    v22 = (int64_t)p_str_length((void*)v21, v2);
    memset(v23, 0, 8);
    *(int64_t*)v23 = (int64_t)v22;
    v25 = *(int64_t*)v23;
    v26 = 0LL;
    v27 = v25 == v26;
    if (v27) goto bb6;
    else goto bb7;
  bb6:
    v29 = 0LL;
    return v29;
  bb7:
    goto bb8;
  bb8:
    v32 = *(int64_t*)v23;
    v33 = *(int64_t*)v19;
    v34 = v32 > v33;
    if (v34) goto bb9;
    else goto bb10;
  bb9:
    v36 = -1LL;
    return v36;
  bb10:
    goto bb11;
  bb11:
    v39 = 0LL;
    memset(v40, 0, 8);
    *(int64_t*)v40 = (int64_t)v39;
    goto bb12;
  bb12:
    p_co_tick();
    v44 = *(int64_t*)v40;
    v45 = *(int64_t*)v19;
    v46 = *(int64_t*)v23;
    v47 = p_sub_i64(v45, v46);
    v48 = v44 <= v47;
    if (v48) goto bb13;
    else goto bb14;
  bb13:
    v50 = true;
    memset(v51, 0, 1);
    *(bool*)v51 = v50;
    v53 = 0LL;
    memset(v54, 0, 8);
    *(int64_t*)v54 = (int64_t)v53;
    goto bb15;
  bb14:
    v115 = -1LL;
    return v115;
  bb15:
    p_co_tick();
    v58 = *(int64_t*)v54;
    v59 = *(int64_t*)v23;
    v60 = v58 < v59;
    if (v60) goto bb16;
    else goto bb17;
  bb16:
    v62 = *(int64_t*)v40;
    v63 = p_add_i64(v1, v62);
    v64 = *(int64_t*)v54;
    v65 = p_add_i64(v63, v64);
    memset(v66, 0, 8);
    *(int64_t*)v66 = (int64_t)v65;
    v68 = *(int64_t*)v54;
    v69 = p_add_i64(v2, v68);
    memset(v70, 0, 8);
    *(int64_t*)v70 = (int64_t)v69;
    v72 = *(int64_t*)v66;
    memset(v73, 0, 8);
    *(void**)v73 = (void*)v72;
    v75 = *(int64_t*)v70;
    memset(v76, 0, 8);
    *(void**)v76 = (void*)v75;
    v78 = 0LL;
    memset(v79, 0, 8);
    *(int64_t*)v79 = (int64_t)v78;
    v81 = 0LL;
    memset(v82, 0, 8);
    *(int64_t*)v82 = (int64_t)v81;
    v85 = *(void**)v73;
    v86 = *(uint8_t*)v85;
    *(int64_t*)v79 = (int64_t)v86;
    v88 = *(void**)v76;
    v89 = *(uint8_t*)v88;
    *(int64_t*)v82 = (int64_t)v89;
    v92 = *(int64_t*)v79;
    v93 = *(int64_t*)v82;
    v94 = v92 != v93;
    if (v94) goto bb18;
    else goto bb19;
  bb17:
    v105 = *(bool*)v51;
    if (v105) goto bb21;
    else goto bb22;
  bb18:
    v96 = false;
    *(bool*)v51 = v96;
    goto bb17;
  bb19:
    goto bb20;
  bb20:
    v100 = *(int64_t*)v54;
    v101 = 1LL;
    v102 = (int64_t)((uint64_t)v100 + (uint64_t)v101);
    *(int64_t*)v54 = (int64_t)v102;
    goto bb15;
  bb21:
    v107 = *(int64_t*)v40;
    return v107;
  bb22:
    goto bb23;
  bb23:
    v110 = *(int64_t*)v40;
    v111 = 1LL;
    v112 = p_add_i64(v110, v111);
    *(int64_t*)v40 = (int64_t)v112;
    goto bb12;
    return 0;
}

int64_t String_from(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    void* v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    void* v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    int64_t v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0;
    void* v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0;
    void* v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    void* v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0; /* placeholder for void */
    void* v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    v7 = alloca(8);
    v10 = alloca(8);
    v22 = alloca(8);
    v28 = alloca(8);
    v37 = alloca(8);
    v40 = alloca(8);
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 1LL;
    v6 = p_malloc(v5);
    memset(v7, 0, 8);
    *(int64_t*)v7 = (int64_t)v6;
    v9 = *(int64_t*)v7;
    memset(v10, 0, 8);
    *(void**)v10 = (void*)v9;
    v13 = *(void**)v10;
    v14 = 0LL;
    *(uint8_t*)v13 = (uint8_t)v14;
    v17 = *(int64_t*)v7;
    return v17;
  bb2:
    goto bb3;
  bb3:
    v20 = 0LL;
    v21 = (int64_t)p_str_length((void*)v20, v1);
    memset(v22, 0, 8);
    *(int64_t*)v22 = (int64_t)v21;
    v24 = *(int64_t*)v22;
    v25 = 1LL;
    v26 = p_add_i64(v24, v25);
    v27 = p_malloc(v26);
    memset(v28, 0, 8);
    *(int64_t*)v28 = (int64_t)v27;
    v30 = *(int64_t*)v28;
    v31 = *(int64_t*)v22;
    v32 = 0LL;
    p_str_copy_n((void*)v32, v30, v1, v31);
    v34 = *(int64_t*)v28;
    v35 = *(int64_t*)v22;
    v36 = p_add_i64(v34, v35);
    memset(v37, 0, 8);
    *(int64_t*)v37 = (int64_t)v36;
    v39 = *(int64_t*)v37;
    memset(v40, 0, 8);
    *(void**)v40 = (void*)v39;
    v43 = *(void**)v40;
    v44 = 0LL;
    *(uint8_t*)v43 = (uint8_t)v44;
    v47 = *(int64_t*)v28;
    return v47;
    return 0;
}

bool String_is_empty(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    void* v14 = 0;
    uint8_t v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0;
    int64_t v19 = 0;
    bool v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    v8 = alloca(8);
    v11 = alloca(8);
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = true;
    return v5;
  bb2:
    goto bb3;
  bb3:
    memset(v8, 0, 8);
    *(void**)v8 = (void*)v1;
    v10 = 0LL;
    memset(v11, 0, 8);
    *(int64_t*)v11 = (int64_t)v10;
    v14 = *(void**)v8;
    v15 = *(uint8_t*)v14;
    *(int64_t*)v11 = (int64_t)v15;
    v18 = *(int64_t*)v11;
    v19 = 0LL;
    v20 = v18 == v19;
    return v20;
    return 0;
}

int64_t String_len(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    return v3;
    return 0;
}

int64_t String_parse_int(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    void* v20 = 0;
    uint8_t v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    void* v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0;
    int64_t v30 = 0;
    bool v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    int64_t v35 = 0;
    bool v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    bool v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0;
    int64_t v43 = 0;
    bool v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    bool v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0;
    int64_t v51 = 0;
    bool v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0; /* placeholder for void */
    bool v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0;
    int64_t v58 = 0;
    int64_t v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    void* v63 = 0;
    uint8_t v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0;
    int64_t v68 = 0;
    bool v69 = 0;
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0;
    int64_t v74 = 0;
    int64_t v75 = 0;
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    void* v79 = 0;
    uint8_t v80 = 0;
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0; /* placeholder for void */
    int64_t v83 = 0;
    int64_t v84 = 0;
    bool v85 = 0;
    int64_t v86 = 0; /* placeholder for void */
    int64_t v87 = 0;
    int64_t v88 = 0;
    int64_t v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0;
    int64_t v92 = 0; /* placeholder for void */
    void* v93 = 0;
    uint8_t v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0; /* placeholder for void */
    int64_t v99 = 0; /* placeholder for void */
    int64_t v100 = 0; /* placeholder for void */
    void* v101 = 0;
    int64_t v102 = 0;
    int64_t v103 = 0;
    bool v104 = 0;
    int64_t v105 = 0; /* placeholder for void */
    int64_t v106 = 0; /* placeholder for void */
    int64_t v107 = 0;
    int64_t v108 = 0;
    bool v109 = 0;
    int64_t v110 = 0; /* placeholder for void */
    int64_t v111 = 0; /* placeholder for void */
    bool v112 = 0;
    int64_t v113 = 0; /* placeholder for void */
    int64_t v114 = 0;
    int64_t v115 = 0;
    int64_t v116 = 0;
    int64_t v117 = 0;
    int64_t v118 = 0;
    int64_t v119 = 0;
    int64_t v120 = 0;
    int64_t v121 = 0; /* placeholder for void */
    int64_t v122 = 0;
    int64_t v123 = 0;
    int64_t v124 = 0;
    int64_t v125 = 0; /* placeholder for void */
    int64_t v126 = 0;
    int64_t v127 = 0; /* placeholder for void */
    void* v128 = 0;
    uint8_t v129 = 0;
    int64_t v130 = 0; /* placeholder for void */
    int64_t v131 = 0; /* placeholder for void */
    int64_t v132 = 0; /* placeholder for void */
    int64_t v133 = 0;
    int64_t v134 = 0;
    int64_t v135 = 0;
    int64_t v136 = 0; /* placeholder for void */
    v9 = alloca(8);
    v12 = alloca(8);
    v14 = alloca(8);
    v17 = alloca(8);
    v22 = alloca(8);
    v26 = alloca(1);
    v27 = alloca(1);
    v28 = alloca(1);
    v101 = alloca(1);
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 0LL;
    return v5;
  bb2:
    goto bb3;
  bb3:
    v8 = 0LL;
    memset(v9, 0, 8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 1LL;
    memset(v12, 0, 8);
    *(int64_t*)v12 = (int64_t)v11;
    memset(v14, 0, 8);
    *(int64_t*)v14 = (int64_t)v1;
    v16 = *(int64_t*)v14;
    memset(v17, 0, 8);
    *(void**)v17 = (void*)v16;
    v20 = *(void**)v17;
    v21 = *(uint8_t*)v20;
    memset(v22, 0, 8);
    *(int64_t*)v22 = (int64_t)v21;
    goto bb4;
  bb4:
    p_co_tick();
    memset(v26, 0, 1);
    memset(v27, 0, 1);
    memset(v28, 0, 1);
    v29 = *(int64_t*)v22;
    v30 = 32LL;
    v31 = v29 == v30;
    *(bool*)v28 = v31;
    if (v31) goto bb8;
    else goto bb7;
  bb5:
    v57 = *(int64_t*)v14;
    v58 = 1LL;
    v59 = p_add_i64(v57, v58);
    *(int64_t*)v14 = (int64_t)v59;
    v61 = *(int64_t*)v14;
    *(void**)v17 = (void*)v61;
    v63 = *(void**)v17;
    v64 = *(uint8_t*)v63;
    *(int64_t*)v22 = (int64_t)v64;
    goto bb4;
  bb6:
    v67 = *(int64_t*)v22;
    v68 = 45LL;
    v69 = v67 == v68;
    if (v69) goto bb13;
    else goto bb14;
  bb7:
    v34 = *(int64_t*)v22;
    v35 = 9LL;
    v36 = v34 == v35;
    *(bool*)v28 = v36;
    goto bb8;
  bb8:
    v39 = *(bool*)v28;
    *(bool*)v27 = v39;
    if (v39) goto bb10;
    else goto bb9;
  bb9:
    v42 = *(int64_t*)v22;
    v43 = 10LL;
    v44 = v42 == v43;
    *(bool*)v27 = v44;
    goto bb10;
  bb10:
    v47 = *(bool*)v27;
    *(bool*)v26 = v47;
    if (v47) goto bb12;
    else goto bb11;
  bb11:
    v50 = *(int64_t*)v22;
    v51 = 13LL;
    v52 = v50 == v51;
    *(bool*)v26 = v52;
    goto bb12;
  bb12:
    v55 = *(bool*)v26;
    if (v55) goto bb5;
    else goto bb6;
  bb13:
    v71 = -1LL;
    *(int64_t*)v12 = (int64_t)v71;
    v73 = *(int64_t*)v14;
    v74 = 1LL;
    v75 = p_add_i64(v73, v74);
    *(int64_t*)v14 = (int64_t)v75;
    v77 = *(int64_t*)v14;
    *(void**)v17 = (void*)v77;
    v79 = *(void**)v17;
    v80 = *(uint8_t*)v79;
    *(int64_t*)v22 = (int64_t)v80;
    goto bb15;
  bb14:
    v83 = *(int64_t*)v22;
    v84 = 43LL;
    v85 = v83 == v84;
    if (v85) goto bb16;
    else goto bb17;
  bb15:
    goto bb19;
  bb16:
    v87 = *(int64_t*)v14;
    v88 = 1LL;
    v89 = p_add_i64(v87, v88);
    *(int64_t*)v14 = (int64_t)v89;
    v91 = *(int64_t*)v14;
    *(void**)v17 = (void*)v91;
    v93 = *(void**)v17;
    v94 = *(uint8_t*)v93;
    *(int64_t*)v22 = (int64_t)v94;
    goto bb18;
  bb17:
    goto bb18;
  bb18:
    goto bb15;
  bb19:
    p_co_tick();
    memset(v101, 0, 1);
    v102 = *(int64_t*)v22;
    v103 = 48LL;
    v104 = v102 >= v103;
    *(bool*)v101 = v104;
    if (v104) goto bb22;
    else goto bb23;
  bb20:
    v114 = *(int64_t*)v9;
    v115 = 10LL;
    v116 = p_mul_i64(v114, v115);
    v117 = *(int64_t*)v22;
    v118 = 48LL;
    v119 = p_sub_i64(v117, v118);
    v120 = p_add_i64(v116, v119);
    *(int64_t*)v9 = (int64_t)v120;
    v122 = *(int64_t*)v14;
    v123 = 1LL;
    v124 = p_add_i64(v122, v123);
    *(int64_t*)v14 = (int64_t)v124;
    v126 = *(int64_t*)v14;
    *(void**)v17 = (void*)v126;
    v128 = *(void**)v17;
    v129 = *(uint8_t*)v128;
    *(int64_t*)v22 = (int64_t)v129;
    goto bb19;
  bb21:
    v133 = *(int64_t*)v9;
    v134 = *(int64_t*)v12;
    v135 = p_mul_i64(v133, v134);
    return v135;
  bb22:
    v107 = *(int64_t*)v22;
    v108 = 57LL;
    v109 = v107 <= v108;
    *(bool*)v101 = v109;
    goto bb23;
  bb23:
    v112 = *(bool*)v101;
    if (v112) goto bb20;
    else goto bb21;
    return 0;
}

int64_t String_push_str(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    int64_t v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0;
    void* v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    int64_t v20 = 0;
    int64_t v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0;
    int64_t v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0;
    void* v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    void* v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0; /* placeholder for void */
    void* v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    v5 = alloca(8);
    v9 = alloca(8);
    v17 = alloca(8);
    v34 = alloca(8);
    v37 = alloca(8);
    v3 = 0LL;
    v4 = (int64_t)p_str_length((void*)v3, v1);
    memset(v5, 0, 8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = 0LL;
    v8 = (int64_t)p_str_length((void*)v7, v2);
    memset(v9, 0, 8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = *(int64_t*)v5;
    v12 = *(int64_t*)v9;
    v13 = p_add_i64(v11, v12);
    v14 = 1LL;
    v15 = p_add_i64(v13, v14);
    v16 = p_malloc(v15);
    memset(v17, 0, 8);
    *(int64_t*)v17 = (int64_t)v16;
    v19 = *(int64_t*)v17;
    v20 = *(int64_t*)v5;
    v21 = 0LL;
    p_str_copy_n((void*)v21, v19, v1, v20);
    v23 = *(int64_t*)v17;
    v24 = *(int64_t*)v5;
    v25 = p_add_i64(v23, v24);
    v26 = *(int64_t*)v9;
    v27 = 0LL;
    p_str_copy_n((void*)v27, v25, v2, v26);
    v29 = *(int64_t*)v17;
    v30 = *(int64_t*)v5;
    v31 = p_add_i64(v29, v30);
    v32 = *(int64_t*)v9;
    v33 = p_add_i64(v31, v32);
    memset(v34, 0, 8);
    *(int64_t*)v34 = (int64_t)v33;
    v36 = *(int64_t*)v34;
    memset(v37, 0, 8);
    *(void**)v37 = (void*)v36;
    v40 = *(void**)v37;
    v41 = 0LL;
    *(uint8_t*)v40 = (uint8_t)v41;
    v44 = *(int64_t*)v17;
    return v44;
    return 0;
}

int64_t String_repeat(void* v0, int64_t v1, int64_t v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0; /* placeholder for void */
    bool v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    void* v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0;
    int64_t v30 = 0;
    void* v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0;
    int64_t v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0;
    void* v37 = 0;
    void* v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0;
    bool v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    int64_t v49 = 0;
    int64_t v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0;
    int64_t v57 = 0;
    int64_t v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0;
    void* v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0;
    void* v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0; /* placeholder for void */
    void* v71 = 0;
    int64_t v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0;
    int64_t v76 = 0; /* placeholder for void */
    v3 = alloca(1);
    v16 = alloca(8);
    v19 = alloca(8);
    v31 = alloca(8);
    v38 = alloca(8);
    v41 = alloca(8);
    v65 = alloca(8);
    v68 = alloca(8);
    memset(v3, 0, 1);
    v4 = 0LL;
    v5 = v1 == v4;
    *(bool*)v3 = v5;
    if (v5) goto bb2;
    else goto bb1;
  bb1:
    v8 = 0LL;
    v9 = v2 <= v8;
    *(bool*)v3 = v9;
    goto bb2;
  bb2:
    v12 = *(bool*)v3;
    if (v12) goto bb3;
    else goto bb4;
  bb3:
    v14 = 1LL;
    v15 = p_malloc(v14);
    memset(v16, 0, 8);
    *(int64_t*)v16 = (int64_t)v15;
    v18 = *(int64_t*)v16;
    memset(v19, 0, 8);
    *(void**)v19 = (void*)v18;
    v22 = *(void**)v19;
    v23 = 0LL;
    *(uint8_t*)v22 = (uint8_t)v23;
    v26 = *(int64_t*)v16;
    return v26;
  bb4:
    goto bb5;
  bb5:
    v29 = 0LL;
    v30 = (int64_t)p_str_length((void*)v29, v1);
    memset(v31, 0, 8);
    *(int64_t*)v31 = (int64_t)v30;
    v33 = *(int64_t*)v31;
    v34 = p_mul_i64(v33, v2);
    v35 = 1LL;
    v36 = p_add_i64(v34, v35);
    v37 = p_malloc(v36);
    memset(v38, 0, 8);
    *(int64_t*)v38 = (int64_t)v37;
    v40 = 0LL;
    memset(v41, 0, 8);
    *(int64_t*)v41 = (int64_t)v40;
    goto bb6;
  bb6:
    p_co_tick();
    v45 = *(int64_t*)v41;
    v46 = v45 < v2;
    if (v46) goto bb7;
    else goto bb8;
  bb7:
    v48 = *(int64_t*)v38;
    v49 = *(int64_t*)v41;
    v50 = *(int64_t*)v31;
    v51 = p_mul_i64(v49, v50);
    v52 = p_add_i64(v48, v51);
    v53 = *(int64_t*)v31;
    v54 = 0LL;
    p_str_copy_n((void*)v54, v52, v1, v53);
    v56 = *(int64_t*)v41;
    v57 = 1LL;
    v58 = (int64_t)((uint64_t)v56 + (uint64_t)v57);
    *(int64_t*)v41 = (int64_t)v58;
    goto bb6;
  bb8:
    v61 = *(int64_t*)v38;
    v62 = *(int64_t*)v31;
    v63 = p_mul_i64(v62, v2);
    v64 = p_add_i64(v61, v63);
    memset(v65, 0, 8);
    *(int64_t*)v65 = (int64_t)v64;
    v67 = *(int64_t*)v65;
    memset(v68, 0, 8);
    *(void**)v68 = (void*)v67;
    v71 = *(void**)v68;
    v72 = 0LL;
    *(uint8_t*)v71 = (uint8_t)v72;
    v75 = *(int64_t*)v38;
    return v75;
    return 0;
}

int64_t String_replace(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    void* v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    int64_t v23 = 0;
    bool v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    bool v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0; /* placeholder for void */
    bool v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0;
    int64_t v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    int64_t v38 = 0;
    void* v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    int64_t v42 = 0;
    bool v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    void* v50 = 0;
    void* v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    void* v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    void* v57 = 0;
    int64_t v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0;
    void* v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0;
    int64_t v65 = 0;
    void* v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0;
    int64_t v71 = 0;
    bool v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    bool v74 = 0;
    void* v75 = 0;
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0;
    void* v78 = 0;
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0;
    int64_t v83 = 0;
    bool v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0;
    int64_t v87 = 0;
    int64_t v88 = 0;
    int64_t v89 = 0;
    bool v90 = 0;
    int64_t v91 = 0; /* placeholder for void */
    bool v92 = 0;
    int64_t v93 = 0; /* placeholder for void */
    int64_t v94 = 0; /* placeholder for void */
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0;
    int64_t v97 = 0;
    int64_t v98 = 0;
    int64_t v99 = 0;
    void* v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0;
    int64_t v103 = 0;
    void* v104 = 0;
    int64_t v105 = 0; /* placeholder for void */
    int64_t v106 = 0;
    void* v107 = 0;
    int64_t v108 = 0; /* placeholder for void */
    int64_t v109 = 0;
    void* v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    int64_t v112 = 0;
    void* v113 = 0;
    int64_t v114 = 0; /* placeholder for void */
    int64_t v115 = 0;
    void* v116 = 0;
    int64_t v117 = 0; /* placeholder for void */
    int64_t v118 = 0; /* placeholder for void */
    void* v119 = 0;
    uint8_t v120 = 0;
    int64_t v121 = 0; /* placeholder for void */
    void* v122 = 0;
    uint8_t v123 = 0;
    int64_t v124 = 0; /* placeholder for void */
    int64_t v125 = 0; /* placeholder for void */
    int64_t v126 = 0;
    int64_t v127 = 0;
    bool v128 = 0;
    int64_t v129 = 0; /* placeholder for void */
    bool v130 = 0;
    int64_t v131 = 0; /* placeholder for void */
    int64_t v132 = 0; /* placeholder for void */
    int64_t v133 = 0; /* placeholder for void */
    int64_t v134 = 0;
    int64_t v135 = 0;
    int64_t v136 = 0;
    int64_t v137 = 0; /* placeholder for void */
    int64_t v138 = 0; /* placeholder for void */
    bool v139 = 0;
    int64_t v140 = 0; /* placeholder for void */
    int64_t v141 = 0;
    int64_t v142 = 0;
    int64_t v143 = 0;
    int64_t v144 = 0; /* placeholder for void */
    int64_t v145 = 0;
    int64_t v146 = 0;
    int64_t v147 = 0;
    int64_t v148 = 0; /* placeholder for void */
    int64_t v149 = 0; /* placeholder for void */
    int64_t v150 = 0;
    int64_t v151 = 0;
    void* v152 = 0;
    int64_t v153 = 0; /* placeholder for void */
    int64_t v154 = 0;
    void* v155 = 0;
    int64_t v156 = 0; /* placeholder for void */
    int64_t v157 = 0;
    void* v158 = 0;
    int64_t v159 = 0; /* placeholder for void */
    int64_t v160 = 0; /* placeholder for void */
    void* v161 = 0;
    uint8_t v162 = 0;
    int64_t v163 = 0; /* placeholder for void */
    int64_t v164 = 0; /* placeholder for void */
    int64_t v165 = 0;
    void* v166 = 0;
    void* v167 = 0;
    int64_t v168 = 0; /* placeholder for void */
    int64_t v169 = 0;
    void* v170 = 0;
    int64_t v171 = 0; /* placeholder for void */
    int64_t v172 = 0; /* placeholder for void */
    void* v173 = 0;
    int64_t v174 = 0;
    int64_t v175 = 0; /* placeholder for void */
    int64_t v176 = 0; /* placeholder for void */
    int64_t v177 = 0;
    int64_t v178 = 0;
    int64_t v179 = 0;
    void* v180 = 0;
    int64_t v181 = 0; /* placeholder for void */
    int64_t v182 = 0;
    void* v183 = 0;
    int64_t v184 = 0; /* placeholder for void */
    int64_t v185 = 0; /* placeholder for void */
    void* v186 = 0;
    int64_t v187 = 0;
    int64_t v188 = 0; /* placeholder for void */
    int64_t v189 = 0; /* placeholder for void */
    int64_t v190 = 0;
    int64_t v191 = 0;
    int64_t v192 = 0;
    int64_t v193 = 0;
    int64_t v194 = 0; /* placeholder for void */
    int64_t v195 = 0;
    int64_t v196 = 0; /* placeholder for void */
    int64_t v197 = 0;
    int64_t v198 = 0;
    int64_t v199 = 0;
    int64_t v200 = 0; /* placeholder for void */
    int64_t v201 = 0; /* placeholder for void */
    int64_t v202 = 0; /* placeholder for void */
    int64_t v203 = 0;
    int64_t v204 = 0; /* placeholder for void */
    v9 = alloca(8);
    v12 = alloca(8);
    v22 = alloca(1);
    v39 = alloca(8);
    v51 = alloca(8);
    v54 = alloca(8);
    v62 = alloca(8);
    v66 = alloca(8);
    v75 = alloca(1);
    v78 = alloca(8);
    v100 = alloca(8);
    v104 = alloca(8);
    v107 = alloca(8);
    v110 = alloca(8);
    v113 = alloca(8);
    v116 = alloca(8);
    v152 = alloca(8);
    v155 = alloca(8);
    v158 = alloca(8);
    v167 = alloca(8);
    v170 = alloca(8);
    v180 = alloca(8);
    v183 = alloca(8);
    v4 = 0LL;
    v5 = v1 == v4;
    if (v5) goto bb1;
    else goto bb2;
  bb1:
    v7 = 1LL;
    v8 = p_malloc(v7);
    memset(v9, 0, 8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = *(int64_t*)v9;
    memset(v12, 0, 8);
    *(void**)v12 = (void*)v11;
    v15 = *(void**)v12;
    v16 = 0LL;
    *(uint8_t*)v15 = (uint8_t)v16;
    v19 = *(int64_t*)v9;
    return v19;
  bb2:
    goto bb3;
  bb3:
    memset(v22, 0, 1);
    v23 = 0LL;
    v24 = v2 == v23;
    *(bool*)v22 = v24;
    if (v24) goto bb5;
    else goto bb4;
  bb4:
    v27 = 0LL;
    v28 = v3 == v27;
    *(bool*)v22 = v28;
    goto bb5;
  bb5:
    v31 = *(bool*)v22;
    if (v31) goto bb6;
    else goto bb7;
  bb6:
    v33 = 0LL;
    v34 = (int64_t)String_from((void*)v33, v1);
    return v34;
  bb7:
    goto bb8;
  bb8:
    v37 = 0LL;
    v38 = (int64_t)p_str_length((void*)v37, v2);
    memset(v39, 0, 8);
    *(int64_t*)v39 = (int64_t)v38;
    v41 = *(int64_t*)v39;
    v42 = 0LL;
    v43 = v41 == v42;
    if (v43) goto bb9;
    else goto bb10;
  bb9:
    v45 = 0LL;
    v46 = (int64_t)String_from((void*)v45, v1);
    return v46;
  bb10:
    goto bb11;
  bb11:
    v49 = 1LL;
    v50 = p_malloc(v49);
    memset(v51, 0, 8);
    *(int64_t*)v51 = (int64_t)v50;
    v53 = *(int64_t*)v51;
    memset(v54, 0, 8);
    *(void**)v54 = (void*)v53;
    v57 = *(void**)v54;
    v58 = 0LL;
    *(uint8_t*)v57 = (uint8_t)v58;
    v61 = 0LL;
    memset(v62, 0, 8);
    *(int64_t*)v62 = (int64_t)v61;
    v64 = 0LL;
    v65 = (int64_t)p_str_length((void*)v64, v1);
    memset(v66, 0, 8);
    *(int64_t*)v66 = (int64_t)v65;
    goto bb12;
  bb12:
    p_co_tick();
    v70 = *(int64_t*)v62;
    v71 = *(int64_t*)v66;
    v72 = v70 < v71;
    if (v72) goto bb13;
    else goto bb14;
  bb13:
    v74 = true;
    memset(v75, 0, 1);
    *(bool*)v75 = v74;
    v77 = 0LL;
    memset(v78, 0, 8);
    *(int64_t*)v78 = (int64_t)v77;
    goto bb15;
  bb14:
    v203 = *(int64_t*)v51;
    return v203;
  bb15:
    p_co_tick();
    v82 = *(int64_t*)v78;
    v83 = *(int64_t*)v39;
    v84 = v82 < v83;
    if (v84) goto bb16;
    else goto bb17;
  bb16:
    v86 = *(int64_t*)v62;
    v87 = *(int64_t*)v78;
    v88 = p_add_i64(v86, v87);
    v89 = *(int64_t*)v66;
    v90 = v88 >= v89;
    if (v90) goto bb18;
    else goto bb19;
  bb17:
    v139 = *(bool*)v75;
    if (v139) goto bb24;
    else goto bb25;
  bb18:
    v92 = false;
    *(bool*)v75 = v92;
    goto bb17;
  bb19:
    goto bb20;
  bb20:
    v96 = *(int64_t*)v62;
    v97 = p_add_i64(v1, v96);
    v98 = *(int64_t*)v78;
    v99 = p_add_i64(v97, v98);
    memset(v100, 0, 8);
    *(int64_t*)v100 = (int64_t)v99;
    v102 = *(int64_t*)v78;
    v103 = p_add_i64(v2, v102);
    memset(v104, 0, 8);
    *(int64_t*)v104 = (int64_t)v103;
    v106 = *(int64_t*)v100;
    memset(v107, 0, 8);
    *(void**)v107 = (void*)v106;
    v109 = *(int64_t*)v104;
    memset(v110, 0, 8);
    *(void**)v110 = (void*)v109;
    v112 = 0LL;
    memset(v113, 0, 8);
    *(int64_t*)v113 = (int64_t)v112;
    v115 = 0LL;
    memset(v116, 0, 8);
    *(int64_t*)v116 = (int64_t)v115;
    v119 = *(void**)v107;
    v120 = *(uint8_t*)v119;
    *(int64_t*)v113 = (int64_t)v120;
    v122 = *(void**)v110;
    v123 = *(uint8_t*)v122;
    *(int64_t*)v116 = (int64_t)v123;
    v126 = *(int64_t*)v113;
    v127 = *(int64_t*)v116;
    v128 = v126 != v127;
    if (v128) goto bb21;
    else goto bb22;
  bb21:
    v130 = false;
    *(bool*)v75 = v130;
    goto bb17;
  bb22:
    goto bb23;
  bb23:
    v134 = *(int64_t*)v78;
    v135 = 1LL;
    v136 = (int64_t)((uint64_t)v134 + (uint64_t)v135);
    *(int64_t*)v78 = (int64_t)v136;
    goto bb15;
  bb24:
    v141 = *(int64_t*)v51;
    v142 = 0LL;
    v143 = (int64_t)String_push_str((void*)v142, v141, v3);
    *(int64_t*)v51 = (int64_t)v143;
    v145 = *(int64_t*)v62;
    v146 = *(int64_t*)v39;
    v147 = p_add_i64(v145, v146);
    *(int64_t*)v62 = (int64_t)v147;
    goto bb26;
  bb25:
    v150 = *(int64_t*)v62;
    v151 = p_add_i64(v1, v150);
    memset(v152, 0, 8);
    *(int64_t*)v152 = (int64_t)v151;
    v154 = *(int64_t*)v152;
    memset(v155, 0, 8);
    *(void**)v155 = (void*)v154;
    v157 = 0LL;
    memset(v158, 0, 8);
    *(int64_t*)v158 = (int64_t)v157;
    v161 = *(void**)v155;
    v162 = *(uint8_t*)v161;
    *(int64_t*)v158 = (int64_t)v162;
    v165 = 2LL;
    v166 = p_malloc(v165);
    memset(v167, 0, 8);
    *(int64_t*)v167 = (int64_t)v166;
    v169 = *(int64_t*)v167;
    memset(v170, 0, 8);
    *(void**)v170 = (void*)v169;
    v173 = *(void**)v170;
    v174 = *(int64_t*)v158;
    *(uint8_t*)v173 = (uint8_t)v174;
    v177 = *(int64_t*)v167;
    v178 = 1LL;
    v179 = p_add_i64(v177, v178);
    memset(v180, 0, 8);
    *(int64_t*)v180 = (int64_t)v179;
    v182 = *(int64_t*)v180;
    memset(v183, 0, 8);
    *(void**)v183 = (void*)v182;
    v186 = *(void**)v183;
    v187 = 0LL;
    *(uint8_t*)v186 = (uint8_t)v187;
    v190 = *(int64_t*)v51;
    v191 = *(int64_t*)v167;
    v192 = 0LL;
    v193 = (int64_t)String_push_str((void*)v192, v190, v191);
    *(int64_t*)v51 = (int64_t)v193;
    v195 = *(int64_t*)v167;
    p_free((void*)v195);
    v197 = *(int64_t*)v62;
    v198 = 1LL;
    v199 = p_add_i64(v197, v198);
    *(int64_t*)v62 = (int64_t)v199;
    goto bb26;
  bb26:
    goto bb12;
    return 0;
}

int64_t String_rfind(void* v0, int64_t v1, int64_t v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0; /* placeholder for void */
    bool v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    int64_t v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0;
    int64_t v26 = 0;
    bool v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    int64_t v33 = 0;
    bool v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0;
    void* v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    int64_t v47 = 0;
    bool v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    bool v50 = 0;
    void* v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    void* v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    int64_t v59 = 0;
    bool v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0;
    int64_t v65 = 0;
    void* v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    int64_t v69 = 0;
    void* v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0;
    void* v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0;
    void* v76 = 0;
    int64_t v77 = 0; /* placeholder for void */
    int64_t v78 = 0;
    void* v79 = 0;
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0;
    void* v82 = 0;
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0; /* placeholder for void */
    void* v85 = 0;
    uint8_t v86 = 0;
    int64_t v87 = 0; /* placeholder for void */
    void* v88 = 0;
    uint8_t v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0;
    int64_t v93 = 0;
    bool v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    bool v96 = 0;
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0; /* placeholder for void */
    int64_t v99 = 0; /* placeholder for void */
    int64_t v100 = 0;
    int64_t v101 = 0;
    int64_t v102 = 0;
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0; /* placeholder for void */
    bool v105 = 0;
    int64_t v106 = 0; /* placeholder for void */
    int64_t v107 = 0;
    int64_t v108 = 0; /* placeholder for void */
    int64_t v109 = 0; /* placeholder for void */
    int64_t v110 = 0;
    int64_t v111 = 0;
    int64_t v112 = 0;
    int64_t v113 = 0; /* placeholder for void */
    int64_t v114 = 0; /* placeholder for void */
    int64_t v115 = 0;
    int64_t v116 = 0; /* placeholder for void */
    v3 = alloca(1);
    v19 = alloca(8);
    v23 = alloca(8);
    v42 = alloca(8);
    v51 = alloca(1);
    v54 = alloca(8);
    v66 = alloca(8);
    v70 = alloca(8);
    v73 = alloca(8);
    v76 = alloca(8);
    v79 = alloca(8);
    v82 = alloca(8);
    memset(v3, 0, 1);
    v4 = 0LL;
    v5 = v1 == v4;
    *(bool*)v3 = v5;
    if (v5) goto bb2;
    else goto bb1;
  bb1:
    v8 = 0LL;
    v9 = v2 == v8;
    *(bool*)v3 = v9;
    goto bb2;
  bb2:
    v12 = *(bool*)v3;
    if (v12) goto bb3;
    else goto bb4;
  bb3:
    v14 = -1LL;
    return v14;
  bb4:
    goto bb5;
  bb5:
    v17 = 0LL;
    v18 = (int64_t)p_str_length((void*)v17, v1);
    memset(v19, 0, 8);
    *(int64_t*)v19 = (int64_t)v18;
    v21 = 0LL;
    v22 = (int64_t)p_str_length((void*)v21, v2);
    memset(v23, 0, 8);
    *(int64_t*)v23 = (int64_t)v22;
    v25 = *(int64_t*)v23;
    v26 = 0LL;
    v27 = v25 == v26;
    if (v27) goto bb6;
    else goto bb7;
  bb6:
    v29 = *(int64_t*)v19;
    return v29;
  bb7:
    goto bb8;
  bb8:
    v32 = *(int64_t*)v23;
    v33 = *(int64_t*)v19;
    v34 = v32 > v33;
    if (v34) goto bb9;
    else goto bb10;
  bb9:
    v36 = -1LL;
    return v36;
  bb10:
    goto bb11;
  bb11:
    v39 = *(int64_t*)v19;
    v40 = *(int64_t*)v23;
    v41 = p_sub_i64(v39, v40);
    memset(v42, 0, 8);
    *(int64_t*)v42 = (int64_t)v41;
    goto bb12;
  bb12:
    p_co_tick();
    v46 = *(int64_t*)v42;
    v47 = 0LL;
    v48 = v46 >= v47;
    if (v48) goto bb13;
    else goto bb14;
  bb13:
    v50 = true;
    memset(v51, 0, 1);
    *(bool*)v51 = v50;
    v53 = 0LL;
    memset(v54, 0, 8);
    *(int64_t*)v54 = (int64_t)v53;
    goto bb15;
  bb14:
    v115 = -1LL;
    return v115;
  bb15:
    p_co_tick();
    v58 = *(int64_t*)v54;
    v59 = *(int64_t*)v23;
    v60 = v58 < v59;
    if (v60) goto bb16;
    else goto bb17;
  bb16:
    v62 = *(int64_t*)v42;
    v63 = p_add_i64(v1, v62);
    v64 = *(int64_t*)v54;
    v65 = p_add_i64(v63, v64);
    memset(v66, 0, 8);
    *(int64_t*)v66 = (int64_t)v65;
    v68 = *(int64_t*)v54;
    v69 = p_add_i64(v2, v68);
    memset(v70, 0, 8);
    *(int64_t*)v70 = (int64_t)v69;
    v72 = *(int64_t*)v66;
    memset(v73, 0, 8);
    *(void**)v73 = (void*)v72;
    v75 = *(int64_t*)v70;
    memset(v76, 0, 8);
    *(void**)v76 = (void*)v75;
    v78 = 0LL;
    memset(v79, 0, 8);
    *(int64_t*)v79 = (int64_t)v78;
    v81 = 0LL;
    memset(v82, 0, 8);
    *(int64_t*)v82 = (int64_t)v81;
    v85 = *(void**)v73;
    v86 = *(uint8_t*)v85;
    *(int64_t*)v79 = (int64_t)v86;
    v88 = *(void**)v76;
    v89 = *(uint8_t*)v88;
    *(int64_t*)v82 = (int64_t)v89;
    v92 = *(int64_t*)v79;
    v93 = *(int64_t*)v82;
    v94 = v92 != v93;
    if (v94) goto bb18;
    else goto bb19;
  bb17:
    v105 = *(bool*)v51;
    if (v105) goto bb21;
    else goto bb22;
  bb18:
    v96 = false;
    *(bool*)v51 = v96;
    goto bb17;
  bb19:
    goto bb20;
  bb20:
    v100 = *(int64_t*)v54;
    v101 = 1LL;
    v102 = (int64_t)((uint64_t)v100 + (uint64_t)v101);
    *(int64_t*)v54 = (int64_t)v102;
    goto bb15;
  bb21:
    v107 = *(int64_t*)v42;
    return v107;
  bb22:
    goto bb23;
  bb23:
    v110 = *(int64_t*)v42;
    v111 = 1LL;
    v112 = p_sub_i64(v110, v111);
    *(int64_t*)v42 = (int64_t)v112;
    goto bb12;
    return 0;
}

int64_t String_slice(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    void* v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    v9 = alloca(8);
    v12 = alloca(8);
    v4 = 0LL;
    v5 = v1 == v4;
    if (v5) goto bb1;
    else goto bb2;
  bb1:
    v7 = 1LL;
    v8 = p_malloc(v7);
    memset(v9, 0, 8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = *(int64_t*)v9;
    memset(v12, 0, 8);
    *(void**)v12 = (void*)v11;
    v15 = *(void**)v12;
    v16 = 0LL;
    *(uint8_t*)v15 = (uint8_t)v16;
    v19 = *(int64_t*)v9;
    return v19;
  bb2:
    goto bb3;
  bb3:
    v22 = 0LL;
    v23 = (int64_t)p_str_length((void*)v22, v1);
    v24 = 0LL;
    v25 = (int64_t)String_slice_len((void*)v24, v1, v23, v2, v3);
    return v25;
    return 0;
}

int64_t String_slice_len(void* v0, int64_t v1, int64_t v2, int64_t v3, int64_t v4) {
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    int64_t v10 = 0;
    bool v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    bool v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    int64_t v24 = 0;
    bool v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    void* v28 = 0;
    void* v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0;
    void* v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0; /* placeholder for void */
    void* v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0;
    void* v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0;
    int64_t v48 = 0;
    int64_t v49 = 0;
    void* v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    int64_t v59 = 0;
    int64_t v60 = 0;
    void* v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    int64_t v63 = 0;
    void* v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0; /* placeholder for void */
    void* v67 = 0;
    int64_t v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    v5 = alloca(8);
    v7 = alloca(8);
    v29 = alloca(8);
    v32 = alloca(8);
    v45 = alloca(8);
    v50 = alloca(8);
    v61 = alloca(8);
    v64 = alloca(8);
    memset(v5, 0, 8);
    *(int64_t*)v5 = (int64_t)v3;
    memset(v7, 0, 8);
    *(int64_t*)v7 = (int64_t)v4;
    v9 = *(int64_t*)v5;
    v10 = 0LL;
    v11 = v9 < v10;
    if (v11) goto bb1;
    else goto bb2;
  bb1:
    v13 = 0LL;
    *(int64_t*)v5 = (int64_t)v13;
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v17 = *(int64_t*)v7;
    v18 = v17 > v2;
    if (v18) goto bb4;
    else goto bb5;
  bb4:
    *(int64_t*)v7 = (int64_t)v2;
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v23 = *(int64_t*)v5;
    v24 = *(int64_t*)v7;
    v25 = v23 >= v24;
    if (v25) goto bb7;
    else goto bb8;
  bb7:
    v27 = 1LL;
    v28 = p_malloc(v27);
    memset(v29, 0, 8);
    *(int64_t*)v29 = (int64_t)v28;
    v31 = *(int64_t*)v29;
    memset(v32, 0, 8);
    *(void**)v32 = (void*)v31;
    v35 = *(void**)v32;
    v36 = 0LL;
    *(uint8_t*)v35 = (uint8_t)v36;
    v39 = *(int64_t*)v29;
    return v39;
  bb8:
    goto bb9;
  bb9:
    v42 = *(int64_t*)v7;
    v43 = *(int64_t*)v5;
    v44 = p_sub_i64(v42, v43);
    memset(v45, 0, 8);
    *(int64_t*)v45 = (int64_t)v44;
    v47 = *(int64_t*)v45;
    v48 = 0LL;
    v49 = (int64_t)p_str_alloc((void*)v48, v47);
    memset(v50, 0, 8);
    *(int64_t*)v50 = (int64_t)v49;
    v52 = *(int64_t*)v50;
    v53 = *(int64_t*)v5;
    v54 = p_add_i64(v1, v53);
    v55 = *(int64_t*)v45;
    v56 = 0LL;
    p_str_copy_n((void*)v56, v52, v54, v55);
    v58 = *(int64_t*)v50;
    v59 = *(int64_t*)v45;
    v60 = p_add_i64(v58, v59);
    memset(v61, 0, 8);
    *(int64_t*)v61 = (int64_t)v60;
    v63 = *(int64_t*)v61;
    memset(v64, 0, 8);
    *(void**)v64 = (void*)v63;
    v67 = *(void**)v64;
    v68 = 0LL;
    *(uint8_t*)v67 = (uint8_t)v68;
    v71 = *(int64_t*)v50;
    return v71;
    return 0;
}

int64_t String_split(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0;
    bool v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0; /* placeholder for void */
    bool v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    int64_t v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0;
    int64_t v26 = 0;
    bool v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    void* v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0;
    int64_t v36 = 0;
    void* v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    void* v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0;
    bool v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    bool v50 = 0;
    void* v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    void* v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    int64_t v59 = 0;
    bool v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0;
    int64_t v65 = 0;
    void* v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    int64_t v69 = 0;
    void* v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0;
    void* v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0;
    void* v76 = 0;
    int64_t v77 = 0; /* placeholder for void */
    int64_t v78 = 0;
    void* v79 = 0;
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0;
    void* v82 = 0;
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0; /* placeholder for void */
    void* v85 = 0;
    uint8_t v86 = 0;
    int64_t v87 = 0; /* placeholder for void */
    void* v88 = 0;
    uint8_t v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0;
    int64_t v93 = 0;
    bool v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    bool v96 = 0;
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0; /* placeholder for void */
    int64_t v99 = 0; /* placeholder for void */
    int64_t v100 = 0;
    int64_t v101 = 0;
    int64_t v102 = 0;
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0; /* placeholder for void */
    bool v105 = 0;
    int64_t v106 = 0; /* placeholder for void */
    int64_t v107 = 0;
    int64_t v108 = 0;
    int64_t v109 = 0;
    void* v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    int64_t v112 = 0;
    int64_t v113 = 0;
    int64_t v114 = 0;
    void* v115 = 0;
    void* v116 = 0;
    int64_t v117 = 0; /* placeholder for void */
    int64_t v118 = 0;
    int64_t v119 = 0;
    int64_t v120 = 0;
    int64_t v121 = 0;
    int64_t v122 = 0;
    int64_t v123 = 0; /* placeholder for void */
    int64_t v124 = 0;
    int64_t v125 = 0;
    int64_t v126 = 0;
    void* v127 = 0;
    int64_t v128 = 0; /* placeholder for void */
    int64_t v129 = 0;
    void* v130 = 0;
    int64_t v131 = 0; /* placeholder for void */
    int64_t v132 = 0; /* placeholder for void */
    void* v133 = 0;
    int64_t v134 = 0;
    int64_t v135 = 0; /* placeholder for void */
    int64_t v136 = 0; /* placeholder for void */
    int64_t v137 = 0;
    int64_t v138 = 0;
    int64_t v139 = 0;
    int64_t v140 = 0; /* placeholder for void */
    int64_t v141 = 0;
    int64_t v142 = 0;
    int64_t v143 = 0;
    int64_t v144 = 0; /* placeholder for void */
    int64_t v145 = 0;
    int64_t v146 = 0; /* placeholder for void */
    int64_t v147 = 0; /* placeholder for void */
    int64_t v148 = 0;
    int64_t v149 = 0;
    int64_t v150 = 0;
    int64_t v151 = 0; /* placeholder for void */
    int64_t v152 = 0; /* placeholder for void */
    int64_t v153 = 0; /* placeholder for void */
    int64_t v154 = 0;
    int64_t v155 = 0;
    int64_t v156 = 0;
    void* v157 = 0;
    int64_t v158 = 0; /* placeholder for void */
    int64_t v159 = 0;
    int64_t v160 = 0;
    bool v161 = 0;
    int64_t v162 = 0; /* placeholder for void */
    int64_t v163 = 0;
    int64_t v164 = 0;
    int64_t v165 = 0;
    void* v166 = 0;
    void* v167 = 0;
    int64_t v168 = 0; /* placeholder for void */
    int64_t v169 = 0;
    int64_t v170 = 0;
    int64_t v171 = 0;
    int64_t v172 = 0;
    int64_t v173 = 0;
    int64_t v174 = 0; /* placeholder for void */
    int64_t v175 = 0;
    int64_t v176 = 0;
    int64_t v177 = 0;
    void* v178 = 0;
    int64_t v179 = 0; /* placeholder for void */
    int64_t v180 = 0;
    void* v181 = 0;
    int64_t v182 = 0; /* placeholder for void */
    int64_t v183 = 0; /* placeholder for void */
    void* v184 = 0;
    int64_t v185 = 0;
    int64_t v186 = 0; /* placeholder for void */
    int64_t v187 = 0; /* placeholder for void */
    int64_t v188 = 0;
    int64_t v189 = 0;
    int64_t v190 = 0;
    int64_t v191 = 0; /* placeholder for void */
    int64_t v192 = 0; /* placeholder for void */
    int64_t v193 = 0; /* placeholder for void */
    int64_t v194 = 0;
    int64_t v195 = 0; /* placeholder for void */
    v5 = alloca(8);
    v7 = alloca(1);
    v23 = alloca(8);
    v33 = alloca(8);
    v37 = alloca(8);
    v40 = alloca(8);
    v51 = alloca(1);
    v54 = alloca(8);
    v157 = alloca(8);
    v66 = alloca(8);
    v70 = alloca(8);
    v73 = alloca(8);
    v76 = alloca(8);
    v79 = alloca(8);
    v82 = alloca(8);
    v110 = alloca(8);
    v116 = alloca(8);
    v127 = alloca(8);
    v130 = alloca(8);
    v167 = alloca(8);
    v178 = alloca(8);
    v181 = alloca(8);
    v3 = 0LL;
    v4 = (int64_t)Vec_new((void*)v3);
    memset(v5, 0, 8);
    *(int64_t*)v5 = (int64_t)v4;
    memset(v7, 0, 1);
    v8 = 0LL;
    v9 = v1 == v8;
    *(bool*)v7 = v9;
    if (v9) goto bb2;
    else goto bb1;
  bb1:
    v12 = 0LL;
    v13 = v2 == v12;
    *(bool*)v7 = v13;
    goto bb2;
  bb2:
    v16 = *(bool*)v7;
    if (v16) goto bb3;
    else goto bb4;
  bb3:
    v18 = *(int64_t*)v5;
    return v18;
  bb4:
    goto bb5;
  bb5:
    v21 = 0LL;
    v22 = (int64_t)p_str_length((void*)v21, v2);
    memset(v23, 0, 8);
    *(int64_t*)v23 = (int64_t)v22;
    v25 = *(int64_t*)v23;
    v26 = 0LL;
    v27 = v25 == v26;
    if (v27) goto bb6;
    else goto bb7;
  bb6:
    v29 = *(int64_t*)v5;
    return v29;
  bb7:
    goto bb8;
  bb8:
    v32 = 0LL;
    memset(v33, 0, 8);
    *(int64_t*)v33 = (int64_t)v32;
    v35 = 0LL;
    v36 = (int64_t)p_str_length((void*)v35, v1);
    memset(v37, 0, 8);
    *(int64_t*)v37 = (int64_t)v36;
    v39 = 0LL;
    memset(v40, 0, 8);
    *(int64_t*)v40 = (int64_t)v39;
    goto bb9;
  bb9:
    p_co_tick();
    v44 = *(int64_t*)v40;
    v45 = *(int64_t*)v37;
    v46 = *(int64_t*)v23;
    v47 = p_sub_i64(v45, v46);
    v48 = v44 <= v47;
    if (v48) goto bb10;
    else goto bb11;
  bb10:
    v50 = true;
    memset(v51, 0, 1);
    *(bool*)v51 = v50;
    v53 = 0LL;
    memset(v54, 0, 8);
    *(int64_t*)v54 = (int64_t)v53;
    goto bb12;
  bb11:
    v154 = *(int64_t*)v37;
    v155 = *(int64_t*)v33;
    v156 = p_sub_i64(v154, v155);
    memset(v157, 0, 8);
    *(int64_t*)v157 = (int64_t)v156;
    v159 = *(int64_t*)v157;
    v160 = 0LL;
    v161 = v159 >= v160;
    if (v161) goto bb21;
    else goto bb22;
  bb12:
    p_co_tick();
    v58 = *(int64_t*)v54;
    v59 = *(int64_t*)v23;
    v60 = v58 < v59;
    if (v60) goto bb13;
    else goto bb14;
  bb13:
    v62 = *(int64_t*)v40;
    v63 = p_add_i64(v1, v62);
    v64 = *(int64_t*)v54;
    v65 = p_add_i64(v63, v64);
    memset(v66, 0, 8);
    *(int64_t*)v66 = (int64_t)v65;
    v68 = *(int64_t*)v54;
    v69 = p_add_i64(v2, v68);
    memset(v70, 0, 8);
    *(int64_t*)v70 = (int64_t)v69;
    v72 = *(int64_t*)v66;
    memset(v73, 0, 8);
    *(void**)v73 = (void*)v72;
    v75 = *(int64_t*)v70;
    memset(v76, 0, 8);
    *(void**)v76 = (void*)v75;
    v78 = 0LL;
    memset(v79, 0, 8);
    *(int64_t*)v79 = (int64_t)v78;
    v81 = 0LL;
    memset(v82, 0, 8);
    *(int64_t*)v82 = (int64_t)v81;
    v85 = *(void**)v73;
    v86 = *(uint8_t*)v85;
    *(int64_t*)v79 = (int64_t)v86;
    v88 = *(void**)v76;
    v89 = *(uint8_t*)v88;
    *(int64_t*)v82 = (int64_t)v89;
    v92 = *(int64_t*)v79;
    v93 = *(int64_t*)v82;
    v94 = v92 != v93;
    if (v94) goto bb15;
    else goto bb16;
  bb14:
    v105 = *(bool*)v51;
    if (v105) goto bb18;
    else goto bb19;
  bb15:
    v96 = false;
    *(bool*)v51 = v96;
    goto bb14;
  bb16:
    goto bb17;
  bb17:
    v100 = *(int64_t*)v54;
    v101 = 1LL;
    v102 = (int64_t)((uint64_t)v100 + (uint64_t)v101);
    *(int64_t*)v54 = (int64_t)v102;
    goto bb12;
  bb18:
    v107 = *(int64_t*)v40;
    v108 = *(int64_t*)v33;
    v109 = p_sub_i64(v107, v108);
    memset(v110, 0, 8);
    *(int64_t*)v110 = (int64_t)v109;
    v112 = *(int64_t*)v110;
    v113 = 1LL;
    v114 = p_add_i64(v112, v113);
    v115 = p_malloc(v114);
    memset(v116, 0, 8);
    *(int64_t*)v116 = (int64_t)v115;
    v118 = *(int64_t*)v116;
    v119 = *(int64_t*)v33;
    v120 = p_add_i64(v1, v119);
    v121 = *(int64_t*)v110;
    v122 = 0LL;
    p_str_copy_n((void*)v122, v118, v120, v121);
    v124 = *(int64_t*)v116;
    v125 = *(int64_t*)v110;
    v126 = p_add_i64(v124, v125);
    memset(v127, 0, 8);
    *(int64_t*)v127 = (int64_t)v126;
    v129 = *(int64_t*)v127;
    memset(v130, 0, 8);
    *(void**)v130 = (void*)v129;
    v133 = *(void**)v130;
    v134 = 0LL;
    *(uint8_t*)v133 = (uint8_t)v134;
    v137 = *(int64_t*)v5;
    v138 = *(int64_t*)v116;
    v139 = 0LL;
    Vec_push((void*)v139, v137, v138);
    v141 = *(int64_t*)v40;
    v142 = *(int64_t*)v23;
    v143 = p_add_i64(v141, v142);
    *(int64_t*)v33 = (int64_t)v143;
    v145 = *(int64_t*)v33;
    *(int64_t*)v40 = (int64_t)v145;
    goto bb20;
  bb19:
    v148 = *(int64_t*)v40;
    v149 = 1LL;
    v150 = p_add_i64(v148, v149);
    *(int64_t*)v40 = (int64_t)v150;
    goto bb20;
  bb20:
    goto bb9;
  bb21:
    v163 = *(int64_t*)v157;
    v164 = 1LL;
    v165 = p_add_i64(v163, v164);
    v166 = p_malloc(v165);
    memset(v167, 0, 8);
    *(int64_t*)v167 = (int64_t)v166;
    v169 = *(int64_t*)v167;
    v170 = *(int64_t*)v33;
    v171 = p_add_i64(v1, v170);
    v172 = *(int64_t*)v157;
    v173 = 0LL;
    p_str_copy_n((void*)v173, v169, v171, v172);
    v175 = *(int64_t*)v167;
    v176 = *(int64_t*)v157;
    v177 = p_add_i64(v175, v176);
    memset(v178, 0, 8);
    *(int64_t*)v178 = (int64_t)v177;
    v180 = *(int64_t*)v178;
    memset(v181, 0, 8);
    *(void**)v181 = (void*)v180;
    v184 = *(void**)v181;
    v185 = 0LL;
    *(uint8_t*)v184 = (uint8_t)v185;
    v188 = *(int64_t*)v5;
    v189 = *(int64_t*)v167;
    v190 = 0LL;
    Vec_push((void*)v190, v188, v189);
    goto bb23;
  bb22:
    goto bb23;
  bb23:
    v194 = *(int64_t*)v5;
    return v194;
    return 0;
}

bool String_starts_with(void* v0, int64_t v1, int64_t v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0; /* placeholder for void */
    bool v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    int64_t v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0;
    int64_t v26 = 0;
    bool v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    bool v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    void* v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    int64_t v38 = 0;
    bool v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    int64_t v42 = 0;
    void* v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0;
    int64_t v46 = 0;
    void* v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    void* v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    void* v53 = 0;
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0;
    void* v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    void* v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0; /* placeholder for void */
    void* v62 = 0;
    uint8_t v63 = 0;
    int64_t v64 = 0; /* placeholder for void */
    void* v65 = 0;
    uint8_t v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0;
    int64_t v70 = 0;
    bool v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    bool v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0;
    int64_t v77 = 0;
    int64_t v78 = 0;
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0; /* placeholder for void */
    bool v81 = 0;
    int64_t v82 = 0; /* placeholder for void */
    v3 = alloca(1);
    v19 = alloca(8);
    v23 = alloca(8);
    v33 = alloca(8);
    v43 = alloca(8);
    v47 = alloca(8);
    v50 = alloca(8);
    v53 = alloca(8);
    v56 = alloca(8);
    v59 = alloca(8);
    memset(v3, 0, 1);
    v4 = 0LL;
    v5 = v1 == v4;
    *(bool*)v3 = v5;
    if (v5) goto bb2;
    else goto bb1;
  bb1:
    v8 = 0LL;
    v9 = v2 == v8;
    *(bool*)v3 = v9;
    goto bb2;
  bb2:
    v12 = *(bool*)v3;
    if (v12) goto bb3;
    else goto bb4;
  bb3:
    v14 = false;
    return v14;
  bb4:
    goto bb5;
  bb5:
    v17 = 0LL;
    v18 = (int64_t)p_str_length((void*)v17, v1);
    memset(v19, 0, 8);
    *(int64_t*)v19 = (int64_t)v18;
    v21 = 0LL;
    v22 = (int64_t)p_str_length((void*)v21, v2);
    memset(v23, 0, 8);
    *(int64_t*)v23 = (int64_t)v22;
    v25 = *(int64_t*)v23;
    v26 = *(int64_t*)v19;
    v27 = v25 > v26;
    if (v27) goto bb6;
    else goto bb7;
  bb6:
    v29 = false;
    return v29;
  bb7:
    goto bb8;
  bb8:
    v32 = 0LL;
    memset(v33, 0, 8);
    *(int64_t*)v33 = (int64_t)v32;
    goto bb9;
  bb9:
    p_co_tick();
    v37 = *(int64_t*)v33;
    v38 = *(int64_t*)v23;
    v39 = v37 < v38;
    if (v39) goto bb10;
    else goto bb11;
  bb10:
    v41 = *(int64_t*)v33;
    v42 = p_add_i64(v1, v41);
    memset(v43, 0, 8);
    *(int64_t*)v43 = (int64_t)v42;
    v45 = *(int64_t*)v33;
    v46 = p_add_i64(v2, v45);
    memset(v47, 0, 8);
    *(int64_t*)v47 = (int64_t)v46;
    v49 = *(int64_t*)v43;
    memset(v50, 0, 8);
    *(void**)v50 = (void*)v49;
    v52 = *(int64_t*)v47;
    memset(v53, 0, 8);
    *(void**)v53 = (void*)v52;
    v55 = 0LL;
    memset(v56, 0, 8);
    *(int64_t*)v56 = (int64_t)v55;
    v58 = 0LL;
    memset(v59, 0, 8);
    *(int64_t*)v59 = (int64_t)v58;
    v62 = *(void**)v50;
    v63 = *(uint8_t*)v62;
    *(int64_t*)v56 = (int64_t)v63;
    v65 = *(void**)v53;
    v66 = *(uint8_t*)v65;
    *(int64_t*)v59 = (int64_t)v66;
    v69 = *(int64_t*)v56;
    v70 = *(int64_t*)v59;
    v71 = v69 != v70;
    if (v71) goto bb12;
    else goto bb13;
  bb11:
    v81 = true;
    return v81;
  bb12:
    v73 = false;
    return v73;
  bb13:
    goto bb14;
  bb14:
    v76 = *(int64_t*)v33;
    v77 = 1LL;
    v78 = (int64_t)((uint64_t)v76 + (uint64_t)v77);
    *(int64_t*)v33 = (int64_t)v78;
    goto bb9;
    return 0;
}

int64_t String_to_lowercase(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    void* v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    void* v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    int64_t v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0;
    void* v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0;
    void* v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0;
    int64_t v36 = 0;
    bool v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    void* v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    void* v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0;
    void* v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    void* v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    void* v58 = 0;
    uint8_t v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0; /* placeholder for void */
    void* v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0;
    bool v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    int64_t v69 = 0;
    bool v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0; /* placeholder for void */
    bool v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0;
    int64_t v76 = 0;
    int64_t v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0; /* placeholder for void */
    void* v82 = 0;
    int64_t v83 = 0;
    int64_t v84 = 0; /* placeholder for void */
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0;
    int64_t v87 = 0;
    int64_t v88 = 0;
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0;
    int64_t v92 = 0;
    int64_t v93 = 0;
    void* v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0;
    void* v97 = 0;
    int64_t v98 = 0; /* placeholder for void */
    int64_t v99 = 0; /* placeholder for void */
    void* v100 = 0;
    int64_t v101 = 0;
    int64_t v102 = 0; /* placeholder for void */
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0;
    int64_t v105 = 0; /* placeholder for void */
    v7 = alloca(8);
    v10 = alloca(8);
    v22 = alloca(8);
    v28 = alloca(8);
    v31 = alloca(8);
    v41 = alloca(8);
    v46 = alloca(8);
    v49 = alloca(8);
    v52 = alloca(8);
    v55 = alloca(8);
    v62 = alloca(1);
    v94 = alloca(8);
    v97 = alloca(8);
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 1LL;
    v6 = p_malloc(v5);
    memset(v7, 0, 8);
    *(int64_t*)v7 = (int64_t)v6;
    v9 = *(int64_t*)v7;
    memset(v10, 0, 8);
    *(void**)v10 = (void*)v9;
    v13 = *(void**)v10;
    v14 = 0LL;
    *(uint8_t*)v13 = (uint8_t)v14;
    v17 = *(int64_t*)v7;
    return v17;
  bb2:
    goto bb3;
  bb3:
    v20 = 0LL;
    v21 = (int64_t)p_str_length((void*)v20, v1);
    memset(v22, 0, 8);
    *(int64_t*)v22 = (int64_t)v21;
    v24 = *(int64_t*)v22;
    v25 = 1LL;
    v26 = p_add_i64(v24, v25);
    v27 = p_malloc(v26);
    memset(v28, 0, 8);
    *(int64_t*)v28 = (int64_t)v27;
    v30 = 0LL;
    memset(v31, 0, 8);
    *(int64_t*)v31 = (int64_t)v30;
    goto bb4;
  bb4:
    p_co_tick();
    v35 = *(int64_t*)v31;
    v36 = *(int64_t*)v22;
    v37 = v35 < v36;
    if (v37) goto bb5;
    else goto bb6;
  bb5:
    v39 = *(int64_t*)v31;
    v40 = p_add_i64(v1, v39);
    memset(v41, 0, 8);
    *(int64_t*)v41 = (int64_t)v40;
    v43 = *(int64_t*)v28;
    v44 = *(int64_t*)v31;
    v45 = p_add_i64(v43, v44);
    memset(v46, 0, 8);
    *(int64_t*)v46 = (int64_t)v45;
    v48 = *(int64_t*)v41;
    memset(v49, 0, 8);
    *(void**)v49 = (void*)v48;
    v51 = *(int64_t*)v46;
    memset(v52, 0, 8);
    *(void**)v52 = (void*)v51;
    v54 = 0LL;
    memset(v55, 0, 8);
    *(int64_t*)v55 = (int64_t)v54;
    v58 = *(void**)v49;
    v59 = *(uint8_t*)v58;
    *(int64_t*)v55 = (int64_t)v59;
    memset(v62, 0, 1);
    v63 = *(int64_t*)v55;
    v64 = 65LL;
    v65 = v63 >= v64;
    *(bool*)v62 = v65;
    if (v65) goto bb7;
    else goto bb8;
  bb6:
    v91 = *(int64_t*)v28;
    v92 = *(int64_t*)v22;
    v93 = p_add_i64(v91, v92);
    memset(v94, 0, 8);
    *(int64_t*)v94 = (int64_t)v93;
    v96 = *(int64_t*)v94;
    memset(v97, 0, 8);
    *(void**)v97 = (void*)v96;
    v100 = *(void**)v97;
    v101 = 0LL;
    *(uint8_t*)v100 = (uint8_t)v101;
    v104 = *(int64_t*)v28;
    return v104;
  bb7:
    v68 = *(int64_t*)v55;
    v69 = 90LL;
    v70 = v68 <= v69;
    *(bool*)v62 = v70;
    goto bb8;
  bb8:
    v73 = *(bool*)v62;
    if (v73) goto bb9;
    else goto bb10;
  bb9:
    v75 = *(int64_t*)v55;
    v76 = 32LL;
    v77 = p_add_i64(v75, v76);
    *(int64_t*)v55 = (int64_t)v77;
    goto bb11;
  bb10:
    goto bb11;
  bb11:
    v82 = *(void**)v52;
    v83 = *(int64_t*)v55;
    *(uint8_t*)v82 = (uint8_t)v83;
    v86 = *(int64_t*)v31;
    v87 = 1LL;
    v88 = (int64_t)((uint64_t)v86 + (uint64_t)v87);
    *(int64_t*)v31 = (int64_t)v88;
    goto bb4;
    return 0;
}

int64_t String_to_string(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_from((void*)v2, v1);
    return v3;
    return 0;
}

int64_t String_to_uppercase(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    void* v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    void* v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    int64_t v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0;
    void* v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0;
    void* v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0;
    int64_t v36 = 0;
    bool v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    void* v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    void* v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0;
    void* v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    void* v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    void* v58 = 0;
    uint8_t v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0; /* placeholder for void */
    void* v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0;
    bool v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    int64_t v69 = 0;
    bool v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0; /* placeholder for void */
    bool v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0;
    int64_t v76 = 0;
    int64_t v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0; /* placeholder for void */
    void* v82 = 0;
    int64_t v83 = 0;
    int64_t v84 = 0; /* placeholder for void */
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0;
    int64_t v87 = 0;
    int64_t v88 = 0;
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0;
    int64_t v92 = 0;
    int64_t v93 = 0;
    void* v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0;
    void* v97 = 0;
    int64_t v98 = 0; /* placeholder for void */
    int64_t v99 = 0; /* placeholder for void */
    void* v100 = 0;
    int64_t v101 = 0;
    int64_t v102 = 0; /* placeholder for void */
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0;
    int64_t v105 = 0; /* placeholder for void */
    v7 = alloca(8);
    v10 = alloca(8);
    v22 = alloca(8);
    v28 = alloca(8);
    v31 = alloca(8);
    v41 = alloca(8);
    v46 = alloca(8);
    v49 = alloca(8);
    v52 = alloca(8);
    v55 = alloca(8);
    v62 = alloca(1);
    v94 = alloca(8);
    v97 = alloca(8);
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 1LL;
    v6 = p_malloc(v5);
    memset(v7, 0, 8);
    *(int64_t*)v7 = (int64_t)v6;
    v9 = *(int64_t*)v7;
    memset(v10, 0, 8);
    *(void**)v10 = (void*)v9;
    v13 = *(void**)v10;
    v14 = 0LL;
    *(uint8_t*)v13 = (uint8_t)v14;
    v17 = *(int64_t*)v7;
    return v17;
  bb2:
    goto bb3;
  bb3:
    v20 = 0LL;
    v21 = (int64_t)p_str_length((void*)v20, v1);
    memset(v22, 0, 8);
    *(int64_t*)v22 = (int64_t)v21;
    v24 = *(int64_t*)v22;
    v25 = 1LL;
    v26 = p_add_i64(v24, v25);
    v27 = p_malloc(v26);
    memset(v28, 0, 8);
    *(int64_t*)v28 = (int64_t)v27;
    v30 = 0LL;
    memset(v31, 0, 8);
    *(int64_t*)v31 = (int64_t)v30;
    goto bb4;
  bb4:
    p_co_tick();
    v35 = *(int64_t*)v31;
    v36 = *(int64_t*)v22;
    v37 = v35 < v36;
    if (v37) goto bb5;
    else goto bb6;
  bb5:
    v39 = *(int64_t*)v31;
    v40 = p_add_i64(v1, v39);
    memset(v41, 0, 8);
    *(int64_t*)v41 = (int64_t)v40;
    v43 = *(int64_t*)v28;
    v44 = *(int64_t*)v31;
    v45 = p_add_i64(v43, v44);
    memset(v46, 0, 8);
    *(int64_t*)v46 = (int64_t)v45;
    v48 = *(int64_t*)v41;
    memset(v49, 0, 8);
    *(void**)v49 = (void*)v48;
    v51 = *(int64_t*)v46;
    memset(v52, 0, 8);
    *(void**)v52 = (void*)v51;
    v54 = 0LL;
    memset(v55, 0, 8);
    *(int64_t*)v55 = (int64_t)v54;
    v58 = *(void**)v49;
    v59 = *(uint8_t*)v58;
    *(int64_t*)v55 = (int64_t)v59;
    memset(v62, 0, 1);
    v63 = *(int64_t*)v55;
    v64 = 97LL;
    v65 = v63 >= v64;
    *(bool*)v62 = v65;
    if (v65) goto bb7;
    else goto bb8;
  bb6:
    v91 = *(int64_t*)v28;
    v92 = *(int64_t*)v22;
    v93 = p_add_i64(v91, v92);
    memset(v94, 0, 8);
    *(int64_t*)v94 = (int64_t)v93;
    v96 = *(int64_t*)v94;
    memset(v97, 0, 8);
    *(void**)v97 = (void*)v96;
    v100 = *(void**)v97;
    v101 = 0LL;
    *(uint8_t*)v100 = (uint8_t)v101;
    v104 = *(int64_t*)v28;
    return v104;
  bb7:
    v68 = *(int64_t*)v55;
    v69 = 122LL;
    v70 = v68 <= v69;
    *(bool*)v62 = v70;
    goto bb8;
  bb8:
    v73 = *(bool*)v62;
    if (v73) goto bb9;
    else goto bb10;
  bb9:
    v75 = *(int64_t*)v55;
    v76 = 32LL;
    v77 = p_sub_i64(v75, v76);
    *(int64_t*)v55 = (int64_t)v77;
    goto bb11;
  bb10:
    goto bb11;
  bb11:
    v82 = *(void**)v52;
    v83 = *(int64_t*)v55;
    *(uint8_t*)v82 = (uint8_t)v83;
    v86 = *(int64_t*)v31;
    v87 = 1LL;
    v88 = (int64_t)((uint64_t)v86 + (uint64_t)v87);
    *(int64_t*)v31 = (int64_t)v88;
    goto bb4;
    return 0;
}

int64_t String_trim(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    void* v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    void* v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    int64_t v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    void* v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    void* v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    void* v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    void* v36 = 0;
    uint8_t v37 = 0;
    void* v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0; /* placeholder for void */
    void* v42 = 0;
    void* v43 = 0;
    void* v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    bool v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0;
    int64_t v51 = 0;
    bool v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0; /* placeholder for void */
    bool v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    int64_t v59 = 0;
    bool v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0; /* placeholder for void */
    bool v63 = 0;
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0;
    int64_t v67 = 0;
    bool v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0; /* placeholder for void */
    bool v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0;
    int64_t v74 = 0;
    int64_t v75 = 0;
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0;
    int64_t v78 = 0;
    int64_t v79 = 0;
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0;
    int64_t v82 = 0; /* placeholder for void */
    void* v83 = 0;
    uint8_t v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0; /* placeholder for void */
    int64_t v87 = 0; /* placeholder for void */
    int64_t v88 = 0;
    int64_t v89 = 0;
    bool v90 = 0;
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0;
    void* v93 = 0;
    void* v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0;
    void* v97 = 0;
    int64_t v98 = 0; /* placeholder for void */
    int64_t v99 = 0; /* placeholder for void */
    void* v100 = 0;
    int64_t v101 = 0;
    int64_t v102 = 0; /* placeholder for void */
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0;
    int64_t v105 = 0; /* placeholder for void */
    int64_t v106 = 0; /* placeholder for void */
    int64_t v107 = 0;
    int64_t v108 = 0;
    int64_t v109 = 0;
    int64_t v110 = 0;
    void* v111 = 0;
    int64_t v112 = 0; /* placeholder for void */
    int64_t v113 = 0;
    void* v114 = 0;
    int64_t v115 = 0; /* placeholder for void */
    int64_t v116 = 0; /* placeholder for void */
    void* v117 = 0;
    uint8_t v118 = 0;
    void* v119 = 0;
    int64_t v120 = 0; /* placeholder for void */
    int64_t v121 = 0; /* placeholder for void */
    int64_t v122 = 0; /* placeholder for void */
    void* v123 = 0;
    void* v124 = 0;
    void* v125 = 0;
    int64_t v126 = 0;
    int64_t v127 = 0;
    bool v128 = 0;
    int64_t v129 = 0; /* placeholder for void */
    int64_t v130 = 0; /* placeholder for void */
    int64_t v131 = 0;
    int64_t v132 = 0;
    bool v133 = 0;
    int64_t v134 = 0; /* placeholder for void */
    int64_t v135 = 0; /* placeholder for void */
    bool v136 = 0;
    int64_t v137 = 0; /* placeholder for void */
    int64_t v138 = 0; /* placeholder for void */
    int64_t v139 = 0;
    int64_t v140 = 0;
    bool v141 = 0;
    int64_t v142 = 0; /* placeholder for void */
    int64_t v143 = 0; /* placeholder for void */
    bool v144 = 0;
    int64_t v145 = 0; /* placeholder for void */
    int64_t v146 = 0; /* placeholder for void */
    int64_t v147 = 0;
    int64_t v148 = 0;
    bool v149 = 0;
    int64_t v150 = 0; /* placeholder for void */
    int64_t v151 = 0; /* placeholder for void */
    bool v152 = 0;
    int64_t v153 = 0; /* placeholder for void */
    int64_t v154 = 0;
    int64_t v155 = 0;
    int64_t v156 = 0;
    int64_t v157 = 0; /* placeholder for void */
    int64_t v158 = 0;
    int64_t v159 = 0;
    bool v160 = 0;
    int64_t v161 = 0; /* placeholder for void */
    int64_t v162 = 0; /* placeholder for void */
    int64_t v163 = 0; /* placeholder for void */
    int64_t v164 = 0;
    int64_t v165 = 0;
    int64_t v166 = 0;
    int64_t v167 = 0;
    int64_t v168 = 0; /* placeholder for void */
    int64_t v169 = 0;
    int64_t v170 = 0; /* placeholder for void */
    void* v171 = 0;
    uint8_t v172 = 0;
    int64_t v173 = 0; /* placeholder for void */
    int64_t v174 = 0; /* placeholder for void */
    int64_t v175 = 0; /* placeholder for void */
    int64_t v176 = 0;
    int64_t v177 = 0;
    int64_t v178 = 0;
    void* v179 = 0;
    int64_t v180 = 0; /* placeholder for void */
    int64_t v181 = 0;
    int64_t v182 = 0;
    bool v183 = 0;
    int64_t v184 = 0; /* placeholder for void */
    int64_t v185 = 0;
    void* v186 = 0;
    void* v187 = 0;
    int64_t v188 = 0; /* placeholder for void */
    int64_t v189 = 0;
    void* v190 = 0;
    int64_t v191 = 0; /* placeholder for void */
    int64_t v192 = 0; /* placeholder for void */
    void* v193 = 0;
    int64_t v194 = 0;
    int64_t v195 = 0; /* placeholder for void */
    int64_t v196 = 0; /* placeholder for void */
    int64_t v197 = 0;
    int64_t v198 = 0; /* placeholder for void */
    int64_t v199 = 0; /* placeholder for void */
    int64_t v200 = 0;
    int64_t v201 = 0;
    int64_t v202 = 0;
    void* v203 = 0;
    void* v204 = 0;
    int64_t v205 = 0; /* placeholder for void */
    int64_t v206 = 0;
    int64_t v207 = 0;
    int64_t v208 = 0;
    int64_t v209 = 0;
    int64_t v210 = 0;
    int64_t v211 = 0; /* placeholder for void */
    int64_t v212 = 0;
    int64_t v213 = 0;
    int64_t v214 = 0;
    void* v215 = 0;
    int64_t v216 = 0; /* placeholder for void */
    int64_t v217 = 0;
    void* v218 = 0;
    int64_t v219 = 0; /* placeholder for void */
    int64_t v220 = 0; /* placeholder for void */
    void* v221 = 0;
    int64_t v222 = 0;
    int64_t v223 = 0; /* placeholder for void */
    int64_t v224 = 0; /* placeholder for void */
    int64_t v225 = 0;
    int64_t v226 = 0; /* placeholder for void */
    v7 = alloca(8);
    v10 = alloca(8);
    v22 = alloca(8);
    v25 = alloca(8);
    v28 = alloca(8);
    v30 = alloca(8);
    v33 = alloca(8);
    v38 = alloca(8);
    v42 = alloca(1);
    v43 = alloca(1);
    v44 = alloca(1);
    v94 = alloca(8);
    v97 = alloca(8);
    v111 = alloca(8);
    v114 = alloca(8);
    v119 = alloca(8);
    v123 = alloca(1);
    v124 = alloca(1);
    v125 = alloca(1);
    v179 = alloca(8);
    v187 = alloca(8);
    v190 = alloca(8);
    v204 = alloca(8);
    v215 = alloca(8);
    v218 = alloca(8);
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 1LL;
    v6 = p_malloc(v5);
    memset(v7, 0, 8);
    *(int64_t*)v7 = (int64_t)v6;
    v9 = *(int64_t*)v7;
    memset(v10, 0, 8);
    *(void**)v10 = (void*)v9;
    v13 = *(void**)v10;
    v14 = 0LL;
    *(uint8_t*)v13 = (uint8_t)v14;
    v17 = *(int64_t*)v7;
    return v17;
  bb2:
    goto bb3;
  bb3:
    v20 = 0LL;
    v21 = (int64_t)p_str_length((void*)v20, v1);
    memset(v22, 0, 8);
    *(int64_t*)v22 = (int64_t)v21;
    v24 = 0LL;
    memset(v25, 0, 8);
    *(int64_t*)v25 = (int64_t)v24;
    v27 = *(int64_t*)v22;
    memset(v28, 0, 8);
    *(int64_t*)v28 = (int64_t)v27;
    memset(v30, 0, 8);
    *(int64_t*)v30 = (int64_t)v1;
    v32 = *(int64_t*)v30;
    memset(v33, 0, 8);
    *(void**)v33 = (void*)v32;
    v36 = *(void**)v33;
    v37 = *(uint8_t*)v36;
    memset(v38, 0, 8);
    *(int64_t*)v38 = (int64_t)v37;
    goto bb4;
  bb4:
    p_co_tick();
    memset(v42, 0, 1);
    memset(v43, 0, 1);
    memset(v44, 0, 1);
    v45 = *(int64_t*)v38;
    v46 = 32LL;
    v47 = v45 == v46;
    *(bool*)v44 = v47;
    if (v47) goto bb8;
    else goto bb7;
  bb5:
    v73 = *(int64_t*)v25;
    v74 = 1LL;
    v75 = p_add_i64(v73, v74);
    *(int64_t*)v25 = (int64_t)v75;
    v77 = *(int64_t*)v30;
    v78 = 1LL;
    v79 = p_add_i64(v77, v78);
    *(int64_t*)v30 = (int64_t)v79;
    v81 = *(int64_t*)v30;
    *(void**)v33 = (void*)v81;
    v83 = *(void**)v33;
    v84 = *(uint8_t*)v83;
    *(int64_t*)v38 = (int64_t)v84;
    goto bb4;
  bb6:
    v88 = *(int64_t*)v25;
    v89 = *(int64_t*)v28;
    v90 = v88 >= v89;
    if (v90) goto bb13;
    else goto bb14;
  bb7:
    v50 = *(int64_t*)v38;
    v51 = 9LL;
    v52 = v50 == v51;
    *(bool*)v44 = v52;
    goto bb8;
  bb8:
    v55 = *(bool*)v44;
    *(bool*)v43 = v55;
    if (v55) goto bb10;
    else goto bb9;
  bb9:
    v58 = *(int64_t*)v38;
    v59 = 10LL;
    v60 = v58 == v59;
    *(bool*)v43 = v60;
    goto bb10;
  bb10:
    v63 = *(bool*)v43;
    *(bool*)v42 = v63;
    if (v63) goto bb12;
    else goto bb11;
  bb11:
    v66 = *(int64_t*)v38;
    v67 = 13LL;
    v68 = v66 == v67;
    *(bool*)v42 = v68;
    goto bb12;
  bb12:
    v71 = *(bool*)v42;
    if (v71) goto bb5;
    else goto bb6;
  bb13:
    v92 = 1LL;
    v93 = p_malloc(v92);
    memset(v94, 0, 8);
    *(int64_t*)v94 = (int64_t)v93;
    v96 = *(int64_t*)v94;
    memset(v97, 0, 8);
    *(void**)v97 = (void*)v96;
    v100 = *(void**)v97;
    v101 = 0LL;
    *(uint8_t*)v100 = (uint8_t)v101;
    v104 = *(int64_t*)v94;
    return v104;
  bb14:
    goto bb15;
  bb15:
    v107 = *(int64_t*)v28;
    v108 = p_add_i64(v1, v107);
    v109 = 1LL;
    v110 = p_sub_i64(v108, v109);
    memset(v111, 0, 8);
    *(int64_t*)v111 = (int64_t)v110;
    v113 = *(int64_t*)v111;
    memset(v114, 0, 8);
    *(void**)v114 = (void*)v113;
    v117 = *(void**)v114;
    v118 = *(uint8_t*)v117;
    memset(v119, 0, 8);
    *(int64_t*)v119 = (int64_t)v118;
    goto bb16;
  bb16:
    p_co_tick();
    memset(v123, 0, 1);
    memset(v124, 0, 1);
    memset(v125, 0, 1);
    v126 = *(int64_t*)v119;
    v127 = 32LL;
    v128 = v126 == v127;
    *(bool*)v125 = v128;
    if (v128) goto bb20;
    else goto bb19;
  bb17:
    v154 = *(int64_t*)v28;
    v155 = 1LL;
    v156 = p_sub_i64(v154, v155);
    *(int64_t*)v28 = (int64_t)v156;
    v158 = *(int64_t*)v28;
    v159 = *(int64_t*)v25;
    v160 = v158 <= v159;
    if (v160) goto bb25;
    else goto bb26;
  bb18:
    v176 = *(int64_t*)v28;
    v177 = *(int64_t*)v25;
    v178 = p_sub_i64(v176, v177);
    memset(v179, 0, 8);
    *(int64_t*)v179 = (int64_t)v178;
    v181 = *(int64_t*)v179;
    v182 = 0LL;
    v183 = v181 <= v182;
    if (v183) goto bb28;
    else goto bb29;
  bb19:
    v131 = *(int64_t*)v119;
    v132 = 9LL;
    v133 = v131 == v132;
    *(bool*)v125 = v133;
    goto bb20;
  bb20:
    v136 = *(bool*)v125;
    *(bool*)v124 = v136;
    if (v136) goto bb22;
    else goto bb21;
  bb21:
    v139 = *(int64_t*)v119;
    v140 = 10LL;
    v141 = v139 == v140;
    *(bool*)v124 = v141;
    goto bb22;
  bb22:
    v144 = *(bool*)v124;
    *(bool*)v123 = v144;
    if (v144) goto bb24;
    else goto bb23;
  bb23:
    v147 = *(int64_t*)v119;
    v148 = 13LL;
    v149 = v147 == v148;
    *(bool*)v123 = v149;
    goto bb24;
  bb24:
    v152 = *(bool*)v123;
    if (v152) goto bb17;
    else goto bb18;
  bb25:
    goto bb18;
  bb26:
    goto bb27;
  bb27:
    v164 = *(int64_t*)v28;
    v165 = p_add_i64(v1, v164);
    v166 = 1LL;
    v167 = p_sub_i64(v165, v166);
    *(int64_t*)v111 = (int64_t)v167;
    v169 = *(int64_t*)v111;
    *(void**)v114 = (void*)v169;
    v171 = *(void**)v114;
    v172 = *(uint8_t*)v171;
    *(int64_t*)v119 = (int64_t)v172;
    goto bb16;
  bb28:
    v185 = 1LL;
    v186 = p_malloc(v185);
    memset(v187, 0, 8);
    *(int64_t*)v187 = (int64_t)v186;
    v189 = *(int64_t*)v187;
    memset(v190, 0, 8);
    *(void**)v190 = (void*)v189;
    v193 = *(void**)v190;
    v194 = 0LL;
    *(uint8_t*)v193 = (uint8_t)v194;
    v197 = *(int64_t*)v187;
    return v197;
  bb29:
    goto bb30;
  bb30:
    v200 = *(int64_t*)v179;
    v201 = 1LL;
    v202 = p_add_i64(v200, v201);
    v203 = p_malloc(v202);
    memset(v204, 0, 8);
    *(int64_t*)v204 = (int64_t)v203;
    v206 = *(int64_t*)v204;
    v207 = *(int64_t*)v25;
    v208 = p_add_i64(v1, v207);
    v209 = *(int64_t*)v179;
    v210 = 0LL;
    p_str_copy_n((void*)v210, v206, v208, v209);
    v212 = *(int64_t*)v204;
    v213 = *(int64_t*)v179;
    v214 = p_add_i64(v212, v213);
    memset(v215, 0, 8);
    *(int64_t*)v215 = (int64_t)v214;
    v217 = *(int64_t*)v215;
    memset(v218, 0, 8);
    *(void**)v218 = (void*)v217;
    v221 = *(void**)v218;
    v222 = 0LL;
    *(uint8_t*)v221 = (uint8_t)v222;
    v225 = *(int64_t*)v204;
    return v225;
    return 0;
}

int64_t Vec_capacity(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0;
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0;
    int64_t v19 = 0;
    int64_t v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    v4 = alloca(8);
    v6 = alloca(8);
    v2 = 16LL;
    v3 = p_add_i64(v1, v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    memset(v6, 0, 8);
    v8 = *(void**)v4;
    v9 = *(int64_t*)v8;
    *(int64_t*)v6 = (int64_t)v9;
    v12 = *(int64_t*)v6;
    v13 = 0LL;
    v14 = v12 < v13;
    if (v14) goto bb1;
    else goto bb2;
  bb1:
    v16 = 0LL;
    v17 = *(int64_t*)v6;
    v18 = p_sub_i64(v16, v17);
    v19 = 1LL;
    v20 = p_sub_i64(v18, v19);
    *(int64_t*)v6 = (int64_t)v20;
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v24 = *(int64_t*)v6;
    return v24;
    return 0;
}

void Vec_clear(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0; /* placeholder for void */
    v4 = alloca(8);
    v2 = 8LL;
    v3 = p_add_i64(v1, v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    v7 = *(void**)v4;
    v8 = 0LL;
    *(int64_t*)v7 = (int64_t)v8;
    return;
    return;
}

int64_t Vec_clone(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    void* v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    int64_t v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    void* v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0;
    int64_t v30 = 0;
    bool v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0;
    int64_t v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0;
    void* v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    void* v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    void* v47 = 0;
    int64_t v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0;
    int64_t v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    v4 = alloca(8);
    v6 = alloca(8);
    v8 = alloca(8);
    v14 = alloca(8);
    v22 = alloca(8);
    v25 = alloca(8);
    v38 = alloca(8);
    v41 = alloca(8);
    v44 = alloca(8);
    v2 = 8LL;
    v3 = p_add_i64(v1, v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    memset(v6, 0, 8);
    *(void**)v6 = (void*)v1;
    memset(v8, 0, 8);
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v8 = (int64_t)v11;
    memset(v14, 0, 8);
    v16 = *(void**)v6;
    v17 = *(int64_t*)v16;
    *(int64_t*)v14 = (int64_t)v17;
    v20 = 0LL;
    v21 = (int64_t)Vec_new((void*)v20);
    memset(v22, 0, 8);
    *(int64_t*)v22 = (int64_t)v21;
    v24 = 0LL;
    memset(v25, 0, 8);
    *(int64_t*)v25 = (int64_t)v24;
    goto bb1;
  bb1:
    p_co_tick();
    v29 = *(int64_t*)v25;
    v30 = *(int64_t*)v8;
    v31 = v29 < v30;
    if (v31) goto bb2;
    else goto bb3;
  bb2:
    v33 = *(int64_t*)v14;
    v34 = *(int64_t*)v25;
    v35 = 8LL;
    v36 = p_mul_i64(v34, v35);
    v37 = p_add_i64(v33, v36);
    memset(v38, 0, 8);
    *(int64_t*)v38 = (int64_t)v37;
    v40 = *(int64_t*)v38;
    memset(v41, 0, 8);
    *(void**)v41 = (void*)v40;
    v43 = 0LL;
    memset(v44, 0, 8);
    *(int64_t*)v44 = (int64_t)v43;
    v47 = *(void**)v41;
    v48 = *(int64_t*)v47;
    *(int64_t*)v44 = (int64_t)v48;
    v51 = *(int64_t*)v22;
    v52 = *(int64_t*)v44;
    v53 = 0LL;
    Vec_push((void*)v53, v51, v52);
    v55 = *(int64_t*)v25;
    v56 = 1LL;
    v57 = (int64_t)((uint64_t)v55 + (uint64_t)v56);
    *(int64_t*)v25 = (int64_t)v57;
    goto bb1;
  bb3:
    v60 = *(int64_t*)v22;
    return v60;
    return 0;
}

bool Vec_contains(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    int64_t v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    int64_t v27 = 0;
    bool v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0;
    int64_t v34 = 0;
    void* v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    void* v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    void* v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    bool v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    bool v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0; /* placeholder for void */
    bool v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    v5 = alloca(8);
    v7 = alloca(8);
    v9 = alloca(8);
    v15 = alloca(8);
    v22 = alloca(8);
    v35 = alloca(8);
    v38 = alloca(8);
    v41 = alloca(8);
    v3 = 8LL;
    v4 = p_add_i64(v1, v3);
    memset(v5, 0, 8);
    *(void**)v5 = (void*)v4;
    memset(v7, 0, 8);
    *(void**)v7 = (void*)v1;
    memset(v9, 0, 8);
    v11 = *(void**)v5;
    v12 = *(int64_t*)v11;
    *(int64_t*)v9 = (int64_t)v12;
    memset(v15, 0, 8);
    v17 = *(void**)v7;
    v18 = *(int64_t*)v17;
    *(int64_t*)v15 = (int64_t)v18;
    v21 = 0LL;
    memset(v22, 0, 8);
    *(int64_t*)v22 = (int64_t)v21;
    goto bb1;
  bb1:
    p_co_tick();
    v26 = *(int64_t*)v22;
    v27 = *(int64_t*)v9;
    v28 = v26 < v27;
    if (v28) goto bb2;
    else goto bb3;
  bb2:
    v30 = *(int64_t*)v15;
    v31 = *(int64_t*)v22;
    v32 = 8LL;
    v33 = p_mul_i64(v31, v32);
    v34 = p_add_i64(v30, v33);
    memset(v35, 0, 8);
    *(int64_t*)v35 = (int64_t)v34;
    v37 = *(int64_t*)v35;
    memset(v38, 0, 8);
    *(void**)v38 = (void*)v37;
    v40 = 0LL;
    memset(v41, 0, 8);
    *(int64_t*)v41 = (int64_t)v40;
    v44 = *(void**)v38;
    v45 = *(int64_t*)v44;
    *(int64_t*)v41 = (int64_t)v45;
    v48 = *(int64_t*)v41;
    v49 = v48 == v2;
    if (v49) goto bb4;
    else goto bb5;
  bb3:
    v59 = false;
    return v59;
  bb4:
    v51 = true;
    return v51;
  bb5:
    goto bb6;
  bb6:
    v54 = *(int64_t*)v22;
    v55 = 1LL;
    v56 = (int64_t)((uint64_t)v54 + (uint64_t)v55);
    *(int64_t*)v22 = (int64_t)v56;
    goto bb1;
    return 0;
}

void Vec_drop(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    void* v20 = 0;
    int64_t v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    void* v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    void* v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0;
    bool v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0;
    int64_t v34 = 0;
    bool v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0; /* placeholder for void */
    bool v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    v7 = alloca(8);
    v11 = alloca(8);
    v14 = alloca(8);
    v17 = alloca(8);
    v27 = alloca(1);
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    return;
  bb2:
    goto bb3;
  bb3:
    memset(v7, 0, 8);
    *(void**)v7 = (void*)v1;
    v9 = 16LL;
    v10 = p_add_i64(v1, v9);
    memset(v11, 0, 8);
    *(void**)v11 = (void*)v10;
    v13 = 0LL;
    memset(v14, 0, 8);
    *(int64_t*)v14 = (int64_t)v13;
    v16 = 0LL;
    memset(v17, 0, 8);
    *(int64_t*)v17 = (int64_t)v16;
    v20 = *(void**)v7;
    v21 = *(int64_t*)v20;
    *(int64_t*)v14 = (int64_t)v21;
    v23 = *(void**)v11;
    v24 = *(int64_t*)v23;
    *(int64_t*)v17 = (int64_t)v24;
    memset(v27, 0, 1);
    v28 = *(int64_t*)v14;
    v29 = 0LL;
    v30 = v28 != v29;
    *(bool*)v27 = v30;
    if (v30) goto bb4;
    else goto bb5;
  bb4:
    v33 = *(int64_t*)v17;
    v34 = 0LL;
    v35 = v33 >= v34;
    *(bool*)v27 = v35;
    goto bb5;
  bb5:
    v38 = *(bool*)v27;
    if (v38) goto bb6;
    else goto bb7;
  bb6:
    v40 = *(int64_t*)v14;
    p_free((void*)v40);
    goto bb8;
  bb7:
    goto bb8;
  bb8:
    p_free((void*)v1);
    return;
    return;
}

void Vec_drop_strs(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    int64_t v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    int64_t v20 = 0;
    bool v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    int64_t v24 = 0;
    void* v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    int64_t v33 = 0;
    bool v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    v9 = alloca(8);
    v12 = alloca(8);
    v25 = alloca(8);
    v28 = alloca(8);
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    return;
  bb2:
    goto bb3;
  bb3:
    v7 = 16LL;
    v8 = p_add_i64(v1, v7);
    memset(v9, 0, 8);
    *(void**)v9 = (void*)v8;
    v11 = 0LL;
    memset(v12, 0, 8);
    *(int64_t*)v12 = (int64_t)v11;
    v15 = *(void**)v9;
    v16 = *(int64_t*)v15;
    *(int64_t*)v12 = (int64_t)v16;
    v19 = *(int64_t*)v12;
    v20 = 0LL;
    v21 = v19 >= v20;
    if (v21) goto bb4;
    else goto bb5;
  bb4:
    v23 = 0LL;
    v24 = (int64_t)Vec_len((void*)v23, v1);
    memset(v25, 0, 8);
    *(int64_t*)v25 = (int64_t)v24;
    v27 = 0LL;
    memset(v28, 0, 8);
    *(int64_t*)v28 = (int64_t)v27;
    goto bb7;
  bb5:
    goto bb6;
  bb6:
    v48 = 0LL;
    Vec_drop((void*)v48, v1);
    return;
  bb7:
    p_co_tick();
    v32 = *(int64_t*)v28;
    v33 = *(int64_t*)v25;
    v34 = v32 < v33;
    if (v34) goto bb8;
    else goto bb9;
  bb8:
    v36 = *(int64_t*)v28;
    v37 = 0LL;
    v38 = (int64_t)Vec_get((void*)v37, v1, v36);
    v39 = 0LL;
    Str_drop((void*)v39, v38);
    v41 = *(int64_t*)v28;
    v42 = 1LL;
    v43 = (int64_t)((uint64_t)v41 + (uint64_t)v42);
    *(int64_t*)v28 = (int64_t)v43;
    goto bb7;
  bb9:
    goto bb6;
    return;
}

void Vec_drop_vecs(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    int64_t v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    int64_t v20 = 0;
    bool v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    int64_t v24 = 0;
    void* v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    int64_t v33 = 0;
    bool v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    v9 = alloca(8);
    v12 = alloca(8);
    v25 = alloca(8);
    v28 = alloca(8);
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    return;
  bb2:
    goto bb3;
  bb3:
    v7 = 16LL;
    v8 = p_add_i64(v1, v7);
    memset(v9, 0, 8);
    *(void**)v9 = (void*)v8;
    v11 = 0LL;
    memset(v12, 0, 8);
    *(int64_t*)v12 = (int64_t)v11;
    v15 = *(void**)v9;
    v16 = *(int64_t*)v15;
    *(int64_t*)v12 = (int64_t)v16;
    v19 = *(int64_t*)v12;
    v20 = 0LL;
    v21 = v19 >= v20;
    if (v21) goto bb4;
    else goto bb5;
  bb4:
    v23 = 0LL;
    v24 = (int64_t)Vec_len((void*)v23, v1);
    memset(v25, 0, 8);
    *(int64_t*)v25 = (int64_t)v24;
    v27 = 0LL;
    memset(v28, 0, 8);
    *(int64_t*)v28 = (int64_t)v27;
    goto bb7;
  bb5:
    goto bb6;
  bb6:
    v48 = 0LL;
    Vec_drop((void*)v48, v1);
    return;
  bb7:
    p_co_tick();
    v32 = *(int64_t*)v28;
    v33 = *(int64_t*)v25;
    v34 = v32 < v33;
    if (v34) goto bb8;
    else goto bb9;
  bb8:
    v36 = *(int64_t*)v28;
    v37 = 0LL;
    v38 = (int64_t)Vec_get((void*)v37, v1, v36);
    v39 = 0LL;
    Vec_drop((void*)v39, v38);
    v41 = *(int64_t*)v28;
    v42 = 1LL;
    v43 = (int64_t)((uint64_t)v41 + (uint64_t)v42);
    *(int64_t*)v28 = (int64_t)v43;
    goto bb7;
  bb9:
    goto bb6;
    return;
}

bool Vec_eq_strs(void* v0, int64_t v1, int64_t v2) {
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    bool v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    bool v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    bool v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    int64_t v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    bool v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    bool v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    void* v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0;
    bool v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0;
    int64_t v48 = 0;
    int64_t v49 = 0;
    bool v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    bool v53 = 0;
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0;
    int64_t v57 = 0;
    int64_t v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0; /* placeholder for void */
    bool v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    v8 = alloca(1);
    v24 = alloca(8);
    v35 = alloca(8);
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = true;
    return v5;
  bb2:
    goto bb3;
  bb3:
    memset(v8, 0, 1);
    v9 = 0LL;
    v10 = v1 == v9;
    *(bool*)v8 = v10;
    if (v10) goto bb5;
    else goto bb4;
  bb4:
    v13 = 0LL;
    v14 = v2 == v13;
    *(bool*)v8 = v14;
    goto bb5;
  bb5:
    v17 = *(bool*)v8;
    if (v17) goto bb6;
    else goto bb7;
  bb6:
    v19 = false;
    return v19;
  bb7:
    goto bb8;
  bb8:
    v22 = 0LL;
    v23 = (int64_t)Vec_len((void*)v22, v1);
    memset(v24, 0, 8);
    *(int64_t*)v24 = (int64_t)v23;
    v26 = *(int64_t*)v24;
    v27 = 0LL;
    v28 = (int64_t)Vec_len((void*)v27, v2);
    v29 = v26 != v28;
    if (v29) goto bb9;
    else goto bb10;
  bb9:
    v31 = false;
    return v31;
  bb10:
    goto bb11;
  bb11:
    v34 = 0LL;
    memset(v35, 0, 8);
    *(int64_t*)v35 = (int64_t)v34;
    goto bb12;
  bb12:
    p_co_tick();
    v39 = *(int64_t*)v35;
    v40 = *(int64_t*)v24;
    v41 = v39 < v40;
    if (v41) goto bb13;
    else goto bb14;
  bb13:
    v43 = *(int64_t*)v35;
    v44 = 0LL;
    v45 = (int64_t)Vec_get((void*)v44, v1, v43);
    v46 = *(int64_t*)v35;
    v47 = 0LL;
    v48 = (int64_t)Vec_get((void*)v47, v2, v46);
    v49 = 0LL;
    v50 = (bool)str_equal((void*)v49, (void*)v45, (void*)v48);
    v51 = !v50;
    if (v51) goto bb15;
    else goto bb16;
  bb14:
    v61 = true;
    return v61;
  bb15:
    v53 = false;
    return v53;
  bb16:
    goto bb17;
  bb17:
    v56 = *(int64_t*)v35;
    v57 = 1LL;
    v58 = (int64_t)((uint64_t)v56 + (uint64_t)v57);
    *(int64_t*)v35 = (int64_t)v58;
    goto bb12;
    return 0;
}

bool Vec_eq_words(void* v0, int64_t v1, int64_t v2) {
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    bool v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    bool v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    bool v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    int64_t v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    bool v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    bool v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    void* v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0;
    bool v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0;
    int64_t v48 = 0;
    bool v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    bool v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0; /* placeholder for void */
    bool v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    v8 = alloca(1);
    v24 = alloca(8);
    v35 = alloca(8);
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = true;
    return v5;
  bb2:
    goto bb3;
  bb3:
    memset(v8, 0, 1);
    v9 = 0LL;
    v10 = v1 == v9;
    *(bool*)v8 = v10;
    if (v10) goto bb5;
    else goto bb4;
  bb4:
    v13 = 0LL;
    v14 = v2 == v13;
    *(bool*)v8 = v14;
    goto bb5;
  bb5:
    v17 = *(bool*)v8;
    if (v17) goto bb6;
    else goto bb7;
  bb6:
    v19 = false;
    return v19;
  bb7:
    goto bb8;
  bb8:
    v22 = 0LL;
    v23 = (int64_t)Vec_len((void*)v22, v1);
    memset(v24, 0, 8);
    *(int64_t*)v24 = (int64_t)v23;
    v26 = *(int64_t*)v24;
    v27 = 0LL;
    v28 = (int64_t)Vec_len((void*)v27, v2);
    v29 = v26 != v28;
    if (v29) goto bb9;
    else goto bb10;
  bb9:
    v31 = false;
    return v31;
  bb10:
    goto bb11;
  bb11:
    v34 = 0LL;
    memset(v35, 0, 8);
    *(int64_t*)v35 = (int64_t)v34;
    goto bb12;
  bb12:
    p_co_tick();
    v39 = *(int64_t*)v35;
    v40 = *(int64_t*)v24;
    v41 = v39 < v40;
    if (v41) goto bb13;
    else goto bb14;
  bb13:
    v43 = *(int64_t*)v35;
    v44 = 0LL;
    v45 = (int64_t)Vec_get((void*)v44, v1, v43);
    v46 = *(int64_t*)v35;
    v47 = 0LL;
    v48 = (int64_t)Vec_get((void*)v47, v2, v46);
    v49 = v45 != v48;
    if (v49) goto bb15;
    else goto bb16;
  bb14:
    v59 = true;
    return v59;
  bb15:
    v51 = false;
    return v51;
  bb16:
    goto bb17;
  bb17:
    v54 = *(int64_t*)v35;
    v55 = 1LL;
    v56 = (int64_t)((uint64_t)v54 + (uint64_t)v55);
    *(int64_t*)v35 = (int64_t)v56;
    goto bb12;
    return 0;
}

void Vec_extend(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    vec_raw_extend((void*)v3, v1, v2);
    return;
    return;
}

int64_t Vec_first(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    int64_t v15 = 0;
    bool v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    void* v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0;
    void* v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0;
    void* v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0; /* placeholder for void */
    void* v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    v4 = alloca(8);
    v6 = alloca(8);
    v8 = alloca(8);
    v22 = alloca(8);
    v29 = alloca(8);
    v32 = alloca(8);
    v2 = 8LL;
    v3 = p_add_i64(v1, v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    memset(v6, 0, 8);
    *(void**)v6 = (void*)v1;
    memset(v8, 0, 8);
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v8 = (int64_t)v11;
    v14 = *(int64_t*)v8;
    v15 = 0LL;
    v16 = v14 == v15;
    if (v16) goto bb1;
    else goto bb2;
  bb1:
    v18 = _str7;
    p_panic((void*)v18);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    memset(v22, 0, 8);
    v24 = *(void**)v6;
    v25 = *(int64_t*)v24;
    *(int64_t*)v22 = (int64_t)v25;
    v28 = *(int64_t*)v22;
    memset(v29, 0, 8);
    *(void**)v29 = (void*)v28;
    v31 = 0LL;
    memset(v32, 0, 8);
    *(int64_t*)v32 = (int64_t)v31;
    v35 = *(void**)v29;
    v36 = *(int64_t*)v35;
    *(int64_t*)v32 = (int64_t)v36;
    v39 = *(int64_t*)v32;
    return v39;
    return 0;
}

int64_t Vec_get(void* v0, int64_t v1, int64_t v2) {
    int64_t* __h = (int64_t*)(intptr_t)v1;
    if (v2 >= __h[1]) p_panic("Vec index out of bounds");
    if (v2 < 0) p_panic("Vec index out of bounds");
    return ((int64_t*)(intptr_t)__h[0])[v2];
}

void Vec_insert(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    int64_t v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    v4 = 0LL;
    vec_raw_insert((void*)v4, v1, v2, v3);
    return;
    return;
}

bool Vec_is_empty(void* v0, int64_t v1) {
    int64_t* __h = (int64_t*)(intptr_t)v1;
    return __h[1] == 0;
}

int64_t Vec_last(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    int64_t v15 = 0;
    bool v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    void* v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0;
    int64_t v29 = 0;
    int64_t v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0;
    int64_t v34 = 0;
    void* v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    void* v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    void* v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    v4 = alloca(8);
    v6 = alloca(8);
    v8 = alloca(8);
    v22 = alloca(8);
    v35 = alloca(8);
    v38 = alloca(8);
    v41 = alloca(8);
    v2 = 8LL;
    v3 = p_add_i64(v1, v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    memset(v6, 0, 8);
    *(void**)v6 = (void*)v1;
    memset(v8, 0, 8);
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v8 = (int64_t)v11;
    v14 = *(int64_t*)v8;
    v15 = 0LL;
    v16 = v14 == v15;
    if (v16) goto bb1;
    else goto bb2;
  bb1:
    v18 = _str7;
    p_panic((void*)v18);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    memset(v22, 0, 8);
    v24 = *(void**)v6;
    v25 = *(int64_t*)v24;
    *(int64_t*)v22 = (int64_t)v25;
    v28 = *(int64_t*)v22;
    v29 = *(int64_t*)v8;
    v30 = 1LL;
    v31 = p_sub_i64(v29, v30);
    v32 = 8LL;
    v33 = p_mul_i64(v31, v32);
    v34 = p_add_i64(v28, v33);
    memset(v35, 0, 8);
    *(int64_t*)v35 = (int64_t)v34;
    v37 = *(int64_t*)v35;
    memset(v38, 0, 8);
    *(void**)v38 = (void*)v37;
    v40 = 0LL;
    memset(v41, 0, 8);
    *(int64_t*)v41 = (int64_t)v40;
    v44 = *(void**)v38;
    v45 = *(int64_t*)v44;
    *(int64_t*)v41 = (int64_t)v45;
    v48 = *(int64_t*)v41;
    return v48;
    return 0;
}

int64_t Vec_len(void* v0, int64_t v1) {
    int64_t* __h = (int64_t*)(intptr_t)v1;
    return __h[1];
}

int64_t Vec_new(void* v0) {
    int64_t* __h = (int64_t*)p_malloc(24);
    __h[0] = 0; __h[1] = 0; __h[2] = 0;
    return (int64_t)(intptr_t)__h;
}

int64_t Vec_pop(void* v0, int64_t v1) {
    int64_t* __h = (int64_t*)(intptr_t)v1;
    if (__h[1] == 0) return 0;
    __h[1] = __h[1] - 1;
    return ((int64_t*)(intptr_t)__h[0])[__h[1]];
}

void Vec_push(void* v0, int64_t v1, int64_t v2) {
    int64_t* __h = (int64_t*)(intptr_t)v1;
    int64_t __len = __h[1];
    if (P_UNLIKELY(__len >= __h[2])) __p_vec_grow(__h);
    ((int64_t*)(intptr_t)__h[0])[__len] = v2;
    __h[1] = __len + 1;
}

int64_t Vec_remove(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)vec_raw_remove((void*)v3, v1, v2);
    return v4;
    return 0;
}

void Vec_reverse(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    vec_raw_reverse((void*)v2, v1);
    return;
    return;
}

void Vec_set(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    int64_t* __h = (int64_t*)(intptr_t)v1;
    if (v2 >= __h[1]) p_panic("Vec index out of bounds");
    if (v2 < 0) p_panic("Vec index out of bounds");
    if (P_UNLIKELY(__h[2] < 0)) __p_vec_grow(__h);
    ((int64_t*)(intptr_t)__h[0])[v2] = v3;
}

int64_t Vec_share(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    int64_t v14 = 0;
    void* v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    void* v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    void* v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    void* v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    void* v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    void* v33 = 0;
    int64_t v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    int64_t v38 = 0;
    bool v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0;
    void* v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    int64_t v49 = 0;
    bool v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0; /* placeholder for void */
    void* v59 = 0;
    int64_t v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0; /* placeholder for void */
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0;
    void* v66 = 0;
    void* v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0;
    void* v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0;
    int64_t v73 = 0;
    int64_t v74 = 0;
    void* v75 = 0;
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0;
    int64_t v78 = 0;
    int64_t v79 = 0;
    void* v80 = 0;
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0; /* placeholder for void */
    void* v83 = 0;
    int64_t v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    void* v86 = 0;
    int64_t v87 = 0;
    int64_t v88 = 0; /* placeholder for void */
    void* v89 = 0;
    int64_t v90 = 0;
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0; /* placeholder for void */
    int64_t v93 = 0;
    int64_t v94 = 0; /* placeholder for void */
    v7 = alloca(8);
    v11 = alloca(8);
    v15 = alloca(8);
    v18 = alloca(8);
    v21 = alloca(8);
    v24 = alloca(8);
    v46 = alloca(8);
    v67 = alloca(8);
    v70 = alloca(8);
    v75 = alloca(8);
    v80 = alloca(8);
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    return v1;
  bb2:
    goto bb3;
  bb3:
    memset(v7, 0, 8);
    *(void**)v7 = (void*)v1;
    v9 = 8LL;
    v10 = p_add_i64(v1, v9);
    memset(v11, 0, 8);
    *(void**)v11 = (void*)v10;
    v13 = 16LL;
    v14 = p_add_i64(v1, v13);
    memset(v15, 0, 8);
    *(void**)v15 = (void*)v14;
    v17 = 0LL;
    memset(v18, 0, 8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = 0LL;
    memset(v21, 0, 8);
    *(int64_t*)v21 = (int64_t)v20;
    v23 = 0LL;
    memset(v24, 0, 8);
    *(int64_t*)v24 = (int64_t)v23;
    v27 = *(void**)v7;
    v28 = *(int64_t*)v27;
    *(int64_t*)v18 = (int64_t)v28;
    v30 = *(void**)v11;
    v31 = *(int64_t*)v30;
    *(int64_t*)v21 = (int64_t)v31;
    v33 = *(void**)v15;
    v34 = *(int64_t*)v33;
    *(int64_t*)v24 = (int64_t)v34;
    v37 = *(int64_t*)v18;
    v38 = 0LL;
    v39 = v37 == v38;
    if (v39) goto bb4;
    else goto bb5;
  bb4:
    v41 = 0LL;
    v42 = (int64_t)Vec_new((void*)v41);
    return v42;
  bb5:
    goto bb6;
  bb6:
    v45 = *(int64_t*)v24;
    memset(v46, 0, 8);
    *(int64_t*)v46 = (int64_t)v45;
    v48 = *(int64_t*)v24;
    v49 = 0LL;
    v50 = v48 >= v49;
    if (v50) goto bb7;
    else goto bb8;
  bb7:
    v52 = 0LL;
    v53 = *(int64_t*)v24;
    v54 = p_sub_i64(v52, v53);
    v55 = 1LL;
    v56 = p_sub_i64(v54, v55);
    *(int64_t*)v46 = (int64_t)v56;
    v59 = *(void**)v15;
    v60 = *(int64_t*)v46;
    *(int64_t*)v59 = (int64_t)v60;
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    v65 = 24LL;
    v66 = p_malloc(v65);
    memset(v67, 0, 8);
    *(int64_t*)v67 = (int64_t)v66;
    v69 = *(int64_t*)v67;
    memset(v70, 0, 8);
    *(void**)v70 = (void*)v69;
    v72 = *(int64_t*)v67;
    v73 = 8LL;
    v74 = p_add_i64(v72, v73);
    memset(v75, 0, 8);
    *(void**)v75 = (void*)v74;
    v77 = *(int64_t*)v67;
    v78 = 16LL;
    v79 = p_add_i64(v77, v78);
    memset(v80, 0, 8);
    *(void**)v80 = (void*)v79;
    v83 = *(void**)v70;
    v84 = *(int64_t*)v18;
    *(int64_t*)v83 = (int64_t)v84;
    v86 = *(void**)v75;
    v87 = *(int64_t*)v21;
    *(int64_t*)v86 = (int64_t)v87;
    v89 = *(void**)v80;
    v90 = *(int64_t*)v46;
    *(int64_t*)v89 = (int64_t)v90;
    v93 = *(int64_t*)v67;
    return v93;
    return 0;
}

int64_t Vec_share_deep(void* v0, int64_t v1, int64_t v2, int64_t v3, int64_t v4) {
    void* v5 = 0;
    int64_t v6 = 0;
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0;
    bool v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    int64_t v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    void* v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0;
    void* v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0; /* placeholder for void */
    void* v33 = 0;
    int64_t v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    void* v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0;
    int64_t v41 = 0;
    void* v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0;
    void* v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    int64_t v50 = 0;
    bool v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0;
    void* v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    void* v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    int64_t v63 = 0; /* placeholder for void */
    void* v64 = 0;
    int64_t v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    bool v69 = 0;
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0;
    int64_t v72 = 0;
    int64_t v73 = 0;
    int64_t v74 = 0;
    int64_t v75 = 0;
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0; /* placeholder for void */
    int64_t v78 = 0;
    int64_t v79 = 0;
    bool v80 = 0;
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0;
    int64_t v83 = 0;
    int64_t v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0; /* placeholder for void */
    int64_t v87 = 0;
    int64_t v88 = 0;
    void* v89 = 0;
    void* v90 = 0;
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0;
    void* v93 = 0;
    int64_t v94 = 0; /* placeholder for void */
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0;
    bool v98 = 0;
    int64_t v99 = 0; /* placeholder for void */
    int64_t v100 = 0;
    int64_t v101 = 0;
    int64_t v102 = 0;
    int64_t v103 = 0;
    int64_t v104 = 0;
    void* v105 = 0;
    int64_t v106 = 0; /* placeholder for void */
    int64_t v107 = 0;
    int64_t v108 = 0;
    int64_t v109 = 0;
    int64_t v110 = 0;
    int64_t v111 = 0;
    void* v112 = 0;
    int64_t v113 = 0; /* placeholder for void */
    int64_t v114 = 0;
    void* v115 = 0;
    int64_t v116 = 0; /* placeholder for void */
    int64_t v117 = 0; /* placeholder for void */
    void* v118 = 0;
    int64_t v119 = 0;
    int64_t v120 = 0; /* placeholder for void */
    int64_t v121 = 0; /* placeholder for void */
    void* v122 = 0;
    int64_t v123 = 0;
    int64_t v124 = 0;
    bool v125 = 0;
    int64_t v126 = 0; /* placeholder for void */
    int64_t v127 = 0; /* placeholder for void */
    int64_t v128 = 0;
    int64_t v129 = 0;
    int64_t v130 = 0;
    int64_t v131 = 0;
    int64_t v132 = 0;
    bool v133 = 0;
    int64_t v134 = 0; /* placeholder for void */
    int64_t v135 = 0; /* placeholder for void */
    bool v136 = 0;
    int64_t v137 = 0; /* placeholder for void */
    int64_t v138 = 0;
    int64_t v139 = 0;
    int64_t v140 = 0;
    int64_t v141 = 0; /* placeholder for void */
    int64_t v142 = 0; /* placeholder for void */
    int64_t v143 = 0; /* placeholder for void */
    int64_t v144 = 0; /* placeholder for void */
    void* v145 = 0;
    int64_t v146 = 0;
    int64_t v147 = 0; /* placeholder for void */
    int64_t v148 = 0; /* placeholder for void */
    int64_t v149 = 0;
    int64_t v150 = 0;
    int64_t v151 = 0;
    int64_t v152 = 0; /* placeholder for void */
    int64_t v153 = 0; /* placeholder for void */
    int64_t v154 = 0;
    int64_t v155 = 0;
    int64_t v156 = 0;
    int64_t v157 = 0; /* placeholder for void */
    int64_t v158 = 0; /* placeholder for void */
    int64_t v159 = 0; /* placeholder for void */
    int64_t v160 = 0;
    int64_t v161 = 0;
    int64_t v162 = 0;
    int64_t v163 = 0; /* placeholder for void */
    int64_t v164 = 0; /* placeholder for void */
    int64_t v165 = 0;
    int64_t v166 = 0; /* placeholder for void */
    v5 = alloca(1);
    v20 = alloca(8);
    v24 = alloca(8);
    v27 = alloca(8);
    v30 = alloca(8);
    v42 = alloca(8);
    v45 = alloca(8);
    v58 = alloca(8);
    v61 = alloca(8);
    v90 = alloca(8);
    v93 = alloca(8);
    v105 = alloca(8);
    v112 = alloca(8);
    v115 = alloca(8);
    v122 = alloca(1);
    memset(v5, 0, 1);
    v6 = 0LL;
    v7 = v2 == v6;
    *(bool*)v5 = v7;
    if (v7) goto bb2;
    else goto bb1;
  bb1:
    v10 = 0LL;
    v11 = v1 == v10;
    *(bool*)v5 = v11;
    goto bb2;
  bb2:
    v14 = *(bool*)v5;
    if (v14) goto bb3;
    else goto bb4;
  bb3:
    v16 = 0LL;
    v17 = (int64_t)Vec_share((void*)v16, v1);
    return v17;
  bb4:
    goto bb5;
  bb5:
    memset(v20, 0, 8);
    *(void**)v20 = (void*)v1;
    v22 = 8LL;
    v23 = p_add_i64(v1, v22);
    memset(v24, 0, 8);
    *(void**)v24 = (void*)v23;
    v26 = 0LL;
    memset(v27, 0, 8);
    *(int64_t*)v27 = (int64_t)v26;
    v29 = 0LL;
    memset(v30, 0, 8);
    *(int64_t*)v30 = (int64_t)v29;
    v33 = *(void**)v20;
    v34 = *(int64_t*)v33;
    *(int64_t*)v27 = (int64_t)v34;
    v36 = *(void**)v24;
    v37 = *(int64_t*)v36;
    *(int64_t*)v30 = (int64_t)v37;
    v40 = 0LL;
    v41 = (int64_t)Vec_new((void*)v40);
    memset(v42, 0, 8);
    *(int64_t*)v42 = (int64_t)v41;
    v44 = 0LL;
    memset(v45, 0, 8);
    *(int64_t*)v45 = (int64_t)v44;
    goto bb6;
  bb6:
    p_co_tick();
    v49 = *(int64_t*)v45;
    v50 = *(int64_t*)v30;
    v51 = v49 < v50;
    if (v51) goto bb7;
    else goto bb8;
  bb7:
    v53 = *(int64_t*)v27;
    v54 = *(int64_t*)v45;
    v55 = 8LL;
    v56 = p_mul_i64(v54, v55);
    v57 = p_add_i64(v53, v56);
    memset(v58, 0, 8);
    *(void**)v58 = (void*)v57;
    v60 = 0LL;
    memset(v61, 0, 8);
    *(int64_t*)v61 = (int64_t)v60;
    v64 = *(void**)v58;
    v65 = *(int64_t*)v64;
    *(int64_t*)v61 = (int64_t)v65;
    v68 = 1LL;
    v69 = v2 == v68;
    if (v69) goto bb9;
    else goto bb10;
  bb8:
    v165 = *(int64_t*)v42;
    return v165;
  bb9:
    v71 = *(int64_t*)v42;
    v72 = *(int64_t*)v61;
    v73 = 0LL;
    v74 = (int64_t)Vec_share((void*)v73, v72);
    v75 = 0LL;
    Vec_push((void*)v75, v71, v74);
    goto bb11;
  bb10:
    v78 = *(int64_t*)v61;
    v79 = 0LL;
    v80 = v78 == v79;
    if (v80) goto bb12;
    else goto bb13;
  bb11:
    v160 = *(int64_t*)v45;
    v161 = 1LL;
    v162 = (int64_t)((uint64_t)v160 + (uint64_t)v161);
    *(int64_t*)v45 = (int64_t)v162;
    goto bb6;
  bb12:
    v82 = *(int64_t*)v42;
    v83 = 0LL;
    v84 = 0LL;
    Vec_push((void*)v84, v82, v83);
    goto bb14;
  bb13:
    v87 = 8LL;
    v88 = p_mul_i64(v3, v87);
    v89 = p_malloc(v88);
    memset(v90, 0, 8);
    *(int64_t*)v90 = (int64_t)v89;
    v92 = 0LL;
    memset(v93, 0, 8);
    *(int64_t*)v93 = (int64_t)v92;
    goto bb15;
  bb14:
    goto bb11;
  bb15:
    p_co_tick();
    v97 = *(int64_t*)v93;
    v98 = v97 < v3;
    if (v98) goto bb16;
    else goto bb17;
  bb16:
    v100 = *(int64_t*)v61;
    v101 = *(int64_t*)v93;
    v102 = 8LL;
    v103 = p_mul_i64(v101, v102);
    v104 = p_add_i64(v100, v103);
    memset(v105, 0, 8);
    *(void**)v105 = (void*)v104;
    v107 = *(int64_t*)v90;
    v108 = *(int64_t*)v93;
    v109 = 8LL;
    v110 = p_mul_i64(v108, v109);
    v111 = p_add_i64(v107, v110);
    memset(v112, 0, 8);
    *(void**)v112 = (void*)v111;
    v114 = 0LL;
    memset(v115, 0, 8);
    *(int64_t*)v115 = (int64_t)v114;
    v118 = *(void**)v105;
    v119 = *(int64_t*)v118;
    *(int64_t*)v115 = (int64_t)v119;
    memset(v122, 0, 1);
    v123 = *(int64_t*)v93;
    v124 = 63LL;
    v125 = v123 < v124;
    *(bool*)v122 = v125;
    if (v125) goto bb18;
    else goto bb19;
  bb17:
    v154 = *(int64_t*)v42;
    v155 = *(int64_t*)v90;
    v156 = 0LL;
    Vec_push((void*)v156, v154, v155);
    goto bb14;
  bb18:
    v128 = *(int64_t*)v93;
    v129 = (int64_t)((int64_t)v4 >> (v128 & 63));
    v130 = 2LL;
    v131 = p_mod_i64(v129, v130);
    v132 = 1LL;
    v133 = v131 == v132;
    *(bool*)v122 = v133;
    goto bb19;
  bb19:
    v136 = *(bool*)v122;
    if (v136) goto bb20;
    else goto bb21;
  bb20:
    v138 = *(int64_t*)v115;
    v139 = 0LL;
    v140 = (int64_t)Vec_share((void*)v139, v138);
    *(int64_t*)v115 = (int64_t)v140;
    goto bb22;
  bb21:
    goto bb22;
  bb22:
    v145 = *(void**)v112;
    v146 = *(int64_t*)v115;
    *(int64_t*)v145 = (int64_t)v146;
    v149 = *(int64_t*)v93;
    v150 = 1LL;
    v151 = (int64_t)((uint64_t)v149 + (uint64_t)v150);
    *(int64_t*)v93 = (int64_t)v151;
    goto bb15;
    return 0;
}

void Vec_sort(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    vec_raw_sort((void*)v2, v1);
    return;
    return;
}

void* Vec_to_str(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    void* v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    void* v20 = 0;
    void* v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0;
    int64_t v29 = 0;
    bool v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    int64_t v33 = 0;
    bool v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    void* v36 = 0;
    void* v37 = 0;
    void* v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    void* v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    void* v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    void* v53 = 0;
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0; /* placeholder for void */
    void* v56 = 0;
    int64_t v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
    int64_t v59 = 0; /* placeholder for void */
    void* v60 = 0;
    int64_t v61 = 0;
    void* v62 = 0;
    void* v63 = 0;
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0;
    int64_t v66 = 0;
    int64_t v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0; /* placeholder for void */
    void* v70 = 0;
    void* v71 = 0;
    void* v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    void* v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    v4 = alloca(8);
    v6 = alloca(8);
    v8 = alloca(8);
    v14 = alloca(8);
    v21 = alloca(8);
    v24 = alloca(8);
    v47 = alloca(8);
    v50 = alloca(8);
    v53 = alloca(8);
    v2 = 8LL;
    v3 = p_add_i64(v1, v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    memset(v6, 0, 8);
    *(void**)v6 = (void*)v1;
    memset(v8, 0, 8);
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v8 = (int64_t)v11;
    memset(v14, 0, 8);
    v16 = *(void**)v6;
    v17 = *(int64_t*)v16;
    *(int64_t*)v14 = (int64_t)v17;
    v20 = _str9;
    memset(v21, 0, 8);
    *(void**)v21 = (void*)v20;
    v23 = 0LL;
    memset(v24, 0, 8);
    *(int64_t*)v24 = (int64_t)v23;
    goto bb1;
  bb1:
    p_co_tick();
    v28 = *(int64_t*)v24;
    v29 = *(int64_t*)v8;
    v30 = v28 < v29;
    if (v30) goto bb2;
    else goto bb3;
  bb2:
    v32 = *(int64_t*)v24;
    v33 = 0LL;
    v34 = v32 > v33;
    if (v34) goto bb4;
    else goto bb5;
  bb3:
    v70 = *(void**)v21;
    v71 = _str10;
    v72 = p_str_concat((void*)v70, (void*)v71);
    *(void**)v21 = (void*)v72;
    v74 = *(void**)v21;
    return v74;
  bb4:
    v36 = *(void**)v21;
    v37 = _str3;
    v38 = p_str_concat((void*)v36, (void*)v37);
    *(void**)v21 = (void*)v38;
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v42 = *(int64_t*)v14;
    v43 = *(int64_t*)v24;
    v44 = 8LL;
    v45 = p_mul_i64(v43, v44);
    v46 = p_add_i64(v42, v45);
    memset(v47, 0, 8);
    *(int64_t*)v47 = (int64_t)v46;
    v49 = *(int64_t*)v47;
    memset(v50, 0, 8);
    *(void**)v50 = (void*)v49;
    v52 = 0LL;
    memset(v53, 0, 8);
    *(int64_t*)v53 = (int64_t)v52;
    v56 = *(void**)v50;
    v57 = *(int64_t*)v56;
    *(int64_t*)v53 = (int64_t)v57;
    v60 = *(void**)v21;
    v61 = *(int64_t*)v53;
    v62 = (void*)p_to_str((int64_t)v61);
    v63 = p_str_concat((void*)v60, (void*)v62);
    *(void**)v21 = (void*)v63;
    v65 = *(int64_t*)v24;
    v66 = 1LL;
    v67 = (int64_t)((uint64_t)v65 + (uint64_t)v66);
    *(int64_t*)v24 = (int64_t)v67;
    goto bb1;
    return 0;
}

void Vec_truncate(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    v5 = alloca(8);
    v7 = alloca(8);
    v3 = 8LL;
    v4 = p_add_i64(v1, v3);
    memset(v5, 0, 8);
    *(void**)v5 = (void*)v4;
    memset(v7, 0, 8);
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
    return;
    return;
}

bool __eq_ByteBuffer(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    void* v4 = 0;
    void* v5 = 0;
    void* v6 = 0;
    void* v7 = 0;
    bool v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0;
    bool v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    bool v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    v3 = alloca(1);
    memset(v3, 0, 1);
    v4 = (char*)v1 + 0;
    v5 = *(void**)v4;
    v6 = (char*)v2 + 0;
    v7 = *(void**)v6;
    v8 = (int64_t)v5 == (int64_t)v7;
    *(bool*)v3 = v8;
    if (v8) goto bb1;
    else goto bb2;
  bb1:
    v11 = (char*)v1 + 8;
    v12 = *(int64_t*)v11;
    v13 = (char*)v2 + 8;
    v14 = *(int64_t*)v13;
    v15 = v12 == v14;
    *(bool*)v3 = v15;
    goto bb2;
  bb2:
    v18 = *(bool*)v3;
    return v18;
    return 0;
}

bool __eq_Expr(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0;
    bool v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0;
    bool v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    bool v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0; /* placeholder for void */
    void* v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0;
    bool v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    void* v29 = 0;
    bool v30 = 0;
    void* v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0;
    bool v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    void* v36 = 0;
    bool v37 = 0;
    bool v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0; /* placeholder for void */
    bool v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0; /* placeholder for void */
    void* v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0;
    bool v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    void* v50 = 0;
    void* v51 = 0;
    void* v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0;
    bool v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
    void* v57 = 0;
    void* v58 = 0;
    bool v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0; /* placeholder for void */
    bool v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0; /* placeholder for void */
    bool v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    v67 = false;
    return v67;
  bb2:
    v8 = (char*)v1 + 8;
    v9 = *(int64_t*)v8;
    v10 = (char*)v2 + 0;
    v11 = *(int64_t*)v10;
    v12 = 0LL;
    v13 = v11 == v12;
    if (v13) goto bb5;
    else goto bb6;
  bb3:
    v24 = (char*)v1 + 0;
    v25 = *(int64_t*)v24;
    v26 = 1LL;
    v27 = v25 == v26;
    if (v27) goto bb9;
    else goto bb10;
  bb4:
    goto bb1;
  bb5:
    v15 = (char*)v2 + 8;
    v16 = *(int64_t*)v15;
    v17 = v9 == v16;
    return v17;
  bb6:
    goto bb7;
  bb7:
    v20 = false;
    return v20;
  bb8:
    goto bb4;
  bb9:
    v29 = (char*)v1 + 8;
    v30 = *(bool*)v29;
    v31 = (char*)v2 + 0;
    v32 = *(int64_t*)v31;
    v33 = 1LL;
    v34 = v32 == v33;
    if (v34) goto bb12;
    else goto bb13;
  bb10:
    v45 = (char*)v1 + 0;
    v46 = *(int64_t*)v45;
    v47 = 2LL;
    v48 = v46 == v47;
    if (v48) goto bb16;
    else goto bb17;
  bb11:
    goto bb1;
  bb12:
    v36 = (char*)v2 + 8;
    v37 = *(bool*)v36;
    v38 = v30 == v37;
    return v38;
  bb13:
    goto bb14;
  bb14:
    v41 = false;
    return v41;
  bb15:
    goto bb11;
  bb16:
    v50 = (char*)v1 + 8;
    v51 = *(void**)v50;
    v52 = (char*)v2 + 0;
    v53 = *(int64_t*)v52;
    v54 = 2LL;
    v55 = v53 == v54;
    if (v55) goto bb19;
    else goto bb20;
  bb17:
    goto bb1;
  bb18:
    goto bb1;
  bb19:
    v57 = (char*)v2 + 8;
    v58 = *(void**)v57;
    v59 = (int64_t)v51 == (int64_t)v58;
    return v59;
  bb20:
    goto bb21;
  bb21:
    v62 = false;
    return v62;
  bb22:
    goto bb18;
    return 0;
}

bool __eq_IfExpr(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    void* v4 = 0;
    void* v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0;
    bool v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    void* v12 = 0;
    void* v13 = 0;
    void* v14 = 0;
    bool v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    bool v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    void* v21 = 0;
    void* v22 = 0;
    void* v23 = 0;
    void* v24 = 0;
    bool v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    bool v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    v3 = alloca(1);
    v4 = alloca(1);
    memset(v3, 0, 1);
    memset(v4, 0, 1);
    v5 = (char*)v1 + 0;
    v6 = (char*)v2 + 0;
    v7 = 0LL;
    v8 = (bool)__eq_Expr((void*)v7, (void*)v5, (void*)v6);
    *(bool*)v4 = v8;
    if (v8) goto bb1;
    else goto bb2;
  bb1:
    v11 = (char*)v1 + 16;
    v12 = *(void**)v11;
    v13 = (char*)v2 + 16;
    v14 = *(void**)v13;
    v15 = (int64_t)v12 == (int64_t)v14;
    *(bool*)v4 = v15;
    goto bb2;
  bb2:
    v18 = *(bool*)v4;
    *(bool*)v3 = v18;
    if (v18) goto bb3;
    else goto bb4;
  bb3:
    v21 = (char*)v1 + 24;
    v22 = *(void**)v21;
    v23 = (char*)v2 + 24;
    v24 = *(void**)v23;
    v25 = (int64_t)v22 == (int64_t)v24;
    *(bool*)v3 = v25;
    goto bb4;
  bb4:
    v28 = *(bool*)v3;
    return v28;
    return 0;
}

bool __eq_ParseResultIfExpr(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    void* v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0;
    void* v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    void* v18 = 0;
    int64_t v19 = 0;
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0;
    bool v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    void* v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    void* v30 = 0;
    void* v31 = 0;
    void* v32 = 0;
    int64_t v33 = 0;
    void* v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    void* v36 = 0;
    void* v37 = 0;
    int64_t v38 = 0;
    void* v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    bool v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0; /* placeholder for void */
    bool v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    void* v49 = 0;
    int64_t v50 = 0;
    int64_t v51 = 0;
    bool v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    void* v54 = 0;
    void* v55 = 0;
    void* v56 = 0;
    int64_t v57 = 0;
    int64_t v58 = 0;
    bool v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    void* v61 = 0;
    void* v62 = 0;
    int64_t v63 = 0;
    bool v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0; /* placeholder for void */
    bool v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0; /* placeholder for void */
    bool v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    v9 = alloca(32);
    v28 = alloca(32);
    v3 = (char*)v1 + 0;
    v4 = *(int64_t*)v3;
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb2;
    else goto bb3;
  bb1:
    v72 = false;
    return v72;
  bb2:
    v8 = (char*)v1 + 8;
    memset(v9, 0, 32);
    memcpy(v9, v8, 32);
    v11 = (char*)v9 + 0;
    v12 = (char*)v9 + 16;
    v13 = *(void**)v12;
    v14 = 0LL;
    v15 = (void*)Vec_share((void*)v14, (int64_t)v13);
    *(void**)v12 = (void*)v15;
    v17 = (char*)v9 + 24;
    v18 = *(void**)v17;
    v19 = 0LL;
    v20 = (void*)Vec_share((void*)v19, (int64_t)v18);
    *(void**)v17 = (void*)v20;
    v22 = (char*)v2 + 0;
    v23 = *(int64_t*)v22;
    v24 = 0LL;
    v25 = v23 == v24;
    if (v25) goto bb5;
    else goto bb6;
  bb3:
    v49 = (char*)v1 + 0;
    v50 = *(int64_t*)v49;
    v51 = 1LL;
    v52 = v50 == v51;
    if (v52) goto bb9;
    else goto bb10;
  bb4:
    goto bb1;
  bb5:
    v27 = (char*)v2 + 8;
    memset(v28, 0, 32);
    memcpy(v28, v27, 32);
    v30 = (char*)v28 + 0;
    v31 = (char*)v28 + 16;
    v32 = *(void**)v31;
    v33 = 0LL;
    v34 = (void*)Vec_share((void*)v33, (int64_t)v32);
    *(void**)v31 = (void*)v34;
    v36 = (char*)v28 + 24;
    v37 = *(void**)v36;
    v38 = 0LL;
    v39 = (void*)Vec_share((void*)v38, (int64_t)v37);
    *(void**)v36 = (void*)v39;
    v41 = 0LL;
    v42 = (bool)__eq_IfExpr((void*)v41, (void*)v9, (void*)v28);
    return v42;
  bb6:
    goto bb7;
  bb7:
    v45 = false;
    return v45;
  bb8:
    goto bb4;
  bb9:
    v54 = (char*)v1 + 8;
    v55 = *(void**)v54;
    v56 = (char*)v2 + 0;
    v57 = *(int64_t*)v56;
    v58 = 1LL;
    v59 = v57 == v58;
    if (v59) goto bb12;
    else goto bb13;
  bb10:
    goto bb1;
  bb11:
    goto bb1;
  bb12:
    v61 = (char*)v2 + 8;
    v62 = *(void**)v61;
    v63 = 0LL;
    v64 = (bool)p_str_equal((void*)v63, (int64_t)v55, (int64_t)v62);
    return v64;
  bb13:
    goto bb14;
  bb14:
    v67 = false;
    return v67;
  bb15:
    goto bb11;
    return 0;
}

bool __eq_StringBuilder(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0;
    bool v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0;
    bool v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    bool v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    v3 = alloca(1);
    memset(v3, 0, 1);
    v4 = (char*)v1 + 0;
    v5 = *(int64_t*)v4;
    v6 = (char*)v2 + 0;
    v7 = *(int64_t*)v6;
    v8 = v5 == v7;
    *(bool*)v3 = v8;
    if (v8) goto bb1;
    else goto bb2;
  bb1:
    v11 = (char*)v1 + 8;
    v12 = *(int64_t*)v11;
    v13 = (char*)v2 + 8;
    v14 = *(int64_t*)v13;
    v15 = v12 == v14;
    *(bool*)v3 = v15;
    goto bb2;
  bb2:
    v18 = *(bool*)v3;
    return v18;
    return 0;
}

int64_t p_abs(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    int64_t v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = v1 < v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 0LL;
    v6 = p_sub_i64(v5, v1);
    return v6;
  bb2:
    goto bb3;
  bb3:
    return v1;
    return 0;
}

int64_t allocator_alloc(void* v0, void* v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v3 = (int64_t)p_allocator_alloc(v1, v2);
    return v3;
    return 0;
}

int64_t allocator_bytes_used(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = (int64_t)p_allocator_stats(v1);
    return v2;
    return 0;
}

void allocator_free(void* v0, void* v1, int64_t v2) {
    int64_t v3 = 0; /* placeholder for void */
    int64_t v4 = 0; /* placeholder for void */
    p_allocator_free(v1, v2);
    return;
    return;
}

void assert(void* v0, bool v1) {
    int64_t v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0; /* placeholder for void */
    v2 = !v1;
    if (v2) goto bb1;
    else goto bb2;
  bb1:
    v4 = _str11;
    p_panic((void*)v4);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    return;
    return;
}

void assert_msg(void* v0, bool v1, void* v2) {
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0; /* placeholder for void */
    v3 = !v1;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    p_panic((void*)v2);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    return;
    return;
}

void* byte_to_str(void* v0, int64_t v1) {
    void* v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = p_char_to_str(v1);
    return v2;
    return 0;
}

bool char_is_alpha(void* v0, int64_t v1) {
    void* v2 = 0;
    void* v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0; /* placeholder for void */
    bool v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0;
    bool v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    bool v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0; /* placeholder for void */
    bool v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    bool v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    v2 = alloca(1);
    v3 = alloca(1);
    v15 = alloca(1);
    memset(v2, 0, 1);
    memset(v3, 0, 1);
    v4 = 65LL;
    v5 = v1 >= v4;
    *(bool*)v3 = v5;
    if (v5) goto bb1;
    else goto bb2;
  bb1:
    v8 = 90LL;
    v9 = v1 <= v8;
    *(bool*)v3 = v9;
    goto bb2;
  bb2:
    v12 = *(bool*)v3;
    *(bool*)v2 = v12;
    if (v12) goto bb4;
    else goto bb3;
  bb3:
    memset(v15, 0, 1);
    v16 = 97LL;
    v17 = v1 >= v16;
    *(bool*)v15 = v17;
    if (v17) goto bb5;
    else goto bb6;
  bb4:
    v27 = *(bool*)v2;
    return v27;
  bb5:
    v20 = 122LL;
    v21 = v1 <= v20;
    *(bool*)v15 = v21;
    goto bb6;
  bb6:
    v24 = *(bool*)v15;
    *(bool*)v2 = v24;
    goto bb4;
    return 0;
}

bool char_is_digit(void* v0, int64_t v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    bool v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    bool v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    v2 = alloca(1);
    memset(v2, 0, 1);
    v3 = 48LL;
    v4 = v1 >= v3;
    *(bool*)v2 = v4;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    v7 = 57LL;
    v8 = v1 <= v7;
    *(bool*)v2 = v8;
    goto bb2;
  bb2:
    v11 = *(bool*)v2;
    return v11;
    return 0;
}

bool char_is_lower(void* v0, int64_t v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    bool v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    bool v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    v2 = alloca(1);
    memset(v2, 0, 1);
    v3 = 97LL;
    v4 = v1 >= v3;
    *(bool*)v2 = v4;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    v7 = 122LL;
    v8 = v1 <= v7;
    *(bool*)v2 = v8;
    goto bb2;
  bb2:
    v11 = *(bool*)v2;
    return v11;
    return 0;
}

bool char_is_upper(void* v0, int64_t v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    bool v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    bool v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    v2 = alloca(1);
    memset(v2, 0, 1);
    v3 = 65LL;
    v4 = v1 >= v3;
    *(bool*)v2 = v4;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    v7 = 90LL;
    v8 = v1 <= v7;
    *(bool*)v2 = v8;
    goto bb2;
  bb2:
    v11 = *(bool*)v2;
    return v11;
    return 0;
}

bool char_is_whitespace(void* v0, int64_t v1) {
    void* v2 = 0;
    void* v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    bool v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    bool v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    bool v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    bool v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    bool v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    bool v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    v2 = alloca(1);
    v3 = alloca(1);
    v4 = alloca(1);
    memset(v2, 0, 1);
    memset(v3, 0, 1);
    memset(v4, 0, 1);
    v5 = 32LL;
    v6 = v1 == v5;
    *(bool*)v4 = v6;
    if (v6) goto bb2;
    else goto bb1;
  bb1:
    v9 = 9LL;
    v10 = v1 == v9;
    *(bool*)v4 = v10;
    goto bb2;
  bb2:
    v13 = *(bool*)v4;
    *(bool*)v3 = v13;
    if (v13) goto bb4;
    else goto bb3;
  bb3:
    v16 = 10LL;
    v17 = v1 == v16;
    *(bool*)v3 = v17;
    goto bb4;
  bb4:
    v20 = *(bool*)v3;
    *(bool*)v2 = v20;
    if (v20) goto bb6;
    else goto bb5;
  bb5:
    v23 = 13LL;
    v24 = v1 == v23;
    *(bool*)v2 = v24;
    goto bb6;
  bb6:
    v27 = *(bool*)v2;
    return v27;
    return 0;
}

int64_t char_to_lower(void* v0, int64_t v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    bool v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    bool v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    v2 = alloca(1);
    memset(v2, 0, 1);
    v3 = 65LL;
    v4 = v1 >= v3;
    *(bool*)v2 = v4;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    v7 = 90LL;
    v8 = v1 <= v7;
    *(bool*)v2 = v8;
    goto bb2;
  bb2:
    v11 = *(bool*)v2;
    if (v11) goto bb3;
    else goto bb4;
  bb3:
    v13 = 32LL;
    v14 = p_add_i64(v1, v13);
    return v14;
  bb4:
    goto bb5;
  bb5:
    return v1;
    return 0;
}

void* char_to_str(void* v0, int64_t v1) {
    void* v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = p_char_to_str(v1);
    return v2;
    return 0;
}

int64_t char_to_upper(void* v0, int64_t v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    bool v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    bool v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    v2 = alloca(1);
    memset(v2, 0, 1);
    v3 = 97LL;
    v4 = v1 >= v3;
    *(bool*)v2 = v4;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    v7 = 122LL;
    v8 = v1 <= v7;
    *(bool*)v2 = v8;
    goto bb2;
  bb2:
    v11 = *(bool*)v2;
    if (v11) goto bb3;
    else goto bb4;
  bb3:
    v13 = 32LL;
    v14 = p_sub_i64(v1, v13);
    return v14;
  bb4:
    goto bb5;
  bb5:
    return v1;
    return 0;
}

int64_t clamp(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    bool v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
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

void* create_allocator(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = p_allocator_create(v1);
    return v2;
    return 0;
}

void* float_to_str(void* v0, double v1) {
    void* v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = p_float_to_str(v1);
    return v2;
    return 0;
}

bool hm_eq(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0;
    bool v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    v4 = 0LL;
    v5 = (bool)hm_is_int((void*)v4, v1);
    if (v5) goto bb1;
    else goto bb2;
  bb1:
    v7 = v2 == v3;
    return v7;
  bb2:
    goto bb3;
  bb3:
    v10 = 0LL;
    v11 = (bool)p_str_equal((void*)v10, v2, v3);
    return v11;
    return 0;
}

int64_t hm_hash(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    int64_t v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (bool)hm_is_int((void*)v3, v1);
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    v6 = 0LL;
    v7 = (int64_t)hm_int_hash((void*)v6, v2);
    return v7;
  bb2:
    goto bb3;
  bb3:
    v10 = 0LL;
    v11 = (int64_t)p_str_hash((void*)v10, v2);
    return v11;
    return 0;
}

int64_t hm_int_hash(void* v0, int64_t v1) {
    void* v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    int64_t v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    int64_t v7 = 0;
    int64_t v8 = 0;
    int64_t v9 = 0;
    int64_t v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0;
    int64_t v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    int64_t v21 = 0;
    int64_t v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    v2 = alloca(8);
    memset(v2, 0, 8);
    *(int64_t*)v2 = (int64_t)v1;
    v4 = *(int64_t*)v2;
    v5 = *(int64_t*)v2;
    v6 = 31LL;
    v7 = (int64_t)((int64_t)v5 >> (v6 & 63));
    v8 = v4 ^ v7;
    v9 = 9202493588570546565LL;
    v10 = (int64_t)((uint64_t)v8 * (uint64_t)v9);
    *(int64_t*)v2 = (int64_t)v10;
    v12 = *(int64_t*)v2;
    v13 = *(int64_t*)v2;
    v14 = 27LL;
    v15 = (int64_t)((int64_t)v13 >> (v14 & 63));
    v16 = v12 ^ v15;
    v17 = 1946526487930394057LL;
    v18 = (int64_t)((uint64_t)v16 * (uint64_t)v17);
    *(int64_t*)v2 = (int64_t)v18;
    v20 = *(int64_t*)v2;
    v21 = *(int64_t*)v2;
    v22 = 33LL;
    v23 = (int64_t)((int64_t)v21 >> (v22 & 63));
    v24 = v20 ^ v23;
    *(int64_t*)v2 = (int64_t)v24;
    v26 = *(int64_t*)v2;
    v27 = 4611686018427387903LL;
    v28 = v26 & v27;
    return v28;
    return 0;
}

bool hm_is_int(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    int64_t v15 = 0;
    bool v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    v4 = alloca(8);
    v7 = alloca(8);
    v2 = 40LL;
    v3 = p_add_i64(v1, v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    v6 = 0LL;
    memset(v7, 0, 8);
    *(int64_t*)v7 = (int64_t)v6;
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v7 = (int64_t)v11;
    v14 = *(int64_t*)v7;
    v15 = 0LL;
    v16 = v14 != v15;
    return v16;
    return 0;
}

int64_t hm_store_i(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0;
    int64_t v13 = 0;
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    void* v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0;
    bool v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    int64_t v23 = 0;
    bool v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    bool v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    void* v29 = 0;
    int64_t v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    v5 = alloca(8);
    v14 = alloca(8);
    v16 = alloca(1);
    v3 = 40LL;
    v4 = p_add_i64(v1, v3);
    memset(v5, 0, 8);
    *(void**)v5 = (void*)v4;
    v8 = *(void**)v5;
    v9 = 1LL;
    *(int64_t*)v8 = (int64_t)v9;
    v12 = 6768574230975169895LL;
    v13 = v2 ^ v12;
    memset(v14, 0, 8);
    *(int64_t*)v14 = (int64_t)v13;
    memset(v16, 0, 1);
    v17 = *(int64_t*)v14;
    v18 = 0LL;
    v19 = v17 == v18;
    *(bool*)v16 = v19;
    if (v19) goto bb2;
    else goto bb1;
  bb1:
    v22 = *(int64_t*)v14;
    v23 = -1LL;
    v24 = v22 == v23;
    *(bool*)v16 = v24;
    goto bb2;
  bb2:
    v27 = *(bool*)v16;
    if (v27) goto bb3;
    else goto bb4;
  bb3:
    v29 = _str12;
    v30 = 0LL;
    panic((void*)v30, (void*)v29);
    goto bb5;
  bb4:
    goto bb5;
  bb5:
    v34 = *(int64_t*)v14;
    return v34;
    return 0;
}

void* int_to_str(void* v0, int64_t v1) {
    void* v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = (void*)p_to_str((int64_t)v1);
    return v2;
    return 0;
}

int64_t main_entry(void* v0) {
    void* v1 = 0;
    void* v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    bool v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    void* v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    void* v14 = 0;
    void* v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0;
    void* v19 = 0;
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    void* v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    void* v28 = 0;
    void* v29 = 0;
    void* v30 = 0;
    int64_t v31 = 0;
    void* v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    void* v34 = 0;
    void* v35 = 0;
    int64_t v36 = 0;
    void* v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    void* v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    void* v41 = 0;
    void* v42 = 0;
    void* v43 = 0;
    int64_t v44 = 0;
    void* v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    void* v47 = 0;
    void* v48 = 0;
    int64_t v49 = 0;
    void* v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    void* v52 = 0;
    void* v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
    void* v57 = 0;
    int64_t v58 = 0;
    void* v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    void* v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    void* v63 = 0;
    void* v64 = 0;
    void* v65 = 0;
    int64_t v66 = 0;
    void* v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    void* v69 = 0;
    void* v70 = 0;
    int64_t v71 = 0;
    void* v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    void* v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    void* v76 = 0;
    int64_t v77 = 0;
    int64_t v78 = 0;
    bool v79 = 0;
    int64_t v80 = 0; /* placeholder for void */
    void* v81 = 0;
    void* v82 = 0;
    int64_t v83 = 0; /* placeholder for void */
    void* v84 = 0;
    void* v85 = 0;
    void* v86 = 0;
    int64_t v87 = 0;
    void* v88 = 0;
    int64_t v89 = 0; /* placeholder for void */
    void* v90 = 0;
    void* v91 = 0;
    int64_t v92 = 0;
    void* v93 = 0;
    int64_t v94 = 0; /* placeholder for void */
    void* v95 = 0;
    void* v96 = 0;
    int64_t v97 = 0;
    int64_t v98 = 0;
    int64_t v99 = 0; /* placeholder for void */
    void* v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0; /* placeholder for void */
    void* v103 = 0;
    int64_t v104 = 0;
    int64_t v105 = 0;
    bool v106 = 0;
    int64_t v107 = 0; /* placeholder for void */
    void* v108 = 0;
    void* v109 = 0;
    int64_t v110 = 0; /* placeholder for void */
    void* v111 = 0;
    int64_t v112 = 0; /* placeholder for void */
    int64_t v113 = 0; /* placeholder for void */
    int64_t v114 = 0; /* placeholder for void */
    int64_t v115 = 0; /* placeholder for void */
    v1 = alloca(32);
    v2 = alloca(16);
    v26 = alloca(32);
    v39 = alloca(32);
    v57 = alloca(40);
    v74 = alloca(40);
    v82 = alloca(32);
    memset(v1, 0, 32);
    memset(v2, 0, 16);
    v3 = 1LL;
    v4 = (char*)v2 + 0;
    *(int64_t*)v4 = (int64_t)v3;
    v6 = true;
    v7 = (char*)v2 + 8;
    *(bool*)v7 = v6;
    v9 = (char*)v1 + 0;
    v10 = (char*)v2 + 0;
    v11 = (char*)v9 + 0;
    v12 = *(int64_t*)v10;
    *(int64_t*)v11 = (int64_t)v12;
    v14 = (char*)v2 + 8;
    v15 = (char*)v9 + 8;
    v16 = *(int64_t*)v14;
    *(int64_t*)v15 = (int64_t)v16;
    v18 = 0LL;
    v19 = (void*)Vec_new((void*)v18);
    v20 = (char*)v1 + 16;
    *(void**)v20 = (void*)v19;
    v22 = 0LL;
    v23 = (void*)Vec_new((void*)v22);
    v24 = (char*)v1 + 24;
    *(void**)v24 = (void*)v23;
    memset(v26, 0, 32);
    memcpy(v26, v1, 32);
    v28 = (char*)v26 + 0;
    v29 = (char*)v26 + 16;
    v30 = *(void**)v29;
    v31 = 0LL;
    v32 = (void*)Vec_share((void*)v31, (int64_t)v30);
    *(void**)v29 = (void*)v32;
    v34 = (char*)v26 + 24;
    v35 = *(void**)v34;
    v36 = 0LL;
    v37 = (void*)Vec_share((void*)v36, (int64_t)v35);
    *(void**)v34 = (void*)v37;
    memset(v39, 0, 32);
    memcpy(v39, v26, 32);
    v41 = (char*)v39 + 0;
    v42 = (char*)v39 + 16;
    v43 = *(void**)v42;
    v44 = 0LL;
    v45 = (void*)Vec_share((void*)v44, (int64_t)v43);
    *(void**)v42 = (void*)v45;
    v47 = (char*)v39 + 24;
    v48 = *(void**)v47;
    v49 = 0LL;
    v50 = (void*)Vec_share((void*)v49, (int64_t)v48);
    *(void**)v47 = (void*)v50;
    v52 = (char*)v39 + 16;
    v53 = *(void**)v52;
    v54 = 1LL;
    v55 = 0LL;
    Vec_push((void*)v55, (int64_t)v53, v54);
    memset(v57, 0, 40);
    v58 = 0LL;
    v59 = (char*)v57 + 0;
    *(int64_t*)v59 = (int64_t)v58;
    v61 = (char*)v57 + 8;
    memcpy(v61, v39, 32);
    v63 = (char*)v61 + 0;
    v64 = (char*)v61 + 8;
    v65 = *(void**)v64;
    v66 = 0LL;
    v67 = (void*)Vec_share((void*)v66, (int64_t)v65);
    *(void**)v64 = (void*)v67;
    v69 = (char*)v61 + 16;
    v70 = *(void**)v69;
    v71 = 0LL;
    v72 = (void*)Vec_share((void*)v71, (int64_t)v70);
    *(void**)v69 = (void*)v72;
    memset(v74, 0, 40);
    memcpy(v74, v57, 40);
    v76 = (char*)v74 + 0;
    v77 = *(int64_t*)v76;
    v78 = 0LL;
    v79 = v77 == v78;
    if (v79) goto bb2;
    else goto bb3;
  bb1:
    return 0;
  bb2:
    v81 = (char*)v74 + 8;
    memset(v82, 0, 32);
    memcpy(v82, v81, 32);
    v84 = (char*)v82 + 0;
    v85 = (char*)v82 + 16;
    v86 = *(void**)v85;
    v87 = 0LL;
    v88 = (void*)Vec_share((void*)v87, (int64_t)v86);
    *(void**)v85 = (void*)v88;
    v90 = (char*)v82 + 24;
    v91 = *(void**)v90;
    v92 = 0LL;
    v93 = (void*)Vec_share((void*)v92, (int64_t)v91);
    *(void**)v90 = (void*)v93;
    v95 = (char*)v82 + 16;
    v96 = *(void**)v95;
    v97 = 0LL;
    v98 = (int64_t)Vec_len((void*)v97, (int64_t)v96);
    p_print_int((int64_t)v98);
    v100 = _str13;
    p_print_str(v100);
    goto bb1;
  bb3:
    v103 = (char*)v74 + 0;
    v104 = *(int64_t*)v103;
    v105 = 1LL;
    v106 = v104 == v105;
    if (v106) goto bb4;
    else goto bb5;
  bb4:
    v108 = (char*)v74 + 8;
    v109 = *(void**)v108;
    p_print_str(v109);
    v111 = _str13;
    p_print_str(v111);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

void map_own(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    int64_t v15 = 0;
    bool v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    int64_t v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    int64_t v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0;
    void* v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0;
    void* v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    void* v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0; /* placeholder for void */
    void* v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    void* v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    void* v46 = 0;
    int64_t v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    void* v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0;
    bool v53 = 0;
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0;
    int64_t v57 = 0;
    bool v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0; /* placeholder for void */
    bool v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    int64_t v63 = 0;
    int64_t v64 = 0;
    int64_t v65 = 0;
    void* v66 = 0;
    void* v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0;
    int64_t v70 = 0;
    int64_t v71 = 0;
    void* v72 = 0;
    void* v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0;
    void* v76 = 0;
    int64_t v77 = 0; /* placeholder for void */
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0;
    int64_t v81 = 0;
    bool v82 = 0;
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0;
    int64_t v85 = 0;
    int64_t v86 = 0;
    int64_t v87 = 0;
    int64_t v88 = 0;
    void* v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0;
    int64_t v92 = 0;
    int64_t v93 = 0;
    int64_t v94 = 0;
    int64_t v95 = 0;
    void* v96 = 0;
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0;
    int64_t v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0;
    int64_t v102 = 0;
    void* v103 = 0;
    int64_t v104 = 0; /* placeholder for void */
    int64_t v105 = 0;
    int64_t v106 = 0;
    int64_t v107 = 0;
    int64_t v108 = 0;
    int64_t v109 = 0;
    void* v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    int64_t v112 = 0; /* placeholder for void */
    void* v113 = 0;
    void* v114 = 0;
    int64_t v115 = 0;
    int64_t v116 = 0; /* placeholder for void */
    void* v117 = 0;
    void* v118 = 0;
    int64_t v119 = 0;
    int64_t v120 = 0; /* placeholder for void */
    int64_t v121 = 0; /* placeholder for void */
    int64_t v122 = 0;
    int64_t v123 = 0;
    int64_t v124 = 0;
    int64_t v125 = 0; /* placeholder for void */
    int64_t v126 = 0; /* placeholder for void */
    int64_t v127 = 0; /* placeholder for void */
    void* v128 = 0;
    int64_t v129 = 0;
    int64_t v130 = 0; /* placeholder for void */
    void* v131 = 0;
    int64_t v132 = 0;
    int64_t v133 = 0; /* placeholder for void */
    int64_t v134 = 0; /* placeholder for void */
    int64_t v135 = 0; /* placeholder for void */
    int64_t v136 = 0; /* placeholder for void */
    int64_t v137 = 0; /* placeholder for void */
    void* v138 = 0;
    int64_t v139 = 0;
    int64_t v140 = 0; /* placeholder for void */
    int64_t v141 = 0; /* placeholder for void */
    int64_t v142 = 0; /* placeholder for void */
    v4 = alloca(8);
    v7 = alloca(8);
    v20 = alloca(8);
    v24 = alloca(8);
    v28 = alloca(8);
    v31 = alloca(8);
    v34 = alloca(8);
    v37 = alloca(8);
    v50 = alloca(1);
    v67 = alloca(8);
    v73 = alloca(8);
    v76 = alloca(8);
    v89 = alloca(8);
    v96 = alloca(8);
    v103 = alloca(8);
    v110 = alloca(8);
    v2 = 32LL;
    v3 = p_add_i64(v1, v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    v6 = 0LL;
    memset(v7, 0, 8);
    *(int64_t*)v7 = (int64_t)v6;
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v7 = (int64_t)v11;
    v14 = *(int64_t*)v7;
    v15 = 0LL;
    v16 = v14 == v15;
    if (v16) goto bb1;
    else goto bb2;
  bb1:
    return;
  bb2:
    goto bb3;
  bb3:
    memset(v20, 0, 8);
    *(void**)v20 = (void*)v1;
    v22 = 8LL;
    v23 = p_add_i64(v1, v22);
    memset(v24, 0, 8);
    *(void**)v24 = (void*)v23;
    v26 = 24LL;
    v27 = p_add_i64(v1, v26);
    memset(v28, 0, 8);
    *(void**)v28 = (void*)v27;
    v30 = 0LL;
    memset(v31, 0, 8);
    *(int64_t*)v31 = (int64_t)v30;
    v33 = 0LL;
    memset(v34, 0, 8);
    *(int64_t*)v34 = (int64_t)v33;
    v36 = 0LL;
    memset(v37, 0, 8);
    *(int64_t*)v37 = (int64_t)v36;
    v40 = *(void**)v20;
    v41 = *(int64_t*)v40;
    *(int64_t*)v31 = (int64_t)v41;
    v43 = *(void**)v24;
    v44 = *(int64_t*)v43;
    *(int64_t*)v34 = (int64_t)v44;
    v46 = *(void**)v28;
    v47 = *(int64_t*)v46;
    *(int64_t*)v37 = (int64_t)v47;
    memset(v50, 0, 1);
    v51 = *(int64_t*)v37;
    v52 = 0LL;
    v53 = v51 > v52;
    *(bool*)v50 = v53;
    if (v53) goto bb4;
    else goto bb5;
  bb4:
    v56 = *(int64_t*)v31;
    v57 = 0LL;
    v58 = v56 != v57;
    *(bool*)v50 = v58;
    goto bb5;
  bb5:
    v61 = *(bool*)v50;
    if (v61) goto bb6;
    else goto bb7;
  bb6:
    v63 = *(int64_t*)v37;
    v64 = 8LL;
    v65 = p_mul_i64(v63, v64);
    v66 = p_malloc(v65);
    memset(v67, 0, 8);
    *(int64_t*)v67 = (int64_t)v66;
    v69 = *(int64_t*)v37;
    v70 = 8LL;
    v71 = p_mul_i64(v69, v70);
    v72 = p_malloc(v71);
    memset(v73, 0, 8);
    *(int64_t*)v73 = (int64_t)v72;
    v75 = 0LL;
    memset(v76, 0, 8);
    *(int64_t*)v76 = (int64_t)v75;
    goto bb9;
  bb7:
    goto bb8;
  bb8:
    v138 = *(void**)v4;
    v139 = 0LL;
    *(int64_t*)v138 = (int64_t)v139;
    return;
  bb9:
    p_co_tick();
    v80 = *(int64_t*)v76;
    v81 = *(int64_t*)v37;
    v82 = v80 < v81;
    if (v82) goto bb10;
    else goto bb11;
  bb10:
    v84 = *(int64_t*)v31;
    v85 = *(int64_t*)v76;
    v86 = 8LL;
    v87 = p_mul_i64(v85, v86);
    v88 = p_add_i64(v84, v87);
    memset(v89, 0, 8);
    *(void**)v89 = (void*)v88;
    v91 = *(int64_t*)v67;
    v92 = *(int64_t*)v76;
    v93 = 8LL;
    v94 = p_mul_i64(v92, v93);
    v95 = p_add_i64(v91, v94);
    memset(v96, 0, 8);
    *(void**)v96 = (void*)v95;
    v98 = *(int64_t*)v34;
    v99 = *(int64_t*)v76;
    v100 = 8LL;
    v101 = p_mul_i64(v99, v100);
    v102 = p_add_i64(v98, v101);
    memset(v103, 0, 8);
    *(void**)v103 = (void*)v102;
    v105 = *(int64_t*)v73;
    v106 = *(int64_t*)v76;
    v107 = 8LL;
    v108 = p_mul_i64(v106, v107);
    v109 = p_add_i64(v105, v108);
    memset(v110, 0, 8);
    *(void**)v110 = (void*)v109;
    v113 = *(void**)v96;
    v114 = *(void**)v89;
    v115 = *(int64_t*)v114;
    *(int64_t*)v113 = (int64_t)v115;
    v117 = *(void**)v110;
    v118 = *(void**)v103;
    v119 = *(int64_t*)v118;
    *(int64_t*)v117 = (int64_t)v119;
    v122 = *(int64_t*)v76;
    v123 = 1LL;
    v124 = (int64_t)((uint64_t)v122 + (uint64_t)v123);
    *(int64_t*)v76 = (int64_t)v124;
    goto bb9;
  bb11:
    v128 = *(void**)v20;
    v129 = *(int64_t*)v67;
    *(int64_t*)v128 = (int64_t)v129;
    v131 = *(void**)v24;
    v132 = *(int64_t*)v73;
    *(int64_t*)v131 = (int64_t)v132;
    goto bb8;
    return;
}

int64_t p_max(void* v0, int64_t v1, int64_t v2) {
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
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

int64_t p_min(void* v0, int64_t v1, int64_t v2) {
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
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

int64_t p_str_alloc(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    void* v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    int64_t v18 = 0;
    int64_t v19 = 0;
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    void* v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0;
    int64_t v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    v5 = alloca(8);
    v15 = alloca(8);
    v20 = alloca(8);
    v2 = 17LL;
    v3 = p_add_i64(v1, v2);
    v4 = p_malloc(v3);
    memset(v5, 0, 8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = *(int64_t*)v5;
    v8 = 0LL;
    v9 = v7 == v8;
    if (v9) goto bb1;
    else goto bb2;
  bb1:
    v11 = 0LL;
    return v11;
  bb2:
    goto bb3;
  bb3:
    v14 = *(int64_t*)v5;
    memset(v15, 0, 8);
    *(void**)v15 = (void*)v14;
    v17 = *(int64_t*)v5;
    v18 = 8LL;
    v19 = p_add_i64(v17, v18);
    memset(v20, 0, 8);
    *(void**)v20 = (void*)v19;
    v23 = *(void**)v15;
    v24 = 5064654320496300545LL;
    *(int64_t*)v23 = (int64_t)v24;
    v26 = *(void**)v20;
    *(int64_t*)v26 = (int64_t)v1;
    v29 = *(int64_t*)v5;
    v30 = 16LL;
    v31 = p_add_i64(v29, v30);
    return v31;
    return 0;
}

int64_t p_str_compare(void* v0, int64_t v1, int64_t v2) {
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    bool v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    void* v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0; /* placeholder for void */
    void* v31 = 0;
    uint8_t v32 = 0;
    void* v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    void* v35 = 0;
    uint8_t v36 = 0;
    void* v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0; /* placeholder for void */
    void* v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0;
    bool v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0;
    int64_t v48 = 0;
    bool v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0; /* placeholder for void */
    bool v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    int64_t v55 = 0;
    bool v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    int64_t v59 = 0;
    int64_t v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0; /* placeholder for void */
    int64_t v63 = 0;
    int64_t v64 = 0;
    int64_t v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0;
    int64_t v68 = 0;
    int64_t v69 = 0;
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    void* v75 = 0;
    uint8_t v76 = 0;
    int64_t v77 = 0; /* placeholder for void */
    void* v78 = 0;
    uint8_t v79 = 0;
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0;
    int64_t v83 = 0;
    int64_t v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    v20 = alloca(8);
    v22 = alloca(8);
    v25 = alloca(8);
    v28 = alloca(8);
    v33 = alloca(8);
    v37 = alloca(8);
    v41 = alloca(1);
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 0LL;
    return v5;
  bb2:
    goto bb3;
  bb3:
    v8 = 0LL;
    v9 = v1 == v8;
    if (v9) goto bb4;
    else goto bb5;
  bb4:
    v11 = -1LL;
    return v11;
  bb5:
    goto bb6;
  bb6:
    v14 = 0LL;
    v15 = v2 == v14;
    if (v15) goto bb7;
    else goto bb8;
  bb7:
    v17 = 1LL;
    return v17;
  bb8:
    goto bb9;
  bb9:
    memset(v20, 0, 8);
    *(int64_t*)v20 = (int64_t)v1;
    memset(v22, 0, 8);
    *(int64_t*)v22 = (int64_t)v2;
    v24 = *(int64_t*)v20;
    memset(v25, 0, 8);
    *(void**)v25 = (void*)v24;
    v27 = *(int64_t*)v22;
    memset(v28, 0, 8);
    *(void**)v28 = (void*)v27;
    v31 = *(void**)v25;
    v32 = *(uint8_t*)v31;
    memset(v33, 0, 8);
    *(int64_t*)v33 = (int64_t)v32;
    v35 = *(void**)v28;
    v36 = *(uint8_t*)v35;
    memset(v37, 0, 8);
    *(int64_t*)v37 = (int64_t)v36;
    goto bb10;
  bb10:
    p_co_tick();
    memset(v41, 0, 1);
    v42 = *(int64_t*)v33;
    v43 = 0LL;
    v44 = v42 != v43;
    *(bool*)v41 = v44;
    if (v44) goto bb13;
    else goto bb14;
  bb11:
    v54 = *(int64_t*)v33;
    v55 = *(int64_t*)v37;
    v56 = v54 != v55;
    if (v56) goto bb15;
    else goto bb16;
  bb12:
    v82 = *(int64_t*)v33;
    v83 = *(int64_t*)v37;
    v84 = p_sub_i64(v82, v83);
    return v84;
  bb13:
    v47 = *(int64_t*)v37;
    v48 = 0LL;
    v49 = v47 != v48;
    *(bool*)v41 = v49;
    goto bb14;
  bb14:
    v52 = *(bool*)v41;
    if (v52) goto bb11;
    else goto bb12;
  bb15:
    v58 = *(int64_t*)v33;
    v59 = *(int64_t*)v37;
    v60 = p_sub_i64(v58, v59);
    return v60;
  bb16:
    goto bb17;
  bb17:
    v63 = *(int64_t*)v20;
    v64 = 1LL;
    v65 = p_add_i64(v63, v64);
    *(int64_t*)v20 = (int64_t)v65;
    v67 = *(int64_t*)v22;
    v68 = 1LL;
    v69 = p_add_i64(v67, v68);
    *(int64_t*)v22 = (int64_t)v69;
    v71 = *(int64_t*)v20;
    *(void**)v25 = (void*)v71;
    v73 = *(int64_t*)v22;
    *(void**)v28 = (void*)v73;
    v75 = *(void**)v25;
    v76 = *(uint8_t*)v75;
    *(int64_t*)v33 = (int64_t)v76;
    v78 = *(void**)v28;
    v79 = *(uint8_t*)v78;
    *(int64_t*)v37 = (int64_t)v79;
    goto bb10;
    return 0;
}

void p_str_copy_n(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    void* v23 = 0;
    uint8_t v24 = 0;
    void* v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    void* v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0; /* placeholder for void */
    v4 = alloca(8);
    v6 = alloca(8);
    v9 = alloca(8);
    v17 = alloca(8);
    v20 = alloca(8);
    v25 = alloca(8);
    memset(v4, 0, 8);
    *(int64_t*)v4 = (int64_t)v1;
    memset(v6, 0, 8);
    *(int64_t*)v6 = (int64_t)v2;
    v8 = 0LL;
    memset(v9, 0, 8);
    *(int64_t*)v9 = (int64_t)v8;
    goto bb1;
  bb1:
    p_co_tick();
    v13 = *(int64_t*)v9;
    v14 = v13 < v3;
    if (v14) goto bb2;
    else goto bb3;
  bb2:
    v16 = *(int64_t*)v4;
    memset(v17, 0, 8);
    *(void**)v17 = (void*)v16;
    v19 = *(int64_t*)v6;
    memset(v20, 0, 8);
    *(void**)v20 = (void*)v19;
    v23 = *(void**)v20;
    v24 = *(uint8_t*)v23;
    memset(v25, 0, 8);
    *(int64_t*)v25 = (int64_t)v24;
    v27 = *(void**)v17;
    v28 = *(int64_t*)v25;
    *(uint8_t*)v27 = (uint8_t)v28;
    v31 = *(int64_t*)v4;
    v32 = 1LL;
    v33 = p_add_i64(v31, v32);
    *(int64_t*)v4 = (int64_t)v33;
    v35 = *(int64_t*)v6;
    v36 = 1LL;
    v37 = p_add_i64(v35, v36);
    *(int64_t*)v6 = (int64_t)v37;
    v39 = *(int64_t*)v9;
    v40 = 1LL;
    v41 = (int64_t)((uint64_t)v39 + (uint64_t)v40);
    *(int64_t*)v9 = (int64_t)v41;
    goto bb1;
  bb3:
    return;
    return;
}

bool p_str_equal(void* v0, int64_t v1, int64_t v2) {
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    bool v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    bool v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    bool v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    void* v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0; /* placeholder for void */
    void* v31 = 0;
    uint8_t v32 = 0;
    void* v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    void* v35 = 0;
    uint8_t v36 = 0;
    void* v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0; /* placeholder for void */
    void* v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0;
    bool v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0;
    int64_t v48 = 0;
    bool v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0; /* placeholder for void */
    bool v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    int64_t v55 = 0;
    bool v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    bool v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0;
    int64_t v66 = 0;
    int64_t v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0;
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    void* v73 = 0;
    uint8_t v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    void* v76 = 0;
    uint8_t v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0;
    int64_t v81 = 0;
    bool v82 = 0;
    int64_t v83 = 0; /* placeholder for void */
    v20 = alloca(8);
    v22 = alloca(8);
    v25 = alloca(8);
    v28 = alloca(8);
    v33 = alloca(8);
    v37 = alloca(8);
    v41 = alloca(1);
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = true;
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
    return v17;
  bb8:
    goto bb9;
  bb9:
    memset(v20, 0, 8);
    *(int64_t*)v20 = (int64_t)v1;
    memset(v22, 0, 8);
    *(int64_t*)v22 = (int64_t)v2;
    v24 = *(int64_t*)v20;
    memset(v25, 0, 8);
    *(void**)v25 = (void*)v24;
    v27 = *(int64_t*)v22;
    memset(v28, 0, 8);
    *(void**)v28 = (void*)v27;
    v31 = *(void**)v25;
    v32 = *(uint8_t*)v31;
    memset(v33, 0, 8);
    *(int64_t*)v33 = (int64_t)v32;
    v35 = *(void**)v28;
    v36 = *(uint8_t*)v35;
    memset(v37, 0, 8);
    *(int64_t*)v37 = (int64_t)v36;
    goto bb10;
  bb10:
    p_co_tick();
    memset(v41, 0, 1);
    v42 = *(int64_t*)v33;
    v43 = 0LL;
    v44 = v42 != v43;
    *(bool*)v41 = v44;
    if (v44) goto bb13;
    else goto bb14;
  bb11:
    v54 = *(int64_t*)v33;
    v55 = *(int64_t*)v37;
    v56 = v54 != v55;
    if (v56) goto bb15;
    else goto bb16;
  bb12:
    v80 = *(int64_t*)v33;
    v81 = *(int64_t*)v37;
    v82 = v80 == v81;
    return v82;
  bb13:
    v47 = *(int64_t*)v37;
    v48 = 0LL;
    v49 = v47 != v48;
    *(bool*)v41 = v49;
    goto bb14;
  bb14:
    v52 = *(bool*)v41;
    if (v52) goto bb11;
    else goto bb12;
  bb15:
    v58 = false;
    return v58;
  bb16:
    goto bb17;
  bb17:
    v61 = *(int64_t*)v20;
    v62 = 1LL;
    v63 = p_add_i64(v61, v62);
    *(int64_t*)v20 = (int64_t)v63;
    v65 = *(int64_t*)v22;
    v66 = 1LL;
    v67 = p_add_i64(v65, v66);
    *(int64_t*)v22 = (int64_t)v67;
    v69 = *(int64_t*)v20;
    *(void**)v25 = (void*)v69;
    v71 = *(int64_t*)v22;
    *(void**)v28 = (void*)v71;
    v73 = *(void**)v25;
    v74 = *(uint8_t*)v73;
    *(int64_t*)v33 = (int64_t)v74;
    v76 = *(void**)v28;
    v77 = *(uint8_t*)v76;
    *(int64_t*)v37 = (int64_t)v77;
    goto bb10;
    return 0;
}

void p_str_free(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    int64_t v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    int64_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0; /* placeholder for void */
    void* v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0;
    int64_t v19 = 0;
    bool v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    void* v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0;
    bool v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0; /* placeholder for void */
    bool v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    v9 = alloca(8);
    v13 = alloca(8);
    v16 = alloca(1);
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    return;
  bb2:
    goto bb3;
  bb3:
    v7 = 16LL;
    v8 = p_sub_i64(v1, v7);
    memset(v9, 0, 8);
    *(void**)v9 = (void*)v8;
    v11 = 8LL;
    v12 = p_sub_i64(v1, v11);
    memset(v13, 0, 8);
    *(void**)v13 = (void*)v12;
    memset(v16, 0, 1);
    v17 = *(void**)v9;
    v18 = *(int64_t*)v17;
    v19 = 5064654320496300545LL;
    v20 = v18 == v19;
    *(bool*)v16 = v20;
    if (v20) goto bb4;
    else goto bb5;
  bb4:
    v23 = *(void**)v13;
    v24 = *(int64_t*)v23;
    v25 = 0LL;
    v26 = (bool)p_str_len_word((void*)v25, v24);
    *(bool*)v16 = v26;
    goto bb5;
  bb5:
    v29 = *(bool*)v16;
    if (v29) goto bb6;
    else goto bb7;
  bb6:
    v31 = 16LL;
    v32 = p_sub_i64(v1, v31);
    p_free((void*)v32);
    return;
  bb7:
    goto bb8;
  bb8:
    p_free((void*)v1);
    return;
    return;
}

void* p_str_from_ptr(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    void* v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    void* v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0;
    void* v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0; /* placeholder for void */
    void* v29 = 0;
    uint8_t v30 = 0;
    void* v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0;
    int64_t v36 = 0;
    bool v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    void* v49 = 0;
    uint8_t v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    void* v57 = 0;
    void* v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    void* v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    void* v63 = 0;
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0;
    void* v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0;
    void* v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0; /* placeholder for void */
    int64_t v74 = 0;
    int64_t v75 = 0;
    bool v76 = 0;
    int64_t v77 = 0; /* placeholder for void */
    void* v78 = 0;
    uint8_t v79 = 0;
    void* v80 = 0;
    int64_t v81 = 0; /* placeholder for void */
    void* v82 = 0;
    int64_t v83 = 0;
    int64_t v84 = 0; /* placeholder for void */
    int64_t v85 = 0;
    int64_t v86 = 0;
    int64_t v87 = 0;
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0;
    int64_t v90 = 0;
    int64_t v91 = 0;
    int64_t v92 = 0; /* placeholder for void */
    int64_t v93 = 0;
    int64_t v94 = 0; /* placeholder for void */
    void* v95 = 0;
    void* v96 = 0;
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0;
    int64_t v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0;
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0; /* placeholder for void */
    void* v105 = 0;
    int64_t v106 = 0;
    int64_t v107 = 0; /* placeholder for void */
    int64_t v108 = 0; /* placeholder for void */
    int64_t v109 = 0;
    void* v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    void* v112 = 0;
    int64_t v113 = 0; /* placeholder for void */
    v7 = alloca(8);
    v10 = alloca(8);
    v20 = alloca(8);
    v23 = alloca(8);
    v26 = alloca(8);
    v31 = alloca(8);
    v58 = alloca(8);
    v61 = alloca(8);
    v63 = alloca(8);
    v66 = alloca(8);
    v70 = alloca(8);
    v80 = alloca(8);
    v96 = alloca(8);
    v110 = alloca(8);
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 1LL;
    v6 = p_malloc(v5);
    memset(v7, 0, 8);
    *(int64_t*)v7 = (int64_t)v6;
    v9 = *(int64_t*)v7;
    memset(v10, 0, 8);
    *(void**)v10 = (void*)v9;
    v13 = *(void**)v10;
    v14 = 0LL;
    *(uint8_t*)v13 = (uint8_t)v14;
    v17 = *(void**)v10;
    return v17;
  bb2:
    goto bb3;
  bb3:
    memset(v20, 0, 8);
    *(int64_t*)v20 = (int64_t)v1;
    v22 = *(int64_t*)v20;
    memset(v23, 0, 8);
    *(void**)v23 = (void*)v22;
    v25 = 0LL;
    memset(v26, 0, 8);
    *(int64_t*)v26 = (int64_t)v25;
    v29 = *(void**)v23;
    v30 = *(uint8_t*)v29;
    memset(v31, 0, 8);
    *(int64_t*)v31 = (int64_t)v30;
    goto bb4;
  bb4:
    p_co_tick();
    v35 = *(int64_t*)v31;
    v36 = 0LL;
    v37 = v35 != v36;
    if (v37) goto bb5;
    else goto bb6;
  bb5:
    v39 = *(int64_t*)v26;
    v40 = 1LL;
    v41 = p_add_i64(v39, v40);
    *(int64_t*)v26 = (int64_t)v41;
    v43 = *(int64_t*)v20;
    v44 = 1LL;
    v45 = p_add_i64(v43, v44);
    *(int64_t*)v20 = (int64_t)v45;
    v47 = *(int64_t*)v20;
    *(void**)v23 = (void*)v47;
    v49 = *(void**)v23;
    v50 = *(uint8_t*)v49;
    *(int64_t*)v31 = (int64_t)v50;
    goto bb4;
  bb6:
    v54 = *(int64_t*)v26;
    v55 = 1LL;
    v56 = p_add_i64(v54, v55);
    v57 = p_malloc(v56);
    memset(v58, 0, 8);
    *(int64_t*)v58 = (int64_t)v57;
    v60 = *(int64_t*)v58;
    memset(v61, 0, 8);
    *(void**)v61 = (void*)v60;
    memset(v63, 0, 8);
    *(int64_t*)v63 = (int64_t)v1;
    v65 = *(int64_t*)v63;
    memset(v66, 0, 8);
    *(void**)v66 = (void*)v65;
    v69 = 0LL;
    memset(v70, 0, 8);
    *(int64_t*)v70 = (int64_t)v69;
    goto bb7;
  bb7:
    p_co_tick();
    v74 = *(int64_t*)v70;
    v75 = *(int64_t*)v26;
    v76 = v74 < v75;
    if (v76) goto bb8;
    else goto bb9;
  bb8:
    v78 = *(void**)v66;
    v79 = *(uint8_t*)v78;
    memset(v80, 0, 8);
    *(int64_t*)v80 = (int64_t)v79;
    v82 = *(void**)v61;
    v83 = *(int64_t*)v80;
    *(uint8_t*)v82 = (uint8_t)v83;
    v85 = *(int64_t*)v70;
    v86 = 1LL;
    v87 = (int64_t)((uint64_t)v85 + (uint64_t)v86);
    *(int64_t*)v70 = (int64_t)v87;
    v89 = *(int64_t*)v63;
    v90 = 1LL;
    v91 = p_add_i64(v89, v90);
    *(int64_t*)v63 = (int64_t)v91;
    v93 = *(int64_t*)v63;
    *(void**)v66 = (void*)v93;
    v95 = *(void**)v61;
    memset(v96, 0, 8);
    *(int64_t*)v96 = (int64_t)v95;
    v98 = *(int64_t*)v96;
    v99 = 1LL;
    v100 = p_add_i64(v98, v99);
    *(int64_t*)v96 = (int64_t)v100;
    v102 = *(int64_t*)v96;
    *(void**)v61 = (void*)v102;
    goto bb7;
  bb9:
    v105 = *(void**)v61;
    v106 = 0LL;
    *(uint8_t*)v105 = (uint8_t)v106;
    v109 = *(int64_t*)v58;
    memset(v110, 0, 8);
    *(void**)v110 = (void*)v109;
    v112 = *(void**)v110;
    return v112;
    return 0;
}

int64_t p_str_hash(void* v0, int64_t v1) {
    int64_t v2 = 0;
    void* v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    uint8_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    int64_t v18 = 0;
    bool v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    int64_t v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0;
    int64_t v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0;
    int64_t v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    void* v35 = 0;
    uint8_t v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0;
    int64_t v41 = 0;
    bool v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    v3 = alloca(8);
    v5 = alloca(8);
    v8 = alloca(8);
    v13 = alloca(8);
    v2 = -3750763034362895579LL;
    memset(v3, 0, 8);
    *(int64_t*)v3 = (int64_t)v2;
    memset(v5, 0, 8);
    *(int64_t*)v5 = (int64_t)v1;
    v7 = *(int64_t*)v5;
    memset(v8, 0, 8);
    *(void**)v8 = (void*)v7;
    v11 = *(void**)v8;
    v12 = *(uint8_t*)v11;
    memset(v13, 0, 8);
    *(int64_t*)v13 = (int64_t)v12;
    goto bb1;
  bb1:
    p_co_tick();
    v17 = *(int64_t*)v13;
    v18 = 0LL;
    v19 = v17 != v18;
    if (v19) goto bb2;
    else goto bb3;
  bb2:
    v21 = *(int64_t*)v3;
    v22 = *(int64_t*)v13;
    v23 = v21 ^ v22;
    *(int64_t*)v3 = (int64_t)v23;
    v25 = *(int64_t*)v3;
    v26 = 1099511628211LL;
    v27 = (int64_t)((uint64_t)v25 * (uint64_t)v26);
    *(int64_t*)v3 = (int64_t)v27;
    v29 = *(int64_t*)v5;
    v30 = 1LL;
    v31 = (int64_t)((uint64_t)v29 + (uint64_t)v30);
    *(int64_t*)v5 = (int64_t)v31;
    v33 = *(int64_t*)v5;
    *(void**)v8 = (void*)v33;
    v35 = *(void**)v8;
    v36 = *(uint8_t*)v35;
    *(int64_t*)v13 = (int64_t)v36;
    goto bb1;
  bb3:
    v40 = *(int64_t*)v3;
    v41 = 0LL;
    v42 = v40 < v41;
    if (v42) goto bb4;
    else goto bb5;
  bb4:
    v44 = *(int64_t*)v3;
    v45 = 0LL;
    v46 = (int64_t)((uint64_t)v45 - (uint64_t)v44);
    return v46;
  bb5:
    goto bb6;
  bb6:
    v49 = *(int64_t*)v3;
    return v49;
    return 0;
}

bool p_str_len_word(void* v0, int64_t v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    bool v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    bool v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    v2 = alloca(1);
    memset(v2, 0, 1);
    v3 = 0LL;
    v4 = v1 >= v3;
    *(bool*)v2 = v4;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    v7 = 1099511627776LL;
    v8 = v1 < v7;
    *(bool*)v2 = v8;
    goto bb2;
  bb2:
    v11 = *(bool*)v2;
    return v11;
    return 0;
}

int64_t p_str_length(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    int64_t v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0;
    int64_t v13 = 0;
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    void* v18 = 0;
    int64_t v19 = 0;
    int64_t v20 = 0;
    bool v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0; /* placeholder for void */
    void* v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0;
    bool v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0; /* placeholder for void */
    bool v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    void* v32 = 0;
    int64_t v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0; /* placeholder for void */
    void* v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    void* v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0;
    void* v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    void* v46 = 0;
    uint8_t v47 = 0;
    void* v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    int64_t v53 = 0;
    bool v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0;
    int64_t v57 = 0;
    int64_t v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    int64_t v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    void* v66 = 0;
    uint8_t v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    v10 = alloca(8);
    v14 = alloca(8);
    v17 = alloca(1);
    v37 = alloca(8);
    v40 = alloca(8);
    v43 = alloca(8);
    v48 = alloca(8);
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = 0LL;
    return v5;
  bb2:
    goto bb3;
  bb3:
    v8 = 16LL;
    v9 = p_sub_i64(v1, v8);
    memset(v10, 0, 8);
    *(void**)v10 = (void*)v9;
    v12 = 8LL;
    v13 = p_sub_i64(v1, v12);
    memset(v14, 0, 8);
    *(void**)v14 = (void*)v13;
    memset(v17, 0, 1);
    v18 = *(void**)v10;
    v19 = *(int64_t*)v18;
    v20 = 5064654320496300545LL;
    v21 = v19 == v20;
    *(bool*)v17 = v21;
    if (v21) goto bb4;
    else goto bb5;
  bb4:
    v24 = *(void**)v14;
    v25 = *(int64_t*)v24;
    v26 = 0LL;
    v27 = (bool)p_str_len_word((void*)v26, v25);
    *(bool*)v17 = v27;
    goto bb5;
  bb5:
    v30 = *(bool*)v17;
    if (v30) goto bb6;
    else goto bb7;
  bb6:
    v32 = *(void**)v14;
    v33 = *(int64_t*)v32;
    return v33;
  bb7:
    goto bb8;
  bb8:
    memset(v37, 0, 8);
    *(int64_t*)v37 = (int64_t)v1;
    v39 = *(int64_t*)v37;
    memset(v40, 0, 8);
    *(void**)v40 = (void*)v39;
    v42 = 0LL;
    memset(v43, 0, 8);
    *(int64_t*)v43 = (int64_t)v42;
    v46 = *(void**)v40;
    v47 = *(uint8_t*)v46;
    memset(v48, 0, 8);
    *(int64_t*)v48 = (int64_t)v47;
    goto bb9;
  bb9:
    p_co_tick();
    v52 = *(int64_t*)v48;
    v53 = 0LL;
    v54 = v52 != v53;
    if (v54) goto bb10;
    else goto bb11;
  bb10:
    v56 = *(int64_t*)v43;
    v57 = 1LL;
    v58 = p_add_i64(v56, v57);
    *(int64_t*)v43 = (int64_t)v58;
    v60 = *(int64_t*)v37;
    v61 = 1LL;
    v62 = p_add_i64(v60, v61);
    *(int64_t*)v37 = (int64_t)v62;
    v64 = *(int64_t*)v37;
    *(void**)v40 = (void*)v64;
    v66 = *(void**)v40;
    v67 = *(uint8_t*)v66;
    *(int64_t*)v48 = (int64_t)v67;
    goto bb9;
  bb11:
    v71 = *(int64_t*)v43;
    return v71;
    return 0;
}

void panic(void* v0, void* v1) {
    int64_t v2 = 0; /* placeholder for void */
    int64_t v3 = 0; /* placeholder for void */
    p_panic((void*)v1);
    return;
    return;
}

void println(void* v0, void* v1) {
    int64_t v2 = 0; /* placeholder for void */
    void* v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0; /* placeholder for void */
    p_print_str(v1);
    v3 = _str13;
    p_print_str(v3);
    return;
    return;
}

int64_t str_byte_at(void* v0, void* v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)String_byte_at((void*)v3, (int64_t)v1, v2);
    return v4;
    return 0;
}

void* str_bytes(void* v0, void* v1) {
    int64_t v2 = 0;
    void* v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    int64_t v7 = 0;
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    int64_t v16 = 0;
    bool v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    int64_t v20 = 0;
    void* v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    void* v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0; /* placeholder for void */
    void* v30 = 0;
    uint8_t v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    void* v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0; /* placeholder for void */
    void* v43 = 0;
    int64_t v44 = 0;
    void* v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    v4 = alloca(8);
    v8 = alloca(8);
    v11 = alloca(8);
    v21 = alloca(8);
    v24 = alloca(8);
    v27 = alloca(8);
    v2 = 0LL;
    v3 = (void*)Vec_new((void*)v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    v6 = 0LL;
    v7 = (int64_t)String_len((void*)v6, (int64_t)v1);
    memset(v8, 0, 8);
    *(int64_t*)v8 = (int64_t)v7;
    v10 = 0LL;
    memset(v11, 0, 8);
    *(int64_t*)v11 = (int64_t)v10;
    goto bb1;
  bb1:
    p_co_tick();
    v15 = *(int64_t*)v11;
    v16 = *(int64_t*)v8;
    v17 = v15 < v16;
    if (v17) goto bb2;
    else goto bb3;
  bb2:
    v19 = *(int64_t*)v11;
    v20 = (int64_t)v1 + v19;
    memset(v21, 0, 8);
    *(int64_t*)v21 = (int64_t)v20;
    v23 = *(int64_t*)v21;
    memset(v24, 0, 8);
    *(void**)v24 = (void*)v23;
    v26 = 0LL;
    memset(v27, 0, 8);
    *(int64_t*)v27 = (int64_t)v26;
    v30 = *(void**)v24;
    v31 = *(uint8_t*)v30;
    *(int64_t*)v27 = (int64_t)v31;
    v34 = *(void**)v4;
    v35 = *(int64_t*)v27;
    v36 = 0LL;
    Vec_push((void*)v36, (int64_t)v34, v35);
    v38 = *(int64_t*)v11;
    v39 = 1LL;
    v40 = (int64_t)((uint64_t)v38 + (uint64_t)v39);
    *(int64_t*)v11 = (int64_t)v40;
    goto bb1;
  bb3:
    v43 = *(void**)v4;
    v44 = 0LL;
    v45 = (void*)Vec_share((void*)v44, (int64_t)v43);
    return v45;
    return 0;
}

void* str_clone(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    int64_t v7 = 0;
    int64_t v8 = 0;
    int64_t v9 = 0;
    int64_t v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    v4 = alloca(8);
    v2 = 0LL;
    v3 = (int64_t)str_len((void*)v2, (void*)v1);
    memset(v4, 0, 8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    v7 = 0LL;
    v8 = *(int64_t*)v4;
    v9 = 0LL;
    v10 = (int64_t)String_slice_len((void*)v9, (int64_t)v1, v6, v7, v8);
    return (void*)v10;
    return 0;
}

int64_t str_compare(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)p_str_compare((void*)v3, (int64_t)v1, (int64_t)v2);
    return v4;
    return 0;
}

void* str_concat(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v3 = p_str_concat((void*)v1, (void*)v2);
    return v3;
    return 0;
}

bool str_contains(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (bool)String_contains((void*)v3, (int64_t)v1, (int64_t)v2);
    return v4;
    return 0;
}

bool str_ends_with(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (bool)String_ends_with((void*)v3, (int64_t)v1, (int64_t)v2);
    return v4;
    return 0;
}

bool str_equal(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (bool)p_str_equal((void*)v3, (int64_t)v1, (int64_t)v2);
    return v4;
    return 0;
}

int64_t str_find(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)String_find((void*)v3, (int64_t)v1, (int64_t)v2);
    return v4;
    return 0;
}

void* str_from_bytes(void* v0, void* v1) {
    void* v2 = 0;
    void* v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    int64_t v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    int64_t v15 = 0;
    bool v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    int64_t v19 = 0;
    int64_t v20 = 0;
    int64_t v21 = 0;
    void* v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0;
    int64_t v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0; /* placeholder for void */
    void* v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    v3 = alloca(8);
    v6 = alloca(8);
    v10 = alloca(8);
    v2 = _str0;
    memset(v3, 0, 8);
    *(void**)v3 = (void*)v2;
    v5 = 0LL;
    memset(v6, 0, 8);
    *(int64_t*)v6 = (int64_t)v5;
    v8 = 0LL;
    v9 = (int64_t)Vec_len((void*)v8, (int64_t)v1);
    memset(v10, 0, 8);
    *(int64_t*)v10 = (int64_t)v9;
    goto bb1;
  bb1:
    p_co_tick();
    v14 = *(int64_t*)v6;
    v15 = *(int64_t*)v10;
    v16 = v14 < v15;
    if (v16) goto bb2;
    else goto bb3;
  bb2:
    v18 = *(void**)v3;
    v19 = *(int64_t*)v6;
    v20 = 0LL;
    v21 = (int64_t)Vec_get((void*)v20, (int64_t)v1, v19);
    v22 = p_char_to_str(v21);
    v23 = p_str_concat((void*)v18, (void*)v22);
    *(void**)v3 = (void*)v23;
    v25 = *(int64_t*)v6;
    v26 = 1LL;
    v27 = (int64_t)((uint64_t)v25 + (uint64_t)v26);
    *(int64_t*)v6 = (int64_t)v27;
    goto bb1;
  bb3:
    v30 = *(void**)v3;
    return v30;
    return 0;
}

bool str_is_empty(void* v0, void* v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (bool)String_is_empty((void*)v2, (int64_t)v1);
    return v3;
    return 0;
}

int64_t str_len(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_len((void*)v2, (int64_t)v1);
    return v3;
    return 0;
}

void* str_lines(void* v0, void* v1) {
    int64_t v2 = 0;
    void* v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0;
    int64_t v13 = 0;
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0;
    int64_t v19 = 0;
    bool v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0;
    bool v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    void* v28 = 0;
    int64_t v29 = 0;
    int64_t v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    int64_t v47 = 0;
    bool v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    void* v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0; /* placeholder for void */
    void* v59 = 0;
    int64_t v60 = 0;
    void* v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    v4 = alloca(8);
    v7 = alloca(8);
    v10 = alloca(8);
    v14 = alloca(8);
    v2 = 0LL;
    v3 = (void*)Vec_new((void*)v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    v6 = 0LL;
    memset(v7, 0, 8);
    *(int64_t*)v7 = (int64_t)v6;
    v9 = 0LL;
    memset(v10, 0, 8);
    *(int64_t*)v10 = (int64_t)v9;
    v12 = 0LL;
    v13 = (int64_t)String_len((void*)v12, (int64_t)v1);
    memset(v14, 0, 8);
    *(int64_t*)v14 = (int64_t)v13;
    goto bb1;
  bb1:
    p_co_tick();
    v18 = *(int64_t*)v7;
    v19 = *(int64_t*)v14;
    v20 = v18 < v19;
    if (v20) goto bb2;
    else goto bb3;
  bb2:
    v22 = *(int64_t*)v7;
    v23 = 0LL;
    v24 = (int64_t)str_byte_at((void*)v23, (void*)v1, v22);
    v25 = 10LL;
    v26 = v24 == v25;
    if (v26) goto bb4;
    else goto bb5;
  bb3:
    v46 = *(int64_t*)v10;
    v47 = *(int64_t*)v14;
    v48 = v46 < v47;
    if (v48) goto bb7;
    else goto bb8;
  bb4:
    v28 = *(void**)v4;
    v29 = *(int64_t*)v10;
    v30 = *(int64_t*)v7;
    v31 = 0LL;
    v32 = (int64_t)String_slice((void*)v31, (int64_t)v1, v29, v30);
    v33 = 0LL;
    Vec_push((void*)v33, (int64_t)v28, v32);
    v35 = *(int64_t*)v7;
    v36 = 1LL;
    v37 = p_add_i64(v35, v36);
    *(int64_t*)v10 = (int64_t)v37;
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v41 = *(int64_t*)v7;
    v42 = 1LL;
    v43 = (int64_t)((uint64_t)v41 + (uint64_t)v42);
    *(int64_t*)v7 = (int64_t)v43;
    goto bb1;
  bb7:
    v50 = *(void**)v4;
    v51 = *(int64_t*)v10;
    v52 = *(int64_t*)v14;
    v53 = 0LL;
    v54 = (int64_t)String_slice((void*)v53, (int64_t)v1, v51, v52);
    v55 = 0LL;
    Vec_push((void*)v55, (int64_t)v50, v54);
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    v59 = *(void**)v4;
    v60 = 0LL;
    v61 = (void*)Vec_share((void*)v60, (int64_t)v59);
    return v61;
    return 0;
}

double str_parse_float(void* v0, void* v1) {
    double v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = (double)p_str_to_float(v1);
    return v2;
    return 0;
}

int64_t str_parse_hex(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    int64_t v7 = 0;
    bool v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    bool v19 = 0;
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    int64_t v25 = 0;
    bool v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0;
    int64_t v29 = 0;
    void* v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    void* v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0;
    void* v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    void* v39 = 0;
    uint8_t v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    int64_t v44 = 0;
    bool v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0;
    int64_t v56 = 0;
    bool v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
    int64_t v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0;
    int64_t v63 = 0;
    void* v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0;
    void* v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0;
    void* v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0; /* placeholder for void */
    void* v73 = 0;
    uint8_t v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0;
    int64_t v78 = 0;
    bool v79 = 0;
    int64_t v80 = 0; /* placeholder for void */
    bool v81 = 0;
    int64_t v82 = 0; /* placeholder for void */
    int64_t v83 = 0;
    int64_t v84 = 0;
    int64_t v85 = 0;
    int64_t v86 = 0; /* placeholder for void */
    int64_t v87 = 0; /* placeholder for void */
    int64_t v88 = 0;
    int64_t v89 = 0;
    bool v90 = 0;
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0;
    int64_t v93 = 0;
    int64_t v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0; /* placeholder for void */
    int64_t v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0;
    int64_t v102 = 0;
    bool v103 = 0;
    int64_t v104 = 0; /* placeholder for void */
    int64_t v105 = 0;
    int64_t v106 = 0;
    void* v107 = 0;
    int64_t v108 = 0; /* placeholder for void */
    int64_t v109 = 0;
    void* v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    int64_t v112 = 0;
    void* v113 = 0;
    int64_t v114 = 0; /* placeholder for void */
    int64_t v115 = 0; /* placeholder for void */
    void* v116 = 0;
    uint8_t v117 = 0;
    int64_t v118 = 0; /* placeholder for void */
    int64_t v119 = 0; /* placeholder for void */
    int64_t v120 = 0;
    int64_t v121 = 0;
    int64_t v122 = 0;
    int64_t v123 = 0;
    void* v124 = 0;
    int64_t v125 = 0; /* placeholder for void */
    int64_t v126 = 0;
    void* v127 = 0;
    int64_t v128 = 0; /* placeholder for void */
    int64_t v129 = 0;
    void* v130 = 0;
    int64_t v131 = 0; /* placeholder for void */
    int64_t v132 = 0; /* placeholder for void */
    void* v133 = 0;
    uint8_t v134 = 0;
    int64_t v135 = 0; /* placeholder for void */
    int64_t v136 = 0; /* placeholder for void */
    void* v137 = 0;
    int64_t v138 = 0;
    int64_t v139 = 0;
    bool v140 = 0;
    int64_t v141 = 0; /* placeholder for void */
    int64_t v142 = 0; /* placeholder for void */
    void* v143 = 0;
    int64_t v144 = 0;
    int64_t v145 = 0;
    bool v146 = 0;
    int64_t v147 = 0; /* placeholder for void */
    int64_t v148 = 0; /* placeholder for void */
    int64_t v149 = 0;
    int64_t v150 = 0;
    bool v151 = 0;
    int64_t v152 = 0; /* placeholder for void */
    int64_t v153 = 0; /* placeholder for void */
    bool v154 = 0;
    int64_t v155 = 0; /* placeholder for void */
    int64_t v156 = 0; /* placeholder for void */
    bool v157 = 0;
    int64_t v158 = 0; /* placeholder for void */
    int64_t v159 = 0;
    int64_t v160 = 0;
    int64_t v161 = 0;
    int64_t v162 = 0; /* placeholder for void */
    int64_t v163 = 0; /* placeholder for void */
    int64_t v164 = 0; /* placeholder for void */
    int64_t v165 = 0; /* placeholder for void */
    int64_t v166 = 0; /* placeholder for void */
    int64_t v167 = 0; /* placeholder for void */
    int64_t v168 = 0; /* placeholder for void */
    int64_t v169 = 0;
    int64_t v170 = 0;
    bool v171 = 0;
    int64_t v172 = 0; /* placeholder for void */
    int64_t v173 = 0;
    int64_t v174 = 0;
    void* v175 = 0;
    int64_t v176 = 0; /* placeholder for void */
    int64_t v177 = 0;
    void* v178 = 0;
    int64_t v179 = 0; /* placeholder for void */
    int64_t v180 = 0;
    void* v181 = 0;
    int64_t v182 = 0; /* placeholder for void */
    int64_t v183 = 0; /* placeholder for void */
    void* v184 = 0;
    uint8_t v185 = 0;
    int64_t v186 = 0; /* placeholder for void */
    int64_t v187 = 0; /* placeholder for void */
    int64_t v188 = 0;
    void* v189 = 0;
    int64_t v190 = 0; /* placeholder for void */
    void* v191 = 0;
    int64_t v192 = 0;
    int64_t v193 = 0;
    bool v194 = 0;
    int64_t v195 = 0; /* placeholder for void */
    int64_t v196 = 0; /* placeholder for void */
    int64_t v197 = 0;
    int64_t v198 = 0;
    bool v199 = 0;
    int64_t v200 = 0; /* placeholder for void */
    int64_t v201 = 0; /* placeholder for void */
    bool v202 = 0;
    int64_t v203 = 0; /* placeholder for void */
    int64_t v204 = 0;
    int64_t v205 = 0;
    int64_t v206 = 0;
    int64_t v207 = 0; /* placeholder for void */
    int64_t v208 = 0; /* placeholder for void */
    void* v209 = 0;
    int64_t v210 = 0;
    int64_t v211 = 0;
    bool v212 = 0;
    int64_t v213 = 0; /* placeholder for void */
    int64_t v214 = 0; /* placeholder for void */
    int64_t v215 = 0;
    int64_t v216 = 0;
    bool v217 = 0;
    int64_t v218 = 0; /* placeholder for void */
    int64_t v219 = 0; /* placeholder for void */
    bool v220 = 0;
    int64_t v221 = 0; /* placeholder for void */
    int64_t v222 = 0;
    int64_t v223 = 0;
    int64_t v224 = 0;
    int64_t v225 = 0; /* placeholder for void */
    int64_t v226 = 0; /* placeholder for void */
    void* v227 = 0;
    int64_t v228 = 0;
    int64_t v229 = 0;
    bool v230 = 0;
    int64_t v231 = 0; /* placeholder for void */
    int64_t v232 = 0; /* placeholder for void */
    int64_t v233 = 0;
    int64_t v234 = 0;
    bool v235 = 0;
    int64_t v236 = 0; /* placeholder for void */
    int64_t v237 = 0; /* placeholder for void */
    bool v238 = 0;
    int64_t v239 = 0; /* placeholder for void */
    int64_t v240 = 0;
    int64_t v241 = 0;
    int64_t v242 = 0;
    int64_t v243 = 0; /* placeholder for void */
    int64_t v244 = 0; /* placeholder for void */
    int64_t v245 = 0; /* placeholder for void */
    int64_t v246 = 0; /* placeholder for void */
    int64_t v247 = 0; /* placeholder for void */
    int64_t v248 = 0;
    int64_t v249 = 0;
    int64_t v250 = 0;
    int64_t v251 = 0;
    int64_t v252 = 0;
    int64_t v253 = 0; /* placeholder for void */
    int64_t v254 = 0;
    int64_t v255 = 0;
    int64_t v256 = 0;
    int64_t v257 = 0; /* placeholder for void */
    int64_t v258 = 0; /* placeholder for void */
    bool v259 = 0;
    int64_t v260 = 0; /* placeholder for void */
    int64_t v261 = 0;
    int64_t v262 = 0;
    int64_t v263 = 0;
    int64_t v264 = 0; /* placeholder for void */
    int64_t v265 = 0; /* placeholder for void */
    int64_t v266 = 0; /* placeholder for void */
    int64_t v267 = 0;
    int64_t v268 = 0; /* placeholder for void */
    v4 = alloca(8);
    v14 = alloca(8);
    v17 = alloca(8);
    v20 = alloca(1);
    v30 = alloca(8);
    v33 = alloca(8);
    v36 = alloca(8);
    v64 = alloca(8);
    v67 = alloca(8);
    v70 = alloca(8);
    v107 = alloca(8);
    v110 = alloca(8);
    v113 = alloca(8);
    v124 = alloca(8);
    v127 = alloca(8);
    v130 = alloca(8);
    v137 = alloca(1);
    v143 = alloca(1);
    v175 = alloca(8);
    v178 = alloca(8);
    v181 = alloca(8);
    v189 = alloca(8);
    v191 = alloca(1);
    v209 = alloca(1);
    v227 = alloca(1);
    v2 = 0LL;
    v3 = (int64_t)String_len((void*)v2, (int64_t)v1);
    memset(v4, 0, 8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    v7 = 0LL;
    v8 = v6 == v7;
    if (v8) goto bb1;
    else goto bb2;
  bb1:
    v10 = 0LL;
    return v10;
  bb2:
    goto bb3;
  bb3:
    v13 = 0LL;
    memset(v14, 0, 8);
    *(int64_t*)v14 = (int64_t)v13;
    v16 = 0LL;
    memset(v17, 0, 8);
    *(int64_t*)v17 = (int64_t)v16;
    v19 = false;
    memset(v20, 0, 1);
    *(bool*)v20 = v19;
    goto bb4;
  bb4:
    p_co_tick();
    v24 = *(int64_t*)v14;
    v25 = *(int64_t*)v4;
    v26 = v24 < v25;
    if (v26) goto bb5;
    else goto bb6;
  bb5:
    v28 = *(int64_t*)v14;
    v29 = (int64_t)v1 + v28;
    memset(v30, 0, 8);
    *(int64_t*)v30 = (int64_t)v29;
    v32 = *(int64_t*)v30;
    memset(v33, 0, 8);
    *(void**)v33 = (void*)v32;
    v35 = 0LL;
    memset(v36, 0, 8);
    *(int64_t*)v36 = (int64_t)v35;
    v39 = *(void**)v33;
    v40 = *(uint8_t*)v39;
    *(int64_t*)v36 = (int64_t)v40;
    v43 = *(int64_t*)v36;
    v44 = 0LL;
    v45 = (bool)char_is_whitespace((void*)v44, v43);
    v46 = !v45;
    if (v46) goto bb7;
    else goto bb8;
  bb6:
    v55 = *(int64_t*)v14;
    v56 = *(int64_t*)v4;
    v57 = v55 >= v56;
    if (v57) goto bb10;
    else goto bb11;
  bb7:
    goto bb6;
  bb8:
    goto bb9;
  bb9:
    v50 = *(int64_t*)v14;
    v51 = 1LL;
    v52 = (int64_t)((uint64_t)v50 + (uint64_t)v51);
    *(int64_t*)v14 = (int64_t)v52;
    goto bb4;
  bb10:
    v59 = 0LL;
    return v59;
  bb11:
    goto bb12;
  bb12:
    v62 = *(int64_t*)v14;
    v63 = (int64_t)v1 + v62;
    memset(v64, 0, 8);
    *(int64_t*)v64 = (int64_t)v63;
    v66 = *(int64_t*)v64;
    memset(v67, 0, 8);
    *(void**)v67 = (void*)v66;
    v69 = 0LL;
    memset(v70, 0, 8);
    *(int64_t*)v70 = (int64_t)v69;
    v73 = *(void**)v67;
    v74 = *(uint8_t*)v73;
    *(int64_t*)v70 = (int64_t)v74;
    v77 = *(int64_t*)v70;
    v78 = 45LL;
    v79 = v77 == v78;
    if (v79) goto bb13;
    else goto bb14;
  bb13:
    v81 = true;
    *(bool*)v20 = v81;
    v83 = *(int64_t*)v14;
    v84 = 1LL;
    v85 = (int64_t)((uint64_t)v83 + (uint64_t)v84);
    *(int64_t*)v14 = (int64_t)v85;
    goto bb15;
  bb14:
    v88 = *(int64_t*)v70;
    v89 = 43LL;
    v90 = v88 == v89;
    if (v90) goto bb16;
    else goto bb17;
  bb15:
    v99 = *(int64_t*)v14;
    v100 = 1LL;
    v101 = (int64_t)((uint64_t)v99 + (uint64_t)v100);
    v102 = *(int64_t*)v4;
    v103 = v101 < v102;
    if (v103) goto bb19;
    else goto bb20;
  bb16:
    v92 = *(int64_t*)v14;
    v93 = 1LL;
    v94 = (int64_t)((uint64_t)v92 + (uint64_t)v93);
    *(int64_t*)v14 = (int64_t)v94;
    goto bb18;
  bb17:
    goto bb18;
  bb18:
    goto bb15;
  bb19:
    v105 = *(int64_t*)v14;
    v106 = (int64_t)v1 + v105;
    memset(v107, 0, 8);
    *(int64_t*)v107 = (int64_t)v106;
    v109 = *(int64_t*)v107;
    memset(v110, 0, 8);
    *(void**)v110 = (void*)v109;
    v112 = 0LL;
    memset(v113, 0, 8);
    *(int64_t*)v113 = (int64_t)v112;
    v116 = *(void**)v110;
    v117 = *(uint8_t*)v116;
    *(int64_t*)v113 = (int64_t)v117;
    v120 = *(int64_t*)v14;
    v121 = (int64_t)v1 + v120;
    v122 = 1LL;
    v123 = (int64_t)((uint64_t)v121 + (uint64_t)v122);
    memset(v124, 0, 8);
    *(int64_t*)v124 = (int64_t)v123;
    v126 = *(int64_t*)v124;
    memset(v127, 0, 8);
    *(void**)v127 = (void*)v126;
    v129 = 0LL;
    memset(v130, 0, 8);
    *(int64_t*)v130 = (int64_t)v129;
    v133 = *(void**)v127;
    v134 = *(uint8_t*)v133;
    *(int64_t*)v130 = (int64_t)v134;
    memset(v137, 0, 1);
    v138 = *(int64_t*)v113;
    v139 = 48LL;
    v140 = v138 == v139;
    *(bool*)v137 = v140;
    if (v140) goto bb22;
    else goto bb23;
  bb20:
    goto bb21;
  bb21:
    goto bb29;
  bb22:
    memset(v143, 0, 1);
    v144 = *(int64_t*)v130;
    v145 = 120LL;
    v146 = v144 == v145;
    *(bool*)v143 = v146;
    if (v146) goto bb25;
    else goto bb24;
  bb23:
    v157 = *(bool*)v137;
    if (v157) goto bb26;
    else goto bb27;
  bb24:
    v149 = *(int64_t*)v130;
    v150 = 88LL;
    v151 = v149 == v150;
    *(bool*)v143 = v151;
    goto bb25;
  bb25:
    v154 = *(bool*)v143;
    *(bool*)v137 = v154;
    goto bb23;
  bb26:
    v159 = *(int64_t*)v14;
    v160 = 2LL;
    v161 = (int64_t)((uint64_t)v159 + (uint64_t)v160);
    *(int64_t*)v14 = (int64_t)v161;
    goto bb28;
  bb27:
    goto bb28;
  bb28:
    goto bb21;
  bb29:
    p_co_tick();
    v169 = *(int64_t*)v14;
    v170 = *(int64_t*)v4;
    v171 = v169 < v170;
    if (v171) goto bb30;
    else goto bb31;
  bb30:
    v173 = *(int64_t*)v14;
    v174 = (int64_t)v1 + v173;
    memset(v175, 0, 8);
    *(int64_t*)v175 = (int64_t)v174;
    v177 = *(int64_t*)v175;
    memset(v178, 0, 8);
    *(void**)v178 = (void*)v177;
    v180 = 0LL;
    memset(v181, 0, 8);
    *(int64_t*)v181 = (int64_t)v180;
    v184 = *(void**)v178;
    v185 = *(uint8_t*)v184;
    *(int64_t*)v181 = (int64_t)v185;
    v188 = -1LL;
    memset(v189, 0, 8);
    *(int64_t*)v189 = (int64_t)v188;
    memset(v191, 0, 1);
    v192 = *(int64_t*)v181;
    v193 = 48LL;
    v194 = v192 >= v193;
    *(bool*)v191 = v194;
    if (v194) goto bb32;
    else goto bb33;
  bb31:
    v259 = *(bool*)v20;
    if (v259) goto bb47;
    else goto bb48;
  bb32:
    v197 = *(int64_t*)v181;
    v198 = 57LL;
    v199 = v197 <= v198;
    *(bool*)v191 = v199;
    goto bb33;
  bb33:
    v202 = *(bool*)v191;
    if (v202) goto bb34;
    else goto bb35;
  bb34:
    v204 = *(int64_t*)v181;
    v205 = 48LL;
    v206 = (int64_t)((uint64_t)v204 - (uint64_t)v205);
    *(int64_t*)v189 = (int64_t)v206;
    goto bb36;
  bb35:
    memset(v209, 0, 1);
    v210 = *(int64_t*)v181;
    v211 = 65LL;
    v212 = v210 >= v211;
    *(bool*)v209 = v212;
    if (v212) goto bb37;
    else goto bb38;
  bb36:
    v248 = *(int64_t*)v17;
    v249 = 16LL;
    v250 = (int64_t)((uint64_t)v248 * (uint64_t)v249);
    v251 = *(int64_t*)v189;
    v252 = (int64_t)((uint64_t)v250 + (uint64_t)v251);
    *(int64_t*)v17 = (int64_t)v252;
    v254 = *(int64_t*)v14;
    v255 = 1LL;
    v256 = (int64_t)((uint64_t)v254 + (uint64_t)v255);
    *(int64_t*)v14 = (int64_t)v256;
    goto bb29;
  bb37:
    v215 = *(int64_t*)v181;
    v216 = 70LL;
    v217 = v215 <= v216;
    *(bool*)v209 = v217;
    goto bb38;
  bb38:
    v220 = *(bool*)v209;
    if (v220) goto bb39;
    else goto bb40;
  bb39:
    v222 = *(int64_t*)v181;
    v223 = 55LL;
    v224 = (int64_t)((uint64_t)v222 - (uint64_t)v223);
    *(int64_t*)v189 = (int64_t)v224;
    goto bb41;
  bb40:
    memset(v227, 0, 1);
    v228 = *(int64_t*)v181;
    v229 = 97LL;
    v230 = v228 >= v229;
    *(bool*)v227 = v230;
    if (v230) goto bb42;
    else goto bb43;
  bb41:
    goto bb36;
  bb42:
    v233 = *(int64_t*)v181;
    v234 = 102LL;
    v235 = v233 <= v234;
    *(bool*)v227 = v235;
    goto bb43;
  bb43:
    v238 = *(bool*)v227;
    if (v238) goto bb44;
    else goto bb45;
  bb44:
    v240 = *(int64_t*)v181;
    v241 = 87LL;
    v242 = (int64_t)((uint64_t)v240 - (uint64_t)v241);
    *(int64_t*)v189 = (int64_t)v242;
    goto bb46;
  bb45:
    goto bb31;
  bb46:
    goto bb41;
  bb47:
    v261 = 0LL;
    v262 = *(int64_t*)v17;
    v263 = (int64_t)((uint64_t)v261 - (uint64_t)v262);
    *(int64_t*)v17 = (int64_t)v263;
    goto bb49;
  bb48:
    goto bb49;
  bb49:
    v267 = *(int64_t*)v17;
    return v267;
    return 0;
}

int64_t str_parse_int(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_parse_int((void*)v2, (int64_t)v1);
    return v3;
    return 0;
}

int64_t str_parse_int_range(void* v0, void* v1, int64_t v2, int64_t v3) {
    int64_t v4 = 0;
    int64_t v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0;
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    int64_t v21 = 0;
    bool v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0;
    int64_t v29 = 0;
    bool v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0;
    void* v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0;
    void* v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    int64_t v44 = 0;
    bool v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0;
    int64_t v48 = 0;
    void* v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0;
    void* v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    void* v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    void* v58 = 0;
    uint8_t v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0; /* placeholder for void */
    void* v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0;
    bool v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    int64_t v69 = 0;
    bool v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0; /* placeholder for void */
    bool v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0;
    int64_t v78 = 0;
    int64_t v79 = 0;
    int64_t v80 = 0;
    int64_t v81 = 0;
    int64_t v82 = 0;
    int64_t v83 = 0;
    int64_t v84 = 0; /* placeholder for void */
    int64_t v85 = 0;
    int64_t v86 = 0;
    int64_t v87 = 0;
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0;
    int64_t v91 = 0; /* placeholder for void */
    v6 = alloca(8);
    v8 = alloca(8);
    v10 = alloca(8);
    v36 = alloca(8);
    v39 = alloca(8);
    v49 = alloca(8);
    v52 = alloca(8);
    v55 = alloca(8);
    v62 = alloca(1);
    v4 = 0LL;
    v5 = (int64_t)String_len((void*)v4, (int64_t)v1);
    memset(v6, 0, 8);
    *(int64_t*)v6 = (int64_t)v5;
    memset(v8, 0, 8);
    *(int64_t*)v8 = (int64_t)v2;
    memset(v10, 0, 8);
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
    return v32;
  bb8:
    goto bb9;
  bb9:
    v35 = 0LL;
    memset(v36, 0, 8);
    *(int64_t*)v36 = (int64_t)v35;
    v38 = *(int64_t*)v8;
    memset(v39, 0, 8);
    *(int64_t*)v39 = (int64_t)v38;
    goto bb10;
  bb10:
    p_co_tick();
    v43 = *(int64_t*)v39;
    v44 = *(int64_t*)v10;
    v45 = v43 < v44;
    if (v45) goto bb11;
    else goto bb12;
  bb11:
    v47 = *(int64_t*)v39;
    v48 = (int64_t)v1 + v47;
    memset(v49, 0, 8);
    *(int64_t*)v49 = (int64_t)v48;
    v51 = *(int64_t*)v49;
    memset(v52, 0, 8);
    *(void**)v52 = (void*)v51;
    v54 = 0LL;
    memset(v55, 0, 8);
    *(int64_t*)v55 = (int64_t)v54;
    v58 = *(void**)v52;
    v59 = *(uint8_t*)v58;
    *(int64_t*)v55 = (int64_t)v59;
    memset(v62, 0, 1);
    v63 = *(int64_t*)v55;
    v64 = 48LL;
    v65 = v63 < v64;
    *(bool*)v62 = v65;
    if (v65) goto bb14;
    else goto bb13;
  bb12:
    v90 = *(int64_t*)v36;
    return v90;
  bb13:
    v68 = *(int64_t*)v55;
    v69 = 57LL;
    v70 = v68 > v69;
    *(bool*)v62 = v70;
    goto bb14;
  bb14:
    v73 = *(bool*)v62;
    if (v73) goto bb15;
    else goto bb16;
  bb15:
    goto bb12;
  bb16:
    goto bb17;
  bb17:
    v77 = *(int64_t*)v36;
    v78 = 10LL;
    v79 = p_mul_i64(v77, v78);
    v80 = *(int64_t*)v55;
    v81 = 48LL;
    v82 = p_sub_i64(v80, v81);
    v83 = p_add_i64(v79, v82);
    *(int64_t*)v36 = (int64_t)v83;
    v85 = *(int64_t*)v39;
    v86 = 1LL;
    v87 = (int64_t)((uint64_t)v85 + (uint64_t)v86);
    *(int64_t*)v39 = (int64_t)v87;
    goto bb10;
    return 0;
}

void* str_repeat(void* v0, void* v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)String_repeat((void*)v3, (int64_t)v1, v2);
    return (void*)v4;
    return 0;
}

void* str_replace(void* v0, void* v1, void* v2, void* v3) {
    int64_t v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    v4 = 0LL;
    v5 = (int64_t)String_replace((void*)v4, (int64_t)v1, (int64_t)v2, (int64_t)v3);
    return (void*)v5;
    return 0;
}

int64_t str_rfind(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)String_rfind((void*)v3, (int64_t)v1, (int64_t)v2);
    return v4;
    return 0;
}

void* str_slice(void* v0, void* v1, int64_t v2, int64_t v3) {
    int64_t v4 = 0;
    int64_t v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0;
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    int64_t v21 = 0;
    bool v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0;
    int64_t v29 = 0;
    bool v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    void* v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    v6 = alloca(8);
    v8 = alloca(8);
    v10 = alloca(8);
    v4 = 0LL;
    v5 = (int64_t)String_len((void*)v4, (int64_t)v1);
    memset(v6, 0, 8);
    *(int64_t*)v6 = (int64_t)v5;
    memset(v8, 0, 8);
    *(int64_t*)v8 = (int64_t)v2;
    memset(v10, 0, 8);
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
    return v32;
  bb8:
    goto bb9;
  bb9:
    v35 = *(int64_t*)v8;
    v36 = *(int64_t*)v10;
    v37 = 0LL;
    v38 = (int64_t)String_slice((void*)v37, (int64_t)v1, v35, v36);
    return (void*)v38;
    return 0;
}

void* str_slice_len(void* v0, void* v1, int64_t v2, int64_t v3, int64_t v4) {
    int64_t v5 = 0;
    int64_t v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    v5 = 0LL;
    v6 = (int64_t)String_slice_len((void*)v5, (int64_t)v1, v2, v3, v4);
    return (void*)v6;
    return 0;
}

void* str_split(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    void* v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    int64_t v8 = 0;
    int64_t v9 = 0;
    bool v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    void* v12 = 0;
    int64_t v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    void* v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    int64_t v24 = 0;
    void* v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    int64_t v28 = 0;
    void* v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0;
    void* v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0;
    bool v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    bool v42 = 0;
    void* v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0;
    void* v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0;
    int64_t v51 = 0;
    bool v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0;
    void* v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    int64_t v61 = 0;
    void* v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0;
    void* v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0;
    void* v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0;
    void* v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0;
    void* v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0; /* placeholder for void */
    void* v77 = 0;
    uint8_t v78 = 0;
    int64_t v79 = 0; /* placeholder for void */
    void* v80 = 0;
    uint8_t v81 = 0;
    int64_t v82 = 0; /* placeholder for void */
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0;
    int64_t v85 = 0;
    bool v86 = 0;
    int64_t v87 = 0; /* placeholder for void */
    bool v88 = 0;
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0;
    int64_t v93 = 0;
    int64_t v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    bool v97 = 0;
    int64_t v98 = 0; /* placeholder for void */
    void* v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0;
    int64_t v102 = 0;
    int64_t v103 = 0;
    int64_t v104 = 0;
    int64_t v105 = 0; /* placeholder for void */
    int64_t v106 = 0;
    int64_t v107 = 0;
    int64_t v108 = 0;
    int64_t v109 = 0; /* placeholder for void */
    int64_t v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    int64_t v112 = 0; /* placeholder for void */
    int64_t v113 = 0;
    int64_t v114 = 0;
    int64_t v115 = 0;
    int64_t v116 = 0; /* placeholder for void */
    int64_t v117 = 0; /* placeholder for void */
    int64_t v118 = 0; /* placeholder for void */
    void* v119 = 0;
    int64_t v120 = 0;
    int64_t v121 = 0;
    int64_t v122 = 0;
    int64_t v123 = 0;
    int64_t v124 = 0;
    int64_t v125 = 0; /* placeholder for void */
    void* v126 = 0;
    int64_t v127 = 0;
    void* v128 = 0;
    int64_t v129 = 0; /* placeholder for void */
    v5 = alloca(8);
    v21 = alloca(8);
    v25 = alloca(8);
    v29 = alloca(8);
    v32 = alloca(8);
    v43 = alloca(1);
    v46 = alloca(8);
    v58 = alloca(8);
    v62 = alloca(8);
    v65 = alloca(8);
    v68 = alloca(8);
    v71 = alloca(8);
    v74 = alloca(8);
    v3 = 0LL;
    v4 = (void*)Vec_new((void*)v3);
    memset(v5, 0, 8);
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
    v16 = 0LL;
    v17 = (void*)Vec_share((void*)v16, (int64_t)v15);
    return v17;
  bb2:
    goto bb3;
  bb3:
    v20 = 0LL;
    memset(v21, 0, 8);
    *(int64_t*)v21 = (int64_t)v20;
    v23 = 0LL;
    v24 = (int64_t)String_len((void*)v23, (int64_t)v1);
    memset(v25, 0, 8);
    *(int64_t*)v25 = (int64_t)v24;
    v27 = 0LL;
    v28 = (int64_t)String_len((void*)v27, (int64_t)v2);
    memset(v29, 0, 8);
    *(int64_t*)v29 = (int64_t)v28;
    v31 = 0LL;
    memset(v32, 0, 8);
    *(int64_t*)v32 = (int64_t)v31;
    goto bb4;
  bb4:
    p_co_tick();
    v36 = *(int64_t*)v32;
    v37 = *(int64_t*)v25;
    v38 = *(int64_t*)v29;
    v39 = p_sub_i64(v37, v38);
    v40 = v36 <= v39;
    if (v40) goto bb5;
    else goto bb6;
  bb5:
    v42 = true;
    memset(v43, 0, 1);
    *(bool*)v43 = v42;
    v45 = 0LL;
    memset(v46, 0, 8);
    *(int64_t*)v46 = (int64_t)v45;
    goto bb7;
  bb6:
    v119 = *(void**)v5;
    v120 = *(int64_t*)v21;
    v121 = *(int64_t*)v25;
    v122 = 0LL;
    v123 = (int64_t)String_slice((void*)v122, (int64_t)v1, v120, v121);
    v124 = 0LL;
    Vec_push((void*)v124, (int64_t)v119, v123);
    v126 = *(void**)v5;
    v127 = 0LL;
    v128 = (void*)Vec_share((void*)v127, (int64_t)v126);
    return v128;
  bb7:
    p_co_tick();
    v50 = *(int64_t*)v46;
    v51 = *(int64_t*)v29;
    v52 = v50 < v51;
    if (v52) goto bb8;
    else goto bb9;
  bb8:
    v54 = *(int64_t*)v32;
    v55 = (int64_t)v1 + v54;
    v56 = *(int64_t*)v46;
    v57 = p_add_i64(v55, v56);
    memset(v58, 0, 8);
    *(int64_t*)v58 = (int64_t)v57;
    v60 = *(int64_t*)v46;
    v61 = (int64_t)v2 + v60;
    memset(v62, 0, 8);
    *(int64_t*)v62 = (int64_t)v61;
    v64 = *(int64_t*)v58;
    memset(v65, 0, 8);
    *(void**)v65 = (void*)v64;
    v67 = *(int64_t*)v62;
    memset(v68, 0, 8);
    *(void**)v68 = (void*)v67;
    v70 = 0LL;
    memset(v71, 0, 8);
    *(int64_t*)v71 = (int64_t)v70;
    v73 = 0LL;
    memset(v74, 0, 8);
    *(int64_t*)v74 = (int64_t)v73;
    v77 = *(void**)v65;
    v78 = *(uint8_t*)v77;
    *(int64_t*)v71 = (int64_t)v78;
    v80 = *(void**)v68;
    v81 = *(uint8_t*)v80;
    *(int64_t*)v74 = (int64_t)v81;
    v84 = *(int64_t*)v71;
    v85 = *(int64_t*)v74;
    v86 = v84 != v85;
    if (v86) goto bb10;
    else goto bb11;
  bb9:
    v97 = *(bool*)v43;
    if (v97) goto bb13;
    else goto bb14;
  bb10:
    v88 = false;
    *(bool*)v43 = v88;
    goto bb9;
  bb11:
    goto bb12;
  bb12:
    v92 = *(int64_t*)v46;
    v93 = 1LL;
    v94 = (int64_t)((uint64_t)v92 + (uint64_t)v93);
    *(int64_t*)v46 = (int64_t)v94;
    goto bb7;
  bb13:
    v99 = *(void**)v5;
    v100 = *(int64_t*)v21;
    v101 = *(int64_t*)v32;
    v102 = 0LL;
    v103 = (int64_t)String_slice((void*)v102, (int64_t)v1, v100, v101);
    v104 = 0LL;
    Vec_push((void*)v104, (int64_t)v99, v103);
    v106 = *(int64_t*)v32;
    v107 = *(int64_t*)v29;
    v108 = p_add_i64(v106, v107);
    *(int64_t*)v21 = (int64_t)v108;
    v110 = *(int64_t*)v21;
    *(int64_t*)v32 = (int64_t)v110;
    goto bb15;
  bb14:
    v113 = *(int64_t*)v32;
    v114 = 1LL;
    v115 = p_add_i64(v113, v114);
    *(int64_t*)v32 = (int64_t)v115;
    goto bb15;
  bb15:
    goto bb4;
    return 0;
}

void* str_split_char(void* v0, void* v1, int64_t v2) {
    int64_t v3 = 0;
    void* v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0;
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0;
    int64_t v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    void* v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    int64_t v20 = 0;
    bool v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    int64_t v24 = 0;
    void* v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0;
    void* v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    void* v34 = 0;
    uint8_t v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0;
    bool v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    void* v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    int64_t v49 = 0;
    int64_t v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0; /* placeholder for void */
    void* v59 = 0;
    int64_t v60 = 0;
    int64_t v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    void* v66 = 0;
    int64_t v67 = 0;
    void* v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    v5 = alloca(8);
    v8 = alloca(8);
    v12 = alloca(8);
    v15 = alloca(8);
    v25 = alloca(8);
    v28 = alloca(8);
    v31 = alloca(8);
    v3 = 0LL;
    v4 = (void*)Vec_new((void*)v3);
    memset(v5, 0, 8);
    *(void**)v5 = (void*)v4;
    v7 = 0LL;
    memset(v8, 0, 8);
    *(int64_t*)v8 = (int64_t)v7;
    v10 = 0LL;
    v11 = (int64_t)String_len((void*)v10, (int64_t)v1);
    memset(v12, 0, 8);
    *(int64_t*)v12 = (int64_t)v11;
    v14 = 0LL;
    memset(v15, 0, 8);
    *(int64_t*)v15 = (int64_t)v14;
    goto bb1;
  bb1:
    p_co_tick();
    v19 = *(int64_t*)v15;
    v20 = *(int64_t*)v12;
    v21 = v19 < v20;
    if (v21) goto bb2;
    else goto bb3;
  bb2:
    v23 = *(int64_t*)v15;
    v24 = (int64_t)v1 + v23;
    memset(v25, 0, 8);
    *(int64_t*)v25 = (int64_t)v24;
    v27 = *(int64_t*)v25;
    memset(v28, 0, 8);
    *(void**)v28 = (void*)v27;
    v30 = 0LL;
    memset(v31, 0, 8);
    *(int64_t*)v31 = (int64_t)v30;
    v34 = *(void**)v28;
    v35 = *(uint8_t*)v34;
    *(int64_t*)v31 = (int64_t)v35;
    v38 = *(int64_t*)v31;
    v39 = v38 == v2;
    if (v39) goto bb4;
    else goto bb5;
  bb3:
    v59 = *(void**)v5;
    v60 = *(int64_t*)v8;
    v61 = *(int64_t*)v12;
    v62 = 0LL;
    v63 = (int64_t)String_slice((void*)v62, (int64_t)v1, v60, v61);
    v64 = 0LL;
    Vec_push((void*)v64, (int64_t)v59, v63);
    v66 = *(void**)v5;
    v67 = 0LL;
    v68 = (void*)Vec_share((void*)v67, (int64_t)v66);
    return v68;
  bb4:
    v41 = *(void**)v5;
    v42 = *(int64_t*)v8;
    v43 = *(int64_t*)v15;
    v44 = 0LL;
    v45 = (int64_t)String_slice((void*)v44, (int64_t)v1, v42, v43);
    v46 = 0LL;
    Vec_push((void*)v46, (int64_t)v41, v45);
    v48 = *(int64_t*)v15;
    v49 = 1LL;
    v50 = p_add_i64(v48, v49);
    *(int64_t*)v8 = (int64_t)v50;
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v54 = *(int64_t*)v15;
    v55 = 1LL;
    v56 = (int64_t)((uint64_t)v54 + (uint64_t)v55);
    *(int64_t*)v15 = (int64_t)v56;
    goto bb1;
    return 0;
}

bool str_starts_with(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (bool)String_starts_with((void*)v3, (int64_t)v1, (int64_t)v2);
    return v4;
    return 0;
}

void* str_to_lower(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_to_lowercase((void*)v2, (int64_t)v1);
    return (void*)v3;
    return 0;
}

void* str_to_upper(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_to_uppercase((void*)v2, (int64_t)v1);
    return (void*)v3;
    return 0;
}

void* str_trim(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_trim((void*)v2, (int64_t)v1);
    return (void*)v3;
    return 0;
}

void* str_trim_end(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    int64_t v12 = 0;
    bool v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    void* v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    void* v28 = 0;
    uint8_t v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    int64_t v33 = 0;
    bool v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0;
    int64_t v45 = 0;
    bool v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    void* v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0;
    int64_t v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    v4 = alloca(8);
    v7 = alloca(8);
    v19 = alloca(8);
    v22 = alloca(8);
    v25 = alloca(8);
    v2 = 0LL;
    v3 = (int64_t)String_len((void*)v2, (int64_t)v1);
    memset(v4, 0, 8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    memset(v7, 0, 8);
    *(int64_t*)v7 = (int64_t)v6;
    goto bb1;
  bb1:
    p_co_tick();
    v11 = *(int64_t*)v7;
    v12 = 0LL;
    v13 = v11 > v12;
    if (v13) goto bb2;
    else goto bb3;
  bb2:
    v15 = *(int64_t*)v7;
    v16 = (int64_t)v1 + v15;
    v17 = 1LL;
    v18 = p_sub_i64(v16, v17);
    memset(v19, 0, 8);
    *(int64_t*)v19 = (int64_t)v18;
    v21 = *(int64_t*)v19;
    memset(v22, 0, 8);
    *(void**)v22 = (void*)v21;
    v24 = 0LL;
    memset(v25, 0, 8);
    *(int64_t*)v25 = (int64_t)v24;
    v28 = *(void**)v22;
    v29 = *(uint8_t*)v28;
    *(int64_t*)v25 = (int64_t)v29;
    v32 = *(int64_t*)v25;
    v33 = 0LL;
    v34 = (bool)char_is_whitespace((void*)v33, v32);
    v35 = !v34;
    if (v35) goto bb4;
    else goto bb5;
  bb3:
    v44 = *(int64_t*)v7;
    v45 = 0LL;
    v46 = v44 <= v45;
    if (v46) goto bb7;
    else goto bb8;
  bb4:
    goto bb3;
  bb5:
    goto bb6;
  bb6:
    v39 = *(int64_t*)v7;
    v40 = 1LL;
    v41 = p_sub_i64(v39, v40);
    *(int64_t*)v7 = (int64_t)v41;
    goto bb1;
  bb7:
    v48 = _str0;
    return v48;
  bb8:
    goto bb9;
  bb9:
    v51 = 0LL;
    v52 = *(int64_t*)v7;
    v53 = 0LL;
    v54 = (int64_t)String_slice((void*)v53, (int64_t)v1, v51, v52);
    return (void*)v54;
    return 0;
}

void* str_trim_end_char(void* v0, void* v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    int64_t v10 = 0;
    bool v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0;
    void* v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    void* v20 = 0;
    void* v21 = 0;
    int64_t v22 = 0;
    bool v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    int64_t v27 = 0;
    bool v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0; /* placeholder for void */
    bool v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    int64_t v35 = 0;
    bool v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    bool v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    int64_t v49 = 0;
    bool v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    void* v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0;
    int64_t v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    v5 = alloca(8);
    v18 = alloca(8);
    v20 = alloca(1);
    v21 = alloca(1);
    v3 = 0LL;
    v4 = (int64_t)String_len((void*)v3, (int64_t)v1);
    memset(v5, 0, 8);
    *(int64_t*)v5 = (int64_t)v4;
    goto bb1;
  bb1:
    p_co_tick();
    v9 = *(int64_t*)v5;
    v10 = 0LL;
    v11 = v9 > v10;
    if (v11) goto bb2;
    else goto bb3;
  bb2:
    v13 = *(int64_t*)v5;
    v14 = 1LL;
    v15 = p_sub_i64(v13, v14);
    v16 = 0LL;
    v17 = (int64_t)str_byte_at((void*)v16, (void*)v1, v15);
    memset(v18, 0, 8);
    *(int64_t*)v18 = (int64_t)v17;
    memset(v20, 0, 1);
    memset(v21, 0, 1);
    v22 = *(int64_t*)v18;
    v23 = v22 == v2;
    *(bool*)v21 = v23;
    if (v23) goto bb5;
    else goto bb4;
  bb3:
    v48 = *(int64_t*)v5;
    v49 = 0LL;
    v50 = v48 <= v49;
    if (v50) goto bb11;
    else goto bb12;
  bb4:
    v26 = *(int64_t*)v18;
    v27 = 32LL;
    v28 = v26 == v27;
    *(bool*)v21 = v28;
    goto bb5;
  bb5:
    v31 = *(bool*)v21;
    *(bool*)v20 = v31;
    if (v31) goto bb7;
    else goto bb6;
  bb6:
    v34 = *(int64_t*)v18;
    v35 = 9LL;
    v36 = v34 == v35;
    *(bool*)v20 = v36;
    goto bb7;
  bb7:
    v39 = *(bool*)v20;
    if (v39) goto bb8;
    else goto bb9;
  bb8:
    v41 = *(int64_t*)v5;
    v42 = 1LL;
    v43 = p_sub_i64(v41, v42);
    *(int64_t*)v5 = (int64_t)v43;
    goto bb10;
  bb9:
    goto bb3;
  bb10:
    goto bb1;
  bb11:
    v52 = _str0;
    return v52;
  bb12:
    goto bb13;
  bb13:
    v55 = 0LL;
    v56 = *(int64_t*)v5;
    v57 = 0LL;
    v58 = (int64_t)String_slice((void*)v57, (int64_t)v1, v55, v56);
    return (void*)v58;
    return 0;
}

void* str_trim_start(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    int64_t v12 = 0;
    bool v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    int64_t v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    uint8_t v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0;
    int64_t v31 = 0;
    bool v32 = 0;
    int64_t v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0;
    int64_t v43 = 0;
    bool v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    void* v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    int64_t v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    v4 = alloca(8);
    v7 = alloca(8);
    v17 = alloca(8);
    v20 = alloca(8);
    v23 = alloca(8);
    v2 = 0LL;
    v3 = (int64_t)String_len((void*)v2, (int64_t)v1);
    memset(v4, 0, 8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = 0LL;
    memset(v7, 0, 8);
    *(int64_t*)v7 = (int64_t)v6;
    goto bb1;
  bb1:
    p_co_tick();
    v11 = *(int64_t*)v7;
    v12 = *(int64_t*)v4;
    v13 = v11 < v12;
    if (v13) goto bb2;
    else goto bb3;
  bb2:
    v15 = *(int64_t*)v7;
    v16 = (int64_t)v1 + v15;
    memset(v17, 0, 8);
    *(int64_t*)v17 = (int64_t)v16;
    v19 = *(int64_t*)v17;
    memset(v20, 0, 8);
    *(void**)v20 = (void*)v19;
    v22 = 0LL;
    memset(v23, 0, 8);
    *(int64_t*)v23 = (int64_t)v22;
    v26 = *(void**)v20;
    v27 = *(uint8_t*)v26;
    *(int64_t*)v23 = (int64_t)v27;
    v30 = *(int64_t*)v23;
    v31 = 0LL;
    v32 = (bool)char_is_whitespace((void*)v31, v30);
    v33 = !v32;
    if (v33) goto bb4;
    else goto bb5;
  bb3:
    v42 = *(int64_t*)v7;
    v43 = *(int64_t*)v4;
    v44 = v42 >= v43;
    if (v44) goto bb7;
    else goto bb8;
  bb4:
    goto bb3;
  bb5:
    goto bb6;
  bb6:
    v37 = *(int64_t*)v7;
    v38 = 1LL;
    v39 = (int64_t)((uint64_t)v37 + (uint64_t)v38);
    *(int64_t*)v7 = (int64_t)v39;
    goto bb1;
  bb7:
    v46 = _str0;
    return v46;
  bb8:
    goto bb9;
  bb9:
    v49 = *(int64_t*)v7;
    v50 = *(int64_t*)v4;
    v51 = 0LL;
    v52 = (int64_t)String_slice((void*)v51, (int64_t)v1, v49, v50);
    return (void*)v52;
    return 0;
}

void* u64_to_str(void* v0, uint64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    int64_t v7 = 0;
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    uint64_t v12 = 0;
    int64_t v13 = 0;
    uint64_t v14 = 0;
    void* v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    uint64_t v17 = 0;
    int64_t v18 = 0;
    uint64_t v19 = 0;
    uint64_t v20 = 0;
    void* v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    uint64_t v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0;
    void* v26 = 0;
    uint64_t v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0;
    void* v30 = 0;
    void* v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    v15 = alloca(8);
    v21 = alloca(8);
    v2 = v1;
    v3 = 0LL;
    v4 = v2 >= v3;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    v6 = v1;
    v7 = 0LL;
    v8 = int_to_str((void*)v7, v6);
    return v8;
  bb2:
    goto bb3;
  bb3:
    v11 = 1LL;
    v12 = (uint64_t)((uint64_t)v1 >> (v11 & 63));
    v13 = 5LL;
    v14 = p_div_u64(v12, v13);
    memset(v15, 0, 8);
    *(uint64_t*)v15 = (uint64_t)v14;
    v17 = *(uint64_t*)v15;
    v18 = 10LL;
    v19 = p_mul_u64(v17, v18);
    v20 = p_sub_u64(v1, v19);
    memset(v21, 0, 8);
    *(uint64_t*)v21 = (uint64_t)v20;
    v23 = *(uint64_t*)v15;
    v24 = v23;
    v25 = 0LL;
    v26 = int_to_str((void*)v25, v24);
    v27 = *(uint64_t*)v21;
    v28 = v27;
    v29 = 0LL;
    v30 = int_to_str((void*)v29, v28);
    v31 = p_str_concat((void*)v26, (void*)v30);
    v33 = 0LL;
    Str_drop((void*)v33, (int64_t)v26);
    v35 = 0LL;
    Str_drop((void*)v35, (int64_t)v30);
    return v31;
    return 0;
}

int64_t vec_get_or(void* v0, void* v1, int64_t v2, int64_t v3) {
    int64_t v4 = 0;
    void* v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    int64_t v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    int64_t v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    void* v21 = 0;
    int64_t v22 = 0;
    bool v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    bool v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0; /* placeholder for void */
    bool v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    void* v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    void* v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0; /* placeholder for void */
    void* v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0;
    int64_t v48 = 0;
    void* v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0;
    void* v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0; /* placeholder for void */
    void* v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0; /* placeholder for void */
    int64_t v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    v6 = alloca(8);
    v11 = alloca(8);
    v14 = alloca(8);
    v21 = alloca(1);
    v35 = alloca(8);
    v38 = alloca(8);
    v49 = alloca(8);
    v52 = alloca(8);
    v4 = 0LL;
    v5 = (void*)Vec_share((void*)v4, (int64_t)v1);
    memset(v6, 0, 8);
    *(int64_t*)v6 = (int64_t)v5;
    v8 = *(int64_t*)v6;
    v9 = 8LL;
    v10 = (int64_t)((uint64_t)v8 + (uint64_t)v9);
    memset(v11, 0, 8);
    *(void**)v11 = (void*)v10;
    v13 = 0LL;
    memset(v14, 0, 8);
    *(int64_t*)v14 = (int64_t)v13;
    v17 = *(void**)v11;
    v18 = *(int64_t*)v17;
    *(int64_t*)v14 = (int64_t)v18;
    memset(v21, 0, 1);
    v22 = 0LL;
    v23 = v2 < v22;
    *(bool*)v21 = v23;
    if (v23) goto bb2;
    else goto bb1;
  bb1:
    v26 = *(int64_t*)v14;
    v27 = v2 >= v26;
    *(bool*)v21 = v27;
    goto bb2;
  bb2:
    v30 = *(bool*)v21;
    if (v30) goto bb3;
    else goto bb4;
  bb3:
    return v3;
  bb4:
    goto bb5;
  bb5:
    v34 = *(int64_t*)v6;
    memset(v35, 0, 8);
    *(void**)v35 = (void*)v34;
    v37 = 0LL;
    memset(v38, 0, 8);
    *(int64_t*)v38 = (int64_t)v37;
    v41 = *(void**)v35;
    v42 = *(int64_t*)v41;
    *(int64_t*)v38 = (int64_t)v42;
    v45 = *(int64_t*)v38;
    v46 = 8LL;
    v47 = (int64_t)((uint64_t)v2 * (uint64_t)v46);
    v48 = (int64_t)((uint64_t)v45 + (uint64_t)v47);
    memset(v49, 0, 8);
    *(void**)v49 = (void*)v48;
    v51 = 0LL;
    memset(v52, 0, 8);
    *(int64_t*)v52 = (int64_t)v51;
    v55 = *(void**)v49;
    v56 = *(int64_t*)v55;
    *(int64_t*)v52 = (int64_t)v56;
    v59 = *(int64_t*)v52;
    return v59;
    return 0;
}

void vec_own(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    int64_t v15 = 0;
    bool v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    int64_t v21 = 0;
    int64_t v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0;
    int64_t v29 = 0;
    void* v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    void* v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0;
    void* v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    void* v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    void* v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    int64_t v47 = 0;
    bool v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    int64_t v55 = 0;
    bool v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0;
    void* v65 = 0;
    void* v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    void* v69 = 0;
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0;
    int64_t v74 = 0;
    bool v75 = 0;
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0;
    int64_t v78 = 0;
    int64_t v79 = 0;
    int64_t v80 = 0;
    int64_t v81 = 0;
    void* v82 = 0;
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0;
    int64_t v85 = 0;
    int64_t v86 = 0;
    int64_t v87 = 0;
    int64_t v88 = 0;
    void* v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0; /* placeholder for void */
    void* v92 = 0;
    void* v93 = 0;
    int64_t v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0;
    int64_t v98 = 0;
    int64_t v99 = 0;
    int64_t v100 = 0; /* placeholder for void */
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0; /* placeholder for void */
    void* v103 = 0;
    int64_t v104 = 0;
    int64_t v105 = 0; /* placeholder for void */
    void* v106 = 0;
    int64_t v107 = 0;
    int64_t v108 = 0; /* placeholder for void */
    int64_t v109 = 0; /* placeholder for void */
    int64_t v110 = 0; /* placeholder for void */
    v4 = alloca(8);
    v7 = alloca(8);
    v26 = alloca(8);
    v30 = alloca(8);
    v33 = alloca(8);
    v36 = alloca(8);
    v66 = alloca(8);
    v69 = alloca(8);
    v82 = alloca(8);
    v89 = alloca(8);
    v2 = 16LL;
    v3 = p_add_i64(v1, v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    v6 = 0LL;
    memset(v7, 0, 8);
    *(int64_t*)v7 = (int64_t)v6;
    v10 = *(void**)v4;
    v11 = *(int64_t*)v10;
    *(int64_t*)v7 = (int64_t)v11;
    v14 = *(int64_t*)v7;
    v15 = 0LL;
    v16 = v14 >= v15;
    if (v16) goto bb1;
    else goto bb2;
  bb1:
    return;
  bb2:
    goto bb3;
  bb3:
    v20 = 0LL;
    v21 = *(int64_t*)v7;
    v22 = p_sub_i64(v20, v21);
    v23 = 1LL;
    v24 = p_sub_i64(v22, v23);
    *(int64_t*)v7 = (int64_t)v24;
    memset(v26, 0, 8);
    *(void**)v26 = (void*)v1;
    v28 = 8LL;
    v29 = p_add_i64(v1, v28);
    memset(v30, 0, 8);
    *(void**)v30 = (void*)v29;
    v32 = 0LL;
    memset(v33, 0, 8);
    *(int64_t*)v33 = (int64_t)v32;
    v35 = 0LL;
    memset(v36, 0, 8);
    *(int64_t*)v36 = (int64_t)v35;
    v39 = *(void**)v26;
    v40 = *(int64_t*)v39;
    *(int64_t*)v33 = (int64_t)v40;
    v42 = *(void**)v30;
    v43 = *(int64_t*)v42;
    *(int64_t*)v36 = (int64_t)v43;
    v46 = *(int64_t*)v7;
    v47 = 8LL;
    v48 = v46 < v47;
    if (v48) goto bb4;
    else goto bb5;
  bb4:
    v50 = 8LL;
    *(int64_t*)v7 = (int64_t)v50;
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v54 = *(int64_t*)v7;
    v55 = *(int64_t*)v36;
    v56 = v54 < v55;
    if (v56) goto bb7;
    else goto bb8;
  bb7:
    v58 = *(int64_t*)v36;
    *(int64_t*)v7 = (int64_t)v58;
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    v62 = *(int64_t*)v7;
    v63 = 8LL;
    v64 = p_mul_i64(v62, v63);
    v65 = p_malloc(v64);
    memset(v66, 0, 8);
    *(int64_t*)v66 = (int64_t)v65;
    v68 = 0LL;
    memset(v69, 0, 8);
    *(int64_t*)v69 = (int64_t)v68;
    goto bb10;
  bb10:
    p_co_tick();
    v73 = *(int64_t*)v69;
    v74 = *(int64_t*)v36;
    v75 = v73 < v74;
    if (v75) goto bb11;
    else goto bb12;
  bb11:
    v77 = *(int64_t*)v33;
    v78 = *(int64_t*)v69;
    v79 = 8LL;
    v80 = p_mul_i64(v78, v79);
    v81 = p_add_i64(v77, v80);
    memset(v82, 0, 8);
    *(void**)v82 = (void*)v81;
    v84 = *(int64_t*)v66;
    v85 = *(int64_t*)v69;
    v86 = 8LL;
    v87 = p_mul_i64(v85, v86);
    v88 = p_add_i64(v84, v87);
    memset(v89, 0, 8);
    *(void**)v89 = (void*)v88;
    v92 = *(void**)v89;
    v93 = *(void**)v82;
    v94 = *(int64_t*)v93;
    *(int64_t*)v92 = (int64_t)v94;
    v97 = *(int64_t*)v69;
    v98 = 1LL;
    v99 = (int64_t)((uint64_t)v97 + (uint64_t)v98);
    *(int64_t*)v69 = (int64_t)v99;
    goto bb10;
  bb12:
    v103 = *(void**)v26;
    v104 = *(int64_t*)v66;
    *(int64_t*)v103 = (int64_t)v104;
    v106 = *(void**)v4;
    v107 = *(int64_t*)v7;
    *(int64_t*)v106 = (int64_t)v107;
    return;
    return;
}

void vec_raw_extend(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    int64_t v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    void* v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    void* v19 = 0;
    int64_t v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0;
    int64_t v29 = 0;
    bool v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    int64_t v33 = 0;
    int64_t v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0;
    void* v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    void* v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0;
    void* v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    void* v46 = 0;
    int64_t v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0; /* placeholder for void */
    v7 = alloca(8);
    v9 = alloca(8);
    v11 = alloca(8);
    v17 = alloca(8);
    v24 = alloca(8);
    v37 = alloca(8);
    v40 = alloca(8);
    v43 = alloca(8);
    v3 = 0LL;
    vec_own((void*)v3, v1);
    v5 = 8LL;
    v6 = p_add_i64(v2, v5);
    memset(v7, 0, 8);
    *(void**)v7 = (void*)v6;
    memset(v9, 0, 8);
    *(void**)v9 = (void*)v2;
    memset(v11, 0, 8);
    v13 = *(void**)v7;
    v14 = *(int64_t*)v13;
    *(int64_t*)v11 = (int64_t)v14;
    memset(v17, 0, 8);
    v19 = *(void**)v9;
    v20 = *(int64_t*)v19;
    *(int64_t*)v17 = (int64_t)v20;
    v23 = 0LL;
    memset(v24, 0, 8);
    *(int64_t*)v24 = (int64_t)v23;
    goto bb1;
  bb1:
    p_co_tick();
    v28 = *(int64_t*)v24;
    v29 = *(int64_t*)v11;
    v30 = v28 < v29;
    if (v30) goto bb2;
    else goto bb3;
  bb2:
    v32 = *(int64_t*)v17;
    v33 = *(int64_t*)v24;
    v34 = 8LL;
    v35 = p_mul_i64(v33, v34);
    v36 = p_add_i64(v32, v35);
    memset(v37, 0, 8);
    *(int64_t*)v37 = (int64_t)v36;
    v39 = *(int64_t*)v37;
    memset(v40, 0, 8);
    *(void**)v40 = (void*)v39;
    v42 = 0LL;
    memset(v43, 0, 8);
    *(int64_t*)v43 = (int64_t)v42;
    v46 = *(void**)v40;
    v47 = *(int64_t*)v46;
    *(int64_t*)v43 = (int64_t)v47;
    v50 = *(int64_t*)v43;
    v51 = 0LL;
    Vec_push((void*)v51, v1, v50);
    v53 = *(int64_t*)v24;
    v54 = 1LL;
    v55 = (int64_t)((uint64_t)v53 + (uint64_t)v54);
    *(int64_t*)v24 = (int64_t)v55;
    goto bb1;
  bb3:
    return;
    return;
}

void vec_raw_insert(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    int64_t v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    int64_t v7 = 0;
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    void* v14 = 0;
    int64_t v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0;
    bool v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    void* v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0;
    bool v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    int64_t v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    void* v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0; /* placeholder for void */
    void* v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    void* v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    int64_t v47 = 0;
    int64_t v48 = 0;
    void* v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    bool v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0;
    int64_t v57 = 0;
    int64_t v58 = 0;
    int64_t v59 = 0;
    int64_t v60 = 0;
    void* v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    int64_t v63 = 0;
    int64_t v64 = 0;
    int64_t v65 = 0;
    int64_t v66 = 0;
    int64_t v67 = 0;
    int64_t v68 = 0;
    int64_t v69 = 0;
    void* v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0;
    void* v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0;
    void* v76 = 0;
    int64_t v77 = 0; /* placeholder for void */
    int64_t v78 = 0; /* placeholder for void */
    void* v79 = 0;
    int64_t v80 = 0;
    void* v81 = 0;
    int64_t v82 = 0; /* placeholder for void */
    void* v83 = 0;
    int64_t v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0; /* placeholder for void */
    int64_t v87 = 0;
    int64_t v88 = 0;
    int64_t v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0;
    int64_t v93 = 0;
    int64_t v94 = 0;
    int64_t v95 = 0;
    void* v96 = 0;
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0;
    void* v99 = 0;
    int64_t v100 = 0; /* placeholder for void */
    int64_t v101 = 0; /* placeholder for void */
    void* v102 = 0;
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0; /* placeholder for void */
    int64_t v105 = 0; /* placeholder for void */
    v8 = alloca(8);
    v10 = alloca(8);
    v12 = alloca(8);
    v40 = alloca(8);
    v49 = alloca(8);
    v61 = alloca(8);
    v70 = alloca(8);
    v73 = alloca(8);
    v76 = alloca(8);
    v81 = alloca(8);
    v96 = alloca(8);
    v99 = alloca(8);
    v4 = 0LL;
    vec_own((void*)v4, v1);
    v6 = 8LL;
    v7 = p_add_i64(v1, v6);
    memset(v8, 0, 8);
    *(void**)v8 = (void*)v7;
    memset(v10, 0, 8);
    *(void**)v10 = (void*)v1;
    memset(v12, 0, 8);
    v14 = *(void**)v8;
    v15 = *(int64_t*)v14;
    *(int64_t*)v12 = (int64_t)v15;
    v18 = 0LL;
    v19 = v2 < v18;
    if (v19) goto bb1;
    else goto bb2;
  bb1:
    v21 = _str14;
    p_panic((void*)v21);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v25 = *(int64_t*)v12;
    v26 = v2 > v25;
    if (v26) goto bb4;
    else goto bb5;
  bb4:
    v28 = _str14;
    p_panic((void*)v28);
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v32 = 0LL;
    v33 = 0LL;
    Vec_push((void*)v33, v1, v32);
    v36 = *(void**)v8;
    v37 = *(int64_t*)v36;
    *(int64_t*)v12 = (int64_t)v37;
    memset(v40, 0, 8);
    v42 = *(void**)v10;
    v43 = *(int64_t*)v42;
    *(int64_t*)v40 = (int64_t)v43;
    v46 = *(int64_t*)v12;
    v47 = 1LL;
    v48 = p_sub_i64(v46, v47);
    memset(v49, 0, 8);
    *(int64_t*)v49 = (int64_t)v48;
    goto bb7;
  bb7:
    p_co_tick();
    v53 = *(int64_t*)v49;
    v54 = v53 > v2;
    if (v54) goto bb8;
    else goto bb9;
  bb8:
    v56 = *(int64_t*)v40;
    v57 = *(int64_t*)v49;
    v58 = 8LL;
    v59 = p_mul_i64(v57, v58);
    v60 = p_add_i64(v56, v59);
    memset(v61, 0, 8);
    *(int64_t*)v61 = (int64_t)v60;
    v63 = *(int64_t*)v40;
    v64 = *(int64_t*)v49;
    v65 = 1LL;
    v66 = p_sub_i64(v64, v65);
    v67 = 8LL;
    v68 = p_mul_i64(v66, v67);
    v69 = p_add_i64(v63, v68);
    memset(v70, 0, 8);
    *(int64_t*)v70 = (int64_t)v69;
    v72 = *(int64_t*)v61;
    memset(v73, 0, 8);
    *(void**)v73 = (void*)v72;
    v75 = *(int64_t*)v70;
    memset(v76, 0, 8);
    *(void**)v76 = (void*)v75;
    v79 = *(void**)v76;
    v80 = *(int64_t*)v79;
    memset(v81, 0, 8);
    *(int64_t*)v81 = (int64_t)v80;
    v83 = *(void**)v73;
    v84 = *(int64_t*)v81;
    *(int64_t*)v83 = (int64_t)v84;
    v87 = *(int64_t*)v49;
    v88 = 1LL;
    v89 = p_sub_i64(v87, v88);
    *(int64_t*)v49 = (int64_t)v89;
    goto bb7;
  bb9:
    v92 = *(int64_t*)v40;
    v93 = 8LL;
    v94 = p_mul_i64(v2, v93);
    v95 = p_add_i64(v92, v94);
    memset(v96, 0, 8);
    *(int64_t*)v96 = (int64_t)v95;
    v98 = *(int64_t*)v96;
    memset(v99, 0, 8);
    *(void**)v99 = (void*)v98;
    v102 = *(void**)v99;
    *(int64_t*)v102 = (int64_t)v3;
    return;
    return;
}

int64_t vec_raw_remove(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    int64_t v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    void* v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    bool v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    bool v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    void* v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0; /* placeholder for void */
    void* v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    void* v33 = 0;
    int64_t v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    void* v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    void* v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    void* v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    void* v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    int64_t v59 = 0;
    int64_t v60 = 0;
    int64_t v61 = 0;
    bool v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0;
    int64_t v65 = 0;
    int64_t v66 = 0;
    int64_t v67 = 0;
    int64_t v68 = 0;
    void* v69 = 0;
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0;
    int64_t v72 = 0;
    int64_t v73 = 0;
    int64_t v74 = 0;
    int64_t v75 = 0;
    int64_t v76 = 0;
    int64_t v77 = 0;
    void* v78 = 0;
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0;
    void* v81 = 0;
    int64_t v82 = 0; /* placeholder for void */
    int64_t v83 = 0;
    void* v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0; /* placeholder for void */
    void* v87 = 0;
    int64_t v88 = 0;
    void* v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    void* v91 = 0;
    int64_t v92 = 0;
    int64_t v93 = 0; /* placeholder for void */
    int64_t v94 = 0; /* placeholder for void */
    int64_t v95 = 0;
    int64_t v96 = 0;
    int64_t v97 = 0;
    int64_t v98 = 0; /* placeholder for void */
    int64_t v99 = 0; /* placeholder for void */
    int64_t v100 = 0;
    int64_t v101 = 0;
    int64_t v102 = 0;
    void* v103 = 0;
    int64_t v104 = 0; /* placeholder for void */
    int64_t v105 = 0; /* placeholder for void */
    void* v106 = 0;
    int64_t v107 = 0;
    int64_t v108 = 0; /* placeholder for void */
    int64_t v109 = 0; /* placeholder for void */
    int64_t v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    v7 = alloca(8);
    v9 = alloca(8);
    v11 = alloca(8);
    v31 = alloca(8);
    v41 = alloca(8);
    v44 = alloca(8);
    v47 = alloca(8);
    v54 = alloca(8);
    v69 = alloca(8);
    v78 = alloca(8);
    v81 = alloca(8);
    v84 = alloca(8);
    v89 = alloca(8);
    v103 = alloca(8);
    v3 = 0LL;
    vec_own((void*)v3, v1);
    v5 = 8LL;
    v6 = p_add_i64(v1, v5);
    memset(v7, 0, 8);
    *(void**)v7 = (void*)v6;
    memset(v9, 0, 8);
    *(void**)v9 = (void*)v1;
    memset(v11, 0, 8);
    v13 = *(void**)v7;
    v14 = *(int64_t*)v13;
    *(int64_t*)v11 = (int64_t)v14;
    v17 = 0LL;
    v18 = v2 < v17;
    if (v18) goto bb1;
    else goto bb2;
  bb1:
    v20 = _str15;
    p_panic((void*)v20);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v24 = *(int64_t*)v11;
    v25 = v2 >= v24;
    if (v25) goto bb4;
    else goto bb5;
  bb4:
    v27 = _str15;
    p_panic((void*)v27);
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    memset(v31, 0, 8);
    v33 = *(void**)v9;
    v34 = *(int64_t*)v33;
    *(int64_t*)v31 = (int64_t)v34;
    v37 = *(int64_t*)v31;
    v38 = 8LL;
    v39 = p_mul_i64(v2, v38);
    v40 = p_add_i64(v37, v39);
    memset(v41, 0, 8);
    *(int64_t*)v41 = (int64_t)v40;
    v43 = *(int64_t*)v41;
    memset(v44, 0, 8);
    *(void**)v44 = (void*)v43;
    v46 = 0LL;
    memset(v47, 0, 8);
    *(int64_t*)v47 = (int64_t)v46;
    v50 = *(void**)v44;
    v51 = *(int64_t*)v50;
    *(int64_t*)v47 = (int64_t)v51;
    memset(v54, 0, 8);
    *(int64_t*)v54 = (int64_t)v2;
    goto bb7;
  bb7:
    p_co_tick();
    v58 = *(int64_t*)v54;
    v59 = *(int64_t*)v11;
    v60 = 1LL;
    v61 = p_sub_i64(v59, v60);
    v62 = v58 < v61;
    if (v62) goto bb8;
    else goto bb9;
  bb8:
    v64 = *(int64_t*)v31;
    v65 = *(int64_t*)v54;
    v66 = 8LL;
    v67 = p_mul_i64(v65, v66);
    v68 = p_add_i64(v64, v67);
    memset(v69, 0, 8);
    *(int64_t*)v69 = (int64_t)v68;
    v71 = *(int64_t*)v31;
    v72 = *(int64_t*)v54;
    v73 = 1LL;
    v74 = p_add_i64(v72, v73);
    v75 = 8LL;
    v76 = p_mul_i64(v74, v75);
    v77 = p_add_i64(v71, v76);
    memset(v78, 0, 8);
    *(int64_t*)v78 = (int64_t)v77;
    v80 = *(int64_t*)v69;
    memset(v81, 0, 8);
    *(void**)v81 = (void*)v80;
    v83 = *(int64_t*)v78;
    memset(v84, 0, 8);
    *(void**)v84 = (void*)v83;
    v87 = *(void**)v84;
    v88 = *(int64_t*)v87;
    memset(v89, 0, 8);
    *(int64_t*)v89 = (int64_t)v88;
    v91 = *(void**)v81;
    v92 = *(int64_t*)v89;
    *(int64_t*)v91 = (int64_t)v92;
    v95 = *(int64_t*)v54;
    v96 = 1LL;
    v97 = (int64_t)((uint64_t)v95 + (uint64_t)v96);
    *(int64_t*)v54 = (int64_t)v97;
    goto bb7;
  bb9:
    v100 = *(int64_t*)v11;
    v101 = 1LL;
    v102 = p_sub_i64(v100, v101);
    memset(v103, 0, 8);
    *(int64_t*)v103 = (int64_t)v102;
    v106 = *(void**)v7;
    v107 = *(int64_t*)v103;
    *(int64_t*)v106 = (int64_t)v107;
    v110 = *(int64_t*)v47;
    return v110;
    return 0;
}

void vec_raw_reverse(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    int64_t v4 = 0;
    int64_t v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    void* v12 = 0;
    int64_t v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0; /* placeholder for void */
    void* v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0;
    int64_t v30 = 0;
    bool v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0;
    int64_t v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0;
    void* v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0;
    int64_t v48 = 0;
    void* v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0;
    void* v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    void* v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    void* v58 = 0;
    int64_t v59 = 0;
    void* v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    void* v62 = 0;
    int64_t v63 = 0;
    void* v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    void* v66 = 0;
    int64_t v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    void* v69 = 0;
    int64_t v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0;
    int64_t v74 = 0;
    int64_t v75 = 0;
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0; /* placeholder for void */
    int64_t v78 = 0; /* placeholder for void */
    v6 = alloca(8);
    v8 = alloca(8);
    v10 = alloca(8);
    v16 = alloca(8);
    v23 = alloca(8);
    v38 = alloca(8);
    v49 = alloca(8);
    v52 = alloca(8);
    v55 = alloca(8);
    v60 = alloca(8);
    v64 = alloca(8);
    v2 = 0LL;
    vec_own((void*)v2, v1);
    v4 = 8LL;
    v5 = p_add_i64(v1, v4);
    memset(v6, 0, 8);
    *(void**)v6 = (void*)v5;
    memset(v8, 0, 8);
    *(void**)v8 = (void*)v1;
    memset(v10, 0, 8);
    v12 = *(void**)v6;
    v13 = *(int64_t*)v12;
    *(int64_t*)v10 = (int64_t)v13;
    memset(v16, 0, 8);
    v18 = *(void**)v8;
    v19 = *(int64_t*)v18;
    *(int64_t*)v16 = (int64_t)v19;
    v22 = 0LL;
    memset(v23, 0, 8);
    *(int64_t*)v23 = (int64_t)v22;
    goto bb1;
  bb1:
    p_co_tick();
    v27 = *(int64_t*)v23;
    v28 = *(int64_t*)v10;
    v29 = 2LL;
    v30 = p_div_i64(v28, v29);
    v31 = v27 < v30;
    if (v31) goto bb2;
    else goto bb3;
  bb2:
    v33 = *(int64_t*)v16;
    v34 = *(int64_t*)v23;
    v35 = 8LL;
    v36 = p_mul_i64(v34, v35);
    v37 = p_add_i64(v33, v36);
    memset(v38, 0, 8);
    *(int64_t*)v38 = (int64_t)v37;
    v40 = *(int64_t*)v16;
    v41 = *(int64_t*)v10;
    v42 = 1LL;
    v43 = p_sub_i64(v41, v42);
    v44 = *(int64_t*)v23;
    v45 = p_sub_i64(v43, v44);
    v46 = 8LL;
    v47 = p_mul_i64(v45, v46);
    v48 = p_add_i64(v40, v47);
    memset(v49, 0, 8);
    *(int64_t*)v49 = (int64_t)v48;
    v51 = *(int64_t*)v38;
    memset(v52, 0, 8);
    *(void**)v52 = (void*)v51;
    v54 = *(int64_t*)v49;
    memset(v55, 0, 8);
    *(void**)v55 = (void*)v54;
    v58 = *(void**)v52;
    v59 = *(int64_t*)v58;
    memset(v60, 0, 8);
    *(int64_t*)v60 = (int64_t)v59;
    v62 = *(void**)v55;
    v63 = *(int64_t*)v62;
    memset(v64, 0, 8);
    *(int64_t*)v64 = (int64_t)v63;
    v66 = *(void**)v52;
    v67 = *(int64_t*)v64;
    *(int64_t*)v66 = (int64_t)v67;
    v69 = *(void**)v55;
    v70 = *(int64_t*)v60;
    *(int64_t*)v69 = (int64_t)v70;
    v73 = *(int64_t*)v23;
    v74 = 1LL;
    v75 = (int64_t)((uint64_t)v73 + (uint64_t)v74);
    *(int64_t*)v23 = (int64_t)v75;
    goto bb1;
  bb3:
    return;
    return;
}

void vec_raw_sort(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    int64_t v4 = 0;
    int64_t v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    void* v12 = 0;
    int64_t v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0; /* placeholder for void */
    void* v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    int64_t v23 = 0;
    bool v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0;
    int64_t v29 = 0;
    int64_t v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0;
    void* v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    int64_t v38 = 0;
    bool v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    int64_t v47 = 0;
    int64_t v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0;
    int64_t v52 = 0;
    int64_t v53 = 0;
    void* v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    int64_t v59 = 0;
    bool v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0;
    int64_t v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0;
    int64_t v68 = 0;
    int64_t v69 = 0;
    int64_t v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0;
    int64_t v73 = 0;
    int64_t v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0; /* placeholder for void */
    v6 = alloca(8);
    v8 = alloca(8);
    v10 = alloca(8);
    v16 = alloca(8);
    v33 = alloca(8);
    v54 = alloca(8);
    v2 = 0LL;
    vec_own((void*)v2, v1);
    v4 = 8LL;
    v5 = p_add_i64(v1, v4);
    memset(v6, 0, 8);
    *(void**)v6 = (void*)v5;
    memset(v8, 0, 8);
    *(void**)v8 = (void*)v1;
    memset(v10, 0, 8);
    v12 = *(void**)v6;
    v13 = *(int64_t*)v12;
    *(int64_t*)v10 = (int64_t)v13;
    memset(v16, 0, 8);
    v18 = *(void**)v8;
    v19 = *(int64_t*)v18;
    *(int64_t*)v16 = (int64_t)v19;
    v22 = *(int64_t*)v10;
    v23 = 1LL;
    v24 = v22 <= v23;
    if (v24) goto bb1;
    else goto bb2;
  bb1:
    return;
  bb2:
    goto bb3;
  bb3:
    v28 = *(int64_t*)v10;
    v29 = 2LL;
    v30 = p_div_i64(v28, v29);
    v31 = 1LL;
    v32 = p_sub_i64(v30, v31);
    memset(v33, 0, 8);
    *(int64_t*)v33 = (int64_t)v32;
    goto bb4;
  bb4:
    p_co_tick();
    v37 = *(int64_t*)v33;
    v38 = 0LL;
    v39 = v37 >= v38;
    if (v39) goto bb5;
    else goto bb6;
  bb5:
    v41 = *(int64_t*)v16;
    v42 = *(int64_t*)v33;
    v43 = *(int64_t*)v10;
    v44 = 0LL;
    vec_sift_down((void*)v44, v41, v42, v43);
    v46 = *(int64_t*)v33;
    v47 = 1LL;
    v48 = p_sub_i64(v46, v47);
    *(int64_t*)v33 = (int64_t)v48;
    goto bb4;
  bb6:
    v51 = *(int64_t*)v10;
    v52 = 1LL;
    v53 = p_sub_i64(v51, v52);
    memset(v54, 0, 8);
    *(int64_t*)v54 = (int64_t)v53;
    goto bb7;
  bb7:
    p_co_tick();
    v58 = *(int64_t*)v54;
    v59 = 0LL;
    v60 = v58 > v59;
    if (v60) goto bb8;
    else goto bb9;
  bb8:
    v62 = *(int64_t*)v16;
    v63 = 0LL;
    v64 = *(int64_t*)v54;
    v65 = 0LL;
    vec_swap_slots((void*)v65, v62, v63, v64);
    v67 = *(int64_t*)v16;
    v68 = 0LL;
    v69 = *(int64_t*)v54;
    v70 = 0LL;
    vec_sift_down((void*)v70, v67, v68, v69);
    v72 = *(int64_t*)v54;
    v73 = 1LL;
    v74 = p_sub_i64(v72, v73);
    *(int64_t*)v54 = (int64_t)v74;
    goto bb7;
  bb9:
    return;
    return;
}

void vec_sift_down(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    bool v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
    bool v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    int64_t v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    bool v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    bool v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    void* v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0;
    int64_t v30 = 0;
    bool v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0;
    bool v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0; /* placeholder for void */
    bool v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0;
    int64_t v48 = 0;
    int64_t v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0;
    int64_t v58 = 0;
    bool v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    bool v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0;
    int64_t v65 = 0;
    int64_t v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0; /* placeholder for void */
    v4 = alloca(8);
    v7 = alloca(1);
    v19 = alloca(8);
    v27 = alloca(1);
    memset(v4, 0, 8);
    *(int64_t*)v4 = (int64_t)v2;
    v6 = false;
    memset(v7, 0, 1);
    *(bool*)v7 = v6;
    goto bb1;
  bb1:
    p_co_tick();
    v11 = *(bool*)v7;
    v12 = !v11;
    if (v12) goto bb2;
    else goto bb3;
  bb2:
    v14 = 2LL;
    v15 = *(int64_t*)v4;
    v16 = p_mul_i64(v14, v15);
    v17 = 1LL;
    v18 = p_add_i64(v16, v17);
    memset(v19, 0, 8);
    *(int64_t*)v19 = (int64_t)v18;
    v21 = *(int64_t*)v19;
    v22 = v21 >= v3;
    if (v22) goto bb4;
    else goto bb5;
  bb3:
    return;
  bb4:
    v24 = true;
    *(bool*)v7 = v24;
    goto bb6;
  bb5:
    memset(v27, 0, 1);
    v28 = *(int64_t*)v19;
    v29 = 1LL;
    v30 = p_add_i64(v28, v29);
    v31 = v30 < v3;
    *(bool*)v27 = v31;
    if (v31) goto bb7;
    else goto bb8;
  bb6:
    goto bb1;
  bb7:
    v34 = *(int64_t*)v19;
    v35 = 0LL;
    v36 = (int64_t)vec_slot((void*)v35, v1, v34);
    v37 = *(int64_t*)v19;
    v38 = 1LL;
    v39 = p_add_i64(v37, v38);
    v40 = 0LL;
    v41 = (int64_t)vec_slot((void*)v40, v1, v39);
    v42 = v36 < v41;
    *(bool*)v27 = v42;
    goto bb8;
  bb8:
    v45 = *(bool*)v27;
    if (v45) goto bb9;
    else goto bb10;
  bb9:
    v47 = *(int64_t*)v19;
    v48 = 1LL;
    v49 = p_add_i64(v47, v48);
    *(int64_t*)v19 = (int64_t)v49;
    goto bb11;
  bb10:
    goto bb11;
  bb11:
    v53 = *(int64_t*)v4;
    v54 = 0LL;
    v55 = (int64_t)vec_slot((void*)v54, v1, v53);
    v56 = *(int64_t*)v19;
    v57 = 0LL;
    v58 = (int64_t)vec_slot((void*)v57, v1, v56);
    v59 = v55 >= v58;
    if (v59) goto bb12;
    else goto bb13;
  bb12:
    v61 = true;
    *(bool*)v7 = v61;
    goto bb14;
  bb13:
    v64 = *(int64_t*)v4;
    v65 = *(int64_t*)v19;
    v66 = 0LL;
    vec_swap_slots((void*)v66, v1, v64, v65);
    v68 = *(int64_t*)v19;
    *(int64_t*)v4 = (int64_t)v68;
    goto bb14;
  bb14:
    goto bb6;
    return;
}

int64_t vec_slot(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0; /* placeholder for void */
    void* v12 = 0;
    int64_t v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    v6 = alloca(8);
    v9 = alloca(8);
    v3 = 8LL;
    v4 = p_mul_i64(v2, v3);
    v5 = p_add_i64(v1, v4);
    memset(v6, 0, 8);
    *(void**)v6 = (void*)v5;
    v8 = 0LL;
    memset(v9, 0, 8);
    *(int64_t*)v9 = (int64_t)v8;
    v12 = *(void**)v6;
    v13 = *(int64_t*)v12;
    *(int64_t*)v9 = (int64_t)v13;
    v16 = *(int64_t*)v9;
    return v16;
    return 0;
}

void vec_swap_slots(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    int64_t v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    int64_t v10 = 0;
    int64_t v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    void* v19 = 0;
    void* v20 = 0;
    int64_t v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    void* v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    v7 = alloca(8);
    v12 = alloca(8);
    v17 = alloca(8);
    v4 = 8LL;
    v5 = p_mul_i64(v2, v4);
    v6 = p_add_i64(v1, v5);
    memset(v7, 0, 8);
    *(void**)v7 = (void*)v6;
    v9 = 8LL;
    v10 = p_mul_i64(v3, v9);
    v11 = p_add_i64(v1, v10);
    memset(v12, 0, 8);
    *(void**)v12 = (void*)v11;
    v15 = *(void**)v7;
    v16 = *(int64_t*)v15;
    memset(v17, 0, 8);
    *(int64_t*)v17 = (int64_t)v16;
    v19 = *(void**)v7;
    v20 = *(void**)v12;
    v21 = *(int64_t*)v20;
    *(int64_t*)v19 = (int64_t)v21;
    v23 = *(void**)v12;
    v24 = *(int64_t*)v17;
    *(int64_t*)v23 = (int64_t)v24;
    return;
    return;
}

// ─── C entry point ───
#ifdef _WIN32
#include <windows.h>
#endif
void __p_install_crash_handlers(void);
int real_main(int argc, char** argv) {
    __p_install_crash_handlers();
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
#ifdef _WIN32
    _setmode(_fileno(stdout), _O_BINARY);
    _setmode(_fileno(stderr), _O_BINARY);
#endif
    __p_argc = argc; __p_argv = argv;
    // Register functions for p_thread_spawn lookup
    p_thread_register("ByteBuffer_clear", (int64_t(*)())ByteBuffer_clear);
    p_thread_register("ByteBuffer_from_string", (int64_t(*)())ByteBuffer_from_string);
    p_thread_register("ByteBuffer_is_empty", (int64_t(*)())ByteBuffer_is_empty);
    p_thread_register("ByteBuffer_len", (int64_t(*)())ByteBuffer_len);
    p_thread_register("ByteBuffer_new", (int64_t(*)())ByteBuffer_new);
    p_thread_register("ByteBuffer_read_byte", (int64_t(*)())ByteBuffer_read_byte);
    p_thread_register("ByteBuffer_set_byte", (int64_t(*)())ByteBuffer_set_byte);
    p_thread_register("ByteBuffer_to_str", (int64_t(*)())ByteBuffer_to_str);
    p_thread_register("ByteBuffer_write_byte", (int64_t(*)())ByteBuffer_write_byte);
    p_thread_register("ByteBuffer_write_bytes", (int64_t(*)())ByteBuffer_write_bytes);
    p_thread_register("HashMap_clear", (int64_t(*)())HashMap_clear);
    p_thread_register("HashMap_clone", (int64_t(*)())HashMap_clone);
    p_thread_register("HashMap_contains", (int64_t(*)())HashMap_contains);
    p_thread_register("HashMap_contains_i", (int64_t(*)())HashMap_contains_i);
    p_thread_register("HashMap_drop", (int64_t(*)())HashMap_drop);
    p_thread_register("HashMap_find_slot", (int64_t(*)())HashMap_find_slot);
    p_thread_register("HashMap_get", (int64_t(*)())HashMap_get);
    p_thread_register("HashMap_get_i", (int64_t(*)())HashMap_get_i);
    p_thread_register("HashMap_get_or", (int64_t(*)())HashMap_get_or);
    p_thread_register("HashMap_get_or_i", (int64_t(*)())HashMap_get_or_i);
    p_thread_register("HashMap_grow", (int64_t(*)())HashMap_grow);
    p_thread_register("HashMap_insert", (int64_t(*)())HashMap_insert);
    p_thread_register("HashMap_insert_i", (int64_t(*)())HashMap_insert_i);
    p_thread_register("HashMap_is_empty", (int64_t(*)())HashMap_is_empty);
    p_thread_register("HashMap_keys", (int64_t(*)())HashMap_keys);
    p_thread_register("HashMap_keys_i", (int64_t(*)())HashMap_keys_i);
    p_thread_register("HashMap_len", (int64_t(*)())HashMap_len);
    p_thread_register("HashMap_new", (int64_t(*)())HashMap_new);
    p_thread_register("HashMap_remove", (int64_t(*)())HashMap_remove);
    p_thread_register("HashMap_remove_i", (int64_t(*)())HashMap_remove_i);
    p_thread_register("HashMap_share", (int64_t(*)())HashMap_share);
    p_thread_register("HashMap_to_str", (int64_t(*)())HashMap_to_str);
    p_thread_register("HashMap_values", (int64_t(*)())HashMap_values);
    p_thread_register("HashMap_values_i", (int64_t(*)())HashMap_values_i);
    p_thread_register("Option_and_then", (int64_t(*)())Option_and_then);
    p_thread_register("Option_context", (int64_t(*)())Option_context);
    p_thread_register("Option_expect", (int64_t(*)())Option_expect);
    p_thread_register("Option_is_none", (int64_t(*)())Option_is_none);
    p_thread_register("Option_is_some", (int64_t(*)())Option_is_some);
    p_thread_register("Option_map", (int64_t(*)())Option_map);
    p_thread_register("Option_or_else", (int64_t(*)())Option_or_else);
    p_thread_register("Option_unwrap", (int64_t(*)())Option_unwrap);
    p_thread_register("Option_unwrap_or", (int64_t(*)())Option_unwrap_or);
    p_thread_register("Option_unwrap_or_else", (int64_t(*)())Option_unwrap_or_else);
    p_thread_register("Result_and_then", (int64_t(*)())Result_and_then);
    p_thread_register("Result_context", (int64_t(*)())Result_context);
    p_thread_register("Result_err", (int64_t(*)())Result_err);
    p_thread_register("Result_expect", (int64_t(*)())Result_expect);
    p_thread_register("Result_is_err", (int64_t(*)())Result_is_err);
    p_thread_register("Result_is_ok", (int64_t(*)())Result_is_ok);
    p_thread_register("Result_map", (int64_t(*)())Result_map);
    p_thread_register("Result_map_err", (int64_t(*)())Result_map_err);
    p_thread_register("Result_ok", (int64_t(*)())Result_ok);
    p_thread_register("Result_unwrap", (int64_t(*)())Result_unwrap);
    p_thread_register("Result_unwrap_or", (int64_t(*)())Result_unwrap_or);
    p_thread_register("Result_unwrap_or_else", (int64_t(*)())Result_unwrap_or_else);
    p_thread_register("Result_with_context", (int64_t(*)())Result_with_context);
    p_thread_register("Str_drop", (int64_t(*)())Str_drop);
    p_thread_register("StringBuilder_append_char", (int64_t(*)())StringBuilder_append_char);
    p_thread_register("StringBuilder_append_int", (int64_t(*)())StringBuilder_append_int);
    p_thread_register("StringBuilder_append_str", (int64_t(*)())StringBuilder_append_str);
    p_thread_register("StringBuilder_clear", (int64_t(*)())StringBuilder_clear);
    p_thread_register("StringBuilder_is_empty", (int64_t(*)())StringBuilder_is_empty);
    p_thread_register("StringBuilder_len", (int64_t(*)())StringBuilder_len);
    p_thread_register("StringBuilder_new", (int64_t(*)())StringBuilder_new);
    p_thread_register("StringBuilder_to_str", (int64_t(*)())StringBuilder_to_str);
    p_thread_register("String_byte_at", (int64_t(*)())String_byte_at);
    p_thread_register("String_bytes", (int64_t(*)())String_bytes);
    p_thread_register("String_char_at", (int64_t(*)())String_char_at);
    p_thread_register("String_char_count", (int64_t(*)())String_char_count);
    p_thread_register("String_clone", (int64_t(*)())String_clone);
    p_thread_register("String_contains", (int64_t(*)())String_contains);
    p_thread_register("String_ends_with", (int64_t(*)())String_ends_with);
    p_thread_register("String_find", (int64_t(*)())String_find);
    p_thread_register("String_from", (int64_t(*)())String_from);
    p_thread_register("String_is_empty", (int64_t(*)())String_is_empty);
    p_thread_register("String_len", (int64_t(*)())String_len);
    p_thread_register("String_parse_int", (int64_t(*)())String_parse_int);
    p_thread_register("String_push_str", (int64_t(*)())String_push_str);
    p_thread_register("String_repeat", (int64_t(*)())String_repeat);
    p_thread_register("String_replace", (int64_t(*)())String_replace);
    p_thread_register("String_rfind", (int64_t(*)())String_rfind);
    p_thread_register("String_slice", (int64_t(*)())String_slice);
    p_thread_register("String_slice_len", (int64_t(*)())String_slice_len);
    p_thread_register("String_split", (int64_t(*)())String_split);
    p_thread_register("String_starts_with", (int64_t(*)())String_starts_with);
    p_thread_register("String_to_lowercase", (int64_t(*)())String_to_lowercase);
    p_thread_register("String_to_string", (int64_t(*)())String_to_string);
    p_thread_register("String_to_uppercase", (int64_t(*)())String_to_uppercase);
    p_thread_register("String_trim", (int64_t(*)())String_trim);
    p_thread_register("Vec_capacity", (int64_t(*)())Vec_capacity);
    p_thread_register("Vec_clear", (int64_t(*)())Vec_clear);
    p_thread_register("Vec_clone", (int64_t(*)())Vec_clone);
    p_thread_register("Vec_contains", (int64_t(*)())Vec_contains);
    p_thread_register("Vec_drop", (int64_t(*)())Vec_drop);
    p_thread_register("Vec_drop_strs", (int64_t(*)())Vec_drop_strs);
    p_thread_register("Vec_drop_vecs", (int64_t(*)())Vec_drop_vecs);
    p_thread_register("Vec_eq_strs", (int64_t(*)())Vec_eq_strs);
    p_thread_register("Vec_eq_words", (int64_t(*)())Vec_eq_words);
    p_thread_register("Vec_extend", (int64_t(*)())Vec_extend);
    p_thread_register("Vec_first", (int64_t(*)())Vec_first);
    p_thread_register("Vec_get", (int64_t(*)())Vec_get);
    p_thread_register("Vec_insert", (int64_t(*)())Vec_insert);
    p_thread_register("Vec_is_empty", (int64_t(*)())Vec_is_empty);
    p_thread_register("Vec_last", (int64_t(*)())Vec_last);
    p_thread_register("Vec_len", (int64_t(*)())Vec_len);
    p_thread_register("Vec_new", (int64_t(*)())Vec_new);
    p_thread_register("Vec_pop", (int64_t(*)())Vec_pop);
    p_thread_register("Vec_push", (int64_t(*)())Vec_push);
    p_thread_register("Vec_remove", (int64_t(*)())Vec_remove);
    p_thread_register("Vec_reverse", (int64_t(*)())Vec_reverse);
    p_thread_register("Vec_set", (int64_t(*)())Vec_set);
    p_thread_register("Vec_share", (int64_t(*)())Vec_share);
    p_thread_register("Vec_share_deep", (int64_t(*)())Vec_share_deep);
    p_thread_register("Vec_sort", (int64_t(*)())Vec_sort);
    p_thread_register("Vec_to_str", (int64_t(*)())Vec_to_str);
    p_thread_register("Vec_truncate", (int64_t(*)())Vec_truncate);
    p_thread_register("__eq_ByteBuffer", (int64_t(*)())__eq_ByteBuffer);
    p_thread_register("__eq_Expr", (int64_t(*)())__eq_Expr);
    p_thread_register("__eq_IfExpr", (int64_t(*)())__eq_IfExpr);
    p_thread_register("__eq_ParseResultIfExpr", (int64_t(*)())__eq_ParseResultIfExpr);
    p_thread_register("__eq_StringBuilder", (int64_t(*)())__eq_StringBuilder);
    p_thread_register("abs", (int64_t(*)())p_abs);
    p_thread_register("allocator_alloc", (int64_t(*)())allocator_alloc);
    p_thread_register("allocator_bytes_used", (int64_t(*)())allocator_bytes_used);
    p_thread_register("allocator_free", (int64_t(*)())allocator_free);
    p_thread_register("assert", (int64_t(*)())assert);
    p_thread_register("assert_msg", (int64_t(*)())assert_msg);
    p_thread_register("byte_to_str", (int64_t(*)())byte_to_str);
    p_thread_register("clamp", (int64_t(*)())clamp);
    p_thread_register("create_allocator", (int64_t(*)())create_allocator);
    p_thread_register("float_to_str", (int64_t(*)())float_to_str);
    p_thread_register("hm_eq", (int64_t(*)())hm_eq);
    p_thread_register("hm_hash", (int64_t(*)())hm_hash);
    p_thread_register("hm_int_hash", (int64_t(*)())hm_int_hash);
    p_thread_register("hm_is_int", (int64_t(*)())hm_is_int);
    p_thread_register("hm_store_i", (int64_t(*)())hm_store_i);
    p_thread_register("max", (int64_t(*)())p_max);
    p_thread_register("min", (int64_t(*)())p_min);
    p_thread_register("panic", (int64_t(*)())panic);
    p_thread_register("println", (int64_t(*)())println);
    p_thread_register("u64_to_str", (int64_t(*)())u64_to_str);
    int64_t result = main_entry(NULL);
    return (int)result;
}

#ifdef _WIN32
int main(int argc, char** argv) { return real_main(argc, argv); }
#else
int main(int argc, char** argv) { return real_main(argc, argv); }
#endif


// ─── Runtime implementation ───

PVec* p_vec_new(void) {
    PVec* v = (PVec*)p_malloc(sizeof(PVec));
    v->data = NULL; v->len = 0; v->cap = 0;
    return v;
}

void p_vec_push(PVec* v, void* val) {
    if (!v) return;
    if (v->len >= v->cap) {
        v->cap = v->cap == 0 ? 8 : v->cap * 2;
        void** new_data = (void**)p_malloc(v->cap * (int64_t)sizeof(void*));
        if (v->data && v->len > 0) { memcpy(new_data, v->data, v->len * sizeof(void*)); }
        v->data = new_data;
    }
    v->data[v->len++] = val;
}

void* p_vec_get(PVec* v, int64_t idx) {
    if (!v || idx < 0 || idx >= v->len) return NULL;
    return v->data[idx];
}

void p_vec_set(PVec* v, int64_t idx, void* val) {
    if (!v || idx < 0 || idx >= v->len) return;
    v->data[idx] = val;
}

int64_t p_vec_len(PVec* v) { return v ? v->len : 0; }
bool p_vec_is_empty(PVec* v) { return v ? v->len == 0 : true; }
void p_vec_clear(PVec* v) { if (v) v->len = 0; }
int64_t p_vec_pop(PVec* v) {
    if (!v || v->len == 0) return 0;
    v->len--;
    return (int64_t)v->data[v->len];
}

PMap* p_map_new(void) {
    PMap* m = (PMap*)p_malloc(sizeof(PMap));
    m->keys = NULL; m->vals = NULL; m->len = 0; m->cap = 0;
    return m;
}

void p_map_insert(PMap* m, const char* key, int64_t val) {
    if (!m || !key) return;
    for (int64_t i = 0; i < m->len; i++) {
        if (strcmp(m->keys[i], key) == 0) { m->vals[i] = val; return; }
    }
    if (m->len >= m->cap) {
        m->cap = m->cap == 0 ? 8 : m->cap * 2;
        char** new_keys = (char**)p_malloc(m->cap * (int64_t)sizeof(char*));
        int64_t* new_vals = (int64_t*)p_malloc(m->cap * (int64_t)sizeof(int64_t));
        if (m->keys && m->len > 0) { memcpy(new_keys, m->keys, m->len * sizeof(char*)); memcpy(new_vals, m->vals, m->len * sizeof(int64_t)); }
        m->keys = new_keys;
        m->vals = new_vals;
    }
    { size_t klen = strlen(key); char* kcopy = (char*)p_malloc((int64_t)(klen + 1)); memcpy(kcopy, key, klen + 1); m->keys[m->len] = kcopy; }
    m->vals[m->len] = val;
    m->len++;
}

int64_t p_map_get(PMap* m, const char* key) {
    if (!m || !key) return 0;
    for (int64_t i = 0; i < m->len; i++) {
        if (strcmp(m->keys[i], key) == 0) return m->vals[i];
    }
    return 0;
}

bool p_map_contains(PMap* m, const char* key) {
    if (!m || !key) return false;
    for (int64_t i = 0; i < m->len; i++) {
        if (strcmp(m->keys[i], key) == 0) return true;
    }
    return false;
}

void p_map_remove(PMap* m, const char* key) {
    if (!m || !key) return;
    for (int64_t i = 0; i < m->len; i++) {
        if (strcmp(m->keys[i], key) == 0) {
            free(m->keys[i]);
            m->keys[i] = m->keys[m->len-1];
            m->vals[i] = m->vals[m->len-1];
            m->len--;
            return;
        }
    }
}

int64_t p_map_len(PMap* m) { return m ? m->len : 0; }
bool p_map_is_empty(PMap* m) { return m ? m->len == 0 : true; }
void p_map_clear(PMap* m) { if (m) m->len = 0; }

// Phase 10B: p_map_keys returns a Fides Vec (i64 pointer)
int64_t p_map_keys(PMap* m) {
    PVec* v = p_vec_new();
    for (int64_t i = 0; i < m->len; i++)
        p_vec_push(v, (void*)m->keys[i]);
    return (int64_t)v;
}

int64_t p_map_values(PMap* m) {
    PVec* v = p_vec_new();
    for (int64_t i = 0; i < m->len; i++) {
        p_vec_push(v, (void*)m->vals[i]);
    }
    return (int64_t)v;
}

int64_t p_string_len(const char* s) { return s ? (int64_t)strlen(s) : 0; }
bool p_string_is_empty(const char* s) { return !s || s[0] == 0; }

char* p_string_push_str(const char* s, const char* other) {
    if (!s) s = ""; if (!other) other = "";
    size_t len = strlen(s) + strlen(other);
    char* result = (char*)malloc(len + 1);
    strcpy(result, s);
    strcat(result, other);
    return result;
}

char* p_string_from(const char* s) { return s ? strdup(s) : strdup(""); }

char* p_string_slice(const char* s, int64_t start, int64_t end) {
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
}

char* p_to_str(int64_t val) {
    char* buf = (char*)p_malloc(32);
    snprintf(buf, 32, "%lld", (long long)val);
    return buf;
}

char* p_str_concat(const char* a, const char* b) {
    size_t la = strlen(a), lb = strlen(b);
    // Issue 25: overflow check before allocation
    if (la > SIZE_MAX - lb - 1) { char* r = (char*)p_malloc(1); r[0] = 0; return r; }
    size_t len = la + lb;
    char* result = (char*)p_malloc((int64_t)(len + 1));
    if (!result) { char* r = (char*)p_malloc(1); r[0] = 0; return r; }
    strcpy(result, a);
    strcat(result, b);
    return result;
}

char* p_str_join_n(PVec* parts) {
    size_t total = 1;
    for (int64_t i = 0; i < parts->len; i++) {
        size_t pl = strlen((char*)parts->data[i]);
        if (pl > SIZE_MAX - total) pl = SIZE_MAX - total; // Issue 25: overflow guard
        total += pl;
    }
    char* result = (char*)p_malloc((int64_t)total);
    if (!result) { char* r = (char*)p_malloc(1); r[0] = 0; return r; }
    result[0] = 0;
    for (int64_t i = 0; i < parts->len; i++) {
        strcat(result, (char*)parts->data[i]);
    }
    return result;
}

typedef struct { char* buf; int64_t len; int64_t cap; } p_sb;
void* p_sb_new(void) {
    p_sb* sb = (p_sb*)malloc(sizeof(p_sb));
    sb->cap = 4096;
    sb->len = 0;
    sb->buf = (char*)malloc(sb->cap);
    sb->buf[0] = 0;
    return sb;
}
void p_sb_append(void* vsb, const char* s) {
    p_sb* sb = (p_sb*)vsb;
    size_t slen = strlen(s);
    if (sb->len + (int64_t)slen + 1 > sb->cap) {
        while (sb->len + (int64_t)slen + 1 > sb->cap) sb->cap *= 2;
        sb->buf = (char*)realloc(sb->buf, sb->cap);
    }
    memcpy(sb->buf + sb->len, s, slen);
    sb->len += (int64_t)slen;
    sb->buf[sb->len] = 0;
}
const char* p_sb_to_str(void* vsb) {
    p_sb* sb = (p_sb*)vsb;
    size_t len = (size_t)sb->len;
    char* result = (char*)p_malloc((int64_t)(len + 1));
    memcpy(result, sb->buf, len);
    result[len] = 0;
    return result;
}

void p_print(int64_t val) { char b[32]; int n = snprintf(b, sizeof b, "%lld", (long long)val); fwrite(b, 1, (size_t)n, stdout); }
void p_print_str(const char* s) { fwrite(s, 1, strlen(s), stdout); }
void p_eprint_str(const char* s) { fwrite(s, 1, strlen(s), stderr); fflush(stderr); }
void p_flush_stdout(void) { fflush(stdout); }
int64_t p_stdin_eof(void) { return (int64_t)feof(stdin); }
char* p_read_line(void) {
    static char buf[65536];
    if (fgets(buf, sizeof(buf), stdin) == NULL) return NULL;
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
    return buf;
}
char* p_read_stdin(int64_t n) {
    char* buf = (char*)malloc(n + 1);
    if (!buf) return NULL;
    int64_t read = 0;
    while (read < n) {
        int c = fgetc(stdin);
        if (c == EOF) break;
        buf[read] = (char)c;
        read++;
    }
    buf[read] = '\0';
    return buf;
}
void p_print_int(int64_t val) { char b[32]; int n = snprintf(b, sizeof b, "%lld", (long long)val); fwrite(b, 1, (size_t)n, stdout); }
void p_print_bool(bool val) { if (val) fwrite("true", 1, 4, stdout); else fwrite("false", 1, 5, stdout); }
void p_print_float(double val) { char* _s = p_to_str_float(val); fwrite(_s, 1, strlen(_s), stdout); free(_s); }
void p_print_newline(void) { fwrite("\n", 1, 1, stdout); }
/* Crashes, stack overflow and panic traces. Nothing is recorded per call:
   a trace walks the machine stack and names each frame that starts at a
   Fides function; a stack overflow hits the OS guard page, and its handler
   runs on reserved stack. */
typedef struct { const void* addr; const char* name; } __p_fn_entry;
extern const __p_fn_entry __p_fns[];
extern const int __p_fn_count;
static __p_fn_entry* __p_fns_sorted = NULL;
static int __p_fn_cmp(const void* a, const void* b) {
    uintptr_t x = (uintptr_t)((const __p_fn_entry*)a)->addr, y = (uintptr_t)((const __p_fn_entry*)b)->addr;
    return x < y ? -1 : x > y;
}
static const char* __p_fn_name(uintptr_t start) {
    if (!__p_fns_sorted) {
        __p_fns_sorted = (__p_fn_entry*)malloc(sizeof(__p_fn_entry) * (size_t)(__p_fn_count + 1));
        if (!__p_fns_sorted) return NULL;
        memcpy(__p_fns_sorted, __p_fns, sizeof(__p_fn_entry) * (size_t)__p_fn_count);
        qsort(__p_fns_sorted, (size_t)__p_fn_count, sizeof(__p_fn_entry), __p_fn_cmp);
    }
    int lo = 0, hi = __p_fn_count - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        uintptr_t a = (uintptr_t)__p_fns_sorted[mid].addr;
        if (a == start) return __p_fns_sorted[mid].name;
        if (a < start) lo = mid + 1; else hi = mid - 1;
    }
    return NULL;
}
#if defined(_WIN32) && (defined(_M_X64) || defined(__x86_64__)) && !defined(__TINYC__)
#include <windows.h>
/* Where the function of each frame starts, from the unwind tables. ctx: a
   CONTEXT to start from (exact: its pc is the faulting one), or NULL. */
static int __p_frames(void* ctx, int exact, uintptr_t* out, int max) {
    CONTEXT c;
    if (ctx) c = *(CONTEXT*)ctx; else { RtlCaptureContext(&c); exact = 0; }
    int k = 0;
    for (int i = 0; i < 256 && k < max && c.Rip; i++) {
        DWORD64 pc = (i == 0 && exact) ? c.Rip : c.Rip - 1;
        DWORD64 base = 0; PVOID hd = NULL; DWORD64 est = 0;
        PRUNTIME_FUNCTION rf = RtlLookupFunctionEntry(pc, &base, NULL);
        if (!rf) { c.Rip = *(DWORD64*)c.Rsp; c.Rsp += 8; continue; }
        out[k++] = (uintptr_t)(base + rf->BeginAddress);
        RtlVirtualUnwind(0, base, pc, rf, &c, &hd, &est, NULL);
    }
    return k;
}
#elif (defined(__GNUC__) || defined(__clang__)) && !defined(__TINYC__) && !defined(_WIN32)
#include <unwind.h>
typedef struct { uintptr_t* out; int n; int max; } __p_unw;
static _Unwind_Reason_Code __p_unw_step(struct _Unwind_Context* uc, void* arg) {
    __p_unw* u = (__p_unw*)arg;
    if (u->n >= u->max) return _URC_END_OF_STACK;
    uintptr_t s = (uintptr_t)_Unwind_GetRegionStart(uc);
    if (s) u->out[u->n++] = s;
    return _URC_NO_REASON;
}
static int __p_frames(void* ctx, int exact, uintptr_t* out, int max) {
    (void)ctx; (void)exact;
    __p_unw u = { out, 0, max };
    _Unwind_Backtrace(__p_unw_step, &u);
    return u.n;
}
#else
static int __p_frames(void* ctx, int exact, uintptr_t* out, int max) { (void)ctx; (void)exact; (void)out; (void)max; return 0; }
#endif
void __p_print_trace_ctx(void* ctx, int exact) {
    uintptr_t f[64];
    const char* names[64];
    int n = __p_frames(ctx, exact, f, 64), m = 0;
    for (int i = 0; i < n; i++) {
        const char* name = __p_fn_name(f[i]);
        if (name) names[m++] = name;
    }
    if (m == 0) return;
    fprintf(stderr, "stack trace:\n    → %s\n", names[0]);
    for (int i = 1; i < m; ) {
        int j = i + 1;
        while (j < m && strcmp(names[j], names[i]) == 0) j++;
        if (j - i > 1) fprintf(stderr, "    ↳ called from %s (%d times)\n", names[i], j - i);
        else fprintf(stderr, "    ↳ called from %s\n", names[i]);
        i = j;
    }
}
#ifdef _WIN32
void __p_thread_guard_init(void) {
#if !defined(__TINYC__)
    ULONG g = 64 * 1024;
    SetThreadStackGuarantee(&g);
#endif
}
void __p_thread_guard_done(void) {}
LONG WINAPI __p_crash_handler(PEXCEPTION_POINTERS ep) {
    DWORD code = ep->ExceptionRecord->ExceptionCode;
    if (code == EXCEPTION_STACK_OVERFLOW) {
        fprintf(stderr, "\nStack overflow: the recursion is too deep\n");
    } else if (code == EXCEPTION_ACCESS_VIOLATION) {
        fprintf(stderr, "\n=== CRASH: 0x%lX ===\n", (unsigned long)code);
        fprintf(stderr, "Access violation at address 0x%llX\n", (unsigned long long)ep->ExceptionRecord->ExceptionInformation[1]);
    } else {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    __p_print_trace_ctx(ep->ContextRecord, 1);
    fflush(stderr);
    return EXCEPTION_CONTINUE_SEARCH;
}
void __p_install_crash_handlers(void) {
    SetUnhandledExceptionFilter(__p_crash_handler);
    __p_thread_guard_init();
}
#else
#include <signal.h>
static P_THREAD_LOCAL char* __p_stack_top;
static void __p_sig_handler(int sig, siginfo_t* si, void* uc) {
    (void)uc;
    const char* name = sig == SIGSEGV ? "SIGSEGV" : sig == SIGBUS ? "SIGBUS" : sig == SIGABRT ? "SIGABRT" : sig == SIGFPE ? "SIGFPE" : "UNKNOWN";
    char* a = (char*)si->si_addr;
    if ((sig == SIGSEGV || sig == SIGBUS) && __p_stack_top && a < __p_stack_top && a >= __p_stack_top - (64 << 20))
        fprintf(stderr, "\nStack overflow: the recursion is too deep\n");
    else
        fprintf(stderr, "\n=== CRASH: signal %d (%s) ===\n", sig, name);
    __p_print_trace_ctx(NULL, 0);
    fflush(stderr);
    _exit(128 + sig);
}
/* Each thread: a reserved stack for the handler, so it runs on overflow. */
void __p_thread_guard_init(void) {
    char here;
    __p_stack_top = &here;
    stack_t ss;
    memset(&ss, 0, sizeof ss);
    ss.ss_sp = malloc(64 * 1024);
    ss.ss_size = 64 * 1024;
    if (ss.ss_sp) sigaltstack(&ss, NULL);
}
void __p_thread_guard_done(void) {
    stack_t ss, old;
    memset(&ss, 0, sizeof ss);
    ss.ss_flags = SS_DISABLE;
    if (sigaltstack(&ss, &old) == 0 && old.ss_sp) free(old.ss_sp);
}
void __p_install_crash_handlers(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_sigaction = __p_sig_handler;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGFPE, &sa, NULL);
    __p_thread_guard_init();
}
#endif
void p_panic(const char* msg) {
    fprintf(stderr, "Panic: %s\n", msg);
    __p_print_trace_ctx(NULL, 0);
    fflush(stdout);
    exit(1);
}
#define __P_ARENA_CHUNK_SIZE (4*1024*1024)
typedef struct __p_arena_chunk { char* base; char* ptr; char* end; struct __p_arena_chunk* next; } __p_arena_chunk;
static P_THREAD_LOCAL __p_arena_chunk* __p_arena_head = NULL;
static P_THREAD_LOCAL __p_arena_chunk* __p_arena_cur = NULL;
static __p_arena_chunk* __p_arena_new_chunk(void) {
    __p_arena_chunk* c = (__p_arena_chunk*)malloc(sizeof(__p_arena_chunk));
    if (!c) return NULL;
    c->base = (char*)malloc(__P_ARENA_CHUNK_SIZE);
    if (!c->base) { free(c); return NULL; }
    c->ptr = c->base;
    c->end = c->base + __P_ARENA_CHUNK_SIZE;
    c->next = NULL;
    return c;
}
#define P_BLOCK_MAGIC 0xF1DE5A0000000000ULL
#define P_BLOCK_FREED 0xF1DE5B0000000000ULL
#define P_CLASS_LARGE 63
static P_THREAD_LOCAL uint64_t* __p_free_lists[41];
void* p_malloc(int64_t size) {
    if (size < 0) { fprintf(stderr, "p_malloc: negative size %lld\n", (long long)size); abort(); }
    size_t total = (size_t)size + 8;
    if (total > ((size_t)1 << 20)) {
        uint64_t* big = (uint64_t*)calloc(1, total);
        if (!big) { fprintf(stderr, "p_malloc: OOM allocating %zu bytes\n", total); abort(); }
        big[0] = P_BLOCK_MAGIC | P_CLASS_LARGE;
        return big + 1;
    }
    int k = 4;
    while (((size_t)1 << k) < total) k++;
    size_t sz = (size_t)1 << k;
    k *= 2;
    if (k > 8 && total <= sz - sz / 4) { sz -= sz / 4; k--; }
    uint64_t* reused = __p_free_lists[k];
    if (reused) {
        __p_free_lists[k] = (uint64_t*)reused[0];
        memset(reused, 0, sz - 8);
        reused[-1] = P_BLOCK_MAGIC | (uint64_t)k;
        return reused;
    }
    if (!__p_arena_cur) { __p_arena_cur = __p_arena_new_chunk(); __p_arena_head = __p_arena_cur; }
    if (!__p_arena_cur) { fprintf(stderr, "p_malloc: OOM — failed to create first arena chunk\n"); abort(); }
    if (__p_arena_cur->ptr + sz > __p_arena_cur->end) {
        __p_arena_chunk* nc = __p_arena_new_chunk();
        if (!nc) { fprintf(stderr, "p_malloc: OOM — failed to allocate new arena chunk\n"); abort(); }
        __p_arena_cur->next = nc;
        __p_arena_cur = nc;
    }
    uint64_t* block = (uint64_t*)__p_arena_cur->ptr;
    __p_arena_cur->ptr += sz;
    block[0] = P_BLOCK_MAGIC | (uint64_t)k;
    return block + 1;
}
void* p_malloc_persistent(int64_t size) { if (size < 0) return NULL; return malloc((size_t)size); }
void* p_alloc(int64_t size) { return p_malloc(size); }
void p_free(void* ptr) {
    if (!ptr) return;
    uint64_t* b = (uint64_t*)ptr;
    uint64_t h = b[-1];
    if ((h & ~(uint64_t)0xFF) == P_BLOCK_FREED) p_panic("Double free");
    if ((h & ~(uint64_t)0xFF) != P_BLOCK_MAGIC) p_panic("Invalid free: not an allocated block");
    int k = (int)(h & 0xFF);
    b[-1] = P_BLOCK_FREED | (uint64_t)k;
    if (k == P_CLASS_LARGE) { free(b - 1); return; }
    b[0] = (uint64_t)__p_free_lists[k];
    __p_free_lists[k] = b;
}
void p_region_free(void* region) { (void)region; }
void p_memset(void* ptr, int val, int64_t size) { if (ptr && size > 0) memset(ptr, val, (size_t)size); }
void p_str_zeroize(void* s) {
    if (!s) return;
    for (__p_arena_chunk* c = __p_arena_head; c; c = c->next) {
        if ((char*)s >= c->base && (char*)s < c->end) {
            volatile char* p = (volatile char*)s;
            while (p < (volatile char*)c->end && *p) { *p = 0; p++; }
            return;
        }
    }
}
void p_vec_zeroize(void* v) {
    PVec* pv = (PVec*)v;
    if (!pv || !pv->data) return;
    volatile char* p = (volatile char*)pv->data;
    for (int64_t i = 0; i < pv->cap * (int64_t)sizeof(void*); i++) p[i] = 0;
}
int64_t p_arena_save(void) {
    int64_t idx = 0;
    for (__p_arena_chunk* c = __p_arena_head; c; c = c->next) {
        idx++;
        if (c == __p_arena_cur) return (idx << 32) | (int64_t)(c->ptr - c->base);
    }
    return 0;
}
void p_arena_restore(int64_t mark) {
    memset(__p_free_lists, 0, sizeof(__p_free_lists));
    int64_t idx = mark >> 32, off = mark & 0xffffffffLL, i = 0;
    __p_arena_chunk* keep = __p_arena_head;
    if (!keep) return;
    if (idx == 0) { keep->ptr = keep->base; }
    else {
        for (__p_arena_chunk* c = __p_arena_head; c; c = c->next) { if (++i == idx) { keep = c; break; } }
        if (i != idx) return;
        keep->ptr = keep->base + off;
    }
    __p_arena_chunk* c = keep->next;
    keep->next = NULL;
    while (c) { __p_arena_chunk* n = c->next; free(c->base); free(c); c = n; }
    __p_arena_cur = keep;
}
char* p_file_read(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f);
    if (sz < 0) { fclose(f); return NULL; }
    fseek(f, 0, SEEK_SET);
    char* buf = (char*)malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f); buf[rd] = 0; fclose(f);
    return buf;
}
int64_t p_file_write(const char* path, const char* content) {
    FILE* f = fopen(path, "wb");
    if (!f) return -1;
    fputs(content, f); fclose(f); return 0;
}
int64_t p_file_write_bin(const char* path, const char* data, int64_t len) {
    FILE* f = fopen(path, "wb");
    if (!f) return -1;
    fwrite(data, 1, (size_t)len, f); fclose(f); return 0;
}
PVec* p_file_read_bytes(const char* path) {
    PVec* v = p_vec_new();
    FILE* f = fopen(path, "rb");
    if (!f) return v;
    int c;
    while ((c = fgetc(f)) != EOF) p_vec_push(v, (void*)(intptr_t)c);
    fclose(f); return v;
}
int64_t p_file_write_vec(const char* path, PVec* v) {
    FILE* f = fopen(path, "wb");
    if (!f) return -1;
    if (v) { int64_t i; for (i = 0; i < v->len; i++) { unsigned char b = (unsigned char)(int64_t)v->data[i]; fwrite(&b, 1, 1, f); } }
    fclose(f); return 0;
}
bool p_file_exists(const char* path) { FILE* f = fopen(path, "rb"); if (f) { fclose(f); return true; } return false; }
int64_t p_file_append(const char* path, const char* content) {
    FILE* f = fopen(path, "ab");
    if (!f) return -1;
    fputs(content, f); fclose(f); return 0;
}
int64_t p_file_delete(const char* path) { return remove(path) == 0 ? 0 : -1; }
int64_t p_file_size(const char* path) { FILE* f = fopen(path, "rb"); if (!f) return -1; fseek(f, 0, SEEK_END); int64_t sz = ftell(f); fclose(f); return sz < 0 ? -1 : sz; }
int64_t p_file_hash(const char* path) { FILE* f = fopen(path, "rb"); if (!f) return 0; uint64_t h = 0xcbf29ce484222325ULL; unsigned char buf[65536]; size_t n; while ((n = fread(buf, 1, sizeof buf, f)) > 0) { for (size_t i = 0; i < n; i++) { h ^= buf[i]; h *= 0x100000001b3ULL; } } fclose(f); return (int64_t)h; }
char* p_getenv(const char* name) { return getenv(name); }
int64_t p_argc(void) { return (int64_t)__p_argc; }
char* p_argv(int64_t index) { if (index < 0 || index >= __p_argc) return NULL; return __p_argv[index]; }
int64_t p_system(const char* cmd) {
    int st = system(cmd);
#ifdef _WIN32
    return (int64_t)st;
#else
    return WIFEXITED(st) ? (int64_t)WEXITSTATUS(st) : -1;
#endif
}
int64_t p_getpid(void) {
#ifdef _WIN32
    return (int64_t)_getpid();
#else
    return (int64_t)getpid();
#endif
}
typedef struct { char* out; char* err; int64_t code; int used; } PProc;
static PProc __p_proc_table[128];
static char* __p_read_all(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) { char* e = (char*)malloc(1); e[0] = 0; return e; }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    if (n < 0) n = 0;
    char* buf = (char*)malloc((size_t)n + 1);
    size_t got = fread(buf, 1, (size_t)n, f);
    buf[got] = 0; fclose(f); return buf;
}
static char* __p_proc_text(const char* s) {
    char* r = (char*)p_malloc((int64_t)strlen(s) + 1);
    strcpy(r, s);
    return r;
}
int64_t p_process_run(const char* cmd) {
    if (!cmd || !*cmd) return 0;
    static int ctr = 0;
    int slot = -1;
    for (int i = 0; i < 128; i++) if (!__p_proc_table[i].used) { slot = i; break; }
    if (slot < 0) return 0;
    char of[128], ef[128], full[8192];
    snprintf(of, sizeof of, ".fides_proc_%ld_%d.out", (long)p_getpid(), ctr);
    snprintf(ef, sizeof ef, ".fides_proc_%ld_%d.err", (long)p_getpid(), ctr);
    ctr++;
    snprintf(full, sizeof full, "( %s ) > \"%s\" 2> \"%s\"", cmd, of, ef);
    int st = system(full);
#ifdef _WIN32
    __p_proc_table[slot].code = (int64_t)st;
#else
    __p_proc_table[slot].code = WIFEXITED(st) ? (int64_t)WEXITSTATUS(st) : -1;
#endif
    __p_proc_table[slot].out = __p_read_all(of);
    __p_proc_table[slot].err = __p_read_all(ef);
    remove(of); remove(ef);
    __p_proc_table[slot].used = 1;
    return (int64_t)slot + 1;
}
char* p_process_stdout(int64_t h) {
    if (h < 1 || h > 128 || !__p_proc_table[h-1].used) return "";
    return __p_proc_text(__p_proc_table[h-1].out);
}
char* p_process_stderr(int64_t h) {
    if (h < 1 || h > 128 || !__p_proc_table[h-1].used) return "";
    return __p_proc_text(__p_proc_table[h-1].err);
}
int64_t p_process_code(int64_t h) {
    if (h < 1 || h > 128 || !__p_proc_table[h-1].used) return -1;
    return __p_proc_table[h-1].code;
}
void p_process_close(int64_t h) {
    if (h < 1 || h > 128 || !__p_proc_table[h-1].used) return;
    free(__p_proc_table[h-1].out); free(__p_proc_table[h-1].err);
    __p_proc_table[h-1].used = 0;
}
void p_exit(int64_t code) { exit((int)code); }
int64_t p_mkdir(const char* path) {
#ifdef _WIN32
    return _mkdir(path) == 0 ? 0 : -1;
#else
    return mkdir(path, 0755) == 0 ? 0 : -1;
#endif
}
int64_t p_rmdir(const char* path) {
#ifdef _WIN32
    return _rmdir(path) == 0 ? 0 : -1;
#else
    return rmdir(path) == 0 ? 0 : -1;
#endif
}
int64_t p_rename(const char* a, const char* b) { return rename(a, b) == 0 ? 0 : -1; }
bool p_is_dir(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) return false;
    return (st.st_mode & S_IFMT) == S_IFDIR;
}
char* p_dir_list(const char* path) {
    // entry names joined by '\n', no "."/".."; "" on failure
    size_t need = 1;
#ifdef _WIN32
    WIN32_FIND_DATAA fd; char pat[2056];
    snprintf(pat, sizeof pat, "%s\\*", path);
    HANDLE h = FindFirstFileA(pat, &fd);
    if (h == INVALID_HANDLE_VALUE) { char* e = (char*)malloc(1); e[0] = 0; return e; }
    do {
        if (strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, ".."))
            need += strlen(fd.cFileName) + 1;
    } while (FindNextFileA(h, &fd));
    FindClose(h);
    char* out = (char*)malloc(need);
    out[0] = 0;
    h = FindFirstFileA(pat, &fd);
    if (h == INVALID_HANDLE_VALUE) return out;
    int first = 1;
    do {
        if (strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, "..")) {
            if (!first) strcat(out, "\n");
            strcat(out, fd.cFileName); first = 0;
        }
    } while (FindNextFileA(h, &fd));
    FindClose(h);
    return out;
#else
    DIR* d = opendir(path);
    if (!d) { char* e = (char*)malloc(1); e[0] = 0; return e; }
    struct dirent* ent;
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, ".."))
            need += strlen(ent->d_name) + 1;
    }
    rewinddir(d);
    char* out = (char*)malloc(need);
    out[0] = 0;
    int first = 1;
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
            if (!first) strcat(out, "\n");
            strcat(out, ent->d_name); first = 0;
        }
    }
    closedir(d);
    return out;
#endif
}
#if defined(__linux__) || defined(__APPLE__)
#include <time.h>
int64_t p_clock_now(void) { struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts); return (int64_t)ts.tv_sec * 1000000000LL + (int64_t)ts.tv_nsec; }
int64_t p_clock_now_us(void) { struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts); return (int64_t)ts.tv_sec * 1000000LL + (int64_t)ts.tv_nsec / 1000; }
int64_t p_clock_now_ms(void) { struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts); return (int64_t)ts.tv_sec * 1000LL + (int64_t)ts.tv_nsec / 1000000; }
#elif defined(_WIN32)
#include <windows.h>
/* 100 ns units since 1970: the precise clock (time() has 1 s resolution) */
static int64_t __p_now_100ns(void) {
    FILETIME ft;
#if defined(__TINYC__)
    GetSystemTimeAsFileTime(&ft);
#else
    GetSystemTimePreciseAsFileTime(&ft);
#endif
    return (int64_t)((((uint64_t)ft.dwHighDateTime) << 32) | ft.dwLowDateTime) - 116444736000000000LL;
}
int64_t p_clock_now(void) { return __p_now_100ns() * 100; }
int64_t p_clock_now_us(void) { return __p_now_100ns() / 10; }
int64_t p_clock_now_ms(void) { return __p_now_100ns() / 10000; }
#else
#include <time.h>
int64_t p_clock_now(void) { return (int64_t)time(NULL) * 1000000000LL; }
int64_t p_clock_now_us(void) { return (int64_t)time(NULL) * 1000000LL; }
int64_t p_clock_now_ms(void) { return (int64_t)time(NULL) * 1000LL; }
#endif
/* 64 random bits from the operating system's secure generator */
#ifdef _WIN32
int rand_s(unsigned int* v);
int64_t p_os_random_u64(void) {
    unsigned int a = 0, b = 0;
    if (rand_s(&a) != 0 || rand_s(&b) != 0) p_panic("no random source in the operating system");
    return (int64_t)(((uint64_t)a << 32) | b);
}
#else
#include <fcntl.h>
#include <unistd.h>
int64_t p_os_random_u64(void) {
    uint64_t v = 0;
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0 || read(fd, &v, sizeof v) != (ssize_t)sizeof v) p_panic("no random source in the operating system");
    close(fd);
    return (int64_t)v;
}
#endif
bool p_regex_match(const char* s, const char* pattern) {
    if (!s || !pattern) return false;
    int64_t si = 0, pi = 0, s_len = (int64_t)strlen(s), p_len = (int64_t)strlen(pattern);
    int64_t star_s = -1, star_p = -1;
    while (si < s_len) {
        if (pi < p_len && (pattern[pi] == '?' || pattern[pi] == s[si])) { si++; pi++; }
        else if (pi < p_len && pattern[pi] == '*') { star_p = pi; star_s = si; pi++; }
        else if (star_p >= 0) { pi = star_p + 1; si = star_s + 1; star_s = si; }
        else return false;
    }
    while (pi < p_len && pattern[pi] == '*') pi++;
    return pi == p_len;
}
static char** __p_intern_table = NULL;
static int64_t __p_intern_cap = 0;
static int64_t __p_intern_count = 0;
static int64_t __p_intern_hash(const char* s) {
    int64_t h = 1469598103934665603ULL;
    while (*s) { h ^= (unsigned char)*s; h *= 1099511628211ULL; s++; }
    return h;
}
static void __p_intern_grow(void) {
    int64_t new_cap = __p_intern_cap == 0 ? 4096 : __p_intern_cap * 2;
    char** new_table = (char**)calloc(new_cap, sizeof(char*));
    if (!new_table) { fprintf(stderr, "p_intern: OOM\n"); abort(); }
    for (int64_t i = 0; i < __p_intern_cap; i++) {
        if (__p_intern_table[i]) {
            int64_t h = __p_intern_hash(__p_intern_table[i]) & (new_cap - 1);
            while (new_table[h]) h = (h + 1) & (new_cap - 1);
            new_table[h] = __p_intern_table[i];
        }
    }
    free(__p_intern_table);
    __p_intern_table = new_table;
    __p_intern_cap = new_cap;
}
const char* p_intern_str(const char* s) {
    if (!s) return NULL;
    if (__p_intern_count * 2 >= __p_intern_cap) __p_intern_grow();
    int64_t mask = __p_intern_cap - 1;
    int64_t h = __p_intern_hash(s) & mask;
    while (__p_intern_table[h]) {
        if (strcmp(__p_intern_table[h], s) == 0) {
            return __p_intern_table[h];
        }
        h = (h + 1) & mask;
    }
    __p_intern_table[h] = (char*)s;
    __p_intern_count++;
    return s;
}
#ifdef _WIN32
typedef struct { HANDLE h; void* ret; void* (*fn)(void*); void* arg; } __p_win_thread;
typedef __p_win_thread* pthread_t;
typedef CRITICAL_SECTION pthread_mutex_t;
typedef CONDITION_VARIABLE pthread_cond_t;
typedef DWORD pthread_key_t;
static DWORD WINAPI __p_thread_start(LPVOID p) { __p_win_thread* t = (__p_win_thread*)p; t->ret = t->fn(t->arg); return 0; }
static inline int pthread_create(pthread_t* t, void* attr, void* (*fn)(void*), void* arg) { (void)attr; __p_win_thread* w = (__p_win_thread*)malloc(sizeof(__p_win_thread)); if (!w) return -1; w->ret = NULL; w->fn = fn; w->arg = arg; w->h = CreateThread(NULL, 64 * 1024 * 1024, __p_thread_start, w, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL); if (!w->h) { free(w); p_panic("could not start a thread"); } *t = w; return 0; }
static inline int pthread_join(pthread_t t, void** ret) { WaitForSingleObject(t->h, INFINITE); CloseHandle(t->h); if (ret) *ret = t->ret; free(t); return 0; }
static inline int pthread_mutex_init(pthread_mutex_t* m, void* a) { (void)a; InitializeCriticalSection(m); return 0; }
static inline int pthread_mutex_lock(pthread_mutex_t* m) { EnterCriticalSection(m); return 0; }
static inline int pthread_mutex_unlock(pthread_mutex_t* m) { LeaveCriticalSection(m); return 0; }
static inline int pthread_cond_init(pthread_cond_t* c, void* a) { (void)a; InitializeConditionVariable(c); return 0; }
static inline int pthread_cond_wait(pthread_cond_t* c, pthread_mutex_t* m) { SleepConditionVariableCS(c, m, INFINITE); return 0; }
static inline int pthread_cond_signal(pthread_cond_t* c) { WakeConditionVariable(c); return 0; }
static inline int pthread_cond_broadcast(pthread_cond_t* c) { WakeAllConditionVariable(c); return 0; }
static inline int pthread_key_create(pthread_key_t* k, void(*d)(void*)) { (void)d; *k = TlsAlloc(); return *k == TLS_OUT_OF_INDEXES ? -1 : 0; }
static inline void* pthread_getspecific(pthread_key_t k) { return TlsGetValue(k); }
static inline int pthread_setspecific(pthread_key_t k, const void* v) { return TlsSetValue(k, (LPVOID)v) ? 0 : -1; }
#else
#include <pthread.h>
#endif
int64_t p_future_ready(int64_t value) { return value; }
static pthread_t p_thread_handles[256];
static int p_thread_handle_count = 0;
static int64_t p_task_results[256];
static int64_t __p_co_await(int64_t future);
int64_t p_future_poll(int64_t future) {
    if (((uint64_t)future >> 48) == 0x7FA5) return __p_co_await(future);
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
#ifndef _WIN32
#include <stdatomic.h>
#endif
#ifdef _WIN32
typedef long long atomic_int;
typedef long long atomic_int_least64_t;
static inline long long _p_atomic_load64(atomic_int_least64_t* a) { return InterlockedCompareExchange64(a, 0, 0); }
static inline void _p_atomic_store64(atomic_int_least64_t* a, long long v) { InterlockedExchange64(a, v); }
static inline long long _p_atomic_add64(atomic_int_least64_t* a, long long v) { return InterlockedExchangeAdd64(a, v); }
static inline long long _p_atomic_sub64(atomic_int_least64_t* a, long long v) { return InterlockedExchangeAdd64(a, -v); }
static inline int _p_atomic_cas64(atomic_int_least64_t* a, long long* e, long long v) { long long o = InterlockedCompareExchange64(a, v, *e); if (o == *e) return 1; *e = o; return 0; }
#define atomic_load(a) _p_atomic_load64((atomic_int_least64_t*)(a))
#define atomic_store(a, v) _p_atomic_store64((atomic_int_least64_t*)(a), (v))
#define atomic_fetch_add(a, v) _p_atomic_add64((atomic_int_least64_t*)(a), (v))
#define atomic_fetch_sub(a, v) _p_atomic_sub64((atomic_int_least64_t*)(a), (v))
#define atomic_compare_exchange_strong(a, e, v) _p_atomic_cas64((atomic_int_least64_t*)(a), (e), (v))
#define atomic_init(a, v) (*(a) = (v))
#endif
#include <stdarg.h>
typedef struct { const char* name; int64_t (*fn)(); } p_thread_entry_t;
static p_thread_entry_t p_thread_entries[2048];
static int p_thread_entry_count = 0;
void p_thread_register(const char* name, int64_t (*fn)()) {
    if (p_thread_entry_count < 2048) {
        p_thread_entries[p_thread_entry_count].name = name;
        p_thread_entries[p_thread_entry_count].fn = fn;
        p_thread_entry_count++;
    }
}
typedef struct { int64_t (*fn)(); int64_t nargs; int64_t args[8]; atomic_int* done; } p_thread_arg_t;
#if !defined(_WIN32) && !defined(__APPLE__)
#include <ucontext.h>
#endif
#ifndef _WIN32
#include <poll.h>
#include <time.h>
#endif
#if !defined(_WIN32)
#if defined(__APPLE__)
typedef struct { void* sp; } __p_ctx_t;
#else
typedef ucontext_t __p_ctx_t;
#endif
#endif
typedef struct __p_co { int64_t (*fn)(); int64_t nargs; int64_t args[8]; int64_t result; int done; int detached;
    struct __p_co* awaits; struct __p_co* back;
    int64_t fd; int64_t fd_write; int64_t ready; int64_t wake_ms;
#ifdef _WIN32
    void* fiber;
#else
    __p_ctx_t ctx; char* stack;
#endif
} __p_co;
static P_THREAD_LOCAL __p_co** __p_cos = NULL;
static P_THREAD_LOCAL int64_t __p_co_count = 0;
static P_THREAD_LOCAL int64_t __p_co_cap = 0;
static P_THREAD_LOCAL int64_t __p_co_total = 0;
static P_THREAD_LOCAL __p_co* __p_co_cur = NULL;
#ifdef _WIN32
static P_THREAD_LOCAL void* __p_co_sched = NULL;
#else
static P_THREAD_LOCAL __p_ctx_t __p_co_sched;
#endif
#define __P_CO_TAG 0x7FA5
static int64_t __p_co_now(void) {
#ifdef _WIN32
    return (int64_t)GetTickCount64();
#else
    struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t);
    return (int64_t)t.tv_sec * 1000 + t.tv_nsec / 1000000;
#endif
}
#ifndef _WIN32
#include <sys/mman.h>
#endif
/* A task stack on POSIX, with a guard page at its low end. */
#define P_CO_STACK (256 * 1024 + 4096)
/* macOS/iOS have no ucontext on arm64 (and only deprecated it on x86_64),
   so coroutines there use a direct callee-saved register + sp switch.
   A new task starts through __p_co_enter_start, which pulls the __p_co*
   out of a fabricated frame register (x19 on arm64, r15 on x86_64). */
#if defined(__APPLE__)
void __p_co_switch(void** save_sp, void** load_sp);
void __p_co_enter_start(void);
#if defined(__x86_64__)
__asm__(
    ".text\n"
    ".global ___p_co_switch\n"
    ".p2align 4\n"
    "___p_co_switch:\n"
    "    pushq %rbp\n"
    "    pushq %rbx\n"
    "    pushq %r12\n"
    "    pushq %r13\n"
    "    pushq %r14\n"
    "    pushq %r15\n"
    "    movq %rsp, (%rdi)\n"
    "    movq (%rsi), %rsp\n"
    "    popq %r15\n"
    "    popq %r14\n"
    "    popq %r13\n"
    "    popq %r12\n"
    "    popq %rbx\n"
    "    popq %rbp\n"
    "    retq\n"
    ".global ___p_co_enter_start\n"
    ".p2align 4\n"
    "___p_co_enter_start:\n"
    "    movq %r15, %rdi\n"
    "    jmp ___p_co_body_arg\n"
);
#elif defined(__aarch64__)
__asm__(
    ".text\n"
    ".global ___p_co_switch\n"
    ".p2align 4\n"
    "___p_co_switch:\n"
    "    sub sp, sp, #112\n"
    "    stp x19, x20, [sp, #0]\n"
    "    stp x21, x22, [sp, #16]\n"
    "    stp x23, x24, [sp, #32]\n"
    "    stp x25, x26, [sp, #48]\n"
    "    stp x27, x28, [sp, #64]\n"
    "    stp x29, x30, [sp, #80]\n"
    "    mov x9, sp\n"
    "    str x9, [x0]\n"
    "    ldr x9, [x1]\n"
    "    mov sp, x9\n"
    "    ldp x19, x20, [sp, #0]\n"
    "    ldp x21, x22, [sp, #16]\n"
    "    ldp x23, x24, [sp, #32]\n"
    "    ldp x25, x26, [sp, #48]\n"
    "    ldp x27, x28, [sp, #64]\n"
    "    ldp x29, x30, [sp, #80]\n"
    "    add sp, sp, #112\n"
    "    ret\n"
    ".global ___p_co_enter_start\n"
    ".p2align 4\n"
    "___p_co_enter_start:\n"
    "    mov x0, x19\n"
    "    b ___p_co_body_arg\n"
);
#else
#error "unsupported Apple architecture for coroutine switch"
#endif
#endif
static void __p_co_leave(void) {
    __p_co* c = __p_co_cur;
    __p_co* back = c->back;
    c->back = NULL;
    __p_co_cur = back;
#ifdef _WIN32
    SwitchToFiber(back ? back->fiber : __p_co_sched);
#elif defined(__APPLE__)
    __p_co_switch(&c->ctx.sp, back ? &back->ctx.sp : &__p_co_sched.sp);
#else
    swapcontext(&c->ctx, back ? &back->ctx : &__p_co_sched);
#endif
}
static void __p_co_enter(__p_co* c) {
    __p_co* from = __p_co_cur;
    __p_co_cur = c;
#ifdef _WIN32
    (void)from;
    SwitchToFiber(c->fiber);
#elif defined(__APPLE__)
    __p_co_switch(from ? &from->ctx.sp : &__p_co_sched.sp, &c->ctx.sp);
#else
    swapcontext(from ? &from->ctx : &__p_co_sched, &c->ctx);
#endif
    if (c->done) {
#ifdef _WIN32
        DeleteFiber(c->fiber);
#else
        munmap(c->stack, P_CO_STACK);
#endif
    }
}
static void __p_co_body(__p_co* c) {
    int64_t (*f)() = c->fn;
    int64_t* a = c->args;
    switch (c->nargs) {
        case 0: c->result = ((int64_t (*)(void*))f)(NULL); break;
        case 1: c->result = ((int64_t (*)(void*, int64_t))f)(NULL, a[0]); break;
        case 2: c->result = ((int64_t (*)(void*, int64_t, int64_t))f)(NULL, a[0], a[1]); break;
        case 3: c->result = ((int64_t (*)(void*, int64_t, int64_t, int64_t))f)(NULL, a[0], a[1], a[2]); break;
        case 4: c->result = ((int64_t (*)(void*, int64_t, int64_t, int64_t, int64_t))f)(NULL, a[0], a[1], a[2], a[3]); break;
        case 5: c->result = ((int64_t (*)(void*, int64_t, int64_t, int64_t, int64_t, int64_t))f)(NULL, a[0], a[1], a[2], a[3], a[4]); break;
        case 6: c->result = ((int64_t (*)(void*, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t))f)(NULL, a[0], a[1], a[2], a[3], a[4], a[5]); break;
        case 7: c->result = ((int64_t (*)(void*, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t))f)(NULL, a[0], a[1], a[2], a[3], a[4], a[5], a[6]); break;
        default: c->result = ((int64_t (*)(void*, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t))f)(NULL, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]); break;
    }
    c->done = 1;
    __p_co_leave();
}
#ifdef _WIN32
static void WINAPI __p_co_start(void* p) { __p_co_body((__p_co*)p); }
#elif defined(__APPLE__)
static void __p_co_body_arg(__p_co* c) __attribute__((used));
static void __p_co_body_arg(__p_co* c) { __p_co_body(c); }
#else
static void __p_co_start(unsigned lo, unsigned hi) { __p_co_body((__p_co*)(((uintptr_t)hi << 32) | lo)); }
#endif
static __p_co* __p_co_new(const char* name, int64_t nargs, const int64_t* args) {
    int64_t (*fn)() = NULL;
    for (int i = 0; i < p_thread_entry_count; i++) {
        if (strcmp(p_thread_entries[i].name, name) == 0) { fn = p_thread_entries[i].fn; break; }
    }
    if (!fn) p_panic("task: no such function");
#ifdef _WIN32
    if (!__p_co_sched) __p_co_sched = ConvertThreadToFiber(NULL);
#endif
    if (__p_co_count == __p_co_cap) {
        __p_co_cap = __p_co_cap ? __p_co_cap * 2 : 64;
        __p_cos = (__p_co**)realloc(__p_cos, (size_t)__p_co_cap * sizeof(__p_co*));
    }
    __p_co* c = (__p_co*)calloc(1, sizeof(__p_co));
    c->fn = fn; c->nargs = nargs; c->fd = -1;
    for (int64_t i = 0; i < nargs && i < 8; i++) c->args[i] = args[i];
#ifdef _WIN32
    c->fiber = CreateFiberEx(16384, 1024 * 1024, 0, __p_co_start, c);
    if (!c->fiber) p_panic("task: out of memory");
#else
    c->stack = (char*)mmap(NULL, P_CO_STACK, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (c->stack == MAP_FAILED) p_panic("task: out of memory");
    mprotect(c->stack, 4096, PROT_NONE);
#if defined(__APPLE__)
    {
        char* __top = c->stack + P_CO_STACK;
#if defined(__x86_64__)
        uint64_t* __fr = (uint64_t*)(__top - 64);
        __fr[0] = (uint64_t)(uintptr_t)c;
        __fr[6] = (uint64_t)(uintptr_t)__p_co_enter_start;
#else
        uint64_t* __fr = (uint64_t*)(__top - 112);
        memset(__fr, 0, 112);
        __fr[0] = (uint64_t)(uintptr_t)c;
        __fr[11] = (uint64_t)(uintptr_t)__p_co_enter_start;
#endif
        c->ctx.sp = __fr;
    }
#else
    getcontext(&c->ctx);
    c->ctx.uc_stack.ss_sp = c->stack;
    c->ctx.uc_stack.ss_size = P_CO_STACK;
    c->ctx.uc_link = NULL;
    makecontext(&c->ctx, (void (*)(void))__p_co_start, 2, (unsigned)(uintptr_t)c, (unsigned)((uintptr_t)c >> 32));
#endif
#endif
    __p_cos[__p_co_count++] = c;
    __p_co_total++;
    return c;
}
/* One turn of every task that can run. Returns how many are unfinished;
   ran says how many took their turn. */
static int64_t __p_co_round(int64_t* ran) {
    int64_t kept = 0;
    *ran = 0;
    for (int64_t i = 0; i < __p_co_count; i++) {
        __p_co* c = __p_cos[i];
        if (c->done) { if (c->detached) free(c); continue; }
        if (c->awaits && c->awaits->done) c->awaits = NULL;
        if (!c->awaits && c->fd < 0 && !c->wake_ms) {
            (*ran)++;
            __p_co_enter(c);
            if (c->done) { if (c->detached) free(c); continue; }
        }
        __p_cos[kept++] = c;
    }
    __p_co_count = kept;
    return kept;
}
/* Sleep until a waiting task can run, at most limit ms (-1: no limit). With
   ran set, only look. Panics if no task ever will. Wakes the earliest
   deadline only: the next pass takes the next, so timers fire in order. */
static void __p_co_wait(int64_t ran, int64_t limit) {
    int64_t n = 0;
    int64_t timeout = ran ? 0 : limit;
    int64_t first = 0;
    int64_t now = __p_co_now();
#ifdef _WIN32
    static P_THREAD_LOCAL WSAPOLLFD* fds = NULL;
#else
    static P_THREAD_LOCAL struct pollfd* fds = NULL;
#endif
    static P_THREAD_LOCAL int64_t cap = 0;
    if (__p_co_count > cap) {
        cap = __p_co_count * 2;
        fds = realloc(fds, (size_t)cap * sizeof(*fds));
    }
    for (int64_t i = 0; i < __p_co_count; i++) {
        __p_co* c = __p_cos[i];
        if (c->fd >= 0) {
            fds[n].fd = (c->fd);
            fds[n].events = c->fd_write ? POLLOUT : POLLIN;
            fds[n].revents = 0;
            n++;
        }
        if (c->wake_ms) {
            if (!first || c->wake_ms < first) first = c->wake_ms;
            int64_t left = c->wake_ms > now ? c->wake_ms - now : 0;
            if (timeout < 0 || left < timeout) timeout = left;
        }
    }
    if (n == 0 && timeout < 0) p_panic("Tasks are deadlocked: each one awaits another");
    if (n > 0) {
#ifdef _WIN32
        WSAPoll(fds, (ULONG)n, (INT)timeout);
#else
        poll(fds, (nfds_t)n, (int)timeout);
#endif
    } else if (timeout > 0) {
#ifdef _WIN32
        Sleep((DWORD)timeout);
#else
        poll(NULL, 0, (int)timeout);
#endif
    }
    now = __p_co_now();
    int64_t k = 0;
    for (int64_t i = 0; i < __p_co_count; i++) {
        __p_co* c = __p_cos[i];
        int hit = 0;
        if (c->fd >= 0) { hit = fds[k].revents != 0; k++; }
        if (hit) { c->ready = 1; c->fd = -1; c->wake_ms = 0; }
        else if (c->wake_ms == first && first && first <= now) { c->ready = 0; c->fd = -1; c->wake_ms = 0; }
    }
}
int64_t p_co_go(const char* name, int64_t arg) { return (int64_t)__p_co_new(name, 1, &arg); }
int64_t p_co_async(const char* name, int64_t nargs, ...) {
    int64_t args[8] = {0};
    va_list ap;
    va_start(ap, nargs);
    for (int64_t i = 0; i < nargs && i < 8; i++) args[i] = va_arg(ap, int64_t);
    va_end(ap);
    __p_co* c = __p_co_new(name, nargs, args);
    c->back = __p_co_cur;
    __p_co_enter(c);
    return (int64_t)((uint64_t)__P_CO_TAG << 48 | (uint64_t)(uintptr_t)c);
}
int64_t p_co_spawn(const char* name, int64_t nargs, ...) {
    va_list ap; va_start(ap, nargs);
    int64_t args[8];
    for (int64_t i = 0; i < nargs && i < 8; i++) args[i] = va_arg(ap, int64_t);
    va_end(ap);
    __p_co* c = __p_co_new(name, nargs, args);
    c->back = __p_co_cur;
    __p_co_enter(c);
    return (int64_t)((uint64_t)__P_CO_TAG << 48 | (uint64_t)(uintptr_t)c);
}
void p_co_tick(void) { if (__p_co_cur && __p_co_count > 1) __p_co_leave(); }
int64_t p_co_pool_call(const char* name, int64_t nargs, const int64_t* argv) {
    int64_t (*fn)() = NULL;
    for (int i = 0; i < p_thread_entry_count; i++) {
        if (strcmp(p_thread_entries[i].name, name) == 0) { fn = p_thread_entries[i].fn; break; }
    }
    if (!fn) { p_panic("pool call: no such function"); return 0; }
    const int64_t* a = argv;
    switch (nargs) {
        case 0: return ((int64_t (*)(void*))fn)(NULL);
        case 1: return ((int64_t (*)(void*, int64_t))fn)(NULL, a[0]);
        case 2: return ((int64_t (*)(void*, int64_t, int64_t))fn)(NULL, a[0], a[1]);
        case 3: return ((int64_t (*)(void*, int64_t, int64_t, int64_t))fn)(NULL, a[0], a[1], a[2]);
        default: return ((int64_t (*)(void*, int64_t, int64_t, int64_t, int64_t))fn)(NULL, a[0], a[1], a[2], a[3]);
    }
}
void p_co_yield(void) { if (__p_co_cur) __p_co_leave(); }
int64_t p_co_run(void) {
    for (;;) {
        int64_t ran;
        if (__p_co_round(&ran) == 0) return __p_co_total;
        __p_co_wait(ran, -1);
    }
}
static int64_t __p_co_await(int64_t future) {
    __p_co* c = (__p_co*)(uintptr_t)((uint64_t)future & 0xFFFFFFFFFFFFULL);
    while (!c->done) {
        if (__p_co_cur) {
            __p_co_cur->awaits = c;
            __p_co_leave();
        } else {
            int64_t ran;
            __p_co_round(&ran);
            if (!c->done) __p_co_wait(ran, -1);
        }
    }
    return c->result;
}
int64_t p_co_sleep(int64_t ms) {
    if (__p_co_cur) {
        __p_co_cur->wake_ms = __p_co_now() + (ms > 0 ? ms : 0) + 1;
        __p_co_leave();
        return 0;
    }
    int64_t end = __p_co_now() + ms;
    for (;;) {
        int64_t ran;
        int64_t live = __p_co_round(&ran);
        int64_t left = end - __p_co_now();
        if (left <= 0) return 0;
        if (live) __p_co_wait(ran, left);
        else {
#ifdef _WIN32
            Sleep((DWORD)left);
#else
            poll(NULL, 0, (int)left);
#endif
        }
    }
}
int64_t p_co_wait_fd(int64_t fd, int64_t write, int64_t timeout_ms) {
    if (__p_co_cur) {
        __p_co_cur->fd = fd;
        __p_co_cur->fd_write = write;
        __p_co_cur->wake_ms = timeout_ms >= 0 ? __p_co_now() + timeout_ms + 1 : 0;
        __p_co_leave();
        return __p_co_cur->ready;
    }
#ifdef _WIN32
    WSAPOLLFD one;
    one.fd = (SOCKET)fd; one.events = write ? POLLOUT : POLLIN; one.revents = 0;
    return WSAPoll(&one, 1, (INT)timeout_ms) > 0;
#else
    struct pollfd one;
    one.fd = (int)fd; one.events = write ? POLLOUT : POLLIN; one.revents = 0;
    return poll(&one, 1, (int)timeout_ms) > 0;
#endif
}
void p_co_detach(int64_t t) {
    __p_co* c = (__p_co*)(uintptr_t)t;
    if (c->done) free(c);
    else c->detached = 1;
}
int64_t p_co_done(int64_t t) { return (int64_t)((__p_co*)(uintptr_t)t)->done; }
int64_t p_co_result(int64_t t) { return ((__p_co*)(uintptr_t)t)->result; }
#ifdef _WIN32
#define __P_CO_BLOCKED() (WSAGetLastError() == WSAEWOULDBLOCK)
#else
#include <errno.h>
#include <fcntl.h>
#define __P_CO_BLOCKED() (errno == EAGAIN || errno == EWOULDBLOCK)
#endif
static void __p_co_nonblock(int64_t fd) {
#ifdef _WIN32
    u_long on = 1;
    ioctlsocket((SOCKET)fd, FIONBIO, &on);
#else
    fcntl((int)fd, F_SETFL, fcntl((int)fd, F_GETFL, 0) | O_NONBLOCK);
#endif
}
int64_t p_co_accept(int64_t fd) {
    p_co_wait_fd(fd, 0, -1);
    int64_t c = p_socket_accept(fd);
    if (c >= 0) __p_co_nonblock(c);
    return c;
}
#ifdef _WIN32
static void init_winsock(void);
#endif
int64_t p_co_connect(const char* host, int64_t port) {
#ifdef _WIN32
    init_winsock();
#endif
    char service[16];
    struct addrinfo hints, *res = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    snprintf(service, sizeof(service), "%lld", (long long)port);
    if (getaddrinfo(host, service, &hints, &res) != 0 || !res) return -1;
    int64_t s = (int64_t)socket(res->ai_family, res->ai_socktype, 0);
    if (s >= 0 && connect((SOCKET)s, res->ai_addr, (int)res->ai_addrlen) != 0) {
        p_socket_close(s);
        s = -1;
    }
    freeaddrinfo(res);
    if (s >= 0) __p_co_nonblock(s);
    return s;
}
int64_t p_co_recv(int64_t fd, int64_t buf, int64_t len, int64_t timeout_ms) {
    for (;;) {
        int64_t n = p_socket_read(fd, (char*)(uintptr_t)buf, len);
        if (n >= 0) return n;
        if (!__P_CO_BLOCKED()) return -1;
        if (!p_co_wait_fd(fd, 0, timeout_ms)) return -2;
    }
}
int64_t p_co_send(int64_t fd, int64_t buf, int64_t len) {
    int64_t done = 0;
    while (done < len) {
        int64_t n = p_socket_write(fd, (const char*)(uintptr_t)buf + done, len - done);
        if (n > 0) done += n;
        else if (n < 0 && !__P_CO_BLOCKED()) return -1;
        else if (!p_co_wait_fd(fd, 1, 30000)) return -1;
    }
    return done;
}
static void* p_thread_wrapper(void* arg) {
    p_thread_arg_t* ta = (p_thread_arg_t*)arg;
    __p_thread_guard_init();
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
    __p_thread_guard_done();
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
    if (((uint64_t)future >> 48) == 0x7FA5) return p_future_poll(future);
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
    if (((uint64_t)future >> 48) == 0x7FA5) return p_co_done(future & 0xFFFFFFFFFFFFLL) != 0;
    if (future >= 0) return true;
    int idx = -(future + 1);
    if (idx < 0 || idx >= p_thread_handle_count) return false;
    return atomic_load(&p_task_done_flags[idx]) != 0;
}
int64_t p_executor_block_on(int64_t exec, int64_t future) {
    (void)exec;
    return p_future_poll(future);
}
int64_t p_executor_cancel(int64_t exec) {
    (void)exec;
    int64_t count = 0;
    for (int i = 0; i < p_thread_handle_count; i++) {
        if (p_thread_handles[i] == 0) continue; // already joined
        int was_done = atomic_load(&p_task_done_flags[i]);
        void* ret = NULL; pthread_join(p_thread_handles[i], &ret);
        p_task_results[i] = was_done ? (int64_t)(intptr_t)ret : 0;
        if (!was_done) count++;
        p_thread_handles[i] = 0;
    }
    return count;
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
    count += p_co_run();
    return count;
}
int64_t p_task_poll_nonblocking(int64_t future) {
    if (((uint64_t)future >> 48) == 0x7FA5) return p_co_done(future & 0xFFFFFFFFFFFFLL) ? p_future_poll(future) : (int64_t)0x8000000000000001LL;
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
#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif
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
#ifdef _WIN32
    { WSADATA d; static int ws = 0; if (!ws) { WSAStartup(MAKEWORD(2,2), &d); ws = 1; } }
#endif
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
#ifdef _WIN32
    if (InetPtonA(AF_INET, host, &addr.sin_addr) != 1) {
#else
    if (inet_pton(AF_INET, host, &addr.sin_addr) != 1) {
#endif
        // Not a dotted IP — resolve via getaddrinfo.
        struct addrinfo hints = {0}, *res = NULL;
        hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
        char portbuf[16]; snprintf(portbuf, sizeof(portbuf), "%lld", (long long)port);
        if (getaddrinfo(host, portbuf, &hints, &res) != 0 || !res) return -1;
        addr = *(struct sockaddr_in*)res->ai_addr;
        freeaddrinfo(res);
    }
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return -1;
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
#ifdef _WIN32
    closesocket((SOCKET)sock);
#else
    close((int)sock);
#endif
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
int64_t p_shared_alloc(int64_t size) { if (size < 0) return 0; uint64_t* b = (uint64_t*)calloc(1, (size_t)size + 8); if (!b) { p_panic("p_shared_alloc: out of memory"); } b[0] = P_BLOCK_MAGIC | P_CLASS_LARGE; return (int64_t)(intptr_t)(b + 1); }
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
int64_t p_tls_create(void) { pthread_key_t key; pthread_key_create(&key, NULL); return (int64_t)key; }
int64_t p_tls_get(int64_t key) { return (int64_t)(intptr_t)pthread_getspecific((pthread_key_t)key); }
void p_tls_set(int64_t key, int64_t value) { pthread_setspecific((pthread_key_t)key, (void*)(intptr_t)value); }
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
// Simulated dlopen: returns sequential handles (1, 2, ...) to match interpreter
static int64_t dl_handle_counter = 0;
int64_t p_dl_open(const char* path) {
    if (!path || !*path) return 0;
    return ++dl_handle_counter;
}
int64_t p_dl_sym(int64_t handle, const char* symbol) {
    if (handle == 0) return 0;
    if (!symbol || !*symbol) return 0;
    return 0x1000 + handle * 0x100;
}
void p_dl_close(int64_t handle) {
    (void)handle; // no-op in simulation
}
int64_t p_dl_call(int64_t func_ptr, int64_t arg) {
    (void)func_ptr; // simulated: return arg unchanged
    return arg;
}
void* p_allocator_create(void* arg) { (void)arg; return (void*)1; }
int64_t p_allocator_alloc(void* alloc, int64_t size) { (void)alloc; if (size < 0) return 0; return (int64_t)(intptr_t)malloc((size_t)size); }
void p_allocator_free(void* alloc, int64_t ptr) { (void)alloc; free((void*)(intptr_t)ptr); }
int64_t p_allocator_stats(void* alloc) { (void)alloc; return 0; }
int64_t print(int64_t val) { char b[32]; int n = snprintf(b, sizeof b, "%lld", (long long)val); fwrite(b, 1, (size_t)n, stdout); return 0; }
int64_t print_str(const char* s) { fwrite(s, 1, strlen(s), stdout); return 0; }

bool p_string_contains(const char* s, const char* sub) { return s && sub && strstr(s, sub) != NULL; }
bool p_string_starts_with(const char* s, const char* prefix) {
    if (!s || !prefix) return false;
    size_t plen = strlen(prefix);
    return strncmp(s, prefix, plen) == 0;
}
bool p_string_ends_with(const char* s, const char* suffix) {
    if (!s || !suffix) return false;
    size_t slen = strlen(s), flen = strlen(suffix);
    if (flen > slen) return false;
    return strcmp(s + slen - flen, suffix) == 0;
}
int64_t p_string_find(const char* s, const char* sub) {
    if (!s || !sub) return -1;
    const char* p = strstr(s, sub);
    return p ? (int64_t)(p - s) : -1;
}
char* p_string_trim(const char* s) {
    if (!s) return strdup("");
    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') s++;
    size_t len = strlen(s);
    while (len > 0 && (s[len-1] == ' ' || s[len-1] == '\t' || s[len-1] == '\n' || s[len-1] == '\r')) len--;
    char* result = (char*)malloc(len + 1);
    memcpy(result, s, len);
    result[len] = 0;
    return result;
}
char* p_string_to_uppercase(const char* s) {
    if (!s) return strdup("");
    size_t len = strlen(s);
    char* result = (char*)malloc(len + 1);
    for (size_t i = 0; i < len; i++) result[i] = toupper((unsigned char)s[i]);
    result[len] = 0;
    return result;
}
char* p_string_to_lowercase(const char* s) {
    if (!s) return strdup("");
    size_t len = strlen(s);
    char* result = (char*)malloc(len + 1);
    for (size_t i = 0; i < len; i++) result[i] = tolower((unsigned char)s[i]);
    result[len] = 0;
    return result;
}
char* p_string_repeat(const char* s, int64_t n) {
    if (!s || n < 0) return strdup("");
    size_t slen = strlen(s);
    if (slen == 0 || n == 0) return strdup("");
    if (slen > SIZE_MAX / (size_t)n) return strdup("");
    size_t total = slen * (size_t)n + 1;
    char* result = (char*)malloc(total);
    if (!result) return strdup("");
    result[0] = 0;
    for (int64_t i = 0; i < n; i++) strcat(result, s);
    return result;
}
char* p_string_replace(const char* s, const char* from, const char* to) {
    if (!s) return strdup("");
    if (!from || !to || !*from) return strdup(s);
    size_t slen = strlen(s), flen = strlen(from), tlen = strlen(to);
    char* result = (char*)malloc(slen * (tlen > flen ? tlen : flen) + 1);
    size_t ri = 0; size_t si = 0;
    while (si < slen) {
        if (slen - si >= flen && strncmp(s + si, from, flen) == 0) {
            memcpy(result + ri, to, tlen); ri += tlen; si += flen;
        } else { result[ri++] = s[si++]; }
    }
    result[ri] = 0;
    return result;
}
int64_t p_string_split(const char* s, const char* delim) {
    PVec* v = p_vec_new();
    if (!s || !delim) return (int64_t)v;
    const char* start = s;
    size_t dlen = strlen(delim);
    while (*start) {
        const char* p = strstr(start, delim);
        if (!p) { p_vec_push(v, (void*)strdup(start)); break; }
        size_t len = p - start;
        char* part = (char*)malloc(len + 1);
        memcpy(part, start, len); part[len] = 0;
        p_vec_push(v, (void*)part);
        start = p + dlen;
    }
    return (int64_t)v;
}
int64_t p_string_parse_int(const char* s) { return s ? (int64_t)atoll(s) : 0; }
char* p_string_char_at(const char* s, int64_t idx) {
    if (!s) return strdup("");
    char* result = (char*)malloc(2);
    result[0] = s[idx]; result[1] = 0;
    return result;
}
char* p_string_substring(const char* s, int64_t start, int64_t end) {
    return p_string_slice(s, start, end);
}

int64_t p_vec_capacity(PVec* v) { return v ? v->cap : 0; }
PVec* p_vec_clone(PVec* v) {
    PVec* c = p_vec_new();
    for (int64_t i = 0; i < v->len; i++) p_vec_push(c, v->data[i]);
    return c;
}
bool p_vec_contains(PVec* v, void* val) {
    for (int64_t i = 0; i < v->len; i++) if (v->data[i] == val) return true;
    return false;
}
void p_vec_extend(PVec* v, PVec* other) {
    for (int64_t i = 0; i < other->len; i++) p_vec_push(v, other->data[i]);
}
void* p_vec_first(PVec* v) { return v->len > 0 ? v->data[0] : NULL; }
void* p_vec_last(PVec* v) { return v->len > 0 ? v->data[v->len - 1] : NULL; }
void p_vec_insert(PVec* v, int64_t idx, void* val) {
    if (idx < 0 || idx > v->len) return;
    if (v->len >= v->cap) { v->cap = v->cap == 0 ? 8 : v->cap * 2; v->data = (void**)realloc(v->data, v->cap * sizeof(void*)); }
    memmove(&v->data[idx+1], &v->data[idx], (v->len - idx) * sizeof(void*));
    v->data[idx] = val; v->len++;
}
void* p_vec_remove(PVec* v, int64_t idx) {
    if (idx < 0 || idx >= v->len) return NULL;
    void* val = v->data[idx];
    memmove(&v->data[idx], &v->data[idx+1], (v->len - idx - 1) * sizeof(void*));
    v->len--;
    return val;
}
void p_vec_reverse(PVec* v) {
    for (int64_t i = 0, j = v->len - 1; i < j; i++, j--) { void* t = v->data[i]; v->data[i] = v->data[j]; v->data[j] = t; }
}
void p_vec_sort(PVec* v) {
    for (int64_t i = 0; i < v->len; i++)
        for (int64_t j = i+1; j < v->len; j++)
            if ((int64_t)v->data[i] > (int64_t)v->data[j]) { void* t = v->data[i]; v->data[i] = v->data[j]; v->data[j] = t; }
}
void p_vec_truncate(PVec* v, int64_t len) { if (len < v->len) v->len = len; }

PMap* p_map_clone(PMap* m) {
    PMap* c = p_map_new();
    for (int64_t i = 0; i < m->len; i++) p_map_insert(c, m->keys[i], m->vals[i]);
    return c;
}
int64_t p_map_get_or(PMap* m, const char* key, int64_t default_val) {
    for (int64_t i = 0; i < m->len; i++) if (strcmp(m->keys[i], key) == 0) return m->vals[i];
    return default_val;
}

char* p_to_str_vec(PVec* v) {
    char* result = strdup("[");
    for (int64_t i = 0; i < v->len; i++) {
        if (i > 0) { char* tmp = p_str_concat(result, ", "); free(result); result = tmp; }
        char* s = p_to_str((int64_t)v->data[i]);
        char* tmp = p_str_concat(result, s); free(result); free(s); result = tmp;
    }
    char* tmp = p_str_concat(result, "]"); free(result); result = tmp;
    return result;
}
char* p_to_str_map(PMap* m) {
    if (!m) return strdup("{}");
    // Sort keys for deterministic output
    int64_t* order = (int64_t*)malloc(m->len * sizeof(int64_t));
    for (int64_t i = 0; i < m->len; i++) order[i] = i;
    for (int64_t i = 0; i < m->len; i++)
        for (int64_t j = i+1; j < m->len; j++)
            if (strcmp(m->keys[order[i]], m->keys[order[j]]) > 0) { int64_t t = order[i]; order[i] = order[j]; order[j] = t; }
    char* result = strdup("{");
    for (int64_t i = 0; i < m->len; i++) {
        if (i > 0) { char* tmp = p_str_concat(result, ", "); free(result); result = tmp; }
        char* tmp = p_str_concat(result, m->keys[order[i]]); free(result); result = tmp;
        tmp = p_str_concat(result, ": "); free(result); result = tmp;
        char* vs = p_to_str(m->vals[order[i]]);
        tmp = p_str_concat(result, vs); free(result); free(vs); result = tmp;
    }
    char* tmp = p_str_concat(result, "}"); free(result); result = tmp;
    free(order);
    return result;
}
char* p_to_str_str(const char* s) { return strdup(s); }
char* p_char_to_str(int64_t val) { char* s = (char*)malloc(2); s[0] = (char)val; s[1] = 0; return s; }
char* p_to_str_bool(bool val) { return strdup(val ? "true" : "false"); }
char* p_to_str_float(double val) {
    char* buf = (char*)malloc(512);
    // Match Rust's {} format: shortest representation that round-trips.
    // Rust's Display for f64 always uses fixed-point (never scientific notation).
    if (val == 0.0) { snprintf(buf, 512, "0"); return buf; }
    // Calculate required max precision from exponent.
    // For 2.22e-16, we need prec=32 (16 zeros + 16 sig digits).
    int exp = 0;
    double absval = fabs(val);
    if (absval >= 1.0) { while (absval >= 10.0) { absval /= 10.0; exp++; } }
    else { while (absval < 1.0) { absval *= 10.0; exp--; } }
    int max_prec = 17 + (exp < 0 ? -exp : 0);
    if (max_prec > 350) max_prec = 350;
    // Try increasing precision with %f format (no scientific notation)
    for (int prec = 0; prec <= max_prec; prec++) {
        snprintf(buf, 512, "%.*f", prec, val);
        double parsed = strtod(buf, NULL);
        if (parsed == val) {
            // Trim trailing zeros after decimal point (e.g., "10.0" -> "10")
            char* dot = strchr(buf, '.');
            if (dot) {
                char* end = buf + strlen(buf) - 1;
                while (end > dot && *end == '0') *end-- = '\0';
                if (end == dot) *end = '\0'; /* remove trailing dot */
            }
            return buf;
        }
    }
    // If no precision round-trips, use max precision
    snprintf(buf, 512, "%.*f", max_prec, val);
    return buf;
}
char* p_float_to_str(double val) { return p_to_str_float(val); }
char* p_bool_to_str(bool val) { return p_to_str_bool(val); }
char* p_vec_to_str(PVec* v) { return p_to_str_vec(v); }
double p_str_to_float(const char* s) { if (!s) return 0.0; return atof(s); }
int64_t p_f64_to_bits(double val) { union { double d; int64_t i; } u; u.d = val; return u.i; }
int64_t p_f32_to_bits(float val) { union { float f; int32_t i; } u; u.f = val; return (int64_t)u.i; }
double p_f64_from_bits(int64_t bits) { union { double d; int64_t i; } u; u.i = bits; return u.d; }
float p_f32_from_bits(int64_t bits) { union { float f; int32_t i; } u; u.i = (int32_t)bits; return u.f; }
double p_int_to_float(int64_t val) { return (double)val; }
int64_t p_float_to_int(double val) { return (int64_t)val; }
int64_t p_str_to_int(const char* s) { if (!s) return 0; return atoll(s); }

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
    if (n >= (int64_t)sizeof(buf)) n = sizeof(buf) - 1;
    buf[n] = 0;
    return strdup(buf);
}

// ─── Nonblocking sockets + readiness poll (async event loop) ───
int64_t p_socket_nonblock(int64_t fd, int64_t on) {
#ifdef _WIN32
    u_long m = on ? 1u : 0u;
    return ioctlsocket((SOCKET)fd, FIONBIO, &m);
#else
    int fl = fcntl((int)fd, F_GETFL, 0);
    if (fl < 0) return -1;
    return fcntl((int)fd, F_SETFL, on ? (fl | O_NONBLOCK) : (fl & ~O_NONBLOCK));
#endif
}
int64_t p_socket_poll(int64_t fd, int64_t timeout_ms, int64_t write_mode) {
    fd_set rset, wset; struct timeval tv;
    FD_ZERO(&rset); FD_ZERO(&wset);
    if (write_mode) FD_SET((SOCKET)fd, &wset); else FD_SET((SOCKET)fd, &rset);
    tv.tv_sec = (long)(timeout_ms / 1000);
    tv.tv_usec = (long)((timeout_ms % 1000) * 1000);
    int n = select((int)(fd + 1), &rset, &wset, NULL, timeout_ms < 0 ? NULL : &tv);
    return (int64_t)n;
}

// ─── Signals ───
#include <signal.h>
static volatile sig_atomic_t p_signal_flags[64] = {0};
static void p_signal_trampoline(int s) { if (s >= 0 && s < 64) p_signal_flags[s] = 1; }
int64_t p_signal_register(int64_t sig) {
    if (sig < 0 || sig >= 64) return -1;
    if (signal((int)sig, p_signal_trampoline) == SIG_ERR) return -1;
    return 0;
}
int64_t p_signal_check(int64_t sig) {
    if (sig < 0 || sig >= 64) return 0;
    int64_t f = p_signal_flags[sig]; p_signal_flags[sig] = 0;
    return f;
}
int64_t p_signal_raise(int64_t sig) { return raise((int)sig); }

// ─── Memory-mapped files / anonymous mappings ───
#ifndef _WIN32
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif
typedef struct { void* addr; int64_t len; int64_t h1; int64_t h2; int is_file; } p_mmap_entry_t;
static p_mmap_entry_t p_mmap_table[256];
static int p_mmap_find(void* addr) { for (int i = 0; i < 256; i++) if (p_mmap_table[i].addr == addr) return i; return -1; }
static int p_mmap_slot(void) { for (int i = 0; i < 256; i++) if (!p_mmap_table[i].addr) return i; return -1; }
int64_t p_mmap_anon(int64_t len) {
    if (len <= 0) return 0;
#ifdef _WIN32
    void* p = VirtualAlloc(NULL, (SIZE_T)len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    void* p = mmap(NULL, (size_t)len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) p = NULL;
#endif
    if (!p) return 0;
    int s = p_mmap_slot(); if (s < 0) return 0;
    p_mmap_table[s].addr = p; p_mmap_table[s].len = len; p_mmap_table[s].is_file = 0;
    return (int64_t)(intptr_t)p;
}
int64_t p_mmap_file(const char* path) {
#ifdef _WIN32
    HANDLE f = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f == INVALID_HANDLE_VALUE) f = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f == INVALID_HANDLE_VALUE) return 0;
    LARGE_INTEGER sz; if (!GetFileSizeEx(f, &sz) || sz.QuadPart == 0) { CloseHandle(f); return 0; }
    HANDLE m = CreateFileMappingA(f, NULL, PAGE_READWRITE, 0, 0, NULL);
    if (!m) m = CreateFileMappingA(f, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!m) { CloseHandle(f); return 0; }
    void* p = MapViewOfFile(m, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!p) p = MapViewOfFile(m, FILE_MAP_READ, 0, 0, 0);
    if (!p) { CloseHandle(m); CloseHandle(f); return 0; }
    int s = p_mmap_slot(); if (s < 0) return 0;
    p_mmap_table[s].addr = p; p_mmap_table[s].len = (int64_t)sz.QuadPart;
    p_mmap_table[s].h1 = (int64_t)(intptr_t)m; p_mmap_table[s].h2 = (int64_t)(intptr_t)f;
    p_mmap_table[s].is_file = 1;
    return (int64_t)(intptr_t)p;
#else
    int fd = open(path, O_RDWR); if (fd < 0) fd = open(path, O_RDONLY);
    if (fd < 0) return 0;
    struct stat st; if (fstat(fd, &st) < 0 || st.st_size == 0) { close(fd); return 0; }
    void* p = mmap(NULL, (size_t)st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) p = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) { close(fd); return 0; }
    int s = p_mmap_slot(); if (s < 0) return 0;
    p_mmap_table[s].addr = p; p_mmap_table[s].len = (int64_t)st.st_size;
    p_mmap_table[s].h1 = fd; p_mmap_table[s].is_file = 1;
    return (int64_t)(intptr_t)p;
#endif
}
int64_t p_mmap_len(int64_t addr) { int s = p_mmap_find((void*)(intptr_t)addr); return s < 0 ? 0 : p_mmap_table[s].len; }
int64_t p_munmap(int64_t addr) {
    int s = p_mmap_find((void*)(intptr_t)addr); if (s < 0) return -1;
    p_mmap_entry_t e = p_mmap_table[s]; p_mmap_table[s].addr = NULL;
#ifdef _WIN32
    if (e.is_file) { UnmapViewOfFile(e.addr); CloseHandle((HANDLE)(intptr_t)e.h1); CloseHandle((HANDLE)(intptr_t)e.h2); }
    else VirtualFree(e.addr, 0, MEM_RELEASE);
#else
    munmap(e.addr, (size_t)e.len); if (e.is_file) close((int)e.h1);
#endif
    return 0;
}
int64_t p_msync(int64_t addr) {
    int s = p_mmap_find((void*)(intptr_t)addr); if (s < 0) return -1;
#ifdef _WIN32
    return FlushViewOfFile(p_mmap_table[s].addr, 0) ? 0 : -1;
#else
    return msync(p_mmap_table[s].addr, (size_t)p_mmap_table[s].len, MS_SYNC);
#endif
}

// Bare metal stubs — no-ops in C codegen (use --bare for real hardware)
void p_cli(void) {}
void p_sti(void) {}
void p_hlt(void) {}
int64_t p_cr0_read(void) { return 0; }
int64_t p_cr3_read(void) { return 0; }
int64_t p_cr4_read(void) { return 0; }
void p_cr3_write(int64_t val) { (void)val; }
void p_invlpg(int64_t addr) { (void)addr; }
void p_page_map(int64_t cr3, int64_t virt, int64_t phys, int64_t flags) { (void)cr3; (void)virt; (void)phys; (void)flags; }
void p_page_unmap(int64_t cr3, int64_t virt) { (void)cr3; (void)virt; }
static int64_t _phys_mem[65536];
int64_t p_phys_read64(int64_t addr) { return _phys_mem[(addr/8) & 65535]; }
void p_phys_write64(int64_t addr, int64_t val) { _phys_mem[(addr/8) & 65535] = val; }
int64_t p_volatile_load(int64_t addr) { volatile int64_t* p = (volatile int64_t*)(intptr_t)addr; return *p; }
void p_volatile_store(int64_t addr, int64_t val) { volatile int64_t* p = (volatile int64_t*)(intptr_t)addr; *p = val; }
int64_t p_volatile_load8(int64_t addr) { volatile uint8_t* p = (volatile uint8_t*)(intptr_t)addr; return (int64_t)*p; }
int64_t p_volatile_load16(int64_t addr) { volatile uint16_t* p = (volatile uint16_t*)(intptr_t)addr; return (int64_t)*p; }
int64_t p_volatile_load32(int64_t addr) { volatile uint32_t* p = (volatile uint32_t*)(intptr_t)addr; return (int64_t)*p; }
void p_volatile_store8(int64_t addr, int64_t val) { volatile uint8_t* p = (volatile uint8_t*)(intptr_t)addr; *p = (uint8_t)val; }
void p_volatile_store16(int64_t addr, int64_t val) { volatile uint16_t* p = (volatile uint16_t*)(intptr_t)addr; *p = (uint16_t)val; }
void p_volatile_store32(int64_t addr, int64_t val) { volatile uint32_t* p = (volatile uint32_t*)(intptr_t)addr; *p = (uint32_t)val; }
void p_lgdt(int64_t gdt_desc) { (void)gdt_desc; }
void p_gdt_set_entry(int64_t gdt_ptr, int64_t index, int64_t base, int64_t limit, int64_t flags) { (void)gdt_ptr; (void)index; (void)base; (void)limit; (void)flags; }
void p_serial_init(void) {}
void p_serial_write(int64_t c) { (void)c; }
int64_t p_serial_read(void) { return 0; }
int64_t p_serial_ready(void) { return 0; }

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
int64_t p_simd_add(int64_t a, int64_t b) {
    int64_t r = _fvec_new();
    int n = (int)p_vec_len((PVec*)a);
    for (int i = 0; i < n; i++) _fvec_push_f64(r, _fvec_get_f64(a,i) + _fvec_get_f64(b,i));
    return r;
}
int64_t p_simd_sub(int64_t a, int64_t b) {
    int64_t r = _fvec_new();
    int n = (int)p_vec_len((PVec*)a);
    for (int i = 0; i < n; i++) _fvec_push_f64(r, _fvec_get_f64(a,i) - _fvec_get_f64(b,i));
    return r;
}
int64_t p_simd_mul(int64_t a, int64_t b) {
    int64_t r = _fvec_new();
    int n = (int)p_vec_len((PVec*)a);
    for (int i = 0; i < n; i++) _fvec_push_f64(r, _fvec_get_f64(a,i) * _fvec_get_f64(b,i));
    return r;
}
int64_t p_simd_div(int64_t a, int64_t b) {
    int64_t r = _fvec_new();
    int n = (int)p_vec_len((PVec*)a);
    for (int i = 0; i < n; i++) { double db = _fvec_get_f64(b,i); _fvec_push_f64(r, db != 0 ? _fvec_get_f64(a,i) / db : 0); }
    return r;
}
double p_simd_dot(int64_t a, int64_t b) {
    int n = (int)p_vec_len((PVec*)a); double s = 0;
    for (int i = 0; i < n; i++) s += _fvec_get_f64(a,i) * _fvec_get_f64(b,i);
    return s;
}
double p_simd_sum(int64_t a) {
    int n = (int)p_vec_len((PVec*)a); double s = 0;
    for (int i = 0; i < n; i++) s += _fvec_get_f64(a,i);
    return s;
}
int64_t p_simd_scale(int64_t a, double s) {
    int64_t r = _fvec_new();
    int n = (int)p_vec_len((PVec*)a);
    for (int i = 0; i < n; i++) _fvec_push_f64(r, _fvec_get_f64(a,i) * s);
    return r;
}
int64_t p_simd_from_vec(int64_t v) {
    int64_t r = _fvec_new();
    int n = (int)p_vec_len((PVec*)v);
    for (int i = 0; i < n; i++) _fvec_push_f64(r, (double)(int64_t)p_vec_get((PVec*)v, i));
    return r;
}
char* p_simd_to_str(int64_t a) {
    if (!a) return strdup("[]");
    size_t cap = 64; char* result = (char*)malloc(cap); size_t pos = 0;
    result[pos++] = '[';
    int64_t n = p_vec_len((PVec*)a);
    for (int64_t i = 0; i < n; i++) {
        if (i > 0) { if (pos+2>=cap){cap*=2;result=(char*)realloc(result,cap);} result[pos++]=','; result[pos++]=' '; }
        double d = _fvec_get_f64(a, i);
        char buf[32];
        if (d == (int64_t)d) snprintf(buf,sizeof(buf),"%lld",(long long)d);
        else snprintf(buf,sizeof(buf),"%g",d);
        size_t bl=strlen(buf);
        if (pos+bl+1>=cap){cap=(cap+bl)*2;result=(char*)realloc(result,cap);}
        memcpy(result+pos,buf,bl); pos+=bl;
    }
    if (pos+2>=cap){cap+=2;result=(char*)realloc(result,cap);}
    result[pos++]=']'; result[pos]=0;
    return result;
}

// Tensor builtins — simple flat array with shape metadata
// PTensor typedef already emitted in header section
typedef enum { AG_ADD, AG_SUB, AG_MUL, AG_MATMUL, AG_SCALE, AG_RELU, AG_SIGMOID, AG_SUM, AG_MEAN, AG_NEG } AGOpKind;
typedef struct {
    AGOpKind op;
    void* result;  void* a;  void* b;  double scalar;
    int64_t a_shape[8]; int64_t a_ndim;
    int64_t b_shape[8]; int64_t b_ndim;
    double* stored_data; int64_t stored_len; int is_checkpoint;
} AGEntry;
static AGEntry* ag_tape = NULL;
static int64_t ag_tape_cap = 0;
static int64_t ag_tape_len = 0;
static int ag_mode = 0;
static int ag_checkpoint_mode = 0;
static void** ag_grad_ptrs = NULL;
static PTensor** ag_grads = NULL;
static int64_t ag_grad_cap = 0;
static int64_t ag_grad_len = 0;
static double ag_dp_budget = 0.0;
static double ag_dp_limit = 0.0;
static int ag_tp_rank = 0;
static int ag_tp_world_size = 1;
static PTensor* ag_get_grad(void* ptr) {
    for (int64_t i = 0; i < ag_grad_len; i++) if (ag_grad_ptrs[i] == ptr) return ag_grads[i];
    return NULL;
}
static void ag_accum_grad(void* ptr, PTensor* grad) {
    PTensor* ex = ag_get_grad(ptr);
    if (ex) { for (int64_t i = 0; i < ex->len && i < grad->len; i++) ex->data[i] += grad->data[i]; }
    else {
        if (ag_grad_len >= ag_grad_cap) {
            int64_t new_cap = ag_grad_cap == 0 ? 64 : ag_grad_cap * 2;
            ag_grad_ptrs = (void**)realloc(ag_grad_ptrs, new_cap * sizeof(void*));
            ag_grads = (PTensor**)realloc(ag_grads, new_cap * sizeof(PTensor*));
            ag_grad_cap = new_cap;
        }
        ag_grad_ptrs[ag_grad_len] = ptr; ag_grads[ag_grad_len] = grad; ag_grad_len++;
    }
}
static PTensor* ag_clone_tensor(PTensor* t) {
    if (!t) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = t->ndim; r->len = t->len;
    for (int64_t i = 0; i < t->ndim && i < 8; i++) r->shape[i] = t->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = t->data[i];
    return r;
}
static PTensor* ag_zeros_like(PTensor* t) {
    if (!t) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = t->ndim; r->len = t->len;
    for (int64_t i = 0; i < t->ndim && i < 8; i++) r->shape[i] = t->shape[i];
    r->data = (double*)calloc(r->len, sizeof(double));
    return r;
}
static PTensor* ag_filled_shape(int64_t* shape, int64_t ndim, double val) {
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = ndim; int64_t len = 1;
    for (int64_t i = 0; i < ndim && i < 8; i++) { r->shape[i] = shape[i]; len *= shape[i]; }
    r->len = len; r->data = (double*)malloc(len * sizeof(double));
    for (int64_t i = 0; i < len; i++) r->data[i] = val;
    return r;
}
static void ag_record(AGOpKind op, void* result, void* a, void* b, double s) {
    if (!ag_mode) return;
    if (ag_tape_len >= ag_tape_cap) {
        int64_t new_cap = ag_tape_cap == 0 ? 64 : ag_tape_cap * 2;
        ag_tape = (AGEntry*)realloc(ag_tape, new_cap * sizeof(AGEntry));
        ag_tape_cap = new_cap;
    }
    AGEntry* e = &ag_tape[ag_tape_len++];
    e->op = op; e->result = result; e->a = a; e->b = b; e->scalar = s;
    e->stored_data = NULL; e->stored_len = 0; e->is_checkpoint = 0;
    PTensor* ta = (PTensor*)a;
    if (ta) { e->a_ndim = ta->ndim; for (int64_t i = 0; i < ta->ndim && i < 8; i++) e->a_shape[i] = ta->shape[i]; } else e->a_ndim = 0;
    PTensor* tb = (PTensor*)b;
    if (tb) { e->b_ndim = tb->ndim; for (int64_t i = 0; i < tb->ndim && i < 8; i++) e->b_shape[i] = tb->shape[i]; } else e->b_ndim = 0;
}
static AGEntry* ag_grow(void) {
    if (!ag_mode) return NULL;
    if (ag_tape_len >= ag_tape_cap) {
        int64_t new_cap = ag_tape_cap == 0 ? 64 : ag_tape_cap * 2;
        AGEntry* nt = (AGEntry*)realloc(ag_tape, new_cap * sizeof(AGEntry));
        if (!nt) return NULL;
        ag_tape = nt; ag_tape_cap = new_cap;
    }
    return &ag_tape[ag_tape_len++];
}
void* p_tensor_new(int64_t ndim, int64_t* shape) {
    PTensor* t = (PTensor*)malloc(sizeof(PTensor));
    t->ndim = ndim;
    t->len = 1;
    for (int64_t i = 0; i < ndim && i < 8; i++) { t->shape[i] = shape[i]; t->len *= shape[i]; }
    t->data = (double*)calloc(t->len, sizeof(double));
    return t;
}
void* p_tensor_from(void* data, int64_t ndim, int64_t* shape) {
    (void)data;
    return p_tensor_new(ndim, shape);
}
double p_tensor_at(void* t, int64_t idx) {
    PTensor* pt = (PTensor*)t;
    if (!pt || idx < 0 || idx >= pt->len) return 0;
    return pt->data[idx];
}
void* p_tensor_add(void* a, void* b) {
    PTensor* ta = (PTensor*)a; PTensor* tb = (PTensor*)b;
    if (!ta || !tb) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = ta->ndim; r->len = ta->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = ta->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = ta->data[i] + tb->data[i];
    ag_record(AG_ADD, r, a, b, 0);
    return r;
}
void* p_tensor_mul(void* a, void* b) {
    PTensor* ta = (PTensor*)a; PTensor* tb = (PTensor*)b;
    if (!ta || !tb) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = ta->ndim; r->len = ta->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = ta->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = ta->data[i] * tb->data[i];
    ag_record(AG_MUL, r, a, b, 0);
    return r;
}
void* p_tensor_matmul(void* a, void* b) {
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
    ag_record(AG_MATMUL, r, a, b, 0);
    return r;
}
void* p_tensor_transpose(void* t) {
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
}
void* p_tensor_relu(void* t) {
    PTensor* pt = (PTensor*)t;
    if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = pt->data[i] > 0 ? pt->data[i] : 0;
    { AGEntry* e = ag_grow();
        if (e) {
        e->op = AG_RELU; e->result = r; e->a = t; e->b = NULL; e->scalar = 0;
        e->a_ndim = pt->ndim; for (int64_t i = 0; i < pt->ndim && i < 8; i++) e->a_shape[i] = pt->shape[i];
        e->b_ndim = 0;
        if (ag_checkpoint_mode) { e->is_checkpoint = 1; e->stored_len = pt->len; e->stored_data = (double*)malloc(pt->len * sizeof(double)); for (int64_t i = 0; i < pt->len; i++) e->stored_data[i] = pt->data[i]; }
        else { e->is_checkpoint = 0; e->stored_len = r->len; e->stored_data = (double*)malloc(r->len * sizeof(double)); for (int64_t i = 0; i < r->len; i++) e->stored_data[i] = r->data[i]; }
        }
    }
    return r;
}
void* p_tensor_softmax(void* t) {
    PTensor* pt = (PTensor*)t;
    if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    double max = pt->data[0];
    for (int64_t i = 1; i < pt->len; i++) if (pt->data[i] > max) max = pt->data[i];
    double sum = 0;
    for (int64_t i = 0; i < pt->len; i++) { r->data[i] = exp(pt->data[i] - max); sum += r->data[i]; }
    for (int64_t i = 0; i < pt->len; i++) r->data[i] /= sum;
    return r;
}
int64_t p_tensor_len(void* t) { PTensor* pt = (PTensor*)t; return pt ? pt->len : 0; }
int64_t p_tensor_ndim(void* t) { PTensor* pt = (PTensor*)t; return pt ? pt->ndim : 0; }
void* p_tensor_shape(void* t) {
    PTensor* pt = (PTensor*)t;
    PVec* v = (PVec*)p_vec_new();
    if (pt) for (int64_t i = 0; i < pt->ndim; i++) { int64_t d = pt->shape[i]; p_vec_push(v, (void*)d); }
    return v;
}
int64_t p_tensor_size(void* t) { return p_tensor_len(t); }
void* p_tensor_zeros(void* shape_vec) {
    PVec* sv = (PVec*)shape_vec;
    if (!sv) return p_tensor_new(0, NULL);
    int64_t shape[8]; int64_t ndim = sv->len < 8 ? sv->len : 8;
    for (int64_t i = 0; i < ndim; i++) shape[i] = (int64_t)sv->data[i];
    return p_tensor_new(ndim, shape);
}
void* p_tensor_ones(void* shape_vec) {
    PVec* sv = (PVec*)shape_vec;
    if (!sv) return p_tensor_new(0, NULL);
    int64_t shape[8]; int64_t ndim = sv->len < 8 ? sv->len : 8;
    for (int64_t i = 0; i < ndim; i++) shape[i] = (int64_t)sv->data[i];
    PTensor* t = (PTensor*)p_tensor_new(ndim, shape);
    if (t) for (int64_t i = 0; i < t->len; i++) t->data[i] = 1.0;
    return t;
}
void* p_tensor_filled(void* shape_vec, double val) {
    PVec* sv = (PVec*)shape_vec;
    if (!sv) return p_tensor_new(0, NULL);
    int64_t shape[8]; int64_t ndim = sv->len < 8 ? sv->len : 8;
    for (int64_t i = 0; i < ndim; i++) shape[i] = (int64_t)sv->data[i];
    PTensor* t = (PTensor*)p_tensor_new(ndim, shape);
    if (t) for (int64_t i = 0; i < t->len; i++) t->data[i] = val;
    return t;
}
void* p_tensor_from_vec(void* v, void* shape_vec) {
    PVec* sv = (PVec*)shape_vec;
    if (!sv) return p_tensor_new(0, NULL);
    int64_t shape[8]; int64_t ndim = sv->len < 8 ? sv->len : 8;
    for (int64_t i = 0; i < ndim; i++) shape[i] = (int64_t)sv->data[i];
    PTensor* t = (PTensor*)p_tensor_new(ndim, shape);
    PVec* pv = (PVec*)v;
    if (t && pv) for (int64_t i = 0; i < t->len && i < pv->len; i++) t->data[i] = (double)(int64_t)pv->data[i];
    return t;
}
void* p_tensor_clone(void* t) {
    PTensor* pt = (PTensor*)t;
    if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = pt->data[i];
    return r;
}
double p_tensor_get(void* t, void* idx_vec) {
    PTensor* pt = (PTensor*)t;
    PVec* iv = (PVec*)idx_vec;
    if (!pt || !iv || iv->len == 0) return 0;
    int64_t idx = 0;
    for (int64_t i = 0; i < pt->ndim && i < (int64_t)iv->len; i++) {
        int64_t dim = (int64_t)iv->data[i];
        int64_t stride = 1;
        for (int64_t j = i+1; j < pt->ndim; j++) stride *= pt->shape[j];
        idx += dim * stride;
    }
    if (idx < 0 || idx >= pt->len) return 0;
    return pt->data[idx];
}
void p_tensor_set(void* t, void* idx_vec, double val) {
    PTensor* pt = (PTensor*)t;
    PVec* iv = (PVec*)idx_vec;
    if (!pt || !iv || iv->len == 0) return;
    int64_t idx = 0;
    for (int64_t i = 0; i < pt->ndim && i < (int64_t)iv->len; i++) {
        int64_t dim = (int64_t)iv->data[i];
        int64_t stride = 1;
        for (int64_t j = i+1; j < pt->ndim; j++) stride *= pt->shape[j];
        idx += dim * stride;
    }
    if (idx < 0 || idx >= pt->len) return;
    pt->data[idx] = val;
}
void* p_tensor_sub(void* a, void* b) {
    PTensor* ta = (PTensor*)a; PTensor* tb = (PTensor*)b;
    if (!ta || !tb) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = ta->ndim; r->len = ta->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = ta->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = ta->data[i] - tb->data[i];
    return r;
}
void* p_tensor_sigmoid(void* t) {
    PTensor* pt = (PTensor*)t;
    if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = 1.0 / (1.0 + exp(-pt->data[i]));
    { AGEntry* e = ag_grow();
        if (e) {
        e->op = AG_SIGMOID; e->result = r; e->a = t; e->b = NULL; e->scalar = 0;
        e->a_ndim = pt->ndim; for (int64_t i = 0; i < pt->ndim && i < 8; i++) e->a_shape[i] = pt->shape[i];
        e->b_ndim = 0;
        if (ag_checkpoint_mode) { e->is_checkpoint = 1; e->stored_len = pt->len; e->stored_data = (double*)malloc(pt->len * sizeof(double)); for (int64_t i = 0; i < pt->len; i++) e->stored_data[i] = pt->data[i]; }
        else { e->is_checkpoint = 0; e->stored_len = r->len; e->stored_data = (double*)malloc(r->len * sizeof(double)); for (int64_t i = 0; i < r->len; i++) e->stored_data[i] = r->data[i]; }
        }
    }
    return r;
}
void* p_tensor_scale(void* t, double s) {
    PTensor* pt = (PTensor*)t;
    if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = pt->data[i] * s;
    ag_record(AG_SCALE, r, t, NULL, s);
    return r;
}
double p_tensor_sum(void* t) { PTensor* pt = (PTensor*)t; if (!pt) return 0; double s=0; for (int64_t i=0;i<pt->len;i++) s+=pt->data[i]; ag_record(AG_SUM, (void*)pt, t, NULL, 0); return s; }
double p_tensor_mean(void* t) { PTensor* pt = (PTensor*)t; if (!pt || pt->len==0) return 0; double r=p_tensor_sum(t)/pt->len; if (ag_mode && ag_tape_len > 0) { AGEntry* e=&ag_tape[ag_tape_len-1]; e->op=AG_MEAN; e->scalar=(double)pt->len; } return r; }
double p_tensor_min(void* t) { PTensor* pt = (PTensor*)t; if (!pt || pt->len==0) return 0; double m=pt->data[0]; for (int64_t i=1;i<pt->len;i++) if (pt->data[i]<m) m=pt->data[i]; return m; }
double p_tensor_max(void* t) { PTensor* pt = (PTensor*)t; if (!pt || pt->len==0) return 0; double m=pt->data[0]; for (int64_t i=1;i<pt->len;i++) if (pt->data[i]>m) m=pt->data[i]; return m; }
double p_tensor_dot(void* a, void* b) { PTensor* ta=(PTensor*)a; PTensor* tb=(PTensor*)b; if (!ta||!tb) return 0; double s=0; for (int64_t i=0;i<ta->len&&i<tb->len;i++) s+=ta->data[i]*tb->data[i]; return s; }
void* p_tensor_reshape(void* t, void* shape_vec) {
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    PVec* sv = (PVec*)shape_vec;
    PTensor* r = (PTensor*)p_tensor_clone(t);
    if (sv) {
        r->ndim = sv->len < 8 ? sv->len : 8; r->len = 1;
        for (int64_t i = 0; i < r->ndim; i++) { r->shape[i] = (int64_t)sv->data[i]; r->len *= r->shape[i]; }
    }
    return r;
}
char* p_tensor_to_json(void* t) {
    // Same text as the interpreter (src/tensor.rs to_json): integral
    // values print as integers, others in shortest fixed-point form.
    PTensor* pt = (PTensor*)t;
    if (!pt) return strdup("{}");
    size_t cap = 64 + (size_t)pt->ndim * 24 + (size_t)pt->len * 8; char* result = (char*)malloc(cap); size_t pos = 0;
    pos += snprintf(result+pos, cap-pos, "{\"shape\":[");
    for (int64_t i = 0; i < pt->ndim; i++) {
        if (i > 0) result[pos++] = ',';
        pos += snprintf(result+pos, cap-pos, "%lld", (long long)pt->shape[i]);
    }
    pos += snprintf(result+pos, cap-pos, "],\"data\":[");
    for (int64_t i = 0; i < pt->len; i++) {
        double d = pt->data[i];
        char nb[40]; const char* s;
        if (d == trunc(d) && isfinite(d) && fabs(d) < 1e15) { snprintf(nb, sizeof nb, "%lld", (long long)d); s = nb; }
        else s = p_to_str_float(d);
        size_t l = strlen(s);
        if (pos + l + 4 > cap) { while (pos + l + 4 > cap) cap *= 2; result = (char*)realloc(result, cap); }
        if (i > 0) result[pos++] = ',';
        memcpy(result+pos, s, l); pos += l;
    }
    result[pos++] = ']'; result[pos++] = '}'; result[pos] = 0;
    return result;
}
char* p_tensor_to_str(void* t) {
    PTensor* pt = (PTensor*)t;
    if (!pt) return strdup("[]");
    size_t cap = 128; char* result = (char*)malloc(cap); size_t pos = 0;
    if (pt->ndim <= 1) {
        result[pos++] = '[';
        for (int64_t i = 0; i < pt->len; i++) {
            if (i > 0) { if (pos+2>=cap){cap*=2;result=(char*)realloc(result,cap);} result[pos++]=','; result[pos++]=' '; }
            char buf[32]; double _v=pt->data[i]; if (_v==floor(_v)&&isfinite(_v)&&fabs(_v)<1e15) snprintf(buf,sizeof(buf),"%lld",(long long)_v); else snprintf(buf,sizeof(buf),"%.4f",_v); size_t bl=strlen(buf);
            if (pos+bl+1>=cap){cap=(cap+bl)*2;result=(char*)realloc(result,cap);}
            memcpy(result+pos,buf,bl); pos+=bl;
        }
        if (pos+2>=cap){cap+=2;result=(char*)realloc(result,cap);}
        result[pos++]=']'; result[pos]=0;
        return result;
    }
    // 2D tensor: print as nested brackets
    int64_t rows = pt->shape[0], cols = pt->ndim >= 2 ? pt->shape[1] : 1;
    result[pos++] = '[';
    for (int64_t r = 0; r < rows; r++) {
        if (r > 0) { if (pos+2>=cap){cap*=2;result=(char*)realloc(result,cap);} result[pos++]=','; result[pos++]=' '; }
        if (pos+1>=cap){cap+=2;result=(char*)realloc(result,cap);} result[pos++]='[';
        for (int64_t c = 0; c < cols; c++) {
            if (c > 0) { if (pos+2>=cap){cap*=2;result=(char*)realloc(result,cap);} result[pos++]=','; result[pos++]=' '; }
            char buf[32]; double _v=pt->data[r*cols+c]; if (_v==floor(_v)&&isfinite(_v)&&fabs(_v)<1e15) snprintf(buf,sizeof(buf),"%lld",(long long)_v); else snprintf(buf,sizeof(buf),"%.4f",_v); size_t bl=strlen(buf);
            if (pos+bl+1>=cap){cap=(cap+bl)*2;result=(char*)realloc(result,cap);}
            memcpy(result+pos,buf,bl); pos+=bl;
        }
        if (pos+1>=cap){cap+=2;result=(char*)realloc(result,cap);} result[pos++]=']';
    }
    if (pos+2>=cap){cap+=2;result=(char*)realloc(result,cap);}
    result[pos++]=']'; result[pos]=0;
    return result;
}

void p_autograd_begin(void) {
    ag_mode = 1; ag_tape_len = 0; ag_grad_len = 0;
}
void p_autograd_end(void) {
    ag_mode = 0;
}
void p_autograd_backward(void* loss) {
    (void)loss; // seed is always 1.0 (d(loss)/d(loss) = 1)
    double seed = 1.0;
    // Find the last scalar-producing op (sum/mean) to seed
    int64_t seeded_idx = -1;
    for (int64_t i = ag_tape_len - 1; i >= 0; i--) {
        if (ag_tape[i].op == AG_SUM) {
            PTensor* g = ag_filled_shape(ag_tape[i].a_shape, ag_tape[i].a_ndim, seed);
            ag_accum_grad(ag_tape[i].a, g); seeded_idx = i; break;
        }
        if (ag_tape[i].op == AG_MEAN) {
            PTensor* g = ag_filled_shape(ag_tape[i].a_shape, ag_tape[i].a_ndim, seed / ag_tape[i].scalar);
            ag_accum_grad(ag_tape[i].a, g); seeded_idx = i; break;
        }
    }
    if (seeded_idx < 0 && ag_tape_len > 0) {
        PTensor* g = (PTensor*)malloc(sizeof(PTensor)); g->ndim=1; g->len=1; g->shape[0]=1; g->data=(double*)malloc(sizeof(double)); g->data[0]=seed;
        ag_accum_grad(ag_tape[ag_tape_len-1].result, g);
    }
    // Walk tape in reverse
    for (int64_t i = ag_tape_len - 1; i >= 0; i--) {
        if (i == seeded_idx) continue; // skip already-seeded op
        AGEntry* e = &ag_tape[i];
        PTensor* grad_out = ag_get_grad(e->result);
        if (!grad_out) continue;
        switch (e->op) {
            case AG_ADD: {
                ag_accum_grad(e->a, ag_clone_tensor(grad_out));
                ag_accum_grad(e->b, ag_clone_tensor(grad_out));
                break;
            }
            case AG_SUB: {
                ag_accum_grad(e->a, ag_clone_tensor(grad_out));
                PTensor* ng = ag_clone_tensor(grad_out); for (int64_t j=0;j<ng->len;j++) ng->data[j]=-ng->data[j];
                ag_accum_grad(e->b, ng);
                break;
            }
            case AG_MUL: {
                ag_accum_grad(e->a, ag_clone_tensor(grad_out));
                ag_accum_grad(e->b, ag_clone_tensor(grad_out));
                break;
            }
            case AG_MATMUL: {
                PTensor* ag = ag_filled_shape(e->a_shape, e->a_ndim, 1.0);
                PTensor* bg = ag_filled_shape(e->b_shape, e->b_ndim, 1.0);
                ag_accum_grad(e->a, ag); ag_accum_grad(e->b, bg);
                break;
            }
            case AG_SCALE: {
                PTensor* ga = ag_clone_tensor(grad_out); for (int64_t j=0;j<ga->len;j++) ga->data[j]*=e->scalar;
                ag_accum_grad(e->a, ga);
                break;
            }
            case AG_RELU: {
                // Recompute mask from stored data
                PTensor* ga = ag_clone_tensor(grad_out);
                if (e->stored_data) {
                    for (int64_t j=0;j<ga->len && j<e->stored_len;j++) {
                        double val = e->is_checkpoint ? (e->stored_data[j] > 0 ? e->stored_data[j] : 0) : e->stored_data[j];
                        ga->data[j] *= (val > 0) ? 1.0 : 0.0;
                    }
                }
                ag_accum_grad(e->a, ga);
                break;
            }
            case AG_SIGMOID: {
                PTensor* ga = ag_clone_tensor(grad_out);
                if (e->stored_data) {
                    for (int64_t j=0;j<ga->len && j<e->stored_len;j++) {
                        double s = e->is_checkpoint ? (1.0/(1.0+exp(-e->stored_data[j]))) : e->stored_data[j];
                        ga->data[j] *= s * (1.0 - s);
                    }
                }
                ag_accum_grad(e->a, ga);
                break;
            }
            case AG_SUM: {
                PTensor* g = ag_filled_shape(e->a_shape, e->a_ndim, seed);
                ag_accum_grad(e->a, g);
                break;
            }
            case AG_MEAN: {
                PTensor* g = ag_filled_shape(e->a_shape, e->a_ndim, seed / e->scalar);
                ag_accum_grad(e->a, g);
                break;
            }
            case AG_NEG: {
                PTensor* ng = ag_clone_tensor(grad_out); for (int64_t j=0;j<ng->len;j++) ng->data[j]=-ng->data[j];
                ag_accum_grad(e->a, ng);
                break;
            }
        }
    }
}
void* p_grad_get(void* t) {
    PTensor* g = ag_get_grad(t);
    if (g) return g;
    // No gradient — return zero tensor of same shape
    PTensor* pt = (PTensor*)t;
    if (!pt) return NULL;
    return (void*)ag_zeros_like(pt);
}
void p_grad_zero(void) {
    ag_grad_len = 0;
}
void p_checkpoint_begin(void) { ag_checkpoint_mode = 1; }
void p_checkpoint_end(void) { ag_checkpoint_mode = 0; }
void p_dp_budget_reset(double epsilon) { ag_dp_budget = 0.0; ag_dp_limit = epsilon; }
void p_dp_budget_check(double epsilon, double limit) {
    ag_dp_budget += epsilon;
    if (ag_dp_budget > limit) { fprintf(stderr, "DP budget exhausted: consumed %g > limit %g\n", ag_dp_budget, limit); exit(1); }
}
void* p_dp_noise(void* t, double sigma) {
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < pt->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) {
        double val = pt->data[i];
        double u1 = 0.0001 + fabs(fmod(val * 1000.0, 1.0)) * 0.9998;
        double u2 = 0.0001 + fabs(fmod(sin(val) * 1000.0, 1.0)) * 0.9998;
        double z = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
        r->data[i] = val + sigma * z;
    }
    return r;
}
void* p_gaussian_noise(void* shape_vec, double sigma) {
    // shape_vec is a Fides Vec (i64 pointer to 24-byte block)
    int64_t* sv = (int64_t*)shape_vec;
    int64_t ndim = sv ? sv[1] : 0;
    int64_t* data_ptr = sv ? (int64_t*)sv[0] : NULL;
    int64_t shape[8]; int64_t len = 1;
    for (int64_t i = 0; i < ndim && i < 8; i++) { shape[i] = data_ptr ? data_ptr[i] : 1; len *= shape[i]; }
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = ndim; r->len = len;
    for (int64_t i = 0; i < ndim && i < 8; i++) r->shape[i] = shape[i];
    r->data = (double*)malloc(len * sizeof(double));
    for (int64_t i = 0; i < len; i++) {
        double seed = (double)i * 0.1;
        double u1 = 0.0001 + fabs(sin(seed)) * 0.9998;
        double u2 = 0.0001 + fabs(cos(seed)) * 0.9998;
        double z = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
        r->data[i] = sigma * z;
    }
    return r;
}
void* p_dropout(void* t, double drop_prob) {
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    PTensor* r = ag_clone_tensor(pt);
    double scale = 1.0 / (1.0 - drop_prob);
    for (int64_t i = 0; i < r->len; i++) {
        double s = fabs(sin((double)i * 7.0));
        if (s < drop_prob) r->data[i] = 0.0; else r->data[i] *= scale;
    }
    return r;
}
int64_t p_tp_rank(void) { return ag_tp_rank; }
int64_t p_tp_world_size(void) { return ag_tp_world_size; }
void* p_all_reduce(void* t) {
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    return (void*)ag_clone_tensor(pt);
}
void* p_broadcast(void* t) {
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    return (void*)ag_clone_tensor(pt);
}
void* p_all_gather(void* t) {
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    return (void*)ag_clone_tensor(pt);
}
void* p_scatter(void* t) {
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    return (void*)ag_clone_tensor(pt);
}
void p_send_tensor(void* t) { (void)t; /* no-op in single-rank */ }
void* p_recv_tensor(void* shape_vec) {
    int64_t* sv = (int64_t*)shape_vec;
    int64_t ndim = sv ? sv[1] : 0;
    int64_t* data_ptr = sv ? (int64_t*)sv[0] : NULL;
    int64_t shape[8]; int64_t len = 1;
    for (int64_t i = 0; i < ndim && i < 8; i++) { shape[i] = data_ptr ? data_ptr[i] : 1; len *= shape[i]; }
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = ndim; r->len = len;
    for (int64_t i = 0; i < ndim && i < 8; i++) r->shape[i] = shape[i];
    r->data = (double*)calloc(len, sizeof(double));
    return r;
}

typedef struct { int64_t seq_len; PTensor* k_data; PTensor* v_data; } KVLayer;
typedef struct { int64_t num_layers; int64_t max_seq_len; int64_t window_size; int64_t head_dim; KVLayer* layers; } KVCache;
static KVCache* kv_caches[16];
static int64_t kv_cache_count = 0;
int64_t p_kv_cache_new(int64_t num_layers, int64_t max_seq_len, int64_t window_size, int64_t head_dim) {
    (void)max_seq_len;
    if (kv_cache_count >= 16) return -1;
    KVCache* c = (KVCache*)malloc(sizeof(KVCache));
    c->num_layers = num_layers; c->max_seq_len = max_seq_len;
    c->window_size = window_size; c->head_dim = head_dim;
    c->layers = (KVLayer*)calloc(num_layers, sizeof(KVLayer));
    for (int64_t i = 0; i < num_layers; i++) { c->layers[i].seq_len = 0; c->layers[i].k_data = NULL; c->layers[i].v_data = NULL; }
    int64_t id = kv_cache_count; kv_caches[kv_cache_count++] = c;
    return id;
}
void p_kv_cache_append(int64_t cache_id, int64_t layer, void* k, void* v) {
    if (cache_id < 0 || cache_id >= kv_cache_count) return;
    KVCache* c = kv_caches[cache_id];
    if (layer < 0 || layer >= c->num_layers) return;
    PTensor* kt = (PTensor*)k; PTensor* vt = (PTensor*)v;
    if (!kt || !vt) return;
    KVLayer* L = &c->layers[layer];
    if (!L->k_data) {
        L->k_data = (PTensor*)calloc(1, sizeof(PTensor)); L->k_data->ndim = 2; L->k_data->shape[1] = c->head_dim;
        L->v_data = (PTensor*)calloc(1, sizeof(PTensor)); L->v_data->ndim = 2; L->v_data->shape[1] = c->head_dim;
    }
    // Append every new value, then drop the oldest beyond the limit.
    PTensor* dst[2] = { L->k_data, L->v_data };
    PTensor* src[2] = { kt, vt };
    int64_t max_tokens = c->window_size > 0 && c->window_size < c->max_seq_len ? c->window_size : c->max_seq_len;
    int64_t max_vals = max_tokens * c->head_dim;
    if (max_vals < 0) max_vals = 0;
    for (int t = 0; t < 2; t++) {
        int64_t total = dst[t]->len + src[t]->len;
        dst[t]->data = (double*)realloc(dst[t]->data, (total > 0 ? total : 1) * sizeof(double));
        for (int64_t i = 0; i < src[t]->len; i++) dst[t]->data[dst[t]->len + i] = src[t]->data[i];
        dst[t]->len = total;
        if (dst[t]->len > max_vals) {
            int64_t excess = dst[t]->len - max_vals;
            memmove(dst[t]->data, dst[t]->data + excess, max_vals * sizeof(double));
            dst[t]->len = max_vals;
        }
    }
    int64_t hd = c->head_dim > 0 ? c->head_dim : 1;
    L->seq_len = L->k_data->len / hd;
    L->k_data->shape[0] = L->seq_len; L->v_data->shape[0] = L->seq_len;
}
void* p_kv_cache_get(int64_t cache_id, int64_t layer) {
    if (cache_id < 0 || cache_id >= kv_cache_count) return NULL;
    KVCache* c = kv_caches[cache_id];
    if (layer < 0 || layer >= c->num_layers) return NULL;
    if (!c->layers[layer].k_data) return NULL;
    int64_t sl = c->layers[layer].seq_len; int64_t hd = c->head_dim;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = 2; r->shape[0] = sl; r->shape[1] = hd * 2; r->len = sl * hd * 2;
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < sl * hd; i++) { r->data[i] = c->layers[layer].k_data->data[i]; r->data[sl * hd + i] = c->layers[layer].v_data->data[i]; }
    return r;
}
int64_t p_kv_cache_len(int64_t cache_id, int64_t layer) {
    if (cache_id < 0 || cache_id >= kv_cache_count) return 0;
    KVCache* c = kv_caches[cache_id];
    if (layer < 0 || layer >= c->num_layers) return 0;
    return c->layers[layer].seq_len;
}
void p_kv_cache_clear(int64_t cache_id) {
    if (cache_id < 0 || cache_id >= kv_cache_count) return;
    KVCache* c = kv_caches[cache_id];
    for (int64_t i = 0; i < c->num_layers; i++) c->layers[i].seq_len = 0;
}
void p_kv_cache_evict(int64_t cache_id, int64_t num_tokens) {
    if (cache_id < 0 || cache_id >= kv_cache_count) return;
    KVCache* c = kv_caches[cache_id];
    for (int64_t i = 0; i < c->num_layers; i++) {
        if (c->layers[i].seq_len > num_tokens) c->layers[i].seq_len -= num_tokens;
        else c->layers[i].seq_len = 0;
    }
}

void* p_quantize_q8(void* t, double scale, int64_t zero_point) {
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < pt->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(pt->len * sizeof(double));
    for (int64_t i = 0; i < pt->len; i++) { double q = round(pt->data[i] / scale) + zero_point; if (q < -128) q = -128; if (q > 127) q = 127; r->data[i] = q; }
    return r;
}
void* p_dequantize_q8(void* t, double scale, int64_t zero_point) {
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < pt->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(pt->len * sizeof(double));
    for (int64_t i = 0; i < pt->len; i++) r->data[i] = (pt->data[i] - zero_point) * scale;
    return r;
}
void* p_qmatmul(void* a, void* b) {
    PTensor* ta = (PTensor*)a; PTensor* tb = (PTensor*)b;
    if (!ta || !tb) return NULL;
    int64_t m = ta->shape[0], k = ta->shape[1], n = tb->shape[1];
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = 2; r->shape[0] = m; r->shape[1] = n; r->len = m * n;
    r->data = (double*)calloc(m * n, sizeof(double));
    // Issue 12: Use INT32 integer accumulation for quantized values.
    // Quantized tensors store int8 values as doubles (clamped to [-128, 127]).
    // The accumulation is done in int32 to avoid overflow, then rescaled.
    // This gives the same result as float matmul but with INT8 inputs.
    for (int64_t i = 0; i < m; i++) for (int64_t j = 0; j < n; j++) {
        int32_t acc = 0;
        for (int64_t x = 0; x < k; x++) {
            int32_t qa = (int32_t)(ta->data[i*k+x]);  // already quantized int8
            int32_t qb = (int32_t)(tb->data[x*n+j]);  // already quantized int8
            acc += qa * qb;  // INT8 x INT8 -> INT32 accumulate
        }
        r->data[i*n+j] = (double)acc;  // store as int32 result (caller dequantizes)
    }
    return r;
}
char* p_find_quant_params(void* t) {
    PTensor* pt = (PTensor*)t; if (!pt || pt->len == 0) return strdup("1,0");
    double mn = pt->data[0], mx = pt->data[0];
    for (int64_t i = 1; i < pt->len; i++) { if (pt->data[i] < mn) mn = pt->data[i]; if (pt->data[i] > mx) mx = pt->data[i]; }
    double abs_max = fabs(mn) > fabs(mx) ? fabs(mn) : fabs(mx);
    double scale = abs_max > 0.0 ? abs_max / 127.0 : 1.0;
    int64_t zp = 0;
    char* r = (char*)malloc(64); snprintf(r, 64, "%.17g,%lld", scale, (long long)zp); return r;
}

// JSON builtins — simple key-value store
typedef struct { char** keys; char** vals; int64_t len; int64_t cap; int64_t is_array; } PJson;
static char* _json_dump_obj(PJson* pj); // forward decl
void _json_append(PJson* j, const char* key, const char* val) {
    if (j->len >= j->cap) { j->cap = j->cap == 0 ? 8 : j->cap * 2; j->keys = (char**)realloc(j->keys, j->cap * sizeof(char*)); j->vals = (char**)realloc(j->vals, j->cap * sizeof(char*)); }
    j->keys[j->len] = key ? strdup(key) : strdup(""); j->vals[j->len] = val ? strdup(val) : strdup(""); j->len++;
}
char* p_json_parse(const char* s) {
    if (!s) return strdup("");
    // Simple JSON parser: extracts string and number values
    PJson* j = (PJson*)p_json_new_object();
    const char* p = s;
    while (*p) {
        // Skip whitespace and structural chars
        while (*p && *p != '"' && *p != '-' && (*p < '0' || *p > '9') && *p != 't' && *p != 'f' && *p != 'n') p++;
        if (!*p) break;
        // Try to parse a key (string followed by colon)
        if (*p == '"' && p > s && p[-1] != ':') {
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
            if (*p == '"') {
                p++; const char* vstart = p;
                while (*p && *p != '"') p++;
                size_t vlen = p - vstart;
                if (*p) p++;
                char* val = (char*)malloc(vlen+1); memcpy(val, vstart, vlen); val[vlen] = 0;
                _json_append(j, key, val); free(key); free(val);
            } else if (*p == 't') {
                _json_append(j, key, "true"); free(key); p += 4;
            } else if (*p == 'f') {
                _json_append(j, key, "false"); free(key); p += 5;
            } else if (*p == 'n') {
                _json_append(j, key, "null"); free(key); p += 4;
            } else {
                const char* vstart = p;
                while (*p && *p != ',' && *p != '}' && *p != ']' && *p != ' ') p++;
                size_t vlen = p - vstart;
                char* val = (char*)malloc(vlen+1); memcpy(val, vstart, vlen); val[vlen] = 0;
                _json_append(j, key, val); free(key); free(val);
            }
        } else {
            p++;
        }
    }
    char* result = _json_dump_obj(j);
    return result;
}
void* p_json_new_object(void) {
    PJson* j = (PJson*)malloc(sizeof(PJson));
    j->keys = NULL; j->vals = NULL; j->len = 0; j->cap = 0; j->is_array = 0;
    return j;
}
void* p_json_new_array(void) {
    PJson* j = (PJson*)p_json_new_object(); j->is_array = 1; return j;
}
void p_json_set(void* j, const char* key, const char* val) { _json_append((PJson*)j, key, val); }
void p_json_set_int(void* j, const char* key, int64_t val) { char buf[32]; snprintf(buf,sizeof(buf),"%lld",(long long)val); _json_append((PJson*)j, key, buf); }
void p_json_set_float(void* j, const char* key, double val) { char buf[32]; snprintf(buf,sizeof(buf),"%g",val); _json_append((PJson*)j, key, buf); }
void p_json_set_bool(void* j, const char* key, int64_t val) { _json_append((PJson*)j, key, val ? "true" : "false"); }
void p_json_append(void* j, const char* val) { _json_append((PJson*)j, NULL, val); }
void p_json_append_int(void* j, int64_t val) { char buf[32]; snprintf(buf,sizeof(buf),"%lld",(long long)val); _json_append((PJson*)j, NULL, buf); }
void p_json_append_float(void* j, double val) { char buf[32]; snprintf(buf,sizeof(buf),"%g",val); _json_append((PJson*)j, NULL, buf); }
// Internal: dump PJson object to string (sorted keys, no quotes on numbers/bools)
static char* _json_dump_obj(PJson* pj) {
    if (!pj) return strdup("{}");
    // Sort keys (simple insertion sort) for objects
    if (!pj->is_array) {
        for (int64_t i = 1; i < pj->len; i++) {
            char* k = pj->keys[i]; char* v = pj->vals[i];
            int64_t j = i - 1;
            while (j >= 0 && strcmp(pj->keys[j], k) > 0) { pj->keys[j+1] = pj->keys[j]; pj->vals[j+1] = pj->vals[j]; j--; }
            pj->keys[j+1] = k; pj->vals[j+1] = v;
        }
    }
    size_t cap = 64; char* result = (char*)malloc(cap); size_t pos = 0;
    result[pos++] = pj->is_array ? '[' : '{';
    for (int64_t i = 0; i < pj->len; i++) {
        if (i > 0) { if (pos+1>=cap){cap*=2;result=(char*)realloc(result,cap);} result[pos++]=','; }
        if (!pj->is_array) { size_t kl=strlen(pj->keys[i]); if (pos+kl+4>=cap){cap=(cap+kl)*2;result=(char*)realloc(result,cap);} result[pos++]='"'; memcpy(result+pos,pj->keys[i],kl); pos+=kl; result[pos++]='"'; result[pos++]=':'; }
        // Don't quote numbers, booleans, null
        const char* v = pj->vals[i];
        bool is_numeric = (*v >= '0' && *v <= '9') || *v == '-';
        bool is_bool = strcmp(v, "true") == 0 || strcmp(v, "false") == 0;
        bool is_null = strcmp(v, "null") == 0;
        bool need_quotes = !is_numeric && !is_bool && !is_null;
        size_t vl=strlen(v); if (pos+vl+3>=cap){cap=(cap+vl)*2;result=(char*)realloc(result,cap);}
        if (need_quotes) { result[pos++]='"'; memcpy(result+pos,v,vl); pos+=vl; result[pos++]='"'; }
        else { memcpy(result+pos,v,vl); pos+=vl; }
    }
    if (pos+2>=cap){cap+=2;result=(char*)realloc(result,cap);}
    result[pos++] = pj->is_array ? ']' : '}'; result[pos] = 0;
    return result;
}
// User-facing: parse JSON string and re-dump in canonical form
char* p_json_dump(void* j) {
    return p_json_parse((const char*)j);
}
char* p_json_dump_pretty(void* j) { return p_json_dump(j); }
char* p_json_pretty(void* j) {
    // Pretty-print with 2-space indentation (canonicalize first)
    char* canonical = p_json_parse((const char*)j);
    const char* s = canonical;
    if (!s) return strdup("");
    size_t cap = strlen(s) * 4 + 64; char* result = (char*)malloc(cap); size_t pos = 0;
    int indent = 0; bool in_str = false;
    while (*s) {
        if (*s == '"' && (s == (const char*)j || s[-1] != '\\')) in_str = !in_str;
        if (!in_str) {
            if (*s == '{' || *s == '[') {
                result[pos++] = *s++; result[pos++] = '\n';
                indent++;
                for (int i = 0; i < indent*2; i++) result[pos++] = ' ';
                continue;
            }
            if (*s == '}' || *s == ']') {
                result[pos++] = '\n'; indent--;
                for (int i = 0; i < indent*2; i++) result[pos++] = ' ';
                result[pos++] = *s++;
                continue;
            }
            if (*s == ',') {
                result[pos++] = *s++; result[pos++] = '\n';
                for (int i = 0; i < indent*2; i++) result[pos++] = ' ';
                continue;
            }
            if (*s == ':') { result[pos++] = *s++; result[pos++] = ' '; continue; }
        }
        if (pos >= cap-1) { cap *= 2; result = (char*)realloc(result, cap); }
        result[pos++] = *s++;
    }
    result[pos] = 0;
    return result;
}
char* p_json_string(void* j) {
    // Serialize a plain string as a JSON string literal (with quotes + escaping)
    const char* s = (const char*)j;
    if (!s) return strdup("\"\"");
    size_t cap = strlen(s) * 2 + 4; char* result = (char*)malloc(cap); size_t pos = 0;
    result[pos++] = '"';
    while (*s) {
        if (*s == '"' || *s == '\\') { if (pos >= cap-2) { cap *= 2; result = (char*)realloc(result, cap); } result[pos++] = '\\'; }
        if (*s == '\n') { if (pos >= cap-2) { cap *= 2; result = (char*)realloc(result, cap); } result[pos++] = '\\'; result[pos++] = 'n'; s++; continue; }
        if (pos >= cap-1) { cap *= 2; result = (char*)realloc(result, cap); }
        result[pos++] = *s++;
    }
    result[pos++] = '"'; result[pos] = 0;
    return result;
}
const char* p_json_type(void* j) {
    const char* s = (const char*)j;
    if (!s) return "null";
    while (*s == ' ') s++;
    if (*s == '{') return "object";
    if (*s == '[') return "array";
    if (*s == '"') return "string";
    if (*s == 't' || *s == 'f') return "boolean";
    if (*s == 'n') return "null";
    return "number";
}
char* p_json_get(void* j, const char* key) {
    const char* s = (const char*)j;
    if (!s || !key) return strdup("null");
    size_t klen = strlen(key);
    const char* p = s;
    while (*p) {
        if (*p == '"') {
            const char* ks = p + 1;
            const char* ke = ks;
            while (*ke && *ke != '"') { if (*ke == '\\' && ke[1]) ke += 2; else ke++; }
            if (*ke == '"' && (size_t)(ke - ks) == klen && strncmp(ks, key, klen) == 0) {
                const char* c = ke + 1;
                while (*c == ' ' || *c == '\t') c++;
                if (*c == ':') {
                    c++;
                    while (*c == ' ' || *c == '\t') c++;
                    if (*c == '"') {
                        const char* end = c + 1;
                        while (*end) { if (*end == '\\' && end[1]) end += 2; else if (*end == '"') break; else end++; }
                        size_t len = end - c + 1;
                        char* result = (char*)malloc(len+1); memcpy(result, c, len); result[len] = 0;
                        return result;
                    }
                    if (*c == '{' || *c == '[') {
                        char open = *c; char close = open == '{' ? '}' : ']';
                        int depth = 1; const char* end = c + 1;
                        while (*end && depth > 0) {
                            if (*end == '"') { const char* se = end + 1; while (*se) { if (*se == '\\' && se[1]) se += 2; else if (*se == '"') { end = se; break; } else se++; } if (*se != '"') { end = se; break; } end++; continue; }
                            if (*end == open) depth++;
                            if (*end == close) depth--;
                            if (depth > 0) end++;
                        }
                        size_t len = end - c + 1;
                        char* result = (char*)malloc(len+1); memcpy(result, c, len); result[len] = 0;
                        char* canonical = p_json_parse(result); free(result);
                        return canonical;
                    }
                    const char* end = c;
                    while (*end && *end != ',' && *end != '}' && *end != ']' && *end != ' ' && *end != '\n') end++;
                    size_t len = end - c;
                    char* result = (char*)malloc(len+1); memcpy(result, c, len); result[len] = 0;
                    return result;
                }
            }
            p = ke + 1;
        } else { p++; }
    }
    return strdup("null");
}
int64_t p_json_get_int(void* j, const char* key) { char* v = p_json_get(j, key); int64_t r = atoll(v); free(v); return r; }
double p_json_get_float(void* j, const char* key) { char* v = p_json_get(j, key); double r = atof(v); free(v); return r; }
int64_t p_json_get_bool(void* j, const char* key) { char* v = p_json_get(j, key); int64_t r = strcmp(v, "true") == 0; free(v); return r; }
void* p_json_at(void* j, int64_t idx) {
    const char* s = (const char*)j;
    if (!s || idx < 0) return strdup("null");
    // Find array start
    while (*s && *s != '[') s++;
    if (*s != '[') return strdup("null");
    s++; // skip '['
    int64_t cur = 0;
    while (*s) {
        while (*s == ' ' || *s == '\t') s++;
        if (*s == ']' || !*s) return strdup("null");
        if (cur == idx) {
            // Found the element — extract it
            if (*s == '"') {
                const char* end = s + 1;
                while (*end && *end != '"') end++;
                size_t len = end - s + 1; // include closing quote
                char* result = (char*)malloc(len+1); memcpy(result, s, len); result[len] = 0;
                return result;
            }
            const char* end = s;
            while (*end && *end != ',' && *end != ']') end++;
            size_t len = end - s;
            char* result = (char*)malloc(len+1); memcpy(result, s, len); result[len] = 0;
            return result;
        }
        // Skip this element
        if (*s == '"') {
            s++; while (*s && *s != '"') s++; if (*s) s++;
        } else {
            while (*s && *s != ',' && *s != ']') s++;
        }
        if (*s == ',') s++;
        cur++;
    }
    return strdup("null");
}
int64_t p_json_len(void* j) {
    const char* s = (const char*)j;
    if (!s) return 0;
    int64_t count = 0; int depth = 0; bool in_str = false;
    while (*s) {
        if (*s == '"' && (s == (const char*)j || s[-1] != '\\')) in_str = !in_str;
        if (!in_str) {
            if (*s == '{' || *s == '[') depth++;
            if (*s == '}' || *s == ']') depth--;
            if (*s == ',' && depth == 1) count++;
        }
        s++;
    }
    return count + 1;
}
void* p_json_object_get(void* j, const char* key) { return p_json_get(j, key); }

int64_t p_http_listen(int64_t port) { return p_socket_listen(port); }
char* p_http_recv_request(int64_t fd) { return p_socket_recv_str(fd); }
void p_http_send_response(int64_t fd, int64_t status, const char* body) {
    char header[256];
    const char* reason = "OK";
    if (status == 200) reason = "OK";
    else if (status == 201) reason = "Created";
    else if (status == 204) reason = "No Content";
    else if (status == 301) reason = "Moved Permanently";
    else if (status == 302) reason = "Found";
    else if (status == 304) reason = "Not Modified";
    else if (status == 400) reason = "Bad Request";
    else if (status == 401) reason = "Unauthorized";
    else if (status == 403) reason = "Forbidden";
    else if (status == 404) reason = "Not Found";
    else if (status == 405) reason = "Method Not Allowed";
    else if (status == 409) reason = "Conflict";
    else if (status == 413) reason = "Payload Too Large";
    else if (status == 429) reason = "Too Many Requests";
    else if (status == 500) reason = "Internal Server Error";
    else if (status == 501) reason = "Not Implemented";
    else if (status == 502) reason = "Bad Gateway";
    else if (status == 503) reason = "Service Unavailable";
    else if (status == 504) reason = "Gateway Timeout";
    snprintf(header, sizeof(header), "HTTP/1.1 %lld %s\r\nContent-Type: text/plain\r\nContent-Length: %zu\r\n\r\n", (long long)status, reason, strlen(body));
    p_socket_write(fd, header, strlen(header));
    p_socket_write(fd, body, strlen(body));
}
char* p_http_build_response(int64_t status, const char* content_type, const char* body) {
    (void)content_type;
    size_t blen = strlen(body);
    char* resp = (char*)malloc(blen + 256);
    const char* reason = "OK";
    if (status == 200) reason = "OK";
    else if (status == 201) reason = "Created";
    else if (status == 204) reason = "No Content";
    else if (status == 301) reason = "Moved Permanently";
    else if (status == 302) reason = "Found";
    else if (status == 304) reason = "Not Modified";
    else if (status == 400) reason = "Bad Request";
    else if (status == 401) reason = "Unauthorized";
    else if (status == 403) reason = "Forbidden";
    else if (status == 404) reason = "Not Found";
    else if (status == 405) reason = "Method Not Allowed";
    else if (status == 409) reason = "Conflict";
    else if (status == 413) reason = "Payload Too Large";
    else if (status == 429) reason = "Too Many Requests";
    else if (status == 500) reason = "Internal Server Error";
    else if (status == 501) reason = "Not Implemented";
    else if (status == 502) reason = "Bad Gateway";
    else if (status == 503) reason = "Service Unavailable";
    else if (status == 504) reason = "Gateway Timeout";
    snprintf(resp, blen+256, "HTTP/1.1 %lld %s\r\nContent-Length: %zu\r\n\r\n%s", (long long)status, reason, blen, body);
    return resp;
}
char* p_http_parse_request(const char* request) {
    if (!request) return strdup("{}");
    // Parse request line: METHOD PATH HTTP/VERSION
    const char* p = request;
    const char* line_end = strstr(p, "\r\n");
    if (!line_end) line_end = p + strlen(p);
    // Extract method
    const char* sp1 = strchr(p, ' ');
    char method[16] = {0};
    if (sp1 && sp1 < line_end) { size_t ml = sp1 - p; if (ml > 15) ml = 15; memcpy(method, p, ml); }
    // Extract path
    const char* path_start = sp1 ? sp1 + 1 : p;
    const char* sp2 = strchr(path_start, ' ');
    char path[1024] = {0};
    if (sp2 && sp2 < line_end) { size_t pl = sp2 - path_start; if (pl > 1023) pl = 1023; memcpy(path, path_start, pl); }
    // Extract version
    const char* ver_start = sp2 ? sp2 + 1 : p;
    char version[16] = {0};
    if (ver_start < line_end) { size_t vl = line_end - ver_start; if (vl > 15) vl = 15; memcpy(version, ver_start, vl); }
    // Strip HTTP/ prefix
    const char* v = version;
    if (strncmp(v, "HTTP/", 5) == 0) v += 5;
    // Parse headers until empty line
    p = line_end + 2; // skip \r\n
    size_t cap = 1024; char* result = (char*)malloc(cap); size_t pos = 0;
    pos += snprintf(result+pos, cap-pos, "{\"method\":\"%s\",\"path\":\"%s\",\"version\":\"%s\",\"headers\":{", method, path, v);
    int first_hdr = 1;
    while (*p) {
        const char* next_line = strstr(p, "\r\n");
        if (!next_line) break;
        if (next_line == p) { p += 2; break; } // empty line = end of headers
        // Find colon
        const char* colon = strchr(p, ':');
        if (colon && colon < next_line) {
            size_t kl = colon - p;
            const char* val_start = colon + 1;
            while (*val_start == ' ') val_start++;
            size_t vl = next_line - val_start;
            if (!first_hdr) pos += snprintf(result+pos, cap-pos, ",");
            first_hdr = 0;
            if (pos + kl + vl + 10 >= cap) { cap = (cap + kl + vl) * 2; result = (char*)realloc(result, cap); }
            result[pos++] = '"'; memcpy(result+pos, p, kl); pos += kl; result[pos++] = '"'; result[pos++] = ':'; result[pos++] = '"'; memcpy(result+pos, val_start, vl); pos += vl; result[pos++] = '"';
        }
        p = next_line + 2;
    }
    pos += snprintf(result+pos, cap-pos, "},\"body\":\"");
    // Extract body (everything after the empty line) and escape for JSON
    if (*p) {
        while (*p) {
            if (pos >= cap-2) { cap *= 2; result = (char*)realloc(result, cap); }
            if (*p == '"') { result[pos++] = '\\'; result[pos++] = '"'; }
            else if (*p == '\\') { result[pos++] = '\\'; result[pos++] = '\\'; }
            else if (*p == '\n') { result[pos++] = '\\'; result[pos++] = 'n'; }
            else if (*p == '\r') { result[pos++] = '\\'; result[pos++] = 'r'; }
            else if (*p == '\t') { result[pos++] = '\\'; result[pos++] = 't'; }
            else result[pos++] = *p;
            p++;
        }
    }
    pos += snprintf(result+pos, cap-pos, "\"}");
    return result;
}

// DB builtins — in-memory key-value store
typedef struct { char** keys; char** vals; int64_t len; int64_t cap; } PDB;
void* p_db_connect(const char* conn_str) {
    (void)conn_str;
    PDB* db = (PDB*)malloc(sizeof(PDB));
    db->keys = NULL; db->vals = NULL; db->len = 0; db->cap = 0;
    return db;
}
void p_db_exec(void* db, const char* query) { (void)db; (void)query; }
void* p_db_query(void* db, const char* query) { (void)query; return db; }
char* p_db_get(void* db, const char* key) {
    PDB* pdb = (PDB*)db;
    if (!pdb) return strdup("");
    for (int64_t i = 0; i < pdb->len; i++) if (strcmp(pdb->keys[i], key) == 0) return strdup(pdb->vals[i]);
    return strdup("");
}
void p_db_set(void* db, const char* key, const char* val) {
    PDB* pdb = (PDB*)db;
    if (!pdb) return;
    for (int64_t i = 0; i < pdb->len; i++) if (strcmp(pdb->keys[i], key) == 0) { free(pdb->vals[i]); pdb->vals[i] = strdup(val); return; }
    if (pdb->len >= pdb->cap) { pdb->cap = pdb->cap == 0 ? 8 : pdb->cap * 2; pdb->keys = (char**)realloc(pdb->keys, pdb->cap * sizeof(char*)); pdb->vals = (char**)realloc(pdb->vals, pdb->cap * sizeof(char*)); }
    pdb->keys[pdb->len] = strdup(key); pdb->vals[pdb->len] = strdup(val); pdb->len++;
}
typedef struct { char** keys; char** vals; int64_t len; int64_t cap; } PMemDB;
void* p_db_mem_open(void) {
    PMemDB* db = (PMemDB*)calloc(1, sizeof(PMemDB));
    return db;
}
static int64_t p_db_mem_find(PMemDB* mdb, const char* k, size_t kl) {
    for (int64_t i = 0; i < mdb->len; i++) {
        if (strlen(mdb->keys[i]) == kl && memcmp(mdb->keys[i], k, kl) == 0) return i;
    }
    return -1;
}
int64_t p_db_mem_exec(void* db, const char* query) {
    PMemDB* mdb = (PMemDB*)db;
    if (!mdb || !query) return 0;
    const char* p = query;
    while (*p == ' ') p++;
    char cmd[16] = {0}; int ci = 0;
    while (*p && *p != ' ' && ci < 15) cmd[ci++] = tolower(*p++);
    while (*p == ' ') p++;
    const char* kstart = p;
    while (*p && *p != ' ') p++;
    size_t klen = p - kstart;
    while (*p == ' ') p++;
    const char* vstart = p;
    while (*p) p++;
    size_t vlen = p - vstart;
    if ((strcmp(cmd, "insert") == 0 || strcmp(cmd, "set") == 0) && klen > 0) {
        char* v = (char*)malloc(vlen+1); memcpy(v, vstart, vlen); v[vlen] = 0;
        int64_t at = p_db_mem_find(mdb, kstart, klen);
        if (at >= 0) { free(mdb->vals[at]); mdb->vals[at] = v; return 1; }
        if (mdb->len >= mdb->cap) { mdb->cap = mdb->cap ? mdb->cap*2 : 8; mdb->keys = (char**)realloc(mdb->keys, mdb->cap*sizeof(char*)); mdb->vals = (char**)realloc(mdb->vals, mdb->cap*sizeof(char*)); }
        mdb->keys[mdb->len] = (char*)malloc(klen+1); memcpy(mdb->keys[mdb->len], kstart, klen); mdb->keys[mdb->len][klen] = 0;
        mdb->vals[mdb->len] = v;
        mdb->len++;
        return 1;
    }
    return 0;
}
char* p_db_mem_query(void* db, const char* query) {
    PMemDB* mdb = (PMemDB*)db;
    if (!mdb) return strdup("[]");
    const char* w = query ? strstr(query, "key=") : NULL;
    if (w) {
        const char* k = w + 4;
        while (*k == '\'' || *k == '"') k++;
        size_t kl = 0;
        while (k[kl] && k[kl] != ' ' && k[kl] != ';' && k[kl] != '\'' && k[kl] != '"') kl++;
        int64_t at = p_db_mem_find(mdb, k, kl);
        return strdup(at >= 0 ? mdb->vals[at] : "");
    }
    size_t cap = 64; char* result = (char*)malloc(cap); size_t pos = 0;
    result[pos++] = '[';
    for (int64_t i = 0; i < mdb->len; i++) {
        if (i > 0) result[pos++] = ',';
        size_t kl = strlen(mdb->keys[i]);
        if (pos+kl+4 >= cap) { cap = (cap+kl)*2; result = (char*)realloc(result, cap); }
        result[pos++] = '"'; memcpy(result+pos, mdb->keys[i], kl); pos += kl; result[pos++] = '"';
    }
    result[pos++] = ']'; result[pos] = 0;
    return result;
}
void p_db_mem_close(void* db) {
    PMemDB* mdb = (PMemDB*)db;
    if (mdb) { for (int64_t i = 0; i < mdb->len; i++) { free(mdb->keys[i]); free(mdb->vals[i]); } free(mdb->keys); free(mdb->vals); free(mdb); }
}

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
const __p_fn_entry __p_fns[] = {
    {(const void*)ByteBuffer_clear, "ByteBuffer_clear"},
    {(const void*)ByteBuffer_from_string, "ByteBuffer_from_string"},
    {(const void*)ByteBuffer_is_empty, "ByteBuffer_is_empty"},
    {(const void*)ByteBuffer_len, "ByteBuffer_len"},
    {(const void*)ByteBuffer_new, "ByteBuffer_new"},
    {(const void*)ByteBuffer_read_byte, "ByteBuffer_read_byte"},
    {(const void*)ByteBuffer_set_byte, "ByteBuffer_set_byte"},
    {(const void*)ByteBuffer_to_str, "ByteBuffer_to_str"},
    {(const void*)ByteBuffer_write_byte, "ByteBuffer_write_byte"},
    {(const void*)ByteBuffer_write_bytes, "ByteBuffer_write_bytes"},
    {(const void*)HashMap_clear, "HashMap_clear"},
    {(const void*)HashMap_clone, "HashMap_clone"},
    {(const void*)HashMap_contains, "HashMap_contains"},
    {(const void*)HashMap_contains_i, "HashMap_contains_i"},
    {(const void*)HashMap_drop, "HashMap_drop"},
    {(const void*)HashMap_find_slot, "HashMap_find_slot"},
    {(const void*)HashMap_get, "HashMap_get"},
    {(const void*)HashMap_get_i, "HashMap_get_i"},
    {(const void*)HashMap_get_or, "HashMap_get_or"},
    {(const void*)HashMap_get_or_i, "HashMap_get_or_i"},
    {(const void*)HashMap_grow, "HashMap_grow"},
    {(const void*)HashMap_insert, "HashMap_insert"},
    {(const void*)HashMap_insert_i, "HashMap_insert_i"},
    {(const void*)HashMap_is_empty, "HashMap_is_empty"},
    {(const void*)HashMap_keys, "HashMap_keys"},
    {(const void*)HashMap_keys_i, "HashMap_keys_i"},
    {(const void*)HashMap_len, "HashMap_len"},
    {(const void*)HashMap_new, "HashMap_new"},
    {(const void*)HashMap_remove, "HashMap_remove"},
    {(const void*)HashMap_remove_i, "HashMap_remove_i"},
    {(const void*)HashMap_share, "HashMap_share"},
    {(const void*)HashMap_to_str, "HashMap_to_str"},
    {(const void*)HashMap_values, "HashMap_values"},
    {(const void*)HashMap_values_i, "HashMap_values_i"},
    {(const void*)Option_and_then, "Option_and_then"},
    {(const void*)Option_context, "Option_context"},
    {(const void*)Option_expect, "Option_expect"},
    {(const void*)Option_is_none, "Option_is_none"},
    {(const void*)Option_is_some, "Option_is_some"},
    {(const void*)Option_map, "Option_map"},
    {(const void*)Option_or_else, "Option_or_else"},
    {(const void*)Option_unwrap, "Option_unwrap"},
    {(const void*)Option_unwrap_or, "Option_unwrap_or"},
    {(const void*)Option_unwrap_or_else, "Option_unwrap_or_else"},
    {(const void*)Result_and_then, "Result_and_then"},
    {(const void*)Result_context, "Result_context"},
    {(const void*)Result_err, "Result_err"},
    {(const void*)Result_expect, "Result_expect"},
    {(const void*)Result_is_err, "Result_is_err"},
    {(const void*)Result_is_ok, "Result_is_ok"},
    {(const void*)Result_map, "Result_map"},
    {(const void*)Result_map_err, "Result_map_err"},
    {(const void*)Result_ok, "Result_ok"},
    {(const void*)Result_unwrap, "Result_unwrap"},
    {(const void*)Result_unwrap_or, "Result_unwrap_or"},
    {(const void*)Result_unwrap_or_else, "Result_unwrap_or_else"},
    {(const void*)Result_with_context, "Result_with_context"},
    {(const void*)Str_drop, "Str_drop"},
    {(const void*)StringBuilder_append_char, "StringBuilder_append_char"},
    {(const void*)StringBuilder_append_int, "StringBuilder_append_int"},
    {(const void*)StringBuilder_append_str, "StringBuilder_append_str"},
    {(const void*)StringBuilder_clear, "StringBuilder_clear"},
    {(const void*)StringBuilder_is_empty, "StringBuilder_is_empty"},
    {(const void*)StringBuilder_len, "StringBuilder_len"},
    {(const void*)StringBuilder_new, "StringBuilder_new"},
    {(const void*)StringBuilder_to_str, "StringBuilder_to_str"},
    {(const void*)String_byte_at, "String_byte_at"},
    {(const void*)String_bytes, "String_bytes"},
    {(const void*)String_char_at, "String_char_at"},
    {(const void*)String_char_count, "String_char_count"},
    {(const void*)String_clone, "String_clone"},
    {(const void*)String_contains, "String_contains"},
    {(const void*)String_ends_with, "String_ends_with"},
    {(const void*)String_find, "String_find"},
    {(const void*)String_from, "String_from"},
    {(const void*)String_is_empty, "String_is_empty"},
    {(const void*)String_len, "String_len"},
    {(const void*)String_parse_int, "String_parse_int"},
    {(const void*)String_push_str, "String_push_str"},
    {(const void*)String_repeat, "String_repeat"},
    {(const void*)String_replace, "String_replace"},
    {(const void*)String_rfind, "String_rfind"},
    {(const void*)String_slice, "String_slice"},
    {(const void*)String_slice_len, "String_slice_len"},
    {(const void*)String_split, "String_split"},
    {(const void*)String_starts_with, "String_starts_with"},
    {(const void*)String_to_lowercase, "String_to_lowercase"},
    {(const void*)String_to_string, "String_to_string"},
    {(const void*)String_to_uppercase, "String_to_uppercase"},
    {(const void*)String_trim, "String_trim"},
    {(const void*)Vec_capacity, "Vec_capacity"},
    {(const void*)Vec_clear, "Vec_clear"},
    {(const void*)Vec_clone, "Vec_clone"},
    {(const void*)Vec_contains, "Vec_contains"},
    {(const void*)Vec_drop, "Vec_drop"},
    {(const void*)Vec_drop_strs, "Vec_drop_strs"},
    {(const void*)Vec_drop_vecs, "Vec_drop_vecs"},
    {(const void*)Vec_eq_strs, "Vec_eq_strs"},
    {(const void*)Vec_eq_words, "Vec_eq_words"},
    {(const void*)Vec_extend, "Vec_extend"},
    {(const void*)Vec_first, "Vec_first"},
    {(const void*)Vec_get, "Vec_get"},
    {(const void*)Vec_insert, "Vec_insert"},
    {(const void*)Vec_is_empty, "Vec_is_empty"},
    {(const void*)Vec_last, "Vec_last"},
    {(const void*)Vec_len, "Vec_len"},
    {(const void*)Vec_new, "Vec_new"},
    {(const void*)Vec_pop, "Vec_pop"},
    {(const void*)Vec_push, "Vec_push"},
    {(const void*)Vec_remove, "Vec_remove"},
    {(const void*)Vec_reverse, "Vec_reverse"},
    {(const void*)Vec_set, "Vec_set"},
    {(const void*)Vec_share, "Vec_share"},
    {(const void*)Vec_share_deep, "Vec_share_deep"},
    {(const void*)Vec_sort, "Vec_sort"},
    {(const void*)Vec_to_str, "Vec_to_str"},
    {(const void*)Vec_truncate, "Vec_truncate"},
    {(const void*)__eq_ByteBuffer, "__eq_ByteBuffer"},
    {(const void*)__eq_Expr, "__eq_Expr"},
    {(const void*)__eq_IfExpr, "__eq_IfExpr"},
    {(const void*)__eq_ParseResultIfExpr, "__eq_ParseResultIfExpr"},
    {(const void*)__eq_StringBuilder, "__eq_StringBuilder"},
    {(const void*)p_abs, "abs"},
    {(const void*)allocator_alloc, "allocator_alloc"},
    {(const void*)allocator_bytes_used, "allocator_bytes_used"},
    {(const void*)allocator_free, "allocator_free"},
    {(const void*)assert, "assert"},
    {(const void*)assert_msg, "assert_msg"},
    {(const void*)byte_to_str, "byte_to_str"},
    {(const void*)char_is_alpha, "char_is_alpha"},
    {(const void*)char_is_digit, "char_is_digit"},
    {(const void*)char_is_lower, "char_is_lower"},
    {(const void*)char_is_upper, "char_is_upper"},
    {(const void*)char_is_whitespace, "char_is_whitespace"},
    {(const void*)char_to_lower, "char_to_lower"},
    {(const void*)char_to_str, "char_to_str"},
    {(const void*)char_to_upper, "char_to_upper"},
    {(const void*)clamp, "clamp"},
    {(const void*)create_allocator, "create_allocator"},
    {(const void*)float_to_str, "float_to_str"},
    {(const void*)hm_eq, "hm_eq"},
    {(const void*)hm_hash, "hm_hash"},
    {(const void*)hm_int_hash, "hm_int_hash"},
    {(const void*)hm_is_int, "hm_is_int"},
    {(const void*)hm_store_i, "hm_store_i"},
    {(const void*)int_to_str, "int_to_str"},
    {(const void*)main_entry, "main"},
    {(const void*)map_own, "map_own"},
    {(const void*)p_max, "max"},
    {(const void*)p_min, "min"},
    {(const void*)p_str_alloc, "p_str_alloc"},
    {(const void*)p_str_compare, "p_str_compare"},
    {(const void*)p_str_copy_n, "p_str_copy_n"},
    {(const void*)p_str_equal, "p_str_equal"},
    {(const void*)p_str_free, "p_str_free"},
    {(const void*)p_str_from_ptr, "p_str_from_ptr"},
    {(const void*)p_str_hash, "p_str_hash"},
    {(const void*)p_str_len_word, "p_str_len_word"},
    {(const void*)p_str_length, "p_str_length"},
    {(const void*)panic, "panic"},
    {(const void*)println, "println"},
    {(const void*)str_byte_at, "str_byte_at"},
    {(const void*)str_bytes, "str_bytes"},
    {(const void*)str_clone, "str_clone"},
    {(const void*)str_compare, "str_compare"},
    {(const void*)str_concat, "str_concat"},
    {(const void*)str_contains, "str_contains"},
    {(const void*)str_ends_with, "str_ends_with"},
    {(const void*)str_equal, "str_equal"},
    {(const void*)str_find, "str_find"},
    {(const void*)str_from_bytes, "str_from_bytes"},
    {(const void*)str_is_empty, "str_is_empty"},
    {(const void*)str_len, "str_len"},
    {(const void*)str_lines, "str_lines"},
    {(const void*)str_parse_float, "str_parse_float"},
    {(const void*)str_parse_hex, "str_parse_hex"},
    {(const void*)str_parse_int, "str_parse_int"},
    {(const void*)str_parse_int_range, "str_parse_int_range"},
    {(const void*)str_repeat, "str_repeat"},
    {(const void*)str_replace, "str_replace"},
    {(const void*)str_rfind, "str_rfind"},
    {(const void*)str_slice, "str_slice"},
    {(const void*)str_slice_len, "str_slice_len"},
    {(const void*)str_split, "str_split"},
    {(const void*)str_split_char, "str_split_char"},
    {(const void*)str_starts_with, "str_starts_with"},
    {(const void*)str_to_lower, "str_to_lower"},
    {(const void*)str_to_upper, "str_to_upper"},
    {(const void*)str_trim, "str_trim"},
    {(const void*)str_trim_end, "str_trim_end"},
    {(const void*)str_trim_end_char, "str_trim_end_char"},
    {(const void*)str_trim_start, "str_trim_start"},
    {(const void*)u64_to_str, "u64_to_str"},
    {(const void*)vec_get_or, "vec_get_or"},
    {(const void*)vec_own, "vec_own"},
    {(const void*)vec_raw_extend, "vec_raw_extend"},
    {(const void*)vec_raw_insert, "vec_raw_insert"},
    {(const void*)vec_raw_remove, "vec_raw_remove"},
    {(const void*)vec_raw_reverse, "vec_raw_reverse"},
    {(const void*)vec_raw_sort, "vec_raw_sort"},
    {(const void*)vec_sift_down, "vec_sift_down"},
    {(const void*)vec_slot, "vec_slot"},
    {(const void*)vec_swap_slots, "vec_swap_slots"},
};
const int __p_fn_count = 203;
