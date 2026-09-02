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
double p_str_to_float(const char* s);
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
int64_t p_clock_now(void);
int64_t p_clock_now_us(void);
int64_t p_clock_now_ms(void);
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
static const char _str0[] = "FAIL: ";
static const char _str1[] = " — expected '";
static const char _str2[] = "', got '";
static const char _str3[] = "'\n";
static const char _str4[] = "test failed";
static const char _str5[] = "";
static const char _str6[] = " — expected ";
static const char _str7[] = "true";
static const char _str8[] = "false";
static const char _str9[] = ", got ";
static const char _str10[] = "\n";
static const char _str11[] = "{";
static const char _str12[] = "}";
static const char _str13[] = ", ";
static const char _str14[] = ": ";
static const char _str15[] = "0";
static const char _str16[] = "0123456789abcdef";
static const char _str17[] = "SH-7.2: AST module test\n";
static const char _str18[] = "========================\n\n";
static const char _str19[] = "Test 1: Primitive type_to_str...\n";
static const char _str20[] = "i8";
static const char _str21[] = "I8";
static const char _str22[] = "i16";
static const char _str23[] = "I16";
static const char _str24[] = "i32";
static const char _str25[] = "I32";
static const char _str26[] = "i64";
static const char _str27[] = "I64";
static const char _str28[] = "i128";
static const char _str29[] = "I128";
static const char _str30[] = "u8";
static const char _str31[] = "U8";
static const char _str32[] = "u16";
static const char _str33[] = "U16";
static const char _str34[] = "u32";
static const char _str35[] = "U32";
static const char _str36[] = "u64";
static const char _str37[] = "U64";
static const char _str38[] = "u128";
static const char _str39[] = "U128";
static const char _str40[] = "f32";
static const char _str41[] = "F32";
static const char _str42[] = "f64";
static const char _str43[] = "F64";
static const char _str44[] = "bool";
static const char _str45[] = "Bool";
static const char _str46[] = "str";
static const char _str47[] = "Str";
static const char _str48[] = "void";
static const char _str49[] = "Void";
static const char _str50[] = "never";
static const char _str51[] = "Never";
static const char _str52[] = "_";
static const char _str53[] = "Infer";
static const char _str54[] = "  OK\n\n";
static const char _str55[] = "Test 2: Named/compound type_to_str...\n";
static const char _str56[] = "Point";
static const char _str57[] = "Named(Point)";
static const char _str58[] = "HashMap";
static const char _str59[] = "Named(HashMap)";
static const char _str60[] = "[i64; 10]";
static const char _str61[] = "Array";
static const char _str62[] = "[f64; 0]";
static const char _str63[] = "Array(0)";
static const char _str64[] = "T";
static const char _str65[] = "N";
static const char _str66[] = "[T; N]";
static const char _str67[] = "ArrayConst";
static const char _str68[] = "&i64";
static const char _str69[] = "Ref(shared)";
static const char _str70[] = "&mut str";
static const char _str71[] = "Ref(mut)";
static const char _str72[] = "*mut T";
static const char _str73[] = "RawPtr(mut)";
static const char _str74[] = "*const u8";
static const char _str75[] = "RawPtr(const)";
static const char _str76[] = "Test 3: Generic/Tuple/Func/Assoc/Dyn...\n";
static const char _str77[] = "Vec";
static const char _str78[] = "Vec<i64>";
static const char _str79[] = "Generic(Vec<i64>)";
static const char _str80[] = "HashMap<str, i64>";
static const char _str81[] = "Generic(HashMap)";
static const char _str82[] = "Option";
static const char _str83[] = "Option<>";
static const char _str84[] = "Generic(empty)";
static const char _str85[] = "(i64, str)";
static const char _str86[] = "Tuple(2)";
static const char _str87[] = "()";
static const char _str88[] = "Tuple(0)";
static const char _str89[] = "fn(i64, str) -> bool";
static const char _str90[] = "Func";
static const char _str91[] = "fn() -> void";
static const char _str92[] = "Func(noargs)";
static const char _str93[] = "Iterator";
static const char _str94[] = "Item";
static const char _str95[] = "T::Item(Iterator)";
static const char _str96[] = "Assoc(noargs)";
static const char _str97[] = "T::Item<i64>(Iterator)";
static const char _str98[] = "Assoc(args)";
static const char _str99[] = "Display";
static const char _str100[] = "dyn Display";
static const char _str101[] = "Dyn";
static const char _str102[] = "Test 4: ShapedTensor type_to_str...\n";
static const char _str103[] = "Tensor<f32, [784, 128]>";
static const char _str104[] = "ShapedTensor(known)";
static const char _str105[] = "batch";
static const char _str106[] = "Tensor<f32, [batch, 784]>";
static const char _str107[] = "ShapedTensor(symbolic)";
static const char _str108[] = "Tensor<f64>";
static const char _str109[] = "ShapedTensor(empty)";
static const char _str110[] = "Test 5: shape_dim_to_str...\n";
static const char _str111[] = "42";
static const char _str112[] = "Known(42)";
static const char _str113[] = "Known(0)";
static const char _str114[] = "seq_len";
static const char _str115[] = "Symbolic(seq_len)";
static const char _str116[] = "Test 6: Recursive Box<Expr>...\n";
static const char _str117[] = "Test 7: Deeply nested Box<Expr>...\n";
static const char _str118[] = "node count (3 internal + 4 leaves)";
static const char _str119[] = "Test 8: Expr variant payloads...\n";
static const char _str120[] = "hello";
static const char _str121[] = "Add tag";
static const char _str122[] = "left operand";
static const char _str123[] = "expected IntLit left";
static const char _str124[] = "right operand";
static const char _str125[] = "expected IntLit right";
static const char _str126[] = "expected Binary";
static const char _str127[] = "StringLit payload";
static const char _str128[] = "expected StringLit";
static const char _str129[] = "BoolLit payload";
static const char _str130[] = "expected BoolLit";
static const char _str131[] = "foo";
static const char _str132[] = "FloatLit payload mismatch";
static const char _str133[] = "expected FloatLit";
static const char _str134[] = "add";
static const char _str135[] = "Ident payload";
static const char _str136[] = "expected Ident";
static const char _str137[] = "Test 9: Field/Method/Index Expr...\n";
static const char _str138[] = "p";
static const char _str139[] = "x";
static const char _str140[] = "Call name";
static const char _str141[] = "Call arg count";
static const char _str142[] = "expected Call";
static const char _str143[] = "v";
static const char _str144[] = "push";
static const char _str145[] = "FieldAccess field";
static const char _str146[] = "FieldAccess receiver";
static const char _str147[] = "expected Ident receiver";
static const char _str148[] = "expected FieldAccess";
static const char _str149[] = "arr";
static const char _str150[] = "MethodCall method";
static const char _str151[] = "MethodCall arg count";
static const char _str152[] = "MethodCall receiver";
static const char _str153[] = "expected MethodCall";
static const char _str154[] = "Test 10: StructLit...\n";
static const char _str155[] = "y";
static const char _str156[] = "Index index";
static const char _str157[] = "expected IntLit index";
static const char _str158[] = "Index array";
static const char _str159[] = "expected Ident array";
static const char _str160[] = "expected Index";
static const char _str161[] = "Test 11: Stmt + stmt_line...\n";
static const char _str162[] = "Let line";
static const char _str163[] = "Assign line";
static const char _str164[] = "Return line";
static const char _str165[] = "Return(none) line";
static const char _str166[] = "Continue line";
static const char _str167[] = "Expr stmt line";
static const char _str168[] = "While line";
static const char _str169[] = "i";
static const char _str170[] = "range";
static const char _str171[] = "For line";
static const char _str172[] = "Loop line";
static const char _str173[] = "Break line";
static const char _str174[] = "Defer line";
static const char _str175[] = "core";
static const char _str176[] = "Use line";
static const char _str177[] = "Sandbox line";
static const char _str178[] = "Unsafe line";
static const char _str179[] = "Txn line";
static const char _str180[] = "r";
static const char _str181[] = "RegionBlock line";
static const char _str182[] = "GradBlock line";
static const char _str183[] = "loss";
static const char _str184[] = "Backward line";
static const char _str185[] = "Position";
static const char _str186[] = "Query line";
static const char _str187[] = "Test 12: More Stmt variants...\n";
static const char _str188[] = "a";
static const char _str189[] = "b";
static const char _str190[] = "pair";
static const char _str191[] = "LetTuple line";
static const char _str192[] = "LetUninit line";
static const char _str193[] = "IndexAssign line";
static const char _str194[] = "ptr";
static const char _str195[] = "DerefAssign line";
static const char _str196[] = "FieldAssign line";
static const char _str197[] = "If line";
static const char _str198[] = "Match line";
static const char _str199[] = "Handle line";
static const char _str200[] = "IO";
static const char _str201[] = "print";
static const char _str202[] = "Do line";
static const char _str203[] = "ComptimeGen line";
static const char _str204[] = "Select line";
static const char _str205[] = "Test 13: Pattern variants...\n";
static const char _str206[] = "StructLit name";
static const char _str207[] = "StructLit field count";
static const char _str208[] = "StructLit field0 name";
static const char _str209[] = "expected None spread";
static const char _str210[] = "StructLit field0 value";
static const char _str211[] = "expected IntLit field0";
static const char _str212[] = "expected StructLit";
static const char _str213[] = "expected Wildcard";
static const char _str214[] = "Pattern IntLit";
static const char _str215[] = "expected IntLit pattern";
static const char _str216[] = "Pattern BoolLit";
static const char _str217[] = "expected BoolLit pattern";
static const char _str218[] = "Pattern Ident";
static const char _str219[] = "expected Ident pattern";
static const char _str220[] = "value";
static const char _str221[] = "Some";
static const char _str222[] = "Pattern StrLit";
static const char _str223[] = "expected StrLit pattern";
static const char _str224[] = "Pattern EnumVariant enum";
static const char _str225[] = "Pattern EnumVariant variant";
static const char _str226[] = "Pattern EnumVariant binding count";
static const char _str227[] = "Pattern binding name";
static const char _str228[] = "Pattern binding ty";
static const char _str229[] = "expected EnumVariant pattern";
static const char _str230[] = "Test 14: MatchArm...\n";
static const char _str231[] = "Pattern Range start";
static const char _str232[] = "expected IntLit range start";
static const char _str233[] = "Pattern Range end";
static const char _str234[] = "expected IntLit range end";
static const char _str235[] = "expected Range pattern";
static const char _str236[] = "arm pattern";
static const char _str237[] = "arm body len";
static const char _str238[] = "arm guard";
static const char _str239[] = "Test 15: Struct construction...\n";
static const char _str240[] = "Param name";
static const char _str241[] = "Param ty";
static const char _str242[] = "Param is_pub";
static const char _str243[] = "kernel";
static const char _str244[] = "Annotation name";
static const char _str245[] = "StructDef name";
static const char _str246[] = "StructDef is_pub";
static const char _str247[] = "None";
static const char _str248[] = "EnumDef name";
static const char _str249[] = "EnumDef variant count";
static const char _str250[] = "EnumDef variant1 name";
static const char _str251[] = "arm guard expr";
static const char _str252[] = "expected Ident guard";
static const char _str253[] = "expected Some guard";
static const char _str254[] = "main";
static const char _str255[] = "Func name";
static const char _str256[] = "Func ret_ty";
static const char _str257[] = "Func is_async";
static const char _str258[] = "Test 16: Program...\n";
static const char _str259[] = "expected None data";
static const char _str260[] = "Program funcs empty";
static const char _str261[] = "Program structs empty";
static const char _str262[] = "Program enums empty";
static const char _str263[] = "Program consts empty";
static const char _str264[] = "Program imports empty";
static const char _str265[] = "Program traits empty";
static const char _str266[] = "Program trait_impls empty";
static const char _str267[] = "Program extern_decls empty";
static const char _str268[] = "Program layers empty";
static const char _str269[] = "Program models empty";
static const char _str270[] = "Program components empty";
static const char _str271[] = "Program systems empty";
static const char _str272[] = "Program state_machines empty";
static const char _str273[] = "Program event_handlers empty";
static const char _str274[] = "Program funcs pushed";
static const char _str275[] = "Test 17: Optional wrappers...\n";
static const char _str276[] = "expected None module";
static const char _str277[] = "expected OptBoxExpr::None";
static const char _str278[] = "OptBoxExpr payload";
static const char _str279[] = "expected IntLit in OptBoxExpr";
static const char _str280[] = "expected OptBoxExpr::Some";
static const char _str281[] = "expected OptType::None";
static const char _str282[] = "OptType payload";
static const char _str283[] = "expected OptType::Some";
static const char _str284[] = "expected OptStmts::None";
static const char _str285[] = "OptStmts payload";
static const char _str286[] = "expected OptStmts::Some";
static const char _str287[] = "expected OptI64::None";
static const char _str288[] = "OptI64 payload";
static const char _str289[] = "expected OptI64::Some";
static const char _str290[] = "hi";
static const char _str291[] = "expected OptStr::None";
static const char _str292[] = "OptStr payload";
static const char _str293[] = "expected OptStr::Some";
static const char _str294[] = "expected OptParams::None";
static const char _str295[] = "Test 18: Helper structs...\n";
static const char _str296[] = "FieldInit name";
static const char _str297[] = "OptParams payload";
static const char _str298[] = "expected OptParams::Some";
static const char _str299[] = "Printable";
static const char _str300[] = "StrPair a";
static const char _str301[] = "StrPair b";
static const char _str302[] = "AssocTypeDecl name";
static const char _str303[] = "AssocTypeBinding name";
static const char _str304[] = "AssocTypeBinding ty";
static const char _str305[] = "784";
static const char _str306[] = "128";
static const char _str307[] = "l1";
static const char _str308[] = "Dense";
static const char _str309[] = "LayerInst field";
static const char _str310[] = "LayerInst layer_type";
static const char _str311[] = "LayerInst args";
static const char _str312[] = "Binding name";
static const char _str313[] = "Binding ty";
static const char _str314[] = "Test 19: SelectCase/EffectHandler/AsmOperand...\n";
static const char _str315[] = "ch";
static const char _str316[] = "SelectCase var_name";
static const char _str317[] = "msg";
static const char _str318[] = "EffectHandler effect";
static const char _str319[] = "EffectHandler op";
static const char _str320[] = "EffectHandler params";
static const char _str321[] = "rax";
static const char _str322[] = "AsmOperand reg";
static const char _str323[] = "FieldInit value";
static const char _str324[] = "expected IntLit FieldInit value";
static const char _str325[] = "Test 20: Trait/Impl structs...\n";
static const char _str326[] = "fmt";
static const char _str327[] = "TraitMethod name";
static const char _str328[] = "TraitMethod has_self";
static const char _str329[] = "TraitMethod ret_ty";
static const char _str330[] = "TraitDef name";
static const char _str331[] = "TraitDef methods";
static const char _str332[] = "TraitImpl trait_name";
static const char _str333[] = "TraitImpl type_name";
static const char _str334[] = "ImplDef struct_name";
static const char _str335[] = "Test 21: Misc structs...\n";
static const char _str336[] = "PI";
static const char _str337[] = "ConstDef name";
static const char _str338[] = "ConstDef ty";
static const char _str339[] = "Int";
static const char _str340[] = "TypeAlias name";
static const char _str341[] = "TypeAlias alias_for";
static const char _str342[] = "ImportDef path";
static const char _str343[] = "ImportDef is_glob";
static const char _str344[] = "no-alloc";
static const char _str345[] = "Attribute name";
static const char _str346[] = "C";
static const char _str347[] = "printf";
static const char _str348[] = "ExternDecl abi";
static const char _str349[] = "ExternDecl c_name";
static const char _str350[] = "ExternDecl ret_ty";
static const char _str351[] = "Test 22: Game architecture structs...\n";
static const char _str352[] = "ComponentDef name";
static const char _str353[] = "Move";
static const char _str354[] = "SystemDef name";
static const char _str355[] = "PlayerState";
static const char _str356[] = "StateMachineDef name";
static const char _str357[] = "Idle";
static const char _str358[] = "StateDef name";
static const char _str359[] = "move";
static const char _str360[] = "Running";
static const char _str361[] = "StateTransition event";
static const char _str362[] = "StateTransition target";
static const char _str363[] = "click";
static const char _str364[] = "EventHandlerDef event_name";
static const char _str365[] = "Test 23: Layer/Model structs...\n";
static const char _str366[] = "LayerDef name";
static const char _str367[] = "MLP";
static const char _str368[] = "ModelDef name";
static const char _str369[] = "ModelDef layers";
static const char _str370[] = "Test 24: Operator enums...\n";
static const char _str371[] = "expected AsmDir::In";
static const char _str372[] = "BinOp::Add";
static const char _str373[] = "BinOp::Shr";
static const char _str374[] = "UnaryOp::Neg";
static const char _str375[] = "UnaryOp::BitNot";
static const char _str376[] = "AssignOp::AddAssign";
static const char _str377[] = "AssignOp::ShrAssign";
static const char _str378[] = "ReflectOp::TypeName";
static const char _str379[] = "ReflectOp::EnumVariants";
static const char _str380[] = "AsmDir::Out";
static const char _str381[] = "FStringPart::Literal";
static const char _str382[] = "Test 25: Func with bounds/const_params...\n";
static const char _str383[] = "generic_fn";
static const char _str384[] = "U";
static const char _str385[] = "Func type_param_bounds";
static const char _str386[] = "bound0 a";
static const char _str387[] = "bound0 b";
static const char _str388[] = "Func const_params";
static const char _str389[] = "cparam0 a";
static const char _str390[] = "cparam0 b";
static const char _str391[] = "Func has_effect_annotation";
static const char _str392[] = "SH-7.2: All tests passed!\n";
static const char _str393[] = "FStringPart::Expr";
static const char _str394[] = "FStringPart::Expr inner";
static const char _str395[] = "[";
static const char _str396[] = "]";
static const char _str397[] = "called unwrap on None";
static const char _str398[] = "called unwrap on Err";
static const char _str399[] = "Assertion failed";
static const char _str400[] = "; ";
static const char _str401[] = "&mut ";
static const char _str402[] = "&";
static const char _str403[] = "*mut ";
static const char _str404[] = "*const ";
static const char _str405[] = "<";
static const char _str406[] = ">";
static const char _str407[] = "(";
static const char _str408[] = ")";
static const char _str409[] = "fn(";
static const char _str410[] = ") -> ";
static const char _str411[] = "::";
static const char _str412[] = ">(";
static const char _str413[] = "dyn ";
static const char _str414[] = "Tensor<";
static const char _str415[] = ", [";

// ─── Struct definitions ───
struct ReflectOp_s {
    int64_t field_0;
};

struct OptBoxExpr_s {
    int64_t field_0;
    int64_t field_1;
};

struct SelectCase_s {
    struct Expr_s* field_0;
    void* field_1;
    void* field_2;
};

struct ConstDef_s {
    void* field_0;
    struct Type_s* field_1;
    struct Expr_s* field_2;
    bool field_3;
};

struct StateMachineDef_s {
    void* field_0;
    void* field_1;
    bool field_2;
};

struct Program_s {
    struct OptStr_s* field_0;
    void* field_1;
    void* field_2;
    void* field_3;
    void* field_4;
    void* field_5;
    void* field_6;
    void* field_7;
    void* field_8;
    void* field_9;
    void* field_10;
    void* field_11;
    void* field_12;
    void* field_13;
    void* field_14;
    void* field_15;
    void* field_16;
    void* field_17;
};

struct FieldInit_s {
    void* field_0;
    struct Expr_s* field_1;
};

struct MatchArm_s {
    struct Pattern_s* field_0;
    struct OptBoxExpr_s* field_1;
    void* field_2;
};

struct Pattern_s {
    int64_t field_0;
    int64_t field_1;
    int64_t field_2;
    int64_t field_3;
};

struct UnaryOp_s {
    int64_t field_0;
};

struct StateTransition_s {
    void* field_0;
    void* field_1;
};

struct TraitImpl_s {
    void* field_0;
    void* field_1;
    void* field_2;
    void* field_3;
    void* field_4;
};

struct Binding_s {
    void* field_0;
    void* field_1;
};

struct ExternDecl_s {
    void* field_0;
    void* field_1;
    void* field_2;
    void* field_3;
    struct Type_s* field_4;
    struct OptStr_s* field_5;
    struct OptStr_s* field_6;
};

struct StateDef_s {
    void* field_0;
    void* field_1;
};

struct StringBuilder_s {
    void* field_0;
    int64_t field_1;
};

struct ShapeDim_s {
    int64_t field_0;
    int64_t field_1;
};

struct Param_s {
    void* field_0;
    struct Type_s* field_1;
    bool field_2;
    void* field_3;
};

struct ByteBuffer_s {
    void* field_0;
    int64_t field_1;
};

struct AssocTypeDecl_s {
    void* field_0;
    void* field_1;
};

struct StrPair_s {
    void* field_0;
    void* field_1;
};

struct BinOp_s {
    int64_t field_0;
};

struct Stmt_s {
    int64_t field_0;
    int64_t field_1;
    int64_t field_2;
    int64_t field_3;
    int64_t field_4;
    int64_t field_5;
    int64_t field_6;
    int64_t field_7;
};

struct EnumVariant_s {
    void* field_0;
    struct OptParams_s* field_1;
};

struct ImplDef_s {
    void* field_0;
    void* field_1;
    void* field_2;
};

struct TypeAlias_s {
    void* field_0;
    struct Type_s* field_1;
    bool field_2;
};

struct Result_s {
    int64_t field_0;
    int64_t field_1;
};

struct AssocTypeBinding_s {
    void* field_0;
    struct Type_s* field_1;
};

struct Type_s {
    int64_t field_0;
    int64_t field_1;
    int64_t field_2;
    int64_t field_3;
    int64_t field_4;
};

struct Option_s {
    int64_t field_0;
    int64_t field_1;
};

struct Func_s {
    void* field_0;
    void* field_1;
    void* field_2;
    void* field_3;
    void* field_4;
    struct Type_s* field_5;
    void* field_6;
    bool field_7;
    bool field_8;
    void* field_9;
    bool field_10;
    bool field_11;
    void* field_12;
    void* field_13;
    void* field_14;
    bool field_15;
};

struct AsmOperand_s {
    struct AsmDir_s* field_0;
    void* field_1;
    struct Expr_s* field_2;
};

struct EffectHandler_s {
    void* field_0;
    void* field_1;
    void* field_2;
    void* field_3;
};

struct ModelDef_s {
    void* field_0;
    void* field_1;
    struct Func_s* field_2;
    bool field_3;
};

struct Attribute_s {
    void* field_0;
    void* field_1;
};

struct ComponentDef_s {
    void* field_0;
    void* field_1;
    bool field_2;
};

struct LayerDef_s {
    void* field_0;
    void* field_1;
    void* field_2;
    struct Func_s* field_3;
    bool field_4;
};

struct AssignOp_s {
    int64_t field_0;
};

struct EnumDef_s {
    void* field_0;
    void* field_1;
    bool field_2;
    void* field_3;
};

struct LayerInst_s {
    void* field_0;
    void* field_1;
    void* field_2;
};

struct OptStmts_s {
    int64_t field_0;
    int64_t field_1;
};

struct ImportDef_s {
    void* field_0;
    void* field_1;
    void* field_2;
    struct OptStr_s* field_3;
    struct OptStr_s* field_4;
    bool field_5;
    bool field_6;
};

struct TraitMethod_s {
    void* field_0;
    void* field_1;
    struct Type_s* field_2;
    bool field_3;
    bool field_4;
    struct OptStmts_s* field_5;
};

struct Vec_s {
    int64_t field_0;
    int64_t field_1;
    int64_t field_2;
};

struct Annotation_s {
    void* field_0;
    void* field_1;
};

struct EventHandlerDef_s {
    void* field_0;
    void* field_1;
    void* field_2;
    bool field_3;
};

struct OptType_s {
    int64_t field_0;
    int64_t field_1;
};

struct SystemDef_s {
    void* field_0;
    void* field_1;
    void* field_2;
    bool field_3;
};

struct Expr_s {
    int64_t field_0;
    int64_t field_1;
    int64_t field_2;
    int64_t field_3;
};

struct OptStr_s {
    int64_t field_0;
    int64_t field_1;
};

struct AsmDir_s {
    int64_t field_0;
};

struct OptI64_s {
    int64_t field_0;
    int64_t field_1;
};

struct OptFunc_s {
    int64_t field_0;
    int64_t field_1;
};

struct StructDef_s {
    void* field_0;
    void* field_1;
    bool field_2;
    void* field_3;
    void* field_4;
};

struct TraitDef_s {
    void* field_0;
    void* field_1;
    void* field_2;
    bool field_3;
};

struct OptParams_s {
    int64_t field_0;
    int64_t field_1;
};

struct FStringPart_s {
    int64_t field_0;
    int64_t field_1;
};

// ─── Function prototypes ───
void p_thread_register(const char* name, int64_t (*fn)());
int64_t String_char_count(void* v0, int64_t v1);
void assert_eq(void* v0, void* v1, void* v2, void* v3);
int64_t String_find(void* v0, int64_t v1, int64_t v2);
int64_t String_split(void* v0, int64_t v1, int64_t v2);
void* float_to_str(void* v0, double v1);
bool Vec_any(void* v0, void* v1, void* v2);
void ByteBuffer_write_bytes(void* v0, void* v1, int64_t v2, int64_t v3);
void Vec_reverse(void* v0, void* v1);
void* Vec_clone(void* v0, void* v1);
int64_t Result_expect(void* v0, void* v1, void* v2);
int64_t p_max(void* v0, int64_t v1, int64_t v2);
void vec_dedup(void* v0, int64_t v1);
int64_t Vec_fold(void* v0, void* v1, int64_t v2, void* v3);
void* str_title(void* v0, void* v1);
int64_t HashMap_values(void* v0, int64_t v1);
bool HashMap_is_empty(void* v0, int64_t v1);
bool str_is_numeric(void* v0, void* v1);
bool str_equal(void* v0, void* v1, void* v2);
void* Vec_take(void* v0, void* v1, int64_t v2);
int64_t Option_unwrap_or(void* v0, void* v1, int64_t v2);
int64_t Vec_last(void* v0, void* v1);
void AssocTypeDecl_new(void* v0, void* v1, void* v2, void* v3);
void* str_bytes(void* v0, void* v1);
void* str_slice(void* v0, void* v1, int64_t v2, int64_t v3);
void FieldInit_new(void* v0, void* v1, void* v2, void* v3);
int64_t String_char_at(void* v0, int64_t v1, int64_t v2);
void* str_trim(void* v0, void* v1);
bool Result_is_err(void* v0, void* v1);
int64_t p_abs(void* v0, int64_t v1);
void AsmOperand_new(void* v0, void* v1, void* v2, void* v3, void* v4);
bool HashMap_contains(void* v0, int64_t v1, int64_t v2);
void* Vec_zip(void* v0, void* v1, void* v2);
void* str_trim_end(void* v0, void* v1);
void assert_eq_bool(void* v0, bool v1, bool v2, void* v3);
void EnumVariant_new(void* v0, void* v1, void* v2, void* v3);
bool String_ends_with(void* v0, int64_t v1, int64_t v2);
bool str_is_alphanumeric(void* v0, void* v1);
void println(void* v0, void* v1);
int64_t str_rfind(void* v0, void* v1, void* v2);
int64_t vec_take(void* v0, int64_t v1, int64_t v2);
void* create_allocator(void* v0, void* v1);
int64_t char_to_lower(void* v0, int64_t v1);
int64_t vec_enumerate(void* v0, int64_t v1);
void ConstDef_new(void* v0, void* v1, void* v2, void* v3, void* v4, bool v5);
int64_t Vec_capacity(void* v0, void* v1);
void vec_sort_by(void* v0, int64_t v1, void* v2);
void AssocTypeBinding_new(void* v0, void* v1, void* v2, void* v3);
void* type_to_str(void* v0, void* v1);
int64_t String_len(void* v0, int64_t v1);
int64_t str_count_char(void* v0, void* v1, int64_t v2);
void* char_to_str(void* v0, int64_t v1);
void StringBuilder_append_str(void* v0, void* v1, void* v2);
void Vec_extend(void* v0, void* v1, void* v2);
bool str_starts_with(void* v0, void* v1, void* v2);
void ByteBuffer_clear(void* v0, void* v1);
int64_t vec_chunk(void* v0, int64_t v1, int64_t v2);
void* str_reverse(void* v0, void* v1);
int64_t String_bytes(void* v0, int64_t v1);
void StateMachineDef_new(void* v0, void* v1, void* v2, bool v3);
void Func_new(void* v0, void* v1, void* v2, void* v3);
int64_t str_byte_at(void* v0, void* v1, int64_t v2);
void* str_trim_start(void* v0, void* v1);
void* str_to_lower(void* v0, void* v1);
void* str_repeat(void* v0, void* v1, int64_t v2);
bool ByteBuffer_is_empty(void* v0, void* v1);
int64_t p_str_length(void* v0, int64_t v1);
void Option_and_then(void* v0, void* v1, void* v2, void* v3);
void HashMap_grow(void* v0, int64_t v1);
int64_t String_rfind(void* v0, int64_t v1, int64_t v2);
int64_t Vec_remove(void* v0, void* v1, int64_t v2);
void* HashMap_to_str(void* v0, int64_t v1);
int64_t str_count(void* v0, void* v1, void* v2);
void* str_concat(void* v0, void* v1, void* v2);
void Vec_dedup(void* v0, void* v1);
void* Vec_new(void* v0);
void Result_ok(void* v0, void* v1, void* v2);
bool String_is_empty(void* v0, int64_t v1);
int64_t Vec_pop(void* v0, void* v1);
void StructDef_new(void* v0, void* v1, void* v2, bool v3);
void StateTransition_new(void* v0, void* v1, void* v2, void* v3);
int64_t String_clone(void* v0, int64_t v1);
int64_t char_to_upper(void* v0, int64_t v1);
void SystemDef_new(void* v0, void* v1, void* v2);
void* str_pad_left(void* v0, void* v1, int64_t v2, int64_t v3);
int64_t allocator_alloc(void* v0, void* v1, int64_t v2);
void* int_to_str(void* v0, int64_t v1);
void Vec_insert(void* v0, void* v1, int64_t v2, int64_t v3);
void ComponentDef_new(void* v0, void* v1, void* v2, bool v3);
void* str_join(void* v0, void* v1, void* v2);
bool p_str_equal(void* v0, int64_t v1, int64_t v2);
int64_t HashMap_new(void* v0);
void MatchArm_new(void* v0, void* v1, void* v2, void* v3, void* v4);
bool str_contains(void* v0, void* v1, void* v2);
void Param_new(void* v0, void* v1, void* v2, void* v3, bool v4);
bool char_is_lower(void* v0, int64_t v1);
void* str_center(void* v0, void* v1, int64_t v2, int64_t v3);
void* Vec_skip(void* v0, void* v1, int64_t v2);
void* int_to_bin(void* v0, int64_t v1);
void ByteBuffer_write_byte(void* v0, void* v1, int64_t v2);
int64_t stmt_line(void* v0, void* v1);
int64_t str_parse_int(void* v0, void* v1);
int64_t String_slice(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t str_len(void* v0, void* v1);
void HashMap_insert(void* v0, int64_t v1, int64_t v2, int64_t v3);
void* Vec_chunk(void* v0, void* v1, int64_t v2);
void ImplDef_new(void* v0, void* v1, void* v2);
bool char_is_digit(void* v0, int64_t v1);
void* byte_to_str(void* v0, int64_t v1);
void* int_to_hex(void* v0, int64_t v1);
bool Vec_all(void* v0, void* v1, void* v2);
void program_new(void* v0, void* v1);
void* shape_dim_to_str(void* v0, void* v1);
int64_t ByteBuffer_len(void* v0, void* v1);
int64_t HashMap_find_slot(void* v0, int64_t v1, int64_t v2);
void StrPair_new(void* v0, void* v1, void* v2, void* v3);
void* str_split(void* v0, void* v1, void* v2);
int64_t op_tag(void* v0, void* v1);
int64_t Result_unwrap_or(void* v0, void* v1, int64_t v2);
bool vec_contains_val(void* v0, int64_t v1, int64_t v2);
void Binding_new(void* v0, void* v1, void* v2, void* v3);
void* str_from_bytes(void* v0, void* v1);
int64_t String_parse_int(void* v0, int64_t v1);
int64_t str_find_nth(void* v0, void* v1, void* v2, int64_t v3);
void* str_strip_char(void* v0, void* v1, int64_t v2);
int64_t ByteBuffer_read_byte(void* v0, void* v1, int64_t v2);
void LayerDef_new(void* v0, void* v1, void* v2, void* v3, bool v4);
void* Vec_map(void* v0, void* v1, void* v2);
void LayerInst_new(void* v0, void* v1, void* v2, void* v3, void* v4);
void* bool_to_str(void* v0, bool v1);
int64_t vec_zip(void* v0, int64_t v1, int64_t v2);
void ImportDef_new(void* v0, void* v1, void* v2, bool v3);
bool str_is_blank(void* v0, void* v1);
int64_t vec_window(void* v0, int64_t v1, int64_t v2);
void allocator_free(void* v0, void* v1, int64_t v2);
int64_t String_byte_at(void* v0, int64_t v1, int64_t v2);
void assert_msg(void* v0, bool v1, void* v2);
void* str_remove_char(void* v0, void* v1, int64_t v2);
void Result_map(void* v0, void* v1, void* v2, void* v3);
int64_t str_parse_hex(void* v0, void* v1);
void HashMap_remove(void* v0, int64_t v1, int64_t v2);
void Result_err(void* v0, void* v1, void* v2);
void* ByteBuffer_to_str(void* v0, void* v1);
bool Result_is_ok(void* v0, void* v1);
int64_t Vec_get(void* v0, void* v1, int64_t v2);
int64_t main_entry(void* v0);
bool Vec_is_empty(void* v0, void* v1);
void EffectHandler_new(void* v0, void* v1, void* v2, void* v3, void* v4, void* v5);
int64_t String_push_str(void* v0, int64_t v1, int64_t v2);
bool char_is_upper(void* v0, int64_t v1);
void StringBuilder_clear(void* v0, void* v1);
int64_t p_str_hash(void* v0, int64_t v1);
int64_t p_str_compare(void* v0, int64_t v1, int64_t v2);
int64_t StringBuilder_len(void* v0, void* v1);
int64_t p_min(void* v0, int64_t v1, int64_t v2);
void TraitDef_new(void* v0, void* v1, void* v2, void* v3);
bool char_is_whitespace(void* v0, int64_t v1);
int64_t Option_expect(void* v0, void* v1, void* v2);
void Option_or_else(void* v0, void* v1, void* v2, void* v3);
int64_t HashMap_get_or(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t String_from(void* v0, int64_t v1);
void Vec_clear(void* v0, void* v1);
int64_t HashMap_get(void* v0, int64_t v1, int64_t v2);
void HashMap_clear(void* v0, int64_t v1);
void* str_split_char(void* v0, void* v1, int64_t v2);
bool Vec_contains(void* v0, void* v1, int64_t v2);
int64_t HashMap_len(void* v0, int64_t v1);
void StringBuilder_append_char(void* v0, void* v1, int64_t v2);
int64_t String_repeat(void* v0, int64_t v1, int64_t v2);
void* Vec_filter(void* v0, void* v1, void* v2);
void Annotation_new(void* v0, void* v1, void* v2, void* v3);
int64_t clamp(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t str_compare(void* v0, void* v1, void* v2);
int64_t Vec_first(void* v0, void* v1);
void Vec_for_each(void* v0, void* v1, void* v2);
int64_t Result_unwrap_or_else(void* v0, void* v1, void* v2);
int64_t count_expr_nodes(void* v0, void* v1);
void* Vec_to_str(void* v0, int64_t v1);
int64_t String_replace(void* v0, int64_t v1, int64_t v2, int64_t v3);
void* str_capitalize(void* v0, void* v1);
bool String_starts_with(void* v0, int64_t v1, int64_t v2);
int64_t str_parse_int_range(void* v0, void* v1, int64_t v2, int64_t v3);
int64_t String_to_lowercase(void* v0, int64_t v1);
void ByteBuffer_set_byte(void* v0, void* v1, int64_t v2, int64_t v3);
void TypeAlias_new(void* v0, void* v1, void* v2, void* v3, bool v4);
bool StringBuilder_is_empty(void* v0, void* v1);
void Vec_push(void* v0, void* v1, int64_t v2);
void* str_join_ints(void* v0, void* v1, void* v2);
int64_t HashMap_clone(void* v0, int64_t v1);
void ExternDecl_new(void* v0, void* v1, void* v2, void* v3, void* v4, void* v5);
void Result_map_err(void* v0, void* v1, void* v2, void* v3);
void* p_str_from_ptr(void* v0, int64_t v1);
void* str_to_upper(void* v0, void* v1);
bool Vec_contains_val(void* v0, void* v1, int64_t v2);
bool char_is_alpha(void* v0, int64_t v1);
int64_t Option_unwrap(void* v0, void* v1);
void StringBuilder_append_int(void* v0, void* v1, int64_t v2);
int64_t vec_flat_map(void* v0, int64_t v1, void* v2);
void TraitImpl_new(void* v0, void* v1, void* v2, void* v3);
void StateDef_new(void* v0, void* v1, void* v2);
void* str_pad_right(void* v0, void* v1, int64_t v2, int64_t v3);
void Option_map(void* v0, void* v1, void* v2, void* v3);
void ModelDef_new(void* v0, void* v1, void* v2, void* v3, void* v4, bool v5);
void Attribute_new(void* v0, void* v1, void* v2, void* v3);
void* int_to_str_padded(void* v0, int64_t v1, int64_t v2);
void EnumDef_new(void* v0, void* v1, void* v2, void* v3, bool v4);
int64_t vec_skip(void* v0, int64_t v1, int64_t v2);
int64_t Vec_find(void* v0, void* v1, void* v2);
int64_t Vec_position(void* v0, void* v1, void* v2);
int64_t String_to_string(void* v0, int64_t v1);
bool str_is_empty(void* v0, void* v1);
int64_t Result_unwrap(void* v0, void* v1);
void Result_and_then(void* v0, void* v1, void* v2, void* v3);
int64_t str_find(void* v0, void* v1, void* v2);
void assert(void* v0, bool v1);
bool str_ends_with(void* v0, void* v1, void* v2);
void SelectCase_new(void* v0, void* v1, void* v2, void* v3, void* v4);
int64_t HashMap_keys(void* v0, int64_t v1);
void assert_eq_i64(void* v0, int64_t v1, int64_t v2, void* v3);
bool str_is_alpha(void* v0, void* v1);
void p_str_copy_n(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t String_to_uppercase(void* v0, int64_t v1);
void* StringBuilder_to_str(void* v0, void* v1);
void ByteBuffer_new(void* v0, void* v1, int64_t v2);
void ByteBuffer_from_string(void* v0, void* v1, void* v2);
double str_parse_float(void* v0, void* v1);
void EventHandlerDef_new(void* v0, void* v1, void* v2);
int64_t Option_unwrap_or_else(void* v0, void* v1, void* v2);
void Vec_truncate(void* v0, void* v1, int64_t v2);
void* Vec_flat_map(void* v0, void* v1, void* v2);
void StringBuilder_new(void* v0, void* v1);
bool Option_is_some(void* v0, void* v1);
int64_t String_trim(void* v0, int64_t v1);
void* str_replace(void* v0, void* v1, void* v2, void* v3);
void Vec_sort(void* v0, void* v1);
void Vec_set(void* v0, void* v1, int64_t v2, int64_t v3);
int64_t Vec_len(void* v0, void* v1);
void TraitMethod_new(void* v0, void* v1, void* v2, void* v3, bool v4);
bool Option_is_none(void* v0, void* v1);
void Vec_sort_by(void* v0, void* v1, void* v2);
void* box_type_to_str(void* v0, void* v1);
bool String_contains(void* v0, int64_t v1, int64_t v2);
int64_t allocator_bytes_used(void* v0, void* v1);
void* Vec_window(void* v0, void* v1, int64_t v2);
void* Vec_enumerate(void* v0, void* v1);

int64_t String_char_count(void* v0, int64_t v1) {
    p_call_stack_push("String_char_count");
    int64_t v3;
    int64_t v4; /* placeholder for void */
    int64_t v2;
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

void assert_eq(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("assert_eq");
    int64_t v15; /* placeholder for void */
    bool v6;
    void* v8;
    int64_t v23; /* placeholder for void */
    bool v5;
    int64_t v18; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    void* v19;
    int64_t v20; /* placeholder for void */
    int64_t v4;
    void* v11;
    int64_t v13; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    void* v17;
    int64_t v16; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v14;
    int64_t v22; /* placeholder for void */
    v4 = 0LL;
    v5 = (bool)p_str_equal((void*)v4, (int64_t)v1, (int64_t)v2);
    v6 = !v5;
    if (v6) goto bb1;
    else goto bb2;
  bb1:
    v8 = _str0;
    p_print_str(v8);
    p_print_str(v3);
    v11 = _str1;
    p_print_str(v11);
    p_print_str(v2);
    v14 = _str2;
    p_print_str(v14);
    p_print_str(v1);
    v17 = _str3;
    p_print_str(v17);
    v19 = _str4;
    p_panic(v19);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return;
    return;
}

int64_t String_find(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_find");
    void* v19;
    int64_t v39; /* placeholder for void */
    int64_t v32;
    int64_t v62; /* placeholder for void */
    int64_t v64;
    void* v68;
    int64_t v104; /* placeholder for void */
    int64_t v22;
    bool v44;
    int64_t v78; /* placeholder for void */
    int64_t v107;
    int64_t v110;
    int64_t v43;
    int64_t v49;
    int64_t v21;
    int64_t v59;
    int64_t v27; /* placeholder for void */
    int64_t v57;
    int64_t v48; /* placeholder for void */
    int64_t v36;
    int64_t v66; /* placeholder for void */
    void* v37;
    int64_t v112; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v17;
    int64_t v101; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    void* v50;
    void* v15;
    int64_t v40;
    int64_t v79; /* placeholder for void */
    void* v71;
    int64_t v38; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    bool v23;
    int64_t v45; /* placeholder for void */
    int64_t v102;
    int64_t v9;
    void* v80;
    int64_t v18;
    int64_t v93; /* placeholder for void */
    int64_t v108; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    int64_t v85; /* placeholder for void */
    void* v65;
    int64_t v41;
    uint8_t v84;
    int64_t v42;
    int64_t v29;
    bool v4;
    int64_t v67;
    int64_t v52; /* placeholder for void */
    void* v61;
    int64_t v111;
    int64_t v5;
    int64_t v97;
    void* v47;
    void* v74;
    int64_t v13;
    int64_t v14;
    int64_t v34; /* placeholder for void */
    int64_t v60;
    void* v83;
    int64_t v99; /* placeholder for void */
    int64_t v95;
    int64_t v3;
    bool v91;
    int64_t v109; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v96;
    bool v89;
    bool v100;
    int64_t v73;
    int64_t v26; /* placeholder for void */
    bool v30;
    int64_t v53;
    int64_t v94; /* placeholder for void */
    int64_t v54;
    int64_t v106;
    int64_t v11; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    uint8_t v81;
    int64_t v103; /* placeholder for void */
    int64_t v88;
    int64_t v10;
    int64_t v28;
    int64_t v33;
    int64_t v51; /* placeholder for void */
    int64_t v63;
    int64_t v35; /* placeholder for void */
    bool v6;
    bool v46;
    bool v55;
    int64_t v58;
    int64_t v24; /* placeholder for void */
    int64_t v25;
    int64_t v56; /* placeholder for void */
    int64_t v76;
    void* v77;
    bool v7;
    int64_t v82; /* placeholder for void */
    int64_t v105;
    int64_t v87;
    int64_t v20; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v72; /* placeholder for void */
    int64_t v70;
    int64_t v31; /* placeholder for void */
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

int64_t String_split(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_split");
    void* v78;
    int64_t v104; /* placeholder for void */
    void* v75;
    void* v32;
    int64_t v151; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v100;
    int64_t v46; /* placeholder for void */
    int64_t v170;
    int64_t v184; /* placeholder for void */
    int64_t v162;
    int64_t v116; /* placeholder for void */
    int64_t v138;
    bool v154;
    int64_t v84; /* placeholder for void */
    int64_t v178;
    int64_t v85;
    void* v126;
    int64_t v144; /* placeholder for void */
    int64_t v153;
    int64_t v60; /* placeholder for void */
    int64_t v7;
    void* v109;
    void* v123;
    int64_t v19; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v24;
    int64_t v99; /* placeholder for void */
    int64_t v101;
    int64_t v106;
    int64_t v135;
    int64_t v141;
    void* v35;
    int64_t v23; /* placeholder for void */
    int64_t v169;
    int64_t v51;
    int64_t v172; /* placeholder for void */
    int64_t v161; /* placeholder for void */
    void* v174;
    int64_t v88; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    bool v98;
    int64_t v14; /* placeholder for void */
    int64_t v128; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    int64_t v129; /* placeholder for void */
    bool v10;
    int64_t v113;
    int64_t v64; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v130;
    int64_t v50; /* placeholder for void */
    int64_t v139; /* placeholder for void */
    int64_t v27;
    int64_t v165;
    int64_t v183;
    int64_t v164;
    int64_t v56;
    int64_t v55;
    int64_t v16;
    int64_t v71;
    int64_t v41;
    void* v63;
    int64_t v80; /* placeholder for void */
    int64_t v62;
    void* v150;
    int64_t v25; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    void* v69;
    int64_t v61;
    void* v103;
    int64_t v83; /* placeholder for void */
    void* v159;
    int64_t v119;
    int64_t v26; /* placeholder for void */
    bool v53;
    bool v42;
    int64_t v57;
    int64_t v163;
    int64_t v179; /* placeholder for void */
    void* v5;
    int64_t v20;
    void* v48;
    void* v120;
    int64_t v111;
    int64_t v134;
    int64_t v38;
    int64_t v40;
    void* v72;
    bool v89;
    int64_t v102;
    int64_t v115;
    int64_t v167; /* placeholder for void */
    int64_t v175; /* placeholder for void */
    int64_t v188; /* placeholder for void */
    int64_t v95;
    int64_t v13;
    int64_t v157;
    bool v11;
    int64_t v9;
    void* v81;
    int64_t v86;
    int64_t v152;
    int64_t v155; /* placeholder for void */
    int64_t v65;
    int64_t v67; /* placeholder for void */
    int64_t v125; /* placeholder for void */
    int64_t v148;
    void* v160;
    int64_t v176; /* placeholder for void */
    int64_t v187;
    void* v45;
    void* v59;
    int64_t v21;
    int64_t v96; /* placeholder for void */
    int64_t v158;
    int64_t v117;
    void* v18;
    int64_t v33; /* placeholder for void */
    void* v108;
    int64_t v29; /* placeholder for void */
    int64_t v58;
    int64_t v93;
    int64_t v47;
    int64_t v52;
    int64_t v114;
    int64_t v127;
    int64_t v131;
    bool v8;
    int64_t v147;
    bool v87;
    int64_t v180; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v186; /* placeholder for void */
    int64_t v107;
    int64_t v149;
    int64_t v3;
    int64_t v6; /* placeholder for void */
    int64_t v31;
    int64_t v74;
    bool v44;
    int64_t v54; /* placeholder for void */
    int64_t v73; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v122;
    int64_t v132;
    int64_t v136;
    int64_t v166;
    int64_t v143;
    int64_t v94;
    int64_t v168;
    void* v28;
    int64_t v34;
    int64_t v4;
    int64_t v92; /* placeholder for void */
    int64_t v156;
    int64_t v137; /* placeholder for void */
    int64_t v146; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    int64_t v133; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    void* v66;
    int64_t v68;
    int64_t v112;
    uint8_t v79;
    bool v22;
    int64_t v105;
    int64_t v110; /* placeholder for void */
    void* v171;
    uint8_t v82;
    int64_t v140; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    int64_t v173;
    int64_t v17;
    int64_t v30;
    int64_t v182;
    int64_t v145; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v118;
    void* v177;
    int64_t v142;
    int64_t v185; /* placeholder for void */
    int64_t v121; /* placeholder for void */
    int64_t v181;
    int64_t v124; /* placeholder for void */
    int64_t v39;
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
    Vec_push((void*)v132, (void*)v130, v131);
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
    Vec_push((void*)v183, (void*)v181, v182);
    goto bb21;
  bb20:
    goto bb21;
  bb21:
    v187 = *(int64_t*)v5;
    p_call_stack_pop();
    return v187;
    return 0;
}

void* float_to_str(void* v0, double v1) {
    p_call_stack_push("float_to_str");
    int64_t v3; /* placeholder for void */
    void* v2;
    v2 = p_float_to_str(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

bool Vec_any(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_any");
    bool v21;
    bool v13;
    int64_t v20;
    int64_t v11;
    int64_t v25; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v26;
    int64_t v32; /* placeholder for void */
    bool v31;
    int64_t v30; /* placeholder for void */
    bool v23;
    void* v4;
    int64_t v14; /* placeholder for void */
    int64_t v12;
    int64_t v6;
    int64_t v16;
    int64_t v28;
    int64_t v27;
    int64_t v29; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v3;
    int64_t v7;
    int64_t v17;
    int64_t v10; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    void* v8;
    int64_t v15;
    void* v18;
    v3 = 0LL;
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = 0LL;
    v7 = (int64_t)Vec_len((void*)v6, v1);
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    goto bb1;
  bb1:
    v11 = *(int64_t*)v4;
    v12 = *(int64_t*)v8;
    v13 = v11 < v12;
    if (v13) goto bb2;
    else goto bb3;
  bb2:
    v15 = *(int64_t*)v4;
    v16 = 0LL;
    v17 = (int64_t)Vec_get((void*)v16, v1, v15);
    v18 = malloc(8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = *(int64_t*)v18;
    {
        if (!v2) abort();
        char* _fn_name = *(char**)v2;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "str_is_blank") == 0) {
            v21 = str_is_blank((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Option_is_some") == 0) {
            v21 = Option_is_some((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Option_is_none") == 0) {
            v21 = Option_is_none((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Vec_is_empty") == 0) {
            v21 = Vec_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_numeric") == 0) {
            v21 = str_is_numeric((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "String_is_empty") == 0) {
            v21 = String_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_alpha") == 0) {
            v21 = char_is_alpha((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "str_is_alpha") == 0) {
            v21 = str_is_alpha((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "HashMap_is_empty") == 0) {
            v21 = HashMap_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Result_is_err") == 0) {
            v21 = Result_is_err((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_alphanumeric") == 0) {
            v21 = str_is_alphanumeric((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_empty") == 0) {
            v21 = str_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "StringBuilder_is_empty") == 0) {
            v21 = StringBuilder_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Result_is_ok") == 0) {
            v21 = Result_is_ok((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_whitespace") == 0) {
            v21 = char_is_whitespace((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_upper") == 0) {
            v21 = char_is_upper((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "ByteBuffer_is_empty") == 0) {
            v21 = ByteBuffer_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_digit") == 0) {
            v21 = char_is_digit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_lower") == 0) {
            v21 = char_is_lower((void*)v2, (int64_t)v20);
        } else { abort(); }
    }
    if (v21) goto bb4;
    else goto bb5;
  bb3:
    v31 = false;
    p_call_stack_pop();
    return v31;
  bb4:
    v23 = true;
    p_call_stack_pop();
    return v23;
  bb5:
    goto bb6;
  bb6:
    v26 = *(int64_t*)v4;
    v27 = 1LL;
    v28 = v26 + v27;
    *(int64_t*)v4 = (int64_t)v28;
    goto bb1;
    return 0;
}

void ByteBuffer_write_bytes(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("ByteBuffer_write_bytes");
    bool v9;
    int64_t v26;
    int64_t v28; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v31;
    int64_t v6; /* placeholder for void */
    void* v13;
    int64_t v15;
    int64_t v25; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v5;
    uint8_t v23;
    int64_t v21; /* placeholder for void */
    int64_t v27;
    int64_t v33; /* placeholder for void */
    int64_t v18;
    int64_t v30;
    int64_t v4;
    int64_t v8;
    int64_t v10; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v11;
    int64_t v7; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    void* v19;
    void* v22;
    int64_t v29;
    int64_t v12;
    int64_t v32; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    void* v16;
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

void Vec_reverse(void* v0, void* v1) {
    p_call_stack_push("Vec_reverse");
    int64_t v3; /* placeholder for void */
    int64_t v2; /* placeholder for void */
    p_vec_reverse(v1);
    p_call_stack_pop();
    return;
    return;
}

void* Vec_clone(void* v0, void* v1) {
    p_call_stack_push("Vec_clone");
    int64_t v2;
    int64_t v3; /* placeholder for void */
    v2 = (int64_t)p_vec_clone(v1);
    p_call_stack_pop();
    return (void*)v2;
    return 0;
}

int64_t Result_expect(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_expect");
    int64_t v7; /* placeholder for void */
    int64_t v9;
    bool v14;
    int64_t v18; /* placeholder for void */
    bool v6;
    int64_t v13;
    int64_t v10; /* placeholder for void */
    void* v16;
    int64_t v19; /* placeholder for void */
    void* v8;
    int64_t v15; /* placeholder for void */
    int64_t v12;
    int64_t v4;
    void* v3;
    int64_t v5;
    int64_t v17;
    int64_t v20; /* placeholder for void */
    int64_t v21;
    int64_t v22; /* placeholder for void */
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

int64_t p_max(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("max");
    bool v3;
    int64_t v6; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v5; /* placeholder for void */
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

void vec_dedup(void* v0, int64_t v1) {
    p_call_stack_push("vec_dedup");
    void* v13;
    int64_t v33;
    int64_t v24;
    int64_t v23;
    int64_t v36;
    int64_t v48; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    bool v8;
    int64_t v17; /* placeholder for void */
    void* v4;
    int64_t v2;
    int64_t v30;
    int64_t v42; /* placeholder for void */
    int64_t v50;
    int64_t v45;
    int64_t v9; /* placeholder for void */
    int64_t v20;
    int64_t v34;
    int64_t v29;
    int64_t v12;
    int64_t v27;
    int64_t v3;
    int64_t v11; /* placeholder for void */
    bool v31;
    int64_t v46;
    int64_t v28;
    int64_t v25;
    int64_t v41;
    int64_t v5; /* placeholder for void */
    int64_t v7;
    void* v16;
    int64_t v44; /* placeholder for void */
    int64_t v37;
    int64_t v38; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v39;
    int64_t v22; /* placeholder for void */
    int64_t v47;
    int64_t v19;
    int64_t v49; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v26;
    int64_t v6;
    int64_t v10; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v35;
    bool v21;
    int64_t v18; /* placeholder for void */
    int64_t v40;
    int64_t v15;
    v2 = 0LL;
    v3 = (int64_t)Vec_len((void*)v2, (void*)v1);
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    v7 = 1LL;
    v8 = v6 <= v7;
    if (v8) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return;
  bb2:
    goto bb3;
  bb3:
    v12 = 1LL;
    v13 = malloc(8);
    *(int64_t*)v13 = (int64_t)v12;
    v15 = 1LL;
    v16 = malloc(8);
    *(int64_t*)v16 = (int64_t)v15;
    goto bb4;
  bb4:
    v19 = *(int64_t*)v16;
    v20 = *(int64_t*)v4;
    v21 = v19 < v20;
    if (v21) goto bb5;
    else goto bb6;
  bb5:
    v23 = *(int64_t*)v16;
    v24 = 0LL;
    v25 = (int64_t)Vec_get((void*)v24, (void*)v1, v23);
    v26 = *(int64_t*)v16;
    v27 = 1LL;
    v28 = v26 - v27;
    v29 = 0LL;
    v30 = (int64_t)Vec_get((void*)v29, (void*)v1, v28);
    v31 = v25 != v30;
    if (v31) goto bb7;
    else goto bb8;
  bb6:
    v50 = *(int64_t*)v13;
    p_vec_truncate((void*)v1, v50);
    p_call_stack_pop();
    return;
  bb7:
    v33 = *(int64_t*)v13;
    v34 = *(int64_t*)v16;
    v35 = 0LL;
    v36 = (int64_t)Vec_get((void*)v35, (void*)v1, v34);
    v37 = 0LL;
    Vec_set((void*)v37, (void*)v1, v33, v36);
    v39 = *(int64_t*)v13;
    v40 = 1LL;
    v41 = v39 + v40;
    *(int64_t*)v13 = (int64_t)v41;
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    v45 = *(int64_t*)v16;
    v46 = 1LL;
    v47 = v45 + v46;
    *(int64_t*)v16 = (int64_t)v47;
    goto bb4;
    return;
}

int64_t Vec_fold(void* v0, void* v1, int64_t v2, void* v3) {
    p_call_stack_push("Vec_fold");
    int64_t v31; /* placeholder for void */
    int64_t v14;
    int64_t v24;
    int64_t v17; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v10;
    int64_t v15;
    int64_t v32;
    int64_t v26; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    void* v7;
    int64_t v29;
    int64_t v6;
    int64_t v20;
    int64_t v19;
    int64_t v8; /* placeholder for void */
    int64_t v18;
    int64_t v5; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v25;
    void* v11;
    int64_t v13; /* placeholder for void */
    int64_t v27;
    void* v21;
    int64_t v23;
    bool v16;
    void* v4;
    int64_t v28;
    int64_t v9;
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v2;
    v6 = 0LL;
    v7 = malloc(8);
    *(int64_t*)v7 = (int64_t)v6;
    v9 = 0LL;
    v10 = (int64_t)Vec_len((void*)v9, v1);
    v11 = malloc(8);
    *(int64_t*)v11 = (int64_t)v10;
    goto bb1;
  bb1:
    v14 = *(int64_t*)v7;
    v15 = *(int64_t*)v11;
    v16 = v14 < v15;
    if (v16) goto bb2;
    else goto bb3;
  bb2:
    v18 = *(int64_t*)v7;
    v19 = 0LL;
    v20 = (int64_t)Vec_get((void*)v19, v1, v18);
    v21 = malloc(8);
    *(int64_t*)v21 = (int64_t)v20;
    v23 = *(int64_t*)v4;
    v24 = *(int64_t*)v21;
    {
        if (!v3) abort();
        char* _fn_name = *(char**)v3;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "str_count") == 0) {
            v25 = str_count((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "String_push_str") == 0) {
            v25 = String_push_str((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "vec_skip") == 0) {
            v25 = vec_skip((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Option_unwrap_or") == 0) {
            v25 = Option_unwrap_or((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Vec_get") == 0) {
            v25 = Vec_get((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_repeat") == 0) {
            v25 = String_repeat((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Result_unwrap_or_else") == 0) {
            v25 = Result_unwrap_or_else((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "str_rfind") == 0) {
            v25 = str_rfind((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "Option_unwrap_or_else") == 0) {
            v25 = Option_unwrap_or_else((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "vec_chunk") == 0) {
            v25 = vec_chunk((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "str_byte_at") == 0) {
            v25 = str_byte_at((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "vec_window") == 0) {
            v25 = vec_window((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "vec_take") == 0) {
            v25 = vec_take((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Result_expect") == 0) {
            v25 = Result_expect((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "String_byte_at") == 0) {
            v25 = String_byte_at((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_find") == 0) {
            v25 = String_find((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Vec_find") == 0) {
            v25 = Vec_find((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "max") == 0) {
            v25 = p_max((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_split") == 0) {
            v25 = String_split((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Vec_remove") == 0) {
            v25 = Vec_remove((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "p_str_compare") == 0) {
            v25 = p_str_compare((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "str_compare") == 0) {
            v25 = str_compare((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "Vec_position") == 0) {
            v25 = Vec_position((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "Result_unwrap_or") == 0) {
            v25 = Result_unwrap_or((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Option_expect") == 0) {
            v25 = Option_expect((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "String_rfind") == 0) {
            v25 = String_rfind((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "str_find") == 0) {
            v25 = str_find((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "ByteBuffer_read_byte") == 0) {
            v25 = ByteBuffer_read_byte((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "allocator_alloc") == 0) {
            v25 = allocator_alloc((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "HashMap_get") == 0) {
            v25 = HashMap_get((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "vec_zip") == 0) {
            v25 = vec_zip((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "str_count_char") == 0) {
            v25 = str_count_char((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "HashMap_find_slot") == 0) {
            v25 = HashMap_find_slot((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "min") == 0) {
            v25 = p_min((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_char_at") == 0) {
            v25 = String_char_at((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "vec_flat_map") == 0) {
            v25 = vec_flat_map((void*)v3, (int64_t)v23, (void*)v24);
        } else { abort(); }
    }
    *(int64_t*)v4 = (int64_t)v25;
    v27 = *(int64_t*)v7;
    v28 = 1LL;
    v29 = v27 + v28;
    *(int64_t*)v7 = (int64_t)v29;
    goto bb1;
  bb3:
    v32 = *(int64_t*)v4;
    p_call_stack_pop();
    return v32;
    return 0;
}

void* str_title(void* v0, void* v1) {
    p_call_stack_push("str_title");
    int64_t v11; /* placeholder for void */
    int64_t v38;
    int64_t v39; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    int64_t v74;
    void* v75;
    int64_t v7;
    int64_t v55;
    bool v65;
    int64_t v10; /* placeholder for void */
    int64_t v24;
    int64_t v32;
    void* v60;
    int64_t v61;
    int64_t v57;
    bool v8;
    bool v16;
    int64_t v41; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    bool v47;
    int64_t v68;
    int64_t v71; /* placeholder for void */
    void* v20;
    void* v14;
    int64_t v3;
    void* v13;
    bool v25;
    int64_t v21; /* placeholder for void */
    void* v4;
    int64_t v66; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    int64_t v72; /* placeholder for void */
    int64_t v12;
    int64_t v15; /* placeholder for void */
    int64_t v44;
    int64_t v56;
    void* v30;
    int64_t v37;
    int64_t v63; /* placeholder for void */
    int64_t v19;
    int64_t v62;
    int64_t v27;
    bool v34;
    bool v43;
    int64_t v52; /* placeholder for void */
    void* v73;
    int64_t v18; /* placeholder for void */
    int64_t v48;
    int64_t v49;
    int64_t v58; /* placeholder for void */
    void* v53;
    int64_t v70;
    bool v46;
    int64_t v9; /* placeholder for void */
    int64_t v6;
    int64_t v29;
    bool v50;
    int64_t v28;
    int64_t v33;
    int64_t v45;
    int64_t v35; /* placeholder for void */
    bool v40;
    int64_t v69;
    int64_t v5; /* placeholder for void */
    void* v17;
    void* v36;
    bool v51;
    int64_t v2;
    int64_t v26; /* placeholder for void */
    int64_t v23;
    int64_t v31; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    int64_t v54;
    v2 = 0LL;
    v3 = (int64_t)str_len((void*)v2, v1);
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    v7 = 0LL;
    v8 = v6 == v7;
    if (v8) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return v1;
  bb2:
    goto bb3;
  bb3:
    v12 = 0LL;
    v13 = Vec_new((void*)v12);
    v14 = malloc(8);
    *(void**)v14 = (void*)v13;
    v16 = true;
    v17 = malloc(1);
    *(bool*)v17 = v16;
    v19 = 0LL;
    v20 = malloc(8);
    *(int64_t*)v20 = (int64_t)v19;
    goto bb4;
  bb4:
    v23 = *(int64_t*)v20;
    v24 = *(int64_t*)v4;
    v25 = v23 < v24;
    if (v25) goto bb5;
    else goto bb6;
  bb5:
    v27 = *(int64_t*)v20;
    v28 = 0LL;
    v29 = (int64_t)str_byte_at((void*)v28, v1, v27);
    v30 = malloc(8);
    *(int64_t*)v30 = (int64_t)v29;
    v32 = *(int64_t*)v30;
    v33 = 0LL;
    v34 = (bool)char_is_whitespace((void*)v33, v32);
    if (v34) goto bb7;
    else goto bb8;
  bb6:
    v73 = *(void**)v14;
    v74 = 0LL;
    v75 = str_from_bytes((void*)v74, v73);
    p_call_stack_pop();
    return v75;
  bb7:
    v36 = *(void**)v14;
    v37 = *(int64_t*)v30;
    v38 = 0LL;
    Vec_push((void*)v38, v36, v37);
    v40 = true;
    *(bool*)v17 = v40;
    goto bb9;
  bb8:
    v43 = *(bool*)v17;
    v44 = *(int64_t*)v30;
    v45 = 97LL;
    v46 = v44 >= v45;
    v47 = v43 && v46;
    v48 = *(int64_t*)v30;
    v49 = 122LL;
    v50 = v48 <= v49;
    v51 = v47 && v50;
    if (v51) goto bb10;
    else goto bb11;
  bb9:
    v68 = *(int64_t*)v20;
    v69 = 1LL;
    v70 = v68 + v69;
    *(int64_t*)v20 = (int64_t)v70;
    goto bb4;
  bb10:
    v53 = *(void**)v14;
    v54 = *(int64_t*)v30;
    v55 = 32LL;
    v56 = v54 - v55;
    v57 = 0LL;
    Vec_push((void*)v57, v53, v56);
    goto bb12;
  bb11:
    v60 = *(void**)v14;
    v61 = *(int64_t*)v30;
    v62 = 0LL;
    Vec_push((void*)v62, v60, v61);
    goto bb12;
  bb12:
    v65 = false;
    *(bool*)v17 = v65;
    goto bb9;
    return 0;
}

int64_t HashMap_values(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_values");
    int64_t v34;
    int64_t v6;
    int64_t v18; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v3;
    int64_t v12;
    int64_t v26;
    void* v4;
    int64_t v30;
    int64_t v40;
    int64_t v17; /* placeholder for void */
    int64_t v23;
    int64_t v35;
    int64_t v20;
    void* v13;
    int64_t v42; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v36;
    int64_t v15;
    int64_t v2;
    int64_t v41; /* placeholder for void */
    int64_t v29;
    int64_t v38;
    int64_t v7;
    void* v9;
    int64_t v14; /* placeholder for void */
    int64_t v25;
    int64_t v44; /* placeholder for void */
    int64_t v24;
    int64_t v11;
    int64_t v19;
    int64_t v39;
    int64_t v31;
    int64_t v22; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    bool v21;
    int64_t v8;
    int64_t v43;
    void* v27;
    void* v16;
    void* v32;
    v2 = 0LL;
    v3 = (int64_t)HashMap_keys((void*)v2, v1);
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    v7 = 0LL;
    v8 = (int64_t)Vec_len((void*)v7, (void*)v6);
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 0LL;
    v12 = (int64_t)Vec_new((void*)v11);
    v13 = malloc(8);
    *(int64_t*)v13 = (int64_t)v12;
    v15 = 0LL;
    v16 = malloc(8);
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
    v26 = (int64_t)Vec_get((void*)v25, (void*)v23, v24);
    v27 = malloc(8);
    *(int64_t*)v27 = (int64_t)v26;
    v29 = *(int64_t*)v27;
    v30 = 0LL;
    v31 = (int64_t)HashMap_get((void*)v30, v1, v29);
    v32 = malloc(8);
    *(int64_t*)v32 = (int64_t)v31;
    v34 = *(int64_t*)v13;
    v35 = *(int64_t*)v32;
    v36 = 0LL;
    Vec_push((void*)v36, (void*)v34, v35);
    v38 = *(int64_t*)v16;
    v39 = 1LL;
    v40 = v38 + v39;
    *(int64_t*)v16 = (int64_t)v40;
    goto bb1;
  bb3:
    v43 = *(int64_t*)v13;
    p_call_stack_pop();
    return v43;
    return 0;
}

bool HashMap_is_empty(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_is_empty");
    void* v10;
    void* v7;
    bool v16;
    int64_t v14;
    int64_t v11;
    int64_t v8; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v3;
    void* v4;
    int64_t v12; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v2;
    int64_t v6;
    int64_t v15;
    int64_t v17; /* placeholder for void */
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

bool str_is_numeric(void* v0, void* v1) {
    p_call_stack_push("str_is_numeric");
    int64_t v14;
    int64_t v29;
    int64_t v30;
    bool v34;
    int64_t v22;
    int64_t v13; /* placeholder for void */
    bool v7;
    bool v26;
    int64_t v18; /* placeholder for void */
    int64_t v21;
    int64_t v2;
    int64_t v9; /* placeholder for void */
    int64_t v3;
    int64_t v24;
    int64_t v28; /* placeholder for void */
    int64_t v31;
    bool v5;
    bool v23;
    bool v17;
    int64_t v27; /* placeholder for void */
    void* v11;
    int64_t v4;
    int64_t v6; /* placeholder for void */
    int64_t v10;
    int64_t v15;
    int64_t v33; /* placeholder for void */
    int64_t v20;
    int64_t v32; /* placeholder for void */
    int64_t v19;
    int64_t v35; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v16;
    int64_t v8; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)str_len((void*)v2, v1);
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb1;
    else goto bb2;
  bb1:
    v7 = false;
    p_call_stack_pop();
    return v7;
  bb2:
    goto bb3;
  bb3:
    v10 = 0LL;
    v11 = malloc(8);
    *(int64_t*)v11 = (int64_t)v10;
    goto bb4;
  bb4:
    v14 = *(int64_t*)v11;
    v15 = 0LL;
    v16 = (int64_t)str_len((void*)v15, v1);
    v17 = v14 < v16;
    if (v17) goto bb5;
    else goto bb6;
  bb5:
    v19 = *(int64_t*)v11;
    v20 = 0LL;
    v21 = (int64_t)str_byte_at((void*)v20, v1, v19);
    v22 = 0LL;
    v23 = (bool)char_is_digit((void*)v22, v21);
    v24 = !v23;
    if (v24) goto bb7;
    else goto bb8;
  bb6:
    v34 = true;
    p_call_stack_pop();
    return v34;
  bb7:
    v26 = false;
    p_call_stack_pop();
    return v26;
  bb8:
    goto bb9;
  bb9:
    v29 = *(int64_t*)v11;
    v30 = 1LL;
    v31 = v29 + v30;
    *(int64_t*)v11 = (int64_t)v31;
    goto bb4;
    return 0;
}

bool str_equal(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_equal");
    int64_t v5; /* placeholder for void */
    int64_t v3;
    bool v4;
    v3 = 0LL;
    v4 = (bool)p_str_equal((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

void* Vec_take(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_take");
    int64_t v4;
    int64_t v3;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)vec_take((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return (void*)v4;
    return 0;
}

int64_t Option_unwrap_or(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Option_unwrap_or");
    void* v8;
    bool v6;
    void* v3;
    int64_t v13;
    bool v14;
    int64_t v7; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v18;
    int64_t v5;
    int64_t v19; /* placeholder for void */
    int64_t v12;
    int64_t v10; /* placeholder for void */
    int64_t v4;
    int64_t v16; /* placeholder for void */
    int64_t v9;
    void* v11;
    int64_t v17; /* placeholder for void */
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

int64_t Vec_last(void* v0, void* v1) {
    p_call_stack_push("Vec_last");
    int64_t v3; /* placeholder for void */
    int64_t v2;
    v2 = (int64_t)p_vec_last(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void AssocTypeDecl_new(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("AssocTypeDecl_new");
    void* v11;
    void* v9;
    void* v5;
    void* v13;
    void* v4;
    void* v14;
    void* v10;
    int64_t v16; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v7;
    void* v15;
    int64_t v12; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    v4 = malloc(16);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = (char*)v4 + 8;
    *(void**)v7 = (void*)v3;
    v9 = (char*)v4 + 0;
    v10 = (char*)v0 + 0;
    v11 = *(void**)v9;
    *(void**)v10 = (void*)v11;
    v13 = (char*)v4 + 8;
    v14 = (char*)v0 + 8;
    v15 = *(void**)v13;
    *(void**)v14 = (void*)v15;
    p_call_stack_pop();
    return;
    return;
}

void* str_bytes(void* v0, void* v1) {
    p_call_stack_push("str_bytes");
    int64_t v39;
    int64_t v13; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v10;
    int64_t v18;
    int64_t v27; /* placeholder for void */
    void* v33;
    int64_t v36; /* placeholder for void */
    void* v29;
    int64_t v35;
    int64_t v9; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    void* v20;
    int64_t v32; /* placeholder for void */
    void* v42;
    int64_t v38;
    bool v16;
    int64_t v12; /* placeholder for void */
    uint8_t v30;
    int64_t v31; /* placeholder for void */
    void* v11;
    void* v3;
    int64_t v5; /* placeholder for void */
    int64_t v6;
    int64_t v17; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v15;
    int64_t v19;
    int64_t v40; /* placeholder for void */
    int64_t v25;
    void* v4;
    int64_t v24; /* placeholder for void */
    void* v8;
    int64_t v7;
    void* v23;
    int64_t v34;
    int64_t v37;
    int64_t v22;
    int64_t v41; /* placeholder for void */
    void* v26;
    int64_t v2;
    int64_t v14;
    v2 = 0LL;
    v3 = Vec_new((void*)v2);
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
    Vec_push((void*)v35, v33, v34);
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

void* str_slice(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("str_slice");
    void* v6;
    int64_t v38;
    int64_t v15; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    bool v22;
    int64_t v35;
    int64_t v39; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v16;
    int64_t v19; /* placeholder for void */
    int64_t v28;
    int64_t v26; /* placeholder for void */
    int64_t v4;
    void* v8;
    int64_t v18; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    void* v32;
    int64_t v34; /* placeholder for void */
    bool v30;
    void* v10;
    int64_t v36;
    int64_t v5;
    int64_t v12;
    int64_t v24;
    int64_t v7; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    bool v14;
    int64_t v33; /* placeholder for void */
    int64_t v13;
    int64_t v29;
    int64_t v25; /* placeholder for void */
    int64_t v21;
    int64_t v31; /* placeholder for void */
    int64_t v20;
    int64_t v37;
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
    v32 = _str5;
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

void FieldInit_new(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("FieldInit_new");
    void* v7;
    int64_t v6; /* placeholder for void */
    void* v9;
    void* v28;
    void* v30;
    int64_t v15; /* placeholder for void */
    void* v31;
    void* v34;
    int64_t v10;
    void* v5;
    void* v17;
    int64_t v33; /* placeholder for void */
    void* v35;
    int64_t v41; /* placeholder for void */
    void* v42;
    void* v20;
    int64_t v40;
    void* v43;
    int64_t v45; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    void* v8;
    void* v25;
    void* v12;
    int64_t v32;
    int64_t v19; /* placeholder for void */
    int64_t v36;
    int64_t v11; /* placeholder for void */
    void* v21;
    void* v38;
    void* v24;
    int64_t v23; /* placeholder for void */
    void* v4;
    void* v16;
    int64_t v44;
    void* v13;
    int64_t v18;
    int64_t v22;
    int64_t v27; /* placeholder for void */
    void* v26;
    void* v29;
    int64_t v14;
    int64_t v37; /* placeholder for void */
    void* v39;
    v4 = malloc(40);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = (char*)v4 + 8;
    v8 = (char*)v3 + 0;
    v9 = (char*)v7 + 0;
    v10 = *(int64_t*)v8;
    *(int64_t*)v9 = (int64_t)v10;
    v12 = (char*)v3 + 8;
    v13 = (char*)v7 + 8;
    v14 = *(int64_t*)v12;
    *(int64_t*)v13 = (int64_t)v14;
    v16 = (char*)v3 + 16;
    v17 = (char*)v7 + 16;
    v18 = *(int64_t*)v16;
    *(int64_t*)v17 = (int64_t)v18;
    v20 = (char*)v3 + 24;
    v21 = (char*)v7 + 24;
    v22 = *(int64_t*)v20;
    *(int64_t*)v21 = (int64_t)v22;
    v24 = (char*)v4 + 0;
    v25 = (char*)v0 + 0;
    v26 = *(void**)v24;
    *(void**)v25 = (void*)v26;
    v28 = (char*)v4 + 8;
    v29 = (char*)v0 + 8;
    v30 = (char*)v28 + 0;
    v31 = (char*)v29 + 0;
    v32 = *(int64_t*)v30;
    *(int64_t*)v31 = (int64_t)v32;
    v34 = (char*)v28 + 8;
    v35 = (char*)v29 + 8;
    v36 = *(int64_t*)v34;
    *(int64_t*)v35 = (int64_t)v36;
    v38 = (char*)v28 + 16;
    v39 = (char*)v29 + 16;
    v40 = *(int64_t*)v38;
    *(int64_t*)v39 = (int64_t)v40;
    v42 = (char*)v28 + 24;
    v43 = (char*)v29 + 24;
    v44 = *(int64_t*)v42;
    *(int64_t*)v43 = (int64_t)v44;
    p_call_stack_pop();
    return;
    return;
}

int64_t String_char_at(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_char_at");
    bool v14;
    int64_t v9;
    int64_t v52; /* placeholder for void */
    void* v54;
    void* v38;
    int64_t v5; /* placeholder for void */
    void* v41;
    void* v11;
    int64_t v47; /* placeholder for void */
    int64_t v3;
    void* v51;
    int64_t v6;
    int64_t v31; /* placeholder for void */
    int64_t v61;
    int64_t v10;
    void* v44;
    int64_t v19;
    int64_t v25;
    void* v37;
    int64_t v40;
    int64_t v18; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    int64_t v58;
    int64_t v20; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v49;
    int64_t v15;
    int64_t v53;
    uint8_t v33;
    int64_t v62; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    void* v23;
    void* v26;
    int64_t v45;
    int64_t v60; /* placeholder for void */
    bool v16;
    void* v29;
    int64_t v12; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v13;
    int64_t v48;
    int64_t v55; /* placeholder for void */
    int64_t v22;
    int64_t v24; /* placeholder for void */
    void* v32;
    int64_t v36;
    int64_t v50;
    int64_t v56; /* placeholder for void */
    int64_t v28;
    void* v57;
    int64_t v30; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    bool v17;
    bool v4;
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

void* str_trim(void* v0, void* v1) {
    p_call_stack_push("str_trim");
    int64_t v2;
    int64_t v3;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_trim((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return (void*)v3;
    return 0;
}

bool Result_is_err(void* v0, void* v1) {
    p_call_stack_push("Result_is_err");
    int64_t v6; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v12;
    void* v7;
    int64_t v17;
    bool v21;
    int64_t v19; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    bool v5;
    int64_t v8;
    bool v18;
    bool v9;
    int64_t v13;
    void* v16;
    int64_t v20; /* placeholder for void */
    int64_t v3;
    int64_t v4;
    bool v14;
    int64_t v10; /* placeholder for void */
    void* v2;
    void* v11;
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

int64_t p_abs(void* v0, int64_t v1) {
    p_call_stack_push("abs");
    int64_t v4; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v5;
    int64_t v2;
    bool v3;
    int64_t v6; /* placeholder for void */
    int64_t v8; /* placeholder for void */
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

void AsmOperand_new(void* v0, void* v1, void* v2, void* v3, void* v4) {
    p_call_stack_push("AsmOperand_new");
    int64_t v58; /* placeholder for void */
    void* v51;
    void* v42;
    void* v19;
    int64_t v17; /* placeholder for void */
    void* v37;
    int64_t v48;
    int64_t v49; /* placeholder for void */
    void* v40;
    void* v50;
    void* v31;
    void* v6;
    int64_t v53; /* placeholder for void */
    void* v54;
    int64_t v16;
    void* v18;
    void* v38;
    void* v22;
    void* v36;
    int64_t v39; /* placeholder for void */
    void* v47;
    void* v14;
    int64_t v9;
    void* v43;
    int64_t v10; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    void* v7;
    int64_t v24;
    int64_t v56;
    int64_t v52;
    int64_t v25; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    void* v13;
    void* v27;
    void* v11;
    int64_t v28;
    void* v46;
    int64_t v34;
    void* v23;
    void* v5;
    void* v8;
    void* v55;
    int64_t v44;
    void* v41;
    void* v15;
    int64_t v20;
    int64_t v12; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    void* v32;
    void* v30;
    void* v33;
    void* v26;
    v5 = malloc(48);
    v6 = (char*)v5 + 0;
    v7 = (char*)v2 + 0;
    v8 = (char*)v6 + 0;
    v9 = *(int64_t*)v7;
    *(int64_t*)v8 = (int64_t)v9;
    v11 = (char*)v5 + 8;
    *(void**)v11 = (void*)v3;
    v13 = (char*)v5 + 16;
    v14 = (char*)v4 + 0;
    v15 = (char*)v13 + 0;
    v16 = *(int64_t*)v14;
    *(int64_t*)v15 = (int64_t)v16;
    v18 = (char*)v4 + 8;
    v19 = (char*)v13 + 8;
    v20 = *(int64_t*)v18;
    *(int64_t*)v19 = (int64_t)v20;
    v22 = (char*)v4 + 16;
    v23 = (char*)v13 + 16;
    v24 = *(int64_t*)v22;
    *(int64_t*)v23 = (int64_t)v24;
    v26 = (char*)v4 + 24;
    v27 = (char*)v13 + 24;
    v28 = *(int64_t*)v26;
    *(int64_t*)v27 = (int64_t)v28;
    v30 = (char*)v5 + 0;
    v31 = (char*)v0 + 0;
    v32 = (char*)v30 + 0;
    v33 = (char*)v31 + 0;
    v34 = *(int64_t*)v32;
    *(int64_t*)v33 = (int64_t)v34;
    v36 = (char*)v5 + 8;
    v37 = (char*)v0 + 8;
    v38 = *(void**)v36;
    *(void**)v37 = (void*)v38;
    v40 = (char*)v5 + 16;
    v41 = (char*)v0 + 16;
    v42 = (char*)v40 + 0;
    v43 = (char*)v41 + 0;
    v44 = *(int64_t*)v42;
    *(int64_t*)v43 = (int64_t)v44;
    v46 = (char*)v40 + 8;
    v47 = (char*)v41 + 8;
    v48 = *(int64_t*)v46;
    *(int64_t*)v47 = (int64_t)v48;
    v50 = (char*)v40 + 16;
    v51 = (char*)v41 + 16;
    v52 = *(int64_t*)v50;
    *(int64_t*)v51 = (int64_t)v52;
    v54 = (char*)v40 + 24;
    v55 = (char*)v41 + 24;
    v56 = *(int64_t*)v54;
    *(int64_t*)v55 = (int64_t)v56;
    p_call_stack_pop();
    return;
    return;
}

bool HashMap_contains(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_contains");
    void* v24;
    int64_t v23;
    int64_t v46;
    int64_t v53; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    void* v32;
    int64_t v18; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    void* v5;
    int64_t v58;
    int64_t v37;
    void* v26;
    int64_t v51;
    bool v56;
    int64_t v9; /* placeholder for void */
    void* v47;
    int64_t v55;
    bool v60;
    int64_t v30; /* placeholder for void */
    bool v61;
    int64_t v20; /* placeholder for void */
    bool v17;
    void* v41;
    int64_t v39;
    int64_t v62; /* placeholder for void */
    int64_t v33;
    int64_t v54;
    int64_t v45; /* placeholder for void */
    void* v8;
    int64_t v57;
    void* v50;
    int64_t v25; /* placeholder for void */
    void* v44;
    int64_t v42; /* placeholder for void */
    int64_t v43;
    int64_t v40;
    int64_t v34; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v59;
    int64_t v52; /* placeholder for void */
    int64_t v36;
    int64_t v4;
    bool v19;
    int64_t v28;
    int64_t v22;
    int64_t v31; /* placeholder for void */
    void* v11;
    int64_t v6; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v16;
    int64_t v38;
    int64_t v12;
    int64_t v7;
    int64_t v3;
    int64_t v13; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v15;
    void* v29;
    int64_t v21; /* placeholder for void */
    int64_t v27; /* placeholder for void */
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

void* Vec_zip(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_zip");
    int64_t v3;
    int64_t v5; /* placeholder for void */
    int64_t v4;
    v3 = 0LL;
    v4 = (int64_t)vec_zip((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return (void*)v4;
    return 0;
}

void* str_trim_end(void* v0, void* v1) {
    p_call_stack_push("str_trim_end");
    int64_t v36; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v21;
    int64_t v23;
    void* v47;
    int64_t v6;
    int64_t v22; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v16;
    int64_t v30; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v10;
    uint8_t v28;
    bool v33;
    void* v27;
    void* v4;
    int64_t v17;
    int64_t v25; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v32;
    int64_t v44;
    int64_t v50;
    int64_t v15;
    int64_t v51;
    int64_t v52;
    bool v12;
    int64_t v53;
    void* v24;
    int64_t v14;
    int64_t v9; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    void* v7;
    int64_t v29; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    int64_t v39;
    int64_t v38;
    int64_t v26; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    void* v18;
    int64_t v35; /* placeholder for void */
    int64_t v43;
    int64_t v31;
    int64_t v20;
    int64_t v42; /* placeholder for void */
    bool v45;
    int64_t v3;
    int64_t v40;
    int64_t v2;
    int64_t v34;
    int64_t v11;
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
    v47 = _str5;
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

void assert_eq_bool(void* v0, bool v1, bool v2, void* v3) {
    p_call_stack_push("assert_eq_bool");
    int64_t v7; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    void* v6;
    int64_t v5; /* placeholder for void */
    void* v18;
    void* v12;
    int64_t v14; /* placeholder for void */
    void* v29;
    bool v4;
    void* v24;
    int64_t v17; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    void* v9;
    int64_t v8; /* placeholder for void */
    void* v27;
    int64_t v30; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    void* v15;
    int64_t v10; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    void* v21;
    int64_t v26; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    v4 = v1 != v2;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    v6 = _str0;
    p_print_str(v6);
    p_print_str(v3);
    v9 = _str6;
    p_print_str(v9);
    if (v2) goto bb4;
    else goto bb5;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return;
  bb4:
    v12 = _str7;
    p_print_str(v12);
    goto bb6;
  bb5:
    v15 = _str8;
    p_print_str(v15);
    goto bb6;
  bb6:
    v18 = _str9;
    p_print_str(v18);
    if (v1) goto bb7;
    else goto bb8;
  bb7:
    v21 = _str7;
    p_print_str(v21);
    goto bb9;
  bb8:
    v24 = _str8;
    p_print_str(v24);
    goto bb9;
  bb9:
    v27 = _str10;
    p_print_str(v27);
    v29 = _str4;
    p_panic(v29);
    goto bb3;
    return;
}

void EnumVariant_new(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("EnumVariant_new");
    void* v7;
    int64_t v30; /* placeholder for void */
    void* v4;
    int64_t v28;
    void* v5;
    void* v16;
    int64_t v19; /* placeholder for void */
    void* v23;
    void* v26;
    int64_t v11; /* placeholder for void */
    void* v13;
    void* v8;
    void* v22;
    int64_t v6; /* placeholder for void */
    int64_t v10;
    void* v20;
    void* v9;
    int64_t v25; /* placeholder for void */
    int64_t v14;
    int64_t v15; /* placeholder for void */
    void* v21;
    void* v27;
    void* v12;
    void* v17;
    void* v18;
    int64_t v29; /* placeholder for void */
    int64_t v24;
    v4 = malloc(24);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = (char*)v4 + 8;
    v8 = (char*)v3 + 0;
    v9 = (char*)v7 + 0;
    v10 = *(int64_t*)v8;
    *(int64_t*)v9 = (int64_t)v10;
    v12 = (char*)v3 + 8;
    v13 = (char*)v7 + 8;
    v14 = *(int64_t*)v12;
    *(int64_t*)v13 = (int64_t)v14;
    v16 = (char*)v4 + 0;
    v17 = (char*)v0 + 0;
    v18 = *(void**)v16;
    *(void**)v17 = (void*)v18;
    v20 = (char*)v4 + 8;
    v21 = (char*)v0 + 8;
    v22 = (char*)v20 + 0;
    v23 = (char*)v21 + 0;
    v24 = *(int64_t*)v22;
    *(int64_t*)v23 = (int64_t)v24;
    v26 = (char*)v20 + 8;
    v27 = (char*)v21 + 8;
    v28 = *(int64_t*)v26;
    *(int64_t*)v27 = (int64_t)v28;
    p_call_stack_pop();
    return;
    return;
}

bool String_ends_with(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_ends_with");
    bool v6;
    void* v18;
    void* v61;
    int64_t v44;
    bool v45;
    int64_t v48;
    int64_t v60;
    int64_t v3;
    int64_t v15; /* placeholder for void */
    void* v37;
    void* v67;
    int64_t v27;
    int64_t v12;
    int64_t v16;
    int64_t v53;
    int64_t v65; /* placeholder for void */
    void* v70;
    void* v73;
    bool v9;
    void* v55;
    int64_t v23; /* placeholder for void */
    void* v64;
    uint8_t v74;
    int64_t v72; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    void* v40;
    int64_t v32; /* placeholder for void */
    int64_t v39;
    int64_t v42; /* placeholder for void */
    void* v58;
    int64_t v83; /* placeholder for void */
    int64_t v84;
    int64_t v62; /* placeholder for void */
    int64_t v87; /* placeholder for void */
    int64_t v35;
    int64_t v56; /* placeholder for void */
    int64_t v88; /* placeholder for void */
    bool v89;
    int64_t v17;
    int64_t v80; /* placeholder for void */
    int64_t v86;
    bool v29;
    int64_t v20;
    int64_t v38; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v43;
    int64_t v68; /* placeholder for void */
    int64_t v34;
    int64_t v49;
    int64_t v77;
    int64_t v82; /* placeholder for void */
    bool v81;
    int64_t v28;
    int64_t v52; /* placeholder for void */
    int64_t v66;
    int64_t v11; /* placeholder for void */
    int64_t v63;
    int64_t v13;
    bool v24;
    int64_t v47;
    void* v51;
    int64_t v90; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    bool v79;
    bool v22;
    int64_t v30; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v57;
    int64_t v5;
    void* v14;
    int64_t v19; /* placeholder for void */
    int64_t v54;
    int64_t v50;
    uint8_t v71;
    int64_t v75; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v78;
    int64_t v59; /* placeholder for void */
    int64_t v85;
    bool v4;
    bool v31;
    int64_t v21;
    bool v7;
    int64_t v36;
    int64_t v46; /* placeholder for void */
    int64_t v25; /* placeholder for void */
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

bool str_is_alphanumeric(void* v0, void* v1) {
    p_call_stack_push("str_is_alphanumeric");
    bool v7;
    int64_t v20;
    int64_t v35; /* placeholder for void */
    int64_t v29;
    bool v30;
    int64_t v39;
    int64_t v28;
    int64_t v40; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    void* v11;
    int64_t v14;
    int64_t v37;
    int64_t v13; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v19;
    int64_t v38;
    int64_t v3;
    int64_t v31;
    int64_t v6; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v15;
    int64_t v18; /* placeholder for void */
    void* v22;
    bool v5;
    bool v26;
    bool v17;
    bool v34;
    int64_t v36; /* placeholder for void */
    int64_t v2;
    int64_t v24;
    bool v32;
    int64_t v16;
    int64_t v4;
    bool v42;
    int64_t v25;
    int64_t v33; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v21;
    int64_t v23; /* placeholder for void */
    int64_t v27;
    int64_t v10;
    v2 = 0LL;
    v3 = (int64_t)str_len((void*)v2, v1);
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb1;
    else goto bb2;
  bb1:
    v7 = false;
    p_call_stack_pop();
    return v7;
  bb2:
    goto bb3;
  bb3:
    v10 = 0LL;
    v11 = malloc(8);
    *(int64_t*)v11 = (int64_t)v10;
    goto bb4;
  bb4:
    v14 = *(int64_t*)v11;
    v15 = 0LL;
    v16 = (int64_t)str_len((void*)v15, v1);
    v17 = v14 < v16;
    if (v17) goto bb5;
    else goto bb6;
  bb5:
    v19 = *(int64_t*)v11;
    v20 = 0LL;
    v21 = (int64_t)str_byte_at((void*)v20, v1, v19);
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v21;
    v24 = *(int64_t*)v22;
    v25 = 0LL;
    v26 = (bool)char_is_alpha((void*)v25, v24);
    v27 = !v26;
    v28 = *(int64_t*)v22;
    v29 = 0LL;
    v30 = (bool)char_is_digit((void*)v29, v28);
    v31 = !v30;
    v32 = v27 && v31;
    if (v32) goto bb7;
    else goto bb8;
  bb6:
    v42 = true;
    p_call_stack_pop();
    return v42;
  bb7:
    v34 = false;
    p_call_stack_pop();
    return v34;
  bb8:
    goto bb9;
  bb9:
    v37 = *(int64_t*)v11;
    v38 = 1LL;
    v39 = v37 + v38;
    *(int64_t*)v11 = (int64_t)v39;
    goto bb4;
    return 0;
}

void println(void* v0, void* v1) {
    p_call_stack_push("println");
    void* v3;
    int64_t v4; /* placeholder for void */
    int64_t v2; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    p_print_str(v1);
    v3 = _str10;
    p_print_str(v3);
    p_call_stack_pop();
    return;
    return;
}

int64_t str_rfind(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_rfind");
    int64_t v4;
    int64_t v3;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)String_rfind((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

int64_t vec_take(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("vec_take");
    int64_t v3;
    int64_t v12; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v33;
    int64_t v25;
    int64_t v37;
    void* v9;
    int64_t v23; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v30;
    int64_t v28; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v21;
    int64_t v7;
    bool v15;
    int64_t v40;
    bool v27;
    int64_t v24; /* placeholder for void */
    int64_t v35;
    void* v22;
    void* v11;
    int64_t v32;
    int64_t v26;
    int64_t v17;
    int64_t v39; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v4;
    void* v5;
    int64_t v8;
    int64_t v20; /* placeholder for void */
    int64_t v29;
    int64_t v31;
    int64_t v16; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v36;
    int64_t v14;
    int64_t v13;
    v3 = 0LL;
    v4 = (int64_t)Vec_new((void*)v3);
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = 0LL;
    v8 = (int64_t)Vec_len((void*)v7, (void*)v1);
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = malloc(8);
    *(int64_t*)v11 = (int64_t)v2;
    v13 = *(int64_t*)v11;
    v14 = *(int64_t*)v9;
    v15 = v13 > v14;
    if (v15) goto bb1;
    else goto bb2;
  bb1:
    v17 = *(int64_t*)v9;
    *(int64_t*)v11 = (int64_t)v17;
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v21 = 0LL;
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v21;
    goto bb4;
  bb4:
    v25 = *(int64_t*)v22;
    v26 = *(int64_t*)v11;
    v27 = v25 < v26;
    if (v27) goto bb5;
    else goto bb6;
  bb5:
    v29 = *(int64_t*)v5;
    v30 = *(int64_t*)v22;
    v31 = 0LL;
    v32 = (int64_t)Vec_get((void*)v31, (void*)v1, v30);
    v33 = 0LL;
    Vec_push((void*)v33, (void*)v29, v32);
    v35 = *(int64_t*)v22;
    v36 = 1LL;
    v37 = v35 + v36;
    *(int64_t*)v22 = (int64_t)v37;
    goto bb4;
  bb6:
    v40 = *(int64_t*)v5;
    p_call_stack_pop();
    return v40;
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

int64_t char_to_lower(void* v0, int64_t v1) {
    p_call_stack_push("char_to_lower");
    int64_t v11; /* placeholder for void */
    bool v3;
    bool v6;
    int64_t v8;
    int64_t v10; /* placeholder for void */
    bool v5;
    int64_t v7; /* placeholder for void */
    int64_t v9;
    int64_t v12; /* placeholder for void */
    int64_t v2;
    int64_t v4;
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

int64_t vec_enumerate(void* v0, int64_t v1) {
    p_call_stack_push("vec_enumerate");
    int64_t v6;
    int64_t v5; /* placeholder for void */
    int64_t v2;
    int64_t v30;
    bool v16;
    int64_t v31; /* placeholder for void */
    int64_t v33;
    int64_t v15;
    int64_t v29;
    int64_t v34; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v28;
    void* v4;
    void* v11;
    int64_t v23;
    int64_t v32; /* placeholder for void */
    int64_t v14;
    int64_t v21; /* placeholder for void */
    int64_t v26;
    int64_t v18;
    int64_t v24;
    int64_t v17; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v10;
    int64_t v27; /* placeholder for void */
    int64_t v19;
    int64_t v3;
    int64_t v7;
    void* v8;
    int64_t v22;
    int64_t v20;
    int64_t v25;
    int64_t v13; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)Vec_new((void*)v2);
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = 0LL;
    v7 = (int64_t)Vec_len((void*)v6, (void*)v1);
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
    v18 = *(int64_t*)v4;
    v19 = *(int64_t*)v11;
    v20 = 0LL;
    Vec_push((void*)v20, (void*)v18, v19);
    v22 = *(int64_t*)v4;
    v23 = *(int64_t*)v11;
    v24 = 0LL;
    v25 = (int64_t)Vec_get((void*)v24, (void*)v1, v23);
    v26 = 0LL;
    Vec_push((void*)v26, (void*)v22, v25);
    v28 = *(int64_t*)v11;
    v29 = 1LL;
    v30 = v28 + v29;
    *(int64_t*)v11 = (int64_t)v30;
    goto bb1;
  bb3:
    v33 = *(int64_t*)v4;
    p_call_stack_pop();
    return v33;
    return 0;
}

void ConstDef_new(void* v0, void* v1, void* v2, void* v3, void* v4, bool v5) {
    p_call_stack_push("ConstDef_new");
    int64_t v57;
    void* v78;
    void* v27;
    int64_t v34; /* placeholder for void */
    void* v47;
    void* v56;
    int64_t v73;
    int64_t v88; /* placeholder for void */
    void* v44;
    int64_t v16;
    void* v36;
    void* v71;
    int64_t v80; /* placeholder for void */
    int64_t v24;
    int64_t v92; /* placeholder for void */
    void* v10;
    int64_t v84; /* placeholder for void */
    void* v59;
    void* v60;
    void* v14;
    int64_t v33;
    void* v40;
    void* v50;
    int64_t v87;
    int64_t v91;
    void* v63;
    int64_t v66; /* placeholder for void */
    int64_t v20;
    int64_t v79;
    int64_t v12;
    int64_t v52; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    void* v76;
    int64_t v70; /* placeholder for void */
    int64_t v37;
    int64_t v42; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    void* v85;
    void* v11;
    void* v72;
    int64_t v28;
    int64_t v21; /* placeholder for void */
    void* v89;
    void* v90;
    void* v9;
    void* v93;
    void* v94;
    int64_t v97; /* placeholder for void */
    void* v81;
    void* v7;
    void* v53;
    int64_t v41;
    void* v15;
    void* v19;
    int64_t v74; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v18;
    void* v23;
    void* v32;
    void* v49;
    int64_t v69;
    void* v77;
    void* v54;
    void* v35;
    void* v68;
    void* v30;
    void* v31;
    int64_t v45;
    int64_t v8; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v83;
    void* v55;
    int64_t v96; /* placeholder for void */
    void* v39;
    void* v26;
    int64_t v58; /* placeholder for void */
    void* v64;
    int64_t v29; /* placeholder for void */
    void* v43;
    int64_t v13; /* placeholder for void */
    void* v22;
    void* v67;
    bool v95;
    void* v51;
    int64_t v61;
    void* v86;
    void* v6;
    void* v82;
    int64_t v65;
    void* v75;
    v6 = malloc(88);
    v7 = (char*)v6 + 0;
    *(void**)v7 = (void*)v2;
    v9 = (char*)v6 + 8;
    v10 = (char*)v3 + 0;
    v11 = (char*)v9 + 0;
    v12 = *(int64_t*)v10;
    *(int64_t*)v11 = (int64_t)v12;
    v14 = (char*)v3 + 8;
    v15 = (char*)v9 + 8;
    v16 = *(int64_t*)v14;
    *(int64_t*)v15 = (int64_t)v16;
    v18 = (char*)v3 + 16;
    v19 = (char*)v9 + 16;
    v20 = *(int64_t*)v18;
    *(int64_t*)v19 = (int64_t)v20;
    v22 = (char*)v3 + 24;
    v23 = (char*)v9 + 24;
    v24 = *(int64_t*)v22;
    *(int64_t*)v23 = (int64_t)v24;
    v26 = (char*)v3 + 32;
    v27 = (char*)v9 + 32;
    v28 = *(int64_t*)v26;
    *(int64_t*)v27 = (int64_t)v28;
    v30 = (char*)v6 + 48;
    v31 = (char*)v4 + 0;
    v32 = (char*)v30 + 0;
    v33 = *(int64_t*)v31;
    *(int64_t*)v32 = (int64_t)v33;
    v35 = (char*)v4 + 8;
    v36 = (char*)v30 + 8;
    v37 = *(int64_t*)v35;
    *(int64_t*)v36 = (int64_t)v37;
    v39 = (char*)v4 + 16;
    v40 = (char*)v30 + 16;
    v41 = *(int64_t*)v39;
    *(int64_t*)v40 = (int64_t)v41;
    v43 = (char*)v4 + 24;
    v44 = (char*)v30 + 24;
    v45 = *(int64_t*)v43;
    *(int64_t*)v44 = (int64_t)v45;
    v47 = (char*)v6 + 80;
    *(bool*)v47 = v5;
    v49 = (char*)v6 + 0;
    v50 = (char*)v0 + 0;
    v51 = *(void**)v49;
    *(void**)v50 = (void*)v51;
    v53 = (char*)v6 + 8;
    v54 = (char*)v0 + 8;
    v55 = (char*)v53 + 0;
    v56 = (char*)v54 + 0;
    v57 = *(int64_t*)v55;
    *(int64_t*)v56 = (int64_t)v57;
    v59 = (char*)v53 + 8;
    v60 = (char*)v54 + 8;
    v61 = *(int64_t*)v59;
    *(int64_t*)v60 = (int64_t)v61;
    v63 = (char*)v53 + 16;
    v64 = (char*)v54 + 16;
    v65 = *(int64_t*)v63;
    *(int64_t*)v64 = (int64_t)v65;
    v67 = (char*)v53 + 24;
    v68 = (char*)v54 + 24;
    v69 = *(int64_t*)v67;
    *(int64_t*)v68 = (int64_t)v69;
    v71 = (char*)v53 + 32;
    v72 = (char*)v54 + 32;
    v73 = *(int64_t*)v71;
    *(int64_t*)v72 = (int64_t)v73;
    v75 = (char*)v6 + 48;
    v76 = (char*)v0 + 48;
    v77 = (char*)v75 + 0;
    v78 = (char*)v76 + 0;
    v79 = *(int64_t*)v77;
    *(int64_t*)v78 = (int64_t)v79;
    v81 = (char*)v75 + 8;
    v82 = (char*)v76 + 8;
    v83 = *(int64_t*)v81;
    *(int64_t*)v82 = (int64_t)v83;
    v85 = (char*)v75 + 16;
    v86 = (char*)v76 + 16;
    v87 = *(int64_t*)v85;
    *(int64_t*)v86 = (int64_t)v87;
    v89 = (char*)v75 + 24;
    v90 = (char*)v76 + 24;
    v91 = *(int64_t*)v89;
    *(int64_t*)v90 = (int64_t)v91;
    v93 = (char*)v6 + 80;
    v94 = (char*)v0 + 80;
    v95 = *(bool*)v93;
    *(bool*)v94 = v95;
    p_call_stack_pop();
    return;
    return;
}

int64_t Vec_capacity(void* v0, void* v1) {
    p_call_stack_push("Vec_capacity");
    int64_t v2;
    int64_t v3; /* placeholder for void */
    v2 = (int64_t)p_vec_capacity(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void vec_sort_by(void* v0, int64_t v1, void* v2) {
    p_call_stack_push("vec_sort_by");
    int64_t v33;
    int64_t v26;
    int64_t v20;
    int64_t v24; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v12;
    int64_t v42;
    int64_t v6; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    int64_t v16;
    int64_t v21;
    int64_t v48;
    int64_t v44;
    int64_t v53;
    int64_t v58; /* placeholder for void */
    int64_t v43;
    int64_t v19; /* placeholder for void */
    int64_t v22;
    int64_t v59;
    void* v18;
    int64_t v57;
    int64_t v4;
    int64_t v62; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v3;
    int64_t v17;
    int64_t v11;
    int64_t v29; /* placeholder for void */
    int64_t v30;
    int64_t v61;
    int64_t v49;
    int64_t v32;
    int64_t v55;
    int64_t v54;
    int64_t v60;
    int64_t v34;
    bool v28;
    int64_t v50;
    int64_t v15;
    int64_t v45;
    int64_t v38; /* placeholder for void */
    int64_t v40;
    void* v8;
    int64_t v41;
    int64_t v25; /* placeholder for void */
    int64_t v56;
    bool v13;
    int64_t v10; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v27;
    int64_t v9; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v7;
    void* v5;
    void* v23;
    int64_t v46;
    bool v36;
    int64_t v35;
    int64_t v31;
    int64_t v64; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)Vec_len((void*)v3, (void*)v1);
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = 1LL;
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    goto bb1;
  bb1:
    v11 = *(int64_t*)v8;
    v12 = *(int64_t*)v5;
    v13 = v11 < v12;
    if (v13) goto bb2;
    else goto bb3;
  bb2:
    v15 = *(int64_t*)v8;
    v16 = 0LL;
    v17 = (int64_t)Vec_get((void*)v16, (void*)v1, v15);
    v18 = malloc(8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = *(int64_t*)v8;
    v21 = 1LL;
    v22 = v20 - v21;
    v23 = malloc(8);
    *(int64_t*)v23 = (int64_t)v22;
    goto bb4;
  bb3:
    p_call_stack_pop();
    return;
  bb4:
    v26 = *(int64_t*)v23;
    v27 = 0LL;
    v28 = v26 >= v27;
    if (v28) goto bb5;
    else goto bb6;
  bb5:
    v30 = *(int64_t*)v23;
    v31 = 0LL;
    v32 = (int64_t)Vec_get((void*)v31, (void*)v1, v30);
    v33 = *(int64_t*)v18;
    {
        if (!v2) abort();
        char* _fn_name = *(char**)v2;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "str_count") == 0) {
            v34 = str_count((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "String_push_str") == 0) {
            v34 = String_push_str((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "vec_skip") == 0) {
            v34 = vec_skip((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Option_unwrap_or") == 0) {
            v34 = Option_unwrap_or((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Vec_get") == 0) {
            v34 = Vec_get((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "String_repeat") == 0) {
            v34 = String_repeat((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Result_unwrap_or_else") == 0) {
            v34 = Result_unwrap_or_else((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "str_rfind") == 0) {
            v34 = str_rfind((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "Option_unwrap_or_else") == 0) {
            v34 = Option_unwrap_or_else((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "vec_chunk") == 0) {
            v34 = vec_chunk((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "str_byte_at") == 0) {
            v34 = str_byte_at((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "vec_window") == 0) {
            v34 = vec_window((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "vec_take") == 0) {
            v34 = vec_take((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Result_expect") == 0) {
            v34 = Result_expect((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "String_byte_at") == 0) {
            v34 = String_byte_at((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "String_find") == 0) {
            v34 = String_find((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Vec_find") == 0) {
            v34 = Vec_find((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "max") == 0) {
            v34 = p_max((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "String_split") == 0) {
            v34 = String_split((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Vec_remove") == 0) {
            v34 = Vec_remove((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "p_str_compare") == 0) {
            v34 = p_str_compare((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "str_compare") == 0) {
            v34 = str_compare((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "Vec_position") == 0) {
            v34 = Vec_position((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "Result_unwrap_or") == 0) {
            v34 = Result_unwrap_or((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Option_expect") == 0) {
            v34 = Option_expect((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "String_rfind") == 0) {
            v34 = String_rfind((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "str_find") == 0) {
            v34 = str_find((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "ByteBuffer_read_byte") == 0) {
            v34 = ByteBuffer_read_byte((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "allocator_alloc") == 0) {
            v34 = allocator_alloc((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "HashMap_get") == 0) {
            v34 = HashMap_get((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "vec_zip") == 0) {
            v34 = vec_zip((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "str_count_char") == 0) {
            v34 = str_count_char((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "HashMap_find_slot") == 0) {
            v34 = HashMap_find_slot((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "min") == 0) {
            v34 = p_min((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "String_char_at") == 0) {
            v34 = String_char_at((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "vec_flat_map") == 0) {
            v34 = vec_flat_map((void*)v2, (int64_t)v32, (void*)v33);
        } else { abort(); }
    }
    v35 = 0LL;
    v36 = v34 <= v35;
    if (v36) goto bb7;
    else goto bb8;
  bb6:
    v53 = *(int64_t*)v23;
    v54 = 1LL;
    v55 = v53 + v54;
    v56 = *(int64_t*)v18;
    v57 = 0LL;
    Vec_set((void*)v57, (void*)v1, v55, v56);
    v59 = *(int64_t*)v8;
    v60 = 1LL;
    v61 = v59 + v60;
    *(int64_t*)v8 = (int64_t)v61;
    goto bb1;
  bb7:
    goto bb6;
  bb8:
    goto bb9;
  bb9:
    v40 = *(int64_t*)v23;
    v41 = 1LL;
    v42 = v40 + v41;
    v43 = *(int64_t*)v23;
    v44 = 0LL;
    v45 = (int64_t)Vec_get((void*)v44, (void*)v1, v43);
    v46 = 0LL;
    Vec_set((void*)v46, (void*)v1, v42, v45);
    v48 = *(int64_t*)v23;
    v49 = 1LL;
    v50 = v48 - v49;
    *(int64_t*)v23 = (int64_t)v50;
    goto bb4;
    return;
}

void AssocTypeBinding_new(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("AssocTypeBinding_new");
    void* v33;
    void* v13;
    void* v21;
    int64_t v18;
    void* v25;
    void* v32;
    int64_t v37; /* placeholder for void */
    void* v7;
    int64_t v40;
    int64_t v45; /* placeholder for void */
    void* v47;
    int64_t v48;
    void* v50;
    void* v9;
    int64_t v14;
    void* v17;
    int64_t v22;
    void* v30;
    int64_t v41; /* placeholder for void */
    void* v8;
    void* v20;
    void* v29;
    void* v34;
    void* v42;
    void* v46;
    int64_t v31; /* placeholder for void */
    int64_t v26;
    int64_t v54; /* placeholder for void */
    void* v28;
    int64_t v36;
    void* v35;
    int64_t v11; /* placeholder for void */
    void* v4;
    void* v12;
    void* v43;
    void* v51;
    int64_t v15; /* placeholder for void */
    int64_t v52;
    int64_t v23; /* placeholder for void */
    void* v39;
    int64_t v49; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    void* v38;
    void* v24;
    int64_t v27; /* placeholder for void */
    int64_t v44;
    int64_t v6; /* placeholder for void */
    int64_t v10;
    void* v5;
    void* v16;
    int64_t v19; /* placeholder for void */
    v4 = malloc(48);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = (char*)v4 + 8;
    v8 = (char*)v3 + 0;
    v9 = (char*)v7 + 0;
    v10 = *(int64_t*)v8;
    *(int64_t*)v9 = (int64_t)v10;
    v12 = (char*)v3 + 8;
    v13 = (char*)v7 + 8;
    v14 = *(int64_t*)v12;
    *(int64_t*)v13 = (int64_t)v14;
    v16 = (char*)v3 + 16;
    v17 = (char*)v7 + 16;
    v18 = *(int64_t*)v16;
    *(int64_t*)v17 = (int64_t)v18;
    v20 = (char*)v3 + 24;
    v21 = (char*)v7 + 24;
    v22 = *(int64_t*)v20;
    *(int64_t*)v21 = (int64_t)v22;
    v24 = (char*)v3 + 32;
    v25 = (char*)v7 + 32;
    v26 = *(int64_t*)v24;
    *(int64_t*)v25 = (int64_t)v26;
    v28 = (char*)v4 + 0;
    v29 = (char*)v0 + 0;
    v30 = *(void**)v28;
    *(void**)v29 = (void*)v30;
    v32 = (char*)v4 + 8;
    v33 = (char*)v0 + 8;
    v34 = (char*)v32 + 0;
    v35 = (char*)v33 + 0;
    v36 = *(int64_t*)v34;
    *(int64_t*)v35 = (int64_t)v36;
    v38 = (char*)v32 + 8;
    v39 = (char*)v33 + 8;
    v40 = *(int64_t*)v38;
    *(int64_t*)v39 = (int64_t)v40;
    v42 = (char*)v32 + 16;
    v43 = (char*)v33 + 16;
    v44 = *(int64_t*)v42;
    *(int64_t*)v43 = (int64_t)v44;
    v46 = (char*)v32 + 24;
    v47 = (char*)v33 + 24;
    v48 = *(int64_t*)v46;
    *(int64_t*)v47 = (int64_t)v48;
    v50 = (char*)v32 + 32;
    v51 = (char*)v33 + 32;
    v52 = *(int64_t*)v50;
    *(int64_t*)v51 = (int64_t)v52;
    p_call_stack_pop();
    return;
    return;
}

void* type_to_str(void* v0, void* v1) {
    p_call_stack_push("type_to_str");
    int64_t v5;
    int64_t v2;
    void* v3;
    int64_t v7; /* placeholder for void */
    void* v6;
    int64_t v4; /* placeholder for void */
    v2 = 8LL;
    v3 = p_malloc(v2);
    *(void**)v3 = (void*)v1;
    v5 = 0LL;
    v6 = box_type_to_str((void*)v5, v3);
    p_call_stack_pop();
    return v6;
    return 0;
}

int64_t String_len(void* v0, int64_t v1) {
    p_call_stack_push("String_len");
    int64_t v3;
    int64_t v4; /* placeholder for void */
    int64_t v2;
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t str_count_char(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("str_count_char");
    int64_t v26;
    int64_t v23; /* placeholder for void */
    int64_t v15;
    int64_t v32; /* placeholder for void */
    void* v4;
    int64_t v19; /* placeholder for void */
    int64_t v10;
    int64_t v22;
    bool v18;
    int64_t v25; /* placeholder for void */
    int64_t v6;
    int64_t v20;
    int64_t v21;
    bool v13;
    int64_t v29; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v11;
    int64_t v17;
    int64_t v3;
    int64_t v27;
    int64_t v14; /* placeholder for void */
    int64_t v31;
    int64_t v24; /* placeholder for void */
    int64_t v16;
    void* v7;
    int64_t v9; /* placeholder for void */
    int64_t v12;
    int64_t v8; /* placeholder for void */
    int64_t v28;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = 0LL;
    v7 = malloc(8);
    *(int64_t*)v7 = (int64_t)v6;
    goto bb1;
  bb1:
    v10 = *(int64_t*)v7;
    v11 = 0LL;
    v12 = (int64_t)str_len((void*)v11, v1);
    v13 = v10 < v12;
    if (v13) goto bb2;
    else goto bb3;
  bb2:
    v15 = *(int64_t*)v7;
    v16 = 0LL;
    v17 = (int64_t)str_byte_at((void*)v16, v1, v15);
    v18 = v17 == v2;
    if (v18) goto bb4;
    else goto bb5;
  bb3:
    v31 = *(int64_t*)v4;
    p_call_stack_pop();
    return v31;
  bb4:
    v20 = *(int64_t*)v4;
    v21 = 1LL;
    v22 = v20 + v21;
    *(int64_t*)v4 = (int64_t)v22;
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v26 = *(int64_t*)v7;
    v27 = 1LL;
    v28 = v26 + v27;
    *(int64_t*)v7 = (int64_t)v28;
    goto bb1;
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

void StringBuilder_append_str(void* v0, void* v1, void* v2) {
    p_call_stack_push("StringBuilder_append_str");
    void* v4;
    void* v3;
    void* v9;
    void* v5;
    int64_t v15; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v10;
    void* v8;
    int64_t v12;
    int64_t v13;
    void* v6;
    int64_t v11;
    int64_t v14; /* placeholder for void */
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

void Vec_extend(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_extend");
    int64_t v4; /* placeholder for void */
    int64_t v3; /* placeholder for void */
    p_vec_extend(v1, v2);
    p_call_stack_pop();
    return;
    return;
}

bool str_starts_with(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_starts_with");
    int64_t v5; /* placeholder for void */
    int64_t v3;
    bool v4;
    v3 = 0LL;
    v4 = (bool)String_starts_with((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

void ByteBuffer_clear(void* v0, void* v1) {
    p_call_stack_push("ByteBuffer_clear");
    int64_t v5; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    void* v3;
    void* v2;
    void* v6;
    int64_t v4;
    int64_t v8; /* placeholder for void */
    int64_t v7;
    v2 = (char*)v1 + 0;
    v3 = *(void**)v2;
    v4 = 0LL;
    Vec_clear((void*)v4, v3);
    v6 = (char*)v1 + 8;
    v7 = 0LL;
    *(int64_t*)v6 = (int64_t)v7;
    p_call_stack_pop();
    return;
    return;
}

int64_t vec_chunk(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("vec_chunk");
    int64_t v16;
    int64_t v24;
    int64_t v19;
    int64_t v6; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    void* v5;
    int64_t v11;
    int64_t v3;
    int64_t v4;
    int64_t v7;
    int64_t v18; /* placeholder for void */
    int64_t v8;
    int64_t v20;
    int64_t v25; /* placeholder for void */
    int64_t v27;
    int64_t v13; /* placeholder for void */
    bool v17;
    int64_t v14; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v9;
    int64_t v15;
    int64_t v21;
    int64_t v26; /* placeholder for void */
    int64_t v23;
    void* v12;
    v3 = 0LL;
    v4 = (int64_t)Vec_new((void*)v3);
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = 0LL;
    v8 = (int64_t)Vec_len((void*)v7, (void*)v1);
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
    v19 = *(int64_t*)v5;
    v20 = *(int64_t*)v12;
    v21 = 0LL;
    Vec_push((void*)v21, (void*)v19, v20);
    v23 = *(int64_t*)v12;
    v24 = v23 + v2;
    *(int64_t*)v12 = (int64_t)v24;
    goto bb1;
  bb3:
    v27 = *(int64_t*)v5;
    p_call_stack_pop();
    return v27;
    return 0;
}

void* str_reverse(void* v0, void* v1) {
    p_call_stack_push("str_reverse");
    int64_t v40; /* placeholder for void */
    int64_t v7;
    int64_t v17;
    int64_t v25; /* placeholder for void */
    int64_t v34;
    int64_t v15; /* placeholder for void */
    int64_t v28;
    int64_t v22;
    int64_t v33;
    void* v37;
    int64_t v30;
    int64_t v23;
    int64_t v2;
    int64_t v6;
    void* v13;
    void* v19;
    void* v26;
    int64_t v20; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v16;
    int64_t v32;
    int64_t v18;
    int64_t v36; /* placeholder for void */
    int64_t v38;
    bool v8;
    int64_t v35; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    void* v14;
    int64_t v3;
    int64_t v5; /* placeholder for void */
    void* v4;
    int64_t v12;
    int64_t v29;
    bool v24;
    int64_t v27;
    int64_t v11; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    void* v39;
    v2 = 0LL;
    v3 = (int64_t)str_len((void*)v2, v1);
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    v7 = 1LL;
    v8 = v6 <= v7;
    if (v8) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return v1;
  bb2:
    goto bb3;
  bb3:
    v12 = 0LL;
    v13 = Vec_new((void*)v12);
    v14 = malloc(8);
    *(void**)v14 = (void*)v13;
    v16 = *(int64_t*)v4;
    v17 = 1LL;
    v18 = v16 - v17;
    v19 = malloc(8);
    *(int64_t*)v19 = (int64_t)v18;
    goto bb4;
  bb4:
    v22 = *(int64_t*)v19;
    v23 = 0LL;
    v24 = v22 >= v23;
    if (v24) goto bb5;
    else goto bb6;
  bb5:
    v26 = *(void**)v14;
    v27 = *(int64_t*)v19;
    v28 = 0LL;
    v29 = (int64_t)str_byte_at((void*)v28, v1, v27);
    v30 = 0LL;
    Vec_push((void*)v30, v26, v29);
    v32 = *(int64_t*)v19;
    v33 = 1LL;
    v34 = v32 - v33;
    *(int64_t*)v19 = (int64_t)v34;
    goto bb4;
  bb6:
    v37 = *(void**)v14;
    v38 = 0LL;
    v39 = str_from_bytes((void*)v38, v37);
    p_call_stack_pop();
    return v39;
    return 0;
}

int64_t String_bytes(void* v0, int64_t v1) {
    p_call_stack_push("String_bytes");
    int64_t v4; /* placeholder for void */
    int64_t v3;
    int64_t v2;
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

void StateMachineDef_new(void* v0, void* v1, void* v2, bool v3) {
    p_call_stack_push("StateMachineDef_new");
    int64_t v10; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    void* v21;
    void* v13;
    void* v5;
    int64_t v12; /* placeholder for void */
    void* v8;
    void* v19;
    bool v23;
    int64_t v7;
    int64_t v16; /* placeholder for void */
    void* v4;
    void* v18;
    int64_t v6; /* placeholder for void */
    void* v17;
    void* v11;
    void* v22;
    int64_t v24; /* placeholder for void */
    void* v14;
    void* v15;
    void* v9;
    int64_t v25; /* placeholder for void */
    v4 = malloc(24);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = 0LL;
    v8 = Vec_new((void*)v7);
    v9 = (char*)v4 + 8;
    *(void**)v9 = (void*)v8;
    v11 = (char*)v4 + 16;
    *(bool*)v11 = v3;
    v13 = (char*)v4 + 0;
    v14 = (char*)v0 + 0;
    v15 = *(void**)v13;
    *(void**)v14 = (void*)v15;
    v17 = (char*)v4 + 8;
    v18 = (char*)v0 + 8;
    v19 = *(void**)v17;
    *(void**)v18 = (void*)v19;
    v21 = (char*)v4 + 16;
    v22 = (char*)v0 + 16;
    v23 = *(bool*)v21;
    *(bool*)v22 = v23;
    p_call_stack_pop();
    return;
    return;
}

void Func_new(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Func_new");
    int64_t v128; /* placeholder for void */
    void* v24;
    void* v36;
    void* v158;
    void* v151;
    void* v155;
    void* v59;
    void* v153;
    void* v141;
    int64_t v15;
    void* v41;
    void* v29;
    int64_t v43; /* placeholder for void */
    void* v8;
    void* v16;
    int64_t v18; /* placeholder for void */
    void* v73;
    void* v96;
    void* v122;
    bool v127;
    void* v134;
    bool v61;
    int64_t v75; /* placeholder for void */
    void* v138;
    bool v58;
    int64_t v156; /* placeholder for void */
    void* v70;
    int64_t v64;
    void* v49;
    int64_t v161; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    bool v131;
    int64_t v31; /* placeholder for void */
    void* v91;
    void* v157;
    int64_t v119;
    void* v62;
    void* v105;
    int64_t v10; /* placeholder for void */
    int64_t v26;
    int64_t v27; /* placeholder for void */
    int64_t v42;
    void* v121;
    int64_t v160; /* placeholder for void */
    int64_t v54;
    bool v143;
    int64_t v116; /* placeholder for void */
    int64_t v108; /* placeholder for void */
    int64_t v68;
    void* v133;
    void* v129;
    void* v45;
    void* v109;
    int64_t v14; /* placeholder for void */
    void* v95;
    int64_t v86; /* placeholder for void */
    void* v110;
    void* v37;
    int64_t v60; /* placeholder for void */
    void* v25;
    void* v17;
    int64_t v35; /* placeholder for void */
    void* v149;
    int64_t v120; /* placeholder for void */
    int64_t v136; /* placeholder for void */
    void* v137;
    void* v9;
    void* v100;
    void* v123;
    int64_t v71; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    void* v32;
    void* v33;
    void* v66;
    void* v79;
    void* v40;
    int64_t v44;
    void* v4;
    int64_t v104; /* placeholder for void */
    void* v154;
    int64_t v53; /* placeholder for void */
    void* v74;
    void* v52;
    void* v77;
    void* v81;
    int64_t v72;
    int64_t v67; /* placeholder for void */
    void* v92;
    int64_t v98; /* placeholder for void */
    void* v114;
    bool v139;
    int64_t v148; /* placeholder for void */
    bool v159;
    void* v69;
    void* v23;
    int64_t v124; /* placeholder for void */
    bool v76;
    void* v83;
    int64_t v144; /* placeholder for void */
    int64_t v152; /* placeholder for void */
    void* v55;
    void* v93;
    void* v46;
    void* v135;
    int64_t v7;
    int64_t v107;
    int64_t v112; /* placeholder for void */
    void* v117;
    void* v145;
    void* v126;
    int64_t v140; /* placeholder for void */
    int64_t v30;
    void* v12;
    void* v65;
    int64_t v34;
    int64_t v38;
    void* v99;
    int64_t v19;
    void* v20;
    void* v106;
    int64_t v111;
    void* v28;
    void* v150;
    void* v101;
    void* v5;
    int64_t v90; /* placeholder for void */
    bool v48;
    void* v97;
    int64_t v115;
    void* v142;
    void* v88;
    void* v87;
    void* v125;
    void* v146;
    int64_t v78; /* placeholder for void */
    void* v147;
    int64_t v22; /* placeholder for void */
    void* v113;
    void* v89;
    void* v56;
    int64_t v11;
    void* v80;
    void* v102;
    int64_t v132; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    void* v85;
    void* v13;
    int64_t v39; /* placeholder for void */
    int64_t v103;
    void* v84;
    int64_t v94; /* placeholder for void */
    bool v51;
    void* v130;
    int64_t v57; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    void* v21;
    void* v118;
    v4 = malloc(160);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = 0LL;
    v8 = Vec_new((void*)v7);
    v9 = (char*)v4 + 8;
    *(void**)v9 = (void*)v8;
    v11 = 0LL;
    v12 = Vec_new((void*)v11);
    v13 = (char*)v4 + 16;
    *(void**)v13 = (void*)v12;
    v15 = 0LL;
    v16 = Vec_new((void*)v15);
    v17 = (char*)v4 + 24;
    *(void**)v17 = (void*)v16;
    v19 = 0LL;
    v20 = Vec_new((void*)v19);
    v21 = (char*)v4 + 32;
    *(void**)v21 = (void*)v20;
    v23 = (char*)v4 + 40;
    v24 = (char*)v3 + 0;
    v25 = (char*)v23 + 0;
    v26 = *(int64_t*)v24;
    *(int64_t*)v25 = (int64_t)v26;
    v28 = (char*)v3 + 8;
    v29 = (char*)v23 + 8;
    v30 = *(int64_t*)v28;
    *(int64_t*)v29 = (int64_t)v30;
    v32 = (char*)v3 + 16;
    v33 = (char*)v23 + 16;
    v34 = *(int64_t*)v32;
    *(int64_t*)v33 = (int64_t)v34;
    v36 = (char*)v3 + 24;
    v37 = (char*)v23 + 24;
    v38 = *(int64_t*)v36;
    *(int64_t*)v37 = (int64_t)v38;
    v40 = (char*)v3 + 32;
    v41 = (char*)v23 + 32;
    v42 = *(int64_t*)v40;
    *(int64_t*)v41 = (int64_t)v42;
    v44 = 0LL;
    v45 = Vec_new((void*)v44);
    v46 = (char*)v4 + 80;
    *(void**)v46 = (void*)v45;
    v48 = false;
    v49 = (char*)v4 + 88;
    *(bool*)v49 = v48;
    v51 = false;
    v52 = (char*)v4 + 96;
    *(bool*)v52 = v51;
    v54 = 0LL;
    v55 = Vec_new((void*)v54);
    v56 = (char*)v4 + 104;
    *(void**)v56 = (void*)v55;
    v58 = false;
    v59 = (char*)v4 + 112;
    *(bool*)v59 = v58;
    v61 = false;
    v62 = (char*)v4 + 120;
    *(bool*)v62 = v61;
    v64 = 0LL;
    v65 = Vec_new((void*)v64);
    v66 = (char*)v4 + 128;
    *(void**)v66 = (void*)v65;
    v68 = 0LL;
    v69 = Vec_new((void*)v68);
    v70 = (char*)v4 + 136;
    *(void**)v70 = (void*)v69;
    v72 = 0LL;
    v73 = Vec_new((void*)v72);
    v74 = (char*)v4 + 144;
    *(void**)v74 = (void*)v73;
    v76 = false;
    v77 = (char*)v4 + 152;
    *(bool*)v77 = v76;
    v79 = (char*)v4 + 0;
    v80 = (char*)v0 + 0;
    v81 = *(void**)v79;
    *(void**)v80 = (void*)v81;
    v83 = (char*)v4 + 8;
    v84 = (char*)v0 + 8;
    v85 = *(void**)v83;
    *(void**)v84 = (void*)v85;
    v87 = (char*)v4 + 16;
    v88 = (char*)v0 + 16;
    v89 = *(void**)v87;
    *(void**)v88 = (void*)v89;
    v91 = (char*)v4 + 24;
    v92 = (char*)v0 + 24;
    v93 = *(void**)v91;
    *(void**)v92 = (void*)v93;
    v95 = (char*)v4 + 32;
    v96 = (char*)v0 + 32;
    v97 = *(void**)v95;
    *(void**)v96 = (void*)v97;
    v99 = (char*)v4 + 40;
    v100 = (char*)v0 + 40;
    v101 = (char*)v99 + 0;
    v102 = (char*)v100 + 0;
    v103 = *(int64_t*)v101;
    *(int64_t*)v102 = (int64_t)v103;
    v105 = (char*)v99 + 8;
    v106 = (char*)v100 + 8;
    v107 = *(int64_t*)v105;
    *(int64_t*)v106 = (int64_t)v107;
    v109 = (char*)v99 + 16;
    v110 = (char*)v100 + 16;
    v111 = *(int64_t*)v109;
    *(int64_t*)v110 = (int64_t)v111;
    v113 = (char*)v99 + 24;
    v114 = (char*)v100 + 24;
    v115 = *(int64_t*)v113;
    *(int64_t*)v114 = (int64_t)v115;
    v117 = (char*)v99 + 32;
    v118 = (char*)v100 + 32;
    v119 = *(int64_t*)v117;
    *(int64_t*)v118 = (int64_t)v119;
    v121 = (char*)v4 + 80;
    v122 = (char*)v0 + 80;
    v123 = *(void**)v121;
    *(void**)v122 = (void*)v123;
    v125 = (char*)v4 + 88;
    v126 = (char*)v0 + 88;
    v127 = *(bool*)v125;
    *(bool*)v126 = v127;
    v129 = (char*)v4 + 96;
    v130 = (char*)v0 + 96;
    v131 = *(bool*)v129;
    *(bool*)v130 = v131;
    v133 = (char*)v4 + 104;
    v134 = (char*)v0 + 104;
    v135 = *(void**)v133;
    *(void**)v134 = (void*)v135;
    v137 = (char*)v4 + 112;
    v138 = (char*)v0 + 112;
    v139 = *(bool*)v137;
    *(bool*)v138 = v139;
    v141 = (char*)v4 + 120;
    v142 = (char*)v0 + 120;
    v143 = *(bool*)v141;
    *(bool*)v142 = v143;
    v145 = (char*)v4 + 128;
    v146 = (char*)v0 + 128;
    v147 = *(void**)v145;
    *(void**)v146 = (void*)v147;
    v149 = (char*)v4 + 136;
    v150 = (char*)v0 + 136;
    v151 = *(void**)v149;
    *(void**)v150 = (void*)v151;
    v153 = (char*)v4 + 144;
    v154 = (char*)v0 + 144;
    v155 = *(void**)v153;
    *(void**)v154 = (void*)v155;
    v157 = (char*)v4 + 152;
    v158 = (char*)v0 + 152;
    v159 = *(bool*)v157;
    *(bool*)v158 = v159;
    p_call_stack_pop();
    return;
    return;
}

int64_t str_byte_at(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("str_byte_at");
    int64_t v5; /* placeholder for void */
    int64_t v4;
    int64_t v3;
    v3 = 0LL;
    v4 = (int64_t)String_byte_at((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

void* str_trim_start(void* v0, void* v1) {
    p_call_stack_push("str_trim_start");
    int64_t v33; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v6;
    int64_t v14;
    int64_t v36;
    void* v45;
    int64_t v50;
    int64_t v41;
    int64_t v20; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v11;
    bool v12;
    int64_t v15;
    int64_t v30;
    void* v4;
    void* v7;
    int64_t v9; /* placeholder for void */
    int64_t v10;
    int64_t v29;
    int64_t v34; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v2;
    int64_t v44; /* placeholder for void */
    int64_t v51;
    int64_t v47; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    void* v25;
    int64_t v8; /* placeholder for void */
    void* v19;
    int64_t v42;
    int64_t v48;
    int64_t v32;
    int64_t v37;
    int64_t v46; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v22;
    bool v31;
    int64_t v49;
    int64_t v18;
    int64_t v27; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v21;
    int64_t v38;
    bool v43;
    void* v16;
    int64_t v24; /* placeholder for void */
    int64_t v3;
    uint8_t v26;
    int64_t v52; /* placeholder for void */
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
    v45 = _str5;
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
    int64_t v4;
    int64_t v5; /* placeholder for void */
    int64_t v3;
    v3 = 0LL;
    v4 = (int64_t)String_repeat((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return (void*)v4;
    return 0;
}

bool ByteBuffer_is_empty(void* v0, void* v1) {
    p_call_stack_push("ByteBuffer_is_empty");
    int64_t v4;
    int64_t v6; /* placeholder for void */
    bool v5;
    void* v2;
    int64_t v3;
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    p_call_stack_pop();
    return v5;
    return 0;
}

int64_t p_str_length(void* v0, int64_t v1) {
    p_call_stack_push("p_str_length");
    int64_t v34;
    int64_t v9; /* placeholder for void */
    bool v3;
    int64_t v2;
    uint8_t v37;
    int64_t v25; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v26;
    int64_t v7; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v10;
    bool v24;
    int64_t v29; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v27;
    int64_t v39; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    void* v11;
    int64_t v12; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    int64_t v5;
    void* v14;
    int64_t v33; /* placeholder for void */
    int64_t v28;
    void* v36;
    void* v19;
    int64_t v41;
    int64_t v31;
    int64_t v21; /* placeholder for void */
    int64_t v13;
    int64_t v23;
    int64_t v30;
    int64_t v4; /* placeholder for void */
    uint8_t v18;
    int64_t v32;
    void* v17;
    int64_t v22;
    int64_t v38; /* placeholder for void */
    void* v8;
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

void Option_and_then(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Option_and_then");
    int64_t v44; /* placeholder for void */
    void* v11;
    void* v34;
    int64_t v46; /* placeholder for void */
    void* v22;
    void* v40;
    int64_t v17; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v28;
    int64_t v21; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    void* v37;
    int64_t v24;
    int64_t v31; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    void* v4;
    int64_t v8; /* placeholder for void */
    bool v7;
    void* v19;
    int64_t v10;
    int64_t v42;
    void* v18;
    int64_t v29;
    void* v9;
    int64_t v15;
    void* v32;
    void* v13;
    int64_t v43; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v20;
    int64_t v38;
    int64_t v5;
    int64_t v6;
    int64_t v14; /* placeholder for void */
    void* v16;
    void* v23;
    int64_t v33;
    void* v36;
    bool v30;
    void* v41;
    int64_t v12;
    void* v27;
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
        if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "count_expr_nodes") == 0) {
            v15 = count_expr_nodes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "op_tag") == 0) {
            v15 = op_tag((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "vec_enumerate") == 0) {
            v15 = vec_enumerate((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "stmt_line") == 0) {
            v15 = stmt_line((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
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

void HashMap_grow(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_grow");
    void* v53;
    int64_t v158;
    int64_t v24; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    int64_t v230; /* placeholder for void */
    int64_t v39;
    void* v59;
    int64_t v216; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v217; /* placeholder for void */
    int64_t v245;
    int64_t v119; /* placeholder for void */
    int64_t v252;
    int64_t v150;
    int64_t v164;
    int64_t v251; /* placeholder for void */
    void* v271;
    int64_t v192; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    int64_t v171; /* placeholder for void */
    void* v118;
    void* v10;
    int64_t v30;
    int64_t v186;
    int64_t v55;
    int64_t v195;
    void* v214;
    int64_t v255; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v200;
    int64_t v8;
    int64_t v57;
    int64_t v210;
    void* v265;
    int64_t v167; /* placeholder for void */
    int64_t v142;
    int64_t v264; /* placeholder for void */
    int64_t v234;
    void* v187;
    int64_t v16;
    void* v6;
    int64_t v117;
    int64_t v168;
    int64_t v242; /* placeholder for void */
    int64_t v73;
    void* v184;
    int64_t v125; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v188; /* placeholder for void */
    int64_t v197; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v170; /* placeholder for void */
    int64_t v180;
    bool v133;
    int64_t v224;
    int64_t v86;
    int64_t v162; /* placeholder for void */
    int64_t v51;
    bool v174;
    int64_t v19;
    int64_t v220;
    int64_t v266;
    int64_t v88; /* placeholder for void */
    int64_t v273; /* placeholder for void */
    int64_t v253; /* placeholder for void */
    int64_t v116;
    void* v161;
    int64_t v163;
    int64_t v203;
    int64_t v13;
    void* v20;
    int64_t v238;
    int64_t v152; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    void* v127;
    void* v166;
    int64_t v178;
    int64_t v60; /* placeholder for void */
    int64_t v135;
    int64_t v209; /* placeholder for void */
    int64_t v113;
    int64_t v83;
    int64_t v136;
    bool v250;
    void* v62;
    int64_t v46; /* placeholder for void */
    void* v52;
    int64_t v85; /* placeholder for void */
    int64_t v270; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    void* v93;
    int64_t v35; /* placeholder for void */
    int64_t v144;
    int64_t v204;
    int64_t v159;
    void* v124;
    bool v111;
    void* v268;
    int64_t v243;
    int64_t v207;
    int64_t v246; /* placeholder for void */
    int64_t v233;
    int64_t v198; /* placeholder for void */
    int64_t v130; /* placeholder for void */
    int64_t v134;
    bool v137;
    int64_t v56;
    int64_t v193; /* placeholder for void */
    int64_t v80;
    int64_t v241; /* placeholder for void */
    int64_t v108; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    int64_t v9;
    void* v274;
    int64_t v267; /* placeholder for void */
    int64_t v259; /* placeholder for void */
    bool v196;
    int64_t v221;
    int64_t v213; /* placeholder for void */
    void* v211;
    int64_t v40;
    int64_t v218; /* placeholder for void */
    bool v138;
    int64_t v54; /* placeholder for void */
    int64_t v97;
    int64_t v34; /* placeholder for void */
    int64_t v33;
    int64_t v172;
    int64_t v229; /* placeholder for void */
    int64_t v276; /* placeholder for void */
    int64_t v222;
    int64_t v43;
    bool v67;
    int64_t v36;
    int64_t v82;
    void* v23;
    void* v58;
    int64_t v225;
    int64_t v232;
    int64_t v61;
    int64_t v260;
    void* v14;
    int64_t v105;
    int64_t v115;
    int64_t v122; /* placeholder for void */
    int64_t v131;
    int64_t v240; /* placeholder for void */
    void* v151;
    int64_t v129; /* placeholder for void */
    int64_t v99;
    int64_t v71;
    void* v77;
    int64_t v256;
    int64_t v128;
    void* v87;
    int64_t v75; /* placeholder for void */
    int64_t v141;
    void* v90;
    void* v169;
    void* v74;
    int64_t v101; /* placeholder for void */
    int64_t v49;
    int64_t v5; /* placeholder for void */
    void* v103;
    int64_t v153; /* placeholder for void */
    int64_t v185; /* placeholder for void */
    int64_t v227; /* placeholder for void */
    int64_t v100; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v27;
    int64_t v22;
    int64_t v132;
    int64_t v25; /* placeholder for void */
    void* v106;
    int64_t v149; /* placeholder for void */
    int64_t v262; /* placeholder for void */
    int64_t v275;
    int64_t v94;
    int64_t v102;
    int64_t v212; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v247; /* placeholder for void */
    int64_t v194;
    bool v258;
    int64_t v239;
    void* v208;
    int64_t v123;
    int64_t v191;
    void* v29;
    int64_t v165;
    int64_t v31; /* placeholder for void */
    void* v199;
    int64_t v91;
    int64_t v114;
    bool v226;
    int64_t v261; /* placeholder for void */
    int64_t v189; /* placeholder for void */
    void* v26;
    int64_t v2;
    void* v37;
    int64_t v155;
    int64_t v18; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v44;
    int64_t v179;
    int64_t v236; /* placeholder for void */
    int64_t v254; /* placeholder for void */
    int64_t v146; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v219; /* placeholder for void */
    int64_t v182; /* placeholder for void */
    int64_t v126; /* placeholder for void */
    int64_t v231; /* placeholder for void */
    int64_t v249;
    int64_t v89; /* placeholder for void */
    int64_t v143;
    int64_t v176;
    int64_t v235; /* placeholder for void */
    int64_t v202; /* placeholder for void */
    int64_t v244;
    void* v17;
    int64_t v50;
    int64_t v76;
    int64_t v72;
    int64_t v110;
    int64_t v173;
    int64_t v177;
    int64_t v223; /* placeholder for void */
    int64_t v237;
    int64_t v278; /* placeholder for void */
    int64_t v109;
    int64_t v3;
    int64_t v65;
    int64_t v120;
    int64_t v263; /* placeholder for void */
    int64_t v66;
    int64_t v104; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    void* v84;
    int64_t v277; /* placeholder for void */
    int64_t v156; /* placeholder for void */
    void* v190;
    int64_t v79;
    void* v4;
    int64_t v139; /* placeholder for void */
    int64_t v147;
    int64_t v160;
    int64_t v15; /* placeholder for void */
    int64_t v69;
    int64_t v98;
    int64_t v183;
    int64_t v201; /* placeholder for void */
    int64_t v206;
    int64_t v175; /* placeholder for void */
    int64_t v12;
    void* v148;
    int64_t v157; /* placeholder for void */
    int64_t v81;
    void* v145;
    void* v154;
    int64_t v269;
    bool v41;
    int64_t v215;
    int64_t v107; /* placeholder for void */
    int64_t v272;
    void* v32;
    void* v181;
    int64_t v228;
    int64_t v70;
    int64_t v257;
    int64_t v112; /* placeholder for void */
    void* v121;
    int64_t v140;
    int64_t v45;
    int64_t v205;
    int64_t v248;
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

int64_t String_rfind(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_rfind");
    bool v4;
    int64_t v108; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    void* v80;
    void* v47;
    int64_t v32;
    int64_t v95;
    void* v71;
    int64_t v36;
    int64_t v66; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v5;
    int64_t v90; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v54;
    int64_t v107;
    int64_t v97;
    int64_t v64;
    int64_t v103; /* placeholder for void */
    int64_t v111;
    int64_t v112; /* placeholder for void */
    int64_t v28;
    int64_t v106;
    int64_t v10;
    int64_t v42;
    int64_t v105;
    int64_t v29;
    int64_t v14;
    bool v44;
    int64_t v63;
    int64_t v22;
    int64_t v37;
    int64_t v52; /* placeholder for void */
    int64_t v60;
    uint8_t v81;
    int64_t v70;
    int64_t v16; /* placeholder for void */
    void* v65;
    int64_t v78; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    int64_t v17;
    int64_t v41; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v109; /* placeholder for void */
    int64_t v76;
    int64_t v3;
    bool v30;
    void* v39;
    int64_t v9;
    bool v89;
    int64_t v25;
    int64_t v38;
    int64_t v12; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v57;
    int64_t v13;
    int64_t v56; /* placeholder for void */
    bool v6;
    void* v77;
    int64_t v93; /* placeholder for void */
    int64_t v33;
    bool v7;
    int64_t v58;
    int64_t v67;
    int64_t v98; /* placeholder for void */
    int64_t v110;
    int64_t v88;
    bool v91;
    int64_t v21;
    void* v83;
    int64_t v73;
    void* v50;
    int64_t v34; /* placeholder for void */
    void* v19;
    bool v100;
    int64_t v43;
    void* v68;
    int64_t v45; /* placeholder for void */
    int64_t v99; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v59;
    int64_t v27; /* placeholder for void */
    uint8_t v84;
    int64_t v86; /* placeholder for void */
    int64_t v96;
    int64_t v69; /* placeholder for void */
    bool v55;
    int64_t v31; /* placeholder for void */
    int64_t v87;
    int64_t v102;
    int64_t v94; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    void* v61;
    bool v23;
    int64_t v26; /* placeholder for void */
    bool v46;
    void* v74;
    int64_t v85; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v53;
    void* v15;
    int64_t v72; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v18;
    int64_t v49;
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

int64_t Vec_remove(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_remove");
    int64_t v3;
    int64_t v4; /* placeholder for void */
    v3 = (int64_t)p_vec_remove(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

void* HashMap_to_str(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_to_str");
    void* v120;
    int64_t v98;
    void* v123;
    int64_t v33;
    int64_t v66;
    int64_t v48;
    int64_t v80; /* placeholder for void */
    int64_t v52;
    int64_t v94; /* placeholder for void */
    int64_t v73;
    int64_t v36; /* placeholder for void */
    int64_t v121;
    int64_t v63; /* placeholder for void */
    int64_t v40;
    void* v130;
    int64_t v8;
    void* v134;
    bool v84;
    int64_t v53;
    void* v109;
    void* v116;
    void* v35;
    int64_t v64; /* placeholder for void */
    int64_t v42;
    int64_t v59;
    int64_t v32;
    int64_t v78; /* placeholder for void */
    int64_t v46;
    int64_t v31;
    int64_t v104;
    bool v17;
    int64_t v37;
    int64_t v119; /* placeholder for void */
    int64_t v107;
    void* v43;
    int64_t v135; /* placeholder for void */
    int64_t v49;
    void* v132;
    int64_t v102;
    void* v114;
    void* v117;
    void* v100;
    int64_t v72;
    int64_t v87;
    int64_t v133; /* placeholder for void */
    int64_t v54;
    int64_t v67;
    int64_t v82;
    int64_t v41;
    int64_t v14; /* placeholder for void */
    int64_t v68;
    int64_t v7;
    int64_t v13; /* placeholder for void */
    int64_t v128; /* placeholder for void */
    int64_t v60;
    void* v4;
    int64_t v83;
    int64_t v101; /* placeholder for void */
    int64_t v34;
    int64_t v18; /* placeholder for void */
    void* v92;
    int64_t v45;
    int64_t v95; /* placeholder for void */
    int64_t v86;
    int64_t v108;
    int64_t v75; /* placeholder for void */
    void* v9;
    int64_t v55;
    int64_t v57;
    int64_t v25;
    void* v90;
    void* v131;
    bool v29;
    void* v77;
    int64_t v115; /* placeholder for void */
    void* v110;
    int64_t v62;
    int64_t v26;
    int64_t v89; /* placeholder for void */
    void* v20;
    int64_t v6;
    int64_t v79;
    int64_t v10; /* placeholder for void */
    int64_t v19;
    int64_t v47;
    int64_t v27;
    void* v105;
    int64_t v23;
    int64_t v22; /* placeholder for void */
    int64_t v111; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    int64_t v125;
    int64_t v38;
    bool v88;
    int64_t v30; /* placeholder for void */
    int64_t v15;
    int64_t v97;
    int64_t v24;
    int64_t v51; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v11;
    int64_t v58;
    int64_t v39;
    void* v91;
    void* v112;
    int64_t v44; /* placeholder for void */
    int64_t v3;
    int64_t v16;
    void* v76;
    int64_t v85; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    void* v118;
    int64_t v2;
    void* v122;
    int64_t v61;
    int64_t v21; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v93; /* placeholder for void */
    void* v12;
    int64_t v96;
    int64_t v70; /* placeholder for void */
    int64_t v129; /* placeholder for void */
    bool v50;
    int64_t v71;
    int64_t v127;
    int64_t v99;
    int64_t v124; /* placeholder for void */
    void* v113;
    int64_t v126;
    int64_t v28;
    int64_t v103;
    int64_t v65; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)HashMap_keys((void*)v2, v1);
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    v7 = 0LL;
    v8 = (int64_t)Vec_len((void*)v7, (void*)v6);
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
    v76 = _str11;
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
    v34 = (int64_t)Vec_get((void*)v33, (void*)v31, v32);
    v35 = malloc(8);
    *(int64_t*)v35 = (int64_t)v34;
    v37 = *(int64_t*)v4;
    v38 = *(int64_t*)v20;
    v39 = 1LL;
    v40 = v38 + v39;
    v41 = 0LL;
    v42 = (int64_t)Vec_get((void*)v41, (void*)v37, v40);
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
    Vec_set((void*)v55, (void*)v52, v53, v54);
    v57 = *(int64_t*)v4;
    v58 = *(int64_t*)v20;
    v59 = 1LL;
    v60 = v58 + v59;
    v61 = *(int64_t*)v35;
    v62 = 0LL;
    Vec_set((void*)v62, (void*)v57, v60, v61);
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
    v131 = _str12;
    v132 = p_str_concat(v130, v131);
    *(void**)v77 = (void*)v132;
    v134 = *(void**)v77;
    p_call_stack_pop();
    return v134;
  bb13:
    v90 = *(void**)v77;
    v91 = _str13;
    v92 = p_str_concat(v90, v91);
    *(void**)v77 = (void*)v92;
    goto bb15;
  bb14:
    goto bb15;
  bb15:
    v96 = *(int64_t*)v4;
    v97 = *(int64_t*)v12;
    v98 = 0LL;
    v99 = (int64_t)Vec_get((void*)v98, (void*)v96, v97);
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
    v117 = _str14;
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

int64_t str_count(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_count");
    int64_t v13; /* placeholder for void */
    int64_t v51;
    void* v37;
    int64_t v8;
    int64_t v46;
    void* v34;
    int64_t v38;
    int64_t v9; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    int64_t v45;
    int64_t v24;
    int64_t v56;
    int64_t v44; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    int64_t v3;
    int64_t v23;
    int64_t v30;
    int64_t v49; /* placeholder for void */
    int64_t v5;
    void* v12;
    int64_t v29;
    void* v19;
    int64_t v36; /* placeholder for void */
    bool v27;
    int64_t v7; /* placeholder for void */
    int64_t v32;
    int64_t v4;
    int64_t v18;
    int64_t v31;
    int64_t v57; /* placeholder for void */
    int64_t v26;
    int64_t v28; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v22;
    int64_t v33;
    int64_t v42;
    int64_t v40; /* placeholder for void */
    void* v15;
    int64_t v50;
    int64_t v14;
    int64_t v16; /* placeholder for void */
    int64_t v25;
    bool v39;
    int64_t v52;
    int64_t v17;
    int64_t v21; /* placeholder for void */
    int64_t v41;
    int64_t v48; /* placeholder for void */
    int64_t v43;
    int64_t v47;
    int64_t v53; /* placeholder for void */
    bool v6;
    int64_t v11;
    void* v35;
    v3 = 0LL;
    v4 = (int64_t)str_len((void*)v3, v2);
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb1;
    else goto bb2;
  bb1:
    v8 = 0LL;
    p_call_stack_pop();
    return v8;
  bb2:
    goto bb3;
  bb3:
    v11 = 0LL;
    v12 = malloc(8);
    *(int64_t*)v12 = (int64_t)v11;
    v14 = 0LL;
    v15 = malloc(8);
    *(int64_t*)v15 = (int64_t)v14;
    v17 = 0LL;
    v18 = (int64_t)str_len((void*)v17, v2);
    v19 = malloc(8);
    *(int64_t*)v19 = (int64_t)v18;
    goto bb4;
  bb4:
    v22 = *(int64_t*)v15;
    v23 = 0LL;
    v24 = (int64_t)str_len((void*)v23, v1);
    v25 = *(int64_t*)v19;
    v26 = v24 - v25;
    v27 = v22 <= v26;
    if (v27) goto bb5;
    else goto bb6;
  bb5:
    v29 = *(int64_t*)v15;
    v30 = *(int64_t*)v15;
    v31 = *(int64_t*)v19;
    v32 = v30 + v31;
    v33 = 0LL;
    v34 = str_slice((void*)v33, v1, v29, v32);
    v35 = malloc(8);
    *(void**)v35 = (void*)v34;
    v37 = *(void**)v35;
    v38 = 0LL;
    v39 = (bool)str_equal((void*)v38, v37, v2);
    if (v39) goto bb7;
    else goto bb8;
  bb6:
    v56 = *(int64_t*)v12;
    p_call_stack_pop();
    return v56;
  bb7:
    v41 = *(int64_t*)v12;
    v42 = 1LL;
    v43 = v41 + v42;
    *(int64_t*)v12 = (int64_t)v43;
    v45 = *(int64_t*)v15;
    v46 = *(int64_t*)v19;
    v47 = v45 + v46;
    *(int64_t*)v15 = (int64_t)v47;
    goto bb9;
  bb8:
    v50 = *(int64_t*)v15;
    v51 = 1LL;
    v52 = v50 + v51;
    *(int64_t*)v15 = (int64_t)v52;
    goto bb9;
  bb9:
    goto bb4;
    return 0;
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

void Vec_dedup(void* v0, void* v1) {
    p_call_stack_push("Vec_dedup");
    int64_t v3; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v2;
    v2 = 0LL;
    vec_dedup((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return;
    return;
}

void* Vec_new(void* v0) {
    p_call_stack_push("Vec_new");
    int64_t v2; /* placeholder for void */
    int64_t v1;
    v1 = (int64_t)p_vec_new();
    p_call_stack_pop();
    return (void*)v1;
    return 0;
}

void Result_ok(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_ok");
    int64_t v7; /* placeholder for void */
    bool v6;
    int64_t v9;
    void* v12;
    int64_t v43; /* placeholder for void */
    int64_t v26;
    void* v10;
    int64_t v46; /* placeholder for void */
    void* v3;
    int64_t v39; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    void* v36;
    void* v30;
    int64_t v31;
    int64_t v5;
    int64_t v13; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v11;
    void* v20;
    void* v41;
    int64_t v19; /* placeholder for void */
    int64_t v33;
    int64_t v42;
    int64_t v44; /* placeholder for void */
    void* v14;
    void* v16;
    int64_t v38;
    int64_t v18;
    void* v34;
    int64_t v22;
    int64_t v24; /* placeholder for void */
    void* v8;
    void* v17;
    void* v25;
    bool v28;
    int64_t v15; /* placeholder for void */
    void* v40;
    void* v32;
    void* v21;
    void* v37;
    int64_t v4;
    int64_t v29; /* placeholder for void */
    int64_t v27;
    int64_t v23; /* placeholder for void */
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

bool String_is_empty(void* v0, int64_t v1) {
    p_call_stack_push("String_is_empty");
    bool v20;
    int64_t v12; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    void* v11;
    int64_t v17; /* placeholder for void */
    void* v14;
    int64_t v7; /* placeholder for void */
    int64_t v19;
    bool v3;
    int64_t v4; /* placeholder for void */
    int64_t v10;
    int64_t v13; /* placeholder for void */
    bool v5;
    void* v8;
    uint8_t v15;
    int64_t v16; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v2;
    int64_t v18;
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

int64_t Vec_pop(void* v0, void* v1) {
    p_call_stack_push("Vec_pop");
    int64_t v2;
    int64_t v3; /* placeholder for void */
    v2 = (int64_t)p_vec_pop(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void StructDef_new(void* v0, void* v1, void* v2, bool v3) {
    p_call_stack_push("StructDef_new");
    void* v35;
    int64_t v6; /* placeholder for void */
    void* v5;
    void* v29;
    void* v25;
    void* v27;
    void* v23;
    int64_t v40; /* placeholder for void */
    void* v39;
    void* v15;
    int64_t v17;
    void* v26;
    int64_t v10; /* placeholder for void */
    void* v11;
    bool v31;
    void* v14;
    int64_t v36; /* placeholder for void */
    void* v37;
    void* v33;
    void* v18;
    int64_t v12; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v7;
    int64_t v16; /* placeholder for void */
    void* v21;
    void* v30;
    void* v4;
    int64_t v13;
    void* v9;
    int64_t v32; /* placeholder for void */
    void* v34;
    int64_t v41; /* placeholder for void */
    void* v19;
    int64_t v20; /* placeholder for void */
    void* v38;
    void* v22;
    void* v8;
    int64_t v28; /* placeholder for void */
    v4 = malloc(40);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = 0LL;
    v8 = Vec_new((void*)v7);
    v9 = (char*)v4 + 8;
    *(void**)v9 = (void*)v8;
    v11 = (char*)v4 + 16;
    *(bool*)v11 = v3;
    v13 = 0LL;
    v14 = Vec_new((void*)v13);
    v15 = (char*)v4 + 24;
    *(void**)v15 = (void*)v14;
    v17 = 0LL;
    v18 = Vec_new((void*)v17);
    v19 = (char*)v4 + 32;
    *(void**)v19 = (void*)v18;
    v21 = (char*)v4 + 0;
    v22 = (char*)v0 + 0;
    v23 = *(void**)v21;
    *(void**)v22 = (void*)v23;
    v25 = (char*)v4 + 8;
    v26 = (char*)v0 + 8;
    v27 = *(void**)v25;
    *(void**)v26 = (void*)v27;
    v29 = (char*)v4 + 16;
    v30 = (char*)v0 + 16;
    v31 = *(bool*)v29;
    *(bool*)v30 = v31;
    v33 = (char*)v4 + 24;
    v34 = (char*)v0 + 24;
    v35 = *(void**)v33;
    *(void**)v34 = (void*)v35;
    v37 = (char*)v4 + 32;
    v38 = (char*)v0 + 32;
    v39 = *(void**)v37;
    *(void**)v38 = (void*)v39;
    p_call_stack_pop();
    return;
    return;
}

void StateTransition_new(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("StateTransition_new");
    int64_t v17; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    void* v7;
    void* v11;
    int64_t v6; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v10;
    void* v9;
    void* v4;
    void* v15;
    int64_t v12; /* placeholder for void */
    void* v13;
    void* v5;
    void* v14;
    v4 = malloc(16);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = (char*)v4 + 8;
    *(void**)v7 = (void*)v3;
    v9 = (char*)v4 + 0;
    v10 = (char*)v0 + 0;
    v11 = *(void**)v9;
    *(void**)v10 = (void*)v11;
    v13 = (char*)v4 + 8;
    v14 = (char*)v0 + 8;
    v15 = *(void**)v13;
    *(void**)v14 = (void*)v15;
    p_call_stack_pop();
    return;
    return;
}

int64_t String_clone(void* v0, int64_t v1) {
    p_call_stack_push("String_clone");
    int64_t v2;
    int64_t v3;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_from((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t char_to_upper(void* v0, int64_t v1) {
    p_call_stack_push("char_to_upper");
    int64_t v2;
    bool v3;
    int64_t v4;
    int64_t v12; /* placeholder for void */
    bool v5;
    int64_t v7; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    bool v6;
    int64_t v8;
    int64_t v9;
    int64_t v11; /* placeholder for void */
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

void SystemDef_new(void* v0, void* v1, void* v2) {
    p_call_stack_push("SystemDef_new");
    int64_t v13; /* placeholder for void */
    void* v27;
    int64_t v28; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    void* v22;
    int64_t v6;
    bool v14;
    void* v25;
    int64_t v9; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    void* v26;
    int64_t v33; /* placeholder for void */
    void* v15;
    int64_t v16; /* placeholder for void */
    void* v12;
    int64_t v5; /* placeholder for void */
    int64_t v10;
    void* v19;
    void* v23;
    int64_t v32; /* placeholder for void */
    void* v30;
    void* v11;
    void* v29;
    void* v3;
    void* v4;
    void* v18;
    void* v7;
    void* v21;
    void* v17;
    void* v8;
    bool v31;
    v3 = malloc(32);
    v4 = (char*)v3 + 0;
    *(void**)v4 = (void*)v2;
    v6 = 0LL;
    v7 = Vec_new((void*)v6);
    v8 = (char*)v3 + 8;
    *(void**)v8 = (void*)v7;
    v10 = 0LL;
    v11 = Vec_new((void*)v10);
    v12 = (char*)v3 + 16;
    *(void**)v12 = (void*)v11;
    v14 = false;
    v15 = (char*)v3 + 24;
    *(bool*)v15 = v14;
    v17 = (char*)v3 + 0;
    v18 = (char*)v0 + 0;
    v19 = *(void**)v17;
    *(void**)v18 = (void*)v19;
    v21 = (char*)v3 + 8;
    v22 = (char*)v0 + 8;
    v23 = *(void**)v21;
    *(void**)v22 = (void*)v23;
    v25 = (char*)v3 + 16;
    v26 = (char*)v0 + 16;
    v27 = *(void**)v25;
    *(void**)v26 = (void*)v27;
    v29 = (char*)v3 + 24;
    v30 = (char*)v0 + 24;
    v31 = *(bool*)v29;
    *(bool*)v30 = v31;
    p_call_stack_pop();
    return;
    return;
}

void* str_pad_left(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("str_pad_left");
    void* v18;
    void* v19;
    int64_t v13;
    int64_t v7; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v15;
    int64_t v20; /* placeholder for void */
    void* v21;
    int64_t v22;
    bool v9;
    int64_t v10; /* placeholder for void */
    void* v23;
    int64_t v24; /* placeholder for void */
    void* v6;
    int64_t v5;
    int64_t v12; /* placeholder for void */
    void* v14;
    int64_t v17;
    int64_t v8;
    int64_t v16;
    int64_t v4;
    v4 = 0LL;
    v5 = (int64_t)str_len((void*)v4, v1);
    v6 = malloc(8);
    *(int64_t*)v6 = (int64_t)v5;
    v8 = *(int64_t*)v6;
    v9 = v8 >= v2;
    if (v9) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return v1;
  bb2:
    goto bb3;
  bb3:
    v13 = 0LL;
    v14 = char_to_str((void*)v13, v3);
    v15 = *(int64_t*)v6;
    v16 = v2 - v15;
    v17 = 0LL;
    v18 = str_repeat((void*)v17, v14, v16);
    v19 = malloc(8);
    *(void**)v19 = (void*)v18;
    v21 = *(void**)v19;
    v22 = 0LL;
    v23 = str_concat((void*)v22, v21, v1);
    p_call_stack_pop();
    return v23;
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

void* int_to_str(void* v0, int64_t v1) {
    p_call_stack_push("int_to_str");
    void* v2;
    int64_t v3; /* placeholder for void */
    v2 = (void*)p_to_str((int64_t)v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void Vec_insert(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("Vec_insert");
    int64_t v5; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    p_vec_insert(v1, v2, (void*)v3);
    p_call_stack_pop();
    return;
    return;
}

void ComponentDef_new(void* v0, void* v1, void* v2, bool v3) {
    p_call_stack_push("ComponentDef_new");
    void* v15;
    void* v11;
    void* v19;
    int64_t v16; /* placeholder for void */
    void* v22;
    int64_t v25; /* placeholder for void */
    void* v8;
    int64_t v10; /* placeholder for void */
    void* v18;
    bool v23;
    int64_t v6; /* placeholder for void */
    void* v5;
    void* v4;
    void* v9;
    int64_t v12; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    void* v17;
    void* v13;
    void* v14;
    int64_t v7;
    int64_t v20; /* placeholder for void */
    void* v21;
    v4 = malloc(24);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = 0LL;
    v8 = Vec_new((void*)v7);
    v9 = (char*)v4 + 8;
    *(void**)v9 = (void*)v8;
    v11 = (char*)v4 + 16;
    *(bool*)v11 = v3;
    v13 = (char*)v4 + 0;
    v14 = (char*)v0 + 0;
    v15 = *(void**)v13;
    *(void**)v14 = (void*)v15;
    v17 = (char*)v4 + 8;
    v18 = (char*)v0 + 8;
    v19 = *(void**)v17;
    *(void**)v18 = (void*)v19;
    v21 = (char*)v4 + 16;
    v22 = (char*)v0 + 16;
    v23 = *(bool*)v21;
    *(bool*)v22 = v23;
    p_call_stack_pop();
    return;
    return;
}

void* str_join(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_join");
    int64_t v13;
    int64_t v9; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v12;
    int64_t v16;
    int64_t v15; /* placeholder for void */
    int64_t v36;
    void* v37;
    int64_t v21;
    int64_t v24; /* placeholder for void */
    bool v6;
    int64_t v20;
    int64_t v33; /* placeholder for void */
    void* v8;
    int64_t v28; /* placeholder for void */
    void* v34;
    int64_t v39;
    int64_t v42; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    void* v44;
    int64_t v10; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v22;
    int64_t v19; /* placeholder for void */
    bool v23;
    int64_t v38; /* placeholder for void */
    int64_t v31;
    int64_t v4;
    void* v27;
    void* v17;
    void* v35;
    int64_t v41;
    int64_t v5;
    void* v14;
    int64_t v30;
    void* v32;
    void* v25;
    int64_t v26;
    int64_t v3;
    int64_t v11;
    int64_t v29;
    int64_t v40;
    v3 = 0LL;
    v4 = (int64_t)Vec_len((void*)v3, v1);
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb1;
    else goto bb2;
  bb1:
    v8 = _str5;
    p_call_stack_pop();
    return v8;
  bb2:
    goto bb3;
  bb3:
    v11 = 0LL;
    v12 = 0LL;
    v13 = (int64_t)Vec_get((void*)v12, v1, v11);
    v14 = malloc(8);
    *(void**)v14 = (void*)v13;
    v16 = 1LL;
    v17 = malloc(8);
    *(int64_t*)v17 = (int64_t)v16;
    goto bb4;
  bb4:
    v20 = *(int64_t*)v17;
    v21 = 0LL;
    v22 = (int64_t)Vec_len((void*)v21, v1);
    v23 = v20 < v22;
    if (v23) goto bb5;
    else goto bb6;
  bb5:
    v25 = *(void**)v14;
    v26 = 0LL;
    v27 = str_concat((void*)v26, v25, v2);
    *(void**)v14 = (void*)v27;
    v29 = *(int64_t*)v17;
    v30 = 0LL;
    v31 = (int64_t)Vec_get((void*)v30, v1, v29);
    v32 = malloc(8);
    *(void**)v32 = (void*)v31;
    v34 = *(void**)v14;
    v35 = *(void**)v32;
    v36 = 0LL;
    v37 = str_concat((void*)v36, v34, v35);
    *(void**)v14 = (void*)v37;
    v39 = *(int64_t*)v17;
    v40 = 1LL;
    v41 = v39 + v40;
    *(int64_t*)v17 = (int64_t)v41;
    goto bb4;
  bb6:
    v44 = *(void**)v14;
    p_call_stack_pop();
    return v44;
    return 0;
}

bool p_str_equal(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("p_str_equal");
    int64_t v18; /* placeholder for void */
    void* v35;
    void* v25;
    int64_t v39; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    void* v22;
    int64_t v55;
    int64_t v57;
    int64_t v61;
    int64_t v63;
    void* v37;
    bool v46;
    void* v33;
    int64_t v10; /* placeholder for void */
    int64_t v8;
    int64_t v24;
    int64_t v48;
    uint8_t v36;
    int64_t v56;
    uint8_t v71;
    int64_t v7; /* placeholder for void */
    void* v20;
    int64_t v29; /* placeholder for void */
    bool v3;
    int64_t v49;
    int64_t v75;
    int64_t v66; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    bool v76;
    int64_t v59;
    bool v42;
    int64_t v27;
    int64_t v44;
    int64_t v14;
    int64_t v54; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    bool v52;
    bool v11;
    void* v31;
    int64_t v60;
    int64_t v69; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    uint8_t v68;
    int64_t v12; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    void* v70;
    int64_t v72; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v73; /* placeholder for void */
    bool v50;
    int64_t v51; /* placeholder for void */
    bool v15;
    int64_t v26; /* placeholder for void */
    bool v45;
    int64_t v41;
    int64_t v40;
    int64_t v74;
    int64_t v43;
    int64_t v77; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v65;
    void* v28;
    bool v17;
    uint8_t v32;
    bool v9;
    int64_t v21; /* placeholder for void */
    int64_t v58; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    void* v67;
    bool v5;
    int64_t v23; /* placeholder for void */
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

int64_t HashMap_new(void* v0) {
    p_call_stack_push("HashMap_new");
    int64_t v38; /* placeholder for void */
    void* v16;
    int64_t v14;
    int64_t v26; /* placeholder for void */
    void* v30;
    void* v6;
    int64_t v13;
    int64_t v17; /* placeholder for void */
    void* v24;
    int64_t v23; /* placeholder for void */
    void* v3;
    int64_t v18;
    int64_t v32; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v11;
    int64_t v28;
    int64_t v34;
    int64_t v4; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v15;
    int64_t v22; /* placeholder for void */
    void* v27;
    int64_t v35; /* placeholder for void */
    int64_t v19;
    int64_t v10;
    void* v33;
    int64_t v9;
    int64_t v1;
    int64_t v8;
    int64_t v37;
    void* v2;
    int64_t v5;
    int64_t v29; /* placeholder for void */
    int64_t v25;
    int64_t v12; /* placeholder for void */
    int64_t v31;
    void* v21;
    int64_t v20;
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

void MatchArm_new(void* v0, void* v1, void* v2, void* v3, void* v4) {
    p_call_stack_push("MatchArm_new");
    void* v41;
    int64_t v30;
    int64_t v27; /* placeholder for void */
    int64_t v38;
    void* v48;
    void* v64;
    void* v28;
    int64_t v39; /* placeholder for void */
    void* v35;
    int64_t v57; /* placeholder for void */
    int64_t v17;
    int64_t v18; /* placeholder for void */
    void* v62;
    void* v63;
    int64_t v51; /* placeholder for void */
    void* v29;
    void* v32;
    void* v52;
    int64_t v46;
    int64_t v31; /* placeholder for void */
    void* v34;
    void* v25;
    void* v19;
    int64_t v26;
    int64_t v13;
    void* v16;
    void* v24;
    int64_t v60;
    void* v37;
    int64_t v9;
    void* v15;
    void* v7;
    void* v53;
    int64_t v43; /* placeholder for void */
    void* v58;
    void* v45;
    void* v5;
    void* v44;
    void* v49;
    void* v59;
    int64_t v21;
    int64_t v42;
    void* v54;
    int64_t v10; /* placeholder for void */
    void* v12;
    void* v23;
    void* v36;
    void* v20;
    void* v8;
    int64_t v56;
    void* v6;
    int64_t v22; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    int64_t v50;
    void* v55;
    int64_t v65; /* placeholder for void */
    void* v40;
    void* v11;
    int64_t v66; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v61; /* placeholder for void */
    v5 = malloc(56);
    v6 = (char*)v5 + 0;
    v7 = (char*)v2 + 0;
    v8 = (char*)v6 + 0;
    v9 = *(int64_t*)v7;
    *(int64_t*)v8 = (int64_t)v9;
    v11 = (char*)v2 + 8;
    v12 = (char*)v6 + 8;
    v13 = *(int64_t*)v11;
    *(int64_t*)v12 = (int64_t)v13;
    v15 = (char*)v2 + 16;
    v16 = (char*)v6 + 16;
    v17 = *(int64_t*)v15;
    *(int64_t*)v16 = (int64_t)v17;
    v19 = (char*)v2 + 24;
    v20 = (char*)v6 + 24;
    v21 = *(int64_t*)v19;
    *(int64_t*)v20 = (int64_t)v21;
    v23 = (char*)v5 + 32;
    v24 = (char*)v3 + 0;
    v25 = (char*)v23 + 0;
    v26 = *(int64_t*)v24;
    *(int64_t*)v25 = (int64_t)v26;
    v28 = (char*)v3 + 8;
    v29 = (char*)v23 + 8;
    v30 = *(int64_t*)v28;
    *(int64_t*)v29 = (int64_t)v30;
    v32 = (char*)v5 + 48;
    *(void**)v32 = (void*)v4;
    v34 = (char*)v5 + 0;
    v35 = (char*)v0 + 0;
    v36 = (char*)v34 + 0;
    v37 = (char*)v35 + 0;
    v38 = *(int64_t*)v36;
    *(int64_t*)v37 = (int64_t)v38;
    v40 = (char*)v34 + 8;
    v41 = (char*)v35 + 8;
    v42 = *(int64_t*)v40;
    *(int64_t*)v41 = (int64_t)v42;
    v44 = (char*)v34 + 16;
    v45 = (char*)v35 + 16;
    v46 = *(int64_t*)v44;
    *(int64_t*)v45 = (int64_t)v46;
    v48 = (char*)v34 + 24;
    v49 = (char*)v35 + 24;
    v50 = *(int64_t*)v48;
    *(int64_t*)v49 = (int64_t)v50;
    v52 = (char*)v5 + 32;
    v53 = (char*)v0 + 32;
    v54 = (char*)v52 + 0;
    v55 = (char*)v53 + 0;
    v56 = *(int64_t*)v54;
    *(int64_t*)v55 = (int64_t)v56;
    v58 = (char*)v52 + 8;
    v59 = (char*)v53 + 8;
    v60 = *(int64_t*)v58;
    *(int64_t*)v59 = (int64_t)v60;
    v62 = (char*)v5 + 48;
    v63 = (char*)v0 + 48;
    v64 = *(void**)v62;
    *(void**)v63 = (void*)v64;
    p_call_stack_pop();
    return;
    return;
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

void Param_new(void* v0, void* v1, void* v2, void* v3, bool v4) {
    p_call_stack_push("Param_new");
    int64_t v64; /* placeholder for void */
    void* v57;
    void* v10;
    int64_t v23;
    void* v54;
    int64_t v59;
    void* v5;
    void* v67;
    void* v35;
    int64_t v19;
    int64_t v38; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    void* v41;
    void* v45;
    int64_t v7; /* placeholder for void */
    void* v9;
    void* v14;
    int64_t v47;
    int64_t v55;
    void* v53;
    int64_t v12; /* placeholder for void */
    void* v37;
    void* v50;
    void* v66;
    int64_t v27;
    void* v22;
    int64_t v31;
    int64_t v16; /* placeholder for void */
    void* v25;
    bool v63;
    void* v32;
    int64_t v11;
    int64_t v51;
    void* v8;
    int64_t v56; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    void* v21;
    void* v58;
    int64_t v43;
    void* v40;
    void* v46;
    int64_t v48; /* placeholder for void */
    void* v62;
    void* v61;
    void* v36;
    int64_t v60; /* placeholder for void */
    void* v49;
    void* v26;
    void* v42;
    void* v39;
    void* v13;
    int64_t v44; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v15;
    void* v17;
    void* v18;
    int64_t v20; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    void* v29;
    void* v65;
    int64_t v69; /* placeholder for void */
    void* v6;
    int64_t v52; /* placeholder for void */
    void* v33;
    v5 = malloc(64);
    v6 = (char*)v5 + 0;
    *(void**)v6 = (void*)v2;
    v8 = (char*)v5 + 8;
    v9 = (char*)v3 + 0;
    v10 = (char*)v8 + 0;
    v11 = *(int64_t*)v9;
    *(int64_t*)v10 = (int64_t)v11;
    v13 = (char*)v3 + 8;
    v14 = (char*)v8 + 8;
    v15 = *(int64_t*)v13;
    *(int64_t*)v14 = (int64_t)v15;
    v17 = (char*)v3 + 16;
    v18 = (char*)v8 + 16;
    v19 = *(int64_t*)v17;
    *(int64_t*)v18 = (int64_t)v19;
    v21 = (char*)v3 + 24;
    v22 = (char*)v8 + 24;
    v23 = *(int64_t*)v21;
    *(int64_t*)v22 = (int64_t)v23;
    v25 = (char*)v3 + 32;
    v26 = (char*)v8 + 32;
    v27 = *(int64_t*)v25;
    *(int64_t*)v26 = (int64_t)v27;
    v29 = (char*)v5 + 48;
    *(bool*)v29 = v4;
    v31 = 0LL;
    v32 = Vec_new((void*)v31);
    v33 = (char*)v5 + 56;
    *(void**)v33 = (void*)v32;
    v35 = (char*)v5 + 0;
    v36 = (char*)v0 + 0;
    v37 = *(void**)v35;
    *(void**)v36 = (void*)v37;
    v39 = (char*)v5 + 8;
    v40 = (char*)v0 + 8;
    v41 = (char*)v39 + 0;
    v42 = (char*)v40 + 0;
    v43 = *(int64_t*)v41;
    *(int64_t*)v42 = (int64_t)v43;
    v45 = (char*)v39 + 8;
    v46 = (char*)v40 + 8;
    v47 = *(int64_t*)v45;
    *(int64_t*)v46 = (int64_t)v47;
    v49 = (char*)v39 + 16;
    v50 = (char*)v40 + 16;
    v51 = *(int64_t*)v49;
    *(int64_t*)v50 = (int64_t)v51;
    v53 = (char*)v39 + 24;
    v54 = (char*)v40 + 24;
    v55 = *(int64_t*)v53;
    *(int64_t*)v54 = (int64_t)v55;
    v57 = (char*)v39 + 32;
    v58 = (char*)v40 + 32;
    v59 = *(int64_t*)v57;
    *(int64_t*)v58 = (int64_t)v59;
    v61 = (char*)v5 + 48;
    v62 = (char*)v0 + 48;
    v63 = *(bool*)v61;
    *(bool*)v62 = v63;
    v65 = (char*)v5 + 56;
    v66 = (char*)v0 + 56;
    v67 = *(void**)v65;
    *(void**)v66 = (void*)v67;
    p_call_stack_pop();
    return;
    return;
}

bool char_is_lower(void* v0, int64_t v1) {
    p_call_stack_push("char_is_lower");
    bool v3;
    int64_t v4;
    int64_t v7; /* placeholder for void */
    bool v6;
    int64_t v2;
    bool v5;
    v2 = 97LL;
    v3 = v1 >= v2;
    v4 = 122LL;
    v5 = v1 <= v4;
    v6 = v3 && v5;
    p_call_stack_pop();
    return v6;
    return 0;
}

void* str_center(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("str_center");
    int64_t v27;
    void* v45;
    int64_t v23;
    void* v32;
    int64_t v5;
    void* v6;
    int64_t v19;
    int64_t v21; /* placeholder for void */
    int64_t v30;
    int64_t v13;
    int64_t v35;
    void* v40;
    void* v34;
    void* v43;
    int64_t v10; /* placeholder for void */
    int64_t v14;
    int64_t v11; /* placeholder for void */
    void* v15;
    int64_t v29;
    bool v9;
    void* v38;
    void* v20;
    void* v36;
    int64_t v37; /* placeholder for void */
    int64_t v4;
    int64_t v16; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v28;
    int64_t v18;
    int64_t v22;
    void* v31;
    int64_t v12; /* placeholder for void */
    void* v47;
    int64_t v48; /* placeholder for void */
    int64_t v42;
    int64_t v33; /* placeholder for void */
    int64_t v24;
    int64_t v39;
    int64_t v44;
    int64_t v17;
    int64_t v8;
    int64_t v26; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v41;
    void* v25;
    v4 = 0LL;
    v5 = (int64_t)str_len((void*)v4, v1);
    v6 = malloc(8);
    *(int64_t*)v6 = (int64_t)v5;
    v8 = *(int64_t*)v6;
    v9 = v8 >= v2;
    if (v9) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return v1;
  bb2:
    goto bb3;
  bb3:
    v13 = *(int64_t*)v6;
    v14 = v2 - v13;
    v15 = malloc(8);
    *(int64_t*)v15 = (int64_t)v14;
    v17 = *(int64_t*)v15;
    v18 = 2LL;
    v19 = v17 / v18;
    v20 = malloc(8);
    *(int64_t*)v20 = (int64_t)v19;
    v22 = *(int64_t*)v15;
    v23 = *(int64_t*)v20;
    v24 = v22 - v23;
    v25 = malloc(8);
    *(int64_t*)v25 = (int64_t)v24;
    v27 = 0LL;
    v28 = char_to_str((void*)v27, v3);
    v29 = *(int64_t*)v20;
    v30 = 0LL;
    v31 = str_repeat((void*)v30, v28, v29);
    v32 = malloc(8);
    *(void**)v32 = (void*)v31;
    v34 = *(void**)v32;
    v35 = 0LL;
    v36 = str_concat((void*)v35, v34, v1);
    *(void**)v32 = (void*)v36;
    v38 = *(void**)v32;
    v39 = 0LL;
    v40 = char_to_str((void*)v39, v3);
    v41 = *(int64_t*)v25;
    v42 = 0LL;
    v43 = str_repeat((void*)v42, v40, v41);
    v44 = 0LL;
    v45 = str_concat((void*)v44, v38, v43);
    *(void**)v32 = (void*)v45;
    v47 = *(void**)v32;
    p_call_stack_pop();
    return v47;
    return 0;
}

void* Vec_skip(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_skip");
    int64_t v3;
    int64_t v5; /* placeholder for void */
    int64_t v4;
    v3 = 0LL;
    v4 = (int64_t)vec_skip((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return (void*)v4;
    return 0;
}

void* int_to_bin(void* v0, int64_t v1) {
    p_call_stack_push("int_to_bin");
    int64_t v50; /* placeholder for void */
    int64_t v60;
    int64_t v21;
    void* v44;
    int64_t v32;
    int64_t v24; /* placeholder for void */
    int64_t v2;
    int64_t v14; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v58;
    int64_t v62; /* placeholder for void */
    void* v12;
    bool v17;
    int64_t v53;
    void* v30;
    int64_t v31;
    int64_t v26;
    void* v25;
    bool v3;
    bool v54;
    void* v42;
    int64_t v52;
    int64_t v7; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    int64_t v63;
    int64_t v4; /* placeholder for void */
    int64_t v65;
    int64_t v36;
    int64_t v38; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v15;
    void* v49;
    int64_t v71; /* placeholder for void */
    int64_t v59;
    int64_t v28; /* placeholder for void */
    void* v9;
    int64_t v37;
    void* v57;
    void* v41;
    int64_t v67; /* placeholder for void */
    int64_t v48;
    int64_t v55; /* placeholder for void */
    int64_t v16;
    int64_t v61;
    int64_t v46;
    int64_t v8;
    int64_t v69;
    void* v10;
    int64_t v27;
    int64_t v13; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v20;
    void* v68;
    int64_t v51; /* placeholder for void */
    int64_t v35;
    int64_t v64;
    void* v5;
    int64_t v18; /* placeholder for void */
    void* v70;
    int64_t v47;
    int64_t v19;
    int64_t v40;
    void* v56;
    bool v23;
    int64_t v45;
    int64_t v22;
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = _str15;
    p_call_stack_pop();
    return v5;
  bb2:
    goto bb3;
  bb3:
    v8 = 0LL;
    v9 = Vec_new((void*)v8);
    v10 = malloc(8);
    *(void**)v10 = (void*)v9;
    v12 = malloc(8);
    *(int64_t*)v12 = (int64_t)v1;
    goto bb4;
  bb4:
    v15 = *(int64_t*)v12;
    v16 = 0LL;
    v17 = v15 > v16;
    if (v17) goto bb5;
    else goto bb6;
  bb5:
    v19 = *(int64_t*)v12;
    v20 = 1LL;
    v21 = v19 & v20;
    v22 = 1LL;
    v23 = v21 == v22;
    if (v23) goto bb7;
    else goto bb8;
  bb6:
    v40 = 0LL;
    v41 = Vec_new((void*)v40);
    v42 = malloc(8);
    *(void**)v42 = (void*)v41;
    v44 = *(void**)v10;
    v45 = 0LL;
    v46 = (int64_t)Vec_len((void*)v45, v44);
    v47 = 1LL;
    v48 = v46 - v47;
    v49 = malloc(8);
    *(int64_t*)v49 = (int64_t)v48;
    goto bb10;
  bb7:
    v25 = *(void**)v10;
    v26 = 49LL;
    v27 = 0LL;
    Vec_push((void*)v27, v25, v26);
    goto bb9;
  bb8:
    v30 = *(void**)v10;
    v31 = 48LL;
    v32 = 0LL;
    Vec_push((void*)v32, v30, v31);
    goto bb9;
  bb9:
    v35 = *(int64_t*)v12;
    v36 = 1LL;
    v37 = v35 >> v36;
    *(int64_t*)v12 = (int64_t)v37;
    goto bb4;
  bb10:
    v52 = *(int64_t*)v49;
    v53 = 0LL;
    v54 = v52 >= v53;
    if (v54) goto bb11;
    else goto bb12;
  bb11:
    v56 = *(void**)v42;
    v57 = *(void**)v10;
    v58 = *(int64_t*)v49;
    v59 = 0LL;
    v60 = (int64_t)Vec_get((void*)v59, v57, v58);
    v61 = 0LL;
    Vec_push((void*)v61, v56, v60);
    v63 = *(int64_t*)v49;
    v64 = 1LL;
    v65 = v63 - v64;
    *(int64_t*)v49 = (int64_t)v65;
    goto bb10;
  bb12:
    v68 = *(void**)v42;
    v69 = 0LL;
    v70 = str_from_bytes((void*)v69, v68);
    p_call_stack_pop();
    return v70;
    return 0;
}

void ByteBuffer_write_byte(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("ByteBuffer_write_byte");
    void* v7;
    void* v3;
    void* v8;
    int64_t v9;
    int64_t v6; /* placeholder for void */
    int64_t v5;
    void* v4;
    int64_t v10;
    int64_t v11;
    int64_t v12; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    v3 = (char*)v1 + 0;
    v4 = *(void**)v3;
    v5 = 0LL;
    Vec_push((void*)v5, v4, v2);
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

int64_t stmt_line(void* v0, void* v1) {
    p_call_stack_push("stmt_line");
    int64_t v359;
    void* v15;
    int64_t v136;
    bool v275;
    void* v299;
    void* v192;
    void* v244;
    int64_t v348;
    int64_t v237; /* placeholder for void */
    bool v262;
    int64_t v48; /* placeholder for void */
    void* v88;
    int64_t v239;
    void* v28;
    void* v210;
    void* v101;
    void* v249;
    void* v346;
    int64_t v186;
    int64_t v190;
    void* v13;
    void* v146;
    void* v65;
    int64_t v46;
    int64_t v194;
    void* v160;
    void* v260;
    void* v81;
    bool v264;
    void* v263;
    int64_t v152;
    int64_t v162;
    void* v40;
    void* v49;
    void* v180;
    bool v117;
    void* v38;
    void* v130;
    int64_t v168; /* placeholder for void */
    void* v198;
    int64_t v251; /* placeholder for void */
    int64_t v183; /* placeholder for void */
    int64_t v196; /* placeholder for void */
    int64_t v60;
    void* v164;
    void* v161;
    int64_t v218; /* placeholder for void */
    void* v292;
    void* v309;
    int64_t v311; /* placeholder for void */
    void* v84;
    bool v255;
    int64_t v204;
    int64_t v356;
    bool v229;
    bool v285;
    int64_t v306; /* placeholder for void */
    void* v302;
    void* v331;
    int64_t v118; /* placeholder for void */
    int64_t v125;
    int64_t v172;
    void* v232;
    int64_t v314;
    void* v226;
    void* v272;
    void* v67;
    int64_t v103; /* placeholder for void */
    int64_t v200;
    void* v96;
    int64_t v24; /* placeholder for void */
    int64_t v45;
    void* v109;
    int64_t v78; /* placeholder for void */
    void* v80;
    void* v158;
    void* v74;
    void* v189;
    int64_t v291; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v4;
    bool v217;
    int64_t v225; /* placeholder for void */
    int64_t v224;
    void* v50;
    void* v10;
    void* v64;
    int64_t v42;
    void* v184;
    bool v337;
    void* v169;
    int64_t v273;
    void* v8;
    int64_t v123; /* placeholder for void */
    int64_t v296; /* placeholder for void */
    int64_t v173; /* placeholder for void */
    void* v55;
    void* v32;
    void* v354;
    int64_t v75;
    bool v187;
    void* v287;
    void* v63;
    bool v241;
    void* v155;
    void* v319;
    void* v233;
    int64_t v18;
    void* v279;
    bool v167;
    void* v329;
    bool v305;
    void* v124;
    int64_t v336;
    void* v17;
    bool v35;
    void* v147;
    void* v231;
    int64_t v112;
    int64_t v90;
    void* v219;
    int64_t v3;
    int64_t v102;
    void* v129;
    int64_t v21;
    int64_t v30;
    void* v134;
    bool v14;
    int64_t v105;
    void* v220;
    int64_t v175;
    bool v23;
    int64_t v240;
    int64_t v313;
    void* v111;
    void* v341;
    void* v355;
    int64_t v115;
    int64_t v230; /* placeholder for void */
    void* v82;
    int64_t v6; /* placeholder for void */
    void* v25;
    void* v83;
    int64_t v106;
    void* v339;
    int64_t v137; /* placeholder for void */
    int64_t v325;
    void* v269;
    int64_t v178; /* placeholder for void */
    int64_t v316; /* placeholder for void */
    int64_t v254;
    bool v91;
    void* v259;
    void* v133;
    void* v278;
    void* v71;
    void* v132;
    int64_t v62; /* placeholder for void */
    int64_t v149; /* placeholder for void */
    int64_t v22;
    void* v58;
    void* v44;
    bool v107;
    void* v114;
    int64_t v126;
    void* v170;
    int64_t v227;
    void* v245;
    void* v258;
    int64_t v270;
    void* v282;
    void* v98;
    int64_t v122;
    int64_t v215;
    void* v317;
    void* v321;
    int64_t v360; /* placeholder for void */
    int64_t v310;
    int64_t v347;
    int64_t v151;
    int64_t v34;
    bool v177;
    void* v135;
    int64_t v357; /* placeholder for void */
    int64_t v274;
    void* v145;
    void* v342;
    void* v54;
    int64_t v154; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    int64_t v166;
    int64_t v148;
    int64_t v345; /* placeholder for void */
    void* v51;
    void* v37;
    void* v247;
    void* v351;
    bool v16;
    bool v47;
    int64_t v213; /* placeholder for void */
    int64_t v323; /* placeholder for void */
    void* v343;
    void* v171;
    int64_t v176;
    void* v353;
    void* v120;
    int64_t v163; /* placeholder for void */
    int64_t v216;
    void* v2;
    int64_t v182;
    int64_t v358; /* placeholder for void */
    void* v174;
    void* v143;
    bool v205;
    int64_t v293;
    void* v159;
    bool v195;
    void* v208;
    void* v199;
    int64_t v301; /* placeholder for void */
    void* v330;
    int64_t v335;
    void* v221;
    void* v209;
    bool v295;
    bool v315;
    int64_t v43; /* placeholder for void */
    void* v99;
    void* v318;
    void* v234;
    int64_t v284;
    void* v27;
    void* v66;
    int64_t v188; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    void* v26;
    void* v85;
    void* v243;
    int64_t v350; /* placeholder for void */
    int64_t v116;
    int64_t v76;
    void* v257;
    void* v94;
    void* v202;
    int64_t v33;
    void* v100;
    void* v110;
    void* v248;
    void* v131;
    void* v156;
    void* v268;
    void* v308;
    void* v41;
    int64_t v128; /* placeholder for void */
    void* v207;
    void* v320;
    void* v197;
    int64_t v185;
    int64_t v300;
    void* v93;
    bool v141;
    void* v211;
    void* v119;
    void* v214;
    int64_t v142; /* placeholder for void */
    int64_t v276; /* placeholder for void */
    void* v95;
    void* v288;
    void* v307;
    void* v97;
    int64_t v73; /* placeholder for void */
    bool v77;
    int64_t v56;
    int64_t v59;
    int64_t v113; /* placeholder for void */
    void* v223;
    void* v312;
    bool v153;
    int64_t v286; /* placeholder for void */
    int64_t v333; /* placeholder for void */
    void* v266;
    int64_t v326;
    int64_t v139;
    void* v70;
    int64_t v72;
    int64_t v191; /* placeholder for void */
    int64_t v256; /* placeholder for void */
    int64_t v290;
    bool v349;
    void* v79;
    void* v252;
    void* v7;
    void* v138;
    void* v277;
    int64_t v344;
    int64_t v108; /* placeholder for void */
    bool v127;
    void* v39;
    void* v150;
    void* v53;
    void* v157;
    int64_t v193;
    void* v297;
    void* v52;
    void* v29;
    int64_t v328; /* placeholder for void */
    void* v334;
    void* v121;
    bool v5;
    int64_t v281; /* placeholder for void */
    void* v12;
    int64_t v304;
    void* v324;
    void* v179;
    int64_t v19; /* placeholder for void */
    void* v69;
    int64_t v89;
    void* v144;
    int64_t v228;
    int64_t v242; /* placeholder for void */
    int64_t v165;
    void* v222;
    int64_t v250;
    void* v261;
    void* v267;
    int64_t v283;
    void* v289;
    int64_t v140;
    int64_t v294;
    int64_t v322;
    int64_t v338; /* placeholder for void */
    void* v340;
    void* v9;
    bool v61;
    int64_t v86;
    void* v246;
    bool v327;
    int64_t v36; /* placeholder for void */
    int64_t v253;
    int64_t v271; /* placeholder for void */
    int64_t v332;
    void* v238;
    void* v181;
    int64_t v203;
    void* v265;
    void* v11;
    void* v20;
    void* v104;
    int64_t v303;
    void* v68;
    int64_t v87; /* placeholder for void */
    int64_t v236;
    int64_t v280;
    int64_t v201; /* placeholder for void */
    void* v235;
    void* v298;
    int64_t v206; /* placeholder for void */
    void* v352;
    int64_t v212;
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v359 = 0LL;
    p_call_stack_pop();
    return v359;
  bb2:
    v7 = (char*)v1 + 8;
    v8 = *(void**)v7;
    v9 = (char*)v1 + 16;
    v10 = *(void**)v9;
    v11 = (char*)v1 + 24;
    v12 = *(void**)v11;
    v13 = (char*)v1 + 32;
    v14 = *(bool*)v13;
    v15 = (char*)v1 + 40;
    v16 = *(bool*)v15;
    v17 = (char*)v1 + 48;
    v18 = *(int64_t*)v17;
    p_call_stack_pop();
    return v18;
  bb3:
    v20 = (char*)v1 + 0;
    v21 = *(int64_t*)v20;
    v22 = 1LL;
    v23 = v21 == v22;
    if (v23) goto bb4;
    else goto bb5;
  bb4:
    v25 = (char*)v1 + 8;
    v26 = *(void**)v25;
    v27 = (char*)v1 + 16;
    v28 = *(void**)v27;
    v29 = (char*)v1 + 24;
    v30 = *(int64_t*)v29;
    p_call_stack_pop();
    return v30;
  bb5:
    v32 = (char*)v1 + 0;
    v33 = *(int64_t*)v32;
    v34 = 2LL;
    v35 = v33 == v34;
    if (v35) goto bb6;
    else goto bb7;
  bb6:
    v37 = (char*)v1 + 8;
    v38 = *(void**)v37;
    v39 = (char*)v1 + 16;
    v40 = *(void**)v39;
    v41 = (char*)v1 + 24;
    v42 = *(int64_t*)v41;
    p_call_stack_pop();
    return v42;
  bb7:
    v44 = (char*)v1 + 0;
    v45 = *(int64_t*)v44;
    v46 = 3LL;
    v47 = v45 == v46;
    if (v47) goto bb8;
    else goto bb9;
  bb8:
    v49 = (char*)v1 + 8;
    v50 = *(void**)v49;
    v51 = (char*)v1 + 16;
    v52 = *(void**)v51;
    v53 = (char*)v1 + 24;
    v54 = *(void**)v53;
    v55 = (char*)v1 + 32;
    v56 = *(int64_t*)v55;
    p_call_stack_pop();
    return v56;
  bb9:
    v58 = (char*)v1 + 0;
    v59 = *(int64_t*)v58;
    v60 = 4LL;
    v61 = v59 == v60;
    if (v61) goto bb10;
    else goto bb11;
  bb10:
    v63 = (char*)v1 + 8;
    v64 = *(void**)v63;
    v65 = (char*)v1 + 16;
    v66 = *(void**)v65;
    v67 = (char*)v1 + 24;
    v68 = *(void**)v67;
    v69 = (char*)v1 + 32;
    v70 = *(void**)v69;
    v71 = (char*)v1 + 40;
    v72 = *(int64_t*)v71;
    p_call_stack_pop();
    return v72;
  bb11:
    v74 = (char*)v1 + 0;
    v75 = *(int64_t*)v74;
    v76 = 5LL;
    v77 = v75 == v76;
    if (v77) goto bb12;
    else goto bb13;
  bb12:
    v79 = (char*)v1 + 8;
    v80 = *(void**)v79;
    v81 = (char*)v1 + 16;
    v82 = *(void**)v81;
    v83 = (char*)v1 + 24;
    v84 = *(void**)v83;
    v85 = (char*)v1 + 32;
    v86 = *(int64_t*)v85;
    p_call_stack_pop();
    return v86;
  bb13:
    v88 = (char*)v1 + 0;
    v89 = *(int64_t*)v88;
    v90 = 6LL;
    v91 = v89 == v90;
    if (v91) goto bb14;
    else goto bb15;
  bb14:
    v93 = (char*)v1 + 8;
    v94 = *(void**)v93;
    v95 = (char*)v1 + 16;
    v96 = *(void**)v95;
    v97 = (char*)v1 + 24;
    v98 = *(void**)v97;
    v99 = (char*)v1 + 32;
    v100 = *(void**)v99;
    v101 = (char*)v1 + 40;
    v102 = *(int64_t*)v101;
    p_call_stack_pop();
    return v102;
  bb15:
    v104 = (char*)v1 + 0;
    v105 = *(int64_t*)v104;
    v106 = 7LL;
    v107 = v105 == v106;
    if (v107) goto bb16;
    else goto bb17;
  bb16:
    v109 = (char*)v1 + 8;
    v110 = *(void**)v109;
    v111 = (char*)v1 + 16;
    v112 = *(int64_t*)v111;
    p_call_stack_pop();
    return v112;
  bb17:
    v114 = (char*)v1 + 0;
    v115 = *(int64_t*)v114;
    v116 = 8LL;
    v117 = v115 == v116;
    if (v117) goto bb18;
    else goto bb19;
  bb18:
    v119 = (char*)v1 + 8;
    v120 = *(void**)v119;
    v121 = (char*)v1 + 16;
    v122 = *(int64_t*)v121;
    p_call_stack_pop();
    return v122;
  bb19:
    v124 = (char*)v1 + 0;
    v125 = *(int64_t*)v124;
    v126 = 9LL;
    v127 = v125 == v126;
    if (v127) goto bb20;
    else goto bb21;
  bb20:
    v129 = (char*)v1 + 8;
    v130 = *(void**)v129;
    v131 = (char*)v1 + 16;
    v132 = *(void**)v131;
    v133 = (char*)v1 + 24;
    v134 = *(void**)v133;
    v135 = (char*)v1 + 32;
    v136 = *(int64_t*)v135;
    p_call_stack_pop();
    return v136;
  bb21:
    v138 = (char*)v1 + 0;
    v139 = *(int64_t*)v138;
    v140 = 10LL;
    v141 = v139 == v140;
    if (v141) goto bb22;
    else goto bb23;
  bb22:
    v143 = (char*)v1 + 8;
    v144 = *(void**)v143;
    v145 = (char*)v1 + 16;
    v146 = *(void**)v145;
    v147 = (char*)v1 + 24;
    v148 = *(int64_t*)v147;
    p_call_stack_pop();
    return v148;
  bb23:
    v150 = (char*)v1 + 0;
    v151 = *(int64_t*)v150;
    v152 = 11LL;
    v153 = v151 == v152;
    if (v153) goto bb24;
    else goto bb25;
  bb24:
    v155 = (char*)v1 + 8;
    v156 = *(void**)v155;
    v157 = (char*)v1 + 16;
    v158 = *(void**)v157;
    v159 = (char*)v1 + 24;
    v160 = *(void**)v159;
    v161 = (char*)v1 + 32;
    v162 = *(int64_t*)v161;
    p_call_stack_pop();
    return v162;
  bb25:
    v164 = (char*)v1 + 0;
    v165 = *(int64_t*)v164;
    v166 = 12LL;
    v167 = v165 == v166;
    if (v167) goto bb26;
    else goto bb27;
  bb26:
    v169 = (char*)v1 + 8;
    v170 = *(void**)v169;
    v171 = (char*)v1 + 16;
    v172 = *(int64_t*)v171;
    p_call_stack_pop();
    return v172;
  bb27:
    v174 = (char*)v1 + 0;
    v175 = *(int64_t*)v174;
    v176 = 13LL;
    v177 = v175 == v176;
    if (v177) goto bb28;
    else goto bb29;
  bb28:
    v179 = (char*)v1 + 8;
    v180 = *(void**)v179;
    v181 = (char*)v1 + 16;
    v182 = *(int64_t*)v181;
    p_call_stack_pop();
    return v182;
  bb29:
    v184 = (char*)v1 + 0;
    v185 = *(int64_t*)v184;
    v186 = 14LL;
    v187 = v185 == v186;
    if (v187) goto bb30;
    else goto bb31;
  bb30:
    v189 = (char*)v1 + 8;
    v190 = *(int64_t*)v189;
    p_call_stack_pop();
    return v190;
  bb31:
    v192 = (char*)v1 + 0;
    v193 = *(int64_t*)v192;
    v194 = 15LL;
    v195 = v193 == v194;
    if (v195) goto bb32;
    else goto bb33;
  bb32:
    v197 = (char*)v1 + 8;
    v198 = *(void**)v197;
    v199 = (char*)v1 + 16;
    v200 = *(int64_t*)v199;
    p_call_stack_pop();
    return v200;
  bb33:
    v202 = (char*)v1 + 0;
    v203 = *(int64_t*)v202;
    v204 = 16LL;
    v205 = v203 == v204;
    if (v205) goto bb34;
    else goto bb35;
  bb34:
    v207 = (char*)v1 + 8;
    v208 = *(void**)v207;
    v209 = (char*)v1 + 16;
    v210 = *(void**)v209;
    v211 = (char*)v1 + 24;
    v212 = *(int64_t*)v211;
    p_call_stack_pop();
    return v212;
  bb35:
    v214 = (char*)v1 + 0;
    v215 = *(int64_t*)v214;
    v216 = 17LL;
    v217 = v215 == v216;
    if (v217) goto bb36;
    else goto bb37;
  bb36:
    v219 = (char*)v1 + 8;
    v220 = *(void**)v219;
    v221 = (char*)v1 + 16;
    v222 = *(void**)v221;
    v223 = (char*)v1 + 24;
    v224 = *(int64_t*)v223;
    p_call_stack_pop();
    return v224;
  bb37:
    v226 = (char*)v1 + 0;
    v227 = *(int64_t*)v226;
    v228 = 18LL;
    v229 = v227 == v228;
    if (v229) goto bb38;
    else goto bb39;
  bb38:
    v231 = (char*)v1 + 8;
    v232 = *(void**)v231;
    v233 = (char*)v1 + 16;
    v234 = *(void**)v233;
    v235 = (char*)v1 + 24;
    v236 = *(int64_t*)v235;
    p_call_stack_pop();
    return v236;
  bb39:
    v238 = (char*)v1 + 0;
    v239 = *(int64_t*)v238;
    v240 = 19LL;
    v241 = v239 == v240;
    if (v241) goto bb40;
    else goto bb41;
  bb40:
    v243 = (char*)v1 + 8;
    v244 = *(void**)v243;
    v245 = (char*)v1 + 16;
    v246 = *(void**)v245;
    v247 = (char*)v1 + 24;
    v248 = *(void**)v247;
    v249 = (char*)v1 + 32;
    v250 = *(int64_t*)v249;
    p_call_stack_pop();
    return v250;
  bb41:
    v252 = (char*)v1 + 0;
    v253 = *(int64_t*)v252;
    v254 = 20LL;
    v255 = v253 == v254;
    if (v255) goto bb42;
    else goto bb43;
  bb42:
    v257 = (char*)v1 + 8;
    v258 = *(void**)v257;
    v259 = (char*)v1 + 16;
    v260 = *(void**)v259;
    v261 = (char*)v1 + 24;
    v262 = *(bool*)v261;
    v263 = (char*)v1 + 32;
    v264 = *(bool*)v263;
    v265 = (char*)v1 + 40;
    v266 = *(void**)v265;
    v267 = (char*)v1 + 48;
    v268 = *(void**)v267;
    v269 = (char*)v1 + 56;
    v270 = *(int64_t*)v269;
    p_call_stack_pop();
    return v270;
  bb43:
    v272 = (char*)v1 + 0;
    v273 = *(int64_t*)v272;
    v274 = 21LL;
    v275 = v273 == v274;
    if (v275) goto bb44;
    else goto bb45;
  bb44:
    v277 = (char*)v1 + 8;
    v278 = *(void**)v277;
    v279 = (char*)v1 + 16;
    v280 = *(int64_t*)v279;
    p_call_stack_pop();
    return v280;
  bb45:
    v282 = (char*)v1 + 0;
    v283 = *(int64_t*)v282;
    v284 = 22LL;
    v285 = v283 == v284;
    if (v285) goto bb46;
    else goto bb47;
  bb46:
    v287 = (char*)v1 + 8;
    v288 = *(void**)v287;
    v289 = (char*)v1 + 16;
    v290 = *(int64_t*)v289;
    p_call_stack_pop();
    return v290;
  bb47:
    v292 = (char*)v1 + 0;
    v293 = *(int64_t*)v292;
    v294 = 23LL;
    v295 = v293 == v294;
    if (v295) goto bb48;
    else goto bb49;
  bb48:
    v297 = (char*)v1 + 8;
    v298 = *(void**)v297;
    v299 = (char*)v1 + 16;
    v300 = *(int64_t*)v299;
    p_call_stack_pop();
    return v300;
  bb49:
    v302 = (char*)v1 + 0;
    v303 = *(int64_t*)v302;
    v304 = 24LL;
    v305 = v303 == v304;
    if (v305) goto bb50;
    else goto bb51;
  bb50:
    v307 = (char*)v1 + 8;
    v308 = *(void**)v307;
    v309 = (char*)v1 + 16;
    v310 = *(int64_t*)v309;
    p_call_stack_pop();
    return v310;
  bb51:
    v312 = (char*)v1 + 0;
    v313 = *(int64_t*)v312;
    v314 = 25LL;
    v315 = v313 == v314;
    if (v315) goto bb52;
    else goto bb53;
  bb52:
    v317 = (char*)v1 + 8;
    v318 = *(void**)v317;
    v319 = (char*)v1 + 16;
    v320 = *(void**)v319;
    v321 = (char*)v1 + 24;
    v322 = *(int64_t*)v321;
    p_call_stack_pop();
    return v322;
  bb53:
    v324 = (char*)v1 + 0;
    v325 = *(int64_t*)v324;
    v326 = 26LL;
    v327 = v325 == v326;
    if (v327) goto bb54;
    else goto bb55;
  bb54:
    v329 = (char*)v1 + 8;
    v330 = *(void**)v329;
    v331 = (char*)v1 + 16;
    v332 = *(int64_t*)v331;
    p_call_stack_pop();
    return v332;
  bb55:
    v334 = (char*)v1 + 0;
    v335 = *(int64_t*)v334;
    v336 = 27LL;
    v337 = v335 == v336;
    if (v337) goto bb56;
    else goto bb57;
  bb56:
    v339 = (char*)v1 + 8;
    v340 = *(void**)v339;
    v341 = (char*)v1 + 16;
    v342 = *(void**)v341;
    v343 = (char*)v1 + 24;
    v344 = *(int64_t*)v343;
    p_call_stack_pop();
    return v344;
  bb57:
    v346 = (char*)v1 + 0;
    v347 = *(int64_t*)v346;
    v348 = 28LL;
    v349 = v347 == v348;
    if (v349) goto bb58;
    else goto bb59;
  bb58:
    v351 = (char*)v1 + 8;
    v352 = *(void**)v351;
    v353 = (char*)v1 + 16;
    v354 = *(void**)v353;
    v355 = (char*)v1 + 24;
    v356 = *(int64_t*)v355;
    p_call_stack_pop();
    return v356;
  bb59:
    goto bb1;
    return 0;
}

int64_t str_parse_int(void* v0, void* v1) {
    p_call_stack_push("str_parse_int");
    int64_t v2;
    int64_t v4; /* placeholder for void */
    int64_t v3;
    v2 = 0LL;
    v3 = (int64_t)String_parse_int((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t String_slice(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("String_slice");
    int64_t v43; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    int64_t v39;
    int64_t v49; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    bool v5;
    int64_t v23;
    int64_t v65;
    int64_t v77;
    int64_t v16;
    bool v32;
    int64_t v21; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    void* v12;
    int64_t v67;
    int64_t v69; /* placeholder for void */
    int64_t v38;
    int64_t v90; /* placeholder for void */
    int64_t v11;
    int64_t v14; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    bool v40;
    void* v24;
    void* v26;
    int64_t v53; /* placeholder for void */
    int64_t v78;
    int64_t v34;
    void* v74;
    int64_t v35; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v47;
    void* v8;
    int64_t v30;
    int64_t v80;
    int64_t v46;
    void* v51;
    int64_t v56; /* placeholder for void */
    int64_t v66;
    int64_t v82;
    int64_t v19;
    int64_t v61; /* placeholder for void */
    int64_t v87;
    void* v88;
    int64_t v20; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v4;
    int64_t v64; /* placeholder for void */
    int64_t v71;
    int64_t v83;
    int64_t v92;
    int64_t v95;
    void* v55;
    void* v28;
    int64_t v93; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v42;
    int64_t v59;
    void* v91;
    int64_t v89; /* placeholder for void */
    int64_t v22;
    int64_t v62;
    int64_t v70;
    void* v52;
    int64_t v75; /* placeholder for void */
    int64_t v31;
    void* v9;
    int64_t v33; /* placeholder for void */
    bool v48;
    int64_t v84;
    int64_t v6; /* placeholder for void */
    void* v85;
    void* v15;
    int64_t v44; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v50;
    void* v58;
    int64_t v79;
    int64_t v7;
    void* v73;
    void* v68;
    int64_t v76;
    int64_t v86; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    int64_t v54;
    int64_t v45; /* placeholder for void */
    int64_t v72;
    int64_t v63; /* placeholder for void */
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

int64_t str_len(void* v0, void* v1) {
    p_call_stack_push("str_len");
    int64_t v2;
    int64_t v4; /* placeholder for void */
    int64_t v3;
    v2 = 0LL;
    v3 = (int64_t)String_len((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

void HashMap_insert(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("HashMap_insert");
    int64_t v129;
    bool v36;
    int64_t v83; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v29;
    int64_t v56; /* placeholder for void */
    void* v51;
    int64_t v8;
    int64_t v59; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v123;
    int64_t v50; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v92;
    int64_t v11; /* placeholder for void */
    int64_t v23;
    int64_t v5;
    void* v135;
    int64_t v126; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v71;
    void* v119;
    void* v10;
    int64_t v86;
    int64_t v90;
    int64_t v15;
    int64_t v139; /* placeholder for void */
    void* v22;
    int64_t v117;
    int64_t v134;
    void* v110;
    int64_t v32;
    int64_t v34;
    int64_t v38;
    int64_t v12;
    void* v67;
    int64_t v60;
    int64_t v48;
    int64_t v77; /* placeholder for void */
    void* v64;
    bool v95;
    int64_t v7; /* placeholder for void */
    int64_t v9;
    int64_t v25; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v111; /* placeholder for void */
    void* v122;
    int64_t v45; /* placeholder for void */
    int64_t v73;
    int64_t v103;
    int64_t v114; /* placeholder for void */
    int64_t v121; /* placeholder for void */
    int64_t v81;
    void* v49;
    int64_t v57;
    int64_t v120; /* placeholder for void */
    int64_t v130;
    int64_t v141; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v108; /* placeholder for void */
    int64_t v88; /* placeholder for void */
    void* v16;
    void* v19;
    void* v79;
    int64_t v136; /* placeholder for void */
    void* v41;
    int64_t v97; /* placeholder for void */
    int64_t v74;
    int64_t v68;
    void* v55;
    int64_t v53;
    void* v58;
    int64_t v109;
    void* v82;
    void* v99;
    int64_t v78;
    int64_t v46; /* placeholder for void */
    int64_t v100; /* placeholder for void */
    int64_t v124; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v72;
    int64_t v69; /* placeholder for void */
    int64_t v133; /* placeholder for void */
    int64_t v115; /* placeholder for void */
    bool v91;
    int64_t v40; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    void* v113;
    int64_t v98; /* placeholder for void */
    int64_t v116;
    int64_t v4;
    bool v28;
    int64_t v39; /* placeholder for void */
    int64_t v47;
    void* v85;
    int64_t v125; /* placeholder for void */
    int64_t v137; /* placeholder for void */
    int64_t v140; /* placeholder for void */
    int64_t v54;
    int64_t v70; /* placeholder for void */
    int64_t v94;
    int64_t v80; /* placeholder for void */
    int64_t v65;
    int64_t v106;
    int64_t v30;
    bool v96;
    void* v76;
    int64_t v87; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v131;
    int64_t v93;
    bool v35;
    void* v138;
    int64_t v142; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v31;
    int64_t v102;
    int64_t v89;
    void* v107;
    void* v6;
    int64_t v63; /* placeholder for void */
    void* v132;
    int64_t v112; /* placeholder for void */
    int64_t v128;
    int64_t v26;
    int64_t v104;
    void* v13;
    int64_t v75;
    int64_t v33;
    int64_t v105;
    void* v61;
    int64_t v27;
    int64_t v20;
    int64_t v118;
    int64_t v127;
    int64_t v37; /* placeholder for void */
    int64_t v42;
    int64_t v24; /* placeholder for void */
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

void* Vec_chunk(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_chunk");
    int64_t v3;
    int64_t v4;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)vec_chunk((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return (void*)v4;
    return 0;
}

void ImplDef_new(void* v0, void* v1, void* v2) {
    p_call_stack_push("ImplDef_new");
    void* v12;
    void* v18;
    void* v15;
    void* v4;
    void* v11;
    void* v19;
    void* v22;
    void* v3;
    int64_t v25; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v14;
    int64_t v5; /* placeholder for void */
    int64_t v6;
    int64_t v26; /* placeholder for void */
    void* v7;
    int64_t v9; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v10;
    void* v16;
    void* v20;
    void* v23;
    void* v24;
    void* v8;
    int64_t v13; /* placeholder for void */
    v3 = malloc(24);
    v4 = (char*)v3 + 0;
    *(void**)v4 = (void*)v2;
    v6 = 0LL;
    v7 = Vec_new((void*)v6);
    v8 = (char*)v3 + 8;
    *(void**)v8 = (void*)v7;
    v10 = 0LL;
    v11 = Vec_new((void*)v10);
    v12 = (char*)v3 + 16;
    *(void**)v12 = (void*)v11;
    v14 = (char*)v3 + 0;
    v15 = (char*)v0 + 0;
    v16 = *(void**)v14;
    *(void**)v15 = (void*)v16;
    v18 = (char*)v3 + 8;
    v19 = (char*)v0 + 8;
    v20 = *(void**)v18;
    *(void**)v19 = (void*)v20;
    v22 = (char*)v3 + 16;
    v23 = (char*)v0 + 16;
    v24 = *(void**)v22;
    *(void**)v23 = (void*)v24;
    p_call_stack_pop();
    return;
    return;
}

bool char_is_digit(void* v0, int64_t v1) {
    p_call_stack_push("char_is_digit");
    int64_t v7; /* placeholder for void */
    int64_t v4;
    bool v5;
    bool v6;
    int64_t v2;
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

void* byte_to_str(void* v0, int64_t v1) {
    p_call_stack_push("byte_to_str");
    void* v2;
    int64_t v3; /* placeholder for void */
    v2 = p_char_to_str(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void* int_to_hex(void* v0, int64_t v1) {
    p_call_stack_push("int_to_hex");
    int64_t v49;
    int64_t v18;
    void* v45;
    void* v40;
    void* v52;
    int64_t v6; /* placeholder for void */
    int64_t v29;
    int64_t v31;
    int64_t v57;
    int64_t v60;
    void* v64;
    int64_t v26;
    int64_t v25;
    int64_t v10; /* placeholder for void */
    bool v20;
    int64_t v11;
    int64_t v43;
    int64_t v14; /* placeholder for void */
    void* v9;
    int64_t v30; /* placeholder for void */
    int64_t v33;
    void* v38;
    void* v53;
    int64_t v32;
    int64_t v24;
    void* v37;
    int64_t v54;
    int64_t v67; /* placeholder for void */
    int64_t v55;
    void* v23;
    int64_t v34; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v66;
    void* v15;
    int64_t v42;
    int64_t v47; /* placeholder for void */
    int64_t v58; /* placeholder for void */
    bool v3;
    int64_t v65;
    int64_t v61;
    void* v13;
    void* v5;
    int64_t v56;
    int64_t v7; /* placeholder for void */
    void* v8;
    int64_t v16; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    int64_t v27;
    void* v22;
    int64_t v19;
    bool v50;
    int64_t v35; /* placeholder for void */
    void* v12;
    int64_t v39; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v28;
    int64_t v41;
    int64_t v48;
    int64_t v62; /* placeholder for void */
    int64_t v44;
    int64_t v51; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v36;
    int64_t v2;
    int64_t v59;
    int64_t v46; /* placeholder for void */
    v2 = 0LL;
    v3 = v1 == v2;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    v5 = _str15;
    p_call_stack_pop();
    return v5;
  bb2:
    goto bb3;
  bb3:
    v8 = _str16;
    v9 = malloc(8);
    *(void**)v9 = (void*)v8;
    v11 = 0LL;
    v12 = Vec_new((void*)v11);
    v13 = malloc(8);
    *(void**)v13 = (void*)v12;
    v15 = malloc(8);
    *(int64_t*)v15 = (int64_t)v1;
    goto bb4;
  bb4:
    v18 = *(int64_t*)v15;
    v19 = 0LL;
    v20 = v18 > v19;
    if (v20) goto bb5;
    else goto bb6;
  bb5:
    v22 = *(void**)v13;
    v23 = *(void**)v9;
    v24 = *(int64_t*)v15;
    v25 = 15LL;
    v26 = v24 & v25;
    v27 = 0LL;
    v28 = (int64_t)str_byte_at((void*)v27, v23, v26);
    v29 = 0LL;
    Vec_push((void*)v29, v22, v28);
    v31 = *(int64_t*)v15;
    v32 = 4LL;
    v33 = v31 >> v32;
    *(int64_t*)v15 = (int64_t)v33;
    goto bb4;
  bb6:
    v36 = 0LL;
    v37 = Vec_new((void*)v36);
    v38 = malloc(8);
    *(void**)v38 = (void*)v37;
    v40 = *(void**)v13;
    v41 = 0LL;
    v42 = (int64_t)Vec_len((void*)v41, v40);
    v43 = 1LL;
    v44 = v42 - v43;
    v45 = malloc(8);
    *(int64_t*)v45 = (int64_t)v44;
    goto bb7;
  bb7:
    v48 = *(int64_t*)v45;
    v49 = 0LL;
    v50 = v48 >= v49;
    if (v50) goto bb8;
    else goto bb9;
  bb8:
    v52 = *(void**)v38;
    v53 = *(void**)v13;
    v54 = *(int64_t*)v45;
    v55 = 0LL;
    v56 = (int64_t)Vec_get((void*)v55, v53, v54);
    v57 = 0LL;
    Vec_push((void*)v57, v52, v56);
    v59 = *(int64_t*)v45;
    v60 = 1LL;
    v61 = v59 - v60;
    *(int64_t*)v45 = (int64_t)v61;
    goto bb7;
  bb9:
    v64 = *(void**)v38;
    v65 = 0LL;
    v66 = str_from_bytes((void*)v65, v64);
    p_call_stack_pop();
    return v66;
    return 0;
}

bool Vec_all(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_all");
    void* v8;
    int64_t v10; /* placeholder for void */
    int64_t v28;
    int64_t v33; /* placeholder for void */
    int64_t v15;
    int64_t v17;
    int64_t v26; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v12;
    int64_t v29;
    void* v4;
    int64_t v3;
    int64_t v16;
    int64_t v6;
    bool v21;
    void* v18;
    bool v13;
    int64_t v7;
    int64_t v25; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    bool v32;
    int64_t v31; /* placeholder for void */
    int64_t v11;
    int64_t v20;
    int64_t v22;
    int64_t v23; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    bool v24;
    int64_t v27;
    int64_t v19; /* placeholder for void */
    v3 = 0LL;
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = 0LL;
    v7 = (int64_t)Vec_len((void*)v6, v1);
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    goto bb1;
  bb1:
    v11 = *(int64_t*)v4;
    v12 = *(int64_t*)v8;
    v13 = v11 < v12;
    if (v13) goto bb2;
    else goto bb3;
  bb2:
    v15 = *(int64_t*)v4;
    v16 = 0LL;
    v17 = (int64_t)Vec_get((void*)v16, v1, v15);
    v18 = malloc(8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = *(int64_t*)v18;
    {
        if (!v2) abort();
        char* _fn_name = *(char**)v2;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "str_is_blank") == 0) {
            v21 = str_is_blank((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Option_is_some") == 0) {
            v21 = Option_is_some((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Option_is_none") == 0) {
            v21 = Option_is_none((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Vec_is_empty") == 0) {
            v21 = Vec_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_numeric") == 0) {
            v21 = str_is_numeric((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "String_is_empty") == 0) {
            v21 = String_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_alpha") == 0) {
            v21 = char_is_alpha((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "str_is_alpha") == 0) {
            v21 = str_is_alpha((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "HashMap_is_empty") == 0) {
            v21 = HashMap_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Result_is_err") == 0) {
            v21 = Result_is_err((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_alphanumeric") == 0) {
            v21 = str_is_alphanumeric((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_empty") == 0) {
            v21 = str_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "StringBuilder_is_empty") == 0) {
            v21 = StringBuilder_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Result_is_ok") == 0) {
            v21 = Result_is_ok((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_whitespace") == 0) {
            v21 = char_is_whitespace((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_upper") == 0) {
            v21 = char_is_upper((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "ByteBuffer_is_empty") == 0) {
            v21 = ByteBuffer_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_digit") == 0) {
            v21 = char_is_digit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_lower") == 0) {
            v21 = char_is_lower((void*)v2, (int64_t)v20);
        } else { abort(); }
    }
    v22 = !v21;
    if (v22) goto bb4;
    else goto bb5;
  bb3:
    v32 = true;
    p_call_stack_pop();
    return v32;
  bb4:
    v24 = false;
    p_call_stack_pop();
    return v24;
  bb5:
    goto bb6;
  bb6:
    v27 = *(int64_t*)v4;
    v28 = 1LL;
    v29 = v27 + v28;
    *(int64_t*)v4 = (int64_t)v29;
    goto bb1;
    return 0;
}

void program_new(void* v0, void* v1) {
    p_call_stack_push("program_new");
    void* v115;
    void* v87;
    void* v119;
    void* v107;
    int64_t v20;
    void* v159;
    void* v90;
    int64_t v51; /* placeholder for void */
    void* v134;
    void* v132;
    int64_t v89; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v149; /* placeholder for void */
    void* v118;
    void* v138;
    void* v94;
    void* v114;
    int64_t v133; /* placeholder for void */
    int64_t v44;
    void* v126;
    int64_t v48;
    void* v12;
    int64_t v27; /* placeholder for void */
    int64_t v32;
    void* v29;
    int64_t v75; /* placeholder for void */
    void* v38;
    int64_t v92;
    int64_t v63; /* placeholder for void */
    void* v9;
    void* v13;
    void* v3;
    int64_t v88;
    void* v8;
    void* v112;
    int64_t v31; /* placeholder for void */
    void* v54;
    void* v136;
    void* v147;
    int64_t v157; /* placeholder for void */
    int64_t v52;
    int64_t v162; /* placeholder for void */
    void* v144;
    int64_t v97; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    void* v41;
    void* v108;
    void* v77;
    void* v26;
    void* v103;
    void* v122;
    void* v148;
    void* v123;
    int64_t v72;
    int64_t v35; /* placeholder for void */
    void* v154;
    int64_t v101; /* placeholder for void */
    void* v111;
    void* v124;
    void* v143;
    int64_t v24;
    void* v37;
    void* v62;
    void* v61;
    int64_t v145; /* placeholder for void */
    void* v155;
    void* v57;
    void* v116;
    void* v160;
    int64_t v28;
    void* v86;
    void* v78;
    void* v45;
    void* v2;
    void* v49;
    void* v5;
    int64_t v43; /* placeholder for void */
    int64_t v4;
    void* v96;
    void* v104;
    int64_t v14;
    void* v17;
    int64_t v15; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    void* v85;
    void* v7;
    void* v91;
    void* v139;
    void* v74;
    void* v58;
    int64_t v76;
    int64_t v56;
    int64_t v19; /* placeholder for void */
    void* v30;
    void* v130;
    int64_t v113; /* placeholder for void */
    void* v110;
    void* v142;
    void* v69;
    void* v65;
    void* v156;
    int64_t v161; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    int64_t v40;
    void* v22;
    void* v98;
    void* v135;
    int64_t v6; /* placeholder for void */
    void* v106;
    void* v34;
    void* v84;
    void* v33;
    int64_t v55; /* placeholder for void */
    void* v18;
    int64_t v60;
    void* v50;
    void* v82;
    void* v100;
    int64_t v125; /* placeholder for void */
    void* v140;
    void* v151;
    int64_t v117; /* placeholder for void */
    int64_t v105; /* placeholder for void */
    void* v128;
    int64_t v109; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    void* v25;
    int64_t v68;
    int64_t v129; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    void* v66;
    void* v46;
    void* v146;
    int64_t v141; /* placeholder for void */
    void* v70;
    void* v81;
    int64_t v93; /* placeholder for void */
    void* v158;
    void* v152;
    void* v150;
    void* v53;
    int64_t v36;
    void* v127;
    int64_t v16;
    void* v131;
    void* v42;
    int64_t v83; /* placeholder for void */
    void* v120;
    int64_t v64;
    int64_t v121; /* placeholder for void */
    int64_t v153; /* placeholder for void */
    void* v102;
    void* v95;
    void* v73;
    int64_t v137; /* placeholder for void */
    void* v21;
    int64_t v80;
    void* v99;
    int64_t v10;
    int64_t v67; /* placeholder for void */
    v2 = malloc(152);
    v3 = malloc(16);
    v4 = 1LL;
    v5 = (char*)v3 + 0;
    *(int64_t*)v5 = (int64_t)v4;
    v7 = (char*)v2 + 0;
    v8 = (char*)v3 + 0;
    v9 = (char*)v7 + 0;
    v10 = *(int64_t*)v8;
    *(int64_t*)v9 = (int64_t)v10;
    v12 = (char*)v3 + 8;
    v13 = (char*)v7 + 8;
    v14 = *(int64_t*)v12;
    *(int64_t*)v13 = (int64_t)v14;
    v16 = 0LL;
    v17 = Vec_new((void*)v16);
    v18 = (char*)v2 + 16;
    *(void**)v18 = (void*)v17;
    v20 = 0LL;
    v21 = Vec_new((void*)v20);
    v22 = (char*)v2 + 24;
    *(void**)v22 = (void*)v21;
    v24 = 0LL;
    v25 = Vec_new((void*)v24);
    v26 = (char*)v2 + 32;
    *(void**)v26 = (void*)v25;
    v28 = 0LL;
    v29 = Vec_new((void*)v28);
    v30 = (char*)v2 + 40;
    *(void**)v30 = (void*)v29;
    v32 = 0LL;
    v33 = Vec_new((void*)v32);
    v34 = (char*)v2 + 48;
    *(void**)v34 = (void*)v33;
    v36 = 0LL;
    v37 = Vec_new((void*)v36);
    v38 = (char*)v2 + 56;
    *(void**)v38 = (void*)v37;
    v40 = 0LL;
    v41 = Vec_new((void*)v40);
    v42 = (char*)v2 + 64;
    *(void**)v42 = (void*)v41;
    v44 = 0LL;
    v45 = Vec_new((void*)v44);
    v46 = (char*)v2 + 72;
    *(void**)v46 = (void*)v45;
    v48 = 0LL;
    v49 = Vec_new((void*)v48);
    v50 = (char*)v2 + 80;
    *(void**)v50 = (void*)v49;
    v52 = 0LL;
    v53 = Vec_new((void*)v52);
    v54 = (char*)v2 + 88;
    *(void**)v54 = (void*)v53;
    v56 = 0LL;
    v57 = Vec_new((void*)v56);
    v58 = (char*)v2 + 96;
    *(void**)v58 = (void*)v57;
    v60 = 0LL;
    v61 = Vec_new((void*)v60);
    v62 = (char*)v2 + 104;
    *(void**)v62 = (void*)v61;
    v64 = 0LL;
    v65 = Vec_new((void*)v64);
    v66 = (char*)v2 + 112;
    *(void**)v66 = (void*)v65;
    v68 = 0LL;
    v69 = Vec_new((void*)v68);
    v70 = (char*)v2 + 120;
    *(void**)v70 = (void*)v69;
    v72 = 0LL;
    v73 = Vec_new((void*)v72);
    v74 = (char*)v2 + 128;
    *(void**)v74 = (void*)v73;
    v76 = 0LL;
    v77 = Vec_new((void*)v76);
    v78 = (char*)v2 + 136;
    *(void**)v78 = (void*)v77;
    v80 = 0LL;
    v81 = Vec_new((void*)v80);
    v82 = (char*)v2 + 144;
    *(void**)v82 = (void*)v81;
    v84 = (char*)v2 + 0;
    v85 = (char*)v0 + 0;
    v86 = (char*)v84 + 0;
    v87 = (char*)v85 + 0;
    v88 = *(int64_t*)v86;
    *(int64_t*)v87 = (int64_t)v88;
    v90 = (char*)v84 + 8;
    v91 = (char*)v85 + 8;
    v92 = *(int64_t*)v90;
    *(int64_t*)v91 = (int64_t)v92;
    v94 = (char*)v2 + 16;
    v95 = (char*)v0 + 16;
    v96 = *(void**)v94;
    *(void**)v95 = (void*)v96;
    v98 = (char*)v2 + 24;
    v99 = (char*)v0 + 24;
    v100 = *(void**)v98;
    *(void**)v99 = (void*)v100;
    v102 = (char*)v2 + 32;
    v103 = (char*)v0 + 32;
    v104 = *(void**)v102;
    *(void**)v103 = (void*)v104;
    v106 = (char*)v2 + 40;
    v107 = (char*)v0 + 40;
    v108 = *(void**)v106;
    *(void**)v107 = (void*)v108;
    v110 = (char*)v2 + 48;
    v111 = (char*)v0 + 48;
    v112 = *(void**)v110;
    *(void**)v111 = (void*)v112;
    v114 = (char*)v2 + 56;
    v115 = (char*)v0 + 56;
    v116 = *(void**)v114;
    *(void**)v115 = (void*)v116;
    v118 = (char*)v2 + 64;
    v119 = (char*)v0 + 64;
    v120 = *(void**)v118;
    *(void**)v119 = (void*)v120;
    v122 = (char*)v2 + 72;
    v123 = (char*)v0 + 72;
    v124 = *(void**)v122;
    *(void**)v123 = (void*)v124;
    v126 = (char*)v2 + 80;
    v127 = (char*)v0 + 80;
    v128 = *(void**)v126;
    *(void**)v127 = (void*)v128;
    v130 = (char*)v2 + 88;
    v131 = (char*)v0 + 88;
    v132 = *(void**)v130;
    *(void**)v131 = (void*)v132;
    v134 = (char*)v2 + 96;
    v135 = (char*)v0 + 96;
    v136 = *(void**)v134;
    *(void**)v135 = (void*)v136;
    v138 = (char*)v2 + 104;
    v139 = (char*)v0 + 104;
    v140 = *(void**)v138;
    *(void**)v139 = (void*)v140;
    v142 = (char*)v2 + 112;
    v143 = (char*)v0 + 112;
    v144 = *(void**)v142;
    *(void**)v143 = (void*)v144;
    v146 = (char*)v2 + 120;
    v147 = (char*)v0 + 120;
    v148 = *(void**)v146;
    *(void**)v147 = (void*)v148;
    v150 = (char*)v2 + 128;
    v151 = (char*)v0 + 128;
    v152 = *(void**)v150;
    *(void**)v151 = (void*)v152;
    v154 = (char*)v2 + 136;
    v155 = (char*)v0 + 136;
    v156 = *(void**)v154;
    *(void**)v155 = (void*)v156;
    v158 = (char*)v2 + 144;
    v159 = (char*)v0 + 144;
    v160 = *(void**)v158;
    *(void**)v159 = (void*)v160;
    p_call_stack_pop();
    return;
    return;
}

void* shape_dim_to_str(void* v0, void* v1) {
    p_call_stack_push("shape_dim_to_str");
    void* v2;
    int64_t v22; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v3;
    int64_t v9;
    void* v7;
    int64_t v6; /* placeholder for void */
    bool v15;
    int64_t v4;
    int64_t v16; /* placeholder for void */
    void* v18;
    int64_t v13;
    void* v10;
    int64_t v8;
    void* v12;
    void* v17;
    int64_t v20; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v14;
    void* v21;
    bool v5;
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
    v9 = 0LL;
    v10 = int_to_str((void*)v9, v8);
    p_call_stack_pop();
    return v10;
  bb3:
    v12 = (char*)v1 + 0;
    v13 = *(int64_t*)v12;
    v14 = 1LL;
    v15 = v13 == v14;
    if (v15) goto bb4;
    else goto bb5;
  bb4:
    v17 = (char*)v1 + 8;
    v18 = *(void**)v17;
    p_call_stack_pop();
    return v18;
  bb5:
    goto bb1;
    return 0;
}

int64_t ByteBuffer_len(void* v0, void* v1) {
    p_call_stack_push("ByteBuffer_len");
    int64_t v3;
    int64_t v4; /* placeholder for void */
    void* v2;
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t HashMap_find_slot(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_find_slot");
    int64_t v63;
    int64_t v6; /* placeholder for void */
    int64_t v32;
    void* v39;
    int64_t v90;
    int64_t v94;
    int64_t v106; /* placeholder for void */
    void* v34;
    int64_t v56;
    int64_t v116; /* placeholder for void */
    void* v46;
    int64_t v62; /* placeholder for void */
    bool v113;
    int64_t v110; /* placeholder for void */
    void* v58;
    int64_t v44; /* placeholder for void */
    int64_t v100;
    void* v7;
    int64_t v48; /* placeholder for void */
    int64_t v103;
    int64_t v111;
    int64_t v130; /* placeholder for void */
    int64_t v123; /* placeholder for void */
    int64_t v76;
    int64_t v97; /* placeholder for void */
    int64_t v105; /* placeholder for void */
    int64_t v65; /* placeholder for void */
    void* v10;
    int64_t v66; /* placeholder for void */
    int64_t v57;
    int64_t v122; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    int64_t v41;
    int64_t v50;
    int64_t v75;
    int64_t v121;
    int64_t v78; /* placeholder for void */
    int64_t v102; /* placeholder for void */
    int64_t v42;
    int64_t v53;
    bool v88;
    int64_t v14;
    int64_t v107;
    int64_t v33;
    int64_t v124;
    int64_t v18;
    int64_t v12; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    void* v61;
    int64_t v120;
    int64_t v36;
    void* v43;
    int64_t v74; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v109;
    int64_t v24;
    int64_t v96; /* placeholder for void */
    bool v73;
    int64_t v69; /* placeholder for void */
    int64_t v82;
    int64_t v60;
    int64_t v127; /* placeholder for void */
    int64_t v93; /* placeholder for void */
    bool v51;
    int64_t v95; /* placeholder for void */
    void* v25;
    int64_t v108;
    int64_t v30; /* placeholder for void */
    int64_t v129; /* placeholder for void */
    void* v28;
    int64_t v35; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    int64_t v117; /* placeholder for void */
    int64_t v4;
    int64_t v91;
    void* v64;
    void* v13;
    int64_t v31; /* placeholder for void */
    int64_t v17;
    int64_t v99;
    int64_t v87;
    int64_t v132; /* placeholder for void */
    int64_t v3;
    int64_t v79;
    int64_t v118; /* placeholder for void */
    int64_t v37;
    int64_t v9;
    int64_t v38;
    int64_t v112;
    int64_t v26; /* placeholder for void */
    void* v5;
    int64_t v83; /* placeholder for void */
    bool v101;
    int64_t v80; /* placeholder for void */
    int64_t v21;
    int64_t v71;
    int64_t v85;
    bool v92;
    bool v126;
    int64_t v45;
    int64_t v49;
    int64_t v89; /* placeholder for void */
    int64_t v114; /* placeholder for void */
    int64_t v55;
    bool v77;
    int64_t v68;
    int64_t v52; /* placeholder for void */
    int64_t v86;
    int64_t v125;
    int64_t v8; /* placeholder for void */
    int64_t v115;
    bool v19;
    int64_t v54;
    int64_t v16; /* placeholder for void */
    void* v67;
    int64_t v70; /* placeholder for void */
    int64_t v72;
    int64_t v119;
    int64_t v47; /* placeholder for void */
    int64_t v131;
    int64_t v20; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v29;
    int64_t v128;
    int64_t v104; /* placeholder for void */
    int64_t v59; /* placeholder for void */
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

void StrPair_new(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("StrPair_new");
    void* v4;
    int64_t v17; /* placeholder for void */
    void* v5;
    int64_t v12; /* placeholder for void */
    void* v10;
    void* v15;
    void* v9;
    void* v14;
    int64_t v8; /* placeholder for void */
    void* v11;
    void* v13;
    void* v7;
    int64_t v16; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    v4 = malloc(16);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = (char*)v4 + 8;
    *(void**)v7 = (void*)v3;
    v9 = (char*)v4 + 0;
    v10 = (char*)v0 + 0;
    v11 = *(void**)v9;
    *(void**)v10 = (void*)v11;
    v13 = (char*)v4 + 8;
    v14 = (char*)v0 + 8;
    v15 = *(void**)v13;
    *(void**)v14 = (void*)v15;
    p_call_stack_pop();
    return;
    return;
}

void* str_split(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_split");
    int64_t v120;
    void* v122;
    void* v23;
    int64_t v28; /* placeholder for void */
    int64_t v65; /* placeholder for void */
    int64_t v69;
    int64_t v11; /* placeholder for void */
    int64_t v90;
    int64_t v22;
    int64_t v123; /* placeholder for void */
    bool v39;
    void* v95;
    int64_t v108; /* placeholder for void */
    int64_t v7;
    int64_t v113; /* placeholder for void */
    int64_t v36;
    void* v15;
    int64_t v118;
    void* v70;
    int64_t v102;
    int64_t v31; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v103;
    bool v84;
    int64_t v51;
    int64_t v55; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    int64_t v107; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v104;
    int64_t v63;
    int64_t v60;
    int64_t v35;
    int64_t v111;
    int64_t v25;
    int64_t v42;
    int64_t v101; /* placeholder for void */
    int64_t v53;
    int64_t v106;
    int64_t v80;
    int64_t v96;
    int64_t v121; /* placeholder for void */
    int64_t v50;
    int64_t v26;
    int64_t v79; /* placeholder for void */
    bool v82;
    int64_t v98;
    void* v27;
    int64_t v114; /* placeholder for void */
    void* v5;
    int64_t v86; /* placeholder for void */
    void* v76;
    int64_t v8;
    int64_t v9;
    int64_t v44; /* placeholder for void */
    int64_t v112; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    void* v67;
    int64_t v13;
    int64_t v83; /* placeholder for void */
    int64_t v88;
    int64_t v105; /* placeholder for void */
    int64_t v29;
    int64_t v41; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v52;
    int64_t v56;
    int64_t v85; /* placeholder for void */
    void* v19;
    int64_t v14; /* placeholder for void */
    void* v115;
    bool v10;
    int64_t v38; /* placeholder for void */
    bool v37;
    int64_t v72; /* placeholder for void */
    int64_t v97;
    int64_t v100;
    int64_t v117;
    void* v58;
    uint8_t v74;
    int64_t v75; /* placeholder for void */
    void* v4;
    int64_t v16; /* placeholder for void */
    void* v73;
    int64_t v34;
    uint8_t v77;
    int64_t v92; /* placeholder for void */
    void* v30;
    int64_t v45; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v18;
    bool v48;
    int64_t v21;
    int64_t v33;
    int64_t v119;
    int64_t v46;
    bool v93;
    int64_t v94; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    int64_t v81;
    void* v64;
    void* v61;
    int64_t v47;
    int64_t v99;
    int64_t v110;
    void* v54;
    int64_t v66;
    int64_t v57;
    void* v43;
    int64_t v32; /* placeholder for void */
    void* v12;
    int64_t v87; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v116;
    void* v40;
    int64_t v3;
    int64_t v89;
    int64_t v62; /* placeholder for void */
    int64_t v109;
    int64_t v20; /* placeholder for void */
    v3 = 0LL;
    v4 = Vec_new((void*)v3);
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
    Vec_push((void*)v13, v12, (int64_t)v1);
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
    Vec_push((void*)v120, v115, v119);
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
    Vec_push((void*)v100, v95, v99);
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

int64_t op_tag(void* v0, void* v1) {
    p_call_stack_push("op_tag");
    void* v9;
    int64_t v8; /* placeholder for void */
    int64_t v17;
    bool v12;
    int64_t v20; /* placeholder for void */
    bool v5;
    int64_t v21;
    int64_t v4;
    int64_t v22; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v14;
    int64_t v7;
    bool v19;
    int64_t v24;
    int64_t v29; /* placeholder for void */
    int64_t v25;
    void* v16;
    int64_t v13; /* placeholder for void */
    int64_t v3;
    int64_t v10;
    int64_t v18;
    int64_t v23; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v11;
    int64_t v26; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    void* v2;
    int64_t v28;
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v28 = 0LL;
    p_call_stack_pop();
    return v28;
  bb2:
    v7 = 0LL;
    p_call_stack_pop();
    return v7;
  bb3:
    v9 = (char*)v1 + 0;
    v10 = *(int64_t*)v9;
    v11 = 1LL;
    v12 = v10 == v11;
    if (v12) goto bb4;
    else goto bb5;
  bb4:
    v14 = 1LL;
    p_call_stack_pop();
    return v14;
  bb5:
    v16 = (char*)v1 + 0;
    v17 = *(int64_t*)v16;
    v18 = 2LL;
    v19 = v17 == v18;
    if (v19) goto bb6;
    else goto bb7;
  bb6:
    v21 = 2LL;
    p_call_stack_pop();
    return v21;
  bb7:
    goto bb8;
  bb8:
    v24 = 1LL;
    v25 = -v24;
    p_call_stack_pop();
    return v25;
  bb9:
    goto bb1;
    return 0;
}

int64_t Result_unwrap_or(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Result_unwrap_or");
    void* v11;
    int64_t v12;
    void* v3;
    int64_t v17;
    int64_t v18; /* placeholder for void */
    int64_t v9;
    void* v8;
    int64_t v21; /* placeholder for void */
    int64_t v5;
    bool v6;
    int64_t v15; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    bool v14;
    int64_t v4;
    int64_t v7; /* placeholder for void */
    void* v16;
    int64_t v19; /* placeholder for void */
    int64_t v13;
    int64_t v20;
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

bool vec_contains_val(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("vec_contains_val");
    int64_t v29; /* placeholder for void */
    bool v28;
    int64_t v27; /* placeholder for void */
    void* v5;
    int64_t v15;
    int64_t v6; /* placeholder for void */
    int64_t v24;
    void* v8;
    int64_t v17;
    int64_t v22; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    bool v13;
    int64_t v21; /* placeholder for void */
    int64_t v11;
    int64_t v23;
    int64_t v16;
    bool v20;
    int64_t v25;
    int64_t v14; /* placeholder for void */
    int64_t v3;
    int64_t v4;
    bool v18;
    int64_t v7;
    int64_t v26; /* placeholder for void */
    int64_t v12;
    v3 = 0LL;
    v4 = (int64_t)Vec_len((void*)v3, (void*)v1);
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = 0LL;
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    goto bb1;
  bb1:
    v11 = *(int64_t*)v8;
    v12 = *(int64_t*)v5;
    v13 = v11 < v12;
    if (v13) goto bb2;
    else goto bb3;
  bb2:
    v15 = *(int64_t*)v8;
    v16 = 0LL;
    v17 = (int64_t)Vec_get((void*)v16, (void*)v1, v15);
    v18 = v17 == v2;
    if (v18) goto bb4;
    else goto bb5;
  bb3:
    v28 = false;
    p_call_stack_pop();
    return v28;
  bb4:
    v20 = true;
    p_call_stack_pop();
    return v20;
  bb5:
    goto bb6;
  bb6:
    v23 = *(int64_t*)v8;
    v24 = 1LL;
    v25 = v23 + v24;
    *(int64_t*)v8 = (int64_t)v25;
    goto bb1;
    return 0;
}

void Binding_new(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Binding_new");
    int64_t v17; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v13;
    int64_t v16; /* placeholder for void */
    void* v11;
    void* v5;
    void* v14;
    int64_t v12; /* placeholder for void */
    void* v7;
    void* v9;
    void* v15;
    void* v10;
    void* v4;
    v4 = malloc(16);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = (char*)v4 + 8;
    *(void**)v7 = (void*)v3;
    v9 = (char*)v4 + 0;
    v10 = (char*)v0 + 0;
    v11 = *(void**)v9;
    *(void**)v10 = (void*)v11;
    v13 = (char*)v4 + 8;
    v14 = (char*)v0 + 8;
    v15 = *(void**)v13;
    *(void**)v14 = (void*)v15;
    p_call_stack_pop();
    return;
    return;
}

void* str_from_bytes(void* v0, void* v1) {
    p_call_stack_push("str_from_bytes");
    int64_t v14;
    void* v3;
    int64_t v7; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    void* v6;
    int64_t v16; /* placeholder for void */
    int64_t v20;
    int64_t v26;
    int64_t v4; /* placeholder for void */
    bool v15;
    int64_t v24;
    int64_t v27; /* placeholder for void */
    void* v10;
    int64_t v8;
    void* v29;
    int64_t v19;
    int64_t v30; /* placeholder for void */
    int64_t v13;
    int64_t v11; /* placeholder for void */
    void* v2;
    int64_t v28; /* placeholder for void */
    void* v17;
    int64_t v5;
    void* v22;
    void* v21;
    int64_t v25;
    int64_t v18;
    int64_t v12; /* placeholder for void */
    int64_t v9;
    v2 = _str5;
    v3 = malloc(8);
    *(void**)v3 = (void*)v2;
    v5 = 0LL;
    v6 = malloc(8);
    *(int64_t*)v6 = (int64_t)v5;
    v8 = 0LL;
    v9 = (int64_t)Vec_len((void*)v8, v1);
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
    v20 = (int64_t)Vec_get((void*)v19, v1, v18);
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

int64_t String_parse_int(void* v0, int64_t v1) {
    p_call_stack_push("String_parse_int");
    int64_t v77; /* placeholder for void */
    int64_t v74;
    int64_t v26;
    int64_t v18; /* placeholder for void */
    int64_t v61; /* placeholder for void */
    int64_t v25;
    int64_t v58;
    int64_t v85;
    int64_t v2;
    int64_t v82; /* placeholder for void */
    int64_t v41;
    uint8_t v48;
    int64_t v40; /* placeholder for void */
    bool v3;
    bool v39;
    int64_t v11;
    int64_t v110; /* placeholder for void */
    void* v17;
    bool v35;
    int64_t v111; /* placeholder for void */
    bool v91;
    int64_t v98;
    int64_t v115; /* placeholder for void */
    int64_t v56;
    uint8_t v65;
    int64_t v71; /* placeholder for void */
    bool v70;
    int64_t v49; /* placeholder for void */
    int64_t v112;
    int64_t v62;
    bool v34;
    int64_t v103;
    int64_t v15; /* placeholder for void */
    bool v90;
    int64_t v76;
    int64_t v43;
    int64_t v81; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    uint8_t v108;
    int64_t v100; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    int64_t v101;
    int64_t v7; /* placeholder for void */
    int64_t v72;
    int64_t v109; /* placeholder for void */
    int64_t v73;
    int64_t v66; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v93;
    bool v87;
    int64_t v59;
    int64_t v97;
    int64_t v36;
    int64_t v114;
    void* v107;
    int64_t v83; /* placeholder for void */
    int64_t v45;
    int64_t v24; /* placeholder for void */
    int64_t v105;
    int64_t v50; /* placeholder for void */
    int64_t v52;
    int64_t v92; /* placeholder for void */
    int64_t v95;
    int64_t v106; /* placeholder for void */
    int64_t v69;
    void* v9;
    uint8_t v21;
    uint8_t v79;
    int64_t v94;
    void* v78;
    bool v53;
    int64_t v84; /* placeholder for void */
    void* v22;
    int64_t v96;
    int64_t v19; /* placeholder for void */
    bool v31;
    int64_t v37;
    bool v38;
    int64_t v42;
    void* v14;
    int64_t v8;
    int64_t v75; /* placeholder for void */
    void* v20;
    int64_t v113;
    int64_t v6; /* placeholder for void */
    int64_t v32;
    int64_t v33;
    int64_t v86;
    int64_t v46; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    void* v12;
    int64_t v88;
    int64_t v102;
    int64_t v68;
    int64_t v54; /* placeholder for void */
    int64_t v16;
    void* v47;
    int64_t v10; /* placeholder for void */
    int64_t v60;
    bool v30;
    bool v27;
    int64_t v99;
    int64_t v57; /* placeholder for void */
    int64_t v89;
    int64_t v4; /* placeholder for void */
    int64_t v51;
    int64_t v29;
    int64_t v55;
    int64_t v28;
    void* v64;
    int64_t v5;
    int64_t v13; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v80; /* placeholder for void */
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

int64_t str_find_nth(void* v0, void* v1, void* v2, int64_t v3) {
    p_call_stack_push("str_find_nth");
    int64_t v54; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v31;
    int64_t v69; /* placeholder for void */
    int64_t v65; /* placeholder for void */
    int64_t v12;
    int64_t v38;
    int64_t v63;
    int64_t v57;
    int64_t v58;
    bool v5;
    int64_t v7;
    int64_t v15; /* placeholder for void */
    bool v9;
    int64_t v48;
    int64_t v8;
    int64_t v49; /* placeholder for void */
    bool v51;
    int64_t v53;
    int64_t v37;
    int64_t v14; /* placeholder for void */
    int64_t v34;
    int64_t v29;
    bool v32;
    int64_t v52; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    int64_t v27;
    bool v10;
    int64_t v55; /* placeholder for void */
    int64_t v47;
    int64_t v18; /* placeholder for void */
    void* v24;
    int64_t v46;
    void* v39;
    void* v20;
    int64_t v62;
    int64_t v13;
    int64_t v50;
    int64_t v6;
    int64_t v16;
    int64_t v4;
    int64_t v41; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v28;
    bool v44;
    int64_t v68;
    int64_t v43;
    int64_t v36;
    int64_t v61;
    int64_t v60; /* placeholder for void */
    int64_t v22;
    int64_t v35;
    int64_t v30;
    int64_t v59; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    void* v17;
    int64_t v23;
    int64_t v21; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    void* v40;
    int64_t v45; /* placeholder for void */
    int64_t v56;
    int64_t v19;
    int64_t v67;
    void* v42;
    int64_t v25; /* placeholder for void */
    v4 = 1LL;
    v5 = v3 < v4;
    v6 = 0LL;
    v7 = (int64_t)str_len((void*)v6, v2);
    v8 = 0LL;
    v9 = v7 == v8;
    v10 = v5 || v9;
    if (v10) goto bb1;
    else goto bb2;
  bb1:
    v12 = 1LL;
    v13 = -v12;
    p_call_stack_pop();
    return v13;
  bb2:
    goto bb3;
  bb3:
    v16 = 0LL;
    v17 = malloc(8);
    *(int64_t*)v17 = (int64_t)v16;
    v19 = 0LL;
    v20 = malloc(8);
    *(int64_t*)v20 = (int64_t)v19;
    v22 = 0LL;
    v23 = (int64_t)str_len((void*)v22, v2);
    v24 = malloc(8);
    *(int64_t*)v24 = (int64_t)v23;
    goto bb4;
  bb4:
    v27 = *(int64_t*)v20;
    v28 = 0LL;
    v29 = (int64_t)str_len((void*)v28, v1);
    v30 = *(int64_t*)v24;
    v31 = v29 - v30;
    v32 = v27 <= v31;
    if (v32) goto bb5;
    else goto bb6;
  bb5:
    v34 = *(int64_t*)v20;
    v35 = *(int64_t*)v20;
    v36 = *(int64_t*)v24;
    v37 = v35 + v36;
    v38 = 0LL;
    v39 = str_slice((void*)v38, v1, v34, v37);
    v40 = malloc(8);
    *(void**)v40 = (void*)v39;
    v42 = *(void**)v40;
    v43 = 0LL;
    v44 = (bool)str_equal((void*)v43, v42, v2);
    if (v44) goto bb7;
    else goto bb8;
  bb6:
    v67 = 1LL;
    v68 = -v67;
    p_call_stack_pop();
    return v68;
  bb7:
    v46 = *(int64_t*)v17;
    v47 = 1LL;
    v48 = v46 + v47;
    *(int64_t*)v17 = (int64_t)v48;
    v50 = *(int64_t*)v17;
    v51 = v50 == v3;
    if (v51) goto bb10;
    else goto bb11;
  bb8:
    v61 = *(int64_t*)v20;
    v62 = 1LL;
    v63 = v61 + v62;
    *(int64_t*)v20 = (int64_t)v63;
    goto bb9;
  bb9:
    goto bb4;
  bb10:
    v53 = *(int64_t*)v20;
    p_call_stack_pop();
    return v53;
  bb11:
    goto bb12;
  bb12:
    v56 = *(int64_t*)v20;
    v57 = *(int64_t*)v24;
    v58 = v56 + v57;
    *(int64_t*)v20 = (int64_t)v58;
    goto bb9;
    return 0;
}

void* str_strip_char(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("str_strip_char");
    int64_t v30;
    int64_t v36;
    int64_t v42;
    void* v8;
    int64_t v13; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    bool v21;
    int64_t v22; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v18;
    bool v31;
    int64_t v43; /* placeholder for void */
    int64_t v7;
    int64_t v14;
    int64_t v15;
    int64_t v24;
    bool v20;
    int64_t v45;
    int64_t v25;
    int64_t v6; /* placeholder for void */
    int64_t v47;
    int64_t v32;
    int64_t v46;
    bool v16;
    int64_t v3;
    int64_t v27; /* placeholder for void */
    int64_t v34;
    bool v37;
    int64_t v39; /* placeholder for void */
    int64_t v10;
    void* v11;
    int64_t v4;
    void* v5;
    int64_t v29;
    int64_t v19;
    int64_t v17;
    int64_t v23;
    int64_t v26; /* placeholder for void */
    int64_t v33;
    bool v38;
    int64_t v41;
    int64_t v40;
    int64_t v44; /* placeholder for void */
    void* v48;
    int64_t v49; /* placeholder for void */
    int64_t v35;
    int64_t v9; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)str_len((void*)v3, v1);
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = 0LL;
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    v10 = *(int64_t*)v5;
    v11 = malloc(8);
    *(int64_t*)v11 = (int64_t)v10;
    goto bb1;
  bb1:
    v14 = *(int64_t*)v8;
    v15 = *(int64_t*)v11;
    v16 = v14 < v15;
    v17 = *(int64_t*)v8;
    v18 = 0LL;
    v19 = (int64_t)str_byte_at((void*)v18, v1, v17);
    v20 = v19 == v2;
    v21 = v16 && v20;
    if (v21) goto bb2;
    else goto bb3;
  bb2:
    v23 = *(int64_t*)v8;
    v24 = 1LL;
    v25 = v23 + v24;
    *(int64_t*)v8 = (int64_t)v25;
    goto bb1;
  bb3:
    goto bb4;
  bb4:
    v29 = *(int64_t*)v11;
    v30 = *(int64_t*)v8;
    v31 = v29 > v30;
    v32 = *(int64_t*)v11;
    v33 = 1LL;
    v34 = v32 - v33;
    v35 = 0LL;
    v36 = (int64_t)str_byte_at((void*)v35, v1, v34);
    v37 = v36 == v2;
    v38 = v31 && v37;
    if (v38) goto bb5;
    else goto bb6;
  bb5:
    v40 = *(int64_t*)v11;
    v41 = 1LL;
    v42 = v40 - v41;
    *(int64_t*)v11 = (int64_t)v42;
    goto bb4;
  bb6:
    v45 = *(int64_t*)v8;
    v46 = *(int64_t*)v11;
    v47 = 0LL;
    v48 = str_slice((void*)v47, v1, v45, v46);
    p_call_stack_pop();
    return v48;
    return 0;
}

int64_t ByteBuffer_read_byte(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("ByteBuffer_read_byte");
    bool v4;
    bool v8;
    int64_t v11;
    int64_t v10;
    int64_t v6;
    int64_t v12; /* placeholder for void */
    int64_t v17;
    void* v14;
    int64_t v13; /* placeholder for void */
    int64_t v16;
    int64_t v9; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    void* v15;
    int64_t v3;
    void* v5;
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
    v17 = (int64_t)Vec_get((void*)v16, v15, v2);
    p_call_stack_pop();
    return v17;
    return 0;
}

void LayerDef_new(void* v0, void* v1, void* v2, void* v3, bool v4) {
    p_call_stack_push("LayerDef_new");
    int64_t v82; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    void* v109;
    int64_t v53;
    void* v137;
    void* v174;
    void* v177;
    int64_t v188; /* placeholder for void */
    void* v14;
    void* v52;
    int64_t v148; /* placeholder for void */
    void* v33;
    int64_t v184; /* placeholder for void */
    void* v191;
    void* v162;
    void* v194;
    void* v170;
    void* v37;
    void* v40;
    void* v121;
    bool v97;
    void* v27;
    int64_t v32; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    void* v138;
    void* v99;
    int64_t v139;
    int64_t v46; /* placeholder for void */
    bool v69;
    void* v123;
    int64_t v152; /* placeholder for void */
    void* v190;
    void* v107;
    int64_t v45;
    void* v48;
    void* v21;
    int64_t v160; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    void* v88;
    int64_t v20; /* placeholder for void */
    int64_t v126; /* placeholder for void */
    void* v185;
    int64_t v70; /* placeholder for void */
    void* v16;
    int64_t v168; /* placeholder for void */
    void* v17;
    int64_t v24; /* placeholder for void */
    void* v125;
    void* v171;
    int64_t v201; /* placeholder for void */
    void* v87;
    void* v128;
    void* v189;
    void* v47;
    void* v71;
    void* v56;
    bool v199;
    void* v131;
    void* v178;
    int64_t v36; /* placeholder for void */
    bool v175;
    void* v145;
    void* v25;
    void* v6;
    void* v30;
    void* v83;
    void* v129;
    int64_t v164; /* placeholder for void */
    void* v61;
    int64_t v180; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    void* v182;
    bool v195;
    int64_t v192; /* placeholder for void */
    void* v197;
    void* v93;
    void* v96;
    void* v101;
    int64_t v7; /* placeholder for void */
    void* v114;
    bool v65;
    void* v132;
    int64_t v140; /* placeholder for void */
    void* v142;
    void* v193;
    int64_t v41;
    int64_t v66; /* placeholder for void */
    int64_t v108; /* placeholder for void */
    void* v103;
    int64_t v57;
    void* v76;
    void* v115;
    int64_t v134; /* placeholder for void */
    void* v136;
    void* v119;
    void* v92;
    void* v43;
    int64_t v156; /* placeholder for void */
    void* v183;
    int64_t v200; /* placeholder for void */
    void* v67;
    void* v10;
    void* v23;
    void* v55;
    void* v95;
    int64_t v58; /* placeholder for void */
    void* v26;
    void* v80;
    void* v157;
    void* v9;
    void* v91;
    void* v181;
    void* v19;
    void* v79;
    void* v34;
    int64_t v54; /* placeholder for void */
    void* v64;
    bool v167;
    void* v22;
    int64_t v62; /* placeholder for void */
    void* v29;
    void* v169;
    void* v44;
    void* v31;
    void* v117;
    void* v124;
    bool v163;
    int64_t v86; /* placeholder for void */
    void* v120;
    void* v51;
    void* v149;
    void* v154;
    void* v18;
    int64_t v49;
    void* v165;
    void* v75;
    int64_t v144; /* placeholder for void */
    void* v63;
    void* v166;
    int64_t v11; /* placeholder for void */
    bool v179;
    void* v35;
    void* v141;
    int64_t v143;
    int64_t v172; /* placeholder for void */
    void* v127;
    void* v38;
    void* v105;
    int64_t v100; /* placeholder for void */
    void* v73;
    void* v111;
    void* v158;
    void* v85;
    void* v39;
    void* v133;
    int64_t v8;
    int64_t v130; /* placeholder for void */
    void* v13;
    void* v102;
    void* v106;
    int64_t v112; /* placeholder for void */
    int64_t v118; /* placeholder for void */
    void* v153;
    void* v186;
    int64_t v196; /* placeholder for void */
    void* v146;
    bool v77;
    int64_t v12;
    int64_t v94; /* placeholder for void */
    void* v110;
    int64_t v98; /* placeholder for void */
    int64_t v147;
    void* v150;
    void* v89;
    void* v173;
    void* v59;
    int64_t v104; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v122; /* placeholder for void */
    void* v72;
    void* v116;
    int64_t v155;
    void* v84;
    void* v187;
    void* v135;
    int64_t v151;
    void* v198;
    int64_t v176; /* placeholder for void */
    void* v159;
    void* v161;
    bool v81;
    void* v68;
    void* v5;
    void* v113;
    void* v60;
    v5 = malloc(192);
    v6 = (char*)v5 + 0;
    *(void**)v6 = (void*)v2;
    v8 = 0LL;
    v9 = Vec_new((void*)v8);
    v10 = (char*)v5 + 8;
    *(void**)v10 = (void*)v9;
    v12 = 0LL;
    v13 = Vec_new((void*)v12);
    v14 = (char*)v5 + 16;
    *(void**)v14 = (void*)v13;
    v16 = (char*)v5 + 24;
    v17 = (char*)v3 + 0;
    v18 = (char*)v16 + 0;
    v19 = *(void**)v17;
    *(void**)v18 = (void*)v19;
    v21 = (char*)v3 + 8;
    v22 = (char*)v16 + 8;
    v23 = *(void**)v21;
    *(void**)v22 = (void*)v23;
    v25 = (char*)v3 + 16;
    v26 = (char*)v16 + 16;
    v27 = *(void**)v25;
    *(void**)v26 = (void*)v27;
    v29 = (char*)v3 + 24;
    v30 = (char*)v16 + 24;
    v31 = *(void**)v29;
    *(void**)v30 = (void*)v31;
    v33 = (char*)v3 + 32;
    v34 = (char*)v16 + 32;
    v35 = *(void**)v33;
    *(void**)v34 = (void*)v35;
    v37 = (char*)v3 + 40;
    v38 = (char*)v16 + 40;
    v39 = (char*)v37 + 0;
    v40 = (char*)v38 + 0;
    v41 = *(int64_t*)v39;
    *(int64_t*)v40 = (int64_t)v41;
    v43 = (char*)v37 + 8;
    v44 = (char*)v38 + 8;
    v45 = *(int64_t*)v43;
    *(int64_t*)v44 = (int64_t)v45;
    v47 = (char*)v37 + 16;
    v48 = (char*)v38 + 16;
    v49 = *(int64_t*)v47;
    *(int64_t*)v48 = (int64_t)v49;
    v51 = (char*)v37 + 24;
    v52 = (char*)v38 + 24;
    v53 = *(int64_t*)v51;
    *(int64_t*)v52 = (int64_t)v53;
    v55 = (char*)v37 + 32;
    v56 = (char*)v38 + 32;
    v57 = *(int64_t*)v55;
    *(int64_t*)v56 = (int64_t)v57;
    v59 = (char*)v3 + 80;
    v60 = (char*)v16 + 80;
    v61 = *(void**)v59;
    *(void**)v60 = (void*)v61;
    v63 = (char*)v3 + 88;
    v64 = (char*)v16 + 88;
    v65 = *(bool*)v63;
    *(bool*)v64 = v65;
    v67 = (char*)v3 + 96;
    v68 = (char*)v16 + 96;
    v69 = *(bool*)v67;
    *(bool*)v68 = v69;
    v71 = (char*)v3 + 104;
    v72 = (char*)v16 + 104;
    v73 = *(void**)v71;
    *(void**)v72 = (void*)v73;
    v75 = (char*)v3 + 112;
    v76 = (char*)v16 + 112;
    v77 = *(bool*)v75;
    *(bool*)v76 = v77;
    v79 = (char*)v3 + 120;
    v80 = (char*)v16 + 120;
    v81 = *(bool*)v79;
    *(bool*)v80 = v81;
    v83 = (char*)v3 + 128;
    v84 = (char*)v16 + 128;
    v85 = *(void**)v83;
    *(void**)v84 = (void*)v85;
    v87 = (char*)v3 + 136;
    v88 = (char*)v16 + 136;
    v89 = *(void**)v87;
    *(void**)v88 = (void*)v89;
    v91 = (char*)v3 + 144;
    v92 = (char*)v16 + 144;
    v93 = *(void**)v91;
    *(void**)v92 = (void*)v93;
    v95 = (char*)v3 + 152;
    v96 = (char*)v16 + 152;
    v97 = *(bool*)v95;
    *(bool*)v96 = v97;
    v99 = (char*)v5 + 184;
    *(bool*)v99 = v4;
    v101 = (char*)v5 + 0;
    v102 = (char*)v0 + 0;
    v103 = *(void**)v101;
    *(void**)v102 = (void*)v103;
    v105 = (char*)v5 + 8;
    v106 = (char*)v0 + 8;
    v107 = *(void**)v105;
    *(void**)v106 = (void*)v107;
    v109 = (char*)v5 + 16;
    v110 = (char*)v0 + 16;
    v111 = *(void**)v109;
    *(void**)v110 = (void*)v111;
    v113 = (char*)v5 + 24;
    v114 = (char*)v0 + 24;
    v115 = (char*)v113 + 0;
    v116 = (char*)v114 + 0;
    v117 = *(void**)v115;
    *(void**)v116 = (void*)v117;
    v119 = (char*)v113 + 8;
    v120 = (char*)v114 + 8;
    v121 = *(void**)v119;
    *(void**)v120 = (void*)v121;
    v123 = (char*)v113 + 16;
    v124 = (char*)v114 + 16;
    v125 = *(void**)v123;
    *(void**)v124 = (void*)v125;
    v127 = (char*)v113 + 24;
    v128 = (char*)v114 + 24;
    v129 = *(void**)v127;
    *(void**)v128 = (void*)v129;
    v131 = (char*)v113 + 32;
    v132 = (char*)v114 + 32;
    v133 = *(void**)v131;
    *(void**)v132 = (void*)v133;
    v135 = (char*)v113 + 40;
    v136 = (char*)v114 + 40;
    v137 = (char*)v135 + 0;
    v138 = (char*)v136 + 0;
    v139 = *(int64_t*)v137;
    *(int64_t*)v138 = (int64_t)v139;
    v141 = (char*)v135 + 8;
    v142 = (char*)v136 + 8;
    v143 = *(int64_t*)v141;
    *(int64_t*)v142 = (int64_t)v143;
    v145 = (char*)v135 + 16;
    v146 = (char*)v136 + 16;
    v147 = *(int64_t*)v145;
    *(int64_t*)v146 = (int64_t)v147;
    v149 = (char*)v135 + 24;
    v150 = (char*)v136 + 24;
    v151 = *(int64_t*)v149;
    *(int64_t*)v150 = (int64_t)v151;
    v153 = (char*)v135 + 32;
    v154 = (char*)v136 + 32;
    v155 = *(int64_t*)v153;
    *(int64_t*)v154 = (int64_t)v155;
    v157 = (char*)v113 + 80;
    v158 = (char*)v114 + 80;
    v159 = *(void**)v157;
    *(void**)v158 = (void*)v159;
    v161 = (char*)v113 + 88;
    v162 = (char*)v114 + 88;
    v163 = *(bool*)v161;
    *(bool*)v162 = v163;
    v165 = (char*)v113 + 96;
    v166 = (char*)v114 + 96;
    v167 = *(bool*)v165;
    *(bool*)v166 = v167;
    v169 = (char*)v113 + 104;
    v170 = (char*)v114 + 104;
    v171 = *(void**)v169;
    *(void**)v170 = (void*)v171;
    v173 = (char*)v113 + 112;
    v174 = (char*)v114 + 112;
    v175 = *(bool*)v173;
    *(bool*)v174 = v175;
    v177 = (char*)v113 + 120;
    v178 = (char*)v114 + 120;
    v179 = *(bool*)v177;
    *(bool*)v178 = v179;
    v181 = (char*)v113 + 128;
    v182 = (char*)v114 + 128;
    v183 = *(void**)v181;
    *(void**)v182 = (void*)v183;
    v185 = (char*)v113 + 136;
    v186 = (char*)v114 + 136;
    v187 = *(void**)v185;
    *(void**)v186 = (void*)v187;
    v189 = (char*)v113 + 144;
    v190 = (char*)v114 + 144;
    v191 = *(void**)v189;
    *(void**)v190 = (void*)v191;
    v193 = (char*)v113 + 152;
    v194 = (char*)v114 + 152;
    v195 = *(bool*)v193;
    *(bool*)v194 = v195;
    v197 = (char*)v5 + 184;
    v198 = (char*)v0 + 184;
    v199 = *(bool*)v197;
    *(bool*)v198 = v199;
    p_call_stack_pop();
    return;
    return;
}

void* Vec_map(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_map");
    void* v12;
    int64_t v29;
    int64_t v30;
    int64_t v7;
    int64_t v19;
    int64_t v24;
    void* v26;
    int64_t v14; /* placeholder for void */
    void* v37;
    int64_t v31; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v3;
    void* v22;
    int64_t v15;
    int64_t v23; /* placeholder for void */
    int64_t v25;
    void* v5;
    int64_t v18; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    void* v8;
    int64_t v21;
    int64_t v10;
    int64_t v13; /* placeholder for void */
    bool v17;
    int64_t v27; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v32;
    int64_t v34;
    int64_t v38; /* placeholder for void */
    int64_t v20;
    void* v28;
    void* v4;
    int64_t v11;
    int64_t v33;
    int64_t v16;
    v3 = 0LL;
    v4 = Vec_new((void*)v3);
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = 0LL;
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    v10 = 0LL;
    v11 = (int64_t)Vec_len((void*)v10, v1);
    v12 = malloc(8);
    *(int64_t*)v12 = (int64_t)v11;
    goto bb1;
  bb1:
    v15 = *(int64_t*)v8;
    v16 = *(int64_t*)v12;
    v17 = v15 < v16;
    if (v17) goto bb2;
    else goto bb3;
  bb2:
    v19 = *(int64_t*)v8;
    v20 = 0LL;
    v21 = (int64_t)Vec_get((void*)v20, v1, v19);
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v21;
    v24 = *(int64_t*)v22;
    {
        if (!v2) abort();
        char* _fn_name = *(char**)v2;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "String_char_count") == 0) {
            v25 = String_char_count((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v25 = String_trim((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v25 = HashMap_len((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v25 = HashMap_keys((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v25 = str_parse_int((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v25 = Vec_capacity((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v25 = p_abs((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "count_expr_nodes") == 0) {
            v25 = count_expr_nodes((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v25 = str_parse_hex((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v25 = String_to_uppercase((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v25 = Result_unwrap((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v25 = String_bytes((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "op_tag") == 0) {
            v25 = op_tag((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v25 = p_str_length((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v25 = HashMap_values((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v25 = p_str_hash((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v25 = Vec_first((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v25 = Vec_len((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v25 = String_clone((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v25 = Option_unwrap((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "vec_enumerate") == 0) {
            v25 = vec_enumerate((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v25 = Vec_pop((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v25 = char_to_lower((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v25 = StringBuilder_len((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v25 = String_from((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v25 = char_to_upper((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v25 = String_to_string((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "stmt_line") == 0) {
            v25 = stmt_line((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v25 = Vec_last((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v25 = str_len((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v25 = HashMap_clone((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v25 = String_parse_int((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v25 = ByteBuffer_len((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v25 = String_to_lowercase((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v25 = allocator_bytes_used((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v25 = String_len((void*)v2, (int64_t)v24);
        } else { abort(); }
    }
    v26 = malloc(8);
    *(int64_t*)v26 = (int64_t)v25;
    v28 = *(void**)v5;
    v29 = *(int64_t*)v26;
    v30 = 0LL;
    Vec_push((void*)v30, v28, v29);
    v32 = *(int64_t*)v8;
    v33 = 1LL;
    v34 = v32 + v33;
    *(int64_t*)v8 = (int64_t)v34;
    goto bb1;
  bb3:
    v37 = *(void**)v5;
    p_call_stack_pop();
    return v37;
    return 0;
}

void LayerInst_new(void* v0, void* v1, void* v2, void* v3, void* v4) {
    p_call_stack_push("LayerInst_new");
    int64_t v24; /* placeholder for void */
    void* v5;
    void* v6;
    void* v14;
    int64_t v19; /* placeholder for void */
    void* v21;
    int64_t v11; /* placeholder for void */
    void* v17;
    void* v16;
    int64_t v7; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    void* v10;
    void* v13;
    void* v18;
    void* v20;
    int64_t v15; /* placeholder for void */
    void* v22;
    int64_t v23; /* placeholder for void */
    void* v8;
    void* v12;
    v5 = malloc(24);
    v6 = (char*)v5 + 0;
    *(void**)v6 = (void*)v2;
    v8 = (char*)v5 + 8;
    *(void**)v8 = (void*)v3;
    v10 = (char*)v5 + 16;
    *(void**)v10 = (void*)v4;
    v12 = (char*)v5 + 0;
    v13 = (char*)v0 + 0;
    v14 = *(void**)v12;
    *(void**)v13 = (void*)v14;
    v16 = (char*)v5 + 8;
    v17 = (char*)v0 + 8;
    v18 = *(void**)v16;
    *(void**)v17 = (void*)v18;
    v20 = (char*)v5 + 16;
    v21 = (char*)v0 + 16;
    v22 = *(void**)v20;
    *(void**)v21 = (void*)v22;
    p_call_stack_pop();
    return;
    return;
}

void* bool_to_str(void* v0, bool v1) {
    p_call_stack_push("bool_to_str");
    int64_t v5; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v2; /* placeholder for void */
    void* v6;
    void* v3;
    int64_t v4; /* placeholder for void */
    if (v1) goto bb1;
    else goto bb2;
  bb1:
    v3 = _str7;
    p_call_stack_pop();
    return v3;
  bb2:
    goto bb3;
  bb3:
    v6 = _str8;
    p_call_stack_pop();
    return v6;
    return 0;
}

int64_t vec_zip(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("vec_zip");
    int64_t v26;
    int64_t v38;
    int64_t v6; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    void* v27;
    int64_t v14; /* placeholder for void */
    int64_t v12;
    int64_t v30;
    int64_t v18;
    bool v32;
    int64_t v35;
    int64_t v46;
    void* v5;
    int64_t v21; /* placeholder for void */
    int64_t v31;
    int64_t v34;
    int64_t v25; /* placeholder for void */
    void* v9;
    int64_t v8;
    void* v16;
    int64_t v33; /* placeholder for void */
    bool v20;
    int64_t v36;
    int64_t v42;
    int64_t v44;
    int64_t v15;
    int64_t v24; /* placeholder for void */
    int64_t v37;
    void* v13;
    int64_t v45; /* placeholder for void */
    int64_t v43;
    int64_t v48;
    int64_t v52; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v3;
    int64_t v51;
    int64_t v4;
    int64_t v22;
    int64_t v19;
    int64_t v40;
    int64_t v17; /* placeholder for void */
    int64_t v41;
    int64_t v11;
    int64_t v49; /* placeholder for void */
    int64_t v7;
    int64_t v47;
    int64_t v10; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)Vec_new((void*)v3);
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = 0LL;
    v8 = (int64_t)Vec_len((void*)v7, (void*)v1);
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 0LL;
    v12 = (int64_t)Vec_len((void*)v11, (void*)v2);
    v13 = malloc(8);
    *(int64_t*)v13 = (int64_t)v12;
    v15 = *(int64_t*)v9;
    v16 = malloc(8);
    *(int64_t*)v16 = (int64_t)v15;
    v18 = *(int64_t*)v13;
    v19 = *(int64_t*)v16;
    v20 = v18 < v19;
    if (v20) goto bb1;
    else goto bb2;
  bb1:
    v22 = *(int64_t*)v13;
    *(int64_t*)v16 = (int64_t)v22;
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v26 = 0LL;
    v27 = malloc(8);
    *(int64_t*)v27 = (int64_t)v26;
    goto bb4;
  bb4:
    v30 = *(int64_t*)v27;
    v31 = *(int64_t*)v16;
    v32 = v30 < v31;
    if (v32) goto bb5;
    else goto bb6;
  bb5:
    v34 = *(int64_t*)v5;
    v35 = *(int64_t*)v27;
    v36 = 0LL;
    v37 = (int64_t)Vec_get((void*)v36, (void*)v1, v35);
    v38 = 0LL;
    Vec_push((void*)v38, (void*)v34, v37);
    v40 = *(int64_t*)v5;
    v41 = *(int64_t*)v27;
    v42 = 0LL;
    v43 = (int64_t)Vec_get((void*)v42, (void*)v2, v41);
    v44 = 0LL;
    Vec_push((void*)v44, (void*)v40, v43);
    v46 = *(int64_t*)v27;
    v47 = 1LL;
    v48 = v46 + v47;
    *(int64_t*)v27 = (int64_t)v48;
    goto bb4;
  bb6:
    v51 = *(int64_t*)v5;
    p_call_stack_pop();
    return v51;
    return 0;
}

void ImportDef_new(void* v0, void* v1, void* v2, bool v3) {
    p_call_stack_push("ImportDef_new");
    void* v75;
    void* v24;
    int64_t v35;
    void* v69;
    void* v33;
    int64_t v77; /* placeholder for void */
    bool v80;
    void* v52;
    int64_t v81; /* placeholder for void */
    bool v84;
    int64_t v85; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    void* v28;
    void* v19;
    void* v13;
    void* v51;
    void* v55;
    void* v34;
    int64_t v22;
    int64_t v62;
    void* v65;
    int64_t v66;
    void* v21;
    int64_t v14; /* placeholder for void */
    void* v60;
    int64_t v23; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    void* v17;
    int64_t v63; /* placeholder for void */
    int64_t v7;
    int64_t v40; /* placeholder for void */
    int64_t v76;
    void* v79;
    int64_t v45; /* placeholder for void */
    void* v83;
    void* v42;
    int64_t v53; /* placeholder for void */
    void* v71;
    int64_t v36; /* placeholder for void */
    int64_t v11;
    int64_t v18; /* placeholder for void */
    void* v38;
    void* v74;
    int64_t v16;
    void* v15;
    void* v48;
    bool v41;
    void* v46;
    int64_t v73; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    void* v9;
    void* v44;
    void* v4;
    void* v54;
    void* v82;
    int64_t v39;
    void* v47;
    int64_t v31; /* placeholder for void */
    void* v12;
    void* v25;
    void* v30;
    int64_t v49; /* placeholder for void */
    void* v59;
    void* v70;
    void* v8;
    int64_t v29;
    void* v78;
    int64_t v72;
    int64_t v10; /* placeholder for void */
    int64_t v26;
    int64_t v6; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    void* v32;
    void* v56;
    void* v37;
    void* v50;
    void* v58;
    void* v64;
    void* v61;
    void* v5;
    void* v20;
    int64_t v57; /* placeholder for void */
    void* v68;
    v4 = malloc(72);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = 0LL;
    v8 = Vec_new((void*)v7);
    v9 = (char*)v4 + 8;
    *(void**)v9 = (void*)v8;
    v11 = 0LL;
    v12 = Vec_new((void*)v11);
    v13 = (char*)v4 + 16;
    *(void**)v13 = (void*)v12;
    v15 = malloc(16);
    v16 = 1LL;
    v17 = (char*)v15 + 0;
    *(int64_t*)v17 = (int64_t)v16;
    v19 = (char*)v4 + 24;
    v20 = (char*)v15 + 0;
    v21 = (char*)v19 + 0;
    v22 = *(int64_t*)v20;
    *(int64_t*)v21 = (int64_t)v22;
    v24 = (char*)v15 + 8;
    v25 = (char*)v19 + 8;
    v26 = *(int64_t*)v24;
    *(int64_t*)v25 = (int64_t)v26;
    v28 = malloc(16);
    v29 = 1LL;
    v30 = (char*)v28 + 0;
    *(int64_t*)v30 = (int64_t)v29;
    v32 = (char*)v4 + 40;
    v33 = (char*)v28 + 0;
    v34 = (char*)v32 + 0;
    v35 = *(int64_t*)v33;
    *(int64_t*)v34 = (int64_t)v35;
    v37 = (char*)v28 + 8;
    v38 = (char*)v32 + 8;
    v39 = *(int64_t*)v37;
    *(int64_t*)v38 = (int64_t)v39;
    v41 = false;
    v42 = (char*)v4 + 56;
    *(bool*)v42 = v41;
    v44 = (char*)v4 + 64;
    *(bool*)v44 = v3;
    v46 = (char*)v4 + 0;
    v47 = (char*)v0 + 0;
    v48 = *(void**)v46;
    *(void**)v47 = (void*)v48;
    v50 = (char*)v4 + 8;
    v51 = (char*)v0 + 8;
    v52 = *(void**)v50;
    *(void**)v51 = (void*)v52;
    v54 = (char*)v4 + 16;
    v55 = (char*)v0 + 16;
    v56 = *(void**)v54;
    *(void**)v55 = (void*)v56;
    v58 = (char*)v4 + 24;
    v59 = (char*)v0 + 24;
    v60 = (char*)v58 + 0;
    v61 = (char*)v59 + 0;
    v62 = *(int64_t*)v60;
    *(int64_t*)v61 = (int64_t)v62;
    v64 = (char*)v58 + 8;
    v65 = (char*)v59 + 8;
    v66 = *(int64_t*)v64;
    *(int64_t*)v65 = (int64_t)v66;
    v68 = (char*)v4 + 40;
    v69 = (char*)v0 + 40;
    v70 = (char*)v68 + 0;
    v71 = (char*)v69 + 0;
    v72 = *(int64_t*)v70;
    *(int64_t*)v71 = (int64_t)v72;
    v74 = (char*)v68 + 8;
    v75 = (char*)v69 + 8;
    v76 = *(int64_t*)v74;
    *(int64_t*)v75 = (int64_t)v76;
    v78 = (char*)v4 + 56;
    v79 = (char*)v0 + 56;
    v80 = *(bool*)v78;
    *(bool*)v79 = v80;
    v82 = (char*)v4 + 64;
    v83 = (char*)v0 + 64;
    v84 = *(bool*)v82;
    *(bool*)v83 = v84;
    p_call_stack_pop();
    return;
    return;
}

bool str_is_blank(void* v0, void* v1) {
    p_call_stack_push("str_is_blank");
    bool v18;
    int64_t v20; /* placeholder for void */
    int64_t v23;
    int64_t v24; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    bool v26;
    int64_t v4; /* placeholder for void */
    int64_t v16;
    int64_t v22;
    int64_t v8;
    int64_t v27; /* placeholder for void */
    int64_t v11;
    int64_t v17; /* placeholder for void */
    int64_t v21;
    int64_t v2;
    void* v3;
    bool v9;
    int64_t v12;
    bool v15;
    int64_t v6;
    int64_t v7;
    int64_t v19; /* placeholder for void */
    int64_t v13;
    int64_t v10; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v14;
    v2 = 0LL;
    v3 = malloc(8);
    *(int64_t*)v3 = (int64_t)v2;
    goto bb1;
  bb1:
    v6 = *(int64_t*)v3;
    v7 = 0LL;
    v8 = (int64_t)str_len((void*)v7, v1);
    v9 = v6 < v8;
    if (v9) goto bb2;
    else goto bb3;
  bb2:
    v11 = *(int64_t*)v3;
    v12 = 0LL;
    v13 = (int64_t)str_byte_at((void*)v12, v1, v11);
    v14 = 0LL;
    v15 = (bool)char_is_whitespace((void*)v14, v13);
    v16 = !v15;
    if (v16) goto bb4;
    else goto bb5;
  bb3:
    v26 = true;
    p_call_stack_pop();
    return v26;
  bb4:
    v18 = false;
    p_call_stack_pop();
    return v18;
  bb5:
    goto bb6;
  bb6:
    v21 = *(int64_t*)v3;
    v22 = 1LL;
    v23 = v21 + v22;
    *(int64_t*)v3 = (int64_t)v23;
    goto bb1;
    return 0;
}

int64_t vec_window(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("vec_window");
    int64_t v29; /* placeholder for void */
    int64_t v7;
    int64_t v8;
    int64_t v15; /* placeholder for void */
    bool v24;
    int64_t v31;
    int64_t v6; /* placeholder for void */
    void* v9;
    int64_t v26;
    int64_t v10; /* placeholder for void */
    int64_t v23;
    int64_t v22;
    int64_t v14;
    int64_t v21;
    void* v18;
    int64_t v34; /* placeholder for void */
    int64_t v3;
    int64_t v16; /* placeholder for void */
    int64_t v28;
    int64_t v25; /* placeholder for void */
    void* v5;
    int64_t v19; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v35;
    int64_t v11;
    bool v12;
    int64_t v20; /* placeholder for void */
    int64_t v27;
    int64_t v30;
    int64_t v36; /* placeholder for void */
    int64_t v17;
    int64_t v32;
    int64_t v4;
    v3 = 0LL;
    v4 = (int64_t)Vec_new((void*)v3);
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = 0LL;
    v8 = (int64_t)Vec_len((void*)v7, (void*)v1);
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 0LL;
    v12 = v2 <= v11;
    if (v12) goto bb1;
    else goto bb2;
  bb1:
    v14 = *(int64_t*)v5;
    p_call_stack_pop();
    return v14;
  bb2:
    goto bb3;
  bb3:
    v17 = 0LL;
    v18 = malloc(8);
    *(int64_t*)v18 = (int64_t)v17;
    goto bb4;
  bb4:
    v21 = *(int64_t*)v18;
    v22 = v21 + v2;
    v23 = *(int64_t*)v9;
    v24 = v22 <= v23;
    if (v24) goto bb5;
    else goto bb6;
  bb5:
    v26 = *(int64_t*)v5;
    v27 = *(int64_t*)v18;
    v28 = 0LL;
    Vec_push((void*)v28, (void*)v26, v27);
    v30 = *(int64_t*)v18;
    v31 = 1LL;
    v32 = v30 + v31;
    *(int64_t*)v18 = (int64_t)v32;
    goto bb4;
  bb6:
    v35 = *(int64_t*)v5;
    p_call_stack_pop();
    return v35;
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

int64_t String_byte_at(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_byte_at");
    int64_t v26; /* placeholder for void */
    int64_t v14;
    int64_t v3;
    bool v11;
    uint8_t v28;
    int64_t v40; /* placeholder for void */
    int64_t v23;
    int64_t v25; /* placeholder for void */
    int64_t v39;
    int64_t v8; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v13;
    int64_t v37; /* placeholder for void */
    int64_t v6;
    int64_t v17;
    int64_t v10;
    int64_t v32;
    bool v33;
    bool v4;
    int64_t v7;
    int64_t v19; /* placeholder for void */
    int64_t v20;
    void* v27;
    int64_t v31;
    int64_t v12; /* placeholder for void */
    int64_t v35;
    int64_t v9; /* placeholder for void */
    void* v18;
    void* v24;
    int64_t v36;
    int64_t v22; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    void* v21;
    int64_t v30; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v15; /* placeholder for void */
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
    v11 = v2 < v10;
    if (v11) goto bb4;
    else goto bb5;
  bb4:
    v13 = 1LL;
    v14 = -v13;
    p_call_stack_pop();
    return v14;
  bb5:
    goto bb6;
  bb6:
    v17 = v1 + v2;
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
    v33 = v31 == v32;
    if (v33) goto bb7;
    else goto bb8;
  bb7:
    v35 = 1LL;
    v36 = -v35;
    p_call_stack_pop();
    return v36;
  bb8:
    goto bb9;
  bb9:
    v39 = *(int64_t*)v24;
    p_call_stack_pop();
    return v39;
    return 0;
}

void assert_msg(void* v0, bool v1, void* v2) {
    p_call_stack_push("assert_msg");
    int64_t v6; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v3;
    int64_t v8; /* placeholder for void */
    v3 = !v1;
    if (v3) goto bb1;
    else goto bb2;
  bb1:
    p_panic(v2);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return;
    return;
}

void* str_remove_char(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("str_remove_char");
    int64_t v26;
    void* v5;
    int64_t v20; /* placeholder for void */
    int64_t v21;
    int64_t v32;
    int64_t v34; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v13;
    bool v14;
    int64_t v16;
    int64_t v12;
    bool v22;
    int64_t v27; /* placeholder for void */
    void* v24;
    int64_t v29; /* placeholder for void */
    int64_t v11;
    void* v4;
    int64_t v28; /* placeholder for void */
    int64_t v30;
    int64_t v31;
    int64_t v33; /* placeholder for void */
    int64_t v36;
    int64_t v9; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v3;
    void* v35;
    int64_t v25;
    int64_t v17;
    int64_t v18;
    void* v19;
    int64_t v7;
    void* v37;
    int64_t v38; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v8;
    v3 = 0LL;
    v4 = Vec_new((void*)v3);
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = 0LL;
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    goto bb1;
  bb1:
    v11 = *(int64_t*)v8;
    v12 = 0LL;
    v13 = (int64_t)str_len((void*)v12, v1);
    v14 = v11 < v13;
    if (v14) goto bb2;
    else goto bb3;
  bb2:
    v16 = *(int64_t*)v8;
    v17 = 0LL;
    v18 = (int64_t)str_byte_at((void*)v17, v1, v16);
    v19 = malloc(8);
    *(int64_t*)v19 = (int64_t)v18;
    v21 = *(int64_t*)v19;
    v22 = v21 != v2;
    if (v22) goto bb4;
    else goto bb5;
  bb3:
    v35 = *(void**)v5;
    v36 = 0LL;
    v37 = str_from_bytes((void*)v36, v35);
    p_call_stack_pop();
    return v37;
  bb4:
    v24 = *(void**)v5;
    v25 = *(int64_t*)v19;
    v26 = 0LL;
    Vec_push((void*)v26, v24, v25);
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v30 = *(int64_t*)v8;
    v31 = 1LL;
    v32 = v30 + v31;
    *(int64_t*)v8 = (int64_t)v32;
    goto bb1;
    return 0;
}

void Result_map(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Result_map");
    int64_t v8; /* placeholder for void */
    int64_t v10;
    int64_t v25; /* placeholder for void */
    void* v22;
    void* v32;
    int64_t v12;
    int64_t v43; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v24;
    void* v40;
    void* v18;
    int64_t v21; /* placeholder for void */
    void* v27;
    int64_t v31; /* placeholder for void */
    int64_t v29;
    int64_t v35;
    int64_t v37; /* placeholder for void */
    int64_t v46;
    int64_t v47; /* placeholder for void */
    int64_t v6;
    bool v30;
    void* v34;
    void* v19;
    int64_t v33;
    void* v38;
    int64_t v48; /* placeholder for void */
    void* v9;
    void* v41;
    void* v4;
    int64_t v20;
    int64_t v5;
    int64_t v15;
    int64_t v42;
    void* v11;
    bool v7;
    int64_t v17; /* placeholder for void */
    int64_t v28;
    int64_t v39; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    void* v36;
    void* v13;
    void* v16;
    void* v44;
    void* v45;
    void* v23;
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
        if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "count_expr_nodes") == 0) {
            v15 = count_expr_nodes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "op_tag") == 0) {
            v15 = op_tag((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "vec_enumerate") == 0) {
            v15 = vec_enumerate((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "stmt_line") == 0) {
            v15 = stmt_line((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
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

int64_t str_parse_hex(void* v0, void* v1) {
    p_call_stack_push("str_parse_hex");
    int64_t v191; /* placeholder for void */
    void* v63;
    bool v195;
    int64_t v177;
    int64_t v220; /* placeholder for void */
    int64_t v23;
    int64_t v2;
    int64_t v105;
    void* v109;
    int64_t v114; /* placeholder for void */
    int64_t v120;
    int64_t v41; /* placeholder for void */
    int64_t v42;
    int64_t v134; /* placeholder for void */
    int64_t v148;
    int64_t v154; /* placeholder for void */
    int64_t v183;
    int64_t v201;
    int64_t v174; /* placeholder for void */
    int64_t v222;
    int64_t v91;
    bool v138;
    void* v178;
    int64_t v221; /* placeholder for void */
    int64_t v200; /* placeholder for void */
    int64_t v28;
    void* v129;
    int64_t v140;
    int64_t v216;
    int64_t v67; /* placeholder for void */
    int64_t v139;
    int64_t v18; /* placeholder for void */
    void* v69;
    int64_t v59; /* placeholder for void */
    int64_t v104;
    bool v8;
    int64_t v46; /* placeholder for void */
    int64_t v180;
    int64_t v118; /* placeholder for void */
    int64_t v149;
    int64_t v57; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v142;
    int64_t v168;
    int64_t v7;
    int64_t v136;
    int64_t v240; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v130; /* placeholder for void */
    bool v211;
    int64_t v161;
    int64_t v239; /* placeholder for void */
    int64_t v165;
    int64_t v65;
    void* v72;
    void* v169;
    int64_t v121;
    int64_t v34;
    int64_t v176;
    int64_t v135; /* placeholder for void */
    int64_t v77;
    bool v78;
    int64_t v83;
    void* v17;
    int64_t v12; /* placeholder for void */
    int64_t v225;
    int64_t v58;
    int64_t v10;
    bool v233;
    int64_t v218; /* placeholder for void */
    int64_t v217; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    int64_t v111;
    bool v141;
    int64_t v82;
    int64_t v190;
    int64_t v238; /* placeholder for void */
    void* v163;
    void* v66;
    int64_t v147; /* placeholder for void */
    int64_t v153; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    uint8_t v73;
    int64_t v71; /* placeholder for void */
    void* v166;
    int64_t v21; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    bool v44;
    int64_t v175; /* placeholder for void */
    int64_t v207;
    int64_t v210;
    int64_t v224;
    int64_t v75; /* placeholder for void */
    int64_t v242; /* placeholder for void */
    int64_t v232; /* placeholder for void */
    int64_t v101;
    bool v145;
    int64_t v150;
    void* v106;
    uint8_t v173;
    void* v35;
    bool v146;
    void* v38;
    bool v212;
    int64_t v189;
    int64_t v48; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    bool v199;
    int64_t v86; /* placeholder for void */
    int64_t v49;
    int64_t v181;
    int64_t v47; /* placeholder for void */
    void* v115;
    bool v25;
    int64_t v6;
    int64_t v167; /* placeholder for void */
    int64_t v230;
    int64_t v184;
    int64_t v33; /* placeholder for void */
    int64_t v152; /* placeholder for void */
    int64_t v179; /* placeholder for void */
    bool v186;
    int64_t v197;
    int64_t v60; /* placeholder for void */
    int64_t v45;
    uint8_t v133;
    int64_t v43;
    bool v182;
    int64_t v193;
    int64_t v223;
    int64_t v151; /* placeholder for void */
    int64_t v203;
    int64_t v205; /* placeholder for void */
    int64_t v3;
    int64_t v27;
    int64_t v170; /* placeholder for void */
    int64_t v237;
    void* v4;
    int64_t v93;
    void* v20;
    int64_t v157;
    bool v56;
    int64_t v52; /* placeholder for void */
    int64_t v214;
    int64_t v164; /* placeholder for void */
    int64_t v108;
    int64_t v84;
    int64_t v235;
    int64_t v68;
    int64_t v160; /* placeholder for void */
    int64_t v88;
    int64_t v127; /* placeholder for void */
    int64_t v219; /* placeholder for void */
    void* v32;
    int64_t v54;
    bool v89;
    bool v144;
    int64_t v156; /* placeholder for void */
    int64_t v236;
    int64_t v26; /* placeholder for void */
    uint8_t v39;
    int64_t v231; /* placeholder for void */
    int64_t v51;
    int64_t v171; /* placeholder for void */
    int64_t v206;
    int64_t v64; /* placeholder for void */
    int64_t v124; /* placeholder for void */
    int64_t v131; /* placeholder for void */
    int64_t v137;
    int64_t v50;
    int64_t v87;
    int64_t v107; /* placeholder for void */
    int64_t v204; /* placeholder for void */
    int64_t v119;
    int64_t v192; /* placeholder for void */
    int64_t v196;
    int64_t v113; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    void* v126;
    int64_t v85; /* placeholder for void */
    int64_t v228;
    int64_t v229;
    int64_t v70; /* placeholder for void */
    int64_t v227; /* placeholder for void */
    uint8_t v116;
    bool v80;
    int64_t v215;
    int64_t v234; /* placeholder for void */
    int64_t v226;
    int64_t v36; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v122;
    int64_t v22; /* placeholder for void */
    int64_t v103; /* placeholder for void */
    int64_t v187; /* placeholder for void */
    bool v19;
    int64_t v128;
    int64_t v11; /* placeholder for void */
    int64_t v61;
    int64_t v62;
    int64_t v31;
    int64_t v213; /* placeholder for void */
    void* v14;
    void* v29;
    int64_t v96; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    bool v185;
    int64_t v81; /* placeholder for void */
    int64_t v24;
    int64_t v76;
    bool v102;
    int64_t v97; /* placeholder for void */
    int64_t v98;
    bool v208;
    int64_t v125;
    void* v112;
    int64_t v37; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v13;
    int64_t v117; /* placeholder for void */
    bool v159;
    void* v123;
    int64_t v162;
    int64_t v194;
    int64_t v55;
    int64_t v202;
    int64_t v95; /* placeholder for void */
    bool v198;
    int64_t v158;
    void* v132;
    int64_t v155; /* placeholder for void */
    void* v172;
    int64_t v99;
    int64_t v209;
    int64_t v143;
    int64_t v16;
    int64_t v110; /* placeholder for void */
    int64_t v188;
    int64_t v100;
    int64_t v241;
    int64_t v92;
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

void HashMap_remove(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_remove");
    int64_t v86; /* placeholder for void */
    void* v5;
    int64_t v30; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v79;
    int64_t v48; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v8;
    int64_t v7;
    int64_t v23; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v54;
    int64_t v46;
    int64_t v62;
    int64_t v73; /* placeholder for void */
    int64_t v4;
    int64_t v74;
    int64_t v84;
    void* v15;
    int64_t v83;
    int64_t v43;
    int64_t v58;
    void* v85;
    int64_t v20;
    int64_t v55; /* placeholder for void */
    int64_t v87; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    void* v47;
    int64_t v37; /* placeholder for void */
    int64_t v26;
    int64_t v36;
    bool v59;
    int64_t v60;
    int64_t v66; /* placeholder for void */
    void* v44;
    int64_t v28; /* placeholder for void */
    int64_t v40;
    int64_t v65; /* placeholder for void */
    void* v27;
    int64_t v67; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    int64_t v49;
    int64_t v89;
    bool v21;
    int64_t v90; /* placeholder for void */
    void* v32;
    void* v75;
    void* v53;
    int64_t v6; /* placeholder for void */
    int64_t v41;
    int64_t v57;
    int64_t v34; /* placeholder for void */
    void* v78;
    int64_t v80; /* placeholder for void */
    void* v29;
    int64_t v25;
    int64_t v33; /* placeholder for void */
    int64_t v19;
    int64_t v11;
    int64_t v72; /* placeholder for void */
    void* v12;
    int64_t v77; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v31;
    bool v63;
    int64_t v71;
    int64_t v68; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v35;
    int64_t v51; /* placeholder for void */
    int64_t v39;
    int64_t v42;
    int64_t v82;
    int64_t v61;
    int64_t v18; /* placeholder for void */
    void* v88;
    void* v69;
    int64_t v92; /* placeholder for void */
    int64_t v16;
    int64_t v70;
    void* v9;
    void* v50;
    int64_t v3;
    bool v64;
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

void Result_err(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_err");
    void* v3;
    void* v34;
    int64_t v4;
    int64_t v39; /* placeholder for void */
    void* v15;
    int64_t v21; /* placeholder for void */
    void* v23;
    int64_t v31;
    void* v40;
    int64_t v13; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    void* v36;
    void* v28;
    void* v32;
    int64_t v42;
    int64_t v29;
    int64_t v7; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    bool v6;
    void* v12;
    void* v8;
    int64_t v43; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v5;
    bool v26;
    int64_t v27; /* placeholder for void */
    void* v14;
    void* v19;
    int64_t v9;
    int64_t v17; /* placeholder for void */
    int64_t v24;
    void* v10;
    int64_t v11;
    void* v18;
    void* v30;
    void* v37;
    int64_t v38;
    void* v41;
    int64_t v16;
    int64_t v35; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v25;
    int64_t v20;
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

void* ByteBuffer_to_str(void* v0, void* v1) {
    p_call_stack_push("ByteBuffer_to_str");
    void* v24;
    void* v15;
    int64_t v26;
    int64_t v29; /* placeholder for void */
    int64_t v9;
    void* v21;
    bool v12;
    int64_t v16;
    void* v31;
    int64_t v25; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v14;
    int64_t v11;
    void* v6;
    int64_t v28;
    int64_t v18;
    void* v19;
    int64_t v7; /* placeholder for void */
    int64_t v5;
    void* v3;
    int64_t v22;
    int64_t v30; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    void* v2;
    int64_t v27;
    int64_t v13; /* placeholder for void */
    int64_t v17;
    void* v10;
    int64_t v4; /* placeholder for void */
    void* v23;
    v2 = _str5;
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
    v18 = (int64_t)Vec_get((void*)v17, v15, v16);
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

bool Result_is_ok(void* v0, void* v1) {
    p_call_stack_push("Result_is_ok");
    int64_t v20; /* placeholder for void */
    int64_t v4;
    bool v5;
    void* v2;
    bool v9;
    int64_t v22; /* placeholder for void */
    void* v7;
    int64_t v15; /* placeholder for void */
    bool v21;
    int64_t v12;
    void* v11;
    int64_t v17;
    int64_t v3;
    bool v14;
    int64_t v8;
    int64_t v6; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v16;
    int64_t v19; /* placeholder for void */
    bool v18;
    int64_t v13;
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

int64_t Vec_get(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_get");
    int64_t v3;
    int64_t v4; /* placeholder for void */
    v3 = (int64_t)p_vec_get(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t main_entry(void* v0) {
    p_call_stack_push("main");
    void* v3132;
    int64_t v1078; /* placeholder for void */
    int64_t v4067;
    void* v2571;
    void* v901;
    void* v3381;
    void* v3525;
    int64_t v515; /* placeholder for void */
    int64_t v3041;
    void* v4118;
    int64_t v2724; /* placeholder for void */
    void* v4007;
    bool v3317;
    void* v3446;
    int64_t v3295; /* placeholder for void */
    int64_t v661;
    int64_t v3302; /* placeholder for void */
    void* v652;
    void* v693;
    void* v3204;
    void* v1854;
    int64_t v2901;
    void* v369;
    void* v4135;
    int64_t v2515; /* placeholder for void */
    void* v2726;
    int64_t v1127; /* placeholder for void */
    int64_t v1406;
    int64_t v2507;
    void* v1557;
    int64_t v4022; /* placeholder for void */
    void* v2616;
    int64_t v2715;
    void* v786;
    int64_t v3261;
    void* v4351;
    void* v354;
    int64_t v676; /* placeholder for void */
    void* v1392;
    void* v2534;
    int64_t v2615; /* placeholder for void */
    void* v3631;
    int64_t v830; /* placeholder for void */
    int64_t v628; /* placeholder for void */
    int64_t v701; /* placeholder for void */
    int64_t v3687; /* placeholder for void */
    int64_t v682;
    void* v3008;
    void* v3836;
    int64_t v2386; /* placeholder for void */
    int64_t v1899;
    void* v157;
    void* v613;
    int64_t v1094;
    int64_t v4126; /* placeholder for void */
    void* v2718;
    int64_t v1341;
    void* v807;
    bool v4140;
    void* v3537;
    int64_t v4079;
    int64_t v4314; /* placeholder for void */
    void* v1611;
    void* v611;
    int64_t v1183;
    int64_t v1621;
    int64_t v3843;
    int64_t v3869; /* placeholder for void */
    void* v1111;
    void* v1793;
    int64_t v1173;
    void* v1773;
    int64_t v2682; /* placeholder for void */
    int64_t v1477;
    int64_t v739;
    int64_t v3037;
    int64_t v2239; /* placeholder for void */
    void* v3879;
    void* v3111;
    void* v1495;
    int64_t v2988; /* placeholder for void */
    int64_t v2087;
    int64_t v2281;
    int64_t v2202;
    int64_t v3328; /* placeholder for void */
    int64_t v722; /* placeholder for void */
    void* v1656;
    int64_t v2902;
    void* v4002;
    int64_t v1569; /* placeholder for void */
    void* v3072;
    void* v2926;
    void* v3399;
    void* v4301;
    void* v4376;
    void* v572;
    int64_t v353;
    void* v2434;
    void* v4364;
    int64_t v3983; /* placeholder for void */
    bool v336;
    int64_t v3583; /* placeholder for void */
    int64_t v1940;
    double v1057;
    void* v1808;
    int64_t v678; /* placeholder for void */
    void* v3947;
    void* v2223;
    void* v2335;
    int64_t v3165; /* placeholder for void */
    void* v3959;
    int64_t v4177; /* placeholder for void */
    int64_t v432;
    int64_t v905; /* placeholder for void */
    int64_t v503; /* placeholder for void */
    void* v2304;
    void* v1329;
    void* v1348;
    void* v49;
    void* v2845;
    void* v2944;
    void* v4368;
    void* v941;
    void* v3665;
    int64_t v2419; /* placeholder for void */
    int64_t v1643;
    int64_t v2586;
    int64_t v1185; /* placeholder for void */
    int64_t v2317;
    void* v4337;
    int64_t v1082; /* placeholder for void */
    int64_t v1533; /* placeholder for void */
    int64_t v3083;
    void* v3666;
    int64_t v1102; /* placeholder for void */
    void* v34;
    int64_t v2158; /* placeholder for void */
    int64_t v3258;
    void* v3118;
    int64_t v1876;
    void* v271;
    int64_t v4108;
    int64_t v3918; /* placeholder for void */
    void* v4352;
    void* v4056;
    int64_t v1623;
    int64_t v3623; /* placeholder for void */
    int64_t v1675; /* placeholder for void */
    int64_t v1661;
    int64_t v85;
    void* v1045;
    int64_t v1238;
    void* v1622;
    int64_t v1064; /* placeholder for void */
    int64_t v2181;
    void* v3911;
    int64_t v6; /* placeholder for void */
    int64_t v688;
    int64_t v220;
    void* v332;
    void* v3809;
    void* v1633;
    int64_t v3794; /* placeholder for void */
    int64_t v1765;
    int64_t v1922; /* placeholder for void */
    int64_t v2916;
    int64_t v538;
    void* v3759;
    int64_t v3929; /* placeholder for void */
    void* v530;
    void* v2212;
    int64_t v2232;
    int64_t v2949;
    int64_t v1044;
    void* v2673;
    void* v203;
    int64_t v2693; /* placeholder for void */
    void* v3271;
    int64_t v1242; /* placeholder for void */
    int64_t v1536; /* placeholder for void */
    int64_t v269; /* placeholder for void */
    void* v1263;
    void* v1843;
    void* v2228;
    void* v3268;
    void* v698;
    int64_t v1310; /* placeholder for void */
    int64_t v883; /* placeholder for void */
    int64_t v3562;
    int64_t v3605; /* placeholder for void */
    int64_t v385; /* placeholder for void */
    int64_t v2759; /* placeholder for void */
    void* v2828;
    int64_t v2199;
    void* v4293;
    void* v2073;
    int64_t v145;
    int64_t v862;
    void* v278;
    int64_t v223;
    void* v1307;
    void* v1690;
    bool v2227;
    int64_t v2854;
    void* v3189;
    int64_t v4162; /* placeholder for void */
    void* v620;
    void* v849;
    void* v1706;
    void* v2558;
    int64_t v918; /* placeholder for void */
    int64_t v3021;
    int64_t v3563; /* placeholder for void */
    int64_t v3124;
    int64_t v275; /* placeholder for void */
    int64_t v2382; /* placeholder for void */
    int64_t v1304; /* placeholder for void */
    void* v3864;
    void* v2687;
    int64_t v3269; /* placeholder for void */
    void* v3625;
    int64_t v3332;
    void* v3634;
    void* v32;
    void* v2376;
    int64_t v2348;
    void* v1665;
    void* v3327;
    int64_t v2286; /* placeholder for void */
    void* v3722;
    void* v4372;
    void* v1827;
    void* v3820;
    void* v893;
    int64_t v4136; /* placeholder for void */
    int64_t v458;
    int64_t v1798; /* placeholder for void */
    int64_t v1868; /* placeholder for void */
    void* v1260;
    int64_t v1049; /* placeholder for void */
    void* v67;
    int64_t v600; /* placeholder for void */
    int64_t v664;
    void* v3319;
    void* v3870;
    int64_t v4278; /* placeholder for void */
    int64_t v140; /* placeholder for void */
    bool v2918;
    void* v3777;
    void* v1453;
    void* v3473;
    int64_t v3681; /* placeholder for void */
    void* v634;
    void* v4329;
    int64_t v609; /* placeholder for void */
    int64_t v2020;
    void* v2344;
    int64_t v2815; /* placeholder for void */
    void* v62;
    void* v1597;
    int64_t v1980; /* placeholder for void */
    int64_t v897;
    void* v456;
    int64_t v1619;
    int64_t v3889;
    int64_t v802; /* placeholder for void */
    int64_t v1165; /* placeholder for void */
    void* v207;
    bool v2426;
    int64_t v4190; /* placeholder for void */
    int64_t v437;
    int64_t v305; /* placeholder for void */
    int64_t v1431; /* placeholder for void */
    int64_t v3406; /* placeholder for void */
    int64_t v3831; /* placeholder for void */
    void* v7;
    int64_t v547; /* placeholder for void */
    int64_t v4325;
    int64_t v227;
    void* v2125;
    int64_t v71;
    void* v3730;
    int64_t v653; /* placeholder for void */
    int64_t v28;
    void* v3624;
    void* v731;
    void* v436;
    int64_t v4266; /* placeholder for void */
    int64_t v1142; /* placeholder for void */
    int64_t v2539;
    void* v4344;
    int64_t v3413; /* placeholder for void */
    void* v532;
    void* v2338;
    void* v1591;
    void* v4036;
    void* v1812;
    void* v882;
    int64_t v700;
    void* v3672;
    int64_t v1912;
    void* v69;
    void* v774;
    int64_t v873; /* placeholder for void */
    int64_t v2215;
    int64_t v455; /* placeholder for void */
    void* v4327;
    int64_t v4211; /* placeholder for void */
    int64_t v4205; /* placeholder for void */
    void* v2117;
    void* v3309;
    int64_t v569; /* placeholder for void */
    void* v2508;
    void* v3937;
    int64_t v413; /* placeholder for void */
    void* v134;
    void* v833;
    void* v3390;
    int64_t v3392; /* placeholder for void */
    void* v3882;
    void* v4358;
    void* v3699;
    int64_t v2678; /* placeholder for void */
    int64_t v3554;
    void* v1667;
    void* v695;
    void* v3899;
    int64_t v2389;
    int64_t v3038;
    int64_t v2809;
    int64_t v1502; /* placeholder for void */
    void* v3432;
    void* v2830;
    int64_t v1951; /* placeholder for void */
    void* v3248;
    int64_t v3690;
    void* v502;
    void* v1915;
    int64_t v3087; /* placeholder for void */
    void* v563;
    int64_t v3963; /* placeholder for void */
    int64_t v1869;
    int64_t v2035; /* placeholder for void */
    int64_t v2438;
    int64_t v3250; /* placeholder for void */
    void* v3995;
    void* v393;
    int64_t v2083; /* placeholder for void */
    int64_t v2048;
    int64_t v2231; /* placeholder for void */
    int64_t v1004;
    void* v3007;
    int64_t v870;
    int64_t v4207; /* placeholder for void */
    int64_t v4225; /* placeholder for void */
    int64_t v2134;
    int64_t v4253;
    int64_t v4349;
    void* v3867;
    int64_t v4130; /* placeholder for void */
    void* v2745;
    int64_t v2520;
    int64_t v885; /* placeholder for void */
    void* v4215;
    int64_t v2566; /* placeholder for void */
    void* v4066;
    void* v3175;
    void* v4069;
    int64_t v2218; /* placeholder for void */
    void* v2372;
    int64_t v582; /* placeholder for void */
    void* v2882;
    int64_t v2945;
    int64_t v1653; /* placeholder for void */
    int64_t v1399; /* placeholder for void */
    void* v3908;
    int64_t v1139; /* placeholder for void */
    void* v692;
    void* v1566;
    void* v1921;
    void* v3577;
    int64_t v4139;
    void* v3348;
    bool v1468;
    void* v3782;
    void* v3923;
    void* v2368;
    int64_t v2966; /* placeholder for void */
    void* v273;
    int64_t v1115;
    void* v147;
    void* v124;
    void* v204;
    int64_t v1475; /* placeholder for void */
    void* v1754;
    int64_t v2964; /* placeholder for void */
    void* v2643;
    int64_t v3461; /* placeholder for void */
    void* v3707;
    void* v4382;
    int64_t v1829;
    int64_t v1290;
    double v1062;
    void* v2269;
    int64_t v471;
    int64_t v519;
    int64_t v3784;
    int64_t v727; /* placeholder for void */
    void* v1562;
    int64_t v3251; /* placeholder for void */
    void* v1143;
    void* v2339;
    void* v937;
    void* v1913;
    int64_t v2303;
    int64_t v3057;
    void* v3358;
    int64_t v3448; /* placeholder for void */
    void* v4197;
    int64_t v2279;
    int64_t v3293;
    void* v3740;
    int64_t v598;
    void* v1344;
    void* v2939;
    void* v3256;
    int64_t v618; /* placeholder for void */
    void* v2600;
    int64_t v1573; /* placeholder for void */
    int64_t v1860; /* placeholder for void */
    int64_t v234;
    int64_t v50; /* placeholder for void */
    void* v1388;
    int64_t v208;
    int64_t v1776; /* placeholder for void */
    int64_t v768;
    void* v2610;
    int64_t v2888; /* placeholder for void */
    void* v211;
    void* v514;
    int64_t v1845;
    int64_t v2282; /* placeholder for void */
    void* v3457;
    int64_t v958;
    int64_t v2395; /* placeholder for void */
    void* v3294;
    int64_t v3163;
    int64_t v1889;
    int64_t v2290; /* placeholder for void */
    int64_t v3874;
    void* v4378;
    int64_t v4147; /* placeholder for void */
    int64_t v3235; /* placeholder for void */
    int64_t v2310; /* placeholder for void */
    int64_t v1250; /* placeholder for void */
    void* v3288;
    int64_t v3557;
    void* v2107;
    int64_t v2055; /* placeholder for void */
    void* v927;
    bool v2677;
    int64_t v4050; /* placeholder for void */
    int64_t v3855; /* placeholder for void */
    int64_t v902; /* placeholder for void */
    int64_t v1943;
    int64_t v3060; /* placeholder for void */
    int64_t v3260; /* placeholder for void */
    int64_t v4040;
    bool v4017;
    void* v1509;
    void* v1327;
    void* v2405;
    int64_t v1781; /* placeholder for void */
    int64_t v2423;
    bool v2804;
    void* v3168;
    void* v304;
    void* v3455;
    int64_t v1856;
    void* v3900;
    int64_t v81;
    int64_t v4033; /* placeholder for void */
    void* v4393;
    int64_t v834; /* placeholder for void */
    void* v3783;
    int64_t v1282;
    int64_t v2110;
    void* v2910;
    void* v965;
    void* v2832;
    int64_t v3732;
    void* v1731;
    int64_t v2752; /* placeholder for void */
    void* v3045;
    void* v2147;
    void* v3366;
    int64_t v4192; /* placeholder for void */
    void* v2748;
    void* v224;
    int64_t v3384; /* placeholder for void */
    int64_t v2941;
    void* v1319;
    int64_t v1817;
    int64_t v1761; /* placeholder for void */
    void* v1530;
    int64_t v2409; /* placeholder for void */
    int64_t v3033; /* placeholder for void */
    void* v858;
    int64_t v1596;
    void* v2079;
    int64_t v3523; /* placeholder for void */
    void* v656;
    void* v2485;
    void* v1638;
    void* v3484;
    void* v2801;
    void* v1722;
    void* v3512;
    int64_t v3498; /* placeholder for void */
    bool v4080;
    int64_t v1891; /* placeholder for void */
    int64_t v1208;
    void* v1735;
    int64_t v4020; /* placeholder for void */
    int64_t v4332;
    int64_t v1555;
    int64_t v1441; /* placeholder for void */
    int64_t v2626; /* placeholder for void */
    int64_t v2763; /* placeholder for void */
    void* v3773;
    int64_t v3801; /* placeholder for void */
    int64_t v2224;
    int64_t v1792;
    int64_t v3679; /* placeholder for void */
    void* v742;
    void* v4386;
    int64_t v1326; /* placeholder for void */
    void* v3117;
    void* v805;
    void* v896;
    int64_t v651;
    int64_t v3764;
    int64_t v2349;
    int64_t v2956;
    void* v3210;
    void* v1528;
    int64_t v533; /* placeholder for void */
    void* v2528;
    int64_t v3776; /* placeholder for void */
    int64_t v4070; /* placeholder for void */
    void* v4390;
    void* v1423;
    void* v3639;
    void* v109;
    int64_t v2623; /* placeholder for void */
    void* v2245;
    void* v492;
    void* v4018;
    void* v122;
    int64_t v3325; /* placeholder for void */
    int64_t v1620;
    int64_t v924; /* placeholder for void */
    int64_t v3962;
    void* v3985;
    void* v725;
    void* v485;
    void* v2998;
    int64_t v1422; /* placeholder for void */
    void* v3227;
    int64_t v401; /* placeholder for void */
    void* v3464;
    int64_t v1228; /* placeholder for void */
    void* v2593;
    void* v2781;
    int64_t v1351;
    bool v1291;
    int64_t v60; /* placeholder for void */
    int64_t v1512; /* placeholder for void */
    void* v2755;
    int64_t v8;
    void* v1517;
    void* v4367;
    void* v2015;
    void* v4062;
    void* v3635;
    void* v1679;
    void* v1957;
    void* v3262;
    void* v3731;
    int64_t v1605; /* placeholder for void */
    void* v1582;
    int64_t v951; /* placeholder for void */
    void* v2723;
    int64_t v934;
    int64_t v215;
    int64_t v573; /* placeholder for void */
    int64_t v1306; /* placeholder for void */
    void* v1532;
    int64_t v1752; /* placeholder for void */
    int64_t v1083; /* placeholder for void */
    int64_t v2332; /* placeholder for void */
    void* v2788;
    void* v3906;
    int64_t v3833; /* placeholder for void */
    void* v764;
    int64_t v1801; /* placeholder for void */
    int64_t v2706; /* placeholder for void */
    void* v2823;
    bool v3276;
    void* v2874;
    void* v657;
    int64_t v557; /* placeholder for void */
    void* v2190;
    int64_t v2570;
    int64_t v4131; /* placeholder for void */
    void* v1979;
    int64_t v2679; /* placeholder for void */
    void* v3726;
    int64_t v1671;
    int64_t v2291;
    int64_t v1757;
    void* v4026;
    void* v4165;
    void* v4031;
    void* v3331;
    int64_t v681; /* placeholder for void */
    int64_t v3363; /* placeholder for void */
    bool v1493;
    void* v2263;
    void* v1186;
    int64_t v3489;
    int64_t v989;
    void* v2194;
    int64_t v1001; /* placeholder for void */
    int64_t v1297;
    void* v973;
    int64_t v1235;
    void* v2526;
    int64_t v1658;
    void* v3823;
    int64_t v2404; /* placeholder for void */
    int64_t v3150;
    void* v3932;
    void* v911;
    void* v2535;
    void* v3739;
    int64_t v3883;
    int64_t v1077; /* placeholder for void */
    int64_t v4164;
    void* v2210;
    int64_t v2870;
    int64_t v3116; /* placeholder for void */
    int64_t v3862; /* placeholder for void */
    int64_t v3966;
    int64_t v155;
    void* v3702;
    int64_t v1560; /* placeholder for void */
    int64_t v1893; /* placeholder for void */
    void* v2982;
    int64_t v3670; /* placeholder for void */
    int64_t v2842;
    int64_t v1303; /* placeholder for void */
    int64_t v1378;
    int64_t v852;
    void* v2075;
    int64_t v1842;
    int64_t v319; /* placeholder for void */
    int64_t v2174;
    void* v4341;
    int64_t v222; /* placeholder for void */
    void* v3486;
    void* v3654;
    void* v9;
    void* v1580;
    int64_t v2736; /* placeholder for void */
    void* v627;
    void* v525;
    int64_t v1361; /* placeholder for void */
    void* v3671;
    void* v2514;
    int64_t v161;
    void* v3815;
    int64_t v2636;
    void* v3586;
    int64_t v4183;
    int64_t v3355;
    void* v511;
    void* v438;
    int64_t v2807; /* placeholder for void */
    int64_t v1651;
    int64_t v1871; /* placeholder for void */
    int64_t v2371; /* placeholder for void */
    void* v1010;
    int64_t v2933; /* placeholder for void */
    int64_t v3967; /* placeholder for void */
    bool v4218;
    void* v4038;
    void* v290;
    void* v1570;
    int64_t v327; /* placeholder for void */
    bool v4047;
    int64_t v3459; /* placeholder for void */
    void* v3076;
    int64_t v3452; /* placeholder for void */
    void* v950;
    void* v3145;
    int64_t v2592;
    int64_t v2139; /* placeholder for void */
    int64_t v743;
    void* v1677;
    void* v4212;
    int64_t v4258;
    int64_t v4113; /* placeholder for void */
    void* v2410;
    int64_t v1805;
    int64_t v2554;
    int64_t v2650; /* placeholder for void */
    void* v702;
    int64_t v3840; /* placeholder for void */
    int64_t v3444;
    void* v3602;
    void* v2446;
    void* v3062;
    int64_t v2692; /* placeholder for void */
    int64_t v2099; /* placeholder for void */
    void* v3393;
    int64_t v4187; /* placeholder for void */
    int64_t v822; /* placeholder for void */
    void* v2735;
    int64_t v1504; /* placeholder for void */
    void* v1697;
    int64_t v330; /* placeholder for void */
    void* v1977;
    void* v2672;
    void* v3416;
    int64_t v3940; /* placeholder for void */
    int64_t v2493; /* placeholder for void */
    void* v2587;
    void* v3359;
    void* v4396;
    int64_t v3306; /* placeholder for void */
    int64_t v2580;
    void* v4025;
    void* v449;
    int64_t v446;
    void* v162;
    void* v2786;
    void* v1332;
    int64_t v2664; /* placeholder for void */
    void* v3412;
    int64_t v2622; /* placeholder for void */
    void* v3049;
    int64_t v1744;
    int64_t v3142; /* placeholder for void */
    int64_t v1909;
    void* v2185;
    int64_t v826;
    int64_t v1427; /* placeholder for void */
    void* v2955;
    int64_t v3402;
    bool v3676;
    int64_t v51;
    int64_t v1709; /* placeholder for void */
    int64_t v2648; /* placeholder for void */
    int64_t v1600;
    int64_t v3733; /* placeholder for void */
    void* v4300;
    int64_t v3153;
    int64_t v4362;
    int64_t v2740;
    void* v3017;
    int64_t v4371;
    int64_t v3458;
    int64_t v417;
    void* v666;
    void* v2070;
    int64_t v671;
    int64_t v606; /* placeholder for void */
    int64_t v2710; /* placeholder for void */
    void* v3737;
    void* v4046;
    int64_t v4202; /* placeholder for void */
    int64_t v3209;
    int64_t v2431;
    int64_t v2769; /* placeholder for void */
    void* v3860;
    int64_t v1592;
    int64_t v1772; /* placeholder for void */
    int64_t v2393; /* placeholder for void */
    int64_t v2785; /* placeholder for void */
    int64_t v1244;
    int64_t v3187; /* placeholder for void */
    int64_t v306;
    int64_t v1469; /* placeholder for void */
    int64_t v3674;
    void* v3544;
    int64_t v2053; /* placeholder for void */
    void* v4008;
    int64_t v2257; /* placeholder for void */
    void* v389;
    void* v3254;
    int64_t v2860; /* placeholder for void */
    void* v2144;
    void* v429;
    int64_t v1122; /* placeholder for void */
    void* v4338;
    void* v842;
    void* v718;
    void* v1867;
    void* v448;
    void* v997;
    int64_t v2713; /* placeholder for void */
    void* v1719;
    int64_t v3894; /* placeholder for void */
    void* v196;
    int64_t v1603;
    int64_t v1158;
    void* v2272;
    void* v2605;
    void* v1008;
    void* v2663;
    int64_t v3064;
    int64_t v4374; /* placeholder for void */
    void* v3694;
    void* v1481;
    int64_t v838; /* placeholder for void */
    int64_t v1851; /* placeholder for void */
    int64_t v2337;
    int64_t v3423; /* placeholder for void */
    int64_t v368; /* placeholder for void */
    int64_t v1786;
    int64_t v2491; /* placeholder for void */
    int64_t v3481; /* placeholder for void */
    int64_t v2248;
    int64_t v3051; /* placeholder for void */
    int64_t v1962;
    void* v4084;
    int64_t v962;
    void* v1760;
    void* v123;
    int64_t v992; /* placeholder for void */
    void* v1976;
    int64_t v942; /* placeholder for void */
    int64_t v1996; /* placeholder for void */
    void* v3169;
    void* v3700;
    void* v2849;
    void* v1024;
    void* v2937;
    int64_t v1305; /* placeholder for void */
    void* v1561;
    int64_t v277;
    int64_t v3409; /* placeholder for void */
    int64_t v3719;
    void* v2500;
    int64_t v2412;
    int64_t v3789; /* placeholder for void */
    void* v3797;
    void* v3955;
    int64_t v4399; /* placeholder for void */
    int64_t v3291; /* placeholder for void */
    void* v63;
    void* v1593;
    int64_t v1525;
    int64_t v2038;
    int64_t v4306; /* placeholder for void */
    int64_t v3039;
    int64_t v1585;
    int64_t v3352; /* placeholder for void */
    int64_t v1778; /* placeholder for void */
    void* v3431;
    void* v757;
    void* v229;
    int64_t v1664;
    void* v3769;
    int64_t v3378; /* placeholder for void */
    int64_t v1862;
    int64_t v3340; /* placeholder for void */
    int64_t v639; /* placeholder for void */
    void* v189;
    void* v2852;
    int64_t v1362; /* placeholder for void */
    int64_t v182;
    void* v593;
    int64_t v3023;
    void* v1595;
    int64_t v3948;
    int64_t v2770; /* placeholder for void */
    void* v4032;
    void* v2122;
    int64_t v2577;
    int64_t v898; /* placeholder for void */
    void* v3872;
    int64_t v4117; /* placeholder for void */
    void* v859;
    int64_t v2108; /* placeholder for void */
    int64_t v2078; /* placeholder for void */
    void* v2659;
    int64_t v120; /* placeholder for void */
    int64_t v3277; /* placeholder for void */
    void* v362;
    void* v3856;
    void* v1311;
    bool v3693;
    void* v3237;
    void* v2115;
    int64_t v3705; /* placeholder for void */
    int64_t v188;
    void* v3845;
    int64_t v2096; /* placeholder for void */
    int64_t v669; /* placeholder for void */
    int64_t v1145;
    int64_t v4156; /* placeholder for void */
    int64_t v1051;
    int64_t v1753;
    int64_t v3440;
    void* v3576;
    void* v478;
    void* v617;
    int64_t v2572; /* placeholder for void */
    void* v3974;
    int64_t v3152; /* placeholder for void */
    void* v2208;
    void* v4345;
    int64_t v1136; /* placeholder for void */
    void* v953;
    void* v659;
    void* v3688;
    int64_t v3976; /* placeholder for void */
    int64_t v2016; /* placeholder for void */
    int64_t v543; /* placeholder for void */
    int64_t v1991;
    int64_t v2407;
    int64_t v2171;
    int64_t v2760; /* placeholder for void */
    void* v2796;
    int64_t v2836; /* placeholder for void */
    void* v3979;
    void* v4377;
    void* v3218;
    void* v424;
    int64_t v2399;
    int64_t v2994;
    int64_t v3255; /* placeholder for void */
    int64_t v4199;
    void* v235;
    int64_t v383;
    int64_t v4201; /* placeholder for void */
    int64_t v1218; /* placeholder for void */
    void* v1577;
    void* v2314;
    void* v3520;
    int64_t v4059; /* placeholder for void */
    void* v2453;
    int64_t v2950; /* placeholder for void */
    bool v4095;
    void* v3524;
    void* v869;
    void* v103;
    void* v625;
    int64_t v3742; /* placeholder for void */
    int64_t v2025; /* placeholder for void */
    void* v3508;
    void* v3552;
    double v1065;
    void* v3569;
    void* v575;
    int64_t v1713;
    int64_t v2445; /* placeholder for void */
    void* v3934;
    int64_t v357; /* placeholder for void */
    int64_t v2536;
    int64_t v3059;
    int64_t v1579;
    void* v4163;
    void* v913;
    int64_t v1602; /* placeholder for void */
    int64_t v1003;
    void* v129;
    void* v684;
    void* v3766;
    void* v2105;
    int64_t v2829; /* placeholder for void */
    int64_t v2894;
    int64_t v3176;
    void* v3361;
    void* v3157;
    void* v423;
    int64_t v3380; /* placeholder for void */
    void* v812;
    int64_t v1835;
    bool v2436;
    void* v2240;
    int64_t v2701;
    int64_t v2617;
    void* v3141;
    void* v3097;
    int64_t v3405; /* placeholder for void */
    int64_t v2397; /* placeholder for void */
    void* v2746;
    void* v3494;
    int64_t v2455;
    int64_t v2862; /* placeholder for void */
    int64_t v919;
    int64_t v2336; /* placeholder for void */
    void* v4204;
    int64_t v2179; /* placeholder for void */
    void* v3346;
    void* v3998;
    int64_t v274;
    void* v1034;
    void* v2059;
    void* v672;
    int64_t v3324; /* placeholder for void */
    int64_t v3942; /* placeholder for void */
    int64_t v2766; /* placeholder for void */
    int64_t v2118; /* placeholder for void */
    int64_t v404;
    int64_t v75;
    void* v3419;
    void* v3819;
    int64_t v4259;
    int64_t v663; /* placeholder for void */
    void* v1151;
    int64_t v1161; /* placeholder for void */
    void* v3965;
    int64_t v2366; /* placeholder for void */
    void* v4005;
    void* v4309;
    int64_t v3156;
    int64_t v2441; /* placeholder for void */
    int64_t v3198;
    void* v1400;
    void* v4099;
    int64_t v1747;
    void* v3770;
    int64_t v508; /* placeholder for void */
    void* v3780;
    int64_t v3785; /* placeholder for void */
    void* v2511;
    void* v33;
    void* v2359;
    int64_t v2124;
    void* v1550;
    int64_t v1458; /* placeholder for void */
    void* v2793;
    int64_t v1971; /* placeholder for void */
    int64_t v2691; /* placeholder for void */
    int64_t v3108; /* placeholder for void */
    void* v3273;
    void* v4321;
    int64_t v170; /* placeholder for void */
    int64_t v1088; /* placeholder for void */
    int64_t v1308; /* placeholder for void */
    int64_t v4100; /* placeholder for void */
    int64_t v3185; /* placeholder for void */
    void* v779;
    int64_t v3755;
    void* v2551;
    int64_t v586;
    void* v2285;
    int64_t v3192; /* placeholder for void */
    void* v2006;
    void* v3907;
    int64_t v4237;
    void* v3667;
    void* v839;
    void* v2238;
    void* v2722;
    int64_t v1316;
    void* v1672;
    void* v2155;
    bool v3802;
    int64_t v4350; /* placeholder for void */
    int64_t v914; /* placeholder for void */
    int64_t v4235; /* placeholder for void */
    int64_t v2351;
    void* v1564;
    void* v1415;
    int64_t v3516;
    void* v298;
    int64_t v2334;
    int64_t v2772; /* placeholder for void */
    int64_t v1256; /* placeholder for void */
    void* v3844;
    void* v977;
    void* v2182;
    int64_t v2637;
    int64_t v3101;
    int64_t v3308;
    void* v1295;
    int64_t v4000;
    void* v3000;
    void* v3757;
    void* v1609;
    void* v2230;
    void* v1688;
    int64_t v4109;
    void* v1813;
    void* v1451;
    int64_t v1617; /* placeholder for void */
    int64_t v645; /* placeholder for void */
    bool v2408;
    void* v578;
    int64_t v2002;
    void* v544;
    int64_t v715; /* placeholder for void */
    void* v2900;
    void* v3964;
    int64_t v1494; /* placeholder for void */
    int64_t v686;
    void* v3543;
    void* v4322;
    int64_t v2186; /* placeholder for void */
    int64_t v361;
    void* v581;
    int64_t v2917;
    void* v1089;
    bool v4295;
    void* v1401;
    int64_t v2211; /* placeholder for void */
    int64_t v1657;
    void* v3638;
    void* v3818;
    void* v4359;
    void* v169;
    int64_t v1192;
    void* v4180;
    void* v416;
    int64_t v1689;
    void* v248;
    int64_t v264; /* placeholder for void */
    void* v3167;
    void* v721;
    int64_t v1103; /* placeholder for void */
    void* v1418;
    void* v1618;
    int64_t v1707; /* placeholder for void */
    int64_t v2492; /* placeholder for void */
    bool v2633;
    int64_t v828; /* placeholder for void */
    void* v1126;
    void* v1050;
    void* v2097;
    int64_t v3115;
    int64_t v1074; /* placeholder for void */
    int64_t v2246; /* placeholder for void */
    int64_t v3282;
    int64_t v2267;
    int64_t v3598; /* placeholder for void */
    void* v4105;
    void* v2825;
    int64_t v1166; /* placeholder for void */
    int64_t v1952;
    int64_t v642;
    void* v1890;
    int64_t v150; /* placeholder for void */
    int64_t v2774; /* placeholder for void */
    void* v1503;
    int64_t v2588; /* placeholder for void */
    void* v969;
    int64_t v1693;
    void* v3838;
    void* v2176;
    int64_t v197; /* placeholder for void */
    int64_t v48;
    void* v2531;
    void* v3403;
    bool v1053;
    int64_t v3092;
    void* v1430;
    void* v1170;
    void* v489;
    int64_t v3750;
    int64_t v654;
    int64_t v254; /* placeholder for void */
    void* v3701;
    int64_t v846; /* placeholder for void */
    void* v2989;
    void* v2300;
    int64_t v2911;
    int64_t v1924;
    bool v4200;
    void* v3114;
    void* v821;
    int64_t v2287;
    int64_t v3727;
    void* v847;
    void* v2546;
    void* v179;
    void* v4148;
    int64_t v1035;
    int64_t v1978; /* placeholder for void */
    int64_t v4074;
    int64_t v2973; /* placeholder for void */
    int64_t v1054; /* placeholder for void */
    void* v1601;
    void* v3491;
    int64_t v3817;
    int64_t v3050;
    void* v2889;
    int64_t v195;
    int64_t v550; /* placeholder for void */
    int64_t v2835;
    int64_t v3954; /* placeholder for void */
    void* v2858;
    void* v1898;
    int64_t v3796;
    int64_t v377;
    void* v1402;
    void* v856;
    int64_t v1506; /* placeholder for void */
    int64_t v366; /* placeholder for void */
    int64_t v498;
    void* v2915;
    void* v3112;
    void* v1124;
    int64_t v1425; /* placeholder for void */
    void* v2753;
    void* v2884;
    int64_t v783; /* placeholder for void */
    void* v2233;
    void* v3170;
    int64_t v2439; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    void* v2256;
    void* v2880;
    void* v3079;
    int64_t v1147; /* placeholder for void */
    int64_t v184; /* placeholder for void */
    int64_t v3046;
    int64_t v658;
    int64_t v426;
    int64_t v1352;
    int64_t v106; /* placeholder for void */
    bool v1696;
    void* v1769;
    int64_t v3884; /* placeholder for void */
    int64_t v3434;
    int64_t v1221; /* placeholder for void */
    void* v363;
    void* v2590;
    void* v2986;
    int64_t v2843; /* placeholder for void */
    int64_t v3003;
    int64_t v4194;
    void* v3040;
    int64_t v212;
    void* v730;
    int64_t v784;
    int64_t v1590; /* placeholder for void */
    int64_t v4019;
    int64_t v439; /* placeholder for void */
    int64_t v2844;
    int64_t v2277;
    void* v4073;
    int64_t v3280;
    int64_t v1820;
    void* v577;
    void* v1762;
    int64_t v2467; /* placeholder for void */
    int64_t v776; /* placeholder for void */
    bool v2614;
    int64_t v244; /* placeholder for void */
    void* v1100;
    void* v1189;
    void* v2189;
    void* v815;
    int64_t v1806;
    void* v3429;
    int64_t v4112; /* placeholder for void */
    void* v1866;
    int64_t v1879;
    void* v844;
    int64_t v2188; /* placeholder for void */
    int64_t v2749;
    int64_t v1025; /* placeholder for void */
    int64_t v1641;
    void* v3266;
    int64_t v4153;
    int64_t v2708;
    int64_t v1648;
    void* v2583;
    int64_t v4049;
    void* v3745;
    int64_t v3238;
    int64_t v2924; /* placeholder for void */
    int64_t v3220;
    void* v1367;
    void* v507;
    void* v3788;
    void* v3895;
    int64_t v4042;
    int64_t v2791;
    void* v4054;
    void* v1110;
    void* v3314;
    int64_t v4065;
    int64_t v121;
    int64_t v1471; /* placeholder for void */
    int64_t v1281;
    int64_t v1907; /* placeholder for void */
    void* v418;
    int64_t v414;
    void* v3944;
    void* v4308;
    void* v395;
    int64_t v3751; /* placeholder for void */
    int64_t v3949; /* placeholder for void */
    int64_t v2730;
    void* v638;
    int64_t v910; /* placeholder for void */
    int64_t v2046; /* placeholder for void */
    int64_t v2156;
    void* v2184;
    void* v2747;
    void* v3795;
    int64_t v3240; /* placeholder for void */
    void* v420;
    void* v749;
    void* v2981;
    void* v133;
    void* v3067;
    void* v1433;
    int64_t v3286; /* placeholder for void */
    int64_t v3337;
    int64_t v380;
    int64_t v2401; /* placeholder for void */
    int64_t v1480; /* placeholder for void */
    int64_t v2391; /* placeholder for void */
    int64_t v3249; /* placeholder for void */
    void* v1568;
    void* v1273;
    void* v1116;
    void* v1972;
    void* v4277;
    int64_t v2253; /* placeholder for void */
    void* v1009;
    void* v1098;
    void* v1;
    void* v441;
    void* v2029;
    int64_t v665; /* placeholder for void */
    void* v2506;
    void* v154;
    void* v44;
    void* v2013;
    bool v2803;
    int64_t v4001; /* placeholder for void */
    int64_t v545;
    void* v1647;
    int64_t v704;
    void* v3835;
    double v1071;
    int64_t v4281;
    int64_t v214; /* placeholder for void */
    int64_t v865;
    int64_t v3712; /* placeholder for void */
    void* v2850;
    void* v3607;
    int64_t v3246; /* placeholder for void */
    void* v4398;
    int64_t v2896;
    int64_t v1240; /* placeholder for void */
    void* v981;
    int64_t v370; /* placeholder for void */
    void* v1452;
    void* v2496;
    int64_t v1108; /* placeholder for void */
    void* v2578;
    int64_t v2765; /* placeholder for void */
    int64_t v3179;
    void* v4090;
    int64_t v3321;
    bool v1920;
    int64_t v2893;
    void* v1188;
    void* v3501;
    int64_t v3807; /* placeholder for void */
    void* v4044;
    int64_t v3798; /* placeholder for void */
    int64_t v1989;
    void* v3053;
    void* v1714;
    int64_t v1155; /* placeholder for void */
    void* v497;
    int64_t v365;
    int64_t v2465; /* placeholder for void */
    void* v52;
    void* v53;
    void* v2638;
    int64_t v1046; /* placeholder for void */
    int64_t v2166; /* placeholder for void */
    bool v3338;
    int64_t v4145; /* placeholder for void */
    int64_t v1121;
    int64_t v175;
    int64_t v288; /* placeholder for void */
    void* v616;
    int64_t v1510; /* placeholder for void */
    void* v827;
    int64_t v3318; /* placeholder for void */
    void* v2980;
    int64_t v2321;
    void* v3649;
    void* v3760;
    int64_t v3375;
    void* v2113;
    void* v3988;
    int64_t v1109;
    void* v139;
    int64_t v1343; /* placeholder for void */
    void* v1986;
    int64_t v2043; /* placeholder for void */
    void* v2936;
    int64_t v3822; /* placeholder for void */
    void* v3885;
    int64_t v4363;
    int64_t v2487; /* placeholder for void */
    int64_t v101;
    int64_t v30; /* placeholder for void */
    int64_t v4115; /* placeholder for void */
    void* v1262;
    void* v1339;
    int64_t v2668;
    void* v287;
    void* v850;
    int64_t v1655; /* placeholder for void */
    void* v1629;
    int64_t v2997; /* placeholder for void */
    void* v152;
    int64_t v266; /* placeholder for void */
    void* v562;
    void* v3103;
    void* v3113;
    int64_t v171;
    int64_t v3086;
    void* v2312;
    int64_t v1966;
    void* v703;
    int64_t v1916; /* placeholder for void */
    void* v427;
    void* v1007;
    int64_t v851;
    int64_t v820;
    void* v2712;
    bool v358;
    void* v294;
    void* v2794;
    int64_t v3330; /* placeholder for void */
    int64_t v706; /* placeholder for void */
    int64_t v2851;
    void* v312;
    int64_t v2697; /* placeholder for void */
    void* v3534;
    void* v1878;
    void* v3957;
    int64_t v1216; /* placeholder for void */
    int64_t v1202;
    void* v3462;
    int64_t v422; /* placeholder for void */
    int64_t v398;
    void* v3546;
    void* v716;
    void* v3151;
    bool v3924;
    void* v4051;
    void* v466;
    void* v2798;
    int64_t v1338; /* placeholder for void */
    int64_t v3199;
    void* v917;
    void* v1498;
    int64_t v1499;
    void* v2728;
    void* v1141;
    int64_t v926; /* placeholder for void */
    void* v2573;
    void* v2995;
    int64_t v3411; /* placeholder for void */
    int64_t v3653;
    void* v2771;
    void* v4114;
    void* v4342;
    void* v3806;
    void* v3960;
    void* v3566;
    void* v3762;
    void* v2927;
    void* v4289;
    int64_t v3172;
    int64_t v1676;
    void* v2254;
    void* v999;
    int64_t v1356;
    int64_t v10; /* placeholder for void */
    int64_t v1496;
    void* v2136;
    int64_t v2543; /* placeholder for void */
    void* v1681;
    void* v84;
    int64_t v761; /* placeholder for void */
    int64_t v445; /* placeholder for void */
    int64_t v1547; /* placeholder for void */
    int64_t v2782;
    int64_t v1076; /* placeholder for void */
    int64_t v1093;
    int64_t v960; /* placeholder for void */
    void* v1746;
    void* v2904;
    int64_t v3404;
    void* v979;
    void* v405;
    void* v1039;
    int64_t v1018; /* placeholder for void */
    void* v2044;
    void* v3080;
    void* v1301;
    int64_t v1668;
    void* v1892;
    void* v2243;
    int64_t v3032;
    void* v3372;
    int64_t v1733;
    void* v3027;
    void* v3717;
    void* v3857;
    int64_t v908; /* placeholder for void */
    void* v3991;
    void* v2403;
    void* v1097;
    int64_t v3139; /* placeholder for void */
    int64_t v2104;
    int64_t v2103; /* placeholder for void */
    int64_t v2879; /* placeholder for void */
    int64_t v3211; /* placeholder for void */
    int64_t v2214;
    void* v1998;
    int64_t v2428; /* placeholder for void */
    void* v548;
    void* v153;
    bool v4255;
    void* v376;
    void* v785;
    int64_t v1944; /* placeholder for void */
    void* v1138;
    void* v3779;
    void* v3106;
    int64_t v4340;
    void* v2922;
    int64_t v3144; /* placeholder for void */
    void* v1150;
    int64_t v3341; /* placeholder for void */
    bool v2522;
    int64_t v2977; /* placeholder for void */
    int64_t v1670; /* placeholder for void */
    int64_t v2396; /* placeholder for void */
    void* v57;
    int64_t v3056;
    void* v2365;
    void* v3154;
    int64_t v1606;
    int64_t v461;
    void* v1861;
    int64_t v3206; /* placeholder for void */
    int64_t v2634; /* placeholder for void */
    int64_t v1745;
    int64_t v2101;
    int64_t v2261; /* placeholder for void */
    void* v2461;
    void* v2292;
    void* v3134;
    int64_t v3527; /* placeholder for void */
    int64_t v1101;
    int64_t v1389; /* placeholder for void */
    void* v3223;
    int64_t v4288; /* placeholder for void */
    int64_t v3495; /* placeholder for void */
    void* v3214;
    void* v2754;
    void* v558;
    void* v1159;
    int64_t v3019;
    void* v378;
    int64_t v2320; /* placeholder for void */
    int64_t v3336;
    void* v1293;
    int64_t v1534;
    bool v1029;
    int64_t v3595;
    void* v2895;
    void* v4167;
    int64_t v325;
    void* v605;
    int64_t v299;
    int64_t v778;
    int64_t v1336; /* placeholder for void */
    int64_t v529;
    int64_t v1556; /* placeholder for void */
    void* v3385;
    void* v800;
    int64_t v2014; /* placeholder for void */
    int64_t v3449; /* placeholder for void */
    void* v2145;
    int64_t v339;
    int64_t v1364; /* placeholder for void */
    void* v1537;
    int64_t v2741;
    void* v142;
    int64_t v2892;
    void* v3071;
    int64_t v3315;
    int64_t v1459;
    int64_t v3177;
    int64_t v509;
    int64_t v1254;
    int64_t v928;
    void* v2005;
    int64_t v4072; /* placeholder for void */
    int64_t v4264; /* placeholder for void */
    int64_t v2229; /* placeholder for void */
    int64_t v1723; /* placeholder for void */
    int64_t v2000; /* placeholder for void */
    int64_t v4292; /* placeholder for void */
    int64_t v388; /* placeholder for void */
    void* v1749;
    int64_t v1736; /* placeholder for void */
    void* v4241;
    int64_t v4063;
    int64_t v2244; /* placeholder for void */
    int64_t v2249;
    void* v3647;
    int64_t v2170;
    int64_t v771;
    int64_t v848; /* placeholder for void */
    void* v1315;
    void* v1031;
    void* v113;
    int64_t v2654; /* placeholder for void */
    int64_t v3830;
    int64_t v555;
    int64_t v2464; /* placeholder for void */
    void* v4299;
    int64_t v806; /* placeholder for void */
    int64_t v2826;
    void* v324;
    int64_t v2504;
    void* v2153;
    void* v1604;
    int64_t v1387; /* placeholder for void */
    void* v1905;
    void* v1625;
    int64_t v3347;
    int64_t v637; /* placeholder for void */
    int64_t v210; /* placeholder for void */
    void* v2095;
    int64_t v379; /* placeholder for void */
    int64_t v2920; /* placeholder for void */
    void* v504;
    bool v3828;
    int64_t v1476;
    void* v149;
    int64_t v344; /* placeholder for void */
    void* v1796;
    int64_t v490; /* placeholder for void */
    bool v4125;
    int64_t v1464; /* placeholder for void */
    void* v2151;
    int64_t v2469; /* placeholder for void */
    int64_t v3581; /* placeholder for void */
    void* v3619;
    void* v2165;
    void* v1196;
    void* v2960;
    void* v1908;
    int64_t v1551;
    int64_t v2031; /* placeholder for void */
    void* v3873;
    int64_t v2621; /* placeholder for void */
    int64_t v2538;
    int64_t v1492;
    int64_t v1079; /* placeholder for void */
    void* v4208;
    void* v1771;
    void* v4021;
    void* v537;
    int64_t v2255;
    int64_t v2019;
    int64_t v3323;
    void* v4193;
    int64_t v187; /* placeholder for void */
    void* v556;
    int64_t v2512; /* placeholder for void */
    bool v3200;
    void* v3089;
    int64_t v3005;
    int64_t v1553;
    void* v1742;
    void* v3723;
    int64_t v2853; /* placeholder for void */
    void* v3482;
    void* v2274;
    int64_t v3373; /* placeholder for void */
    int64_t v1732; /* placeholder for void */
    void* v1841;
    int64_t v3989;
    int64_t v810; /* placeholder for void */
    int64_t v4324; /* placeholder for void */
    void* v3851;
    int64_t v623;
    int64_t v982;
    void* v2307;
    void* v342;
    int64_t v793; /* placeholder for void */
    int64_t v2462;
    int64_t v1182; /* placeholder for void */
    void* v1870;
    void* v2948;
    int64_t v3305; /* placeholder for void */
    int64_t v2191; /* placeholder for void */
    void* v3597;
    void* v4353;
    int64_t v1112; /* placeholder for void */
    void* v1350;
    int64_t v2175; /* placeholder for void */
    void* v3561;
    int64_t v4356;
    void* v668;
    void* v1652;
    int64_t v2932;
    void* v3128;
    void* v384;
    int64_t v1682;
    int64_t v3475; /* placeholder for void */
    void* v3970;
    int64_t v985;
    int64_t v1527; /* placeholder for void */
    void* v172;
    int64_t v2360; /* placeholder for void */
    int64_t v2132;
    void* v1669;
    void* v2556;
    int64_t v2163; /* placeholder for void */
    void* v1631;
    int64_t v2275; /* placeholder for void */
    int64_t v2734; /* placeholder for void */
    void* v2519;
    void* v272;
    void* v2552;
    int64_t v732;
    int64_t v2341;
    int64_t v2066;
    int64_t v936; /* placeholder for void */
    void* v1881;
    int64_t v1695; /* placeholder for void */
    int64_t v2207;
    void* v2093;
    int64_t v26; /* placeholder for void */
    int64_t v2159;
    void* v3580;
    void* v1526;
    int64_t v1873; /* placeholder for void */
    int64_t v3474;
    void* v1482;
    void* v1640;
    int64_t v3389;
    void* v2149;
    int64_t v4296; /* placeholder for void */
    void* v1832;
    void* v689;
    int64_t v2060; /* placeholder for void */
    int64_t v1704; /* placeholder for void */
    int64_t v2705;
    void* v2040;
    int64_t v2596; /* placeholder for void */
    void* v2811;
    int64_t v2022;
    void* v3212;
    int64_t v3500; /* placeholder for void */
    int64_t v3596; /* placeholder for void */
    int64_t v2392; /* placeholder for void */
    int64_t v1578; /* placeholder for void */
    int64_t v1220; /* placeholder for void */
    int64_t v1328;
    int64_t v1446;
    int64_t v1712;
    int64_t v2041;
    int64_t v1904; /* placeholder for void */
    void* v1974;
    int64_t v2651; /* placeholder for void */
    int64_t v3275;
    void* v1239;
    int64_t v431; /* placeholder for void */
    void* v2814;
    void* v3849;
    void* v2867;
    int64_t v1417;
    int64_t v35;
    int64_t v3125; /* placeholder for void */
    void* v3570;
    void* v328;
    void* v1484;
    void* v4348;
    void* v2010;
    int64_t v801;
    void* v4243;
    int64_t v3935;
    void* v3958;
    void* v1055;
    int64_t v1022; /* placeholder for void */
    int64_t v1227; /* placeholder for void */
    void* v2495;
    void* v3888;
    int64_t v619;
    int64_t v192;
    int64_t v1354; /* placeholder for void */
    void* v2247;
    int64_t v3604;
    int64_t v1269; /* placeholder for void */
    void* v3842;
    int64_t v3162;
    int64_t v482; /* placeholder for void */
    int64_t v823;
    int64_t v3191; /* placeholder for void */
    int64_t v2183; /* placeholder for void */
    void* v3774;
    void* v4319;
    int64_t v2012;
    void* v3652;
    double v1061;
    void* v1797;
    void* v4213;
    int64_t v407;
    void* v2003;
    void* v3259;
    int64_t v2861;
    int64_t v974;
    int64_t v2076;
    int64_t v754;
    void* v2967;
    int64_t v2505; /* placeholder for void */
    bool v1917;
    int64_t v894;
    int64_t v1169; /* placeholder for void */
    int64_t v2164;
    void* v1313;
    int64_t v3549; /* placeholder for void */
    int64_t v2779; /* placeholder for void */
    int64_t v469; /* placeholder for void */
    void* v1894;
    int64_t v1156;
    void* v2021;
    int64_t v1963;
    void* v2355;
    void* v1369;
    bool v1542;
    int64_t v1730; /* placeholder for void */
    int64_t v3917;
    void* v1791;
    void* v3933;
    int64_t v1104; /* placeholder for void */
    int64_t v4366; /* placeholder for void */
    void* v1257;
    int64_t v2452; /* placeholder for void */
    void* v1483;
    int64_t v2970;
    void* v2657;
    void* v3601;
    void* v1252;
    int64_t v270;
    int64_t v1687; /* placeholder for void */
    int64_t v2177;
    void* v2525;
    void* v3691;
    int64_t v4268; /* placeholder for void */
    void* v4279;
    void* v1728;
    int64_t v1783; /* placeholder for void */
    void* v3521;
    int64_t v2594; /* placeholder for void */
    int64_t v1505; /* placeholder for void */
    int64_t v1787;
    int64_t v108;
    void* v2400;
    int64_t v3661; /* placeholder for void */
    void* v1607;
    void* v2907;
    int64_t v1809;
    void* v226;
    void* v1708;
    int64_t v1052;
    int64_t v1292; /* placeholder for void */
    int64_t v1608; /* placeholder for void */
    void* v1181;
    void* v1852;
    int64_t v3148; /* placeholder for void */
    void* v3391;
    void* v3608;
    void* v3611;
    int64_t v2313; /* placeholder for void */
    void* v4055;
    int64_t v4395; /* placeholder for void */
    void* v1599;
    void* v243;
    void* v487;
    int64_t v1507; /* placeholder for void */
    int64_t v2074; /* placeholder for void */
    void* v1363;
    int64_t v2589;
    void* v1414;
    void* v2557;
    void* v3239;
    void* v932;
    int64_t v2812;
    void* v1428;
    void* v2398;
    void* v923;
    void* v1236;
    void* v2914;
    void* v523;
    int64_t v3267;
    int64_t v3497;
    void* v1846;
    void* v1959;
    int64_t v3813;
    void* v947;
    int64_t v3677; /* placeholder for void */
    int64_t v748; /* placeholder for void */
    int64_t v2004; /* placeholder for void */
    void* v4182;
    int64_t v1646; /* placeholder for void */
    void* v246;
    int64_t v3047;
    int64_t v1739; /* placeholder for void */
    void* v3437;
    int64_t v1544; /* placeholder for void */
    void* v3322;
    int64_t v2126; /* placeholder for void */
    void* v907;
    int64_t v1466;
    int64_t v1885;
    void* v2891;
    int64_t v2356; /* placeholder for void */
    void* v723;
    void* v238;
    void* v3627;
    int64_t v1935; /* placeholder for void */
    bool v2954;
    int64_t v1036; /* placeholder for void */
    int64_t v462; /* placeholder for void */
    int64_t v878;
    void* v2133;
    void* v795;
    int64_t v4247;
    void* v3559;
    int64_t v3875; /* placeholder for void */
    int64_t v2729; /* placeholder for void */
    int64_t v3903; /* placeholder for void */
    int64_t v1741; /* placeholder for void */
    int64_t v1210; /* placeholder for void */
    int64_t v1014; /* placeholder for void */
    void* v887;
    bool v3244;
    void* v3626;
    void* v549;
    int64_t v3368; /* placeholder for void */
    int64_t v1491;
    int64_t v4209;
    int64_t v2302;
    int64_t v3155; /* placeholder for void */
    int64_t v566;
    void* v583;
    int64_t v4082; /* placeholder for void */
    void* v2609;
    int64_t v2354;
    int64_t v2768; /* placeholder for void */
    int64_t v3421;
    void* v2472;
    void* v4236;
    int64_t v2471;
    void* v3841;
    int64_t v468;
    int64_t v3555;
    int64_t v3683; /* placeholder for void */
    int64_t v1531;
    int64_t v797;
    void* v939;
    void* v191;
    int64_t v3075;
    void* v1740;
    int64_t v2362;
    void* v1180;
    void* v1663;
    void* v4275;
    void* v3307;
    void* v402;
    int64_t v4326; /* placeholder for void */
    int64_t v3922; /* placeholder for void */
    void* v1465;
    int64_t v3002;
    void* v2530;
    int64_t v1360; /* placeholder for void */
    void* v2198;
    void* v4232;
    void* v2516;
    void* v1777;
    int64_t v4323;
    void* v2819;
    int64_t v3382; /* placeholder for void */
    int64_t v352; /* placeholder for void */
    void* v677;
    int64_t v2091;
    void* v367;
    int64_t v4231; /* placeholder for void */
    int64_t v972; /* placeholder for void */
    void* v474;
    void* v3342;
    void* v2045;
    void* v2611;
    void* v447;
    int64_t v2449; /* placeholder for void */
    int64_t v165;
    int64_t v1612; /* placeholder for void */
    void* v1815;
    int64_t v2411;
    int64_t v2733;
    void* v4307;
    int64_t v4269; /* placeholder for void */
    int64_t v2375;
    int64_t v2327;
    int64_t v2154; /* placeholder for void */
    int64_t v2201; /* placeholder for void */
    int64_t v2343; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v2957; /* placeholder for void */
    void* v3886;
    void* v3916;
    int64_t v3630; /* placeholder for void */
    int64_t v765;
    void* v3241;
    bool v4155;
    void* v4347;
    int64_t v3877; /* placeholder for void */
    bool v2480;
    void* v276;
    int64_t v2486;
    int64_t v2640; /* placeholder for void */
    int64_t v4029; /* placeholder for void */
    void* v1821;
    void* v3253;
    void* v886;
    int64_t v3133; /* placeholder for void */
    bool v2953;
    void* v1270;
    int64_t v825; /* placeholder for void */
    int64_t v262;
    int64_t v1779;
    int64_t v2457; /* placeholder for void */
    int64_t v3371;
    int64_t v1538; /* placeholder for void */
    int64_t v3526;
    void* v2820;
    int64_t v788;
    void* v766;
    int64_t v1385; /* placeholder for void */
    int64_t v1902;
    int64_t v2039; /* placeholder for void */
    void* v2560;
    int64_t v4096; /* placeholder for void */
    void* v993;
    void* v675;
    void* v680;
    int64_t v2868; /* placeholder for void */
    void* v345;
    int64_t v3695;
    int64_t v2018;
    void* v3492;
    int64_t v534;
    int64_t v31;
    int64_t v2984;
    int64_t v2325; /* placeholder for void */
    void* v3335;
    int64_t v1637; /* placeholder for void */
    int64_t v80; /* placeholder for void */
    void* v3528;
    int64_t v3617;
    int64_t v91;
    void* v293;
    int64_t v601;
    int64_t v4179;
    int64_t v3242;
    int64_t v922;
    int64_t v2306; /* placeholder for void */
    void* v12;
    int64_t v3578;
    void* v3488;
    void* v945;
    void* v2524;
    int64_t v283; /* placeholder for void */
    void* v3582;
    void* v1847;
    void* v2628;
    int64_t v4196; /* placeholder for void */
    void* v2068;
    int64_t v1865;
    void* v3469;
    void* v2167;
    void* v3986;
    void* v2816;
    void* v4027;
    double v1067;
    void* v3149;
    void* v2054;
    void* v1211;
    int64_t v136; /* placeholder for void */
    int64_t v2983;
    int64_t v2388;
    int64_t v3778; /* placeholder for void */
    int64_t v1237; /* placeholder for void */
    int64_t v4370;
    int64_t v1666; /* placeholder for void */
    void* v3509;
    int64_t v4158; /* placeholder for void */
    void* v3303;
    int64_t v770; /* placeholder for void */
    void* v1002;
    void* v740;
    void* v2017;
    void* v2869;
    void* v3058;
    void* v2326;
    int64_t v4132; /* placeholder for void */
    void* v280;
    int64_t v2562; /* placeholder for void */
    int64_t v2873; /* placeholder for void */
    void* v1286;
    int64_t v2806;
    int64_t v4171; /* placeholder for void */
    int64_t v4239; /* placeholder for void */
    int64_t v2206; /* placeholder for void */
    int64_t v944;
    int64_t v4400;
    int64_t v4203; /* placeholder for void */
    void* v268;
    void* v3438;
    int64_t v3724; /* placeholder for void */
    int64_t v2567; /* placeholder for void */
    bool v1073;
    void* v73;
    void* v505;
    int64_t v2023; /* placeholder for void */
    int64_t v517; /* placeholder for void */
    int64_t v126; /* placeholder for void */
    bool v4391;
    void* v2597;
    int64_t v2660; /* placeholder for void */
    int64_t v707;
    void* v1627;
    int64_t v3641;
    int64_t v1718;
    void* v2482;
    void* v1123;
    void* v1275;
    int64_t v1511; /* placeholder for void */
    void* v2052;
    int64_t v753; /* placeholder for void */
    void* v4034;
    int64_t v3194;
    void* v4383;
    int64_t v3825;
    int64_t v789; /* placeholder for void */
    void* v864;
    void* v1515;
    int64_t v552;
    int64_t v4146; /* placeholder for void */
    void* v164;
    int64_t v2143; /* placeholder for void */
    void* v434;
    int64_t v900; /* placeholder for void */
    int64_t v1964;
    void* v2990;
    int64_t v814;
    void* v3567;
    int64_t v4161; /* placeholder for void */
    void* v255;
    int64_t v2971;
    void* v1584;
    void* v536;
    void* v2065;
    int64_t v2993;
    int64_t v775;
    int64_t v1610; /* placeholder for void */
    void* v72;
    void* v334;
    void* v2178;
    bool v853;
    int64_t v2473; /* placeholder for void */
    int64_t v1162; /* placeholder for void */
    void* v3123;
    int64_t v3181; /* placeholder for void */
    void* v1489;
    void* v3897;
    void* v4077;
    int64_t v1518;
    void* v603;
    void* v3718;
    void* v2784;
    void* v729;
    int64_t v310; /* placeholder for void */
    void* v4360;
    int64_t v1373; /* placeholder for void */
    void* v4150;
    void* v1374;
    void* v1726;
    void* v1834;
    int64_t v4085; /* placeholder for void */
    void* v1751;
    void* v2630;
    int64_t v728;
    void* v2908;
    int64_t v2680; /* placeholder for void */
    int64_t v1529; /* placeholder for void */
    void* v1440;
    int64_t v3535; /* placeholder for void */
    void* v1949;
    void* v2667;
    int64_t v2696; /* placeholder for void */
    void* v4057;
    void* v400;
    int64_t v2717; /* placeholder for void */
    void* v3575;
    void* v372;
    void* v2761;
    void* v512;
    void* v3063;
    int64_t v880; /* placeholder for void */
    int64_t v1811;
    int64_t v1011;
    void* v1043;
    void* v874;
    int64_t v1626;
    int64_t v3953;
    void* v2169;
    int64_t v526;
    void* v1296;
    void* v2877;
    int64_t v3655; /* placeholder for void */
    void* v194;
    double v1047;
    void* v2802;
    void* v1287;
    int64_t v1125;
    void* v1376;
    int64_t v2639;
    int64_t v2518; /* placeholder for void */
    void* v3479;
    void* v3013;
    int64_t v4315;
    int64_t v1837;
    int64_t v2417; /* placeholder for void */
    void* v3928;
    void* v2670;
    int64_t v2764; /* placeholder for void */
    int64_t v2521;
    void* v24;
    int64_t v1426;
    void* v4328;
    void* v2427;
    int64_t v4078;
    void* v1137;
    void* v216;
    int64_t v931;
    int64_t v1803; /* placeholder for void */
    int64_t v2919; /* placeholder for void */
    void* v4178;
    int64_t v3066;
    void* v1918;
    void* v2276;
    void* v1838;
    int64_t v3274;
    void* v1197;
    int64_t v1195; /* placeholder for void */
    void* v4336;
    void* v857;
    int64_t v3532;
    void* v2459;
    int64_t v2576; /* placeholder for void */
    int64_t v528; /* placeholder for void */
    void* v614;
    int64_t v3645;
    int64_t v2421; /* placeholder for void */
    int64_t v3428; /* placeholder for void */
    int64_t v3219;
    int64_t v3533; /* placeholder for void */
    int64_t v1411; /* placeholder for void */
    void* v2385;
    int64_t v2618;
    void* v3793;
    void* v1184;
    int64_t v4270; /* placeholder for void */
    int64_t v232; /* placeholder for void */
    void* v2448;
    int64_t v396;
    int64_t v632;
    void* v3098;
    int64_t v3065;
    void* v2709;
    void* v925;
    void* v1358;
    int64_t v2684; /* placeholder for void */
    int64_t v4261;
    void* v3496;
    bool v1372;
    int64_t v2195; /* placeholder for void */
    void* v975;
    void* v3503;
    int64_t v3747;
    void* v102;
    int64_t v3909;
    void* v27;
    void* v22;
    int64_t v125;
    void* v535;
    void* v127;
    int64_t v840; /* placeholder for void */
    void* v1942;
    int64_t v4116; /* placeholder for void */
    int64_t v791;
    void* v4103;
    int64_t v348; /* placeholder for void */
    int64_t v560;
    int64_t v579; /* placeholder for void */
    int64_t v2564; /* placeholder for void */
    void* v4251;
    int64_t v3637; /* placeholder for void */
    int64_t v1807;
    void* v4195;
    void* v199;
    int64_t v3186; /* placeholder for void */
    int64_t v717;
    int64_t v2727; /* placeholder for void */
    void* v3950;
    int64_t v4102; /* placeholder for void */
    bool v4291;
    int64_t v1175; /* placeholder for void */
    bool v1033;
    int64_t v2476; /* placeholder for void */
    int64_t v2545;
    void* v3224;
    void* v3591;
    void* v1129;
    int64_t v1822; /* placeholder for void */
    int64_t v2961; /* placeholder for void */
    int64_t v1300; /* placeholder for void */
    int64_t v2783; /* placeholder for void */
    int64_t v759; /* placeholder for void */
    int64_t v4246;
    void* v1205;
    void* v4354;
    int64_t v1844; /* placeholder for void */
    void* v2129;
    void* v3657;
    void* v302;
    bool v2603;
    void* v1724;
    int64_t v1289;
    int64_t v1408; /* placeholder for void */
    int64_t v1586;
    int64_t v1831; /* placeholder for void */
    bool v2690;
    void* v3644;
    void* v2704;
    int64_t v916;
    int64_t v4003; /* placeholder for void */
    int64_t v2822; /* placeholder for void */
    int64_t v3310; /* placeholder for void */
    int64_t v1434; /* placeholder for void */
    int64_t v3311;
    void* v4290;
    int64_t v2384;
    void* v2458;
    int64_t v863;
    int64_t v2278;
    int64_t v1911; /* placeholder for void */
    int64_t v990; /* placeholder for void */
    void* v2288;
    void* v457;
    void* v794;
    void* v967;
    int64_t v2220;
    void* v309;
    void* v3615;
    int64_t v3765; /* placeholder for void */
    int64_t v2689;
    int64_t v964; /* placeholder for void */
    void* v3283;
    void* v1455;
    bool v891;
    int64_t v2537; /* placeholder for void */
    int64_t v1298; /* placeholder for void */
    void* v2585;
    int64_t v4081; /* placeholder for void */
    int64_t v2503; /* placeholder for void */
    int64_t v4128; /* placeholder for void */
    void* v3977;
    void* v1421;
    void* v2251;
    int64_t v2963;
    int64_t v38;
    void* v2142;
    int64_t v166; /* placeholder for void */
    int64_t v1691; /* placeholder for void */
    void* v1906;
    int64_t v871;
    int64_t v1365; /* placeholder for void */
    int64_t v1613;
    int64_t v3629;
    void* v4294;
    int64_t v3650; /* placeholder for void */
    void* v1863;
    void* v747;
    double v1072;
    int64_t v3506;
    void* v2559;
    int64_t v3969; /* placeholder for void */
    int64_t v1880; /* placeholder for void */
    int64_t v2295;
    int64_t v66; /* placeholder for void */
    void* v394;
    void* v2353;
    void* v2470;
    void* v553;
    int64_t v3956; /* placeholder for void */
    int64_t v1096; /* placeholder for void */
    void* v2818;
    int64_t v1403;
    int64_t v493; /* placeholder for void */
    int64_t v4191; /* placeholder for void */
    int64_t v2297; /* placeholder for void */
    void* v2991;
    void* v1199;
    int64_t v811;
    void* v1409;
    void* v1174;
    void* v2387;
    int64_t v3345; /* placeholder for void */
    int64_t v687; /* placeholder for void */
    void* v37;
    int64_t v2613;
    void* v3686;
    void* v3697;
    int64_t v4229; /* placeholder for void */
    int64_t v3297;
    int64_t v4227; /* placeholder for void */
    int64_t v3565; /* placeholder for void */
    int64_t v1395; /* placeholder for void */
    void* v1377;
    void* v1178;
    int64_t v2627; /* placeholder for void */
    int64_t v2226; /* placeholder for void */
    void* v1738;
    void* v1654;
    void* v798;
    int64_t v2921; /* placeholder for void */
    void* v3463;
    void* v1204;
    void* v1819;
    int64_t v3131;
    void* v1816;
    void* v3004;
    bool v1353;
    int64_t v1128;
    void* v991;
    void* v952;
    int64_t v477;
    int64_t v1322;
    int64_t v1725;
    bool v3137;
    int64_t v3383; /* placeholder for void */
    int64_t v168;
    void* v3478;
    void* v3880;
    int64_t v604;
    int64_t v3996;
    void* v119;
    void* v421;
    int64_t v2011; /* placeholder for void */
    void* v1394;
    int64_t v1473; /* placeholder for void */
    void* v3593;
    int64_t v3975;
    void* v4282;
    int64_t v68;
    int64_t v895;
    int64_t v98;
    int64_t v1720; /* placeholder for void */
    int64_t v3104;
    int64_t v1439;
    void* v2938;
    int64_t v3696; /* placeholder for void */
    void* v2203;
    int64_t v286; /* placeholder for void */
    void* v3881;
    int64_t v473; /* placeholder for void */
    int64_t v724; /* placeholder for void */
    void* v756;
    int64_t v1106; /* placeholder for void */
    int64_t v3362;
    bool v1539;
    int64_t v3304; /* placeholder for void */
    int64_t v3839;
    int64_t v3329; /* placeholder for void */
    void* v2265;
    void* v1660;
    void* v2358;
    int64_t v3507; /* placeholder for void */
    int64_t v1882;
    bool v1209;
    int64_t v1302; /* placeholder for void */
    void* v2698;
    int64_t v1069; /* placeholder for void */
    void* v382;
    void* v2051;
    void* v3427;
    void* v3456;
    void* v3573;
    int64_t v303;
    int64_t v118;
    bool v2972;
    void* v3999;
    void* v4252;
    int64_t v1767;
    int64_t v4214; /* placeholder for void */
    void* v1692;
    void* v760;
    void* v868;
    void* v3536;
    int64_t v1567; /* placeholder for void */
    int64_t v2092; /* placeholder for void */
    int64_t v2242;
    void* v183;
    int64_t v1359;
    int64_t v2264; /* placeholder for void */
    void* v3183;
    void* v1416;
    int64_t v2034;
    int64_t v1933;
    int64_t v1386; /* placeholder for void */
    void* v3465;
    void* v167;
    void* v2160;
    int64_t v1391; /* placeholder for void */
    void* v3009;
    int64_t v1248;
    int64_t v86; /* placeholder for void */
    void* v1800;
    int64_t v4219; /* placeholder for void */
    int64_t v1678; /* placeholder for void */
    void* v1081;
    void* v2608;
    void* v1200;
    void* v3226;
    int64_t v3231; /* placeholder for void */
    bool v1023;
    int64_t v3252;
    void* v3296;
    void* v4287;
    int64_t v1190; /* placeholder for void */
    int64_t v612; /* placeholder for void */
    int64_t v980; /* placeholder for void */
    int64_t v1366; /* placeholder for void */
    int64_t v710;
    int64_t v2062;
    int64_t v2996;
    void* v4013;
    int64_t v1467;
    void* v218;
    int64_t v2406;
    void* v231;
    void* v2361;
    void* v132;
    int64_t v1384; /* placeholder for void */
    int64_t v3243;
    int64_t v1461;
    int64_t v256;
    int64_t v3920; /* placeholder for void */
    int64_t v2978; /* placeholder for void */
    void* v3808;
    void* v1113;
    int64_t v3609; /* placeholder for void */
    int64_t v1937; /* placeholder for void */
    int64_t v1224; /* placeholder for void */
    int64_t v1958; /* placeholder for void */
    void* v2700;
    void* v1521;
    int64_t v3024; /* placeholder for void */
    int64_t v3284;
    int64_t v2347;
    void* v2162;
    void* v4333;
    void* v3;
    int64_t v1016; /* placeholder for void */
    int64_t v2161; /* placeholder for void */
    int64_t v2352; /* placeholder for void */
    int64_t v2415; /* placeholder for void */
    void* v23;
    void* v1075;
    void* v3547;
    int64_t v626;
    void* v390;
    int64_t v667;
    int64_t v3082;
    int64_t v2266; /* placeholder for void */
    void* v300;
    void* v3159;
    void* v3754;
    void* v915;
    void* v1347;
    int64_t v3861;
    int64_t v3741;
    int64_t v2612;
    int64_t v2540;
    int64_t v4365;
    int64_t v3871;
    void* v2737;
    void* v79;
    void* v1930;
    int64_t v1748; /* placeholder for void */
    void* v3374;
    int64_t v105;
    int64_t v2294; /* placeholder for void */
    int64_t v691; /* placeholder for void */
    int64_t v836;
    int64_t v1264; /* placeholder for void */
    int64_t v2620;
    void* v316;
    void* v2111;
    void* v2929;
    void* v3018;
    void* v3398;
    int64_t v1203; /* placeholder for void */
    int64_t v3541;
    void* v3600;
    void* v4346;
    int64_t v3202; /* placeholder for void */
    int64_t v4138;
    void* v735;
    void* v4015;
    int64_t v3445;
    void* v633;
    int64_t v3893;
    int64_t v3728; /* placeholder for void */
    int64_t v219; /* placeholder for void */
    void* v690;
    int64_t v1552;
    void* v3846;
    int64_t v1833; /* placeholder for void */
    int64_t v3316;
    int64_t v3515; /* placeholder for void */
    void* v4045;
    void* v971;
    void* v920;
    int64_t v1380; /* placeholder for void */
    void* v3470;
    void* v1923;
    int64_t v3300; /* placeholder for void */
    int64_t v297; /* placeholder for void */
    void* v3743;
    int64_t v486; /* placeholder for void */
    void* v629;
    void* v3945;
    void* v117;
    int64_t v4173; /* placeholder for void */
    void* v2863;
    int64_t v3140; /* placeholder for void */
    int64_t v3442; /* placeholder for void */
    int64_t v1929;
    void* v2466;
    void* v3081;
    int64_t v3685; /* placeholder for void */
    void* v3892;
    int64_t v1445;
    int64_t v1565;
    void* v2280;
    int64_t v239; /* placeholder for void */
    int64_t v3553;
    int64_t v1848; /* placeholder for void */
    int64_t v591; /* placeholder for void */
    int64_t v1987;
    int64_t v3379; /* placeholder for void */
    int64_t v3558; /* placeholder for void */
    int64_t v1981;
    void* v190;
    void* v4159;
    int64_t v1030; /* placeholder for void */
    void* v2216;
    void* v3233;
    void* v3550;
    void* v1559;
    void* v202;
    int64_t v1659;
    void* v3551;
    void* v317;
    int64_t v3127; /* placeholder for void */
    int64_t v1486;
    void* v97;
    void* v845;
    int64_t v2027; /* placeholder for void */
    bool v2456;
    void* v3734;
    void* v2024;
    int64_t v1615; /* placeholder for void */
    void* v2322;
    int64_t v1877;
    int64_t v1177;
    int64_t v292;
    int64_t v1086;
    int64_t v1357;
    void* v3370;
    int64_t v978; /* placeholder for void */
    void* v755;
    void* v1019;
    int64_t v1012; /* placeholder for void */
    int64_t v2273; /* placeholder for void */
    int64_t v3376;
    int64_t v2213;
    void* v2324;
    int64_t v2582; /* placeholder for void */
    int64_t v737; /* placeholder for void */
    int64_t v1371;
    void* v2773;
    int64_t v2454;
    int64_t v2131;
    int64_t v1020;
    void* v1092;
    void* v2033;
    void* v3668;
    int64_t v205;
    void* v2797;
    void* v4222;
    void* v1883;
    void* v3803;
    void* v2460;
    int64_t v4083; /* placeholder for void */
    bool v2876;
    int64_t v419; /* placeholder for void */
    int64_t v1999;
    void* v4355;
    void* v54;
    void* v963;
    void* v3790;
    int64_t v4157; /* placeholder for void */
    int64_t v904; /* placeholder for void */
    void* v2498;
    void* v1775;
    int64_t v3663; /* placeholder for void */
    void* v4320;
    int64_t v3746;
    void* v4071;
    int64_t v3364; /* placeholder for void */
    void* v1953;
    void* v3099;
    int64_t v1988;
    int64_t v3222; /* placeholder for void */
    int64_t v3105; /* placeholder for void */
    int64_t v3669;
    void* v2221;
    int64_t v237;
    void* v2731;
    void* v3603;
    int64_t v2007; /* placeholder for void */
    void* v412;
    void* v1784;
    void* v1230;
    int64_t v41;
    int64_t v1037; /* placeholder for void */
    void* v2649;
    void* v4389;
    void* v1059;
    void* v943;
    void* v4228;
    void* v2205;
    int64_t v1589;
    int64_t v2329;
    int64_t v4388; /* placeholder for void */
    int64_t v1217; /* placeholder for void */
    int64_t v3397; /* placeholder for void */
    void* v2833;
    void* v3614;
    int64_t v3636;
    void* v2442;
    int64_t v1312;
    void* v2775;
    int64_t v1041; /* placeholder for void */
    int64_t v176; /* placeholder for void */
    int64_t v1941;
    int64_t v3771;
    void* v4298;
    void* v2342;
    bool v3850;
    bool v311;
    int64_t v2923; /* placeholder for void */
    int64_t v4244; /* placeholder for void */
    void* v3767;
    int64_t v201;
    int64_t v1131;
    void* v209;
    void* v410;
    void* v1272;
    void* v1456;
    int64_t v3621; /* placeholder for void */
    int64_t v4240;
    int64_t v796; /* placeholder for void */
    int64_t v1541; /* placeholder for void */
    int64_t v881; /* placeholder for void */
    int64_t v3572; /* placeholder for void */
    int64_t v261; /* placeholder for void */
    int64_t v3853; /* placeholder for void */
    void* v1214;
    int64_t v3247; /* placeholder for void */
    int64_t v3367; /* placeholder for void */
    int64_t v3980;
    int64_t v4334;
    void* v607;
    void* v4373;
    int64_t v2081;
    int64_t v1299; /* placeholder for void */
    void* v3530;
    void* v3887;
    void* v4189;
    int64_t v2152; /* placeholder for void */
    int64_t v1743;
    void* v1780;
    void* v2549;
    int64_t v3590; /* placeholder for void */
    int64_t v128;
    void* v1149;
    int64_t v1828; /* placeholder for void */
    void* v2077;
    int64_t v1928; /* placeholder for void */
    void* v3890;
    void* v1271;
    void* v712;
    void* v1337;
    int64_t v2316; /* placeholder for void */
    int64_t v100; /* placeholder for void */
    int64_t v151;
    int64_t v875;
    int64_t v1662; /* placeholder for void */
    void* v2109;
    int64_t v3388; /* placeholder for void */
    int64_t v1946;
    void* v89;
    void* v1729;
    int64_t v3400;
    int64_t v1932; /* placeholder for void */
    int64_t v95;
    bool v3377;
    void* v4234;
    void* v2383;
    int64_t v4010; /* placeholder for void */
    void* v4006;
    int64_t v2631;
    int64_t v1818; /* placeholder for void */
    int64_t v2; /* placeholder for void */
    void* v4039;
    int64_t v4387;
    void* v2909;
    int64_t v2762; /* placeholder for void */
    void* v295;
    void* v2026;
    int64_t v70; /* placeholder for void */
    int64_t v4154;
    int64_t v3349; /* placeholder for void */
    void* v87;
    void* v3800;
    void* v93;
    void* v3738;
    void* v809;
    void* v177;
    void* v1485;
    int64_t v2130;
    void* v2575;
    void* v3930;
    void* v546;
    int64_t v1684;
    int64_t v1280;
    int64_t v1919; /* placeholder for void */
    void* v2490;
    int64_t v2647; /* placeholder for void */
    void* v2513;
    void* v2140;
    void* v2898;
    void* v1407;
    int64_t v2112; /* placeholder for void */
    void* v835;
    void* v59;
    void* v351;
    void* v2086;
    void* v2550;
    int64_t v2859;
    int64_t v3643;
    int64_t v998; /* placeholder for void */
    int64_t v2725;
    void* v2429;
    void* v1934;
    void* v1021;
    void* v3178;
    int64_t v3579; /* placeholder for void */
    int64_t v3121;
    void* v4152;
    void* v1449;
    void* v3662;
    void* v2258;
    void* v2808;
    int64_t v4303;
    int64_t v946; /* placeholder for void */
    int64_t v1314; /* placeholder for void */
    int64_t v3658; /* placeholder for void */
    int64_t v476; /* placeholder for void */
    int64_t v1448; /* placeholder for void */
    void* v1572;
    bool v3299;
    void* v1164;
    int64_t v3006; /* placeholder for void */
    void* v949;
    bool v2780;
    void* v1925;
    int64_t v2430;
    int64_t v3234; /* placeholder for void */
    void* v3540;
    int64_t v4217;
    void* v2072;
    int64_t v65;
    int64_t v4111; /* placeholder for void */
    void* v282;
    int64_t v2420; /* placeholder for void */
    int64_t v1042; /* placeholder for void */
    int64_t v3014;
    void* v3471;
    int64_t v2301;
    void* v3044;
    int64_t v343;
    void* v3744;
    void* v762;
    void* v2100;
    int64_t v3265; /* placeholder for void */
    void* v2157;
    int64_t v4313;
    void* v610;
    int64_t v1849;
    void* v2394;
    void* v2309;
    int64_t v3290; /* placeholder for void */
    void* v803;
    void* v3992;
    bool v4392;
    int64_t v2128; /* placeholder for void */
    int64_t v2494;
    int64_t v2561;
    void* v2947;
    bool v3221;
    int64_t v636;
    int64_t v200; /* placeholder for void */
    int64_t v1390; /* placeholder for void */
    int64_t v909; /* placeholder for void */
    int64_t v3334; /* placeholder for void */
    int64_t v2777;
    bool v1146;
    void* v3518;
    int64_t v4127; /* placeholder for void */
    void* v4107;
    void* v1277;
    void* v3408;
    void* v646;
    void* v3837;
    void* v428;
    void* v1243;
    int64_t v808;
    int64_t v3435; /* placeholder for void */
    void* v3816;
    int64_t v510; /* placeholder for void */
    void* v2846;
    int64_t v4224;
    void* v3054;
    int64_t v3548;
    void* v3684;
    bool v1447;
    int64_t v2106; /* placeholder for void */
    int64_t v2463; /* placeholder for void */
    int64_t v3369; /* placeholder for void */
    int64_t v442; /* placeholder for void */
    void* v285;
    int64_t v1424;
    void* v3433;
    void* v3938;
    int64_t v1070; /* placeholder for void */
    int64_t v2444; /* placeholder for void */
    int64_t v2683; /* placeholder for void */
    int64_t v3468; /* placeholder for void */
    int64_t v460; /* placeholder for void */
    int64_t v4397; /* placeholder for void */
    void* v1710;
    void* v159;
    int64_t v956; /* placeholder for void */
    int64_t v1825; /* placeholder for void */
    void* v4120;
    int64_t v494;
    int64_t v3620;
    void* v773;
    int64_t v2447;
    void* v574;
    void* v2686;
    void* v4220;
    int64_t v2688;
    void* v14;
    int64_t v3357; /* placeholder for void */
    void* v837;
    void* v841;
    void* v1355;
    int64_t v3354;
    void* v3858;
    void* v443;
    void* v2767;
    void* v2848;
    void* v2958;
    int64_t v1060; /* placeholder for void */
    int64_t v4361;
    bool v2839;
    int64_t v1383; /* placeholder for void */
    void* v1548;
    bool v2840;
    void* v3633;
    int64_t v1810; /* placeholder for void */
    int64_t v1716; /* placeholder for void */
    void* v1823;
    int64_t v2252;
    int64_t v4124;
    void* v1245;
    int64_t v2658;
    int64_t v2676;
    void* v3504;
    int64_t v4305;
    void* v260;
    int64_t v655; /* placeholder for void */
    void* v2477;
    int64_t v4176; /* placeholder for void */
    int64_t v1258; /* placeholder for void */
    int64_t v130; /* placeholder for void */
    int64_t v3439;
    int64_t v4256; /* placeholder for void */
    void* v787;
    void* v265;
    void* v2499;
    void* v2856;
    int64_t v3095;
    void* v1119;
    int64_t v1514; /* placeholder for void */
    int64_t v2743; /* placeholder for void */
    void* v2553;
    int64_t v4141; /* placeholder for void */
    void* v112;
    int64_t v318;
    int64_t v1558;
    void* v2976;
    void* v263;
    int64_t v1084; /* placeholder for void */
    int64_t v3320;
    void* v4221;
    void* v1782;
    int64_t v3939;
    int64_t v406; /* placeholder for void */
    int64_t v2146; /* placeholder for void */
    bool v2751;
    void* v1276;
    void* v3768;
    int64_t v2328;
    bool v3709;
    int64_t v4335; /* placeholder for void */
    void* v64;
    int64_t v1836;
    void* v1522;
    void* v1954;
    int64_t v713;
    void* v988;
    int64_t v1027;
    bool v2742;
    void* v1234;
    void* v3529;
    int64_t v381; /* placeholder for void */
    int64_t v397; /* placeholder for void */
    void* v1105;
    int64_t v3613; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v415; /* placeholder for void */
    int64_t v323; /* placeholder for void */
    int64_t v2591; /* placeholder for void */
    int64_t v4097; /* placeholder for void */
    void* v4312;
    int64_t v1508; /* placeholder for void */
    int64_t v3236; /* placeholder for void */
    void* v1172;
    int64_t v1549; /* placeholder for void */
    void* v3414;
    int64_t v1650; /* placeholder for void */
    void* v1830;
    void* v2262;
    void* v2036;
    int64_t v1379; /* placeholder for void */
    int64_t v3805; /* placeholder for void */
    int64_t v4076; /* placeholder for void */
    void* v3193;
    int64_t v1370;
    void* v2413;
    int64_t v2114; /* placeholder for void */
    int64_t v3420; /* placeholder for void */
    void* v3460;
    int64_t v450;
    int64_t v3129; /* placeholder for void */
    void* v2315;
    int64_t v2532;
    int64_t v2928;
    void* v2173;
    void* v2311;
    void* v2180;
    int64_t v2135; /* placeholder for void */
    void* v792;
    void* v19;
    int64_t v1334;
    int64_t v1080; /* placeholder for void */
    int64_t v3188; /* placeholder for void */
    void* v3228;
    int64_t v1639; /* placeholder for void */
    void* v2805;
    void* v3714;
    void* v225;
    void* v1970;
    void* v3417;
    void* v251;
    int64_t v1261; /* placeholder for void */
    void* v3487;
    void* v3931;
    void* v1927;
    int64_t v2871; /* placeholder for void */
    void* v1225;
    void* v2965;
    int64_t v3201; /* placeholder for void */
    int64_t v4381; /* placeholder for void */
    void* v2569;
    int64_t v3289; /* placeholder for void */
    int64_t v4121; /* placeholder for void */
    void* v77;
    int64_t v780; /* placeholder for void */
    int64_t v1993;
    void* v670;
    void* v1887;
    int64_t v2675;
    void* v3863;
    void* v4384;
    int64_t v3102;
    void* v3560;
    int64_t v906; /* placeholder for void */
    int64_t v559; /* placeholder for void */
    int64_t v1229;
    int64_t v3410; /* placeholder for void */
    void* v315;
    int64_t v3048;
    int64_t v3339; /* placeholder for void */
    int64_t v3827; /* placeholder for void */
    int64_t v160; /* placeholder for void */
    void* v3126;
    void* v3810;
    int64_t v3868;
    void* v3022;
    int64_t v3675;
    int64_t v3447;
    int64_t v1028;
    void* v3729;
    void* v1294;
    void* v475;
    void* v83;
    void* v736;
    int64_t v1853; /* placeholder for void */
    void* v1255;
    void* v3952;
    int64_t v3792;
    int64_t v4091; /* placeholder for void */
    int64_t v3011;
    int64_t v2656; /* placeholder for void */
    int64_t v1278;
    void* v82;
    void* v4092;
    void* v2864;
    void* v356;
    void* v824;
    int64_t v1680; /* placeholder for void */
    int64_t v481;
    void* v1090;
    void* v1247;
    int64_t v1859;
    int64_t v3196; /* placeholder for void */
    void* v1349;
    void* v3333;
    int64_t v1179; /* placeholder for void */
    void* v386;
    int64_t v2847; /* placeholder for void */
    int64_t v516;
    int64_t v1226; /* placeholder for void */
    int64_t v4151; /* placeholder for void */
    int64_t v2028;
    int64_t v4168;
    int64_t v1634; /* placeholder for void */
    void* v1874;
    void* v2875;
    int64_t v3891; /* placeholder for void */
    int64_t v2094;
    int64_t v602; /* placeholder for void */
    int64_t v2625; /* placeholder for void */
    int64_t v1463;
    void* v2883;
    int64_t v2905; /* placeholder for void */
    void* v3422;
    void* v3531;
    int64_t v1068; /* placeholder for void */
    void* v2517;
    int64_t v3136;
    void* v4257;
    int64_t v1967; /* placeholder for void */
    void* v1478;
    int64_t v3418;
    void* v2581;
    void* v2841;
    int64_t v1223; /* placeholder for void */
    void* v2381;
    int64_t v3287; /* placeholder for void */
    void* v1450;
    int64_t v158;
    void* v1997;
    void* v635;
    void* v4137;
    void* v1546;
    void* v2834;
    int64_t v832;
    void* v387;
    int64_t v1318; /* placeholder for void */
    void* v3781;
    int64_t v4198;
    int64_t v745; /* placeholder for void */
    void* v1442;
    int64_t v1134;
    int64_t v392;
    void* v3898;
    void* v1438;
    int64_t v2645; /* placeholder for void */
    void* v491;
    int64_t v3073;
    int64_t v4206; /* placeholder for void */
    int64_t v1960; /* placeholder for void */
    void* v855;
    int64_t v249;
    int64_t v1759; /* placeholder for void */
    void* v2527;
    void* v2940;
    void* v2225;
    int64_t v3606;
    int64_t v2579; /* placeholder for void */
    void* v3876;
    int64_t v4030;
    void* v683;
    int64_t v1331;
    int64_t v1839;
    int64_t v2050; /* placeholder for void */
    void* v2138;
    int64_t v3467;
    void* v3943;
    bool v872;
    int64_t v4041; /* placeholder for void */
    void* v1000;
    bool v1032;
    int64_t v2080; /* placeholder for void */
    int64_t v4186; /* placeholder for void */
    void* v929;
    void* v4060;
    void* v3043;
    int64_t v2137;
    void* v3915;
    void* v1219;
    int64_t v804; /* placeholder for void */
    void* v3854;
    int64_t v1163; /* placeholder for void */
    int64_t v156; /* placeholder for void */
    int64_t v1814; /* placeholder for void */
    void* v3312;
    int64_t v360; /* placeholder for void */
    void* v480;
    int64_t v711; /* placeholder for void */
    void* v4297;
    void* v94;
    void* v259;
    int64_t v2935; /* placeholder for void */
    void* v74;
    int64_t v3028;
    int64_t v2305;
    void* v3070;
    void* v3826;
    int64_t v408; /* placeholder for void */
    void* v3968;
    int64_t v1768; /* placeholder for void */
    int64_t v933; /* placeholder for void */
    void* v2831;
    void* v3618;
    void* v3610;
    int64_t v321;
    int64_t v565; /* placeholder for void */
    void* v163;
    int64_t v258; /* placeholder for void */
    void* v1274;
    void* v1249;
    int64_t v1246; /* placeholder for void */
    void* v1804;
    int64_t v2555; /* placeholder for void */
    int64_t v3281;
    void* v43;
    bool v2824;
    int64_t v4357; /* placeholder for void */
    void* v1397;
    int64_t v4283; /* placeholder for void */
    void* v2838;
    int64_t v1683;
    void* v3689;
    void* v3725;
    int64_t v912; /* placeholder for void */
    int64_t v4272; /* placeholder for void */
    int64_t v430;
    void* v3824;
    int64_t v2665;
    void* v2890;
    int64_t v940;
    void* v4122;
    int64_t v2433; /* placeholder for void */
    void* v2084;
    void* v403;
    void* v2047;
    int64_t v2992;
    int64_t v746;
    void* v1524;
    int64_t v2123; /* placeholder for void */
    int64_t v3394;
    void* v4250;
    int64_t v1396;
    double v1056;
    void* v4129;
    int64_t v4086; /* placeholder for void */
    int64_t v2669; /* placeholder for void */
    int64_t v2489; /* placeholder for void */
    void* v1850;
    int64_t v291; /* placeholder for void */
    void* v3088;
    int64_t v3365; /* placeholder for void */
    int64_t v674;
    int64_t v338; /* placeholder for void */
    void* v1903;
    int64_t v921; /* placeholder for void */
    int64_t v987;
    int64_t v719; /* placeholder for void */
    void* v1614;
    void* v1938;
    int64_t v2750;
    void* v3987;
    void* v621;
    void* v799;
    int64_t v3981; /* placeholder for void */
    int64_t v2974; /* placeholder for void */
    void* v1824;
    void* v4317;
    void* v584;
    int64_t v1167; /* placeholder for void */
    int64_t v3493;
    void* v2886;
    int64_t v1895;
    int64_t v499; /* placeholder for void */
    void* v240;
    void* v2661;
    void* v4133;
    int64_t v699; /* placeholder for void */
    void* v1198;
    void* v4310;
    int64_t v2172;
    void* v527;
    void* v1886;
    void* v3656;
    int64_t v2652; /* placeholder for void */
    int64_t v3326; /* placeholder for void */
    void* v1788;
    int64_t v1118;
    int64_t v3074;
    int64_t v3454; /* placeholder for void */
    void* v495;
    int64_t v3415; /* placeholder for void */
    int64_t v374; /* placeholder for void */
    void* v496;
    int64_t v1956;
    void* v1375;
    int64_t v595; /* placeholder for void */
    int64_t v1750;
    int64_t v2402;
    int64_t v4101; /* placeholder for void */
    void* v1335;
    int64_t v1939;
    int64_t v2241; /* placeholder for void */
    void* v3278;
    void* v3466;
    int64_t v111;
    int64_t v1253; /* placeholder for void */
    void* v3921;
    int64_t v2758; /* placeholder for void */
    int64_t v3010;
    int64_t v1571;
    void* v479;
    int64_t v506;
    void* v409;
    void* v831;
    void* v1694;
    int64_t v1727; /* placeholder for void */
    int64_t v2222; /* placeholder for void */
    int64_t v2523; /* placeholder for void */
    void* v3978;
    void* v1858;
    void* v744;
    int64_t v889;
    void* v373;
    void* v983;
    int64_t v1284;
    void* v444;
    int64_t v631; /* placeholder for void */
    void* v1826;
    int64_t v115;
    double v1058;
    void* v2042;
    void* v3829;
    int64_t v1160;
    void* v2424;
    void* v2090;
    int64_t v2141;
    int64_t v1130; /* placeholder for void */
    void* v4048;
    void* v1462;
    int64_t v2601;
    int64_t v3084;
    void* v2565;
    void* v2346;
    int64_t v3110; /* placeholder for void */
    int64_t v3720; /* placeholder for void */
    void* v2969;
    int64_t v1412;
    int64_t v1770;
    void* v2711;
    void* v17;
    int64_t v2150;
    void* v2720;
    int64_t v3585;
    void* v2319;
    int64_t v4119;
    void* v3085;
    int64_t v3592;
    int64_t v1594; /* placeholder for void */
    int64_t v1673; /* placeholder for void */
    void* v708;
    int64_t v1686;
    int64_t v2866;
    void* v3832;
    void* v4271;
    int64_t v2340; /* placeholder for void */
    int64_t v4098; /* placeholder for void */
    int64_t v673; /* placeholder for void */
    void* v2685;
    int64_t v2563; /* placeholder for void */
    int64_t v1501; /* placeholder for void */
    int64_t v301; /* placeholder for void */
    void* v3616;
    int64_t v1017; /* placeholder for void */
    void* v955;
    void* v599;
    int64_t v2641; /* placeholder for void */
    void* v3485;
    void* v29;
    int64_t v1419; /* placeholder for void */
    void* v2370;
    void* v2599;
    int64_t v135;
    int64_t v2357;
    int64_t v435; /* placeholder for void */
    int64_t v2959; /* placeholder for void */
    void* v1535;
    int64_t v3896; /* placeholder for void */
    int64_t v3660;
    int64_t v2259;
    void* v143;
    int64_t v4184;
    int64_t v2799;
    int64_t v2987;
    void* v3628;
    int64_t v2881; /* placeholder for void */
    int64_t v1576; /* placeholder for void */
    void* v561;
    int64_t v331;
    int64_t v4188; /* placeholder for void */
    int64_t v2695; /* placeholder for void */
    int64_t v2067; /* placeholder for void */
    void* v3556;
    void* v307;
    int64_t v970; /* placeholder for void */
    void* v1323;
    int64_t v3682; /* placeholder for void */
    void* v1432;
    void* v961;
    int64_t v1413; /* placeholder for void */
    int64_t v3029;
    int64_t v15;
    int64_t v1006; /* placeholder for void */
    void* v3716;
    void* v4385;
    int64_t v90; /* placeholder for void */
    void* v2999;
    void* v3753;
    int64_t v3642;
    void* v1872;
    void* v2379;
    int64_t v3146;
    void* v644;
    void* v233;
    int64_t v2374;
    int64_t v284;
    void* v1644;
    void* v341;
    void* v567;
    int64_t v2598; /* placeholder for void */
    int64_t v1324; /* placeholder for void */
    double v3799;
    int64_t v3174; /* placeholder for void */
    void* v3588;
    int64_t v3648;
    void* v257;
    void* v2790;
    void* v3270;
    void* v1026;
    int64_t v1598; /* placeholder for void */
    int64_t v3122;
    int64_t v2813; /* placeholder for void */
    int64_t v1884; /* placeholder for void */
    void* v39;
    void* v107;
    int64_t v2204; /* placeholder for void */
    int64_t v4052; /* placeholder for void */
    int64_t v1266;
    void* v4375;
    int64_t v4166; /* placeholder for void */
    void* v1201;
    int64_t v524; /* placeholder for void */
    int64_t v3313; /* placeholder for void */
    int64_t v4106; /* placeholder for void */
    void* v1099;
    int64_t v2795;
    int64_t v467; /* placeholder for void */
    int64_t v650; /* placeholder for void */
    void* v3026;
    int64_t v3207; /* placeholder for void */
    void* v751;
    int64_t v2821;
    int64_t v938; /* placeholder for void */
    int64_t v948; /* placeholder for void */
    void* v198;
    void* v3622;
    void* v1191;
    int64_t v4037;
    int64_t v3100;
    void* v3673;
    void* v2318;
    void* v1699;
    int64_t v1701;
    int64_t v3772; /* placeholder for void */
    int64_t v3902;
    int64_t v3510;
    void* v2606;
    void* v1543;
    void* v3919;
    void* v1490;
    int64_t v1756; /* placeholder for void */
    void* v3034;
    int64_t v3522;
    void* v3061;
    int64_t v3401;
    int64_t v1973;
    int64_t v452;
    void* v1206;
    int64_t v1144;
    int64_t v4175; /* placeholder for void */
    void* v1153;
    int64_t v4304; /* placeholder for void */
    void* v2234;
    void* v464;
    void* v647;
    int64_t v1285; /* placeholder for void */
    int64_t v2440; /* placeholder for void */
    int64_t v4226; /* placeholder for void */
    void* v42;
    void* v185;
    int64_t v2116;
    void* v1519;
    int64_t v1267;
    void* v2120;
    int64_t v55;
    bool v1194;
    int64_t v1215;
    void* v2968;
    int64_t v3519; /* placeholder for void */
    int64_t v1457;
    int64_t v335; /* placeholder for void */
    int64_t v2270; /* placeholder for void */
    void* v1488;
    void* v3539;
    int64_t v242;
    void* v2061;
    int64_t v279; /* placeholder for void */
    int64_t v1487; /* placeholder for void */
    void* v3208;
    int64_t v296;
    int64_t v4068; /* placeholder for void */
    int64_t v1763;
    int64_t v2209; /* placeholder for void */
    int64_t v2069;
    void* v3035;
    int64_t v2912; /* placeholder for void */
    int64_t v3587; /* placeholder for void */
    int64_t v781;
    int64_t v131;
    bool v1268;
    int64_t v1013; /* placeholder for void */
    int64_t v500;
    int64_t v391; /* placeholder for void */
    int64_t v1241;
    void* v2778;
    void* v518;
    int64_t v1900; /* placeholder for void */
    int64_t v45;
    void* v2451;
    int64_t v1955; /* placeholder for void */
    void* v1968;
    void* v3786;
    void* v3811;
    void* v2707;
    int64_t v141;
    void* v2030;
    int64_t v2584; /* placeholder for void */
    int64_t v3213;
    void* v3513;
    int64_t v4058;
    void* v777;
    int64_t v2058; /* placeholder for void */
    void* v3736;
    void* v2653;
    int64_t v892; /* placeholder for void */
    void* v3640;
    void* v1346;
    int64_t v3215; /* placeholder for void */
    int64_t v4160; /* placeholder for void */
    int64_t v2703;
    int64_t v2323; /* placeholder for void */
    int64_t v1896;
    int64_t v1864; /* placeholder for void */
    int64_t v2502;
    int64_t v2975; /* placeholder for void */
    void* v1259;
    int64_t v4149;
    void* v3031;
    void* v3130;
    void* v1734;
    int64_t v3711;
    int64_t v3735; /* placeholder for void */
    void* v1176;
    int64_t v1545;
    void* v1265;
    void* v1945;
    int64_t v18;
    int64_t v4104;
    void* v790;
    void* v1148;
    int64_t v1107; /* placeholder for void */
    int64_t v3927; /* placeholder for void */
    int64_t v245;
    void* v829;
    int64_t v217; /* placeholder for void */
    int64_t v641; /* placeholder for void */
    int64_t v1497;
    void* v2776;
    void* v4379;
    void* v1575;
    int64_t v2985;
    int64_t v3612;
    int64_t v3847;
    void* v320;
    int64_t v58;
    int64_t v3217; /* placeholder for void */
    void* v594;
    int64_t v3245; /* placeholder for void */
    int64_t v3343; /* placeholder for void */
    int64_t v3997; /* placeholder for void */
    int64_t v483;
    void* v4260;
    void* v3834;
    void* v3360;
    void* v359;
    int64_t v4263; /* placeholder for void */
    void* v752;
    int64_t v3020;
    int64_t v2827; /* placeholder for void */
    int64_t v592;
    void* v1436;
    int64_t v61;
    int64_t v884; /* placeholder for void */
    int64_t v465;
    int64_t v521; /* placeholder for void */
    int64_t v539; /* placeholder for void */
    void* v2350;
    bool v3173;
    int64_t v750; /* placeholder for void */
    void* v463;
    int64_t v1117; /* placeholder for void */
    void* v2674;
    void* v2899;
    int64_t v4009;
    bool v3356;
    void* v340;
    int64_t v16; /* placeholder for void */
    void* v484;
    void* v589;
    int64_t v1581; /* placeholder for void */
    void* v4265;
    int64_t v3756; /* placeholder for void */
    int64_t v4331;
    int64_t v116; /* placeholder for void */
    int64_t v590;
    int64_t v3190; /* placeholder for void */
    void* v3878;
    int64_t v4267; /* placeholder for void */
    int64_t v2671; /* placeholder for void */
    int64_t v2443; /* placeholder for void */
    void* v3353;
    int64_t v968;
    void* v860;
    int64_t v4094;
    int64_t v818; /* placeholder for void */
    int64_t v411;
    int64_t v3171;
    void* v3538;
    void* v2817;
    int64_t v36; /* placeholder for void */
    int64_t v4343; /* placeholder for void */
    void* v2666;
    int64_t v1587;
    void* v2330;
    int64_t v3426; /* placeholder for void */
    int64_t v2331;
    int64_t v1969;
    int64_t v720;
    void* v3161;
    void* v470;
    int64_t v986;
    int64_t v2544;
    int64_t v3135;
    void* v3147;
    void* v1965;
    bool v4170;
    bool v289;
    void* v3016;
    void* v772;
    int64_t v2642; /* placeholder for void */
    int64_t v4134;
    int64_t v3158; /* placeholder for void */
    void* v3476;
    int64_t v1785;
    void* v375;
    void* v1472;
    int64_t v4181; /* placeholder for void */
    int64_t v3990; /* placeholder for void */
    void* v1213;
    void* v3946;
    void* v3994;
    int64_t v110; /* placeholder for void */
    int64_t v1875;
    int64_t v564;
    void* v1994;
    int64_t v3848; /* placeholder for void */
    int64_t v1410;
    int64_t v3055;
    int64_t v3120;
    int64_t v3225;
    void* v3197;
    void* v3257;
    void* v2699;
    int64_t v531; /* placeholder for void */
    int64_t v2063;
    void* v640;
    bool v3866;
    int64_t v1321; /* placeholder for void */
    void* v2200;
    int64_t v1789;
    void* v1382;
    void* v1168;
    void* v3505;
    void* v1513;
    void* v580;
    int64_t v2088;
    void* v3984;
    int64_t v1914; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    bool v3164;
    int64_t v333; /* placeholder for void */
    void* v520;
    int64_t v976; /* placeholder for void */
    int64_t v1795;
    void* v433;
    int64_t v1583; /* placeholder for void */
    void* v2510;
    void* v1454;
    int64_t v994; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v3143; /* placeholder for void */
    int64_t v1888; /* placeholder for void */
    bool v4185;
    void* v221;
    bool v3708;
    int64_t v78;
    bool v3396;
    int64_t v180; /* placeholder for void */
    void* v2057;
    int64_t v861;
    void* v2260;
    void* v3036;
    void* v2333;
    void* v3292;
    int64_t v1470; /* placeholder for void */
    int64_t v250; /* placeholder for void */
    int64_t v3298;
    int64_t v3852;
    void* v2607;
    int64_t v2089;
    void* v2283;
    int64_t v1794; /* placeholder for void */
    void* v568;
    int64_t v1715;
    void* v1588;
    int64_t v1948; /* placeholder for void */
    bool v2432;
    void* v2541;
    int64_t v2857; /* placeholder for void */
    int64_t v3182; /* placeholder for void */
    void* v2744;
    void* v144;
    int64_t v2197; /* placeholder for void */
    void* v782;
    int64_t v2425; /* placeholder for void */
    void* v1721;
    void* v2962;
    int64_t v2800; /* placeholder for void */
    void* v2475;
    void* v1048;
    int64_t v3272; /* placeholder for void */
    int64_t v954; /* placeholder for void */
    void* v3424;
    void* v13;
    int64_t v4089;
    void* v697;
    int64_t v56; /* placeholder for void */
    int64_t v1642;
    void* v2810;
    int64_t v3721;
    void* v4284;
    void* v1233;
    int64_t v21;
    int64_t v1207;
    int64_t v1711;
    int64_t v3107;
    void* v3703;
    int64_t v2662;
    void* v2738;
    void* v2756;
    int64_t v3910; /* placeholder for void */
    int64_t v2602;
    int64_t v3936; /* placeholder for void */
    int64_t v767; /* placeholder for void */
    bool v2390;
    void* v1325;
    int64_t v2364; /* placeholder for void */
    void* v2865;
    void* v3216;
    int64_t v4169;
    int64_t v1038; /* placeholder for void */
    int64_t v1460;
    void* v3094;
    int64_t v996;
    int64_t v2644; /* placeholder for void */
    int64_t v1429;
    void* v364;
    int64_t v1705;
    int64_t v4043; /* placeholder for void */
    int64_t v741; /* placeholder for void */
    int64_t v2721; /* placeholder for void */
    int64_t v488;
    void* v2655;
    void* v3472;
    int64_t v2299; /* placeholder for void */
    int64_t v4401; /* placeholder for void */
    void* v2082;
    void* v1212;
    int64_t v1523; /* placeholder for void */
    bool v1342;
    int64_t v3068;
    void* v3971;
    int64_t v178; /* placeholder for void */
    void* v4075;
    void* v2001;
    int64_t v350;
    int64_t v2702;
    void* v329;
    void* v3351;
    void* v2501;
    int64_t v2235; /* placeholder for void */
    void* v2951;
    void* v3511;
    void* v1133;
    void* v3632;
    void* v1251;
    void* v104;
    void* v570;
    void* v769;
    int64_t v1120; /* placeholder for void */
    int64_t v3229;
    void* v662;
    int64_t v3678; /* placeholder for void */
    int64_t v4276; /* placeholder for void */
    void* v1674;
    int64_t v281;
    int64_t v696; /* placeholder for void */
    int64_t v1193;
    void* v4024;
    int64_t v313; /* placeholder for void */
    void* v2098;
    void* v551;
    void* v3545;
    void* v4174;
    int64_t v2369; /* placeholder for void */
    void* v597;
    int64_t v1154;
    int64_t v1040; /* placeholder for void */
    void* v2293;
    int64_t v308; /* placeholder for void */
    void* v1766;
    void* v2450;
    int64_t v88;
    void* v1758;
    int64_t v1975; /* placeholder for void */
    int64_t v554; /* placeholder for void */
    void* v1283;
    int64_t v709; /* placeholder for void */
    int64_t v4273; /* placeholder for void */
    void* v1685;
    int64_t v11;
    void* v3350;
    void* v3109;
    int64_t v3453; /* placeholder for void */
    void* v3752;
    int64_t v4216;
    void* v622;
    bool v1095;
    int64_t v2632;
    void* v1961;
    void* v92;
    int64_t v241; /* placeholder for void */
    void* v2296;
    int64_t v2979; /* placeholder for void */
    void* v1554;
    bool v3441;
    void* v2102;
    int64_t v1420;
    void* v3584;
    void* v3651;
    void* v4035;
    void* v1717;
    int64_t v2237;
    int64_t v2217;
    void* v957;
    void* v2739;
    int64_t v3263; /* placeholder for void */
    int64_t v2509; /* placeholder for void */
    bool v3951;
    void* v1231;
    void* v1444;
    void* v2547;
    int64_t v2542;
    void* v2943;
    bool v1063;
    void* v2192;
    void* v3090;
    int64_t v4233;
    void* v2714;
    void* v4004;
    int64_t v4142; /* placeholder for void */
    void* v4088;
    void* v137;
    void* v587;
    void* v1703;
    void* v47;
    void* v1317;
    int64_t v451; /* placeholder for void */
    void* v2716;
    int64_t v425; /* placeholder for void */
    void* v454;
    void* v3564;
    int64_t v3483; /* placeholder for void */
    void* v3436;
    void* v935;
    void* v247;
    int64_t v1563; /* placeholder for void */
    int64_t v1985; /* placeholder for void */
    int64_t v2085; /* placeholder for void */
    int64_t v2497; /* placeholder for void */
    void* v3961;
    void* v2946;
    int64_t v1232; /* placeholder for void */
    int64_t v1624; /* placeholder for void */
    int64_t v3180; /* placeholder for void */
    void* v3387;
    int64_t v3514;
    int64_t v1764;
    int64_t v1645;
    void* v2009;
    void* v2629;
    int64_t v2308;
    int64_t v3012;
    void* v3710;
    int64_t v2168; /* placeholder for void */
    void* v3761;
    void* v5;
    void* v4053;
    int64_t v4262; /* placeholder for void */
    int64_t v2064;
    int64_t v2481; /* placeholder for void */
    void* v2906;
    int64_t v3001;
    void* v3706;
    int64_t v3571;
    int64_t v1983; /* placeholder for void */
    void* v867;
    void* v877;
    void* v1320;
    int64_t v2373;
    int64_t v3748;
    int64_t v3542; /* placeholder for void */
    int64_t v3775;
    void* v1404;
    void* v1982;
    int64_t v193; /* placeholder for void */
    void* v1435;
    void* v1984;
    int64_t v3042; /* placeholder for void */
    void* v3451;
    int64_t v3698; /* placeholder for void */
    int64_t v2719;
    int64_t v513;
    void* v2619;
    void* v4280;
    void* v2837;
    void* v174;
    void* v1135;
    void* v186;
    int64_t v1790; /* placeholder for void */
    int64_t v1340;
    void* v4223;
    int64_t v252;
    int64_t v2271;
    bool v2435;
    void* v3912;
    int64_t v4394;
    void* v3914;
    void* v1995;
    int64_t v2757;
    int64_t v1437; /* placeholder for void */
    void* v995;
    int64_t v1574;
    int64_t v2377;
    int64_t v2568; /* placeholder for void */
    void* v2855;
    void* v705;
    void* v4011;
    void* v2681;
    void* v347;
    void* v1802;
    void* v2037;
    int64_t v1091; /* placeholder for void */
    int64_t v4330;
    void* v3443;
    int64_t v3096; /* placeholder for void */
    int64_t v2250;
    int64_t v4230; /* placeholder for void */
    void* v2474;
    int64_t v4316; /* placeholder for void */
    int64_t v2284;
    void* v114;
    int64_t v1222; /* placeholder for void */
    int64_t v876;
    void* v3589;
    void* v1755;
    int64_t v3821;
    int64_t v1840; /* placeholder for void */
    void* v3905;
    void* v3517;
    void* v4023;
    void* v181;
    int64_t v4123;
    int64_t v1405; /* placeholder for void */
    void* v1649;
    void* v2268;
    int64_t v1520; /* placeholder for void */
    int64_t v3230; /* placeholder for void */
    void* v1702;
    void* v3859;
    void* v4369;
    void* v2289;
    int64_t v3138; /* placeholder for void */
    int64_t v984; /* placeholder for void */
    int64_t v3078; /* placeholder for void */
    void* v3499;
    void* v1931;
    void* v3599;
    void* v1140;
    void* v2694;
    int64_t v96; /* placeholder for void */
    int64_t v763; /* placeholder for void */
    void* v3925;
    void* v2196;
    int64_t v585; /* placeholder for void */
    void* v726;
    void* v2789;
    int64_t v3203; /* placeholder for void */
    void* v3680;
    void* v1540;
    int64_t v3646; /* placeholder for void */
    int64_t v4274; /* placeholder for void */
    int64_t v3069; /* placeholder for void */
    void* v1636;
    int64_t v3758; /* placeholder for void */
    void* v3904;
    int64_t v843; /* placeholder for void */
    int64_t v4087; /* placeholder for void */
    void* v1114;
    void* v1947;
    void* v541;
    int64_t v2942; /* placeholder for void */
    int64_t v4093;
    void* v3982;
    void* v4302;
    void* v2903;
    int64_t v890;
    void* v2422;
    int64_t v4242; /* placeholder for void */
    void* v3713;
    int64_t v615; /* placeholder for void */
    void* v1015;
    void* v213;
    int64_t v3692; /* placeholder for void */
    int64_t v1635;
    void* v2049;
    void* v3119;
    int64_t v2925; /* placeholder for void */
    int64_t v3344; /* placeholder for void */
    void* v440;
    void* v4061;
    void* v714;
    int64_t v2148; /* placeholder for void */
    void* v99;
    int64_t v1330; /* placeholder for void */
    int64_t v206; /* placeholder for void */
    void* v2219;
    int64_t v3704;
    int64_t v2071; /* placeholder for void */
    int64_t v3077;
    int64_t v2193;
    void* v3973;
    void* v4016;
    void* v2236;
    int64_t v355; /* placeholder for void */
    void* v2484;
    void* v3430;
    void* v3993;
    void* v173;
    int64_t v1630;
    int64_t v2119;
    void* v3664;
    int64_t v4014; /* placeholder for void */
    int64_t v1132; /* placeholder for void */
    int64_t v3232; /* placeholder for void */
    int64_t v648;
    int64_t v1992; /* placeholder for void */
    int64_t v4339;
    void* v1398;
    void* v3195;
    int64_t v4172; /* placeholder for void */
    void* v326;
    int64_t v1855;
    void* v2624;
    int64_t v4064;
    int64_t v966; /* placeholder for void */
    void* v2635;
    void* v4245;
    int64_t v2345; /* placeholder for void */
    void* v337;
    void* v3052;
    int64_t v3184; /* placeholder for void */
    int64_t v571;
    int64_t v2380; /* placeholder for void */
    void* v630;
    int64_t v3407; /* placeholder for void */
    int64_t v3490; /* placeholder for void */
    int64_t v3804;
    int64_t v2897; /* placeholder for void */
    int64_t v2367;
    void* v2787;
    void* v322;
    int64_t v228; /* placeholder for void */
    void* v2032;
    void* v3025;
    void* v608;
    int64_t v576; /* placeholder for void */
    int64_t v1516; /* placeholder for void */
    int64_t v3030;
    int64_t v1897;
    void* v267;
    int64_t v1628; /* placeholder for void */
    void* v3901;
    int64_t v2488; /* placeholder for void */
    int64_t v930; /* placeholder for void */
    int64_t v3386;
    void* v2529;
    void* v2931;
    void* v2934;
    void* v1616;
    bool v4110;
    void* v2483;
    int64_t v1474; /* placeholder for void */
    int64_t v1443; /* placeholder for void */
    void* v501;
    int64_t v1774;
    int64_t v2008;
    void* v2127;
    int64_t v2792; /* placeholder for void */
    int64_t v3160; /* placeholder for void */
    bool v4248;
    void* v819;
    void* v3166;
    int64_t v734;
    int64_t v4012; /* placeholder for void */
    int64_t v2378; /* placeholder for void */
    void* v649;
    int64_t v2604; /* placeholder for void */
    void* v4238;
    int64_t v3814; /* placeholder for void */
    void* v1479;
    int64_t v3285; /* placeholder for void */
    void* v3574;
    void* v685;
    int64_t v1500; /* placeholder for void */
    void* v1910;
    void* v3812;
    int64_t v694;
    int64_t v1632; /* placeholder for void */
    void* v3749;
    void* v596;
    int64_t v2930; /* placeholder for void */
    int64_t v1309; /* placeholder for void */
    void* v542;
    int64_t v2121; /* placeholder for void */
    int64_t v2878;
    int64_t v2414;
    int64_t v1157;
    void* v2732;
    int64_t v314;
    int64_t v899; /* placeholder for void */
    int64_t v758;
    int64_t v4254;
    int64_t v4143; /* placeholder for void */
    void* v2187;
    int64_t v866; /* placeholder for void */
    int64_t v2646; /* placeholder for void */
    void* v3568;
    int64_t v1799;
    int64_t v371;
    void* v472;
    void* v2885;
    int64_t v3093;
    int64_t v3264;
    bool v3865;
    void* v349;
    void* v1936;
    void* v1288;
    int64_t v3425;
    int64_t v346;
    void* v1087;
    int64_t v1393; /* placeholder for void */
    void* v522;
    void* v1152;
    int64_t v4249; /* placeholder for void */
    int64_t v2478;
    bool v1005;
    int64_t v2887;
    void* v2952;
    int64_t v813; /* placeholder for void */
    void* v888;
    int64_t v2533; /* placeholder for void */
    int64_t v3395;
    int64_t v3301; /* placeholder for void */
    int64_t v1698; /* placeholder for void */
    void* v2595;
    int64_t v4028;
    int64_t v25;
    void* v1901;
    void* v3941;
    void* v4311;
    int64_t v4285;
    int64_t v854; /* placeholder for void */
    int64_t v3477;
    void* v1085;
    void* v1368;
    int64_t v540;
    int64_t v1737;
    void* v253;
    void* v679;
    int64_t v146; /* placeholder for void */
    int64_t v879; /* placeholder for void */
    int64_t v1187; /* placeholder for void */
    int64_t v3205; /* placeholder for void */
    void* v3594;
    void* v959;
    void* v2298;
    int64_t v2468; /* placeholder for void */
    int64_t v3450; /* placeholder for void */
    int64_t v4380;
    void* v4318;
    int64_t v236; /* placeholder for void */
    int64_t v3787; /* placeholder for void */
    void* v4144;
    int64_t v148;
    void* v1345;
    void* v1950;
    void* v738;
    int64_t v1381; /* placeholder for void */
    int64_t v903; /* placeholder for void */
    void* v2418;
    void* v2913;
    int64_t v3091;
    int64_t v733; /* placeholder for void */
    void* v453;
    int64_t v230;
    void* v1990;
    int64_t v2872;
    void* v2363;
    int64_t v2479;
    int64_t v1857;
    int64_t v2056;
    int64_t v3913;
    int64_t v3015; /* placeholder for void */
    void* v3659;
    void* v3972;
    void* v817;
    void* v3279;
    void* v399;
    int64_t v660; /* placeholder for void */
    int64_t v1279; /* placeholder for void */
    void* v4210;
    int64_t v138;
    int64_t v4286; /* placeholder for void */
    int64_t v1926; /* placeholder for void */
    double v1066;
    void* v3791;
    int64_t v2548; /* placeholder for void */
    int64_t v1171; /* placeholder for void */
    void* v643;
    int64_t v1700; /* placeholder for void */
    void* v2437;
    int64_t v2416; /* placeholder for void */
    int64_t v2574;
    void* v3763;
    int64_t v624; /* placeholder for void */
    int64_t v816; /* placeholder for void */
    int64_t v3480;
    int64_t v3715;
    int64_t v3926;
    void* v3502;
    void* v588;
    void* v459;
    int64_t v1333; /* placeholder for void */
    v1 = _str17;
    p_print_str(v1);
    v3 = _str18;
    p_print_str(v3);
    v5 = _str19;
    p_print_str(v5);
    v7 = malloc(40);
    v8 = 0LL;
    v9 = (char*)v7 + 0;
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 0LL;
    v12 = type_to_str((void*)v11, v7);
    v13 = _str20;
    v14 = _str21;
    v15 = 0LL;
    assert_eq((void*)v15, v12, v13, v14);
    v17 = malloc(40);
    v18 = 1LL;
    v19 = (char*)v17 + 0;
    *(int64_t*)v19 = (int64_t)v18;
    v21 = 0LL;
    v22 = type_to_str((void*)v21, v17);
    v23 = _str22;
    v24 = _str23;
    v25 = 0LL;
    assert_eq((void*)v25, v22, v23, v24);
    v27 = malloc(40);
    v28 = 2LL;
    v29 = (char*)v27 + 0;
    *(int64_t*)v29 = (int64_t)v28;
    v31 = 0LL;
    v32 = type_to_str((void*)v31, v27);
    v33 = _str24;
    v34 = _str25;
    v35 = 0LL;
    assert_eq((void*)v35, v32, v33, v34);
    v37 = malloc(40);
    v38 = 3LL;
    v39 = (char*)v37 + 0;
    *(int64_t*)v39 = (int64_t)v38;
    v41 = 0LL;
    v42 = type_to_str((void*)v41, v37);
    v43 = _str26;
    v44 = _str27;
    v45 = 0LL;
    assert_eq((void*)v45, v42, v43, v44);
    v47 = malloc(40);
    v48 = 4LL;
    v49 = (char*)v47 + 0;
    *(int64_t*)v49 = (int64_t)v48;
    v51 = 0LL;
    v52 = type_to_str((void*)v51, v47);
    v53 = _str28;
    v54 = _str29;
    v55 = 0LL;
    assert_eq((void*)v55, v52, v53, v54);
    v57 = malloc(40);
    v58 = 5LL;
    v59 = (char*)v57 + 0;
    *(int64_t*)v59 = (int64_t)v58;
    v61 = 0LL;
    v62 = type_to_str((void*)v61, v57);
    v63 = _str30;
    v64 = _str31;
    v65 = 0LL;
    assert_eq((void*)v65, v62, v63, v64);
    v67 = malloc(40);
    v68 = 6LL;
    v69 = (char*)v67 + 0;
    *(int64_t*)v69 = (int64_t)v68;
    v71 = 0LL;
    v72 = type_to_str((void*)v71, v67);
    v73 = _str32;
    v74 = _str33;
    v75 = 0LL;
    assert_eq((void*)v75, v72, v73, v74);
    v77 = malloc(40);
    v78 = 7LL;
    v79 = (char*)v77 + 0;
    *(int64_t*)v79 = (int64_t)v78;
    v81 = 0LL;
    v82 = type_to_str((void*)v81, v77);
    v83 = _str34;
    v84 = _str35;
    v85 = 0LL;
    assert_eq((void*)v85, v82, v83, v84);
    v87 = malloc(40);
    v88 = 8LL;
    v89 = (char*)v87 + 0;
    *(int64_t*)v89 = (int64_t)v88;
    v91 = 0LL;
    v92 = type_to_str((void*)v91, v87);
    v93 = _str36;
    v94 = _str37;
    v95 = 0LL;
    assert_eq((void*)v95, v92, v93, v94);
    v97 = malloc(40);
    v98 = 9LL;
    v99 = (char*)v97 + 0;
    *(int64_t*)v99 = (int64_t)v98;
    v101 = 0LL;
    v102 = type_to_str((void*)v101, v97);
    v103 = _str38;
    v104 = _str39;
    v105 = 0LL;
    assert_eq((void*)v105, v102, v103, v104);
    v107 = malloc(40);
    v108 = 10LL;
    v109 = (char*)v107 + 0;
    *(int64_t*)v109 = (int64_t)v108;
    v111 = 0LL;
    v112 = type_to_str((void*)v111, v107);
    v113 = _str40;
    v114 = _str41;
    v115 = 0LL;
    assert_eq((void*)v115, v112, v113, v114);
    v117 = malloc(40);
    v118 = 11LL;
    v119 = (char*)v117 + 0;
    *(int64_t*)v119 = (int64_t)v118;
    v121 = 0LL;
    v122 = type_to_str((void*)v121, v117);
    v123 = _str42;
    v124 = _str43;
    v125 = 0LL;
    assert_eq((void*)v125, v122, v123, v124);
    v127 = malloc(40);
    v128 = 12LL;
    v129 = (char*)v127 + 0;
    *(int64_t*)v129 = (int64_t)v128;
    v131 = 0LL;
    v132 = type_to_str((void*)v131, v127);
    v133 = _str44;
    v134 = _str45;
    v135 = 0LL;
    assert_eq((void*)v135, v132, v133, v134);
    v137 = malloc(40);
    v138 = 13LL;
    v139 = (char*)v137 + 0;
    *(int64_t*)v139 = (int64_t)v138;
    v141 = 0LL;
    v142 = type_to_str((void*)v141, v137);
    v143 = _str46;
    v144 = _str47;
    v145 = 0LL;
    assert_eq((void*)v145, v142, v143, v144);
    v147 = malloc(40);
    v148 = 14LL;
    v149 = (char*)v147 + 0;
    *(int64_t*)v149 = (int64_t)v148;
    v151 = 0LL;
    v152 = type_to_str((void*)v151, v147);
    v153 = _str48;
    v154 = _str49;
    v155 = 0LL;
    assert_eq((void*)v155, v152, v153, v154);
    v157 = malloc(40);
    v158 = 15LL;
    v159 = (char*)v157 + 0;
    *(int64_t*)v159 = (int64_t)v158;
    v161 = 0LL;
    v162 = type_to_str((void*)v161, v157);
    v163 = _str50;
    v164 = _str51;
    v165 = 0LL;
    assert_eq((void*)v165, v162, v163, v164);
    v167 = malloc(40);
    v168 = 16LL;
    v169 = (char*)v167 + 0;
    *(int64_t*)v169 = (int64_t)v168;
    v171 = 0LL;
    v172 = type_to_str((void*)v171, v167);
    v173 = _str52;
    v174 = _str53;
    v175 = 0LL;
    assert_eq((void*)v175, v172, v173, v174);
    v177 = _str54;
    p_print_str(v177);
    v179 = _str55;
    p_print_str(v179);
    v181 = malloc(40);
    v182 = 17LL;
    v183 = (char*)v181 + 0;
    *(int64_t*)v183 = (int64_t)v182;
    v185 = _str56;
    v186 = (char*)v181 + 8;
    *(void**)v186 = (void*)v185;
    v188 = 0LL;
    v189 = type_to_str((void*)v188, v181);
    v190 = _str56;
    v191 = _str57;
    v192 = 0LL;
    assert_eq((void*)v192, v189, v190, v191);
    v194 = malloc(40);
    v195 = 17LL;
    v196 = (char*)v194 + 0;
    *(int64_t*)v196 = (int64_t)v195;
    v198 = _str58;
    v199 = (char*)v194 + 8;
    *(void**)v199 = (void*)v198;
    v201 = 0LL;
    v202 = type_to_str((void*)v201, v194);
    v203 = _str58;
    v204 = _str59;
    v205 = 0LL;
    assert_eq((void*)v205, v202, v203, v204);
    v207 = malloc(40);
    v208 = 18LL;
    v209 = (char*)v207 + 0;
    *(int64_t*)v209 = (int64_t)v208;
    v211 = malloc(40);
    v212 = 3LL;
    v213 = (char*)v211 + 0;
    *(int64_t*)v213 = (int64_t)v212;
    v215 = 8LL;
    v216 = p_malloc(v215);
    *(void**)v216 = (void*)v211;
    v218 = (char*)v207 + 8;
    *(void**)v218 = (void*)v216;
    v220 = 10LL;
    v221 = (char*)v207 + 16;
    *(int64_t*)v221 = (int64_t)v220;
    v223 = 0LL;
    v224 = type_to_str((void*)v223, v207);
    v225 = _str60;
    v226 = _str61;
    v227 = 0LL;
    assert_eq((void*)v227, v224, v225, v226);
    v229 = malloc(40);
    v230 = 18LL;
    v231 = (char*)v229 + 0;
    *(int64_t*)v231 = (int64_t)v230;
    v233 = malloc(40);
    v234 = 11LL;
    v235 = (char*)v233 + 0;
    *(int64_t*)v235 = (int64_t)v234;
    v237 = 8LL;
    v238 = p_malloc(v237);
    *(void**)v238 = (void*)v233;
    v240 = (char*)v229 + 8;
    *(void**)v240 = (void*)v238;
    v242 = 0LL;
    v243 = (char*)v229 + 16;
    *(int64_t*)v243 = (int64_t)v242;
    v245 = 0LL;
    v246 = type_to_str((void*)v245, v229);
    v247 = _str62;
    v248 = _str63;
    v249 = 0LL;
    assert_eq((void*)v249, v246, v247, v248);
    v251 = malloc(40);
    v252 = 19LL;
    v253 = (char*)v251 + 0;
    *(int64_t*)v253 = (int64_t)v252;
    v255 = malloc(40);
    v256 = 17LL;
    v257 = (char*)v255 + 0;
    *(int64_t*)v257 = (int64_t)v256;
    v259 = _str64;
    v260 = (char*)v255 + 8;
    *(void**)v260 = (void*)v259;
    v262 = 8LL;
    v263 = p_malloc(v262);
    *(void**)v263 = (void*)v255;
    v265 = (char*)v251 + 8;
    *(void**)v265 = (void*)v263;
    v267 = _str65;
    v268 = (char*)v251 + 16;
    *(void**)v268 = (void*)v267;
    v270 = 0LL;
    v271 = type_to_str((void*)v270, v251);
    v272 = _str66;
    v273 = _str67;
    v274 = 0LL;
    assert_eq((void*)v274, v271, v272, v273);
    v276 = malloc(40);
    v277 = 20LL;
    v278 = (char*)v276 + 0;
    *(int64_t*)v278 = (int64_t)v277;
    v280 = malloc(40);
    v281 = 3LL;
    v282 = (char*)v280 + 0;
    *(int64_t*)v282 = (int64_t)v281;
    v284 = 8LL;
    v285 = p_malloc(v284);
    *(void**)v285 = (void*)v280;
    v287 = (char*)v276 + 8;
    *(void**)v287 = (void*)v285;
    v289 = false;
    v290 = (char*)v276 + 16;
    *(bool*)v290 = v289;
    v292 = 0LL;
    v293 = type_to_str((void*)v292, v276);
    v294 = _str68;
    v295 = _str69;
    v296 = 0LL;
    assert_eq((void*)v296, v293, v294, v295);
    v298 = malloc(40);
    v299 = 20LL;
    v300 = (char*)v298 + 0;
    *(int64_t*)v300 = (int64_t)v299;
    v302 = malloc(40);
    v303 = 13LL;
    v304 = (char*)v302 + 0;
    *(int64_t*)v304 = (int64_t)v303;
    v306 = 8LL;
    v307 = p_malloc(v306);
    *(void**)v307 = (void*)v302;
    v309 = (char*)v298 + 8;
    *(void**)v309 = (void*)v307;
    v311 = true;
    v312 = (char*)v298 + 16;
    *(bool*)v312 = v311;
    v314 = 0LL;
    v315 = type_to_str((void*)v314, v298);
    v316 = _str70;
    v317 = _str71;
    v318 = 0LL;
    assert_eq((void*)v318, v315, v316, v317);
    v320 = malloc(40);
    v321 = 21LL;
    v322 = (char*)v320 + 0;
    *(int64_t*)v322 = (int64_t)v321;
    v324 = malloc(40);
    v325 = 17LL;
    v326 = (char*)v324 + 0;
    *(int64_t*)v326 = (int64_t)v325;
    v328 = _str64;
    v329 = (char*)v324 + 8;
    *(void**)v329 = (void*)v328;
    v331 = 8LL;
    v332 = p_malloc(v331);
    *(void**)v332 = (void*)v324;
    v334 = (char*)v320 + 8;
    *(void**)v334 = (void*)v332;
    v336 = true;
    v337 = (char*)v320 + 16;
    *(bool*)v337 = v336;
    v339 = 0LL;
    v340 = type_to_str((void*)v339, v320);
    v341 = _str72;
    v342 = _str73;
    v343 = 0LL;
    assert_eq((void*)v343, v340, v341, v342);
    v345 = malloc(40);
    v346 = 21LL;
    v347 = (char*)v345 + 0;
    *(int64_t*)v347 = (int64_t)v346;
    v349 = malloc(40);
    v350 = 5LL;
    v351 = (char*)v349 + 0;
    *(int64_t*)v351 = (int64_t)v350;
    v353 = 8LL;
    v354 = p_malloc(v353);
    *(void**)v354 = (void*)v349;
    v356 = (char*)v345 + 8;
    *(void**)v356 = (void*)v354;
    v358 = false;
    v359 = (char*)v345 + 16;
    *(bool*)v359 = v358;
    v361 = 0LL;
    v362 = type_to_str((void*)v361, v345);
    v363 = _str74;
    v364 = _str75;
    v365 = 0LL;
    assert_eq((void*)v365, v362, v363, v364);
    v367 = _str54;
    p_print_str(v367);
    v369 = _str76;
    p_print_str(v369);
    v371 = 0LL;
    v372 = Vec_new((void*)v371);
    v373 = malloc(8);
    *(void**)v373 = (void*)v372;
    v375 = *(void**)v373;
    v376 = malloc(40);
    v377 = 3LL;
    v378 = (char*)v376 + 0;
    *(int64_t*)v378 = (int64_t)v377;
    v380 = 0LL;
    Vec_push((void*)v380, v375, (int64_t)v376);
    v382 = malloc(40);
    v383 = 22LL;
    v384 = (char*)v382 + 0;
    *(int64_t*)v384 = (int64_t)v383;
    v386 = _str77;
    v387 = (char*)v382 + 8;
    *(void**)v387 = (void*)v386;
    v389 = *(void**)v373;
    v390 = (char*)v382 + 16;
    *(void**)v390 = (void*)v389;
    v392 = 0LL;
    v393 = type_to_str((void*)v392, v382);
    v394 = _str78;
    v395 = _str79;
    v396 = 0LL;
    assert_eq((void*)v396, v393, v394, v395);
    v398 = 0LL;
    v399 = Vec_new((void*)v398);
    v400 = malloc(8);
    *(void**)v400 = (void*)v399;
    v402 = *(void**)v400;
    v403 = malloc(40);
    v404 = 13LL;
    v405 = (char*)v403 + 0;
    *(int64_t*)v405 = (int64_t)v404;
    v407 = 0LL;
    Vec_push((void*)v407, v402, (int64_t)v403);
    v409 = *(void**)v400;
    v410 = malloc(40);
    v411 = 3LL;
    v412 = (char*)v410 + 0;
    *(int64_t*)v412 = (int64_t)v411;
    v414 = 0LL;
    Vec_push((void*)v414, v409, (int64_t)v410);
    v416 = malloc(40);
    v417 = 22LL;
    v418 = (char*)v416 + 0;
    *(int64_t*)v418 = (int64_t)v417;
    v420 = _str58;
    v421 = (char*)v416 + 8;
    *(void**)v421 = (void*)v420;
    v423 = *(void**)v400;
    v424 = (char*)v416 + 16;
    *(void**)v424 = (void*)v423;
    v426 = 0LL;
    v427 = type_to_str((void*)v426, v416);
    v428 = _str80;
    v429 = _str81;
    v430 = 0LL;
    assert_eq((void*)v430, v427, v428, v429);
    v432 = 0LL;
    v433 = Vec_new((void*)v432);
    v434 = malloc(8);
    *(void**)v434 = (void*)v433;
    v436 = malloc(40);
    v437 = 22LL;
    v438 = (char*)v436 + 0;
    *(int64_t*)v438 = (int64_t)v437;
    v440 = _str82;
    v441 = (char*)v436 + 8;
    *(void**)v441 = (void*)v440;
    v443 = *(void**)v434;
    v444 = (char*)v436 + 16;
    *(void**)v444 = (void*)v443;
    v446 = 0LL;
    v447 = type_to_str((void*)v446, v436);
    v448 = _str83;
    v449 = _str84;
    v450 = 0LL;
    assert_eq((void*)v450, v447, v448, v449);
    v452 = 0LL;
    v453 = Vec_new((void*)v452);
    v454 = malloc(8);
    *(void**)v454 = (void*)v453;
    v456 = *(void**)v454;
    v457 = malloc(40);
    v458 = 3LL;
    v459 = (char*)v457 + 0;
    *(int64_t*)v459 = (int64_t)v458;
    v461 = 0LL;
    Vec_push((void*)v461, v456, (int64_t)v457);
    v463 = *(void**)v454;
    v464 = malloc(40);
    v465 = 13LL;
    v466 = (char*)v464 + 0;
    *(int64_t*)v466 = (int64_t)v465;
    v468 = 0LL;
    Vec_push((void*)v468, v463, (int64_t)v464);
    v470 = malloc(40);
    v471 = 23LL;
    v472 = (char*)v470 + 0;
    *(int64_t*)v472 = (int64_t)v471;
    v474 = *(void**)v454;
    v475 = (char*)v470 + 8;
    *(void**)v475 = (void*)v474;
    v477 = 0LL;
    v478 = type_to_str((void*)v477, v470);
    v479 = _str85;
    v480 = _str86;
    v481 = 0LL;
    assert_eq((void*)v481, v478, v479, v480);
    v483 = 0LL;
    v484 = Vec_new((void*)v483);
    v485 = malloc(8);
    *(void**)v485 = (void*)v484;
    v487 = malloc(40);
    v488 = 23LL;
    v489 = (char*)v487 + 0;
    *(int64_t*)v489 = (int64_t)v488;
    v491 = *(void**)v485;
    v492 = (char*)v487 + 8;
    *(void**)v492 = (void*)v491;
    v494 = 0LL;
    v495 = type_to_str((void*)v494, v487);
    v496 = _str87;
    v497 = _str88;
    v498 = 0LL;
    assert_eq((void*)v498, v495, v496, v497);
    v500 = 0LL;
    v501 = Vec_new((void*)v500);
    v502 = malloc(8);
    *(void**)v502 = (void*)v501;
    v504 = *(void**)v502;
    v505 = malloc(40);
    v506 = 3LL;
    v507 = (char*)v505 + 0;
    *(int64_t*)v507 = (int64_t)v506;
    v509 = 0LL;
    Vec_push((void*)v509, v504, (int64_t)v505);
    v511 = *(void**)v502;
    v512 = malloc(40);
    v513 = 13LL;
    v514 = (char*)v512 + 0;
    *(int64_t*)v514 = (int64_t)v513;
    v516 = 0LL;
    Vec_push((void*)v516, v511, (int64_t)v512);
    v518 = malloc(40);
    v519 = 24LL;
    v520 = (char*)v518 + 0;
    *(int64_t*)v520 = (int64_t)v519;
    v522 = *(void**)v502;
    v523 = (char*)v518 + 8;
    *(void**)v523 = (void*)v522;
    v525 = malloc(40);
    v526 = 12LL;
    v527 = (char*)v525 + 0;
    *(int64_t*)v527 = (int64_t)v526;
    v529 = 8LL;
    v530 = p_malloc(v529);
    *(void**)v530 = (void*)v525;
    v532 = (char*)v518 + 16;
    *(void**)v532 = (void*)v530;
    v534 = 0LL;
    v535 = type_to_str((void*)v534, v518);
    v536 = _str89;
    v537 = _str90;
    v538 = 0LL;
    assert_eq((void*)v538, v535, v536, v537);
    v540 = 0LL;
    v541 = Vec_new((void*)v540);
    v542 = malloc(8);
    *(void**)v542 = (void*)v541;
    v544 = malloc(40);
    v545 = 24LL;
    v546 = (char*)v544 + 0;
    *(int64_t*)v546 = (int64_t)v545;
    v548 = *(void**)v542;
    v549 = (char*)v544 + 8;
    *(void**)v549 = (void*)v548;
    v551 = malloc(40);
    v552 = 14LL;
    v553 = (char*)v551 + 0;
    *(int64_t*)v553 = (int64_t)v552;
    v555 = 8LL;
    v556 = p_malloc(v555);
    *(void**)v556 = (void*)v551;
    v558 = (char*)v544 + 16;
    *(void**)v558 = (void*)v556;
    v560 = 0LL;
    v561 = type_to_str((void*)v560, v544);
    v562 = _str91;
    v563 = _str92;
    v564 = 0LL;
    assert_eq((void*)v564, v561, v562, v563);
    v566 = 0LL;
    v567 = Vec_new((void*)v566);
    v568 = malloc(8);
    *(void**)v568 = (void*)v567;
    v570 = malloc(40);
    v571 = 25LL;
    v572 = (char*)v570 + 0;
    *(int64_t*)v572 = (int64_t)v571;
    v574 = _str64;
    v575 = (char*)v570 + 8;
    *(void**)v575 = (void*)v574;
    v577 = _str93;
    v578 = (char*)v570 + 16;
    *(void**)v578 = (void*)v577;
    v580 = _str94;
    v581 = (char*)v570 + 24;
    *(void**)v581 = (void*)v580;
    v583 = *(void**)v568;
    v584 = (char*)v570 + 32;
    *(void**)v584 = (void*)v583;
    v586 = 0LL;
    v587 = type_to_str((void*)v586, v570);
    v588 = _str95;
    v589 = _str96;
    v590 = 0LL;
    assert_eq((void*)v590, v587, v588, v589);
    v592 = 0LL;
    v593 = Vec_new((void*)v592);
    v594 = malloc(8);
    *(void**)v594 = (void*)v593;
    v596 = *(void**)v594;
    v597 = malloc(40);
    v598 = 3LL;
    v599 = (char*)v597 + 0;
    *(int64_t*)v599 = (int64_t)v598;
    v601 = 0LL;
    Vec_push((void*)v601, v596, (int64_t)v597);
    v603 = malloc(40);
    v604 = 25LL;
    v605 = (char*)v603 + 0;
    *(int64_t*)v605 = (int64_t)v604;
    v607 = _str64;
    v608 = (char*)v603 + 8;
    *(void**)v608 = (void*)v607;
    v610 = _str93;
    v611 = (char*)v603 + 16;
    *(void**)v611 = (void*)v610;
    v613 = _str94;
    v614 = (char*)v603 + 24;
    *(void**)v614 = (void*)v613;
    v616 = *(void**)v594;
    v617 = (char*)v603 + 32;
    *(void**)v617 = (void*)v616;
    v619 = 0LL;
    v620 = type_to_str((void*)v619, v603);
    v621 = _str97;
    v622 = _str98;
    v623 = 0LL;
    assert_eq((void*)v623, v620, v621, v622);
    v625 = malloc(40);
    v626 = 26LL;
    v627 = (char*)v625 + 0;
    *(int64_t*)v627 = (int64_t)v626;
    v629 = _str99;
    v630 = (char*)v625 + 8;
    *(void**)v630 = (void*)v629;
    v632 = 0LL;
    v633 = type_to_str((void*)v632, v625);
    v634 = _str100;
    v635 = _str101;
    v636 = 0LL;
    assert_eq((void*)v636, v633, v634, v635);
    v638 = _str54;
    p_print_str(v638);
    v640 = _str102;
    p_print_str(v640);
    v642 = 0LL;
    v643 = Vec_new((void*)v642);
    v644 = malloc(8);
    *(void**)v644 = (void*)v643;
    v646 = *(void**)v644;
    v647 = malloc(16);
    v648 = 0LL;
    v649 = (char*)v647 + 0;
    *(int64_t*)v649 = (int64_t)v648;
    v651 = 784LL;
    v652 = (char*)v647 + 8;
    *(int64_t*)v652 = (int64_t)v651;
    v654 = 0LL;
    Vec_push((void*)v654, v646, (int64_t)v647);
    v656 = *(void**)v644;
    v657 = malloc(16);
    v658 = 0LL;
    v659 = (char*)v657 + 0;
    *(int64_t*)v659 = (int64_t)v658;
    v661 = 128LL;
    v662 = (char*)v657 + 8;
    *(int64_t*)v662 = (int64_t)v661;
    v664 = 0LL;
    Vec_push((void*)v664, v656, (int64_t)v657);
    v666 = malloc(40);
    v667 = 27LL;
    v668 = (char*)v666 + 0;
    *(int64_t*)v668 = (int64_t)v667;
    v670 = malloc(40);
    v671 = 10LL;
    v672 = (char*)v670 + 0;
    *(int64_t*)v672 = (int64_t)v671;
    v674 = 8LL;
    v675 = p_malloc(v674);
    *(void**)v675 = (void*)v670;
    v677 = (char*)v666 + 8;
    *(void**)v677 = (void*)v675;
    v679 = *(void**)v644;
    v680 = (char*)v666 + 16;
    *(void**)v680 = (void*)v679;
    v682 = 0LL;
    v683 = type_to_str((void*)v682, v666);
    v684 = _str103;
    v685 = _str104;
    v686 = 0LL;
    assert_eq((void*)v686, v683, v684, v685);
    v688 = 0LL;
    v689 = Vec_new((void*)v688);
    v690 = malloc(8);
    *(void**)v690 = (void*)v689;
    v692 = *(void**)v690;
    v693 = malloc(16);
    v694 = 1LL;
    v695 = (char*)v693 + 0;
    *(int64_t*)v695 = (int64_t)v694;
    v697 = _str105;
    v698 = (char*)v693 + 8;
    *(void**)v698 = (void*)v697;
    v700 = 0LL;
    Vec_push((void*)v700, v692, (int64_t)v693);
    v702 = *(void**)v690;
    v703 = malloc(16);
    v704 = 0LL;
    v705 = (char*)v703 + 0;
    *(int64_t*)v705 = (int64_t)v704;
    v707 = 784LL;
    v708 = (char*)v703 + 8;
    *(int64_t*)v708 = (int64_t)v707;
    v710 = 0LL;
    Vec_push((void*)v710, v702, (int64_t)v703);
    v712 = malloc(40);
    v713 = 27LL;
    v714 = (char*)v712 + 0;
    *(int64_t*)v714 = (int64_t)v713;
    v716 = malloc(40);
    v717 = 10LL;
    v718 = (char*)v716 + 0;
    *(int64_t*)v718 = (int64_t)v717;
    v720 = 8LL;
    v721 = p_malloc(v720);
    *(void**)v721 = (void*)v716;
    v723 = (char*)v712 + 8;
    *(void**)v723 = (void*)v721;
    v725 = *(void**)v690;
    v726 = (char*)v712 + 16;
    *(void**)v726 = (void*)v725;
    v728 = 0LL;
    v729 = type_to_str((void*)v728, v712);
    v730 = _str106;
    v731 = _str107;
    v732 = 0LL;
    assert_eq((void*)v732, v729, v730, v731);
    v734 = 0LL;
    v735 = Vec_new((void*)v734);
    v736 = malloc(8);
    *(void**)v736 = (void*)v735;
    v738 = malloc(40);
    v739 = 27LL;
    v740 = (char*)v738 + 0;
    *(int64_t*)v740 = (int64_t)v739;
    v742 = malloc(40);
    v743 = 11LL;
    v744 = (char*)v742 + 0;
    *(int64_t*)v744 = (int64_t)v743;
    v746 = 8LL;
    v747 = p_malloc(v746);
    *(void**)v747 = (void*)v742;
    v749 = (char*)v738 + 8;
    *(void**)v749 = (void*)v747;
    v751 = *(void**)v736;
    v752 = (char*)v738 + 16;
    *(void**)v752 = (void*)v751;
    v754 = 0LL;
    v755 = type_to_str((void*)v754, v738);
    v756 = _str108;
    v757 = _str109;
    v758 = 0LL;
    assert_eq((void*)v758, v755, v756, v757);
    v760 = _str54;
    p_print_str(v760);
    v762 = _str110;
    p_print_str(v762);
    v764 = malloc(16);
    v765 = 0LL;
    v766 = (char*)v764 + 0;
    *(int64_t*)v766 = (int64_t)v765;
    v768 = 42LL;
    v769 = (char*)v764 + 8;
    *(int64_t*)v769 = (int64_t)v768;
    v771 = 0LL;
    v772 = shape_dim_to_str((void*)v771, v764);
    v773 = _str111;
    v774 = _str112;
    v775 = 0LL;
    assert_eq((void*)v775, v772, v773, v774);
    v777 = malloc(16);
    v778 = 0LL;
    v779 = (char*)v777 + 0;
    *(int64_t*)v779 = (int64_t)v778;
    v781 = 0LL;
    v782 = (char*)v777 + 8;
    *(int64_t*)v782 = (int64_t)v781;
    v784 = 0LL;
    v785 = shape_dim_to_str((void*)v784, v777);
    v786 = _str15;
    v787 = _str113;
    v788 = 0LL;
    assert_eq((void*)v788, v785, v786, v787);
    v790 = malloc(16);
    v791 = 1LL;
    v792 = (char*)v790 + 0;
    *(int64_t*)v792 = (int64_t)v791;
    v794 = _str114;
    v795 = (char*)v790 + 8;
    *(void**)v795 = (void*)v794;
    v797 = 0LL;
    v798 = shape_dim_to_str((void*)v797, v790);
    v799 = _str114;
    v800 = _str115;
    v801 = 0LL;
    assert_eq((void*)v801, v798, v799, v800);
    v803 = _str54;
    p_print_str(v803);
    v805 = _str116;
    p_print_str(v805);
    v807 = malloc(32);
    v808 = 0LL;
    v809 = (char*)v807 + 0;
    *(int64_t*)v809 = (int64_t)v808;
    v811 = 1LL;
    v812 = (char*)v807 + 8;
    *(int64_t*)v812 = (int64_t)v811;
    v814 = 8LL;
    v815 = p_malloc(v814);
    *(void**)v815 = (void*)v807;
    v817 = malloc(8);
    *(void**)v817 = (void*)v815;
    v819 = malloc(32);
    v820 = 0LL;
    v821 = (char*)v819 + 0;
    *(int64_t*)v821 = (int64_t)v820;
    v823 = 2LL;
    v824 = (char*)v819 + 8;
    *(int64_t*)v824 = (int64_t)v823;
    v826 = 8LL;
    v827 = p_malloc(v826);
    *(void**)v827 = (void*)v819;
    v829 = malloc(8);
    *(void**)v829 = (void*)v827;
    v831 = malloc(32);
    v832 = 5LL;
    v833 = (char*)v831 + 0;
    *(int64_t*)v833 = (int64_t)v832;
    v835 = malloc(8);
    v836 = 0LL;
    v837 = (char*)v835 + 0;
    *(int64_t*)v837 = (int64_t)v836;
    v839 = (char*)v831 + 8;
    *(void**)v839 = (void*)v835;
    v841 = *(void**)v817;
    v842 = (char*)v831 + 16;
    *(void**)v842 = (void*)v841;
    v844 = *(void**)v829;
    v845 = (char*)v831 + 24;
    *(void**)v845 = (void*)v844;
    v847 = malloc(8);
    *(void**)v847 = (void*)v831;
    v849 = *(void**)v847;
    v850 = (char*)v849 + 0;
    v851 = *(int64_t*)v850;
    v852 = 5LL;
    v853 = v851 == v852;
    if (v853) goto bb2;
    else goto bb3;
  bb1:
    v911 = _str54;
    p_print_str(v911);
    v913 = _str117;
    p_print_str(v913);
    v915 = malloc(32);
    v916 = 0LL;
    v917 = (char*)v915 + 0;
    *(int64_t*)v917 = (int64_t)v916;
    v919 = 3LL;
    v920 = (char*)v915 + 8;
    *(int64_t*)v920 = (int64_t)v919;
    v922 = 8LL;
    v923 = p_malloc(v922);
    *(void**)v923 = (void*)v915;
    v925 = malloc(8);
    *(void**)v925 = (void*)v923;
    v927 = malloc(32);
    v928 = 0LL;
    v929 = (char*)v927 + 0;
    *(int64_t*)v929 = (int64_t)v928;
    v931 = 4LL;
    v932 = (char*)v927 + 8;
    *(int64_t*)v932 = (int64_t)v931;
    v934 = 8LL;
    v935 = p_malloc(v934);
    *(void**)v935 = (void*)v927;
    v937 = malloc(8);
    *(void**)v937 = (void*)v935;
    v939 = malloc(32);
    v940 = 5LL;
    v941 = (char*)v939 + 0;
    *(int64_t*)v941 = (int64_t)v940;
    v943 = malloc(8);
    v944 = 0LL;
    v945 = (char*)v943 + 0;
    *(int64_t*)v945 = (int64_t)v944;
    v947 = (char*)v939 + 8;
    *(void**)v947 = (void*)v943;
    v949 = *(void**)v925;
    v950 = (char*)v939 + 16;
    *(void**)v950 = (void*)v949;
    v952 = *(void**)v937;
    v953 = (char*)v939 + 24;
    *(void**)v953 = (void*)v952;
    v955 = malloc(8);
    *(void**)v955 = (void*)v939;
    v957 = malloc(32);
    v958 = 5LL;
    v959 = (char*)v957 + 0;
    *(int64_t*)v959 = (int64_t)v958;
    v961 = malloc(8);
    v962 = 2LL;
    v963 = (char*)v961 + 0;
    *(int64_t*)v963 = (int64_t)v962;
    v965 = (char*)v957 + 8;
    *(void**)v965 = (void*)v961;
    v967 = *(void**)v847;
    v968 = 8LL;
    v969 = p_malloc(v968);
    *(void**)v969 = (void*)v967;
    v971 = (char*)v957 + 16;
    *(void**)v971 = (void*)v969;
    v973 = *(void**)v955;
    v974 = 8LL;
    v975 = p_malloc(v974);
    *(void**)v975 = (void*)v973;
    v977 = (char*)v957 + 24;
    *(void**)v977 = (void*)v975;
    v979 = malloc(8);
    *(void**)v979 = (void*)v957;
    v981 = *(void**)v979;
    v982 = 8LL;
    v983 = p_malloc(v982);
    *(void**)v983 = (void*)v981;
    v985 = 0LL;
    v986 = (int64_t)count_expr_nodes((void*)v985, v983);
    v987 = 7LL;
    v988 = _str118;
    v989 = 0LL;
    assert_eq_i64((void*)v989, v986, v987, v988);
    v991 = _str54;
    p_print_str(v991);
    v993 = _str119;
    p_print_str(v993);
    v995 = malloc(32);
    v996 = 2LL;
    v997 = (char*)v995 + 0;
    *(int64_t*)v997 = (int64_t)v996;
    v999 = _str120;
    v1000 = (char*)v995 + 8;
    *(void**)v1000 = (void*)v999;
    v1002 = (char*)v995 + 0;
    v1003 = *(int64_t*)v1002;
    v1004 = 2LL;
    v1005 = v1003 == v1004;
    if (v1005) goto bb17;
    else goto bb18;
  bb2:
    v855 = (char*)v849 + 8;
    v856 = *(void**)v855;
    v857 = (char*)v849 + 16;
    v858 = *(void**)v857;
    v859 = (char*)v849 + 24;
    v860 = *(void**)v859;
    v861 = 0LL;
    v862 = (int64_t)op_tag((void*)v861, v856);
    v863 = 0LL;
    v864 = _str121;
    v865 = 0LL;
    assert_eq_i64((void*)v865, v862, v863, v864);
    v867 = (char*)v858 + 0;
    v868 = *(void**)v867;
    v869 = (char*)v868 + 0;
    v870 = *(int64_t*)v869;
    v871 = 0LL;
    v872 = v870 == v871;
    if (v872) goto bb5;
    else goto bb6;
  bb3:
    goto bb14;
  bb4:
    v886 = (char*)v860 + 0;
    v887 = *(void**)v886;
    v888 = (char*)v887 + 0;
    v889 = *(int64_t*)v888;
    v890 = 0LL;
    v891 = v889 == v890;
    if (v891) goto bb10;
    else goto bb11;
  bb5:
    v874 = (char*)v868 + 8;
    v875 = *(int64_t*)v874;
    v876 = 1LL;
    v877 = _str122;
    v878 = 0LL;
    assert_eq_i64((void*)v878, v875, v876, v877);
    goto bb4;
  bb6:
    goto bb7;
  bb7:
    v882 = _str123;
    p_panic(v882);
    goto bb4;
  bb8:
    goto bb4;
  bb9:
    goto bb1;
  bb10:
    v893 = (char*)v887 + 8;
    v894 = *(int64_t*)v893;
    v895 = 2LL;
    v896 = _str124;
    v897 = 0LL;
    assert_eq_i64((void*)v897, v894, v895, v896);
    goto bb9;
  bb11:
    goto bb12;
  bb12:
    v901 = _str125;
    p_panic(v901);
    goto bb9;
  bb13:
    goto bb9;
  bb14:
    v907 = _str126;
    p_panic(v907);
    goto bb1;
  bb15:
    goto bb1;
  bb16:
    v1019 = malloc(32);
    v1020 = 3LL;
    v1021 = (char*)v1019 + 0;
    *(int64_t*)v1021 = (int64_t)v1020;
    v1023 = true;
    v1024 = (char*)v1019 + 8;
    *(bool*)v1024 = v1023;
    v1026 = (char*)v1019 + 0;
    v1027 = *(int64_t*)v1026;
    v1028 = 3LL;
    v1029 = v1027 == v1028;
    if (v1029) goto bb22;
    else goto bb23;
  bb17:
    v1007 = (char*)v995 + 8;
    v1008 = *(void**)v1007;
    v1009 = _str120;
    v1010 = _str127;
    v1011 = 0LL;
    assert_eq((void*)v1011, v1008, v1009, v1010);
    goto bb16;
  bb18:
    goto bb19;
  bb19:
    v1015 = _str128;
    p_panic(v1015);
    goto bb16;
  bb20:
    goto bb16;
  bb21:
    v1043 = malloc(32);
    v1044 = 1LL;
    v1045 = (char*)v1043 + 0;
    *(int64_t*)v1045 = (int64_t)v1044;
    v1047 = ((union { uint64_t u; double f; }){ .u = 4614253070214989087ULL }).f;
    v1048 = (char*)v1043 + 8;
    *(double*)v1048 = v1047;
    v1050 = (char*)v1043 + 0;
    v1051 = *(int64_t*)v1050;
    v1052 = 1LL;
    v1053 = v1051 == v1052;
    if (v1053) goto bb27;
    else goto bb28;
  bb22:
    v1031 = (char*)v1019 + 8;
    v1032 = *(bool*)v1031;
    v1033 = true;
    v1034 = _str129;
    v1035 = 0LL;
    assert_eq_bool((void*)v1035, v1032, v1033, v1034);
    goto bb21;
  bb23:
    goto bb24;
  bb24:
    v1039 = _str130;
    p_panic(v1039);
    goto bb21;
  bb25:
    goto bb21;
  bb26:
    v1085 = malloc(32);
    v1086 = 4LL;
    v1087 = (char*)v1085 + 0;
    *(int64_t*)v1087 = (int64_t)v1086;
    v1089 = _str131;
    v1090 = (char*)v1085 + 8;
    *(void**)v1090 = (void*)v1089;
    v1092 = (char*)v1085 + 0;
    v1093 = *(int64_t*)v1092;
    v1094 = 4LL;
    v1095 = v1093 == v1094;
    if (v1095) goto bb38;
    else goto bb39;
  bb27:
    v1055 = (char*)v1043 + 8;
    v1056 = *(double*)v1055;
    v1057 = ((union { uint64_t u; double f; }){ .u = 4614253070214989087ULL }).f;
    v1058 = v1056 - v1057;
    v1059 = malloc(8);
    *(double*)v1059 = v1058;
    v1061 = *(double*)v1059;
    v1062 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v1063 = v1061 < v1062;
    if (v1063) goto bb29;
    else goto bb30;
  bb28:
    goto bb35;
  bb29:
    v1065 = ((union { uint64_t u; double f; }){ .u = 0ULL }).f;
    v1066 = *(double*)v1059;
    v1067 = v1065 - v1066;
    *(double*)v1059 = v1067;
    goto bb31;
  bb30:
    goto bb31;
  bb31:
    v1071 = *(double*)v1059;
    v1072 = ((union { uint64_t u; double f; }){ .u = 4547007122018943789ULL }).f;
    v1073 = v1071 > v1072;
    if (v1073) goto bb32;
    else goto bb33;
  bb32:
    v1075 = _str132;
    p_panic(v1075);
    goto bb34;
  bb33:
    goto bb34;
  bb34:
    goto bb26;
  bb35:
    v1081 = _str133;
    p_panic(v1081);
    goto bb26;
  bb36:
    goto bb26;
  bb37:
    v1109 = 0LL;
    v1110 = Vec_new((void*)v1109);
    v1111 = malloc(8);
    *(void**)v1111 = (void*)v1110;
    v1113 = *(void**)v1111;
    v1114 = malloc(32);
    v1115 = 0LL;
    v1116 = (char*)v1114 + 0;
    *(int64_t*)v1116 = (int64_t)v1115;
    v1118 = 1LL;
    v1119 = (char*)v1114 + 8;
    *(int64_t*)v1119 = (int64_t)v1118;
    v1121 = 0LL;
    Vec_push((void*)v1121, v1113, (int64_t)v1114);
    v1123 = *(void**)v1111;
    v1124 = malloc(32);
    v1125 = 0LL;
    v1126 = (char*)v1124 + 0;
    *(int64_t*)v1126 = (int64_t)v1125;
    v1128 = 2LL;
    v1129 = (char*)v1124 + 8;
    *(int64_t*)v1129 = (int64_t)v1128;
    v1131 = 0LL;
    Vec_push((void*)v1131, v1123, (int64_t)v1124);
    v1133 = malloc(32);
    v1134 = 7LL;
    v1135 = (char*)v1133 + 0;
    *(int64_t*)v1135 = (int64_t)v1134;
    v1137 = _str134;
    v1138 = (char*)v1133 + 8;
    *(void**)v1138 = (void*)v1137;
    v1140 = *(void**)v1111;
    v1141 = (char*)v1133 + 16;
    *(void**)v1141 = (void*)v1140;
    v1143 = (char*)v1133 + 0;
    v1144 = *(int64_t*)v1143;
    v1145 = 7LL;
    v1146 = v1144 == v1145;
    if (v1146) goto bb43;
    else goto bb44;
  bb38:
    v1097 = (char*)v1085 + 8;
    v1098 = *(void**)v1097;
    v1099 = _str131;
    v1100 = _str135;
    v1101 = 0LL;
    assert_eq((void*)v1101, v1098, v1099, v1100);
    goto bb37;
  bb39:
    goto bb40;
  bb40:
    v1105 = _str136;
    p_panic(v1105);
    goto bb37;
  bb41:
    goto bb37;
  bb42:
    v1168 = _str54;
    p_print_str(v1168);
    v1170 = _str137;
    p_print_str(v1170);
    v1172 = malloc(32);
    v1173 = 8LL;
    v1174 = (char*)v1172 + 0;
    *(int64_t*)v1174 = (int64_t)v1173;
    v1176 = malloc(32);
    v1177 = 4LL;
    v1178 = (char*)v1176 + 0;
    *(int64_t*)v1178 = (int64_t)v1177;
    v1180 = _str138;
    v1181 = (char*)v1176 + 8;
    *(void**)v1181 = (void*)v1180;
    v1183 = 8LL;
    v1184 = p_malloc(v1183);
    *(void**)v1184 = (void*)v1176;
    v1186 = (char*)v1172 + 8;
    *(void**)v1186 = (void*)v1184;
    v1188 = _str139;
    v1189 = (char*)v1172 + 16;
    *(void**)v1189 = (void*)v1188;
    v1191 = (char*)v1172 + 0;
    v1192 = *(int64_t*)v1191;
    v1193 = 8LL;
    v1194 = v1192 == v1193;
    if (v1194) goto bb48;
    else goto bb49;
  bb43:
    v1148 = (char*)v1133 + 8;
    v1149 = *(void**)v1148;
    v1150 = (char*)v1133 + 16;
    v1151 = *(void**)v1150;
    v1152 = _str134;
    v1153 = _str140;
    v1154 = 0LL;
    assert_eq((void*)v1154, v1149, v1152, v1153);
    v1156 = 0LL;
    v1157 = (int64_t)Vec_len((void*)v1156, v1151);
    v1158 = 2LL;
    v1159 = _str141;
    v1160 = 0LL;
    assert_eq_i64((void*)v1160, v1157, v1158, v1159);
    goto bb42;
  bb44:
    goto bb45;
  bb45:
    v1164 = _str142;
    p_panic(v1164);
    goto bb42;
  bb46:
    goto bb42;
  bb47:
    v1229 = 0LL;
    v1230 = Vec_new((void*)v1229);
    v1231 = malloc(8);
    *(void**)v1231 = (void*)v1230;
    v1233 = *(void**)v1231;
    v1234 = malloc(32);
    v1235 = 0LL;
    v1236 = (char*)v1234 + 0;
    *(int64_t*)v1236 = (int64_t)v1235;
    v1238 = 10LL;
    v1239 = (char*)v1234 + 8;
    *(int64_t*)v1239 = (int64_t)v1238;
    v1241 = 0LL;
    Vec_push((void*)v1241, v1233, (int64_t)v1234);
    v1243 = malloc(32);
    v1244 = 14LL;
    v1245 = (char*)v1243 + 0;
    *(int64_t*)v1245 = (int64_t)v1244;
    v1247 = malloc(32);
    v1248 = 4LL;
    v1249 = (char*)v1247 + 0;
    *(int64_t*)v1249 = (int64_t)v1248;
    v1251 = _str143;
    v1252 = (char*)v1247 + 8;
    *(void**)v1252 = (void*)v1251;
    v1254 = 8LL;
    v1255 = p_malloc(v1254);
    *(void**)v1255 = (void*)v1247;
    v1257 = (char*)v1243 + 8;
    *(void**)v1257 = (void*)v1255;
    v1259 = _str144;
    v1260 = (char*)v1243 + 16;
    *(void**)v1260 = (void*)v1259;
    v1262 = *(void**)v1231;
    v1263 = (char*)v1243 + 24;
    *(void**)v1263 = (void*)v1262;
    v1265 = (char*)v1243 + 0;
    v1266 = *(int64_t*)v1265;
    v1267 = 14LL;
    v1268 = v1266 == v1267;
    if (v1268) goto bb58;
    else goto bb59;
  bb48:
    v1196 = (char*)v1172 + 8;
    v1197 = *(void**)v1196;
    v1198 = (char*)v1172 + 16;
    v1199 = *(void**)v1198;
    v1200 = _str139;
    v1201 = _str145;
    v1202 = 0LL;
    assert_eq((void*)v1202, v1199, v1200, v1201);
    v1204 = (char*)v1197 + 0;
    v1205 = *(void**)v1204;
    v1206 = (char*)v1205 + 0;
    v1207 = *(int64_t*)v1206;
    v1208 = 4LL;
    v1209 = v1207 == v1208;
    if (v1209) goto bb51;
    else goto bb52;
  bb49:
    goto bb55;
  bb50:
    goto bb47;
  bb51:
    v1211 = (char*)v1205 + 8;
    v1212 = *(void**)v1211;
    v1213 = _str138;
    v1214 = _str146;
    v1215 = 0LL;
    assert_eq((void*)v1215, v1212, v1213, v1214);
    goto bb50;
  bb52:
    goto bb53;
  bb53:
    v1219 = _str147;
    p_panic(v1219);
    goto bb50;
  bb54:
    goto bb50;
  bb55:
    v1225 = _str148;
    p_panic(v1225);
    goto bb47;
  bb56:
    goto bb47;
  bb57:
    v1311 = malloc(32);
    v1312 = 16LL;
    v1313 = (char*)v1311 + 0;
    *(int64_t*)v1313 = (int64_t)v1312;
    v1315 = malloc(32);
    v1316 = 4LL;
    v1317 = (char*)v1315 + 0;
    *(int64_t*)v1317 = (int64_t)v1316;
    v1319 = _str149;
    v1320 = (char*)v1315 + 8;
    *(void**)v1320 = (void*)v1319;
    v1322 = 8LL;
    v1323 = p_malloc(v1322);
    *(void**)v1323 = (void*)v1315;
    v1325 = (char*)v1311 + 8;
    *(void**)v1325 = (void*)v1323;
    v1327 = malloc(32);
    v1328 = 0LL;
    v1329 = (char*)v1327 + 0;
    *(int64_t*)v1329 = (int64_t)v1328;
    v1331 = 0LL;
    v1332 = (char*)v1327 + 8;
    *(int64_t*)v1332 = (int64_t)v1331;
    v1334 = 8LL;
    v1335 = p_malloc(v1334);
    *(void**)v1335 = (void*)v1327;
    v1337 = (char*)v1311 + 16;
    *(void**)v1337 = (void*)v1335;
    v1339 = (char*)v1311 + 0;
    v1340 = *(int64_t*)v1339;
    v1341 = 16LL;
    v1342 = v1340 == v1341;
    if (v1342) goto bb68;
    else goto bb69;
  bb58:
    v1270 = (char*)v1243 + 8;
    v1271 = *(void**)v1270;
    v1272 = (char*)v1243 + 16;
    v1273 = *(void**)v1272;
    v1274 = (char*)v1243 + 24;
    v1275 = *(void**)v1274;
    v1276 = _str144;
    v1277 = _str150;
    v1278 = 0LL;
    assert_eq((void*)v1278, v1273, v1276, v1277);
    v1280 = 0LL;
    v1281 = (int64_t)Vec_len((void*)v1280, v1275);
    v1282 = 1LL;
    v1283 = _str151;
    v1284 = 0LL;
    assert_eq_i64((void*)v1284, v1281, v1282, v1283);
    v1286 = (char*)v1271 + 0;
    v1287 = *(void**)v1286;
    v1288 = (char*)v1287 + 0;
    v1289 = *(int64_t*)v1288;
    v1290 = 4LL;
    v1291 = v1289 == v1290;
    if (v1291) goto bb61;
    else goto bb62;
  bb59:
    goto bb65;
  bb60:
    goto bb57;
  bb61:
    v1293 = (char*)v1287 + 8;
    v1294 = *(void**)v1293;
    v1295 = _str143;
    v1296 = _str152;
    v1297 = 0LL;
    assert_eq((void*)v1297, v1294, v1295, v1296);
    goto bb60;
  bb62:
    goto bb63;
  bb63:
    v1301 = _str147;
    p_panic(v1301);
    goto bb60;
  bb64:
    goto bb60;
  bb65:
    v1307 = _str153;
    p_panic(v1307);
    goto bb57;
  bb66:
    goto bb57;
  bb67:
    v1392 = _str54;
    p_print_str(v1392);
    v1394 = _str154;
    p_print_str(v1394);
    v1396 = 0LL;
    v1397 = Vec_new((void*)v1396);
    v1398 = malloc(8);
    *(void**)v1398 = (void*)v1397;
    v1400 = *(void**)v1398;
    v1401 = _str139;
    v1402 = malloc(32);
    v1403 = 0LL;
    v1404 = (char*)v1402 + 0;
    *(int64_t*)v1404 = (int64_t)v1403;
    v1406 = 1LL;
    v1407 = (char*)v1402 + 8;
    *(int64_t*)v1407 = (int64_t)v1406;
    v1409 = malloc(40);
    v1410 = 0LL;
    FieldInit_new(v1409, (void*)v1410, v1401, v1402);
    v1412 = 0LL;
    Vec_push((void*)v1412, v1400, (int64_t)v1409);
    v1414 = *(void**)v1398;
    v1415 = _str155;
    v1416 = malloc(32);
    v1417 = 0LL;
    v1418 = (char*)v1416 + 0;
    *(int64_t*)v1418 = (int64_t)v1417;
    v1420 = 2LL;
    v1421 = (char*)v1416 + 8;
    *(int64_t*)v1421 = (int64_t)v1420;
    v1423 = malloc(40);
    v1424 = 0LL;
    FieldInit_new(v1423, (void*)v1424, v1415, v1416);
    v1426 = 0LL;
    Vec_push((void*)v1426, v1414, (int64_t)v1423);
    v1428 = malloc(32);
    v1429 = 10LL;
    v1430 = (char*)v1428 + 0;
    *(int64_t*)v1430 = (int64_t)v1429;
    v1432 = _str56;
    v1433 = (char*)v1428 + 8;
    *(void**)v1433 = (void*)v1432;
    v1435 = *(void**)v1398;
    v1436 = (char*)v1428 + 16;
    *(void**)v1436 = (void*)v1435;
    v1438 = malloc(16);
    v1439 = 1LL;
    v1440 = (char*)v1438 + 0;
    *(int64_t*)v1440 = (int64_t)v1439;
    v1442 = (char*)v1428 + 24;
    *(void**)v1442 = (void*)v1438;
    v1444 = (char*)v1428 + 0;
    v1445 = *(int64_t*)v1444;
    v1446 = 10LL;
    v1447 = v1445 == v1446;
    if (v1447) goto bb83;
    else goto bb84;
  bb68:
    v1344 = (char*)v1311 + 8;
    v1345 = *(void**)v1344;
    v1346 = (char*)v1311 + 16;
    v1347 = *(void**)v1346;
    v1348 = (char*)v1347 + 0;
    v1349 = *(void**)v1348;
    v1350 = (char*)v1349 + 0;
    v1351 = *(int64_t*)v1350;
    v1352 = 0LL;
    v1353 = v1351 == v1352;
    if (v1353) goto bb71;
    else goto bb72;
  bb69:
    goto bb80;
  bb70:
    v1367 = (char*)v1345 + 0;
    v1368 = *(void**)v1367;
    v1369 = (char*)v1368 + 0;
    v1370 = *(int64_t*)v1369;
    v1371 = 4LL;
    v1372 = v1370 == v1371;
    if (v1372) goto bb76;
    else goto bb77;
  bb71:
    v1355 = (char*)v1349 + 8;
    v1356 = *(int64_t*)v1355;
    v1357 = 0LL;
    v1358 = _str156;
    v1359 = 0LL;
    assert_eq_i64((void*)v1359, v1356, v1357, v1358);
    goto bb70;
  bb72:
    goto bb73;
  bb73:
    v1363 = _str157;
    p_panic(v1363);
    goto bb70;
  bb74:
    goto bb70;
  bb75:
    goto bb67;
  bb76:
    v1374 = (char*)v1368 + 8;
    v1375 = *(void**)v1374;
    v1376 = _str149;
    v1377 = _str158;
    v1378 = 0LL;
    assert_eq((void*)v1378, v1375, v1376, v1377);
    goto bb75;
  bb77:
    goto bb78;
  bb78:
    v1382 = _str159;
    p_panic(v1382);
    goto bb75;
  bb79:
    goto bb75;
  bb80:
    v1388 = _str160;
    p_panic(v1388);
    goto bb67;
  bb81:
    goto bb67;
  bb82:
    v1513 = _str54;
    p_print_str(v1513);
    v1515 = _str161;
    p_print_str(v1515);
    v1517 = malloc(64);
    v1518 = 0LL;
    v1519 = (char*)v1517 + 0;
    *(int64_t*)v1519 = (int64_t)v1518;
    v1521 = _str139;
    v1522 = (char*)v1517 + 8;
    *(void**)v1522 = (void*)v1521;
    v1524 = malloc(16);
    v1525 = 1LL;
    v1526 = (char*)v1524 + 0;
    *(int64_t*)v1526 = (int64_t)v1525;
    v1528 = (char*)v1517 + 16;
    *(void**)v1528 = (void*)v1524;
    v1530 = malloc(32);
    v1531 = 0LL;
    v1532 = (char*)v1530 + 0;
    *(int64_t*)v1532 = (int64_t)v1531;
    v1534 = 42LL;
    v1535 = (char*)v1530 + 8;
    *(int64_t*)v1535 = (int64_t)v1534;
    v1537 = (char*)v1517 + 24;
    *(void**)v1537 = (void*)v1530;
    v1539 = false;
    v1540 = (char*)v1517 + 32;
    *(bool*)v1540 = v1539;
    v1542 = false;
    v1543 = (char*)v1517 + 40;
    *(bool*)v1543 = v1542;
    v1545 = 10LL;
    v1546 = (char*)v1517 + 48;
    *(int64_t*)v1546 = (int64_t)v1545;
    v1548 = malloc(8);
    *(void**)v1548 = (void*)v1517;
    v1550 = *(void**)v1548;
    v1551 = 0LL;
    v1552 = (int64_t)stmt_line((void*)v1551, v1550);
    v1553 = 10LL;
    v1554 = _str162;
    v1555 = 0LL;
    assert_eq_i64((void*)v1555, v1552, v1553, v1554);
    v1557 = malloc(64);
    v1558 = 3LL;
    v1559 = (char*)v1557 + 0;
    *(int64_t*)v1559 = (int64_t)v1558;
    v1561 = _str139;
    v1562 = (char*)v1557 + 8;
    *(void**)v1562 = (void*)v1561;
    v1564 = malloc(8);
    v1565 = 0LL;
    v1566 = (char*)v1564 + 0;
    *(int64_t*)v1566 = (int64_t)v1565;
    v1568 = (char*)v1557 + 16;
    *(void**)v1568 = (void*)v1564;
    v1570 = malloc(32);
    v1571 = 0LL;
    v1572 = (char*)v1570 + 0;
    *(int64_t*)v1572 = (int64_t)v1571;
    v1574 = 5LL;
    v1575 = (char*)v1570 + 8;
    *(int64_t*)v1575 = (int64_t)v1574;
    v1577 = (char*)v1557 + 24;
    *(void**)v1577 = (void*)v1570;
    v1579 = 20LL;
    v1580 = (char*)v1557 + 32;
    *(int64_t*)v1580 = (int64_t)v1579;
    v1582 = malloc(8);
    *(void**)v1582 = (void*)v1557;
    v1584 = *(void**)v1582;
    v1585 = 0LL;
    v1586 = (int64_t)stmt_line((void*)v1585, v1584);
    v1587 = 20LL;
    v1588 = _str163;
    v1589 = 0LL;
    assert_eq_i64((void*)v1589, v1586, v1587, v1588);
    v1591 = malloc(64);
    v1592 = 8LL;
    v1593 = (char*)v1591 + 0;
    *(int64_t*)v1593 = (int64_t)v1592;
    v1595 = malloc(16);
    v1596 = 0LL;
    v1597 = (char*)v1595 + 0;
    *(int64_t*)v1597 = (int64_t)v1596;
    v1599 = malloc(32);
    v1600 = 0LL;
    v1601 = (char*)v1599 + 0;
    *(int64_t*)v1601 = (int64_t)v1600;
    v1603 = 0LL;
    v1604 = (char*)v1599 + 8;
    *(int64_t*)v1604 = (int64_t)v1603;
    v1606 = 8LL;
    v1607 = p_malloc(v1606);
    *(void**)v1607 = (void*)v1599;
    v1609 = (char*)v1595 + 8;
    *(void**)v1609 = (void*)v1607;
    v1611 = (char*)v1591 + 8;
    *(void**)v1611 = (void*)v1595;
    v1613 = 30LL;
    v1614 = (char*)v1591 + 16;
    *(int64_t*)v1614 = (int64_t)v1613;
    v1616 = malloc(8);
    *(void**)v1616 = (void*)v1591;
    v1618 = *(void**)v1616;
    v1619 = 0LL;
    v1620 = (int64_t)stmt_line((void*)v1619, v1618);
    v1621 = 30LL;
    v1622 = _str164;
    v1623 = 0LL;
    assert_eq_i64((void*)v1623, v1620, v1621, v1622);
    v1625 = malloc(64);
    v1626 = 8LL;
    v1627 = (char*)v1625 + 0;
    *(int64_t*)v1627 = (int64_t)v1626;
    v1629 = malloc(16);
    v1630 = 1LL;
    v1631 = (char*)v1629 + 0;
    *(int64_t*)v1631 = (int64_t)v1630;
    v1633 = (char*)v1625 + 8;
    *(void**)v1633 = (void*)v1629;
    v1635 = 31LL;
    v1636 = (char*)v1625 + 16;
    *(int64_t*)v1636 = (int64_t)v1635;
    v1638 = malloc(8);
    *(void**)v1638 = (void*)v1625;
    v1640 = *(void**)v1638;
    v1641 = 0LL;
    v1642 = (int64_t)stmt_line((void*)v1641, v1640);
    v1643 = 31LL;
    v1644 = _str165;
    v1645 = 0LL;
    assert_eq_i64((void*)v1645, v1642, v1643, v1644);
    v1647 = malloc(64);
    v1648 = 14LL;
    v1649 = (char*)v1647 + 0;
    *(int64_t*)v1649 = (int64_t)v1648;
    v1651 = 40LL;
    v1652 = (char*)v1647 + 8;
    *(int64_t*)v1652 = (int64_t)v1651;
    v1654 = malloc(8);
    *(void**)v1654 = (void*)v1647;
    v1656 = *(void**)v1654;
    v1657 = 0LL;
    v1658 = (int64_t)stmt_line((void*)v1657, v1656);
    v1659 = 40LL;
    v1660 = _str166;
    v1661 = 0LL;
    assert_eq_i64((void*)v1661, v1658, v1659, v1660);
    v1663 = malloc(64);
    v1664 = 7LL;
    v1665 = (char*)v1663 + 0;
    *(int64_t*)v1665 = (int64_t)v1664;
    v1667 = malloc(32);
    v1668 = 0LL;
    v1669 = (char*)v1667 + 0;
    *(int64_t*)v1669 = (int64_t)v1668;
    v1671 = 1LL;
    v1672 = (char*)v1667 + 8;
    *(int64_t*)v1672 = (int64_t)v1671;
    v1674 = (char*)v1663 + 8;
    *(void**)v1674 = (void*)v1667;
    v1676 = 50LL;
    v1677 = (char*)v1663 + 16;
    *(int64_t*)v1677 = (int64_t)v1676;
    v1679 = malloc(8);
    *(void**)v1679 = (void*)v1663;
    v1681 = *(void**)v1679;
    v1682 = 0LL;
    v1683 = (int64_t)stmt_line((void*)v1682, v1681);
    v1684 = 50LL;
    v1685 = _str167;
    v1686 = 0LL;
    assert_eq_i64((void*)v1686, v1683, v1684, v1685);
    v1688 = malloc(64);
    v1689 = 10LL;
    v1690 = (char*)v1688 + 0;
    *(int64_t*)v1690 = (int64_t)v1689;
    v1692 = malloc(32);
    v1693 = 3LL;
    v1694 = (char*)v1692 + 0;
    *(int64_t*)v1694 = (int64_t)v1693;
    v1696 = true;
    v1697 = (char*)v1692 + 8;
    *(bool*)v1697 = v1696;
    v1699 = (char*)v1688 + 8;
    *(void**)v1699 = (void*)v1692;
    v1701 = 0LL;
    v1702 = Vec_new((void*)v1701);
    v1703 = (char*)v1688 + 16;
    *(void**)v1703 = (void*)v1702;
    v1705 = 60LL;
    v1706 = (char*)v1688 + 24;
    *(int64_t*)v1706 = (int64_t)v1705;
    v1708 = malloc(8);
    *(void**)v1708 = (void*)v1688;
    v1710 = *(void**)v1708;
    v1711 = 0LL;
    v1712 = (int64_t)stmt_line((void*)v1711, v1710);
    v1713 = 60LL;
    v1714 = _str168;
    v1715 = 0LL;
    assert_eq_i64((void*)v1715, v1712, v1713, v1714);
    v1717 = malloc(64);
    v1718 = 11LL;
    v1719 = (char*)v1717 + 0;
    *(int64_t*)v1719 = (int64_t)v1718;
    v1721 = _str169;
    v1722 = (char*)v1717 + 8;
    *(void**)v1722 = (void*)v1721;
    v1724 = malloc(32);
    v1725 = 4LL;
    v1726 = (char*)v1724 + 0;
    *(int64_t*)v1726 = (int64_t)v1725;
    v1728 = _str170;
    v1729 = (char*)v1724 + 8;
    *(void**)v1729 = (void*)v1728;
    v1731 = (char*)v1717 + 16;
    *(void**)v1731 = (void*)v1724;
    v1733 = 0LL;
    v1734 = Vec_new((void*)v1733);
    v1735 = (char*)v1717 + 24;
    *(void**)v1735 = (void*)v1734;
    v1737 = 70LL;
    v1738 = (char*)v1717 + 32;
    *(int64_t*)v1738 = (int64_t)v1737;
    v1740 = malloc(8);
    *(void**)v1740 = (void*)v1717;
    v1742 = *(void**)v1740;
    v1743 = 0LL;
    v1744 = (int64_t)stmt_line((void*)v1743, v1742);
    v1745 = 70LL;
    v1746 = _str171;
    v1747 = 0LL;
    assert_eq_i64((void*)v1747, v1744, v1745, v1746);
    v1749 = malloc(64);
    v1750 = 12LL;
    v1751 = (char*)v1749 + 0;
    *(int64_t*)v1751 = (int64_t)v1750;
    v1753 = 0LL;
    v1754 = Vec_new((void*)v1753);
    v1755 = (char*)v1749 + 8;
    *(void**)v1755 = (void*)v1754;
    v1757 = 80LL;
    v1758 = (char*)v1749 + 16;
    *(int64_t*)v1758 = (int64_t)v1757;
    v1760 = malloc(8);
    *(void**)v1760 = (void*)v1749;
    v1762 = *(void**)v1760;
    v1763 = 0LL;
    v1764 = (int64_t)stmt_line((void*)v1763, v1762);
    v1765 = 80LL;
    v1766 = _str172;
    v1767 = 0LL;
    assert_eq_i64((void*)v1767, v1764, v1765, v1766);
    v1769 = malloc(64);
    v1770 = 13LL;
    v1771 = (char*)v1769 + 0;
    *(int64_t*)v1771 = (int64_t)v1770;
    v1773 = malloc(16);
    v1774 = 1LL;
    v1775 = (char*)v1773 + 0;
    *(int64_t*)v1775 = (int64_t)v1774;
    v1777 = (char*)v1769 + 8;
    *(void**)v1777 = (void*)v1773;
    v1779 = 90LL;
    v1780 = (char*)v1769 + 16;
    *(int64_t*)v1780 = (int64_t)v1779;
    v1782 = malloc(8);
    *(void**)v1782 = (void*)v1769;
    v1784 = *(void**)v1782;
    v1785 = 0LL;
    v1786 = (int64_t)stmt_line((void*)v1785, v1784);
    v1787 = 90LL;
    v1788 = _str173;
    v1789 = 0LL;
    assert_eq_i64((void*)v1789, v1786, v1787, v1788);
    v1791 = malloc(64);
    v1792 = 15LL;
    v1793 = (char*)v1791 + 0;
    *(int64_t*)v1793 = (int64_t)v1792;
    v1795 = 0LL;
    v1796 = Vec_new((void*)v1795);
    v1797 = (char*)v1791 + 8;
    *(void**)v1797 = (void*)v1796;
    v1799 = 100LL;
    v1800 = (char*)v1791 + 16;
    *(int64_t*)v1800 = (int64_t)v1799;
    v1802 = malloc(8);
    *(void**)v1802 = (void*)v1791;
    v1804 = *(void**)v1802;
    v1805 = 0LL;
    v1806 = (int64_t)stmt_line((void*)v1805, v1804);
    v1807 = 100LL;
    v1808 = _str174;
    v1809 = 0LL;
    assert_eq_i64((void*)v1809, v1806, v1807, v1808);
    v1811 = 0LL;
    v1812 = Vec_new((void*)v1811);
    v1813 = malloc(8);
    *(void**)v1813 = (void*)v1812;
    v1815 = *(void**)v1813;
    v1816 = _str131;
    v1817 = 0LL;
    Vec_push((void*)v1817, v1815, (int64_t)v1816);
    v1819 = malloc(64);
    v1820 = 17LL;
    v1821 = (char*)v1819 + 0;
    *(int64_t*)v1821 = (int64_t)v1820;
    v1823 = _str175;
    v1824 = (char*)v1819 + 8;
    *(void**)v1824 = (void*)v1823;
    v1826 = *(void**)v1813;
    v1827 = (char*)v1819 + 16;
    *(void**)v1827 = (void*)v1826;
    v1829 = 110LL;
    v1830 = (char*)v1819 + 24;
    *(int64_t*)v1830 = (int64_t)v1829;
    v1832 = malloc(8);
    *(void**)v1832 = (void*)v1819;
    v1834 = *(void**)v1832;
    v1835 = 0LL;
    v1836 = (int64_t)stmt_line((void*)v1835, v1834);
    v1837 = 110LL;
    v1838 = _str176;
    v1839 = 0LL;
    assert_eq_i64((void*)v1839, v1836, v1837, v1838);
    v1841 = malloc(64);
    v1842 = 21LL;
    v1843 = (char*)v1841 + 0;
    *(int64_t*)v1843 = (int64_t)v1842;
    v1845 = 0LL;
    v1846 = Vec_new((void*)v1845);
    v1847 = (char*)v1841 + 8;
    *(void**)v1847 = (void*)v1846;
    v1849 = 120LL;
    v1850 = (char*)v1841 + 16;
    *(int64_t*)v1850 = (int64_t)v1849;
    v1852 = malloc(8);
    *(void**)v1852 = (void*)v1841;
    v1854 = *(void**)v1852;
    v1855 = 0LL;
    v1856 = (int64_t)stmt_line((void*)v1855, v1854);
    v1857 = 120LL;
    v1858 = _str177;
    v1859 = 0LL;
    assert_eq_i64((void*)v1859, v1856, v1857, v1858);
    v1861 = malloc(64);
    v1862 = 22LL;
    v1863 = (char*)v1861 + 0;
    *(int64_t*)v1863 = (int64_t)v1862;
    v1865 = 0LL;
    v1866 = Vec_new((void*)v1865);
    v1867 = (char*)v1861 + 8;
    *(void**)v1867 = (void*)v1866;
    v1869 = 130LL;
    v1870 = (char*)v1861 + 16;
    *(int64_t*)v1870 = (int64_t)v1869;
    v1872 = malloc(8);
    *(void**)v1872 = (void*)v1861;
    v1874 = *(void**)v1872;
    v1875 = 0LL;
    v1876 = (int64_t)stmt_line((void*)v1875, v1874);
    v1877 = 130LL;
    v1878 = _str178;
    v1879 = 0LL;
    assert_eq_i64((void*)v1879, v1876, v1877, v1878);
    v1881 = malloc(64);
    v1882 = 23LL;
    v1883 = (char*)v1881 + 0;
    *(int64_t*)v1883 = (int64_t)v1882;
    v1885 = 0LL;
    v1886 = Vec_new((void*)v1885);
    v1887 = (char*)v1881 + 8;
    *(void**)v1887 = (void*)v1886;
    v1889 = 140LL;
    v1890 = (char*)v1881 + 16;
    *(int64_t*)v1890 = (int64_t)v1889;
    v1892 = malloc(8);
    *(void**)v1892 = (void*)v1881;
    v1894 = *(void**)v1892;
    v1895 = 0LL;
    v1896 = (int64_t)stmt_line((void*)v1895, v1894);
    v1897 = 140LL;
    v1898 = _str179;
    v1899 = 0LL;
    assert_eq_i64((void*)v1899, v1896, v1897, v1898);
    v1901 = malloc(64);
    v1902 = 20LL;
    v1903 = (char*)v1901 + 0;
    *(int64_t*)v1903 = (int64_t)v1902;
    v1905 = _str180;
    v1906 = (char*)v1901 + 8;
    *(void**)v1906 = (void*)v1905;
    v1908 = malloc(16);
    v1909 = 0LL;
    v1910 = (char*)v1908 + 0;
    *(int64_t*)v1910 = (int64_t)v1909;
    v1912 = 4096LL;
    v1913 = (char*)v1908 + 8;
    *(int64_t*)v1913 = (int64_t)v1912;
    v1915 = (char*)v1901 + 16;
    *(void**)v1915 = (void*)v1908;
    v1917 = false;
    v1918 = (char*)v1901 + 24;
    *(bool*)v1918 = v1917;
    v1920 = false;
    v1921 = (char*)v1901 + 32;
    *(bool*)v1921 = v1920;
    v1923 = malloc(16);
    v1924 = 1LL;
    v1925 = (char*)v1923 + 0;
    *(int64_t*)v1925 = (int64_t)v1924;
    v1927 = (char*)v1901 + 40;
    *(void**)v1927 = (void*)v1923;
    v1929 = 0LL;
    v1930 = Vec_new((void*)v1929);
    v1931 = (char*)v1901 + 48;
    *(void**)v1931 = (void*)v1930;
    v1933 = 150LL;
    v1934 = (char*)v1901 + 56;
    *(int64_t*)v1934 = (int64_t)v1933;
    v1936 = malloc(8);
    *(void**)v1936 = (void*)v1901;
    v1938 = *(void**)v1936;
    v1939 = 0LL;
    v1940 = (int64_t)stmt_line((void*)v1939, v1938);
    v1941 = 150LL;
    v1942 = _str181;
    v1943 = 0LL;
    assert_eq_i64((void*)v1943, v1940, v1941, v1942);
    v1945 = malloc(64);
    v1946 = 25LL;
    v1947 = (char*)v1945 + 0;
    *(int64_t*)v1947 = (int64_t)v1946;
    v1949 = _str139;
    v1950 = (char*)v1945 + 8;
    *(void**)v1950 = (void*)v1949;
    v1952 = 0LL;
    v1953 = Vec_new((void*)v1952);
    v1954 = (char*)v1945 + 16;
    *(void**)v1954 = (void*)v1953;
    v1956 = 160LL;
    v1957 = (char*)v1945 + 24;
    *(int64_t*)v1957 = (int64_t)v1956;
    v1959 = malloc(8);
    *(void**)v1959 = (void*)v1945;
    v1961 = *(void**)v1959;
    v1962 = 0LL;
    v1963 = (int64_t)stmt_line((void*)v1962, v1961);
    v1964 = 160LL;
    v1965 = _str182;
    v1966 = 0LL;
    assert_eq_i64((void*)v1966, v1963, v1964, v1965);
    v1968 = malloc(64);
    v1969 = 26LL;
    v1970 = (char*)v1968 + 0;
    *(int64_t*)v1970 = (int64_t)v1969;
    v1972 = malloc(32);
    v1973 = 4LL;
    v1974 = (char*)v1972 + 0;
    *(int64_t*)v1974 = (int64_t)v1973;
    v1976 = _str183;
    v1977 = (char*)v1972 + 8;
    *(void**)v1977 = (void*)v1976;
    v1979 = (char*)v1968 + 8;
    *(void**)v1979 = (void*)v1972;
    v1981 = 170LL;
    v1982 = (char*)v1968 + 16;
    *(int64_t*)v1982 = (int64_t)v1981;
    v1984 = malloc(8);
    *(void**)v1984 = (void*)v1968;
    v1986 = *(void**)v1984;
    v1987 = 0LL;
    v1988 = (int64_t)stmt_line((void*)v1987, v1986);
    v1989 = 170LL;
    v1990 = _str184;
    v1991 = 0LL;
    assert_eq_i64((void*)v1991, v1988, v1989, v1990);
    v1993 = 0LL;
    v1994 = Vec_new((void*)v1993);
    v1995 = malloc(8);
    *(void**)v1995 = (void*)v1994;
    v1997 = *(void**)v1995;
    v1998 = _str185;
    v1999 = 0LL;
    Vec_push((void*)v1999, v1997, (int64_t)v1998);
    v2001 = malloc(64);
    v2002 = 27LL;
    v2003 = (char*)v2001 + 0;
    *(int64_t*)v2003 = (int64_t)v2002;
    v2005 = *(void**)v1995;
    v2006 = (char*)v2001 + 8;
    *(void**)v2006 = (void*)v2005;
    v2008 = 0LL;
    v2009 = Vec_new((void*)v2008);
    v2010 = (char*)v2001 + 16;
    *(void**)v2010 = (void*)v2009;
    v2012 = 180LL;
    v2013 = (char*)v2001 + 24;
    *(int64_t*)v2013 = (int64_t)v2012;
    v2015 = malloc(8);
    *(void**)v2015 = (void*)v2001;
    v2017 = *(void**)v2015;
    v2018 = 0LL;
    v2019 = (int64_t)stmt_line((void*)v2018, v2017);
    v2020 = 180LL;
    v2021 = _str186;
    v2022 = 0LL;
    assert_eq_i64((void*)v2022, v2019, v2020, v2021);
    v2024 = _str54;
    p_print_str(v2024);
    v2026 = _str187;
    p_print_str(v2026);
    v2028 = 0LL;
    v2029 = Vec_new((void*)v2028);
    v2030 = malloc(8);
    *(void**)v2030 = (void*)v2029;
    v2032 = *(void**)v2030;
    v2033 = _str188;
    v2034 = 0LL;
    Vec_push((void*)v2034, v2032, (int64_t)v2033);
    v2036 = *(void**)v2030;
    v2037 = _str189;
    v2038 = 0LL;
    Vec_push((void*)v2038, v2036, (int64_t)v2037);
    v2040 = malloc(64);
    v2041 = 1LL;
    v2042 = (char*)v2040 + 0;
    *(int64_t*)v2042 = (int64_t)v2041;
    v2044 = *(void**)v2030;
    v2045 = (char*)v2040 + 8;
    *(void**)v2045 = (void*)v2044;
    v2047 = malloc(32);
    v2048 = 4LL;
    v2049 = (char*)v2047 + 0;
    *(int64_t*)v2049 = (int64_t)v2048;
    v2051 = _str190;
    v2052 = (char*)v2047 + 8;
    *(void**)v2052 = (void*)v2051;
    v2054 = (char*)v2040 + 16;
    *(void**)v2054 = (void*)v2047;
    v2056 = 200LL;
    v2057 = (char*)v2040 + 24;
    *(int64_t*)v2057 = (int64_t)v2056;
    v2059 = malloc(8);
    *(void**)v2059 = (void*)v2040;
    v2061 = *(void**)v2059;
    v2062 = 0LL;
    v2063 = (int64_t)stmt_line((void*)v2062, v2061);
    v2064 = 200LL;
    v2065 = _str191;
    v2066 = 0LL;
    assert_eq_i64((void*)v2066, v2063, v2064, v2065);
    v2068 = malloc(64);
    v2069 = 2LL;
    v2070 = (char*)v2068 + 0;
    *(int64_t*)v2070 = (int64_t)v2069;
    v2072 = _str139;
    v2073 = (char*)v2068 + 8;
    *(void**)v2073 = (void*)v2072;
    v2075 = malloc(40);
    v2076 = 3LL;
    v2077 = (char*)v2075 + 0;
    *(int64_t*)v2077 = (int64_t)v2076;
    v2079 = (char*)v2068 + 16;
    *(void**)v2079 = (void*)v2075;
    v2081 = 210LL;
    v2082 = (char*)v2068 + 24;
    *(int64_t*)v2082 = (int64_t)v2081;
    v2084 = malloc(8);
    *(void**)v2084 = (void*)v2068;
    v2086 = *(void**)v2084;
    v2087 = 0LL;
    v2088 = (int64_t)stmt_line((void*)v2087, v2086);
    v2089 = 210LL;
    v2090 = _str192;
    v2091 = 0LL;
    assert_eq_i64((void*)v2091, v2088, v2089, v2090);
    v2093 = malloc(64);
    v2094 = 4LL;
    v2095 = (char*)v2093 + 0;
    *(int64_t*)v2095 = (int64_t)v2094;
    v2097 = _str149;
    v2098 = (char*)v2093 + 8;
    *(void**)v2098 = (void*)v2097;
    v2100 = malloc(32);
    v2101 = 0LL;
    v2102 = (char*)v2100 + 0;
    *(int64_t*)v2102 = (int64_t)v2101;
    v2104 = 0LL;
    v2105 = (char*)v2100 + 8;
    *(int64_t*)v2105 = (int64_t)v2104;
    v2107 = (char*)v2093 + 16;
    *(void**)v2107 = (void*)v2100;
    v2109 = malloc(8);
    v2110 = 0LL;
    v2111 = (char*)v2109 + 0;
    *(int64_t*)v2111 = (int64_t)v2110;
    v2113 = (char*)v2093 + 24;
    *(void**)v2113 = (void*)v2109;
    v2115 = malloc(32);
    v2116 = 0LL;
    v2117 = (char*)v2115 + 0;
    *(int64_t*)v2117 = (int64_t)v2116;
    v2119 = 9LL;
    v2120 = (char*)v2115 + 8;
    *(int64_t*)v2120 = (int64_t)v2119;
    v2122 = (char*)v2093 + 32;
    *(void**)v2122 = (void*)v2115;
    v2124 = 220LL;
    v2125 = (char*)v2093 + 40;
    *(int64_t*)v2125 = (int64_t)v2124;
    v2127 = malloc(8);
    *(void**)v2127 = (void*)v2093;
    v2129 = *(void**)v2127;
    v2130 = 0LL;
    v2131 = (int64_t)stmt_line((void*)v2130, v2129);
    v2132 = 220LL;
    v2133 = _str193;
    v2134 = 0LL;
    assert_eq_i64((void*)v2134, v2131, v2132, v2133);
    v2136 = malloc(64);
    v2137 = 5LL;
    v2138 = (char*)v2136 + 0;
    *(int64_t*)v2138 = (int64_t)v2137;
    v2140 = malloc(32);
    v2141 = 4LL;
    v2142 = (char*)v2140 + 0;
    *(int64_t*)v2142 = (int64_t)v2141;
    v2144 = _str194;
    v2145 = (char*)v2140 + 8;
    *(void**)v2145 = (void*)v2144;
    v2147 = (char*)v2136 + 8;
    *(void**)v2147 = (void*)v2140;
    v2149 = malloc(8);
    v2150 = 0LL;
    v2151 = (char*)v2149 + 0;
    *(int64_t*)v2151 = (int64_t)v2150;
    v2153 = (char*)v2136 + 16;
    *(void**)v2153 = (void*)v2149;
    v2155 = malloc(32);
    v2156 = 0LL;
    v2157 = (char*)v2155 + 0;
    *(int64_t*)v2157 = (int64_t)v2156;
    v2159 = 7LL;
    v2160 = (char*)v2155 + 8;
    *(int64_t*)v2160 = (int64_t)v2159;
    v2162 = (char*)v2136 + 24;
    *(void**)v2162 = (void*)v2155;
    v2164 = 230LL;
    v2165 = (char*)v2136 + 32;
    *(int64_t*)v2165 = (int64_t)v2164;
    v2167 = malloc(8);
    *(void**)v2167 = (void*)v2136;
    v2169 = *(void**)v2167;
    v2170 = 0LL;
    v2171 = (int64_t)stmt_line((void*)v2170, v2169);
    v2172 = 230LL;
    v2173 = _str195;
    v2174 = 0LL;
    assert_eq_i64((void*)v2174, v2171, v2172, v2173);
    v2176 = malloc(64);
    v2177 = 6LL;
    v2178 = (char*)v2176 + 0;
    *(int64_t*)v2178 = (int64_t)v2177;
    v2180 = malloc(32);
    v2181 = 4LL;
    v2182 = (char*)v2180 + 0;
    *(int64_t*)v2182 = (int64_t)v2181;
    v2184 = _str138;
    v2185 = (char*)v2180 + 8;
    *(void**)v2185 = (void*)v2184;
    v2187 = (char*)v2176 + 8;
    *(void**)v2187 = (void*)v2180;
    v2189 = _str139;
    v2190 = (char*)v2176 + 16;
    *(void**)v2190 = (void*)v2189;
    v2192 = malloc(8);
    v2193 = 0LL;
    v2194 = (char*)v2192 + 0;
    *(int64_t*)v2194 = (int64_t)v2193;
    v2196 = (char*)v2176 + 24;
    *(void**)v2196 = (void*)v2192;
    v2198 = malloc(32);
    v2199 = 0LL;
    v2200 = (char*)v2198 + 0;
    *(int64_t*)v2200 = (int64_t)v2199;
    v2202 = 1LL;
    v2203 = (char*)v2198 + 8;
    *(int64_t*)v2203 = (int64_t)v2202;
    v2205 = (char*)v2176 + 32;
    *(void**)v2205 = (void*)v2198;
    v2207 = 240LL;
    v2208 = (char*)v2176 + 40;
    *(int64_t*)v2208 = (int64_t)v2207;
    v2210 = malloc(8);
    *(void**)v2210 = (void*)v2176;
    v2212 = *(void**)v2210;
    v2213 = 0LL;
    v2214 = (int64_t)stmt_line((void*)v2213, v2212);
    v2215 = 240LL;
    v2216 = _str196;
    v2217 = 0LL;
    assert_eq_i64((void*)v2217, v2214, v2215, v2216);
    v2219 = malloc(64);
    v2220 = 9LL;
    v2221 = (char*)v2219 + 0;
    *(int64_t*)v2221 = (int64_t)v2220;
    v2223 = malloc(32);
    v2224 = 3LL;
    v2225 = (char*)v2223 + 0;
    *(int64_t*)v2225 = (int64_t)v2224;
    v2227 = true;
    v2228 = (char*)v2223 + 8;
    *(bool*)v2228 = v2227;
    v2230 = (char*)v2219 + 8;
    *(void**)v2230 = (void*)v2223;
    v2232 = 0LL;
    v2233 = Vec_new((void*)v2232);
    v2234 = (char*)v2219 + 16;
    *(void**)v2234 = (void*)v2233;
    v2236 = malloc(16);
    v2237 = 1LL;
    v2238 = (char*)v2236 + 0;
    *(int64_t*)v2238 = (int64_t)v2237;
    v2240 = (char*)v2219 + 24;
    *(void**)v2240 = (void*)v2236;
    v2242 = 250LL;
    v2243 = (char*)v2219 + 32;
    *(int64_t*)v2243 = (int64_t)v2242;
    v2245 = malloc(8);
    *(void**)v2245 = (void*)v2219;
    v2247 = *(void**)v2245;
    v2248 = 0LL;
    v2249 = (int64_t)stmt_line((void*)v2248, v2247);
    v2250 = 250LL;
    v2251 = _str197;
    v2252 = 0LL;
    assert_eq_i64((void*)v2252, v2249, v2250, v2251);
    v2254 = malloc(64);
    v2255 = 16LL;
    v2256 = (char*)v2254 + 0;
    *(int64_t*)v2256 = (int64_t)v2255;
    v2258 = malloc(32);
    v2259 = 4LL;
    v2260 = (char*)v2258 + 0;
    *(int64_t*)v2260 = (int64_t)v2259;
    v2262 = _str139;
    v2263 = (char*)v2258 + 8;
    *(void**)v2263 = (void*)v2262;
    v2265 = (char*)v2254 + 8;
    *(void**)v2265 = (void*)v2258;
    v2267 = 0LL;
    v2268 = Vec_new((void*)v2267);
    v2269 = (char*)v2254 + 16;
    *(void**)v2269 = (void*)v2268;
    v2271 = 260LL;
    v2272 = (char*)v2254 + 24;
    *(int64_t*)v2272 = (int64_t)v2271;
    v2274 = malloc(8);
    *(void**)v2274 = (void*)v2254;
    v2276 = *(void**)v2274;
    v2277 = 0LL;
    v2278 = (int64_t)stmt_line((void*)v2277, v2276);
    v2279 = 260LL;
    v2280 = _str198;
    v2281 = 0LL;
    assert_eq_i64((void*)v2281, v2278, v2279, v2280);
    v2283 = malloc(64);
    v2284 = 18LL;
    v2285 = (char*)v2283 + 0;
    *(int64_t*)v2285 = (int64_t)v2284;
    v2287 = 0LL;
    v2288 = Vec_new((void*)v2287);
    v2289 = (char*)v2283 + 8;
    *(void**)v2289 = (void*)v2288;
    v2291 = 0LL;
    v2292 = Vec_new((void*)v2291);
    v2293 = (char*)v2283 + 16;
    *(void**)v2293 = (void*)v2292;
    v2295 = 270LL;
    v2296 = (char*)v2283 + 24;
    *(int64_t*)v2296 = (int64_t)v2295;
    v2298 = malloc(8);
    *(void**)v2298 = (void*)v2283;
    v2300 = *(void**)v2298;
    v2301 = 0LL;
    v2302 = (int64_t)stmt_line((void*)v2301, v2300);
    v2303 = 270LL;
    v2304 = _str199;
    v2305 = 0LL;
    assert_eq_i64((void*)v2305, v2302, v2303, v2304);
    v2307 = malloc(64);
    v2308 = 19LL;
    v2309 = (char*)v2307 + 0;
    *(int64_t*)v2309 = (int64_t)v2308;
    v2311 = _str200;
    v2312 = (char*)v2307 + 8;
    *(void**)v2312 = (void*)v2311;
    v2314 = _str201;
    v2315 = (char*)v2307 + 16;
    *(void**)v2315 = (void*)v2314;
    v2317 = 0LL;
    v2318 = Vec_new((void*)v2317);
    v2319 = (char*)v2307 + 24;
    *(void**)v2319 = (void*)v2318;
    v2321 = 280LL;
    v2322 = (char*)v2307 + 32;
    *(int64_t*)v2322 = (int64_t)v2321;
    v2324 = malloc(8);
    *(void**)v2324 = (void*)v2307;
    v2326 = *(void**)v2324;
    v2327 = 0LL;
    v2328 = (int64_t)stmt_line((void*)v2327, v2326);
    v2329 = 280LL;
    v2330 = _str202;
    v2331 = 0LL;
    assert_eq_i64((void*)v2331, v2328, v2329, v2330);
    v2333 = malloc(64);
    v2334 = 24LL;
    v2335 = (char*)v2333 + 0;
    *(int64_t*)v2335 = (int64_t)v2334;
    v2337 = 0LL;
    v2338 = Vec_new((void*)v2337);
    v2339 = (char*)v2333 + 8;
    *(void**)v2339 = (void*)v2338;
    v2341 = 290LL;
    v2342 = (char*)v2333 + 16;
    *(int64_t*)v2342 = (int64_t)v2341;
    v2344 = malloc(8);
    *(void**)v2344 = (void*)v2333;
    v2346 = *(void**)v2344;
    v2347 = 0LL;
    v2348 = (int64_t)stmt_line((void*)v2347, v2346);
    v2349 = 290LL;
    v2350 = _str203;
    v2351 = 0LL;
    assert_eq_i64((void*)v2351, v2348, v2349, v2350);
    v2353 = malloc(64);
    v2354 = 28LL;
    v2355 = (char*)v2353 + 0;
    *(int64_t*)v2355 = (int64_t)v2354;
    v2357 = 0LL;
    v2358 = Vec_new((void*)v2357);
    v2359 = (char*)v2353 + 8;
    *(void**)v2359 = (void*)v2358;
    v2361 = malloc(16);
    v2362 = 1LL;
    v2363 = (char*)v2361 + 0;
    *(int64_t*)v2363 = (int64_t)v2362;
    v2365 = (char*)v2353 + 16;
    *(void**)v2365 = (void*)v2361;
    v2367 = 300LL;
    v2368 = (char*)v2353 + 24;
    *(int64_t*)v2368 = (int64_t)v2367;
    v2370 = malloc(8);
    *(void**)v2370 = (void*)v2353;
    v2372 = *(void**)v2370;
    v2373 = 0LL;
    v2374 = (int64_t)stmt_line((void*)v2373, v2372);
    v2375 = 300LL;
    v2376 = _str204;
    v2377 = 0LL;
    assert_eq_i64((void*)v2377, v2374, v2375, v2376);
    v2379 = _str54;
    p_print_str(v2379);
    v2381 = _str205;
    p_print_str(v2381);
    v2383 = malloc(32);
    v2384 = 0LL;
    v2385 = (char*)v2383 + 0;
    *(int64_t*)v2385 = (int64_t)v2384;
    v2387 = (char*)v2383 + 0;
    v2388 = *(int64_t*)v2387;
    v2389 = 0LL;
    v2390 = v2388 == v2389;
    if (v2390) goto bb98;
    else goto bb99;
  bb83:
    v1449 = (char*)v1428 + 8;
    v1450 = *(void**)v1449;
    v1451 = (char*)v1428 + 16;
    v1452 = *(void**)v1451;
    v1453 = (char*)v1428 + 24;
    v1454 = *(void**)v1453;
    v1455 = _str56;
    v1456 = _str206;
    v1457 = 0LL;
    assert_eq((void*)v1457, v1450, v1455, v1456);
    v1459 = 0LL;
    v1460 = (int64_t)Vec_len((void*)v1459, v1452);
    v1461 = 2LL;
    v1462 = _str207;
    v1463 = 0LL;
    assert_eq_i64((void*)v1463, v1460, v1461, v1462);
    v1465 = (char*)v1454 + 0;
    v1466 = *(int64_t*)v1465;
    v1467 = 1LL;
    v1468 = v1466 == v1467;
    if (v1468) goto bb86;
    else goto bb87;
  bb84:
    goto bb95;
  bb85:
    v1476 = 0LL;
    v1477 = 0LL;
    v1478 = (void*)Vec_get((void*)v1477, v1452, v1476);
    v1479 = malloc(8);
    *(void**)v1479 = (void*)v1478;
    v1481 = *(void**)v1479;
    v1482 = (char*)v1481 + 0;
    v1483 = *(void**)v1482;
    v1484 = _str139;
    v1485 = _str208;
    v1486 = 0LL;
    assert_eq((void*)v1486, v1483, v1484, v1485);
    v1488 = *(void**)v1479;
    v1489 = (char*)v1488 + 8;
    v1490 = (char*)v1489 + 0;
    v1491 = *(int64_t*)v1490;
    v1492 = 0LL;
    v1493 = v1491 == v1492;
    if (v1493) goto bb91;
    else goto bb92;
  bb86:
    goto bb85;
  bb87:
    goto bb88;
  bb88:
    v1472 = _str209;
    p_panic(v1472);
    goto bb85;
  bb89:
    goto bb85;
  bb90:
    goto bb82;
  bb91:
    v1495 = (char*)v1489 + 8;
    v1496 = *(int64_t*)v1495;
    v1497 = 1LL;
    v1498 = _str210;
    v1499 = 0LL;
    assert_eq_i64((void*)v1499, v1496, v1497, v1498);
    goto bb90;
  bb92:
    goto bb93;
  bb93:
    v1503 = _str211;
    p_panic(v1503);
    goto bb90;
  bb94:
    goto bb90;
  bb95:
    v1509 = _str212;
    p_panic(v1509);
    goto bb82;
  bb96:
    goto bb82;
  bb97:
    v2398 = malloc(32);
    v2399 = 1LL;
    v2400 = (char*)v2398 + 0;
    *(int64_t*)v2400 = (int64_t)v2399;
    v2402 = 42LL;
    v2403 = (char*)v2398 + 8;
    *(int64_t*)v2403 = (int64_t)v2402;
    v2405 = (char*)v2398 + 0;
    v2406 = *(int64_t*)v2405;
    v2407 = 1LL;
    v2408 = v2406 == v2407;
    if (v2408) goto bb103;
    else goto bb104;
  bb98:
    goto bb97;
  bb99:
    goto bb100;
  bb100:
    v2394 = _str213;
    p_panic(v2394);
    goto bb97;
  bb101:
    goto bb97;
  bb102:
    v2422 = malloc(32);
    v2423 = 2LL;
    v2424 = (char*)v2422 + 0;
    *(int64_t*)v2424 = (int64_t)v2423;
    v2426 = true;
    v2427 = (char*)v2422 + 8;
    *(bool*)v2427 = v2426;
    v2429 = (char*)v2422 + 0;
    v2430 = *(int64_t*)v2429;
    v2431 = 2LL;
    v2432 = v2430 == v2431;
    if (v2432) goto bb108;
    else goto bb109;
  bb103:
    v2410 = (char*)v2398 + 8;
    v2411 = *(int64_t*)v2410;
    v2412 = 42LL;
    v2413 = _str214;
    v2414 = 0LL;
    assert_eq_i64((void*)v2414, v2411, v2412, v2413);
    goto bb102;
  bb104:
    goto bb105;
  bb105:
    v2418 = _str215;
    p_panic(v2418);
    goto bb102;
  bb106:
    goto bb102;
  bb107:
    v2446 = malloc(32);
    v2447 = 3LL;
    v2448 = (char*)v2446 + 0;
    *(int64_t*)v2448 = (int64_t)v2447;
    v2450 = _str139;
    v2451 = (char*)v2446 + 8;
    *(void**)v2451 = (void*)v2450;
    v2453 = (char*)v2446 + 0;
    v2454 = *(int64_t*)v2453;
    v2455 = 3LL;
    v2456 = v2454 == v2455;
    if (v2456) goto bb113;
    else goto bb114;
  bb108:
    v2434 = (char*)v2422 + 8;
    v2435 = *(bool*)v2434;
    v2436 = true;
    v2437 = _str216;
    v2438 = 0LL;
    assert_eq_bool((void*)v2438, v2435, v2436, v2437);
    goto bb107;
  bb109:
    goto bb110;
  bb110:
    v2442 = _str217;
    p_panic(v2442);
    goto bb107;
  bb111:
    goto bb107;
  bb112:
    v2470 = malloc(32);
    v2471 = 5LL;
    v2472 = (char*)v2470 + 0;
    *(int64_t*)v2472 = (int64_t)v2471;
    v2474 = _str120;
    v2475 = (char*)v2470 + 8;
    *(void**)v2475 = (void*)v2474;
    v2477 = (char*)v2470 + 0;
    v2478 = *(int64_t*)v2477;
    v2479 = 5LL;
    v2480 = v2478 == v2479;
    if (v2480) goto bb118;
    else goto bb119;
  bb113:
    v2458 = (char*)v2446 + 8;
    v2459 = *(void**)v2458;
    v2460 = _str139;
    v2461 = _str218;
    v2462 = 0LL;
    assert_eq((void*)v2462, v2459, v2460, v2461);
    goto bb112;
  bb114:
    goto bb115;
  bb115:
    v2466 = _str219;
    p_panic(v2466);
    goto bb112;
  bb116:
    goto bb112;
  bb117:
    v2494 = 0LL;
    v2495 = Vec_new((void*)v2494);
    v2496 = malloc(8);
    *(void**)v2496 = (void*)v2495;
    v2498 = *(void**)v2496;
    v2499 = _str220;
    v2500 = _str143;
    v2501 = malloc(16);
    v2502 = 0LL;
    Binding_new(v2501, (void*)v2502, v2499, v2500);
    v2504 = 0LL;
    Vec_push((void*)v2504, v2498, (int64_t)v2501);
    v2506 = malloc(32);
    v2507 = 4LL;
    v2508 = (char*)v2506 + 0;
    *(int64_t*)v2508 = (int64_t)v2507;
    v2510 = _str82;
    v2511 = (char*)v2506 + 8;
    *(void**)v2511 = (void*)v2510;
    v2513 = _str221;
    v2514 = (char*)v2506 + 16;
    *(void**)v2514 = (void*)v2513;
    v2516 = *(void**)v2496;
    v2517 = (char*)v2506 + 24;
    *(void**)v2517 = (void*)v2516;
    v2519 = (char*)v2506 + 0;
    v2520 = *(int64_t*)v2519;
    v2521 = 4LL;
    v2522 = v2520 == v2521;
    if (v2522) goto bb123;
    else goto bb124;
  bb118:
    v2482 = (char*)v2470 + 8;
    v2483 = *(void**)v2482;
    v2484 = _str120;
    v2485 = _str222;
    v2486 = 0LL;
    assert_eq((void*)v2486, v2483, v2484, v2485);
    goto bb117;
  bb119:
    goto bb120;
  bb120:
    v2490 = _str223;
    p_panic(v2490);
    goto bb117;
  bb121:
    goto bb117;
  bb122:
    v2569 = malloc(32);
    v2570 = 6LL;
    v2571 = (char*)v2569 + 0;
    *(int64_t*)v2571 = (int64_t)v2570;
    v2573 = malloc(32);
    v2574 = 1LL;
    v2575 = (char*)v2573 + 0;
    *(int64_t*)v2575 = (int64_t)v2574;
    v2577 = 1LL;
    v2578 = (char*)v2573 + 8;
    *(int64_t*)v2578 = (int64_t)v2577;
    v2580 = 8LL;
    v2581 = p_malloc(v2580);
    *(void**)v2581 = (void*)v2573;
    v2583 = (char*)v2569 + 8;
    *(void**)v2583 = (void*)v2581;
    v2585 = malloc(32);
    v2586 = 1LL;
    v2587 = (char*)v2585 + 0;
    *(int64_t*)v2587 = (int64_t)v2586;
    v2589 = 10LL;
    v2590 = (char*)v2585 + 8;
    *(int64_t*)v2590 = (int64_t)v2589;
    v2592 = 8LL;
    v2593 = p_malloc(v2592);
    *(void**)v2593 = (void*)v2585;
    v2595 = (char*)v2569 + 16;
    *(void**)v2595 = (void*)v2593;
    v2597 = malloc(8);
    *(void**)v2597 = (void*)v2569;
    v2599 = *(void**)v2597;
    v2600 = (char*)v2599 + 0;
    v2601 = *(int64_t*)v2600;
    v2602 = 6LL;
    v2603 = v2601 == v2602;
    if (v2603) goto bb128;
    else goto bb129;
  bb123:
    v2524 = (char*)v2506 + 8;
    v2525 = *(void**)v2524;
    v2526 = (char*)v2506 + 16;
    v2527 = *(void**)v2526;
    v2528 = (char*)v2506 + 24;
    v2529 = *(void**)v2528;
    v2530 = _str82;
    v2531 = _str224;
    v2532 = 0LL;
    assert_eq((void*)v2532, v2525, v2530, v2531);
    v2534 = _str221;
    v2535 = _str225;
    v2536 = 0LL;
    assert_eq((void*)v2536, v2527, v2534, v2535);
    v2538 = 0LL;
    v2539 = (int64_t)Vec_len((void*)v2538, v2529);
    v2540 = 1LL;
    v2541 = _str226;
    v2542 = 0LL;
    assert_eq_i64((void*)v2542, v2539, v2540, v2541);
    v2544 = 0LL;
    v2545 = 0LL;
    v2546 = (void*)Vec_get((void*)v2545, v2529, v2544);
    v2547 = malloc(8);
    *(void**)v2547 = (void*)v2546;
    v2549 = *(void**)v2547;
    v2550 = (char*)v2549 + 0;
    v2551 = *(void**)v2550;
    v2552 = _str220;
    v2553 = _str227;
    v2554 = 0LL;
    assert_eq((void*)v2554, v2551, v2552, v2553);
    v2556 = *(void**)v2547;
    v2557 = (char*)v2556 + 8;
    v2558 = *(void**)v2557;
    v2559 = _str143;
    v2560 = _str228;
    v2561 = 0LL;
    assert_eq((void*)v2561, v2558, v2559, v2560);
    goto bb122;
  bb124:
    goto bb125;
  bb125:
    v2565 = _str229;
    p_panic(v2565);
    goto bb122;
  bb126:
    goto bb122;
  bb127:
    v2653 = _str54;
    p_print_str(v2653);
    v2655 = _str230;
    p_print_str(v2655);
    v2657 = malloc(32);
    v2658 = 0LL;
    v2659 = (char*)v2657 + 0;
    *(int64_t*)v2659 = (int64_t)v2658;
    v2661 = malloc(16);
    v2662 = 1LL;
    v2663 = (char*)v2661 + 0;
    *(int64_t*)v2663 = (int64_t)v2662;
    v2665 = 0LL;
    v2666 = Vec_new((void*)v2665);
    v2667 = malloc(56);
    v2668 = 0LL;
    MatchArm_new(v2667, (void*)v2668, v2657, v2661, v2666);
    v2670 = malloc(8);
    *(void**)v2670 = (void*)v2667;
    v2672 = *(void**)v2670;
    v2673 = (char*)v2672 + 0;
    v2674 = (char*)v2673 + 0;
    v2675 = *(int64_t*)v2674;
    v2676 = 0LL;
    v2677 = v2675 == v2676;
    if (v2677) goto bb143;
    else goto bb144;
  bb128:
    v2605 = (char*)v2599 + 8;
    v2606 = *(void**)v2605;
    v2607 = (char*)v2599 + 16;
    v2608 = *(void**)v2607;
    v2609 = (char*)v2606 + 0;
    v2610 = *(void**)v2609;
    v2611 = (char*)v2610 + 0;
    v2612 = *(int64_t*)v2611;
    v2613 = 1LL;
    v2614 = v2612 == v2613;
    if (v2614) goto bb131;
    else goto bb132;
  bb129:
    goto bb140;
  bb130:
    v2628 = (char*)v2608 + 0;
    v2629 = *(void**)v2628;
    v2630 = (char*)v2629 + 0;
    v2631 = *(int64_t*)v2630;
    v2632 = 1LL;
    v2633 = v2631 == v2632;
    if (v2633) goto bb136;
    else goto bb137;
  bb131:
    v2616 = (char*)v2610 + 8;
    v2617 = *(int64_t*)v2616;
    v2618 = 1LL;
    v2619 = _str231;
    v2620 = 0LL;
    assert_eq_i64((void*)v2620, v2617, v2618, v2619);
    goto bb130;
  bb132:
    goto bb133;
  bb133:
    v2624 = _str232;
    p_panic(v2624);
    goto bb130;
  bb134:
    goto bb130;
  bb135:
    goto bb127;
  bb136:
    v2635 = (char*)v2629 + 8;
    v2636 = *(int64_t*)v2635;
    v2637 = 10LL;
    v2638 = _str233;
    v2639 = 0LL;
    assert_eq_i64((void*)v2639, v2636, v2637, v2638);
    goto bb135;
  bb137:
    goto bb138;
  bb138:
    v2643 = _str234;
    p_panic(v2643);
    goto bb135;
  bb139:
    goto bb135;
  bb140:
    v2649 = _str235;
    p_panic(v2649);
    goto bb127;
  bb141:
    goto bb127;
  bb142:
    v2685 = *(void**)v2670;
    v2686 = (char*)v2685 + 32;
    v2687 = (char*)v2686 + 0;
    v2688 = *(int64_t*)v2687;
    v2689 = 1LL;
    v2690 = v2688 == v2689;
    if (v2690) goto bb148;
    else goto bb149;
  bb143:
    goto bb142;
  bb144:
    goto bb145;
  bb145:
    v2681 = _str236;
    p_panic(v2681);
    goto bb142;
  bb146:
    goto bb142;
  bb147:
    v2698 = *(void**)v2670;
    v2699 = (char*)v2698 + 48;
    v2700 = *(void**)v2699;
    v2701 = 0LL;
    v2702 = (int64_t)Vec_len((void*)v2701, v2700);
    v2703 = 0LL;
    v2704 = _str237;
    v2705 = 0LL;
    assert_eq_i64((void*)v2705, v2702, v2703, v2704);
    v2707 = malloc(32);
    v2708 = 3LL;
    v2709 = (char*)v2707 + 0;
    *(int64_t*)v2709 = (int64_t)v2708;
    v2711 = _str139;
    v2712 = (char*)v2707 + 8;
    *(void**)v2712 = (void*)v2711;
    v2714 = malloc(16);
    v2715 = 0LL;
    v2716 = (char*)v2714 + 0;
    *(int64_t*)v2716 = (int64_t)v2715;
    v2718 = malloc(32);
    v2719 = 4LL;
    v2720 = (char*)v2718 + 0;
    *(int64_t*)v2720 = (int64_t)v2719;
    v2722 = _str139;
    v2723 = (char*)v2718 + 8;
    *(void**)v2723 = (void*)v2722;
    v2725 = 8LL;
    v2726 = p_malloc(v2725);
    *(void**)v2726 = (void*)v2718;
    v2728 = (char*)v2714 + 8;
    *(void**)v2728 = (void*)v2726;
    v2730 = 0LL;
    v2731 = Vec_new((void*)v2730);
    v2732 = malloc(56);
    v2733 = 0LL;
    MatchArm_new(v2732, (void*)v2733, v2707, v2714, v2731);
    v2735 = malloc(8);
    *(void**)v2735 = (void*)v2732;
    v2737 = *(void**)v2735;
    v2738 = (char*)v2737 + 32;
    v2739 = (char*)v2738 + 0;
    v2740 = *(int64_t*)v2739;
    v2741 = 0LL;
    v2742 = v2740 == v2741;
    if (v2742) goto bb153;
    else goto bb154;
  bb148:
    goto bb147;
  bb149:
    goto bb150;
  bb150:
    v2694 = _str238;
    p_panic(v2694);
    goto bb147;
  bb151:
    goto bb147;
  bb152:
    v2771 = _str54;
    p_print_str(v2771);
    v2773 = _str239;
    p_print_str(v2773);
    v2775 = _str139;
    v2776 = malloc(40);
    v2777 = 3LL;
    v2778 = (char*)v2776 + 0;
    *(int64_t*)v2778 = (int64_t)v2777;
    v2780 = false;
    v2781 = malloc(64);
    v2782 = 0LL;
    Param_new(v2781, (void*)v2782, v2775, v2776, v2780);
    v2784 = malloc(8);
    *(void**)v2784 = (void*)v2781;
    v2786 = *(void**)v2784;
    v2787 = (char*)v2786 + 0;
    v2788 = *(void**)v2787;
    v2789 = _str139;
    v2790 = _str240;
    v2791 = 0LL;
    assert_eq((void*)v2791, v2788, v2789, v2790);
    v2793 = *(void**)v2784;
    v2794 = (char*)v2793 + 8;
    v2795 = 0LL;
    v2796 = type_to_str((void*)v2795, v2794);
    v2797 = _str26;
    v2798 = _str241;
    v2799 = 0LL;
    assert_eq((void*)v2799, v2796, v2797, v2798);
    v2801 = *(void**)v2784;
    v2802 = (char*)v2801 + 48;
    v2803 = *(bool*)v2802;
    v2804 = false;
    v2805 = _str242;
    v2806 = 0LL;
    assert_eq_bool((void*)v2806, v2803, v2804, v2805);
    v2808 = _str243;
    v2809 = 0LL;
    v2810 = Vec_new((void*)v2809);
    v2811 = malloc(16);
    v2812 = 0LL;
    Annotation_new(v2811, (void*)v2812, v2808, v2810);
    v2814 = malloc(8);
    *(void**)v2814 = (void*)v2811;
    v2816 = *(void**)v2814;
    v2817 = (char*)v2816 + 0;
    v2818 = *(void**)v2817;
    v2819 = _str243;
    v2820 = _str244;
    v2821 = 0LL;
    assert_eq((void*)v2821, v2818, v2819, v2820);
    v2823 = _str56;
    v2824 = true;
    v2825 = malloc(40);
    v2826 = 0LL;
    StructDef_new(v2825, (void*)v2826, v2823, v2824);
    v2828 = malloc(8);
    *(void**)v2828 = (void*)v2825;
    v2830 = *(void**)v2828;
    v2831 = (char*)v2830 + 0;
    v2832 = *(void**)v2831;
    v2833 = _str56;
    v2834 = _str245;
    v2835 = 0LL;
    assert_eq((void*)v2835, v2832, v2833, v2834);
    v2837 = *(void**)v2828;
    v2838 = (char*)v2837 + 16;
    v2839 = *(bool*)v2838;
    v2840 = true;
    v2841 = _str246;
    v2842 = 0LL;
    assert_eq_bool((void*)v2842, v2839, v2840, v2841);
    v2844 = 0LL;
    v2845 = Vec_new((void*)v2844);
    v2846 = malloc(8);
    *(void**)v2846 = (void*)v2845;
    v2848 = *(void**)v2846;
    v2849 = _str221;
    v2850 = malloc(16);
    v2851 = 0LL;
    v2852 = (char*)v2850 + 0;
    *(int64_t*)v2852 = (int64_t)v2851;
    v2854 = 0LL;
    v2855 = Vec_new((void*)v2854);
    v2856 = (char*)v2850 + 8;
    *(void**)v2856 = (void*)v2855;
    v2858 = malloc(24);
    v2859 = 0LL;
    EnumVariant_new(v2858, (void*)v2859, v2849, v2850);
    v2861 = 0LL;
    Vec_push((void*)v2861, v2848, (int64_t)v2858);
    v2863 = *(void**)v2846;
    v2864 = _str247;
    v2865 = malloc(16);
    v2866 = 1LL;
    v2867 = (char*)v2865 + 0;
    *(int64_t*)v2867 = (int64_t)v2866;
    v2869 = malloc(24);
    v2870 = 0LL;
    EnumVariant_new(v2869, (void*)v2870, v2864, v2865);
    v2872 = 0LL;
    Vec_push((void*)v2872, v2863, (int64_t)v2869);
    v2874 = _str82;
    v2875 = *(void**)v2846;
    v2876 = true;
    v2877 = malloc(32);
    v2878 = 0LL;
    EnumDef_new(v2877, (void*)v2878, v2874, v2875, v2876);
    v2880 = malloc(8);
    *(void**)v2880 = (void*)v2877;
    v2882 = *(void**)v2880;
    v2883 = (char*)v2882 + 0;
    v2884 = *(void**)v2883;
    v2885 = _str82;
    v2886 = _str248;
    v2887 = 0LL;
    assert_eq((void*)v2887, v2884, v2885, v2886);
    v2889 = *(void**)v2880;
    v2890 = (char*)v2889 + 8;
    v2891 = *(void**)v2890;
    v2892 = 0LL;
    v2893 = (int64_t)Vec_len((void*)v2892, v2891);
    v2894 = 2LL;
    v2895 = _str249;
    v2896 = 0LL;
    assert_eq_i64((void*)v2896, v2893, v2894, v2895);
    v2898 = *(void**)v2880;
    v2899 = (char*)v2898 + 8;
    v2900 = *(void**)v2899;
    v2901 = 1LL;
    v2902 = 0LL;
    v2903 = (void*)Vec_get((void*)v2902, v2900, v2901);
    v2904 = malloc(8);
    *(void**)v2904 = (void*)v2903;
    v2906 = *(void**)v2904;
    v2907 = (char*)v2906 + 0;
    v2908 = *(void**)v2907;
    v2909 = _str247;
    v2910 = _str250;
    v2911 = 0LL;
    assert_eq((void*)v2911, v2908, v2909, v2910);
    v2913 = *(void**)v2904;
    v2914 = (char*)v2913 + 8;
    v2915 = (char*)v2914 + 0;
    v2916 = *(int64_t*)v2915;
    v2917 = 1LL;
    v2918 = v2916 == v2917;
    if (v2918) goto bb163;
    else goto bb164;
  bb153:
    v2744 = (char*)v2738 + 8;
    v2745 = *(void**)v2744;
    v2746 = (char*)v2745 + 0;
    v2747 = *(void**)v2746;
    v2748 = (char*)v2747 + 0;
    v2749 = *(int64_t*)v2748;
    v2750 = 4LL;
    v2751 = v2749 == v2750;
    if (v2751) goto bb156;
    else goto bb157;
  bb154:
    goto bb160;
  bb155:
    goto bb152;
  bb156:
    v2753 = (char*)v2747 + 8;
    v2754 = *(void**)v2753;
    v2755 = _str139;
    v2756 = _str251;
    v2757 = 0LL;
    assert_eq((void*)v2757, v2754, v2755, v2756);
    goto bb155;
  bb157:
    goto bb158;
  bb158:
    v2761 = _str252;
    p_panic(v2761);
    goto bb155;
  bb159:
    goto bb155;
  bb160:
    v2767 = _str253;
    p_panic(v2767);
    goto bb152;
  bb161:
    goto bb152;
  bb162:
    v2926 = _str254;
    v2927 = malloc(40);
    v2928 = 3LL;
    v2929 = (char*)v2927 + 0;
    *(int64_t*)v2929 = (int64_t)v2928;
    v2931 = malloc(160);
    v2932 = 0LL;
    Func_new(v2931, (void*)v2932, v2926, v2927);
    v2934 = malloc(8);
    *(void**)v2934 = (void*)v2931;
    v2936 = *(void**)v2934;
    v2937 = (char*)v2936 + 0;
    v2938 = *(void**)v2937;
    v2939 = _str254;
    v2940 = _str255;
    v2941 = 0LL;
    assert_eq((void*)v2941, v2938, v2939, v2940);
    v2943 = *(void**)v2934;
    v2944 = (char*)v2943 + 40;
    v2945 = 0LL;
    v2946 = type_to_str((void*)v2945, v2944);
    v2947 = _str26;
    v2948 = _str256;
    v2949 = 0LL;
    assert_eq((void*)v2949, v2946, v2947, v2948);
    v2951 = *(void**)v2934;
    v2952 = (char*)v2951 + 152;
    v2953 = *(bool*)v2952;
    v2954 = false;
    v2955 = _str257;
    v2956 = 0LL;
    assert_eq_bool((void*)v2956, v2953, v2954, v2955);
    v2958 = _str54;
    p_print_str(v2958);
    v2960 = _str258;
    p_print_str(v2960);
    v2962 = malloc(152);
    v2963 = 0LL;
    program_new(v2962, (void*)v2963);
    v2965 = malloc(8);
    *(void**)v2965 = (void*)v2962;
    v2967 = *(void**)v2965;
    v2968 = (char*)v2967 + 0;
    v2969 = (char*)v2968 + 0;
    v2970 = *(int64_t*)v2969;
    v2971 = 1LL;
    v2972 = v2970 == v2971;
    if (v2972) goto bb168;
    else goto bb169;
  bb163:
    goto bb162;
  bb164:
    goto bb165;
  bb165:
    v2922 = _str259;
    p_panic(v2922);
    goto bb162;
  bb166:
    goto bb162;
  bb167:
    v2980 = *(void**)v2965;
    v2981 = (char*)v2980 + 16;
    v2982 = *(void**)v2981;
    v2983 = 0LL;
    v2984 = (int64_t)Vec_len((void*)v2983, v2982);
    v2985 = 0LL;
    v2986 = _str260;
    v2987 = 0LL;
    assert_eq_i64((void*)v2987, v2984, v2985, v2986);
    v2989 = *(void**)v2965;
    v2990 = (char*)v2989 + 24;
    v2991 = *(void**)v2990;
    v2992 = 0LL;
    v2993 = (int64_t)Vec_len((void*)v2992, v2991);
    v2994 = 0LL;
    v2995 = _str261;
    v2996 = 0LL;
    assert_eq_i64((void*)v2996, v2993, v2994, v2995);
    v2998 = *(void**)v2965;
    v2999 = (char*)v2998 + 32;
    v3000 = *(void**)v2999;
    v3001 = 0LL;
    v3002 = (int64_t)Vec_len((void*)v3001, v3000);
    v3003 = 0LL;
    v3004 = _str262;
    v3005 = 0LL;
    assert_eq_i64((void*)v3005, v3002, v3003, v3004);
    v3007 = *(void**)v2965;
    v3008 = (char*)v3007 + 40;
    v3009 = *(void**)v3008;
    v3010 = 0LL;
    v3011 = (int64_t)Vec_len((void*)v3010, v3009);
    v3012 = 0LL;
    v3013 = _str263;
    v3014 = 0LL;
    assert_eq_i64((void*)v3014, v3011, v3012, v3013);
    v3016 = *(void**)v2965;
    v3017 = (char*)v3016 + 64;
    v3018 = *(void**)v3017;
    v3019 = 0LL;
    v3020 = (int64_t)Vec_len((void*)v3019, v3018);
    v3021 = 0LL;
    v3022 = _str264;
    v3023 = 0LL;
    assert_eq_i64((void*)v3023, v3020, v3021, v3022);
    v3025 = *(void**)v2965;
    v3026 = (char*)v3025 + 80;
    v3027 = *(void**)v3026;
    v3028 = 0LL;
    v3029 = (int64_t)Vec_len((void*)v3028, v3027);
    v3030 = 0LL;
    v3031 = _str265;
    v3032 = 0LL;
    assert_eq_i64((void*)v3032, v3029, v3030, v3031);
    v3034 = *(void**)v2965;
    v3035 = (char*)v3034 + 88;
    v3036 = *(void**)v3035;
    v3037 = 0LL;
    v3038 = (int64_t)Vec_len((void*)v3037, v3036);
    v3039 = 0LL;
    v3040 = _str266;
    v3041 = 0LL;
    assert_eq_i64((void*)v3041, v3038, v3039, v3040);
    v3043 = *(void**)v2965;
    v3044 = (char*)v3043 + 96;
    v3045 = *(void**)v3044;
    v3046 = 0LL;
    v3047 = (int64_t)Vec_len((void*)v3046, v3045);
    v3048 = 0LL;
    v3049 = _str267;
    v3050 = 0LL;
    assert_eq_i64((void*)v3050, v3047, v3048, v3049);
    v3052 = *(void**)v2965;
    v3053 = (char*)v3052 + 104;
    v3054 = *(void**)v3053;
    v3055 = 0LL;
    v3056 = (int64_t)Vec_len((void*)v3055, v3054);
    v3057 = 0LL;
    v3058 = _str268;
    v3059 = 0LL;
    assert_eq_i64((void*)v3059, v3056, v3057, v3058);
    v3061 = *(void**)v2965;
    v3062 = (char*)v3061 + 112;
    v3063 = *(void**)v3062;
    v3064 = 0LL;
    v3065 = (int64_t)Vec_len((void*)v3064, v3063);
    v3066 = 0LL;
    v3067 = _str269;
    v3068 = 0LL;
    assert_eq_i64((void*)v3068, v3065, v3066, v3067);
    v3070 = *(void**)v2965;
    v3071 = (char*)v3070 + 120;
    v3072 = *(void**)v3071;
    v3073 = 0LL;
    v3074 = (int64_t)Vec_len((void*)v3073, v3072);
    v3075 = 0LL;
    v3076 = _str270;
    v3077 = 0LL;
    assert_eq_i64((void*)v3077, v3074, v3075, v3076);
    v3079 = *(void**)v2965;
    v3080 = (char*)v3079 + 128;
    v3081 = *(void**)v3080;
    v3082 = 0LL;
    v3083 = (int64_t)Vec_len((void*)v3082, v3081);
    v3084 = 0LL;
    v3085 = _str271;
    v3086 = 0LL;
    assert_eq_i64((void*)v3086, v3083, v3084, v3085);
    v3088 = *(void**)v2965;
    v3089 = (char*)v3088 + 136;
    v3090 = *(void**)v3089;
    v3091 = 0LL;
    v3092 = (int64_t)Vec_len((void*)v3091, v3090);
    v3093 = 0LL;
    v3094 = _str272;
    v3095 = 0LL;
    assert_eq_i64((void*)v3095, v3092, v3093, v3094);
    v3097 = *(void**)v2965;
    v3098 = (char*)v3097 + 144;
    v3099 = *(void**)v3098;
    v3100 = 0LL;
    v3101 = (int64_t)Vec_len((void*)v3100, v3099);
    v3102 = 0LL;
    v3103 = _str273;
    v3104 = 0LL;
    assert_eq_i64((void*)v3104, v3101, v3102, v3103);
    v3106 = malloc(152);
    v3107 = 0LL;
    program_new(v3106, (void*)v3107);
    v3109 = malloc(8);
    *(void**)v3109 = (void*)v3106;
    v3111 = *(void**)v3109;
    v3112 = (char*)v3111 + 16;
    v3113 = *(void**)v3112;
    v3114 = *(void**)v2934;
    v3115 = 0LL;
    Vec_push((void*)v3115, v3113, (int64_t)v3114);
    v3117 = *(void**)v3109;
    v3118 = (char*)v3117 + 16;
    v3119 = *(void**)v3118;
    v3120 = 0LL;
    v3121 = (int64_t)Vec_len((void*)v3120, v3119);
    v3122 = 1LL;
    v3123 = _str274;
    v3124 = 0LL;
    assert_eq_i64((void*)v3124, v3121, v3122, v3123);
    v3126 = _str54;
    p_print_str(v3126);
    v3128 = _str275;
    p_print_str(v3128);
    v3130 = malloc(16);
    v3131 = 1LL;
    v3132 = (char*)v3130 + 0;
    *(int64_t*)v3132 = (int64_t)v3131;
    v3134 = (char*)v3130 + 0;
    v3135 = *(int64_t*)v3134;
    v3136 = 1LL;
    v3137 = v3135 == v3136;
    if (v3137) goto bb173;
    else goto bb174;
  bb168:
    goto bb167;
  bb169:
    goto bb170;
  bb170:
    v2976 = _str276;
    p_panic(v2976);
    goto bb167;
  bb171:
    goto bb167;
  bb172:
    v3145 = malloc(16);
    v3146 = 0LL;
    v3147 = (char*)v3145 + 0;
    *(int64_t*)v3147 = (int64_t)v3146;
    v3149 = malloc(32);
    v3150 = 0LL;
    v3151 = (char*)v3149 + 0;
    *(int64_t*)v3151 = (int64_t)v3150;
    v3153 = 1LL;
    v3154 = (char*)v3149 + 8;
    *(int64_t*)v3154 = (int64_t)v3153;
    v3156 = 8LL;
    v3157 = p_malloc(v3156);
    *(void**)v3157 = (void*)v3149;
    v3159 = (char*)v3145 + 8;
    *(void**)v3159 = (void*)v3157;
    v3161 = (char*)v3145 + 0;
    v3162 = *(int64_t*)v3161;
    v3163 = 0LL;
    v3164 = v3162 == v3163;
    if (v3164) goto bb178;
    else goto bb179;
  bb173:
    goto bb172;
  bb174:
    goto bb175;
  bb175:
    v3141 = _str277;
    p_panic(v3141);
    goto bb172;
  bb176:
    goto bb172;
  bb177:
    v3193 = malloc(16);
    v3194 = 1LL;
    v3195 = (char*)v3193 + 0;
    *(int64_t*)v3195 = (int64_t)v3194;
    v3197 = (char*)v3193 + 0;
    v3198 = *(int64_t*)v3197;
    v3199 = 1LL;
    v3200 = v3198 == v3199;
    if (v3200) goto bb188;
    else goto bb189;
  bb178:
    v3166 = (char*)v3145 + 8;
    v3167 = *(void**)v3166;
    v3168 = (char*)v3167 + 0;
    v3169 = *(void**)v3168;
    v3170 = (char*)v3169 + 0;
    v3171 = *(int64_t*)v3170;
    v3172 = 0LL;
    v3173 = v3171 == v3172;
    if (v3173) goto bb181;
    else goto bb182;
  bb179:
    goto bb185;
  bb180:
    goto bb177;
  bb181:
    v3175 = (char*)v3169 + 8;
    v3176 = *(int64_t*)v3175;
    v3177 = 1LL;
    v3178 = _str278;
    v3179 = 0LL;
    assert_eq_i64((void*)v3179, v3176, v3177, v3178);
    goto bb180;
  bb182:
    goto bb183;
  bb183:
    v3183 = _str279;
    p_panic(v3183);
    goto bb180;
  bb184:
    goto bb180;
  bb185:
    v3189 = _str280;
    p_panic(v3189);
    goto bb177;
  bb186:
    goto bb177;
  bb187:
    v3208 = malloc(16);
    v3209 = 0LL;
    v3210 = (char*)v3208 + 0;
    *(int64_t*)v3210 = (int64_t)v3209;
    v3212 = malloc(40);
    v3213 = 3LL;
    v3214 = (char*)v3212 + 0;
    *(int64_t*)v3214 = (int64_t)v3213;
    v3216 = (char*)v3208 + 8;
    *(void**)v3216 = (void*)v3212;
    v3218 = (char*)v3208 + 0;
    v3219 = *(int64_t*)v3218;
    v3220 = 0LL;
    v3221 = v3219 == v3220;
    if (v3221) goto bb193;
    else goto bb194;
  bb188:
    goto bb187;
  bb189:
    goto bb190;
  bb190:
    v3204 = _str281;
    p_panic(v3204);
    goto bb187;
  bb191:
    goto bb187;
  bb192:
    v3237 = malloc(16);
    v3238 = 1LL;
    v3239 = (char*)v3237 + 0;
    *(int64_t*)v3239 = (int64_t)v3238;
    v3241 = (char*)v3237 + 0;
    v3242 = *(int64_t*)v3241;
    v3243 = 1LL;
    v3244 = v3242 == v3243;
    if (v3244) goto bb198;
    else goto bb199;
  bb193:
    v3223 = (char*)v3208 + 8;
    v3224 = *(void**)v3223;
    v3225 = 0LL;
    v3226 = type_to_str((void*)v3225, v3224);
    v3227 = _str26;
    v3228 = _str282;
    v3229 = 0LL;
    assert_eq((void*)v3229, v3226, v3227, v3228);
    goto bb192;
  bb194:
    goto bb195;
  bb195:
    v3233 = _str283;
    p_panic(v3233);
    goto bb192;
  bb196:
    goto bb192;
  bb197:
    v3252 = 0LL;
    v3253 = Vec_new((void*)v3252);
    v3254 = malloc(8);
    *(void**)v3254 = (void*)v3253;
    v3256 = *(void**)v3254;
    v3257 = malloc(64);
    v3258 = 14LL;
    v3259 = (char*)v3257 + 0;
    *(int64_t*)v3259 = (int64_t)v3258;
    v3261 = 1LL;
    v3262 = (char*)v3257 + 8;
    *(int64_t*)v3262 = (int64_t)v3261;
    v3264 = 0LL;
    Vec_push((void*)v3264, v3256, (int64_t)v3257);
    v3266 = malloc(16);
    v3267 = 0LL;
    v3268 = (char*)v3266 + 0;
    *(int64_t*)v3268 = (int64_t)v3267;
    v3270 = *(void**)v3254;
    v3271 = (char*)v3266 + 8;
    *(void**)v3271 = (void*)v3270;
    v3273 = (char*)v3266 + 0;
    v3274 = *(int64_t*)v3273;
    v3275 = 0LL;
    v3276 = v3274 == v3275;
    if (v3276) goto bb203;
    else goto bb204;
  bb198:
    goto bb197;
  bb199:
    goto bb200;
  bb200:
    v3248 = _str284;
    p_panic(v3248);
    goto bb197;
  bb201:
    goto bb197;
  bb202:
    v3292 = malloc(16);
    v3293 = 1LL;
    v3294 = (char*)v3292 + 0;
    *(int64_t*)v3294 = (int64_t)v3293;
    v3296 = (char*)v3292 + 0;
    v3297 = *(int64_t*)v3296;
    v3298 = 1LL;
    v3299 = v3297 == v3298;
    if (v3299) goto bb208;
    else goto bb209;
  bb203:
    v3278 = (char*)v3266 + 8;
    v3279 = *(void**)v3278;
    v3280 = 0LL;
    v3281 = (int64_t)Vec_len((void*)v3280, v3279);
    v3282 = 1LL;
    v3283 = _str285;
    v3284 = 0LL;
    assert_eq_i64((void*)v3284, v3281, v3282, v3283);
    goto bb202;
  bb204:
    goto bb205;
  bb205:
    v3288 = _str286;
    p_panic(v3288);
    goto bb202;
  bb206:
    goto bb202;
  bb207:
    v3307 = malloc(16);
    v3308 = 0LL;
    v3309 = (char*)v3307 + 0;
    *(int64_t*)v3309 = (int64_t)v3308;
    v3311 = 42LL;
    v3312 = (char*)v3307 + 8;
    *(int64_t*)v3312 = (int64_t)v3311;
    v3314 = (char*)v3307 + 0;
    v3315 = *(int64_t*)v3314;
    v3316 = 0LL;
    v3317 = v3315 == v3316;
    if (v3317) goto bb213;
    else goto bb214;
  bb208:
    goto bb207;
  bb209:
    goto bb210;
  bb210:
    v3303 = _str287;
    p_panic(v3303);
    goto bb207;
  bb211:
    goto bb207;
  bb212:
    v3331 = malloc(16);
    v3332 = 1LL;
    v3333 = (char*)v3331 + 0;
    *(int64_t*)v3333 = (int64_t)v3332;
    v3335 = (char*)v3331 + 0;
    v3336 = *(int64_t*)v3335;
    v3337 = 1LL;
    v3338 = v3336 == v3337;
    if (v3338) goto bb218;
    else goto bb219;
  bb213:
    v3319 = (char*)v3307 + 8;
    v3320 = *(int64_t*)v3319;
    v3321 = 42LL;
    v3322 = _str288;
    v3323 = 0LL;
    assert_eq_i64((void*)v3323, v3320, v3321, v3322);
    goto bb212;
  bb214:
    goto bb215;
  bb215:
    v3327 = _str289;
    p_panic(v3327);
    goto bb212;
  bb216:
    goto bb212;
  bb217:
    v3346 = malloc(16);
    v3347 = 0LL;
    v3348 = (char*)v3346 + 0;
    *(int64_t*)v3348 = (int64_t)v3347;
    v3350 = _str290;
    v3351 = (char*)v3346 + 8;
    *(void**)v3351 = (void*)v3350;
    v3353 = (char*)v3346 + 0;
    v3354 = *(int64_t*)v3353;
    v3355 = 0LL;
    v3356 = v3354 == v3355;
    if (v3356) goto bb223;
    else goto bb224;
  bb218:
    goto bb217;
  bb219:
    goto bb220;
  bb220:
    v3342 = _str291;
    p_panic(v3342);
    goto bb217;
  bb221:
    goto bb217;
  bb222:
    v3370 = malloc(16);
    v3371 = 1LL;
    v3372 = (char*)v3370 + 0;
    *(int64_t*)v3372 = (int64_t)v3371;
    v3374 = (char*)v3370 + 0;
    v3375 = *(int64_t*)v3374;
    v3376 = 1LL;
    v3377 = v3375 == v3376;
    if (v3377) goto bb228;
    else goto bb229;
  bb223:
    v3358 = (char*)v3346 + 8;
    v3359 = *(void**)v3358;
    v3360 = _str290;
    v3361 = _str292;
    v3362 = 0LL;
    assert_eq((void*)v3362, v3359, v3360, v3361);
    goto bb222;
  bb224:
    goto bb225;
  bb225:
    v3366 = _str293;
    p_panic(v3366);
    goto bb222;
  bb226:
    goto bb222;
  bb227:
    v3385 = malloc(16);
    v3386 = 0LL;
    v3387 = (char*)v3385 + 0;
    *(int64_t*)v3387 = (int64_t)v3386;
    v3389 = 0LL;
    v3390 = Vec_new((void*)v3389);
    v3391 = (char*)v3385 + 8;
    *(void**)v3391 = (void*)v3390;
    v3393 = (char*)v3385 + 0;
    v3394 = *(int64_t*)v3393;
    v3395 = 0LL;
    v3396 = v3394 == v3395;
    if (v3396) goto bb233;
    else goto bb234;
  bb228:
    goto bb227;
  bb229:
    goto bb230;
  bb230:
    v3381 = _str294;
    p_panic(v3381);
    goto bb227;
  bb231:
    goto bb227;
  bb232:
    v3412 = _str54;
    p_print_str(v3412);
    v3414 = _str295;
    p_print_str(v3414);
    v3416 = _str139;
    v3417 = malloc(32);
    v3418 = 0LL;
    v3419 = (char*)v3417 + 0;
    *(int64_t*)v3419 = (int64_t)v3418;
    v3421 = 1LL;
    v3422 = (char*)v3417 + 8;
    *(int64_t*)v3422 = (int64_t)v3421;
    v3424 = malloc(40);
    v3425 = 0LL;
    FieldInit_new(v3424, (void*)v3425, v3416, v3417);
    v3427 = malloc(8);
    *(void**)v3427 = (void*)v3424;
    v3429 = *(void**)v3427;
    v3430 = (char*)v3429 + 0;
    v3431 = *(void**)v3430;
    v3432 = _str139;
    v3433 = _str296;
    v3434 = 0LL;
    assert_eq((void*)v3434, v3431, v3432, v3433);
    v3436 = *(void**)v3427;
    v3437 = (char*)v3436 + 8;
    v3438 = (char*)v3437 + 0;
    v3439 = *(int64_t*)v3438;
    v3440 = 0LL;
    v3441 = v3439 == v3440;
    if (v3441) goto bb238;
    else goto bb239;
  bb233:
    v3398 = (char*)v3385 + 8;
    v3399 = *(void**)v3398;
    v3400 = 0LL;
    v3401 = (int64_t)Vec_len((void*)v3400, v3399);
    v3402 = 0LL;
    v3403 = _str297;
    v3404 = 0LL;
    assert_eq_i64((void*)v3404, v3401, v3402, v3403);
    goto bb232;
  bb234:
    goto bb235;
  bb235:
    v3408 = _str298;
    p_panic(v3408);
    goto bb232;
  bb236:
    goto bb232;
  bb237:
    v3455 = _str64;
    v3456 = _str299;
    v3457 = malloc(16);
    v3458 = 0LL;
    StrPair_new(v3457, (void*)v3458, v3455, v3456);
    v3460 = malloc(8);
    *(void**)v3460 = (void*)v3457;
    v3462 = *(void**)v3460;
    v3463 = (char*)v3462 + 0;
    v3464 = *(void**)v3463;
    v3465 = _str64;
    v3466 = _str300;
    v3467 = 0LL;
    assert_eq((void*)v3467, v3464, v3465, v3466);
    v3469 = *(void**)v3460;
    v3470 = (char*)v3469 + 8;
    v3471 = *(void**)v3470;
    v3472 = _str299;
    v3473 = _str301;
    v3474 = 0LL;
    assert_eq((void*)v3474, v3471, v3472, v3473);
    v3476 = _str94;
    v3477 = 0LL;
    v3478 = Vec_new((void*)v3477);
    v3479 = malloc(16);
    v3480 = 0LL;
    AssocTypeDecl_new(v3479, (void*)v3480, v3476, v3478);
    v3482 = malloc(8);
    *(void**)v3482 = (void*)v3479;
    v3484 = *(void**)v3482;
    v3485 = (char*)v3484 + 0;
    v3486 = *(void**)v3485;
    v3487 = _str94;
    v3488 = _str302;
    v3489 = 0LL;
    assert_eq((void*)v3489, v3486, v3487, v3488);
    v3491 = _str94;
    v3492 = malloc(40);
    v3493 = 3LL;
    v3494 = (char*)v3492 + 0;
    *(int64_t*)v3494 = (int64_t)v3493;
    v3496 = malloc(48);
    v3497 = 0LL;
    AssocTypeBinding_new(v3496, (void*)v3497, v3491, v3492);
    v3499 = malloc(8);
    *(void**)v3499 = (void*)v3496;
    v3501 = *(void**)v3499;
    v3502 = (char*)v3501 + 0;
    v3503 = *(void**)v3502;
    v3504 = _str94;
    v3505 = _str303;
    v3506 = 0LL;
    assert_eq((void*)v3506, v3503, v3504, v3505);
    v3508 = *(void**)v3499;
    v3509 = (char*)v3508 + 8;
    v3510 = 0LL;
    v3511 = type_to_str((void*)v3510, v3509);
    v3512 = _str26;
    v3513 = _str304;
    v3514 = 0LL;
    assert_eq((void*)v3514, v3511, v3512, v3513);
    v3516 = 0LL;
    v3517 = Vec_new((void*)v3516);
    v3518 = malloc(8);
    *(void**)v3518 = (void*)v3517;
    v3520 = *(void**)v3518;
    v3521 = _str305;
    v3522 = 0LL;
    Vec_push((void*)v3522, v3520, (int64_t)v3521);
    v3524 = *(void**)v3518;
    v3525 = _str306;
    v3526 = 0LL;
    Vec_push((void*)v3526, v3524, (int64_t)v3525);
    v3528 = _str307;
    v3529 = _str308;
    v3530 = *(void**)v3518;
    v3531 = malloc(24);
    v3532 = 0LL;
    LayerInst_new(v3531, (void*)v3532, v3528, v3529, v3530);
    v3534 = malloc(8);
    *(void**)v3534 = (void*)v3531;
    v3536 = *(void**)v3534;
    v3537 = (char*)v3536 + 0;
    v3538 = *(void**)v3537;
    v3539 = _str307;
    v3540 = _str309;
    v3541 = 0LL;
    assert_eq((void*)v3541, v3538, v3539, v3540);
    v3543 = *(void**)v3534;
    v3544 = (char*)v3543 + 8;
    v3545 = *(void**)v3544;
    v3546 = _str308;
    v3547 = _str310;
    v3548 = 0LL;
    assert_eq((void*)v3548, v3545, v3546, v3547);
    v3550 = *(void**)v3534;
    v3551 = (char*)v3550 + 16;
    v3552 = *(void**)v3551;
    v3553 = 0LL;
    v3554 = (int64_t)Vec_len((void*)v3553, v3552);
    v3555 = 2LL;
    v3556 = _str311;
    v3557 = 0LL;
    assert_eq_i64((void*)v3557, v3554, v3555, v3556);
    v3559 = _str220;
    v3560 = _str143;
    v3561 = malloc(16);
    v3562 = 0LL;
    Binding_new(v3561, (void*)v3562, v3559, v3560);
    v3564 = malloc(8);
    *(void**)v3564 = (void*)v3561;
    v3566 = *(void**)v3564;
    v3567 = (char*)v3566 + 0;
    v3568 = *(void**)v3567;
    v3569 = _str220;
    v3570 = _str312;
    v3571 = 0LL;
    assert_eq((void*)v3571, v3568, v3569, v3570);
    v3573 = *(void**)v3564;
    v3574 = (char*)v3573 + 8;
    v3575 = *(void**)v3574;
    v3576 = _str143;
    v3577 = _str313;
    v3578 = 0LL;
    assert_eq((void*)v3578, v3575, v3576, v3577);
    v3580 = _str54;
    p_print_str(v3580);
    v3582 = _str314;
    p_print_str(v3582);
    v3584 = malloc(32);
    v3585 = 4LL;
    v3586 = (char*)v3584 + 0;
    *(int64_t*)v3586 = (int64_t)v3585;
    v3588 = _str315;
    v3589 = (char*)v3584 + 8;
    *(void**)v3589 = (void*)v3588;
    v3591 = _str139;
    v3592 = 0LL;
    v3593 = Vec_new((void*)v3592);
    v3594 = malloc(48);
    v3595 = 0LL;
    SelectCase_new(v3594, (void*)v3595, v3584, v3591, v3593);
    v3597 = malloc(8);
    *(void**)v3597 = (void*)v3594;
    v3599 = *(void**)v3597;
    v3600 = (char*)v3599 + 32;
    v3601 = *(void**)v3600;
    v3602 = _str139;
    v3603 = _str316;
    v3604 = 0LL;
    assert_eq((void*)v3604, v3601, v3602, v3603);
    v3606 = 0LL;
    v3607 = Vec_new((void*)v3606);
    v3608 = malloc(8);
    *(void**)v3608 = (void*)v3607;
    v3610 = *(void**)v3608;
    v3611 = _str317;
    v3612 = 0LL;
    Vec_push((void*)v3612, v3610, (int64_t)v3611);
    v3614 = _str200;
    v3615 = _str201;
    v3616 = *(void**)v3608;
    v3617 = 0LL;
    v3618 = Vec_new((void*)v3617);
    v3619 = malloc(32);
    v3620 = 0LL;
    EffectHandler_new(v3619, (void*)v3620, v3614, v3615, v3616, v3618);
    v3622 = malloc(8);
    *(void**)v3622 = (void*)v3619;
    v3624 = *(void**)v3622;
    v3625 = (char*)v3624 + 0;
    v3626 = *(void**)v3625;
    v3627 = _str200;
    v3628 = _str318;
    v3629 = 0LL;
    assert_eq((void*)v3629, v3626, v3627, v3628);
    v3631 = *(void**)v3622;
    v3632 = (char*)v3631 + 8;
    v3633 = *(void**)v3632;
    v3634 = _str201;
    v3635 = _str319;
    v3636 = 0LL;
    assert_eq((void*)v3636, v3633, v3634, v3635);
    v3638 = *(void**)v3622;
    v3639 = (char*)v3638 + 16;
    v3640 = *(void**)v3639;
    v3641 = 0LL;
    v3642 = (int64_t)Vec_len((void*)v3641, v3640);
    v3643 = 1LL;
    v3644 = _str320;
    v3645 = 0LL;
    assert_eq_i64((void*)v3645, v3642, v3643, v3644);
    v3647 = malloc(8);
    v3648 = 0LL;
    v3649 = (char*)v3647 + 0;
    *(int64_t*)v3649 = (int64_t)v3648;
    v3651 = _str321;
    v3652 = malloc(32);
    v3653 = 4LL;
    v3654 = (char*)v3652 + 0;
    *(int64_t*)v3654 = (int64_t)v3653;
    v3656 = _str139;
    v3657 = (char*)v3652 + 8;
    *(void**)v3657 = (void*)v3656;
    v3659 = malloc(48);
    v3660 = 0LL;
    AsmOperand_new(v3659, (void*)v3660, v3647, v3651, v3652);
    v3662 = malloc(8);
    *(void**)v3662 = (void*)v3659;
    v3664 = *(void**)v3662;
    v3665 = (char*)v3664 + 8;
    v3666 = *(void**)v3665;
    v3667 = _str321;
    v3668 = _str322;
    v3669 = 0LL;
    assert_eq((void*)v3669, v3666, v3667, v3668);
    v3671 = *(void**)v3662;
    v3672 = (char*)v3671 + 0;
    v3673 = (char*)v3672 + 0;
    v3674 = *(int64_t*)v3673;
    v3675 = 0LL;
    v3676 = v3674 == v3675;
    if (v3676) goto bb243;
    else goto bb244;
  bb238:
    v3443 = (char*)v3437 + 8;
    v3444 = *(int64_t*)v3443;
    v3445 = 1LL;
    v3446 = _str323;
    v3447 = 0LL;
    assert_eq_i64((void*)v3447, v3444, v3445, v3446);
    goto bb237;
  bb239:
    goto bb240;
  bb240:
    v3451 = _str324;
    p_panic(v3451);
    goto bb237;
  bb241:
    goto bb237;
  bb242:
    v3684 = _str54;
    p_print_str(v3684);
    v3686 = _str325;
    p_print_str(v3686);
    v3688 = _str326;
    v3689 = malloc(40);
    v3690 = 13LL;
    v3691 = (char*)v3689 + 0;
    *(int64_t*)v3691 = (int64_t)v3690;
    v3693 = true;
    v3694 = malloc(88);
    v3695 = 0LL;
    TraitMethod_new(v3694, (void*)v3695, v3688, v3689, v3693);
    v3697 = malloc(8);
    *(void**)v3697 = (void*)v3694;
    v3699 = *(void**)v3697;
    v3700 = (char*)v3699 + 0;
    v3701 = *(void**)v3700;
    v3702 = _str326;
    v3703 = _str327;
    v3704 = 0LL;
    assert_eq((void*)v3704, v3701, v3702, v3703);
    v3706 = *(void**)v3697;
    v3707 = (char*)v3706 + 56;
    v3708 = *(bool*)v3707;
    v3709 = true;
    v3710 = _str328;
    v3711 = 0LL;
    assert_eq_bool((void*)v3711, v3708, v3709, v3710);
    v3713 = *(void**)v3697;
    v3714 = (char*)v3713 + 16;
    v3715 = 0LL;
    v3716 = type_to_str((void*)v3715, v3714);
    v3717 = _str46;
    v3718 = _str329;
    v3719 = 0LL;
    assert_eq((void*)v3719, v3716, v3717, v3718);
    v3721 = 0LL;
    v3722 = Vec_new((void*)v3721);
    v3723 = malloc(8);
    *(void**)v3723 = (void*)v3722;
    v3725 = *(void**)v3723;
    v3726 = *(void**)v3697;
    v3727 = 0LL;
    Vec_push((void*)v3727, v3725, (int64_t)v3726);
    v3729 = _str99;
    v3730 = *(void**)v3723;
    v3731 = malloc(32);
    v3732 = 0LL;
    TraitDef_new(v3731, (void*)v3732, v3729, v3730);
    v3734 = malloc(8);
    *(void**)v3734 = (void*)v3731;
    v3736 = *(void**)v3734;
    v3737 = (char*)v3736 + 0;
    v3738 = *(void**)v3737;
    v3739 = _str99;
    v3740 = _str330;
    v3741 = 0LL;
    assert_eq((void*)v3741, v3738, v3739, v3740);
    v3743 = *(void**)v3734;
    v3744 = (char*)v3743 + 8;
    v3745 = *(void**)v3744;
    v3746 = 0LL;
    v3747 = (int64_t)Vec_len((void*)v3746, v3745);
    v3748 = 1LL;
    v3749 = _str331;
    v3750 = 0LL;
    assert_eq_i64((void*)v3750, v3747, v3748, v3749);
    v3752 = _str99;
    v3753 = _str56;
    v3754 = malloc(40);
    v3755 = 0LL;
    TraitImpl_new(v3754, (void*)v3755, v3752, v3753);
    v3757 = malloc(8);
    *(void**)v3757 = (void*)v3754;
    v3759 = *(void**)v3757;
    v3760 = (char*)v3759 + 0;
    v3761 = *(void**)v3760;
    v3762 = _str99;
    v3763 = _str332;
    v3764 = 0LL;
    assert_eq((void*)v3764, v3761, v3762, v3763);
    v3766 = *(void**)v3757;
    v3767 = (char*)v3766 + 8;
    v3768 = *(void**)v3767;
    v3769 = _str56;
    v3770 = _str333;
    v3771 = 0LL;
    assert_eq((void*)v3771, v3768, v3769, v3770);
    v3773 = _str56;
    v3774 = malloc(24);
    v3775 = 0LL;
    ImplDef_new(v3774, (void*)v3775, v3773);
    v3777 = malloc(8);
    *(void**)v3777 = (void*)v3774;
    v3779 = *(void**)v3777;
    v3780 = (char*)v3779 + 0;
    v3781 = *(void**)v3780;
    v3782 = _str56;
    v3783 = _str334;
    v3784 = 0LL;
    assert_eq((void*)v3784, v3781, v3782, v3783);
    v3786 = _str54;
    p_print_str(v3786);
    v3788 = _str335;
    p_print_str(v3788);
    v3790 = _str336;
    v3791 = malloc(40);
    v3792 = 11LL;
    v3793 = (char*)v3791 + 0;
    *(int64_t*)v3793 = (int64_t)v3792;
    v3795 = malloc(32);
    v3796 = 1LL;
    v3797 = (char*)v3795 + 0;
    *(int64_t*)v3797 = (int64_t)v3796;
    v3799 = ((union { uint64_t u; double f; }){ .u = 4614253070214989087ULL }).f;
    v3800 = (char*)v3795 + 8;
    *(double*)v3800 = v3799;
    v3802 = true;
    v3803 = malloc(88);
    v3804 = 0LL;
    ConstDef_new(v3803, (void*)v3804, v3790, v3791, v3795, v3802);
    v3806 = malloc(8);
    *(void**)v3806 = (void*)v3803;
    v3808 = *(void**)v3806;
    v3809 = (char*)v3808 + 0;
    v3810 = *(void**)v3809;
    v3811 = _str336;
    v3812 = _str337;
    v3813 = 0LL;
    assert_eq((void*)v3813, v3810, v3811, v3812);
    v3815 = *(void**)v3806;
    v3816 = (char*)v3815 + 8;
    v3817 = 0LL;
    v3818 = type_to_str((void*)v3817, v3816);
    v3819 = _str42;
    v3820 = _str338;
    v3821 = 0LL;
    assert_eq((void*)v3821, v3818, v3819, v3820);
    v3823 = _str339;
    v3824 = malloc(40);
    v3825 = 3LL;
    v3826 = (char*)v3824 + 0;
    *(int64_t*)v3826 = (int64_t)v3825;
    v3828 = true;
    v3829 = malloc(56);
    v3830 = 0LL;
    TypeAlias_new(v3829, (void*)v3830, v3823, v3824, v3828);
    v3832 = malloc(8);
    *(void**)v3832 = (void*)v3829;
    v3834 = *(void**)v3832;
    v3835 = (char*)v3834 + 0;
    v3836 = *(void**)v3835;
    v3837 = _str339;
    v3838 = _str340;
    v3839 = 0LL;
    assert_eq((void*)v3839, v3836, v3837, v3838);
    v3841 = *(void**)v3832;
    v3842 = (char*)v3841 + 8;
    v3843 = 0LL;
    v3844 = type_to_str((void*)v3843, v3842);
    v3845 = _str26;
    v3846 = _str341;
    v3847 = 0LL;
    assert_eq((void*)v3847, v3844, v3845, v3846);
    v3849 = _str175;
    v3850 = false;
    v3851 = malloc(72);
    v3852 = 0LL;
    ImportDef_new(v3851, (void*)v3852, v3849, v3850);
    v3854 = malloc(8);
    *(void**)v3854 = (void*)v3851;
    v3856 = *(void**)v3854;
    v3857 = (char*)v3856 + 0;
    v3858 = *(void**)v3857;
    v3859 = _str175;
    v3860 = _str342;
    v3861 = 0LL;
    assert_eq((void*)v3861, v3858, v3859, v3860);
    v3863 = *(void**)v3854;
    v3864 = (char*)v3863 + 64;
    v3865 = *(bool*)v3864;
    v3866 = false;
    v3867 = _str343;
    v3868 = 0LL;
    assert_eq_bool((void*)v3868, v3865, v3866, v3867);
    v3870 = _str344;
    v3871 = 0LL;
    v3872 = Vec_new((void*)v3871);
    v3873 = malloc(16);
    v3874 = 0LL;
    Attribute_new(v3873, (void*)v3874, v3870, v3872);
    v3876 = malloc(8);
    *(void**)v3876 = (void*)v3873;
    v3878 = *(void**)v3876;
    v3879 = (char*)v3878 + 0;
    v3880 = *(void**)v3879;
    v3881 = _str344;
    v3882 = _str345;
    v3883 = 0LL;
    assert_eq((void*)v3883, v3880, v3881, v3882);
    v3885 = _str346;
    v3886 = _str347;
    v3887 = _str347;
    v3888 = malloc(40);
    v3889 = 2LL;
    v3890 = (char*)v3888 + 0;
    *(int64_t*)v3890 = (int64_t)v3889;
    v3892 = malloc(104);
    v3893 = 0LL;
    ExternDecl_new(v3892, (void*)v3893, v3885, v3886, v3887, v3888);
    v3895 = malloc(8);
    *(void**)v3895 = (void*)v3892;
    v3897 = *(void**)v3895;
    v3898 = (char*)v3897 + 0;
    v3899 = *(void**)v3898;
    v3900 = _str346;
    v3901 = _str348;
    v3902 = 0LL;
    assert_eq((void*)v3902, v3899, v3900, v3901);
    v3904 = *(void**)v3895;
    v3905 = (char*)v3904 + 8;
    v3906 = *(void**)v3905;
    v3907 = _str347;
    v3908 = _str349;
    v3909 = 0LL;
    assert_eq((void*)v3909, v3906, v3907, v3908);
    v3911 = *(void**)v3895;
    v3912 = (char*)v3911 + 32;
    v3913 = 0LL;
    v3914 = type_to_str((void*)v3913, v3912);
    v3915 = _str24;
    v3916 = _str350;
    v3917 = 0LL;
    assert_eq((void*)v3917, v3914, v3915, v3916);
    v3919 = _str54;
    p_print_str(v3919);
    v3921 = _str351;
    p_print_str(v3921);
    v3923 = _str185;
    v3924 = true;
    v3925 = malloc(24);
    v3926 = 0LL;
    ComponentDef_new(v3925, (void*)v3926, v3923, v3924);
    v3928 = malloc(8);
    *(void**)v3928 = (void*)v3925;
    v3930 = *(void**)v3928;
    v3931 = (char*)v3930 + 0;
    v3932 = *(void**)v3931;
    v3933 = _str185;
    v3934 = _str352;
    v3935 = 0LL;
    assert_eq((void*)v3935, v3932, v3933, v3934);
    v3937 = _str353;
    v3938 = malloc(32);
    v3939 = 0LL;
    SystemDef_new(v3938, (void*)v3939, v3937);
    v3941 = malloc(8);
    *(void**)v3941 = (void*)v3938;
    v3943 = *(void**)v3941;
    v3944 = (char*)v3943 + 0;
    v3945 = *(void**)v3944;
    v3946 = _str353;
    v3947 = _str354;
    v3948 = 0LL;
    assert_eq((void*)v3948, v3945, v3946, v3947);
    v3950 = _str355;
    v3951 = true;
    v3952 = malloc(24);
    v3953 = 0LL;
    StateMachineDef_new(v3952, (void*)v3953, v3950, v3951);
    v3955 = malloc(8);
    *(void**)v3955 = (void*)v3952;
    v3957 = *(void**)v3955;
    v3958 = (char*)v3957 + 0;
    v3959 = *(void**)v3958;
    v3960 = _str355;
    v3961 = _str356;
    v3962 = 0LL;
    assert_eq((void*)v3962, v3959, v3960, v3961);
    v3964 = _str357;
    v3965 = malloc(16);
    v3966 = 0LL;
    StateDef_new(v3965, (void*)v3966, v3964);
    v3968 = malloc(8);
    *(void**)v3968 = (void*)v3965;
    v3970 = *(void**)v3968;
    v3971 = (char*)v3970 + 0;
    v3972 = *(void**)v3971;
    v3973 = _str357;
    v3974 = _str358;
    v3975 = 0LL;
    assert_eq((void*)v3975, v3972, v3973, v3974);
    v3977 = _str359;
    v3978 = _str360;
    v3979 = malloc(16);
    v3980 = 0LL;
    StateTransition_new(v3979, (void*)v3980, v3977, v3978);
    v3982 = malloc(8);
    *(void**)v3982 = (void*)v3979;
    v3984 = *(void**)v3982;
    v3985 = (char*)v3984 + 0;
    v3986 = *(void**)v3985;
    v3987 = _str359;
    v3988 = _str361;
    v3989 = 0LL;
    assert_eq((void*)v3989, v3986, v3987, v3988);
    v3991 = *(void**)v3982;
    v3992 = (char*)v3991 + 8;
    v3993 = *(void**)v3992;
    v3994 = _str360;
    v3995 = _str362;
    v3996 = 0LL;
    assert_eq((void*)v3996, v3993, v3994, v3995);
    v3998 = _str363;
    v3999 = malloc(32);
    v4000 = 0LL;
    EventHandlerDef_new(v3999, (void*)v4000, v3998);
    v4002 = malloc(8);
    *(void**)v4002 = (void*)v3999;
    v4004 = *(void**)v4002;
    v4005 = (char*)v4004 + 0;
    v4006 = *(void**)v4005;
    v4007 = _str363;
    v4008 = _str364;
    v4009 = 0LL;
    assert_eq((void*)v4009, v4006, v4007, v4008);
    v4011 = _str54;
    p_print_str(v4011);
    v4013 = _str365;
    p_print_str(v4013);
    v4015 = _str308;
    v4016 = *(void**)v2934;
    v4017 = true;
    v4018 = malloc(192);
    v4019 = 0LL;
    LayerDef_new(v4018, (void*)v4019, v4015, v4016, v4017);
    v4021 = malloc(8);
    *(void**)v4021 = (void*)v4018;
    v4023 = *(void**)v4021;
    v4024 = (char*)v4023 + 0;
    v4025 = *(void**)v4024;
    v4026 = _str308;
    v4027 = _str366;
    v4028 = 0LL;
    assert_eq((void*)v4028, v4025, v4026, v4027);
    v4030 = 0LL;
    v4031 = Vec_new((void*)v4030);
    v4032 = malloc(8);
    *(void**)v4032 = (void*)v4031;
    v4034 = *(void**)v4032;
    v4035 = _str307;
    v4036 = _str308;
    v4037 = 0LL;
    v4038 = Vec_new((void*)v4037);
    v4039 = malloc(24);
    v4040 = 0LL;
    LayerInst_new(v4039, (void*)v4040, v4035, v4036, v4038);
    v4042 = 0LL;
    Vec_push((void*)v4042, v4034, (int64_t)v4039);
    v4044 = _str367;
    v4045 = *(void**)v4032;
    v4046 = *(void**)v2934;
    v4047 = true;
    v4048 = malloc(184);
    v4049 = 0LL;
    ModelDef_new(v4048, (void*)v4049, v4044, v4045, v4046, v4047);
    v4051 = malloc(8);
    *(void**)v4051 = (void*)v4048;
    v4053 = *(void**)v4051;
    v4054 = (char*)v4053 + 0;
    v4055 = *(void**)v4054;
    v4056 = _str367;
    v4057 = _str368;
    v4058 = 0LL;
    assert_eq((void*)v4058, v4055, v4056, v4057);
    v4060 = *(void**)v4051;
    v4061 = (char*)v4060 + 8;
    v4062 = *(void**)v4061;
    v4063 = 0LL;
    v4064 = (int64_t)Vec_len((void*)v4063, v4062);
    v4065 = 1LL;
    v4066 = _str369;
    v4067 = 0LL;
    assert_eq_i64((void*)v4067, v4064, v4065, v4066);
    v4069 = _str54;
    p_print_str(v4069);
    v4071 = _str370;
    p_print_str(v4071);
    v4073 = malloc(8);
    v4074 = 0LL;
    v4075 = (char*)v4073 + 0;
    *(int64_t*)v4075 = (int64_t)v4074;
    v4077 = (char*)v4073 + 0;
    v4078 = *(int64_t*)v4077;
    v4079 = 0LL;
    v4080 = v4078 == v4079;
    if (v4080) goto bb248;
    else goto bb249;
  bb243:
    goto bb242;
  bb244:
    goto bb245;
  bb245:
    v3680 = _str371;
    p_panic(v3680);
    goto bb242;
  bb246:
    goto bb242;
  bb247:
    v4088 = malloc(8);
    v4089 = 17LL;
    v4090 = (char*)v4088 + 0;
    *(int64_t*)v4090 = (int64_t)v4089;
    v4092 = (char*)v4088 + 0;
    v4093 = *(int64_t*)v4092;
    v4094 = 17LL;
    v4095 = v4093 == v4094;
    if (v4095) goto bb253;
    else goto bb254;
  bb248:
    goto bb247;
  bb249:
    goto bb250;
  bb250:
    v4084 = _str372;
    p_panic(v4084);
    goto bb247;
  bb251:
    goto bb247;
  bb252:
    v4103 = malloc(8);
    v4104 = 0LL;
    v4105 = (char*)v4103 + 0;
    *(int64_t*)v4105 = (int64_t)v4104;
    v4107 = (char*)v4103 + 0;
    v4108 = *(int64_t*)v4107;
    v4109 = 0LL;
    v4110 = v4108 == v4109;
    if (v4110) goto bb258;
    else goto bb259;
  bb253:
    goto bb252;
  bb254:
    goto bb255;
  bb255:
    v4099 = _str373;
    p_panic(v4099);
    goto bb252;
  bb256:
    goto bb252;
  bb257:
    v4118 = malloc(8);
    v4119 = 2LL;
    v4120 = (char*)v4118 + 0;
    *(int64_t*)v4120 = (int64_t)v4119;
    v4122 = (char*)v4118 + 0;
    v4123 = *(int64_t*)v4122;
    v4124 = 2LL;
    v4125 = v4123 == v4124;
    if (v4125) goto bb263;
    else goto bb264;
  bb258:
    goto bb257;
  bb259:
    goto bb260;
  bb260:
    v4114 = _str374;
    p_panic(v4114);
    goto bb257;
  bb261:
    goto bb257;
  bb262:
    v4133 = malloc(8);
    v4134 = 1LL;
    v4135 = (char*)v4133 + 0;
    *(int64_t*)v4135 = (int64_t)v4134;
    v4137 = (char*)v4133 + 0;
    v4138 = *(int64_t*)v4137;
    v4139 = 1LL;
    v4140 = v4138 == v4139;
    if (v4140) goto bb268;
    else goto bb269;
  bb263:
    goto bb262;
  bb264:
    goto bb265;
  bb265:
    v4129 = _str375;
    p_panic(v4129);
    goto bb262;
  bb266:
    goto bb262;
  bb267:
    v4148 = malloc(8);
    v4149 = 10LL;
    v4150 = (char*)v4148 + 0;
    *(int64_t*)v4150 = (int64_t)v4149;
    v4152 = (char*)v4148 + 0;
    v4153 = *(int64_t*)v4152;
    v4154 = 10LL;
    v4155 = v4153 == v4154;
    if (v4155) goto bb273;
    else goto bb274;
  bb268:
    goto bb267;
  bb269:
    goto bb270;
  bb270:
    v4144 = _str376;
    p_panic(v4144);
    goto bb267;
  bb271:
    goto bb267;
  bb272:
    v4163 = malloc(8);
    v4164 = 0LL;
    v4165 = (char*)v4163 + 0;
    *(int64_t*)v4165 = (int64_t)v4164;
    v4167 = (char*)v4163 + 0;
    v4168 = *(int64_t*)v4167;
    v4169 = 0LL;
    v4170 = v4168 == v4169;
    if (v4170) goto bb278;
    else goto bb279;
  bb273:
    goto bb272;
  bb274:
    goto bb275;
  bb275:
    v4159 = _str377;
    p_panic(v4159);
    goto bb272;
  bb276:
    goto bb272;
  bb277:
    v4178 = malloc(8);
    v4179 = 6LL;
    v4180 = (char*)v4178 + 0;
    *(int64_t*)v4180 = (int64_t)v4179;
    v4182 = (char*)v4178 + 0;
    v4183 = *(int64_t*)v4182;
    v4184 = 6LL;
    v4185 = v4183 == v4184;
    if (v4185) goto bb283;
    else goto bb284;
  bb278:
    goto bb277;
  bb279:
    goto bb280;
  bb280:
    v4174 = _str378;
    p_panic(v4174);
    goto bb277;
  bb281:
    goto bb277;
  bb282:
    v4193 = malloc(8);
    v4194 = 1LL;
    v4195 = (char*)v4193 + 0;
    *(int64_t*)v4195 = (int64_t)v4194;
    v4197 = (char*)v4193 + 0;
    v4198 = *(int64_t*)v4197;
    v4199 = 1LL;
    v4200 = v4198 == v4199;
    if (v4200) goto bb288;
    else goto bb289;
  bb283:
    goto bb282;
  bb284:
    goto bb285;
  bb285:
    v4189 = _str379;
    p_panic(v4189);
    goto bb282;
  bb286:
    goto bb282;
  bb287:
    v4208 = malloc(16);
    v4209 = 0LL;
    v4210 = (char*)v4208 + 0;
    *(int64_t*)v4210 = (int64_t)v4209;
    v4212 = _str290;
    v4213 = (char*)v4208 + 8;
    *(void**)v4213 = (void*)v4212;
    v4215 = (char*)v4208 + 0;
    v4216 = *(int64_t*)v4215;
    v4217 = 0LL;
    v4218 = v4216 == v4217;
    if (v4218) goto bb293;
    else goto bb294;
  bb288:
    goto bb287;
  bb289:
    goto bb290;
  bb290:
    v4204 = _str380;
    p_panic(v4204);
    goto bb287;
  bb291:
    goto bb287;
  bb292:
    v4232 = malloc(16);
    v4233 = 1LL;
    v4234 = (char*)v4232 + 0;
    *(int64_t*)v4234 = (int64_t)v4233;
    v4236 = malloc(32);
    v4237 = 0LL;
    v4238 = (char*)v4236 + 0;
    *(int64_t*)v4238 = (int64_t)v4237;
    v4240 = 1LL;
    v4241 = (char*)v4236 + 8;
    *(int64_t*)v4241 = (int64_t)v4240;
    v4243 = (char*)v4232 + 8;
    *(void**)v4243 = (void*)v4236;
    v4245 = (char*)v4232 + 0;
    v4246 = *(int64_t*)v4245;
    v4247 = 1LL;
    v4248 = v4246 == v4247;
    if (v4248) goto bb298;
    else goto bb299;
  bb293:
    v4220 = (char*)v4208 + 8;
    v4221 = *(void**)v4220;
    v4222 = _str290;
    v4223 = _str381;
    v4224 = 0LL;
    assert_eq((void*)v4224, v4221, v4222, v4223);
    goto bb292;
  bb294:
    goto bb295;
  bb295:
    v4228 = _str381;
    p_panic(v4228);
    goto bb292;
  bb296:
    goto bb292;
  bb297:
    v4275 = _str54;
    p_print_str(v4275);
    v4277 = _str382;
    p_print_str(v4277);
    v4279 = _str383;
    v4280 = malloc(40);
    v4281 = 14LL;
    v4282 = (char*)v4280 + 0;
    *(int64_t*)v4282 = (int64_t)v4281;
    v4284 = malloc(160);
    v4285 = 0LL;
    Func_new(v4284, (void*)v4285, v4279, v4280);
    v4287 = malloc(8);
    *(void**)v4287 = (void*)v4284;
    v4289 = *(void**)v4287;
    v4290 = (char*)v4289 + 88;
    v4291 = true;
    *(bool*)v4290 = v4291;
    v4293 = *(void**)v4287;
    v4294 = (char*)v4293 + 112;
    v4295 = true;
    *(bool*)v4294 = v4295;
    v4297 = *(void**)v4287;
    v4298 = (char*)v4297 + 16;
    v4299 = *(void**)v4298;
    v4300 = _str64;
    v4301 = _str299;
    v4302 = malloc(16);
    v4303 = 0LL;
    StrPair_new(v4302, (void*)v4303, v4300, v4301);
    v4305 = 0LL;
    Vec_push((void*)v4305, v4299, (int64_t)v4302);
    v4307 = *(void**)v4287;
    v4308 = (char*)v4307 + 16;
    v4309 = *(void**)v4308;
    v4310 = _str384;
    v4311 = _str99;
    v4312 = malloc(16);
    v4313 = 0LL;
    StrPair_new(v4312, (void*)v4313, v4310, v4311);
    v4315 = 0LL;
    Vec_push((void*)v4315, v4309, (int64_t)v4312);
    v4317 = *(void**)v4287;
    v4318 = (char*)v4317 + 24;
    v4319 = *(void**)v4318;
    v4320 = _str65;
    v4321 = _str26;
    v4322 = malloc(16);
    v4323 = 0LL;
    StrPair_new(v4322, (void*)v4323, v4320, v4321);
    v4325 = 0LL;
    Vec_push((void*)v4325, v4319, (int64_t)v4322);
    v4327 = *(void**)v4287;
    v4328 = (char*)v4327 + 16;
    v4329 = *(void**)v4328;
    v4330 = 0LL;
    v4331 = (int64_t)Vec_len((void*)v4330, v4329);
    v4332 = 2LL;
    v4333 = _str385;
    v4334 = 0LL;
    assert_eq_i64((void*)v4334, v4331, v4332, v4333);
    v4336 = *(void**)v4287;
    v4337 = (char*)v4336 + 16;
    v4338 = *(void**)v4337;
    v4339 = 0LL;
    v4340 = 0LL;
    v4341 = (void*)Vec_get((void*)v4340, v4338, v4339);
    v4342 = malloc(8);
    *(void**)v4342 = (void*)v4341;
    v4344 = *(void**)v4342;
    v4345 = (char*)v4344 + 0;
    v4346 = *(void**)v4345;
    v4347 = _str64;
    v4348 = _str386;
    v4349 = 0LL;
    assert_eq((void*)v4349, v4346, v4347, v4348);
    v4351 = *(void**)v4342;
    v4352 = (char*)v4351 + 8;
    v4353 = *(void**)v4352;
    v4354 = _str299;
    v4355 = _str387;
    v4356 = 0LL;
    assert_eq((void*)v4356, v4353, v4354, v4355);
    v4358 = *(void**)v4287;
    v4359 = (char*)v4358 + 24;
    v4360 = *(void**)v4359;
    v4361 = 0LL;
    v4362 = (int64_t)Vec_len((void*)v4361, v4360);
    v4363 = 1LL;
    v4364 = _str388;
    v4365 = 0LL;
    assert_eq_i64((void*)v4365, v4362, v4363, v4364);
    v4367 = *(void**)v4287;
    v4368 = (char*)v4367 + 24;
    v4369 = *(void**)v4368;
    v4370 = 0LL;
    v4371 = 0LL;
    v4372 = (void*)Vec_get((void*)v4371, v4369, v4370);
    v4373 = malloc(8);
    *(void**)v4373 = (void*)v4372;
    v4375 = *(void**)v4373;
    v4376 = (char*)v4375 + 0;
    v4377 = *(void**)v4376;
    v4378 = _str65;
    v4379 = _str389;
    v4380 = 0LL;
    assert_eq((void*)v4380, v4377, v4378, v4379);
    v4382 = *(void**)v4373;
    v4383 = (char*)v4382 + 8;
    v4384 = *(void**)v4383;
    v4385 = _str26;
    v4386 = _str390;
    v4387 = 0LL;
    assert_eq((void*)v4387, v4384, v4385, v4386);
    v4389 = *(void**)v4287;
    v4390 = (char*)v4389 + 112;
    v4391 = *(bool*)v4390;
    v4392 = true;
    v4393 = _str391;
    v4394 = 0LL;
    assert_eq_bool((void*)v4394, v4391, v4392, v4393);
    v4396 = _str54;
    p_print_str(v4396);
    v4398 = _str392;
    p_print_str(v4398);
    v4400 = 0LL;
    p_call_stack_pop();
    return v4400;
  bb298:
    v4250 = (char*)v4232 + 8;
    v4251 = *(void**)v4250;
    v4252 = (char*)v4251 + 0;
    v4253 = *(int64_t*)v4252;
    v4254 = 0LL;
    v4255 = v4253 == v4254;
    if (v4255) goto bb301;
    else goto bb302;
  bb299:
    goto bb305;
  bb300:
    goto bb297;
  bb301:
    v4257 = (char*)v4251 + 8;
    v4258 = *(int64_t*)v4257;
    v4259 = 1LL;
    v4260 = _str393;
    v4261 = 0LL;
    assert_eq_i64((void*)v4261, v4258, v4259, v4260);
    goto bb300;
  bb302:
    goto bb303;
  bb303:
    v4265 = _str394;
    p_panic(v4265);
    goto bb300;
  bb304:
    goto bb300;
  bb305:
    v4271 = _str393;
    p_panic(v4271);
    goto bb297;
  bb306:
    goto bb297;
    return 0;
}

bool Vec_is_empty(void* v0, void* v1) {
    p_call_stack_push("Vec_is_empty");
    int64_t v3; /* placeholder for void */
    bool v2;
    v2 = (bool)p_vec_is_empty(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void EffectHandler_new(void* v0, void* v1, void* v2, void* v3, void* v4, void* v5) {
    p_call_stack_push("EffectHandler_new");
    int64_t v8; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    void* v21;
    void* v27;
    void* v9;
    int64_t v14; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    void* v19;
    void* v7;
    int64_t v18; /* placeholder for void */
    void* v23;
    int64_t v10; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    void* v13;
    void* v17;
    int64_t v26; /* placeholder for void */
    void* v28;
    void* v16;
    void* v11;
    void* v29;
    void* v20;
    void* v24;
    void* v25;
    void* v15;
    void* v6;
    v6 = malloc(32);
    v7 = (char*)v6 + 0;
    *(void**)v7 = (void*)v2;
    v9 = (char*)v6 + 8;
    *(void**)v9 = (void*)v3;
    v11 = (char*)v6 + 16;
    *(void**)v11 = (void*)v4;
    v13 = (char*)v6 + 24;
    *(void**)v13 = (void*)v5;
    v15 = (char*)v6 + 0;
    v16 = (char*)v0 + 0;
    v17 = *(void**)v15;
    *(void**)v16 = (void*)v17;
    v19 = (char*)v6 + 8;
    v20 = (char*)v0 + 8;
    v21 = *(void**)v19;
    *(void**)v20 = (void*)v21;
    v23 = (char*)v6 + 16;
    v24 = (char*)v0 + 16;
    v25 = *(void**)v23;
    *(void**)v24 = (void*)v25;
    v27 = (char*)v6 + 24;
    v28 = (char*)v0 + 24;
    v29 = *(void**)v27;
    *(void**)v28 = (void*)v29;
    p_call_stack_pop();
    return;
    return;
}

int64_t String_push_str(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_push_str");
    int64_t v26;
    int64_t v12;
    int64_t v14;
    int64_t v6; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v3;
    int64_t v7;
    void* v9;
    int64_t v18; /* placeholder for void */
    int64_t v19;
    void* v37;
    int64_t v28; /* placeholder for void */
    int64_t v20;
    int64_t v33;
    void* v17;
    int64_t v44;
    int64_t v36;
    int64_t v8;
    int64_t v32;
    int64_t v10; /* placeholder for void */
    int64_t v23;
    int64_t v29;
    int64_t v30;
    int64_t v11;
    int64_t v21;
    int64_t v24;
    void* v40;
    int64_t v25;
    int64_t v13;
    int64_t v31;
    int64_t v42; /* placeholder for void */
    void* v16;
    int64_t v35; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    void* v34;
    int64_t v4;
    int64_t v15;
    int64_t v41;
    void* v5;
    int64_t v22; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v27;
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

bool char_is_upper(void* v0, int64_t v1) {
    p_call_stack_push("char_is_upper");
    bool v6;
    int64_t v4;
    int64_t v7; /* placeholder for void */
    int64_t v2;
    bool v3;
    bool v5;
    v2 = 65LL;
    v3 = v1 >= v2;
    v4 = 90LL;
    v5 = v1 <= v4;
    v6 = v3 && v5;
    p_call_stack_pop();
    return v6;
    return 0;
}

void StringBuilder_clear(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_clear");
    void* v2;
    void* v5;
    int64_t v6;
    int64_t v4; /* placeholder for void */
    void* v3;
    int64_t v7; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = _str5;
    *(void**)v2 = (void*)v3;
    v5 = (char*)v1 + 8;
    v6 = 0LL;
    *(int64_t*)v5 = (int64_t)v6;
    p_call_stack_pop();
    return;
    return;
}

int64_t p_str_hash(void* v0, int64_t v1) {
    p_call_stack_push("p_str_hash");
    int64_t v11; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v40;
    int64_t v3;
    bool v19;
    int64_t v17;
    int64_t v7; /* placeholder for void */
    void* v12;
    int64_t v8;
    int64_t v23;
    int64_t v30;
    uint8_t v36;
    uint8_t v13;
    void* v9;
    int64_t v39; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v33;
    int64_t v37; /* placeholder for void */
    int64_t v45;
    int64_t v47; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v44;
    void* v35;
    int64_t v38; /* placeholder for void */
    bool v42;
    void* v14;
    int64_t v20; /* placeholder for void */
    int64_t v25;
    int64_t v16; /* placeholder for void */
    int64_t v2;
    int64_t v26;
    int64_t v15; /* placeholder for void */
    int64_t v31;
    int64_t v21;
    void* v6;
    int64_t v48;
    int64_t v18;
    void* v4;
    int64_t v46; /* placeholder for void */
    int64_t v41;
    int64_t v29;
    int64_t v32; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v27;
    int64_t v22;
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

int64_t p_str_compare(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("p_str_compare");
    int64_t v30; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    void* v38;
    int64_t v18;
    int64_t v35; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v59;
    int64_t v17; /* placeholder for void */
    void* v26;
    int64_t v31; /* placeholder for void */
    bool v47;
    int64_t v13; /* placeholder for void */
    uint8_t v33;
    int64_t v6; /* placeholder for void */
    int64_t v55;
    bool v9;
    int64_t v64;
    void* v73;
    uint8_t v37;
    int64_t v24; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v11;
    void* v21;
    bool v51;
    int64_t v52; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    int64_t v62;
    bool v46;
    int64_t v77;
    int64_t v78;
    void* v32;
    void* v29;
    int64_t v67; /* placeholder for void */
    void* v23;
    bool v43;
    int64_t v76; /* placeholder for void */
    int64_t v41;
    int64_t v15;
    int64_t v12;
    int64_t v48; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v66;
    int64_t v56; /* placeholder for void */
    void* v34;
    int64_t v72; /* placeholder for void */
    int64_t v5;
    int64_t v49;
    int64_t v22; /* placeholder for void */
    int64_t v28;
    int64_t v10; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v8;
    int64_t v60;
    int64_t v44;
    int64_t v14; /* placeholder for void */
    int64_t v45;
    int64_t v25;
    int64_t v50;
    int64_t v58;
    int64_t v68;
    int64_t v42;
    int64_t v63;
    void* v36;
    bool v16;
    int64_t v19; /* placeholder for void */
    void* v70;
    uint8_t v74;
    bool v3;
    uint8_t v71;
    int64_t v27; /* placeholder for void */
    int64_t v80; /* placeholder for void */
    int64_t v54;
    int64_t v61; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v65; /* placeholder for void */
    int64_t v79;
    int64_t v20; /* placeholder for void */
    int64_t v53;
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

int64_t p_min(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("min");
    bool v3;
    int64_t v5; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v6; /* placeholder for void */
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

void TraitDef_new(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("TraitDef_new");
    void* v14;
    void* v18;
    int64_t v31; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    void* v20;
    void* v16;
    void* v29;
    void* v24;
    void* v7;
    void* v22;
    void* v26;
    void* v28;
    int64_t v8; /* placeholder for void */
    bool v13;
    void* v21;
    void* v25;
    int64_t v9;
    void* v10;
    void* v17;
    void* v5;
    bool v30;
    int64_t v27; /* placeholder for void */
    void* v11;
    int64_t v15; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    void* v4;
    int64_t v23; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    v4 = malloc(32);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = (char*)v4 + 8;
    *(void**)v7 = (void*)v3;
    v9 = 0LL;
    v10 = Vec_new((void*)v9);
    v11 = (char*)v4 + 16;
    *(void**)v11 = (void*)v10;
    v13 = true;
    v14 = (char*)v4 + 24;
    *(bool*)v14 = v13;
    v16 = (char*)v4 + 0;
    v17 = (char*)v0 + 0;
    v18 = *(void**)v16;
    *(void**)v17 = (void*)v18;
    v20 = (char*)v4 + 8;
    v21 = (char*)v0 + 8;
    v22 = *(void**)v20;
    *(void**)v21 = (void*)v22;
    v24 = (char*)v4 + 16;
    v25 = (char*)v0 + 16;
    v26 = *(void**)v24;
    *(void**)v25 = (void*)v26;
    v28 = (char*)v4 + 24;
    v29 = (char*)v0 + 24;
    v30 = *(bool*)v28;
    *(bool*)v29 = v30;
    p_call_stack_pop();
    return;
    return;
}

bool char_is_whitespace(void* v0, int64_t v1) {
    p_call_stack_push("char_is_whitespace");
    int64_t v4;
    bool v6;
    int64_t v7;
    int64_t v2;
    bool v3;
    bool v8;
    bool v9;
    int64_t v10;
    bool v5;
    bool v11;
    bool v12;
    int64_t v13; /* placeholder for void */
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

int64_t Option_expect(void* v0, void* v1, void* v2) {
    p_call_stack_push("Option_expect");
    bool v14;
    int64_t v15; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v13;
    int64_t v20; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v8;
    int64_t v9;
    bool v6;
    void* v11;
    int64_t v16; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v5;
    int64_t v17; /* placeholder for void */
    int64_t v12;
    void* v3;
    int64_t v4;
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
    p_panic(v2);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

void Option_or_else(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Option_or_else");
    int64_t v10;
    int64_t v27;
    int64_t v5;
    void* v9;
    int64_t v20; /* placeholder for void */
    void* v17;
    void* v26;
    void* v38;
    int64_t v8; /* placeholder for void */
    void* v22;
    bool v29;
    void* v31;
    void* v4;
    int64_t v30; /* placeholder for void */
    void* v43;
    int64_t v16; /* placeholder for void */
    int64_t v12;
    void* v15;
    void* v13;
    int64_t v23;
    void* v18;
    int64_t v32;
    int64_t v35;
    bool v7;
    void* v36;
    int64_t v40;
    int64_t v41; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v28;
    void* v39;
    int64_t v46; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    void* v33;
    void* v11;
    int64_t v6;
    void* v42;
    int64_t v44;
    int64_t v37; /* placeholder for void */
    int64_t v19;
    int64_t v14; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    void* v21;
    int64_t v47; /* placeholder for void */
    int64_t v24; /* placeholder for void */
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
        if (strcmp(_fn_name, "HashMap_new") == 0) {
            v35 = HashMap_new((void*)v3);
        } else if (strcmp(_fn_name, "main") == 0) {
            v35 = main_entry((void*)v3);
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

int64_t HashMap_get_or(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("HashMap_get_or");
    int64_t v11; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v33;
    int64_t v53; /* placeholder for void */
    void* v50;
    int64_t v23;
    int64_t v31; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    void* v67;
    int64_t v84;
    bool v61;
    void* v85;
    int64_t v93; /* placeholder for void */
    int64_t v40;
    int64_t v8;
    int64_t v28;
    int64_t v30; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v36;
    int64_t v42; /* placeholder for void */
    void* v24;
    int64_t v52; /* placeholder for void */
    int64_t v54;
    int64_t v55;
    int64_t v14; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    int64_t v5;
    int64_t v81;
    void* v91;
    int64_t v94; /* placeholder for void */
    void* v88;
    int64_t v45; /* placeholder for void */
    bool v60;
    int64_t v27; /* placeholder for void */
    void* v82;
    void* v29;
    int64_t v4;
    int64_t v16;
    int64_t v68; /* placeholder for void */
    void* v47;
    int64_t v10; /* placeholder for void */
    void* v44;
    int64_t v19; /* placeholder for void */
    int64_t v77;
    int64_t v79;
    int64_t v87;
    void* v41;
    int64_t v75; /* placeholder for void */
    void* v12;
    int64_t v38;
    int64_t v58;
    int64_t v39;
    int64_t v59;
    int64_t v22;
    int64_t v96; /* placeholder for void */
    int64_t v95;
    int64_t v71; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v66;
    int64_t v37;
    bool v56;
    int64_t v21; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    void* v9;
    int64_t v34; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    void* v6;
    int64_t v63; /* placeholder for void */
    int64_t v80;
    void* v73;
    int64_t v13;
    int64_t v46;
    int64_t v69;
    int64_t v78;
    int64_t v83; /* placeholder for void */
    int64_t v89; /* placeholder for void */
    int64_t v43;
    int64_t v74;
    int64_t v65;
    void* v26;
    int64_t v90; /* placeholder for void */
    int64_t v92;
    int64_t v57;
    void* v32;
    int64_t v17;
    bool v18;
    void* v70;
    int64_t v7; /* placeholder for void */
    int64_t v51;
    int64_t v64; /* placeholder for void */
    int64_t v72; /* placeholder for void */
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

int64_t String_from(void* v0, int64_t v1) {
    p_call_stack_push("String_from");
    int64_t v33; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    int64_t v31;
    int64_t v8; /* placeholder for void */
    void* v10;
    int64_t v21;
    void* v27;
    int64_t v2;
    int64_t v11; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    void* v22;
    int64_t v23; /* placeholder for void */
    int64_t v30;
    int64_t v32;
    int64_t v36;
    int64_t v24;
    int64_t v39;
    int64_t v48; /* placeholder for void */
    void* v43;
    int64_t v26;
    void* v37;
    int64_t v41; /* placeholder for void */
    int64_t v17;
    void* v7;
    void* v13;
    int64_t v16; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v9;
    int64_t v38; /* placeholder for void */
    void* v28;
    void* v40;
    int64_t v14;
    int64_t v44;
    void* v6;
    int64_t v5;
    bool v3;
    int64_t v45; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v20;
    int64_t v25;
    int64_t v29; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v47;
    int64_t v35;
    int64_t v34;
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

void Vec_clear(void* v0, void* v1) {
    p_call_stack_push("Vec_clear");
    int64_t v2; /* placeholder for void */
    int64_t v3; /* placeholder for void */
    p_vec_clear(v1);
    p_call_stack_pop();
    return;
    return;
}

int64_t HashMap_get(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_get");
    void* v8;
    int64_t v75;
    int64_t v81;
    int64_t v9; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v67;
    int64_t v13; /* placeholder for void */
    void* v83;
    int64_t v88;
    int64_t v62; /* placeholder for void */
    int64_t v93;
    void* v41;
    int64_t v91; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v79;
    int64_t v25; /* placeholder for void */
    void* v11;
    int64_t v40;
    int64_t v28;
    int64_t v65; /* placeholder for void */
    int64_t v87; /* placeholder for void */
    int64_t v66;
    int64_t v96;
    int64_t v84; /* placeholder for void */
    void* v24;
    bool v56;
    int64_t v53; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    void* v29;
    int64_t v85;
    void* v50;
    int64_t v7;
    int64_t v23;
    void* v44;
    int64_t v27; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    void* v5;
    int64_t v59;
    bool v60;
    int64_t v3;
    int64_t v30; /* placeholder for void */
    int64_t v36;
    int64_t v16;
    int64_t v19;
    int64_t v64; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v70;
    int64_t v72; /* placeholder for void */
    void* v74;
    int64_t v94; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    int64_t v37;
    int64_t v46;
    int64_t v38;
    void* v71;
    int64_t v77; /* placeholder for void */
    void* v89;
    int64_t v22;
    void* v26;
    int64_t v57;
    int64_t v14; /* placeholder for void */
    void* v32;
    int64_t v34; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v33;
    int64_t v90; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v78;
    void* v92;
    int64_t v52; /* placeholder for void */
    int64_t v73; /* placeholder for void */
    int64_t v80;
    bool v17;
    int64_t v54;
    int64_t v55;
    void* v68;
    int64_t v82;
    int64_t v21; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    bool v61;
    void* v86;
    void* v47;
    int64_t v4;
    int64_t v15;
    int64_t v12;
    int64_t v39;
    int64_t v51;
    int64_t v58;
    int64_t v43;
    int64_t v63;
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

void HashMap_clear(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_clear");
    int64_t v64;
    int64_t v26; /* placeholder for void */
    bool v33;
    int64_t v8;
    void* v7;
    int64_t v20;
    int64_t v65; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v27;
    int64_t v34;
    int64_t v48;
    int64_t v13;
    bool v36;
    int64_t v51;
    int64_t v41; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v2;
    int64_t v3;
    int64_t v57; /* placeholder for void */
    int64_t v35;
    int64_t v42; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    void* v40;
    bool v37;
    int64_t v60; /* placeholder for void */
    void* v18;
    int64_t v67; /* placeholder for void */
    int64_t v47;
    bool v45;
    int64_t v54;
    int64_t v38; /* placeholder for void */
    void* v52;
    int64_t v14;
    int64_t v31;
    int64_t v32;
    void* v4;
    int64_t v66; /* placeholder for void */
    int64_t v50;
    int64_t v56; /* placeholder for void */
    void* v24;
    int64_t v69; /* placeholder for void */
    int64_t v43;
    void* v21;
    int64_t v16; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v17;
    int64_t v62;
    int64_t v5; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v61; /* placeholder for void */
    void* v58;
    void* v11;
    int64_t v44;
    int64_t v63;
    int64_t v9; /* placeholder for void */
    int64_t v28;
    int64_t v19; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v59;
    int64_t v49;
    void* v15;
    int64_t v39;
    void* v55;
    int64_t v68; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    int64_t v25;
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

void* str_split_char(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("str_split_char");
    int64_t v50; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v59;
    int64_t v60;
    uint8_t v34;
    int64_t v47;
    int64_t v14;
    void* v12;
    int64_t v7;
    int64_t v25; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v42;
    int64_t v37;
    void* v65;
    int64_t v66; /* placeholder for void */
    int64_t v43;
    void* v30;
    bool v20;
    int64_t v22;
    int64_t v28; /* placeholder for void */
    void* v33;
    int64_t v39; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v55;
    int64_t v16; /* placeholder for void */
    int64_t v18;
    int64_t v64; /* placeholder for void */
    void* v8;
    void* v4;
    void* v5;
    void* v40;
    int64_t v41;
    int64_t v63;
    void* v27;
    int64_t v48;
    int64_t v29;
    int64_t v10;
    int64_t v3;
    int64_t v62;
    int64_t v54;
    void* v15;
    int64_t v21; /* placeholder for void */
    bool v38;
    int64_t v56; /* placeholder for void */
    int64_t v26;
    int64_t v35; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v11;
    int64_t v57; /* placeholder for void */
    int64_t v44;
    int64_t v45;
    int64_t v52; /* placeholder for void */
    int64_t v49;
    void* v58;
    int64_t v61;
    int64_t v23;
    void* v24;
    int64_t v13; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v53;
    int64_t v6; /* placeholder for void */
    int64_t v19;
    v3 = 0LL;
    v4 = Vec_new((void*)v3);
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
    Vec_push((void*)v63, v58, v62);
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
    Vec_push((void*)v45, v40, v44);
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

bool Vec_contains(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_contains");
    bool v3;
    int64_t v4; /* placeholder for void */
    v3 = (bool)p_vec_contains(v1, (void*)v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t HashMap_len(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_len");
    int64_t v3;
    void* v10;
    int64_t v14;
    int64_t v12; /* placeholder for void */
    int64_t v2;
    int64_t v6;
    int64_t v13; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    void* v7;
    int64_t v9; /* placeholder for void */
    void* v4;
    int64_t v15; /* placeholder for void */
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
    p_call_stack_pop();
    return v14;
    return 0;
}

void StringBuilder_append_char(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("StringBuilder_append_char");
    int64_t v13;
    int64_t v14; /* placeholder for void */
    void* v5;
    int64_t v11;
    void* v10;
    int64_t v15; /* placeholder for void */
    void* v4;
    void* v6;
    void* v3;
    void* v9;
    int64_t v12;
    void* v7;
    int64_t v8; /* placeholder for void */
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

int64_t String_repeat(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_repeat");
    int64_t v43;
    int64_t v57;
    void* v36;
    int64_t v52;
    int64_t v38; /* placeholder for void */
    int64_t v45;
    bool v7;
    int64_t v24;
    int64_t v54; /* placeholder for void */
    int64_t v50;
    int64_t v27; /* placeholder for void */
    void* v65;
    int64_t v28;
    int64_t v68; /* placeholder for void */
    void* v26;
    int64_t v47;
    int64_t v58;
    int64_t v12; /* placeholder for void */
    int64_t v5;
    int64_t v53; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    void* v11;
    int64_t v30;
    int64_t v41; /* placeholder for void */
    int64_t v35;
    int64_t v44;
    int64_t v46;
    int64_t v67; /* placeholder for void */
    int64_t v55;
    int64_t v48;
    int64_t v9;
    void* v10;
    int64_t v20; /* placeholder for void */
    int64_t v56;
    int64_t v61;
    int64_t v63; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    void* v62;
    int64_t v16; /* placeholder for void */
    int64_t v39;
    void* v32;
    int64_t v13;
    int64_t v22; /* placeholder for void */
    int64_t v25;
    int64_t v21;
    int64_t v18;
    int64_t v31;
    int64_t v69;
    bool v40;
    void* v14;
    bool v6;
    void* v59;
    int64_t v8; /* placeholder for void */
    void* v33;
    int64_t v23; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    bool v4;
    int64_t v51;
    int64_t v29;
    int64_t v60; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v3;
    void* v17;
    int64_t v66;
    int64_t v70; /* placeholder for void */
    int64_t v42;
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

void* Vec_filter(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_filter");
    int64_t v7;
    void* v5;
    int64_t v11;
    bool v17;
    void* v22;
    int64_t v18; /* placeholder for void */
    void* v12;
    int64_t v23; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v34;
    int64_t v35;
    int64_t v16;
    int64_t v32; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    void* v8;
    int64_t v24;
    int64_t v19;
    int64_t v29;
    int64_t v15;
    int64_t v30; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    void* v27;
    int64_t v31; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    bool v25;
    int64_t v10;
    int64_t v36; /* placeholder for void */
    void* v38;
    int64_t v20;
    int64_t v21;
    int64_t v39; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v3;
    int64_t v28;
    void* v4;
    int64_t v37; /* placeholder for void */
    int64_t v33;
    v3 = 0LL;
    v4 = Vec_new((void*)v3);
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = 0LL;
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    v10 = 0LL;
    v11 = (int64_t)Vec_len((void*)v10, v1);
    v12 = malloc(8);
    *(int64_t*)v12 = (int64_t)v11;
    goto bb1;
  bb1:
    v15 = *(int64_t*)v8;
    v16 = *(int64_t*)v12;
    v17 = v15 < v16;
    if (v17) goto bb2;
    else goto bb3;
  bb2:
    v19 = *(int64_t*)v8;
    v20 = 0LL;
    v21 = (int64_t)Vec_get((void*)v20, v1, v19);
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v21;
    v24 = *(int64_t*)v22;
    {
        if (!v2) abort();
        char* _fn_name = *(char**)v2;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "str_is_blank") == 0) {
            v25 = str_is_blank((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "Option_is_some") == 0) {
            v25 = Option_is_some((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "Option_is_none") == 0) {
            v25 = Option_is_none((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "Vec_is_empty") == 0) {
            v25 = Vec_is_empty((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "str_is_numeric") == 0) {
            v25 = str_is_numeric((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "String_is_empty") == 0) {
            v25 = String_is_empty((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "char_is_alpha") == 0) {
            v25 = char_is_alpha((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "str_is_alpha") == 0) {
            v25 = str_is_alpha((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "HashMap_is_empty") == 0) {
            v25 = HashMap_is_empty((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "Result_is_err") == 0) {
            v25 = Result_is_err((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "str_is_alphanumeric") == 0) {
            v25 = str_is_alphanumeric((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "str_is_empty") == 0) {
            v25 = str_is_empty((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "StringBuilder_is_empty") == 0) {
            v25 = StringBuilder_is_empty((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "Result_is_ok") == 0) {
            v25 = Result_is_ok((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "char_is_whitespace") == 0) {
            v25 = char_is_whitespace((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "char_is_upper") == 0) {
            v25 = char_is_upper((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "ByteBuffer_is_empty") == 0) {
            v25 = ByteBuffer_is_empty((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "char_is_digit") == 0) {
            v25 = char_is_digit((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "char_is_lower") == 0) {
            v25 = char_is_lower((void*)v2, (int64_t)v24);
        } else { abort(); }
    }
    if (v25) goto bb4;
    else goto bb5;
  bb3:
    v38 = *(void**)v5;
    p_call_stack_pop();
    return v38;
  bb4:
    v27 = *(void**)v5;
    v28 = *(int64_t*)v22;
    v29 = 0LL;
    Vec_push((void*)v29, v27, v28);
    goto bb6;
  bb5:
    goto bb6;
  bb6:
    v33 = *(int64_t*)v8;
    v34 = 1LL;
    v35 = v33 + v34;
    *(int64_t*)v8 = (int64_t)v35;
    goto bb1;
    return 0;
}

void Annotation_new(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Annotation_new");
    int64_t v16; /* placeholder for void */
    void* v9;
    int64_t v6; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v14;
    int64_t v12; /* placeholder for void */
    void* v5;
    void* v10;
    int64_t v17; /* placeholder for void */
    void* v11;
    void* v7;
    void* v13;
    void* v4;
    void* v15;
    v4 = malloc(16);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = (char*)v4 + 8;
    *(void**)v7 = (void*)v3;
    v9 = (char*)v4 + 0;
    v10 = (char*)v0 + 0;
    v11 = *(void**)v9;
    *(void**)v10 = (void*)v11;
    v13 = (char*)v4 + 8;
    v14 = (char*)v0 + 8;
    v15 = *(void**)v13;
    *(void**)v14 = (void*)v15;
    p_call_stack_pop();
    return;
    return;
}

int64_t clamp(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("clamp");
    bool v4;
    int64_t v11; /* placeholder for void */
    bool v8;
    int64_t v9; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v7; /* placeholder for void */
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

int64_t str_compare(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_compare");
    int64_t v3;
    int64_t v5; /* placeholder for void */
    int64_t v4;
    v3 = 0LL;
    v4 = (int64_t)p_str_compare((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

int64_t Vec_first(void* v0, void* v1) {
    p_call_stack_push("Vec_first");
    int64_t v2;
    int64_t v3; /* placeholder for void */
    v2 = (int64_t)p_vec_first(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void Vec_for_each(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_for_each");
    int64_t v14; /* placeholder for void */
    int64_t v16;
    void* v18;
    int64_t v23;
    int64_t v27; /* placeholder for void */
    int64_t v7;
    int64_t v21; /* placeholder for void */
    int64_t v20;
    int64_t v15;
    int64_t v11;
    int64_t v12;
    int64_t v10; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    void* v4;
    int64_t v5; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v24;
    int64_t v6;
    bool v13;
    int64_t v3;
    int64_t v9; /* placeholder for void */
    int64_t v17;
    void* v8;
    int64_t v19; /* placeholder for void */
    int64_t v22;
    v3 = 0LL;
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = 0LL;
    v7 = (int64_t)Vec_len((void*)v6, v1);
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    goto bb1;
  bb1:
    v11 = *(int64_t*)v4;
    v12 = *(int64_t*)v8;
    v13 = v11 < v12;
    if (v13) goto bb2;
    else goto bb3;
  bb2:
    v15 = *(int64_t*)v4;
    v16 = 0LL;
    v17 = (int64_t)Vec_get((void*)v16, v1, v15);
    v18 = malloc(8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = *(int64_t*)v18;
    {
        if (!v2) abort();
        char* _fn_name = *(char**)v2;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "Vec_reverse") == 0) {
            Vec_reverse((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "StringBuilder_new") == 0) {
            StringBuilder_new((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "StringBuilder_clear") == 0) {
            StringBuilder_clear((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "HashMap_grow") == 0) {
            HashMap_grow((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "ByteBuffer_clear") == 0) {
            ByteBuffer_clear((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Vec_sort") == 0) {
            Vec_sort((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Vec_clear") == 0) {
            Vec_clear((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "vec_dedup") == 0) {
            vec_dedup((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Vec_dedup") == 0) {
            Vec_dedup((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "println") == 0) {
            println((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "assert") == 0) {
            assert((void*)v2, (bool)v20);
        } else if (strcmp(_fn_name, "program_new") == 0) {
            program_new((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "HashMap_clear") == 0) {
            HashMap_clear((void*)v2, (int64_t)v20);
        } else { abort(); }
    }
    v22 = *(int64_t*)v4;
    v23 = 1LL;
    v24 = v22 + v23;
    *(int64_t*)v4 = (int64_t)v24;
    goto bb1;
  bb3:
    p_call_stack_pop();
    return;
    return;
}

int64_t Result_unwrap_or_else(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_unwrap_or_else");
    int64_t v9;
    bool v6;
    int64_t v13;
    int64_t v19; /* placeholder for void */
    int64_t v5;
    void* v8;
    int64_t v21;
    int64_t v12;
    void* v16;
    bool v14;
    int64_t v10; /* placeholder for void */
    void* v11;
    int64_t v15; /* placeholder for void */
    int64_t v18;
    int64_t v7; /* placeholder for void */
    void* v3;
    int64_t v17;
    int64_t v20; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v4;
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
        if (strcmp(_fn_name, "HashMap_new") == 0) {
            v18 = HashMap_new((void*)v2);
        } else if (strcmp(_fn_name, "main") == 0) {
            v18 = main_entry((void*)v2);
        } else { abort(); }
    }
    p_call_stack_pop();
    return v18;
  bb5:
    goto bb1;
    return 0;
}

int64_t count_expr_nodes(void* v0, void* v1) {
    p_call_stack_push("count_expr_nodes");
    int64_t v10;
    void* v46;
    double v19;
    int64_t v50;
    int64_t v11;
    int64_t v79;
    int64_t v82; /* placeholder for void */
    void* v73;
    void* v27;
    int64_t v42;
    void* v2;
    int64_t v61;
    int64_t v6;
    int64_t v69;
    void* v18;
    void* v13;
    int64_t v5;
    void* v40;
    int64_t v41;
    void* v56;
    int64_t v62;
    void* v31;
    bool v71;
    int64_t v63;
    void* v76;
    int64_t v24;
    int64_t v51;
    void* v57;
    bool v37;
    int64_t v14;
    int64_t v8; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    bool v16;
    void* v58;
    int64_t v47;
    int64_t v66;
    void* v54;
    int64_t v70;
    int64_t v39; /* placeholder for void */
    void* v55;
    int64_t v85; /* placeholder for void */
    int64_t v86;
    void* v4;
    int64_t v35; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    bool v25;
    int64_t v30; /* placeholder for void */
    void* v59;
    bool v43;
    int64_t v12; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    void* v74;
    int64_t v26; /* placeholder for void */
    int64_t v65;
    void* v22;
    int64_t v33;
    void* v49;
    int64_t v60;
    void* v36;
    void* v45;
    bool v7;
    int64_t v64;
    void* v3;
    int64_t v72; /* placeholder for void */
    int64_t v38;
    void* v68;
    int64_t v20;
    int64_t v67; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v28;
    int64_t v15;
    int64_t v29;
    bool v34;
    int64_t v48; /* placeholder for void */
    bool v52;
    int64_t v53; /* placeholder for void */
    void* v75;
    void* v9;
    int64_t v77;
    int64_t v78;
    int64_t v80;
    int64_t v87; /* placeholder for void */
    int64_t v23;
    int64_t v32;
    int64_t v83;
    v2 = (char*)v1 + 0;
    v3 = *(void**)v2;
    v4 = (char*)v3 + 0;
    v5 = *(int64_t*)v4;
    v6 = 0LL;
    v7 = v5 == v6;
    if (v7) goto bb2;
    else goto bb3;
  bb1:
    v86 = 0LL;
    p_call_stack_pop();
    return v86;
  bb2:
    v9 = (char*)v3 + 8;
    v10 = *(int64_t*)v9;
    v11 = 1LL;
    p_call_stack_pop();
    return v11;
  bb3:
    v13 = (char*)v3 + 0;
    v14 = *(int64_t*)v13;
    v15 = 1LL;
    v16 = v14 == v15;
    if (v16) goto bb4;
    else goto bb5;
  bb4:
    v18 = (char*)v3 + 8;
    v19 = *(double*)v18;
    v20 = 1LL;
    p_call_stack_pop();
    return v20;
  bb5:
    v22 = (char*)v3 + 0;
    v23 = *(int64_t*)v22;
    v24 = 2LL;
    v25 = v23 == v24;
    if (v25) goto bb6;
    else goto bb7;
  bb6:
    v27 = (char*)v3 + 8;
    v28 = *(void**)v27;
    v29 = 1LL;
    p_call_stack_pop();
    return v29;
  bb7:
    v31 = (char*)v3 + 0;
    v32 = *(int64_t*)v31;
    v33 = 3LL;
    v34 = v32 == v33;
    if (v34) goto bb8;
    else goto bb9;
  bb8:
    v36 = (char*)v3 + 8;
    v37 = *(bool*)v36;
    v38 = 1LL;
    p_call_stack_pop();
    return v38;
  bb9:
    v40 = (char*)v3 + 0;
    v41 = *(int64_t*)v40;
    v42 = 4LL;
    v43 = v41 == v42;
    if (v43) goto bb10;
    else goto bb11;
  bb10:
    v45 = (char*)v3 + 8;
    v46 = *(void**)v45;
    v47 = 1LL;
    p_call_stack_pop();
    return v47;
  bb11:
    v49 = (char*)v3 + 0;
    v50 = *(int64_t*)v49;
    v51 = 5LL;
    v52 = v50 == v51;
    if (v52) goto bb12;
    else goto bb13;
  bb12:
    v54 = (char*)v3 + 8;
    v55 = *(void**)v54;
    v56 = (char*)v3 + 16;
    v57 = *(void**)v56;
    v58 = (char*)v3 + 24;
    v59 = *(void**)v58;
    v60 = 0LL;
    v61 = (int64_t)count_expr_nodes((void*)v60, v57);
    v62 = 0LL;
    v63 = (int64_t)count_expr_nodes((void*)v62, v59);
    v64 = v61 + v63;
    v65 = 1LL;
    v66 = v64 + v65;
    p_call_stack_pop();
    return v66;
  bb13:
    v68 = (char*)v3 + 0;
    v69 = *(int64_t*)v68;
    v70 = 6LL;
    v71 = v69 == v70;
    if (v71) goto bb14;
    else goto bb15;
  bb14:
    v73 = (char*)v3 + 8;
    v74 = *(void**)v73;
    v75 = (char*)v3 + 16;
    v76 = *(void**)v75;
    v77 = 0LL;
    v78 = (int64_t)count_expr_nodes((void*)v77, v76);
    v79 = 1LL;
    v80 = v78 + v79;
    p_call_stack_pop();
    return v80;
  bb15:
    goto bb16;
  bb16:
    v83 = 1LL;
    p_call_stack_pop();
    return v83;
  bb17:
    goto bb1;
    return 0;
}

void* Vec_to_str(void* v0, int64_t v1) {
    p_call_stack_push("Vec_to_str");
    void* v24;
    int64_t v28;
    bool v29;
    void* v61;
    void* v6;
    int64_t v3;
    int64_t v34; /* placeholder for void */
    void* v49;
    void* v21;
    int64_t v30; /* placeholder for void */
    int64_t v17;
    int64_t v19; /* placeholder for void */
    void* v46;
    int64_t v11;
    int64_t v74; /* placeholder for void */
    void* v10;
    int64_t v72; /* placeholder for void */
    void* v37;
    int64_t v18; /* placeholder for void */
    void* v20;
    int64_t v9; /* placeholder for void */
    void* v14;
    int64_t v50; /* placeholder for void */
    int64_t v27;
    void* v70;
    void* v73;
    int64_t v39; /* placeholder for void */
    int64_t v44;
    int64_t v7; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v32;
    int64_t v23;
    int64_t v66;
    int64_t v54; /* placeholder for void */
    int64_t v31;
    void* v52;
    int64_t v43;
    int64_t v41;
    int64_t v58; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v64;
    void* v69;
    void* v62;
    int64_t v47; /* placeholder for void */
    void* v4;
    int64_t v42;
    int64_t v22; /* placeholder for void */
    bool v33;
    int64_t v57; /* placeholder for void */
    void* v59;
    int64_t v60;
    void* v71;
    int64_t v56;
    int64_t v45;
    int64_t v67; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    int64_t v51;
    int64_t v2;
    int64_t v40; /* placeholder for void */
    int64_t v48;
    int64_t v65;
    int64_t v12; /* placeholder for void */
    void* v8;
    void* v36;
    int64_t v26; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    void* v35;
    void* v16;
    void* v55;
    int64_t v38; /* placeholder for void */
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
    v20 = _str395;
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
    v70 = _str396;
    v71 = p_str_concat(v69, v70);
    *(void**)v21 = (void*)v71;
    v73 = *(void**)v21;
    p_call_stack_pop();
    return v73;
  bb4:
    v35 = *(void**)v21;
    v36 = _str13;
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

int64_t String_replace(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("String_replace");
    void* v15;
    bool v5;
    void* v8;
    int64_t v118; /* placeholder for void */
    int64_t v190;
    int64_t v126; /* placeholder for void */
    int64_t v102;
    int64_t v129;
    int64_t v13; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    int64_t v53;
    bool v85;
    int64_t v114; /* placeholder for void */
    int64_t v79;
    int64_t v146; /* placeholder for void */
    int64_t v172;
    int64_t v186;
    int64_t v136;
    void* v52;
    int64_t v111; /* placeholder for void */
    int64_t v91;
    int64_t v17; /* placeholder for void */
    int64_t v138;
    int64_t v122; /* placeholder for void */
    int64_t v110; /* placeholder for void */
    int64_t v29;
    int64_t v81;
    int64_t v188;
    int64_t v92;
    int64_t v170;
    int64_t v149; /* placeholder for void */
    int64_t v44;
    int64_t v55; /* placeholder for void */
    int64_t v185;
    int64_t v76;
    int64_t v60;
    void* v112;
    int64_t v158;
    int64_t v78; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v59;
    int64_t v86; /* placeholder for void */
    int64_t v142; /* placeholder for void */
    uint8_t v113;
    int64_t v28;
    int64_t v153; /* placeholder for void */
    int64_t v181; /* placeholder for void */
    int64_t v24;
    void* v97;
    void* v45;
    int64_t v39; /* placeholder for void */
    void* v100;
    int64_t v43; /* placeholder for void */
    int64_t v164; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v135;
    int64_t v177; /* placeholder for void */
    void* v151;
    int64_t v62; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    uint8_t v155;
    void* v163;
    int64_t v10; /* placeholder for void */
    void* v166;
    int64_t v11;
    int64_t v64;
    int64_t v75;
    int64_t v107; /* placeholder for void */
    void* v173;
    int64_t v169; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    int64_t v178; /* placeholder for void */
    int64_t v195; /* placeholder for void */
    int64_t v161; /* placeholder for void */
    void* v159;
    int64_t v96;
    int64_t v63; /* placeholder for void */
    int64_t v119;
    int64_t v108;
    int64_t v89;
    int64_t v137; /* placeholder for void */
    int64_t v139;
    int64_t v168; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    void* v57;
    int64_t v167;
    int64_t v175;
    bool v132;
    int64_t v180;
    int64_t v187; /* placeholder for void */
    int64_t v56;
    int64_t v65;
    int64_t v16;
    int64_t v130; /* placeholder for void */
    int64_t v184;
    int64_t v191;
    void* v160;
    int64_t v193; /* placeholder for void */
    bool v23;
    int64_t v20; /* placeholder for void */
    int64_t v7;
    void* v106;
    void* v176;
    int64_t v18; /* placeholder for void */
    int64_t v90;
    int64_t v74; /* placeholder for void */
    void* v115;
    void* v148;
    int64_t v80;
    int64_t v37;
    int64_t v150;
    int64_t v144;
    int64_t v134;
    int64_t v171;
    int64_t v27; /* placeholder for void */
    void* v61;
    int64_t v131; /* placeholder for void */
    int64_t v174; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v58; /* placeholder for void */
    int64_t v147;
    int64_t v165; /* placeholder for void */
    bool v77;
    void* v69;
    int64_t v4;
    int64_t v54; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    int64_t v125; /* placeholder for void */
    int64_t v143;
    int64_t v194; /* placeholder for void */
    int64_t v95;
    int64_t v189; /* placeholder for void */
    int64_t v182; /* placeholder for void */
    void* v109;
    int64_t v127;
    int64_t v152; /* placeholder for void */
    int64_t v196;
    uint8_t v116;
    void* v34;
    bool v66;
    bool v68;
    int64_t v133; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v99;
    int64_t v124; /* placeholder for void */
    bool v83;
    int64_t v88; /* placeholder for void */
    void* v93;
    int64_t v98; /* placeholder for void */
    void* v154;
    int64_t v71;
    void* v103;
    int64_t v183;
    int64_t v19;
    void* v72;
    int64_t v73; /* placeholder for void */
    bool v25;
    bool v38;
    void* v46;
    int64_t v120;
    int64_t v140;
    int64_t v141; /* placeholder for void */
    bool v26;
    int64_t v14; /* placeholder for void */
    void* v49;
    int64_t v67; /* placeholder for void */
    int64_t v105;
    int64_t v36;
    int64_t v22;
    int64_t v117; /* placeholder for void */
    int64_t v128;
    int64_t v33;
    int64_t v40;
    bool v121;
    void* v145;
    void* v179;
    int64_t v156; /* placeholder for void */
    int64_t v192;
    int64_t v32;
    int64_t v197; /* placeholder for void */
    int64_t v162;
    void* v12;
    int64_t v82;
    bool v123;
    int64_t v157; /* placeholder for void */
    int64_t v48;
    int64_t v21; /* placeholder for void */
    int64_t v41;
    void* v9;
    int64_t v84; /* placeholder for void */
    int64_t v87; /* placeholder for void */
    int64_t v42; /* placeholder for void */
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

void* str_capitalize(void* v0, void* v1) {
    p_call_stack_push("str_capitalize");
    int64_t v8; /* placeholder for void */
    int64_t v11;
    int64_t v13; /* placeholder for void */
    int64_t v17;
    int64_t v27;
    int64_t v10;
    void* v23;
    int64_t v42;
    int64_t v43;
    int64_t v44;
    int64_t v30;
    void* v54;
    int64_t v50; /* placeholder for void */
    int64_t v29;
    int64_t v47;
    int64_t v37;
    int64_t v38;
    int64_t v9;
    bool v19;
    int64_t v48;
    int64_t v49;
    int64_t v22;
    int64_t v31; /* placeholder for void */
    void* v52;
    int64_t v7; /* placeholder for void */
    bool v16;
    int64_t v45;
    int64_t v21; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v53;
    int64_t v57; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    void* v26;
    void* v41;
    int64_t v2;
    int64_t v36;
    int64_t v28;
    int64_t v32;
    int64_t v55; /* placeholder for void */
    bool v20;
    int64_t v56; /* placeholder for void */
    void* v12;
    int64_t v40; /* placeholder for void */
    void* v33;
    int64_t v15;
    int64_t v46; /* placeholder for void */
    int64_t v18;
    int64_t v34; /* placeholder for void */
    int64_t v14;
    int64_t v35; /* placeholder for void */
    bool v39;
    bool v5;
    int64_t v3;
    void* v24;
    int64_t v4;
    int64_t v6; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)str_len((void*)v2, v1);
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return v1;
  bb2:
    goto bb3;
  bb3:
    v9 = 0LL;
    v10 = 0LL;
    v11 = (int64_t)str_byte_at((void*)v10, v1, v9);
    v12 = malloc(8);
    *(int64_t*)v12 = (int64_t)v11;
    v14 = *(int64_t*)v12;
    v15 = 97LL;
    v16 = v14 >= v15;
    v17 = *(int64_t*)v12;
    v18 = 122LL;
    v19 = v17 <= v18;
    v20 = v16 && v19;
    if (v20) goto bb4;
    else goto bb5;
  bb4:
    v22 = 0LL;
    v23 = Vec_new((void*)v22);
    v24 = malloc(8);
    *(void**)v24 = (void*)v23;
    v26 = *(void**)v24;
    v27 = *(int64_t*)v12;
    v28 = 32LL;
    v29 = v27 - v28;
    v30 = 0LL;
    Vec_push((void*)v30, v26, v29);
    v32 = 1LL;
    v33 = malloc(8);
    *(int64_t*)v33 = (int64_t)v32;
    goto bb7;
  bb5:
    goto bb6;
  bb6:
    p_call_stack_pop();
    return v1;
  bb7:
    v36 = *(int64_t*)v33;
    v37 = 0LL;
    v38 = (int64_t)str_len((void*)v37, v1);
    v39 = v36 < v38;
    if (v39) goto bb8;
    else goto bb9;
  bb8:
    v41 = *(void**)v24;
    v42 = *(int64_t*)v33;
    v43 = 0LL;
    v44 = (int64_t)str_byte_at((void*)v43, v1, v42);
    v45 = 0LL;
    Vec_push((void*)v45, v41, v44);
    v47 = *(int64_t*)v33;
    v48 = 1LL;
    v49 = v47 + v48;
    *(int64_t*)v33 = (int64_t)v49;
    goto bb7;
  bb9:
    v52 = *(void**)v24;
    v53 = 0LL;
    v54 = str_from_bytes((void*)v53, v52);
    p_call_stack_pop();
    return v54;
    return 0;
}

bool String_starts_with(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_starts_with");
    int64_t v17;
    int64_t v13;
    void* v37;
    int64_t v48; /* placeholder for void */
    void* v41;
    int64_t v3;
    int64_t v45; /* placeholder for void */
    uint8_t v57;
    int64_t v62; /* placeholder for void */
    int64_t v43;
    int64_t v58; /* placeholder for void */
    int64_t v16;
    void* v28;
    void* v53;
    int64_t v54; /* placeholder for void */
    uint8_t v60;
    bool v22;
    int64_t v5;
    int64_t v76; /* placeholder for void */
    int64_t v35;
    int64_t v39;
    int64_t v42; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    int64_t v70;
    int64_t v34; /* placeholder for void */
    bool v6;
    int64_t v51; /* placeholder for void */
    void* v59;
    int64_t v52;
    int64_t v36;
    int64_t v21;
    bool v33;
    bool v9;
    int64_t v32;
    int64_t v11; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    void* v47;
    int64_t v31;
    void* v50;
    int64_t v20;
    int64_t v71;
    int64_t v15; /* placeholder for void */
    int64_t v64;
    int64_t v40;
    int64_t v23; /* placeholder for void */
    int64_t v72;
    bool v75;
    int64_t v10; /* placeholder for void */
    void* v56;
    int64_t v49;
    int64_t v55; /* placeholder for void */
    int64_t v61; /* placeholder for void */
    bool v67;
    int64_t v38; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    void* v14;
    int64_t v25; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v12;
    void* v18;
    bool v24;
    int64_t v30; /* placeholder for void */
    int64_t v27;
    int64_t v46;
    int64_t v68; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    bool v65;
    int64_t v73; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    void* v44;
    bool v7;
    int64_t v63;
    bool v4;
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

int64_t str_parse_int_range(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("str_parse_int_range");
    int64_t v79;
    int64_t v19; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    bool v22;
    int64_t v73;
    int64_t v47;
    int64_t v17; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    void* v39;
    int64_t v65;
    int64_t v35;
    int64_t v28;
    void* v57;
    int64_t v69; /* placeholder for void */
    void* v8;
    int64_t v27; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v24;
    int64_t v59; /* placeholder for void */
    int64_t v32;
    bool v66;
    bool v67;
    int64_t v72;
    int64_t v16;
    int64_t v77;
    int64_t v26; /* placeholder for void */
    int64_t v50;
    int64_t v71;
    int64_t v33; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v5;
    int64_t v38;
    int64_t v83; /* placeholder for void */
    int64_t v85; /* placeholder for void */
    int64_t v46;
    int64_t v56; /* placeholder for void */
    bool v44;
    int64_t v75;
    int64_t v15; /* placeholder for void */
    int64_t v53;
    int64_t v25; /* placeholder for void */
    uint8_t v58;
    int64_t v76;
    int64_t v84;
    int64_t v62;
    void* v48;
    int64_t v13;
    int64_t v74;
    void* v54;
    int64_t v60; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    void* v51;
    int64_t v70; /* placeholder for void */
    int64_t v21;
    int64_t v7; /* placeholder for void */
    int64_t v43;
    int64_t v81;
    int64_t v12;
    int64_t v41; /* placeholder for void */
    bool v63;
    int64_t v82; /* placeholder for void */
    void* v6;
    bool v30;
    int64_t v61;
    int64_t v11; /* placeholder for void */
    int64_t v29;
    int64_t v64;
    int64_t v20;
    int64_t v23; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v4;
    void* v10;
    int64_t v45; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v80;
    int64_t v68; /* placeholder for void */
    int64_t v42;
    bool v14;
    void* v36;
    int64_t v9; /* placeholder for void */
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

int64_t String_to_lowercase(void* v0, int64_t v1) {
    p_call_stack_push("String_to_lowercase");
    int64_t v34;
    int64_t v80;
    void* v88;
    int64_t v24;
    int64_t v47;
    int64_t v90;
    void* v40;
    int64_t v95;
    int64_t v96; /* placeholder for void */
    int64_t v98;
    int64_t v2;
    int64_t v21;
    int64_t v62;
    int64_t v20;
    int64_t v78; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v81;
    int64_t v72; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    int64_t v38;
    int64_t v46; /* placeholder for void */
    void* v51;
    int64_t v11; /* placeholder for void */
    void* v54;
    int64_t v59; /* placeholder for void */
    bool v63;
    int64_t v64;
    int64_t v93; /* placeholder for void */
    int64_t v85;
    int64_t v71;
    int64_t v19; /* placeholder for void */
    void* v31;
    int64_t v15; /* placeholder for void */
    int64_t v65;
    bool v67;
    int64_t v84; /* placeholder for void */
    int64_t v39;
    int64_t v68; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    bool v66;
    int64_t v25;
    int64_t v73; /* placeholder for void */
    void* v13;
    void* v7;
    int64_t v70;
    bool v3;
    int64_t v74; /* placeholder for void */
    int64_t v89; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    bool v36;
    int64_t v49; /* placeholder for void */
    int64_t v99; /* placeholder for void */
    int64_t v9;
    void* v48;
    int64_t v26;
    int64_t v14;
    void* v45;
    int64_t v41; /* placeholder for void */
    int64_t v44;
    int64_t v79; /* placeholder for void */
    void* v91;
    int64_t v8; /* placeholder for void */
    int64_t v5;
    void* v22;
    int64_t v33; /* placeholder for void */
    int64_t v17;
    void* v28;
    void* v27;
    int64_t v53;
    void* v57;
    int64_t v83; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v86;
    int64_t v50;
    int64_t v43;
    int64_t v30;
    void* v10;
    int64_t v42;
    int64_t v75; /* placeholder for void */
    int64_t v77;
    int64_t v23; /* placeholder for void */
    void* v76;
    int64_t v32; /* placeholder for void */
    void* v6;
    uint8_t v58;
    int64_t v61;
    int64_t v82;
    int64_t v87;
    int64_t v69;
    int64_t v92; /* placeholder for void */
    void* v94;
    int64_t v12; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v35;
    int64_t v29; /* placeholder for void */
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

void ByteBuffer_set_byte(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("ByteBuffer_set_byte");
    bool v5;
    int64_t v7;
    int64_t v10; /* placeholder for void */
    int64_t v15;
    void* v14;
    void* v13;
    int64_t v12; /* placeholder for void */
    bool v8;
    int64_t v4;
    int64_t v11; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v6;
    bool v9;
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
    Vec_set((void*)v15, v14, v2, v3);
    p_call_stack_pop();
    return;
    return;
}

void TypeAlias_new(void* v0, void* v1, void* v2, void* v3, bool v4) {
    p_call_stack_push("TypeAlias_new");
    int64_t v48; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    void* v9;
    void* v17;
    int64_t v47;
    int64_t v52; /* placeholder for void */
    void* v32;
    int64_t v24; /* placeholder for void */
    void* v5;
    void* v14;
    int64_t v15;
    void* v29;
    void* v53;
    int64_t v20; /* placeholder for void */
    void* v54;
    void* v45;
    int64_t v23;
    void* v50;
    void* v31;
    void* v37;
    int64_t v61; /* placeholder for void */
    void* v58;
    int64_t v19;
    int64_t v30; /* placeholder for void */
    void* v26;
    int64_t v51;
    int64_t v28; /* placeholder for void */
    void* v25;
    void* v22;
    void* v33;
    int64_t v11;
    bool v59;
    int64_t v7; /* placeholder for void */
    int64_t v27;
    void* v42;
    void* v38;
    void* v46;
    int64_t v44; /* placeholder for void */
    int64_t v55;
    int64_t v39;
    void* v41;
    void* v49;
    void* v57;
    int64_t v40; /* placeholder for void */
    void* v10;
    void* v8;
    int64_t v43;
    void* v6;
    int64_t v16; /* placeholder for void */
    void* v36;
    void* v13;
    void* v21;
    void* v18;
    int64_t v34; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    void* v35;
    int64_t v60; /* placeholder for void */
    v5 = malloc(56);
    v6 = (char*)v5 + 0;
    *(void**)v6 = (void*)v2;
    v8 = (char*)v5 + 8;
    v9 = (char*)v3 + 0;
    v10 = (char*)v8 + 0;
    v11 = *(int64_t*)v9;
    *(int64_t*)v10 = (int64_t)v11;
    v13 = (char*)v3 + 8;
    v14 = (char*)v8 + 8;
    v15 = *(int64_t*)v13;
    *(int64_t*)v14 = (int64_t)v15;
    v17 = (char*)v3 + 16;
    v18 = (char*)v8 + 16;
    v19 = *(int64_t*)v17;
    *(int64_t*)v18 = (int64_t)v19;
    v21 = (char*)v3 + 24;
    v22 = (char*)v8 + 24;
    v23 = *(int64_t*)v21;
    *(int64_t*)v22 = (int64_t)v23;
    v25 = (char*)v3 + 32;
    v26 = (char*)v8 + 32;
    v27 = *(int64_t*)v25;
    *(int64_t*)v26 = (int64_t)v27;
    v29 = (char*)v5 + 48;
    *(bool*)v29 = v4;
    v31 = (char*)v5 + 0;
    v32 = (char*)v0 + 0;
    v33 = *(void**)v31;
    *(void**)v32 = (void*)v33;
    v35 = (char*)v5 + 8;
    v36 = (char*)v0 + 8;
    v37 = (char*)v35 + 0;
    v38 = (char*)v36 + 0;
    v39 = *(int64_t*)v37;
    *(int64_t*)v38 = (int64_t)v39;
    v41 = (char*)v35 + 8;
    v42 = (char*)v36 + 8;
    v43 = *(int64_t*)v41;
    *(int64_t*)v42 = (int64_t)v43;
    v45 = (char*)v35 + 16;
    v46 = (char*)v36 + 16;
    v47 = *(int64_t*)v45;
    *(int64_t*)v46 = (int64_t)v47;
    v49 = (char*)v35 + 24;
    v50 = (char*)v36 + 24;
    v51 = *(int64_t*)v49;
    *(int64_t*)v50 = (int64_t)v51;
    v53 = (char*)v35 + 32;
    v54 = (char*)v36 + 32;
    v55 = *(int64_t*)v53;
    *(int64_t*)v54 = (int64_t)v55;
    v57 = (char*)v5 + 48;
    v58 = (char*)v0 + 48;
    v59 = *(bool*)v57;
    *(bool*)v58 = v59;
    p_call_stack_pop();
    return;
    return;
}

bool StringBuilder_is_empty(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_is_empty");
    bool v5;
    void* v2;
    int64_t v3;
    int64_t v4;
    int64_t v6; /* placeholder for void */
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    p_call_stack_pop();
    return v5;
    return 0;
}

void Vec_push(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_push");
    int64_t v3; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    p_vec_push(v1, (void*)v2);
    p_call_stack_pop();
    return;
    return;
}

void* str_join_ints(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_join_ints");
    int64_t v41;
    int64_t v44; /* placeholder for void */
    int64_t v11;
    void* v16;
    void* v19;
    int64_t v17; /* placeholder for void */
    int64_t v32;
    void* v29;
    int64_t v13;
    int64_t v34;
    void* v36;
    int64_t v37;
    int64_t v4;
    int64_t v40;
    bool v6;
    int64_t v9; /* placeholder for void */
    int64_t v35;
    void* v27;
    int64_t v46; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    void* v38;
    int64_t v28;
    int64_t v3;
    void* v45;
    int64_t v23;
    int64_t v24;
    int64_t v7; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v12;
    int64_t v18;
    int64_t v5;
    void* v15;
    int64_t v21; /* placeholder for void */
    int64_t v22;
    bool v25;
    int64_t v14;
    int64_t v20; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    void* v31;
    int64_t v26; /* placeholder for void */
    void* v8;
    int64_t v33;
    int64_t v42;
    int64_t v43; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)Vec_len((void*)v3, v1);
    v5 = 0LL;
    v6 = v4 == v5;
    if (v6) goto bb1;
    else goto bb2;
  bb1:
    v8 = _str5;
    p_call_stack_pop();
    return v8;
  bb2:
    goto bb3;
  bb3:
    v11 = 0LL;
    v12 = 0LL;
    v13 = (int64_t)Vec_get((void*)v12, v1, v11);
    v14 = 0LL;
    v15 = int_to_str((void*)v14, v13);
    v16 = malloc(8);
    *(void**)v16 = (void*)v15;
    v18 = 1LL;
    v19 = malloc(8);
    *(int64_t*)v19 = (int64_t)v18;
    goto bb4;
  bb4:
    v22 = *(int64_t*)v19;
    v23 = 0LL;
    v24 = (int64_t)Vec_len((void*)v23, v1);
    v25 = v22 < v24;
    if (v25) goto bb5;
    else goto bb6;
  bb5:
    v27 = *(void**)v16;
    v28 = 0LL;
    v29 = str_concat((void*)v28, v27, v2);
    *(void**)v16 = (void*)v29;
    v31 = *(void**)v16;
    v32 = *(int64_t*)v19;
    v33 = 0LL;
    v34 = (int64_t)Vec_get((void*)v33, v1, v32);
    v35 = 0LL;
    v36 = int_to_str((void*)v35, v34);
    v37 = 0LL;
    v38 = str_concat((void*)v37, v31, v36);
    *(void**)v16 = (void*)v38;
    v40 = *(int64_t*)v19;
    v41 = 1LL;
    v42 = v40 + v41;
    *(int64_t*)v19 = (int64_t)v42;
    goto bb4;
  bb6:
    v45 = *(void**)v16;
    p_call_stack_pop();
    return v45;
    return 0;
}

int64_t HashMap_clone(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_clone");
    int64_t v13;
    int64_t v49; /* placeholder for void */
    bool v41;
    int64_t v44;
    void* v90;
    int64_t v99; /* placeholder for void */
    void* v63;
    bool v80;
    int64_t v71; /* placeholder for void */
    int64_t v36;
    int64_t v83;
    void* v93;
    int64_t v81; /* placeholder for void */
    int64_t v74;
    int64_t v58;
    void* v69;
    int64_t v9; /* placeholder for void */
    int64_t v73;
    int64_t v37;
    bool v75;
    int64_t v35; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v39;
    int64_t v102;
    bool v38;
    int64_t v16;
    int64_t v85;
    int64_t v89;
    int64_t v64; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    void* v20;
    int64_t v51;
    int64_t v31; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    int64_t v110; /* placeholder for void */
    bool v53;
    int64_t v3;
    bool v79;
    int64_t v105; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    int64_t v100;
    int64_t v113; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v103;
    int64_t v94; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    void* v4;
    int64_t v57;
    int64_t v84;
    int64_t v55;
    void* v23;
    int64_t v91; /* placeholder for void */
    int64_t v97;
    int64_t v111; /* placeholder for void */
    int64_t v88; /* placeholder for void */
    void* v48;
    int64_t v21; /* placeholder for void */
    int64_t v101;
    int64_t v86;
    void* v29;
    int64_t v67; /* placeholder for void */
    int64_t v112;
    int64_t v5; /* placeholder for void */
    void* v10;
    int64_t v76;
    int64_t v11; /* placeholder for void */
    int64_t v56;
    int64_t v24; /* placeholder for void */
    int64_t v77;
    int64_t v108;
    int64_t v107;
    int64_t v12;
    void* v32;
    void* v17;
    int64_t v72; /* placeholder for void */
    int64_t v59;
    void* v66;
    void* v8;
    int64_t v27;
    bool v42;
    int64_t v28; /* placeholder for void */
    int64_t v7;
    int64_t v6;
    int64_t v33;
    int64_t v70;
    int64_t v18; /* placeholder for void */
    int64_t v62;
    int64_t v109;
    int64_t v50; /* placeholder for void */
    int64_t v22;
    int64_t v2;
    int64_t v65;
    void* v26;
    int64_t v43; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    void* v60;
    int64_t v34; /* placeholder for void */
    int64_t v82;
    int64_t v78;
    int64_t v47;
    int64_t v30;
    int64_t v52;
    int64_t v19;
    int64_t v61; /* placeholder for void */
    void* v96;
    void* v87;
    void* v14;
    int64_t v92;
    int64_t v40;
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

void ExternDecl_new(void* v0, void* v1, void* v2, void* v3, void* v4, void* v5) {
    p_call_stack_push("ExternDecl_new");
    int64_t v117; /* placeholder for void */
    void* v27;
    void* v35;
    int64_t v62;
    void* v22;
    void* v38;
    int64_t v100;
    void* v56;
    int64_t v122; /* placeholder for void */
    void* v80;
    int64_t v79; /* placeholder for void */
    void* v9;
    void* v31;
    void* v103;
    int64_t v97; /* placeholder for void */
    void* v112;
    void* v40;
    void* v26;
    int64_t v49;
    int64_t v37; /* placeholder for void */
    void* v70;
    int64_t v36;
    void* v19;
    int64_t v25; /* placeholder for void */
    void* v119;
    void* v102;
    int64_t v85; /* placeholder for void */
    void* v51;
    void* v82;
    int64_t v10; /* placeholder for void */
    int64_t v45;
    void* v53;
    void* v60;
    int64_t v12; /* placeholder for void */
    void* v17;
    void* v34;
    void* v99;
    void* v76;
    void* v74;
    void* v115;
    void* v48;
    int64_t v58;
    int64_t v39;
    int64_t v75; /* placeholder for void */
    void* v95;
    void* v6;
    void* v68;
    void* v114;
    int64_t v41; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    void* v44;
    int64_t v28;
    void* v57;
    void* v87;
    void* v118;
    void* v66;
    void* v83;
    int64_t v21; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v64;
    void* v109;
    int64_t v121; /* placeholder for void */
    void* v42;
    void* v61;
    int64_t v111; /* placeholder for void */
    int64_t v20;
    int64_t v92;
    int64_t v52;
    int64_t v13;
    void* v14;
    int64_t v24;
    int64_t v110;
    void* v98;
    void* v78;
    int64_t v96;
    void* v94;
    void* v47;
    int64_t v46; /* placeholder for void */
    void* v7;
    void* v81;
    int64_t v107; /* placeholder for void */
    void* v65;
    void* v15;
    void* v23;
    int64_t v106;
    void* v55;
    int64_t v67; /* placeholder for void */
    int64_t v84;
    int64_t v93; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    void* v108;
    void* v18;
    int64_t v54; /* placeholder for void */
    void* v73;
    int64_t v89; /* placeholder for void */
    void* v43;
    void* v30;
    int64_t v120;
    int64_t v71; /* placeholder for void */
    int64_t v88;
    void* v72;
    void* v104;
    int64_t v29; /* placeholder for void */
    void* v91;
    int64_t v101; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v116;
    void* v90;
    void* v86;
    void* v69;
    void* v113;
    int64_t v63; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    void* v11;
    int64_t v32;
    void* v77;
    void* v105;
    v6 = malloc(104);
    v7 = (char*)v6 + 0;
    *(void**)v7 = (void*)v2;
    v9 = (char*)v6 + 8;
    *(void**)v9 = (void*)v3;
    v11 = (char*)v6 + 16;
    *(void**)v11 = (void*)v4;
    v13 = 0LL;
    v14 = Vec_new((void*)v13);
    v15 = (char*)v6 + 24;
    *(void**)v15 = (void*)v14;
    v17 = (char*)v6 + 32;
    v18 = (char*)v5 + 0;
    v19 = (char*)v17 + 0;
    v20 = *(int64_t*)v18;
    *(int64_t*)v19 = (int64_t)v20;
    v22 = (char*)v5 + 8;
    v23 = (char*)v17 + 8;
    v24 = *(int64_t*)v22;
    *(int64_t*)v23 = (int64_t)v24;
    v26 = (char*)v5 + 16;
    v27 = (char*)v17 + 16;
    v28 = *(int64_t*)v26;
    *(int64_t*)v27 = (int64_t)v28;
    v30 = (char*)v5 + 24;
    v31 = (char*)v17 + 24;
    v32 = *(int64_t*)v30;
    *(int64_t*)v31 = (int64_t)v32;
    v34 = (char*)v5 + 32;
    v35 = (char*)v17 + 32;
    v36 = *(int64_t*)v34;
    *(int64_t*)v35 = (int64_t)v36;
    v38 = malloc(16);
    v39 = 1LL;
    v40 = (char*)v38 + 0;
    *(int64_t*)v40 = (int64_t)v39;
    v42 = (char*)v6 + 72;
    v43 = (char*)v38 + 0;
    v44 = (char*)v42 + 0;
    v45 = *(int64_t*)v43;
    *(int64_t*)v44 = (int64_t)v45;
    v47 = (char*)v38 + 8;
    v48 = (char*)v42 + 8;
    v49 = *(int64_t*)v47;
    *(int64_t*)v48 = (int64_t)v49;
    v51 = malloc(16);
    v52 = 1LL;
    v53 = (char*)v51 + 0;
    *(int64_t*)v53 = (int64_t)v52;
    v55 = (char*)v6 + 88;
    v56 = (char*)v51 + 0;
    v57 = (char*)v55 + 0;
    v58 = *(int64_t*)v56;
    *(int64_t*)v57 = (int64_t)v58;
    v60 = (char*)v51 + 8;
    v61 = (char*)v55 + 8;
    v62 = *(int64_t*)v60;
    *(int64_t*)v61 = (int64_t)v62;
    v64 = (char*)v6 + 0;
    v65 = (char*)v0 + 0;
    v66 = *(void**)v64;
    *(void**)v65 = (void*)v66;
    v68 = (char*)v6 + 8;
    v69 = (char*)v0 + 8;
    v70 = *(void**)v68;
    *(void**)v69 = (void*)v70;
    v72 = (char*)v6 + 16;
    v73 = (char*)v0 + 16;
    v74 = *(void**)v72;
    *(void**)v73 = (void*)v74;
    v76 = (char*)v6 + 24;
    v77 = (char*)v0 + 24;
    v78 = *(void**)v76;
    *(void**)v77 = (void*)v78;
    v80 = (char*)v6 + 32;
    v81 = (char*)v0 + 32;
    v82 = (char*)v80 + 0;
    v83 = (char*)v81 + 0;
    v84 = *(int64_t*)v82;
    *(int64_t*)v83 = (int64_t)v84;
    v86 = (char*)v80 + 8;
    v87 = (char*)v81 + 8;
    v88 = *(int64_t*)v86;
    *(int64_t*)v87 = (int64_t)v88;
    v90 = (char*)v80 + 16;
    v91 = (char*)v81 + 16;
    v92 = *(int64_t*)v90;
    *(int64_t*)v91 = (int64_t)v92;
    v94 = (char*)v80 + 24;
    v95 = (char*)v81 + 24;
    v96 = *(int64_t*)v94;
    *(int64_t*)v95 = (int64_t)v96;
    v98 = (char*)v80 + 32;
    v99 = (char*)v81 + 32;
    v100 = *(int64_t*)v98;
    *(int64_t*)v99 = (int64_t)v100;
    v102 = (char*)v6 + 72;
    v103 = (char*)v0 + 72;
    v104 = (char*)v102 + 0;
    v105 = (char*)v103 + 0;
    v106 = *(int64_t*)v104;
    *(int64_t*)v105 = (int64_t)v106;
    v108 = (char*)v102 + 8;
    v109 = (char*)v103 + 8;
    v110 = *(int64_t*)v108;
    *(int64_t*)v109 = (int64_t)v110;
    v112 = (char*)v6 + 88;
    v113 = (char*)v0 + 88;
    v114 = (char*)v112 + 0;
    v115 = (char*)v113 + 0;
    v116 = *(int64_t*)v114;
    *(int64_t*)v115 = (int64_t)v116;
    v118 = (char*)v112 + 8;
    v119 = (char*)v113 + 8;
    v120 = *(int64_t*)v118;
    *(int64_t*)v119 = (int64_t)v120;
    p_call_stack_pop();
    return;
    return;
}

void Result_map_err(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Result_map_err");
    int64_t v28;
    int64_t v34;
    int64_t v32;
    void* v35;
    int64_t v37;
    int64_t v42;
    int64_t v16; /* placeholder for void */
    void* v21;
    int64_t v5;
    void* v18;
    int64_t v12;
    int64_t v25; /* placeholder for void */
    bool v29;
    int64_t v30; /* placeholder for void */
    void* v31;
    int64_t v24; /* placeholder for void */
    bool v7;
    int64_t v36; /* placeholder for void */
    void* v15;
    void* v26;
    int64_t v39; /* placeholder for void */
    void* v40;
    void* v41;
    void* v44;
    int64_t v43; /* placeholder for void */
    void* v45;
    void* v17;
    int64_t v10;
    int64_t v14; /* placeholder for void */
    void* v13;
    void* v33;
    void* v38;
    int64_t v48; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v46;
    int64_t v23;
    void* v9;
    int64_t v19;
    void* v22;
    int64_t v50; /* placeholder for void */
    int64_t v6;
    int64_t v47; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v4;
    void* v11;
    int64_t v27;
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
        if (strcmp(_fn_name, "String_char_count") == 0) {
            v37 = String_char_count((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v37 = String_trim((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v37 = HashMap_len((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v37 = HashMap_keys((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v37 = str_parse_int((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v37 = Vec_capacity((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v37 = p_abs((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "count_expr_nodes") == 0) {
            v37 = count_expr_nodes((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v37 = str_parse_hex((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v37 = String_to_uppercase((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v37 = Result_unwrap((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v37 = String_bytes((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "op_tag") == 0) {
            v37 = op_tag((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v37 = p_str_length((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v37 = HashMap_values((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v37 = p_str_hash((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v37 = Vec_first((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v37 = Vec_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v37 = String_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v37 = Option_unwrap((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "vec_enumerate") == 0) {
            v37 = vec_enumerate((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v37 = Vec_pop((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v37 = char_to_lower((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v37 = StringBuilder_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v37 = String_from((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v37 = char_to_upper((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v37 = String_to_string((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "stmt_line") == 0) {
            v37 = stmt_line((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v37 = Vec_last((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v37 = str_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v37 = HashMap_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v37 = String_parse_int((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v37 = ByteBuffer_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v37 = String_to_lowercase((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v37 = allocator_bytes_used((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v37 = String_len((void*)v3, (int64_t)v32);
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

void* p_str_from_ptr(void* v0, int64_t v1) {
    p_call_stack_push("p_str_from_ptr");
    int64_t v87;
    void* v103;
    void* v76;
    void* v6;
    int64_t v105; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    int64_t v111; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v42;
    int64_t v45; /* placeholder for void */
    void* v17;
    int64_t v18; /* placeholder for void */
    int64_t v64;
    int64_t v85;
    int64_t v98;
    int64_t v99; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    uint8_t v77;
    int64_t v39;
    int64_t v54;
    int64_t v34;
    int64_t v50; /* placeholder for void */
    int64_t v44;
    void* v80;
    int64_t v24; /* placeholder for void */
    int64_t v96;
    int64_t v51; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v35;
    int64_t v11; /* placeholder for void */
    int64_t v59;
    void* v62;
    void* v110;
    void* v13;
    int64_t v67; /* placeholder for void */
    int64_t v84;
    void* v65;
    int64_t v58; /* placeholder for void */
    int64_t v81;
    int64_t v16; /* placeholder for void */
    void* v108;
    int64_t v46;
    void* v26;
    int64_t v14;
    int64_t v15; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v5;
    void* v31;
    int64_t v72;
    uint8_t v30;
    void* v23;
    int64_t v61; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    void* v57;
    int64_t v21; /* placeholder for void */
    void* v60;
    int64_t v95; /* placeholder for void */
    void* v56;
    int64_t v22;
    void* v7;
    void* v20;
    int64_t v27; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    int64_t v53;
    bool v3;
    int64_t v75; /* placeholder for void */
    int64_t v89;
    int64_t v90; /* placeholder for void */
    int64_t v25;
    int64_t v97;
    void* v69;
    int64_t v100;
    int64_t v52; /* placeholder for void */
    int64_t v102; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v43;
    int64_t v12; /* placeholder for void */
    int64_t v55;
    int64_t v68;
    int64_t v41; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    void* v78;
    int64_t v8; /* placeholder for void */
    void* v10;
    int64_t v71; /* placeholder for void */
    void* v48;
    int64_t v40;
    int64_t v88;
    int64_t v73;
    void* v29;
    int64_t v86; /* placeholder for void */
    int64_t v91;
    int64_t v83;
    void* v93;
    int64_t v104;
    int64_t v107;
    int64_t v109; /* placeholder for void */
    int64_t v2;
    int64_t v9;
    bool v36;
    int64_t v38;
    int64_t v4; /* placeholder for void */
    uint8_t v49;
    int64_t v70; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    bool v74;
    void* v94;
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
    int64_t v4; /* placeholder for void */
    int64_t v3;
    int64_t v2;
    v2 = 0LL;
    v3 = (int64_t)String_to_uppercase((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return (void*)v3;
    return 0;
}

bool Vec_contains_val(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_contains_val");
    int64_t v3;
    int64_t v5; /* placeholder for void */
    bool v4;
    v3 = 0LL;
    v4 = (bool)vec_contains_val((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

bool char_is_alpha(void* v0, int64_t v1) {
    p_call_stack_push("char_is_alpha");
    int64_t v7;
    int64_t v9;
    bool v11;
    bool v12;
    bool v8;
    int64_t v13; /* placeholder for void */
    bool v10;
    int64_t v4;
    bool v5;
    int64_t v2;
    bool v3;
    bool v6;
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

int64_t Option_unwrap(void* v0, void* v1) {
    p_call_stack_push("Option_unwrap");
    int64_t v20; /* placeholder for void */
    int64_t v12;
    bool v13;
    int64_t v3;
    int64_t v6; /* placeholder for void */
    int64_t v8;
    int64_t v19;
    bool v5;
    int64_t v9; /* placeholder for void */
    void* v2;
    int64_t v17; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v11;
    int64_t v4;
    void* v10;
    void* v7;
    int64_t v14; /* placeholder for void */
    void* v15;
    int64_t v16; /* placeholder for void */
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
    v15 = _str397;
    p_panic(v15);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

void StringBuilder_append_int(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("StringBuilder_append_int");
    void* v6;
    void* v9;
    int64_t v11;
    int64_t v8; /* placeholder for void */
    int64_t v12;
    int64_t v13;
    int64_t v14; /* placeholder for void */
    void* v5;
    void* v3;
    void* v4;
    void* v7;
    void* v10;
    int64_t v15; /* placeholder for void */
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

int64_t vec_flat_map(void* v0, int64_t v1, void* v2) {
    p_call_stack_push("vec_flat_map");
    bool v17;
    int64_t v46;
    int64_t v34;
    int64_t v13; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v45;
    int64_t v33; /* placeholder for void */
    int64_t v47;
    int64_t v37; /* placeholder for void */
    int64_t v35;
    int64_t v21;
    int64_t v19;
    int64_t v18; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    void* v9;
    int64_t v3;
    int64_t v8;
    int64_t v14; /* placeholder for void */
    void* v23;
    int64_t v55;
    int64_t v38;
    int64_t v42;
    int64_t v11;
    int64_t v15;
    void* v31;
    int64_t v4;
    int64_t v40;
    int64_t v39;
    int64_t v51;
    int64_t v26;
    int64_t v20;
    int64_t v22;
    int64_t v25;
    int64_t v16;
    int64_t v30;
    int64_t v32; /* placeholder for void */
    int64_t v43;
    void* v12;
    bool v36;
    int64_t v56; /* placeholder for void */
    int64_t v41;
    int64_t v52;
    void* v28;
    int64_t v53; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v7;
    int64_t v29; /* placeholder for void */
    int64_t v27;
    int64_t v48; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v50;
    int64_t v49; /* placeholder for void */
    void* v5;
    v3 = 0LL;
    v4 = (int64_t)Vec_new((void*)v3);
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = 0LL;
    v8 = (int64_t)Vec_len((void*)v7, (void*)v1);
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
    v19 = *(int64_t*)v12;
    v20 = 0LL;
    v21 = (int64_t)Vec_get((void*)v20, (void*)v1, v19);
    {
        if (!v2) abort();
        char* _fn_name = *(char**)v2;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "String_char_count") == 0) {
            v22 = String_char_count((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v22 = String_trim((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v22 = HashMap_len((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v22 = HashMap_keys((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v22 = str_parse_int((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v22 = Vec_capacity((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v22 = p_abs((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "count_expr_nodes") == 0) {
            v22 = count_expr_nodes((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v22 = str_parse_hex((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v22 = String_to_uppercase((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v22 = Result_unwrap((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v22 = String_bytes((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "op_tag") == 0) {
            v22 = op_tag((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v22 = p_str_length((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v22 = HashMap_values((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v22 = p_str_hash((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v22 = Vec_first((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v22 = Vec_len((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v22 = String_clone((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v22 = Option_unwrap((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "vec_enumerate") == 0) {
            v22 = vec_enumerate((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v22 = Vec_pop((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v22 = char_to_lower((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v22 = StringBuilder_len((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v22 = String_from((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v22 = char_to_upper((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v22 = String_to_string((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "stmt_line") == 0) {
            v22 = stmt_line((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v22 = Vec_last((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v22 = str_len((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v22 = HashMap_clone((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v22 = String_parse_int((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v22 = ByteBuffer_len((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v22 = String_to_lowercase((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v22 = allocator_bytes_used((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v22 = String_len((void*)v2, (int64_t)v21);
        } else { abort(); }
    }
    v23 = malloc(8);
    *(int64_t*)v23 = (int64_t)v22;
    v25 = *(int64_t*)v23;
    v26 = 0LL;
    v27 = (int64_t)Vec_len((void*)v26, (void*)v25);
    v28 = malloc(8);
    *(int64_t*)v28 = (int64_t)v27;
    v30 = 0LL;
    v31 = malloc(8);
    *(int64_t*)v31 = (int64_t)v30;
    goto bb4;
  bb3:
    v55 = *(int64_t*)v5;
    p_call_stack_pop();
    return v55;
  bb4:
    v34 = *(int64_t*)v31;
    v35 = *(int64_t*)v28;
    v36 = v34 < v35;
    if (v36) goto bb5;
    else goto bb6;
  bb5:
    v38 = *(int64_t*)v5;
    v39 = *(int64_t*)v23;
    v40 = *(int64_t*)v31;
    v41 = 0LL;
    v42 = (int64_t)Vec_get((void*)v41, (void*)v39, v40);
    v43 = 0LL;
    Vec_push((void*)v43, (void*)v38, v42);
    v45 = *(int64_t*)v31;
    v46 = 1LL;
    v47 = v45 + v46;
    *(int64_t*)v31 = (int64_t)v47;
    goto bb4;
  bb6:
    v50 = *(int64_t*)v12;
    v51 = 1LL;
    v52 = v50 + v51;
    *(int64_t*)v12 = (int64_t)v52;
    goto bb1;
    return 0;
}

void TraitImpl_new(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("TraitImpl_new");
    int64_t v12; /* placeholder for void */
    void* v33;
    int64_t v40; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    void* v34;
    int64_t v41; /* placeholder for void */
    void* v4;
    int64_t v36; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    void* v27;
    int64_t v9;
    void* v22;
    void* v37;
    void* v21;
    void* v5;
    int64_t v20; /* placeholder for void */
    void* v11;
    void* v29;
    void* v35;
    void* v14;
    void* v18;
    int64_t v28; /* placeholder for void */
    void* v19;
    void* v25;
    void* v31;
    void* v39;
    void* v38;
    void* v10;
    int64_t v32; /* placeholder for void */
    void* v7;
    void* v26;
    void* v30;
    int64_t v17;
    int64_t v13;
    int64_t v8; /* placeholder for void */
    void* v23;
    void* v15;
    v4 = malloc(40);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = (char*)v4 + 8;
    *(void**)v7 = (void*)v3;
    v9 = 0LL;
    v10 = Vec_new((void*)v9);
    v11 = (char*)v4 + 16;
    *(void**)v11 = (void*)v10;
    v13 = 0LL;
    v14 = Vec_new((void*)v13);
    v15 = (char*)v4 + 24;
    *(void**)v15 = (void*)v14;
    v17 = 0LL;
    v18 = Vec_new((void*)v17);
    v19 = (char*)v4 + 32;
    *(void**)v19 = (void*)v18;
    v21 = (char*)v4 + 0;
    v22 = (char*)v0 + 0;
    v23 = *(void**)v21;
    *(void**)v22 = (void*)v23;
    v25 = (char*)v4 + 8;
    v26 = (char*)v0 + 8;
    v27 = *(void**)v25;
    *(void**)v26 = (void*)v27;
    v29 = (char*)v4 + 16;
    v30 = (char*)v0 + 16;
    v31 = *(void**)v29;
    *(void**)v30 = (void*)v31;
    v33 = (char*)v4 + 24;
    v34 = (char*)v0 + 24;
    v35 = *(void**)v33;
    *(void**)v34 = (void*)v35;
    v37 = (char*)v4 + 32;
    v38 = (char*)v0 + 32;
    v39 = *(void**)v37;
    *(void**)v38 = (void*)v39;
    p_call_stack_pop();
    return;
    return;
}

void StateDef_new(void* v0, void* v1, void* v2) {
    p_call_stack_push("StateDef_new");
    void* v8;
    void* v10;
    void* v11;
    void* v12;
    void* v4;
    int64_t v13; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v3;
    int64_t v6;
    void* v15;
    void* v14;
    int64_t v9; /* placeholder for void */
    void* v7;
    int64_t v18; /* placeholder for void */
    void* v16;
    v3 = malloc(16);
    v4 = (char*)v3 + 0;
    *(void**)v4 = (void*)v2;
    v6 = 0LL;
    v7 = Vec_new((void*)v6);
    v8 = (char*)v3 + 8;
    *(void**)v8 = (void*)v7;
    v10 = (char*)v3 + 0;
    v11 = (char*)v0 + 0;
    v12 = *(void**)v10;
    *(void**)v11 = (void*)v12;
    v14 = (char*)v3 + 8;
    v15 = (char*)v0 + 8;
    v16 = *(void**)v14;
    *(void**)v15 = (void*)v16;
    p_call_stack_pop();
    return;
    return;
}

void* str_pad_right(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("str_pad_right");
    int64_t v20; /* placeholder for void */
    void* v23;
    int64_t v8;
    int64_t v22;
    int64_t v15;
    int64_t v13;
    int64_t v4;
    int64_t v24; /* placeholder for void */
    bool v9;
    void* v21;
    void* v14;
    void* v6;
    int64_t v16;
    int64_t v10; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    void* v18;
    int64_t v11; /* placeholder for void */
    int64_t v17;
    int64_t v5;
    int64_t v7; /* placeholder for void */
    void* v19;
    v4 = 0LL;
    v5 = (int64_t)str_len((void*)v4, v1);
    v6 = malloc(8);
    *(int64_t*)v6 = (int64_t)v5;
    v8 = *(int64_t*)v6;
    v9 = v8 >= v2;
    if (v9) goto bb1;
    else goto bb2;
  bb1:
    p_call_stack_pop();
    return v1;
  bb2:
    goto bb3;
  bb3:
    v13 = 0LL;
    v14 = char_to_str((void*)v13, v3);
    v15 = *(int64_t*)v6;
    v16 = v2 - v15;
    v17 = 0LL;
    v18 = str_repeat((void*)v17, v14, v16);
    v19 = malloc(8);
    *(void**)v19 = (void*)v18;
    v21 = *(void**)v19;
    v22 = 0LL;
    v23 = str_concat((void*)v22, v1, v21);
    p_call_stack_pop();
    return v23;
    return 0;
}

void Option_map(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Option_map");
    int64_t v5;
    void* v32;
    void* v18;
    int64_t v25; /* placeholder for void */
    void* v4;
    int64_t v33;
    void* v34;
    int64_t v12;
    int64_t v38;
    int64_t v6;
    int64_t v8; /* placeholder for void */
    int64_t v24;
    int64_t v43; /* placeholder for void */
    void* v27;
    int64_t v35; /* placeholder for void */
    int64_t v20;
    int64_t v21; /* placeholder for void */
    void* v23;
    void* v41;
    void* v37;
    int64_t v46; /* placeholder for void */
    bool v7;
    void* v13;
    int64_t v26; /* placeholder for void */
    void* v9;
    int64_t v14; /* placeholder for void */
    void* v36;
    void* v19;
    void* v40;
    int64_t v10;
    void* v16;
    int64_t v29;
    int64_t v39; /* placeholder for void */
    int64_t v42;
    int64_t v28;
    int64_t v45; /* placeholder for void */
    int64_t v15;
    int64_t v31; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    void* v22;
    int64_t v17; /* placeholder for void */
    bool v30;
    void* v11;
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
        if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "count_expr_nodes") == 0) {
            v15 = count_expr_nodes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "op_tag") == 0) {
            v15 = op_tag((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "vec_enumerate") == 0) {
            v15 = vec_enumerate((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "stmt_line") == 0) {
            v15 = stmt_line((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
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

void ModelDef_new(void* v0, void* v1, void* v2, void* v3, void* v4, bool v5) {
    p_call_stack_push("ModelDef_new");
    void* v28;
    void* v75;
    int64_t v69; /* placeholder for void */
    int64_t v99; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    void* v111;
    void* v137;
    void* v58;
    int64_t v44;
    bool v60;
    void* v124;
    void* v144;
    bool v166;
    int64_t v113; /* placeholder for void */
    void* v18;
    void* v24;
    int64_t v40;
    void* v71;
    void* v83;
    void* v32;
    void* v13;
    void* v38;
    void* v30;
    void* v126;
    int64_t v130;
    int64_t v134;
    int64_t v10; /* placeholder for void */
    void* v110;
    int64_t v81; /* placeholder for void */
    int64_t v138;
    void* v176;
    int64_t v159; /* placeholder for void */
    bool v154;
    void* v185;
    bool v92;
    void* v172;
    void* v74;
    int64_t v95; /* placeholder for void */
    void* v56;
    void* v78;
    void* v119;
    void* v174;
    void* v177;
    void* v157;
    bool v158;
    int64_t v27; /* placeholder for void */
    void* v70;
    bool v190;
    void* v97;
    int64_t v131; /* placeholder for void */
    bool v170;
    void* v82;
    int64_t v121; /* placeholder for void */
    int64_t v146;
    void* v148;
    void* v91;
    void* v128;
    void* v129;
    void* v152;
    void* v33;
    int64_t v57; /* placeholder for void */
    int64_t v52;
    int64_t v65; /* placeholder for void */
    void* v55;
    void* v86;
    int64_t v93; /* placeholder for void */
    void* v43;
    void* v21;
    void* v160;
    void* v29;
    void* v9;
    int64_t v175; /* placeholder for void */
    void* v182;
    int64_t v61; /* placeholder for void */
    int64_t v187; /* placeholder for void */
    void* v136;
    int64_t v151; /* placeholder for void */
    void* v54;
    void* v20;
    void* v107;
    int64_t v73; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    void* v80;
    void* v108;
    void* v150;
    void* v161;
    void* v105;
    void* v173;
    void* v25;
    void* v153;
    void* v180;
    int64_t v191; /* placeholder for void */
    void* v141;
    void* v114;
    bool v64;
    void* v98;
    void* v59;
    int64_t v8; /* placeholder for void */
    void* v14;
    void* v88;
    void* v118;
    void* v11;
    void* v116;
    void* v132;
    int64_t v37; /* placeholder for void */
    void* v140;
    int64_t v143; /* placeholder for void */
    void* v94;
    void* v87;
    void* v149;
    void* v63;
    void* v156;
    void* v162;
    int64_t v163; /* placeholder for void */
    void* v164;
    void* v145;
    int64_t v167; /* placeholder for void */
    void* v181;
    void* v34;
    int64_t v36;
    int64_t v15; /* placeholder for void */
    void* v16;
    void* v106;
    void* v178;
    int64_t v45; /* placeholder for void */
    int64_t v142;
    void* v12;
    void* v133;
    bool v76;
    int64_t v117; /* placeholder for void */
    void* v66;
    void* v90;
    bool v72;
    void* v46;
    void* v169;
    int64_t v103; /* placeholder for void */
    void* v62;
    int64_t v89; /* placeholder for void */
    int64_t v109; /* placeholder for void */
    void* v120;
    int64_t v135; /* placeholder for void */
    void* v50;
    void* v112;
    void* v68;
    void* v26;
    void* v39;
    int64_t v41; /* placeholder for void */
    void* v79;
    void* v115;
    void* v96;
    void* v6;
    int64_t v85; /* placeholder for void */
    void* v122;
    int64_t v125; /* placeholder for void */
    int64_t v147; /* placeholder for void */
    void* v165;
    int64_t v183; /* placeholder for void */
    void* v188;
    int64_t v192; /* placeholder for void */
    void* v168;
    void* v17;
    int64_t v48;
    void* v123;
    int64_t v139; /* placeholder for void */
    void* v189;
    void* v67;
    int64_t v53; /* placeholder for void */
    int64_t v155; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    void* v22;
    void* v35;
    void* v42;
    void* v101;
    int64_t v171; /* placeholder for void */
    void* v127;
    int64_t v23; /* placeholder for void */
    void* v7;
    void* v47;
    void* v104;
    void* v51;
    int64_t v179; /* placeholder for void */
    void* v184;
    bool v186;
    void* v100;
    void* v102;
    void* v84;
    v6 = malloc(184);
    v7 = (char*)v6 + 0;
    *(void**)v7 = (void*)v2;
    v9 = (char*)v6 + 8;
    *(void**)v9 = (void*)v3;
    v11 = (char*)v6 + 16;
    v12 = (char*)v4 + 0;
    v13 = (char*)v11 + 0;
    v14 = *(void**)v12;
    *(void**)v13 = (void*)v14;
    v16 = (char*)v4 + 8;
    v17 = (char*)v11 + 8;
    v18 = *(void**)v16;
    *(void**)v17 = (void*)v18;
    v20 = (char*)v4 + 16;
    v21 = (char*)v11 + 16;
    v22 = *(void**)v20;
    *(void**)v21 = (void*)v22;
    v24 = (char*)v4 + 24;
    v25 = (char*)v11 + 24;
    v26 = *(void**)v24;
    *(void**)v25 = (void*)v26;
    v28 = (char*)v4 + 32;
    v29 = (char*)v11 + 32;
    v30 = *(void**)v28;
    *(void**)v29 = (void*)v30;
    v32 = (char*)v4 + 40;
    v33 = (char*)v11 + 40;
    v34 = (char*)v32 + 0;
    v35 = (char*)v33 + 0;
    v36 = *(int64_t*)v34;
    *(int64_t*)v35 = (int64_t)v36;
    v38 = (char*)v32 + 8;
    v39 = (char*)v33 + 8;
    v40 = *(int64_t*)v38;
    *(int64_t*)v39 = (int64_t)v40;
    v42 = (char*)v32 + 16;
    v43 = (char*)v33 + 16;
    v44 = *(int64_t*)v42;
    *(int64_t*)v43 = (int64_t)v44;
    v46 = (char*)v32 + 24;
    v47 = (char*)v33 + 24;
    v48 = *(int64_t*)v46;
    *(int64_t*)v47 = (int64_t)v48;
    v50 = (char*)v32 + 32;
    v51 = (char*)v33 + 32;
    v52 = *(int64_t*)v50;
    *(int64_t*)v51 = (int64_t)v52;
    v54 = (char*)v4 + 80;
    v55 = (char*)v11 + 80;
    v56 = *(void**)v54;
    *(void**)v55 = (void*)v56;
    v58 = (char*)v4 + 88;
    v59 = (char*)v11 + 88;
    v60 = *(bool*)v58;
    *(bool*)v59 = v60;
    v62 = (char*)v4 + 96;
    v63 = (char*)v11 + 96;
    v64 = *(bool*)v62;
    *(bool*)v63 = v64;
    v66 = (char*)v4 + 104;
    v67 = (char*)v11 + 104;
    v68 = *(void**)v66;
    *(void**)v67 = (void*)v68;
    v70 = (char*)v4 + 112;
    v71 = (char*)v11 + 112;
    v72 = *(bool*)v70;
    *(bool*)v71 = v72;
    v74 = (char*)v4 + 120;
    v75 = (char*)v11 + 120;
    v76 = *(bool*)v74;
    *(bool*)v75 = v76;
    v78 = (char*)v4 + 128;
    v79 = (char*)v11 + 128;
    v80 = *(void**)v78;
    *(void**)v79 = (void*)v80;
    v82 = (char*)v4 + 136;
    v83 = (char*)v11 + 136;
    v84 = *(void**)v82;
    *(void**)v83 = (void*)v84;
    v86 = (char*)v4 + 144;
    v87 = (char*)v11 + 144;
    v88 = *(void**)v86;
    *(void**)v87 = (void*)v88;
    v90 = (char*)v4 + 152;
    v91 = (char*)v11 + 152;
    v92 = *(bool*)v90;
    *(bool*)v91 = v92;
    v94 = (char*)v6 + 176;
    *(bool*)v94 = v5;
    v96 = (char*)v6 + 0;
    v97 = (char*)v0 + 0;
    v98 = *(void**)v96;
    *(void**)v97 = (void*)v98;
    v100 = (char*)v6 + 8;
    v101 = (char*)v0 + 8;
    v102 = *(void**)v100;
    *(void**)v101 = (void*)v102;
    v104 = (char*)v6 + 16;
    v105 = (char*)v0 + 16;
    v106 = (char*)v104 + 0;
    v107 = (char*)v105 + 0;
    v108 = *(void**)v106;
    *(void**)v107 = (void*)v108;
    v110 = (char*)v104 + 8;
    v111 = (char*)v105 + 8;
    v112 = *(void**)v110;
    *(void**)v111 = (void*)v112;
    v114 = (char*)v104 + 16;
    v115 = (char*)v105 + 16;
    v116 = *(void**)v114;
    *(void**)v115 = (void*)v116;
    v118 = (char*)v104 + 24;
    v119 = (char*)v105 + 24;
    v120 = *(void**)v118;
    *(void**)v119 = (void*)v120;
    v122 = (char*)v104 + 32;
    v123 = (char*)v105 + 32;
    v124 = *(void**)v122;
    *(void**)v123 = (void*)v124;
    v126 = (char*)v104 + 40;
    v127 = (char*)v105 + 40;
    v128 = (char*)v126 + 0;
    v129 = (char*)v127 + 0;
    v130 = *(int64_t*)v128;
    *(int64_t*)v129 = (int64_t)v130;
    v132 = (char*)v126 + 8;
    v133 = (char*)v127 + 8;
    v134 = *(int64_t*)v132;
    *(int64_t*)v133 = (int64_t)v134;
    v136 = (char*)v126 + 16;
    v137 = (char*)v127 + 16;
    v138 = *(int64_t*)v136;
    *(int64_t*)v137 = (int64_t)v138;
    v140 = (char*)v126 + 24;
    v141 = (char*)v127 + 24;
    v142 = *(int64_t*)v140;
    *(int64_t*)v141 = (int64_t)v142;
    v144 = (char*)v126 + 32;
    v145 = (char*)v127 + 32;
    v146 = *(int64_t*)v144;
    *(int64_t*)v145 = (int64_t)v146;
    v148 = (char*)v104 + 80;
    v149 = (char*)v105 + 80;
    v150 = *(void**)v148;
    *(void**)v149 = (void*)v150;
    v152 = (char*)v104 + 88;
    v153 = (char*)v105 + 88;
    v154 = *(bool*)v152;
    *(bool*)v153 = v154;
    v156 = (char*)v104 + 96;
    v157 = (char*)v105 + 96;
    v158 = *(bool*)v156;
    *(bool*)v157 = v158;
    v160 = (char*)v104 + 104;
    v161 = (char*)v105 + 104;
    v162 = *(void**)v160;
    *(void**)v161 = (void*)v162;
    v164 = (char*)v104 + 112;
    v165 = (char*)v105 + 112;
    v166 = *(bool*)v164;
    *(bool*)v165 = v166;
    v168 = (char*)v104 + 120;
    v169 = (char*)v105 + 120;
    v170 = *(bool*)v168;
    *(bool*)v169 = v170;
    v172 = (char*)v104 + 128;
    v173 = (char*)v105 + 128;
    v174 = *(void**)v172;
    *(void**)v173 = (void*)v174;
    v176 = (char*)v104 + 136;
    v177 = (char*)v105 + 136;
    v178 = *(void**)v176;
    *(void**)v177 = (void*)v178;
    v180 = (char*)v104 + 144;
    v181 = (char*)v105 + 144;
    v182 = *(void**)v180;
    *(void**)v181 = (void*)v182;
    v184 = (char*)v104 + 152;
    v185 = (char*)v105 + 152;
    v186 = *(bool*)v184;
    *(bool*)v185 = v186;
    v188 = (char*)v6 + 176;
    v189 = (char*)v0 + 176;
    v190 = *(bool*)v188;
    *(bool*)v189 = v190;
    p_call_stack_pop();
    return;
    return;
}

void Attribute_new(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Attribute_new");
    void* v4;
    void* v15;
    void* v14;
    void* v5;
    int64_t v6; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v13;
    int64_t v17; /* placeholder for void */
    void* v7;
    void* v11;
    int64_t v12; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    void* v9;
    void* v10;
    v4 = malloc(16);
    v5 = (char*)v4 + 0;
    *(void**)v5 = (void*)v2;
    v7 = (char*)v4 + 8;
    *(void**)v7 = (void*)v3;
    v9 = (char*)v4 + 0;
    v10 = (char*)v0 + 0;
    v11 = *(void**)v9;
    *(void**)v10 = (void*)v11;
    v13 = (char*)v4 + 8;
    v14 = (char*)v0 + 8;
    v15 = *(void**)v13;
    *(void**)v14 = (void*)v15;
    p_call_stack_pop();
    return;
    return;
}

void* int_to_str_padded(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("int_to_str_padded");
    int64_t v3;
    int64_t v6; /* placeholder for void */
    int64_t v8;
    void* v7;
    void* v10;
    void* v4;
    int64_t v9;
    int64_t v11; /* placeholder for void */
    void* v5;
    v3 = 0LL;
    v4 = int_to_str((void*)v3, v1);
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = *(void**)v5;
    v8 = 48LL;
    v9 = 0LL;
    v10 = str_pad_left((void*)v9, v7, v2, v8);
    p_call_stack_pop();
    return v10;
    return 0;
}

void EnumDef_new(void* v0, void* v1, void* v2, void* v3, bool v4) {
    p_call_stack_push("EnumDef_new");
    void* v18;
    void* v21;
    int64_t v23; /* placeholder for void */
    void* v30;
    void* v10;
    int64_t v31; /* placeholder for void */
    void* v8;
    int64_t v7; /* placeholder for void */
    void* v6;
    void* v22;
    void* v13;
    int64_t v12;
    void* v20;
    void* v16;
    int64_t v15; /* placeholder for void */
    void* v17;
    int64_t v19; /* placeholder for void */
    void* v25;
    void* v29;
    void* v5;
    bool v26;
    int64_t v32; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    void* v28;
    void* v14;
    void* v24;
    v5 = malloc(32);
    v6 = (char*)v5 + 0;
    *(void**)v6 = (void*)v2;
    v8 = (char*)v5 + 8;
    *(void**)v8 = (void*)v3;
    v10 = (char*)v5 + 16;
    *(bool*)v10 = v4;
    v12 = 0LL;
    v13 = Vec_new((void*)v12);
    v14 = (char*)v5 + 24;
    *(void**)v14 = (void*)v13;
    v16 = (char*)v5 + 0;
    v17 = (char*)v0 + 0;
    v18 = *(void**)v16;
    *(void**)v17 = (void*)v18;
    v20 = (char*)v5 + 8;
    v21 = (char*)v0 + 8;
    v22 = *(void**)v20;
    *(void**)v21 = (void*)v22;
    v24 = (char*)v5 + 16;
    v25 = (char*)v0 + 16;
    v26 = *(bool*)v24;
    *(bool*)v25 = v26;
    v28 = (char*)v5 + 24;
    v29 = (char*)v0 + 24;
    v30 = *(void**)v28;
    *(void**)v29 = (void*)v30;
    p_call_stack_pop();
    return;
    return;
}

int64_t vec_skip(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("vec_skip");
    int64_t v8;
    void* v22;
    int64_t v14;
    int64_t v3;
    int64_t v31;
    int64_t v16; /* placeholder for void */
    int64_t v13;
    int64_t v20; /* placeholder for void */
    bool v15;
    int64_t v29;
    int64_t v7;
    int64_t v34; /* placeholder for void */
    int64_t v4;
    int64_t v21;
    int64_t v25;
    int64_t v39; /* placeholder for void */
    int64_t v36;
    void* v9;
    int64_t v28; /* placeholder for void */
    int64_t v33;
    int64_t v6; /* placeholder for void */
    int64_t v17;
    int64_t v12; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    void* v5;
    int64_t v19; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v32;
    int64_t v10; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v40;
    int64_t v30;
    int64_t v35;
    int64_t v37;
    bool v27;
    int64_t v26;
    void* v11;
    v3 = 0LL;
    v4 = (int64_t)Vec_new((void*)v3);
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = 0LL;
    v8 = (int64_t)Vec_len((void*)v7, (void*)v1);
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = malloc(8);
    *(int64_t*)v11 = (int64_t)v2;
    v13 = *(int64_t*)v11;
    v14 = 0LL;
    v15 = v13 < v14;
    if (v15) goto bb1;
    else goto bb2;
  bb1:
    v17 = 0LL;
    *(int64_t*)v11 = (int64_t)v17;
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    v21 = *(int64_t*)v11;
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v21;
    goto bb4;
  bb4:
    v25 = *(int64_t*)v22;
    v26 = *(int64_t*)v9;
    v27 = v25 < v26;
    if (v27) goto bb5;
    else goto bb6;
  bb5:
    v29 = *(int64_t*)v5;
    v30 = *(int64_t*)v22;
    v31 = 0LL;
    v32 = (int64_t)Vec_get((void*)v31, (void*)v1, v30);
    v33 = 0LL;
    Vec_push((void*)v33, (void*)v29, v32);
    v35 = *(int64_t*)v22;
    v36 = 1LL;
    v37 = v35 + v36;
    *(int64_t*)v22 = (int64_t)v37;
    goto bb4;
  bb6:
    v40 = *(int64_t*)v5;
    p_call_stack_pop();
    return v40;
    return 0;
}

int64_t Vec_find(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_find");
    void* v8;
    int64_t v12;
    int64_t v29; /* placeholder for void */
    int64_t v6;
    int64_t v10; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v31;
    bool v13;
    int64_t v9; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v11;
    int64_t v14; /* placeholder for void */
    int64_t v17;
    int64_t v32;
    int64_t v19; /* placeholder for void */
    int64_t v20;
    int64_t v26;
    int64_t v5; /* placeholder for void */
    int64_t v3;
    int64_t v7;
    int64_t v23;
    void* v4;
    int64_t v27;
    int64_t v28;
    int64_t v16;
    void* v18;
    int64_t v25; /* placeholder for void */
    int64_t v15;
    bool v21;
    int64_t v22; /* placeholder for void */
    v3 = 0LL;
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = 0LL;
    v7 = (int64_t)Vec_len((void*)v6, v1);
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    goto bb1;
  bb1:
    v11 = *(int64_t*)v4;
    v12 = *(int64_t*)v8;
    v13 = v11 < v12;
    if (v13) goto bb2;
    else goto bb3;
  bb2:
    v15 = *(int64_t*)v4;
    v16 = 0LL;
    v17 = (int64_t)Vec_get((void*)v16, v1, v15);
    v18 = malloc(8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = *(int64_t*)v18;
    {
        if (!v2) abort();
        char* _fn_name = *(char**)v2;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "str_is_blank") == 0) {
            v21 = str_is_blank((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Option_is_some") == 0) {
            v21 = Option_is_some((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Option_is_none") == 0) {
            v21 = Option_is_none((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Vec_is_empty") == 0) {
            v21 = Vec_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_numeric") == 0) {
            v21 = str_is_numeric((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "String_is_empty") == 0) {
            v21 = String_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_alpha") == 0) {
            v21 = char_is_alpha((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "str_is_alpha") == 0) {
            v21 = str_is_alpha((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "HashMap_is_empty") == 0) {
            v21 = HashMap_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Result_is_err") == 0) {
            v21 = Result_is_err((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_alphanumeric") == 0) {
            v21 = str_is_alphanumeric((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_empty") == 0) {
            v21 = str_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "StringBuilder_is_empty") == 0) {
            v21 = StringBuilder_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Result_is_ok") == 0) {
            v21 = Result_is_ok((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_whitespace") == 0) {
            v21 = char_is_whitespace((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_upper") == 0) {
            v21 = char_is_upper((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "ByteBuffer_is_empty") == 0) {
            v21 = ByteBuffer_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_digit") == 0) {
            v21 = char_is_digit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_lower") == 0) {
            v21 = char_is_lower((void*)v2, (int64_t)v20);
        } else { abort(); }
    }
    if (v21) goto bb4;
    else goto bb5;
  bb3:
    v31 = 1LL;
    v32 = -v31;
    p_call_stack_pop();
    return v32;
  bb4:
    v23 = *(int64_t*)v18;
    p_call_stack_pop();
    return v23;
  bb5:
    goto bb6;
  bb6:
    v26 = *(int64_t*)v4;
    v27 = 1LL;
    v28 = v26 + v27;
    *(int64_t*)v4 = (int64_t)v28;
    goto bb1;
    return 0;
}

int64_t Vec_position(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_position");
    int64_t v27;
    int64_t v20;
    int64_t v30; /* placeholder for void */
    int64_t v32;
    int64_t v3;
    int64_t v28;
    bool v13;
    int64_t v22; /* placeholder for void */
    bool v21;
    int64_t v17;
    int64_t v15;
    int64_t v26;
    int64_t v33; /* placeholder for void */
    int64_t v11;
    int64_t v24; /* placeholder for void */
    void* v18;
    int64_t v31;
    int64_t v19; /* placeholder for void */
    void* v4;
    int64_t v7;
    int64_t v6;
    int64_t v9; /* placeholder for void */
    int64_t v12;
    int64_t v14; /* placeholder for void */
    void* v8;
    int64_t v23;
    int64_t v10; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v16;
    int64_t v25; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    v3 = 0LL;
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = 0LL;
    v7 = (int64_t)Vec_len((void*)v6, v1);
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    goto bb1;
  bb1:
    v11 = *(int64_t*)v4;
    v12 = *(int64_t*)v8;
    v13 = v11 < v12;
    if (v13) goto bb2;
    else goto bb3;
  bb2:
    v15 = *(int64_t*)v4;
    v16 = 0LL;
    v17 = (int64_t)Vec_get((void*)v16, v1, v15);
    v18 = malloc(8);
    *(int64_t*)v18 = (int64_t)v17;
    v20 = *(int64_t*)v18;
    {
        if (!v2) abort();
        char* _fn_name = *(char**)v2;
        if (!_fn_name) abort();
        if (strcmp(_fn_name, "str_is_blank") == 0) {
            v21 = str_is_blank((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Option_is_some") == 0) {
            v21 = Option_is_some((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Option_is_none") == 0) {
            v21 = Option_is_none((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Vec_is_empty") == 0) {
            v21 = Vec_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_numeric") == 0) {
            v21 = str_is_numeric((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "String_is_empty") == 0) {
            v21 = String_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_alpha") == 0) {
            v21 = char_is_alpha((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "str_is_alpha") == 0) {
            v21 = str_is_alpha((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "HashMap_is_empty") == 0) {
            v21 = HashMap_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Result_is_err") == 0) {
            v21 = Result_is_err((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_alphanumeric") == 0) {
            v21 = str_is_alphanumeric((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_empty") == 0) {
            v21 = str_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "StringBuilder_is_empty") == 0) {
            v21 = StringBuilder_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Result_is_ok") == 0) {
            v21 = Result_is_ok((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_whitespace") == 0) {
            v21 = char_is_whitespace((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_upper") == 0) {
            v21 = char_is_upper((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "ByteBuffer_is_empty") == 0) {
            v21 = ByteBuffer_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_digit") == 0) {
            v21 = char_is_digit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_lower") == 0) {
            v21 = char_is_lower((void*)v2, (int64_t)v20);
        } else { abort(); }
    }
    if (v21) goto bb4;
    else goto bb5;
  bb3:
    v31 = 1LL;
    v32 = -v31;
    p_call_stack_pop();
    return v32;
  bb4:
    v23 = *(int64_t*)v4;
    p_call_stack_pop();
    return v23;
  bb5:
    goto bb6;
  bb6:
    v26 = *(int64_t*)v4;
    v27 = 1LL;
    v28 = v26 + v27;
    *(int64_t*)v4 = (int64_t)v28;
    goto bb1;
    return 0;
}

int64_t String_to_string(void* v0, int64_t v1) {
    p_call_stack_push("String_to_string");
    int64_t v3;
    int64_t v2;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)String_from((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

bool str_is_empty(void* v0, void* v1) {
    p_call_stack_push("str_is_empty");
    bool v3;
    int64_t v2;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (bool)String_is_empty((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t Result_unwrap(void* v0, void* v1) {
    p_call_stack_push("Result_unwrap");
    bool v5;
    int64_t v11;
    void* v17;
    int64_t v3;
    int64_t v18; /* placeholder for void */
    bool v13;
    int64_t v4;
    int64_t v12;
    void* v7;
    int64_t v9; /* placeholder for void */
    void* v15;
    int64_t v19; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    void* v2;
    int64_t v8;
    void* v10;
    int64_t v14; /* placeholder for void */
    int64_t v16;
    int64_t v21;
    int64_t v20; /* placeholder for void */
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
    v17 = _str398;
    p_panic(v17);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

void Result_and_then(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Result_and_then");
    int64_t v8; /* placeholder for void */
    void* v16;
    void* v23;
    void* v4;
    int64_t v17; /* placeholder for void */
    int64_t v24;
    void* v11;
    int64_t v15;
    bool v30;
    int64_t v39; /* placeholder for void */
    int64_t v35;
    int64_t v42;
    int64_t v47; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    void* v9;
    void* v22;
    int64_t v29;
    void* v18;
    void* v13;
    int64_t v26; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    void* v19;
    void* v27;
    int64_t v33;
    int64_t v43; /* placeholder for void */
    int64_t v28;
    void* v44;
    bool v7;
    int64_t v20;
    void* v36;
    void* v40;
    void* v41;
    int64_t v49; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    void* v32;
    void* v38;
    void* v45;
    void* v34;
    int64_t v10;
    int64_t v37; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v5;
    int64_t v46;
    int64_t v14; /* placeholder for void */
    int64_t v12;
    int64_t v6;
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
        if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "count_expr_nodes") == 0) {
            v15 = count_expr_nodes((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "op_tag") == 0) {
            v15 = op_tag((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "vec_enumerate") == 0) {
            v15 = vec_enumerate((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "stmt_line") == 0) {
            v15 = stmt_line((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
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

int64_t str_find(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_find");
    int64_t v4;
    int64_t v3;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)String_find((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

void assert(void* v0, bool v1) {
    p_call_stack_push("assert");
    int64_t v3; /* placeholder for void */
    void* v4;
    int64_t v6; /* placeholder for void */
    int64_t v2;
    int64_t v5; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    v2 = !v1;
    if (v2) goto bb1;
    else goto bb2;
  bb1:
    v4 = _str399;
    p_panic(v4);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return;
    return;
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

void SelectCase_new(void* v0, void* v1, void* v2, void* v3, void* v4) {
    p_call_stack_push("SelectCase_new");
    int64_t v9;
    int64_t v26; /* placeholder for void */
    void* v46;
    int64_t v53; /* placeholder for void */
    int64_t v21;
    int64_t v13;
    void* v15;
    void* v49;
    void* v51;
    int64_t v44; /* placeholder for void */
    void* v38;
    int64_t v14; /* placeholder for void */
    void* v28;
    int64_t v22; /* placeholder for void */
    void* v29;
    int64_t v17;
    int64_t v18; /* placeholder for void */
    void* v23;
    void* v25;
    void* v30;
    void* v16;
    void* v19;
    int64_t v24; /* placeholder for void */
    void* v27;
    int64_t v31;
    int64_t v32; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v12;
    int64_t v36; /* placeholder for void */
    void* v11;
    void* v20;
    int64_t v39;
    void* v41;
    void* v42;
    int64_t v43;
    void* v45;
    int64_t v35;
    void* v50;
    void* v6;
    void* v8;
    void* v7;
    void* v5;
    void* v34;
    void* v37;
    void* v47;
    int64_t v48; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    void* v33;
    int64_t v40; /* placeholder for void */
    v5 = malloc(48);
    v6 = (char*)v5 + 0;
    v7 = (char*)v2 + 0;
    v8 = (char*)v6 + 0;
    v9 = *(int64_t*)v7;
    *(int64_t*)v8 = (int64_t)v9;
    v11 = (char*)v2 + 8;
    v12 = (char*)v6 + 8;
    v13 = *(int64_t*)v11;
    *(int64_t*)v12 = (int64_t)v13;
    v15 = (char*)v2 + 16;
    v16 = (char*)v6 + 16;
    v17 = *(int64_t*)v15;
    *(int64_t*)v16 = (int64_t)v17;
    v19 = (char*)v2 + 24;
    v20 = (char*)v6 + 24;
    v21 = *(int64_t*)v19;
    *(int64_t*)v20 = (int64_t)v21;
    v23 = (char*)v5 + 32;
    *(void**)v23 = (void*)v3;
    v25 = (char*)v5 + 40;
    *(void**)v25 = (void*)v4;
    v27 = (char*)v5 + 0;
    v28 = (char*)v0 + 0;
    v29 = (char*)v27 + 0;
    v30 = (char*)v28 + 0;
    v31 = *(int64_t*)v29;
    *(int64_t*)v30 = (int64_t)v31;
    v33 = (char*)v27 + 8;
    v34 = (char*)v28 + 8;
    v35 = *(int64_t*)v33;
    *(int64_t*)v34 = (int64_t)v35;
    v37 = (char*)v27 + 16;
    v38 = (char*)v28 + 16;
    v39 = *(int64_t*)v37;
    *(int64_t*)v38 = (int64_t)v39;
    v41 = (char*)v27 + 24;
    v42 = (char*)v28 + 24;
    v43 = *(int64_t*)v41;
    *(int64_t*)v42 = (int64_t)v43;
    v45 = (char*)v5 + 32;
    v46 = (char*)v0 + 32;
    v47 = *(void**)v45;
    *(void**)v46 = (void*)v47;
    v49 = (char*)v5 + 40;
    v50 = (char*)v0 + 40;
    v51 = *(void**)v49;
    *(void**)v50 = (void*)v51;
    p_call_stack_pop();
    return;
    return;
}

int64_t HashMap_keys(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_keys");
    int64_t v36; /* placeholder for void */
    int64_t v60;
    bool v69;
    int64_t v33; /* placeholder for void */
    int64_t v16;
    int64_t v48;
    int64_t v37;
    void* v24;
    int64_t v10; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v23;
    int64_t v54; /* placeholder for void */
    int64_t v68;
    int64_t v2;
    int64_t v71; /* placeholder for void */
    int64_t v84;
    int64_t v86;
    void* v9;
    int64_t v49;
    int64_t v74;
    void* v12;
    int64_t v29;
    int64_t v75; /* placeholder for void */
    int64_t v80;
    int64_t v72;
    void* v38;
    int64_t v34;
    int64_t v64;
    void* v50;
    int64_t v62; /* placeholder for void */
    int64_t v41;
    int64_t v11;
    int64_t v22;
    int64_t v87; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    int64_t v3;
    bool v43;
    int64_t v21; /* placeholder for void */
    int64_t v61; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    bool v28;
    int64_t v77; /* placeholder for void */
    int64_t v67;
    int64_t v20; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v8;
    void* v15;
    int64_t v42;
    int64_t v44; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v55;
    int64_t v66;
    int64_t v83;
    int64_t v45;
    int64_t v46;
    bool v32;
    int64_t v39; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    void* v53;
    int64_t v76; /* placeholder for void */
    void* v59;
    int64_t v26;
    void* v4;
    bool v70;
    int64_t v27;
    int64_t v73;
    int64_t v79;
    bool v31;
    int64_t v25; /* placeholder for void */
    int64_t v63;
    int64_t v30;
    int64_t v5; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v52;
    void* v56;
    int64_t v58; /* placeholder for void */
    bool v65;
    void* v18;
    int64_t v78;
    void* v6;
    int64_t v47;
    int64_t v85; /* placeholder for void */
    int64_t v19;
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
    v84 = 0LL;
    Vec_sort((void*)v84, (void*)v83);
    v86 = *(int64_t*)v24;
    p_call_stack_pop();
    return v86;
  bb7:
    v72 = *(int64_t*)v24;
    v73 = *(int64_t*)v56;
    v74 = 0LL;
    Vec_push((void*)v74, (void*)v72, v73);
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

void assert_eq_i64(void* v0, int64_t v1, int64_t v2, void* v3) {
    p_call_stack_push("assert_eq_i64");
    int64_t v8; /* placeholder for void */
    bool v4;
    void* v9;
    int64_t v11;
    int64_t v20; /* placeholder for void */
    void* v21;
    int64_t v23; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    void* v12;
    int64_t v13; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v19;
    int64_t v16;
    int64_t v5; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v17;
    int64_t v18; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    void* v6;
    void* v14;
    v4 = v1 != v2;
    if (v4) goto bb1;
    else goto bb2;
  bb1:
    v6 = _str0;
    p_print_str(v6);
    p_print_str(v3);
    v9 = _str6;
    p_print_str(v9);
    v11 = 0LL;
    v12 = int_to_str((void*)v11, v2);
    p_print_str(v12);
    v14 = _str9;
    p_print_str(v14);
    v16 = 0LL;
    v17 = int_to_str((void*)v16, v1);
    p_print_str(v17);
    v19 = _str10;
    p_print_str(v19);
    v21 = _str4;
    p_panic(v21);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return;
    return;
}

bool str_is_alpha(void* v0, void* v1) {
    p_call_stack_push("str_is_alpha");
    bool v5;
    int64_t v9; /* placeholder for void */
    bool v34;
    int64_t v10;
    int64_t v27; /* placeholder for void */
    int64_t v22;
    int64_t v35; /* placeholder for void */
    int64_t v31;
    void* v11;
    int64_t v20;
    int64_t v32; /* placeholder for void */
    int64_t v14;
    int64_t v16;
    int64_t v21;
    int64_t v18; /* placeholder for void */
    bool v26;
    int64_t v13; /* placeholder for void */
    bool v7;
    int64_t v12; /* placeholder for void */
    int64_t v2;
    int64_t v29;
    int64_t v33; /* placeholder for void */
    int64_t v19;
    bool v23;
    int64_t v8; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    bool v17;
    int64_t v4;
    int64_t v15;
    int64_t v24;
    int64_t v25; /* placeholder for void */
    int64_t v30;
    int64_t v3;
    v2 = 0LL;
    v3 = (int64_t)str_len((void*)v2, v1);
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb1;
    else goto bb2;
  bb1:
    v7 = false;
    p_call_stack_pop();
    return v7;
  bb2:
    goto bb3;
  bb3:
    v10 = 0LL;
    v11 = malloc(8);
    *(int64_t*)v11 = (int64_t)v10;
    goto bb4;
  bb4:
    v14 = *(int64_t*)v11;
    v15 = 0LL;
    v16 = (int64_t)str_len((void*)v15, v1);
    v17 = v14 < v16;
    if (v17) goto bb5;
    else goto bb6;
  bb5:
    v19 = *(int64_t*)v11;
    v20 = 0LL;
    v21 = (int64_t)str_byte_at((void*)v20, v1, v19);
    v22 = 0LL;
    v23 = (bool)char_is_alpha((void*)v22, v21);
    v24 = !v23;
    if (v24) goto bb7;
    else goto bb8;
  bb6:
    v34 = true;
    p_call_stack_pop();
    return v34;
  bb7:
    v26 = false;
    p_call_stack_pop();
    return v26;
  bb8:
    goto bb9;
  bb9:
    v29 = *(int64_t*)v11;
    v30 = 1LL;
    v31 = v29 + v30;
    *(int64_t*)v11 = (int64_t)v31;
    goto bb4;
    return 0;
}

void p_str_copy_n(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("p_str_copy_n");
    int64_t v7; /* placeholder for void */
    uint8_t v23;
    int64_t v27;
    int64_t v35;
    int64_t v29; /* placeholder for void */
    void* v4;
    void* v9;
    int64_t v30;
    int64_t v21; /* placeholder for void */
    int64_t v12;
    void* v26;
    void* v22;
    int64_t v36;
    void* v16;
    int64_t v8;
    int64_t v42; /* placeholder for void */
    int64_t v38;
    int64_t v14; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v34;
    int64_t v39;
    int64_t v43; /* placeholder for void */
    bool v13;
    int64_t v25; /* placeholder for void */
    int64_t v31;
    int64_t v40;
    int64_t v11; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v24;
    int64_t v32;
    int64_t v18;
    void* v19;
    void* v6;
    int64_t v33; /* placeholder for void */
    int64_t v15;
    int64_t v28; /* placeholder for void */
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

int64_t String_to_uppercase(void* v0, int64_t v1) {
    p_call_stack_push("String_to_uppercase");
    void* v7;
    int64_t v38;
    int64_t v55; /* placeholder for void */
    int64_t v34;
    int64_t v46; /* placeholder for void */
    void* v27;
    int64_t v9;
    int64_t v32; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    void* v51;
    int64_t v60; /* placeholder for void */
    int64_t v61;
    bool v3;
    int64_t v4; /* placeholder for void */
    int64_t v62;
    int64_t v65;
    int64_t v30;
    int64_t v68; /* placeholder for void */
    int64_t v71;
    int64_t v72; /* placeholder for void */
    void* v45;
    int64_t v49; /* placeholder for void */
    void* v76;
    int64_t v77;
    int64_t v83; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v43;
    int64_t v2;
    int64_t v81;
    int64_t v23; /* placeholder for void */
    int64_t v87;
    bool v63;
    void* v88;
    int64_t v86;
    int64_t v89; /* placeholder for void */
    int64_t v95;
    int64_t v20;
    int64_t v21;
    void* v40;
    bool v67;
    int64_t v96; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    bool v66;
    void* v22;
    int64_t v85;
    int64_t v92; /* placeholder for void */
    int64_t v98;
    int64_t v8; /* placeholder for void */
    void* v28;
    int64_t v42;
    int64_t v44;
    int64_t v82;
    void* v54;
    int64_t v15; /* placeholder for void */
    int64_t v26;
    int64_t v59; /* placeholder for void */
    int64_t v39;
    int64_t v17;
    int64_t v93; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v25;
    int64_t v37; /* placeholder for void */
    uint8_t v58;
    int64_t v79; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v90;
    void* v48;
    bool v36;
    void* v31;
    int64_t v52; /* placeholder for void */
    int64_t v64;
    int64_t v73; /* placeholder for void */
    int64_t v50;
    int64_t v74; /* placeholder for void */
    int64_t v99; /* placeholder for void */
    int64_t v24;
    void* v94;
    int64_t v97; /* placeholder for void */
    int64_t v69;
    void* v91;
    int64_t v5;
    int64_t v47;
    void* v10;
    void* v57;
    int64_t v12; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v53;
    int64_t v80;
    int64_t v41; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v35;
    void* v13;
    void* v6;
    int64_t v14;
    int64_t v18; /* placeholder for void */
    int64_t v70;
    int64_t v78; /* placeholder for void */
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

void* StringBuilder_to_str(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_to_str");
    int64_t v4; /* placeholder for void */
    void* v3;
    void* v2;
    v2 = (char*)v1 + 0;
    v3 = *(void**)v2;
    p_call_stack_pop();
    return v3;
    return 0;
}

void ByteBuffer_new(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("ByteBuffer_new");
    int64_t v10; /* placeholder for void */
    void* v6;
    void* v12;
    void* v16;
    int64_t v17;
    void* v9;
    int64_t v4;
    int64_t v18; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    void* v5;
    int64_t v8;
    int64_t v14; /* placeholder for void */
    void* v15;
    void* v11;
    int64_t v7; /* placeholder for void */
    void* v13;
    void* v3;
    v3 = malloc(16);
    v4 = 0LL;
    v5 = Vec_new((void*)v4);
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

void ByteBuffer_from_string(void* v0, void* v1, void* v2) {
    p_call_stack_push("ByteBuffer_from_string");
    int64_t v52;
    int64_t v54; /* placeholder for void */
    int64_t v13;
    void* v14;
    int64_t v16; /* placeholder for void */
    void* v23;
    int64_t v31; /* placeholder for void */
    int64_t v18;
    int64_t v40;
    int64_t v6; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    uint8_t v33;
    int64_t v44; /* placeholder for void */
    void* v47;
    int64_t v20; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    int64_t v10;
    int64_t v12; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    void* v32;
    int64_t v17;
    int64_t v41;
    int64_t v42;
    int64_t v49; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    bool v19;
    void* v45;
    void* v46;
    void* v36;
    int64_t v15; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v5;
    int64_t v21;
    int64_t v25;
    int64_t v30; /* placeholder for void */
    int64_t v37;
    int64_t v38;
    void* v48;
    void* v26;
    int64_t v3;
    int64_t v9;
    void* v11;
    int64_t v22;
    int64_t v28;
    void* v29;
    int64_t v27; /* placeholder for void */
    void* v4;
    void* v7;
    void* v50;
    void* v51;
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

double str_parse_float(void* v0, void* v1) {
    p_call_stack_push("str_parse_float");
    int64_t v3; /* placeholder for void */
    double v2;
    v2 = (double)p_str_to_float(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void EventHandlerDef_new(void* v0, void* v1, void* v2) {
    p_call_stack_push("EventHandlerDef_new");
    void* v12;
    int64_t v13; /* placeholder for void */
    void* v29;
    void* v8;
    void* v15;
    int64_t v5; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    void* v4;
    void* v18;
    void* v23;
    int64_t v20; /* placeholder for void */
    void* v11;
    void* v21;
    int64_t v33; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    bool v14;
    int64_t v16; /* placeholder for void */
    void* v17;
    void* v25;
    void* v19;
    int64_t v9; /* placeholder for void */
    void* v27;
    int64_t v28; /* placeholder for void */
    void* v30;
    void* v26;
    void* v3;
    void* v22;
    int64_t v6;
    void* v7;
    int64_t v10;
    bool v31;
    v3 = malloc(32);
    v4 = (char*)v3 + 0;
    *(void**)v4 = (void*)v2;
    v6 = 0LL;
    v7 = Vec_new((void*)v6);
    v8 = (char*)v3 + 8;
    *(void**)v8 = (void*)v7;
    v10 = 0LL;
    v11 = Vec_new((void*)v10);
    v12 = (char*)v3 + 16;
    *(void**)v12 = (void*)v11;
    v14 = false;
    v15 = (char*)v3 + 24;
    *(bool*)v15 = v14;
    v17 = (char*)v3 + 0;
    v18 = (char*)v0 + 0;
    v19 = *(void**)v17;
    *(void**)v18 = (void*)v19;
    v21 = (char*)v3 + 8;
    v22 = (char*)v0 + 8;
    v23 = *(void**)v21;
    *(void**)v22 = (void*)v23;
    v25 = (char*)v3 + 16;
    v26 = (char*)v0 + 16;
    v27 = *(void**)v25;
    *(void**)v26 = (void*)v27;
    v29 = (char*)v3 + 24;
    v30 = (char*)v0 + 24;
    v31 = *(bool*)v29;
    *(bool*)v30 = v31;
    p_call_stack_pop();
    return;
    return;
}

int64_t Option_unwrap_or_else(void* v0, void* v1, void* v2) {
    p_call_stack_push("Option_unwrap_or_else");
    int64_t v9;
    int64_t v7; /* placeholder for void */
    int64_t v4;
    int64_t v10; /* placeholder for void */
    void* v11;
    int64_t v15; /* placeholder for void */
    bool v6;
    void* v3;
    int64_t v20; /* placeholder for void */
    void* v8;
    int64_t v12;
    int64_t v13;
    int64_t v16;
    int64_t v18; /* placeholder for void */
    bool v14;
    int64_t v17; /* placeholder for void */
    int64_t v5;
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
        if (strcmp(_fn_name, "HashMap_new") == 0) {
            v16 = HashMap_new((void*)v2);
        } else if (strcmp(_fn_name, "main") == 0) {
            v16 = main_entry((void*)v2);
        } else { abort(); }
    }
    p_call_stack_pop();
    return v16;
  bb5:
    goto bb1;
    return 0;
}

void Vec_truncate(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_truncate");
    int64_t v3; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    p_vec_truncate(v1, v2);
    p_call_stack_pop();
    return;
    return;
}

void* Vec_flat_map(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_flat_map");
    int64_t v4;
    int64_t v5; /* placeholder for void */
    int64_t v3;
    v3 = 0LL;
    v4 = (int64_t)vec_flat_map((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return (void*)v4;
    return 0;
}

void StringBuilder_new(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_new");
    void* v3;
    int64_t v16; /* placeholder for void */
    void* v13;
    int64_t v12; /* placeholder for void */
    void* v2;
    void* v4;
    void* v11;
    int64_t v17; /* placeholder for void */
    void* v7;
    void* v10;
    void* v14;
    int64_t v15;
    int64_t v5; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v6;
    void* v9;
    v2 = malloc(16);
    v3 = _str5;
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

bool Option_is_some(void* v0, void* v1) {
    p_call_stack_push("Option_is_some");
    int64_t v8;
    int64_t v17; /* placeholder for void */
    bool v19;
    int64_t v3;
    bool v9;
    bool v14;
    int64_t v20; /* placeholder for void */
    int64_t v4;
    int64_t v10; /* placeholder for void */
    int64_t v13;
    void* v7;
    int64_t v15; /* placeholder for void */
    bool v16;
    void* v11;
    int64_t v18; /* placeholder for void */
    int64_t v12;
    int64_t v6; /* placeholder for void */
    void* v2;
    bool v5;
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

int64_t String_trim(void* v0, int64_t v1) {
    p_call_stack_push("String_trim");
    int64_t v179; /* placeholder for void */
    int64_t v72;
    bool v119;
    int64_t v141; /* placeholder for void */
    bool v54;
    int64_t v126;
    int64_t v159; /* placeholder for void */
    int64_t v45;
    int64_t v148; /* placeholder for void */
    void* v28;
    int64_t v41;
    int64_t v109;
    int64_t v70; /* placeholder for void */
    int64_t v73;
    int64_t v94;
    void* v30;
    uint8_t v102;
    bool v111;
    int64_t v44;
    int64_t v18; /* placeholder for void */
    bool v74;
    int64_t v105; /* placeholder for void */
    int64_t v114;
    bool v115;
    int64_t v121; /* placeholder for void */
    int64_t v27;
    int64_t v40; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v137;
    int64_t v92;
    int64_t v138; /* placeholder for void */
    int64_t v149;
    int64_t v150;
    void* v10;
    int64_t v26; /* placeholder for void */
    bool v50;
    int64_t v61;
    void* v155;
    int64_t v144;
    int64_t v166; /* placeholder for void */
    int64_t v173; /* placeholder for void */
    int64_t v177;
    int64_t v153;
    int64_t v42;
    int64_t v182;
    int64_t v127;
    int64_t v14;
    bool v47;
    int64_t v152; /* placeholder for void */
    int64_t v190;
    int64_t v191; /* placeholder for void */
    int64_t v130; /* placeholder for void */
    int64_t v184; /* placeholder for void */
    int64_t v118;
    bool v108;
    int64_t v2;
    int64_t v133;
    bool v51;
    int64_t v163; /* placeholder for void */
    bool v43;
    void* v22;
    int64_t v48;
    int64_t v57;
    int64_t v16; /* placeholder for void */
    int64_t v21;
    int64_t v75; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v52;
    int64_t v86; /* placeholder for void */
    int64_t v156; /* placeholder for void */
    int64_t v125; /* placeholder for void */
    int64_t v97;
    void* v161;
    int64_t v164; /* placeholder for void */
    void* v154;
    uint8_t v68;
    int64_t v8; /* placeholder for void */
    int64_t v88;
    int64_t v167; /* placeholder for void */
    int64_t v185;
    void* v98;
    int64_t v89; /* placeholder for void */
    int64_t v178;
    int64_t v82; /* placeholder for void */
    int64_t v91;
    int64_t v62;
    void* v81;
    int64_t v34; /* placeholder for void */
    int64_t v188; /* placeholder for void */
    int64_t v24;
    void* v67;
    int64_t v129; /* placeholder for void */
    int64_t v187; /* placeholder for void */
    int64_t v131; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v63;
    int64_t v104; /* placeholder for void */
    int64_t v117;
    int64_t v160; /* placeholder for void */
    int64_t v135;
    int64_t v53;
    void* v95;
    int64_t v122;
    void* v78;
    void* v38;
    int64_t v79; /* placeholder for void */
    int64_t v174;
    void* v171;
    int64_t v65;
    int64_t v60; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    void* v6;
    int64_t v71; /* placeholder for void */
    void* v77;
    int64_t v193;
    int64_t v49;
    int64_t v64; /* placeholder for void */
    int64_t v142; /* placeholder for void */
    int64_t v146;
    int64_t v168;
    int64_t v113;
    void* v84;
    int64_t v59;
    int64_t v69; /* placeholder for void */
    int64_t v85;
    void* v25;
    void* v33;
    int64_t v134;
    bool v116;
    void* v36;
    int64_t v19; /* placeholder for void */
    bool v128;
    int64_t v192; /* placeholder for void */
    void* v172;
    bool v112;
    int64_t v181;
    void* v186;
    int64_t v31; /* placeholder for void */
    int64_t v143; /* placeholder for void */
    int64_t v9;
    void* v147;
    int64_t v99; /* placeholder for void */
    int64_t v165;
    int64_t v194; /* placeholder for void */
    bool v46;
    void* v7;
    int64_t v5;
    int64_t v11; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v80;
    int64_t v32;
    int64_t v106;
    void* v158;
    int64_t v107;
    bool v55;
    int64_t v66; /* placeholder for void */
    int64_t v170;
    int64_t v176;
    int64_t v76;
    bool v151;
    int64_t v162;
    int64_t v17;
    int64_t v169;
    bool v3;
    int64_t v35; /* placeholder for void */
    void* v189;
    int64_t v123;
    int64_t v132;
    int64_t v136; /* placeholder for void */
    int64_t v124;
    void* v183;
    int64_t v93;
    void* v139;
    int64_t v180;
    int64_t v96; /* placeholder for void */
    int64_t v58;
    int64_t v4; /* placeholder for void */
    uint8_t v37;
    void* v103;
    int64_t v175;
    int64_t v20;
    void* v101;
    void* v13;
    int64_t v145;
    int64_t v157;
    int64_t v90; /* placeholder for void */
    uint8_t v140;
    int64_t v100; /* placeholder for void */
    int64_t v110;
    int64_t v87; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    bool v120;
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

void* str_replace(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("str_replace");
    int64_t v4;
    int64_t v5;
    int64_t v6; /* placeholder for void */
    v4 = 0LL;
    v5 = (int64_t)String_replace((void*)v4, (int64_t)v1, (int64_t)v2, (int64_t)v3);
    p_call_stack_pop();
    return (void*)v5;
    return 0;
}

void Vec_sort(void* v0, void* v1) {
    p_call_stack_push("Vec_sort");
    int64_t v3; /* placeholder for void */
    int64_t v2; /* placeholder for void */
    p_vec_sort(v1);
    p_call_stack_pop();
    return;
    return;
}

void Vec_set(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("Vec_set");
    int64_t v4; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    p_vec_set(v1, v2, (void*)v3);
    p_call_stack_pop();
    return;
    return;
}

int64_t Vec_len(void* v0, void* v1) {
    p_call_stack_push("Vec_len");
    int64_t v2;
    int64_t v3; /* placeholder for void */
    v2 = (int64_t)p_vec_len(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void TraitMethod_new(void* v0, void* v1, void* v2, void* v3, bool v4) {
    p_call_stack_push("TraitMethod_new");
    int64_t v97;
    void* v60;
    void* v13;
    void* v12;
    void* v10;
    int64_t v54; /* placeholder for void */
    int64_t v63;
    int64_t v27;
    int64_t v64; /* placeholder for void */
    void* v65;
    int64_t v93;
    void* v17;
    int64_t v75;
    int64_t v7; /* placeholder for void */
    int64_t v8;
    int64_t v39;
    void* v56;
    int64_t v67;
    bool v87;
    int64_t v28; /* placeholder for void */
    void* v70;
    int64_t v23;
    void* v21;
    int64_t v16; /* placeholder for void */
    void* v25;
    void* v33;
    bool v35;
    void* v36;
    void* v51;
    int64_t v15;
    void* v53;
    int64_t v58; /* placeholder for void */
    void* v30;
    void* v69;
    void* v78;
    void* v81;
    int64_t v11; /* placeholder for void */
    void* v48;
    void* v5;
    int64_t v80; /* placeholder for void */
    int64_t v45;
    int64_t v50; /* placeholder for void */
    void* v86;
    int64_t v88; /* placeholder for void */
    void* v89;
    void* v90;
    int64_t v94; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v99; /* placeholder for void */
    void* v42;
    int64_t v71;
    void* v43;
    void* v62;
    int64_t v49;
    int64_t v68; /* placeholder for void */
    int64_t v19;
    int64_t v24; /* placeholder for void */
    void* v85;
    void* v29;
    void* v55;
    void* v74;
    void* v22;
    void* v61;
    int64_t v34; /* placeholder for void */
    void* v82;
    int64_t v76; /* placeholder for void */
    void* v96;
    void* v52;
    void* v59;
    void* v14;
    int64_t v72; /* placeholder for void */
    void* v47;
    void* v57;
    void* v73;
    int64_t v41; /* placeholder for void */
    void* v44;
    int64_t v46; /* placeholder for void */
    void* v77;
    int64_t v20; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    void* v18;
    void* v9;
    int64_t v79;
    void* v91;
    void* v95;
    void* v26;
    void* v6;
    void* v92;
    void* v38;
    void* v66;
    void* v40;
    bool v83;
    int64_t v32; /* placeholder for void */
    int64_t v31;
    v5 = malloc(88);
    v6 = (char*)v5 + 0;
    *(void**)v6 = (void*)v2;
    v8 = 0LL;
    v9 = Vec_new((void*)v8);
    v10 = (char*)v5 + 8;
    *(void**)v10 = (void*)v9;
    v12 = (char*)v5 + 16;
    v13 = (char*)v3 + 0;
    v14 = (char*)v12 + 0;
    v15 = *(int64_t*)v13;
    *(int64_t*)v14 = (int64_t)v15;
    v17 = (char*)v3 + 8;
    v18 = (char*)v12 + 8;
    v19 = *(int64_t*)v17;
    *(int64_t*)v18 = (int64_t)v19;
    v21 = (char*)v3 + 16;
    v22 = (char*)v12 + 16;
    v23 = *(int64_t*)v21;
    *(int64_t*)v22 = (int64_t)v23;
    v25 = (char*)v3 + 24;
    v26 = (char*)v12 + 24;
    v27 = *(int64_t*)v25;
    *(int64_t*)v26 = (int64_t)v27;
    v29 = (char*)v3 + 32;
    v30 = (char*)v12 + 32;
    v31 = *(int64_t*)v29;
    *(int64_t*)v30 = (int64_t)v31;
    v33 = (char*)v5 + 56;
    *(bool*)v33 = v4;
    v35 = true;
    v36 = (char*)v5 + 64;
    *(bool*)v36 = v35;
    v38 = malloc(16);
    v39 = 1LL;
    v40 = (char*)v38 + 0;
    *(int64_t*)v40 = (int64_t)v39;
    v42 = (char*)v5 + 72;
    v43 = (char*)v38 + 0;
    v44 = (char*)v42 + 0;
    v45 = *(int64_t*)v43;
    *(int64_t*)v44 = (int64_t)v45;
    v47 = (char*)v38 + 8;
    v48 = (char*)v42 + 8;
    v49 = *(int64_t*)v47;
    *(int64_t*)v48 = (int64_t)v49;
    v51 = (char*)v5 + 0;
    v52 = (char*)v0 + 0;
    v53 = *(void**)v51;
    *(void**)v52 = (void*)v53;
    v55 = (char*)v5 + 8;
    v56 = (char*)v0 + 8;
    v57 = *(void**)v55;
    *(void**)v56 = (void*)v57;
    v59 = (char*)v5 + 16;
    v60 = (char*)v0 + 16;
    v61 = (char*)v59 + 0;
    v62 = (char*)v60 + 0;
    v63 = *(int64_t*)v61;
    *(int64_t*)v62 = (int64_t)v63;
    v65 = (char*)v59 + 8;
    v66 = (char*)v60 + 8;
    v67 = *(int64_t*)v65;
    *(int64_t*)v66 = (int64_t)v67;
    v69 = (char*)v59 + 16;
    v70 = (char*)v60 + 16;
    v71 = *(int64_t*)v69;
    *(int64_t*)v70 = (int64_t)v71;
    v73 = (char*)v59 + 24;
    v74 = (char*)v60 + 24;
    v75 = *(int64_t*)v73;
    *(int64_t*)v74 = (int64_t)v75;
    v77 = (char*)v59 + 32;
    v78 = (char*)v60 + 32;
    v79 = *(int64_t*)v77;
    *(int64_t*)v78 = (int64_t)v79;
    v81 = (char*)v5 + 56;
    v82 = (char*)v0 + 56;
    v83 = *(bool*)v81;
    *(bool*)v82 = v83;
    v85 = (char*)v5 + 64;
    v86 = (char*)v0 + 64;
    v87 = *(bool*)v85;
    *(bool*)v86 = v87;
    v89 = (char*)v5 + 72;
    v90 = (char*)v0 + 72;
    v91 = (char*)v89 + 0;
    v92 = (char*)v90 + 0;
    v93 = *(int64_t*)v91;
    *(int64_t*)v92 = (int64_t)v93;
    v95 = (char*)v89 + 8;
    v96 = (char*)v90 + 8;
    v97 = *(int64_t*)v95;
    *(int64_t*)v96 = (int64_t)v97;
    p_call_stack_pop();
    return;
    return;
}

bool Option_is_none(void* v0, void* v1) {
    p_call_stack_push("Option_is_none");
    bool v5;
    void* v11;
    bool v9;
    int64_t v8;
    bool v14;
    int64_t v4;
    int64_t v18; /* placeholder for void */
    bool v19;
    bool v16;
    int64_t v15; /* placeholder for void */
    int64_t v13;
    void* v2;
    int64_t v17; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v12;
    int64_t v3;
    void* v7;
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

void Vec_sort_by(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_sort_by");
    int64_t v5; /* placeholder for void */
    int64_t v3;
    int64_t v4; /* placeholder for void */
    v3 = 0LL;
    vec_sort_by((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return;
    return;
}

void* box_type_to_str(void* v0, void* v1) {
    p_call_stack_push("box_type_to_str");
    void* v209;
    int64_t v405;
    int64_t v286; /* placeholder for void */
    void* v383;
    void* v409;
    void* v490;
    void* v258;
    void* v456;
    void* v513;
    void* v79;
    int64_t v29; /* placeholder for void */
    int64_t v493; /* placeholder for void */
    void* v149;
    void* v128;
    int64_t v362; /* placeholder for void */
    int64_t v203;
    int64_t v413; /* placeholder for void */
    void* v37;
    int64_t v422;
    int64_t v452;
    int64_t v254;
    void* v74;
    int64_t v366;
    void* v488;
    int64_t v246;
    int64_t v314;
    int64_t v334;
    void* v9;
    int64_t v154;
    void* v272;
    void* v494;
    void* v262;
    int64_t v415;
    bool v91;
    void* v447;
    void* v67;
    int64_t v141;
    void* v165;
    int64_t v214;
    bool v237;
    int64_t v201; /* placeholder for void */
    void* v437;
    int64_t v479;
    int64_t v455; /* placeholder for void */
    bool v49;
    void* v264;
    bool v482;
    void* v210;
    int64_t v311; /* placeholder for void */
    void* v317;
    void* v375;
    int64_t v498;
    void* v140;
    void* v432;
    int64_t v173;
    int64_t v440; /* placeholder for void */
    void* v53;
    void* v138;
    bool v179;
    int64_t v73; /* placeholder for void */
    int64_t v187;
    int64_t v228; /* placeholder for void */
    void* v429;
    void* v296;
    void* v321;
    bool v381;
    void* v18;
    void* v476;
    int64_t v483; /* placeholder for void */
    void* v88;
    int64_t v397; /* placeholder for void */
    int64_t v282; /* placeholder for void */
    void* v360;
    void* v371;
    int64_t v75;
    int64_t v90;
    int64_t v302;
    int64_t v345;
    int64_t v236;
    int64_t v424;
    int64_t v238; /* placeholder for void */
    int64_t v48;
    void* v93;
    int64_t v378;
    int64_t v394; /* placeholder for void */
    int64_t v118;
    int64_t v125;
    bool v454;
    void* v463;
    void* v339;
    void* v39;
    void* v188;
    int64_t v8; /* placeholder for void */
    int64_t v478; /* placeholder for void */
    void* v169;
    void* v102;
    void* v116;
    bool v155;
    void* v385;
    int64_t v439; /* placeholder for void */
    int64_t v229; /* placeholder for void */
    int64_t v404;
    int64_t v278;
    void* v218;
    int64_t v17; /* placeholder for void */
    void* v387;
    int64_t v367;
    void* v293;
    bool v28;
    void* v417;
    void* v46;
    int64_t v132;
    int64_t v20;
    int64_t v38; /* placeholder for void */
    bool v331;
    void* v438;
    int64_t v6;
    void* v222;
    int64_t v354; /* placeholder for void */
    int64_t v276; /* placeholder for void */
    void* v72;
    void* v393;
    int64_t v106; /* placeholder for void */
    void* v514;
    int64_t v82;
    void* v269;
    void* v123;
    void* v389;
    void* v298;
    void* v271;
    int64_t v512; /* placeholder for void */
    bool v469;
    void* v517;
    void* v81;
    void* v259;
    void* v219;
    int64_t v291; /* placeholder for void */
    bool v42;
    void* v240;
    int64_t v249;
    bool v335;
    int64_t v336; /* placeholder for void */
    int64_t v510; /* placeholder for void */
    bool v119;
    void* v428;
    void* v419;
    int64_t v101; /* placeholder for void */
    int64_t v485;
    void* v361;
    void* v471;
    void* v436;
    void* v176;
    void* v446;
    void* v168;
    int64_t v54;
    int64_t v110;
    int64_t v466;
    void* v121;
    bool v77;
    void* v202;
    void* v396;
    int64_t v41;
    bool v200;
    int64_t v344;
    int64_t v31; /* placeholder for void */
    bool v84;
    bool v21;
    int64_t v87; /* placeholder for void */
    int64_t v124;
    int64_t v5;
    int64_t v127; /* placeholder for void */
    int64_t v277; /* placeholder for void */
    int64_t v304; /* placeholder for void */
    void* v51;
    int64_t v355; /* placeholder for void */
    int64_t v443;
    int64_t v379;
    void* v497;
    int64_t v301;
    void* v451;
    int64_t v511; /* placeholder for void */
    int64_t v329;
    void* v365;
    int64_t v400;
    int64_t v50; /* placeholder for void */
    int64_t v117;
    void* v160;
    void* v337;
    int64_t v172;
    int64_t v292; /* placeholder for void */
    void* v241;
    int64_t v309; /* placeholder for void */
    int64_t v342; /* placeholder for void */
    void* v163;
    int64_t v480;
    int64_t v96;
    int64_t v324;
    int64_t v43; /* placeholder for void */
    int64_t v243; /* placeholder for void */
    void* v343;
    void* v430;
    void* v433;
    int64_t v52; /* placeholder for void */
    void* v161;
    void* v325;
    int64_t v268; /* placeholder for void */
    int64_t v33;
    void* v299;
    int64_t v162;
    void* v515;
    void* v457;
    void* v346;
    void* v408;
    int64_t v516; /* placeholder for void */
    void* v459;
    int64_t v496;
    void* v520;
    int64_t v450; /* placeholder for void */
    int64_t v257; /* placeholder for void */
    int64_t v491; /* placeholder for void */
    int64_t v399;
    void* v136;
    int64_t v231;
    int64_t v230;
    bool v281;
    void* v147;
    void* v338;
    int64_t v418;
    int64_t v519; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    void* v157;
    int64_t v61;
    void* v166;
    void* v250;
    int64_t v395;
    void* v137;
    int64_t v340; /* placeholder for void */
    int64_t v180; /* placeholder for void */
    int64_t v232;
    void* v270;
    int64_t v64; /* placeholder for void */
    int64_t v247;
    void* v186;
    int64_t v89;
    void* v183;
    int64_t v13;
    void* v356;
    int64_t v323; /* placeholder for void */
    int64_t v475;
    int64_t v68;
    void* v32;
    int64_t v265;
    int64_t v226;
    int64_t v518; /* placeholder for void */
    int64_t v80; /* placeholder for void */
    int64_t v193;
    int64_t v411; /* placeholder for void */
    int64_t v190; /* placeholder for void */
    void* v462;
    void* v23;
    int64_t v47;
    int64_t v76;
    int64_t v261; /* placeholder for void */
    void* v287;
    int64_t v294;
    void* v107;
    bool v368;
    int64_t v401;
    void* v414;
    int64_t v461;
    void* v473;
    int64_t v170; /* placeholder for void */
    int64_t v477; /* placeholder for void */
    void* v509;
    void* v363;
    int64_t v353;
    int64_t v194;
    int64_t v57; /* placeholder for void */
    int64_t v474; /* placeholder for void */
    void* v205;
    int64_t v92; /* placeholder for void */
    void* v11;
    int64_t v108; /* placeholder for void */
    int64_t v279;
    int64_t v501; /* placeholder for void */
    void* v239;
    int64_t v97;
    int64_t v104;
    void* v142;
    void* v260;
    int64_t v273; /* placeholder for void */
    int64_t v521; /* placeholder for void */
    void* v184;
    int64_t v211; /* placeholder for void */
    void* v357;
    bool v14;
    int64_t v484;
    void* v390;
    void* v204;
    int64_t v425; /* placeholder for void */
    int64_t v235;
    int64_t v300; /* placeholder for void */
    void* v60;
    void* v499;
    void* v197;
    void* v131;
    int64_t v253;
    int64_t v391; /* placeholder for void */
    int64_t v182;
    int64_t v66; /* placeholder for void */
    int64_t v196; /* placeholder for void */
    void* v198;
    int64_t v333;
    void* v289;
    void* v251;
    int64_t v256; /* placeholder for void */
    int64_t v407; /* placeholder for void */
    void* v164;
    void* v318;
    void* v384;
    void* v189;
    int64_t v467;
    int64_t v316; /* placeholder for void */
    int64_t v369; /* placeholder for void */
    bool v126;
    int64_t v185; /* placeholder for void */
    bool v402;
    int64_t v445; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    void* v500;
    int64_t v130; /* placeholder for void */
    int64_t v225; /* placeholder for void */
    int64_t v113; /* placeholder for void */
    int64_t v133;
    void* v145;
    void* v348;
    void* v458;
    void* v3;
    bool v7;
    int64_t v208;
    void* v464;
    int64_t v487; /* placeholder for void */
    void* v25;
    int64_t v305; /* placeholder for void */
    int64_t v62;
    void* v4;
    int64_t v27;
    void* v245;
    bool v70;
    int64_t v330;
    int64_t v24; /* placeholder for void */
    bool v315;
    void* v2;
    int64_t v83;
    bool v406;
    bool v35;
    bool v112;
    int64_t v111;
    int64_t v36; /* placeholder for void */
    void* v158;
    void* v150;
    void* v143;
    int64_t v175; /* placeholder for void */
    int64_t v212; /* placeholder for void */
    int64_t v364; /* placeholder for void */
    void* v370;
    void* v431;
    void* v507;
    int64_t v503;
    void* v322;
    void* v388;
    void* v129;
    int64_t v55;
    int64_t v252; /* placeholder for void */
    void* v349;
    void* v248;
    void* v224;
    int64_t v206; /* placeholder for void */
    void* v148;
    void* v181;
    void* v319;
    int64_t v412; /* placeholder for void */
    int64_t v244; /* placeholder for void */
    bool v285;
    void* v312;
    int64_t v215;
    void* v307;
    bool v216;
    int64_t v505; /* placeholder for void */
    int64_t v34;
    void* v220;
    int64_t v22; /* placeholder for void */
    void* v100;
    void* v227;
    int64_t v266;
    void* v171;
    void* v207;
    int64_t v453;
    void* v86;
    void* v288;
    void* v320;
    int64_t v352;
    int64_t v416;
    void* v420;
    int64_t v115; /* placeholder for void */
    int64_t v465; /* placeholder for void */
    int64_t v26;
    void* v427;
    int64_t v191; /* placeholder for void */
    int64_t v327; /* placeholder for void */
    int64_t v69;
    void* v373;
    bool v105;
    bool v486;
    int64_t v103;
    int64_t v303;
    int64_t v423;
    bool v233;
    int64_t v19;
    int64_t v255;
    int64_t v242; /* placeholder for void */
    void* v178;
    void* v460;
    void* v223;
    void* v308;
    int64_t v85; /* placeholder for void */
    int64_t v120; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    bool v98;
    bool v195;
    void* v221;
    void* v65;
    void* v410;
    void* v44;
    void* v386;
    int64_t v481;
    int64_t v492; /* placeholder for void */
    int64_t v328;
    void* v508;
    int64_t v153;
    bool v444;
    int64_t v468;
    int64_t v470; /* placeholder for void */
    int64_t v332; /* placeholder for void */
    int64_t v284;
    void* v448;
    void* v376;
    void* v372;
    void* v213;
    bool v174;
    void* v144;
    int64_t v297;
    bool v63;
    void* v377;
    void* v58;
    void* v167;
    void* v449;
    void* v199;
    int64_t v290; /* placeholder for void */
    void* v441;
    int64_t v502;
    void* v30;
    void* v374;
    int64_t v398; /* placeholder for void */
    int64_t v403; /* placeholder for void */
    void* v177;
    void* v192;
    int64_t v12;
    int64_t v217; /* placeholder for void */
    int64_t v504;
    int64_t v94; /* placeholder for void */
    bool v134;
    int64_t v139;
    int64_t v146;
    void* v392;
    int64_t v280;
    void* v435;
    int64_t v59; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v135; /* placeholder for void */
    int64_t v326; /* placeholder for void */
    int64_t v263; /* placeholder for void */
    int64_t v351;
    int64_t v40;
    int64_t v495;
    int64_t v283;
    int64_t v151; /* placeholder for void */
    int64_t v380;
    int64_t v45; /* placeholder for void */
    void* v95;
    int64_t v274;
    int64_t v426; /* placeholder for void */
    int64_t v341; /* placeholder for void */
    int64_t v234; /* placeholder for void */
    void* v472;
    void* v109;
    int64_t v156; /* placeholder for void */
    int64_t v347;
    void* v16;
    int64_t v359;
    void* v489;
    int64_t v122; /* placeholder for void */
    void* v306;
    void* v114;
    bool v56;
    int64_t v506; /* placeholder for void */
    void* v310;
    void* v434;
    int64_t v99; /* placeholder for void */
    void* v159;
    int64_t v295;
    int64_t v442;
    bool v267;
    int64_t v313;
    void* v152;
    int64_t v421; /* placeholder for void */
    void* v275;
    void* v358;
    int64_t v350; /* placeholder for void */
    int64_t v382; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = *(void**)v2;
    v4 = (char*)v3 + 0;
    v5 = *(int64_t*)v4;
    v6 = 0LL;
    v7 = v5 == v6;
    if (v7) goto bb2;
    else goto bb3;
  bb1:
    v520 = 0LL;
    p_call_stack_pop();
    return v520;
  bb2:
    v9 = _str20;
    p_call_stack_pop();
    return v9;
  bb3:
    v11 = (char*)v3 + 0;
    v12 = *(int64_t*)v11;
    v13 = 1LL;
    v14 = v12 == v13;
    if (v14) goto bb4;
    else goto bb5;
  bb4:
    v16 = _str22;
    p_call_stack_pop();
    return v16;
  bb5:
    v18 = (char*)v3 + 0;
    v19 = *(int64_t*)v18;
    v20 = 2LL;
    v21 = v19 == v20;
    if (v21) goto bb6;
    else goto bb7;
  bb6:
    v23 = _str24;
    p_call_stack_pop();
    return v23;
  bb7:
    v25 = (char*)v3 + 0;
    v26 = *(int64_t*)v25;
    v27 = 3LL;
    v28 = v26 == v27;
    if (v28) goto bb8;
    else goto bb9;
  bb8:
    v30 = _str26;
    p_call_stack_pop();
    return v30;
  bb9:
    v32 = (char*)v3 + 0;
    v33 = *(int64_t*)v32;
    v34 = 4LL;
    v35 = v33 == v34;
    if (v35) goto bb10;
    else goto bb11;
  bb10:
    v37 = _str28;
    p_call_stack_pop();
    return v37;
  bb11:
    v39 = (char*)v3 + 0;
    v40 = *(int64_t*)v39;
    v41 = 5LL;
    v42 = v40 == v41;
    if (v42) goto bb12;
    else goto bb13;
  bb12:
    v44 = _str30;
    p_call_stack_pop();
    return v44;
  bb13:
    v46 = (char*)v3 + 0;
    v47 = *(int64_t*)v46;
    v48 = 6LL;
    v49 = v47 == v48;
    if (v49) goto bb14;
    else goto bb15;
  bb14:
    v51 = _str32;
    p_call_stack_pop();
    return v51;
  bb15:
    v53 = (char*)v3 + 0;
    v54 = *(int64_t*)v53;
    v55 = 7LL;
    v56 = v54 == v55;
    if (v56) goto bb16;
    else goto bb17;
  bb16:
    v58 = _str34;
    p_call_stack_pop();
    return v58;
  bb17:
    v60 = (char*)v3 + 0;
    v61 = *(int64_t*)v60;
    v62 = 8LL;
    v63 = v61 == v62;
    if (v63) goto bb18;
    else goto bb19;
  bb18:
    v65 = _str36;
    p_call_stack_pop();
    return v65;
  bb19:
    v67 = (char*)v3 + 0;
    v68 = *(int64_t*)v67;
    v69 = 9LL;
    v70 = v68 == v69;
    if (v70) goto bb20;
    else goto bb21;
  bb20:
    v72 = _str38;
    p_call_stack_pop();
    return v72;
  bb21:
    v74 = (char*)v3 + 0;
    v75 = *(int64_t*)v74;
    v76 = 10LL;
    v77 = v75 == v76;
    if (v77) goto bb22;
    else goto bb23;
  bb22:
    v79 = _str40;
    p_call_stack_pop();
    return v79;
  bb23:
    v81 = (char*)v3 + 0;
    v82 = *(int64_t*)v81;
    v83 = 11LL;
    v84 = v82 == v83;
    if (v84) goto bb24;
    else goto bb25;
  bb24:
    v86 = _str42;
    p_call_stack_pop();
    return v86;
  bb25:
    v88 = (char*)v3 + 0;
    v89 = *(int64_t*)v88;
    v90 = 12LL;
    v91 = v89 == v90;
    if (v91) goto bb26;
    else goto bb27;
  bb26:
    v93 = _str44;
    p_call_stack_pop();
    return v93;
  bb27:
    v95 = (char*)v3 + 0;
    v96 = *(int64_t*)v95;
    v97 = 13LL;
    v98 = v96 == v97;
    if (v98) goto bb28;
    else goto bb29;
  bb28:
    v100 = _str46;
    p_call_stack_pop();
    return v100;
  bb29:
    v102 = (char*)v3 + 0;
    v103 = *(int64_t*)v102;
    v104 = 14LL;
    v105 = v103 == v104;
    if (v105) goto bb30;
    else goto bb31;
  bb30:
    v107 = _str48;
    p_call_stack_pop();
    return v107;
  bb31:
    v109 = (char*)v3 + 0;
    v110 = *(int64_t*)v109;
    v111 = 15LL;
    v112 = v110 == v111;
    if (v112) goto bb32;
    else goto bb33;
  bb32:
    v114 = _str50;
    p_call_stack_pop();
    return v114;
  bb33:
    v116 = (char*)v3 + 0;
    v117 = *(int64_t*)v116;
    v118 = 16LL;
    v119 = v117 == v118;
    if (v119) goto bb34;
    else goto bb35;
  bb34:
    v121 = _str52;
    p_call_stack_pop();
    return v121;
  bb35:
    v123 = (char*)v3 + 0;
    v124 = *(int64_t*)v123;
    v125 = 17LL;
    v126 = v124 == v125;
    if (v126) goto bb36;
    else goto bb37;
  bb36:
    v128 = (char*)v3 + 8;
    v129 = *(void**)v128;
    p_call_stack_pop();
    return v129;
  bb37:
    v131 = (char*)v3 + 0;
    v132 = *(int64_t*)v131;
    v133 = 18LL;
    v134 = v132 == v133;
    if (v134) goto bb38;
    else goto bb39;
  bb38:
    v136 = (char*)v3 + 8;
    v137 = *(void**)v136;
    v138 = (char*)v3 + 16;
    v139 = *(int64_t*)v138;
    v140 = _str395;
    v141 = 0LL;
    v142 = box_type_to_str((void*)v141, v137);
    v143 = p_str_concat(v140, v142);
    v144 = _str400;
    v145 = p_str_concat(v143, v144);
    v146 = 0LL;
    v147 = int_to_str((void*)v146, v139);
    v148 = p_str_concat(v145, v147);
    v149 = _str396;
    v150 = p_str_concat(v148, v149);
    p_call_stack_pop();
    return v150;
  bb39:
    v152 = (char*)v3 + 0;
    v153 = *(int64_t*)v152;
    v154 = 19LL;
    v155 = v153 == v154;
    if (v155) goto bb40;
    else goto bb41;
  bb40:
    v157 = (char*)v3 + 8;
    v158 = *(void**)v157;
    v159 = (char*)v3 + 16;
    v160 = *(void**)v159;
    v161 = _str395;
    v162 = 0LL;
    v163 = box_type_to_str((void*)v162, v158);
    v164 = p_str_concat(v161, v163);
    v165 = _str400;
    v166 = p_str_concat(v164, v165);
    v167 = p_str_concat(v166, v160);
    v168 = _str396;
    v169 = p_str_concat(v167, v168);
    p_call_stack_pop();
    return v169;
  bb41:
    v171 = (char*)v3 + 0;
    v172 = *(int64_t*)v171;
    v173 = 20LL;
    v174 = v172 == v173;
    if (v174) goto bb42;
    else goto bb43;
  bb42:
    v176 = (char*)v3 + 8;
    v177 = *(void**)v176;
    v178 = (char*)v3 + 16;
    v179 = *(bool*)v178;
    if (v179) goto bb44;
    else goto bb45;
  bb43:
    v192 = (char*)v3 + 0;
    v193 = *(int64_t*)v192;
    v194 = 21LL;
    v195 = v193 == v194;
    if (v195) goto bb47;
    else goto bb48;
  bb44:
    v181 = _str401;
    v182 = 0LL;
    v183 = box_type_to_str((void*)v182, v177);
    v184 = p_str_concat(v181, v183);
    p_call_stack_pop();
    return v184;
  bb45:
    v186 = _str402;
    v187 = 0LL;
    v188 = box_type_to_str((void*)v187, v177);
    v189 = p_str_concat(v186, v188);
    p_call_stack_pop();
    return v189;
  bb46:
    goto bb1;
  bb47:
    v197 = (char*)v3 + 8;
    v198 = *(void**)v197;
    v199 = (char*)v3 + 16;
    v200 = *(bool*)v199;
    if (v200) goto bb49;
    else goto bb50;
  bb48:
    v213 = (char*)v3 + 0;
    v214 = *(int64_t*)v213;
    v215 = 22LL;
    v216 = v214 == v215;
    if (v216) goto bb52;
    else goto bb53;
  bb49:
    v202 = _str403;
    v203 = 0LL;
    v204 = box_type_to_str((void*)v203, v198);
    v205 = p_str_concat(v202, v204);
    p_call_stack_pop();
    return v205;
  bb50:
    v207 = _str404;
    v208 = 0LL;
    v209 = box_type_to_str((void*)v208, v198);
    v210 = p_str_concat(v207, v209);
    p_call_stack_pop();
    return v210;
  bb51:
    goto bb1;
  bb52:
    v218 = (char*)v3 + 8;
    v219 = *(void**)v218;
    v220 = (char*)v3 + 16;
    v221 = *(void**)v220;
    v222 = _str405;
    v223 = p_str_concat(v219, v222);
    v224 = malloc(8);
    *(void**)v224 = (void*)v223;
    v226 = 0LL;
    v227 = malloc(8);
    *(int64_t*)v227 = (int64_t)v226;
    goto bb54;
  bb53:
    v264 = (char*)v3 + 0;
    v265 = *(int64_t*)v264;
    v266 = 23LL;
    v267 = v265 == v266;
    if (v267) goto bb60;
    else goto bb61;
  bb54:
    v230 = *(int64_t*)v227;
    v231 = 0LL;
    v232 = (int64_t)Vec_len((void*)v231, v221);
    v233 = v230 < v232;
    if (v233) goto bb55;
    else goto bb56;
  bb55:
    v235 = *(int64_t*)v227;
    v236 = 0LL;
    v237 = v235 > v236;
    if (v237) goto bb57;
    else goto bb58;
  bb56:
    v258 = *(void**)v224;
    v259 = _str406;
    v260 = p_str_concat(v258, v259);
    *(void**)v224 = (void*)v260;
    v262 = *(void**)v224;
    p_call_stack_pop();
    return v262;
  bb57:
    v239 = *(void**)v224;
    v240 = _str13;
    v241 = p_str_concat(v239, v240);
    *(void**)v224 = (void*)v241;
    goto bb59;
  bb58:
    goto bb59;
  bb59:
    v245 = *(void**)v224;
    v246 = *(int64_t*)v227;
    v247 = 0LL;
    v248 = (void*)Vec_get((void*)v247, v221, v246);
    v249 = 0LL;
    v250 = type_to_str((void*)v249, v248);
    v251 = p_str_concat(v245, v250);
    *(void**)v224 = (void*)v251;
    v253 = *(int64_t*)v227;
    v254 = 1LL;
    v255 = v253 + v254;
    *(int64_t*)v227 = (int64_t)v255;
    goto bb54;
  bb60:
    v269 = (char*)v3 + 8;
    v270 = *(void**)v269;
    v271 = _str407;
    v272 = malloc(8);
    *(void**)v272 = (void*)v271;
    v274 = 0LL;
    v275 = malloc(8);
    *(int64_t*)v275 = (int64_t)v274;
    goto bb62;
  bb61:
    v312 = (char*)v3 + 0;
    v313 = *(int64_t*)v312;
    v314 = 24LL;
    v315 = v313 == v314;
    if (v315) goto bb68;
    else goto bb69;
  bb62:
    v278 = *(int64_t*)v275;
    v279 = 0LL;
    v280 = (int64_t)Vec_len((void*)v279, v270);
    v281 = v278 < v280;
    if (v281) goto bb63;
    else goto bb64;
  bb63:
    v283 = *(int64_t*)v275;
    v284 = 0LL;
    v285 = v283 > v284;
    if (v285) goto bb65;
    else goto bb66;
  bb64:
    v306 = *(void**)v272;
    v307 = _str408;
    v308 = p_str_concat(v306, v307);
    *(void**)v272 = (void*)v308;
    v310 = *(void**)v272;
    p_call_stack_pop();
    return v310;
  bb65:
    v287 = *(void**)v272;
    v288 = _str13;
    v289 = p_str_concat(v287, v288);
    *(void**)v272 = (void*)v289;
    goto bb67;
  bb66:
    goto bb67;
  bb67:
    v293 = *(void**)v272;
    v294 = *(int64_t*)v275;
    v295 = 0LL;
    v296 = (void*)Vec_get((void*)v295, v270, v294);
    v297 = 0LL;
    v298 = type_to_str((void*)v297, v296);
    v299 = p_str_concat(v293, v298);
    *(void**)v272 = (void*)v299;
    v301 = *(int64_t*)v275;
    v302 = 1LL;
    v303 = v301 + v302;
    *(int64_t*)v275 = (int64_t)v303;
    goto bb62;
  bb68:
    v317 = (char*)v3 + 8;
    v318 = *(void**)v317;
    v319 = (char*)v3 + 16;
    v320 = *(void**)v319;
    v321 = _str409;
    v322 = malloc(8);
    *(void**)v322 = (void*)v321;
    v324 = 0LL;
    v325 = malloc(8);
    *(int64_t*)v325 = (int64_t)v324;
    goto bb70;
  bb69:
    v365 = (char*)v3 + 0;
    v366 = *(int64_t*)v365;
    v367 = 25LL;
    v368 = v366 == v367;
    if (v368) goto bb76;
    else goto bb77;
  bb70:
    v328 = *(int64_t*)v325;
    v329 = 0LL;
    v330 = (int64_t)Vec_len((void*)v329, v318);
    v331 = v328 < v330;
    if (v331) goto bb71;
    else goto bb72;
  bb71:
    v333 = *(int64_t*)v325;
    v334 = 0LL;
    v335 = v333 > v334;
    if (v335) goto bb73;
    else goto bb74;
  bb72:
    v356 = *(void**)v322;
    v357 = _str410;
    v358 = p_str_concat(v356, v357);
    v359 = 0LL;
    v360 = box_type_to_str((void*)v359, v320);
    v361 = p_str_concat(v358, v360);
    *(void**)v322 = (void*)v361;
    v363 = *(void**)v322;
    p_call_stack_pop();
    return v363;
  bb73:
    v337 = *(void**)v322;
    v338 = _str13;
    v339 = p_str_concat(v337, v338);
    *(void**)v322 = (void*)v339;
    goto bb75;
  bb74:
    goto bb75;
  bb75:
    v343 = *(void**)v322;
    v344 = *(int64_t*)v325;
    v345 = 0LL;
    v346 = (void*)Vec_get((void*)v345, v318, v344);
    v347 = 0LL;
    v348 = type_to_str((void*)v347, v346);
    v349 = p_str_concat(v343, v348);
    *(void**)v322 = (void*)v349;
    v351 = *(int64_t*)v325;
    v352 = 1LL;
    v353 = v351 + v352;
    *(int64_t*)v325 = (int64_t)v353;
    goto bb70;
  bb76:
    v370 = (char*)v3 + 8;
    v371 = *(void**)v370;
    v372 = (char*)v3 + 16;
    v373 = *(void**)v372;
    v374 = (char*)v3 + 24;
    v375 = *(void**)v374;
    v376 = (char*)v3 + 32;
    v377 = *(void**)v376;
    v378 = 0LL;
    v379 = (int64_t)Vec_len((void*)v378, v377);
    v380 = 0LL;
    v381 = v379 == v380;
    if (v381) goto bb78;
    else goto bb79;
  bb77:
    v441 = (char*)v3 + 0;
    v442 = *(int64_t*)v441;
    v443 = 26LL;
    v444 = v442 == v443;
    if (v444) goto bb87;
    else goto bb88;
  bb78:
    v383 = _str411;
    v384 = p_str_concat(v371, v383);
    v385 = p_str_concat(v384, v375);
    v386 = _str407;
    v387 = p_str_concat(v385, v386);
    v388 = p_str_concat(v387, v373);
    v389 = _str408;
    v390 = p_str_concat(v388, v389);
    p_call_stack_pop();
    return v390;
  bb79:
    v392 = _str5;
    v393 = malloc(8);
    *(void**)v393 = (void*)v392;
    v395 = 0LL;
    v396 = malloc(8);
    *(int64_t*)v396 = (int64_t)v395;
    goto bb81;
  bb80:
    goto bb1;
  bb81:
    v399 = *(int64_t*)v396;
    v400 = 0LL;
    v401 = (int64_t)Vec_len((void*)v400, v377);
    v402 = v399 < v401;
    if (v402) goto bb82;
    else goto bb83;
  bb82:
    v404 = *(int64_t*)v396;
    v405 = 0LL;
    v406 = v404 > v405;
    if (v406) goto bb84;
    else goto bb85;
  bb83:
    v427 = _str411;
    v428 = p_str_concat(v371, v427);
    v429 = p_str_concat(v428, v375);
    v430 = _str405;
    v431 = p_str_concat(v429, v430);
    v432 = *(void**)v393;
    v433 = p_str_concat(v431, v432);
    v434 = _str412;
    v435 = p_str_concat(v433, v434);
    v436 = p_str_concat(v435, v373);
    v437 = _str408;
    v438 = p_str_concat(v436, v437);
    p_call_stack_pop();
    return v438;
  bb84:
    v408 = *(void**)v393;
    v409 = _str13;
    v410 = p_str_concat(v408, v409);
    *(void**)v393 = (void*)v410;
    goto bb86;
  bb85:
    goto bb86;
  bb86:
    v414 = *(void**)v393;
    v415 = *(int64_t*)v396;
    v416 = 0LL;
    v417 = (void*)Vec_get((void*)v416, v377, v415);
    v418 = 0LL;
    v419 = type_to_str((void*)v418, v417);
    v420 = p_str_concat(v414, v419);
    *(void**)v393 = (void*)v420;
    v422 = *(int64_t*)v396;
    v423 = 1LL;
    v424 = v422 + v423;
    *(int64_t*)v396 = (int64_t)v424;
    goto bb81;
  bb87:
    v446 = (char*)v3 + 8;
    v447 = *(void**)v446;
    v448 = _str413;
    v449 = p_str_concat(v448, v447);
    p_call_stack_pop();
    return v449;
  bb88:
    v451 = (char*)v3 + 0;
    v452 = *(int64_t*)v451;
    v453 = 27LL;
    v454 = v452 == v453;
    if (v454) goto bb89;
    else goto bb90;
  bb89:
    v456 = (char*)v3 + 8;
    v457 = *(void**)v456;
    v458 = (char*)v3 + 16;
    v459 = *(void**)v458;
    v460 = _str414;
    v461 = 0LL;
    v462 = box_type_to_str((void*)v461, v457);
    v463 = p_str_concat(v460, v462);
    v464 = malloc(8);
    *(void**)v464 = (void*)v463;
    v466 = 0LL;
    v467 = (int64_t)Vec_len((void*)v466, v459);
    v468 = 0LL;
    v469 = v467 > v468;
    if (v469) goto bb91;
    else goto bb92;
  bb90:
    goto bb1;
  bb91:
    v471 = *(void**)v464;
    v472 = _str415;
    v473 = p_str_concat(v471, v472);
    *(void**)v464 = (void*)v473;
    v475 = 0LL;
    v476 = malloc(8);
    *(int64_t*)v476 = (int64_t)v475;
    goto bb94;
  bb92:
    goto bb93;
  bb93:
    v513 = *(void**)v464;
    v514 = _str406;
    v515 = p_str_concat(v513, v514);
    *(void**)v464 = (void*)v515;
    v517 = *(void**)v464;
    p_call_stack_pop();
    return v517;
  bb94:
    v479 = *(int64_t*)v476;
    v480 = 0LL;
    v481 = (int64_t)Vec_len((void*)v480, v459);
    v482 = v479 < v481;
    if (v482) goto bb95;
    else goto bb96;
  bb95:
    v484 = *(int64_t*)v476;
    v485 = 0LL;
    v486 = v484 > v485;
    if (v486) goto bb97;
    else goto bb98;
  bb96:
    v507 = *(void**)v464;
    v508 = _str396;
    v509 = p_str_concat(v507, v508);
    *(void**)v464 = (void*)v509;
    goto bb93;
  bb97:
    v488 = *(void**)v464;
    v489 = _str13;
    v490 = p_str_concat(v488, v489);
    *(void**)v464 = (void*)v490;
    goto bb99;
  bb98:
    goto bb99;
  bb99:
    v494 = *(void**)v464;
    v495 = *(int64_t*)v476;
    v496 = 0LL;
    v497 = (void*)Vec_get((void*)v496, v459, v495);
    v498 = 0LL;
    v499 = shape_dim_to_str((void*)v498, v497);
    v500 = p_str_concat(v494, v499);
    *(void**)v464 = (void*)v500;
    v502 = *(int64_t*)v476;
    v503 = 1LL;
    v504 = v502 + v503;
    *(int64_t*)v476 = (int64_t)v504;
    goto bb94;
    return 0;
}

bool String_contains(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_contains");
    bool v100;
    int64_t v61;
    bool v98;
    int64_t v88; /* placeholder for void */
    int64_t v95;
    int64_t v101; /* placeholder for void */
    int64_t v105;
    int64_t v64; /* placeholder for void */
    bool v53;
    int64_t v90; /* placeholder for void */
    int64_t v71;
    bool v22;
    int64_t v11; /* placeholder for void */
    bool v7;
    int64_t v32; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v41;
    int64_t v49; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    bool v42;
    int64_t v77; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    int64_t v21;
    void* v69;
    int64_t v38;
    int64_t v16;
    uint8_t v82;
    int64_t v102; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    bool v24;
    int64_t v40;
    int64_t v74;
    int64_t v84; /* placeholder for void */
    int64_t v85;
    int64_t v34;
    int64_t v19; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v52;
    int64_t v60; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v80; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v5;
    int64_t v55;
    bool v4;
    int64_t v12;
    void* v78;
    int64_t v86;
    int64_t v97; /* placeholder for void */
    void* v66;
    int64_t v106; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v3;
    int64_t v51;
    int64_t v23; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    void* v81;
    int64_t v15; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v20;
    int64_t v47;
    void* v72;
    int64_t v39;
    void* v63;
    int64_t v107; /* placeholder for void */
    int64_t v109; /* placeholder for void */
    bool v108;
    int64_t v93;
    bool v6;
    int64_t v65;
    uint8_t v79;
    int64_t v25; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    int64_t v62;
    int64_t v104;
    int64_t v13;
    int64_t v57;
    void* v48;
    int64_t v68;
    int64_t v50; /* placeholder for void */
    void* v45;
    void* v59;
    int64_t v76; /* placeholder for void */
    int64_t v103;
    int64_t v8; /* placeholder for void */
    int64_t v56;
    bool v89;
    int64_t v28;
    void* v14;
    void* v18;
    void* v75;
    int64_t v99; /* placeholder for void */
    int64_t v17;
    bool v31;
    int64_t v58;
    bool v44;
    bool v87;
    int64_t v91; /* placeholder for void */
    bool v9;
    int64_t v94;
    int64_t v27;
    bool v29;
    void* v35;
    int64_t v73; /* placeholder for void */
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

int64_t allocator_bytes_used(void* v0, void* v1) {
    p_call_stack_push("allocator_bytes_used");
    int64_t v3; /* placeholder for void */
    int64_t v2;
    v2 = (int64_t)p_allocator_stats(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void* Vec_window(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_window");
    int64_t v3;
    int64_t v4;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)vec_window((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return (void*)v4;
    return 0;
}

void* Vec_enumerate(void* v0, void* v1) {
    p_call_stack_push("Vec_enumerate");
    int64_t v2;
    int64_t v3;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)vec_enumerate((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return (void*)v3;
    return 0;
}

// ─── C entry point ───
int main(int argc, char** argv) {
    __p_argc = argc; __p_argv = argv;
    // Register functions for p_thread_spawn lookup
    p_thread_register("String_char_count", (int64_t(*)())String_char_count);
    p_thread_register("assert_eq", (int64_t(*)())assert_eq);
    p_thread_register("String_find", (int64_t(*)())String_find);
    p_thread_register("String_split", (int64_t(*)())String_split);
    p_thread_register("float_to_str", (int64_t(*)())float_to_str);
    p_thread_register("Vec_any", (int64_t(*)())Vec_any);
    p_thread_register("ByteBuffer_write_bytes", (int64_t(*)())ByteBuffer_write_bytes);
    p_thread_register("Vec_reverse", (int64_t(*)())Vec_reverse);
    p_thread_register("Vec_clone", (int64_t(*)())Vec_clone);
    p_thread_register("Result_expect", (int64_t(*)())Result_expect);
    p_thread_register("max", (int64_t(*)())p_max);
    p_thread_register("vec_dedup", (int64_t(*)())vec_dedup);
    p_thread_register("Vec_fold", (int64_t(*)())Vec_fold);
    p_thread_register("str_title", (int64_t(*)())str_title);
    p_thread_register("HashMap_values", (int64_t(*)())HashMap_values);
    p_thread_register("HashMap_is_empty", (int64_t(*)())HashMap_is_empty);
    p_thread_register("str_is_numeric", (int64_t(*)())str_is_numeric);
    p_thread_register("str_equal", (int64_t(*)())str_equal);
    p_thread_register("Vec_take", (int64_t(*)())Vec_take);
    p_thread_register("Option_unwrap_or", (int64_t(*)())Option_unwrap_or);
    p_thread_register("Vec_last", (int64_t(*)())Vec_last);
    p_thread_register("AssocTypeDecl_new", (int64_t(*)())AssocTypeDecl_new);
    p_thread_register("str_bytes", (int64_t(*)())str_bytes);
    p_thread_register("str_slice", (int64_t(*)())str_slice);
    p_thread_register("FieldInit_new", (int64_t(*)())FieldInit_new);
    p_thread_register("String_char_at", (int64_t(*)())String_char_at);
    p_thread_register("str_trim", (int64_t(*)())str_trim);
    p_thread_register("Result_is_err", (int64_t(*)())Result_is_err);
    p_thread_register("abs", (int64_t(*)())p_abs);
    p_thread_register("AsmOperand_new", (int64_t(*)())AsmOperand_new);
    p_thread_register("HashMap_contains", (int64_t(*)())HashMap_contains);
    p_thread_register("Vec_zip", (int64_t(*)())Vec_zip);
    p_thread_register("str_trim_end", (int64_t(*)())str_trim_end);
    p_thread_register("assert_eq_bool", (int64_t(*)())assert_eq_bool);
    p_thread_register("EnumVariant_new", (int64_t(*)())EnumVariant_new);
    p_thread_register("String_ends_with", (int64_t(*)())String_ends_with);
    p_thread_register("str_is_alphanumeric", (int64_t(*)())str_is_alphanumeric);
    p_thread_register("println", (int64_t(*)())println);
    p_thread_register("str_rfind", (int64_t(*)())str_rfind);
    p_thread_register("vec_take", (int64_t(*)())vec_take);
    p_thread_register("create_allocator", (int64_t(*)())create_allocator);
    p_thread_register("char_to_lower", (int64_t(*)())char_to_lower);
    p_thread_register("vec_enumerate", (int64_t(*)())vec_enumerate);
    p_thread_register("ConstDef_new", (int64_t(*)())ConstDef_new);
    p_thread_register("Vec_capacity", (int64_t(*)())Vec_capacity);
    p_thread_register("vec_sort_by", (int64_t(*)())vec_sort_by);
    p_thread_register("AssocTypeBinding_new", (int64_t(*)())AssocTypeBinding_new);
    p_thread_register("type_to_str", (int64_t(*)())type_to_str);
    p_thread_register("String_len", (int64_t(*)())String_len);
    p_thread_register("str_count_char", (int64_t(*)())str_count_char);
    p_thread_register("char_to_str", (int64_t(*)())char_to_str);
    p_thread_register("StringBuilder_append_str", (int64_t(*)())StringBuilder_append_str);
    p_thread_register("Vec_extend", (int64_t(*)())Vec_extend);
    p_thread_register("str_starts_with", (int64_t(*)())str_starts_with);
    p_thread_register("ByteBuffer_clear", (int64_t(*)())ByteBuffer_clear);
    p_thread_register("vec_chunk", (int64_t(*)())vec_chunk);
    p_thread_register("str_reverse", (int64_t(*)())str_reverse);
    p_thread_register("String_bytes", (int64_t(*)())String_bytes);
    p_thread_register("StateMachineDef_new", (int64_t(*)())StateMachineDef_new);
    p_thread_register("Func_new", (int64_t(*)())Func_new);
    p_thread_register("str_byte_at", (int64_t(*)())str_byte_at);
    p_thread_register("str_trim_start", (int64_t(*)())str_trim_start);
    p_thread_register("str_to_lower", (int64_t(*)())str_to_lower);
    p_thread_register("str_repeat", (int64_t(*)())str_repeat);
    p_thread_register("ByteBuffer_is_empty", (int64_t(*)())ByteBuffer_is_empty);
    p_thread_register("p_str_length", (int64_t(*)())p_str_length);
    p_thread_register("Option_and_then", (int64_t(*)())Option_and_then);
    p_thread_register("HashMap_grow", (int64_t(*)())HashMap_grow);
    p_thread_register("String_rfind", (int64_t(*)())String_rfind);
    p_thread_register("Vec_remove", (int64_t(*)())Vec_remove);
    p_thread_register("HashMap_to_str", (int64_t(*)())HashMap_to_str);
    p_thread_register("str_count", (int64_t(*)())str_count);
    p_thread_register("str_concat", (int64_t(*)())str_concat);
    p_thread_register("Vec_dedup", (int64_t(*)())Vec_dedup);
    p_thread_register("Vec_new", (int64_t(*)())Vec_new);
    p_thread_register("Result_ok", (int64_t(*)())Result_ok);
    p_thread_register("String_is_empty", (int64_t(*)())String_is_empty);
    p_thread_register("Vec_pop", (int64_t(*)())Vec_pop);
    p_thread_register("StructDef_new", (int64_t(*)())StructDef_new);
    p_thread_register("StateTransition_new", (int64_t(*)())StateTransition_new);
    p_thread_register("String_clone", (int64_t(*)())String_clone);
    p_thread_register("char_to_upper", (int64_t(*)())char_to_upper);
    p_thread_register("SystemDef_new", (int64_t(*)())SystemDef_new);
    p_thread_register("str_pad_left", (int64_t(*)())str_pad_left);
    p_thread_register("allocator_alloc", (int64_t(*)())allocator_alloc);
    p_thread_register("int_to_str", (int64_t(*)())int_to_str);
    p_thread_register("Vec_insert", (int64_t(*)())Vec_insert);
    p_thread_register("ComponentDef_new", (int64_t(*)())ComponentDef_new);
    p_thread_register("str_join", (int64_t(*)())str_join);
    p_thread_register("p_str_equal", (int64_t(*)())p_str_equal);
    p_thread_register("HashMap_new", (int64_t(*)())HashMap_new);
    p_thread_register("MatchArm_new", (int64_t(*)())MatchArm_new);
    p_thread_register("str_contains", (int64_t(*)())str_contains);
    p_thread_register("Param_new", (int64_t(*)())Param_new);
    p_thread_register("char_is_lower", (int64_t(*)())char_is_lower);
    p_thread_register("str_center", (int64_t(*)())str_center);
    p_thread_register("Vec_skip", (int64_t(*)())Vec_skip);
    p_thread_register("int_to_bin", (int64_t(*)())int_to_bin);
    p_thread_register("ByteBuffer_write_byte", (int64_t(*)())ByteBuffer_write_byte);
    p_thread_register("stmt_line", (int64_t(*)())stmt_line);
    p_thread_register("str_parse_int", (int64_t(*)())str_parse_int);
    p_thread_register("String_slice", (int64_t(*)())String_slice);
    p_thread_register("str_len", (int64_t(*)())str_len);
    p_thread_register("HashMap_insert", (int64_t(*)())HashMap_insert);
    p_thread_register("Vec_chunk", (int64_t(*)())Vec_chunk);
    p_thread_register("ImplDef_new", (int64_t(*)())ImplDef_new);
    p_thread_register("char_is_digit", (int64_t(*)())char_is_digit);
    p_thread_register("byte_to_str", (int64_t(*)())byte_to_str);
    p_thread_register("int_to_hex", (int64_t(*)())int_to_hex);
    p_thread_register("Vec_all", (int64_t(*)())Vec_all);
    p_thread_register("program_new", (int64_t(*)())program_new);
    p_thread_register("shape_dim_to_str", (int64_t(*)())shape_dim_to_str);
    p_thread_register("ByteBuffer_len", (int64_t(*)())ByteBuffer_len);
    p_thread_register("HashMap_find_slot", (int64_t(*)())HashMap_find_slot);
    p_thread_register("StrPair_new", (int64_t(*)())StrPair_new);
    p_thread_register("str_split", (int64_t(*)())str_split);
    p_thread_register("op_tag", (int64_t(*)())op_tag);
    p_thread_register("Result_unwrap_or", (int64_t(*)())Result_unwrap_or);
    p_thread_register("vec_contains_val", (int64_t(*)())vec_contains_val);
    p_thread_register("Binding_new", (int64_t(*)())Binding_new);
    p_thread_register("str_from_bytes", (int64_t(*)())str_from_bytes);
    p_thread_register("String_parse_int", (int64_t(*)())String_parse_int);
    p_thread_register("str_find_nth", (int64_t(*)())str_find_nth);
    p_thread_register("str_strip_char", (int64_t(*)())str_strip_char);
    p_thread_register("ByteBuffer_read_byte", (int64_t(*)())ByteBuffer_read_byte);
    p_thread_register("LayerDef_new", (int64_t(*)())LayerDef_new);
    p_thread_register("Vec_map", (int64_t(*)())Vec_map);
    p_thread_register("LayerInst_new", (int64_t(*)())LayerInst_new);
    p_thread_register("bool_to_str", (int64_t(*)())bool_to_str);
    p_thread_register("vec_zip", (int64_t(*)())vec_zip);
    p_thread_register("ImportDef_new", (int64_t(*)())ImportDef_new);
    p_thread_register("str_is_blank", (int64_t(*)())str_is_blank);
    p_thread_register("vec_window", (int64_t(*)())vec_window);
    p_thread_register("allocator_free", (int64_t(*)())allocator_free);
    p_thread_register("String_byte_at", (int64_t(*)())String_byte_at);
    p_thread_register("assert_msg", (int64_t(*)())assert_msg);
    p_thread_register("str_remove_char", (int64_t(*)())str_remove_char);
    p_thread_register("Result_map", (int64_t(*)())Result_map);
    p_thread_register("str_parse_hex", (int64_t(*)())str_parse_hex);
    p_thread_register("HashMap_remove", (int64_t(*)())HashMap_remove);
    p_thread_register("Result_err", (int64_t(*)())Result_err);
    p_thread_register("ByteBuffer_to_str", (int64_t(*)())ByteBuffer_to_str);
    p_thread_register("Result_is_ok", (int64_t(*)())Result_is_ok);
    p_thread_register("Vec_get", (int64_t(*)())Vec_get);
    p_thread_register("Vec_is_empty", (int64_t(*)())Vec_is_empty);
    p_thread_register("EffectHandler_new", (int64_t(*)())EffectHandler_new);
    p_thread_register("String_push_str", (int64_t(*)())String_push_str);
    p_thread_register("char_is_upper", (int64_t(*)())char_is_upper);
    p_thread_register("StringBuilder_clear", (int64_t(*)())StringBuilder_clear);
    p_thread_register("p_str_hash", (int64_t(*)())p_str_hash);
    p_thread_register("p_str_compare", (int64_t(*)())p_str_compare);
    p_thread_register("StringBuilder_len", (int64_t(*)())StringBuilder_len);
    p_thread_register("min", (int64_t(*)())p_min);
    p_thread_register("TraitDef_new", (int64_t(*)())TraitDef_new);
    p_thread_register("char_is_whitespace", (int64_t(*)())char_is_whitespace);
    p_thread_register("Option_expect", (int64_t(*)())Option_expect);
    p_thread_register("Option_or_else", (int64_t(*)())Option_or_else);
    p_thread_register("HashMap_get_or", (int64_t(*)())HashMap_get_or);
    p_thread_register("String_from", (int64_t(*)())String_from);
    p_thread_register("Vec_clear", (int64_t(*)())Vec_clear);
    p_thread_register("HashMap_get", (int64_t(*)())HashMap_get);
    p_thread_register("HashMap_clear", (int64_t(*)())HashMap_clear);
    p_thread_register("str_split_char", (int64_t(*)())str_split_char);
    p_thread_register("Vec_contains", (int64_t(*)())Vec_contains);
    p_thread_register("HashMap_len", (int64_t(*)())HashMap_len);
    p_thread_register("StringBuilder_append_char", (int64_t(*)())StringBuilder_append_char);
    p_thread_register("String_repeat", (int64_t(*)())String_repeat);
    p_thread_register("Vec_filter", (int64_t(*)())Vec_filter);
    p_thread_register("Annotation_new", (int64_t(*)())Annotation_new);
    p_thread_register("clamp", (int64_t(*)())clamp);
    p_thread_register("str_compare", (int64_t(*)())str_compare);
    p_thread_register("Vec_first", (int64_t(*)())Vec_first);
    p_thread_register("Vec_for_each", (int64_t(*)())Vec_for_each);
    p_thread_register("Result_unwrap_or_else", (int64_t(*)())Result_unwrap_or_else);
    p_thread_register("count_expr_nodes", (int64_t(*)())count_expr_nodes);
    p_thread_register("Vec_to_str", (int64_t(*)())Vec_to_str);
    p_thread_register("String_replace", (int64_t(*)())String_replace);
    p_thread_register("str_capitalize", (int64_t(*)())str_capitalize);
    p_thread_register("String_starts_with", (int64_t(*)())String_starts_with);
    p_thread_register("str_parse_int_range", (int64_t(*)())str_parse_int_range);
    p_thread_register("String_to_lowercase", (int64_t(*)())String_to_lowercase);
    p_thread_register("ByteBuffer_set_byte", (int64_t(*)())ByteBuffer_set_byte);
    p_thread_register("TypeAlias_new", (int64_t(*)())TypeAlias_new);
    p_thread_register("StringBuilder_is_empty", (int64_t(*)())StringBuilder_is_empty);
    p_thread_register("Vec_push", (int64_t(*)())Vec_push);
    p_thread_register("str_join_ints", (int64_t(*)())str_join_ints);
    p_thread_register("HashMap_clone", (int64_t(*)())HashMap_clone);
    p_thread_register("ExternDecl_new", (int64_t(*)())ExternDecl_new);
    p_thread_register("Result_map_err", (int64_t(*)())Result_map_err);
    p_thread_register("p_str_from_ptr", (int64_t(*)())p_str_from_ptr);
    p_thread_register("str_to_upper", (int64_t(*)())str_to_upper);
    p_thread_register("Vec_contains_val", (int64_t(*)())Vec_contains_val);
    p_thread_register("char_is_alpha", (int64_t(*)())char_is_alpha);
    p_thread_register("Option_unwrap", (int64_t(*)())Option_unwrap);
    p_thread_register("StringBuilder_append_int", (int64_t(*)())StringBuilder_append_int);
    p_thread_register("vec_flat_map", (int64_t(*)())vec_flat_map);
    p_thread_register("TraitImpl_new", (int64_t(*)())TraitImpl_new);
    p_thread_register("StateDef_new", (int64_t(*)())StateDef_new);
    p_thread_register("str_pad_right", (int64_t(*)())str_pad_right);
    p_thread_register("Option_map", (int64_t(*)())Option_map);
    p_thread_register("ModelDef_new", (int64_t(*)())ModelDef_new);
    p_thread_register("Attribute_new", (int64_t(*)())Attribute_new);
    p_thread_register("int_to_str_padded", (int64_t(*)())int_to_str_padded);
    p_thread_register("EnumDef_new", (int64_t(*)())EnumDef_new);
    p_thread_register("vec_skip", (int64_t(*)())vec_skip);
    p_thread_register("Vec_find", (int64_t(*)())Vec_find);
    p_thread_register("Vec_position", (int64_t(*)())Vec_position);
    p_thread_register("String_to_string", (int64_t(*)())String_to_string);
    p_thread_register("str_is_empty", (int64_t(*)())str_is_empty);
    p_thread_register("Result_unwrap", (int64_t(*)())Result_unwrap);
    p_thread_register("Result_and_then", (int64_t(*)())Result_and_then);
    p_thread_register("str_find", (int64_t(*)())str_find);
    p_thread_register("assert", (int64_t(*)())assert);
    p_thread_register("str_ends_with", (int64_t(*)())str_ends_with);
    p_thread_register("SelectCase_new", (int64_t(*)())SelectCase_new);
    p_thread_register("HashMap_keys", (int64_t(*)())HashMap_keys);
    p_thread_register("assert_eq_i64", (int64_t(*)())assert_eq_i64);
    p_thread_register("str_is_alpha", (int64_t(*)())str_is_alpha);
    p_thread_register("p_str_copy_n", (int64_t(*)())p_str_copy_n);
    p_thread_register("String_to_uppercase", (int64_t(*)())String_to_uppercase);
    p_thread_register("StringBuilder_to_str", (int64_t(*)())StringBuilder_to_str);
    p_thread_register("ByteBuffer_new", (int64_t(*)())ByteBuffer_new);
    p_thread_register("ByteBuffer_from_string", (int64_t(*)())ByteBuffer_from_string);
    p_thread_register("str_parse_float", (int64_t(*)())str_parse_float);
    p_thread_register("EventHandlerDef_new", (int64_t(*)())EventHandlerDef_new);
    p_thread_register("Option_unwrap_or_else", (int64_t(*)())Option_unwrap_or_else);
    p_thread_register("Vec_truncate", (int64_t(*)())Vec_truncate);
    p_thread_register("Vec_flat_map", (int64_t(*)())Vec_flat_map);
    p_thread_register("StringBuilder_new", (int64_t(*)())StringBuilder_new);
    p_thread_register("Option_is_some", (int64_t(*)())Option_is_some);
    p_thread_register("String_trim", (int64_t(*)())String_trim);
    p_thread_register("str_replace", (int64_t(*)())str_replace);
    p_thread_register("Vec_sort", (int64_t(*)())Vec_sort);
    p_thread_register("Vec_set", (int64_t(*)())Vec_set);
    p_thread_register("Vec_len", (int64_t(*)())Vec_len);
    p_thread_register("TraitMethod_new", (int64_t(*)())TraitMethod_new);
    p_thread_register("Option_is_none", (int64_t(*)())Option_is_none);
    p_thread_register("Vec_sort_by", (int64_t(*)())Vec_sort_by);
    p_thread_register("box_type_to_str", (int64_t(*)())box_type_to_str);
    p_thread_register("String_contains", (int64_t(*)())String_contains);
    p_thread_register("allocator_bytes_used", (int64_t(*)())allocator_bytes_used);
    p_thread_register("Vec_window", (int64_t(*)())Vec_window);
    p_thread_register("Vec_enumerate", (int64_t(*)())Vec_enumerate);
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
void p_print_float(double val) {{ char* _s = p_to_str_float(val); printf("%s", _s); free(_s); }}
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
void* p_malloc(int64_t size) { if (size < 0) return NULL; return malloc((size_t)size); }
void p_free(void* ptr) { free(ptr); }
char* p_file_read(const char* path) {{
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f);
    if (sz < 0) {{ fclose(f); return NULL; }}
    fseek(f, 0, SEEK_SET);
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
int64_t p_file_size(const char* path) {{ FILE* f = fopen(path, "rb"); if (!f) return -1; fseek(f, 0, SEEK_END); int64_t sz = ftell(f); fclose(f); return sz < 0 ? -1 : sz; }}
char* p_getenv(const char* name) {{ return getenv(name); }}
int64_t p_argc(void) {{ return (int64_t)__p_argc; }}
char* p_argv(int64_t index) {{ if (index < 0 || index >= __p_argc) return NULL; return __p_argv[index]; }}
int64_t p_system(const char* cmd) {{ return (int64_t)system(cmd); }}
void p_exit(int64_t code) {{ exit((int)code); }}
#if defined(__linux__) || defined(__APPLE__)
#include <time.h>
int64_t p_clock_now(void) {{ struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts); return (int64_t)ts.tv_sec * 1000000000LL + (int64_t)ts.tv_nsec; }}
int64_t p_clock_now_us(void) {{ struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts); return (int64_t)ts.tv_sec * 1000000LL + (int64_t)ts.tv_nsec / 1000; }}
int64_t p_clock_now_ms(void) {{ struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts); return (int64_t)ts.tv_sec * 1000LL + (int64_t)ts.tv_nsec / 1000000; }}
#else
#include <time.h>
int64_t p_clock_now(void) {{ return (int64_t)time(NULL) * 1000000000LL; }}
int64_t p_clock_now_us(void) {{ return (int64_t)time(NULL) * 1000000LL; }}
int64_t p_clock_now_ms(void) {{ return (int64_t)time(NULL) * 1000LL; }}
#endif
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
int64_t p_shared_alloc(int64_t size) { if (size < 0) return 0; return (int64_t)(intptr_t)malloc((size_t)size); }
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
int64_t p_tls_create(void) {{ pthread_key_t key; pthread_key_create(&key, NULL); return (int64_t)key; }}
int64_t p_tls_get(int64_t key) {{ return (int64_t)(intptr_t)pthread_getspecific((pthread_key_t)key); }}
void p_tls_set(int64_t key, int64_t value) {{ pthread_setspecific((pthread_key_t)key, (void*)(intptr_t)value); }}
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
    char* buf = (char*)malloc(512);
    // Match Rust's {{}} format: shortest representation that round-trips.
    // Rust's Display for f64 always uses fixed-point (never scientific notation).
    if (val == 0.0) {{ snprintf(buf, 512, "0"); return buf; }}
    // Calculate required max precision from exponent.
    // For 2.22e-16, we need prec=32 (16 zeros + 16 sig digits).
    int exp = 0;
    double absval = fabs(val);
    if (absval >= 1.0) {{ while (absval >= 10.0) {{ absval /= 10.0; exp++; }} }}
    else {{ while (absval < 1.0) {{ absval *= 10.0; exp--; }} }}
    int max_prec = 17 + (exp < 0 ? -exp : 0);
    if (max_prec > 350) max_prec = 350;
    // Try increasing precision with %f format (no scientific notation)
    for (int prec = 0; prec <= max_prec; prec++) {{
        snprintf(buf, 512, "%.*f", prec, val);
        double parsed = strtod(buf, NULL);
        if (parsed == val) {{
            // Trim trailing zeros after decimal point (e.g., "10.0" -> "10")
            char* dot = strchr(buf, '.');
            if (dot) {{
                char* end = buf + strlen(buf) - 1;
                while (end > dot && *end == '0') *end-- = '\0';
                if (end == dot) *end = '\0'; /* remove trailing dot */
            }}
            return buf;
        }}
    }}
    // If no precision round-trips, use max precision
    snprintf(buf, 512, "%.*f", max_prec, val);
    return buf;
}}
char* p_float_to_str(double val) {{ return p_to_str_float(val); }}
char* p_bool_to_str(bool val) {{ return p_to_str_bool(val); }}
char* p_vec_to_str(PVec* v) {{ return p_to_str_vec(v); }}
double p_str_to_float(const char* s) {{ if (!s) return 0.0; return atof(s); }}

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
// PTensor typedef already emitted in header section
typedef enum { AG_ADD, AG_SUB, AG_MUL, AG_MATMUL, AG_SCALE, AG_RELU, AG_SIGMOID, AG_SUM, AG_MEAN, AG_NEG } AGOpKind;
typedef struct {
    AGOpKind op;
    void* result;  void* a;  void* b;  double scalar;
    int64_t a_shape[8]; int64_t a_ndim;
    int64_t b_shape[8]; int64_t b_ndim;
    double* stored_data; int64_t stored_len; int is_checkpoint;
} AGEntry;
static AGEntry ag_tape[256];
static int64_t ag_tape_len = 0;
static int ag_mode = 0;
static int ag_checkpoint_mode = 0;
static void* ag_grad_ptrs[256];
static PTensor* ag_grads[256];
static int64_t ag_grad_len = 0;
static double ag_dp_budget = 0.0;
static double ag_dp_limit = 0.0;
static int ag_tp_rank = 0;
static int ag_tp_world_size = 1;
static PTensor* ag_get_grad(void* ptr) {{
    for (int64_t i = 0; i < ag_grad_len; i++) if (ag_grad_ptrs[i] == ptr) return ag_grads[i];
    return NULL;
}}
static void ag_accum_grad(void* ptr, PTensor* grad) {{
    PTensor* ex = ag_get_grad(ptr);
    if (ex) {{ for (int64_t i = 0; i < ex->len && i < grad->len; i++) ex->data[i] += grad->data[i]; }}
    else {{ if (ag_grad_len < 256) {{ ag_grad_ptrs[ag_grad_len] = ptr; ag_grads[ag_grad_len] = grad; ag_grad_len++; }} }}
}}
static PTensor* ag_clone_tensor(PTensor* t) {{
    if (!t) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = t->ndim; r->len = t->len;
    for (int64_t i = 0; i < t->ndim && i < 8; i++) r->shape[i] = t->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) r->data[i] = t->data[i];
    return r;
}}
static PTensor* ag_zeros_like(PTensor* t) {{
    if (!t) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = t->ndim; r->len = t->len;
    for (int64_t i = 0; i < t->ndim && i < 8; i++) r->shape[i] = t->shape[i];
    r->data = (double*)calloc(r->len, sizeof(double));
    return r;
}}
static PTensor* ag_filled_shape(int64_t* shape, int64_t ndim, double val) {{
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = ndim; int64_t len = 1;
    for (int64_t i = 0; i < ndim && i < 8; i++) {{ r->shape[i] = shape[i]; len *= shape[i]; }}
    r->len = len; r->data = (double*)malloc(len * sizeof(double));
    for (int64_t i = 0; i < len; i++) r->data[i] = val;
    return r;
}}
static void ag_record(AGOpKind op, void* result, void* a, void* b, double s) {{
    if (!ag_mode || ag_tape_len >= 256) return;
    AGEntry* e = &ag_tape[ag_tape_len++];
    e->op = op; e->result = result; e->a = a; e->b = b; e->scalar = s;
    e->stored_data = NULL; e->stored_len = 0; e->is_checkpoint = 0;
    PTensor* ta = (PTensor*)a;
    if (ta) {{ e->a_ndim = ta->ndim; for (int64_t i = 0; i < ta->ndim && i < 8; i++) e->a_shape[i] = ta->shape[i]; }} else e->a_ndim = 0;
    PTensor* tb = (PTensor*)b;
    if (tb) {{ e->b_ndim = tb->ndim; for (int64_t i = 0; i < tb->ndim && i < 8; i++) e->b_shape[i] = tb->shape[i]; }} else e->b_ndim = 0;
}}
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
    ag_record(AG_ADD, r, a, b, 0);
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
    ag_record(AG_MUL, r, a, b, 0);
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
    ag_record(AG_MATMUL, r, a, b, 0);
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
    if (ag_mode && ag_tape_len < 256) {{
        AGEntry* e = &ag_tape[ag_tape_len++];
        e->op = AG_RELU; e->result = r; e->a = t; e->b = NULL; e->scalar = 0;
        e->a_ndim = pt->ndim; for (int64_t i = 0; i < pt->ndim && i < 8; i++) e->a_shape[i] = pt->shape[i];
        e->b_ndim = 0;
        if (ag_checkpoint_mode) {{ e->is_checkpoint = 1; e->stored_len = pt->len; e->stored_data = (double*)malloc(pt->len * sizeof(double)); for (int64_t i = 0; i < pt->len; i++) e->stored_data[i] = pt->data[i]; }}
        else {{ e->is_checkpoint = 0; e->stored_len = r->len; e->stored_data = (double*)malloc(r->len * sizeof(double)); for (int64_t i = 0; i < r->len; i++) e->stored_data[i] = r->data[i]; }}
    }}
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
    if (ag_mode && ag_tape_len < 256) {{
        AGEntry* e = &ag_tape[ag_tape_len++];
        e->op = AG_SIGMOID; e->result = r; e->a = t; e->b = NULL; e->scalar = 0;
        e->a_ndim = pt->ndim; for (int64_t i = 0; i < pt->ndim && i < 8; i++) e->a_shape[i] = pt->shape[i];
        e->b_ndim = 0;
        if (ag_checkpoint_mode) {{ e->is_checkpoint = 1; e->stored_len = pt->len; e->stored_data = (double*)malloc(pt->len * sizeof(double)); for (int64_t i = 0; i < pt->len; i++) e->stored_data[i] = pt->data[i]; }}
        else {{ e->is_checkpoint = 0; e->stored_len = r->len; e->stored_data = (double*)malloc(r->len * sizeof(double)); for (int64_t i = 0; i < r->len; i++) e->stored_data[i] = r->data[i]; }}
    }}
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
    ag_record(AG_SCALE, r, t, NULL, s);
    return r;
}}
double p_tensor_sum(void* t) {{ PTensor* pt = (PTensor*)t; if (!pt) return 0; double s=0; for (int64_t i=0;i<pt->len;i++) s+=pt->data[i]; ag_record(AG_SUM, (void*)pt, t, NULL, 0); return s; }}
double p_tensor_mean(void* t) {{ PTensor* pt = (PTensor*)t; if (!pt || pt->len==0) return 0; double r=p_tensor_sum(t)/pt->len; if (ag_mode && ag_tape_len < 256) {{ AGEntry* e=&ag_tape[ag_tape_len-1]; e->op=AG_MEAN; e->scalar=(double)pt->len; }} return r; }}
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
            char buf[32]; double _v=pt->data[i]; if (_v==floor(_v)&&isfinite(_v)&&fabs(_v)<1e15) snprintf(buf,sizeof(buf),"%lld",(long long)_v); else snprintf(buf,sizeof(buf),"%.4f",_v); size_t bl=strlen(buf);
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
            char buf[32]; double _v=pt->data[r*cols+c]; if (_v==floor(_v)&&isfinite(_v)&&fabs(_v)<1e15) snprintf(buf,sizeof(buf),"%lld",(long long)_v); else snprintf(buf,sizeof(buf),"%.4f",_v); size_t bl=strlen(buf);
            if (pos+bl+1>=cap){{cap=(cap+bl)*2;result=(char*)realloc(result,cap);}}
            memcpy(result+pos,buf,bl); pos+=bl;
        }}
        if (pos+1>=cap){{cap+=2;result=(char*)realloc(result,cap);}} result[pos++]=']';
    }}
    if (pos+2>=cap){{cap+=2;result=(char*)realloc(result,cap);}}
    result[pos++]=']'; result[pos]=0;
    return result;
}}

void p_autograd_begin(void) {{
    ag_mode = 1; ag_tape_len = 0; ag_grad_len = 0;
}}
void p_autograd_end(void) {{
    ag_mode = 0;
}}
void p_autograd_backward(void* loss) {{
    (void)loss; // seed is always 1.0 (d(loss)/d(loss) = 1)
    double seed = 1.0;
    // Find the last scalar-producing op (sum/mean) to seed
    int64_t seeded_idx = -1;
    for (int64_t i = ag_tape_len - 1; i >= 0; i--) {{
        if (ag_tape[i].op == AG_SUM) {{
            PTensor* g = ag_filled_shape(ag_tape[i].a_shape, ag_tape[i].a_ndim, seed);
            ag_accum_grad(ag_tape[i].a, g); seeded_idx = i; break;
        }}
        if (ag_tape[i].op == AG_MEAN) {{
            PTensor* g = ag_filled_shape(ag_tape[i].a_shape, ag_tape[i].a_ndim, seed / ag_tape[i].scalar);
            ag_accum_grad(ag_tape[i].a, g); seeded_idx = i; break;
        }}
    }}
    if (seeded_idx < 0 && ag_tape_len > 0) {{
        PTensor* g = (PTensor*)malloc(sizeof(PTensor)); g->ndim=1; g->len=1; g->shape[0]=1; g->data=(double*)malloc(sizeof(double)); g->data[0]=seed;
        ag_accum_grad(ag_tape[ag_tape_len-1].result, g);
    }}
    // Walk tape in reverse
    for (int64_t i = ag_tape_len - 1; i >= 0; i--) {{
        if (i == seeded_idx) continue; // skip already-seeded op
        AGEntry* e = &ag_tape[i];
        PTensor* grad_out = ag_get_grad(e->result);
        if (!grad_out) continue;
        switch (e->op) {{
            case AG_ADD: {{
                ag_accum_grad(e->a, ag_clone_tensor(grad_out));
                ag_accum_grad(e->b, ag_clone_tensor(grad_out));
                break;
            }}
            case AG_SUB: {{
                ag_accum_grad(e->a, ag_clone_tensor(grad_out));
                PTensor* ng = ag_clone_tensor(grad_out); for (int64_t j=0;j<ng->len;j++) ng->data[j]=-ng->data[j];
                ag_accum_grad(e->b, ng);
                break;
            }}
            case AG_MUL: {{
                ag_accum_grad(e->a, ag_clone_tensor(grad_out));
                ag_accum_grad(e->b, ag_clone_tensor(grad_out));
                break;
            }}
            case AG_MATMUL: {{
                PTensor* ag = ag_filled_shape(e->a_shape, e->a_ndim, 1.0);
                PTensor* bg = ag_filled_shape(e->b_shape, e->b_ndim, 1.0);
                ag_accum_grad(e->a, ag); ag_accum_grad(e->b, bg);
                break;
            }}
            case AG_SCALE: {{
                PTensor* ga = ag_clone_tensor(grad_out); for (int64_t j=0;j<ga->len;j++) ga->data[j]*=e->scalar;
                ag_accum_grad(e->a, ga);
                break;
            }}
            case AG_RELU: {{
                // Recompute mask from stored data
                PTensor* ga = ag_clone_tensor(grad_out);
                if (e->stored_data) {{
                    for (int64_t j=0;j<ga->len && j<e->stored_len;j++) {{
                        double val = e->is_checkpoint ? (e->stored_data[j] > 0 ? e->stored_data[j] : 0) : e->stored_data[j];
                        ga->data[j] *= (val > 0) ? 1.0 : 0.0;
                    }}
                }}
                ag_accum_grad(e->a, ga);
                break;
            }}
            case AG_SIGMOID: {{
                PTensor* ga = ag_clone_tensor(grad_out);
                if (e->stored_data) {{
                    for (int64_t j=0;j<ga->len && j<e->stored_len;j++) {{
                        double s = e->is_checkpoint ? (1.0/(1.0+exp(-e->stored_data[j]))) : e->stored_data[j];
                        ga->data[j] *= s * (1.0 - s);
                    }}
                }}
                ag_accum_grad(e->a, ga);
                break;
            }}
            case AG_SUM: {{
                PTensor* g = ag_filled_shape(e->a_shape, e->a_ndim, seed);
                ag_accum_grad(e->a, g);
                break;
            }}
            case AG_MEAN: {{
                PTensor* g = ag_filled_shape(e->a_shape, e->a_ndim, seed / e->scalar);
                ag_accum_grad(e->a, g);
                break;
            }}
            case AG_NEG: {{
                PTensor* ng = ag_clone_tensor(grad_out); for (int64_t j=0;j<ng->len;j++) ng->data[j]=-ng->data[j];
                ag_accum_grad(e->a, ng);
                break;
            }}
        }}
    }}
}}
void* p_grad_get(void* t) {{
    PTensor* g = ag_get_grad(t);
    if (g) return g;
    // No gradient — return zero tensor of same shape
    PTensor* pt = (PTensor*)t;
    if (!pt) return NULL;
    return (void*)ag_zeros_like(pt);
}}
void p_grad_zero(void) {{
    ag_grad_len = 0;
}}
void p_checkpoint_begin(void) {{ ag_checkpoint_mode = 1; }}
void p_checkpoint_end(void) {{ ag_checkpoint_mode = 0; }}
void p_dp_budget_reset(double epsilon) {{ ag_dp_budget = 0.0; ag_dp_limit = epsilon; }}
void p_dp_budget_check(double epsilon, double limit) {{
    ag_dp_budget += epsilon;
    if (ag_dp_budget > limit) {{ fprintf(stderr, "DP budget exhausted: consumed %g > limit %g\n", ag_dp_budget, limit); exit(1); }}
}}
void* p_dp_noise(void* t, double sigma) {{
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < pt->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < r->len; i++) {{
        double val = pt->data[i];
        double u1 = 0.0001 + fabs(fmod(val * 1000.0, 1.0)) * 0.9998;
        double u2 = 0.0001 + fabs(fmod(sin(val) * 1000.0, 1.0)) * 0.9998;
        double z = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
        r->data[i] = val + sigma * z;
    }}
    return r;
}}
void* p_gaussian_noise(void* shape_vec, double sigma) {{
    // shape_vec is a Fides Vec (i64 pointer to 24-byte block)
    int64_t* sv = (int64_t*)shape_vec;
    int64_t ndim = sv ? sv[0] : 0;
    int64_t* data_ptr = sv ? (int64_t*)sv[1] : NULL;
    int64_t shape[8]; int64_t len = 1;
    for (int64_t i = 0; i < ndim && i < 8; i++) {{ shape[i] = data_ptr ? data_ptr[i] : 1; len *= shape[i]; }}
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = ndim; r->len = len;
    for (int64_t i = 0; i < ndim && i < 8; i++) r->shape[i] = shape[i];
    r->data = (double*)malloc(len * sizeof(double));
    for (int64_t i = 0; i < len; i++) {{
        double seed = (double)i * 0.1;
        double u1 = 0.0001 + fabs(sin(seed)) * 0.9998;
        double u2 = 0.0001 + fabs(cos(seed)) * 0.9998;
        double z = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
        r->data[i] = sigma * z;
    }}
    return r;
}}
void* p_dropout(void* t, double drop_prob) {{
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    PTensor* r = ag_clone_tensor(pt);
    double scale = 1.0 / (1.0 - drop_prob);
    for (int64_t i = 0; i < r->len; i++) {{
        double s = fabs(sin((double)i * 7.0));
        if (s < drop_prob) r->data[i] = 0.0; else r->data[i] *= scale;
    }}
    return r;
}}
int64_t p_tp_rank(void) {{ return ag_tp_rank; }}
int64_t p_tp_world_size(void) {{ return ag_tp_world_size; }}
void* p_all_reduce(void* t) {{
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    return (void*)ag_clone_tensor(pt);
}}
void* p_broadcast(void* t) {{
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    return (void*)ag_clone_tensor(pt);
}}
void* p_all_gather(void* t) {{
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    return (void*)ag_clone_tensor(pt);
}}
void* p_scatter(void* t) {{
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    return (void*)ag_clone_tensor(pt);
}}
void p_send_tensor(void* t) {{ (void)t; /* no-op in single-rank */ }}
void* p_recv_tensor(void* shape_vec) {{
    int64_t* sv = (int64_t*)shape_vec;
    int64_t ndim = sv ? sv[0] : 0;
    int64_t* data_ptr = sv ? (int64_t*)sv[1] : NULL;
    int64_t shape[8]; int64_t len = 1;
    for (int64_t i = 0; i < ndim && i < 8; i++) {{ shape[i] = data_ptr ? data_ptr[i] : 1; len *= shape[i]; }}
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = ndim; r->len = len;
    for (int64_t i = 0; i < ndim && i < 8; i++) r->shape[i] = shape[i];
    r->data = (double*)calloc(len, sizeof(double));
    return r;
}}

typedef struct { int64_t seq_len; PTensor* k_data; PTensor* v_data; } KVLayer;
typedef struct { int64_t num_layers; int64_t max_seq_len; int64_t window_size; int64_t head_dim; KVLayer* layers; } KVCache;
static KVCache* kv_caches[16];
static int64_t kv_cache_count = 0;
int64_t p_kv_cache_new(int64_t num_layers, int64_t max_seq_len, int64_t window_size, int64_t head_dim) {{
    (void)max_seq_len;
    if (kv_cache_count >= 16) return -1;
    KVCache* c = (KVCache*)malloc(sizeof(KVCache));
    c->num_layers = num_layers; c->max_seq_len = max_seq_len;
    c->window_size = window_size; c->head_dim = head_dim;
    c->layers = (KVLayer*)calloc(num_layers, sizeof(KVLayer));
    for (int64_t i = 0; i < num_layers; i++) {{ c->layers[i].seq_len = 0; c->layers[i].k_data = NULL; c->layers[i].v_data = NULL; }}
    int64_t id = kv_cache_count; kv_caches[kv_cache_count++] = c;
    return id;
}}
void p_kv_cache_append(int64_t cache_id, int64_t layer, void* k, void* v) {{
    if (cache_id < 0 || cache_id >= kv_cache_count) return;
    KVCache* c = kv_caches[cache_id];
    if (layer < 0 || layer >= c->num_layers) return;
    PTensor* kt = (PTensor*)k; PTensor* vt = (PTensor*)v;
    if (!kt || !vt) return;
    int64_t new_tokens = kt->shape[0];
    int64_t new_len = c->layers[layer].seq_len + new_tokens;
    if (c->window_size > 0 && new_len > c->window_size) {{
        int64_t evict = new_len - c->window_size;
        if (c->layers[layer].k_data) {{
            int64_t remain = c->layers[layer].seq_len - evict;
            if (remain > 0) {{ memmove(c->layers[layer].k_data->data, c->layers[layer].k_data->data + evict * c->head_dim, remain * c->head_dim * sizeof(double)); memmove(c->layers[layer].v_data->data, c->layers[layer].v_data->data + evict * c->head_dim, remain * c->head_dim * sizeof(double)); }}
            c->layers[layer].seq_len = remain;
        }} else c->layers[layer].seq_len = 0;
        new_len = c->layers[layer].seq_len + new_tokens;
    }}
    int64_t total = new_len * c->head_dim;
    if (!c->layers[layer].k_data) {{
        c->layers[layer].k_data = (PTensor*)malloc(sizeof(PTensor)); c->layers[layer].k_data->ndim = 2; c->layers[layer].k_data->shape[0] = new_len; c->layers[layer].k_data->shape[1] = c->head_dim; c->layers[layer].k_data->len = total; c->layers[layer].k_data->data = (double*)malloc(total * sizeof(double));
        c->layers[layer].v_data = (PTensor*)malloc(sizeof(PTensor)); c->layers[layer].v_data->ndim = 2; c->layers[layer].v_data->shape[0] = new_len; c->layers[layer].v_data->shape[1] = c->head_dim; c->layers[layer].v_data->len = total; c->layers[layer].v_data->data = (double*)malloc(total * sizeof(double));
    }} else {{
        c->layers[layer].k_data->data = (double*)realloc(c->layers[layer].k_data->data, total * sizeof(double)); c->layers[layer].k_data->len = total; c->layers[layer].k_data->shape[0] = new_len;
        c->layers[layer].v_data->data = (double*)realloc(c->layers[layer].v_data->data, total * sizeof(double)); c->layers[layer].v_data->len = total; c->layers[layer].v_data->shape[0] = new_len;
    }}
    int64_t offset = c->layers[layer].seq_len * c->head_dim;
    for (int64_t i = 0; i < kt->len; i++) c->layers[layer].k_data->data[offset + i] = kt->data[i];
    for (int64_t i = 0; i < vt->len; i++) c->layers[layer].v_data->data[offset + i] = vt->data[i];
    c->layers[layer].seq_len = new_len;
}}
void* p_kv_cache_get(int64_t cache_id, int64_t layer) {{
    if (cache_id < 0 || cache_id >= kv_cache_count) return NULL;
    KVCache* c = kv_caches[cache_id];
    if (layer < 0 || layer >= c->num_layers) return NULL;
    if (!c->layers[layer].k_data) return NULL;
    int64_t sl = c->layers[layer].seq_len; int64_t hd = c->head_dim;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = 2; r->shape[0] = sl; r->shape[1] = hd * 2; r->len = sl * hd * 2;
    r->data = (double*)malloc(r->len * sizeof(double));
    for (int64_t i = 0; i < sl * hd; i++) {{ r->data[i] = c->layers[layer].k_data->data[i]; r->data[sl * hd + i] = c->layers[layer].v_data->data[i]; }}
    return r;
}}
int64_t p_kv_cache_len(int64_t cache_id, int64_t layer) {{
    if (cache_id < 0 || cache_id >= kv_cache_count) return 0;
    KVCache* c = kv_caches[cache_id];
    if (layer < 0 || layer >= c->num_layers) return 0;
    return c->layers[layer].seq_len;
}}
void p_kv_cache_clear(int64_t cache_id) {{
    if (cache_id < 0 || cache_id >= kv_cache_count) return;
    KVCache* c = kv_caches[cache_id];
    for (int64_t i = 0; i < c->num_layers; i++) c->layers[i].seq_len = 0;
}}
void p_kv_cache_evict(int64_t cache_id, int64_t num_tokens) {{
    if (cache_id < 0 || cache_id >= kv_cache_count) return;
    KVCache* c = kv_caches[cache_id];
    for (int64_t i = 0; i < c->num_layers; i++) {{
        if (c->layers[i].seq_len > num_tokens) c->layers[i].seq_len -= num_tokens;
        else c->layers[i].seq_len = 0;
    }}
}}

void* p_quantize_q8(void* t, double scale, int64_t zero_point) {{
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < pt->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(pt->len * sizeof(double));
    for (int64_t i = 0; i < pt->len; i++) {{ double q = round(pt->data[i] / scale) + zero_point; if (q < -128) q = -128; if (q > 127) q = 127; r->data[i] = q; }}
    return r;
}}
void* p_dequantize_q8(void* t, double scale, int64_t zero_point) {{
    PTensor* pt = (PTensor*)t; if (!pt) return NULL;
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = pt->ndim; r->len = pt->len;
    for (int64_t i = 0; i < pt->ndim && i < 8; i++) r->shape[i] = pt->shape[i];
    r->data = (double*)malloc(pt->len * sizeof(double));
    for (int64_t i = 0; i < pt->len; i++) r->data[i] = (pt->data[i] - zero_point) * scale;
    return r;
}}
void* p_qmatmul(void* a, void* b) {{
    PTensor* ta = (PTensor*)a; PTensor* tb = (PTensor*)b;
    if (!ta || !tb) return NULL;
    int64_t m = ta->shape[0], k = ta->shape[1], n = tb->shape[1];
    PTensor* r = (PTensor*)malloc(sizeof(PTensor));
    r->ndim = 2; r->shape[0] = m; r->shape[1] = n; r->len = m * n;
    r->data = (double*)calloc(m * n, sizeof(double));
    for (int64_t i = 0; i < m; i++) for (int64_t j = 0; j < n; j++) {{ double s = 0; for (int64_t x = 0; x < k; x++) s += ta->data[i*k+x] * tb->data[x*n+j]; r->data[i*n+j] = s; }}
    return r;
}}
char* p_find_quant_params(void* t) {{
    PTensor* pt = (PTensor*)t; if (!pt || pt->len == 0) return strdup("1,0");
    double mn = pt->data[0], mx = pt->data[0];
    for (int64_t i = 1; i < pt->len; i++) {{ if (pt->data[i] < mn) mn = pt->data[i]; if (pt->data[i] > mx) mx = pt->data[i]; }}
    double abs_max = fabs(mn) > fabs(mx) ? fabs(mn) : fabs(mx);
    double scale = abs_max > 0.0 ? abs_max / 127.0 : 1.0;
    int64_t zp = 0;
    char* r = (char*)malloc(64); snprintf(r, 64, "%.17g,%lld", scale, (long long)zp); return r;
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
