#define P_CHECKED_ALLOC 1
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
#endif
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
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

// --- Runtime types ---
typedef struct { void** data; int64_t len; int64_t cap; } PVec;
typedef struct { char** keys; int64_t* vals; int64_t len; int64_t cap; } PMap;

// --- Runtime function declarations ---
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
int64_t p_phys_read8(int64_t addr);
void p_phys_write8(int64_t addr, int64_t val);
int64_t p_phys_read16(int64_t addr);
void p_phys_write16(int64_t addr, int64_t val);
int64_t p_phys_read32(int64_t addr);
void p_phys_write32(int64_t addr, int64_t val);
int64_t p_inb(int64_t port);
void p_outb(int64_t port, int64_t val);
int64_t p_inw(int64_t port);
void p_outw(int64_t port, int64_t val);
int64_t p_inl(int64_t port);
void p_outl(int64_t port, int64_t val);
int64_t p_msr_read(int64_t addr);
void p_msr_write(int64_t addr, int64_t val);
int64_t p_cpuid(int64_t leaf, int64_t subleaf);
int64_t p_cpuid_leaf(int64_t leaf);
void p_gdt_set_entry(int64_t gdt, int64_t index, int64_t base, int64_t limit, int64_t flags);
void p_idt_set_entry(int64_t idt, int64_t vector, int64_t handler, int64_t flags);
void p_tss_init(int64_t tss, int64_t rsp0);
void p_tss_load(int64_t selector);
void p_lapic_write(int64_t reg, int64_t val);
int64_t p_lapic_read(int64_t reg);
void p_lapic_eoi(void);
void p_ioapic_write(int64_t reg, int64_t val);
int64_t p_ioapic_read(int64_t reg);
void p_pic_remap(void);
void p_pic_mask(int64_t irq, int64_t masked);
void p_pic_eoi(int64_t irq);
int64_t p_cmos_read(int64_t reg);
void p_cmos_write(int64_t reg, int64_t val);
void p_serial_init(void);
void p_serial_write(int64_t c);
int64_t p_serial_read(void);
int64_t p_serial_ready(void);
void p_serial_putc(int64_t c);
int64_t p_serial_getc(void);
int64_t p_serial_available(void);
void p_memset(void* ptr, int val, int64_t size);
void p_str_zeroize(void* s);
void p_vec_zeroize(void* v);
void p_lgdt(int64_t gdt_desc);
void p_enter_user_mode(int64_t entry, int64_t stack);
void p_pit_set_frequency(int64_t hz);
int64_t p_hpet_read(int64_t reg);
void p_hpet_write(int64_t reg, int64_t val);
void p_syscall_set_handler(int64_t num, int64_t handler);
void p_ring3_enter(int64_t entry, int64_t stack, int64_t arg);
void p_context_save(int64_t ctx);
void p_context_restore(int64_t ctx);
int64_t p_pci_read(int64_t bus, int64_t dev, int64_t func, int64_t offset);
void p_pci_write(int64_t bus, int64_t dev, int64_t func, int64_t offset, int64_t val);
void p_ata_read(int64_t lba, int64_t buf);
void p_ata_write(int64_t lba, int64_t buf);
void p_acpi_shutdown(void);
void p_fb_init(void);
void p_fb_plot(int64_t x, int64_t y, int64_t color);
void p_fb_fill(int64_t x, int64_t y, int64_t w, int64_t h, int64_t color);
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
int64_t p_sb_new(void);
void p_sb_append(int64_t sb, const char* s);
char* p_sb_to_str(int64_t sb);
void p_print(int64_t val);
void p_print_str(const char* s);
void p_print_int(int64_t val);
void p_print_bool(bool val);
void p_print_float(double val);
void p_print_newline(void);
void p_eprint_str(const char* s);
void p_flush_stdout(void);
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
void* p_malloc_persistent(int64_t size);
void* p_alloc(int64_t size);
int64_t p_arena_save(void);
void p_arena_restore(int64_t mark);
void p_region_free(void* region);
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
    int64_t cap = h[2], nc = cap > 0 ? cap * 2 : 8;
    int64_t* nd = (int64_t*)p_malloc(nc * 8);
    int64_t* od = (int64_t*)(intptr_t)h[0];
    if (od) { memcpy(nd, od, (size_t)h[1] * 8); p_free(od); }
    h[0] = (int64_t)(intptr_t)nd;
    h[2] = nc;
}
/* Element access a counted loop proved in bounds (pe.rs unchecked_ops). */
#define P_VEC_UNCHECKED \
static inline int64_t __p_vec_get_u(void* e, int64_t v, int64_t i) { (void)e; return ((int64_t*)(intptr_t)((int64_t*)(intptr_t)v)[0])[i]; } \
static inline void __p_vec_set_u(void* e, int64_t v, int64_t i, int64_t x) { (void)e; ((int64_t*)(intptr_t)((int64_t*)(intptr_t)v)[0])[i] = x; } \
static inline void* __p_pvec_get_u(PVec* v, int64_t i) { return v->data[i]; } \
static inline void __p_pvec_set_u(PVec* v, int64_t i, void* x) { v->data[i] = x; }
P_VEC_UNCHECKED
char* p_file_read(const char* path);
int64_t p_file_write(const char* path, const char* content);
int64_t p_file_write_vec(const char* path, PVec* v);
PVec* p_file_read_bytes(const char* path);
bool p_file_exists(const char* path);
int64_t p_file_append(const char* path, const char* content);
int64_t p_file_delete(const char* path);
char* p_read_line(void);
char* p_read_stdin(int64_t n);
int64_t p_stdin_eof(void);
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
void p_thread_join(int64_t handle);
int64_t p_executor_new(void);
int64_t p_co_go(const char* name, int64_t arg);
void p_co_yield(void);
int64_t p_co_run(void);
int64_t p_co_done(int64_t t);
int64_t p_co_result(int64_t t);
int64_t p_co_async(const char* name, int64_t nargs, ...);
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
void p_vec_set(PVec* v, int64_t idx, void* val);

// --- String constants ---
static const char _str0[] = "called unwrap on None";
static const char _str1[] = "called unwrap on Err";
static const char _str2[] = ": ";
static const char _str3[] = "";
static const char _str4[] = "empty";
static const char _str5[] = "reading the port";
static const char _str6[] = "not in the list";
static const char _str7[] = "loading the config";
static const char _str8[] = "\n";
static const char _str9[] = "error: ";
static const char _str10[] = "8080";
static const char _str11[] = "label";
static const char _str12[] = "no/such/file.txt";
static const char _str13[] = "cannot read ";
static const char _str14[] = "Assertion failed";
static const char _str15[] = "Vec index out of bounds";
static const char _str16[] = "Vec is empty";
static const char _str17[] = "Vec insert index out of bounds";
static const char _str18[] = "Vec remove index out of bounds";
static const char _str19[] = "[";
static const char _str20[] = "]";
static const char _str21[] = ", ";
static const char _str22[] = "{";
static const char _str23[] = "}";

// --- Struct definitions ---
struct Option_s {
    int64_t field_0;
    int64_t field_1;
};

struct Result_s {
    int64_t field_0;
    int64_t field_1;
};

struct StringBuilder_s {
    int64_t field_0;
    int64_t field_1;
};

struct ByteBuffer_s {
    void* field_0;
    int64_t field_1;
};

// --- Function prototypes ---
void p_thread_register(const char* name, int64_t (*fn)());
bool Option_is_some(void* v0, void* v1);
bool Option_is_none(void* v0, void* v1);
int64_t Option_unwrap(void* v0, void* v1);
int64_t Option_unwrap_or(void* v0, void* v1, int64_t v2);
void Option_map(void* v0, void* v1, void* v2, void* v3);
void Option_and_then(void* v0, void* v1, void* v2, void* v3);
void Option_or_else(void* v0, void* v1, void* v2, void* v3);
int64_t Option_unwrap_or_else(void* v0, void* v1, void* v2);
int64_t Option_expect(void* v0, void* v1, void* v2);
bool Result_is_ok(void* v0, void* v1);
bool Result_is_err(void* v0, void* v1);
int64_t Result_unwrap(void* v0, void* v1);
int64_t Result_unwrap_or(void* v0, void* v1, int64_t v2);
void Result_ok(void* v0, void* v1, void* v2);
void Result_err(void* v0, void* v1, void* v2);
void Result_map(void* v0, void* v1, void* v2, void* v3);
void Result_map_err(void* v0, void* v1, void* v2, void* v3);
void Result_and_then(void* v0, void* v1, void* v2, void* v3);
int64_t Result_unwrap_or_else(void* v0, void* v1, void* v2);
int64_t Result_expect(void* v0, void* v1, void* v2);
void Result_context(void* v0, void* v1, void* v2, void* v3);
void Result_with_context(void* v0, void* v1, void* v2, void* v3);
void Option_context(void* v0, void* v1, void* v2, void* v3);
void StringBuilder_new(void* v0, void* v1);
void StringBuilder_append_str(void* v0, void* v1, void* v2);
void StringBuilder_append_char(void* v0, void* v1, int64_t v2);
void StringBuilder_append_int(void* v0, void* v1, int64_t v2);
void* StringBuilder_to_str(void* v0, void* v1);
int64_t StringBuilder_len(void* v0, void* v1);
bool StringBuilder_is_empty(void* v0, void* v1);
void StringBuilder_clear(void* v0, void* v1);
void ByteBuffer_new(void* v0, void* v1, int64_t v2);
void ByteBuffer_from_string(void* v0, void* v1, void* v2);
void ByteBuffer_write_byte(void* v0, void* v1, int64_t v2);
void ByteBuffer_write_bytes(void* v0, void* v1, int64_t v2, int64_t v3);
int64_t ByteBuffer_read_byte(void* v0, void* v1, int64_t v2);
void ByteBuffer_set_byte(void* v0, void* v1, int64_t v2, int64_t v3);
int64_t ByteBuffer_len(void* v0, void* v1);
bool ByteBuffer_is_empty(void* v0, void* v1);
void ByteBuffer_clear(void* v0, void* v1);
void* ByteBuffer_to_str(void* v0, void* v1);
void parse_port(void* v0, void* v1, void* v2);
void load_port(void* v0, void* v1, void* v2);
void find(void* v0, void* v1, void* v2, int64_t v3);
void where_is(void* v0, void* v1, void* v2, int64_t v3);
void* label(void* v0);
void show(void* v0, void* v1);
int64_t main_entry(void* v0);
void* file_read(void* v0, void* v1);
void file_read_result(void* v0, void* v1, void* v2);
int64_t file_write(void* v0, void* v1, void* v2);
int64_t file_append(void* v0, void* v1, void* v2);
bool file_exists(void* v0, void* v1);
int64_t file_write_bytes(void* v0, void* v1, void* v2);
void* file_read_bytes(void* v0, void* v1);
int64_t file_write_raw(void* v0, void* v1, void* v2);
int64_t file_delete(void* v0, void* v1);
int64_t file_size(void* v0, void* v1);
void* fs_read_text(void* v0, void* v1);
int64_t fs_write_text(void* v0, void* v1, void* v2);
int64_t fs_append_text(void* v0, void* v1, void* v2);
bool fs_exists(void* v0, void* v1);
int64_t fs_delete(void* v0, void* v1);
int64_t fs_size(void* v0, void* v1);
void println(void* v0, void* v1);
void assert(void* v0, bool v1);
void assert_msg(void* v0, bool v1, void* v2);
void panic(void* v0, void* v1);
int64_t p_max(void* v0, int64_t v1, int64_t v2);
int64_t p_min(void* v0, int64_t v1, int64_t v2);
int64_t p_abs(void* v0, int64_t v1);
int64_t clamp(void* v0, int64_t v1, int64_t v2, int64_t v3);
void* create_allocator(void* v0, void* v1);
int64_t allocator_alloc(void* v0, void* v1, int64_t v2);
void allocator_free(void* v0, void* v1, int64_t v2);
int64_t allocator_bytes_used(void* v0, void* v1);
int64_t Vec_new(void* v0);
void Vec_drop(void* v0, int64_t v1);
void Str_drop(void* v0, int64_t v1);
void Vec_push(void* v0, int64_t v1, int64_t v2);
int64_t Vec_pop(void* v0, int64_t v1);
int64_t Vec_len(void* v0, int64_t v1);
bool Vec_is_empty(void* v0, int64_t v1);
int64_t Vec_get(void* v0, int64_t v1, int64_t v2);
int64_t vec_get_or(void* v0, void* v1, int64_t v2, int64_t v3);
void Vec_set(void* v0, int64_t v1, int64_t v2, int64_t v3);
void Vec_clear(void* v0, int64_t v1);
int64_t Vec_capacity(void* v0, int64_t v1);
bool Vec_contains(void* v0, int64_t v1, int64_t v2);
int64_t Vec_first(void* v0, int64_t v1);
int64_t Vec_last(void* v0, int64_t v1);
void Vec_truncate(void* v0, int64_t v1, int64_t v2);
void Vec_reverse(void* v0, int64_t v1);
void vec_raw_reverse(void* v0, int64_t v1);
void Vec_extend(void* v0, int64_t v1, int64_t v2);
void vec_raw_extend(void* v0, int64_t v1, int64_t v2);
void Vec_insert(void* v0, int64_t v1, int64_t v2, int64_t v3);
void vec_raw_insert(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t Vec_remove(void* v0, int64_t v1, int64_t v2);
int64_t vec_raw_remove(void* v0, int64_t v1, int64_t v2);
int64_t Vec_clone(void* v0, int64_t v1);
void* Vec_to_str(void* v0, int64_t v1);
void Vec_sort(void* v0, int64_t v1);
void vec_raw_sort(void* v0, int64_t v1);
int64_t vec_slot(void* v0, int64_t v1, int64_t v2);
void vec_swap_slots(void* v0, int64_t v1, int64_t v2, int64_t v3);
void vec_sift_down(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t p_str_hash(void* v0, int64_t v1);
bool p_str_equal(void* v0, int64_t v1, int64_t v2);
int64_t p_str_compare(void* v0, int64_t v1, int64_t v2);
int64_t HashMap_new(void* v0);
int64_t HashMap_find_slot(void* v0, int64_t v1, int64_t v2);
void HashMap_grow(void* v0, int64_t v1);
void HashMap_insert(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t HashMap_get(void* v0, int64_t v1, int64_t v2);
bool HashMap_contains(void* v0, int64_t v1, int64_t v2);
void HashMap_remove(void* v0, int64_t v1, int64_t v2);
int64_t HashMap_len(void* v0, int64_t v1);
bool HashMap_is_empty(void* v0, int64_t v1);
void HashMap_clear(void* v0, int64_t v1);
int64_t HashMap_keys(void* v0, int64_t v1);
int64_t HashMap_values(void* v0, int64_t v1);
int64_t HashMap_clone(void* v0, int64_t v1);
int64_t HashMap_get_or(void* v0, int64_t v1, int64_t v2, int64_t v3);
void* HashMap_to_str(void* v0, int64_t v1);
void* p_str_from_ptr(void* v0, int64_t v1);
int64_t p_str_length(void* v0, int64_t v1);
void p_str_copy_n(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t String_from(void* v0, int64_t v1);
int64_t String_len(void* v0, int64_t v1);
bool String_is_empty(void* v0, int64_t v1);
int64_t String_push_str(void* v0, int64_t v1, int64_t v2);
bool String_contains(void* v0, int64_t v1, int64_t v2);
bool String_starts_with(void* v0, int64_t v1, int64_t v2);
bool String_ends_with(void* v0, int64_t v1, int64_t v2);
int64_t String_trim(void* v0, int64_t v1);
int64_t String_to_uppercase(void* v0, int64_t v1);
int64_t String_to_lowercase(void* v0, int64_t v1);
int64_t String_slice(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t String_slice_len(void* v0, int64_t v1, int64_t v2, int64_t v3, int64_t v4);
int64_t String_find(void* v0, int64_t v1, int64_t v2);
int64_t String_rfind(void* v0, int64_t v1, int64_t v2);
int64_t String_replace(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t String_split(void* v0, int64_t v1, int64_t v2);
int64_t String_parse_int(void* v0, int64_t v1);
int64_t String_char_at(void* v0, int64_t v1, int64_t v2);
int64_t String_repeat(void* v0, int64_t v1, int64_t v2);
int64_t String_to_string(void* v0, int64_t v1);
int64_t String_clone(void* v0, int64_t v1);
int64_t String_bytes(void* v0, int64_t v1);
int64_t String_byte_at(void* v0, int64_t v1, int64_t v2);
int64_t String_char_count(void* v0, int64_t v1);
void* char_to_str(void* v0, int64_t v1);
bool char_is_digit(void* v0, int64_t v1);
bool char_is_alpha(void* v0, int64_t v1);
bool char_is_whitespace(void* v0, int64_t v1);
bool char_is_upper(void* v0, int64_t v1);
bool char_is_lower(void* v0, int64_t v1);
int64_t char_to_upper(void* v0, int64_t v1);
int64_t char_to_lower(void* v0, int64_t v1);
void* str_bytes(void* v0, void* v1);
void* str_from_bytes(void* v0, void* v1);
void* str_slice(void* v0, void* v1, int64_t v2, int64_t v3);
void* str_slice_len(void* v0, void* v1, int64_t v2, int64_t v3, int64_t v4);
bool str_starts_with(void* v0, void* v1, void* v2);
bool str_ends_with(void* v0, void* v1, void* v2);
bool str_contains(void* v0, void* v1, void* v2);
int64_t str_find(void* v0, void* v1, void* v2);
int64_t str_rfind(void* v0, void* v1, void* v2);
void* str_trim_start(void* v0, void* v1);
void* str_trim_end(void* v0, void* v1);
void* str_trim(void* v0, void* v1);
void* str_lines(void* v0, void* v1);
void* str_trim_end_char(void* v0, void* v1, int64_t v2);
void* str_split(void* v0, void* v1, void* v2);
void* str_split_char(void* v0, void* v1, int64_t v2);
void* str_replace(void* v0, void* v1, void* v2, void* v3);
void* str_repeat(void* v0, void* v1, int64_t v2);
void* str_to_upper(void* v0, void* v1);
void* str_to_lower(void* v0, void* v1);
int64_t str_parse_int(void* v0, void* v1);
int64_t str_parse_hex(void* v0, void* v1);
int64_t str_parse_int_range(void* v0, void* v1, int64_t v2, int64_t v3);
int64_t str_byte_at(void* v0, void* v1, int64_t v2);
int64_t str_len(void* v0, void* v1);
bool str_is_empty(void* v0, void* v1);
void* str_concat(void* v0, void* v1, void* v2);
int64_t str_compare(void* v0, void* v1, void* v2);
bool str_equal(void* v0, void* v1, void* v2);
void* int_to_str(void* v0, int64_t v1);
void* u64_to_str(void* v0, uint64_t v1);
void* float_to_str(void* v0, double v1);
double str_parse_float(void* v0, void* v1);
void* byte_to_str(void* v0, int64_t v1);

// --- Runtime global initialization (bug 3.16 fix) ---
static void __init_globals(void);
static void __init_globals(void) { }

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
    v15 = _str0;
    p_panic(v15);
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

void Option_map(void* v0, void* v1, void* v2, void* v3) {
    void* v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0;
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
    void* v33 = 0;
    int64_t v34 = 0;
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
    v12 = (char*)v11 + 0;
    v13 = 0LL;
    *(int64_t*)v12 = (int64_t)v13;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = (int64_t)Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = (int64_t)Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_to_str") == 0) {
            v15 = (int64_t)StringBuilder_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = (int64_t)StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = (int64_t)ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_to_str") == 0) {
            v15 = (int64_t)ByteBuffer_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_read") == 0) {
            v15 = (int64_t)file_read((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_read_bytes") == 0) {
            v15 = (int64_t)file_read_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_delete") == 0) {
            v15 = (int64_t)file_delete((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_size") == 0) {
            v15 = (int64_t)file_size((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "fs_read_text") == 0) {
            v15 = (int64_t)fs_read_text((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "fs_delete") == 0) {
            v15 = (int64_t)fs_delete((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "fs_size") == 0) {
            v15 = (int64_t)fs_size((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = (int64_t)p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "create_allocator") == 0) {
            v15 = (int64_t)create_allocator((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = (int64_t)allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = (int64_t)Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = (int64_t)Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = (int64_t)Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = (int64_t)Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = (int64_t)Vec_last((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = (int64_t)Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_to_str") == 0) {
            v15 = (int64_t)Vec_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = (int64_t)p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = (int64_t)HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = (int64_t)HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = (int64_t)HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = (int64_t)HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_to_str") == 0) {
            v15 = (int64_t)HashMap_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_from_ptr") == 0) {
            v15 = (int64_t)p_str_from_ptr((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = (int64_t)p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = (int64_t)String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = (int64_t)String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = (int64_t)String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = (int64_t)String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = (int64_t)String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = (int64_t)String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = (int64_t)String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = (int64_t)String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = (int64_t)String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = (int64_t)String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_str") == 0) {
            v15 = (int64_t)char_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = (int64_t)char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = (int64_t)char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_bytes") == 0) {
            v15 = (int64_t)str_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_from_bytes") == 0) {
            v15 = (int64_t)str_from_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_start") == 0) {
            v15 = (int64_t)str_trim_start((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_end") == 0) {
            v15 = (int64_t)str_trim_end((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim") == 0) {
            v15 = (int64_t)str_trim((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_lines") == 0) {
            v15 = (int64_t)str_lines((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_upper") == 0) {
            v15 = (int64_t)str_to_upper((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_lower") == 0) {
            v15 = (int64_t)str_to_lower((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = (int64_t)str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = (int64_t)str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = (int64_t)str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "int_to_str") == 0) {
            v15 = (int64_t)int_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "u64_to_str") == 0) {
            v15 = (int64_t)u64_to_str((void*)v3, (uint64_t)v10);
        } else if (strcmp(_fn_name, "float_to_str") == 0) {
            v15 = (int64_t)float_to_str((void*)v3, (double)(intptr_t)v10);
        } else if (strcmp(_fn_name, "byte_to_str") == 0) {
            v15 = (int64_t)byte_to_str((void*)v3, (int64_t)v10);
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
    v33 = (char*)v32 + 0;
    v34 = 1LL;
    *(int64_t*)v33 = (int64_t)v34;
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

void Option_and_then(void* v0, void* v1, void* v2, void* v3) {
    void* v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0;
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
    void* v33 = 0;
    int64_t v34 = 0;
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
    v12 = (char*)v11 + 0;
    v13 = 0LL;
    *(int64_t*)v12 = (int64_t)v13;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = (int64_t)Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = (int64_t)Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_to_str") == 0) {
            v15 = (int64_t)StringBuilder_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = (int64_t)StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = (int64_t)ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_to_str") == 0) {
            v15 = (int64_t)ByteBuffer_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_read") == 0) {
            v15 = (int64_t)file_read((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_read_bytes") == 0) {
            v15 = (int64_t)file_read_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_delete") == 0) {
            v15 = (int64_t)file_delete((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_size") == 0) {
            v15 = (int64_t)file_size((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "fs_read_text") == 0) {
            v15 = (int64_t)fs_read_text((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "fs_delete") == 0) {
            v15 = (int64_t)fs_delete((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "fs_size") == 0) {
            v15 = (int64_t)fs_size((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = (int64_t)p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "create_allocator") == 0) {
            v15 = (int64_t)create_allocator((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = (int64_t)allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = (int64_t)Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = (int64_t)Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = (int64_t)Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = (int64_t)Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = (int64_t)Vec_last((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = (int64_t)Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_to_str") == 0) {
            v15 = (int64_t)Vec_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = (int64_t)p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = (int64_t)HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = (int64_t)HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = (int64_t)HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = (int64_t)HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_to_str") == 0) {
            v15 = (int64_t)HashMap_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_from_ptr") == 0) {
            v15 = (int64_t)p_str_from_ptr((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = (int64_t)p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = (int64_t)String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = (int64_t)String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = (int64_t)String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = (int64_t)String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = (int64_t)String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = (int64_t)String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = (int64_t)String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = (int64_t)String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = (int64_t)String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = (int64_t)String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_str") == 0) {
            v15 = (int64_t)char_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = (int64_t)char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = (int64_t)char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_bytes") == 0) {
            v15 = (int64_t)str_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_from_bytes") == 0) {
            v15 = (int64_t)str_from_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_start") == 0) {
            v15 = (int64_t)str_trim_start((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_end") == 0) {
            v15 = (int64_t)str_trim_end((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim") == 0) {
            v15 = (int64_t)str_trim((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_lines") == 0) {
            v15 = (int64_t)str_lines((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_upper") == 0) {
            v15 = (int64_t)str_to_upper((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_lower") == 0) {
            v15 = (int64_t)str_to_lower((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = (int64_t)str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = (int64_t)str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = (int64_t)str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "int_to_str") == 0) {
            v15 = (int64_t)int_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "u64_to_str") == 0) {
            v15 = (int64_t)u64_to_str((void*)v3, (uint64_t)v10);
        } else if (strcmp(_fn_name, "float_to_str") == 0) {
            v15 = (int64_t)float_to_str((void*)v3, (double)(intptr_t)v10);
        } else if (strcmp(_fn_name, "byte_to_str") == 0) {
            v15 = (int64_t)byte_to_str((void*)v3, (int64_t)v10);
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
    v33 = (char*)v32 + 0;
    v34 = 1LL;
    *(int64_t*)v33 = (int64_t)v34;
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
    void* v12 = 0;
    int64_t v13 = 0;
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
    int64_t v33 = 0;
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
    v12 = (char*)v11 + 0;
    v13 = 0LL;
    *(int64_t*)v12 = (int64_t)v13;
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
    v32 = (char*)v31 + 0;
    v33 = 0LL;
    *(int64_t*)v32 = (int64_t)v33;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "label") == 0) {
            v35 = (int64_t)label((void*)v3);
        } else if (strcmp(_fn_name, "main") == 0) {
            v35 = (int64_t)main_entry((void*)v3);
        } else if (strcmp(_fn_name, "Vec_new") == 0) {
            v35 = (int64_t)Vec_new((void*)v3);
        } else if (strcmp(_fn_name, "HashMap_new") == 0) {
            v35 = (int64_t)HashMap_new((void*)v3);
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
        if (strcmp(_fn_name, "label") == 0) {
            v16 = (int64_t)label((void*)v2);
        } else if (strcmp(_fn_name, "main") == 0) {
            v16 = (int64_t)main_entry((void*)v2);
        } else if (strcmp(_fn_name, "Vec_new") == 0) {
            v16 = (int64_t)Vec_new((void*)v2);
        } else if (strcmp(_fn_name, "HashMap_new") == 0) {
            v16 = (int64_t)HashMap_new((void*)v2);
        } else { abort(); }
    }
    return v16;
  bb5:
    goto bb1;
    return 0;
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
    p_panic(v2);
    goto bb1;
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

void Result_ok(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    void* v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0;
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
    void* v33 = 0;
    int64_t v34 = 0;
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
    v11 = (char*)v10 + 0;
    v12 = 0LL;
    *(int64_t*)v11 = (int64_t)v12;
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
    v33 = (char*)v32 + 0;
    v34 = 1LL;
    *(int64_t*)v33 = (int64_t)v34;
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

void Result_err(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0;
    void* v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0;
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
    void* v31 = 0;
    int64_t v32 = 0;
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
    v11 = (char*)v10 + 0;
    v12 = 1LL;
    *(int64_t*)v11 = (int64_t)v12;
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
    v31 = (char*)v30 + 0;
    v32 = 0LL;
    *(int64_t*)v31 = (int64_t)v32;
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

void Result_map(void* v0, void* v1, void* v2, void* v3) {
    void* v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0;
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
    void* v35 = 0;
    int64_t v36 = 0;
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
    v12 = (char*)v11 + 0;
    v13 = 0LL;
    *(int64_t*)v12 = (int64_t)v13;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = (int64_t)Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = (int64_t)Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_to_str") == 0) {
            v15 = (int64_t)StringBuilder_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = (int64_t)StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = (int64_t)ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_to_str") == 0) {
            v15 = (int64_t)ByteBuffer_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_read") == 0) {
            v15 = (int64_t)file_read((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_read_bytes") == 0) {
            v15 = (int64_t)file_read_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_delete") == 0) {
            v15 = (int64_t)file_delete((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_size") == 0) {
            v15 = (int64_t)file_size((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "fs_read_text") == 0) {
            v15 = (int64_t)fs_read_text((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "fs_delete") == 0) {
            v15 = (int64_t)fs_delete((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "fs_size") == 0) {
            v15 = (int64_t)fs_size((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = (int64_t)p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "create_allocator") == 0) {
            v15 = (int64_t)create_allocator((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = (int64_t)allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = (int64_t)Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = (int64_t)Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = (int64_t)Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = (int64_t)Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = (int64_t)Vec_last((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = (int64_t)Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_to_str") == 0) {
            v15 = (int64_t)Vec_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = (int64_t)p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = (int64_t)HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = (int64_t)HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = (int64_t)HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = (int64_t)HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_to_str") == 0) {
            v15 = (int64_t)HashMap_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_from_ptr") == 0) {
            v15 = (int64_t)p_str_from_ptr((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = (int64_t)p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = (int64_t)String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = (int64_t)String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = (int64_t)String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = (int64_t)String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = (int64_t)String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = (int64_t)String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = (int64_t)String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = (int64_t)String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = (int64_t)String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = (int64_t)String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_str") == 0) {
            v15 = (int64_t)char_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = (int64_t)char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = (int64_t)char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_bytes") == 0) {
            v15 = (int64_t)str_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_from_bytes") == 0) {
            v15 = (int64_t)str_from_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_start") == 0) {
            v15 = (int64_t)str_trim_start((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_end") == 0) {
            v15 = (int64_t)str_trim_end((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim") == 0) {
            v15 = (int64_t)str_trim((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_lines") == 0) {
            v15 = (int64_t)str_lines((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_upper") == 0) {
            v15 = (int64_t)str_to_upper((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_lower") == 0) {
            v15 = (int64_t)str_to_lower((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = (int64_t)str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = (int64_t)str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = (int64_t)str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "int_to_str") == 0) {
            v15 = (int64_t)int_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "u64_to_str") == 0) {
            v15 = (int64_t)u64_to_str((void*)v3, (uint64_t)v10);
        } else if (strcmp(_fn_name, "float_to_str") == 0) {
            v15 = (int64_t)float_to_str((void*)v3, (double)(intptr_t)v10);
        } else if (strcmp(_fn_name, "byte_to_str") == 0) {
            v15 = (int64_t)byte_to_str((void*)v3, (int64_t)v10);
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
    v35 = (char*)v34 + 0;
    v36 = 1LL;
    *(int64_t*)v35 = (int64_t)v36;
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
    void* v12 = 0;
    int64_t v13 = 0;
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
    void* v34 = 0;
    int64_t v35 = 0;
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
    v12 = (char*)v11 + 0;
    v13 = 0LL;
    *(int64_t*)v12 = (int64_t)v13;
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
    v34 = (char*)v33 + 0;
    v35 = 1LL;
    *(int64_t*)v34 = (int64_t)v35;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v37 = (int64_t)Option_unwrap((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v37 = (int64_t)Result_unwrap((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "StringBuilder_to_str") == 0) {
            v37 = (int64_t)StringBuilder_to_str((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v37 = (int64_t)StringBuilder_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v37 = (int64_t)ByteBuffer_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "ByteBuffer_to_str") == 0) {
            v37 = (int64_t)ByteBuffer_to_str((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "file_read") == 0) {
            v37 = (int64_t)file_read((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "file_read_bytes") == 0) {
            v37 = (int64_t)file_read_bytes((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "file_delete") == 0) {
            v37 = (int64_t)file_delete((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "file_size") == 0) {
            v37 = (int64_t)file_size((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "fs_read_text") == 0) {
            v37 = (int64_t)fs_read_text((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "fs_delete") == 0) {
            v37 = (int64_t)fs_delete((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "fs_size") == 0) {
            v37 = (int64_t)fs_size((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v37 = (int64_t)p_abs((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "create_allocator") == 0) {
            v37 = (int64_t)create_allocator((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v37 = (int64_t)allocator_bytes_used((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v37 = (int64_t)Vec_pop((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v37 = (int64_t)Vec_len((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v37 = (int64_t)Vec_capacity((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v37 = (int64_t)Vec_first((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v37 = (int64_t)Vec_last((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v37 = (int64_t)Vec_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_to_str") == 0) {
            v37 = (int64_t)Vec_to_str((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v37 = (int64_t)p_str_hash((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v37 = (int64_t)HashMap_len((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v37 = (int64_t)HashMap_keys((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v37 = (int64_t)HashMap_values((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v37 = (int64_t)HashMap_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_to_str") == 0) {
            v37 = (int64_t)HashMap_to_str((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "p_str_from_ptr") == 0) {
            v37 = (int64_t)p_str_from_ptr((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v37 = (int64_t)p_str_length((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v37 = (int64_t)String_from((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v37 = (int64_t)String_len((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v37 = (int64_t)String_trim((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v37 = (int64_t)String_to_uppercase((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v37 = (int64_t)String_to_lowercase((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v37 = (int64_t)String_parse_int((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v37 = (int64_t)String_to_string((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v37 = (int64_t)String_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v37 = (int64_t)String_bytes((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v37 = (int64_t)String_char_count((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "char_to_str") == 0) {
            v37 = (int64_t)char_to_str((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v37 = (int64_t)char_to_upper((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v37 = (int64_t)char_to_lower((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "str_bytes") == 0) {
            v37 = (int64_t)str_bytes((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_from_bytes") == 0) {
            v37 = (int64_t)str_from_bytes((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_trim_start") == 0) {
            v37 = (int64_t)str_trim_start((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_trim_end") == 0) {
            v37 = (int64_t)str_trim_end((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_trim") == 0) {
            v37 = (int64_t)str_trim((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_lines") == 0) {
            v37 = (int64_t)str_lines((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_to_upper") == 0) {
            v37 = (int64_t)str_to_upper((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_to_lower") == 0) {
            v37 = (int64_t)str_to_lower((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v37 = (int64_t)str_parse_int((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v37 = (int64_t)str_parse_hex((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v37 = (int64_t)str_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "int_to_str") == 0) {
            v37 = (int64_t)int_to_str((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "u64_to_str") == 0) {
            v37 = (int64_t)u64_to_str((void*)v3, (uint64_t)v32);
        } else if (strcmp(_fn_name, "float_to_str") == 0) {
            v37 = (int64_t)float_to_str((void*)v3, (double)(intptr_t)v32);
        } else if (strcmp(_fn_name, "byte_to_str") == 0) {
            v37 = (int64_t)byte_to_str((void*)v3, (int64_t)v32);
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

void Result_and_then(void* v0, void* v1, void* v2, void* v3) {
    void* v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0;
    bool v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    void* v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0;
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
    void* v35 = 0;
    int64_t v36 = 0;
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
    v12 = (char*)v11 + 0;
    v13 = 0LL;
    *(int64_t*)v12 = (int64_t)v13;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = (int64_t)Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = (int64_t)Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_to_str") == 0) {
            v15 = (int64_t)StringBuilder_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = (int64_t)StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = (int64_t)ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_to_str") == 0) {
            v15 = (int64_t)ByteBuffer_to_str((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_read") == 0) {
            v15 = (int64_t)file_read((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_read_bytes") == 0) {
            v15 = (int64_t)file_read_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_delete") == 0) {
            v15 = (int64_t)file_delete((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "file_size") == 0) {
            v15 = (int64_t)file_size((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "fs_read_text") == 0) {
            v15 = (int64_t)fs_read_text((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "fs_delete") == 0) {
            v15 = (int64_t)fs_delete((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "fs_size") == 0) {
            v15 = (int64_t)fs_size((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = (int64_t)p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "create_allocator") == 0) {
            v15 = (int64_t)create_allocator((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = (int64_t)allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = (int64_t)Vec_pop((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = (int64_t)Vec_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = (int64_t)Vec_capacity((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = (int64_t)Vec_first((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = (int64_t)Vec_last((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_clone") == 0) {
            v15 = (int64_t)Vec_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_to_str") == 0) {
            v15 = (int64_t)Vec_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = (int64_t)p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = (int64_t)HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = (int64_t)HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = (int64_t)HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = (int64_t)HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_to_str") == 0) {
            v15 = (int64_t)HashMap_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_from_ptr") == 0) {
            v15 = (int64_t)p_str_from_ptr((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = (int64_t)p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = (int64_t)String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = (int64_t)String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = (int64_t)String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = (int64_t)String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = (int64_t)String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = (int64_t)String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = (int64_t)String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = (int64_t)String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = (int64_t)String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = (int64_t)String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_str") == 0) {
            v15 = (int64_t)char_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = (int64_t)char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = (int64_t)char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_bytes") == 0) {
            v15 = (int64_t)str_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_from_bytes") == 0) {
            v15 = (int64_t)str_from_bytes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_start") == 0) {
            v15 = (int64_t)str_trim_start((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim_end") == 0) {
            v15 = (int64_t)str_trim_end((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_trim") == 0) {
            v15 = (int64_t)str_trim((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_lines") == 0) {
            v15 = (int64_t)str_lines((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_upper") == 0) {
            v15 = (int64_t)str_to_upper((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_to_lower") == 0) {
            v15 = (int64_t)str_to_lower((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = (int64_t)str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = (int64_t)str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = (int64_t)str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "int_to_str") == 0) {
            v15 = (int64_t)int_to_str((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "u64_to_str") == 0) {
            v15 = (int64_t)u64_to_str((void*)v3, (uint64_t)v10);
        } else if (strcmp(_fn_name, "float_to_str") == 0) {
            v15 = (int64_t)float_to_str((void*)v3, (double)(intptr_t)v10);
        } else if (strcmp(_fn_name, "byte_to_str") == 0) {
            v15 = (int64_t)byte_to_str((void*)v3, (int64_t)v10);
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
    v35 = (char*)v34 + 0;
    v36 = 1LL;
    *(int64_t*)v35 = (int64_t)v36;
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
        if (strcmp(_fn_name, "label") == 0) {
            v18 = (int64_t)label((void*)v2);
        } else if (strcmp(_fn_name, "main") == 0) {
            v18 = (int64_t)main_entry((void*)v2);
        } else if (strcmp(_fn_name, "Vec_new") == 0) {
            v18 = (int64_t)Vec_new((void*)v2);
        } else if (strcmp(_fn_name, "HashMap_new") == 0) {
            v18 = (int64_t)HashMap_new((void*)v2);
        } else { abort(); }
    }
    return v18;
  bb5:
    goto bb1;
    return 0;
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
    p_panic(v2);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
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
    void* v12 = 0;
    int64_t v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    void* v20 = 0;
    int64_t v21 = 0;
    int64_t v22 = 0;
    bool v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    void* v25 = 0;
    void* v26 = 0;
    void* v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    void* v31 = 0;
    void* v32 = 0;
    void* v33 = 0;
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
    int64_t v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    v27 = alloca(16);
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
    v11 = (char*)v2 + 0;
    v12 = (char*)v0 + 0;
    v13 = *(int64_t*)v11;
    *(int64_t*)v12 = (int64_t)v13;
    v15 = (char*)v2 + 8;
    v16 = (char*)v0 + 8;
    v17 = *(int64_t*)v15;
    *(int64_t*)v16 = (int64_t)v17;
    return;
  bb3:
    v20 = (char*)v2 + 0;
    v21 = *(int64_t*)v20;
    v22 = 1LL;
    v23 = v21 == v22;
    if (v23) goto bb4;
    else goto bb5;
  bb4:
    v25 = (char*)v2 + 8;
    v26 = *(void**)v25;
    memset(v27, 0, 16);
    v28 = (char*)v27 + 0;
    v29 = 1LL;
    *(int64_t*)v28 = (int64_t)v29;
    v31 = _str2;
    v32 = p_str_concat(v3, v31);
    v33 = p_str_concat(v32, v26);
    v47 = 0LL;
    Str_drop((void*)v47, (int64_t)v32);
    v34 = (char*)v27 + 8;
    *(void**)v34 = (void*)v33;
    v36 = (char*)v27 + 0;
    v37 = (char*)v0 + 0;
    v38 = *(int64_t*)v36;
    *(int64_t*)v37 = (int64_t)v38;
    v40 = (char*)v27 + 8;
    v41 = (char*)v0 + 8;
    v42 = *(int64_t*)v40;
    *(int64_t*)v41 = (int64_t)v42;
    return;
  bb5:
    goto bb1;
    return;
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
    void* v12 = 0;
    int64_t v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
    void* v20 = 0;
    int64_t v21 = 0;
    int64_t v22 = 0;
    bool v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    void* v25 = 0;
    void* v26 = 0;
    void* v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    void* v31 = 0;
    void* v32 = 0;
    void* v33 = 0;
    void* v34 = 0;
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
    int64_t v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    v27 = alloca(16);
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
    v11 = (char*)v2 + 0;
    v12 = (char*)v0 + 0;
    v13 = *(int64_t*)v11;
    *(int64_t*)v12 = (int64_t)v13;
    v15 = (char*)v2 + 8;
    v16 = (char*)v0 + 8;
    v17 = *(int64_t*)v15;
    *(int64_t*)v16 = (int64_t)v17;
    return;
  bb3:
    v20 = (char*)v2 + 0;
    v21 = *(int64_t*)v20;
    v22 = 1LL;
    v23 = v21 == v22;
    if (v23) goto bb4;
    else goto bb5;
  bb4:
    v25 = (char*)v2 + 8;
    v26 = *(void**)v25;
    memset(v27, 0, 16);
    v28 = (char*)v27 + 0;
    v29 = 1LL;
    *(int64_t*)v28 = (int64_t)v29;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "label") == 0) {
            v31 = (void*)label((void*)v3);
        } else if (strcmp(_fn_name, "main") == 0) {
            v31 = (void*)main_entry((void*)v3);
        } else if (strcmp(_fn_name, "Vec_new") == 0) {
            v31 = (void*)Vec_new((void*)v3);
        } else if (strcmp(_fn_name, "HashMap_new") == 0) {
            v31 = (void*)HashMap_new((void*)v3);
        } else { abort(); }
    }
    v32 = _str2;
    v33 = p_str_concat(v31, v32);
    v34 = p_str_concat(v33, v26);
    v48 = 0LL;
    Str_drop((void*)v48, (int64_t)v33);
    v35 = (char*)v27 + 8;
    *(void**)v35 = (void*)v34;
    v37 = (char*)v27 + 0;
    v38 = (char*)v0 + 0;
    v39 = *(int64_t*)v37;
    *(int64_t*)v38 = (int64_t)v39;
    v41 = (char*)v27 + 8;
    v42 = (char*)v0 + 8;
    v43 = *(int64_t*)v41;
    *(int64_t*)v42 = (int64_t)v43;
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
    void* v12 = 0;
    int64_t v13 = 0;
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
    int64_t v33 = 0;
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
    v12 = (char*)v11 + 0;
    v13 = 0LL;
    *(int64_t*)v12 = (int64_t)v13;
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
    v32 = (char*)v31 + 0;
    v33 = 1LL;
    *(int64_t*)v32 = (int64_t)v33;
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
    p_sb_append(v4, v2);
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
    p_sb_append(v4, v6);
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
    p_sb_append(v8, v9);
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

void* StringBuilder_to_str(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = p_sb_to_str(v3);
    return v4;
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
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0; /* placeholder for void */
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
    *(void**)v12 = (void*)v13;
    v15 = (char*)v3 + 8;
    v16 = (char*)v0 + 8;
    v17 = *(int64_t*)v15;
    *(int64_t*)v16 = (int64_t)v17;
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
    int64_t v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0;
    int64_t v18 = 0;
    bool v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    void* v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0;
    void* v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0;
    void* v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0; /* placeholder for void */
    void* v32 = 0;
    uint8_t v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    void* v44 = 0;
    void* v45 = 0;
    void* v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    void* v48 = 0;
    void* v49 = 0;
    int64_t v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0; /* placeholder for void */
    v4 = alloca(16);
    v7 = alloca(16);
    v11 = alloca(8);
    v14 = alloca(8);
    v23 = alloca(8);
    v26 = alloca(8);
    v29 = alloca(8);
    v3 = 0LL;
    memset(v4, 0, 16);
    v5 = 0LL;
    ByteBuffer_new(v4, (void*)v5, v3);
    memset(v7, 0, 16);
    memcpy(v7, v4, 16);
    v9 = 0LL;
    v10 = (int64_t)String_len((void*)v9, (int64_t)v2);
    memset(v11, 0, 8);
    *(int64_t*)v11 = (int64_t)v10;
    v13 = 0LL;
    memset(v14, 0, 8);
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
    v22 = (void*)(intptr_t)((int64_t)v2 + v21);
    memset(v23, 0, 8);
    *(int64_t*)v23 = (int64_t)v22;
    v25 = *(int64_t*)v23;
    memset(v26, 0, 8);
    *(void**)v26 = (void*)v25;
    v28 = 0LL;
    memset(v29, 0, 8);
    *(int64_t*)v29 = (int64_t)v28;
    v32 = *(void**)v26;
    v33 = *(uint8_t*)v32;
    *(int64_t*)v29 = (int64_t)v33;
    v36 = *(int64_t*)v29;
    v37 = 0LL;
    ByteBuffer_write_byte((void*)v37, v7, v36);
    v39 = *(int64_t*)v14;
    v40 = 1LL;
    v41 = (int64_t)((uint64_t)v39 + (uint64_t)v40);
    *(int64_t*)v14 = (int64_t)v41;
    goto bb1;
  bb3:
    v44 = (char*)v7 + 0;
    v45 = (char*)v0 + 0;
    v46 = *(void**)v44;
    *(void**)v45 = (void*)v46;
    v48 = (char*)v7 + 8;
    v49 = (char*)v0 + 8;
    v50 = *(int64_t*)v48;
    *(int64_t*)v49 = (int64_t)v50;
    return;
    return;
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
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    int64_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    uint8_t v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0;
    int64_t v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0; /* placeholder for void */
    v5 = alloca(8);
    v13 = alloca(8);
    v16 = alloca(8);
    v19 = alloca(8);
    v4 = 0LL;
    memset(v5, 0, 8);
    *(int64_t*)v5 = (int64_t)v4;
    goto bb1;
  bb1:
    v8 = *(int64_t*)v5;
    v9 = v8 < v3;
    if (v9) goto bb2;
    else goto bb3;
  bb2:
    v11 = *(int64_t*)v5;
    v12 = p_add_i64(v2, v11);
    memset(v13, 0, 8);
    *(int64_t*)v13 = (int64_t)v12;
    v15 = *(int64_t*)v13;
    memset(v16, 0, 8);
    *(void**)v16 = (void*)v15;
    v18 = 0LL;
    memset(v19, 0, 8);
    *(int64_t*)v19 = (int64_t)v18;
    v22 = *(void**)v16;
    v23 = *(uint8_t*)v22;
    *(int64_t*)v19 = (int64_t)v23;
    v26 = *(int64_t*)v19;
    v27 = 0LL;
    ByteBuffer_write_byte((void*)v27, v1, v26);
    v29 = *(int64_t*)v5;
    v30 = 1LL;
    v31 = (int64_t)((uint64_t)v29 + (uint64_t)v30);
    *(int64_t*)v5 = (int64_t)v31;
    goto bb1;
  bb3:
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

int64_t ByteBuffer_len(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    return v3;
    return 0;
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

void* ByteBuffer_to_str(void* v0, void* v1) {
    void* v2 = 0;
    void* v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0;
    bool v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    void* v14 = 0;
    void* v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    void* v21 = 0;
    int64_t v22 = 0;
    void* v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0; /* placeholder for void */
    void* v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    v3 = alloca(8);
    v6 = alloca(8);
    v19 = alloca(8);
    v2 = _str3;
    memset(v3, 0, 8);
    *(void**)v3 = (void*)v2;
    v5 = 0LL;
    memset(v6, 0, 8);
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
    memset(v19, 0, 8);
    *(int64_t*)v19 = (int64_t)v18;
    v21 = *(void**)v3;
    v22 = *(int64_t*)v19;
    v23 = p_char_to_str(v22);
    v24 = p_str_concat(v21, v23);
    *(void**)v3 = (void*)v24;
    v26 = *(int64_t*)v6;
    v27 = 1LL;
    v28 = (int64_t)((uint64_t)v26 + (uint64_t)v27);
    *(int64_t*)v6 = (int64_t)v28;
    goto bb1;
  bb3:
    v31 = *(void**)v3;
    return v31;
    return 0;
}

void parse_port(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0;
    bool v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    void* v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    void* v19 = 0;
    void* v20 = 0;
    int64_t v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0; /* placeholder for void */
    void* v25 = 0;
    void* v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0;
    int64_t v30 = 0;
    void* v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    void* v33 = 0;
    void* v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    void* v37 = 0;
    void* v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0; /* placeholder for void */
    v8 = alloca(16);
    v25 = alloca(16);
    v3 = 0LL;
    v4 = (int64_t)str_len((void*)v3, v2);
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb1;
    else goto bb2;
  bb1:
    memset(v8, 0, 16);
    v9 = (char*)v8 + 0;
    v10 = 1LL;
    *(int64_t*)v9 = (int64_t)v10;
    v12 = _str4;
    v13 = (char*)v8 + 8;
    *(void**)v13 = (void*)v12;
    v15 = (char*)v8 + 0;
    v16 = (char*)v0 + 0;
    v17 = *(int64_t*)v15;
    *(int64_t*)v16 = (int64_t)v17;
    v19 = (char*)v8 + 8;
    v20 = (char*)v0 + 8;
    v21 = *(int64_t*)v19;
    *(int64_t*)v20 = (int64_t)v21;
    return;
  bb2:
    goto bb3;
  bb3:
    memset(v25, 0, 16);
    v26 = (char*)v25 + 0;
    v27 = 0LL;
    *(int64_t*)v26 = (int64_t)v27;
    v29 = 0LL;
    v30 = (int64_t)str_parse_int((void*)v29, v2);
    v31 = (char*)v25 + 8;
    *(int64_t*)v31 = (int64_t)v30;
    v33 = (char*)v25 + 0;
    v34 = (char*)v0 + 0;
    v35 = *(int64_t*)v33;
    *(int64_t*)v34 = (int64_t)v35;
    v37 = (char*)v25 + 8;
    v38 = (char*)v0 + 8;
    v39 = *(int64_t*)v37;
    *(int64_t*)v38 = (int64_t)v39;
    return;
    return;
}

void load_port(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    void* v6 = 0;
    int64_t v7 = 0;
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0;
    bool v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0; /* placeholder for void */
    void* v16 = 0;
    int64_t v17 = 0;
    void* v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    void* v20 = 0;
    int64_t v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0; /* placeholder for void */
    void* v25 = 0;
    int64_t v26 = 0;
    void* v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    void* v29 = 0;
    void* v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0;
    int64_t v34 = 0;
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
    v4 = alloca(16);
    v8 = alloca(16);
    v27 = alloca(8);
    v29 = alloca(16);
    v3 = 0LL;
    memset(v4, 0, 16);
    parse_port(v4, (void*)v3, v2);
    v6 = _str5;
    v7 = 0LL;
    memset(v8, 0, 16);
    Result_context(v8, (void*)v7, v4, v6);
    v10 = (char*)v8 + 0;
    v11 = *(int64_t*)v10;
    v12 = 0LL;
    v13 = v11 == v12;
    if (v13) goto bb1;
    else goto bb2;
  bb1:
    goto bb3;
  bb2:
    v16 = (char*)v8 + 0;
    v17 = *(int64_t*)v16;
    v18 = (char*)v0 + 0;
    *(int64_t*)v18 = (int64_t)v17;
    v20 = (char*)v8 + 8;
    v21 = *(int64_t*)v20;
    v22 = (char*)v0 + 8;
    *(int64_t*)v22 = (int64_t)v21;
    return;
  bb3:
    v25 = (char*)v8 + 8;
    v26 = *(int64_t*)v25;
    memset(v27, 0, 8);
    *(int64_t*)v27 = (int64_t)v26;
    memset(v29, 0, 16);
    v30 = (char*)v29 + 0;
    v31 = 0LL;
    *(int64_t*)v30 = (int64_t)v31;
    v33 = *(int64_t*)v27;
    v34 = 1LL;
    v35 = p_add_i64(v33, v34);
    v36 = (char*)v29 + 8;
    *(int64_t*)v36 = (int64_t)v35;
    v38 = (char*)v29 + 0;
    v39 = (char*)v0 + 0;
    v40 = *(int64_t*)v38;
    *(int64_t*)v39 = (int64_t)v40;
    v42 = (char*)v29 + 8;
    v43 = (char*)v0 + 8;
    v44 = *(int64_t*)v42;
    *(int64_t*)v43 = (int64_t)v44;
    return;
    return;
}

void find(void* v0, void* v1, void* v2, int64_t v3) {
    int64_t v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    int64_t v9 = 0;
    int64_t v10 = 0;
    bool v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0;
    bool v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    void* v25 = 0;
    void* v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    void* v29 = 0;
    void* v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0; /* placeholder for void */
    void* v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    void* v44 = 0;
    void* v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    void* v48 = 0;
    void* v49 = 0;
    int64_t v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0; /* placeholder for void */
    v5 = alloca(8);
    v40 = alloca(16);
    v18 = alloca(16);
    v4 = 0LL;
    memset(v5, 0, 8);
    *(int64_t*)v5 = (int64_t)v4;
    goto bb1;
  bb1:
    v8 = *(int64_t*)v5;
    v9 = 0LL;
    v10 = (int64_t)Vec_len((void*)v9, (int64_t)v2);
    v11 = v8 < v10;
    if (v11) goto bb2;
    else goto bb3;
  bb2:
    v13 = *(int64_t*)v5;
    v14 = 0LL;
    v15 = (int64_t)__p_vec_get_u((void*)v14, (int64_t)v2, v13);
    v16 = v15 == v3;
    if (v16) goto bb4;
    else goto bb5;
  bb3:
    memset(v40, 0, 16);
    v41 = (char*)v40 + 0;
    v42 = 1LL;
    *(int64_t*)v41 = (int64_t)v42;
    v44 = (char*)v40 + 0;
    v45 = (char*)v0 + 0;
    v46 = *(int64_t*)v44;
    *(int64_t*)v45 = (int64_t)v46;
    v48 = (char*)v40 + 8;
    v49 = (char*)v0 + 8;
    v50 = *(int64_t*)v48;
    *(int64_t*)v49 = (int64_t)v50;
    return;
  bb4:
    memset(v18, 0, 16);
    v19 = (char*)v18 + 0;
    v20 = 0LL;
    *(int64_t*)v19 = (int64_t)v20;
    v22 = *(int64_t*)v5;
    v23 = (char*)v18 + 8;
    *(int64_t*)v23 = (int64_t)v22;
    v25 = (char*)v18 + 0;
    v26 = (char*)v0 + 0;
    v27 = *(int64_t*)v25;
    *(int64_t*)v26 = (int64_t)v27;
    v29 = (char*)v18 + 8;
    v30 = (char*)v0 + 8;
    v31 = *(int64_t*)v29;
    *(int64_t*)v30 = (int64_t)v31;
    return;
  bb5:
    goto bb6;
  bb6:
    v35 = *(int64_t*)v5;
    v36 = 1LL;
    v37 = (int64_t)((uint64_t)v35 + (uint64_t)v36);
    *(int64_t*)v5 = (int64_t)v37;
    goto bb1;
    return;
}

void where_is(void* v0, void* v1, void* v2, int64_t v3) {
    int64_t v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    int64_t v12 = 0;
    int64_t v13 = 0;
    bool v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    void* v21 = 0;
    int64_t v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    void* v30 = 0;
    void* v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
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
    v5 = alloca(16);
    v9 = alloca(16);
    v28 = alloca(8);
    v30 = alloca(16);
    v4 = 0LL;
    memset(v5, 0, 16);
    find(v5, (void*)v4, v2, v3);
    v7 = _str6;
    v8 = 0LL;
    memset(v9, 0, 16);
    Option_context(v9, (void*)v8, v5, v7);
    v11 = (char*)v9 + 0;
    v12 = *(int64_t*)v11;
    v13 = 0LL;
    v14 = v12 == v13;
    if (v14) goto bb1;
    else goto bb2;
  bb1:
    goto bb3;
  bb2:
    v17 = (char*)v9 + 0;
    v18 = *(int64_t*)v17;
    v19 = (char*)v0 + 0;
    *(int64_t*)v19 = (int64_t)v18;
    v21 = (char*)v9 + 8;
    v22 = *(int64_t*)v21;
    v23 = (char*)v0 + 8;
    *(int64_t*)v23 = (int64_t)v22;
    return;
  bb3:
    v26 = (char*)v9 + 8;
    v27 = *(int64_t*)v26;
    memset(v28, 0, 8);
    *(int64_t*)v28 = (int64_t)v27;
    memset(v30, 0, 16);
    v31 = (char*)v30 + 0;
    v32 = 0LL;
    *(int64_t*)v31 = (int64_t)v32;
    v34 = *(int64_t*)v28;
    v35 = (char*)v30 + 8;
    *(int64_t*)v35 = (int64_t)v34;
    v37 = (char*)v30 + 0;
    v38 = (char*)v0 + 0;
    v39 = *(int64_t*)v37;
    *(int64_t*)v38 = (int64_t)v39;
    v41 = (char*)v30 + 8;
    v42 = (char*)v0 + 8;
    v43 = *(int64_t*)v41;
    *(int64_t*)v42 = (int64_t)v43;
    return;
    return;
}

void* label(void* v0) {
    void* v1 = 0;
    int64_t v2 = 0; /* placeholder for void */
    v1 = _str7;
    return v1;
    return 0;
}

void show(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0;
    bool v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    void* v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0;
    bool v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    void* v19 = 0;
    void* v20 = 0;
    void* v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    return;
  bb2:
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    p_print_int((int64_t)v8);
    v10 = _str8;
    p_print_str(v10);
    goto bb1;
  bb3:
    v13 = (char*)v1 + 0;
    v14 = *(int64_t*)v13;
    v15 = 1LL;
    v16 = v14 == v15;
    if (v16) goto bb4;
    else goto bb5;
  bb4:
    v18 = (char*)v1 + 8;
    v19 = *(void**)v18;
    v20 = _str9;
    v21 = p_str_concat(v20, v19);
    p_print_str(v21);
    v28 = 0LL;
    Str_drop((void*)v28, (int64_t)v21);
    v23 = _str8;
    p_print_str(v23);
    goto bb1;
  bb5:
    goto bb1;
    return;
}

int64_t main_entry(void* v0) {
    void* v1 = 0;
    int64_t v2 = 0;
    void* v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0;
    void* v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    void* v14 = 0;
    void* v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    int64_t v18 = 0;
    int64_t v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    void* v21 = 0;
    int64_t v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    void* v25 = 0;
    int64_t v26 = 0;
    int64_t v27 = 0;
    void* v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    void* v32 = 0;
    int64_t v33 = 0;
    int64_t v34 = 0;
    void* v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    void* v39 = 0;
    int64_t v40 = 0;
    void* v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    void* v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    void* v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    void* v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    void* v54 = 0;
    int64_t v55 = 0;
    void* v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    void* v58 = 0;
    int64_t v59 = 0;
    int64_t v60 = 0;
    bool v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    void* v63 = 0;
    void* v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    void* v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0; /* placeholder for void */
    void* v69 = 0;
    int64_t v70 = 0;
    int64_t v71 = 0;
    bool v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    void* v74 = 0;
    void* v75 = 0;
    int64_t v76 = 0; /* placeholder for void */
    void* v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0;
    int64_t v82 = 0; /* placeholder for void */
    int64_t v83 = 0;
    void* v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0;
    int64_t v87 = 0;
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0;
    int64_t v91 = 0;
    int64_t v92 = 0; /* placeholder for void */
    v84 = alloca(8);
    v3 = alloca(16);
    v9 = alloca(16);
    v15 = alloca(8);
    v28 = alloca(16);
    v35 = alloca(16);
    v41 = alloca(16);
    v50 = alloca(16);
    v56 = alloca(16);
    v83 = 0LL;
    memset(v84, 0, 8);
    *(int64_t*)v84 = (int64_t)v83;
    v1 = _str10;
    v2 = 0LL;
    memset(v3, 0, 16);
    load_port(v3, (void*)v2, v1);
    v5 = 0LL;
    show((void*)v5, v3);
    v7 = _str3;
    v8 = 0LL;
    memset(v9, 0, 16);
    load_port(v9, (void*)v8, v7);
    v11 = 0LL;
    show((void*)v11, v9);
    v13 = 0LL;
    v14 = (void*)Vec_new((void*)v13);
    memset(v15, 0, 8);
    v86 = *(int64_t*)v84;
    v87 = 0LL;
    Vec_drop((void*)v87, v86);
    *(void**)v15 = (void*)v14;
    *(int64_t*)v84 = (int64_t)v14;
    v17 = *(void**)v15;
    v18 = 3LL;
    v19 = 0LL;
    Vec_push((void*)v19, (int64_t)v17, v18);
    v21 = *(void**)v15;
    v22 = 7LL;
    v23 = 0LL;
    Vec_push((void*)v23, (int64_t)v21, v22);
    v25 = *(void**)v15;
    v26 = 7LL;
    v27 = 0LL;
    memset(v28, 0, 16);
    where_is(v28, (void*)v27, v25, v26);
    v30 = 0LL;
    show((void*)v30, v28);
    v32 = *(void**)v15;
    v33 = 9LL;
    v34 = 0LL;
    memset(v35, 0, 16);
    where_is(v35, (void*)v34, v32, v33);
    v37 = 0LL;
    show((void*)v37, v35);
    v39 = _str3;
    v40 = 0LL;
    memset(v41, 0, 16);
    parse_port(v41, (void*)v40, v39);
    v43 = 8LL;
    v44 = p_malloc(v43);
    v45 = (int64_t)_str11;
    v46 = 0LL;
    v47 = (char*)v44 + (v46 * 8);
    *(int64_t*)v47 = (int64_t)v45;
    v49 = 0LL;
    memset(v50, 0, 16);
    Result_with_context(v50, (void*)v49, v41, v44);
    v52 = 0LL;
    show((void*)v52, v50);
    v54 = _str12;
    v55 = 0LL;
    memset(v56, 0, 16);
    file_read_result(v56, (void*)v55, v54);
    v58 = (char*)v56 + 0;
    v59 = *(int64_t*)v58;
    v60 = 0LL;
    v61 = v59 == v60;
    if (v61) goto bb2;
    else goto bb3;
  bb1:
    v81 = 0LL;
    v90 = *(int64_t*)v84;
    v91 = 0LL;
    Vec_drop((void*)v91, v90);
    return v81;
  bb2:
    v63 = (char*)v56 + 8;
    v64 = *(void**)v63;
    p_print_str(v64);
    v66 = _str8;
    p_print_str(v66);
    goto bb1;
  bb3:
    v69 = (char*)v56 + 0;
    v70 = *(int64_t*)v69;
    v71 = 1LL;
    v72 = v70 == v71;
    if (v72) goto bb4;
    else goto bb5;
  bb4:
    v74 = (char*)v56 + 8;
    v75 = *(void**)v74;
    p_print_str(v75);
    v77 = _str8;
    p_print_str(v77);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

void* file_read(void* v0, void* v1) {
    void* v2 = 0;
    void* v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    void* v5 = 0;
    int64_t v6 = 0;
    int64_t v7 = 0;
    bool v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0; /* placeholder for void */
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    v3 = alloca(8);
    v2 = p_file_read(v1);
    memset(v3, 0, 8);
    *(void**)v3 = (void*)v2;
    v5 = *(void**)v3;
    v6 = (int64_t)(intptr_t)v5;
    v7 = 0LL;
    v8 = v6 == v7;
    if (v8) goto bb1;
    else goto bb2;
  bb1:
    v10 = _str3;
    return v10;
  bb2:
    goto bb3;
  bb3:
    v13 = *(void**)v3;
    return v13;
    return 0;
}

void file_read_result(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    void* v6 = 0;
    int64_t v7 = 0;
    int64_t v8 = 0;
    bool v9 = 0;
    int64_t v10 = 0; /* placeholder for void */
    void* v11 = 0;
    void* v12 = 0;
    int64_t v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    void* v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    void* v19 = 0;
    void* v20 = 0;
    int64_t v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    void* v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0; /* placeholder for void */
    void* v29 = 0;
    void* v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    void* v33 = 0;
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
    v4 = alloca(8);
    v11 = alloca(16);
    v29 = alloca(16);
    v3 = p_file_read(v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    v6 = *(void**)v4;
    v7 = (int64_t)(intptr_t)v6;
    v8 = 0LL;
    v9 = v7 == v8;
    if (v9) goto bb1;
    else goto bb2;
  bb1:
    memset(v11, 0, 16);
    v12 = (char*)v11 + 0;
    v13 = 1LL;
    *(int64_t*)v12 = (int64_t)v13;
    v15 = _str13;
    v16 = p_str_concat(v15, v2);
    v17 = (char*)v11 + 8;
    *(void**)v17 = (void*)v16;
    v19 = (char*)v11 + 0;
    v20 = (char*)v0 + 0;
    v21 = *(int64_t*)v19;
    *(int64_t*)v20 = (int64_t)v21;
    v23 = (char*)v11 + 8;
    v24 = (char*)v0 + 8;
    v25 = *(int64_t*)v23;
    *(int64_t*)v24 = (int64_t)v25;
    return;
  bb2:
    goto bb3;
  bb3:
    memset(v29, 0, 16);
    v30 = (char*)v29 + 0;
    v31 = 0LL;
    *(int64_t*)v30 = (int64_t)v31;
    v33 = *(void**)v4;
    v34 = (char*)v29 + 8;
    *(void**)v34 = (void*)v33;
    v36 = (char*)v29 + 0;
    v37 = (char*)v0 + 0;
    v38 = *(int64_t*)v36;
    *(int64_t*)v37 = (int64_t)v38;
    v40 = (char*)v29 + 8;
    v41 = (char*)v0 + 8;
    v42 = *(int64_t*)v40;
    *(int64_t*)v41 = (int64_t)v42;
    return;
    return;
}

int64_t file_write(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v3 = (int64_t)p_file_write(v1, v2);
    return v3;
    return 0;
}

int64_t file_append(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v3 = (int64_t)p_file_append(v1, v2);
    return v3;
    return 0;
}

bool file_exists(void* v0, void* v1) {
    bool v2 = 0;
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v2 = (bool)p_file_exists(v1);
    v3 = 0LL;
    v4 = v2 != v3;
    return v4;
    return 0;
}

int64_t file_write_bytes(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v3 = (int64_t)p_file_write_vec(v1, v2);
    return v3;
    return 0;
}

void* file_read_bytes(void* v0, void* v1) {
    void* v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = p_file_read_bytes(v1);
    return v2;
    return 0;
}

int64_t file_write_raw(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v3 = (int64_t)p_file_write(v1, v2);
    return v3;
    return 0;
}

int64_t file_delete(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = (int64_t)p_file_delete(v1);
    return v2;
    return 0;
}

int64_t file_size(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = (int64_t)p_file_size(v1);
    return v2;
    return 0;
}

void* fs_read_text(void* v0, void* v1) {
    int64_t v2 = 0;
    void* v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = file_read((void*)v2, v1);
    return v3;
    return 0;
}

int64_t fs_write_text(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)file_write((void*)v3, v1, v2);
    return v4;
    return 0;
}

int64_t fs_append_text(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)file_append((void*)v3, v1, v2);
    return v4;
    return 0;
}

bool fs_exists(void* v0, void* v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (bool)file_exists((void*)v2, v1);
    return v3;
    return 0;
}

int64_t fs_delete(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)file_delete((void*)v2, v1);
    return v3;
    return 0;
}

int64_t fs_size(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)file_size((void*)v2, v1);
    return v3;
    return 0;
}

void println(void* v0, void* v1) {
    int64_t v2 = 0; /* placeholder for void */
    void* v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0; /* placeholder for void */
    p_print_str(v1);
    v3 = _str8;
    p_print_str(v3);
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
    v4 = _str14;
    p_panic(v4);
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
    p_panic(v2);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    return;
    return;
}

void panic(void* v0, void* v1) {
    int64_t v2 = 0; /* placeholder for void */
    int64_t v3 = 0; /* placeholder for void */
    p_panic(v1);
    return;
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

int64_t allocator_alloc(void* v0, void* v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v3 = (int64_t)p_allocator_alloc(v1, v2);
    return v3;
    return 0;
}

void allocator_free(void* v0, void* v1, int64_t v2) {
    int64_t v3 = 0; /* placeholder for void */
    int64_t v4 = 0; /* placeholder for void */
    p_allocator_free(v1, v2);
    return;
    return;
}

int64_t allocator_bytes_used(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = (int64_t)p_allocator_stats(v1);
    return v2;
    return 0;
}

int64_t Vec_new(void* v0) {
    int64_t* __h = (int64_t*)p_malloc(24);
    __h[0] = 0; __h[1] = 0; __h[2] = 0;
    return (int64_t)(intptr_t)__h;
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
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0; /* placeholder for void */
    v7 = alloca(8);
    v10 = alloca(8);
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
    v9 = 0LL;
    memset(v10, 0, 8);
    *(int64_t*)v10 = (int64_t)v9;
    v13 = *(void**)v7;
    v14 = *(int64_t*)v13;
    *(int64_t*)v10 = (int64_t)v14;
    v17 = *(int64_t*)v10;
    v18 = 0LL;
    v19 = v17 != v18;
    if (v19) goto bb4;
    else goto bb5;
  bb4:
    v21 = *(int64_t*)v10;
    p_free((void*)v21);
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    p_free((void*)v1);
    return;
    return;
}

void Str_drop(void* v0, int64_t v1) {
    int64_t v2 = 0; /* placeholder for void */
    int64_t v3 = 0; /* placeholder for void */
    p_free((void*)v1);
    return;
    return;
}

void Vec_push(void* v0, int64_t v1, int64_t v2) {
    int64_t* __h = (int64_t*)(intptr_t)v1;
    int64_t __len = __h[1];
    if (P_UNLIKELY(__len >= __h[2])) __p_vec_grow(__h);
    ((int64_t*)(intptr_t)__h[0])[__len] = v2;
    __h[1] = __len + 1;
}

int64_t Vec_pop(void* v0, int64_t v1) {
    int64_t* __h = (int64_t*)(intptr_t)v1;
    if (__h[1] == 0) return 0;
    __h[1] = __h[1] - 1;
    return ((int64_t*)(intptr_t)__h[0])[__h[1]];
}

int64_t Vec_len(void* v0, int64_t v1) {
    int64_t* __h = (int64_t*)(intptr_t)v1;
    return __h[1];
}

bool Vec_is_empty(void* v0, int64_t v1) {
    int64_t* __h = (int64_t*)(intptr_t)v1;
    return __h[1] == 0;
}

int64_t Vec_get(void* v0, int64_t v1, int64_t v2) {
    int64_t* __h = (int64_t*)(intptr_t)v1;
    if (v2 >= __h[1]) p_panic("Vec index out of bounds");
    if (v2 < 0) p_panic("Vec index out of bounds");
    return ((int64_t*)(intptr_t)__h[0])[v2];
}

int64_t vec_get_or(void* v0, void* v1, int64_t v2, int64_t v3) {
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
    int64_t v14 = 0; /* placeholder for void */
    void* v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    void* v19 = 0;
    int64_t v20 = 0;
    bool v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    bool v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    bool v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0; /* placeholder for void */
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
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    void* v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    void* v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0; /* placeholder for void */
    void* v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
    v4 = alloca(8);
    v9 = alloca(8);
    v12 = alloca(8);
    v19 = alloca(1);
    v33 = alloca(8);
    v36 = alloca(8);
    v47 = alloca(8);
    v50 = alloca(8);
    memset(v4, 0, 8);
    *(int64_t*)v4 = (int64_t)v1;
    v6 = *(int64_t*)v4;
    v7 = 8LL;
    v8 = (int64_t)((uint64_t)v6 + (uint64_t)v7);
    memset(v9, 0, 8);
    *(void**)v9 = (void*)v8;
    v11 = 0LL;
    memset(v12, 0, 8);
    *(int64_t*)v12 = (int64_t)v11;
    v15 = *(void**)v9;
    v16 = *(int64_t*)v15;
    *(int64_t*)v12 = (int64_t)v16;
    memset(v19, 0, 1);
    v20 = 0LL;
    v21 = v2 < v20;
    *(bool*)v19 = v21;
    if (v21) goto bb2;
    else goto bb1;
  bb1:
    v24 = *(int64_t*)v12;
    v25 = v2 >= v24;
    *(bool*)v19 = v25;
    goto bb2;
  bb2:
    v28 = *(bool*)v19;
    if (v28) goto bb3;
    else goto bb4;
  bb3:
    return v3;
  bb4:
    goto bb5;
  bb5:
    v32 = *(int64_t*)v4;
    memset(v33, 0, 8);
    *(void**)v33 = (void*)v32;
    v35 = 0LL;
    memset(v36, 0, 8);
    *(int64_t*)v36 = (int64_t)v35;
    v39 = *(void**)v33;
    v40 = *(int64_t*)v39;
    *(int64_t*)v36 = (int64_t)v40;
    v43 = *(int64_t*)v36;
    v44 = 8LL;
    v45 = (int64_t)((uint64_t)v2 * (uint64_t)v44);
    v46 = (int64_t)((uint64_t)v43 + (uint64_t)v45);
    memset(v47, 0, 8);
    *(void**)v47 = (void*)v46;
    v49 = 0LL;
    memset(v50, 0, 8);
    *(int64_t*)v50 = (int64_t)v49;
    v53 = *(void**)v47;
    v54 = *(int64_t*)v53;
    *(int64_t*)v50 = (int64_t)v54;
    v57 = *(int64_t*)v50;
    return v57;
    return 0;
}

void Vec_set(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    int64_t* __h = (int64_t*)(intptr_t)v1;
    if (v2 >= __h[1]) p_panic("Vec index out of bounds");
    if (v2 < 0) p_panic("Vec index out of bounds");
    ((int64_t*)(intptr_t)__h[0])[v2] = v3;
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
    int64_t v13 = 0; /* placeholder for void */
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
    return v12;
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
    int64_t v25 = 0;
    int64_t v26 = 0;
    bool v27 = 0;
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
    int64_t v39 = 0;
    void* v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0; /* placeholder for void */
    void* v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0;
    bool v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    bool v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    bool v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    v5 = alloca(8);
    v7 = alloca(8);
    v9 = alloca(8);
    v15 = alloca(8);
    v22 = alloca(8);
    v34 = alloca(8);
    v37 = alloca(8);
    v40 = alloca(8);
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
    v25 = *(int64_t*)v22;
    v26 = *(int64_t*)v9;
    v27 = v25 < v26;
    if (v27) goto bb2;
    else goto bb3;
  bb2:
    v29 = *(int64_t*)v15;
    v30 = *(int64_t*)v22;
    v31 = 8LL;
    v32 = p_mul_i64(v30, v31);
    v33 = p_add_i64(v29, v32);
    memset(v34, 0, 8);
    *(int64_t*)v34 = (int64_t)v33;
    v36 = *(int64_t*)v34;
    memset(v37, 0, 8);
    *(void**)v37 = (void*)v36;
    v39 = 0LL;
    memset(v40, 0, 8);
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
    return v58;
  bb4:
    v50 = true;
    return v50;
  bb5:
    goto bb6;
  bb6:
    v53 = *(int64_t*)v22;
    v54 = 1LL;
    v55 = (int64_t)((uint64_t)v53 + (uint64_t)v54);
    *(int64_t*)v22 = (int64_t)v55;
    goto bb1;
    return 0;
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
    v18 = _str16;
    p_panic(v18);
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
    v18 = _str16;
    p_panic(v18);
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

void Vec_reverse(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    vec_raw_reverse((void*)v2, v1);
    return;
    return;
}

void vec_raw_reverse(void* v0, int64_t v1) {
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
    void* v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    int64_t v25 = 0;
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
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0;
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
    void* v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
    void* v59 = 0;
    int64_t v60 = 0;
    void* v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    void* v63 = 0;
    int64_t v64 = 0;
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
    v4 = alloca(8);
    v6 = alloca(8);
    v8 = alloca(8);
    v14 = alloca(8);
    v21 = alloca(8);
    v35 = alloca(8);
    v46 = alloca(8);
    v49 = alloca(8);
    v52 = alloca(8);
    v57 = alloca(8);
    v61 = alloca(8);
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
    memset(v21, 0, 8);
    *(int64_t*)v21 = (int64_t)v20;
    goto bb1;
  bb1:
    v24 = *(int64_t*)v21;
    v25 = *(int64_t*)v8;
    v26 = 2LL;
    v27 = p_div_i64(v25, v26);
    v28 = v24 < v27;
    if (v28) goto bb2;
    else goto bb3;
  bb2:
    v30 = *(int64_t*)v14;
    v31 = *(int64_t*)v21;
    v32 = 8LL;
    v33 = p_mul_i64(v31, v32);
    v34 = p_add_i64(v30, v33);
    memset(v35, 0, 8);
    *(int64_t*)v35 = (int64_t)v34;
    v37 = *(int64_t*)v14;
    v38 = *(int64_t*)v8;
    v39 = 1LL;
    v40 = p_sub_i64(v38, v39);
    v41 = *(int64_t*)v21;
    v42 = p_sub_i64(v40, v41);
    v43 = 8LL;
    v44 = p_mul_i64(v42, v43);
    v45 = p_add_i64(v37, v44);
    memset(v46, 0, 8);
    *(int64_t*)v46 = (int64_t)v45;
    v48 = *(int64_t*)v35;
    memset(v49, 0, 8);
    *(void**)v49 = (void*)v48;
    v51 = *(int64_t*)v46;
    memset(v52, 0, 8);
    *(void**)v52 = (void*)v51;
    v55 = *(void**)v49;
    v56 = *(int64_t*)v55;
    memset(v57, 0, 8);
    *(int64_t*)v57 = (int64_t)v56;
    v59 = *(void**)v52;
    v60 = *(int64_t*)v59;
    memset(v61, 0, 8);
    *(int64_t*)v61 = (int64_t)v60;
    v63 = *(void**)v49;
    v64 = *(int64_t*)v61;
    *(int64_t*)v63 = (int64_t)v64;
    v66 = *(void**)v52;
    v67 = *(int64_t*)v57;
    *(int64_t*)v66 = (int64_t)v67;
    v70 = *(int64_t*)v21;
    v71 = 1LL;
    v72 = (int64_t)((uint64_t)v70 + (uint64_t)v71);
    *(int64_t*)v21 = (int64_t)v72;
    goto bb1;
  bb3:
    return;
    return;
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

void vec_raw_extend(void* v0, int64_t v1, int64_t v2) {
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
    int64_t v25 = 0;
    int64_t v26 = 0;
    bool v27 = 0;
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
    int64_t v39 = 0;
    void* v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0; /* placeholder for void */
    void* v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0;
    int64_t v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0; /* placeholder for void */
    v5 = alloca(8);
    v7 = alloca(8);
    v9 = alloca(8);
    v15 = alloca(8);
    v22 = alloca(8);
    v34 = alloca(8);
    v37 = alloca(8);
    v40 = alloca(8);
    v3 = 8LL;
    v4 = p_add_i64(v2, v3);
    memset(v5, 0, 8);
    *(void**)v5 = (void*)v4;
    memset(v7, 0, 8);
    *(void**)v7 = (void*)v2;
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
    v25 = *(int64_t*)v22;
    v26 = *(int64_t*)v9;
    v27 = v25 < v26;
    if (v27) goto bb2;
    else goto bb3;
  bb2:
    v29 = *(int64_t*)v15;
    v30 = *(int64_t*)v22;
    v31 = 8LL;
    v32 = p_mul_i64(v30, v31);
    v33 = p_add_i64(v29, v32);
    memset(v34, 0, 8);
    *(int64_t*)v34 = (int64_t)v33;
    v36 = *(int64_t*)v34;
    memset(v37, 0, 8);
    *(void**)v37 = (void*)v36;
    v39 = 0LL;
    memset(v40, 0, 8);
    *(int64_t*)v40 = (int64_t)v39;
    v43 = *(void**)v37;
    v44 = *(int64_t*)v43;
    *(int64_t*)v40 = (int64_t)v44;
    v47 = *(int64_t*)v40;
    v48 = 0LL;
    Vec_push((void*)v48, v1, v47);
    v50 = *(int64_t*)v22;
    v51 = 1LL;
    v52 = (int64_t)((uint64_t)v50 + (uint64_t)v51);
    *(int64_t*)v22 = (int64_t)v52;
    goto bb1;
  bb3:
    return;
    return;
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

void vec_raw_insert(void* v0, int64_t v1, int64_t v2, int64_t v3) {
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
    int64_t v16 = 0;
    bool v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    bool v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    void* v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0; /* placeholder for void */
    void* v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    void* v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    void* v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
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
    int64_t v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0;
    int64_t v65 = 0;
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
    int64_t v77 = 0;
    void* v78 = 0;
    int64_t v79 = 0; /* placeholder for void */
    void* v80 = 0;
    int64_t v81 = 0;
    int64_t v82 = 0; /* placeholder for void */
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0;
    int64_t v85 = 0;
    int64_t v86 = 0;
    int64_t v87 = 0; /* placeholder for void */
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0;
    int64_t v90 = 0;
    int64_t v91 = 0;
    int64_t v92 = 0;
    void* v93 = 0;
    int64_t v94 = 0; /* placeholder for void */
    int64_t v95 = 0;
    void* v96 = 0;
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0; /* placeholder for void */
    void* v99 = 0;
    int64_t v100 = 0; /* placeholder for void */
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0; /* placeholder for void */
    v6 = alloca(8);
    v8 = alloca(8);
    v10 = alloca(8);
    v38 = alloca(8);
    v47 = alloca(8);
    v58 = alloca(8);
    v67 = alloca(8);
    v70 = alloca(8);
    v73 = alloca(8);
    v78 = alloca(8);
    v93 = alloca(8);
    v96 = alloca(8);
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
    v16 = 0LL;
    v17 = v2 < v16;
    if (v17) goto bb1;
    else goto bb2;
  bb1:
    v19 = _str17;
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
    v26 = _str17;
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
    memset(v38, 0, 8);
    v40 = *(void**)v8;
    v41 = *(int64_t*)v40;
    *(int64_t*)v38 = (int64_t)v41;
    v44 = *(int64_t*)v10;
    v45 = 1LL;
    v46 = p_sub_i64(v44, v45);
    memset(v47, 0, 8);
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
    v56 = p_mul_i64(v54, v55);
    v57 = p_add_i64(v53, v56);
    memset(v58, 0, 8);
    *(int64_t*)v58 = (int64_t)v57;
    v60 = *(int64_t*)v38;
    v61 = *(int64_t*)v47;
    v62 = 1LL;
    v63 = p_sub_i64(v61, v62);
    v64 = 8LL;
    v65 = p_mul_i64(v63, v64);
    v66 = p_add_i64(v60, v65);
    memset(v67, 0, 8);
    *(int64_t*)v67 = (int64_t)v66;
    v69 = *(int64_t*)v58;
    memset(v70, 0, 8);
    *(void**)v70 = (void*)v69;
    v72 = *(int64_t*)v67;
    memset(v73, 0, 8);
    *(void**)v73 = (void*)v72;
    v76 = *(void**)v73;
    v77 = *(int64_t*)v76;
    memset(v78, 0, 8);
    *(int64_t*)v78 = (int64_t)v77;
    v80 = *(void**)v70;
    v81 = *(int64_t*)v78;
    *(int64_t*)v80 = (int64_t)v81;
    v84 = *(int64_t*)v47;
    v85 = 1LL;
    v86 = p_sub_i64(v84, v85);
    *(int64_t*)v47 = (int64_t)v86;
    goto bb7;
  bb9:
    v89 = *(int64_t*)v38;
    v90 = 8LL;
    v91 = p_mul_i64(v2, v90);
    v92 = p_add_i64(v89, v91);
    memset(v93, 0, 8);
    *(int64_t*)v93 = (int64_t)v92;
    v95 = *(int64_t*)v93;
    memset(v96, 0, 8);
    *(void**)v96 = (void*)v95;
    v99 = *(void**)v96;
    *(int64_t*)v99 = (int64_t)v3;
    return;
    return;
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

int64_t vec_raw_remove(void* v0, int64_t v1, int64_t v2) {
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
    int64_t v15 = 0;
    bool v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    void* v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    bool v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    void* v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0; /* placeholder for void */
    void* v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    void* v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0;
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
    void* v48 = 0;
    int64_t v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0; /* placeholder for void */
    void* v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0; /* placeholder for void */
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
    int64_t v65 = 0;
    void* v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    int64_t v69 = 0;
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
    void* v81 = 0;
    int64_t v82 = 0; /* placeholder for void */
    int64_t v83 = 0; /* placeholder for void */
    void* v84 = 0;
    int64_t v85 = 0;
    void* v86 = 0;
    int64_t v87 = 0; /* placeholder for void */
    void* v88 = 0;
    int64_t v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0;
    int64_t v93 = 0;
    int64_t v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0;
    int64_t v98 = 0;
    int64_t v99 = 0;
    void* v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0; /* placeholder for void */
    void* v103 = 0;
    int64_t v104 = 0;
    int64_t v105 = 0; /* placeholder for void */
    int64_t v106 = 0; /* placeholder for void */
    int64_t v107 = 0;
    int64_t v108 = 0; /* placeholder for void */
    v5 = alloca(8);
    v7 = alloca(8);
    v9 = alloca(8);
    v29 = alloca(8);
    v39 = alloca(8);
    v42 = alloca(8);
    v45 = alloca(8);
    v52 = alloca(8);
    v66 = alloca(8);
    v75 = alloca(8);
    v78 = alloca(8);
    v81 = alloca(8);
    v86 = alloca(8);
    v100 = alloca(8);
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
    v15 = 0LL;
    v16 = v2 < v15;
    if (v16) goto bb1;
    else goto bb2;
  bb1:
    v18 = _str18;
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
    v25 = _str18;
    p_panic(v25);
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    memset(v29, 0, 8);
    v31 = *(void**)v7;
    v32 = *(int64_t*)v31;
    *(int64_t*)v29 = (int64_t)v32;
    v35 = *(int64_t*)v29;
    v36 = 8LL;
    v37 = p_mul_i64(v2, v36);
    v38 = p_add_i64(v35, v37);
    memset(v39, 0, 8);
    *(int64_t*)v39 = (int64_t)v38;
    v41 = *(int64_t*)v39;
    memset(v42, 0, 8);
    *(void**)v42 = (void*)v41;
    v44 = 0LL;
    memset(v45, 0, 8);
    *(int64_t*)v45 = (int64_t)v44;
    v48 = *(void**)v42;
    v49 = *(int64_t*)v48;
    *(int64_t*)v45 = (int64_t)v49;
    memset(v52, 0, 8);
    *(int64_t*)v52 = (int64_t)v2;
    goto bb7;
  bb7:
    v55 = *(int64_t*)v52;
    v56 = *(int64_t*)v9;
    v57 = 1LL;
    v58 = p_sub_i64(v56, v57);
    v59 = v55 < v58;
    if (v59) goto bb8;
    else goto bb9;
  bb8:
    v61 = *(int64_t*)v29;
    v62 = *(int64_t*)v52;
    v63 = 8LL;
    v64 = p_mul_i64(v62, v63);
    v65 = p_add_i64(v61, v64);
    memset(v66, 0, 8);
    *(int64_t*)v66 = (int64_t)v65;
    v68 = *(int64_t*)v29;
    v69 = *(int64_t*)v52;
    v70 = 1LL;
    v71 = p_add_i64(v69, v70);
    v72 = 8LL;
    v73 = p_mul_i64(v71, v72);
    v74 = p_add_i64(v68, v73);
    memset(v75, 0, 8);
    *(int64_t*)v75 = (int64_t)v74;
    v77 = *(int64_t*)v66;
    memset(v78, 0, 8);
    *(void**)v78 = (void*)v77;
    v80 = *(int64_t*)v75;
    memset(v81, 0, 8);
    *(void**)v81 = (void*)v80;
    v84 = *(void**)v81;
    v85 = *(int64_t*)v84;
    memset(v86, 0, 8);
    *(int64_t*)v86 = (int64_t)v85;
    v88 = *(void**)v78;
    v89 = *(int64_t*)v86;
    *(int64_t*)v88 = (int64_t)v89;
    v92 = *(int64_t*)v52;
    v93 = 1LL;
    v94 = (int64_t)((uint64_t)v92 + (uint64_t)v93);
    *(int64_t*)v52 = (int64_t)v94;
    goto bb7;
  bb9:
    v97 = *(int64_t*)v9;
    v98 = 1LL;
    v99 = p_sub_i64(v97, v98);
    memset(v100, 0, 8);
    *(int64_t*)v100 = (int64_t)v99;
    v103 = *(void**)v5;
    v104 = *(int64_t*)v100;
    *(int64_t*)v103 = (int64_t)v104;
    v107 = *(int64_t*)v45;
    return v107;
    return 0;
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
    int64_t v52 = 0;
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0; /* placeholder for void */
    int64_t v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    v4 = alloca(8);
    v6 = alloca(8);
    v8 = alloca(8);
    v14 = alloca(8);
    v22 = alloca(8);
    v25 = alloca(8);
    v37 = alloca(8);
    v40 = alloca(8);
    v43 = alloca(8);
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
    v28 = *(int64_t*)v25;
    v29 = *(int64_t*)v8;
    v30 = v28 < v29;
    if (v30) goto bb2;
    else goto bb3;
  bb2:
    v32 = *(int64_t*)v14;
    v33 = *(int64_t*)v25;
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
    v50 = *(int64_t*)v22;
    v51 = *(int64_t*)v43;
    v52 = 0LL;
    Vec_push((void*)v52, v50, v51);
    v54 = *(int64_t*)v25;
    v55 = 1LL;
    v56 = (int64_t)((uint64_t)v54 + (uint64_t)v55);
    *(int64_t*)v25 = (int64_t)v56;
    goto bb1;
  bb3:
    v59 = *(int64_t*)v22;
    return v59;
    return 0;
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
    int64_t v27 = 0;
    int64_t v28 = 0;
    bool v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0;
    int64_t v32 = 0;
    bool v33 = 0;
    int64_t v34 = 0; /* placeholder for void */
    void* v35 = 0;
    void* v36 = 0;
    void* v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
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
    void* v61 = 0;
    void* v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0;
    int64_t v65 = 0;
    int64_t v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0; /* placeholder for void */
    void* v69 = 0;
    void* v70 = 0;
    void* v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    void* v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    v4 = alloca(8);
    v6 = alloca(8);
    v8 = alloca(8);
    v14 = alloca(8);
    v21 = alloca(8);
    v24 = alloca(8);
    v46 = alloca(8);
    v49 = alloca(8);
    v52 = alloca(8);
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
    v20 = _str19;
    memset(v21, 0, 8);
    *(void**)v21 = (void*)v20;
    v23 = 0LL;
    memset(v24, 0, 8);
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
    v70 = _str20;
    v71 = p_str_concat(v69, v70);
    *(void**)v21 = (void*)v71;
    v73 = *(void**)v21;
    return v73;
  bb4:
    v35 = *(void**)v21;
    v36 = _str21;
    v37 = p_str_concat(v35, v36);
    *(void**)v21 = (void*)v37;
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v41 = *(int64_t*)v14;
    v42 = *(int64_t*)v24;
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
    v59 = *(void**)v21;
    v60 = *(int64_t*)v52;
    v61 = (void*)p_to_str((int64_t)v60);
    v62 = p_str_concat(v59, v61);
    *(void**)v21 = (void*)v62;
    v64 = *(int64_t*)v24;
    v65 = 1LL;
    v66 = (int64_t)((uint64_t)v64 + (uint64_t)v65);
    *(int64_t*)v24 = (int64_t)v66;
    goto bb1;
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

void vec_raw_sort(void* v0, int64_t v1) {
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
    bool v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0;
    int64_t v30 = 0;
    void* v31 = 0;
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    int64_t v35 = 0;
    bool v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    int64_t v49 = 0;
    int64_t v50 = 0;
    void* v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    int64_t v55 = 0;
    bool v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    int64_t v59 = 0;
    int64_t v60 = 0;
    int64_t v61 = 0;
    int64_t v62 = 0; /* placeholder for void */
    int64_t v63 = 0;
    int64_t v64 = 0;
    int64_t v65 = 0;
    int64_t v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    int64_t v69 = 0;
    int64_t v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0; /* placeholder for void */
    v4 = alloca(8);
    v6 = alloca(8);
    v8 = alloca(8);
    v14 = alloca(8);
    v31 = alloca(8);
    v51 = alloca(8);
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
    v20 = *(int64_t*)v8;
    v21 = 1LL;
    v22 = v20 <= v21;
    if (v22) goto bb1;
    else goto bb2;
  bb1:
    return;
  bb2:
    goto bb3;
  bb3:
    v26 = *(int64_t*)v8;
    v27 = 2LL;
    v28 = p_div_i64(v26, v27);
    v29 = 1LL;
    v30 = p_sub_i64(v28, v29);
    memset(v31, 0, 8);
    *(int64_t*)v31 = (int64_t)v30;
    goto bb4;
  bb4:
    v34 = *(int64_t*)v31;
    v35 = 0LL;
    v36 = v34 >= v35;
    if (v36) goto bb5;
    else goto bb6;
  bb5:
    v38 = *(int64_t*)v14;
    v39 = *(int64_t*)v31;
    v40 = *(int64_t*)v8;
    v41 = 0LL;
    vec_sift_down((void*)v41, v38, v39, v40);
    v43 = *(int64_t*)v31;
    v44 = 1LL;
    v45 = p_sub_i64(v43, v44);
    *(int64_t*)v31 = (int64_t)v45;
    goto bb4;
  bb6:
    v48 = *(int64_t*)v8;
    v49 = 1LL;
    v50 = p_sub_i64(v48, v49);
    memset(v51, 0, 8);
    *(int64_t*)v51 = (int64_t)v50;
    goto bb7;
  bb7:
    v54 = *(int64_t*)v51;
    v55 = 0LL;
    v56 = v54 > v55;
    if (v56) goto bb8;
    else goto bb9;
  bb8:
    v58 = *(int64_t*)v14;
    v59 = 0LL;
    v60 = *(int64_t*)v51;
    v61 = 0LL;
    vec_swap_slots((void*)v61, v58, v59, v60);
    v63 = *(int64_t*)v14;
    v64 = 0LL;
    v65 = *(int64_t*)v51;
    v66 = 0LL;
    vec_sift_down((void*)v66, v63, v64, v65);
    v68 = *(int64_t*)v51;
    v69 = 1LL;
    v70 = p_sub_i64(v68, v69);
    *(int64_t*)v51 = (int64_t)v70;
    goto bb7;
  bb9:
    return;
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

void vec_sift_down(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    bool v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    bool v10 = 0;
    int64_t v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0;
    int64_t v16 = 0;
    int64_t v17 = 0;
    void* v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    bool v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    bool v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0;
    bool v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0;
    int64_t v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0;
    bool v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    bool v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    int64_t v47 = 0;
    int64_t v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0;
    bool v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    bool v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0; /* placeholder for void */
    int64_t v63 = 0;
    int64_t v64 = 0;
    int64_t v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0; /* placeholder for void */
    v4 = alloca(8);
    v7 = alloca(1);
    v18 = alloca(8);
    v26 = alloca(1);
    memset(v4, 0, 8);
    *(int64_t*)v4 = (int64_t)v2;
    v6 = false;
    memset(v7, 0, 1);
    *(bool*)v7 = v6;
    goto bb1;
  bb1:
    v10 = *(bool*)v7;
    v11 = !v10;
    if (v11) goto bb2;
    else goto bb3;
  bb2:
    v13 = 2LL;
    v14 = *(int64_t*)v4;
    v15 = p_mul_i64(v13, v14);
    v16 = 1LL;
    v17 = p_add_i64(v15, v16);
    memset(v18, 0, 8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = *(int64_t*)v18;
    v21 = v20 >= v3;
    if (v21) goto bb4;
    else goto bb5;
  bb3:
    return;
  bb4:
    v23 = true;
    *(bool*)v7 = v23;
    goto bb6;
  bb5:
    memset(v26, 0, 1);
    v27 = *(int64_t*)v18;
    v28 = 1LL;
    v29 = p_add_i64(v27, v28);
    v30 = v29 < v3;
    *(bool*)v26 = v30;
    if (v30) goto bb7;
    else goto bb8;
  bb6:
    goto bb1;
  bb7:
    v33 = *(int64_t*)v18;
    v34 = 0LL;
    v35 = (int64_t)vec_slot((void*)v34, v1, v33);
    v36 = *(int64_t*)v18;
    v37 = 1LL;
    v38 = p_add_i64(v36, v37);
    v39 = 0LL;
    v40 = (int64_t)vec_slot((void*)v39, v1, v38);
    v41 = v35 < v40;
    *(bool*)v26 = v41;
    goto bb8;
  bb8:
    v44 = *(bool*)v26;
    if (v44) goto bb9;
    else goto bb10;
  bb9:
    v46 = *(int64_t*)v18;
    v47 = 1LL;
    v48 = p_add_i64(v46, v47);
    *(int64_t*)v18 = (int64_t)v48;
    goto bb11;
  bb10:
    goto bb11;
  bb11:
    v52 = *(int64_t*)v4;
    v53 = 0LL;
    v54 = (int64_t)vec_slot((void*)v53, v1, v52);
    v55 = *(int64_t*)v18;
    v56 = 0LL;
    v57 = (int64_t)vec_slot((void*)v56, v1, v55);
    v58 = v54 >= v57;
    if (v58) goto bb12;
    else goto bb13;
  bb12:
    v60 = true;
    *(bool*)v7 = v60;
    goto bb14;
  bb13:
    v63 = *(int64_t*)v4;
    v64 = *(int64_t*)v18;
    v65 = 0LL;
    vec_swap_slots((void*)v65, v1, v63, v64);
    v67 = *(int64_t*)v18;
    *(int64_t*)v4 = (int64_t)v67;
    goto bb14;
  bb14:
    goto bb6;
    return;
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
    int64_t v16 = 0;
    int64_t v17 = 0;
    bool v18 = 0;
    int64_t v19 = 0; /* placeholder for void */
    int64_t v20 = 0;
    int64_t v21 = 0;
    int64_t v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0;
    int64_t v29 = 0;
    int64_t v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    void* v34 = 0;
    uint8_t v35 = 0;
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
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
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
    v16 = *(int64_t*)v13;
    v17 = 0LL;
    v18 = v16 != v17;
    if (v18) goto bb2;
    else goto bb3;
  bb2:
    v20 = *(int64_t*)v3;
    v21 = *(int64_t*)v13;
    v22 = v20 ^ v21;
    *(int64_t*)v3 = (int64_t)v22;
    v24 = *(int64_t*)v3;
    v25 = 1099511628211LL;
    v26 = (int64_t)((uint64_t)v24 * (uint64_t)v25);
    *(int64_t*)v3 = (int64_t)v26;
    v28 = *(int64_t*)v5;
    v29 = 1LL;
    v30 = (int64_t)((uint64_t)v28 + (uint64_t)v29);
    *(int64_t*)v5 = (int64_t)v30;
    v32 = *(int64_t*)v5;
    *(void**)v8 = (void*)v32;
    v34 = *(void**)v8;
    v35 = *(uint8_t*)v34;
    *(int64_t*)v13 = (int64_t)v35;
    goto bb1;
  bb3:
    v39 = *(int64_t*)v3;
    v40 = 0LL;
    v41 = v39 < v40;
    if (v41) goto bb4;
    else goto bb5;
  bb4:
    v43 = *(int64_t*)v3;
    v44 = 0LL;
    v45 = (int64_t)((uint64_t)v44 - (uint64_t)v43);
    return v45;
  bb5:
    goto bb6;
  bb6:
    v48 = *(int64_t*)v3;
    return v48;
    return 0;
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
    void* v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0;
    bool v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    int64_t v47 = 0;
    bool v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    bool v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    int64_t v54 = 0;
    bool v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
    bool v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    int64_t v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0;
    int64_t v65 = 0;
    int64_t v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    void* v72 = 0;
    uint8_t v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    void* v75 = 0;
    uint8_t v76 = 0;
    int64_t v77 = 0; /* placeholder for void */
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0;
    int64_t v80 = 0;
    bool v81 = 0;
    int64_t v82 = 0; /* placeholder for void */
    v20 = alloca(8);
    v22 = alloca(8);
    v25 = alloca(8);
    v28 = alloca(8);
    v33 = alloca(8);
    v37 = alloca(8);
    v40 = alloca(1);
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
    memset(v40, 0, 1);
    v41 = *(int64_t*)v33;
    v42 = 0LL;
    v43 = v41 != v42;
    *(bool*)v40 = v43;
    if (v43) goto bb13;
    else goto bb14;
  bb11:
    v53 = *(int64_t*)v33;
    v54 = *(int64_t*)v37;
    v55 = v53 != v54;
    if (v55) goto bb15;
    else goto bb16;
  bb12:
    v79 = *(int64_t*)v33;
    v80 = *(int64_t*)v37;
    v81 = v79 == v80;
    return v81;
  bb13:
    v46 = *(int64_t*)v37;
    v47 = 0LL;
    v48 = v46 != v47;
    *(bool*)v40 = v48;
    goto bb14;
  bb14:
    v51 = *(bool*)v40;
    if (v51) goto bb11;
    else goto bb12;
  bb15:
    v57 = false;
    return v57;
  bb16:
    goto bb17;
  bb17:
    v60 = *(int64_t*)v20;
    v61 = 1LL;
    v62 = p_add_i64(v60, v61);
    *(int64_t*)v20 = (int64_t)v62;
    v64 = *(int64_t*)v22;
    v65 = 1LL;
    v66 = p_add_i64(v64, v65);
    *(int64_t*)v22 = (int64_t)v66;
    v68 = *(int64_t*)v20;
    *(void**)v25 = (void*)v68;
    v70 = *(int64_t*)v22;
    *(void**)v28 = (void*)v70;
    v72 = *(void**)v25;
    v73 = *(uint8_t*)v72;
    *(int64_t*)v33 = (int64_t)v73;
    v75 = *(void**)v28;
    v76 = *(uint8_t*)v75;
    *(int64_t*)v37 = (int64_t)v76;
    goto bb10;
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
    void* v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0;
    bool v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    int64_t v47 = 0;
    bool v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    bool v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    int64_t v54 = 0;
    bool v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0;
    int64_t v58 = 0;
    int64_t v59 = 0;
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0;
    int64_t v67 = 0;
    int64_t v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    void* v74 = 0;
    uint8_t v75 = 0;
    int64_t v76 = 0; /* placeholder for void */
    void* v77 = 0;
    uint8_t v78 = 0;
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0;
    int64_t v82 = 0;
    int64_t v83 = 0;
    int64_t v84 = 0; /* placeholder for void */
    v20 = alloca(8);
    v22 = alloca(8);
    v25 = alloca(8);
    v28 = alloca(8);
    v33 = alloca(8);
    v37 = alloca(8);
    v40 = alloca(1);
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
    memset(v40, 0, 1);
    v41 = *(int64_t*)v33;
    v42 = 0LL;
    v43 = v41 != v42;
    *(bool*)v40 = v43;
    if (v43) goto bb13;
    else goto bb14;
  bb11:
    v53 = *(int64_t*)v33;
    v54 = *(int64_t*)v37;
    v55 = v53 != v54;
    if (v55) goto bb15;
    else goto bb16;
  bb12:
    v81 = *(int64_t*)v33;
    v82 = *(int64_t*)v37;
    v83 = p_sub_i64(v81, v82);
    return v83;
  bb13:
    v46 = *(int64_t*)v37;
    v47 = 0LL;
    v48 = v46 != v47;
    *(bool*)v40 = v48;
    goto bb14;
  bb14:
    v51 = *(bool*)v40;
    if (v51) goto bb11;
    else goto bb12;
  bb15:
    v57 = *(int64_t*)v33;
    v58 = *(int64_t*)v37;
    v59 = p_sub_i64(v57, v58);
    return v59;
  bb16:
    goto bb17;
  bb17:
    v62 = *(int64_t*)v20;
    v63 = 1LL;
    v64 = p_add_i64(v62, v63);
    *(int64_t*)v20 = (int64_t)v64;
    v66 = *(int64_t*)v22;
    v67 = 1LL;
    v68 = p_add_i64(v66, v67);
    *(int64_t*)v22 = (int64_t)v68;
    v70 = *(int64_t*)v20;
    *(void**)v25 = (void*)v70;
    v72 = *(int64_t*)v22;
    *(void**)v28 = (void*)v72;
    v74 = *(void**)v25;
    v75 = *(uint8_t*)v74;
    *(int64_t*)v33 = (int64_t)v75;
    v77 = *(void**)v28;
    v78 = *(uint8_t*)v77;
    *(int64_t*)v37 = (int64_t)v78;
    goto bb10;
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
    int64_t v8 = 0;
    int64_t v9 = 0;
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0;
    int64_t v19 = 0;
    int64_t v20 = 0;
    void* v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0; /* placeholder for void */
    void* v24 = 0;
    int64_t v25 = 0;
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
    int64_t v38 = 0; /* placeholder for void */
    v3 = alloca(8);
    v6 = alloca(8);
    v11 = alloca(8);
    v16 = alloca(8);
    v21 = alloca(8);
    v1 = 32LL;
    v2 = p_malloc(v1);
    memset(v3, 0, 8);
    *(int64_t*)v3 = (int64_t)v2;
    v5 = *(int64_t*)v3;
    memset(v6, 0, 8);
    *(void**)v6 = (void*)v5;
    v8 = *(int64_t*)v3;
    v9 = 8LL;
    v10 = p_add_i64(v8, v9);
    memset(v11, 0, 8);
    *(void**)v11 = (void*)v10;
    v13 = *(int64_t*)v3;
    v14 = 16LL;
    v15 = p_add_i64(v13, v14);
    memset(v16, 0, 8);
    *(void**)v16 = (void*)v15;
    v18 = *(int64_t*)v3;
    v19 = 24LL;
    v20 = p_add_i64(v18, v19);
    memset(v21, 0, 8);
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
    return v37;
    return 0;
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
    int64_t v48 = 0;
    int64_t v49 = 0;
    bool v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    void* v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
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
    bool v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    int64_t v74 = 0;
    int64_t v75 = 0;
    bool v76 = 0;
    int64_t v77 = 0; /* placeholder for void */
    int64_t v78 = 0;
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0;
    int64_t v82 = 0; /* placeholder for void */
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0;
    int64_t v85 = 0;
    bool v86 = 0;
    int64_t v87 = 0; /* placeholder for void */
    int64_t v88 = 0;
    int64_t v89 = 0;
    bool v90 = 0;
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0;
    int64_t v93 = 0; /* placeholder for void */
    int64_t v94 = 0; /* placeholder for void */
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0;
    int64_t v98 = 0;
    bool v99 = 0;
    int64_t v100 = 0; /* placeholder for void */
    int64_t v101 = 0;
    int64_t v102 = 0; /* placeholder for void */
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0; /* placeholder for void */
    int64_t v105 = 0;
    int64_t v106 = 0;
    int64_t v107 = 0;
    int64_t v108 = 0; /* placeholder for void */
    int64_t v109 = 0;
    int64_t v110 = 0;
    bool v111 = 0;
    int64_t v112 = 0; /* placeholder for void */
    int64_t v113 = 0;
    int64_t v114 = 0; /* placeholder for void */
    int64_t v115 = 0; /* placeholder for void */
    int64_t v116 = 0; /* placeholder for void */
    int64_t v117 = 0;
    int64_t v118 = 0;
    int64_t v119 = 0;
    int64_t v120 = 0; /* placeholder for void */
    int64_t v121 = 0; /* placeholder for void */
    int64_t v122 = 0;
    int64_t v123 = 0;
    bool v124 = 0;
    int64_t v125 = 0; /* placeholder for void */
    int64_t v126 = 0;
    int64_t v127 = 0; /* placeholder for void */
    int64_t v128 = 0; /* placeholder for void */
    int64_t v129 = 0;
    int64_t v130 = 0; /* placeholder for void */
    v5 = alloca(8);
    v7 = alloca(8);
    v10 = alloca(8);
    v25 = alloca(8);
    v34 = alloca(8);
    v39 = alloca(8);
    v42 = alloca(8);
    v45 = alloca(8);
    v57 = alloca(8);
    v60 = alloca(8);
    v63 = alloca(8);
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
    v33 = (int64_t)p_str_hash((void*)v32, v2);
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
    v48 = *(int64_t*)v45;
    v49 = *(int64_t*)v10;
    v50 = v48 < v49;
    if (v50) goto bb5;
    else goto bb6;
  bb5:
    v52 = *(int64_t*)v25;
    v53 = *(int64_t*)v39;
    v54 = 8LL;
    v55 = p_mul_i64(v53, v54);
    v56 = p_add_i64(v52, v55);
    memset(v57, 0, 8);
    *(int64_t*)v57 = (int64_t)v56;
    v59 = *(int64_t*)v57;
    memset(v60, 0, 8);
    *(void**)v60 = (void*)v59;
    v62 = 0LL;
    memset(v63, 0, 8);
    *(int64_t*)v63 = (int64_t)v62;
    v66 = *(void**)v60;
    v67 = *(int64_t*)v66;
    *(int64_t*)v63 = (int64_t)v67;
    v70 = *(int64_t*)v63;
    v71 = 0LL;
    v72 = v70 == v71;
    if (v72) goto bb7;
    else goto bb8;
  bb6:
    v122 = *(int64_t*)v42;
    v123 = 0LL;
    v124 = v122 >= v123;
    if (v124) goto bb25;
    else goto bb26;
  bb7:
    v74 = *(int64_t*)v42;
    v75 = 0LL;
    v76 = v74 >= v75;
    if (v76) goto bb10;
    else goto bb11;
  bb8:
    goto bb9;
  bb9:
    v84 = *(int64_t*)v63;
    v85 = -1LL;
    v86 = v84 == v85;
    if (v86) goto bb13;
    else goto bb14;
  bb10:
    v78 = *(int64_t*)v42;
    return v78;
  bb11:
    goto bb12;
  bb12:
    v81 = *(int64_t*)v39;
    return v81;
  bb13:
    v88 = *(int64_t*)v42;
    v89 = 0LL;
    v90 = v88 < v89;
    if (v90) goto bb16;
    else goto bb17;
  bb14:
    v97 = *(int64_t*)v63;
    v98 = 0LL;
    v99 = (bool)p_str_equal((void*)v98, v97, v2);
    if (v99) goto bb19;
    else goto bb20;
  bb15:
    v105 = *(int64_t*)v39;
    v106 = 1LL;
    v107 = p_add_i64(v105, v106);
    *(int64_t*)v39 = (int64_t)v107;
    v109 = *(int64_t*)v39;
    v110 = *(int64_t*)v10;
    v111 = v109 >= v110;
    if (v111) goto bb22;
    else goto bb23;
  bb16:
    v92 = *(int64_t*)v39;
    *(int64_t*)v42 = (int64_t)v92;
    goto bb18;
  bb17:
    goto bb18;
  bb18:
    goto bb15;
  bb19:
    v101 = *(int64_t*)v39;
    return v101;
  bb20:
    goto bb21;
  bb21:
    goto bb15;
  bb22:
    v113 = 0LL;
    *(int64_t*)v39 = (int64_t)v113;
    goto bb24;
  bb23:
    goto bb24;
  bb24:
    v117 = *(int64_t*)v45;
    v118 = 1LL;
    v119 = (int64_t)((uint64_t)v117 + (uint64_t)v118);
    *(int64_t*)v45 = (int64_t)v119;
    goto bb4;
  bb25:
    v126 = *(int64_t*)v42;
    return v126;
  bb26:
    goto bb27;
  bb27:
    v129 = 0LL;
    return v129;
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
    int64_t v65 = 0;
    int64_t v66 = 0;
    bool v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0;
    int64_t v70 = 0;
    int64_t v71 = 0;
    int64_t v72 = 0;
    int64_t v73 = 0;
    void* v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0;
    void* v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0;
    int64_t v80 = 0;
    int64_t v81 = 0;
    int64_t v82 = 0;
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
    void* v93 = 0;
    int64_t v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0;
    int64_t v98 = 0;
    int64_t v99 = 0;
    int64_t v100 = 0; /* placeholder for void */
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0;
    void* v103 = 0;
    int64_t v104 = 0; /* placeholder for void */
    int64_t v105 = 0;
    void* v106 = 0;
    int64_t v107 = 0; /* placeholder for void */
    int64_t v108 = 0; /* placeholder for void */
    int64_t v109 = 0;
    int64_t v110 = 0;
    bool v111 = 0;
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
    int64_t v123 = 0;
    void* v124 = 0;
    int64_t v125 = 0; /* placeholder for void */
    int64_t v126 = 0; /* placeholder for void */
    void* v127 = 0;
    int64_t v128 = 0;
    int64_t v129 = 0; /* placeholder for void */
    int64_t v130 = 0; /* placeholder for void */
    void* v131 = 0;
    int64_t v132 = 0;
    int64_t v133 = 0;
    bool v134 = 0;
    int64_t v135 = 0; /* placeholder for void */
    int64_t v136 = 0; /* placeholder for void */
    int64_t v137 = 0;
    int64_t v138 = 0;
    bool v139 = 0;
    int64_t v140 = 0; /* placeholder for void */
    int64_t v141 = 0; /* placeholder for void */
    bool v142 = 0;
    int64_t v143 = 0; /* placeholder for void */
    int64_t v144 = 0;
    int64_t v145 = 0;
    int64_t v146 = 0;
    int64_t v147 = 0;
    int64_t v148 = 0;
    void* v149 = 0;
    int64_t v150 = 0; /* placeholder for void */
    int64_t v151 = 0;
    void* v152 = 0;
    int64_t v153 = 0; /* placeholder for void */
    int64_t v154 = 0;
    void* v155 = 0;
    int64_t v156 = 0; /* placeholder for void */
    int64_t v157 = 0; /* placeholder for void */
    void* v158 = 0;
    int64_t v159 = 0;
    int64_t v160 = 0; /* placeholder for void */
    int64_t v161 = 0; /* placeholder for void */
    int64_t v162 = 0;
    int64_t v163 = 0;
    int64_t v164 = 0;
    void* v165 = 0;
    int64_t v166 = 0; /* placeholder for void */
    int64_t v167 = 0;
    int64_t v168 = 0;
    int64_t v169 = 0;
    void* v170 = 0;
    int64_t v171 = 0; /* placeholder for void */
    int64_t v172 = 0;
    void* v173 = 0;
    int64_t v174 = 0; /* placeholder for void */
    int64_t v175 = 0; /* placeholder for void */
    int64_t v176 = 0;
    int64_t v177 = 0;
    bool v178 = 0;
    int64_t v179 = 0; /* placeholder for void */
    int64_t v180 = 0;
    int64_t v181 = 0;
    int64_t v182 = 0;
    int64_t v183 = 0;
    int64_t v184 = 0;
    void* v185 = 0;
    int64_t v186 = 0; /* placeholder for void */
    int64_t v187 = 0;
    void* v188 = 0;
    int64_t v189 = 0; /* placeholder for void */
    int64_t v190 = 0;
    void* v191 = 0;
    int64_t v192 = 0; /* placeholder for void */
    int64_t v193 = 0; /* placeholder for void */
    void* v194 = 0;
    int64_t v195 = 0;
    int64_t v196 = 0; /* placeholder for void */
    int64_t v197 = 0; /* placeholder for void */
    int64_t v198 = 0;
    int64_t v199 = 0;
    bool v200 = 0;
    int64_t v201 = 0; /* placeholder for void */
    int64_t v202 = 0; /* placeholder for void */
    void* v203 = 0;
    int64_t v204 = 0;
    int64_t v205 = 0; /* placeholder for void */
    int64_t v206 = 0; /* placeholder for void */
    int64_t v207 = 0;
    int64_t v208 = 0;
    int64_t v209 = 0;
    int64_t v210 = 0;
    int64_t v211 = 0;
    void* v212 = 0;
    int64_t v213 = 0; /* placeholder for void */
    int64_t v214 = 0;
    void* v215 = 0;
    int64_t v216 = 0; /* placeholder for void */
    int64_t v217 = 0; /* placeholder for void */
    void* v218 = 0;
    int64_t v219 = 0;
    int64_t v220 = 0; /* placeholder for void */
    int64_t v221 = 0; /* placeholder for void */
    int64_t v222 = 0; /* placeholder for void */
    int64_t v223 = 0; /* placeholder for void */
    int64_t v224 = 0;
    int64_t v225 = 0;
    int64_t v226 = 0;
    int64_t v227 = 0; /* placeholder for void */
    int64_t v228 = 0;
    int64_t v229 = 0;
    bool v230 = 0;
    int64_t v231 = 0; /* placeholder for void */
    int64_t v232 = 0;
    int64_t v233 = 0; /* placeholder for void */
    int64_t v234 = 0; /* placeholder for void */
    int64_t v235 = 0; /* placeholder for void */
    int64_t v236 = 0;
    int64_t v237 = 0;
    int64_t v238 = 0;
    int64_t v239 = 0; /* placeholder for void */
    int64_t v240 = 0; /* placeholder for void */
    int64_t v241 = 0;
    int64_t v242 = 0;
    int64_t v243 = 0;
    int64_t v244 = 0; /* placeholder for void */
    int64_t v245 = 0; /* placeholder for void */
    int64_t v246 = 0; /* placeholder for void */
    int64_t v247 = 0;
    int64_t v248 = 0;
    int64_t v249 = 0;
    int64_t v250 = 0; /* placeholder for void */
    int64_t v251 = 0; /* placeholder for void */
    int64_t v252 = 0;
    int64_t v253 = 0;
    bool v254 = 0;
    int64_t v255 = 0; /* placeholder for void */
    int64_t v256 = 0;
    int64_t v257 = 0; /* placeholder for void */
    int64_t v258 = 0; /* placeholder for void */
    int64_t v259 = 0; /* placeholder for void */
    int64_t v260 = 0;
    int64_t v261 = 0;
    bool v262 = 0;
    int64_t v263 = 0; /* placeholder for void */
    int64_t v264 = 0;
    int64_t v265 = 0; /* placeholder for void */
    int64_t v266 = 0; /* placeholder for void */
    int64_t v267 = 0; /* placeholder for void */
    int64_t v268 = 0; /* placeholder for void */
    void* v269 = 0;
    int64_t v270 = 0;
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
    int64_t v281 = 0; /* placeholder for void */
    int64_t v282 = 0; /* placeholder for void */
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
    v74 = alloca(8);
    v77 = alloca(8);
    v84 = alloca(8);
    v87 = alloca(8);
    v103 = alloca(8);
    v106 = alloca(8);
    v118 = alloca(8);
    v121 = alloca(8);
    v124 = alloca(8);
    v131 = alloca(1);
    v149 = alloca(8);
    v152 = alloca(8);
    v155 = alloca(8);
    v165 = alloca(8);
    v170 = alloca(8);
    v173 = alloca(8);
    v185 = alloca(8);
    v188 = alloca(8);
    v191 = alloca(8);
    v212 = alloca(8);
    v215 = alloca(8);
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
    v65 = *(int64_t*)v62;
    v66 = *(int64_t*)v37;
    v67 = v65 < v66;
    if (v67) goto bb5;
    else goto bb6;
  bb5:
    v69 = *(int64_t*)v53;
    v70 = *(int64_t*)v62;
    v71 = 8LL;
    v72 = (int64_t)((uint64_t)v70 * (uint64_t)v71);
    v73 = (int64_t)((uint64_t)v69 + (uint64_t)v72);
    memset(v74, 0, 8);
    *(int64_t*)v74 = (int64_t)v73;
    v76 = *(int64_t*)v74;
    memset(v77, 0, 8);
    *(void**)v77 = (void*)v76;
    v79 = *(int64_t*)v59;
    v80 = *(int64_t*)v62;
    v81 = 8LL;
    v82 = (int64_t)((uint64_t)v80 * (uint64_t)v81);
    v83 = (int64_t)((uint64_t)v79 + (uint64_t)v82);
    memset(v84, 0, 8);
    *(int64_t*)v84 = (int64_t)v83;
    v86 = *(int64_t*)v84;
    memset(v87, 0, 8);
    *(void**)v87 = (void*)v86;
    v90 = *(void**)v77;
    v91 = 0LL;
    *(int64_t*)v90 = (int64_t)v91;
    v93 = *(void**)v87;
    v94 = 0LL;
    *(int64_t*)v93 = (int64_t)v94;
    v97 = *(int64_t*)v62;
    v98 = 1LL;
    v99 = (int64_t)((uint64_t)v97 + (uint64_t)v98);
    *(int64_t*)v62 = (int64_t)v99;
    goto bb4;
  bb6:
    v102 = 0LL;
    memset(v103, 0, 8);
    *(int64_t*)v103 = (int64_t)v102;
    v105 = 0LL;
    memset(v106, 0, 8);
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
    v116 = (int64_t)((uint64_t)v114 * (uint64_t)v115);
    v117 = (int64_t)((uint64_t)v113 + (uint64_t)v116);
    memset(v118, 0, 8);
    *(int64_t*)v118 = (int64_t)v117;
    v120 = *(int64_t*)v118;
    memset(v121, 0, 8);
    *(void**)v121 = (void*)v120;
    v123 = 0LL;
    memset(v124, 0, 8);
    *(int64_t*)v124 = (int64_t)v123;
    v127 = *(void**)v121;
    v128 = *(int64_t*)v127;
    *(int64_t*)v124 = (int64_t)v128;
    memset(v131, 0, 1);
    v132 = *(int64_t*)v124;
    v133 = 0LL;
    v134 = v132 != v133;
    *(bool*)v131 = v134;
    if (v134) goto bb10;
    else goto bb11;
  bb9:
    v252 = *(int64_t*)v20;
    v253 = 0LL;
    v254 = v252 != v253;
    if (v254) goto bb24;
    else goto bb25;
  bb10:
    v137 = *(int64_t*)v124;
    v138 = -1LL;
    v139 = v137 != v138;
    *(bool*)v131 = v139;
    goto bb11;
  bb11:
    v142 = *(bool*)v131;
    if (v142) goto bb12;
    else goto bb13;
  bb12:
    v144 = *(int64_t*)v23;
    v145 = *(int64_t*)v106;
    v146 = 8LL;
    v147 = (int64_t)((uint64_t)v145 * (uint64_t)v146);
    v148 = (int64_t)((uint64_t)v144 + (uint64_t)v147);
    memset(v149, 0, 8);
    *(int64_t*)v149 = (int64_t)v148;
    v151 = *(int64_t*)v149;
    memset(v152, 0, 8);
    *(void**)v152 = (void*)v151;
    v154 = 0LL;
    memset(v155, 0, 8);
    *(int64_t*)v155 = (int64_t)v154;
    v158 = *(void**)v152;
    v159 = *(int64_t*)v158;
    *(int64_t*)v155 = (int64_t)v159;
    v162 = *(int64_t*)v124;
    v163 = 0LL;
    v164 = (int64_t)p_str_hash((void*)v163, v162);
    memset(v165, 0, 8);
    *(int64_t*)v165 = (int64_t)v164;
    v167 = *(int64_t*)v165;
    v168 = *(int64_t*)v37;
    v169 = p_mod_i64(v167, v168);
    memset(v170, 0, 8);
    *(int64_t*)v170 = (int64_t)v169;
    v172 = 0LL;
    memset(v173, 0, 8);
    *(int64_t*)v173 = (int64_t)v172;
    goto bb15;
  bb13:
    goto bb14;
  bb14:
    v247 = *(int64_t*)v106;
    v248 = 1LL;
    v249 = (int64_t)((uint64_t)v247 + (uint64_t)v248);
    *(int64_t*)v106 = (int64_t)v249;
    goto bb7;
  bb15:
    v176 = *(int64_t*)v173;
    v177 = *(int64_t*)v37;
    v178 = v176 < v177;
    if (v178) goto bb16;
    else goto bb17;
  bb16:
    v180 = *(int64_t*)v53;
    v181 = *(int64_t*)v170;
    v182 = 8LL;
    v183 = (int64_t)((uint64_t)v181 * (uint64_t)v182);
    v184 = (int64_t)((uint64_t)v180 + (uint64_t)v183);
    memset(v185, 0, 8);
    *(int64_t*)v185 = (int64_t)v184;
    v187 = *(int64_t*)v185;
    memset(v188, 0, 8);
    *(void**)v188 = (void*)v187;
    v190 = 0LL;
    memset(v191, 0, 8);
    *(int64_t*)v191 = (int64_t)v190;
    v194 = *(void**)v188;
    v195 = *(int64_t*)v194;
    *(int64_t*)v191 = (int64_t)v195;
    v198 = *(int64_t*)v191;
    v199 = 0LL;
    v200 = v198 == v199;
    if (v200) goto bb18;
    else goto bb19;
  bb17:
    v241 = *(int64_t*)v103;
    v242 = 1LL;
    v243 = (int64_t)((uint64_t)v241 + (uint64_t)v242);
    *(int64_t*)v103 = (int64_t)v243;
    goto bb14;
  bb18:
    v203 = *(void**)v188;
    v204 = *(int64_t*)v124;
    *(int64_t*)v203 = (int64_t)v204;
    v207 = *(int64_t*)v59;
    v208 = *(int64_t*)v170;
    v209 = 8LL;
    v210 = (int64_t)((uint64_t)v208 * (uint64_t)v209);
    v211 = (int64_t)((uint64_t)v207 + (uint64_t)v210);
    memset(v212, 0, 8);
    *(int64_t*)v212 = (int64_t)v211;
    v214 = *(int64_t*)v212;
    memset(v215, 0, 8);
    *(void**)v215 = (void*)v214;
    v218 = *(void**)v215;
    v219 = *(int64_t*)v155;
    *(int64_t*)v218 = (int64_t)v219;
    goto bb17;
  bb19:
    goto bb20;
  bb20:
    v224 = *(int64_t*)v170;
    v225 = 1LL;
    v226 = (int64_t)((uint64_t)v224 + (uint64_t)v225);
    *(int64_t*)v170 = (int64_t)v226;
    v228 = *(int64_t*)v170;
    v229 = *(int64_t*)v37;
    v230 = v228 >= v229;
    if (v230) goto bb21;
    else goto bb22;
  bb21:
    v232 = 0LL;
    *(int64_t*)v170 = (int64_t)v232;
    goto bb23;
  bb22:
    goto bb23;
  bb23:
    v236 = *(int64_t*)v173;
    v237 = 1LL;
    v238 = (int64_t)((uint64_t)v236 + (uint64_t)v237);
    *(int64_t*)v173 = (int64_t)v238;
    goto bb15;
  bb24:
    v256 = *(int64_t*)v20;
    p_free((void*)v256);
    goto bb26;
  bb25:
    goto bb26;
  bb26:
    v260 = *(int64_t*)v23;
    v261 = 0LL;
    v262 = v260 != v261;
    if (v262) goto bb27;
    else goto bb28;
  bb27:
    v264 = *(int64_t*)v23;
    p_free((void*)v264);
    goto bb29;
  bb28:
    goto bb29;
  bb29:
    v269 = *(void**)v6;
    v270 = *(int64_t*)v53;
    *(int64_t*)v269 = (int64_t)v270;
    v272 = *(void**)v10;
    v273 = *(int64_t*)v59;
    *(int64_t*)v272 = (int64_t)v273;
    v275 = *(void**)v14;
    v276 = *(int64_t*)v103;
    *(int64_t*)v275 = (int64_t)v276;
    v278 = *(void**)v4;
    v279 = *(int64_t*)v37;
    *(int64_t*)v278 = (int64_t)v279;
    return;
    return;
}

void HashMap_insert(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    int64_t v4 = 0;
    int64_t v5 = 0;
    void* v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    int64_t v9 = 0;
    void* v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0;
    void* v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0; /* placeholder for void */
    void* v19 = 0;
    int64_t v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    bool v29 = 0;
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
    int64_t v40 = 0; /* placeholder for void */
    bool v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    void* v46 = 0;
    int64_t v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    int64_t v53 = 0;
    void* v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    void* v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    int64_t v59 = 0;
    void* v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0;
    void* v63 = 0;
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0;
    void* v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0; /* placeholder for void */
    void* v69 = 0;
    int64_t v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    void* v72 = 0;
    int64_t v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
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
    int64_t v107 = 0; /* placeholder for void */
    void* v108 = 0;
    int64_t v109 = 0; /* placeholder for void */
    int64_t v110 = 0; /* placeholder for void */
    int64_t v111 = 0;
    int64_t v112 = 0;
    int64_t v113 = 0;
    int64_t v114 = 0;
    int64_t v115 = 0;
    void* v116 = 0;
    int64_t v117 = 0; /* placeholder for void */
    int64_t v118 = 0;
    void* v119 = 0;
    int64_t v120 = 0; /* placeholder for void */
    int64_t v121 = 0; /* placeholder for void */
    void* v122 = 0;
    int64_t v123 = 0; /* placeholder for void */
    int64_t v124 = 0; /* placeholder for void */
    int64_t v125 = 0;
    int64_t v126 = 0;
    int64_t v127 = 0;
    void* v128 = 0;
    int64_t v129 = 0; /* placeholder for void */
    int64_t v130 = 0; /* placeholder for void */
    void* v131 = 0;
    int64_t v132 = 0;
    int64_t v133 = 0; /* placeholder for void */
    int64_t v134 = 0; /* placeholder for void */
    int64_t v135 = 0; /* placeholder for void */
    int64_t v136 = 0;
    int64_t v137 = 0;
    int64_t v138 = 0;
    int64_t v139 = 0;
    int64_t v140 = 0;
    void* v141 = 0;
    int64_t v142 = 0; /* placeholder for void */
    int64_t v143 = 0;
    void* v144 = 0;
    int64_t v145 = 0; /* placeholder for void */
    int64_t v146 = 0; /* placeholder for void */
    void* v147 = 0;
    int64_t v148 = 0; /* placeholder for void */
    int64_t v149 = 0; /* placeholder for void */
    int64_t v150 = 0; /* placeholder for void */
    int64_t v151 = 0; /* placeholder for void */
    v6 = alloca(8);
    v10 = alloca(8);
    v13 = alloca(8);
    v16 = alloca(8);
    v26 = alloca(1);
    v54 = alloca(8);
    v56 = alloca(8);
    v60 = alloca(8);
    v63 = alloca(8);
    v66 = alloca(8);
    v81 = alloca(8);
    v84 = alloca(8);
    v87 = alloca(8);
    v94 = alloca(1);
    v116 = alloca(8);
    v119 = alloca(8);
    v128 = alloca(8);
    v141 = alloca(8);
    v144 = alloca(8);
    v4 = 16LL;
    v5 = p_add_i64(v1, v4);
    memset(v6, 0, 8);
    *(void**)v6 = (void*)v5;
    v8 = 24LL;
    v9 = p_add_i64(v1, v8);
    memset(v10, 0, 8);
    *(void**)v10 = (void*)v9;
    v12 = 0LL;
    memset(v13, 0, 8);
    *(int64_t*)v13 = (int64_t)v12;
    v15 = 0LL;
    memset(v16, 0, 8);
    *(int64_t*)v16 = (int64_t)v15;
    v19 = *(void**)v6;
    v20 = *(int64_t*)v19;
    *(int64_t*)v13 = (int64_t)v20;
    v22 = *(void**)v10;
    v23 = *(int64_t*)v22;
    *(int64_t*)v16 = (int64_t)v23;
    memset(v26, 0, 1);
    v27 = *(int64_t*)v16;
    v28 = 0LL;
    v29 = v27 == v28;
    *(bool*)v26 = v29;
    if (v29) goto bb2;
    else goto bb1;
  bb1:
    v32 = *(int64_t*)v13;
    v33 = 10LL;
    v34 = p_mul_i64(v32, v33);
    v35 = *(int64_t*)v16;
    v36 = 7LL;
    v37 = p_mul_i64(v35, v36);
    v38 = v34 >= v37;
    *(bool*)v26 = v38;
    goto bb2;
  bb2:
    v41 = *(bool*)v26;
    if (v41) goto bb3;
    else goto bb4;
  bb3:
    v43 = 0LL;
    HashMap_grow((void*)v43, v1);
    v46 = *(void**)v10;
    v47 = *(int64_t*)v46;
    *(int64_t*)v16 = (int64_t)v47;
    goto bb5;
  bb4:
    goto bb5;
  bb5:
    v52 = 0LL;
    v53 = (int64_t)HashMap_find_slot((void*)v52, v1, v2);
    memset(v54, 0, 8);
    *(int64_t*)v54 = (int64_t)v53;
    memset(v56, 0, 8);
    *(void**)v56 = (void*)v1;
    v58 = 8LL;
    v59 = p_add_i64(v1, v58);
    memset(v60, 0, 8);
    *(void**)v60 = (void*)v59;
    v62 = 0LL;
    memset(v63, 0, 8);
    *(int64_t*)v63 = (int64_t)v62;
    v65 = 0LL;
    memset(v66, 0, 8);
    *(int64_t*)v66 = (int64_t)v65;
    v69 = *(void**)v56;
    v70 = *(int64_t*)v69;
    *(int64_t*)v63 = (int64_t)v70;
    v72 = *(void**)v60;
    v73 = *(int64_t*)v72;
    *(int64_t*)v66 = (int64_t)v73;
    v76 = *(int64_t*)v63;
    v77 = *(int64_t*)v54;
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
    v97 = v95 == v96;
    *(bool*)v94 = v97;
    if (v97) goto bb7;
    else goto bb6;
  bb6:
    v100 = *(int64_t*)v87;
    v101 = -1LL;
    v102 = v100 == v101;
    *(bool*)v94 = v102;
    goto bb7;
  bb7:
    v105 = *(bool*)v94;
    if (v105) goto bb8;
    else goto bb9;
  bb8:
    v108 = *(void**)v84;
    *(int64_t*)v108 = (int64_t)v2;
    v111 = *(int64_t*)v66;
    v112 = *(int64_t*)v54;
    v113 = 8LL;
    v114 = p_mul_i64(v112, v113);
    v115 = p_add_i64(v111, v114);
    memset(v116, 0, 8);
    *(int64_t*)v116 = (int64_t)v115;
    v118 = *(int64_t*)v116;
    memset(v119, 0, 8);
    *(void**)v119 = (void*)v118;
    v122 = *(void**)v119;
    *(int64_t*)v122 = (int64_t)v3;
    v125 = *(int64_t*)v13;
    v126 = 1LL;
    v127 = p_add_i64(v125, v126);
    memset(v128, 0, 8);
    *(int64_t*)v128 = (int64_t)v127;
    v131 = *(void**)v6;
    v132 = *(int64_t*)v128;
    *(int64_t*)v131 = (int64_t)v132;
    goto bb10;
  bb9:
    v136 = *(int64_t*)v66;
    v137 = *(int64_t*)v54;
    v138 = 8LL;
    v139 = p_mul_i64(v137, v138);
    v140 = p_add_i64(v136, v139);
    memset(v141, 0, 8);
    *(int64_t*)v141 = (int64_t)v140;
    v143 = *(int64_t*)v141;
    memset(v144, 0, 8);
    *(void**)v144 = (void*)v143;
    v147 = *(void**)v144;
    *(int64_t*)v147 = (int64_t)v3;
    goto bb10;
  bb10:
    return;
    return;
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

void HashMap_remove(void* v0, int64_t v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    void* v5 = 0;
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
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0;
    int64_t v26 = 0;
    void* v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
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
    int64_t v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0;
    void* v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    void* v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    void* v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0; /* placeholder for void */
    void* v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    void* v57 = 0;
    int64_t v58 = 0;
    int64_t v59 = 0;
    bool v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0; /* placeholder for void */
    int64_t v63 = 0;
    int64_t v64 = 0;
    bool v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0; /* placeholder for void */
    bool v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0; /* placeholder for void */
    void* v73 = 0;
    int64_t v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0;
    void* v78 = 0;
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0; /* placeholder for void */
    void* v81 = 0;
    int64_t v82 = 0;
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0; /* placeholder for void */
    int64_t v85 = 0;
    int64_t v86 = 0;
    int64_t v87 = 0;
    void* v88 = 0;
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0; /* placeholder for void */
    void* v91 = 0;
    int64_t v92 = 0;
    int64_t v93 = 0; /* placeholder for void */
    int64_t v94 = 0; /* placeholder for void */
    int64_t v95 = 0; /* placeholder for void */
    v5 = alloca(8);
    v9 = alloca(8);
    v12 = alloca(8);
    v27 = alloca(8);
    v29 = alloca(8);
    v32 = alloca(8);
    v44 = alloca(8);
    v47 = alloca(8);
    v50 = alloca(8);
    v57 = alloca(1);
    v78 = alloca(8);
    v88 = alloca(8);
    v3 = 24LL;
    v4 = p_add_i64(v1, v3);
    memset(v5, 0, 8);
    *(void**)v5 = (void*)v4;
    v7 = 16LL;
    v8 = p_add_i64(v1, v7);
    memset(v9, 0, 8);
    *(void**)v9 = (void*)v8;
    v11 = 0LL;
    memset(v12, 0, 8);
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
    return;
  bb2:
    goto bb3;
  bb3:
    v25 = 0LL;
    v26 = (int64_t)HashMap_find_slot((void*)v25, v1, v2);
    memset(v27, 0, 8);
    *(int64_t*)v27 = (int64_t)v26;
    memset(v29, 0, 8);
    *(void**)v29 = (void*)v1;
    v31 = 0LL;
    memset(v32, 0, 8);
    *(int64_t*)v32 = (int64_t)v31;
    v35 = *(void**)v29;
    v36 = *(int64_t*)v35;
    *(int64_t*)v32 = (int64_t)v36;
    v39 = *(int64_t*)v32;
    v40 = *(int64_t*)v27;
    v41 = 8LL;
    v42 = p_mul_i64(v40, v41);
    v43 = p_add_i64(v39, v42);
    memset(v44, 0, 8);
    *(int64_t*)v44 = (int64_t)v43;
    v46 = *(int64_t*)v44;
    memset(v47, 0, 8);
    *(void**)v47 = (void*)v46;
    v49 = 0LL;
    memset(v50, 0, 8);
    *(int64_t*)v50 = (int64_t)v49;
    v53 = *(void**)v47;
    v54 = *(int64_t*)v53;
    *(int64_t*)v50 = (int64_t)v54;
    memset(v57, 0, 1);
    v58 = *(int64_t*)v50;
    v59 = 0LL;
    v60 = v58 == v59;
    *(bool*)v57 = v60;
    if (v60) goto bb5;
    else goto bb4;
  bb4:
    v63 = *(int64_t*)v50;
    v64 = -1LL;
    v65 = v63 == v64;
    *(bool*)v57 = v65;
    goto bb5;
  bb5:
    v68 = *(bool*)v57;
    if (v68) goto bb6;
    else goto bb7;
  bb6:
    return;
  bb7:
    goto bb8;
  bb8:
    v73 = *(void**)v47;
    v74 = -1LL;
    *(int64_t*)v73 = (int64_t)v74;
    v77 = 0LL;
    memset(v78, 0, 8);
    *(int64_t*)v78 = (int64_t)v77;
    v81 = *(void**)v9;
    v82 = *(int64_t*)v81;
    *(int64_t*)v78 = (int64_t)v82;
    v85 = *(int64_t*)v78;
    v86 = 1LL;
    v87 = p_sub_i64(v85, v86);
    memset(v88, 0, 8);
    *(int64_t*)v88 = (int64_t)v87;
    v91 = *(void**)v9;
    v92 = *(int64_t*)v88;
    *(int64_t*)v91 = (int64_t)v92;
    return;
    return;
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

void HashMap_clear(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0; /* placeholder for void */
    void* v7 = 0;
    int64_t v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0; /* placeholder for void */
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
    int64_t v23 = 0; /* placeholder for void */
    void* v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    void* v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0; /* placeholder for void */
    void* v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0;
    bool v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    int64_t v38 = 0;
    bool v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0; /* placeholder for void */
    bool v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0;
    void* v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
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
    void* v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
    int64_t v59 = 0;
    void* v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0; /* placeholder for void */
    void* v63 = 0;
    int64_t v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0;
    int64_t v68 = 0;
    int64_t v69 = 0;
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0; /* placeholder for void */
    int64_t v74 = 0; /* placeholder for void */
    v4 = alloca(8);
    v11 = alloca(8);
    v15 = alloca(8);
    v18 = alloca(8);
    v21 = alloca(8);
    v31 = alloca(1);
    v45 = alloca(8);
    v57 = alloca(8);
    v60 = alloca(8);
    v2 = 16LL;
    v3 = p_add_i64(v1, v2);
    memset(v4, 0, 8);
    *(void**)v4 = (void*)v3;
    v7 = *(void**)v4;
    v8 = 0LL;
    *(int64_t*)v7 = (int64_t)v8;
    memset(v11, 0, 8);
    *(void**)v11 = (void*)v1;
    v13 = 24LL;
    v14 = p_add_i64(v1, v13);
    memset(v15, 0, 8);
    *(void**)v15 = (void*)v14;
    v17 = 0LL;
    memset(v18, 0, 8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = 0LL;
    memset(v21, 0, 8);
    *(int64_t*)v21 = (int64_t)v20;
    v24 = *(void**)v11;
    v25 = *(int64_t*)v24;
    *(int64_t*)v18 = (int64_t)v25;
    v27 = *(void**)v15;
    v28 = *(int64_t*)v27;
    *(int64_t*)v21 = (int64_t)v28;
    memset(v31, 0, 1);
    v32 = *(int64_t*)v18;
    v33 = 0LL;
    v34 = v32 != v33;
    *(bool*)v31 = v34;
    if (v34) goto bb1;
    else goto bb2;
  bb1:
    v37 = *(int64_t*)v21;
    v38 = 0LL;
    v39 = v37 > v38;
    *(bool*)v31 = v39;
    goto bb2;
  bb2:
    v42 = *(bool*)v31;
    if (v42) goto bb3;
    else goto bb4;
  bb3:
    v44 = 0LL;
    memset(v45, 0, 8);
    *(int64_t*)v45 = (int64_t)v44;
    goto bb6;
  bb4:
    goto bb5;
  bb5:
    return;
  bb6:
    v48 = *(int64_t*)v45;
    v49 = *(int64_t*)v21;
    v50 = v48 < v49;
    if (v50) goto bb7;
    else goto bb8;
  bb7:
    v52 = *(int64_t*)v18;
    v53 = *(int64_t*)v45;
    v54 = 8LL;
    v55 = p_mul_i64(v53, v54);
    v56 = p_add_i64(v52, v55);
    memset(v57, 0, 8);
    *(int64_t*)v57 = (int64_t)v56;
    v59 = *(int64_t*)v57;
    memset(v60, 0, 8);
    *(void**)v60 = (void*)v59;
    v63 = *(void**)v60;
    v64 = 0LL;
    *(int64_t*)v63 = (int64_t)v64;
    v67 = *(int64_t*)v45;
    v68 = 1LL;
    v69 = (int64_t)((uint64_t)v67 + (uint64_t)v68);
    *(int64_t*)v45 = (int64_t)v69;
    goto bb6;
  bb8:
    goto bb5;
    return;
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
    int64_t v46 = 0;
    int64_t v47 = 0;
    bool v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0;
    void* v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
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
    void* v68 = 0;
    int64_t v69 = 0;
    int64_t v70 = 0;
    bool v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0; /* placeholder for void */
    int64_t v74 = 0;
    int64_t v75 = 0;
    bool v76 = 0;
    int64_t v77 = 0; /* placeholder for void */
    int64_t v78 = 0; /* placeholder for void */
    bool v79 = 0;
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0;
    int64_t v82 = 0;
    int64_t v83 = 0;
    int64_t v84 = 0; /* placeholder for void */
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0; /* placeholder for void */
    int64_t v87 = 0;
    int64_t v88 = 0;
    int64_t v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0;
    int64_t v93 = 0;
    int64_t v94 = 0; /* placeholder for void */
    int64_t v95 = 0;
    int64_t v96 = 0; /* placeholder for void */
    v4 = alloca(8);
    v6 = alloca(8);
    v9 = alloca(8);
    v12 = alloca(8);
    v24 = alloca(8);
    v26 = alloca(1);
    v43 = alloca(8);
    v55 = alloca(8);
    v58 = alloca(8);
    v61 = alloca(8);
    v68 = alloca(1);
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
    v46 = *(int64_t*)v43;
    v47 = *(int64_t*)v9;
    v48 = v46 < v47;
    if (v48) goto bb7;
    else goto bb8;
  bb7:
    v50 = *(int64_t*)v12;
    v51 = *(int64_t*)v43;
    v52 = 8LL;
    v53 = p_mul_i64(v51, v52);
    v54 = p_add_i64(v50, v53);
    memset(v55, 0, 8);
    *(int64_t*)v55 = (int64_t)v54;
    v57 = *(int64_t*)v55;
    memset(v58, 0, 8);
    *(void**)v58 = (void*)v57;
    v60 = 0LL;
    memset(v61, 0, 8);
    *(int64_t*)v61 = (int64_t)v60;
    v64 = *(void**)v58;
    v65 = *(int64_t*)v64;
    *(int64_t*)v61 = (int64_t)v65;
    memset(v68, 0, 1);
    v69 = *(int64_t*)v61;
    v70 = 0LL;
    v71 = v69 != v70;
    *(bool*)v68 = v71;
    if (v71) goto bb9;
    else goto bb10;
  bb8:
    v92 = *(int64_t*)v24;
    v93 = 0LL;
    Vec_sort((void*)v93, v92);
    v95 = *(int64_t*)v24;
    return v95;
  bb9:
    v74 = *(int64_t*)v61;
    v75 = -1LL;
    v76 = v74 != v75;
    *(bool*)v68 = v76;
    goto bb10;
  bb10:
    v79 = *(bool*)v68;
    if (v79) goto bb11;
    else goto bb12;
  bb11:
    v81 = *(int64_t*)v24;
    v82 = *(int64_t*)v61;
    v83 = 0LL;
    Vec_push((void*)v83, v81, v82);
    goto bb13;
  bb12:
    goto bb13;
  bb13:
    v87 = *(int64_t*)v43;
    v88 = 1LL;
    v89 = (int64_t)((uint64_t)v87 + (uint64_t)v88);
    *(int64_t*)v43 = (int64_t)v89;
    goto bb6;
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
    int64_t v19 = 0;
    int64_t v20 = 0;
    bool v21 = 0;
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0;
    void* v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0;
    int64_t v30 = 0;
    int64_t v31 = 0;
    void* v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    v4 = alloca(8);
    v9 = alloca(8);
    v13 = alloca(8);
    v16 = alloca(8);
    v27 = alloca(8);
    v32 = alloca(8);
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
    v19 = *(int64_t*)v16;
    v20 = *(int64_t*)v9;
    v21 = v19 < v20;
    if (v21) goto bb2;
    else goto bb3;
  bb2:
    v23 = *(int64_t*)v4;
    v24 = *(int64_t*)v16;
    v25 = 0LL;
    v26 = (int64_t)Vec_get((void*)v25, v23, v24);
    memset(v27, 0, 8);
    *(int64_t*)v27 = (int64_t)v26;
    v29 = *(int64_t*)v27;
    v30 = 0LL;
    v31 = (int64_t)HashMap_get((void*)v30, v1, v29);
    memset(v32, 0, 8);
    *(int64_t*)v32 = (int64_t)v31;
    v34 = *(int64_t*)v13;
    v35 = *(int64_t*)v32;
    v36 = 0LL;
    Vec_push((void*)v36, v34, v35);
    v38 = *(int64_t*)v16;
    v39 = 1LL;
    v40 = (int64_t)((uint64_t)v38 + (uint64_t)v39);
    *(int64_t*)v16 = (int64_t)v40;
    goto bb1;
  bb3:
    v43 = *(int64_t*)v13;
    return v43;
    return 0;
}

int64_t HashMap_clone(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    int64_t v7 = 0;
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
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
    void* v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0;
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
    int64_t v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    void* v53 = 0;
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0;
    int64_t v57 = 0;
    bool v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    int64_t v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0;
    void* v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0;
    void* v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0;
    void* v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0; /* placeholder for void */
    void* v74 = 0;
    int64_t v75 = 0;
    int64_t v76 = 0; /* placeholder for void */
    int64_t v77 = 0; /* placeholder for void */
    void* v78 = 0;
    int64_t v79 = 0;
    int64_t v80 = 0;
    bool v81 = 0;
    int64_t v82 = 0; /* placeholder for void */
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0;
    int64_t v85 = 0;
    bool v86 = 0;
    int64_t v87 = 0; /* placeholder for void */
    int64_t v88 = 0; /* placeholder for void */
    bool v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0;
    int64_t v92 = 0;
    int64_t v93 = 0;
    int64_t v94 = 0;
    int64_t v95 = 0;
    void* v96 = 0;
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0;
    void* v99 = 0;
    int64_t v100 = 0; /* placeholder for void */
    int64_t v101 = 0;
    void* v102 = 0;
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0; /* placeholder for void */
    void* v105 = 0;
    int64_t v106 = 0;
    int64_t v107 = 0; /* placeholder for void */
    int64_t v108 = 0; /* placeholder for void */
    int64_t v109 = 0;
    int64_t v110 = 0;
    int64_t v111 = 0;
    int64_t v112 = 0;
    int64_t v113 = 0; /* placeholder for void */
    int64_t v114 = 0; /* placeholder for void */
    int64_t v115 = 0; /* placeholder for void */
    int64_t v116 = 0;
    int64_t v117 = 0;
    int64_t v118 = 0;
    int64_t v119 = 0; /* placeholder for void */
    int64_t v120 = 0; /* placeholder for void */
    int64_t v121 = 0;
    int64_t v122 = 0; /* placeholder for void */
    v4 = alloca(8);
    v8 = alloca(8);
    v10 = alloca(8);
    v14 = alloca(8);
    v17 = alloca(8);
    v20 = alloca(8);
    v23 = alloca(8);
    v36 = alloca(1);
    v53 = alloca(8);
    v65 = alloca(8);
    v68 = alloca(8);
    v71 = alloca(8);
    v78 = alloca(1);
    v96 = alloca(8);
    v99 = alloca(8);
    v102 = alloca(8);
    v2 = 0LL;
    v3 = (int64_t)HashMap_new((void*)v2);
    memset(v4, 0, 8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = 24LL;
    v7 = p_add_i64(v1, v6);
    memset(v8, 0, 8);
    *(void**)v8 = (void*)v7;
    memset(v10, 0, 8);
    *(void**)v10 = (void*)v1;
    v12 = 8LL;
    v13 = p_add_i64(v1, v12);
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
    v26 = *(void**)v8;
    v27 = *(int64_t*)v26;
    *(int64_t*)v17 = (int64_t)v27;
    v29 = *(void**)v10;
    v30 = *(int64_t*)v29;
    *(int64_t*)v20 = (int64_t)v30;
    v32 = *(void**)v14;
    v33 = *(int64_t*)v32;
    *(int64_t*)v23 = (int64_t)v33;
    memset(v36, 0, 1);
    v37 = *(int64_t*)v20;
    v38 = 0LL;
    v39 = v37 == v38;
    *(bool*)v36 = v39;
    if (v39) goto bb2;
    else goto bb1;
  bb1:
    v42 = *(int64_t*)v17;
    v43 = 0LL;
    v44 = v42 == v43;
    *(bool*)v36 = v44;
    goto bb2;
  bb2:
    v47 = *(bool*)v36;
    if (v47) goto bb3;
    else goto bb4;
  bb3:
    v49 = *(int64_t*)v4;
    return v49;
  bb4:
    goto bb5;
  bb5:
    v52 = 0LL;
    memset(v53, 0, 8);
    *(int64_t*)v53 = (int64_t)v52;
    goto bb6;
  bb6:
    v56 = *(int64_t*)v53;
    v57 = *(int64_t*)v17;
    v58 = v56 < v57;
    if (v58) goto bb7;
    else goto bb8;
  bb7:
    v60 = *(int64_t*)v20;
    v61 = *(int64_t*)v53;
    v62 = 8LL;
    v63 = p_mul_i64(v61, v62);
    v64 = p_add_i64(v60, v63);
    memset(v65, 0, 8);
    *(int64_t*)v65 = (int64_t)v64;
    v67 = *(int64_t*)v65;
    memset(v68, 0, 8);
    *(void**)v68 = (void*)v67;
    v70 = 0LL;
    memset(v71, 0, 8);
    *(int64_t*)v71 = (int64_t)v70;
    v74 = *(void**)v68;
    v75 = *(int64_t*)v74;
    *(int64_t*)v71 = (int64_t)v75;
    memset(v78, 0, 1);
    v79 = *(int64_t*)v71;
    v80 = 0LL;
    v81 = v79 != v80;
    *(bool*)v78 = v81;
    if (v81) goto bb9;
    else goto bb10;
  bb8:
    v121 = *(int64_t*)v4;
    return v121;
  bb9:
    v84 = *(int64_t*)v71;
    v85 = -1LL;
    v86 = v84 != v85;
    *(bool*)v78 = v86;
    goto bb10;
  bb10:
    v89 = *(bool*)v78;
    if (v89) goto bb11;
    else goto bb12;
  bb11:
    v91 = *(int64_t*)v23;
    v92 = *(int64_t*)v53;
    v93 = 8LL;
    v94 = p_mul_i64(v92, v93);
    v95 = p_add_i64(v91, v94);
    memset(v96, 0, 8);
    *(int64_t*)v96 = (int64_t)v95;
    v98 = *(int64_t*)v96;
    memset(v99, 0, 8);
    *(void**)v99 = (void*)v98;
    v101 = 0LL;
    memset(v102, 0, 8);
    *(int64_t*)v102 = (int64_t)v101;
    v105 = *(void**)v99;
    v106 = *(int64_t*)v105;
    *(int64_t*)v102 = (int64_t)v106;
    v109 = *(int64_t*)v4;
    v110 = *(int64_t*)v71;
    v111 = *(int64_t*)v102;
    v112 = 0LL;
    HashMap_insert((void*)v112, v109, v110, v111);
    goto bb13;
  bb12:
    goto bb13;
  bb13:
    v116 = *(int64_t*)v53;
    v117 = 1LL;
    v118 = (int64_t)((uint64_t)v116 + (uint64_t)v117);
    *(int64_t*)v53 = (int64_t)v118;
    goto bb6;
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
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    int64_t v16 = 0;
    bool v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    int64_t v19 = 0;
    void* v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0; /* placeholder for void */
    int64_t v23 = 0;
    int64_t v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    bool v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0;
    int64_t v34 = 0;
    void* v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0;
    void* v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0;
    int64_t v46 = 0;
    int64_t v47 = 0;
    int64_t v48 = 0;
    int64_t v49 = 0;
    bool v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0;
    int64_t v58 = 0;
    int64_t v59 = 0;
    int64_t v60 = 0;
    int64_t v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0;
    int64_t v67 = 0;
    int64_t v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0;
    int64_t v72 = 0;
    int64_t v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0; /* placeholder for void */
    void* v76 = 0;
    void* v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0;
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0;
    int64_t v83 = 0;
    bool v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0;
    int64_t v87 = 0;
    bool v88 = 0;
    int64_t v89 = 0; /* placeholder for void */
    void* v90 = 0;
    void* v91 = 0;
    void* v92 = 0;
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
    int64_t v104 = 0;
    void* v105 = 0;
    int64_t v106 = 0; /* placeholder for void */
    int64_t v107 = 0;
    int64_t v108 = 0;
    void* v109 = 0;
    void* v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    void* v112 = 0;
    void* v113 = 0;
    void* v114 = 0;
    int64_t v115 = 0; /* placeholder for void */
    void* v116 = 0;
    void* v117 = 0;
    void* v118 = 0;
    int64_t v119 = 0; /* placeholder for void */
    void* v120 = 0;
    int64_t v121 = 0;
    void* v122 = 0;
    void* v123 = 0;
    int64_t v124 = 0; /* placeholder for void */
    int64_t v125 = 0;
    int64_t v126 = 0;
    int64_t v127 = 0;
    int64_t v128 = 0; /* placeholder for void */
    int64_t v129 = 0; /* placeholder for void */
    void* v130 = 0;
    void* v131 = 0;
    void* v132 = 0;
    int64_t v133 = 0; /* placeholder for void */
    void* v134 = 0;
    int64_t v135 = 0; /* placeholder for void */
    v4 = alloca(8);
    v9 = alloca(8);
    v12 = alloca(8);
    v20 = alloca(8);
    v77 = alloca(8);
    v35 = alloca(8);
    v43 = alloca(8);
    v100 = alloca(8);
    v105 = alloca(8);
    v110 = alloca(8);
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
    goto bb1;
  bb1:
    v15 = *(int64_t*)v12;
    v16 = *(int64_t*)v9;
    v17 = v15 < v16;
    if (v17) goto bb2;
    else goto bb3;
  bb2:
    v19 = 0LL;
    memset(v20, 0, 8);
    *(int64_t*)v20 = (int64_t)v19;
    goto bb4;
  bb3:
    v76 = _str22;
    memset(v77, 0, 8);
    *(void**)v77 = (void*)v76;
    v79 = 0LL;
    *(int64_t*)v12 = (int64_t)v79;
    goto bb10;
  bb4:
    v23 = *(int64_t*)v20;
    v24 = 1LL;
    v25 = p_add_i64(v23, v24);
    v26 = *(int64_t*)v9;
    v27 = *(int64_t*)v12;
    v28 = p_sub_i64(v26, v27);
    v29 = v25 < v28;
    if (v29) goto bb5;
    else goto bb6;
  bb5:
    v31 = *(int64_t*)v4;
    v32 = *(int64_t*)v20;
    v33 = 0LL;
    v34 = (int64_t)Vec_get((void*)v33, v31, v32);
    memset(v35, 0, 8);
    *(int64_t*)v35 = (int64_t)v34;
    v37 = *(int64_t*)v4;
    v38 = *(int64_t*)v20;
    v39 = 1LL;
    v40 = p_add_i64(v38, v39);
    v41 = 0LL;
    v42 = (int64_t)Vec_get((void*)v41, v37, v40);
    memset(v43, 0, 8);
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
    v73 = (int64_t)((uint64_t)v71 + (uint64_t)v72);
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
    v60 = p_add_i64(v58, v59);
    v61 = *(int64_t*)v35;
    v62 = 0LL;
    Vec_set((void*)v62, v57, v60, v61);
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    v66 = *(int64_t*)v20;
    v67 = 1LL;
    v68 = p_add_i64(v66, v67);
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
    v131 = _str23;
    v132 = p_str_concat(v130, v131);
    *(void**)v77 = (void*)v132;
    v134 = *(void**)v77;
    return v134;
  bb13:
    v90 = *(void**)v77;
    v91 = _str21;
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
    memset(v100, 0, 8);
    *(int64_t*)v100 = (int64_t)v99;
    v102 = *(int64_t*)v100;
    v103 = 0LL;
    v104 = (int64_t)HashMap_get((void*)v103, v1, v102);
    memset(v105, 0, 8);
    *(int64_t*)v105 = (int64_t)v104;
    v107 = *(int64_t*)v100;
    v108 = 0LL;
    v109 = p_str_from_ptr((void*)v108, v107);
    memset(v110, 0, 8);
    *(void**)v110 = (void*)v109;
    v112 = *(void**)v77;
    v113 = *(void**)v110;
    v114 = p_str_concat(v112, v113);
    *(void**)v77 = (void*)v114;
    v116 = *(void**)v77;
    v117 = _str2;
    v118 = p_str_concat(v116, v117);
    *(void**)v77 = (void*)v118;
    v120 = *(void**)v77;
    v121 = *(int64_t*)v105;
    v122 = (void*)p_to_str((int64_t)v121);
    v123 = p_str_concat(v120, v122);
    *(void**)v77 = (void*)v123;
    v125 = *(int64_t*)v12;
    v126 = 1LL;
    v127 = (int64_t)((uint64_t)v125 + (uint64_t)v126);
    *(int64_t*)v12 = (int64_t)v127;
    goto bb10;
    return 0;
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
    int64_t v34 = 0;
    int64_t v35 = 0;
    bool v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    void* v48 = 0;
    uint8_t v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    void* v56 = 0;
    void* v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
    int64_t v59 = 0;
    void* v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    void* v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0;
    void* v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    void* v69 = 0;
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0;
    int64_t v73 = 0;
    bool v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    void* v76 = 0;
    uint8_t v77 = 0;
    void* v78 = 0;
    int64_t v79 = 0; /* placeholder for void */
    void* v80 = 0;
    int64_t v81 = 0;
    int64_t v82 = 0; /* placeholder for void */
    int64_t v83 = 0;
    int64_t v84 = 0;
    int64_t v85 = 0;
    int64_t v86 = 0; /* placeholder for void */
    int64_t v87 = 0;
    int64_t v88 = 0;
    int64_t v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0;
    int64_t v92 = 0; /* placeholder for void */
    void* v93 = 0;
    void* v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0;
    int64_t v97 = 0;
    int64_t v98 = 0;
    int64_t v99 = 0; /* placeholder for void */
    int64_t v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0; /* placeholder for void */
    void* v103 = 0;
    int64_t v104 = 0;
    int64_t v105 = 0; /* placeholder for void */
    int64_t v106 = 0; /* placeholder for void */
    int64_t v107 = 0;
    void* v108 = 0;
    int64_t v109 = 0; /* placeholder for void */
    void* v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    v7 = alloca(8);
    v10 = alloca(8);
    v20 = alloca(8);
    v23 = alloca(8);
    v26 = alloca(8);
    v31 = alloca(8);
    v57 = alloca(8);
    v60 = alloca(8);
    v62 = alloca(8);
    v65 = alloca(8);
    v69 = alloca(8);
    v78 = alloca(8);
    v94 = alloca(8);
    v108 = alloca(8);
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
    v34 = *(int64_t*)v31;
    v35 = 0LL;
    v36 = v34 != v35;
    if (v36) goto bb5;
    else goto bb6;
  bb5:
    v38 = *(int64_t*)v26;
    v39 = 1LL;
    v40 = p_add_i64(v38, v39);
    *(int64_t*)v26 = (int64_t)v40;
    v42 = *(int64_t*)v20;
    v43 = 1LL;
    v44 = p_add_i64(v42, v43);
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
    v55 = p_add_i64(v53, v54);
    v56 = p_malloc(v55);
    memset(v57, 0, 8);
    *(int64_t*)v57 = (int64_t)v56;
    v59 = *(int64_t*)v57;
    memset(v60, 0, 8);
    *(void**)v60 = (void*)v59;
    memset(v62, 0, 8);
    *(int64_t*)v62 = (int64_t)v1;
    v64 = *(int64_t*)v62;
    memset(v65, 0, 8);
    *(void**)v65 = (void*)v64;
    v68 = 0LL;
    memset(v69, 0, 8);
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
    memset(v78, 0, 8);
    *(int64_t*)v78 = (int64_t)v77;
    v80 = *(void**)v60;
    v81 = *(int64_t*)v78;
    *(uint8_t*)v80 = (uint8_t)v81;
    v83 = *(int64_t*)v69;
    v84 = 1LL;
    v85 = (int64_t)((uint64_t)v83 + (uint64_t)v84);
    *(int64_t*)v69 = (int64_t)v85;
    v87 = *(int64_t*)v62;
    v88 = 1LL;
    v89 = p_add_i64(v87, v88);
    *(int64_t*)v62 = (int64_t)v89;
    v91 = *(int64_t*)v62;
    *(void**)v65 = (void*)v91;
    v93 = *(void**)v60;
    memset(v94, 0, 8);
    *(int64_t*)v94 = (int64_t)v93;
    v96 = *(int64_t*)v94;
    v97 = 1LL;
    v98 = p_add_i64(v96, v97);
    *(int64_t*)v94 = (int64_t)v98;
    v100 = *(int64_t*)v94;
    *(void**)v60 = (void*)v100;
    goto bb7;
  bb9:
    v103 = *(void**)v60;
    v104 = 0LL;
    *(uint8_t*)v103 = (uint8_t)v104;
    v107 = *(int64_t*)v57;
    memset(v108, 0, 8);
    *(void**)v108 = (void*)v107;
    v110 = *(void**)v108;
    return v110;
    return 0;
}

int64_t p_str_length(void* v0, int64_t v1) {
    int64_t v2 = 0;
    bool v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    int64_t v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    void* v8 = 0;
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0;
    void* v11 = 0;
    int64_t v12 = 0; /* placeholder for void */
    int64_t v13 = 0;
    void* v14 = 0;
    int64_t v15 = 0; /* placeholder for void */
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    uint8_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    int64_t v23 = 0;
    bool v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    int64_t v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    void* v36 = 0;
    uint8_t v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    int64_t v42 = 0; /* placeholder for void */
    v8 = alloca(8);
    v11 = alloca(8);
    v14 = alloca(8);
    v19 = alloca(8);
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
    memset(v8, 0, 8);
    *(int64_t*)v8 = (int64_t)v1;
    v10 = *(int64_t*)v8;
    memset(v11, 0, 8);
    *(void**)v11 = (void*)v10;
    v13 = 0LL;
    memset(v14, 0, 8);
    *(int64_t*)v14 = (int64_t)v13;
    v17 = *(void**)v11;
    v18 = *(uint8_t*)v17;
    memset(v19, 0, 8);
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
    v28 = p_add_i64(v26, v27);
    *(int64_t*)v14 = (int64_t)v28;
    v30 = *(int64_t*)v8;
    v31 = 1LL;
    v32 = p_add_i64(v30, v31);
    *(int64_t*)v8 = (int64_t)v32;
    v34 = *(int64_t*)v8;
    *(void**)v11 = (void*)v34;
    v36 = *(void**)v11;
    v37 = *(uint8_t*)v36;
    *(int64_t*)v19 = (int64_t)v37;
    goto bb4;
  bb6:
    v41 = *(int64_t*)v14;
    return v41;
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
    int64_t v12 = 0;
    bool v13 = 0;
    int64_t v14 = 0; /* placeholder for void */
    int64_t v15 = 0;
    void* v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0; /* placeholder for void */
    void* v22 = 0;
    uint8_t v23 = 0;
    void* v24 = 0;
    int64_t v25 = 0; /* placeholder for void */
    void* v26 = 0;
    int64_t v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0; /* placeholder for void */
    v4 = alloca(8);
    v6 = alloca(8);
    v9 = alloca(8);
    v16 = alloca(8);
    v19 = alloca(8);
    v24 = alloca(8);
    memset(v4, 0, 8);
    *(int64_t*)v4 = (int64_t)v1;
    memset(v6, 0, 8);
    *(int64_t*)v6 = (int64_t)v2;
    v8 = 0LL;
    memset(v9, 0, 8);
    *(int64_t*)v9 = (int64_t)v8;
    goto bb1;
  bb1:
    v12 = *(int64_t*)v9;
    v13 = v12 < v3;
    if (v13) goto bb2;
    else goto bb3;
  bb2:
    v15 = *(int64_t*)v4;
    memset(v16, 0, 8);
    *(void**)v16 = (void*)v15;
    v18 = *(int64_t*)v6;
    memset(v19, 0, 8);
    *(void**)v19 = (void*)v18;
    v22 = *(void**)v19;
    v23 = *(uint8_t*)v22;
    memset(v24, 0, 8);
    *(int64_t*)v24 = (int64_t)v23;
    v26 = *(void**)v16;
    v27 = *(int64_t*)v24;
    *(uint8_t*)v26 = (uint8_t)v27;
    v30 = *(int64_t*)v4;
    v31 = 1LL;
    v32 = p_add_i64(v30, v31);
    *(int64_t*)v4 = (int64_t)v32;
    v34 = *(int64_t*)v6;
    v35 = 1LL;
    v36 = p_add_i64(v34, v35);
    *(int64_t*)v6 = (int64_t)v36;
    v38 = *(int64_t*)v9;
    v39 = 1LL;
    v40 = (int64_t)((uint64_t)v38 + (uint64_t)v39);
    *(int64_t*)v9 = (int64_t)v40;
    goto bb1;
  bb3:
    return;
    return;
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

int64_t String_len(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    return v3;
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
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    bool v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    bool v49 = 0;
    void* v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    void* v53 = 0;
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0;
    int64_t v57 = 0;
    bool v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    int64_t v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0;
    void* v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0;
    int64_t v67 = 0;
    void* v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0;
    void* v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0;
    void* v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0;
    void* v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0;
    void* v80 = 0;
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0; /* placeholder for void */
    void* v83 = 0;
    uint8_t v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    void* v86 = 0;
    uint8_t v87 = 0;
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0;
    int64_t v91 = 0;
    bool v92 = 0;
    int64_t v93 = 0; /* placeholder for void */
    bool v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0;
    int64_t v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0; /* placeholder for void */
    bool v103 = 0;
    int64_t v104 = 0; /* placeholder for void */
    bool v105 = 0;
    int64_t v106 = 0; /* placeholder for void */
    int64_t v107 = 0; /* placeholder for void */
    int64_t v108 = 0;
    int64_t v109 = 0;
    int64_t v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    int64_t v112 = 0; /* placeholder for void */
    bool v113 = 0;
    int64_t v114 = 0; /* placeholder for void */
    v3 = alloca(1);
    v19 = alloca(8);
    v23 = alloca(8);
    v40 = alloca(8);
    v50 = alloca(1);
    v53 = alloca(8);
    v64 = alloca(8);
    v68 = alloca(8);
    v71 = alloca(8);
    v74 = alloca(8);
    v77 = alloca(8);
    v80 = alloca(8);
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
    v43 = *(int64_t*)v40;
    v44 = *(int64_t*)v19;
    v45 = *(int64_t*)v23;
    v46 = p_sub_i64(v44, v45);
    v47 = v43 <= v46;
    if (v47) goto bb13;
    else goto bb14;
  bb13:
    v49 = true;
    memset(v50, 0, 1);
    *(bool*)v50 = v49;
    v52 = 0LL;
    memset(v53, 0, 8);
    *(int64_t*)v53 = (int64_t)v52;
    goto bb15;
  bb14:
    v113 = false;
    return v113;
  bb15:
    v56 = *(int64_t*)v53;
    v57 = *(int64_t*)v23;
    v58 = v56 < v57;
    if (v58) goto bb16;
    else goto bb17;
  bb16:
    v60 = *(int64_t*)v40;
    v61 = p_add_i64(v1, v60);
    v62 = *(int64_t*)v53;
    v63 = p_add_i64(v61, v62);
    memset(v64, 0, 8);
    *(int64_t*)v64 = (int64_t)v63;
    v66 = *(int64_t*)v53;
    v67 = p_add_i64(v2, v66);
    memset(v68, 0, 8);
    *(int64_t*)v68 = (int64_t)v67;
    v70 = *(int64_t*)v64;
    memset(v71, 0, 8);
    *(void**)v71 = (void*)v70;
    v73 = *(int64_t*)v68;
    memset(v74, 0, 8);
    *(void**)v74 = (void*)v73;
    v76 = 0LL;
    memset(v77, 0, 8);
    *(int64_t*)v77 = (int64_t)v76;
    v79 = 0LL;
    memset(v80, 0, 8);
    *(int64_t*)v80 = (int64_t)v79;
    v83 = *(void**)v71;
    v84 = *(uint8_t*)v83;
    *(int64_t*)v77 = (int64_t)v84;
    v86 = *(void**)v74;
    v87 = *(uint8_t*)v86;
    *(int64_t*)v80 = (int64_t)v87;
    v90 = *(int64_t*)v77;
    v91 = *(int64_t*)v80;
    v92 = v90 != v91;
    if (v92) goto bb18;
    else goto bb19;
  bb17:
    v103 = *(bool*)v50;
    if (v103) goto bb21;
    else goto bb22;
  bb18:
    v94 = false;
    *(bool*)v50 = v94;
    goto bb17;
  bb19:
    goto bb20;
  bb20:
    v98 = *(int64_t*)v53;
    v99 = 1LL;
    v100 = (int64_t)((uint64_t)v98 + (uint64_t)v99);
    *(int64_t*)v53 = (int64_t)v100;
    goto bb15;
  bb21:
    v105 = true;
    return v105;
  bb22:
    goto bb23;
  bb23:
    v108 = *(int64_t*)v40;
    v109 = 1LL;
    v110 = p_add_i64(v108, v109);
    *(int64_t*)v40 = (int64_t)v110;
    goto bb12;
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
    int64_t v36 = 0;
    int64_t v37 = 0;
    bool v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0;
    int64_t v41 = 0;
    void* v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
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
    int64_t v57 = 0;
    void* v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0; /* placeholder for void */
    void* v61 = 0;
    uint8_t v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    void* v64 = 0;
    uint8_t v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    int64_t v69 = 0;
    bool v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    bool v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0;
    int64_t v76 = 0;
    int64_t v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0; /* placeholder for void */
    bool v80 = 0;
    int64_t v81 = 0; /* placeholder for void */
    v3 = alloca(1);
    v19 = alloca(8);
    v23 = alloca(8);
    v33 = alloca(8);
    v42 = alloca(8);
    v46 = alloca(8);
    v49 = alloca(8);
    v52 = alloca(8);
    v55 = alloca(8);
    v58 = alloca(8);
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
    v36 = *(int64_t*)v33;
    v37 = *(int64_t*)v23;
    v38 = v36 < v37;
    if (v38) goto bb10;
    else goto bb11;
  bb10:
    v40 = *(int64_t*)v33;
    v41 = p_add_i64(v1, v40);
    memset(v42, 0, 8);
    *(int64_t*)v42 = (int64_t)v41;
    v44 = *(int64_t*)v33;
    v45 = p_add_i64(v2, v44);
    memset(v46, 0, 8);
    *(int64_t*)v46 = (int64_t)v45;
    v48 = *(int64_t*)v42;
    memset(v49, 0, 8);
    *(void**)v49 = (void*)v48;
    v51 = *(int64_t*)v46;
    memset(v52, 0, 8);
    *(void**)v52 = (void*)v51;
    v54 = 0LL;
    memset(v55, 0, 8);
    *(int64_t*)v55 = (int64_t)v54;
    v57 = 0LL;
    memset(v58, 0, 8);
    *(int64_t*)v58 = (int64_t)v57;
    v61 = *(void**)v49;
    v62 = *(uint8_t*)v61;
    *(int64_t*)v55 = (int64_t)v62;
    v64 = *(void**)v52;
    v65 = *(uint8_t*)v64;
    *(int64_t*)v58 = (int64_t)v65;
    v68 = *(int64_t*)v55;
    v69 = *(int64_t*)v58;
    v70 = v68 != v69;
    if (v70) goto bb12;
    else goto bb13;
  bb11:
    v80 = true;
    return v80;
  bb12:
    v72 = false;
    return v72;
  bb13:
    goto bb14;
  bb14:
    v75 = *(int64_t*)v33;
    v76 = 1LL;
    v77 = (int64_t)((uint64_t)v75 + (uint64_t)v76);
    *(int64_t*)v33 = (int64_t)v77;
    goto bb9;
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
    int64_t v48 = 0;
    int64_t v49 = 0;
    bool v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    void* v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    int64_t v59 = 0;
    void* v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    int64_t v62 = 0;
    void* v63 = 0;
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0;
    void* v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    void* v69 = 0;
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0;
    void* v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
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
    bool v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    bool v86 = 0;
    int64_t v87 = 0; /* placeholder for void */
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0;
    int64_t v90 = 0;
    int64_t v91 = 0;
    int64_t v92 = 0; /* placeholder for void */
    int64_t v93 = 0; /* placeholder for void */
    bool v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    v3 = alloca(1);
    v19 = alloca(8);
    v23 = alloca(8);
    v42 = alloca(8);
    v45 = alloca(8);
    v56 = alloca(8);
    v60 = alloca(8);
    v63 = alloca(8);
    v66 = alloca(8);
    v69 = alloca(8);
    v72 = alloca(8);
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
    v48 = *(int64_t*)v45;
    v49 = *(int64_t*)v23;
    v50 = v48 < v49;
    if (v50) goto bb13;
    else goto bb14;
  bb13:
    v52 = *(int64_t*)v42;
    v53 = p_add_i64(v1, v52);
    v54 = *(int64_t*)v45;
    v55 = p_add_i64(v53, v54);
    memset(v56, 0, 8);
    *(int64_t*)v56 = (int64_t)v55;
    v58 = *(int64_t*)v45;
    v59 = p_add_i64(v2, v58);
    memset(v60, 0, 8);
    *(int64_t*)v60 = (int64_t)v59;
    v62 = *(int64_t*)v56;
    memset(v63, 0, 8);
    *(void**)v63 = (void*)v62;
    v65 = *(int64_t*)v60;
    memset(v66, 0, 8);
    *(void**)v66 = (void*)v65;
    v68 = 0LL;
    memset(v69, 0, 8);
    *(int64_t*)v69 = (int64_t)v68;
    v71 = 0LL;
    memset(v72, 0, 8);
    *(int64_t*)v72 = (int64_t)v71;
    v75 = *(void**)v63;
    v76 = *(uint8_t*)v75;
    *(int64_t*)v69 = (int64_t)v76;
    v78 = *(void**)v66;
    v79 = *(uint8_t*)v78;
    *(int64_t*)v72 = (int64_t)v79;
    v82 = *(int64_t*)v69;
    v83 = *(int64_t*)v72;
    v84 = v82 != v83;
    if (v84) goto bb15;
    else goto bb16;
  bb14:
    v94 = true;
    return v94;
  bb15:
    v86 = false;
    return v86;
  bb16:
    goto bb17;
  bb17:
    v89 = *(int64_t*)v45;
    v90 = 1LL;
    v91 = (int64_t)((uint64_t)v89 + (uint64_t)v90);
    *(int64_t*)v45 = (int64_t)v91;
    goto bb12;
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
    void* v41 = 0;
    void* v42 = 0;
    void* v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    bool v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    int64_t v50 = 0;
    bool v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
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
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0;
    int64_t v66 = 0;
    bool v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0; /* placeholder for void */
    bool v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0;
    int64_t v73 = 0;
    int64_t v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0;
    int64_t v77 = 0;
    int64_t v78 = 0;
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0;
    int64_t v81 = 0; /* placeholder for void */
    void* v82 = 0;
    uint8_t v83 = 0;
    int64_t v84 = 0; /* placeholder for void */
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0; /* placeholder for void */
    int64_t v87 = 0;
    int64_t v88 = 0;
    bool v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0;
    void* v92 = 0;
    void* v93 = 0;
    int64_t v94 = 0; /* placeholder for void */
    int64_t v95 = 0;
    void* v96 = 0;
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0; /* placeholder for void */
    void* v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0; /* placeholder for void */
    int64_t v103 = 0;
    int64_t v104 = 0; /* placeholder for void */
    int64_t v105 = 0; /* placeholder for void */
    int64_t v106 = 0;
    int64_t v107 = 0;
    int64_t v108 = 0;
    int64_t v109 = 0;
    void* v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    int64_t v112 = 0;
    void* v113 = 0;
    int64_t v114 = 0; /* placeholder for void */
    int64_t v115 = 0; /* placeholder for void */
    void* v116 = 0;
    uint8_t v117 = 0;
    void* v118 = 0;
    int64_t v119 = 0; /* placeholder for void */
    int64_t v120 = 0; /* placeholder for void */
    void* v121 = 0;
    void* v122 = 0;
    void* v123 = 0;
    int64_t v124 = 0;
    int64_t v125 = 0;
    bool v126 = 0;
    int64_t v127 = 0; /* placeholder for void */
    int64_t v128 = 0; /* placeholder for void */
    int64_t v129 = 0;
    int64_t v130 = 0;
    bool v131 = 0;
    int64_t v132 = 0; /* placeholder for void */
    int64_t v133 = 0; /* placeholder for void */
    bool v134 = 0;
    int64_t v135 = 0; /* placeholder for void */
    int64_t v136 = 0; /* placeholder for void */
    int64_t v137 = 0;
    int64_t v138 = 0;
    bool v139 = 0;
    int64_t v140 = 0; /* placeholder for void */
    int64_t v141 = 0; /* placeholder for void */
    bool v142 = 0;
    int64_t v143 = 0; /* placeholder for void */
    int64_t v144 = 0; /* placeholder for void */
    int64_t v145 = 0;
    int64_t v146 = 0;
    bool v147 = 0;
    int64_t v148 = 0; /* placeholder for void */
    int64_t v149 = 0; /* placeholder for void */
    bool v150 = 0;
    int64_t v151 = 0; /* placeholder for void */
    int64_t v152 = 0;
    int64_t v153 = 0;
    int64_t v154 = 0;
    int64_t v155 = 0; /* placeholder for void */
    int64_t v156 = 0;
    int64_t v157 = 0;
    bool v158 = 0;
    int64_t v159 = 0; /* placeholder for void */
    int64_t v160 = 0; /* placeholder for void */
    int64_t v161 = 0; /* placeholder for void */
    int64_t v162 = 0;
    int64_t v163 = 0;
    int64_t v164 = 0;
    int64_t v165 = 0;
    int64_t v166 = 0; /* placeholder for void */
    int64_t v167 = 0;
    int64_t v168 = 0; /* placeholder for void */
    void* v169 = 0;
    uint8_t v170 = 0;
    int64_t v171 = 0; /* placeholder for void */
    int64_t v172 = 0; /* placeholder for void */
    int64_t v173 = 0; /* placeholder for void */
    int64_t v174 = 0;
    int64_t v175 = 0;
    int64_t v176 = 0;
    void* v177 = 0;
    int64_t v178 = 0; /* placeholder for void */
    int64_t v179 = 0;
    int64_t v180 = 0;
    bool v181 = 0;
    int64_t v182 = 0; /* placeholder for void */
    int64_t v183 = 0;
    void* v184 = 0;
    void* v185 = 0;
    int64_t v186 = 0; /* placeholder for void */
    int64_t v187 = 0;
    void* v188 = 0;
    int64_t v189 = 0; /* placeholder for void */
    int64_t v190 = 0; /* placeholder for void */
    void* v191 = 0;
    int64_t v192 = 0;
    int64_t v193 = 0; /* placeholder for void */
    int64_t v194 = 0; /* placeholder for void */
    int64_t v195 = 0;
    int64_t v196 = 0; /* placeholder for void */
    int64_t v197 = 0; /* placeholder for void */
    int64_t v198 = 0;
    int64_t v199 = 0;
    int64_t v200 = 0;
    void* v201 = 0;
    void* v202 = 0;
    int64_t v203 = 0; /* placeholder for void */
    int64_t v204 = 0;
    int64_t v205 = 0;
    int64_t v206 = 0;
    int64_t v207 = 0;
    int64_t v208 = 0;
    int64_t v209 = 0; /* placeholder for void */
    int64_t v210 = 0;
    int64_t v211 = 0;
    int64_t v212 = 0;
    void* v213 = 0;
    int64_t v214 = 0; /* placeholder for void */
    int64_t v215 = 0;
    void* v216 = 0;
    int64_t v217 = 0; /* placeholder for void */
    int64_t v218 = 0; /* placeholder for void */
    void* v219 = 0;
    int64_t v220 = 0;
    int64_t v221 = 0; /* placeholder for void */
    int64_t v222 = 0; /* placeholder for void */
    int64_t v223 = 0;
    int64_t v224 = 0; /* placeholder for void */
    v7 = alloca(8);
    v10 = alloca(8);
    v22 = alloca(8);
    v25 = alloca(8);
    v28 = alloca(8);
    v30 = alloca(8);
    v33 = alloca(8);
    v38 = alloca(8);
    v41 = alloca(1);
    v42 = alloca(1);
    v43 = alloca(1);
    v93 = alloca(8);
    v96 = alloca(8);
    v110 = alloca(8);
    v113 = alloca(8);
    v118 = alloca(8);
    v121 = alloca(1);
    v122 = alloca(1);
    v123 = alloca(1);
    v177 = alloca(8);
    v185 = alloca(8);
    v188 = alloca(8);
    v202 = alloca(8);
    v213 = alloca(8);
    v216 = alloca(8);
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
    memset(v41, 0, 1);
    memset(v42, 0, 1);
    memset(v43, 0, 1);
    v44 = *(int64_t*)v38;
    v45 = 32LL;
    v46 = v44 == v45;
    *(bool*)v43 = v46;
    if (v46) goto bb8;
    else goto bb7;
  bb5:
    v72 = *(int64_t*)v25;
    v73 = 1LL;
    v74 = p_add_i64(v72, v73);
    *(int64_t*)v25 = (int64_t)v74;
    v76 = *(int64_t*)v30;
    v77 = 1LL;
    v78 = p_add_i64(v76, v77);
    *(int64_t*)v30 = (int64_t)v78;
    v80 = *(int64_t*)v30;
    *(void**)v33 = (void*)v80;
    v82 = *(void**)v33;
    v83 = *(uint8_t*)v82;
    *(int64_t*)v38 = (int64_t)v83;
    goto bb4;
  bb6:
    v87 = *(int64_t*)v25;
    v88 = *(int64_t*)v28;
    v89 = v87 >= v88;
    if (v89) goto bb13;
    else goto bb14;
  bb7:
    v49 = *(int64_t*)v38;
    v50 = 9LL;
    v51 = v49 == v50;
    *(bool*)v43 = v51;
    goto bb8;
  bb8:
    v54 = *(bool*)v43;
    *(bool*)v42 = v54;
    if (v54) goto bb10;
    else goto bb9;
  bb9:
    v57 = *(int64_t*)v38;
    v58 = 10LL;
    v59 = v57 == v58;
    *(bool*)v42 = v59;
    goto bb10;
  bb10:
    v62 = *(bool*)v42;
    *(bool*)v41 = v62;
    if (v62) goto bb12;
    else goto bb11;
  bb11:
    v65 = *(int64_t*)v38;
    v66 = 13LL;
    v67 = v65 == v66;
    *(bool*)v41 = v67;
    goto bb12;
  bb12:
    v70 = *(bool*)v41;
    if (v70) goto bb5;
    else goto bb6;
  bb13:
    v91 = 1LL;
    v92 = p_malloc(v91);
    memset(v93, 0, 8);
    *(int64_t*)v93 = (int64_t)v92;
    v95 = *(int64_t*)v93;
    memset(v96, 0, 8);
    *(void**)v96 = (void*)v95;
    v99 = *(void**)v96;
    v100 = 0LL;
    *(uint8_t*)v99 = (uint8_t)v100;
    v103 = *(int64_t*)v93;
    return v103;
  bb14:
    goto bb15;
  bb15:
    v106 = *(int64_t*)v28;
    v107 = p_add_i64(v1, v106);
    v108 = 1LL;
    v109 = p_sub_i64(v107, v108);
    memset(v110, 0, 8);
    *(int64_t*)v110 = (int64_t)v109;
    v112 = *(int64_t*)v110;
    memset(v113, 0, 8);
    *(void**)v113 = (void*)v112;
    v116 = *(void**)v113;
    v117 = *(uint8_t*)v116;
    memset(v118, 0, 8);
    *(int64_t*)v118 = (int64_t)v117;
    goto bb16;
  bb16:
    memset(v121, 0, 1);
    memset(v122, 0, 1);
    memset(v123, 0, 1);
    v124 = *(int64_t*)v118;
    v125 = 32LL;
    v126 = v124 == v125;
    *(bool*)v123 = v126;
    if (v126) goto bb20;
    else goto bb19;
  bb17:
    v152 = *(int64_t*)v28;
    v153 = 1LL;
    v154 = p_sub_i64(v152, v153);
    *(int64_t*)v28 = (int64_t)v154;
    v156 = *(int64_t*)v28;
    v157 = *(int64_t*)v25;
    v158 = v156 <= v157;
    if (v158) goto bb25;
    else goto bb26;
  bb18:
    v174 = *(int64_t*)v28;
    v175 = *(int64_t*)v25;
    v176 = p_sub_i64(v174, v175);
    memset(v177, 0, 8);
    *(int64_t*)v177 = (int64_t)v176;
    v179 = *(int64_t*)v177;
    v180 = 0LL;
    v181 = v179 <= v180;
    if (v181) goto bb28;
    else goto bb29;
  bb19:
    v129 = *(int64_t*)v118;
    v130 = 9LL;
    v131 = v129 == v130;
    *(bool*)v123 = v131;
    goto bb20;
  bb20:
    v134 = *(bool*)v123;
    *(bool*)v122 = v134;
    if (v134) goto bb22;
    else goto bb21;
  bb21:
    v137 = *(int64_t*)v118;
    v138 = 10LL;
    v139 = v137 == v138;
    *(bool*)v122 = v139;
    goto bb22;
  bb22:
    v142 = *(bool*)v122;
    *(bool*)v121 = v142;
    if (v142) goto bb24;
    else goto bb23;
  bb23:
    v145 = *(int64_t*)v118;
    v146 = 13LL;
    v147 = v145 == v146;
    *(bool*)v121 = v147;
    goto bb24;
  bb24:
    v150 = *(bool*)v121;
    if (v150) goto bb17;
    else goto bb18;
  bb25:
    goto bb18;
  bb26:
    goto bb27;
  bb27:
    v162 = *(int64_t*)v28;
    v163 = p_add_i64(v1, v162);
    v164 = 1LL;
    v165 = p_sub_i64(v163, v164);
    *(int64_t*)v110 = (int64_t)v165;
    v167 = *(int64_t*)v110;
    *(void**)v113 = (void*)v167;
    v169 = *(void**)v113;
    v170 = *(uint8_t*)v169;
    *(int64_t*)v118 = (int64_t)v170;
    goto bb16;
  bb28:
    v183 = 1LL;
    v184 = p_malloc(v183);
    memset(v185, 0, 8);
    *(int64_t*)v185 = (int64_t)v184;
    v187 = *(int64_t*)v185;
    memset(v188, 0, 8);
    *(void**)v188 = (void*)v187;
    v191 = *(void**)v188;
    v192 = 0LL;
    *(uint8_t*)v191 = (uint8_t)v192;
    v195 = *(int64_t*)v185;
    return v195;
  bb29:
    goto bb30;
  bb30:
    v198 = *(int64_t*)v177;
    v199 = 1LL;
    v200 = p_add_i64(v198, v199);
    v201 = p_malloc(v200);
    memset(v202, 0, 8);
    *(int64_t*)v202 = (int64_t)v201;
    v204 = *(int64_t*)v202;
    v205 = *(int64_t*)v25;
    v206 = p_add_i64(v1, v205);
    v207 = *(int64_t*)v177;
    v208 = 0LL;
    p_str_copy_n((void*)v208, v204, v206, v207);
    v210 = *(int64_t*)v202;
    v211 = *(int64_t*)v177;
    v212 = p_add_i64(v210, v211);
    memset(v213, 0, 8);
    *(int64_t*)v213 = (int64_t)v212;
    v215 = *(int64_t*)v213;
    memset(v216, 0, 8);
    *(void**)v216 = (void*)v215;
    v219 = *(void**)v216;
    v220 = 0LL;
    *(uint8_t*)v219 = (uint8_t)v220;
    v223 = *(int64_t*)v202;
    return v223;
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
    int64_t v34 = 0;
    int64_t v35 = 0;
    bool v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0;
    int64_t v39 = 0;
    void* v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0;
    void* v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0;
    void* v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0;
    void* v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    void* v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    void* v57 = 0;
    uint8_t v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0; /* placeholder for void */
    void* v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0;
    bool v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0;
    int64_t v68 = 0;
    bool v69 = 0;
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0; /* placeholder for void */
    bool v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    int64_t v74 = 0;
    int64_t v75 = 0;
    int64_t v76 = 0;
    int64_t v77 = 0; /* placeholder for void */
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0; /* placeholder for void */
    void* v81 = 0;
    int64_t v82 = 0;
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0; /* placeholder for void */
    int64_t v85 = 0;
    int64_t v86 = 0;
    int64_t v87 = 0;
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0;
    int64_t v91 = 0;
    int64_t v92 = 0;
    void* v93 = 0;
    int64_t v94 = 0; /* placeholder for void */
    int64_t v95 = 0;
    void* v96 = 0;
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0; /* placeholder for void */
    void* v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0; /* placeholder for void */
    int64_t v103 = 0;
    int64_t v104 = 0; /* placeholder for void */
    v7 = alloca(8);
    v10 = alloca(8);
    v22 = alloca(8);
    v28 = alloca(8);
    v31 = alloca(8);
    v40 = alloca(8);
    v45 = alloca(8);
    v48 = alloca(8);
    v51 = alloca(8);
    v54 = alloca(8);
    v61 = alloca(1);
    v93 = alloca(8);
    v96 = alloca(8);
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
    v34 = *(int64_t*)v31;
    v35 = *(int64_t*)v22;
    v36 = v34 < v35;
    if (v36) goto bb5;
    else goto bb6;
  bb5:
    v38 = *(int64_t*)v31;
    v39 = p_add_i64(v1, v38);
    memset(v40, 0, 8);
    *(int64_t*)v40 = (int64_t)v39;
    v42 = *(int64_t*)v28;
    v43 = *(int64_t*)v31;
    v44 = p_add_i64(v42, v43);
    memset(v45, 0, 8);
    *(int64_t*)v45 = (int64_t)v44;
    v47 = *(int64_t*)v40;
    memset(v48, 0, 8);
    *(void**)v48 = (void*)v47;
    v50 = *(int64_t*)v45;
    memset(v51, 0, 8);
    *(void**)v51 = (void*)v50;
    v53 = 0LL;
    memset(v54, 0, 8);
    *(int64_t*)v54 = (int64_t)v53;
    v57 = *(void**)v48;
    v58 = *(uint8_t*)v57;
    *(int64_t*)v54 = (int64_t)v58;
    memset(v61, 0, 1);
    v62 = *(int64_t*)v54;
    v63 = 97LL;
    v64 = v62 >= v63;
    *(bool*)v61 = v64;
    if (v64) goto bb7;
    else goto bb8;
  bb6:
    v90 = *(int64_t*)v28;
    v91 = *(int64_t*)v22;
    v92 = p_add_i64(v90, v91);
    memset(v93, 0, 8);
    *(int64_t*)v93 = (int64_t)v92;
    v95 = *(int64_t*)v93;
    memset(v96, 0, 8);
    *(void**)v96 = (void*)v95;
    v99 = *(void**)v96;
    v100 = 0LL;
    *(uint8_t*)v99 = (uint8_t)v100;
    v103 = *(int64_t*)v28;
    return v103;
  bb7:
    v67 = *(int64_t*)v54;
    v68 = 122LL;
    v69 = v67 <= v68;
    *(bool*)v61 = v69;
    goto bb8;
  bb8:
    v72 = *(bool*)v61;
    if (v72) goto bb9;
    else goto bb10;
  bb9:
    v74 = *(int64_t*)v54;
    v75 = 32LL;
    v76 = p_sub_i64(v74, v75);
    *(int64_t*)v54 = (int64_t)v76;
    goto bb11;
  bb10:
    goto bb11;
  bb11:
    v81 = *(void**)v51;
    v82 = *(int64_t*)v54;
    *(uint8_t*)v81 = (uint8_t)v82;
    v85 = *(int64_t*)v31;
    v86 = 1LL;
    v87 = (int64_t)((uint64_t)v85 + (uint64_t)v86);
    *(int64_t*)v31 = (int64_t)v87;
    goto bb4;
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
    int64_t v34 = 0;
    int64_t v35 = 0;
    bool v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0;
    int64_t v39 = 0;
    void* v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0;
    void* v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0;
    void* v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0;
    void* v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    void* v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    void* v57 = 0;
    uint8_t v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0; /* placeholder for void */
    void* v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0;
    bool v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0;
    int64_t v68 = 0;
    bool v69 = 0;
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0; /* placeholder for void */
    bool v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    int64_t v74 = 0;
    int64_t v75 = 0;
    int64_t v76 = 0;
    int64_t v77 = 0; /* placeholder for void */
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0; /* placeholder for void */
    int64_t v80 = 0; /* placeholder for void */
    void* v81 = 0;
    int64_t v82 = 0;
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0; /* placeholder for void */
    int64_t v85 = 0;
    int64_t v86 = 0;
    int64_t v87 = 0;
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0;
    int64_t v91 = 0;
    int64_t v92 = 0;
    void* v93 = 0;
    int64_t v94 = 0; /* placeholder for void */
    int64_t v95 = 0;
    void* v96 = 0;
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0; /* placeholder for void */
    void* v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0; /* placeholder for void */
    int64_t v103 = 0;
    int64_t v104 = 0; /* placeholder for void */
    v7 = alloca(8);
    v10 = alloca(8);
    v22 = alloca(8);
    v28 = alloca(8);
    v31 = alloca(8);
    v40 = alloca(8);
    v45 = alloca(8);
    v48 = alloca(8);
    v51 = alloca(8);
    v54 = alloca(8);
    v61 = alloca(1);
    v93 = alloca(8);
    v96 = alloca(8);
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
    v34 = *(int64_t*)v31;
    v35 = *(int64_t*)v22;
    v36 = v34 < v35;
    if (v36) goto bb5;
    else goto bb6;
  bb5:
    v38 = *(int64_t*)v31;
    v39 = p_add_i64(v1, v38);
    memset(v40, 0, 8);
    *(int64_t*)v40 = (int64_t)v39;
    v42 = *(int64_t*)v28;
    v43 = *(int64_t*)v31;
    v44 = p_add_i64(v42, v43);
    memset(v45, 0, 8);
    *(int64_t*)v45 = (int64_t)v44;
    v47 = *(int64_t*)v40;
    memset(v48, 0, 8);
    *(void**)v48 = (void*)v47;
    v50 = *(int64_t*)v45;
    memset(v51, 0, 8);
    *(void**)v51 = (void*)v50;
    v53 = 0LL;
    memset(v54, 0, 8);
    *(int64_t*)v54 = (int64_t)v53;
    v57 = *(void**)v48;
    v58 = *(uint8_t*)v57;
    *(int64_t*)v54 = (int64_t)v58;
    memset(v61, 0, 1);
    v62 = *(int64_t*)v54;
    v63 = 65LL;
    v64 = v62 >= v63;
    *(bool*)v61 = v64;
    if (v64) goto bb7;
    else goto bb8;
  bb6:
    v90 = *(int64_t*)v28;
    v91 = *(int64_t*)v22;
    v92 = p_add_i64(v90, v91);
    memset(v93, 0, 8);
    *(int64_t*)v93 = (int64_t)v92;
    v95 = *(int64_t*)v93;
    memset(v96, 0, 8);
    *(void**)v96 = (void*)v95;
    v99 = *(void**)v96;
    v100 = 0LL;
    *(uint8_t*)v99 = (uint8_t)v100;
    v103 = *(int64_t*)v28;
    return v103;
  bb7:
    v67 = *(int64_t*)v54;
    v68 = 90LL;
    v69 = v67 <= v68;
    *(bool*)v61 = v69;
    goto bb8;
  bb8:
    v72 = *(bool*)v61;
    if (v72) goto bb9;
    else goto bb10;
  bb9:
    v74 = *(int64_t*)v54;
    v75 = 32LL;
    v76 = p_add_i64(v74, v75);
    *(int64_t*)v54 = (int64_t)v76;
    goto bb11;
  bb10:
    goto bb11;
  bb11:
    v81 = *(void**)v51;
    v82 = *(int64_t*)v54;
    *(uint8_t*)v81 = (uint8_t)v82;
    v85 = *(int64_t*)v31;
    v86 = 1LL;
    v87 = (int64_t)((uint64_t)v85 + (uint64_t)v86);
    *(int64_t*)v31 = (int64_t)v87;
    goto bb4;
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
    void* v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
    int64_t v59 = 0;
    int64_t v60 = 0;
    int64_t v61 = 0;
    void* v62 = 0;
    int64_t v63 = 0; /* placeholder for void */
    int64_t v64 = 0;
    void* v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0; /* placeholder for void */
    void* v68 = 0;
    int64_t v69 = 0;
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    v5 = alloca(8);
    v7 = alloca(8);
    v29 = alloca(8);
    v32 = alloca(8);
    v45 = alloca(8);
    v51 = alloca(8);
    v62 = alloca(8);
    v65 = alloca(8);
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
    v48 = 1LL;
    v49 = p_add_i64(v47, v48);
    v50 = p_malloc(v49);
    memset(v51, 0, 8);
    *(int64_t*)v51 = (int64_t)v50;
    v53 = *(int64_t*)v51;
    v54 = *(int64_t*)v5;
    v55 = p_add_i64(v1, v54);
    v56 = *(int64_t*)v45;
    v57 = 0LL;
    p_str_copy_n((void*)v57, v53, v55, v56);
    v59 = *(int64_t*)v51;
    v60 = *(int64_t*)v45;
    v61 = p_add_i64(v59, v60);
    memset(v62, 0, 8);
    *(int64_t*)v62 = (int64_t)v61;
    v64 = *(int64_t*)v62;
    memset(v65, 0, 8);
    *(void**)v65 = (void*)v64;
    v68 = *(void**)v65;
    v69 = 0LL;
    *(uint8_t*)v68 = (uint8_t)v69;
    v72 = *(int64_t*)v51;
    return v72;
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
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    bool v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    bool v49 = 0;
    void* v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    void* v53 = 0;
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0;
    int64_t v57 = 0;
    bool v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    int64_t v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0;
    void* v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0;
    int64_t v67 = 0;
    void* v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0;
    void* v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0;
    void* v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0;
    void* v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0;
    void* v80 = 0;
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0; /* placeholder for void */
    void* v83 = 0;
    uint8_t v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    void* v86 = 0;
    uint8_t v87 = 0;
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0;
    int64_t v91 = 0;
    bool v92 = 0;
    int64_t v93 = 0; /* placeholder for void */
    bool v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0;
    int64_t v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0; /* placeholder for void */
    bool v103 = 0;
    int64_t v104 = 0; /* placeholder for void */
    int64_t v105 = 0;
    int64_t v106 = 0; /* placeholder for void */
    int64_t v107 = 0; /* placeholder for void */
    int64_t v108 = 0;
    int64_t v109 = 0;
    int64_t v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    int64_t v112 = 0; /* placeholder for void */
    int64_t v113 = 0;
    int64_t v114 = 0; /* placeholder for void */
    v3 = alloca(1);
    v19 = alloca(8);
    v23 = alloca(8);
    v40 = alloca(8);
    v50 = alloca(1);
    v53 = alloca(8);
    v64 = alloca(8);
    v68 = alloca(8);
    v71 = alloca(8);
    v74 = alloca(8);
    v77 = alloca(8);
    v80 = alloca(8);
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
    v43 = *(int64_t*)v40;
    v44 = *(int64_t*)v19;
    v45 = *(int64_t*)v23;
    v46 = p_sub_i64(v44, v45);
    v47 = v43 <= v46;
    if (v47) goto bb13;
    else goto bb14;
  bb13:
    v49 = true;
    memset(v50, 0, 1);
    *(bool*)v50 = v49;
    v52 = 0LL;
    memset(v53, 0, 8);
    *(int64_t*)v53 = (int64_t)v52;
    goto bb15;
  bb14:
    v113 = -1LL;
    return v113;
  bb15:
    v56 = *(int64_t*)v53;
    v57 = *(int64_t*)v23;
    v58 = v56 < v57;
    if (v58) goto bb16;
    else goto bb17;
  bb16:
    v60 = *(int64_t*)v40;
    v61 = p_add_i64(v1, v60);
    v62 = *(int64_t*)v53;
    v63 = p_add_i64(v61, v62);
    memset(v64, 0, 8);
    *(int64_t*)v64 = (int64_t)v63;
    v66 = *(int64_t*)v53;
    v67 = p_add_i64(v2, v66);
    memset(v68, 0, 8);
    *(int64_t*)v68 = (int64_t)v67;
    v70 = *(int64_t*)v64;
    memset(v71, 0, 8);
    *(void**)v71 = (void*)v70;
    v73 = *(int64_t*)v68;
    memset(v74, 0, 8);
    *(void**)v74 = (void*)v73;
    v76 = 0LL;
    memset(v77, 0, 8);
    *(int64_t*)v77 = (int64_t)v76;
    v79 = 0LL;
    memset(v80, 0, 8);
    *(int64_t*)v80 = (int64_t)v79;
    v83 = *(void**)v71;
    v84 = *(uint8_t*)v83;
    *(int64_t*)v77 = (int64_t)v84;
    v86 = *(void**)v74;
    v87 = *(uint8_t*)v86;
    *(int64_t*)v80 = (int64_t)v87;
    v90 = *(int64_t*)v77;
    v91 = *(int64_t*)v80;
    v92 = v90 != v91;
    if (v92) goto bb18;
    else goto bb19;
  bb17:
    v103 = *(bool*)v50;
    if (v103) goto bb21;
    else goto bb22;
  bb18:
    v94 = false;
    *(bool*)v50 = v94;
    goto bb17;
  bb19:
    goto bb20;
  bb20:
    v98 = *(int64_t*)v53;
    v99 = 1LL;
    v100 = (int64_t)((uint64_t)v98 + (uint64_t)v99);
    *(int64_t*)v53 = (int64_t)v100;
    goto bb15;
  bb21:
    v105 = *(int64_t*)v40;
    return v105;
  bb22:
    goto bb23;
  bb23:
    v108 = *(int64_t*)v40;
    v109 = 1LL;
    v110 = p_add_i64(v108, v109);
    *(int64_t*)v40 = (int64_t)v110;
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
    int64_t v45 = 0;
    int64_t v46 = 0;
    bool v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    bool v49 = 0;
    void* v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    void* v53 = 0;
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0;
    int64_t v57 = 0;
    bool v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    int64_t v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0;
    void* v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0;
    int64_t v67 = 0;
    void* v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0;
    void* v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0;
    void* v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0;
    void* v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0;
    void* v80 = 0;
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0; /* placeholder for void */
    void* v83 = 0;
    uint8_t v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    void* v86 = 0;
    uint8_t v87 = 0;
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0;
    int64_t v91 = 0;
    bool v92 = 0;
    int64_t v93 = 0; /* placeholder for void */
    bool v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0;
    int64_t v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0; /* placeholder for void */
    bool v103 = 0;
    int64_t v104 = 0; /* placeholder for void */
    int64_t v105 = 0;
    int64_t v106 = 0; /* placeholder for void */
    int64_t v107 = 0; /* placeholder for void */
    int64_t v108 = 0;
    int64_t v109 = 0;
    int64_t v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    int64_t v112 = 0; /* placeholder for void */
    int64_t v113 = 0;
    int64_t v114 = 0; /* placeholder for void */
    v3 = alloca(1);
    v19 = alloca(8);
    v23 = alloca(8);
    v42 = alloca(8);
    v50 = alloca(1);
    v53 = alloca(8);
    v64 = alloca(8);
    v68 = alloca(8);
    v71 = alloca(8);
    v74 = alloca(8);
    v77 = alloca(8);
    v80 = alloca(8);
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
    v45 = *(int64_t*)v42;
    v46 = 0LL;
    v47 = v45 >= v46;
    if (v47) goto bb13;
    else goto bb14;
  bb13:
    v49 = true;
    memset(v50, 0, 1);
    *(bool*)v50 = v49;
    v52 = 0LL;
    memset(v53, 0, 8);
    *(int64_t*)v53 = (int64_t)v52;
    goto bb15;
  bb14:
    v113 = -1LL;
    return v113;
  bb15:
    v56 = *(int64_t*)v53;
    v57 = *(int64_t*)v23;
    v58 = v56 < v57;
    if (v58) goto bb16;
    else goto bb17;
  bb16:
    v60 = *(int64_t*)v42;
    v61 = p_add_i64(v1, v60);
    v62 = *(int64_t*)v53;
    v63 = p_add_i64(v61, v62);
    memset(v64, 0, 8);
    *(int64_t*)v64 = (int64_t)v63;
    v66 = *(int64_t*)v53;
    v67 = p_add_i64(v2, v66);
    memset(v68, 0, 8);
    *(int64_t*)v68 = (int64_t)v67;
    v70 = *(int64_t*)v64;
    memset(v71, 0, 8);
    *(void**)v71 = (void*)v70;
    v73 = *(int64_t*)v68;
    memset(v74, 0, 8);
    *(void**)v74 = (void*)v73;
    v76 = 0LL;
    memset(v77, 0, 8);
    *(int64_t*)v77 = (int64_t)v76;
    v79 = 0LL;
    memset(v80, 0, 8);
    *(int64_t*)v80 = (int64_t)v79;
    v83 = *(void**)v71;
    v84 = *(uint8_t*)v83;
    *(int64_t*)v77 = (int64_t)v84;
    v86 = *(void**)v74;
    v87 = *(uint8_t*)v86;
    *(int64_t*)v80 = (int64_t)v87;
    v90 = *(int64_t*)v77;
    v91 = *(int64_t*)v80;
    v92 = v90 != v91;
    if (v92) goto bb18;
    else goto bb19;
  bb17:
    v103 = *(bool*)v50;
    if (v103) goto bb21;
    else goto bb22;
  bb18:
    v94 = false;
    *(bool*)v50 = v94;
    goto bb17;
  bb19:
    goto bb20;
  bb20:
    v98 = *(int64_t*)v53;
    v99 = 1LL;
    v100 = (int64_t)((uint64_t)v98 + (uint64_t)v99);
    *(int64_t*)v53 = (int64_t)v100;
    goto bb15;
  bb21:
    v105 = *(int64_t*)v42;
    return v105;
  bb22:
    goto bb23;
  bb23:
    v108 = *(int64_t*)v42;
    v109 = 1LL;
    v110 = p_sub_i64(v108, v109);
    *(int64_t*)v42 = (int64_t)v110;
    goto bb12;
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
    int64_t v69 = 0;
    int64_t v70 = 0;
    bool v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    bool v73 = 0;
    void* v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0;
    void* v77 = 0;
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
    bool v88 = 0;
    int64_t v89 = 0; /* placeholder for void */
    bool v90 = 0;
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0; /* placeholder for void */
    int64_t v93 = 0; /* placeholder for void */
    int64_t v94 = 0;
    int64_t v95 = 0;
    int64_t v96 = 0;
    int64_t v97 = 0;
    void* v98 = 0;
    int64_t v99 = 0; /* placeholder for void */
    int64_t v100 = 0;
    int64_t v101 = 0;
    void* v102 = 0;
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0;
    void* v105 = 0;
    int64_t v106 = 0; /* placeholder for void */
    int64_t v107 = 0;
    void* v108 = 0;
    int64_t v109 = 0; /* placeholder for void */
    int64_t v110 = 0;
    void* v111 = 0;
    int64_t v112 = 0; /* placeholder for void */
    int64_t v113 = 0;
    void* v114 = 0;
    int64_t v115 = 0; /* placeholder for void */
    int64_t v116 = 0; /* placeholder for void */
    void* v117 = 0;
    uint8_t v118 = 0;
    int64_t v119 = 0; /* placeholder for void */
    void* v120 = 0;
    uint8_t v121 = 0;
    int64_t v122 = 0; /* placeholder for void */
    int64_t v123 = 0; /* placeholder for void */
    int64_t v124 = 0;
    int64_t v125 = 0;
    bool v126 = 0;
    int64_t v127 = 0; /* placeholder for void */
    bool v128 = 0;
    int64_t v129 = 0; /* placeholder for void */
    int64_t v130 = 0; /* placeholder for void */
    int64_t v131 = 0; /* placeholder for void */
    int64_t v132 = 0;
    int64_t v133 = 0;
    int64_t v134 = 0;
    int64_t v135 = 0; /* placeholder for void */
    int64_t v136 = 0; /* placeholder for void */
    bool v137 = 0;
    int64_t v138 = 0; /* placeholder for void */
    int64_t v139 = 0;
    int64_t v140 = 0;
    int64_t v141 = 0;
    int64_t v142 = 0; /* placeholder for void */
    int64_t v143 = 0;
    int64_t v144 = 0;
    int64_t v145 = 0;
    int64_t v146 = 0; /* placeholder for void */
    int64_t v147 = 0; /* placeholder for void */
    int64_t v148 = 0;
    int64_t v149 = 0;
    void* v150 = 0;
    int64_t v151 = 0; /* placeholder for void */
    int64_t v152 = 0;
    void* v153 = 0;
    int64_t v154 = 0; /* placeholder for void */
    int64_t v155 = 0;
    void* v156 = 0;
    int64_t v157 = 0; /* placeholder for void */
    int64_t v158 = 0; /* placeholder for void */
    void* v159 = 0;
    uint8_t v160 = 0;
    int64_t v161 = 0; /* placeholder for void */
    int64_t v162 = 0; /* placeholder for void */
    int64_t v163 = 0;
    void* v164 = 0;
    void* v165 = 0;
    int64_t v166 = 0; /* placeholder for void */
    int64_t v167 = 0;
    void* v168 = 0;
    int64_t v169 = 0; /* placeholder for void */
    int64_t v170 = 0; /* placeholder for void */
    void* v171 = 0;
    int64_t v172 = 0;
    int64_t v173 = 0; /* placeholder for void */
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
    int64_t v191 = 0;
    int64_t v192 = 0; /* placeholder for void */
    int64_t v193 = 0;
    int64_t v194 = 0; /* placeholder for void */
    int64_t v195 = 0;
    int64_t v196 = 0;
    int64_t v197 = 0;
    int64_t v198 = 0; /* placeholder for void */
    int64_t v199 = 0; /* placeholder for void */
    int64_t v200 = 0; /* placeholder for void */
    int64_t v201 = 0;
    int64_t v202 = 0; /* placeholder for void */
    v9 = alloca(8);
    v12 = alloca(8);
    v22 = alloca(1);
    v39 = alloca(8);
    v51 = alloca(8);
    v54 = alloca(8);
    v62 = alloca(8);
    v66 = alloca(8);
    v74 = alloca(1);
    v77 = alloca(8);
    v98 = alloca(8);
    v102 = alloca(8);
    v105 = alloca(8);
    v108 = alloca(8);
    v111 = alloca(8);
    v114 = alloca(8);
    v150 = alloca(8);
    v153 = alloca(8);
    v156 = alloca(8);
    v165 = alloca(8);
    v168 = alloca(8);
    v178 = alloca(8);
    v181 = alloca(8);
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
    v69 = *(int64_t*)v62;
    v70 = *(int64_t*)v66;
    v71 = v69 < v70;
    if (v71) goto bb13;
    else goto bb14;
  bb13:
    v73 = true;
    memset(v74, 0, 1);
    *(bool*)v74 = v73;
    v76 = 0LL;
    memset(v77, 0, 8);
    *(int64_t*)v77 = (int64_t)v76;
    goto bb15;
  bb14:
    v201 = *(int64_t*)v51;
    return v201;
  bb15:
    v80 = *(int64_t*)v77;
    v81 = *(int64_t*)v39;
    v82 = v80 < v81;
    if (v82) goto bb16;
    else goto bb17;
  bb16:
    v84 = *(int64_t*)v62;
    v85 = *(int64_t*)v77;
    v86 = p_add_i64(v84, v85);
    v87 = *(int64_t*)v66;
    v88 = v86 >= v87;
    if (v88) goto bb18;
    else goto bb19;
  bb17:
    v137 = *(bool*)v74;
    if (v137) goto bb24;
    else goto bb25;
  bb18:
    v90 = false;
    *(bool*)v74 = v90;
    goto bb17;
  bb19:
    goto bb20;
  bb20:
    v94 = *(int64_t*)v62;
    v95 = p_add_i64(v1, v94);
    v96 = *(int64_t*)v77;
    v97 = p_add_i64(v95, v96);
    memset(v98, 0, 8);
    *(int64_t*)v98 = (int64_t)v97;
    v100 = *(int64_t*)v77;
    v101 = p_add_i64(v2, v100);
    memset(v102, 0, 8);
    *(int64_t*)v102 = (int64_t)v101;
    v104 = *(int64_t*)v98;
    memset(v105, 0, 8);
    *(void**)v105 = (void*)v104;
    v107 = *(int64_t*)v102;
    memset(v108, 0, 8);
    *(void**)v108 = (void*)v107;
    v110 = 0LL;
    memset(v111, 0, 8);
    *(int64_t*)v111 = (int64_t)v110;
    v113 = 0LL;
    memset(v114, 0, 8);
    *(int64_t*)v114 = (int64_t)v113;
    v117 = *(void**)v105;
    v118 = *(uint8_t*)v117;
    *(int64_t*)v111 = (int64_t)v118;
    v120 = *(void**)v108;
    v121 = *(uint8_t*)v120;
    *(int64_t*)v114 = (int64_t)v121;
    v124 = *(int64_t*)v111;
    v125 = *(int64_t*)v114;
    v126 = v124 != v125;
    if (v126) goto bb21;
    else goto bb22;
  bb21:
    v128 = false;
    *(bool*)v74 = v128;
    goto bb17;
  bb22:
    goto bb23;
  bb23:
    v132 = *(int64_t*)v77;
    v133 = 1LL;
    v134 = (int64_t)((uint64_t)v132 + (uint64_t)v133);
    *(int64_t*)v77 = (int64_t)v134;
    goto bb15;
  bb24:
    v139 = *(int64_t*)v51;
    v140 = 0LL;
    v141 = (int64_t)String_push_str((void*)v140, v139, v3);
    *(int64_t*)v51 = (int64_t)v141;
    v143 = *(int64_t*)v62;
    v144 = *(int64_t*)v39;
    v145 = p_add_i64(v143, v144);
    *(int64_t*)v62 = (int64_t)v145;
    goto bb26;
  bb25:
    v148 = *(int64_t*)v62;
    v149 = p_add_i64(v1, v148);
    memset(v150, 0, 8);
    *(int64_t*)v150 = (int64_t)v149;
    v152 = *(int64_t*)v150;
    memset(v153, 0, 8);
    *(void**)v153 = (void*)v152;
    v155 = 0LL;
    memset(v156, 0, 8);
    *(int64_t*)v156 = (int64_t)v155;
    v159 = *(void**)v153;
    v160 = *(uint8_t*)v159;
    *(int64_t*)v156 = (int64_t)v160;
    v163 = 2LL;
    v164 = p_malloc(v163);
    memset(v165, 0, 8);
    *(int64_t*)v165 = (int64_t)v164;
    v167 = *(int64_t*)v165;
    memset(v168, 0, 8);
    *(void**)v168 = (void*)v167;
    v171 = *(void**)v168;
    v172 = *(int64_t*)v156;
    *(uint8_t*)v171 = (uint8_t)v172;
    v175 = *(int64_t*)v165;
    v176 = 1LL;
    v177 = p_add_i64(v175, v176);
    memset(v178, 0, 8);
    *(int64_t*)v178 = (int64_t)v177;
    v180 = *(int64_t*)v178;
    memset(v181, 0, 8);
    *(void**)v181 = (void*)v180;
    v184 = *(void**)v181;
    v185 = 0LL;
    *(uint8_t*)v184 = (uint8_t)v185;
    v188 = *(int64_t*)v51;
    v189 = *(int64_t*)v165;
    v190 = 0LL;
    v191 = (int64_t)String_push_str((void*)v190, v188, v189);
    *(int64_t*)v51 = (int64_t)v191;
    v193 = *(int64_t*)v165;
    p_free((void*)v193);
    v195 = *(int64_t*)v62;
    v196 = 1LL;
    v197 = p_add_i64(v195, v196);
    *(int64_t*)v62 = (int64_t)v197;
    goto bb26;
  bb26:
    goto bb12;
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
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0;
    bool v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    bool v49 = 0;
    void* v50 = 0;
    int64_t v51 = 0; /* placeholder for void */
    int64_t v52 = 0;
    void* v53 = 0;
    int64_t v54 = 0; /* placeholder for void */
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0;
    int64_t v57 = 0;
    bool v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    int64_t v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0;
    void* v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0;
    int64_t v67 = 0;
    void* v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0;
    void* v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0;
    void* v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0;
    void* v77 = 0;
    int64_t v78 = 0; /* placeholder for void */
    int64_t v79 = 0;
    void* v80 = 0;
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0; /* placeholder for void */
    void* v83 = 0;
    uint8_t v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    void* v86 = 0;
    uint8_t v87 = 0;
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0;
    int64_t v91 = 0;
    bool v92 = 0;
    int64_t v93 = 0; /* placeholder for void */
    bool v94 = 0;
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0;
    int64_t v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0; /* placeholder for void */
    bool v103 = 0;
    int64_t v104 = 0; /* placeholder for void */
    int64_t v105 = 0;
    int64_t v106 = 0;
    int64_t v107 = 0;
    void* v108 = 0;
    int64_t v109 = 0; /* placeholder for void */
    int64_t v110 = 0;
    int64_t v111 = 0;
    int64_t v112 = 0;
    void* v113 = 0;
    void* v114 = 0;
    int64_t v115 = 0; /* placeholder for void */
    int64_t v116 = 0;
    int64_t v117 = 0;
    int64_t v118 = 0;
    int64_t v119 = 0;
    int64_t v120 = 0;
    int64_t v121 = 0; /* placeholder for void */
    int64_t v122 = 0;
    int64_t v123 = 0;
    int64_t v124 = 0;
    void* v125 = 0;
    int64_t v126 = 0; /* placeholder for void */
    int64_t v127 = 0;
    void* v128 = 0;
    int64_t v129 = 0; /* placeholder for void */
    int64_t v130 = 0; /* placeholder for void */
    void* v131 = 0;
    int64_t v132 = 0;
    int64_t v133 = 0; /* placeholder for void */
    int64_t v134 = 0; /* placeholder for void */
    int64_t v135 = 0;
    int64_t v136 = 0;
    int64_t v137 = 0;
    int64_t v138 = 0; /* placeholder for void */
    int64_t v139 = 0;
    int64_t v140 = 0;
    int64_t v141 = 0;
    int64_t v142 = 0; /* placeholder for void */
    int64_t v143 = 0;
    int64_t v144 = 0; /* placeholder for void */
    int64_t v145 = 0; /* placeholder for void */
    int64_t v146 = 0;
    int64_t v147 = 0;
    int64_t v148 = 0;
    int64_t v149 = 0; /* placeholder for void */
    int64_t v150 = 0; /* placeholder for void */
    int64_t v151 = 0; /* placeholder for void */
    int64_t v152 = 0;
    int64_t v153 = 0;
    int64_t v154 = 0;
    void* v155 = 0;
    int64_t v156 = 0; /* placeholder for void */
    int64_t v157 = 0;
    int64_t v158 = 0;
    bool v159 = 0;
    int64_t v160 = 0; /* placeholder for void */
    int64_t v161 = 0;
    int64_t v162 = 0;
    int64_t v163 = 0;
    void* v164 = 0;
    void* v165 = 0;
    int64_t v166 = 0; /* placeholder for void */
    int64_t v167 = 0;
    int64_t v168 = 0;
    int64_t v169 = 0;
    int64_t v170 = 0;
    int64_t v171 = 0;
    int64_t v172 = 0; /* placeholder for void */
    int64_t v173 = 0;
    int64_t v174 = 0;
    int64_t v175 = 0;
    void* v176 = 0;
    int64_t v177 = 0; /* placeholder for void */
    int64_t v178 = 0;
    void* v179 = 0;
    int64_t v180 = 0; /* placeholder for void */
    int64_t v181 = 0; /* placeholder for void */
    void* v182 = 0;
    int64_t v183 = 0;
    int64_t v184 = 0; /* placeholder for void */
    int64_t v185 = 0; /* placeholder for void */
    int64_t v186 = 0;
    int64_t v187 = 0;
    int64_t v188 = 0;
    int64_t v189 = 0; /* placeholder for void */
    int64_t v190 = 0; /* placeholder for void */
    int64_t v191 = 0; /* placeholder for void */
    int64_t v192 = 0;
    int64_t v193 = 0; /* placeholder for void */
    v5 = alloca(8);
    v7 = alloca(1);
    v23 = alloca(8);
    v33 = alloca(8);
    v37 = alloca(8);
    v40 = alloca(8);
    v50 = alloca(1);
    v53 = alloca(8);
    v155 = alloca(8);
    v64 = alloca(8);
    v68 = alloca(8);
    v71 = alloca(8);
    v74 = alloca(8);
    v77 = alloca(8);
    v80 = alloca(8);
    v108 = alloca(8);
    v114 = alloca(8);
    v125 = alloca(8);
    v128 = alloca(8);
    v165 = alloca(8);
    v176 = alloca(8);
    v179 = alloca(8);
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
    v43 = *(int64_t*)v40;
    v44 = *(int64_t*)v37;
    v45 = *(int64_t*)v23;
    v46 = p_sub_i64(v44, v45);
    v47 = v43 <= v46;
    if (v47) goto bb10;
    else goto bb11;
  bb10:
    v49 = true;
    memset(v50, 0, 1);
    *(bool*)v50 = v49;
    v52 = 0LL;
    memset(v53, 0, 8);
    *(int64_t*)v53 = (int64_t)v52;
    goto bb12;
  bb11:
    v152 = *(int64_t*)v37;
    v153 = *(int64_t*)v33;
    v154 = p_sub_i64(v152, v153);
    memset(v155, 0, 8);
    *(int64_t*)v155 = (int64_t)v154;
    v157 = *(int64_t*)v155;
    v158 = 0LL;
    v159 = v157 >= v158;
    if (v159) goto bb21;
    else goto bb22;
  bb12:
    v56 = *(int64_t*)v53;
    v57 = *(int64_t*)v23;
    v58 = v56 < v57;
    if (v58) goto bb13;
    else goto bb14;
  bb13:
    v60 = *(int64_t*)v40;
    v61 = p_add_i64(v1, v60);
    v62 = *(int64_t*)v53;
    v63 = p_add_i64(v61, v62);
    memset(v64, 0, 8);
    *(int64_t*)v64 = (int64_t)v63;
    v66 = *(int64_t*)v53;
    v67 = p_add_i64(v2, v66);
    memset(v68, 0, 8);
    *(int64_t*)v68 = (int64_t)v67;
    v70 = *(int64_t*)v64;
    memset(v71, 0, 8);
    *(void**)v71 = (void*)v70;
    v73 = *(int64_t*)v68;
    memset(v74, 0, 8);
    *(void**)v74 = (void*)v73;
    v76 = 0LL;
    memset(v77, 0, 8);
    *(int64_t*)v77 = (int64_t)v76;
    v79 = 0LL;
    memset(v80, 0, 8);
    *(int64_t*)v80 = (int64_t)v79;
    v83 = *(void**)v71;
    v84 = *(uint8_t*)v83;
    *(int64_t*)v77 = (int64_t)v84;
    v86 = *(void**)v74;
    v87 = *(uint8_t*)v86;
    *(int64_t*)v80 = (int64_t)v87;
    v90 = *(int64_t*)v77;
    v91 = *(int64_t*)v80;
    v92 = v90 != v91;
    if (v92) goto bb15;
    else goto bb16;
  bb14:
    v103 = *(bool*)v50;
    if (v103) goto bb18;
    else goto bb19;
  bb15:
    v94 = false;
    *(bool*)v50 = v94;
    goto bb14;
  bb16:
    goto bb17;
  bb17:
    v98 = *(int64_t*)v53;
    v99 = 1LL;
    v100 = (int64_t)((uint64_t)v98 + (uint64_t)v99);
    *(int64_t*)v53 = (int64_t)v100;
    goto bb12;
  bb18:
    v105 = *(int64_t*)v40;
    v106 = *(int64_t*)v33;
    v107 = p_sub_i64(v105, v106);
    memset(v108, 0, 8);
    *(int64_t*)v108 = (int64_t)v107;
    v110 = *(int64_t*)v108;
    v111 = 1LL;
    v112 = p_add_i64(v110, v111);
    v113 = p_malloc(v112);
    memset(v114, 0, 8);
    *(int64_t*)v114 = (int64_t)v113;
    v116 = *(int64_t*)v114;
    v117 = *(int64_t*)v33;
    v118 = p_add_i64(v1, v117);
    v119 = *(int64_t*)v108;
    v120 = 0LL;
    p_str_copy_n((void*)v120, v116, v118, v119);
    v122 = *(int64_t*)v114;
    v123 = *(int64_t*)v108;
    v124 = p_add_i64(v122, v123);
    memset(v125, 0, 8);
    *(int64_t*)v125 = (int64_t)v124;
    v127 = *(int64_t*)v125;
    memset(v128, 0, 8);
    *(void**)v128 = (void*)v127;
    v131 = *(void**)v128;
    v132 = 0LL;
    *(uint8_t*)v131 = (uint8_t)v132;
    v135 = *(int64_t*)v5;
    v136 = *(int64_t*)v114;
    v137 = 0LL;
    Vec_push((void*)v137, v135, v136);
    v139 = *(int64_t*)v40;
    v140 = *(int64_t*)v23;
    v141 = p_add_i64(v139, v140);
    *(int64_t*)v33 = (int64_t)v141;
    v143 = *(int64_t*)v33;
    *(int64_t*)v40 = (int64_t)v143;
    goto bb20;
  bb19:
    v146 = *(int64_t*)v40;
    v147 = 1LL;
    v148 = p_add_i64(v146, v147);
    *(int64_t*)v40 = (int64_t)v148;
    goto bb20;
  bb20:
    goto bb9;
  bb21:
    v161 = *(int64_t*)v155;
    v162 = 1LL;
    v163 = p_add_i64(v161, v162);
    v164 = p_malloc(v163);
    memset(v165, 0, 8);
    *(int64_t*)v165 = (int64_t)v164;
    v167 = *(int64_t*)v165;
    v168 = *(int64_t*)v33;
    v169 = p_add_i64(v1, v168);
    v170 = *(int64_t*)v155;
    v171 = 0LL;
    p_str_copy_n((void*)v171, v167, v169, v170);
    v173 = *(int64_t*)v165;
    v174 = *(int64_t*)v155;
    v175 = p_add_i64(v173, v174);
    memset(v176, 0, 8);
    *(int64_t*)v176 = (int64_t)v175;
    v178 = *(int64_t*)v176;
    memset(v179, 0, 8);
    *(void**)v179 = (void*)v178;
    v182 = *(void**)v179;
    v183 = 0LL;
    *(uint8_t*)v182 = (uint8_t)v183;
    v186 = *(int64_t*)v5;
    v187 = *(int64_t*)v165;
    v188 = 0LL;
    Vec_push((void*)v188, v186, v187);
    goto bb23;
  bb22:
    goto bb23;
  bb23:
    v192 = *(int64_t*)v5;
    return v192;
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
    void* v25 = 0;
    void* v26 = 0;
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
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    int64_t v42 = 0;
    bool v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    bool v46 = 0;
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    int64_t v50 = 0;
    bool v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    bool v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0;
    int64_t v57 = 0;
    int64_t v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0;
    int64_t v61 = 0; /* placeholder for void */
    void* v62 = 0;
    uint8_t v63 = 0;
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0;
    int64_t v67 = 0;
    bool v68 = 0;
    int64_t v69 = 0; /* placeholder for void */
    int64_t v70 = 0;
    int64_t v71 = 0; /* placeholder for void */
    int64_t v72 = 0;
    int64_t v73 = 0;
    int64_t v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0;
    int64_t v77 = 0; /* placeholder for void */
    void* v78 = 0;
    uint8_t v79 = 0;
    int64_t v80 = 0; /* placeholder for void */
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0;
    int64_t v83 = 0;
    bool v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0;
    int64_t v87 = 0;
    int64_t v88 = 0;
    int64_t v89 = 0; /* placeholder for void */
    int64_t v90 = 0;
    int64_t v91 = 0; /* placeholder for void */
    void* v92 = 0;
    uint8_t v93 = 0;
    int64_t v94 = 0; /* placeholder for void */
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0; /* placeholder for void */
    void* v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0;
    bool v102 = 0;
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0; /* placeholder for void */
    int64_t v105 = 0;
    int64_t v106 = 0;
    bool v107 = 0;
    int64_t v108 = 0; /* placeholder for void */
    int64_t v109 = 0; /* placeholder for void */
    bool v110 = 0;
    int64_t v111 = 0; /* placeholder for void */
    int64_t v112 = 0;
    int64_t v113 = 0;
    int64_t v114 = 0;
    int64_t v115 = 0;
    int64_t v116 = 0;
    int64_t v117 = 0;
    int64_t v118 = 0;
    int64_t v119 = 0; /* placeholder for void */
    int64_t v120 = 0;
    int64_t v121 = 0;
    int64_t v122 = 0;
    int64_t v123 = 0; /* placeholder for void */
    int64_t v124 = 0;
    int64_t v125 = 0; /* placeholder for void */
    void* v126 = 0;
    uint8_t v127 = 0;
    int64_t v128 = 0; /* placeholder for void */
    int64_t v129 = 0; /* placeholder for void */
    int64_t v130 = 0; /* placeholder for void */
    int64_t v131 = 0;
    int64_t v132 = 0;
    int64_t v133 = 0;
    int64_t v134 = 0; /* placeholder for void */
    v9 = alloca(8);
    v12 = alloca(8);
    v14 = alloca(8);
    v17 = alloca(8);
    v22 = alloca(8);
    v25 = alloca(1);
    v26 = alloca(1);
    v27 = alloca(1);
    v99 = alloca(1);
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
    memset(v25, 0, 1);
    memset(v26, 0, 1);
    memset(v27, 0, 1);
    v28 = *(int64_t*)v22;
    v29 = 32LL;
    v30 = v28 == v29;
    *(bool*)v27 = v30;
    if (v30) goto bb8;
    else goto bb7;
  bb5:
    v56 = *(int64_t*)v14;
    v57 = 1LL;
    v58 = p_add_i64(v56, v57);
    *(int64_t*)v14 = (int64_t)v58;
    v60 = *(int64_t*)v14;
    *(void**)v17 = (void*)v60;
    v62 = *(void**)v17;
    v63 = *(uint8_t*)v62;
    *(int64_t*)v22 = (int64_t)v63;
    goto bb4;
  bb6:
    v66 = *(int64_t*)v22;
    v67 = 45LL;
    v68 = v66 == v67;
    if (v68) goto bb13;
    else goto bb14;
  bb7:
    v33 = *(int64_t*)v22;
    v34 = 9LL;
    v35 = v33 == v34;
    *(bool*)v27 = v35;
    goto bb8;
  bb8:
    v38 = *(bool*)v27;
    *(bool*)v26 = v38;
    if (v38) goto bb10;
    else goto bb9;
  bb9:
    v41 = *(int64_t*)v22;
    v42 = 10LL;
    v43 = v41 == v42;
    *(bool*)v26 = v43;
    goto bb10;
  bb10:
    v46 = *(bool*)v26;
    *(bool*)v25 = v46;
    if (v46) goto bb12;
    else goto bb11;
  bb11:
    v49 = *(int64_t*)v22;
    v50 = 13LL;
    v51 = v49 == v50;
    *(bool*)v25 = v51;
    goto bb12;
  bb12:
    v54 = *(bool*)v25;
    if (v54) goto bb5;
    else goto bb6;
  bb13:
    v70 = -1LL;
    *(int64_t*)v12 = (int64_t)v70;
    v72 = *(int64_t*)v14;
    v73 = 1LL;
    v74 = p_add_i64(v72, v73);
    *(int64_t*)v14 = (int64_t)v74;
    v76 = *(int64_t*)v14;
    *(void**)v17 = (void*)v76;
    v78 = *(void**)v17;
    v79 = *(uint8_t*)v78;
    *(int64_t*)v22 = (int64_t)v79;
    goto bb15;
  bb14:
    v82 = *(int64_t*)v22;
    v83 = 43LL;
    v84 = v82 == v83;
    if (v84) goto bb16;
    else goto bb17;
  bb15:
    goto bb19;
  bb16:
    v86 = *(int64_t*)v14;
    v87 = 1LL;
    v88 = p_add_i64(v86, v87);
    *(int64_t*)v14 = (int64_t)v88;
    v90 = *(int64_t*)v14;
    *(void**)v17 = (void*)v90;
    v92 = *(void**)v17;
    v93 = *(uint8_t*)v92;
    *(int64_t*)v22 = (int64_t)v93;
    goto bb18;
  bb17:
    goto bb18;
  bb18:
    goto bb15;
  bb19:
    memset(v99, 0, 1);
    v100 = *(int64_t*)v22;
    v101 = 48LL;
    v102 = v100 >= v101;
    *(bool*)v99 = v102;
    if (v102) goto bb22;
    else goto bb23;
  bb20:
    v112 = *(int64_t*)v9;
    v113 = 10LL;
    v114 = p_mul_i64(v112, v113);
    v115 = *(int64_t*)v22;
    v116 = 48LL;
    v117 = p_sub_i64(v115, v116);
    v118 = p_add_i64(v114, v117);
    *(int64_t*)v9 = (int64_t)v118;
    v120 = *(int64_t*)v14;
    v121 = 1LL;
    v122 = p_add_i64(v120, v121);
    *(int64_t*)v14 = (int64_t)v122;
    v124 = *(int64_t*)v14;
    *(void**)v17 = (void*)v124;
    v126 = *(void**)v17;
    v127 = *(uint8_t*)v126;
    *(int64_t*)v22 = (int64_t)v127;
    goto bb19;
  bb21:
    v131 = *(int64_t*)v9;
    v132 = *(int64_t*)v12;
    v133 = p_mul_i64(v131, v132);
    return v133;
  bb22:
    v105 = *(int64_t*)v22;
    v106 = 57LL;
    v107 = v105 <= v106;
    *(bool*)v99 = v107;
    goto bb23;
  bb23:
    v110 = *(bool*)v99;
    if (v110) goto bb20;
    else goto bb21;
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
    int64_t v44 = 0;
    bool v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0;
    int64_t v48 = 0;
    int64_t v49 = 0;
    int64_t v50 = 0;
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
    int64_t v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0;
    void* v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0;
    void* v67 = 0;
    int64_t v68 = 0; /* placeholder for void */
    int64_t v69 = 0; /* placeholder for void */
    void* v70 = 0;
    int64_t v71 = 0;
    int64_t v72 = 0; /* placeholder for void */
    int64_t v73 = 0; /* placeholder for void */
    int64_t v74 = 0;
    int64_t v75 = 0; /* placeholder for void */
    v3 = alloca(1);
    v16 = alloca(8);
    v19 = alloca(8);
    v31 = alloca(8);
    v38 = alloca(8);
    v41 = alloca(8);
    v64 = alloca(8);
    v67 = alloca(8);
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
    v44 = *(int64_t*)v41;
    v45 = v44 < v2;
    if (v45) goto bb7;
    else goto bb8;
  bb7:
    v47 = *(int64_t*)v38;
    v48 = *(int64_t*)v41;
    v49 = *(int64_t*)v31;
    v50 = p_mul_i64(v48, v49);
    v51 = p_add_i64(v47, v50);
    v52 = *(int64_t*)v31;
    v53 = 0LL;
    p_str_copy_n((void*)v53, v51, v1, v52);
    v55 = *(int64_t*)v41;
    v56 = 1LL;
    v57 = (int64_t)((uint64_t)v55 + (uint64_t)v56);
    *(int64_t*)v41 = (int64_t)v57;
    goto bb6;
  bb8:
    v60 = *(int64_t*)v38;
    v61 = *(int64_t*)v31;
    v62 = p_mul_i64(v61, v2);
    v63 = p_add_i64(v60, v62);
    memset(v64, 0, 8);
    *(int64_t*)v64 = (int64_t)v63;
    v66 = *(int64_t*)v64;
    memset(v67, 0, 8);
    *(void**)v67 = (void*)v66;
    v70 = *(void**)v67;
    v71 = 0LL;
    *(uint8_t*)v70 = (uint8_t)v71;
    v74 = *(int64_t*)v38;
    return v74;
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

int64_t String_clone(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_from((void*)v2, v1);
    return v3;
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

int64_t String_char_count(void* v0, int64_t v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    return v3;
    return 0;
}

void* char_to_str(void* v0, int64_t v1) {
    void* v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = p_char_to_str(v1);
    return v2;
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
    int64_t v14 = 0;
    int64_t v15 = 0;
    bool v16 = 0;
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0;
    void* v19 = 0;
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
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0; /* placeholder for void */
    void* v33 = 0;
    int64_t v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0; /* placeholder for void */
    void* v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    v4 = alloca(8);
    v8 = alloca(8);
    v11 = alloca(8);
    v20 = alloca(8);
    v23 = alloca(8);
    v26 = alloca(8);
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
    v14 = *(int64_t*)v11;
    v15 = *(int64_t*)v8;
    v16 = v14 < v15;
    if (v16) goto bb2;
    else goto bb3;
  bb2:
    v18 = *(int64_t*)v11;
    v19 = (void*)(intptr_t)((int64_t)v1 + v18);
    memset(v20, 0, 8);
    *(int64_t*)v20 = (int64_t)v19;
    v22 = *(int64_t*)v20;
    memset(v23, 0, 8);
    *(void**)v23 = (void*)v22;
    v25 = 0LL;
    memset(v26, 0, 8);
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
    v39 = (int64_t)((uint64_t)v37 + (uint64_t)v38);
    *(int64_t*)v11 = (int64_t)v39;
    goto bb1;
  bb3:
    v42 = *(void**)v4;
    return v42;
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
    int64_t v13 = 0;
    int64_t v14 = 0;
    bool v15 = 0;
    int64_t v16 = 0; /* placeholder for void */
    void* v17 = 0;
    int64_t v18 = 0;
    int64_t v19 = 0;
    int64_t v20 = 0;
    void* v21 = 0;
    void* v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0;
    int64_t v25 = 0;
    int64_t v26 = 0;
    int64_t v27 = 0; /* placeholder for void */
    int64_t v28 = 0; /* placeholder for void */
    void* v29 = 0;
    int64_t v30 = 0; /* placeholder for void */
    v3 = alloca(8);
    v6 = alloca(8);
    v10 = alloca(8);
    v2 = _str3;
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
    v26 = (int64_t)((uint64_t)v24 + (uint64_t)v25);
    *(int64_t*)v6 = (int64_t)v26;
    goto bb1;
  bb3:
    v29 = *(void**)v3;
    return v29;
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
    v32 = _str3;
    return v32;
  bb8:
    goto bb9;
  bb9:
    v35 = *(int64_t*)v8;
    v36 = *(int64_t*)v10;
    v37 = 0LL;
    v38 = (int64_t)String_slice((void*)v37, (int64_t)v1, v35, v36);
    return (void*)(intptr_t)v38;
    return 0;
}

void* str_slice_len(void* v0, void* v1, int64_t v2, int64_t v3, int64_t v4) {
    int64_t v5 = 0;
    int64_t v6 = 0;
    int64_t v7 = 0; /* placeholder for void */
    v5 = 0LL;
    v6 = (int64_t)String_slice_len((void*)v5, (int64_t)v1, v2, v3, v4);
    return (void*)(intptr_t)v6;
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

bool str_ends_with(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (bool)String_ends_with((void*)v3, (int64_t)v1, (int64_t)v2);
    return v4;
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

int64_t str_find(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)String_find((void*)v3, (int64_t)v1, (int64_t)v2);
    return v4;
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

void* str_trim_start(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    void* v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    int64_t v6 = 0;
    void* v7 = 0;
    int64_t v8 = 0; /* placeholder for void */
    int64_t v9 = 0; /* placeholder for void */
    int64_t v10 = 0;
    int64_t v11 = 0;
    bool v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    void* v15 = 0;
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
    int64_t v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0; /* placeholder for void */
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0;
    int64_t v37 = 0;
    int64_t v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0; /* placeholder for void */
    int64_t v41 = 0;
    int64_t v42 = 0;
    bool v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    void* v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0;
    int64_t v49 = 0;
    int64_t v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    v4 = alloca(8);
    v7 = alloca(8);
    v16 = alloca(8);
    v19 = alloca(8);
    v22 = alloca(8);
    v2 = 0LL;
    v3 = (int64_t)String_len((void*)v2, (int64_t)v1);
    memset(v4, 0, 8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = 0LL;
    memset(v7, 0, 8);
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
    v15 = (void*)(intptr_t)((int64_t)v1 + v14);
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
    v38 = (int64_t)((uint64_t)v36 + (uint64_t)v37);
    *(int64_t*)v7 = (int64_t)v38;
    goto bb1;
  bb7:
    v45 = _str3;
    return v45;
  bb8:
    goto bb9;
  bb9:
    v48 = *(int64_t*)v7;
    v49 = *(int64_t*)v4;
    v50 = 0LL;
    v51 = (int64_t)String_slice((void*)v50, (int64_t)v1, v48, v49);
    return (void*)(intptr_t)v51;
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
    int64_t v10 = 0;
    int64_t v11 = 0;
    bool v12 = 0;
    int64_t v13 = 0; /* placeholder for void */
    int64_t v14 = 0;
    void* v15 = 0;
    int64_t v16 = 0;
    void* v17 = 0;
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
    uint8_t v28 = 0;
    int64_t v29 = 0; /* placeholder for void */
    int64_t v30 = 0; /* placeholder for void */
    int64_t v31 = 0;
    int64_t v32 = 0;
    bool v33 = 0;
    int64_t v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0;
    int64_t v39 = 0;
    int64_t v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0; /* placeholder for void */
    int64_t v43 = 0;
    int64_t v44 = 0;
    bool v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    void* v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0; /* placeholder for void */
    v4 = alloca(8);
    v7 = alloca(8);
    v18 = alloca(8);
    v21 = alloca(8);
    v24 = alloca(8);
    v2 = 0LL;
    v3 = (int64_t)String_len((void*)v2, (int64_t)v1);
    memset(v4, 0, 8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    memset(v7, 0, 8);
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
    v15 = (void*)(intptr_t)((int64_t)v1 + v14);
    v16 = 1LL;
    v17 = (void*)(intptr_t)((int64_t)v15 - v16);
    memset(v18, 0, 8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = *(int64_t*)v18;
    memset(v21, 0, 8);
    *(void**)v21 = (void*)v20;
    v23 = 0LL;
    memset(v24, 0, 8);
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
    v40 = p_sub_i64(v38, v39);
    *(int64_t*)v7 = (int64_t)v40;
    goto bb1;
  bb7:
    v47 = _str3;
    return v47;
  bb8:
    goto bb9;
  bb9:
    v50 = 0LL;
    v51 = *(int64_t*)v7;
    v52 = 0LL;
    v53 = (int64_t)String_slice((void*)v52, (int64_t)v1, v50, v51);
    return (void*)(intptr_t)v53;
    return 0;
}

void* str_trim(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_trim((void*)v2, (int64_t)v1);
    return (void*)(intptr_t)v3;
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
    int64_t v17 = 0;
    int64_t v18 = 0;
    bool v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    int64_t v22 = 0;
    int64_t v23 = 0;
    int64_t v24 = 0;
    bool v25 = 0;
    int64_t v26 = 0; /* placeholder for void */
    void* v27 = 0;
    int64_t v28 = 0;
    int64_t v29 = 0;
    int64_t v30 = 0;
    int64_t v31 = 0;
    int64_t v32 = 0;
    int64_t v33 = 0; /* placeholder for void */
    int64_t v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0;
    int64_t v37 = 0; /* placeholder for void */
    int64_t v38 = 0; /* placeholder for void */
    int64_t v39 = 0; /* placeholder for void */
    int64_t v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0;
    int64_t v46 = 0;
    bool v47 = 0;
    int64_t v48 = 0; /* placeholder for void */
    void* v49 = 0;
    int64_t v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0;
    int64_t v53 = 0;
    int64_t v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    void* v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
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
    v17 = *(int64_t*)v7;
    v18 = *(int64_t*)v14;
    v19 = v17 < v18;
    if (v19) goto bb2;
    else goto bb3;
  bb2:
    v21 = *(int64_t*)v7;
    v22 = 0LL;
    v23 = (int64_t)str_byte_at((void*)v22, v1, v21);
    v24 = 10LL;
    v25 = v23 == v24;
    if (v25) goto bb4;
    else goto bb5;
  bb3:
    v45 = *(int64_t*)v10;
    v46 = *(int64_t*)v14;
    v47 = v45 < v46;
    if (v47) goto bb7;
    else goto bb8;
  bb4:
    v27 = *(void**)v4;
    v28 = *(int64_t*)v10;
    v29 = *(int64_t*)v7;
    v30 = 0LL;
    v31 = (int64_t)String_slice((void*)v30, (int64_t)v1, v28, v29);
    v32 = 0LL;
    Vec_push((void*)v32, (int64_t)v27, v31);
    v34 = *(int64_t*)v7;
    v35 = 1LL;
    v36 = p_add_i64(v34, v35);
    *(int64_t*)v10 = (int64_t)v36;
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v40 = *(int64_t*)v7;
    v41 = 1LL;
    v42 = (int64_t)((uint64_t)v40 + (uint64_t)v41);
    *(int64_t*)v7 = (int64_t)v42;
    goto bb1;
  bb7:
    v49 = *(void**)v4;
    v50 = *(int64_t*)v10;
    v51 = *(int64_t*)v14;
    v52 = 0LL;
    v53 = (int64_t)String_slice((void*)v52, (int64_t)v1, v50, v51);
    v54 = 0LL;
    Vec_push((void*)v54, (int64_t)v49, v53);
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    v58 = *(void**)v4;
    return v58;
    return 0;
}

void* str_trim_end_char(void* v0, void* v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    void* v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    int64_t v7 = 0; /* placeholder for void */
    int64_t v8 = 0;
    int64_t v9 = 0;
    bool v10 = 0;
    int64_t v11 = 0; /* placeholder for void */
    int64_t v12 = 0;
    int64_t v13 = 0;
    int64_t v14 = 0;
    int64_t v15 = 0;
    int64_t v16 = 0;
    void* v17 = 0;
    int64_t v18 = 0; /* placeholder for void */
    void* v19 = 0;
    void* v20 = 0;
    int64_t v21 = 0;
    bool v22 = 0;
    int64_t v23 = 0; /* placeholder for void */
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0;
    int64_t v26 = 0;
    bool v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0; /* placeholder for void */
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
    int64_t v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0; /* placeholder for void */
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0;
    int64_t v48 = 0;
    bool v49 = 0;
    int64_t v50 = 0; /* placeholder for void */
    void* v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    int64_t v55 = 0;
    int64_t v56 = 0;
    int64_t v57 = 0;
    int64_t v58 = 0; /* placeholder for void */
    v5 = alloca(8);
    v17 = alloca(8);
    v19 = alloca(1);
    v20 = alloca(1);
    v3 = 0LL;
    v4 = (int64_t)String_len((void*)v3, (int64_t)v1);
    memset(v5, 0, 8);
    *(int64_t*)v5 = (int64_t)v4;
    goto bb1;
  bb1:
    v8 = *(int64_t*)v5;
    v9 = 0LL;
    v10 = v8 > v9;
    if (v10) goto bb2;
    else goto bb3;
  bb2:
    v12 = *(int64_t*)v5;
    v13 = 1LL;
    v14 = p_sub_i64(v12, v13);
    v15 = 0LL;
    v16 = (int64_t)str_byte_at((void*)v15, v1, v14);
    memset(v17, 0, 8);
    *(int64_t*)v17 = (int64_t)v16;
    memset(v19, 0, 1);
    memset(v20, 0, 1);
    v21 = *(int64_t*)v17;
    v22 = v21 == v2;
    *(bool*)v20 = v22;
    if (v22) goto bb5;
    else goto bb4;
  bb3:
    v47 = *(int64_t*)v5;
    v48 = 0LL;
    v49 = v47 <= v48;
    if (v49) goto bb11;
    else goto bb12;
  bb4:
    v25 = *(int64_t*)v17;
    v26 = 32LL;
    v27 = v25 == v26;
    *(bool*)v20 = v27;
    goto bb5;
  bb5:
    v30 = *(bool*)v20;
    *(bool*)v19 = v30;
    if (v30) goto bb7;
    else goto bb6;
  bb6:
    v33 = *(int64_t*)v17;
    v34 = 9LL;
    v35 = v33 == v34;
    *(bool*)v19 = v35;
    goto bb7;
  bb7:
    v38 = *(bool*)v19;
    if (v38) goto bb8;
    else goto bb9;
  bb8:
    v40 = *(int64_t*)v5;
    v41 = 1LL;
    v42 = p_sub_i64(v40, v41);
    *(int64_t*)v5 = (int64_t)v42;
    goto bb10;
  bb9:
    goto bb3;
  bb10:
    goto bb1;
  bb11:
    v51 = _str3;
    return v51;
  bb12:
    goto bb13;
  bb13:
    v54 = 0LL;
    v55 = *(int64_t*)v5;
    v56 = 0LL;
    v57 = (int64_t)String_slice((void*)v56, (int64_t)v1, v54, v55);
    return (void*)(intptr_t)v57;
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
    int64_t v16 = 0; /* placeholder for void */
    int64_t v17 = 0; /* placeholder for void */
    int64_t v18 = 0;
    void* v19 = 0;
    int64_t v20 = 0; /* placeholder for void */
    int64_t v21 = 0;
    int64_t v22 = 0;
    void* v23 = 0;
    int64_t v24 = 0; /* placeholder for void */
    int64_t v25 = 0;
    int64_t v26 = 0;
    void* v27 = 0;
    int64_t v28 = 0; /* placeholder for void */
    int64_t v29 = 0;
    void* v30 = 0;
    int64_t v31 = 0; /* placeholder for void */
    int64_t v32 = 0; /* placeholder for void */
    int64_t v33 = 0;
    int64_t v34 = 0;
    int64_t v35 = 0;
    int64_t v36 = 0;
    bool v37 = 0;
    int64_t v38 = 0; /* placeholder for void */
    bool v39 = 0;
    void* v40 = 0;
    int64_t v41 = 0; /* placeholder for void */
    int64_t v42 = 0;
    void* v43 = 0;
    int64_t v44 = 0; /* placeholder for void */
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    int64_t v47 = 0;
    bool v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0;
    void* v51 = 0;
    int64_t v52 = 0;
    void* v53 = 0;
    void* v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0;
    void* v57 = 0;
    void* v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
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
    bool v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0; /* placeholder for void */
    int64_t v87 = 0; /* placeholder for void */
    int64_t v88 = 0;
    int64_t v89 = 0;
    int64_t v90 = 0;
    int64_t v91 = 0; /* placeholder for void */
    int64_t v92 = 0; /* placeholder for void */
    bool v93 = 0;
    int64_t v94 = 0; /* placeholder for void */
    void* v95 = 0;
    int64_t v96 = 0;
    int64_t v97 = 0;
    int64_t v98 = 0;
    int64_t v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0; /* placeholder for void */
    int64_t v102 = 0;
    int64_t v103 = 0;
    int64_t v104 = 0;
    int64_t v105 = 0; /* placeholder for void */
    int64_t v106 = 0;
    int64_t v107 = 0; /* placeholder for void */
    int64_t v108 = 0; /* placeholder for void */
    int64_t v109 = 0;
    int64_t v110 = 0;
    int64_t v111 = 0;
    int64_t v112 = 0; /* placeholder for void */
    int64_t v113 = 0; /* placeholder for void */
    int64_t v114 = 0; /* placeholder for void */
    void* v115 = 0;
    int64_t v116 = 0;
    int64_t v117 = 0;
    int64_t v118 = 0;
    int64_t v119 = 0;
    int64_t v120 = 0;
    int64_t v121 = 0; /* placeholder for void */
    void* v122 = 0;
    int64_t v123 = 0; /* placeholder for void */
    v5 = alloca(8);
    v19 = alloca(8);
    v23 = alloca(8);
    v27 = alloca(8);
    v30 = alloca(8);
    v40 = alloca(1);
    v43 = alloca(8);
    v54 = alloca(8);
    v58 = alloca(8);
    v61 = alloca(8);
    v64 = alloca(8);
    v67 = alloca(8);
    v70 = alloca(8);
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
    return v15;
  bb2:
    goto bb3;
  bb3:
    v18 = 0LL;
    memset(v19, 0, 8);
    *(int64_t*)v19 = (int64_t)v18;
    v21 = 0LL;
    v22 = (int64_t)String_len((void*)v21, (int64_t)v1);
    memset(v23, 0, 8);
    *(int64_t*)v23 = (int64_t)v22;
    v25 = 0LL;
    v26 = (int64_t)String_len((void*)v25, (int64_t)v2);
    memset(v27, 0, 8);
    *(int64_t*)v27 = (int64_t)v26;
    v29 = 0LL;
    memset(v30, 0, 8);
    *(int64_t*)v30 = (int64_t)v29;
    goto bb4;
  bb4:
    v33 = *(int64_t*)v30;
    v34 = *(int64_t*)v23;
    v35 = *(int64_t*)v27;
    v36 = p_sub_i64(v34, v35);
    v37 = v33 <= v36;
    if (v37) goto bb5;
    else goto bb6;
  bb5:
    v39 = true;
    memset(v40, 0, 1);
    *(bool*)v40 = v39;
    v42 = 0LL;
    memset(v43, 0, 8);
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
    return v122;
  bb7:
    v46 = *(int64_t*)v43;
    v47 = *(int64_t*)v27;
    v48 = v46 < v47;
    if (v48) goto bb8;
    else goto bb9;
  bb8:
    v50 = *(int64_t*)v30;
    v51 = (void*)(intptr_t)((int64_t)v1 + v50);
    v52 = *(int64_t*)v43;
    v53 = (void*)(intptr_t)((int64_t)v51 + v52);
    memset(v54, 0, 8);
    *(int64_t*)v54 = (int64_t)v53;
    v56 = *(int64_t*)v43;
    v57 = (void*)(intptr_t)((int64_t)v2 + v56);
    memset(v58, 0, 8);
    *(int64_t*)v58 = (int64_t)v57;
    v60 = *(int64_t*)v54;
    memset(v61, 0, 8);
    *(void**)v61 = (void*)v60;
    v63 = *(int64_t*)v58;
    memset(v64, 0, 8);
    *(void**)v64 = (void*)v63;
    v66 = 0LL;
    memset(v67, 0, 8);
    *(int64_t*)v67 = (int64_t)v66;
    v69 = 0LL;
    memset(v70, 0, 8);
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
    v90 = (int64_t)((uint64_t)v88 + (uint64_t)v89);
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
    v104 = p_add_i64(v102, v103);
    *(int64_t*)v19 = (int64_t)v104;
    v106 = *(int64_t*)v19;
    *(int64_t*)v30 = (int64_t)v106;
    goto bb15;
  bb14:
    v109 = *(int64_t*)v30;
    v110 = 1LL;
    v111 = p_add_i64(v109, v110);
    *(int64_t*)v30 = (int64_t)v111;
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
    int64_t v18 = 0;
    int64_t v19 = 0;
    bool v20 = 0;
    int64_t v21 = 0; /* placeholder for void */
    int64_t v22 = 0;
    void* v23 = 0;
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
    uint8_t v34 = 0;
    int64_t v35 = 0; /* placeholder for void */
    int64_t v36 = 0; /* placeholder for void */
    int64_t v37 = 0;
    bool v38 = 0;
    int64_t v39 = 0; /* placeholder for void */
    void* v40 = 0;
    int64_t v41 = 0;
    int64_t v42 = 0;
    int64_t v43 = 0;
    int64_t v44 = 0;
    int64_t v45 = 0;
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
    int64_t v56 = 0; /* placeholder for void */
    int64_t v57 = 0; /* placeholder for void */
    void* v58 = 0;
    int64_t v59 = 0;
    int64_t v60 = 0;
    int64_t v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0;
    int64_t v64 = 0; /* placeholder for void */
    void* v65 = 0;
    int64_t v66 = 0; /* placeholder for void */
    v5 = alloca(8);
    v8 = alloca(8);
    v12 = alloca(8);
    v15 = alloca(8);
    v24 = alloca(8);
    v27 = alloca(8);
    v30 = alloca(8);
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
    v18 = *(int64_t*)v15;
    v19 = *(int64_t*)v12;
    v20 = v18 < v19;
    if (v20) goto bb2;
    else goto bb3;
  bb2:
    v22 = *(int64_t*)v15;
    v23 = (void*)(intptr_t)((int64_t)v1 + v22);
    memset(v24, 0, 8);
    *(int64_t*)v24 = (int64_t)v23;
    v26 = *(int64_t*)v24;
    memset(v27, 0, 8);
    *(void**)v27 = (void*)v26;
    v29 = 0LL;
    memset(v30, 0, 8);
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
    v49 = p_add_i64(v47, v48);
    *(int64_t*)v8 = (int64_t)v49;
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v53 = *(int64_t*)v15;
    v54 = 1LL;
    v55 = (int64_t)((uint64_t)v53 + (uint64_t)v54);
    *(int64_t*)v15 = (int64_t)v55;
    goto bb1;
    return 0;
}

void* str_replace(void* v0, void* v1, void* v2, void* v3) {
    int64_t v4 = 0;
    int64_t v5 = 0;
    int64_t v6 = 0; /* placeholder for void */
    v4 = 0LL;
    v5 = (int64_t)String_replace((void*)v4, (int64_t)v1, (int64_t)v2, (int64_t)v3);
    return (void*)(intptr_t)v5;
    return 0;
}

void* str_repeat(void* v0, void* v1, int64_t v2) {
    int64_t v3 = 0;
    int64_t v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)String_repeat((void*)v3, (int64_t)v1, v2);
    return (void*)(intptr_t)v4;
    return 0;
}

void* str_to_upper(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_to_uppercase((void*)v2, (int64_t)v1);
    return (void*)(intptr_t)v3;
    return 0;
}

void* str_to_lower(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_to_lowercase((void*)v2, (int64_t)v1);
    return (void*)(intptr_t)v3;
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
    bool v44 = 0;
    int64_t v45 = 0;
    int64_t v46 = 0; /* placeholder for void */
    int64_t v47 = 0; /* placeholder for void */
    int64_t v48 = 0; /* placeholder for void */
    int64_t v49 = 0;
    int64_t v50 = 0;
    int64_t v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0; /* placeholder for void */
    int64_t v54 = 0;
    int64_t v55 = 0;
    bool v56 = 0;
    int64_t v57 = 0; /* placeholder for void */
    int64_t v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0; /* placeholder for void */
    int64_t v61 = 0;
    void* v62 = 0;
    void* v63 = 0;
    int64_t v64 = 0; /* placeholder for void */
    int64_t v65 = 0;
    void* v66 = 0;
    int64_t v67 = 0; /* placeholder for void */
    int64_t v68 = 0;
    void* v69 = 0;
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0; /* placeholder for void */
    void* v72 = 0;
    uint8_t v73 = 0;
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0;
    int64_t v77 = 0;
    bool v78 = 0;
    int64_t v79 = 0; /* placeholder for void */
    bool v80 = 0;
    int64_t v81 = 0; /* placeholder for void */
    int64_t v82 = 0;
    int64_t v83 = 0;
    int64_t v84 = 0;
    int64_t v85 = 0; /* placeholder for void */
    int64_t v86 = 0; /* placeholder for void */
    int64_t v87 = 0;
    int64_t v88 = 0;
    bool v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    int64_t v91 = 0;
    int64_t v92 = 0;
    int64_t v93 = 0;
    int64_t v94 = 0; /* placeholder for void */
    int64_t v95 = 0; /* placeholder for void */
    int64_t v96 = 0; /* placeholder for void */
    int64_t v97 = 0; /* placeholder for void */
    int64_t v98 = 0;
    int64_t v99 = 0;
    int64_t v100 = 0;
    int64_t v101 = 0;
    bool v102 = 0;
    int64_t v103 = 0; /* placeholder for void */
    int64_t v104 = 0;
    void* v105 = 0;
    void* v106 = 0;
    int64_t v107 = 0; /* placeholder for void */
    int64_t v108 = 0;
    void* v109 = 0;
    int64_t v110 = 0; /* placeholder for void */
    int64_t v111 = 0;
    void* v112 = 0;
    int64_t v113 = 0; /* placeholder for void */
    int64_t v114 = 0; /* placeholder for void */
    void* v115 = 0;
    uint8_t v116 = 0;
    int64_t v117 = 0; /* placeholder for void */
    int64_t v118 = 0; /* placeholder for void */
    int64_t v119 = 0;
    void* v120 = 0;
    int64_t v121 = 0;
    void* v122 = 0;
    void* v123 = 0;
    int64_t v124 = 0; /* placeholder for void */
    int64_t v125 = 0;
    void* v126 = 0;
    int64_t v127 = 0; /* placeholder for void */
    int64_t v128 = 0;
    void* v129 = 0;
    int64_t v130 = 0; /* placeholder for void */
    int64_t v131 = 0; /* placeholder for void */
    void* v132 = 0;
    uint8_t v133 = 0;
    int64_t v134 = 0; /* placeholder for void */
    int64_t v135 = 0; /* placeholder for void */
    void* v136 = 0;
    int64_t v137 = 0;
    int64_t v138 = 0;
    bool v139 = 0;
    int64_t v140 = 0; /* placeholder for void */
    int64_t v141 = 0; /* placeholder for void */
    void* v142 = 0;
    int64_t v143 = 0;
    int64_t v144 = 0;
    bool v145 = 0;
    int64_t v146 = 0; /* placeholder for void */
    int64_t v147 = 0; /* placeholder for void */
    int64_t v148 = 0;
    int64_t v149 = 0;
    bool v150 = 0;
    int64_t v151 = 0; /* placeholder for void */
    int64_t v152 = 0; /* placeholder for void */
    bool v153 = 0;
    int64_t v154 = 0; /* placeholder for void */
    int64_t v155 = 0; /* placeholder for void */
    bool v156 = 0;
    int64_t v157 = 0; /* placeholder for void */
    int64_t v158 = 0;
    int64_t v159 = 0;
    int64_t v160 = 0;
    int64_t v161 = 0; /* placeholder for void */
    int64_t v162 = 0; /* placeholder for void */
    int64_t v163 = 0; /* placeholder for void */
    int64_t v164 = 0; /* placeholder for void */
    int64_t v165 = 0; /* placeholder for void */
    int64_t v166 = 0; /* placeholder for void */
    int64_t v167 = 0;
    int64_t v168 = 0;
    bool v169 = 0;
    int64_t v170 = 0; /* placeholder for void */
    int64_t v171 = 0;
    void* v172 = 0;
    void* v173 = 0;
    int64_t v174 = 0; /* placeholder for void */
    int64_t v175 = 0;
    void* v176 = 0;
    int64_t v177 = 0; /* placeholder for void */
    int64_t v178 = 0;
    void* v179 = 0;
    int64_t v180 = 0; /* placeholder for void */
    int64_t v181 = 0; /* placeholder for void */
    void* v182 = 0;
    uint8_t v183 = 0;
    int64_t v184 = 0; /* placeholder for void */
    int64_t v185 = 0; /* placeholder for void */
    int64_t v186 = 0;
    void* v187 = 0;
    int64_t v188 = 0; /* placeholder for void */
    void* v189 = 0;
    int64_t v190 = 0;
    int64_t v191 = 0;
    bool v192 = 0;
    int64_t v193 = 0; /* placeholder for void */
    int64_t v194 = 0; /* placeholder for void */
    int64_t v195 = 0;
    int64_t v196 = 0;
    bool v197 = 0;
    int64_t v198 = 0; /* placeholder for void */
    int64_t v199 = 0; /* placeholder for void */
    bool v200 = 0;
    int64_t v201 = 0; /* placeholder for void */
    int64_t v202 = 0;
    int64_t v203 = 0;
    int64_t v204 = 0;
    int64_t v205 = 0; /* placeholder for void */
    int64_t v206 = 0; /* placeholder for void */
    void* v207 = 0;
    int64_t v208 = 0;
    int64_t v209 = 0;
    bool v210 = 0;
    int64_t v211 = 0; /* placeholder for void */
    int64_t v212 = 0; /* placeholder for void */
    int64_t v213 = 0;
    int64_t v214 = 0;
    bool v215 = 0;
    int64_t v216 = 0; /* placeholder for void */
    int64_t v217 = 0; /* placeholder for void */
    bool v218 = 0;
    int64_t v219 = 0; /* placeholder for void */
    int64_t v220 = 0;
    int64_t v221 = 0;
    int64_t v222 = 0;
    int64_t v223 = 0; /* placeholder for void */
    int64_t v224 = 0; /* placeholder for void */
    void* v225 = 0;
    int64_t v226 = 0;
    int64_t v227 = 0;
    bool v228 = 0;
    int64_t v229 = 0; /* placeholder for void */
    int64_t v230 = 0; /* placeholder for void */
    int64_t v231 = 0;
    int64_t v232 = 0;
    bool v233 = 0;
    int64_t v234 = 0; /* placeholder for void */
    int64_t v235 = 0; /* placeholder for void */
    bool v236 = 0;
    int64_t v237 = 0; /* placeholder for void */
    int64_t v238 = 0;
    int64_t v239 = 0;
    int64_t v240 = 0;
    int64_t v241 = 0; /* placeholder for void */
    int64_t v242 = 0; /* placeholder for void */
    int64_t v243 = 0; /* placeholder for void */
    int64_t v244 = 0; /* placeholder for void */
    int64_t v245 = 0; /* placeholder for void */
    int64_t v246 = 0;
    int64_t v247 = 0;
    int64_t v248 = 0;
    int64_t v249 = 0;
    int64_t v250 = 0;
    int64_t v251 = 0; /* placeholder for void */
    int64_t v252 = 0;
    int64_t v253 = 0;
    int64_t v254 = 0;
    int64_t v255 = 0; /* placeholder for void */
    int64_t v256 = 0; /* placeholder for void */
    bool v257 = 0;
    int64_t v258 = 0; /* placeholder for void */
    int64_t v259 = 0;
    int64_t v260 = 0;
    int64_t v261 = 0;
    int64_t v262 = 0; /* placeholder for void */
    int64_t v263 = 0; /* placeholder for void */
    int64_t v264 = 0; /* placeholder for void */
    int64_t v265 = 0;
    int64_t v266 = 0; /* placeholder for void */
    v4 = alloca(8);
    v14 = alloca(8);
    v17 = alloca(8);
    v20 = alloca(1);
    v29 = alloca(8);
    v32 = alloca(8);
    v35 = alloca(8);
    v63 = alloca(8);
    v66 = alloca(8);
    v69 = alloca(8);
    v106 = alloca(8);
    v109 = alloca(8);
    v112 = alloca(8);
    v123 = alloca(8);
    v126 = alloca(8);
    v129 = alloca(8);
    v136 = alloca(1);
    v142 = alloca(1);
    v173 = alloca(8);
    v176 = alloca(8);
    v179 = alloca(8);
    v187 = alloca(8);
    v189 = alloca(1);
    v207 = alloca(1);
    v225 = alloca(1);
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
    v23 = *(int64_t*)v14;
    v24 = *(int64_t*)v4;
    v25 = v23 < v24;
    if (v25) goto bb5;
    else goto bb6;
  bb5:
    v27 = *(int64_t*)v14;
    v28 = (void*)(intptr_t)((int64_t)v1 + v27);
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
    v51 = (int64_t)((uint64_t)v49 + (uint64_t)v50);
    *(int64_t*)v14 = (int64_t)v51;
    goto bb4;
  bb10:
    v58 = 0LL;
    return v58;
  bb11:
    goto bb12;
  bb12:
    v61 = *(int64_t*)v14;
    v62 = (void*)(intptr_t)((int64_t)v1 + v61);
    memset(v63, 0, 8);
    *(int64_t*)v63 = (int64_t)v62;
    v65 = *(int64_t*)v63;
    memset(v66, 0, 8);
    *(void**)v66 = (void*)v65;
    v68 = 0LL;
    memset(v69, 0, 8);
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
    v84 = (int64_t)((uint64_t)v82 + (uint64_t)v83);
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
    v100 = (int64_t)((uint64_t)v98 + (uint64_t)v99);
    v101 = *(int64_t*)v4;
    v102 = v100 < v101;
    if (v102) goto bb19;
    else goto bb20;
  bb16:
    v91 = *(int64_t*)v14;
    v92 = 1LL;
    v93 = (int64_t)((uint64_t)v91 + (uint64_t)v92);
    *(int64_t*)v14 = (int64_t)v93;
    goto bb18;
  bb17:
    goto bb18;
  bb18:
    goto bb15;
  bb19:
    v104 = *(int64_t*)v14;
    v105 = (void*)(intptr_t)((int64_t)v1 + v104);
    memset(v106, 0, 8);
    *(int64_t*)v106 = (int64_t)v105;
    v108 = *(int64_t*)v106;
    memset(v109, 0, 8);
    *(void**)v109 = (void*)v108;
    v111 = 0LL;
    memset(v112, 0, 8);
    *(int64_t*)v112 = (int64_t)v111;
    v115 = *(void**)v109;
    v116 = *(uint8_t*)v115;
    *(int64_t*)v112 = (int64_t)v116;
    v119 = *(int64_t*)v14;
    v120 = (void*)(intptr_t)((int64_t)v1 + v119);
    v121 = 1LL;
    v122 = (void*)(intptr_t)((int64_t)v120 + v121);
    memset(v123, 0, 8);
    *(int64_t*)v123 = (int64_t)v122;
    v125 = *(int64_t*)v123;
    memset(v126, 0, 8);
    *(void**)v126 = (void*)v125;
    v128 = 0LL;
    memset(v129, 0, 8);
    *(int64_t*)v129 = (int64_t)v128;
    v132 = *(void**)v126;
    v133 = *(uint8_t*)v132;
    *(int64_t*)v129 = (int64_t)v133;
    memset(v136, 0, 1);
    v137 = *(int64_t*)v112;
    v138 = 48LL;
    v139 = v137 == v138;
    *(bool*)v136 = v139;
    if (v139) goto bb22;
    else goto bb23;
  bb20:
    goto bb21;
  bb21:
    goto bb29;
  bb22:
    memset(v142, 0, 1);
    v143 = *(int64_t*)v129;
    v144 = 120LL;
    v145 = v143 == v144;
    *(bool*)v142 = v145;
    if (v145) goto bb25;
    else goto bb24;
  bb23:
    v156 = *(bool*)v136;
    if (v156) goto bb26;
    else goto bb27;
  bb24:
    v148 = *(int64_t*)v129;
    v149 = 88LL;
    v150 = v148 == v149;
    *(bool*)v142 = v150;
    goto bb25;
  bb25:
    v153 = *(bool*)v142;
    *(bool*)v136 = v153;
    goto bb23;
  bb26:
    v158 = *(int64_t*)v14;
    v159 = 2LL;
    v160 = (int64_t)((uint64_t)v158 + (uint64_t)v159);
    *(int64_t*)v14 = (int64_t)v160;
    goto bb28;
  bb27:
    goto bb28;
  bb28:
    goto bb21;
  bb29:
    v167 = *(int64_t*)v14;
    v168 = *(int64_t*)v4;
    v169 = v167 < v168;
    if (v169) goto bb30;
    else goto bb31;
  bb30:
    v171 = *(int64_t*)v14;
    v172 = (void*)(intptr_t)((int64_t)v1 + v171);
    memset(v173, 0, 8);
    *(int64_t*)v173 = (int64_t)v172;
    v175 = *(int64_t*)v173;
    memset(v176, 0, 8);
    *(void**)v176 = (void*)v175;
    v178 = 0LL;
    memset(v179, 0, 8);
    *(int64_t*)v179 = (int64_t)v178;
    v182 = *(void**)v176;
    v183 = *(uint8_t*)v182;
    *(int64_t*)v179 = (int64_t)v183;
    v186 = -1LL;
    memset(v187, 0, 8);
    *(int64_t*)v187 = (int64_t)v186;
    memset(v189, 0, 1);
    v190 = *(int64_t*)v179;
    v191 = 48LL;
    v192 = v190 >= v191;
    *(bool*)v189 = v192;
    if (v192) goto bb32;
    else goto bb33;
  bb31:
    v257 = *(bool*)v20;
    if (v257) goto bb47;
    else goto bb48;
  bb32:
    v195 = *(int64_t*)v179;
    v196 = 57LL;
    v197 = v195 <= v196;
    *(bool*)v189 = v197;
    goto bb33;
  bb33:
    v200 = *(bool*)v189;
    if (v200) goto bb34;
    else goto bb35;
  bb34:
    v202 = *(int64_t*)v179;
    v203 = 48LL;
    v204 = (int64_t)((uint64_t)v202 - (uint64_t)v203);
    *(int64_t*)v187 = (int64_t)v204;
    goto bb36;
  bb35:
    memset(v207, 0, 1);
    v208 = *(int64_t*)v179;
    v209 = 65LL;
    v210 = v208 >= v209;
    *(bool*)v207 = v210;
    if (v210) goto bb37;
    else goto bb38;
  bb36:
    v246 = *(int64_t*)v17;
    v247 = 16LL;
    v248 = (int64_t)((uint64_t)v246 * (uint64_t)v247);
    v249 = *(int64_t*)v187;
    v250 = (int64_t)((uint64_t)v248 + (uint64_t)v249);
    *(int64_t*)v17 = (int64_t)v250;
    v252 = *(int64_t*)v14;
    v253 = 1LL;
    v254 = (int64_t)((uint64_t)v252 + (uint64_t)v253);
    *(int64_t*)v14 = (int64_t)v254;
    goto bb29;
  bb37:
    v213 = *(int64_t*)v179;
    v214 = 70LL;
    v215 = v213 <= v214;
    *(bool*)v207 = v215;
    goto bb38;
  bb38:
    v218 = *(bool*)v207;
    if (v218) goto bb39;
    else goto bb40;
  bb39:
    v220 = *(int64_t*)v179;
    v221 = 55LL;
    v222 = (int64_t)((uint64_t)v220 - (uint64_t)v221);
    *(int64_t*)v187 = (int64_t)v222;
    goto bb41;
  bb40:
    memset(v225, 0, 1);
    v226 = *(int64_t*)v179;
    v227 = 97LL;
    v228 = v226 >= v227;
    *(bool*)v225 = v228;
    if (v228) goto bb42;
    else goto bb43;
  bb41:
    goto bb36;
  bb42:
    v231 = *(int64_t*)v179;
    v232 = 102LL;
    v233 = v231 <= v232;
    *(bool*)v225 = v233;
    goto bb43;
  bb43:
    v236 = *(bool*)v225;
    if (v236) goto bb44;
    else goto bb45;
  bb44:
    v238 = *(int64_t*)v179;
    v239 = 87LL;
    v240 = (int64_t)((uint64_t)v238 - (uint64_t)v239);
    *(int64_t*)v187 = (int64_t)v240;
    goto bb46;
  bb45:
    goto bb31;
  bb46:
    goto bb41;
  bb47:
    v259 = 0LL;
    v260 = *(int64_t*)v17;
    v261 = (int64_t)((uint64_t)v259 - (uint64_t)v260);
    *(int64_t*)v17 = (int64_t)v261;
    goto bb49;
  bb48:
    goto bb49;
  bb49:
    v265 = *(int64_t*)v17;
    return v265;
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
    int64_t v42 = 0;
    int64_t v43 = 0;
    bool v44 = 0;
    int64_t v45 = 0; /* placeholder for void */
    int64_t v46 = 0;
    void* v47 = 0;
    void* v48 = 0;
    int64_t v49 = 0; /* placeholder for void */
    int64_t v50 = 0;
    void* v51 = 0;
    int64_t v52 = 0; /* placeholder for void */
    int64_t v53 = 0;
    void* v54 = 0;
    int64_t v55 = 0; /* placeholder for void */
    int64_t v56 = 0; /* placeholder for void */
    void* v57 = 0;
    uint8_t v58 = 0;
    int64_t v59 = 0; /* placeholder for void */
    int64_t v60 = 0; /* placeholder for void */
    void* v61 = 0;
    int64_t v62 = 0;
    int64_t v63 = 0;
    bool v64 = 0;
    int64_t v65 = 0; /* placeholder for void */
    int64_t v66 = 0; /* placeholder for void */
    int64_t v67 = 0;
    int64_t v68 = 0;
    bool v69 = 0;
    int64_t v70 = 0; /* placeholder for void */
    int64_t v71 = 0; /* placeholder for void */
    bool v72 = 0;
    int64_t v73 = 0; /* placeholder for void */
    int64_t v74 = 0; /* placeholder for void */
    int64_t v75 = 0; /* placeholder for void */
    int64_t v76 = 0;
    int64_t v77 = 0;
    int64_t v78 = 0;
    int64_t v79 = 0;
    int64_t v80 = 0;
    int64_t v81 = 0;
    int64_t v82 = 0;
    int64_t v83 = 0; /* placeholder for void */
    int64_t v84 = 0;
    int64_t v85 = 0;
    int64_t v86 = 0;
    int64_t v87 = 0; /* placeholder for void */
    int64_t v88 = 0; /* placeholder for void */
    int64_t v89 = 0;
    int64_t v90 = 0; /* placeholder for void */
    v6 = alloca(8);
    v8 = alloca(8);
    v10 = alloca(8);
    v36 = alloca(8);
    v39 = alloca(8);
    v48 = alloca(8);
    v51 = alloca(8);
    v54 = alloca(8);
    v61 = alloca(1);
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
    v42 = *(int64_t*)v39;
    v43 = *(int64_t*)v10;
    v44 = v42 < v43;
    if (v44) goto bb11;
    else goto bb12;
  bb11:
    v46 = *(int64_t*)v39;
    v47 = (void*)(intptr_t)((int64_t)v1 + v46);
    memset(v48, 0, 8);
    *(int64_t*)v48 = (int64_t)v47;
    v50 = *(int64_t*)v48;
    memset(v51, 0, 8);
    *(void**)v51 = (void*)v50;
    v53 = 0LL;
    memset(v54, 0, 8);
    *(int64_t*)v54 = (int64_t)v53;
    v57 = *(void**)v51;
    v58 = *(uint8_t*)v57;
    *(int64_t*)v54 = (int64_t)v58;
    memset(v61, 0, 1);
    v62 = *(int64_t*)v54;
    v63 = 48LL;
    v64 = v62 < v63;
    *(bool*)v61 = v64;
    if (v64) goto bb14;
    else goto bb13;
  bb12:
    v89 = *(int64_t*)v36;
    return v89;
  bb13:
    v67 = *(int64_t*)v54;
    v68 = 57LL;
    v69 = v67 > v68;
    *(bool*)v61 = v69;
    goto bb14;
  bb14:
    v72 = *(bool*)v61;
    if (v72) goto bb15;
    else goto bb16;
  bb15:
    goto bb12;
  bb16:
    goto bb17;
  bb17:
    v76 = *(int64_t*)v36;
    v77 = 10LL;
    v78 = p_mul_i64(v76, v77);
    v79 = *(int64_t*)v54;
    v80 = 48LL;
    v81 = p_sub_i64(v79, v80);
    v82 = p_add_i64(v78, v81);
    *(int64_t*)v36 = (int64_t)v82;
    v84 = *(int64_t*)v39;
    v85 = 1LL;
    v86 = (int64_t)((uint64_t)v84 + (uint64_t)v85);
    *(int64_t*)v39 = (int64_t)v86;
    goto bb10;
    return 0;
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

int64_t str_len(void* v0, void* v1) {
    int64_t v2 = 0;
    int64_t v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_len((void*)v2, (int64_t)v1);
    return v3;
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

void* str_concat(void* v0, void* v1, void* v2) {
    void* v3 = 0;
    int64_t v4 = 0; /* placeholder for void */
    v3 = p_str_concat(v1, v2);
    return v3;
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

bool str_equal(void* v0, void* v1, void* v2) {
    int64_t v3 = 0;
    bool v4 = 0;
    int64_t v5 = 0; /* placeholder for void */
    v3 = 0LL;
    v4 = (bool)p_str_equal((void*)v3, (int64_t)v1, (int64_t)v2);
    return v4;
    return 0;
}

void* int_to_str(void* v0, int64_t v1) {
    void* v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = (void*)p_to_str((int64_t)v1);
    return v2;
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
    v2 = (int64_t)(intptr_t)v1;
    v3 = 0LL;
    v4 = v2 >= v3;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    v6 = (int64_t)(intptr_t)v1;
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
    v24 = (int64_t)(intptr_t)v23;
    v25 = 0LL;
    v26 = int_to_str((void*)v25, v24);
    v27 = *(uint64_t*)v21;
    v28 = (int64_t)(intptr_t)v27;
    v29 = 0LL;
    v30 = int_to_str((void*)v29, v28);
    v31 = p_str_concat(v26, v30);
    v33 = 0LL;
    Str_drop((void*)v33, (int64_t)v26);
    v35 = 0LL;
    Str_drop((void*)v35, (int64_t)v30);
    return v31;
    return 0;
}

void* float_to_str(void* v0, double v1) {
    void* v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = p_float_to_str(v1);
    return v2;
    return 0;
}

double str_parse_float(void* v0, void* v1) {
    double v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = (double)p_str_to_float(v1);
    return v2;
    return 0;
}

void* byte_to_str(void* v0, int64_t v1) {
    void* v2 = 0;
    int64_t v3 = 0; /* placeholder for void */
    v2 = p_char_to_str(v1);
    return v2;
    return 0;
}

// --- C entry point ---
#ifdef _WIN32
#include <windows.h>
#endif
void __p_install_crash_handlers(void);
#ifdef P_PROFILE
void __p_profile_start(void);
#endif
int main(int argc, char** argv) {
    __p_install_crash_handlers();
#ifdef P_PROFILE
    __p_profile_start();
#endif
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
#ifdef _WIN32
    _setmode(_fileno(stdout), _O_BINARY);
    _setmode(_fileno(stderr), _O_BINARY);
#endif
    __p_argc = argc; __p_argv = argv;
    // Register functions for p_thread_spawn lookup
    p_thread_register("Option_is_some", (int64_t(*)())Option_is_some);
    p_thread_register("Option_is_none", (int64_t(*)())Option_is_none);
    p_thread_register("Option_unwrap", (int64_t(*)())Option_unwrap);
    p_thread_register("Option_unwrap_or", (int64_t(*)())Option_unwrap_or);
    p_thread_register("Option_map", (int64_t(*)())Option_map);
    p_thread_register("Option_and_then", (int64_t(*)())Option_and_then);
    p_thread_register("Option_or_else", (int64_t(*)())Option_or_else);
    p_thread_register("Option_unwrap_or_else", (int64_t(*)())Option_unwrap_or_else);
    p_thread_register("Option_expect", (int64_t(*)())Option_expect);
    p_thread_register("Result_is_ok", (int64_t(*)())Result_is_ok);
    p_thread_register("Result_is_err", (int64_t(*)())Result_is_err);
    p_thread_register("Result_unwrap", (int64_t(*)())Result_unwrap);
    p_thread_register("Result_unwrap_or", (int64_t(*)())Result_unwrap_or);
    p_thread_register("Result_ok", (int64_t(*)())Result_ok);
    p_thread_register("Result_err", (int64_t(*)())Result_err);
    p_thread_register("Result_map", (int64_t(*)())Result_map);
    p_thread_register("Result_map_err", (int64_t(*)())Result_map_err);
    p_thread_register("Result_and_then", (int64_t(*)())Result_and_then);
    p_thread_register("Result_unwrap_or_else", (int64_t(*)())Result_unwrap_or_else);
    p_thread_register("Result_expect", (int64_t(*)())Result_expect);
    p_thread_register("Result_context", (int64_t(*)())Result_context);
    p_thread_register("Result_with_context", (int64_t(*)())Result_with_context);
    p_thread_register("Option_context", (int64_t(*)())Option_context);
    p_thread_register("StringBuilder_new", (int64_t(*)())StringBuilder_new);
    p_thread_register("StringBuilder_append_str", (int64_t(*)())StringBuilder_append_str);
    p_thread_register("StringBuilder_append_char", (int64_t(*)())StringBuilder_append_char);
    p_thread_register("StringBuilder_append_int", (int64_t(*)())StringBuilder_append_int);
    p_thread_register("StringBuilder_to_str", (int64_t(*)())StringBuilder_to_str);
    p_thread_register("StringBuilder_len", (int64_t(*)())StringBuilder_len);
    p_thread_register("StringBuilder_is_empty", (int64_t(*)())StringBuilder_is_empty);
    p_thread_register("StringBuilder_clear", (int64_t(*)())StringBuilder_clear);
    p_thread_register("ByteBuffer_new", (int64_t(*)())ByteBuffer_new);
    p_thread_register("ByteBuffer_from_string", (int64_t(*)())ByteBuffer_from_string);
    p_thread_register("ByteBuffer_write_byte", (int64_t(*)())ByteBuffer_write_byte);
    p_thread_register("ByteBuffer_write_bytes", (int64_t(*)())ByteBuffer_write_bytes);
    p_thread_register("ByteBuffer_read_byte", (int64_t(*)())ByteBuffer_read_byte);
    p_thread_register("ByteBuffer_set_byte", (int64_t(*)())ByteBuffer_set_byte);
    p_thread_register("ByteBuffer_len", (int64_t(*)())ByteBuffer_len);
    p_thread_register("ByteBuffer_is_empty", (int64_t(*)())ByteBuffer_is_empty);
    p_thread_register("ByteBuffer_clear", (int64_t(*)())ByteBuffer_clear);
    p_thread_register("ByteBuffer_to_str", (int64_t(*)())ByteBuffer_to_str);
    p_thread_register("parse_port", (int64_t(*)())parse_port);
    p_thread_register("load_port", (int64_t(*)())load_port);
    p_thread_register("find", (int64_t(*)())find);
    p_thread_register("where_is", (int64_t(*)())where_is);
    p_thread_register("label", (int64_t(*)())label);
    p_thread_register("show", (int64_t(*)())show);
    p_thread_register("file_read", (int64_t(*)())file_read);
    p_thread_register("file_read_result", (int64_t(*)())file_read_result);
    p_thread_register("file_write", (int64_t(*)())file_write);
    p_thread_register("file_append", (int64_t(*)())file_append);
    p_thread_register("file_exists", (int64_t(*)())file_exists);
    p_thread_register("file_write_bytes", (int64_t(*)())file_write_bytes);
    p_thread_register("file_read_bytes", (int64_t(*)())file_read_bytes);
    p_thread_register("file_write_raw", (int64_t(*)())file_write_raw);
    p_thread_register("file_delete", (int64_t(*)())file_delete);
    p_thread_register("file_size", (int64_t(*)())file_size);
    p_thread_register("fs_read_text", (int64_t(*)())fs_read_text);
    p_thread_register("fs_write_text", (int64_t(*)())fs_write_text);
    p_thread_register("fs_append_text", (int64_t(*)())fs_append_text);
    p_thread_register("fs_exists", (int64_t(*)())fs_exists);
    p_thread_register("fs_delete", (int64_t(*)())fs_delete);
    p_thread_register("fs_size", (int64_t(*)())fs_size);
    p_thread_register("println", (int64_t(*)())println);
    p_thread_register("assert", (int64_t(*)())assert);
    p_thread_register("assert_msg", (int64_t(*)())assert_msg);
    p_thread_register("panic", (int64_t(*)())panic);
    p_thread_register("max", (int64_t(*)())p_max);
    p_thread_register("min", (int64_t(*)())p_min);
    p_thread_register("abs", (int64_t(*)())p_abs);
    p_thread_register("clamp", (int64_t(*)())clamp);
    p_thread_register("create_allocator", (int64_t(*)())create_allocator);
    p_thread_register("allocator_alloc", (int64_t(*)())allocator_alloc);
    p_thread_register("allocator_free", (int64_t(*)())allocator_free);
    p_thread_register("allocator_bytes_used", (int64_t(*)())allocator_bytes_used);
    p_thread_register("Vec_new", (int64_t(*)())Vec_new);
    p_thread_register("Vec_drop", (int64_t(*)())Vec_drop);
    p_thread_register("Str_drop", (int64_t(*)())Str_drop);
    p_thread_register("Vec_push", (int64_t(*)())Vec_push);
    p_thread_register("Vec_pop", (int64_t(*)())Vec_pop);
    p_thread_register("Vec_len", (int64_t(*)())Vec_len);
    p_thread_register("Vec_is_empty", (int64_t(*)())Vec_is_empty);
    p_thread_register("Vec_get", (int64_t(*)())Vec_get);
    p_thread_register("Vec_set", (int64_t(*)())Vec_set);
    p_thread_register("Vec_clear", (int64_t(*)())Vec_clear);
    p_thread_register("Vec_capacity", (int64_t(*)())Vec_capacity);
    p_thread_register("Vec_contains", (int64_t(*)())Vec_contains);
    p_thread_register("Vec_first", (int64_t(*)())Vec_first);
    p_thread_register("Vec_last", (int64_t(*)())Vec_last);
    p_thread_register("Vec_truncate", (int64_t(*)())Vec_truncate);
    p_thread_register("Vec_reverse", (int64_t(*)())Vec_reverse);
    p_thread_register("Vec_extend", (int64_t(*)())Vec_extend);
    p_thread_register("Vec_insert", (int64_t(*)())Vec_insert);
    p_thread_register("Vec_remove", (int64_t(*)())Vec_remove);
    p_thread_register("Vec_clone", (int64_t(*)())Vec_clone);
    p_thread_register("Vec_to_str", (int64_t(*)())Vec_to_str);
    p_thread_register("Vec_sort", (int64_t(*)())Vec_sort);
    p_thread_register("HashMap_new", (int64_t(*)())HashMap_new);
    p_thread_register("HashMap_find_slot", (int64_t(*)())HashMap_find_slot);
    p_thread_register("HashMap_grow", (int64_t(*)())HashMap_grow);
    p_thread_register("HashMap_insert", (int64_t(*)())HashMap_insert);
    p_thread_register("HashMap_get", (int64_t(*)())HashMap_get);
    p_thread_register("HashMap_contains", (int64_t(*)())HashMap_contains);
    p_thread_register("HashMap_remove", (int64_t(*)())HashMap_remove);
    p_thread_register("HashMap_len", (int64_t(*)())HashMap_len);
    p_thread_register("HashMap_is_empty", (int64_t(*)())HashMap_is_empty);
    p_thread_register("HashMap_clear", (int64_t(*)())HashMap_clear);
    p_thread_register("HashMap_keys", (int64_t(*)())HashMap_keys);
    p_thread_register("HashMap_values", (int64_t(*)())HashMap_values);
    p_thread_register("HashMap_clone", (int64_t(*)())HashMap_clone);
    p_thread_register("HashMap_get_or", (int64_t(*)())HashMap_get_or);
    p_thread_register("HashMap_to_str", (int64_t(*)())HashMap_to_str);
    p_thread_register("String_from", (int64_t(*)())String_from);
    p_thread_register("String_len", (int64_t(*)())String_len);
    p_thread_register("String_is_empty", (int64_t(*)())String_is_empty);
    p_thread_register("String_push_str", (int64_t(*)())String_push_str);
    p_thread_register("String_contains", (int64_t(*)())String_contains);
    p_thread_register("String_starts_with", (int64_t(*)())String_starts_with);
    p_thread_register("String_ends_with", (int64_t(*)())String_ends_with);
    p_thread_register("String_trim", (int64_t(*)())String_trim);
    p_thread_register("String_to_uppercase", (int64_t(*)())String_to_uppercase);
    p_thread_register("String_to_lowercase", (int64_t(*)())String_to_lowercase);
    p_thread_register("String_slice", (int64_t(*)())String_slice);
    p_thread_register("String_slice_len", (int64_t(*)())String_slice_len);
    p_thread_register("String_find", (int64_t(*)())String_find);
    p_thread_register("String_rfind", (int64_t(*)())String_rfind);
    p_thread_register("String_replace", (int64_t(*)())String_replace);
    p_thread_register("String_split", (int64_t(*)())String_split);
    p_thread_register("String_parse_int", (int64_t(*)())String_parse_int);
    p_thread_register("String_char_at", (int64_t(*)())String_char_at);
    p_thread_register("String_repeat", (int64_t(*)())String_repeat);
    p_thread_register("String_to_string", (int64_t(*)())String_to_string);
    p_thread_register("String_clone", (int64_t(*)())String_clone);
    p_thread_register("String_bytes", (int64_t(*)())String_bytes);
    p_thread_register("String_byte_at", (int64_t(*)())String_byte_at);
    p_thread_register("String_char_count", (int64_t(*)())String_char_count);
    p_thread_register("u64_to_str", (int64_t(*)())u64_to_str);
    p_thread_register("float_to_str", (int64_t(*)())float_to_str);
    p_thread_register("byte_to_str", (int64_t(*)())byte_to_str);
    __init_globals();
    int64_t result = main_entry(NULL);
    return (int)result;
}


// --- Runtime implementation ---

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
    char* result = (char*)p_malloc((int64_t)(len + 1));
    strcpy(result, s);
    strcat(result, other);
    return result;
}

char* p_string_from(const char* s) {
    if (!s) s = "";
    size_t len = strlen(s);
    char* result = (char*)p_malloc((int64_t)(len + 1));
    memcpy(result, s, len + 1);
    return result;
}

char* p_string_slice(const char* s, int64_t start, int64_t end) {
    if (!s) { char* r = (char*)p_malloc(1); r[0] = 0; return r; }
    int64_t len = (int64_t)strlen(s);
    if (start < 0) start = 0;
    if (end > len) end = len;
    if (start > end) start = end;
    int64_t slen = end - start;
    char* result = (char*)p_malloc(slen + 1);
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
    size_t len = strlen(a) + strlen(b);
    char* result = (char*)p_malloc((int64_t)(len + 1));
    strcpy(result, a);
    strcat(result, b);
    return result;
}

char* p_str_join_n(PVec* parts) {
    size_t total = 1; /* null terminator */
    for (int64_t i = 0; i < parts->len; i++) {
        total += strlen((char*)parts->data[i]);
    }
    char* result = (char*)p_malloc((int64_t)total);
    result[0] = 0;
    for (int64_t i = 0; i < parts->len; i++) {
        strcat(result, (char*)parts->data[i]);
    }
    return result;
}

typedef struct { char* buf; int64_t len; int64_t cap; } PStrBuilder;
int64_t p_sb_new(void) {
    PStrBuilder* sb = (PStrBuilder*)p_malloc_persistent(sizeof(PStrBuilder));
    sb->cap = 4096;
    sb->len = 0;
    sb->buf = (char*)p_malloc_persistent(sb->cap);
    sb->buf[0] = 0;
    return (int64_t)sb;
}
void p_sb_append(int64_t sbv, const char* s) {
    PStrBuilder* sb = (PStrBuilder*)sbv;
    int64_t slen = (int64_t)strlen(s);
    if (sb->len + slen + 1 > sb->cap) {
        while (sb->len + slen + 1 > sb->cap) sb->cap *= 2;
        char* nb = (char*)p_malloc_persistent(sb->cap);
        memcpy(nb, sb->buf, sb->len);
        nb[sb->len] = 0;
        sb->buf = nb; /* old buffer leaked in persistent pool - acceptable */
    }
    memcpy(sb->buf + sb->len, s, slen);
    sb->len += slen;
    sb->buf[sb->len] = 0;
}
char* p_sb_to_str(int64_t sbv) {
    PStrBuilder* sb = (PStrBuilder*)sbv;
    char* result = (char*)p_malloc(sb->len + 1);
    memcpy(result, sb->buf, sb->len);
    result[sb->len] = 0;
    return result;
}

void p_print(int64_t val) { char b[32]; int n = snprintf(b, sizeof b, "%lld", (long long)val); fwrite(b, 1, (size_t)n, stdout); }
void p_print_str(const char* s) { fwrite(s, 1, strlen(s), stdout); }
void p_eprint_str(const char* s) { fwrite(s, 1, strlen(s), stderr); fflush(stderr); }
void p_flush_stdout(void) { fflush(stdout); }
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
    fprintf(stderr, "stack trace:\n    - %s\n", names[0]);
    for (int i = 1; i < m; ) {
        int j = i + 1;
        while (j < m && strcmp(names[j], names[i]) == 0) j++;
        if (j - i > 1) fprintf(stderr, "    -| called from %s (%d times)\n", names[i], j - i);
        else fprintf(stderr, "    -| called from %s\n", names[i]);
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
#ifdef P_PROFILE
/* Sampling profiler (fid profile): the call stack every millisecond, written
   at exit as collapsed stacks ("main;f;g 1"), which flamegraph tools read. */
#define P_PROF_DEPTH 64
#define P_PROF_MAX 100000
static uintptr_t* __p_prof_buf;
static volatile long __p_prof_n;
static volatile int __p_prof_on;
static void __p_prof_record(uintptr_t* f, int n) {
    long i = __p_prof_n;
    if (i >= P_PROF_MAX) return;
    uintptr_t* row = __p_prof_buf + i * P_PROF_DEPTH;
    for (int k = 0; k < P_PROF_DEPTH; k++) row[k] = k < n ? f[k] : 0;
    __p_prof_n = i + 1;
}
static void __p_prof_dump(void) {
    __p_prof_on = 0;
    const char* path = getenv("FIDES_PROFILE_OUT");
    if (!path || !*path) path = "fides-profile.folded";
    FILE* out = fopen(path, "w");
    if (!out) return;
    for (long i = 0; i < __p_prof_n; i++) {
        uintptr_t* row = __p_prof_buf + i * P_PROF_DEPTH;
        const char* names[P_PROF_DEPTH];
        int m = 0;
        for (int k = 0; k < P_PROF_DEPTH && row[k]; k++) {
            const char* nm = __p_fn_name(row[k]);
            if (nm) names[m++] = nm;
        }
        if (m == 0) continue;
        for (int k = m - 1; k >= 0; k--) fprintf(out, k == m - 1 ? "%s" : ";%s", names[k]);
        fprintf(out, " 1\n");
    }
    fclose(out);
}
#if defined(_WIN32) && (defined(_M_X64) || defined(__x86_64__)) && !defined(__TINYC__)
static HANDLE __p_prof_target;
static DWORD WINAPI __p_prof_loop(LPVOID a) {
    (void)a;
    /* Sleep(1) sleeps a whole 15.6 ms tick: wait on the performance counter */
    LARGE_INTEGER fr, t0, t1;
    QueryPerformanceFrequency(&fr);
    while (__p_prof_on) {
        QueryPerformanceCounter(&t0);
        do { Sleep(0); QueryPerformanceCounter(&t1); } while ((t1.QuadPart - t0.QuadPart) * 1000 < fr.QuadPart);
        if (SuspendThread(__p_prof_target) == (DWORD)-1) break;
        CONTEXT c;
        memset(&c, 0, sizeof c);
        c.ContextFlags = CONTEXT_FULL;
        if (GetThreadContext(__p_prof_target, &c)) {
            uintptr_t f[P_PROF_DEPTH];
            __p_prof_record(f, __p_frames(&c, 1, f, P_PROF_DEPTH));
        }
        ResumeThread(__p_prof_target);
    }
    return 0;
}
void __p_profile_start(void) {
    __p_prof_buf = (uintptr_t*)malloc(sizeof(uintptr_t) * P_PROF_DEPTH * P_PROF_MAX);
    if (!__p_prof_buf) return;
    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &__p_prof_target, 0, FALSE, DUPLICATE_SAME_ACCESS);
    __p_prof_on = 1;
    atexit(__p_prof_dump);
    CreateThread(NULL, 0, __p_prof_loop, NULL, 0, NULL);
}
#elif !defined(_WIN32) && (defined(__GNUC__) || defined(__clang__)) && !defined(__TINYC__)
#include <sys/time.h>
static void __p_prof_tick(int sig) {
    (void)sig;
    if (!__p_prof_on) return;
    uintptr_t f[P_PROF_DEPTH];
    __p_prof_record(f, __p_frames(NULL, 0, f, P_PROF_DEPTH));
}
void __p_profile_start(void) {
    __p_prof_buf = (uintptr_t*)malloc(sizeof(uintptr_t) * P_PROF_DEPTH * P_PROF_MAX);
    if (!__p_prof_buf) return;
    __p_prof_on = 1;
    atexit(__p_prof_dump);
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = __p_prof_tick;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGPROF, &sa, NULL);
    struct itimerval t;
    t.it_interval.tv_sec = 0;
    t.it_interval.tv_usec = 1000;
    t.it_value = t.it_interval;
    setitimer(ITIMER_PROF, &t, NULL);
}
#else
void __p_profile_start(void) {}
#endif
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
#if defined(P_LEAKS) || defined(P_CHECKED_ALLOC)
#define P_MEMCHECK 1
#define p_malloc __p_block_malloc
#define p_free __p_block_free
static void* __p_block_malloc(int64_t size);
static void __p_block_free(void* ptr);
static
#endif
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
#ifdef P_MEMCHECK
static
#endif
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
#ifdef P_MEMCHECK
#undef p_malloc
#undef p_free
/* fid ccompile --leaks / --checked-alloc. Every live block is in a table with
   the first Fides functions on the stack when it was allocated. --leaks: the
   blocks still live at exit are reported, grouped by where they were
   allocated. --checked-alloc: each block gets its own pages, ending at an
   inaccessible page, and a freed block's pages become inaccessible and are
   never reused, so a read or write past the end or after free faults where
   it happens (the crash trace shows it). */
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif
#if defined(_MSC_VER)
#include <intrin.h>
static volatile long __p_mc_lk = 0;
#define __P_MC_LOCK() while (_InterlockedExchange(&__p_mc_lk, 1)) {}
#define __P_MC_UNLOCK() _InterlockedExchange(&__p_mc_lk, 0)
#else
static volatile int __p_mc_lk = 0;
#define __P_MC_LOCK() while (__sync_lock_test_and_set(&__p_mc_lk, 1)) {}
#define __P_MC_UNLOCK() __sync_lock_release(&__p_mc_lk)
#endif
#define __P_MC_SITES 4
typedef struct { void* p; void* base; int64_t size; int64_t span; uintptr_t site[__P_MC_SITES]; } __p_mc_rec;
static __p_mc_rec* __p_mc_tab = NULL;
static uint64_t __p_mc_cap = 0, __p_mc_used = 0;
static int __p_mc_started = 0;
static uint64_t __p_mc_hash(void* p) {
    uint64_t x = (uint64_t)(uintptr_t)p;
    x ^= x >> 33; x *= 0xff51afd7ed558ccdULL; x ^= x >> 33;
    return x;
}
static __p_mc_rec* __p_mc_find(void* p) {
    if (!__p_mc_cap) return NULL;
    uint64_t m = __p_mc_cap - 1, i = __p_mc_hash(p) & m;
    while (__p_mc_tab[i].p) {
        if (__p_mc_tab[i].p == p) return &__p_mc_tab[i];
        i = (i + 1) & m;
    }
    return NULL;
}
static void __p_mc_put(const __p_mc_rec* r) {
    if ((__p_mc_used + 1) * 2 > __p_mc_cap) {
        uint64_t nc = __p_mc_cap ? __p_mc_cap * 2 : 4096;
        __p_mc_rec* old = __p_mc_tab;
        uint64_t oc = __p_mc_cap;
        __p_mc_tab = (__p_mc_rec*)calloc((size_t)nc, sizeof(__p_mc_rec));
        if (!__p_mc_tab) { fprintf(stderr, "fid: out of memory in the allocation table\n"); abort(); }
        __p_mc_cap = nc;
        for (uint64_t i = 0; i < oc; i++) {
            if (!old[i].p) continue;
            uint64_t j = __p_mc_hash(old[i].p) & (nc - 1);
            while (__p_mc_tab[j].p) j = (j + 1) & (nc - 1);
            __p_mc_tab[j] = old[i];
        }
        free(old);
    }
    uint64_t m = __p_mc_cap - 1, i = __p_mc_hash(r->p) & m;
    while (__p_mc_tab[i].p) i = (i + 1) & m;
    __p_mc_tab[i] = *r;
    __p_mc_used++;
}
/* Linear probing: deleting shifts the entries after it back into place. */
static void __p_mc_del(__p_mc_rec* e) {
    uint64_t m = __p_mc_cap - 1, i = (uint64_t)(e - __p_mc_tab), j = i;
    for (;;) {
        j = (j + 1) & m;
        if (!__p_mc_tab[j].p) break;
        uint64_t k = __p_mc_hash(__p_mc_tab[j].p) & m;
        if ((j > i && (k <= i || k > j)) || (j < i && k <= i && k > j)) { __p_mc_tab[i] = __p_mc_tab[j]; i = j; }
    }
    __p_mc_tab[i].p = NULL;
    __p_mc_used--;
}
static void __p_mc_where(uintptr_t* site) {
    uintptr_t f[24];
    int n = __p_frames(NULL, 0, f, 24), k = 0;
    for (int i = 0; i < n && k < __P_MC_SITES; i++) {
        if (__p_fn_name(f[i])) site[k++] = f[i];
    }
}
#ifdef P_LEAKS
static int __p_mc_cmp(const void* a, const void* b) {
    int64_t x = ((const __p_mc_rec*)a)->size, y = ((const __p_mc_rec*)b)->size;
    return x < y ? 1 : x > y ? -1 : 0;
}
static void __p_mc_report(void) {
    fflush(stdout);
    uint64_t blocks = 0; int64_t bytes = 0;
    __p_mc_rec* by_site = (__p_mc_rec*)calloc((size_t)__p_mc_used + 1, sizeof(__p_mc_rec));
    uint64_t sites = 0;
    for (uint64_t i = 0; i < __p_mc_cap && by_site; i++) {
        __p_mc_rec* r = &__p_mc_tab[i];
        if (!r->p) continue;
        blocks++; bytes += r->size;
        uint64_t s = 0;
        while (s < sites && memcmp(by_site[s].site, r->site, sizeof r->site) != 0) s++;
        if (s == sites) { memcpy(by_site[s].site, r->site, sizeof r->site); sites++; }
        by_site[s].size += r->size;
        by_site[s].span += 1;
    }
    if (blocks == 0) { fprintf(stderr, "fid --leaks: every block was freed\n"); free(by_site); return; }
    fprintf(stderr, "fid --leaks: %llu block(s), %lld bytes, never freed; by where they were allocated:\n",
            (unsigned long long)blocks, (long long)bytes);
    qsort(by_site, (size_t)sites, sizeof(__p_mc_rec), __p_mc_cmp);
    for (uint64_t s = 0; s < sites && s < 20; s++) {
        fprintf(stderr, "  %8lld bytes in %6lld block(s): ", (long long)by_site[s].size, (long long)by_site[s].span);
        int shown = 0;
        for (int k = 0; k < __P_MC_SITES && by_site[s].site[k]; k++) {
            fprintf(stderr, "%s%s", shown ? " <- " : "", __p_fn_name(by_site[s].site[k]));
            shown = 1;
        }
        fprintf(stderr, "%s\n", shown ? "" : "(no Fides frame)");
    }
    if (sites > 20) fprintf(stderr, "  ... and %llu more place(s)\n", (unsigned long long)(sites - 20));
    free(by_site);
}
#endif
#ifdef P_CHECKED_ALLOC
static size_t __p_mc_page(void) {
#ifdef _WIN32
    SYSTEM_INFO si; GetSystemInfo(&si); return (size_t)si.dwPageSize;
#else
    return (size_t)sysconf(_SC_PAGESIZE);
#endif
}
#endif
void* p_malloc(int64_t size) {
    if (size < 0) { fprintf(stderr, "p_malloc: negative size %lld\n", (long long)size); abort(); }
    __p_mc_rec r;
    memset(&r, 0, sizeof r);
    __p_mc_where(r.site);
    r.size = size;
#ifdef P_CHECKED_ALLOC
    size_t pg = __p_mc_page();
    size_t used = ((size_t)size + 15) & ~(size_t)15;
    size_t span = (used + pg - 1) / pg * pg + pg;
#ifdef _WIN32
    char* base = (char*)VirtualAlloc(NULL, span, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    DWORD old;
    if (base) VirtualProtect(base + span - pg, pg, PAGE_NOACCESS, &old);
#else
    char* base = (char*)mmap(NULL, span, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (base == (char*)MAP_FAILED) base = NULL;
    if (base) mprotect(base + span - pg, pg, PROT_NONE);
#endif
    if (!base) { fprintf(stderr, "p_malloc: out of memory (--checked-alloc uses pages per block)\n"); abort(); }
    r.base = base;
    r.span = (int64_t)span;
    r.p = base + span - pg - used;
#else
    r.p = __p_block_malloc(size);
#endif
    if (!r.p) return r.p;
    __P_MC_LOCK();
#ifdef P_LEAKS
    if (!__p_mc_started) { __p_mc_started = 1; atexit(__p_mc_report); }
#endif
    __p_mc_put(&r);
    __P_MC_UNLOCK();
    return r.p;
}
void p_free(void* ptr) {
    if (!ptr) return;
    __P_MC_LOCK();
    __p_mc_rec* e = __p_mc_find(ptr);
    __p_mc_rec r;
    if (e) { r = *e; __p_mc_del(e); }
    __P_MC_UNLOCK();
    /* not from p_malloc (p_shared_alloc, say): the block allocator's own */
    if (!e) { __p_block_free(ptr); return; }
#ifdef P_CHECKED_ALLOC
#ifdef _WIN32
    DWORD old;
    VirtualProtect(r.base, (size_t)r.span, PAGE_NOACCESS, &old);
#else
    mprotect(r.base, (size_t)r.span, PROT_NONE);
#endif
#else
    __p_block_free(ptr);
#endif
}
#endif
void* p_alloc(int64_t size) { return p_malloc(size); }
void p_region_free(void* region) { (void)region; }
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
bool p_file_exists(const char* path) { FILE* f = fopen(path, "rb"); if (f) { fclose(f); return true; } return false; }
int64_t p_file_append(const char* path, const char* content) {
    FILE* f = fopen(path, "ab");
    if (!f) return -1;
    fputs(content, f); fclose(f); return 0;
}
int64_t p_file_delete(const char* path) { return remove(path) == 0 ? 0 : -1; }
int64_t p_file_size(const char* path) { FILE* f = fopen(path, "rb"); if (!f) return -1; fseek(f, 0, SEEK_END); int64_t sz = ftell(f); fclose(f); return sz < 0 ? -1 : sz; }
int64_t p_file_hash(const char* path) { FILE* f = fopen(path, "rb"); if (!f) return 0; uint64_t h = 0xcbf29ce484222325ULL; unsigned char buf[65536]; size_t n; while ((n = fread(buf, 1, sizeof buf, f)) > 0) { for (size_t i = 0; i < n; i++) { h ^= buf[i]; h *= 0x100000001b3ULL; } } fclose(f); return (int64_t)h; }
char* p_read_line(void) {
    static char* __p_line_buf = NULL;
    static int __p_line_cap = 0;
    if (!__p_line_buf) { __p_line_cap = 4096; __p_line_buf = (char*)p_malloc_persistent(__p_line_cap); }
    if (!fgets(__p_line_buf, __p_line_cap, stdin)) return NULL;
    int len = (int)strlen(__p_line_buf);
    while (len > 0 && (__p_line_buf[len-1] == '\n' || __p_line_buf[len-1] == '\r')) { __p_line_buf[--len] = 0; }
    return __p_line_buf;
}
int64_t p_stdin_eof(void) { return (int64_t)feof(stdin); }
char* p_read_stdin(int64_t n) {
    char* buf = (char*)p_malloc_persistent(n + 1);
    if (!buf) return NULL;
    int64_t total = 0;
    while (total < n) {
        int64_t rd = (int64_t)fread(buf + total, 1, (size_t)(n - total), stdin);
        if (rd <= 0) { if (total == 0) return NULL; break; }
        total += rd;
    }
    buf[total] = 0;
    return buf;
}
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
#ifndef _WIN32
#include <ucontext.h>
#include <poll.h>
#include <time.h>
#endif
typedef struct __p_co { int64_t (*fn)(); int64_t nargs; int64_t args[8]; int64_t result; int done; int detached;
    struct __p_co* awaits; struct __p_co* back;
    int64_t fd; int64_t fd_write; int64_t ready; int64_t wake_ms;
#ifdef _WIN32
    void* fiber;
#else
    ucontext_t ctx; char* stack;
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
static P_THREAD_LOCAL ucontext_t __p_co_sched;
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
static void __p_co_leave(void) {
    __p_co* c = __p_co_cur;
    __p_co* back = c->back;
    c->back = NULL;
    __p_co_cur = back;
#ifdef _WIN32
    SwitchToFiber(back ? back->fiber : __p_co_sched);
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
    getcontext(&c->ctx);
    c->ctx.uc_stack.ss_sp = c->stack;
    c->ctx.uc_stack.ss_size = P_CO_STACK;
    c->ctx.uc_link = NULL;
    makecontext(&c->ctx, (void (*)(void))__p_co_start, 2, (unsigned)(uintptr_t)c, (unsigned)((uintptr_t)c >> 32));
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
void p_thread_join(int64_t handle) {
    if (handle >= 0) return; // not a thread handle
    int64_t idx = -handle - 1;
    if (idx < 0 || idx >= p_thread_handle_count) return;
    pthread_join(p_thread_handles[idx], NULL);
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
// PENDING_SENTINEL - returned by p_chan_try_recv when channel is empty
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
    if (!s) { char* r = (char*)p_malloc(1); r[0] = 0; return r; }
    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') s++;
    size_t len = strlen(s);
    while (len > 0 && (s[len-1] == ' ' || s[len-1] == '\t' || s[len-1] == '\n' || s[len-1] == '\r')) len--;
    char* result = (char*)p_malloc((int64_t)(len + 1));
    memcpy(result, s, len);
    result[len] = 0;
    return result;
}
char* p_string_to_uppercase(const char* s) {
    if (!s) { char* r = (char*)p_malloc(1); r[0] = 0; return r; }
    size_t len = strlen(s);
    char* result = (char*)p_malloc((int64_t)(len + 1));
    for (size_t i = 0; i < len; i++) result[i] = toupper((unsigned char)s[i]);
    result[len] = 0;
    return result;
}
char* p_string_to_lowercase(const char* s) {
    if (!s) { char* r = (char*)p_malloc(1); r[0] = 0; return r; }
    size_t len = strlen(s);
    char* result = (char*)p_malloc((int64_t)(len + 1));
    for (size_t i = 0; i < len; i++) result[i] = tolower((unsigned char)s[i]);
    result[len] = 0;
    return result;
}
char* p_string_repeat(const char* s, int64_t n) {
    if (!s) { char* r = (char*)p_malloc(1); r[0] = 0; return r; }
    if (n < 0) n = 0;
    size_t slen = strlen(s);
    if (slen > 0 && (size_t)n > (size_t)(-1) / slen) {
        char* r = (char*)p_malloc(1); r[0] = 0; return r;
    }
    size_t total = slen * (size_t)n;
    if (total > (size_t)(-1) - 1) {
        char* r = (char*)p_malloc(1); r[0] = 0; return r;
    }
    char* result = (char*)p_malloc((int64_t)(total + 1));
    result[0] = 0;
    for (int64_t i = 0; i < n; i++) strcat(result, s);
    return result;
}
char* p_string_replace(const char* s, const char* from, const char* to) {
    if (!s) { char* r = (char*)p_malloc(1); r[0] = 0; return r; }
    if (!from || !to || !*from) { size_t sl = strlen(s); char* r = (char*)p_malloc((int64_t)(sl + 1)); memcpy(r, s, sl + 1); return r; }
    size_t slen = strlen(s), flen = strlen(from), tlen = strlen(to);
    char* result = (char*)p_malloc((int64_t)(slen * (tlen > flen ? tlen : flen) + 1));
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
    if (!s) { char* r = (char*)p_malloc(1); r[0] = 0; return r; }
    char* result = (char*)p_malloc(2);
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
char* p_to_str_str(const char* s) { if (!s) s = ""; size_t l = strlen(s); char* r = (char*)p_malloc((int64_t)(l+1)); memcpy(r, s, l+1); return r; }
char* p_char_to_str(int64_t val) { char* s = (char*)p_malloc(2); s[0] = (char)val; s[1] = 0; return s; }
char* p_to_str_bool(bool val) { char* r = (char*)p_malloc(6); strcpy(r, val ? "true" : "false"); return r; }
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

// Socket operations - native POSIX/Winsock implementation
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
    if (n >= (int64_t)sizeof(buf)) n = (int64_t)sizeof(buf)-1;
    buf[n] = 0;
    return strdup(buf);
}

// --- Nonblocking sockets + readiness poll (async event loop) ---
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

// --- Signals ---
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

// --- Memory-mapped files / anonymous mappings ---
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

// Bare metal stubs - no-ops in C codegen (use --bare for real hardware)
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
int64_t p_phys_read8(int64_t addr) { return _phys_mem[(addr/8) & 65535] & 0xFF; }
void p_phys_write8(int64_t addr, int64_t val) { _phys_mem[(addr/8) & 65535] = (_phys_mem[(addr/8) & 65535] & ~0xFF) | (val & 0xFF); }
int64_t p_phys_read16(int64_t addr) { return _phys_mem[(addr/8) & 65535] & 0xFFFF; }
void p_phys_write16(int64_t addr, int64_t val) { _phys_mem[(addr/8) & 65535] = (_phys_mem[(addr/8) & 65535] & ~0xFFFF) | (val & 0xFFFF); }
int64_t p_phys_read32(int64_t addr) { return _phys_mem[(addr/8) & 65535] & 0xFFFFFFFF; }
void p_phys_write32(int64_t addr, int64_t val) { _phys_mem[(addr/8) & 65535] = (_phys_mem[(addr/8) & 65535] & ~0xFFFFFFFFLL) | (val & 0xFFFFFFFFLL); }
int64_t p_inb(int64_t port) { (void)port; return 0; }
void p_outb(int64_t port, int64_t val) { (void)port; (void)val; }
int64_t p_inw(int64_t port) { (void)port; return 0; }
void p_outw(int64_t port, int64_t val) { (void)port; (void)val; }
int64_t p_inl(int64_t port) { (void)port; return 0; }
void p_outl(int64_t port, int64_t val) { (void)port; (void)val; }
int64_t p_msr_read(int64_t addr) { (void)addr; return 0; }
void p_msr_write(int64_t addr, int64_t val) { (void)addr; (void)val; }
int64_t p_cpuid(int64_t leaf, int64_t subleaf) { (void)leaf; (void)subleaf; return 0; }
int64_t p_cpuid_leaf(int64_t leaf) { (void)leaf; return 0; }
void p_gdt_set_entry(int64_t gdt, int64_t index, int64_t base, int64_t limit, int64_t flags) { (void)gdt; (void)index; (void)base; (void)limit; (void)flags; }
void p_idt_set_entry(int64_t idt, int64_t vector, int64_t handler, int64_t flags) { (void)idt; (void)vector; (void)handler; (void)flags; }
void p_tss_init(int64_t tss, int64_t rsp0) { (void)tss; (void)rsp0; }
void p_tss_load(int64_t selector) { (void)selector; }
void p_lapic_write(int64_t reg, int64_t val) { (void)reg; (void)val; }
int64_t p_lapic_read(int64_t reg) { (void)reg; return 0; }
void p_lapic_eoi(void) {}
void p_ioapic_write(int64_t reg, int64_t val) { (void)reg; (void)val; }
int64_t p_ioapic_read(int64_t reg) { (void)reg; return 0; }
void p_pic_remap(void) {}
void p_pic_mask(int64_t irq, int64_t masked) { (void)irq; (void)masked; }
void p_pic_eoi(int64_t irq) { (void)irq; }
int64_t p_cmos_read(int64_t reg) { (void)reg; return 0; }
void p_cmos_write(int64_t reg, int64_t val) { (void)reg; (void)val; }
void p_serial_init(void) {}
void p_serial_write(int64_t c) { (void)c; }
int64_t p_serial_read(void) { return 0; }
int64_t p_serial_ready(void) { return 0; }
void p_serial_putc(int64_t c) { (void)c; }
int64_t p_serial_getc(void) { return 0; }
int64_t p_serial_available(void) { return 0; }
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
void p_lgdt(int64_t gdt_desc) { (void)gdt_desc; }
void p_enter_user_mode(int64_t entry, int64_t stack) { (void)entry; (void)stack; }
void p_pit_set_frequency(int64_t hz) { (void)hz; }
int64_t p_hpet_read(int64_t reg) { (void)reg; return 0; }
void p_hpet_write(int64_t reg, int64_t val) { (void)reg; (void)val; }
void p_syscall_set_handler(int64_t num, int64_t handler) { (void)num; (void)handler; }
void p_ring3_enter(int64_t entry, int64_t stack, int64_t arg) { (void)entry; (void)stack; (void)arg; }
void p_context_save(int64_t ctx) { (void)ctx; }
void p_context_restore(int64_t ctx) { (void)ctx; }
int64_t p_pci_read(int64_t bus, int64_t dev, int64_t func, int64_t offset) { (void)bus; (void)dev; (void)func; (void)offset; return 0; }
void p_pci_write(int64_t bus, int64_t dev, int64_t func, int64_t offset, int64_t val) { (void)bus; (void)dev; (void)func; (void)offset; (void)val; }
void p_ata_read(int64_t lba, int64_t buf) { (void)lba; (void)buf; }
void p_ata_write(int64_t lba, int64_t buf) { (void)lba; (void)buf; }
void p_acpi_shutdown(void) {}
void p_fb_init(void) {}
void p_fb_plot(int64_t x, int64_t y, int64_t color) { (void)x; (void)y; (void)color; }
void p_fb_fill(int64_t x, int64_t y, int64_t w, int64_t h, int64_t color) { (void)x; (void)y; (void)w; (void)h; (void)color; }

// SIMD builtins - operate on Fides Vec (i64 pointer, f64 stored as i64 bits)
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

// --- Tensor builtins (Phase 5C) ---

// Tensor builtins - simple flat array with shape metadata
// PTensor typedef already emitted in header section

// --- ML3: Autograd tape data structures (must be before tensor builtins) ---
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
        char nb[40]; char* fs = 0; const char* s;
        if (d == trunc(d) && isfinite(d) && fabs(d) < 1e15) { snprintf(nb, sizeof nb, "%lld", (long long)d); s = nb; }
        else { fs = p_to_str_float(d); s = fs; }
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

// --- ML3: Autograd tape runtime (functions only - data structures defined above) ---

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
    // No gradient - return zero tensor of same shape
    PTensor* pt = (PTensor*)t;
    if (!pt) return NULL;
    return (void*)ag_zeros_like(pt);
}
void p_grad_zero(void) {
    ag_grad_len = 0;
}
bool p_requires_grad(void* t) { return ag_mode != 0; }
void p_set_checkpoint(int64_t v) { ag_checkpoint_mode = v; }
void* p_get_grad(void* t) { return p_grad_get(t); }
int64_t p_grad_count(void) { return ag_grad_len; }
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

// ML13: Tensor parallelism builtins (single-rank: identity/no-ops)
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

// --- ML9: KV Cache runtime ---

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
// --- ML10: Quantization runtime ---

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
    for (int64_t i = 0; i < m; i++) for (int64_t j = 0; j < n; j++) { double s = 0; for (int64_t x = 0; x < k; x++) s += ta->data[i*k+x] * tb->data[x*n+j]; r->data[i*n+j] = s; }
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

// --- JSON builtins (Phase 5D) - simple JSON implementation ---

// JSON builtins - simple key-value store
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
            // Found the element - extract it
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

// --- HTTP builtins (Phase 5D) ---

int64_t p_http_listen(int64_t port) { return p_socket_listen(port); }
char* p_http_recv_request(int64_t fd) { return p_socket_recv_str(fd); }
static const char* p_http_reason(int64_t status) {
    switch(status) {
        case 100: return "Continue";
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 304: return "Not Modified";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        default: return "OK";
    }
}
void p_http_send_response(int64_t fd, int64_t status, const char* body) {
    char header[256];
    snprintf(header, sizeof(header), "HTTP/1.1 %lld %s\r\nContent-Type: text/plain\r\nContent-Length: %zu\r\n\r\n", (long long)status, p_http_reason(status), strlen(body));
    p_socket_write(fd, header, strlen(header));
    p_socket_write(fd, body, strlen(body));
}
char* p_http_build_response(int64_t status, const char* content_type, const char* body) {
    (void)content_type;
    size_t blen = strlen(body);
    char* resp = (char*)malloc(blen + 256);
    snprintf(resp, blen+256, "HTTP/1.1 %lld %s\r\nContent-Length: %zu\r\n\r\n%s", (long long)status, p_http_reason(status), blen, body);
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

// --- DB builtins (Phase 5D) ---

// DB builtins - in-memory key-value store
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
// --- Memory DB implementation ---
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
    // Check if entity already exists - update value
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
    for (int64_t i = 0; i < v->len; i++) {
        unsigned char b = (unsigned char)(int64_t)v->data[i];
        fwrite(&b, 1, 1, f);
    }
    fclose(f); return 0;
}
const __p_fn_entry __p_fns[] = {
    {(const void*)Option_is_some, "Option_is_some"},
    {(const void*)Option_is_none, "Option_is_none"},
    {(const void*)Option_unwrap, "Option_unwrap"},
    {(const void*)Option_unwrap_or, "Option_unwrap_or"},
    {(const void*)Option_map, "Option_map"},
    {(const void*)Option_and_then, "Option_and_then"},
    {(const void*)Option_or_else, "Option_or_else"},
    {(const void*)Option_unwrap_or_else, "Option_unwrap_or_else"},
    {(const void*)Option_expect, "Option_expect"},
    {(const void*)Result_is_ok, "Result_is_ok"},
    {(const void*)Result_is_err, "Result_is_err"},
    {(const void*)Result_unwrap, "Result_unwrap"},
    {(const void*)Result_unwrap_or, "Result_unwrap_or"},
    {(const void*)Result_ok, "Result_ok"},
    {(const void*)Result_err, "Result_err"},
    {(const void*)Result_map, "Result_map"},
    {(const void*)Result_map_err, "Result_map_err"},
    {(const void*)Result_and_then, "Result_and_then"},
    {(const void*)Result_unwrap_or_else, "Result_unwrap_or_else"},
    {(const void*)Result_expect, "Result_expect"},
    {(const void*)Result_context, "Result_context"},
    {(const void*)Result_with_context, "Result_with_context"},
    {(const void*)Option_context, "Option_context"},
    {(const void*)StringBuilder_new, "StringBuilder_new"},
    {(const void*)StringBuilder_append_str, "StringBuilder_append_str"},
    {(const void*)StringBuilder_append_char, "StringBuilder_append_char"},
    {(const void*)StringBuilder_append_int, "StringBuilder_append_int"},
    {(const void*)StringBuilder_to_str, "StringBuilder_to_str"},
    {(const void*)StringBuilder_len, "StringBuilder_len"},
    {(const void*)StringBuilder_is_empty, "StringBuilder_is_empty"},
    {(const void*)StringBuilder_clear, "StringBuilder_clear"},
    {(const void*)ByteBuffer_new, "ByteBuffer_new"},
    {(const void*)ByteBuffer_from_string, "ByteBuffer_from_string"},
    {(const void*)ByteBuffer_write_byte, "ByteBuffer_write_byte"},
    {(const void*)ByteBuffer_write_bytes, "ByteBuffer_write_bytes"},
    {(const void*)ByteBuffer_read_byte, "ByteBuffer_read_byte"},
    {(const void*)ByteBuffer_set_byte, "ByteBuffer_set_byte"},
    {(const void*)ByteBuffer_len, "ByteBuffer_len"},
    {(const void*)ByteBuffer_is_empty, "ByteBuffer_is_empty"},
    {(const void*)ByteBuffer_clear, "ByteBuffer_clear"},
    {(const void*)ByteBuffer_to_str, "ByteBuffer_to_str"},
    {(const void*)parse_port, "parse_port"},
    {(const void*)load_port, "load_port"},
    {(const void*)find, "find"},
    {(const void*)where_is, "where_is"},
    {(const void*)label, "label"},
    {(const void*)show, "show"},
    {(const void*)main_entry, "main"},
    {(const void*)file_read, "file_read"},
    {(const void*)file_read_result, "file_read_result"},
    {(const void*)file_write, "file_write"},
    {(const void*)file_append, "file_append"},
    {(const void*)file_exists, "file_exists"},
    {(const void*)file_write_bytes, "file_write_bytes"},
    {(const void*)file_read_bytes, "file_read_bytes"},
    {(const void*)file_write_raw, "file_write_raw"},
    {(const void*)file_delete, "file_delete"},
    {(const void*)file_size, "file_size"},
    {(const void*)fs_read_text, "fs_read_text"},
    {(const void*)fs_write_text, "fs_write_text"},
    {(const void*)fs_append_text, "fs_append_text"},
    {(const void*)fs_exists, "fs_exists"},
    {(const void*)fs_delete, "fs_delete"},
    {(const void*)fs_size, "fs_size"},
    {(const void*)println, "println"},
    {(const void*)assert, "assert"},
    {(const void*)assert_msg, "assert_msg"},
    {(const void*)panic, "panic"},
    {(const void*)p_max, "max"},
    {(const void*)p_min, "min"},
    {(const void*)p_abs, "abs"},
    {(const void*)clamp, "clamp"},
    {(const void*)create_allocator, "create_allocator"},
    {(const void*)allocator_alloc, "allocator_alloc"},
    {(const void*)allocator_free, "allocator_free"},
    {(const void*)allocator_bytes_used, "allocator_bytes_used"},
    {(const void*)Vec_new, "Vec_new"},
    {(const void*)Vec_drop, "Vec_drop"},
    {(const void*)Str_drop, "Str_drop"},
    {(const void*)Vec_push, "Vec_push"},
    {(const void*)Vec_pop, "Vec_pop"},
    {(const void*)Vec_len, "Vec_len"},
    {(const void*)Vec_is_empty, "Vec_is_empty"},
    {(const void*)Vec_get, "Vec_get"},
    {(const void*)vec_get_or, "vec_get_or"},
    {(const void*)Vec_set, "Vec_set"},
    {(const void*)Vec_clear, "Vec_clear"},
    {(const void*)Vec_capacity, "Vec_capacity"},
    {(const void*)Vec_contains, "Vec_contains"},
    {(const void*)Vec_first, "Vec_first"},
    {(const void*)Vec_last, "Vec_last"},
    {(const void*)Vec_truncate, "Vec_truncate"},
    {(const void*)Vec_reverse, "Vec_reverse"},
    {(const void*)vec_raw_reverse, "vec_raw_reverse"},
    {(const void*)Vec_extend, "Vec_extend"},
    {(const void*)vec_raw_extend, "vec_raw_extend"},
    {(const void*)Vec_insert, "Vec_insert"},
    {(const void*)vec_raw_insert, "vec_raw_insert"},
    {(const void*)Vec_remove, "Vec_remove"},
    {(const void*)vec_raw_remove, "vec_raw_remove"},
    {(const void*)Vec_clone, "Vec_clone"},
    {(const void*)Vec_to_str, "Vec_to_str"},
    {(const void*)Vec_sort, "Vec_sort"},
    {(const void*)vec_raw_sort, "vec_raw_sort"},
    {(const void*)vec_slot, "vec_slot"},
    {(const void*)vec_swap_slots, "vec_swap_slots"},
    {(const void*)vec_sift_down, "vec_sift_down"},
    {(const void*)p_str_hash, "p_str_hash"},
    {(const void*)p_str_equal, "p_str_equal"},
    {(const void*)p_str_compare, "p_str_compare"},
    {(const void*)HashMap_new, "HashMap_new"},
    {(const void*)HashMap_find_slot, "HashMap_find_slot"},
    {(const void*)HashMap_grow, "HashMap_grow"},
    {(const void*)HashMap_insert, "HashMap_insert"},
    {(const void*)HashMap_get, "HashMap_get"},
    {(const void*)HashMap_contains, "HashMap_contains"},
    {(const void*)HashMap_remove, "HashMap_remove"},
    {(const void*)HashMap_len, "HashMap_len"},
    {(const void*)HashMap_is_empty, "HashMap_is_empty"},
    {(const void*)HashMap_clear, "HashMap_clear"},
    {(const void*)HashMap_keys, "HashMap_keys"},
    {(const void*)HashMap_values, "HashMap_values"},
    {(const void*)HashMap_clone, "HashMap_clone"},
    {(const void*)HashMap_get_or, "HashMap_get_or"},
    {(const void*)HashMap_to_str, "HashMap_to_str"},
    {(const void*)p_str_from_ptr, "p_str_from_ptr"},
    {(const void*)p_str_length, "p_str_length"},
    {(const void*)p_str_copy_n, "p_str_copy_n"},
    {(const void*)String_from, "String_from"},
    {(const void*)String_len, "String_len"},
    {(const void*)String_is_empty, "String_is_empty"},
    {(const void*)String_push_str, "String_push_str"},
    {(const void*)String_contains, "String_contains"},
    {(const void*)String_starts_with, "String_starts_with"},
    {(const void*)String_ends_with, "String_ends_with"},
    {(const void*)String_trim, "String_trim"},
    {(const void*)String_to_uppercase, "String_to_uppercase"},
    {(const void*)String_to_lowercase, "String_to_lowercase"},
    {(const void*)String_slice, "String_slice"},
    {(const void*)String_slice_len, "String_slice_len"},
    {(const void*)String_find, "String_find"},
    {(const void*)String_rfind, "String_rfind"},
    {(const void*)String_replace, "String_replace"},
    {(const void*)String_split, "String_split"},
    {(const void*)String_parse_int, "String_parse_int"},
    {(const void*)String_char_at, "String_char_at"},
    {(const void*)String_repeat, "String_repeat"},
    {(const void*)String_to_string, "String_to_string"},
    {(const void*)String_clone, "String_clone"},
    {(const void*)String_bytes, "String_bytes"},
    {(const void*)String_byte_at, "String_byte_at"},
    {(const void*)String_char_count, "String_char_count"},
    {(const void*)char_to_str, "char_to_str"},
    {(const void*)char_is_digit, "char_is_digit"},
    {(const void*)char_is_alpha, "char_is_alpha"},
    {(const void*)char_is_whitespace, "char_is_whitespace"},
    {(const void*)char_is_upper, "char_is_upper"},
    {(const void*)char_is_lower, "char_is_lower"},
    {(const void*)char_to_upper, "char_to_upper"},
    {(const void*)char_to_lower, "char_to_lower"},
    {(const void*)str_bytes, "str_bytes"},
    {(const void*)str_from_bytes, "str_from_bytes"},
    {(const void*)str_slice, "str_slice"},
    {(const void*)str_slice_len, "str_slice_len"},
    {(const void*)str_starts_with, "str_starts_with"},
    {(const void*)str_ends_with, "str_ends_with"},
    {(const void*)str_contains, "str_contains"},
    {(const void*)str_find, "str_find"},
    {(const void*)str_rfind, "str_rfind"},
    {(const void*)str_trim_start, "str_trim_start"},
    {(const void*)str_trim_end, "str_trim_end"},
    {(const void*)str_trim, "str_trim"},
    {(const void*)str_lines, "str_lines"},
    {(const void*)str_trim_end_char, "str_trim_end_char"},
    {(const void*)str_split, "str_split"},
    {(const void*)str_split_char, "str_split_char"},
    {(const void*)str_replace, "str_replace"},
    {(const void*)str_repeat, "str_repeat"},
    {(const void*)str_to_upper, "str_to_upper"},
    {(const void*)str_to_lower, "str_to_lower"},
    {(const void*)str_parse_int, "str_parse_int"},
    {(const void*)str_parse_hex, "str_parse_hex"},
    {(const void*)str_parse_int_range, "str_parse_int_range"},
    {(const void*)str_byte_at, "str_byte_at"},
    {(const void*)str_len, "str_len"},
    {(const void*)str_is_empty, "str_is_empty"},
    {(const void*)str_concat, "str_concat"},
    {(const void*)str_compare, "str_compare"},
    {(const void*)str_equal, "str_equal"},
    {(const void*)int_to_str, "int_to_str"},
    {(const void*)u64_to_str, "u64_to_str"},
    {(const void*)float_to_str, "float_to_str"},
    {(const void*)str_parse_float, "str_parse_float"},
    {(const void*)byte_to_str, "byte_to_str"},
};
const int __p_fn_count = 194;
